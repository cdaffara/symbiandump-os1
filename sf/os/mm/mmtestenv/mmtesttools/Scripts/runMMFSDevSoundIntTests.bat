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

del E:\Logs\TestResults\TSI_MMF_SDevSound_01.htm
del E:\Logs\TestResults\TSI_MMF_SDevSound_01_Panic.htm
del E:\Logs\TestResults\TSI_MMF_SDEVSOUND_NOCAP.htm
del E:\Logs\TestResults\TSI_MMF_SDEVSOUND_MMDDCAP.htm
del E:\Logs\TestResults\TSI_MMF_SDEVSOUND_UECAP.htm
del E:\Logs\TestResults\TSI_MMF_SDevSound_01_PREEMPTION.htm

md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** SDEVSOUND TESTS ******************
REM ***********************************************

md c:\AclntITestData\
md C:\sdevsoundinttestdata\
md C:\TSU_MMF_DEVSOUND_SUITE\
md C:\TSU_MMF_DEVSOUND_SUITE\input\

copy E:\inttest\MMF\AclntITestData\16bitmPcm.wav C:\AclntITestData\16bitmPcm.wav
copy E:\inttest\MMF\AclntITestData\8bitmAlaw.wav C:\AclntITestData\8bitmAlaw.wav
copy E:\inttest\MMF\AclntITestData\8bitmPcm.wav C:\AclntITestData\8bitmPcm.wav
copy E:\inttest\MMF\AclntITestData\8bitmPcm-6144bytes.wav C:\AclntITestData\8bitmPcm-6144bytes.wav
copy E:\inttest\MMF\AclntITestData\8bitmuLaw.wav C:\AclntITestData\8bitmuLaw.wav
copy E:\inttest\MMF\AclntITestData\8bitmUPCM.raw C:\AclntITestData\8bitmUPCM.raw
copy E:\inttest\MMF\AclntITestData\Gsm610.wav C:\AclntITestData\Gsm610.wav
copy E:\inttest\MMF\AclntITestData\IMAD.wav C:\AclntITestData\IMAD.wav
copy E:\inttest\MMF\AclntITestData\IMAS.wav C:\AclntITestData\IMAS.wav
copy E:\inttest\MMF\AclntITestData\PcmU16BE.wav C:\AclntITestData\PcmU16BE.wav
copy E:\inttest\MMF\AclntITestData\mainTst.wav C:\AclntITestData\mainTst.wav
copy E:\inttest\MMF\AclntITestData\small.wav C:\AclntITestData\small.wav
copy E:\inttest\MMF\AclntITestData\small.au C:\AclntITestData\small.au
copy E:\inttest\MMF\AclntITestData\test.wav C:\AclntITestData\test.wav
copy E:\inttest\MMF\AclntITestData\test16m2.au C:\AclntITestData\test16m2.au
copy E:\inttest\MMF\AclntITestData\test16m2.wav C:\AclntITestData\test16m2.wav
copy E:\inttest\MMF\AclntITestData\test2.wav C:\AclntITestData\test2.wav
copy E:\inttest\MMF\AclntITestData\unsupported.wav C:\AclntITestData\unsupported.wav
copy E:\inttest\MMF\AclntITestData\alf-noproblem.wav C:\AclntITestData\alf-noproblem.wav
copy E:\inttest\MMF\AclntITestData\100khzSRate1KhzTone.wav C:\AclntITestData\100khzSRate1KhzTone.wav
copy E:\inttest\MMF\AclntITestData\7.33khzSRate1KhzTone.wav C:\AclntITestData\7.33khzSRate1KhzTone.wav
copy E:\inttest\MMF\AclntITestData\OneSample.wav C:\AclntITestData\OneSample.wav
copy E:\inttest\MMF\AclntITestData\sequence1.sqn C:\AclntITestData\sequence1.sqn
copy E:\inttest\MMF\AclntITestData\sequence2.sqn C:\AclntITestData\sequence2.sqn
copy E:\inttest\MMF\AclntITestData\16bitmPcm.xxx C:\AclntITestData\16bitmPcm.xxx
copy E:\inttest\MMF\AclntITestData\explo.wav C:\AclntITestData\explo.wav

