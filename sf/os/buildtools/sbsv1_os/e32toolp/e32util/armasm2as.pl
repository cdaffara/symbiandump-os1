# Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# e32toolp\e32util\armasm2as.pl
# Convert an ARMASM assembler source or include file to AS format
# Syntax:
# perl armasm2as.pl <input> <output>
# 
#

if (scalar(@ARGV)!=2) {
	die "perl armasm2as.pl <input> <output>\n";
}
my ($infile, $outfile) = @ARGV;
open IN, $infile or die "Can't open $infile for input\n";
my $gccfile;
my @input;
while (<IN>) {
	push @input, $_;
	next if defined($gccfile);
	next if (/^\s*$/);
	if (/^\s*\@/) {
		$gccfile = 1;	# If first non-blank line starts with @, assume already in GCC format
	} else {
		$gccfile = 0;
	}
}
close IN;
my @output;
my $outref = \@output;
if ($gccfile) {
	$outref = \@input;
} else {
	process(\@input, \@output);
}
open OUT, ">$outfile" or die "Can't open $outfile for write\n";
print OUT @$outref;
close OUT;



sub process($$) {
	my $level=0;
	my @block;
	my ($inref, $outref) = @_;
	foreach $line (@$inref) {
		$line=~s/\s*$//;
		my $comment;
		if ($line =~ /\;(.*)$/o) {
			$comment = $1;
			$line =~ s/\;(.*)$//o;
			if ($line =~ /^\s*$/o and $level==0) {
				push @$outref, "$line\@$comment\n";
				next;
			}
			$comment = "\t\@$comment";
		}
		if ($line =~ /^\s+PRESERVE8/) {
			next;
		}
		if ($level == 1) {
			if ($line =~ /^\s+MEND/i) {
				process_macro(\@block, $outref);
				$level = 0;
			} else {
				push @block, $line;
			}
			next;
		} elsif ($level == 0) {
			if ($line =~ /^\s+MACRO\s*$/i) {
				@block = ();
				$level = 1;
				next;
			}
		}
		if ($line =~ /^\s+GBLL\s+(\S+)/i) {
			push @$outref, "\t.set\t$1, 0$comment\n";
			next;
		}
		if ($line =~ /^\s+GBLA\s+(\S+)/i) {
			push @$outref, "\t.set\t$1, 0$comment\n";
			next;
		}
		if ($line =~ /^\s+INCLUDE\s+(\S+)/i) {
			my $arg = $1;
			if ($arg =~ /(\w+)\.inc/io) {
				$arg = $1.'.ginc';
			}
			push @$outref, "\t.include\t\"$arg\"$comment\n";
			next;
		}
		if ($line =~ /^\s+IMPORT\s+(\S+)/i) {
			push @$outref, "\t.extern\t$1$comment\n";
			next;
		}
		if ($line =~ /^\s+EXPORT\s+(\S+)/i) {
			push @$outref, "\t.global\t$1$comment\n";
			next;
		}
		if ($line =~ /^\s+ELSE/i or $line =~ /^\s+\|/i ) {
			push @$outref, "\t.else$comment\n";
			next;
		}
		if ($line =~ /^\s+ENDIF/i or $line =~ /^\s+\]/i) {
			push @$outref, "\t.endif$comment\n";
			next;
		}
		if ($line =~ /^\s+LTORG/i) {
			push @$outref, "\t.ltorg$comment\n";
			next;
		}
		if ($line =~ /^\s+END$/i) {
			next;
		}
		if ($line =~ /^\s+\!\s*(\d+)\,\s*(.*?)$/) {
			my $msg = $2;
			push @$outref, "\t.print $msg\n\t.err$comment\n";
			next;
		}
		if ($line =~ /^\s+INIT_LOGICAL_SYMBOL\s+(\w+)(.*?)$/) {
			process_init_logical_symbol($1, $2, $outref, $comment);
			next;
		}
		if ($line =~ /^\s+INIT_NUMERIC_SYMBOL\s+(\w+)\s*\,\s*(.*?)$/) {
			process_init_numeric_symbol($1, $2, $outref, $comment);
			next;
		}
		if ($line =~ /^\s+AREA\s+(.*?)$/) {
			process_area($1, $outref, $comment);
			next;
		}
		if ($line =~ /^\s+\%\s+(.*?)$/) {
			my $expr = process_numeric_expr($1, $outref);
			push @$outref, "\t.space $expr$comment\n";
			next;
		}
		if ($line =~ /^\s+ALIGN\s*(.*?)$/) {
			process_align($1, $outref, $comment);
			next;
		}

		# Strip label if there is one
		my $label;
		if ($line =~ /^(\S+)\s*(.*?)$/) {
			$label = $1;
			$line = $2;
		} else {
			$line =~ s/^\s*//;
		}
		if ($line =~ /^SETL\s+(\S+)/i) {
			my $val = $1;
			my $expr = process_logical_expr($val, $outref);
			push @$outref, "\t.set\t$label,$expr$comment\n";
			next;
		}
		if ($line =~ /^SETA\s+(.*?)$/i) {
			my $val = $1;
			my $expr = process_numeric_expr($val, $outref);
			push @$outref, "\t.set\t$label,$expr$comment\n";
			next;
		}
		if ($line =~ /^(EQU|\*)\s+(.*?)$/i) {
			my $val = $2;
			my $expr = process_numeric_expr($val, $outref);
			push @$outref, "\t.equ\t$label,$expr$comment\n";
			next;
		}
		if ($line =~ /^(if|\[)\s+(.*?)$/i) {
			my $cond = $2;
			if ($cond =~ /^\s*(\:LNOT\:)?\s*\:DEF\:\s*(.*?)$/i) {
				my $n = $1;
				my $sym = $2;
				if ($sym =~ /^(\w|\\|\:)+$/) {
					if (uc($n) eq ':LNOT:') {
						push @$outref, "\t.ifndef\t$sym$comment\n";
					} else {
						push @$outref, "\t.ifdef\t$sym$comment\n";
					}
					next;
				}
			}
			my $expr = process_logical_expr($cond, $outref);
			push @$outref, "\t.if $expr$comment\n";
			next;
		}
		if ($line =~ /^(\=|DCB)\s*(.*?)$/) {
			process_dcb($label, $2, $outref, $comment);
			next;
		}
		if ($line =~ /^DCW\s*(.*?)$/) {
			process_dcw($label, $1, $outref, $comment);
			next;
		}
		if ($line =~ /^DCD\s*(.*?)$/) {
			process_dcd($label, $1, $outref, $comment);
			next;
		}
		if ($line =~ /^ROUT\s*$/) {
			$line = '';
		}
		if ($line =~ /^(\w+)\s+\%(\w+)\s*$/o) {
			# ARMASM local label reference
			my $inst = $1;
			my $llab = $2;
			if ($llab =~ /F\w?(\d+)/o) {
				$line = "$inst $1f";
			} elsif ($llab =~ /B\w?(\d+)/o) {
				$line = "$inst $1b";
			} else {
				die "Can't determine local label direction\n";
			}
		}
		if ($line =~ /^(\w+)\s+(\w+)\s*\,\s*\%(\w+)\s*$/o) {
			# ARMASM local label reference
			my $inst = $1;
			my $op1 = $2;
			my $llab = $3;
			if ($llab =~ /F\w?(\d+)/o) {
				$line = "$inst $op1\, $1f";
			} elsif ($llab =~ /B\w?(\d+)/o) {
				$line = "$inst $op1\, $1b";
			} else {
				die "Can't determine local label direction\n";
			}
		}

		$line = process_numeric_expr($line, $outref);
		if (defined($label)) {
			push @$outref, "$label\:\t$line$comment\n";
		} else {
			push @$outref, "\t$line$comment\n";
		}
	}
}


