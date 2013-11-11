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
# e32toolp\e32util\h2inc.pl
# Convert structures in C++ include files to assembler format
# Syntax:
# perl h2inc.pl <input.h> <output.inc> <format>
# where <format>=arm or x86
# 
#

%basictypes = (
	TInt8		=>	1,
	TUint8		=>	1,
	TInt16		=>	2,
	TUint16		=>	2,
	TInt32		=>	4,
	TUint32		=>	4,
	TInt		=>	4,
	TUint		=>	4,
	TInt64		=>	8,
	TUint64		=>	8,
	TLinAddr	=>	4,
	TVersion	=>	4,
	TPde		=>	4,
	TPte		=>	4,
	TProcessPriority => 4
);

if (scalar(@ARGV)!=3) {
	die "perl h2inc.pl <input.h> <output.inc> <format>\n";
}
my ($infile, $outfile, $format) = @ARGV;
open IN, $infile or die "Can't open $infile for input\n";
my $in;
while (<IN>) {
	$in.=$_;
}
close IN;
$format = uc($format);
$format_sub = undef();
$comment_sub = undef();
$end_sub = undef();
if ($format eq "ARMASM") {
	$format_sub = \&armasm_format;
	$comment_sub = \&armasm_comment;
	$end_sub = \&armasm_end;
} elsif ($format eq "AS") {
	$format_sub = \&as_format;
	$comment_sub = \&as_comment;
	$end_sub = \&as_end;
} elsif ($format eq "TASM") {
	$format_sub = \&tasm_format;
	$comment_sub = \&tasm_comment;
	$end_sub = \&tasm_end;
} else {
	die "Format $format unknown\nOnly ARMASM, AS or TASM supported\n";
}

# First remove any backslash-newline combinations
$in =~ s/\\\n//gms;

# Change escaped quotes to double quotes
$in =~ s/\\\"/\"\"/gms;
$in =~ s/\\\'/\'\'/gms;

# Remove any character constants
$in =~  s/\'(.?(${0})*?)\'//gms;

# Remove any string literals
$in =~ s/\"(.*?)\"//gms;

# Strip comments
$in =~ s/\/\*(.*?)\*\//\n/gms;
$in =~ s/\/\/(.*?)\n/\n/gms;

# Collapse whitespace into a single space or newline
$in =~ s/\t/\ /gms;
$in =~ s/\r/\ /gms;
$in =~ s/(\ )+/\ /gms;
$in =~ s/\n(\ )*/\n/gms;
$in =~ s/(\ )*\n/\n/gms;

# Tokenize on non-identifier characters
my @tokens0 = split(/(\W)/,$in);
my @tokens;
foreach $t (@tokens0) {
	next if ($t eq " " or $t eq "");
	push @tokens, $t;
}

my %macros;
my %filescope;
$filescope{file}=1;
$filescope{name}='*** FILE SCOPE ***';
my @ftypedefs;
$filescope{typedefs}=\@ftypedefs;
my $line=1;
parse_scope(\%filescope, \@tokens, \$line);


my @output;
push @output, &$comment_sub('*' x 80);
push @output, &$comment_sub($outfile);
push @output, &$comment_sub('*' x 80);
push @output, &$comment_sub("GENERATED FILE - DO NOT EDIT");
push @output, "";

output_scope(\%filescope, \@output);

push @output, &$end_sub();
push @output, "";

open OUT, ">$outfile" or die "Can't open $outfile for write\n";
print OUT join("\n", @output);
print OUT "\n\n";
close OUT;

sub get_token($$) {
	my ($tokenlist,$line) = @_;
	while (scalar(@$tokenlist)) {
		my $t = shift @$tokenlist;
		return $t if (!defined($t));
		return $t if ($t !~ /^\s*$/);
		++$$line;
	}
}

