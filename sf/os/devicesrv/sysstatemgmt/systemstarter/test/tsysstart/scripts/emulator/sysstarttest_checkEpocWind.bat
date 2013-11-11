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

set DLL_RESULT_FILE=\epoc32\winscw\c\T_DLL_ResultFile.txt
set STFP_RESULT_FILE=\epoc32\winscw\c\T_STFP_PanicResultFile.txt
set APP_RESULT_FILE=\epoc32\winscw\c\T_AppPanicResultFile.txt


set TEST_CASE=%1
set EPOCWIND=%TEMP%\epocwind.out

goto :%TEST_CASE%

:checkLog
	:: checkLog SEARCH_STRING EXPECTED_NUMBER_OF_OCCURENCES
	set SEARCH_STRING=%1
	set TEMP_FILE=sysstart_temp.txt
	set EXPECTED_COUNT=%2
	set LOG_COUNT=0
	type %EPOCWIND% | %windir%\system32\find /c %SEARCH_STRING%>%TEMP_FILE%
	for /f "delims=\" %%i in ('type %TEMP_FILE%') do set LOG_COUNT=%%i
	del %TEMP_FILE%
	if "%EXPECTED_COUNT%" ==  "%LOG_COUNT%" goto :EOF
	if "%EXPECTED_COUNT%" NEQ "%LOG_COUNT%" set TEST_RESULT_FLAG=false
goto :EOF

:initialise
	if exist %STFP_RESULT_FILE% del %STFP_RESULT_FILE%
	if exist %DLL_RESULT_FILE%  del %DLL_RESULT_FILE%
	if exist %APP_RESULT_FILE%  del %APP_RESULT_FILE%
	set TEST_RESULT_FLAG=true
goto :EOF

:StfpCdToNcRetry0Panic
:StfpCdToNcRetry1Panic
:StfpCsToCdRetry0Panic
:StfpCsToCdRetry1Panic
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %STFP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %STFP_RESULT_FILE%
goto :EOF

:DllInvalidRetry0Ignore
	call :initialise
	call :checkLog "SysStart: Unable to load DLL - t_invalid_dll1.dll, error" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllInvalidRetry1Ignore
	call :initialise
	call :checkLog "SysStart: Unable to load DLL - t_invalid_dll2.dll, error" 2
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^2 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllInvalidRetry0Panic
	call :initialise
	call :checkLog "SysStart: Unable to load DLL - t_invalid_dll1.dll, error" 1
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllInvalidRetry1Panic
	call :initialise
	call :checkLog "SysStart: Unable to load DLL - t_invalid_dll1.dll, error" 2
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^2 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllInvalidOrdinalRetry0Ignore
	call :initialise
	call :checkLog "Sysstart: No function at specified ordinal 99" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllInvalidOrdinalRetry1Ignore
	call :initialise
	call :checkLog "Sysstart: No function at specified ordinal 100" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllInvalidOrdinalRetry0Panic
:DllInvalidOrdinalRetry1Panic
	call :initialise
	call :checkLog "Sysstart: No function at specified ordinal 99" 1
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllInvalidCustomData
	call :initialise
	call :checkLog "Invalid custom_dll_data_link value of 0" 1
	call :checkLog "Thread SYSSTART.EXE::Main Panic CResourceReader 14" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllInvalidOrdinal0
	call :initialise
	call :checkLog "Invalid ordinal value of 0" 1
	call :checkLog "Thread SYSSTART.EXE::Main Panic CResourceReader 13" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllCdFailPanic
:DllCsFailPanic
:DllNcFailPanic
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:DllCsFnInvalidFail
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic CResourceReader 12" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %DLL_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %DLL_RESULT_FILE%
goto :EOF

:AppFailRetry0Panic
:AppFailRetry1Panic
:ProcFailRetry0Panic
:ProcFailRetry1Panic
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:AppTimeoutRetry3Panic
:ProcTimeoutRetry3Panic
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:DefTimeoutAppPanic
:DefTimeoutProcPanic
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:DefNoMult1
:DefNoMult2
:DefNoMult3
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic CResourceReader 18" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:DefInvalidAppRetry
:DefInvalidProcRetry
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic CStartupProperti 6" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:DefInvalidAppTimeout
:DefInvalidProcTimeout
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic CStartupProperti 5" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:DefInvalidMultFail
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic CResourceReader 11" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:DefInvalidMultWait
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic CResourceReader 17" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:AppStartFailPanic
:ProcStartFailPanic
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:DefStartFailAppPanic
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:DefStartFailProcPanic
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF


:DefRendFailAppPanic
:DefRendFailProcPanic
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic RestartSys 1" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

:InitApparcBeforeServer
	call :initialise
	call :checkLog "Thread SYSSTART.EXE::Main Panic APSTART 0" 1
	call :checkLog "FAULT: KERN 0x00000004" 1
	if /i "%TEST_RESULT_FLAG%"=="true" echo count=^1 > %APP_RESULT_FILE%
	if /i "%TEST_RESULT_FLAG%"=="false" echo count=^0 > %APP_RESULT_FILE%
goto :EOF

endlocal
