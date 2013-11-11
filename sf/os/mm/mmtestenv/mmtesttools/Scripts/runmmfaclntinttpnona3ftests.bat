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
rem

del E:\Logs\TestResults\tsi_mmf_aclnt_00_nona3f.htm
del E:\Logs\TestResults\tsi_mmf_aclnt_03_nona3f.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** ACLNT TESTS ******************
REM ***********************************************

md c:\AclntITestData\

copy e:\inttest\mmf\aclntitestdata\maintst.ogg c:\aclntitestdata\maintst.ogg
copy e:\inttest\mmf\aclntitestdata\test.raw c:\aclntitestdata\test.raw
copy e:\inttest\mmf\aclntitestdata\sequence1.sqn c:\aclntitestdata\sequence1.sqn
copy e:\inttest\mmf\aclntitestdata\sequence2.sqn c:\aclntitestdata\sequence2.sqn
copy e:\inttest\mmf\aclntitestdata\imad.wav c:\aclntitestdata\imad.wav
copy e:\inttest\mmf\aclntitestdata\main.wav c:\aclntitestdata\main.wav

copy e:\inttest\mmf\aclntitestdata\audio.ini  c:\audio.ini

REM Tests for CR1566 to be executed with a3f disabled only

testframework tsi_mmf_aclnt_00_nona3f.script
move c:\logs\testresults\*.htm e:\logs\testresults\

testframework tsi_mmf_aclnt_03_nona3f.script
move c:\logs\testresults\*.htm e:\logs\testresults\

del c:\AclntITestData\*.*
del c:\audio.ini