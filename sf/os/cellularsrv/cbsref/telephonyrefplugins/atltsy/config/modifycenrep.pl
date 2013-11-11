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
# This script modifies the CCE plugin cenrep file to use the CS plugin rather than the VCC one.
# 
#

use strict;

# Open the cenrep files
my $cenrep = "/epoc32/release/winscw/UDEB/z/private/10202be9/10282E7F.txt";
my $newcenrep = "/epoc32/release/winscw/UDEB/z/private/10202be9/10282E7F.tmp";
my $cenrepbak = "/epoc32/release/winscw/UDEB/z/private/10202be9/10282E7F.bak";
my $file2string;

# Open the file in UNICODE-16 and read the whole contents into a string
open(my $CENREPREAD, "<:raw:encoding(UTF-16LE):crlf:utf8", $cenrep) or die "Couldn't open current cenrep file";
{
$/ =undef;
$file2string = <$CENREPREAD>;
}
close($CENREPREAD);

# Modify the crucial line to the new UID for CS calls rather than VCC
$file2string =~ s/0x9001 string \"536924074\"/0x9001 string \"271067365\"/g;

# Write out the contents into a new UNICODE-16 encoded file
open(my $CENREPOUT, ">:raw:encoding(UTF-16LE):crlf:utf8", $newcenrep) or die "Couldn't open new cenrep file for output";
print $CENREPOUT $file2string;
close($CENREPOUT);


# Now backup the old and copy the new files
rename($cenrep, $cenrepbak) or die "Couldn't backup feature data file '$cenrep'\n" unless (-e $cenrepbak); # OK for this to fail as it probably just means there's already a backup
rename($newcenrep, $cenrep) or die "Couldn't copy feature data file '$newcenrep'\n";

print "\tCCE CenRep Setup\n";