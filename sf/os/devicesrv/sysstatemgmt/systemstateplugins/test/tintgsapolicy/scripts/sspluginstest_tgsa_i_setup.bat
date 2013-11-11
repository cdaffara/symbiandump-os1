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
@echo off
setlocal

:: Initialise local variables
set COMMAND_CHOICE=%1
set RSC_TEST_LOCATION=Z\private\2000d75b\test
set RSC_LOCATION=Z\private\2000d75b\startup\0

set TEST_CASE=%2
call :%TEST_CASE%
set TEST_RSC=%RSC_TEST_LOCATION%\%3
set NEWTEST_RSC=%RSC_LOCATION%\%3

set EPOCWIND=%TEMP%\epocwind.out

goto :%COMMAND_CHOICE%

:install
	if exist %NEWTEST_RSC% del %NEWTEST_RSC%

	if not exist "%TEST_RSC%" echo Ssplugins Test: ERROR - test resource file does not exist & goto :EOF

	copy %TEST_RSC% %NEWTEST_RSC% > NUL
goto :EOF

:uninstall
	call :restore
	
	if /i "%TEST_FLAG%"=="checkEpocWind" call :test
goto :EOF

:execTimeOut
	set TIMEOUT=300
	\ExecTimeOut.exe "testexecute.exe z:\sspluginstest\sspluginstest_%TEST_CASE%.script" %TIMEOUT%
goto :EOF

:testExecute
	testexecute.exe z:\sspluginstest\sspluginstest_%TEST_CASE%.script
goto :EOF

:test
	call z\sspluginstest\sspluginstest_tgsa_i_checkepocwind.bat %TEST_CASE%

	if exist %TEMP%\epocwind.out copy %TEMP%\epocwind.out %TEMP%\epocwind.out.temp > NUL

	:: If using SYMBIAN overnight DABS test scripts, then utilise ExecTimeOut to ensure
	:: execution does not exceed expected execution time.
	if exist \ExecTimeOut.exe call :execTimeOut

	:: If simply executing manually, then call testexecute directly.
	if not exist \ExecTimeOut.exe call :testExecute

	if exist %TEMP%\epocwind.out.temp copy %TEMP%\epocwind.out.temp %TEMP%\epocwind.out > NUL
	if exist %TEMP%\epocwind.out.temp del %TEMP%\epocwind.out.temp
goto :EOF

:restore
	if exist %NEWTEST_RSC% del %NEWTEST_RSC% > NUL
goto :EOF

:statetranfromstartup
:statetranfromnormal
:statetranfromshutdown
:statetrantoshutdown
:statetranfromstartuptoshutdown
:duplicatesubstate
	set TEST_FLAG=checkEpocWind
goto :EOF        

endlocal
