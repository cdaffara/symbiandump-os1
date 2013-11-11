#
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
#

use File::Copy;
use Getopt::Long;
use Cwd;
use Archive::Zip;
use Archive::Zip::Tree;

sub Usage()
	{
	print <<USAGE_EOF;
Usage
perl buildorbiter.pl --in=FileNameIn --out=FileNameOut --help

    --in=FileNameIn             : Input oby file name

    --out=FileNameOut           : Output zip file name

    --help                      : This help
USAGE_EOF
	exit( 0 )
	}

sub main()
	{
	my	$help='';
	my	$input="";
	my	$output="";

	GetOptions(
		'in=s'		=> \$input,
		'out=s'		=> \$output,
		'help' 		=> \$help
	);

	if($help or ($input eq "") or ($output eq "") )
		{
		Usage();
		exit(0);
		}

	my	$curdir = cwd;
	$_ = $curdir;
	s/\//\\/g;
	$curdir = $_;

	my	$theEpocRoot=$ENV{EPOCROOT};
	my	@epocdir = "$theEpocRoot.\\epoc32";

	open FH, "$input" || die "Couldn't open $input file for Reading: $!\n";

	while ( <FH> )
		{
		chomp;                  # no newline
		s/#.*//;                # no comments
		s/^\s+//;               # no leading white
		s/\s+$//;               # no trailing white
		next unless length;     # anything left?
		if ( /(^file|^data)\s*=\s*(\S+)\s+(\S+)/ )
			{
			my	$inputFile=$2;

			$inputFile =~ m/(\S*)(\\|\/)(\S+)$/;

			my	$dir=$curdir.$1;
			unless ( -d $dir )
				{
				system("mkdir $dir");
				}
			printf "Copying: $inputFile\n";
			copy($inputFile, $dir) or die "File cannot be copied.";
			}
		}
	close FH;

	my	$zip = Archive::Zip->new();
	if ( $input =~ m/(\S*)(\\|\/)(\S+)$/ )
		{
		$zip->addFile("$input", $3);
		}
	else
		{
		$zip->addFile("$input");
		}
	$zip->addTree("epoc32", "epoc32");
	die 'write error' if $zip->writeToFileNamed( "$output" ) != AZ_OK;
	printf "Completed\n";
	}

main();
