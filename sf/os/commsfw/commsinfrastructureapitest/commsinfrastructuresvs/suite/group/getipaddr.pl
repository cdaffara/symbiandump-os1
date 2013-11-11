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
	getipaddr.pl -connection=Connection

	Connection
		Local Area Connection | RAS
USAGE_EOF
	}

###Validate parameters
sub GetParameters()
	{
	my $connection="";

	GetOptions(
		'connection=s'	=> \$connection,
		'help'			=> \$help);

	if ( $help )
		{
	print	"Help\n";
		Usage();
		exit(0);
		}

    return ($connection);
	}

sub GetIpAddr($)
{
	my ($connection) = @_;
	$connection .= ":";

	my	$sCfgFile = "./ipconfig.txt";
	my	$sSrvAddr = "";
	my	$cfgFound = 0;
	system("ipconfig > $sCfgFile");
	open(IPCONFIG, $sCfgFile);

	foreach (<IPCONFIG>)
	{
		if (/.+adapter.+/)
		{
			if (/.+adapter\s+$connection/)
			{
				$cfgFound = 1;
			}
			else
			{
				$cfgFound = 0;
			}
		}
		elsif($cfgFound == 1)
		{
			if(/IP.+\s+(\d+\.\d+\.\d+\.\d+)/)
			{
				$sSrvAddr = $1;
			}
		}
	}
	close (IPCONFIG);

	return $sSrvAddr ;
}

sub Main()
{
	my	$connection="";
	($connection)=GetParameters();

	my	$ipaddr="";
	$ipaddr=GetIpAddr($connection);

	print "$ipaddr\r\n";
}

Main();
