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


package Cl_gcc;

my $GccPrefix='';
my $ToolPrefix='';
my $HelperLib='';
my %PlatOpt=(
	'Dlltool'=>'',
	'Entry'=>'-e',
	'Gcc'=>'',
	'Ld'=>'',
	'Petran'=>'',
	'Optimize'=>'-O'
);
my $Dlltool;
my $Archive;
my $Link;
my $Objcopy;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	PMHelp_Mmp

	PMPlatProcessMmp

	PMUnderlyingABI

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
use cl_generic;
use Genutl;

use constant NOCOMPRESSIONMETHOD => 0;
use constant INFLATECOMPRESSIONMETHOD => 1;
use constant BYTEPAIRCOMPRESSIONMETHOD => 2;

use constant NOTPAGED => 0;
use constant UNPAGED => 1;
use constant PAGED => 2;
sub PMHelp_Mmp {
	print "// No additional keywords for this platform\n";
}

sub PMPlatProcessMmp (@) {
	my $MMPFILE=&main::MmpFile;

	# set up START MARM ... END block module variables
	my @MmpWarn=();
	my $Line;
	LINE: foreach $Line (@_) {
		my $LineInfo=shift @$Line;
		$_=shift @$Line;
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

sub SystemTarget() {
	return 1 if &main::SystemTrg;
	my $ExportLibrary=&main::ExportLibrary;
	# N.B. should get better way to detect kernel probably!!
	return 1 if ($ExportLibrary =~ /EKERN/i);
	
	return 0;
}

my %CompatibleABIs=(
	ARMI=>['ARM4', 'THUMB'],
	ARM4=>['ARMI'],
	THUMB=>['ARMI']
);
my @CompatibleABIs;
sub PMUnderlyingABI($) {
	my ($ABI) = @_;
	if ($ABI eq 'ARM4T') {
		if (&main::BuildAsARM) {
			return 'ARMI';
		}
		elsif (SystemTarget()) {
			return 'ARM4';
		}
		else {
			return 'THUMB';
		}
	}
	return $ABI;
}

my $Makecmd;
my %ABILibPath=();
sub PMStartBldList($) {
	($Makecmd) = @_;
	my $ABI=&main::ABI;
	my $UnderlyingABI=PMUnderlyingABI($ABI);
	my $BaseTrg=&main::BaseTrg;
	my $BasicTrgType=&main::BasicTrgType;
	my @BldList=&main::BldList;
	my @ChopRTWSysIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::SysIncPaths));
	my @ChopRTWUserIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::UserIncPaths));
	my $DefFile=&main::DefFile;
	my $EPOCPath=&main::EPOCPath;
	my $LinkAs=&main::LinkAs;
	my $LibPath=&main::LibPath;
	my @MacroList=&main::MacroList();
	my $VariantFile=&main::VariantFile();
	my $Plat=&main::Plat;
	my $Trg=&main::Trg;
	my $TrgType=&main::TrgType;
	my @UidList=&main::UidList;	
	my $WarningLevel=&main::CompilerOption("GCC");
	my $ExportLibrary=&main::ExportLibrary;
	my $NoExportLibrary=&main::NoExportLibrary;
	my $SystemTrg = SystemTarget();
	my %Version = &main::Version();
	my $ExtraExportLibrary;
	my $PrimaryExportLibrary = $ExportLibrary;
	unless ($Version{explicit}) {
		$ExtraExportLibrary = $ExportLibrary;
		$ExtraExportLibrary =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
		$PrimaryExportLibrary = $ExtraExportLibrary;
	}

#	set up LinkAs
	$UidList[2]=~/^0x(.*)$/o;
	if ($1 ne '00000000') { # have to make sure than series of noughts in brackets doesn't appear in name for null uids
		$LinkAs=join '', &main::Path_Split('Base',$LinkAs),"[$1]",&main::Path_Split('Ext',$LinkAs);
	}

#	set up dlltool flag hash
	my %ABIDlltool=(
		ARMI=>'-m arm_interwork',
		ARM4=>'-m arm',
		THUMB=>'-m thumb'
	);

