# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package Cl_win;

# declare variables global for module
my @Win32LibList=();
my $Win32StdHeaders;
my $BaseAddressFlag;
my $Win32Resrc;


require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	PMHelp_Mmp

	PMCheckPlatformL

	PMPlatProcessMmp
	
	PMStartBldList
		PMBld
	PMStartSrcList
		PMBitMapBld
		PMResrcBld
		PMAifBld
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
use Pathutl;

sub PMHelp_Mmp {
	&Winutl_Help_Mmp;
}

sub SysTrg () {
	return 1 if &main::SystemTrg;
	my $ExportLibrary=&main::ExportLibrary;
	return 1 if ($ExportLibrary =~ /EKERN/i);
	my $Trg=&main::Trg;
	return 1 if ($Trg =~ /KSRT/i);
	return 0;
}

sub PMCheckPlatformL {
	if ((&main::Plat eq 'TOOLS') and (&main::BasicTrgType ne 'EXE') and (&main::BasicTrgType ne 'LIB')) {
		die "Can't specify anything but EXE or LIB targettypes for this platform\n";
	}
}

sub PMPlatProcessMmp (@) {
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
	my $AifStructRef=&main::AifStructRef;
	my $BaseTrg=&main::BaseTrg;
	my $BitMapStructRef=&main::BitMapStructRef;
	my @BldList=&main::BldList;
	my @ChopSysIncPaths=&main::Path_Chop(&main::SysIncPaths);
	my @ChopUserIncPaths=&main::Path_Chop(&main::UserIncPaths);
	my $ExportLibrary=&main::ExportLibrary;
	my $NoExportLibrary=&main::NoExportLibrary;
	my $DefFile=&main::DefFile;
	my $BasicTrgType=&main::BasicTrgType;
	my $LibPath=&main::LibPath;

	my $MSVCVer = &Winutl_MSVCVer;

	my @MacroList=&main::MacroList();
	push @MacroList, "__SUPPORT_CPP_EXCEPTIONS__";

	my $VariantFile=&main::VariantFile();
	my $Plat=&main::Plat;
	my $ResourceStructRef=&main::ResourceStructRef;
	my $Trg=&main::Trg;
	my $TrgType=&main::TrgType;
	my $defaultWarningLevel="/W4";
	my $CompilerOption=&main::CompilerOption("MSVC");
	my $LinkAs=&main::LinkAs;

	&Generic_Header(0,$makecmd);	# define standard things using absolute paths

	my $TrgDir="";
	if (&Generic_Definition("TRGDIR") ne "") {
		$TrgDir="\$(TRGDIR)\\";
	}
	if (&main::Plat eq "WINC") {	# target path ignored under WINC
		$TrgDir="";
	}
	&main::Output(
		"INCDIR  ="
	);
	if($VariantFile){
        my $VariantFileFileName  = Path_Split('FILE',$VariantFile);
        &main::Output(
              " /FI \"$VariantFileFileName\"",
              );

	}

	foreach (@ChopUserIncPaths,@ChopSysIncPaths) {
		&main::Output(
			" /I \"$_\""
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	my $MSVCVer = &Winutl_MSVCVer;

	&main::Output(
		"CLFLAGS =",
			" /nologo",			# suppress "sign-on" banner message
			" /Zp4",			# packs structures on 4 byte boundaries
			" /GF"				# Pools strings and places them in read-only memory 
		);	

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

	if ($MSVCVer >= 8) {
		&main::Output(
			" /Zc:wchar_t-"				# Don't make wchar_t a built-in; breaks mangled names
		);		
	}

	if (&main::Plat ne 'TOOLS') {
		unless ($Win32StdHeaders) {
			&main::Output(
				" /X"			# ignore standard include directories
			);
		}
	}
	if ($CompilerOption !~ /\/W\d/) {
		# Supply default warning level unless /Wn already specified via OPTION MSVC
		&main::Output(
			" $defaultWarningLevel"
		);
	}
	&main::Output(
		" $CompilerOption",		# user-supplied compiler options
		"\n",
		"\n"
	);

	&main::Output(
		"CLDEFS  ="
	);
	foreach(@MacroList) {
		&main::Output(
			" /D \"$_\""
		);
	}
	if (($BasicTrgType=~/^LIB$/o) and (&main::Plat eq 'TOOLS')) {
		&main::Output(
			" /D _MT"
		); 
	}
	&main::Output(
		" \$(USERDEFS)\n",
		"\n"
	);

	foreach (@BldList) {
		&main::Output(
			"CL$_ = cl.exe"
		);
		if ($BasicTrgType=~/^EXE$/o) {
			&main::Output(
				' /MT'			# Creates a multi-threaded executable file, using libcmt.lib
			);
			if (/DEB$/o) {
				&main::Output(
					'd'				# i.e. /MTd or /MLd, debug executable using debug version of LIB
				);
			}
		}
		elsif (($BasicTrgType=~/^(DLL|LIB)$/o) and (&main::Plat ne 'TOOLS')){
			&main::Output(
				' /MD'			# Creates a multithreaded DLL, using MSVCRT.LIB
			);
			if (/DEB$/o) {
				&main::Output(
					'd'				# i.e. /MDd, debug executable using debug version of LIB
				);
			}
		}
		if (/DEB$/o) {
			&main::Output(
				' /Zi',			# Generates complete debugging information
				' /Od'			# Disables optimization
			);
#			euser change to apply inlining on the _NAKED functions
			if ($BaseTrg=~/^EUSER$/oi) {
				&main::Output(
					' /Ob1'		# Specific control of expension of inline functions
				);
			}
		}
		elsif (/REL$/o) {
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
			' $(CLFLAGS)'
		);
		if (/DEB$/o) {
			&main::Output(
				" /Fd\"\$(EPOCTRG$_)\\$TrgDir$BaseTrg.pdb\""
			);
		}
		foreach (&main::MacroList($_)) {
			&main::Output(
				" /D $_"
			);
		}
		&main::Output(
			" \$(CLDEFS) \$(INCDIR)\n"
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
		if ($BasicTrgType !~ /^IMPLIB$/io) {
			&main::Output (
				" \\\n",
				"\t\"\$(EPOCTRG$_)\\$TrgDir$Trg\""
			);
			unless (/REL$/o) {
				&main::Output(
					" \\\n",
					"\t\"\$(EPOCTRG$_)\\$BaseTrg.bsc\""
				);
			}
			if (&Winutl_CopyForStaticLinkage) {
				&main::Output(
					" \\\n",
					"\t\"\$(EPOCTRG$_)\\$Trg\""
				);
			}
		}

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


	foreach (@BldList) {
		my $makework="MAKEWORK$_";
		&main::Output(
			"\n",
			"RESOURCE$_ : $makework"
		);

		my $BitMapRef;
		foreach $BitMapRef (@$BitMapStructRef) {
			my $file="\$(EPOCTRG$_)\\$$BitMapRef{TrgPath}$$BitMapRef{Trg}";
			&Generic_MakeWorkFile($makework,$file);
			&main::Output(
				" \\\n",
				"\t\"$file\""
			);
		}
		undef $BitMapRef;

		my $ResourceRef;
		foreach $ResourceRef (@$ResourceStructRef) {
			my $file="\$(EPOCTRG$_)\\$$ResourceRef{Trg}";
			&Generic_MakeWorkFile($makework,$file);
			&main::Output(	# must come before main target because source for target will depend on the
			" \\\n",		# *.rsg file in $EPOCIncPath
			"\t", &Generic_Quote("$file")
			);
		}
		undef $ResourceRef;

		my $AifRef;
		foreach $AifRef (@$AifStructRef) {
			my $file="\$(EPOCTRG$_)\\$TrgDir$$AifRef{Trg}";
			&Generic_MakeWorkFile($makework,$file);
			&main::Output(
				" \\\n",
				"\t\"$file\""
			);
		}
		undef $AifRef;

		&main::Output(
			"\n"
		);
	}
	&main::Output(
		"\n",
		"\n",
	);

	&main::Output(
		"LIBRARY : MAKEWORKLIBRARY"
	);
	if ($BasicTrgType=~/^LIB$/o) {
#		code to ensure that the static libraries for all builds are built at the library stage
		foreach (@BldList) {
			&main::Output(
				" $_"
			);
		}
	}
	elsif ($DefFile and !$NoExportLibrary) {
		unless (&main::ExportUnfrozen) {
			if (-e $DefFile) { # effectively "if project frozen ..."
				&main::Output(
					" \"\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib\"\n"
				);
			} else {
				&main::Output(
					"\n",
					"\t\@echo WARNING: Not attempting to create \"\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib\".\n",
					"\t\@echo When exports are frozen in \"$DefFile\", regenerate Makefile.\n"
				);
			}
		} else {
			&main::Output(
				"\n",
				"\t\@echo Not attempting to create \"\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib\"\n",
				"\t\@echo from frozen .DEF file, since EXPORTUNFROZEN specified.\n"
			);
		}
		my $LibLinkAs = ($BasicTrgType=~/^IMPLIB$/io) ? $LinkAs : $Trg;
		&main::Output(
			"\n",
			"\n",
			"# REAL TARGET - LIBRARY\n",
			"\n",
			"\"\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib\" : \"$DefFile\"\n",
			"\tperl -w -S prepdef.pl \"$DefFile\" \"\$(EPOCBLD)\\$ExportLibrary.prep.def\"\n",
			"\tlib.exe /nologo /machine:i386 /nodefaultlib /name:\"$LibLinkAs\"    /def:\"\$(EPOCBLD)\\$ExportLibrary.prep.def\" /out:\"\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib\"\n",
			"\tdel \"\$(EPOCLIB)\\UDEB\\$ExportLibrary.exp\"\n"
		);
	}
	&main::Output(
		"\n",
		"\n",
		"FREEZE :\n"
	);
	if ($DefFile and $BasicTrgType !~ /^IMPLIB$/io) {
		&main::Output(
#			call perl on the script here so nmake will die if there are errors - this doesn't happen if calling perl in a batch file
			"\tperl -w -S efreeze.pl \$(EFREEZE_ALLOW_REMOVE) \"$DefFile\" \"\$(EPOCBLD)\\$ExportLibrary.def\" \n"
		);
	}
	&main::Output(
		"\n",
		"CLEANLIBRARY :\n"
	);
	if ($DefFile and !$NoExportLibrary) {
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib\"\n"
		);
	}
	&Generic_MakeWorkDir('MAKEWORKLIBRARY',"${LibPath}UDEB");

	&Generic_Releaseables;
}

sub PMBld {

	my $AifStructRef=&main::AifStructRef;
	my @SrcList=&main::SrcList;
	my $BaseTrg=&main::BaseTrg;
	my $BitMapStructRef=&main::BitMapStructRef;
	my $Bld=&main::Bld;
	my $ExportLibrary=&main::ExportLibrary;
	my $NoExportLibrary=&main::NoExportLibrary;
	my $DefFile=&main::DefFile;
	my $EPOCIncPath=&main::EPOCIncPath;
	my $FirstLib=&main::FirstLib;
	my $BasicTrgType=&main::BasicTrgType;
	my @LibList;
	my $RelPath=&main::RelPath;
	my $ResourceStructRef=&main::ResourceStructRef;
	my @StatLibList=&main::StatLibList;
	my $Trg=&main::Trg;
	my $TrgPath=&main::TrgPath;
	my $TrgType=&main::TrgType;
	my $MSVCVer = &Winutl_MSVCVer;
	my $newlib = main::NewLib(); # Check if newlib has been set in the MMP file.


    my $NewLib = 'scppnwdl.lib'; # This is where operator new and operator delete
                                 # are defined for user side targets.

    my $NewKernLib = 'scppnwdl_kern.lib'; # This is where operator new and operator delete
                                 	  # are defined for kernel side targets.



	my $WarningLevel='/WARN:3';
	if (&main::Plat() eq 'TOOLS') {
		$WarningLevel='/WARN:1';    # avoid LNK4005 warnings about unused libraries
	}

	if ($Bld =~ /DEB/) {
		@LibList=&main::DebugLibList;
	} else {
		@LibList=&main::LibList;
	}

	my $BLDTRGPATH = "\$(EPOCTRG$Bld)\\";
	my $BLDDATAPATH = "\$(EPOCTRG$Bld)\\";
	if (&main::Plat ne "WINC") {	# target paths ignored under WINC
		if (&Generic_Definition("TRGDIR") ne "") {
			$BLDTRGPATH .= "\$(TRGDIR)\\";	    # handles TARGETPATH
		}
		$BLDDATAPATH .= "\$(DATADIR)\\";
	}

	my $Entry="";
	my $EntrySymbol='';
	my $Include="";
	if ($BasicTrgType=~/^DLL$/o) {
		$Entry="/noentry";
		$Include="/include:\"__E32Dll\"";
		$EntrySymbol='_E32Dll';
	}
	elsif ($TrgType=~/^EXEXP$/o) {
		$Entry="/noentry";
		$Include="/include:\"__E32Startup\"";
		$EntrySymbol='_E32Startup';
	}
	elsif ($BasicTrgType=~/^EXE$/o) {
		unless (&main::Plat eq 'TOOLS') {
			$Entry="/entry:\"_E32Bootstrap\"";
			$Include="/include:\"__E32Startup\"";
		}
	}
	my $AbsentSubst = '';
	if ($EntrySymbol) {
		$AbsentSubst = " -absent $EntrySymbol";
	}

	# REAL TARGETS
	#-------------
	&main::Output(
		"# REAL TARGET - BUILD VARIANT $Bld\n",
		"\n"
	);

#	releasables
	my @releaseables;

	unless (&main::Plat() eq 'TOOLS') {
		if ($BasicTrgType !~ /^IMPLIB$/io) {
			push @releaseables, "$BLDTRGPATH$Trg";
			if (&Winutl_CopyForStaticLinkage) {
				push @releaseables, "\$(EPOCTRG$Bld)\\$Trg";
			}
			my $BitMapRef;
			foreach $BitMapRef (@$BitMapStructRef) {
				push @releaseables, "\$(EPOCTRG$Bld)\\$$BitMapRef{TrgPath}$$BitMapRef{Trg}";
			}
			my $ResourceRef;
			foreach $ResourceRef (@$ResourceStructRef) {
				push @releaseables, "\$(EPOCTRG$Bld)\\$$ResourceRef{Trg}";
			}
			my $AifRef;
			foreach $AifRef (@$AifStructRef) {
				push @releaseables, "$BLDTRGPATH$$AifRef{Trg}";
			}
			if ($Bld=~/DEB$/o) {
				push @releaseables,"$BLDTRGPATH$BaseTrg.PDB";
			}
		}
		if (-e $DefFile and !$NoExportLibrary) { # effectively "if project frozen ..."
			push @releaseables, "\$(EPOCLIB$Bld)\\$ExportLibrary.lib";
		}
	}
	else {
		my $toolspath=&main::EPOCToolsPath();
		push @releaseables, "$toolspath$Trg";
	}

	&main::Output(
		"WHAT$Bld : WHATGENERIC\n",
		"\n",
		"CLEAN$Bld : CLEANBUILD$Bld CLEANRELEASE$Bld\n",
		"\n",
		"CLEANBUILD$Bld : CLEANILK$Bld\n",
		"\t\@perl -w -S ermdir.pl \"\$(EPOCBLD$Bld)\"\n",
		"\n",
		"CLEANRELEASE$Bld : CLEANGENERIC CLEANILK$Bld\n",
		"\n",
		"CLEANILK$Bld :\n",
		"\t-\$(ERASE) \"$BLDTRGPATH$BaseTrg.ILK\"\n",
		"\n"
	);
	&Generic_WhatCleanTargets($Bld, "WHAT$Bld", "CLEANRELEASE$Bld", @releaseables);

	&Generic_MakeWorkDir("MAKEWORK$Bld", &main::BldPath);
	&Generic_MakeWorkDir("MAKEWORK$Bld", $BLDTRGPATH);

	return if ($BasicTrgType =~ /^IMPLIB$/io);

	&main::Output(
		"LISTING$Bld : MAKEWORK$Bld"
	);
	foreach (@SrcList) {
		my $BaseSrc = &main::Path_Split('Base', $_);
		my $Ext = &main::Path_Split('Ext', $_);
		$BaseSrc.='_' if (lc($Ext) eq '.cia');
   		&main::Output(
			" \\\n\tLISTING$Bld$BaseSrc"
   		);
   	}
	&main::Output(
		"\n",
		"\n"
	);

	if ($Bld=~/DEB$/o) {
		&main::Output(
			"BSC32_SBRS="
		);
		foreach (@SrcList) {
			my $BaseSrc = &main::Path_Split('Base', $_);
			my $Ext = &main::Path_Split('Ext', $_);
			$BaseSrc.='_' if (lc($Ext) eq '.cia');
			&main::Output(
				" \\\n",
				"\t\"\$(EPOCBLD$Bld)\\$BaseSrc.sbr\""
			);
   		}
		&main::Output(
			"\n",
			"\n",
		"\"\$(EPOCTRG$Bld)\\$BaseTrg.bsc\" : \$(BSC32_SBRS)\n",
		"\tbscmake.exe \@<<\n",
		"\t/nologo /o\$\@ \$(BSC32_SBRS)\n",
		"<<\n",
		"\n",
		"\n"
		);
	}

	&main::Output(
		"LIBS="
	);
	foreach (@StatLibList) {
		&main::Output(
			" \\\n\t\"\$(EPOCSTATLINK$Bld)\\$_\""
		);
	}
	foreach (@LibList) {
		&main::Output(
			" \\\n\t\"\$(EPOCLINK$Bld)\\$_\""
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"LINK_OBJS="
	);
	foreach (@SrcList) {
		my $BaseSrc = &main::Path_Split('Base', $_);
		my $Ext = &main::Path_Split('Ext', $_);
		$BaseSrc.='_' if (lc($Ext) eq '.cia');
   		&main::Output(
			" \\\n",
			"\t\"\$(EPOCBLD$Bld)\\$BaseSrc.obj\""
   		);
   	}
	if ($Win32Resrc) {
		&main::Output(
			" \\\n",
			"\t\"\$(EPOCBLD$Bld)\\",&main::Path_Split('Base',$Win32Resrc),".res\""
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		&main::OutFormat(
			"STAGE1_LINK_FLAGS="
		);
		&main::OutFormat(
			"\"\$(EPOCSTATLINK$Bld)\\$FirstLib\""
		);
        
        if (main::StdCppSupport())
        {
            unless ($newlib) {
		    	if ( main::SystemTrg() ) {
		    		# System targets are PDD, LDD, VAR, KEXT and KDLL.
		    		&main::OutFormat(
		    			"\"\$(EPOCSTATLINK$Bld)\\$NewKernLib\""
		    		);
		    	}
		    	else {
		    		&main::OutFormat(
		    			"\"\$(EPOCSTATLINK$Bld)\\$NewLib\""
		    		);
		    	}
		    }
	    	else {
	    		&main::OutFormat(
				    "\"\$(EPOCSTATLINK$Bld)\\$newlib\""
			    );
		    }    
        }
		
		foreach (@Win32LibList) {
			&main::OutFormat(
				' ',lc $_
			);
		}
		&main::OutFormat(
			" \$(LIBS) /nologo$BaseAddressFlag $Entry /subsystem:windows /dll"
		);
		if ($Bld=~/DEB$/o) {
			&main::OutFormat(
				' /debug'
			);
		}
		&main::OutFormat(
			" /incremental:no /machine:IX86"
		);

		if ($BasicTrgType=~/^EXE$/o) {
			my $debug = '';
			if ($Bld =~ /DEB$/o) {
				$debug .= 'd';
			}
			&main::OutFormat(
				" /nodefaultlib:libcmt$debug.lib"
			);
		}
		&main::OutFormat(
			" $Include /out:\"\$(EPOCBLD$Bld)\\$Trg\""
		);

		if ($MSVCVer < 7) {
			&main::OutFormat(
				" $WarningLevel"
			);
		}
		&main::OutFormat(
			" /implib:\"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"",
			' /ignore:4089',		# LNK4089: all references to "dynamic-link library" discarded by /OPT:REF
			' /ignore:4005'			# LNK4005: no objects used from XXX
		);
		if ($MSVCVer >= 7) {
			&main::OutFormat(
				' /ignore:4210'		# LNK4210: .CRT section exists; there may be unhandled static initializers or terminators
			);
		}


		&main::Output(
			"\n",
			"\n"
		);
	}

	&main::OutFormat(
		'LINK_FLAGS='
	);
	if ($BasicTrgType=~/^(EXE|DLL)$/o) {
		unless (&main::Plat eq 'TOOLS') {
			&main::OutFormat(
				" \"\$(EPOCSTATLINK$Bld)\\$FirstLib\""
			);

            if (main::StdCppSupport())
            {
                unless ($newlib) {
			    	if ( main::SystemTrg() ) {
				    	# System targets are PDD, LDD, VAR, KEXT and KDLL.

					    &main::OutFormat(
						    "\"\$(EPOCSTATLINK$Bld)\\$NewKernLib\""
					    );
	    			}
		    		else {
			    		&main::OutFormat(
				    		"\"\$(EPOCSTATLINK$Bld)\\$NewLib\""
					    );
    				}
	    		}
		    	else {
			    	&main::OutFormat(
				    	"\"\$(EPOCSTATLINK$Bld)\\$newlib\""
    				);
	    		}
            }
		}
	}
	foreach (@Win32LibList) {
		&main::OutFormat(
			' ',lc $_
		);
	}
	&main::OutFormat(
		' $(LIBS) /nologo'
	);
	if ($BasicTrgType=~/^(EXE|DLL)$/o) {
		unless (&main::Plat eq 'TOOLS') {
			&main::OutFormat(
				" /fixed:no $BaseAddressFlag"
			);
		}
	}
	&main::OutFormat(
		" $Entry"
	);
	if (&main::Plat=~/^(WINC|TOOLS)$/o && $BasicTrgType=~/^EXE$/o) {
		&main::OutFormat(
			' /subsystem:console'
		);
	}
	else {
		&main::OutFormat(
			' /subsystem:windows'
		);
	}
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		&main::OutFormat(
			" /dll \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\""
		);
	}
	if (&main::HeapSize) {
		my %HeapSize=&main::HeapSize;
		&main::OutFormat(
			' /heap:',$HeapSize{Max},',',$HeapSize{Min}
		);
	}
	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		if ($Bld=~/DEB$/o) {
			&main::OutFormat(
				' /debug'
			);
		}
		elsif ($Bld=~/REL$/o) {
			&main::OutFormat(
				' /incremental:no'
			);
		}
	}
	&main::OutFormat(
		' /machine:IX86',
		' /ignore:4089',	# LNK4089: all references to "dynamic-link library" discarded by /OPT:REF
		' /ignore:4005'		# LNK4005: no objects used from XXX
	);
	if ($MSVCVer >= 7) {
		&main::OutFormat(
			' /ignore:4210'		# LNK4210: .CRT section exists; there may be unhandled static initializers or terminators
		);
	}
	if (&main::Plat eq 'TOOLS') {
	    if (length(&main::LinkerOption("MSVC")) > 0)
	    {
		&main::OutFormat( " " . &main::LinkerOption("MSVC"));
	    } else {
		&main::OutFormat(
			' /ignore:4098'	# LNK4098: defaultlib 'LIBCMT' conflicts with use of other libs; use /NODEFAULTLIB:library
		);
	    }
	}
	else
	{
		if ($BasicTrgType=~/^EXE$/o) {
			my $debug = '';
			if ($Bld =~ /DEB$/o) {
				$debug .= 'd';
			}
			&main::OutFormat(
				" /nodefaultlib:libcmt$debug.lib"
			);
		}
		&main::OutFormat(
			" $Include"
		);
	} 
	&main::OutFormat(
		" /out:\"$BLDTRGPATH$Trg\""
	);
	if ($BasicTrgType=~/^(DLL)$/o) {
		if ($MSVCVer < 7) {
			&main::OutFormat(
				" $WarningLevel"
			);
		}
		&main::OutFormat(
			" /implib:\"$BLDTRGPATH$ExportLibrary.lib\""
		);
	} elsif ($BasicTrgType=~/^(EXE)$/o) {
		if ($MSVCVer < 7) {
			&main::OutFormat(
				" $WarningLevel"
			);
		}
		&main::OutFormat(
			" /implib:\"$BLDTRGPATH$ExportLibrary.exe.lib\""
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"\"$BLDTRGPATH$Trg\" : \$(LINK_OBJS)"
	);
	if (-e $DefFile) { # effectively "if project frozen ..."
		&main::Output(
			" \"$DefFile\""
		);
	}
	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		unless (&main::Plat eq 'TOOLS') {
			&main::Output(
				" \"\$(EPOCSTATLINK$Bld)\\$FirstLib\""
			);
			
			if (main::StdCppSupport())
			{
			    unless ($newlib) {
    				if ( main::SystemTrg() ) {
	    				# System targets are PDD, LDD, VAR, KEXT and KDLL.
    
	    				&main::Output(
		    				"\"\$(EPOCSTATLINK$Bld)\\$NewKernLib\""
			    		);
				    }
    				else {
	    				&main::Output(
		    				"\"\$(EPOCSTATLINK$Bld)\\$NewLib\""
			    		);
				    }
    			}
	    		else {
		    		&main::Output(
			    		"\"\$(EPOCSTATLINK$Bld)\\$newlib\""
				    );
    			}		    
			}
		}
	}
	&main::Output(
		" \$(LIBS)\n"
	);


#	Link by name first time round for dlls
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		&main::Output(
			"\tlink.exe \@<<\n",
			"\t\t\$(STAGE1_LINK_FLAGS) \$(LINK_OBJS)\n",
			"<<\n",
			"\tdel \"\$(EPOCBLD$Bld)\\$Trg\"\n",
			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\"\n"
		);

#		Generate an export info file
		&main::Output(
			"\tdumpbin /exports /out:\"\$(EPOCBLD$Bld)\\$ExportLibrary.inf\" \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n",
			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n"
		);

#		call makedef to reorder the export information
		&main::Output(
#			call perl on the script here so nmake will die if there are errors - this doesn't happen if calling perl in a batch file
			"\tperl -w -S makedef.pl $AbsentSubst -Inffile \"\$(EPOCBLD$Bld)\\$ExportLibrary.inf\""
		);
		if (SysTrg()) {
    			&main::Output( "\t\t-SystemTargetType \\\n" );
    		}
		if (-e $DefFile) { # effectively "if project frozen ..."
			&main::Output(
				" -Frzfile \"$DefFile\""
			);
		}
		# freeze ordinals, a maximum of 2, for polymorphic dlls
		my $Ordinal;
		my $Num=1;
		foreach $Ordinal (&main::Exports) {
#			replace "$" with "$$" so that NMAKE doesn't think there's a macro in the function name
			$Ordinal=~s-\$-\$\$-go;
			&main::Output(
				" -$Num $Ordinal"
			);
			$Num++;
		}
		&main::Output(
			" \"\$(EPOCBLD)\\$ExportLibrary.def\" \n",
			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.inf\"\n"
		);

		# create the export object from the .DEF file
		&main::Output(
			"\tlib.exe  /nologo /machine:i386 /nodefaultlib /name:\"$Trg\" /def:\"\$(EPOCBLD)\\$ExportLibrary.def\" /out:\"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n"
		);
		if (&main::ExportUnfrozen) {
			&main::Output(
			"\tcopy \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\" \"\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib\"\n"
			);
		}
		&main::Output(
			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n"
		);
	}

	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		&main::Output(
			"\tlink.exe \@<<\n"
		);
	}
	elsif ($BasicTrgType=~/^LIB$/o) {
		&main::Output(
			"\tlib.exe \@<<\n"
		);
	}
	&main::Output(
		"\t\t\$(LINK_FLAGS) \$(LINK_OBJS)\n",
		"<<\n"
	);

	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		&main::Output(
			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\"\n"
		);
	}

	if (&main::Plat eq 'TOOLS') {
		&main::Output(
			"\tcopy \"$BLDTRGPATH$Trg\" \"",&main::EPOCToolsPath,"$Trg\"\n"
		);
	}
	if (&Winutl_CopyForStaticLinkage) {
		&Generic_MakeWorkDir("MAKEWORK$Bld", "\$(EPOCTRG$Bld)");
		&main::Output(
			"\n",
			"\"\$(EPOCTRG$Bld)\\$Trg\" : \"$BLDTRGPATH$Trg\"\n",
			"\tcopy \$\? \$\@\n"
		);
	}

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

sub PMBitMapBld {

	&Generic_BitMapBld;

	# Need to copy generic resource into emulated Z drive

	my $BitMapRef=&main::BitMapRef;

	my $ChopTrgPath="";
	if ($$BitMapRef{TrgPath}) {
		$ChopTrgPath.="\\$$BitMapRef{TrgPath}";
		chop $ChopTrgPath;
	}

	my @BldList=&main::BldList;
	my $Bld;
	foreach $Bld (@BldList) {
		my $path="\$(EPOCTRG$Bld)$ChopTrgPath";
		&main::Output(
			&Generic_Quote("$path\\$$BitMapRef{Trg}"), " : ", 
			&Generic_Quote("$$BitMapRef{GenericTrg}"), "\n",
			"\t", &Generic_CopyAction(),
			"\n"
		);
	}
}


sub PMResrcBld {

	&Generic_ResrcBld;

	# Need to copy generic resource into emulated Z drive

	my $ResourceRef=&main::ResourceRef;
	my @BldList=&main::BldList;

	foreach my $Bld (@BldList) {
		&main::Output(
			&Generic_Quote("\$(EPOCTRG$Bld)\\$$ResourceRef{Trg}"), " : ", 
			&Generic_Quote("$$ResourceRef{GenericTrg}"), "\n",
			"\t", &Generic_CopyAction(),
			"\n"
		);
	}
}

sub PMAifBld {

	&Generic_AifBld;

	# Need to copy generic resource into emulated Z drive

	my $AifRef=&main::AifRef;
	my $TrgDir="";
	if (&Generic_Definition("TRGDIR") ne "") {
		$TrgDir="\\\$(TRGDIR)";
	}
	if (&main::Plat eq "WINC") {	# target path ignored under WINC
		$TrgDir="";
	}

	my @BldList=&main::BldList;
	my $Bld;
	foreach $Bld (@BldList) {
		my $file="\$(EPOCTRG$Bld)$TrgDir\\$$AifRef{Trg}";
		&main::Output(
			"\"$file\" : \"$$AifRef{GenericTrg}\"\n",
			"\tperl -w -S ecopyfile.pl \$? \$\@\n",
			"\n"
		);
	}
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

	&main::Output(
		"DEPEND="
	);
	foreach (@DepList) {
		&main::Output(
		" \\\n\t\"$_\""
		);
	}
	&main::Output(
		"\n",
		"\n"
	);
	foreach (@BldList) {
		&main::Output(
			"DEPEND$_=\$(DEPEND)\n",
			"\n"
		);
	}
	&main::Output(
		"\n"
	);
}

sub PMSrcBldDepend {
	my $Bld=&main::Bld;
	my @DepList=&main::DepList;

	&main::Output(
		"DEPEND$Bld="
	);
	foreach (@DepList) {
		&main::Output(
			" \\\n\t\"$_\""
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
	my $Plat=&main::Plat;
	my $Src=ucfirst lc &main::Src;
	my $SrcPath=&main::SrcPath;
	my $Ext=&main::ExtSrc;
	my $Cia = (lc($Ext) eq '.cia') ? 1 : 0;

	if ($Cia) {
		&main::Output(
			"\"\$(EPOCBLD$Bld)\\$BaseSrc\_.obj\" \"\$(EPOCBLD$Bld)\\$BaseSrc\_.sbr\" : \"$SrcPath$Src\" \$(DEPEND$Bld)\n",
			"\t\$(CL$Bld) /TP /Fo\"\$(EPOCBLD$Bld)\\$BaseSrc\_.obj\" /FR\"\$(EPOCBLD$Bld)\\$BaseSrc\_.sbr\" /c \"$SrcPath$Src\"\n",
			"\n",
#		assembler listing target 
			"LISTING$Bld$BaseSrc\_ : \"\$(EPOCBLD$Bld)\\$BaseSrc\_.lis\"\n",
			"\tcopy \$? \"$SrcPath$BaseSrc\_.$Plat.lst\"\n",
			"\n",
			"\"\$(EPOCBLD$Bld)\\$BaseSrc\_.lis\": \"$SrcPath$Src\" \$(DEPEND$Bld)\n",
			"\t\$(CL$Bld) /TP /Fa\"\$\@\" /Fo\"\$(EPOCBLD$Bld)\\$BaseSrc\_.obj\" /FR\"\$(EPOCBLD$Bld)\\$BaseSrc\_.sbr\" /c \"$SrcPath$Src\"\n",
			"\n"
		);
	} else {
		&main::Output(
			"\"\$(EPOCBLD$Bld)\\$BaseSrc.obj\" \"\$(EPOCBLD$Bld)\\$BaseSrc.sbr\" : \"$SrcPath$Src\" \$(DEPEND$Bld)\n",
			"\t\$(CL$Bld) /Fo\"\$(EPOCBLD$Bld)/\" /FR\"\$(EPOCBLD$Bld)\\$BaseSrc.sbr\" /c \"$SrcPath$Src\"\n",
			"\n",
#		assembler listing target 
			"LISTING$Bld$BaseSrc : \"\$(EPOCBLD$Bld)\\$BaseSrc.lis\"\n",
			"\tcopy \$? \"$SrcPath$BaseSrc.$Plat.lst\"\n",
			"\n",
			"\"\$(EPOCBLD$Bld)\\$BaseSrc.lis\": \"$SrcPath$Src\" \$(DEPEND$Bld)\n",
			"\t\$(CL$Bld) /Fa\"\$\@\" /Fo\"\$(EPOCBLD$Bld)/\" /FR\"\$(EPOCBLD$Bld)\\$BaseSrc\_.sbr\" /c \"$SrcPath$Src\"\n",
			"\n"
		);
	}
}

sub PMEndSrc {

	&main::Output(
		"\n",
		"\n"
	);
}

sub PMEndSrcList {
	if ($Win32Resrc) {
		my @BldList=&main::BldList;
		my @DepList=&main::Deps_GenDependsL($Win32Resrc);

		&main::Output(
			"# Win32 Resource $Win32Resrc\n",
			"\n",
			"DEPEND="
		);
		foreach (@DepList) {
			&main::Output(
				" \\\n\t\"$_\""
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
	
		my $Bld;
		foreach $Bld (@BldList) {
			&main::Output(
				"\"\$(EPOCBLD$Bld)\\",&main::Path_Split('Base',$Win32Resrc),".res\" : \"$Win32Resrc\" \$(DEPEND)\n",
				"\trc /l 0x809 /fo\$\@ /i \"",&main::Path_Chop(&main::Path_Split('Path',$Win32Resrc)),"\" \"$Win32Resrc\"\n",
				"\n"
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
	}

	# Deal with accumulated MAKEDIRS etc.

	&Generic_End;
}

1;
