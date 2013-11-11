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

SETLOCAL
SET __PYTHON__=%SBS_HOME%\win32\python252\python.exe

REM Checking for __PYTHON__ existing is actually a double check that SBS_HOME is defined and it points
REM to something that looks like raptor installation. We piggy back onto the python that comes with raptor.
IF NOT EXIST %__PYTHON__% GOTO fallbacktoperl

%__PYTHON__% buildutils\mmbuild2.py %1 %2 %3 %4 %5 %6 %7 %8 %9

goto end

:fallbacktoperl
REM the old mmbuild script - intended now for use only if raptor is not installed
perl -S mmbuild.pl %1 %2 %3 %4 %5 %6 %7 %8 %9

:end
