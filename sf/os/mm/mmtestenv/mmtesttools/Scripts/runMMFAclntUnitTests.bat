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

move c:\Logs\TestResults\*.htm e:\Logs\TestResults\
del E:\Logs\TestResults\TSU_MMF_ACLNT_01.htm
del E:\Logs\TestResults\TSU_MMF_ACLNT_01_ALLOC.htm
del E:\Logs\TestResults\TSU_MMF_ACLNT_02_ALLOC.htm
del E:\Logs\TestResults\TSU_MMF_ACLNT_03_ALLOC.htm
del E:\Logs\TestResults\TSU_MMF_ACLNT_04_ALLOC.htm
del E:\Logs\TestResults\TSU_MMF_ACLNT_05_ALLOC.htm
del E:\Logs\TestResults\TSU_MMF_ACLNT_RF_ALLOC.htm
del E:\Logs\TestResults\TSU_MMF_ACLNT_01_PREEMPTION.htm
del E:\Logs\TestResults\TSU_MMF_ACLNT_01_NOTIFICATION.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** ACLNT TESTS *****************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\aclnt\

copy z:\TSU_MMF_ACLNT_01.ini c:\mm\TSU_MMF_ACLNT_01.ini

copy E:\unittest\r3.wav  c:\r3.wav
copy E:\unittest\mmf\aclnt\*.* c:\mm\mmf\testfiles\aclnt\

attrib c:\mm\mmf\testfiles\aclnt\*.* -r

testframework TSU_MMF_ACLNT_01.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_ACLNT_01_ALLOC.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_ACLNT_02_ALLOC.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_ACLNT_03_ALLOC.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_ACLNT_04_ALLOC.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_ACLNT_05_ALLOC.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_ACLNT_RF_ALLOC.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_ACLNT_01_PREEMPTION.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_ACLNT_01_NOTIFICATION.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\aclnt\*.*
del c:\mm\TSU_MMF_ACLNT_01.ini