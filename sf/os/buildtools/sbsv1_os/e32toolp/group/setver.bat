@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@echo off

goto Invoke

#!perl

#
# use to set the version in relevant e32toolp files.
#
# Add new files requiring this service to the @InputFiles list below.
#
# Each file must contain the text to be changed in the form
# 'version=xxx', where xxx is a 3 digit number.
#

use strict;

my @InputFiles=qw(
	\E32TOOLP\E32UTIL\E32TPVER.PM
);
	
die "Usage:\nSETVER [version]\n" unless $#ARGV==0;

my $Version=shift @ARGV;

die "Unexpected version format\n" unless $Version=~/\d{3}/o;

my $FILE;
foreach $FILE (@InputFiles) {
	open FILE, $FILE or die "Can't open \"$FILE\": $!\n";
	my $FileText='';
	my $Unchanged=1;
	while (<FILE>) {
		if ($Unchanged) {
			$Unchanged=0 if s/^(.*version=)\d{3}([^\d]*.*)$/$1$Version$2/io;
		}
		$FileText.=$_;
	}
	open FILE,">$FILE" or die "Can't open \"$FILE\": $!\n";
	print FILE $FileText;
	close FILE or die "Can't close \"$FILE\": $!\n";
}

__END__

:Invoke
@rem command.com is rubbish and does not understand "%*"
@shift
@perl -x \E32TOOLP\GROUP\SETVER.BAT %0 %1 %2 %3 %4 %5 %6 %7 %8 %9














