
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
# this package does various ancillary things for windows modules
# 
#

package Winutl;

my $BaseAddress='';
my @Win32LibList=();
my $Win32Resrc='';
my $CopyForStaticLinkage=0;
my $Win32StdHeaders=0;
my $MSVCVer=0;
my $MSVCSubVer=0;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	Winutl_Help_Mmp

	Winutl_DoMmp_Parse
	Winutl_DoMmp

	Winutl_BaseAddress
	Winutl_Win32LibList
	Winutl_Win32Resrc
	Winutl_CopyForStaticLinkage
	Winutl_Win32StdHeaders

	Winutl_AdjustTargetPath

	Winutl_MSVCVer
	Winutl_MSVCSubVer

	Winutl_CheckSourceMMPMetaData
);

use strict;
use Genutl;
use E32Variant;
use CheckSource;
use Pathutl;
use Cwd;

my %CheckSourceMMPMetaData;

sub Winutl_Help_Mmp {
# provide the help text for START <windows platforms> END blocks

	print
		"BASEADDRESS    [base address for dll loading]\n",
		"WIN32_LIBRARY  [win32 libraries]\n",
		"WIN32_RESOURCE  [win32 resource]\n",
		"COPY_FOR_STATIC_LINKAGE   // copy dll from emulated Z drive\n",
		"WIN32_HEADERS // instruct compiler to look into standard header directories\n",
		"              // (implied by WIN32_LIBRARY)\n"
	;
}

sub Winutl_DoMmp_Parse ($$) {
	# takes reference to platform text and semicolon-separated list
	# of compiler-specific include directories
	my @PlatTxt=@{$_[0]};
	my $CompilerIncPaths=$_[1];

# process the START <windows platforms> END blocks

	my $BaseTrg=&main::BaseTrg;
	my $BasicTrgType=&main::BasicTrgType;
	my $MakeFilePath=&main::MakeFilePath;
	my $MMPFILE=&main::MmpFile;

	# set up START WINS ... END block module variables
	my @MmpWarn=();
	my $Line;
	LINE: foreach $Line (@PlatTxt) {
		my $LineInfo=shift @$Line;

		$LineInfo =~ /^(.+)\((\d+)\)$/;
		my $CurFile = $1;
		my $LineNum = $2;
		my $BldInfDir = cwd;
		$BldInfDir =~ s/\//\\/g;
		$BldInfDir =~ s/^\w+://;
		$BldInfDir .= "\\";
		
		$_=shift @$Line;
		
		if (/^BASEADDRESS$/oi) {
			if (@$Line) {
				$BaseAddress=shift @$Line;
				$BaseAddress=~s/X/x/o;
				next LINE;
			}
			push @MmpWarn, "$LineInfo : No base address specified for keyword BASEADDRESS\n";
			next LINE;
		}
		if (/^WIN32_LIBRARY$/oi) {
			if (@$Line)
				{
				$Win32StdHeaders = 1;

				foreach (@$Line)
					{
					if (/^\./)
						{
						# local - check for UNIX slash and physical consistency of file as it exists relative to the bld.inf
						CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "WIN32_LIBRARY", $_, $LineNum, $CheckSource_PhysicalCheck, $BldInfDir);
						}
					else
						{
						# global - check for UNIX slash and assume that it must be lower case
						CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "WIN32_LIBRARY", $_, $LineNum);
						}
					$_ = &Path_Norm($_);
					push @Win32LibList, $_;
					}
				}
			else
				{
				push @MmpWarn, "$LineInfo : No libraries specified for keyword WIN32_LIBRARY\n";
				}
			next LINE;
		}
		if (/^WIN32_RESOURCE$/oi) {
			if (@$Line) {
				$Win32Resrc=shift @$Line;

				CheckSource_MetaData(%CheckSourceMMPMetaData, $CurFile, "WIN32_RESOURCE", $Win32Resrc, $LineNum, $CheckSource_PhysicalCheck);
				$Win32Resrc = &Path_Norm($Win32Resrc);
				
				$Win32Resrc=&main::Path_MakeAbs($MMPFILE, $Win32Resrc);
				next LINE;
			}
			push @MmpWarn, "$LineInfo : No resource specified for keyword WIN32_RESOURCE\n";
			next LINE;
		}
		if (/^COPY_FOR_STATIC_LINKAGE$/oi) {
			if ($BasicTrgType!~/^DLL$/oi) {
				push @MmpWarn, "$LineInfo : COPY_FOR_STATIC_LINKAGE only applies to DLLs\n";
				next LINE;
			}
			if (&main::TrgPath eq "") {
				push @MmpWarn, "$LineInfo : COPY_FOR_STATIC_LINKAGE requires TARGETPATH\n";
				next LINE;
			}
			$CopyForStaticLinkage=1;
			next LINE;
		}
		if (/^WIN32_HEADERS$/oi) {
			$Win32StdHeaders = 1;
			next LINE;
		}
		
		push @MmpWarn, "$LineInfo : Unrecognised Keyword \"$_\"\n";
	}

	undef $Line;
	if (@MmpWarn) {
		warn
			"\nMMPFILE \"",$MMPFILE,"\"\n",
			"START .. END BLOCK WARNINGS(S)\n",
			@MmpWarn,
			"\n"
		;
	}
	undef @MmpWarn;

	# if Win32 Libraries required - set the Windows standard include paths
	if ($Win32StdHeaders || $Win32Resrc || &main::Plat eq 'TOOLS' || &main::Plat eq 'CWTOOLS'
		|| &main::Plat eq 'TOOLS2')
	{	# show where to find win32 libraries
		# include env variable takes everything between ';', including spaces and '"', as part of path
		my @StdIncPaths=split ';', $CompilerIncPaths;
		my $path;
		foreach $path (@StdIncPaths) {
			$path =~ s-/-\\-go;	# for those working with UNIX shells
			if ($path =~ /^\+/) {
				# expand CodeWarrior "recursive" entries
				$path =~ s-^\+--go;		# remove the + from the current entry
				if (opendir DIR, $path) {
					my @list = grep !/^\.\.?$/, readdir DIR;
					closedir DIR;
					foreach (@list) {
						my $newpath="$path\\$_";
						if (-d $newpath) {
							push @StdIncPaths,"+$newpath";	# add subdirs for later expansion
						}
					}
				}
			}
		}
		&main::SetStdIncPaths(@StdIncPaths);
		&main::AddPlatMacros('WIN32','_WINDOWS');
	}
}