#	work out the flags for various platforms
	if ($ABI eq 'ARMI') {
		$PlatOpt{Gcc}='-march=armv4t -mthumb-interwork';
		$PlatOpt{Dlltool}=$ABIDlltool{ARMI};
	}
	elsif ($ABI eq 'ARM4T') {
		if (&main::BuildAsARM) {
			$PlatOpt{Gcc}='-march=armv4t -mthumb-interwork';
			$PlatOpt{Dlltool}=$ABIDlltool{ARMI};
		}
		elsif ($SystemTrg) {
			$PlatOpt{Gcc}='-march=armv4';
#			allow thumb for ARM4 ABI where necessary
			unless (&main::PlatABI eq 'ARM4') {
				$PlatOpt{Gcc}.='t';
			}
			$PlatOpt{Dlltool}=$ABIDlltool{ARM4};
		}
		else {
			$GccPrefix='thumb-epoc-pe-';
			$PlatOpt{Gcc}='-mthumb-interwork -D__MARM_THUMB__';
			$PlatOpt{Dlltool}=$ABIDlltool{THUMB};
		}
	}
	elsif ($ABI eq 'ARM4') {
		$PlatOpt{Gcc}='-march=armv4';
#		allow thumb for ARM4 ABI where necessary
		unless (&main::PlatABI eq 'ARM4') {
			$PlatOpt{Gcc}.='t';
		}
		$PlatOpt{Dlltool}=$ABIDlltool{ARM4};
	}
	elsif ($ABI eq 'THUMB') {
		$GccPrefix='thumb-epoc-pe-';
		$PlatOpt{Gcc}='-mthumb-interwork';
		$PlatOpt{Dlltool}=$ABIDlltool{THUMB};
	}
	else {
		&main::FatalError("Platform module - ABI \"$ABI\" unrecognised");
	}
	
	@CompatibleABIs=@{$CompatibleABIs{$UnderlyingABI}};

#	set up CompatibleABI lib path hash
	foreach (@CompatibleABIs) {
		$ABILibPath{$_}=&main::Path_Strip("$LibPath..\\$_\\");
	}
	my %ABIRTWLibPath=();
	foreach (@CompatibleABIs) {
		$ABIRTWLibPath{$_}=&main::Path_RltToWork($ABILibPath{$_});
	}

	$Dlltool=$ToolPrefix.'dlltool';
	$Archive=$ToolPrefix.'ar';
	$Link=$ToolPrefix.'ld';
	$Objcopy=$ToolPrefix.'objcopy';

	&Generic_Header(1,$Makecmd);	# define standard things using relative paths

