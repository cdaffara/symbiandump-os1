rem Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem @echo off
rem 
rem


@if not "%1" == "" goto GotDrive
:Syntax
@echo syntax : CopyDataToCF CF_drive_letter
@goto end

:GotDrive

mkdir %1\unittest\
mkdir %1\unittest\mmf\

mkdir %1\inttest\
mkdir %1\inttest\mmf\
mkdir %1\rta\

REM ################################# START RTA ########################################

mkdir %1\rta\
copy %EPOCROOT%epoc32\data\c\rta\*.*		%1\rta\

REM ################################# END RTA #########################################


REM ################################# START ACLNT DATA ##################################
mkdir %1\unittest\mmf\aclnt\
copy %EPOCROOT%epoc32\data\c\r3.wav                      		%1\unittest\r3.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\aclnt\*.*			%1\unittest\mmf\aclnt\

REM SYMBIAN_CAF_V2
copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst.wav		%1\unittest\mmf\aclnt\mainTst.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst2.wav		%1\unittest\mmf\aclnt\mainTst2.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst3.wav		%1\unittest\mmf\aclnt\mainTst3.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\Audio.content		%1\unittest\mmf\aclnt\Audio.content
copy %EPOCROOT%epoc32\data\c\AclntITestData\Audio.contentrights	%1\unittest\mmf\aclnt\Audio.contentrights
copy %EPOCROOT%epoc32\data\c\AclntITestData\AudioContentNoRights.content	%1\unittest\mmf\aclnt\AudioContentNoRights.content

REM ################################# END ACLNT DATA ##################################

REM ################################# START BASECL DATA ###############################

mkdir %1\unittest\mmf\basecl\
copy %EPOCROOT%epoc32\data\c\Reference100HzTone*.refsnd %1\unittest\mmf\basecl\

REM ################################# END BASECL DATA #################################



REM ################################# START ACOD DATA ##################################
REM Copy files to root of CF card.

REM MMF_ACOD codec unit test source files
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\PCM16Source.raw		%1\PCM16Source.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\PCM16BSource.raw		%1\PCM16BSource.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\PCMU16Source.raw		%1\PCMU16Source.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\PCMU16BSource.raw		%1\PCMU16BSource.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\StereoPCM16Source.raw	%1\StereoPCM16Source.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\PCM8Source.raw		%1\PCM8Source.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\PCMU8Source.raw		%1\PCMU8Source.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\AlawSource.raw		%1\AlawSource.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\MulawSource.raw		%1\MulawSource.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\AdpcmSource.raw		%1\AdpcmSource.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\StereoAdpcmSource.raw	%1\StereoAdpcmSource.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\GSM610Source.raw		%1\GSM610Source.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\acod\AAAASource.raw		%1\AAAASource.raw

REM ACOD codec unit test comparison files
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\AlawToPCM16Comparison.raw		%1\AlawToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToAlawComparison.raw		%1\PCM16ToAlawComparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM8ToPCM16Comparison.raw		%1\PCM8ToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToPCM8Comparison.raw		%1\PCM16ToPCM8Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToPCM16BComparison.raw	%1\PCM16ToPCM16BComparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16BToPCM16Comparison.raw	%1\PCM16BToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCMU16ToPCMU16BComparison.raw	%1\PCMU16ToPCMU16BComparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCMU16BToPCMU16Comparison.raw	%1\PCMU16BToPCMU16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToPCMU16BComparison.raw	%1\PCM16ToPCMU16BComparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCMU16BToPCM16Comparison.raw	%1\PCMU16BToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToPCMU16Comparison.raw	%1\PCM16ToPCMU16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCMU16ToPCM16Comparison.raw	%1\PCMU16ToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToPCMU8Comparison.raw		%1\PCM16ToPCMU8Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCMU8ToPCM16Comparison.raw		%1\PCMU8ToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\AdpcmToPCM16Comparison.raw		%1\AdpcmToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToAdpcmComparison.raw		%1\PCM16ToAdpcmComparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\MulawToPCM16Comparison.raw		%1\MulawToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToMulawComparison.raw		%1\PCM16ToMulawComparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\StereoAdpcmToPCM16Comparison.raw	%1\StereoAdpcmToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToStereoAdpcmComparison.raw	%1\PCM16ToStereoAdpcmComparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\GSM610ToPCM16Comparison.raw	%1\GSM610ToPCM16Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\PCM16ToGSM610Comparison.raw	%1\PCM16ToGSM610Comparison.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\ref\acod\AAAABBBBPrefSuppComparison.raw	%1\AAAABBBBPrefSuppComparison.raw
REM ################################# END ACOD DATA ##################################


REM ################################# START ACTRL DATA ##################################
mkdir %1\unittest\mmf\actrl\
mkdir %1\unittest\mmf\actrl\Reference
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\newmail.wav		%1\unittest\mmf\actrl\newmail.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\newmail.au		%1\unittest\mmf\actrl\newmail.au
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\16bitmpcm.wav	%1\unittest\mmf\actrl\16bitmpcm.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\11KHzADPCMMonoWAVTo8KHzPCM16Mono.raw	%1\unittest\mmf\actrl\Reference\11KHzADPCMMonoWAVTo8KHzPCM16Mono.raw	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\11KHzPCM16MonoRAWTo8KHzGSM610Mono.wav	%1\unittest\mmf\actrl\Reference\11KHzPCM16MonoRAWTo8KHzGSM610Mono.wav	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\11KHzPCM16StereoWAVTo8KHzMuLAWMono.wav	%1\unittest\mmf\actrl\Reference\11KHzPCM16StereoWAVTo8KHzMuLAWMono.wav	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\22KHzALAWMonoWAVTo8KHzPCM16Mono.wav		%1\unittest\mmf\actrl\Reference\22KHzALAWMonoWAVTo8KHzPCM16Mono.wav	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\22KHzALAWStereoRAWTo11KHzPCM16Mono.raw	%1\unittest\mmf\actrl\Reference\22KHzALAWStereoRAWTo11KHzPCM16Mono.raw	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\22KHzPCM16MonoAUTo8KhzPCM16Mono.wav		%1\unittest\mmf\actrl\Reference\22KHzPCM16MonoAUTo8KhzPCM16Mono.wav	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\22KHzPCM8MonoWAVTo11KHzPCM16Stereo.wav	%1\unittest\mmf\actrl\Reference\22KHzPCM8MonoWAVTo11KHzPCM16Stereo.wav	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\44KHzPCM16StereoAUTo22KHzPCM16Mono.raw	%1\unittest\mmf\actrl\Reference\44KHzPCM16StereoAUTo22KHzPCM16Mono.raw	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\44KHzPCM16StereoRAWTo8KHzPCM16Stereo.au	%1\unittest\mmf\actrl\Reference\44KHzPCM16StereoRAWTo8KHzPCM16Stereo.au
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\44KHzPCM16StereoWAVTo8KHzPCM16Mono.au	%1\unittest\mmf\actrl\Reference\44KHzPCM16StereoWAVTo8KHzPCM16Mono.au	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\8KHzGSM610MonoRAWTo8KHzPCM16Mono.wav	%1\unittest\mmf\actrl\Reference\8KHzGSM610MonoRAWTo8KHzPCM16Mono.wav	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\8KHzPCM16MonoAUTo8KHzPCM16Mono.au		%1\unittest\mmf\actrl\Reference\8KHzPCM16MonoAUTo8KHzPCM16Mono.au		
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\8KHzPCM16StereoWAVTo8KHzGSM610Mono.wav	%1\unittest\mmf\actrl\Reference\8KHzPCM16StereoWAVTo8KHzGSM610Mono.wav	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\11KHzPCMU8MonoAUTo8KHzPCM16Mono.wav		%1\unittest\mmf\actrl\Reference\11KHzPCMU8MonoAUTo8KHzPCM16Mono.wav	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\11KHzPCM16StereoWAVTo11KHzADPCMMono.wav 	%1\unittest\mmf\actrl\Reference\11KHzPCM16StereoWAVTo11KHzADPCMMono.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\11KHzPCM16StereoWAVTo11KHzADPCMMono.raw 	%1\unittest\mmf\actrl\Reference\11KHzPCM16StereoWAVTo11KHzADPCMMono.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\NoReferenceFile				%1\unittest\mmf\actrl\Reference\NoReferenceFile	
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\11KHzADPCMMono1KHzTone.wav			%1\unittest\mmf\actrl\11KHzADPCMMono1KHzTone.wav					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\11KHzPCM16Mono1KHzTone.raw			%1\unittest\mmf\actrl\11KHzPCM16Mono1KHzTone.raw					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\11KHzPCM16Stereo1KHzTone.wav		%1\unittest\mmf\actrl\11KHzPCM16Stereo1KHzTone.wav					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\22KHzALawMono1KHzTone.wav			%1\unittest\mmf\actrl\22KHzALawMono1KHzTone.wav					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\22KHzALAWStereo1KHzTone.raw			%1\unittest\mmf\actrl\22KHzALAWStereo1KHzTone.raw					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\22KHzPCM16Mono1KHzTone.au			%1\unittest\mmf\actrl\22KHzPCM16Mono1KHzTone.au					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\22KHzPCM16Stereo1KHzTone.wav		%1\unittest\mmf\actrl\22KHzPCM16Stereo1KHzTone.wav					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\22KHzPCM8Mono1KHzTone.wav			%1\unittest\mmf\actrl\22KHzPCM8Mono1KHzTone.wav					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\44KHzPCM16Stereo1KHzTone.au			%1\unittest\mmf\actrl\44KHzPCM16Stereo1KHzTone.au					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\44KHzPCM16Stereo1KHzTone.raw		%1\unittest\mmf\actrl\44KHzPCM16Stereo1KHzTone.raw					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\44KHzPCM16Stereo1KHzTone.wav		%1\unittest\mmf\actrl\44KHzPCM16Stereo1KHzTone.wav					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\8KHzGSM610Mono1KHzTone.raw			%1\unittest\mmf\actrl\8KHzGSM610Mono1KHzTone.raw					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\8KHzGSM610Mono1KHzTone.wav			%1\unittest\mmf\actrl\8KHzGSM610Mono1KHzTone.wav					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\8KHzPCM16Mono1KHzTone.au			%1\unittest\mmf\actrl\8KHzPCM16Mono1KHzTone.au					
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\8KHzPCM16Stereo1KHzTone.wav			%1\unittest\mmf\actrl\8KHzPCM16Stereo1KHzTone.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\RampVolume.wav				%1\unittest\mmf\actrl\RampVolume.wav							
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\actrl\mainTst.wav					%1\unittest\mmf\actrl\mainTst.wav
REM ################################# END ACTRL DATA ##################################


