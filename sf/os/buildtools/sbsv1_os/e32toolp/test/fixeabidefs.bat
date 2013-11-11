@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
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
#line 14

use strict;
use Getopt::Long;

my $toolVersion = 1.0;
my $RVCTVersion = 2.1;
my $RVCTBuild = 416;
my @redundantExportsRegEx = (qr/^_ZTI/, qr/^_ZTV/);


# 1. Check arguments, output help etc.

if (@ARGV == 0)
	{
	print (STDERR "\nFIXEABIDEFS.BAT - Version $toolVersion for RVCT$RVCTVersion b$RVCTBuild\n");

	print STDERR << 'END_OF_HELP';

Usage: fixeabidefs.bat [-analyse|-list|-update] build_log.log 

Parses the output from build_log.log and locates all MAKEDEF errors where ARMV5 built
object files do not include exports specified in the current, frozen, EABI .def file.
Where this has occurred, the tool checks against an internal list to determine
whether these are redundant exports that are no longer required.

-analyse  Process MAKEDEF errors and warnings list .def files that cannot be updated by this tool.
-list     Lists all .def files that will be updated by the tool.
-update   Creates updated .def files with redundant exports removed.

Updated .def files have redundant exports removed by "back-filling" from unfrozen exports
flagged against the same .def file.  If no unfrozen exports are available, the "ABSENT"
export placeholder is used to effectively remove the export, but maintain a degree of BC.

NOTE: The tool assumes that the original build source layout is replicated on the drive
where it is being executed.

Redundant exports processed on the following regex:
END_OF_HELP

	foreach my $redundantExportRegEx (@redundantExportsRegEx)
		{
		print ("\t$redundantExportRegEx\n");
		}

	exit(1);
	}

my $list = 0;
my $update = 0;
my $analyse = 0;

GetOptions ('list' => \$list, 'update' => \$update, 'analyse' => \$analyse);


# 2. Parse the log and record the detail of all relevant MAKEDEF errors and warnings

my $BUILD_LOG = $ARGV[0];
open BUILD_LOG, "< $BUILD_LOG" or die "\nCan't read $BUILD_LOG!\n\n";

my $line;
my @buildLog;

while ($line = <BUILD_LOG>)
	{
	push @buildLog, $line;
	}

close BUILD_LOG;

my $defFile;
my $export;
my $processExport;
my $variant;

# All hashes keyed on fully pathed .def file
my %impactedDefFiles;
my %missingURELExports;
my %missingUDEBExports;
my %unfrozenURELExports;
my %unfrozenUDEBExports;

my $parseWarning = 0;
my $parseError = 0;

my @exports;


# Scan the log and build up UREL and UDEB lists of unfrozen and missing exports