sub process_macro($$) {
	my ($inref, $outref) = @_;
	my $line;
	while(1) {
		$line = shift @$inref;
		last if ($line !~ /^\s*$/);
	}
	unless ($line =~ /^\s+(\w+)\s*(.*?)$/) {
		die "Bad macro - no name\n";
	}
	my $macro_name = $1;
	if ($macro_name eq 'INIT_LOGICAL_SYMBOL' or $macro_name eq 'INIT_NUMERIC_SYMBOL') {
		return;
	}
	$line = $2;
	my @args = split ( '\s*,\s*', $line );
	foreach (@args) {
		die "Bad macro argument name\n" unless (/^\$\w+$/);
		s/^\$//;
	}
	my $ev = '';
	foreach $arg (@args) {
		$ev.="s/\\\$$arg(\\W\|\$)/\\\\$arg\$1/go;";
	}
	foreach (@$inref) {
		eval $ev;
	}
	push @$outref, "\t.macro $macro_name ".join(',',@args)."\n";
	process($inref, $outref);
	push @$outref, "\t.endm\n";
}


sub process_logical_expr($$) {
	my ($val, $outref) = @_;
	$val = process_numeric_expr($val, $outref);
	$val =~ s/\<\=/________LE________/g;	# protect <= and >= during expansion of =
	$val =~ s/\>\=/________GE________/g;
	$val =~ s/\=/\=\=/g;					# equality operator is = on ARMASM, == on AS
	$val =~ s/________LE________/\<\=/g;
	$val =~ s/________GE________/\>\=/g;
	$val =~ s/\{TRUE\}/(1)/g;
	$val =~ s/\{FALSE\}/(0)/g;

	my @lops = split( /(\s*\:LAND\:\s*|\s*\:LOR\:\s*|\s*\:LNOT\:\s*|\s*\:DEF\:\s*)/, $val );
	foreach (@lops) {
		s/\s*\:LAND\:\s*/\:LAND\:/go;
		s/\s*\:LOR\:\s*/\:LOR\:/go;
		s/\s*\:LNOT\:\s*/\:LNOT\:/go;
		s/\s*\:DEF\:\s*/\:DEF\:/go;
	}
	my @lops2;
	while (scalar (@lops)) {
		my $x = shift @lops;
		if ($x eq ':DEF:') {
			my $sym = shift @lops;
			$sym =~ s/^\s*//;
			$sym =~ s/\s*$//;
			if ($sym =~ /^(\w|\$|\\)+$/) {
				push @$outref, "\t.ifdef $sym\n\t.set __defined__$sym, 1\n\t.else\n\t.set __defined__$sym, 0\n\t.endif\n";
				push @lops2, " __defined__$sym ";
			} else {
				die "Bad :DEF: operand\n";
			}
		} elsif ($x eq ':LAND:') {
			push @lops2, ' && ';
		} elsif ($x eq ':LOR:') {
			push @lops2, ' || ';
		} else {
			push @lops2, $x;
		}
	}
	my @lops3;
	while (scalar (@lops2)) {
		my $x = shift @lops2;
		if ($x eq ':LNOT:') {
			my $operand;
			while (1) {
				$operand = shift @lops2;
				last if ($operand !~ /^\s*$/);
			}
			push @lops3, "(0==($operand))";
		} else {
			push @lops3, $x;
		}
	}
	return join('',@lops3);
}

