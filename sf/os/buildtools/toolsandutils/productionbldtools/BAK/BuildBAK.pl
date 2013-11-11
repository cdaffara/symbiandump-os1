#!perl

# BuildBAK.pl

# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# 
#

use strict;
use File::Copy; 
use Getopt::Long;
use Cwd;


# ------------------------------------- Global variables -----------------------------------


my $DevKitPackagesDirectory = "";
my $TargetDirectory = "";
my $SrcDefFile = "";
my $KitBuildID = "";
my $Version="";
my $oldLayoutIntroPages = 0; # FALSE

# start the program
main();





# --------------------------------- Start of SearchAndReplace() ----------------------------
#
# Parameters: <search string> <replace string> <old file name> <new file name>
sub SearchAndReplace($$$$)
{
	# read in file
	open (FILE, "<$_[2]") or die "Couldn't open file $_[2]";
	my @file = <FILE>;
	close(FILE);

	if ($_[3] eq $_[2])
	{
		unlink($_[2]);
	}
		

	open (FILE, ">$_[3]") or die "Couldn't create file $_[3]";
	
	foreach my $line (@file)
	{
		$line =~ s/$_[0]/$_[1]/gi ;
    		print FILE $line ;
	}
	close(FILE);
}


# --------------------------------- CommandLineInterface() ----------------------------------------


sub CommandLineInterface()
{
	my $help;
	if ( (GetOptions( "devkit|d=s" => \$DevKitPackagesDirectory,
			  "target|t=s" => \$TargetDirectory,
			  "help|h|?" => \$help,
			  "srcdef|s=s" => \$SrcDefFile,
			  "version|v=s" => \$Version,
			  "kitbuildid|k=s" => \$KitBuildID ) == 0 ) || ($help == 1) )
	{
		Usage();
		exit; 
	}


	# check values received

	# check that DevKit is in stated directory, exit if not found
	my @Packages = <$DevKitPackagesDirectory/com.symbian.devkit.*.sdkpkg>;
	if ( scalar(@Packages) == 0 )
	{
		die "DevKit packages not found in directory: $DevKitPackagesDirectory\n";
	}


	# check that target location to write extracted files to exists
	while ( (substr($TargetDirectory, -1, 1) eq '\\') || (substr($TargetDirectory, -1, 1) eq '/') )
	{
		chop($TargetDirectory); 		# remove final backslashes
	}	
	if (! -e $TargetDirectory) 
	{
		mkdir $TargetDirectory;
	}
	
	
	# check that srcdef (*.srcdef) file exists
	if ( ($SrcDefFile ne "") && (! -e $SrcDefFile) )
	{
		die ".srcdef file not found at: $SrcDefFile\n";
	}


	# check that version has been specified
	if ($Version eq "") 
	{
		die "Version number undefined\n";
	}

}

# --------------------------------- Usage() ----------------------------------------

sub Usage()
{
	print <<ENDOFUSAGETEXT;
	
BUILDBAK.PL    Version 1.0    Copyright (c) 2005 Symbian Software Ltd
                              All rights reserved
                                  
Usage:
  perl BuildBAK.pl  [options]
 
where options are:
  -d[evkit] <path>        path to directory containing DevKit packages
  -k[itbuildID] <ID>      Kit build ID (e.g. 03651b_Symbian_OS_v9.1 or 02407 (v7.0s))
  -s[rcdef] <path>        full path and file name for the .srcdef file
  -t[arget] <path>        path to directory to build BAK into
  -v[ersion] <number>     Symbian OS version number (e.g. 7.0s or 9.1)
   
ENDOFUSAGETEXT
}

# --------------------------------- Start of CopyPreBuiltPackagesAndInstaller() ----------------------------------------

