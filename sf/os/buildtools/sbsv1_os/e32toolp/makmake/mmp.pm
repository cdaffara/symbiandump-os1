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
# Processes an mmp file and sets up subroutines to return the data
# 
#

package Mmp;

require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	Mmp_SetVerbose
	Mmp_ProcessL

	Mmp_ABI
	Mmp_AifStruct
	Mmp_AllowDllData
	Mmp_CompressTarget
	Mmp_ASSPExports
	Mmp_ASSPLibList
	Mmp_BuildAsARM
	Mmp_BitMapStruct 
	Mmp_CallDllEntryPoints
	Mmp_Capability
	Mmp_CapabilityFlags
	Mmp_DataLinkAddress
	Mmp_DebugLibList 
	Mmp_Def
	Mmp_DocHash
	Mmp_ExportUnfrozen
	Mmp_FirstLib
	Mmp_FixedProcess
	Mmp_HeapSize
	Mmp_LibList 
	Mmp_LinkAs
	Mmp_LinkAsBase
	Mmp_ExportLibrary
	Mmp_NewLib
	Mmp_NoExportLibrary
	Mmp_Macros
	Mmp_MmpFlag
	Mmp_PlatTxt2D
	Mmp_ProcessPriority
	Mmp_ResourceStruct 
	Mmp_SmpSafe
	Mmp_RamTargets
	Mmp_RomTargets
	Mmp_SourceStruct 
	Mmp_StackSize
	Mmp_StatLibList 
	Mmp_SysIncPaths
	Mmp_Trg
	Mmp_TrgType
	Mmp_UidList
	Mmp_UserIncPaths
	Mmp_SrcDbg
	Mmp_StdCpp
	Mmp_NoStdCpp
	Mmp_WarningLevel
	Mmp_LinkerOptions
	Mmp_Reset
	Mmp_Uids
	Mmp_Version
	Mmp_SecureId
	Mmp_VendorId
	Mmp_Replace
	Mmp_ARMFPU	
	Mmp_SetIgnoreMissingDef 
	Mmp_StringTable
	Mmp_CompressTargetMode
	Mmp_CodePagingTargetMode
	Mmp_DataPagingTargetMode
	Mmp_CheckSourceMMPMetaData
	Mmp_CheckSourceMMPIncludes
	Mmp_IsWideCharMain
	Mmp_IsDebuggable
	Mmp_IsFeatureVariant
	Mmp_TestedMacros
);

use strict;

use Genutl;
use Prepfile;
use Pathutl;
use Trgtype;
use CheckSource;
use Cwd;

my %ProcessPriorityNames = (
	LOW=>'Low',
	BACKGROUND=>'Background',
	FOREGROUND=>'Foreground',
	HIGH=>'High',
	WINDOWSERVER=>'WindowServer',
	FILESERVER=>'FileServer',
	REALTIMESERVER=>'RealTimeServer',
	SUPERVISOR=>'SuperVisor'
);

my %CapabilityNames;

my %Mode;

my $ABI;
my @AifStruct;
my $AllowDllData=0;

use constant COMPRESS => 0;
use constant NOCOMPRESS => 1;
my $CompressTarget=COMPRESS;	# compress the target binary (negative logic)
use constant NOCOMPRESSIONMETHOD => 0;
use constant INFLATECOMPRESSIONMETHOD => 1;
use constant BYTEPAIRCOMPRESSIONMETHOD => 2;
my $CompressTargetMethod=NOCOMPRESSIONMETHOD;   #NONE

my $ASSPABISwitch=0;
my $ASSPExports=0;
my @ASSPLibList;
my @BitMapStruct;
my $BuildAsARM=0;
my $CallDllEntryPoints=0;
my $Capability;
my @CapabilityFlags=(0,0);
my %CurSource;
my $DataLinkAddress='';
my @DebugLibList;
my %Def;
$Def{Path}='';
$Def{Base}='';
$Def{Ext}='';
my %DocHash;
my $ExportUnfrozen=0;
my $FirstLib;
my $FixedProcess=0;
my %HeapSize;
my @LangList;
my @LibList;
my $LinkAs;
my $LinkAsBase;
my $ExportLibrary;
my $NoExportLibrary;
my @Macros;
my %MmpFlag;
my $NewLib;
my @PlatTxt2D;
my $ProcessPriority='';
my @ResourceStruct;
my @RamTargets;
my @RomTargets=({}); # include default
my $SmpSafe = 0;
my @SourceStruct;
my $StackSize='';
my @StatLibList;
my $StdCpp = 0;
my $NoStdCpp = 0;
my @SysIncPaths;
my $Trg;
my %TrgType;
my @UidList;
my @UserIncPaths;
my $SrcDbg=0;
my %Compiler;
my %LinkerOptions;
my %Version;
my $SecureId;
my $VendorId;
my %Replace;
my $ARMFPU;
my $IgnoreMissingDef=0;
my @StringTableStruct;
my $CodePagingTargetMode;	# 0-N/A, 1-UNPAGED, 2-PAGED
my $DataPagingTargetMode;	# 0-N/A, 1-UNPAGED, 2-PAGED
my $WideCharMain=0;
my $IsDebuggable;	# 0-NON_DEBUGGABLE, 1-DEBUGGABLE,2-DEBUGGABLE_UDEBONLY
my $FeatureVariant=0;
use constant NOTPAGED => 0;
use constant UNPAGED => 1;
use constant PAGED => 2;
use constant NON_DEBUGGABLE => 0;
use constant DEBUGGABLE => 1;
use constant DEBUGGABLE_UDEBONLY => 2;
my %CheckSourceMMPMetaData;
my %CheckSourceMMPIncludes;
my %mmptestedMacrosHash;

# List of deprecated 2nd UIDs. These are checked in a Secure Platform
my %deprecatedUIDs=
    (
    "0x10005e32" => "Unmigrated FEP detected from use of UID 0x10005e32 ",
    "0x10004cc1" => "Unmigrated Application Initaliser (CEikLibrary deriver) detected from use of UID 0x10004cc1 ",
    "0x10003a30" => "Unmigrated Conarc plugin detected from use of UID 0x10003a30",
    "0x10003a19" => "Unmigrated Recogniser detected from use of UID 0x10003a19",
    "0x10003a37" => "Unmigrated Recogniser detected from use of UID 0x10003a37",
    "0x10003a34" => "Unmigrated CTL detected from use of UID 0x10003a34"
    );


sub Mmp_Reset() {
	undef $ABI;
	undef @AifStruct;
	$AllowDllData=0;
	$CompressTarget=COMPRESS;
    $CompressTargetMethod=NOCOMPRESSIONMETHOD;
	$ASSPABISwitch=0;
	$ASSPExports=0;
	$BuildAsARM=0;
	undef @ASSPLibList;
	undef @BitMapStruct;
	$CallDllEntryPoints=0;
	undef $Capability;
	@CapabilityFlags=(0,0);
	undef $VendorId;
	$DataLinkAddress='';
	undef @DebugLibList;
	undef %Def;
	$Def{Path}='';
	$Def{Base}='';
	$Def{Ext}='';
	undef %DocHash;
	$ExportUnfrozen=0;
	undef $FirstLib;
	$FixedProcess=0;
	undef %HeapSize;
	undef @LangList;
	undef @LibList;
	undef $LinkAs;
	undef $LinkAsBase;
	undef $ExportLibrary;
	undef @Macros;
	undef %MmpFlag;
	undef $NewLib;
	undef @PlatTxt2D;
	$ProcessPriority='';
	undef @ResourceStruct;
	undef @RamTargets;
	@RomTargets=({}); # include default
	undef @SourceStruct;   
	$StackSize='';
	undef @StatLibList;
	$SmpSafe = 0;
	$StdCpp = 0;
	$NoStdCpp = 0;
	undef @SysIncPaths;
	undef $Trg;
	undef %TrgType;
	undef @UidList;
	undef @UserIncPaths;
	$SrcDbg=0;
	undef %Compiler;
	undef %LinkerOptions;
    undef %Version;
    $IgnoreMissingDef=0;
    undef $ARMFPU;
	undef @StringTableStruct;
	$CodePagingTargetMode=0;	# default N/A
	$DataPagingTargetMode=0;	# default N/A
	$IsDebuggable=0; # Default = 0 (Not debuggable)
	undef %mmptestedMacrosHash;
}

BEGIN {
	$Mode{'Verbose'}=0;

	%CapabilityNames = (

		TCB =>					(1<<0),
		COMMDD =>				(1<<1),
		POWERMGMT =>			(1<<2),
		MULTIMEDIADD =>			(1<<3),
		READDEVICEDATA =>		(1<<4),
		WRITEDEVICEDATA =>		(1<<5),
		DRM =>					(1<<6),
		TRUSTEDUI =>			(1<<7),
		PROTSERV =>				(1<<8),
		DISKADMIN =>			(1<<9),
		NETWORKCONTROL =>		(1<<10),
		ALLFILES =>				(1<<11),
		SWEVENT =>				(1<<12),
		NETWORKSERVICES =>		(1<<13),
		LOCALSERVICES =>		(1<<14),
		READUSERDATA =>			(1<<15),
		WRITEUSERDATA =>		(1<<16),
		LOCATION =>				(1<<17),
		SURROUNDINGSDD =>		(1<<18),
		USERENVIRONMENT =>		(1<<19),
#old capability names have zero value
		ROOT =>				0,
		MEDIADD =>			0,
		READSYSTEMDATA =>	0,
		WRITESYSTEMDATA =>	0,
		SOUNDDD =>			0,
		UIDD =>				0,
		KILLANYPROCESS =>	0,
		DEVMAN =>			0,
		PHONENETWORK =>		0,
		LOCALNETWORK =>		0
	);
	my $all=0;
	foreach (keys %CapabilityNames)
		{
		$all = $all | $CapabilityNames{$_};
		}
	$CapabilityNames{"ALL"} = $all;
}

sub Mmp_SetVerbose () {
	$Mode{'Verbose'}=1;
}

sub Mmp_SetIgnoreMissingDef() {
    $IgnoreMissingDef = 1;
}

