#!/usr/bin/perl
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
#

use strict;
use Getopt::Long;
use Cwd;

my $Pwd = cwd;

my $incroot = $Pwd;
if ($Pwd =~ /.\:(.*)$/) {
	$incroot = $1;
	$incroot =~ s/\//\\/go;
	$incroot = "$incroot"."\\";
}

if ($^O == "MSWin32" ) {
	my $PATHSEP='\\'
} else {
	my $PATHSEP='/'
}


my $commentToken = "//";

my $emitUnimplemented = 1;

my %opts = ();

my $result = GetOptions(\%opts,
						"record-emitter",
						"suppress-check",
						"no-original",
						"output:s",
						"error-string:s",
						"autoflush",
						"help",
						"lineno",
						);

Usage() if(!$result || $opts{'help'} || @ARGV < 1);

my $errorString = "\t>>> CHECK THIS <<<";

my $recordEmitter = $opts{"record-emitter"};
my $plineno = $opts{"lineno"};
my $forceCheck = !$opts{"suppress-check"};
my $printOriginal = !$opts{"no-original"};
my $outfile = $opts{"output"} if $opts{"output"};
my $infile = @ARGV[0];
$errorString = $opts{"error-string"} if $opts{"error-string"};

#my $symbolsfile = "tranasm-symbols.log";
my $symbolsfile = "";
my $savedOut;
# set to false to prevent recording files in \tranlated-files.log
#my $recordFile = 0;

#system "echo $infile >> \\translated-files.log" if $recordFile;
my @unmangledSymbols;
my $recordUnmangledSymbols = $symbolsfile;

if ($outfile) {
	open OUT, ">$outfile";
	$savedOut = select OUT;
}

$| = $opts{"autoflush"};


my $labelN = 0;
my $labelRoot = "Label";

my $lineno = 0;
my @knownLabels = ();

sub Croak($)
{
	my ($msg) = @_;
    die  "\nERROR: line $.: $msg";
}

sub PrintComment($)
{
	my ($comment) = @_;
    printf "\t$commentToken$comment" if $comment;
}

sub PrintCheck() { printf "\t$errorString\n" if $forceCheck; }

sub Nl () { printf "\n"; }

# cache for results of unmangling....
my %unmangledSymbols = ();
# cache to say whether symbol was mangled
my %mangledSymbols = ();

my $sourcefile = "\"$infile\"";

my @IncFiles;


