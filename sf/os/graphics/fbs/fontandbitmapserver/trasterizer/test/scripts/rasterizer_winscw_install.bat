rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


@echo off
setlocal

call :init %1
call :setup
call :listfiles

goto :skipFunctions

:help
	echo.
	echo Test Extended Bitmap rasterizer installer for winscw
	echo.
	echo Usage: rasterizer_winscw_install.bat [install_flag]
	echo Where:
	echo install_flag:    install or uninstall
	echo.
goto :EOF

:init
	set INSTALL_FLAG=%1
	set TEST_EB_RASTERIZER=fbsrasterizer_test.dll
goto :EOF

:setup
	if "%INSTALL_FLAG%"=="install" call :rasterizer_install & goto :EOF
	if "%INSTALL_FLAG%"=="uninstall" call :rasterizer_uninstall & goto :EOF
	call :help
goto :EOF

:rasterizer_install
	if not exist "%EPOC_INI%" echo ERROR: %EPOC_INI% does not exist & call :help & goto :EOF

	if exist "%EPOC_INI%.rasterizer.bak" del %EPOC_INI% >NUL
	if exist "%EPOC_INI%.rasterizer.bak" copy %EPOC_INI%.rasterizer.bak %EPOC_INI% >NUL
	if not exist "%EPOC_INI%.rasterizer.bak" copy %EPOC_INI% %EPOC_INI%.rasterizer.bak >NUL
	echo INFO: Modifying %EPOC_INI% for test Extended Bitmap rasterizer
	echo. >> %EPOC_INI%
	echo FBSRASTERIZER_DLL %TEST_EB_RASTERIZER% >> %EPOC_INI%
goto :EOF

:rasterizer_uninstall
	if exist "%EPOC_INI%.rasterizer.bak" del %EPOC_INI% >NUL
	echo INFO: Restoring %EPOC_INI%.rasterizer.bak to %EPOC_INI%
	if exist "%EPOC_INI%.rasterizer.bak" copy %EPOC_INI%.rasterizer.bak %EPOC_INI% >NUL
	if exist "%EPOC_INI%.rasterizer.bak" del %EPOC_INI%.rasterizer.bak >NUL
goto :EOF

:listfiles

echo =======================================
echo files after %0 %1
echo =======================================
echo epoc.ini:	%EPOC_INI%
echo =======================================
type %EPOC_INI%
echo =======================================

goto :EOF

:skipFunctions

endlocal