copy E:\inttest\MMF\AclntITestData\8bitmuPcm.wav C:\AclntITestData\8bitmuPcm.wav
copy E:\inttest\MMF\AclntITestData\mainTst2.wav C:\AclntITestData\mainTst2.wav
copy E:\inttest\MMF\AclntITestData\main.wav C:\AclntITestData\main.wav
copy E:\inttest\MMF\AclntITestData\test.raw C:\AclntITestData\test.raw
copy E:\inttest\MMF\AclntITestData\pcm8mono44khz.wav C:\AclntITestData\pcm8mono44khz.wav
copy E:\inttest\MMF\AclntITestData\pcm16mono44khz.wav C:\AclntITestData\pcm16mono44khz.wav
copy E:\inttest\MMF\AclntITestData\pcm16mono48khz.wav C:\AclntITestData\pcm16mono48khz.wav
copy E:\inttest\MMF\AclntITestData\pcm16mono12khz.wav C:\AclntITestData\pcm16mono12khz.wav
copy E:\inttest\MMF\AclntITestData\pcm16mono24khz.wav C:\AclntITestData\pcm16mono24khz.wav
copy E:\inttest\MMF\AclntITestData\test.abc C:\AclntITestData\test.abc
copy E:\inttest\MMF\AclntITestData\test.xyz C:\AclntITestData\test.xyz
copy E:\inttest\MMF\AclntITestData\drmwavfile.drm C:\AclntITestData\drmwavfile.drm

copy E:\inttest\MMF\AclntITestData\PCM8bitMono.wav C:\AclntITestData\PCM8bitMono.wav
copy E:\inttest\MMF\AclntITestData\PCM8bitStereo.wav C:\AclntITestData\PCM8bitStereo.wav
copy E:\inttest\MMF\AclntITestData\PCM8bitMono.au C:\AclntITestData\PCM8bitMono.au
copy E:\inttest\MMF\AclntITestData\PCM8bitStereo.au C:\AclntITestData\PCM8bitStereo.au
copy E:\inttest\MMF\AclntITestData\PCM16bitMono.wav C:\AclntITestData\PCM16bitMono.wav
copy E:\inttest\MMF\AclntITestData\PCM16bitStereo.wav C:\AclntITestData\PCM16bitStereo.wav
copy E:\inttest\MMF\AclntITestData\PCM16bitMono.au C:\AclntITestData\PCM16bitMono.au
copy E:\inttest\MMF\AclntITestData\PCM16bitStereo.au C:\AclntITestData\PCM16bitStereo.au

copy E:\inttest\MMF\sdevsoundinttestdata\8bitmPcm.wav C:\sdevsoundinttestdata\8bitmPcm.wav
copy E:\inttest\MMF\sdevsoundinttestdata\PCM_08_16_m.raw C:\sdevsoundinttestdata\PCM_08_16_m.raw
copy E:\inttest\MMF\sdevsoundinttestdata\mainTst.wav C:\sdevsoundinttestdata\mainTst.wav
copy E:\inttest\MMF\AclntITestData\mainTst.wav C:\TSU_MMF_DEVSOUND_SUITE\input\mainTst.wav

copy E:\inttest\MMF\AclntITestData\audio.ini  c:\audio.ini
copy E:\inttest\MMF\AclntITestData\sdevsound_audio.ini  c:\sdevsound_audio.ini

REM *** Test framework with ALL -TCB capabilities ***
testframework TSI_MMF_SDevSound_01.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSI_MMF_SDevSound_01_Panic.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframeworknone TSI_MMF_SDEVSOUND_NOCAP.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

REM *** Test framework with MultimediaDD capability ***
testframeworkMMDDCap TSI_MMF_SDEVSOUND_MMDDCAP.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

REM *** Test framework with UserEnvironment capability ***
testframeworkUECap TSI_MMF_SDEVSOUND_UECAP.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSI_MMF_SDevSound_01_PREEMPTION.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\sdevsoundinttestdata\*.*
del c:\AclntITestData\*.*
del C:\TSU_MMF_DEVSOUND_SUITE\input\*.*
del c:\audio.ini