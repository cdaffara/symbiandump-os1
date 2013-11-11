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

del E:\Logs\TestResults\TSU_MMF_ACLNT_DRM.*
del E:\Logs\TestResults\TSU_MMF_ACLNT_DRM_ALLOC.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** ACLNT DRM TESTS**************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\aclnt\
md c:\AclntITestData\

copy z:\TSU_MMF_ACLNT_01.ini c:\mm\TSU_MMF_ACLNT_01.ini

copy E:\unittest\mmf\aclnt\mainTst.wav c:\AclntITestData\mainTst.wav
copy E:\unittest\mmf\aclnt\mainTst2.wav c:\AclntITestData\mainTst2.wav
copy E:\unittest\mmf\aclnt\mainTst3.wav c:\AclntITestData\mainTst3.wav
copy E:\unittest\mmf\aclnt\dtmfsample1.txt c:\mm\mmf\testfiles\aclnt\dtmfsample1.txt

e:\rta\makeContent.bat

testframework TSU_MMF_ACLNT_DRM.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_ACLNT_DRM_ALLOC.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\aclnt\*.*
del c:\AclntITestData\*.*
