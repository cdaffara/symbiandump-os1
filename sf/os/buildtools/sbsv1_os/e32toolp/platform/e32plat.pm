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
# Module providing platform details for platforms supported
# by Symbian OS
# all data is uppercase
# 
#

package E32Plat;

require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	Plat_SetVerbose
	Plat_Init
	Plat_GetL
	Plat_List
	Plat_AssocIDE
	Plat_Customizations
	Plat_Customizes
	Plat_Root
	Plat_SupportsFeatureVariants
);

use strict;
use Winutl;
use RVCT_plat2set;
use BPABIutl;
use E32Variant;

my $variantABIV2Keyword = &Variant_GetMacro();

my %Mode=(
	Verbose=>0
);
my $ModulePath;

sub Plat_SetVerbose () {
	$Mode{Verbose}=1;
}

my %BldLists=(
	EPOC32=>['UREL','UDEB'],
	WINS=>['UDEB','UREL'],
	TOOLS=>['DEB','REL'],
	TOOLS2=>['DEB','REL'],
);

my %BldMacros=(
	DEB=>['_DEBUG'],
	REL=>['NDEBUG'],
	UDEB=>['_DEBUG','_UNICODE'],
	UREL=>['NDEBUG','_UNICODE']
);


my @EpocMacros=('__SYMBIAN32__');

my @BPABIPlats = &BPABIutl_Plat_List;

