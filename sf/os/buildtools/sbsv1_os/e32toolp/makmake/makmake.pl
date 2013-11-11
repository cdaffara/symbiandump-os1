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
# all variables called *Path* are set up to end with a backslash
# all variables called *Path or *File are stored as absolute (file)paths within makmake
# all variables called UpPath* are stored as relative paths within makmake
# 
#


use FindBin;		# for FindBin::Bin
use Getopt::Long;
use Cwd;
use strict;
# modified start: makefile improvement 
use File::stat;
use Time::localtime;
# modified end: makefile improvement 

my $PerlLibPath;    # fully qualified pathname of the directory containing our Perl modules

# Prototype to remove warning.
sub AddStringTables();

BEGIN {
# check user has a version of perl that will cope
	require 5.005_03;
# establish the path to the Perl libraries: currently the same directory as this script
	$PerlLibPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlLibPath =~ s/\//\\/g;	# X:\epoc32\tools
	$PerlLibPath .= "\\";
}

use lib $PerlLibPath;
use E32env;
use E32Plat;
use E32Variant;
use Genutl;
use Modload;
use Pathutl;
use Trgtype;
use CheckSource;
use featurevariantparser;
use featurevariantmap;

# THE MAIN PROGRAM SECTION
##########################

{
	Load_SetModulePath($PerlLibPath);
	Plat_Init($PerlLibPath);
}

my $MAKEFILE;
my $MMPFILE;
my %Options;
my %Plat;
my %TruePlat;
my %BldMacros;
my $PlatArg;
my @PlatOverrideList=();
my $EABIDef;
my $DebugSwitch=undef;

my $IsCompilerWrapperOption = 0;
my $IsProxyWrapperOption = 0;
my $FeatureVariantArg;
my %FeatureVariantInfo;

{
	# process the command line
	unless (GetOptions(\%Options, 'd', 'mmp', 'plat=s', 'v', 'arm', 'nd' , 'ithumb' , 'iarm' , 'debug', 'no_debug', 'logfc','wrap:s')) {
		exit 1;
	}
	#Update the variable to set the information of -wrap option
	if(exists($Options{wrap})) {
		if ($Options{wrap} eq "") {
			# Set the Compiler wrapper option information i.e. '1'
			$IsCompilerWrapperOption = 1;
		} elsif ($Options{wrap} eq "proxy") {
			$IsProxyWrapperOption = 1;
		} else {
			print "WARNING: Invalid value for option -wrap: $Options{wrap}\n";
		}
	}
	
	$Options{makemakefile}='1' unless ($Options{mmp} || $Options{plat});

	if ($Options{mmp} or $Options{plat}) {
		eval { &Load_ModuleL('MAKHELP'); };
		die $@ if $@;
	}

	if ($Options{mmp}) {
		&Help_Mmp;
		exit;
	}

	if ($Options{plat}) {
		eval { &Plat_GetL($Options{plat},\%Plat,\%BldMacros); };
		die $@ if $@;
		eval { &Load_ModuleL($Plat{MakeMod}); };
		die $@ if $@;
		&Help_Plat($Plat{Real},$Plat{CPU}, $Plat{DefFile}, \@{$Plat{MmpMacros}},\@{$Plat{Macros}});
		exit;
	}

#	show help & exit if necessary
	if (@ARGV != 2) {
		&Usage();
	}
	if ($Options{v}) {
		print "perl -S makmake.pl @ARGV\n";
		&Load_SetVerbose;
		&Path_SetVerbose;
		&Plat_SetVerbose;
	}

	$PlatArg=uc pop @ARGV;

	# Process build platform arguments where they differ from the "norm"
	if ($PlatArg=~/^(\S+)\.(\S+)$/)
		{
		# Explicit feature variant platforms take the form <base platform>.<variant name>
		# e.g. armv5.variant1
		$PlatArg=$1;
		$FeatureVariantArg=$2;
		}
	elsif ($PlatArg=~/^(\S+):(\S+)$/)
		{
		# IDE platforms can take the form cw_ide:<platform-1>+<platform-2>+<platform-n>
		# e.g. cw_ide:plat1+plat2+plat3
		# 			
		$PlatArg=$1;
		@PlatOverrideList=split(/\+/,$2);
		}

	eval { &Plat_GetL($PlatArg,\%TruePlat,\%BldMacros); };
	die $@ if $@;
	if (scalar @PlatOverrideList) {
		$PlatArg=$PlatOverrideList[0];
	}

	
	$MMPFILE=pop @ARGV;
	die "ERROR: Can't specify MMP file on a different drive\n" if $MMPFILE=~/^\w:\\/o;
	if ($MMPFILE!~/.MMP$/io) {
		$MMPFILE.='.MMP';
	}
	$MMPFILE=&Path_AbsToWork($MMPFILE);

	eval { &Load_ModuleL('Mmp'); };
	die $@ if $@;
	if ($Options{v}) {
		&Mmp_SetVerbose;
	}
	if ($Options{d}) {
		die "ERROR: $E32env::Data{EPOCPath} does not exist\n" if (!-d $E32env::Data{EPOCPath});
	}
	
	if ($Options{debug}) {
		$DebugSwitch = 1;
	}
	elsif($Options{no_debug}){
		$DebugSwitch = 0;
	}
}

my %LinkerOptions;
my %WarningLevel;
my $ABI;
my @AifStruct;
my $AllowDllData;
my $CompressTarget;
my $CompressTargetMode;   #NONE
my $ASSPExports;
my @ASSPLibList;
my @BitMapStruct;
my $BuildAsARM=$Options{arm};
my $CallDllEntryPoints;
my $Capability;
my @CapabilityFlags;
my $DataLinkAddress;
my @DebugLibList;
my %Def;
my %DocHash;
my $ExportUnfrozen;
my $FirstLib;
my $FixedProcess;
my %HeapSize;
my @LibList;
my $LinkAs;
my $LinkAsBase;
my $ExportLibrary;
my $NoExportLibrary;
my %MmpFlag;
my @PlatTxt2D;
my $ProcessPriority;
my @RamTargets;
my @ResourceStruct;
my @RomTargets;
my $SmpSafe;
my @SourceStruct;
my $StackSize;
my @StatLibList;    
my $StdCpp;
my $NoStdCpp;
my $NewLib;
my @SysIncPaths;
my @ResourceSysIncPaths;
my $ResourceVariantMacroHRHFile;
my $Trg;
my %TrgType;
my @UidList;
my @UserIncPaths;
my $SrcDbg;
my %Path;
my %Version;
my $SecureId;
my $VendorId;
my $variantMacroHRHFile = Variant_GetMacroHRHFile();  # HRH file containing macros specific to a variant
my %ReplaceOptions;
my $ARMFPU;
my @StringTable;
my @StringTableUserIncPaths;
my $CodePagingTargetMode;	# 0-N/A, 1-UNPAGED, 2-PAGED
my $DataPagingTargetMode;	# 0-N/A, 1-UNPAGED, 2-PAGED
my %CheckSourceUDEBIncludes;
my %CheckSourceURELIncludes;
my %CheckSourceMMPMetaData;
my %CheckSourceMMPIncludes;
my $IsWideCharMain=0;
my $IsDebuggable; # 0-NONDEBUGGABLE, 1-DEBUGGABLE, 2-DEBUGGABLE_UDEBONLY


use constant NOTPAGED => 0;
use constant UNPAGED => 1;
use constant PAGED => 2;

use constant INFLATECOMPRESSIONMETHOD => 1;
use constant BYTEPAIRCOMPRESSIONMETHOD => 2;

use constant NOTDEBUGGABLE => 0;
use constant DEBUGGABLE =>  1;
use constant DEBUGGABLE_UDEBONLY => 2;

# If the platform does support feature variants but none are specified, then we assume the use of "DEFAULT" if it exists
# If default doesn't exist feature variantion is basically disabled?
$FeatureVariantArg = 'default' if (!$FeatureVariantArg && defined &PMSupportsFeatureVariants && featurevariantparser->DefaultExists());

