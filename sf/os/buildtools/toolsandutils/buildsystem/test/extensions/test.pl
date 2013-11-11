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
# template extension makefile test
#
#

use strict;
use Test;

# how many tests do we plan to run?
BEGIN { plan tests => 6 }

# remember where we started
use Cwd;
my $cwd = getcwd();

# change to the test data directory
use File::Basename;
my $dir = dirname($0);
chdir($dir);
print "# running in $dir\n";

my $epocroot = $ENV{'EPOCROOT'};

###############################################################################
# THE TESTS                                                                   #
###############################################################################

# check base/copy_default in a [non] feature variant world
my $mk = "../../extension/base/copy_default.mk";
my $name = "my_testing";
my $path = "${epocroot}epoc32/release/armv5/udeb/$name";
my $vars = "SHELL=cmd PLATFORM_PATH=armv5 CFG_PATH=udeb SOURCES=$name.01 TARGET=$name.loc";

use File::Copy;
copy("$name.01", "$path.01");		# no variants
unlink("$path.loc");

system("make -f $mk $vars BLD");
system("make -f $mk $vars RELEASABLES");

print "# checking $path.loc\n";
ok(-f "$path.loc");

unlink("$path.01");
system("make -f $mk $vars CLEAN");
ok(! -f "$path.loc");

my $v1 = "11111111101111111110111111111011";	# variant
my $v2 = "22222222202222222220222222222022";	# variant
copy("$name.01", "$path.$v1.01");
copy("$name.01", "$path.$v2.01");

system("make -f $mk $vars BLD");
system("make -f $mk $vars RELEASABLES");

print "# checking $path.$v1.loc\n";
ok(-f "$path.$v1.loc");
print "# checking $path.$v2.loc\n";
ok(-f "$path.$v2.loc");

unlink("$path.$v1.01");
unlink("$path.$v2.01");

system("make -f $mk $vars CLEAN");
ok(! -f "$path.$v1.loc");
ok(! -f "$path.$v2.loc");

###############################################################################
# END OF TESTS                                                                #
###############################################################################

# change back to the starting directory
chdir($cwd);

