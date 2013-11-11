#!perl -w
# Copyright (c) 2010 Symbian Foundation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation - initial contribution.
#
# Contributors:
#
# Description:
# This modifies CommDB for the AT LTSY modem, asking the user for the comm port and baud rate to use.
# 
#

use strict;


# Ask for the COM port and Baud
my $com;
my $baud;
print "What COMM port is the modem on?\n(Note: Symbian COMM ports are 1 less than windows)\n\tCOMM::";
chomp ($com = <>);

print "What is the baud rate of the modem?\n(Wavecomm default is 9600, Telit default is 152000)\n\tBaud=";
chomp ($baud = <>);

#print "\nComm::$com Baud=$baud\n";

# Open the config files
my $defaultcomdb = "<./AT-LTSY(default).cfg";
my $commdb = ">/epoc32/winscw/c/AT-LTSY.cfg";

open(COMMDBOUT, $commdb);
open(COMMDBREAD, $defaultcomdb);

# Read in the default commdb and write it out to the new file, changing the com port and baud where necessary
my $modembearertable = 0;
my $modementry = 0;

while(<COMMDBREAD>)
{
my($line)=$_;

# first wait till we've found the $modembearertable line.
if ($line =~ m/^\[ModemBearer\]/) 
	{
	#print "Found the modem bearer table\n";
	$modembearertable = 1;
	}
# Or if we've got the end of the table unset the boolean
elsif ($modembearertable && $line =~ m/^\[/) 
	{
	#print "Found the end fo the modem bearer table\n";
	$modembearertable = 0;	
	}

# If we're in the modem table then check for a few things
if ($modembearertable)
	{
	# Now wait till we've got to an entry for our modem
	if ($line =~ m/^\sName=CommModem/) 
		{
		#print "Found the modem entry\n";
		$modementry = 1;
		}

	# Or if we've reached the end of the modem entry unset the boolean
	if ($modementry && $line =~ m/^END_ADD/) 
		{
		#print "Found the end of the modem entry\n";
		$modementry = 0;
		}
	}

# If we're in the entry for our modem then potentially modify the line
if ($modementry)
	{
	# If it's the comm port number set that up
	if ($line =~ m/^\sPortName=COMM::/) 
		{
		print "\tSetting the COMM port\n";
		$line = "\tPortName=COMM::$com\n"
		}
	
	# If it's the rate then set that up
	if ($line =~ m/^\sRate=/) 
		{
		print "\tSetting the Baud rate\n";
		$line = "\tRate=$baud\n"
		}
	}

# Write out the (modified) line to the new commdb
print COMMDBOUT $line;
}

# Close the files
close(COMMDBOUT);
close(COMMDBREAD);


