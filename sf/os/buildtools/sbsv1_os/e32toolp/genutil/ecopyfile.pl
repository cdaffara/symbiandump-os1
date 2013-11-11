# Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
use File::Copy;
use File::Basename;
use File::Path;


if (@ARGV!=2)
	{
#........1.........2.........3.........4.........5.........6.........7.....
	print <<USAGE_EOF;
Usage : perl ecopyfile.pl srcfile dstfile

If dstfile exists and is identical to srcfile then do nothing,
otherwise copy srcfile to dstfile.

USAGE_EOF
	exit 1;
	}

my $srcfile = shift;
my $dstfile = shift;

# Sanity checking
#

if (!-e $srcfile)
	{
	print STDERR "$srcfile does not exist\n";
	exit 1;
	}

if (!-f $srcfile)
	{
	print STDERR "$srcfile is not a plain file\n";
	exit 1;
	}

my $updating = -e $dstfile;

if ($updating && !-f $dstfile)
	{
	print STDERR "$dstfile exists, but is not a plain file\n";
	exit 1;
	}

# Can we avoid doing the copy?
#

if ($updating && !defined($ENV{ECOPYFILE_ALWAYS_COPY}) && (-s $srcfile == -s $dstfile))
	{
	# file exists and is the same size - check contents

	open SRC, $srcfile or print STDERR "Cannot open $srcfile\n" and exit 1;
	open DST, $dstfile or print STDERR "Cannot open $dstfile\n" and exit 1;

	binmode SRC;
	binmode DST;

	my $srcbuf;
	my $dstbuf;
	my $srclen;
	my $dstlen;
	my $same = 1;

	while ($same)
		{
		$srclen = read SRC, $srcbuf, 4096;
		$dstlen = read DST, $dstbuf, 4096;
		if ($srclen == 0 && $dstlen == 0)
			{
			last;
			}
		$same= $srcbuf eq $dstbuf;
		}

	close SRC;
	close DST;

	if ($same)
		{
		# Files match - no need to copy
		exit 0;
		}

	# files are different
	}

# Copy srcfile to dstfile
#

my $action = "create";

if ($updating) {
	$action = "update";
}
else {
	# see if the destination directory exists to create the file into...
	my $directory = dirname($dstfile);
	if (!-e $directory) {
		# ...and attempt to create it if not.
		if (!mkpath ($directory)) {
			print STDERR "Failed to create directory $directory\n";
			exit 1;
		}
	}
}


if (!copy ($srcfile, $dstfile))
        {
	print STDERR "Failed to $action file $dstfile\n";
	unlink $dstfile;
	exit 1;
	}


printf "%sd %s\n", ucfirst $action, $dstfile;
exit 0;


