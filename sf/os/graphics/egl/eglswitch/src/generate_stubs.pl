#!/bin/perl -w

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
# Generates the egl_stubs.h header file and the eglswitchu.def
# file from the libeglu def file.
# 
#

use strict;
my $COPYRIGHT = "" .
"// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).\n" .
"// All rights reserved.\n" .
"// This component and the accompanying materials are made available\n" .
"// under the terms of the License \"Eclipse Public License v1.0\"\n" .
"// which accompanies this distribution, and is available\n" .
"// at the URL \"http://www.eclipse.org/legal/epl-v10.html\".\n" .
"//\n" .
"// Initial Contributors:\n" .
"// Nokia Corporation - initial contribution.\n" .
"//\n" .
"// Contributors:\n" .
"//\n" .
"// Description:\n";

my $EGL_DEF;
my $EGLSWITCH_HEADER = "egl_stubs.h";
my $EGLSWITCH_DEF = "../BWINS/eglswitchu.def";
my $SOURCE_DEF_SIZE = 0;

&main();
exit(0);

sub main() {
	if (@ARGV == 0) {
		printf "Usage: generate_stubs.pl <EGL DEF file>\n";
		printf "e.g. generate_stubs.pl /epoc32/include/def/win32/libegl14u.def\n";
		return 0;
	}
	
	$EGL_DEF = $ARGV[0];
	my $maxOrdinal = 1;

	open DEF, $EGL_DEF or
		die "Cannot open $EGL_DEF\n";

	my ($dev, $ino, $mode, $nlink, $uid, $gid, 
		$rdev, $size, $atime, $mtime, $ctime, 
		$blksize, $blocks) 
		= stat($EGL_DEF);
	# the file size could be checked by the switcher build to verify that the stub is up to date.
    $SOURCE_DEF_SIZE= $size;
    
	open HEADER_OUT, ">${EGLSWITCH_HEADER}" or
		die "Cannot create $EGLSWITCH_HEADER\n";

	open DEF_OUT, ">${EGLSWITCH_DEF}" or
		die "Cannot create $EGLSWITCH_DEF\n";

	&printHeaderStart(\*HEADER_OUT);
	&printDefStart(\*DEF_OUT);

	while (<DEF>) {
		chomp;
		if (/@/) {
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

	print $fh "$COPYRIGHT\n".
    "// Generated from \"$EGL_DEF\" file size: $SOURCE_DEF_SIZE\n" .
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