sub Unmangle ($)
{
	my ($str) = @_;
	return $str if ($str =~ /\s*__cpp\(/); # these don't need unmangling
	my $res = $unmangledSymbols{$str};
	if ($res) {
		my $l = $lineno;
		if ($mangledSymbols{$str}) {
			my $sfile = $sourcefile;
			$sfile =~ s/\"//go;
			$sfile =~ s/\\\\/${main::PATHSEP}/go;
			push @unmangledSymbols, "$incroot$sfile $l: Symbol = $str : Result = $res : Filt = \?\?\? : @IncFiles"
			}
		return $res;
	} else {
		return $unmangledSymbols{$str} = UnmangleX($str);
	}
}

sub UnmangleX ($)
{
	my ($str) = @_;

	my $sfile = $sourcefile;
	$sfile =~ s/\"//go;
	$sfile =~ s/\\\\/${main::PATHSEP}/go;

	# recognize non-c++ derived symbols/labels
	if ($str =~ /^\s*(__.*)\s*$/) {
	    $str =~ s/\./_/;
	    return $str;
	} 

	my $cppfilt = $ENV{CPPFILT} ? $ENV{CPPFILT} : "c++filt";
	open UNM, "$cppfilt -s gnu $str|" or die "Error: Tranasm problem running $cppfilt to unmangle symbols.\n";
	my $result = <UNM> ;
	chop $result;

	my $pat = "\^\\s*$result\\s*\$";
	if ($str =~ $pat) {
	    return $str;
	}

	close UNM;
	#strip of any args
	if ($result =~ /([^\(]*)\s*\(/) {
		my $res = $1;
		if ($recordUnmangledSymbols) {
			my $l = $lineno;
			$mangledSymbols{$str} = 1;
			push @unmangledSymbols, "$incroot$sfile $l: Symbol = $str : Result = $res : Filt = $result : @IncFiles";
		}
		$result = $res;
	} else {
		# didn't have args so try as a (static) var
		my $res = "\&$result";
		if ($recordUnmangledSymbols) {
			my $l = $lineno;
			$mangledSymbols{$str} = 1;
			push @unmangledSymbols, "$incroot$sfile $l: Symbol = $str : Result = $res : Filt = $result : @IncFiles";
		}
		$result = $res;
	}		
	return $result;
}

sub EmitOriginal($$)
{
	my ($orig , $emitter) = @_;
	$orig =~ /\s*(.*)/;
	printf "\t$commentToken Original - $1\n" if ($printOriginal);
	printf "\t$commentToken emitted by $emitter\n" if ($recordEmitter);
}

sub EmitUnimplementedOpcode ($$$$)
{
    my ($original, $asm) = @_;
    if ($emitUnimplemented) {
		EmitOriginal ($original, "EmitUnimplementedOpcode");
		if ($asm =~ /(\S+)\s+/ or $asm =~ /\.*(\S+)/){
			my $opcode = uc $1;
			printf "\t$commentToken Translation of opcode $opcode not implemented\n";
			printf "\t**** Insert translation here ****\n";
		}
		else { 
			UnrecognisedAsmWarning("EmitUnimplementedOpcode", $original);
		}
    }
}

sub SimpleEmit ($$$$)
{
	my ($original, $str, $emitter, $comment) = @_;
	PrintCheck();
    EmitOriginal ($original, $emitter);
    printf "$str";
    PrintComment($comment);
    Nl();
}

sub UnrecognisedAsmWarning ($$)
{
    my ($where, $what) = @_;
    printf STDERR "WARNING: line $. unrecognised asm format in $where: $what";
}

sub Count ($$$$) {
	my ($str, $c, $start, $end) = @_;
	my $total = 0;
	my @a = split //, $str;
	for (;$start < $end; $start++) {
		$total++ if ($a[$start] eq $c);
	}
	return $total;
}

sub TranslateConstrainedArgs($$) {
	my ($args, $constraints) = @_;
	my $ins = GetInputConstraints($constraints) if ($constraints);
	my @arglist;
	my @rl;
	my $start = 0;
	my $end = length $args;
	my $cpos = index $args, ",", $start;
	if ($cpos > -1) {
		while ($cpos > -1) {
			#make sure we got a match number of '('s and ')' $start and $cpos
			my $nl = Count($args, '(', $start, $cpos);
			my $nr = Count($args, ')', $start, $cpos);
			if ($nl == $nr) {
				my $arg = substr($args, $start, $cpos - $start);
				push @arglist, $arg;
				$start = $cpos + 1;
				$cpos = index $args, ",", $start;
			} else {
				$cpos = index $args, ",", $cpos + 1;
			}
		}
		push @arglist, substr($args, $start, $end);

	} else {
		push @arglist, ChopWhiteSpace($args);
	}
	foreach (@arglist) {
		push @rl, SubstituteConstraint($_, $ins);
	}
	return join ", ", @rl;
}

sub GetInputConstraints($) {
	my ($cs) = @_;
	if ($cs =~ /\:\s+\:\s*(.*)/) {
		return join "", split '\"i\" ', $1;
	} else {
		Croak("unrecognized contraints format: $cs\n");
	}
}

sub ChopWhiteSpace ($) {
	my ($str) = @_;
	my @a = split //, $str;
	my $n = length($str);
	return $str if $n == 0;
	while (--$n) {
	    if ($a[$n] eq ' ') {
		next;
	    } else {
		last;
	    }
	}
	$n++ unless $a[$n] eq ' ';
	return substr $str, 0, $n;
}

sub SubstituteConstraint($$) {
    my ($arg, $cs) = @_;
    my $u;
	$arg = ChopWhiteSpace($arg);
	unless ($cs) {
		if ($arg =~ /\s*(.*)\s*$/ ) {
			$u = $1;
		} else {
			Croak("Arg not supplied in SubstituteConstraint\n");
		}
	} elsif ($arg =~ /\%\S+(\d+)/ ) {
		my $i = $1;
		my @c = split '\,', $cs;
        $u = $c[$i];
    } elsif ($arg =~ /\s*(.*)\s*$/ ) {
		$u = $1;
    } else {
		Croak("Arg not supplied in SubstituteConstraint\n");
	}
	my $metau = quotemeta "$u";
	if (NeedsImporting($u)) {
		print "\timport $u ";
		PrintComment("Added by Substitute Constraint");
		Nl();
		AssertSourceFile();
		return "$u";
	} elsif ($u =~ /\s*__cpp/) {
		return $u;
	} elsif (grep /^$metau/, @knownLabels) {
		return $u;
	} else {
		return "__cpp($u)";
	}
}

sub RegisterSymbol($)
{
	my ($sym) = @_;
	return 1 if ($sym =~ /^r1[0-5]\s*$/i);
	return 1 if ($sym =~ /^r[0-9]\s*$/i);
	return 1 if ($sym =~ /^lr\s*$/i);
	return 1 if ($sym =~ /^pc\s*$/i);
	return 1 if ($sym =~ /^ip\s*$/i);
	return 1 if ($sym =~ /^sp\s*$/i);
	return 0;
}	


sub NeedsImporting($)
{
	my ($sym) = @_;
	return 0 if ($sym =~ /\s*0x/i );
	return 0 if ($sym =~ /^\s*0\s*/ );
	return 0 if ($sym =~ /^\s*\d+\s*/ );
	return 0 if ($sym =~ /\s*\(/ );
	return 0 if ($sym =~ /\s*__cpp\(/ );
	return 0 if RegisterSymbol($sym);

	my $unms = Unmangle($sym);
	my $pat = quotemeta($unms);
	unless ($sym =~ /$pat/) {
		return 0;
	}
	if (($sym =~ /(\w*)/) && (grep /^$1/, @knownLabels) ) {
		return 0;
	} else {
		return 1;
	}
}
	
sub MaybeImportArgs($)
{
	my ($args) = @_;
	my $arg;
	foreach $arg (split /\,/, $args) {
		MaybeEmitImport($arg);
	}
}
sub GetInputConstraint($$$)
{
    # It would have been nice if we could have used split to get at the constraints
    # but we can't coz ':' can obviously appear as part of a qualified name. So we have to do it
    # by hand.

    my ($constraints, $index, $noError) = @_;
    # assume constraints look like " : output : input [: sideffects"]
    my $i1 = index($constraints, ":"); # output field after this index
    Croak("unrecognized contraints format: $constraints\n") if (!$noError and $i1 < 0);
    my $i2 = index($constraints, ":", $i1 + 1); # input field after this index
    Croak("unrecognized contraints format: $constraints\n") if !$noError and $i2 < 0;

    Croak("can't deal with output constraints: $constraints\n") 
		if !$noError and (substr($constraints, $i1 + 1, $i2 - $i1 - 1) =~ /\S+/);

    Croak("can't deal with side effect constraints: $constraints\n") 
		if (substr($constraints, $i2 + 1) =~ /(\s*\".+\".*\(.*\))\s*\:+/);

    if ($i2 > 0 
		and (length($constraints) - 1) > $i2 
		and substr($constraints, $i2 + 1) =~ /(\s*\".+\".*\(.*\S+.*\))\s*\:*/) {
        return $1;
    } else {
		return 0;
    }
}

sub GetOutputConstraint($$)
{
    # It would have been nice if we could have used split to get at the constraints
    # but we can't coz ':' can obviously appear as part of a qualified name. So we have to do it
    # by hand.
    my ($constraints, $index) = @_;
    # assume constraints look like " : output : input [: sideffects"]
    my $i1 = index($constraints, ":"); # output field after this index
    my $i2 = index($constraints, ":", $i1 + 1); # output field after this index

	if ($i2 != -1) {
		if ( substr($constraints, $i1 + 1, $i2 - $i1 - 1) =~ /\s*(\".*\"\s*\S*.*\))\s*\:*/) {
			return $1;
		} else {
			return 0;
		}
	} elsif ( substr($constraints, $i1 + 1) =~ /\s*(\".*\"\s*\S*.*\))\s*\:*/) {
		return $1;
    } else {
		return 0;
    }
}

# NB: assumes no mangled symbols in constraint expr.
sub CppExprFromConstraint ($)
{
    my ($constraints) = @_;
	return $constraints if ($constraints =~ /\s*__cpp/);
    my $inputExpr;
    if ($constraints =~ /\s*\".*\"\s+(.*)/) {
		$inputExpr = $1;
    } else {
		Croak( "Unrecognized constraint pattern @ $lineno: $constraints");
    }

    unless ($inputExpr =~ /^\(/) {
		$inputExpr = "($inputExpr)";
    }

    my $result = "__cpp$inputExpr";

    return $result;
}

sub TranslateConstrainedInputAsmDefault ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;

    # we make some gross assumptions here which appear to hold for the majority of 
    # our code base namely:
    # 1. there is normally only one input operand and 
    # 2. it is named 'a0'
    # This allows us to carry out the simple minded substitution seen below.
    my $cppExpr0 = CppExprFromConstraint(GetInputConstraint($constraints, 0, 0));

    $asm =~ s/\%a0/$cppExpr0/;
    if ($asm =~ /(\w+)\s+(\S+)\s*\,\s*(.+)\s*,?(.+)?/) {
	PrintCheck();
	EmitOriginal($original, "TranslateConstrainedInputAsmDefault");
	EmitAsm($1, $2, $3, $4, $comment);
    } else {
	UnrecognisedAsmWarning("TranslateConstrainedInputAsmDefault", $original);
    }
}

sub TranslateAsmDefault ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
    if ($constraints) {
		TranslateConstrainedInputAsmDefault($original, $asm, $constraints, $comment);
    } elsif (@_[1] =~ /(\w+)\s+([^\,]+)\s*\,\s*([^\,]+)\s*,?(.+)?/) {
		my $opcode = uc $1;
		my $op1 = $2;
		my $op2 = $3;
		my $op3 = $4;
		# deal with hand introduced labels that correspond to a mangled C++ name
		if ($op2 =~ /\s*\[[^\,]+\,\s*\#([^\-]+)/) {
			my $adr = $1;
			my $pattern = quotemeta($adr);
			my $unmangledAdr = Unmangle($adr);
			$op2 =~ s/$adr/$unmangledAdr/i unless $unmangledAdr =~ /$pattern/;
		}
		if ($opcode =~ /ldr/i) {
			if ($op2 =~ /^(\d+)([fFbB])/) {
				my $id = $1;
				my $dir = uc $2;
				$op2 = "%$dir$id";
			} 
		}
		# rename obsolete shift ASL -> LSL
		if ($op3 =~ /([^\,]*)\,\s*asl (.*)/i) {
			$op3 = "$1, lsl $2";
		}
		# deal with the likes of #___2PP.KernCSLocked-.-8
		if ($op3 =~ /([^\-\s]\.[^\s\-])/ ) {
			my $s = "$1";
			my $p = quotemeta($1);
			$s =~ s/\./\_/;
			$op3 =~ s/$p/$s/;
		}
		PrintCheck();
		EmitOriginal ($original, "TranslateAsmDefault");
		EmitAsm($opcode, $op1, $op2, $op3, $comment);
    } else {
		UnrecognisedAsmWarning("TranslateAsmDefault", $original);
    }
}


# Work around 'feature' in embedded assembler stemming from the fact that
# 'and' is both asm and a C++ keyword.
sub TranslateConstrainedAnd ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;

    # we make some gross assumptions here which appear to hold for the majority of 
    # our code base namely:
    # 1. there is normally only one input operand and 
    # 2. it is named 'a0'
    # This allows us to carry out the simple minded substitution seen below.
    my $cppExpr0 = CppExprFromConstraint(GetInputConstraint($constraints, 0, 0));

	$asm =~ s/\%a0/$cppExpr0/;
    if ($asm =~ /(\w+)\s+(.+)\s*\,\s*(.+)\s*,?(.+)?/) {
	my $opcode = uc $1;
	my $op1 = $2;
	my $op2 = $3;
	my $op3 = $4;
		# rename obsolete shift ASL -> LSL
		if ($op3 =~ /([^\,]*)\,\s*asl (.*)/) {
			$op3 = "$1, lsl $2";
		}
		PrintCheck();
		EmitOriginal ($original, "TranslateConstrainedAnd");

		printf "\t$opcode $op1, $op2";
		printf ", $op3" if $op3;
		PrintComment($comment);
		Nl();
    } else {
	UnrecognisedAsmWarning("TranslateConstrainedAnd", $original);
    }
}

sub TranslateAnd ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
    if ($constraints) {
		TranslateConstrainedAnd($original, $asm, $constraints, $comment);
    } elsif (@_[1] =~ /(\w+)\s+([^\,]+)\s*\,\s*([^\,]+)\s*,?(.+)?/) {
		my $opcode = uc $1;
		my $op1 = $2;
		my $op2 = $3;
		my $op3 = $4;
		# rename obsolete shift ASL -> LSL
		if ($op3 =~ /([^\,]*)\,\s*asl (.*)/) {
			$op3 = "$1, lsl $2";
		}
		PrintCheck();
		EmitOriginal ($original, "TranslateAnd");

		printf "\t$opcode $op1, $op2";
		printf ", $op3" if $op3;
		PrintComment($comment);
		Nl();
    } else {
		UnrecognisedAsmWarning("TranslateAnd", $original);
    }
}


# based on TranslateConstrainedInputAsmDefault
sub TranslateConstrainedCoprocessorInsn ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;

    # we make some gross assumptions here which appear to hold for the majority of 
    # our code base namely:
    # 1. there is normally only one input operand and 
    # 2. it is named 'a0'
    # This allows us to carry out the simple minded substitution seen below.
    my $cppExpr0 = CppExprFromConstraint(GetInputConstraint($constraints, 0, 0));

	$asm =~ s/\%a0/$cppExpr0/;
    if ($asm =~ /(\w+)\s+(.+)\s*\,\s*(.+)\s*,?(.+)?/) {
	my $opcode = $1;
	my $coproc = lc $2;
	my $op1 = $3;
	my $op2 = $4;
	$coproc = "p$coproc" unless $coproc =~ /^p.+/;
	PrintCheck();
	EmitOriginal($original, "TranslateConstrainedCoprocessorInsn");
	EmitAsm($opcode, $coproc, $op1, $op2, $comment);
    } else {
	UnrecognisedAsmWarning("TranslateConstrainedCoprocessorInsn", $original);
    }
}

# based on TranslateAsmDefault
sub TranslateCoprocessorInsn ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
    if ($constraints) {
	TranslateConstrainedCoprocessorInsn($original, $asm, $constraints, $comment);
    } elsif (@_[1] =~ /(\w+)\s+(.+)\s*\,\s*(.+)\s*,?(.+)?/) {
	my $opcode = $1;
	my $coproc = lc $2;
	my $op1 = $3;
	my $op2 = $4;
	$coproc = "p$coproc" unless $coproc =~ /^p.+/;
	PrintCheck();
	EmitOriginal ($original, "TranslateCoprocessorInsn");
	EmitAsm($opcode, $coproc, $op1, $op2, $comment);
    } else {
	UnrecognisedAsmWarning("TranslateCoprocessorInsn", $original);
    }
}

sub TranslateConstrainedSWI ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;

    # we make some gross assumptions here which appear to hold for the majority of 
    # our code base namely:
    # 1. there is normally only one input operand and 
    # 2. it is named 'a0'
    # This allows us to carry out the simple minded substitution seen below.
    my $cppExpr0 = CppExprFromConstraint(GetInputConstraint($constraints, 0, 0));

	$asm =~ s/\%a0/$cppExpr0/;
    if ($asm =~ /(\w+)\s+(.+)/) {
	my $opcode = $1;
	my $op1 = $2;
	PrintCheck();
	EmitOriginal($original, "TranslateConstrainedSWI");
	$opcode = RequiredCase($opcode);
    printf "\t$opcode $op1";
    PrintComment($comment);
    Nl();
    } else {
	UnrecognisedAsmWarning("TranslateConstrainedSWI", $original);
    }
}

sub TranslateSWI ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
    if ($constraints) {
		TranslateConstrainedSWI($original, $asm, $constraints, $comment);
    } elsif (@_[1] =~ /(\w+)\s+(.+)/) {
	my $opcode = $1;
	my $op1 = $2;
	PrintCheck();
	EmitOriginal ($original, "TranslateSWI");
	$opcode = RequiredCase($opcode);
    printf "\t$opcode $op1";
    PrintComment($comment);
    Nl();
    } else {
	UnrecognisedAsmWarning("TranslateSWI", $original);
    }
}


sub TranslateLabel ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
	if ( $asm =~ /\s*(\S+)\:/) {
		my $label = $1;
		$label = Unmangle($label) unless ($label =~ /^(\d+)/);
		SimpleEmit($original, $label, "TranslateLabel", $comment);
    } else { 
		UnrecognisedAsmWarning("TranslateLabel", $original);
    }
}

sub TranslateConstrainedAdr ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;

    # we make some gross assumptions here which appear to hold for the majority of 
    # our code base namely:
    # 1. there is normally only one input operand and 
    # 2. it is named 'a0'
    # This allows us to carry out the simple minded substitution seen below.
    my $cppExpr0 = CppExprFromConstraint(GetInputConstraint($constraints, 0, 0));

    $asm =~ s/\%a0/$cppExpr0/;
    if ($asm =~ /(\w+)\s+(\S+)\s*\,\s*(.+)\s*,?(.+)?/) {
	PrintCheck();
	EmitOriginal($original, "TranslateConstrainedAdr");
	EmitAsm($1, $2, $3, $4, $comment);
    } else {
	UnrecognisedAsmWarning("TranslateConstrainedAdr", $original);
    }
}

sub TranslateAdr ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
    if ($constraints) {
		TranslateConstrainedAdr($original, $asm, $constraints, $comment);
    } elsif (@_[1] =~ /(\w+)\s+([^\,]+)\s*\,\s*([^\,]+)/) {
		my $opcode = uc $1;
		my $op1 = $2;
		my $eadr = $3;
		my $op3;
		
		if ($eadr =~ /^(\d+)([fFbB])/) {
				my $id = $1;
				my $dir = uc $2;
				$eadr = "%$dir$id";
		} else {
			my $unmangledEadr = Unmangle($eadr);
			my $pattern = quotemeta($eadr);
			$eadr = "__cpp($unmangledEadr)" unless $unmangledEadr =~ /$pattern/;
		}
		MaybeEmitImport($eadr);
		PrintCheck();
		EmitOriginal ($original, "TranslateAdr");
		EmitAsm($opcode, $op1, $eadr, $op3, $comment);
    } else {
		UnrecognisedAsmWarning("TranslateAdr", $original);
    }
}

sub RequiredCase($)
{
	my ($s) = @_;
	lc $s;
}

sub TranslateAlign ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
    if ( $asm =~ /\s*\.align\s+(\S+)/i) {
		my $alignment = $1;
		my $boundary = "1:SHL:$1";
#		my $boundary = "";
		my $boundary = "" if ($alignment =~ /\s*0\s*/);
		
		my $directive = RequiredCase("ALIGN");
		SimpleEmit($original, "\t$directive $boundary", "TranslateAlign", $comment);
    }
    else { 
		UnrecognisedAsmWarning("TranslateAlign", $original);
    }
}

