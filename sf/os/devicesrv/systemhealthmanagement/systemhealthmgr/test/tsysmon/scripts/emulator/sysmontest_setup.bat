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
set RSC_TEST_LOCATION=Z\private\2000d75b\startup\test
set RSC_LOCATION=Z\private\2000d75b\startup

set TEST_CASE=%2
set EPOC_COUNT=%4
call :%TEST_CASE%
set TEST_RSC=%RSC_TEST_LOCATION%\%3

set SM0_RSC=%RSC_LOCATION%\0\gsastartupcmdlist0.RSC
set SM0_RSC_ORIGINAL=%RSC_LOCATION%\test\gsastartupcmdlist0.RSC

set SM1_RSC=%RSC_LOCATION%\1\gsastartupcmdlist1.RSC
set SM1_RSC_ORIGINAL=%RSC_LOCATION%\test\gsastartupcmdlist1.RSC

set EPOCWIND=%TEMP%\epocwind.out

goto :%COMMAND_CHOICE%

:install
	set EPOCWIND_COPY=\epoc32\winscw\c\logs\testexecute\shmatest_sysmon_%TEST_CASE%_fromepoc_epocwind.out
	set EPOCWIND_ORIG=%TEMP%\epocwind.out

	if not exist %SM0_RSC_ORIGINAL%.original copy %SM0_RSC% %SM0_RSC_ORIGINAL%.original > NUL
	if exist %SM0_RSC% del %SM0_RSC%

	if not exist %SM1_RSC_ORIGINAL%.original copy %SM1_RSC% %SM1_RSC_ORIGINAL%.original > NUL
	if exist %SM1_RSC% del %SM1_RSC%

	if exist %EPOCWIND_COPY% del %EPOCWIND_COPY%

	if not exist "%TEST_RSC%" echo Shma Test: ERROR - test resource file does not exist & goto :EOF

	copy %TEST_RSC% %SM0_RSC% > NUL
	copy %TEST_RSC% %SM1_RSC% > NUL
	
	if exist \epoc32\winscw\c\testdata\runcounts\*.* del /Q \epoc32\winscw\c\testdata\runcounts\*.*

	echo restart=0 > \epoc32\winscw\c\Restart_ResultFile.txt
	echo register=0 > \epoc32\winscw\c\Register_ResultFile.txt
	
	if "%EPOC_COUNT%" NEQ "0"	call :epoc
	if "%EPOC_COUNT%" NEQ "0"	copy %EPOCWIND_ORIG% %EPOCWIND_COPY% >NUL
	if "%EPOC_COUNT%" NEQ "0"	call z\shmatest\sysmontest_checkEpocWind.bat %TEST_CASE%
	
	if "%EPOC_COUNT%" ==  "2"	call :epoc
	
	call :restore
	
goto :EOF

:uninstall
	if exist %EPOCROOT%epoc32\winscw\c\*ResultFile.txt del %EPOCROOT%epoc32\winscw\c\*ResultFile.txt
goto :EOF

:execTimeOut
	echo testexecute.exe z:\shmatest\shmatest_sysmon_%TEST_CASE%.script with Timeout
	set TIMEOUT=300
	\ExecTimeOut.exe "testexecute.exe z:\shmatest\shmatest_sysmon_%TEST_CASE%.script" %TIMEOUT%
goto :EOF

:testExecute
	echo testexecute.exe z:\shmatest\shmatest_sysmon_%TEST_CASE%.script
	testexecute.exe z:\shmatest\shmatest_sysmon_%TEST_CASE%.script
goto :EOF

:epocExecTimeOut
	set TIMEOUT=60
	\ExecTimeOut.exe "epoc.exe" %TIMEOUT%
goto :EOF

:epoc
	:: If using SYMBIAN overnight DABS test scripts, then utilise ExecTimeOut to ensure
	:: execution does not exceed expected execution time.
	if exist \ExecTimeOut.exe echo epoc.exe - will timeout automatically
	if exist \ExecTimeOut.exe call :epocExecTimeOut

	:: If simply executing manually, then call epoc.exe directly.
	if not exist \ExecTimeOut.exe echo epoc.exe - if does not terminate within 1 minute, terminate manually
	if not exist \ExecTimeOut.exe epoc.exe
goto :EOF

:test
	if exist %TEMP%\epocwind.out copy %TEMP%\epocwind.out %TEMP%\epocwind.out.temp > NUL

	:: If using SYMBIAN overnight DABS test scripts, then utilise ExecTimeOut to ensure
	:: execution does not exceed expected execution time.
	if exist \ExecTimeOut.exe call :execTimeOut

	:: If simply executing manually, then call testexecute directly.
	if not exist \ExecTimeOut.exe call :testExecute

	if exist %TEMP%\epocwind.out.temp copy %TEMP%\epocwind.out.temp %TEMP%\epocwind.out > NUL
	if exist %TEMP%\epocwind.out.temp del %TEMP%\epocwind.out.temp
goto :EOF

:wait
	echo INFO: Waiting %1 seconds
	ping 1.1.1.1 -w 1000 -n %1 >NUL
goto :EOF

:restore
	call :wait 2
	if exist %SM0_RSC_ORIGINAL%.original del %SM0_RSC%
	if exist %SM0_RSC_ORIGINAL%.original copy /Y %SM0_RSC_ORIGINAL%.original %SM0_RSC% > NUL
	if exist %SM1_RSC_ORIGINAL%.original del %SM1_RSC%
	if exist %SM1_RSC_ORIGINAL%.original copy /Y %SM1_RSC_ORIGINAL%.original %SM1_RSC% > NUL
	if exist %SM0_RSC_ORIGINAL%.original del %SM0_RSC_ORIGINAL%.original
	if exist %SM1_RSC_ORIGINAL%.original del %SM1_RSC_ORIGINAL%.original
goto :EOF

                  
:conndiscon
:regeiofcapsnops
:regeiofcapsok
:regeroscapsnops
:regeroscapsok
:regeroswmcapsnopm
:regeroswmcapsnops
:regeroswmcapsok
:regselfeiofcapsok
:regselferoscapsnops
:regselferoscapsok
:regselferoswmcapsnopm
:regselferoswmcapsnops
:regselferoswmcapsok
:reg1proctwice
:reg2procs
:regcritical
:regecnorcapsok
:regecnorcapsnops
:regselfecnorcapsok
:regselfecnorcapsnops
	set TEST_FLAG=
goto :EOF        

          
:failcount2failquick
:failcount2failwait
:faileiofcount0
:faileiofcount1fail
:faileroscount1fail
::faileroswmcount1fail
:restartfail
::restartfailwithmode1
::restartfailwithmode2
:restartreg
:startupregeiof
:startupregeioffailstart1
:startupregeioffailstart2
:startupregerosfaillater
:startupregerosfailstart
:startupregeroswmfaillater
:startupregeroswmfailstart
:failecnorcount1fail
	set TEST_FLAG=checkEpocWind
goto :EOF        

endlocal
