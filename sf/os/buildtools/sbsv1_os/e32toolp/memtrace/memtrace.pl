# Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Postprocess EPOC memory usage trace file
# 
#

no strict 'vars';
use English;
# Standard Symbian boilerplate to find and load E32env library
use strict;
use FindBin;		# for FindBin::Bin
use Getopt::Long;

my $PerlLibPath;    # fully qualified pathname of the directory containing our Perl modules

BEGIN {
# check user has a version of perl that will cope
	require 5.005_03;
# establish the path to the Perl libraries: currently the same directory as this script
	$PerlLibPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlLibPath =~ s/\//\\/g;	# X:\epoc32\tools
	$PerlLibPath .= "\\";
}

use lib $PerlLibPath;
use Modload;
Load_SetModulePath($PerlLibPath);

my $tickperiod = 0;
my $emulproc = quotemeta("epoc[00000000]0001::");
my $apprun = quotemeta("apprun[10003a4b]");
my $ramdrive = quotemeta("TheRamDriveChunk");

my @chunknames;	# Array of chunk names, indexed by chunkid
my @traces;		# Array of chunk traces, each trace being an array
				# of (tick,chunkid,size) triples
my @chunkgroup;	# Group that a chunk belongs to, indexed by chunkid
my @groupnames;	# Array of chunk group names (normally a group==a process)
my @groups;		# Array of chunk groups, each group being an array of chunkids

my %opts;
my $result = GetOptions (\%opts, "detailed", "help");
if($result && ($opts{'help'} || $#ARGV<0))
	{
	&help;
	exit 0;
	}
if(!$result || $#ARGV>0)
	{
	&usage;
	exit 1;
	}

my $file = $ARGV[0];
open TRACE,$file or die "Error: Can't open trace file \"$file\".\n";

#
# Parse trace file
#
my %current;	# chunkids hashed by DChunk address
my $nextchunkid;
while(<TRACE>)
	{
	if (/MT:P\s+(\d+)\s*$/)
		{
		$tickperiod = $1;
		}
	elsif (/MT:C\s+(\d+)\s+([0-9a-fA-F]+)\s+(.+)\s*$/)
		{
		$current{$2} = $nextchunkid++;
		push @chunknames, $3;
		}
	elsif (my ($tick, $addr, $size, $name) =
		   (/MT:A\s+(\d+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+(.+)\s*$/))
		{
		my $chunkid = $current{$addr};
		die "Error: Parsing failure - is trace file complete ?"
			unless (defined $chunkid);
		push @traces, [0+$tick, $chunkid, hex($size)];

		# Check whether chunk has been renamed to something more useful
		$chunknames[$chunkid] = $name
			if (($chunknames[$chunkid] =~ /^$emulproc/io) ||
				($chunknames[$chunkid] =~ /^$apprun/io));
		}
	elsif (/MT:D\s+(\d+)\s+([0-9a-fA-F]+)\s+(.+)\s*$/)
		{
		die "Error: Parsing failure" unless (defined $current{$2});
		push @traces, [0+$1, $current{$2}, 0];
		delete $current{$2};
		}
	elsif (/(MT:.\s+.*)/)
		{
		printf "Warning: Unrecognised trace line \"$1\".\n";
		}
	}
close TRACE;
die "Error: File \"$file\" does not contain any memory traces.\n"
  unless ($#chunknames>0);


#
# Group chunks together by name
#
for (0..$#chunknames)
	{
	my $chunkid = $_;
	my $name = $chunknames[$chunkid];
	$name = $1 if ($name =~ /($ramdrive)$/i);	# Special handling for ramdrive
	$name = $1 if ($name =~ /^$emulproc(.*)/i);	# Use thread names on Emulator
	($name) = ($name =~ /([^:]+)/);				# otherwise strip thread name

	# yuck! linear search
	my $found = 0;
	for (0..$#groupnames)
		{
		my $groupid = $_;
		if ($groupnames[$groupid] eq $name)
			{
			$found = 1;
			push @{$groups[$groupid]}, $chunkid;
			$chunkgroup[$chunkid] = $groupid;
			last;
			}
		}
	
	if (!$found)
		{
		push @groupnames, $name;
		push @groups, [$chunkid];
		$chunkgroup[$chunkid] = $#groups;
		}
	}

# Strip instance number (if any) from group name for presentation
for (0..$#groupnames)
	{
	$groupnames[$_] = $1 if ($groupnames[$_] =~ /^([^]]+]?)/);
	}

#
# Output
#
my @chunksizes; # Array of chunk sizes, indexed by chunkid
for (0..$#chunknames) { $chunksizes[$_] = 0 };
	
if ($opts{'detailed'})
	{
	# Detailed output

	foreach my $name (@groupnames) { print ",\"$name\"" };
	print "\n";

	# if the tick size in microseconds hasn't been reported in the log with MT:P, take a guess
	my $tickdiv = 0;
	if ($tickperiod == 0)
		{
		# Uses hacky method to determine whether on Emulator or Target
		$tickdiv = ($chunknames[0] =~ /^$emulproc/io) ? 10 : 64;
		}
	else
		{
		# tickperiod is number of microseconds 
		$tickdiv = 1000000 / $tickperiod;
		}

	my ($oldtick, $minitick) = (0,0);
	my @groupsizes;
	for my $trace (@traces)
		{
		my ($tick, $chunkid, $size) = @$trace;
		if ($oldtick != $tick)
			{
			$oldtick = $tick;
			$minitick=0;
			}

		$chunksizes[$chunkid] = $size;
		my $groupid=$chunkgroup[$chunkid];
		my $groupsize = 0;
		foreach $chunkid (@{$groups[$groupid]})
			{
			$groupsize += $chunksizes[$chunkid];
			}
		$groupsizes[$groupid] = $groupsize;

		print $tick/$tickdiv + ($minitick++)/1000000;
		foreach $groupsize (@groupsizes)
			{
			if ($groupsize)
				{
				printf ",%d", $groupsize/1024;
				}
			else
				{
				print ',';		# make output prettier by omitting 0s
				}
			}
		print "\n";
		}
	}
else
	{
	# Summary output

	my @grouphighs;
	for my $trace (@traces)
		{
		my ($tick, $chunkid, $size) = @$trace;
		$chunksizes[$chunkid] = $size;
		my $groupid=$chunkgroup[$chunkid];
		my $groupsize = 0;
		foreach $chunkid (@{$groups[$groupid]})
			{
			$groupsize += $chunksizes[$chunkid];
			}
		$grouphighs[$groupid] = $groupsize
			if (!defined($grouphighs[$groupid]) ||
				($grouphighs[$groupid] < $groupsize));
	}

	printf "\"Process\", Size [K]\n";
	for (0..$#groupnames)
		{
		printf "\"$groupnames[$_]\", %d\n", $grouphighs[$_]/1024;
		}
	}
exit 0;


sub help ()
	{
	my $build;
	
	&Load_ModuleL('E32TPVER');
	print "\nmemtrace - " .
	  "Postprocess EPOC memory usage trace (Build ", &E32tpver, ")\n";
	&usage;
	}
	
sub usage ()
	{
	print <<EOF

Usage:
  memtrace [-d] <logfile>

Where:
  <logfile>     Memory usage trace file.

Options:
  -d            Produce a detailed listing.
EOF
	;
	}
