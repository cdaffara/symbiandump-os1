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
rem

del E:\Logs\TestResults\TSU_ICL_FRM_01*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM ***************** FRM TESTS *******************
REM ***********************************************

md c:\TSU_ICL_FRM_01\

copy E:\unittest\icl\frm\*.* c:\TSU_ICL_FRM_01\

testframework -q TSU_ICL_FRM_01.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_frm_01b.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

REM*****Always run this at the end************
REM*****************LONG!!!!******************
testframework -q TSU_ICL_FRM_01_ALLOC.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TSU_ICL_FRM_01\*.*