sub process_numeric_expr($$) {
	my ($val, $outref) = @_;
	$val =~ s/\&/0x/g;			# ARMASM accepts hex numbers starting with & or 0x, AS only accepts 0x
	$val =~ s/(\W|^)2_([0|1]+)(\W|$)/$1 0b$2$3/g;	# Binary numbers start with 2_ on ARMASM, 0b on AS
	$val =~ s/\:AND\:/\&/g;
	$val =~ s/\:OR\:/\|/g;
	$val =~ s/\:SHL\:/\<\</g;
	$val =~ s/\:SHR\:/\>\>/g;
	return $val;
}

sub process_init_logical_symbol($$$$) {
	my ($name, $rest, $outref, $comment) = @_;
	my $counter;
	if ($rest =~ /^\s*\,\s*(\w+)\s*$/) {
		$counter = $1;
	} elsif ($rest !~ /^\s*$/) {
		die "Bad INIT_LOGICAL_SYMBOL\n";
	}
	push @$outref, "\t.ifndef $name$comment\n\t.set $name, 0\n\t.else\n\t.set $name, 1\n";
	if ($counter) {
		push @$outref, "\t.set $counter, $counter \+ 1\n";
	}
	push @$outref, "\t.endif\n";
}

sub process_init_numeric_symbol($$$$) {
	my ($name, $value, $outref, $comment) = @_;
	my $expr = process_numeric_expr($value, $outref);
	push @$outref, "\t.ifndef $name$comment\n\t.set $name, $expr\n\t.endif\n";
}

