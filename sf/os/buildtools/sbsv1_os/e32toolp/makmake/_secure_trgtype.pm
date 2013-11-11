# Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# this package controls target types known to the build system
# To add new types to the system, simply add an entry to the %Types data structure.
# Look at the existing types for an appropriate example
# 
#

package Trgtype;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	Trg_GetL
	Trg_List
);

use strict;
use Genutl;

my %Types=(
	ANI=>{
		'Exports'=>{
			MARM=>['CreateCAnimDllL__Fv'],
			EABI=>['_Z15CreateCAnimDllLv'],
			WINS=>['?CreateCAnimDllL@@YAPAVCAnimDll@@XZ'],
			X86=>['?CreateCAnimDllL@@YAPAVCAnimDll@@XZ'],
			x86gcc=>['_Z15CreateCAnimDllLv']
		},
		UID2=>'0x10003b22',
	},
	APP=>{
		'Exports'=>{
			MARM=>['NewApplication__Fv'],
			EABI=>['_Z14NewApplicationv'],
			WINS=>['?NewApplication@@YAPAVCApaApplication@@XZ'],
			X86=>['?NewApplication@@YAPAVCApaApplication@@XZ'],
		},
		NeedUID3=>1,
		UID2=>'0x100039ce',
		Deprecated=>"Convert to EXE",
	},
	CTL=>{
		'Exports'=>{
			MARM=>['CreateControlL__FRC7TDesC16'],
			EABI=>['_Z14CreateControlLRK7TDesC16'], 
			WINS=>['?CreateControlL@@YAXABVTDesC16@@@Z'],
			X86=>['?CreateControlL@@YAXABVTDesC16@@@Z'],
		},
		UID2=>'0x10003a34',
		Deprecated=>"Convert to application",
	},
	DLL=>{
		NeedDeffile=>1,
	},	
	EPOCEXE=>{
		Basic=>'EXEDLL',
		'Exports'=>{
			WINS=>['?WinsMain@@YAHXZ'],
		},
	},
	EXE=>{
		Basic=>'EXE',
	},
	EXEDLL=>{
		Basic=>'EXEDLL',
		NeedDeffile=>1,
	},
	EXEXP=>{
		Basic=>'EXE',
		NeedDeffile=>1,
	},
	FSY=>{
		'Exports'=>{
			MARM=>['CreateFileSystem'],
			EABI=>['CreateFileSystem'],
			WINS=>['CreateFileSystem'],
			X86=>['CreateFileSystem'],
			x86gcc=>['CreateFileSystem']
		},
		UID2=>'0x100039df',
	},
	IMPLIB=>{
		NeedDeffile=>1,
		Basic=>'IMPLIB',
	},	
	KDLL=>{
		FirstLib=>'EKLL.LIB',
#		Kernel=>1,
		System=>1,
	},
	KEXT=>{
		FirstLib=>'EEXT.LIB',
#		Kernel=>1,
		System=>1,
	},
	KLIB=>{
		Basic=>'LIB',
#		Kernel=>1,
		System=>1,
	},
	LDD=>{
		FirstLib=>'EDEV.LIB',
#		Kernel=>1,
		System=>1,
		'Exports'=>{
			MARM=>['CreateLogicalDevice__Fv'],
			EABI=>['_Z19CreateLogicalDevicev'],
			WINS=>['?CreateLogicalDevice@@YAPAVDLogicalDevice@@XZ'],
			X86=>['?CreateLogicalDevice@@YAPAVDLogicalDevice@@XZ'],
			x86gcc=>['_Z19CreateLogicalDevicev']
		},
		UID2=>'0x100000af',
	},
	LIB=>{
		Basic=>'LIB',
	},
	ECOMIIC=>{
		'Exports'=>{
			MARM=>['ImplementationGroupProxy__FRi'],
			EABI=>['_Z24ImplementationGroupProxyRi'], 
			WINS=>['?ImplementationGroupProxy@@YAPBUTImplementationProxy@@AAH@Z'],
			X86=>['?ImplementationGroupProxy@@YAPBUTImplementationProxy@@AAH@Z'],
		},
		Path=>'System\Libs\Plugins',
		UID2=>'0x10009D8D',
		Deprecated=>"Convert to PLUGIN (ECOM)",
	},
	PLUGIN=>{
		'Exports'=>{
			MARM=>['ImplementationGroupProxy__FRi'],
			EABI=>['_Z24ImplementationGroupProxyRi'],
			WINS=>['?ImplementationGroupProxy@@YAPBUTImplementationProxy@@AAH@Z'],
			X86=>['?ImplementationGroupProxy@@YAPBUTImplementationProxy@@AAH@Z'],
			x86gcc=>['_Z24ImplementationGroupProxyRi']
		},
		ResourcePath=>'Resource\Plugins',
		UID2=>'0x10009D8D',
	},
	PLUGIN3=>{
		'Exports'=>{
			MARM=>['ImplementationGroupProxy__FRi'],
			EABI=>['_Z24ImplementationGroupProxyRi'],
			WINS=>['?ImplementationGroupProxy@@YAPBUTImplementationProxy3@@AAH@Z'],
			X86=>['?ImplementationGroupProxy@@YAPBUTImplementationProxy3@@AAH@Z'],
			x86gcc=>['_Z24ImplementationGroupProxyRi']
		},
		ResourcePath=>'Resource\Plugins',
		UID2=>'0x10009D93',
	},
	MDA=>{
		'Exports'=>{
			MARM=>['NewMediaLibraryL__Fv'],
			EABI=>['_Z16NewMediaLibraryLv'],
			WINS=>['?NewMediaLibraryL@@YAPAVCMdaLibrary@@XZ'],
			X86=>['?NewMediaLibraryL@@YAPAVCMdaLibrary@@XZ'],
		},
		UID2=>'0x1000393f',
		Deprecated=>"Convert to ???",
	},
	MDL=>{
		'Exports'=>{
			MARM=>['CreateRecognizer__Fv'],
			EABI=>['_Z16CreateRecognizerv'],
			WINS=>['?CreateRecognizer@@YAPAVCApaDataRecognizerType@@XZ'],
			X86=>['?CreateRecognizer@@YAPAVCApaDataRecognizerType@@XZ'],
		},
		UID2=>'0x10003a19',
		Deprecated=>"Convert to PLUGIN (ECOM)",
	},
	RDL=>{
		'Exports'=>{
			MARM=>['CreateRecognizer__Fv'],
			EABI=>['_Z16CreateRecognizerv'],
			WINS=>['?CreateRecognizer@@YAPAVCApaFileRecognizerType@@XZ'],
		},
		UID2=>'0x10003a37',
		Deprecated=>"Convert to PLUGIN (ECOM)",
	},
	NOTIFIER=>{
		'Exports'=>{
			MARM=>['NotifierArray__Fv'],
			EABI=>['_Z13NotifierArrayv'],
			WINS=>['?NotifierArray@@YAPAV?$CArrayPtr@VMEikSrvNotifierBase@@@@XZ'],
			X86=>['?NotifierArray@@YAPAV?$CArrayPtr@VMEikSrvNotifierBase@@@@XZ'],
		},
		Path=>'System\Notifiers',
		UID2=>'0x10005522',
		Deprecated=>"Convert to PLUGIN (ECOM)",
	},
	NOTIFIER2=>{
		'Exports'=>{
			MARM=>['NotifierArray__Fv'],
			EABI=>['_Z13NotifierArrayv'],
			WINS=>['?NotifierArray@@YAPAV?$CArrayPtr@VMEikSrvNotifierBase2@@@@XZ'],
			X86=>['?NotifierArray@@YAPAV?$CArrayPtr@VMEikSrvNotifierBase2@@@@XZ'],
		},
		Path=>'System\Notifiers',
		UID2=>'0x101fdfae',
		Deprecated=>"Convert to PLUGIN (ECOM)",
	},
	TEXTNOTIFIER2=>{
		'Exports'=>{
			MARM=>['NotifierArray__Fv'],
			EABI=>['_Z13NotifierArrayv'],
			WINS=>['?NotifierArray@@YAPAV?$CArrayPtr@VMNotifierBase2@@@@XZ'],
			X86=>['?NotifierArray@@YAPAV?$CArrayPtr@VMNotifierBase2@@@@XZ'],
			x86gcc=>['_Z13NotifierArrayv']
		},
		Path=>'System\Notifiers',
		UID2=>'0x101fe38b',
		# Not deprecated - the Text Window Server will have AllFiles, 
		# so it can continue scanning for binaries
	},
	PDD=>{
		FirstLib=>'EDEV.LIB',
#		Kernel=>1,
		System=>1,
		'Exports'=>{
			MARM=>['CreatePhysicalDevice__Fv'],
			EABI=>['_Z20CreatePhysicalDevicev'],
			WINS=>['?CreatePhysicalDevice@@YAPAVDPhysicalDevice@@XZ'],
			X86=>['?CreatePhysicalDevice@@YAPAVDPhysicalDevice@@XZ'],
			x86gcc=>['_Z20CreatePhysicalDevicev']
		},
		UID2=>'0x100039d0',
	},
	PDL=>{
		'Exports'=>{
			MARM=>['NewPrinterDeviceL__Fv'],
			EABI=>['_Z17NewPrinterDeviceLv'],
			WINS=>['?NewPrinterDeviceL@@YAPAVCPrinterDevice@@XZ'],
			X86=>['?NewPrinterDeviceL@@YAPAVCPrinterDevice@@XZ'],
			x86gcc=>['_Z17NewPrinterDeviceLv']
		},
		UID2=>'0x10003b1c',
		ResourcePath=>'Resource\Printers',
	},
	STDDLL=>{
		NeedDeffile=>1,
		UID2=>'0x20004C45',
	},
	STDEXE=>{
		Basic=>'EXE',
		UID2=>'0x20004C45',
	},
	STDLIB=>{
		Basic=>'LIB',
	},
	VAR=>{
		'Exports'=>{
			MARM=>['VariantInitialise__Fv'],
			EABI=>['_Z17VariantInitialisev'],
			X86=>['?VariantInitialise@@YAPAVAsic@@XZ'],
			x86gcc=>['_Z17VariantInitialisev']
		},
		FirstLib=>'EVAR.LIB',
#		Kernel=>1,
		System=>1,
	},
	VAR2=>{
		'Exports'=>{
			MARM=>['VariantInitialise'],
			EABI=>['VariantInitialise'],
			X86=>['VariantInitialise'],
			x86gcc=>['VariantInitialise']
		},
		FirstLib=>'EVAR.LIB',
		System=>1,
	},
	NONE=>{
		Basic=>'IMPLIB',
	},
);

