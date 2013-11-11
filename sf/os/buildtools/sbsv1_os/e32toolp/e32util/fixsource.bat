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
use File::Spec;
use File::Copy;
use File::Path;
use File::Basename;

my $toolVersion = "2.1";

# Example warnings:
#
# Incorrect slash
#	\src\common\generic\comms-infras\commsfw\group\bld.inf:10: Incorrect slash in PRJ_EXPORTS - '..\inc\commschan.h'.
#	\src\cedar\generic\base\f32\group\ecomp.mmp:14: Incorrect slash in SYSTEMINCLUDE - '..\inc'.
#	\src\common\generic\syncml\framework\TransportProvision\HttpWsp\SmlHttpBase.cpp:13: Incorrect slash in #include - 'http\rhttpheaders.h'.
#
# Incorrect case for epoc32 tree
#	\src\common\generic\syslibs\pwrcli\group\bld.inf:23: Incorrect case for epoc32 tree in PRJ_EXPORTS - '\epoc32\rom\include\PwrCli.IBY'.
#	\src\common\generic\security\crypto\group\hash.mmp:8: Incorrect case for epoc32 tree in TARGET - 'hash.DLL'.
#	\src\common\generic\syslibs\ecom\ongoing\Framework\frame\Discoverer.cpp:20: Incorrect case for epoc32 tree in #include - 'BaSPI.h'.
#
# Incorrect case versus filesystem
#	\src\common\generic\comms-infras\commdb\commdbshim\group\BLD.INF:20: Incorrect case versus filesystem in PRJ_EXPORTS - '..\inc\cdblen.h' vs. \src\common\generic\comms-infras\commdb\commdbshim\INC\CDBLEN.H.
#	\src\common\generic\syslibs\ecom\ongoing\Framework\MMPFiles\EComServer.mmp:45: Incorrect case versus filesystem in USERINCLUDE - '..\..\framework\inc' vs. \src\common\generic\syslibs\ecom\ongoing\Framework\inc.
#	\src\common\generic\comms-infras\commdb\commdbshim\INC\CDBOVER.H:16: Incorrect case versus filesystem in #include - 'cdbpreftable.h' vs. \src\common\generic\comms-infras\commdb\commdbshim\INC\CDBPREFTABLE.H.
#
# Incorrect case for epoc32 tree from default export (i.e. source export case will replicated under \epoc32)
#	\src\common\generic\application-protocols\http\group\bld.inf:22: Incorrect case for epoc32 tree from default export in PRJ_EXPORTS - '..\inc\HTTPSocketConstants.h'.
#	\src\common\generic\messaging\email\smtpservermtm\group\bld.inf:11: Incorrect case for epoc32 tree from default export in PRJ_EXPORTS - '..\inc\smts.h' vs. \src\common\generic\messaging\email\smtpservermtm\inc\SMTS.H.
#
# Incorrect case versus exclusion list
#	\src\common\generic\Multimedia\openmax\group\bld.inf:14: Incorrect case versus exclusion list in PRJ_EXPORTS - '\epoc32\include\openmax\il\OMX_Types.h' vs. OMX_TYPES.H.
#	\src\common\generic\Multimedia\openmax\inc\openmax\il\OMX_Audio.h:41: Incorrect case versus exclusion list in #include - 'OMX_Types.h' vs. OMX_TYPES.H.
#
# Can't find physical file match for
#	\src\common\generic\app-framework\conarc\group\BLD.INF:48: Can't find physical file match for PRJ_TESTMMPFILES MMP - '..\tsrc\tcon3_V2.mpp' on filesystem.
#


# 1. Check arguments, output help etc.

my $analyse = 0;
my $list = 0;
my $update = 0;
my $warnings = "";
my $verbose = 0;
my $debug = 0;
my $debugUpdate = 0;
GetOptions ('analyse|a' => \$analyse, 'list|l' => \$list, 'update|u' => \$update, 'warnings|w=s' => \$warnings,
			'verbose|v' => \$verbose, 'debug|d' => \$debug, 'debugupdate|du' => \$debugUpdate);

