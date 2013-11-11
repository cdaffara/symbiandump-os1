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
perl InsertIpAddr.pl --in=FileNameIn --out=FileNameOut [--connection=Connection] --tag=Tag --help

    --in=FileNameIn             : Input file name

    --out=FileNameOut           : Output file name

    --connection=Connection     : Name of connection to get IP address of
                                  defaults to "Local Area Connection"

    --tag=Tag                   : Tag in input file to replace with the IP address

    --help                      : This help
USAGE_EOF
	exit( 0 )
	}

sub trim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}

sub main($$$$)
{
	my	($fileNameIn, $fileNameOut, $connectionName, $tagName) = @_;

	my	$help='';
	my	$input="";
	my	$output="";
	my	$connection="Local Area Connection";
	my	$tag="";

	GetOptions(
		'in=s'			=> \$input,
		'out=s'			=> \$output,
		'connection=s'	=> \$connection,
		'tag=s'			=> \$tag,
		'help' 			=> \$help
	);

	if($help or ($input eq "") or ($output eq "") or ($connection eq "") or ($tag eq ""))
		{
		Usage();
		exit(0);
		}

	my	$ipAddr=trim(`perl -S getipaddr.pl -connection=\"$connection\"`);
	print $ipAddr, "\n";
	if ($ipAddr == "")
	{
		print "ERROR: No IP Address\n";
		exit(-1);
	}

	my	@newLines = ();
	open(FIN, $input);
	open(FOUT, ">$output");

	foreach (<FIN>)
	{
		if (/$tag/)
		{
			s/$tag/$ipAddr/;
		}
		print FOUT $_;
	}
	close(FIN);
	close(FOUT);
}

main($ARGV[0], $ARGV[1], $ARGV[2], $ARGV[3]);
