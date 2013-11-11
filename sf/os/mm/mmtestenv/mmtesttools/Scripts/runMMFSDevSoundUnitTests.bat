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
rem runMMFDevSoundUnitTests.bat
rem 
rem

del E:\Logs\TestResults\TSU_MMF_SDEVSOUND_SUITE.*
del E:\Logs\TestResults\TSU_MMF_DEVSOUND_SUITE_NOCAP.*
del E:\Logs\TestResults\TSU_MMF_DEVSOUND_SUITE_MMDDCAP.*
del E:\Logs\TestResults\TSU_MMF_DEVSOUND_SUITE_UECAP.*

md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** DevSound Tests ***************
REM ***********************************************

md c:\tsu_mmf_devsound_suite\
md c:\tsu_mmf_devsound_suite\input\
md c:\tsu_mmf_devsound_suite\output\

copy e:\tsu_mmf_devsound_suite\input\*.* c:\tsu_mmf_devsound_suite\input\
attrib c:\tsu_mmf_devsound_suite\input\*.* -r

REM *** Test framework with ALL -TCB capabilities ***
testframework TSU_MMF_SDEVSOUND_SUITE.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\
del c:\tsu_mmf_devsound_suite\input\*.*

REM *** Test framework with no capabilities ***
testframeworkNone TSU_MMF_DEVSOUND_SUITE_NOCAP.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

REM *** Test framework with MultimediaDD capability ***
testframeworkMMDDCap TSU_MMF_DEVSOUND_SUITE_MMDDCAP.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

REM *** Test framework with UserEnvironment capability ***
testframeworkUECap TSU_MMF_DEVSOUND_SUITE_UECAP.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\tsu_mmf_devsound_suite\input\*.*
del c:\tsu_mmf_devsound_suite\output\*.*
del c:\tsu_mmf_devsound_suite\*.*