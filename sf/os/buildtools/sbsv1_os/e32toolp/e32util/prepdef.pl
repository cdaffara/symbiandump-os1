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
# e32toolp/e32util/prepdef.pl
# 
#

use strict;

use FindBin;		# for FindBin::Bin
use Getopt::Long;

my $PerlLibPath;    # fully qualified pathname of the directory containing our Perl modules

BEGIN {
# check user has a version of perl that will cope
	require 5.005_03;
# establish the path to the Perl libraries: currently the same directory as this script
	$PerlLibPath = $FindBin::Bin;	# X:/epoc32/tools
	if ($^O eq "MSWin32")
	{
		$PerlLibPath =~ s/\//\\/g;	# X:\epoc32\tools
		$PerlLibPath .= "\\";
	}
}

use lib $PerlLibPath;
use Defutl;
use E32tpver;
use Pathutl;	



# THE MAIN PROGRAM SECTION
##########################

{
	

	# process the command-line
	unless (@ARGV==2 ||  @ARGV==3 || @ARGV==4) {
		&Usage;
	}
	my ($FRZFILE,$GENFILE,$DATASWITCH,$ABSENTSYM)=@ARGV;
	
	

#	Read the Frozen .DEF file
	my @FrzDataStruct;
	my $FrzExportsOn=0;
	eval { &Def_ReadFileL(\@FrzDataStruct, $FRZFILE, $FrzExportsOn); };
	die $@ if $@;

	eval { &WriteOutputFileL(\@FrzDataStruct, $GENFILE, $DATASWITCH, $ABSENTSYM); };
	die $@ if $@;

	exit;
}

#######################################################################
# SUBROUTINES
#######################################################################

sub Usage () {

	print(
		"\n",
		"PREPDEF - Prepare frozen DEF file for library generation (Build ",&E32tpver,")\n",
		"\n",
		"PREPDEF [frozen .DEF file] [processed .DEF file] [nodatasizes] [entrypoint_name]\n",
		"\n",
		"\n",
		"\n"
	);
	exit 1;
}



sub WriteOutputFileL ($$$$) {
	my ($FrzStructRef, $FILE, $DATASWITCH, $ABSENTSYM)=@_;

	my @Text;

	if ($ABSENTSYM) {
		$ABSENTSYM = '='.$ABSENTSYM;
	}

#	Process the frozen .DEF file
	my $FrzRef;
	my $ExportsDeclared;

#	get the lines of text from the frozen .DEF file
	foreach $FrzRef (@$FrzStructRef) {
		next if (!$FrzRef);
		if (!defined($$FrzRef{Ordinal})) {
			push @Text, $$FrzRef{Line};
			$ExportsDeclared = 1 if ($$FrzRef{Line} =~ /^\s*EXPORTS\s*(\s+\S+.*)?$/io);
			next;
		}
		my $Comment='';
		if ($$FrzRef{Comment}) {
			$Comment=" ; $$FrzRef{Comment}";
		}
#		Mention if it is a Data symbol along with its size.
		my $data = "";
		if(defined $$FrzRef{Data})
			{			
			if ($DATASWITCH eq "nodatasizes")
				{
				$data = " DATA";
				}
			else
				{
				$data = " DATA $$FrzRef{Size}";
				}
			}
		my $r3unused = $$FrzRef{R3Unused} ? " R3UNUSED" : "";
		my $ord = $$FrzRef{Ordinal};
		if ($$FrzRef{Absent}) {
			push @Text, "\t\"_._.absent_export_$ord\"$ABSENTSYM \@ $ord NONAME$data$r3unused$Comment\n";
		} else {
			my $export = $$FrzRef{ExportName};
			if ($export ne $$FrzRef{Name})
				{
				$export .= "=$$FrzRef{Name}";
				}
			push @Text, "\t$export \@ $ord NONAME$data$r3unused$Comment\n";
		}
	}
	unshift @Text, "EXPORTS\n" unless ($ExportsDeclared);

#	add a terminating newline
	push @Text, "\n";

#	write the new frozen .DEF file
	eval { &Def_WriteFileL(\@Text, $FILE); };
	die $@ if $@;
}

