# Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
use File::Path;


# THE MAIN PROGRAM SECTION
{
	unless (@ARGV) {
		&Usage();
	}

	my $originalPath = join (' ',@ARGV);
	print ("Creating $originalPath\n");

#	temp change for old perl
	foreach (@ARGV) {
		s-\\-\/-go;
	}
	mkpath([@ARGV]);
	foreach my $path (@ARGV) {
	    if (! -e $path) {
		print ("ERROR: Couldn't create $path\n");
	    }
	}
}

sub Usage () {
	print <<ENDHERESTRING;
Usage : perl emkdir.pl list_of_directories

  Creates the directories listed
ENDHERESTRING

	exit 1;
}
