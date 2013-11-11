REM tscreenconstruct.bat
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


set WSINI_ARM=%EPOCROOT%epoc32\data\z\system\data\wsini.ini
set WSINI_WINSCW=z\system\data\wsini.ini

if /i "%1"=="uninstall" goto :uninstall
if /i "%1"=="install" goto :install

:help
	echo.
	echo Description:
	echo This script modifies epoc ini and substitutes in the correct window server ini file for the test
	echo Usage:
	echo tscreenconstruct.bat install/uninstall [color] [nga] [changetracking]
	echo 
	echo arg1: is either install or uninstall
	echo arg2: is one of "color4k" "color64k" "color16m" "screenconstruct" "scrmodes"
	echo arg3: optional indicator for configuration, either nonnga, which is default or nga
	echo arg4: optional argument to use changetracking mode in wserv, the default mode is non changetracking  
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
	set OPT_CT=
	if /i "%3"=="changetracking"		set OPT_CT=_changetracking
	if /i "%4"=="changetracking"		set OPT_CT=_changetracking
	set OPT_NGA=
	if /i "%3"=="nga" set OPT_NGA=1

	set WSINI_TEST=wsini%OPT_CT%.ini
	if /i "%2"=="screenconstruct"	set WSINI_TEST=wsini_screenconstruct%OPT_CT%.ini
	if /i "%2"=="scrmodes"			set WSINI_TEST=wsini_scrmodes%OPT_CT%.ini
	set WSINI_TEST=%EPOCROOT%epoc32\data\z\wstest\screenconstruct\%WSINI_TEST%

	set EPOC_COLORDEPTH=color16m
	if /i "%2"=="color4k" set EPOC_COLORDEPTH=color4k
	if /i "%2"=="color64k" set EPOC_COLORDEPTH=color64k
	if /i "%2"=="color16m" set EPOC_COLORDEPTH=color16m

	echo Replacing files with test versions.
	call :replace     %EPOC_INI%
	if defined OPT_NGA (
		echo SYMBIAN_GRAPHICS_USE_GCE on >> %EPOC_INI%
		echo SYMBIAN_BASE_USE_GCE on >> %EPOC_INI%
	)
	echo COLORDEPTH %EPOC_COLORDEPTH% >> %EPOC_INI%
	echo _NewScreen_ >> %EPOC_INI%
	echo ScreenHeight 640 >> %EPOC_INI%
	echo ScreenWidth  320 >> %EPOC_INI%

	call :replace     %WSINI_ARM%      %WSINI_TEST%
	call :replace_em  %WSINI_WINSCW%   %WSINI_TEST%
	goto :done

:uninstall
	echo Restoring files
	call :restore     %EPOC_INI%
	call :restore     %WSINI_ARM%
	call :restore_em  %WSINI_WINSCW%

:done
endlocal