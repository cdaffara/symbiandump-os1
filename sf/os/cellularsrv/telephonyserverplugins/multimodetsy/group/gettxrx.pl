# Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Perl script to strip out the Tx: and Rx: information from a GSMTSY.TXT log file
# 
#

my $line;
my $txrx = 0;
my $lastx;
while ($line=<>)
	{
	if ($line =~ /^\d\d\.\d\d:\d\d:\d\d\d\d\d\d/)
		{
		$line = substr $line,16;
		$txrx = 0;
		if ($line =~ /^(.)x:\t/)
			{
			if ($lastx eq $1)
				{
				$line = substr $line,4;
				}
			print $line;
			$txrx = 1;
			$lastx = $1;
			}
		next;
		}
	if ($txrx)
		{
		print $line;
		}
	}