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

del E:\Logs\TestResults\tsi_mvs_audioagent.htm
del E:\Logs\TestResults\tsi_mvs_audioagent_alloc.htm
del E:\Logs\TestResults\tsi_mvs_plugin.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ************** MVS AUDIOAGENTS TESTS **********
REM ***********************************************

md c:\mvs\

copy e:\tsi_mvs_agents.ini c:\mvs\tsi_mvs_agents.ini

copy E:\inttest\mvs\wavtest01.wav c:\mvs\wavtest01.wav
copy E:\inttest\mvs\wavtest02.wav c:\mvs\wavtest02.wav
copy E:\inttest\mvs\textfile.ogg c:\mvs\textfile.ogg
copy E:\inttest\mvs\oggtest01.ogg c:\mvs\oggtest01.ogg
copy E:\inttest\mvs\mp3file.ogg c:\mvs\mp3file.ogg
copy E:\inttest\mvs\pcm16stereo8khz.sjau c:\mvs\pcm16stereo8khz.sjau

testframework tsi_mvs_audioagent.script
testframework tsi_mvs_audioagent_alloc.script
testframework tsi_mvs_plugin.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\mvs\*.*
