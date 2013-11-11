#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

# unit tests for featurevariantmap.pm

use strict;
use Test;
use File::Basename;
use File::Spec;
use lib "../../e32util";	# run from e32toolp/test/featuervariantmap
use lib "../e32util";		# run from e32toolp/test
use featurevariantmap;
use File::Copy;
use Digest::MD5;

# how many tests do we plan to run?
BEGIN { plan tests => 56 }

# where is the test data
my $dir = File::Spec->rel2abs(dirname($0));
print "# running in $dir\n";

# Copy the features lists needed for this test to the right place
mkdir("$ENV{EPOCROOT}epoc32/include/variant/featurelists/");
unlink "$ENV{EPOCROOT}epoc32/include/variant/featurelists/featnames.txt";
copy("$dir/featnames.txt", "$ENV{EPOCROOT}epoc32/include/variant/featurelists/featnames.txt");

###############################################################################
# tests for Find                                                              #
###############################################################################

# we wont find a non-existent file
ok(featurevariantmap->Find("non-existant-file", "myvar"), "");

# we will find a real file (with no vmap)
ok(featurevariantmap->Find("$dir/fred.dll", "myvar"), "$dir/fred.dll");

# we wont find a non-existent feature variant
print "# expect an ERROR from this test\n";
ok(featurevariantmap->Find("$dir/john.dll", "not-a-variant"), "");

# we will find a real feature variant
ok(featurevariantmap->Find("$dir/john.dll", "myvar"),
   "$dir/john.12345678901234567890123456789012.dll");

# the feature variant name should not depend on case
ok(featurevariantmap->Find("$dir/john.dll", "MyVar"),
   "$dir/john.12345678901234567890123456789012.dll");

###############################################################################
# tests for Save                                                              #
###############################################################################

# create a .vmap from scratch
unlink("$dir/tony.dll.vmap"); 

