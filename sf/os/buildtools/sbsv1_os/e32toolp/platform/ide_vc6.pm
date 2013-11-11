# Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Makmake-module for creating MAKMAKE MSVC6 IDE makefiles shared by windows platform modules
# 
#

package Ide_vc6;

# declare variables global for module
my $BaseAddressFlag;
my @CppCall;
my %IdeBlds=();
my %PrjHdrs=();
my @Win32LibList=();
my $Win32Resrc;
my $Win32StdHeaders;
my $MSVCVer;

my $SupText='';


require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	PMHelp_Mmp

	PMCheckPlatformL

	PMPlatProcessMmp

	PMStartBldList
		PMBld
	PMEndBldList
	PMStartSrcList
			PMResrcBld
			PMDoc
			PMStartSrc
			PMSrcDepend
	PMEndSrcList
);

use strict;
use Winutl;
use Pathutl;

sub PMHelp_Mmp {
# get the windows help from WINUTL
	&Winutl_Help_Mmp;
}

sub PMCheckPlatformL {
	if ((&main::Plat eq 'TOOLS') and (&main::BasicTrgType ne 'EXE') and (&main::BasicTrgType ne 'LIB')) {
		die "Can't specify anything but EXE or LIB targettypes for this platform\n";
	}
}

sub PMPlatProcessMmp (@) {

	# get WINUTL to do the windows mmp file processing
	&Winutl_DoMmp(\@_, $ENV{INCLUDE});
	$BaseAddressFlag=&Winutl_BaseAddress;
	$BaseAddressFlag=~s/^(.+$)$/ \/base:\"$1\"/o;
	@Win32LibList=&Winutl_Win32LibList;
	my $MSVCVer = &Winutl_MSVCVer;
	push @Win32LibList, "kernel32.lib";
	$Win32Resrc=&Winutl_Win32Resrc;
	$Win32StdHeaders=&Winutl_Win32StdHeaders;
}

