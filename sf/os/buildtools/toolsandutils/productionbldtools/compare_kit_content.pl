#! perl

# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Compare the sdkpkg lists for corresponding packages in two kits.
# 
#

use strict;
use Getopt::Long;

my $toolVersion = "0.1";

sub usage($)
	{
	my ($errmsg) = @_;
	print STDERR "\ncompare_kit_content.pl - Version $toolVersion\n";

	print "$errmsg\n" if ($errmsg);
	
	print STDERR << 'END_OF_HELP';

Usage: compare_kit_content [-subset exp] [-temp dir] [-123] kit1 kit2

Compare the two kits, by matching the sdkpkg files in each kit and comparing the
list of contents for each package. Each kit can be specified either as a zip file
or an unpacked directory.

Options:
-subset exp        Compare only packages with names matching exp
-temp dir          Use dir as the location for temporary files
-123               Controls the output: 1 = left only, 2= right only, 3= both (default 12)

END_OF_HELP

	exit(1);
	}


my $subset = "";
my $temp = "";
my $verbose = 0;
my $help = 0;
my $leftonly = 0;
my $rightonly = 0;
my $both = 0;

# 1. Check arguments, output help etc.

GetOptions (
	'h|help' => \$help,				# print help message
	'v+' => \$verbose,				# print extra diagnostic info
	's|subset=s' => \$subset,		# pattern to match 
	't|temp=s' => \$temp,	        # temporary directory
	'1' => \$leftonly,	            # display files only in kit1
	'2' => \$rightonly,	        # display files only in kit2
	'3' => \$both,	                # display files present in both kits
	);

usage("") if ($help);
usage("must specify two kits") if (@ARGV != 2);

usage ("temp directory $temp does not exist") if ($temp ne "" && ! -d $temp);

if ($leftonly==0 && $rightonly==0 && $both==0)
	{
	$leftonly = 1;
	$rightonly = 1;
	}

sub leftfiles(@)
	{
	return if ($leftonly == 0 || scalar @_ == 0);
	print "left:\t",join("\nleft:\t",sort @_), "\n";
	}
sub rightfiles(@)
	{
	return if ($rightonly == 0 || scalar @_ == 0);
	print "right:\t",join("\nright:\t",sort @_), "\n";
	}
sub bothfiles(@)
	{
	return if ($both == 0 || scalar @_ == 0);
	print "both:\t",join("\nboth:\t",sort @_), "\n";
	}

my ($kit1, $kit2) = @ARGV;

# 2. Get the kit content list for both kits

sub kit_content($)
	{
	my ($kit) = @_;
	
	if (-f "$kit\\package-list.xml.zip")
		{
		opendir DIR, $kit or usage("cannot read $kit directory: $!");
		my @contents = grep /\.sdkpkg$/, readdir DIR;
		closedir DIR;
		
		return "dir", @contents;
		}
	
	usage("$kit is not a valid kit directory") if (!-f $kit);
	
	open UNZIP, "unzip -l $kit |" or usage("cannot read $kit as zip archive");
	my @lines = <UNZIP>;
	close UNZIP;
	
	#  36836436  07-12-08 02:59   com.symbian.api.GT-arm_0_0_M04604_Symbian_OS_v9.5.sdkpkg
	my @contents;
	foreach my $line (@lines)
		{
		if ($line =~ /\s(\S+\.sdkpkg)$/)
			{
			push @contents, $1;
			}
		}
	return "zip", @contents;
	}

my ($kit1_type,@kit1_contents) = kit_content($kit1);
my ($kit2_type,@kit2_contents) = kit_content($kit2);

sub compare_package_files($$)
	{
	my ($zip1,$zip2) = @_;

	open UNZIP, "unzip -l $zip1 |" or usage("cannot read $zip1 as zip archive");
	my @lines1 = <UNZIP>;
	close UNZIP;

	open UNZIP, "unzip -l $zip2 |" or usage("cannot read $zip2 as zip archive");
	my @lines2 = <UNZIP>;
	close UNZIP;

	#     699790  12-20-07 00:14   [emul]/epoc32/release/ARMV5/LIB/SipCodec.lib
	my %zip1_files;
	foreach my $line (@lines1)
		{
		next if ($line !~ /\s\d\d:\d\d\s+(\S.*)$/);
		$zip1_files{$1} = 1;
		}

	my @mismatches;
	my @bothlist;
	foreach my $line (@lines2)
		{
		next if ($line !~ /\s\d\d:\d\d\s+(\S.*)$/);
		my $file = $1;
		
		if (!defined $zip1_files{$file})
			{
			push @mismatches, $file;
			next;
			}
		push @bothlist, $file;
		delete $zip1_files{$file};	# matched - remove from the hash 
		}
	
	bothfiles(@bothlist);
	rightfiles(@mismatches);
	leftfiles(keys %zip1_files);
	}

sub compare_packages($$$$)
	{
	my ($type1,$package1,$type2,$package2) = @_;
	my @unzip = ("unzip", "-j");
	my $tempdir = ".\\";
	if ($temp ne "")
		{
		push @unzip, ("-d", "$temp");
		$tempdir = "$temp\\";
		}
	
	my @unlink_temporaries = ();
	if ($type1 eq "zip")
		{
		my $expected_file = $tempdir.$package1;
		system @unzip,$kit1,$package1;
		usage ("failed to read $package1 from $kit1") if (! -f $expected_file);
		push @unlink_temporaries, $expected_file;
		$package1 = $expected_file;
		}
	else
		{
		$package1 = $kit1."\\".$package1;
		}
	if ($type2 eq "zip")
		{
		my $expected_file = $tempdir.$package2;
		system @unzip,$kit2,$package2;
		usage ("failed to read $package2 from $kit2") if (! -f $expected_file);
		push @unlink_temporaries, $expected_file;
		$package2 = $expected_file;
		}
	else
		{
		$package2 = $kit2."\\".$package2;
		}
	compare_package_files($package1, $package2);
	
	unlink @unlink_temporaries if (scalar @unlink_temporaries != 0);
	}

# 3. sort the lists, eliminating the version information

print "left  ($kit1_type) = $kit1\n";
print "right ($kit2_type) = $kit2\n";

my %kit1_lookup;
foreach my $package (sort @kit1_contents)
	{
	my $rootname = $package;
	if ($package =~ /^([^_]*_)\d+_\d+_\S+\.sdkpkg$/)
		{
		$rootname = $1;
		next if ($subset ne "" && $rootname !~ /$subset/i);
		}
	$kit1_lookup{$rootname} = $package;
	}

my @missing_packages;

foreach my $package (sort @kit2_contents)
	{
	my $rootname = $package;
	if ($package =~ /^([^_]*_)\d+_\d+_\S+\.sdkpkg$/)
		{
		$rootname = $1;
		next if ($subset ne "" && $rootname !~ /$subset/i);
		}
	my $kit1_package = $kit1_lookup{$rootname};
	
	if (!defined $kit1_package)
		{
		push @missing_packages, "* $package is only in $kit2\n" if ($rightonly);
		next;
		}
	
	print "Comparing $rootname...\n";
	compare_packages($kit1_type, $kit1_package, $kit2_type, $package);
	delete $kit1_lookup{$rootname};
	}

foreach my $rootname (sort keys %kit1_lookup)
	{
	my $kit1_package = $kit1_lookup{$rootname};
	push @missing_packages, "* $kit1_package is only in $kit1\n" if ($leftonly);
	}
	
print sort @missing_packages;