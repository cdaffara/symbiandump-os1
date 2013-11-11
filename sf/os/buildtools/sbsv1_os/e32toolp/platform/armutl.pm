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
# this package does various ancillary things for armedg modules
# 
#

package Armutl;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	Armutl_Help_Mmp

	Armutl_DoMmp

	Armutl_ArmIncDir
	Armutl_ArmLibList
	Armutl_ArmRT
	Armutl_AsmFileList
	Armutl_ArmVersion
	Armutl_RVCTMajorVersion
	Armutl_RVCTMinorVersion
	Armutl_RVCTPatchLevel
	Armutl_RVCTBuildNumber
	Armutl_ArmLibDir
);

use strict;
use RVCT_plat2set;

my $ArmInc='';
my @ArmLibList=();
my $ArmRT=0;
my @AsmFileList=();

# make sure that some of the tool subroutines  still work in case of Plat() does not exists in namespace main
my $Plat = main::Plat() if defined &main::Plat;

my ($RVCTMajorVersion, $RVCTMinorVersion, $RVCTBuildNumber) = RVCT_plat2set::get_version_list($Plat);

my $RVCTVersion = "$RVCTMajorVersion.$RVCTMinorVersion";
my $RVCTPatchLevel = 0;


sub Armutl_Help_Mmp {
# provide the help text for START <armedg platforms> END blocks

	print
		"ARMINC  // include value of RVCT*INC environment variable to search paths\n",
		"ARMLIBS // list the ARM supplied libraries to be linked against\n",
		"ARMRT   // indicates this target froms part of the runtime and so\n",
                "        // shouldn't be linked against itself or other runtime libs\n",
	        "ARMNAKEDASM // list .cpp files subject to auto-translation from GCC inline asm to ARM embedded asm\n"
	;
}

sub Armutl_DoMmp (@) { # takes platform text
	my @PlatTxt=@_;

	my $BaseTrg=&main::BaseTrg;
	my $BasicTrgType=&main::BasicTrgType;
	my $MakeFilePath=&main::MakeFilePath;
	my $MMPFILE=&main::MmpFile;
	my @UidList=&main::UidList;

	# set up START ARMV5|THUMB2 ... END block module variables
	my @MmpWarn=();
	my $Line;

	LINE: foreach $Line (@PlatTxt) {
		my $LineInfo=shift @$Line;
		$_=shift @$Line;
		if (/^ARMINC$/o) {
			$ArmInc = RVCT_plat2set::get_inc_path($Plat);
			next LINE;
		}
		if (/^ARMRT$/o) {
		    $ArmRT = 1;
		    next LINE;
		}
		if (/^ARMLIBS$/o) {
			my $LibVar = "RVCT${RVCTMajorVersion}${RVCTMinorVersion}LIB";
			my $ArmLibDir = RVCT_plat2set::get_lib_path($Plat);

			push @MmpWarn, "$LineInfo : No libraries specified for keyword ARMLIBS\n" unless @$Line;

			while (@$Line) {
			  my $lib = shift @$Line;

              my $lib_path = RVCT_plat2set::find_lib( $Plat, $lib );

              if ($lib_path)
              {
				  push @ArmLibList, $lib_path;
              }
              else
              {
				  push @MmpWarn, "$LineInfo : arm library file $lib not found.\n" ;
              }
			}
			next LINE;
		      }
		if (/^ARMNAKEDASM$/o) {
		    push @MmpWarn, "$LineInfo : No files specified for keyword ARMNAKEDASM\n" unless @$Line;
		    push @AsmFileList, @$Line;
		    next LINE;
		}
		push @MmpWarn, "$LineInfo : Unrecognised Keyword \"$_\"\n";
	}

	undef $Line;
	if (@MmpWarn) {
		warn
			"\nMMPFILE \"$MMPFILE\"\n",
			"START .. END BLOCK WARNINGS(S)\n",
			@MmpWarn,
			"\n"
		;
	}
	undef @MmpWarn;
    }

sub Armutl_ArmIncDir() {
    $ArmInc;
}

sub Armutl_ArmLibList() {
    @ArmLibList;
}

sub Armutl_ArmRT() {
    $ArmRT;
}

sub Armutl_AsmFileList() {
    @AsmFileList;
}

sub Armutl_ArmVersion() {
  return $RVCTVersion;
}

sub Armutl_RVCTMajorVersion() {
  return $RVCTMajorVersion;
}

sub Armutl_RVCTMinorVersion() {
  return $RVCTMinorVersion;
}

sub Armutl_RVCTPatchLevel() {
  return $RVCTPatchLevel;
}

sub Armutl_RVCTBuildNumber() {
  return $RVCTBuildNumber;
}

sub Armutl_ArmLibDir() {
  my $LibVar = "RVCT${RVCTMajorVersion}${RVCTMinorVersion}LIB";
  my $ArmLibDir = $ENV{$LibVar};
  return $ArmLibDir;
}

1;





