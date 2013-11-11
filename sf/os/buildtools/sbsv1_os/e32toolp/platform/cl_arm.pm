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


package Cl_arm;

my $ToolPrefix='';
my %PlatOpt=(
	'Dlltool'=>'',
	'Entry'=>'-e',
	'Ld'=>'',
	'Elftran'=>'',
);

# takes an 'expression'  to evaluate with $_ bound to each of the 
# remaining args
sub PrintList
{
    my $expr = shift @_;
    foreach (@_) {
	my $str = eval($expr);
	&main::Output($str);
    }
}

# specify floating point model here
my $floatingpointmodel = "softvfp";

if (&main::ARMFPU && (&main::ARMFPU =~ /^VFPV2$/i)) {
	$floatingpointmodel = "vfpv2";
}

my $Archive;
my $Link;
my $Objcopy;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	PMHelp_Mmp

	PMPlatProcessMmp
	
	PMStartBldList
		PMBld
	PMStartSrcList
		PMBitMapBld
		PMResrcBld
		PMStartSrc
		PMAifBld
		PMSrcDepend
			PMSrcBldDepend
			PMEndSrcBld
		PMEndSrc
	PMEndSrcList
	PMPrefixFile
	PMSupportsFeatureVariants
);

use Cwd;
use Armutl;
use RVCT_plat2set;
use cl_generic;
use E32env;
use Genutl;
use strict;

use constant NOCOMPRESSIONMETHOD => 0;
use constant INFLATECOMPRESSIONMETHOD => 1;
use constant BYTEPAIRCOMPRESSIONMETHOD => 2;

use constant NON_DEBUGGABLE => 0;
use constant DEBUGGABLE => 1;
use constant DEBUGGABLE_UDEBONLY => 2;

use constant NOTPAGED => 0;
use constant UNPAGED => 1;
use constant PAGED => 2;

sub PMHelp_Mmp {
    &Armutl_Help_Mmp;
}

my $Plat=main::Plat();

my ($RVCTMajorVersion, $RVCTMinorVersion, $RVCTBuildNumber) = RVCT_plat2set::get_version_list($Plat); 

my $RVCTVersion = "${RVCTMajorVersion}_${RVCTMinorVersion}";

my $ARMCCVersion = "${RVCTMajorVersion}${RVCTMinorVersion}0${RVCTBuildNumber}";

my $oP = '--';
$oP = '-' if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2);

#Check if Function call Logger is enabled
my $Function_Call_Logger=&main::IsFunctionCallLogging();

# Get the information regarding supporting Compiler Wrapper Option
my $IsCompilerWrapperOption=&main::CompilerWrapperOption();

#Check the existence of elf2e32.exe in the system path
my $IsExistELF2E32EXE = 0;
    open ELF2E32PIPE, "elf2e32 2>&1 |";
    while (<ELF2E32PIPE>) {
	if($_=~/^Symbian Post Linker\, Elf2E32/) {
	    $IsExistELF2E32EXE = 1;
	    last;
	}
	next;
    }
    close ELF2E32PIPE;

my $ArmIncDir;
my @ArmLibList;
my $ArmRT = 0;
my %AsmFiles = ();
my %AsmDirs = ();
my $oe_options = '';
my $NamedSymLkup = 0;
my $symNameLkupOpt = '';


sub PMPlatProcessMmp (@) {
        &Armutl_DoMmp(@_);
	$ArmIncDir = RVCT_plat2set::get_inc_path($Plat);
	&main::SetStdIncPaths($ArmIncDir);
	@ArmLibList = &Armutl_ArmLibList;
	$ArmRT = &Armutl_ArmRT;
	my @AsmFileList = &Armutl_AsmFileList;
	foreach (@AsmFileList) { $AsmFiles{ucfirst lc $_} = 1; }
}

sub SysTrg () {
	return 1 if &main::SystemTrg;
	my $ExportLibrary=&main::ExportLibrary;
	return 1 if ($ExportLibrary =~ /EKERN/i);
	my $Trg=&main::Trg;
	return 1 if ($Trg =~ /KSRT/i);
	return 0;
}

my $RVCT20 = 0; 

# suppress these warnings & errors
#Warning: #66-D: enumeration value is out of "int" range 
# "EUSER\CBASE\Ub_act.cpp", line 20: Warning:  #161-D: unrecognized #pragma
# 611: overloaded virtual function "entity" is only partially overridden in <entitykind> "entity"
# "EUSER\CBASE\Ub_act.cpp", line 256: Warning:  #654-D: declaration modifiers are incompatible with previous declaration
# 997: <entity-kind> "entity" is hidden by "entity" -- virtual function override intended?
# "EPOC32\include\s32stor.h", line 354: Error:  #1152-D: polymorphic base classes need to be exported as well
# "INCLUDE\e32base.h", line 31: Warning:  #1300-D: ~CBufBase inherits implicit virtual
#"COMPSUPP\\RVCT2_1\\Dfpaeabi.cpp", line 87: Warning: A1488W: PROC/FUNC at line 9 without matching ENDP/ENDFUNC
#"COMPSUPP\\RVCT2_1\\Dfpaeabi.cpp", line 85: Warning: A1464W: ENDP/ENDFUNC without corresponding PROC/FUNC
#Warning: L6318W: _integrator_cm1136_ekern.in(.text) contains branch to a non-code symbol FindMostSignificantOne(unsigned long).

my $diag_suppressions = '--diag_suppress 66,161,611,654,997,1152,1300,1464,1488,6318,6331';

# downgrade from errors to warnings
my $diag_warnings = '';

# upgrade from warnings to errors
# Warning:  #1267-D: Implicit physical register R0 should be defined as a variable
my $diag_errors = '--diag_error 1267';
  
my $commonOptions = $RVCT20 ?
  "$diag_suppressions $diag_warnings $diag_errors" :
  "$diag_suppressions $diag_warnings $diag_errors";  

my @mmpOption = &main::ReplaceOptions("ARMCC");
my $CompilerOption = &main::CompilerOption("ARMCC");
my $contingentOptions;
my $thumbOptions = $RVCT20 ? '-thumb' : '--thumb ';
my $armOptions = $RVCT20 ? '-arm' : '--arm ';
my $kernelOptions = $RVCT20 ? '-arm' : '--arm --no_exceptions --no_exceptions_unwind';
my $invariantOptions = $RVCT20 ? 
  '-cpu 5T --enum_is_int -Ono_known_library --export_vtbl -apcs /inter' :
  '--cpu 5T --enum_is_int -Ono_known_library --fpmode ieee_no_fenv --export_all_vtbl --no_vfe --apcs /inter';
$invariantOptions .= ' --dllimport_runtime' unless ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2);  
my $exceptions = $RVCT20 ? '' : ' --exceptions --exceptions_unwind';
my $floatingpoint = $RVCT20 ? '' : ' --fpu '.$floatingpointmodel.'';
my @FCLogger_Macros; # Macros required to be passed to the FCLogger

my $useLinkerFeedBack = 0;

sub LinkerFeedBackFile() {
  return unless $useLinkerFeedBack;
  my $Trg = &main::Trg;
  return "$Trg.lfb";
}

sub LinkerFeedBackOption() {
  return "" unless $useLinkerFeedBack;
  my $BasicTrgType=&main::BasicTrgType;
  return "" unless ($BasicTrgType=~/^(DLL|EXE)/o);
  my $file = LinkerFeedBackFile();
  return "--feedback $file ";
}

my %BSF_keywords = (
		    COMMON_OPTIONS => 1,
		    THUMB_OPTIONS => 1,
		    ARM_OPTIONS => 1,
		    KERNEL_OPTIONS => 1,
		    INVARIANT_OPTIONS => 1
		   );

sub Read_BSF_Options() {
        my %plat = (main::PlatRec());
	my @Customization_Data = split(/\n/,$plat{'CUSTOMIZATION_DATA'});
	foreach my $option (@Customization_Data) {
			next if ($option =~ /^$/);
	        warn "Unrecognized BSF syntax: $option.\n"
		        unless ($option =~ /\s*(\S+)\s+(.+)$/);
		my $key = uc $1;
		my $val = $2;
	        warn "Unrecognized BSF keyword: $key.\n"
		        unless ($BSF_keywords{$key});
		if ($key =~ /COMMON_OPTIONS/) {
		        Set_BSF_Options(\$commonOptions,$val);
			next;
		}
		if ($key =~ /THUMB_OPTIONS/) {
		        Set_BSF_Options(\$thumbOptions,$val);
			next;
		}
		if ($key =~ /ARM_OPTIONS/) {
		        Set_BSF_Options(\$armOptions,$val);
			next;
		}
		if ($key =~ /KERNEL_OPTIONS/) {
		        Set_BSF_Options(\$kernelOptions,$val);
			next;
		}
		if ($key =~ /INVARIANT_OPTIONS/) {
		        Set_BSF_Options(\$invariantOptions,$val);
			next;
		}

	}		
}

