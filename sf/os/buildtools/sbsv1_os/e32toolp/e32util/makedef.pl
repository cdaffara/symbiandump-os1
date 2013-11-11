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

use strict   ;

use FindBin;		# for FindBin::Bin
use Getopt::Long;

my $PerlBinPath;	# fully qualified pathname of the directory containing this script
my $EntryPoint;         
my $InternalEntryPoint;

# establish the path to the Perl binaries
BEGIN {
	require 5.005_03;				# check user has a version of perl that will cope
	$PerlBinPath = $FindBin::Bin;	# X:/epoc32/tools

	if ($^O eq "MSWin32")
		{	
		$PerlBinPath =~ s/\//\\/g;		# X:\epoc32\tools
		}
}
use lib $PerlBinPath;

use Defutl;
use E32tpver;
use Pathutl;
use File::Copy;

my %Options;	# command line option information

my $NamedSymLkup = 0;			# This flag is used to enable named lookup on emulator
my $IgnoreUnfrozenExports = 0;	# This flag is used to ignore the 'unfrozen exports' warnings. This is 
								# required for STDEXEs which export symbols just to enable 'named lookup'
								# from within the STDEXE (via 'dlsym') and are never exposed outside (via def files or
								# import libraries.
my $ExportEntrypointE32Dll = 0;	 # Workaround: To export entry point _E32DLL for target type STDDLL

# THE MAIN PROGRAM SECTION
##########################

