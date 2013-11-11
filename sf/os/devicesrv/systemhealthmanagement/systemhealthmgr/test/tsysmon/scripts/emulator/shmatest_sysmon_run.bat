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
:: ..\hardware\shmatest_run.bat
:: *******************************************************************

set SHMATEST_SYSMON_SUMMARY=\epoc32\winscw\c\logs\testexecute\shmatest_sysmon_summary.txt
if exist %SHMATEST_SYSMON_SUMMARY% del %SHMATEST_SYSMON_SUMMARY% >NUL

::call :test test_case_name			  SSC_file_name						test_case_id	run_epoc_before_test

call :test conndiscon                 gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0017	0
call :test regeiofcapsnops            gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0019	0
call :test regeiofcapsok              gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0018	0
call :test regeroscapsnops            gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0022	0
call :test regeroscapsok              gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0021	0
call :test regeroswmcapsnopm          gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0026	0
call :test regeroswmcapsnops          gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0027	0
call :test regeroswmcapsok            gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0025	0
call :test regselfeiofcapsok          gsastartupcmdlist0.RSC.original		APPFWK-SYSMON-0020	0
call :test regselferoscapsnops        gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0024	0
call :test regselferoscapsok          gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0023	0
call :test regselferoswmcapsnopm      gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0029	0
call :test regselferoswmcapsnops      gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0030	0
call :test regselferoswmcapsok        gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0028	0
call :test reg1proctwice              gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0051	0
call :test reg2procs                  gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0050	0
call :test regcritical				  gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0031	0

call :test regecnorcapsok			  gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0058	0
call :test regecnorcapsnops			  gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0059	0
call :test regselfecnorcapsok		  gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0060	0
call :test regselfecnorcapsnops		  gsastartupcmdlist0.RSC.original	APPFWK-SYSMON-0061	0
call :test faileiofcount0             appfwk-sysmon-0033_emulator0.rsc	APPFWK-SYSMON-0033	1
call :test faileiofcount1fail         appfwk-sysmon-0034_emulator0.rsc	APPFWK-SYSMON-0034	1
call :test faileroscount1fail         appfwk-sysmon-0035_emulator0.rsc	APPFWK-SYSMON-0035	1
::call :test faileroswmcount1fail       appfwk-sysmon-0036_emulator0.rsc	APPFWK-SYSMON-0036	1
call :test failcount2failquick        appfwk-sysmon-0037_emulator0.rsc	APPFWK-SYSMON-0037	1
call :test failcount2failwait         appfwk-sysmon-0038_emulator0.rsc	APPFWK-SYSMON-0038	1
::call :test restartreg                 appfwk-sysmon-0039_emulator0.rsc	APPFWK-SYSMON-0039	1
::call :test restartfail                appfwk-sysmon-0040_emulator0.rsc	APPFWK-SYSMON-0040	1
::call :test restartfailwithmode1       appfwk-sysmon-0041_emulator0.rsc	APPFWK-SYSMON-0041	1
::call :test restartfailwithmode2       appfwk-sysmon-0042_emulator0.rsc	APPFWK-SYSMON-0042	1
call :test startupregeiof             appfwk-sysmon-0043_emulator0.rsc	APPFWK-SYSMON-0043	1
call :test startupregeioffailstart1   appfwk-sysmon-0044_emulator0.rsc	APPFWK-SYSMON-0044	1
call :test startupregeioffailstart2   appfwk-sysmon-0049_emulator0.rsc	APPFWK-SYSMON-0049	2
call :test startupregerosfaillater    appfwk-sysmon-0045_emulator0.rsc	APPFWK-SYSMON-0045	2
call :test startupregerosfailstart    appfwk-sysmon-0046_emulator0.rsc	APPFWK-SYSMON-0046	2
call :test startupregeroswmfaillater  appfwk-sysmon-0047_emulator0.rsc	APPFWK-SYSMON-0047	2
call :test startupregeroswmfailstart  appfwk-sysmon-0048_emulator0.rsc	APPFWK-SYSMON-0048	2
call :test failecnorcount1fail         appfwk-sysmon-0054_emulator0.rsc	APPFWK-SYSMON-0054	1

goto :skipFunctions

:test
	set TEST_CASE=%1
	set TEST_RSC=%2
	set TEST_ID=%3
	set EPOC_COUNT=%4
	set EPOCWIND_COPY=\epoc32\winscw\c\logs\testexecute\shmatest_sysmon_%TEST_CASE%_epocwind.out
	set EPOCWIND_ORIG=%TEMP%\epocwind.out
	
	if exist %EPOCWIND_COPY% del %EPOCWIND_COPY%

	call z\shmatest\sysmontest_setup.bat install   %TEST_CASE% %TEST_RSC% %EPOC_COUNT%

	echo testexecute.exe z:\shmatest\shmatest_sysmon_%TEST_CASE%.script
	testexecute.exe z:\shmatest\shmatest_sysmon_%TEST_CASE%.script
	
	call z\shmatest\sysmontest_setup.bat uninstall %TEST_CASE% %TEST_RSC% 

	copy %EPOCWIND_ORIG% %EPOCWIND_COPY% >NUL
	
	call :updateTestSummary
goto :EOF


:checkLog
	:: checkLog SEARCH_FILE SEARCH_STRING EXPECTED_NUMBER_OF_OCCURENCES
	set SEARCH_FILE=%1
	set SEARCH_STRING=%2
	set TEMP_FILE=sysmon_temp.txt
	set EXPECTED_COUNT=%3
	set LOG_COUNT=0
	type %SEARCH_FILE% | find /c %SEARCH_STRING%>%TEMP_FILE%
	for /f "delims=\" %%i in ('type %TEMP_FILE%') do set LOG_COUNT=%%i
	del %TEMP_FILE%
	if "%EXPECTED_COUNT%" ==  "%LOG_COUNT%" goto :EOF
	if "%EXPECTED_COUNT%" NEQ "%LOG_COUNT%" set TEST_RESULT_FLAG=FAIL
goto :EOF

:updateTestSummary
	set TEST_RESULT_FLAG=PASS
	call :checkLog \epoc32\winscw\c\logs\testexecute\shmatest_sysmon_%TEST_CASE%.htm "%TEST_ID% ***TestCaseResult = PASS" 1
	echo %TEST_CASE% (%TEST_ID%) - %TEST_RESULT_FLAG% >> %SHMATEST_SYSMON_SUMMARY%
goto :EOF

:skipFunctions 


endlocal