sub PMStartBldList($) {
	my ($makecmd) = @_;
	die "Cannot generate $makecmd makefiles\n" if ($makecmd ne "nmake");
	my $BaseTrg=&main::BaseTrg;
	my @BldList=&main::BldList;
	my $BldPath=&main::BldPath;
	my $ChopDataPath=&main::Path_Chop(&main::DataPath);
	my $ChopTrgPath=&main::Path_Chop(&main::TrgPath);
	my $DefFile=&main::DefFile;
	my $BasicTrgType=&main::BasicTrgType;
	my $ExportLibrary=&main::ExportLibrary;
	my $LibPath=&main::LibPath;

	my $MSVCVer = &Winutl_MSVCVer;

	my $LinkPath=&main::LinkPath;
	my $Plat=&main::Plat;
	my $RelPath=&main::RelPath;
	my $Trg=&main::Trg;
	my $StatLinkPath=&main::StatLinkPath;
	my $TrgType=&main::TrgType;


	# set up global IDE builds variable
	%IdeBlds= (
		UREL=> "$BaseTrg - Win32 Uni Release",
		UDEB=> "$BaseTrg - Win32 Uni Debug",
	);
	if (&main::Plat eq 'TOOLS') {
		%IdeBlds= (
			REL=> "$BaseTrg - Win32 Release",
			DEB=> "$BaseTrg - Win32 Debug",
		);
	}
	

#	Start the supplementary makefile

	$SupText.=join('',
		"\n",
#			need to set path here because MSDEV might be set up so that
#			it does not take account of the PATH environment variable
		"PATH=$ENV{PATH}\n",
		"\n",
		"# EPOC DEFINITIONS\n",
		"\n",
		"EPOCBLD = $BldPath #\n",
		"EPOCTRG = $RelPath #\n",
		"EPOCLIB = $LibPath #\n",
		"EPOCLINK = $LinkPath #\n",
		"EPOCSTATLINK = $StatLinkPath #\n",
		"\n"
	);
			
	if ($BasicTrgType eq 'DLL' || $TrgType eq 'EXEXP') {
		foreach (@BldList) {
			$SupText.=join('',
				"EPOCBLD$_ = \$(EPOCBLD)$_\n",
				"EPOCTRG$_ = \$(EPOCTRG)$_\n",
				"EPOCLIB$_ = \$(EPOCLIB)UDEB\n",
				"EPOCLINK$_ = \$(EPOCLINK)UDEB\n",
				"EPOCSTATLINK$_ = \$(EPOCSTATLINK)$_\n",
				"\n"
			);
		}
		$SupText.="\nTRGDIR = ";
		if ($Plat!~/^WINC$/o && $ChopTrgPath) {	# target path not allowed under WINC
			$SupText.=$ChopTrgPath;
		}
		else {
			$SupText.=".\\";
		}
		$SupText.="\n\nDATADIR = $ChopDataPath\n\n";


#		commands for creating the import library
		$SupText.="LIBRARY :";
		if ($DefFile) {
			unless (&main::ExportUnfrozen) {
				if (-e $DefFile) { # effectively "if project frozen ..."
					$SupText.=" \"\$(EPOCLIB)UDEB\\$ExportLibrary.lib\"\n";
				}
				else {
					$SupText.=join('',
						"\n",
						"\t\@echo WARNING: Not attempting to create \"\$(EPOCLIB)UDEB\\$ExportLibrary.lib\".\n",
						"\t\@echo When exports are frozen in \"$DefFile\", regenerate Makefile.\n"
					);
				}
			}
			else {
				$SupText.=join('',
					"\n",
					"\t\@echo Not attempting to create \"\$(EPOCLIB)UDEB\\$ExportLibrary.lib\"\n",
					"\t\@echo from frozen .DEF file, since EXPORTUNFROZEN specified.\n"
				);
			}
			$SupText.=join('',
				"\n",
				"\n",
				"# REAL TARGET - IMPORT LIBRARY\n",
				"\n",
				"\"\$(EPOCLIB)UDEB\\$ExportLibrary.lib\" : \"$DefFile\" MAKEWORKLIBRARY\n",
				"\tperl -S prepdef.pl \"$DefFile\" \"\$(EPOCBLD)\\$ExportLibrary.prep.def\"\n",
				"\tlib.exe /nologo /machine:i386 /nodefaultlib /name:\"$Trg\" /def:\"\$(EPOCBLD)\\$ExportLibrary.prep.def\" /out:\"\$(EPOCLIB)UDEB\\$ExportLibrary.lib\"\n",
				"\tdel \"\$(EPOCLIB)UDEB\\$ExportLibrary.exp\"\n"
			);
		}
		$SupText.=join('',
			"\n",
			"\n",
			"MAKEWORKLIBRARY : \"${LibPath}UDEB\"\n",
			"\n",
			"\"${LibPath}UDEB\" :\n"
		);
		$SupText.="\t\@perl -w -S emkdir.pl \"${LibPath}UDEB\"\n\n\n";
	}



	&main::Output(
		"# Microsoft Developer Studio Project File - Name=\"$BaseTrg\" - Package Owner=<4>\n",
		"# Microsoft Developer Studio Generated Build File, Format Version 6.00\n",
		"# ** DO NOT EDIT **\n",
		"\n"
	);

	if ($BasicTrgType=~/^DLL$/o) {
		&main::Output(
			"# TARGTYPE \"Win32 (x86) Dynamic-Link Library\" 0x0102\n"
		);
	}
	elsif ($BasicTrgType=~/^EXE$/o) {
		if (not &main::Plat=~/^(WINC|TOOLS)$/o) {
			&main::Output(
				"# TARGTYPE \"Win32 (x86) Application\" 0x0101\n"
			);
		}
		else {
			&main::Output(
				"# TARGTYPE \"Win32 (x86) Console Application\" 0x0103\n"
			);
		}
	}
	elsif ($BasicTrgType=~/^(LIB)$/o) {
		&main::Output(
			"# TARGTYPE \"Win32 (x86) Static Library\" 0x0104\n"
		);
	}
	
	&main::Output(
		"\n",
		"CFG=",$IdeBlds{$BldList[0]},"\n",
		"!MESSAGE This is not a valid makefile. To build this project using NMAKE,\n",
		"!MESSAGE use the Export Makefile command and run\n",
		"!MESSAGE \n",
		"!MESSAGE NMAKE /f \"",$BaseTrg,".mak\".\n",
		"!MESSAGE \n",
		"!MESSAGE You can specify a configuration when running NMAKE\n",
		"!MESSAGE by defining the macro CFG on the command line. For example:\n",
		"!MESSAGE \n",
		"!MESSAGE NMAKE /f \"",$BaseTrg,".mak\" CFG=\"".$IdeBlds{$BldList[0]}."\"\n",
		"!MESSAGE \n",
		"!MESSAGE Possible choices for configuration are:\n",
		"!MESSAGE \n"
	);
	&main::OutSetLength(79);
	if ($BasicTrgType=~/^DLL$/o) {
		foreach (reverse @BldList) { # reverse because VC6 takes last as the default
			&main::OutFormat(
				"!MESSAGE \"",$IdeBlds{$_},"\" (based on \"Win32 (x86) Dynamic-Link Library\")"
			);
			&main::Output();
		}
	}
	elsif ($BasicTrgType=~/^EXE$/o) {
		if (not &main::Plat=~/^(WINC|TOOLS)$/o) {
			foreach (reverse @BldList) {
				&main::OutFormat(
					"!MESSAGE \"",$IdeBlds{$_},"\" (based on \"Win32 (x86) Application\")"
				);
				&main::Output();
			}
		}
		else {
			foreach (reverse @BldList) {
				&main::OutFormat(
					"!MESSAGE \"",$IdeBlds{$_},"\" (based on \"Win32 (x86) Console Application\")"
				);
				&main::Output();
			}
		}
	}
	elsif ($BasicTrgType=~/^(LIB)$/o) {
		foreach (@BldList) {
			&main::OutFormat(
				"!MESSAGE \"",$IdeBlds{$_},"\" (based on \"Win32 (x86) Static Library\")"
			);
			&main::Output();
		}
	}
	&main::OutSetLength();
	&main::Output(
		"!MESSAGE \n",
		"\n"
	);

	# BEGIN THE PROJECT
	#------------------
	&main::Output(
		"# Begin Project\n",
		"# PROP Scc_ProjName \"\"\n",
		"# PROP Scc_LocalPath \"\"\n",
		"CPP=cl.exe\n",
		"MTL=midl.exe\n",
		"RSC=rc.exe\n",
		"\n"
	);
}