{

	my $INFILE;
	my $FRZFILE;
	my $OUTFILE;
	my @ObjFiles;

	# process the command-line
	unless (GetOptions(\%Options, '1=s', '2=s', 'deffile=s', 'frzfile=s', 'inffile=s', 'overwrite', 'absent=s', 'ignore_unfrozen_noncallable', 'SystemTargetType', 'sym_name_lkup', 'ignore_unfrozen_exports','export_entrypoint_E32Dll')) {
		exit 1;
	}
	unless (@ARGV==1) {
		&Usage;
	}

#	check the flags

	if (($Options{deffile} and $Options{inffile}) or (not ($Options{deffile} or $Options{inffile}))) {
		die "MAKEDEF ERROR: Must specify either -Deffile [file] or -Inf [file]\n";
	}
	if ($Options{2} && !$Options{1}) {
		die "MAKEDEF ERROR: Can't specify second export name and not first export name\n";
	}

#	process the flags
	if ($Options{deffile}) {
		$INFILE=$Options{deffile};
		unless (-e $INFILE) {
			die "MAKEDEF ERROR: $INFILE: Deffile not found\n";
		}
	}
	else {
		$INFILE=$Options{inffile};
		unless (-e $INFILE) {
			die "MAKEDEF ERROR: $INFILE: Inffile not found\n";
		}
	}
	if ($Options{frzfile}) {
		$FRZFILE=$Options{frzfile};
#		check the frozen .DEF file exists
		unless (-e $FRZFILE) {
			die "MAKEDEF ERROR: $FRZFILE: Frzfile not found\n";
		}
	}
	$OUTFILE=pop @ARGV;


	$NamedSymLkup = $Options{sym_name_lkup};
	$IgnoreUnfrozenExports = $Options{ignore_unfrozen_exports};
	$ExportEntrypointE32Dll = $Options{export_entrypoint_E32Dll};	# Workaround: To export entry point _E32DLL for target type STDDLL

#	Read the Frozen .DEF file if specified
	my @FrzDataStruct;
	if ($FRZFILE) {
		eval { &Def_ReadFileL(\@FrzDataStruct, $FRZFILE); };
		die $@ if $@;
		if ($Options{1}) {
#			Check that frozen def file matches the -1 and -2 arguments given, if any
			my $export1="";
			my $export2="";
			foreach my $FrzRef (@FrzDataStruct) {
				next unless $$FrzRef{Name};		# ignore lines not containing an export
				if ($$FrzRef{Ordinal} == 1) {
					$export1 = $$FrzRef{Name};
					next;
				}
				if ($$FrzRef{Ordinal} == 2) {
					$export2 = $$FrzRef{Name};
					next;
				}
				if ($Options{1} && ($Options{1} ne $export1)) {
					die "MAKEDEF ERROR: $FRZFILE: Frzfile ordinal 1 does not match command line\n";
				}
				if ($Options{2} && ($Options{2} ne $export2)) {
					die "MAKEDEF ERROR: $FRZFILE: Frzfile ordinal 2 does not match command line\n";
				}
			}
		}
	}
	elsif ($Options{1}) {
#		create an export structure for the names passed on the command line
		push @FrzDataStruct, {
			Name=>$Options{1},
			ExportName=>$Options{1},
			Ordinal=>1
		};
		if ($Options{2}) {
			push @FrzDataStruct, {
				Name=>$Options{2},
				ExportName=>$Options{2},
				Ordinal=>2
			};
		}
	}

#	Read the Input .DEF file
	my @InDataStruct;
	if ($Options{deffile}) {
		eval { &Def_ReadFileL(\@InDataStruct, $INFILE); };
	}
	else {
		eval { &ReadInfFileL(\@InDataStruct, $INFILE); };
	}
	die $@ if $@;

#	Compare the frozen .DEF data with the input .DEF or export info file data
	my (@NewDataStruct, @MissingDataStruct, @MatchedDataStruct);
	eval { &CompareFrzInL (\@NewDataStruct, \@MissingDataStruct, \@MatchedDataStruct, \@FrzDataStruct, \@InDataStruct); };
	die $@ if $@;

	# MAKEDEF should generate a warning if the def file has no exports (old or new)
	# and the associated MMP 'targettype' is not a System Target type.
	print "MAKEDEF WARNING: $OUTFILE has no EXPORTS\n" unless (@MatchedDataStruct || @NewDataStruct || $Options{SystemTargetType});

#	Create the output .DEF file
	eval { &CreateDefFileL(\@NewDataStruct, \@MatchedDataStruct, $OUTFILE, $FRZFILE); };
	die $@ if $@;

#	report missing frozen export errors
	if (@MissingDataStruct) {
		my @Errors;
		my $Num=@MissingDataStruct;
		my $Ref;
		if ($FRZFILE) {
			push @Errors, "MAKEDEF ERROR: $Num Frozen Export(s) missing from object files (POSSIBLE COMPATIBILITY BREAK):\n";
			foreach $Ref (@MissingDataStruct) {
				push @Errors, "  $FRZFILE($$Ref{LineNum}) : $$Ref{Name} \@$$Ref{Ordinal}\n";
			}
		}
		else {
			push @Errors, "MAKEDEF ERROR: command-line: $Num Frozen Export(s) missing from object files (POSSIBLE COMPATIBILITY BREAK):\n";
			foreach $Ref (@MissingDataStruct) {
				push @Errors, "  $$Ref{Name} \@$$Ref{Ordinal}\n";
			}
		}
		die "\n", @Errors;
	}
	elsif ($Options{overwrite} && -w $FRZFILE) #sag
		{
		print "Copying $OUTFILE to $FRZFILE\n";
		rename $FRZFILE, "$FRZFILE.bak";
		copy($OUTFILE, $FRZFILE);
		}

	exit 0;
}

#######################################################################
# SUBROUTINES
#######################################################################

sub Usage () {

	print(
		"\n",
		"MAKEDEF - .DEF file generator (Build ",&E32tpver,")\n",
		"\n",
		"MAKEDEF {options} [Output .DEF file]\n",
		"\n",
		"options:   (case-insensitive)\n",
		"  -Deffile [Input .DEF file]\n",
		"  -Inffile [Input export information file]\n",
		"  -Frzfile [Frozen .DEF file]\n",
		"  -1 [first export name] {-2 [second export name]}\n",
		"  -Overwrite\n",
		"  -Absent [symbol to use for absent exports]\n",
		"  -ignore_unfrozen_noncallable\n",
		"  -SystemTargetType\n",
		"  -sym_name_lkup [Enable symbol lookup by name]\n",
		"  -ignore_unfrozen_exports \n",
		"\n",
		"Either specify -Deffile or -Inffile, and\n",
		"either -Frzfile or -1 {-2} if required.\n"
	);
	exit 1;
}

