# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

# Directory of the reference images
my @files = `dir .\\ref\\*.mbm /B`;

# Convert all the reference images under its directory from .mbm to .bmp files
foreach $file (@files)
{
	chomp $file;
	my $bmpfile;
	$bmpfile = $file;
	$bmpfile =~ s/^(.*)mbm/$1bmp/;
	my $cmd = "bmconv /u .\\ref\\$file .\\ref\\$bmpfile";
	print "$cmd\n";
	print `$cmd`;
}

# Directory of the test images
my @files = `dir .\\test\\*.mbm /B`;

# Convert all the test images under its directory from .mbm to .bmp files
foreach $file (@files)
{
	chomp $file;
	my $bmpfile;
	$bmpfile = $file;
	$bmpfile =~ s/^(.*)mbm/$1bmp/;
	my $cmd = "bmconv /u .\\test\\$file .\\test\\$bmpfile";
	print "$cmd\n";
	print `$cmd`;
}