sub PMBld {

	my @BaseSrcList=&main::BaseSrcList;
	my $BaseTrg=&main::BaseTrg;
	my $Bld=&main::Bld;
	my @BldList=&main::BldList;
	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my $ChopLinkPath=&main::Path_Chop(&main::LinkPath);
	my $ChopRelPath=&main::Path_Chop(&main::RelPath);
	my $ChopStatLinkPath=&main::Path_Chop(&main::StatLinkPath);
	my @ChopSysIncPaths=&main::Path_Chop(&main::SysIncPaths);
	my @ChopUserIncPaths=&main::Path_Chop(&main::UserIncPaths);
	my $ExportLibrary=&main::ExportLibrary;
	my $DefFile=&main::DefFile;
	my $FirstLib=&main::FirstLib;
	my $BasicTrgType=&main::BasicTrgType;
	my @MacroList=&main::MacroList();
	push @MacroList, "__SUPPORT_CPP_EXCEPTIONS__";
    my $VariantFile=&main::VariantFile();


	my @LibList;
	my $PathBaseDsp=&main::MakeFilePath.&main::BaseMak;
	my @StatLibList=&main::StatLibList;
	my $Trg=&main::Trg;
	my $TrgPath=&main::TrgPath;
	my $TrgType=&main::TrgType;
	
	my $newlib = main::NewLib(); # Check if newlib has been set in the MMP file.
	my $NewLib = 'scppnwdl.lib'; # This is where operator new and operator delete
                                 # are defined for user side targets.
	my $NewKernLib = 'scppnwdl_kern.lib'; # This is where operator new and operator delete
                                 	  # are defined for kernel side targets.


	if ($Bld =~ /DEB/) {
		@LibList=&main::DebugLibList;
	} else {
		@LibList=&main::LibList;
	}

	my $NotUseWin32LibsFlag="";
	unless ($Win32StdHeaders or &main::Plat eq 'TOOLS') {
		$NotUseWin32LibsFlag=" /X"; # this flag suppresses searching of the standard directories for header files
	}


	if ($Bld eq $BldList[0]) {
		&main::Output(
			"!IF  \"\$(CFG)\" == \"$IdeBlds{$Bld}\"\n",
			"\n"
		);
	}
	else {
		&main::Output(
			"!ELSEIF  \"\$(CFG)\" == \"$IdeBlds{$Bld}\"\n",
			"\n"
		);
	}
	&main::Output(
		"# PROP BASE Use_MFC 0\n"
	);
	if ($Bld=~/REL$/o) {
		&main::Output(
			"# PROP BASE Use_Debug_Libraries 0\n",
			"# PROP BASE Output_Dir \".\\Win32_Un\"\n",
			"# PROP BASE Intermediate_Dir \".\\Win32_Un\"\n"
		);
	}
	elsif ($Bld=~/DEB$/o) {
		&main::Output(
			"# PROP BASE Use_Debug_Libraries 1\n",
			"# PROP BASE Output_Dir \".\\Win32_U0\"\n",
			"# PROP BASE Intermediate_Dir \".\\Win32_U0\"\n"
		);
	}
	&main::Output(
			"# PROP Use_MFC 0\n"
	);
	if ($Bld=~/REL$/o) {
		&main::Output(
			"# PROP Use_Debug_Libraries 0\n"
		);
	}
	elsif ($Bld=~/DEB$/o) {
		&main::Output(
			"# PROP Use_Debug_Libraries 1\n"
		);
	}
	&main::Output(
		"# PROP Output_Dir \"$ChopRelPath\"\n",
		"# PROP Intermediate_Dir \"$ChopBldPath\"\n",
	);

	&main::Output(
		"# ADD CPP",
		" /nologo",		# suppress "sign-on" banner message
		" /Zp4"	,		# packs structures on 4 byte boundaries
		" /GF"			# Pools strings and places them in read-only memory 
	);

	$MSVCVer = &Winutl_MSVCVer;

	if ($MSVCVer >= 7) {
		&main::Output(
			" /wd4996",			# C4996: 'xxxx' was declared deprecated
			" /wd4571"			# C4571: catch(...) blocks compiled with /EHs do not catch or re-throw Structured Exceptions
		);	

		if (&main::Plat ne 'TOOLS') {
			&main::Output(
				" /EHsc",			# Exceptions on
				" /GR"				# RTTI on
			);
			if ($MSVCVer >= 8) {
				&main::Output(
					" /GS-"			# Buffer checking off
				);
			}
		} else {
			&main::Output(
				" /EHsc",			# Exceptions on
				" /GR",				# RTTI on
				" /GS"				# Buffer checking on
			);
		}
	} else {
		if (&main::Plat ne 'TOOLS') {
			&main::Output(
					" /GX",				# Exceptions on
					" /GR"				# RTTI on
			);
		}
	}

	if ($BasicTrgType=~/^EXE$/o) {
		&main::Output(
			' /MT'			# Creates a multi-threaded executable file, using libcmt.lib
		);
		if ($Bld=~/DEB$/o) {
			&main::Output(
				"d"			# i.e. /MTd, debug executable using debug version of libcmt
			);
		}
	}
	elsif (($BasicTrgType=~/^(DLL|LIB)$/o) and (&main::Plat ne 'TOOLS')) {
		&main::Output(
			" /MD"			# Creates a multithreaded DLL, using MSVCRT.LIB
		);
		if ($Bld=~/DEB$/o) {
			&main::Output(
				"d"			# i.e. /MDd, debug executable using debug version of msvcrt
			);
		}
	}

	if ($MSVCVer >= 8) {
		&main::Output(
			" /Zc:wchar_t-"				# Don't make wchar_t a built-in; breaks mangled names
		);		
	}

	if (($MSVCVer >= 7) || (&main::Plat ne 'TOOLS')) {
		&main::Output(
			" /W4"
		);
	}

	if ($Bld=~/DEB$/o) {
		&main::Output(
			" /Zi",		# Generates complete debugging information
			" /Od"		# Disables optimization
		);
#		euser change to apply inlining on the _NAKED functions
		if ($BaseTrg=~/^EUSER$/oi) {
			&main::Output(
				' /Ob1'	# Specific control of expension of inline functions
			);
		}
	}
	elsif ($Bld=~/REL$/o) {
		&main::Output(
			' /O1'				# Creates small code
		);
		if ($MSVCVer >= 8) {
			&main::Output(
				' /fp:strict'		# Improves floating-point consistency
			);
		} else {
			&main::Output(
				' /Op'				# Improves floating-point consistency
			);
		}

	}
	&main::Output(
		"$NotUseWin32LibsFlag"
	);
	my @incList=();
	my $candidate;
	INCLOOP: foreach $candidate (@ChopUserIncPaths,@ChopSysIncPaths) {
		my $accepted;	# remove duplicates
		foreach $accepted (@incList) {
			if ($candidate eq $accepted) {
				next INCLOOP;
			}
		}
		push @incList, $candidate;
		&main::Output(
			" /I \"$candidate\""
		);
	}
	foreach (@MacroList) {
		&main::Output(
			" /D \"$_\""
		);
	}


   if($VariantFile){
        my $VariantFileFileName  = Path_Split('FILE',$VariantFile);
        &main::Output(
              " /FI\"$VariantFileFileName\""
              );
	}

	if ($Bld=~/DEB$/o) {
		&main::Output(
			" /FR /Fd\"$ChopRelPath\\$TrgPath$BaseTrg.PDB\""
		);
	}
	&main::Output(
		" /c\n",
	);

	if ($BasicTrgType=~/^DLL$/o) {
		&main::Output(
			"# ADD MTL /nologo"
		);
		if ($Bld=~/REL$/o) {
			&main::Output(
				" /mktyplib203 /D"
			);
		}
		elsif ($Bld=~/DEB$/o) {
			&main::Output(
				" /D \"_DEBUG\" /mktyplib203"
			);
		}
		&main::Output(
			" /win32\n"
		);
	}

	&main::Output(
		"# ADD BASE RSC /l 0x809\n",
		"# ADD RSC /l 0x809 /d"
	);
	if ($Bld=~/REL$/o) {
		&main::Output(
			" \"NDEBUG\""
		);
	}
	elsif ($Bld=~/DEB$/o) {
		&main::Output(
			" \"_DEBUG\""
		);
	}
	&main::Output(
		"\n"
	);

	&main::Output(
		"BSC32=bscmake.exe\n",
		"# ADD BSC32 /nologo\n",
	);

	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		&main::Output(
			"LINK32=link.exe\n",
			"# ADD BASE LINK32 /machine:IX86\n",
			"# ADD LINK32"
		);
		unless (&main::Plat eq 'TOOLS') {
			&main::Output(
				" \"$ChopStatLinkPath\\$FirstLib\""
			);
			
			unless ($newlib) {
				if ( main::SystemTrg() ) {
					# System targets are PDD, LDD, VAR, KEXT and KDLL.

					&main::Output(
						" \"$ChopStatLinkPath\\$NewKernLib\""
					);
				}
				else {
					&main::Output(
						" \"$ChopStatLinkPath\\$NewLib\""
					);
				}
			}
			else {
				&main::Output(
					" \"$ChopStatLinkPath\\$newlib\""
				);
			}
		}
	}
	elsif ($BasicTrgType=~/^LIB$/o) {
		&main::Output(
			"LIB32=link.exe -lib\n",
			"# ADD BASE LIB32 /machine:IX86\n",
			"# ADD LIB32"
		);
	}
	foreach (@Win32LibList) {
		&main::Output(
			" \"",lc $_,"\""
		);
	}
	foreach (@StatLibList) {
		&main::Output(
			" \"$ChopStatLinkPath\\",lc $_,"\""
		);
	}
	foreach (@LibList) {
		&main::Output(
			" \"$ChopLinkPath\\",lc $_,"\""
		);
	}
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		&main::Output(
			" \"$ChopBldPath\\$ExportLibrary.exp\""
		);
	}
	&main::Output(' /nologo');
	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		&main::Output(
			" /fixed:no $BaseAddressFlag"
		);
	}
	if (&main::HeapSize) {
		my %HeapSize=&main::HeapSize;
		&main::Output(
			' /heap:',$HeapSize{Max},',',$HeapSize{Min}
		);
	}
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		&main::Output(
			' /noentry /dll'
		);
	}
	elsif ($BasicTrgType=~/^EXE$/o) {
		unless (&main::Plat eq 'TOOLS') {
			&main::Output(
				' /entry:"?_E32Bootstrap@@YGXXZ"'
			);
		}
	}
	if (&main::Plat=~/^(WINC|TOOLS)$/o && $BasicTrgType=~/^EXE$/o) { # shouldn't really have WINC mentioned here
		&main::Output(
			' /subsystem:console'
		);
	}
	else {
		&main::Output(
			' /subsystem:windows'
		);
	}
	if ($BasicTrgType=~/^(DLL|EXE)$/o && $Bld=~/DEB$/o) {
		&main::Output(
#			pdb needs specifying here to be the same as the default applied in command-line builds
			" /pdb:\"$ChopRelPath\\$TrgPath$BaseTrg.pdb\" /debug"
		);
	}
	&main::Output(
		' /machine:IX86',
		' /ignore:4089',		# LNK4089: all references to "dynamic-link library" discarded by /OPT:REF
		' /ignore:4005'			# LNK4005: no objects used from XXX
	);

	if ($MSVCVer >= 7) {
		&main::Output(
			' /ignore:4210'	# LNK4210: .CRT section exists; there may be unhandled static initializers or terminators
		);
	}

	my $EntrySymbol='';
	unless (&main::Plat eq 'TOOLS') {
		if ($BasicTrgType=~/^EXE$/o) {
			my $debug = '';
			if ($Bld =~ /DEB$/o) {
				$debug .= 'd';
			}
			&main::Output(
				" /nodefaultlib:libcmt$debug.lib"
			);
		}
		if ($BasicTrgType=~/^DLL$/o) {
			&main::Output(
				" /include:\"__E32Dll\""
			);
			$EntrySymbol='_E32Dll';
		}
		elsif ($BasicTrgType=~/^EXE$/o) {
			&main::Output(
				" /include:\"__E32Startup\""
			);
			$EntrySymbol='_E32Startup';
		}
	} else {
		&main::Output(
			' /ignore:4098'	# LNK4098: defaultlib 'LIBCMT' conflicts with use of other libs; use /NODEFAULTLIB:library
		);
	}
	my $AbsentSubst = '';
	if ($EntrySymbol) {
		$AbsentSubst = " -absent $EntrySymbol";
	}
	&main::Output(
		" /out:\"$ChopRelPath\\$TrgPath$Trg\""
	);
	if ($MSVCVer <= 7) {
		if ($BasicTrgType=~/^(DLL|EXE)$/o) {
			&main::Output(
				' /WARN:3'
			);
		}
	}
	if ($BasicTrgType=~/^(DLL|EXE)$/o && $Bld=~/DEB$/o) {
		&main::Output(
			"\n",
			"# SUBTRACT LINK32 /pdb:none"
		);
	}
	&main::Output(
		"\n"
	);
	if (&main::Plat eq 'TOOLS' and $BasicTrgType eq 'EXE') {
		&main::Output(
			"# Begin Special Build Tool\n",
			"PostBuild_Cmds=copy \"$ChopRelPath\\$TrgPath$Trg\" \"",&main::EPOCToolsPath,"$Trg\" \n",
			"# End Special Build Tool\n"
		);
	}
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		&main::Output(
			"# Begin Special Build Tool\n",
			"SOURCE=\$(InputPath)\n",
			"PreLink_Cmds=echo Doing first-stage link by name\\\n"
		);
		&main::Output(
			"\tnmake -nologo -f \"${PathBaseDsp}.SUP.MAKE\" PRELINK$Bld\\\n",
			"\tif errorlevel 1 nmake -nologo -f \"${PathBaseDsp}.SUP.MAKE\" STOPLINK$Bld \n",
			"PostBuild_Cmds=nmake -nologo -f \"${PathBaseDsp}.SUP.MAKE\" POSTBUILD$Bld\n",
			"# End Special Build Tool\n"
		);


