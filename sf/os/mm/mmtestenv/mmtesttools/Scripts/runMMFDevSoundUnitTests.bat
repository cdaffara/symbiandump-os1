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

del E:\Logs\TestResults\TSU_MMF_DEVSOUND_SUITE_debug.htm
del E:\Logs\TestResults\TSU_MMF_DEVSOUND_SUITE_MUXDEMUX.htm
del E:\Logs\TestResults\TSU_MMF_DEVSOUND_SUITE_alloc.htm
del E:\Logs\TestResults\TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.htm
del E:\Logs\TestResults\tsu_mmf_devsound_ciu.htm
del E:\Logs\TestResults\TSU_MMF_DEVSOUND_SUITE_PREEMPTION.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** DevSound Tests ***************
REM ***********************************************

md c:\mm\
md c:\tsu_mmf_devsound_suite\
md c:\tsu_mmf_devsound_suite\input\
md c:\tsu_mmf_devsound_suite\output\

copy e:\tsu_mmf_devsound_suite\input\*.* c:\tsu_mmf_devsound_suite\input\
attrib c:\tsu_mmf_devsound_suite\input\*.* -r

copy z:\tsu_mmf_devsound_record.ini	c:\tsu_mmf_devsound_record.ini

testframework TSU_MMF_DEVSOUND_SUITE_debug.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_DEVSOUND_SUITE_MUXDEMUX.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_DEVSOUND_SUITE_alloc.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mmf_devsound_ciu.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_DEVSOUND_SUITE_PREEMPTION.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\tsu_mmf_devsound_suite\input\*.*
del c:\tsu_mmf_devsound_suite\output\*.*
del c:\tsu_mmf_devsound_suite\*.*
