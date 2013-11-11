@echo off
REM wspointershifttest_run.bat
REM Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
REM All rights reserved.
REM This component and the accompanying materials are made available
REM under the terms of "Eclipse Public License v1.0"
REM which accompanies this distribution, and is available
REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
REM 
REM Initial Contributors:
REM Nokia Corporation - initial contribution.
REM 
REM Contributors:
REM 
REM Description:
REM 
setlocal

set WSINI_ARM=%EPOCROOT%epoc32\data\z\system\data\wsini.ini
set WSINI_WINSCW=z\system\data\wsini.ini
set WSINI_TEST_WINS=%EPOCROOT%epoc32\data\z\wstest\wsini_pointershift_wins.ini
set WSINI_TEST_ARM=%EPOCROOT%epoc32\data\z\wstest\wsini_pointershift.ini
set EPOCINI_TEST=%EPOCROOT%epoc32\data\z\wstest\epoc_pointershift.ini
if not defined EPOC_INI set EPOC_INI=%EPOCROOT%epoc32\data\epoc.ini

if /i "%1"=="uninstall" goto :uninstall
if /i "%1"=="install" goto :install

:help
	echo.
	echo Description:
	echo This script substitutes in the correct epoc ini and window server ini file for the test
	echo arg1: It takes argument "install or "uninstall"
	goto :done

rem *** START STANDARD FILE-REPLACEMENT FUNCTIONS ***
:replace
	set DST=%1
	set SRC=%2
	echo INFO: Replacing %DST% with %SRC%
	copy /y %DST% %DST%.original >NUL
	if defined SRC (
		copy /y %SRC% %DST% >NUL
		if not exist %DST% echo ERROR: Copy failed!
	)
	goto :EOF
:restore
	echo INFO: Restoring %1 
	if not exist %1.original echo ERROR: %1.original does not exist
	copy /y %1.original %1 >NUL
	del %1.original >NUL
	goto :EOF
:replace_em
	if defined EMULATOR_DATA_DIR (
		call :replace %EMULATOR_DATA_DIR%\%1 %2
	) else (
		call :replace %EPOCROOT%epoc32\release\winscw\udeb\%1 %2
		call :replace %EPOCROOT%epoc32\release\winscw\urel\%1 %2
	)
	goto :EOF
:replace_em2
	if defined EMULATOR_DATA_DIR (
		call :replace %EMULATOR_DATA_DIR%\%1 %EMULATOR_DATA_DIR%\%2
	) else (
		call :replace %EPOCROOT%epoc32\release\winscw\udeb\%1 %EPOCROOT%epoc32\release\winscw\udeb\%2
		call :replace %EPOCROOT%epoc32\release\winscw\urel\%1 %EPOCROOT%epoc32\release\winscw\urel\%2
	)
	goto :EOF
:restore_em
	if defined EMULATOR_DATA_DIR (
		call :restore %EMULATOR_DATA_DIR%\%1
	) else (
		call :restore %EPOCROOT%epoc32\release\winscw\udeb\%1
		call :restore %EPOCROOT%epoc32\release\winscw\urel\%1
	)
	goto :EOF
rem *** END STANDARD FILE-REPLACEMENT FUNCTIONS ***


:install
	echo Replacing files with test versions.
	call :replace    %EPOC_INI%		%EPOCINI_TEST%
	call :replace    %WSINI_ARM%	%WSINI_TEST_ARM%
	call :replace_em %WSINI_WINSCW% %WSINI_TEST_WINS%
	if defined EPOC_HOST_CPU echo HostCPU %EPOC_HOST_CPU% >> %EPOC_INI%
	goto :done

:uninstall
	echo Restoring files
	call :restore    %EPOC_INI%
	call :restore    %WSINI_ARM%
	call :restore_em %WSINI_WINSCW%


:done
endlocal
