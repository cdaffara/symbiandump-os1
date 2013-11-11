# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# Perl script to scan tree looking for MMP files with particular keywords
# 
#

use strict;
use File::Find;			# for find()
use File::Basename;		# for fileparse()
use Cwd;				# for getcwd()

use Getopt::Long;
my %Options=();
GetOptions(\%Options, "f=s");

my @patterns = (

'START\s+BITMAP.*\.MCL',
'START\s+BITMAP.*\.MBW',
'START\s+BITMAP',
'START\s+RESOURCE',
'ALWAYS_BUILD_AS_ARM',
'ASSPABI',
'ASSPEXPORTS',
'ASSPLIBRARY',
'CAPABILITY',
'COMPRESSTARGET',
'DEFFILE',
'DOCUMENT',
'EPOCALLOWDLLDATA',
'EPOCCALLDLLENTRYPOINTS',
'EPOCDATALINKADDRESS',
'EPOCFIXEDPROCESS',
'EPOCHEAPSIZE',
'EPOCPROCESSPRIORITY',
'EPOCSTACKSIZE',
'EXPORTLIBRARY',
'EXPORTUNFROZEN',
'FIRSTLIB',
'LANG',
# 'LIBRARY',
'DEBUGLIBRARY',
'LINKAS',
'MACRO\s+__SECURE_API__',
'MACRO',
'NOCOMPRESSTARGET',
'NOEXPORTLIBRARY',
'NOSTRICTDEF',
'OPTION',
'RAMTARGET',
'RESOURCE',
'ROMTARGET',
'SECUREID',
'SRCDBG',
# 'SOURCE',
# 'SOURCEPATH',
'STATICLIBRARY',
'STRICTDEPEND',
'SYSTEMINCLUDE',
'SYSTEMRESOURCE',
# 'TARGET',
'TARGETPATH',
'TARGETTYPE\s+ANI',
'TARGETTYPE\s+APP',
'TARGETTYPE\s+CTL',
'TARGETTYPE\s+DLL',
'TARGETTYPE\s+ECOMIIC',
'TARGETTYPE\s+EPOCEXE',
'TARGETTYPE\s+EXEDLL',
'TARGETTYPE\s+EXEXP',		# must precede TARGETTYPE EXE
'TARGETTYPE\s+EXE',
'TARGETTYPE\s+FSY',
'TARGETTYPE\s+IMPLIB',
'TARGETTYPE\s+KDLL',
'TARGETTYPE\s+KEXT',
'TARGETTYPE\s+KLIB',
'TARGETTYPE\s+LDD',
'TARGETTYPE\s+LIB',
'TARGETTYPE\s+MDA',
'TARGETTYPE\s+MDL',
'TARGETTYPE\s+NOTIFIER2',
'TARGETTYPE\s+NOTIFIER',
'TARGETTYPE\s+PDD',
'TARGETTYPE\s+PDL',
'TARGETTYPE\s+PLUGIN',
'TARGETTYPE\s+PLUGIN3',
'TARGETTYPE\s+RDL',
'TARGETTYPE\s+TEXTNOTIFIER2',
'TARGETTYPE\s+VAR',
'UID',
'USERINCLUDE',
'VENDORID',
'VERSION',

# from START WINS ... END
'START\s+WINS',
'BASEADDRESS',
'WIN32_LIBRARY',
'WIN32_RESOURCE',
'COPY_FOR_STATIC_LINKAGE',
'WIN32_HEADERS',

# count some unmatched things
'AIF',
'SOURCE\s.*\.CIA',			# CIA files
'TARGETTYPE\s+(other)?',	# a trick to catch unsupported targettypes
'TARGET\s.*DUMMY',			# who doesn't really want a TARGET
'#IF',
'#INCLUDE_FAILURE',			# unresolved #include directives
'#INCLUDE',

);

my %instances = ();
my %patternsbyfile = ();
my $mmpcount = 0;

sub read_mmp($$$)
	{
	my ($top,$name,$lines) =@_;
	open MMP, "<$name" or print "error: $name $top\n" and return 0;
	my @mmplines=<MMP>;
	close MMP;
	
	foreach my $line (@mmplines)
		{
		push @$lines, $line;
		if ($line =~/^\#include\s+(\"|\<)([^"<]+)(\"|\>)/)
			{
			my $currentdir = getcwd();
			my $includename = $2;
			if (-f $2)
				{
				# name exists relative to current directory
				}
			elsif (-f "$ENV{EPOCROOT}epoc32/include/$includename")
				{
				$includename = "$ENV{EPOCROOT}epoc32/include/$includename";
				}
			else
				{
				print "Can't locate $2 from $currentdir/$name $top\n";
				push @$lines, "#INCLUDE_FAILURE";
				next;
				}
			my ($newname, $newdir, $newext) = fileparse($includename, '\..*');
			chdir($newdir);
			read_mmp("from $name $top", "$newname$newext", $lines);
			chdir($currentdir);
			}
		}
	return 1;
	}
	
sub mmpgrep 
	{
	my $fullname = $File::Find::name;
	return if (defined($patternsbyfile{$fullname}));

	my ($name,$dir,$ext) = fileparse($fullname,'\..*');
	return unless (lc($ext) eq '.mmp');
	
	my %found = ();
	my @lines = ();
	return if (read_mmp("", "$name$ext", \@lines)==0);
	
	$mmpcount++;
	my $line;
	LINE: foreach $line (@lines)
		{
		$line = uc $line;
		foreach (@patterns)
			{
			if ($line =~/^\s*$_/)
				{
				$found{$_} = 1;
				next LINE;
				}
			}
		}
	close MMP;
	foreach (keys %found)
		{
		@{$instances{$_}} = () if (!defined $instances{$_});
		push @{$instances{$_}}, $fullname;
		}
	@{$patternsbyfile{$fullname}} = sort keys %found;
	}

# Scan the tree, processing each filename with mmpgrep

my @scanlist = ();
if ($Options{f})
	{
	open FILELIST, "<$Options{f}" or die "Cannot open $Options{f}\n";
	my $line;
	while ($line = <FILELIST>)
		{
		$line =~ s/#.*$//;		# remove comments
		$line =~ s/\s*$//;		# remove trailing ws
		$line =~ s/^\s*//;		# remove leading ws
		next if ($line eq "");
		push @scanlist, $line;
		}
	close FILELIST;
	}
push @scanlist, @ARGV;
find (\&mmpgrep,@scanlist);

# Report the results

print "$mmpcount mmp files processed\n";

my $pattern;
print "\nSummary by pattern\n";
foreach $pattern (@patterns)
	{
	my $count=0;
	my $list = $instances{$pattern};
	$count = scalar @{$list} if (defined $list);
	printf "%-30s %5d\n", $pattern, $count;
	}
	
print "\nDetail\n";
foreach $pattern (sort keys %instances)
	{
	my $list = $instances{$pattern};
	my $title = $pattern;
	$title =~ s/\\s\+/ /;
	print "\n$title:\n";
	foreach my $file (@{$list})
		{
		print "  $file\n";
		}
	}

foreach (sort keys %patternsbyfile)
	{
	print "\n$_ :  ", join(" ",@{$patternsbyfile{$_}});
	}

print "\n";