# Preload the details of the variant requested if any - we need the HRH file for MMP file processing
if ($FeatureVariantArg)
	{
	# First check the feature variant is valid
	my @buildableFeatureVariants = featurevariantparser->GetBuildableFeatureVariants();
	die "ERROR: \"$PlatArg.$FeatureVariantArg\" feature variant is either invalid or virtual.\n" if !(grep /^$FeatureVariantArg$/i, @buildableFeatureVariants);

	# Now load the variant
	%FeatureVariantInfo = featurevariantparser->GetVariant($FeatureVariantArg);
	
	# Change the HRH file to use
	$variantMacroHRHFile = $FeatureVariantInfo{VARIANT_HRH} if $FeatureVariantInfo{VARIANT_HRH};
	}
	
&SetVarsFromMmp($PlatArg);
die $@ if $@;

{
	# set up the makefile filepath - need to do this before loading the platform module
	# because UID source file will be added and set up in the makefile path under WINS
	if ($Options{d}) {
		$MAKEFILE=join ('', $Path{Bld}, &Path_Split('Base',$MMPFILE), $TruePlat{Ext});
	}
	else {
		$MAKEFILE=join "", &Path_WorkPath, &Path_Split('Base',$MMPFILE), $TruePlat{Ext};
	}
}

{
	# Generate an X86GCC def file from eabi def file in build dir if needed
	if (($PlatArg eq "X86GCC" || $PlatArg eq "X86GMP") && $Def{Base} && not -e &DefFile)
	{
		# Find the equivalent eabi def file
		my $eabiDefFile = File::Spec->canonpath("$Def{Path}../eabi/$Def{Base}$Def{Ext}");
		if (-e $eabiDefFile)
		{
			# Need to create MAKEFILE directory early in this case
			eval { &Path_MakePathL($MAKEFILE); };
			die $@ if $@;
			# Change def file path to build path 
			$Def{Path} = $Path{Bld};
			&generateX86GCCDefFile($eabiDefFile, &DefFile);
		}
		else
		{
			print "WARNING: Unable to find EABI def file at $eabiDefFile to generate X86GCC def file with\n";
		}
	}
}

{


#	load the platform module
	eval { &Load_ModuleL($TruePlat{MakeMod}); };
	die $@ if $@;

	unless (defined &PMHelp_Mmp) {
#		check this function is defined - all modules must have it - if not perhaps the
#		platform module has not loaded is compiler module successfully via "use"
		die "ERROR: Module \"$Plat{MakeMod}\" not loaded successfully\n";
	}
}

	# Allow the platform to bow out if feature variants have been specified but it doesn't support them
	if ($FeatureVariantArg && !defined &PMSupportsFeatureVariants)
		{
		die "ERROR: The \"$PlatArg\" platform does not support feature variants.\n";
		}

{
	# allow the platform to bow out if it can't support some .MMP file specifications
	if (defined &PMCheckPlatformL) {
		eval { &PMCheckPlatformL(); };
		die $@ if $@;
	}
}

my @StdIncPaths=();

{
	# get the platform module to do it's mmpfile processing - WINS modules may set up an extra source file
	# for UIDs here depending upon the targettype
	&PMPlatProcessMmp(@PlatTxt2D) if defined &PMPlatProcessMmp;
}

%CheckSourceMMPMetaData = &Mmp_CheckSourceMMPMetaData();
%CheckSourceMMPIncludes = &Mmp_CheckSourceMMPIncludes();

# merge checksource processing from platform specific .mmp sections, if applicable
%CheckSourceMMPMetaData = (%CheckSourceMMPMetaData, &PMPlatCheckSource()) if defined &PMPlatCheckSource;

@ResourceSysIncPaths = @SysIncPaths;
$ResourceVariantMacroHRHFile = $variantMacroHRHFile;

AddStringTables();

# Process feature variants if applicable

if ($FeatureVariantArg)
	{
	if ($Options{v})
		{
		$featurevariantmap::verbose = 1;
		$featurevariantparser::verbose = 1;
		}

	# Get the default variant details
	my %DefaultFeatureVariantInfo = $FeatureVariantInfo{NAME} =~ /^default$/i ? %FeatureVariantInfo : featurevariantparser->GetVariant("DEFAULT");	
	die "ERROR: Feature variant \"$PlatArg.default\" is invalid.\n" if !$DefaultFeatureVariantInfo{VALID};

	# The following IF statement decides whether to use the default variant and not use the hash in the filename 
	# This prevents the generation of dll's/exe's for which variants are not needed (i.e they are invariant)
	# It also avoids the time-consuming and redundant hash generation
	# A component is considered invariant if it's not a DLL or EXE or FEATUREVARIANT isn't present in the .mmp file
	
# modified start: makefile improvement 
	if ($TrgType{Basic} =~ /^(EXEDLL|EXE|DLL|LIB)$/ && &Mmp_IsFeatureVariant)
# modified end: makefile improvement 
		{
		# Load the requested variant if it hasn't already been preloaded		
		%FeatureVariantInfo = featurevariantparser->GetVariant($FeatureVariantArg) if !$FeatureVariantInfo{NAME} || $FeatureVariantInfo{NAME} ne $FeatureVariantArg;
		}
	else
		{
		# Use the default variant
		%FeatureVariantInfo = %DefaultFeatureVariantInfo;
		$FeatureVariantInfo{INVARIANT} = 1;
		$FeatureVariantInfo{NAME} = uc $FeatureVariantArg;
		}

	die "ERROR: Feature variant \"$PlatArg.$FeatureVariantInfo{NAME}\" is invalid.\n" if !$FeatureVariantInfo{VALID};
	
	my @featureVariantSysIncPaths = (@{$FeatureVariantInfo{BUILD_INCLUDES}},@SysIncPaths);
	
	# Further process paths and filenames so that they include a drive letter.
	# We store this in a hash specifically for passing on to featurevariantmap->Hash
	
	my @processedIncludes = &Path_PrefixWithDrive(&Path_Chop(@UserIncPaths), &Path_Chop(@featureVariantSysIncPaths));
	push @processedIncludes, &Path_Chop(&PMToolChainIncDir) if defined &PMToolChainIncDir && &PMToolChainIncDir;
	
	my $processedPreInclude = "";
	if (defined &PMPrefixFile)
		{
		$processedPreInclude = &PMPrefixFile;
		$processedPreInclude =~ s/\"//g;
		$processedPreInclude = &Path_PrefixWithDrive($processedPreInclude);		
		}

	my %processedFeatureVariantInfo;
	$processedFeatureVariantInfo{PREINCLUDE} = $processedPreInclude if $processedPreInclude;
	$processedFeatureVariantInfo{BUILD_INCLUDES} = \@processedIncludes if @processedIncludes;
	$processedFeatureVariantInfo{VALID} = 1;
	
	# Pass in the details of the macros tested in the MMP
	$processedFeatureVariantInfo{MMPTESTED} = &Mmp_TestedMacros();

	my @pathedSrcList = ();
	push @pathedSrcList, Path_PrefixWithDrive($$_{SrcPath}.$$_{CurFile}) foreach (@SourceStruct);

	foreach my $bld (@{$Plat{Blds}})
		{
# modified start: makefile improvement 
		my @reusedHash;
# modified end: makefile improvement 
		if ($FeatureVariantInfo{INVARIANT})															# Invariant override
			{
			$FeatureVariantInfo{$bld."_LABEL"} = "INVARIANT";
			}
		else
			{
# modified by SV start: makefile improvement 
			my $vmap = "$E32env::Data{RelPath}".lc($Plat{Real})."\\".lc($bld)."\\".Trg()."." . $FeatureVariantInfo{NAME}.".vmap";
# modified by SV end: makefile improvement 
			$vmap = Path_PrefixWithDrive($vmap);
			if(-e $vmap){
				my @variantlist = featurevariantmap->GetVariantListFromVmap($vmap);
				my @calls;
				foreach(@variantlist)
				{
					my $target = "CHECKVMAP".uc($bld);
					my $makefile = $MAKEFILE.".".$_;
					if(-e $makefile){
						push @calls, "make -r -f \"$makefile\"  $target";
					}
				}
				foreach my $call (@calls)
				{
					print "call: $call" if $Options{v};
					open PIPE, "$call |";
					while(<PIPE>) {
						print $_;
					}
					close PIPE;
				}
				if(-e $vmap){
					@reusedHash = featurevariantmap->CheckOldVmapFile($vmap, \%FeatureVariantInfo);
				}
			}
			if(@reusedHash)
			{
				$FeatureVariantInfo{$bld."_LABEL"} = $reusedHash[0];
				$FeatureVariantInfo{$bld."_FEATURES"} = $reusedHash[1];
				next;
			}
# modified end: makefile improvement 
			my @macros = (@{$Plat{Macros}}, @{$BldMacros{$bld}}, "__SUPPORT_CPP_EXCEPTIONS__");
			push @macros, "__PRODUCT_INCLUDE__=\"".&Path_PrefixWithDrive($FeatureVariantInfo{VARIANT_HRH})."\"" if $FeatureVariantInfo{VARIANT_HRH};
			$processedFeatureVariantInfo{MACROS} = \@macros;

			print ("Feature variant hash processing: \"$FeatureVariantInfo{NAME} $bld\"\n") if $Options{v};
			
			my @result = featurevariantmap->HashAndFeatures(\@pathedSrcList, \%processedFeatureVariantInfo);
			$FeatureVariantInfo{$bld."_LABEL"} = $result[0];
			$FeatureVariantInfo{$bld."_FEATURES"} = $result[1];
			
			die "ERROR: Can't obtain hash for \"$PlatArg.$FeatureVariantInfo{NAME}\" feature variant.\n" if !$FeatureVariantInfo{$bld."_LABEL"};
			}
		}

	# Customise standard content based on feature variant updates
	@SysIncPaths = @featureVariantSysIncPaths;
	$variantMacroHRHFile = $FeatureVariantInfo{VARIANT_HRH};
	$MAKEFILE .= ".$FeatureVariantInfo{NAME}";

	# Resources are always processed in the context of the default variant's system include and variant files when
	# feature variants are in use
	@ResourceSysIncPaths = (@{$DefaultFeatureVariantInfo{BUILD_INCLUDES}},@ResourceSysIncPaths);
	$ResourceVariantMacroHRHFile = $DefaultFeatureVariantInfo{VARIANT_HRH};
	}