# Set the options passed from BSF file 
# @param OptionName    - BSF Keyword using which the options would be overridden in the BSF file
# @param Options       - List of options read from the BSF keyword
sub Set_BSF_Options($$)
{
	my ($OptionName,$Options) = @_;
	my @Fragments=();
	
	# Check if the value of BSF option is to be set or added/removed.
	if($Options =~ /\+\[.*\]\+|\-\[.*\]\-/)
	{
		if (@Fragments = Split_BSF_Options($Options,'RemoveOptions'))
		{
			foreach my $Opt (@Fragments) 
			{
				# Remove trailing white spaces
				$Opt =~ s/\s+$//;				
				$$OptionName =~ s/$Opt//g;
			}					
			@Fragments=();
		}
		if (@Fragments = Split_BSF_Options($Options,'AddOptions')) 
		{
			$$OptionName .= " @Fragments";
			@Fragments=();
		}		
		
		# Warn if options are not present in the form '+[...]+' or '-[...]-'
		$Options =~ s/\+\[.*?\]\+|\-\[.*?\]\-//g;
		if($Options !~ /^\s*$/)
		{
			print "Warning: Ignoring option(s) \"$Options\" specified in BSF as option(s) should be in the form '+[...]+' or '-[...]-.\n";
		}
	}
	else
	{
		$$OptionName = $Options;
	}		
	
	&main::Output(					
		"\n"
	);
}

# Split BSF options to find options which are to be added/removed
# @param String      - List of options present in form '+[...]+' or '-[....]-'
# @param $Task       - Variable to decide whether to return options to be addded or options to be removed
sub Split_BSF_Options($$)
{
	my ($String,$Task) = @_;
    my @Result = ();
	my @Fragments = ();
	my $Pattern = '';
		
	if ($Task eq 'AddOptions')
	{
		# Get the options which are to be added (present in the form '+[...]+')
		@Fragments = $String =~ /\+\[(.*?)\]\+/g;	
	}
	elsif ($Task eq 'RemoveOptions') 
	{
		# Get the options which are to be removed (present in the form '-[...]-')
		@Fragments = $String =~ /\-\[(.*?)\]\-/g;
	}	

	# Set the value of '$Pattern' which is required to segregate one option from another based on the option prefix.
	# Option prefix for RVCT can be '-' or '--'.
	$Pattern = '-{1,2}\S+\s*(?!-)\S*';	

	foreach my $Val (@Fragments) 
	{
		my @Opt = $Val =~ /$Pattern/g;
		push @Result,@Opt;		 		
	}
	return @Result;	
}

sub ComputeCompilerOpts() {
	my %plat = &main::PlatRec();
	Read_BSF_Options() if ($plat{'CUSTOMIZES'});
	my $ABI=&main::ABI;
	my $TrgType=&main::TrgType;
	if (SysTrg()) {
	        $contingentOptions = $kernelOptions.$floatingpoint;
        } elsif (main::BuildAsARM() or ($ABI eq 'ARMV4')) {
	        $contingentOptions = $armOptions.$floatingpoint.$exceptions;
	    } else {
			$contingentOptions = $thumbOptions.$floatingpoint.$exceptions.' -D__MARM_THUMB__';	
			push @FCLogger_Macros, '__MARM_THUMB__';
	}
	# support for ARMV4
	my $invopts = "$invariantOptions";
	if ($ABI eq 'ARMV4') {
		$invopts =~ s/5T/4/;
		$invopts =~ s/inter/nointer/;
	} else {
		$contingentOptions .= ' -D__MARM_INTERWORK__';
		push @FCLogger_Macros, '__MARM_INTERWORK__';
	}

	#Options to export all the external symbols for OE DLL , OE Exe and OE Static Lib
	if ($TrgType=~/^STDDLL$/o || $TrgType=~/^STDEXE$/o || $TrgType=~/^STDLIB$/o) {
		$oe_options=' --no_hide_all';
	}

	return $commonOptions.' '.$contingentOptions.' '.$invopts.' '.$oe_options.' '.&main::CompilerOption("ARMCC");
}

my $Makecmd;

sub PMStartBldList($) {
	($Makecmd) = @_;
	my $ABI=&main::ABI;
	my $BaseTrg=&main::BaseTrg;
	my $BasicTrgType=&main::BasicTrgType;
	my @BldList=&main::BldList;
	my @ChopSysIncPaths=&main::Path_Chop(&main::SysIncPaths);
	my @ChopUserIncPaths=&main::Path_Chop(&main::UserIncPaths);
	my $DefFile=&main::DefFile;
	my $EPOCPath=&main::EPOCPath;
	my $LinkAs=&main::LinkAs;
	my $LibPath=&main::LibPath.'LIB\\';
	my @MacroList=&main::MacroList();
	push @MacroList, "__SUPPORT_CPP_EXCEPTIONS__";

	my $VariantFile=&main::VariantFile();

	my $Plat=&main::Plat;
	my $Trg=&main::Trg;
	if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
		if ($BasicTrgType=~/^LIB$/o) {
			# Temporary Workaround for RVCT2.1 static libs problem with RVCT2.2 builds
			# Rename all the static libs produced with RVCT2.1 as {libname}2_1.lib
		        if ($Trg=~/^\s*(\S+)(\.lib)$/io) {
			        if ($1!~/$RVCTVersion/i) {
				        $Trg=$1.$RVCTVersion.".lib";
				}
			}
			if ($BaseTrg!~/$RVCTVersion/i) {
			        $BaseTrg .= $RVCTVersion;
			}
		}
	}

	my $TrgType=&main::TrgType;
	my @UidList=&main::UidList;	
    my $SystemTrg = &main::SystemTrg;
    my $ExportLibrary=&main::ExportLibrary;
    my $NoExportLibrary=&main::NoExportLibrary;
    # N.B. should get better way to detect kernel probably!!
    $SystemTrg = 1 if ($ExportLibrary =~ /EKERN/i);
	
    # N.B. should get better way to detect this
    $SystemTrg = 1 if ($Trg =~ /KSRT/i);

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

