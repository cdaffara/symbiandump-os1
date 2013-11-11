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

del E:\Logs\TestResults\TSI_ICL_BTRANS_01*.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** BTRANS TESTS ****************
REM ***********************************************

md c:\TImage\

copy E:\inttest\ICL\TImage\bmp8bpp_8.bmp c:\TImage\bmp8bpp_8.bmp
copy E:\inttest\ICL\TImage\imagevid_8.mbm c:\TImage\imagevid_8.mbm

testframework -q TSI_ICL_BTRANS_01.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q TSI_ICL_BTRANS_01_ALLOC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q TSI_ICL_BTRANS_01_PANIC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TImage\*.*