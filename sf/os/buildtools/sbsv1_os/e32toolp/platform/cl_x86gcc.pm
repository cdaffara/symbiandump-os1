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


package Cl_x86gcc;


my $GccPrefix='';
my $ToolPrefix='';

my $HelperLib='';
my %PlatOpt=(
	'Dlltool'=>'',
	'Entry'=>'--entry',
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
	PMPrefixFile
);

use cl_generic;
use Genutl;

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





sub PMUnderlyingABI($) {
	my ($ABI) = @_;
	if ($ABI eq 'X86gcc') {
			return 'X86gcc';
	}
	return $ABI;
}

my $Makecmd;
my %ABILibPath=();
my $genDefFile;
sub PMStartBldList($) {
	($Makecmd) = @_;
	my $ABI=&main::ABI;
	my $UnderlyingABI=PMUnderlyingABI($ABI);
	my $BaseTrg=&main::BaseTrg;
	my $BasicTrgType=&main::BasicTrgType;
	my @BldList=&main::BldList;
	my @ChopRTWSysIncPaths=&main::Path_Chop(&main::SysIncPaths);
	my @ChopRTWUserIncPaths=&main::Path_Chop(&main::UserIncPaths);
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
		X86gcc=>' -m i386' 
	);

#	work out the flags for various platforms
	if ($ABI eq 'X86gcc') {
		$PlatOpt{Gcc}='-c  -masm=intel -B\\epoc32\\gcc_mingw\\_p';
		$PlatOpt{Dlltool}=$ABIDlltool{X86gcc};
	}


	else {
		&main::FatalError("Platform module - ABI \"$ABI\" unrecognised");
	}
	

