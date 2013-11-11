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
# Build bitmap test files
# 
#

use Cwd;
use File::Path;
use File::Copy;

$dir = getcwd();
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
	    print "Couldn't create $dir: $@";
  	} 
}

system("bmconv /s $datadir/16romc.mbm /c16$datadir/RLETest.BMP");
system("bmconv /r $datadir/16rom.mbm /c16$datadir/RLETest.BMP");
system("bmconv $datadir/16ramc.mbm /c16$datadir/RLETest.BMP");
system("bmconv /n $datadir/16ram.mbm /c16$datadir/RLETest.BMP");
system("bmconv $datadir/8ramc.mbm /c8$datadir/RLETest.BMP");
system("bmconv /s $datadir/8romc.mbm /c8$datadir/RLETest.BMP");
system("bmconv $datadir/12ramc.mbm /c12$datadir/RLETest.BMP");
system("bmconv /s $datadir/12romc.mbm /c12$datadir/RLETest.BMP");
system("bmconv $datadir/bmctest.mbm /c24$datadir/agbw24.bmp /c24$datadir/Aif24.bmp /c24$datadir/calc24.bmp /c24$datadir/Data24.bmp /c24$datadir/DILBERT1.BMP /c24$datadir/Icn24.bmp /c24$datadir/Inst24.bmp /c24$datadir/Jotter24.bmp");
system("bmconv /s $datadir/24romc.mbm /c24$datadir/RLETest.BMP");
system("bmconv /r $datadir/24rom.mbm /c24$datadir/RLETest.BMP");
system("bmconv $datadir/24ramc.mbm /c24$datadir/RLETest.BMP");
system("bmconv /n $datadir/24ram.mbm /c24$datadir/RLETest.BMP");
system("bmconv /n $datadir/32ram.mbm /c32$datadir/RLETest.BMP");
system("bmconv /n $datadir/drawbitmap16mu.mbm /c32$datadir/DrawBitmap.bmp");
system("bmconv $datadir/drawbitmapcomp16mu.mbm /c32$datadir/DrawBitmap.bmp");

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