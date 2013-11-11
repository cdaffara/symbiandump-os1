rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
testexecute.exe  z:\fbstest\fbstest_t_all.script
@echo off

md e:\logs
md e:\logs\testexecute
copy c:\logs\testexecute\fbstest_t_*.htm e:\logs\testexecute\
copy c:\logs\testexecute\testresults.htm e:\logs\testexecute\fbsresults.htm