sub TranslateSpace ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
    if ( $asm =~ /\s*\.space\s+(\S+)/i) {
		my $directive = RequiredCase("SPACE");
		SimpleEmit($original, "\t$directive $1", "TranslateSpace", $comment);
    }
    else { 
		UnrecognisedAsmWarning("TranslateSpace", $original);
    }
}

sub TranslateByte ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
	my $directive = RequiredCase("DCB");
	$asm =~ /\s*.byte\s+(.*)/i;
	my $args = $1;
	if ($constraints) {
		$args = TranslateConstrainedArgs($args, $constraints);
		SimpleEmit($original, "\t$directive $args", "TranslateByte", $comment);
	} else {
		MaybeImportArgs($args);
		SimpleEmit($original, "\t$directive $args", "TranslateByte", $comment);
	}
}

sub CppExprList($)
{
    my ($arg) = @_;
	return $arg if ($arg =~ /\s*__cpp/);

    if ($arg =~ /(.*)\,\s*([^\s]*)/) {
		my $result = CppExprList($1);
		my $expr = $2;
		if ($expr =~ /\s*0x\d+/) {
			return $result .= ", __cpp($expr)";
		} elsif ($expr =~ /\s*(\d+)/) {
			my $hex = sprintf("%#.8x", $1);
			return $result .= ", __cpp($hex)";
		} else {
			my $pattern = quotemeta($expr);
			my $unmangledExpr = Unmangle($expr);
			return ($unmangledExpr =~ /$pattern/) ? $expr : "__cpp(\&$unmangledExpr)";
		}
    } else {
		if ($arg =~ /\s*0x\d+/) {
			return " __cpp($arg)";
		} elsif ($arg =~ /\s*(\d+)/) {
			my $hex = sprintf("%#.8x", $1);
			return " __cpp($hex)";
		} else {
			if ($arg =~ /\s*([^\s]*)/) {
				$arg = $1;
				my $pattern = quotemeta($arg);
				my $unmangledArg = Unmangle($arg);
				return ($unmangledArg =~ /$pattern/) ? $arg : "__cpp(\&$unmangledArg)";
			}
		}
    }
}