sub process_area($$$) {
	my ($line, $outref, $comment) = @_;
	my @args = split(',',$line);
	my $align = 0;
	foreach (@args) {
		if (/^\s*ALIGN\s*\=\s*(\d+)\s*$/) {
			$align = $1;
		}
	}
	push @$outref, "\t.text$comment\n\t.p2align $align\n";
}

sub process_align($$$) {
	my ($line, $outref, $comment) = @_;
	if ($line =~ /^\s*$/o) {
		push @$outref, "\t.align$comment\n";
	} else {
		push @$outref, "\t.balign $line$comment\n";
	}
}

sub process_dcb($$$$) {
	my ($label, $args, $outref, $comment) = @_;
	if (defined($label)) {
		push @$outref, "$label\:";
	}
	while ($args !~ /^\s*$/) {
		my $arg;
		$args =~ s/^\s*//;
		if ($args =~ /^\"/) {
			$args =~ s/\\\"/________ESCAPED_QUOTE________/go;
			$args =~ /\"(.*?)\"\s*(.*?)$/o or die "Unterminated string literal\n";
			$arg = $1;
			$args = $2;
			$arg =~ s/________ESCAPED_QUOTE________/\\\"/go;
			push @$outref, "\t.ascii \"$arg\"$comment\n";
			undef $comment;
			last if ($args !~ /\s*\,(.*?)$/o);
			$args = $1;
		} else {
			$args =~ /(.*?)\s*(\,|$)(.*?)$/o;
			$arg = $1;
			$args = $3;
			my $expr = process_numeric_expr($arg, $outref);
			push @$outref, "\t.byte $expr$comment\n";
			undef $comment;
		}
	}
}

sub process_dcw($$$$) {
	my ($label, $args, $outref, $comment) = @_;
	if (defined($label)) {
		push @$outref, "$label\:";
	}
	while ($args !~ /^\s*$/) {
		my $arg;
		$args =~ s/^\s*//;
		$args =~ /(.*?)\s*(\,|$)(.*?)$/o;
		$arg = $1;
		$args = $3;
		my $expr = process_numeric_expr($arg, $outref);
		push @$outref, "\t.hword $expr$comment\n";
		undef $comment;
	}
}

sub process_dcd($$$$) {
	my ($label, $args, $outref, $comment) = @_;
	if (defined($label)) {
		push @$outref, "$label\:";
	}
	while ($args !~ /^\s*$/) {
		my $arg;
		$args =~ s/^\s*//;
		$args =~ /(.*?)\s*(\,|$)(.*?)$/o;
		$arg = $1;
		$args = $3;
		my $expr = process_numeric_expr($arg, $outref);
		push @$outref, "\t.word $expr$comment\n";
		undef $comment;
	}
}