ok(! featurevariantmap->Save("$dir/tony.dll", 
     "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "a_phone", "features_for_a"));

ok(check("$dir/tony.dll.vmap", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","a_phone","features_for_a"));

# add to an existing .vmap file
ok(! featurevariantmap->Save("$dir/tony.dll", 
     "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", "b_phone", "features_for_b1"));

ok(check("$dir/tony.dll.vmap", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","a_phone","features_for_a"));
ok(check("$dir/tony.dll.vmap", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb","b_phone","features_for_b1"));

# change an entry in an existing .vmap file
ok(! featurevariantmap->Save("$dir/tony.dll", 
     "babababababababababababababababa", "b_phone", "features_for_b2"));

ok(check("$dir/tony.dll.vmap", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","a_phone","features_for_a"));
ok(check("$dir/tony.dll.vmap", "babababababababababababababababa","b_phone","features_for_b2"));

# change with one case and check with another
ok(! featurevariantmap->Save("$dir/tony.dll", 
     "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbba", "B_PhOnE", "features_for_b3"));

ok(check("$dir/tony.dll.vmap", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","a_phone","features_for_a"));
ok(check("$dir/tony.dll.vmap", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbba","b_phone","features_for_b3"));

# add an entry and a hint to an existing .vmap file
my @hints = ("FEATUREVARIANT");
ok(! featurevariantmap->Save("$dir/tony.dll", 
     "cccccccccccccccccccccccccccccccc", "c_phone", "features_for_c", \@hints));

ok(check("$dir/tony.dll.vmap", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","a_phone","features_for_a"));
ok(check("$dir/tony.dll.vmap", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbba","b_phone","features_for_b3"));
ok(check("$dir/tony.dll.vmap", "cccccccccccccccccccccccccccccccc","c_phone","features_for_c"));
ok(check("$dir/tony.dll.vmap", "FEATUREVARIANT"));

###############################################################################
# tests for Hash                                                              #
###############################################################################

# test the hash key for an invalid feature variant
my @src = ("$dir/a.cpp", "$dir/b.cpp");
my %var = ('NAME' => 'something', 'VALID' => 0);

ok(featurevariantmap->Hash(\@src, \%var), "");
ok(featurevariantmap->HashAndFeatures(\@src, \%var), "");

# test the hash key for a valid feature variant with missing headers
$var{'VALID'} = 1;
ok(featurevariantmap->Hash(\@src, \%var), qr/^[0-9a-f]{32}$/);
ok(CompareHashAndFeatures(featurevariantmap->HashAndFeatures(\@src, \%var)), 'ADEF=undefined,BDEF=undefined');

# test the hash key for a working feature variant
$var{'BUILD_INCLUDES'} = ["$dir/inc_jack"];
my $h1 = featurevariantmap->Hash(\@src, \%var);
ok($h1, qr/^[0-9a-f]{32}$/);
ok(CompareHashAndFeatures(featurevariantmap->HashAndFeatures(\@src, \%var)), 'ADEF=undefined,BDEF=undefined,JACK=undefined');

# test the hash key for a different working feature variant
$var{'BUILD_INCLUDES'} = ["$dir/inc_noel", "$dir/inc_fake"];
my $h2 = featurevariantmap->Hash(\@src, \%var);
ok($h2, qr/^[0-9a-f]{32}$/);
ok($h1 ne $h2);
ok(CompareHashAndFeatures(featurevariantmap->HashAndFeatures(\@src, \%var)), 'ADEF=undefined,BDEF=undefined,NOEL=undefined');

# test the hash key for a working feature variant with a pre-include
$var{'PREINCLUDE'} = "$dir/jack.hrh";
$var{'BUILD_INCLUDES'} = ["$dir/inc_jack"];
my $h3 = featurevariantmap->Hash(\@src, \%var);
ok($h3, qr/^[0-9a-f]{32}$/);
ok($h3 ne $h2);
ok($h3 ne $h1);
ok(CompareHashAndFeatures(featurevariantmap->HashAndFeatures(\@src, \%var)), 'ADEF=undefined,BDEF=undefined,JACK=defined');

# test that we get the same hash value when we should (jack2.hrh ~ jack.hrh)
$var{'PREINCLUDE'} = "$dir/jack2.hrh";
my $h3a = featurevariantmap->Hash(\@src, \%var);
ok($h3a, $h3);
ok(CompareHashAndFeatures(featurevariantmap->HashAndFeatures(\@src, \%var)), 'ADEF=undefined,BDEF=undefined,JACK=defined');

# test the hash key for a different working feature variant with a pre-include
$var{'PREINCLUDE'} = "$dir/noel.hrh";
$var{'BUILD_INCLUDES'} = ["$dir/inc_noel"];
my $h4 = featurevariantmap->Hash(\@src, \%var);
ok($h4, qr/^[0-9a-f]{32}$/);
ok($h4 ne $h3);
ok($h4 ne $h2);
ok($h4 ne $h1);
ok(CompareHashAndFeatures(featurevariantmap->HashAndFeatures(\@src, \%var)), 'ADEF=undefined,BDEF=undefined,NOEL=defined');

# test the hash key for a different working feature variant with extra macros
$var{'PREINCLUDE'} = "$dir/noel.hrh";
$var{'BUILD_INCLUDES'} = ["$dir/inc_noel"];
$var{'MACROS'} = ["ADEF"];
my $h5 = featurevariantmap->Hash(\@src, \%var);
ok($h5, qr/^[0-9a-f]{32}$/);
ok($h5 ne $h4);
ok($h5 ne $h3);
ok($h5 ne $h2);
ok($h5 ne $h1);
ok(CompareHashAndFeatures(featurevariantmap->HashAndFeatures(\@src, \%var)), 'ADEF=\'1\',BDEF=undefined,NOEL=defined');

$var{'MACROS'} = ["ADEF", "BDEF"];
my $h6 = featurevariantmap->Hash(\@src, \%var);
ok($h6, qr/^[0-9a-f]{32}$/);
ok($h6 ne $h5);
ok($h6 ne $h4);
ok($h6 ne $h3);
ok($h6 ne $h2);
ok($h6 ne $h1);
ok(CompareHashAndFeatures(featurevariantmap->HashAndFeatures(\@src, \%var)), 'ADEF=\'1\',BDEF=\'1\',NOEL=defined');

# test verbose mode
$featurevariantmap::verbose = 1;
print "# this test is verbose\n";
my $h7 = featurevariantmap->Hash(\@src, \%var);
ok($h7, $h6);
print "# this test is verbose\n";
ok(CompareHashAndFeatures(featurevariantmap->HashAndFeatures(\@src, \%var)), 'ADEF=\'1\',BDEF=\'1\',NOEL=defined');

###############################################################################
# END OF TESTS                                                                #
###############################################################################

# utility functions

sub check
{
	my $file = shift;
	my $arg1 = shift;
	my $arg2 = shift;
	my $feat = shift;
	
	if ($arg2)
	{
		return checkHash($file, $arg1, $arg2, $feat);
	}
	return checkHint($file, $arg1);
}

sub checkHash
{
	my $file = shift;
	my $hash = shift;
	my $name = shift;
	my $feat = shift;
	
	open(FILE, $file) or return 0;
	my $hit = 0;	# found hash with the right name
	my $bad = 0;	# found hash with the wrong name
	while (<FILE>)
	{
		if (/^$hash\s+(\w+)\s+$feat$/)
		{
			if (uc($1) eq uc($name))
			{
				$hit = 1;
			}
			else
			{
				$bad = 1;
			}
		}
	}
	close(FILE);
	return ($hit && !$bad);
}

sub checkHint
{
	my $file = shift;
	my $hint = shift;

	open(FILE, $file) or return 0;
	while (<FILE>)
	{
		if (/^$hint\s*$/)
		{
			close(FILE);
			return 1;
		}
	}
	close(FILE);
	return 0;
}

# Check hash and feature list matches
sub CompareHashAndFeatures
{
	return if !$_[0] || !$_[1];
	return $_[1] if $_[0] eq Digest::MD5::md5_hex($_[1]);
}