sub CopyPreBuiltPackagesAndInstaller($$)
{
	# underscore required at end of filenames which may be ambiguous (i.e.  com.symbian.bin.GT-arm_ and  com.symbian.bin.GT-arm-data_)
	my @Files=(	"com.symbian.api.GT-arm_",
			"com.symbian.api.GT-restricted_",
			"com.symbian.api.GT-shared_",
			"com.symbian.api.GT-winscw_",
			"com.symbian.api.GT-wins_",
			"com.symbian.api.TechView-arm_",
			"com.symbian.api.TechView-restricted_",
			"com.symbian.api.TechView-shared_",
			"com.symbian.api.TechView-winscw_",
			"com.symbian.api.TechView-wins_",
			"com.symbian.bin.GT-arm-data_",
			"com.symbian.bin.GT-arm_",
			"com.symbian.bin.GT-restricted-data_",
			"com.symbian.bin.GT-restricted_",
			"com.symbian.bin.GT-shared-data_",
			"com.symbian.bin.GT-shared_",
			"com.symbian.bin.GT-wins-shared_",
			"com.symbian.bin.GT-wins-udeb_",
			"com.symbian.bin.GT-wins-urel_",
			"com.symbian.bin.GT-winscw-shared_",
			"com.symbian.bin.GT-winscw-udeb_",
			"com.symbian.bin.GT-winscw-urel_",
			"com.symbian.bin.TechView-arm-data_",
			"com.symbian.bin.TechView-arm_",
			"com.symbian.bin.TechView-restricted-data_",
			"com.symbian.bin.TechView-restricted_",
			"com.symbian.bin.TechView-shared-data_",
			"com.symbian.bin.TechView-shared_",
			"com.symbian.bin.TechView-wins-shared_",
			"com.symbian.bin.TechView-wins-udeb_",
			"com.symbian.bin.TechView-wins-urel_",
			"com.symbian.bin.TechView-winscw-shared_",
			"com.symbian.bin.TechView-winscw-udeb_",
			"com.symbian.bin.TechView-winscw-urel_",
			"com.symbian.bin.TechView-winscw_",
			"com.symbian.bin.TechView-wins_",
			"com.symbian.debug.GT-winscw_",
			"com.symbian.debug.GT-wins_",
			"com.symbian.debug.TechView-winscw_",
			"com.symbian.debug.TechView-wins_",
			"com.symbian.doc.intro-pages_",
			"com.symbian.doc.sdl-cpp-examples_",
			"com.symbian.doc.sdl-java-examples_",
			"com.symbian.doc.sdl-shared-examples_",
			"com.symbian.src.GT-general_",
			"com.symbian.src.open_",
			"com.symbian.tools.all-arm_",
			"com.symbian.tools.all-shared_",
			"com.symbian.tools.all-winscw_",
			"com.symbian.tools.all-wins_",
			"com.symbian.tools.arm_",
			"com.symbian.tools.boardsupport_",
			"com.symbian.tools.cpp-custom_",
			"com.symbian.tools.cpp_",
			"com.symbian.tools.java_",
			"com.symbian.tools.libraries_",
			"com.symbian.tools.shared-custom_",
			"com.symbian.tools.shared_",
			"com.symbian.tools.winscw_",
			"com.symbian.tools.wins_",
	# installer
			"autorun.inf",
			"data1.cab",
			"data1.hdr",
			"data2.cab",
			"ikernel.ex_",
			"layout.bin",
			"Setup.exe",
			"setup.inx",
			"ReadMe.html");

	print "Checking to see what files need copying...\n";

	foreach my $file (@Files)
	{
		my @fullfilename = <$DevKitPackagesDirectory\\$file*>;
		$fullfilename[0] =~ m/([^\\\/]*)$/i;
		my $filename = $1;

		if (! -e "$TargetDirectory\\$filename") 
		{
			copy "$DevKitPackagesDirectory\\$filename",  "$TargetDirectory\\$filename";
			print "  copied:  $filename\n";
		}
	}

}


# --------------------------------- Start of WriteSetupIni() ----------------------------------------


sub WriteSetupIni()
{
	# write setup.ini with current KitID

	print "Writing setup.ini...\n";


	open (FILE, ">$TargetDirectory\\setup.ini") or die "Couldn't open file $TargetDirectory\\setup.ini";

	print FILE "[Startup]\n";
	print FILE "FreeDiskSpace=0\n";
	print FILE "AppName=Symbian OS Kit Installer\n";
	print FILE "[Languages]\n";
	print FILE "Default=0x0009\n";
	print FILE "count=1\n";
	print FILE "key0=0x0009\n";
	print FILE "[Symbian]\n";
	print FILE "KitID=$KitBuildID\n";
	print FILE "TargetDirectory=Symbian\n";
	print FILE "DiskSpaceRequirementMessage=Please note that a full installation of the Kit may require up to 2.0Gb of disk space on an NTFS partition or 2.5Gb on a FAT32 partition, in addition to around 500Mb of space in the system Temporary directory.\n";

	close(FILE);
}