#	work out the flags for various platforms
	unless ($ABI eq 'ARMV5' or $ABI eq 'ARMV4') {
		&main::FatalError("Platform module - ABI \"$ABI\" unrecognised");
	}
	my $ComputeCompilerOpts=ComputeCompilerOpts();
	if (@mmpOption) 
	{
		my $pattern = '-{1,2}\S+\s*(?!-)\S*';	
		foreach my $options (@mmpOption) 
		{
			my @opts = $options =~ /$pattern/g;
			my $count = 0;
			while ($count <= $#opts) 
			{
				my $opt;
				my $rep;
				if ($opts[$count] =~ /^(\S+)\s+(\S+)$/) 
				{
					$opt = $1;
					$rep = $2;
					$ComputeCompilerOpts =~ s/$opt\s+\S+\s+/ $opt $rep /g;
					$count++;
				}
				else
				{
					$opt = $opts[$count];
					$rep = $opts[$count+1];
					$ComputeCompilerOpts =~ s/$opt/$rep/g;					
					$count+=2;
				}
			}		
		}
	}

	$PlatOpt{Arm} = $ComputeCompilerOpts;

	my $InterWorking = ($ABI eq 'ARMV4') ? "" : "--inter";

	$Archive=$ToolPrefix.'armar';
	$Link=$ToolPrefix."armlink ${oP}diag_suppress 6331,6780 ";
	$Objcopy=$ToolPrefix.'objcopy';

	&Generic_Header(0,$Makecmd);	# define standard things using absolute paths
		 
	if ($Makecmd eq "nmake") {
		&main::Output(
			"\n",
  			"PATH=",&main::Path_Drive,$EPOCPath,"gcc\$(PBUILDPID)\\bin;\$(PATH)\n",
			"\n"
		);
	}
	else {
		&main::Output(
			"\n",
			"# must set both PATH and Path to make it work correctly\n",
  			"Path:=",&main::Path_Drive,$EPOCPath,"gcc\$(PBUILDPID)\\bin;\$(Path)\n",
			"PATH:=\$(Path)\n",
			"\n"
		);
	}

	&main::Output(
		"INCDIR  ="
	);
	PrintList("\" -J \$_\"", @ChopUserIncPaths);
	PrintList("\" -J \$_\"", @ChopSysIncPaths);
	if ($ArmIncDir) {
		&main::Output(
		    " -J \"$ArmIncDir\" ",
		);	
	}
	&main::Output(
		"\n",
		"\n"
	);

	#Function Call Logger
	if ($Function_Call_Logger)	{
		&main::Output(
			"INCDIR_FCLOGGER  ="
		);
		PrintList("\" -I \$_\"", @ChopUserIncPaths);
		PrintList("\" -I \$_\"", @ChopSysIncPaths);
		if ($ArmIncDir) {
			&main::Output(
				" -I \"$ArmIncDir\" "
			);	
		}
		&main::Output(
			"\n",
			"\n"
		);
	}
	
	&main::Output(
		"ARMCCFLAGS=$PlatOpt{Arm} -c\\\n",
		"\n"
	);

	&main::Output(
		"ARMCCDEFS = "
	);
	PrintList("\" -D\$_\"", @MacroList);
	
	if($VariantFile) {
		if ($Function_Call_Logger) {
			#FC Logger accepts product include file without path
			my $file=&main::Path_Split('File', ${VariantFile});
			
			&main::Output(
				" -D\"__PRODUCT_INCLUDE__=\\\"${file}\\\"\""
			);
		}
		else {
			&main::Output(
				" -D__PRODUCT_INCLUDE__=\\\"${VariantFile}\\\""
			);
		}
	}

	&main::Output(
		" \$(USERDEFS)\n",
		"\n"
	);


	if ($TrgType=~/^STDDLL$/o || $TrgType=~/^STDEXE$/o){
#	For now, named symbol lookup is enabled only for the STD binaries. Later, it may be enabled based on an mmp 
#	keyword.

		$NamedSymLkup = 1;
		$symNameLkupOpt = '--sym_name_lkup';
	}



	foreach (@BldList) {
		&main::Output(
			"ARMCC$_ = armcc"
		);
		if(&main::DebugSwitchUsed() ) {
#			when the debug switch is specified and enabled, set the compiler's debug flag for both udeb or urel.
#			when the debug switch is disabled, don't set the compiler's debug flag for either udeb or urel.
#			The optimization option is set only based on the build i.e., Udeb or Urel and is independent of
#			whether debug is enabled or not. This might give a poorer debug view for debug-enabled Urel build.
			if(&main::SymbolicDebugEnabled() ) {
				&main::Output(
						  ' -g'
				);
			}
		}
		elsif (/DEB$/o) {
			&main::Output(
				      ' -g'
			);
		}

		if (/DEB$/o) {
			&main::Output(
				      ' -O0'
			);
		}
		else {
			&main::Output(
				      ' -O2'
			);
		}		
		&main::Output(
			' $(ARMCCFLAGS)'
		);
		my @ml = &main::MacroList($_);
		PrintList("\" -D\$_\"", @ml);
		&main::Output(
			" \$(ARMCCDEFS)\n"
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	#Function call logger
	if ($Function_Call_Logger)	{
		#Send all the debug macros to logger
		foreach (@BldList) {
			&main::Output (
				"FCLOGGER$_ = ",
				$EPOCPath, 
				"tools\\fc_logger\\edgcpfe"
			);

			my @ml = &main::MacroList($_);
			push @ml, "__ARMCC_VERSION=$ARMCCVersion";
			PrintList("\" -D\$_\"", @ml);
			&main::Output(
				" \$(ARMCCDEFS)"
			);
			PrintList("\" -D\$_\"", @FCLogger_Macros);
			&main::Output(
				"\n",
				"\n"
			);
		}
	}
	
	foreach (@BldList) {
		&main::Output(
			"$_ :"
		);

		if ($BasicTrgType !~ /IMPLIB/io) {
			&main::Output (
				" \\\n\t",
				&Generic_Quote("\$(EPOCTRG$_)\\".&main::Trg($_))
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
	PrintList("\"\nRESOURCE\$_ : MAKEWORK\$_\"", @BldList);
	&main::Output(
		"\n",
		"\n",
	);

	&main::Output(
		"LIBRARY : MAKEWORKLIBRARY"
	);
	if ($BasicTrgType=~/^LIB$/o) {
#		code to ensure that the static libraries for all builds are built at the library stage
	        PrintList("\" \$_\"", @BldList);
	}
	elsif ($DefFile and !$NoExportLibrary) {
		unless (&main::ExportUnfrozen) {
			if (-e $DefFile) { # effectively "if project frozen ..."
				&main::Output(
					" ", &Generic_Quote("\$(EPOCLIB)\\LIB\\$PrimaryExportLibrary.lib"),
				);
				# if elf2e32.exe(postlinker) exists, then generate .dso along with .lib
				if ($IsExistELF2E32EXE) {
					&main::Output(
						" ", &Generic_Quote("\$(EPOCLIB)\\LIB\\$PrimaryExportLibrary.dso"), "\n"
					);
				}
				else {
					&main::Output("\n");
				}
			}
			else {
				&main::Output(
					"\n",
					"\t\@echo WARNING: Not attempting to create any import libraries.\n",
					"\t\@echo When exports are frozen in \"$DefFile\", regenerate Makefile.\n"
				);
			}
		} else {
			&main::Output(
				"\n",
				"\t\@echo Not attempting to create \"\$(EPOCLIB)\\LIB\\$PrimaryExportLibrary.lib\"\n",
				"\t\@echo from frozen .DEF file, since EXPORTUNFROZEN specified.\n"
			);
		}

		my $theDefFile = $DefFile;
		$theDefFile = "\$(EPOCBLD)\\$BaseTrg.def" unless (-e $DefFile);
		&main::Output(
			"\n",
			"\n",
			"# REAL TARGET - LIBRARY\n",
			"\n",
			&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.lib"), " : ",
			&Generic_Quote($DefFile), "\n",
				"\tperl -S prepdef.pl ", &Generic_Quote($DefFile), " \"\$(EPOCBLD)\\$ExportLibrary.prep.def\"\n",
				"\tdef2dll.bat --path=\$(EPOCLIB)\\LIB \\\n\t\t--bldpath=\$(EPOCBLD) \\\n\t\t--import=$ExportLibrary \\\n",
			"\t\t--deffile=\"\$(EPOCBLD)\\$ExportLibrary.prep.def\" \\\n\t\t--linkAs=$LinkAs \\\n\t\t$InterWorking\n",
			"\n",
		 );
		if ($ExtraExportLibrary) {
			&main::Output(
				"\n",
				&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.lib"), " : ",
				&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.lib"), "\n",
				"\tcopy \"\$<\" \"\$@\"\n"
			);
		}
		#if elf2e32.exe(postlinker) exists, then generate .dso(which will be used by ABIV2 platforms)
		if ($IsExistELF2E32EXE) {
			&main::Output(
				"\n",
				&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.dso"), " : ",
				&Generic_Quote($DefFile), "\n",
					"\telf2e32 --definput=\"\$(EPOCBLD)\\$ExportLibrary.prep.def\" --dso=",
					&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.dso"),
					" --linkas=$LinkAs\n"
			);
			if ($ExtraExportLibrary) {
				&main::Output(
					"\n",
					&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.dso"), " : ",
					&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.dso"), "\n",
					"\tcopy \"\$<\" \"\$@\"\n"
				);
			}		
		}		
	}

	my $freezeDir = &main::Path_Split('Path', $DefFile);
	chop($freezeDir);

	# dummy rule for def files to cope with filename case differences
	unless (&main::ExportUnfrozen) {
		if (-e $DefFile) { # effectively "if project frozen ..."
			&main::Output(
				"\n",
				"\n",
				&Generic_Quote($DefFile), " : ", "\n",
				"\t\@rem Do nothing\n",
			);
		}
	}

	&main::Output(
		"\n",
		"\n",
		&Generic_Quote($freezeDir), " : ", "\n",
		"\tperl -S emkdir.pl \$\@\n",
	);

	&main::Output(
		"\n",
		"\n",
		"FREEZE : ",
		&Generic_Quote($freezeDir), "\n",
	);
	if ($DefFile and $BasicTrgType!~/^IMPLIB$/io) {
# 	    call perl on the script here so make will die if there are errors 
#           - this doesn't happen if calling perl in a batch file
	    &main::Output( "\tperl -S efreeze.pl \$(EFREEZE_ALLOW_REMOVE) \"$DefFile\" \"\$(EPOCBLD)\\$ExportLibrary.def\" \n" );
	}
	&main::Output(
		"\n",
		"CLEANLIBRARY :\n"
	);
	if ($DefFile and !$NoExportLibrary) {
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCLIB)\\LIB\\$ExportLibrary.lib\"\n"
		);
		if ($ExtraExportLibrary) {
			&main::Output(
				"\t-\$(ERASE) \"\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.lib\"\n"
			);
		}
	}
	&main::Output(
		"\n",
		"\n"
	);
	&Generic_MakeWorkDir('MAKEWORKLIBRARY',"${LibPath}");

	&Generic_Releaseables;
}


sub PMBld {

	my $ABI=&main::ABI;
    my @ASSPLibList=&main::ASSPLibList;
    my @SrcList=&main::SrcList;
    my @StringTables=&main::StringTables;
    my $BaseTrg=&main::BaseTrg;
	my $FeatureVariantBaseTrg=&main::FeatureVariantBaseTrg;
    my $Bld=&main::Bld;
    my $ChopBldPath=&main::Path_Chop(&main::BldPath);
    my $DefFile=&main::DefFile;
    my $EPOCIncPath=&main::EPOCIncPath;
    my $FirstLib=&main::FirstLib;
    if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
	    # Temporary Workaround for RVCT2.1 static libs problem with RVCT2.2 builds
	    # Rename all the static libs used with RVCT2.1 as {libname}2_1.lib
	    if ($FirstLib=~/^\s*(\S+)(\.lib)$/io) {
		    if ($1!~/$RVCTVersion/i) {
			    $FirstLib=$1."${RVCTVersion}.lib";
		    }
	    }
    }
    my $BasicTrgType=&main::BasicTrgType;
    my @LibList;
    my @RTLibList = $RVCT20 ?
      ('udfp.lib', 'udrt.lib', 'udrt.lib(VtblExports.o)'):
      ('dfpaeabi.lib', "dfprvct${RVCTVersion}.lib", 'drtaeabi.lib', 'drtaeabi.lib(VtblExports.o)');
    if ( $RVCTVersion lt "2_2" ) {
		push @RTLibList, "dfprvct${RVCTVersion}-thunk.lib";
		push @RTLibList, "drtrvct${RVCTVersion}.lib";
    }
    else {
		# The scppnwdl.lib should come before drtrvct2_2.lib
		push @RTLibList, "scppnwdl.lib";
		push @RTLibList, "drtrvct${RVCTVersion}.lib";
    }
    my $SystemTrg = &main::SystemTrg;
    my $LibPath= &main::LibPath;
    my $LinkAs=&main::LinkAs;
    my $ExportLibrary=&main::ExportLibrary;
    my $NoExportLibrary=&main::NoExportLibrary;
    # N.B. should get better way to detect kernel probably!!
    $SystemTrg = 1 if ($ExportLibrary =~ /EKERN/i);
    my $ChopRelPath=&main::Path_Chop(&main::RelPath);
    my $RelPath=&main::RelPath;
    my @StatLibList=&main::StatLibList;
    if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
	    # Temporary Workaround for RVCT2.1 static libs problem with RVCT2.2 builds
	    # Rename all the static libs used with RVCT2.1 as {libname}2_1.lib
	    for (my $i =0; $i < scalar(@StatLibList); $i++) {
		    if ($StatLibList[$i]=~/^\s*(\S+)(\.lib)$/io) {
			    if ($1!~/$RVCTVersion/i) {
				    $StatLibList[$i]=$1."${RVCTVersion}.lib";
			    }
		    }
	    }
    }	     
    my $StatLinkPath=&main::StatLinkPath;
    my $Trg=&main::Trg;
    if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
	    if ($BasicTrgType=~/^LIB$/o) {
		    # Temporary Workaround for RVCT2.1 static libs problem with RVCT2.2 builds
		    # Rename all the static libs produced with RVCT2.1 as {libname}2_1.lib
		    if ($Trg=~/^\s*(\S+)(\.lib)$/io) {
			    if ($1!~/$RVCTVersion/i) {
				    $Trg=$1.$RVCTVersion.".lib";
			    }
		    }
		    if ($BaseTrg!~/$RVCTVersion/i) {
			    $BaseTrg .= $RVCTVersion;
		    }
	    }
    }

	#OE Glue Code
	my @oe_exe_libs=("libcrt0.lib");
	my @oe_exe_libs_wchar=("libwcrt0.lib");

	#OE Import Library List
	my @oe_import_library_list=();

    my $TrgType=&main::TrgType;
    my @UidList=&main::UidList;
	my $InterWorking = ($ABI eq 'ARMV4') ? "" : "--inter";
	my %Version = &main::Version();
	my $ExtraExportLibrary;
	unless ($Version{explicit}) {
		$ExtraExportLibrary = $ExportLibrary;
		$ExtraExportLibrary =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
	}	


    my $linkerDebugOpt = "";
    if(&main::DebugSwitchUsed() ){
		if(&main::SymbolicDebugEnabled ()){
#		set the linker's debug flag if the debug switch is specified and enabled.
		$linkerDebugOpt = "${oP}debug ";
		}
	}
	elsif ($Bld =~ /DEB/) {
	$linkerDebugOpt = "${oP}debug ";
	}
    
	if ($Bld =~ /DEB/) {
	@LibList = &main::DebugLibList;
	} else {
	@LibList = &main::LibList;
    }

	if(not(grep /^euser.lib$/, @LibList)){
		push @oe_import_library_list, "euser.lib";
	}
	if(not (grep /^libc.lib$/i, @LibList)){
		push @oe_import_library_list, "libc.lib";
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
		push @releaseables, "$RelPath$Trg.map";
	}
	if (-e $DefFile and !$NoExportLibrary) { # effectively "if project frozen ..."
		push @releaseables, "$LibPath$ExportLibrary.lib";
		push @releaseables, "$LibPath$ExtraExportLibrary.lib" if ($ExtraExportLibrary);
		#if elf2e32.exe(postlinker) exists in the $PATH, then include .dsos also into releasables list
		if ($IsExistELF2E32EXE) {
			push @releaseables, "$LibPath$ExportLibrary.dso";
			push @releaseables, "$LibPath$ExtraExportLibrary.dso" if ($ExtraExportLibrary);
		}		
	}

	push @releaseables, &main::FeatureVariantVMapFile() if &main::FeatureVariantVMapFile();
	&main::Output(
		"WHAT$Bld : WHATGENERIC\n",
		"\n",
		"CLEAN$Bld : CLEANBUILD$Bld CLEANRELEASE$Bld\n",
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
		if ($Ext =~ /cia/i) {
		    $BaseSrc = "$BaseSrc\_";
		}
		&main::Output(
			" \\\n\tLISTING$Bld$BaseSrc"
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	# Compiler wrapper support starts
	if($IsCompilerWrapperOption)
	{
	 	my $Platcmpwrap=&main::Plat;
	 	
		&main::Output(
			"COMPWRAP$Bld : OUTPUT_NAME = ",
			"$Platcmpwrap\_$Bld",
			"\n"
		);
	 	
		&main::Output(
			"COMPWRAP$Bld : MAKEWORK$Bld"
		);

		foreach (@SrcList) {
			my $BaseSrc = &main::Path_Split('Base', $_);
			&main::Output(
				" \\\n\tCOMPWRAP$Bld$BaseSrc"
			);
		}

		&main::Output(
			"\n",
			"\n"
		);
	}
	# Compiler wrapper support

	&main::Output(
		"LIBS$Bld="
	);
	if ($BasicTrgType=~/^DLL$/o) { # Add the DLL stub library
		if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
			# Temporary Workaround for RVCT2.1 static libs problem with RVCT2.2 builds
			&main::Output(
				" \\\n\t",
				&Generic_Quote("\$(EPOCSTATLINK$Bld)\\EDLLSTUB$RVCTVersion.lib")
			);
		}
		else {
			&main::Output(
				" \\\n\t",
				&Generic_Quote("\$(EPOCSTATLINK$Bld)\\EDLLSTUB.lib")
			);
		}
	}
	    
        PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCSTATLINK$Bld\)\\\\\$_\"\)", @StatLibList);
        PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCLIB\)\\\\LIB\\\\\$_\"\)", @LibList);

		#OE Import Libraries 
		if ( $TrgType=~/^STDEXE$/o || $TrgType=~/^STDDLL$/o ) 
		{
			PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCLIB\)\\\\LIB\\\\\$_\"\)", @oe_import_library_list);
		}

		#OE Glue Code
		if ($TrgType=~/^STDEXE$/o) {
			if (&main::IsWideCharMain()) {
				PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCLIB\)\\\\$Bld\\\\\$_\"\)", @oe_exe_libs_wchar);
			}
			else {
				PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCLIB\)\\\\$Bld\\\\\$_\"\)", @oe_exe_libs);
			}
		}

        my $StaticRTLib = $RVCT20 ? "usrt20" : "usrt${RVCTVersion}" ;
        # use ksrt for system code and usrt for user ARM code
        $StaticRTLib = "ksrt${RVCTVersion}" if ($SystemTrg);
        &main::Output(
	        " \\\n\t",
		&Generic_Quote("\$(EPOCSTATLINK$Bld)\\$StaticRTLib\.lib")
		) unless ($Trg =~ /(U|K)SRT/i || ($BasicTrgType=~/^LIB$/o));

	unless ($ArmRT || ($BasicTrgType=~/^LIB$/o)) {
	    my $TargLib = "$ExportLibrary.lib";
		$TargLib =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
	    unless ($SystemTrg) {
			foreach (@RTLibList) {
				&main::Output(
					" \\\n\t",
					&Generic_Quote("\$(EPOCLIB)\\LIB\\$_")
				) unless ($_ =~ /$TargLib/i);
			}
	    }
	}
        PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\$_\"\)", @ArmLibList);
	&main::Output(
		"\n",
		"\n"
	);

   	&main::Output(
 		"\n# ADDITIONAL LINKER OPTIONS",
 		"\nUSERLDFLAGS = ",
 		&main::LinkerOption("ARMCC"),
  		"\n\n"
  	);
	
	&main::Output(
		"VTBLEXPORTS$Bld="
	);
        my $vtobj = quotemeta("(VtblExports.o)");
        PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCLIB\)\\\\LIB\\\\\$_$vtobj\"\)", @LibList);
	&main::Output(
		"\n",
		"\n"
	);

	my $objectFiles = "";

	my $bldPath =	&main::BldPath;
	my $replacement;
