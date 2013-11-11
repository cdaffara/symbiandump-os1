@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@echo off
cls

set plat=%1
set build=%2

if $%plat%==$ set plat=WINS
if $%build%==$ set build=UDEB

:removelogs
if not exist \epoc32\%plat%\c\logs mkdir \epoc32\%plat%\c\logs
if not exist \epoc32\%plat%\c\logs\testresults mkdir \epoc32\%plat%\c\logs\testresults
if not exist \epoc32\%plat%\c\logs\netcon mkdir \epoc32\%plat%\c\logs\netcon
if exist \epoc32\%plat%\c\logs\netcon\netcon.txt del \epoc32\%plat%\c\logs\netcon\netcon.txt
if exist %temp%\EPOCWIND.OUT del %temp%\EPOCWIND.OUT

:build
echo.
echo Building Test Code...
echo.

if not exist .\abld.bat call bldmake bldfiles

call abld test clean %plat% %build% ts_queue
call abld test clean %plat% %build% ts_netcon
call abld test clean %plat% %build% ts_netconoom
call abld test build %plat% %build% ts_queue
call abld test build %plat% %build% ts_netcon
call abld test build %plat% %build% ts_netconoom

:run
echo.
echo Running Tests...
echo.
call \epoc32\release\%plat%\%build%\scheduletest.exe -a \epoc32\release\%plat%\%build%\z\testdata\scripts\ts_netcon.script

call \epoc32\release\%plat%\%build%\scheduletest.exe -a \epoc32\release\%plat%\%build%\z\testdata\scripts\ts_queue.script

call \epoc32\release\%plat%\%build%\scheduletest.exe -u ts_netconoom

:end

echo.
echo Test Results...
echo.
call tail \epoc32\%plat%\c\logs\testresults\ts_netcon.htm
start \epoc32\%plat%\c\logs\testresults\ts_netcon.htm
start \epoc32\%plat%\c\logs\testresults\ts_queue.htm
start \epoc32\%plat%\c\logs\testresults\ts_netconoom.htm
echo.

set plat=
set build=