# Add symbols here that aren't imported if they're 'special'.
my %recognizedSymbols = 
	(
	 Followers => 1,
	 TheScheduler => 1,
	 TheMonitor => 1,
	 MonitorStack => 1,
	 ServerAccept => 1,
	 ServerReceive => 1,
	 wordmove => 1,
	 memcpy => 1,
	 memcompare => 1,
	 memclr => 1,
	 memset => 1,
	 memmove => 1,
	 );

sub EmitRecognizedSymbol ($$$$)
{
	my ($original, $directive, $sym, $comment) = @_;
	return 0 if ($sym =~ /\s*0x/i );
	return 0 if ($sym =~ /^\s*0\s*/ );
	return 0 if ($sym =~ /^\s*\d+\s*/ );
	return 0 if ($sym =~ /\s*__cpp\(/ );
	my $unms = Unmangle($sym);
	my $pat = quotemeta($unms);
	unless ($sym =~ /$pat/) {
		SimpleEmit($original, "\t$directive __cpp($unms)", "TranslateWord", $comment);
		return 1;
	}
	if (($sym =~ /(\S*)/) && !(grep /^$1$/, @knownLabels) ) {
		SimpleEmit($original, "\timport $sym", "TranslateWord", "// added by Tranasm");
		AssertSourceFile();
		SimpleEmit($original, "\t$directive $sym", "TranslateWord", $comment);
		return 1;
	} else {
		return 0;
	}
}

sub TranslateWord ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
	my $directive = RequiredCase("DCD");
	$asm =~ /\s*.word\s+(.*)/i;
	my $args = $1;
	if ($constraints) {
		$args = TranslateConstrainedArgs($args, $constraints);
		SimpleEmit($original, "\t$directive $args", "TranslateWord", $comment);
	} else {
		MaybeImportArgs($args);
		SimpleEmit($original, "\t$directive $args", "TranslateWord", $comment);
	}
}

