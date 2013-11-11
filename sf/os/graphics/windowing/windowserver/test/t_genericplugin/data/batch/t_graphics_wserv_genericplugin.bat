REM t_graphics_wserv_genericplugin.bat
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

set OPT_NGA=
if /i "%2"=="nga" set OPT_NGA=1
set OPT_SINGLE=
if /i "%3"=="single" set OPT_SINGLE=1
set OPT_CT=
if /i "%2"=="changetracking" set OPT_CT=1
if /i "%3"=="changetracking" set OPT_CT=1


set WSINI_ARM=%EPOCROOT%epoc32\data\z\system\data\wsini.ini
set WSINI_WINSCW=z\system\data\wsini.ini

set WSINI_TEST=wsini_nonnga.ini
if defined OPT_NGA (
	set WSINI_TEST=wsini_nga.ini
	if defined OPT_SINGLE set WSINI_TEST=wsini_nga_onescreen.ini
)
if defined OPT_CT set WSINI_TEST=wsini_changetracking.ini
set WSINI_TEST=%EPOCROOT%epoc32\data\z\wstest\genericplugin\%WSINI_TEST%



if /i "%1"=="uninstall" goto :uninstall
if /i "%1"=="install" goto :install


:help
	echo.
	echo Description:
	echo This script substitutes in the correct epoc ini and window server ini file for the test
	echo Usage:
	echo t_graphics_wserv_genericplugin install/uninstall [nga] [changetracking]
	echo t_graphics_wserv_genericplugin install/uninstall [nga] [single]
	echo 
	echo arg1: is either install or uninstall
	echo arg2: optional indicator for configuration, either nonnga, which is default or nga
	echo arg3: optional argument to use changetracking mode in wserv, the default mode is non changetracking  
	echo arg3: optional indicator for configuration, either two screens, which is default or one screen
	echo.
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
	echo Replacing files with test versions
	call :replace     %WSINI_ARM%     %WSINI_TEST%
	call :replace_em  %WSINI_WINSCW%  %WSINI_TEST%
	call :replace     %EPOC_INI%
	echo. >> %EPOC_INI%
	REM Uncomment the following two lines to run this test manually. This ensures that GCE mode is turned on for NGA tests.
	REM	if /i "%OPT_NGA%"=="true" echo SYMBIAN_GRAPHICS_USE_GCE on >> %EPOC_INI%
	REM	if /i "%OPT_NGA%"=="true" echo SYMBIAN_BASE_USE_GCE on >> %EPOC_INI%
	if defined OPT_SINGLE goto :consoleoutput
		echo _NewScreen_ >> %EPOC_INI%
		echo ScreenHeight 240 >> %EPOC_INI%
		echo ScreenWidth  320 >> %EPOC_INI%
:consoleoutput
	echo =======================================
	echo wsini.ini:	%WSINI_WINSCW%
	echo =======================================
	type %WSINI_TEST%
	echo =======================================
	echo epoc.ini:	%EPOC_INI%
	echo =======================================
	type %EPOC_INI%
	echo =======================================
	goto :done

:uninstall
	echo Restoring files
	call :restore     %WSINI_ARM%
	call :restore_em  %WSINI_WINSCW%
	call :restore     %EPOC_INI%

:done
endlocal