my %Plat=(
	ARM4=>{
		ABI=>'ARM4',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
	},
	ARM4SMP=>{
		ABI=>'ARM4',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		SMP=>1,
		StatLink=>'ARM4SMP',
	},
	ARM4T=>{
		ABI=>'ARM4T',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
	},
	ARMI=>{
		ASSP=>'MARM',
		Generic=>1,
		ASSPABI=>'',
	},
	SARM4=>{
		ABI=>'ARM4',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		Single=>1,
	},
	SARMI=>{
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		Single=>1,
	},
	STHUMB=>{
		ABI=>'THUMB',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		Single=>1,
	},
	THUMB=>{
		ABI=>'THUMB',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
	},
	TOOLS=>{
		ABI=>'TOOLS',
		ASSPABI=>'',
		Compiler=>'VC32',
		CPU=>'TOOLS',
		OS=>'TOOLS',
		MakeMod=>'Cl_win',
		MakeCmd=>'nmake',
	},
	TOOLS2=>{
		ABI=>'TOOLS2',
		ASSPABI=>'',
		Compiler=>'GCC32',
		CPU=>'TOOLS2',
		OS=>'TOOLS2',
		MakeMod=>'Cl_mingw',
		MakeCmd=>'make',
	},
	CWTOOLS=>{
		ABI=>'TOOLS',
		ASSPABI=>'',
		Compiler=>'CW32',
		CPU=>'TOOLS',
		OS=>'TOOLS',
		MakeMod=>'Cl_tools',
		MakeCmd=>'make',
	},
	VC6TOOLS=>{
		ABI=>'TOOLS',
		ASSPABI=>'',
		Compiler=>'VC32',
		CPU=>'TOOLS',
		Ext=>'.DSP',
		MakeMod=>'Ide_vc6',
		MakeCmd=>'nmake',
		OS=>'TOOLS',
		Real=>'TOOLS',
		UsrHdrsOnly=>1,
	},
	WINS=>{
		ABI=>'WINS',
		ASSPABI=>'',
		Compiler=>'VC32',
		CPU=>'WINS',
		MakeMod=>'Cl_win',
		MakeCmd=>'nmake',
		OS=>'WINS',
	},
	VC6=>{
		ABI=>'WINS',
		ASSPABI=>'',
		Compiler=>'VC32',
		CPU=>'WINS',
		Ext=>'.DSP',
		MakeMod=>'Ide_vc6',
		MakeCmd=>'nmake',
		OS=>'WINS',
		Real=>'WINS',
		UsrHdrsOnly=>1,
	},
	WINSCW=>{
		ABI=>'WINSCW',
		ASSPABI=>'',
		Compiler=>'CW32',
		CPU=>'WINS',
		MakeMod=>'Cl_codewarrior',
		OS=>'WINS',
		DefFile=>'WINS',	# use the MSVC def files
	},
	CW_IDE=>{
		ABI=>'WINSCW',
		ASSPABI=>'',
		Compiler=>'CW32',
		CPU=>'WINS',
		Ext=>'.xml',
		MakeMod=>'Ide_cw',
		MakeCmd=>'make',
		OS=>'WINS',
		Real=>'WINSCW',
		DefFile=>'WINS',	# use the MSVC def files
		UsrHdrsOnly=>1,
		SupportsMultiplePlatforms=>1,	# supports more than one real platform
	},
	X86=>{
		ABI=>'X86',
		ASSPABI=>'',
		Compiler=>'VC32',
		CPU=>'X86',
		MakeMod=>'Cl_x86',
		MakeCmd=>'nmake',
		OS=>'EPOC32',
		DefFile=>'X86',
		Generic=>1,
	},
	X86SMP=>{
		ABI=>'X86',
		ASSPABI=>'',
		Compiler=>'VC32',
		CPU=>'X86',
		MakeMod=>'Cl_x86',
		MakeCmd=>'nmake',
		OS=>'EPOC32',
		DefFile=>'X86',
		Generic=>1,
		SMP=>1,
		StatLink=>'X86SMP',
	},
	X86GCC=>{
		ABI=>'X86gcc',
		ASSPABI=>'',
		Compiler=>'X86GCC',
		CPU=>'X86',
		MakeMod=>'Cl_x86gcc',
		OS=>'EPOC32',
		DefFile=>'x86gcc',
		Generic=>1,
	},	
	X86GMP=>{
		ABI=>'X86gcc',
		ASSPABI=>'',
		Compiler=>'X86GCC',
		CPU=>'X86',
		MakeMod=>'Cl_x86gcc',
		OS=>'EPOC32',
		DefFile=>'x86gcc',
		Generic=>1,
		SMP=>1,
		StatLink=>'X86GMP',
	},	
	ARMV4=>{
		ABI=>'ARMV4',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		MakeMod=>'Cl_arm',
		Compiler=>'ARMCC',
		DefFile=>'EABI',
		EABI=>1,
	},
	ARMV4SMP=>{
		ABI=>'ARMV4',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		MakeMod=>'Cl_arm',
		Compiler=>'ARMCC',
		DefFile=>'EABI',
		EABI=>1,
		SMP=>1,
		StatLink=>'ARMV4SMP',
	},
	ARMV5_ABIV1=>{
		ABI=>'ARMV5',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		MakeMod=>'Cl_arm',
		Compiler=>'ARMCC',
		DefFile=>'EABI',
		EABI=>1,
		SupportsFeatureVariants=>1,
	},
	ABIV2=>{
		ABI=>'ARMV5',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		MakeMod=>'Cl_bpabi',
		DefFile=>'EABI',
		EABI=>1,
		SupportsFeatureVariants=>1,
	},
	GCCXML=>{
		ABI=>'ARM4',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		MakeMod=>'cl_gccxml',
	},
	VS6=>{
		ABI=>'WINSCW',
		ASSPABI=>'',
		Compiler=>'CW32',
		CPU=>'WINS',
		MakeMod=>'Cl_vscw',
		OS=>'WINS',
		Real=>'WINSCW',
		DefFile=>'WINS',	# use the MSVC def files
		Ext=>'.mak'		
	},
	VS2003=>{
		ABI=>'WINSCW',
		ASSPABI=>'',
		Compiler=>'CW32',
		CPU=>'WINS',
		MakeMod=>'Cl_vscw',
		OS=>'WINS',
		Real=>'WINSCW',
		DefFile=>'WINS',	# use the MSVC def files
		Ext=>'.mak'
	},
	EDG=>{
		ABI=>'ARMV5',
		ASSP=>'MARM',
		ASSPABI=>'',
		Generic=>1,
		MakeMod=>'cl_edg',
	},

	# ASSP platforms should be described using .ASSP files
	# Do not add additional ASSP platforms to this file.
);

