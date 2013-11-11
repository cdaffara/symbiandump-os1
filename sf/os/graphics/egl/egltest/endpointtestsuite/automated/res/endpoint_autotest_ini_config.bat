@echo off


REM Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

REM Batch file used for installing the correct environment (epoc.ini, wsini.ini,
REM testexecute.ini) for running the automatic egl endpoint tests that require the 
REM automated test renderstage in wserv. This file does the correct thing in mistral.
REM
REM Commands:
REM endpoint_autotest_ini_config.bat install
REM endpoint_autotest_ini_config.bat uninstall 


REM Constants.
set WS_FILENAME=wsini.ini
set WS_ALT_FILENAME=wsini_minigui.ini
set WS_ALT2_FILENAME=mingui_wsini.ini
set WS_REPLACEMENT=endpoint_autotest_wsini.ini

set EMULATOR_PATH_UDEB=%EPOCROOT%epoc32\release\winscw\udeb
set EMULATOR_PATH_UREL=%EPOCROOT%epoc32\release\winscw\urel
set HARDWARE_PATH=%EPOCROOT%epoc32\data


if /i "%1"=="uninstall" goto :uninstall
if /i "%1"=="install" goto :install
if /i "%1"=="help" goto :help


:help
	echo.
	echo Description:
	echo This script installs the correct environment (epoc.ini, wsini.ini, testexecute.ini) for the automatic egl endpoint tests.
	echo.It takes the argument "install or "uninstall"
goto :EOF


:move
	set SRC=%1
	set DST=%2
	if exist %SRC% copy %SRC% %DST% >NUL
	if exist %SRC% del %SRC% >NUL
	goto :EOF

	
:copy
	set SRC=%1
	set DST=%2
	if exist %SRC% copy %SRC% %DST% >NUL
	goto :EOF

	
:backupAndInstallWsini
	set ORIGINAL_FILE=%1
	REM Save original and copy the modified wsini file to the specified location.
	call :copy %ORIGINAL_FILE% %ORIGINAL_FILE%.backup
	if defined EMULATOR_DATA_DIR (
		call :copy %EMULATOR_DATA_DIR%\z\egltest\%WS_REPLACEMENT% %ORIGINAL_FILE%
	) else (
		REM We assume that the file is in the same directory as this batch file.
		call :copy %~dp0\%WS_REPLACEMENT% %ORIGINAL_FILE%
	)
	goto :EOF


:backupAndInstallEpoc
	set ORIGINAL_FILE=%1
	REM Save original and add minigui to epoc.ini
	perl -p -i.backup -e "s/^.*SYMBIAN_(GRAPHICS|BASE)_USE_GCE.*$//i" %ORIGINAL_FILE%
	echo. >> %ORIGINAL_FILE%
	echo SYMBIAN_GRAPHICS_USE_GCE ON >> %ORIGINAL_FILE%
	echo SYMBIAN_BASE_USE_GCE ON >> %ORIGINAL_FILE%
	echo ScreenWidth 320 >> %ORIGINAL_FILE%
	echo ScreenHeight 480 >> %ORIGINAL_FILE%
	echo ScreenOffsetX 0 >> %ORIGINAL_FILE%
	echo ScreenOffsetY 0 >> %ORIGINAL_FILE%
	echo FasciaBitmap null >> %ORIGINAL_FILE%
	echo minigui >> %ORIGINAL_FILE%
	echo. >> %ORIGINAL_FILE%
	goto :EOF
	

:backupAndInstallTestexecute
	set ORIGINAL_FILE=%1
	REM Disable SystemStarter in testexecute.ini 
	perl -p -i.backup -e "s/SystemStarter.*ON.*$/SystemStarter = OFF/i" %ORIGINAL_FILE%
	goto :EOF