sub TranslateCode ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
	my $directive = RequiredCase("CODE");
    if ( $asm =~ /\s*\.code\s+(\d+)\s*/i) {
		SimpleEmit($original, "\t$directive$1", "TranslateCode", $comment);
    }
    else { 
		UnrecognisedAsmWarning("TranslateCode", $original);
    }
}

sub TranslateGlobal ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
	my $directive = RequiredCase("EXPORT");
    if ( $asm =~ /\s*\.global\s+(\S+)/i) {
		SimpleEmit($original, "\t$directive $1", "TranslateGlobal", $comment);
    }
    else { 
		UnrecognisedAsmWarning("TranslateGlobal", $original);
    }
}

sub TranslateExtern ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
	my $directive = RequiredCase("IMPORT");
    if ( $asm =~ /\s*\.extern\s+(\S+)/i) {
		SimpleEmit($original, "\t$directive $1", "TranslateExtern", $comment);
    }
    else { 
		UnrecognisedAsmWarning("TranslateExtern", $original);
    }
}

sub TranslateNop ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;
	my $directive = RequiredCase("NOP");
    SimpleEmit($original, "\t$directive", "TranslateNop", $comment);
}

sub EmitAsm($$$$$)
{
	my ($opcode, $op1, $op2, $op3, $comment) = @_;
	$opcode = RequiredCase($opcode);
    printf "\t%s %s, %s", $opcode, $op1, $op2;
    printf(", %s", $op3) if $op3;
    PrintComment($comment);
    Nl();
}

sub TranslateBranchDefault ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;    
	if ($constraints) {
		Croak( "TranslateBranchDefault can't deal with Constraint instructions\n E.G. - $original");
	} elsif ($asm =~ /(\w+)\s+(.+)\s*$/) {
		my $opcode = RequiredCase($1);
		my $target = $2;

		if ($target =~ /^(\d+)([fFbB])/) {
			my $id = $1;
			my $dir = uc $2;
			$target = "%$dir$id";
		} else {
			my $unmangledTarget = Unmangle($target);
			my $pattern = quotemeta($target);
			$target = "__cpp($unmangledTarget)" unless $unmangledTarget =~ /$pattern/;
		}
		EmitOriginal ($original, "TranslateBranchDefault");
		MaybeEmitImport($target);
		printf("\t%s %s", $opcode, $target);
		PrintComment($comment);
		Nl();
    } else {
		UnrecognisedAsmWarning("TranslateBranchDefault", $original);
    }
}

sub TranslatePushPop ($$$$)
{
    my ($original, $asm, $constraints, $comment) = @_;    
    if ($asm =~ /(\w+)\s+(.+)\s*$/) {
		my $opcode = RequiredCase($1);
		my $registers = $2;

		if ($constraints) {
			Croak( "TranslatePushPop can't deal with constrained instructions\n E.G. - $original\n");
		} else {
			EmitOriginal ($original, "TranslatePushPop");
			printf "\t$opcode $registers";
			PrintComment($comment);
			Nl();
		}
    }
    else {
		UnrecognisedAsmWarning("TranslatePushPop", $comment);
    }
}

