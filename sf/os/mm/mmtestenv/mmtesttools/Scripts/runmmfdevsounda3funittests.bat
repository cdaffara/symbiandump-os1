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
rem True Pause tests for CR1566. True Pause is implemented on a3f only
rem 
rem

del E:\Logs\TestResults\TSU_MMF_DEVSOUND_SUITE_TRUEPAUSE.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** A3FDevSound Tests ************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\truepause_devsound_test_data\

copy z:\tsu_mmf_truepause_devsound.ini	c:\tsu_mmf_truepause_devsound.ini

copy e:\mm\mmf\testfiles\truepause_devsound_test_data\*.* c:\mm\mmf\testfiles\truepause_devsound_test_data\

testframework tsu_mmf_devsound_suite_truepause.script
move c:\logs\testresults\*.htm e:\logs\testresults\

del c:\mm\mmf\testfiles\truepause_devsound_test_data\*.*
