# Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package Cl_vscw;

# declare variables global for module

my @Win32LibList=();
my $Win32StdHeaders;
my $BaseAddressFlag;
my $Win32Resrc;

my $MWCC;
my $MWLD;
my $MWIncludePath;
my $MWIncludeVar;

# declare variables global for VS6
my %IdeBlds=();
my %PrjHdrs=();
my $DspText='';
my $VcprojText='';
my $PlatName=&main::PlatName;
my $guid='';

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
		PMDoc
		PMStartSrc
		PMSrcDepend
			PMSrcBldDepend
			PMEndSrcBld
		PMEndSrc
	PMEndSrcList
);

use Winutl;
use cl_generic;
use E32Variant;
use lockit_info;

sub RoundUp1k($) {
	# Accept C hexadecimal number (0xNNN).  Convert argument to Kb
	# rounded up to the next 1kb boundary.
	use integer;
	return (hex($_[0]) + 1023) / 1024;
}


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

	# check version of CodeWarrior for Symbian OS
	
	if (defined($ENV{MWCSym2Includes})) {
		$MWCC = "mwccsym2.exe";
		$MWLD = "mwldsym2.exe";
	}
	if (!defined($MWCC) && defined($ENV{MWSym2Libraries})) {
		$MWCC = "mwccsym2.exe";
		$MWLD = "mwldsym2.exe";
	}
	if (!defined($MWCC) && defined($ENV{CWFolder})) {
		$MWCC = "mwcc.exe";
		$MWLD = "mwld.exe";
	}
	if (!defined($MWCC)) {
		die "ERROR: Unable to identify a valid CodeWarrior for Symbian OS installation\n";
	}
	
	# determine default include path
	
	$MWIncludeVar = 'MWCIncludes';	# default, even if it doesn't exist
	$MWIncludePath = '';			# default is empty
	
	foreach my $var ('MWCSym2Includes','MWCWinx86Includes','MWCIncludes') {
		if (defined($ENV{$var})) {
			$MWIncludePath = $ENV{$var};
			$MWIncludeVar = $var;
			last;
		}
	}
}

sub PMPlatProcessMmp (@) {
	&Winutl_DoMmp(\@_, $MWIncludePath);
	$BaseAddressFlag=&Winutl_BaseAddress;
	if ($BaseAddressFlag ne "") {
		$BaseAddressFlag=" -imagebase \"$BaseAddressFlag\"";
	}
	@Win32LibList=&Winutl_Win32LibList;
	$Win32Resrc=&Winutl_Win32Resrc;
	$Win32StdHeaders=&Winutl_Win32StdHeaders;
}

