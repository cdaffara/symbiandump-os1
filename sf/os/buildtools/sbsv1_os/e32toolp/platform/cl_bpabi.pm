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

# This package contains routines to handle Base Platform ABI (BPABI) Platforms.
package Cl_bpabi;

my @commonOptions;
my @thumbOptions;
my @armOptions;
my @kernelOptions;
my @invariantOptions;
my @linkerOptions;
my @archiverOptions;
my @debugFormat;
my $ArmIncDir;
my @ArmLibList;

my %configVariables;

#Check if Function call Logger is enabled
my $Function_Call_Logger=&main::IsFunctionCallLogging();

# Get the information regarding supporting Compiler Wrapper Option
my $IsCompilerWrapperOption=&main::CompilerWrapperOption();
my $IsProxyWrapperOption=&main::ProxyWrapperOption();

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

sub DepLinkList
{
    my $expr = shift @_;
    my @arr;
    foreach (@_) {
	my $str = eval($expr);
	push @arr, $str;
    }
    return @arr;
}

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
	PMToolChainIncDir
	PMSupportsFeatureVariants
);

use Cwd;

# Armutl package does various ancillary things for armedg modules
use Armutl;
use BPABIutl;
use E32Plat;
use RVCT_plat2set;
use gcce_plat2set;

# This is the RVCT Version information required by Armutl package
my $RVCTMajorVersion;
my $RVCTMinorVersion;
my $RVCTVersion;

my $GCCEMajorVersion;
my $GCCEMinorVersion;
my $GCCEVersion;

# The name of the build platform, e.g. ARMV6.
my $PlatName = main::Plat();

# The name of the root platform. This is useful if the plaform is derived using a
# BSF file. If the current platform is not derived, $RootName will be the same as
# $PlatName.
my $RootName = E32Plat::Plat_Root($PlatName);


# cl_generic package contains generic routines to handle bits of makefiles which are
# common to all of the platforms. Currently it deals with AIF, MBM amd RSC files.
use cl_generic;

# E32env package contains information for makmake and associated e32tools perl programs
# within the Epoc32 Environment
use E32env;

# Genutl package contains utility subroutines for MAKMAKE and associated scripts
use Genutl;

# Modload package is the runtime module-loading routine for loading e32tools modules into
# 'main' module
use Modload;

use constant NOCOMPRESSIONMETHOD => 0;
use constant INFLATECOMPRESSIONMETHOD => 1;
use constant BYTEPAIRCOMPRESSIONMETHOD => 2;

use constant NOTPAGED => 0;
use constant UNPAGED => 1;
use constant PAGED => 2;

use constant NON_DEBUGGABLE => 0;
use constant DEBUGGABLE => 1;
use constant DEBUGGABLE_UDEBONLY => 2;

my %plat = &main::PlatRec();
my $CustomizedARMV5Plat = 0;

if (($plat{'CUSTOMIZES'}) && (($plat{'ROOTPLATNAME'} eq "ARMV5") || ($plat{'ROOTPLATNAME'} eq "ARMV5_ABIV2"))) {
# The following flag is set to handle the exceptions related to ARMV5 toolchain.
	$CustomizedARMV5Plat = 1;		
}

#this fucntion will be used for cw_ide platform to update the PlatName and reinitialize the hashtable
sub getVariableForNewPlat
{
	$PlatName = main::Plat();	
	undef(%configVariables);
}
sub PMHelp_Mmp {

# Help Text for ARM Platform, lists out the MMP keywords used incase of ARM Compiler
	if($PlatName eq "ARMV5" || $PlatName eq "ARMV5_ABIV2" || $CustomizedARMV5Plat)
	{
		return &Armutl_Help_Mmp;
	}
	else
	{
		return "";
	}
}

my $ToolChainIncDir;
my @ToolChainLibList;
my $ArmRT = 0;
my %AsmFiles = ();
my %AsmDirs = ();
my $NamedSymLkup = 0;
my $InterWorking = '';

sub PMPlatProcessMmp (@) {
	&InitToolChain(@_);
	$ToolChainIncDir = &GetToolChainIncDir;
	&main::SetStdIncPaths($ToolChainIncDir);
	@ToolChainLibList = &GetLibList;
# Variable to check if the target forms part of the run time libraries, if it is so
# shouldn't be linked against itself or other runtime libs
	$ArmRT = &IsTargetRT;
	my @AsmFileList = &GetToolChainAsmFileList;
	foreach (@AsmFileList) { $AsmFiles{ucfirst lc $_} = 1; }
}

my $preinclude;

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
	push @MacroList, "__SYMBIAN_STDCPP_SUPPORT__" if ( StdCppTarget() );

  	my $Plat=&main::Plat;
  	my $Trg=&main::Trg;
  	my $TrgType=&main::TrgType;
  
  # This horrible change (presumably to allow GCCE to work with edll.lib etc.
  # produced by RVCT breaks SMP (and also breaks any optimized platform
  # builds such as ARMV6, and also Thumb2 builds).
  # Work round for now by conditioning the horrible change on the absence of
  # SMP in the platform definition.
  	my %PlatHash = &main::PlatRec();
  	unless ($PlatHash{SMP}) {
		my $myStatLinkPath;
		$myStatLinkPath = "$E32env::Data{LinkPath}";
		$myStatLinkPath .= "ARMV5";
		&main::SetStatLinkPath($myStatLinkPath);
	}

	my $VariantFile=&ChangeSlash(&main::VariantFile);

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

	# Required for .lib generation
	$InterWorking = ($ABI eq 'ARMV4') ? "" : "--inter";

	# need to add config file for makmake invocation
	my $config_file = BPABIutl_Config_Path($PlatName);

	&main::Output("\n", "include $config_file\n", "\n");
	&Generic_Header(0,$Makecmd, 1);	# define standard things using absolute paths and request that a make function
									# is provided to provide optional conversion of absolute paths to Unix slashes

# modified start: makefile improvement 
	&main::Output(
		"CHECKVMAP : CHECKVMAPUDEB CHECKVMAPUREL",
		"\n"
	);
