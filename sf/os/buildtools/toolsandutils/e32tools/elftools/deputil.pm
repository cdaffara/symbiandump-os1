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
#

package DepUtil;

require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	GenDependencies
);
use strict;
my ($interworking, $outfile, $genpath, $libpath, $librariesRef, $gVerbose);
sub GenDependencies {
	($interworking, $outfile, $genpath, $libpath, $librariesRef, $gVerbose) = @_;
	my @dsoList;
	foreach my $alib (@$librariesRef) {
#	.lib files are archive file. Get the corresponding dso file which is in ELF.
		if($alib =~ /(.*)\.lib$/) {
			my $dso = "$1.dso";
			if(-e $dso){
				push @dsoList, $dso;
				}
		}
		elsif($alib =~ /.*\.dso$/) {
			if(-e $alib){
				push @dsoList, $alib;
				}
		}
	}
	my @linkAsList=();
	foreach my $dso (@dsoList) {
#	Get the linkas names of the libraries
		open PIPE, "getexports -s $dso |";
		while(<PIPE>) {
			if($_ =~ /(\S+)/){
			push @linkAsList, $1;
			}
	 	}
		close PIPE;
	}
	GenLinkerSteeringFileForDeps(\@linkAsList) if($outfile);
}

sub GenLinkerSteeringFileForDeps {
#	Generate the linker steering file to indicate in the final ELF file 
#	about the static dependencies. This list gives the list and order of
#	library names in which name lookup shall happen. Whether they were
#	actually linked-in in the final ELF shall be determined by the import
#	table. The import table fails to maintain the linking order and is
#	achieved through these linker directives.

	my ($LinkAsNamesRef) = @_;
	my $depfile = "$genpath\\$outfile.dep";
	open OUTFILE, ">$depfile" or
		die "Can't create file $depfile\n";
	print OUTFILE "; This is a generated file for Static Dependency listing";
	print OUTFILE "; causing the linker to create DT_NEEDED tag(s) in the dynamic array\n\n";

	foreach my $name (@$LinkAsNamesRef) {
		print OUTFILE "\n\tREQUIRE \"$name\"";
	}
	print OUTFILE "\n";
	close OUTFILE;
}

1;
