rem Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem

del E:\Logs\TestResults\tsu_mmf_actrl_01.htm
del E:\Logs\TestResults\tsu_mmf_actrl_01_alloc.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** ACTRL TESTS *****************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\actrl\

REM [ RampVolume.wav added for ramp test 064                                     ]
REM ------------------------------------------------------------------------------

copy E:\unittest\mmf\actrl\RampVolume.wav c:\mm\mmf\testfiles\actrl\RampVolume.wav
copy E:\unittest\mmf\actrl\newmail.wav c:\mm\mmf\testfiles\actrl\newmail.wav
copy E:\unittest\mmf\actrl\newmail.au c:\mm\mmf\testfiles\actrl\newmail.au
copy E:\unittest\mmf\actrl\16bitmPcm.wav c:\mm\mmf\testfiles\actrl\16bitmPcm.wav
copy E:\unittest\mmf\actrl\11KHzADPCMMono1KHzTone.wav c:\mm\mmf\testfiles\actrl\11KHzADPCMMono1KHzTone.wav
copy E:\unittest\mmf\actrl\11KHzPCM16Mono1KHzTone.raw c:\mm\mmf\testfiles\actrl\11KHzPCM16Mono1KHzTone.raw
copy E:\unittest\mmf\actrl\11KHzPCM16Stereo1KHzTone.wav c:\mm\mmf\testfiles\actrl\11KHzPCM16Stereo1KHzTone.wav
copy E:\unittest\mmf\actrl\22KHzALawMono1KHzTone.wav c:\mm\mmf\testfiles\actrl\22KHzALawMono1KHzTone.wav
copy E:\unittest\mmf\actrl\22KHzALAWStereo1KHzTone.raw c:\mm\mmf\testfiles\actrl\22KHzALAWStereo1KHzTone.raw
copy E:\unittest\mmf\actrl\22KHzPCM16Mono1KHzTone.au c:\mm\mmf\testfiles\actrl\22KHzPCM16Mono1KHzTone.au
copy E:\unittest\mmf\actrl\22KHzPCM16Stereo1KHzTone.wav c:\mm\mmf\testfiles\actrl\22KHzPCM16Stereo1KHzTone.wav
copy E:\unittest\mmf\actrl\22KHzPCM8Mono1KHzTone.wav c:\mm\mmf\testfiles\actrl\22KHzPCM8Mono1KHzTone.wav
copy E:\unittest\mmf\actrl\44KHzPCM16Stereo1KHzTone.au c:\mm\mmf\testfiles\actrl\44KHzPCM16Stereo1KHzTone.au
copy E:\unittest\mmf\actrl\44KHzPCM16Stereo1KHzTone.raw c:\mm\mmf\testfiles\actrl\44KHzPCM16Stereo1KHzTone.raw
copy E:\unittest\mmf\actrl\44KHzPCM16Stereo1KHzTone.wav c:\mm\mmf\testfiles\actrl\44KHzPCM16Stereo1KHzTone.wav
copy E:\unittest\mmf\actrl\8KHzGSM610Mono1KHzTone.raw c:\mm\mmf\testfiles\actrl\8KHzGSM610Mono1KHzTone.raw
copy E:\unittest\mmf\actrl\8KHzGSM610Mono1KHzTone.wav c:\mm\mmf\testfiles\actrl\8KHzGSM610Mono1KHzTone.wav
copy E:\unittest\mmf\actrl\8KHzPCM16Mono1KHzTone.au c:\mm\mmf\testfiles\actrl\8KHzPCM16Mono1KHzTone.au
copy E:\unittest\mmf\actrl\8KHzPCM16Stereo1KHzTone.wav c:\mm\mmf\testfiles\actrl\8KHzPCM16Stereo1KHzTone.wav
copy E:\unittest\mmf\actrl\Reference\*.wav c:\mm\mmf\testfiles\actrl\
copy E:\unittest\mmf\actrl\Reference\*.au c:\mm\mmf\testfiles\actrl\
copy E:\unittest\mmf\actrl\Reference\*.raw c:\mm\mmf\testfiles\actrl\
copy E:\unittest\mmf\actrl\Reference\NoReferenceFile c:\mm\mmf\testfiles\actrl\NoReferenceFile
copy E:\unittest\mmf\actrl\mainTst.wav c:\mm\mmf\testfiles\actrl\mainTst.wav

