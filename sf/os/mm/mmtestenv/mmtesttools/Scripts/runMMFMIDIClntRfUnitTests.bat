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
rem runMMFMIDIClntUnitTests.bat
rem 
rem

del E:\Logs\TestResults\TSU_MMF_MIDICLNT_RF.htm
del E:\Logs\TestResults\TSU_MMF_MIDICLNT_RF_Alloc.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** MIDICLNT TESTS ******************
REM ***********************************************

md c:\MidiClntITestData\

copy E:\MidiClntITestData\midi.mid c:\MidiClntITestData\midi.mid
copy E:\MidiClntITestData\textmidi.mid c:\MidiClntITestData\textmidi.mid

testframework TSU_MMF_MIDICLNT_RF.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework TSU_MMF_MIDICLNT_RF_Alloc.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\MidiClntITestData\*.*
