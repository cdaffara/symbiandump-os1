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

REM a basic perl preprocessor

if exist %0.bat perl -x %0.bat %1 %2 %3 %4 %5
if exist %0 perl -x %0 %1 %2 %3 %4 %5
GOTO End

#!perl

use strict;

die "Usage:\nPERLPREP [infile] [outfile]\n" unless @ARGV==2;

my ($INFILE, $OUTFILE)=map lc $_, @ARGV;
die "Can't find $INFILE\n" unless -e $INFILE;

if ($INFILE=~/\.(bat|cmd|pm|pl)$/io) {
	open INFILE,$INFILE or die "Can't open $INFILE: $!\n";
	my $FileText='';
	while (<INFILE>) {
		s/(perl)(\.exe)?\s+-w/$1$2/io;		# remove perl -w switch - has to be first switch
		s/use\s+strict;\s*\n//oe;	# remove use strict; statement
		$FileText.=$_;
	}
	close INFILE or die "Can't close $INFILE: $!\n";
	open OUTFILE,">$OUTFILE" or die "Can't open $OUTFILE: $!\n";
	print OUTFILE $FileText;
	close OUTFILE or die "Can't close $OUTFILE: $!\n";
}
else {
	system "copy $INFILE $OUTFILE";
}


__END__

:End