if (@ARGV == 0)
	{
	print (STDERR "\nFIXSOURCE.BAT - Version $toolVersion\n");

	print STDERR << 'END_OF_HELP';

Usage: fixsource.bat -analyse|-list|-update|-warnings buildlog_1.log [buildlog_n.log] 

Parses the output from the specified build logs to locate warnings produced via
"abld -checksource".  Provides the option to update source automatically to comply
with Symbian's Filename Policy.

-analyse | -a	             List and describe all warnings that cannot be addressed
                             by this tool.
-list    | -l	             List all source files that can be updated by this tool
                             using "-update".
-update  | -u	             Update source files, as output by "-list", to comply with
                             Symbian's Filename Policy.
-warnings| -w [all|fixable]  Output all unique "-checksource" warnings present in the
                             specified logs:
                                all     - every warning, regardless of whether this
                                          tool can fix them.
                                fixable - only warnings that this tool can fix.
-verbose | -v                Additional verbose output for the "-update" option.

NOTES:
* The tool assumes that the original build source and layout is present on the drive
  where it is being executed.
* With the exception of the "-warnings all" output, any warnings for files under the
  known release and build locations of %EPOCROOT%epoc32\include and
  %EPOCROOT%epoc32\build are discarded by default.

END_OF_HELP

	}


# 2. Parse the logs storing all GNU format warnings and errors


my %ActionableFilenamePolicyWarnings;	# Hash Key (filename)
										#	Hash Key (line number)
										#		Hash Key (problematic text)
										#			Hash Key ORIGINAL_WARNINGS
										#				Hash Key (original warning)
										#					1
										#			Hash Key ITEM
										#				item type that has generated the warning
										#			Hash Key SEARCH_TEXT
										#				quotemeta version of problematic text
										#			Hash Key UNIX_SLASH
										#				1
										#			Hash Key LOWERCASE
										#				1
										#			Hash Key PHYSICAL
										#				1
										#			Hash Key EXCLUSION
										#				1
										#			Hash Key PHYSICAL_REALITY
										#				Hash Key (physical reality)
										#					1
										#				fully pathed filesystem reality used in physical checking
										#			Hash Key EXCLUSION_LISTING
										#				required format of reference as dictated from an exclusion list
										#			Hash Key ACTUAL_TEST
										#				test used for check when this differed from that in actual source
										#			Hash Key DEFAULT_EXPORT
										#				special case - a PRJ_EXPORTS line without a destination will break
										#				the filename policy if the source line is not lowercase
my %NonActionableFilenamePolicyWarnings;
my %AllFilenamePolicyWarnings;
my %OtherWarningsAndErrors;

