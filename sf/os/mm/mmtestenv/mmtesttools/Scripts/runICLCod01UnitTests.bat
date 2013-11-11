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

del E:\Logs\TestResults\tsu_icl_cod_01.*
del E:\Logs\TestResults\tsu_icl_cod_01_alloc.*
del E:\Logs\TestResults\tsu_icl_cod_01_Panic.*
del E:\Logs\TestResults\tsu_icl_cod_01b*.*
del E:\Logs\TestResults\tsu_icl_cod_01c*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM **************** COD01 TESTS ******************
REM ***********************************************

md c:\TImage\
md c:\TImage\anon\
md c:\Timage\prof\
md c:\TImage\ref\
md c:\TImage\00d4\
md c:\TImage\00d6\
md c:\TImage\bmp\
md c:\tsu_icl_cod_01\

copy E:\unittest\icl\TImage\*.* c:\TImage\
copy E:\unittest\icl\TImage\Anon\*.* c:\TImage\Anon\
copy E:\unittest\icl\TImage\prof\*.* c:\TImage\prof\
copy E:\unittest\icl\TImage\ref\*.* c:\TImage\ref\
copy E:\unittest\icl\TImage\00d4\*.* c:\TImage\00d4\
copy E:\unittest\icl\TImage\00d6\*.* c:\TImage\00d6\
copy E:\unittest\icl\TImage\bmp\*.* c:\TImage\bmp\
copy E:\unittest\icl\tsu_icl_cod_01\*.* c:\tsu_icl_cod_01\

testframework -q tsu_icl_cod_01.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_cod_01_Panic.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_cod_01b_alloc.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_cod_01c_alloc.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

REM***** This test is long so always run at the end ************
testframework -q tsu_icl_cod_01_alloc.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TImage\anon\*.*
del c:\Timage\prof\*.*
del c:\TImage\ref\*.*
del c:\TImage\00d4\*.*
del c:\TImage\00d6\*.*
del c:\TImage\bmp\*.*
del c:\TImage\*.*
del c:\tsu_icl_cod_01\*.*
