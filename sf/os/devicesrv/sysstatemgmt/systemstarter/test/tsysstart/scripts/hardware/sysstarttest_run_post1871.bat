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
:: ..\emulator\sysstarttest_run.bat
:: *******************************************************************

call :initialise %1 %2
if /i "%INPUT_ERROR%"=="true" goto :EOF

:: SysStart DLL Function call tests
call :test DllCdFailIgnore					appfwk_sysstart_test_dll_hardware1.RSC  

:: if automation is not supported, then don't bother attempting to run
:: the remaining test cases.
if /i "%TEST_MANUAL_FLAG%"=="false" (if /i "%AUTOMATED_FLAG%"=="false" goto :EOF) 

call :test DllCdNormal						appfwk_sysstart_test_dll_hardware1.RSC 
call :test DllCsFailIgnore					appfwk_sysstart_test_dll_hardware0.RSC  
call :test DllCsFnInvalidFail				appfwk_sysstart_test_dll_hardware12.RSC 
call :test DllCsFnMultipleRetry				appfwk_sysstart_test_dll_hardware13.RSC
:: PANIC-FAIL call :test DllCdFailPanic                appfwk_sysstart_test_dll_hardware10.RSC 
:: PANIC-FAIL call :test DllCsFailPanic                appfwk_sysstart_test_dll_hardware9.RSC 

:: This test is contained within the normal DABS overnight test run. So if the result file is
:: present then do not run it again.
if not exist "\logs\armv5\TESTEXECUTEsysstarttest_DllCsNormal.script.htm" call :test DllCsNormal appfwk_sysstart_test_dll_hardware0.RSC  

call :test DllFnSharedData					appfwk_sysstart_test_dll_hardware0.RSC  
call :test DllInvalidCustomData				appfwk_sysstart_test_dll_hardware14.RSC
call :test DllInvalidOrdinal0				appfwk_sysstart_test_dll_hardware8.RSC   
call :test DllInvalidOrdinalRetry0Ignore	appfwk_sysstart_test_dll_hardware3.RSC  
call :test DllInvalidOrdinalRetry1Ignore	appfwk_sysstart_test_dll_hardware3.RSC  
call :test DllInvalidRetry0Ignore			appfwk_sysstart_test_dll_hardware3.RSC  
call :test DllInvalidRetry1Ignore			appfwk_sysstart_test_dll_hardware3.RSC  
call :test DllNcFailIgnore					appfwk_sysstart_test_dll_hardware2.RSC  
call :test DllNcNormal						appfwk_sysstart_test_dll_hardware2.RSC  
:: PANIC-FAIL call :test DllInvalidOrdinalRetry0Panic  appfwk_sysstart_test_dll_hardware6.RSC
:: PANIC-FAIL call :test DllInvalidOrdinalRetry1Panic  appfwk_sysstart_test_dll_hardware7.RSC
:: PANIC-FAIL call :test DllInvalidRetry0Panic         appfwk_sysstart_test_dll_hardware4.RSC
:: PANIC-FAIL call :test DllInvalidRetry1Panic         appfwk_sysstart_test_dll_hardware5.RSC
:: PANIC-FAIL call :test DllNcFailPanic                appfwk_sysstart_test_dll_hardware11.RSC 

:: SysStart START_APP test cases
call :test AppFailRetry0Ignore				appfwk_sysstart_test_app_hardware3.RSC
call :test AppFailRetry1Ignore				appfwk_sysstart_test_app_hardware4.RSC
call :test AppStartCd						appfwk_sysstart_test_app_hardware1.RSC
call :test AppStartCs						appfwk_sysstart_test_app_hardware0.RSC
call :test AppStartNc						appfwk_sysstart_test_app_hardware2.RSC
call :test AppTimeoutRetry3Ignore			appfwk_sysstart_test_app_hardware7.RSC
:: PANIC call :test AppFailRetry0Panic            appfwk_sysstart_test_app_hardware5.RSC
:: PANIC call :test AppFailRetry1Panic            appfwk_sysstart_test_app_hardware6.RSC
:: PANIC call :test AppStartFailPanic             appfwk_sysstart_test_app_hardware9.RSC
:: PANIC call :test AppTimeoutRetry3Panic         appfwk_sysstart_test_app_hardware8.RSC

