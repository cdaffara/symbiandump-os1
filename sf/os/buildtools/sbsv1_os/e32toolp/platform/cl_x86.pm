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


package Cl_x86;

# declare variables global for module


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
use Genutl;
use Pathutl;

use constant NOCOMPRESSIONMETHOD => 0;
use constant INFLATECOMPRESSIONMETHOD => 1;
use constant BYTEPAIRCOMPRESSIONMETHOD => 2;

use constant NOTPAGED => 0;
use constant UNPAGED => 1;
use constant PAGED => 2;
sub PMHelp_Mmp {
	&Winutl_Help_Mmp;
}

sub PMCheckPlatformL {
}

sub PMPlatProcessMmp (@) {

}

sub PMStartBldList($) {
	my ($makecmd) = @_;
	die "Cannot generate $makecmd makefiles\n" if ($makecmd ne "nmake");
	my $BaseTrg=&main::BaseTrg;
	my @BldList=&main::BldList;
	my @ChopSysIncPaths=&main::Path_Chop(&main::SysIncPaths);
	my @ChopUserIncPaths=&main::Path_Chop(&main::UserIncPaths);
	my $DefFile=&main::DefFile;
	my $BasicTrgType=&main::BasicTrgType;
	my $LinkAs=&main::LinkAs;
	my $ExportLibrary=&main::ExportLibrary;
	my $NoExportLibrary=&main::NoExportLibrary;
	my $LibPath=&main::LibPath;
	my @MacroList=&main::MacroList();
	my $VariantFile=&main::VariantFile();
	my $Plat=&main::Plat;
	my $Trg=&main::Trg;
	my $TrgType=&main::TrgType;
	my @UidList=&main::UidList;	
	my $WarningLevel=&main::CompilerOption("MSVC");
	my %Version = &main::Version();
	my $ExtraExportLibrary;
	my $PrimaryExportLibrary = $ExportLibrary;
	unless ($Version{explicit}) {
		$ExtraExportLibrary = $ExportLibrary;
		$ExtraExportLibrary =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
		$PrimaryExportLibrary = $ExtraExportLibrary;
	}

	&Generic_Header(0,$makecmd);	# define standard things using absolute paths

#	set up LinkAs
	$UidList[2]=~/^0x(.*)$/o;
	if ($1 ne '00000000') { # have to make sure than series of noughts in brackets doesn't appear in name for null uids
		$LinkAs=join '', &main::Path_Split('Base',$LinkAs),"[$1]",&main::Path_Split('Ext',$LinkAs);
	}

	my $TrgDir="";

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

	&main::Output(
		"CLFLAGS = /nologo /Zp4 $WarningLevel /Zm256"
		);	
	&main::Output(
		" /X"
		);
	&main::Output(
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
	&main::Output(
		" \$(USERDEFS)\n",
		"\n"
	);

	foreach (@BldList) {
		&main::Output(
			"CL$_ = cl.exe"
		);
		if (/DEB$/o) {
			&main::Output(
				' /Od /Gs0x10000 /Gy /GR- /GX-'
			);
#			euser change to apply inlining on the _NAKED functions
			if ($BaseTrg=~/^EUSER$/oi) {
				&main::Output(
					' /Ob1'
				);
			}
		}
		elsif (/REL$/o) {
#			euser change to prevent minimum size optimisation from disrupting the maths functions
				&main::Output(
					' /O1 /Op /Gs0x10000 /Gy /GF /GR- /GX-'
				);
		}
		&main::Output(
			' $(CLFLAGS)'
		);
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


	# Resource building is done entirely via cl_generic.pm
	
	foreach (@BldList) {
		my $makework="MAKEWORK$_";
		&main::Output(
			"\n",
			"RESOURCE$_ : $makework"
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
					" \"\$(EPOCLIB)\\UREL\\$PrimaryExportLibrary.LIB\"\n"
				);
			}
			else {
				&main::Output(
					"\n",
					"\t\@echo WARNING: Not attempting to create \"\$(EPOCLIB)\\UREL\\$PrimaryExportLibrary.LIB\".\n",
					"\t\@echo When exports are frozen in \"$DefFile\", regenerate Makefile.\n"
				);
			}
		}
		else {
			&main::Output(
				"\n",
				"\t\@echo Not attempting to create \"\$(EPOCLIB)UREL\\$PrimaryExportLibrary.LIB\"\n",
				"\t\@echo from frozen .DEF file, since EXPORTUNFROZEN specified.\n"
			);
		}
		&main::Output(
			"\n",
			"\n",
			"# REAL TARGET - LIBRARY\n",
			"\n",
			"\"\$(EPOCLIB)\\UREL\\$ExportLibrary.LIB\" : \"$DefFile\"\n",
			"\tperl -S prepdef.pl \"$DefFile\" \"\$(EPOCBLD)\\$ExportLibrary.prep.def\"\n",
			"\tlib.exe /nologo /machine:i386 /nodefaultlib /subsystem:native /name:\"$LinkAs\" /def:\"\$(EPOCBLD)\\$ExportLibrary.prep.def\" /out:\"\$(EPOCLIB)\\UREL\\$ExportLibrary.LIB\"\n",
			"\tdel \"\$(EPOCLIB)\\UREL\\$ExportLibrary.exp\"\n"
		);
		if ($ExtraExportLibrary) {
			&main::Output(
				"\n",
				&Generic_Quote("\$(EPOCLIB)\\UREL\\$ExtraExportLibrary.lib"), " : ",
				&Generic_Quote("\$(EPOCLIB)\\UREL\\$ExportLibrary.lib"), "\n",
				"\tcopy ", &Generic_Quote("\$(EPOCLIB)\\UREL\\$ExportLibrary.lib"), " \$@\n"
			);
		}
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
			"\t-\$(ERASE) \"\$(EPOCLIB)\\UREL\\$ExportLibrary.LIB\"\n"
		);
		if ($ExtraExportLibrary) {
			&main::Output(
				"\t-\$(ERASE) \"\$(EPOCLIB)\\UREL\\$ExtraExportLibrary.LIB\"\n"
			);
		}
	}
	&Generic_MakeWorkDir('MAKEWORKLIBRARY',"${LibPath}UREL");

	&Generic_Releaseables;
}

