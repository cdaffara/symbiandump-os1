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
# Makmake-module for creating XML files which can be imported as CodeWarrior IDE projects
# 
#

package Ide_cw;

my $CW_minimum_supported_version = 2.8;

# declare variables global for module
my $BaseAddress="";
my %IdeBlds=();
my %PrjHdrs=();
my @Win32LibList=();
my $Win32Resrc;
my $Win32StdHeaders;

my $ExtraFilesPath="";
my @KnownRoots=();

my @addedFiles=();
my $addHeaders = 1;
my $addDocuments = 1;

my %processedPlatforms;

require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	PMHelp_Mmp

	PMCheckPlatformL

	PMPlatProcessMmp

	PMStartBldList
		PMBld
			PMResrcBld

	PMEndSrcList
);

require Cl_bpabi;
use strict;
use BPABIutl;
use E32Variant;
use E32Plat; 
use Winutl;
use Armutl;
use Pathutl;
use Win32API::Registry qw( :ALL );
use Preprocessor;
use RVCT_plat2set;

sub PMHelp_Mmp {
	&Winutl_Help_Mmp;
}

my $RVCTMajorVersion = Armutl_RVCTMajorVersion();
my $RVCTMinorVersion = Armutl_RVCTMinorVersion();
my $RVCTVersion = "${RVCTMajorVersion}_${RVCTMinorVersion}";

my $oP = "--";
$oP = "-" if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2);

my $cfgMacro = &Variant_GetMacro();

use FindBin;
use lib $FindBin::Bin."\\perllib";

#use the DOM xml module
use XML::DOM;

my $projectTemplate;
my $linkDescriptorTemplate;

my $xmlParser;
my $xmlProjectDoc;
my $xmlLinkDescriptorDoc;
my $xmlLinkDescriptorCommandParent;
my $xmlLinkDescriptorSymbolParent;
my $xmlLinkDescriptorDumpFileParent;

my $xmlLinkOrder;		# for accumulating the link order
my $xmlFileList;		# for accumulating the file list
my $xmlSourceGroup;		# for accumulating the source
my $xmlHeadersGroup;	# for accumulating local header files
my $xmlRootGroup;		# for accumulating files to be added outside of project groups
my $xmlLinkGroup;		# for accumulating the link descriptor files
my $xmlLibGroup;		# for accumulating the libraries
my $xmlResourcesGroup;	# for accumulating resource related files
my $xmlDocumentsGroup;	# for accumulating supported files specified with DOCUMENT in the .mmp file

my $MmpFile;
my $VariantFile;
my $PrefixFile;

my $CW_major_version;
my $CW_minor_version;
my $CW_libpath;
my @CW_librarypath;
my @ToolChainLibList;

# Hash to store configuration makefile data for furthur processing
my %configdata;

sub RoundUp1k($) {
	# Accept C hexadecimal number (0xNNN).  Convert argument to Kb
	# rounded up to the next 1kb boundary.
	use integer;
	return (hex($_[0]) + 1023) / 1024;
}

use Genutl;
use cl_generic;

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

my %CompatibleABIs=(
	ARMI=>['ARM4', 'THUMB'],
	ARM4=>['ARMI'],
	THUMB=>['ARMI'],
	ARMV5=>['ARMV5_ABIV1'],
	GCCE=>['GCCE'],
	BPABI=>['ARMV5_ABIV2']
);
my @CompatibleABIs;

my $Makecmd;
my %ABILibPath=();

sub SystemTarget() {
	return 1 if &main::SystemTrg;
	my $ExportLibrary=&main::ExportLibrary;
	# N.B. should get better way to detect kernel probably!!
	return 1 if ($ExportLibrary =~ /EKERN/i);
	
	return 0;
}

sub SysTrg () {
	return 1 if &main::SystemTrg;
	my $ExportLibrary=&main::ExportLibrary;
	return 1 if ($ExportLibrary =~ /EKERN/i);
	my $Trg=&main::Trg;
	return 1 if ($Trg =~ /KSRT/i);
	return 0;
}

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
			return 'ARMV4';
		}
	}
	return $ABI;
}

##### ARMV5 specific options #####
my $diag_suppressions;
my $diag_warnings;
my $diag_errors;

#The options here are handcoded for ABIV1 mode.
my $contingentOptions;
my $exceptions = ' --exceptions --exceptions_unwind';
my $thumbOptions = '--thumb ';
my $armOptions = '--arm ';
my $kernelOptions = '--arm --no_exceptions --no_exceptions_unwind';
my $invariantOptions = 
  '--cpu 5T --enum_is_int -Ono_known_library --fpmode ieee_no_fenv --export_all_vtbl --no_vfe --apcs /inter';
$invariantOptions .= ' --dllimport_runtime' unless ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2);  
# specify floating point model here
my $floatingpointmodel = "softvfp";
if (&main::ARMFPU && (&main::ARMFPU =~ /^VFPV2$/i)) {
	$floatingpointmodel = "vfpv2";
}
my $floatingpoint = ' --fpu '.$floatingpointmodel.'';

# Set compiler settings
$diag_suppressions = '--diag_suppress 66,161,611,654,997,1152,1300,1464,1488,6318,6331';
$diag_warnings = '';
$diag_errors = '--diag_error 1267';
my $commonOptions = "$diag_suppressions $diag_warnings $diag_errors";  
# variables added to fetch the options from the BSF files
my @commonOptions;
my @thumbOptions;
my @armOptions;
my @kernelOptions;
my @invariantOptions;
my @linkerOptions;
my @archiverOptions;

my $linkCommand = "";		
# variables to process and store the default and BSF file options
my $invopts = $invariantOptions;
my $linkeropts;
my $archiveropts;
#varible to store the BSF add options
my $bsfaddoptions = undef;
#variable to store the compiler flags, defs and other options
my $CCFLAGS = undef;
my $CCDEFS = undef;
my $CCUREL = undef;
my $CCUDEB = undef;


#GCCE specific OPIONS

my $GCCE_CompilerOption = '-march=armv5t -mthumb-interwork -mapcs';
$GCCE_CompilerOption .= " -msoft-float -fexceptions -pipe -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas";
#Flag to check for the customized GCCE platform
my $CustGCCE=0;
#Variable to fetch the Customized platform
my %CustPlat=&main::PlatRec;
#Flags to read the options
my $kernelOption=0;
my $buildAsArmOption=0;
my $thumbOption=0;


my $ArmIncDir;
my @ArmLibList;
my $ArmRT;

my %BSF_keywords = (
		    COMMON_OPTIONS => 1,
		    THUMB_OPTIONS => 1,
		    ARM_OPTIONS => 1,
		    KERNEL_OPTIONS => 1,
		    INVARIANT_OPTIONS => 1,
			LD_OPTIONS => 1,
			AR_OPTIONS => 1
		   );
sub PMStartBldList($) 
{
	($Makecmd) = @_;
	my @BldList=&main::BldList;
	my $BasicTrgType=&main::BasicTrgType;
	my $ABI=&main::ABI;
	my $kernelOption=0;
	my $buildAsArmOption=0;
	my $thumbOption=0;
	my @MacroList=&main::MacroList();
	push @MacroList, "__SUPPORT_CPP_EXCEPTIONS__ ";
	my %plat = &main::PlatRec();

	$CCFLAGS = "";
	$CCDEFS = "";
	$CCUREL = "";
	$CCUDEB = "";
	
	$VariantFile=&main::VariantFile();	

	#read the configuration make file into the hash for only the BPABI platforms
	my $config_file = BPABIutl_Config_Path(&main::Plat);
	if ($config_file) {
		collect_config_data($config_file);
	}
		
	Read_BSF_Options() if ($plat{'CUSTOMIZES'});

	if (SysTrg()) {
		$kernelOption=1;
	} elsif (main::BuildAsARM() or ($ABI eq 'ARMV4')) {
		$buildAsArmOption=1;
	} else {
		$thumbOption=1;
	}

	my $OtherOpts = undef;
	my $toolchain = $configdata{COMPILER_PLAT};
	
	$OtherOpts = &main::CompilerOption($toolchain);
	
	if($kernelOption==1)
	{
		if(@kernelOptions) {
# Kernel options as read from BSF file (KERNEL_OPTIONS keyword)
			Set_BSF_Options('KERNEL_OPTIONS',\@kernelOptions);
		}
		$OtherOpts .= " ".fetch_config_data($configdata{KERNEL_OPTIONS});
	}
	elsif($buildAsArmOption==1)
	{	
		if(@armOptions) {
# Arm options as read from BSF file (ARM_OPTIONS keyword)
			Set_BSF_Options('ARM_OPTIONS',\@armOptions);
		}
		$OtherOpts .= " ".fetch_config_data($configdata{ARM_OPTIONS});	
	}
	elsif($thumbOption==1)
	{
		if(@thumbOptions) {
# Thumb options as read from BSF file (THUMB_OPTIONS keyword)
			Set_BSF_Options('THUMB_OPTIONS',\@thumbOptions);
		}
		$OtherOpts .= " ".fetch_config_data($configdata{THUMB_OPTIONS});
	}
	
	if($thumbOption==1 || $buildAsArmOption==1)
	{
		if (&main::ARMFPU && (&main::ARMFPU =~ /^VFPV2$/i))
		{
			$OtherOpts .= "	".$configdata{VFP2MODE_OPTION};
		}
		else
		{
			$OtherOpts .= "	".$configdata{SOFTVFPMODE_OPTION};
		}
	}

	if ($thumbOption==1)
	{
		$OtherOpts .= "	".$configdata{COMPILER_THUMB_DEFINES};
	}
	
	$OtherOpts .= "	".$configdata{COMPILER_INTERWORK_DEFINES};
	
	if((&main::Plat eq "ARMV5_ABIV1") || (&main::Plat eq "ARMV5" && !$cfgMacro)
	|| ($CustPlat{'CUSTOMIZES'} 
	&& (($CustPlat{'ROOTPLATNAME'} eq "ARMV5_ABIV1" && $cfgMacro) 
	|| ($CustPlat{'ROOTPLATNAME'} eq "ARMV5" && !$cfgMacro)))) {
		ComputeCompilerOpts();
	}
	elsif($config_file) {
		$CCFLAGS = &Cl_bpabi::getConfigVariable('CCFLAGS');
	}
	$CCFLAGS .= $OtherOpts;

	if(@invariantOptions)
	{
	# Invariant options as read from BSF file (INVARIANT_OPTIONS keyword)
		Set_BSF_Options('INVARIANT_OPTIONS',\@invariantOptions);
	}
	if(@commonOptions)
	{
	# Common options as read from BSF file (COMMON_OPTIONS keyword)
		Set_BSF_Options('COMMON_OPTIONS',\@commonOptions);
	}
	if(@linkerOptions)
	{
	# Invariant options as read from BSF file (LD_OPTIONS keyword)
		Set_BSF_Options('LD_OPTIONS',\@linkerOptions);
	}
	if ($BasicTrgType=~/^LIB$/o) {
		if(@archiverOptions)
		{
		# Invariant options as read from BSF file (AR_OPTIONS keyword)
			Set_BSF_Options('AR_OPTIONS',\@archiverOptions);
		}	
	}

	if($bsfaddoptions)
	{
		fixbsfoptions($bsfaddoptions);
	}
	$CCFLAGS .= $bsfaddoptions;
	$CCDEFS = $configdata{COMPILER_DEFINES};
	$CCDEFS .= printlist("-D", @MacroList);
	$CCDEFS .= $configdata{PLATFORM_DEFINES};

	if($kernelOption==1)
	{
		$CCDEFS .= -D__KERNEL_MODE__;
	}
	
	if($VariantFile) { 
		$CCDEFS .= " -D__PRODUCT_INCLUDE__="."\\\"".&main::Path_Split('File',$VariantFile)."\\\"";
	}
	
	foreach (@BldList) 
	{
		if($kernelOption == 0)
		{
			if (/DEB$/o) {
				$CCUDEB .= " ".$configdata{EXCEPTIONS};
			}
			else {
				$CCUREL .= " ".$configdata{EXCEPTIONS};
			}
		}
		#collect the options and macro's depending on the whether it is a UREL or UDEB		
		my @ml = &main::MacroList($_);
		if (/DEB$/o) {
			$CCUDEB .= " ".$CCFLAGS;
			$CCUDEB .= printlist("-D", @ml);
			$CCUDEB .= " ".$CCDEFS;
		}
		else {
			$CCUREL .= " ".$CCFLAGS;
			$CCUREL .= printlist("-D", @ml);
			$CCUREL .= " ".$CCDEFS;
		}
	}
}

sub ComputeCompilerOpts() {
	my %plat = &main::PlatRec();
	my $ABI=&main::ABI;
	my $TrgType=&main::TrgType;

	if (SysTrg()) {
	        $contingentOptions = $kernelOptions;
        } elsif (main::BuildAsARM() or ($ABI eq 'ARMV4')) {
	        $contingentOptions = $armOptions.$floatingpoint.$exceptions;
	    } else {
			$contingentOptions = $thumbOptions.$floatingpoint.$exceptions.' -D__MARM_THUMB__';	
	}
	# change support for ARMV4
	if ($ABI eq 'ARMV4') {
		$invopts =~ s/5T/4/;
		$invopts =~ s/inter/nointer/;
	} else {
		$contingentOptions .= ' -D__MARM_INTERWORK__';
	}
	$CCFLAGS = $commonOptions.' '.$contingentOptions.' '.$invopts.' -c '.' '.&main::CompilerOption("ARMCC");
}

sub GetMajorVersion ($)
	{
	my ($versionString) = @_;

	if ($versionString =~ /^\d\.\d\.\d$/)
		{
		$versionString =~ s/\.\d$//;
		}

	return $versionString;
	}

sub GetMinorVersion ($)
	{
	my ($versionString) = @_;

	if ($versionString =~ /^\d\.\d\.\d$/)
		{
		$versionString =~ s/^\d\.\d\.//;
		}
	else
		{
		$versionString = 0;
		}

	return $versionString;
	}

sub PMCheckPlatformL {

	# check version of CodeWarrior for Symbian OS

	my @compatibleCWInstallations;

	$CW_major_version = 0;
	$CW_minor_version = 0;

	my $minimumMajorVersion = GetMajorVersion ($CW_minimum_supported_version);
	my $minimumMinorVersion = GetMinorVersion ($CW_minimum_supported_version);

	if (defined $ENV{CW_SYMBIAN_VERSION})
		{
		# CW_SYMBIAN_VERSION is set - either MAKMAKE is being executed by an IDE's .mmp Importer,
		# or the user is specifying a specific CW version to target from the command line.
		# Either way, we've been given a single version to target, so we attempt to do just that.

		$CW_major_version = GetMajorVersion ($ENV{CW_SYMBIAN_VERSION});
		$CW_minor_version = GetMinorVersion ($ENV{CW_SYMBIAN_VERSION});
		
		push @compatibleCWInstallations, $ENV{CW_SYMBIAN_VERSION};
		}
	else
		{
		# CW_SYMBIAN_VERSION isn't set - either MAKMAKE is being executed by a pre-OEM3.0 IDE .mmp
		# Importer or from the command line.  Either way, we delve into the registry and attempt to
		# target the latest CW for Symbian OS installed, recording all CW for Symbian OS installations
		# too.

		my $regKeyHandle;
		my $topLevelKey = HKEY_LOCAL_MACHINE;
		my $productVersionKey = 'SOFTWARE\\Metrowerks\\CodeWarrior\\Product Versions';

		if (!RegOpenKeyEx($topLevelKey, $productVersionKey, 0, KEY_READ, $regKeyHandle))
			{		    		
			die "Can't read \"HKEY_LOCAL_MACHINE\\$productVersionKey\" : ", regLastError(), "\n";
		    }

		my $subKeyIndex = 0;
		my $subKeySize = 0;
		my $subKeyName;

		my @installedCWForSymbianKeys;

		while (RegEnumKeyEx($regKeyHandle, $subKeyIndex, $subKeyName, $subKeySize, [], [], [], []))
			{
			push (@installedCWForSymbianKeys, $productVersionKey."\\".$subKeyName) unless ($subKeyName !~ /Symbian/);
			$subKeyIndex++;
			}

		RegCloseKey($regKeyHandle) || print STDERR "WARNING: Could not close registry key.";

		my $versionType;
		my $versionValue;

		foreach my $installedCWForSymbianKey (@installedCWForSymbianKeys)
			{
			if (!RegOpenKeyEx($topLevelKey, $installedCWForSymbianKey, 0, KEY_READ, $regKeyHandle))
				{		    		
				die "Can't read \"HKEY_LOCAL_MACHINE\\$installedCWForSymbianKey\" : ", regLastError(), "\n";
			    }

			if (!RegQueryValueEx($regKeyHandle, "VERSION", [], $versionType, $versionValue, []))
				{
				die "Can't read \"HKEY_LOCAL_MACHINE\\$installedCWForSymbianKey\\VERSION\" : ", regLastError(), "\n";
				}

			my $temp_major_version = GetMajorVersion ($versionValue);
			my $temp_minor_version = GetMinorVersion ($versionValue);

			if (($temp_major_version > $CW_major_version) ||
				(($temp_minor_version > $CW_minor_version) &&
				   ($temp_major_version >= $CW_major_version)))
				{
				$CW_major_version = $temp_major_version;
				$CW_minor_version = $temp_minor_version;
				}
			
			if (($temp_major_version > $minimumMajorVersion) ||
				(($temp_minor_version > $minimumMinorVersion) &&
				   ($temp_major_version >= $minimumMajorVersion)))
				{
				push @compatibleCWInstallations, $versionValue;
				}

			RegCloseKey($regKeyHandle);
			}
		}

	# We've determined a CW version to target, now we validate if we actually support this

	if (!$CW_major_version ||
		($CW_major_version < $minimumMajorVersion) ||
		(($CW_major_version >= $minimumMajorVersion) && ($CW_minor_version < $minimumMinorVersion)))
		{
		if (defined $ENV{CW_SYMBIAN_VERSION})
			{
			die "Error: CW_SYMBIAN_VERSION is set to $ENV{CW_SYMBIAN_VERSION}.\n       The minimum version supported by these tools is $CW_minimum_supported_version.\n";
			}
		else
			{
			die "ERROR: Unable to identify a compatible CodeWarrior for Symbian OS installation.\n";
			}
		}

	if (@compatibleCWInstallations > 1)
		{
		my $targetVersion = $CW_major_version;
		$targetVersion .= ".$CW_minor_version" if $CW_minor_version;
			
		print ("Info: More than one compatible CodeWarrior for Symbian OS installation has been detected.\n");
		print ("      The generated project will target $targetVersion - to override this, set the CW_SYMBIAN_VERSION\n");
		print ("      environment variable to the version number you wish to target and re-run this command.\n");
		print ("      Supported version numbers detected : @compatibleCWInstallations.\n");
		}
	else
		{
		print ("Info: Detected CodeWarrior Version Major=$CW_major_version Minor=$CW_minor_version\n");
		}

	# CW version has been validated, tailor generated projects on this basis

	$CW_libpath = 'Symbian_Support\Win32-x86 Support\Libraries\Win32 SDK';
 	push @CW_librarypath,$CW_libpath;
 	# Lib path to support the Carbide runtime libraries
 	$CW_libpath = 'Symbian_Support\Runtime\Runtime_x86\Runtime_Win32\Libs';
 	push @CW_librarypath,$CW_libpath;
		
	if ($CW_major_version == 2.8)
		{
		$projectTemplate = "CW_project_template_v3.xml";
		$linkDescriptorTemplate = "cw_link_descriptor_template.cwlink";
		}
	else
		{
		$projectTemplate = "CW_project_template_v4.xml";
		$linkDescriptorTemplate = "cw_link_descriptor_template_v2.cwlink";		
		}
		
	$xmlParser = new XML::DOM::Parser; 
	$xmlProjectDoc = $xmlParser->parsefile ($FindBin::Bin."\\$projectTemplate");
}

# Check if a platform is a customization
sub IsCustomization($) {
	my ($plat) = @_;
	return 1 if (Plat_Customizes($plat));
	return 0;
}

