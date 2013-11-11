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


package Cl_mingw;

my $MinGWInstall='$(EPOCROOT)epoc32\\gcc_mingw\\bin';
my $MinGWPreInclude='$(EPOCINC)\\gcc_mingw\\gcc_mingw.h';

my $GccPrefix='';
my $ToolPrefix='';
my $HelperLib='';
my %PlatOpt=(
	'Dlltool'=>'',
    'Gcc'=>'',
	'Ld'=>'',
	# MinGW > 3.2.3 has been found to generate programs that crash (rombuild in particular) 
	# and a workaround this appears to be to use specific optimisations rather than
	# specifying -O options
	'Optimize'=>'-fdefer-pop -fmerge-constants '.
	  '-fthread-jumps -floop-optimize '.
	  '-fif-conversion -fif-conversion2 '.
	  '-fguess-branch-probability -fcprop-registers '.
          '-fforce-mem -foptimize-sibling-calls -fstrength-reduce '.
	  '-fcse-follow-jumps  -fcse-skip-blocks '.
          '-frerun-cse-after-loop  -frerun-loop-opt '.
          '-fgcse  -fgcse-lm  -fgcse-sm  -fgcse-las '.
          '-fdelete-null-pointer-checks '.
          '-fexpensive-optimizations '.
          '-fregmove '.
          '-fschedule-insns  -fschedule-insns2 '.
          '-fsched-interblock  -fsched-spec '.
          '-fcaller-saves '.
          '-fpeephole2 '.
          '-freorder-blocks  -freorder-functions '.
          '-fstrict-aliasing '.
          '-funit-at-a-time '.
          '-falign-functions  -falign-jumps '.
          '-falign-loops  -falign-labels '.
          '-fcrossjumping'

);
my $Dlltool;
my $Archive;
my $Link;
my $Objcopy;

my @Win32LibList=();
my $Win32StdHeaders;


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
use cl_generic;
use Genutl;
use Winutl;

sub PMHelp_Mmp {
}

sub PMCheckPlatformL {
	if ((&main::Plat eq 'TOOLS2') and (&main::BasicTrgType ne 'EXE') and (&main::BasicTrgType ne 'LIB'))
		{
		die "Can't specify anything but EXE or LIB TARGETTYPEs for this platform\n";
		}
}

sub PMPlatProcessMmp (@) {
	&Winutl_DoMmp(\@_, "$ENV{EPOCROOT}epoc32\\gcc_mingw\\include");
	@Win32LibList=&Winutl_Win32LibList;
	$Win32StdHeaders=&Winutl_Win32StdHeaders;
}

my $Makecmd;

