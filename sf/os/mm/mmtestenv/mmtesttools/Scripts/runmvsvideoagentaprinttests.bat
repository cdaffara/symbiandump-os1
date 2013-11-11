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

del E:\Logs\TestResults\mvs\TSI_MVS_VIDEOAGENT_APR.*
md E:\Logs\
md E:\Logs\TestResults\


REM *************************************************
REM ************** APR TESTS ************************
REM *************************************************

md c:\mvs\
md c:\mvs\
md c:\AclntITestData\

copy E:\inttest\mvs\tsi_mvs_agents.ini      c:\mvs\tsi_mvs_agents.ini
copy E:\inttest\mvs\aud_vid.avi             c:\mvs\aud_vid.avi
copy E:\inttest\mvs\aud_vid2.avi            c:\mvs\aud_vid2.avi
copy E:\inttest\mvs\aud_vid_vprp.avi             c:\mvs\aud_vid_vprp.avi
copy E:\inttest\mvs\aud_vid_vprp2.avi            c:\mvs\aud_vid_vprp2.avi
copy E:\inttest\mmf\AclntITestData\mainTst.wav     	    c:\AclntITestData\mainTst.wav

testframework  tsi_mvs_videoagent_apr.script

move   C:\Logs\TestResults\*.htm    E:\Logs\TestResults\  


del  c:\mvs\*.*   