sub PMPlatProcessMmp (@) {
	
	my $currentPlat=&main::Plat;

	return if ($processedPlatforms{$currentPlat});
	$processedPlatforms{$currentPlat}=1;
	@ToolChainLibList = &GetLibList;
	my $TrgType=&main::TrgType;
	if ($CustPlat{'CUSTOMIZES'} && ($CustPlat{'ROOTPLATNAME'} eq "GCCE"))
	{
		$CustGCCE=1;
	}

	if ($currentPlat =~ /^WINSCW$/)
		{	
		my $includes = $ENV{MWCWinx86Includes};
		&Winutl_DoMmp_Parse(\@_, $includes);
		@Win32LibList=&Winutl_Win32LibList;
		$Win32Resrc=&Winutl_Win32Resrc;
		$Win32StdHeaders=&Winutl_Win32StdHeaders;
		$BaseAddress=&Winutl_BaseAddress unless ($TrgType eq 'EXE');
		}
	elsif ($currentPlat =~ /^ARMV5/ || IsCustomization($currentPlat))
		{	
		&Armutl_DoMmp(@_);
		$ArmIncDir = &Armutl_ArmIncDir;
		&main::SetStdIncPaths($ArmIncDir);
		@ArmLibList = &Armutl_ArmLibList;
		$ArmRT = &Armutl_ArmRT;
		}

	my $BaseTrg=&main::BaseTrg;
	my $MakeFilePath=&main::MakeFilePath;
	$MmpFile=&main::MmpFile;
	$VariantFile=&main::VariantFile();		

	# Set up the list of known roots

	my $epocroot=&main::Path_Drive . &main::EPOCPath;
	$epocroot =~ s/EPOC32\\$//i;

	if ($currentPlat eq "GCCE" || $CustGCCE)
	{
		$PrefixFile=$epocroot.'epoc32\\include\\gcce\\gcce.h';
	}
	else
	{
		$PrefixFile=$epocroot.'epoc32\\include\\rvct'.$RVCTVersion.'\\rvct'.$RVCTVersion.'.h';
	}
	my $mmproot = &main::Path_Drive . &main::Path_Split('Path',$MmpFile);
	my $mmprootname = "MMPDir";
	my $mmpisglobal = 0;

	if (defined $ENV{CW_ROOT_NAME})
		{
		# generate KnownRoots suitable for the IDE MMP importer
		# This has a global source tree for EPOCROOT, and puts the
		# project next to the MMP file

		addKnownRoot($ENV{CW_ROOT_NAME}, 1, $epocroot, "");
		$mmprootname = "Project";
		$mmpisglobal = 1;
		}
	else
		{
		# generate KnownRoots suitable for command-line generated XML files
		# We will add a user source tree for MMPDir and EPOCROOT, but can't use
		# EPOCROOT for the OutputDirectory

		addKnownRoot("EPOCROOT", 0, $epocroot, "");
		}
	addKnownRoot($mmprootname, $mmpisglobal, $mmproot, "");

	# Allow for MMP files in component subdirectories by matching multiple levels
	# up to get {MMPDir}..\whatever paths. Stop one level down from the root,
	# since "everything on this drive" seems a bit extreme
	#
	my $tmppath = $mmproot;
	my $dotdots = '';
	while ($tmppath =~ /^(.:.+\\)[^\\]+\\$/i)
		{
		$tmppath = $1;
		$dotdots .= "..\\";
		addKnownRoot($mmprootname, $mmpisglobal, $tmppath, $dotdots);
		}
}

sub findTarget($$$) {
	my ($name,$platbld,$abibld) = @_;

	my @targets = $xmlProjectDoc->getElementsByTagName("TARGET",1); 

	foreach my $target (@targets)
		{
		
		my $element = $target->getElementsByTagName("NAME",0)->item(0);
		$element = $element->getFirstChild;
		
	#   here we want to get the plat build that is specified in the cw project tempalte
	#   and not the abi build - there are more platbuilds than abibuilds so other targets
	#   (e.g. GCCE) can get the wrong tempalte "NAME" (i.e. it will be ARMV5 rather than GCCE, which is not want we want)
		if ($element->getData() =~ /$platbld$/)
			{
			
			my $newtarget=$target->cloneNode(1);
			$target->getParentNode()->appendChild($newtarget);
			$element = $newtarget->getElementsByTagName("NAME",0)->item(0);
			$element = $element->getFirstChild;
			$element->setData("$platbld");
			
			# remember name as an attribute: this is removed before
			# writing out the project.
			$newtarget->setAttribute("NAME", "$platbld");
			return $newtarget;
			}
			else
			{
				my $newtarget=$target->cloneNode(1);
				my $newplat=&main::Plat." ".&main::Bld;
				$target->getParentNode()->appendChild($newtarget);
				$element = $newtarget->getElementsByTagName("NAME",0)->item(0);
				$element = $element->getFirstChild;
				$element->setData("$newplat");
	 			
				# remember name as an attribute: this is removed before
				# writing out the project.
				$newtarget->setAttribute("NAME", "$newplat");
				return $newtarget;
			}
		}
	return undef;
}

sub newList($$$) {
	my ($target, $tag, $replace) = @_;

	my $newlist = new XML::DOM::Element($xmlProjectDoc,$tag);
	if ($replace==1)
		{
		my $elements = $target->getElementsByTagName($tag,0);
		my $element = $elements->item(0);
		$target->replaceChild($newlist, $element);
		}
	else
		{
		$target->appendChild($newlist);
		}
	return $newlist;
}

sub changeValue($$) {
	my ($setting, $replacement) = @_;
	my $value = $setting->getElementsByTagName("VALUE",0)->item(0);

	if (defined $value)
		{
		if ($value->hasChildNodes)
			{
			$value->getFirstChild->setData($replacement);
			}
		else
			{
			$value->addText($replacement);
			}
		}
}

sub textSetting($$$$) {
	my ($element,$name,$value,$insertionpoint)=@_;

	my $setting = new XML::DOM::Element($xmlProjectDoc,"SETTING");
	&textElement($setting, "NAME", $name);
	&textElement($setting, "VALUE", $value);
	$element->insertBefore($setting, $insertionpoint);
	$element->addText("\n");
}

sub addKnownRoot($$$$) {
	my ($rootname, $global, $rootpath, $pathprefix) = @_;
	$rootpath=&main::Path_Chop($rootpath);
	push @KnownRoots, [$rootname, $global, quotemeta($rootpath), $pathprefix];
}

sub addRootedPath($$$) {
	my ($setting,$needglobal,$path) = @_;
	my $root = "Absolute";

	if ($path =~ /^\\/)
		{
		$path = &main::Path_Drive . $path;	# ensure it has a drive letter
		}

	foreach (@KnownRoots)
		{
		my ($rootname, $global, $rootpath, $pathprefix) = @{$_};

		next if ($needglobal && !$global);
		if ($path =~ /^$rootpath\\/i)
			{			
			$path =~ s/^$rootpath\\/$pathprefix/i;
			$root = $rootname;
			last;
			}
		}
	$path=&main::Path_Chop($path);
	if ($root eq "Absolute" && $path =~ /^(.:)$/)
		{
		$path .= "\\";
		}

	&textSetting($setting, "Path", $path);
	&textSetting($setting, "PathFormat", "Windows");
	&textSetting($setting, "PathRoot", $root);
}

sub changePathSetting($$$) {	
	my ($setting,$global,$value) = @_;

	my @oldstuff = $setting->getElementsByTagName("SETTING",0);
	foreach my $old (@oldstuff)
		{
		&removeNode($old);
		}

	&addRootedPath($setting,$global,$value);
}

sub addSourceTrees($) {
	my ($node) = @_;

	my $element = $node->getElementsByTagName("VALUE",0)->item(0);
	&removeNode($element) if (defined $element);

	if (defined $ENV{CW_ROOT_NAME})
		{
		return;	# paths were converted to be relative to global source trees
		}

	my $sourcepath = new XML::DOM::Element($xmlProjectDoc,"SETTING");
	$sourcepath->addText("\n");
	&textSetting($sourcepath, "Name", "EPOCROOT");
	&textSetting($sourcepath, "Kind", "AbsolutePath");

	my $pathdata = new XML::DOM::Element($xmlProjectDoc,"SETTING");
	&textElement($pathdata, "NAME", "Path"); 
	$pathdata->addText("\n");

	my $epocroot=&main::EPOCPath;
	$epocroot =~ s/\\EPOC32\\$/\\/i;

	&addRootedPath($pathdata, 1, $epocroot);

	$sourcepath->appendChild($pathdata);
	$node->appendChild($sourcepath);

	my $mmproot = &main::Path_Split('Path',$MmpFile);
	my $sourcepath2 = new XML::DOM::Element($xmlProjectDoc,"SETTING");
	$sourcepath2->addText("\n");
	&textSetting($sourcepath2, "Name", "MMPDir");
	&textSetting($sourcepath2, "Kind", "AbsolutePath");

	my $pathdata2 = new XML::DOM::Element($xmlProjectDoc,"SETTING");
	&textElement($pathdata2, "NAME", "Path"); 
	$pathdata2->addText("\n");
	&addRootedPath($pathdata2, 1, $mmproot);

	$sourcepath2->appendChild($pathdata2);
	$node->appendChild($sourcepath2);

}

sub addUserSearchPaths($) {
	my ($node) = @_;

	my @elements = $node->getElementsByTagName("SETTING",0);
	foreach (@elements)
		{
		&removeNode($_);
		}

	my %ordereddirs;
	my @ordereddirlist=();
	foreach my $dir (&main::UserIncPaths)
		{
		next if (!defined $dir);
		$dir = &main::Path_Chop($dir)."\\";
		my $key = uc $dir;
		if (! defined($ordereddirs{$key}))
			{
			$ordereddirs{$key}=1;
			push @ordereddirlist, $dir;
			}
		}

	# now add the directories used to find source files
	
	my %dirs;
	my $SourceStructRef=&main::SourceStructRef;
	foreach my $SourceRef (@$SourceStructRef)
		{
		 $dirs{$$SourceRef{SrcPath}}=1;
		}
	my $DefFile = &main::DefFile;
	if ($DefFile)
		{			
		$DefFile = &main::Path_Split('Path',$DefFile);
		$dirs{$DefFile}=1;
		}

	my $MmpFilePath = &main::Path_Split('Path',$MmpFile);
	$dirs{$MmpFilePath}=1;
		
	$dirs{$ExtraFilesPath}=1;
		

	foreach my $srcdir (sort keys %dirs)
		{
		if (!defined($ordereddirs{uc $srcdir}))
			{
			push @ordereddirlist, $srcdir;
			}
		}
		
	foreach my $srcdir (@ordereddirlist)
		{
		my $accesspath = new XML::DOM::Element($xmlProjectDoc,"SETTING");
		$accesspath->addText("\n");
		my $pathdata = new XML::DOM::Element($xmlProjectDoc,"SETTING");
		&textElement($pathdata, "NAME", "SearchPath"); 
		$pathdata->addText("\n");
		&addRootedPath($pathdata, 0, $srcdir);

		$accesspath->appendChild($pathdata);
		&textSetting($accesspath, "Recursive", "false");
		&textSetting($accesspath, "FrameworkPath", "false");
		&textSetting($accesspath, "HostFlags", "All");
		$node->appendChild($accesspath);
		}
}

sub addSystemSearchPaths($) {
	my ($node) = @_;

	my @elements = $node->getElementsByTagName("SETTING",0);
	foreach (@elements)
		{
		&removeNode($_);
		}

	my $ASSPLinkPath;
	$ASSPLinkPath = &main::ASSPLinkPath if (&main::ASSPLibList);

	my @extraIncPaths=();
	push @extraIncPaths, $ArmIncDir if $ArmIncDir;

	my %ordereddirs;
	my @ordereddirlist=();
	
	foreach my $dir (&main::SysIncPaths, @extraIncPaths, &main::StatLinkPath, $ASSPLinkPath, &main::LinkPath)
		{
		next if (!defined $dir);
		$dir = &main::Path_Chop($dir)."\\";
		my $key = uc $dir;
		if (! defined($ordereddirs{$key}))
			{
			$ordereddirs{$key}=1;
			push @ordereddirlist, $dir;
			}
		}

	my %dirs;

	if ($VariantFile)
		{
		my $VariantFilePath = &main::Path_Split('Path',$VariantFile);
		$dirs{$VariantFilePath}=1;
		}

	if (((&main::Plat =~ /^ARMV5/) || (&main::Plat =~ /^GCCE$/) ||(IsCustomization(&main::Plat))) && $PrefixFile) 
		{
		my $PrefixFilePath = &main::Path_Split('Path',$PrefixFile);
		$dirs{$PrefixFilePath}=1;
		}

	foreach my $srcdir (sort keys %dirs)
		{
		if (!defined($ordereddirs{uc $srcdir}))
			{
			push @ordereddirlist, $srcdir;
			}
		}
		
	foreach my $srcdir (@ordereddirlist)
		{
		my $accesspath = new XML::DOM::Element($xmlProjectDoc,"SETTING");
		$accesspath->addText("\n");
		my $pathdata = new XML::DOM::Element($xmlProjectDoc,"SETTING");
		&textElement($pathdata, "NAME", "SearchPath"); 
		$pathdata->addText("\n");
		&addRootedPath($pathdata, 0, $srcdir);

		$accesspath->appendChild($pathdata);
		&textSetting($accesspath, "Recursive", "false");
		&textSetting($accesspath, "FrameworkPath", "false");
		&textSetting($accesspath, "HostFlags", "All");
		$node->appendChild($accesspath);
		}
	
	if (&main::Plat =~ /^WINSCW$/)
	{
 		my $lpath;
 		foreach $lpath (@CW_librarypath)
 		{
 			# only add Win32 SDK for WINSCW system access paths	
 			my $accesspath = new XML::DOM::Element($xmlProjectDoc,"SETTING");
 			$accesspath->addText("\n");
 			my $pathdata = new XML::DOM::Element($xmlProjectDoc,"SETTING");
 			&textElement($pathdata, "NAME", "SearchPath"); 
 			$pathdata->addText("\n");
 			
 			&textSetting($pathdata, "Path", $lpath);
 			&textSetting($pathdata, "PathFormat", "Windows");
 			&textSetting($pathdata, "PathRoot", "CodeWarrior");
 			$accesspath->appendChild($pathdata);
 			&textSetting($accesspath, "Recursive", "false");
 			&textSetting($accesspath, "FrameworkPath", "false");
 			&textSetting($accesspath, "HostFlags", "All");
 			$node->appendChild($accesspath);
 		}
	}
	if ($CustPlat{'CUSTOMIZES'} && ($CustPlat{'ROOTPLATNAME'} eq "GCCE"))
	{
		$CustGCCE=1;
	}
	if (&main::Plat =~ /^GCCE$/ || $CustGCCE)
	{
		my $accesspath = new XML::DOM::Element($xmlProjectDoc,"SETTING");
		$accesspath->addText("\n");
		my $pathdata = new XML::DOM::Element($xmlProjectDoc,"SETTING");
		&textElement($pathdata, "NAME", "SearchPath"); 
		$pathdata->addText("\n");
		
		my $GCCE_IncludePath = GetGCCELibPath("-print-libgcc-file-name");
		$GCCE_IncludePath .= '\\include';
		&textSetting($pathdata, "Path", $GCCE_IncludePath);
		
		&textSetting($pathdata, "PathFormat", "Windows");
		&textSetting($pathdata, "PathRoot", "Absolute");
		$accesspath->appendChild($pathdata);
		&textSetting($accesspath, "Recursive", "false");
		&textSetting($accesspath, "FrameworkPath", "false");
		&textSetting($accesspath, "HostFlags", "All");
		$node->appendChild($accesspath);
	}
	
}

sub addDownloadFileList($) {
	my ($node, @DownloadList) = @_;
	
	my @elements = $node->getElementsByTagName("SETTING",0);
	foreach (@elements)
		{
		&removeNode($_);
		}

	my $epocdata = &main::EPOCPath . "data\\";
	foreach my $srcfile (@DownloadList)
		{
		my $targetpath = $srcfile;
		$targetpath =~ s/^Z\\/C:\\/i;
		
		my $download = new XML::DOM::Element($xmlProjectDoc,"SETTING");
		$download->addText("\n");
		my $pathdata = new XML::DOM::Element($xmlProjectDoc,"SETTING");
		&textElement($pathdata, "NAME", "HostFilePath"); 
		$pathdata->addText("\n");
		&addRootedPath($pathdata, 0, "$epocdata$srcfile");

		$download->appendChild($pathdata);
		&textSetting($download, "TargetFilePath", $targetpath);
		$node->appendChild($download);
		}
	}

sub kitRelativePath ($) {
	my ($kitRootBasedPath) = @_;
		
	my $kitRoot = &main::EPOCPath;
	$kitRoot =~ s/EPOC32\\$//i;
	$kitRoot = quotemeta (&main::Path_Chop($kitRoot));

	$kitRootBasedPath =~ s/^$kitRoot//i;

	$kitRootBasedPath;
}
	
sub PMBld() {

	my %changedsettings;

	my $ABI=&main::ABI;
	my $Bld=&main::Bld;
	my $Plat=&main::Plat;
	my $BaseName=&main::BaseMak;
	my @SrcList=&main::SrcList;
	my $BaseTrg=&main::BaseTrg;
	my @BldList=&main::BldList;
	my $DefFile=&main::DefFile;
	my $FirstLib=&main::FirstLib;
	# IsCustomDllUseCase() subroutine is called to check if the given executable 
	# is a custom dll or not.
	my $IsCustomDll = Cl_bpabi::IsCustomDllUseCase();
	# ABI flags set depending on the ENABLE_ABIV2_MODE macro set in the variant file.
	my $ABIV1 = 0;
	my $ABIV2 = 0;
	if (($Plat eq "ARMV5_ABIV1" && $cfgMacro) || ($Plat eq "ARMV5" && !$cfgMacro)
	|| ($CustPlat{'CUSTOMIZES'} 
	&& (($CustPlat{'ROOTPLATNAME'} eq "ARMV5_ABIV1" && $cfgMacro) || ($CustPlat{'ROOTPLATNAME'} eq "ARMV5" && !$cfgMacro))))
	{
		$ABIV1=1;
	}
	elsif (($Plat eq "ARMV5_ABIV2" && !$cfgMacro) || ($Plat eq "ARMV5" && $cfgMacro)
	|| ($CustPlat{'CUSTOMIZES'} 
	&& (($CustPlat{'ROOTPLATNAME'} eq "ARMV5_ABIV2" && !$cfgMacro) || ($CustPlat{'ROOTPLATNAME'} eq "ARMV5" && $cfgMacro))))
	{
		$ABIV2=1;
	}
	
	if ($CustPlat{'CUSTOMIZES'} && ($CustPlat{'ROOTPLATNAME'} eq "GCCE"))
	{
		$CustGCCE=1;
	}

 	if ($ABIV1 && ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2)) {
	        # Temporary Workaround for RVCT2.1 static libs problem with RVCT2.2 builds
		# Rename FirstLib.lib static lib used with RVCT2.1 as FirstLib2_1.lib
		if ($FirstLib=~/^\s*(\S+)(\.lib)$/io) {
		        if ($1!~/$RVCTVersion/i) {
				$FirstLib=$1.$RVCTVersion.".lib";
			}
		}
	}	

	my $BasicTrgType=&main::BasicTrgType;
	my @LibList;
	my @StatLibList=&main::StatLibList;
	if ($ABIV1 && ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2)) {
	        # Temporary Workaround for RVCT2.1 static libs problem with RVCT2.2 builds
		# Rename all the static libs used with RVCT2.1 as libname2_1.lib
		for (my $i =0; $i < scalar(@StatLibList); $i++) {
		        if ($StatLibList[$i]=~/^\s*(\S+)(\.lib)$/io) {
			        if ($1!~/$RVCTVersion/i) {
				        $StatLibList[$i]=$1.$RVCTVersion.".lib";
				}
			}
		}
	}
	

	my @ASSPLibList = &main::ASSPLibList;
	my $Trg=&main::Trg;
	if ($ABIV1 && ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2)) {
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

	my $TrgPath=&main::TrgPath;
	my $TrgType=&main::TrgType;
	my $epocroot=&main::Path_Drive . &main::EPOCPath;
	$epocroot =~ s/EPOC32\\$//i;
	my $UIDFile;

	$ExtraFilesPath = &main::MakeFilePath;

	if ($Bld =~ /DEB/) {
		@LibList=&main::DebugLibList;
	} else {
		@LibList=&main::LibList;
	}
	my $xmlTarget;
	if ($ABI =~ /BPABI/)
	{
		$ABI = "GCCE";
	}
	
	if ($Plat eq "GCCE" || $CustGCCE || $ABIV2) {
	
		if ($CW_major_version < 3.1) {
			die "FATAL ERROR: Target $Plat requires CodeWarrior for Symbian release 3.1 at minimum.\n";
		}
	}
	
	if ($ABIV2 && ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2))
	{
		die "FATAL ERROR: Target ARMV5_ABIV2 requires RVCT version 2.2 and greater. Detected RVCT $RVCTMajorVersion.$RVCTMinorVersion.\n";
	}
	
	if (($RVCTMajorVersion == 2 && $RVCTMinorVersion >= 2) && $CW_major_version < 3.1 && $ABIV2)
	{
		die "FATAL ERROR: Detected RVCT Version $RVCTMajorVersion.$RVCTMinorVersion and CodeWarrior version $CW_major_version. RVCT 2.2 and greater requies CodeWarrior version 3.1 at minimum.\n";
	}
	
    $xmlTarget = findTarget($BaseName, "$Plat $Bld", "$ABI $Bld");
		
	return if (!defined($xmlTarget));
	
	my $targetname = $xmlTarget->getAttribute("NAME");

	my $UnderlyingABI=PMUnderlyingABI($ABI);
	my @ChopRTWSysIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::SysIncPaths));
	my @ChopRTWUserIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::UserIncPaths));
	my $EPOCPath=&main::EPOCPath;
	my $LinkAs=&main::LinkAs;
	my $LibPath=&main::LibPath;
	my @UidList=&main::UidList;	
	my $WarningLevelGCC=&main::CompilerOption("GCC");
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

	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my $EPOCIncPath=&main::EPOCIncPath;
	my $ChopRelPath=&main::Path_Chop(&main::RelPath);
	my $RelPath=&main::RelPath;
	my $StatLinkPath=&main::StatLinkPath;
	my $ParentPlat;
	