foreach my $BUILD_LOG (@ARGV)
	{
	open BUILD_LOG, "< $BUILD_LOG" or die "\nCannot read \"$BUILD_LOG\"!\n\n";

	while (<BUILD_LOG>)
		{
		chomp;
			
		if (/^\\\S+:.*: .+$/)
			{
			if (!/:\d+: (Incorrect case|Incorrect slash|Can\'t find) /)
				{
				$OtherWarningsAndErrors{$_} = 1 if (!/(unresolved api-item|unknown base class|unresolved xref|no image file|xm-replace_text)/);	# Ignore the noise of doc stuff...
				next;					
				}

			$AllFilenamePolicyWarnings{$_} = 1;
			
			if (/: Can\'t find /)
				{
				$NonActionableFilenamePolicyWarnings{$_} = 1;
				next;
				}

			my $originalWarning = $_;

			/(^.*):(\d+): Incorrect (case for epoc32 tree|case versus filesystem|case versus exclusion list|slash|case for epoc32 tree from default export) in (.+) - \'(.+?)\'/;

			my $filename = $1;
			my $lineNumber = $2;
			my $type = $3;
			my $item = $4;
			my $problematicText = $5;

			$type =~ s/case for epoc32 tree from default export/defaultexport/;
			$type =~ s/case for epoc32 tree/lowercase/;
			$type =~ s/case versus filesystem/physical/;
			$type =~ s/case versus exclusion list/exclusion/;

			my $actualTest = "";
			my $physicalReality = "";
			my $exclusionListing = "";
			$actualTest = $1 if (/\(actual test \'(.*)\'\)/);

			if (/ vs\. (.*)\./)
				{
				$physicalReality = $1 if ($type eq "physical");
				$exclusionListing = $1 if ($type eq "exclusion");
				}

			if ($debug)
				{
				print ("ORIGINAL WARNING   : $originalWarning\n");
				print ("FILENAME           : $filename\n");
				print ("LINENUMBER         : $lineNumber\n");
				print ("TYPE               : $type\n");
				print ("ITEM               : $item\n");
				print ("PROBLEMATIC TEXT   : $problematicText\n");
				print ("ACTUAL TEST        : $actualTest\n") if ($actualTest);
				print ("PHYSICAL REALITY   : $physicalReality\n") if ($physicalReality);
				print ("EXCLUSION LISTING  : $exclusionListing\n") if ($exclusionListing); 
				print ("\n");
				}

			next if ($warnings =~ /all/i);

	# Line Number

			my $lineNumberHashRef;
			if ($ActionableFilenamePolicyWarnings{$filename})
				{
				$lineNumberHashRef = $ActionableFilenamePolicyWarnings{$filename};
				}
			else
				{
				my %newHash;
				$lineNumberHashRef = \%newHash;
				$ActionableFilenamePolicyWarnings{$filename} = $lineNumberHashRef;
				}

	# Problematic Text

			my $problematicTextHashRef;
			if ($lineNumberHashRef->{$lineNumber})
				{
				$problematicTextHashRef = $lineNumberHashRef->{$lineNumber};
				}
			else
				{
				my %newHash;
				$problematicTextHashRef = \%newHash;
				$lineNumberHashRef->{$lineNumber} = $problematicTextHashRef;
				}

	# Attributes

			my $attributesHashRef;
			if ($problematicTextHashRef->{$problematicText})
				{
				$attributesHashRef = $problematicTextHashRef->{$problematicText};
				}
			else
				{
				my %newHash;
				$attributesHashRef = \%newHash;
				$problematicTextHashRef->{$problematicText} = $attributesHashRef;
				}

	# Attributes : Original Warnings

			my $originalWarningsHashRef;
			if ($attributesHashRef->{ORIGINAL_WARNINGS})
				{
				$originalWarningsHashRef = $attributesHashRef->{ORIGINAL_WARNINGS};
				}
			else
				{
				my %newHash;
				$originalWarningsHashRef = \%newHash;			
				$attributesHashRef->{ORIGINAL_WARNINGS} = $originalWarningsHashRef;
				}
			$originalWarningsHashRef->{$originalWarning} = 1;

	# Attributes : Item

			$attributesHashRef->{ITEM} = $item;

	# Attributes : Search Text

			$attributesHashRef->{SEARCH_TEXT} = quotemeta ($problematicText);

	# Attributes : Unix Slash

			$attributesHashRef->{UNIX_SLASH} = 1 if ($type eq "slash");

	# Attributes : Lowercase

			$attributesHashRef->{LOWERCASE} = 1 if ($type eq "lowercase");

	# Attributes : Physical

			$attributesHashRef->{PHYSICAL} = 1 if ($type eq "physical");

	# Attributes : Exclusion

			$attributesHashRef->{EXCLUSION} = 1 if ($type eq "exclusion");

	# Attributes : Physical Reality

			my $physicalRealityHashRef;
			if ($physicalReality)
				{
				if ($attributesHashRef->{PHYSICAL_REALITY})
					{
					$physicalRealityHashRef = $attributesHashRef->{PHYSICAL_REALITY};
					}
				else
					{
					my %newHash;
					$physicalRealityHashRef = \%newHash;			
					$attributesHashRef->{PHYSICAL_REALITY} = $physicalRealityHashRef;
					}
				$physicalRealityHashRef->{$physicalReality} = 1;
				}

	# Attributes : Actual Test

			$attributesHashRef->{ACTUAL_TEST} = $actualTest if ($actualTest);

	# Attributes : Exclusion Listing

			$attributesHashRef->{EXCLUSION_LISTING} = $exclusionListing if ($exclusionListing);

	# Attributes : Default Export

			$attributesHashRef->{DEFAULT_EXPORT} = 1 if ($type eq "defaultexport");
			}
		}

	close BUILD_LOG;
	}


# 3. Examine source and warnings and compile lists of files and warnings that we can/can't do anything about

my %WarningsNotMatchingSource;
my %WarningsWithMissingFiles;
my %WarningsForBothLowercaseAndPhysicalOnSameReference;
my %WarningsForMultiplePhysicalUpdates;


if ($analyse || $list || $update || $debugUpdate || $warnings =~ /^fixable$/i)
	{
		
	foreach my $SOURCE_FILE (sort keys %ActionableFilenamePolicyWarnings)
		{
			
	# Discard anything in known release locations

		if ($SOURCE_FILE =~ /\\epoc32\\(include|build)\\/i)
			{
			delete ($ActionableFilenamePolicyWarnings{$SOURCE_FILE});
			next;
			}

		my $lineNumbersHashRef = $ActionableFilenamePolicyWarnings{$SOURCE_FILE};

	# Discard warnings where source files cannot be found

		if (!(open SOURCE_FILE, "< $SOURCE_FILE"))
			{
			foreach my $lineNumber (sort keys (%$lineNumbersHashRef))
				{
				my $problematicTextHashRef = $lineNumbersHashRef->{$lineNumber};

				foreach my $problematicText (sort keys (%$problematicTextHashRef))
					{
					my $attributesHashRef = $problematicTextHashRef->{$problematicText};
					my $originalWarningsHashRef = $attributesHashRef->{ORIGINAL_WARNINGS};
					
					foreach my $originalWarning (keys (%$originalWarningsHashRef))
						{
						$WarningsWithMissingFiles{$originalWarning} = 1;
						}					
					}
				}
			delete ($ActionableFilenamePolicyWarnings{$SOURCE_FILE});
			next;
			}


	# Identify and discard warnings where, for the same reference:
	# (a) both lowercase and physical warnings are flagged and
	# (b) multiple, different, filesystem matches have been found
	# These will need to be resolved manually

		foreach my $lineNumber (sort keys (%$lineNumbersHashRef))
			{
			my $problematicTextHashRef = $lineNumbersHashRef->{$lineNumber};

			foreach my $problematicText (sort keys (%$problematicTextHashRef))
				{
				my $attributesHashRef = $problematicTextHashRef->{$problematicText};
				my $originalWarningsHashRef = $attributesHashRef->{ORIGINAL_WARNINGS};

				my $skipPhysicalUpdate = 0;
				my $skipLowercaseUpdate = 0;
				
				if ($attributesHashRef->{LOWERCASE} && $attributesHashRef->{PHYSICAL})
					{
					$skipPhysicalUpdate = 1;
					$skipLowercaseUpdate = 1;

					foreach my $originalWarning (keys %{$originalWarningsHashRef})
						{
						next if ($originalWarning !~ /Incorrect case/);

						$originalWarning =~ /\:(.*$)/;

						my $lowercaseAndPhysicalWarningsHashRef;	
						if ($WarningsForBothLowercaseAndPhysicalOnSameReference{$SOURCE_FILE})
							{
							$lowercaseAndPhysicalWarningsHashRef = $WarningsForBothLowercaseAndPhysicalOnSameReference{$SOURCE_FILE};
							}
						else
							{
							my %newHash;
							$lowercaseAndPhysicalWarningsHashRef = \%newHash;			
							$WarningsForBothLowercaseAndPhysicalOnSameReference{$SOURCE_FILE} = $lowercaseAndPhysicalWarningsHashRef;
							}
						$lowercaseAndPhysicalWarningsHashRef->{$1} = 1;						
						}
					}

				my $physicalRealityHashRef = $attributesHashRef->{PHYSICAL_REALITY};

				if ($physicalRealityHashRef && ((keys %{$physicalRealityHashRef}) > 1))
					{						
					my $physicalMatchCheck;
					if ($attributesHashRef->{ACTUAL_TEST})
						{
						$physicalMatchCheck = $attributesHashRef->{ACTUAL_TEST};
						}
					else
						{
						$physicalMatchCheck = $problematicText;
						$physicalMatchCheck =~ s/\.\.[\\|\/]//g;
						$physicalMatchCheck =~ s/\.[\\|\/]//g;
						}

					$physicalMatchCheck =~ s/\\\\/\\/g;		
					$physicalMatchCheck =~ s/\/\//\//g;
					$physicalMatchCheck =~ s/\//\\/g;
					$physicalMatchCheck = quotemeta($physicalMatchCheck);
					$physicalMatchCheck =~ s/\\\*/\\w\+/g;			# * -> \w+
					$physicalMatchCheck =~ s/\\\?/\\w\{1\}/g;		# ? -> \w{1}

					my %normalisedPhysicalReferences;
	
					foreach my $physicalReality (keys %{$physicalRealityHashRef})
						{
						$physicalReality =~ /($physicalMatchCheck)$/i;
						$normalisedPhysicalReferences{$1} = 1;
						}

					if ((keys (%normalisedPhysicalReferences)) > 1)
						{
						foreach my $originalWarning (keys %{$originalWarningsHashRef})
							{
							next if ($originalWarning !~ /Incorrect case versus/);

							$originalWarning =~ /\:(.*$)/;

							my $multiplePhysicalWarningsHashRef;	
							if ($WarningsForMultiplePhysicalUpdates{$SOURCE_FILE})
								{
								$multiplePhysicalWarningsHashRef = $WarningsForMultiplePhysicalUpdates{$SOURCE_FILE};
								}
							else
								{
								my %newHash;
								$multiplePhysicalWarningsHashRef = \%newHash;			
								$WarningsForMultiplePhysicalUpdates{$SOURCE_FILE} = $multiplePhysicalWarningsHashRef;
								}
							$multiplePhysicalWarningsHashRef->{$1} = 1;
							}
						$skipPhysicalUpdate = 1;
						}
					}

				$attributesHashRef->{LOWERCASE} = 0 if ($skipLowercaseUpdate);
				$attributesHashRef->{PHYSICAL} = 0 if ($skipPhysicalUpdate);

				if (!$attributesHashRef->{LOWERCASE} && !$attributesHashRef->{PHYSICAL} &&
					!$attributesHashRef->{UNIX_SLASH} && !$attributesHashRef->{DEFAULT_EXPORT} &&
					!$attributesHashRef->{EXCLUSION})
					{
					delete ($problematicTextHashRef->{$problematicText});
					}
				}

			delete ($lineNumbersHashRef->{$lineNumber}) if (!scalar (keys %{$problematicTextHashRef}));
			}

		my $lineNumber = 0;

		while (my $line = <SOURCE_FILE>)
			{
			$lineNumber++;

			next if (!($lineNumbersHashRef->{$lineNumber}));

			my $problematicTextHashRef = $lineNumbersHashRef->{$lineNumber};

			foreach my $text (keys %{$problematicTextHashRef})
				{
				my $attributesHashRef = $problematicTextHashRef->{$text};				

				chomp ($line);

				if ($line !~ /(\"|\<|^|\s){1}$attributesHashRef->{SEARCH_TEXT}/)
					{
					# Put warning(s) onto the failed list, as we can't find the required text
					# in the source present on the machine
						
					my $originalWarningsHashRef = $attributesHashRef->{ORIGINAL_WARNINGS};
					foreach my $originalWarning (keys %{$originalWarningsHashRef})
						{
						$WarningsNotMatchingSource{$originalWarning} = $line;
						}
						
					delete ($problematicTextHashRef->{$text});
					}
				}

			delete ($lineNumbersHashRef->{$lineNumber}) if (!scalar (keys %{$problematicTextHashRef}));
			}

		delete ($ActionableFilenamePolicyWarnings{$SOURCE_FILE}) if (!scalar (keys %{$lineNumbersHashRef}));
		close SOURCE_FILE;
		}
	}


# 4. Provide -warnings [all|fixable] output

if ($warnings =~ /^all$/i)
	{
	foreach my $warning (sort keys %AllFilenamePolicyWarnings)
		{
		print ("$warning\n");
		}
	}
elsif ($warnings =~ /^fixable$/i)
	{
	my %fixableWarnings;
		
	foreach my $sourceFile (keys %ActionableFilenamePolicyWarnings)
		{
		my $lineNumbersHashRef = $ActionableFilenamePolicyWarnings{$sourceFile};
		
		foreach my $lineNumber (keys (%$lineNumbersHashRef))
			{
			my $problematicTextHashRef = $lineNumbersHashRef->{$lineNumber};

			foreach my $text (keys %{$problematicTextHashRef})
				{
				my $attributesHashRef = $problematicTextHashRef->{$text};				
				my $originalWarningsHashRef = $attributesHashRef->{ORIGINAL_WARNINGS};

				foreach my $originalWarning (keys %{$originalWarningsHashRef})
					{						
					$fixableWarnings{$originalWarning} = 1;
					}
				}
			}
		}

	foreach my $fixableWarning (sort keys %fixableWarnings)
		{
		print ("$fixableWarning\n");
		}		
	}


# 5. Provide -list output

if ($list)
	{
	foreach my $sourceFile (sort keys %ActionableFilenamePolicyWarnings)
		{
		print ("$sourceFile\n");
		}
	}


# 6. Provide -analyse output

if ($analyse)
	{
	print ("\nFilename policy warnings with missing files\n".
		     "-------------------------------------------\n\n");
	foreach my $warningWithMissingFile (sort keys %WarningsWithMissingFiles)
		{
		print ("$warningWithMissingFile\n");
		}
	print ("NONE\n") if (!scalar (keys %WarningsWithMissingFiles));


	print ("\n\nFilename policy warnings that don't match source\n".
		       "------------------------------------------------\n\n");		
	foreach my $warningNotMatchingSource (sort keys %WarningsNotMatchingSource)
		{
		print ("$warningNotMatchingSource\n");
		print ("\tACTUAL LINE : \'$WarningsNotMatchingSource{$warningNotMatchingSource}\'\n");
		}
	print ("NONE\n") if (!scalar (keys %WarningsNotMatchingSource));

	print ("\n\nFilename policy warnings with both lowercase and physical warnings for the same reference\n".
		       "-----------------------------------------------------------------------------------------\n\n");		
	foreach my $sourceFile (sort keys %WarningsForBothLowercaseAndPhysicalOnSameReference)
		{
		print ("$sourceFile\n");
		foreach my $warning (sort keys %{$WarningsForBothLowercaseAndPhysicalOnSameReference{$sourceFile}})
			{
			print ("\t$warning\n");
			}
		}
	print ("NONE\n") if (!scalar (keys %WarningsForBothLowercaseAndPhysicalOnSameReference));

	print ("\n\nMultiple differing physical filename policy warnings for the same reference\n".
		       "---------------------------------------------------------------------------\n\n");		
	foreach my $sourceFile (sort keys %WarningsForMultiplePhysicalUpdates)
		{
		print ("$sourceFile\n");
		foreach my $warning (sort keys %{$WarningsForMultiplePhysicalUpdates{$sourceFile}})
			{
			print ("\t$warning\n");
			}
		}
	print ("NONE\n") if (!scalar (keys %WarningsForMultiplePhysicalUpdates));

	print ("\n\nNon-actionable filename policy warnings\n".
		     "---------------------------------------\n\n");		
	foreach my $nonActionableWarning (sort keys %NonActionableFilenamePolicyWarnings)
		{
		print ("$nonActionableWarning\n");
		}
	print ("NONE\n") if (!scalar (keys %NonActionableFilenamePolicyWarnings));


	print ("\n\nOther detected warnings unrelated to filename policy\n".
		     "----------------------------------------------------\n\n");		
	foreach my $otherWarningOrError (sort keys %OtherWarningsAndErrors)
		{
		print ("$otherWarningOrError\n");
		}
	print ("NONE\n") if (!scalar (keys %OtherWarningsAndErrors));

	print ("\n\n");
	}


# 7. Perform -update function

if ($update || $debugUpdate)
	{
	foreach my $SOURCE_FILE (sort keys %ActionableFilenamePolicyWarnings)
		{
		if (!(open SOURCE_FILE, "< $SOURCE_FILE"))
			{				
			print ("ERROR: Could not open $SOURCE_FILE to read.\n");
			next;
			}

		print ("Updating \'$SOURCE_FILE\'...\n") unless ($debugUpdate);

		my $lineNumbersHashRef = $ActionableFilenamePolicyWarnings{$SOURCE_FILE};
		my $lineNumber = 0;
		my @newSourceFile;

		while (my $line = <SOURCE_FILE>)
			{
			$lineNumber++;

			if ($lineNumbersHashRef->{$lineNumber})
				{				
				print ("\tOriginal : $line") if ($verbose);
					
				my $problematicTextHashRef = $lineNumbersHashRef->{$lineNumber};

				# We need to order the updates on a per-line basis so that, for example,
				# a search and update for 'nkern\arm\' occurs after one for 'include\nkern\arm\nk_plat.h'
				# We can do this by length, making sure the longest updates are performed first
				my @problematicTextOrderedHashKeys = sort {length $b <=> length $a} (keys %{$problematicTextHashRef});

				foreach my $problematicText (@problematicTextOrderedHashKeys)
					{
					my $attributesHashRef = $problematicTextHashRef->{$problematicText};
					my $revisedText = $problematicText;

		# Physical

					if ($attributesHashRef->{PHYSICAL})
						{
						my $physicalRealityHashRef = $attributesHashRef->{PHYSICAL_REALITY};

						my $physicalReality = (keys %{$physicalRealityHashRef})[0];
						my $physicalRealityUnixSlash = $physicalReality;
						$physicalRealityUnixSlash =~ s/\\/\//g;						
							
						if ($physicalReality =~ /($attributesHashRef->{SEARCH_TEXT})$/i ||
							$physicalRealityUnixSlash =~ /($attributesHashRef->{SEARCH_TEXT})$/i)
							{
							# Simple case - direct match with just case and slash differences
							my $replacement = $1;
							$replacement =~ s/\\/\//g;
							$revisedText =~ s/$attributesHashRef->{SEARCH_TEXT}/$replacement/;
							}
						else
							{
							# What we're looking at in the source file doesn't map directly
							# to what's physically on the file system.

							my $modifiedSearchText = $problematicText;
							$modifiedSearchText =~ s/\.\.[\\|\/]//g;
							$modifiedSearchText =~ s/\.[\\|\/]//g;

							my $physicalMatchCheck;
							if ($attributesHashRef->{ACTUAL_TEST})
								{
								$physicalMatchCheck = $attributesHashRef->{ACTUAL_TEST};
								}
							else
								{
								$physicalMatchCheck = $modifiedSearchText;
								}

							# The physical match check needs to remove double-slashing...
							$physicalMatchCheck =~ s/\\\\/\\/g;		
							$physicalMatchCheck =~ s/\/\//\//g;
							
							$modifiedSearchText = quotemeta($modifiedSearchText);
							$physicalMatchCheck = quotemeta($physicalMatchCheck);

							$physicalMatchCheck =~ s/\\\*/\\w\+/g;			# * -> \w+
							$physicalMatchCheck =~ s/\\\?/\\w\{1\}/g;		# ? -> \w{1}

							if ($physicalReality =~ /($physicalMatchCheck)$/i ||
								$physicalRealityUnixSlash =~ /($physicalMatchCheck)$/i )
								{
								my $replacement = $1;
								$replacement =~ s/\\/\//g;
								
								if ($attributesHashRef->{ACTUAL_TEST} &&
									($attributesHashRef->{ITEM} =~ /MMP$/ || $attributesHashRef->{ITEM} =~ /DEFFILE/))
									{
									# Both DEFFILE and PRJ_[TEST]MMPFILE entries may be specifed without extension
									$replacement =~ s/\.\w+$// if ($problematicText !~ /\.\w+$/);

									# DEFFILE entries may have eabi\bwins in the physical match and had a "u" inserted or appended 
									if ($attributesHashRef->{ITEM} =~ /DEFFILE/ && $replacement !~ /$modifiedSearchText$/i)
										{
										$replacement =~ s/(eabi|bwins)\//~\//i if ($problematicText =~ /~[\\|\/]/);
										$replacement =~ s/u(\.\w+)?$/$1/i if ($attributesHashRef->{ITEM} !~ /NOSTRICTDEF/);
										}
									}

								$revisedText =~ s/$modifiedSearchText/$replacement/;
								}
							else
								{
								print ("ERROR: Can\'t perform physical consistency updates for:");
								
								my $originalWarningsHashRef = $attributesHashRef->{ORIGINAL_WARNINGS};
								foreach my $originalWarning (keys %{$originalWarningsHashRef})
									{
									print ("\t$originalWarning") if ($originalWarning =~ /case versus/);
									}

								print ("\n");
								}
							}
						}

		# Exclusion

					if ($attributesHashRef->{EXCLUSION})
						{
						my $exclusionListingSearch = quotemeta($attributesHashRef->{EXCLUSION_LISTING});						
						$revisedText =~ s/$exclusionListingSearch/$attributesHashRef->{EXCLUSION_LISTING}/i;
						}						

		# Slash
					if ($attributesHashRef->{UNIX_SLASH})
						{
						$revisedText =~ s/\\/\//g;
						$revisedText =~ s/\/\//\//g;		# Don't allow replacements that lead to "//" in paths
						}

		# Lowercase

					if ($attributesHashRef->{LOWERCASE})
						{
						$revisedText = lc ($revisedText);
						}

		# Default Export

					if ($attributesHashRef->{DEFAULT_EXPORT})
						{
						my $exportedFilename = lc (basename ($problematicText));							
						$revisedText .= " \/epoc32\/include\/".$exportedFilename;
						}

					$line =~ s/(\"|\<|^|\s){1}$attributesHashRef->{SEARCH_TEXT}/$1$revisedText/;
					}

				print ("\tUpdated  : $line") if ($verbose);
				}
				
			push @newSourceFile, $line;
			}

		close SOURCE_FILE;

		if ($debugUpdate)
			{
			# Don't touch the original source, but create two trees for easy comparison
				
			my $baseDir = dirname ($SOURCE_FILE);				
			mkpath ("\\compare\\orig".$baseDir);
			mkpath ("\\compare\\updated".$baseDir);
			print ("Copying  \'\\compare\\orig".$SOURCE_FILE."\'...\n");
			copy ($SOURCE_FILE, "\\compare\\orig".$SOURCE_FILE);
			$SOURCE_FILE = "\\compare\\updated".$SOURCE_FILE;
			print ("Updating \'$SOURCE_FILE\'...\n");
			}

		if (!(open SOURCE_FILE, "> $SOURCE_FILE"))
			{				
			print ("ERROR: Could not open $SOURCE_FILE to write.\n");
			next;
			}

		foreach my $line (@newSourceFile)
			{
			print (SOURCE_FILE $line);
			}

		close SOURCE_FILE;
		}

	print ("\n") if ($verbose);
	}

__END__

:endofperl
