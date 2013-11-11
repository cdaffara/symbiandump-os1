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

del E:\Logs\TestResults\TSI_MMF_ACLNT_DRM.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** ACLNT DRM TESTS ******************
REM ***********************************************

md c:\AclntITestData\

copy E:\inttest\MMF\AclntITestData\audio.ini C:\audio.ini

copy E:\inttest\MMF\AclntITestData\mainTst.wav C:\AclntITestData\mainTst.wav
copy E:\inttest\MMF\AclntITestData\mainTst2.wav C:\AclntITestData\mainTst2.wav
copy E:\inttest\MMF\AclntITestData\mainTst3.wav C:\AclntITestData\mainTst3.wav
copy E:\inttest\MMF\AclntITestData\Audio.content C:\AclntITestData\Audio.content
copy E:\inttest\MMF\AclntITestData\Audio.contentrights C:\AclntITestData\Audio.contentrights
e:\rta\makeContent.bat

testframework TSI_MMF_ACLNT_DRM.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\AclntITestData\*.*
del c:\audio.ini
