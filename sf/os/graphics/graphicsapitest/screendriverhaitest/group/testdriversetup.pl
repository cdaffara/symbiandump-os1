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
use Cwd;


sub Usage()
	{
	print <<USAGE_EOF;
Usage
perl testdriversetup.pl --statlite=StatLite

    StatLite = [true | false ]

    --help                      : This help
USAGE_EOF
	exit( 0 )
	}

sub main()
	{
	my	$help='';
	my	$statLite="";

	GetOptions(
		'statlite=s'=> \$statLite,
		'help' 		=> \$help
	);

	if($help or (($statLite ne "true") and ($statLite ne "false")) )
		{
		Usage();
		exit(0);
		}

	my $theEpocRoot=$ENV{EPOCROOT};
	my $epoc32Location="$theEpocRoot.\\epoc32";

	my $currentDirectory=cwd;
	$currentDirectory =~ s/Group//i;
	$currentDirectory =~ s/\//\\/g;

	my $currentDrive = substr($currentDirectory,0,2);

	my	$cmd="TestDriver config";
	my	$suite="file:/$currentDrive$epoc32Location\\testdriver\\salt\\screendriverhai.driver";
	$suite =~ s.\\./.g;
	$suite =~ s\/./\/\g;

	$cmd .= " --bldclean OFF";
	$cmd .= " --bldmake OFF";
	$cmd .= " -e $currentDrive$theEpocRoot";
	$cmd .= " -x $currentDrive$epoc32Location\\testdriver\\salt";
	$cmd .= " --repos $currentDrive$epoc32Location\\testdriver\\Repository";
	$cmd .= " -c $currentDrive$epoc32Location\\testdriver\\Results";
	$cmd .= " -i $currentDirectory";
	$cmd .= " -s $suite";
	$cmd .= " --source $currentDirectory";
	$cmd .= " --tp $currentDirectory";
	$cmd .= " --testexec true";
	$cmd .= " --statlite $statLite";
	$cmd .= " --teflite true";
	$cmd .= " --commdb overwrite";
	system("$cmd");
	}

main();
