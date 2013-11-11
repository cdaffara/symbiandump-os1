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

use Getopt::Long;

sub Usage()
	{
	print <<USAGE_EOF;
Usage
perl uartscript.pl --in=FileNameIn --out=FileNameOut --port=Port --baud=Baud --help

    --in=FileNameIn             : Input file name

    --out=FileNameOut           : Output file name

    --port=Port                 : Serial port number

    --baud=Baud                 : Baud rate of the port

    --help                      : This help
USAGE_EOF
	exit( 0 )
	}

sub main()
{
	my	$help='';
	my	$input="";
	my	$output="";
	my	$port="";
	my	$baud="";

	GetOptions(
		'in=s'		=> \$input,
		'out=s'		=> \$output,
		'port=s'	=> \$port,
		'baud=s'	=> \$baud,
		'help' 		=> \$help
	);

	if($help or ($input eq "") or ($output eq "") or ($port eq "") or ($baud eq ""))
		{
		Usage();
		exit(0);
		}

	open(FIN, $input);
	open(FOUT, ">$output");

	foreach (<FIN>)
	{
		if (/<port>/)
		{
			s/<port>/$port/;
		}
		if (/<baud>/)
		{
			s/<baud>/$baud/;
		}
		print FOUT $_;
	}
	close(FIN);
	close(FOUT);
}

main();
