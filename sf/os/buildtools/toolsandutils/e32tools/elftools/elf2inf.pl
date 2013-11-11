# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

use Getopt::Long;
use strict;

my $outputFileName = "default.inf";
my $discard_noncallable_exports = 0;
my @inputFiles;
my @exports = ();
my %symsSeen = ();

#

&args;
&main;


sub args ()
{
    my %opts = ();
    my $result = GetOptions(\%opts, "output:s", "discard_noncallable_exports");
    $outputFileName = $opts{'output'} if $opts{'output'} ;
    $discard_noncallable_exports = 1 if $opts{'discard_noncallable_exports'};
    @inputFiles = @ARGV;
}

sub main ()
{

    foreach my $file (@inputFiles){
	ReadFromElf($file);
    }
    open( INFFILE, ">$outputFileName") or 
	die "Error: can't open $outputFileName\n";
    foreach my $export (sort {$a cmp $b } @exports) {
	print INFFILE $export;
    }
    close INFFILE;
}

sub ReadFromElf ($)
{
    my ($filename) = @_;
    my $line;
    %symsSeen = ();
    open(FROMELF, "getexports $filename|")
	or die "Error: can't open $filename\n";

    while ($_ = <FROMELF> ) 
    {
	    if ($_ =~ /^\s*(\S+)\s+((DATA)\s+(\d+))?\s*$/) {
		my $sym = $1;
		my $data = "";
		unless ($symsSeen{$sym}) { 
		    $symsSeen{$sym} = $sym;
			
			if($2){
#			Indicate if this is a data symbol along with its size.
			if($4){
				$data = " DATA $4";
				}
			}

		    if ($sym =~ /^_ZTV/ && !$discard_noncallable_exports) { # allow vtables thru
			push @exports, " _$sym (#<VT>#)\n";
		    } elsif ($sym =~ /^_ZTI/ && !$discard_noncallable_exports) { # allow type info  thru: comment out if we don't want it
			push @exports, " _$sym (#<TI>#)\n";
		    } elsif ($sym =~ /^_ZThn\d/) { # allow thunk  thru
			push @exports, " _$sym (#<thunk>#)\n";
		    } elsif ($sym =~ /^_ZT/) { # punt all other type stuff: shouldn't be exported anyway
			next;
		    } elsif (C9Ctor($sym)) { #work around non-export of constructors
			PushCtors($sym, $filename);
		    } elsif (D9Dtor($sym)) { #work around non-export of constructors
			PushDtors($sym, $filename);
		    } else {
			push @exports, " _$sym$data\n";
		    }
		}
	    }
	}
    close FROMELF;
}

sub PushSym($$)
{
    my ($s, $f) = @_;
    unless ($symsSeen{$s}) {
	$symsSeen{$s} = $s;
	push @exports, " _$s\n";
    }
}

sub simpleC9Ctor($)
{
    my ($sym) = @_;
    if ($sym =~ /_ZN(\d+)(.+)C9E.*$/) {
	# Really check it
	return 1 if ($1 == length($2));
    } 
    return 0;
}

sub simpleD9Ctor($)
{
    my ($sym) = @_;
    if ($sym =~ /_ZN(\d+)(.+)D9E.*$/) {
	# Really check it
	return 1 if ($1 == length($2));
    } 
    return 0;
}

sub C9Ctor($)
{
    my ($sym) = @_;
    return 1 if simpleC9Ctor($sym);
    if ($sym =~ /_ZN(\d+)(.+)C9E.*$/) {
	my $l = $1;
	my $s = $2;
	my $t_params = substr $s, $l;
	return 1 if ($t_params =~ /^I.*I$/); # not totally convincing check for templated ctor
    }
    return 0;
}

sub PushCtors($$)
{
    my ($sym, $filename) = @_;
    return PushSimpleCtors($sym, $filename) if (simpleC9Ctor($sym));

    my ($sym, $filename) = @_;
    $sym =~ /(_ZN\d+.+IC)9E.*$/o;
    my $s = $1;
    my $i = length $s;
    my $e = substr $sym, $i + 1;
    my $C1 = "$s"."1"."$e";
    my $C2 = "$s"."2"."$e";
    push @exports, " _$sym\n";
    PushSym($C1, $filename);
    PushSym($C2, $filename);

} 

sub PushSimpleCtors($$)
{
    my ($sym, $filename) = @_;
    $sym =~ /(_ZN\d+.+C)9E.*$/o;
    my $s = $1;
    my $i = length $s;
    my $e = substr $sym, $i + 1;
    my $C1 = "$s"."1"."$e";
    my $C2 = "$s"."2"."$e";
    push @exports, " _$sym\n";
    PushSym($C1, $filename);
    PushSym($C2, $filename);
}

sub D9Dtor($)
{
    my ($sym) = @_;
    return 1 if simpleD9Ctor($sym);
    if ($sym =~ /_ZN(\d+)(.+)D9E.*$/) {
	my $l = $1;
	my $s = $2;
	my $t_params = substr $s, $l;
	return 1 if ($t_params =~ /^I.*I$/); # not totally convincing check for templated ctor
    }
    return 0;
}

sub PushDtors($$)
{
    my ($sym, $filename) = @_;
    return PushSimpleDtors($sym, $filename) if (simpleD9Ctor($sym));

    my ($sym, $filename) = @_;
    $sym =~ /(_ZN\d+.+ID)9E.*$/o;
    my $s = $1;
    my $i = length $s;
    my $e = substr $sym, $i + 1;
    my $D0 = "$s"."0"."$e";
    my $D1 = "$s"."1"."$e";
    my $D2 = "$s"."2"."$e";
    push @exports, " _$sym\n";
    PushSym($D0, $filename);
    PushSym($D1, $filename);
    PushSym($D2, $filename);
} 


sub PushSimpleDtors($$)
{
    my ($sym, $filename) = @_;
    $sym =~ /(_ZN\d+.+D)9E.*$/o;
    my $s = $1;
    my $i = length $s;
    my $e = substr $sym, $i + 1;
    my $D0 = "$s"."0"."$e";
    my $D1 = "$s"."1"."$e";
    my $D2 = "$s"."2"."$e";
    push @exports, " _$sym\n";
    PushSym($D0, $filename);
    PushSym($D1, $filename);
    PushSym($D2, $filename);
}

