#!/usr/bin/perl
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
#

use strict;
use File::Basename;

if (@ARGV <2)
	{
	print (STDERR "\ncreaterfifile.pl\n");
	print STDERR << 'END_OF_HELP';

Usage: createrfifile.pl rss_cpp_deps_file.d output_file.rfi [exclude path]

Takes a file containing CPP dependency output from the preprocessing of a .rss file and
generates a "combined resource" .rfi that can  be consumed by CDB.
Optionally takes an exclusion path under which "found" dependencies are then ignored.

END_OF_HELP
	exit(0);
	}

my ($deps, $rfi, $exclude) = @ARGV;

if ($exclude)
	{
	$exclude =~ s/\\/\//g;			# Ensure consistent slashes
	$exclude =~ s/\/\//\//g;		# Remove double slashes
	$exclude = quotemeta($exclude);	# Convert for regex match
	}

print ("RFI : exclude under - \"$exclude\"\n");

my @resources;
open DEPS, "< $deps" or die "\nCannot read \"$deps\"!\n\n";
while (<DEPS>)
	{
	# .d file format - whitespace at front is key, path format varies depending on platform
	# the aim is to get a list of the "real" files. Missing files can appear "unpathed", although
	# this isn't currently dealt with.
	#
	#HelloWorld_reg.o:  \
	# F:/source/personal/misc/filenamepolicy/ported/examples/helloworld/HelloWorld_reg.rss \
	#  F:/builds/sbs/9.5/epoc32/include/variant/Symbian_OS.hrh \
	#  F:/builds/sbs/9.5/epoc32/include/appinfo.rh \
	#  F:/builds/sbs/9.5/epoc32/include/helloworld.rsg
	#
	
	s/^.*\.o\://;
	s/^\s+//;
	s/\s*\\$//;
	s/\/\//\//g;
	chomp ($_);
	next if !/\S/;

	print ("WARNING: Could not find dependency \"$_\" in \"$deps\"\n") if !(-e $_);
	
	print ("RFI : processing - \"$_\"\n");
	next if ($exclude && /^$exclude/i);
	push @resources,$_;	
	}
close DEPS;

open RFI, "> $rfi" or die "\nCannot write \"$deps\"!\n\n";
foreach my $resource (@resources)
	{
	print RFI "\n\n/* GXP ***********************\n";
	print RFI " * ".basename($resource)."\n";
	print RFI " ****************************/\n\n";
	
	open RESOURCE, "< $resource" or die "\nCannot read \"$resource\"!\n\n";
	print RFI $_ while (<RESOURCE>);
	close RESOURCE;
	}
close RFI;

