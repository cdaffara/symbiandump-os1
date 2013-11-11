@echo off
rem Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
rem @internalComponent - Internal Symbian
rem 
rem
set nonnga=false
if not defined EPOC_INI set EPOC_INI=%EPOCROOT%epoc32\data\epoc.ini
if /i "%2"=="nonnga" set nonnga=true
if /i "%3"=="nonnga" set nonnga=true
if "%nonnga%"=="true" echo Configuring for Non-NGA
if /i "%1"=="uninstall" goto :uninstall
if /i "%2"=="changetracking" goto :install_changetracking
rem ================ install ================

:install
echo Replacing WSINI.INI files with test versions.
if "%EMULATOR_DATA_DIR%"=="" (
	call :doinstall %EPOCROOT%epoc32\release\winscw\udeb multiscreen %2 %3
	call :doinstall %EPOCROOT%epoc32\release\winscw\urel multiscreen %2 %3
) else (
	call :doinstall %EMULATOR_DATA_DIR% multiscreen %2 %3
)

rem If EPOC.INI isn't already backed up then backup EPOC.INI to EPOC.INI.bak
if not exist %EPOC_INI%.bak copy /y %EPOC_INI% %EPOC_INI%.bak

echo Editing EPOC.INI to use multiscreen.
echo. >> %EPOC_INI%
echo StartupMode ^7>> %EPOC_INI%
echo _NewScreen_ >> %EPOC_INI%
echo ScreenWidth 640 >> %EPOC_INI%
echo ScreenHeight 240 >> %EPOC_INI%
goto :EOF


:doinstall
rem If WSINI.INI(s) aren't already backed up then backup WSINI.INI(s)
if not exist %1\z\system\data\wsini.bak   if exist %1\z\system\data\wsini.ini   copy /y %1\z\system\data\wsini.ini   %1\z\system\data\wsini.bak >nul
if not exist %1\z\resource\data\wsini.bak if exist %1\z\resource\data\wsini.ini copy /y %1\z\resource\data\wsini.ini %1\z\resource\data\wsini.bak >nul

rem Back up ECONS.DLL
if exist %1\econs.dll copy /y %1\econs.dll %1\econs-temp.dll

rem Rename WSECONS.DLL to ECONS.DLL
if exist %1\wsecons.dll copy /y %1\wsecons.dll %1\econs.dll

rem Replace WSINI.INI with test version
if "%nonnga%"=="true" goto :doinstall_nonnga
if exist %1\z\system\data\wsini.ini   copy /y %1\z\wstest\wservu_multiscreen.ini %1\z\system\data\wsini.ini
if exist %1\z\resource\data\wsini.ini copy /y %1\z\wstest\wservu_multiscreen.ini %1\z\resource\data\wsini.ini
goto :doinstall_editwsini

:doinstall_nonnga
if exist %1\z\system\data\wsini.ini   copy /y %1\z\wstest\wservu_multiscreen-nonnga.ini %1\z\system\data\wsini.ini
if exist %1\z\resource\data\wsini.ini copy /y %1\z\wstest\wservu_multiscreen-nonnga.ini %1\z\resource\data\wsini.ini

:doinstall_editwsini
rem Edit WSINI.INI to run tests automatically/multiscreen
if exist %1\z\system\data\wsini.ini   perl %1\z\wstest\wsini-writer.pl %1\z\system\data\wsini.ini %2 %3 %4
if exist %1\z\resource\data\wsini.ini perl %1\z\wstest\wsini-writer.pl %1\z\resource\data\wsini.ini %2 %3 %4

goto :EOF

rem ================ install changetracking ================

:install_changetracking
echo Replacing WSINI.INI files with test changetracking versions.
if "%EMULATOR_DATA_DIR%"=="" (
	call :doinstall_changetracking %EPOCROOT%epoc32\release\winscw\udeb multiscreen %2 %3
	call :doinstall_changetracking %EPOCROOT%epoc32\release\winscw\urel multiscreen %2 %3
) else (
	call :doinstall_changetracking %EMULATOR_DATA_DIR% multiscreen %2 %3
)

rem If EPOC.INI isn't already backed up then backup EPOC.INI to EPOC.INI.bak
if not exist %EPOC_INI%.bak copy /y %EPOC_INI% %EPOC_INI%.bak >nul

echo Editing EPOC.INI to use multiscreen.
echo. >> %EPOC_INI%
echo StartupMode ^7>> %EPOC_INI%
echo _NewScreen_ >> %EPOC_INI%
echo ScreenWidth 640 >> %EPOC_INI%
echo ScreenHeight 240 >> %EPOC_INI%
goto :EOF

:doinstall_changetracking

rem Back up ECONS.DLL
if exist %1\econs.dll copy /y %1\econs.dll %1\econs-temp.dll

rem Rename WSECONS.DLL to ECONS.DLL
if exist %1\wsecons.dll copy /y %1\wsecons.dll %1\econs.dll

rem Back up WSINI.INI
if not exist %1\z\system\data\wsini.bak   if exist %1\z\system\data\wsini.ini   copy /y %1\z\system\data\wsini.ini %1\z\system\data\wsini.bak >nul
if not exist %1\z\resource\data\wsini.bak if exist %1\z\resource\data\wsini.ini copy /y %1\z\resource\data\wsini.ini %1\z\resource\data\wsini.bak >nul

rem Replace WSINI.INI with test changetracking version
if exist %1\z\system\data\wsini.ini   copy /y %1\z\wstest\wservu_multiscreen_changetracking.ini %1\z\system\data\wsini.ini >nul
if exist %1\z\resource\data\wsini.ini copy /y %1\z\wstest\wservu_multiscreen_changetracking.ini %1\z\resource\data\wsini.ini >nul

rem Edit WSINI.INI to run tests automatically/multiscreen
if exist %1\z\system\data\wsini.ini perl %1\z\wstest\wsini-writer.pl %1\z\system\data\wsini.ini %2 %3 %4
if exist %1\z\resource\data\wsini.ini perl %1\z\wstest\wsini-writer.pl %1\z\resource\data\wsini.ini %2 %3 %4

goto :EOF

rem ================ Uninstall ==============

:uninstall
echo Restoring WSINI.INI files.
if "%EMULATOR_DATA_DIR%"=="" (
	call :douninstall %EPOCROOT%epoc32\release\winscw\udeb
	call :douninstall %EPOCROOT%epoc32\release\winscw\urel
) else (
	call :douninstall %EMULATOR_DATA_DIR%
)

rem If EPOC.INI was already backed up then restore EPOC.INI from EPOC.INI.bak
if exist %EPOC_INI%.bak (
	echo Restoring EPOC.INI from backup
	copy /y %EPOC_INI%.bak %EPOC_INI% >nul
	del /f /q %EPOC_INI%.bak >nul
)
goto :EOF


:douninstall

rem If WSINI.INI(s) were already backed up then restore WSINI.INI
if exist %1\z\system\data\wsini.bak (
copy /y %1\z\system\data\wsini.bak   %1\z\system\data\wsini.ini >nul
del  /f /q %1\z\system\data\wsini.bak >nul
)
if exist %1\z\resource\data\wsini.bak (
copy /y %1\z\resource\data\wsini.bak %1\z\resource\data\wsini.ini >nul
del  /f /q %1\z\resource\data\wsini.bak >nul
)

echo Restoring backedup ECONS.DLL
if exist %1\econs-temp.dll copy /y %1\econs-temp.dll %1\econs.dll
goto :EOF