:: SysStart EDeferredWaitForStart tests
call :test DefInvalidAppRetry				appfwk_sysstart_test_def_hardware10.RSC
call :test DefInvalidAppTimeout				appfwk_sysstart_test_def_hardware8.RSC
call :test DefInvalidMultFail				appfwk_sysstart_test_def_hardware19.RSC
call :test DefInvalidMultWait				appfwk_sysstart_test_def_hardware23.RSC
call :test DefInvalidProcRetry				appfwk_sysstart_test_def_hardware17.RSC
call :test DefInvalidProcTimeout			appfwk_sysstart_test_def_hardware9.RSC
call :test DefMultGrp						appfwk_sysstart_test_def_hardware7.RSC
call :test DefNoMult1						appfwk_sysstart_test_def_hardware3.RSC
call :test DefNoMult2						appfwk_sysstart_test_def_hardware4.RSC
call :test DefNoMult3						appfwk_sysstart_test_def_hardware5.RSC
call :test DefRendFailIgnore				appfwk_sysstart_test_def_hardware20.RSC
call :test DefStartAllState					appfwk_sysstart_test_def_hardware6.RSC
call :test DefStartCd						appfwk_sysstart_test_def_hardware0.RSC
call :test DefStartCs						appfwk_sysstart_test_def_hardware1.RSC
call :test DefStartFailIgnore				appfwk_sysstart_test_def_hardware14.RSC
call :test DefStartNc						appfwk_sysstart_test_def_hardware2.RSC
call :test DefTimeoutAppIgnore				appfwk_sysstart_test_def_hardware11.RSC
call :test DefTimeoutProcIgnore				appfwk_sysstart_test_def_hardware18.RSC
:: PANIC call :test DefRendFailAppPanic           appfwk_sysstart_test_def_hardware21.RSC
:: PANIC call :test DefRendFailProcPanic          appfwk_sysstart_test_def_hardware22.RSC
:: PANIC call :test DefStartFailAppPanic          appfwk_sysstart_test_def_hardware15.RSC
:: PANIC call :test DefStartFailProcPanic         appfwk_sysstart_test_def_hardware16.RSC
:: PANIC call :test DefTimeoutAppPanic            appfwk_sysstart_test_def_hardware12.RSC
:: PANIC call :test DefTimeoutProcPanic           appfwk_sysstart_test_def_hardware13.RSC

:: SysStart INIT_APPARC tests 
call :test InitApparc						appfwk_sysstart_test_apparc_hardware0.RSC
call :test InitApparcBeforeServer			appfwk_sysstart_test_apparc_hardware1.RSC

:: SysStart START_PROC test cases
call :test ProcFailRetry0Ignore				appfwk_sysstart_test_proc_hardware3.RSC
call :test ProcFailRetry1Ignore				appfwk_sysstart_test_proc_hardware4.RSC
call :test ProcStartCd						appfwk_sysstart_test_proc_hardware1.RSC
call :test ProcStartCs						appfwk_sysstart_test_proc_hardware0.RSC
call :test ProcStartNc						appfwk_sysstart_test_proc_hardware2.RSC
call :test ProcTimeoutRetry3Ignore			appfwk_sysstart_test_proc_hardware7.RSC
:: PANIC call :test ProcFailRetry0Panic           appfwk_sysstart_test_proc_hardware5.RSC
:: PANIC call :test ProcFailRetry1Panic           appfwk_sysstart_test_proc_hardware6.RSC
:: PANIC call :test ProcStartFailPanic            appfwk_sysstart_test_proc_hardware9.RSC
:: PANIC call :test ProcTimeoutRetry3Panic        appfwk_sysstart_test_proc_hardware8.RSC

:: SysStart re-running protection test
:: This test is contained within the normal DABS overnight test run. So if the result file is
:: present then do not run it again.
if not exist "\logs\armv5\TESTEXECUTEsysstarttest_ReRunSysStart.script.htm" call :test ReRunSysStart                 appfwk_sysstart_test_eshell_hardware.RSC

:: SysStart State Transition Failure Policy tests
call :test StfpCdToNcDefault				appfwk_sysstart_test_stfp_hardware0.RSC 
call :test StfpCdToNcRetry0Ignore			appfwk_sysstart_test_stfp_hardware2.RSC 
call :test StfpCdToNcRetry1Ignore			appfwk_sysstart_test_stfp_hardware4.RSC 
call :test StfpCsToCdDefault				appfwk_sysstart_test_stfp_hardware0.RSC
:: PANIC call :test StfpCdToNcRetry0Panic         appfwk_sysstart_test_stfp_hardware1.RSC 
:: PANIC call :test StfpCdToNcRetry1Panic         appfwk_sysstart_test_stfp_hardware3.RSC 

