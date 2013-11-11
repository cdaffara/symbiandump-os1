# Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Build bitmap test files
# 
#

use Cwd;
use File::Path;

$dir = getcwd();
chdir "..";
$parentdir = getcwd();

$datadir="/tfbs/mbmfiles";
$datadir = $parentdir . $datadir;

if (-e "$datadir")
{ 
  #proceed with your code
}
else
{
	eval { mkpath($datadir) };
	if ($@) {
	    print "Couldn't create $dir: $@";
  	} 
}


binaryCopy("$datadir/tfbs_file4.mbm","$datadir/tfbs_file4a.mbm");
binaryCopy("$datadir/tfbs_file4.mbm","$datadir/tfbs_file4b.mbm");
binaryCopy("$datadir/tfbs_file4.mbm","$datadir/tfbs_file4c.mbm");
binaryCopy("$datadir/tfbs_file4.mbm","$datadir/tfbs_file4d.mbm");
binaryCopy("$datadir/tfbs_file4.mbm","$datadir/tfbs_file4e.mbm");
binaryCopy("$datadir/tfbs_file4.mbm","$datadir/tfbs_file4f.mbm");
binaryCopy("$datadir/tfbs_file4.mbm","$datadir/tfbs_file4g.mbm");
binaryCopy("$datadir/tfbs_file4.mbm","$datadir/tfbs_file4h.mbm");

binaryCopy("$datadir/RscHeader3.Bin","$datadir/DummyRscFile3.RSC","$datadir/TFBS.MBM","$datadir/tfbs_rsc.rsc");

sub binaryCopy
{
	$numOfParams = @_;
	my $buffer;
	
	open OUTF, ">$_[$numOfParams-1]" or die "\nCan't open $_[$numOfParams-1] for writing: $!\n";
	binmode OUTF;

	for($i=0;$i<$numOfParams-1;$i++)
	{
		open INF, $_[$i] or die "\nCan't open $_[$i] for reading: $!\n";	
		binmode INF;
		while	(
			read (INF, $buffer, 65536)	# read in (up to) 64k chunks, write
			and print OUTF $buffer	# exit if read or write fails
			) {};
			die "Problem copying: $!\n" if $!;
			close INF or die "Can't close $_[$i]: $!\n";
	}
	close OUTF or die "Can't close $_[$numOfParams-1]: $!\n";
}