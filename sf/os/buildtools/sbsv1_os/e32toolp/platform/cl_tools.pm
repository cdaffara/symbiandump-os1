# Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package Cl_tools;

# declare variables global for module
my @Win32LibList=();
my $BaseAddressFlag;
my $Win32Resrc;

my $MWCC;
my $MWLD;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	PMHelp_Mmp
	PMCheckPlatformL
	PMPlatProcessMmp
	PMStartBldList
	PMBld
	PMStartSrcList
	PMStartSrc
	PMSrcDepend
	PMSrcBldDepend
	PMEndSrcBld
	PMEndSrc
	PMEndSrcList
);

use strict;
use Winutl;
use cl_generic;

sub PMHelp_Mmp {
	&Winutl_Help_Mmp;
}

sub PMCheckPlatformL {
	if ((&main::Plat eq 'TOOLS') and (&main::BasicTrgType ne 'EXE') and (&main::BasicTrgType ne 'LIB')) {
		die "Can't specify anything but EXE or LIB targettypes for this platform\n";
	}
}

sub PMPlatProcessMmp (@) {
	&Winutl_DoMmp(\@_, $ENV{MWCSym2Includes});
	$BaseAddressFlag=&Winutl_BaseAddress;
	if ($BaseAddressFlag ne "") {
		$BaseAddressFlag=" -imagebase \"$BaseAddressFlag\"";
	}
	@Win32LibList=&Winutl_Win32LibList;
	$Win32Resrc=&Winutl_Win32Resrc;
}