sub skip_qualifiers($) {
	my ($tokens) = @_;
	my $f=0;
	my %quals = (
		EXPORT_C => 1,
		IMPORT_C => 1,
		inline => 1,
		const => 0,
		volatile => 0,
		static => 0,
		extern => 0,
		LOCAL_C => 0,
		LOCAL_D => 0,
		GLDEF_C => 0,
		GLREF_C => 0,
		GLDEF_D => 0,
		GLREF_D => 0
		);
	for (;;) {
		my $t = $$tokens[0];
		my $q = $quals{$t};
		last unless (defined ($q));
		$f |= $q;
		shift @$tokens;
	}
	return $f;
}

sub parse_indirection($) {
	my ($tokens) = @_;
	my $level = 0;
	for (;;) {
		my $t = $$tokens[0];
		if ($t eq '*') {
			++$level;
			shift @$tokens;
			next;
		}
		last if ($t ne "const" and $t ne "volatile");
		shift @$tokens;
	}
	return $level;
}

sub parse_scope($$$) {
	my ($scope, $tokens, $line) = @_;
	my $state = 0;
	my %values;
	my @classes;
	my @enums;
	my $curr_offset=0;
	my $overall_align=0;
	$scope->{values}=\%values;
	$scope->{classes}=\@classes;
	$scope->{enums}=\@enums;
	while (scalar(@$tokens)) {
		my $t = shift @$tokens;
		if ($state>=-1 and $t eq "\n") {
			++$$line;
			$state=1;
			next;
		} elsif ($state==-1 and $t ne "\n") {
			next;
		} elsif ($state==-2 and $t ne ';') {
			next;
		}
		if ($state>0 and $t eq '#') {
			if ($scope->{scope}) {
				warn "Preprocessor directive in class/struct at line $$line\n";
			}
			$t = shift @$tokens;
			if ($t eq 'define') {
				my $ident = shift @$tokens;
				my $defn = shift @$tokens;
				if ($defn ne '(') {	# don't do macros with parameters
					$macros{$ident} = $defn;
				}
			}
			$state=-1;	# skip to next line
			next;
		}
		if ($t eq "struct" or $t eq "class") {
			next if ($state==0);
			$state=0;
			my %cl;
			$cl{specifier}=$t;
			$cl{scope}=$scope;
			my @members;
			my @typedefs;
			$cl{members}=\@members;
			$cl{typedefs}=\@typedefs;
			my $new_class = \%cl;
			my $n = get_token($tokens,$line);
			if ($n !~ /\w+/) {
				die "Unnamed $t not supported at line $$line\n";
			}
			$new_class->{name}=$n;
			my @class_match = grep {$_->{name} eq $n} @classes;
			my $exists = scalar(@class_match);
			my $b = get_token($tokens,$line);
			if ($b eq ':') {
				die "Inheritance not supported at line $$line\n";
			} elsif ($b eq ';') {
				# forward declaration
				push @classes, $new_class unless ($exists);
				next;
			} elsif ($b ne '{') {
				die "Syntax error at line $$line\n";
			}
			if ($exists) {
				$new_class = $class_match[0];
				if ($new_class->{complete}) {
					die "Duplicate definition of $cl{specifier} $n\n";
				}
			}
			push @classes, $new_class unless ($exists);
			parse_scope($new_class, $tokens, $line);
			next;
		} elsif ($t eq "enum") {
			$state=0;
			my $n = get_token($tokens,$line);
			my $name="";
			if ($n =~ /\w+/) {
				$name = $n;
				$n = get_token($tokens,$line);
			}
			push @enums, $name;
			if ($n ne '{') {
				die "Syntax error at line $$line\n";
			}
			parse_enum($scope, $tokens, $line, $name);
			next;
		} elsif ($t eq '}') {
			$state=0;
			if ($scope->{scope}) {
				$t = get_token($tokens,$line);
				if ($t eq ';') {
					$scope->{complete}=1;
					last;
				}
			}
			die "Syntax error at line $$line\n";
		}
		$state=0;
		if ($scope->{scope}) {
			if ($t eq "public" or $t eq "private" or $t eq "protected") {
				if (shift (@$tokens) eq ':') {
					next;	# ignore access specifiers
				}
			die "Syntax error at line $$line\n";
			}
		}
		unshift @$tokens, $t;
		my @currdecl = parse_decl_def($scope, $tokens, $line);
		if ($t eq 'static') {
			next;	# skip static members
		}
		my $typedef;
		if ($t eq 'typedef') {
			$typedef = 1;
			$t = shift @currdecl;
			$t = $currdecl[0];
		} else {
			$typedef = 0;
		}
		next if (scalar(@currdecl)==0);
		if ($t eq "const") {
			# check for constant declaration
			my $ctype = lookup_type($scope, $currdecl[1]);
			if ($ctype->{basic} and $currdecl[2]=~/^\w+$/ and $currdecl[3] eq '=') {
				if ($typedef!=0) {
					die "Syntax error at line $$line\n";
				}
				shift @currdecl;
				shift @currdecl;
				my $type = $ctype->{name};
				my $name = shift @currdecl;
				my $size = $ctype->{size};
				shift @currdecl;
				my $value = get_constant_expr($scope,\@currdecl,$line);
				$values{$name} = {type=>$type, size=>$size, value=>$value};
				next;
			}
		}
		if (skip_qualifiers(\@currdecl)!=0 or ($scope->{file} and !$typedef)) {
			next;	# function declaration or stuff at file scope
		}
		my $type1 = shift @currdecl;	# type, type pointed to or return type
		if ($type1 !~ /^\w+$/) {
			die "Syntax error at line $$line\n";
		}
		my $ind1 = parse_indirection(\@currdecl);
		my $ident;	# identifier being declared
		my $size = -1;
		my $array = -1;
		my $align = 0;
		my $alias;
		my $category;
		if ($currdecl[0] eq '(' and $currdecl[1] eq '*' and $currdecl[2]=~/^\w+$/) {
			# function pointer
			$ident = $currdecl[2];
			$size = 4;
			$category = 'fptr';
			shift @currdecl;
			shift @currdecl;
			shift @currdecl;
		} elsif ($currdecl[0]=~/^\w+$/) {
			$ident = shift @currdecl;
			if ($currdecl[0] ne '(') {
				# not function declaration
				if ($ind1>0) {
					# pointer
					$category = 'ptr';
					$size = 4;
				} else {
					my $type2 = lookup_type($scope, $type1);
					if (!defined($type2)) {
						die "Unrecognised type $type1 at line $$line\n";
					}
					if ($type2->{basic}) {
						$alias = $type2->{name};
						$size = $type2->{size};
						$category = 'basic';
					} elsif ($type2->{enum}) {
						$alias = $type2->{name};
						$category = 'enum';
						$size = 4;
					} elsif ($type2->{class}) {
						$alias = $type2->{name};
						$size = $type2->{class}->{size};
						$category = 'class';
						$align = $type2->{class}->{align};
					} elsif ($type->{ptr}) {
						$size = 4;
						$category = 'ptr';
						$align = 4;
					} elsif ($type->{fptr}) {
						$size = 4;
						$category = 'ptr';
						$align = 4;
					}
				}
			}
		}
		if ($size>0) {
			# data member declared
			# check for array
			if ($currdecl[0] eq '[') {
				shift @currdecl;
				$array = get_constant_expr($scope, \@currdecl, $line);
				if ($array<=0) {
					die "Bad array size at line $$line\n";
				}
				if ($currdecl[0] ne ']') {
					die "Syntax error at line $$line\n";
				}
			}
			my $members = $scope->{members};
			my $typedefs = $scope->{typedefs};
			if ($align==0) {
				$align = $size;
			}
			my $am = $align-1;
			unless ($typedef) {
				my $al = $curr_offset & $am;
				if ($align==8 and $al!=0) {
					die "Bad alignment of 64-bit data $ident at line $$line\n";
				}
				$curr_offset += ($align-$al) if ($al!=0);
			}
			if ($array>0) {
				$size = ($size + $am) &~ $am;
				if ($typedef) {
					push @$typedefs, {name=>$ident, category=>$category, alias=>$alias, size=>$size*$array, spacing=>$size, array=>$array};
				} else {
					push @$members, {name=>$ident, size=>$size*$array, offset=>$curr_offset, spacing=>$size};
				}
				$size *= $array;
			} else {
				if ($typedef) {
					push @$typedefs, {name=>$ident, category=>$category, alias=>$alias, size=>$size};
				} else {
					push @$members, {name=>$ident, size=>$size, offset=>$curr_offset};
				}
			}
			unless ($typedef) {
				$curr_offset += $size;
				if ($align > $overall_align) {
					$overall_align = $align;
				}
			}
		}
	}
	if ($scope->{scope}) {
		if ($state==-2) {
			die "Missing ; at end of file\n";
		}
		if (!$scope->{complete}) {
			die "Unexpected end of file at line $$line\n";
		}
		my $total_size = ($curr_offset + $overall_align - 1) &~ ($overall_align - 1);
		$scope->{size} = $total_size;
		$scope->{align} = $overall_align;
	}
}

