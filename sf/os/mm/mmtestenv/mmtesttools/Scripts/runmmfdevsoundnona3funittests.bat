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
rem runMMFNonA3FDevSoundUnitTests.bat
rem 
rem

del E:\Logs\TestResults\TSU_MMF_DEVSOUND_SUITE_NONA3F.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ************* NonA3FDevSound Tests ************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\truepause_devsound_test_data\

copy z:\tsu_mmf_truepause_devsound.ini	c:\tsu_mmf_truepause_devsound.ini

copy e:\mm\mmf\testfiles\truepause_devsound_test_data\cerrodelasilla.raw c:\mm\mmf\testfiles\truepause_devsound_test_data\
attrib c:\tsu_mmf_devsound_suite\input\cerrodelasilla.raw -r

testframework tsu_mmf_devsound_suite_nona3f.script
move c:\logs\testresults\*.htm e:\logs\testresults\

del c:\mm\mmf\testfiles\truepause_devsound_test_data\cerrodelasilla.raw