:: This test is contained within the normal DABS overnight test run. So if the result file is
:: present then do not run it again.
if not exist "\logs\armv5\TESTEXECUTEsysstarttest_StfpCsToCdRetry0Ignore.script.htm" call :test StfpCsToCdRetry0Ignore  appfwk_sysstart_test_stfp_hardware2.RSC 

call :test StfpCsToCdRetry1Ignore			appfwk_sysstart_test_stfp_hardware4.RSC 
:: PANIC call :test StfpCsToCdRetry0Panic         appfwk_sysstart_test_stfp_hardware1.RSC 
:: PANIC call :test StfpCsToCdRetry1Panic         appfwk_sysstart_test_stfp_hardware3.RSC 


goto :skipFunctions

:help
	echo.
	echo Description:
	echo This script runs the apparc startup test suite on the hardware
	echo reference platform. 
	echo.
	echo Syntax:
	echo %RUN_COMMAND% [choice] [hrp]
	echo where the argument [choice] can be one of:
	echo   "-h" or "help" - this help screen
	echo   "manual"       - run scripts manually i.e. build auto 
	echo                    start ROMs for each test case, but do  
	echo                    not attempt to upload ROM
	echo   "auto"         - run scripts in automated mode, i.e.
	echo			requires USB download to HRP
	echo and argument [hrp] is the hardware reference platform, e.g.
	echo   "h2" or "h4hrp"
	echo.
	echo e.g.
	echo %RUN_COMMAND% auto h4hrp
	echo.
goto :EOF

:initialise
	set APPFWK_COMPONENT=sysstarttest
	set EPOC_Z=\epoc32\data\z

	set BUILDROM_COMMAND=%EPOC_Z%\%APPFWK_COMPONENT%\sysstarttest_buildrom.bat
	set CHECKEPOC_COMMAND=%EPOC_Z%\%APPFWK_COMPONENT%\sysstarttest_checkepocwind_post1871.bat
	set INSERT_COMMAND=%EPOC_Z%\%APPFWK_COMPONENT%\sysstarttest_insertlog.pl
	set RUN_COMMAND=%EPOC_Z%\%APPFWK_COMPONENT%\sysstarttest_run_post871.bat

	:: check input values
	set INPUT_ERROR=false
	set TEST_CHOICE=%1
	if /i "%TEST_CHOICE%"=="-h" set INPUT_ERROR=true
	if /i "%TEST_CHOICE%"=="help" set INPUT_ERROR=true
	set TEST_MANUAL_FLAG=false
	if /i "%TEST_CHOICE%"=="manual" set TEST_MANUAL_FLAG=true
	set HARDWARE_REFERENCE_PLATFORM=%2
	if /i "%HARDWARE_REFERENCE_PLATFORM%"=="" set INPUT_ERROR=true
	if /i "%INPUT_ERROR%"=="true" call :help & goto :EOF
	
	set TEST_CASE=INIT_VALUE
	set TEST_RSC=INIT_VALUE
	set TEST_ROM_PORT=1
	set TEST_CASE_TIMEOUT=60

	set TEST_LOG_LOCATION=%EPOC_Z%\%APPFWK_COMPONENT%\logs
	if not exist %TEST_LOG_LOCATION% md %TEST_LOG_LOCATION% 

	set DABS_TEST_LOG_LOCATION=\logs\armv5

	set TEST_LOG=%DABS_TEST_LOG_LOCATION%\%APPFWK_COMPONENT%.log
	if exist %TEST_LOG% del %TEST_LOG%

	set SUMMARY_LOG=%DABS_TEST_LOG_LOCATION%\%APPFWK_COMPONENT%_summary.txt
	if exist %SUMMARY_LOG% del %SUMMARY_LOG% >NUL
goto :EOF

:checkForUsbDrive
	dir %1:\ 2>&1 | find "%1:" >NUL 2>&1
	if ERRORLEVEL 1 goto :EOF
	if not exist %1:\usbload.zip goto :EOF
	echo INFO: %1:\usbload.zip found!
	set HARDWARE_USB=true
	set HARDWARE_USB_DRIVE=%1
goto :EOF

:wait
	echo INFO: Waiting %1 seconds
	ping 1.1.1.1 -w 1000 -n %1 >NUL
goto :EOF

:searchForUSB
	for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do call :checkForUsbDrive %%i
goto :EOF

