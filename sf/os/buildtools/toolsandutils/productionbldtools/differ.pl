#!\usr\bin\perl
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
# Treates the output of diff -q to list common parts of filenames of differing
# files
# 
#

while (my $line=<STDIN>)
	{
	chomp($line);
	
	if ($line =~ /^Files .* differ$/)
		{
		if ($line =~ /^Files ([^\s]+) and ([^\s]+) differ$/)
			{
			# Files present but differing
			my @one = reverse(split(/[\/\\]/,$1));
			my @two = reverse(split(/[\/\\]/,$2));
			my @common; # Shared RHS of path/filename

			my $index = 0;

			while
			  (  ($index < scalar(@one))
			  && ($index < scalar(@two))
			  )
				{
				# If path section is identical, copy to @common
				if ($one[$index] eq $two[$index])
					{ unshift @common, $one[$index]; }
				else
					{ last; }
				$index++;
				}

			print join("\\",@common)."\n";
			}
		else
			{
			die "Could not parse \"$line\"\n";
			}
		}
	}