#		append to the supplementary makefile for each build
		$SupText.=join('',
			"# BUILD - $Bld\n",
			"\n",
			"LIBS="
		);
		foreach (@StatLibList) {
			$SupText.=" \\\n\t\"\$(EPOCSTATLINK$Bld)\\$_\"";
		}
		foreach (@LibList) {
			$SupText.=" \\\n\t\"\$(EPOCLINK$Bld)\\$_\"";
		}
		$SupText.="\n\nLINK_OBJS=";
		foreach (@BaseSrcList) {
			$SupText.=" \\\n\t\"\$(EPOCBLD$Bld)\\$_.obj\"";
		}
		if ($Win32Resrc) {
			$SupText.=" \\\n\t\"\$(EPOCBLD$Bld)\\".&main::Path_Split('Base',$Win32Resrc).".res\"";
		}
		$SupText.="\n\nSTAGE1_LINK_FLAGS=\"\$(EPOCSTATLINK$Bld)\\$FirstLib\"";
		
		unless ($newlib) {
			if ( main::SystemTrg() ) {
				# System targets are PDD, LDD, VAR, KEXT and KDLL.

				$SupText.=" \"\$(EPOCSTATLINK$Bld)\\$NewKernLib\"";
			}
			else {
				$SupText.=" \"\$(EPOCSTATLINK$Bld)\\$NewLib\"";
			}
		}
		else {
			$SupText.=" \"\$(EPOCSTATLINK$Bld)\\$newlib\"";
		}
		
		foreach (@Win32LibList) {
			$SupText.=' ';
			$SupText.=lc $_;
		}
		$SupText.=" \\\n \$(LIBS) /nologo$BaseAddressFlag /noentry /dll /subsystem:windows";
		if ($Bld=~/DEB$/o) {
			$SupText.=' /debug';
		}
		$SupText.=" \\\n /incremental:no /machine:IX86 /ignore:4089";
		if ($BasicTrgType=~/^EXE$/o) {
			my $debug = '';
			if ($Bld =~ /DEB$/o) {
				$debug .= 'd';
			}
			$SupText .= " /nodefaultlib:libcmt$debug.lib";
		}
		if ($MSVCVer >= 7) {
			$SupText .=  " /ignore:4210";
		}
		if ($BasicTrgType=~/^DLL$/o) {
			$SupText.=" /include:\"__E32Dll\"";
		}
		else {
			$SupText.=" /include:\"__E32Startup\"";
		}
		if ($BasicTrgType=~/^(DLL)$/o) {
			$SupText.=" /implib:\"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"";
		} elsif ($BasicTrgType=~/^(EXE)$/o) {
			$SupText.=" /implib:\"\$(EPOCBLD$Bld)\\$ExportLibrary.exe.lib\"";
		}
		$SupText.=join('',
			' /ignore:4089',		# LNK4089: all references to "dynamic-link library" discarded by /OPT:REF
			' /ignore:4005'			# LNK4005: no objects used from XXX
		);
		if ($MSVCVer >= 7) {
			$SupText.=" /ignore:4210";		# LNK4210: .CRT section exists; there may be unhandled static initializers or terminators
		}
		$SupText.=" /out:\"\$(EPOCBLD$Bld)\\$Trg\"";
		if ($MSVCVer < 7) {
			$SupText .= " /WARN:3";
		}
		$SupText.="\n\n";
		$SupText.="PRELINK$Bld : \$(LINK_OBJS)";
		if (-e $DefFile) { # effectively "if project frozen ..."
			$SupText.=" \"$DefFile\"";
		}
		unless (&main::Plat eq 'TOOLS') {
			$SupText.=" \"\$(EPOCSTATLINK$Bld)\\$FirstLib\"";
			
			unless ($newlib) {
				if ( main::SystemTrg() ) {
					# System targets are PDD, LDD, VAR, KEXT and KDLL.

					$SupText.=" \"\$(EPOCSTATLINK$Bld)\\$NewKernLib\"";
				}
				else {
					$SupText.=" \"\$(EPOCSTATLINK$Bld)\\$NewLib\"";
				}
			}
			else {
				$SupText.=" \"\$(EPOCSTATLINK$Bld)\\$newlib\"";
			}
		}
		$SupText.=" \$(LIBS)\n";

