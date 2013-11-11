#!/usr/bin/perl
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Tool to convert xti ascii logs with freeway logging into btrace files
# suitable for input to utracedecoder
#

my $input = $ARGV[0];
if (length($input) == 0)
{
    print "Usage: xti2bt.pl <inputfile> <outputfile>";
    exit(-1);
}

my $output = $ARGV[1];
if (length($output) == 0)
{
    print "Outputting to btrace.bin\n";
    $output = "btrace.bin";
}

open INPUT, "< $input" or die "Couldn't open $input";
open BTRACE , "> $output" or die "Couldn't open $output";
binmode BTRACE;

sub writePrintf
{
    $trace = shift;
    $trace =~ s/¿/\t/g;
    $len = length($trace);
    if ($len > 104)
    {
	$trace = substr $trace, 0, 104;
	$len = length($trace);
    }
    $alignment = $len % 4;
    for ($i = 0; $i < (4-$alignment); $i++)
    {
	$trace = $trace."X";
    }
    $len = length($trace) + 8;
    
    $header = pack("CCCC", $len, 0, 0, 0);
    print BTRACE $header;
    print BTRACE "0000";
    print BTRACE $trace;
}

while (<INPUT>)
{
    if (/IConsole/)
    {
	next;
    } elsif (/xti1:SYMBIAN_TRACE; channel:0xE0; \[([0-9a-f,]*)/)
    {
	@bytes = split(/,/, $1);

	$size = hex($bytes[0]);
	$flags = hex($bytes[1]);
	$size2 = scalar(@bytes);

#	$bytes[0] = sprintf "%x", $size2;
# 	print "reported size $size\tarray size $size2  $trailing0\t";
# 	print "flags $flags\t";
# 	if ($flags & 1<<0) {
# 	    $flags2 = hex($bytes[4]);
# 	    $part = "?";
# 	    if ($flags2 == 1)
# 	    {
# 		$part = "S";
# 	    } elsif ($flags2 == 2)
# 	    {
# 		$part = "M";
# 	    } elsif ($flags2 == 3)
# 	    {
# 		$part = "E";
# 	    }
# 	    print "H2[$part], ";
# 	}

# 	if ($flags & 1<<1) {
# 	    print "T,";
# 	}

# 	if ($flags & 1<<2) {
# 	    print "T2, ";
# 	}

# 	if ($flags & 1<<3) {
# 	    print "Cid, ";
# 	}
	
# 	if ($flags & 1<<4) {
# 	    print "PC, ";
# 	}

# 	if ($flags & 1<<5) {
# 	    print "Ext, ";
# 	}

# 	if ($flags & 1<<6) {
# 	    print "Trunc, ";
# 	}

# 	if ($flags & 1<<7) {
# 	    print "Miss, ";
# 	}
# 	print "\t@bytes\n";

	$moddedsize = $size;
	if (($moddedsize % 4) != 0)
	{
	    $moddedsize += 4 - ($moddedsize % 4);
	}
	while (scalar(@bytes) < $moddedsize)
	{
	    push @bytes, "00";
	 #   print "Size all strange ( $size $size2 ):  @bytes\n";
	}
	
	foreach (@bytes) {
	    my $out = pack("C", hex $_);
	    print BTRACE $out;
	}
    } elsif (/xti1:MCU_ASCII_PRINTF; channel:0xE0; msg:(.*)/)
    {
	writePrintf($1);
    } elsif (/symb:(.*)/)
    {
	writePrintf($1);
    }
}
close BTRACE;
close INPUT;
