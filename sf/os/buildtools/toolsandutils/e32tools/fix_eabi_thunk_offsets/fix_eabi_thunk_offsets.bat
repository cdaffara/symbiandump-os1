:: Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
:: All rights reserved.
:: This component and the accompanying materials are made available
:: under the terms of "Eclipse Public License v1.0"
:: which accompanies this distribution, and is available
:: at the URL "http://www.eclipse.org/legal/epl-v10.html".
::
:: Initial Contributors:
:: Nokia Corporation - initial contribution.
::
:: Contributors:
::
:: Description:
::

@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S "%0" %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
goto endofperl
@rem ';
#!perl
#line 28

use strict;
use Getopt::Long;

my $toolVersion = "1.0";

my $update = 0;
my $use_perforce = 0;
my $verbose = 0;
my $defFile;

# 1. Check arguments, output help etc.

GetOptions (
	'update' => \$update,			# modify the files
	'perforce' => \$use_perforce,	# apply "p4 edit" to changed files
	'v+' => \$verbose,				# print extra diagnostic info
	'match=s' => \$defFile			# only process DEF files matching a pattern
	);

if (@ARGV == 0)
	{
	print STDERR "\nfix_eabi_thunk_offsets.bat - Version $toolVersion\n";

	print STDERR << 'END_OF_HELP';

Usage: fix_eabi_think_offsets [-update] [-perforce] [-match exp] build_log ... 

Parse the output from one or more build logs, extracting MAKEDEF errors and
warnings which relate to EABI virtual function override thunks. Using this
information, prepare modified DEF files in which each "missing" export is 
replaced by a corresponding "unfrozen" export.

-update     Overwrite the existing .def files with the modified versions
-perforce   Apply "p4 edit" to each of the modified .def files
-match exp  Process only .def files with names that contain "\exp"

NOTE: The tool assumes that the original build source layout is replicated on 
the drive where it is being executed.

Build logs will sometimes contain corrupted warning messages, in which case
the tool will probably report that there is nothing to replace some missing
symbol. It may help to edit the log file and try again: it is always safe to 
run this tool more than once on the same log file.

END_OF_HELP

	exit(1);
	}

my $parseWarnings = 1;
my $parseErrors = 1;


# 2. Parse the build logs, extracting the Makedef warnings & errors

my $line;
my $header;
my $parseWarning = 0;
my $parseError = 0;
my $variant;
my $component;
my $sourceDefFile;
my @errorOutput;
my @warningOutput;

my %DefFiles;
my %TempDefFiles;

sub newDefFile($)
	{
	my ($defFile) = @_;
	if (!defined $DefFiles{$defFile})
		{
		@{$DefFiles{$defFile}} = \();
		}
	}

while ($line = <>)
	{
	if ($line =~ /^Chdir /)
		{
		$component = $line;
		$component =~ s/^Chdir //;
		$component =~ s/\s//g;
		next;
		}
		
	if (($line =~ /^  make/) && ($line =~ / CFG\=/))
		{
		$variant = $line;
		$variant =~ s/^.*CFG\=//;
		$variant =~ s/ .*$//;
		$variant =~ s/\s//g;
		next;
		}

	if ($parseWarnings && ($line =~ /MAKEDEF WARNING:/))
		{
		$parseWarning =  1;
		$parseError = 0;
		$header = $line;
		next;		
		}
		
	if ($parseErrors && ($line =~ /MAKEDEF ERROR:/))
		{
		$parseWarning =  0;
		$parseError = 1;
		$header = $line;
		next;		
		}

	if ($line !~ /^  /)
		{
		$parseWarning = 0;
		$parseError = 0;
		next;
		}

	if ($parseWarning)
		{
		if ($header)
			{
			if ($defFile && ($header !~ /\\$defFile/i))
				{
				$parseWarning = 0;
				$parseError = 0;
				next;
				}
			
			$sourceDefFile = $header;
			$sourceDefFile =~ s/^.*not yet Frozen in//;
			$sourceDefFile =~ s/://;
			$sourceDefFile =~ s/\s//g;
			
			push @warningOutput, "--\n$sourceDefFile ($variant)\n$component\n$header";
			newDefFile($sourceDefFile);
			$header = "";
			}

		next if ($line =~ /\*\*\*/);
		if ($line =~ /^  (\S.*}\.def)(\(\d+\) : \S+.*)$/)
			{
			push @{$DefFiles{$sourceDefFile}}, "W$2";
			$TempDefFiles{$1} = $sourceDefFile;
			}
		push @warningOutput, $line;
		
		next;		
		}

	if ($parseError)
		{
		if ($defFile && ($line !~ /\\$defFile/i))
			{
			$parseWarning = 0;
			$parseError = 0;
			next;
			}
			
		if ($header)
			{
			$sourceDefFile = $line;
			$sourceDefFile =~ s/\(.*$//;
			$sourceDefFile =~ s/\s//g;

			push @errorOutput, "--\n$sourceDefFile ($variant)\n$component\n$header";
			newDefFile($sourceDefFile);
			$header = "";
			}

		next if ($line =~ /\*\*\*/);
		if ($line =~ /(\(\d+\) : \S+.*)$/)
			{
			push @{$DefFiles{$sourceDefFile}}, "E$1";
			}
		push @errorOutput, $line;
		
		next;
		}

	# Catch a orphaned warning line...
	
	if ($line =~ /^  (\S.*}\.def)(\(\d+\) : \S+.*)$/)
		{
		my $tempDefFile = $1;
		my $newline = $2;
		
		next if ($defFile && ($tempDefFile !~ /\\$defFile/i));

		my $sourceDefFile = $TempDefFiles{$tempDefFile};
		push @{$DefFiles{$sourceDefFile}}, "W$newline";
		push @warningOutput, $line;
		}

	}

