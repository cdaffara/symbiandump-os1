 #!/usr/bin/perl -w
#
# SetEnv.pl
# Script file to build Environment for PlatTest depending on test configuration

# Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

use Cwd;
use Getopt::Long;
use File::Copy;
use File::Basename;
use File::Stat;

my $theEpocRoot=$ENV{EPOCROOT};
my $epoc32Location = $theEpocRoot."\\epoc32";
#We need to replace the "\\" with "\" in the variable name otherwise we'll have
#problems further down when trying to use the variable. 
$epoc32Location =~ s/\\\\/\\/g;

 
sub InstallTestDriver()
	{
	###Install TestDriver
	#my ($secureData, $platform) = @_;
	unlink("$epoc32Location\\testdriver -U");
	system("$theEpocRoot.\\epoc32\\tools\\unzip -o $theEpocRoot.\\epoc32\\tools\\TestDriver\\testdriver.zip *.* -d $epoc32Location  -u");

	my $commsinfrasDirectory=cwd;
	$commsinfrasDirectory =~ s/commsfw\/Group$//i;
	$commsinfrasDirectory =~ s/\//\\/g;
	#my $commsfwDirectory=cwd;

	my $commsinfrasDrive = substr($commsinfrasDirectory,0,2);

	system("$epoc32Location\\testdriver\\bin\\TestDriver config -e $commsinfrasDrive$theEpocRoot >nul");
	system("$epoc32Location\\testdriver\\bin\\TestDriver config -x $commsinfrasDrive$epoc32Location\\testdriver\\Networking_Tests >nul");
	system("$epoc32Location\\testdriver\\bin\\TestDriver config -r $commsinfrasDrive$epoc32Location\\testdriver\\Repository >nul");
	system("$epoc32Location\\testdriver\\bin\\TestDriver config -l $commsinfrasDrive$epoc32Location\\testdriver\\Results >nul");
	system("$epoc32Location\\testdriver\\bin\\TestDriver config -s $commsinfrasDrive\\comms-infras\\commsfw >nul");
	system("$epoc32Location\\testdriver\\bin\\TestDriver config -c $commsinfrasDrive$epoc32Location\\testdriver\\certs\\testdriver.cert >nul");
	system("$epoc32Location\\testdriver\\bin\\TestDriver config -k $commsinfrasDrive$epoc32Location\\testdriver\\certs\\testdriver.key >nul");
        system("$epoc32Location\\testdriver\\bin\\TestDriver config -p OFF");
	}


sub Main()
	{
	my $currentDirectory=cwd;
	my $theCommand=$0;

	###	Extract perl command
	$theCommand =~ s/\\\S{1,8}\.\S{0,3}$//;

	chdir "$theCommand";

        InstallTestDriver();

	chdir "$currentDirectory";
        print ("START OF EXPORT.........\n");
        print ("bldmake bldfiles>\n");
        system("bldmake bldfiles");
        print ("abld export>\n");
	system("abld export");
        print ("abld test export>\n");
        system("abld test export");
        print (".........END OF EXPORT");
	}

Main();