{
	# if verbose mode set, output some info
	#--------------------------------------
	if ($Options{v}) {
		print  
			"Target: \"$Trg\"\n",
			"TargetType: \"$TrgType{Name}\"\n",
			"Libraries: \"@LibList\"\n",
			"Debug Libraries: \"@DebugLibList\"\n",
			"Static Libraries: \"@StatLibList\"\n",
			"Uids: \"@UidList\"\n",
			"BuildVariants: \"@{$Plat{Blds}}\"\n",
			"TargetMakeFile: \"$MAKEFILE\"\n",
			"UserIncludes: \"<Source Dir> @UserIncPaths\"\n",
			"SystemIncludes: \"@SysIncPaths\"\n"
		;

	if (%FeatureVariantInfo)
		{
		print 
			"Feature Variant Name: \"$FeatureVariantInfo{NAME}\"\n",
			"Feature Variant SystemIncludes: \"@{$FeatureVariantInfo{BUILD_INCLUDES}}\"\n",
			"Feature Variant HRH file: \"$FeatureVariantInfo{VARIANT_HRH}\"\n";

		foreach my $bld (@{$Plat{Blds}})
			{
			print "Feature Variant $bld Label: \"".$FeatureVariantInfo{$bld."_LABEL"}."\"\n";			
			}
		
		}
	}
}

# Special handling for non-default invariant makefiles without FEATUREVARIANT in the MMP file
# In this situation the default variant makefle is just included into the variant makefile
# modified start: makefile improvement 
if ($TrgType{Basic} =~ /^(EXEDLL|EXE|DLL|LIB)$/ && %FeatureVariantInfo && $FeatureVariantInfo{INVARIANT})
	{
	$MAKEFILE =~ s/([^.]*$)/DEFAULT/;
# modified by SV start: makefile improvement 
		if( $FeatureVariantInfo{NAME} !~ /^default$/i)
			{
				print "not creating makefile for  : $FeatureVariantInfo{NAME}\n" if ($Options{v});
			}
# modified by SV end: makefile improvement 
	if(-e $MAKEFILE )
		{
		my $mmp_time = -M $MMPFILE;
		my $makefile_time = -M $MAKEFILE;
		if( $makefile_time <= $mmp_time)
			{
			exit;
			}
		}
	undef %FeatureVariantInfo;
	%FeatureVariantInfo = featurevariantparser->GetVariant("default");
	$FeatureVariantInfo{INVARIANT} = 1;
	$FeatureVariantInfo{UREL_LABEL} = 'INVARIANT';
	$FeatureVariantInfo{UDEB_LABEL} = 'INVARIANT';
	}
# modified by SV start: makefile improvement 
	elsif(%FeatureVariantInfo)
	{
	my $variant_info = &Path_Chop($E32env::Data{BldPath}).$Path{BldInfPath}."\\FeatureVariantInfo\\".$Plat{Real}."\\".$Plat{Real}.".".$FeatureVariantInfo{NAME}.".".&Path_Split('Base', $MMPFILE).".info";
	#if mmp file does not exists
	$variant_info = &Path_Chop($E32env::Data{BldPath}).$Path{BldInfPath}."\\FeatureVariantInfo\\".$Plat{Real}."\\".$Plat{Real}.".".$FeatureVariantInfo{NAME}.".info" if ! -e $MMPFILE;
# modified by SV end: makefile improvement 
	my $variant_key = "VARIANT_PLAT_NAME_".&Path_Split('Base', $MMPFILE);
	my $variant_info_new = $variant_info.".tmp";
	open VARIANTINFOR_NEW, ">$variant_info_new" or die "ERROR: Can't open or create file \"$variant_info_new\"\n";

	# Open the variant infor file
	open VARIANTINFOR, "<$variant_info" or die "ERROR: Can't open file \"$variant_info\"\n";
	while(<VARIANTINFOR>)
		{
		if(/^$variant_key/)
			{
			print VARIANTINFOR_NEW $variant_key.":=".$FeatureVariantInfo{NAME}."\n";
			}
			else
			{
			print VARIANTINFOR_NEW $_;
			}
		}
	# Close and cleanup
	close VARIANTINFOR or die "ERROR: Can't close file \"$variant_info\"\n";
	close VARIANTINFOR_NEW or die "ERROR: Can't close file \"$variant_info\"\n";
	unlink $variant_info;
	rename($variant_info_new, $variant_info);
	if ($Options{v}) {
		print "Successful Variant Infor File Creation\n";

		}
	}
# modified end: makefile improvement 

my $CurAifRef;
my $CurBaseObj;
my $CurBld;
my $CurBitMapRef;
my @CurDepList;
my $CurDoc;
my $CurResrc;
my $CurResourceRef;
my $CurSrc;
my $CurSrcPath;
my $ResrcIsSys;
# modified start: makefile improvement 
my %CurSrcSet;
# modified end: makefile improvement 

# Set up library paths getting the backend module to help if it wants to
{
	&InitLinkPaths();
}

