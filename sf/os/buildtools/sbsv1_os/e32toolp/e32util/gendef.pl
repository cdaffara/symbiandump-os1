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
# e32toolp/e32util/gendef.pl
# 
#

use FindBin;		# for FindBin::Bin

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
use E32tpver;

# THE MAIN PROGRAM SECTION
##########################

unless (@ARGV>1)
{
	&Usage;
}
my @exports = @ARGV;
my $outfile = shift @exports;

open OUTFILE, ">$outfile" or die "Gendef.pl: Unable to open $outfile for writing";

print OUTFILE "EXPORTS\n";
my $exportNum = 1;
foreach (@exports)
{
	print OUTFILE "\t$_ @ $exportNum NONAME\n";
	++$exportNum;
}

close OUTFILE;

#######################################################################
# SUBROUTINES
#######################################################################

sub Usage () {

	print(
		"\n",
		"GENDEF - Generate a DEF file from passed exports (Build ",&E32tpver,")\n",
		"\n",
		"GENDEF.PL [output def file] [1st export name] ([Other export names]...)\n",
		"\n",
		"\n",
		"\n"
	);
	exit 1;
}


