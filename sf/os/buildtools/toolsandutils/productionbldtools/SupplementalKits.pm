#!perl

# SupplementalKits.pl

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
#


package SupplementalKits;


use File::Path;
use FindBin;                # where does this script live?
#use lib $FindBin::Bin;      # use this directory for libraries, to find...
use KitStandardLocations;



my $PlatformProductToolsDir = $FindBin::Bin;
$PlatformProductToolsDir =~ s/common/$Platform/i;    # change "common" to platform name in path to tools directory
unshift (@INC, $PlatformProductToolsDir);            # add this to the @INC list of library paths
require KitPlatformSpecific;                         # and read the platform-specific definitions
require $BuildBaseName."_supplementary";

my $scriptFileSupplementaryKits = "InstallSupplementaryKit.pl";



# ------------------------------------- main routine -----------------------------------

sub SupplementalKits()
{
	# set up environment & preserve old values
	my $OutputDirWithoutDrive = $OutputDir;
	$OutputDirWithoutDrive =~ s/^\w:(.*)$/$1/i;
	my $oldEpocRoot = $ENV{EPOCROOT};
	my $oldPath = $ENV{path};
	my $oldOutputDir = $ENV{OUTPUTDIR};
	$ENV{EPOCROOT} ="$OutputDirWithoutDrive\\techview\\";
	$ENV{path} = "$ENV{EPOCROOT}epoc32\\tools\\;$ENV{EPOCROOT}epoc32\\gcc\\bin\\;".$ENV{path};
	$ENV{OUTPUTDIR} = "$ProductPath\\SupplementaryProducts";



	# read [emul] tag and replace it in the files
	foreach my $package (@replaceEmulPackage)
	{
		replaceEmul($package);
	}


	# set up the source directory path for .bin packages
	foreach my $package (@addSourcePackage)
	{
		addSource($package->[0], eval $package->[1]);
	}


	# create Supplementary products directory
	(mkpath ($ENV{OUTPUTDIR}) or print "WARNING: Unable to create directory $ENV{OUTPUTDIR} - $!\n") if (! -d $ENV{OUTPUTDIR});



	# main build procedure
	foreach my $directory (@requiredSupplementaryKits)
	{
	print "** Supplemental Kit:  building from $directory\n";
		# set up the source directory path for .src packages in this product
		chdir ("$SuppKitPath\\$directory\\pkgdef");
		foreach my $sourcePackage (<$SuppKitPath\\$directory\\pkgdef\\com.symbian.src.*>)
		{
			addSource($sourcePackage, $SourceDir);
		}
		chdir ("$SuppKitPath\\$directory\\group");
		system ("bldmake bldfiles");
		system ("abld makefile");
		system ("abld target tools");
	}


	# copy Supplementary Products installation script into the right directory
	my $ProductToolsDir = $FindBin::Bin;
	$ProductToolsDir =~ s|/|\\|g;
	system("copy ".$ProductToolsDir."\\".$scriptFileSupplementaryKits." ".$ENV{OUTPUTDIR});



	# restore environment & finish
	$ENV{EPOCROOT} =  $oldEpocRoot;
	$ENV{path} = $oldPath;
	$ENV{OUTPUTDIR} = $oldOutputDir;

}




# ------------------------------------- Sub procedures -----------------------------------


# function inserts the leading directory in the src= statement
sub addSource()
{
    my $pkgDefFile = $_[0] ;
    my $srcDir = $_[1] ;


    # read source definition file
    open (FILE, "<$pkgDefFile") or print "WARNING: Supplementary Kits: Couldn't read file $pkgDefFile\n";
    my @pkgList = <FILE>;
    close(FILE);
    unlink($pkgDefFile);

    # write file with replaced src=
    open (FILE, ">$pkgDefFile") or print "WARNING: Supplementary Kits: Couldn't write file $pkgDefFile\n";
    foreach $line (@pkgList)
    {
		# insert source directory at start
		$line =~ s|src="(\\){0,1}|src="$srcDir\\|gi;

		# remove double slashes if present
		$line =~ s|\\\\|\\|gi ;
        
    	print FILE $line ;
    }
    close(FILE);
}




# function reads [emul] tag and replaces it in the file specified as the parameter
sub replaceEmul()
{
    my $emul = "" ;
    my $pkgDefFile = $_[0] ;

    # read source definition file
    open (FILE, "<$pkgDefFile") or print "WARNING: Supplementary Kits: Couldn't read file $pkgDefFile\n";
    my @pkgList = <FILE>;
    close(FILE);
    

    # look for definition of [emul]
    foreach $line (@pkgList)
    {
        if ( $line =~ m|emul\]\">(.*?)<|i )
        {
            $emul = $1;

            # remove [sdkroot] if present
            if ( $emul =~ m|\[sdkroot\](.*)|i )
            {
                $emul = $1;
            }
            
            last;
        }
    }


    # if [emul] not defined, enter a default
    if ($emul EQ "")
    {
        $emul = "bin\\TechView";
    }

   
    # write file with replaced emul
    open (FILE, ">$pkgDefFile") or print "WARNING: Supplementary Kits: Couldn't write file $pkgDefFile\n";
    foreach $line (@pkgList)
    {
        if ($line =~ m|<item|gi)
        {
    	    $line =~ s|\[emul\]|$emul|g ;
        }
    	print FILE $line ;
    }
    close(FILE);
}