#	GCC needs a fully-qualified path
	if ($ENV{'PATH'} !~ m/[a-zA-z]{1}:{1}.[^;]*epoc32\\gcc\\bin/i) {
		if ($Makecmd eq "nmake") {
			&main::Output(
				"\n",
				"PATH=",&main::Path_Drive,$EPOCPath,"gcc\\bin;\$(PATH)\n",
				"\n"
			);
		}
		else {
			&main::Output(
				"\n",
				"# must set both PATH and Path to make it work correctly\n",
				"Path:=",&main::Path_Drive,$EPOCPath,"gcc\\bin;\$(Path)\n",
				"PATH:=\$(Path)\n",
				"\n"
			);
		}
	}

	&main::Output(
		"INCDIR  ="
	);

	foreach (@ChopRTWUserIncPaths) {
		&main::Output(
			" -I \"$_\""
		);
	}
	foreach (@ChopRTWSysIncPaths) {
		&main::Output(
			" -I \"$_\""
		);
	}
	if($VariantFile){
	    # gcc needs a relativ path
	    $VariantFile = &main::Path_RltToWork($VariantFile);
	    &main::Output("\\\n  -include \"$VariantFile\"");
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"GCCFLAGS=$PlatOpt{Gcc} \\\n",
		"\t\t-pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas $WarningLevel\n",
		"\n"
	);

	&main::Output(
		"GCCDEFS ="
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

	foreach (@BldList) {
		&main::Output(
			"GCC$_ = ${GccPrefix}gcc"
		);
		if (/REL$/o) {
			&main::Output(
				      ' -s -fomit-frame-pointer ',
				      $PlatOpt{Optimize}
			);
		}
		elsif (/DEB$/o) {
			&main::Output(
				' -g'
			);
			unless (&main::SrcDbg) {
			    &main::Output(
			    	' ', $PlatOpt{Optimize}
			    );
			}
		}
		&main::Output(
			' $(GCCFLAGS)'
		);
		foreach (&main::MacroList($_)) {
			&main::Output(
				" -D$_"
			);
		}
		&main::Output(
			" \$(GCCDEFS)\n"
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
				" \\\n\t",
				&Generic_Quote("\$(EPOCTRG$_)\\$Trg")
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
		&main::Output(
			"\n",
			"RESOURCE$_ : MAKEWORK$_"
		);
	}
	&main::Output(
		"\n",
		"\n",
	);

	&main::Output(
		"LIBRARY : MAKEWORK"
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
					" ", &Generic_Quote("\$(EPOCLIB)\\UREL\\$PrimaryExportLibrary.lib")
				);
				foreach (@CompatibleABIs) {
					&main::Output(
						" ", &Generic_Quote("$ABILibPath{$_}UREL\\$PrimaryExportLibrary.lib")
					);
					&Generic_MakeWorkDir('MAKEWORKLIBRARY',"$ABILibPath{$_}UREL");
				}
				&main::Output(
					"\n"
				);
			} else {
				&main::Output(
					"\n",
					"\t\@echo WARNING: Not attempting to create any import libraries.\n",
					"\t\@echo When exports are frozen in \"$DefFile\", regenerate Makefile.\n"
				);
			}
		}
		else {
			&main::Output(
				"\n",
				"\t\@echo Not attempting to create \"\$(EPOCLIB)\\UREL\\$PrimaryExportLibrary.lib\"\n",
				"\t\@echo from frozen .DEF file, since EXPORTUNFROZEN specified.\n"
			);
		}
		&main::Output(
			"\n",
			"\n",
			"# REAL TARGET - LIBRARY\n",
			"# EPOCBLDP = ", &Generic_Definition('EPOCBLDP'), "\n",
			"# EPOCLIB = ", &Generic_Definition('EPOCLIB'), "\n",
		);
		&main::Output(
			"\n",
			&Generic_Quote("\$(EPOCBLDP)\\$ExportLibrary.prep.def"), " : ",
			&Generic_Quote($DefFile), "\n",
			"\tperl -S prepdef.pl \$< \$@\n",
			"\n",
			&Generic_Quote("\$(EPOCLIB)\\UREL\\$ExportLibrary.lib"), " : ",
			&Generic_Quote("\$(EPOCBLDP)\\$ExportLibrary.prep.def"), "\n",
			"\tpushd ".&Generic_Quote("\$(EPOCBLDP)")." && \\\n",
			"\t$Dlltool $PlatOpt{Dlltool} --output-lib \"$ExportLibrary.tmp.lib\" --def \"$ExportLibrary.prep.def\" --dllname \"$LinkAs\" && popd && move ", &Generic_Quote("\$(EPOCBLDP)\\$ExportLibrary.tmp.lib"), " \$@\n"
		);
		if ($ExtraExportLibrary) {
			&main::Output(
				"\n",
				&Generic_Quote("\$(EPOCLIB)\\UREL\\$ExtraExportLibrary.lib"), " : ",
				&Generic_Quote("\$(EPOCLIB)\\UREL\\$ExportLibrary.lib"), "\n",
				"\tcopy \$< \$@\n"
			);
		}
		foreach (@CompatibleABIs) {
			&main::Output(
				"\n",
				&Generic_Quote("$ABILibPath{$_}UREL\\$ExportLibrary.lib"), " : ",
				&Generic_Quote("\$(EPOCBLDP)\\$ExportLibrary.prep.def"), "\n",
				"\tpushd ".&Generic_Quote("\$(EPOCBLDP)")." && \\\n",
				"\t$Dlltool $ABIDlltool{$_} --output-lib \"$ExportLibrary.tmp.lib\" \\\n",
				"\t\t--def \"$ExportLibrary.prep.def\" \\\n",
				"\t\t--dllname \"$LinkAs\" && popd && move ", &Generic_Quote("\$(EPOCBLDP)\\$ExportLibrary.tmp.lib"), " \$@\n"
			);
			if ($ExtraExportLibrary) {
				&main::Output(
					"\n",
					&Generic_Quote("$ABILibPath{$_}UREL\\$ExtraExportLibrary.lib"), " : ",
					&Generic_Quote("$ABILibPath{$_}UREL\\$ExportLibrary.lib"), "\n",
					"\tcopy \$< \$@\n"
				);
			}
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
			"\tperl -w -S efreeze.pl \$(EFREEZE_ALLOW_REMOVE) \"$DefFile\" \"\$(EPOCBLDP)\\$ExportLibrary.def\" \n"
		);
	}
	&main::Output(
		"\n",
		"CLEANLIBRARY :\n"
	);
	if ($DefFile and !$NoExportLibrary) {
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCLIB)\\UREL\\$ExportLibrary.lib\"\n",
			"\t-\$(ERASE) \"\$(EPOCBLDP)\\$ExportLibrary.prep.def\"\n"
		);
		if ($ExtraExportLibrary) {
			&main::Output(
				"\t-\$(ERASE) \"\$(EPOCLIB)\\UREL\\$ExtraExportLibrary.lib\"\n"
			);
		}
		foreach (@CompatibleABIs) {
			&main::Output(
				"\t-\$(ERASE) \"$ABILibPath{$_}UREL\\$ExportLibrary.lib\"\n"
			);
			if ($ExtraExportLibrary) {
				&main::Output(
					"\t-\$(ERASE) \"$ABILibPath{$_}UREL\\$ExtraExportLibrary.lib\"\n"
				);
			}
		}
	}
	&main::Output(
		"\n",
		"\n"
	);
	&Generic_MakeWorkDir('MAKEWORKLIBRARY',"${LibPath}UREL");

	&Generic_Releaseables;
}


