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


my @files = `dir *.mbm /B`;

# Convert every .mbm file in the current working directory to a .bmp file.
foreach $file (@files)
{
	chomp $file;
	my $bmpfile;
	$bmpfile = $file;
	$bmpfile =~ s/^(.*)mbm/$1bmp/;
	my $cmd = "bmconv /u $file $bmpfile";
	print "$cmd\n";
	print `$cmd`;
}