sub PMStartBldList($) {
	($Makecmd) = @_;
	my $BaseTrg=&main::BaseTrg;
	my $BasicTrgType=&main::BasicTrgType;
	my @BldList=&main::BldList;
	my @SysIncPaths=&main::Path_Chop(&main::SysIncPaths);
	my @UserIncPaths=&main::Path_Chop(&main::UserIncPaths);

	my $EPOCPath=&main::EPOCPath;
	my $LibPath=&main::LibPath;

	my @MacroList=&main::MacroList();

	my $VariantFile=&main::VariantFile();
	my $Plat=&main::Plat;
	my $Trg=&main::Trg;
	my $TrgType=&main::TrgType;

	my $ExportLibrary=&main::ExportLibrary;
	my $NoExportLibrary=&main::NoExportLibrary;
	my $CompilerOption=&main::CompilerOption("GCC");
	my $LinkerOption=&main::LinkerOption("GCC");

	$Dlltool=$ToolPrefix.'$(DLLTOOL)';
	$Archive=$ToolPrefix.'$(AR)';
	$Link=$ToolPrefix.'$(GCC)';
	$Objcopy=$ToolPrefix.'$(OBJCOPY)';

	&Generic_Header(0, $Makecmd);	# define standard things using absolute paths
	
	push @MacroList, "__TOOLS__";
	push @MacroList, "__TOOLS2_WINDOWS__";
	push @MacroList, "__SUPPORT_CPP_EXCEPTIONS__";
	push @MacroList, "__PRODUCT_INCLUDE__=\\\"${VariantFile}\\\"" if ($VariantFile);

	# Remove __GCC32__ for TOOLS2 builds from the macro list.
	my $count = 0;
	foreach my $item (@MacroList) {
		if ($item =~ m/GCC32/i) {
			splice (@MacroList,$count,1)
		}
		$count++;
	}


#	GCC needs a fully-qualified path
	&main::Output(
		"\n",
		"# must set both PATH and Path to make it work correctly\n",
		"Path:=\$(Path)\n",
		"PATH:=\$(Path)\n",
		"\n"
	);

	&main::Output(
		"INCDIR= -isystem \"$ENV{EPOCROOT}epoc32\\include\\tools\\stlport\" "
	);

	foreach (@SysIncPaths) {
		&main::Output(
			" -isystem \"$_\""
		);
	}

	foreach (@UserIncPaths) {
		&main::Output(
			" -I \"$_\""
		);
	}

	if($VariantFile){
	    &main::Output("\\\n  -include $MinGWPreInclude");
	}



	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"GCCFLAGS=$PlatOpt{Gcc} \\\n",
		"\t\t-pipe -c -Wall -W -Wno-ctor-dtor-privacy -Wno-unknown-pragmas",
	);
	
	&main::Output(
		" $CompilerOption",		# user-supplied compiler options
		"\n",
		"\n"
	);
	
	&main::Output(
 		"\n# ADDITIONAL LINKER OPTIONS",
 		"\nUSERLDFLAGS = ",
 		$LinkerOption,
  		"\n\n"
  	);

	&main::Output(
		"GCCDEFS="
	);

	foreach(@MacroList) {
		&main::Output(
			" -D$_"
		);
	}

	&main::Output(
		" \$(USERDEFS)\n",
		"\n"
	);

	&main::Output(
		"MINGWPATH?=$MinGWInstall", "\n",
		"DLLTOOL:=\$(MINGWPATH)\\dlltool", "\n",
		"AR:=\$(MINGWPATH)\\ar", "\n",
		"OBJCOPY:=\$(MINGWPATH)\\objcopy", "\n",
		"GCC:=\$(MINGWPATH)\\g++", "\n" 
	);
	foreach (@BldList) {
		&main::Output(
			"GCC$_=\$(GCC)"
		);
		if (/^REL$/o) {
			&main::Output(' -s ', $PlatOpt{Optimize});
		}
		elsif (/^DEB$/o) {
			&main::Output(' -g');
			unless (&main::SrcDbg) {
				&main::Output(' ', $PlatOpt{Optimize});
			}
		}
		&main::Output(' $(GCCFLAGS)');
		foreach (&main::MacroList($_)) {
			&main::Output(" -D$_");
		}
		&main::Output(" \$(GCCDEFS)\n");
	}

	&main::Output("\n","\n");

	foreach (@BldList) {
		&main::Output("$_ :");

		if ($BasicTrgType !~ /IMPLIB/io) {
			&main::Output (" \\\n\t",&Generic_Quote("\$(EPOCTRG$_)\\$Trg"));
		}
		&main::Output("\n","\n");
	}

	# Resource building is done entirely via cl_generic.pm

	foreach (@BldList) {
		&main::Output("\n",	"RESOURCE$_ : MAKEWORK$_");
	}
	&main::Output("\n", "\n",);

	&main::Output("LIBRARY : MAKEWORKLIBRARY");
	if ($BasicTrgType=~/^LIB$/o) {
#		code to ensure that the static libraries for all builds are built at the library stage
		foreach (@BldList) {
			&main::Output(" $_");
		}
	}

	&main::Output(
		"\n",
		"CLEANLIBRARY :\n"
	);

	&main::Output(
		"\n",
		"\n"
	);

	&Generic_MakeWorkDir('MAKEWORKLIBRARY',"${LibPath}LIB");

	&Generic_Releaseables;
}


sub PMBld {
	my @SrcList=&main::SrcList;
	my $BaseTrg=&main::BaseTrg;
	my $Bld=&main::Bld;
	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my $EPOCIncPath=&main::EPOCIncPath;
	my $BasicTrgType=&main::BasicTrgType;
	my @LibList;
	my $LibPath=&main::LibPath;
	my $ChopRelPath=&main::Path_Chop(&main::RelPath);
	my $RelPath=&main::RelPath;
	my @StatLibList=&main::StatLibList;
	my $StatLinkPath=&main::StatLinkPath;
	my $Trg=&main::Trg;
	my $TrgType=&main::TrgType;
	my $ExportLibrary=&main::ExportLibrary;
	my $NoExportLibrary=&main::NoExportLibrary;

	if ($Bld =~ /DEB/) {
		@LibList = &main::DebugLibList;
	} else {
		@LibList = &main::LibList;
	}

	# REAL TARGETS
	#-------------
	&main::Output(
		"# REAL TARGET - BUILD VARIANT $Bld\n",
		"\n"
	);

#	releasables
	my @releaseables;
	my $toolspath=&main::EPOCToolsPath();

	if ($BasicTrgType!~/^LIB$/io)
		{
		push @releaseables, "$toolspath$Trg";
		}
	else
		{
		push @releaseables, "$RelPath$Trg";
		}

	if (!$NoExportLibrary && ($BasicTrgType!~/^(LIB|EXE)$/io)) {	
		push @releaseables, "\$(EPOCLINK$Bld)\\lib$ExportLibrary.a";
	}

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

	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopBldPath);
	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopRelPath);

	return if ($BasicTrgType=~/^IMPLIB$/io);

	&main::Output(
		"LISTING$Bld : MAKEWORK$Bld"
	);
	foreach (@SrcList) {
		my $BaseSrc = &main::Path_Split('Base', $_);
		my $Ext = &main::Path_Split('Ext', $_);
   		&main::Output(
			" \\\n\tLISTING$Bld$BaseSrc"
   		);
   	}
	&main::Output(
		"\n",
		"\n"
	);

	
	&main::Output(
		"OBJECTS$Bld="
	);
	foreach (@SrcList) {
		my $BaseSrc = &main::Path_Split('Base', $_);
		my $Ext = &main::Path_Split('Ext', $_);
		&main::Output(
			" \\\n\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o")
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		&Generic_Quote("\$(EPOCTRG$Bld)\\$Trg"), " : ",
		&Generic_Quote("\$(OBJECTS$Bld)")
	);
	&main::Output(
		" \$(LIBS$Bld)\n"
	);