sub PMBld {

	my @ASSPLibList=&main::ASSPLibList;
	my @SrcList=&main::SrcList;
	my $BaseTrg=&main::BaseTrg;
	my $Bld=&main::Bld;
	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my $DefFile=&main::DefFile;
	my $EPOCIncPath=&main::EPOCIncPath;
	my $FirstLib=&main::FirstLib;
	my $BasicTrgType=&main::BasicTrgType;
	my @LibList;
	my $LibPath=&main::LibPath;
	my $LinkAs=&main::LinkAs;
	my $ChopRelPath=&main::Path_Chop(&main::RelPath);
	my $RelPath=&main::RelPath;
	my @StatLibList=&main::StatLibList;
	my $StatLinkPath=&main::StatLinkPath;
	my $Trg=&main::Trg;
	my $TrgType=&main::TrgType;
	my @UidList=&main::UidList;
	my $ExportLibrary=&main::ExportLibrary;
	my $NoExportLibrary=&main::NoExportLibrary;
	my $SystemTrg = SystemTarget();
	my %Version = &main::Version();
	my $ExtraExportLibrary;
	unless ($Version{explicit}) {
		$ExtraExportLibrary = $ExportLibrary;
		$ExtraExportLibrary =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
	}

	if ($Bld =~ /DEB/) {
		@LibList = &main::DebugLibList;
	} else {
		@LibList = &main::LibList;
	}

#	set up $LinkAs
	$UidList[2]=~/^0x(.*)$/o;
	if ($1 ne '00000000') {	# have to make sure than series of noughts in brackets doesn't appear in name for null uids
		$LinkAs=join '', &main::Path_Split('Base',$LinkAs),"[$1]",&main::Path_Split('Ext',$LinkAs);
	}


	# REAL TARGETS
	#-------------
	&main::Output(
		"# REAL TARGET - BUILD VARIANT $Bld\n",
		"\n"
	);

#	releasables
	my @releaseables;
	

	push @releaseables, "$RelPath$Trg" if ($BasicTrgType!~/^IMPLIB$/io);
	if ($BasicTrgType=~/^(DLL|EXE)$/o) {
		push @releaseables, "$RelPath$Trg.MAP";
	}
	if (-e $DefFile and !$NoExportLibrary) { # effectively "if project frozen ..."
		push @releaseables, "$LibPath$ExportLibrary.lib";
		push @releaseables, "$LibPath$ExtraExportLibrary.lib" if ($ExtraExportLibrary);
		foreach (@CompatibleABIs) {
			push @releaseables, "$ABILibPath{$_}UREL\\$ExportLibrary.lib";
			push @releaseables, "$ABILibPath{$_}UREL\\$ExtraExportLibrary.lib" if ($ExtraExportLibrary);
		}
	}

	&main::Output(
		"WHAT$Bld : WHATGENERIC\n",
		"\n",
		"CLEAN$Bld : CLEANBUILD$Bld CLEANRELEASE$Bld\n",
		"\n",
		"CLEANBUILD$Bld : \n",
		"\t\@perl -w -S ermdir.pl \"\$(EPOCBLDP$Bld)\"\n",
		"\n",
		"CLEANRELEASE$Bld : CLEANGENERIC\n",
		"\n"
	);
	&Generic_WhatCleanTargets($Bld, "WHAT$Bld", "CLEANRELEASE$Bld", @releaseables);

	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopBldPath);
	if (defined $ENV{PBUILDPID}) {
		my $ChopBldPPath = $ChopBldPath;
		$ChopBldPPath =~ s/(.*)\\(\w+)\\$Bld$/$1\\$2\$\(PBUILDPID\)\\$Bld/i;
		&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopBldPPath);
	}
	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopRelPath);

	return if ($BasicTrgType=~/^IMPLIB$/io);

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
		"LIBS$Bld="
	);
	if ($BasicTrgType=~/^DLL$/o) { # Add the DLL stub library
		&main::Output(
			" \\\n\t",
			&Generic_Quote("\$(EPOCSTATLINK$Bld)\\EDLLSTUB.LIB")
		);
	}
	if ($HelperLib) {
		&main::Output(
			" \\\n\t",
			&Generic_Quote("\$(EPOCSTATLINK$Bld)\\$HelperLib")
		);
	}
	foreach (@StatLibList) {
		&main::Output(
			" \\\n\t",
			&Generic_Quote("\$(EPOCSTATLINK$Bld)\\$_")
		);
	}
	if ($BasicTrgType=~/^(DLL|EXE)/o) { # Add the GCC helper fns
		&main::Output(
			" \\\n\t",
			&Generic_Quote("\$(EPOCSTATLINK$Bld)\\EGCC.LIB")
		);
	}
	foreach (@ASSPLibList) {
		&main::Output(
			" \\\n\t",
			&Generic_Quote("\$(EPOCASSPLINK$Bld)\\$_")
		);
	}
	foreach (@LibList) {
		&main::Output(
			" \\\n\t",
			&Generic_Quote("\$(EPOCLINK$Bld)\\$_")
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output(
		"# EPOCBLDP$Bld = ", &main::Path_AbsToWork(&Generic_Definition("EPOCBLDP$Bld")), "\n",
		"# EPOCSTATLINK$Bld = ", &main::Path_AbsToWork(&Generic_Definition("EPOCSTATLINK$Bld")), "\n",
		"# EPOCLIB = ", &main::Path_AbsToWork(&Generic_Definition("EPOCLIB")), "\n",
	);

	&main::Output(
		&Generic_Quote("\$(EPOCTRG$Bld)\\$Trg"), " : ",
		&Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseTrg.in")
	);
	if (-e $DefFile) { # effectively "if project frozen ..."
		&main::Output(
			" ", &Generic_Quote($DefFile)
		);
	}
	if ($BasicTrgType=~/^(EXE|DLL)$/o) {
		&main::Output(
			" ", &Generic_Quote("\$(EPOCSTATLINK$Bld)\\$FirstLib")
		);
	}
	&main::Output(
		" \$(LIBS$Bld)\n"
	);


#	Establish the entry point symbol
	my $EntrySymbol;
	if ($BasicTrgType=~/^DLL$/o) {
		$EntrySymbol = '_E32Dll';
	}
	elsif ($BasicTrgType=~/^EXE$/o) {
		$EntrySymbol = '_E32Startup';
	}
	my $AbsentSubst = '';
	if ($EntrySymbol) {
		$AbsentSubst = " -absent $EntrySymbol";
	}

	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {

#		generate a .DEF file from the objects and static libraries
		&main::Output(
			"\tpushd ".&Generic_Quote("\$(EPOCBLDP$Bld)")." && \\\n",
			"\t$Dlltool $PlatOpt{Dlltool} --output-def \"$ExportLibrary.inf\" \"$BaseTrg.in\""
		);
		foreach (@StatLibList) {
			&main::Output(
				" \"", &main::Path_MakeRltToBase(&main::Path_AbsToWork(&Generic_Definition("EPOCBLDP$Bld")."\\"), &main::Path_AbsToWork(&Generic_Definition("EPOCSTATLINK$Bld")."\\"))."$_\""
			);
		}
		&main::Output(
			" && popd\n"
		);

#		reorder the .DEF file taking frozen exports into account if there are any
		&main::Output(
#			call perl on the script here so nmake will die if there are errors - this doesn't happen if calling perl in a batch file
			"\tperl -w -S makedef.pl -Deffile \"\$(EPOCBLDP$Bld)\\$ExportLibrary.inf\"$AbsentSubst"
		);
		if (SystemTarget()) {
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
			&main::Output(
				" -$Num $Ordinal"
			);
			$Num++;
		}
		&main::Output(
			"  \"\$(EPOCBLDP)\\$ExportLibrary.def\"\n"
		);

#		delete the unordered definition file
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCBLDP$Bld)\\$ExportLibrary.inf\"\n"
		);

#		generate an export object from the ordered .DEF file
		&main::Output(
			"\tpushd ".&Generic_Quote("\$(EPOCBLDP$Bld)")." && \\\n",
			"\t$Dlltool $PlatOpt{Dlltool} --def \"..\\$ExportLibrary.def\" \\\n",
			"\t\t--output-exp \"$ExportLibrary.exp\" \\\n",
			"\t\t--dllname \"$LinkAs\""
		);
		if (&main::ExportUnfrozen) {
			&main::Output(
				"\\\n",
				"\t\t--output-lib \"", &main::Path_MakeRltToBase(&main::Path_AbsToWork(&Generic_Definition("EPOCBLDP$Bld")."\\"), &main::Path_AbsToWork(&Generic_Definition("EPOCLIB")."\\"))."UREL\\$ExportLibrary.lib\"",
				" && popd\n"
			);
			if ($ExtraExportLibrary) {
				&main::Output(
					"\n",
					"\tcopy \$(EPOCLIB)\\UREL\\$ExportLibrary.lib ",
				       	"\$(EPOCLIB)\\UREL\\$ExtraExportLibrary.lib",
					"\n"
				);
			}
		}
		else {
			&main::Output(
				" && popd\n"
			);
		}				
	}

