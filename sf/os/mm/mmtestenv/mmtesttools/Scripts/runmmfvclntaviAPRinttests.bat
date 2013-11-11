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

del E:\Logs\TestResults\TSI_MMF_VCLNTAVI_APR.*
del E:\Logs\TestResults\TSI_MMF_VCLNTAVI_APR_ALLOC.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** VCLNTAVI TESTS **************
REM ***********************************************

md c:\vclntavi\
md c:\AclntITestData\

copy e:\inttest\mmf\vclntavi\aud_vid.avi c:\vclntavi\aud_vid.avi
copy e:\inttest\mmf\vclntavi\aud_vid_vprp.avi c:\vclntavi\aud_vid_vprp.avi
copy e:\inttest\MMF\AclntITestData\mainTst.wav c:\AclntITestData\mainTst.wav
copy e:\inttest\MMF\AclntITestData\mainTst3.wav c:\AclntITestData\mainTst3.wav

testframework tsi_mmf_vclntavi_apr.script
testframework tsi_mmf_vclntavi_apr_alloc.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\vclntavi\*.*
del c:\AclntITestData\*.*