sub get_operand($$$) {
	my ($scope,$tokens,$line) = @_;
	my $t = get_token($tokens,$line);
	if ($t eq '-') {
		my $x = get_operand($scope,$tokens,$line);
		return -$x;
	} elsif ($t eq '+') {
		my $x = get_operand($scope,$tokens,$line);
		return $x;
	} elsif ($t eq '~') {
		my $x = get_operand($scope,$tokens,$line);
		return ~$x;
	} elsif ($t eq '!') {
		my $x = get_operand($scope,$tokens,$line);
		return $x ? 0 : 1;
	} elsif ($t eq '(') {
		my $x = get_constant_expr($scope,$tokens,$line);
		my $t = get_token($tokens,$line);
		if ($t ne ')') {
			die "Missing ) at line $$line\n";
		}
		return $x;
	} elsif ($t eq "sizeof") {
		my $ident = get_token($tokens,$line);
		if ($ident eq '(') {
			$ident = get_token($tokens,$line);
			my $cb = get_token($tokens,$line);
			if ($cb ne ')') {
				die "Bad sizeof() syntax at line $$line\n";
			}
		}
		$ident = look_through_macros($ident);
		if ($ident !~ /^\w+$/) {
			die "Bad sizeof() syntax at line $$line\n";
		}
		my $type = lookup_type($scope, $ident);
		if (!defined $type) {
			die "Unrecognised type $ident at line $$line\n";
		}
		if ($type->{basic}) {
			return $type->{size};
		} elsif ($type->{enum}) {
			return 4;
		} elsif ($type->{ptr}) {
			return 4;
		} elsif ($type->{fptr}) {
			return 4;
		}
		my $al = $type->{class}->{align};
		my $sz = $type->{class}->{size};
		return ($sz+$al-1)&~($al-1);
	}
	$t = look_through_macros($t);
	if ($t =~ /^0x[0-9a-f]+/i) {
		return oct($t);
	} elsif ($t =~ /^\d/) {
		return $t;
	} elsif ($t =~ /^\w+$/) {
		my $x = lookup_value($scope,$t);
		die "Unrecognised identifier '$t' at line $$line\n" unless defined($x);
		return $x;
	} else {
		die "Syntax error at line $$line\n";
	}
}