{

	# LOOPING SECTION
	#----------------
	# Load the output module
	eval { &Load_ModuleL('OUTPUT'); };
	die $@ if $@;

 
	# Believe include first on the system list. 
    my $VariantFile=&main::VariantFile();
    if($VariantFile){
        my $VariantFilePath = Path_Split('Path',$VariantFile);
        chop($VariantFilePath);

        push(@SysIncPaths, $VariantFilePath);
    }

	my $ResourceVariantFile=&main::ResourceVariantFile();
    if($ResourceVariantFile){
        my $ResourceVariantFilePath = Path_Split('Path',$ResourceVariantFile);
        chop($ResourceVariantFilePath);

        push(@ResourceSysIncPaths, $ResourceVariantFilePath);
    }
    
    ## Add default system include info for TOOLS2
    if ($PlatArg eq 'TOOLS2') {    	
    	push @SysIncPaths , "$E32env::Data{EPOCPath}include\\tools\\stlport";
    }

	# If the stdcpp keyword is used, or if the target type is STD* ...
	if ($StdCpp or $TrgType{Name} eq 'STDEXE' or $TrgType{Name} eq 'STDDLL' or $TrgType{Name} eq 'STDLIB') {
		push @SysIncPaths, $E32env::Data{EPOCPath}."include\\stdapis";
	}

	&PMStartBldList($Plat{MakeCmd}) if defined &PMStartBldList;
	my $LoopBld;
	foreach $LoopBld (@{$Plat{Blds}}) {
		$CurBld=$LoopBld;
		&PMBld if defined &PMBld;
	}
	undef $CurBld;
	undef $LoopBld;
	&PMEndBldList if defined &PMEndBldList;


	# Load the Dependency Generator
	eval { &Load_ModuleL('MAKDEPS'); };
	die $@ if $@;
	eval { &Deps_InitL($E32env::Data{EPOCIncPath},@StdIncPaths); };
	die $@ if $@;
	if ($Options{v}) {
		&Deps_SetVerbose;
	}
	if ($Plat{UsrHdrsOnly}) {
		&Deps_SetUserHdrsOnly;
	}

	if ($Options{nd} || ($ENV{SYMBIANBUILD_DEPENDENCYOFF} && ($ENV{SYMBIANBUILD_DEPENDENCYOFF}==1))) {
		&Deps_SetNoDependencies
	}
	&Deps_SetNoDependencies if(grep /boostlibrary/i, &Mmp_UserIncPaths);

	&Deps_SetUserIncPaths(@UserIncPaths);
	&Deps_SetSysIncPaths(@ResourceSysIncPaths);
	&Deps_SetPlatMacros(@{$Plat{Macros}});

	my $prefixFile;
	$prefixFile = &PMPrefixFile if defined &PMPrefixFile;
	&Deps_SetPrefixFile($prefixFile) if $prefixFile;

#	Start source list - bitmaps, resources, .AIF files, documents, sources.

	# If feature variants are in use, dependency analysis may use a different overall variant file to that for "straight" source
	my $curDepOSVariantFile = &Deps_GetOSVariantFile();
	&Deps_SetOSVariantFile($ResourceVariantFile);

	&PMStartSrcList if defined &PMStartSrcList;

#	start bitmaps

	if ($Options{v}) {
		print "Starting bitmaps\n";
	}
	my $LoopBitMapRef;
	foreach $LoopBitMapRef (@BitMapStruct) {
		$CurBitMapRef=$LoopBitMapRef;
		if ($Options{v}) {
			print "BitMap: \"$$CurBitMapRef{Trg}\"\n";
		}
		&PMBitMapBld if defined &PMBitMapBld;
	}
	undef $CurBitMapRef;
	undef $LoopBitMapRef;

#	end bitmaps

#	start resources

	if ($Options{v}) {
		print "Starting resources\n";
	}
	my $LoopResourceRef;
	foreach $LoopResourceRef (@ResourceStruct) {
		$CurResourceRef=$LoopResourceRef;
		if ($Options{v}) {
			print "Resource: \"$$CurResourceRef{Trg}\"\n";
		}
		eval { @CurDepList=&Deps_GenDependsL($$CurResourceRef{Source}, ("LANGUAGE_$$CurResourceRef{Lang}")); };
		die $@ if $@;
		&PMResrcBld if defined &PMResrcBld;
		undef @CurDepList;
	}
	undef $CurResourceRef;
	undef $LoopResourceRef;

#	end resources

#	start aifs

	if ($Options{v}) {
		print "Starting aifs\n";
	}

# Add tools-relative include path to sys includes, to allow for shared include\aiftool.rh
	use FindBin;
	$FindBin::Bin =~ /:(.*)\//;
	my $extraIncPath = $1;
	$extraIncPath =~ s/\//\\/g;
	my @SavedResourceSysIncPaths = @ResourceSysIncPaths;
	push @ResourceSysIncPaths, "$extraIncPath\\INCLUDE";
	&Deps_SetSysIncPaths(@ResourceSysIncPaths);

	my $LoopAifRef;
	foreach $LoopAifRef (@AifStruct) {
		$CurAifRef=$LoopAifRef;
		if ($Options{v}) {
			print "Aif: \"$$CurAifRef{Trg}\"\n";
		}
		eval { @CurDepList=&Deps_GenDependsL("$$CurAifRef{Source}"); };
		die $@ if $@;
		&PMAifBld if defined &PMAifBld;
		undef @CurDepList;
	}
	undef $CurAifRef;
	undef $LoopAifRef;

	@ResourceSysIncPaths = @SavedResourceSysIncPaths;

#	end aifs

#	start sources

	if ($Options{v}) {
		print "Starting sources\n";
	}
	
	my $SrcRef;
	&Deps_SetOSVariantFile($curDepOSVariantFile);
	&Deps_SetSysIncPaths(@SysIncPaths);

	foreach $SrcRef (@SourceStruct){
		 $CurSrcPath=$$SrcRef{SrcPath};
		 $CurSrc=$$SrcRef{CurFile};

		 my @userIncludes = &Mmp_UserIncPaths;
		 @userIncludes = (@userIncludes, @StringTableUserIncPaths) if (@StringTableUserIncPaths);
		 unshift (@userIncludes, $CurSrcPath);		 

		 if ($TruePlat{Ext} !~ /\.DSP|\.xml/i)
		 	{
			foreach my $buildVariant (@{$Plat{Blds}})
				{		 		
				my @macros = &MacroList;
				@macros = (@macros, @{$BldMacros{$buildVariant}});

				my $checkSourceCommandStore;
			
				if ($buildVariant =~ /rel$/i)
					{
					$checkSourceCommandStore = \%CheckSourceURELIncludes
					}
				else
					{
					$checkSourceCommandStore = \%CheckSourceUDEBIncludes
					}
				CheckSource_Includes($CurSrcPath.$CurSrc, %$checkSourceCommandStore, $VariantFile, @macros, @userIncludes, @SysIncPaths);
			}
		 }
		 if ($Options{v}) {
			print "Sourcepath: \"$CurSrcPath\"\n";
		 }
		 &PMStartSrc if defined &PMStartSrc;

#			strict depend alt 1 start - call different module function if strict depend flag specified
			if (((not $MmpFlag{StrictDepend}) || (not defined &PMSrcBldDepend)) && defined &PMSrcDepend) {
				eval { @CurDepList=&Deps_GenDependsL($CurSrcPath.$CurSrc);};
				die $@ if $@;
# modified start: makefile improvement 
				foreach $srcFile (@CurDepList) {
					if(not exists($CurSrcSet{$srcFile})){
						my $srctmp = $srcFile;
						$CurSrcSet{$srctmp} = 1;
					}
				}
# modified end: makefile improvement 
				&PMSrcDepend if defined &PMSrcDepend;
				undef @CurDepList;
			}
							
#			strict depend alt 1 end

			my $LoopBld;
			foreach $LoopBld (@{$Plat{Blds}}) {
				$CurBld=$LoopBld;
				&PMStartSrcBld if defined &PMStartSrcBld;

#				strict depend alt 2 start - call the module function that deals with dependencies generated for each build variant
				if ($MmpFlag{StrictDepend} && defined &PMSrcBldDepend) {
					eval { @CurDepList=Deps_GenDependsL($CurSrcPath.$CurSrc,@{$BldMacros{$CurBld}}); };
					die $@ if $@;
					&PMSrcBldDepend if defined &PMSrcBldDepend;
					undef @CurDepList;
				}
#				strict depend alt 2 end

				&PMEndSrcBld if defined &PMEndSrcBld;
			}
			undef $CurBld;
			undef $LoopBld;
			&PMEndSrc if defined &PMEndSrc;
# modified start: makefile improvement 
			my $cursrcfile = $CurSrcPath.$CurSrc;
			if(not exists($CurSrcSet{$cursrcfile})){
				$CurSrcSet{$cursrcfile} = 1;
			}
# modified end: makefile improvement 
		}
		undef $CurSrc;
		undef $CurSrcPath;  
	
	

#	end sources

#	start documents

	if ($Options{v}) {
		print "Starting documents\n";
	}
	my $LoopSrcPath;
	foreach $LoopSrcPath (sort keys %DocHash) {
		$CurSrcPath=$LoopSrcPath;
		if ($Options{v}) {
			print "Sourcepath: \"$CurSrcPath\"\n";
		}
		my $LoopDoc;
		foreach $LoopDoc (sort @{$DocHash{$CurSrcPath}}) {
			$CurDoc=$LoopDoc;
			if ($Options{v}) {
				print "Document: \"$CurDoc\"\n";
			}
			&PMDoc if defined &PMDoc;
		}
		undef $CurDoc;
		undef $LoopDoc;
	}
	undef $CurSrcPath;
	undef $LoopSrcPath;

#	end documents

#	rombuild

	my %SpecialRomFileTypes=(
		KEXT=>'extension[MAGIC]',
		LDD=>'device[MAGIC]',
		PDD=>'device[MAGIC]',
		VAR=>'variant[MAGIC]'
	);
	my %KHash1 = (
		kext=>1,
		ldd=>1,
		pdd=>1,
		var=>1,
		kdll=>1
	);
	my %KHash2 = (
		primary=>1,
		variant=>1,
		extension=>1,
		device=>1
	);
	unless ($TrgType{Basic} =~ /^IMPLIB$/io or $TruePlat{Ext} =~ /\.DSP|\.xml/i) { # change to avoid rombuild target for IDE makefiles
		&Output("ROMFILE:\n");
		unless ($Plat{OS} ne 'EPOC32' or $TrgType{Basic} eq 'LIB') {
			my $ref;
			foreach $ref (@RomTargets) {
				my $ABIDir = '##MAIN##';
				my $RomFileType='file';
				if ($$ref{FileType}) {	# handle EKERN.EXE and EFILE.EXE with new ROMFILETYPE keyword instead
					$RomFileType=$$ref{FileType}; # or just do this bit as a custom build makefile
				}
				elsif ($CallDllEntryPoints) {
					$RomFileType='dll';
				}
				elsif ($SpecialRomFileTypes{$TrgType{Name}}) {
					$RomFileType=$SpecialRomFileTypes{$TrgType{Name}};
				}
				my $RomPath="sys\\bin\\";
				if ($$ref{Path}) {
					$RomPath=$$ref{Path};
				}
				elsif ($TrgType{Path}) {
					$RomPath=$TrgType{Path};
					$RomPath=~s-z\\(.*)-$1-o;
				}
				my $RomFile=$LinkAsBase;
				if ($$ref{File}) {
					$RomFile=$$ref{File};
				}
				my $RomDecorations='';
				if ($DataLinkAddress) {
					$RomDecorations="reloc=$DataLinkAddress";
				}
				elsif ($FixedProcess) {
					$RomDecorations.='fixed';
				}
				
				$ABIDir = '##KMAIN##' if ($KHash1{lc $TrgType{Name}});
				$ABIDir = '##KMAIN##' if ($KHash2{lc $RomFileType});
				my $IbyTextFrom="$RomFileType=$E32env::Data{RelPath}$ABIDir\\##BUILD##\\$Trg";
				my $IbyTextTo="$RomPath$RomFile";
				my $Spaces= 60>length($IbyTextFrom) ? 60-length($IbyTextFrom) : 1; 
				&Output("\t\@echo ", $IbyTextFrom, ' 'x$Spaces, "$IbyTextTo $RomDecorations\n");
			}
			foreach $ref (@RamTargets) {
				my $ABIDir = '##MAIN##';
				$ABIDir = '##KMAIN##' if ($KHash1{lc $TrgType{Name}});
				my $RomFileType='data';
				my $RomPath="sys\\bin\\";
				if ($$ref{Path}) {
					$RomPath=$$ref{Path};
				}
				my $RomFile=$Trg;
				if ($$ref{File}) {
					$RomFile=$$ref{File};
				}
				my $RomDecorations='attrib=r';

				my $IbyTextFrom="$RomFileType=$E32env::Data{RelPath}$ABIDir\\##BUILD##\\$Trg";
				my $IbyTextTo="$RomPath$RomFile";
				my $Spaces= 60>length($IbyTextFrom) ? 60-length($IbyTextFrom) : 1; 
				&Output("\t\@echo ", $IbyTextFrom, ' 'x$Spaces, "$IbyTextTo $RomDecorations\n");
			}
		}
	}
#	end rombuild

	&PMEndSrcList if defined &PMEndSrcList;
}