# 	Check if a platform is customization, if yes find the parent platform.
	my $IsPlatCustomization=IsCustomization($Plat);
	if ($IsPlatCustomization) {
		$ParentPlat = Plat_Customizes($Plat);
	}
	
	my @RTLibList;
	if ($ABIV1) {
	    @RTLibList = ('dfpaeabi.lib', "dfprvct${RVCTVersion}.lib", 'drtaeabi.lib', 'drtaeabi.lib(VtblExports.o)');
	    if ($RVCTMajorVersion == 2 && $RVCTMinorVersion >= 2) {
		# The scppnwdl.lib should come before drtrvct2_2.lib
		push @RTLibList, "scppnwdl.lib";
		push @RTLibList, "drtrvct${RVCTVersion}.lib";
	    }
	    else
	    {
		push @RTLibList, "dfprvct${RVCTVersion}-thunk.lib";
		push @RTLibList, "drtrvct${RVCTVersion}.lib";
	    }    
	}
	elsif ($ABIV2)
	{
		@RTLibList = ('drtaeabi.dso', 'dfpaeabi.dso', "dfprvct${RVCTVersion}.dso");
	    if ($RVCTMajorVersion == 2 && $RVCTMinorVersion >= 2) {
		# The scppnwdl.lib should come before drtrvct2_2.lib
		push @RTLibList, "scppnwdl.dso";
		push @RTLibList, "drtrvct${RVCTVersion}.dso";
	    }
	}
	
	my @compatibleDOCUMENTExtensions = ("cfg", "h", "hrh", "iby", "inf", "ini", "loc", "mmpi", "policy", "ra", "rh", "rls", "rss", "script", "txt");
	my @DocList = &main::DocList;
	@addedFiles=();


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
		$PlatOpt{Gcc}='-mthumb-interwork';
		$PlatOpt{Dlltool}=$ABIDlltool{THUMB};
	}
	
	elsif ($Plat ne 'WINSCW' && $Plat ne 'ARMV5' && !$IsPlatCustomization && $Plat ne 'GCCE' && $Plat ne 'ARMV5_ABIV2' && $Plat ne 'ARMV5_ABIV1') { 
		&main::FatalError("Platform module - ABI \"$ABI\" unrecognised");
	}
	
	if ($Plat ne 'WINSCW') {
		@CompatibleABIs=@{$CompatibleABIs{$UnderlyingABI}};
	}

#	set up CompatibleABI lib path hash
	foreach (@CompatibleABIs) {
		$ABILibPath{$_}=&main::Path_Strip("$LibPath..\\..\\$_\\");
	}

	$Dlltool=$ToolPrefix.'dlltool.exe';
	$Archive=$ToolPrefix.'ar.exe';
	$Link=$ToolPrefix.'ld.exe';
	$Objcopy=$ToolPrefix.'objcopy.exe';

	my $WarningLevelCW=&main::CompilerOption("CW");

	$xmlFileList = newList($xmlTarget,"FILELIST",1);
	$xmlFileList->addText("\n");

	$xmlLinkOrder = newList($xmlTarget,"LINKORDER",1);
	$xmlLinkOrder->addText("\n");

	# Create temporary sublists, which will be
	# removed during finaliseProject

	$xmlSourceGroup = newList($xmlTarget,"SOURCEGROUP",0);
	$xmlSourceGroup->setAttribute("TARGET", $targetname);
	$xmlSourceGroup->addText("\n");

	$xmlHeadersGroup = newList($xmlTarget,"HEADERSGROUP",0);
	$xmlHeadersGroup->setAttribute("TARGET", $targetname);
	$xmlHeadersGroup->addText("\n");

	$xmlRootGroup = newList($xmlTarget,"ROOTGROUP",0);
	$xmlRootGroup->setAttribute("TARGET", $targetname);
	$xmlRootGroup->addText("\n");
		
	$xmlLinkGroup = newList($xmlTarget,"LINKGROUP",0);
	$xmlLinkGroup->setAttribute("TARGET", $targetname);
	$xmlLinkGroup->addText("\n");

	$xmlLibGroup = newList($xmlTarget,"LIBGROUP",0);
	$xmlLibGroup->setAttribute("TARGET", $targetname);
	$xmlLibGroup->setAttribute("PLAT", $Plat);
	$xmlLibGroup->addText("\n");

	$xmlResourcesGroup = newList($xmlTarget,"RESOURCESGROUP",0);
	$xmlResourcesGroup->setAttribute("TARGET", $targetname);
	$xmlResourcesGroup->addText("\n");

	$xmlDocumentsGroup = newList($xmlTarget,"DOCUMENTSGROUP",0);
	$xmlDocumentsGroup->setAttribute("TARGET", $targetname);
	$xmlDocumentsGroup->addText("\n");

	my $debug="";
	$debug="Debug" if ($Bld =~ /DEB$/);

	my @RuntimeLibs = ();	# add platform-specific runtime libraries here
	if (&main::PlatCompiler eq "GCC32")
		{
		if ($BasicTrgType=~/^DLL$/o) 
			{ # Add the DLL stub library
			push @RuntimeLibs, "EDLLSTUB.LIB";
			}
		if ($BasicTrgType=~/^(DLL|EXE)/o) 
			{ # Add the GCC helper fns
			push @RuntimeLibs, "EGCC.LIB";
			}
		}
	
	if ($Plat eq "GCCE" || $CustGCCE)
		{
			push @RuntimeLibs, "usrt2_2.lib";    # UDEB/UREL Specific
			push @RuntimeLibs, "dfpaeabi.dso";
			push @RuntimeLibs, "dfprvct2_2.dso";
			push @RuntimeLibs, "drtaeabi.dso"; 
			push @RuntimeLibs, "scppnwdl.dso"; 
			push @RuntimeLibs, "drtrvct2_2.dso";
			if ($BasicTrgType=~/^DLL$/o) { # Add the DLL stub library
				push @RuntimeLibs, "EDLLSTUB.LIB";   # UDEB/UREL Specific
				}
		}

	addFile(&main::Path_Split('File',$MmpFile), "Text", "", "", "Root");

	# Create the uid.cpp file	
	if ($Plat eq "WINSCW" && $BasicTrgType=~/^(EXE|DLL)$/oi)
		{
		my @UidList=&main::UidList;
		
		# create the UID source file
		my $priority = "EPriorityForeground";
		if (&main::ProcessPriority) {
			$priority="EPriority".&main::ProcessPriority;
		}

		my $UidText=join(
			"\n",
			'// Makmake-generated uid source file',
			'#include <e32cmn.h>',
			'#pragma data_seg(".SYMBIAN")',
			'__EMULATOR_IMAGE_HEADER2('
		);
		foreach (@UidList) {
			$UidText.="$_,";
		}
		my $vstr = "0x".&Genutl_VersionToHexString(&main::Version);
		my $vid = &main::VendorId;
		if(!$vid) { $vid="0"; }
		$UidText.="$priority,".(&main::CapabilityFlags)[0]."u,".(&main::CapabilityFlags)[1]."u,".&main::SecureId.",".$vid.",$vstr,";	# second capability word always 0 for now
		if (&main::AllowDllData) {
			$UidText.="1,";
		} else {
			$UidText.="0,";
		}
		chop $UidText;
		$UidText.=")\n";
		$UidText.="#pragma data_seg()\n";

		$UIDFile = $BaseTrg.'_UID_.cpp';
		&main::CreateExtraFile("${ExtraFilesPath}$UIDFile", $UidText);
		}


	if (-e $DefFile)
		{
		addFile(&main::Path_Split('File',$DefFile), "Text", "", &main::DefFileType."\\");
		}

	# Add resources: rsc files, mbm files and aif files

	my $mmpdir = &main::Path_Split('Path',$MmpFile);
	$changedsettings{"SymbianResourcesMMPFileLocation"} = "{0}$mmpdir";
	my $ResourcesText="";
	my @ResourceDownloadList=();
	
	# --- MBM files
	
	my $BitMapStructRef = &main::BitMapStructRef();
	my $BitMapRef;
	
	foreach my $BitMapRef (@$BitMapStructRef) {
		my $trgfile = $$BitMapRef{Trg};
# change - only use colour resource files
		next if ($trgfile =~ /\.MBW$/i);	# ignore greyscale MBM files
		$trgfile =~ s/\.MCL$/.MBM/;			# convert MCL to MBM for immediate use
		my $entry = "  <mbm targetfile = \"$trgfile\"";
		$entry .= " targetpath = \"".&main::Path_Chop($$BitMapRef{TrgPath})."\"";
		push @ResourceDownloadList, $$BitMapRef{TrgPath}.$trgfile;
		if (defined $$BitMapRef{Hdr})
			{
			$entry .= " header = \"true\"";
			}
		else
			{
			$entry .= " header = \"false\"";
			}
		$entry .= ">\n";
		foreach my $SrcRef (@{$$BitMapRef{Source}}) {
			$entry .= "    <bmp bpp = \"$$SrcRef{ClDepth}\"";
			my $bmpfile = &main::Path_Split('File',$$SrcRef{Src});
			my $bmppath = &main::Path_Split('Path',$$SrcRef{Src});
			my $sourcepath = &main::Path_Chop(&main::Path_MakeRltToBase($mmpdir,$bmppath));
			$entry .= " sourcepath = \"$sourcepath\"";
			$entry .= " sourcefile = \"$bmpfile\"/>\n";
		}
		$ResourcesText .= $entry . "  </mbm>\n";
	}
	
	# --- AIF files

	my $AifStructRef = &main::AifStructRef();
	my $AifRef;

	foreach $AifRef (@$AifStructRef) {
# regression change - workaround lack of AIF directory
		my $trgpath=&main::TrgPath;
		my $trgfile=&main::Path_Split('File',$$AifRef{Trg});
		my $path=&main::Path_Split('Path',"$trgpath$$AifRef{Trg}");
		$path=&main::Path_Chop($path);  
# change - only use colour resource files
		next if ($trgfile =~ /\.ABW$/i);	# ignore greyscale AIF files
		$trgfile =~ s/\.ACL$/.AIF/;			# convert ACL to AIF for immediate use
 		my $rssfile = &main::Path_Split('File',$$AifRef{Source});
 		my $rsspath = &main::Path_Split('Path',$$AifRef{Source});
 		my $sourcepath=&main::Path_Chop(&main::Path_MakeRltToBase($mmpdir,$rsspath));
 		my $entry = "  <aif sourcefile = \"$rssfile\"";
   		$entry .= " sourcepath = \"$sourcepath\"";
   		$entry .= " targetfile = \"$trgfile\" targetpath = \"$path\">\n";
   		push @ResourceDownloadList, "$path\\$trgfile";
 		foreach my $BitmapRef (@{$$AifRef{BitMaps}}) {
 			$entry .= "    <bmp bpp = \"$$BitmapRef{ClDepth}\"";
 			my $bmpfile = &main::Path_Split('File',$$BitmapRef{Src});
 			my $bmppath = &main::Path_Split('Path',$$BitmapRef{Src});
 			$sourcepath = &main::Path_Chop(&main::Path_MakeRltToBase($mmpdir,$bmppath));
 			$entry .= " sourcepath = \"$sourcepath\"";
 			$entry .= " sourcefile = \"$bmpfile\"/>\n";
		}
   		$ResourcesText .= $entry . "  </aif>\n";

   	}

	
	# --- RSC files, which must come after .MBM files since they may use the .MBG header files
	
	my $ResourceStructRef=&main::ResourceStructRef;
	my @resourcefiles;
	my %resourcetargets;

	# NOTE: An <rsc/> block is now created for each START RESOURCE blocks LANG entries.  This
	# shouldn't be necessary as <rsc/> blocks support multiple <language/> tags, and the generation
	# of separate localised binaries should be dealt with by the Symbian Resources IDE plugin.
	# However, a defect in the plugin's processing of <rsc/> "targetfile" attributes means that is
	# doesn't correctly generate separate localised binaries with per-LANG extensions.

	my %headerProcessed;

	foreach my $ResourceRef (@$ResourceStructRef) {
		my $fullsource=$$ResourceRef{Source};
		my $rssfile=&main::Path_Split('File', $fullsource);
		my $rsspath=&main::Path_Split('Path', $fullsource);
		my $entry = "  <rsc sourcefile = \"$rssfile\"";
		$entry .= " targetpath = \"".&main::Path_Chop($$ResourceRef{TrgPath})."\"";
		
		#############################################################
		# if CW version is 3.1 or greater, add TARGET file if present
		# tkelly 4-May-05
		if ($CW_major_version >= 3.1)
		{
			my $trgfile=&main::Path_Split('File',$$ResourceRef{Trg}); 
			$entry .= " targetfile = \"$trgfile\"\n"; #tk
		}
		##############################################################
		if ((defined $$ResourceRef{Hdr}) && (!$headerProcessed{$fullsource}))
			{
			$entry .= " header = \"true\"";
			$headerProcessed{$fullsource} = 1;
			}
		else
			{
			$entry .= " header = \"false\"";
			}
		my $sourcepath=&main::Path_Chop(&main::Path_MakeRltToBase($mmpdir,$rsspath));
		$entry .= " sourcepath = \"$sourcepath\">\n";
		# ignore the UidList for now..
		$resourcetargets{$fullsource} = $$ResourceRef{TrgPath}.&main::Path_Split('Base', $rssfile);

		$entry .= "    <language id = \"$$ResourceRef{Lang}\"/>\n";
		push @resourcefiles, $entry;
		push @ResourceDownloadList, $resourcetargets{$fullsource}.".R".$$ResourceRef{Lang};
	}

 	foreach my $resourceEntry (@resourcefiles) {	
 			$ResourcesText .= $resourceEntry . "  </rsc>\n";
 			}

	# --- If required, generate .resources file per platform
	
	if ($ResourcesText ne "")
		{
		my $resourcesfile = "$BaseTrg$Plat.resources";
		&main::CreateExtraFile("${ExtraFilesPath}$resourcesfile", "<resources>\n$ResourcesText</resources>\n");
		addFile($resourcesfile, "Text", "", "", "Resources");
		}
		
	# Build the rest of the file list

	if ($BasicTrgType!~/^LIB$/o)
		{
		addFile($FirstLib, "Library", $debug, "$Bld\\");	# static library, build-specific
		}
	
	my $file;
	foreach $file (@SrcList)
		{
		# ensure the case of the extension is what GCC expects
		$file =~ s/\.CPP$/.cpp/i;
		$file =~ s/\.C$/.c/i;
		$file =~ s/\.s$/.S/i;
		my $srcfile=&main::Path_Split('File',$file);
		addFile($srcfile, "Text", $debug, "");
		}

	# If required, add the uid.cpp file so that it appears after all other source files in the link order
	if (defined $UIDFile)
		{
		addFile($UIDFile, "Text", "", "");
		}
	
	if ($Plat ne "GCCE" && !$CustGCCE)
	{
	# linking with GCCE, Runtime libs need to be at the end to match with make, otherwise evalid can fail.
	foreach $file (@RuntimeLibs)
		{
		next if ( $file eq $FirstLib );		#skip if file equals FirstLib.
		addFile($file, "Library", $debug, "$Bld\\"); # static library, build specific
		}
	}
				
	foreach $file (@StatLibList)
		{
		next if ( $file eq $FirstLib );		#skip if file equals FirstLib.
		addFile($file, "Library", $debug, "$Bld\\"); # static library, build specific
		}
	foreach $file (@ASSPLibList, @LibList)
		{
		next if ( $file eq $FirstLib );		#skip if file equals FirstLib.
		if ($Plat eq "GCCE" or $ABIV2 or $CustGCCE) {
			$file =~ s/\.LIB$/.DSO/;
			$file =~ s/\.lib$/.dso/;
			}
		addFile($file, "Library", $debug, "");
		}
		
	if ($Plat eq "GCCE" || $CustGCCE)
	{
		foreach $file (@RuntimeLibs)
		{
			next if ( $file eq $FirstLib );		#skip if file equals FirstLib.
			
			#change to prevent multiple libs being listed when they are shared between targets.
			if ($file eq "usrt2_2.lib" || $file eq "EDLLSTUB.LIB") {
				addFile($file, "Library", $debug, "$Bld\\"); # static library, build specific
			}
			else {
				addFile($file, "Library", $debug, ""); # static library, build non-specific
			}
		}
	}
		
	if ($Plat eq "WINSCW")
		{
		my $defaults = $ENV{'MWSym2LibraryFiles'};
		# look out for paths?
		foreach $file (@Win32LibList)
			{
			# skip default libs and FirstLib
			next if ( ($defaults =~ /;$file/) || ($file eq $FirstLib) );
			addFile($file, "Library", $debug, "");
			}
		}

	
	# Update the project settings

	$changedsettings{"UserSourceTrees"} = "{}";
	$changedsettings{"UserSearchPaths"} = "{}";
	$changedsettings{"SystemSearchPaths"} = "{}";
	$changedsettings{"Targetname"} = $targetname;

	my $outputdir = $RelPath;
	if ($Plat eq "WINSCW")
		{
		my $trgpath = &main::TrgPath;
		&Winutl_AdjustTargetPath(\$trgpath);
		$outputdir .= $trgpath;
		}
	$changedsettings{"OutputDirectory"} = "{1}".&main::Path_Chop($outputdir);
	$changedsettings{"SymbianInstallationContentSearchLocation"} = "{0}".&main::Path_Chop($RelPath);

	$changedsettings{"SymbianResourcesHeaderFileOutputLocation"} = "{0}".&main::Path_Chop(&main::EPOCIncPath());
	if ($Plat eq "WINSCW")
		{
		$changedsettings{"SymbianResourcesBinaryOutputLocation"} = "{0}".&main::Path_Chop($RelPath);
		}
	else
		{
		$changedsettings{"SymbianResourcesBinaryOutputLocation"} = "{0}".&main::Path_Chop(&main::EPOCDataPath());
		}
		
	if ($Plat eq "WINSCW")
		{
		if ($TrgType eq "EXE")
			{	
			# IDE would do the right thing, but we might as well make sure...
			$changedsettings{"MWRuntimeSettings_HostApplication"} = "{0}$outputdir$Trg";
			}
		else
			{
			$changedsettings{"MWRuntimeSettings_HostApplication"} = "{0}${RelPath}epoc.exe";
			}
		}


	$changedsettings{"SymbianEpocToolsPath"} = "{0}${epocroot}";

	if ($Plat ne "WINSCW")
		{
		my $downloadpath = &main::TrgPath;
		if (&main::EPOCSecurePlatform && $downloadpath !~ /^Z\\sys\\bin\\/)
			{
			my @hrhMacros = &Variant_GetMacroList;
			if (grep /^SYMBIAN_IGNORE_BIN_TARGETPATH\s*$/, @hrhMacros)
				{
				$downloadpath = "Z\\sys\\bin\\";
				}
			}
		$downloadpath =~ s/^Z\\/C:\\/i;
		$changedsettings{"DownloadPath"} = $downloadpath;
		$changedsettings{"FileList"} = "{}";
		}

	my @MacroList;

	@MacroList = &main::MacroList();			
		
	push @MacroList, "__${Plat}__" if ($Plat ne $ABI);
	push @MacroList, "__SUPPORT_CPP_EXCEPTIONS__" if ($Plat eq "WINSCW");
	push @MacroList, "__SUPPORT_CPP_EXCEPTIONS__" if (($Plat eq "ARMV5") || ($Plat eq "ARMV5_ABIV2") || ($Plat eq "ARMV5_ABIV1") || ($Plat eq "GCCE") || $IsPlatCustomization);

	# Symbian Compiler Panel
			
	my $compiler="";
	my $compilerargs="";
	my $macros="";

	if ((($Plat eq "ARMV5") || ($Plat eq "ARMV5_ABIV2") || ($Plat eq "ARMV5_ABIV1") || ($Plat eq "GCCE") || $IsPlatCustomization) && $VariantFile)
		{
		push @MacroList, '__PRODUCT_INCLUDE__=\\"'.&main::Path_Split('File',$VariantFile).'\\"' if $VariantFile;
		}

	foreach (@MacroList)
		{
		$_ =~ s/\s+$//;
		$_ =~ s/^\s+//;
		$macros .= "$_,";
		}

