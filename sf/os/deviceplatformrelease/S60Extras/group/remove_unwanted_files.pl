#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#!/usr/local/bin/perl

if ($#ARGV != 1)
	{
	die "FileRemover\nThis must be called with two parameters:\n	Arg1 : Database of files with their current status\n	Arg2 : Destination batch file to remove the undesired files.\n";
	}
$database=shift(@ARGV);
$resultingList=shift(@ARGV);

open(FH,"< $database")|| die "Cannot open file $database for reading\n";
open(WFH,"> $resultingList")|| die "Cannot open file $resultingList for writing\n";
while ($line=<FH>)
	{
	$line=uc $line; 	# read all lines in uppercase
	chop($line);		# remove EOL char
	if (($filename,$status) = $line =~ /([^;\s]+)\s*;\s*([K|R])/) 
			# if match found 
		{
		if ($status eq "R")
			{
			if (-e $filename)
				{
				print WFH "call attrib -r $filename\n";
				print WFH "call del $filename\n";		
				};
			if ($filename =~ /([^\\]+)$/)
				{
				if (-e "\\epoc32\\include\\$1")
					{
					print WFH "call attrib -r \\epoc32\\include\\$1\n";
					print WFH "call del \\epoc32\\include\\$1\n";
					}
				}
			}
		}
	};
close(FH);
close(WFH);