#		Link by name first time round for dlls
		$SupText.=join('',
			"\tlink.exe \@<<\n",
			"\t\t\$(STAGE1_LINK_FLAGS) \$(LINK_OBJS)\n",
			"<<\n",
			"\tdel \"\$(EPOCBLD$Bld)\\$Trg\"\n"
		);

		if ($BasicTrgType=~/^(DLL)$/o) {
			$SupText.="\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\"\n";
		} elsif ($BasicTrgType=~/^(EXE)$/o) {
			$SupText.="\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.exe.exp\"\n";
		}

#		Generate an export info file
		if ($BasicTrgType=~/^(DLL)$/o) {
			$SupText.=join('',
				"\tdumpbin /exports /out:\"\$(EPOCBLD$Bld)\\$ExportLibrary.inf\" \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n",
				"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n"
			);
		} elsif ($BasicTrgType=~/^(EXE)$/o) {
			$SupText.=join('',
				"\tdumpbin /exports /out:\"\$(EPOCBLD$Bld)\\$ExportLibrary.inf\" \"\$(EPOCBLD$Bld)\\$ExportLibrary.exe.lib\"\n",
				"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.exe.lib\"\n"
			);
		}

#		call makedef to reorder the export information
#		call perl on the script here so nmake will die if there are errors - this doesn't happen if calling perl in a batch file
		$SupText.="\tperl -w -S makedef.pl $AbsentSubst -Inffile \"\$(EPOCBLD$Bld)\\$ExportLibrary.inf\"";
		if (-e $DefFile) { # effectively "if project frozen ..."
			$SupText.=" -Frzfile \"$DefFile\"";
		}
		# freeze ordinals, a maximum of 2, for polymorphic dlls
		my $Ordinal;
		my $Num=1;
		foreach $Ordinal (&main::Exports) {
#			replace "$" with "$$" so that NMAKE doesn't think there's a macro in the function name
			$Ordinal=~s-\$-\$\$-go;
			$SupText.=" -$Num $Ordinal";
			$Num++;
		}
		$SupText.=join('',
			" \"\$(EPOCBLD)$ExportLibrary.def\" \n",
			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.inf\"\n"
		);

		# create the export object from the .DEF file
		$SupText.="\tlib.exe  /nologo /machine:i386 /nodefaultlib /name:\"$Trg\" /def:\"\$(EPOCBLD)$ExportLibrary.def\" /out:\"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n";
		if (&main::ExportUnfrozen) {
			$SupText.="\tcopy \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\" \"\$(EPOCLIB)UDEB\\$ExportLibrary.lib\"\n";
		}
		$SupText.=join('',
			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n",
			"\t\@echo First-stage link successful\n",
			"\n",
			"\n",
			"STOPLINK$Bld : DELEXPOBJ$Bld\n",
			"\t\@echo Stopped the build by removing the export object,\n",
			"\t\@echo if present, because the pre-link stage failed\n",
			"\n",
			"\n",
			"POSTBUILD$Bld : DELEXPOBJ$Bld"
		);
		if ($DefFile and not &main::ExportUnfrozen) {
			$SupText.=" LIBRARY";
		}
		$SupText.=join('',
			"\n",
			"\n",
			"\n",
			"DELEXPOBJ$Bld :\n"
		);

		if ($BasicTrgType=~/^(DLL)$/o) {
			$SupText.="\tif exist \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\" del \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\"\n";
		} elsif ($BasicTrgType=~/^(EXE)$/o) {
			$SupText.="\tif exist \"\$(EPOCBLD$Bld)\\$ExportLibrary.exe.exp\" del \"\$(EPOCBLD$Bld)\\$ExportLibrary.exe.exp\"\n";
		}

		$SupText.=join('',
			"\n",
			"\n",
			"\n"
		);
	}

	&main::Output(
		"\n"
	);
}