# --------------------------------- BuildChangedPackages() ----------------------------------------
sub BuildChangedPackages()
{
	print "Building new packages...\n";

	SearchAndReplace("X.X", $Version, "com.symbian.api.StrongCrypto.template", "com.symbian.api.StrongCrypto.pkgdef");
	SearchAndReplace("X.X", $Version, "com.symbian.bak.bin.template", "com.symbian.bak.bin.pkgdef");
	SearchAndReplace("X.X", $Version, "com.symbian.bak.doc-old.layout.template", "com.symbian.bak.doc.pkgdef") if ($oldLayoutIntroPages);
	SearchAndReplace("X.X", $Version, "com.symbian.bak.doc.template", "com.symbian.bak.doc.pkgdef") if (!$oldLayoutIntroPages);
	SearchAndReplace("X.X", $Version, "com.symbian.bak.src.template", "com.symbian.bak.src.pkgdef");
	SearchAndReplace("X.X", $Version, "com.symbian.bak.tool.template", "com.symbian.bak.tool.pkgdef");

	system ("buildpkg.exe -v -f -b $KitBuildID -k $KitBuildID com.symbian.api.StrongCrypto.pkgdef");
	system ("buildpkg.exe -v -f -b $KitBuildID -k $KitBuildID com.symbian.bak.bin.pkgdef");
	system ("buildpkg.exe -v -f -b $KitBuildID -k $KitBuildID com.symbian.bak.doc.pkgdef");
	system ("buildpkg.exe -v -f -b $KitBuildID -k $KitBuildID com.symbian.bak.src.pkgdef");
	system ("buildpkg.exe -v -f -b $KitBuildID -k $KitBuildID com.symbian.bak.tool.pkgdef");
}


# --------------------------------- BuildNewNavigationPages() ----------------------------------------


sub BuildNewNavigationPages()
{
	print "Building new navigation pages...\n";

	system ("rd [sdkroot] /s/q");

	system ("unzip -q -o $DevKitPackagesDirectory\\com.symbian.doc.intro-pages_0_0* -x package.xml");
	system ("copy start.html [sdkroot]\\doc");
	system ("copy readme.html [sdkroot]\\doc\\documents") if (($Version !~ /^7\./) && ($Version !~ /^8\./)); # for v9 and later
	system ("copy aboutdevkit*.gif [sdkroot]\\doc\\Graphics");
	SearchAndReplace("X.X", $Version, "com.symbian.doc.intro-pages.template", "com.symbian.doc.intro-pages.pkgdef");
	system ("buildpkg.exe -v -f -b $KitBuildID -k $KitBuildID com.symbian.doc.intro-pages.pkgdef");
}



# --------------------------------- BuildNewSrcDefFile() ----------------------------------------


sub BuildNewSrcDefFile()
{
	SearchAndReplace("%version%", $KitBuildID, "DevKit.srcdef.template", "$KitBuildID.srcdef");
}




# --------------------------------- Main() ----------------------------------------

sub main()
{
	# N.B. ensure path contains \epoc32\tools;\epoc32\gcc\bin;

	CommandLineInterface();


	$oldLayoutIntroPages = 1 if ($Version =~ /^7\./) || ($Version =~ /^8\.0/); # set TRUE for v8.0 and previous


	# get required pre-built package definitions and installer
	CopyPreBuiltPackagesAndInstaller($DevKitPackagesDirectory, $TargetDirectory);


	# write the new setup.ini file
	WriteSetupIni();


	# build changed packages and move to target directory
	BuildChangedPackages();


	# build new navigation pages 
	BuildNewNavigationPages() if (!$oldLayoutIntroPages);


	# move changed packages to target directory
	foreach my $sdkpkg (<*.sdkpkg>)
	{
		if (move ($sdkpkg, $TargetDirectory) == 0)
		{	print $!; }
	}
	unlink <*.pkgdef>;


	# build srcdef file for this Kit if necessary
	my $SrcDefFilePreExisted = 1;
	if ($SrcDefFile eq "") 
	{
		BuildNewSrcDefFile();
		my $dir = cwd;
		$SrcDefFile = "$dir\\$KitBuildID.srcdef";
		$SrcDefFilePreExisted = 0;
	}
	

	# copy the readme.html file and licence text to root of BAK
	system ("copy [sdkroot]\\doc\\documents\\readme.html $TargetDirectory") if (!$oldLayoutIntroPages);
	system ("copy [sdkroot]\\doc\\packages\\sdl7.0\\intro_pages\\readme.html $TargetDirectory") if ($oldLayoutIntroPages);
	system ("copy licence.txt $TargetDirectory");


	# create XML package source file
	print "Creating XML source file...\n";
	chdir $TargetDirectory;
	system("createpkgsrc -s \"$SrcDefFile\"");
	unlink $SrcDefFile if ($SrcDefFilePreExisted == 0);

}