foreach $line (@buildLog)
	{
	if (($line !~ /^  /))
		{
		if ($parseError)
			{
			$variant =~ /UREL/ ? push @{$missingURELExports{$defFile}}, [@exports] : push @{$missingUDEBExports{$defFile}}, [@exports];
			$parseError = 0;
			$impactedDefFiles{$defFile} = 1;
			}
		elsif ($parseWarning)
			{
			$variant =~ /UREL/ ? push @{$unfrozenURELExports{$defFile}}, [@exports] : push @{$unfrozenUDEBExports{$defFile}}, [@exports];
			$parseWarning = 0;
			$impactedDefFiles{$defFile} = 1;
			}
		
		@exports = ();
		}
		
	if ($line =~ /^  make.* CFG\=/)
		{
		$variant = $line;
		$variant =~ s/^.*CFG\=//;
		$variant =~ s/ .*$//;
		$variant =~ s/\s//g;
		next;
		}
		
	if ($line =~ /MAKEDEF WARNING:.*not yet Frozen in/)
		{			
		$parseError = 0;
		$parseWarning = 1;

		$defFile = $line;
		$defFile =~ s/^.*not yet Frozen in//;
		$defFile =~ s/:$//;
		$defFile =~ s/\s//g;
		$defFile = lc $defFile;
		next;
		}
	
	if ($line =~ /MAKEDEF ERROR:/)
		{
		$parseError = 1;
		$parseWarning = 0;
		next;
		}

	if (($line =~ /^  /) && ($parseError || $parseWarning))
		{			
		$export = $line;
		$export =~ s/^.* : //;
		$export =~ s/ .*$//;
		$export =~ s/\s//g;

		if ($parseError)
			{
			$defFile = $line;
			$defFile =~ s/\(.*$//;
			$defFile =~ s/\s//g;
			$defFile = lc $defFile;
			}

		push @exports, $export;
		
		next;
		}
	}


my %validDefFiles = %impactedDefFiles;

my %missingExportDifferences;
my %unfrozenExportDifferences;

my %sharedDifferences;

my $redundantExportRegEx;
my %invalidExports;
my $validExport;

my $URELelements;
my $UDEBelements;
my $index1;
my $index2;

foreach $defFile (sort keys %impactedDefFiles)
	{
	if ($missingURELExports{$defFile})
		{			
		$URELelements = @{$missingURELExports{$defFile}};
		$UDEBelements = @{$missingUDEBExports{$defFile}};

		for ($index1 = 0; $index1 < $URELelements; $index1++)
			{
			foreach $export (@{@{$missingURELExports{$defFile}}[$index1]})
				{
				for ($index2 = 0; $index2 < $UDEBelements; $index2++)
					{
					if (!grep (/$export/, @{@{$missingUDEBExports{$defFile}}[$index2]}))
						{
						if (!$index1 && !$index2)
							{
							$missingExportDifferences{$defFile} = 1;
							}
						else
							{
							$sharedDifferences{$defFile} = 1;
							}

						delete $validDefFiles{$defFile};
						}
					}

				$validExport = 0;
			
				foreach $redundantExportRegEx (@redundantExportsRegEx)
					{
					if ($export =~ /$redundantExportRegEx/)
						{
						$validExport = 1;
						}
					}

				if (!$validExport)
					{
					${$invalidExports{$defFile}}{$export} = 1;
					delete $validDefFiles{$defFile};
					}
				}
			}

		for ($index1 = 0; $index1 < $URELelements; $index1++)
			{
			foreach $export (@{@{$missingUDEBExports{$defFile}}[$index1]})
				{
				for ($index2 = 0; $index2 < $URELelements; $index2++)
					{
					if (!grep (/$export/, @{@{$missingURELExports{$defFile}}[$index2]}))
						{
						if (!$index1 && !$index2)
							{
							$missingExportDifferences{$defFile} = 1;
							}
						else
							{
							$sharedDifferences{$defFile} = 1;
							}
							
						delete $validDefFiles{$defFile};
						}
					}

				$validExport = 0;
			
				foreach $redundantExportRegEx (@redundantExportsRegEx)
					{
					if ($export =~ /$redundantExportRegEx/)
						{
						$validExport = 1;
						}
					}

				if (!$validExport)
					{
					${$invalidExports{$defFile}}{$export} = 1;
					delete $validDefFiles{$defFile};
					}
				}
			}
		}

	if (!$unfrozenURELExports{$defFile} && $unfrozenUDEBExports{$defFile})
		{
		$unfrozenExportDifferences{$defFile}  = 1;
		}
	elsif ($unfrozenURELExports{$defFile})
		{
		$URELelements = @{$unfrozenURELExports{$defFile}};
		$UDEBelements = @{$unfrozenUDEBExports{$defFile}};

		for ($index1 = 0; $index1 < $URELelements; $index1++)
			{
			foreach $export (@{@{$unfrozenURELExports{$defFile}}[$index1]})
				{
				for ($index2 = 0; $index2 < $UDEBelements; $index2++)
					{
					if (!grep (/$export/, @{@{$unfrozenUDEBExports{$defFile}}[$index2]}))
						{
						if (!$index1 && !$index2)
							{
							$unfrozenExportDifferences{$defFile} = 1;
							}
						else
							{
							delete $validDefFiles{$defFile};
							$sharedDifferences{$defFile} = 1;
							}
						}
					}
				}
			}

		for ($index1 = 0; $index1 < $URELelements; $index1++)
			{
			foreach $export (@{@{$unfrozenUDEBExports{$defFile}}[$index1]})
				{
				for ($index2 = 0; $index2 < $URELelements; $index2++)
					{
					if (!grep (/$export/, @{@{$unfrozenURELExports{$defFile}}[$index2]}))
						{
						if (!$index1 && !$index2)
							{
							$unfrozenExportDifferences{$defFile} = 1;
							}
						else
							{
							delete $validDefFiles{$defFile};
							$sharedDifferences{$defFile} = 1;
							}
						}
					}
				}
			}
		}
	}

if ($analyse)
	{
	if (%missingExportDifferences)
		{
		print ("\nThe following .def files differ in the number of missing exports between UREL and UDEB builds.\n");
		print ("These files will not be treated by this tool.\n\n");
		
		foreach $defFile (sort keys %missingExportDifferences)
			{
			print ("\t$defFile\n");
			}
		}

	if (%unfrozenExportDifferences)
		{	
		print ("\nThe following .def files differ in the number of unfrozen exports between UREL and UDEB builds.\n");
		print ("These files will be or were back-filled with regard to any UREL unfrozen exports available.");
		print ("If no UREL unfrozen exports are available, the classes in question have to be marked non-sharable.\n\n");
				
		foreach $defFile (sort keys %unfrozenExportDifferences)
			{
			print ("\t$defFile\n");
			}
		}

	if (%sharedDifferences)
		{
		print ("\nThe following .def files are shared between multiple components, and differ in exports\n");
		print ("between either UDEB or UREL builds, or between the build of the components from which they are used.\n");
		print ("These files will not be treated by this tool.\n\n");
		
		foreach $defFile (sort keys %sharedDifferences)
			{
			print ("\t$defFile\n");
			}
		}

	if (%invalidExports)
		{
		print ("\nThe following .def files contain missing exports that cannot be resolved by this tool.\n");

		foreach $defFile (sort keys %invalidExports)
			{
			print ("\n\t$defFile\n");

			foreach $export (sort keys %{$invalidExports{$defFile}})
				{
				print ("\t\t$export\n");
				}
			}
		}

	if (!%missingExportDifferences && !%unfrozenExportDifferences && !%sharedDifferences && !%invalidExports)
		{
		print ("\nAll MAKEDEF ERRORs and WARNINGs in the specified log can be treated by this tool.\n");
		}
	}


if ($list)
	{
	foreach $defFile (sort keys %validDefFiles)
		{
		print ("$defFile\n");
		}
	}


if ($update)
	{
	my %updatedDefFiles;
	my $missingExport;
	my $unfrozenExport;

	foreach $defFile (keys %validDefFiles)
		{
		if (!open DEF_FILE, "< $defFile")
			{
			print "Can't read $defFile!\n";
			next;
			}

		while ($line = <DEF_FILE>)
		 	{
		 		
# Either Back-fill or make 'ABSENT' any missing exports

			if ($missingURELExports{$defFile})
				{
				foreach $missingExport (@{@{$missingURELExports{$defFile}}[0]})
					{
					if ($line =~ /$missingExport/)
						{
						$unfrozenExport = "";

						if ($unfrozenURELExports{$defFile})
							{
							$unfrozenExport = pop @{@{$unfrozenURELExports{$defFile}}[0]};
							}

						if ($unfrozenExport)
							{
							$line =~ s/$missingExport/$unfrozenExport/;

							# If there's an existing comment section, then update it appropriately

							my $commentUpdate = '';
					
							if ($unfrozenExport =~ /^_ZTV/)
								{
								$commentUpdate = '; #<VT>#';
								}
							elsif ($unfrozenExport =~ /^_ZTI/)
								{
								$commentUpdate = '; #<TI>#';
								}
					
							$line =~ s/;.*$/$commentUpdate/;
							}
						else
							{
							if ($line =~ / \;/)
								{
								$line =~ s/ \;/ ABSENT \;/;
								}
							else
								{
								$line.= ' ABSENT';
								}
							}

						$line .= "\n" unless ($line =~ /\n$/);
							
						last;
						}
					}
			 	}

			push (@{$updatedDefFiles{$defFile}}, $line);
			}
		close DEF_FILE;
		}


# Resolve any remaining unfrozen exports in the standard way

	my $lastExportIndex;
	my $lastExportOrdinal;

	foreach $defFile (keys %updatedDefFiles)
		{
		if ($unfrozenURELExports{$defFile} && @{@{$unfrozenURELExports{$defFile}}[0]})
			{
			$lastExportIndex = @{$updatedDefFiles{$defFile}}-1;

			while (@{$updatedDefFiles{$defFile}}[$lastExportIndex] =~ /^\s$/)
				{
				pop @{$updatedDefFiles{$defFile}};
				$lastExportIndex--;
				}

			@{$updatedDefFiles{$defFile}}[$lastExportIndex] .= "\n" unless (@{$updatedDefFiles{$defFile}}[$lastExportIndex] =~ /\n$/);

			$lastExportOrdinal = @{$updatedDefFiles{$defFile}}[$lastExportIndex];
			$lastExportOrdinal =~ s/^.*@\s+//;
			$lastExportOrdinal =~ s/ .*$//;
			$lastExportOrdinal =~ s/\s//g;

			foreach $unfrozenExport (@{@{$unfrozenURELExports{$defFile}}[0]})
				{
				$lastExportOrdinal++;

				my $comment = '';
		
				if ($unfrozenExport =~ /^_ZTV/)
					{
					$comment = ' ; #<VT>#';
					}
				elsif ($unfrozenExport =~ /^_ZTI/)
					{
					$comment = ' ; #<TI>#';
					}
				
				push (@{$updatedDefFiles{$defFile}}, "\t$unfrozenExport @ $lastExportOrdinal NONAME$comment\n");
				}
			
			push (@{$updatedDefFiles{$defFile}}, "\n");
			}
		}


# Create the new .def files

	foreach $defFile (sort keys %updatedDefFiles)
		{		
		if (!open NEW_DEF_FILE, ">$defFile")
			{
			print ("ERROR : Can\'t create \"$defFile\"\n");
			next;
			}

		foreach $line (@{$updatedDefFiles{$defFile}})
			{
			print (NEW_DEF_FILE $line);
			}

		close NEW_DEF_FILE;

		print ("Created : \"$defFile\"\n");
		}

	}


__END__
:endofperl