###############################
# WINSCW compilation settings #
###############################

	if ($Plat eq "WINSCW")
		{
		$compiler = "WINSCW";
		$compilerargs .= "-wchar_t off -align 4 -warnings on -w nohidevirtual, nounusedexpr -msgstyle gcc -enum int -str pool ";
		$compilerargs .= "-exc ms ";
		$compilerargs .= "-trigraphs on ";

		if ($Bld =~ /DEB/)
			{
			$compilerargs .= "-O0 ";

			# euser change to apply inlining on the _NAKED functions
			if ($BaseTrg !~ /^EUSER$/oi)
				{
				$compilerargs .= "-inline off ";
				}
			}
		else
			{
			$compilerargs .= "-O4,s ";
			}
			
		if ($Win32StdHeaders || $Win32Resrc ) 
			{
			$macros .= "WIN32,_WINDOWS,";
			# Callisto defect workaround
			# NOTE: persisting with this for consistency
			$compilerargs .= "-stdinc ";
			}
		else
			{
			$compilerargs .= "-nostdinc ";
			}
				
		$compilerargs .= &main::CompilerOption("CW");
		$changedsettings{"Macros"} = $macros;
		if ($VariantFile)
			{
			$changedsettings{"PrefixFile"} = &main::Path_Split('File',$VariantFile);
			}			
		}

#############################
# RVCT compilation settings #
#############################
		
	elsif ((($Plat eq "ARMV5") || ($Plat eq "ARMV5_ABIV2") || ($Plat eq "ARMV5_ABIV1") || $ABIV1 || $ABIV2) && ($CW_major_version >= 3)) #|| $IsPlatCustomization
		{
		
		if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
			$changedsettings{"CompilerXMLDescriptor"} = "ARM RVCT";
		}
		else {
			if(($CW_major_version == 3))
			{ 
				$changedsettings{"CompilerXMLDescriptor"} = "ARM RVCT";
			}
			else
			{
				# RVCT 2.2
				$changedsettings{"CompilerXMLDescriptor"} = "ARM RVCT2_2";
			}
		}	
		if ($Bld =~ /REL$/)
			{
				$compilerargs .= $configdata{"REL_OPTIMISATION"}." ".$configdata{"RUNTIME_SYMBOL_VISIBILITY_OPTION"}.$CCUREL;		
			}
		else
			{
			unless (&main::SrcDbg)
				{
					$compilerargs .= $configdata{"DEBUG_OPTIMISATION"}." ".$configdata{"RUNTIME_SYMBOL_VISIBILITY_OPTION"}.$CCUDEB;		
				}
			}
		$changedsettings{"PrefixFile"} = &main::Path_Split('File',$PrefixFile);
		}

############################
# GCC compilation settings #
############################
		
	elsif ($Plat eq "ARM4")
		{
		$compiler = "ARM";
			
		if ($Bld =~ /REL$/)
			{
			$compilerargs .= "-s -fomit-frame-pointer -O "
			}
		else
			{
			unless (&main::SrcDbg)
				{
				$compilerargs .= "-O ";
				}
			}
		
		if ($ABI eq "ARMI")
			{
			$compilerargs .= "-march=armv4t -mthumb-interwork";
			}
		elsif ($ABI eq "ARM4T")
			{
			if (&main::BuildAsARM)
				{
				$compilerargs .= "-march=armv4t -mthumb-interwork";
				}
			elsif ($SystemTrg)
				{
				$compilerargs .= "-march=armv4";

				unless (&main::PlatABI eq "ARM4")
					{
					$compilerargs .= "t";
					}
				}
			else
				{
				$compiler = "THUMB";
				$compilerargs .= "-mthumb-interwork";
				$macros .= "__MARM_THUMB__,";
				}
			}
		elsif ($ABI eq "ARM4")
			{
			$compilerargs .= "-march=armv4";

			unless (&main::PlatABI eq "ARM4")
				{
				$compilerargs .= "t";
				}
			}
		elsif ($ABI eq "THUMB")
			{
			$compiler = "THUMB";
			$compilerargs .= "-mthumb-interwork";
			}
	
		if ($VariantFile)
			{
			$changedsettings{"PrefixFile"} = &main::Path_Split('File',$VariantFile);
			}
		}

############################
# GCCE BPABI compilation settings #
############################

		
	elsif ((($Plat eq "GCCE") || $CustGCCE)) # || $IsPlatCustomization) && ($CW_major_version >= 3)) 
		{
		$compiler = "ARM GCCE";
		#Change setting CompilerXMLDescriptor is only good for CW 3.0 and greater.
		$changedsettings{"CompilerXMLDescriptor"} = "ARM GCCE";	
		
		if ($Bld =~ /REL$/)
		{
			$compilerargs .= $configdata{"REL_OPTIMISATION"}." ".$configdata{"RUNTIME_SYMBOL_VISIBILITY_OPTION"}.$CCUREL;
		}
		else
		{
			unless (&main::SrcDbg)
			{
				$compilerargs .= $configdata{"DEBUG_OPTIMISATION"}." ".$configdata{"RUNTIME_SYMBOL_VISIBILITY_OPTION"}.$CCUDEB
			}
		}
		$changedsettings{"PrefixFile"} = &main::Path_Split('File',$PrefixFile);
		}
####################
# General settings #
####################

	$macros =~ s/,$//;
	$compilerargs =~ s/ $//;

	$changedsettings{"Compiler"} = $compiler; # in CW 3.0, "Compiler" no longer exists. This line has no effect on those versions
	$changedsettings{"Arguments"} = $compilerargs;

	# Symbian Linker Panel
	$changedsettings{"LinkOutputFile"} = $Trg;
	
	if ($Plat eq "GCCE" || $CustGCCE || $ABIV2) {
		$changedsettings{"SymbianImportLibrary"} = $ExportLibrary.'.dso';
	}
	else {
		$changedsettings{"SymbianImportLibrary"} = $ExportLibrary.'.lib';
	}
	
	# Template defaults for canDebug/canRun are both "true"
	if ($Bld =~ /REL/)
		{
		$changedsettings{"canDebug"} = "false";
		}

	if ($Plat eq "WINSCW")
		{
		if ($TrgType ne "APP" && $TrgType ne "EXE" && $TrgType ne "EXEDLL" && $TrgType ne "EPOCEXE")
			{
			$changedsettings{"canDebug"} = "false";
			$changedsettings{"canRun"} = "false";
			}
		}
	else
		{
		$changedsettings{"canRun"} = "false";

		if ($TrgType eq "LIB" || $TrgType eq "KLIB")
			{
			$changedsettings{"canDebug"} = "false";
			}
		}
		
		
	$xmlLinkDescriptorDoc = $xmlParser->parsefile ($FindBin::Bin."\\$linkDescriptorTemplate");
	$xmlLinkDescriptorCommandParent = $xmlLinkDescriptorDoc->getElementsByTagName("array",1)->item(0);

	if ($CW_major_version >= 3)
		{
		$xmlLinkDescriptorSymbolParent = $xmlLinkDescriptorDoc->getElementsByTagName("array",1)->item(1);
		$xmlLinkDescriptorDumpFileParent = $xmlLinkDescriptorDoc->getElementsByTagName("array",1)->item(2);
		}
	
	my $linkDescriptorFile = "$BaseTrg$Plat$Bld.cwlink";

	my $copyCommand = 'perl.exe -S ecopyfile.pl ';
	my $deleteCommand = 'cmd.exe /C del ';

	my $tempFilenameRoot = '${var:IMPORT_LIBRARY_NO_EXT}';

	if ($CW_major_version < 3)
		{
		$tempFilenameRoot = $ExportLibrary;
		}	

	my $exportUnfrozenWarningMessage = 'Created "${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).'${var:IMPORT_LIBRARY}" '.
										'from "${target.data}\\'.$tempFilenameRoot.'.def" as EXPORTUNFROZEN specified.';



############################################
# WINSCW library generation and link stage #
############################################

	if ($Plat eq "WINSCW")
		{
		# Generate library
		if ($DefFile and !$NoExportLibrary)
			{
			unless (&main::ExportUnfrozen)
				{
					my $LibLinkAs = ($BasicTrgType=~/^IMPLIB$/io) ? $LinkAs : $Trg;

					$linkCommand = 'perl.exe -S prepdef.pl "${var:DEF_FILE}" "${target.data}\\'.$tempFilenameRoot.'.prep.def"';
					addLinkDescriptorCommand ($linkCommand);

					$linkCommand = 'mwldsym2.exe "${target.data}\\'.$tempFilenameRoot.'.prep.def" -importlib -o "'.
									'${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).'${var:IMPORT_LIBRARY}" -addcommand "out:'.$LibLinkAs.'" -warnings off';
					addLinkDescriptorCommand ($linkCommand);
				}
			}

		if ((2.8 == $CW_major_version) && (0 == $CW_minor_version))
			{
			# For OEM2.8, create a file containing all objects required in the link.  This is used in all
			# calls to mwldsym2 in order to avoid exceeding the Windows command line
			# length in projects containing a large amount of source files
			$linkCommand ='cmd.exe /C echo ${var:LINK_OBJS}>"${target.data}\${output.file.root}.lst"';
			addLinkDescriptorCommand ($linkCommand);
			}
			
		my $stage1linkflags = "";
		my $linkflags = "";
		my $commonLinkFlags = '-msgstyle gcc -stdlib';
		my $libPath = "epoc32\\release\\winscw\\".lc $Bld;
		if ($SystemTrg){
			$commonLinkFlags .=" ${libPath}\\scppnwdl_kern.lib";
		}
		else{
			$commonLinkFlags .=" ${libPath}\\scppnwdl.lib";
		}
		if ($BasicTrgType=~/^(EXE|DLL)$/o) {
					$commonLinkFlags .= ' ${var:FIRST_LIB} '
			}

		foreach my $lib (@Win32LibList)
			{
			my $win32lib = $lib;
			$win32lib = "-l$win32lib" unless ($win32lib =~ /\\/);
			$commonLinkFlags .= " ". lc $win32lib;
			}

		if ($BasicTrgType =~ /^DLL$/o || $TrgType =~ /^EXEXP$/o)
			{
			if ($BaseAddress ne "")
				{
				$commonLinkFlags .= " -imagebase $BaseAddress";
				}
			
			$commonLinkFlags .= ' -noentry -shared';
			}
		elsif ($BasicTrgType =~ /^EXE$/o)
			{
			$commonLinkFlags .= ' -m "?_E32Bootstrap@@YGXXZ"';
			}

		$commonLinkFlags .= ' -subsystem windows';

		if (&main::HeapSize)
			{
			my %HeapSize = &main::HeapSize;
			$commonLinkFlags .= ' -heapreserve='.RoundUp1k($HeapSize{Max}).' -heapcommit='.RoundUp1k($HeapSize{Min}); 
			}

		if ($BasicTrgType =~ /^(DLL|EXE)$/o)
			{
			if ($Bld =~ /DEB$/o)
				{
				$commonLinkFlags .= ' -g';
				}
			}
			
		my $EntrySymbol='';
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o)
			{
			my $Include="";
			if ($BasicTrgType=~/^DLL$/o)
				{
				$Include="-m __E32Dll";
				$EntrySymbol='__E32Dll';
				}
			else
				{
				$Include="-m __E32Startup";
				$EntrySymbol='__E32Startup';
				}
				
			$stage1linkflags = $commonLinkFlags.' ${var:LIBS}'.
				' -o "${target.data}\\${output.file.name}"'. 
				' -export dllexport '.
				$Include.
				' -nocompactimportlib'. 
				' -implib "${target.data}\\${var:IMPORT_LIBRARY}"'.
				' -addcommand "out:${output.file.name}"'.
				' -warnings off';
			}
				
		my $AbsentSubst = "";
		if ($EntrySymbol)
			{
			$AbsentSubst = " -absent $EntrySymbol";
			}

		$linkflags = $commonLinkFlags.' ${var:LIBS}'.
			' -o "${output}\\${output.file.name}"';
			
		if ($Bld=~/REL$/o && $BasicTrgType!~/^LIB$/o)
			{
			# Generate map file for release build executables
			$linkflags .= ' -map "${output}\\${output.file.name}.map"';
			}
		
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o)
			{
			$linkflags .= ' -f "${target.data}\\'.$tempFilenameRoot.'.def"';
			
			if (&main::ExportUnfrozen)
				{
				$linkflags .= ' -implib "${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).'${var:IMPORT_LIBRARY}"'.
					' -addcommand "out:${output.file.name}" -warnings off';
				}
			else
				{
				$linkflags .= ' -noimplib ';
				}
			}
			else
			{
			$linkflags .= ' -noimplib ';
			}

		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o)
			{
			if (($CW_major_version >= 3) ||
				((2.8 == $CW_major_version) && ($CW_minor_version >= 1)))
				{
				# For OEM2.8.1 onwards, make use of ${var:LINK_OBJS_NO_PATH} in order to reduce link
				# command line lengths
				$linkCommand = 'mwldsym2.exe '.$stage1linkflags.' ${var:COMMON_LINK_FLAGS} -l "${target.data}\\ObjectCode" -search ${var:LINK_OBJS_NO_PATH}';				
				}
			else
				{
				$linkCommand = 'mwldsym2.exe '.$stage1linkflags.' ${var:COMMON_LINK_FLAGS} @"${target.data}\${output.file.root}.lst"';
				}

			addLinkDescriptorCommand ($linkCommand, undef, undef, undef, $FirstLib);
				
			$linkCommand = $deleteCommand.'"${target.data}\\${output.file.name}"';
			addLinkDescriptorCommand ($linkCommand);

			my $show_options = 'names,unmangled,verbose';
			$linkCommand = 'mwldsym2.exe -S -show only,'.$show_options.' -o "${target.data}\\'.$tempFilenameRoot.'.inf" "${target.data}\\${var:IMPORT_LIBRARY}"';
			addLinkDescriptorCommand ($linkCommand);

			$linkCommand = 'perl.exe -w -S makedef.pl '.$AbsentSubst.' -Inffile "${target.data}\\'.$tempFilenameRoot.'.inf"';
			if (-e $DefFile)
				{
				$linkCommand .= ' -Frzfile "'.$DefFile.'"';
				}
				
			my $Export;
			my $Ordinal=1;
			foreach $Export (&main::Exports)
				{
				$linkCommand .= " -$Ordinal $Export";
				$Ordinal++;
				}					

			$linkCommand .= ' "${target.data}\\'.$tempFilenameRoot.'.def"';					
			addLinkDescriptorCommand ($linkCommand);
			
			$linkCommand = $deleteCommand.'"${target.data}\\'.$tempFilenameRoot.'.inf"';
			addLinkDescriptorCommand ($linkCommand);
			
			$linkCommand = $deleteCommand.'"${target.data}\\${var:IMPORT_LIBRARY}"';
			addLinkDescriptorCommand ($linkCommand);
			}

		$linkCommand = "mwldsym2.exe ";

		if ($BasicTrgType =~/^LIB$/o)
			{
			$linkCommand .= '-library ';
			}

		if (($CW_major_version >= 3) ||
			((2.8 == $CW_major_version) && ($CW_minor_version >= 1)))
			{
			# For OEM2.8.1 onwards, make use of ${var:LINK_OBJS_NO_PATH} in order to reduce link
			# command line lengths
			$linkCommand .= $linkflags.' ${var:COMMON_LINK_FLAGS} -l "${target.data}\\ObjectCode" -search ${var:LINK_OBJS_NO_PATH}';	
			}
		else
			{
			$linkCommand .= $linkflags.'${var:COMMON_LINK_FLAGS} @"${target.data}\${output.file.root}.lst"';				
			}

		my $warningMessage;
		
		if (&main::ExportUnfrozen)
			{
			$warningMessage = $exportUnfrozenWarningMessage;
			}

		if ($BasicTrgType =~/^LIB$/o)
			{				
			addLinkDescriptorCommand ($linkCommand, undef, undef, undef, undef, $warningMessage);
			}
		else
			{
			addLinkDescriptorCommand ($linkCommand, undef, undef, undef, $FirstLib, $warningMessage);
			}

		if (&Winutl_CopyForStaticLinkage)
			{
			$linkCommand = $copyCommand.
							'"${output}\\${output.file.name}" '.
							'"${var:KIT_EPOCROOT}'.kitRelativePath($RelPath).'${output.file.name}"';
			addLinkDescriptorCommand ($linkCommand, "false", "false");
			}
		}

