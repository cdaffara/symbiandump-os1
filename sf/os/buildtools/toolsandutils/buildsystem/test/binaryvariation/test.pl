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
# binary variation build test
#
# build a variant DLL and an invariant DLL and use them.

use strict;
use Test;
use Digest::MD5;

# how many tests do we plan to run?
BEGIN { plan tests => 204 }

# remember where we started
use Cwd;
my $cwd = getcwd();

# change to the test data directory
use File::Basename;
my $dir = dirname($0);
chdir($dir);
print "# running in $dir\n";

# create a local default.hrh, as it varies per OS version
# (make it just include the product include file)
my $epocroot = $ENV{'EPOCROOT'};
my $include = $epocroot . "epoc32/include/variant";
my $variant = $epocroot . "epoc32/tools/variant";

my $cfg = $variant . "/variant.cfg";
my $productInclude;

open(VARCFG, "$cfg") or die("cannot read $cfg\n");
while (<VARCFG>)
{
	$productInclude = "$1" if /epoc32.+?([^\\\/]+\.hrh)$/i;
}
close(VARCFG);
die("no product include file in $cfg\n") unless ($productInclude);

open(DEFHRH, ">default.hrh") or die("cannot write $dir/default.hrh\n");
print(DEFHRH "#include <$productInclude>\n");
print(DEFHRH "\n#define FEATURE_VARIANT_DEFAULT\n");
close(DEFHRH);
print("# testing against product $productInclude\n");

# copy test files to EPOCROOT (make sure we do not destroy any valid
# files that are already there and remember to clean up afterwards)
#
# /epoc32/include/variant/default.hrh
# /epoc32/include/variant/variant_a.hrh
# /epoc32/include/variant/variant_b.hrh
# /epoc32/include/variant/a/variant_a_extra.hrh
# /epoc32/include/variant/b/variant_b_extra.hrh
# /epoc32/tools/variant/default.var
# /epoc32/tools/variant/variant_a.var
# /epoc32/tools/variant/variant_b.var
# /epoc32/tools/variant/variant_c.var
# /epoc32/include/variant/featurelists/bvtest.txt

my @created = ("$include/default.hrh",
			   "$include/variant_a.hrh",
			   "$include/variant_b.hrh",
			   "$include/a/variant_a_extra.hrh",
			   "$include/b/variant_b_extra.hrh",
			   "$variant/default.var",
			   "$variant/variant_a.var",
			   "$variant/variant_b.var",
			   "$variant/variant_c.var",
			   "$include/featurelists/bvtest.txt"
			  );

use File::Copy;
my $file;
foreach $file (@created)
{
	my $epocDir = dirname($file);
	my $localFile = basename($file);

	mkdir($epocDir) if (!-d $epocDir);
	move($file, "$file.bak") if (-f $file);

	print "# copy $localFile $file\n";
	unlink $file;
	copy($localFile, $file) or die "Failed copy: $!";
}

###############################################################################
# THE TESTS                                                                   #
###############################################################################

# we need to test the ABIv1 and ABIv2 builds

testABI("armv5");
testABI("armv5_abiv1");

###############################################################################
# END OF TESTS                                                                #
###############################################################################

# delete test files and restore backed up files to their original state
foreach $file (@created)
{
	if (-f "$file.bak")
	{
		move("$file.bak", $file);
	}
	else
	{
		unlink($file);
	}
}

# change back to the starting directory
chdir($cwd);

# ALL DONE


###############################################################################
# SUBROUTINES                                                                 #
###############################################################################

sub testABI
{
	my $platform = shift;

	# remove the binaries if they already exist
	my $release = $epocroot . "epoc32/release/$platform";

	my @binaries = (
			    	"$release/udeb/InvariantStaticDLL.dll",
			    	"$release/urel/InvariantStaticDLL.dll",
					);

	foreach (@binaries)
	{
		unlink($_);
	}

	cleanVariants("$release/udeb", "VariantStaticDLL.dll");
	cleanVariants("$release/urel", "VariantStaticDLL.dll");
	cleanVariants("$release/udeb", "UseStaticDLL.exe");
	cleanVariants("$release/urel", "UseStaticDLL.exe");

	# we cannot test the command return values because they are always 0
	system("bldmake bldfiles");
	
	# clean out everything so makefiles have to be recreated
	system("abld reallyclean $platform.variant_a >nul 2>&1");
	system("abld reallyclean $platform.variant_b >nul 2>&1");
	system("abld reallyclean $platform.variant_c >nul 2>&1");
	system("abld reallyclean $platform >nul 2>&1");
	
	# Build variants first to ensure the default makefile is created for invariant dlls
	system("abld build $platform.variant_a");
	system("abld build $platform.variant_b");
	system("abld build $platform.variant_c");
	system("abld build $platform");

	# test for the existence of each invariant binary file
	foreach (@binaries)
	{
		print "# checking $_\n";
		ok(-f $_);
	}

	# test for the existence of each variant binary file
	checkVariants("$release/udeb/VariantStaticDLL.dll");
	checkVariants("$release/urel/VariantStaticDLL.dll");
	checkVariants("$release/udeb/UseStaticDLL.exe");
	checkVariants("$release/urel/UseStaticDLL.exe");
}

