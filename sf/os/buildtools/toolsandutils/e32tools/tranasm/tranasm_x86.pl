# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Small Perl script for converting GCC inline X86 assembly instructions 
# into a format acceptable to the MSVC compiler.
# 
#


	# Read the input .cia file
	open (INPUT_FILE,"$ARGV[0]") or
		die "Couldn't open input file $!";
	my @lines = <INPUT_FILE>;
	close INPUT_FILE;

	# Open the output file (somewhere under \epoc32\build)
	open (OUTPUT_FILE,">$ARGV[1]") or
		die "Couldn't open output file  $!";

	# Iterate each line of the input .cia
	foreach my $line (@lines) {

		# Filter GCC inline assembly lines
		$line=~s/\/\*(.*?)\*\///g;
		my $output_line=$line;
		if ($line=~m/^\s*asm\("(.*?)"(.*)\)(.*)/) {

			# The above regexp seperates the asm declaration into the instruction part, and
			# optionally a C expression as an operand
			my $instr=$1;
			my $c_arg=$2;
			my $eol=$3;

			# If a C operand is used, strip the weird GCC-specific syntax from it and paste
			# it into the '%0' or '%a0' operand in the assembly instruction
			if ($c_arg =~m/^\s*:\s*:\s*"i"\s*(.*)/) {
				$c_arg = $1;
				$output_line=$instr;

				# Use of _FOFF has to be converted to MSVC syntax
				if ($c_arg =~m/^_FOFF\((.*)\s*,\s*(.*)\)/) {
					my $classname=$1;
					my $member=$2;
					$output_line=~s/\+%0]/]$classname.$member/;
					$output_line=~s/\%0/$classname.$member/;
				}
				else {

					$c_arg=~s/&//;			# strip the '&' used for address-of global code/data
					$output_line=~s/%0/$c_arg/;
					$output_line=~s/%a0/$c_arg/;
				}
			}
			else {
				$output_line="$instr$c_arg";
			}

			# Prefix the MSVC __asm keyword
			$output_line = "__asm $output_line $eol\n";
		}
		print OUTPUT_FILE "$output_line";
	}

	close OUTPUT_FILE;