close BUILD_LOG;

# 3. Process the information for each DEF file

my %Classes;
my @DefFileList;

foreach my $def (sort keys %DefFiles)
	{
	my @replacements;
	my @errors;
	my @warnings;
	my $problems = 0;
	
	print "\n----\n$def\n";
	if ($verbose > 1)
		{
		print "Information extracted from Makedef warnings and errors:\n";
		# printed inside the following loop...
		}

	# Process into lists of errors and warnings which can be sorted
	
	my $previousline = "";
	foreach $line (sort @{$DefFiles{$def}})
		{
		next if ($line eq $previousline);	# skip duplicates
		$previousline = $line;
		print "\t$line\n" if ($verbose > 1);
			
		if ($line =~ /^(.)\((\d+)\) : (((_ZTh|_ZTv)([n0-9_]+)_(NK?(\d+)(\S+)))\s.*)$/)
			{
			my $msgtype = $1;
			my $lineno = $2;
			my $defline = $3;
			my $symbol = $4;
			my $thunkprefix = $5;
			my $thunkoffset = $6;
			my $unthunked = $7;
			my $topnamelen = $8;
			my $restofsymbol = $9;
			
			if ($msgtype eq "E")
				{
				push @errors, "$unthunked\@$thunkprefix $thunkoffset $lineno $symbol";
				}
			else
				{
				push @warnings, "$unthunked\@$thunkprefix $thunkoffset $symbol";
				}
				
			my $class = substr $restofsymbol, 0, $topnamelen;
			$Classes{$class} = 1;
			}
		else
			{
			print "WARNING: Ignored - not a thunk: $line\n";
			}
		}
	
	# Match up the errors and warnings for related symbols
	
	@errors = sort @errors;
	@warnings = sort @warnings;
	my $error;
	my $warning;
	while (scalar @errors && scalar @warnings)
		{
		# Unpack the first entry in each of the lists
		
		$error = shift @errors;
		my ($ekey, $eoffset, $eline, $esymbol) = split / /, $error;
		$warning = shift @warnings;
		my ($wkey, $woffset, $wsymbol) = split / /, $warning;
		
		# Are they for the same thunk?
		
		if ($ekey lt $wkey) 
			{
			# no - unmatched error, so put back the warning
			unshift @warnings, $warning;
			print "Nothing to replace missing symbol on $eline : $esymbol\n";
			$problems += 1;
			next;
			}
		
		if ($ekey gt $wkey)
			{
			# no - unmatched warning, so put back the error
			unshift @errors, $error;
			print "Nothing missing for replacement symbol : $wsymbol\n";
			$problems += 1;
			next;
			}
		
		# Yes - create replacement instruction
		
		push @replacements, "$eline $esymbol => $wsymbol";
		}
	
	# drain remaining problems, if any
	
	foreach my $error (@errors)
		{
		my ($ekey, $eoffset, $eline, $esymbol) = split / /, $error;
		print "Nothing to replace missing symbol on $eline : $esymbol\n";
		$problems += 1;
		}
	foreach my $warning (@warnings)
		{
		my ($wkey, $woffset, $wsymbol) = split / /, $warning;
		print "Nothing missing for replacement symbol : $wsymbol\n";
		$problems += 1;
		}
		
	if ($verbose)
		{
		print "\nSubstitions identified:\n\t";
		print join("\n\t", sort @replacements);
		print "\n";
		}
	
	open DEFFILE, "<$def" or print "Can't open $def: $!\n" and next;
	my @deflines = <DEFFILE>;
	close DEFFILE;
	my $changedlines = 0;

	foreach my $fix (@replacements)
		{
		my ($lineno, $before, $to, $after) = split ' ', $fix;

		my $line = @deflines[$lineno-1];
		if ($line =~ /\s($after)\s/)
			{
			print "$lineno - already fixed\n";
			next;
			}
		if ($line =~ /\s($before)\s/)
			{
			$line =~ s/(\s)$before(\s)/$1$after$2/;
			@deflines[$lineno-1] = $line;
			print "Changed $lineno to $line" if ($verbose > 1);
			$changedlines += 1;
			next;
			}
		print "$lineno doesn't contain $before\n";
		$problems += 1;
		}
	print "\n";
	
	if ($problems != 0)
		{
		print "WARNING: $problems thunks could not be repaired\n";
		}

	if ($changedlines == 0)
		{
		print "Nothing to change\n";
		next;
		}
	print "Will change $changedlines lines\n\n";

	# Now update the file (and edit in Perforce if required)
		
	if ($update)
		{
		chmod 0666, $def;	# make it writeable
		
		open DEFFILE, ">$def" or print "Can't open $def for writing: $!\n" and next;
		print DEFFILE @deflines;
		close DEFFILE;
		
		print "Updated $def\n";
		push @DefFileList, $def;
		
		if ($use_perforce)
			{
			print "* p4 edit $def\n";
			system "p4 edit $def";
			print "\n";
			}
		}
	}

# 5. More diagnostic information

if (scalar @DefFileList)
	{
	print "\nList of updated def files\n";
	print join("\n", @DefFileList);
	print "\n";
	}

if ($verbose && scalar keys %Classes != 0)
	{
	print "\nList of affected classes:\n";
	print join("\n", sort keys %Classes), "\n";
	}

__END__
:endofperl
