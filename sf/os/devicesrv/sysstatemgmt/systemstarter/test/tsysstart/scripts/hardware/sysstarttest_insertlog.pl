# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Used within automated overnight testing system to insert results
# of sysstart tests on hardware in test summary log.
# 
#

use strict;

my $file=@ARGV[0];
my $fromString=@ARGV[1];
my $insertString1=@ARGV[2];
my $insertString2=@ARGV[3];

my $outfile;

if (not open (INFILE, "< $file"))
	{
	die "WARNING: insert.pl: Can't open $file";
	}

while (my $line = <INFILE>)
	{
	if ($line =~ /$fromString/)
		{
		$outfile .= $line;
		$outfile .= $insertString1;
		$outfile .= "\n";
		$outfile .= $insertString2;
		$outfile .= "\n";
		}
	else
		{
		$outfile .= $line;
		}
	}
close(INFILE);

open (OUTFILE, "> $file");
print OUTFILE $outfile;
close(OUTFILE);