#	Link target
	if ($BasicTrgType=~/^EXE/o) {
#		call g++ to link the target
		&main::Output(
			"\t$Link $PlatOpt{Ld}"
		);
		if ($Bld=~/^REL$/o) {
			&main::Output(
				" -s"
			);
		}
		
		&main::Output(
			" -o \"\$(EPOCTRG$Bld)\\$Trg\" \\\n",
			"\t\t\$(OBJECTS$Bld) \\\n"
		);
		&main::Output(
			"\t\t-L\$(EPOCTRG$Bld) \\\n"
		);
			
		# Add dynamic libraries (dlls on Windows, .so on *nix systems) to linker options
		if (scalar @LibList)
		{
			&main::Output("\t\t-Wl,-Bdynamic ");
		}
		my $libs="";
		foreach my $lib (@LibList)
		{
			$libs.="-l$lib ";
		}
		if ($libs ne "")
		{
			&main::Output(
				"$libs \\\n"
			);
		}
		
		# Add static libraries (.a on both Windows and *nix) to linker options
		if (scalar @StatLibList)
		{
			&main::Output("\t\t-Wl,-Bstatic ");
		}
		my $statLibs="-lstlport.5.1 ";
		foreach my $lib (@StatLibList)
		{
			$statLibs.="-l$lib ";
		}
		if ($statLibs ne "")
		{
			$statLibs.="\\\n";
		}
		&main::Output(
			"$statLibs"
		);
		
		&main::Output(
			"\t\t \$(USERLDFLAGS)\n"
		);
		&main::Output(
			"\tcopy \"\$(EPOCTRG$Bld)\\$Trg\" \"",&main::EPOCToolsPath,"$Trg\"\n"
		);
	}
	elsif ($BasicTrgType=~/^LIB$/o) {
		&main::Output(
			"\tif exist \"\$\@\" del \"\$\@\"\n",
			"\t$Archive cr \$\@ \$^\n"
		);
	}
	elsif ($BasicTrgType =~ /^DLL$/o)
	{
		&main::Output(
			"\t$Link -shared -o \$@ -Wl,--out-implib,\$(EPOCLINK$Bld)\\" . "lib" . "$ExportLibrary.a \$(OBJECTS$Bld)",
			"\n"
		);
		&main::Output(
			"\tcopy \"\$@\" \"",&main::EPOCToolsPath,"$Trg\"\n"
		);

	}


	&main::Output(
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
	my $ExtSrc=&main::ExtSrc;

	return if (@DepList == 0);

	foreach (@BldList) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseSrc.lis"), " ",
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
	my $ExtSrc=&main::ExtSrc;

	return if (@DepList == 0);

	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " ",
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
	my $Bld=&main::Bld;
	my $Src=&main::Src;
	my $SrcPath=&main::Path_Chop(&main::SrcPath);
	my $Ext=&main::Path_Split('Ext', $Src);
	my $Plat=$main::Plat;

	# Use GCC trick to get assembler source files preprocessed with CPP
	$Src =~ s/\.s$/.S/i;	
	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"), " : ",
		&Generic_Quote("$SrcPath\\$Src"), "\n",
		"\techo $Src\n",
		"\t\$(GCC$Bld) -I \"$SrcPath\" \$(INCDIR) -o \$\@ \"$SrcPath\\$Src\"\n",
		"\n",
#		generate an assembly listing target too
		"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), "\n",
		"\t", &Generic_CopyAction("$SrcPath\\$BaseSrc.tools2.lst"),
		"\n",
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " : ",
		&Generic_Quote("$SrcPath\\$Src"), "\n",
		"\t\$(GCC$Bld) -Wa,-adln -I \"$SrcPath\" \$(INCDIR) -o nul: \"$SrcPath\\$Src\" > \$\@\n",
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

	# Deal with accumulated MAKEDIRS etc.

	&Generic_End;
}

1;