# modified end: makefile improvement 
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

	if ($BasicTrgType=~/^(DLL|EXE)/o)
	{

		my $OtherLinkerOpts;
		my $toolchain = getConfigVariable('COMPILER_PLAT');
		
		# In case of GCCE releases, '--map' option was supported only from build 3.4.3 Q1C release
		# Hence this is a special case, where GCCE platform is checked to ensure that the --map option
		# is accepted in the correct relase. Even if the option is provided in the config file, it will be
		# ignored for versions before 3.4.3 Q1C 2005 release.

		# Check to be included in the Makefile, so that the map filename is provided as input only
		# when the MAP option is supplied

		&main::Output("ifdef LINKER_SYMBOLS_MAP_OPTION\n");
		foreach (@BldList)
			{
			&main::Output("\t$_"."_MAP_FILE=\"\$(EPOCTRG$_)\\".&main::Trg($_).".map\"\n");
			}
		&main::Output("else\n");
		foreach (@BldList)
			{
			&main::Output("\t$_"."_MAP_FILE=\n");
			}
		&main::Output("endif\n");

		#	In some compiler toolchains, the start address may be required to be provided using the appropriate option.
		#	Incase if the option is provided by the toolchain, then pass we need to pass in the start address.
		&main::Output(
			"\nifdef CODE_SEGMENT_START\n",
			"\tCODE_SEGMENT_START += 0x8000 \n",
			"endif\n",
		);


		# In some compiler toolchain, the symboled archives may have to be listed
		&main::Output(
			"ifdef START_GROUP_SYMBOL\n",
			"\tEEXE_OBJECT=\$(START_GROUP_SYMBOL)UC_EXE_.o\$(END_GROUP_SYMBOL)\n",
			"\tEDLL_OBJECT=\$(START_GROUP_SYMBOL)UC_DLL_.o\$(END_GROUP_SYMBOL)\n",
			"\tDENTRY_OBJECT=\$(START_GROUP_SYMBOL)D_ENTRY_.o\$(END_GROUP_SYMBOL)\n",
			"\tLENTRY_OBJECT=\$(START_GROUP_SYMBOL)L_ENTRY_.o\$(END_GROUP_SYMBOL)\n",
			"\tXENTRY_OBJECT=\$(START_GROUP_SYMBOL)X_ENTRY_.o\$(END_GROUP_SYMBOL)\n",
			"\tVENTRY_OBJECT=\$(START_GROUP_SYMBOL)V_ENTRY_.o\$(END_GROUP_SYMBOL)\n",
			"\tKENTRY_OBJECT=\$(START_GROUP_SYMBOL)K_ENTRY_.o\$(END_GROUP_SYMBOL)\n",
			"endif\n\n"
		);
		
		# put the extra linker options from MMP file into the linker flags
		$OtherLinkerOpts=&main::LinkerOption($toolchain);
		
		if($OtherLinkerOpts) {
			&main::Output(
				"\n# ADDITIONAL LINKER OPTIONS\n",
				"ifdef SYMBIAN_UREL_LINK_FLAGS\n",
				"\tSYMBIAN_UREL_LINK_FLAGS += $OtherLinkerOpts \n",
				"else\n",
				"\tSYMBIAN_UREL_LINK_FLAGS = $OtherLinkerOpts \n",
				"endif\n",
				"ifdef SYMBIAN_UDEB_LINK_FLAGS\n",
				"\tSYMBIAN_UDEB_LINK_FLAGS += $OtherLinkerOpts \n",
				"else\n",
				"\tSYMBIAN_UDEB_LINK_FLAGS = $OtherLinkerOpts \n",
				"endif\n\n"
			);
		}
	}

	&main::Output(
		"INCDIR ="
	);

	PrintList("\"  \\\$(INCLUDE_OPTION) \$_\"", @ChopUserIncPaths);
	PrintList("\" \\\$(INCLUDE_OPTION) \$_\"", @ChopSysIncPaths);

	$ToolchainIncDir = &GetToolChainIncDir;

	if($ToolchainIncDir ne '')
	{
		&main::Output(
			" \$(INCLUDE_OPTION) ","\"$ToolchainIncDir\"");
	}

	&main::Output(
		"\n",
		"\n"
	);

	#Function call logger takes -I as include option
	my $FCLogger_inc_option = getConfigVariable('FC_LOGGER_INCLUDE_OPTION'); 
	if ($Function_Call_Logger)	{
		&main::Output(
			"INCDIR_FCLOGGER  ="
		);
		PrintList("\" $FCLogger_inc_option \$_\"", @ChopUserIncPaths);
		PrintList("\" $FCLogger_inc_option \$_\"", @ChopSysIncPaths);
		$ToolchainIncDir = &GetToolChainIncDir;
		if($ToolchainIncDir ne '')	{
			&main::Output(
				" $FCLogger_inc_option \"$ToolchainIncDir\""
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
	}

  	# Set control warnings and errors options for building Standard C++ application
  	if( StdCppTarget() ) {
  		&main::Output("CC_WARNINGS_CONTROL_OPTION=\$(CC_STDCPP_WARNINGS_CONTROL_OPTION)","\n");
  		&main::Output("CC_ERRORS_CONTROL_OPTION=\$(CC_STDCPP_ERRORS_CONTROL_OPTION)","\n");
  		&main::Output("\n",	"\n");
  	}
  	
	Read_BSF_Options() if ($plat{'CUSTOMIZES'});

	my $kernelOption=0;
	my $buildAsArmOption=0;
	my $thumbOption=0;

	if (SysTrg())
	{
		$kernelOption=1;
	}
	elsif (main::BuildAsARM() or ($ABI eq 'ARMV4'))
	{
		$buildAsArmOption=1;
	}
	else
	{
		$thumbOption=1;
	}

	my $OtherOpts = undef;	

	my $toolchain = getConfigVariable('COMPILER_PLAT');

	$OtherOpts = &main::CompilerOption($toolchain);

	
	if($kernelOption==1)
	{
		if(@kernelOptions) {
# Kernel options as read from BSF file (KERNEL_OPTIONS keyword)
			Set_BSF_Options('KERNEL_OPTIONS',\@kernelOptions);
		}
		$OtherOpts .= " \$(KERNEL_OPTIONS) ";
	}
	elsif($buildAsArmOption==1)
	{	
		if(@armOptions) {
# Arm options as read from BSF file (ARM_OPTIONS keyword)
			Set_BSF_Options('ARM_OPTIONS',\@armOptions);
		}
		$OtherOpts .= " \$(ARM_OPTIONS) ";
	}
	elsif($thumbOption==1)
	{
		if(@thumbOptions) {
# Thumb options as read from BSF file (THUMB_OPTIONS keyword)
			Set_BSF_Options('THUMB_OPTIONS',\@thumbOptions);
		}
		$OtherOpts .= " \$(THUMB_OPTIONS) ";
	}

	if($thumbOption==1 || $buildAsArmOption==1 || $kernelOption ==1 )
	{
		if (&main::ARMFPU && (&main::ARMFPU =~ /^VFPV2$/i))
		{
			$OtherOpts .=	" \$(VFP2MODE_OPTION) ";
		}
		else
		{
			$OtherOpts .=	" \$(SOFTVFPMODE_OPTION) ";
		}
	}

	if ($thumbOption==1)
	{
		$OtherOpts .= " \$(COMPILER_THUMB_DEFINES) ";
	}

	$OtherOpts .= " \$(COMPILER_INTERWORK_DEFINES) ";

	# Options to export symbols by default, for OE
	if($TrgType=~/^STDDLL$/o || $TrgType=~/^STDEXE$/o || $TrgType=~/^STDLIB$/o) {
		$OtherOpts .= " \$(OE_OPTIONS) ";
	}

	if($OtherOpts)
	{
		&main::Output(
			"OTHEROPTIONS=$OtherOpts",
			"\n",
			"CCFLAGS += \$(OTHEROPTIONS)",
			"\n"
		);
	}

	if(@invariantOptions) {
# Invariant options as read from BSF file (INVARIANT_OPTIONS keyword)
		Set_BSF_Options('INVARIANT_OPTIONS',\@invariantOptions);
	}

	if(@commonOptions) {
# Common options as read from BSF file (COMMON_OPTIONS keyword)
		Set_BSF_Options('COMMON_OPTIONS',\@commonOptions);
	}
	
	if(@linkerOptions) {
# Linker options as read from BSF file (LD_OPTIONS keyword)
		Set_BSF_Options('LD_OPTIONS',\@linkerOptions);
	}	

	if ($BasicTrgType=~/^LIB$/o) {
		if(@archiverOptions) {
# Archiver options as read from BSF file (AR_OPTIONS keyword)
		Set_BSF_Options('AR_OPTIONS',\@archiverOptions);
		}
	}		

	if(@debugFormat) {
		Set_BSF_Options('DEBUG_FORMAT',\@debugFormat);
	}	

	&main::Output(
		"CCDEFS = \$(COMPILER_DEFINES) "
	);

	PrintList("\" -D\$_\"", @MacroList);

	&main::Output(
		" \$(PLATFORM_DEFINES) "
	);

	&main::Output(

	);

	if($kernelOption==1)
	{
		&main::Output(
			"-D__KERNEL_MODE__ "
		);
	}

	if($VariantFile){
		#Function Call Logger
		if ($Function_Call_Logger) {
			#FC Logger accepts product include file without path
			my $file=&main::Path_Split('File', &main::VariantFile);
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

	if ( $RootName =~ /^ARMV5$/ ) {
		# Now we know that the platform is either ARMV5 or a platform derived from
		# ARMV5 (BSF).
	
		# See if an environment variable overrides the default DWARF version for
		# this platform.
		
		my $env_dwarf_key = "ABLD_${PlatName}_DWARF";
		my $env_dwarf_val = $ENV{$env_dwarf_key};

		if ($env_dwarf_val) {
			main::FatalError("The value of $env_dwarf_key is invalid.") unless ($env_dwarf_val =~ /^[23]$/);
			main::FatalError("We don't support DWARF-2 on ARMV7.") if ($PlatName =~ /^ARMV7/ && $env_dwarf_val == 2);

			&main::Output( "DEBUG_FORMAT=\$(DEBUG_FORMAT_DWARF$env_dwarf_val)\n\n");
		}
	}


	foreach (@BldList) {
		&main::Output(
			"CC$_ = ", $IsProxyWrapperOption ? "$ENV{ABLD_COMPWRAP} ":"", "\$(CC) "
		);

		# SYMBIAN_UDEB/UREL_CCFLAGS don't have the optimisation levels.
		if(&main::DebugSwitchUsed() ){
			if(&main::SymbolicDebugEnabled() ) {
				&main::Output(
					" \$(SYMBIAN_UDEB_CCFLAGS) "
				);
			}
			else {
				&main::Output(
					" \$(SYMBIAN_UREL_CCFLAGS) "
				);
			}
		}
		elsif (/DEB$/o) {
			&main::Output(
				" \$(SYMBIAN_UDEB_CCFLAGS) "
			);
		}
		else {
			&main::Output(
				" \$(SYMBIAN_UREL_CCFLAGS) "
			);

		}
		#Set the optimisation levels depending on whether it is a UREL or a UDEB build
		if (/DEB$/o) {
			&main::Output(
				" \$(DEBUG_OPTIMISATION) "
			);
		}
		else {
		&main::Output(
				" \$(REL_OPTIMISATION) "
			);
		}

		&main::Output(
			"\$(RUNTIME_SYMBOL_VISIBILITY_OPTION) "
		);

		if($kernelOption == 0)
		{
			&main::Output(
				"\$(EXCEPTIONS) "
			);
		}
		&main::Output(
			'$(CCFLAGS) '
		);

		my @ml = &main::MacroList($_);
		PrintList("\" -D\$_\"", @ml);

		&main::Output(
			" \$(CCDEFS)\n"
		);
		my @mmpReplaceOptions = &main::ReplaceOptions($toolchain);
		
		if (@mmpReplaceOptions)
		{
			my $OptionPrefix = getConfigVariable('OPTION_PREFIX');
			my $Pattern;

			# Set the value of '$Pattern' which is required to segregate one option from another based on the option prefix.
			if($OptionPrefix) 
			{
				$Pattern = $OptionPrefix.'\S+\s*(?!'.$OptionPrefix.')\S*';
			}
			else 
			{
				# If option prefix is not set in the configuration make file, then set default
				# option prefix as '-' or '--'.
				$Pattern = '-{1,2}\S+\s*(?!-)\S*';
			}

			foreach my $options (@mmpReplaceOptions) 
			{
				my @opts = $options =~ /$Pattern/g;
				my $count = 0;

				while ($count <= $#opts) 
				{
					my $opt;
					my $rep;
					if ($opts[$count] =~ /^(\S+)\s+(\S+)$/) 
					{
						$opt = $1;
						$rep = $2;
						&main::Output(
							"CC$_ := \$(subst $opt ,@@,\$(CC$_))\n",
							"CC$_ := \$(CC$_:@@%=$opt $rep)",							
							"\n"
						);			
						$count++;
					}
					else
					{
						$opt = $opts[$count];
						$rep = $opts[$count+1];
						
						# Substitute '=' with '%' which is a wild card character in makefile.					
						$opt =~ s/=/%/;
					
						&main::Output(
							"CC$_ := \$(CC$_:$opt=$rep)",
							"\n"
						);							
						$count+=2;
					}
				}
			}
		}
		&main::Output(
			"\n"
		);
	}

	&main::Output(		
		"\n"
	);

	#Function call logger
	if ($Function_Call_Logger)	{
		#Send all the debug macros to logger
		foreach (@BldList) {
			&main::Output
			(
				"FCLOGGER$_ = ", 
				$EPOCPath, 
				"tools\\fc_logger\\edgcpfe"
			);

			my @ml = &main::MacroList($_);
			PrintList("\" -D\$_\"", @ml);
			if ($thumbOption==1) {
				&main::Output(
					" \$(COMPILER_THUMB_DEFINES)"
				);
			}
			&main::Output(" \$(COMPILER_INTERWORK_DEFINES)",
						  " \$(CCDEFS)",
						  " \$(FC_LOGGER_DEFINES)",
						  " \$(FC_LOGGER_OPTION)");
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
				if ($PlatName =~ /^ARMV5$/) {
					&main::Output(
						" ", &Generic_Quote("\$(EPOCLIB)\\LIB\\$PrimaryExportLibrary.dso"),
						" ", &Generic_Quote("\$(EPOCLIB)\\LIB\\$PrimaryExportLibrary.lib"), "\n"
					);
				}
				else {
					&main::Output(
						" ", &Generic_Quote("\$(EPOCLIB)\\LIB\\$PrimaryExportLibrary.dso"), "\n"
					);
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
				"\t\@echo Not attempting to create \"\$(EPOCLIB)\\LIB\\$PrimaryExportLibrary.dso\"\n",
				"\t\@echo from frozen .DEF file, since EXPORTUNFROZEN specified.\n"
			);
		}

		my $theDefFile = $DefFile;
		$theDefFile = "\$(EPOCBLD)\\$BaseTrg.def" unless (-e $DefFile);
		&main::Output(
			"\n",
			"\n",
			"# REAL TARGET - LIBRARY (.dso) \n",
			"\n",
			&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.dso"), " : ",
			&Generic_Quote($DefFile), "\n",
				"\tperl -S prepdef.pl ", &Generic_Quote($DefFile), " \"\$(EPOCBLD)\\$ExportLibrary.prep.def\"\n",
				"\telf2e32 --definput=\"\$(EPOCBLD)\\$ExportLibrary.prep.def\" --dso=",
				&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.dso"),
				" --linkas=$LinkAs\n",
			"\n"
		 );
		if ($ExtraExportLibrary) {
			&main::Output(
				"\n",
				&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.dso"), " : ",
				&Generic_Quote("\$(EPOCLIB)\\LIB\\$ExportLibrary.dso"), "\n",
				"\tcopy \"\$<\" \"\$@\"\n"
			);
		}

		# Generate .lib files which will be used for ARMV5_ABIV1 platform (ABIV1 mode toolchain)
		# Only for ARMV5 platform
		if ($PlatName =~ /^ARMV5$/)
		{
			&main::Output(
				"\n",
				"\n",
				"# REAL TARGET - LIBRARY (.lib) \n",
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
#		call perl on the script here so make will die if there are errors
#           - this doesn't happen if calling perl in a batch file
		&main::Output( "\tperl -S efreeze.pl \$(EFREEZE_ALLOW_REMOVE) \"$DefFile\" \"\$(EPOCBLD)\\$ExportLibrary.def\" \n" );
	}
	else {
		&main::Output( "\tperl -e \"print \\\"warning: freeze could not be supported or \\
		         you need to declare an explicitly specified def file using the keyword \\
			 DEFFILE in the MMP file!\\n\\\"\""); 
	}
	&main::Output(
		"\n",
		"CLEANLIBRARY :\n"
	);
	if ($DefFile and !$NoExportLibrary) {
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCLIB)\\LIB\\$ExportLibrary.dso\"\n"
		);
		if ($ExtraExportLibrary) {
			&main::Output(
				"\t-\$(ERASE) \"\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.dso\"\n"
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
    my $FeatureVariantBaseTrg=&main::FeatureVariantBaseTrg;
    my $Bld=&main::Bld;
    my $ChopBldPath=&main::Path_Chop(&main::BldPath);
    my $DefFile=&main::DefFile;
    my $EPOCIncPath=&main::EPOCIncPath;
    my $FirstLib=&main::FirstLib;
    my $BasicTrgType=&main::BasicTrgType;
    my @LibList;
    my @RTLibList = &GetRTLibList();
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
    my $StatLinkPath=&main::StatLinkPath;
    my $Trg=&main::Trg;
    my $TrgType=&main::TrgType;
    my @UidList=&main::UidList;
	my %Version = &main::Version();
	my $ExtraExportLibrary;
	unless ($Version{explicit}) {
		$ExtraExportLibrary = $ExportLibrary;
		$ExtraExportLibrary =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
	}
	my $objectFiles;

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
		push @releaseables, "$RelPath$Trg.map";
	}
	if (-e $DefFile and !$NoExportLibrary) { # effectively "if project frozen ..."
		push @releaseables, "$LibPath$ExportLibrary.dso";
		push @releaseables, "$LibPath$ExtraExportLibrary.dso" if ($ExtraExportLibrary);
		if ($PlatName =~ /^ARMV5$/)	{
			push @releaseables, "$LibPath$ExportLibrary.lib";
			push @releaseables, "$LibPath$ExtraExportLibrary.lib" if ($ExtraExportLibrary);
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
	
	&Generic_WhatTargets($Bld, "WHAT$Bld", @releaseables);
	my @cleantargets = (@releaseables, "$RelPath$ExtraExportLibrary.sym");
	
	if (-e $DefFile and !$NoExportLibrary) { # effectively "if project frozen ..."
		&Generic_CleanTargets($Bld, "CLEANRELEASE$Bld", @cleantargets);
	}
	else {
		push @cleantargets, "$LibPath$ExportLibrary.dso";
			push @cleantargets, "$LibPath$ExtraExportLibrary.dso" if ($ExtraExportLibrary);
		if ($PlatName =~ /^ARMV5$/) {
            push @cleantargets, "$LibPath$ExportLibrary.lib";
            push @cleantargets, "$LibPath$ExtraExportLibrary.lib" if ($ExtraExportLibrary);
		}
			&Generic_CleanTargets($Bld, "CLEANRELEASE$Bld", @cleantargets);
	}
	
	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopBldPath);
	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopRelPath);

	return if ($BasicTrgType=~/^IMPLIB$/io);

	&main::Output(
		"LISTING$Bld : MAKEWORK$Bld"
	);
	foreach (@SrcList) {
			my $BaseSrc = &main::Path_Split('Base', $_);
			my $Ext = &main::Path_Split('Ext', $_);
		if ($Ext =~ /cia/i && ($PlatName ne "GCCE")) {
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
	#Compiler wrapper support starts
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
	#--Compiler wrapper support ends

	# Flag that tells us wheter to run checklib.exe on user-added static libraries.
	my $run_checklib = 0;

  	if (@StatLibList && $BasicTrgType =~ /^(EXE|DLL)$/o && !$SystemTrg && main::StdCppSupport()) {
		# There are user-added static libraries. They should be checked for
		# consistent use of operator new.
		$run_checklib = 1;

		#Create a make variable for the libraries.
		main::Output("\nUSER_ADDED_ARCHIVES_$Bld=");
		PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCSTATLINK$Bld\)\\\\\$_\"\)", @StatLibList);
		main::Output("\n\n");
	}

	&main::Output(
		"LIBS$Bld="
	);
        
	my @depLibs;
	if ($BasicTrgType=~/^DLL$/o) { # Add the DLL stub library
		&main::Output(
			" \\\n\t",
			&Generic_Quote("\$(EPOCSTATLINK$Bld)\\EDLLSTUB.lib")
		);
		push @depLibs, &Generic_Quote("\$(EPOCSTATLINK$Bld)\\EDLLSTUB.lib");
	}
        
	my $StaticRTLib = "";


	if ($SystemTrg) {
		$StaticRTLib = "ksrt";
	}
	else {
		$StaticRTLib = "usrt";
	}

	if ($RVCTVersion) {
		$StaticRTLib .= $RVCTVersion;
	}
	elsif ($GCCEVersion) {
		if ($GCCEMajorVersion < 4) {
			$StaticRTLib .= "2_2";
		}
		else {
			$StaticRTLib .= "3_1";
		}
	}
	else {
		$StaticRTLib .= "2_2";
	}

	&main::Output(" \\\n\$(EPOCSTATLINK$Bld)\\$StaticRTLib\.lib");

	&main::Output(" \\\n\t",getConfigVariable('VIA_FILE_PREFIX'));
	PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCBSFSTATLINK$Bld\)\\\\\$_\"\)", @StatLibList);
	&main::Output(" \\\n\t",getConfigVariable('VIA_FILE_SUFFIX'));
	
	@depLibs = (@depLibs, DepLinkList("\&Generic_Quote\(\"\\\$\(EPOCBSFSTATLINK$Bld\)\\\\\$_\"\)", @StatLibList));
	
	
	my @ImportLibList = ImportLibraryList(@LibList);
	PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCLIB\)\\\\LIB\\\\\$_\"\)", @ImportLibList);
	@depLibs = (@depLibs, DepLinkList("\&Generic_Quote\(\"\\\$\(EPOCLIB\)\\\\LIB\\\\\$_\"\)", @ImportLibList));
        
	if ($TrgType=~/^STDEXE$/o || $TrgType=~/^STDDLL$/o) {
		$NamedSymLkup = 1;
		my @OEImportLibraryList = &GetOEImportLibList();
		@OEImportLibraryList = ImportLibraryList(@OEImportLibraryList);
		if(not (grep /^libc.dso$/i, @LibList)) {
				push @OEImportLibraryList, "libc.dso";
		}
		PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\\\$\(EPOCLIB\)\\\\LIB\\\\\$_\"\)", @OEImportLibraryList);
		@depLibs = (@depLibs, DepLinkList("\&Generic_Quote\(\"\\\$\(EPOCLIB\)\\\\LIB\\\\\$_\"\)", @OEImportLibraryList));
	}

	unless ($ArmRT || ($BasicTrgType=~/^LIB$/o)) {
		my $TargLib = "$ExportLibrary.dso";
		$TargLib =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
		unless ($SystemTrg) {
			foreach (@RTLibList) {
  				# Ignore the empty file name      Modified by Kun Xu for DEF126030 on 06/08/2008
  				if(!$_) 
  				{
  					next;		
  				}
				&main::Output(
					" \\\n\t",
					&Generic_Quote("\$(EPOCLIB)\\LIB\\$_")
				) unless ($_ =~ /$TargLib/i);
				push @depLibs, &Generic_Quote("\$(EPOCLIB)\\LIB\\$_") unless ($_ =~ /$TargLib/i);
			}
		}
	}
    #OE Glue Code 
	if ($TrgType=~/^STDEXE$/o) {
		my @OELibList = &GetOELibList();
		foreach (@OELibList) {
			&main::Output(
				" \\\n\t",
				&Generic_Quote("\$(EPOCSTATLINK$Bld)\\$_"),
			);
			push @depLibs, &Generic_Quote("\$(EPOCSTATLINK$Bld)\\$_");
		}
	}
	PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\$_\"\)", @ToolChainLibList);
                
	&main::Output(
		"\n",
		"\n"
	);
        
	main::Output(
                "LIBS$Bld" . "DEPS="
	);
        
	my @tmp;
	for my $lib (@depLibs)
	{
            $lib =~ s/\(.*\.o\)//;
            $lib =~ s/\\$//;
            push @tmp, $lib unless ($lib =~ /-\(/ || $lib =~ /-\)/);
	}
                
	PrintList("\' \\\n\t\'\.\&Generic_Quote\(\"\$_\"\)", @tmp);
        
	&main::Output(
			"\n",
			"\n"
	);

	&main::Output(
		"VTBLEXPORTS$Bld="
	);
        my $vtobj = quotemeta("(VtblExports.o)");
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
		if ($Ext =~ /cia/i && ($PlatName ne "GCCE")) {
			$BaseSrc = "$BaseSrc\_";
		}

			&main::Output(
				" \\\n\t",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o")
			  );

			$objectFiles .= &main::BldPath.$BaseSrc.".o ";
	}
        &main::Output(
			"\n",
		"\n"
	);

	# IsCustomDllUseCase() subroutine is called to check if the given executable 
	# is a custom dll or not.
	my $IsCustomDll = IsCustomDllUseCase();

	if ($IsCustomDll) 
	{
		# Generate ".bin" file(assembly listing file) only if frozen deffile is present and EXPORTUNFROZEN is not specified
		# in the Mmp file.
		if((-e $DefFile) && (!&main::ExportUnfrozen))
		{
			&main::Output(
				"EXPFILE$Bld = \$(EPOCBLD$Bld)\\$ExportLibrary.bin \n",			
				"\n"
			);
		}
		else 
		{
			&main::Output(
				"EXPFILE$Bld = \n",
				"\n"
			);
		}		
	}
	
	# Create "via" file containing all object files in order to reduce
	# command line lengths in pertinent calls
	my $objectsViaFile = &main::CommandFile();
	my $viaoption = getConfigVariable('COMMANDFILE_OPTION');
	
    if ($BasicTrgType=~/^LIB$/o && StdCppTarget() ) {

        # Add the magic object that identifies the library as a STDLIB.
        $objectFiles .= "$ENV{EPOCROOT}epoc32/tools/tag/tag_elf";
	}

	$objectFiles =~ s/\\/\//g ;

	&main::CreateExtraFile($objectsViaFile, $objectFiles);
		 
		   &main::Output(
			 &Generic_Quote("\$(EPOCTRG$Bld)\\$Trg"),
			 " : \$(OBJECTS$Bld) \$(LIBS". $Bld . "DEPS)"
            );

	if (-e $DefFile) { # effectively "if project frozen ..."
		&main::Output(
			" ", &Generic_Quote($DefFile)
		);
	}

	if($IsCustomDll eq 1)
	{
		&main::Output( " \$(EXPFILE$Bld) " );	
	}

    &main::Output("\n");