sub ReadInfFileL ($$$) {
	my ($DataStructRef, $FILE)=@_;

#	open export information file for reading
	open (FILE, "<$FILE") or die "could not open $FILE: $!";

#	read the export info. file, and create dummy frozen .DEF file text
	my $LineNum=0;
	my $Ordinal=0;
	my $Comment='';
	my $Name='';
	my $InfType=0;	# dumpbin output
	my %exports;	# MWLD workaround - record mangled names in case we don't see the demangled ones
	my $Line;
	while ($Line=<FILE>) {
	        my $Data=0;
			my $SymbolSize=0;
		$LineNum++;
		if ($InfType == 0) {
			if ($Line =~ /\*\*\* ARCHIVE SYMBOL TABLE.* \*\*\*/o) {
				$InfType=1;	# mwld disassembly output
				next;
			}
			if ($Line =~ /^\s+(\?\S+)(\s+\((.*)\))?$/o) {
#                   ??0TAgnAlarmDefaults@@QAE@XZ (public: __thiscall TAgnAlarmDefaults::TAgnAlarmDefaults(void))
				$Name=$1;
				$Comment=$3;
			} 
			elsif ($Line =~ /^\s+_(\S+)(\s+\((.*)\))?$/o) {
# frozen ordinals like "UserNoLongerSupported01" seem to inherit a leading underscore in the dumpbin output
				$Name=$1;
				$Comment=$3;
			}
			elsif ($Line =~ /^\s+_(\S+) DATA (\d+)(\s+\((.*)\))?$/o) {
#				Mark the data symbols and retain their sizes
				$Name=$1;
				$Data=1;
				$SymbolSize=$2;
				$Comment=$4;
			}
			else {
				next;
			}
		}
		else {
			# The Windows CW linker produces .inf files with a 'something' unmangled comment at the end, the Linux CW linker doesn't
			if ($Line =~ /^\s*\d+\s+\(.+\)\s+__imp_((.)(\S+))(\s+'__declspec\(dllimport\)\s+(.*)')?$/o) {
					if ($2 eq "_") {
							$Name = $3;		# C symbol, so remove leading underscore
							$Comment = $5 if ($4);	# name isn't mangled anyway
					} else {
							$Name = $1;		# C++ mangled name
							$Comment = $5 if ($4);	# can't unmangle names...
					}

					# One side-effect of ignoring comments if they're not present is that Windows entry points need to be
					# specifically ignored.  Previously they were ignored by virture of the fact they had no comment.
					next if ($Name eq "_E32Dll" || $Name eq "_E32Startup");

					$Comment = $4 ? $5 : $Name;
					
					# need to save both the line number and
					# comment
					my %entry;
					$entry{'lineNum'} = $LineNum;
					$entry{'comment'} = $Comment;
					$exports{$Name}=\%entry;
					next;
			}
			if ($Line =~ /\*\*\* Unmangled Symbols \*\*\*/o) {
				# Currently for Linux "Unmangled Symbols" section is blank
				<FILE>; 
				$Line = <FILE>; 
				$LineNum+=2;
				if ($^O eq "MSWin32") {
					if ($Line !~ /^\s*\d+:\s+(\S+)$/o) {
						print STDERR "MAKEDEF WARNING: unknown inf file format\n";
						next;
					}
				}
				$Name = length $1 ? $1 : '';
# Workaround: if MWLD can't demangle the name, we will see only the __imp_ version.
				if ($Name =~ /^__imp_(\S+)$/o) {
					$Name = $1;
				}
				$Line = <FILE>; 
				$LineNum++;
				next if ($Line !~ /^\s+(.+)$/o);
				$Comment = $1;
			}
			elsif ($Line =~ /^0x\S{8}\s+__imp__(\S+)$/o) {
				$Name = $1;	# leading underscore already removed
				$Comment = '';	# a C symbol, and therefore not mangled
			}
			else {
				next;
			}
		}
# Check for WINS entrypoint symbols
		if ($Name eq "_E32Dll" || $Name eq "_E32Startup") {
			$EntryPoint = $Name;
			# when mwld resolves an exported symbol S coming from
			# the def file, it looks both for S() and _S() in
			# every object file but only for _S() in static
			# libraries.
			#
			# As a consequence, we need to distinguish the
			# internal entry point name from the external one.
			$InternalEntryPoint = "_$Name" if ($InfType != 0);
			my $entry = $exports{$Name};
			$entry->{'lineNum'} = 0; # indicates processed name
			next;
		}
		$Ordinal++;
		$Comment='' if (!defined $Comment);
		push @$DataStructRef, {
			Ordinal=>$Ordinal,
			Name=>$Name,
			Data=>$Data,
			Size=>$SymbolSize,
			ExportName=>$Name,
			Comment=>$Comment,
			LineNum=>$LineNum
		};
		my $entry = $exports{$Name};
		$entry->{'lineNum'} = 0; # indicates processed name
	}
	foreach $Name (keys %exports) {
	    	my $entry = $exports{$Name};
		$LineNum = $entry->{'lineNum'};
		if ($LineNum > 0) {
			$Ordinal++;
			push @$DataStructRef, {
				Ordinal=>$Ordinal,
				Name=>$Name,
				ExportName=>$Name,
				Comment=> $entry->{'comment'},
				LineNum=>$LineNum
			};
		}
	}
}

