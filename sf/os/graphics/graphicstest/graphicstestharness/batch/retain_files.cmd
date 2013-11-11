rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

rem Copy the files that we want to keep
set SOURCE=%EMULATOR_C_DIR%\%1
set TARGET=%EPOCROOT%epoc32\data\retained_files\%2

echo Source: %SOURCE%
echo Target: %TARGET%
for %%i in (%SOURCE%\*.mbm) do bmconv /u %%i %%i.bmp

xcopy %SOURCE%\*.* %TARGET% /EIR

rem then call the real post script
call %EMULATOR_DATA_DIR%\%3 %4 %5 %6 %7 %8 %9