sub cleanVariants
{
	my $dir = shift;
	my $name = shift;

	if (opendir(DIR, $dir))
	{
		while (my $file = readdir(DIR))
		{
			if ($file =~ /^$name/)
			{
				print "removing $dir/$file\n";
				unlink("$dir/$file");
			}
		}
		closedir(DIR);
	}
	else
	{
		print "cannot clean $dir/$name*\n";
	}
}

sub checkVariants
{
	my $root = shift;
	
	$root =~ s/\.([^\.]+)$//;
	my $ext = $1;
	
	my $vmap = "$root.$ext.vmap";

	# there should be a VMAP file
	print "# checking $vmap\n";
	ok(-f $vmap);

	my $hashDefault = "0";
	my $hashVariantA = "0";
	my $hashVariantB = "0";
	my $hashVariantC = "0";

	# Variables to hold feature macro values
	my ( $featDefault, $featVariantA, $featVariantB, $featVariantC );
	
	if (open(VMAP, $vmap))
	{
		while (<VMAP>)
		{
			if (/([0-9a-f]{32})\s+(\S+)\s(\S+)/i)
			{
				my $var = lc($2);
				
				# Store the data for later
				( $hashDefault, $featDefault ) = ( $1, $3) if ($var eq 'default');
				( $hashVariantA, $featVariantA ) = ( $1, $3) if ($var eq 'variant_a');
				( $hashVariantB, $featVariantB ) = ( $1, $3) if ($var eq 'variant_b');
				( $hashVariantC, $featVariantC ) = ( $1, $3) if ($var eq 'variant_c');
			}
		}
		close(VMAP);
	}

	# the three hashes Default,A,B should be valid and different
	ok($hashDefault);
	ok($hashVariantA);
	ok($hashVariantB);

	ok($hashDefault ne $hashVariantA);
	ok($hashDefault ne $hashVariantB);
	ok($hashVariantA ne $hashVariantB);

	# the three feature lists for Default,A,B should be valid and different
	ok($featDefault);
	ok($featVariantA);
	ok($featVariantB);

	ok($featDefault ne $featVariantA);
	ok($featDefault ne $featVariantB);
	ok($featVariantA ne $featVariantB);

	# Check the feature lists are correct
	ok($featDefault eq 'FEATURE_VARIANT_A=undefined,FEATURE_VARIANT_B=undefined');
	ok($featVariantA eq 'FEATURE_VARIANT_A=defined,FEATURE_VARIANT_B=undefined');
	ok($featVariantB eq 'FEATURE_VARIANT_A=undefined,FEATURE_VARIANT_B=\'123\'');
	
	# Check the hash and feature lists match
	ok($hashDefault eq Digest::MD5::md5_hex($featDefault));
	ok($hashVariantA eq Digest::MD5::md5_hex($featVariantA));
	ok($hashVariantB eq Digest::MD5::md5_hex($featVariantB));
	
	# hashes A and C should be the same
	ok($hashVariantA, $hashVariantC);

	# feature lists for A and C should be the same
	ok($featVariantA, $featVariantC);
	
	# the corresponding binaries should exist
	print("# checking $root.$hashDefault.$ext\n");
	ok(-f "$root.$hashDefault.$ext");

	print("# checking $root.$hashVariantA.$ext\n");
	ok(-f "$root.$hashVariantA.$ext");

	print("# checking $root.$hashVariantB.$ext\n");
	ok(-f "$root.$hashVariantB.$ext");

	print("# checking $root.$hashVariantC.$ext\n");
	ok(-f "$root.$hashVariantC.$ext");
}