{

	# open the makefile and write all the text it requires to it if makmake has so far been successful
	#-------------------------------------------------------------------------------------------------
	eval { &Path_MakePathL($MAKEFILE); };
	die $@ if $@;
	if ($Options{v}) {
		print "Creating: \"$MAKEFILE\"\n";
	}
	open MAKEFILE,">$MAKEFILE" or die "ERROR: Can't open or create file \"$MAKEFILE\"\n";
	print MAKEFILE &OutText or die "ERROR: Can't write output to file \"$MAKEFILE\"\n";
	close MAKEFILE or die "ERROR: Can't close file \"$MAKEFILE\"\n";
	if ($Options{v}) {
		print "Successful MakeFile Creation\n";
	}
}

	
################ END OF MAIN PROGRAM SECTION #################
#------------------------------------------------------------#
##############################################################





# SUBROUTINE SECTION
####################

sub FatalError (@) {

	print STDERR "MAKMAKE ERROR: @_\n";
	exit 1;
}

sub Usage () {

		eval { &Load_ModuleL('MAKHELP'); };
		die $@ if $@; 
		eval { &Help_Invocation; };
		die $@ if $@;
		exit;
}

sub getEABIDef() {
	# Preprocess the mmp with ARMv5 platform settings so we can pick up 
    # EABI specific def file entries etc. 
    my ($platname)="ARMV5";
    
    # get platform info for armv5
    eval { &Plat_GetL($platname,\%Plat,\%BldMacros); };
	return $@ if $@;
	&Mmp_Reset;
	# process 
    
    # set package to ignore warnings about missing .def file. 
    &Mmp_SetIgnoreMissingDef;
    
    eval { &Mmp_ProcessL($E32env::Data{EPOCPath}, $MMPFILE, \%Plat, $FeatureVariantInfo{BUILD_INCLUDES}); };
	return $@ if $@;
	my %EABIDef=%{&Mmp_Def};
	
    # handle case that def file doesn't exist -> Simply set to ""
    my $EABIDefFile = "$EABIDef{Path}$EABIDef{Base}$EABIDef{Ext}";;
   	unless (-e "$EABIDefFile") {
        $EABIDefFile = "";
	}
    
    return $EABIDefFile;
	
}

