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
    eval 'exec perl.exe -S $0 ${1+"$@"}'
    if $running_under_some_shell;

use strict; # important pragma
use File::Copy;
use File::Find;


my @Month =("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec");
my ($sec,my $min,my $hour,my $mday,my $mon,my $year,my $wday,my $yday,my $isdst); 


my $BuildToUse="winscw";
my $EpocCDrive = "$ENV{EPOCROOT}epoc32\\$BuildToUse\\c";
my $ResultsDirectory = "$EpocCDrive\\Logs\\TestResults";
my $TestFrameworkOpts="";
my $TestFrameworkNoDelay="";
my $Debug=0;
my $Warning=0; #controls how pernickety the output is
my $Cmd; #general variable to carry system commands
my $CodeCover=0;
my $ScriptToRun='[\S]+\.script';
my $TimesToRun=1;


if(@ARGV && (($ARGV[0] eq "-h") ||
     ($ARGV[0] eq "--h") ||
     ($ARGV[0] eq "-help") ||
     ($ARGV[0] eq "--help") ||
     ($ARGV[0] eq "help"))
	 )
    {
    print "Runs all the Test Framework and Test Execute script files.\n";
    print "\nSyntax : runtests -b=sss -d -c -a -v=nnn -cc -rep=nnn -q\n";
	print "-b   Compiler version to use [wins\|winscw) (default=$BuildToUse)\n";
    print "-d   Force to run testframework at debug mode \n";
    print "-c   Clean results directory ($ResultsDirectory)\n";
    print "-a   Full output in testframework console\n";
    print "-v   Provide verbose output 1=limited 2=maximum\n";
    print "-s   Define a script to be executed (ommitting causes all scripts to be run)\n";
    print "-cc  Only run release builds for Code Cover\n";
    print "-rep Repeat the test run a number of times\n";
    print "-q      Start testframework without the 20 second delay\n";

    exit 0;
    }



use Getopt::Long;
my %optctl = ();
keys(%optctl)=10;
unless (GetOptions (\%optctl, "b=s","d","c","v=i", "cc", "s=s","rep=i","a","q")) 
  {exit 1;}



if($optctl{"b"})
  {
  $BuildToUse=$optctl{"b"};
  if((lc($BuildToUse) ne "wins") && (lc($BuildToUse) ne "winscw"))
    {
	print STDERR "only wins and winscw builds are supported\n";
	exit 1;
	}

   $EpocCDrive = "$ENV{EPOCROOT}epoc32\\$BuildToUse\\c";
   $ResultsDirectory = "$EpocCDrive\\Logs\\TestResults";
  }
#repeat test run a number of times
if($optctl{"rep"})
  {
  $TimesToRun = $optctl{"rep"};
  if(($TimesToRun < 1))
    {
    print STDERR "must specify a positive number of repeates\n";
    exit 1;
    }
  }


#debug on
if($optctl{"v"})
  {$Debug=$optctl{"v"};}

if($Debug ==2)
  {
  print "Command line options:-\n";
  while ((my $key, my $val) = each %optctl) 
    {print "$key  =>  $val\n";}
  }


#Code Cover run
if($optctl{"cc"})
  {$CodeCover=1;}


if($Debug ==2)
  {
  print "Command line options:-\n";
  while ((my $key, my $val) = each %optctl) 
    {print "$key  =>  $val\n";}
  }



if($optctl{"c"})
  {
  print "Deleting Existing Results FIles\n";

  my @ExistingResultsFiles;
  my $ExistingResultsFile;
  find( sub { push @ExistingResultsFiles, $File::Find::name if /\.htm/ }, ($ResultsDirectory) );


  foreach $ExistingResultsFile (@ExistingResultsFiles)
    {
    $ExistingResultsFile =~ s/\//\\/g;
    $Cmd="del $ExistingResultsFile";
    if($Debug==2)
	  {print "executing $Cmd\n";}
    system($Cmd);
	}

  }

if($optctl{"a"})
  {
  print "Full output in testframework console\n";
  $TestFrameworkOpts .= " -a";
  }

