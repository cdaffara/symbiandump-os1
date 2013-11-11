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
rem rummmfsubtitleinttests.bat
rem 
rem

del E:\Logs\TestResults\vclntavi\*
md E:\Logs\
md E:\Logs\TestResults\
md E:\Logs\TestResults\vclntavi\

REM **********************************************************
REM ************* SUBTITLE INTEGRATION TESTS *****************
REM **********************************************************

md C:\vclntavi\

copy \vclntavi\video_avi.ini C:\vclntavi\video_avi.ini

copy E:\inttest\mmf\vclntavi\xvid_subtitle_long.avi C:\vclntavi\xvid_subtitle_long.avi
copy E:\inttest\mmf\vclntavi\xvid_subtitle_long.srt C:\vclntavi\xvid_subtitle_long.srt
copy E:\inttest\mmf\vclntavi\xvid_subtitle_corrupt.avi C:\vclntavi\xvid_subtitle_corrupt.avi
copy E:\inttest\mmf\vclntavi\xvid_subtitle_corrupt.srt C:\vclntavi\xvid_subtitle_corrupt.srt
copy E:\inttest\mmf\vclntavi\xvid_subtitle.avi C:\vclntavi\xvid_subtitle.avi
copy E:\inttest\mmf\vclntavi\xvid_subtitle.srt C:\vclntavi\xvid_subtitle.srt
copy E:\inttest\mmf\vclntavi\xvidonly.avi C:\vclntavi\xvidonly.avi

testframework \vclntavi\tsi_mmf_vclntavi_subtitles.script
testframework \vclntavi\tsi_mmf_vclntavi_subtitles_alloc.script
testframework \vclntavi\tsi_mmf_vclntavi_subtitles_urel.script


copy C:\Logs\TestResults\vclntavi\*.htm E:\Logs\TestResults\vclntavi\

del C:\vclntavi\*