# If LOCAL_BUILD_PATH is set replace the \epoc32\build with local setting
	if ( defined( $ENV{LOCAL_BUILD_PATH} ) )  {
		$replacement = 	$ENV{"LOCAL_BUILD_PATH"};
		my $epocroot=$ENV{"EPOCROOT"};
		my $match = "\Q${epocroot}\EEPOC32\\\\BUILD";
		$bldPath =~ s/${match}/${replacement}/;
	}
#	Must add StringTable obj files first to preserve Evalid results consistency.
	foreach my $item (@StringTables) {
		$objectFiles .= $bldPath.$$item{BaseTrg}.".o\n" if !($$item{Hdronly});
	}

        &main::Output(
	        "OBJECTS$Bld="
	);
        foreach (@SrcList) {
	    	my $BaseSrc = &main::Path_Split('Base', $_);
	    	my $Ext = &main::Path_Split('Ext', $_);
		if ($Ext =~ /cia/i) {
		    $BaseSrc = "$BaseSrc\_";
		}

	        &main::Output(
		        " \\\n\t",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o")
		      );

#	Only add if not already added from @StringTables
	my $objectFile = $bldPath.$BaseSrc.".o\n";
	$objectFile=~ s/\\/\\\\/g;    # escape the '\'
	if ($objectFiles !~ m/$objectFile/i){
		$objectFiles .= &main::BldPath.$BaseSrc.".o\n";
	}


	}
        &main::Output(
	        "\n",
		"\n"
	);


	# Create "via" file containing all object files in order to reduce
	# command line lengths in pertinent calls
	my $objectsViaFile = &main::CommandFile();	
	&main::CreateExtraFile($objectsViaFile, $objectFiles);
	

        if ($BasicTrgType=~/^LIB$/o) {
	        &main::Output(
		      &Generic_Quote("\$(EPOCTRG$Bld)\\$Trg"),
		      " : \$(OBJECTS$Bld)"
		);
        } else {
	        &main::Output(
		      &Generic_Quote("\$(EPOCTRG$Bld)\\$Trg"), " : ",
		      &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg.in")
	        );
        }

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
		" \$(LIBS$Bld)"
	);

