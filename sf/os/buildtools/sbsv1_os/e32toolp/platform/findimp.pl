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

my $argc=scalar(@ARGV);
$argc==2 or die "findimp <map file> <output file>\n";
my $infile=@ARGV[0];
my $outfile=@ARGV[1];

my @imp_dll_names;
my @imp_dll_ordinal_lists;
my $i=0;
open INFILE, $infile or die "Can't open input file $infile\n";
while (<INFILE>) {
	if (/^\s*(\d+)\:(([0-9|a-f|A-F])+)\s+__imp_(\S+)\s+(([0-9|a-f|A-F])+)\s+(\S+?)\:(.*?)\s*$/) {
		my $section_num=$1;
		my $section_offset=hex $2;
		my $import_name="__imp_$4";
		my $addr=$5;
		my $dllname=$8;
		my $implist;
		for ($i=0; $i<scalar(@imp_dll_names); ++$i) {
			if ($imp_dll_names[$i] eq $dllname) {
				$implist=$imp_dll_ordinal_lists[$i];
				push @$implist, $section_offset;				
				last;
			}
		}
		if ($i==scalar(@imp_dll_names)) {
			my @new_list;
			push @new_list, $section_offset;
			push @imp_dll_names, $dllname;
			push @imp_dll_ordinal_lists, \@new_list;
		}
	}
}
close INFILE;
my $noffset=4;
my $n_imp_dlls=scalar(@imp_dll_names);
for ($i=0; $i<$n_imp_dlls; ++$i) {
	$noffset+=8;
	my $implist=$imp_dll_ordinal_lists[$i];
	foreach (@$implist) {
		$noffset+=4;
	}
}
open OUTFILE, ">$outfile" or die "Can't open output file $outfile\n";
binmode OUTFILE;
printf OUTFILE "%c%c%c%c",$n_imp_dlls&0xff,($n_imp_dlls>>8)&0xff,($n_imp_dlls>>16)&0xff,$n_imp_dlls>>24;
$i=0;
for ($i=0; $i<$n_imp_dlls; ++$i) {
	my $nlen=length $imp_dll_names[$i];
	printf OUTFILE "%c%c%c%c",$noffset&0xff,($noffset>>8)&0xff,($noffset>>16)&0xff,$noffset>>24;
	$noffset+=$nlen + 1;
	my $implist=$imp_dll_ordinal_lists[$i];
	my $nimp=scalar(@$implist);
	printf OUTFILE "%c%c%c%c",$nimp&0xff,($nimp>>8)&0xff,($nimp>>16)&0xff,$nimp>>24;
	foreach (@$implist) {
		printf OUTFILE "%c%c%c%c",$_&0xff,($_>>8)&0xff,($_>>16)&0xff,$_>>24;
	}
}
for ($i=0; $i<$n_imp_dlls; ++$i) {
	print OUTFILE $imp_dll_names[$i];
	printf OUTFILE "%c",0;
}
close OUTFILE;
