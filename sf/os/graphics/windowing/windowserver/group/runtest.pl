# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

use File::Copy;
use File::Path;
use FindBin;
use lib $FindBin::Bin;
use Win32::Process; 
use Win32; 
use Win32::Job;

my $platform = "winscw";
my $screens = 2;

# ------------------------------------------------------------------------------------------------------------------------------------- #
#             log-id				wins/winscw	executable	test-context					wsini selection					#of screens #
# ------------------------------------------------------------------------------------------------------------------------------------- #
&RunWservTest("normal",				$platform, "auto", 		"autou.cfg", 					"wservu.ini",					$screens);
&RunWservTest("sparse", 			$platform, "auto", 		"autou_sparse.cfg", 			"wservu_sparse.ini",			$screens);
&RunWservTest("sparse_nodefault", 	$platform, "auto", 		"autou_sparse_nodefault.cfg",	"wservu_sparse_nodefault.ini",	$screens);
exit;

sub RunWservTest()
{
  my $session = shift;
  my $plat = shift;
  my $exe = shift;
  my $cfg = shift;
  my $wservu = shift;
  my $nScreens = shift;
  
  my $epocDir = "\\epoc32\\release\\$plat\\udeb\\";
  my $epocEmulator = $epocDir."epoc.exe";

  my $wsini = $epocDir."z\\system\\data\\wsini.ini";
  my $wsiniBackup = $wsini.".bak";
  
  my $epocFile = "\\epoc32\\data\\epoc.ini";
  my $epocBackup = $epocFile.".bak";

  # backup original epoc.ini and wsini.ini	
  &DoCopy($epocFile, $epocBackup) if (!-f $epocBackup);
  &DoCopy($wsini, $wsiniBackup) if (!-f $wsiniBackup);

  print "Mnt install\n";
  my $wservDir = ".";
  &StartMntCommand($wservDir, $plat, $wservu, $cfg);

  $exeFile = $epocDir.$exe.".exe";
  if (-f $exeFile)
  {
	$epocEmulator = $exeFile;
  }
  else
  {
	&AddShellCmd($wsini, $exe);
  }

  if ($nScreens > 1)
  {
	&CreateEpocMultiScreen($epocFile, $wsini, $nScreens);
  }
  
  my $job = Win32::Job->new;
  if (!$job) { &ErrorReport(); next; }

  print "Running wserv test $session...";
  my $spawned = $job->spawn($epocEmulator, 'epoc.exe');
  if (!$spawned) { &ErrorReport(); next; }

  my $timeout = 45;
  my $ok = $job->run($timeout * 60); 
  warn "WARNING: Process \"$epocEmulator\" killed due to timeout.\n" if (!$ok);
	
  print "done\n";
  
  # save wserv.log
  my $logDir = "\\epoc32\\$plat\\c\\data\\";
  my $wservLog = $logDir."wserv.log";
  my $logBackup = $logDir."wserv_$session.log";
  &DoCopy($wservLog, $logBackup);
  
  
  # restore original epoc.ini and wsini.ini
  &DoCopy($wsiniBackup, $wsini);
  &DoCopy($epocBackup, $epocFile);
  
  # remove auto.cfg
  system("del /f /q \\epoc32\\release\\$plat\\udeb\\z\\system\\data\\auto.cfg");
}

sub StartMntCommand()
{
  my $wservDir = shift;
  my $plat = shift;
  my $wservu = shift;
  my $autou = shift;

  &DoCopy("$wservDir\\..\\group\\system.ini", "\\epoc32\\data\\");
  &MntDoInstall($wservDir, $plat, 'udeb', $wservu, $autou);
  &MntDoInstall($wservDir, $plat, 'urel', $wservu, $autou);
}

sub MntDoInstall()
{
  my $wservDir = shift;
  my $plat = shift;
  my $UrelUdeb = shift;
  my $whatFile = shift;
  my $autoCfg = shift;

  my $dataDir   = "\\epoc32\\release\\$plat\\$UrelUdeb\\z\\system\\data\\";
  mkpath($dataDir);
  &DoCopy("$wservDir\\..\\group\\$whatFile",    $dataDir.'wsini.ini');
  &DoCopy("$wservDir\\..\\group\\$autoCfg",     $dataDir.'auto.cfg');  
}