#       get rid of any -symbols produced .map file
        if ($BasicTrgType=~/^(DLL|EXE)/o) {
			&main::Output(
			"\t-\$(ERASE) \"\$(EPOCTRG$Bld)\\$Trg.map\" \n"
		);
		}

		my $AbsentSubst = '';
		if ($BasicTrgType=~/^(DLL|EXE)/o) {

			if ($run_checklib) {
				my $checklib = "checklib";

				if ( StdCppTarget() ) {
					$checklib .= " stdc++ --elf";
				}
				else {
					$checklib .= " symc++ --elf";
				}

				&main::Output( "\t$checklib ","\$(USER_ADDED_ARCHIVES_$Bld)\n" );
			}

			my $WrapLD = $IsProxyWrapperOption ? "$ENV{ABLD_COMPWRAP} " : "";
			if ($Bld =~ /DEB/)
			{
				if($PlatName ne "GCCE"){
					# if the compiler is RVCT then append SYMBIAN_UDEB_LINK_FLAGS to option string 
					# in order to override the default ones. 
					&main::Output(
						"\t", $WrapLD, "\$(LD) ","\$(STATIC_LIBS) ", "\$(SHARED_OBJECT_OPTION) ",
						"\$(CODE_SEGMENT_START) ", "\$(DATA_SEGMENT_START) 0x400000 ", "\$(SYMVER_OPTION) ", "\$(SYMBIAN_UDEB_LINK_FLAGS) ", "\$(SO_NAME_OPTION) ","$LinkAs"
					); 
				} else {
					&main::Output(
						"\t", $WrapLD, "\$(LD) ","\$(STATIC_LIBS) ","\$(SYMBIAN_UDEB_LINK_FLAGS) ", "\$(SHARED_OBJECT_OPTION) ",
						"\$(CODE_SEGMENT_START) ", "\$(DATA_SEGMENT_START) 0x400000 ", "\$(SYMVER_OPTION) ","\$(SO_NAME_OPTION) ","$LinkAs"
					);
				}
			}
			else
			{
				if($PlatName ne "GCCE"){
					# if the compiler is RVCT then append SYMBIAN_UREL_LINK_FLAGS to option string 
					# in order to override the default ones. 
					&main::Output(
						"\t", $WrapLD, "\$(LD) ","\$(STATIC_LIBS) ","\$(SHARED_OBJECT_OPTION) ",
						"\$(CODE_SEGMENT_START) ", "\$(DATA_SEGMENT_START) 0x400000 ", "\$(SYMVER_OPTION) ", "\$(SYMBIAN_UREL_LINK_FLAGS) ", "\$(SO_NAME_OPTION) ", "$LinkAs"
					);
				} else {
					&main::Output(
						"\t", $WrapLD, "\$(LD) ","\$(STATIC_LIBS) ","\$(SYMBIAN_UREL_LINK_FLAGS) ","\$(SHARED_OBJECT_OPTION) ",
						"\$(CODE_SEGMENT_START) ", "\$(DATA_SEGMENT_START) 0x400000 ", "\$(SYMVER_OPTION) ","\$(SO_NAME_OPTION) ","$LinkAs"
					);
				}
			}

			if(&main::DebugSwitchUsed()){
				if(&main::SymbolicDebugEnabled() ) {
					&main::Output(
						" \$(LINKER_DEBUG_OPTION) "
					);
				}
				else {
					&main::Output(
						" \$(LINKER_NODEBUG_OPTION) "
					);
				}
			}
			elsif($Bld =~/DEB/){
				&main::Output(
					" \$(LINKER_DEBUG_OPTION) "
				);
			}
			else {
				&main::Output(
					" \$(LINKER_NODEBUG_OPTION) "
				);
			}

			my $EntrySymbol;
			if ($BasicTrgType=~/^DLL$/o) {
				$EntrySymbol = '_E32Dll';
			}
			elsif ($BasicTrgType=~/^EXE$/o) {
				$EntrySymbol = '_E32Startup';
			}
			if ($EntrySymbol) {
				$AbsentSubst = " --absent $EntrySymbol";
			}

			if (($BasicTrgType=~/^DLL$/o) || ($BasicTrgType=~/^EXE$/o)) {
				&main::Output(
					" \$(LINKER_ENTRY_OPTION)",
					" $EntrySymbol "
					);
				my $undefined_option = getConfigVariable('UNDEFINED_SYMBOL_REF_OPTION');
				if ($undefined_option)
				{
					&main::Output(
						" \$(UNDEFINED_SYMBOL_REF_OPTION)",
						" $EntrySymbol "
					);
				}
			}

			if ($BasicTrgType=~/^DLL$/o) {
				# get the right object file for the entry point
				my $ObjFile="\$(EDLL_OBJECT)";
				if ($FirstLib =~ /EDEV/i) {
					$ObjFile="\$(DENTRY_OBJECT)";
				}
				if ($FirstLib =~ /EKLL/i) {
					$ObjFile="\$(LENTRY_OBJECT)";
				}
				if ($FirstLib =~ /EEXT/i) {
					$ObjFile="\$(XENTRY_OBJECT)";
				}
				if ($FirstLib =~ /EVAR/i) {
					$ObjFile="\$(VENTRY_OBJECT)";
				}

				&main::Output(
					" \$(EPOCSTATLINK$Bld)\\$FirstLib", "$ObjFile",
					" \$(EPOCSTATLINK$Bld)\\$FirstLib", " \\", "\n"
				);
			} elsif ($BasicTrgType=~/^EXE$/o || $TrgType=~/^EXEXP$/o) {
				# get the right object file for the entry point
				my $ObjFile="\$(EEXE_OBJECT)";
				if ($FirstLib =~ /KC_EXE/i) {
					$ObjFile="\$(KENTRY_OBJECT)";
				}

				&main::Output(
					" \$(EPOCSTATLINK$Bld)\\$FirstLib", "$ObjFile",
					" \$(EPOCSTATLINK$Bld)\\$FirstLib", " \\", "\n"
				);
			}

			&main::Output(
				"\t\t\$(LINKER_OUTPUT_OPTION) \"\$(EPOCBLD$Bld)\\$Trg\" \\\n",
#				"\t\t\$(OBJECTS$Bld) \\\n"
			);

		&main::Output(
			"\t\t\$(LINKER_SYMBOLS_MAP_OPTION) ",
			"\$(","$Bld","_MAP_FILE) \\\n"
		);

		# Pass in the command file if the command file option is passed in
		if($viaoption) {
                        #'@' is for GCCE whos version is not 3.4.3
			if($viaoption eq '@'){
				main::Output(
					"\t\t$viaoption$objectsViaFile \\\n"
				);
			}
                        #'-via' is for RVCT 
			else{
				main::Output(
					"\t\t$viaoption $objectsViaFile \\\n"
				);
			}
		}
		else {
			&main::Output(
				"\t\t\$(OBJECTS$Bld) \\\n"
			);
		}

		if($IsCustomDll eq 1) {
			&main::Output( "\t\t\$(EXPFILE$Bld) \\\n" );
		}

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
			elsif ($Bld=~/^U?DEB$/o) {
				   &main::Output(
				   "\tcopy \"\$(EPOCBLD$Bld)\\$Trg\" \"\$(EPOCTRG$Bld)\\$FeatureVariantBaseTrg.sym\"\n"
			   );
			}
		if (-e $DefFile) {
			&main::Output(
				"\tcopy ", &Generic_Quote(("\"$DefFile\"")), " \"\$(EPOCBLD)\\$ExportLibrary.prep.def\"\n",
			);
		}

		&main::Output(
			"\telf2e32 --sid=", &main::SecureId(), " "
		);

		if (&main::IsDebuggable eq DEBUGGABLE) {
			&main::Output(
				' --debuggable '
			);
		}

		if (&main::IsDebuggable eq DEBUGGABLE_UDEBONLY) {
			if ($Bld=~/^UDEB$/o) {
				&main::Output(
				' --debuggable '
				);
			}
		}
		
		# Do not export Arm Static Library Symbols
		if (&Armutl_ArmLibList) {
			&main::Output(
				' --excludeunwantedexports'
			);
		}

		if ($IsCustomDll eq 1) {
			&main::Output(
				' --customdlltarget'
			);
		}
		if (keys(%Version)) {
			&main::Output(
				' --version=', &Genutl_VersionToUserString(%Version)
			);
		}
		# exexps are allowed data, but they look like dlls to elftran....
		if (&main::AllowDllData || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			&main::Output(
				' --dlldata'
			);
		}
		if (&main::DataLinkAddress) {
			&main::Output(
				' --datalinkaddress=',&main::DataLinkAddress
			);
		}
		if (&main::FixedProcess) {
			&main::Output(
				' --fixedaddress'
			);
		}
		if (&main::HeapSize) {
			my %HeapSize=&main::HeapSize;
			&main::Output(
				' --heap=',$HeapSize{Min},',',$HeapSize{Max}
			);
		}
		if (&main::ProcessPriority) {
			&main::Output(
				' --priority=',&main::ProcessPriority
			);
		}
		if (&main::StackSize) {
			&main::Output(
				' --stack=',&main::StackSize
			);
		}
		&main::Output(
			"\\\n\t\t"
		);

		my $i=1;
		foreach (@UidList) {
			&main::Output(
				" --uid$i=$_"
			);
			$i++;
		}
		if(&main::VendorId) {
			&main::Output(
				' --vid=',&main::VendorId
			);
		}
		&main::Output(
			"\\\n\t\t"
		);

		&main::Output(
			' --capability=',&main::Capability
		);

		my $vfpv2_support = getConfigVariable('VFP2MODE_OPTION');
		if ($vfpv2_support && &main::ARMFPU && (&main::ARMFPU =~ /^VFPV2$/i))
		{
			&main::Output(
				' --fpu=vfpv2'
			);
		}
		else
		{
			&main::Output(
				' --fpu=softvfp'
			);
		}

		if (&main::SmpSafe)
		{
			&main::Output(
				' --smpsafe'
			);
		}


		if(($BasicTrgType=~/^DLL/ && $TrgType!~/^DLL/ ) || $TrgType=~/^EXEXP/ || $TrgType=~/^STDEXE/){
			&main::Output(
				' --targettype=',$TrgType
				);
		}
		else{
			&main::Output(
				' --targettype=',$BasicTrgType
				);
		}

		&main::Output(
			' --output=',"\"\$\@\""
			);
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			if (-e $DefFile) {
				&main::Output(
					' --definput=',"\"\$(EPOCBLD)\\$ExportLibrary.prep.def\""
					);
			}
			# Non-callable exports will be ignored if the MMP file does not contain the DEFFILE keyword or contains the NOEXPORTLIBRARY keyword			 
			if (!($DefFile) || $NoExportLibrary) {				
				&main::Output( " --ignorenoncallable" );
			}
			&main::Output(
				' --dso=',
				&Generic_Quote("\$(EPOCBLD$Bld)\\$ExportLibrary.dso")
				);
			&main::Output(
				' --defoutput=',
				&Generic_Quote("\$(EPOCBLD$Bld)\\$ExportLibrary.def")
				);
				if(&main::ExportUnfrozen) {
					&main::Output( ' --unfrozen ');
				}
			}
		#the input elf file - as the last arg
		&main::Output(
			" --elfinput=","\"\$(EPOCBLD$Bld)\\$Trg\"",
			" --linkas=$LinkAs"
			);
		if (&main::CompressTarget)
			{
			&main::Output(
			" --uncompressed"
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
				" --compressionmethod inflate"
			);
			}
			elsif(&main::CompressTargetMode == BYTEPAIRCOMPRESSIONMETHOD)
			{
			&main::Output(
				" --compressionmethod bytepair"
			);
			}

			}
		
		if($NamedSymLkup){
			&main::Output(" --namedlookup"
		);
		}

		&main::Output(
		" --libpath=", "\"\$(EPOCLIB)\\LIB\""
		);

		if($BasicTrgType=~/^DLL/ && $TrgType!~/^DLL/){
			my $Export;
			my $Ordinal=1;
			foreach $Export (&main::Exports) {
				if($Ordinal == 1) {
				&main::Output(" --sysdef=");
				}
				&main::Output(
					"$Export,$Ordinal; "
				);
				$Ordinal++;
			}
		}

		if (&main::CodePagingTargetMode == UNPAGED) {
			&main::Output(
				' --codepaging=unpaged'
			);
		}
		elsif (&main::CodePagingTargetMode == PAGED) {
			&main::Output(
				' --codepaging=paged'
			);
		}

		if (&main::DataPagingTargetMode == UNPAGED) {
			&main::Output(
				' --datapaging=unpaged'
			);
		}
		elsif (&main::DataPagingTargetMode == PAGED) {
			&main::Output(
				' --datapaging=paged'
			);
		}

		&main::Output(
			"\n"
		);

		}
         elsif ($BasicTrgType=~/^LIB$/o) {
			&main::Output(
			"\t\$(AR) ",
				" \$(ARCHIVER_CREATE_OPTION) ",
				" \$(EPOCBSFSTATLINK$Bld)\\$Trg \\\n"
			);
		# Pass in the command file if the command file option is passed in
		if($viaoption) {
                        #'@' is for GCCE whos version is not 3.4.3
			if($viaoption eq '@'){
				&main::Output(
					"\t\t$viaoption$objectsViaFile\n"
				);
			}
                        #'-via' is for RVCT  
			else{
				&main::Output(
					"\t\t$viaoption $objectsViaFile\n"
				);
			}
		}
		else {
			&main::Output(
				"\t\t\$(OBJECTS$Bld) \n"
			);
		}
		# Pass the archiver options which can be customized form BSF file
		&main::Output(
			"\t\t\$(AR_OPTIONS) \n"
		);
         }

         &main::Output(
		 "\n"
	 );

	if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
		&main::Output(
			"\tcopy ", " \"\$(EPOCBLD$Bld)\\$ExportLibrary.def\" ", "\"\$(EPOCBLD)\\$ExportLibrary.def\"\n"
			);
		if  (&main::ExportUnfrozen) {
			if ($PlatName =~ /^ARMV5$/)
			{
				&main::Output(
				"\tdef2dll.bat --path=\$(EPOCBLD$Bld) \\\n\t\t--bldpath=\$(EPOCBLD$Bld) \\\n\t\t--export=$ExportLibrary \\\n\t\t--import=$ExportLibrary \\\n",
				"\t\t--deffile=\$(EPOCBLD)\\$ExportLibrary.def \\\n\t\t--linkAs=$LinkAs \\\n\t\t$InterWorking \\\n\t\t--absent=undef \n"
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
					"\tcopy ", " \"\$(EPOCLIB)\\LIB\\$ExportLibrary.lib\" ",
					"\"\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.lib\"",
					"\n"
				);
				}
			}

			&main::Output(
					  "\n",
				  "\tcopy ", " \"\$(EPOCBLD$Bld)\\$ExportLibrary.dso\" ",
				  "\"\$(EPOCLIB)\\LIB\\$ExportLibrary.dso\"",
				  "\n"
				 );

			&main::Output(
					  "\n",
				  "\tcopy ", " \"\$(EPOCBLD$Bld)\\$ExportLibrary.dso\" ",
				  "\"\$(EPOCLIB)\\LIB\\$ExtraExportLibrary.dso\"",
				  "\n"
				 ) if ($ExtraExportLibrary);

		}
	}

	if($IsCustomDll eq 1)
	{
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$ExportLibrary.bin"), ": $DefFile\n");

	    my $theDefFile = "\$(EPOCBLD)\\$ExportLibrary.def";
	    $theDefFile = $DefFile if (-e $DefFile && !&main::ExportUnfrozen);
	    my $theAssembler = " \$(ASM) ";
	    &main::Output(
		"\telf2e32  \\\n\t\t--definput=$theDefFile \\\n\t\t--dump=a \\\n\t\t--output=\$(EPOCBLD$Bld)\\$ExportLibrary.s \n",
  		"\t$theAssembler \$(AAPCS_OPTION) \\\n\t\t \$(ASM_OUTPUT_OPTION) \$(EPOCBLD$Bld)\\$ExportLibrary.bin  \$(EPOCBLD$Bld)\\$ExportLibrary.s\n\n"
	    );
		&main::Output(
			"\t-\$(ERASE) \"\$(EPOCBLD$Bld)\\$ExportLibrary.s\"\n\n"
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
				$base .= "_" if $cia && ($PlatName ne "GCCE");
				push @FileList, $File;
				push @ObjectList, "$base.o";
#				this gives us our source files xxx
				push @SourceList, ($cia && ($PlatName ne "GCCE")) ? "$base.cpp" : "$SrcPath$base$Ext";
				$NumToGo--;
				unless ($NumToGo) {
#					   Use the last file as the key. This means e.g that all the dependency
#					   info will have been generated for the earlier files in the list
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
		$BaseObj .= '_' if ($PlatName ne "GCCE");
	}

  	foreach my $Bld (@BldList) {
		my $tranasm = getConfigVariable('TRANASM');
		if ($tranasm)
		{
			&main::Output(
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), " ",
			) if $cia && ($PlatName ne "GCCE");
		}

		&main::Output(
  			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseObj.cpp"), " ",
		) if $cia && ($PlatName ne "GCCE");
		&main::Output(
  			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " ",
  			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseObj.o"), " \\\n",
		);
	}
	&main::Output(
		":"
	);

	my $prefix_file=getConfigVariable('PREFIXFILE');

	my @tempdeplist = &main::DepList;

	my $totalcount = $#tempdeplist;
	my $count = 0;
	while( $totalcount >= $count )
	{
		my $dependencyfile = shift(@tempdeplist);
		if($dependencyfile eq $prefix_file )
		{
			$DepList[$count]="\$(PREFIXFILE)";
		}
		$count += 1;
	}

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
		$BaseObj .= '_' if ($PlatName ne "GCCE");
	}

	my $tranasm = getConfigVariable('TRANASM');
	if ($tranasm)
	{
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), " ",
		) if $cia && ($PlatName ne "GCCE");
	}

	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseObj.cpp"), " ",
	) if $cia && ($PlatName ne "GCCE");

	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " ",
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseObj.o"), " :",
	);

	my $prefix_file=getConfigVariable('PREFIXFILE');

	my @tempdeplist = &main::DepList;

	my $totalcount = $#tempdeplist;
	my $count = 0;
	while( $totalcount >= $count )
	{
		my $dependencyfile = shift(@tempdeplist);
		if($dependencyfile eq $prefix_file )
		{
			$DepList[$count]="\$(PREFIXFILE)";
		}
		$count += 1;
	}

        PrintList("\' \\\n\t\'\.\&Generic_Quote\(\$_\)", @DepList);
	&main::Output(
		"\n",
		"\n"
	);
}

