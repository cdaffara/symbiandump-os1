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

del E:\Logs\TestResults\tsu_icl_pfm_02*.*
md E:\Logs
md E:\Logs\TestResults
md c:\tsu_icl_pfm_02\

REM ***********************************************
REM **************** PFM TESTS ******************
REM ***********************************************

copy E:\unittest\icl\tsu_icl_pfm_02\*.* c:\tsu_icl_pfm_02\

testframework -q tsu_icl_pfm_02.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\tsu_icl_pfm_02\*.*
