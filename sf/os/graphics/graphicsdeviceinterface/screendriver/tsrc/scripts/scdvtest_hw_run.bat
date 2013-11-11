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
rem 
rem

@echo on

testexecute.exe  z:\scdvtest\scdvtest_t_lowlevel.script
testexecute.exe  z:\scdvtest\scdvtest_t_lowlevel1.script
testexecute.exe  z:\scdvtest\scdvtest_t_rwindows.script
testexecute.exe  z:\scdvtest\scdvtest_t_scaling.script
testexecute.exe  z:\scdvtest\scdvtest_t_scdv.script
testexecute.exe  z:\scdvtest\scdvtest_t_devorientation.script


@echo off
md e:\logs
md e:\logs\testexecute
copy c:\logs\testexecute\scdvtest_t_scdv*.htm e:\logs\testexecute\