sub CompareFrzInL ($$$$$) {
	my ($NewStructRef, $MissingStructRef, $MatchedStructRef, $FrzStructRef, $InStructRef)=@_;
	my $AbsentSubst = $Options{absent};
	my $IgnoreNoncallable = $Options{ignore_unfrozen_noncallable};
	
#	compare the input export data with the frozen data

#	this function trashes the frozen .DEF data structure and the new .DEF data structure

#	nullify non-export statements in the structures
	foreach (@$FrzStructRef,@$InStructRef) {
		next if $$_{Name};
		undef $_;
	}

	my $LastOrdinal=0;

	my $FrzRef;
	my $InRef;
	FRZLOOP: foreach $FrzRef (@$FrzStructRef) {
		next unless $$FrzRef{Name};		# ignore lines in the .DEF file not containing an export
		if ($LastOrdinal<$$FrzRef{Ordinal}) {
			$LastOrdinal=$$FrzRef{Ordinal};
		}
		foreach $InRef (@$InStructRef) {
			next unless defined $InRef; # ignore nullified entries in the temporary array
#			does the function name match?
			if ($$InRef{Name} eq $$FrzRef{Name}) {
#				give the generated export the same number as the corresponding frozen one
				$$InRef{Ordinal}=$$FrzRef{Ordinal};
				$$InRef{Data}=$$FrzRef{Data};
				$$InRef{Size}=$$FrzRef{Size};
#				if the export is marked as absent, redirect it appropriately
				if ($$FrzRef{Absent}) {
					if ($AbsentSubst) {
						$$InRef{Name} = $AbsentSubst;
						$$InRef{ExportName} = sprintf("\"_._.absent_export_%d\"", $$InRef{Ordinal});
					}
				}
				push @$MatchedStructRef, $InRef;
				undef $InRef;
				next FRZLOOP;
			}
		}
#		these frozen exports haven't been found in the object files
#		first check for ABSENT declarations
		if ($AbsentSubst and $$FrzRef{Absent}) {
			$$FrzRef{Name} = $AbsentSubst;
			$$FrzRef{ExportName} = sprintf("\"_._.absent_export_%d\"", $$FrzRef{Ordinal});
			push @$MatchedStructRef, $FrzRef;
			next FRZLOOP;
		}

#		No - it's really missing
		push @$MissingStructRef, $FrzRef;
#		put a comment in the generated .DEF file to that effect
		$$FrzRef{Missing}=1;
		push @$MatchedStructRef, $FrzRef;
	}

#	all the exports left in the new .DEF file aren't frozen - give them the right ordinals
	foreach $InRef (@$InStructRef) {
		next unless defined $InRef; # ignore nullified entries
		if ($$InRef{Name} =~ /^_ZTV|_ZTI/) {
			# EABI non-callable exports
			next if ($IgnoreNoncallable);	
		}
		$LastOrdinal++;
		$$InRef{Ordinal}=$LastOrdinal;
		push @$NewStructRef, $InRef;
	}
}

