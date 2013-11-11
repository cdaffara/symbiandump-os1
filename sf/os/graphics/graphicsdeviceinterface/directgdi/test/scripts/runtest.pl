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


sub convertMbm
{
	my $dir = shift or die( "no argument given." );
	# Directory of the reference images
	$dir = ".\\$dir";
	my @files = `dir $dir\\*.mbm /B`;
	# Convert all the reference images under its directory from .mbm to .bmp files
	foreach $file (@files)
	{
		chomp $file;
		$file = "$dir\\$file";
		my $bmpfile = $file;
		$bmpfile =~ s/mbm$/bmp/;

		# -M OPERAND Returns the age of OPERAND in days when the program started.
    # Dont output a message if skipping the file, to keep the output relevant.
    # The conversion utility outputs a message if conversion is performed.
		if ( -e $bmpfile && ( -M $bmpfile < -M $file ) ) {
			# print "skipping $file\n";
			next;
		}
		
		my $cmd = "bmconv /u $file $bmpfile";
		print "$cmd\n";
		print `$cmd`;
	}
}

convertMbm( "ref" );
convertMbm( "test" );

# Call python scripts to compare the difference between reference images and test images
my $cmd = "runtest.py $ARGV[0]";
print "$cmd\n";
print `$cmd`;

