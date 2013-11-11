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

set TEST_CASE=%1
set EPOCWIND=%TEMP%\epocwind.out

set GSA_RESULT_FILE=\epoc32\winscw\c\t_gsapanicresultfile.txt

goto :%TEST_CASE%

:checkLog
	:: checkLog SEARCH_STRING EXPECTED_NUMBER_OF_OCCURENCES
	set SEARCH_STRING=%1
	set TEMP_FILE=ssplugins_temp.txt
	set EXPECTED_COUNT=%2
	set LOG_COUNT=0
	type %EPOCWIND% | %windir%\system32\find /c %SEARCH_STRING%>%TEMP_FILE%
	for /f "delims=\" %%i in ('type %TEMP_FILE%') do set LOG_COUNT=%%i
	del %TEMP_FILE%
	if "%EXPECTED_COUNT%" ==  "%LOG_COUNT%" goto :EOF
	if "%EXPECTED_COUNT%" NEQ "%LOG_COUNT%" set TEST_RESULT_FLAG=false
goto :EOF

:initialise
	if exist %GSA_RESULT_FILE% del %GSA_RESULT_FILE%
	set TEST_RESULT_FLAG=true
goto :EOF

:statetranfromstartup
:statetranfromnormal
:statetranfromshutdown
:statetrantoshutdown
:statetranfromstartuptoshutdown
goto :EOF

:duplicatesubstate
	call :initialise
	call :checkLog "Thread SYSSTATEMGR.EXE::!SysStateMgr Panic CmdResourceRead 22" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %GSA_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %GSA_RESULT_FILE%
goto :EOF

endlocal