REM ################################# START AFMT DATA ##################################
mkdir %1\unittest\mmf\afmt\
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\afmt\*.*	%1\unittest\mmf\afmt\*.*
REM ################################# END AFMT DATA ##################################


REM ################################# START SRSSNK DATA ##################################
mkdir %1\unittest\mmf\srssnk\
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\srssnk\MMFTestFile1.dat		%1\unittest\mmf\srssnk\MMFTestFile1.dat
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\srssnk\MMFTestFile2.dat		%1\unittest\mmf\srssnk\MMFTestFile2.dat
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\srssnk\MMFTestFile3.dat		%1\unittest\mmf\srssnk\TestFile3.dat
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\srssnk\MMFTestFile1.wav		%1\unittest\mmf\srssnk\MMFTestFile1.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\srssnk\MMFTestFile2.wav		%1\unittest\mmf\srssnk\MMFTestFile2.wav
REM ################################# END SRSSNK DATA ##################################


REM ################################# START VCLNT DATA ##################################
mkdir %1\unittest\mmf\vclnt\
mkdir %1\unittest\mmf\vclnt\mm\
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\vclnt\small2.wav		%1\unittest\mmf\vclnt\mm\small2.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\vclnt\smallv.wav		%1\unittest\mmf\vclnt\mm\smallv.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\vclnt\clock.dummy		%1\unittest\mmf\vclnt\mm\clock.dummy
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\vclnt\LongVideo.dummy	%1\unittest\mmf\vclnt\mm\LongVideo.dummy
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\vclnt\ShortVideo.dummy 	%1\unittest\mmf\vclnt\mm\ShortVideo.dummy
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\vclnt\AudVid.avi		%1\unittest\mmf\vclnt\mm\AudVid.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\vclnt\Vid.avi		%1\unittest\mmf\vclnt\mm\Vid.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\vclnt\xvidonly.avi	%1\unittest\mmf\vclnt\mm\xvidonly.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\vclnt\clock2.dummy	%1\unittest\mmf\vclnt\mm\clock2.dummy
REM ################################# END VCLNT DATA ##################################


REM ################################ START SWCDWRAPPER ###################################
mkdir %1\unittest\mmf\swcdwrap\
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\swcdwrap\*.*	%1\unittest\mmf\swcdwrap\*.*
REM ################################ START SWCDWRAPPER ###################################

REM ################################ START SWCDDEVICES ###################################
mkdir %1\unittest\mmf\swcodecdevices\
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\swcodecdevices\*.*	%1\unittest\mmf\swcodecdevices\*.*
REM ################################ START SWCDDEVICES ###################################

@if not "%2" == "icl" goto NoICL

mkdir %1\unittest\icl\
mkdir %1\inttest\icl\

REM ################################# START PFM DATA ##################################
mkdir %1\unittest\icl\TSU_ICL_PFM_02\
copy %EPOCROOT%epoc32\data\c\TSU_ICL_PFM_02\*.*		%1\unittest\icl\TSU_ICL_PFM_02\*.*
REM ################################# START PFM DATA ##################################

REM ################################# START COD DATA ##################################
mkdir %1\unittest\icl\TSU_ICL_COD_03\
mkdir %1\unittest\icl\TSU_ICL_COD_03\WMF\
mkdir %1\unittest\icl\TSU_ICL_COD_03\WMFRef\
mkdir %1\unittest\icl\TSU_ICL_COD_03\WMFRefMask\
mkdir %1\unittest\icl\TSU_ICL_COD_03\WMFArmRef\
mkdir %1\unittest\icl\TSU_ICL_COD_03\WMFArmRefMask\

copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_03\*.*			%1\unittest\icl\TSU_ICL_COD_03\*.*
copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_03\WMF\*.*		%1\unittest\icl\TSU_ICL_COD_03\WMF\*.*
copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_03\WMFRef\*.*		%1\unittest\icl\TSU_ICL_COD_03\WMFRef\*.*
copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_03\WMFRefMask\*.*	%1\unittest\icl\TSU_ICL_COD_03\WMFRefMask\*.*
copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_03\WMFArmRef\*.*	%1\unittest\icl\TSU_ICL_COD_03\WMFArmRef\*.*
copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_03\WMFArmRefMask\*.*	%1\unittest\icl\TSU_ICL_COD_03\WMFArmRefMask\*.*

mkdir %1\unittest\icl\TSU_ICL_COD_02\
copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_02\*.* 		%1\unittest\icl\TSU_ICL_COD_02\*.*
copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_02\wirelessbitmap.wbmp %1\unittest\icl\TSU_ICL_COD_02\wirelessbitmap.wbm
mkdir %1\unittest\icl\TSU_ICL_COD_04\
copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_04\*.*		%1\unittest\icl\TSU_ICL_COD_04\*.*
mkdir %1\unittest\icl\TSU_ICL_COD_04b\
copy %EPOCROOT%epoc32\data\c\TSU_ICL_COD_04b\*.*		%1\unittest\icl\TSU_ICL_COD_04b\*.*

mkdir %1\unittest\icl\TImage\
mkdir %1\unittest\icl\TImage\anon\
mkdir %1\unittest\icl\TImage\prof\
mkdir %1\unittest\icl\TImage\ref\
mkdir %1\unittest\icl\TImage\00d4\
mkdir %1\unittest\icl\TImage\00d6\
mkdir %1\unittest\icl\TImage\bmp\

copy %EPOCROOT%epoc32\data\c\TImage\*.*       			%1\unittest\icl\TImage\.
copy %EPOCROOT%epoc32\data\c\TImage\Anon\*.*     		%1\unittest\icl\TImage\Anon\.
copy %EPOCROOT%epoc32\data\c\TImage\prof\*.*     		%1\unittest\icl\TImage\prof\.
copy %EPOCROOT%epoc32\data\c\TImage\Ref\*.*    			%1\unittest\icl\TImage\Ref\.
copy %EPOCROOT%epoc32\data\c\TImage\00d4\*.*     		%1\unittest\icl\TImage\00d4\.
copy %EPOCROOT%epoc32\data\c\TImage\00d6\*.*     		%1\unittest\icl\TImage\00d6\.
copy %EPOCROOT%epoc32\data\c\TImage\bmp\*.*     		%1\unittest\icl\TImage\bmp\.

mkdir %1\unittest\icl\validation\
mkdir %1\unittest\icl\validation\png

copy %EPOCROOT%epoc32\data\c\ICL\Validation\png\00000022.png    %1\unittest\icl\validation\png\00000022.png
copy %EPOCROOT%epoc32\data\c\ICL\Validation\png\00000055.png    %1\unittest\icl\validation\png\00000055.png
copy %EPOCROOT%epoc32\data\c\ICL\Validation\png\00000064.png    %1\unittest\icl\validation\png\00000064.png
copy %EPOCROOT%epoc32\data\c\ICL\Validation\png\00000316.png    %1\unittest\icl\validation\png\00000316.png

REM ################################# END COD DATA ##################################