attrib c:\mm\mmf\testfiles\actrl\newmail.wav -r
attrib c:\mm\mmf\testfiles\actrl\newmail.au -r
attrib c:\mm\mmf\testfiles\actrl\16bitmPcm.wav -r
attrib c:\mm\mmf\testfiles\actrl\11KHzADPCMMonoWAVTo8KHzPCM16Mono.raw -r
attrib c:\mm\mmf\testfiles\actrl\11KHzPCM16MonoRAWTo8KHzGSM610Mono.wav -r
attrib c:\mm\mmf\testfiles\actrl\11KHzPCM16StereoWAVTo8KHzMuLAWMono.wav -r
attrib c:\mm\mmf\testfiles\actrl\22KHzALAWMonoWAVTo8KHzPCM16Mono.wav -r
attrib c:\mm\mmf\testfiles\actrl\22KHzALAWStereoRAWTo11KHzPCM16Mono.raw -r
attrib c:\mm\mmf\testfiles\actrl\22KHzPCM16MonoAUTo8KhzPCM16Mono.wav -r
attrib c:\mm\mmf\testfiles\actrl\22KHzPCM8MonoWAVTo11KHzPCM16Stereo.wav -r
attrib c:\mm\mmf\testfiles\actrl\44KHzPCM16StereoAUTo22KHzPCM16Mono.raw -r
attrib c:\mm\mmf\testfiles\actrl\44KHzPCM16StereoRAWTo8KHzPCM16Stereo.au -r
attrib c:\mm\mmf\testfiles\actrl\44KHzPCM16StereoWAVTo8KHzPCM16Mono.au -r
attrib c:\mm\mmf\testfiles\actrl\8KHzGSM610MonoRAWTo8KHzPCM16Mono.wav -r
attrib c:\mm\mmf\testfiles\actrl\8KHzPCM16MonoAUTo8KHzPCM16Mono.au -r
attrib c:\mm\mmf\testfiles\actrl\8KHzPCM16StereoWAVTo8KHzGSM610Mono.wav -r
attrib c:\mm\mmf\testfiles\actrl\11KHzPCMU8MonoAUTo8KHzPCM16Mono.wav -r
attrib c:\mm\mmf\testfiles\actrl\11KHzPCM16StereoWAVTo11KHzADPCMMono.wav -r
attrib c:\mm\mmf\testfiles\actrl\11KHzPCM16StereoWAVTo11KHzADPCMMono.raw -r
attrib c:\mm\mmf\testfiles\actrl\NoReferenceFile -r
attrib c:\mm\mmf\testfiles\actrl\11KHzADPCMMono1KHzTone.wav -r
attrib c:\mm\mmf\testfiles\actrl\11KHzPCM16Mono1KHzTone.raw -r
attrib c:\mm\mmf\testfiles\actrl\11KHzPCM16Stereo1KHzTone.wav -r
attrib c:\mm\mmf\testfiles\actrl\22KHzALawMono1KHzTone.wav -r
attrib c:\mm\mmf\testfiles\actrl\22KHzALAWStereo1KHzTone.raw -r
attrib c:\mm\mmf\testfiles\actrl\22KHzPCM16Mono1KHzTone.au -r
attrib c:\mm\mmf\testfiles\actrl\22KHzPCM16Stereo1KHzTone.wav -r
attrib c:\mm\mmf\testfiles\actrl\22KHzPCM8Mono1KHzTone.wav -r
attrib c:\mm\mmf\testfiles\actrl\44KHzPCM16Stereo1KHzTone.au -r
attrib c:\mm\mmf\testfiles\actrl\44KHzPCM16Stereo1KHzTone.raw -r
attrib c:\mm\mmf\testfiles\actrl\44KHzPCM16Stereo1KHzTone.wav -r
attrib c:\mm\mmf\testfiles\actrl\8KHzGSM610Mono1KHzTone.raw -r
attrib c:\mm\mmf\testfiles\actrl\8KHzGSM610Mono1KHzTone.wav -r
attrib c:\mm\mmf\testfiles\actrl\8KHzPCM16Mono1KHzTone.au -r
attrib c:\mm\mmf\testfiles\actrl\8KHzPCM16Stereo1KHzTone.wav -r
attrib c:\mm\mmf\testfiles\actrl\mainTst.wav -r

testframework tsu_mmf_actrl_01.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mmf_actrl_01_alloc.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\actrl\*.*


