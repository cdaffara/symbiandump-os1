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
# FC_LOGGER.PM
# 
#

package FCLoggerUTL;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	PMCheckFCLoggerVersion
);

#Function Call Logger
sub PMCheckFCLoggerVersion() {
	my $exe=$ENV{EPOCROOT}."epoc32\\tools\\fc_logger\\edgcpfe.exe";
	#Check if file exists
	if (not -e $exe) {
		print "WARNING: File doesn't exist $exe \n";
		return 0;
	}

	open FCLPIPE, "$exe -v 2>&1 |";
	# Read all output from file into array
	my @lines=<FCLPIPE>;
	chomp @lines;
	# combine into single string with each line starting with a :
	my $line=join(":", "", @lines);
	close FCLPIPE;

	if ($line =~ m/version\s([0-9\.]+)/) {
		my $Version      = $1;
		$Version		 =~ m/(\d+)\.(\d+)\.?([\d+])?/;
		my $MajorVersion = $1;
		my $MinorVersion = $2;
		my $PatchLevel   = $3;
		if ($MajorVersion<3 or $MinorVersion<7) {
			print "WARNING: $exe version is less than 3.7.1. The -logfc option will be ignored.\n";
			return 0;
		}
	}
	return 1;
}