#	call ld to do base relocations (and dll exports)
	if ($BasicTrgType=~/^(DLL|EXE)/o) {
		&main::Output(
			"\t$Link $PlatOpt{Ld} -s"
			);	
		if ($BasicTrgType=~/^DLL$/o) {
			&main::Output(
				" $PlatOpt{Entry} $EntrySymbol -u $EntrySymbol \"\$(EPOCBLDP$Bld)\\$ExportLibrary.exp\" --dll \\\n"
			);
		}
		elsif ($BasicTrgType=~/^EXE$/o) {
			&main::Output(
				" $PlatOpt{Entry} $EntrySymbol -u $EntrySymbol \\\n"
			);
		}
#		--whole-archive is required here apparently because of a defect  in the gcc toolchain
#		the flag can probably be removed with a later version of gcc
		&main::Output(
			"\t\t--base-file \"\$(EPOCBLDP$Bld)\\$BaseTrg.bas\" -o \"\$(EPOCBLDP$Bld)\\$Trg\" \\\n",
			"\t\t\"\$(EPOCSTATLINK$Bld)\\$FirstLib\" --whole-archive \"\$(EPOCBLDP$Bld)\\$BaseTrg.in\" \\\n",
			"\t\t--no-whole-archive"
		);
		&main::Output(
			" \$(LIBS$Bld) \$(USERLDFLAGS)\n"
		);

#		delete temporary files
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			&main::Output(
				"\t-\$(ERASE) \"\$(EPOCBLDP$Bld)\\$ExportLibrary.exp\"\n"
			);
		}
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCBLDP$Bld)\\$Trg\"\n"
		);