REM ################################# tsu_icl_imgdisp  ##################################
mkdir %1\tsu_icl_imgdisp_01\
mkdir %1\tsu_icl_imgdisp_01\ref\
copy %EPOCROOT%epoc32\data\c\tsu_icl_imgdisp_01\dummy.idp       	%1\tsu_icl_imgdisp_01\dummy.idp
copy %EPOCROOT%epoc32\data\c\tsu_icl_imgdisp_01\wolf.jpg       		%1\tsu_icl_imgdisp_01\wolf.jpg
copy %EPOCROOT%epoc32\data\c\tsu_icl_imgdisp_01\empty.mng 		%1\tsu_icl_imgdisp_01\empty.mng
copy %EPOCROOT%epoc32\data\c\tsu_icl_imgdisp_01\simple_b.mng 		%1\tsu_icl_imgdisp_01\simple_b.mng
copy %EPOCROOT%epoc32\data\c\tsu_icl_imgdisp_01\Ref\frame_1.mbm 	%1\tsu_icl_imgdisp_01\ref\frame_1.mbm
copy %EPOCROOT%epoc32\data\c\tsu_icl_imgdisp_01\Ref\frame_2.mbm 	%1\tsu_icl_imgdisp_01\ref\frame_2.mbm
REM ################################# END OF THE tsu_icl_imgdisp  ##################################

REM ################################# tsu_icl_Mngimgdisp_01  ##################################
mkdir %1\tsu_icl_Mngimgdisp_01\
mkdir %1\tsu_icl_Mngimgdisp_01\ref\

copy %EPOCROOT%epoc32\data\c\tsu_icl_Mngimgdisp_01\*.*			%1\tsu_icl_Mngimgdisp_01\*.*
copy %EPOCROOT%epoc32\data\c\tsu_icl_Mngimgdisp_01\ref\*.*		%1\tsu_icl_Mngimgdisp_01\ref\*.*
REM ################################# END OF THE tsu_icl_Mngimgdisp_01  ##################################

REM ################################# tsu_icl_Exifimgdisp_01  ##################################
mkdir %1\tsu_icl_Exifimgdisp_01\
mkdir %1\tsu_icl_Exifimgdisp_01\ref\

copy %EPOCROOT%epoc32\data\c\tsu_icl_Exifimgdisp_01\*.*			%1\tsu_icl_Exifimgdisp_01\*.*
copy %EPOCROOT%epoc32\data\c\tsu_icl_Exifimgdisp_01\ref\*.*		%1\tsu_icl_Exifimgdisp_01\ref\*.*
REM ############################### END OF THE tsu_icl_Exifimgdisp_01  ##########################

REM ################################# tsu_icl_GenIclImgDisp_01 ##################################
mkdir %1\tsu_icl_Exifimgdisp_01\genIcl\
copy %EPOCROOT%epoc32\data\c\tsu_icl_Exifimgdisp_01\genicl\AnmatCat.gif	%1\tsu_icl_Exifimgdisp_01\genicl\AnmatCat.gif
copy %EPOCROOT%epoc32\data\c\tsu_icl_Exifimgdisp_01\genicl\clock.gif	%1\tsu_icl_Exifimgdisp_01\genicl\clock.gif
copy %EPOCROOT%epoc32\data\c\tsu_icl_Exifimgdisp_01\genicl\png2.png	%1\tsu_icl_Exifimgdisp_01\genicl\png2.png

copy %EPOCROOT%epoc32\data\c\tsu_icl_Exifimgdisp_01\ref\*.*		%1\tsu_icl_Exifimgdisp_01\ref\*.*
REM ################################# end of the tsu_icl_GenIclImgDisp_01  #######################

REM ################################# START FRM DATA ##################################
mkdir %1\unittest\icl\frm\
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\Tif2.tif		%1\unittest\icl\frm\Tif2.tif
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\Tif2.mbm		%1\unittest\icl\frm\Tif2.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\Trans_1.gif		%1\unittest\icl\frm\Trans_1.gif
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\Trans_1.mbm		%1\unittest\icl\frm\Trans_1.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\Gif87a.gif		%1\unittest\icl\frm\Gif87a.gif
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\Gif87a.mbm		%1\unittest\icl\frm\Gif87a.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\Test.tst		%1\unittest\icl\frm\Test.tst
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\Png24bpp_1.png	%1\unittest\icl\frm\Png24bpp_1.png

REM SYMBIAN_CAF_V2
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\jfif444.jpg		%1\unittest\icl\frm\jfif444.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\jfif444.mbm		%1\unittest\icl\frm\jfif444.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\trans_8.gif		%1\unittest\icl\frm\trans_8.gif
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\trans_8.mbm		%1\unittest\icl\frm\trans_8.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\png4_8.png		%1\unittest\icl\frm\png4_8.png
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\png4_8.mbm		%1\unittest\icl\frm\png4_8.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\ImageContentNoRights.content	%1\unittest\icl\frm\ImageContentNoRights.content
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\operator.otb		%1\unittest\icl\frm\operator.otb
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\operator.mbm		%1\unittest\icl\frm\operator.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\video.mp4		%1\unittest\icl\frm\video.mp4
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\simple_b.mng	%1\unittest\icl\frm\simple_b.mng
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\jfif422_4_ref.JPG	%1\unittest\icl\frm\jfif422_4_ref.JPG
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\clock.gif		%1\unittest\icl\frm\clock.gif
copy %EPOCROOT%epoc32\data\c\TSU_ICL_FRM_01\decodertest.jpg		%1\unittest\icl\frm\decodertest.jpg

REM ################################# END FRM DATA ##################################

REM ################################# te_tsi_icl_cod_05 #######################

mkdir %1\te_tsi_icl_cod_05\bmp
mkdir %1\te_tsi_icl_cod_05\bmp\ref
mkdir %1\te_tsi_icl_cod_05\gif
mkdir %1\te_tsi_icl_cod_05\gif\ref
mkdir %1\te_tsi_icl_cod_05\ico
mkdir %1\te_tsi_icl_cod_05\ico\ref
mkdir %1\te_tsi_icl_cod_05\jpeg
mkdir %1\te_tsi_icl_cod_05\jpeg\ref
mkdir %1\te_tsi_icl_cod_05\mbm
mkdir %1\te_tsi_icl_cod_05\mbm\ref
mkdir %1\te_tsi_icl_cod_05\mng
mkdir %1\te_tsi_icl_cod_05\mng\ref
mkdir %1\te_tsi_icl_cod_05\ota
mkdir %1\te_tsi_icl_cod_05\ota\ref
mkdir %1\te_tsi_icl_cod_05\png
mkdir %1\te_tsi_icl_cod_05\png\ref
mkdir %1\te_tsi_icl_cod_05\tif
mkdir %1\te_tsi_icl_cod_05\tif\ref
mkdir %1\te_tsi_icl_cod_05\wbmp
mkdir %1\te_tsi_icl_cod_05\wbmp\ref
mkdir %1\te_tsi_icl_cod_05\wmf
mkdir %1\te_tsi_icl_cod_05\wmf\ref

copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\bmp\*.*		%1\te_tsi_icl_cod_05\bmp\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\bmp\ref\*.*		%1\te_tsi_icl_cod_05\bmp\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\gif\*.*		%1\te_tsi_icl_cod_05\gif\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\gif\ref\*.*		%1\te_tsi_icl_cod_05\gif\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\ico\*.*		%1\te_tsi_icl_cod_05\ico\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\ico\ref\*.*		%1\te_tsi_icl_cod_05\ico\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\jpeg\*.*		%1\te_tsi_icl_cod_05\jpeg\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\jpeg\ref\*.*		%1\te_tsi_icl_cod_05\jpeg\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\mbm\*.*		%1\te_tsi_icl_cod_05\mbm\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\mbm\ref\*.*		%1\te_tsi_icl_cod_05\mbm\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\mng\*.*		%1\te_tsi_icl_cod_05\mng\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\mng\ref\*.*		%1\te_tsi_icl_cod_05\mng\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\ota\*.*		%1\te_tsi_icl_cod_05\ota\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\ota\ref\*.*		%1\te_tsi_icl_cod_05\ota\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\png\*.*		%1\te_tsi_icl_cod_05\png\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\png\ref\*.*		%1\te_tsi_icl_cod_05\png\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\tif\*.*		%1\te_tsi_icl_cod_05\tif\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\tif\ref\*.*		%1\te_tsi_icl_cod_05\tif\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\wbmp\*.*		%1\te_tsi_icl_cod_05\wbmp\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\wbmp\ref\*.*		%1\te_tsi_icl_cod_05\wbmp\ref\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\wmf\*.*		%1\te_tsi_icl_cod_05\wmf\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_cod_05\wmf\ref\*.*		%1\te_tsi_icl_cod_05\wmf\ref\*.*

REM ################################# end te_tsi_icl_cod_05 #######################

