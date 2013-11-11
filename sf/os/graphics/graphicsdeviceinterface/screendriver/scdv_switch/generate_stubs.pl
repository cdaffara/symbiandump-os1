#!/bin/perl -w

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
# Generates the scdv_stubs.h header file and the scdvswitchu.def
# file from the scdvu def file.
# 
#

use strict;
my $COPYRIGHT = "Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).";

my $SCDV_DEF = "../bwins/scdvu.def";
my $SCDVSWITCH_HEADER = "scdv_stubs.h";
my $SCDVSWITCH_DEF = "../bwins/scdvswitchu.def";
my $SOURCE_DEF_SIZE = 0;

&main();
exit(0);

sub main() {
	my $maxOrdinal = 1;

	open DEF, $SCDV_DEF or
		die "Cannot open $SCDV_DEF\n";

	my ($dev, $ino, $mode, $nlink, $uid, $gid, 
		$rdev, $size, $atime, $mtime, $ctime, 
		$blksize, $blocks) 
		= stat($SCDV_DEF);
	# the file size could be checked by the switcher build to verify that the stub is up to date.
    $SOURCE_DEF_SIZE= $size;
    
	open HEADER_OUT, ">${SCDVSWITCH_HEADER}" or
		die "Cannot create $SCDVSWITCH_HEADER\n";

	open DEF_OUT, ">${SCDVSWITCH_DEF}" or
		die "Cannot create $SCDVSWITCH_DEF\n";

	&printHeaderStart(\*HEADER_OUT);
	&printDefStart(\*DEF_OUT);

	while (<DEF>) {
		chomp;
		if (/^\s+\?/) {
			if (s/.*;/;/) {
				&printDefEntry(\*DEF_OUT, $maxOrdinal, $_);
				&printHeaderEntry(\*HEADER_OUT,$maxOrdinal,$_);
			} else {
				&printDefEntry(\*DEF_OUT, $maxOrdinal, "");
				&printHeaderEntry(\*HEADER_OUT,$maxOrdinal, "(noname)");
			}
			$maxOrdinal++;
		}
	}
	&printHeaderEnd(\*HEADER_OUT,$maxOrdinal);
	&printDefEnd(\*DEF_OUT);

	close DEF;
	close HEADER_OUT;
	close DEF_OUT;
}

sub printDefStart(\$) {
	my ($fh) = @_;
	print $fh "EXPORTS\n";
}

sub printDefEntry(\$\$\$) {
	my ($fh, $ordinal, $comment) = @_;
	print $fh "\tcall_vector_${ordinal} @ ${ordinal} NONAME $comment\n";
}

sub printDefEnd(\$) {
	my ($fh) = @_;
	print $fh "\n";
}

sub printHeaderStart(\$) {
	my ($fh) = @_;

	print $fh "// Generated from \"$SCDV_DEF\" file size: $SOURCE_DEF_SIZE\n" .
		"// $COPYRIGHT\n" .
		"\n" .
		"extern \"C\" {\n" .
		"void common_dispatch();\n" .
		"\n";
}

sub printHeaderEntry(\$\$\$) {
	my ($fh, $ordinal, $comment) = @_;

	print $fh "__declspec(dllexport)\n" .
		"__declspec(naked)\n" .
		"void call_vector_${ordinal} ()\n" .
		"\t{\n" .
		"\t// ${comment}\n" .
		"\t_asm mov eax, $ordinal\n" .
		"\t_asm jmp common_dispatch\n" .
		"\t}\n\n";
}

sub printHeaderEnd(\$\$) {
	my ($fh, $maxOrdinal) = @_;
	print $fh "}\n" .
		"#define MAX_ORDINAL $maxOrdinal\n\n";
}