#		call dlltool to do base relocations (and dll exports)
		&main::Output(
			"\tpushd ".&Generic_Quote("\$(EPOCBLDP$Bld)")." && \\\n",
			"\t$Dlltool $PlatOpt{Dlltool} \\\n"
		);
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			&main::Output(
				"\t\t--def \"..\\$ExportLibrary.def\" \\\n",
				"\t\t--dllname \"$LinkAs\" \\\n"
			);
		}
		&main::Output(
			"\t\t--base-file \"$BaseTrg.bas\" \\\n",
			"\t\t--output-exp \"$ExportLibrary.exp\" && popd\n"
		);

#		delete temporary files
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCBLDP$Bld)\\$BaseTrg.bas\"\n"
		);

#		call ld to link the target
		&main::Output(
			"\t$Link $PlatOpt{Ld}"
		);
		if ($Bld=~/^U?REL$/o) {
			&main::Output(
				" -s"
			);
		}
		if ($BasicTrgType=~/^DLL$/o) {
			&main::Output(
				" $PlatOpt{Entry} $EntrySymbol -u $EntrySymbol --dll \\\n"
			);
		}
		elsif ($BasicTrgType=~/^EXE$/o) {
			&main::Output(
				" $PlatOpt{Entry} $EntrySymbol -u $EntrySymbol \\\n"
			);
		}
