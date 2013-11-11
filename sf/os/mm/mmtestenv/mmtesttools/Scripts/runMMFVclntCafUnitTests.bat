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

del E:\Logs\TestResults\TSU_MMF_VCLNT_01_*.*
del E:\Logs\TestResults\tsu_mmf_vclnt_rf_*.*

md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** VCLNT TESTS ******************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\vclnt\

copy E:\unittest\mmf\vclnt\mm\small2.wav c:\mm\mmf\testfiles\vclnt\small2.wav
copy E:\unittest\mmf\vclnt\mm\smallv.wav c:\mm\mmf\testfiles\vclnt\smallv.wav
copy E:\unittest\mmf\vclnt\mm\clock.dummy c:\mm\mmf\testfiles\vclnt\clock.dummy

copy E:\unittest\mmf\vclnt\mm\AudVid.avi c:\mm\mmf\testfiles\vclnt\AudVid.avi
copy E:\unittest\mmf\vclnt\mm\Vid.avi c:\mm\mmf\testfiles\vclnt\Vid.avi
copy E:\unittest\mmf\vclnt\mm\xvidonly.avi c:\mm\mmf\testfiles\vclnt\xvidonly.avi

testframework TSU_MMF_VCLNT_01_WM_CAF_V2.script
testframework tsu_mmf_vclnt_01_alloc_wm.script
testframework tsu_mmf_vclnt_rf_wm.script
testframework tsu_mmf_vclnt_rf_alloc_wm.script

testframework tsu_mmf_vclnt_rf_wm_notification.script
testframework TSU_MMF_VCLNT_01_WM_CAF_V2_priority.script

move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\vclnt\*.*