sub quoted_path
{
    my $curdrive="";
    my ($arg) = @_;
    return "\"$arg\"" if ($arg !~ /^\\[^\\]/);	# not an absolute path

		$curdrive=$1 if (cwd =~ /^(.:)/);
    return "\"$curdrive$arg\"";
}

sub PMPrefixFile
{
# Prefix Header File passed to the preprocessor

	my $prefix_file=getConfigVariable('PREFIXFILE');
	return quoted_path($prefix_file) if defined $prefix_file;
}

sub PMToolChainIncDir
	{
	# Extra system include directories dictated by the toolchain
	return $ToolChainIncDir;
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
	my $SrcPath=&main::SrcPath;
	my $BaseTrg=&main::BaseTrg;
	my $BldPath = &main::BldPath;
	my $Ext = &main::Path_Split('Ext', $Src);
	$Src = ucfirst $Src if ($Ext !~ /\.(cpp|c)$/);
	my $LangOptions = &SelectLangOptions($Ext);
	# support for auto 'translated' ASM
	my $AsmFilep = $AsmFiles{$Src};

	$preinclude = &GetToolChainPreInclude();
	
	#Function call logger takes -I as include option
	my $FCLogger_inc_option = getConfigVariable('FC_LOGGER_INCLUDE_OPTION'); 
	my $logger_preinclude = getConfigVariable('PREINCLUDE_OPTION_FCLOGGER'); 
	my $Dictionary_File_Name = getConfigVariable('FC_LOGGER_DICTIONARY_FILE_NAME');
	my $Generated_C_File_Name = getConfigVariable('FC_LOGGER_GENERATED_C_FILE_NAME');
	
	my $ChopSrcPath=&main::Path_Chop($SrcPath);
	my $lfboption = LinkerFeedBackOption();
	my $LstExt ;
	if($Plat =~ /^(ARMV[6-9])/i){
		$LstExt = $1 ;	
	}
	else{
		$LstExt = $ABI;
	}
	if ($AsmFilep || ($Ext =~ /cia/i && ($PlatName ne "GCCE"))) {
		&main::Output(
# compile the translated, preprocessed source
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.o"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), "\n",
			"\t\@echo $Src\n",
			"\t\$(CC$Bld) $lfboption $LangOptions ", "\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR) \$(OUTPUT_OPTION) \$\@ \$(EPOCBLD$Bld)\\$BaseSrc\_.cpp)\n",
			"\n"
			);
# rule to translate the preprocessed source
			my $tranasm = getConfigVariable('TRANASM');
			if ($tranasm)
			{
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), "\n",

				"\t\$(TRANASM) \$(TRANASM_FLAGS) \$(TRANASM_OUTPUT_OPTION)\$\@ \$(TRANASM_INPUT_OPTION)\$(EPOCBLD$Bld)\\$BaseSrc.pre\n",
			"\n"
			);
# rule to preprocess the source
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(CC$Bld) \$(PREPROCESSOR_OPTION) $preinclude $LangOptions ","\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR) $ChopSrcPath\\$Src \$(OUTPUT_OPTION) \$\@) \n",
# generate an assembly listing target too
			"LISTING$Bld$BaseSrc\_ : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc\_.$LstExt.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), "\n",
			"\t\$(CC$Bld) $LangOptions ", "\$(ASSEMBLER_LISTING_OPTION) ","\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR) \$(OUTPUT_OPTION) \$\@ \$(EPOCBLD$Bld)\\$BaseSrc\_.cpp)\n",
			"\n"
			);
			}
			else
			{
			&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(CC$Bld) \$(PREPROCESSOR_OPTION) $preinclude $LangOptions ","\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR) $ChopSrcPath\\$Src \$(OUTPUT_OPTION) \$\@) \n",
