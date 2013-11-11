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

REM Run command passed as argument
if defined EMULATOR_DATA_DIR (
	call %EMULATOR_DATA_DIR%\%*
) else (
	call %EPOCROOT%epoc32\release\winscw\udeb\%*
)

set TESTEXECUTE_INI=
if defined EMULATOR_C_DIR (
	set TESTEXECUTE_INI=%EMULATOR_C_DIR%\system\data\testexecute.ini
) else (
	set TESTEXECUTE_INI=%EPOCROOT%epoc32\winscw\c\system\data\testexecute.ini
)

echo Restore EPOC.INI
if defined EPOC_INI (
	if EXIST %EPOC_INI%.mgbak (
		del /f %EPOC_INI%
		copy %EPOC_INI%.mgbak %EPOC_INI%
		del /f %EPOC_INI%.mgbak
	)
) else (
	if EXIST %EPOCROOT%epoc32\data\epoc.ini.mgbak (
		del /f %EPOCROOT%epoc32\data\epoc.ini
		copy %EPOCROOT%epoc32\data\epoc.ini.mgbak %EPOCROOT%epoc32\data\epoc.ini
		del /f %EPOCROOT%epoc32\data\epoc.ini.mgbak
	)
)
echo Restore testexecute.ini
if EXIST %TESTEXECUTE_INI%.mgbak (
	del /f %TESTEXECUTE_INI%
	copy %TESTEXECUTE_INI%.mgbak %TESTEXECUTE_INI%
	del /f %TESTEXECUTE_INI%.mgbak
)
echo Restore wsini.ini
if defined EMULATOR_DATA_DIR (
	if EXIST %EMULATOR_DATA_DIR%\z\system\data\wsini.ini.mgbak (
		del /f %EMULATOR_DATA_DIR%\z\system\data\wsini.ini
		copy %EMULATOR_DATA_DIR%\z\system\data\wsini.ini.mgbak %EMULATOR_DATA_DIR%\z\system\data\wsini.ini
		del /f %EMULATOR_DATA_DIR%\z\system\data\wsini.ini.mgbak
	)
) else (
	if EXIST %EPOCROOT%epoc32\release\winscw\udeb\z\system\data\wsini.ini.mgbak (
		del /f %EPOCROOT%epoc32\release\winscw\udeb\z\system\data\wsini.ini
		copy %EPOCROOT%epoc32\release\winscw\udeb\z\system\data\wsini.ini.mgbak %EPOCROOT%epoc32\release\winscw\udeb\z\system\data\wsini.ini
		del /f %EPOCROOT%epoc32\release\winscw\udeb\z\system\data\wsini.ini.mgbak
	)
	if EXIST %EPOCROOT%epoc32\release\winscw\urel\z\system\data\wsini.ini.mgbak (
		del /f %EPOCROOT%epoc32\release\winscw\urel\z\system\data\wsini.ini
		copy %EPOCROOT%epoc32\release\winscw\urel\z\system\data\wsini.ini.mgbak %EPOCROOT%epoc32\release\winscw\urel\z\system\data\wsini.ini
		del /f %EPOCROOT%epoc32\release\winscw\urel\z\system\data\wsini.ini.mgbak
	)
)
