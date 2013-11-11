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

del E:\Logs\TestResults\tsu_mmf_basecl.htm
del E:\Logs\TestResults\tsu_mmf_basecl_alloc.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** BASECL TESTS *****************
REM ***********************************************

copy E:\unittest\mmf\basecl\*.refsnd C:\

testframework tsu_mmf_basecl.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del C:\Reference100Hz*.refsnd
del C:\Converted100Hz*.raw

testframework tsu_mmf_basecl_alloc.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\