if($optctl{"s"})
  {
  $ScriptToRun = $optctl{"s"};
    
  print "Looking for script file $ScriptToRun\n";
  }

# Only add -q flag to textframework request
if($optctl{"q"})
	{
  	$TestFrameworkNoDelay .= " -q";
  	}

my $UrelTestFramework = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\urel\\testframework.exe" . $TestFrameworkOpts . $TestFrameworkNoDelay;
my $UdebTestFramework = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\udeb\\testframework.exe -t" . $TestFrameworkOpts . $TestFrameworkNoDelay;
my $RecogUrelTestFramework = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\urel\\testframeworkrecognizer.exe" . $TestFrameworkOpts . $TestFrameworkNoDelay;
my $RecogUdebTestFramework = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\udeb\\testframeworkrecognizer.exe" . $TestFrameworkOpts . $TestFrameworkNoDelay;
my $NoneUrelTestFramework = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\urel\\testframeworkNone.exe" . $TestFrameworkOpts . $TestFrameworkNoDelay;
my $NoneUdebTestFramework = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\udeb\\testframeworkNone.exe" . $TestFrameworkOpts . $TestFrameworkNoDelay;
my $MMDDCapUrelTestFramework = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\urel\\testframeworkMMDDCap.exe" . $TestFrameworkOpts . $TestFrameworkNoDelay;
my $UECapUrelTestFramework = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\urel\\testframeworkUECap.exe" . $TestFrameworkOpts . $TestFrameworkNoDelay;
my $UrelEpocEmulator  = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\urel\\epoc.exe" . $TestFrameworkOpts;
my $UdebEpocEmulator  = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\udeb\\epoc.exe" . $TestFrameworkOpts;
my $eka2IdentifyFile  = "$ENV{EPOCROOT}epoc32\\release\\winscw\\udeb\\winsgui.dll"; # if file present, is EKA2 build
my $UrelTestExecute = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\urel\\testexecute.exe" . $TestFrameworkOpts;
my $UdebTestExecute = "$ENV{EPOCROOT}epoc32\\release\\$BuildToUse\\udeb\\testexecute.exe" . $TestFrameworkOpts;

my $RecogniserCfgFile = "$EpocCDrive\\mm\\AutorunTests.cfg";
my $copy_ini_secdisp = "perl secdisp_ini_append.pl append"; 
my $delete_ini_secdisp = "perl secdisp_ini_append.pl restore";
if($Debug>=1)
  {
  print "\nUsing $RecogniserCfgFile to configure the Epoc recogniser\n";
  }

print "\nScanning $EpocCDrive for script files\n";
my @ScriptFilesFound;
my $ScriptFile;
my $ScriptToRunTmp = $ScriptToRun;

#Firstly, try to find an exact script name
print "ScriptToRunTmp = $ScriptToRunTmp\n";

if(!($ScriptToRun =~ /\.script/i))
  {$ScriptToRunTmp = $ScriptToRun .'\.script';}
 
#find all the required script files
find( sub { push @ScriptFilesFound, $File::Find::name if /^$ScriptToRunTmp$/i }, ($EpocCDrive) );

#if no scripts were found, look for scripts that wildcard match
if(@ScriptFilesFound < 1)
  {
  if(!($ScriptToRun =~ /\.script/i))
    {$ScriptToRunTmp = '[\S]*' . $ScriptToRun .'[\S]*\.script';}
  else
    {$ScriptToRunTmp = $ScriptToRun;}

  find( sub { push @ScriptFilesFound, $File::Find::name if /$ScriptToRunTmp/i }, ($EpocCDrive) );
  }

if($Debug>=1)
  {
  my $len = @ScriptFilesFound;
  print "\nScanned $len script files in $EpocCDrive\n";
  foreach $ScriptFile (@ScriptFilesFound)
    {print "$ScriptFile\n";}
  }