sub look_through_macros($) {
	my ($ident) = @_;
	while ($ident and $macros{$ident}) {
		$ident = $macros{$ident};
	}
	return $ident;
}

sub lookup_value($$) {
	my ($scope,$ident) = @_;
	while ($scope) {
		my $vl = $scope->{values};
		if (defined($vl->{$ident})) {
			return $vl->{$ident}->{value};
		}
		$scope = $scope->{scope};
	}
	return undef();
}

sub lookup_type($$) {
	my ($scope,$ident) = @_;
	if ($basictypes{$ident}) {
		return {scope=>$scope, basic=>1, name=>$ident, size=>$basictypes{$ident} };
	}
	while ($scope) {
		if ($basictypes{$ident}) {
			return {scope=>$scope, basic=>1, name=>$ident, size=>$basictypes{$ident} };
		}
		my $el = $scope->{enums};
		my $cl = $scope->{classes};
		my $td = $scope->{typedefs};
		if (grep {$_ eq $ident} @$el) {
			return {scope=>$scope, enum=>1, name=>$ident, size=>4 };
		}
		my @match_class = (grep {$_->{name} eq $ident} @$cl);
		if (scalar(@match_class)) {
			return {scope=>$scope, class=>$match_class[0]};
		}
		my @match_td = (grep {$_->{name} eq $ident} @$td);
		if (scalar(@match_td)) {
			my $tdr = $match_td[0];
			my $cat = $tdr->{category};
			if ($cat eq 'basic' or $cat eq 'enum' or $cat eq 'class') {
				$ident = $tdr->{alias};
				next;
			} else {
				return { scope=>$scope, $cat=>1, $size=>$tdr->{size} };
			}
		}
		$scope = $scope->{scope};
	}
	return undef();
}

