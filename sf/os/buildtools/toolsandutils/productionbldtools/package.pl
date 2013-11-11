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
# Version 007: Master Codeline - beech/cedar split & DocPlugInTool removed
# Version 008: Master Codeline - custkit.txt generation removed
# 
#



use Cwd;
use File::Basename;
use File::Path;

use FindBin;                # where does this script live?
use lib $FindBin::Bin;      # use this directory for libraries, to find...
use KitStandardLocations;

my $PlatformProductToolsDir = $FindBin::Bin;
$PlatformProductToolsDir =~ s/common/$Platform/i;    # change "common" to platform name in path to tools directory
unshift (@INC, $PlatformProductToolsDir);            # add this to the @INC list of library paths
require KitPlatformSpecific;                         # and read the platform-specific definitions
require AllKitsRebaseline;
require BuildKit;
require TestKit;
require MakeCustKitZips;

# ------------------------------ Global variables ----------------------------

my $Company = "Symbian";


my $BuildNumber = $ARGV[0];
$Kit_ID = $BuildNumber;

my $pkgSrcDefDevKitDir = "$DevKitPath\\$SrcDefDir";
my $pkgSrcDefCustKitDir ="$CustKitPath\\$SrcDefDir";

my $outputCustKitZipsDir =  "$ProductPath\\_PACKAGES\\CustKit_Zips";
my $outputInternalZipsDir = "$ProductPath\\generated";


my $KitBuilderSrcDir =    "$SourceDir\\common\\generic\\tools\\CustKits\\KitBuilder";
my $KitBuilderBldDir =    "$OutputDir\\techview\\epoc32\\tools\\CustKits\\KitBuilder";
my $NavigationPagesDir =  "$SourceDir\\common\\generic\\tools\\CustKits\\NavigationPages";


my $sourceListDevKit =  "DevKit.srcdef";
my $sourceListCustKit = "CustKit.srcdef";


my $buildPkg =     "$OutputDir\\generic\\epoc32\\tools\\buildpkg";
my $createPkgSrc = "$OutputDir\\generic\\epoc32\\tools\\createpkgsrc";
my $diff =         "$OutputDir\\generic\\epoc32\\gcc\\bin\\diff";

my $scriptFile =     "InstallCustKit.pl";
my $scriptFileZips = "InstallCustKitZIPs.pl";
my $scriptHelpFile = "ReadMe_First.txt";



# ----------------- Start of main() ------------------------

open STDERR, ">&STDOUT" ;
select STDOUT; $| = 1;
select STDERR; $| = 1;

my $time=localtime;
print "===  Build process for Kit $Kit_ID started at $time\n";


# get the drive we're running on - use for various parameters later on
my $initialDir = cwd();
$initialDir =~ m/^(\w:)/ ;
my $initialDrive = $1;


my $CommonProductToolsDir = $FindBin::Bin;
$CommonProductToolsDir =~ s|/|\\|g;
$PlatformProductToolsDir =~ s|/|\\|g;


# Generate a list of available files
print "** generate .lis files\n";
my $logsPath = "$ProductPath\\generated\\logs";		# ensure we can create log files
(mkpath $logsPath or print "WARNING:  Couldn't create directory for logs\n") if (! -d $logsPath);
chdir("$OutputDir\\generic");
system("perl $CommonProductToolsDir\\bfrc\\listdir.pl --files epoc32 > $logsPath\\GT.lis");
chdir("$OutputDir\\techview");
system("perl $CommonProductToolsDir\\bfrc\\listdir.pl --files epoc32 > $logsPath\\TechView.lis");



# run tools to produce up-to-date packages and compare to previous build's package definitions
print "** RebaselineAndCompare()\n";
RebaselineAndCompare();



# generate the licencee list of restricted files
print "** LicenceeRestrictionsList\n";
LicenceeRestrictionsList();



# build the packages into Kits
print "** BuildKits()\n";
BuildKit::BuildKit();



# produce CustKit as a set of ZIPs, with script and HTML navigation pages
# print "** CustKit as ZIPs\n";
# CustKitAsZips();



# produce kit report and difference between this report & previous build's
TestKit::TestKit();
system ("$diff --text --speed-large-files $ENV{'PublishLocation'}\\$ENV{'Type'}\\$ENV{'Previousbuildnumber'}\\logs\\KitReport.txt $ENV{'LogsDir'}\\KitReport.txt > $ENV{'LogsDir'}\\KitReportDiffs.txt");



$time=localtime;
print "===  Build process for Kit $Kit_ID ended at $time\n";





# ----------------- Start of LicenceeRestrictionsList() ------------------------
sub LicenceeRestrictionsList()
{
	my $PackageDefs_copy = "\\PackageDefinitions_copy";
	mkdir $KitBuilderBldDir."\\".$PackageDefs_copy, 0777 ;
	system("xcopy  $pkgDefDevKitDir  $KitBuilderBldDir$PackageDefs_copy /E /Z /I /Q"); 
	system("del $KitBuilderBldDir\\$PackageDefs_copy /F /Q");
}



# ----------------- Start of RebaselineAndCompare() ------------------------
sub RebaselineAndCompare()
{
	chdir($CommonProductToolsDir) or die "Unable to change to $CommonProductToolsDir\n";
	AllKitsRebaseline::AllKitsRebaseline("$ProductPath\\generated");

}


# ----------------- Start of CustKitAsZips() ------------------------
sub CustKitAsZips()
{
	chdir($PlatformProductToolsDir) or die "Unable to change to $PlatformProductToolsDir\n";
	MakeCustKitZIPs::MakeCustKitZIPs ($pkgDefCustKitDir, $pkgDefDevKitDir, $outputCustKitZipsDir, "$ProductPath\\generated", $outputInternalZipsDir);
	system("copy ".$PlatformProductToolsDir."\\".$scriptFileZips." ".$outputCustKitZipsDir);
	system("xcopy ".$NavigationPagesDir." ".$outputCustKitZipsDir." /S /I /Q") ;
	system("del ".$outputCustKitZipsDir."\\distribution.policy /F /Q") ;
}