sub PMStartBldList($) {
	my ($makecmd) = @_;
	my $BaseTrg=&main::BaseTrg;
	my @BldList=&main::BldList;
	my @ChopSysIncPaths=&main::Path_Chop(&main::SysIncPaths);
	my @ChopUserIncPaths=&main::Path_Chop(&main::UserIncPaths);
	my $DefFile=&main::DefFile;
	my $BasicTrgType=&main::BasicTrgType;
	my $LibPath=&main::LibPath;
	my @MacroList=&main::MacroList();
	my $VariantFile=&main::VariantFile();
	my $Plat=&main::Plat;
	my $Trg=&main::Trg;
	my $TrgType=&main::TrgType;
	my $WarningLevel=&main::CompilerOption("CW");

	# check version of CodeWarrior for Symbian OS
	if (defined($ENV{MWSym2Libraries})) {
		my $allthere=1;
		foreach (split/;/,$ENV{MWSym2Libraries}) {
			s/^\+//;	# remove leading "+", if any
			if (!-d $_) {
				$allthere=0;
				last;
			}
		}
		if ($allthere) {
			$MWCC = "mwccsym2.exe";
			$MWLD = "mwldsym2.exe";
			print "Detected CodeWarrior for Symbian OS, version 2.0\n";
		}
	}
	if (!defined($MWCC)) {
		if (defined($ENV{CWFolder}) && -d $ENV{CWFolder}) {
			$MWCC = "mwcc.exe";
			$MWLD = "mwld.exe";
			print "Detected CodeWarrior for Symbian OS, version 1.0\n";
			die "ERROR: This version of CodeWarrior is not suitable for Symbian OS version 8\n";
		}
		else {
			die "ERROR: Unable to identify a valid CodeWarrior for Symbian OS installation\n";
		}
	}
	
	Generic_Header(0,$makecmd);	# define standard things using absolute paths

	my $TrgDir="";
	if (&Generic_Definition("TRGDIR") ne "") {
		$TrgDir="\$(TRGDIR)\\";
	}

# Change - winutl.pm checks MWCWinx86Includes, but mwccsym2.exe seems to use MWCIncludes
	if ($MWCC eq "mwccsym2.exe" && !defined($ENV{MWCIncludes})) {
		&main::Output(
			"\n",
			"MWCIncludes:=\$(MWCSym2Includes)\n",
			"export MWCIncludes\n",
			"\n"
		);
	}
		
	&main::Output(
		"# EPOC DEFINITIONS\n",
		"\n",
		"INCDIR  = -cwd source -i-"
	);
	foreach (@ChopUserIncPaths,@ChopSysIncPaths) {
		&main::Output(
			" \\\n -i \"$_\""
		);
	}
	use Pathutl;
	if($VariantFile){
	    my $variantFilePath = Path_Split('Path',$VariantFile);
	    $VariantFile  = Path_Split('FILE',$VariantFile);
	    # workaround for codewarrior defect:
	    # codewarrior at the moment doesn't accept an absolute path with the
	    # the -include option unless it contains a drive letter, this is solved
	    # by including the directory and the file separately
	    &main::Output("\\\n -i \"$variantFilePath\" -include \"$VariantFile\"");
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"CWFLAGS =",
		" -wchar_t off",	# do not treat wchar_t as a predefined type
		" -align 4",		# long alignment
		" -warnings on",	# selection of warnings
		" -w nohidevirtual",	# turn off "hides inherited virtual function" warning
		   ",nounusedexpr",	# turn off "expression has no side effect" warning
		" -msgstyle gcc",	# use GCC-style message format
		# " -strict on",		# strict ANSI/ISO C++ checking
		" -enum int",		# use int for enumeration types
		" -str pool",		# pool strings into a single data object
		" $WarningLevel"		
	
	);

	&main::Output(
		" -stdinc"		# insist because mwccsym2 has -nostdinc as the default(?)
	);

	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"CWDEFS  = "
	);
	foreach(@MacroList) {
		&main::Output(
			" -d \"$_\""
		);
	}
	&main::Output(
		" \$(USERDEFS)\n",
		"\n"
	);

	foreach (@BldList) {
		&main::Output(
			"CW$_ = $MWCC"
		);
		if (/DEB$/o) {
			&main::Output(
				' -g -O0'   # debug information, no optimisation
			);
		}
		elsif (/REL$/o) {
			&main::Output(
				' -O4,s'    # highest level of optimisation, optimise for space
			);
		}
		&main::Output(
			' $(CWFLAGS)'
		);
		foreach (&main::MacroList($_)) {
			&main::Output(
				" -d $_"
			);
		}
		&main::Output(
			" \$(CWDEFS) \$(INCDIR)\n"
		);
	}
	&main::Output(
		"\n",
		"\n"
	);
	
	foreach (@BldList) {
		&main::Output(
			"$_ :"
		);

		&main::Output (
			" \\\n",
			"\t", &Generic_Quote("\$(EPOCTRG$_)\\$TrgDir$Trg")
		);
		

#		lib has to come after the main target so that a .DEF file will be generated if the project is not frozen
		if ($DefFile and not &main::ExportUnfrozen) {
			&main::Output(
				" \\\n",
				"\tLIBRARY\n"
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
	}

	&main::Output(
		"LIBRARY : MAKEWORKLIBRARY",
		"\n"
	);
	&main::Output(
		"\n",
		"FREEZE :\n"
	);
	&main::Output(
		"\n",
		"CLEANLIBRARY :\n"
	);
	&Generic_MakeWorkDir('MAKEWORKLIBRARY',"${LibPath}UDEB");

	&Generic_Releaseables;
}

my $uidfile;
sub PMBld {

	my @SrcList=&main::SrcList;
	my $BaseTrg=&main::BaseTrg;
	my $Bld=&main::Bld;
	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my $DefFile=&main::DefFile;
	my $EPOCIncPath=&main::EPOCIncPath;
	my $FirstLib=&main::FirstLib;
	my $BasicTrgType=&main::BasicTrgType;
	my @LibList;
	my $RelPath=&main::RelPath;
	my $Trg=&main::Trg;
	my $TrgPath=&main::TrgPath;
	my $TrgType=&main::TrgType;

	$uidfile = "$BaseTrg.UID";

	if ($Bld =~ /DEB/) {
		@LibList=&main::DebugLibList;
	} else {
		@LibList=&main::LibList;
	}

	my $BLDTRGPATH = "\$(EPOCTRG$Bld)\\";
	if ($TrgPath) {
		$BLDTRGPATH .= "\$(TRGDIR)\\";	    # handles TARGETPATH
	}


#	releasables
	my @releaseables;
	
	my $toolspath=&main::EPOCToolsPath();
	push @releaseables, "$toolspath$Trg";

	&main::Output(
		"WHAT$Bld : WHATGENERIC\n",
		"\n",
		"CLEAN$Bld : CLEANBUILD$Bld CLEANRELEASE$Bld\n",
		"\n",
		"CLEANBUILD$Bld : \n",
		"\t\@perl -w -S ermdir.pl \"\$(EPOCBLD$Bld)\"\n",
		"\n",
		"CLEANRELEASE$Bld : CLEANGENERIC\n",
		"\n"
	);
	
	&Generic_WhatCleanTargets($Bld, "WHAT$Bld", "CLEANRELEASE$Bld", @releaseables);
	&Generic_MakeWorkDir("MAKEWORK$Bld", &main::BldPath);
	&Generic_MakeWorkDir("MAKEWORK$Bld", "$RelPath$TrgPath");

	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"LISTING$Bld : MAKEWORK$Bld"
	);
	foreach (@SrcList) {
		my $BaseSrc = &main::Path_Split('Base', $_);
		my $Ext = &main::Path_Split('Ext', $_);
		$BaseSrc = "uid" if ($BaseSrc eq $uidfile);
		
   		&main::Output(
			" \\\n\tLISTING$Bld$BaseSrc"
   		);
   	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"LIBS$Bld="
	);
	
	foreach (@LibList) {
		&main::Output(
			" \\\n\t", &Generic_Quote("\$(EPOCLINK$Bld)\\$_")
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"LINK_OBJS$Bld="
	);
	my $have_uidfile = 0;
	foreach (@SrcList) {
		my $BaseSrc = &main::Path_Split('Base', $_);
		my $Ext = &main::Path_Split('Ext', $_);
		if ($BaseSrc eq $uidfile) {
			$have_uidfile = 1;
			next;
		}
   		&main::Output(
			" \\\n",
			"\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o")
   		);
   	}
	
	if ($have_uidfile) {
		# ensure that the uid file is at the end of the list, as it's implicit in
		# CodeWarrior IDE projects.
		&main::Output(
			" \\\n",
			"\t", &Generic_Quote("\$(EPOCBLD$Bld)\\uid.o")
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::OutFormat(
		"COMMON_LINK_FLAGS$Bld=",
		' -msgstyle gcc',
		' -stdlib'		# use default runtime library for compiler help functions
	);
	if ($MWLD eq "mwcc.exe") {
		&main::OutFormat(
			' -warnings on'	# on by default in mwccsym2.exe
		);
	}

	&main::OutFormat(
		' -subsystem console'
	);
		
	if ($Bld=~/DEB$/o) {
		&main::OutFormat(
				' -g'
			);
		}
	
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"LINK_FLAGS$Bld= \$(COMMON_LINK_FLAGS$Bld) \$(LIBS$Bld) \\\n\t",
		" -o \"$BLDTRGPATH$Trg\"" 
	);
	
	&main::Output(
		' -noimplib'
	);

	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		&Generic_Quote("$BLDTRGPATH$Trg"), " : \$(LINK_OBJS$Bld) "
	);
	if (-e $DefFile) { # effectively "if project frozen ..."
		&main::Output(
			&Generic_Quote($DefFile)
		);
	}

	&main::Output(
		" \$(LIBS$Bld)\n"
	);

#	Perform the final link step
	&main::Output(
		"\t$MWLD "
	);
	
	&main::Output(
		"\$(LINK_FLAGS$Bld) \$(LINK_OBJS$Bld)\n",
	);

	&main::Output(
		"\tcopy \"$BLDTRGPATH$Trg\" \"",&main::EPOCToolsPath,"$Trg\"\n"
	);

	&main::Output(
		"\n",
		"\n"
	);
}