sub get_mult_expr($$$) {
	my ($scope,$tokens,$line) = @_;
	my $x = get_operand($scope,$tokens,$line);
	my $t;
	for (;;) {
		$t = get_token($tokens,$line);
		if ($t eq '*') {
			my $y = get_operand($scope,$tokens,$line);
			$x = $x * $y;
		} elsif ($t eq '/') {
			my $y = get_operand($scope,$tokens,$line);
			$x = int($x / $y);
		} elsif ($t eq '%') {
			my $y = get_operand($scope,$tokens,$line);
			$x = int($x % $y);
		} else {
			last;
		}
	}
	unshift @$tokens, $t;
	return $x;
}

sub get_add_expr($$$) {
	my ($scope,$tokens,$line) = @_;
	my $x = get_mult_expr($scope,$tokens,$line);
	my $t;
	for (;;) {
		$t = get_token($tokens,$line);
		if ($t eq '+') {
			my $y = get_mult_expr($scope,$tokens,$line);
			$x = $x + $y;
		} elsif ($t eq '-') {
			my $y = get_mult_expr($scope,$tokens,$line);
			$x = $x - $y;
		} else {
			last;
		}
	}
	unshift @$tokens, $t;
	return $x;
}

sub get_shift_expr($$$) {
	my ($scope,$tokens,$line) = @_;
	my $x = get_add_expr($scope,$tokens,$line);
	my $t, $t2;
	for (;;) {
		$t = get_token($tokens,$line);
		if ($t eq '<' or $t eq '>') {
			$t2 = get_token($tokens,$line);
			if ($t2 ne $t) {
				unshift @$tokens, $t2;
				last;
			}
		}
		if ($t eq '<') {
			my $y = get_add_expr($scope,$tokens,$line);
			$x = $x << $y;
		} elsif ($t eq '>') {
			my $y = get_add_expr($scope,$tokens,$line);
			$x = $x >> $y;
		} else {
			last;
		}
	}
	unshift @$tokens, $t;
	return $x;
}

sub get_and_expr($$$) {
	my ($scope,$tokens,$line) = @_;
	my $x = get_shift_expr($scope,$tokens,$line);
	my $t;
	for (;;) {
		$t = get_token($tokens,$line);
		if ($t eq '&') {
			my $y = get_shift_expr($scope,$tokens,$line);
			$x = $x & $y;
		} else {
			last;
		}
	}
	unshift @$tokens, $t;
	return $x;
}

sub get_xor_expr($$$) {
	my ($scope,$tokens,$line) = @_;
	my $x = get_and_expr($scope,$tokens,$line);
	my $t;
	for (;;) {
		$t = get_token($tokens,$line);
		if ($t eq '^') {
			my $y = get_and_expr($scope,$tokens,$line);
			$x = $x ^ $y;
		} else {
			last;
		}
	}
	unshift @$tokens, $t;
	return $x;
}