mkdir %1\te_tsi_icl_dirscan_decode_01
mkdir %1\te_tsi_icl_dirscan_decode_01\jpg
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_dirscan_decode_01\*.*		%1\te_tsi_icl_dirscan_decode_01\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_dirscan_decode_01\jpg\*.*		%1\te_tsi_icl_dirscan_decode_01\jpg\*.*

mkdir %1\te_tsi_icl_excludedfiles_01
mkdir %1\te_tsi_icl_excludedfiles_01\inputimages
mkdir %1\te_tsi_icl_excludedfiles_01\refimages

copy %EPOCROOT%epoc32\data\c\te_tsi_icl_excludedfiles_01\*.*		%1\te_tsi_icl_excludedfiles_01\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_excludedfiles_01\inputimages\*.*	%1\te_tsi_icl_excludedfiles_01\inputimages\*.*
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_excludedfiles_01\refimages\*.*		%1\te_tsi_icl_excludedfiles_01\refimages\*.*

REM ################################# START te_tsi_icl_pluginsupport_01 ###########################

mkdir %1\te_tsi_icl_pluginsupport_01
copy %EPOCROOT%epoc32\data\c\te_tsi_icl_pluginsupport_01\*.*		%1\te_tsi_icl_pluginsupport_01\*.*

REM ################################# END te_tsi_icl_pluginsupport_01 ###########################

REM ################################# START MEDIASVR DATA ##################################
mkdir %1\unittest\icl\mediasvr\
mkdir %1\unittest\icl\mediasvr\stress\
copy %EPOCROOT%epoc32\data\c\MediaSvrTest\*.* 			%1\unittest\icl\mediasvr\*.*
copy %EPOCROOT%epoc32\data\c\MediaSvrTest\stress\*.*		%1\unittest\icl\mediasvr\stress\*.*
REM ################################# END MEDIASVR DATA ##################################


REM ################################# START TMDAVID DATA ##################################
mkdir %1\unittest\icl\tmdavid\
mkdir %1\unittest\icl\tmdavid\Anon
mkdir %1\unittest\icl\tmdavid\Ref

copy %EPOCROOT%epoc32\data\c\tmdavid\*.*			%1\unittest\icl\tmdavid\*.*
copy %EPOCROOT%epoc32\data\c\tmdavid\Anon\*.*			%1\unittest\icl\tmdavid\Anon\*.*
copy %EPOCROOT%epoc32\data\c\tmdavid\Ref\*.*			%1\unittest\icl\tmdavid\Ref\*.*
REM ################################# END TMDAVID DATA ##################################



REM #######################################################################################
REM ############################# START INTEGRATION TESTS #################################
REM #######################################################################################

REM ################################# START BTRANS DATA ################################
mkdir %1\unittest\icl\Btrans\
copy %EPOCROOT%epoc32\data\c\Btrans\bmp8bpp_8.bmp        %1\unittest\icl\Btrans\bmp8bpp_8.bmp
copy %EPOCROOT%epoc32\data\c\Btrans\320x240_c24.mbm        %1\unittest\icl\Btrans\320x240_c24.mbm
copy %EPOCROOT%epoc32\data\c\Btrans\imagevid_8.mbm       %1\unittest\icl\Btrans\imagevid_8.mbm
copy %EPOCROOT%epoc32\data\c\Btrans\imagevid_8Ref.mbm    %1\unittest\icl\Btrans\imagevid_8Ref.mbm
copy %EPOCROOT%epoc32\data\c\Btrans\test.mbm    		%1\unittest\icl\Btrans\test.mbm


mkdir %1\inttest\icl\TImage\
copy %EPOCROOT%epoc32\data\c\TImage\bmp8bpp_8.bmp        %1\inttest\icl\TImage\bmp8bpp_8.bmp
copy %EPOCROOT%epoc32\data\c\TImage\imagevid_8.mbm       %1\inttest\icl\TImage\imagevid_8.mbm
copy %EPOCROOT%epoc32\data\c\TImage\qvga_24bpp.mbm       %1\inttest\icl\TImage\qvga_24bpp.mbm

copy %EPOCROOT%epoc32\data\c\EColor16IclBtrans.mbm       %1\unittest\icl\EColor16IclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\EColor16MIclBtrans.mbm      %1\unittest\icl\EColor16MIclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\EColor256IclBtrans.mbm      %1\unittest\icl\EColor256IclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\EColor4KIclBtrans.mbm       %1\unittest\icl\EColor4KIclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\EColor64kIclBtrans.mbm      %1\unittest\icl\EColor64kIclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\EGray16IclBtrans.mbm        %1\unittest\icl\EGray16IclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\EGray256IclBtrans.mbm       %1\unittest\icl\EGray256IclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\EGray2IclBtrans.mbm         %1\unittest\icl\EGray2IclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\EGray4IclBtrans.mbm         %1\unittest\icl\EGray4IclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\EColor16MUIclBtrans.mbm     %1\unittest\icl\EColor16MUIclBtrans.mbm
copy %EPOCROOT%epoc32\data\c\xfile.mbm 			 %1\unittest\icl\xfile.mbm

copy %EPOCROOT%epoc32\data\c\Btrans\*.*			%1\unittest\icl\btrans\*.*

REM ################################# END BTRANS DATA ##################################



REM ################################# START GSCAL DATA ################################
mkdir %1\unittest\icl\TSU_ICL_GSCAL_01\
copy %EPOCROOT%epoc32\data\c\TSU_ICL_GSCAL_01\*.*        	%1\unittest\icl\TSU_ICL_GSCAL_01\*.*
REM ################################# END GSCAL DATA ##################################

REM ################################# START ITFM DATA ################################
mkdir %1\unittest\icl\TSU_ICL_ITFM_01\
mkdir %1\unittest\icl\TSU_ICL_ITFM_01\ref
mkdir %1\unittest\icl\TSU_ICL_ITFM_01\output
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\mbmProg.mbm		%1\unittest\icl\TSU_ICL_ITFM_01\mbmProg.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\png1.png			%1\unittest\icl\TSU_ICL_ITFM_01\png1.png
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\test.txt			%1\unittest\icl\TSU_ICL_ITFM_01\test.txt
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\TooShort.txt		%1\unittest\icl\TSU_ICL_ITFM_01\TooShort.txt
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\0thIFDTag_0000.jpg		%1\unittest\icl\TSU_ICL_ITFM_01\0thIFDTag_0000.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\0thIFDTag_7FFF.jpg		%1\unittest\icl\TSU_ICL_ITFM_01\0thIFDTag_7FFF.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\0thIFDTag_FFFF.jpg		%1\unittest\icl\TSU_ICL_ITFM_01\0thIFDTag_FFFF.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\IllegalExifImage_Leave1.jpg		%1\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Leave1.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\IllegalExifImage_Leave2.jpg		%1\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Leave2.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\IllegalExifImage_Leave3.jpg		%1\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Leave3.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\IllegalExifImage_Panic1.jpg		%1\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Panic1.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\IllegalExifImage_Panic2.jpg		%1\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Panic2.jpg

copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\mbmProg_100x100_m.mbm	%1\unittest\icl\TSU_ICL_ITFM_01\ref\mbmProg_100x100_m.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\mbmProg_160x90.mbm	%1\unittest\icl\TSU_ICL_ITFM_01\ref\mbmProg_160x90.mbm
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\png1_100x100_m.png	%1\unittest\icl\TSU_ICL_ITFM_01\ref\png1_100x100_m.png
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\0thIFDTag_0000_100x100_m.jpg	%1\unittest\icl\TSU_ICL_ITFM_01\ref\0thIFDTag_0000_100x100_m.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\0thIFDTag_7FFF_100x100_m.jpg	%1\unittest\icl\TSU_ICL_ITFM_01\ref\0thIFDTag_7FFF_100x100_m.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\0thIFDTag_FFFF_100x100_m.jpg	%1\unittest\icl\TSU_ICL_ITFM_01\ref\0thIFDTag_FFFF_100x100_m.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave1_100x100_m.jpg	%1\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave1_100x100_m.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave2_100x100_m.jpg	%1\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave2_100x100_m.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave3_100x100_m.jpg	%1\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave3_100x100_m.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\IllegalExifImage_Panic1_100x100_m.jpg	%1\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Panic1_100x100_m.jpg
copy %EPOCROOT%epoc32\data\c\TSU_ICL_ITFM_01\ref\IllegalExifImage_Panic2_100x100_m.jpg	%1\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Panic2_100x100_m.jpg

REM ################################# END ITFM DATA ##################################



REM ################################# START FRM DATA ################################
mkdir %1\inttest\icl\TestImages\
mkdir %1\inttest\icl\RefImages\

copy %EPOCROOT%epoc32\data\c\ICL\TestImages\*.*		%1\inttest\ICL\TestImages\*.*
copy %EPOCROOT%epoc32\data\c\ICL\RefImages\*.*		%1\inttest\ICL\RefImages\*.*


REM ################################# Image Encoder test data start ################################

