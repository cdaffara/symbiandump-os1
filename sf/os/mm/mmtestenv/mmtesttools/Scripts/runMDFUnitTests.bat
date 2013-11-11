rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del E:\Logs\TestResults\tsu_mdf_audiocodecs_01.htm
del E:\Logs\TestResults\tsu_mdf_videocodecs_01.htm
del E:\Logs\TestResults\tsu_mdf_videocodecs_cov.htm
del E:\Logs\TestResults\tsu_mdf_videocodecs_cov_panic.htm
del E:\Logs\TestResults\tsu_mdf_codecapi_01.htm
del E:\Logs\TestResults\tsu_mdf_omxvorbiscodecs_01.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** MDF TESTS ********************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\mdf\

copy E:\unittest\mdf\audio\depeche1.wav c:\mm\mmf\testfiles\mdf\depeche1.wav
copy E:\unittest\mdf\video\decodervideotest.in c:\mm\mmf\testfiles\mdf\decodervideotest.in
copy E:\unittest\mdf\video\earthrot.gif c:\mm\mmf\testfiles\mdf\earthrot.gif
copy E:\unittest\mdf\video\clock.gif c:\mm\mmf\testfiles\mdf\clock.gif
copy E:\unittest\mdf\audio\depeche2.wav c:\mm\mmf\testfiles\mdf\depeche2.wav
copy E:\unittest\mdf\audio\ogg-encode-in.wav c:\mm\mmf\testfiles\mdf\ogg-encode-in.wav
copy E:\unittest\mdf\audio\ogg-decode-in.ogg c:\mm\mmf\testfiles\mdf\ogg-decode-in.ogg

testframework tsu_mdf_audiocodecs_01.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mdf_videocodecs_01.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mdf_videocodecs_cov.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mdf_videocodecs_cov_panic.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mdf_codecapi_01.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

REM testframework tsu_mdf_omxvorbiscodecs_01.script
REM move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del  c:\mm\mmf\testfiles\mdf\*.*