#		--whole-archive is required here apparently because of a defect in the gcc toolchain
#		the flag can probably be removed with a later version of gcc
		&main::Output(
			"\t\t\"\$(EPOCBLDP$Bld)\\$ExportLibrary.exp\" \\\n",
			"\t\t-Map \"\$(EPOCTRG$Bld)\\$Trg.map\" -o \"\$(EPOCBLDP$Bld)\\$Trg\" \\\n",
			"\t\t\"\$(EPOCSTATLINK$Bld)\\$FirstLib\" --whole-archive \"\$(EPOCBLDP$Bld)\\$BaseTrg.in\" \\\n",
			"\t\t--no-whole-archive"
		);
		&main::Output(
			" \$(LIBS$Bld) \$(USERLDFLAGS)\n"
		);

#		delete temporary files
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCBLDP$Bld)\\$ExportLibrary.exp\"\n"
		);

		if ($Bld=~/DEB$/o) {
			&main::Output(
				"\t$Objcopy -X \"\$(EPOCBLDP$Bld)\\$Trg\" \"\$(EPOCTRG$Bld)\\$BaseTrg.sym\"\n"
			);
		}

		if (&main::CompressTarget) {
			&main::Output(
			"\tpetran $PlatOpt{Petran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " -nocompress " ,  " \"\$(EPOCBLDP$Bld)\\$Trg\" \"\$\@\" \\\n",
			"\t\t"
			);
		}
		else {
			if(&main::CompressTargetMode==NOCOMPRESSIONMETHOD){
				&main::Output(
					"\tpetran $PlatOpt{Petran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " \"\$(EPOCBLDP$Bld)\\$Trg\" \"\$\@\" \\\n",
					"\t\t"
				);
			}
			elsif(&main::CompressTargetMode==INFLATECOMPRESSIONMETHOD){
				&main::Output(
					"\tpetran $PlatOpt{Petran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " ", "  -compressionmethod deflate", " \"\$(EPOCBLDP$Bld)\\$Trg\" \"\$\@\" \\\n",
					"\t\t"
				);
			}
			elsif(&main::CompressTargetMode==BYTEPAIRCOMPRESSIONMETHOD){
				&main::Output(
					"\tpetran $PlatOpt{Petran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " ", "  -compressionmethod bytepair", " \"\$(EPOCBLDP$Bld)\\$Trg\" \"\$\@\" \\\n",
					"\t\t"
				);
			}
		}

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
		&main::Output("\n");
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCBLDP$Bld)\\$Trg\"\n"
		);
	}
	elsif ($BasicTrgType=~/^LIB$/o) {
		&main::Output(
			"\tcopy \"\$(EPOCBLDP$Bld)\\$BaseTrg.in\" \"\$(EPOCSTATLINK$Bld)\\$Trg\"\n"
		);
	}

	&main::Output(
		"\n"
	);


	# TARGET *.IN
	#------------
	if (scalar @SrcList >150) {
		# deal with very long lists by splitting them into 150 file pieces, which allows
		# about 200 bytes per filename if the underlying max size is 32K
		#
		my $counter1=150;	# i.e. trigger new variable immediately
		my $counter2=0;
		my @objvarlist=();
		foreach (@SrcList) {
			if ($counter1==150) {
				$counter1=0;
				$counter2++;
				my $objvar = "OBJECTS$Bld$counter2";
				push @objvarlist, " \$($objvar)";
				&main::Output(
					"\n",
					"$objvar="
				);
			}
			my $BaseSrc = &main::Path_Split('Base', $_);
			my $Ext = &main::Path_Split('Ext', $_);
			$BaseSrc.='_' if (lc($Ext) eq '.cia');
			&main::Output(
				" \\\n\t", &Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc.o")
			);
			$counter1++;
		}
		&main::Output(
			"\n",
			"\n",
			&Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseTrg.in"), " : ", @objvarlist,"\n",
			"\tif exist \"\$\@\" del \"\$\@\"\n"
		);
		foreach (@objvarlist) {
			# Add the files to the list in groups
			&main::Output(
				"\t$Archive cr \$\@$_\n"
			);
		}
		&main::Output(
			"\n\n"
		);
	} else {
		# shorter lists remain unchanged
		#
		&main::Output(
			"OBJECTS$Bld="
		);
		foreach (@SrcList) {
			my $BaseSrc = &main::Path_Split('Base', $_);
			my $Ext = &main::Path_Split('Ext', $_);
			$BaseSrc.='_' if (lc($Ext) eq '.cia');
			&main::Output(
				" \\\n\t", &Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc.o")
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
		&main::Output(
			&Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseTrg.in"), " : \$(OBJECTS$Bld)\n",
			"\tif exist \"\$\@\" del \"\$\@\"\n",
			"\t$Archive cr \$\@ \$^\n",
			"\n\n"
		);
	}
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
	my $BaseSrc=&main::BaseSrc;
	my $ExtSrc=&main::ExtSrc;
	my $cia = (lc($ExtSrc) eq '.cia') ? "_" : "";

	return if (@DepList == 0);

	foreach (@BldList) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLDP$_)\\$BaseSrc$cia.lis"), " ",
			&Generic_Quote("\$(EPOCBLDP$_)\\$BaseSrc$cia.o"), " \\\n",
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
	my $cia = (lc($ExtSrc) eq '.cia') ? "_" : "";

	return if (@DepList == 0);

	&main::Output(
		&Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc$cia.lis"), " ",
		&Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc$cia.o"), " :",
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
	my $ABI=&main::ABI;
	my $BaseSrc=&main::BaseSrc;
	my $Bld=&main::Bld;
	my $Src=ucfirst lc &main::Src;
	my $SrcPath=&main::SrcPath;
	my $Ext = &main::Path_Split('Ext', $Src);
	my $Cia = (lc($Ext) eq '.cia') ? 1 : 0;

	my $RTWSrcPath=&main::Path_Chop(&main::Path_RltToWork($SrcPath));

	# Use GCC trick to get assembler source files preprocessed with CPP
	$Src =~ s/\.s$/.S/i;

	if ($Cia) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc\_.o"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\techo $Src\n",
			"\t\$(GCC$Bld) -x c++ -D__CIA__ -I \"$RTWSrcPath\" \$(INCDIR) -o \$\@ \"$RTWSrcPath\\$Src\"\n",
			"\n",
	#		generate an assembly listing target too
			"LISTING$Bld$BaseSrc\_ : ", &Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc\_.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc\_.$ABI.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc\_.lis"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(GCC$Bld) -x c++ -D__CIA__ -Wa,-adln -I \"$RTWSrcPath\" \$(INCDIR) -o nul: \"$RTWSrcPath\\$Src\" > \$\@\n",
			"\n"
		);
	} else {
		&main::Output(
			&Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc.o"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\techo $Src\n",
			"\t\$(GCC$Bld) -I \"$RTWSrcPath\" \$(INCDIR) -o \$\@ \"$RTWSrcPath\\$Src\"\n",
			"\n",
	#		generate an assembly listing target too
			"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc.$ABI.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLDP$Bld)\\$BaseSrc.lis"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(GCC$Bld) -Wa,-adln -I \"$RTWSrcPath\" \$(INCDIR) -o nul: \"$RTWSrcPath\\$Src\" > \$\@\n",
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
