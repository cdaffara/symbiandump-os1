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

del E:\Logs\TestResults\TSI_MMF_VCLNTAVI_PLAY_SECDISP.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** VCLNTAVI SECDISP TESTS **************
REM ***********************************************

md c:\vclntavi\

copy e:\vclntavi\video_avi.ini c:\vclntavi\video_avi.ini

copy e:\inttest\mmf\vclntavi\xvidonly.avi c:\vclntavi\xvidonly.avi
copy e:\inttest\mmf\vclntavi\aud_vid.avi c:\vclntavi\aud_vid.avi

testframework tsi_mmf_vclntavi_play_secdisp.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\vclntavi\*.*