sub Set_Plat() 
{
	@BPABIPlats = &BPABIutl_Plat_List;
	foreach my $Candidate (@BPABIPlats)
	{
# All BPABI platforms inherit from ABIV2 properties as listed in the platlist
# and Platlist is updated to include the BPABI platforms.
		my ( $key, $value);
		while (($key, $value) = each %{$Plat{ABIV2}}) {
			$Plat{$Candidate}{$key}=$value;
		}
	}
}

sub Plat_SupportsFeatureVariants($)
	{
	my ($plat) = @_;

	# In a non-ABIV2 world, ARMV5 means ARMV5_ABIV1 within e32plat content
	if (!$variantABIV2Keyword && $plat =~ /^ARMV5$/i)
		{
		$plat .= "_ABIV1";
		}
		
	return ($plat && defined $Plat{$plat}{SupportsFeatureVariants}) ? $Plat{$plat}{SupportsFeatureVariants} : 0;
	}

sub Plat_Customizations($) {
        my ($plat) = @_;
	my @empty = ();
	return @{$Plat{$plat}{'CUSTOMIZATIONS'}} if $Plat{$plat}{'CUSTOMIZATIONS'};
	return @empty;
      }

sub Plat_Customizes($) {
	my ($plat) = @_;
	return $Plat{$plat}{'CUSTOMIZES'} ? $Plat{$plat}{'CUSTOMIZES'} : "";
}

sub Plat_Root($) {
	my ($plat) = @_;

	my $RootName = $Plat{$plat}{'ROOTPLATNAME'};

	if ($RootName) {
		return $RootName;
	}
	else {
		# A non-BSF platform is its own root.
		return $plat;
	}
}