sub Trg_GetL ($$$) {
#	takes target type, followed by a ref to a data structure
#	to fill with exports, second UID and default targetpath, etc.,
#
#	dies upon error

	my ($Candidate, $TrgHash_ref)=@_;
	$Candidate= uc $Candidate;

#	Is type in our list?
	unless (defined $Types{$Candidate}) {
		die "ERROR: Target type \"$Candidate\" not supported\n";
	}

#	Get the data
	my %TrgHash=%{$Types{$Candidate}};


#	Set the defaults
	$TrgHash{Name}=$Candidate;

	
	unless ($TrgHash{Basic}) {
		$TrgHash{Basic}='DLL';
	}

	unless ($TrgHash{FirstLib}) {
		$TrgHash{FirstLib}='';
	}

	unless ($TrgHash{Kernel}) {
		$TrgHash{Kernel}=0;
	}

	unless ($TrgHash{System}) {
		$TrgHash{System}=0;
	}

	unless ($TrgHash{Exports}{MARM}) {
		$TrgHash{Exports}{MARM}=[];
	}
	else {
		unless (@{$TrgHash{Exports}{MARM}}<=2) {
			die "INTERNAL ERROR: Too many MARM exports defined for type \"$TrgHash{Name}\" in Trgtype.pm\n";
		}
	}

	unless ($TrgHash{Exports}{EABI}) {
		$TrgHash{Exports}{EABI}=[];
	}
	else {
		unless (@{$TrgHash{Exports}{EABI}}<=2) {
			die "INTERNAL ERROR: Too many EABI exports defined for type \"$TrgHash{Name}\" in Trgtype.pm\n";
		}
	}

	unless ($TrgHash{Exports}{WINS}) {
		$TrgHash{Exports}{'WINS'}=[];
	}
	else {
		unless (@{$TrgHash{Exports}{WINS}}<=2) {
			die "INTERNAL ERROR: Too many WINS exports defined for type \"$TrgHash{Name}\" in Trgtype.pm\n";
		}
	}

	unless ($TrgHash{Exports}{X86}) {
		$TrgHash{Exports}{X86}=[];
	}
	else {
		unless (@{$TrgHash{Exports}{X86}}<=2) {
			die "INTERNAL ERROR: Too many X86 exports defined for type \"$TrgHash{Name}\" in Trgtype.pm\n";
		}
	}

	unless ($TrgHash{Exports}{x86gcc}) {
		$TrgHash{Exports}{x86gcc}=[];
	}
	else {
		unless (@{$TrgHash{Exports}{x86gcc}}<=2) {
			die "INTERNAL ERROR: Too many x86gcc exports defined for type \"$TrgHash{Name}\" in Trgtype.pm\n";
		}
	}


	unless ($TrgHash{NeedDeffile}) {
		$TrgHash{NeedDeffile}=0;
	}

	unless ($TrgHash{NeedUID3}) {
		$TrgHash{NeedUID3}=0;
	}

	unless ($TrgHash{Path}) {
		$TrgHash{Path}='';
	}
	else {
		# apply terminating backslash
		$TrgHash{Path}=~s-^(.*[^\\])$-$1\\-o;
		# apply Z drive
		$TrgHash{Path}="Z\\$TrgHash{Path}";
	}

	unless ($TrgHash{ResourcePath}) {
		$TrgHash{ResourcePath}='';
	}
	else {
		# apply terminating backslash & Z drive
		$TrgHash{ResourcePath}=~s-^(.*[^\\])$-Z\\$1\\-o;
	}

	unless ($TrgHash{UID2}) {
		$TrgHash{UID2}='';
	}
	else {
		$TrgHash{UID2}=&Genutl_AnyToHex($TrgHash{UID2});
		unless (defined $TrgHash{UID2}) {
			die "INTERNAL ERROR: UID2 badly defined for type \"$TrgHash{Name}\" in Trgtype.pm\n";
		}
	}

#	Deprecated target warnings

	if ($TrgHash{Deprecated}) {
		print "MIGRATION_NOTE: type \"$TrgHash{Name}\" is deprecated - $TrgHash{Deprecated}\n";
	}
	
#	Pass the data
	%{$TrgHash_ref}=%TrgHash;
}

sub Trg_List () {
#	returns a list of known poly types

	sort keys %Types;
}


1;
