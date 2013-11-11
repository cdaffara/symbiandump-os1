# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#

use strict;
use LWP::Simple;
use TestScriptResults;
use Getopt::Std;
use File::Find;

# Mistral tags the result files from test scripts in a particular format, depending on the test harness used to execute them. 
# We use these names to identify which files are results files.
my $KMistralCoreConfFileName = "-coreconf-";
my $KMistralTEFFileName = "-testexecute-";
my $KMistralTestFrameworkFileName = "-testframework"; # No trailing hyphen due to different security variants

#  It appears that Mistral uses a completely different file format for results from HW runs
my $KMistrealTEFFileNameHW = "testexecute";
my $KMistrealTFFileNameHW1 = "testframework";
my $KMistrealTFFileNameHW2 = "testframeworkmmddcap";
my $KMistrealTFFileNameHW3 = "testframeworkuecap";
my $KMistrealTFFileNameHW4 = "testframeworknone";

# Work in progress script outputs.
my $iKnownFails;
my $iUnknownFailsButKnownInRef;
my $iUnknownFailsButKnownInOtherCodelines;
my $iUnknownFails;
my $iMissingTests; 	

my $iVerbose;

###############################################
##	Source test domain specific functionality
###############################################
# Scans the test results summary page from the Mistral build and compiles a collection of the 
# URL paths for the log files.
# param1 - the URL to the Test Results HTML Summary page of the Mistral build
# returns - the array of logs' URLS
sub GetResultFilesFromMistral
	{
	my $aMistralTestResultsSummaryUrl = shift;

	my @fileNames;
	my $nextFileNamesIndex = 0;

	my $summaryContents = get "$aMistralTestResultsSummaryUrl"; # Returns undef if failure
	unless ($summaryContents)
		{
		print "\nERROR: Unable to retrieve source summary file from $aMistralTestResultsSummaryUrl\n";
		exit;
		}
	
	#Could add an optimisation to this to search for the <td class="error"> tag BUT doesn't take into account the crashed tests and would need to check here the pass/fail count
	#In addition this might lead to migration problems with Sirroco and certainly isn't appliable to developer's personal builds where we must scan each file
	#So it's simpler to do the brute force approach and keep all implementations alike, it doesn't take long anyway
	while ($summaryContents =~ s/href="(.*?($KMistralCoreConfFileName|$KMistralTEFFileName|$KMistralTestFrameworkFileName).*?\.html?)"//i)
		{
		$fileNames[$nextFileNamesIndex] = $1;
		$nextFileNamesIndex++;
		}

	# Scan this second so we don't accidentally cut off the filenames if they are matched against Winscw runs
	while ($summaryContents =~ s/href="(.*?($KMistrealTEFFileNameHW|$KMistrealTFFileNameHW1|$KMistrealTFFileNameHW2|$KMistrealTFFileNameHW3|$KMistrealTFFileNameHW4).*?\.html?)"//i)
		{
		$fileNames[$nextFileNamesIndex] = $1;
		$nextFileNamesIndex++;
		}
	
	unless ($fileNames[0])
		{
		print "\nERROR: No test result log files found, please ensure the -s parameter points to Mistral's TestResults.html summary page\n";	
		exit;
		}
		
	return @fileNames;
	}

# Parse the Mistral test results summary page and generate an array of TestScriptResults
# objects which contain all the required information such as script names, test counts
# and failures.
# param1 - the URL of the Test Results HTML Summary page for the Mistral build 
# returns - the array of parsed log data
sub PopulateResultsArrayFromMistral
	{
	my $aTestResultsSummaryPageUrl = shift;
	my @aResultsArray = ();
	my $nextFreeIndexInResultsArray = 0;
	
	my @fileNames = GetResultFilesFromMistral($aTestResultsSummaryPageUrl);
	foreach my $fileName (@fileNames)
		{
		my $testFileContents = get "$fileName";
		unless ($testFileContents)
			{
			print "\nERROR: Unable to open logfile $fileName\n";
			next;
			}
			
		my $scriptResults = TestScriptResults->TestScriptResults();
		$scriptResults->SetFilePath($fileName);		
		if (($fileName =~ m/.*?$KMistralTEFFileName(.*?)\.html?/i) || ($fileName =~ m/.*?$KMistrealTEFFileNameHW(.*?)\.html?/i))
			{
			$scriptResults->SetScriptName($1);
			if (!($testFileContents =~ m/TEST CASE SUMMARY:/i) && ($testFileContents =~ m/SUMMARY:/i))
				{
				# One of the old MM TEF tests which didn't use testcases
				$scriptResults->TEFNoTestcasesTest();
				$aResultsArray[$nextFreeIndexInResultsArray] = ScanTEFLogFileForFailures($scriptResults, \$testFileContents, 0);
				}
			else
				{
				$scriptResults->TEFTest();
				$aResultsArray[$nextFreeIndexInResultsArray] = ScanTEFLogFileForFailures($scriptResults, \$testFileContents, 1);
				}	
			}
		elsif ($fileName =~ m/.*?$KMistralCoreConfFileName(.*?)\.txt\.html?/i)
			{
			$scriptResults->SetScriptName($1);
			$scriptResults->CoreConfTest();
			$aResultsArray[$nextFreeIndexInResultsArray] = ScanCoreConfLogFileForFailures($scriptResults, \$testFileContents);
			}
		elsif (($fileName =~ m/.*?$KMistralTestFrameworkFileName[^\-]*?\-(.*?)\.html?/i) || ($fileName =~ m/.*?$KMistrealTFFileNameHW2(.*?)\.html?/i) || ($fileName =~ m/.*?$KMistrealTFFileNameHW3(.*?)\.html?/i) || ($fileName =~ m/.*?$KMistrealTFFileNameHW4(.*?)\.html?/i) || ($fileName =~ m/.*?$KMistrealTFFileNameHW1(.*?)\.html?/i))
			{
			$scriptResults->SetScriptName($1);
			$scriptResults->TestFrameworkTest();
			
			$aResultsArray[$nextFreeIndexInResultsArray] = ScanTestFrameworkLogFileForFailures($scriptResults, \$testFileContents);
			}
		else
			{
			print "\nWARNING: Results file has unrecognised format - $fileName.\n";			
			}
		$nextFreeIndexInResultsArray++;	
		}
	
	return \@aResultsArray;
	}
	

# Walk the file path provided by the developer for his local machine, parse test logs
#  and generate an array of TestScriptResults objects which contain all the required 
# information such as script names, test counts and failures.
# param1 - the pathname of a folder containing the test results
# returns - the array of parsed log data
sub PopulateResultsArrayFromLocalMachine
	{
	my $aTestResultsPath = shift;
	my @aResultsArray = ();
	my $nextFreeIndexInResultsArray = 0;

	my @directories = ("$aTestResultsPath");
	my @fileNames;
	find( sub { push @fileNames, $File::Find::name if /\.html?$/ }, @directories );

	foreach my $fileName (@fileNames)
		{
		my $testFileContents = do { local $/; open(I,"$fileName"); <I> };
		my $scriptResults = TestScriptResults->TestScriptResults();
		$scriptResults->SetFilePath($fileName);
		$fileName =~ m/([^\\\/\.]*)\.[^\\\/]*\Z/;
		$scriptResults->SetScriptName($1);
		
		if ($testFileContents =~ m/Core Loader Conformance Suite/)
			{
			$scriptResults->CoreConfTest();
			$aResultsArray[$nextFreeIndexInResultsArray] = ScanCoreConfLogFileForFailures($scriptResults, \$testFileContents);
			}
		elsif ($testFileContents =~ m/TestFrameworkMain.cpp/)
			{
			$scriptResults->TestFrameworkTest();
			$aResultsArray[$nextFreeIndexInResultsArray] = ScanTestFrameworkLogFileForFailures($scriptResults, \$testFileContents);
			}
		elsif ($testFileContents =~ m/TEF Version/)
			{
			if ($testFileContents =~ m/TEST CASE SUMMARY:/i)
				{
				$scriptResults->TEFTest();
				$aResultsArray[$nextFreeIndexInResultsArray] = ScanTEFLogFileForFailures($scriptResults, \$testFileContents, 1);
				}
			else
				{
				# One of the old MM TEF tests which didn't use testcases
				$scriptResults->TEFNoTestcasesTest();
				$aResultsArray[$nextFreeIndexInResultsArray] = ScanTEFLogFileForFailures($scriptResults, \$testFileContents, 0);
				}			
			}
		else
			{
			print "\nWARNING: Results file has unrecognised format - $fileName.\n";	
			}
					
		$nextFreeIndexInResultsArray++;	
		}
	
	return \@aResultsArray;
	}
	
###############################################
##	Test harness specific functionality
###############################################	

# Parses a TF log file for failures, inconclusives and crashes
# param1 - the TestScriptResults object to populate
# param2 - reference to the contents of the log file
# returns - the TestScriptResults object containing the parsed data
sub ScanTestFrameworkLogFileForFailures
	{
	my ($aScriptResults, $aLogFileContents) = @_;
	
	my $testFileContents = $$aLogFileContents;
	my $numberOfTests = 0;
	# Search for test case results, which take the following form:
	#04/09/2002 15:51:39:772    V   Log.cpp 736 Test Result for TSI_ICL_BTRANS_01:MM_ICL_BTRANS_I_0202_CP is PASS
	while ($testFileContents =~ s/Test[\s]+Result[\s]+for[\s]+[\S]+:([\_|\-|\S]+)[\s]+is[\s]+([\_|\-|\S]+)[\s]*\<\/font\>//)
		{
		my $testId  = $1;
	    my $result = $2;
	    $numberOfTests++;
		if ($result =~ /PASS/)
			{
			# Do nothing
			}
		elsif ($result =~ /INCONCLUSIVE/)
			{
			$aScriptResults->AddInconclusiveResult($testId);
			}
		else
			{
			# Treat all other results as failure
			$aScriptResults->AddFailureResult($testId);
			}
		}
	$aScriptResults->SetTotalTestCount($numberOfTests);
	
	unless ($testFileContents =~ m/Test Results Summary/)
		{
		# Test file summary not in the log file - the test has crashed
		$aScriptResults->TestCrashed();
		}
	
	return $aScriptResults;
	}

# Parses a TEF log file for failures, inconclusives and crashes
# param1 - the TestScriptResults object to populate
# param2 - reference to the contents of the log file
# param3 - boolean whether the test is using testcases (older MM tests didn't, in which case we use the test steps)
# returns - the TestScriptResults object containing the parsed data
sub ScanTEFLogFileForFailures
	{
	my ($aScriptResults, $aLogFileContents, $aTestcasesInUse) = @_;
	
	my $testFileContents = $$aLogFileContents;
	my $numberOfTests = 0;

	# Search for test case results, which take the following form:
	# 02:56:42:145 c:\mm\tsu_3gp_compose_api_te.script Line = 58 Command = END_TESTCASE MM-3GP-COMP-U-0003-CP ***TestCaseResult = PASS 
	# Or if testcases not in use in this script file test step results, which take the following form:
	# 14:20:51:459 c:\mm\tsu_mmf_aclnt_securedrmtestdata.script Line = 36 Command = RUN_TEST_STEP 1000 RTAUtils ImportDrmArchive c:\mm\tsu_mmf_aclnt_securedrmtestdata.ini ImportSmallWavDrmArchiveAudio ***Result = UNEXECUTED 
	while (($testFileContents =~ s/Command[\s]+=[\s]+END\_TESTCASE[\s]+([\_|\-|\S]+)[\s]+\*\*\*TestCaseResult[\s]+=[\s]+([\_|\-|\S]+)[\s]*\<\/font\>//)
			|| (!$aTestcasesInUse && ($testFileContents =~ s/Command[\s]+=[\s]+RUN\_TEST\_STEP[\s]+\d*[\s]+.*?[\s]+([\_|\-|\S]+)[\s]+.*?\*\*\*Result[\s]+=[\s]+([\_|\-|\S]+)[\s]*\<\/font\>//)))
		{
		my $testId  = $1;
	    my $result = $2;
	    $numberOfTests++;
		if ($result =~ /PASS/)
			{
			# Do nothing
			}
		elsif ($result =~ /INCONCLUSIVE/)
			{
			$aScriptResults->AddInconclusiveResult($testId);
			}
		else
			{
			# Treat all other results as failure
			$aScriptResults->AddFailureResult($testId);
			}
		}
	$aScriptResults->SetTotalTestCount($numberOfTests);
	
	# Testcase scripts use TEST CASE SUMMARY, non-testcase scripts just use SUMMARY
	unless ($testFileContents =~ m/SUMMARY:/)
		{
		# Test file summary not in the log file - the test has crashed
		$aScriptResults->TestCrashed();
		}
	
	return $aScriptResults;	
	}

# Parses a CoreConf log file for failures, inconclusives and crashes
# param1 - the TestScriptResults object to populate
# param2 - reference to the contents of the log file
# returns - the TestScriptResults object containing the parsed data	
sub ScanCoreConfLogFileForFailures
	{
	my ($aScriptResults, $aLogFileContents) = @_;
	my $testFileContents = $$aLogFileContents;
	
	if ($testFileContents =~ s/\*\*\*\s*?Summary of tests executed(.*\n)*//)
		{
		my $numberOfTests = 0;

		# Parse the summary listings
		while($testFileContents =~ s/\*\*\*\s*?Passed tests:((.*\n)*?)\*\*\*\s*?Failed tests:.*\n((\*\*\*.*\n)*)//)
			{
			my $passingTests = $1;
			my $failingTests = $3;
			
			# Passing tests
			while ($passingTests =~ s/\*\*\*[\s]*?\S+[\s]*\n//)
				{
				$numberOfTests++;
				}
			
			# Failing tests
			while ($failingTests =~ s/\*\*\*[\s]*?(\S+)[\s]*\n//)
				{
				$aScriptResults->AddFailureResult($1);
				$numberOfTests++;
				}			
			}

		$aScriptResults->SetTotalTestCount($numberOfTests);		
		}
	else
		{
		# Test file summary not in the log file - the test has crashed
		$aScriptResults->TestCrashed();		
		}
	
	return $aScriptResults;		
	}

# Gets the test case count from a Test Execute Framework log file
# param1 - a reference to the contents of the TEF log file
#returns - The number of test cases in the script or -1 if the summary could not be found
sub GetTEFTestCount
	{
	my $aReferenceLogContents = shift;
	my $refContents = $$aReferenceLogContents;

	if ($refContents =~ m/TEST CASE SUMMARY:(.*\n)*?.*?PASS =\s*(\d*)(.*\n)*?.*?FAIL =\s*(\d*)(.*\n)*?.*?INCONCLUSIVE =\s*(\d*)/)
		{
		my $result = $2 + $4 + $6;
		return $result;
		}
	
	if ($refContents =~ m/SUMMARY:(.*\n)*?.*?PASS =\s*(\d*)(.*\n)*?.*?FAIL =\s*(\d*)(.*\n)*?.*?ABORT =\s*(\d*)(.*\n)*?.*?PANIC =\s*(\d*)(.*\n)*?.*?INCONCLUSIVE =\s*(\d*)(.*\n)*?.*?UNKNOWN =\s*(\d*)(.*\n)*?.*?UNEXECUTED =\s*(\d*)/)	
		{
		# One of the MM tests that doesn't use testcases
		my $result = $2 + $4 + $6 + $8 + $10 + $12 + $14;
		return $result;
		}									
	
	# Summary not found, we crashed
	return -1;
	}

# Gets the test count from a Test Framework log file
# param1 - a reference to the contents of the Test Framework log file
#returns - The number of test cases in the script or -1 if the summary could not be found	
sub GetTestFrameworkTestCount
	{
	my $aReferenceLogContents = shift;
	my $refContents = $$aReferenceLogContents;
	
	unless ($refContents =~ m/Test Results Summary(.*\n)*?.*?Total\s*:(\d*)\s*\n/)
		{
		# Summary not found, we crashed
		return -1;
		}
	return $2;
	}

# Gets the test count from an OpenMAX IL Core Conformance log file
# param1 - a reference to the contents of the Core Conformance log file
#returns - The number of test cases in the script or -1 if the summary could not be found	
sub GetCoreConfTestCount
	{
	my $aReferenceLogContents = shift;
	my $refContents = $$aReferenceLogContents;
	
	unless ($refContents =~ m/\*\*\*\s*Summary of tests executed(.*\n)*?\*\*\*\s*Total :\s*(\d*)\s*\n/)
		{
		# Summary not found, we crashed
		return -1;
		}
	return $2;
	}

# Scans a TEF log looking for the test specified and checks if it returned inconclusive
# param1 - the test name
# param2 - reference to the contents of the log file, the test case result is removed from the argument if passed by ref
# param3 - whether the TEF script uses testcases
# returns - 1 if it returned inconclusive, 0 otherwise	
sub MatchingTEFInconclusiveResult	
	{
	my $aInconclusive = shift;
	my $aReferenceLogContents = shift;
	my $aTestcasesInUse = shift;
	my $refContents = $$aReferenceLogContents;
	if ($aTestcasesInUse)
		{
		if ($refContents =~ s/Command[\s]+=[\s]+END\_TESTCASE[\s]+$aInconclusive[\s]+\*\*\*TestCaseResult[\s]+=[\s]+([\_|\-|\S]+)[\s]*\<\/font\>//)
			{
			if ($1 eq "INCONCLUSIVE")
				{
				return 1;
				}
			}
		}
	else
		{
		if ($refContents =~ s/Command[\s]+=[\s]+RUN\_TEST\_STEP[\s]+\d*[\s]+.*?[\s]+$aInconclusive[\s]+.*?\*\*\*Result[\s]+=[\s]+([\_|\-|\S]+)[\s]*\<\/font\>//)
			{
			if ($1 eq "INCONCLUSIVE")
				{
				return 1;
				}
			}
		}
	return 0;
	}

# Scans a TestFramework log looking for the test specified and checks if it returned inconclusive
# param1 - the test name
# param2 - reference to the contents of the log file, the test case result is removed from the argument if passed by ref
# returns - 1 if it returned inconclusive, 0 otherwise		
sub MatchingTFInconclusiveResult 
	{
	my $aInconclusive = shift;
	my $aReferenceLogContents = shift;
	my $refContents = $$aReferenceLogContents;
	if ($refContents =~ s/Test[\s]+Result[\s]+for[\s]+[\S]+:$aInconclusive[\s]+is[\s]+([\_|\-|\S]+)[\s]*\<\/font\>//)
		{
		if ($1 eq "INCONCLUSIVE")
			{
			return 1;
			}
		}
	return 0;
	}

# Scans a TEF log looking for the test specified and checks if it returned an error
# param1 - the test name
# param2 - reference to the contents of the log file, the test case result is removed from the argument if passed by ref
# param3 - whether the TEF script uses testcases
# returns - 1 if it returned an error, 0 otherwise		
sub MatchingTEFErrorResult 
	{
	my $aFail = shift;
	my $aReferenceLogContents = shift;
	my $aTestcasesInUse = shift;
	my $refContents = $$aReferenceLogContents;
	
	if ($aTestcasesInUse)
		{
		if (not $refContents =~ s/Command[\s]+=[\s]+END\_TESTCASE[\s]+$aFail[\s]+\*\*\*TestCaseResult[\s]+=[\s]+([\_|\-|\S]+)[\s]*\<\/font\>//)
			{
			return 0;
			}
	
		if (($1 eq "PASS") || ($1 eq "INCONCLUSIVE"))
			{
			return 0;
			}
		}
	else
		{
		if (not $refContents =~ s/Command[\s]+=[\s]+RUN\_TEST\_STEP[\s]+\d*[\s]+.*?[\s]+$aFail[\s]+.*?\*\*\*Result[\s]+=[\s]+([\_|\-|\S]+)[\s]*\<\/font\>//)
			{
			return 0;
			}
	
		if (($1 eq "PASS") || ($1 eq "INCONCLUSIVE"))
			{
			return 0;
			}
		}
		
	return 1;
	}	
	
# Scans a TestFramework log looking for the test specified and checks if it returned an error
# param1 - the test name
# param2 - reference to the contents of the log file, the test case result is removed from the argument if passed by ref
# returns - 1 if it returned an error, 0 otherwise		
sub MatchingTFErrorResult 
	{
	my $aFail = shift;
	my $aReferenceLogContents = shift;
	my $refContents = $$aReferenceLogContents;
	if (not $refContents =~ s/Test[\s]+Result[\s]+for[\s]+[\S]+:$aFail[\s]+is[\s]+([\_|\-|\S]+)[\s]*\<\/font\>//)
		{
		return 0;
		}
	
	if (($1 eq "PASS") || ($1 eq "INCONCLUSIVE"))
		{
		return 0;
		}
		
	return 1;
	}	
	
# N.B. VERY IMPORTANT that the log file is passed by reference.  Core conf scripts can include numerous
# different tests that use the same test name so to avoid false positives from this function the 
# substitution done in the comparison MUST affect the caller's copy of the log file.
sub MatchingCoreConfErrorResult
	{	
	my $aFail = shift;
	my $aReferenceLogContents = shift;
	my $refContents = $$aReferenceLogContents;
	
	if ($refContents =~ s/\*\*\*[\s]*$aFail[\s]*FAILED//)
		{
		return 1;
		}

	return 0;	
	}
	
###############################################		
##	Failure comparison functions
###############################################		

# The analysis takes an array of TestScriptResults, each storing the results of a test script's log file.
# It processes this list in stages looking for any scripts with errors and trying to match them against
# entries in a Known Failure sheet and/or comparison reference build.  Whenever a match is found
# the error is removed from the TestScriptResults object, these essentially act as an object containing
# unresolved errors.  When errors are matched their details are added to the corresponding text 
# variables at the top of this file (e.g. iKnownFails).
sub AnalyseFailures
	{
	my ($aKFSheetLocation, $aReferenceMistralURL, $aIgnoreFailsNotInRef, $aResultsRef, $aKfPlatforms) = @_;

	my @results = @$aResultsRef;
	
	my $refContents = undef;
	if ($aReferenceMistralURL)
		{
		$refContents = get "$aReferenceMistralURL"; # Returns undef if failure
		unless ($refContents)
			{
			print "\nERROR: Unable to retrieve reference summary file from $aReferenceMistralURL\n";
			exit;
			}		
		}
	
	my $kfSheetContents = undef;
	if ($aKFSheetLocation)
		{
		unless ($aKFSheetLocation =~ m/\.xml\Z/i)
			{
			print "\nERROR: KF sheet ($aKFSheetLocation) does not appear to be in XML format, you should save the Excel worksheet as XML Spreadsheet 2003 format for input to this script.\n";	
			exit;
			}		
		$kfSheetContents = do { local $/; open(I,"$aKFSheetLocation"); <I> };
		unless ($kfSheetContents)
			{
			print "\nERROR: Unable to open KF sheet at $aKFSheetLocation\n";	
			exit;			
			}
		}
		
	foreach my $scriptResults (@results)
		{
		my $scriptName = $scriptResults->ScriptName();
		
		my $referenceResults = undef;
		
		# If a reference build was provided, find the corresponding log file
		if ($refContents)
			{
			# We use substitution here because some of the coreconf test scripts
			# have the same script name.  This ensures we don't keep referencing
			# against the same copy.  We cannot do the comparison on the script 
			# path because this isn't resilient to adding new tests or cross domain 
			# comparisons.
			# N.B. This assumes the test list ordering remains the same.
			# N.B. The optional .txt is for Core Conf tests
			$refContents =~ s/href="(.*?$scriptName(\.txt)?(\.script)?\.html?)"//;
			my $file = $1;
			if ($file eq undef)
				{
				if ($aIgnoreFailsNotInRef)
					{
					next;
					}
				print "\nWARNING: Unable to find $scriptName in Reference Build\n";	
				}
			else
				{
				$referenceResults = get "$file";	# returns undef on failure
				unless ($referenceResults)
					{
					print "\nWARNING: Unable to open $scriptName ($file) in Reference Build\n";			
					}
				}
			}
		
		my $refTestCrashed;
		# Check the test count hasn't decreased
		if ($referenceResults)
			{
			$refTestCrashed = CheckTestCount($scriptResults, \$referenceResults);
			}
			
		if (not $scriptResults->AnyFailures())
			{
			# No errors so move onto the next
			next;
			}
		
		if ($kfSheetContents)
			{
			ResolveFailuresAgainstKFSheet($scriptResults, \$kfSheetContents, $aKfPlatforms); 
		
			if (not $scriptResults->AnyFailures())
				{
				# All errors resolved against KF sheet so move onto the next
				next;
				}
			}	
		
		if ($referenceResults)
			{
			ResolveFailuresAgainstReferenceRun($scriptResults, $refTestCrashed, \$referenceResults);
			
			if (not $scriptResults->AnyFailures())
				{
				# All errors resolved against reference run so move onto the next
				next;
				}			
			}
		
		# Unknown failures
		$iUnknownFails = $iUnknownFails . "\n$scriptName\n";
		
		my $inconText;
		my @inconclusives = $scriptResults->Inconclusives();
		foreach my $inconclusive (@inconclusives)
			{
			$inconText = $inconText . "$inconclusive\n";
			}
		my $failText;
		my @failures = $scriptResults->Failures();
		foreach my $failure (@failures)
			{
			$failText = $failText . "$failure\n";
			}	
			
		if ($inconText)
			{
			$iUnknownFails = $iUnknownFails . "INCONCLUSIVES:\n$inconText";
			}
		if ($failText)
			{
			$iUnknownFails = $iUnknownFails . "FAILS:\n$failText";
			}
		if ($scriptResults->DidItCrash())	
			{
			$iUnknownFails = $iUnknownFails . "CRASHED\n";
			}
		}	
	
	}

# Will compare the test count of the test in question against the equivalent result from a reference build's log
# to detect if there has been a reduction in the total number of tests run as part of the script, unless that script 
# has crashed.  As a side effect the return value indicates whether the test in question crashed or not (saves
# having to scan it twice).
# param1 - the log results to compare against the reference build's run
# param2 - the contents of the reference build's corresponding test log
# returns = 1 if the reference script crashed and we were unable to compare the counts, 0 otherwise
sub CheckTestCount
	{
	my ($aTestResultsObject, $aReferenceLogContents) = @_;
	
	my $testHarness = $aTestResultsObject->TestHarness();
	my $refTestCount;
	if (($testHarness eq $TestScriptResults::KTEFTest) || ($testHarness eq $TestScriptResults::KTEFNoTestcasesTest))
		{
		$refTestCount = GetTEFTestCount($aReferenceLogContents);
		}
	elsif ($testHarness eq $TestScriptResults::KTestFrameworkTest)
		{
		$refTestCount = GetTestFrameworkTestCount($aReferenceLogContents);
		}
	else
		{
		$refTestCount = GetCoreConfTestCount($aReferenceLogContents);
		}
	
	if ($refTestCount < 0)
		{
		# Reference test crashed
		unless ($aTestResultsObject->DidItCrash())
			{
			my $scriptName = $aTestResultsObject->ScriptName();
			print "\nWARNING: $scriptName crashed in the reference build, unable to compare test counts\n";
			}
		return 1;
		}
	
	my $testCount = $aTestResultsObject->TotalTestCount;
	if ($testCount < $refTestCount)
		{
		unless ($aTestResultsObject->DidItCrash())
			{
			my $testName = $aTestResultsObject->ScriptName();
			$iMissingTests = $iMissingTests . "$testName  Previous = $refTestCount, Current = $testCount\n"; 	
			}
		}
	return 0;	
	}	

# Will scan the Known Failure sheet for any entries indicating if the failures,inconclusives and/or crash
# witnessed in the script in question are already known. If so these will be appended to the known failures list
# and removed from the TestScriptResults object in question. If matching entries were found but existed for
# other codelines then this information is stored but the failures are not removed from the TestScriptResults 
# object.
# param1 - TestScriptResults object for the script with failures
# param2 - the contents of the known failure sheet
# param3 - the array of platform entries in the known failure sheet that are applicable to this run
sub ResolveFailuresAgainstKFSheet
	{
	my ($aScriptWithProblems, $aKfSheetContents, $aKfPlatforms) = @_;
	my $kfSheetContents = $$aKfSheetContents;
	my $scriptName = $aScriptWithProblems->ScriptName();
	
	my @kFApplicablePlatforms;
	if ($aKfPlatforms)
		{
		# Will error if undef
		@kFApplicablePlatforms = @$aKfPlatforms;
		}

	# Modified version of the KF Sheet contents, at the end of this function it will contain the Rows from the KF Sheet with any that correspond to the current test script stripped out.
	# This is a by product of the fact that we inspect each row one at a time looking for any that are relevant, trying to do a global search results in potentially error prone and difficult to maintain
	# regexs.  And in reality underneath Perl shouldn't be doing that much in the way of optimisation versus this approach.
	my $wipKfSheet;
	my $candidatesForOtherPlatforms;
	my $foundSamePlatFails;
	my $foundSamePlatInconcs;
	my $foundSamePlatCrash;
	while ($kfSheetContents =~ s/<Row((.|\n)*?>)((.|\n)*?)<\/Row>//i)
		{
		my $row = $3;
		# Optimisation
		unless ($row =~ m/$scriptName/i)
			{
			$wipKfSheet = $wipKfSheet . "<Row>$row<\/Row>";
			next;
			}
		
		# None of the cells prior to the 'Scriptname' named cell are of any interest, now verify this row is actually for 
		# this test script and we didn't pick the hit up off the entries for one of the other fields, e.g. Remarks
		my $applicable;
		while ($row =~ s/(<Cell(.|\n)*?<\/Cell>)//i)
			{
			my $cell = $1;
			if ($cell =~ m/<NamedCell\s*\n?\s*ss:Name="Scriptname\d*"\//i)
				{
				# Named field we're looking for
				if ($cell =~ m/$scriptName/i)
					{
					$applicable = 1;
					last;
					}
				else
					{
					$wipKfSheet = $wipKfSheet . "<Row>$cell\n$row<\/Row>";	# This entry isn't applicable to us, reinsert it into the KF sheet
					last;
					}
				}
			}
		unless ($applicable)
			{
			next;
			}
		
		# We are now dealing with a Row corresponding to the script in question
		# So pull out the relevant named cells contents
		my $crash;
		my $fails;
		my $inconclusives;
		my $targets;
		while ($row =~ s/(<Cell(.|\n)*?<\/Cell>)//i)
			{
			my $cell = $1;
			if ($cell =~ m/<NamedCell\s*\n?\s*ss:Name="CrashedStatus\d*"\//i)
				{
				$crash = $cell;
				}
			elsif ($cell =~ m/<NamedCell\s*\n?\s*ss:Name="Fails\d*"\//i)
				{
				$fails = $cell;			
				}
			elsif ($cell =~ m/<NamedCell\s*\n?\s*ss:Name="Inconclusives\d*"\//i)
				{
				$inconclusives = $cell;			
				}
			elsif ($cell =~ m/<NamedCell\s*\n?\s*ss:Name="Platforms\d*"\//i)
				{
				$targets = $cell;			
				}				
			}
			
		my $platformMatched;
		foreach my $platform (@kFApplicablePlatforms)
			{
			unless (($targets =~ m/$platform&#10;/i) || ($targets =~ m/$platform<\/Data>/i))	# To ensure for example that All isn't picked up by an All Winscw entry
				{
				next;	# Platform not found in the entry
				}		
			$platformMatched = 1;

			if (($aScriptWithProblems->DidItCrash()) && ($crash =~ m/Crashed/i))
				{
				$foundSamePlatCrash = 1;
				$aScriptWithProblems->ResetCrashed();
				}
				
			if ($fails)
				{
				my @failures = $aScriptWithProblems->Failures();
				$aScriptWithProblems->ResetFailures();
				foreach my $failure (@failures)
					{
					if (($fails =~ m/$failure/i) || ($fails =~ m/>ALL<\/Data>/))
						{
						$foundSamePlatFails = $foundSamePlatFails . "$failure\n";
						}
					else
						{
						
						$aScriptWithProblems->AddFailureResult($failure);
						}
					}
				}

			if ($inconclusives)
				{
				my @incons = $aScriptWithProblems->Inconclusives();
				$aScriptWithProblems->ResetInconclusives();
				foreach my $incon (@incons)
					{
					if (($inconclusives =~ m/$incon/i) || ($inconclusives =~ m/>ALL<\/Data>/))
						{
						$foundSamePlatInconcs = $foundSamePlatInconcs . "$incon\n";
						}
					else
						{
						$aScriptWithProblems->AddInconclusiveResult($incon);
						}
					}
				}
			}	# End of platform matching loop
		
		unless ($platformMatched)
			{
			# The row entry did not match any of the applicable platforms.  We need to keep checking for more appropriate rows.
			# However, if after the KF Sheet has been parsed we still have unknown errors, we need to compare against these rows as well
			# in case the issue has been seen on other platforms.
	
			$candidatesForOtherPlatforms = $candidatesForOtherPlatforms . "<Row>$crash$fails$inconclusives<\/Row>";
			}
			
		}	# End of row scanning
	

	my $foundOtherPlatFails;
	my $foundOtherPlatInconcs;
	my $foundOtherPlatCrash;	
	if ($aScriptWithProblems->AnyFailures())	
		{
		# Failures remain, potentially matched against the rows for other platforms
		
		while ($candidatesForOtherPlatforms =~ s/<Row>((.|\n)*?)<\/Row>//i)
			{
			my $row = $1;
			my $crash;
			my $fails;
			my $inconclusives;
			while ($row =~ s/(<Cell(.|\n)*?<\/Cell>)//i)
				{
				my $cell = $1;
				if ($cell =~ m/<NamedCell\s*\n?\s*ss:Name="CrashedStatus\d*"\//i)
					{
					$crash = $cell;
					}
				elsif ($cell =~ m/<NamedCell\s*\n?\s*ss:Name="Fails\d*"\//i)
					{
					$fails = $cell;
					}
				elsif ($cell =~ m/<NamedCell\s*\n?\s*ss:Name="Inconclusives\d*"\//i)
					{
					$inconclusives = $cell;
					}					
				}	# End of stripping out content details
			
			if (($aScriptWithProblems->DidItCrash()) && ($crash =~ m/Crashed<\/Data>/i))
				{
				$foundOtherPlatCrash = 1;
				}
				
			if ($fails)
				{
				my @failures = $aScriptWithProblems->Failures();
				foreach my $failure (@failures)
					{
					if (($fails =~ m/$failure/i) || ($fails =~ m/>ALL<\/Data>/))
						{
						$foundOtherPlatFails = $foundOtherPlatFails . "$failure\n";
						}
					}
				}

			if ($inconclusives)
				{
				my @incons = $aScriptWithProblems->Inconclusives();
				foreach my $incon (@incons)
					{
					if (($inconclusives =~ m/$incon/i) || ($inconclusives =~ m/>ALL<\/Data>/))
						{
						$foundOtherPlatInconcs = $foundOtherPlatInconcs . "$incon\n";
						}
					}
				}
				
			}	# End of cross-platform candidate matchine
		}	# End of dealing with potential cross-platform matches

	# Output known failures
	if ($foundSamePlatFails || $foundSamePlatInconcs || $foundSamePlatCrash)
		{
		$iKnownFails = $iKnownFails . "\n$scriptName\n";
		}
	if ($foundSamePlatInconcs)
		{
		$iKnownFails = $iKnownFails . "INCONCLUSIVES:\n$foundSamePlatInconcs";
		}
	if ($foundSamePlatFails)
		{
		$iKnownFails = $iKnownFails . "FAILS:\n$foundSamePlatFails";
		}
	if ($foundSamePlatCrash)
		{
		$iKnownFails = $iKnownFails . "CRASHED\n";
		}		
	
	# Output matches found only in other test platforms
	if ($foundOtherPlatFails || $foundOtherPlatInconcs || $foundOtherPlatCrash)
		{
		$iUnknownFailsButKnownInOtherCodelines = $iUnknownFailsButKnownInOtherCodelines .  "\n$scriptName\n";
		}
	if ($foundOtherPlatInconcs)
		{
		$iUnknownFailsButKnownInOtherCodelines = $iUnknownFailsButKnownInOtherCodelines . "INCONCLUSIVES:\n$foundOtherPlatInconcs";
		}
	if ($foundOtherPlatFails)
		{
		$iUnknownFailsButKnownInOtherCodelines = $iUnknownFailsButKnownInOtherCodelines . "FAILS:\n$foundOtherPlatFails";
		}
	if ($foundOtherPlatCrash)
		{
		$iUnknownFailsButKnownInOtherCodelines = $iUnknownFailsButKnownInOtherCodelines . "CRASHED\n";
		}
		
	$aKfSheetContents = $wipKfSheet;
	}


# Takes the corresponding log file from the reference build and test for a failing script and
# compare it to see if the failures match.  If so this information is stored and the matching
# failures removed from the TestScriptResults object.
# param1 - TestScriptResults object for the script with failures
# param2 - boolean whether the test crashed in the reference build and test
# param3 - the contents of the corresponding log file from the reference build
sub ResolveFailuresAgainstReferenceRun
	{
	my $aReferenceTestResults = shift;
	my $aRefTestCrashed = shift;
	my $aReferenceLogContents = shift;
	my $refContents = $$aReferenceLogContents;	

	my $scriptName = $aReferenceTestResults->ScriptName();
	my $testHarness = $aReferenceTestResults->TestHarness();
	
	my $inconText;
	my @inconclusives = $aReferenceTestResults->Inconclusives();
	$aReferenceTestResults->ResetInconclusives();
	foreach my $inconclusive (@inconclusives)
		{
		if (($testHarness eq $TestScriptResults::KTEFTest) || ($testHarness eq $TestScriptResults::KTEFNoTestcasesTest))
			{
			my $testcasesInUse = 0;
			if ($testHarness eq $TestScriptResults::KTEFTest)
				{
				$testcasesInUse = 1;
				}
			if (MatchingTEFInconclusiveResult($inconclusive, \$refContents, $testcasesInUse))
				{
				$inconText = $inconText . "$inconclusive\n";
				}
			else
				{
				$aReferenceTestResults->AddInconclusiveResult($inconclusive);
				}
			}
		elsif ($testHarness eq $TestScriptResults::KTestFrameworkTest)
			{
			if (MatchingTFInconclusiveResult($inconclusive, \$refContents))
				{
				$inconText = $inconText . "$inconclusive\n";
				}
			else
				{
				$aReferenceTestResults->AddInconclusiveResult($inconclusive);			
				}			
			}
		# Core Conf tests have no comprehension of inconclusive
		}
			
	my $failText;
	my @failures = $aReferenceTestResults->Failures();
	$aReferenceTestResults->ResetFailures();
	foreach my $failure (@failures)
		{
		if (($testHarness eq $TestScriptResults::KTEFTest) || ($testHarness eq $TestScriptResults::KTEFNoTestcasesTest))
			{
			my $testcasesInUse = 0;
			if ($testHarness eq $TestScriptResults::KTEFTest)
				{
				$testcasesInUse = 1;
				}
			if (MatchingTEFErrorResult($failure, \$refContents, $testcasesInUse))
				{
				$failText = $failText . "$failure\n";
				}
			else
				{
				$aReferenceTestResults->AddFailureResult($failure);
				}
			}
		elsif ($testHarness eq $TestScriptResults::KTestFrameworkTest)
			{
			if (MatchingTFErrorResult($failure, \$refContents))
				{
				$failText = $failText . "$failure\n";
				}
			else
				{
				$aReferenceTestResults->AddFailureResult($failure);
				}			
			}
		else
			{
			# Core Conf 
			if (MatchingCoreConfErrorResult($failure, \$refContents))
				{
				$failText = $failText . "$failure\n";
				}
			else
				{
				$aReferenceTestResults->AddFailureResult($failure);
				}			
			}
		}

	my $bothCrashed;
	if (($aReferenceTestResults->DidItCrash()) && $aRefTestCrashed)
		{
		$bothCrashed = 1;
		$aReferenceTestResults->ResetCrashed();
		}
	
	if ($inconText || $failText || $bothCrashed)
		{
		$iUnknownFailsButKnownInRef = $iUnknownFailsButKnownInRef . "\n$scriptName\n";
		}
	
	if 	($inconText)
		{
		$iUnknownFailsButKnownInRef = $iUnknownFailsButKnownInRef . "INCONCLUSIVES:\n$inconText";
		}
	if ($failText)
		{
		$iUnknownFailsButKnownInRef = $iUnknownFailsButKnownInRef . "FAILS:\n$failText";
		}
	if ($bothCrashed)
		{
		$iUnknownFailsButKnownInRef = $iUnknownFailsButKnownInRef . "CRASHED\n";
		}
	}
	
###############################################
##	Utility functions
###############################################	

sub PrintHelp
	{
	print "\n\nResultsComparison.pl -m Mode -s Results_source [-k Known_failure_sheet_path -c Codeline -p Platform] [-r Mistral_comparison_job_url] [-i] [-v]\n\n";
	print "  -m = Mode of operation, specify the source of the results that you are\n";
	print "    comparing. Currently supported modes: Mistral, Local\n\n";
	print "  -s = Source of the results, mode dependant:\n";
	print "      (i) Mistral = The URL of the test results HTML summary page\n";
	print "      (ii) Local = The pathname of a folder containing the test log files\n\n";
	print "  -k = Pathname of the KF sheet, this should be exported as an XML spreadsheet\n";
	print "    from Excel\n\n";
	print "  -r = Reference Mistral build, the URL of the test results HTML summary page.\n";
	print "    This can be used in addition or in place of the Known Failure sheet\n\n";
	print "  -i = Ignore test failures in scripts not found in the Reference Mistral build\n\n";
	print "  -c = An index indicating the codeline used in the test run, only used in\n";
	print "    conjunction with a KF sheet. Possible values: TB92, TB101, TB102\n\n";	
	print "  -p = An index indicating the platform the test run is from, only used in\n";
	print "    conjunction with a KF sheet. Possible values: Winscw, H6, H4 WDP,\n";
	print "    NaviEngine\n\n";
	print "  -v = Verbose output\n\n";
	}
	
###############################################
##	Main processing loop
###############################################		

# Read in parameters
my %aParams;
getopts('ihvm:s:k:r:c:p:', \%aParams);
if ($aParams{h})
	{
	PrintHelp();
	exit;
	}

$iVerbose = $aParams{v};
	
# The mode affects where we get the source files from and nothing else.
my @runResults;
if ($aParams{m} =~ m/\AMistral\Z/i)
	{
	@runResults = @{PopulateResultsArrayFromMistral($aParams{s})};	
	}
elsif ($aParams{m} =~ m/\ALocal\Z/i)
	{
	@runResults = @{PopulateResultsArrayFromLocalMachine($aParams{s})};	
	}
else
	{
	print "\nERROR: Operation mode absent or not recognised.\n";
	PrintHelp();
	exit;
	}
	
my @kfPlatforms;
if ($aParams{k})
	{
	$kfPlatforms[0] = "All";
	
	# KF sheet in use, codeline and platform arguments are required in order to search for matching failures
	my $codeline = $aParams{c};
	my $platform = $aParams{p};
	unless (($codeline eq "TB92") || ($codeline eq "TB101") || ($codeline eq "TB102"))
		{
		print "\nERROR: The codeline specified was not recognised, supported options: TB92, TB101, TB102.\n";
		exit;		
		}

	if ($platform eq "Winscw")
		{
		$kfPlatforms[1] = "All Winscw";
		}
	elsif (($platform eq "H6") || ($platform eq "H4 WDP") || ($platform eq "NaviEngine"))
		{
		$kfPlatforms[1] = "All HW";
		}
	else
		{
		print "\nERROR: The platform specified was not recognised, supported options: Winscw, H6, H4 WDP, NaviEngine.\n";
		exit;		
		}
	
	$kfPlatforms[2] = "$codeline $platform";
	}

if ($iVerbose)
	{
	print "\nLog files found:\n";
	foreach my $file (@runResults)
		{
		my $scriptName = $file->ScriptName();
		print "$scriptName\n";
		}
	}
	
AnalyseFailures($aParams{k}, $aParams{r}, $aParams{i}, \@runResults, \@kfPlatforms);

print "\n\nKnown failures:\n";
print "------------------\n";
print "$iKnownFails\n\n";

print "Unknown failures but found in the reference build:\n";
print "----------------------------------------------------\n";
print "$iUnknownFailsButKnownInRef\n\n";

print "Unknown failures but known in other codelines:\n";
print "------------------------------------------------\n";
print "$iUnknownFailsButKnownInOtherCodelines\n\n";

print "Unknown failures:\n";
print "-------------------\n";
print "$iUnknownFails\n\n";

print "Tests with reduced test counts:\n";
print "---------------------------------\n";
print "$iMissingTests\n\n";