mkdir %1\inttest\icl\testimages\mbm
copy %EPOCROOT%epoc32\data\c\ICL\TestImages\mbm\*.*	%1\inttest\ICL\TestImages\mbm\*.*

REM ################################# Image Encoder test data end ################################

REM ################################# Image Decoder test data start ################################

mkdir %1\inttest\icl\testimages\jpeg
copy %EPOCROOT%epoc32\data\c\icl\testimages\jpeg\*.*     %1\inttest\icl\testimages\jpeg\*.*

REM ################################# Image Decoder test data end ################################

REM SYMBIAN_CAF_V2
REM ################################# END FRM DATA ################################

REM ################################# START ICL COD DATA ################################
mkdir %1\inttest\ICL\Validation\

mkdir %1\inttest\ICL\Validation\bmp
copy %EPOCROOT%epoc32\data\c\ICL\Validation\bmp\*.* %1\inttest\ICL\Validation\bmp\*.*

mkdir %1\inttest\ICL\Validation\png
copy %EPOCROOT%epoc32\data\c\ICL\Validation\png\*.* %1\inttest\ICL\Validation\png\*.*

mkdir %1\inttest\ICL\Validation\jpg
copy %EPOCROOT%epoc32\data\c\ICL\Validation\jpg\*.* %1\inttest\ICL\Validation\jpg\*.*
REM ################################# END ICL COD DATA ################################

REM ########## PREQ1920 - START Image Transform inttest and image processor unittest DATA  ###############################

mkdir %1\inttest\icl\testdata\images
xcopy /E /I /Y %EPOCROOT%epoc32\data\c\testdata\images %1\inttest\icl\testdata\images
xcopy /E /I /Y %EPOCROOT%epoc32\data\c\testdata\testref %1\inttest\icl\testdata\testref

REM ################################ PREQ1920 - START Image Processor Inttest DATA ################################

mkdir %1\inttest\icl\testdata\caps\input
xcopy /E /I /Y %EPOCROOT%epoc32\data\c\testdata\caps\input %1\inttest\icl\testdata\caps\input
		
REM ################################# PREQ1920 - END Image Processor Inttest DATA #################################

REM ################################ PREQ1920 - START Image Panorama Inttest DATA ################################
mkdir %1\inttest\icl\testdata\images\panorama	
xcopy /E /I /Y %EPOCROOT%epoc32\data\c\testdata\images\panorama  %1\inttest\icl\testdata\images\panorama
xcopy /E /I /Y %EPOCROOT%epoc32\data\c\testdata\testref %1\inttest\icl\testdata\testref
		
REM ################################# PREQ1920 - END Image Panorama Inttest DATA #################################

:NoICL

REM ################################# START ACLNT DATA ##################################
mkdir %1\inttest\MMF\AclntITestData\

copy %EPOCROOT%epoc32\data\c\AclntITestData\16bitmPcm.wav 	%1\inttest\MMF\AclntITestData\16bitmPcm.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\8bitmAlaw.wav 	%1\inttest\MMF\AclntITestData\8bitmAlaw.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\8bitmPcm.wav 	%1\inttest\MMF\AclntITestData\8bitmPcm.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\8bitmPcm-6144bytes.wav %1\inttest\MMF\AclntITestData\8bitmPcm-6144bytes.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\8bitmuLaw.wav 	%1\inttest\MMF\AclntITestData\8bitmuLaw.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\8bitmUPCM.raw 	%1\inttest\MMF\AclntITestData\8bitmUPCM.raw
copy %EPOCROOT%epoc32\data\c\AclntITestData\Gsm610.wav 		%1\inttest\MMF\AclntITestData\Gsm610.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\IMAD.wav 		%1\inttest\MMF\AclntITestData\IMAD.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\IMAS.wav 		%1\inttest\MMF\AclntITestData\IMAS.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\PcmU16BE.wav 	%1\inttest\MMF\AclntITestData\PcmU16BE.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\PcmU16.wav 		%1\inttest\MMF\AclntITestData\PcmU16.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst.wav 	%1\inttest\MMF\AclntITestData\mainTst.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst2.wav 	%1\inttest\MMF\AclntITestData\mainTst2.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\small.wav 		%1\inttest\MMF\AclntITestData\small.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\small.au 		%1\inttest\MMF\AclntITestData\small.au
copy %EPOCROOT%epoc32\data\c\AclntITestData\test.wav 		%1\inttest\MMF\AclntITestData\test.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\test16m2.au 	%1\inttest\MMF\AclntITestData\test16m2.au
copy %EPOCROOT%epoc32\data\c\AclntITestData\test16m2.wav 	%1\inttest\MMF\AclntITestData\test16m2.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\test2.wav 		%1\inttest\MMF\AclntITestData\test2.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\unsupported.wav 	%1\inttest\MMF\AclntITestData\unsupported.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\alf-noproblem.wav   %1\inttest\MMF\AclntITestData\alf-noproblem.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\100khzSRate1KhzTone.wav %1\inttest\MMF\AclntITestData\100khzSRate1KhzTone.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\7.33khzSRate1KhzTone.wav %1\inttest\MMF\AclntITestData\7.33khzSRate1KhzTone.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\OneSample.wav 	%1\inttest\MMF\AclntITestData\OneSample.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\sequence1.sqn 	%1\inttest\MMF\AclntITestData\sequence1.sqn
copy %EPOCROOT%epoc32\data\c\AclntITestData\sequence2.sqn 	%1\inttest\MMF\AclntITestData\sequence2.sqn
copy %EPOCROOT%epoc32\data\c\AclntITestData\16bitmPcm.xxx 	%1\inttest\MMF\AclntITestData\16bitmPcm.xxx
copy %EPOCROOT%epoc32\data\c\AclntITestData\explo.wav 		%1\inttest\MMF\AclntITestData\explo.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\imad2.wav 		%1\inttest\MMF\AclntITestData\imad2.wav

copy %EPOCROOT%epoc32\data\c\AclntITestData\8bitmuPcm.wav 	%1\inttest\MMF\AclntITestData\8bitmuPcm.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst2.wav 	%1\inttest\MMF\AclntITestData\mainTst2.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\main.wav 		%1\inttest\MMF\AclntITestData\main.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\test.raw 		%1\inttest\MMF\AclntITestData\test.raw
copy %EPOCROOT%epoc32\data\c\AclntITestData\pcm8mono44khz.wav 	%1\inttest\MMF\AclntITestData\pcm8mono44khz.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\pcm16mono44khz.wav 	%1\inttest\MMF\AclntITestData\pcm16mono44khz.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\pcm16mono48khz.wav 	%1\inttest\MMF\AclntITestData\pcm16mono48khz.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\pcm16mono12khz.wav 	%1\inttest\MMF\AclntITestData\pcm16mono12khz.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\pcm16mono24khz.wav 	%1\inttest\MMF\AclntITestData\pcm16mono24khz.wav

copy %EPOCROOT%epoc32\data\c\AclntITestData\a.txt 	%1\inttest\MMF\AclntITestData\a.txt
copy %EPOCROOT%epoc32\data\c\AclntITestData\b.txt 	%1\inttest\MMF\AclntITestData\b.txt
copy %EPOCROOT%epoc32\data\c\AclntITestData\g.txt 	%1\inttest\MMF\AclntITestData\g.txt
copy %EPOCROOT%epoc32\data\c\AclntITestData\gif.txt 	%1\inttest\MMF\AclntITestData\gif.txt
copy %EPOCROOT%epoc32\data\c\AclntITestData\i.txt	%1\inttest\MMF\AclntITestData\i.txt
copy %EPOCROOT%epoc32\data\c\AclntITestData\m.txt 	%1\inttest\MMF\AclntITestData\m.txt
copy %EPOCROOT%epoc32\data\c\AclntITestData\r.txt 	%1\inttest\MMF\AclntITestData\r.txt
copy %EPOCROOT%epoc32\data\c\AclntITestData\test.abc 	%1\inttest\MMF\AclntITestData\test.abc
copy %EPOCROOT%epoc32\data\c\AclntITestData\test.xyz 	%1\inttest\MMF\AclntITestData\test.xyz

copy %EPOCROOT%epoc32\data\c\AclntITestData\recog_wav.wav %1\inttest\MMF\AclntITestData\recog_wav.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\recog_wav.au  %1\inttest\MMF\AclntITestData\recog_wav.au
copy %EPOCROOT%epoc32\data\c\AclntITestData\recog_wav 	  %1\inttest\MMF\AclntITestData\recog_wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\recog_au.au   %1\inttest\MMF\AclntITestData\recog_au.au
copy %EPOCROOT%epoc32\data\c\AclntITestData\recog_au.wav  %1\inttest\MMF\AclntITestData\recog_au.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\recog_raw.wav %1\inttest\MMF\AclntITestData\recog_raw.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\drmwavfile.drm %1\inttest\MMF\AclntITestData\drmwavfile.drm