# generate an assembly listing target too
			"LISTING$Bld$BaseSrc\_ : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc\_.$LstExt.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), "\n",
			"\t\$(CC$Bld) $LangOptions ", "\$(ASSEMBLER_LISTING_OPTION) ","\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR) \$(OUTPUT_OPTION) \$\@ \$(EPOCBLD$Bld)\\$BaseSrc\_.cpp)\n",
			"\n"
			);
			}
	} 
	else {
		#If Function call logging is enabled, add call to function call logger
		if ($Function_Call_Logger)	{
			&main::Output(
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"), 
				" : ",
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.int.cpp"), 
				"\n",
				"\t\@echo $BaseSrc.int.cpp\n",
				"\t\$(CC$Bld) $lfboption$LangOptions ", 
				" \$(INCLUDE_OPTION) ",
				"\$(call absolutePaths, $ChopSrcPath \$(INCDIR) \$(OUTPUT_OPTION) \$\@ \$(EPOCBLD$Bld)\\$BaseSrc.int.cpp)\n",
				"\n",
# generate an assembly listing target too
				"LISTING$Bld$BaseSrc : ", 
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), 
				"\n",
				"\t",
				&Generic_CopyAction("$SrcPath$BaseSrc.$LstExt.lst"),
				"\n",
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), 
				" : ",
				&Generic_Quote("$SrcPath$Src"), 
				"\n",
				"\t\$(CC$Bld) $LangOptions ",
				"\$(ASSEMBLER_LISTING_OPTION) ",
				"\$(INCLUDE_OPTION)",
				" \$(call absolutePaths,$ChopSrcPath \$(INCDIR) \$(OUTPUT_OPTION) \$\@ $ChopSrcPath\\$Src) \n",
				"\n"
			);
			
			#Call to Function Call Logger
			&main::Output(
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.int.cpp"),
				" : ",
				&Generic_Quote("$SrcPath\\$Src"),
				"\n",
				"\t\@echo $BaseSrc.int.cpp\n",
				"\t\$(FCLOGGER$Bld) $lfboption \\\n" ,
				"\t$FCLogger_inc_option $ChopSrcPath \\\n",
				"\t$logger_preinclude \\\n ",
				"\t\$(INCDIR_FCLOGGER) \\\n" ,
				"\t$Dictionary_File_Name $BldPath$BaseTrg.txt \\\n" ,
				"\t$Generated_C_File_Name \$\@ $ChopSrcPath\\$Src \n",
				"\n\n"
			);
		}
		else {
			&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.o"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\@echo $Src\n",
			"\t\$(CC$Bld) $lfboption $LangOptions ", "\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR) \$(OUTPUT_OPTION) \$\@ $ChopSrcPath\\$Src)\n",
			"\n",
# generate an assembly listing target too
			"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc.$LstExt.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(CC$Bld) $LangOptions ","\$(ASSEMBLER_LISTING_OPTION) ","\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR) \$(OUTPUT_OPTION) \$\@ $ChopSrcPath\\$Src) \n",
			"\n"
			);
			# Compiler wrapper support starts...
			if($IsCompilerWrapperOption)
			{
				&main::Output(
					"COMPWRAP$Bld$BaseSrc : ",
					&Generic_Quote("$SrcPath$Src"), "\n",
					"\t\@echo Analysing $Src\n",
					"\t\$(COMPWRAP) \$(CC$Bld) $lfboption $LangOptions ", "\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR) \$(OUTPUT_OPTION) \$\@ $ChopSrcPath\\$Src)\n",
					"\n"
				);
			}
			# Compiler wrapper support ends...
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
	my $SrcPath=&main::SrcPath;
	my $Ext = &main::Path_Split('Ext', $Src);
	my $LangOptions = &SelectLangOptions($Ext);
	# support for auto 'translated' ASM
	my $AsmFilep = $AsmFiles{$Src};

	my $ChopSrcPath=&main::Path_Chop($SrcPath);
	my $lfboption = LinkerFeedBackOption();

	if ($AsmFilep || ($Ext =~ /cia/i && ($PlatName ne "GCCE"))) {
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
					 "\t\$(CC$Bld) ","\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR)) $lfboption\\\n",
					 "\t\t$LangOptions \$(OUTPUT_OPTION) \$\@ --multifile \$(SOURCES$MFVarN)"
			  );
				   &main::Output( "\n\n");
			   $MFVarN++;
		}
		&main::Output(
# rule to translate the preprocessed source
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), "\n",
			"\ttranasm -n -s -o=\$\@ \$(EPOCBLD$Bld)\\$BaseSrc.pre\n",
			"\n",
