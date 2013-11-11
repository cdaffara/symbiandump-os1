# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# this module requires Pathutl module to have been 'used' by the main program
# General Def file utilities
# 
#

package Defutl;

require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	Def_ReadFileL Def_WriteFileL
);

use strict;
use File::Path;
use File::Basename;

sub Def_ReadFileL ($$$$) {
#	this function reads a .DEF file, putting the export information into a data
#	structure

	my ($DataStructRef, $FILE, $ExportsOn, $IgnoreSequence)=@_;

#	initialisation
	@$DataStructRef=();

#	this function expects all statements to appear on separate lines - though MSVC doesn't

	open (FILE, "<$FILE") or die "could not open $FILE: $!";

	my $PreviousOrdinal=0;
	my $MultiLineStatement='';
	my $NAMEorLIBRARYallowed=1;
	my $LineNum=0;
	while (<FILE>) {
		$LineNum++;

		my %Data;
		$Data{Line}=$_;
		$Data{LineNum}=$LineNum;

#		blank lines and comment lines
		if (/^\s*(;.*)?$/o) {
			push @$DataStructRef, \%Data;
			next;
		}

		if (/^\s*(EXPORTS|SECTIONS|IMPORTS)\s*(\s+\S+.*)?$/io) {
#			check for multi-line sections starting
			$MultiLineStatement=uc $1;
			$NAMEorLIBRARYallowed=0;
			unless ($2) {
				push @$DataStructRef, \%Data;
				next;
			}
			$_=$2;
		}
		elsif (/^\s*(NAME|LIBRARY|DESCRIPTION|STACKSIZE|VERSION)\s*(\s+\S+.*)?$/io) {
#			set MULTI-LINE statement to OFF
			$MultiLineStatement='';
#			check single-line statements are specified correctly
			$_=uc $1;
#			check NAME or LIBRARY statements aren't supplied incorrectly
			if (/^(NAME|LIBRARY)$/o) {
				unless ($NAMEorLIBRARYallowed) {
					die "$FILE($LineNum) : DEFFILE ERROR: NAME or LIBRARY statements must precede all other statements\n";
				}
				push @$DataStructRef, \%Data;
				next;
			}
			$NAMEorLIBRARYallowed=0;
			push @$DataStructRef, \%Data;
			next;
		}
		else {
			unless ($MultiLineStatement) {
				chomp $_;
				die "$FILE($LineNum) : DEFFILE ERROR: Unrecognised Syntax \"$_\"\n";
			}
		}

		if ($MultiLineStatement eq 'EXPORTS') {
#			get export data
			if (/^\s*(\S+)\s+\@\s*(\d+)\s*(NONAME)?\s*((DATA)\s*(\d+))?\s*(R3UNUSED)?\s*(ABSENT)?\s*(;\s*(.*))?$/io) {
				$Data{Name}=$1;
				$Data{Ordinal}=$2;
				if ($4) {
#					Mark the data symbols and retain the size.
					$Data{Data} = 1;
					if($6){
					$Data{Size} = $6;
					}
				}
				if ($7) {
					$Data{R3Unused} = 1;
				}
				if ($8) {
					$Data{Absent} = 1;
				}
				if ($10) {
					$Data{Comment}=$10;
				}

				if ($Data{Name} =~ /^(\S+?)=(\S+)$/) {
# 					rename this export
					$Data{ExportName}=$1;
					$Data{Name}=$2;
					if ($Data{Name} =~ /=/) {
						die "$FILE($LineNum) : DEFFILE ERROR: $Data{Name} Invalid rename syntax\n";
					}
				}
				else {
					$Data{ExportName}=$Data{Name};
				}
#				test the export - this is probably too slow to bother with
				my $ExportRef;
				unless ($IgnoreSequence) {
#					check ordinal specified in sequence - this check is a style matter
#					rather the a .DEF file syntax matter, so maybe it shouldn't be applied
					unless ($Data{Ordinal}==($PreviousOrdinal+1)) {
						die "$FILE($LineNum) : DEFFILE ERROR: Ordinal not specified in sequence\n";
					}
				}
				$PreviousOrdinal=$Data{Ordinal};
				push @$DataStructRef, \%Data;
				next;
			}
			if (/^\s*_E32Startup(\s*\=\s*__E32Startup)?\s+(;\s*(.*))?$/io) {
#				Emulator's special entrypoint ordinal
				next;
			}
			if (/^\s*_E32Dll(\s*\=\s*__E32Dll)?\s+(;\s*(.*))?$/io) {
#				Emulator's special entrypoint ordinal
				next;
			}
			die "$FILE($LineNum) : DEFFILE ERROR: Incorrect EXPORTS statement syntax\n";
		}

	}

#	decide whether we've ended up with an EXPORTS section specified
	if ($MultiLineStatement eq 'EXPORTS') {
		$_[2]=1; # $ExportsOn
	}
	else {
		$_[2]=0; # $ExportsOn
	}

	close FILE or die "DEFFILE ERROR: Can't close file $FILE: $!\n";
}

sub Def_WriteFileL ($$) {
#	Writes an array of text to a file

	my ($TextArrayRef, $FILE)=@_;
	
	# make sure path exists
	my($filename, $directories, $suffix) = fileparse($FILE);
	unless (-d $directories) {
		eval { mkpath[$directories] };
		die $@ if $@;
	} 

	open (FILE, ">$FILE") or die "DEFFILE ERROR: Could not open $FILE: $!\n";
	print FILE @$TextArrayRef or die "DEFFILE ERROR: Can't write output to $FILE: $!\n";
	close FILE or die "DEFFILE ERROR: Can't close file $FILE: $!\n";
}

1;