sub PMEndBldList {

	# CLOSE THE !IF ... !ENDIF LOOP
	#------------------------------
	&main::Output(
		"!ENDIF \n",
		"\n"
	);
}

sub PMStartSrcList {

	my @BldList=&main::BldList;

	&main::Output(
		"# Begin Target\n",
		"\n"
	);
	foreach (@BldList) {
		&main::Output(
			"# Name \"".$IdeBlds{$_}."\"\n"
		);
	}
	&main::Output(
		"# Begin Group \"Source Files\"\n",
		"\n",
		"# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90\"\n"
	);
}

sub PMResrcBld {

	my $ResourceRef=&main::ResourceRef;
	my $Resrc=ucfirst lc &main::Path_Split('File', $$ResourceRef{Source});
	my $BaseResrc=&main::Path_Split('Base', $$ResourceRef{Source});
	my $SrcPath=&main::Path_Split('Path', $$ResourceRef{Source});
	my $TrgPath=&main::Path_Split('Path', $$ResourceRef{Trg});
	my @LangList=($$ResourceRef{Lang});
	
	my $inputpath="$SrcPath$Resrc";
	
	&main::Output(
		"# Begin Source File\n",
		"\n",
		"SOURCE=$inputpath\n"
	);

	my @BldList=&main::BldList;
	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my @ChopSysIncPaths=&main::Path_Chop(&main::SysIncPaths);
	my @ChopUserIncPaths=&main::Path_Chop(&main::UserIncPaths);
	my $DataPath=&main::DataPath;
	my @DepList=&main::DepList;
	my $PathBaseDsp=&main::MakeFilePath.&main::BaseMak;
	my $PerlLibPath=&main::PerlLibPath;
	my $RelPath=&main::RelPath;
	my $ChopSrcPath=&main::Path_Chop($SrcPath);

	my $ResrcHdr=join '', &main::EPOCIncPath(), $BaseResrc, '.RSG';

	&main::OutFormat(
		"USERDEP__$BaseResrc="
	);
	$SupText.="DEPEND=";
	my $Dep;
	foreach $Dep (@DepList) {
		&main::OutFormat(
			"\"$Dep\"\t"
		);
		$SupText.=" \\\n\t\"$Dep\"";
	}
	&main::Output(
		"\n"
	);
	$SupText.="\n\n";

	my $Bld;
	foreach $Bld (@BldList) {
		my $ResrcTrgFullName="$RelPath$Bld\\$TrgPath$BaseResrc.r";
		if ($Bld eq $BldList[0]) {
			&main::Output(
				'!IF'
			);
		}
		else {
			&main::Output(
				'!ELSEIF'
			);
		}
		&main::Output(
			"  \"\$(CFG)\" == \"$IdeBlds{$Bld}\"\n",
			"\n"
		);
		&main::Output(
			"# PROP Intermediate_Dir \"$ChopBldPath\"\n",
			"# Begin Custom Build - Building resources from $Resrc\n",
			"InputPath=$inputpath\n",
			"\n",
			"BuildCmds= \\\n",
			"\tnmake -nologo -f \"${PathBaseDsp}.SUP.MAKE\"\\\n",
			"  \"$ResrcTrgFullName\"\n"
		);
		$SupText.="\"$ResrcTrgFullName\" :";
		my $Lang;
		foreach $Lang (@LangList) {
			if ($Lang eq 'SC') {
#				change to put dummy file in dependency list
				$SupText.=" \"$ResrcTrgFullName$Lang.dummy\"";
				next;
			}
			$SupText.=" \"$ResrcTrgFullName$Lang\"";
		}
		$SupText.="\n\n";
		foreach $Lang (@LangList) {
#			change to put dummy file in dependency list
			if ($Lang eq 'SC') {
				$SupText.="\"$ResrcTrgFullName$Lang.dummy\"";
			}
			else {
				$SupText.="\"$ResrcTrgFullName$Lang\"";
			}
			$SupText.=" : \"$SrcPath$Resrc\" \$(DEPEND)\n";
			$SupText.="\tperl -S epocrc.pl -I \"$ChopSrcPath\"";
			foreach (@ChopUserIncPaths) {
				$SupText.=" -I \"$_\"";
			}
			$SupText.=" -I-";
			foreach (@ChopSysIncPaths) {
				$SupText.=" -I \"$_\"";
			}
			$SupText.=" -DLANGUAGE_$Lang -u \"$SrcPath$Resrc\"";
			$SupText.=" -o\"$ResrcTrgFullName$Lang\" -h\"$SrcPath$BaseResrc.rs~\" -t\"\$(EPOCBLD$Bld)\"\n";
# 			change because if a .RSC file is output then VC5 tries to link it to the main target as a Win32 resource file
			if ($Lang eq 'SC') {
				$SupText.="\techo this is a dummy output file > \"$ResrcTrgFullName$Lang.dummy\"\n";
			}
			$SupText.=join('',
				"\tperl -w -S ecopyfile.pl \"$SrcPath$BaseResrc.rs~\" \"$ResrcHdr\"\n",
				"\tdel \"$SrcPath$BaseResrc.rs~\"\n",
				"\n"
			);
		}

		foreach $Lang (@LangList) {
#			change again to avoid VC5 linking the resource
			my $TmpLang=$Lang;
			if ($TmpLang eq 'SC') {
				$TmpLang.='.dummy';
			}
			&main::Output(
				"\n",
				"\"$ResrcTrgFullName$TmpLang\" : \$(SOURCE) \"\$(INTDIR)\"\\\n",
				" \"\$(OUTDIR)\"\n",
				"   \$(BuildCmds)\n",
			);
		}
		&main::Output(
			"# End Custom Build\n",
			"\n"
		);
	}
	&main::Output(
		"!ENDIF \n",
		"\n",
		"# End Source File\n"
	);
}

