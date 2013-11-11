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
# module for preprocessing makmake-style project files
# 
#


package Prepfile;

require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	Prepfile_SetVerbose Prepfile_SetUpperCase Prepfile_ProcessL Prepfile_SetCmdOptions
);

use strict;

use Checkgcc;
use Pathutl;
use Preprocessor;

my %Mode=(
	Verbose=>0,
	UpperCase=>0,
        CmdOptions=>''
);

sub Prepfile_SetVerbose () {
	$Mode{Verbose}=1;
}

sub Prepfile_SetUpperCase () {
	$Mode{UpperCase}=1;
}

sub Prepfile_SetCmdOptions ($) {
        $Mode{CmdOptions} = shift;
        $Mode{CmdOptions} .= ' ' if $Mode{CmdOptions};
}

sub Prepfile_ProcessL ($$;$@) {
# use the C++ preprocessor to process a file.  The function fills the data structure specified
# by the first argument, an array reference, according to the contents of the second argument -
# a filehandle.  Any other arguments are assumed to be MACROS and are defined for preprocessing.

	my ($ArrayRef,$FILE,$VariantHRHFile,@Macros)=@_;
	die "\nERROR: Project File \"$FILE\" not found\n" unless -e $FILE;

	my $exe = &PreprocessorToUseExe();
 	my $cpp = "$exe.EXE $Mode{CmdOptions}-undef -nostdinc -+ ";
	my @CppCall;
 	push @CppCall, $cpp;

	push @CppCall, join '',	"-I ",&Path_PrefixWithDriveAndQuote("$ENV{EPOCROOT}epoc32\\include"),
							" -I .",
							" -I ",&Path_PrefixWithDriveAndQuote(&Path_Split('Path',$FILE));

	my $Macro;
	# add CL macros to the CPP call for preprocessing of the file
	foreach $Macro (@Macros) {
		$Macro=uc $Macro;					 # add the underscores so we can tell what has been
		push @CppCall, "-D $Macro=_____$Macro"; # expanded and remove the space CPP puts in most of the time
	}
	#if a variant file is used
	if(defined($VariantHRHFile)){
        my $variantFilePath = Path_Split('Path',$VariantHRHFile);
	chop( $variantFilePath );
        push @CppCall, " -I " . &Path_PrefixWithDriveAndQuote($variantFilePath) . " -include " . &Path_PrefixWithDriveAndQuote($VariantHRHFile); 
	}
	# all macros made upper case and suppress user-expansion of macros for nefarious purposes

	push @CppCall, &Path_PrefixWithDriveAndQuote($FILE);
	if ($Mode{'Verbose'}) {
		print "@CppCall\n";
	}
	my $CPPPIPE;
	open CPPPIPE,"@CppCall |" or die "ERROR: Can't invoke CPP.EXE\n";

	# read the processed output
	#--------------------------

	my $LineNum=0;
	my $FileName;
	while (<CPPPIPE>) {

		# skip blank lines
		if (/^\s*$/o) {
			$LineNum++;
			next;
		}

		my @Tmp=();

	    # Process the file information lines that cpp inserts.
		# (note that we don't currently do anything with the
		# number cpp sometimes puts out after the filename -
		# it's something to do with inclusion nesting levels)
		if (/^# (\d+) "(.*)"( \d+)?/o) {
			$LineNum = scalar $1;
			my $CurFile=$2;
			$CurFile=~s-\\\\-\\-go;
			$CurFile=~s-\\\/-\\-go;
			$CurFile=~s-\/\\-\\-go;
			$CurFile=~s-\/\/-\\-go;
			$CurFile=~s-\/-\\-go;
			$CurFile=~s-(.:)--go;

			$CurFile=&Path_AbsToWork($CurFile);
			@Tmp=('#', $CurFile);
			push @{$ArrayRef}, [ @Tmp ];
			next;
		}

		# Process file data
		push @Tmp, $LineNum;
		# get rid of the space that cpp puts in most of the time after macro expansion (CPP help doesn't say exactly when!)
		# don't upper case everything until you've done this.
		
		foreach $Macro (@Macros) {
			s/\/ _____$Macro /\/$Macro/g;
			s/_____$Macro /$Macro/g;
			s/_____$Macro/$Macro/g;
		}
		if(/^macro/i)
		{
			#Parse and Store the mmp file statement by retaining double quotes
			while (/("([^\t\n\r\f]+)"|([^ \t\n\r\f]+))/go) {
			        my $Flag = $2 ? $2 : $3;
				if($Flag =~ m/\\\"/) { 
					$Flag =~ s/\"\\/\\/g;
					$Flag =~ s/\""/\"/g;
				}
				push @Tmp, $Flag;
			}
		}
		else
		{
			#Parse and Store the mmp file statement by removing double quotes
			while (/("([^"\t\n\r\f]+)"|([^ "\t\n\r\f]+))/go) {
				push @Tmp, $2 ? $2 : $3;
			}
		}
		push @{$ArrayRef}, [ @Tmp ];
		$LineNum++;
	}		

	if ($Mode{UpperCase}) {
#		upper-case all the data
		my $Line;
		my $Item;
		foreach $Line (@{$ArrayRef}) {
			foreach $Item (@$Line) {
				$Item=uc $Item;
			}
		}
	}
	close CPPPIPE or die $! ? "ERROR: Error closing $exe.exe pipe ($!)\n\t@CppCall\n"
 				: "ERROR: $exe.exe returned non-zero exit status ($?)\n\t@CppCall\n";
}

1;
