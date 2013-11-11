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

:: @echo off
setlocal

:: *******************************************************************
:: Any ADDITIONS or DELETIONS to the test cases in this script MUST
:: be copied to the partner script that is used on hardware:
:: ..\hardware\sysstarttest_amastarter_run.bat
:: *******************************************************************


:: SysStart AmaStarter tests
call :test invalidsid
call :test ssc tamastarter_amaadder.rsc
call :test standalone
call :test startl

goto :skipFunctions

:test
	set TEST_CASE=%1
	set TEST_RSC=%2
	if "%TEST_RSC%" == "" call :normal %TEST_CASE%
	if "%TEST_RSC%" NEQ "" call :setup %TEST_CASE% %TEST_RSC%
goto :EOF

:normal
	set TEST_CASE=%1
	testexecute.exe z:\sysstarttest\sysstarttest_amastarter_%TEST_CASE%.script
goto :EOF

:setup
	set TEST_CASE=%1
	set TEST_RSC=%2
	call z\sysstarttest\sysstarttest_amastarter_setup.bat install %TEST_RSC%
	testexecute.exe z:\sysstarttest\sysstarttest_amastarter_%TEST_CASE%.script
	call z\sysstarttest\sysstarttest_amastarter_setup.bat uninstall %TEST_RSC%
goto :EOF

:skipFunctions 

endlocal