sub Mmp_ProcessL ($$$$) {
	my ($EPOCPath, $MMPFILE, $Plat_ref, $BuildIncList)=@_;
	my $emulator = $$Plat_ref{OS} ne 'EPOC32';

	if ($Mode{Verbose}) {
		&Prepfile_SetVerbose;
	}

	# Generate macro usage information and add variant build includes
	my $options = '-dU';
	foreach ( @$BuildIncList )
		{
		$options .= ' -I ' . &Path_PrefixWithDriveAndQuote($_);
		}

#	preprocess the project description file
	my @Mmp2D;
	Prepfile_SetCmdOptions($options);
	eval { &Prepfile_ProcessL(\@Mmp2D, $MMPFILE, &main::VariantFile(), @{$$Plat_ref{MmpMacros}}); };
	Prepfile_SetCmdOptions('');
	die $@ if $@;

	my @checkSourceMacros;
	foreach (@{$$Plat_ref{MmpMacros}}) {
		$_=uc $_;
		push @checkSourceMacros, " $_=_____$_";
	}

	my $current = cwd;
	$current =~ s/\//\\/g;
	$current =~ s/^[a-zA-Z]://;

	# Mimic what the current parsing routine does in terms of include paths and .mmp files
	my @checkSourceUserIncludePaths;
	push @checkSourceUserIncludePaths, "$ENV{EPOCROOT}epoc32\\include";
	push @checkSourceUserIncludePaths, "\.";
	push @checkSourceUserIncludePaths, $current;
	my @checkSourceSystemIncludePaths = ();

	CheckSource_Includes($MMPFILE, %CheckSourceMMPIncludes, &main::VariantFile(), @checkSourceMacros, @checkSourceUserIncludePaths, @checkSourceSystemIncludePaths, $CheckSource_NoUserSystemDistinction);

	my $NoStrictDef=0;
	my $TrgPath='';
	my $TrgType;

	my $MmpPath=&Path_Split('Path', $MMPFILE);

	my @MmpDie=();
	my @MmpWarn=();
	my @MmpDiagnostic=();
	my @MmpMigrationNote=();

	my (%CheckAif, %CheckASSPLib, %CheckDoc, %CheckLang, %CheckLib, %CheckMacro, %CheckResrc, %CheckSrc, %CheckStatLib, %CheckSysInc, %CheckSysResrc, %CheckUserInc);

	my ($CheckDef);

	my ($CheckRamTargets, $CheckRomTargets);

	my ($OtherPlatSwitch, $PlatTxtSwitch);

	my (%CurBitMap, $CurBitMapSrcPath);
	$CurBitMapSrcPath=$MmpPath;

	my (%CurResource, %ResourceCheckLang);

	my %CurStringTable;
	my ($CurSrcPath, $MmpMacro, $Line);
	$CurSrcPath=$MmpPath;

#	include the .MMP file as a document
	@{$DocHash{$MmpPath}}=(&Path_Split('File', $MMPFILE));
	$CheckDoc{$MMPFILE}='zero - specified by default';

# process the data structure
	my $CurFile=$MMPFILE;
	LINE: foreach $Line (@Mmp2D) {
		my $LineNum=shift @$Line;
		$_=shift @$Line;
		if ($LineNum eq '#') {
			$CurFile=$_;
			next LINE;
		}

		# Get details of tested macros affecting the MMP file
		# We get this from -dU preprocessor option
		if (/^#define$/ && "@$Line" =~ /^(\w+(?:\([^\)]*\))?)(?:\s(\S.*?))?\s*$/)
			{
			# Record macro details for defined tested macro
			$mmptestedMacrosHash{$1} = $2 ? "\'$2\'" : 'defined';
			next LINE;
			}
		elsif (/^#undef$/)
			{
			# Record macro details for undefined tested macro
			$mmptestedMacrosHash{"@$Line"} = 'undefined';
			next LINE;
			}
				
		$CurFile = &Path_Norm ($CurFile);
		
		$_=uc $_;

		my $mainElement = $_;
		
		if ($PlatTxtSwitch) {
			if (/^END$/o) {
				$PlatTxtSwitch=0;
				next LINE;
			}
			push @PlatTxt2D, [ "$CurFile($LineNum)", $_, @$Line ];
			next LINE;
		}
		if ($OtherPlatSwitch) {
			if (/^END$/o) {
				$OtherPlatSwitch=0;
				next LINE;
			}
			next LINE;
		}
		# ----------- handle body of START BITMAP ... END -------------
		if (%CurBitMap) {
			if (/^SOURCE$/o) {
				unless (@$Line>1) {
					push @MmpDie, "$CurFile($LineNum) : Not enough arguments for Bitmap keyword SOURCE\n";
				}
				my $cldepth = shift @$Line;
				my @ClDepths = split(/,/, $cldepth);
				foreach (@ClDepths) {
					$_ = lc $_; # bmconv can't handle upper-case 'C's
					unless (/^c?\d\d?a?$/o) {
						push @MmpDie, "$CurFile($LineNum) : BITMAP color depth \"$_\" - unexpected format\n";
					}
				}
				@ClDepths = (@ClDepths) x @$Line;	# make a sufficiently long list of color depths
				foreach (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "BITMAP SOURCE", $_, $LineNum, $CheckSource_PhysicalCheck, $CurBitMapSrcPath);
					$_ = &Path_Norm($_);
					
					$_=lc $_;	# bmconv generates a header with case-sensitive enums
					my $bitmap = "$CurBitMapSrcPath$_";
					push @{$CurBitMap{Source}}, { # sources must be kept in order
						Src=>$bitmap,
						ClDepth=>shift @ClDepths	# take next color depth from the list
					};
					unless (-e $bitmap) {
						push @MmpWarn, "$CurFile($LineNum) : BITMAP source \"$CurBitMapSrcPath$_\" not found\n";
					}
				}
				next LINE;
			}
			if (/^END$/o) {
				$CurBitMapSrcPath=$MmpPath;
				my %BitMap=%CurBitMap;
				undef %CurBitMap;
				push @BitMapStruct, \%BitMap;
				next LINE;
			}
			if (/^SOURCEPATH$/o) {
				unless ($CurBitMapSrcPath=shift @$Line) {
					push @MmpDie, "$CurFile($LineNum) : No path specified with Bitmap keyword SOURCEPATH\n";
					next LINE;
				}
				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "BITMAP SOURCEPATH", $CurBitMapSrcPath, $LineNum, $CheckSource_PhysicalCheck);
				$CurBitMapSrcPath = &Path_Norm($CurBitMapSrcPath);
				
				$CurBitMapSrcPath=~s-^(.*[^\\])$-$1\\-o;   # ensure path ends with a backslash
				$CurBitMapSrcPath=&Path_MakeAbs($CurFile,$CurBitMapSrcPath);
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Too many arguments for Bitmap keyword SOURCEPATH\n";
				}
				next LINE;
			}
			if (/^HEADER$/o) {
				if ($CurBitMap{Hdr}) {
					push @MmpWarn, "$CurFile($LineNum) : Bitmap HEADER already specified at line $CurBitMap{Hdr}\n";
					next LINE;
				}
				$CurBitMap{Hdr}="$CurFile($LineNum)";
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Bitmap keyword HEADER takes no arguments\n";
				}
				next LINE;
			}


			if (/^TARGETPATH$/o) {
				if ($CurBitMap{TrgPath}) {
					push @MmpWarn, "$CurFile($LineNum) : Bitmap TARGETPATH already specified\n";
					next LINE;
				}
				unless ($CurBitMap{TrgPath}=shift @$Line) {
					push @MmpDie, "$CurFile($LineNum) : No path specified with Bitmap keyword TARGETPATH\n";
					next LINE;
				}
				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "BITMAP TARGETPATH", $CurBitMap{TrgPath}, $LineNum);
				$CurBitMap{TrgPath} = &Path_Norm($CurBitMap{TrgPath});

				$CurBitMap{TrgPath}=~s-^\\(.*)$-$1-o;        # remove leading backslash, if any
				$CurBitMap{TrgPath}=~s-^(.*[^\\])$-$1\\-o;   # ensure path ends with a backslash
				$CurBitMap{TrgPath}="z\\$CurBitMap{TrgPath}";
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Too many arguments for Bitmap keyword TARGETPATH\n";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : Unrecognised Bitmap Keyword \"$_\"\n";
		}
		
		# ----------- handle body of START RESOURCE ... END -------------
		if (%CurResource) {
			if (/^END$/o) {
				$CurResource{SrcPath}=$CurSrcPath;
				my %Resource=%CurResource;
				undef %CurResource;
				push @ResourceStruct, \%Resource;
				undef %ResourceCheckLang;
				next LINE;
			}
			if (/^HEADER$/o) {
				if ($CurResource{Hdr}) {
					push @MmpWarn, "$CurFile($LineNum) : Resource HEADER already specified at line $CurResource{Hdr}\n";
					next LINE;
				}
				elsif ($CurResource{Hdronly}) {
					push @MmpWarn, "$CurFile($LineNum) : Resource HEADERONLY already specified at line $CurResource{Hdr}\n";
					next LINE;
				}
				$CurResource{Hdr}="$CurFile($LineNum)";
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Resource keyword HEADER takes no arguments\n";
				}
				next LINE;
			}
			if (/^HEADERONLY$/o) {
				if ($CurResource{Hdronly}) {
					push @MmpWarn, "$CurFile($LineNum) : Resource HEADERONLY already specified at line $CurResource{Hdr}\n";
					next LINE;
				}
				elsif ($CurResource{Hdr}) {
					push @MmpWarn, "$CurFile($LineNum) : Resource HEADER already specified at line $CurResource{Hdr}\n";
					next LINE;
				}
				$CurResource{Hdronly}="$CurFile($LineNum)";
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Resource keyword HEADERONLY takes no arguments\n";
				}
				next LINE;
			}

			if (/^LANG$/o) {
				if (@$Line) {
					my $Candidate;
					foreach $Candidate (@$Line) {
						if ($ResourceCheckLang{$Candidate}) {
							push @MmpWarn, "$CurFile($LineNum) : Duplicate Language \"$Candidate\" at line $ResourceCheckLang{$Candidate}\n";
							next; 
						}
						push @{$CurResource{Lang}}, $Candidate;
						$ResourceCheckLang{$Candidate}="$CurFile($LineNum)";
					}
					next LINE;
				}
				push @MmpWarn, "$CurFile($LineNum) : No Languages specified for keyword LANG\n";
				next LINE;
			}
			if (/^TARGET$/o) {
				if ($CurResource{BaseTrg}) {
					push @MmpWarn, "$CurFile($LineNum) : Resource TARGET already specified\n";
					next LINE;
				}
			    my $src=shift @$Line;
			    CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "RESOURCE TARGET", $src, $LineNum);
			    
			    $src = &Path_Norm($src);
				$CurResource{BaseTrg}=&Path_Split('Base',$src);
				unless ($CurResource{BaseTrg}) {
					push @MmpDie, "$CurFile($LineNum) : No name specified with Resource keyword TARGET\n";
					next LINE;
				}
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Too many arguments for Resource keyword TARGET\n";
				}
				next LINE;
			}
			if (/^TARGETPATH$/o) {
				if ($CurResource{TrgPath}) {
					push @MmpWarn, "$CurFile($LineNum) : Resource TARGETPATH already specified\n";
					next LINE;
				}
				unless ($CurResource{TrgPath}=shift @$Line) {
					push @MmpDie, "$CurFile($LineNum) : No path specified with Resource keyword TARGETPATH\n";
					next LINE;
				}
				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "RESOURCE TARGETPATH", $CurResource{TrgPath}, $LineNum);
				$CurResource{TrgPath} = &Path_Norm($CurResource{TrgPath});
				
				$CurResource{TrgPath}=~s-^\\(.*)$-$1-o;        # remove leading backslash, if any
				$CurResource{TrgPath}=~s-^(.*[^\\])$-$1\\-o;   # ensure path ends with a backslash
				$CurResource{TrgPath}="z\\$CurResource{TrgPath}";
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Too many arguments for Resource keyword TARGETPATH\n";
				}
				next LINE;
			}
			if (/^UID$/o) {
				if (@$Line) {
					if (scalar @$Line>2) {
						push @MmpWarn, "$CurFile($LineNum) : Can't specify more than 2 Uids\n";
						next LINE;
					}
					foreach (@$Line) {
						$_=&Genutl_AnyToHex($_);
						if (defined $_) {
							push @{$CurResource{Uids}}, $_;
							next;
						}
						push @MmpDie, "$CurFile($LineNum) : Uid doesn't fit expected number format\n";
						next LINE;
					}
					next LINE;
				}
				push @MmpWarn, "$CurFile($LineNum) : No Uids specified for Resource keyword UID\n";
				next LINE;
			}
			if (/^DEPENDS$/o) {
				# Note that DEPENDS lines are tolerated, but ignored - these are only relevant to build systems
				# other than ABLD
				next LINE;
			}
			
			push @MmpWarn, "$CurFile($LineNum) : Unrecognised Resource Keyword \"$_\"\n";
		}
		if (/^WCHARENTRYPOINT$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			if ($TrgType=~/^STDEXE$/io) {
				$WideCharMain=1;	
			}
			else{
				push @MmpWarn, "$CurFile($LineNum) : WCHARENTRYPOINT is supported only for Target Type STDEXE\n";
			}
			next LINE;
		}
		# ----------- handle body of START STRINGTABLE ... END -------------
		if(%CurStringTable)
		{	
			if (/^END$/o) {
				my %stringtable = %CurStringTable;
				push @StringTableStruct, \%stringtable;
				undef %CurStringTable;
				next LINE;
			}
			if (/^EXPORTPATH/o) {
				if (scalar @$Line != 1) {
					push @MmpDie, "$CurFile($LineNum) : Wrong number of arguments for EXPORTPATH\n";
				} 
				if(defined $CurStringTable{ExportPath})	{
					push @MmpDie, "$CurFile($LineNum) : Redefinition of EXPORTPATH\n";
				}
				else {
				    my $src=shift @$Line;
					$CurStringTable{ExportPath} = $src;

					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "STRINGTABLE EXPORTPATH", $CurStringTable{ExportPath}, $LineNum);
					
					$CurStringTable{ExportPath} = &Path_Norm($CurStringTable{ExportPath});					

					$CurStringTable{ExportPath} =~ s/\\$//o;   # ensure path doesn't end with a backslash (cl_generic.pm will add one)
					$CurStringTable{ExportPath} = &Path_MakeEAbs($EPOCPath,$CurFile,$CurStringTable{ExportPath});

				}
				next LINE;
			}
			if (/^HEADERONLY$/o) {
				if ($CurStringTable{Hdronly}) {
					push @MmpWarn, "$CurFile($LineNum) : Stringtable HEADERONLY already specified\n";
					next LINE;
				}
				$CurStringTable{Hdronly}="$CurFile($LineNum)";
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Stringtable keyword HEADERONLY takes no arguments\n";
				}
				next LINE;
			}
		}
		# ----------- handle top-level MMP file -------------------	
		if (/^START$/) {
			unless ($_=uc shift @$Line) {
				push @MmpWarn, "$CurFile($LineNum) : No Argument specified for START block\n";
				next LINE;
			}
			foreach $MmpMacro (@{$$Plat_ref{MmpMacros}}) {
				if ($_ eq $MmpMacro) {
					$PlatTxtSwitch="$CurFile($LineNum)";
					next LINE;
				}
			}
			if ($_ eq 'BITMAP') {
				unless ($CurBitMap{Trg}=shift @$Line) {
					push @MmpWarn, "$CurFile($LineNum) : No Bitmap Target specified\n";
					$CurBitMap{Trg}='NoTargetSpecified';
				}
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Too many arguments for START BITMAP clause\n";
				}

				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "START BITMAP", $CurBitMap{Trg}, $LineNum);
				$CurBitMap{Trg} = &Path_Norm($CurBitMap{Trg});
				next LINE;
			}
			if ($_ eq 'RESOURCE') {
				if (scalar @$Line != 1) {
					push @MmpWarn, "$CurFile($LineNum) : Wrong number of arguments for START RESOURCE clause\n";
				} 
				else {
				    my $src=shift @$Line;
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "START RESOURCE", $src, $LineNum, $CheckSource_PhysicalCheck, $CurSrcPath);
					$src = &Path_Norm($src);
				    
					$CurResource{Source}="$CurSrcPath$src";
				}

				
				next LINE;
			}
			if ($_ eq 'STRINGTABLE') {
				if (scalar @$Line != 1) {
					push @MmpWarn, "$CurFile($LineNum) : Wrong number of arguments for START STRINGTABLE clause\n";
				} 
				else {
				    my $Candidate = shift @$Line;
				    CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "START STRINGTABLE", $Candidate, $LineNum, $CheckSource_PhysicalCheck, $CurSrcPath);
					$Candidate = &Path_Norm($Candidate);
				    
					$Candidate =~ s/^\\//;	# remove leading \, if any	
					$CurStringTable{BaseTrg}=&Path_Split('Base',$Candidate);
					my $path=&Path_Split('Path',$Candidate);
					if($path){
					  $CurStringTable{STPath}=&Path_MakeAbs($CurSrcPath,&Path_Split('Path',$Candidate));
					}
					else {
					  $CurStringTable{STPath}=$CurSrcPath;
					}
					$CurStringTable{STFile}=&Path_Split('File',$Candidate);

				}
				next LINE;
			}
			$OtherPlatSwitch="$CurFile($LineNum)";
			next LINE;
		}

		if (/^AIF$/o) {
			my ($trg, $dir, $file, $clDepth, @bitmaps)=@$Line;
			unless ($file) { # bitmaps aren't essential
				push @MmpDie, "$CurFile($LineNum) : Not enough arguments for keyword AIF\n";
				next LINE;
			}
			my %Data;
			$Data{Trg} = $trg;	# Defect: may include directory
			$dir=~s-^(.*[^\\])$-$1\\-o;   # ensure path ends with a backslash
			$Data{Source}=&Path_MakeAbs($CurFile, "$dir$file");
			unless (-e "$Data{Source}") {
				push @MmpWarn, "$CurFile($LineNum) : AIF source \"$Data{Source}\" not found\n";
			}
			if ($CheckAif{$Data{Trg}}) {
				push @MmpDie, "$CurFile($LineNum) : Duplicate Aif \"$Data{Trg}\" at line $CheckAif{$Data{Trg}}\n";
				next LINE;
			}
			$CheckAif{$Data{Trg}}="$CurFile($LineNum)";
			push @AifStruct, \%Data;
			next LINE unless (@bitmaps);
			# handle bitmaps
			my @ClDepths = split(/,/, $clDepth);
			foreach (@ClDepths) {
				$_ = lc $_; # bmconv can't handle upper-case 'C's
				unless (/^c?\d\d?$/o) {
					push @MmpDie, "$CurFile($LineNum) : AIF color depth \"$_\" - unexpected format\n";
				}
			}
			@ClDepths = (@ClDepths) x @bitmaps;	# make a sufficiently long list of color depths
			foreach $file (@bitmaps) {
				if ($file !~ /^\\/) {
					$file = &Path_MakeAbs($CurFile, "$dir$file");
				}
				push @{$Data{BitMaps}}, { # sources must be kept in order
					Src=>$file,
					ClDepth=>shift @ClDepths	# take next color depth from the list
				};
				unless (-e $file) {
					push @MmpWarn, "$CurFile($LineNum) : AIF source \"$file\" not found\n";
				}
			}
			next LINE;
		}
		if (/^ASSPABI$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$ASSPABISwitch=1;
			next LINE;
		}
		if (/^ASSPEXPORTS$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$ASSPExports=1;
			next LINE;
		}
		if (/^ASSPLIBRARY$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum);
					$Candidate = &Path_Norm($Candidate);
					
					unless ($emulator or &Genutl_ParseVersionedName(\$Candidate)) {
						push @MmpWarn, "$CurFile($LineNum) : Bad version in ASSPLIBRARY\n";
					}
					if ($CheckASSPLib{$Candidate}) {
						push @MmpWarn, "$CurFile($LineNum) : Duplicate Library \"$Candidate\" at line $CheckASSPLib{$Candidate}\n";
						next; 
					}
					
					push @ASSPLibList, $Candidate;
					$CheckASSPLib{$Candidate}="$CurFile($LineNum)";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Libraries specified for keyword ASSPLIBRARY\n";
			next LINE;
		}
		if (/^CAPABILITY$/o) {
			if (defined($Capability)) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine CAPABILITY\n";
				next LINE;
			}
			if (scalar @$Line == 0) {
				push @MmpWarn, "$CurFile($LineNum) : Wrong number of arguments for CAPABILITY\n";
				next LINE;
			}
			foreach my $capname (@$Line) {
				my $invert = 0;
				if ($capname =~ /^-(\w*)/) {
					$invert = 0xffffffff;
					$capname = $1;
				}
				my $capFlag = 0;
				if (defined($CapabilityNames{uc $capname})) {
					$capFlag = $CapabilityNames{uc $capname};
					if (not $capFlag) {
						push @MmpDiagnostic, "$CurFile($LineNum) : Use of old capability name, \"$capname\" ignored\n";
						next;
					}
					if ("ALL" eq uc $capname and $invert) {
						push @MmpWarn, "Capability '-ALL' not allowed";
						next;
					}
				}
				elsif ("NONE" eq uc $capname) {
					if($invert) {
						push @MmpWarn, "Capability '-NONE' not allowed";
						next;
					}
				}
				else {
					push @MmpWarn, "$CurFile($LineNum) : Unknown capability \"$capname\" ignored\n";
					next;
				}
				# append name to capability string
				if (defined($Capability) and not $invert) { $Capability .= "+"; }
				if($invert) { $Capability .= "-"; }
				$Capability .= $capname;

				# add capability mask to flags
				$CapabilityFlags[0] = $CapabilityFlags[0] ^ $invert;
				$CapabilityFlags[0] = $CapabilityFlags[0] | $capFlag;
				$CapabilityFlags[0] = $CapabilityFlags[0] ^ $invert;
				$CapabilityFlags[1] = 0;
				next;
			}
			next LINE;
		}
		if (/^DEBUGLIBRARY$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum);
					$Candidate = &Path_Norm($Candidate);
					
					unless ($emulator or &Genutl_ParseVersionedName(\$Candidate)) {
						push @MmpWarn, "$CurFile($LineNum) : Bad version in DEBUGLIBRARY\n";
					}
					if ($CheckLib{$Candidate}) {
						push @MmpWarn, "$CurFile($LineNum) : Duplicate Library \"$Candidate\" at line $CheckLib{$Candidate}\n";
						next; 
					}
					
					push @DebugLibList, $Candidate;
					$CheckLib{$Candidate}="$CurFile($LineNum)";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Libraries specified for keyword DEBUGLIBRARY\n";
			next LINE;
		}
		if (/^DEFFILE$/o)  {
			if ($CheckDef) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine DEFFILE\n";
				next LINE;
			}
			$CheckDef=1;
			unless ($_=shift @$Line) {
				push @MmpWarn, "$CurFile($LineNum) : No file specified for keyword DEFFILE\n";
				next LINE;
			}

			$Def{CheckSource_MMPEntry} = $_;		
			$Def{CheckSource_LineNumber} = $LineNum;
			$Def{CheckSource_MMPFile} = $CurFile;
			
			$_ = &Path_Norm($_);
			
			$Def{Base}=&Path_Split('Base',$_);
			$Def{Ext}=&Path_Split('Ext',$_);
			$Def{Path}=&Path_Split('Path',$_);
			
			if ($Def{Path}) {
				$Def{Path}=&Path_MakeEAbs($EPOCPath,$CurFile,$Def{Path});
			}

			if ($Def{Base} =~ /\{|\}/) {
				push @MmpDie, "$CurFile($LineNum) : Bad DEFFILE name\n";
			}
			next LINE;
		}
		if (/^DOCUMENT$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum, $CheckSource_PhysicalCheck, $CurSrcPath);
					$Candidate = &Path_Norm($Candidate);
					
					if ($CheckDoc{"$CurSrcPath$Candidate"}) {
						push @MmpWarn, "$CurFile($LineNum) : Duplicate Document \"$CurSrcPath$Candidate\" at line ", $CheckDoc{"$CurSrcPath$Candidate"}, "\n";
						next; 
					}
					unless (-e "$CurSrcPath$Candidate") {
						push @MmpWarn, "$CurFile($LineNum) : DOCUMENT \"$CurSrcPath$Candidate\" not found\n";
					}
					
					push @{$DocHash{$CurSrcPath}}, $Candidate;
					$CheckDoc{"$CurSrcPath$Candidate"}="$CurFile($LineNum)";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Files specified for keyword DOCUMENT\n";
			next LINE;
		}
		if (/^EPOCALLOWDLLDATA$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$AllowDllData=1;
			next LINE;
		}
		if (/^ALWAYS_BUILD_AS_ARM$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$BuildAsARM=1;
			next LINE;
		}
		if (/^EPOCCALLDLLENTRYPOINTS$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$CallDllEntryPoints=1;
			next LINE;
		}
		if (/^EPOCDATALINKADDRESS$/o) {
			if (@$Line) { 
				my $temp=&main::Genutl_AnyToHex(shift @$Line);
				if (defined $temp) {
					$DataLinkAddress=$temp;
					next LINE;
				}
				push @MmpDie, "$CurFile($LineNum) : Data link address doesn't fit expected number format\n";
			}
			push @MmpWarn, "$CurFile($LineNum) : No data link address specified for keyword $_\n";
			next LINE;
		}
		if (/^EPOCFIXEDPROCESS$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$FixedProcess=1;
			next LINE;
		}
		if (/^EPOCHEAPSIZE$/o) {
			if (@$Line) {
				my $tempMin=&main::Genutl_AnyToHex(shift @$Line);
				if (defined $tempMin) {
					if (@$Line) {
						my $tempMax=&main::Genutl_AnyToHex(shift @$Line);
						if (defined $tempMax) {
							$HeapSize{Min}=$tempMin;
							$HeapSize{Max}=$tempMax;
							next LINE;
						}
						push @MmpDie, "$CurFile($LineNum) : maximum heap size doesn't fit expected number format\n";
						next LINE;
					}
					push @MmpDie, "$CurFile($LineNum) : No maximum heap size specified for keyword $_\n";
					next LINE;
				}
				push @MmpDie, "$CurFile($LineNum) : minimum heap size doesn't fit expected number format\n";
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No minimum heap size specified for keyword $_\n";
			next LINE;
		}
		if (/^EPOCPROCESSPRIORITY$/o) {
			if ($ProcessPriority) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine EPOCPROCESSPRIORITY\n";
				next LINE;
			}
			if ($ProcessPriority=shift @$Line) {
				if (defined($ProcessPriorityNames{uc $ProcessPriority})) {
					$ProcessPriority = $ProcessPriorityNames{uc $ProcessPriority}; # canonical form
					next LINE;
				}
				push @MmpDie, "$CurFile($LineNum) : ProcessPriority \"$ProcessPriority\" not supported\n";
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Priority specified for keyword EPOCPROCESSPRIORITY\n";
			next LINE;
		}
		if (/^EPOCSTACKSIZE$/o) {
			if (@$Line) {
				my $temp=&main::Genutl_AnyToHex(shift @$Line);
				if (defined $temp) {
					$StackSize=$temp;
					next LINE;
				}
				push @MmpDie, "$CurFile($LineNum) : Stack size doesn't fit expected number format\n";
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No stack size specified for keyword STACKSIZE\n";
			next LINE;
		}
		if (/^COMPRESSTARGET$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$CompressTarget=COMPRESS;
            $CompressTargetMethod=NOCOMPRESSIONMETHOD; # means 'use default'
			next LINE;
		}
		if (/^NOCOMPRESSTARGET$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$CompressTarget=NOCOMPRESS;
            $CompressTargetMethod=NOCOMPRESSIONMETHOD;
			next LINE;
		}
        if (/^INFLATECOMPRESSTARGET$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
            $CompressTarget=COMPRESS;
			$CompressTargetMethod=INFLATECOMPRESSIONMETHOD;
			next LINE;
		}
		if (/^BYTEPAIRCOMPRESSTARGET$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
            $CompressTarget=COMPRESS;
			$CompressTargetMethod=BYTEPAIRCOMPRESSIONMETHOD;
			next LINE;
		}
		if (/^EXPORTUNFROZEN$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$ExportUnfrozen=1;
			next LINE;
		}
		if (/^FIRSTLIB$/o) {
			if ($FirstLib) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine FIRSTLIB\n";
				next LINE;
			}
			if ($FirstLib=shift @$Line) {
				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $FirstLib, $LineNum);
				$FirstLib = &Path_Norm($FirstLib);
				next LINE;
			}
			
			push @MmpWarn, "$CurFile($LineNum) : Nothing specified for keyword FIRSTLIB\n";
			next LINE;
		}
		if (/^LANG$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					if ($CheckLang{$Candidate}) {
						push @MmpWarn, "$CurFile($LineNum) : Duplicate Language \"$Candidate\" at line $CheckLang{$Candidate}\n";
						next; 
					}
					push @LangList, $Candidate;
					$CheckLang{$Candidate}="$CurFile($LineNum)";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Languages specified for keyword LANG\n";
			next LINE;
		}
		if (/^LIBRARY$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum);
					$Candidate = &Path_Norm($Candidate);
					
					unless ($emulator or &Genutl_ParseVersionedName(\$Candidate)) {
						push @MmpWarn, "$CurFile($LineNum) : Bad version in LIBRARY\n";
					}
					if ($CheckLib{$Candidate}) {
						push @MmpWarn, "$CurFile($LineNum) : Duplicate Library \"$Candidate\" at line $CheckLib{$Candidate}\n";
						next; 
					}
					
					push @LibList, $Candidate;
					push @DebugLibList, $Candidate;	    # appears in both
					$CheckLib{$Candidate}="$CurFile($LineNum)";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Libraries specified for keyword LIBRARY\n";
			next LINE;
		}
		if (/^LINKAS$/o) {
			if ($LinkAs) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine LINKAS\n";
				next LINE;
			}
			if ($LinkAs=shift @$Line) {
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No name specified for keyword LINKAS\n";
			next LINE;
		}
		if (/^EXPORTLIBRARY$/o) {
			if ($NoExportLibrary) {
				push @MmpDie, "$CurFile($LineNum) : Can't specify both EXPORTLIBRARY and NOEXPORTLIBRARY\n";
				next LINE;
			}
			if ($ExportLibrary) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine EXPORTLIBRARY\n";
				next LINE;
			}
			if ($ExportLibrary=shift @$Line) {
				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $ExportLibrary, $LineNum);
				$ExportLibrary = &Path_Norm($ExportLibrary);
				$ExportLibrary=&Path_Split('Base',$ExportLibrary);
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No name specified for keyword EXPORTLIBRARY\n";
			next LINE;
		}
		if (/^NOEXPORTLIBRARY$/o) {
			if ($ExportLibrary) {
				push @MmpDie, "$CurFile($LineNum) : Can't specify both EXPORTLIBRARY and NOEXPORTLIBRARY\n";
				next LINE;
			}
			$NoExportLibrary = 1;
			next LINE;
		}
		if (/^NEWLIB$/o) {
			if ($NewLib) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine NEWLIB\n";
				next LINE;
			}
			if ($NewLib = shift @$Line) {
				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $NewLib, $LineNum);
				$NewLib = &Path_Norm($NewLib);
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No library specified for keyword NEWLIB\n";
			next LINE;
		}
		if (/^MACRO$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					if ($CheckMacro{$Candidate}) {
						push @MmpWarn, "$CurFile($LineNum) : Duplicate Macro \"$Candidate\" at line $CheckMacro{$Candidate}\n";
						next; 
					}
					push @Macros, $Candidate;
					$CheckMacro{$Candidate}="$CurFile($LineNum)";
				}
				next LINE;
			}
			next LINE; 
		}
		if (/^NOSTRICTDEF$/o) {
			if ($NoStrictDef) {
				push @MmpWarn, "$CurFile($LineNum) : NOSTRICTDEF already set\n";
				next LINE;
			}
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword NOSTRICTDEF takes no arguments\n";
			}
			$NoStrictDef=1;
			next LINE;
		}
		if (/^RAMTARGET$/o) {
			if ($CheckRamTargets) {
				push @MmpWarn, "$CurFile($LineNum) : RAM targets already specified at line $CheckRamTargets\n";
				next LINE;
			}
			$CheckRamTargets="$CurFile($LineNum)";
			unless (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : No targets specified for keyword RAMTARGET\n";
				next LINE;
			}
			if ($$Line[0] eq '+') {
				my %Data=();
				push @RamTargets, \%Data;	# include default
				shift @$Line;
			}
			my $Elem;
			foreach $Elem (@$Line) {
				my %Data=();
				$Data{File}=&Path_Split('File',$Elem);
				$Data{Path}=&Path_Split('Path',$Elem);
				push @RamTargets, \%Data;
			}
			next LINE;
		}
		if (/^RESOURCE$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {			
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum, $CheckSource_PhysicalCheck, $CurSrcPath);
					$Candidate = &Path_Norm($Candidate);
					
					if ($CheckResrc{$Candidate}) {
						push @MmpDie, "$CurFile($LineNum) : Duplicate Resource $Candidate at line $CheckResrc{$Candidate}\n";
						next;
					}
					
					$CheckResrc{$Candidate}="$CurFile($LineNum)";
					my $source="$CurSrcPath$Candidate";
					unless (-e $source) {
						push @MmpWarn, "$CurFile($LineNum) : RESOURCE source \"$source\" not found\n";
					}
					
					$CurResource{BaseTrg}=&Path_Split('Base',$Candidate);
					$CurResource{Source}=$source;
					$CurResource{Hdr}="$CurFile($LineNum)";
					# $CurResource{TrgPath} will be filled in at the end;
					my %Resource=%CurResource;
					undef %CurResource;
					push @ResourceStruct, \%Resource;
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Resources specified for keyword RESOURCE\n";
			next LINE; 
		}
		if (/^ROMTARGET$/o) {
			if ($CheckRomTargets) {
				push @MmpWarn, "$CurFile($LineNum) : ROM targets already specified at line $CheckRomTargets\n";
				next LINE;
			}
			$CheckRomTargets="$CurFile($LineNum)";
			unless (@$Line) {
				@RomTargets=();
				next LINE;
			}
			if ($$Line[0] eq '+') {
				shift @$Line;
			}
			else {
				@RomTargets=(); # remove default
			}
			my $Elem;
			foreach $Elem (@$Line) {
				my %Data=();
				$Data{File}=&Path_Split('File',$Elem);
				$Data{Path}=&Path_Split('Path',$Elem);
				push @RomTargets, \%Data;
			}
			next LINE;
		}
		if (/^SMPSAFE$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$SmpSafe = 1;
			next LINE;
		}
		if (/^SOURCE$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum, $CheckSource_PhysicalCheck, $CurSrcPath);
					$Candidate = &Path_Norm($Candidate);
					
					$Candidate =~ s/^\\//;	# remove leading \, if any	
					$CurSource{BaseTrg}=&Path_Split('Base',$Candidate);
					my $path=&Path_Split('Path',$Candidate);
					if($path){
					  $CurSource{SrcPath}=&Path_MakeAbs($CurSrcPath,&Path_Split('Path',$Candidate));
					}
					else {
					  $CurSource{SrcPath}=$CurSrcPath;
					}
					$CurSource{CurFile}=&Path_Split('File',$Candidate);

					my %Source=%CurSource;
					undef %CurSource;
					push @SourceStruct, \%Source;
					
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Sources specified for keyword SOURCE\n";
			next LINE; 
		}
		if (/^SOURCEPATH$/o) {
			if ($CurSrcPath=shift @$Line) {
				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $CurSrcPath, $LineNum, $CheckSource_PhysicalCheck);				
				$CurSrcPath = &Path_Norm($CurSrcPath);
				
				$CurSrcPath=~s-^(.*[^\\])$-$1\\-o;	# in case no terminating backslash
				$CurSrcPath=&Path_MakeEAbs($EPOCPath,$CurFile,$CurSrcPath);
				if (-d &Path_Chop($CurSrcPath)) {
					next LINE;
				}
				push @MmpWarn, "$CurFile($LineNum) : SOURCEPATH \"$CurSrcPath\" not found\n";
				next LINE;
			}
			push @MmpDie, "$CurFile($LineNum) : No Source Path specified for keyword SOURCEPATH\n";
			next LINE;
		}
		if (/^STATICLIBRARY$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum);
					$Candidate = &Path_Norm($Candidate);
					
					if ($CheckStatLib{$Candidate}) {
						push @MmpWarn, "$CurFile($LineNum) : Duplicate Library \"$Candidate\" at line $CheckStatLib{$Candidate}\n";
						next;
					}
					
					push @StatLibList, $Candidate;
					$CheckStatLib{$Candidate}="$CurFile($LineNum)";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Libraries specified for keyword STATICLIBRARY\n";
			next LINE;
		}
		if (/^STDCPP$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$StdCpp=1;
			next LINE;
		}
  		if (/^NOSTDCPP$/o) {
  			if (@$Line) {
  				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
  			}
  			$NoStdCpp=1;
  			next LINE;
  		}
		if (/^STRICTDEPEND$/o) {
			if ($MmpFlag{StrictDepend}) {
				push @MmpWarn, "$CurFile($LineNum) : STRICTDEPEND already set\n";
				next LINE;
			}
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword STRICTDEPEND takes no arguments\n";
			}
			$MmpFlag{StrictDepend}=1;
			next LINE;
		}
		if (/^SYSTEMINCLUDE$/o){
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum, $CheckSource_PhysicalCheck);
					$Candidate = &Path_Norm($Candidate);
					
					$Candidate=~s-^(.*[^\\])$-$1\\-o;   # ensure path ends with a backslash
					$Candidate=&Path_MakeEAbs($EPOCPath,$CurFile,$Candidate);
					if ($CheckSysInc{$Candidate}) {
						next; 
					}
					push @SysIncPaths,$Candidate;
					$CheckSysInc{$Candidate}="$CurFile($LineNum)";
					if (-d &Path_Chop($Candidate)) {
						next;
					}
					push @MmpWarn, "$CurFile($LineNum) : SYSTEMINCLUDE path \"$Candidate\" not found\n";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Paths specified for keyword SYSTEMINCLUDE\n";
			next LINE;
		}
		if (/^SYSTEMRESOURCE$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum, $CheckSource_PhysicalCheck, $CurSrcPath);
					$Candidate = &Path_Norm($Candidate);
					
					if ($CheckSysResrc{$Candidate}) {
						push @MmpDie, "$CurFile($LineNum) : Duplicate Resource \"$Candidate\" at line $CheckSysResrc{$Candidate}\n";
						next; 
					}
					$CheckSysResrc{$Candidate}="$CurFile($LineNum)";
					my $source="$CurSrcPath$Candidate";
					unless (-e $source) {
						push @MmpWarn, "$CurFile($LineNum) : SYSTEMRESOURCE source \"$source\" not found\n";
					}
					$CurResource{BaseTrg}=&Path_Split('Base',$Candidate);
					$CurResource{Source}=$source;
					$CurResource{Hdr}="$CurFile($LineNum)";
					$CurResource{TrgPath}="z\\system\\data\\";	# needs to match e32env.pm
					my %Resource=%CurResource;
					undef %CurResource;
					push @ResourceStruct, \%Resource;
				}
				if (&main::EPOCSecurePlatform()) {    
					push @MmpMigrationNote, "Obsolete SYSTEMRESOURCE keyword specified in \"$MMPFILE\"";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Resources specified for keyword SYSTEMRESOURCE\n";
			next LINE; 
		}
		if (/^TARGET$/o) {
			if ($Trg) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine TARGET\n";
				next LINE;
			}
			if ($Trg=shift @$Line) {
				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Trg, $LineNum);
				$Trg = &Path_Norm($Trg);
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Target specified for keyword TARGET\n";
			next LINE;
		}
		if (/^TARGETPATH$/o) {
			if ($TrgPath) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine TARGETPATH\n";
				next LINE;
			}
			unless ($TrgPath=shift @$Line) {
				push @MmpWarn, "$CurFile($LineNum) : No Path specified for keyword TARGETPATH\n";
				next LINE;
			}

			CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $TrgPath, $LineNum);
			$TrgPath = &Path_Norm($TrgPath);
			
			$TrgPath=~s-^\\(.*)$-$1-o;
			$TrgPath=~s-^(.*[^\\])$-$1\\-o;
			$TrgPath="z\\$TrgPath";
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Too many arguments for keyword TARGETPATH\n";
			}
			next LINE;
		}
		if (/^TARGETTYPE$/o) {
			if ($TrgType) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine TARGETTYPE\n";
				next LINE;
			}
			unless ($TrgType=shift @$Line) {
				push @MmpWarn, "$CurFile($LineNum) : No Type specified for keyword TARGETTYPE\n";
				next LINE;
			}
			eval { &Trg_GetL($TrgType, \%TrgType); };
			if ($@) {
				push @MmpDie, "$CurFile($LineNum) : $@";
			}
			next LINE;
		}
		if (/^UID$/o) {
			if (@$Line) {
				foreach (@$Line) {
					if ($#UidList>=1) {
						push @MmpWarn, "$CurFile($LineNum) : Can't specify more than 2 Uids\n";
						next LINE;
					}
					$_=&Genutl_AnyToHex($_);
					if (defined $_) {
						push @UidList, $_;
						next;
					}
					push @MmpDie, "$CurFile($LineNum) : Uid doesn't fit expected number format\n";
					next LINE;
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Uids specified for keyword UID\n";
			next LINE;
		}
		if (/^SECUREID$/o) {
			if ($SecureId) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine SECUREID\n";
				next LINE;
			}
			if (@$Line) {
				$SecureId = &Genutl_AnyToHex(shift @$Line);
				if (!defined $SecureId) {
					push @MmpDie, "$CurFile($LineNum) : SECUREID doesn't fit expected number format\n";
					next LINE;
				}
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Too many arguments for keyword SECUREID\n";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : Missing argument for keyword SECUREID\n";
			next LINE;
		}
		if (/^VENDORID$/o) {
			if (defined($VendorId)) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine VENDORID\n";
				next LINE;
			}
			if (@$Line) {
				$VendorId = &Genutl_AnyToHex(shift @$Line);
				if (!defined $VendorId) {
					push @MmpDie, "$CurFile($LineNum) : VENDORID doesn't fit expected number format\n";
					next LINE;
				}
				if (@$Line) {
					push @MmpWarn, "$CurFile($LineNum) : Too many arguments for keyword VENDORID\n";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : Missing argument for keyword VENDORID\n";
			next LINE;
		}
		if (/^USERINCLUDE$/o) {
			if (@$Line) {
				my $Candidate;
				foreach $Candidate (@$Line) {
					CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, $mainElement, $Candidate, $LineNum, $CheckSource_PhysicalCheck);
					$Candidate = &Path_Norm($Candidate);
					
					$Candidate=~s-^(.*[^\\])$-$1\\-o;   # ensure path ends with a backslash
					$Candidate=&Path_MakeEAbs($EPOCPath,$CurFile,$Candidate);
					if ($CheckUserInc{$Candidate}) {
						next; 
					}
					push @UserIncPaths,$Candidate;
					$CheckUserInc{$Candidate}="$CurFile($LineNum)";
					if (-d &Path_Chop($Candidate)) {
						next;
					}
					push @MmpWarn, "$CurFile($LineNum) : USERINCLUDE path \"$Candidate\" not found\n";
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : No Paths specified for keyword USERINCLUDE\n";
			next LINE;
		}
		if (/^SRCDBG$/o) {
			if (@$Line) {
				push @MmpWarn, "$CurFile($LineNum) : Keyword $_ takes no arguments\n";
			}
			$SrcDbg=1;
			next LINE;
		}
		if (/^VERSION$/o) {
			if (%Version) {
				push @MmpDie, "$CurFile($LineNum) : Attempt to redefine VERSION\n";
				next LINE;
			}
			unless (@$Line) {
				push @MmpDie, "$CurFile($LineNum) : Missing argument to VERSION\n";
				next LINE;
			}
			%Version = &Genutl_StringToVersion(shift @$Line);
			if (!%Version) {
				push @MmpDie, "$CurFile($LineNum) : Bad VERSION number\n";
				next LINE;
			}
			if (@$Line) {
				if ((lc $$Line[0]) eq 'explicit') {
					$Version{explicit} = 1;
					shift @$Line;
				}
				if (scalar(@$Line)) {
					push @MmpDie, "$CurFile($LineNum) : Garbage after VERSION number\n";
				}
			}
			next LINE;
		}
		
		if (/^OPTION$/oi ) {
			if (@$Line >= 2) {
				my $compilerkey= uc shift @$Line;
				if (!defined($Compiler{$compilerkey})) {
					# first use of "OPTION xxx"
					$Compiler{$compilerkey}=shift @$Line;
				}
				# concatenate extra stuff
				while (@$Line) {
					$Compiler{$compilerkey}.=" ".shift @$Line;
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : Keyword OPTION requires at least two arguments\n";
			next LINE;
			}
		if (/^LINKEROPTION$/oi ) {
			if (@$Line >= 2) {
			        # first parameter is the compiler
				my $compilerkey= uc shift @$Line;
				if (!defined($Compiler{$compilerkey})) {
					# first use of "LINKEROPTION xxx"
					$LinkerOptions{$compilerkey}=shift @$Line;
				}
				# concatenate extra stuff
				while (@$Line) {
					$LinkerOptions{$compilerkey}.=" ".shift @$Line;
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : Keyword LINKEROPTION requires at least two arguments\n";
			next LINE;
			}
		if (/^OPTION_Replace$/oi ) {
			if (@$Line >= 2) {
				my $compilerkey= uc shift @$Line;
				my $repOptions= shift @$Line;
				while (@$Line) 
				{
					$repOptions.= " ".shift @$Line;
				}
				$repOptions =~ s/=\s*/=/g;
				push @{$Replace{$compilerkey}},$repOptions;
				
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : Keyword OPTION_Replace requires at least two arguments\n";
			next LINE;
			}
		if (/^ARMFPU$/oi) {
			if ($ARMFPU) {
				push @MmpWarn, "$CurFile($LineNum) : Attempt to redefine ARMFPU\n";
				next LINE;
			}
			if ($ARMFPU=shift @$Line) {
				if (($ARMFPU !~ /^SOFTVFP$/oi) && ($ARMFPU !~ /^VFPV2$/oi)) {
					push @MmpWarn, "$CurFile($LineNum) : ARMFPU can only specify SOFTVFP or VFPV2 as an argument\n";
					undef $ARMFPU;
				}
				next LINE;
			}
			push @MmpWarn, "$CurFile($LineNum) : ARMFPU must specify either SOFTVFP or VFPV2 as an argument\n";
			next LINE;
		}
		if( /^PAGED$/o) {
			#revert "PAGED" keyword from code and data paged to code paged only
			#if ($CodePagingTargetMode == PAGED or $DataPagingTargetMode == PAGED) {
			if ($CodePagingTargetMode == PAGED) {
				push @MmpWarn, "$CurFile($LineNum) : duplicate paging setting\n";
			}
			#revert "PAGED" keyword from code and data paged to code paged only
			#if ($CodePagingTargetMode == UNPAGED or $DataPagingTargetMode == UNPAGED) {
			if ($CodePagingTargetMode == UNPAGED) {
				push @MmpWarn, "$CurFile($LineNum) : conflict paging setting\n";
			}
			$CodePagingTargetMode = PAGED;
			#revert "PAGED" keyword from code and data paged to code paged only
			#$DataPagingTargetMode = PAGED;
			next LINE;
			}
		if( /^UNPAGED$/o) {
			if ($CodePagingTargetMode == UNPAGED or $DataPagingTargetMode == UNPAGED) {
				push @MmpWarn, "$CurFile($LineNum) : duplicate paging setting\n";
			}
			if ($CodePagingTargetMode == PAGED or $DataPagingTargetMode == PAGED) {
				push @MmpWarn, "$CurFile($LineNum) : conflict paging setting\n";
			}
			$CodePagingTargetMode = UNPAGED;
			$DataPagingTargetMode = UNPAGED;
			next LINE;
			}
		if( /^PAGEDCODE$/o) {
			if ($CodePagingTargetMode == PAGED) {
				push @MmpWarn, "$CurFile($LineNum) : duplicate paging setting\n";
			}
			if ($CodePagingTargetMode == UNPAGED) {
				push @MmpWarn, "$CurFile($LineNum) : conflict paging setting\n";
			}
			$CodePagingTargetMode = PAGED;
			next LINE;
			}
		if( /^UNPAGEDCODE$/o) {
			if ($CodePagingTargetMode == UNPAGED) {
				push @MmpWarn, "$CurFile($LineNum) : duplicate paging setting\n";
			}
			if ($CodePagingTargetMode == PAGED) {
				push @MmpWarn, "$CurFile($LineNum) : conflict paging setting\n";
			}
			$CodePagingTargetMode = UNPAGED;
			next LINE;
			}
		if( /^PAGEDDATA$/o) {
			if ($DataPagingTargetMode == PAGED) {
				push @MmpWarn, "$CurFile($LineNum) : duplicate paging setting\n";
			}
			if ($DataPagingTargetMode == UNPAGED) {
				push @MmpWarn, "$CurFile($LineNum) : conflict paging setting\n";
			}
			$DataPagingTargetMode = PAGED;
			next LINE;
			}
		if( /^UNPAGEDDATA$/o) {
			if ($DataPagingTargetMode == UNPAGED) {
				push @MmpWarn, "$CurFile($LineNum) : duplicate paging setting\n";
			}
			if ($DataPagingTargetMode == PAGED) {
				push @MmpWarn, "$CurFile($LineNum) : conflict paging setting\n";
			}
			$DataPagingTargetMode = UNPAGED;
			next LINE;
			}
		if( /^DEBUGGABLE_UDEBONLY$/o) {
			$IsDebuggable = DEBUGGABLE_UDEBONLY;
			next LINE;
			}
		if( /^DEBUGGABLE$/o) {
			if ($IsDebuggable != DEBUGGABLE_UDEBONLY)
			{
				$IsDebuggable = DEBUGGABLE;
			}
			next LINE;
			}
		if( /^FEATUREVARIANT$/o) {
			push @MmpDie, "$CurFile($LineNum) : FEATUREVARIANT specified multiple times" if ($FeatureVariant);
			$FeatureVariant = 1;
			next LINE;
			}

		push @MmpWarn, "$CurFile($LineNum) : Unrecognised Keyword \"$_\"\n";
	}

	undef $Line;
	undef $MmpMacro;


	# test the mmp contents
	#----------------------
	if ($PlatTxtSwitch || $OtherPlatSwitch)	{
		push @MmpDie, $PlatTxtSwitch ? $PlatTxtSwitch : $OtherPlatSwitch, ": Unterminated START ... END block\n";
	}
	undef $PlatTxtSwitch;
	undef $OtherPlatSwitch;

	# Check the consistency of the mmp file contents
	unless ($Trg) {
		unless($TrgType=~/^NONE$/io){
			push @MmpDie, "ERROR: No Target specified\n";
		}
	}
	unless ($TrgType) {
		push @MmpDie, "ERROR: No TargetType specified\n";
	}
  	if( $StdCpp && $NoStdCpp ) {
  		push @MmpWarn, "WARNING: Keyword statement STDCPP not permitted with keyword NOSTDCPP.\n";
  		push @MmpWarn, "WARNING: Keyword statements STDCPP and NOSTDCPP ignored.\n";
  		$StdCpp = 0;
  		$NoStdCpp = 0;
  	}

	if (($TrgType =~ /^LIB$/io) && (scalar(@DebugLibList) || scalar(@LibList) || scalar(@StatLibList))) {
		push @MmpWarn, "WARNING: Library statements (DEBUGLIBRARY, LIBRARY or STATICLIBRARY) not permitted with TARGETTYPE LIB.";
		push @MmpWarn, "WARNING: Library statements (DEBUGLIBRARY, LIBRARY or STATICLIBRARY) ignored.\n";
		undef @DebugLibList;
		undef @LibList;
		undef @StatLibList;
	}

	if ($TrgType =~ /^LIB$/io && $NewLib) {
		push @MmpWarn, "WARNING: Library statement NEWLIB not permitted with TARGETTYPE LIB.";
		push @MmpWarn, "WARNING: Library statement NEWLIB ignored.\n";
		undef @DebugLibList;
		undef @LibList;
		undef @StatLibList;
	}

	if ($TrgType =~ /^STD(EXE|DLL)$/io && $NewLib) {
		push @MmpWarn, "WARNING: Library statement NEWLIB not permitted with TARGETTYPE STD*.";
		push @MmpWarn, "WARNING: Library statement NEWLIB ignored.\n";
		undef @DebugLibList;
		undef @LibList;
		undef @StatLibList;
	}

	if (($TrgType=~/^IMPLIB$/io) || ($TrgType=~/^NONE$/io)) {
		push @MmpDie, "ERROR: SOURCE not permitted with TARGETTYPE $TrgType\n" if (@SourceStruct);
	} elsif (!@SourceStruct) {
		push @MmpDie, "ERROR: No Sources specified\n";
	}
	if ($TrgType{NeedUID3} && $#UidList<1) {
		push @MmpWarn, "WARNING: No Uid3 specified in \"$MMPFILE\" for TargetType \"$TrgType\"\n";
	}
	if ($Trg && $Trg=~/\{|\}/) {
		push @MmpDie, "ERROR: Bad TARGET name specified\n";
	}
	if ($ExportLibrary and $ExportLibrary=~/\{|\}/) {
		push @MmpDie, "ERROR: Bad EXPORTLIBRARY name specified\n";
	}
	if ($LinkAs and $LinkAs=~/\{|\}/) {
		push @MmpDie, "ERROR: Bad LINKAS name specified\n";
	}
	# Make sure a valid compression method has been chosen if target is going to be paged
	if ($CodePagingTargetMode == PAGED && $CompressTarget == COMPRESS) {
		if ($CompressTargetMethod == INFLATECOMPRESSIONMETHOD) {
			push @MmpWarn, "Incompatible keywords! The INFLATECOMPRESSTARGET is not allowed with PAGED!\n";
			push @MmpWarn, "Changing compression method to BYTEPAIRCOMPRESSTARGET\n";
		}
		$CompressTargetMethod = BYTEPAIRCOMPRESSIONMETHOD;
	}

  # Ensure 2nd UID is not deprecated
  if (&main::EPOCSecurePlatform() && $TrgType{Basic} eq "DLL")
    {
    $deprecatedUIDs{"0x100039ce"} = "Unmigrated dll style application detected from use of UID 0x100039ce";
    }
  
  # Do not need UIDs for a resource only (i.e. TARGETTYPE is "none") mmp file
  if  ((!$TrgType=~/^NONE$/io) && &main::EPOCSecurePlatform() && defined($deprecatedUIDs{$UidList[0]}))
    {
    push @MmpMigrationNote, "$deprecatedUIDs{$UidList[0]}\n";
    }

#	PUT IN SOME DEFAULTS

	if ($TrgPath eq '') {
		# set up default path from $TrgType 
		$TrgPath = $TrgType{Path};
	}
	my $ResourceTrgPath = $TrgType{ResourcePath};
	if ($ResourceTrgPath eq '') {
		$ResourceTrgPath = $TrgPath;	# default to TrgPath, as before
	}
	unless (%Version) {
	    if ($$Plat_ref{DefFile} =~ /^\s*EABI\s*/i ) {
			$Version{major} = 10;	# Start major versions at 10 for EABI to allow coexistence with GCC98r2 ABI
		} else {
			$Version{major} = 1;
		}
		$Version{minor} = 0;
	}

	# check for languages
	@LangList=('SC') unless @LangList;

  if (&main::EPOCSecurePlatform() && !defined($Capability) &&
    (($TrgType{Basic} eq "DLL") || (uc($TrgType) eq "EXEXP")))
    {
    push @MmpMigrationNote, "No Capabilities set in \"$MMPFILE\" for TargetType: $TrgType\n";
    }

	# Supply default capability mask if not set explicitly, and convert to hex string
	$Capability = "none" unless (defined($Capability));
	$CapabilityFlags[0] = 0 unless (defined($CapabilityFlags[0]));
        $CapabilityFlags[1] = 0 unless (defined($CapabilityFlags[1]));

        $CapabilityFlags[0] = sprintf("0x%08x", $CapabilityFlags[0]);
        $CapabilityFlags[1] = sprintf("0x%08x", $CapabilityFlags[1]);

#	ensure all bitmaps have targetpaths and check for duplicate bitmaps
	my %BitMapCheck;
	my $BitMapRef;
	foreach $BitMapRef (@BitMapStruct) {
		unless ($$BitMapRef{TrgPath}) {
			$$BitMapRef{TrgPath}=$ResourceTrgPath;
		}
		if ($BitMapCheck{"$$BitMapRef{TrgPath}$$BitMapRef{Trg}"}) {
			push @MmpDie, "ERROR: Duplicate bitmap target \"$$BitMapRef{TrgPath}$$BitMapRef{Trg}\"\n";
			next;
		}
		$BitMapCheck{"$$BitMapRef{TrgPath}$$BitMapRef{Trg}"}=1;
	}

#	ensure all resources have targetpaths, expand language list and check for duplicates
	my %ResourceCheck;
	my $ResourceRef;
	my @PerLanguageResourceStruct;
	foreach $ResourceRef (@ResourceStruct) {
					
		unless ($$ResourceRef{BaseTrg}) {
			$$ResourceRef{BaseTrg}=&Path_Split('Base',$$ResourceRef{Source});
		}
		unless ($$ResourceRef{TrgPath}) {
			$$ResourceRef{TrgPath}=$ResourceTrgPath;
		}
		unless ($$ResourceRef{Lang}) {
			@{$$ResourceRef{Lang}}=@LangList;
		}
		# generate one instance per language.
		my @list = @{$$ResourceRef{Lang}};
		my $base = "$$ResourceRef{TrgPath}$$ResourceRef{BaseTrg}";
		foreach my $lang (@list) {
			my %newResourceRef = %{$ResourceRef};
			$newResourceRef{Lang} = $lang;
			$newResourceRef{Trg} = $base.lc("\.R$lang");
			push @PerLanguageResourceStruct, \%newResourceRef;
		}
	}
	@ResourceStruct = @PerLanguageResourceStruct;
	undef @PerLanguageResourceStruct;
	
	foreach $ResourceRef (@ResourceStruct) {
		if ($ResourceCheck{"$$ResourceRef{TrgPath}$$ResourceRef{Trg}"}) {
			push @MmpDie, "ERROR: Duplicate Resource target \"$$ResourceRef{TrgPath}$$ResourceRef{Trg}\"\n";
			next;
		}
		$ResourceCheck{"$$ResourceRef{TrgPath}$$ResourceRef{Trg}"}=1;
	}

	my $Plat=&main::Plat;

	if (@MmpDie || @MmpWarn || @MmpDiagnostic || @MmpMigrationNote) {
		warn "\nMMPFILE \"$MMPFILE\"\n";
		if (@MmpDiagnostic) {
			warn
				"DIAGNOSTIC MESSAGE(S)\n",
				@MmpDiagnostic,
				"\n"
			;
			}

		foreach my $printedWarning (@MmpWarn)
			{
			print "WARNING: $printedWarning\n";
			}
		foreach my $printedError (@MmpDie)
			{
			print "FATAL ERROR: $printedError\n";
			}

		foreach my $printedMigrationNote (@MmpMigrationNote)
			{
			print "MIGRATION_NOTE: $printedMigrationNote\n";
			}		

		if (@MmpDie)
			{
			die;
			}
	}


#	COMPLETE THE UID LIST

	while ($#UidList<1) {
		push @UidList, '0x00000000';
	}

	# check the second UID, or set it
	if ($TrgType{UID2}) {
		if ($UidList[0]=~/^0x00000000$/o) {
			# put in second uid for known targetypes without them
			$UidList[0]=$TrgType{UID2};
		}
		elsif ($UidList[0] ne $TrgType{UID2}) {
			# text comparison is OK here because UIDs have both been through AnyToHex function
			if ($UidList[0] ne '0x01111111') {
				# issue warning (but not if UID == 0x01111111 (to allow test code to deliberately set incorrect UID)
				warn(
					"WARNING: Second Uid is $UidList[0], but\n",
					" expected value for Target Type $TrgType is $TrgType{UID2}\n"
				);
			}
		}
	}

#	Put in the first UID in the list
	if ($TrgType{Basic}=~/^DLL$/o) {
		unshift @UidList, '0x10000079';
	}
	elsif ($TrgType{Basic}=~/^(EXE)$/o) {
		unshift @UidList, '0x1000007a';
	}
	elsif ($TrgType{Basic}=~/^(EXEDLL)$/o) {
# EXE on EPOC32, DLL on Emulator
# NOTE: On EKA1 EXEDLL used EXE UID1 on emulator
# On EKA2 this is unacceptable
		if (!$emulator) {
			unshift @UidList, '0x1000007a';
		} else {
			unshift @UidList, '0x10000079';
		}
	}
	else {
		unshift @UidList, '0x00000000';
	}

# If no SID specified use UID3
	$SecureId = $UidList[2] unless(defined $SecureId);

#	SORT OUT TARGET TYPE DATA STRUCTURE

	# set the target path
	if ($TrgPath) {
		$TrgType{Path}=$TrgPath;
	}

#	put in the $Linkas default
	if (!$LinkAs and $TrgType=~/^IMPLIB$/io) {
		$LinkAs = $Trg;
		$LinkAs =~ s/\.lib$/\.dll/i;
	} else {
		$LinkAs = $Trg unless $LinkAs;
	}
	$LinkAsBase = $LinkAs;
	unless ($emulator) {
		$LinkAs = &DecorateWithVersion($LinkAs, %Version);
	}

# If explicit version add to target
	if ($Version{explicit} && !$emulator) {
		$Trg = &DecorateWithVersion($Trg, %Version);
	}


#	Reconcile any EXEDLL targettypes - must be done after first UIDs put in the list
#	and after the $LinkAs keyword has been defined/defaulted
	if ($TrgType{Basic} eq 'EXEDLL') {
		$TrgType{Basic} = $emulator ? 'DLL' : 'EXE';
		$Trg=&Path_Split('Base',$Trg).'.EXE';
		$LinkAsBase=&Path_Split('Base',$LinkAsBase).'.EXE';
		$LinkAs=&Path_Split('Base',$LinkAs).'.EXE';
	}

#	put in the $ExportLibrary default
	$ExportLibrary=&Path_Split('Base',$Trg) unless $ExportLibrary;
	unless ($emulator) {
		$ExportLibrary = &DecorateWithVersion($ExportLibrary, %Version);
	}

#	Get the EPOC entrypoint static library
	unless ($FirstLib) {
		unless ($TrgType{FirstLib}) {
			$FirstLib="E$TrgType{Basic}.LIB";
		}
		else {
			$FirstLib=$TrgType{FirstLib};
		}
	}


#	 WORK OUT THE ASSP IMPLICATIONS

#	Nullify ASSP-specific API things for WINS/WINC since the API should always be
#	the same for WINC as for WINS,
	if ($emulator) {
		$ASSPABISwitch=0;
		$ASSPExports=0;
		unshift @LibList, @ASSPLibList;
		unshift @DebugLibList, @ASSPLibList;	# keep DebugLibList in sync with LibList
		@ASSPLibList=();
	}
	else {
#		Force ASSPABISwitch for known kernel components or if ASSPEXPORTS or ASSPLIBRARY specified in .MMP file
		if ($TrgType{Kernel} or $ASSPExports or @ASSPLibList) {
			$ASSPABISwitch=1;
		}
	}

#	Switch the ABI if necessary
	unless ($ASSPABISwitch) {
#		apply the standard ABI
		$ABI=$$Plat_ref{ABI};
	}
	else {
#		kernel-specific stuff
#		check that the platform is not generic
		if ($$Plat_ref{Generic}) {
			die "ERROR: Can't apply ASSPABI, ASSPEXPORTS or ASSPLIBRARY for generic platform \"$$Plat_ref{Name}\n";
		}

#		apply the ASSP-specific ABI
		$ABI=$$Plat_ref{ASSPABI};
	}

#	COMPLETE THE .DEF FILE STRUCTURE

	# apply the default .DEF filename, and
	# check as far as possible that the project is frozen
	if (($TrgType{NeedDeffile} or $CheckDef)) {
		unless ($Def{Path} and $Def{Path} !~ /\\\~\\$/) {
			my $augment;
			if ($ASSPExports) {
			    if ($$Plat_ref{DefFile} =~ /^\s*EABI\s*/i ) {
					$augment = $$Plat_ref{ASSP};
			    } else {
					$augment = "B$$Plat_ref{ASSP}";
			    }
			} else {
			    if ($$Plat_ref{DefFile} =~ /^\s*EABI\s*/i ) {
					$augment = lc($$Plat_ref{DefFile});
			    } else {
					$augment = lc("B$$Plat_ref{DefFile}");
			    }
			}
			if ($Def{Path} =~ /\\\~\\$/) {
				$Def{Path} =~ s/\\\~\\$/\\$augment\\/;
			} else {
				$Def{Path}=&Path_Strip(&Path_Split('Path',$MMPFILE)."..\\$augment\\");
			}
		}
		unless ($Def{Base}) {
			$Def{Base} = &Path_Split('Base',$LinkAsBase);
		}
		unless ($Def{Ext}) {
			$Def{Ext}='.def';
		}
#		now that we've dumped narrow, treat the 'u' basename suffix as part of the frozen
#		.DEF file basename.  Once we've dumped the suffix we won't have to store the extension
#		separately either
		if (!$emulator && $Version{explicit}) {
			$Def{Base} .= &Genutl_VersionToFileAugment(%Version);
		} elsif (!$NoStrictDef) {
			$Def{Base}.='u';
		}

		if ($Def{CheckSource_MMPEntry})
		{
			my $type = "DEFFILE";
			$type .= " (NOSTRICTDEF)" if ($NoStrictDef);
			
			# for GCCXML and X86GCC, ignore check source errors for missing .def file
			my $checkdef = (($IgnoreMissingDef) && ($Def{Path} =~ /[\\\/]bmarm[\\\/]/ || $$Plat_ref{Name} eq "X86GCC" || $$Plat_ref{Name} eq "X86GMP")) ? 0 : 1;
			
			if( $checkdef ) {
			  CheckSource_MetaData(%CheckSourceMMPMetaData, $Def{CheckSource_MMPFile}, $type, $Def{CheckSource_MMPEntry}, $Def{CheckSource_LineNumber}, $CheckSource_PhysicalCheck, $Def{Path});
			}
		}
			
#		check deffile exists,
        unless (-e "$Def{Path}$Def{Base}$Def{Ext}") {
            if($IgnoreMissingDef == 0) {
                warn "WARNING: Frozen .def file $Def{Path}$Def{Base}$Def{Ext} not found - project not frozen\n";
            }
     	}
	}
}

sub Mmp_ABI () {
	$ABI;
}
sub Mmp_AifStruct () {
	\@AifStruct;
}
sub Mmp_AllowDllData () {
	$AllowDllData;
}
sub Mmp_CompressTarget () {
	$CompressTarget;
}
sub Mmp_CompressTargetMode () {
	$CompressTargetMethod;
}
sub Mmp_ASSPExports () {
	$ASSPExports;
}
sub Mmp_ASSPLibList () {
	@ASSPLibList;
}
sub Mmp_BitMapStruct () {
	\@BitMapStruct;
}
sub Mmp_BuildAsARM () {
	$BuildAsARM;
}
sub Mmp_CallDllEntryPoints () {
	$CallDllEntryPoints;
}
sub Mmp_Capability () {
	$Capability;
}
sub Mmp_CapabilityFlags () {
	@CapabilityFlags;
}
sub Mmp_DataLinkAddress () {
	$DataLinkAddress;
}
sub Mmp_DebugLibList () {
	\@DebugLibList;
}
sub Mmp_Def () {
	\%Def;
}
sub Mmp_DocHash () {
	\%DocHash;
}
sub Mmp_ExportUnfrozen () {
	$ExportUnfrozen;
}
sub Mmp_FirstLib () {
	$FirstLib;
}
sub Mmp_FixedProcess () {
	$FixedProcess;
}
sub Mmp_HeapSize () {
	\%HeapSize;
}
sub Mmp_LibList () {
	\@LibList;
}
sub Mmp_LinkAs () {
	$LinkAs;
}
sub Mmp_LinkAsBase () {
	$LinkAsBase;
}
sub Mmp_ExportLibrary () {
	$ExportLibrary;
}
sub Mmp_NoExportLibrary () {
	$NoExportLibrary;
}
sub Mmp_NewLib () {
	$NewLib;
}
sub Mmp_Macros () {
	@Macros;
}
sub Mmp_MmpFlag () {
	\%MmpFlag;
}
sub	Mmp_PlatTxt2D () {
	@PlatTxt2D;
}
sub Mmp_ProcessPriority () {
	$ProcessPriority;
}
sub Mmp_RamTargets () {
	@RamTargets;
}
sub Mmp_ResourceStruct () {
	\@ResourceStruct;
}
sub Mmp_RomTargets () {
	@RomTargets;
}
sub Mmp_SourceStruct () {
	\@SourceStruct;
}
sub Mmp_StackSize () {
	$StackSize;
}
sub Mmp_StatLibList () {
	@StatLibList;
}
sub Mmp_StdCpp () {
	$StdCpp;
}
sub Mmp_NoStdCpp () {
	$NoStdCpp;
}
sub Mmp_SysIncPaths () {
	@SysIncPaths;
}
sub Mmp_Trg () {
	$Trg;
}
sub Mmp_TrgType () {
	\%TrgType;
}
sub Mmp_UidList () {
	@UidList;
}
sub Mmp_UserIncPaths () {
	@UserIncPaths;
}
sub Mmp_SrcDbg () {
	$SrcDbg;
}

sub Mmp_WarningLevel() {
     %Compiler; 
} 

sub Mmp_LinkerOptions() {
    %LinkerOptions
}

sub Mmp_Version() {
	%Version;
}

sub Mmp_SecureId() {
	$SecureId;
}

sub Mmp_VendorId () {
	$VendorId;
}

sub DecorateWithVersion($$) {
	my ($name, %ver) = @_;
	my $base = &Path_Split('Base', $name);
	my $ext = &Path_Split('Ext', $name);
	unless ($base =~ /\{(\d|a|b|c|d|e|f){8}\}$/i) {
		$base .= &Genutl_VersionToFileAugment(%Version);
	}
	return "$base$ext";
}

sub Mmp_Replace() {
	%Replace;
}

sub Mmp_SmpSafe() {
	$SmpSafe;
}

sub Mmp_StringTable() {
	\@StringTableStruct;
}
sub Mmp_ARMFPU() {
	$ARMFPU;
}

sub Mmp_CheckSourceMMPMetaData() {
	%CheckSourceMMPMetaData;
}

sub Mmp_CheckSourceMMPIncludes() {
	%CheckSourceMMPIncludes;
}

sub Mmp_CodePagingTargetMode() {
	$CodePagingTargetMode;
}

sub Mmp_DataPagingTargetMode() {
	$DataPagingTargetMode;
}

sub Mmp_IsWideCharMain() {
	$WideCharMain;
}

sub Mmp_IsDebuggable() {
	$IsDebuggable;
}

sub Mmp_IsFeatureVariant() {
	$FeatureVariant;
}

# Return the macros tested in the MMP file
sub Mmp_TestedMacros() {
	return \%mmptestedMacrosHash;
}

1;
