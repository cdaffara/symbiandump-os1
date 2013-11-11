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

del E:\Logs\TestResults\tsu_mmf_afmt.htm
del E:\Logs\TestResults\tsu_mmf_afmt_alloc.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** AFMT TESTS ******************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\afmt\

copy E:\unittest\mmf\afmt\adpcm16mono8khz.raw c:\mm\mmf\testfiles\afmt\adpcm16mono8khz.raw
copy E:\unittest\mmf\afmt\adpcm16mono8khz.wav c:\mm\mmf\testfiles\afmt\adpcm16mono8khz.wav
copy E:\unittest\mmf\afmt\adpcm16mono8khzCompare.raw c:\mm\mmf\testfiles\afmt\adpcm16mono8khzCompare.raw
copy E:\unittest\mmf\afmt\adpcm16mono8khzCompare.wav c:\mm\mmf\testfiles\afmt\adpcm16mono8khzCompare.wav
copy E:\unittest\mmf\afmt\adpcm16stereo8khz.raw c:\mm\mmf\testfiles\afmt\adpcm16stereo8khz.raw
copy E:\unittest\mmf\afmt\adpcm16stereo8khz.wav c:\mm\mmf\testfiles\afmt\adpcm16stereo8khz.wav
copy E:\unittest\mmf\afmt\adpcm16stereo8khzCompare.raw c:\mm\mmf\testfiles\afmt\adpcm16stereo8khzCompare.raw
copy E:\unittest\mmf\afmt\adpcm16stereo8khzCompare.wav c:\mm\mmf\testfiles\afmt\adpcm16stereo8khzCompare.wav
copy E:\unittest\mmf\afmt\gsmMono8khz.raw c:\mm\mmf\testfiles\afmt\gsmMono8khz.raw
copy E:\unittest\mmf\afmt\gsmMono8khz.wav c:\mm\mmf\testfiles\afmt\gsmMono8khz.wav
copy E:\unittest\mmf\afmt\gsmMono8khzCompare.raw c:\mm\mmf\testfiles\afmt\gsmMono8khzCompare.raw
copy E:\unittest\mmf\afmt\gsmMono8khzCompare.wav c:\mm\mmf\testfiles\afmt\gsmMono8khzCompare.wav
copy E:\unittest\mmf\afmt\mulaw8mono44khz.au c:\mm\mmf\testfiles\afmt\mulaw8mono44khz.au
copy E:\unittest\mmf\afmt\pcm16mono22khz.au c:\mm\mmf\testfiles\afmt\pcm16mono22khz.au
copy E:\unittest\mmf\afmt\pcm16mono22khz.raw c:\mm\mmf\testfiles\afmt\pcm16mono22khz.raw
copy E:\unittest\mmf\afmt\pcm16mono22khz.wav c:\mm\mmf\testfiles\afmt\pcm16mono22khz.wav
copy E:\unittest\mmf\afmt\pcm16mono8khz.au c:\mm\mmf\testfiles\afmt\pcm16mono8khz.au
copy E:\unittest\mmf\afmt\pcm16mono8khz.raw c:\mm\mmf\testfiles\afmt\pcm16mono8khz.raw
copy E:\unittest\mmf\afmt\pcm16mono8khz.wav c:\mm\mmf\testfiles\afmt\pcm16mono8khz.wav
copy E:\unittest\mmf\afmt\pcm16stereo8khz.au c:\mm\mmf\testfiles\afmt\pcm16stereo8khz.au
copy E:\unittest\mmf\afmt\pcm16stereo8khz.raw c:\mm\mmf\testfiles\afmt\pcm16stereo8khz.raw
copy E:\unittest\mmf\afmt\pcm16stereo8khz.wav c:\mm\mmf\testfiles\afmt\pcm16stereo8khz.wav
copy E:\unittest\mmf\afmt\pcm16stereo8khzCompare.au c:\mm\mmf\testfiles\afmt\pcm16stereo8khzCompare.au
copy E:\unittest\mmf\afmt\pcm16stereo8khzCompare.raw c:\mm\mmf\testfiles\afmt\pcm16stereo8khzCompare.raw
copy E:\unittest\mmf\afmt\pcm16stereo8khzCompare.wav c:\mm\mmf\testfiles\afmt\pcm16stereo8khzCompare.wav
copy E:\unittest\mmf\afmt\pcm8mono11khz.au c:\mm\mmf\testfiles\afmt\pcm8mono11khz.au
copy E:\unittest\mmf\afmt\pcm8mono11khz.raw c:\mm\mmf\testfiles\afmt\pcm8mono11khz.raw
copy E:\unittest\mmf\afmt\pcm8mono11khz.wav c:\mm\mmf\testfiles\afmt\pcm8mono11khz.wav
copy E:\unittest\mmf\afmt\pcm8mono44khz.au c:\mm\mmf\testfiles\afmt\pcm8mono44khz.au
copy E:\unittest\mmf\afmt\pcm8mono44khz.raw c:\mm\mmf\testfiles\afmt\pcm8mono44khz.raw
copy E:\unittest\mmf\afmt\pcm8mono44khz.wav c:\mm\mmf\testfiles\afmt\pcm8mono44khz.wav
copy E:\unittest\mmf\afmt\pcm4mono11khz.wav c:\mm\mmf\testfiles\afmt\pcm4mono11khz.wav

testframework tsu_mmf_afmt.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mmf_afmt_alloc.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\afmt\*.*
