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
# Text formatting module
# 
#

package Output;
require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	OutSetLength OutSetPostWrap OutFormat Output
	OutText
);

use strict;


my $Len=80;
my $PreWrap="\\";
my $PreWrapLen=length($PreWrap);
my $PostWrap=' ';
my $PostWrapLen=length($PostWrap);
my $Buf='';
my $Text='';

sub OutSetLength ($) {
	if ($_[0]) {
		$Len=$_[0];
		return $Len;
	}
	$Len=80;
}

sub OutSetPreWrap ($) {
	$PreWrap=$_[0];
	$PreWrapLen=length($PreWrap);
}

sub OutSetPostWrap ($) {
	$PostWrap=$_[0];
	$PostWrapLen=length($PostWrap);
}

sub OutFormat (@) {
	my $Item;
	foreach $Item (@_) {
		$Buf.=$Item;
	}
}

sub OutWrite () {
	my @Buf=();
	my $CurLen=0;
	if ($Buf=~/^(\s)/o) {
		# output any starting spaces or tabs
		$Text.=$1;
		$CurLen=length($1);
	}
	while ($Buf=~/([^ "\t\n\r\f]*"[^"\t\n\r\f]+"[^ "\t\n\r\f]*|[^ "\t\n\r\f]+)/go) {
		# get the elements of $Buf into @Buf
		push @Buf, $1;
	}
	$Buf='';
	my $Elem;
	foreach $Elem (@Buf) {
		my $ElemLen=length($Elem);
		if (($CurLen+$ElemLen+$PreWrapLen) > $Len) {
			# $Len doesn't account for the newline character
			# wrap the line if adding another element will take it over the prescribed length
			$Text.="$PreWrap\n$PostWrap";
			$CurLen=$PostWrapLen;
		}
		elsif ($CurLen>$PostWrapLen) {
			# add a space to the line if they're are already elements in the line
			$Text.=' ';
			$CurLen++;
		}
		# add element to the line
		$Text.=$Elem;
		$CurLen+=$ElemLen;
	}
	# finish with a newline
	$Text.="\n";
}

sub Output (@) {
	OutWrite if $Buf;	# output the formatted text before doing any more output
	my $Item;
	foreach $Item (@_) {
		$Text.=$Item;
	}
}

sub OutText () {
	my $temp=$Text;
	$Text='';
	$temp;
}

1;