##########################################
# RVCT library generation and link stage #
##########################################

	elsif ($ABIV1 && ($CW_major_version >= 3)) 
		{
		my $InterWorking = ($ABI eq 'ARMV4') ? "" : "--inter";
			
		# Generate library

		if ($DefFile and !$NoExportLibrary)
			{
			unless (&main::ExportUnfrozen)
				{
				$linkCommand = 'perl.exe -S prepdef.pl "${var:DEF_FILE}" "${target.data}\\'.$tempFilenameRoot.'.prep.def"';
				addLinkDescriptorCommand ($linkCommand);

				$linkCommand = 'def2dll.bat --path="${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).'\\" --bldpath="${target.data}" --import='.$tempFilenameRoot.' '.
					'--deffile="${target.data}\\'.$tempFilenameRoot.'.prep.def" --linkAs='.$LinkAs.' '.$InterWorking;
				addLinkDescriptorCommand ($linkCommand);

				if ($ExtraExportLibrary)
					{
					$linkCommand = $copyCommand.'"${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).'${var:IMPORT_LIBRARY}" '.
									'"${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).$ExtraExportLibrary.'.lib"';
					addLinkDescriptorCommand ($linkCommand,"false", "false");
					}
				}
			}

		return if ($BasicTrgType=~/^IMPLIB$/io);
		

		# Create custom symbols only required in RVCT builds
		my $implibs_no_path_vtblexport = "";

		foreach my $lib (@LibList)
			{
			$implibs_no_path_vtblexport.="$lib(VtblExports.o) ";
			}

		addLinkDescriptorSymbol ('${var:IMPLIBS_NO_PATH_VTBLEXPORT}', $implibs_no_path_vtblexport);

		my $AbsentSubst = '';
		my $EntrySymbol;
		my $Link = '';

		if ($BasicTrgType=~/^DLL$/o) {
			$EntrySymbol = '_E32Dll';
		}
		elsif ($BasicTrgType=~/^EXE$/o) {
			$EntrySymbol = '_E32Startup';
		}
		if ($EntrySymbol) {
			$AbsentSubst = " -absent $EntrySymbol";
		}

		$Link = 'armlink '.$oP.'diag_suppress 6331,6780'.$linkeropts.' ';

		if ($Bld =~ /DEB$/o)
			{
			$Link .= ' '.$oP.'debug';
			}


	    # TARGET *.IN
	    #------------

		# Create "via" file containing all link objects in order to reduce link
		# command line length
		addLinkDescriptorDumpFile ('${var:LINK_OBJS}', '${target.data}\\${output.file.root}_'.$Bld.'_objects.via');
		
		if ($BasicTrgType!~/^LIB$/o) {
			$linkCommand = $Link .' '.$oP.'partial -o ${var:COMMON_LINK_FLAGS} "${target.data}\\${output.file.root}.in" '.$oP.'via "${target.data}\\${output.file.root}_'.$Bld.'_objects.via"';
			addLinkDescriptorCommand ($linkCommand);
		}

		# Perform link

	    if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
	#		reorder the .DEF file taking frozen exports into account if there are any

			$linkCommand = 'perl -S elf2inf.pl -o "${target.data}\\'.$tempFilenameRoot.'.inf" "${target.data}\\${output.file.root}.in"';
			addLinkDescriptorCommand ($linkCommand);

			$linkCommand = 'perl -S makedef.pl '.$AbsentSubst.' -Inf "${target.data}\\'.$tempFilenameRoot.'.inf"';

	    	if (!$DefFile || $NoExportLibrary) {    			
	    		$linkCommand .= ' -ignore_unfrozen_noncallable ';
	    	}
		if (SysTrg()) {
	    		$linkCommand .= ' -SystemTargetType ';
	    	}		
		
		    if (-e $DefFile) {	# effectively "if project frozen ..."
		        $linkCommand .= " -Frzfile \"".'${var:DEF_FILE}'."\"";
		    }
		    # freeze ordinals, a maximum of 2, for polymorphic dlls
		    my $Ordinal;
		    my $Num=1;
		    foreach $Ordinal (&main::Exports) {
		    	$linkCommand .= " -$Num $Ordinal";
			    $Num++;
		    }

		    $linkCommand.= ' "${target.data}\\'.$tempFilenameRoot.'.def"';
		    addLinkDescriptorCommand ($linkCommand);
    
		    my $theDefFile = '"${target.data}\\'.$tempFilenameRoot.'.def"';
		    $theDefFile = '"${var:DEF_FILE}"' if (-e $DefFile && !&main::ExportUnfrozen);

			$linkCommand = 'def2dll.bat'.$AbsentSubst.' --path="${target.data}" --bldpath="${target.data}" --export='.$tempFilenameRoot.' --deffile='.$theDefFile.' --linkAs='.$LinkAs.' '.$InterWorking;
			addLinkDescriptorCommand ($linkCommand);
	    }

        if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
#		generate an export object from the ordered .DEF file
		if (&main::ExportUnfrozen) {
			$linkCommand = 'def2dll.bat --path="${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).'\\" --bldpath="${target.data}" --import='.$tempFilenameRoot.
				' --deffile="${target.data}\\'.$tempFilenameRoot.'.def" --linkAs='.$LinkAs.' '.$InterWorking;
		    addLinkDescriptorCommand ($linkCommand, undef, undef, undef, undef, $exportUnfrozenWarningMessage);
			}
        }

#       get rid of any -symbols produced .map file
        if ($BasicTrgType=~/^(DLL|EXE)/o) {
			$linkCommand = $deleteCommand.'"${output}\\${output.file.name}.map"';
			addLinkDescriptorCommand ($linkCommand, "false", "false", undef, undef, undef, undef, (0,1));
		}

        if ($BasicTrgType=~/^(DLL|EXE)/o) {
	        my $datalinkbase = "0x400000";
	        $datalinkbase = &main::DataLinkAddress if (&main::DataLinkAddress);
		
	        $linkCommand = "$Link ".$oP.'shl '.$oP.'reloc '.$oP.'split '.$oP."rw-base $datalinkbase ".$oP.'noscanlib '."$PlatOpt{Ld}";
	        
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

#		    If platform is a customization, take libs from parent directory.		   
			$linkCommand .= $oP.'entry _E32Dll "'.$FirstLib.'('.$ObjFile.')"';
		    if($IsCustomDll)
		    {
				$linkCommand .=	' "${target.data}\\'.$tempFilenameRoot.'.exp"';
			}
		    
	        } elsif ($BasicTrgType=~/^EXE$/o || $TrgType=~/^EXEXP$/o) {
			    # get the right object file for the entry point
			    my $ObjFile = "UC_EXE_.o" ;
			    if ($FirstLib =~ /KC_EXE/i) {
					$ObjFile = "K_ENTRY_.o";
			    }

			if($IsPlatCustomization) 
			{
				$linkCommand .= $oP.'entry _E32Startup "'.$FirstLib.'('.$ObjFile.')"';
			} 
			else
			{
			    	$linkCommand .= $oP.'entry _E32Startup "'.$FirstLib.'('.$ObjFile.')"';
			}
		    
			    if ($TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
					$linkCommand .= ' "${target.data}\\'.$tempFilenameRoot.'.exp"';
				}
			}

			$linkCommand .= ' -o "${target.data}\\${output.file.name}"';

			$linkCommand .= ' '.$oP.'symbols '.$oP.'list "${output}\\${output.file.name}.map"';
			$linkCommand .= ' "${target.data}\\${output.file.root}.in"';


			if ($BasicTrgType=~/^DLL$/o) { # Add the DLL stub library
				if($IsPlatCustomization) 
				{
				if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
				   $linkCommand .= ' "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\\'.$Bld.'\EDLLSTUB'.$RVCTVersion.'.LIB"';
				}
				else {
				   $linkCommand .= ' "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\\'.$Bld.'\EDLLSTUB.LIB"';
				}
				}
				else
				{
				if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
				   $linkCommand .= ' "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\\'.$Bld.'\EDLLSTUB'.$RVCTVersion.'.LIB"';
				}
				else {
				   $linkCommand .= ' "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\\'.$Bld.'\EDLLSTUB.LIB"';
				}
				}
			}

			$linkCommand .= ' ${var:LIBS}';
			
			
			my $runtimeLibs = "";
			my $StaticRTLib = "usrt".$RVCTVersion;
			
			# use ksrt for system code and for user ARM code
			$StaticRTLib = "ksrt".$RVCTVersion if ($SystemTrg);
			$runtimeLibs .= $StaticRTLib.".lib" unless ($Trg =~ /(U|K)SRT/i || ($BasicTrgType=~/^LIB$/o));
		
			unless ($ArmRT || ($BasicTrgType=~/^LIB$/o)) {
				my $TargLib = "$ExportLibrary.lib";
				$TargLib =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
				unless ($SystemTrg) {
					foreach (@RTLibList) {
						$runtimeLibs .= " ".$_ unless ($_ =~ /$TargLib/i);
					}
			    }
			}

			foreach (@ArmLibList)
				{
				$runtimeLibs.= " ".$_;
				}
			
			addLinkDescriptorSymbol ('${var:RUNTIME_LIBS}', $runtimeLibs);
				
			if($IsPlatCustomization) 
			{
			     $linkCommand .= ' --userlibpath "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\\'.$ParentPlat.'\\'.$Bld.'","${var:KIT_EPOCROOT}\\EPOC32\RELEASE\\'.$ParentPlat.'\LIB" ${var:RUNTIME_LIBS} ${var:IMPLIBS_NO_PATH_VTBLEXPORT}';
			}
			else
				{
				addLinkDescriptorSymbol ('${var:RUNTIME_LIBS}', 'usrt'.$RVCTVersion.'.lib dfpaeabi.lib dfprvct'.$RVCTVersion.'.lib dfprvct'.$RVCTVersion.'-thunk.lib drtaeabi.lib drtaeabi.lib(VtblExports.o) drtrvct'.$RVCTVersion.'.lib');
				}
				
			$linkCommand .= ' --userlibpath "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\\'.$Bld.'","${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\LIB" ${var:RUNTIME_LIBS} ${var:IMPLIBS_NO_PATH_VTBLEXPORT}';
			
			addLinkDescriptorCommand ($linkCommand, undef, undef, undef, $FirstLib);

	    	if ($Bld=~/^U?DEB$/o) {
			$linkCommand = $copyCommand. ' "${target.data}\\${output.file.name}" "${output}\\${output.file.root}.sym"'; 
			addLinkDescriptorCommand ($linkCommand, "false", "false");
	    	}
	
			$linkCommand = 'elftran -version '. &Genutl_VersionToUserString(%Version).' -sid '. &main::SecureId(); 
			if (&main::CompressTarget) {
				$linkCommand .= ' -nocompress ';
			}
			# change - exexps are allowed data, but they look like dlls to elftran....
			if (&main::AllowDllData || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
				$linkCommand.=' -allow';
			}
			if (not &main::CallDllEntryPoints ) {
				$linkCommand.=' -nocall';
			}
			if (&main::DataLinkAddress) {
				$linkCommand.=' -datalinkaddress '.&main::DataLinkAddress;
			}
			if (&main::FixedProcess) {
				$linkCommand.=' -fixed';
			}
			if (&main::HeapSize) {
				my %HeapSize=&main::HeapSize;
				$linkCommand.=' -heap '.$HeapSize{Min}.' '.$HeapSize{Max};
			}
			if (&main::ProcessPriority) {
				$linkCommand.=' -priority '.&main::ProcessPriority;
			}
			if (&main::StackSize) {
				$linkCommand.=' -stack '.&main::StackSize;
			}

			my $i=1;
			foreach (@UidList) {
				$linkCommand.=" -uid$i $_";
				$i++;
			}
			if(&main::VendorId) {
				$linkCommand.=' -vid '.&main::VendorId;
			}

			$linkCommand.=' -fpu '.$floatingpointmodel;

			$linkCommand.=' -capability '.&main::Capability. ' "${target.data}\\${output.file.name}" "${output}\\${output.file.name}"';
		
			addLinkDescriptorCommand ($linkCommand, "false");
        	}
        elsif ($BasicTrgType=~/^LIB$/o) {
			$linkCommand = 'armar '.$oP.'create "${output}\\${output.file.name}" '.$oP.'via "${target.data}\\${output.file.root}_'.$Bld.'_objects.via"'.' '.$archiveropts;
			addLinkDescriptorCommand ($linkCommand);
			}
		}

##############################################
# BPABI library generation and link stage    #
# Assumes use of RVCT 2.2					 #
##############################################

	elsif ($ABIV2 && ($CW_major_version >= 3.1)) {
		
		# prolly don't need this...
		my $InterWorking = ($ABI eq 'ARMV4') ? "" : "--inter";
			
		return if ($BasicTrgType=~/^IMPLIB$/io);
		
		if ($BasicTrgType=~/^LIB$/o) {
			$linkCommand = 'armar --create ${output}\\${output.file.name} ${var:LINK_OBJS} ${var:LIBS}'.' '.$archiveropts;
	        addLinkDescriptorCommand ($linkCommand);
         }
		else
		{
		my $AbsentSubst = '';
		my $EntrySymbol;
		my $Link = '';

		if ($BasicTrgType=~/^DLL$/o) {
			$EntrySymbol = '_E32Dll';
		}
		elsif ($BasicTrgType=~/^EXE$/o) {
			$EntrySymbol = '_E32Startup';
		}
		if ($EntrySymbol) {
			$AbsentSubst = " -absent $EntrySymbol";
		}

		$Link = 'armlink '.$oP.'diag_suppress 6331,6780'.$linkeropts.' ';

		if ($Bld =~ /DEB$/o)
			{
			$Link .= ' '.$oP.'debug';
			}


	    # TARGET *.IN
	    #------------

		# Create "via" file containing all link objects in order to reduce link
		# command line length
		addLinkDescriptorDumpFile ('${var:LINK_OBJS}', '${target.data}\\${output.file.root}_'.$Bld.'_objects.via');
		
		if ($BasicTrgType!~/^LIB$/o) {
			$linkCommand = $Link .' '.$oP.'partial -o ${var:COMMON_LINK_FLAGS} "${target.data}\\${output.file.root}.in" '.$oP.'via "${target.data}\\${output.file.root}_'.$Bld.'_objects.via"';

		}


#       get rid of any -symbols produced .map file
        if ($BasicTrgType=~/^(DLL|EXE)/o) {
			$linkCommand = $deleteCommand.'"${output}\\${output.file.name}.map"';
			addLinkDescriptorCommand ($linkCommand, "false", "false", undef, undef, undef, undef, (0,1));
		}

        if ($BasicTrgType=~/^(DLL|EXE)/o) {
	        my $datalinkbase = "0x400000";
			my $librarylist;
			my $expr;
			@ToolChainLibList = &GetLibList;

			foreach $expr (@ToolChainLibList) {
				$librarylist .= $expr.' ';
			}
	        
  	        $datalinkbase = &main::DataLinkAddress if (&main::DataLinkAddress);
	        $linkCommand = "$Link ".$oP.'bpabi '.$oP.'reloc '.$oP.'split '.$oP.'no_scanlib '.$oP.'datacompressor=off '.$oP."rw-base $datalinkbase "."$PlatOpt{Ld}";
			$linkCommand .= ' --dll --symver_soname --soname '.$LinkAs.' ';
			
			
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

#		    If platform is a customization, take libs from parent directory.		   
		    if($IsCustomDll)
		    {
		    
			$linkCommand .= $oP.'entry _E32Dll "'.$FirstLib.'('.$ObjFile.')"'.
					' "${target.data}\\'.$tempFilenameRoot.'.exp"';
			    
		    }
		    else
		    {
		    # ARMV5 hardcoded here...
		    $linkCommand .= $oP.'entry _E32Dll "'.$FirstLib.'('.$ObjFile.')"';
		    }

	        } elsif ($BasicTrgType=~/^EXE$/o || $TrgType=~/^EXEXP$/o) {
			    # get the right object file for the entry point
			    my $ObjFile = "UC_EXE_.o" ;
			    if ($FirstLib =~ /KC_EXE/i) {
					$ObjFile = "K_ENTRY_.o";
			    }

		    # Should this use $ParentPlat rather than hardcoded ARMV5 dir?
		   $linkCommand .= $oP.'entry _E32Startup "'.$FirstLib.'('.$ObjFile.')"';
		
			}

			$linkCommand .= ' -o "${target.data}\\${output.file.name}"';

			if ($BasicTrgType=~/^DLL$/o) { # Add the DLL stub library
				if($IsPlatCustomization) 
				{
				if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
				   $linkCommand .= ' "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\\'.$ParentPlat.'\\'.$Bld.'\EDLLSTUB'.$RVCTVersion.'.LIB"';
				}
				else {
				   $linkCommand .= ' "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\\'.$ParentPlat.'\\'.$Bld.'\EDLLSTUB.LIB"';
				}
				}
				else
				{
				if ($RVCTMajorVersion == 2 && $RVCTMinorVersion < 2) {
				   $linkCommand .= ' "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\\'.$Bld.'\EDLLSTUB'.$RVCTVersion.'.LIB"';
				}
				else {
				   $linkCommand .= ' "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\\'.$Bld.'\EDLLSTUB.LIB"';
				}
				}
			}

			$linkCommand .= ' ${var:LIBS}';
			
			
			my $runtimeLibs = "";
			my $StaticRTLib = "usrt".$RVCTVersion;
			
			# use ksrt for system code and for user ARM code
			$StaticRTLib = "ksrt".$RVCTVersion if ($SystemTrg);
			$runtimeLibs .= $StaticRTLib.".lib" unless ($Trg =~ /(U|K)SRT/i || ($BasicTrgType=~/^LIB$/o));
		
			unless ($ArmRT || ($BasicTrgType=~/^LIB$/o)) {
				my $TargLib = "$ExportLibrary.lib";
				$TargLib =~ s/\{(\d|a|b|c|d|e|f){8}\}//i;
				unless ($SystemTrg) {
					foreach (@RTLibList) {
						$runtimeLibs .= " ".$_ unless ($_ =~ /$TargLib/i);
					}
			    }
			}

			foreach (@ArmLibList)
				{
				$runtimeLibs.= " ".$_;
				}
			
			addLinkDescriptorSymbol ('${var:RUNTIME_LIBS}', $runtimeLibs);
				
			$linkCommand .= ' --userlibpath "${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\\'.$Bld.'","${var:KIT_EPOCROOT}\\EPOC32\RELEASE\ARMV5\LIB" '.$oP.'via "${target.data}\\${output.file.root}_'.$Bld.'_objects.via" ${var:RUNTIME_LIBS} '.$librarylist.' ';
			
			addLinkDescriptorCommand ($linkCommand, undef, undef, undef, $FirstLib);

		}
		
		#### Create the .sym file
		$linkCommand = 'cmd.exe /C copy "${target.data}\\${output.file.name}" "${output}\\${output.file.root}.sym"';
		addLinkDescriptorCommand ($linkCommand);
		
		#### copy the project .def file for prep
		if ($DefFile and !$NoExportLibrary)
		{
			unless (&main::ExportUnfrozen)
			{
        	$linkCommand = 'cmd.exe /C copy "${var:DEF_FILE}" "${target.data}\\${var:IMPORT_LIBRARY_NO_EXT}.prep.def"';
			addLinkDescriptorCommand ($linkCommand);
			}
		}
		
		
		#### ELF2E32 POST-LINK COMMAND ####
		# Section needs to be generic for BPABI (e.g. GCCE & ARMV5_ABIV2)
		$linkCommand  = '${var:KIT_EPOCROOT}\\epoc32\\tools\\elf2e32.exe ';		
		
		# Change - exexps are allowed data, but they look like dlls to elftran....
		if (&main::AllowDllData || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			$linkCommand .= ' --dlldata';
		}

		if (&main::DataLinkAddress) {
			$linkCommand .= ' --datalinkaddress=',&main::DataLinkAddress;
		}
		if (&main::FixedProcess) {
			$linkCommand .=	' --fixedaddress';
		}
		
		$linkCommand .= ' --sid='.&main::SecureId();
		$linkCommand .= ' --version='. &Genutl_VersionToUserString(%Version);
		
		if (&main::HeapSize) {
			my %HeapSize=&main::HeapSize;
			$linkCommand.=' --heap '.$HeapSize{Min} .','.$HeapSize{Max};
		}
		
		if (&main::ProcessPriority) {
			$linkCommand .=	' --priority='.&main::ProcessPriority;
		}
		
		if (&main::StackSize) {
			$linkCommand .= ' --stack='.&main::StackSize;
		}
		
		my $i=1;
		foreach (@UidList) {
			$linkCommand .= " --uid$i=$_";
			$i++;
		}
		if (&main::VendorId) {
			$linkCommand .= ' --vid='.&main::VendorId;
		}
		
		$linkCommand .= ' --capability='.&main::Capability;

		# ARMFPU only currently supported for RVCT BPABI builds
		if (&main::ARMFPU && (&main::ARMFPU =~ /^VFPV2$/i)) {
			$linkCommand .= ' --fpu=vfpv2'
		}
		else {
			$linkCommand .= ' --fpu=softvfp'
		}
		
	
		if(($BasicTrgType=~/^DLL/ && $TrgType!~/^DLL/ ) || $TrgType=~/^EXEXP/) {
	 		$linkCommand .= ' --targettype='.$TrgType;
 		}
 		else {
 			$linkCommand .= ' --targettype='.$BasicTrgType;
 		}
		
		$linkCommand .= ' --output="${output}\\${output.file.name}"';
		
		my $warningMessage;
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			if ($DefFile and !$NoExportLibrary) {
				$linkCommand .= ' --definput="${target.data}\\${var:IMPORT_LIBRARY_NO_EXT}.prep.def"';
			}
			$linkCommand .= ' --dso=';
			$linkCommand .= '"${var:KIT_EPOCROOT}\\EPOC32\\RELEASE\\ARMV5\\LIB\\${var:IMPORT_LIBRARY}"';
				
			$linkCommand .= ' --defoutput=';
			$linkCommand .= '"${target.data}\\${var:IMPORT_LIBRARY_NO_EXT}.def"';
				
			if (&main::ExportUnfrozen) {
				$warningMessage = $exportUnfrozenWarningMessage;
				$linkCommand .= ' --unfrozen';
			}
		}
		
		$linkCommand .= ' --elfinput="${target.data}\\${output.file.name}"';
		$linkCommand .= ' --linkas='.$LinkAs;
		#Change - LIB path is hardcoded here...
		$linkCommand .= ' --libpath="${var:KIT_EPOCROOT}\\EPOC32\\RELEASE\\ARMV5\\LIB"';
		
        if ($BasicTrgType=~/^DLL$/o && $TrgType!~/^DLL/){
			my $Export;
			my $Ordinal=1;
			foreach $Export (&main::Exports)
				{
				if ($Ordinal eq 1) {
					$linkCommand .= ' --sysdef=';
				}
				elsif ($Ordinal ne 1) {
					$linkCommand .= ';';
					}
					
				$linkCommand .= "$Export,".$Ordinal;
				$Ordinal++;
				}
		}
		
		addLinkDescriptorCommand ($linkCommand, undef, undef, undef, undef, $warningMessage);
		
		}
		
		# copy def file output
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			$linkCommand = 'cmd.exe /C copy  "${target.data}\\${var:IMPORT_LIBRARY_NO_EXT}.def" "${Project}\${var:IMPORT_LIBRARY_NO_EXT}.def"';
			addLinkDescriptorCommand($linkCommand);
		}
		
		# copy the import lib (dso) created
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			if ($DefFile and !$NoExportLibrary) {
				$linkCommand = 'cmd.exe /C copy ${var:KIT_EPOCROOT}\\EPOC32\\RELEASE\\ARMV5\\LIB\\${var:IMPORT_LIBRARY} ${var:KIT_EPOCROOT}\\EPOC32\\RELEASE\\ARMV5\\LIB\\${output.file.root}.dso';
			}
			addLinkDescriptorCommand($linkCommand);
		}
	
	}
			
