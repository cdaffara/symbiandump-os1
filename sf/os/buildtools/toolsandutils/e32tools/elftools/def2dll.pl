# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
use FindBin;		# for FindBin::Bin
use Getopt::Long;
use Cwd;

my $PerlLibPath;    # fully qualified pathname of the directory containing our Perl modules

BEGIN {
# check user has a version of perl that will cope
	require 5.005_03;
# establish the path to the Perl libraries: currently the same directory as this script
	$PerlLibPath = $FindBin::Bin;	# X:/epoc32/tools
	if ($^O eq "MSWin32")
	{
		$PerlLibPath =~ s/\//\\/g;	# X:\epoc32\tools
		$PerlLibPath .= "\\";
	}	
}

use lib $PerlLibPath;
use Defutl;

my %opts = ();

my $result = GetOptions(\%opts,
						"path:s",
						"bldpath:s",
						"deffile:s",
						"linkAs:s",
						"import:s",
						"export:s",
						"absent:s",
						"inter",
						"sym_name_lkup"
						);

my $gPath = $opts{"path"}; # 0
my $gBldPath = $opts{"bldpath"}; # 0
my $compName = $opts{"import"}; # 1
my $gDefFile = $opts{"deffile"}; # 2
my $gDllName = $opts{"linkAs"};  # 3
my $gExpFile = $opts{"export"};
my $gLibFile = "$compName.lib" if $compName;
my $gSymbolNameLookup = $opts{"sym_name_lkup"};

my $oP = '--';

my $floatingpointmodel = "${oP}fpu softvfp";
my $interworkingp = $opts{"inter"};
my $interworking = "${oP}apcs /nointer";
$interworking = "${oP}apcs /inter" if ($interworkingp);

my @objectFiles;

&main;

my @DefDataStruct;

sub main ()
{
       unless($gDefFile)
       {
               usage();
       }
	my $FrzExportsOn=0;
	eval { &Def_ReadFileL(\@DefDataStruct, $gDefFile, $FrzExportsOn); };
	die $@ if $@;

    &parseDefFile(\@DefDataStruct, $opts{absent}, $gDefFile);
    &genExpFile($gPath, $gBldPath, $gExpFile) if $gExpFile;
    &genLibFile($gPath, $gBldPath, $gLibFile, $gDllName) if $gLibFile;
}

# Usage subroutine
sub usage( )
{
        print "\n";
        print "DEF2DLL -Creates binary objects used to implement the Symbian OS DLL model\n";
        print "\n";
        print "Usage: def2dll --deffile=<file> [--path=<dir>] [--bldpath=<dir>] [--import=<file>]  [--linkas=<file>] [--inter] [--export=<file>] [--sym_name_lkup]\n";

        print "\nOptions:\n";
	print "\t--deffile=<file>   	 	: def file to be converted\n";
        print "\t--path =<direcotry>		: destination path\n";
        print "\t--bldpath=<direcotry>		: build path for dll\n";
        print "\t--import=<file>			: import from file name\n";
        print "\t--linkas=<file>     		: linkas to file name specified\n";
        print "\t--inter                 	: enables interworking on ARM and THUMB\n";
        print "\t--export=<file>			: export to filename\n";
        print "\t--sym_name_lkup         	: symbol name ordinal number lookupç\n";
        print "\n";
        exit 1;
}

my %symbols = ();
my %symbolIndexMap = ();
my $exports = 0;

sub parseDefFile ($$$)
{
    my ($defStructRef, $AbsentSubst, $defFile) = @_;
	my $Ref;
	foreach $Ref (@$defStructRef) {
		next if (!$Ref);
		next if (!defined ($$Ref{Ordinal}));
		my $symbol = $$Ref{Name};
		my $index = $$Ref{Ordinal};
		my $rest = $$Ref{Comment};
		my $symbolType = 'CODE';
		if ($$Ref{Data} || ($symbol =~ /^(_ZTV|_ZTI|_ZTT)/)){
			$symbolType = 'DATA';
		}
		else {
			$exports = 1;
		}
		if ($symbols{$symbol} and !$$Ref{Absent}) {
			warn "DEF2DLL - WARNING: $symbol duplicated in $defFile\n";
		} else {
			if ($$Ref{Absent}) {
				$symbolIndexMap{$index} = $AbsentSubst;
			} else {
				$symbols{$symbol} = $symbolType;
				$symbolIndexMap{$index} = $symbol;
			}
		}
    }
}

