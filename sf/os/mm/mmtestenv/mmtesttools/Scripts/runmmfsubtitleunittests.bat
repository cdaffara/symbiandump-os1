rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem rummmfsubtitleunittests.bat
rem 
rem

del E:\Logs\TestResults\*
md E:\Logs
md E:\Logs\TestResults\

REM **********************************************************
REM **************** SUBTITLE UNIT TESTS *********************
REM **********************************************************

md C:\mm\
md C:\mm\mmf\
md C:\mm\mmf\testfiles\
md C:\mm\mmf\testfiles\avictrl\
md C:\mm\mmf\testfiles\vclnt\
md C:\mm\mmf\testfiles\subtitlegraphic\
md C:\mm\mmf\testfiles\devsubtitle\

REM avictrl data:
copy E:\unittest\mmf\avictrl\video_only.avi C:\mm\mmf\testfiles\avictrl\video_only.avi
copy E:\unittest\mmf\avictrl\srt_available.avi C:\mm\mmf\testfiles\avictrl\srt_available.avi
copy E:\unittest\mmf\avictrl\srt_available.srt C:\mm\mmf\testfiles\avictrl\srt_available.srt

REM vclnt data:
copy E:\unittest\mmf\vclnt\mm\clock2.dummy C:\mm\mmf\testfiles\vclnt\clock2.dummy
copy E:\unittest\mmf\vclnt\mm\clock.dummy C:\mm\mmf\testfiles\vclnt\clock.dummy

REM subtitle graphic data:
copy E:\unittest\mmf\subtitlegraphic\subtitletestdata.png C:\mm\mmf\testfiles\subtitlegraphic\subtitletestdata.png
copy E:\unittest\mmf\subtitlegraphic\subtitletestdata2.png C:\mm\mmf\testfiles\subtitlegraphic\subtitletestdata2.png
copy E:\unittest\mmf\subtitlegraphic\subtitletestdata3.png C:\mm\mmf\testfiles\subtitlegraphic\subtitletestdata3.png

REM devsubtitle data:
copy E:\unittest\mmf\devsubtitle\devsubtitle.srt C:\mm\mmf\testfiles\devsubtitle\devsubtitle.srt
copy E:\unittest\mmf\devsubtitle\subtitletestdata.png C:\mm\mmf\testfiles\devsubtitle\subtitletestdata.png
copy E:\unittest\mmf\devsubtitle\subtitletestdata2.png C:\mm\mmf\testfiles\devsubtitle\subtitletestdata2.png

REM srt decoder data:
copy E:\unittest\mmf\subtitle\subtitle1.srt C:\mm\subtitle1.srt
copy E:\unittest\mmf\subtitle\subtitle2.srt C:\mm\subtitle2.srt
copy E:\unittest\mmf\subtitle\subtitlebmp0.mbm C:\mm\subtitlebmp0.mbm
copy E:\unittest\mmf\subtitle\subtitlebmp1.mbm C:\mm\subtitlebmp1.mbm
copy E:\unittest\mmf\subtitle\subtitlebmp2.mbm C:\mm\subtitlebmp2.mbm
copy E:\unittest\mmf\subtitle\subtitlebmp3.mbm C:\mm\subtitlebmp3.mbm
copy E:\unittest\mmf\subtitle\subtitlebmp4.mbm C:\mm\subtitlebmp4.mbm
copy E:\unittest\mmf\subtitle\subtitlebmp5.mbm C:\mm\subtitlebmp5.mbm
copy E:\unittest\mmf\subtitle\subtitlebmp6.mbm C:\mm\subtitlebmp6.mbm
copy E:\unittest\mmf\subtitle\subtitlebmp7.mbm C:\mm\subtitlebmp7.mbm
copy E:\unittest\mmf\subtitle\subtitlebmp8.mbm C:\mm\subtitlebmp8.mbm
copy E:\unittest\mmf\subtitle\subtitlebmp9.mbm C:\mm\subtitlebmp9.mbm


REM Run test scripts
testframework tsu_mmf_ctlfrm_subtitles.script
testframework tsu_mmf_avictrl_subtitle.script
testframework tsu_mmf_vclnt_u_01_subtitle_gce_on.script
testframework tsu_mmf_vclnt_u_01_subtitle_gce_on_udeb.script
testframework tsu_mmf_subtitlegraphic.script
testframework tsu_mmf_devsubtitle_panic.script
testframework tsu_mmf_devsubtitle.script
testframework \mm\tsu_mmf_subtitledecoder.script

copy C:\Logs\TestResults\*.htm E:\Logs\TestResults\
copy C:\Logs\TestResults\mm\*.htm E:\Logs\TestResults\

del C:\mm\mmf\testfiles\avictrl\*
del C:\mm\mmf\testfiles\vclnt\*
del C:\mm\mmf\testfiles\subtitlegraphic\*
del C:\mm\mmf\testfiles\devsubtitle\*