# rule to preprocess the source
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.pre"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(CC$Bld) \$(PREPROCESSOR_OPTION) $preinclude $LangOptions ","\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR)) $ChopSrcPath\\$Src \$(OUTPUT_OPTION) \$\@ \n",
# generate an assembly listing target too
			"LISTING$Bld$BaseSrc\_ : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc\_.$LstExt.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.lis"), " : ",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.cpp"), "\n",
			"\t\$(CC$Bld) $LangOptions ", "\$(ASSEMBLER_LISTING_OPTION) ","\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR)) \$(OUTPUT_OPTION) \$\@ \$(EPOCBLD$Bld)\\$BaseSrc\_.cpp\n",
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
					 "\t\$(CC$Bld) ", "\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR)) $lfboption\\\n",
					 "\t\t$LangOptions \$(OUTPUT_OPTION) \$\@ --multifile \$(SOURCES$MFVarN)"
			  );
				   &main::Output( "\n\n");
			   $MFVarN++;
		}
#		generate an assembly listing target too
		&main::Output(
			"LISTING$Bld$BaseSrc : ", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), "\n",
			"\t", &Generic_CopyAction("$SrcPath$BaseSrc.$LstExt.lst"),
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.lis"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(CC$Bld) $LangOptions ","\$(ASSEMBLER_LISTING_OPTION) ","\$(INCLUDE_OPTION)"," \$(call absolutePaths,$ChopSrcPath \$(INCDIR)) \$(OUTPUT_OPTION) \$\@ $ChopSrcPath\\$Src \n",
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
# modified start: makefile improvement 
	&checkVMAPFiles;
