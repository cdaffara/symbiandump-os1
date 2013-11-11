#! /usr/bin/perl -w
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
# Scans a number of Symbian OS directories to identify usage of ETel APIs.
# The script takes a number of parameters (some are optional):
# Usage: etel_APIs_xref.pl [-testcode] <telephony_src_dir> <symbian_os_src_dir> <output_csv_filename>
# where,
# -testcode  includes test components.
# <telephony_src_dir>    is the location of the telephony code.
# <symbian_os_src_dir>   is the location of the code to analyse.
# <output_csv_filename>  is the output CSV file.
# etel_APIs_xref.pl p:\master\common\generic\telephony p:\master\common\generic p:\etel_APIs.txt
# Let's write this stuff properly...
# 
#

use strict;


#
# Constants...
#
my $version = "v1.0";
my $usageText = "Usage: etel_APIs_xref.pl [-testcode] <telephony_src_dir> <symbian_os_src_dir> <output_csv_filename>\n" .
                "       where,\n" .
                "           -testcode  includes test components.\n" .
                "           <telephony_src_dir>    is the location of the telephony code.\n" .
                "           <symbian_os_src_dir>   is the location of the code to analyse.\n" .
                "           <output_csv_filename>  is the output CSV file.\n" .
                "\n" .
                "       E.g.: etel_APIs_xref.pl p:\\master\\common\\generic\\telephony p:\\master\\common\\generic p:\\etel_APIs.csv\n";
my @methodsToIgnore = ("Open", "Close", "Read", "Write", "Start", "Open", "StartL", "Swap",
                       "StoreL", "StoreLC", "InternalizeL", "ExternalizeL", "RestoreL", "Resume",
                       "Get", "Set", "GetAndSet", "SetAndGet", "SetTextL", "SetMode", "SetFormat",
                       "AddEntry", "AddEntryL", "DeleteEntry", "GetEntry", "GetNextEntry", "Init",
                       "Initialise", "Connect", "NewL", "ConstructL", "Destruct", "Delete",
                       "CancelReq", "DoCancel", "ReadCancel", "Reset", "PanicClient", "Ptr1",
                       "Ptr2", "ExtensionId", "Format", "Service", "RunL",
                       "__DbgCheckHeap", "__DbgFailNext", "__DbgMarkEnd", "__DbgMarkHeap");
my @etelFolderNames = ("etel", "etelmm", "etelpckt", "etelcdma", "etelsat", 
                       "cdmatsy", "mmtsy", "gsmbtt", "gprsbtt", "simtsy");
my @testFolderNames = ("test", "tetel", "testharness", "ts_etelpacket", "plattest", "unittest",
                       "ts_common", "qostest", "qostesting");
my @extensionsToScan = ("cpp", "inl", "c", "cc", "hpp");


#
# Globals...
#
my @searchFileList   = ();
my @filteredFileList = ();
my %skipMethods      = ();
my %etelFolders      = ();
my %testFolders      = ();
my %fileExtensions   = ();
my %methodNames      = ();
my %methodClassNames = ();
my %methodFullNames  = ();


###############################################################################
#
# F  U  N  C  T  I  O  N  S  .  .  .
#
###############################################################################


#
# Function: resetGlobalVariables
#
# Purpose:  Resets the the execution environment leaving the search directory
#           variables intact.
#
# Returns:  None.
#
sub resetGlobalVariables
{
    #
    # Check and get the parameters...
    #
    if (@_ != 0)
    {
        die("Error: Invalid parameters to resetGlobalVariables(@_)!\n");
    }

    @filteredFileList = ();
    %skipMethods      = ();
    %etelFolders      = ();
    %testFolders      = ();
    %fileExtensions   = ();
    %methodNames      = ();
    %methodClassNames = ();
    %methodFullNames  = ();

    for (my $index = 0;  $index < @methodsToIgnore;  $index++)
    {
        $skipMethods{$methodsToIgnore[$index]} = "defined";
    }

    for (my $index = 0;  $index < @etelFolderNames;  $index++)
    {
        $etelFolders{$etelFolderNames[$index]} = "defined";
    }

    for (my $index = 0;  $index < @testFolderNames;  $index++)
    {
        $testFolders{$testFolderNames[$index]} = "defined";
    }

    for (my $index = 0;  $index < @extensionsToScan;  $index++)
    {
        $fileExtensions{$extensionsToScan[$index]} = "defined";
    }
} # resetGlobalVariables


