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
:: ..\hardware\sysstarttest_run.bat
:: *******************************************************************

set SYSSTARTTEST_SUMMARY=\epoc32\winscw\c\logs\testexecute\sysstarttest_summary.txt
if exist %SYSSTARTTEST_SUMMARY% del %SYSSTARTTEST_SUMMARY% >NUL

:: SysStart START_APP_INFO tests
call :test AppFailRetry0Ignore           appfwk_sysstart_test_app_emulator3.RSC
call :test AppFailRetry0Panic            appfwk_sysstart_test_app_emulator5.RSC
call :test AppFailRetry1Ignore           appfwk_sysstart_test_app_emulator4.RSC
call :test AppFailRetry1Panic            appfwk_sysstart_test_app_emulator6.RSC
call :test AppStartCd                    appfwk_sysstart_test_app_emulator1.RSC
call :test AppStartCs                    appfwk_sysstart_test_app_emulator0.RSC
call :test AppStartFailPanic             appfwk_sysstart_test_app_emulator9.RSC
call :test AppStartNc                    appfwk_sysstart_test_app_emulator2.RSC
call :test AppTimeoutRetry3Ignore        appfwk_sysstart_test_app_emulator7.RSC
call :test AppTimeoutRetry3Panic         appfwk_sysstart_test_app_emulator8.RSC

:: SysStart EDeferredWaitForStart tests
call :test DefInvalidAppRetry            appfwk_sysstart_test_def_emulator10.RSC
call :test DefInvalidAppTimeout          appfwk_sysstart_test_def_emulator8.RSC
call :test DefInvalidMultFail            appfwk_sysstart_test_def_emulator19.RSC
call :test DefInvalidMultWait            appfwk_sysstart_test_def_emulator23.RSC
call :test DefInvalidProcRetry           appfwk_sysstart_test_def_emulator17.RSC
call :test DefInvalidProcTimeout         appfwk_sysstart_test_def_emulator9.RSC
call :test DefMultGrp                    appfwk_sysstart_test_def_emulator7.RSC
call :test DefNoMult1                    appfwk_sysstart_test_def_emulator3.RSC
call :test DefNoMult2                    appfwk_sysstart_test_def_emulator4.RSC
call :test DefNoMult3                    appfwk_sysstart_test_def_emulator5.RSC
call :test DefRendFailAppPanic           appfwk_sysstart_test_def_emulator21.RSC
call :test DefRendFailIgnore             appfwk_sysstart_test_def_emulator20.RSC
call :test DefRendFailProcPanic          appfwk_sysstart_test_def_emulator22.RSC
call :test DefStartAllState              appfwk_sysstart_test_def_emulator6.RSC
call :test DefStartCd                    appfwk_sysstart_test_def_emulator0.RSC
call :test DefStartCs                    appfwk_sysstart_test_def_emulator1.RSC
call :test DefStartFailAppPanic          appfwk_sysstart_test_def_emulator15.RSC
call :test DefStartFailIgnore            appfwk_sysstart_test_def_emulator14.RSC
call :test DefStartFailProcPanic         appfwk_sysstart_test_def_emulator16.RSC
call :test DefStartNc                    appfwk_sysstart_test_def_emulator2.RSC
call :test DefTimeoutAppIgnore           appfwk_sysstart_test_def_emulator11.RSC
call :test DefTimeoutAppPanic            appfwk_sysstart_test_def_emulator12.RSC
call :test DefTimeoutProcIgnore          appfwk_sysstart_test_def_emulator18.RSC
call :test DefTimeoutProcPanic           appfwk_sysstart_test_def_emulator13.RSC

:: SysStart DLL Function call tests
call :test DllCdFailIgnore               appfwk_sysstart_test_dll_emulator1.RSC  
call :test DllCdFailPanic                appfwk_sysstart_test_dll_emulator10.RSC 
call :test DllCdNormal                   appfwk_sysstart_test_dll_emulator1.RSC 
call :test DllCsFailIgnore               appfwk_sysstart_test_dll_emulator0.RSC  
call :test DllCsFailPanic                appfwk_sysstart_test_dll_emulator9.RSC 
call :test DllCsFnInvalidFail            appfwk_sysstart_test_dll_emulator12.RSC 
call :test DllCsFnMultipleRetry          appfwk_sysstart_test_dll_emulator13.RSC   
call :test DllCsNormal                   appfwk_sysstart_test_dll_emulator0.RSC 
call :test DllFnSharedData               appfwk_sysstart_test_dll_emulator0.RSC  
call :test DllInvalidCustomData          appfwk_sysstart_test_dll_emulator14.RSC
call :test DllInvalidOrdinal0            appfwk_sysstart_test_dll_emulator8.RSC   
call :test DllInvalidOrdinalRetry0Ignore appfwk_sysstart_test_dll_emulator3.RSC  
call :test DllInvalidOrdinalRetry1Ignore appfwk_sysstart_test_dll_emulator3.RSC  
call :test DllInvalidOrdinalRetry0Panic  appfwk_sysstart_test_dll_emulator6.RSC  
call :test DllInvalidOrdinalRetry1Panic  appfwk_sysstart_test_dll_emulator7.RSC  
call :test DllInvalidRetry0Ignore        appfwk_sysstart_test_dll_emulator3.RSC  
call :test DllInvalidRetry0Panic         appfwk_sysstart_test_dll_emulator4.RSC  
call :test DllInvalidRetry1Ignore        appfwk_sysstart_test_dll_emulator3.RSC  
call :test DllInvalidRetry1Panic         appfwk_sysstart_test_dll_emulator5.RSC  
call :test DllNcFailIgnore               appfwk_sysstart_test_dll_emulator2.RSC  
call :test DllNcFailPanic                appfwk_sysstart_test_dll_emulator11.RSC 
call :test DllNcNormal                   appfwk_sysstart_test_dll_emulator2.RSC  

