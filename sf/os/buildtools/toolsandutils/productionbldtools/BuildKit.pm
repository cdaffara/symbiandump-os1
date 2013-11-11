#!perl

# package.pl

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
# Version 002: Typhoon: InstallCustKit.pl added for CustKit 
# Version 003: Typhoon: calls GenerateLicenseeBuild.exe before building
# Version 004: Typhoon: auto-generates packages before building; 
# produces differences between this report & previous build's
# calls script to produce CustKit as ZIPs
# Version 005: Typhoon: calls docplugintool script 
# Version 006: Typhoon: Master Codeline
# Version 007: Master Codeline
# 
#

package BuildKit;

use Cwd;
use File::Basename;
use File::Path;

use KitStandardLocations;
use KitPlatformSpecific;
use SupplementalKits;



# ------------------------------ Global variables ----------------------------

my $Company = "Symbian";


my $BuildNumber = $ARGV[0];
$Kit_ID = $BuildNumber;

my $pkgSrcDefDevKitDir = "$DevKitPath\\$SrcDefDir";
my $pkgSrcDefCustKitDir ="$CustKitPath\\$SrcDefDir";

my $outputDevKitDir =       "$ProductPath\\_PACKAGES\\DevKit_CD";
my $outputCustKitDir =      "$ProductPath\\_PACKAGES\\CustKit_Extra_CD";
my $outputCustKitZipsDir =  "$ProductPath\\_PACKAGES\\CustKit_Zips";
my $outputInternalZipsDir = "$ProductPath\\generated";

my $sourceListDevKit =  "DevKit.srcdef";
my $sourceListCustKit = "CustKit.srcdef";

my $buildPkg =     "$OutputDir\\generic\\epoc32\\tools\\buildpkg";
my $createPkgSrc = "$OutputDir\\generic\\epoc32\\tools\\createpkgsrc";
my $diff =         "$OutputDir\\generic\\epoc32\\gcc\\bin\\diff";

my $scriptFile =     "InstallCustKit.pl";
my $scriptHelpFile = "ReadMe_First.txt";

# ----------------- Start of main() ------------------------

sub BuildKit()
{
	# build the Symbian supplied packages
	buildPkgSet(\@KitPlatformSpecific::SymbianPkgs);

	# create the packages etc. for the DevKit
	createDistrib($outputDevKitDir, $pkgSrcDefDevKitDir."\\".$sourceListDevKit);

	SupplementalKits::SupplementalKits();
}



# function builds a set of packages
# function returns true if all the packages in the set were built correctly
sub buildPkgSet()
{
    my ($packageSet)=@_ ;
    my $success = 1;

    # loop through all the packages in the package set
    foreach my $packageDef (@$packageSet)
    {
		# build this package using the buildpkg tool

		print "Building package ".$packageDef->[1]."\n";

		my $srcRootArg = "";
		my $srcroot = $packageDef->[0];

		#check if the source root is empty
		if(!($srcroot eq ""))
		{
		    # the source root argument
		    $srcRootArg = " -s ".$srcroot;
		}

		my $command = $buildPkg." -v -f -o ".$packageDef->[2]." -b ".$BuildNumber." -k ".$Kit_ID.$srcRootArg." ".$packageDef->[1];
		$success &= !system($command);
	}
	return $success;
}




# function collates the distribution directory contents
sub createDistrib()
{
    my $DistribDir = $_[0] ;
    my $PkgSrcList = $_[1];
    print "Creating distribution directory in $DistribDir\n";

    # create the package source for the CustKit
    createSrc($DistribDir, $PkgSrcList);

    # add the KSA files to make the kit installable
    addKSA($DistribDir);

    # create the setup.ini file for the kit
    createSetupIni($DistribDir);
}





# function creates a package source
sub createSrc()
{
    chdir($_[0]);
    
    my $pkgSrcDefFile = $_[1] ;

    print "  creating package source using list in $pkgSrcDefFile \n";

    # read source definition file
    open (FILE, "<$pkgSrcDefFile") or die "Couldn't open file $pkgSrcDefFile ";
    my @pkgList = <FILE>;
    close(FILE);
    
    # remove previous copy
    unlink ($pkgSrcDefFile);
    
    # write file with build number
    open (FILE, ">$pkgSrcDefFile") or die "Couldn't open file $pkgSrcDefFile";
    foreach $file (@pkgList)
    {
    	$file =~ s/%version%/$BuildNumber/g ;
    	print FILE $file ;
    }
    close(FILE);

    # build the package source XML
    my $command = $createPkgSrc." -s ".$pkgSrcDefFile ;
    system($command);
}




# function amends setup.ini file created by the KSA
sub createSetupIni()
{
    chdir($_[0]);
    print "  amending setup.ini file\n";


    # create setup.ini file
    open (FILE, ">>setup.ini") or die "Couldn't open setup.ini";

    print FILE "[Symbian]\n";
    print FILE "KitID=$Kit_ID\n";
    print FILE "TargetDirectory=Symbian";

    close(FILE);
}




# function creates adds the KSA files to the Kit
sub addKSA()
{
    chdir($_[0]);
    print "  copying KSA\n";

    system("copy $OutputDir\\generic\\epoc32\\ksa\\autorun.inf");
    system("copy $OutputDir\\generic\\epoc32\\ksa\\data1.cab");
    system("copy $OutputDir\\generic\\epoc32\\ksa\\data1.hdr");
    system("copy $OutputDir\\generic\\epoc32\\ksa\\data2.cab");
    system("copy $OutputDir\\generic\\epoc32\\ksa\\ikernel.ex_");
    system("copy $OutputDir\\generic\\epoc32\\ksa\\layout.bin");
    system("copy $OutputDir\\generic\\epoc32\\ksa\\Setup.exe");
    system("copy $OutputDir\\generic\\epoc32\\ksa\\setup.inx");
    system("copy $OutputDir\\generic\\epoc32\\ksa\\setup.ini");
    system("copy $OutputDir\\generic\\epoc32\\ksa\\ReadMe.*");
}