sub genExpFile ($$$)
{
    my ($path, $bldpath, $expFile) = @_;
    my $numkeys = keys %symbolIndexMap;
    my $failed = 0;

    open EXPFILE, ">$path\\$expFile.s" or
		die "Error: can't create $path\\$expFile.s\n";

    print EXPFILE "\tEXPORT __DLL_Export_Table__\n\n";
    print EXPFILE "\tEXPORT |DLL\#\#ExportTable|\n\n";
    print EXPFILE "\tEXPORT |DLL\#\#ExportTableSize|\n\n";
    print EXPFILE "\tAREA ExportTable, CODE\n";


    print EXPFILE "__DLL_Export_Table__\n";
	if ($interworkingp) {
	  print EXPFILE "\tBX lr\n";
	} else {
	  print EXPFILE "\tMOV pc, lr\n";
	}

    print EXPFILE "|DLL\#\#ExportTableSize|\n";
    printf EXPFILE "\tDCD %d\n", $numkeys;
	if($gSymbolNameLookup) {
		print EXPFILE "\tDCD 0 \n";# This is the 0th ordinal for elftran to fill in.
	}
    print EXPFILE "|DLL\#\#ExportTable|\n";

    my @orderedSyms;
    my $maxIndex = 0;
    my $index;
    foreach $index (keys %symbolIndexMap) {
		$maxIndex = $index if $index > $maxIndex;
		$orderedSyms[$index] = $symbolIndexMap{$index};
    }

    print EXPFILE "\tPRESERVE8\n\n";
    my $n;
    for ($n = 1; $n <= $maxIndex ; $n++) {
		my $entry = $orderedSyms[$n];
		if ($entry) {
			print EXPFILE "\tIMPORT $entry\n";
			print EXPFILE "\tDCD $entry \t; @ $n\n";
		} else {
			warn "WARNING: missing entry at index $n\n";
			print EXPFILE "\tDCD 0 ; missing symbol\n";
		}
    }


    # create a .directive section
    print EXPFILE "\n\n\tAREA |.directive|, READONLY, NOALLOC\n";
    # Mark the section as armlink edit commands
    print EXPFILE "\tDCB  \"#<SYMEDIT>#\\n\"\n"; 
    # mark the imported symbol for 'dynamic' export
    print EXPFILE "\tDCB  \"EXPORT DLL##ExportTable\\n\"\n";
    print EXPFILE "\tDCB  \"EXPORT DLL##ExportTableSize\\n\"\n";

    print EXPFILE "\tEND";
    close EXPFILE;

    $failed = system "armasm $floatingpointmodel $interworking -o $path\\$expFile.exp $path\\$expFile.s";
    unlink ("$path\\$expFile.s") unless $failed;
    die "Error: cant create $path\\$expFile.exp\n" if $failed;
}

my %DataSymbols = ();

sub genVtblExportFile($$)
{
    my ($bldpath, $dllName) = @_;
    my $FileName = "VtblExports";

    open VTBLFILE, ">$bldpath\\$FileName.s" or
		die "Error: can't create $bldpath\\$FileName.s\n";

    print VTBLFILE "\tAREA |.directive|, NOALLOC, READONLY, ALIGN=2\n";
    printf VTBLFILE "\tDCB \"\#\<SYMEDIT\>\#\\n\"\n";

    my $symbol;
    foreach $symbol (sort keys %DataSymbols) {
		my $index = $DataSymbols{$symbol};
		
		$symbol =~ s/^"(.*)"$/$1/;	# remove enclosing quotes
		printf VTBLFILE "\tDCB \"IMPORT \#\<DLL\>$dllName\#\<\\\\DLL\>%x AS $symbol \\n\"\n", $index;
    }
#    printf VTBLFILE "\tDCB \"\#\<\\\\VTBLSYMS\>\#\\n\"\n";
    print VTBLFILE "\tEND";
    close VTBLFILE;

    my $failed = system "armasm $floatingpointmodel $interworking -o $bldpath\\$FileName.o $bldpath\\$FileName.s";
    unlink ("$bldpath\\$FileName.s");
    die "Error: cant create $bldpath\\$FileName.o\n" if $failed;
    push @objectFiles, "$bldpath\\$FileName.o";
}

sub genLibFile ($$$$)
{
    my ($path, $bldpath, $libFile, $dllName) = @_;
    my $tempFileName = "$bldpath\\$compName";
    my $viaFileName = sprintf("$bldpath\\_t%x_via_.txt", time);
    my $keyz = keys %symbolIndexMap;
    my $failed = 0;
    my $key;

    if ($keyz > 0) {
		open STUBGEN, "|$ENV{'EPOCROOT'}/epoc32/tools/genstubs" if $exports;
		foreach $key (sort keys %symbolIndexMap) {
			my $symbol = $symbolIndexMap{$key};
			my $stubFileName = "$tempFileName-$key";
			if ( $symbols{$symbol} eq 'DATA') {
				$DataSymbols{$symbol} = $key;
			} else {
				printf STUBGEN "$stubFileName.o $symbol #<DLL>$dllName#<\\DLL>%x\n", $key;
				push @objectFiles, "$stubFileName.o";
			}
		}
		genVtblExportFile($bldpath, $dllName);
    } else {
		# create dummy stub so armar creates a .lib for us
		open STUBGEN, "|$ENV{'EPOCROOT'}/epoc32/tools/genstubs";
		print STUBGEN "$tempFileName-stub.o $tempFileName##stub $dllName##dummy\n";
		push @objectFiles, "$tempFileName-stub.o";
    }
    close STUBGEN;

    open VIAFILE, ">$viaFileName" or
		die "Error: can't create VIA fie $viaFileName\n";

    print VIAFILE "${oP}create \"$path\\$libFile\"\n";
    my $objectFile;
    foreach $objectFile (@objectFiles) {
		print VIAFILE "\"$objectFile\"\n";
    }
    close VIAFILE;

    $failed = system( "armar ${oP}via $viaFileName");
    push @objectFiles, $viaFileName;
    unlink @objectFiles;
    die "Error: can't create $path\\$libFile\n" if $failed;
}

__END__