sub PMBld {

	my @ASSPLibList=&main::ASSPLibList;
	my @SrcList=&main::SrcList;
	my $BaseTrg=&main::BaseTrg;
	my $Bld=&main::Bld;
	my $DefFile=&main::DefFile;
	my $EPOCIncPath=&main::EPOCIncPath;
	my $FirstLib=&main::FirstLib;
	my $BasicTrgType=&main::BasicTrgType;
	my @LibList;
	my $LinkAs=&main::LinkAs;
	my $ExportLibrary=&main::ExportLibrary;
	my $NoExportLibrary=&main::NoExportLibrary;
	my $LibPath=&main::LibPath;
	my $RelPath=&main::RelPath;
	my @StatLibList=&main::StatLibList;
	my $Trg=&main::Trg;
	my $TrgPath=&main::TrgPath;
	my $TrgType=&main::TrgType;
	my @UidList=&main::UidList;	
	my $HasExports=0;
	if (-e $DefFile or scalar(&main::Exports)!=0) {
		$HasExports=1;
	}
	my %Version = &main::Version();
	my $ExtraExportLibrary;
	unless ($Version{explicit}) {
		$ExtraExportLibrary = $ExportLibrary;
		$ExtraExportLibrary =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
	}

	my $WarningLevel='/WARN:3';
	$WarningLevel='/WARN:1';    # avoid LNK4005 warnings about unused libraries

	if ($Bld =~ /DEB/) {
		@LibList=&main::DebugLibList;
	} else {
		@LibList=&main::LibList;
	}

#	set up $LinkAs
	$UidList[2]=~/^0x(.*)$/o;
	if ($1 ne '00000000') {	# have to make sure than series of noughts in brackets doesn't appear in name for null uids
		$LinkAs=join '', &main::Path_Split('Base',$LinkAs),"[$1]",&main::Path_Split('Ext',$LinkAs);
	}

	my $BLDTRGPATH = "\$(EPOCTRG$Bld)\\";
	my $BLDDATAPATH = "\$(EPOCTRG$Bld)\\";
	my $Entry="";
	my $Include="";
	my $EntrySymbol='';
	if ($BasicTrgType=~/^DLL$/o) {
		$Entry="_E32Dll";
		$Include="/include:\"__E32Dll\"";
		$EntrySymbol='_E32Dll';
	}
	elsif ($BasicTrgType=~/^EXE$/o) {
		$Entry="_E32Startup";
		$Include="/include:\"__E32Startup\"";
		$EntrySymbol='_E32Startup';
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
	my @releasables;
 	push @releasables, "$RelPath$Trg" if ($BasicTrgType !~ /^IMPLIB$/io);
   	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
 		push @releasables, "$RelPath$Trg.MAP";
	}
	if (-e $DefFile and !$NoExportLibrary) { # effectively "if project frozen ..."
 		push @releasables, "$LibPath$ExportLibrary.LIB";
 		push @releasables, "$LibPath$ExtraExportLibrary.LIB" if ($ExtraExportLibrary);
	}

	&main::Output(
		"WHAT$Bld : WHATGENERIC\n",
		"\n",
		"CLEAN$Bld : CLEANBUILD$Bld CLEANRELEASE$Bld\n",
		"\n",
		"CLEANBUILD$Bld : \n",
		"\t\@perl -w -S ermdir.pl \"\$(EPOCBLD$Bld)\"\n",
		"\t-\$(ERASE) \"$BLDTRGPATH$BaseTrg.ILK\"\n",
		"\n",
		"CLEANRELEASE$Bld : CLEANGENERIC\n",
		"\n"
	);
 	&Generic_WhatCleanTargets($Bld, "WHAT$Bld", "CLEANRELEASE$Bld", @releasables);

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

	&main::Output(
		"LIBS="
	);
	foreach (@StatLibList) {
		&main::Output(
			" \\\n\t\"\$(EPOCSTATLINK$Bld)\\$_\""
		);
	}
	foreach (@ASSPLibList) {
		&main::Output(
			" \\\n\t\"\$(EPOCASSPLINK$Bld)\\$_\""
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
	&main::Output(
		"\n",
		"\n"
	);

	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
		&main::OutFormat(
			"STAGE1_LINK_FLAGS="
		);
		&main::OutFormat(
			"\"\$(EPOCSTATLINK$Bld)\\$FirstLib\" /PDB:NONE"
		);
		if ($BasicTrgType=~/^(DLL)$/o) {	# Add the DLL stub
			&main::OutFormat(
				" \"\$(EPOCSTATLINK$Bld)\\EDLLSTUB.LIB\""
			);
		}
		&main::OutFormat(
			" \$(LIBS) /nologo /entry:$Entry /subsystem:native"
		);
		if ($BasicTrgType=~/^DLL$/o) {
			&main::OutFormat(
				" /dll"
			);
		}
		if ($Bld=~/DEB$/o) {
			&main::OutFormat(
				' /debug'
			);
		}
		&main::OutFormat(
			" /incremental:no /machine:IX86 /nodefaultlib $Include /out:\"\$(EPOCBLD$Bld)\\$Trg\" $WarningLevel",
			" /implib:\"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"",
			' /ignore:4089',		# LNK4089: all references to "dynamic-link library" discarded by /OPT:REF
			' /ignore:4005'			# LNK4005: no objects used from XXX
		);
		&main::Output(
			"\n",
			"\n"
		);
	}

	&main::OutFormat(
		'LINK_FLAGS='
	);
	if ($BasicTrgType=~/^(EXE|DLL)$/o) {
		&main::OutFormat(
			" \"\$(EPOCSTATLINK$Bld)\\$FirstLib\" /PDB:NONE "
		);
		if ($BasicTrgType=~/^(DLL)$/o) {	# Add the DLL stub
			&main::OutFormat(
				" \"\$(EPOCSTATLINK$Bld)\\EDLLSTUB.LIB\""
			);
		}
	}
	&main::OutFormat(
		' $(LIBS) /nologo'
	);
	if ($BasicTrgType=~/^(EXE|DLL)$/o) {
		&main::OutFormat(
			" /fixed:no "
		);
		&main::OutFormat(
			" /entry:$Entry"
		);
	}
	&main::OutFormat(
		' /subsystem:native'
	);
	if ($BasicTrgType=~/^DLL$/o) {
		if ($HasExports) {
			&main::OutFormat(
				" /dll \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\""
			);
		} else {
			&main::OutFormat(
				" /dll"
			);
		}
	} elsif ($TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
		&main::OutFormat(
			" \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\""
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
	&main::OutFormat(
		" /nodefaultlib $Include"
	);
	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		&main::OutFormat(
			" /out:\"\$(EPOCBLD$Bld)\\$Trg\"",
			" /implib:\"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\""
		);
		&main::OutFormat(
			" $WarningLevel"
		);
	} else {
		&main::OutFormat(
			" /out:\"$BLDTRGPATH$Trg\""
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
		&main::Output(
			" \"\$(EPOCSTATLINK$Bld)\\$FirstLib\""
		);
	}
	&main::Output(
		" \$(LIBS)\n"
	);


#	Link by name first time round for dlls
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
		&main::Output(
			"\techo $Entry>\"\$(EPOCBLD$Bld)\\$ExportLibrary.ord\"\n",
			"\tlink.exe \@<<\n",
			"\t\t\$(STAGE1_LINK_FLAGS) /order:@\"\$(EPOCBLD$Bld)\\$ExportLibrary.ord\" \$(LINK_OBJS)\n",
			"<<\n",
			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.ord\"\n",
			"\tdel \"\$(EPOCBLD$Bld)\\$Trg\"\n",
#			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\"\n"
		);

	if ($HasExports) {
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
			if (-e $DefFile) { # effectively "if project frozen ..."
				&main::Output(
					" -Frzfile \"$DefFile\""
				);
			}
			# freeze ordinals, a maximum of 2, for polymorphic dlls
			my $Ordinal;
			my $Num=1;
			foreach $Ordinal (&main::Exports) {
#				replace "$" with "$$" so that NMAKE doesn't think there's a macro in the function name
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
				"\tlib.exe  /nologo /machine:i386 /nodefaultlib /subsystem:native /name:\"$LinkAs\" /def:\"\$(EPOCBLD)\\$ExportLibrary.def\" /out:\"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n"
			);
			if (&main::ExportUnfrozen) {
				&main::Output(
				"\tcopy \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\" \"\$(EPOCLIB)\\UREL\\$ExportLibrary.lib\"\n"
				);
				if ($ExtraExportLibrary) {
					&main::Output(
						"\n",
						"\tcopy \"\$(EPOCLIB)\\UREL\\$ExportLibrary.lib\" ",
						"\"\$(EPOCLIB)\\UREL\\$ExtraExportLibrary.lib\"",
						"\n"
					);						
				}				
			}
			&main::Output(
				"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n"
			);
		}
	}

	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		&main::Output(
			"\techo $Entry>\"\$(EPOCBLD$Bld)\\$ExportLibrary.ord\"\n",
			"\tlink.exe /order:@\"\$(EPOCBLD$Bld)\\$ExportLibrary.ord\" /MAPINFO:EXPORTS /MAP:\"$RelPath$Trg.MAP\" \@<<\n"
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

	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		&main::Output(
			"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.ord\"\n"
		);
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			if ($HasExports) {
				&main::Output(
					"\tdel \"\$(EPOCBLD$Bld)\\$ExportLibrary.exp\"\n"
				);
			}
		}
		&main::Output(
			"\tperl -S findimp.pl \"$RelPath$Trg.MAP\" \"\$(EPOCBLD$Bld)\\$ExportLibrary.imp\"\n"
		);
		&main::Output(
			"\tpetran -x86imp=\"\$(EPOCBLD$Bld)\\$ExportLibrary.imp\" -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " \"\$(EPOCBLD$Bld)\\$Trg\" \$\@ \\\n",
			"\t\t"
		);
		if (&main::AllowDllData) {
			&main::Output(
				' -allow'
			);
		}
		if (not &main::CallDllEntryPoints) {
			&main::Output(
				' -nocall'
			);
		}
		if (&main::DataLinkAddress) {
			&main::Output(
				' -datalinkaddress ',&main::DataLinkAddress
			);
		}
		if (&main::FixedProcess) {
			&main::Output(
				' -fixed'
			);
		}
		if (&main::HeapSize) {
			my %HeapSize=&main::HeapSize;
			&main::Output(
				' -heap ',$HeapSize{Min},' ',$HeapSize{Max}
			);
		}
		if (&main::ProcessPriority) {
			&main::Output(
				' -priority ',&main::ProcessPriority
			);
		}
		if (&main::SmpSafe) {
			&main::Output(
				' -smpsafe'
			);
		}
		if (&main::StackSize) {
			&main::Output(
				' -stack ',&main::StackSize
			);
		}
		my $i=1;
		foreach (@UidList) {
			&main::Output(
				" -uid$i $_"
			);
			$i++;
		}
		if(&main::VendorId) {
			&main::Output(
				' -vid ',&main::VendorId
			);
		}
		&main::Output(
			' -capability ',&main::Capability,
		);
		if (&main::CompressTarget)
		    {
		    &main::Output(
		    " -nocompress"
		    );
		    }
		else
		    {
		    if(&main::CompressTargetMode == NOCOMPRESSIONMETHOD)
			{
				# Do nothing
			}
		    elsif(&main::CompressTargetMode == INFLATECOMPRESSIONMETHOD)
			{
			&main::Output(
				" -compressionmethod deflate"
			);
			}
		    elsif(&main::CompressTargetMode == BYTEPAIRCOMPRESSIONMETHOD)
			{
			&main::Output(
				" -compressionmethod bytepair"
			);
			}

		    }
		if (&main::CodePagingTargetMode == UNPAGED) {
			&main::Output(
				' -codepaging unpaged'
			);
		}
		elsif (&main::CodePagingTargetMode == PAGED) {
			&main::Output(
				' -codepaging paged'
			);
		}

		if (&main::DataPagingTargetMode == UNPAGED) {
			&main::Output(
				' -datapaging unpaged'
			);
		}
		elsif (&main::DataPagingTargetMode == PAGED) {
			&main::Output(
				' -datapaging paged'
			);
		}
		&main::Output("\n");
		&main::Output(
			"\tdel \"\$(EPOCBLD$Bld)\\$Trg\"\n"
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

}

sub PMResrcBld {

	&Generic_ResrcBld;

}

sub PMAifBld {

	&Generic_AifBld;

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
			"\"\$(EPOCBLD$Bld)\\$BaseSrc\_.transd.cia\" : \"$SrcPath$Src\" \$(DEPEND$Bld)\n",
			"\tperl -S tranasm_x86.pl \"$SrcPath$Src\" \$\@\n",
			"\n",
			"\"\$(EPOCBLD$Bld)\\$BaseSrc\_.obj\" \"\$(EPOCBLD$Bld)\\$BaseSrc\_.sbr\" : \"\$(EPOCBLD$Bld)\\$BaseSrc\_.transd.cia\" \$(DEPEND$Bld)\n",
			"\t\$(CL$Bld) /I \"$SrcPath\.\" /TP /Fo\"\$(EPOCBLD$Bld)\\$BaseSrc\_.obj\" /GF /c \"\$(EPOCBLD$Bld)\\$BaseSrc\_.transd.cia\"\n",
			"\n",
#			assembler listing target
			"LISTING$Bld$BaseSrc\_ : \"\$(EPOCBLD$Bld)\\$BaseSrc\_.lis\"\n",
			"\tcopy \$? \"$SrcPath$BaseSrc\_.$Plat.lst\"\n",
			"\n",
			"\"\$(EPOCBLD$Bld)\\$BaseSrc\_.lis\" : \"\$(EPOCBLD$Bld)\\$BaseSrc\_.transd.cia\" \$(DEPEND$Bld)\n",
			"\t\$(CL$Bld) /I \"$SrcPath\.\" /TP /FAsc /Fa\"\$\@\" /Fo\"\$(EPOCBLD$Bld)\\$BaseSrc\_.obj\" /GF /c \"\$(EPOCBLD$Bld)\\$BaseSrc\_.transd.cia\"\n",
			"\n"
		);
	} else {
		&main::Output(
			"\"\$(EPOCBLD$Bld)\\$BaseSrc.obj\" \"\$(EPOCBLD$Bld)\\$BaseSrc.sbr\" : \"$SrcPath$Src\" \$(DEPEND$Bld)\n",
			"\t\$(CL$Bld) /Fo\"\$(EPOCBLD$Bld)/\" /GF /c \"$SrcPath$Src\"\n",
			"\n",
#			assembler listing target
			"LISTING$Bld$BaseSrc : \"\$(EPOCBLD$Bld)\\$BaseSrc.lis\"\n",
			"\tcopy \$? \"$SrcPath$BaseSrc.$Plat.lst\"\n",
			"\n",
			"\"\$(EPOCBLD$Bld)\\$BaseSrc.lis\": \"$SrcPath$Src\" \$(DEPEND$Bld)\n",
			"\t\$(CL$Bld) /FAsc /Fa\"\$\@\" /Fo\"\$(EPOCBLD$Bld)/\" /GF /c \"$SrcPath$Src\"\n",
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

	# Deal with accumulated MAKEDIRS etc.

	&Generic_End;
}

1;
