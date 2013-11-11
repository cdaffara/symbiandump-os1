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

call :test statetranfromnormal         tgsa_statetranfromnormal.rsc 
call :test statetranfromstartup         tgsa_statetranfromstartup.rsc 
call :test statetranfromshutdown       tgsa_statetranfromshutdown.rsc
call :test statetrantoshutdown         tgsa_statetrantoshutdown.rsc
call :test duplicatesubstate           tgsastartupcmdlistwithduplicatesubstate.rsc
call :test statetranfromstartuptoshutdown         tgsa_statetranfromstartuptoshutdown.rsc
goto :skipFunctions

:test
	set TEST_CASE=%1
	set TEST_RSC=%2
	set EPOCWIND_COPY=\epoc32\winscw\c\logs\testexecute\sspluginstest_%TEST_CASE%_epocwind.out
	set EPOCWIND_ORIG=%TEMP%\epocwind.out

	if exist %EPOCWIND_COPY% del %EPOCWIND_COPY%

	call z\sspluginstest\sspluginstest_tgsa_i_setup.bat install   %TEST_CASE% %TEST_RSC% 

	echo testexecute.exe z:\sspluginstest\sspluginstest_%TEST_CASE%.script
	echo epoc.exe - if does not terminate within 1 minute, terminate manually
	testexecute.exe z:\sspluginstest\sspluginstest_%TEST_CASE%.script

	call z\sspluginstest\sspluginstest_tgsa_i_setup.bat uninstall %TEST_CASE% %TEST_RSC% 

	copy %EPOCWIND_ORIG% %EPOCWIND_COPY% >NUL
goto :EOF

:checkLog
	:: checkLog SEARCH_FILE SEARCH_STRING EXPECTED_NUMBER_OF_OCCURENCES
	set SEARCH_FILE=%1
	set SEARCH_STRING=%2
	set TEMP_FILE=ssplugins_temp.txt
	set EXPECTED_COUNT=%3
	set LOG_COUNT=0
	type %SEARCH_FILE% | find /c %SEARCH_STRING%>%TEMP_FILE%
	for /f "delims=\" %%i in ('type %TEMP_FILE%') do set LOG_COUNT=%%i
	del %TEMP_FILE%
	if "%EXPECTED_COUNT%" ==  "%LOG_COUNT%" goto :EOF
	if "%EXPECTED_COUNT%" NEQ "%LOG_COUNT%" set TEST_RESULT_FLAG=FAIL
goto :EOF

:skipFunctions 


endlocal