#	generate an export object from the ordered .DEF file
        if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
#       	make the .exp file a dependency for targets that have exports		
		&main::Output(" ", &Generic_Quote("\$(EPOCBLD$Bld)\\$ExportLibrary.exp"), "\n");
		if (&main::ExportUnfrozen) {
		    &main::Output(
			"\tdef2dll.bat --path=\$(EPOCBLD$Bld) \\\n\t\t--bldpath=\$(EPOCBLD$Bld) \\\n\t\t--export=$ExportLibrary \\\n\t\t--import=$ExportLibrary\\\n",
			"\t\t--deffile=\$(EPOCBLD$Bld)\\$ExportLibrary.def \\\n\t\t--linkAs=$LinkAs \\\n\t\t$InterWorking $symNameLkupOpt\n",
		    );
		    &main::Output(
		        "\n",
		        "\tcopy ", " \"\$(EPOCBLD$Bld)\\$ExportLibrary.lib\" ",
		        "\"\$(EPOCLIB)\\LIB\\$ExportLibrary.lib\"",
		        "\n"
		    );
		    if ($ExtraExportLibrary) {
			&main::Output(
			    "\n",
			    "\tcopy \"\$(EPOCLIB)\\LIB\\$ExportLibrary.lib\" ",
			    "\"\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.lib\"",
			    "\n"
			);
		    }			    
		    #if elf2e32.exe(postlinker) exists, then generate .dso(which will be used by ABIV2 platforms)
		    if ($IsExistELF2E32EXE) {
			    &main::Output(
				    "\n",
				    "\telf2e32 --definput=\"\$(EPOCBLD$Bld)\\$ExportLibrary.def\" --dso=",
				    "\$(EPOCLIB)\\LIB\\$ExportLibrary.dso --linkas=$LinkAs\n"
			    );
			    if ($ExtraExportLibrary) {
				    &main::Output(
					    "\n",
					    "\tcopy \"\$(EPOCLIB)\\LIB\\$ExportLibrary.dso\" ",
					    "\"\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.dso\"",
					    "\n"
				    );
			    }		
		    }		
	        }
	} elsif($NamedSymLkup){
#		For an EXE, generate the .exp to accomodate 0th ordinal
		&main::Output(" ", &Generic_Quote("\$(EPOCBLD$Bld)\\$ExportLibrary.exp"), "\n");
	}
	else {
	    &main::Output("\n");
	}

#       get rid of any -symbols produced .map file
        if ($BasicTrgType=~/^(DLL|EXE)/o) {
	        &main::Output(
			"\t-\$(ERASE) \"\$(EPOCTRG$Bld)\\$Trg.map\" \n"
		);	
	}