sub get_ior_expr($$$) {
	my ($scope,$tokens,$line) = @_;
	my $x = get_xor_expr($scope,$tokens,$line);
	my $t;
	for (;;) {
		$t = get_token($tokens,$line);
		if ($t eq '|') {
			my $y = get_xor_expr($scope,$tokens,$line);
			$x = $x | $y;
		} else {
			last;
		}
	}
	unshift @$tokens, $t;
	return $x;
}

sub get_constant_expr($$$) {
	my ($scope,$tokens,$line) = @_;
	my $x = get_ior_expr($scope,$tokens,$line);
	return $x;
}

sub parse_enum($$$$) {
	my ($scope,$tokens,$line,$enum_name) = @_;
	my $vl = $scope->{values};
	my $x = 0;
	for (;;) {
		my $t = get_token($tokens,$line);
		last if ($t eq '}');
		if (!defined($t)) {
			die "Unexpected end of file at line $$line\n";
		}
		if ($t !~ /^\w+$/) {
			die "Syntax error at line $$line\n";
		}
		if (defined($vl->{$t})) {
			die "Duplicate identifier at line $$line\n";
		}
		my $t2 = get_token($tokens,$line);
		if ($t2 eq ',') {
			$vl->{$t} = {type=>$enum_name, size=>4, value=>$x, enum=>1};
			++$x;
		} elsif ($t2 eq '}') {
			$vl->{$t} = {type=>$enum_name, size=>4, value=>$x, enum=>1};
			++$x;
			last;
		} elsif ($t2 eq '=') {
			$x = get_constant_expr($scope, $tokens, $line);
			$vl->{$t} = {type=>$enum_name, size=>4, value=>$x, enum=>1};
			++$x;
			$t2 = get_token($tokens,$line);
			last if ($t2 eq '}');
			next if ($t2 eq ',');
			die "Syntax error at line $$line\n";
		} else {
			unshift @$tokens, $t2;
		}
	}
	my $t = get_token($tokens,$line);
	if ($t ne ';') {
		die "Missing ; at line $$line\n";
	}
}

sub parse_decl_def($$$) {
	my ($scope,$tokens,$line) = @_;
	my $level=0;
	my @decl;
	while ( scalar(@$tokens) ) {
		my $t = get_token($tokens, $line);
		if ($t eq ';' and $level==0) {
			return @decl;
		}
		push @decl, $t;
		if ($t eq '{') {
			++$level;
		}
		if ($t eq '}') {
			if ($level==0) {
				die "Syntax error at line $$line\n";
			}
			if (--$level==0) {
				return ();	# end of function definition reached
			}
		}
	}
	die "Unexpected end of file at line $$line\n";
}

sub dump_scope($) {
	my ($scope) = @_;
	my $el = $scope->{enums};
	my $cl = $scope->{classes};
	my $vl = $scope->{values};
	print "SCOPE: $scope->{name}\n";
	if (scalar(@$el)) {
		print "\tenums:\n";
		foreach (@$el) {
			print "\t\t$_\n";
		}
	}
	if (scalar(keys(%$vl))) {
		print "\tvalues:\n";
		foreach $vname (keys(%$vl)) {
			my $v = $vl->{$vname};
			my $x = $v->{value};
			my $t = $v->{type};
			my $sz = $v->{size};
			if ($v->{enum}) {
				print "\t\t$vname\=$x (enum $t) size=$sz\n";
			} else {
				print "\t\t$vname\=$x (type $t) size=$sz\n";
			}
		}
	}
	if ($scope->{scope}) {
		my $members = $scope->{members};
		foreach (@$members) {
			my $n = $_->{name};
			my $sz = $_->{size};
			my $off = $_->{offset};
			my $spc = $_->{spacing};
			if (defined $spc) {
				print "\t$n\[\]\: spacing $spc size $sz offset $off\n";
			} else {
				print "\t$n\: size $sz offset $off\n";
			}
		}
		print "\tOverall size : $scope->{size}\n";
		print "\tOverall align: $scope->{align}\n";
	}
	foreach $s (@$cl) {
		dump_scope($s);
	}
}