copy %EPOCROOT%epoc32\data\c\AclntITestData\PCM8bitMono.wav %1\inttest\MMF\AclntITestData\PCM8bitMono.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\PCM8bitStereo.wav %1\inttest\MMF\AclntITestData\PCM8bitStereo.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\PCM8bitMono.au %1\inttest\MMF\AclntITestData\PCM8bitMono.au
copy %EPOCROOT%epoc32\data\c\AclntITestData\PCM8bitStereo.au %1\inttest\MMF\AclntITestData\PCM8bitStereo.au
copy %EPOCROOT%epoc32\data\c\AclntITestData\PCM16bitMono.wav %1\inttest\MMF\AclntITestData\PCM16bitMono.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\PCM16bitStereo.wav %1\inttest\MMF\AclntITestData\PCM16bitStereo.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\PCM16bitMono.au %1\inttest\MMF\AclntITestData\PCM16bitMono.au
copy %EPOCROOT%epoc32\data\c\AclntITestData\PCM16bitStereo.au %1\inttest\MMF\AclntITestData\PCM16bitStereo.au

copy %EPOCROOT%epoc32\data\c\AclntITestData\textInput.wav 	%1\inttest\MMF\AclntITestData\textInput.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\textOutput.wav 	%1\inttest\MMF\AclntITestData\textOutput.wav

copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst3.wav 	%1\inttest\MMF\AclntITestData\mainTst3.wav
copy %EPOCROOT%epoc32\data\c\AclntITestData\test2.raw 		%1\inttest\MMF\AclntITestData\test2.raw
copy %EPOCROOT%epoc32\data\c\AclntITestData\test3.raw 		%1\inttest\MMF\AclntITestData\test3.raw

copy %EPOCROOT%epoc32\data\c\audio.ini	     		  %1\inttest\MMF\AclntITestData\audio.ini
copy %EPOCROOT%epoc32\data\c\sdevsound_audio.ini	     		  %1\inttest\MMF\AclntITestData\sdevsound_audio.ini
copy %EPOCROOT%epoc32\data\c\AclntITestData\Audio.content		%1\inttest\mmf\aclnt\Audio.content
copy %EPOCROOT%epoc32\data\c\AclntITestData\Audio.contentrights	%1\inttest\mmf\aclnt\Audio.contentrights
copy %EPOCROOT%epoc32\data\c\AclntITestData\AudioContentNoRights.content	%1\inttest\mmf\aclnt\AudioContentNoRights.content

REM SYMBIAN_CAF_V2

REM ################################# END ACLNT DATA ##################################

REM ################################# START ACLNT OGG DATA ##################################
copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst.ogg %1\inttest\MMF\AclntITestData\mainTst.ogg
copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst2.ogg %1\inttest\MMF\AclntITestData\mainTst2.ogg
copy %EPOCROOT%epoc32\data\c\AclntITestData\mainTst3.ogg %1\inttest\MMF\AclntITestData\mainTst3.ogg
copy %EPOCROOT%epoc32\data\c\AclntITestData\small.ogg %1\inttest\MMF\AclntITestData\small.ogg
copy %EPOCROOT%epoc32\data\c\AclntITestData\AudioOgg.content %1\inttest\MMF\AclntITestData\AudioOgg.content
copy %EPOCROOT%epoc32\data\c\AclntITestData\AudioOgg.contentrights %1\inttest\MMF\AclntITestData\AudioOgg.contentrights

REM ################################# END ACLNT OGG DATA ##################################

REM ################################ START VCLNT DATA #################################
mkdir %1\inttest\MMF\VclntITestData\
copy %EPOCROOT%epoc32\data\c\VclntITestData\LongVideo.dummy 	%1\inttest\MMF\VclntITestData\LongVideo.dummy
copy %EPOCROOT%epoc32\data\c\VclntITestData\ShortVideo.dummy 	%1\inttest\MMF\VclntITestData\ShortVideo.dummy
copy %EPOCROOT%epoc32\data\c\VclntITestData\DRMVideo.drm 	%1\inttest\MMF\VclntITestData\DRMVideo.drm

copy %EPOCROOT%epoc32\data\c\VclntITestData\textInput.dummy 	%1\inttest\MMF\VclntITestData\textInput.dummy
copy %EPOCROOT%epoc32\data\c\VclntITestData\textOutput.dummy 	%1\inttest\MMF\VclntITestData\textOutput.dummy
REM ################################# END VCLNT DATA ##################################


REM ################################ START VCLNT DATA #################################
mkdir %1\inttest\MMF\TsiMmfCtlfrmData\
copy %EPOCROOT%epoc32\data\c\TsiMmfCtlfrmData\test.wav 	%1\inttest\MMF\TsiMmfCtlfrmData\test.wav
copy %EPOCROOT%epoc32\data\c\TsiMmfCtlfrmData\test2.wav %1\inttest\MMF\TsiMmfCtlfrmData\test2.wav
REM ################################# END VCLNT DATA ##################################

REM ################################ START MIDICLNT DATA #################################
mkdir %1\MidiClntITestData\
copy %EPOCROOT%epoc32\data\c\MidiClntITestData\Midi.mid %1\MidiClntITestData\Midi.mid
copy %EPOCROOT%epoc32\data\c\MidiClntITestData\textmidi.mid %1\MidiClntITestData\textmidi.mid
REM ################################# END MIDICLNT DATA ##################################


REM ################################ START DEVSOUND DATA #################################
mkdir %1\TSU_MMF_DEVSOUND_SUITE\
mkdir %1\TSU_MMF_DEVSOUND_SUITE\input\
mkdir %1\TSU_MMF_DEVSOUND_SUITE\output\
copy %EPOCROOT%epoc32\data\c\TSU_MMF_DEVSOUND_SUITE\Input\*.* %1\TSU_MMF_DEVSOUND_SUITE\input\
REM ################################# END DEVSOUND DATA ##################################

REM ################################ START DEVSOUND DATA #################################
mkdir %1\inttest\mmf\tsi_mmf_devsound\
mkdir %1\inttest\mmf\tsi_mmf_devsound\output\
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devsound\small.ogg		%1\inttest\mmf\tsi_mmf_devsound\small.ogg
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devsound\output\small.ogg		%1\inttest\mmf\tsi_mmf_devsound\output\small.ogg
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devsound\corrupt.ogg	%1\inttest\mmf\tsi_mmf_devsound\corrupt.ogg
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devsound\output\corrupt.ogg	%1\inttest\mmf\tsi_mmf_devsound\output\corrupt.ogg
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devsound\big.ogg		%1\inttest\mmf\tsi_mmf_devsound\big.ogg
REM ################################# END DEVSOUND DATA ##################################


REM ##### START A3F DEVSOUND DATA #################################################################
mkdir %1\inttest\a3f\tsi_a3f_devsound\
mkdir %1\inttest\a3f\tsi_a3f_devsound\output\
copy %EPOCROOT%epoc32\data\c\tsi_a3f_devsound\*.* %1\inttest\a3f\tsi_a3f_devsound\

mkdir %1\inttest\a3f\tsi_a3f_devsound_aac\
copy %EPOCROOT%epoc32\data\c\tsi_a3f_devsound_aac\*.* %1\inttest\a3f\tsi_a3f_devsound_aac\
REM ##### END A3F DEVSOUND DATA ###################################################################

REM ################################ START DEVVIDEO DATA #################################
mkdir %1\inttest\mmf\tsi_mmf_devvideo\

copy %EPOCROOT%epoc32\data\c\tsi_mmf_devvideo\xvid_decframe.mp4v 	%1\inttest\mmf\tsi_mmf_devvideo\xvid_decframe.mp4v
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devvideo\xvid_encframe.mbm 	%1\inttest\mmf\tsi_mmf_devvideo\xvid_encframe.mbm
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devvideo\xvid_encframe.bin 	%1\inttest\mmf\tsi_mmf_devvideo\xvid_encframe.bin
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devvideo\test_encframe.bin 	%1\inttest\mmf\tsi_mmf_devvideo\test_encframe.bin
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devvideo\pcm_08_16_m.raw 		%1\inttest\mmf\tsi_mmf_devvideo\pcm_08_16_m.raw
copy %EPOCROOT%epoc32\data\c\tsi_mmf_devvideo\decodervideotest.in 	%1\inttest\mmf\tsi_mmf_devvideo\decodervideotest.in
REM ################################# END DEVVIDEO DATA ##################################

REM ################################ START SDEVSOUND DATA #################################
mkdir %1\inttest\MMF\sdevsoundinttestdata\
copy %EPOCROOT%epoc32\data\c\sdevsoundinttestdata\PCM_08_16_m.raw 	%1\inttest\MMF\sdevsoundinttestdata\PCM_08_16_m.raw
copy %EPOCROOT%epoc32\data\c\sdevsoundinttestdata\8bitmPcm.wav 		%1\inttest\MMF\sdevsoundinttestdata\8bitmPcm.wav
copy %EPOCROOT%epoc32\data\c\sdevsoundinttestdata\mainTst.wav		%1\inttest\MMF\sdevsoundinttestdata\mainTst.wav
REM ################################# END DEVSOUND DATA ##################################