sub TranslateConstrainedAsmDefault ($$$$)
{
    # Here we assume:
    # 1. at most one output constraint
    # 2. the output constraint is named '%0'
    # 3. at most one input constraint
    # 4. the input constraint is named '%a0'

    my ($original, $asm, $constraints, $comment) = @_;    
	my $outputConstraint;
    my $inputConstraint;
    if ($outputConstraint = GetOutputConstraint($constraints, 0)) {
		my $outputCppExpr = CppExprFromConstraint($outputConstraint);
		$asm =~ s/\%0/$outputCppExpr/;
    }
    if ($inputConstraint = GetInputConstraint($constraints, 0, 1)) {
		my $inputCppExpr = CppExprFromConstraint($inputConstraint);
		$asm =~ s/\%a0/$inputCppExpr/;
    }
    if ($asm =~ /^\s*(\w+)\s+([^\,]+)\s*\,\s*(.+)\s*,?(.+)?/) {
		my $opcode = uc $1;
		my $op1 = $2;
		my $op2 = $3;
		my $op3 = $4;
		if ($outputConstraint) {
			printf "\t>>> CHECK THIS - output constraints need special attention <<<\n";
		} else {
			PrintCheck();
		}
		EmitOriginal($original, "TranslateConstrainedAsmDefault");
		$op1 =~ s/cpsr_flg/cpsr_f/i;
		$op2 =~ s/asl /lsl /i if $op2;
		$op3 =~ s/asl /lsl /i if $op3;
		EmitAsm($opcode, $op1, $op2, $op3, $comment);
    } else {
		UnrecognisedAsmWarning("TranslateConstrainedAsmDefault", $original);
    }
}

sub TranslatePotentialOutputConstrainedAsm ($$$$)
{
	
    my ($original, $asm, $constraints, $comment) = @_;

    if ($constraints) {
		TranslateConstrainedAsmDefault($original, $asm, $constraints, $comment);
    } elsif ($asm =~ /(\w+)\s+([^\,]+)\s*\,\s*(.+)\s*,?(.+)?/) {
		my $opcode = uc $1;
		my $op1 = $2;
		my $op2 = $3;
		my $op3 = $4;
		PrintCheck();
		EmitOriginal ($original, "TranslatePotentialOutputConstrainedAsm");

		# MSR cpsr,...
		$op1 .= "_cxsf" if ($opcode =~ /msr/i and $op1 =~ /[cs]psr\s*$/);
		$op1 =~ s/cpsr_flg/cpsr_f/i;
		$op2 =~ s/asl /lsl /i if $op2;
		$op3 =~ s/asl /lsl /i if $op3;
		EmitAsm($opcode, $op1, $op2, $op3, $comment);
    } else {
		UnrecognisedAsmWarning("TranslatePotentialOutputConstrainedAsm", $original);
    }
}


