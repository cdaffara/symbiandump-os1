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

del E:\Logs\TestResults\TSU_MMF_AVICTRL*.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** AVICTRL TESTS *****************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\avictrl\

copy E:\unittest\mmf\avictrl\aud_vid.avi  c:\mm\mmf\testfiles\avictrl\aud_vid.avi
copy E:\unittest\mmf\avictrl\video_only.avi  c:\mm\mmf\testfiles\avictrl\video_only.avi
copy E:\unittest\mmf\avictrl\invalid.avi  c:\mm\mmf\testfiles\avictrl\invalid.avi
copy E:\unittest\mmf\avictrl\low_mem_vdo_test.avi c:\mm\mmf\testfiles\avictrl\low_mem_vdo_test.avi
copy E:\unittest\mmf\avictrl\aud_vid_vprp.avi  c:\mm\mmf\testfiles\avictrl\aud_vid_vprp.avi

testframework tsu_mmf_avictrl_play.script
copy c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mmf_avictrl_play_alloc.script
copy c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mmf_avictrl_secdisp.script
copy c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mmf_avictrl_record.script
copy c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\avictrl\*.*