#		Generate the dependency info - This is required to put the libraries in the same order
#		as mentioned in mmp.
		if($NamedSymLkup) {
			&main::Output(
			"\tperl -S deputil.pl $InterWorking --path=\$(EPOCBLD$Bld) \\\n",
			 "\t\t--out=$ExportLibrary \\\n",
			 "\t\t--libpath=\$(EPOCLIB)\\LIB \\\n",
			 "\t\t\$(LIBS$Bld)\n",
			);
		}
		my $AbsentSubst = '';
        if ($BasicTrgType=~/^(DLL|EXE)/o) {
	        my $datalinkbase = "0x400000";
	        $datalinkbase = &main::DataLinkAddress if (&main::DataLinkAddress);

#               make sure the linker feedback file is writable if it exists.
		my $lfbfile = LinkerFeedBackFile();
		&main::Output(
			"\t\@if exist $lfbfile attrib -r $lfbfile\n"
			) if $useLinkerFeedBack;

		my $lfboption = LinkerFeedBackOption();

	        &main::Output(
		        "\t$Link $linkerDebugOpt ${oP}shl ${oP}reloc ${oP}split ${oP}rw-base $datalinkbase  \\\n\t\t$lfboption${oP}noscanlib $PlatOpt{Ld}\\\n"
			);
			my $EntrySymbol;
			if ($BasicTrgType=~/^DLL$/o) {
				$EntrySymbol = '_E32Dll';
			}
			elsif ($BasicTrgType=~/^EXE$/o) {
				$EntrySymbol = '_E32Startup';
			}
			if ($EntrySymbol) {
				$AbsentSubst = " -absent $EntrySymbol";
			}
	        if ($BasicTrgType=~/^DLL$/o) {
	            # get the right object file for the entry point
	            my $ObjFile = "UC_DLL_.o";
	            if ($FirstLib =~ /EDEV/i) {
		            $ObjFile = "D_ENTRY_.o";
	            }
	            if ($FirstLib =~ /EKLL/i) {
		            $ObjFile = "L_ENTRY_.o";
	            }
	            if ($FirstLib =~ /EEXT/i) {
		            $ObjFile = "X_ENTRY_.o";
	            }
	            if ($FirstLib =~ /EVAR/i) {
		            $ObjFile = "V_ENTRY_.o";
	            }
	            &main::Output(
				    "\t\t${oP}entry _E32Dll \$(EPOCSTATLINK$Bld)\\$FirstLib($ObjFile) \$(EPOCSTATLINK$Bld)\\$FirstLib \\\n",
				    "\t\t\$(EPOCBLD$Bld)\\$ExportLibrary.exp \\\n"
				);
	        } elsif ($BasicTrgType=~/^EXE$/o || $TrgType=~/^EXEXP$/o) {
			    # get the right object file for the entry point
			    my $ObjFile = "UC_EXE_.o" ;
			    if ($FirstLib =~ /KC_EXE/i) {
					$ObjFile = "K_ENTRY_.o";
			    }
				# If building user-side under RVCT2.0.x, use 2.0.1 static library	
				if ($RVCT20) { 
					if ($ObjFile =~/UC_EXE_.o/i) { 
						$FirstLib = "EEXE20.LIB"; 
					} 
				} 
			    
			    &main::Output( "\t\t${oP}entry _E32Startup \$(EPOCSTATLINK$Bld)\\$FirstLib($ObjFile) \$(EPOCSTATLINK$Bld)\\$FirstLib \\\n" );
			    if ($TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o || $NamedSymLkup) {
					&main::Output( "\t\t\$(EPOCBLD$Bld)\\$ExportLibrary.exp \\\n" );
			    }
			}
			if($NamedSymLkup) {
				&main::Output(
				"\t\t--edit \"\$(EPOCBLD$Bld)\\$ExportLibrary.dep\" \\\n"
				);
			}
	        &main::Output(
		        "\t\t-o \"\$(EPOCBLD$Bld)\\$Trg\" \\\n",
		        "\t\t${oP}symbols ${oP}list \"\$(EPOCTRG$Bld)\\$Trg.map\" \\\n",
				"\t\t\$(EPOCBLD$Bld)\\$BaseTrg.in \\\n"
			);
	        &main::Output(
		        "\t\t\$(LIBS$Bld) \\\n",
				"\t\t\$(VTBLEXPORTS$Bld) \$(USERLDFLAGS) \n"
			);

	        if(&main::DebugSwitchUsed() ){
				if(&main::SymbolicDebugEnabled() ) {
				&main::Output(
					"\tcopy \"\$(EPOCBLD$Bld)\\$Trg\" \"\$(EPOCTRG$Bld)\\$FeatureVariantBaseTrg.sym\"\n"
					);
				}
			}
	        elsif ($Bld=~/^UDEB$/o) {
	               &main::Output(
			       "\tcopy \"\$(EPOCBLD$Bld)\\$Trg\" \"\$(EPOCTRG$Bld)\\$FeatureVariantBaseTrg.sym\"\n"
		       );
	        }
		
		if (&main::CompressTarget) {
		    &main::Output(
			    "\telftran $PlatOpt{Elftran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " ", " -nocompress "
				 );
		    
		}
		else {
			if(&main::CompressTargetMode==NOCOMPRESSIONMETHOD){
				&main::Output(
					"\telftran $PlatOpt{Elftran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " "
				);
			}
			elsif(&main::CompressTargetMode==INFLATECOMPRESSIONMETHOD){
				&main::Output(
				"\telftran $PlatOpt{Elftran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " ", "  -compressionmethod deflate"
				);
			}
			elsif(&main::CompressTargetMode==BYTEPAIRCOMPRESSIONMETHOD){
				&main::Output(
					"\telftran $PlatOpt{Elftran} -version ", &Genutl_VersionToUserString(%Version), " -sid ", &main::SecureId(), " ", "  -compressionmethod bytepair"
				);
			}
		}

		if (&main::IsDebuggable eq DEBUGGABLE) {
 			&main::Output(
 				' -debuggable '
 			);
 		}

		if (&main::SmpSafe) {
 			&main::Output(
 				' -smpsafe'
 			);
 		}

		if (&main::IsDebuggable eq DEBUGGABLE_UDEBONLY) {		
			if ($Bld=~/^UDEB$/o) {
				&main::Output(
				' -debuggable '
				);
			}
		}
		
		# change - exexps are allowed data, but they look like dlls to elftran....
		if (&main::AllowDllData || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			&main::Output(
				' -allow'
			);
		}
		if (not &main::CallDllEntryPoints ) {
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

		&main::Output(
			"\\\n\t\t"
		);

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
			"\\\n\t\t"
		);
		&main::Output(
			' -fpu ',$floatingpointmodel
		);
		&main::Output(
			' -capability ',&main::Capability
		);
		if($NamedSymLkup) {
			&main::Output(
				' -sym_name_lkup'
			);
		}
		&main::Output(
			"\\\n\t\t"
		);
		&main::Output(
			" \"\$(EPOCBLD$Bld)\\$Trg\""
		);
		&main::Output(
			"\\\n\t\t"
		);
		&main::Output(
			" \"\$\@\" \n"
		);
		&main::Output(
			"\n"
		);

         }
         elsif ($BasicTrgType=~/^LIB$/o) {
	        &main::Output(
		        "\tarmar ${oP}create \$(EPOCSTATLINK$Bld)\\$Trg ${oP}via $objectsViaFile\n"
		);
         }

         &main::Output(
		 "\n"
	 );
	 
#	add static lib into the object via file
	my $libViaFiles=$objectFiles;
	if (@StatLibList) {
		foreach (@StatLibList) {
			$libViaFiles.= &main::RelPath."$_ \n";
		}
		&main::CreateExtraFile($objectsViaFile, $libViaFiles);
	}
	
    # TARGET *.IN
    #------------
	 
    &main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg.in"), ": \$(OBJECTS$Bld)\n",
 	    "\t$Link $linkerDebugOpt ${oP}partial \\\n",
	    "\t\t-o \$\@ \\\n",
	    "\t\t${oP}via $objectsViaFile\n\n",
    );
    

#   reorder the .DEF file taking frozen exports into account if there are any
    if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o || $NamedSymLkup) {

	    # TARGET *.EXP
	    #------------
	    &main::Output(
		    &Generic_Quote("\$(EPOCBLD$Bld)\\$ExportLibrary.exp"), ": \$(EPOCBLD$Bld)\\$BaseTrg.in"
	    );

# if project is frozen, makedef (and hence the .exp file) are dependent upon it
		unless (&main::ExportUnfrozen) {
			if (-e $DefFile) { # effectively "if project frozen ..."
			    &main::Output(" $DefFile");
			}
		}

	    &main::Output(
		"\n\tperl -S elf2inf.pl -o \$(EPOCBLD$Bld)\\$ExportLibrary.inf \\\n", 
		"\t\t\$\<",
		"\n\tperl -S makedef.pl $AbsentSubst -Inf \$(EPOCBLD$Bld)\\$ExportLibrary.inf \\\n"
	    );
    	if (!$DefFile || $NoExportLibrary) {    			
    		&main::Output( "\t\t-ignore_unfrozen_noncallable \\\n" );
    	}
	if (SysTrg()) {
    		&main::Output( "\t\t-SystemTargetType \\\n" );
    	}
    		
	    if (-e $DefFile) {	# effectively "if project frozen ..."
	            &main::Output(
			"\t\t-Frzfile \"$DefFile\" \\\n"
		    );
	    }

		if($NamedSymLkup && !$DefFile){
#		For an EXE with named lookup, suppress the 'unfrozen exports' makedef warnings.
			&main::Output(
			"\t\t-ignore_unfrozen_exports \\\n",
			);
		}

	    # freeze ordinals, a maximum of 2, for polymorphic dlls
	    my $Ordinal;
	    my $Num=1;
	    foreach $Ordinal (&main::Exports) {
	            &main::Output( "\t\t-$Num $Ordinal \\\n" );
		    $Num++;
	    }
	    
	    my $theDefFile = "\$(EPOCBLD$Bld)\\$ExportLibrary.def";
	    $theDefFile = $DefFile if (-e $DefFile && !&main::ExportUnfrozen);
	    &main::Output(
		"\t\t\"\$(EPOCBLD$Bld)\\$ExportLibrary.def\"\n",
		"\tcopy \"\$(EPOCBLD$Bld)\\$ExportLibrary.def\" \"\$(EPOCBLD)\\$ExportLibrary.def\"\n",
		"\tdef2dll.bat $AbsentSubst \\\n\t\t--path=\$(EPOCBLD$Bld) \\\n\t\t--bldpath=\$(EPOCBLD$Bld) \\\n\t\t--export=$ExportLibrary \\\n",
		"\t\t--deffile=$theDefFile \\\n\t\t--linkAs=$LinkAs \\\n\t\t$InterWorking $symNameLkupOpt\n"
	    );
    }

    &main::Output( "\n" );
}

