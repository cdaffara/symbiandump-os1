# Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

use strict;
use Cwd;		# for cwd
use File::Basename;	# for basename()
use FindBin;		# for FindBin::Bin
my $PerlBinPath;	# fully qualified pathname of the directory containing this script

my $epocroot;

# establish the path to the Perl binaries
BEGIN {
	require 5.005_03;		# check user has a version of perl that will cope
	$PerlBinPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlBinPath =~ s/\//\\/g;	# X:\epoc32\tools
}

use lib $PerlBinPath;
use lockit_info;

sub print_usage
	{
	print <<USAGE_EOF;

Usage:
  epocmbm [-h headerfile] [-o outputfile] [-b "bitmaps"] [-l "TargetPath:CWDir"]  

Compile the bitmaps to an EPOC MBM image file.
   -b	  -- list of bitmaps Eg., "-b/c8\\full path\\bmp1... /c8\\full path\\bmp2.."
   -l     -- if specified, captures all source to \\epoc32\\localisation\\...

USAGE_EOF
	}


#-----------------------------------------------
# Process commandline arguments
#

my $opt_o="";
my $opt_h="";	
my $opt_l="";
my $opt_b="";
my $opt_v=0;

my $errors = 0;
while (@ARGV)
	{
	my $arg = shift @ARGV;
	if ($arg =~ /^-o(.*)$/)
		{
		$opt_o =$1;
		next;
		}
	if ($arg =~ /^-h(.*)$/)
		{
		$opt_h =$1;
		next;
		}
	if ($arg =~ /^-b(.*)$/)
		{
		$opt_b =$1;
		next;
		}	
	if ($arg =~ /^-l(.*)$/)
		{
		$opt_l =$1;
		next;
		}

	if($arg =~ /^-/)
		{
		print "Unknown arg: $arg\n";
		$errors++;
		next;
		}
	}

if ($errors || $opt_b eq "")
	{
	print_usage();
	exit 1;
	}

my $headerfile=$opt_h;

if ($opt_b ne "")
	{
	$opt_h = "\/h\"$headerfile\"" if ($headerfile ne "");
	print "* bmconv /q $opt_h $opt_o $opt_b\n" if ($opt_v);
	system("bmconv /q $opt_h $opt_o $opt_b");
	if ($? != 0)
		{
		print "* BMCONV failed\n";
		exit 1;
		}
	}

if ($opt_l ne "")
	{
	my ($Resrc, $FileType) = split(/\./, basename($opt_o));
	&Lockit_SrcFile($Resrc, "", $opt_l, $FileType, $opt_b, $Resrc.".$FileType"); # ""
		}
exit 0;

sub Epocroot_Check
	{
	$epocroot = $ENV{EPOCROOT};
	die "ERROR: Must set the EPOCROOT environment variable\n" if (!defined($epocroot));
	$epocroot =~ s-/-\\-go;	# for those working with UNIX shells
	die "ERROR: EPOCROOT must not include a drive letter\n" if ($epocroot =~ /^.:/);
	die "ERROR: EPOCROOT must be an absolute path without a drive letter\n" if ($epocroot !~ /^\\/);
	die "ERROR: EPOCROOT must not be a UNC path\n" if ($epocroot =~ /^\\\\/);
	die "ERROR: EPOCROOT must end with a backslash\n" if ($epocroot !~ /\\$/);
	die "ERROR: EPOCROOT must specify an existing directory\n" if (!-d $epocroot);
	$epocroot=~ s-\\$--;		# chop trailing \\
	}
