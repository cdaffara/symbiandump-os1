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
rem

del E:\Logs\TestResults\TSU_DVBH_HAI*.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** DVBH HAI Tests ***************
REM ***********************************************

testframework tsu_dvbh_hai.script
testframework tsu_dvbh_hai_panic_debug.script
copy c:\Logs\TestResults\*.htm e:\Logs\TestResults

