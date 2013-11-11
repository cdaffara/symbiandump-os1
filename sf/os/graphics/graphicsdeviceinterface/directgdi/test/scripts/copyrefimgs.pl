#!/usr/bin/perl
# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Script to automate copying of DirectGDI reference images to location where comparison tests can be run.
# 
#

use strict;
use Cwd;

my $srcloc = $ARGV[0] ;#or die "No argument given";
if (!$srcloc)
{
	print "Please specify the source directory which contains graphics component.\n";
	exit;
}
my $imagedir = "\\directgdi\\test\\images\\reference";
# Check argument given exists
if (! -d $srcloc.$imagedir)
{
	print "$srcloc is not a valid source code location for the graphics component.\n";
	exit;
}

# Must be run from \epoc32\winscw\c\ or \epoc32\data\c
if(getcwd() !~ /c\/img/i)
{
print "This tool should be run from \\epoc32\\winscw\\c\\img\\ or \\epoc32\\data\\c\\img\\\n";
exit
}

my $copycmd = "unzip -qo $srcloc$imagedir\\reference.zip -d .\\ref";
print $copycmd."\n";
print `$copycmd`;
my $copycmd2 = "unzip -qo $srcloc$imagedir\\replacements_from_directgdi\\replacement.zip -d .\\ref";
print $copycmd2."\n";
print `$copycmd2`;

