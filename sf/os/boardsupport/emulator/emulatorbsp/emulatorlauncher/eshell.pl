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
# Eshell Launcher
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
    	&launchEshell("udeb","winscw");
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
    		&launchEshell("urel","winscw");
    		exit(0);
		}

	if(lc($ARGV[0]) eq "-wins") 
		{
    		&launchEshell("udeb", "wins");
    		exit(0);
		}

	if(lc($ARGV[0]) eq "-winscw") 
		{
    		&launchEshell("udeb", "winscw");
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
    			&launchEshell("urel","wins");
    			exit(0);
			}

		if (lc($ARGV[1]) eq "-winscw")
			{
    			&launchEshell("urel","winscw");
    			exit(0);
			}
		}	
	
	if (lc($ARGV[0]) eq "-winscw")
		{
		if (lc($ARGV[1] eq "-rel"))
			{
			&launchEshell("urel","winscw");
			exit(0);	
			}
		}
	
	if (lc($ARGV[0]) eq "-wins")
		{
		if (lc($ARGV[1] eq "-rel"))
			{
			&launchEshell("urel","wins");
			exit(0);	
			}
		}	
	}

# Error, unknown argument.
&printHelp;
die "ERROR: Unknown argument " . "\"" . $ARGV[0] . "\".\n";

sub launchEshell
{
    my ($type,$win) = @_;
    $epocroot = &getEpocroot;
    $drive = &getDrive;
    $emu = $drive . $epocroot . "epoc32" . "\\" 
	. "release\\" . $win . "\\" . $type . "\\" . "eshell.exe";
    -e $emu ||
	die "ERROR: File \"$emu\" not found.\n\n" .
	    "The EPOCROOT environment variable does not identify\n" .
	    "a valid eshell installation on this drive.\n" . 
		"EPOCROOT must be an absolute path to an existing\n" .
		    "directory - it should have no drive qualifier and\n" .
			"must end with a backslash.\n";

	#add the stuff to use the console
	$emu.=" -MConsole --";
			
    # If the execute is successful, this never returns.
    exec($emu) || die "Failed to execute eshell \"$emu\": $!";
}

sub printHelp
{
    print "Eshell Launcher\n";
    print "Syntax :\teshell [-rel] [-wins|-winscw] [-help]\n";
    print "(no options)\tLaunch active winscw debug eshell\n";
    print "-rel\t\tLaunch active release eshell\n";
    print "-wins\t\tLaunch active wins eshell\n";
    print "-winscw\t\tLaunch active winscw eshell\n";
    print "-help\t\tOutput this help message\n";
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
    if($wd =~ /^([a-zA-Z]:)/) {
	$drive = $1;
    } else {
	# Perhaps we're on a machine that has no drives.
	$drive = "";
    }
    return $drive;
}
