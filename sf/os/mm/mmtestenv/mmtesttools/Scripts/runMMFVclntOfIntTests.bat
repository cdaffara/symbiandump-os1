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
rem runMMFVclntIntTests.bat
rem 
rem

del E:\Logs\TestResults\TSI_MMF_VCLNT_OF*.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** VCLNT TESTS *****************
REM ***********************************************

md c:\VclntITestData\

copy E:\inttest\MMF\VclntITestData\LongVideo.dummy c:\VclntITestData\LongVideo.dummy
copy E:\inttest\MMF\VclntITestData\ShortVideo.dummy c:\VclntITestData\ShortVideo.dummy
copy E:\inttest\MMF\VclntITestData\DRMVideo.drm c:\VclntITestData\DRMVideo.drm

copy E:\inttest\MMF\VclntITestData\textInput.dummy c:\VclntITestData\textInput.dummy
copy E:\inttest\MMF\VclntITestData\textOutput.dummy c:\VclntITestData\textOutput.dummy


testframework TSI_MMF_VCLNT_OF.script
testframework TSI_MMF_VCLNT_OF_ALLOC.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\VclntITestData\*.*
