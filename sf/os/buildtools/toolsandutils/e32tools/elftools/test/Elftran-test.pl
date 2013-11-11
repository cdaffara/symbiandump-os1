#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#!/usr/bin/perl -w
use strict;

# Perl script that creates PKG files and test MAKESIS tools with different ranges

#my $logFile = "\\epoc32\\winscw\\c\\ELFTRAN_test.txt";
my $logFile = "ELFTRAN_test.txt";
my @retval = 0;

#
# Counters for results
#
my $NumberOfTests  = 0;
my $NumberOfPassed = 0;
my $NumberOfFailed = 0;

#
#Function to write log into file
#
sub WriteLog {
	my ($log) = @_;
	#public $logfile;
        my $fh;
	unless( open($fh, ">> $logFile")) {
		printf STDERR "Can\'t open $logFile:$!\n";
		return;
	}
	printf $fh $log;
	printf $log;
	close $fh;
}


#
# Run command with prepared command line parameters and administrate its result
#
sub RunCommand {

	my( $command, $expectedResult, $expectedLog, $expectedVal) = @_;
	#my ($pkgfile) = @_[0];
	#my ($expectedResult) = @_[1];
	#$pkgfile =~ s/\.pkg//;

       	#@retval = system("$command >run.log");
        @retval = system("$command >NUL");
        my $logMsg = sprintf "Expected code:%5d   result Code:%5d \n", $expectedResult, @retval;
        WriteLog( $logMsg);
        
        #open CMDPIPE, "$command 3>&1 2>&1 | ";
        open CMDPIPE, "$command 2>&1 | ";
        
        my @lines=<CMDPIPE>;
        close CMDPIPE;
       
        my $receivedVal;
        if( $expectedLog ne "")
        {
            $receivedVal = ProcessResult($expectedLog, $expectedVal, @lines);
            $logMsg = sprintf "Expected %s:%s   result value:%s \n", $expectedLog, $expectedVal, $receivedVal;
            WriteLog( $logMsg);
        }
        else
        {
            $receivedVal=$expectedVal;
        }
       
	$NumberOfTests++;

	if( $retval[0] == $expectedResult ) {
                if( $receivedVal eq $expectedVal ){
                    $NumberOfPassed++;
                    WriteLog("Passed\n\n");
                }
                else
                {
                    $NumberOfFailed++;
                    WriteLog("Failed\n\n");
                }
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}

	#unlink("$pkgfile-tmp.sis");
	#unlink("$pkgfile.log");
	#unlink("$pkgfile.pkg");

}

sub ProcessResult {
    my ($expLog, $expVal, @result) = @_;
    for my $line (@result) {
        #WriteLog( $line);
        if($line =~ /($expLog):[ \t](.*)/i)
        #if($line =~ /.?($expLog)[ :][ \t](.*)/i)
        {
            #WriteLog($1);
            if( $2 eq $expVal)
            {
                return $2;
            }
        }
        
    }
    return "";
}

#
# Main
#
# Create environment and control test flow to testing ELFTRAN
#

unlink($logFile);
WriteLog("ELFTRAN test.\n\n");


my $Command = "ELFTRAN.EXE";
my $Path = "\\epoc32\\tools\\";
#
# Array of contents of ELFTRAN parameters and expected results
#
#                    command,     options                                           inputfile/,         outfile    expected , expected log,     expeced log
#                                                                                   e32imagefile                    result     message          value
#               ------------------------------------------------------------------------------------------------------------------------------------------------ 
my @TestItems = (   [$Command,  "",                                                 "",                     "",     65280,    , "",		""],
                    [$Command,  "-h",                                               "",                     "",       256,    , "ERROR",        "Cannot open -h for input."],
                    [$Command,  "-dump h -nocompress",                              "camera1_test.exe",     "",         0,    , "Compression",  "00000000"],
                    [$Command,  "-dump h",                                          "camera1_test.exe",     "",         0,    , "Compression",	"101f7afc"],
                    [$Command,  "-dump h -nocompress",                              "camera1_test.exe",     "",         0,    , "Compression",  "00000000"],
                    [$Command,  "-dump h -compress",                                "camera1_test.exe",     "",         0,    , "Compression",  "101f7afc"],                    
                    [$Command,  "-dump h -nocompress",                              "camera1_test.exe",     "",         0,    , "Compression",  "00000000"],
                    [$Command,  "-dump h -compressionmethod bytepair",              "camera1_test.exe",     "",         0,    , "Compression",  "102822aa"],
                    [$Command,  "-dump h -compressionmethod none",                  "camera1_test.exe",     "",         0,    , "Compression",  "00000000"],
                    [$Command,  "-dump h -compressionmethod deflate",               "camera1_test.exe",     "",         0,    , "Compression",  "101f7afc"],
                    [$Command,  "-dump h -compress -compressionmethod bytepair",    "camera1_test.exe",     "",         0,    , "Compression",  "102822aa"],
                    [$Command,  "-dump h -compress -compressionmethod deflate",     "camera1_test.exe",     "",         0,    , "Compression",  "101f7afc"],
                    [$Command,  "-dump h -compress -compressionmethod none",        "camera1_test.exe",     "",         0,    , "Compression",  "00000000"],
                    [$Command,  "-dump h -nocompress -compressionmethod deflate",   "camera1_test.exe",     "",         0,    , "Compression",  "00000000"],
                    [$Command,  "-dump h -nocompress -compressionmethod none",      "camera1_test.exe",     "",         0,    , "Compression",  "00000000"],
                    [$Command,  "-dump h -nocompress -compressionmethod bytepair",  "camera1_test.exe",     "",         0,    , "Compression",  "00000000"],
	     );


#
# Do test for each elements of TestItems array
#
my $loop = 1;
for my $Test ( @TestItems )  {

    WriteLog( sprintf "Loop:%3d\n", $loop++);
        
    my $command = sprintf "%s%s %s %s %s", $Path, $Test->[0], $Test->[1], $Test->[2], $Test->[3];
    my $logLine = "Command: '$command'\n";
    WriteLog($logLine);

    RunCommand($command, $Test->[4], $Test->[5], $Test->[6]);
 
}


#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $NumberOfTests );
WriteLog(sprintf "Passed: %d\n", $NumberOfPassed );
WriteLog(sprintf "%d tests failed out of %d\n", $NumberOfFailed, $NumberOfTests ); 