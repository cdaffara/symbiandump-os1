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

del E:\Logs\TestResults\tsi_mvs_videoagent.*
del E:\Logs\TestResults\tsi_mvs_videoagent_alloc.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ************** MVS VIDEOAGENTS TESTS **********
REM ***********************************************

md c:\mvs\

copy e:\tsi_mvs_agents.ini c:\mvs\tsi_mvs_agents.ini

copy E:\inttest\mvs\aud_vid.avi c:\mvs\aud_vid.avi
copy E:\inttest\mvs\xvidonly.avi c:\mvs\xvidonly.avi
copy E:\inttest\mvs\xvid_rec.avi c:\mvs\xvid_rec.avi
copy E:\inttest\mvs\corrupt.avi c:\mvs\corrupt.avi
copy E:\inttest\mvs\invalidcodecavi.avi c:\mvs\invalidcodecavi.avi
copy E:\inttest\mvs\textfile.avi c:\mvs\textfile.avi

testframework tsi_mvs_videoagent.script
testframework tsi_mvs_videoagent_alloc.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\mvs\*.*