#########################################
# GCC library generation and link stage #
#########################################

	elsif ($Plat eq "ARM4")
		{
		# Generate library

		if ($DefFile and !$NoExportLibrary)
			{
			unless (&main::ExportUnfrozen)
				{
				$linkCommand = 'perl.exe -S prepdef.pl "${var:DEF_FILE}" "${target.data}\\'.$tempFilenameRoot.'.prep.def"';
				addLinkDescriptorCommand ($linkCommand);

				$linkCommand = "$Dlltool $PlatOpt{Dlltool}".' --output-lib "${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).'${var:IMPORT_LIBRARY}" --def "'.
						'${target.data}\\'.$tempFilenameRoot.'.prep.def" --dllname "'.$LinkAs.'"';
				addLinkDescriptorCommand ($linkCommand);

				if ($ExtraExportLibrary)
					{
					$linkCommand = $copyCommand.'"${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).'${var:IMPORT_LIBRARY}" '.
									'"${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).$ExtraExportLibrary.'.lib"';
					addLinkDescriptorCommand ($linkCommand,"false", "false");
					}

				foreach (@CompatibleABIs)
					{
					$linkCommand = "$Dlltool $ABIDlltool{$_}".' --output-lib "${var:KIT_EPOCROOT}'.kitRelativePath($ABILibPath{$_}).'UREL\\${var:IMPORT_LIBRARY}" --def "'.
							'${target.data}\\'.$tempFilenameRoot.'.prep.def" --dllname "'.$LinkAs.'"';
					addLinkDescriptorCommand ($linkCommand);

					if ($ExtraExportLibrary)
						{
						$linkCommand = $copyCommand.'"${var:KIT_EPOCROOT}'.kitRelativePath($ABILibPath{$_}).'UREL\\${var:IMPORT_LIBRARY}" '.
										'"${var:KIT_EPOCROOT}'.kitRelativePath($ABILibPath{$_}).'UREL\\'.$ExtraExportLibrary.'.lib"';
						addLinkDescriptorCommand ($linkCommand,"false", "false");
						}
					}
				}
			}


		# TARGET *.IN
		#------------
		$linkCommand = $deleteCommand.'"${target.data}\\${output.file.root}.in"';
		addLinkDescriptorCommand ($linkCommand, "false", "false", undef, undef, undef, undef, (0,1));
				
		$linkCommand = 'ar.exe cr "${target.data}\\${output.file.root}.in" ${var:LINK_OBJS}';
		addLinkDescriptorCommand ($linkCommand);
		

		# Perform Link

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
			$linkCommand = "$Dlltool $PlatOpt{Dlltool} --output-def ".'"${target.data}\\'.$tempFilenameRoot.'.inf" "${target.data}\\${output.file.root}.in"';

			foreach (@StatLibList) {
				$linkCommand .= ' "${var:KIT_EPOCROOT}'.kitRelativePath($StatLinkPath).$_.'"';
			}

			addLinkDescriptorCommand ($linkCommand);

	#		reorder the .DEF file taking frozen exports into account if there are any
	#			call perl on the script here so nmake will die if there are errors - this doesn't happen if calling perl in a batch file
			$linkCommand = 'perl.exe -S makedef.pl -Deffile "${target.data}\\'.$tempFilenameRoot.'.inf" '.$AbsentSubst;
			if (-e $DefFile) { # effectively "if project frozen ..."
				$linkCommand .= " -Frzfile \"".'${var:DEF_FILE}'."\"";
			}
			# freeze ordinals, a maximum of 2, for polymorphic dlls
			my $Ordinal;
			my $Num=1;
			foreach $Ordinal (&main::Exports) {
				$linkCommand .= " -$Num $Ordinal";
				$Num++;
			}
			$linkCommand .= ' "${target.data}\\'.$tempFilenameRoot.'.def"';
			addLinkDescriptorCommand ($linkCommand);

	#		delete the unordered definition file
			$linkCommand = $deleteCommand.'"${target.data}\\'.$tempFilenameRoot.'.inf"';
			addLinkDescriptorCommand ($linkCommand, "false", "false");

	#		generate an export object from the ordered .DEF file
			$linkCommand = "$Dlltool $PlatOpt{Dlltool} --def".' "${target.data}\\'.$tempFilenameRoot.'.def"'.
				' --output-exp "${target.data}\\'.$tempFilenameRoot.'.exp"'.
				" --dllname \"$LinkAs\"";

			my $warningMessage;
				
			if (&main::ExportUnfrozen) {
				$warningMessage = $exportUnfrozenWarningMessage;
				$linkCommand .= ' --output-lib "${var:KIT_EPOCROOT}'.kitRelativePath($LibPath).'${var:IMPORT_LIBRARY}"';
			}
			
			addLinkDescriptorCommand ($linkCommand, undef, undef, undef, undef, $warningMessage);
		}

	#	call ld to do base relocations (and dll exports)
		if ($BasicTrgType=~/^(DLL|EXE)/o) {
			$linkCommand = "$Link $PlatOpt{Ld} -s";	
			if ($BasicTrgType=~/^DLL$/o) {
				$linkCommand .= " $PlatOpt{Entry} $EntrySymbol -u $EntrySymbol ".'"${target.data}\\'.$tempFilenameRoot.'.exp" --dll ';
			}
			elsif ($BasicTrgType=~/^EXE$/o) {
				$linkCommand .= " $PlatOpt{Entry} $EntrySymbol -u $EntrySymbol ";
			}
	#		--whole-archive is required here apparently because of a defect in the gcc toolchain
	#		the flag can probably be removed with a later version of gcc

			$linkCommand .= '--base-file "${target.data}\\${output.file.root}.bas" -o "${target.data}\\${output.file.name}" '.
				'${var:FIRST_LIB} --whole-archive "${target.data}\\${output.file.root}.in" '.
				"--no-whole-archive";
			$linkCommand .= ' ${var:COMMON_LINK_FLAGS} ${var:LIBS}';
			addLinkDescriptorCommand ($linkCommand, undef, undef, undef, $FirstLib);

	#		delete temporary files
			if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
				$linkCommand = $deleteCommand.'"${target.data}\\'.$tempFilenameRoot.'.exp"';
				addLinkDescriptorCommand ($linkCommand, "false", "false", undef, undef, undef, undef, (0,1));
			}
			$linkCommand = $deleteCommand.'"${target.data}\\${output.file.name}"';
			addLinkDescriptorCommand ($linkCommand, "false", "false");

	#		call dlltool to do base relocations (and dll exports)
			$linkCommand = "$Dlltool $PlatOpt{Dlltool} ";
			if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
				$linkCommand .= '--def "${target.data}\\'.$tempFilenameRoot.'.def" '.
					"--dllname \"$LinkAs\" ";
			}
			$linkCommand .= '--base-file "${target.data}\\${output.file.root}.bas" '.
				'--output-exp "${target.data}\\'.$tempFilenameRoot.'.exp" ';
			addLinkDescriptorCommand ($linkCommand);

	#		delete temporary files
			$linkCommand = $deleteCommand.'"${target.data}\\${output.file.root}.bas"';
			addLinkDescriptorCommand ($linkCommand, "false", "false");

	#		call ld to link the target
			$linkCommand = "$Link $PlatOpt{Ld}";
			if ($Bld=~/^U?REL$/o) {
				$linkCommand .= " -s";
			}
			if ($BasicTrgType=~/^DLL$/o) {
				$linkCommand .= " $PlatOpt{Entry} $EntrySymbol -u $EntrySymbol --dll ";
			}
			elsif ($BasicTrgType=~/^EXE$/o) {
				$linkCommand .= " $PlatOpt{Entry} $EntrySymbol -u $EntrySymbol ";
			}
	#		--whole-archive is required here apparently because of a defect in the gcc toolchain
	#		the flag can probably be removed with a later version of gcc
			$linkCommand .= '"${target.data}\\'.$tempFilenameRoot.'.exp" '.
				'-Map "${output}\\${output.file.name}.map" -o "${target.data}\\${output.file.name}" '.
				'${var:FIRST_LIB} --whole-archive "${target.data}\\${output.file.root}.in" '.
				"--no-whole-archive";
			$linkCommand .= ' ${var:LIBS}';

			if ($BasicTrgType=~/^LIB$/o) {				
			addLinkDescriptorCommand ($linkCommand, undef, undef, undef, undef);
			}
			else {
			addLinkDescriptorCommand ($linkCommand, undef, undef, undef, $FirstLib);
			}

	#		delete temporary files
			$linkCommand = $deleteCommand.'"${target.data}\\'.$tempFilenameRoot.'.exp"';
			addLinkDescriptorCommand ($linkCommand, "false", "false");

			if ($Bld=~/DEB$/o) {
				$linkCommand = $Objcopy.' -X "${target.data}\\${output.file.name}" "${output}\\${output.file.root}.sym"';
				addLinkDescriptorCommand ($linkCommand);
			}

			$linkCommand = "petran.exe $PlatOpt{Petran} -version ". &Genutl_VersionToUserString(%Version). " -sid ". &main::SecureId(). ' "${target.data}\\${output.file.name}" "${output}\\${output.file.name}" ';

			if (&main::CompressTarget) {
				$linkCommand .= ' -nocompress';
			}
			if (&main::AllowDllData) {
				$linkCommand .= ' -allow';
			}
			if (not &main::CallDllEntryPoints) {
				$linkCommand .= ' -nocall';
			}
			if (&main::DataLinkAddress) {
				$linkCommand .= ' -datalinkaddress '.&main::DataLinkAddress;
			}
			if (&main::FixedProcess) {
				$linkCommand .= ' -fixed';
			}
			if (&main::HeapSize) {
				my %HeapSize=&main::HeapSize;
				$linkCommand .= ' -heap '.$HeapSize{Min}.' '.$HeapSize{Max};
			}
			if (&main::ProcessPriority) {
				$linkCommand .= ' -priority '.&main::ProcessPriority;
			}
			if (&main::StackSize) {
				$linkCommand .= ' -stack '.&main::StackSize;
			}
			my $i=1;
			foreach (@UidList) {
				$linkCommand .= " -uid$i $_";
				$i++;
			}
				
			$linkCommand .= ' -capability '.&main::Capability;
			
			if (&main::VendorId) {
			$linkCommand .= ' -vid '.&main::VendorId;
			}
		
			addLinkDescriptorCommand ($linkCommand, "false");
			
			$linkCommand = $deleteCommand.'"${target.data}\\${output.file.name}"';
			addLinkDescriptorCommand ($linkCommand, "false", "false");
		}
		elsif ($BasicTrgType=~/^LIB$/o) {
			$linkCommand = $copyCommand.'"${target.data}\\${output.file.root}.in" "${var:KIT_EPOCROOT}'.kitRelativePath($StatLinkPath).'${output.file.name}"';
			addLinkDescriptorCommand ($linkCommand,"false", "false");
		}
	}
	
###############################################
# GCCE library generation and link stage      #
# GCCE only supported for CW 3.1 and greater #
###############################################

	elsif ($Plat eq "GCCE" || $CustGCCE)
	{	
	
        if ($BasicTrgType=~/^LIB$/o) {
	        $linkCommand = 'ar cr ${output}\\${output.file.name} ${var:LINK_OBJS} ${var:LIBS}';
	        addLinkDescriptorCommand ($linkCommand);
         }
         
		elsif ($BasicTrgType=~/^(DLL|EXE)/o) {
        
        $linkCommand = 'arm-none-symbianelf-ld';
        my $GCCE_LibGCCPath = ' -L';
        $GCCE_LibGCCPath .= '"'.GetGCCELibPath("-print-file-name=libsupc++.a").'"';
        $GCCE_LibGCCPath .= ' -L';
        $GCCE_LibGCCPath .= '"'.GetGCCELibPath("-print-libgcc-file-name").'"';
		$linkCommand .= $GCCE_LibGCCPath;
		$linkCommand .=	' ${var:COMMON_LINK_FLAGS}';
		$linkCommand .= ' --target1-abs --no-undefined -nostdlib -Ttext 0x8000 -Tdata 0x400000';
		$linkCommand .= ' -shared --default-symver -soname '.$LinkAs." ";
		
		if ($Bld=~/REL$/o) {
			$linkCommand .= ' -Map "${output}\\${output.file.name}.map"';
		}
		
		if ($BasicTrgType=~/^DLL$/o)
			{
			$linkCommand .= ' --entry _E32Dll -u _E32Dll';
			}
		elsif ($BasicTrgType=~/^EXE$/o)
			{
			$linkCommand .= ' --entry _E32Startup -u _E32Startup';
			}		

		$linkCommand .= ' ${var:FIRST_LIB}';
		$linkCommand .= ' -o "${target.data}\\${output.file.name}" ${var:LINK_OBJS} ${var:LIBS}';
		$linkCommand .= ' -lsupc++ -lgcc'.' '.$linkeropts; 
		addLinkDescriptorCommand ($linkCommand, undef, undef, undef, $FirstLib);
		
		
		$linkCommand = 'cmd.exe /C copy "${target.data}\\${output.file.name}" "${output}\\${output.file.root}.sym"';
		addLinkDescriptorCommand ($linkCommand);
		
		# copy the project .def file for prep
		if ($DefFile and !$NoExportLibrary)
		{
			unless (&main::ExportUnfrozen)
			{
        	$linkCommand = 'cmd.exe /C copy "${var:DEF_FILE}" "${target.data}\\${var:IMPORT_LIBRARY_NO_EXT}.prep.def"';
			addLinkDescriptorCommand ($linkCommand);
			}
		}
		
		$linkCommand  = '${var:KIT_EPOCROOT}\\epoc32\\tools\\elf2e32.exe ';		
		
		# Change - exexps are allowed data, but they look like dlls to elftran....
		if (&main::AllowDllData || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			$linkCommand .= ' --dlldata';
		}

		if (&main::DataLinkAddress) {
			$linkCommand .= ' --datalinkaddress=',&main::DataLinkAddress;
		}
		if (&main::FixedProcess) {
			$linkCommand .=	' --fixedaddress';
		}
		
		$linkCommand .= ' --sid='.&main::SecureId();
		
		if (&main::HeapSize) {
			my %HeapSize=&main::HeapSize;
			$linkCommand.=' --heap '.$HeapSize{Min} .','.$HeapSize{Max};
		}
		
		if (&main::ProcessPriority) {
			$linkCommand .=	' --priority='.&main::ProcessPriority;
		}
		
		if (&main::StackSize) {
			$linkCommand .= ' --stack='.&main::StackSize;
		}
		
		my $i=1;
		foreach (@UidList) {
			$linkCommand .= " --uid$i=$_";
			$i++;
		}
		if (&main::VendorId) {
			$linkCommand .= ' --vid='.&main::VendorId;
		}
		
		$linkCommand .= ' --capability='.&main::Capability;
		
	
		if(($BasicTrgType=~/^DLL/ && $TrgType!~/^DLL/ ) || $TrgType=~/^EXEXP/) {
	 		$linkCommand .= ' --targettype='.$TrgType;
 		}
 		else {
 			$linkCommand .= ' --targettype='.$BasicTrgType;
 		}
		
		$linkCommand .= ' --output="${output}\\${output.file.name}"';
		
		my $warningMessage;
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			if ($DefFile and !$NoExportLibrary) {
				$linkCommand .= ' --definput="${target.data}\\${var:IMPORT_LIBRARY_NO_EXT}.prep.def"';
			}
			$linkCommand .= ' --dso=';
			$linkCommand .= '"${var:KIT_EPOCROOT}\\EPOC32\\RELEASE\\ARMV5\\LIB\\${var:IMPORT_LIBRARY}"';
				
			$linkCommand .= ' --defoutput=';
			$linkCommand .= '"${target.data}\\${var:IMPORT_LIBRARY_NO_EXT}.def"';
				
			if (&main::ExportUnfrozen) {
				$warningMessage = $exportUnfrozenWarningMessage;
				$linkCommand .= ' --unfrozen';
			}
		}
		
		$linkCommand .= ' --elfinput="${target.data}\\${output.file.name}"';
		$linkCommand .= ' --linkas='.$LinkAs;
		#Change - LIB path is hardcoded here...
		$linkCommand .= ' --libpath="${var:KIT_EPOCROOT}\\EPOC32\\RELEASE\\ARMV5\\LIB"';
		
        if ($BasicTrgType=~/^DLL$/o && $TrgType!~/^DLL/){
			my $Export;
			my $Ordinal=1;
			foreach $Export (&main::Exports)
				{
				if ($Ordinal eq 1) {
					$linkCommand .= ' --sysdef=';
				}
				elsif ($Ordinal ne 1) {
					$linkCommand .= ';';
					}
					
				$linkCommand .= "$Export,".$Ordinal;
				$Ordinal++;
				}
		}
		
		addLinkDescriptorCommand ($linkCommand, undef, undef, undef, undef, $warningMessage);
				
		} # end...elsif if ($BasicTrgType=~/^(DLL|EXE)/o)
		
		# copy def file output
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			$linkCommand = 'cmd.exe /C copy  "${target.data}\\${var:IMPORT_LIBRARY_NO_EXT}.def" "${Project}\${var:IMPORT_LIBRARY_NO_EXT}.def"';
			addLinkDescriptorCommand($linkCommand);
		}
		
		# copy the import lib (dso) created
		if ($BasicTrgType=~/^DLL$/o || $TrgType=~/^EXEXP$/o || $TrgType=~/^EXEDLL$/o) {
			if ($DefFile and !$NoExportLibrary) {
				$linkCommand = 'cmd.exe /C copy ${var:KIT_EPOCROOT}\\EPOC32\\RELEASE\\ARMV5\\LIB\\${var:IMPORT_LIBRARY} ${var:KIT_EPOCROOT}\\EPOC32\\RELEASE\\ARMV5\\LIB\\${output.file.root}.dso';
			}
			addLinkDescriptorCommand($linkCommand);
		}
		
	
	} # end  GCCE link stage... elsif ($Plat eq "GCCE")
	
	
	if ($addHeaders)
		{
		# Ideally we would do this for all targets, both UREL and UDEB.  This would,
		# however, be very slow - so we just do it for the first target we come to.
		my $cpp = &PreprocessorToUseExe();
		my $cppCommandLine = "$cpp.EXE -M -MG -nostdinc ";
		my $preInclude = "";

		if (($Plat eq "ARMV5" || $Plat eq "ARMV5_ABIV2" || $Plat eq "ARMV5_ABIV1" || $Plat eq "GCCE" || $IsPlatCustomization) && $PrefixFile)
			{
			$preInclude = $PrefixFile;
			$preInclude =~ s/^.*://;
			}
		elsif($VariantFile)
			{	    
		    $preInclude = $VariantFile;
			}

		$cppCommandLine .= '-include '.Path_RltToWork($preInclude).' ' if $preInclude;

		foreach (&main::UserIncPaths, &main::SysIncPaths)
			{
			$cppCommandLine .= '-I '.&Path_Chop(&Path_RltToWork($_)).' ';
			}

		foreach (@MacroList)
			{
			#ARMCC requires escaped '"', but CPP doesn't like them
			s/\\\"/\"/g if /^__PRODUCT_INCLUDE__/; 
			
			$cppCommandLine .= '-D'.$_.' ';
			}
		
		my $SourceStructRef=&main::SourceStructRef;
		my %localIncludes;

		foreach my $SourceRef (@$SourceStructRef)
	   		{
			$file = Path_RltToWork(($$SourceRef{SrcPath}.$$SourceRef{CurFile}));

			# ensure the case of the extension is what GCC expects
			$file =~ s/\.CPP$/.cpp/i;
			$file =~ s/\.C$/.c/i;
			$file =~ s/\.s$/.S/i;

			open CPPPIPE,$cppCommandLine.$file." |" or die "ERROR: Can't invoke CPP.EXE\n";

			while (<CPPPIPE>)
				{
				#convert any Unix slashes found in CPP output to DOS slashes
				s/\//\\/g;	
				while (/\.(\.\\|\\){1}\S+\.(hrh|h|inl){1}/gi)
					{
					my $file = $&;
					my $filePath = &main::Path_Split('Path',$file);
						
					# Ignore files that are clearly not local to the project
					next if ($filePath =~ /\\epoc32\\/i);

					# Ignore files that are #included with intermediate directories -
					# we can't guarantee these will be on an Access Path.
					next if ($filePath =~ /\w+\\\.\./);

					# Finally confirm that the file we have is definitely on an Access Path
					my $presentOnAccessPath = 0;
					foreach my $accessPath (&main::UserIncPaths, &main::SysIncPaths)
						{
						my $accessPathCompare = $accessPath;
						$accessPathCompare =~ s/\\/_/g;

						my $filePathCompare = $filePath;
						$filePathCompare =~ s/(\.\\|\.\.\\)//g;
						$filePathCompare =~ s/\\/_/g;

						$presentOnAccessPath = 1 if ($accessPathCompare =~ /$filePathCompare$/i);
						}
					next if (!$presentOnAccessPath);

					# Maintain availability of original case of filename using a lc keyed hash
					my $localInclude = &main::Path_Split('Base',$file).&main::Path_Split('Ext',$file);					
					$localIncludes{lc ($localInclude)} = $localInclude unless (!-e $file);
					}
				}
			}

		foreach my $localInclude (sort keys %localIncludes)
			{
			addFile($localIncludes{$localInclude}, "Text", 0, "", "Headers");
			}

		$addHeaders = 0;
		}


	# Add DOCUMENT specified files that we know we can add - we only add these for one target,
	# as they should be identical through-out

	if ($addDocuments)
		{
		foreach my $document (@DocList)
			{
			# Only add files as Documents if they haven't already been added to
			# the target (it's not possible to have duplicate entries) and if they
			# have an extension we know about.

			next if (grep (/$document/i, @addedFiles));

			my $extension = $document;
			$extension =~ s/^.*\.//;

			next if (!grep (/$extension/i, @compatibleDOCUMENTExtensions));
			
			addFile($document, "Text", "", "", "Documents");
			}

		$addDocuments = 0;
		}


	# Create the link descriptor file

	$xmlLinkDescriptorCommandParent->addText("\n\t\t\t");

	if ($CW_major_version >= 3)
		{
		$xmlLinkDescriptorDumpFileParent->addText("\n\t\t\t");
		}

	&main::CreateExtraFile("${ExtraFilesPath}$linkDescriptorFile", $xmlLinkDescriptorDoc->toString);
	addFile($linkDescriptorFile, "Text", "", "", "Link");	
	
	# Apply the changed settings

	my $settinglist = $xmlTarget->getElementsByTagName("SETTINGLIST",0)->item(0);
	my @settingnodes = $settinglist->getElementsByTagName("SETTING",0);
	foreach my $setting (@settingnodes)
		{
		my $element = $setting->getElementsByTagName("NAME",0)->item(0);
		my $settingname = $element->getFirstChild->getData();
		my $replacement = $changedsettings{$settingname};
		if (defined $replacement) 
			{
			if ($replacement eq "{}")
				{
				if ($settingname eq "UserSearchPaths")
					{
					&addUserSearchPaths($setting);
					}
				elsif ($settingname eq "SystemSearchPaths")
					{
					&addSystemSearchPaths($setting);
					}
				elsif ($settingname eq "UserSourceTrees")
					{
					&addSourceTrees($setting);
					}
				elsif ($settingname eq "FileList")
					{
					&addDownloadFileList($setting, @ResourceDownloadList);
					}
				}
			elsif ($replacement =~ /^{(.+)}(.+)$/)
				{					
				&changePathSetting($setting,$1,$2);
				}
			else
				{
				&changeValue($setting,$replacement);
				}
			}
		}
	}


