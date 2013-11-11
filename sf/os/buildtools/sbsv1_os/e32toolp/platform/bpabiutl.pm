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
# This package  does various ancillary things for BPABI (Base Platform ABI) platforms
# 
#

package BPABIutl;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	BPABIutl_Plat_List
	BPABIutl_Config_Path
	BPABIutl_Get_Config_Variables
);

use E32Variant;
require E32Plat;

my %ConfigPath;
my $variantABIV2Keyword = &Variant_GetMacro();

sub BPABIutl_Plat_List {

# Identify the BPABI platforms to be supported based on the compiler configuration files
# present in the location specified by the environment variable "SYMBIAN_COMPILATION_CONFIG_DIR"
# and in the directory $EPOCROOT\epoc32\tools\compilation_config

	my @CompilerConfigPath;

	if (exists($ENV{'SYMBIAN_COMPILATION_CONFIG_DIR'})) 
	{
		my $Path = $ENV{SYMBIAN_COMPILATION_CONFIG_DIR};
		@CompilerConfigPath = split(/;/, $Path);
	}

	push @CompilerConfigPath, "$ENV{EPOCROOT}epoc32\\tools\\compilation_config";

	my $ConfigDir;
	my @BPABIPlats = ();

	foreach $ConfigDir (@CompilerConfigPath)
	{
		opendir DIR, "$ConfigDir";
		my @Plats=grep /\.mk$/i, readdir DIR;
		my $Plat;
		foreach $Plat (@Plats) 
		{
# The platform name will be same as the name of the configuration file <config.mk>
# with the suffix '.mk' removed
			$Plat =~ s/\.mk//;
			if ($variantABIV2Keyword) {
				if ($Plat =~ /^armv5_abiv2$/i) {
					$Plat = "ARMV5";
				}
			}
			else {
				if ($Plat =~ /^armv5$/i) {
					$Plat = "ARMV5_ABIV2";
				}
			}
			unless (grep /$Plat$/i, @BPABIPlats) {
				$Plat = uc $Plat;
				push @BPABIPlats, $Plat;
				if (!$variantABIV2Keyword && $Plat =~ /^ARMV5_ABIV2$/i) {
					$ConfigPath{$Plat} = "$ConfigDir\\ARMV5.mk";
				}
				else {
					$ConfigPath{$Plat} = "$ConfigDir\\$Plat.mk";
				}
			}
		}
	}
	closedir DIR;
	return @BPABIPlats;
}

sub BPABIutl_Config_Path($) {
	my ($plat) = @_;
# Returns the Path of the compiler configuration file
	if (!keys(%ConfigPath))
	{
	    # Running for first time so initialise ConfigPath
	    BPABIutl_Plat_List();
	}
	if (!$ConfigPath{$plat})
	{
		# Get the root platform name to support hierarchy of customizations
		my $CustomizedPlat =  &E32Plat::Plat_Root($plat);

# In default ABIV1 mode, the platform name for v2 mode of ARMV5 is ARMV5_ABIV2
		if ( !$variantABIV2Keyword && $CustomizedPlat =~ /^ARMV5$/i ){
			$ConfigPath{$plat} = $ConfigPath{ARMV5_ABIV2};
		}

# In default ABIV2 mode, the platform name for v2 mode of ARMV5 is ARMV5
		elsif ( $variantABIV2Keyword && $CustomizedPlat =~ /^ARMV5$/i) {
			$ConfigPath{$plat} = $ConfigPath{ARMV5};
		}
		else{
			$ConfigPath{$plat}=$ConfigPath{$CustomizedPlat};
		}
	}
	return "$ConfigPath{$plat}";
}

# routine to extract a set of variables from the compilation configuration
# file. The variables extracted are determined by the extractvars.make file.
# The variables are returned in a hash.
sub BPABIutl_Get_Config_Variables
{
    my ($plat) = @_;
    my $configfile = BPABIutl_Config_Path($plat);
    my $extractvars = "$ENV{EPOCROOT}epoc32\\tools\\compilation_config\\extractvars.make";
    my $command = "make CONFIG_FILE=$configfile -s -f $extractvars" ;
    my $result = `$command`;
    my %vars;
    my @lines = split('\n', $result);
    foreach my $line (@lines)
    {
	chomp $line;
	if ($line =~ /(\w+)=(.*)/)
	{
	    $vars{$1} = $2;
	}
    }
    return %vars;
}

1;