:: SysStart INIT_APPARC tests 
call :test InitApparc           	 appfwk_sysstart_test_apparc_emulator0.RSC
call :test InitApparcBeforeServer     	 appfwk_sysstart_test_apparc_emulator1.RSC

:: SysStart START_PROCESS_INFO tests
call :test ProcFailRetry0Ignore          appfwk_sysstart_test_proc_emulator3.RSC
call :test ProcFailRetry0Panic           appfwk_sysstart_test_proc_emulator5.RSC
call :test ProcFailRetry1Ignore          appfwk_sysstart_test_proc_emulator4.RSC
call :test ProcFailRetry1Panic           appfwk_sysstart_test_proc_emulator6.RSC
call :test ProcStartCd                   appfwk_sysstart_test_proc_emulator1.RSC
call :test ProcStartCs                   appfwk_sysstart_test_proc_emulator0.RSC
call :test ProcStartFailPanic            appfwk_sysstart_test_proc_emulator9.RSC
call :test ProcStartNc                   appfwk_sysstart_test_proc_emulator2.RSC
call :test ProcTimeoutRetry3Ignore       appfwk_sysstart_test_proc_emulator7.RSC
call :test ProcTimeoutRetry3Panic        appfwk_sysstart_test_proc_emulator8.RSC

:: SysStart re-running protection test
call :test ReRunSysStart                 SSCForStartupMode0.RSC.original

:: SysStart State Transition Failure Policy tests
call :test StfpCdToNcDefault             appfwk_sysstart_test_stfp_emulator0.RSC 
call :test StfpCdToNcRetry0Ignore        appfwk_sysstart_test_stfp_emulator2.RSC 
call :test StfpCdToNcRetry0Panic         appfwk_sysstart_test_stfp_emulator1.RSC 
call :test StfpCdToNcRetry1Ignore        appfwk_sysstart_test_stfp_emulator4.RSC 
call :test StfpCdToNcRetry1Panic         appfwk_sysstart_test_stfp_emulator3.RSC 
call :test StfpCsToCdDefault             appfwk_sysstart_test_stfp_emulator0.RSC 
call :test StfpCsToCdRetry0Ignore        appfwk_sysstart_test_stfp_emulator2.RSC 
call :test StfpCsToCdRetry0Panic         appfwk_sysstart_test_stfp_emulator1.RSC 
call :test StfpCsToCdRetry1Ignore        appfwk_sysstart_test_stfp_emulator4.RSC 
call :test StfpCsToCdRetry1Panic         appfwk_sysstart_test_stfp_emulator3.RSC 

goto :skipFunctions

:test
	set TEST_CASE=%1
	set TEST_RSC=%2
	set EPOCWIND_COPY=\epoc32\winscw\c\logs\testexecute\sysstarttest_%TEST_CASE%_epocwind.out
	set EPOCWIND_ORIG=%TEMP%\epocwind.out

	if exist %EPOCWIND_COPY% del %EPOCWIND_COPY%

	call z\sysstarttest\sysstarttest_setup_post1871.bat install   %TEST_CASE% %TEST_RSC% 

	echo testexecute.exe z:\sysstarttest\sysstarttest_%TEST_CASE%.script
	echo epoc.exe - if does not terminate within 1 minute, terminate manually
	testexecute.exe z:\sysstarttest\sysstarttest_%TEST_CASE%.script

	call z\sysstarttest\sysstarttest_setup_post1871.bat uninstall %TEST_CASE% %TEST_RSC% 

	copy %EPOCWIND_ORIG% %EPOCWIND_COPY% >NUL
	call :updateTestSummary
goto :EOF

:checkLog
	:: checkLog SEARCH_FILE SEARCH_STRING EXPECTED_NUMBER_OF_OCCURENCES
	set SEARCH_FILE=%1
	set SEARCH_STRING=%2
	set TEMP_FILE=sysstart_temp.txt
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
	call :checkLog \epoc32\winscw\c\logs\testexecute\sysstarttest_%TEST_CASE%.htm "%TEST_CASE% ***Result = PASS" 1
	echo %TEST_CASE% - %TEST_RESULT_FLAG% >> %SYSSTARTTEST_SUMMARY%
goto :EOF

:skipFunctions 


endlocal
