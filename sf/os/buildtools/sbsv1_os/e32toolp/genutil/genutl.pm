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
# Contains utility subroutines for MAKMAKE and associated scripts
# 
#

package Genutl;

require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	Genutl_AnyToHex
	Genutl_VersionToUserString
	Genutl_VersionToHexString
	Genutl_VersionToFileAugment
	Genutl_StringToVersion
	Genutl_ParseVersionedName
	Genutl_NormaliseVersionedName
);

use strict;

sub Genutl_AnyToHex ($) {
# changes decimal and hexadecimal numbers to the required hexadecimal format
	my $Num=$_[0];
	$Num=lc $Num;	# lower casing the x specifying hexadecimal essential
	if ($Num=~/^(\d{1,10}|0x[\dabcdef]{1,8})$/o) { # this isn't perfect
		$Num=oct $Num if $Num=~/^0x/o;
		return sprintf "0x%.8lx", $Num;
	}
	return undef;
}

sub Genutl_VersionToUserString(%) {
	my (%ver) = @_;
	return sprintf("%d\.%d", $ver{major}, $ver{minor});
}

sub Genutl_VersionToHexString(%) {
	my (%ver) = @_;
	return sprintf("%04x%04x", $ver{major}, $ver{minor});
}

sub Genutl_VersionToFileAugment(%) {
	my (%ver) = @_;
	return sprintf("{%04x%04x}", $ver{major}, $ver{minor});
}

sub Genutl_StringToVersion($) {
	my ($s) = @_;
	if ($s =~ /^(\d+)\.(\d+)$/) {
		my %ver;
		$ver{major} = $1;
		$ver{minor} = $2;
		if ($ver{major}<32768 and $ver{minor}<32768) {
			return %ver;
		}
	}
	return undef;
}

sub Genutl_ParseVersionedName($) {
	my ($nref) = @_;
	return 1 unless ($$nref =~ /\{|\}/);
	my $a;
	my $b;
	if ($$nref =~ /(.*)\{((\d|a|b|c|d|e|f){8})\}(.*?)/i) {
		$a = $1;
		$b = $3;
	} elsif ($$nref =~ /(.*)\{\s*(\d+)\s*\.\s*(\d+)\s*\}(.*?)$/i) {
		$a = $1;
		$b = $4;
		my $major = $2;
		my $minor = $3;
		return 0 if ($major>=32768 or $minor>=32768);
		$$nref = $a.sprintf("{%04x%04x}",$major,$minor).$b;
	} else {
		return 0;
	}
	if ($a=~/\{|\}/ or $b=~/\{|\}/) {
		return 0;
	}
	return 1;
}

sub Genutl_NormaliseVersionedName($) {
	my ($name) = @_;
	if ($name =~ /(.*)\{\s*(\d+)\s*\.\s*(\d+)\s*\}(.*?)$/i) {
		my $a = $1;
		my $b = $4;
		my $major = $2;
		my $minor = $3;
		return $a.sprintf("{%04x%04x}",$major,$minor).$b if ($major<32768 and $minor<32768);
	}
	return $name;
}