sub PMStartSrcList {

	&main::Output(
		"# SOURCES\n",
		"\n"
	);
}


sub PMStartSrc {
	my $Src=&main::Src;

	&main::Output(
		"# Source $Src\n",
		"\n"
	);
}

sub PMSrcDepend {
	my @BldList=&main::BldList;	
	my @DepList=&main::DepList;
	my $BaseSrc=&main::BaseSrc;
	$BaseSrc = "uid" if ($BaseSrc eq $uidfile);

	return if (@DepList == 0);

	foreach (@BldList) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseSrc.o"), " \\\n",
		);
	}
	&main::Output(
		":"
	);
	foreach (@DepList) {
		&main::Output(
		" \\\n\t", &Generic_Quote($_)
		);
	}
	&main::Output(
		"\n",
		"\n"
	);
}

sub PMSrcBldDepend {
	my $Bld=&main::Bld;
	my @DepList=&main::DepList;
	my $BaseSrc=&main::BaseSrc;
	$BaseSrc = "uid" if ($BaseSrc eq $uidfile);

	return if (@DepList == 0);

	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"), " :",
	);
	foreach (@DepList) {
		&main::Output(
			" \\\n\t", &Generic_Quote($_)
		);
	}
	&main::Output(
		"\n",
		"\n"
	);
}

sub PMEndSrcBld {
	my $BaseSrc=&main::BaseSrc;
	$BaseSrc = "uid" if ($BaseSrc eq $uidfile);
	my $Bld=&main::Bld;
	my $Plat=&main::Plat;
	my $Src=&main::Src;
	my $SrcPath=&main::SrcPath;

	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"), " : ",
		&Generic_Quote("$SrcPath$Src"), "\n",
		"\t\$(CW$Bld) -o \"\$\@\" -c \"$SrcPath$Src\"\n",
		"\n",
#		assembler listing target - uses implicit rule to do disassembly
		"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), "\n",
		"\t", &Generic_CopyAction("$SrcPath$BaseSrc.$Plat.lst"),
		"\n"
	);
}

sub PMEndSrc {

	&main::Output(
		"\n",
		"\n"
	);
}

sub PMEndSrcList {

	my $show_options = "source";
	$show_options = "source,unmangled,comments" if ($MWLD eq "mwldsym2.exe");
	
	&main::Output(
		"\n",
		"# Implicit rule for generating .lis files\n",
		"\n",
		".SUFFIXES : .lis .o\n",
		"\n",
		".o.lis:\n",
		"\t$MWLD -S -show $show_options \$< -o \$\@\n",
		"\n",
		"\n"
	);

	# Deal with accumulated MAKEDIRS etc.

	&Generic_End;
}

1;
