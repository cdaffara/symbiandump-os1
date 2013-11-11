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
# hello world build test
#
# If we cannot build this then we probably cannot build anything...

use strict;
use Test;

# how many tests do we plan to run?
BEGIN { plan tests => 4 }

# remember where we started
use Cwd;
my $cwd = getcwd();

# change to the test data directory
use File::Basename;
my $dir = dirname($0);
chdir($dir);
print "# running in $dir\n";

# remove the binaries if they already exist
my $release = $ENV{'EPOCROOT'} . "epoc32/release";
my @binaries = ("$release/winscw/udeb/HelloWorld.exe",
			    "$release/winscw/urel/HelloWorld.exe",
				"$release/armv5/udeb/HelloWorld.exe",
				"$release/armv5/urel/HelloWorld.exe"
			   );

foreach (@binaries)
{
	unlink($_);
}

###############################################################################
# THE TESTS                                                                   #
###############################################################################

# we cannot test the command return values because they are always 0
system("bldmake bldfiles");
system("abld build");

# test for the existence of each binary file
foreach (@binaries)
{
	print "# checking $_\n";
	ok(-f $_);
}

###############################################################################
# END OF TESTS                                                                #
###############################################################################

# change back to the starting directory
chdir($cwd);

