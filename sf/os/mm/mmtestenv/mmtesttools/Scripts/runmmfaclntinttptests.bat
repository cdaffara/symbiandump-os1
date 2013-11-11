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

del E:\Logs\TestResults\tsi_mmf_aclnt_00_truepause.htm
del E:\Logs\TestResults\tsi_mmf_aclnt_03_truepause.htm
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

copy e:\inttest\mmf\aclntitestdata\8bitmpcm.wav c:\aclntitestdata\8bitmpcm.wav
copy e:\inttest\mmf\aclntitestdata\16bitmpcm.wav c:\aclntitestdata\16bitmpcm.wav
copy e:\inttest\mmf\aclntitestdata\8bitmupcm.wav c:\aclntitestdata\8bitmupcm.wav
copy e:\inttest\mmf\aclntitestdata\pcmu16be.wav c:\aclntitestdata\pcmu16.wav
copy e:\inttest\mmf\aclntitestdata\8bitmalaw.wav c:\aclntitestdata\8bitmalaw.wav
copy e:\inttest\mmf\aclntitestdata\8bitmulaw.wav c:\aclntitestdata\8bitmulaw.wav
copy e:\inttest\mmf\aclntitestdata\imad2.wav c:\aclntitestdata\imad2.wav
copy e:\inttest\mmf\aclntitestdata\gsm610.wav c:\aclntitestdata\gsm610.wav
copy e:\inttest\mmf\aclntitestdata\pcmu16be.wav c:\aclntitestdata\pcmu16be.wav

copy e:\inttest\mmf\aclntitestdata\audio.ini  c:\audio.ini

REM Tests for CR1566 to be executed with a3f enabled only

testframework tsi_mmf_aclnt_00_truepause.script
move c:\logs\testresults\*.htm e:\logs\testresults\

testframework tsi_mmf_aclnt_03_truepause.script
move c:\logs\testresults\*.htm e:\logs\testresults\

del c:\AclntITestData\*.*
del c:\audio.ini