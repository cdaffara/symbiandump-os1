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
#

use FindBin;		# for FindBin::Bin
use Getopt::Long;

# establish the path to the Perl binaries
BEGIN {
	require 5.005_03;				# check user has a version of perl that will cope
	$PerlBinPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlBinPath =~ s/\//\\/g;		# X:\epoc32\tools
}
use lib $PerlBinPath;

use DepUtil;
use Armutl;


my %opts = ();

my $result = GetOptions(\%opts,
						"inter",
						"path:s",
						"libpath:s",
						"out:s",
						"verbose"
						);
my $gPath = $opts{"path"};
my $glibPath = $opts{"libpath"};
my $gOutFile = $opts{"out"};
my $gVerbose = $opts{"verbose"};
my @libs;
my $lib;
my $aCnt = 0;
while($ARGV[$aCnt]){
	push @libs, $ARGV[$aCnt++];
}
my $oP = '--';
$oP = '-' if (Armutl_RVCTMajorVersion() == 2 && Armutl_RVCTMinorVersion() < 2);
my $interworkingp = $opts{"inter"};
my $interworking = "${oP}apcs /nointer";
$interworking = "${oP}apcs /inter" if ($interworkingp);

&GenDependencies($interworking, $gOutFile, $gPath, $glibPath, \@libs, $gVerbose);