sub PMStartBldList($) {
	my ($makecmd) = @_;
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
	my @MacroList=&main::MacroList();
	push @MacroList, "__SUPPORT_CPP_EXCEPTIONS__";
	my $VariantFile=&main::VariantFile();
	my $ResourceStructRef=&main::ResourceStructRef;
	my $Plat=&main::Plat;
	my $Trg=&main::Trg;
	my $TrgType=&main::TrgType;
	my $WarningLevel=&main::CompilerOption("CW");
	my $LinkAs=&main::LinkAs;

	&main::Output(
			"PATH=$ENV{EPOCROOT}epoc32\\tools\\;$ENV{PATH}",
			"\n",
			"Path=\$(PATH)",
			"\n"
			);

# Get the drive letter to call the Lada compiler to call it with absolute path
	&main::Output(
			"COMPILER_PATH=\"$ENV{EPOCROOT}epoc32\\tools\\nokia_compiler\\Symbian_Tools\\Command_Line_Tools\\\"",
			"\n"
	);

	Generic_Header(0,$makecmd);	# define standard things using absolute paths

	my $TrgDir="";
	my $AifTrgDir="";
	if (&Generic_Definition("TRGDIR") ne "") {
		$TrgDir="\$(TRGDIR)\\";
		$AifTrgDir=$TrgDir;
	}

# Handle migration of binaries to secure location

	&Winutl_AdjustTargetPath(\$TrgDir);
	
# Change - mwwinrc.exe uses MWWinx86Includes or MWCIncludes, but some installations
# fail to install either. Set MWCIncludes from the chosen variable as part
# of the Makefile.
	if (!defined($ENV{MWCIncludes}) && $MWIncludeVar ne 'MWCIncludes') {
		&main::Output(
			"\n",
			"MWCIncludes:=\$($MWIncludeVar)\n",
			"export MWCIncludes\n",
			"\n"
		);
	}

	my $CompilerLibPath="$ENV{EPOCROOT}epoc32\\tools\\nokia_compiler\\Symbian_Support\\Runtime\\Runtime_x86\\Runtime_Win32\\Libs;$ENV{EPOCROOT}epoc32\\tools\\nokia_compiler\\Symbian_Support\\Win32-x86 Support\\Libraries\\Win32 SDK";
	&main::Output(
			"\n",
		"MWLibraries:=\+$CompilerLibPath\n",
		"export MWLibraries\n",
		"\n"
		);
	&main::Output(
			"\n",
		"MWLibraryFiles:=gdi32.lib;user32.lib;kernel32.lib;\n",
		"export MWLibraryFiles\n",
		"\n"
		);
	
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
	    &main::Output("\\\n -i \"$variantFilePath \" -include \"$VariantFile\"");
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
		" -enum int",		# use int for enumeration types
		" -str pool",		# pool strings into a single data object
		" -exc ms",			# SEH C++ exception implementation
		" $WarningLevel"		
	
	);
	if ($Win32StdHeaders or &main::Plat eq 'TOOLS') {
		&main::Output(
			" -stdinc"		# insist because mwccsym2 has -nostdinc as the default(?)
		);
	}
	else {
		&main::Output(
			" -nostdinc"	# insist because mwcc has -stdinc as the default.
		);
	}
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
			"CW$_ = perl -w -S err_formatter.pl \$(COMPILER_PATH)$MWCC -msgstyle parseable "
		);
		if (/DEB$/o) {

			if($PlatName eq "VS6") {
				&main::Output(
					' -sym codeview'
				);
			}
			else
			{
				&main::Output(
					' -sym codeview_new'
				);
			}


#			euser change to apply inlining on the _NAKED functions
			if ($BaseTrg!~/^EUSER$/oi) {
				&main::Output(
					' -inline off'
				);
			}	
		}
		elsif (/REL$/o) {
			&main::Output(
				' -O4,s'	# highest level of optimisation, optimise for space
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

		if ($BasicTrgType !~ /IMPLIB/io) {
			&main::Output (
				" \\\n",
				"\t", &Generic_Quote("\$(EPOCTRG$_)\\$TrgDir$Trg")
			);
			if (&Winutl_CopyForStaticLinkage) {
				&main::Output(
					" \\\n",
					"\t", &Generic_Quote("\$(EPOCTRG$_)\\$Trg")
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
				"\t", &Generic_Quote($file)
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
			my $file="\$(EPOCTRG$_)\\$AifTrgDir$$AifRef{Trg}";
			&Generic_MakeWorkFile($makework,$file);
			&main::Output(
				" \\\n",
				"\t", &Generic_Quote($file)
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
		"\n",
		"# REAL TARGET - LIBRARY\n",
		"\n",
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
				my $LibLinkAs = ($BasicTrgType=~/^IMPLIB$/io) ? $LinkAs : $Trg;
				&main::Output(
					" ", &Generic_Quote("\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib"), "\n",
					"\n",
					&Generic_Quote("\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib"), " : ",
					&Generic_Quote($DefFile), "\n",
					"\tperl -w -S prepdef.pl ",&Generic_Quote($DefFile)," \"\$(EPOCBLD)\\$ExportLibrary.prep.def\"\n",
					"\t\$(COMPILER_PATH)$MWLD -msgstyle gcc \"\$(EPOCBLD)\\$ExportLibrary.prep.def\" -importlib -o \$\@",
					" -addcommand \"out:$LibLinkAs\" -warnings off",
					"\n"
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
	}
	&main::Output(
		"\n",
		"\n",
		"FREEZE :\n"
	);
	if ($DefFile and $BasicTrgType!~/^IMPLIB$/io) {
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
	&main::Output(
		"\n"
	);
	&Generic_MakeWorkDir('MAKEWORKLIBRARY',"${LibPath}UDEB");

	&Generic_Releaseables;

	if($PlatName eq "VS6") {
		VS6StartBldList();
	}
	elsif($PlatName eq "VS2003") 
	{
		VS2003StartBldList();
	}

}

sub VS6StartBldList(){
	my $BaseTrg=&main::BaseTrg;
	my @BldList=&main::BldList;
	my $Plat=&main::Plat;

	# set up global IDE builds variable
	%IdeBlds= (
		UREL=> "$BaseTrg - Win32 Release",
		UDEB=> "$BaseTrg - Win32 Debug",
	);
	if (&main::Plat eq 'TOOLS') {
		%IdeBlds= (
			REL=> "$BaseTrg - Win32 Release",
			DEB=> "$BaseTrg - Win32 Debug",
		);
	}
	

	$DspText=join('',
		"# Microsoft Developer Studio Project File - Name=\"$BaseTrg\" - Package Owner=<4>\n",
		"# Microsoft Developer Studio Generated Build File, Format Version 6.00\n",
		"# ** DO NOT EDIT **\n",
		"\n"
	);

	$DspText.=join('',
		"# TARGTYPE \"Win32 (x86) External Target\" 0x0106\n",
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
	
	foreach (reverse @BldList) {
		$DspText.=join('',
			"!MESSAGE \"",$IdeBlds{$_},"\" (based on \"Win32 (x86) External Target\")"
		);
		$DspText.="\n";
	}

	$DspText.=join('',
		"!MESSAGE \n",
		"\n"
	);

	# BEGIN THE PROJECT
	#------------------
	$DspText.=join('',
		"# Begin Project\n",
		"# PROP AllowPerConfigDependencies 0\n",
		"# PROP Scc_ProjName \"\"\n",
		"# PROP Scc_LocalPath \"\"\n",
		"\n"
	);

}

sub VS2003StartBldList(){
	my $BaseTrg=&main::BaseTrg;
	my @BldList=&main::BldList;
	my $Plat=&main::Plat;

	$guid = `uuidgen.exe`;
	$guid =~ s/\n//g;
	$guid = uc $guid;
# set up global IDE builds variable
	%IdeBlds= (
		UREL=> "Release|Win32",
		UDEB=> "Debug|Win32",
	);
	if ($Plat eq 'TOOLS') {
		%IdeBlds= (
			REL=> "Release|Win32",
			DEB=> "Debug|Win32",
		);
	}
	

	$VcprojText=join('',
		"<?xml version=\"1.0\" encoding=\"Windows-1252\"?>\n",
		"<VisualStudioProject\n",
		"\tProjectType=\"Visual C++\"\n",
		"\tVersion=\"7.10\"\n",
		"\tName=\"$BaseTrg\"\n",
		"\tProjectGUID=\"{$guid}\"\n",
		"\tSccProjectName=\"\"\n",
		"\tSccLocalPath=\"\"\n",
		"\tKeyword=\"MakeFileProj\">\n",
		"\t<Platforms>\n",
		"\t\t<Platform\n",
		"\t\t\tName=\"Win32\"/>\n",
		"\t</Platforms>\n",
		"\t<Configurations>\n",
		"\n"
	);

}

my $uidfile;
sub PMBld {

	my $AifStructRef=&main::AifStructRef;
	my @SrcList=&main::SrcList;
	my $BaseTrg=&main::BaseTrg;
	my $BitMapStructRef=&main::BitMapStructRef;
	my $Bld=&main::Bld;
	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
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

	$uidfile = "$BaseTrg.UID";

	if ($Bld =~ /DEB/) {
		@LibList=&main::DebugLibList;
	} else {
		@LibList=&main::LibList;
	}
 	
# Handle migration of binaries to secure location

	my $BLDTRGPATH = "";
	my $AIFBLDTRGPATH = "";
	if ($TrgPath) {
		$BLDTRGPATH = "\$(TRGDIR)\\";	    # handles TARGETPATH
		$AIFBLDTRGPATH = $BLDTRGPATH;
		&Winutl_AdjustTargetPath(\$BLDTRGPATH);
	}
	$BLDTRGPATH = "\$(EPOCTRG$Bld)\\".$BLDTRGPATH;
	$AIFBLDTRGPATH = "\$(EPOCTRG$Bld)\\".$AIFBLDTRGPATH;

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
			if ($Bld=~/REL$/o && $BasicTrgType!~/^LIB$/o) {
				push @releaseables,"$BLDTRGPATH$Trg.map";
			}
			if (&Winutl_CopyForStaticLinkage) {
				push @releaseables, "\$(EPOCTRG$Bld)\\$Trg";
			}
			my $BitMapRef;
			foreach $BitMapRef (@$BitMapStructRef) {
				push @releaseables, "\$(EPOCTRG$Bld)\\$$BitMapRef{TrgPath}$$BitMapRef{Trg}";
			}
			my $ResourceRef;
			foreach $ResourceRef (@$ResourceStructRef) {
				push @releaseables,"\$(EPOCTRG$Bld)\\$$ResourceRef{Trg}";
			}
			my $AifRef;
			foreach $AifRef (@$AifStructRef) {
				push @releaseables, "$AIFBLDTRGPATH$$AifRef{Trg}";
			}
		}
		if (-e $DefFile and !$NoExportLibrary) { # effectively "if project frozen ..."
			push @releaseables, "\$(EPOCLIB$Bld)\\$ExportLibrary.lib";
		}
		if ($Bld=~/DEB$/o) {
			# debugging support files?
		}
	}
	else {
		if ($BasicTrgType !~ /^IMPLIB$/io) {
			my $toolspath=&main::EPOCToolsPath();
			push @releaseables, "$toolspath$Trg";
		}
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

  my $adjustedTargetPath=$TrgPath;
  &Winutl_AdjustTargetPath(\$adjustedTargetPath);
  &Generic_MakeWorkDir("MAKEWORK$Bld", &main::BldPath);
  &Generic_MakeWorkDir("MAKEWORK$Bld", "$RelPath$adjustedTargetPath");

	&main::Output(
		"\n",
		"\n"
	);

	return if ($BasicTrgType=~/^IMPLIB$/io);

	&main::Output(
		"LISTING$Bld : MAKEWORK$Bld"
	);
	foreach (@SrcList) {
		my $BaseSrc = &main::Path_Split('Base', $_);
		my $Ext = &main::Path_Split('Ext', $_);
		$BaseSrc.='_' if (lc($Ext) eq '.cia');
		$BaseSrc =~ s/\.UID/_UID_/i if ($BaseSrc eq $uidfile);
		
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
	foreach (@StatLibList) {
		&main::Output(
			" \\\n\t", &Generic_Quote("\$(EPOCSTATLINK$Bld)\\$_")
		);
	}
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
		$BaseSrc.='_' if (lc($Ext) eq '.cia');
   		&main::Output(
			" \\\n",
			"\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o")
   		);
   	}
	if ($Win32Resrc) {
		my $resbase=&main::Path_Split('Base',$Win32Resrc);
		&main::Output(
			" \\\n",
			"\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$resbase.res")
		);
	}
	if ($have_uidfile) {
		# ensure that the uid file is at the end of the list, as it's implicit in
		# CodeWarrior IDE projects.
		&main::Output(
			" \\\n",
			"\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg"."_UID_.o")
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::OutFormat(
		"COMMON_LINK_FLAGS$Bld=",
		' -stdlib'		# use default runtime library for compiler help functions
	);
	if ($MWLD eq "mwcc.exe") {
		&main::OutFormat(
			' -warnings on'	# on by default in mwccsym2.exe
		);
	}
	if ($BasicTrgType=~/^(EXE|DLL)$/o) {
		unless (&main::Plat eq 'TOOLS') {
			&main::OutFormat(
				" \"\$(EPOCSTATLINK$Bld)\\$FirstLib\""
			);
		}
	}
	foreach my $lib (@Win32LibList) {
		 my $win32lib = $lib;    # take a copy, to avoid updating contents of Win32LibList!
		$win32lib = "-l$win32lib" unless ($win32lib =~ /\\/);
		&main::OutFormat(
			" ",lc $win32lib
		);
	}
	if ($BasicTrgType=~/^DLL$/o) {
		&main::OutFormat(
			"$BaseAddressFlag",
			' -noentry',
			' -shared'
		);
	}
	elsif ($TrgType=~/^EXEXP$/o) {
		&main::OutFormat(
			"$BaseAddressFlag",
			' -noentry',
			' -shared'
		);
	}
	elsif ($BasicTrgType=~/^EXE$/o) {
		unless (&main::Plat eq 'TOOLS') {
			&main::OutFormat(
				' -m "?_E32Bootstrap@@YGXXZ"'
			);
		}
	}
	if (&main::Plat=~/^(WINC|TOOLS)$/o && $BasicTrgType=~/^EXE$/o) {
		&main::OutFormat(
			' -subsystem console'
		);
	}
	else {
		&main::OutFormat(
			' -subsystem windows'
		);
	}
	if (&main::HeapSize) {
		my %HeapSize=&main::HeapSize;
		&main::OutFormat(
			' -heapreserve=',RoundUp1k($HeapSize{Max}),' -heapcommit=',RoundUp1k($HeapSize{Min})
		);
	}
	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		if ($Bld=~/DEB$/o) {
			if($PlatName eq "VS6")
			{
				&main::OutFormat(
					' -sym codeview'
				);
			}
			else
			{
				&main::OutFormat(
					' -sym codeview_new'
				);
			}
			#mention the debug version of the runtime library
			&main::OutFormat(
				' -lMSL_All_MSE_Symbian_D.lib'
				);
		}
		else
		{
			#mention the release version of the runtime library
			&main::OutFormat(
				' -lMSL_All_MSE_Symbian.lib'
				);
		}
	}
	&main::Output(
		"\n",
		"\n"
	);

	my $EntrySymbol='';
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		my $Include="";
		if ($BasicTrgType=~/^DLL$/o) {
			$Include="-m __E32Dll";
			$EntrySymbol='__E32Dll';
		}
		else {
			$Include="-m __E32Startup";
			$EntrySymbol='__E32Startup';
		}
		&main::Output(
			"STAGE1_LINK_FLAGS$Bld= \$(COMMON_LINK_FLAGS$Bld) \$(LIBS$Bld) \\\n\t",
			" -o \"\$(EPOCBLD$Bld)\\$Trg\"", 
			' -export dllexport',
			" $Include",
			' -nocompactimportlib', 
			" -implib \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"",
			" -addcommand \"out:$Trg\" -warnings off",
			"\n",
		);
	}
	my $AbsentSubst = '';
	if ($EntrySymbol) {
		$AbsentSubst = " -absent $EntrySymbol";
	}

	&main::Output(
		"LINK_FLAGS$Bld= \$(COMMON_LINK_FLAGS$Bld) \$(LIBS$Bld) \\\n\t",
		" -o \"$BLDTRGPATH$Trg\"" 
	);
	if ($Bld=~/REL$/o && $BasicTrgType!~/^LIB$/o) {
		# Generate map file for release build executables
		&main::Output(
			" -map \"$BLDTRGPATH$Trg.map\"", 
		);
	}
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		&main::Output(
			" -f \"\$(EPOCBLD)\\$ExportLibrary.def\"",	# use generated .DEF file
		);
		if (&main::ExportUnfrozen) {
			&main::Output(
				" -implib \"\$(EPOCLIB)\\UDEB\\$ExportLibrary.lib\"",
				" -addcommand \"out:$Trg\" -warnings off"
			);
		}
		else {
			&main::Output(
				' -noimplib'
			);
		}
	}
	else {
		&main::Output(
			' -noimplib'
		);
	}
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
	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		unless (&main::Plat eq 'TOOLS') {
			&main::Output(
				" ", &Generic_Quote("\$(EPOCSTATLINK$Bld)\\$FirstLib")
			);
		}
	}
	&main::Output(
		" \$(LIBS$Bld)\n"
	);


#	Link by name first time round for dlls
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o) {
		&main::Output(
			"\t\$(COMPILER_PATH)$MWLD -msgstyle gcc \$(STAGE1_LINK_FLAGS$Bld) -l \$(EPOCBLD$Bld) -search \$(notdir \$(LINK_OBJS$Bld))\n",
			"\t\$(ERASE) \"\$(EPOCBLD$Bld)\\$Trg\"\n",
		);

#		Generate an export info file
		my $show_options = "names,verbose";
		$show_options = "names,unmangled,verbose" if ($MWLD eq "mwldsym2.exe");
		&main::Output(
			"\t\$(COMPILER_PATH)$MWLD -msgstyle gcc -S -show only,$show_options -o \"\$(EPOCBLD$Bld)\\$ExportLibrary.inf\" \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n"
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
		my $Export;
		my $Ordinal=1;
		foreach $Export (&main::Exports) {
#				replace "$" with "$$" so that NMAKE doesn't think there's a macro in the function name
			$Export=~s-\$-\$\$-go;
			&main::Output(
				" -$Ordinal $Export"
			);
			$Ordinal++;
		}
		&main::Output(
			" \"\$(EPOCBLD)\\$ExportLibrary.def\" \n",
			"\t\$(ERASE) \"\$(EPOCBLD$Bld)\\$ExportLibrary.inf\"\n",
			"\t\$(ERASE) \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\"\n"
		);
	}

#	Perform the final link step
	&main::Output(
		"\t\$(COMPILER_PATH)$MWLD -msgstyle gcc "
	);
	if ($BasicTrgType=~/^LIB$/o) {
		&main::Output(
			"-library "
		);
	}
	&main::Output(
		"\$(LINK_FLAGS$Bld) -l \$(EPOCBLD$Bld) -search \$(notdir \$(LINK_OBJS$Bld))\n",
	);

	if (&main::Plat eq 'TOOLS') {
		&main::Output(
			"\tcopy \"BLDTRGPATH$Trg\" \"",&main::EPOCToolsPath,"$Trg\"\n"
		);
	}
	if (&Winutl_CopyForStaticLinkage) {
		&Generic_MakeWorkDir("MAKEWORK$Bld", "\$(EPOCTRG$Bld)");
		&main::Output(
			"\n",
			&Generic_Quote("\$(EPOCTRG$Bld)\\$Trg"), " : ", 
			&Generic_Quote("$BLDTRGPATH$Trg"), "\n",
			"\t", &Generic_CopyAction(),
		);
	}

	&main::Output(
		"\n",
		"\n"
	);

	if($PlatName eq "VS6") {
		VS6Bld();
	}
	else
	{
		VS2003Bld();
	}
}

sub VS6Bld(){

	my $BaseTrg=&main::BaseTrg;
	my $Bld=&main::Bld;
	my @BldList=&main::BldList;
	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my $ChopRelPath=&main::Path_Chop(&main::RelPath);
	my $Trg=&main::Trg;


	if ($Bld eq $BldList[0]) {
		$DspText.=join('',
			"!IF  \"\$(CFG)\" == \"$IdeBlds{$Bld}\"\n",
			"\n"
		);
	}
	else {
		$DspText.=join('',
			"!ELSEIF  \"\$(CFG)\" == \"$IdeBlds{$Bld}\"\n",
			"\n"
		);
	}
	$DspText.=join('',
		"# PROP BASE Use_MFC 0\n"
	);
	if ($Bld=~/REL$/o) {
		$DspText.=join('',
			"# PROP BASE Use_Debug_Libraries 0\n",
			"# PROP BASE Output_Dir \".\\Release\"\n",
			"# PROP BASE Intermediate_Dir \".\\Release\"\n"
		);
	}
	elsif ($Bld=~/DEB$/o) {
		$DspText.=join('',
			"# PROP BASE Use_Debug_Libraries 1\n",
			"# PROP BASE Output_Dir \".\\Debug\"\n",
			"# PROP BASE Intermediate_Dir \".\\Debug\"\n"
		);
	}

	$DspText.=join('',
		"# PROP BASE Cmd_Line \"NMAKE /f ",$BaseTrg,".mak \"\n",
		"# PROP BASE Rebuild_Opt \"/a\"\n",
		"# PROP BASE Target_File \"",$Trg,"\"\n",
		"# PROP BASE Bsc_Name \"",$BaseTrg,".bsc\"\n",
		"# PROP BASE Target_Dir \"\"\n"
	);

	$DspText.=join('',
			"# PROP Use_MFC\n"
	);
	if ($Bld=~/REL$/o) {
		$DspText.=join('',
			"# PROP Use_Debug_Libraries 0\n"
		);
	}
	elsif ($Bld=~/DEB$/o) {
		$DspText.=join('',
			"# PROP Use_Debug_Libraries 1\n"
		);
	}

	$DspText.=join('',
		"# PROP Output_Dir \"$ChopRelPath\"\n",
		"# PROP Intermediate_Dir \"$ChopBldPath\"\n",
		"# PROP Cmd_Line \"\"$ENV{EPOCROOT}epoc32\\tools\\MAKE.exe\" -r -f ",&main::BaseMak,"_$Bld.mak \"\n",
		"# PROP Rebuild_Opt \"REBUILD\"\n",
		"# PROP Target_File \"",$Trg,"\"\n"
	);

	if ($Bld=~/REL$/o) {
		$DspText.=join('',
			"# PROP Bsc_Name \"",$BaseTrg,".bsc\"\n"
		);
	}
	elsif ($Bld=~/DEB$/o) {
		$DspText.=join('',
			"# PROP Bsc_Name \"\"\n"
		);
	}

	$DspText.=join('',
		"# PROP Target_Dir \"\"\n"
	);

	$DspText.=join('',
		"\n"
	);

}

sub VS2003Bld(){
	my $BaseTrg=&main::BaseTrg;
	my $Bld=&main::Bld;
	my @BldList=&main::BldList;
	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my $ChopRelPath=&main::Path_Chop(&main::Path_MakeRltToBase(&main::MakeFilePath, &main::RelPath));
	my $Trg=&main::Trg;

	$VcprojText.=join('',
		"\t\t<Configuration\n",
		"\t\t\tName=\"",$IdeBlds{$Bld},"\"\n",
		"\t\t\tOutputDirectory=\"$ChopRelPath\"\n",
		"\t\t\tIntermediateDirectory=\"$ChopBldPath\"\n",
		"\t\t\tConfigurationType=\"0\"\n",
		"\t\t\tUseOfMFC=\"0\"\n",
		"\t\t\tATLMinimizesCRunTimeLibraryUsage=\"FALSE\">\n",
		"\t\t\t<Tool\n",
		"\t\t\t\tName=\"VCNMakeTool\"\n",
		"\t\t\t\tBuildCommandLine=\"\&quot;$ENV{EPOCROOT}epoc32\\tools\\make.exe\&quot; -r -f ",&main::BaseMak,".mak $Bld\"\n",
		"\t\t\t\tReBuildCommandLine=\"\&quot;$ENV{EPOCROOT}epoc32\\tools\\make.exe\&quot; -r -f ",&main::BaseMak,".mak CLEAN $Bld \"\n",
		"\t\t\t\tCleanCommandLine=\"\&quot;$ENV{EPOCROOT}epoc32\\tools\\make.exe\&quot; -r -f ",&main::BaseMak,".mak CLEAN$Bld\"\n",
		"\t\t\t\tOutput=\"$ChopRelPath\\$Trg\"/>\n",
		"\t\t</Configuration>\n"
		);

}

sub PMStartSrcList {

	&main::Output(
		"# SOURCES\n",
		"\n"
	);

	if($PlatName eq "VS6") {
		VS6StartSrcList();
	}
	else
	{
		VS2003StartSrcList();
	}
}

sub VS6StartSrcList(){
	my @BldList=&main::BldList;


	$DspText.=join('',
		"\n!ENDIF\n\n",
		"# Begin Target\n",
		"\n"
	);
	foreach (@BldList) {
		$DspText.=join('',
			"# Name \"".$IdeBlds{$_}."\"\n"
		);
	}
	$DspText.=join('',
		"# Begin Group \"Source Files\"\n",
		"\n",
		"# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90\"\n"
	);

}

sub VS2003StartSrcList(){

	$VcprojText.=join('',
		"\t</Configurations>\n",
		"\t<References>\n",
		"\t</References>\n"
	);

	$VcprojText.=join('',
		"\t<Files>\n",
		"\t\t<Filter\n",
		"\t\t\tName=\"Source Files\"\n",
		"\t\t\tFilter=\"cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90\"\n",
		"\t\t\tUniqueIdentifier=\"{4FC737F1-C7A5-4376-A066-2A32D752A2FF}\">\n"
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

	if($PlatName eq "VS6") {
		VS6ResrcBld();
	}
	else
	{
		VS2003ResrcBld();
	}

}

sub VS6ResrcBld() {

	my $ResourceRef=&main::ResourceRef;
	my @BldList=&main::BldList;

	my $Resrc=ucfirst lc &main::Path_Split('File', $$ResourceRef{Source});
	my $BaseResrc=&main::Path_Split('Base', $$ResourceRef{Source});
	my $SrcPath=&main::Path_Split('Path', $$ResourceRef{Source});
	my $TrgPath=&main::Path_Split('Path', $$ResourceRef{Trg});
	my @LangList=($$ResourceRef{Lang});
	
	my $inputpath="$SrcPath$Resrc";
	
	$DspText.=join('',
		"# Begin Source File\n",
		"\n",
		"SOURCE=$inputpath\n"
	);

	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my @DepList=&main::DepList;
	my $PathBaseDsp=&main::MakeFilePath.&main::BaseMak;
	my $RelPath=&main::RelPath;

	$DspText.=join('',
		"USERDEP__$BaseResrc="
	);
	my $Dep;
	foreach $Dep (@DepList) {
		$DspText.=join('',
			"\"$Dep\"\t"
		);
	}
	$DspText.=join('',
		"\n"
	);

	my $Bld;
	foreach $Bld (@BldList) {
		my $ResrcTrgFullName="$RelPath$Bld\\$TrgPath$BaseResrc.r";
		if ($Bld eq $BldList[0]) {
			$DspText.=join('',
				'!IF'
			);
		}
		else {
			$DspText.=join('',
				'!ELSEIF'
			);
		}
		$DspText.=join('',
			"  \"\$(CFG)\" == \"$IdeBlds{$Bld}\"\n",
			"\n"
		);
		$DspText.=join('',
			"# PROP Intermediate_Dir \"$ChopBldPath\"\n",
			"# Begin Custom Build - Building resources from $Resrc\n",
			"InputPath=$inputpath\n",
			"\n",
			"BuildCmds= \\\n",
			"\tnmake -nologo -f \"${PathBaseDsp}.SUP.MAKE\"\\\n",
			"  \"$ResrcTrgFullName\"\n"
		);
		my $Lang;
		foreach $Lang (@LangList) {
#			change again to avoid VC5 linking the resource
			my $TmpLang=$Lang;
			if ($TmpLang eq 'SC') {
				$TmpLang.='.dummy';
			}
			$DspText.=join('',
				"\n",
				"\"$ResrcTrgFullName$TmpLang\" : \$(SOURCE) \"\$(INTDIR)\"\\\n",
				" \"\$(OUTDIR)\"\n",
				"   \$(BuildCmds)\n",
			);
		}
		$DspText.=join('',
			"# End Custom Build\n",
			"\n"
		);
	}
	$DspText.=join('',
		"!ENDIF \n",
		"\n",
		"# End Source File\n"
	);

}

sub VS2003ResrcBld() {

}

sub PMDoc {

###### from PMDoc of VC6 
	
	if($PlatName eq "VS6") {
		VS6Doc();
	}
	else
	{
		VS2003Doc();
	}

}

sub VS6Doc() {

	my $SrcPath=&main::SrcPath;

	$DspText.=join('',
		"# Begin Source File\n",
		"\n",
		"SOURCE=",$SrcPath,ucfirst lc &main::Doc,"\n",
		"# PROP Exclude_From_Build 1\n",
		"# End Source File\n"
	);

}
sub VS2003Doc() {


}

sub PMAifBld {

	&Generic_AifBld;

	# Need to copy generic resource into emulated Z drive

	my $AifRef=&main::AifRef;
	my $TrgDir="";
	if (&Generic_Definition("TRGDIR") ne "") {
		$TrgDir="\\\$(TRGDIR)";
	}

	my @BldList=&main::BldList;
	my $Bld;
	foreach $Bld (@BldList) {
		my $path="\$(EPOCTRG$Bld)$TrgDir";
		&main::Output(
			&Generic_Quote("$path\\$$AifRef{Trg}"), " : ",
			&Generic_Quote("$$AifRef{GenericTrg}"), "\n",
			"\t", &Generic_CopyAction(),
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

	if($PlatName eq "VS6") {
		VS6StartSrc();
	}
	else
	{
		VS2003StartSrc();
	}

}

sub VS6StartSrc() {

	$DspText.=join('',
		"# Begin Source File\n",
		"\n",
		"SOURCE=",&main::SrcPath,ucfirst lc &main::Src,"\n",
		"# End Source File\n"
	);

}

sub VS2003StartSrc(){
	$VcprojText.=join('',
		"\t\t\t<File\n",
		"\t\t\t\tRelativePath=\"",&main::Path_MakeRltToBase(&main::MakeFilePath, &main::SrcPath),ucfirst lc &main::Src,"\">\n",
		"\t\t\t</File>\n"
	);

}

sub PMSrcDepend {
	my @BldList=&main::BldList;	
	my @DepList=&main::DepList;
	my $BaseSrc=&main::BaseSrc;
	$BaseSrc =~ s/\.UID/_UID_/i if ($BaseSrc eq $uidfile);

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

	# Generate user header list for this src, merge with list for all sources
	foreach (&main::DepList) {
		$PrjHdrs{$_}='unusedval';
	}

}

sub PMSrcBldDepend {
	my $Bld=&main::Bld;
	my @DepList=&main::DepList;
	my $BaseSrc=&main::BaseSrc;
	$BaseSrc =~ s/\.UID/_UID_/i if ($BaseSrc eq $uidfile);

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
	$BaseSrc =~ s/\.UID/_UID_/i if ($BaseSrc eq $uidfile);
	my $Bld=&main::Bld;
	my $Plat=&main::Plat;
	my $Src=&main::Src;
	my $SrcPath=&main::SrcPath;
	my $Ext=&main::ExtSrc;
	my $Cia = (lc($Ext) eq '.cia') ? 1 : 0;

	if ($Cia) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.o"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\techo $Src\n",
			"\t\$(CW$Bld) -lang c++ -o \"\$\@\" -c \"$SrcPath$Src\"\n",
			"\n",
#			assembler listing target - uses implicit rule to do disassembly
			"LISTING$Bld$BaseSrc\_ : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc\_.$Plat.lst"),
			"\n"
		);
	} else {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\techo $Src\n",
			"\t\$(CW$Bld) -o \"\$\@\" -c \"$SrcPath$Src\"\n",
			"\n",
#			assembler listing target - uses implicit rule to do disassembly
			"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc.$Plat.lst"),
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

	my $show_options = "source";
	$show_options = "source,unmangled,comments" if ($MWLD eq "mwldsym2.exe");
	
	&main::Output(
		"\n",
		"# Implicit rule for generating .lis files\n",
		"\n",
		".SUFFIXES : .lis .o\n",
		"\n",
		".o.lis:\n",
		"\t\$(COMPILER_PATH)$MWLD -msgstyle gcc -S -show $show_options \$< -o \$\@\n",
		"\n",
		"\n"
	);

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
				" \\\n\t", &Generic_Quote($_)
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
	
		my $Bld;
		my $resbase=&main::Path_Split('Base',$Win32Resrc);
		my $respath=&main::Path_Chop(&main::Path_Split('Path',$Win32Resrc));
		foreach $Bld (@BldList) {
			&main::Output(
				&Generic_Quote("\$(EPOCBLD$Bld)\\$resbase.res"), " : ",
				&Generic_Quote($Win32Resrc), " \$(DEPEND)\n",
				"\tmwwinrc -o \$\@ \"$Win32Resrc\"\n",
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

	my $BaseDsp=&main::BaseMak;
	my $PathBaseDsp=&main::MakeFilePath.$BaseDsp;

	&main::Path_DelFiles("$PathBaseDsp.MAK","$PathBaseDsp.MDP","$PathBaseDsp.NCB","$PathBaseDsp.OPT","$PathBaseDsp.PLG","$PathBaseDsp.SLN","$PathBaseDsp.VCPROJ","$PathBaseDsp.DSW","$PathBaseDsp.DSP","${PathBaseDsp}_UREL.MAK","${PathBaseDsp}_UDEB.MAK");

	if($PlatName eq "VS6")
	{
		VS6EndSrcList();
	}
	elsif($PlatName eq "VS2003")
	{
		VS2003EndSrcList();
	}
}

sub VS6EndSrcList() {

	my $BaseDsp=&main::BaseMak;    
	my $PathBaseDsp=&main::MakeFilePath.$BaseDsp;  
	my $BaseTrg=&main::BaseTrg;

	$DspText.=join('',
		"# End Group\n",
		"# Begin Group \"Resource Files\"\n",
		"\n",
		"# PROP Default_Filter \"ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe\"\n"
	);
	if ($Win32Resrc) {
		$DspText.=join('',
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
	$DspText.=join('',
		"# End Group\n"
	);

	# Use the global %PrjHdrs Hash to produce user header listing
	$DspText.=join('',
		"# Begin Group \"Header Files\"\n",
		"\n",
		"# PROP Default_Filter \"h;hpp;hxx;hm;inl;fi;fd\"\n"
	);
	foreach (keys %PrjHdrs) {
		$DspText.=join('',
			"# Begin Source File\n",
			"\n",
			"SOURCE=",&main::Path_Split('Path',$_),ucfirst lc &main::Path_Split('File',$_),"\n",
			"# End Source File\n"
		);
	}


	$DspText.=join('',
		"# End Group\n",
		"# Begin Group \"Make Files\"\n",
		"\n",
		"# PROP Default_Filter \"mak;mk\"\n"
	);
	$DspText.=join('',
		"# Begin Source File\n",
		"\n",
		"SOURCE=",ucfirst lc $PathBaseDsp,".mak\n",
		"# End Source File\n"
	);

	$DspText.=join('',
		"# End Group\n",
		"# End Target\n",
		"# End Project\n",
	);


	&main::CreateExtraFile("$PathBaseDsp.dsp",$DspText);

	my $RelMakText=join('',
		"\n",
		"BLD: UREL\n",
		"REBUILD : CLEANUREL UREL\n",
		"\n",
		"include ",&main::BaseMak,".mak\n",
		"\n"
	);

	&main::CreateExtraFile("${PathBaseDsp}_UREL.mak",$RelMakText);


	my $DebMakText=join('',
		"\n",
		"BLD: UDEB\n",
		"REBUILD : CLEANUDEB UDEB\n",
		"\n",
		"include ",&main::BaseMak,".mak\n",
		"\n"
	);

	&main::CreateExtraFile("${PathBaseDsp}_UDEB.mak",$DebMakText);

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

sub VS2003EndSrcList() {

	my $BaseDsp=&main::BaseMak;    
	my $PathBaseDsp=&main::MakeFilePath.$BaseDsp;  

# Use the global %PrjHdrs Hash to produce user header listing
	$VcprojText.=join('',
		"\t\t</Filter>\n",
		"\t\t<Filter\n",
		"\t\t\tName=\"Header Files\"\n",
		"\t\t\tFilter=\"h;hpp;hxx;hm;inl;fi;fd\"\n",
		"\t\t\tUniqueIdentifier=\"{93995380-89BD-4b04-88EB-625FBE52EBFB}\">\n"
	);

	foreach (keys %PrjHdrs) {
		$VcprojText.=join('',
			"\t\t\t<File\n",
			"\t\t\t\tRelativePath=\"",&main::Path_MakeRltToBase(&main::MakeFilePath, &main::Path_Split('Path',$_)) 
							, &main::Path_Split('File',$_),"\">","\n",
			"\t\t\t</File>\n"
		);
	}	
	$VcprojText.=join('',
		"\t\t</Filter>\n"
	);

	$VcprojText.=join('',
		"\t\t<Filter\n",
		"\t\t\tName=\"Resource Files\"\n",
		"\t\t\tFilter=\"ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe\"\n",
		"\t\t\tUniqueIdentifier=\"{67DA6AB6-F800-4c08-8B7A-83BB121AAD01}\">\n"
	);

	if ($Win32Resrc) {
		$VcprojText.=join('',
			"\t\t\t<File\n",
			"\t\t\t\tRelativePath=\"", &main::Path_MakeRltToBase(&main::MakeFilePath, $Win32Resrc),"\">\n",
			"\t\t\t</File>\n"
		);
		# Generate user header list for this src, merge with list for all sources
		foreach (&main::Deps_GenDependsL($Win32Resrc)) {
			$PrjHdrs{$_}='unusedval';
		}
	}

	$VcprojText.=join('',
		"\t\t</Filter>\n",
		"\t\t<Filter\n",
		"\t\t\tName=\"Make Files\"\n",
		"\t\t\tFilter=\"Mak;mk\">\n"
	);

	$VcprojText.=join('',
		"\t\t\t<File\n",
		"\t\t\t\tRelativePath=\"",&main::Path_MakeRltToBase(&main::MakeFilePath, $PathBaseDsp),".mak\">\n",
		"\t\t\t</File>\n"
	);

	$VcprojText.=join('',
		"\t\t</Filter>\n",
		"\t</Files>\n",
		"\t<Globals>\n",
		"\t</Globals>\n",
		"</VisualStudioProject>\n"
	);

	&main::CreateExtraFile("$PathBaseDsp.vcproj",$VcprojText);


	my $SlnText=join(
		"\n",
		"Microsoft Visual Studio Solution File, Format Version 8.00",
		"Project\(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\"\) = \"$BaseDsp\", \"$BaseDsp.vcproj\", \"{$guid}\"",
		"\tProjectSection(ProjectDependencies) = postProject",
		"\tEndProjectSection",
		'EndProject',
		'Global',
		"\tGlobalSection(SolutionConfiguration) = preSolution",
		"\t\tDebug = Debug",
		"\t\tRelease = Release",
		"\tEndGlobalSection",
		"\tGlobalSection(ProjectConfiguration) = postSolution",
		"\t\t{$guid}.Debug.ActiveCfg = Debug|Win32",
		"\t\t{$guid}.Debug.Build.0 = Debug|Win32",
		"\t\t{$guid}.Release.ActiveCfg = Release|Win32",
		"\t\t{$guid}.Release.Build.0 = Release|Win32",
		"\tEndGlobalSection",
		"\tGlobalSection(ExtensibilityGlobals) = postSolution",
		"\tEndGlobalSection",
		"\tGlobalSection(ExtensibilityAddIns) = postSolution",
		"\tEndGlobalSection",
		'EndGlobal',
		''
	);

	&main::CreateExtraFile("$PathBaseDsp.Sln",$SlnText);

}

1;
