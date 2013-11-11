# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Generate test RSC files using new RSC file format: RSC file header + RSC file section + Bitmap file section
# 
#

use Cwd;
use File::Path;

chdir "..";
$parentdir = getcwd();

$datadir="/tbit/mbmfiles";
$datadir = $parentdir . $datadir;

if (-e "$datadir")
{ 
  #proceed with your code
}
else
{
	eval { mkpath($datadir) };
	if ($@) {
	    print "Couldn't create $datadir: $@";
  	} 
}
system("bmconv /s $datadir/8romc.mbm /c8$datadir/RLETest.BMP");
system("bmconv $datadir/8ramc.mbm /c8$datadir/RLETest.BMP");
binaryCopy("$datadir/RscHeader2.Bin","$datadir/DummyRscFile.RSC","$datadir/8romc.mbm","$datadir/romrsc_rommbm.rsc");
binaryCopy("$datadir/RscHeader2.Bin","$datadir/DummyRscFile.RSC","$datadir/8ramc.mbm","$datadir/ramrsc_rammbm.rsc");

system("bmconv $datadir/8ramc2.mbm /c8$datadir/TBMP.BMP /c8$datadir/TGDIGEN.BMP");
binaryCopy("$datadir/RscHeader2.Bin","$datadir/DummyRscFile.RSC","$datadir/8ramc2.mbm","$datadir/ramrsc_rammbm2.rsc");
system("bmconv /s $datadir/8romc2.mbm /c8$datadir/TBMP.BMP /c8$datadir/TGDIGEN.BMP");
binaryCopy("$datadir/RscHeader2.Bin","$datadir/DummyRscFile.RSC","$datadir/8romc2.mbm","$datadir/romrsc_rommbm2.rsc");
binaryCopy("$datadir/RscHeader2.Bin","$datadir/DummyRscFile.RSC","$datadir/8ramc.mbm","$datadir/romrsc_rammbm.rsc");
binaryCopy("$datadir/RscHeader2.Bin","$datadir/DummyRscFile.RSC","$datadir/8romc.mbm","$datadir/ramrsc_rommbm.rsc");
binaryCopy("$datadir/RscHeader2.Bin","$datadir/DummyRscFile.RSC","$datadir/8romc2.mbm","$datadir/ramrsc_rommbm2.rsc");
binaryCopy("$datadir/RscHeader2.Bin","$datadir/DummyRscFile.RSC","$datadir/8ramc2.mbm","$datadir/romrsc_rammbm2.rsc");


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