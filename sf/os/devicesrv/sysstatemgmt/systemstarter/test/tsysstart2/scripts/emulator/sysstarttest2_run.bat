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

:: *******************************************************************
:: Any ADDITIONS or DELETIONS to the test cases in this script MUST
:: be copied to the partner script that is used on hardware:
:: ..\hardware\sysstarttest2_run.bat
:: *******************************************************************

set SYSSTARTTEST2_SUMMARY=\epoc32\winscw\c\logs\testexecute\sysstarttest2_summary.txt
if exist %SYSSTARTTEST2_SUMMARY% del %SYSSTARTTEST2_SUMMARY% >NUL

call :test procstart			tsysstart2-procstart_emulator.RSC
call :test procmon				tsysstart2-procmon_emulator.RSC
call :test procmondeferred		tsysstart2-procmondeferred_emulator.RSC

goto :skipFunctions


:test
	set TEST_CASE=%1
	set TEST_RSC=%2
	set EPOCWIND_COPY=\epoc32\winscw\c\logs\testexecute\sysstarttest_tsysstart2_%TEST_CASE%_epocwind.out
	set EPOCWIND_ORIG=%TEMP%\epocwind.out

	if exist %EPOCWIND_COPY% del %EPOCWIND_COPY%

	call z\sysstarttest\sysstarttest2_setup.bat install   %TEST_CASE% %TEST_RSC% 
	call z\sysstarttest\sysstarttest2_setup.bat test %TEST_CASE% %TEST_RSC%				
	call z\sysstarttest\sysstarttest2_setup.bat uninstall %TEST_CASE% %TEST_RSC% 

	copy %EPOCWIND_ORIG% %EPOCWIND_COPY% >NUL
	call :updateTestSummary
goto :EOF


:checkLog
	:: checkLog SEARCH_FILE SEARCH_STRING EXPECTED_NUMBER_OF_OCCURENCES
	set SEARCH_FILE=%1
	set SEARCH_STRING=%2
	set TEMP_FILE=sysstart2_temp.txt
	set EXPECTED_COUNT=%3
	set LOG_COUNT=0
	
	type %SEARCH_FILE%
	type %SEARCH_FILE% | find /c /i %SEARCH_STRING%>%TEMP_FILE%
	for /f "delims=\" %%i in ('type %TEMP_FILE%') do set LOG_COUNT=%%i

	del %TEMP_FILE%
	if "%EXPECTED_COUNT%" ==  "%LOG_COUNT%" goto :EOF
	if "%EXPECTED_COUNT%" NEQ "%LOG_COUNT%" set TEST_RESULT_FLAG=FAIL
goto :EOF


:updateTestSummary
	set TEST_RESULT_FLAG=PASS
	call :checkLog \epoc32\winscw\c\logs\testexecute\sysstarttest_tsysstart2_%TEST_CASE%.htm "%TEST_CASE% ***Result = PASS" 1
	echo %TEST_CASE% - %TEST_RESULT_FLAG% >> %SYSSTARTTEST2_SUMMARY%
goto :EOF


:skipFunctions 
endlocal