# Here's the table of translator functions
my %opcodeTranslatorMapping = 
	(
	 "LABEL:"=>\&TranslateLabel,

	 ".ALIGN"=>\&TranslateAlign,
	 ".BSS"=>\&EmitUnimplementedOpcode,
	 ".BYTE"=>\&TranslateByte,
	 ".CODE"=>\&TranslateCode,
	 ".DATA"=>\&EmitUnimplementedOpcode,
	 ".GLOBAL"=>\&TranslateGlobal,
	 ".EXTERN"=>\&TranslateExtern,
	 ".HWORD"=>\&EmitUnimplementedOpcode,
	 ".LONG"=>\&EmitUnimplementedOpcode,
	 ".SECTION"=>\&EmitUnimplementedOpcode,
	 ".SPACE"=>\&TranslateSpace,
	 ".TEXT"=>\&EmitUnimplementedOpcode,
	 ".WORD"=>\&TranslateWord,
	 "ADC"=>\&TranslateAsmDefault,
	 "ADD"=>\&TranslateAsmDefault,
	 "ADR"=>\&TranslateAdr,
	 "AND"=>\&TranslateAnd,

	 "B"=>\&TranslateBranchDefault,
	 "BEQ"=>\&TranslateBranchDefault,
	 "BNE"=>\&TranslateBranchDefault,
	 "BHS"=>\&TranslateBranchDefault,
	 "BCS"=>\&TranslateBranchDefault,
	 "BCC"=>\&TranslateBranchDefault,
	 "BLO"=>\&TranslateBranchDefault,
	 "BMI"=>\&TranslateBranchDefault,
	 "BPL"=>\&TranslateBranchDefault,
	 "BVS"=>\&TranslateBranchDefault,
	 "BVC"=>\&TranslateBranchDefault,
	 "BHI"=>\&TranslateBranchDefault,
	 "BLS"=>\&TranslateBranchDefault,
	 "BGE"=>\&TranslateBranchDefault,
	 "BLT"=>\&TranslateBranchDefault,
	 "BGT"=>\&TranslateBranchDefault,
	 "BLE"=>\&TranslateBranchDefault,
	 "BCLR"=>\&TranslateBranchDefault,
	 "BIC"=>\&TranslateAsmDefault,
	 "BKPT"=>\&TranslateBranchDefault, # not really a branch but can reuse translator

	 "BL"=>\&TranslateBranchDefault,
	 "BLEQ"=>\&TranslateBranchDefault,
	 "BLNE"=>\&TranslateBranchDefault,
	 "BLHS"=>\&TranslateBranchDefault,
	 "BLCS"=>\&TranslateBranchDefault,
	 "BLCC"=>\&TranslateBranchDefault,
	 "BLLO"=>\&TranslateBranchDefault,
	 "BLMI"=>\&TranslateBranchDefault,
	 "BLPL"=>\&TranslateBranchDefault,
	 "BLVS"=>\&TranslateBranchDefault,
	 "BLVC"=>\&TranslateBranchDefault,
	 "BLHI"=>\&TranslateBranchDefault,
	 "BLLS"=>\&TranslateBranchDefault,
	 "BLGE"=>\&TranslateBranchDefault,
	 "BLLT"=>\&TranslateBranchDefault,
	 "BLGT"=>\&TranslateBranchDefault,
	 "BLLE"=>\&TranslateBranchDefault,

	 "BLX"=>\&TranslateBranchDefault,
	 "BLXEQ"=>\&TranslateBranchDefault,
	 "BLXNE"=>\&TranslateBranchDefault,
	 "BLXHS"=>\&TranslateBranchDefault,
	 "BLXCS"=>\&TranslateBranchDefault,
	 "BLXCC"=>\&TranslateBranchDefault,
	 "BLXLO"=>\&TranslateBranchDefault,
	 "BLXMI"=>\&TranslateBranchDefault,
	 "BLXPL"=>\&TranslateBranchDefault,
	 "BLXVS"=>\&TranslateBranchDefault,
	 "BLXVC"=>\&TranslateBranchDefault,
	 "BLXHI"=>\&TranslateBranchDefault,
	 "BLXLS"=>\&TranslateBranchDefault,
	 "BLXGE"=>\&TranslateBranchDefault,
	 "BLXLT"=>\&TranslateBranchDefault,
	 "BLXGT"=>\&TranslateBranchDefault,
	 "BLXLE"=>\&TranslateBranchDefault,

	 "BSET"=>\&EmitUnimplementedOpcode,

	 "BX"=>\&TranslateBranchDefault,
	 "BXEQ"=>\&TranslateBranchDefault,
	 "BXNE"=>\&TranslateBranchDefault,
	 "BXHS"=>\&TranslateBranchDefault,
	 "BXCS"=>\&TranslateBranchDefault,
	 "BXCC"=>\&TranslateBranchDefault,
	 "BXLO"=>\&TranslateBranchDefault,
	 "BXMI"=>\&TranslateBranchDefault,
	 "BXPL"=>\&TranslateBranchDefault,
	 "BXVS"=>\&TranslateBranchDefault,
	 "BXVC"=>\&TranslateBranchDefault,
	 "BXHI"=>\&TranslateBranchDefault,
	 "BXLS"=>\&TranslateBranchDefault,
	 "BXGE"=>\&TranslateBranchDefault,
	 "BXLT"=>\&TranslateBranchDefault,
	 "BXGT"=>\&TranslateBranchDefault,
	 "BXLE"=>\&TranslateBranchDefault,
	 "CDP"=>\&TranslateAsmDefault,
	 "CLZ"=>\&TranslateAsmDefault,
	 "CMN"=>\&TranslateAsmDefault,
	 "CMP"=>\&TranslateAsmDefault,
	 "EOR"=>\&TranslateAsmDefault,
	 "LDC"=>\&TranslateAsmDefault,
	 "LDM"=>\&TranslateAsmDefault,
	 "LDR"=>\&TranslateAsmDefault,
	 "LDRB"=>\&TranslateAsmDefault,
	 "LSL"=>\&EmitUnimplementedOpcode,
	 "LSR"=>\&EmitUnimplementedOpcode,
	 "MCR"=>\&TranslateCoprocessorInsn,
	 "MLA"=>\&TranslateAsmDefault,
	 "MOV"=>\&TranslatePotentialOutputConstrainedAsm,
	 "MRC"=>\&TranslateCoprocessorInsn,
	 "MRS"=>\&TranslatePotentialOutputConstrainedAsm,
	 "MSR"=>\&TranslatePotentialOutputConstrainedAsm,
	 "MUL"=>\&TranslateAsmDefault,
	 "MVN"=>\&TranslateAsmDefault,
	 "NOP"=>\&TranslateNop,
	 "ORR"=>\&TranslateAsmDefault,
	 "POP"=>\&TranslatePushPop,
	 "PUSH"=>\&TranslatePushPop,
	 "RSB"=>\&TranslateAsmDefault,
	 "RSC"=>\&TranslateAsmDefault,
	 "SBC"=>\&TranslateAsmDefault,
	 "SMLAL"=>\&TranslateAsmDefault,
	 "STC"=>\&TranslateAsmDefault,
	 "STM"=>\&TranslateAsmDefault,
	 "STR"=>\&TranslateAsmDefault,
	 "SUB"=>\&TranslateAsmDefault,
	 "SWI"=>\&TranslateSWI,
	 "SWP"=>\&TranslateAsmDefault,
	 "TEQ"=>\&TranslateAsmDefault,
	 "TST"=>\&TranslateAsmDefault,
	 "UMLAL"=>\&TranslateAsmDefault,
	 "UMULL"=>\&TranslateAsmDefault,
	 "UMULLEQ"=>\&TranslateAsmDefault,
	 "UMULLNE"=>\&TranslateAsmDefault,
	 "UMULLCS"=>\&TranslateAsmDefault,
	 "UMULLCC"=>\&TranslateAsmDefault,
	 "UMULLHS"=>\&TranslateAsmDefault,
	 "UMULLLO"=>\&TranslateAsmDefault,
	 "UMULLMI"=>\&TranslateAsmDefault,
	 "UMULLPL"=>\&TranslateAsmDefault,
	 "UMULLVS"=>\&TranslateAsmDefault,
	 "UMULLVC"=>\&TranslateAsmDefault,
	 "UMULLHI"=>\&TranslateAsmDefault,
	 "UMULLLS"=>\&TranslateAsmDefault,
	 "UMULLGE"=>\&TranslateAsmDefault,
	 "UMULLLT"=>\&TranslateAsmDefault,
	 "UMULLGT"=>\&TranslateAsmDefault,
	 "UMULLLE"=>\&TranslateAsmDefault,
	 );

my @unknownOpcodes;

sub GetTranslator ($)
{
    my $opcode = shift;

    # see if opcode looks like a label
    return $opcodeTranslatorMapping{"LABEL:"} if ($opcode =~ /\w+\:$/);

    # just look it up
    my $translator = $opcodeTranslatorMapping{$opcode};
    return $translator if $translator;

    # see if we know the 'root' of the opcode
    return $opcodeTranslatorMapping{substr($opcode, 0, 3)};
}


my %seenIncFiles = ();

sub trackSourceLine($)
{
	my ($line) = @_;
	if ($line =~ /\#line (\d+)\s*(.*)$/ ) {
		$lineno = $1-1;
		$sourcefile = $2;
		if ($sourcefile =~ /.*\.h/i) {
			unless ($seenIncFiles{$sourcefile}) {
				$seenIncFiles{$sourcefile} = 1;
				my $incfile = "$sourcefile";
				$incfile =~ s/\"//go;
				$incfile =~ s/\\\\/${main::PATHSEP}/go;
				$incfile = "$incroot"."$incfile" unless ($incfile =~ /^${main::PATHSEP}/);
				push @IncFiles, $incfile;
			}
		}
	}
}

sub AssertSourceFile()
{
	printf "#line %d %s\n", $lineno, $sourcefile;
}

my @contents;

sub AddLabel($) {
	my ($label) = @_;
	if ($label =~ /\s*(\S+)\s*/ ) {
		$label = $1;
	}
	push @knownLabels, $label;
}

sub MaybeEmitImport ($) {
	my ($l) = @_;
	print "\timport $l\[DYNAMIC\]\n" if NeedsImporting($l);
}

