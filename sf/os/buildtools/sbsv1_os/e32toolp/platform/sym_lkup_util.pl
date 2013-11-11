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

use FindBin;		# for FindBin::Bin
use Getopt::Long;

BEGIN {
# check user has a version of perl that will cope
	require 5.005_03;
# establish the path to the Perl libraries: currently the same directory as this script
	$PerlLibPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlLibPath =~ s/\//\\/g;	# X:\epoc32\tools
	$PerlLibPath .= "\\";
}

use lib $PerlLibPath;

my $IgnoreExportDir = 0;
my $ImportSymFile;
my $CppFile;
my $mapFile;
my $SegmentContentSz = 0;
my $nSyms = 0;
my $nImports = 0;
my %Symbols = ();
my @Imports = ();

{
	unless (GetOptions(\%Options, 'sym=s', 'o=s', 'map=s', 'ignore_export_dir')) {
		exit 1;
	}
	$ImportSymFile = $Options{sym};
	$CppFile = $Options{o};
	$mapFile = $Options{map};
	$IgnoreExportDir = $Options{ignore_export_dir};

	ReadSymFile() if $ImportSymFile;
	ReadMapFile() if $mapFile;
	
	GenNamedSegment();
}

sub ReadMapFile() {
	open FILE, $mapFile or die "Error :SymLookup: Cannot open map file $mapFile\n";

	my $GlbSyms = 0;
	while(<FILE>) {
		if($_ =~ /Public Symbols/) {
			$GlbSyms = 1;
		}
		elsif( !$GlbSyms ) {
			next;
		}
		if($_ =~ /([0-9a-fA-F]{8})\s+(\S+)\s+_?(\S+)\s/){
			$addr = $1;
			$module = $2;
			$name = $3;
			if( defined $Symbols{$name} ) {
				$Symbols{$name} = $addr;
			}
		}
	}
#	Remove symbols not found in map file
	foreach my $sym (keys %Symbols){
		if( !$Symbols{$sym} ){
			delete $Symbols{$sym};
		}
	}

}

sub ReadSymFile() {
	open FILE, $ImportSymFile or die "Error :SymLookup: Cannot open file $ImportSymFile\n";
	
	my $ImportDirSeen = 0;
	my $ExportDirSeen = 0;
	my $ExportNameTblSeen = 0;
	my $numOfExportNames = 0;
	my $nameCount = 0;
	while (<FILE>) {
# Ignore export table(s) if the flag '$IgnoreExportDir' is set. This flag is set for stddlls, as symbol listing 
# is not required for them. The windows API GetProcAddr can be used directly. While for stdexe, the symbol names
# are filtered out and then looked up in the map file for their addresses.

		if($_ =~ /\*\*\* EXPORT DIRECTORY \*\*\*/){
			next if($IgnoreExportDir);
			$ExportDirSeen = 1;
		}
		elsif($_ =~ /\*\*\* Export Name Pointer Table \*\*\*/){
			next if($IgnoreExportDir);
			$ExportNameTblSeen = 1;
		}
		elsif($_ =~ /\*\*\* IMPORT DIRECTORY \*\*\*/) {
			$ImportDirSeen = 1;
		}

		if($ExportDirSeen){
			if($_ =~ /numberofnames\s+=\s+0x(\S+)/){
				$numOfExportNames = hex($1);
# Reset the flag once done with the export table
				$ExportDirSeen = 0;
			}
		}
		elsif($ExportNameTblSeen && $numOfExportNames){
			if($_ =~ /\d+\s+0x[0-9a-fA-F]+\s+(\S+)/){
				$Symbols{$1}=0;
# Keep track of the symbols seen in "Export Name Pointer Table"
				$nameCount++;
			}
			if($nameCount == $numOfExportNames){
# Reset the flag once done with the name table
				$ExportNameTblSeen = 0;
			}
		}
		elsif($ImportDirSeen) {
			if($_ =~ /^DLL name\s+=\s+\S+\s+\((\S+)\)/) {
				my $dllname = $1;
				push @Imports, $dllname;
				$ImportDirSeen = 0;
			}
		}
	}
}

sub GenNamedSegment() {

	my $SegContents = "";
	
	&Header(\$SegContents);

	&SymAddrTbl(\$SegContents);

	&SymNames(\$SegContents);

	&Footer(\$SegContents);

	open OUTFILE, ">$CppFile" or die "Error :SymLookup:Cannot open file $CppFile\n";
	print OUTFILE $SegContents;
}

sub Header(){
	my $SegContentsRef = shift @_;
	
	$$SegContentsRef .= "\/\* $CppFile\n";
	$$SegContentsRef .= " \* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).  All rights reserved.\n";
	$$SegContentsRef .= " \* Makmake-generated source file for named symbol lookup\n";
	$$SegContentsRef .= " \*\/\n";
	$$SegContentsRef .= "#pragma data_seg(\".expdata\")";

	$$SegContentsRef .= "\n\n";
}

sub Footer() {
	my $SegContentsRef = shift @_;
	$$SegContentsRef .= "\n#pragma data_seg()\n";
	$$SegContentsRef .= "\n";
}

sub SymAddrTbl(){
	my $SegContentsRef = shift @_;
	$nSyms = keys %Symbols;
	$nImports = @Imports;
	$$SegContentsRef .= "int NSymbols = $nSyms;\n";
	$$SegContentsRef .= "int NImports = $nImports;\n";

	if(!$nSyms) {
		return;
	}
	$$SegContentsRef .= "int addresses[] = {\n";
	
	foreach $key (sort keys %Symbols) {
		if($Symbols{$key}){
			$$SegContentsRef .= "\t0x".$Symbols{$key}.",\n";
		}
	}
	$$SegContentsRef .= "};\n";
}

sub SymNames() {
	if(!$nImports && !$nSyms){
		return;
	}
	my $SegContentsRef = shift @_;
	$$SegContentsRef .= "\nchar data[] = {\n";

	my $symnames ;
	my $colCnt ;
	foreach $symnames (sort keys %Symbols) {
		next if( $Symbols{$symnames} == 0);
		my @chars = split(//,$symnames);
		$$SegContentsRef .= "\t";
		$colCnt =0;
		foreach $aChar (@chars) {
			if($colCnt >= 80) {
				$$SegContentsRef .= "\\\n\t";
				$colCnt = 0;
			}
			$$SegContentsRef .= "\'$aChar\',";
			$colCnt += 4;
		}
		$$SegContentsRef .= "0,\n";
	}

	foreach my $dll (@Imports) {
		my @chars = split(//,$dll);
		$$SegContentsRef .= "\t";
		$colCnt =0;
		foreach $aChar (@chars) {
			if($colCnt >= 80) {
				$$SegContentsRef .= "\\\n\t";
				$colCnt = 0;
			}
			$$SegContentsRef .= "\'$aChar\',";
			$colCnt += 4;
		}
		$$SegContentsRef .= "0,\n";
	}
	$$SegContentsRef .= "\n};\n";
}

