rem Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del E:\Logs\TestResults\TSI_MMF_ACLNTOGG_PLAY.htm
del E:\Logs\TestResults\TSI_MMF_ACLNTOGG_APR.htm
del E:\Logs\TestResults\TSI_MMF_ACLNTOGG_DRM.htm
del E:\Logs\TestResults\TSI_MMF_ACLNTOGG_ALLOC.htm
del E:\Logs\TestResults\TSI_MMF_ACLNTOGG_REC.htm
del E:\Logs\TestResults\TSI_MMF_ACLNTOGG_PLAY_PREEMPTION.htm
del E:\Logs\TestResults\TSI_MMF_ACLNTOGG_REC_PREEMPTION.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** ACLNT TESTS *****************
REM ***********************************************

md c:\AclntITestData\
md c:\AclntITestData\output

copy E:\inttest\MMF\AclntITestData\small.ogg c:\AclntITestData\small.ogg
copy E:\inttest\MMF\AclntITestData\mainTst.ogg c:\AclntITestData\mainTst.ogg
copy E:\inttest\MMF\AclntITestData\mainTst2.ogg c:\AclntITestData\mainTst2.ogg
copy E:\inttest\MMF\AclntITestData\mainTst3.ogg c:\AclntITestData\mainTst3.ogg

copy E:\inttest\MMF\AclntITestData\test.raw C:\AclntITestData\test.raw
copy E:\inttest\MMF\AclntITestData\test2.raw C:\AclntITestData\test2.raw
copy E:\inttest\MMF\AclntITestData\test3.raw C:\AclntITestData\test3.raw

copy E:\inttest\MMF\AclntITestData\AudioOgg.content C:\AclntITestData\AudioOgg.content
copy E:\inttest\MMF\AclntITestData\AudioOgg.contentrights C:\AclntITestData\AudioOgg.contentrights

e:\rta\makeContent.bat

testframework TSI_MMF_ACLNTOGG_PLAY.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework TSI_MMF_ACLNTOGG_APR.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework TSI_MMF_ACLNTOGG_DRM.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework TSI_MMF_ACLNTOGG_ALLOC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework TSI_MMF_ACLNTOGG_REC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework TSI_MMF_ACLNTOGG_PLAY_PREEMPTION.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework TSI_MMF_ACLNTOGG_REC_PREEMPTION.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\AclntITestData\*.*
