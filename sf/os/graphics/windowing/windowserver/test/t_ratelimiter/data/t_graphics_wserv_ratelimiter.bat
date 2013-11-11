REM t_graphics_wserv_ratelimiter.bat
REM Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
@echo off
setlocal

set OPT_CHANGETRACKING=
set OPT_SINGLESCREEN=
set OPT_MULTISCREEN=1
if "%2"=="changetracking" set OPT_CHANGETRACKING=1 & set OPT_MULTISCREEN=
if "%3"=="changetracking" set OPT_CHANGETRACKING=1 & set OPT_MULTISCREEN=
if "%2"=="singlescreen" set OPT_SINGLESCREEN=1
if "%3"=="singlescreen" set OPT_SINGLESCREEN=1
if "%2"=="multiscreen" set OPT_MULTISCREEN=1
if "%3"=="multiscreen" set OPT_MULTISCREEN=1


rem *** Choose which test files to use based on command options ***
set WSINI_ARM=%EPOCROOT%epoc32\data\z\system\data\wsini.ini
set WSINI_WINS=z\system\data\wsini.ini

set CT=
if defined OPT_CHANGETRACKING set CT=_changetracking
set WSINI_TEST=%EPOCROOT%epoc32\data\z\wstest\ratelimiter\wsini%CT%.ini
set EPOCINI_TEST=%EPOCROOT%epoc32\data\z\wstest\ratelimiter\epoc.ini
if defined OPT_SINGLESCREEN (
	set WSINI_TEST=%EPOCROOT%epoc32\data\z\wstest\ratelimiter\wsini_hw_singlescreen%CT%.ini
	set EPOCINI_TEST=%EPOCROOT%epoc32\data\z\wstest\ratelimiter\epoc_singlescreen.ini
)
if defined OPT_MULTISCREEN (
	set WSINI_TEST=%EPOCROOT%epoc32\data\z\wstest\ratelimiter\wsini_hw_multiscreen%CT%.ini
)


if /i "%1"=="uninstall" goto :uninstall
if /i "%1"=="install" goto :install

:help
	echo.
	echo Description:
	echo This script substitutes in the correct epoc ini and window server ini file for the test
	echo arg1: It takes argument "install or "uninstall"
	echo arg2: optional argument to use changetracking mode in wserv, the default mode is non changetracking  
	echo arg2: optional - may also specify singlescreen or multiscreen if changetracking argument is not used default is multiscreen
	echo arg3: optional - specifies singlescreen or multiscreen when changetracking is used
	goto :done

rem *** START STANDARD FILE-REPLACEMENT FUNCTIONS ***
:replace
	set DST=%1
	set SRC=%2
	echo INFO: Replacing %DST% with %SRC%
	copy /y %DST% %DST%.original >NUL
	if defined SRC copy /y %SRC% %DST% >NUL
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
	call :replace %EPOC_INI% %EPOCINI_TEST%
	call :replace %WSINI_ARM% %WSINI_TEST% 
	call :replace_em %WSINI_WINS% %WSINI_TEST%
	goto :done

:uninstall
	echo Restoring files
	call :restore %EPOC_INI%
	call :restore %WSINI_ARM%
	call :restore_em %WSINI_WINS%

:done
endlocal
