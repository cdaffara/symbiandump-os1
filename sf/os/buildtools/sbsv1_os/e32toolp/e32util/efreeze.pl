# Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# all variables called *Path* are set up to end with a backslash
# all variables called *Path or *File are stored as absolute (file)paths
# all variables called UpPath* are stored as relative paths
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
# do not convert slashes for linux
	if ($^O eq "MSWin32") {
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
	my %Options;

	# process the command-line
	unless (GetOptions(\%Options, 'compare', 'remove')) {
		exit 1;
	}
	unless (@ARGV==2) {
		&Usage;
	}

	my $FRZFILE;
	my $GENFILE;

	($FRZFILE,$GENFILE)=(@ARGV);
	# if paths are not absolute, then convert them to absolute
	if( !(File::Spec->file_name_is_absolute($FRZFILE)) || !(File::Spec->file_name_is_absolute($GENFILE)) )
	{
		($FRZFILE,$GENFILE)=&Path_AbsToWork(@ARGV);
	}

	
#	check the file exists
	unless (-e $FRZFILE) {
		warn "WARNING: $FRZFILE: File not found - OK if freezing for first time\n";
	}
	unless (-e $GENFILE) {
		die "EFREEZE ERROR: $GENFILE: File not found\n";
	}

#	Read the Frozen .DEF file if it exists
	my @FrzDataStruct;
	my $FrzExportsOn=0;
	if (-e $FRZFILE) {
		eval { &Def_ReadFileL(\@FrzDataStruct, $FRZFILE, $FrzExportsOn); };
		die $@ if $@;
	}

#	Read the New .DEF file
	my @GenDataStruct;
	my $Dummy;
	if ($GENFILE) {
		eval { &Def_ReadFileL(\@GenDataStruct, $GENFILE, $Dummy, ($Options{compare} or $Options{remove})); };
	}
	die $@ if $@;

#	Compare the frozen .DEF data with the new .DEF file
	my (@NewDataStruct, @MissingDataStruct, @BadDataStruct);
	eval { &CompareFrzGenL (\@NewDataStruct, \@MissingDataStruct, \@BadDataStruct, \@FrzDataStruct, \@GenDataStruct, $Options{remove}); };
	die $@ if $@;

#	check for errors
	my $NumRemoved;
	my $Ref;
	my @Errors;
	my $Title='EFREEZE ERROR:';
	if ($Options{compare}) {
		$Title='EFREEZE:';
	}
	if (@MissingDataStruct and $Options{remove}) {
#		Mark missing exports as ABSENT in DEF file
		$NumRemoved=@MissingDataStruct;
		if ($Options{compare}) {
			print "EFREEZE: Marking $NumRemoved Export(s) as ABSENT :\n";
		} else {
			print "EFREEZE: Marking $NumRemoved Export(s) as ABSENT in $FRZFILE :\n";
		}
		foreach (@MissingDataStruct) {
			$$_{Absent} = 1;
			my $Comment='';
			if ($$_{Comment}) {
				$Comment=" ; $$_{Comment}";
			}
			my $r3unused = $$_{R3Unused} ? " R3UNUSED" : "";
			print "  $$_{Name} \@ $$_{Ordinal} NONAME$r3unused$Comment\n";
		}
	}
	elsif (@MissingDataStruct) {
		my $Num=@MissingDataStruct;
		push @Errors, "$Title $FRZFILE: $Num Frozen Export(s) missing from $GENFILE (POSSIBLE COMPATIBILITY BREAK):\n"; 
		foreach $Ref (@MissingDataStruct) {
			my $r3unused = $$Ref{R3Unused} ? " R3UNUSED" : "";
			push @Errors, "  $$Ref{LineNum}: $$Ref{Name} \@ $$Ref{Ordinal} NONAME$r3unused\n";
		}
		push @Errors, "\n";
	}
	if (@BadDataStruct) {
		my $Num=@BadDataStruct;
		push @Errors, "$Title $GENFILE: $Num function(s) exported at wrong ordinal:\n";
		foreach $Ref (@BadDataStruct) {
			my $r3unused = $$Ref{R3Unused} ? " R3UNUSED" : "";
			push @Errors, "  $$Ref{LineNum}: $$Ref{Name} \@ $$Ref{Ordinal} NONAME$r3unused\n";
		}
	}
	if (@Errors) {
		unless ($Options{compare}) {
			die @Errors;
		}
		else {
			print @Errors;
		}
	}

#	Update the frozen .DEF file
	eval { &UpdateFrzFileL(\@NewDataStruct, \@FrzDataStruct, $FRZFILE, $FrzExportsOn, $Options{compare}, $NumRemoved); };
	die $@ if $@;
	

	exit;
}

#######################################################################
# SUBROUTINES
#######################################################################

sub Usage () {

	print(
		"\n",
		"EFREEZE - .DEF file maintenance utility (Build ",&E32tpver,")\n",
		"\n",
		"EFREEZE {options} [frozen .DEF file] [new .DEF file]\n",
		"\n",
		"options:   (case-insensitive)\n",
		"  -Compare\n",
		"  -Remove\n",
		"\n"
	);
	exit 1;
}