sub AddShellCmd
{
  my $file = shift;
  my $cmd = shift;

  my $string = &ascii_to_utf16("SHELLCMD $cmd");

  &Write_UTF16_Newline($file);

  open(FILE, ">>$file") or warn "WARNING: Could not open file: $!\n";
  print FILE $string;
  close FILE;

  &Write_UTF16_Newline($file);
}

sub Write_UTF16_Newline
{
  my $file = shift;

  open(BIN, ">>$file") or warn "WARNING: Could not open \"$file\": $!\n";
  binmode BIN;
  sysseek BIN, 0, SEEK_END;
  syswrite BIN, "\x0D\x00\x0A\x00" or warn "WARNING: Could not write to file\n";
  close BIN;
}

# Function that accepts an ASCII string and returns the same string in UTF16
sub ascii_to_utf16 {
  my $utf16_string = "";
  my $ascii_string = shift;
  my $lengthofstring = length($ascii_string);

  for (my $count=1; $count<=$lengthofstring; $count++)
  {
    my $char = substr($ascii_string,$count-1,1);
    $utf16_string .= $char;
    $utf16_string .= "\x00";
  }

  return $utf16_string;
}

sub	ProduceWServResult()
{
	$plat = shift;
	$subDir = shift;
	$handleFileTable = shift;	
  $wservTestResStatistic = shift;

  $background = 0;

  $wservTestResStatistic = "";
  $duration = "";
  
	my $result = "PASS";

  open TEMPLOGFILE, ">>\\work.txt";
  print TEMPLOGFILE "Start ProduceWServResult: plat $plat,subDir $subDir,handleFileTable $handleFileTable,background $background\n";
  close TEMPLOGFILE;

	$logFile = "$ENV{EPOCROOT}epoc32\\$plat\\c\\data\\WSERV.LOG";

  open LOGFILEWSERV, "$logFile" or $logWSFileRes = -1;
  $CurWinServTestNumber = 0;
  $CurWinServTestName = "";
  $wservTestDuration = 0;
  $wservFullTestDuration = 0;

	print "result ws : $logWSFileRes\n";

  $fileLogWServDest = "$subDir\\1_wserv.htm";
  open FILELOGDEST, ">$fileLogWServDest";#	or die "Can't open file: $fileLogWServDest! \n";
  print FILELOGDEST  "<pre>";

  while($_ = <LOGFILEWSERV>)
	{
    if(/\s(\d+\.\d\d\d)/)
    {
      $wservTestDuration = $1;
    }
    else
    {
      $wservTestDuration = 0;
    }
    $wservFullTestDuration +=  $wservTestDuration;

    if(/AUTO New Test: /)
		{
      $wservFullTestDuration = $wservTestDuration;

      if($CurWinServTestNumber != 0) #previos test was inconclusive
      {
        $Inconclusive++;
        $result = "INCONCLUSIVE";
        OutputWServerTestResultToTable(FILERES, $background, $result,
           $CurWinServTestName, $fileLogWServDest, $wservFullTestDuration);
        $background = !$background;
        print FILELOGDEST  "</pre>";
        close FILELOGDEST;
      }

      if(/Test (\d+),(.+\w)(\s+\d\.)/)
      {
        $CurWinServTestNumber = $1;
        $CurWinServTestName = $2;
      }
      else
      {
        /Test (\d+),(.+)/;
        $CurWinServTestNumber = $1;
        $CurWinServTestName = $2;
      }


      if($CurWinServTestNumber > 1)
      {
          $fileLogWServDest = "$subDir\\$CurWinServTestNumber" . "_wserv.htm";
          open FILELOGDEST, ">$fileLogWServDest";#	or die "Can't open file: $fileLogWServDest! \n";
          print FILELOGDEST  "<pre>";
      }
    }
    elsif(/AUTO TEST RESULT: /)
		{
      $CurWinServTestNumber = 0;

      /AUTO TEST RESULT: (\w+)/;
      $TestResult = $1;
      if($TestResult eq "PASS")
      {
        $col = "#008000";
        $result = $TestResult;
        $Pass++;
      }
      elsif($TestResult eq "FAIL")
      {
        $col = "#ff0000";
        $result = $TestResult;
        $Failed++;
      }
      else
      {
        $col = "#0000ff";
        $Unknown++;
        $result = "N/A";
      }
      
      if($wservFullTestDuration =~ /(\d+\.\d{0,3})/)
      {
        $wservFullTestDuration = $1;
      }

      #output result to the file
      OutputWServerTestResultToTable(FILERES, $background, $result,
          $CurWinServTestName, $fileLogWServDest, $wservFullTestDuration);
      $background = !$background;
      print FILELOGDEST "<FONT COLOR=\"$col\">$_</FONT><br>";

      print FILELOGDEST  "</pre>";
      close FILELOGDEST;
		}
    if(/AUTO Testing Complete, (.*)(\d+\.\d{0,3})/)
    {
      $wservTestResStatistic = $1;
    }

    if(/AUTO Failed /)
    {
       print FILELOGDEST "<FONT COLOR=\"#ff0000\">$_</FONT><br>";
    }
    else
    {
        print FILELOGDEST "$_<br>";
    }
	} #while($_ = <LOGFILEWSERV>)
	
 #check if a test is inconclusive
  if($CurWinServTestNumber != 0)
  {
     $CurWinServTestNumber = 0;
     $wservFullTestDuration = $wservTestDuration;

      $wservTestResStatistic = "Inconclusive";
      $result = "INCONCLUSIVE";
      $Inconclusive++;

      #output result to the file
      OutputWServerTestResultToTable(FILERES, $background, $result,
          $CurWinServTestName, $fileLogWServDest, $wservFullTestDuration);
      $background = !$background;

      print FILELOGDEST  "</pre>";
      close FILELOGDEST;
  }
 
	close LOGFILEWSERV;
	close FILELOGDEST;
}

