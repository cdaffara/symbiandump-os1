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

del E:\Logs\TestResults\tsu_icl_cod_04*.*
md E:\Logs
md E:\Logs\TestResults
md c:\TSU_ICL_COD_04\
md c:\TSU_ICL_COD_04b\

REM ***********************************************
REM **************** COD04 TESTS ******************
REM ***********************************************

copy E:\unittest\icl\TSU_ICL_COD_04b\*.* c:\TSU_ICL_COD_04b\

testframework -q tsu_icl_cod_04b.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TSU_ICL_COD_04b\*.*

copy E:\unittest\icl\TSU_ICL_COD_04\*.* c:\TSU_ICL_COD_04\

testframework -q tsu_icl_cod_04.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_cod_04_alloc.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TSU_ICL_COD_04\*.*