# Set to 1 if multifile compilation wanted
my $domultifile = 0;

sub DoMultiFile () {
        return $ENV{RVCTMultiFile} if (defined $ENV{RVCTMultiFile});
	return $domultifile;
}

my %CompilationGroups = ();

sub InitMultiFileCompilation() {
#	Do preparatory work for multifile compilation
	my $SourceStructRef=&main::SourceStructRef;

#	We sort the source files by path and extension. These form natural groups to compile together.
	my %PathToSourceMap = ();
	foreach my $SourceRef (@$SourceStructRef) {
		my $SrcFile = $$SourceRef{CurFile};
		my $Ext = &main::Path_Split('Ext', $SrcFile);
	        push @{$PathToSourceMap{$$SourceRef{SrcPath}}{$Ext}}, $SrcFile;
	}

#	Now we split each group into sets of 10. 
	foreach my $SrcPath (keys %PathToSourceMap) {
		foreach my $Ext (keys %{$PathToSourceMap{$SrcPath}}) {
			my @FileList;
			my @ObjectList;
			my @SourceList;
			my $NumToGo = 10;
			foreach my $File (@{$PathToSourceMap{$SrcPath}{$Ext}}) {
				my $base = &main::Path_Split('Base', $File);
				my $cia = ($Ext =~ /cia/i);
				$base .= "_" if $cia;
				push @FileList, $File;
				push @ObjectList, "$base.o";
#				this gives us our source files xxx				
				push @SourceList, $cia ? "$base.cpp" : "$SrcPath$base$Ext";
				$NumToGo--;
				unless ($NumToGo) {
#				       Use the last file as the key. This means e.g that all the dependency
#				       info will have been generated for the earlier files in the list
				       push @{$CompilationGroups{$FileList[$#FileList]}{Sources}}, @SourceList;
       				       push @{$CompilationGroups{$FileList[$#FileList]}{Objects}}, @ObjectList;
				       $NumToGo = 10;
				       undef @FileList;
				       undef @ObjectList;
				       undef @SourceList;
				}
			}
			push @{$CompilationGroups{$FileList[$#FileList]}{Sources}}, @SourceList;
			push @{$CompilationGroups{$FileList[$#FileList]}{Objects}}, @ObjectList;
		}
	}

#	debug print out	
	if (0) {
	foreach my $keyfile (keys %CompilationGroups) {
		print "$keyfile :\n";
		foreach my $class (keys %{$CompilationGroups{$keyfile}}) {
			print "\t$class:\n\t\t";
			print join " ", @{$CompilationGroups{$keyfile}{$class}}, "\n";
		}
	}
	}
			
}

sub PMStartSrcList {

	&main::Output(
		"# SOURCES\n",
		"\n"
	);

	InitMultiFileCompilation() if DoMultiFile();

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
	my @DepList=&main::DepList;
	return if (@DepList == 0);

	my @BldList=&main::BldList;	
	my $BaseSrc=&main::BaseSrc;
	my $ExtSrc=&main::ExtSrc;
	
	my $BaseObj=$BaseSrc;
	my $cia = 0;
	if ($ExtSrc =~ /cia/i ) {
		$cia = 1;
		$BaseObj .= '_';
	}

	foreach (@BldList) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseSrc.pre"), " ",
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseObj.cpp"), " ",
		) if $cia;
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseSrc.lis"), " ",
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseObj.o"), " \\\n",
		);
	}
	&main::Output(
		":"
	);
        PrintList("\' \\\n\t\'\.\&Generic_Quote\(\$_\)", @DepList);
	&main::Output(
		"\n",
		"\n"
	);
}

sub PMSrcBldDepend {
	my @DepList=&main::DepList;
	return if (@DepList == 0);
	
	my $Bld=&main::Bld;
	my $BaseSrc=&main::BaseSrc;
	my $ExtSrc=&main::ExtSrc;
	
	my $BaseObj=$BaseSrc;
	my $cia = 0;
	if ($ExtSrc =~ /cia/i ) {
		$cia = 1;
		$BaseObj .= '_';
	}

	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), " ",
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseObj.cpp"), " ",
	) if $cia;
	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " ",
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseObj.o"), " :",
	);
        PrintList("\' \\\n\t\'\.\&Generic_Quote\(\$_\)", @DepList);
	&main::Output(
		"\n",
		"\n"
	);
}

my $curdrive = "x";

sub quoted_path
    {
    my ($arg) = @_;
    return "\"$arg\"" if ($arg !~ /^\\[^\\]/);	# not an absolute path
    if ($curdrive eq "x")
		{
		$curdrive="";
		$curdrive=$1 if (cwd =~ /^(.:)/);	
		}
    return "\"$curdrive$arg\"";
    }

sub PMPrefixFile 
{ 
    my $IncPath = &main::EPOCIncPath;
    
    return quoted_path(&Generic_Quote("$IncPath"."rvct\\rvct.h"));
}


my $preinclude =   "--preinclude \$(EPOCINC)\\rvct\\rvct.h";
my $edg_preinclude = "-I \$(EPOCINC)\\RVCT${RVCTVersion} --preinclude edg_rvct${RVCTVersion}.h";

sub SelectLangOptions {
	my ($Ext) = @_;
	if ($Ext=~/^.cpp$/) {
		# In case of function call logger, the preinclude file is passed to the function call logger
		# hence it is not required to pass the same file to the compiler.
		return "--cpp "	if ($Function_Call_Logger);
		return "--cpp $preinclude ";
	}
	if ($Ext=~/^.cia$/) {
		return "--cpp ";
	}
	if ($Ext=~/^.c$/) {
		if($CompilerOption =~/--cpp/) {
			#Function Call Logger
			return "--cpp " if ($Function_Call_Logger);
			return "--cpp $preinclude ";
		}
		else {
			#Function Call Logger
			return "--c90 " if ($Function_Call_Logger);
			return "--c90 $preinclude ";
		}
	}
	# To support .cc, .cxx, .c++ file extensions for Open Environment
	elsif ($Ext=~/^(.cc|.cxx|.c\+\+)$/) {
		#Function Call Logger
		return "--cpp " if ($Function_Call_Logger);
		return "--cpp $preinclude ";
	}
	return '';
}

sub PMEndSrcBld {
#       Generate multifile compilation stuff if needed.
        if (DoMultiFile()) {
	       MultiFileEndSrcBld();
	       return;
	}

	my $ABI=&main::ABI;
	my $Plat=&main::Plat;
	my $BaseSrc=&main::BaseSrc;
	my $Bld=&main::Bld;
	my $Src=lc &main::Src;	
	my $SrcPath=&main::Path_Chop(&main::SrcPath);
	my $Ext = &main::Path_Split('Ext', $Src);	
	my $BaseTrg=&main::BaseTrg;
	my $BldPath = &main::BldPath;
	$Src = ucfirst $Src if ($Ext !~ /\.(cpp|c)$/);		
	my $LangOptions = &SelectLangOptions($Ext);
	# support for auto 'translated' ASM 
	my $AsmFilep = $AsmFiles{$Src};

	# Logger Ouput filename 
	my $Logger_Output = lc ($BaseSrc) . ".int.cpp";
	my $LstExt ;
	if($Plat =~ /^(ARMV[6-9])/i){
		$LstExt = $1 ;	
	}
	else{
		$LstExt = $ABI;
	}
	
	my $lfboption = LinkerFeedBackOption();

	#Function Call Logger
	my $FC_Logger_Option=" --wchar_t_keyword --microsoft_version=1300 --dictionary_file_name $BldPath$BaseTrg.txt --diag_suppress 66,161,611,654,815,830,997,1152,1300,1390";

	if ($AsmFilep || $Ext =~ /cia/i) {
		&main::Output(
# compile the translated, preprocessed source
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.o"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), "\n",
			"\t\@echo $Src\n",
			"\t\$(ARMCC$Bld) $lfboption$LangOptions -J $SrcPath \$(INCDIR) -o \$\@ \$(EPOCBLD$Bld)\\$BaseSrc\_.cpp\n",
			"\n",
# rule to translate the preprocessed source
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), "\n",
			"\ttranasm.bat -n -s -o=\$\@ \$(EPOCBLD$Bld)\\$BaseSrc.pre\n",
			"\n",
# rule to preprocess the source
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), " : ",
			&Generic_Quote("$SrcPath\\$Src"), "\n",
  			"\t\$(ARMCC$Bld) -D__CIA__ -E $preinclude $LangOptions -J $SrcPath \$(INCDIR) $SrcPath\\$Src -o \$\@ \n",
# generate an assembly listing target too
			"LISTING$Bld$BaseSrc\_ : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath\\$BaseSrc\_.$LstExt.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), "\n",
			"\t\$(ARMCC$Bld) $LangOptions -S -J $SrcPath \$(INCDIR) -o \$\@ \$(EPOCBLD$Bld)\\$BaseSrc\_.cpp\n",
			"\n"
			);
	} else {
		#If Function Call logging is enabled, add call to function call logger
		if ($Function_Call_Logger) {
			&main::Output(
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"),
				" : ",
				&Generic_Quote("\$(EPOCBLD$Bld)\\$Logger_Output"),
				"\n",
				"\t\@echo $Logger_Output\n",
				"\t\$(ARMCC$Bld) $lfboption$LangOptions -J $SrcPath \$(INCDIR) -o \$\@ \$(EPOCBLD$Bld)\\$Logger_Output\n",
				"\n",
# generate an assembly listing target too
				"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), "\n",
				"\t", &Generic_CopyAction("$SrcPath\\$BaseSrc.$LstExt.lst"),
				"\n",
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " : ",
				&Generic_Quote("$SrcPath\\$Logger_Output"), "\n",
				"\t\$(ARMCC$Bld) $LangOptions -S -J $SrcPath \$(INCDIR) -o \$\@ \$(EPOCBLD$Bld)\\$Logger_Output \n",
				"\n"
			);

			#Call to Function Call Logger
			&main::Output(
				&Generic_Quote("\$(EPOCBLD$Bld)\\$Logger_Output"), " : ",
				&Generic_Quote("$SrcPath\\$Src"),
				"\n",
				"\t \@echo $Logger_Output\n",
				"\t \$(FCLOGGER$Bld) $lfboption$edg_preinclude \\\n",
			    "\t -I $SrcPath  \\\n",
				"\t \$(INCDIR_FCLOGGER) $FC_Logger_Option \\\n",
			    "\t --gen_c_file_name \$\@ $SrcPath\\$Src\n",
				"\n\n",
			);
		}
		else {			
			&main::Output(
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"), " : ",
				&Generic_Quote("$SrcPath\\$Src"), "\n",
				"\t\@echo $Src\n",
				"\t\$(ARMCC$Bld) $lfboption$LangOptions -J $SrcPath \$(INCDIR) -o \$\@ $SrcPath\\$Src\n",
				"\n",
# generate an assembly listing target too
			"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath\\$BaseSrc.$LstExt.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " : ",
			&Generic_Quote("$SrcPath\\$Src"), "\n",
			"\t\$(ARMCC$Bld) $LangOptions -S -J $SrcPath \$(INCDIR) -o \$\@ $SrcPath\\$Src \n",
			"\n"
			);
			#Compiler wrapper support starts
			if($IsCompilerWrapperOption)
			{
				my $Platcmpwrap=&main::Plat;
				&main::Output(
					"COMPWRAP$Bld$BaseSrc : ",
					&Generic_Quote("$SrcPath\\$Src"), "\n",
					"\t\@echo Analysing $Src\n",
					"\t\$(COMPWRAP) \$(ARMCC$Bld) $LangOptions -S -J $SrcPath \$(INCDIR) -o \$\@ $SrcPath\\$Src \n",
					"\n"
				);
			}
			#Compiler wrapper support ends
			
		}
	}
}

my $MFVarN = 0;
sub MultiFileEndSrcBld {
	my $ABI=&main::ABI;
	my $BaseSrc=&main::BaseSrc;
	my $Bld=&main::Bld;
        my $KeyFile = &main::Src;
	my $Src=ucfirst lc $KeyFile;
	my $SrcPath=&main::Path_Chop(&main::SrcPath);
	my $Ext = &main::Path_Split('Ext', $Src);
	my $LangOptions = &SelectLangOptions($Ext);
	# support for auto 'translated' ASM 
	my $AsmFilep = $AsmFiles{$Src};

	my $lfboption = LinkerFeedBackOption();

	if ($AsmFilep || $Ext =~ /cia/i) {
		if ($CompilationGroups{$KeyFile}) {
# compile the translated, preprocessed source
		       &main::Output( "OBJECTS$MFVarN = ");
		       foreach my $obj (@{$CompilationGroups{$KeyFile}{Objects}}) {
			       &main::Output( &Generic_Quote("\\\n\t\$(EPOCBLD$Bld)\\$obj"), " "); 
		       }
       		       &main::Output( "\n\n");
		       &main::Output( "SOURCES$MFVarN = ");
		       foreach my $src (@{$CompilationGroups{$KeyFile}{Sources}}) {
			       &main::Output( &Generic_Quote("\\\n\t\$(EPOCBLD$Bld)\\$src", " "));
		       }
       		       &main::Output( "\n\n");
		       &main::Output( "\$(OBJECTS$MFVarN) : \$(SOURCES$MFVarN) \n");

		       &main::Output(
				     "\t\@echo Compiling \$(SOURCES$MFVarN)\n", 
				     "\t\$(ARMCC$Bld) -J $SrcPath \$(INCDIR) $lfboption\\\n",
				     "\t\t$LangOptions -o \$\@ --multifile \$(SOURCES$MFVarN)"
		      );
       		       &main::Output( "\n\n");
		       $MFVarN++;
		}
		&main::Output(
# rule to translate the preprocessed source
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), "\n",
			"\ttranasm.bat -n -s -o=\$\@ \$(EPOCBLD$Bld)\\$BaseSrc.pre\n",
			"\n",
# rule to preprocess the source
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), " : ",
			&Generic_Quote("$SrcPath\\$Src"), "\n",
  			"\t\$(ARMCC$Bld) -D__CIA__ -E $preinclude $LangOptions -J $SrcPath \$(INCDIR) $SrcPath\\$Src -o \$\@ \n",
# generate an assembly listing target too
			"LISTING$Bld$BaseSrc\_ : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath\\$BaseSrc\_.$LstExt.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), "\n",
			"\t\$(ARMCC$Bld) $LangOptions -S -J $SrcPath \$(INCDIR) -o \$\@ \$(EPOCBLD$Bld)\\$BaseSrc\_.cpp\n",
			"\n"
			);
	} else {

		if ($CompilationGroups{$KeyFile}) {
#                      compile the source
		       &main::Output( "OBJECTS$MFVarN = ");
		       foreach my $obj (@{$CompilationGroups{$KeyFile}{Objects}}) {
			       &main::Output( &Generic_Quote("\\\n\t\$(EPOCBLD$Bld)\\$obj"), " "); 
		       }
       		       &main::Output( "\n\n");
		       &main::Output( "SOURCES$MFVarN = ");
		       foreach my $src (@{$CompilationGroups{$KeyFile}{Sources}}) {
			       &main::Output( &Generic_Quote("\\\n\t$src"), " ");
		       }
       		       &main::Output( "\n\n");
		       &main::Output( "\$(OBJECTS$MFVarN) : \$(SOURCES$MFVarN) \n");

		       &main::Output(
				     "\t\@echo Compiling \$(SOURCES$MFVarN)\n", 
				     "\t\$(ARMCC$Bld) -J $SrcPath \$(INCDIR) $lfboption\\\n",
				     "\t\t$LangOptions -o \$\@ --multifile \$(SOURCES$MFVarN)"
		      );
       		       &main::Output( "\n\n");
		       $MFVarN++;
		}
#		generate an assembly listing target too
		&main::Output(
			"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath\\$BaseSrc.$LstExt.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " : ",
			&Generic_Quote("$SrcPath\\$Src"), "\n",
			"\t\$(ARMCC$Bld) $LangOptions -S -J $SrcPath \$(INCDIR) -o \$\@ $SrcPath\\$Src \n",
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

sub PMSupportsFeatureVariants
	{
	return 1;
	}

1;