sub	OutputWServerTestResultToTable()
{
  my $handle = shift;
  my $background = shift;
  my $result = shift;
  my $TestName = shift;
  my $WServDest = shift;
  my $duration = shift;

  if($background == 1)
  {
		    print $handle "<TR BGCOLOR='#ECECE4'><TD WIDTH=\"33%\" VALIGN=\"TOP\">\n";
  }
  else
  {
		    print $handle "<TR><TD WIDTH=\"25%\" VALIGN=\"TOP\">\n";
  }

  print $handle "<FONT SIZE=2><P><a href=\"$WServDest\">$TestName</FONT></TD>\n";

  print $handle "<TD WIDTH=\"25%\" VALIGN=\"TOP\">\n";
  print $handle "<FONT SIZE=2><P>$duration</FONT></TD>\n";

  print $handle "<TD WIDTH=\"25%\" VALIGN=\"TOP\">\n";
  print $handle "<FONT SIZE=2><P>$result</FONT></TD>\n";
  print $handle "</TR>\n";
}

sub DoCopy
{
  my $source = shift;
  my $target = shift;

  my $out = `copy /y \"$source\" \"$target\" 2>&1`;

  if ($? && -e $target && $out =~ /Access is denied./)
  {
    # Target exists and is probably read only, so attempt an xcopy
    `xcopy /y /i /r /e \"$source\" \"$target\" 2>&1`;
  }

  warn "WARNING: Could not copy \"$source\" to \"$target\"\n" if $?;
}

sub CreateEpocMultiScreen()
{
  my $epocFile = shift;
  my $wsiniFile = shift;
  my $nScreens = shift;

  # epoc.ini
  open(FILE, ">>$epocFile") or warn "WARNING: Could not open file: $!\n";
  for (my $i=1; $i<$nScreens; $i++)
  {  
	  print FILE "_NewScreen_\n";
	  print FILE "ScreenWidth 640\n";
	  print FILE "ScreenHeight 240\n";
  }  
  close FILE

  # wsini.ini
  &AppendLineToFile($wsiniFile, "[SCREEN0]"); 
  for (my $i=1; $i<$nScreens; $i++)
  {
	&AppendLineToFile($wsiniFile, "[SCREEN$i]");   
  }
 }
 
 sub AppendLineToFile()
 {
   my $file = shift;
   my $line = shift;
 
   my $string = &ascii_to_utf16($line);
 
   &Write_UTF16_Newline($file);
 
   open(FILE, ">>$file") or warn "WARNING: Could not open file: $!\n";
   print FILE $string;
   close FILE;
 
   &Write_UTF16_Newline($file);
 }
 