sub PMDoc {    

	my $SrcPath=&main::SrcPath;

	&main::Output(
		"# Begin Source File\n",
		"\n",
		"SOURCE=",$SrcPath,ucfirst lc &main::Doc,"\n",
		"# PROP Exclude_From_Build 1\n",
		"# End Source File\n"
	);
}

sub PMStartSrc {    

	&main::Output(
		"# Begin Source File\n",
		"\n",
		"SOURCE=",&main::SrcPath,ucfirst lc &main::Src,"\n",
		"# End Source File\n"
	);
}

sub PMSrcDepend {

	# Generate user header list for this src, merge with list for all sources
	foreach (&main::DepList) {
		$PrjHdrs{$_}='unusedval';
	}
}

sub PMEndSrcList {

	my $BaseDsp=&main::BaseMak;
	my $PathBaseDsp=&main::MakeFilePath.$BaseDsp;

	&main::Output(
		"# End Group\n",
		"# Begin Group \"Resource Files\"\n",
		"\n",
		"# PROP Default_Filter \"ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe\"\n"
	);
	if ($Win32Resrc) {
		&main::Output(
			"# Begin Source File\n",
			"\n",
			"SOURCE=",ucfirst lc $Win32Resrc,"\n",
			"# End Source File\n"
		);
		# Generate user header list for this src, merge with list for all sources
		foreach (&main::Deps_GenDependsL($Win32Resrc)) {
			$PrjHdrs{$_}='unusedval';
		}
	}
	&main::Output(
		"# End Group\n"
	);

	# Use the global %PrjHdrs Hash to produce user header listing
	&main::Output(
		"# Begin Group \"Header Files\"\n",
		"\n",
		"# PROP Default_Filter \"h;hpp;hxx;hm;inl;fi;fd\"\n"
	);
	foreach (keys %PrjHdrs) {
		&main::Output(
			"# Begin Source File\n",
			"\n",
			"SOURCE=",&main::Path_Split('Path',$_),ucfirst lc &main::Path_Split('File',$_),"\n",
			"# End Source File\n"
		);
	}	
	&main::Output(
		"# End Group\n",
		"# End Target\n",
		"# End Project\n",
	);

	&main::Path_DelFiles("$PathBaseDsp.MAK","$PathBaseDsp.MDP","$PathBaseDsp.NCB","$PathBaseDsp.OPT","$PathBaseDsp.PLG");

#	Add target to supplementary makefile to recreate the workspace
#
#	This target is intended for use as a custom tool within the MSVC IDE, for regenerating
#	workspace once the .MMP file has been edited within the IDE.  To install the target as
#	a custom tool in the IDE, select Tools->Customise...->Tools, and choose a name for the
#	tool, e.g. "Recreate Workspace".  Next, type "nmake.exe" as the command and
#	"-nologo -f $(WkspDir)\$(WkspName).sup.make recreateworkspace" as the program arguments.
#	Leave the "initial directory" field blank, and tick the "Close window on exiting" checkbox.
#	Having edited the .MMP file for a project, select the new tool from the tools menu to
#	recreate the workspace.  If the commands have run correctly, you will be prompted to
#	reload the workspace.
	$SupText.=join('',
		"\n",
		"RECREATEWORKSPACE :\n",
		'	cd ', &main::Path_Chop(&main::Path_WorkPath), "\n",
		'	perl -w -S makmake.pl -D ', &main::MmpFile, ' ', &main::PlatName, "\n",
		"\n"
	);

#	Create the supplementary makefile
	
	&main::CreateExtraFile(&main::MakeFilePath.&main::BaseMak.'.SUP.MAKE', $SupText);


# create the .DSW file
	my $DswText=join(
		"\n",
		"Microsoft Developer Studio Workspace File, Format Version 6.00",
		'# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!',
		'',
		'#'x79,
		'',
		"Project: \"$BaseDsp\"=.\\$BaseDsp.dsp - Package Owner=<4>",
		'',
		'Package=<5>',
		'{{{',
		'}}}',
		'',
		'Package=<4>',
		'{{{',
		'}}}',
		'',
		'#'x79,
		'',
		'Global:',
		'',
		'Package=<5>',
		'{{{',
		'}}}',
		'',
		'Package=<3>',
		'{{{',
		'}}}',
		'',
		'#'x79,
		''
	);

	&main::CreateExtraFile("$PathBaseDsp.DSW",$DswText);
}

1;