sub Init_BSFs($) {
	my ($Path)=@_;  
#	get a list of modules
	opendir DIR, $Path;
	my @BSFs=grep s/^([^\.].*)\.BSF$/$1/, map { uc $_ } sort readdir DIR;
	closedir DIR;

	my $BSF;
	foreach $BSF (@BSFs) {
		my $File=$Path.$BSF.'.bsf';
#		check whether the assp is already defined
		if (defined %{$Plat{$BSF}}) {
			warn(
				"$File : warning: Platform \"$BSF\" already defined\n",
				" ... skipping this spec\n"
			);
			delete $Plat{$BSF};
			next;
		}
#		open the module
		unless (open FILE, $File) {
			delete $Plat{$BSF};
			warn "warning: Can't open BSF specification \"$File\"\n";
			next;
		}
		my $line1 = <FILE>;
		$line1 = uc($line1);
		unless ($line1 =~ /^\#\<BSF\>\#/) {
			warn "warning: \"$File\" Invalid BSF specification - missing #<bsf>#\n";
			delete $Plat{$BSF};
			close FILE;
                  next;
            }
            my $custom;
            while ($custom = <FILE>) {
					#skip blank lines and comments
			delete $Plat{$BSF};
					last unless ($custom =~ /^$|^\#/);
            }
            $custom = uc $custom;
            unless ($custom =~ /^\s*CUSTOMIZES\s+(\S+)/) {
				warn "warning: \"$File\" Invalid BSF specification - 'customizes' missing\n";
				delete $Plat{$BSF};
				close FILE;
			next;
            }
		my $root = $1;
		my $platname = '';
		my $CustomizedPlatName = '';		

		# In v1 mode, ARMV5 platform implies ARMV5_ABIV1 platform listed in the platlist		
		my $Armv5Flag = 0;
		if (!$variantABIV2Keyword && $root =~ /^ARMV5$/i) {
			$Armv5Flag = 1;
		}

		# Support for Hierarchy of Customizations (BSF file customization of another BSF file)
		# 1. Check whether the BSF file customizes another BSF file.
		# 2. If so, check whether the root BSF file has already been read.
		# 3. If not read, then defer the current BSF file reading until the root file is read.
		my $rootPlatFound = 0;
		if (defined %{$Plat{$root}} || $Armv5Flag) 
		{
			# BSF platform customizes another valid BSF platform
			if (defined $Plat{$root}{'CUSTOMIZES'}) 
			{
				$rootPlatFound = 1;
				$platname = $root;
				$CustomizedPlatName = $root;

				# Set the root platform name which is same as of customizes platform
				$Plat{$BSF}{'ROOTPLATNAME'} = $Plat{$root}{'ROOTPLATNAME'};
			}
			# BSF platform customizes to one of the existing ABI platforms
			else
			{
				# All BPABI platforms inherits from ABIV2 platform listed in the platlist
				if (grep /^$root$/i, @BPABIPlats) {
					$platname = "ABIV2";
				}
				elsif ($Armv5Flag) {
				# In v1 mode, ARMV5 platform implies ARMV5_ABIV1 platform listed in the platlist
					$platname = "ARMV5_ABIV1";	
				}
				else {
					$platname = $root;
				}
				
				$CustomizedPlatName=$root;

				# BSF File check Begins 
				# The following check is included to handle the existing BSF files which has to behave in different manner
				# in default v1 mode and v2 mode. The following code changes the BSF name and the custmoized platform name
				# to the implied names. This is done to support switching between v1 and v2 modes by enabling the keyword in
				# the variant configuration file.
				# In v1 mode, the ARMV6_ABIV1 => ARMV6 platform and ARMV6 => ARMV6_ABIV2 platform.
				if (!$variantABIV2Keyword) {
					if ($BSF =~ /^ARMV6_ABIV1$/i) {
						$BSF = "ARMV6";	
						$CustomizedPlatName = "ARMV5";	
					}
					elsif ($BSF =~ /^ARMV6$/i) {
						$BSF = "ARMV6_ABIV2";	
						$CustomizedPlatName = "ARMV5_ABIV2";
						$platname = "ABIV2";
					}
				}
				# BSF File check Ends

				# Set the root platform name
				$Plat{$BSF}{'ROOTPLATNAME'} = $CustomizedPlatName;
			}			
		}
		else
		{
			my $rootbsf = $Path.$root.".bsf";			
			if ( -e $rootbsf ) {
				# BSF file customizes another BSF file which has not been read yet.
				# So defer current BSF file reading until the root BSF file is read.				
				delete $Plat{$BSF};
				push(@BSFs, $BSF);
				next;		
			}
		}
		# If the customizes platform is not a valid BSF platform or BPABI platorm or ARMV5 or ARMV5_ABIV1,
		# then throw warning.
		unless ($rootPlatFound || $root =~ /^ARMV5(_ABIV1)?$/ || (grep /^$root$/i, @BPABIPlats)) {
			warn "warning: \"$File\" Invalid BSF specification - customization restricted to ARMV5, ABIv2 and valid BSF platforms\n";
			close FILE;
			delete $Plat{$BSF};
			next;
		}
			
		my ( $key, $value);
		while (($key, $value) = each %{$Plat{$platname}}) {
			$Plat{$BSF}{$key}=$value;
		}
		
		push @{$Plat{$CustomizedPlatName}{'CUSTOMIZATIONS'}}, $BSF;
		$Plat{$BSF}{'CUSTOMIZES'} = $CustomizedPlatName;
		while (<FILE>) {
			next if (/^$|^\#/);
			if (/^\s*SMP\s*$/i) {
				$Plat{$BSF}{'SMP'} = 1;
				$Plat{$BSF}{'StatLink'} = lc $BSF;
				next;
			}
			$Plat{$BSF}{'CUSTOMIZATION_DATA'} .= $_;
		}
		# BSF file statements will have newline character("\n") at the end, except for the last statement.
		# So append "\n" for the last BSF file statement.
		# "\n" will be used to split BSF statements to support hierarchy of customizations.
		$Plat{$BSF}{'CUSTOMIZATION_DATA'} .= "\n";
		close FILE;
	}
	1;
}

sub Plat_Init ($) { # takes path to ASSP modules
	my ($Path)=@_;

	my %PlatHashKeys=(
		ABI=>1,
		ASSPABI=>1,
		SINGLE=>1,
		Compiler=>1,
		CPU=>1,
		MakeMod=>1,
		MakeCmd=>1,
		OS=>1,
		DefFile=>1,
		ASSP=>1,
	);

#	Include the list of BPABI platforms
	&Set_Plat;

	Init_BSFs($Path);

#	get a list of modules
	opendir DIR, $Path;
	my @_ASSPs=grep s/^([^\.].*)\.ASSP$/$1/, map { uc $_ } readdir DIR;
	closedir DIR;

	my @ASSPs;
	foreach (@_ASSPs) {
		if (!$ENV{USEARMCC} and /EDG$/i) {
#			warn(
#				"Note: ASSP \"$_\" disabled\n"
#			);

			next;
		}
		push @ASSPs, $_;
	}

#	open each module in turn, and add it to the array
	my $ASSP;
	foreach $ASSP (@ASSPs) {
		my $File=$Path.$ASSP.'.assp';
#		check whether the assp is already defined
		if (defined %{$Plat{$ASSP}}) {
			warn(
				"$File : warning: ASSP \"$ASSP\" already defined\n",
				" ... skipping this module\n"
			);

			next;
		}
#		open the module
		unless (open FILE, $File) {
			warn "warning: Can't open assp module \"$File\"\n";
			next;
		}
		my %Data=();
		my %SingleData=();
		my $MatchingSingle="";
		my @Errors=();
		while (<FILE>) {
#			strip comments
			s/^([^#]*)#.*$/$1/o;
#			skip blank lines
			if (/^\s*$/o) {
				next;
			}
#			get the key-value pair
			unless (/^\s*(\w+)\s+(\w+)\s*$/o) {
				push @Errors, "$File($.) : warning: syntax error - only key-value pairs allowed\n";
				next;
			}
			my ($Key, $Val)=($1, $2);
			if ($PlatHashKeys{$Key}!=1) {
				push @Errors, "$File($.) : warning: unrecognized keyword - $Key\n";
				next;
			}
			if ($Key eq "SINGLE") {
				$SingleData{Single} = 1;
				$SingleData{ASSP} = $ASSP;
				$MatchingSingle = uc $2;
			} else {
				$Data{$Key}=$Val;
				$SingleData{$Key}=$Val;
			}
		}
		close FILE;
		if (@Errors) {
			warn(
				@Errors,
				" ... skipping this module\n"
			);
			next;
		}
# change -  Allow ASSPs to pick up all the options of the ABI they specify, 
# in particular the compiler they need.
			$Data{'ASSP'} = $ASSP unless $Data{'ASSP'};
			if ($Plat{$Data{'ABI'}}) {
			foreach (keys %{$Plat{$Data{'ABI'}}}) {
			$Data{$_} = $Plat{$Data{'ABI'}}{$_} unless ($_ =~ /^GENERIC$/i) or $Data{$_};
			}
		}

		%{$Plat{$ASSP}}=%Data;
		if ($MatchingSingle ne "") {
			foreach (keys %Data) {
			$SingleData{$_} = $Data{$_} unless ($_ =~ /^GENERIC$/i) or $SingleData{$_};
			}
			%{$Plat{$MatchingSingle}}=%SingleData;
		}			
	}
}

sub Plat_GetL ($$$$) { # takes Platform name, ref to plat hash, ref to bldmacrohash, bldmake plat command notifier
	my ($Candidate,$PlatHash_ref,$BldMacrosHash_ref,$platcommand)=@_;
	$Candidate=uc $Candidate;

# is platform in our list?
	unless (defined $Plat{$Candidate}) {

		# is platform BPABI compliant one?
		if (!$variantABIV2Keyword && $Candidate eq 'ARMV5') {
		}
		elsif (not(grep /^$Candidate$/i, @BPABIPlats))
		{
			die "ERROR: Platform \"$Candidate\" not supported\n";
		}
	}

	my $BPABIPlat;
	my %PlatHash=();

# check the data
# All BPABI platforms inherit ABIV2 properties 
	if (grep /^$Candidate$/i, @BPABIPlats) 
	{
		$BPABIPlat='ABIV2';
		%PlatHash=%{$Plat{$BPABIPlat}};
	}
# In v1 mode, ARMV5 platform implies ARMV5_ABIV1 platform listed in the platlist
	elsif (!$variantABIV2Keyword && $Candidate eq 'ARMV5') {
		%PlatHash=%{$Plat{ARMV5_ABIV1}};
	}
	else {
		%PlatHash=%{$Plat{$Candidate}};
	}

# set the defaults
	$PlatHash{Name}=$Candidate;
	$PlatHash{Real}=$PlatHash{Name} unless $PlatHash{Real};
	$PlatHash{Ext}=".$PlatHash{Real}" unless $PlatHash{Ext};
	$PlatHash{ASSP}=$PlatHash{Real} unless $PlatHash{ASSP};

# Get the root platform name to support hierarchy of customizations
	my $CustomizedPlat=$PlatHash{'ROOTPLATNAME'};

	if ((defined($Candidate)  && ($Candidate =~ /^ARMV5/i)) || (defined($CustomizedPlat) && ($CustomizedPlat =~ /^ARMV5/i))) {
# Compiler name should be set as ARMCC for all ARMV5 platforms
		$PlatHash{Compiler}='ARMCC';
		$PlatHash{Toolchain}='rvct22';
	}
	elsif ($BPABIPlat) {
# Compiler name should be set as that of platform name for all BPABI platforms
		$PlatHash{Compiler}=$Candidate;
		$PlatHash{Toolchain}=$Candidate;
	}
	elsif ($CustomizedPlat) {
# Compiler name should be set as that of the customized platform name incase of customization
		$PlatHash{Compiler}=$CustomizedPlat;
		$PlatHash{Toolchain}=$CustomizedPlat;
	}

	$PlatHash{Compiler}='GCC32' unless $PlatHash{Compiler};
	$PlatHash{OS}='EPOC32' unless $PlatHash{OS};
	$PlatHash{MakeMod}='Cl_gcc' unless $PlatHash{MakeMod};
	$PlatHash{MakeCmd}='make' unless $PlatHash{MakeCmd};
	$PlatHash{CPU}='MARM' unless $PlatHash{CPU};
	$PlatHash{Single}=0 unless $PlatHash{Single};
	$PlatHash{UsrHdrsOnly}=0 unless $PlatHash{UsrHdrsOnly};
	$PlatHash{Generic}=0 unless $PlatHash{Generic}; # generic means "for a target device but no particular ASSP"
	$PlatHash{SupportsMultiplePlatforms}=0 unless $PlatHash{SupportsMultiplePlatforms};

	$PlatHash{ABI}='ARMI' unless $PlatHash{ABI};
	$PlatHash{ASSPABI}='ARM4' unless defined $PlatHash{ASSPABI};

	unless (defined $PlatHash{DefFile}) {
		if ($PlatHash{Compiler} eq 'VC32') {
			$PlatHash{DefFile}='WINS';
		} else {
			$PlatHash{DefFile}='MARM';
		}
	}
	
# .MMP macros - keeping the order is useful
	@{$PlatHash{MmpMacros}}=$PlatHash{Compiler};
	push @{$PlatHash{MmpMacros}}, $PlatHash{OS};
	push @{$PlatHash{MmpMacros}}, $PlatHash{CPU} unless $PlatHash{CPU} eq $PlatHash{OS};
	push @{$PlatHash{MmpMacros}}, $PlatHash{ASSP} unless $PlatHash{ASSP}=~/^($PlatHash{CPU}|$PlatHash{OS})$/;

	if ($PlatHash{Single}) {
		push @{$PlatHash{MmpMacros}}, 'SINGLE';
	}
	if ($PlatHash{SMP}) {
		push @{$PlatHash{MmpMacros}}, 'SMP';
	}
	if ($PlatHash{EABI}) {
		push @{$PlatHash{MmpMacros}}, 'EABI';
	}
	if ($PlatHash{Compiler} eq 'VC32') {
		my $MSVCVer = &Winutl_MSVCVer($platcommand);
		my $MSVCSubVer = &Winutl_MSVCSubVer($platcommand);
		push @{$PlatHash{MmpMacros}}, 'MSVC'.$MSVCVer;
		push @{$PlatHash{MmpMacros}}, 'MSVC'.$MSVCVer.$MSVCSubVer;
		if ($MSVCVer > 6) {
			push @{$PlatHash{MmpMacros}}, 'MSVCDOTNET';
		}
	}
	if ($PlatHash{Compiler} eq 'ARMCC') {
		my ($MajVer, $MinVer) = RVCT_plat2set::get_version_list($Candidate);
		push @{$PlatHash{MmpMacros}}, 'ARMCC_'.$MajVer;
		push @{$PlatHash{MmpMacros}}, 'ARMCC_'.$MajVer.'_'.$MinVer;
	}
	
	if ($PlatHash{Compiler} eq 'X86GCC') {	
			push @{$PlatHash{MmpMacros}}, 'GCC32';
	}

	## TOOLS2 Specific Macros ##
	
	if ($PlatHash{Compiler} eq 'GCC32') {
		push @{$PlatHash{MmpMacros}}, 'MINGW32';
		push @{$PlatHash{MmpMacros}}, '_STLP_LITTLE_ENDIAN';
		
	}

    # add GCCXML to the mmp macro list, so we can filter out stuff in bld.inf files and mmps.
    # note that this mean that __GCCXML__ is automacally routed to makefile, so we can take out explicit call. 
    if( $PlatHash{Name} eq "GCCXML") {
        push @{$PlatHash{MmpMacros}}, 'GCCXML';
    }
	
# add specific platform macros for platforms customising others.
	if (Plat_Customizes($PlatHash{Name})) {
		push @{$PlatHash{MmpMacros}}, $PlatHash{Name} unless grep /$PlatHash{Name}/, @{$PlatHash{MmpMacros}};
	}

# compilation macros
	@{$PlatHash{Macros}}=@EpocMacros;
	foreach (@{$PlatHash{MmpMacros}}) {
		## If STLP_LTTLE_ENDIAN is used, then dont append __ to the macro name ##
		if ($_ =~ m/STLP_LITTLE_ENDIAN/) {
			push @{$PlatHash{Macros}}, $_;
		} else {
		push @{$PlatHash{Macros}}, '__'.$_.'__';
		}
	}

#	extra special .MMP macros which aren't applied for compilation
	if ($PlatHash{Generic}) {
		push @{$PlatHash{MmpMacros}}, "GENERIC_$PlatHash{CPU}";
		if ($PlatHash{CPU} eq 'MARM') {
#			we can't define this for ASSP platforms because we won't be sure what
#			the ABI is until we've processed the .MMP file
			push @{$PlatHash{MmpMacros}}, "MARM_$PlatHash{ABI}";
		}
	}

# builds
	@{$PlatHash{Blds}}=@{$BldLists{$PlatHash{OS}}};

# output some information
	if ($Mode{Verbose}) {
		print
			"Platform         $PlatHash{Name}\n",
			"Real Name        $PlatHash{Real}\n",
			"Compiler         $PlatHash{Compiler}\n",
			"OS               $PlatHash{OS}\n",
			"CPU              $PlatHash{CPU}\n",
			"ASSP             $PlatHash{ASSP}\n",
			"ABI              $PlatHash{ABI}\n",
			"ASSPABI          $PlatHash{ASSPABI}\n",
			"Makefile Module  $PlatHash{MakeMod}\n",
			"Makefile Type    $PlatHash{MakeCmd}\n",
			"MMP Macros       @{$PlatHash{MmpMacros}}\n",
			"Macros           @{$PlatHash{Macros}}\n",
			"Blds             @{$PlatHash{Blds}}\n"
		;
	}

	%{$PlatHash_ref}=%PlatHash;
	%{$BldMacrosHash_ref}=%BldMacros;
}

sub Plat_List () {

#	Include the list of BPABI platforms
	&Set_Plat;

#	return list of supported platforms

	#sort keys %Plat;
	my @PlatList;
	my $Key;
	foreach $Key (keys %Plat) {
		if (!$variantABIV2Keyword && $Key =~ /^armv5_abiv1$/i) {
			$Key = 'ARMV5';
		}
		unless (grep /^$Key$/i, @PlatList) {
			push @PlatList, $Key;
		}
	}
	return @PlatList
}

sub Plat_AssocIDE ($$) {
#	return the IDE associated with a "Real" platform if there is one
	my ($Candidate, $AssocIDEs)=@_;

	unless (defined $Plat{$Candidate}) {
		die "ERROR: Platform \"$Candidate\" not supported\n";
	}

	my $Key;
	foreach $Key (keys %Plat) {
		if (${Plat{$Key}}{Real}) {
			if (${Plat{$Key}}{Real} eq $Candidate) {
				push @$AssocIDEs, $Key;
			}
		}
	}
}

1;
