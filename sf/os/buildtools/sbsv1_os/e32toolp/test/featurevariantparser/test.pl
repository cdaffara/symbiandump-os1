#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

# Unit tests for Feature Variant Parser (featurevariantparser.pm)
# Will only work with EPOCROOT = \, due to some hard coded paths in the VAR files
# To use a custom EPOCROOT, update the VAR files to reflect the correct paths
# This test script can either be called on its own, or run through the test harness under ../unittest.pl

use strict;
use Test;
use lib "../../e32util";
use lib "../e32util";
use featurevariantparser;
use File::Spec;
use File::Basename;

BEGIN { plan tests => 18 }

# Current Working Directory
my $dir = dirname($0);

my @buildincludes;
my $buildincs;
my $romincs;
my @romincludes;
my @parentnodes;
my $parents;
my @childnodes;
my $children;

# Uncommenting the next line will enable verbose mode
#$featurevariantparser::verbose=1;

# Load first variant file
my %variant1 = featurevariantparser->GetVariant( "default", "$dir" );

#1
print "\nCheck the NAME of the variant:\n";
ok( 'default', $variant1{'NAME'} );

#2
print "\nCheck the FULL PATH:\n";
ok( File::Spec->rel2abs("$dir\\default.var"), $variant1{'FULLPATH'} );

#3
print "\nCheck if variant is VALID:\n";
ok( $variant1{'VALID'} );

#4
print "\nCheck if this is a VIRTUAL node:\n";
ok( $variant1{'VIRTUAL'} );

#5
my $tmp = "$ENV{EPOCROOT}epoc32\\include\\variant\\Symbian_OS.hrh";
print "\nTest the VARIANT_HRH:\n";
ok( File::Spec->canonpath($tmp), $variant1{'VARIANT_HRH'} );

#6
print "\nTesting the BUILD_INCLUDES with 'append' keyword:\n";
@buildincludes = ("$ENV{EPOCROOT}epoc32/include/build");
$buildincs     = $variant1{'BUILD_INCLUDES'};
ok( File::Spec->canonpath("@buildincludes"), "@$buildincs" );

#7
print "\nTesting the ROM_INCLUDES:\n";
@romincludes = ("$ENV{EPOCROOT}epoc32\\include\\rom");
$romincs     = $variant1{'ROM_INCLUDES'};
ok( File::Spec->canonpath("@romincludes"), "@$romincs" );

# Load Second Variant File
my %variant2 = featurevariantparser->GetVariant( "a1", "$dir" );

#8
print "\nTesting PARENT NODES:\n";
@parentnodes = ("groupA default");
$parents     = $variant2{'PARENTS'};
ok( "@parentnodes", "@$parents" );

#9
print "\nTesting CHILD NODES:\n";
@childnodes = ( "b1a", "b1b" );
$children = $variant2{'CHILDREN'};
ok( "@childnodes", "@$children" );

#10
print "\nTesting BUILD_INCLUDES of a child variant with 'prepend' keyword:\n";
@buildincludes = (
    "$ENV{EPOCROOT}prepend/include/A1",
    "$ENV{EPOCROOT}epoc32/include/build",
    "$ENV{EPOCROOT}epoc32/include/groupA/build",
    "$ENV{EPOCROOT}epoc32/include/A1/build"
);
$buildincs = $variant2{'BUILD_INCLUDES'};
ok( File::Spec->canonpath("@buildincludes"), "@$buildincs" );

#11
print "\nTesting ROM_INCLUDES of a child variant:\n";
@romincludes = (
    "$ENV{EPOCROOT}epoc32/include/A1/rom",
    "$ENV{EPOCROOT}epoc32/include/rom",
    "$ENV{EPOCROOT}epoc32/include/groupA/rom"
);
$romincs = $variant2{'ROM_INCLUDES'};
ok( File::Spec->canonpath("@romincludes"), "@$romincs" );


#12
print "\nTesting Invalid Variant file (Missing HRH location). Expect an error from this test\n";
# Load third Variant File
my %variant3 = featurevariantparser->GetVariant( "b2", "$dir" );
ok( !$variant3{'VALID'} );

#13
my %info4 = featurevariantparser->GetVariant( "a2", "$dir" );
print "\nTesting the BUILD_INCLUDES of a child node with 'set' keyword:\n";
@buildincludes = (
    "$ENV{EPOCROOT}epoc32/include/A2/build",
    "$ENV{EPOCROOT}epoc32/include/A2/build2"
);
$buildincs = $info4{'BUILD_INCLUDES'};
ok( File::Spec->canonpath("@buildincludes"), "@$buildincs" );

#14 
print "\nTesting for function which searches for default variant file in a given directory\n";
ok(featurevariantparser->DefaultExists("$dir"));


#15 
print "\nTesting for function which searches for default variant file in the default directory\n";
if ( -e "$ENV{EPOCROOT}epoc32/tools/variant/default.var" ) {
	ok(featurevariantparser->DefaultExists());
}
else {
	ok(!featurevariantparser->DefaultExists());
}


#16
print "\nTesting GetBuildableFeatureVariants(). Ignore the generated errors\n";
my @list =featurevariantparser->GetBuildableFeatureVariants($dir);
my $validlist = "b1a";
ok($list[0],$validlist);


#17
print "\nTesting ResolveFeatureVariant().Ignore the generated errors\n";
my @list2 = featurevariantparser->ResolveFeatureVariant("default",$dir);
my @validlist2 = ("b1a");
ok(@list2,@validlist2);

#18
print "\nTesting GetValidVariant().Ignore the generated errors\n";
my @list3 = featurevariantparser->GetValidVariants($dir);
my @validlist3 = ("a1","a2","b1a","default","groupa");
ok("@list3","@validlist3");
