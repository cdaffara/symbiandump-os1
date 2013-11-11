# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#!/usr/bin/perl

use Cwd;


my $fileName = shift;

die "Specify a file\n" if ($fileName eq "");
die "No such path $fileName\n" if (! -f $fileName);

my $inputFile = @_[0];
my $newName;
my $comment;
my $spacing;

if (open (INFILE, $fileName)) {
    my @content = <INFILE>;
    close INFILE;
    
    foreach $line (@content) {
        if ($line =~ m/^[ \t]*\/\/.*/) {
            next;
        }
        if ($line =~ m/typedef (.*)<(.*),.*> T(.*);/) {
            $sig = $1;
            $enum = $2;
            $message = $3;
            
            if ($sig =~ m/.*::(.*)$/) {
                $sig = $1;
            }
            
        print "MESSAGE $message\n    SIGNATURE = $sig\n    MESSAGEID = ";
        print "KESockInternalRealmId:$enum\nEND MESSAGE\n\n";
        }
    }
}