REM ################################ START SBCCODEC DATA #################################
mkdir %1\unittest\mmf\sbc\ref\
copy %EPOCROOT%epoc32\data\c\mm\mmf\testFiles\sbc\*.wav 	%1\.
copy %EPOCROOT%epoc32\data\c\mm\mmf\Ref\sbc\*.sbc 		%1\unittest\mmf\sbc\ref\.
REM ################################# END SBCCODEC DATA ##################################

REM ################################ START MDF DATA ######################################
mkdir %1\unittest\mdf\audio
mkdir %1\unittest\mdf\video
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\mdf\depeche1.wav	%1\unittest\mdf\audio\depeche1.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\mdf\decodervideotest.in	%1\unittest\mdf\video\decodervideotest.in
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\mdf\earthrot.gif %1\unittest\mdf\video\earthrot.gif
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\mdf\clock.gif %1\unittest\mdf\video\clock.gif
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\mdf\depeche2.wav	%1\unittest\mdf\audio\depeche2.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\mdf\ogg-encode-in.wav	%1\unittest\mdf\audio\ogg-encode-in.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\mdf\ogg-decode-in.ogg	%1\unittest\mdf\audio\ogg-decode-in.ogg
REM ################################# END MDF DATA #######################################


REM ################################ COPY TEXT EDITOR INSTALL ############################
copy %EPOCROOT%epoc32\release\arm4\urel\editor.sis %1\
REM ################################ END TEXT EDITOR INSTALL ############################





REM ########################### OPTIONAL SUB SCRIPTS GO LAST ##############################

REM ################################# START OGGCTRL DATA ##################################
mkdir %1\unittest\mmf\oggctrl\
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\oggctrl\6Sec48KHz224KbsStereo.ogg		%1\unittest\mmf\oggctrl\6Sec48KHz224KbsStereo.ogg
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\oggctrl\6sec32khz120kbsstereo.ogg		%1\unittest\mmf\oggctrl\6sec32khz120kbsstereo.ogg
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\oggctrl\6sec44khz128kbsstereo.ogg	%1\unittest\mmf\oggctrl\6sec44khz128kbsstereo.ogg
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\oggctrl\invalid.ogg 	%1\unittest\mmf\oggctrl\invalid.ogg
REM ################################ END OGGCTRL DATA ############################

REM ################################# START MVSAGENTS DATA ##################################
mkdir %1\inttest\mvs\

copy %EPOCROOT%epoc32\data\c\mvs\wavtest01.wav		%1\inttest\mvs\wavtest01.wav
copy %EPOCROOT%epoc32\data\c\mvs\wavtest02.wav		%1\inttest\mvs\wavtest02.wav
copy %EPOCROOT%epoc32\data\c\mvs\textfile.ogg		%1\inttest\mvs\textfile.ogg
copy %EPOCROOT%epoc32\data\c\mvs\oggtest01.ogg		%1\inttest\mvs\oggtest01.ogg
copy %EPOCROOT%epoc32\data\c\mvs\mp3file.ogg		%1\inttest\mvs\mp3file.ogg
copy %EPOCROOT%epoc32\data\c\mvs\aud_vid.avi 			%1\inttest\mvs\aud_vid.avi
copy %EPOCROOT%epoc32\data\c\mvs\xvidonly.avi			%1\inttest\mvs\xvidonly.avi
copy %EPOCROOT%epoc32\data\c\mvs\xvid_rec.avi 			%1\inttest\mvs\xvid_rec.avi
copy %EPOCROOT%epoc32\data\c\mvs\corrupt.avi 			%1\inttest\mvs\corrupt.avi
copy %EPOCROOT%epoc32\data\c\mvs\invalidcodecavi.avi 		%1\inttest\mvs\invalidcodecavi.avi
copy %EPOCROOT%epoc32\data\c\mvs\textfile.avi			%1\inttest\mvs\textfile.avi
copy %EPOCROOT%epoc32\data\c\mvs\aud_vid2.avi			%1\inttest\mvs\aud_vid2.avi
copy %EPOCROOT%epoc32\data\c\mvs\pcm16stereo8khz.sjau		%1\inttest\mvs\pcm16stereo8khz.sjau
copy %EPOCROOT%epoc32\data\c\mvs\aud_vid_Vprp.avi 			%1\inttest\mvs\aud_vid_Vprp.avi
copy %EPOCROOT%epoc32\data\c\mvs\aud_vid_vprp2.avi 			%1\inttest\mvs\aud_vid_vprp2.avi

REM ################################ END MVSAGENTS DATA ############################

REM ################################# START VCLNTAVI DATA ##################################
mkdir %1\inttest\mmf\vclntavi\

copy %EPOCROOT%epoc32\data\c\vclntavi\longvideo.avi     %1\inttest\mmf\vclntavi\longvideo.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\shortvideo.avi    %1\inttest\mmf\vclntavi\shortvideo.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\textinput.avi     %1\inttest\mmf\vclntavi\textinput.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\textoutput.avi    %1\inttest\mmf\vclntavi\textoutput.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\scbtest           %1\inttest\mmf\vclntavi\scbtest
copy %EPOCROOT%epoc32\data\c\vclntavi\xvidonly.avi      %1\inttest\mmf\vclntavi\xvidonly.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\corrupt.avi       %1\inttest\mmf\vclntavi\corrupt.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\invalidcodecavi.avi   %1\inttest\mmf\vclntavi\invalidcodecavi.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\xvid_rec.avi          %1\inttest\mmf\vclntavi\xvid_rec.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\aud_vid.avi           %1\inttest\mmf\vclntavi\aud_vid.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\aud_vid_vprp.avi      %1\inttest\mmf\vclntavi\aud_vid_vprp.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\video.dummy           %1\inttest\mmf\vclntavi\video.dummy
copy %EPOCROOT%epoc32\data\c\vclntavi\xvid_subtitle_long.avi	%1\inttest\mmf\vclntavi\xvid_subtitle_long.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\xvid_subtitle_long.srt    %1\inttest\mmf\vclntavi\xvid_subtitle_long.srt
copy %EPOCROOT%epoc32\data\c\vclntavi\xvid_subtitle_corrupt.avi %1\inttest\mmf\vclntavi\xvid_subtitle_corrupt.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\xvid_subtitle_corrupt.srt %1\inttest\mmf\vclntavi\xvid_subtitle_corrupt.srt
copy %EPOCROOT%epoc32\data\c\vclntavi\xvid_subtitle.avi		%1\inttest\mmf\vclntavi\xvid_subtitle.avi
copy %EPOCROOT%epoc32\data\c\vclntavi\xvid_subtitle.srt		%1\inttest\mmf\vclntavi\xvid_subtitle.srt
REM ################################ END VCLNTAVI DATA ############################

REM ################################ START SECUREDRM DATA #####################################
mkdir %1\inttest\mmf\securedrm\
mkdir %1\unittest\mmf\securedrm\
copy %EPOCROOT%epoc32\data\c\securedrm\small.wav               %1\unittest\mmf\securedrm\small.wav
copy %EPOCROOT%epoc32\data\c\securedrm\audiocontent2.xml    %1\inttest\mmf\securedrm\audiocontent2.xml
copy %EPOCROOT%epoc32\data\c\securedrm\audiorights2.xml    %1\inttest\mmf\securedrm\audiorights2.xml
copy %EPOCROOT%epoc32\data\c\securedrm\videocontent2.xml    %1\inttest\mmf\securedrm\videocontent2.xml
copy %EPOCROOT%epoc32\data\c\securedrm\videorights2.xml    %1\inttest\mmf\securedrm\videorights2.xml
copy %EPOCROOT%epoc32\data\c\securedrm\small.wav    %1\inttest\mmf\securedrm\small.wav
copy %EPOCROOT%epoc32\data\c\securedrm\xvidonly.avi    %1\inttest\mmf\securedrm\xvidonly.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\aclnt\small.wav  %1\unittest\mmf\aclnt\small.wav
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\aclnt\smallwav_audiocontent.xml   %1\unittest\mmf\aclnt\smallwav_audiocontent.xml
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\aclnt\smallwav_audiorights.xml    %1\unittest\mmf\aclnt\smallwav_audiorights.xml
REM ################################### END SECUREDRM DATA ####################################

REM ################################# START AVICTRL DATA ##################################
mkdir %1\unittest\mmf\avictrl\

copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\avictrl\video_only.avi	%1\unittest\mmf\avictrl\video_only.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\avictrl\aud_vid.avi		%1\unittest\mmf\avictrl\aud_vid.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\avictrl\invalid.avi		%1\unittest\mmf\avictrl\invalid.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\avictrl\low_mem_vdo_test.avi 	%1\unittest\mmf\avictrl\low_mem_vdo_test.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\avictrl\aud_vid_vprp.avi		%1\unittest\mmf\avictrl\aud_vid_vprp.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\avictrl\srt_available.avi		%1\unittest\mmf\avictrl\srt_available.avi
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\avictrl\srt_available.srt		%1\unittest\mmf\avictrl\srt_available.srt
REM ################################ END AVICTRL DATA ############################


REM ################################# START UNSUPPORTED FORMATS RECOGNISER DATA ##################################
mkdir %1\recog
mkdir %1\recog\common

copy %EPOCROOT%epoc32\data\c\Recog\*.*		%1\recog\
copy %EPOCROOT%epoc32\data\c\Recog\Common\*.*	%1\recog\common\
REM ################################ END UNSUPPORTED FORMATS RECOGNISER DATA ############################

REM ################################# START MMF CHARACTERISATION DATA ##################################
mkdir %1\char\
mkdir %1\char\mmf\
mkdir %1\char\mmf\data\
copy %EPOCROOT%epoc32\data\c\char\mmf\data\*.*		%1\char\mmf\data\*.*
REM ################################ END MMF CHARACTERISATION DATA ############################

REM ################################# START A3FDEVSOUND DATA ########################################
mkdir %1\char_a3f_devsound_test_data\
copy %EPOCROOT%epoc32\data\c\char_a3f_devsound_test_data\cerrodelasilla.raw	%1\char_a3f_devsound_test_data\cerrodelasilla.raw
copy %EPOCROOT%epoc32\data\c\char_a3f_devsound_test_data\testing123.raw		%1\char_a3f_devsound_test_data\testing123.raw
copy %EPOCROOT%epoc32\data\c\char_a3f_devsound_test_data\sample.aud		%1\char_a3f_devsound_test_data\sample.aud
copy %EPOCROOT%epoc32\data\c\char_a3f_devsound_test_data\recordc.raw		%1\char_a3f_devsound_test_data\recordc.raw
REM ################################# END A3FDEVSOUND DATA #########################################

REM ################################# START DEVSOUND TRUEPAUSE DATA ########################################
mkdir %1\mm\mmf\testfiles\truepause_devsound_test_data\
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\cerrodelasilla.raw	%1\mm\mmf\testfiles\truepause_devsound_test_data\cerrodelasilla.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\maintst3.ogg		%1\mm\mmf\testfiles\truepause_devsound_test_data\maintst3.ogg
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\recordc.raw		%1\mm\mmf\testfiles\truepause_devsound_test_data\recordc.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\sample.aud		%1\mm\mmf\testfiles\truepause_devsound_test_data\sample.aud
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\testing123.raw	%1\mm\mmf\testfiles\truepause_devsound_test_data\testing123.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\record_1.raw 	%1\mm\mmf\testfiles\truepause_devsound_test_data\record_1.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\record_2.raw 	%1\mm\mmf\testfiles\truepause_devsound_test_data\record_2.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\record_3.ogg 	%1\mm\mmf\testfiles\truepause_devsound_test_data\record_3.ogg
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\record_4.raw 	%1\mm\mmf\testfiles\truepause_devsound_test_data\record_4.raw
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\truepause_devsound_test_data\record_5.raw 	%1\mm\mmf\testfiles\truepause_devsound_test_data\record_5.raw
REM #################################  END DEVSOUND TRUEPAUSE DATA  #########################################

REM ################################# START HWDEV TRUEPAUSE DATA ########################################
mkdir %1\mm\mdf\truepause\
copy %EPOCROOT%epoc32\data\c\mm\mdf\truepause\testing123.raw	%1\mm\mdf\truepause\testing123.raw
copy %EPOCROOT%epoc32\data\c\mm\mdf\truepause\testingOut1.raw	%1\mm\mdf\truepause\testingOut1.raw
copy %EPOCROOT%epoc32\data\c\mm\mdf\truepause\testingOut2.raw	%1\mm\mdf\truepause\testingOut2.raw
copy %EPOCROOT%epoc32\data\c\mm\mdf\truepause\testingOut3.raw	%1\mm\mdf\truepause\testingOut3.raw
REM #################################  END HWDEV TRUEPAUSE DATA  #########################################


REM ################################ DevSubtitle data ##########################

mkdir %1\unittest\
mkdir %1\unittest\mmf\
mkdir %1\unittest\mmf\subtitlegraphic
mkdir %1\unittest\mmf\devsubtitle
mkdir %1\unittest\mmf\subtitle

copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\devsubtitle\devsubtitle.srt %1\unittest\mmf\devsubtitle\devsubtitle.srt
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\devsubtitle\subtitletestdata.png %1\unittest\mmf\devsubtitle\subtitletestdata.png
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\devsubtitle\subtitletestdata2.png %1\unittest\mmf\devsubtitle\subtitletestdata2.png
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\subtitlegraphic\subtitletestdata.png %1\unittest\mmf\subtitlegraphic\subtitletestdata.png
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\subtitlegraphic\subtitletestdata2.png %1\unittest\mmf\subtitlegraphic\subtitletestdata2.png
copy %EPOCROOT%epoc32\data\c\mm\mmf\testfiles\subtitlegraphic\subtitletestdata3.png %1\unittest\mmf\subtitlegraphic\subtitletestdata3.png

copy %EPOCROOT%epoc32\data\c\mm\subtitle1.srt %1\unittest\mmf\subtitle\subtitle1.srt
copy %EPOCROOT%epoc32\data\c\mm\subtitle2.srt %1\unittest\mmf\subtitle\subtitle2.srt
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp0.mbm %1\unittest\mmf\subtitle\subtitlebmp0.mbm
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp1.mbm %1\unittest\mmf\subtitle\subtitlebmp1.mbm
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp2.mbm %1\unittest\mmf\subtitle\subtitlebmp2.mbm
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp3.mbm %1\unittest\mmf\subtitle\subtitlebmp3.mbm
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp4.mbm %1\unittest\mmf\subtitle\subtitlebmp4.mbm
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp5.mbm %1\unittest\mmf\subtitle\subtitlebmp5.mbm
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp6.mbm %1\unittest\mmf\subtitle\subtitlebmp6.mbm
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp7.mbm %1\unittest\mmf\subtitle\subtitlebmp7.mbm
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp8.mbm %1\unittest\mmf\subtitle\subtitlebmp8.mbm
copy %EPOCROOT%epoc32\data\c\mm\subtitlebmp9.mbm %1\unittest\mmf\subtitle\subtitlebmp9.mbm


REM ################################ End DevSubtitle data ######################

REM ################################# START 3GP LIBRARY DATA ##################################

mkdir %1\3gplibrary\
mkdir %1\3gplibrary\format\
mkdir %1\3gplibrary\format\invalid\
mkdir %1\3gplibrary\format\invalid\3gp\
mkdir %1\3gplibrary\format\invalid\mp4\
mkdir %1\3gplibrary\format\invalid\avc\
mkdir %1\3gplibrary\format\valid\
mkdir %1\3gplibrary\format\valid\misc\
mkdir %1\3gplibrary\format\valid\3gp\
mkdir %1\3gplibrary\format\valid\mp4\
mkdir %1\3gplibrary\format\valid\3g2\
mkdir %1\3gplibrary\interop\
mkdir %1\3gplibrary\performance\
mkdir %1\3gplibrary\performance\compose\
mkdir %1\3gplibrary\performance\parse\


copy %EPOCROOT%epoc32\data\c\3gplibrary\*.*		                                            	%1\3gplibrary\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\format\*.*												%1\3gplibrary\format\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\format\invalid\3gp\*.*		                            %1\3gplibrary\format\invalid\3gp\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\format\invalid\mp4\*.*		                            %1\3gplibrary\format\invalid\mp4\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\format\invalid\avc\*.*		                            %1\3gplibrary\format\invalid\avc\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\format\valid\3gp\*.*		                            %1\3gplibrary\format\valid\3gp\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\format\valid\mp4\*.*		                            %1\3gplibrary\format\valid\mp4\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\format\valid\3g2\*.*		                            %1\3gplibrary\format\valid\3g2\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\format\valid\misc\*.*		                            %1\3gplibrary\format\valid\misc\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\interop\*.*	                        		   		 	%1\3gplibrary\interop\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\performance\compose\*.*	            		            %1\3gplibrary\performance\compose\*.*
copy %EPOCROOT%epoc32\data\c\3gplibrary\performance\parse\*.*		        	                %1\3gplibrary\performance\parse\*.*

REM ################################ END 3GP LIBRARY DATA ############################


REM ############################### START OF XVID UNITTEST DATA ###############################
mkdir %1\xvidtestdata	
xcopy /E /I /Y %EPOCROOT%epoc32\data\c\xvidtestdata  %1\xvidtestdata
REM ################################ END OF XVID UNITTEST DATA ################################

chkdsk %1
:end

