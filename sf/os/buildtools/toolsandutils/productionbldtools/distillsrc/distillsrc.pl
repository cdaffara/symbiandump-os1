#!/bin/perl -w
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
# distillsrc.pl - compiles a list of source used in .mrp files, and deletes
# any unused source
# 
#

use strict;
use FindBin;
use Getopt::Long;
use File::Spec;

use lib $FindBin::Bin;
use distillsrc;

my $errorLevel = 0;

# Distillsrc is to continue on error rather than die.  A warning handler is used
# so that we can still exit with an error level.
$SIG{__WARN__} = sub {warn @_;
                      $errorLevel = 1};

sub dieHelp()
	{
	print <<HELP_EOF;

  Usage: perl distillsrc.pl [-f file] [-m file] [-c file] -r src_root
    -s source_path [-p src_prefix] -l platform [-n] [-d] [-h help];
	
  Options in detail:
    -file|f file : components.txt file to read (may be more than one)
    -mrp|m file : additional .mrp file to read (may be more than one)
    -config|c file : a configuration file to use containing .mrp files to use
    -r src_root : The root from which all src statements are based
    -s source_path : The path under src_root to the source tree to be processed
    -p src_prefix : An optional prefix which can be stripped from all src
      statements
    -l platform : The platform (beech etc), needed in order to locate the
      correct product directory
    -n : don't check the case of files in the config file and in .mrps against
      the file system
    -d : dummy mode
      
  Note that the src_prefix in combination with the src_root can be used to
  displace the source tree, if it isn't in it's final location.
HELP_EOF
	exit 1;
	}

# Read command line options to find out if we're doing a dummy run, and where the options are
my @files = ();
my @mrps = ();
my $srcroot;
my $srcprefix;
my $dummy = 0;
my $srcpath;
my $config;
my $platform;
my $help;
my $nocheckcase;
GetOptions("config=s" => \$config, "file=s" => \@files, "mrp=s" => \@mrps, "srcroot|r=s" => \$srcroot, "srcpath|s=s" => \$srcpath, "srcprefix|p=s" => \$srcprefix, "platform|l=s" => \$platform, "dummy" => \$dummy, "help" => \$help, "nocheck|n" => \$nocheckcase);

if ($help)
	{
	dieHelp();
	}


my $distiller=New CDistillSrc($srcroot, $srcpath, $srcprefix, $platform, !$nocheckcase);

if (!defined($distiller))
	{
	dieHelp();
	}

if (!($distiller->LoadMrps($config, \@files, \@mrps)))
	{
	exit 1; # Couldn't read all the mrps - don't start deleting source based on incomplete data
	}

$distiller->Print(0);

# Run through the source tree, matching every file against the source items. Anything that doesn't, delete it (or warn, if we're doing a dummy run).
	
$distiller->DistillSrc($dummy);

exit $errorLevel;
