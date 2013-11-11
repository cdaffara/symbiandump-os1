#!perl

# MakeISOs.pl

# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Version Info:
# Version 001: initial version
# Command: MakeISOs.pl [<TargetDirectory>] [<Build number>]
# 
#


use File::Copy;


# ------------------------------------- Global variables -----------------------------------


my $TargetDirectory = $ARGV[0];
my $TargetDirectoryDefault = "\\_PACKAGES";
my $BuildNumber = $ARGV[1];
my $BuildNumberDefault = $ENV{'Buildnumber'};



my $DevKitDir  = "\\_PACKAGES\\DevKit_CD";
my $CustKitDir = "\\_PACKAGES\\CustKit_Extra_CD";

my $pkgSrcDefDevKitDir = "\\Product\\DevKit\\SourceDefinitions";
my $sourceListDevKit = "DevKit.srcdef_EDITED";

my $sourceListDevKit1 = "DevKit1.srcdef";
my $sourceListDevKit2 = "DevKit2.srcdef";

my $createPkgSrc = "\\generic\\epoc32\\tools\\createpkgsrc";
my $createISO   = "c:\\apps\\dao\\makeiso";


my @PackagesToSplitOut = (

		"com.symbian.src.connect",
		"com.symbian.src.GT-general",
		"com.symbian.src.GT-restricted-part1",
		"com.symbian.src.GT-restricted-part2",
		"com.symbian.src.GT-restricted-part3",
		"com.symbian.src.GT-restricted-part4",
		"com.symbian.src.GT-restricted-part5",
		"com.symbian.src.GT-restricted-part6",

		"com.symbian.src.TechView-general",
		"com.symbian.src.TechView-restricted",

		"com.symbian.devkit.src"  );




# --------------------------------- Start of Main() ----------------------------------------


# set up target directory.
$TargetDirectory = EstablishTargetDirectory($ARGV[0]);


my $outputDevKitDir1 = "$TargetDirectory\\DevKit_CD_1";
my $outputDevKitDir2 = "$TargetDirectory\\DevKit_CD_2";


# create sub-directories
mkdir $outputDevKitDir1, 0777 if (! -e $outputDevKitDir1);
mkdir $outputDevKitDir2, 0777 if (! -e $outputDevKitDir2);


# split the .srcdef and copy required .sdkpkg files
SplitSrcdefAndCopyPackages();


# copy non-packages (KSA etc.) files for the DevKits
CopyKSA($DevKitDir, $outputDevKitDir1);
CopyKSA($DevKitDir, $outputDevKitDir2);


# create the package source (package-list.xml.zip) for the DevKits
CreateSrc($outputDevKitDir1, "$TargetDirectory\\$sourceListDevKit1");
CreateSrc($outputDevKitDir2, "$TargetDirectory\\$sourceListDevKit2");


# check build number parameter
if ($BuildNumber eq "")
{
	$BuildNumber = $BuildNumberDefault;
}


# create the ISO images for the Kits (CustKit and DevKits)
system ("call $createISO $outputDevKitDir1 $TargetDirectory\\DevKit_CD_".$BuildNumber."_Source.ISO /BATCH /JOLIET /VOLUME=DevKit_Source");
system ("call $createISO $outputDevKitDir2 $TargetDirectory\\DevKit_CD_$BuildNumber.ISO /BATCH /JOLIET /VOLUME=DevKit");
system ("call $createISO $CustKitDir       $TargetDirectory\\CustKit_Extra_CD_$BuildNumber.ISO /BATCH /JOLIET /VOLUME=CustKiT");




# --------------------------------- Start of EstablishTargetDirectory() ----------------------------------------
# set up target directory. If supplied, create dir if non-existant. If this fails or was not supplied, use default.

sub EstablishTargetDirectory($)
{
	$CommandLineTargetDirectory = $_[0];        # target directory for the ISOs

	if ($CommandLineTargetDirectory ne "")
	{    
	    if ((-d $CommandLineTargetDirectory) or (mkdir $CommandLineTargetDirectory, 0777))     # exists or created OK
		{
			return $CommandLineTargetDirectory;
		}
	}

	# target not given or unable to create it, so try to use default, or set to "."
	if (-d $TargetDirectoryDefault)
	{
		return $TargetDirectoryDefault;     # returns default 
	}
	else
	{
		return ".";
	}
}



# --------------------------------- Start of SplitSrcdefAndCopyPackages() ----------------------------------------
# split the .srcdef and copy required .sdkpkg files

sub SplitSrcdefAndCopyPackages()
{
	# read in original .srcdef file
	open (FILE, "<$pkgSrcDefDevKitDir\\$sourceListDevKit") or die "Couldn't open file $pkgSrcDefDevKitDir\\$sourceListDevKit";
	my @srcList = <FILE>;
	close(FILE);

	# open up two .srcdef files for new Kits
	open (FILE1, ">$TargetDirectory\\$sourceListDevKit1") or die "Couldn't open file $TargetDirectory\\$sourceListDevKit1";
	open (FILE2, ">$TargetDirectory\\$sourceListDevKit2") or die "Couldn't open file $TargetDirectory\\$sourceListDevKit2";


	foreach my $line (@srcList)
	{
		if ($line !~ /<item/)     # line doesn't specify a source file;  must be header or footer, so copy across
		{
			print FILE1 $line;
			print FILE2 $line;
		}	
		else                      # line specifies a source file; copy line across and copy file too
		{
			$line =~ /(com.*?sdkpkg)/ ;
			my $fileName = $1 ;

			if ( grep { $line =~ /$_/ } @PackagesToSplitOut )	          # packages listed in @PackagesToSplitOut
			{
				print FILE1 $line;
				copy ("$DevKitDir\\$fileName", "$outputDevKitDir1\\$fileName") or die "Unable to copy $DevKitDir\\$fileName to $outputDevKitDir1\\$fileName - $!" ;
			}
			else	                                                      # packages not listed in @PackagesToSplitOut
			{
				print FILE2 $line;
				copy ("$DevKitDir\\$fileName", "$outputDevKitDir2\\$fileName") or die "Unable to copy $DevKitDir\\$fileName to $outputDevKitDir2\\$fileName - $!" ;
			}
		}	
	}
	close (FILE1);
	close (FILE2);
}




# --------------------------------- Start of CopyKSA() ----------------------------------------
# copies non-*.sdkpkg files from source to distribution directory

sub CopyKSA($$)
{
	my $SourceDir = $_[0];      # where non-*.sdkpkg files are (usually original DevKit directory)
	my $DistribDir = $_[1];     # where they should be copied to (usually new DevKit directory)
	
	print "Copying KSA from $SourceDir to $DistribDir\n";	
	
	my @Files = <$SourceDir\\*>;
	
	foreach my $FileName (@Files)
	{
		if ($FileName !~m/sdkpkg/i)
		{
			copy($FileName, $DistribDir) or die "Unable to copy $FileName into $DistribDir" ;
		}
	}
}



# --------------------------------- Start of CreateSrc() ----------------------------------------
# creates a package source (package-list.xml.zip)
 
sub CreateSrc($$)
{
	chdir($_[0]) or die "Unable to change directory to $_[0]";                   # directory to put file in to

	my $pkgSrcDefFile = $_[1] ;     # full path location to the *.srcdef file

	# build the package source XML
	my $command = $createPkgSrc." -s ".$pkgSrcDefFile ;
	system($command) ;
}

