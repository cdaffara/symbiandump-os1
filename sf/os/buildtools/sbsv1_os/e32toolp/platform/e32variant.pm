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

package E32Variant;

require Exporter;

@ISA = qw(Exporter);

@EXPORT = qw(Variant_GetMacroList Variant_GetMacroHRHFile Variant_GetMacro);

use strict;
use Pathutl;

# using $FindBin::Bin to figure out the location of the epoc32\tools directory as 
# IDE doesn't understand env{EPOCROOT}

my $PerlBinPath;	# fully qualified pathname of the directory containing this script

BEGIN {
	require 5.005_03;		# check user has a version of perl that will cope
	$PerlBinPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlBinPath =~ s/\//\\/g;	# X:\epoc32\tools
}

# get epocroot if it is set
my $epocroot = $ENV{EPOCROOT};
my $toolspath;

#if epocroot is not set figure it out
if(!$epocroot){
    $epocroot = $PerlBinPath . "\\..\\..\\" ; #infer epocroot
    $toolspath = $PerlBinPath;
}else{
    $toolspath = $epocroot . "epoc32\\tools\\";
}

$epocroot=~s/^(.:)//io;    # remove drive letter
$epocroot=&Path_Strip($epocroot); # collapse

my $cfgFile =  $toolspath . "variant\\variant.cfg"; # default location
$cfgFile =~ /^(.:)/i; # match the drive
my $MacroHRHLocation_Drive = lc($1); # save the drive 

sub Variant_GetMacroHRHFile{
    my $file;
    if(-e $cfgFile){
	open(FILE, $cfgFile) || die "\nCould not open: " . $cfgFile ."\n";
	while (<FILE>) {
	    # strip comments
	    s/^([^#]*)#.*$/$1/o;
	    # skip blank lines
	    if (/^\s*$/o) {
		next;
	    }
	    # get the hrh file
	    if($_ =~ /\.hrh/xi){
		$file = $_; 
		last;
	    }
	}
	close FILE;
	die "\nERROR: No variant file specified in $cfgFile!\n" unless $file;
	$file =~ s/\s+//g;
	$file=~s/^(.:)//io;    # remove drive letter
	my $paths_drive = lc($1);

	$file = Path_MakeEAbs($epocroot."EPOC32\\", $epocroot, $file); # assume relative to EPOCROOT

	if($paths_drive){
	    die "\nERROR: Variant file specified in $cfgFile is not on the same drive as \\epoc32\\\n" 
		unless ($paths_drive eq $MacroHRHLocation_Drive);
	}
	if(!(-e $file)) {
	    die "\nERROR: $cfgFile specifies $file which doesn't exist!\n";
	}

	# make sure it is in dos syntax
	$file=~ s/\//\\/g;
    }
    return $file;
}


# returns the variant specific macro definitions as a list
sub Variant_GetMacroList{

    my @macros = ();
    my $vfile = Variant_GetMacroHRHFile();
    
    if($vfile)
    {
	my $VariantFilePath = Path_Split('Path',$vfile);
	chop( $VariantFilePath );
	$VariantFilePath = &Path_PrefixWithDriveAndQuote($VariantFilePath);
	$vfile = &Path_PrefixWithDriveAndQuote($vfile);
	my $e32Path = &Path_PrefixWithDriveAndQuote($epocroot."epoc32\\include");
	
	open CPPPIPE,"cpp -I $e32Path -I $VariantFilePath -undef -dM $vfile |" or die "ERROR: Can't invoke CPP.EXE\n";
	while(<CPPPIPE>){
	    if($_ =~ /(\#define)(\s+)(.+)/){
		push @macros, $3;
	    }
	}
	close CPPPIPE;
    }
    return @macros;
}

sub Variant_GetMacro{

    my $file;
    if(-e $cfgFile){
	open(FILE, $cfgFile) || die "\nCould not open: " . $cfgFile ."\n";
	while (<FILE>) {
	    # strip comments
	    s/^([^#]*)#.*$/$1/o;
	    # skip blank lines
	    if (/^\s*$/o) {
		next;
	    }
	    # get the hrh file
	    if($_ =~ /^ENABLE_ABIV2_MODE$/xi){
		close FILE;
		return 1;
	    }
	}
	close FILE;
    }
    return 0;
}

1;