sub addLinkDescriptorCommand($$;$;$;$;$;$;$;) {		
	my ($linkCommand, $parseStdOut, $parseStdErr, $outputParser, $firstLibProcessing,
	    $linkWarning, $linkInformation, @successCodes) = @_;

	my $structIndent = "\n\t\t\t\t";
	my $settingIndent = "$structIndent\t";
	my $simpleIndent = "$settingIndent\t";
	my $successCodeArrayIndent = $simpleIndent;
	my $successCodeSimpleIndent = "$successCodeArrayIndent\t";

	my $structElement = new XML::DOM::Element($xmlLinkDescriptorDoc,"struct");
	$xmlLinkDescriptorCommandParent->addText("$structIndent");
	$xmlLinkDescriptorCommandParent->appendChild ($structElement);

	my $settingElementTemplate = new XML::DOM::Element($xmlLinkDescriptorDoc,"setting");
	$settingElementTemplate->setAttribute("uuid-alias", ".");
	my $simpleElementTemplate = new XML::DOM::Element($xmlLinkDescriptorDoc,"simple");

	my $settingElement;
	my $simpleElement;

	$settingElement = $settingElementTemplate->cloneNode(0);
	$simpleElement = $simpleElementTemplate->cloneNode(0);
	
	$settingElement->setAttribute ("entry", "linkCommand");
	$structElement->addText ($settingIndent);
	$structElement->appendChild ($settingElement);

	$simpleElement->addText ($linkCommand);

	$settingElement->addText ($simpleIndent);
	$settingElement->appendChild ($simpleElement);
	$settingElement->addText($settingIndent);
	
	if (defined $parseStdOut)
		{
		$settingElement = $settingElementTemplate->cloneNode(0);
		$simpleElement = $simpleElementTemplate->cloneNode(0);

		$settingElement->setAttribute("entry", "parseStdOut");
		$simpleElement->addText($parseStdOut);

		$structElement->addText ($settingIndent);
		$structElement->appendChild ($settingElement);

		$settingElement->addText ($simpleIndent);
		$settingElement->appendChild ($simpleElement);
		$settingElement->addText($settingIndent);
		}
	
	if (defined $parseStdErr)
		{
		$settingElement = $settingElementTemplate->cloneNode(0);
		$simpleElement = $simpleElementTemplate->cloneNode(0);

		$settingElement->setAttribute("entry", "parseStdErr");
		$simpleElement->addText($parseStdErr);

		$structElement->addText ($settingIndent);
		$structElement->appendChild ($settingElement);

		$settingElement->addText ($simpleIndent);
		$settingElement->appendChild ($simpleElement);
		$settingElement->addText($settingIndent);
		}

	if (defined $outputParser)
		{
		$settingElement = $settingElementTemplate->cloneNode(0);
		$simpleElement = $simpleElementTemplate->cloneNode(0);

		$settingElement->setAttribute("entry", "outputParser");
		$simpleElement->addText($outputParser);

		$structElement->addText ($settingIndent);
		$structElement->appendChild ($settingElement);

		$settingElement->addText ($simpleIndent);
		$settingElement->appendChild ($simpleElement);
		$settingElement->addText($settingIndent);
		}

	if (defined $firstLibProcessing)
		{
		$settingElement = $settingElementTemplate->cloneNode(0);
		$simpleElement = $simpleElementTemplate->cloneNode(0);

		$settingElement->setAttribute("entry", "firstLibProcessing");
		$simpleElement->addText($firstLibProcessing);

		$structElement->addText ($settingIndent);
		$structElement->appendChild ($settingElement);

		$settingElement->addText ($simpleIndent);
		$settingElement->appendChild ($simpleElement);
		$settingElement->addText($settingIndent);
		}

	if (defined $linkWarning)
		{
		$settingElement = $settingElementTemplate->cloneNode(0);
		$simpleElement = $simpleElementTemplate->cloneNode(0);

		$settingElement->setAttribute("entry", "linkWarning");
		$simpleElement->addText($linkWarning);

		$structElement->addText ($settingIndent);
		$structElement->appendChild ($settingElement);

		$settingElement->addText ($simpleIndent);
		$settingElement->appendChild ($simpleElement);
		$settingElement->addText($settingIndent);
		}

	if (defined $linkInformation)
		{
		$settingElement = $settingElementTemplate->cloneNode(0);
		$simpleElement = $simpleElementTemplate->cloneNode(0);

		$settingElement->setAttribute("entry", "linkInformation");
		$simpleElement->addText($linkInformation);

		$structElement->addText ($settingIndent);
		$structElement->appendChild ($settingElement);

		$settingElement->addText ($simpleIndent);
		$settingElement->appendChild ($simpleElement);
		$settingElement->addText($settingIndent);
		}

	if (@successCodes)
		{
		$settingElement = $settingElementTemplate->cloneNode(0);
		$settingElement->setAttribute("entry", "successCodes");

		my $arrayElement = new XML::DOM::Element($xmlLinkDescriptorDoc,"array");
		$arrayElement->setAttribute("inheritance", "none");
		
		foreach my $successCode (@successCodes)
			{
			$simpleElement = $simpleElementTemplate->cloneNode(0);
			$simpleElement->addText($successCode);
			$arrayElement->addText ($successCodeSimpleIndent);
			$arrayElement->appendChild ($simpleElement);
			}

		$arrayElement->addText ($successCodeArrayIndent);

		$settingElement->addText ($successCodeArrayIndent);
		$settingElement->appendChild ($arrayElement);
		$settingElement->addText($settingIndent);

		$structElement->addText ($settingIndent);
		$structElement->appendChild ($settingElement);
		}

	$structElement->addText($structIndent);
}


sub addLinkDescriptorSymbol ($$) {
	my ($symbolName, $symbolValue) = @_;
	
	my $structIndent = "\n\t\t\t\t";
	my $settingIndent = "$structIndent\t";
	my $simpleIndent = "$settingIndent\t";

	my $structElement = new XML::DOM::Element($xmlLinkDescriptorDoc,"struct");
	$xmlLinkDescriptorSymbolParent->addText("$structIndent");
	$xmlLinkDescriptorSymbolParent->appendChild ($structElement);

	my $settingElementTemplate = new XML::DOM::Element($xmlLinkDescriptorDoc,"setting");
	$settingElementTemplate->setAttribute("uuid-alias", ".");
	my $simpleElementTemplate = new XML::DOM::Element($xmlLinkDescriptorDoc,"simple");

	my $symbolNameSettingElement;
	my $symbolNameSimpleElement;
	my $symbolValueSettingElement;
	my $symbolValueSimpleElement;

	$symbolNameSettingElement = $settingElementTemplate->cloneNode(0);
	$symbolNameSimpleElement = $simpleElementTemplate->cloneNode(0);
	$symbolValueSettingElement = $settingElementTemplate->cloneNode(0);
	$symbolValueSimpleElement = $simpleElementTemplate->cloneNode(0);

	$symbolNameSettingElement->setAttribute("entry", "symbolName");
	$symbolNameSimpleElement->addText ($symbolName);
	$symbolValueSettingElement->setAttribute("entry", "symbolValue");
	$symbolValueSimpleElement->addText ($symbolValue);

	$symbolNameSettingElement->addText ($simpleIndent);
	$symbolNameSettingElement->appendChild ($symbolNameSimpleElement);
	$symbolNameSettingElement->addText($settingIndent);
	$symbolValueSettingElement->addText ($simpleIndent);
	$symbolValueSettingElement->appendChild ($symbolValueSimpleElement);
	$symbolValueSettingElement->addText ($settingIndent);

	$structElement->addText ($settingIndent);
	$structElement->appendChild ($symbolNameSettingElement);
	$structElement->addText ($settingIndent);
	$structElement->appendChild ($symbolValueSettingElement);
	$structElement->addText ($structIndent);
	}


sub addLinkDescriptorDumpFile ($$) {
	my ($dumpFileContent, $dumpFileName) = @_;
	
	my $structIndent = "\n\t\t\t\t";
	my $settingIndent = "$structIndent\t";
	my $simpleIndent = "$settingIndent\t";

	my $structElement = new XML::DOM::Element($xmlLinkDescriptorDoc,"struct");
	$xmlLinkDescriptorDumpFileParent->addText("$structIndent");
	$xmlLinkDescriptorDumpFileParent->appendChild ($structElement);

	my $settingElementTemplate = new XML::DOM::Element($xmlLinkDescriptorDoc,"setting");
	$settingElementTemplate->setAttribute("uuid-alias", ".");
	my $simpleElementTemplate = new XML::DOM::Element($xmlLinkDescriptorDoc,"simple");

	my $dumpFileContentSettingElement;
	my $dumpFileContentSimpleElement;
	my $dumpFileNameSettingElement;
	my $dumpFileNameSimpleElement;

	$dumpFileContentSettingElement = $settingElementTemplate->cloneNode(0);
	$dumpFileContentSimpleElement = $simpleElementTemplate->cloneNode(0);
	$dumpFileNameSettingElement = $settingElementTemplate->cloneNode(0);
	$dumpFileNameSimpleElement = $simpleElementTemplate->cloneNode(0);

	$dumpFileContentSettingElement->setAttribute("entry", "dumpFileContent");
	$dumpFileContentSimpleElement->addText ($dumpFileContent);
	$dumpFileNameSettingElement->setAttribute("entry", "dumpFileName");
	$dumpFileNameSimpleElement->addText ($dumpFileName);

	$dumpFileContentSettingElement->addText ($simpleIndent);
	$dumpFileContentSettingElement->appendChild ($dumpFileContentSimpleElement);
	$dumpFileContentSettingElement->addText($settingIndent);
	$dumpFileNameSettingElement->addText ($simpleIndent);
	$dumpFileNameSettingElement->appendChild ($dumpFileNameSimpleElement);
	$dumpFileNameSettingElement->addText ($settingIndent);

	$structElement->addText ($settingIndent);
	$structElement->appendChild ($dumpFileContentSettingElement);
	$structElement->addText ($settingIndent);
	$structElement->appendChild ($dumpFileNameSettingElement);
	$structElement->addText ($structIndent);
	}


sub ExtraPlat($) {

# Call PmBld again after reprocessing the MMP file and tweaking various main:: variables

	my ($Plat) = @_;
	
	&main::SetVarsFromMmp($Plat);
	&main::InitLinkPaths();

	foreach (&main::BldList) {
		&main::SetCurBld($_);
		&PMPlatProcessMmp(&main::PlatTxt2D);
		&PMStartBldList;
		&PMBld;
	}

}

sub disconnectNode($) {

# Remove a node from its parent, also removing the following text node (if any)
# The text node is assumed to contain whitespace for file formatting.
 
	my ($node)=@_;

	my $parent = $node->getParentNode;
	my $sibling = $node->getNextSibling;
	$parent->removeChild($node);
	if (defined $sibling && $sibling->getNodeType == TEXT_NODE)
		{
		$parent->removeChild($sibling);
		}
	return $node;
}

sub removeNode($) {

# disconnect the node and dispose of it

	my ($node) = @_;
	&disconnectNode($node);
	$node->dispose;		# setAttribute("disposed",1);
}

sub textElement($$$$) {
	my ($element,$name,$value,$insertionpoint)=@_;

	my $subElement = new XML::DOM::Element($xmlProjectDoc,$name);
	$subElement->appendChild($xmlProjectDoc->createTextNode($value));
	$element->insertBefore($subElement, $insertionpoint);
}

sub addFile($$$$;$) {    

	my ($src, $kind, $debug, $shared, $group) = @_;

	my $linkElement = new XML::DOM::Element($xmlProjectDoc,"FILEREF");

	&textElement($linkElement, "PATHTYPE",   "Name");
	&textElement($linkElement, "PATH",       $src);
	&textElement($linkElement, "PATHFORMAT", "Windows");

	my $fileElement = $linkElement->cloneNode(1);
	$fileElement->setTagName("FILE");
	&textElement($fileElement, "FILEKIND",   $kind);
	&textElement($fileElement, "FILEFLAGS",  "Debug") if ($debug);

	$xmlLinkOrder->appendChild($linkElement);
	$xmlFileList->appendChild($fileElement);

	$xmlLinkOrder->addText("\n");
	$xmlFileList->addText("\n");

	# Accumulate source group information

	my $groupfile = $linkElement->cloneNode(1);
	$groupfile->setAttribute("NAME", "$shared$src");			# convenience - remove this later!

	push (@addedFiles, $src) unless ($kind eq "Documents");

	if ($kind eq "Library")
		{
		$xmlLibGroup->appendChild($groupfile);
		$xmlLibGroup->addText("\n");
		}
	elsif (defined $group)
		{
		if ($group eq "Link")
			{
			$xmlLinkGroup->appendChild($groupfile);
			$xmlLinkGroup->addText("\n");
			}
		elsif ($group eq "Resources")
			{
			$xmlResourcesGroup->appendChild($groupfile);
			$xmlResourcesGroup->addText("\n");
			}
		elsif ($group eq "Root")
			{
			$xmlRootGroup->appendChild($groupfile);
			$xmlRootGroup->addText("\n");
			}
		elsif ($group eq "Headers")
			{
			$xmlHeadersGroup->appendChild($groupfile);
			$xmlHeadersGroup->addText("\n");
			}
		elsif ($group eq "Documents")
			{
			$xmlDocumentsGroup->appendChild($groupfile);
			$xmlDocumentsGroup->addText("\n");
			}
		}
	else
		{
		$xmlSourceGroup->appendChild($groupfile);
		$xmlSourceGroup->addText("\n");
		}
}

sub addGroup($$) {
	my ($grouplist,$name)=@_;

	my $group = new XML::DOM::Element($xmlProjectDoc,"GROUP");
	$grouplist->appendChild($group);
	$grouplist->addText("\n");

	&textElement($group, "NAME", $name);
	$group->addText("\n");
	return $group;
}

