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
@echo on

set TESTEXECUTE_INI=
if defined EMULATOR_C_DIR (
	set TESTEXECUTE_INI=%EMULATOR_C_DIR%\system\data\testexecute.ini
) else (
	set TESTEXECUTE_INI=%EPOCROOT%epoc32\winscw\c\system\data\testexecute.ini
)

echo Backup EPOC.INI
if defined EPOC_INI (
	copy %EPOC_INI% %EPOC_INI%.mgbak
) else (
	copy %EPOCROOT%epoc32\data\epoc.ini %EPOCROOT%epoc32\data\epoc.ini.mgbak
)
echo Backup testexecute.ini
copy %TESTEXECUTE_INI% %TESTEXECUTE_INI%.mgbak
echo Backup wsini.ini
if defined EMULATOR_DATA_DIR (
	copy %EMULATOR_DATA_DIR%\z\system\data\wsini.ini %EMULATOR_DATA_DIR%\z\system\data\wsini.ini.mgbak
) else (
	copy %EPOCROOT%epoc32\release\winscw\udeb\z\system\data\wsini.ini %EPOCROOT%epoc32\release\winscw\udeb\z\system\data\wsini.ini.mgbak
	copy %EPOCROOT%epoc32\release\winscw\urel\z\system\data\wsini.ini %EPOCROOT%epoc32\release\winscw\urel\z\system\data\wsini.ini.mgbak
)


REM Run command passed as argument
if defined EMULATOR_DATA_DIR (
	call %EMULATOR_DATA_DIR%\%*
) else (
	call %EPOCROOT%epoc32\release\winscw\udeb\%*
)

REM Add minigui to epoc.ini 
echo INFO: Adding minigui to epoc.ini
if defined EPOC_INI (
	echo minigui >> %EPOC_INI%
	echo. >> %EPOC_INI%
) else (
	echo minigui >> %EPOCROOT%epoc32\data\epoc.ini
	echo. >> %EPOCROOT%epoc32\data\epoc.ini
)	

REM Disable SystemStarter in testexecute.ini 
echo INFO: Disabling SystemStarter in testexecute.ini

perl -p -i.bak -e "s/SystemStarter.*ON.*$/SystemStarter = OFF/i" %TESTEXECUTE_INI%

echo testexecute.ini:	%TESTEXECUTE_INI%
echo =======================================
type %TESTEXECUTE_INI%
echo =======================================

REM Modify wsini to use ESHELL
if defined EMULATOR_DATA_DIR (
	call %EMULATOR_DATA_DIR%\z\graphicstest\replace.py %EMULATOR_DATA_DIR%\z\system\data\wsini.ini WSHELL ESHELL
) else (
	call %EPOCROOT%epoc32\release\winscw\udeb\z\graphicstest\replace.py %EPOCROOT%epoc32\release\winscw\udeb\z\system\data\wsini.ini WSHELL ESHELL
	call %EPOCROOT%epoc32\release\winscw\urel\z\graphicstest\replace.py %EPOCROOT%epoc32\release\winscw\urel\z\system\data\wsini.ini WSHELL ESHELL
)
