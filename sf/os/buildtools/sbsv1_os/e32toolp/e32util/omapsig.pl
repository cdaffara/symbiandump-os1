# e32toolp\e32util\omapsig.pl
#
# Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
# Prepend OMAP boot signature to miniboot binaries
#
# Syntax:
#	perl omapsig.pl <load address in hex> <input miniboot> <output minboot with sig>
#

use warnings;
use strict;
use IO::Handle;
use File::Copy;

if (scalar(@ARGV)!=3) {
	die "perl omapsig.pl <load address in hex> <input miniboot> <output minboot with sig>\n";
}

my ($load_address, $infile, $outfile) = @ARGV;

$load_address = pack('L', hex($load_address));

my $filesize_in_bytes = -s $infile;

print "miniboot input ", $filesize_in_bytes, " bytes\n";

$filesize_in_bytes = pack('L', $filesize_in_bytes);

open my $in, "< $infile" or die "Can't open $infile for input: $!";
binmode($in);
open my $out, "> $outfile" or die "Can't open $outfile for output: $!";
binmode($out);
$out->autoflush(1);

print $out $filesize_in_bytes;
print $out $load_address;

copy($in, $out) or die "Couldn't copy from $infile to $outfile: $!";

close $in;
close $out;

print "signed miniboot output ", -s $outfile, " bytes\n";
exit;