sub SetVarsFromMmp ($) {

	my ($platname)=@_;
    
	if($platname eq "GCCXML") {
        $EABIDef = getEABIDef();
    } 
    else {
        $EABIDef = "";
    }

	# MMP FILE PROCESSING - filter the mmp file content through the GCC preprecessor
	#-------------------------------------------------------------------------------
	eval { &Plat_GetL($platname,\%Plat,\%BldMacros); };
	return $@ if $@;
	&Mmp_Reset;

	if($platname eq "GCCXML" || $platname eq "X86GCC" || $platname eq "X86GMP") {
	# set package to ignore warnings about missing .def file, this is necessary,
	# as either EABI .def or GCC .def may not exist, and this shouldn't be reported
	# as a warning or error. Similarly for x86gcc def files - these are autogenerated.
	&Mmp_SetIgnoreMissingDef;
	}

	eval { &Mmp_ProcessL($E32env::Data{EPOCPath}, $MMPFILE, \%Plat, $FeatureVariantInfo{BUILD_INCLUDES}); };
	return $@ if $@;

	%WarningLevel=&Mmp_WarningLevel;
	%LinkerOptions=&Mmp_LinkerOptions;
	$ABI=&Mmp_ABI;
	@AifStruct=@{&Mmp_AifStruct};
	$AllowDllData=&Mmp_AllowDllData;
	$CompressTarget=&Mmp_CompressTarget;
	$CompressTargetMode=&Mmp_CompressTargetMode;
	$ASSPExports=&Mmp_ASSPExports;
	@ASSPLibList=&Mmp_ASSPLibList;
	@BitMapStruct=@{&Mmp_BitMapStruct};
	$BuildAsARM=$BuildAsARM || &Mmp_BuildAsARM;
	$CallDllEntryPoints=&Mmp_CallDllEntryPoints;
	$Capability=&Mmp_Capability;
	@CapabilityFlags=&Mmp_CapabilityFlags;
	$DataLinkAddress=&Mmp_DataLinkAddress;
	@DebugLibList=@{&Mmp_DebugLibList};
	%Def=%{&Mmp_Def};
	%DocHash=%{&Mmp_DocHash};
	$ExportUnfrozen=&Mmp_ExportUnfrozen;
	$FirstLib=&Mmp_FirstLib;
	$FixedProcess=&Mmp_FixedProcess;
	%HeapSize=%{&Mmp_HeapSize};
	@LibList=@{&Mmp_LibList};
	$LinkAs=&Mmp_LinkAs;
	$LinkAsBase=&Mmp_LinkAsBase;
	$ExportLibrary=&Mmp_ExportLibrary;
	$NewLib = &Mmp_NewLib;
    $NoExportLibrary=&Mmp_NoExportLibrary;
	%MmpFlag=%{&Mmp_MmpFlag};
	@PlatTxt2D=&Mmp_PlatTxt2D;
	$ProcessPriority=&Mmp_ProcessPriority;
	@RamTargets=&Mmp_RamTargets;
	@ResourceStruct=@{&Mmp_ResourceStruct};
	@RomTargets=&Mmp_RomTargets;
	$SmpSafe=&Mmp_SmpSafe;
	@SourceStruct=@{&Mmp_SourceStruct};
	$StackSize=&Mmp_StackSize;
	@StatLibList=&Mmp_StatLibList;    
	$StdCpp = &Mmp_StdCpp;
	$NoStdCpp = &Mmp_NoStdCpp;
	@SysIncPaths=&Mmp_SysIncPaths;
	$Trg=&Mmp_Trg;
	%TrgType=%{&Mmp_TrgType};
	@UidList=&Mmp_UidList;
	@UserIncPaths=&Mmp_UserIncPaths;
	$SrcDbg=&Mmp_SrcDbg;
	%Version=&Mmp_Version;
	$SecureId=&Mmp_SecureId;
	$VendorId=&Mmp_VendorId;
	%ReplaceOptions=&Mmp_Replace;
	$ARMFPU=&Mmp_ARMFPU;
	@StringTable=@{&Mmp_StringTable};
	$CodePagingTargetMode=&Mmp_CodePagingTargetMode;
	$DataPagingTargetMode=&Mmp_DataPagingTargetMode;
	$IsWideCharMain=&Mmp_IsWideCharMain;
	$IsDebuggable=&Mmp_IsDebuggable;

#	finish defining any macros

	if ($Plat{CPU} eq 'MARM') {
#		apply the ABI source define - note that it is difficult to define a corresponding
#		.MMP define since we can't be sure what the ABI is until we've processed the .MMP file,
#		though we could apply it for generic MARM builds only
		push @{$Plat{Macros}}, "__MARM_${ABI}__";
	}

	if ($TrgType{Basic}=~/^(DLL|EXE)$/o) { # this macro may soon be removed
		push @{$Plat{Macros}},'__'.$TrgType{Basic}.'__';
	}

#	add the macros defined in the .mmp file
	push @{$Plat{Macros}}, &Mmp_Macros;

# set up a hash containing the start paths for various things
	undef %Path;

#	set up ASSP link path - this is the path where the target looks for ASSP-specific import libraries
        $Path{ASSPLink}="$E32env::Data{LinkPath}$Plat{ASSP}\\";

#	set up build path
	my $BldInfPath=cwd;
	$BldInfPath=~s-/-\\-go;						# separator from Perl 5.005_02+ is forward slash
	$BldInfPath=~s/^(.:)//o;					# remove drive letter
	$BldInfPath=~s-^(.*[^\\])$-$1\\-o;			# ensure workpath ends with a backslash
	$Path{BldInfPath} = $BldInfPath;			# Remember the path to bldinf
	$Path{Bld}=join('', &Path_Chop($E32env::Data{BldPath}), $BldInfPath, &Path_Split('Base',$MMPFILE), "\\$Plat{Real}\\");
}

sub generateX86GCCDefFile()
{
	my ($oldPath, $newPath) = @_;
	print "Generating X86GCC deffile from $oldPath\n";
	if (!open(OLDDEF, $oldPath))
	{
		print "Warning: Failed to open $oldPath for reading\n".
		return;
	}
	# Force creation of new def file even if one already exists
	if (!open(NEWDEF, ">$newPath"))
	{
		close OLDDEF;
		print "Warning: Failed to open $newPath for writing\n".
		return;
	}
	while (<OLDDEF>)
	{
		chomp;
		if (/^\s*_ZT(I|V)/ && !/\sABSENT/i)
	    {
		# Typeinfo that isn't already absent. Add the ABSENT keyword.
		    my @frags = split /;/, $_, 2; # 1 or 2 parts depending on the presence of a comment
		    $frags[0] =~ s/\s*$/ ABSENT /;
		    $_ = join ';', @frags;
	    }
	    else
	    {
		# Try to substitute any va_list params with char* (Pc). Ignore if no match
		s/St9__va_list/Pc/g;
	    }
		print NEWDEF "$_\n";	    
	}
	close OLDDEF;
	close NEWDEF;
	print "Successfully generated $newPath\n";
}

sub InitLinkPaths() {
#	set up lib path - this is the path where the target puts it's import library

	my $ABI=&Mmp_ABI;

#	let the build target determine where it puts its import libray and where its links against its imports
	my $UnderlyingABI=$ABI;
	$UnderlyingABI=&PMUnderlyingABI($ABI) if defined &PMUnderlyingABI;

	$Path{Lib}="$E32env::Data{LinkPath}";
	unless ($ASSPExports) {
		$Path{Lib}.= lc($UnderlyingABI)."\\";
	} 
	else {
		$Path{Lib}.= lc($Plat{ASSP})."\\";
	}


#	set up link path - this is the place where the target looks for ordinary libraries
	$Path{Link}="$E32env::Data{LinkPath}"."$UnderlyingABI\\";

#	set up stat link path - this is where the target looks for static libraries
	$Path{StatLink}="$E32env::Data{LinkPath}";
	if ($Plat{StatLink}) {
			$Path{StatLink}.=lc($Plat{StatLink})."\\";
	} else {
		unless ($Plat{OS} eq 'WINS') {	# WINC and WINS versions of EEXE are different
			$Path{StatLink}.=lc($ABI)."\\"; # ARM static libraries are currently always ASSP-independent
		}
		else {
			$Path{StatLink}.=lc($Plat{ASSP})."\\"; # WINC static libraries are currently always ASSP-specific
		}
	}

#	set up release path
	$Path{Rel}="$E32env::Data{RelPath}".lc($Plat{Real})."\\";
	
}

sub CreateExtraFile ($$) { # takes abs path for source and text
# allows modules to create extrafiles
	my ($FILE,$Text)=@_;
	if ($Options{makemakefile}) {	# only create if making the makefile
		if ($Options{v}) {
			print "Creating \"$FILE\"\n";
		}
		eval { &Path_MakePathL($FILE); };
		die $@ if $@;
		open FILE, ">$FILE" or die "WARNING: Can't open or create \"$FILE\"\n";
		print  FILE $Text or die "WARNING: Can't write text to \"$FILE\"\n";
		close FILE or die "WARNING: Can't close \"$FILE\"\n";
	}
}

