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

del E:\Logs\TestResults\TSU_ICL_BTRANS_01*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM **************** BTRANS TESTS *****************
REM ***********************************************

md c:\Btrans\

copy E:\unittest\icl\EColor16IclBtrans.mbm       c:\EColor16IclBtrans.mbm
copy E:\unittest\icl\EColor16MIclBtrans.mbm      c:\EColor16MIclBtrans.mbm
copy E:\unittest\icl\EColor256IclBtrans.mbm      c:\EColor256IclBtrans.mbm
copy E:\unittest\icl\EColor4KIclBtrans.mbm       c:\EColor4KIclBtrans.mbm
copy E:\unittest\icl\EColor64kIclBtrans.mbm       c:\EColor64kIclBtrans.mbm
copy E:\unittest\icl\EGray16IclBtrans.mbm       c:\EGray16IclBtrans.mbm
copy E:\unittest\icl\EGray256IclBtrans.mbm       c:\EGray256IclBtrans.mbm
copy E:\unittest\icl\EGray2IclBtrans.mbm       c:\EGray2IclBtrans.mbm
copy E:\unittest\icl\EGray4IclBtrans.mbm       c:\EGray4IclBtrans.mbm
copy E:\unittest\icl\EColor16MUIclBtrans.mbm     c:\EColor16MUIclBtrans.mbm
copy E:\unittest\icl\xfile.mbm c:\xfile.mbm
copy E:\unittest\icl\test.mbm c:\test.mbm
copy E:\unittest\icl\test.mbm c:\btrans\test.mbm
copy E:\unittest\icl\Btrans\*.*  c:\btrans\ 

testframework -q tsu_icl_btrans_01.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_btrans_01_ALLOC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_btrans_01_Panic.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del C:\Btrans\*.*
del C:\*.*
