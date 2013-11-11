@echo off
rem Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem @file
rem @test
rem @internalComponent - Internal Symbian test code
rem Run this batch file if any updates are made to the outlineandshadow.ini. 
rem Changes to this file will be made from the test code when calculating new hashes
rem Preconditions: This file should be executed from the bitgdi\tbit\script folder (where) this file exists and will be effective only for outline and shadow tests
rem 
rem

echo Make sure that the generated ini in c:bitgditest is valid.
echo If not then do not copy.

copy %EMULATOR_C_DIR%\bitgditest\toutlineandshadow_winscw.ini . /-Y
copy toutlineandshadow_winscw.ini %EPOCROOT%epoc32\data\z\bitgditest\  /-Y
copy toutlineandshadow_winscw.ini %EPOCROOT%epoc32\release\winscw\udeb\z\bitgditest\ /-Y
copy toutlineandshadow_winscw.ini %EPOCROOT%epoc32\release\winscw\urel\z\bitgditest\ /-Y

@echo on