sub ABI () {
	$ABI;
}
sub AddSrc ($$) { # needs abs path for source
# allows modules to add a source file to the project and have it created if necessary
	my ($SRCFILE,$Text)=@_;
	my $SrcPath=&Path_Split('Path',$SRCFILE);
	my $CurFile=&Path_Split('File',$SRCFILE);
	my $BaseName=&Path_Split('Base',$SRCFILE);
	
	if ((not -e $SRCFILE) || (-M $SRCFILE > -M $MMPFILE)) {
		# only create the file if it's older than the .MMP file
		CreateExtraFile($SRCFILE,$Text);
	}
	 my %CurSource;
	 $CurSource{SrcPath}=$SrcPath;
	 $CurSource{CurFile}=$CurFile;
	 $CurSource{BaseTrg}=$BaseName;
	 push @SourceStruct, \%CurSource;
}
sub AddPlatMacros (@) {
# allows modules to add extra macros to the platform macro list
	push @{$Plat{Macros}},@_;
}
sub AifRef () {
	$CurAifRef;
}
sub AifStructRef () {
	\@AifStruct;
}
sub AllowDllData () {
	$AllowDllData;
}
sub CompressTarget () {
	$CompressTarget;
}
sub CompressTargetMode () {
	$CompressTargetMode;
}
sub BuildAsARM () {
	return 0 if $Options{ithumb};
	return 1 if $Options{iarm};
	$BuildAsARM;
}
sub ASSPLibList () {
	@ASSPLibList;
}
sub ASSPLinkPath () {
#	this is the path where the target looks for ASSP-specific import libraries
	my $Path=$Path{ASSPLink};
	if ($CurBld) {
		if ($Plat{OS} eq 'EPOC32') {
			$Path.="UREL\\";
		}
		else {
			$Path.="UDEB\\";
		}
	}
	$Path;
}
sub BaseMak () {
	&Path_Split('Base',$MAKEFILE);
}
sub BaseResrc () {
	&Path_Split('Base',$CurResrc);
}
sub BaseResrcList () {
	my @ResrcList=&ResrcList;
	my $Path;
	foreach $Path (@ResrcList) {
		$Path=&Path_Split('Base',$Path);
	}
	@ResrcList;
}
sub BaseSrc () {
	&Path_Split('Base',$CurSrc);
}
sub ExtSrc () {
	&Path_Split('Ext',$CurSrc);
}
sub BaseSrcList () {
	my @SrcList=&SrcList;
	my $Path;
	foreach $Path (@SrcList) {
		$Path=&Path_Split('Base',$Path);
	}
	@SrcList;
}
sub BaseSysResrcList () {
	my @SysResrcList=&SysResrcList;
	my $Path;
	foreach $Path (@SysResrcList) {
		$Path=&Path_Split('Base',$Path);
	}
	@SysResrcList;
}
sub BaseTrg () {
	&Path_Split('Base',$Trg);
}
sub BitMapRef () {
	$CurBitMapRef;
}
sub BitMapStructRef () {
	\@BitMapStruct;
}
sub Bld () {
	$CurBld;
}
sub BldList () {
	@{$Plat{Blds}};
}
sub BldPath () {
	my $Path=$Path{"Bld"};
	if ($CurBld) {
		$Path.=$FeatureVariantInfo{$CurBld."_LABEL"}."\\" if (%FeatureVariantInfo && !$FeatureVariantInfo{INVARIANT} && $FeatureVariantInfo{$CurBld."_LABEL"});
		$Path.="$CurBld\\";
	}
	$Path;
}
sub CallDllEntryPoints () {
	$CallDllEntryPoints;
}
sub Capability () {
	$Capability;
}
sub CapabilityFlags () {
	@CapabilityFlags;
}
sub DataLinkAddress () {
	$DataLinkAddress;
}
sub DataPath () {
	$E32env::Data{DataPath};
}
sub DebugLibList () {
	@DebugLibList;
}
sub DefFile () {
	"$Def{Path}$Def{Base}$Def{Ext}";
}
sub DefFileType () {
	$Plat{DefFile};
}
sub DepList () {
	sort @CurDepList;
}
sub Doc () {
	$CurDoc;
}
sub DocList () {
	if ($CurSrcPath) {
		return sort @{$DocHash{$CurSrcPath}};
	}
	my @DocList;
	my $Key;
	foreach $Key (keys %DocHash) {
		push @DocList,@{$DocHash{$Key}};
	}
	sort @DocList;
}
sub EPOCPath () {
	$E32env::Data{EPOCPath};
}
sub EPOCDataPath () {
	$E32env::Data{EPOCDataPath};
}
sub EPOCIncPath () {
	$E32env::Data{EPOCIncPath};
}
sub EPOCRelPath () {
	$E32env::Data{RelPath};
}
sub EPOCSecurePlatform () {
	$E32env::Data{SecurePlatform};
}
sub EPOCToolsPath () {
	$E32env::Data{EPOCToolsPath};
}
sub Exports () {
	@{$TrgType{Exports}{$Plat{"DefFile"}}};
}
sub ExportUnfrozen () {
	$ExportUnfrozen;
}
sub FirstLib () {
	$FirstLib;
}
sub FixedProcess () {
	$FixedProcess;
}
sub BasicTrgType () {
	$TrgType{Basic};
}
sub HeapSize () {
	%HeapSize;
}
sub LibList () {
	@LibList;
}
sub LibPath () {
#	this is the path where the target puts it's import library
	my $Path=$Path{Lib};
	if ($CurBld) {
		if (($Plat{DefFile} eq 'EABI') || ($Plat{DefFile} eq 'x86gcc') || ($Plat{OS} eq 'TOOLS2')) {
			$Path.="lib\\";
		}
		elsif ($Plat{OS} eq 'EPOC32') {
			$Path.="urel\\";
		}
		else {
			$Path.="udeb\\";
		}
	}
	$Path;
}
sub LinkAs () {
	$LinkAs;
}
sub LinkAsBase () {
	$LinkAsBase;
}
sub ExportLibrary () {
	$ExportLibrary;
}
sub NoExportLibrary () {
	$NoExportLibrary;
}
sub LinkPath () {
#	this is the place where the target looks for CPU-specific libraries
	my $Path=$Path{Link};
	if ($CurBld) {
		if ($Plat{DefFile} eq 'EABI' || $Plat{DefFile} eq 'x86gcc') {
			$Path.="LIB\\";
		}
		elsif ($Plat{OS} eq 'EPOC32') {
			$Path.="UREL\\";
		}
		else {
			$Path.="UDEB\\";
		}
	}
	$Path;
}

sub MacroList ($) {
	if ($_[0]) {
	return @{$BldMacros{$_[0]}};
	}
	return @{$Plat{Macros}} unless $CurBld;
	(@{$Plat{Macros}},@{$BldMacros{$CurBld}});
}

# returns the file containing Variant specific Macros
sub VariantFile($)
{
    return $variantMacroHRHFile;
}
# returns the file containing Variant specific Macros, which may differ from the above if feature variants are used
sub ResourceVariantFile($)
{
    return $ResourceVariantMacroHRHFile;
}
sub MakeFilePath () {
	&Path_Split('Path',$MAKEFILE);
}
sub MmpFile () {
	$MMPFILE;
}
sub PerlLibPath () {
	$PerlLibPath;
}
sub Plat () {
	$Plat{Real};
}
sub PlatABI () {
	$Plat{"ABI"};
}
sub PlatCompiler () {
	$Plat{"Compiler"};
}
sub PlatName () {
	$Plat{Name};
}
sub PlatOS () {
	$Plat{OS};
}
sub PlatOverrideList () {
	@PlatOverrideList;
}
sub ProcessPriority () {
	$ProcessPriority;
}
sub RelPath () {
	my $Path=$Path{Rel};
	if ($CurBld) {
		$Path .= lc($CurBld)."\\";
	}
	$Path;
}
sub ResourceRef () {
	$CurResourceRef;
}
sub ResourceStructRef () {
	\@ResourceStruct;
}
sub SetCurBld($) {
	$CurBld=$_[0];		# used by ide_cw.pm when handling additional platforms
}
sub	SetStdIncPaths (@) {
# allows module to set standard include paths
	@StdIncPaths=();
	my $Path;
	foreach $Path (@_) {
		$Path=~s-^(.*[^\\])$-$1\\-o if defined($Path);
		push @StdIncPaths, $Path;	# only place drive letters may appear, up to modules to handle
	}
}
sub Src () {
	$CurSrc;
}
sub SourceStructRef () {
	\@SourceStruct;		#array of references to hashes of SOURCEPATH => (filename1, filename2, ...)
	}
sub SrcList () {
	my @SrcList;
    my $KeyRef;
    foreach $KeyRef (@SourceStruct) {
            push @SrcList,$$KeyRef{CurFile};
           } 
	 @SrcList;
}

sub SmpSafe () {
	$SmpSafe;
}

sub StackSize () {
	$StackSize;
}
sub StatLibList () {
	@StatLibList;
}
sub StatLinkPath () {
	my $Path=$Path{StatLink};
	if ($CurBld) {
		$Path.="$CurBld\\";
	}
	$Path;
}
sub StdCpp () {
	$StdCpp;
}
sub NoStdCpp () {
	$NoStdCpp;
}

