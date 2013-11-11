# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# E32ENV.PM
# Contains information for makmake and associated e32tools perl programs
# within the Epoc32 Environment
# 
#

package E32env;

use strict;

use vars qw(%Data);

BEGIN {
	my $epocroot = $ENV{EPOCROOT};
	die "ERROR: Must set the EPOCROOT environment variable\n" if (!defined($epocroot));
	$epocroot =~ s-/-\\-go;	# for those working with UNIX shells
	$epocroot =~ s/^ +//;
	$epocroot =~ s/ +$//;
	$ENV{EPOCROOT} = $epocroot;
	die "ERROR: EPOCROOT must not include a drive letter\n" if ($epocroot =~ /^.:/);
	die "ERROR: EPOCROOT must be an absolute path without a drive letter\n" if ($epocroot !~ /^\\/);
	die "ERROR: EPOCROOT must not be a UNC path\n" if ($epocroot =~ /^\\\\/);
	die "ERROR: EPOCROOT must end with a backslash\n" if ($epocroot !~ /\\$/);
 	open PIPE, "set EPOCROOT |";
 	my $found=0;
	while (<PIPE>) {
		if (/^EPOCROOT=.*/) {
			$found=1;
			last;
		}
	}
	close PIPE;
	die "EPOCROOT environment variable must be capitalised\n" if (!$found);
	print "WARNING: EPOCROOT does not specify an existing directory\n" if (!-d $epocroot);

	$epocroot=~ s-\\$--;		# chop trailing \\

	$Data{EPOCPath} = $epocroot."\\epoc32\\";

	$Data{EPOCDataPath} = $epocroot."\\epoc32\\data\\";
	$Data{EPOCIncPath} = $epocroot."\\epoc32\\include\\";
	$Data{BldPath} = $epocroot."\\EPOC32\\BUILD\\";
	$Data{LinkPath} = $epocroot."\\epoc32\\release\\";
	$Data{RelPath} = $epocroot."\\epoc32\\release\\";
	$Data{EPOCToolsPath} = $epocroot."\\epoc32\\tools\\";
	$Data{RomPath} = $epocroot."\\epoc32\\rom\\";

	$Data{DataPath} = "z\\system\\data\\";
	
	$Data{SecurePlatform} = 1;
}

1;