sub Pass1()
{
	die "ERROR: Couldn't open $infile\n" unless open INP, "<$infile";
	my $line;
	MAINBLOCK: while ($line = <INP>) {
		# strip off comment if present
		my $statement;
		my $comment = 0;
		
		push @contents, $line;

		if ($line =~ /^\s*$/) {
			next MAINBLOCK;
		}
		if ($line =~ /(.*)\/\/(.+)/) {
			$statement = $1;
			$comment = $2;
		} else {
			$statement = $line;
		}

		if ($statement =~ /^((.*;\s*)|(\s*))asm\s*\(/) {
			foreach $statement ( split /\;/, $statement ) {
			  TRANSLATE_ASM:
				if ($statement =~ /^\s*asm\s*\(\s*\"(.*)\"\s*(:.*)*\)/) {
					my $asm = $1;
					my $constraints = $2;
					$asm =~ s/\"\s*\"//g;
					$asm =~ /\s*(\S+)/;
					my $opcode = $1;

					# if its a label record it
					if ($opcode =~ /(\w+)\:$/) {
						AddLabel($1);
					}
				} 
			}
		}
	}
	close INP;
}

sub CanonicalizeAsm($) {
    my ($s) = @_;
    if ($s =~ /(asm\([^\)]+\))\s*\;(.*)/o) {
	my $start = "$`";
	my $subst = $1;
	my $rem = $2;
	$subst =~ s/\;/ \"\)\; asm\(\"/g;
	return "$start"."$subst; ".CanonicalizeAsm($rem);
    } else {
	return $s;
    }
}

sub Pass2()
{
	$lineno = 0;

	my $startingBody = 0;
	my $line;
  MAINBLOCK: foreach $line ( @contents ) {
	  # strip off comment if present
	  my $statement;
	  my $comment = 0;

	  warn "$lineno\n" if $plineno;
	  $lineno++;
	  if ($line =~ /^\s*$/) {
		  print "$line";
		  next MAINBLOCK;
	  }
	  if ($line =~ /(.*)\/\/(.+)/) {
		  $statement = $1;
		  $comment = $2;
	  } else {
		  $statement = $line;
	  }

	  if ($statement =~ /^((.*;\s*)|(\s*))asm\s*\(/) {
		  # unfortunately we get things like:
		  # asm("mcr"#cc" p15, 0, "#r", c7, c5, 0; sub"#cc" pc, pc, #4 ");
		  # we need to turn this into asm("mcr"#cc" p15, 0, "#r", c7, c5, 0"); asm("sub"#cc" pc, pc, #4 ");
		  $statement = CanonicalizeAsm($statement);
		  foreach $statement ( split /\;/, $statement ) {
			TRANSLATE_ASM:
			  if ($statement =~ /^\s*asm\s*\(\s*\"(.*)\"\s*(:.*)*\)/) {
				  my $asm = $1;
				  my $constraints = $2;
				  $asm =~ s/\"\s*\"//g;
				  $asm =~ /\s*(\S+)/;
				  my $opcode = uc $1;

				  AssertSourceFile();
				  my $translator = GetTranslator($opcode);
				  if ($translator) {
					  $translator->($line, $asm, $constraints, $comment);
				  } else {
					  push @unknownOpcodes, $opcode ;
					  EmitUnimplementedOpcode($line, $asm, $constraints, $comment);
				  }
			  } elsif ($statement =~ /^\s*(__declspec.*\s* __asm .*\)\s*\{)(.*)$/) {
				  AssertSourceFile();
				  print "$1\n";
				  print "\tPRESERVE8\n\tCODE32\n";
				  $statement = $2;
				  goto TRANSLATE_ASM;
			  } elsif ($statement =~ /^\s*(__asm .*\)\s*\{)(.*)$/) {
				  AssertSourceFile();
				  print "$1\n";
				  print "\tPRESERVE8\n\tCODE32\n";
				  $statement = $2;
				  goto TRANSLATE_ASM;
			  } elsif (($statement =~ /^\s*.*\s+__asm [^\{]*$/) || ($statement =~ /^\s*__asm [^\{]*$/)) {
				  AssertSourceFile();
				  print "$statement";
				  $startingBody = 1;
			  } elsif ($startingBody && ($statement =~ /^\s*\{\s*$/) ) {
				  AssertSourceFile();
				  print "$statement";
				  print "\tPRESERVE8\n\tCODE32\n";
				  $startingBody = 0;
			  } elsif ($statement =~ /\s*(\S.*)$/) {
				  print "\t$1;\n";
			  }
		  }
	  } elsif (($statement =~ /^\s*.*\s+__asm [^\{]*$/) || ($statement =~ /^\s*__asm [^\{]*$/)) {
		  AssertSourceFile();
		  print "$statement";
		  $startingBody = 1;
	  } elsif ($startingBody && ($statement =~ /^\s*\{\s*$/) ) {
		  AssertSourceFile();
		  print "$statement";
		  print "\tPRESERVE8\n\tCODE32\n";
		  $startingBody = 0;
	  } else {
		  trackSourceLine($line);
		  print "$line";
	  }
  }
}

sub Main () {
	Pass1();
	Pass2();
}

Main();

if ($outfile) {
	select $savedOut;
	close OUT;
}

if (@unknownOpcodes > 0){
    printf STDERR "WARNING: The following opcodes were unrecognised:\n";
	my $op;
    foreach $op (sort @unknownOpcodes) { printf STDERR "\t$op\n";}
}


if ($recordUnmangledSymbols){
    open US, ">>$symbolsfile";
    foreach (@unmangledSymbols) { print US "$_ \n";}
    close US;
}


sub Usage
{
	print <<EOT;

tranasm

	Translate GCC inline assembler into ARM embedded assembler

Usage:
	tranasm [options] file

Where:
	[file]     The file to be translated.

Options:
	--record-emitter    each translation annotated with name of translation function
	--suppress-check    omit deliberate errors inserted to force human checking
	--no-original       do not emit the original gcc inline assembler as comment
	--error-string      the string to emit as the deliberate error
	--output            the name of the output file
	--help              this message

	Options may also be specified as a short abbreviation, ie -h or -o=foo.tr.
	The default deliberate error is indicated thus />>> CHECK THIS .*<<</.
EOT
	exit 1;
}

__END__