sub CompareFrzGenL ($$$$$$) {
	my ($NewStructRef, $MissingStructRef, $BadStructRef, $FrzStructRef, $GenStructRef, $remove)=@_;

#	compare the input export data with the frozen data

#	take a copy of the frozen .DEF file structure that we can trash
	my @TmpStruct=@$FrzStructRef;

#	remove any entries not containing export data and get the highest ordinal value used
	my $LastOrdinal=0;
	foreach (@TmpStruct) {
		if ($$_{Name}) {
			if ($LastOrdinal<$$_{Ordinal}) {
				$LastOrdinal=$$_{Ordinal};
			}
			next;
		}
		undef $_;
	}

	my $GenRef;
	my $TmpRef;
	GENLOOP: foreach $GenRef (@$GenStructRef) {
		next unless $$GenRef{Name};		# ignore lines in the .DEF file not containing an export
		foreach $TmpRef (@TmpStruct) {
			next unless defined $TmpRef; # ignore nullified entries in the temporary array
#			does the function name match?
			if ($$GenRef{Name} eq $$TmpRef{Name}) {
#				check the names have the same ordinals
				if ($remove or $$GenRef{Ordinal}==$$TmpRef{Ordinal}) {
					undef $TmpRef;
					next GENLOOP;
				}
#				store exports with the wrong ordinals
				push @$BadStructRef, $GenRef;
				undef $TmpRef;
				next GENLOOP;
			}
#			Absent export?
			if ($$TmpRef{Absent} and $$TmpRef{Ordinal}==$$GenRef{Ordinal}) {
				next GENLOOP;
			}
		}
#		store new exports not found in the frozen .DEF file with the right ordinal value
		$LastOrdinal++;
		$$GenRef{Ordinal}=$LastOrdinal;
		push @$NewStructRef, $GenRef;
	}

#	all the exports left in the frozen .DEF file weren't found
	foreach $TmpRef (@TmpStruct) {
		next unless defined $TmpRef; # ignore nullified entries in the temporary array
		next if $$TmpRef{Absent};	# skip entries marked as ABSENT in the DEF file
		push @$MissingStructRef, $TmpRef;
	}
}

sub UpdateFrzFileL ($$$$$$) {
	my ($NewStructRef, $FrzStructRef, $FILE, $ExportsOn, $Compare, $NumRemoved)=@_;

#	add the exports to the frozen .DEF file text
	unless (@$NewStructRef or $NumRemoved) {
		print "EFREEZE: DEF file up to date\n";
		return;
	}

	my $NumNewExports=@$NewStructRef;
	unless ($Compare) {
# 		abort the operation unless the frozen .DEF file is writeable
		if (-e $FILE and not -w $FILE) {
			die
				"EFREEZE ERROR: Can't append $NumNewExports New Export(s) to $FILE\n",
				"  as file is not writeable.  Check source code control system.\n"
			;
		}
		print "EFREEZE: Appending $NumNewExports New Export(s) to $FILE:\n" if ($NumNewExports);
	}
	else {
		print "EFREEZE: $NumNewExports New Export(s):\n" if ($NumNewExports);
	}

	my @Text;
	my $ExportsDeclared;

#	Process the frozen .DEF file
	if (@$FrzStructRef) { # there is already a frozen .DEF file
		my $FrzRef;

#		get the lines of text from the frozen .DEF file
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
			my $r3unused = $$FrzRef{R3Unused} ? " R3UNUSED" : "";
			my $absent = $$FrzRef{Absent} ? " ABSENT" : "";

			my $data = "";
			if( !($$FrzRef{Name} =~ /^(_ZTI|_ZTV|_ZTT)/))
			{
#				A symbol name with the above pattern indicates that it is a Data symbol.
#				Mark symbols as DATA only when it cannot be found from the name alone (i.e.,
#				explicitly exported data symbols).

				if(($$FrzRef{Data}) and ($$FrzRef{Size}) ){
				$data = " DATA $$FrzRef{Size}";
				}
			}
			push @Text, "\t$$FrzRef{Name} \@ $$FrzRef{Ordinal} NONAME$data$r3unused$absent$Comment\n";
		}

#		strip any blank lines at the end of the frozen .DEF file text
		foreach (reverse @Text) {
			if (/^\s*$/o) {
				$_='';
				next;
			}
			last;
		}

	}

#	Add an EXPORTS section header if there aren't already exports
	unshift @Text, "EXPORTS\n" unless ($ExportsDeclared);

	my $NewRef;
	foreach $NewRef (@$NewStructRef) {
		my $Comment='';
		if ($$NewRef{Comment}) {
			$Comment=" ; $$NewRef{Comment}";
		}
		my $r3unused = $$NewRef{R3Unused} ? " R3UNUSED" : "";
		my $absent = $$NewRef{Absent} ? " ABSENT" : "";

		my $data = "";
		if( !($$NewRef{Name} =~ /^(_ZTV|_ZTI|_ZTT)/)) {
#				A symbol name with the above pattern indicates that it is a Data symbol.
#				Mark symbols as DATA only when it cannot be found from the name alone (i.e.,
#				explicitly exported data symbols).
			if(($$NewRef{Data}) and ($$NewRef{Size}) ){
			$data = " DATA $$NewRef{Size}";
			}
		}
		print "  $$NewRef{Name} \@ $$NewRef{Ordinal} NONAME$r3unused$Comment\n";
		push @Text, "\t$$NewRef{Name} \@ $$NewRef{Ordinal} NONAME$data$r3unused$absent$Comment\n";
	}

#	add a terminating newline
	push @Text, "\n";

	unless ($Compare) {
#		write the new frozen .DEF file
		eval { &Def_WriteFileL(\@Text, $FILE); };
		die $@ if $@;
	}
}