#look for any scripts that are called from other scripts; no need to call these directly
my @IndirectScripts;
my $IndirectScript;
foreach $ScriptFile (@ScriptFilesFound)
  {
  unless (open(SCRIPTFILE,"$ScriptFile")) 
    {
    print STDERR "Can't open $ScriptFile: $!\n";
    exit 1;
    }
 
  my $ScriptFileLine;
  my @ResultFileContents = <SCRIPTFILE>;

  foreach $ScriptFileLine (@ResultFileContents)
    {
    if($ScriptFileLine =~ /^RUN_SCRIPT[\s]+[\S]+[\s]*$/)
      { 
	  if($Debug==2)
        {print "File: $ScriptFile Line:  $ScriptFileLine\n";}
	  
	  $IndirectScript =  $ScriptFileLine;
	  $_ = $IndirectScript;

	  m/^RUN_SCRIPT[\s]+([\S]+)[\s]*$/;
      $IndirectScript=($1);

	  if($Debug==2)
        {print "Secondary Script = $IndirectScript\n";}

	  push @IndirectScripts, $IndirectScript;
	  }
    }
  }


#produce a list of the script files that need running
my @ScriptsToRun;
ScriptFile_LBL: foreach $ScriptFile (@ScriptFilesFound)
  {
  
  foreach $IndirectScript (@IndirectScripts)
    {
	if($ScriptFile =~ /$IndirectScript/)
	  {
	  if($Debug==2)
        {print "Not running $ScriptFile directly\n";}
	  next ScriptFile_LBL;}
	}   

  push @ScriptsToRun, $ScriptFile;
  }


if($Debug>=1)
  {
  foreach $ScriptFile (@ScriptsToRun)
    {print "Running $ScriptFile\n";}
  }


unless (open(TESTRUNLOG,">TestRunLog.txt")) 
  {
  print STDERR "Can't open orphanedtests.txt: $!\n";
  exit 1;
  }

unless (open(MASTERSCRIPT,">master.script")) 
  {
  print STDERR "Can't open master.script: $!\n";
  exit 1;
  }
unless (open(MASTERALLOCSCRIPT,">master_alloc.script")) 
  {
  print STDERR "Can't open master_alloc.script: $!\n";
  exit 1;
  }

#create a master script file, this could be usefull for running on hardware.
foreach $ScriptFile (@ScriptsToRun)
  {
  $ScriptFile =~ s/\//\\/g;
  $ScriptFile =~ s/^[\s]*[a-z]://i;

  my $regex = quotemeta $EpocCDrive;
  $ScriptFile =~ s/$regex//gi;

  #alloc tests must be run under debug
  if($ScriptFile =~ /_alloc(_wm)?.script/i)
    {
    print MASTERALLOCSCRIPT "RUN_SCRIPT $ScriptFile\n";}
  else
    {print MASTERSCRIPT "RUN_SCRIPT $ScriptFile\n";}

  }
close MASTERSCRIPT;
close MASTERALLOCSCRIPT;


#remove any alloc tests from list. Alloc tests must be run on debug builds, Code Cover tests 
#are only done against release builds.
if($CodeCover==1)
  {
  my @ScriptsToRunTmp=();
  foreach $ScriptFile (@ScriptsToRun)
    {
    if(!($ScriptFile =~ /_alloc(_wm)?.script/i))
	  {
      push @ScriptsToRunTmp, $ScriptFile;
	  }
	}
  @ScriptsToRun = @ScriptsToRunTmp;
  }


my $TotalScriptFiles = @ScriptsToRun;
if($TimesToRun > 1)
  {
  print "Running $TotalScriptFiles scripts  $TimesToRun times\n";
  }
else
  {
  print "Running $TotalScriptFiles scripts\n";
  }