# modified end: makefile improvement 
}

sub ImportLibraryList {
	my $dso;
	my $base;
	my @ImportLibList;
	foreach $dso (@_) {
		$base = &main::Path_Split('Base', $dso);
		$dso = $base.".dso";
		push @ImportLibList, $dso;
	}
	return @ImportLibList;
}

sub ChangeSlash($) {
	my $abspath = $_[0];
	$abspath =~ s/\\/\//g ;
	return $abspath;
}

# System libraries such as the standard C++ libraries or floating point libraries or any other
# static libraries are made into DLLs by linking its export table against them. Such DLLs that are
# created from a static library is known as a Custom DLL. The ARM libraries can be supplied using
# the ARMLIBS mmp keyword  A DLL is considered to be a customdll based on the presence of ARMLIBS
# keyword and also the presence of DEFFILE keyword.

sub IsCustomDllUseCase()
{
	# To check if ARMLIBS keyword is passed in the MMP file.
	# Armutl_ArmLibList() gets the list of static arm libraries, if used by an executable.
	my $armliblist = &Armutl_ArmLibList;

	# To check if DEFFILE keyword is used in the MMP file to supply the def file.
	my $deffile = &main::DefFile;

	if ($armliblist && $deffile)
	{
		return 1;
	}
	return 0;
	
}

sub GetToolChainPreInclude {
	my $preinclude_file = getConfigVariable('PREINCLUDE_OPTION');

	$preinclude_file =~ s/\\/\//g;
	return "$preinclude_file";
}

sub SelectLangOptions {
	my ($Ext) = @_;
	$preinclude = &GetToolChainPreInclude;
	if ($Ext=~/^.cpp$/ || ($Ext=~/^.cia$/ && ($PlatName eq "GCCE"))) {
		#Function Call Logger
		return " \$(CPP_LANG_OPTION) " if ($Function_Call_Logger);
		return " \$(CPP_LANG_OPTION) $preinclude ";
	}
	if ($Ext=~/^.cia$/) {
		return " \$(CIA_LANG_OPTION) ";
	}
	if ($Ext=~/^.c$/) {
		my $CompilerOption = &main::CompilerOption("ARMCC");
		if($CompilerOption =~ /--cpp/){
			#Function Call Logger
			return " \$(CPP_LANG_OPTION) " if ($Function_Call_Logger);
			return " \$(CPP_LANG_OPTION) $preinclude ";
		}
		else {
			#Function Call Logger
			return " \$(C_LANG_OPTION) " if ($Function_Call_Logger);
			return " \$(C_LANG_OPTION) $preinclude ";
		}
	}
	# To support .cc, .cxx, .c++ file extensions for Open Environment
	elsif ($Ext=~/^(.cc|.cxx|.c\+\+)$/) {
		#Function Call Logger
		return " \$(CPP_LANG_OPTION) " if ($Function_Call_Logger);
		return " \$(CPP_LANG_OPTION) $preinclude ";
	}
	return '';
}

sub IsMapOptAvail() {
	open PIPE, "arm-none-symbianelf-g++ -v 2>&1 | ";
	while(<PIPE>){

		if($_ =~ /gcc version (.*) \(release\).*/i)
		{
			if($1 lt "3.4.3")
			{
				return 0;
			}
			elsif($_ =~ /gcc version (.*) \(release\) \(CodeSourcery ARM (.*) (\d*)\)/i)
			{
				if($1 eq "3.4.3" && uc $2 ne "Q1B" && $3 ge 2005)
				{
					return 1;
				}
				elsif($1 gt "3.4.3")
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}

		}

	}
	close PIPE;
}

sub StdCppTarget() {
    
    # STDCPP is supported
    return 0 if (! main::StdCppSupport());
    
  	if ( main::NoStdCpp() ) { #MMP keyword NOSTDCPP set
  	       return 0;
  	}	       
	
	if ( main::StdCpp() ) { # MMP keyword STDCPP set.
		return 1;
	}
	else {
		return ( main::TrgType() =~ /^(STDEXE|STDDLL|STDLIB)$/io );
	}
}

sub GetRTLibList() {

	my $newlib = main::NewLib(); # Could have been set in the MMP file.

	unless ($newlib) {
		if ( StdCppTarget() ) {
			$newlib = getConfigVariable('OE_NEW_LIB');
		}
		else {
			$newlib = getConfigVariable('SYM_NEW_LIB');
		}
	}

	my @RtLib;
	my $list = "$newlib " . getConfigVariable('RUNTIME_LIBS_LIST') ;
	if (length($list) >0)
	{
		@RtLib = split(/\s+/, $list);
	}
	return @RtLib;
}

sub GetOELibList() {
	my @OELib;
	my $list;
	if (&main::IsWideCharMain()) {
		$list = getConfigVariable('OE_EXE_LIBS_WCHAR');
	}
	else {
		$list = getConfigVariable('OE_EXE_LIBS');
	}

	if (length($list) >0)
	{
		@OELib = split(/\s+/, $list);
	}
	return @OELib;
}

