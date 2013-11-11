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

del E:\Logs\TestResults\TSI_MMF_RECOG.htm
del E:\Logs\TestResults\TSI_MMF_RECOG_ALLOC.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** MMF RECOG TESTS *************
REM ***********************************************

md c:\AclntITestData\

copy E:\inttest\MMF\AclntITestData\test.wav C:\AclntITestData\test.wav
copy E:\inttest\MMF\AclntITestData\test16m2.au C:\AclntITestData\test16m2.au
copy E:\inttest\MMF\AclntITestData\test.raw C:\AclntITestData\test.raw
copy E:\inttest\MMF\AclntITestData\a.txt C:\AclntITestData\a.txt
copy E:\inttest\MMF\AclntITestData\b.txt C:\AclntITestData\b.txt
copy E:\inttest\MMF\AclntITestData\g.txt C:\AclntITestData\g.txt
copy E:\inttest\MMF\AclntITestData\gif.txt C:\AclntITestData\gif.txt
copy E:\inttest\MMF\AclntITestData\i.txt C:\AclntITestData\i.txt
copy E:\inttest\MMF\AclntITestData\m.txt C:\AclntITestData\m.txt
copy E:\inttest\MMF\AclntITestData\r.txt C:\AclntITestData\r.txt
copy E:\inttest\MMF\AclntITestData\recog_wav.wav C:\AclntITestData\recog_wav.wav
copy E:\inttest\MMF\AclntITestData\recog_wav.au C:\AclntITestData\recog_wav.au
copy E:\inttest\MMF\AclntITestData\recog_wav C:\AclntITestData\recog_wav
copy E:\inttest\MMF\AclntITestData\recog_au.au C:\AclntITestData\recog_au.au
copy E:\inttest\MMF\AclntITestData\recog_au.wav C:\AclntITestData\recog_au.wav
copy E:\inttest\MMF\AclntITestData\recog_raw.wav C:\AclntITestData\recog_raw.wav
copy E:\inttest\MMF\AclntITestData\test.abc C:\AclntITestData\test.abc
copy E:\inttest\MMF\AclntITestData\test.xyz C:\AclntITestData\test.xyz

testframeworkrecognizer TSI_MMF_RECOG.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframeworkrecognizer TSI_MMF_RECOG_ALLOC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\AclntITestData\*.*