:checkAutomation
	set AUTOMATED_FLAG=false
	set HARDWARE_USB=false
	if not exist "\trgtest_s.exe"                          echo INFO: \trgtest_s.exe not found & goto :EOF
	if not exist "\empty_image_file"                     echo INFO: \empty_image_file not found & goto :EOF
	if not exist "\sync.exe"                             echo INFO: \sync.exe not found & goto :EOF
	if not exist "\hardwarecontrol.exe"                   echo INFO: \hardwarecontrol.exe not found & goto :EOF
	if not exist "\devcon.exe"                           echo INFO: \devcon.exe not found & goto :EOF
	if not exist "\EJECTDELAY.15"                        echo INFO: \EJECTDELAY.15 not found & goto :EOF
	if not exist "\empty_image_file"                     echo INFO: \empty_image_file not found & goto :EOF
	if not exist "\logs\autorom\dummytest.txt"           echo INFO: \logs\autorom\dummytest.txt not found & goto :EOF
	
	call \devcon.exe remove "USB\Vid_0e22&Pid_1112&Rev_0305" > NUL

	echo INFO: Hardware OFF
	call \hardwarecontrol.exe --off > NUL
	
	call :wait 2

	echo INFO: Hardware ON
	call \hardwarecontrol.exe --on > NUL

	call :wait 15
	call :searchForUSB

	if /i "%HARDWARE_USB%"=="true" goto :skipUsbSearch
	call :wait 10 
	call :searchForUSB

	if /i "%HARDWARE_USB%"=="true" goto :skipUsbSearch
	call :usbNotFound 
	goto :EOF

	:skipUsbSearch
	if not exist \logs\armv5 md \logs\armv5 
	set AUTOMATED_FLAG=true
goto :EOF

:usbNotFound
	echo INFO: Target USB Drive not found
	call \hardwarecontrol.exe --off > NUL
goto :EOF

:upLoadROm
	echo INFO: Uploading ROM to %HARDWARE_USB_DRIVE%:
	if exist "%HARDWARE_USB_DRIVE%:\sys$rom.zip" del %HARDWARE_USB_DRIVE%:\sys$rom.zip
	copy %1 %HARDWARE_USB_DRIVE%:\ > NUL

	del %HARDWARE_USB_DRIVE%:\EJECTDELAY.*
	copy \EJECTDELAY.15 %HARDWARE_USB_DRIVE%:\ > NUL

	echo INFO: Checking %HARDWARE_USB_DRIVE%:
	chkdsk %HARDWARE_USB_DRIVE%: > NUL

	echo INFO: Ejecting %HARDWARE_USB_DRIVE%:
	\sync.exe -r -e %HARDWARE_USB_DRIVE%: > NUL
	call \devcon.exe remove "USB\Vid_0e22&Pid_1112&Rev_0305" >NUL

	echo INFO: Listening to COM%TEST_ROM_PORT% for test results (%TEST_CASE_TIMEOUT% seconds)
	\trgtest_s.exe %TEST_ROM_PORT% \empty_image_file %TEST_CASE_LOG% %TEST_CASE_TIMEOUT%
	echo INFO: test case log - %TEST_CASE_LOG%

	call \devcon.exe remove "USB\Vid_0e22&Pid_1112&Rev_0305" > NUL

	echo INFO: Hardware OFF
	call \hardwarecontrol.exe --off > NUL
	
	call :wait 2
goto :EOF

:manualBuild
	set AUTOMATED_FLAG=false
	call %BUILDROM_COMMAND% %TEST_CASE% %TEST_CONFIG% ROM %AUTOMATED_FLAG%
goto :EOF

