# Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Launcher for the Symbian Emulator, including
# functionality to read the $ENV{EPOCROOT}epoc32\data\BuildInfo.txt
# file to find out information regarding the current Emulator.
# Depends on the current working directory providing
# the drive of the currently used SDK.
# 
#

use Cwd;

#
# Check the argument(s), if any.
#
$numArgs = $#ARGV + 1;

if($numArgs == 0) 
	{
    	&launchEmulator("udeb","winscw");
    	exit(0);
	}

if($numArgs > 2) 
	{
    	&printHelp;
    	die "ERROR: Too many arguments.\n";
	}

if($numArgs == 1)
	{ 
	if(lc($ARGV[0]) eq "-rel") 
		{
    		&launchEmulator("urel","winscw");
    		exit(0);
		}

	if (lc($ARGV[0]) eq "-version") 
		{
    		&printVersion;
    		exit(0);
		}

	if(lc($ARGV[0]) eq "-wins") 
		{
    		&launchEmulator("udeb", "wins");
    		exit(0);
		}

	if(lc($ARGV[0]) eq "-winscw") 
		{
    		&launchEmulator("udeb", "winscw");
    		exit(0);
		}

	if(lc($ARGV[0]) eq "-help") 
		{
    		&printHelp;
    		exit(0);
		}
	}

if ($numArgs == 2)
	{
	if(lc($ARGV[0]) eq "-rel") 
		{
		if (lc($ARGV[1]) eq "-wins")
			{
    			&launchEmulator("urel","wins");
    			exit(0);
			}

		if (lc($ARGV[1]) eq "-winscw")
			{
    			&launchEmulator("urel","winscw");
    			exit(0);
			}
		}	
	
	if (lc($ARGV[0]) eq "-winscw")
		{
		if (lc($ARGV[1] eq "-rel"))
			{
			&launchEmulator("urel","winscw");
			exit(0);	
			}
		}
	
	if (lc($ARGV[0]) eq "-wins")
		{
		if (lc($ARGV[1] eq "-rel"))
			{
			&launchEmulator("urel","wins");
			exit(0);	
			}
		}	
	}

# Error, unknown argument.
&printHelp;
die "ERROR: Unknown argument " . "\"" . $ARGV[0] . "\".\n";

sub launchEmulator
{
    my ($type,$win) = @_;

    my $epocroot = &getEpocroot;
    my $drive = &getDrive;
    my $emu = $drive . $epocroot . "epoc32" . "\\" 
	      . "release\\" . $win . "\\" . $type . "\\" . "epoc.exe";
    -e $emu ||
	die "ERROR: File \"$emu\" not found.\n\n" .
	    "The EPOCROOT environment variable does not identify\n" .
	    "a valid Symbian emulator installation on this drive.\n" . 
		"EPOCROOT must be an absolute path to an existing\n" .
		    "directory - it should have no drive qualifier and\n" .
			"must end with a backslash.\n";
    # If the execute is successful, this never returns.
    exec("\"" . $emu . "\"") || die "Failed to execute the emulator \"$emu\": $!";
}

sub printHelp
{
    print "Symbian Platform Emulator Launcher\n";
    print "Syntax :\tepoc [-rel] [-wins|-winscw] [-version] [-help]\n";
    print "(no options)\tLaunch active winscw debug emulator\n";
    print "-rel\t\tLaunch active release emulator\n";
    print "-wins\t\tLaunch active wins emulator\n";
    print "-winscw\t\tLaunch active winscw emulator\n";
    print "-version\tDisplay active emulator details\n";
    print "-help\tOutput this help message\n";
}

sub printVersion
{
    my $epocroot = &getEpocroot;
    my $drive = &getDrive;

    my $binfo = $drive . $epocroot . "epoc32" . "\\" 
	        . "data" . "\\" . "BuildInfo.txt";

    -e $binfo || die "ERROR: File \"" . $binfo . "\" does not exist.\n";
    open(IFILE, $binfo) ||
	die "ERROR: Failed to open file \"" . $binfo . "\": $!";

    my $DeviceFamily = "";
    my $DeviceFamilyRev = "";
    my $ManufacturerSoftwareRev = "";
    my $ManufacturerSoftwareBuild = "";

    while(<IFILE>) {
	if(/DeviceFamily\s+(.*\S)\s*$/i) {
	    $DeviceFamily = $1;
	}
	if(/DeviceFamilyRev\s+(.*\S)\s*$/i) {
	    $DeviceFamilyRev = $1;
	}
	if(/ManufacturerSoftwareRev\s+(.*\S)\s*$/i) {
	    $ManufacturerSoftwareRev = $1;
	}
	if(/ManufacturerSoftwareBuild\s+(.*\S)\s*$/i) {
	    $ManufacturerSoftwareBuild = $1;
	}
    }

    close(IFILE);

    #
    # Verify that we got everything we should have.
    #
    $DeviceFamily ne "" ||
	die "ERROR: Device family not specified in file \"" . $binfo .
	    "\".\n";
    $DeviceFamilyRev ne "" ||
	die "ERROR: Device family revision not specified in file \"" . $binfo .
	    "\".\n";
    $ManufacturerSoftwareBuild ne "" ||
	die "ERROR: Manufacturer software build not specified in file \"" .
	    $binfo . "\".\n";

    $Revision = (($ManufacturerSoftwareRev eq "")?($DeviceFamilyRev):
		 ($ManufacturerSoftwareRev));

    $DeviceFamily = getDFRDName($DeviceFamily);

    #
    # Make the standard revision representation prettier,
    # but leave other representations untouched.
    #
    if($Revision =~ /^0x([0-9])([0-9][0-9])$/) {
	$Revision = $1 . "." . $2;
    }
   
    print $DeviceFamily . " " .
	"version " . $Revision . " " .
	    "build " . $ManufacturerSoftwareBuild . "\n";
}

#
# Determines, validates, and returns EPOCROOT.
#
sub getEpocroot
{
    my $epocroot = $ENV{EPOCROOT};
    die "ERROR: Must set the EPOCROOT environment variable.\n"
	if (!defined($epocroot));
    $epocroot =~ s-/-\\-go;	# for those working with UNIX shells
    die "ERROR: EPOCROOT must be an absolute path, " .
	"not containing a drive letter.\n" if ($epocroot !~ /^\\/);
    die "ERROR: EPOCROOT must not be a UNC path.\n" if ($epocroot =~ /^\\\\/);
    die "ERROR: EPOCROOT must end with a backslash.\n" if ($epocroot !~ /\\$/);
    die "ERROR: EPOCROOT must specify an existing directory.\n" 
	if (!-d $epocroot);
    return $epocroot;
}

#
# Determines and returns the current drive, if any.
#
sub getDrive
{
    my $wd = cwd;
    my $drive;
    if($wd =~ /^([a-zA-Z]:)/) {
	$drive = $1;
    } else {
	# Perhaps we're on a machine that has no drives.
	$drive = "";
    }
    return $drive;
}

#
# The DFRD may be represented by a numeric value, as defined by HAL.
# Changes known numeric values to the name of the DFRD,
# and leaves all other values untouched.
#
sub getDFRDName
{
    my $dfrd = shift;
    return "Crystal" if $dfrd eq "0";
    return "Pearl" if $dfrd eq "1";
    return "Quartz" if $dfrd eq "2";
    return $dfrd; # as fallback
}