sub Winutl_DoMmp ($$) {
	# takes reference to platform text and semicolon-separated list
	# of compiler-specific include directories
	my @PlatTxt=@{$_[0]};
	my $CompilerIncPaths=$_[1];

	my $Plat=&main::Plat;
	if ($Plat ne "WINSCW" and $Plat ne "CWTOOLS" and $Plat ne "TOOLS2") {
		#	check that we're using VC6 SP3
		&Winutl_MSVCVer();
	}

	&Winutl_DoMmp_Parse(\@PlatTxt, $CompilerIncPaths);
	
	my $BaseTrg=&main::BaseTrg;
	my $BasicTrgType=&main::BasicTrgType;
	my $MakeFilePath=&main::MakeFilePath;
	my $MMPFILE=&main::MmpFile;
	my @UidList=&main::UidList;
	
	if ($BasicTrgType=~/^(EXE|DLL)$/oi) {
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
		unless (&main::Plat eq 'TOOLS' || &main::Plat eq 'CWTOOLS' || &main::Plat eq 'TOOLS2' ) {
			&main::AddSrc("$MakeFilePath$BaseTrg.UID.CPP", $UidText);
		};
	}

}

sub Winutl_BaseAddress () {
	$BaseAddress;
}

sub Winutl_Win32LibList () {
	@Win32LibList;
}

sub Winutl_Win32Resrc () {
	$Win32Resrc;
}

sub Winutl_CopyForStaticLinkage () {
	$CopyForStaticLinkage;
}

sub Winutl_Win32StdHeaders () {
	$Win32StdHeaders;
}

sub Winutl_AdjustTargetPath () {
	my ($TrgPathRef) = @_;

	if (&main::EPOCSecurePlatform) {

		my $plan=1;
		my @macros = &Variant_GetMacroList;
		foreach my $macro (@macros) {
			if ($macro =~ m/^SYMBIAN_IGNORE_BIN_TARGETPATH.*/) {
				$plan = 2;
				last;
			}
		}

		if ($plan == 1) {
			# Intermediate step: TARGETPATH => COPY_FOR_STATIC_LINKAGE
			$CopyForStaticLinkage = 1 if ($$TrgPathRef ne "");
		} else {
			# Finally: Ignore TARGETPATH and COPY_FOR_STATIC_LINKAGE
			# unless it's a subdir of sys\bin
			if (&main::TrgPath !~ /^Z\\sys\\bin\\.+/i) {
				$$TrgPathRef = "";
				$CopyForStaticLinkage = 0;
			}
		}
	}
}

sub Winutl_MSVCVer ($) {
	my $platcommand=shift;
	if(!defined $platcommand) {
		$platcommand=0; }
	open PIPE, "LINK.EXE 2>&1 |" or die "ERROR: Can't invoke LINK.EXE\n";
	my $DoneCheck=0;
	while (<PIPE>) {
		unless ($DoneCheck) {
			if (/^.+\s+Version\s+(\d)\.(\d{2})\.((\d{4})|(\d{5})(.\d{2}))\s*$/o) {
				if (($1<6) or ($1==6 and $2<0) or ($1==6 and $2==0 and $3<8447)) {
					warn "WARNING: Should install MSVC6 Service Pack 3\n";
				}
				$MSVCVer = $1;
				$MSVCSubVer = $2;
				$DoneCheck=1;
			}
		}
	}
	close PIPE;
	# Do not throw any error when link.exe not present  while displaying
	# a list of the supported platforms using bldmake plat command.
	if (!$DoneCheck && !$platcommand) {
		# Couldn't find version information? Might not have link.exe at all
		die "ERROR: failed to find version information for LINK.EXE\n";
	}

	$MSVCVer;
}

sub Winutl_MSVCSubVer ($) {
	my $platcommand=@_;
	&Winutl_MSVCVer($platcommand);

	$MSVCSubVer;
}

sub Winutl_CheckSourceMMPMetaData () {
	%CheckSourceMMPMetaData;
}

1;