sub NewLib () {
	$NewLib;
}
sub SetStatLinkPath($) {
	($Path{StatLink}) = @_;
}
sub SrcPath () {
	$CurSrcPath;
}
sub SysIncPaths () {
	@SysIncPaths;
}
sub ResourceSysIncPaths () {
	return @ResourceSysIncPaths;
}
sub Trg (;$) {
	# The optional $bld argument permits (U)DEB and (U)REL distinction in situations where $CurBld isn't set/relevant
	my ($bld) = @_;
	$bld = $CurBld if (!$bld);

	return "" if !$Trg;

	my $localTrg = $Trg;	
	my $vinfo = $FeatureVariantInfo{$bld."_LABEL"};
	$localTrg =~ s/(\.[^\.]+)$/\.$vinfo$1/ if ($bld && %FeatureVariantInfo && !$FeatureVariantInfo{INVARIANT} && $vinfo);
	return $localTrg;
}
sub TrgPath () {
	$TrgType{Path};
}
sub TrgType () {
	$TrgType{Name};
}
# this can probably go as its always 0 now
sub KernelTrg () {
	$TrgType{Kernel};
}
sub SystemTrg () {
	$TrgType{System};
}
sub UidList () {
	@UidList;
}
sub UserIncPaths () {
	@UserIncPaths;
}
sub SrcDbg () {
	$SrcDbg;
}
sub CompilerOption
{
	my $CompOption=$WarningLevel{$_[0]};
	$CompOption="" if (!defined($CompOption)); 
	$CompOption;
}
sub LinkerOption
{
    my $lnkOption = $LinkerOptions{$_[0]};
    $lnkOption="" if (!defined($lnkOption));
    $lnkOption;
}

sub PlatRec () {
	%Plat;
}

sub Version() {
	%Version;
}

sub SecureId() {
	$SecureId;
}

sub VendorId () {
	$VendorId;
}

sub EABIDef () {
	$EABIDef;
}

sub ReplaceOptions () {
	my @ReplacementOptions = ();
	if (defined($ReplaceOptions{$_[0]}))
	{
		@ReplacementOptions=@{$ReplaceOptions{$_[0]}};
	}	
	@ReplacementOptions;
}

sub ARMFPU () {
	$ARMFPU;
}

sub IsDebuggable () {
	$IsDebuggable;
}

sub PlatTxt2D () {
	@PlatTxt2D;
}

sub ToolChain () {
	my $ToolChain = $TruePlat{Toolchain};
	$ToolChain ="" if(!defined ($TruePlat{Toolchain}) );
	$ToolChain;
}

sub StringTables () 
{
	@StringTable;
}


#This generates and adds the generated source of the stringtable to the source list so it's included in the binary.
sub AddStringTables ()
{
	if(@StringTable)
	{
		foreach my $stringtable (@StringTable)
		{
			my %genSrcHash;

			$genSrcHash{SrcPath} = $Path{Bld};
			$genSrcHash{CurFile} = $stringtable->{BaseTrg}.".cpp";
			$genSrcHash{BaseTrg} = $stringtable->{BaseTrg};

			push(@SourceStruct, \%genSrcHash) if !$stringtable->{Hdronly};

			# Execute it now.  This can probably be moved into the makefile by adding a suitable rule
			# to ensure the resource file is generated before the source tries to #include it.
			my $stPath = $Path{Bld};

			my $stCpp = $stringtable->{BaseTrg}.".cpp";
			my $stHeader = lc($stringtable->{BaseTrg}).".h";
			my $stFile = $stringtable->{STFile};
			my $stOrigin = $stringtable->{STPath};
			system("perl -S ecopyfile.pl $stOrigin$stFile $stPath$stFile");
			system("perl -S stringtable.pl $stPath$stFile");

			#If it's an exported header we'll need to export it.
			if(defined $stringtable->{ExportPath}) 
			{
				my $exportpath = $stringtable->{ExportPath};

				system("perl -S ecopyfile.pl $stPath$stHeader $exportpath\\$stHeader");

				push(@UserIncPaths, $exportpath);
				push(@StringTableUserIncPaths, $exportpath);
			}
			#otherwise we just need the path of the generated header to be added 'userinclude'.
			else 
			{
				push(@UserIncPaths, $Path{Bld});
				push(@StringTableUserIncPaths, $Path{Bld});
			}
		}
	}
}

sub CheckSourceMMPMetaData () {
	%CheckSourceMMPMetaData;
}

sub CheckSourceMMPIncludes () {
	%CheckSourceMMPIncludes;
}

sub CheckSourceURELIncludes () {
	%CheckSourceURELIncludes;
}

sub CheckSourceUDEBIncludes () {
	%CheckSourceUDEBIncludes;
}

sub CodePagingTargetMode() {
	$CodePagingTargetMode;
}

sub DataPagingTargetMode() {
	$DataPagingTargetMode;
}

sub DebugSwitchUsed () {
	return 1 if (defined $DebugSwitch);
	return 0;
}

sub SymbolicDebugEnabled () {
	return $DebugSwitch;
}

sub IsFunctionCallLogging() {
	$Options{logfc};
}

sub IsWideCharMain() {
	return $IsWideCharMain;
}

sub MmpMacros(){
	my @mmpMacros=&Mmp_Macros;
	return @mmpMacros;
}

sub FeatureVariantInfo()
	{
	return %FeatureVariantInfo;
	}

sub FeatureVariantVMapFile()
	{
	return "" if !%FeatureVariantInfo || defined $FeatureVariantInfo{INVARIANT};

	my $target = RelPath().Trg();
	$target =~ s/\.$FeatureVariantInfo{$CurBld."_LABEL"}//;
# modified by SV start: makefile improvement 
	my $vmap = $target. "." . $FeatureVariantInfo{NAME}.".vmap";
# modified by SV end: makefile improvement 

	eval { &Path_MakePathL($vmap); };	

	if (featurevariantmap->Save($target, $FeatureVariantInfo{$CurBld."_LABEL"}, $FeatureVariantInfo{NAME}, $FeatureVariantInfo{$CurBld."_FEATURES"}, &Mmp_IsFeatureVariant ? [ 'FEATUREVARIANT' ] : undef))
		{
		die "ERROR: Couldn't create feature variant map file \"$vmap\".\n";
		}

	return $vmap;
	}

sub FeatureVariantBaseTrg()
	{
	# In some situations, for example .sym files, we need a feature variant tagged name
	# based on the normal BaseTrg i.e. root target name but minus extension.
	my $localBaseTrg = BaseTrg();
		
	if (%FeatureVariantInfo && !defined $FeatureVariantInfo{INVARIANT})
		{
		$localBaseTrg .= ".".$FeatureVariantInfo{$CurBld."_LABEL"};
		}

	return $localBaseTrg;
	}

sub CommandFile()
	{
	my $plat = Plat();	
	$plat .= ".$FeatureVariantInfo{NAME}" if %FeatureVariantInfo;
	return MakeFilePath().BaseTrg().".$plat.".Bld().".objects.via";
	}

#Compiler wrapper option support
sub CompilerWrapperOption()
{
  # Return 1 if compiler wrapper option option(-wrap) is specified else return 0	
  $IsCompilerWrapperOption;
}

#Proxy wrapper option support
sub ProxyWrapperOption()
{
  # Return 1 if compiler wrapper option option(-wrap) is specified else return 0	
  $IsProxyWrapperOption;
}
# modified start: makefile improvement 
sub getSrcSet()
{
	return \%CurSrcSet;
}
# modified end: makefile improvement 

#1 = STDCPP support is available
#others = STDCPP support is not available
my $stdcppsupport;

#STDCPP support check (SYMBIAN_OE_LIBSTDCPP)
#return non-zero means STDCPP support is available,
#otherwise it's not available
sub StdCppSupport()
{
	return $stdcppsupport if (defined $stdcppsupport);
	
	my @hrhMacros = &Variant_GetMacroList;
	if (grep /^\s*SYMBIAN_OE_LIBSTDCPP\s*$/, @hrhMacros)
	{
		$stdcppsupport = 1;
	}
	else
	{
	    $stdcppsupport = 0;
	}
	
	return $stdcppsupport;
}