#
# Function: initialiseGlobalVariables
#
# Purpose:  Initialises the Global variables for the first time, or to reset
#           the execution environment.
#
# Returns:  None.
#
sub initialiseGlobalVariables
{
    #
    # Check and get the parameters...
    #
    if (@_ != 0)
    {
        die("Error: Invalid parameters to initialiseGlobalVariables(@_)!\n");
    }

    @searchFileList = ();

    resetGlobalVariables();
} # initialiseGlobalVariables


#
# Function: readNamesFile
#
# Purpose:  Reads a BWINS DEF file and extracts the API names from it.
#
# Returns:  None.
#
sub readNamesFile
{
    #
    # Check and get the parameters...
    #
    if (@_ != 1)
    {
        die("Error: Invalid parameters to readNamesFile(@_)!\n");
    }

    my $fileName = $_[0];
 
    my $lineNum = 0;
    my $count   = 0;

    my $result = open(FILE, $fileName);
    if (!defined $result  ||  $result == 0)
    {
        die("Error: Unable to open names file \"$fileName\": $!\n");
    }

    while (my $line = <FILE>)
    {
        chop($line);
        $lineNum++;

        if ($line =~ /^\s*$/  ||  $line =~ /^\s*#/  ||
            $line =~ /^\s*EXPORTS\s*$/  ||  $line =~ /^\s*\; NEW\:\s*$/)
        {
            next;
        }

        #
        # First part of a DEF file line: ?<method-name>@gobblygook
        # Full API name can be found after the " ; ".
        #
        if ($line =~ /$\s*(.*)\s;(.*?)(((\S+|\S+\:\:\S+)\:\:(\S+))\(.*)$/)
        {
            my $mangledName = $1;
            my $fullDef     = $3;
            my $fullName    = "$4()";
            my $className   = $5;
            my $methodName  = $6;

#print("mangledName $mangledName\n");
#print("fullDef $fullDef\n");
#print("fullName $fullName\n");
#print("className $className\n");
#print("methodName $methodName\n");
            if ($mangledName =~ /"$methodName"/i)
            {
                die("Error: API DEF file mismatch: \"$mangledName\" does not contain \"$methodName\"!\n");
            }

            if (! $skipMethods{$methodName})
	    {
		$methodNames{$methodName} = "";

                if (defined $methodFullNames{$methodName}  &&  $methodFullNames{$methodName} !~ /$fullName/)
                {
                    $methodFullNames{$methodName} = "$methodFullNames{$methodName} $fullName";
                }
                else
                {
                    $methodFullNames{$methodName} = $fullName;
                }

                $methodClassNames{$className} = 1;

		$count++;
            }
        }
        else
        {
            print("Warning: Cannot understand \"$line\"!\n");
        }
    }

    close(FILE);

    print("  Read $count method names from \"$fileName\"\n");
} # readNamesFile


#
# Function: generateFileListToSearch
#
# Purpose:  Creates a list of searchable files matching the searchable extensions
#           and testcode if needed.
#
# Returns:  None.
#
sub generateFileListToSearch
{
    #
    # Check and get the parameters...
    #
    if (@_ != 2)
    {
        die("Error: Invalid parameters to generateFileListToSearch(@_)!\n");
    }

    my $searchDir        = $_[0];
    my $includeTestCode  = $_[1];

    my @subDirs = ($searchDir);

    @searchFileList = ();

    print("Creating file search list from \"$searchDir\"...\n");

    #
    # From the subDir list, work through every subdirectory scanning for matching
    # files of further subdirectories...
    #
    for (my $index = 0;  $index < @subDirs;  $index++)
    {
        #
        # Should we skip this folder as ETel code or test code?
        #
        if ($index > 0)
        {
            my $searchDirLength = length($searchDir);
            my $directoryName   = substr($subDirs[$index], $searchDirLength + 1);
            my @folders         = split('\\\\', $directoryName);

            $directoryName = $folders[@folders-1];
            $directoryName =~ tr/A-Z/a-z/;

            if (defined $etelFolders{$directoryName})
            {
                # Skip this folder!
                next;
            }

            if ($includeTestCode == 0  &&  defined $testFolders{$directoryName})
            {
                # Skip this folder!
                next;
            }
        }

        #
        # Scan the folder for other folders or files...
        #
        if (! opendir(DIR, $subDirs[$index]))
        {
            die("Error: Unable to open dir \"$subDirs[$index]\": $!\n");
        }

        while (my $thisFile = readdir(DIR))
        {
            if ($thisFile eq "."  ||  $thisFile eq "..")
            {
                 next;
            }

            my $sourcePath = "$subDirs[$index]\\$thisFile";

            #
            # Is this a directory?
            #
            if (-d $sourcePath)
            {
	        push(@subDirs, $sourcePath);
	        next;
       	    }

            my $fileExt = "";

            if ($sourcePath =~ /.*\.(\w+)$/)
            {
	        $fileExt = $1;
                $fileExt =~ tr/A-Z/a-z/;
            }

            if (defined $fileExtensions{$fileExt})
            {
                push(@searchFileList, $sourcePath);
            }
        }  

        closedir(DIR);
    }

    my $totalFiles = @searchFileList;
    print("  Generated a list of $totalFiles files to search.\n");
} # generateFileListToSearch


#
# Function: filterSearchListToMethodClasses
#
# Purpose:  Using the list of class names (obtained from the DEF file) each directory
#           below the one passed in is checked to see of the classes are used. If so,
#           then the higher level directory is added to the filtered search list. 
#
# Returns:  None.
#
sub filterSearchListToMethodClasses
{
    #
    # Check and get the parameters...
    #
    if (@_ != 1)
    {
        die("Error: Invalid parameters to filterSearchListToMethodClasses(@_)!\n");
    }

    my $searchDir = $_[0];

    my %searchFolders = ();

    @filteredFileList = ();

    print("  Filtering source directory \"$searchDir\"...\n");

    #
    # Work through the search list to find files using the class names...
    #
    for (my $fileIndex = 0;  $fileIndex < @searchFileList;  $fileIndex++)
    {
        my $sourceFile = $searchFileList[$fileIndex];

        if (! open(SOURCE, $sourceFile))
        {
            die("Error: Unable to open source file \"$sourceFile\": $!\n");
        }

        my @data = <SOURCE>;
        close(SOURCE);

        foreach my $className (keys %methodClassNames)
        {
            my $numMatches = grep(/\W$className\W/, @data);

            if ($numMatches > 0)
            {
                my $topLevelLength = length($searchDir);
                my $fileName       = substr($sourceFile, $topLevelLength + 1);
                my @folders        = split('\\\\', $fileName);
                my $searchPath     = "";

                if (@folders > 2)
                {
                    $searchPath = "$searchDir\\$folders[0]\\$folders[1]";
                }
                else
                {
                    $searchPath = "$searchDir";
                }

                $searchFolders{$searchPath} = "";
            }
        }
    }

    #
    # Create the filtered search list...
    #
    for (my $fileIndex = 0;  $fileIndex < @searchFileList;  $fileIndex++)
    {
        my @folders      = split('\\\\', $searchFileList[$fileIndex]);
        my $path         = "";
        my $okayToSearch = 0;

        for (my $index = 0;  $index < @folders;  $index++)
        {
            if ($path eq "")
            {
                $path = "$folders[$index]";
            }
            else
            {
                $path = "$path\\$folders[$index]";
            }

            if (defined $searchFolders{$path})
            {
                $okayToSearch = 1;
                last;
            }
        }

        if ($okayToSearch)
        {
            push(@filteredFileList, $searchFileList[$fileIndex]);
        }
    }

    my $totalFiles = @filteredFileList;
    print("  Filtered a list of $totalFiles files to search.\n");
} # filterSearchListToMethodClasses


#
# Function: searchSourceForAPIMethods
#
# Purpose:  Starts a search of the filtered files for any of the APIs methods
#           loaded previously from the DEF files.
#
# Returns:  None.
#
sub searchSourceForAPIMethods
{
    #
    # Check and get the parameters...
    #
    if (@_ != 1)
    {
        die("Error: Invalid parameters to searchSourceForAPIMethods(@_)!\n");
    }

    my $searchDir = $_[0];

    print("  Searching for API method usage in \"$searchDir\"...\n");

    for (my $index = 0;  $index < @filteredFileList;  $index++)
    {
        my $sourceFile = $filteredFileList[$index];

        if (! open(SOURCE, $sourceFile))
        {
            die("Error: Unable to open source file \"$sourceFile\": $!\n");
        }

        my @data = <SOURCE>;
        close(SOURCE);

        foreach my $thisName (keys %methodNames)
        {
            my $numMatches = grep(/\W$thisName\s*\(/, @data);
            if ($numMatches > 0)
            {
                $methodNames{$thisName} .= "$index:";
            }
        }
    }
} # searchSourceForAPIMethods


#
# Function: writeAPIUsageToCSVFile
#
# Purpose:  Writes the API usage to the CSV file.
#
# Returns:  None.
#
sub writeAPIUsageToCSVFile
{
    #
    # Check and get the parameters...
    #
    if (@_ != 1)
    {
        die("Error: Invalid parameters to writeAPIUsageToCSVFile(@_)!\n");
    }

    my $SourceDir = $_[0];

    my $sourceDirLength = length($SourceDir);

    foreach my $apiName (sort keys %methodNames)
    {
        my  @refs = split(':', $methodNames{$apiName});
        my  %components = ();

        if (@refs > 0)
	{
            if ($methodFullNames{$apiName} eq "")
            {
                print CSV "$apiName,";
            }
            else
            {
                print CSV "$methodFullNames{$apiName},";
            }

            while (@refs > 0)
            {
                my $fileIndex = pop(@refs);

                #
                # Get the file and extract the component name...
                #
                my $sourceFile = $filteredFileList[$fileIndex];
                
                $sourceFile = substr($sourceFile, $sourceDirLength+1);

                if ($sourceFile =~ /^(.*?\\.*?)\\.*$/)
                {
                    $sourceFile = $1;
                }

                if (!defined $components{$sourceFile})
                {
                    print CSV "$sourceFile ";
                    $components{$sourceFile} = 1;
                }
	    }

            print CSV ("\n");
	}
    }
} # writeAPIUsageToCSVFile


###############################################################################
#
# M  A  I  N     P  R  O  G  R  A  M  .  .  .
#
###############################################################################

{
    system("title Etel API Cross Referencer $version");

    #
    # Print the end of file stuff...
    #
    print("Etel API Cross Referencer $version\n");
    print("==============================\n");
    print("\n");

    #
    # Check the input parameters...
    #
    my $telSrcDir       = "";
    my $osSrcDir        = "";
    my $outputCSV       = "";
    my $includeTestCode = 0;

    if (@ARGV == 4  &&  $ARGV[0] eq "-testcode")
    {
        $telSrcDir       = $ARGV[1];
        $osSrcDir        = $ARGV[2];
        $outputCSV       = $ARGV[3];
        $includeTestCode = 1;
    }
    elsif (@ARGV == 3)
    {
        $telSrcDir = $ARGV[0];
        $osSrcDir  = $ARGV[1];
        $outputCSV = $ARGV[2];
    }
    else
    {
        die("Error: Incorrect number of parameters.\n" . "$usageText\n");
    }

    if (! -d $telSrcDir)
    {
        die("Error: Source directory \"$telSrcDir\" does not exist!\n");
    }

    if (! -d $osSrcDir)
    {
        die("Error: Source directory \"$osSrcDir\" does not exist!\n");
    }

    if ($includeTestCode)
    {
        print("Using \"$telSrcDir\" to scan \"$osSrcDir\" (including test code) creating \"$outputCSV\"\n");
    }
    else
    {
        print("Using \"$telSrcDir\" to scan \"$osSrcDir\" creating \"$outputCSV\"\n");
    }
    print("\n");

    #
    # Setup all the variables...
    #
    &initialiseGlobalVariables();

    #
    # Generate a list of files to search. This is basically the OS Src Dir files
    # which match the search file extensions and exclude test code if not desired.
    #
    &generateFileListToSearch($osSrcDir, $includeTestCode);

    #
    # Open the CSV file...
    #
    my $result = open(CSV, ">$outputCSV");
    if (!defined $result  ||  $result == 0)
    {
        die("Error: Unable to create method index file \"$outputCSV\": $!\n");
    }

    #
    # Read in each DEF file and write the results...
    #
    print("Scan for ETel APIs...\n");
    print CSV ("ETel APIs,Subsystem Component\n");
    &resetGlobalVariables();
    &readNamesFile("$telSrcDir\\etel\\BWINS\\ETELU.DEF");
    &filterSearchListToMethodClasses($osSrcDir);
    &searchSourceForAPIMethods($osSrcDir);
    &writeAPIUsageToCSVFile($osSrcDir);
    print CSV ("\n");

    print("Scan for ETel MM APIs...\n");
    print CSV ("ETel MM APIs,Subsystem Component\n");
    &resetGlobalVariables();
    &readNamesFile("$telSrcDir\\etelmm\\BWINS\\ETELMMU.DEF");
    &filterSearchListToMethodClasses($osSrcDir);
    &searchSourceForAPIMethods($osSrcDir);
    &writeAPIUsageToCSVFile($osSrcDir);
    print CSV ("\n");

    print("Scan for ETel Packet APIs...\n");
    print CSV ("ETel Packet APIs,Subsystem Component\n");
    &resetGlobalVariables();
    &readNamesFile("$telSrcDir\\etelpckt\\BWINS\\ETELPCKTU.DEF");
    &filterSearchListToMethodClasses($osSrcDir);
    &searchSourceForAPIMethods($osSrcDir);
    &writeAPIUsageToCSVFile($osSrcDir);
    print CSV ("\n");

    print("Scan for ETel SAT APIs...\n");
    print CSV ("ETel SAT APIs,Subsystem Component\n");
    &resetGlobalVariables();
    &readNamesFile("$telSrcDir\\etelsat\\BWINS\\ETELSATU.DEF");
    &filterSearchListToMethodClasses($osSrcDir);
    &searchSourceForAPIMethods($osSrcDir);
    &writeAPIUsageToCSVFile($osSrcDir);
    print CSV ("\n");

    print("Scan for ETel CDMA APIs...\n");
    print CSV ("ETel CDMA APIs,Subsystem Component\n");
    &resetGlobalVariables();
    &readNamesFile("$telSrcDir\\etelCDMA\\BWINS\\ETELCDMAU.DEF");
    &filterSearchListToMethodClasses($osSrcDir);
    &searchSourceForAPIMethods($osSrcDir);
    &writeAPIUsageToCSVFile($osSrcDir);
    print CSV ("\n");

    #
    # Close the CSV file...
    #
    close(CSV);
}