:install
	REM Start with the original files if we previously missed an uninstall.
	call :uninstall
	
	REM Install epoc.ini.
	if defined EPOC_INI (
		call :backupAndInstallEpoc %EPOC_INI%
	) else (
		call :backupAndInstallEpoc %EPOCROOT%epoc32\data\epoc.ini
	)
	
	REM Install testexecute.ini.
	if defined EMULATOR_C_DIR (
		call :backupAndInstallTestexecute %EMULATOR_C_DIR%\system\data\testexecute.ini
	) else (
		call :backupAndInstallTestexecute %EPOCROOT%epoc32\winscw\c\system\data\testexecute.ini
		call :backupAndInstallTestexecute %EPOCROOT%epoc32\data\z\system\data\testexecute.ini
	)
	
	REM Must use different paths if running on Mistral (indicated by EMULATOR_DATA_DIR).
	REM If not Mistral, change emulator urel and udeb variants and hardware locations. 
	if defined EMULATOR_DATA_DIR (
		call :backupAndInstallWsini %EMULATOR_DATA_DIR%\z\system\data\%WS_FILENAME%
	) else (
		call :backupAndInstallWsini %EMULATOR_PATH_UDEB%\z\system\data\%WS_FILENAME%
		call :backupAndInstallWsini %EMULATOR_PATH_UREL%\z\system\data\%WS_FILENAME%
		call :backupAndInstallWsini %HARDWARE_PATH%\z\system\data\%WS_FILENAME%
		call :backupAndInstallWsini %HARDWARE_PATH%\z\%WS_ALT_FILENAME%
		call :backupAndInstallWsini %HARDWARE_PATH%\z\system\data\%WS_ALT2_FILENAME%
	)
	
	goto :EOF

	
:uninstall
	REM Uninstall epoc.ini.
	if defined EPOC_INI (
		call :move %EPOC_INI%.backup %EPOC_INI%
	) else (
		call :move %EPOCROOT%epoc32\data\epoc.ini.backup %EPOCROOT%epoc32\data\epoc.ini
	)
	
	REM Uninstall testexecute.ini.
	if defined EMULATOR_C_DIR (
		call :move %EMULATOR_C_DIR%\system\data\testexecute.ini.backup %EMULATOR_C_DIR%\system\data\testexecute.ini
	) else (
		call :move %EPOCROOT%epoc32\winscw\c\system\data\testexecute.ini.backup %EPOCROOT%epoc32\winscw\c\system\data\testexecute.ini
		call :move %EPOCROOT%epoc32\data\z\system\data\testexecute.ini.backup %EPOCROOT%epoc32\data\z\system\data\testexecute.ini
	)

	REM Must use different paths if running on Mistral (indicated by EMULATOR_DATA_DIR).
	REM If not Mistral, change urel and udeb variants.
	if defined EMULATOR_DATA_DIR (
		REM Restore the backed up files.
		call :move %EMULATOR_DATA_DIR%\z\system\data\%WS_FILENAME%.backup %EMULATOR_DATA_DIR%\z\system\data\%WS_FILENAME%
	) else (
		REM Restore the backed up files.
		call :move %EMULATOR_PATH_UDEB%\z\system\data\%WS_FILENAME%.backup %EMULATOR_PATH_UDEB%\z\system\data\%WS_FILENAME%
		call :move %EMULATOR_PATH_UREL%\z\system\data\%WS_FILENAME%.backup %EMULATOR_PATH_UREL%\z\system\data\%WS_FILENAME%
		call :move %HARDWARE_PATH%\z\system\data\%WS_FILENAME%.backup %HARDWARE_PATH%\z\system\data\%WS_FILENAME%
		call :move %HARDWARE_PATH%\z\%WS_ALT_FILENAME%.backup %HARDWARE_PATH%\z\%WS_ALT_FILENAME%
		call :move %HARDWARE_PATH%\z\system\data\%WS_ALT2_FILENAME%.backup %HARDWARE_PATH%\z\system\data\%WS_ALT2_FILENAME%
	)

	goto :EOF