my $RunCount=0;
for ($RunCount = 0; $RunCount < $TimesToRun; $RunCount++) 
  {
  my $ScriptsRun=0;
  #run each script
  foreach $ScriptFile (@ScriptsToRun)
    {
    $ScriptFile =~ s/\//\\/g;
    $ScriptFile =~ s/^[\s]*[a-z]://i;

    my $regex = quotemeta $EpocCDrive;
    $ScriptFile =~ s/$regex//gi;


    #alloc tests must be run under debug
    if($ScriptFile =~ /_recog_alloc.script/i)
	    {
	    $Cmd = "$RecogUdebTestFramework $ScriptFile";
        }
    elsif($ScriptFile =~ /_nocap_alloc.script/i)
       	{
      	$Cmd = "$NoneUdebTestFramework $ScriptFile";
       	}
    elsif(($ScriptFile =~ /_alloc.script/i) || ($ScriptFile =~ /_alloc_wm.script/i) || ($ScriptFile =~ /_debug.script/i) || ($optctl{"d"}) )
       	{
      	$Cmd = "$UdebTestFramework $ScriptFile";
       	}
    elsif($ScriptFile =~ /_recog.script/i)
       	{
      	$Cmd = "$RecogUrelTestFramework $ScriptFile";
       	}
    elsif($ScriptFile =~ /_nocap.script/i)
       	{
      	$Cmd = "$NoneUrelTestFramework $ScriptFile";
       	}	
    elsif($ScriptFile =~ /_mmddcap.script/i)
       	{
      	$Cmd = "$MMDDCapUrelTestFramework $ScriptFile";
       	}
    elsif($ScriptFile =~ /_uecap.script/i)
       	{
      	$Cmd = "$UECapUrelTestFramework $ScriptFile";
       	}
    elsif($ScriptFile =~ /_secdisp.script/i)
      	{
     	#Take a backup of the existing ini files to .OLD and copy our ini files from the current dir to \epoc folders
	    if(!(system($copy_ini_secdisp)==0))
      	    {
      	    print "Failure to execute - $Cmd: $!";
      	    print TESTRUNLOG "Failure to execute - $Cmd: $!\n";
      	    }
	    $Cmd = "$UrelTestFramework $ScriptFile";
	    }
    elsif($ScriptFile =~ /\\te_/i)
 	    {
 	    $Cmd = "$UdebTestExecute $ScriptFile";
 	    }
    elsif($ScriptFile =~ /_te.script/i)
 	    {
 	    $Cmd = "$UdebTestExecute $ScriptFile";
 	    }
    else
     	{
       	$Cmd = "$UrelTestFramework $ScriptFile";
       	}

    ($sec,my $min,my $hour,my $mday,my $mon,my $year,my $wday,my $yday,my $isdst) = localtime(time);  
    print "Starting script at $hour:$min:$sec\n";
    print TESTRUNLOG "Starting script at $hour:$min:$sec\n";

    $ScriptsRun++;
    print "executing $Cmd ($ScriptsRun of $TotalScriptFiles)\n";

    print TESTRUNLOG "executing $Cmd\n";

    if(!(system($Cmd)==0))
      {
      print "Failure to execute - $Cmd: $!";
      print TESTRUNLOG "Failure to execute - $Cmd: $!\n";
      }
   if($ScriptFile =~ /_secdisp.script/i)
    {
    #Delete .ini files and rename  .OLD files to .ini files
    if(!(system($delete_ini_secdisp)==0))
         {
          print "Failure to execute - $Cmd: $!";
          print TESTRUNLOG "Failure to execute - $Cmd: $!\n";
          }
    } 
   
    }#foreach $ScriptFile (@ScriptsToRun)

  if($TimesToRun > 1)
    {
    #now copy the results to a new numbered directory 
    $Cmd = "xcopy /E /R /I /Y P:\\epoc32\\wins\\c\\Logs\\TestResults P:\\epoc32\\wins\\c\\Logs\\TestResults$RunCount";
    print "executing $Cmd";

    if(!(system($Cmd)==0))
      {
      print "Failure to execute - $Cmd: $!";
      print TESTRUNLOG "Failure to execute - $Cmd: $!\n";
      }
    }
  }



$Cmd="del /F /Q $RecogniserCfgFile";
if($Debug==2)
  {print "executing $Cmd\n";}
system($Cmd);

($sec,my $min,my $hour,my $mday,my $mon,my $year,my $wday,my $yday,my $isdst) = localtime(time);  
print "Test run completed at $hour:$min:$sec\n";
print TESTRUNLOG "Test run completed at $hour:$min:$sec\n";


__END__

:endofperl