sub output_scope($$) {
	my ($scope, $out) = @_;
	my $el = $scope->{enums};
	my $cl = $scope->{classes};
	my $vl = $scope->{values};
	my $sn = scope_full_name($scope);
	my $sp = ($scope->{file}) ? "" : $sn."_";
	if ($scope->{file}) {
		push @$out, "";
		push @$out, &$comment_sub("FILE SCOPE");
		push @$out, "";
	} else {
		push @$out, "";
		push @$out, &$comment_sub($scope->{specifier}." ".$scope->{name});
		push @$out, "";
	}
	if (scalar(keys(%$vl))) {
		foreach $vname (keys(%$vl)) {
			my $v = $vl->{$vname};
			my $x = $v->{value};
			my $t = $v->{type};
			my $sz = $v->{size};
			push @$out, &$format_sub($sp.$vname, $x);
		}
	}
	if ($scope->{scope}) {
		my $members = $scope->{members};
		foreach (@$members) {
			my $n = $_->{name};
			my $sz = $_->{size};
			my $off = $_->{offset};
			my $spc = $_->{spacing};
			push @$out, &$format_sub($sp.$n, $off);
			if (defined $spc) {
				push @$out, &$format_sub($sp.$n."_spc", $spc);
			}
		}
		push @$out, &$format_sub($sp."sz", $scope->{size});
	}
	foreach $s (@$cl) {
		if ($s->{complete})	{
			output_scope($s, $out);
		}
	}
}

sub scope_full_name($) {
	my ($scope) = @_;
	if ($scope->{file}) {
		return "";
	}
	my $parent = $scope->{scope};
	if ($parent->{file}) {
		return $scope->{name};
	}
	return scope_full_name($parent)."_".$scope->{name};
}

sub pad($$) {
	my ($lineref, $n) = @_;
	my $l = length ($$lineref);
	if ($l < $n) {
		$$lineref .= ' 'x($n-$l);
	}
}

#
# Subroutines for ARMASM compatible output
#
sub armasm_format($$;$) {
	my ($name, $value, $comment) = @_;
	my $r = "$name ";
	pad(\$r, 40);
	$r .= sprintf("EQU 0x%08x", $value & 0xFFFFFFFF);
	if ($comment and $comment!~/^\s*$/) {
		$r .= " ";
		pad(\$r, 60);
		$r .= "; $comment";
	}
	return $r;
}

sub armasm_comment($) {
	my ($comment) = @_;
	return "; $comment";
}

sub armasm_end() {
	return "\n\tEND\n";
}

#
# Subroutines for GNU AS compatible output
#
sub as_format($$;$) {
	my ($name, $value, $comment) = @_;
	my $r = "    .equ $name, ";
	pad(\$r, 50);
	$r .= sprintf("0x%08x", $value & 0xFFFFFFFF);
	if ($comment and $comment!~/^\s*$/) {
		$r .= " ";
		pad(\$r, 65);
		$r .= "/* $comment */";
	}
	return $r;
}

sub as_comment($) {
	my ($comment) = @_;
	if (length ($comment) > 0) {
		return "/* $comment */";
	} else {
		return "";
	}
}

sub as_end() {
	return "";
}

#
# Subroutines for Turbo Assembler compatible output
#
sub tasm_format($$;$) {
	my ($name, $value, $comment) = @_;
	my $r = "$name ";
	pad(\$r, 40);
	$r .= sprintf("EQU 0%08xh", $value & 0xFFFFFFFF);
	if ($comment and $comment!~/^\s*$/) {
		$r .= " ";
		pad(\$r, 60);
		$r .= "; $comment";
	}
	return $r;
}

sub tasm_comment($) {
	my ($comment) = @_;
	return "; $comment";
}

sub tasm_end() {
	return "";
}