sub CreateDefFileL ($$$$) {
#	creates a new .DEF file
	my ($NewStructRef, $MatchedStructRef, $FILE, $FRZFILE)=@_;

	my @Text=("EXPORTS\n");
	my $LineNum=1;


	my $InRef;
	foreach $InRef (@$MatchedStructRef) {
		my $Comment='';
		if ($$InRef{Comment}) {
			$Comment=" ; $$InRef{Comment}";
		}
		if ($$InRef{Missing}) {
			push @Text, '; MISSING:';
		}
		my $Data = "";
		if( defined $$InRef{Data} && $$InRef{Data} == 1) {
		$Data = " DATA $$InRef{Size}" ;
		}
		my $r3unused = $$InRef{R3Unused} ? " R3UNUSED" : "";

#		A def file entry with the keyword 'NONAME' indicates the MW linker that a named-lookup is not enabled. 
#		Note that although it may seem, but named lookup is either enabled or disabled on a per-binary basis and not
#		per-symbol.
		my $noname = $NamedSymLkup ? "": " NONAME";
		if ($$InRef{ExportName} and ($$InRef{ExportName} ne $$InRef{Name})) {
			push @Text, "\t$$InRef{ExportName}=$$InRef{Name} \@ $$InRef{Ordinal} $noname$Data$r3unused$Comment\n";
		} else {
			push @Text, "\t$$InRef{Name} \@ $$InRef{Ordinal} $noname$Data$r3unused$Comment\n";
		}
		$LineNum++;
		next;
	}
	if (@$NewStructRef) {

#		warn about unfrozen exports and add them to the end of the generated .DEF file
		my $Num=@$NewStructRef;
		my @Warnings;

		if(!$IgnoreUnfrozenExports) {
			my $warning = "MAKEDEF WARNING: $Num export(s) not yet Frozen";

			if ($FRZFILE)
				{
				$warning .= " in $FRZFILE";
				}

			$warning .= ":\n";

			push @Warnings, $warning;
		}

		push @Text, "; NEW:\n";
		$LineNum++;
		foreach $InRef (@$NewStructRef) {
			my $Comment='';
			if ($$InRef{Comment}) {
				$Comment=" ; $$InRef{Comment}";
			}
			my $Data = "";
			if(defined $$InRef{Data} && $$InRef{Data} == 1){
			$Data = " DATA $$InRef{Size}";
			}
			my $r3unused = $$InRef{R3Unused} ? " R3UNUSED" : "";
			my $noname = $NamedSymLkup ? "": " NONAME";
			if ($$InRef{ExportName} and ($$InRef{ExportName} ne $$InRef{Name})) {
				push @Text, "\t$$InRef{ExportName}=$$InRef{Name} \@ $$InRef{Ordinal} $noname$Data$r3unused$Comment\n";
			} else {
				push @Text, "\t$$InRef{Name} \@ $$InRef{Ordinal} $noname$Data$r3unused$Comment\n";
			}
			$LineNum++;
			if(!$IgnoreUnfrozenExports) {
				push @Warnings, "  $FILE($LineNum) : $$InRef{Name} \@$$InRef{Ordinal}\n";
			}
			next;
		}
		print @Warnings;
	}
	if ($EntryPoint) {
		push @Text, "\t$EntryPoint";
		push @Text, "=$InternalEntryPoint" if ($InternalEntryPoint);
		push @Text, "\t; Entry point for emulation\n";
	}
	elsif ($ExportEntrypointE32Dll) {		# Workaround: To export entry point _E32DLL for target type STDDLL
		push @Text, "\t_E32Dll";
		push @Text, "=__E32Dll" ;
		push @Text, "\t; Entry point for STDDLL emulation\n";
	}

#	add a terminating newline
	push @Text, "\n";

#	write the new .DEF file
	eval { &Def_WriteFileL(\@Text, $FILE); };
	die $@ if $@;
}