sub addSubTarget($$) {
	my ($subtargetlist,$name)=@_;

	my $subtarget = new XML::DOM::Element($xmlProjectDoc,"SUBTARGET");
	$subtargetlist->appendChild($subtarget);
	$subtargetlist->addText("\n");

	&textElement($subtarget, "TARGETNAME", $name);
}

sub addOrderedTarget($$) {
	my ($targetorder,$name)=@_;

	my $orderedtarget = new XML::DOM::Element($xmlProjectDoc,"ORDEREDTARGET");
	$targetorder->appendChild($orderedtarget);
	$targetorder->addText("\n");

	&textElement($orderedtarget, "NAME", $name);
}

sub finaliseProject {

	# Run through the project, removing all unused targets
	# and build up the TARGETORDER list and the "Build All" target

	my $target;
	my $targetname;

	my $xmlSubTargetList = new XML::DOM::Element($xmlProjectDoc,"SUBTARGETLIST");

	my $xmlTargetOrder = new XML::DOM::Element($xmlProjectDoc,"TARGETORDER");
	$xmlTargetOrder->addText("\n");

	my @targets = $xmlProjectDoc->getElementsByTagName("TARGET",1);
	my @emulatortargetnames;
	my @othertargetnames;
	
	foreach $target (@targets)
		{			
		$targetname = $target->getAttribute("NAME");

		if ($targetname eq "")
			{
			&removeNode($target);
			}
		else
			{
			$target->removeAttribute("NAME");

			if ($targetname =~ /^WINSCW/)
				{
				push (@emulatortargetnames, $targetname);
				}
			else
				{
				push (@othertargetnames, $targetname);
				}
			}
		}

	foreach $targetname ((sort @emulatortargetnames), (sort @othertargetnames))
		{
		&addSubTarget($xmlSubTargetList, $targetname);
		&addOrderedTarget($xmlTargetOrder, $targetname);
		}

	# Build the GROUPLIST
	
	my $xmlGroupList = new XML::DOM::Element($xmlProjectDoc,"GROUPLIST");

	# Build the "Root" group

	my %rootfiles;
	my @rootgroups = $xmlProjectDoc->getElementsByTagName("ROOTGROUP",1);
	foreach my $group (@rootgroups)
		{
		$targetname = $group->getAttribute("TARGET");

		my @files = $group->getElementsByTagName("FILEREF",0);
		foreach my $file (@files)
			{
			my $name = $file->getAttribute("NAME");
			if (!defined $rootfiles{$name})
				{
				# first occurrence - add to list
				$rootfiles{$name}=1;
				&textElement($file, "TARGETNAME", $targetname, $file->getFirstChild);
				$file->removeAttribute("NAME");
				&disconnectNode($file);					
				$xmlGroupList->appendChild($file);
				$xmlGroupList->addText("\n");
				}
			}
		&removeNode($group);
		}

	# Build the "Source" group

	my $xmlSourceGroup = &addGroup($xmlGroupList,"Source");
	my %sourcefiles;
	my @sourcegroups = $xmlProjectDoc->getElementsByTagName("SOURCEGROUP",1);
	foreach my $group (@sourcegroups)
		{
		$targetname = $group->getAttribute("TARGET");
		my @files = $group->getElementsByTagName("FILEREF",0);
		foreach my $file (@files)
			{
			my $name = $file->getAttribute("NAME");
			if (!defined $sourcefiles{$name})
				{
				# first occurrence - add to list
				$sourcefiles{$name}=1;
				&textElement($file, "TARGETNAME", $targetname, $file->getFirstChild);
				$file->removeAttribute("NAME");
				&disconnectNode($file);
				$xmlSourceGroup->appendChild($file);
				$xmlSourceGroup->addText("\n");
				}
			}
		&removeNode($group);
		}


	# Build the "Headers" group
			
	my $xmlHeadersGroup;
	my %headerfiles;
	my @headersgroups = $xmlProjectDoc->getElementsByTagName("HEADERSGROUP",1);
	foreach my $group (@headersgroups)
		{
		$targetname = $group->getAttribute("TARGET");
		my @files = $group->getElementsByTagName("FILEREF",0);
		foreach my $file (@files)
			{
			# Only create the "Headers" group if there are some files to add to it
			if (!defined $xmlHeadersGroup)
				{
				$xmlHeadersGroup = &addGroup($xmlGroupList,"Headers");
				}
				
			my $name = $file->getAttribute("NAME");
			if (!defined $headerfiles{$name})
				{
				# first occurrence - add to list
				$headerfiles{$name}=1;
				&textElement($file, "TARGETNAME", $targetname, $file->getFirstChild);
				$file->removeAttribute("NAME");
				&disconnectNode($file);
				$xmlHeadersGroup->appendChild($file);
				$xmlHeadersGroup->addText("\n");
				}
			}
		&removeNode($group);
		}


	# Build the "Resources" group
			
	my $xmlResourcesGroup;
	my %resourcesfiles;
	my @resourcesgroups = $xmlProjectDoc->getElementsByTagName("RESOURCESGROUP",1);
	foreach my $group (@resourcesgroups)
		{
		$targetname = $group->getAttribute("TARGET");
		my @files = $group->getElementsByTagName("FILEREF",0);
		foreach my $file (@files)
			{
			# Only create the main "Resources" groups if there are some files to add
			# to them
			if (!defined $xmlResourcesGroup)
				{
				$xmlResourcesGroup = &addGroup($xmlGroupList,"Resources");
				}
				
			my $name = $file->getAttribute("NAME");
			if (!defined $resourcesfiles{$name})
				{
				# first occurrence - add to list
				$resourcesfiles{$name}=1;
				&textElement($file, "TARGETNAME", $targetname, $file->getFirstChild);
				$file->removeAttribute("NAME");
				&disconnectNode($file);

				$xmlResourcesGroup->appendChild($file);
				$xmlResourcesGroup->addText("\n");
				}
			}
		&removeNode($group);
		}

		
	# Build the "Link" group
			
	my $xmlLinkGroup = &addGroup($xmlGroupList,"Link");
	my %linkfiles;
	my @linkgroups = $xmlProjectDoc->getElementsByTagName("LINKGROUP",1);
	foreach my $group (@linkgroups)
		{
		$targetname = $group->getAttribute("TARGET");
		my @files = $group->getElementsByTagName("FILEREF",0);
		foreach my $file (@files)
			{
			my $name = $file->getAttribute("NAME");
			if (!defined $linkfiles{$name})
				{
				# first occurrence - add to list
				$linkfiles{$name}=1;
				&textElement($file, "TARGETNAME", $targetname, $file->getFirstChild);
				$file->removeAttribute("NAME");
				&disconnectNode($file);
				$xmlLinkGroup->appendChild($file);
				$xmlLinkGroup->addText("\n");
				}
			}
		&removeNode($group);
		}


	# Build the "Documents" group
			
	my $xmlDocumentsGroup;
	my %documentfiles;
	my @documentgroups = $xmlProjectDoc->getElementsByTagName("DOCUMENTSGROUP",1);
	foreach my $group (@documentgroups)
		{
		$targetname = $group->getAttribute("TARGET");
		my @files = $group->getElementsByTagName("FILEREF",0);
		foreach my $file (@files)
			{				
			# Only create the "Documents" group if there are some files to add to it
			if (!defined $xmlDocumentsGroup)
				{
				$xmlDocumentsGroup = &addGroup($xmlGroupList,"Documents");
				}

			my $name = $file->getAttribute("NAME");

			
			if (!defined $documentfiles{$name})
				{
				# first occurrence - add to list
				$documentfiles{$name}=1;
				&textElement($file, "TARGETNAME", $targetname, $file->getFirstChild);
				$file->removeAttribute("NAME");
				&disconnectNode($file);
				$xmlDocumentsGroup->appendChild($file);
				$xmlDocumentsGroup->addText("\n");
				}
			}
		&removeNode($group);
		}


	# Build the "Lib" group and its subgroups

	my $xmlLibGroup = &addGroup($xmlGroupList, "Libraries");
	my %libplats;
	my @libgroups = $xmlProjectDoc->getElementsByTagName("LIBGROUP",1);
	foreach my $group (@libgroups)
		{
		$targetname = $group->getAttribute("TARGET");
		my $plat = $group->getAttribute("PLAT");
		if (!defined $libplats{$plat})
			{
			$libplats{$plat} = &addGroup($xmlLibGroup, $plat);
			}
		my $platgroup = $libplats{$plat};
		my @files = $group->getElementsByTagName("FILEREF",0);
		foreach my $file (@files)
			{
			my $name = $file->getAttribute("NAME");
			if (!defined $sourcefiles{"$plat\\$name"})
				{
				# first occurrence - add to list
				$sourcefiles{"$plat\\$name"}=1;
				&textElement($file, "TARGETNAME", $targetname, $file->getFirstChild);
				$file->removeAttribute("NAME");
				&disconnectNode($file);
				$platgroup->appendChild($file);
				$platgroup->addText("\n");
				}
			}
		&removeNode($group);
		}

	# Replace the GROUPLIST & TARGETORDER in the template document

	my $node = $xmlProjectDoc->getElementsByTagName("GROUPLIST",1)->item(0);
	$node->getParentNode->replaceChild($xmlGroupList, $node);

	$node = $xmlProjectDoc->getElementsByTagName("TARGETORDER",1)->item(0);
	$node->getParentNode->replaceChild($xmlTargetOrder, $node);

	# Insert the "Build All" target

	my $xmlBuildAll = new XML::DOM::Element($xmlProjectDoc,"TARGET");
	$xmlBuildAll->addText("\n");
	&textElement($xmlBuildAll, "NAME", "Build All");
	my $settinglist = new XML::DOM::Element($xmlProjectDoc,"SETTINGLIST");
	&textSetting($settinglist, "Linker", "None");
	&textSetting($settinglist, "Targetname", "Build All");
	$xmlBuildAll->appendChild($settinglist);
	$xmlBuildAll->addText("\n");
	&textElement($xmlBuildAll, "FILELIST", "");
	$xmlBuildAll->addText("\n");
	&textElement($xmlBuildAll, "LINKORDER", "");
	$xmlBuildAll->addText("\n");
	$xmlBuildAll->appendChild($xmlSubTargetList);

	&addOrderedTarget($xmlTargetOrder, "Build All");

	$node = $xmlProjectDoc->getElementsByTagName("TARGETLIST",1)->item(0);
	$node->appendChild($xmlBuildAll);

	# Output the result

	&main::Output(
		$xmlProjectDoc->toString
	);

}

sub PMEndSrcList {

	my @PlatList=&main::PlatOverrideList();
	
	if (scalar @PlatList == 0)
		{
		@PlatList = ("WINSCW", "ARM4", "ARMV5");

		if ($CW_major_version >= 3)
			{
			push @PlatList, "ARMV5_ABIV1";
			}
		}

	shift @PlatList;	# we've already done the first one in the list
	foreach (@PlatList)
		{
		ExtraPlat($_);
		}

	&finaliseProject();

}

sub GetGCCELibPath($) {
	my $gnulibgccPath;
	open PIPE, "arm-none-symbianelf-g++ $_[0] 2>&1 | ";
	while(<PIPE>){
		$gnulibgccPath = $_;
		$gnulibgccPath =~ s/\//\\/g;
	}
	close PIPE;
	my $SearchlibgccDir = &main::Path_Chop(&main::Path_Split('Path', $gnulibgccPath));

	return $SearchlibgccDir;
}

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

	}
}

# Set the options passed from BSF file 
# @param OptionName    - BSF Keyword using which the options would be overridden in the BSF file
# @param Options       - List of options read from the BSF keyword
sub Set_BSF_Options($$)
{
	my ($OptionName,$Options) = @_;
	my @Fragments=();
	
	if ($CustPlat{'CUSTOMIZES'} && ($CustPlat{'ROOTPLATNAME'} eq "GCCE"))
	{
		$CustGCCE=1;
	}
	foreach my $val (@{$Options})
	{		
		# Check if the value of BSF option is to be set or added/removed.
		if($val =~ /\+\[.*\]\+|\-\[.*\]\-/)
		{
			if (@Fragments = Cl_bpabi::Split_BSF_Options($val,'RemoveOptions'))
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
					if((($OptionName =~ /COMMON_OPTIONS/)
					|| ($OptionName =~ /THUMB_OPTIONS/)
					|| ($OptionName =~ /ARM_OPTIONS/)
					|| ($OptionName =~ /KERNEL_OPTIONS/)
					|| ($OptionName =~ /INVARIANT_OPTIONS/))
					&& ($CustGCCE))
					{
						$GCCE_CompilerOption = RemoveBsfOptions($Opt,$GCCE_CompilerOption);
					}
					elsif($OptionName =~ /COMMON_OPTIONS/)
					{
						$CCFLAGS = RemoveBsfOptions($Opt,$CCFLAGS);
					}
					elsif(($OptionName =~ /THUMB_OPTIONS/)
					|| ($OptionName =~ /ARM_OPTIONS/)
					|| ($OptionName =~ /KERNEL_OPTIONS/))
					{
						$CCFLAGS = RemoveBsfOptions($Opt,$CCFLAGS);
					}
					elsif($OptionName =~ /INVARIANT_OPTIONS/)
					{
						$CCFLAGS = RemoveBsfOptions($Opt,$CCFLAGS);
					}
					elsif($OptionName =~ /LD_OPTIONS/)
					{
						$linkeropts = RemoveBsfOptions($Opt,$Link);
						$linkCommand = RemoveBsfOptions($Opt,$Link);
					}
					elsif($OptionName =~ /AR_OPTIONS/)
					{
						$archiveropts = RemoveBsfOptions($Opt,$linkCommand);
					}
				}					
				@Fragments=();
			}
			
			if (@Fragments = Cl_bpabi::Split_BSF_Options($val,'AddOptions')) 
			{
				my $v;
				foreach $v (@Fragments)
				{
					if((($OptionName =~ /COMMON_OPTIONS/)
					|| ($OptionName =~ /THUMB_OPTIONS/)
					|| ($OptionName =~ /ARM_OPTIONS/)
					|| ($OptionName =~ /KERNEL_OPTIONS/)
					|| ($OptionName =~ /INVARIANT_OPTIONS/))
					&& ($CustGCCE))
					{
						$GCCE_CompilerOption .= ' '.$v.' ';
					}
					elsif($OptionName =~ /COMMON_OPTIONS/)
					{
						$bsfaddoptions .= ' '.$v.' '; 						
					}
					elsif(($OptionName =~ /THUMB_OPTIONS/)
					|| ($OptionName =~ /ARM_OPTIONS/)
					|| ($OptionName =~ /KERNEL_OPTIONS/))
					{
						$bsfaddoptions .= ' '.$v.' ';	
					}
					elsif($OptionName =~ /INVARIANT_OPTIONS/)
					{
						$bsfaddoptions .= ' '.$v.' ';
					}
					elsif($OptionName =~ /LD_OPTIONS/)
					{
						$linkeropts .= ' '.$v.' ';
					}
					elsif($OptionName =~ /AR_OPTIONS/)
					{
						$archiveropts .= ' '.$v.' ';
					}
				}
				@Fragments=();
			}
		}
		else
		{
			if((($OptionName =~ /COMMON_OPTIONS/)
			|| ($OptionName =~ /THUMB_OPTIONS/)
			|| ($OptionName =~ /ARM_OPTIONS/)
			|| ($OptionName =~ /KERNEL_OPTIONS/)
			|| ($OptionName =~ /INVARIANT_OPTIONS/))
			&& ($CustGCCE))
			{
				$GCCE_CompilerOption .= ' '.$val.' ';
			}
			elsif($OptionName =~ /COMMON_OPTIONS/)
			{
				$bsfaddoptions .= ' '.$val.' ';
			}
			elsif(($OptionName =~ /THUMB_OPTIONS/)
			|| ($OptionName =~ /ARM_OPTIONS/)
			|| ($OptionName =~ /KERNEL_OPTIONS/))
			{
				$bsfaddoptions .= ' '.$val.' ';
			}
			elsif($OptionName =~ /INVARIANT_OPTIONS/)
			{
				$bsfaddoptions .= ' '.$val.' ';
			}
			elsif($OptionName =~ /LD_OPTIONS/)
			{
				$linkeropts .= ' '.$val.' ';
			}
			elsif($OptionName =~ /AR_OPTIONS/)
			{
				$archiveropts .= ' '.$val.' ';
			}
		}	
	}
}

sub RemoveBsfOptions($$)
{
	my ($Opt_to_replace,$Opt_replaced_in) = @_;
	
	$Opt_replaced_in =~ s/$Opt_to_replace//g;
	return $Opt_replaced_in;
}

# function to expand the macro as pass with appropriate options
sub printlist {
	my $option =shift @_;
	my @list = @_,
	my $data;
	my $finalval=undef;
	
	foreach $data (@list)
	{
		if($option =~ "-D") {
			$finalval .= " ".$option.$data;	
		}
		else {
			$finalval .= " ".$option." ".$data;
		}
	}
	return $finalval;
}

#read the configuration make file into the HASH and use them for further processing
sub collect_config_data {
	my($configfile) = @_;
	open(DATA, "<$configfile");
	while(<DATA>) 
	{	
		my $line = $_;
		if($line =~ /=/)
		{
			if ($line =~ /(.*):=(.*)/)
			{ 
				$configdata{$1}=$2;
			}
			elsif ($line =~ /(.*)=(.*=.*)/)
			{ 
				$configdata{$1}=$2;
			}
			elsif ($line =~ /(.*)=(.*)/)
			{ 
				$configdata{$1}=$2;
			}
		}
	}
	close(DATA)
}

#function is ti fetch the contents of the config data which is read from the configuration make file, 
# for ex: KERNEL_OPTIONS=$(ARM_INSTRUCTION_SET) $(NO_EXCEPTIONS), this function extracts the value for ARM_INSTRUCTION_SET & NO_EXCEPTIONS
sub fetch_config_data {
	my($list) = @_;
	my $op;
	my $op1;
	my $finaldata = undef;
	
	my @ip_options = split(/\s+/, $list);	
	foreach $op (@ip_options)
	{
		$op =~ s/\)//g;
		$op =~ s/\$\(//g;
		if($op =~ /-/) {
			$finaldata .= " ".$op;
		}
		else {
			$finaldata .= " ".$configdata{$op};
		}
	}
	return $finaldata;
}

# function to fix the bsf options, if the bsf option is already present in the CCFLAGS then remove from it so that it can be added from bsf,
# this is to avoid the duplication of the options passed to the compiler.
sub fixbsfoptions {
	my ($options) = @_;
	my $ccflgs = $CCFLAGS;
	my $d;
	my $Pattern = '-{1,2}\S+\s*(?!-)\S*';
	my @list = $options =~ /$Pattern/g;
	foreach $d (@list) {
		if($ccflgs =~ /$d/) {
				$ccflgs =~ s/$d//g;	
		}
		else {
			if($d =~ /(.*)\s+(.*)/) {
				my $a = $1;
				if($ccflgs =~ /$a\s+\S+/) {
					$ccflgs =~ s/$a\s+\S+//g;
				}
			}
		}
	}
	$CCFLAGS = $ccflgs;
}

# funtion to get the list if the libraries to be linked during linking
sub GetLibList() {
	my @LibList;
	my @StaticLibList;
	my $Plat=&main::Plat;
	unless(defined($ENV{RVCT_VER_MAJOR})){
		my ($rvct_M, $rvct_m, $rvct_b) = RVCT_plat2set::get_version_list($Plat);
		$ENV{RVCT_VER_MAJOR}=$rvct_M;
	}
	&Cl_bpabi::getVariableForNewPlat();
	my $list = &Cl_bpabi::getConfigVariable('STATIC_LIBS_LIST') ;
	
	if (length($list) >0)
	{
		@StaticLibList = split(/\s+/, $list);
	}
	if($Plat eq "ARMV5" || $Plat eq "ARMV5_ABIV2" || IsCustomization($Plat)) {
		@LibList=&Armutl_ArmLibList;
		if(@LibList==0) {
			my $LibDir = Armutl_ArmLibDir();
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

1;