:test
	set TEST_CASE=%1
	set TEST_CONFIG=%2
	
	echo INFO: Begin test case %TEST_CASE%
	time /t

	if /i "%TEST_MANUAL_FLAG%"=="true" call :manualBuild & goto :EOF

	set TEST_CASE_LOG=%TEST_LOG_LOCATION%\%TEST_CASE%.log
	set DABS_TEST_CASE_LOG=%DABS_TEST_LOG_LOCATION%\%TEST_CASE%.log

	if exist %TEST_CASE_LOG% del %TEST_CASE_LOG%
	if exist %TEST_CASE_LOG%.original del %TEST_CASE_LOG%.original
	if exist %DABS_TEST_CASE_LOG% del %DABS_TEST_CASE_LOG%
	if exist %DABS_TEST_CASE_LOG%.original del %DABS_TEST_CASE_LOG%.original

	call :checkAutomation

	if /i "%AUTOMATED_FLAG%"=="false" echo ERROR: Automation check failed for rom >> %TEST_CASE_LOG% & goto :EOF

	call %BUILDROM_COMMAND% %TEST_CASE% %TEST_CONFIG% ROM %AUTOMATED_FLAG% %HARDWARE_REFERENCE_PLATFORM%

	if not exist \rom\%TEST_CASE%\sys$rom.zip echo ERROR: \rom\%TEST_CASE%\sys$rom.zip does not exist >> %TEST_CASE_LOG% & goto :logResult

	call :upLoadROm \rom\%TEST_CASE%\sys$rom.zip

	if not exist "\rom\%TEST_CASE%\TEST_ROM_REQUIRED" goto :logResult

	call %CHECKEPOC_COMMAND% %TEST_CASE% %AUTOMATED_FLAG% %HARDWARE_REFERENCE_PLATFORM%

	if not exist \rom\%TEST_CASE%\testrom\sys$rom.zip echo ERROR: \rom\%TEST_CASE%\testrom\sys$rom.zip does not exist >> %TEST_CASE_LOG% & goto :logResult

	call :checkAutomation

	if /i "%AUTOMATED_FLAG%"=="false" echo ERROR: Automation check failed for testrom >> %TEST_CASE_LOG% & goto :logResult

	if exist %TEST_CASE_LOG% copy %TEST_CASE_LOG% %TEST_CASE_LOG%.original >NUL
	if exist %TEST_CASE_LOG% del %TEST_CASE_LOG%

	call :upLoadROm \rom\%TEST_CASE%\testrom\sys$rom.zip

	:logResult
	if not exist %TEST_CASE_LOG% echo ERROR in test execution > %TEST_CASE_LOG% 

	:: add the test case log to the overall test log, which will then be split by the DABS scripts.
	type %TEST_CASE_LOG% >> %TEST_LOG%

	:: copy the test log files across for debugging test runs
	:: NOTE: these won't be reported on the DABS html pages, but can be checked if further info is required
	:: after a failed test case
	copy %TEST_CASE_LOG% %DABS_TEST_CASE_LOG% > NUL
	if exist %TEST_CASE_LOG%.original copy %TEST_CASE_LOG%.original %DABS_TEST_CASE_LOG%.original > NUL

	:: DABS looks for '_epocwind.txt' extension to then include these logs as epocwind
	:: logs, which is useful for debugging what went wrong in case of failure
	copy %TEST_CASE_LOG% %DABS_TEST_LOG_LOCATION%\TESTEXECUTE%APPFWK_COMPONENT%_%TEST_CASE%_epocwind.txt >NUL
	if exist %TEST_CASE_LOG%.original copy %TEST_CASE_LOG%.original %DABS_TEST_LOG_LOCATION%\TESTEXECUTE%APPFWK_COMPONENT%_%TEST_CASE%_epocwind.txt >NUL

	:: insert result into DABS testlog
	call :insertResult

	call :updateTestSummary
	echo INFO: End test case %TEST_CASE%
	time /t
goto :EOF

:insertResult
	:: DABS uses this format in the testlog when building the result summary html page
	set RUNNING_TEXT="9999-99-99=99:99:99 Running <TESTEXECUTE%APPFWK_COMPONENT%_%TEST_CASE%.script.htm> for udeb"
	set LOCATION_TEXT="9999-99-99=99:99:99   Log location: <\logs\armv5\TESTEXECUTE%APPFWK_COMPONENT%_%TEST_CASE%.script.htm>"

	set SEARCH_TEXT="Log location: <\\logs\\armv5\\TESTEXECUTEsysstarttest_rerunsysstart.script.htm>"

	if not exist %DABS_TEST_LOG_LOCATION%\testlog.txt echo ERROR: %DABS_TEST_LOG_LOCATION%\testlog.txt does not exist & goto :EOF
	perl %INSERT_COMMAND% %DABS_TEST_LOG_LOCATION%\testlog.txt %SEARCH_TEXT% %RUNNING_TEXT% %LOCATION_TEXT% 
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
	call :checkLog %DABS_TEST_LOG_LOCATION%\%TEST_CASE%.log "%TEST_CASE% ***Result = PASS" 1
	echo %TEST_CASE% - %TEST_RESULT_FLAG% >> %SUMMARY_LOG%
goto :EOF

:skipFunctions


endlocal