#	set up CompatibleABI lib path hash
	my %ABIRTWLibPath=();
	


	$Dlltool=$ToolPrefix.'dlltool';
	$Archive=$ToolPrefix.'ar';
	$Link=$ToolPrefix.'ld';
	$Objcopy=$ToolPrefix.'objcopy';

	&Generic_Header(0,$Makecmd);	# define standard things using absolute paths

	&main::Output(
		"\n",
		"# must set both PATH to make it work correctly\n",
		"Path:=",&main::Path_Drive,$EPOCPath,"gcc_mingw\\bin;\$(Path)\n",
		"PATH:=\$(Path)\n",
		"\n"
	);


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
			" -isystem \"$_\""
		);
	}

	&main::Output(
		"\\\n -include \"", $EPOCPath, "include\\gcc.h\"",
		"\n",
		"\n"
	);

	&main::Output(
		"GCCFLAGS=$PlatOpt{Gcc} \\\n",
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
		" -D PRODUCT_INCLUDE=\"\\\"$VariantFile\\\"\"",
		" \$(USERDEFS)\n",
		"\n"
	);

	foreach (@BldList) {
		&main::Output(
			"GCC$_ = ${GccPrefix}g++"
		);
		if (/REL$/o) {
			&main::Output(
				      ' ',
				      $PlatOpt{Optimize}
			);
		}
		elsif (/DEB$/o) {
			&main::Output(
				' -g'
			);
			#unless (&main::SrcDbg) {
			#   &main::Output(
			#   	' ', $PlatOpt{Optimize}
			#  );
			#}
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
				#" \\\n\t \"",
				" \\\n\t ",
				&Generic_Quote("\$(EPOCTRG$_)\\$Trg")
				#"\""
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

	if (!-e $DefFile && scalar(&main::Exports)!=0)
	{
		# There are mandatory exports - generate a def file using the mandatory exports.
		$genDefFile = "\$(EPOCBLD)\\$ExportLibrary.gen.def";
	}
	
	if (-e $DefFile || defined($genDefFile)) { # effectively "if project frozen ..."
		
		#	Establish the entry point symbol
		my $EntrySymbol;
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEDLL$/o ) {
			$EntrySymbol = '_E32Dll';
		}
		elsif ($BasicTrgType=~/^EXE$/o || $TrgType=~/^EXEXP$/o ) {
			$EntrySymbol = '_E32Startup';
		}
				
		&main::Output("\n", &Generic_Quote("\$(EPOCBLD)\\$ExportLibrary.prep.def"), " : ");
		my $defFileToUse;
		if (defined($genDefFile))
		{
			# The generated def file is not a dependency of the prep.def - don't add anyting after the ':' char
			&main::Output("\n\tperl -S gendef.pl $genDefFile ", join(' ', &main::Exports), "\n");
			$defFileToUse = $genDefFile;
		}
		else
		{
			&main::Output(&Generic_Quote($DefFile), "\n");
			$defFileToUse = "\$<";
		}
		&main::Output("\tperl -S prepdef.pl $defFileToUse \$@ nodatasizes $EntrySymbol\n\n");
		
		&main::Output(
			&Generic_Quote("\$(EPOCBLD)\\$ExportLibrary.lib.exp"), " : ", &Generic_Quote("\$(EPOCBLD)\\$ExportLibrary.prep.def"), "\n",
			"\t$Dlltool $PlatOpt{Dlltool} --input-def \"\$<\"  --dllname \"$LinkAs\"  -e \"\$@\"  \n",			
			"\t-\$(ERASE) \"\$(EPOCBLD)\\tmp.txt \"\n\n"
		);
	}
	
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
					" ", &Generic_Quote("\$(EPOCLIB)\\LIB\\$PrimaryExportLibrary.lib")
				);
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
				"\t\@echo Not attempting to create \"\$(EPOCLIB)\\LIB\\$PrimaryExportLibrary.lib\"\n",
				"\t\@echo from frozen .DEF file, since EXPORTUNFROZEN specified.\n"
			);
		}



		&main::Output(
			"\n",
			"\n",
			"# REAL TARGET - LIBRARY\n",
		);
		
		&main::Output(
			"\n",
			&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.lib"), " : ",
			&Generic_Quote("\$(EPOCBLD)\\$ExportLibrary.prep.def"), "\n",
			"\t$Dlltool $PlatOpt{Dlltool} --input-def \"\$<\"  --dllname \"$LinkAs\"  --output-lib \"\$@\" \n",
			"\t-\$(ERASE) \"\$(EPOCBLD)\\tmp.txt \"\n"
		);
		if ($ExtraExportLibrary) {
			&main::Output(
				"\n",
				&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.lib"), " : ",
				&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.lib"), "\n",
				"\tcopy \$< \$@\n"
			);
		}
	}


	&main::Output(
		"\n",
		"CLEANLIBRARY :\n"
	);
	if ($DefFile and !$NoExportLibrary) {
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCLIB)\\LIB\\$ExportLibrary.lib \"\n"
		);
		if ($ExtraExportLibrary) {
			&main::Output(
				"\t-\$(ERASE) \"\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.lib \"\n"
			);
		}

	}

	&main::Output(
		"\n",
		"\n"
	);
	&Generic_MakeWorkDir('MAKEWORKLIBRARY',"${LibPath}LIB");

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
	}
	
	&main::Output(
		"WHAT$Bld : WHATGENERIC\n",
		"\n",
		"CLEAN$Bld : CLEANBUILD$Bld CLEANRELEASE$Bld CLEANLIBRARY\n ",
		"\n",
		"CLEANBUILD$Bld : \n",
		"\t\@perl -S ermdir.pl \"\$(EPOCBLD$Bld)\"\n",
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


	foreach (@ASSPLibList) {
		&main::Output(
			" \\\n\t",
			&Generic_Quote("\$(EPOCASSPLINK$Bld)\\$_")
		);
	}
	foreach (@LibList) {
		&main::Output(
			" \\\n\t",
			&Generic_Quote("\$(EPOCLINK)\\LIB\\$_")
		);
	}
	&main::Output(
		"\n",
		"\n"
	);


	&main::Output(
		"LINKLIBS$Bld="
	);
	
	
	if ($BasicTrgType=~/^DLL$/o) { # Add the DLL stub library
		&main::Output(
			" \\\n\t \"",
			&Generic_Quote("\$(EPOCSTATLINK$Bld)\\EDLLSTUB.LIB"),
			" \""
		);
	}

	
	if ($HelperLib) {
		&main::Output(
			" \\\n\t \"",
			&Generic_Quote("\$(EPOCSTATLINK$Bld)\\$HelperLib"),
			" \""
		);
	}
	foreach (@StatLibList) {
		&main::Output(
			" \\\n\t \"",
			&Generic_Quote("\$(EPOCSTATLINK$Bld)\\$_"),
			" \""
		);
	}


	foreach (@ASSPLibList) {
		&main::Output(
			" \\\n\t \"",
			&Generic_Quote("\$(EPOCASSPLINK$Bld)\\$_"),
			" \""
		);
	}
	foreach (@LibList) {
		&main::Output(
			" \\\n\t \"",
			&Generic_Quote("\$(EPOCLINK)\\LIB\\$_"),
			" \""
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

#	Establish the entry point symbol
	my $EntrySymbol;
	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEDLL$/o ) {
		$EntrySymbol = '_E32Dll';
	}
	elsif ($BasicTrgType=~/^EXE$/o || $TrgType=~/^EXEXP$/o ) {
		$EntrySymbol = '_E32Startup';
	}
	
	&main::Output(
		&Generic_Quote("\$(EPOCTRG$Bld)\\$Trg"), " : ",
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg.in")
	);
	
	if (-e $DefFile || defined($genDefFile)) { # effectively "if project frozen ..."
		&main::Output(
			" ", &Generic_Quote("\$(EPOCBLD)\\$ExportLibrary.lib.exp")
		);
	}
	if ($BasicTrgType=~/^(EXE|DLL)$/o) {
		&main::Output(
			" ", &Generic_Quote("\$(EPOCSTATLINK$Bld)\\$FirstLib")
		);
	}
	
	if ($TrgType=~/^EXEXP$/o) {
		&main::Output(
			" ", &Generic_Quote("\$(EPOCSTATLINK$Bld)\\$FirstLib")
		);
	}
	&main::Output(
		" \$(LIBS$Bld)\n"
	);




	if ($BasicTrgType=~/^(DLL|EXE)/o) {


#		call ld to link the target
		&main::Output(
			"\t$Link $PlatOpt{Ld}  -nostdlib   "
		);



	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			if (-e $DefFile || defined($genDefFile)) {
				&main::Output(
				"\"\$(EPOCBLD)\\$ExportLibrary.lib.exp \" \\\n"
				
				);		
			}		
			if ($BasicTrgType=~/^DLL$/o) {
				&main::Output(
					"--dll $PlatOpt{Entry} _$EntrySymbol  \\\n"
				);
			}
			else{
				&main::Output(
					" $PlatOpt{Entry} _$EntrySymbol  \\\n"
				);
						
			}
		}
		elsif ($BasicTrgType=~/^EXE$/o) {
			&main::Output(
				" $PlatOpt{Entry} _$EntrySymbol  \\\n"
			);
		}
		
#		--whole-archive is required here apparently because of a defect in the gcc toolchain
#		the flag can probably be removed with a later version of gcc
		&main::Output(
			"\t\t-Map \"\$(EPOCTRG$Bld)\\$Trg.map \" -o \"\$(EPOCBLD$Bld)\\$Trg \" \\\n",
			"\t\t\"\$(EPOCSTATLINK$Bld)\\$FirstLib \" --whole-archive \"\$(EPOCBLD$Bld)\\$BaseTrg.in \" \\\n",
			"\t\t--no-whole-archive"
		);
		

		&main::Output(
			" \$(LINKLIBS$Bld) \$(USERLDFLAGS)\n"
		);
		

		if (&main::CompressTarget) {
			&main::Output(
			"\tpetran $PlatOpt{Petran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " -nocompress " ,  " \"\$(EPOCBLD$Bld)\\$Trg\" \"\$\@\" \\\n",
			"\t\t"
			);
		}
		else {
			if(&main::CompressTargetMode==NOCOMPRESSIONMETHOD){
				&main::Output(
					"\tpetran $PlatOpt{Petran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " \"\$(EPOCBLD$Bld)\\$Trg\" \"\$\@\" \\\n",
					"\t\t"
				);
			}
			elsif(&main::CompressTargetMode==INFLATECOMPRESSIONMETHOD){
				&main::Output(
					"\tpetran $PlatOpt{Petran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " ", "  -compressionmethod deflate", " \"\$(EPOCBLD$Bld)\\$Trg\" \"\$\@\" \\\n",
					"\t\t"
				);
			}
			elsif(&main::CompressTargetMode==BYTEPAIRCOMPRESSIONMETHOD){
				&main::Output(
					"\tpetran $PlatOpt{Petran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " ", "  -compressionmethod bytepair", " \"\$(EPOCBLD$Bld)\\$Trg\" \"\$\@\" \\\n",
					"\t\t"
				);
			}
		}

# ALWAYS ALLOW WRITEABLE DATA... PART OF THE VTABLE IMPORT WORKAROUND
#		if (&main::AllowDllData) {
			&main::Output(
				' -allow'
			);
#		}
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
	}
	elsif ($BasicTrgType=~/^LIB$/o) {
		&main::Output(
			"\tcopy \"\$(EPOCBLD$Bld)\\$BaseTrg.in\" \"\$(EPOCSTATLINK$Bld)\\$Trg\"\n"
		);
	}

	&main::Output(
		"\n"
	);


	# TARGET *.IN
	#------------
	if (scalar @SrcList >100) {
		# deal with very long lists by splitting them into 150 file pieces, which allows
		# about 200 bytes per filename if the underlying max size is 32K
		#
		my $counter1=100;	# i.e. trigger new variable immediately
		my $counter2=0;
		my @objvarlist=();
		foreach (@SrcList) {
			if ($counter1==100) {
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
				" \\\n\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o")
			);
			$counter1++;
		}
		&main::Output(
			"\n",
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg.in"), " : ", @objvarlist,"\n",
			"\tif exist \"\$\@\" del \"\$\@\"\n"
		);
		foreach (@objvarlist) {
			# Add the files to the list in groups
			&main::Output(
				"\t$Archive cr \"\$\@ \" $_  \n"
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
				" \\\n\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o")
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
		
		##Version of object list with quotes
		&main::Output(
			"ARCHIVEOBJECTS$Bld="
		);
		foreach (@SrcList) {
			my $BaseSrc = &main::Path_Split('Base', $_);
			my $Ext = &main::Path_Split('Ext', $_);
			$BaseSrc.='_' if (lc($Ext) eq '.cia');
			&main::Output(
				" \\\n\t \"", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"),
				" \""
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
						
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg.in"), " : \$(OBJECTS$Bld)\n",
			"\tif exist \"\$\@\" del \"\$\@\"\n",
			"\t$Archive cr \"\$\@ \" \$(ARCHIVEOBJECTS$Bld)  \n",
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
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseSrc$cia.lis"), " ",
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseSrc$cia.o"), " \\\n",
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
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc$cia.lis"), " ",
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc$cia.o"), " :",
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

sub SelectLangOptions {
	my ($Ext) = @_;
	if ($Ext=~/^.c$/) {
		return '-x c';
	}
	return '-fno-rtti -fno-exceptions -fcheck-new';
}

sub PMEndSrcBld {
	my $ABI=&main::ABI;
	my $BaseSrc=&main::BaseSrc;
	my $Bld=&main::Bld;
	my $Src=ucfirst lc &main::Src;
	my $SrcPath=&main::SrcPath;
	my $Ext = &main::Path_Split('Ext', $Src);
	my $Cia = (lc($Ext) eq '.cia') ? 1 : 0;
	my $LangOptions = &SelectLangOptions($Ext);

	my $RTWSrcPath=&main::Path_Chop(&main::Path_RltToWork($SrcPath));

	# Use GCC trick to get assembler source files preprocessed with CPP
	$Src =~ s/\.s$/.S/i;

	if ($Cia) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.o"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\techo $Src\n",
			"\t\$(GCC$Bld) -fomit-frame-pointer -O1 -x c++  -D__CIA__ -I \"$RTWSrcPath\" \$(INCDIR)  -o \$\@ \"$RTWSrcPath\\$Src\"\n",
			"\n",
	#		generate an assembly listing target too
			"LISTING$Bld$BaseSrc\_ : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc\_.$ABI.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(GCC$Bld) -fomit-frame-pointer -O1 -x c++  -D__CIA__ -Wa,-adln -I \"$RTWSrcPath\" \$(INCDIR)  -o nul: \"$RTWSrcPath\\$Src\" > \$\@\n",
			"\n"
		);
	} else {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\techo $Src\n",
			"\t\$(GCC$Bld) $LangOptions -I \"$RTWSrcPath\" \$(INCDIR) -o \$\@ \"$RTWSrcPath\\$Src\"\n",
			"\n",
	#		generate an assembly listing target too
			"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc.$ABI.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(GCC$Bld) $LangOptions -Wa,-adln -I \"$RTWSrcPath\" \$(INCDIR)  -o nul: \"$RTWSrcPath\\$Src\" > \$\@\n",
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

sub PMPrefixFile 
{ 
    return &Generic_Quote(&main::Path_Drive.&main::EPOCIncPath."gcc.h");
}

1;