sub GetOEImportLibList() {
	my @OEImportLib;
	my $list;
	$list = getConfigVariable('OE_IMPORT_LIBS');
	
	if (length($list) >0)
	{
		@OEImportLib = split(/\s+/, $list);
	}
	return @OEImportLib;
}

sub InitToolChain(@) {

	if($PlatName eq "ARMV5" || $PlatName eq "ARMV5_ABIV2" || $CustomizedARMV5Plat)
	{
		my $RVCTBuildNumber;

		($RVCTMajorVersion, $RVCTMinorVersion, $RVCTBuildNumber) = RVCT_plat2set::get_version_list($PlatName);

		$RVCTVersion = "${RVCTMajorVersion}_${RVCTMinorVersion}";

		my $aRVCTVersion = "${RVCTMajorVersion}.${RVCTMinorVersion}";
		if (($aRVCTVersion == 2.2) && ($RVCTBuildNumber < 559))
		{
			warn "WARNING: When building using ABIV2 mode toolchain Compiler RVCT2.2 Build 559 or later is required.\n";
		}
		&Armutl_DoMmp(@_);
	}
	elsif ($RootName eq "GCCE")
	{
		($GCCEMajorVersion, $GCCEMinorVersion) = gcce_plat2set::get_version_list($Platname);

		$GCCEVersion = "${GCCEMajorVersion}_${GCCEMinorVersion}";
	}

}

my %BSF_keywords = (
			COMMON_OPTIONS => 1,
			THUMB_OPTIONS => 1,
			ARM_OPTIONS => 1,
			KERNEL_OPTIONS => 1,
			INVARIANT_OPTIONS => 1,
			LD_OPTIONS => 1,
			AR_OPTIONS => 1,
			DEBUG_FORMAT => 1
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
				push @commonOptions, $val;
			next;
		}
		if ($key =~ /THUMB_OPTIONS/) {
				push @thumbOptions, $val;
			next;
		}
		if ($key =~ /ARM_OPTIONS/) {
				push @armOptions, $val;
			next;
		}
		if ($key =~ /KERNEL_OPTIONS/) {
				push @kernelOptions, $val;
			next;
		}
		if ($key =~ /INVARIANT_OPTIONS/) {
				push @invariantOptions, $val;
			next;
		}
		if ($key =~ /LD_OPTIONS/) {
				push @linkerOptions, $val;
			next;
		}
		if ($key =~ /AR_OPTIONS/) {
				push @archiverOptions, $val;
			next;
		}
		if ($key =~ /DEBUG_FORMAT/) {
				push @debugFormat, $val;
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
	foreach my $val (@{$Options})
	{		
		# Check if the value of BSF option is to be set or added/removed.
		if($val =~ /\+\[.*\]\+|\-\[.*\]\-/)
		{
			if (@Fragments = Split_BSF_Options($val,'RemoveOptions'))
			{
				foreach my $Opt (@Fragments) 
				{
					# Remove trailing white spaces
					$Opt =~ s/\s+$//;
					# Substitute '=' with '%' which is a wild card character in makefile.
					# This is required for cases where option to be removed contains '=' (e.g.'-march=armv5t').
					# When such options are to be removed, "$(INVARIANT_OPTIONS:-march=armv5t=)" is written in the makefile.
					# However, because of the occurence of '=', pattern match fails in the makefile and such options are not removed. 
					# To resolve this, '=' is replaced with '%'  in the makefile so that the substitution pattern looks like 
					# "$(INVARIANT_OPTIONS:-march%armv5t=)" in makefile (e.g."$(INVARIANT_OPTIONS:-march%armv5t=)").
					$Opt =~ s/=/%/;
					&main::Output(
						"$OptionName := \$($OptionName:$Opt=)",					
						"\n"
					);
				}					
				@Fragments=();
			}
			if (@Fragments = Split_BSF_Options($val,'AddOptions')) 
			{
				&main::Output(
					"$OptionName += @Fragments ",
					"\n"
				);
				@Fragments=();
			}
			
			# Warn if options are not present in the form '+[...]+' or '-[...]-'
			$val =~ s/\+\[.*?\]\+|\-\[.*?\]\-//g;
			if($val !~ /^\s*$/)
			{
				print "Warning: Ignoring option(s) \"$val\" for $OptionName as option(s) should be in the form '+[...]+' or '-[...]-.\n";
			}
		}
		else
		{
			&main::Output(
				"$OptionName = $val ",
				"\n"
			);
		}		
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
	# Get the toolchain specific option prefix to segregate the options.
	my $OptionPrefix = getConfigVariable('OPTION_PREFIX');
	
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
	if($OptionPrefix) 
	{
		$Pattern = $OptionPrefix.'\S+\s*(?!'.$OptionPrefix.')\S*';
	}
	else 
	{
		# If option prefix is not set in the configuration make file, then set default
		# option prefix as '-' or '--'.
		$Pattern = '-{1,2}\S+\s*(?!-)\S*';
	}

	foreach my $Val (@Fragments) 
	{
		my @Opt = $Val =~ /$Pattern/g;
		push @Result,@Opt;				
	}
	return @Result;	
}

sub SysTrg () {
	return 1 if &main::SystemTrg;
	my $ExportLibrary=&main::ExportLibrary;
	return 1 if ($ExportLibrary =~ /EKERN/i);
	my $Trg=&main::Trg;
	return 1 if ($Trg =~ /KSRT/i);
	return 0;
}

sub GetLibList() {
	my @LibList;
	my @StaticLibList;
	my $list = getConfigVariable('STATIC_LIBS_LIST') ;

	if (length($list) >0)
	{
		@StaticLibList = split(/\s+/, $list);
	}
	if($PlatName eq "ARMV5" || $PlatName eq "ARMV5_ABIV2" || $CustomizedARMV5Plat) {
		@LibList=&Armutl_ArmLibList;
		if(@LibList==0) {
			my $LibDir = Armutl_ArmLibDir();
			# convert '/' to  '\'  if there are some '/'
			$LibDir =~ s#/#\\#g;
			if (@StaticLibList) {
				foreach my $lib (@StaticLibList) {
					push @LibList, ("$LibDir\\$lib");
				}
			}
		}
	}
	else
	{
		@LibList = ('$(STATIC_LIBS_LIST)');
	}
	return @LibList;
}

sub GetToolChainAsmFileList() {
	my @FileList;
	@FileList=&Armutl_AsmFileList;
	if(@FileList)
	{
		return @FileList;
	}
	return;
}

sub IsTargetRT() {
	my $RTtarget=&Armutl_ArmRT;
	if($RTtarget)
	{
		return $RTtarget;
	}
	return;
}

sub GetToolChainIncDir {
	if($PlatName eq "ARMV5" || $PlatName eq "ARMV5_ABIV2" || $CustomizedARMV5Plat)
	{
  		#the ToolChainIncDir's value depends on the key word ARMINC in mmp file
  		return &Armutl_ArmIncDir;
	}
	else
	{
		my $compiler_inc_path = getConfigVariable('COMPILER_INCLUDE_PATH');
		$compiler_inc_path =~ s/^\s+//g;
		return ($compiler_inc_path);
	}
}

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
	return "/$(FEEDBACK_FILE_OPTION) $file ";

}

sub getConfigVariable
{
    my ($variable) = @_;
    initialiseConfigVariables();
    return $configVariables{$variable};
}

sub initialiseConfigVariables()
{
    if (!keys(%configVariables))
    {
		%configVariables = BPABIutl_Get_Config_Variables($PlatName);

		if (!keys(%configVariables))
		{
			# no variables were extracted from configuration file.
			&main::FatalError("Cannot extract configuration variables for $PlatName");
		}
    }
}

sub PMSupportsFeatureVariants
{
	return 1;
}
# modified start: makefile improvement 
sub checkVMAPFiles
{
	my %srcSet = %{&main::getSrcSet};
	my %featureVariantInfo = &main::FeatureVariantInfo();
	my $vmapfile = &main::Trg.".".$featureVariantInfo{NAME}.".vmap";
	my $mmpFile = &main::MmpFile;
	if ( $vmapfile ){
		&main::Output(
			"\n",
			"\n",
			"# Rules to check out the VMAP files",
			"\n",
			"\n",
		);
		&main::Output(
			"CHECKVMAPUREL : ",
			"\$(EPOCTRG)\\urel\\",
			$vmapfile,
			" \n",
			"\$(EPOCTRG)\\urel\\",
			$vmapfile,
			" : \\\n",
			"\t",
			$mmpFile,
		);
		foreach my $srcfile (sort keys(%srcSet))
		{
			&main::Output(
				" \\\n \t",
				$srcfile,
			);
		}
		&main::Output(
			"\n",
			"\t-\$(ERASE) \$(EPOCTRG)\\urel\\",
			$vmapfile,
			"\n",
			"\n",
		);
		&main::Output(
			"CHECKVMAPUDEB : ",
			"\$(EPOCTRG)\\udeb\\",
			$vmapfile,
			" \n",
			"\$(EPOCTRG)\\udeb\\",
			$vmapfile,
			" : \\\n",
			"\t",
			$mmpFile,
		);
		foreach my $srcfile (sort keys(%srcSet))
		{
			&main::Output(
				" \\\n \t",
				$srcfile,
			);
		}
		&main::Output(
			"\n",
			"\t-\$(ERASE) \$(EPOCTRG)\\udeb\\",
			$vmapfile,
			"\n",
			"\n",
		);
	}
}
# modified by SV end: makefile improvement 
# modified end: makefile improvement 


1;
