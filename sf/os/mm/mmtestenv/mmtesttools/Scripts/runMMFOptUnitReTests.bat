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
rem N.B. Tabs must NOT be used in this batch file!
rem (This is due to a bug in EShell.exe, from which this batch file is run)
rem Create directories on CF card for the result logs
rem Don't ask me why, but immediately after a test has run, attempting to
rem copy the log file returns -14 Not Found. Hence we copy all logs to the CF
rem after each test has run.
rem 
rem

del E:\Logs\TestResults\TSU_MMF_MIDICLNT_00.htm
del E:\Logs\TestResults\TSU_MMF_MIDICLNT_00_Alloc.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** MIDICLNT TESTS ******************
REM ***********************************************
md c:\MidiClntITestData\
copy E:\MidiClntITestData\midi.mid c:\MidiClntITestData\midi.mid

testframework TSU_MMF_MIDICLNT_00.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_MIDICLNT_00_Alloc.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\MidiClntITestData\*.*
