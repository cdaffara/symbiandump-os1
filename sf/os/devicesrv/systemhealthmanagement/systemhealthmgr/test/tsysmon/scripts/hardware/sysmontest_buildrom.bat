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
set TEST_CASE=%1

set RSC_LOCATION=\epoc32\data\Z\private\2000d75b\startup
set RSC_TEST_LOCATION=\epoc32\data\Z\private\2000d75b\startup\test

set TEST_RSC=%RSC_TEST_LOCATION%\%2
set SM0_RSC=%RSC_LOCATION%\0\gsastartupcmdlist0_target.RSC
set SM0_RSC_ORIGINAL=%RSC_LOCATION%\test\gsastartupcmdlist0_target.RSC

set SM1_RSC=%RSC_LOCATION%\1\gsastartupcmdlist1_target.RSC
set SM1_RSC_ORIGINAL=%RSC_LOCATION%\test\gsastartupcmdlist1_target.RSC

set TEST_ROM_FLAG=false
if /i "%3"=="TEST_ROM_ONLY" set TEST_ROM_FLAG=true

call :%TEST_CASE%

set AUTOEXEC=autoexec.bat
set AUTOEXEC_LOC=\epoc32\data\z\shmatest\%AUTOEXEC%

set shmatest_IBY=\epoc32\rom\include\sysmontest.iby

set EPOCWIND=%TEMP%\epocwind.out

set RESTART_RESULT_FILE=\epoc32\data\z\shmatest\Restart_ResultFile.txt
set REGISTER_RESULT_FILE=\epoc32\data\z\shmatest\Register_ResultFile.txt

set DABS_SCRIPT=\logs\autorom\TESTEXECUTEsysmontest_%TEST_CASE%.script.txt

set DABS_FLAG=%4

set HARDWARE_REFERENCE_PLATFORM=%5

goto :skipFunctions

:restore
	cd\
	if exist %shmatest_IBY%.original copy %shmatest_IBY%.original %shmatest_IBY% > NUL
	if exist %shmatest_IBY%.original del %shmatest_IBY%.original 

	if exist %SM0_RSC_ORIGINAL%.original del %SM0_RSC%
	if exist %SM0_RSC_ORIGINAL%.original copy %SM0_RSC_ORIGINAL%.original %SM0_RSC% > NUL
	if exist %SM1_RSC_ORIGINAL%.original del %SM1_RSC%
	if exist %SM1_RSC_ORIGINAL%.original copy %SM1_RSC_ORIGINAL%.original %SM1_RSC% > NUL
	if exist %SM0_RSC_ORIGINAL%.original del %SM0_RSC_ORIGINAL%.original
	if exist %SM1_RSC_ORIGINAL%.original del %SM1_RSC_ORIGINAL%.original

	if exist %DABS_SCRIPT% del %DABS_SCRIPT% 
	if exist %AUTOEXEC_LOC% del %AUTOEXEC_LOC%
goto :EOF

:configure
	if not exist %SM0_RSC_ORIGINAL%.original copy %SM0_RSC% %SM0_RSC_ORIGINAL%.original > NUL
	if exist %SM0_RSC% del %SM0_RSC%

	if not exist %SM1_RSC_ORIGINAL%.original copy %SM1_RSC% %SM1_RSC_ORIGINAL%.original > NUL
	if exist %SM1_RSC% del %SM1_RSC%

	copy %TEST_RSC% %SM0_RSC% > NUL
	copy %TEST_RSC% %SM1_RSC% > NUL

	if not exist %shmatest_IBY%.original copy %shmatest_IBY% %shmatest_IBY%.original > NUL
	echo data=DATAZ_\shmatest\%AUTOEXEC% \%AUTOEXEC% >> %shmatest_IBY%
	echo data=DATAZ_\private\2000d75b\startup\1\gsastartupcmdlist1_target.RSC \private\2000d75b\startup\1\gsastartupcmdlist1_target.RSC >> %shmatest_IBY%

	if exist %RESTART_RESULT_FILE% echo copy z:\shmatest\Restart_ResultFile.txt c:\ >> %AUTOEXEC_LOC%
	if exist %REGISTER_RESULT_FILE% echo copy z:\shmatest\Register_ResultFile.txt c:\ >> %AUTOEXEC_LOC%
	echo c: >> %AUTOEXEC_LOC%
	echo attrib -r >> %AUTOEXEC_LOC%

	if /i "%DABS_FLAG%"=="true" call :configureDabs & goto :EOF

	echo testexecute z:\shmatest\scripts\sysmontest_%TEST_CASE%.script >> %AUTOEXEC_LOC%
	echo md e:\logs >> %AUTOEXEC_LOC%
	echo md e:\logs\testexecute >> %AUTOEXEC_LOC%
	echo copy c:\logs\testexecute\sysmontest_%TEST_CASE%.htm e:\logs\testexecute\ >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="false" echo format k: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo c: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo del c:\*.txt >> %AUTOEXEC_LOC%
goto :EOF

:configureDabs 
	echo testexecute z:\shmatest\scripts\sysmontest_%TEST_CASE%.script >> %DABS_SCRIPT% 
	echo thindump -nop c:\logs\testexecute\sysmontest_%TEST_CASE%.htm >> %DABS_SCRIPT% 

	echo #include ^<thindump.iby^> >> %shmatest_IBY%
	echo data=\logs\autorom\TESTEXECUTEsysmontest_%TEST_CASE%.script.txt   \test\TESTEXECUTEsysmontest_%TEST_CASE%.script.txt >> %shmatest_IBY%
	echo data=\epoc32\data\z\shmatest\sysmontest_delay.txt  \test\sysmontest_delay.txt >> %shmatest_IBY%
	echo data=\epoc32\data\z\shmatest\sysmontest_delay.script  \test\sysmontest_delay.script >> %shmatest_IBY%
	echo file=ABI_DIR\BUILD_DIR\runtests.exe \test\runtests.exe >> %shmatest_IBY%

	echo md c:\logs >> %AUTOEXEC_LOC%
	echo md c:\logs\testresults >> %AUTOEXEC_LOC%
	echo md c:\system >> %AUTOEXEC_LOC%
	echo md c:\system\data >> %AUTOEXEC_LOC%
	echo z: >> %AUTOEXEC_LOC%
	echo cd test >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo RUNTESTS TESTEXECUTEsysmontest_%TEST_CASE%.script.txt -t 13 >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="false" echo format k: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo c: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo del c:\*.txt >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo z: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo del c:\testdata\runcounts\*.* >> %AUTOEXEC_LOC%
	echo RUNTESTS sysmontest_delay.txt -p >> %AUTOEXEC_LOC%
goto :EOF

:zipRom
	if exist rombld.img call zip sys$rom.zip rombld.img > NUL
	if exist rombld.symbol call zip symbol.zip rombld.symbol > NUL
	if exist rombld.img echo ROM built: \rom\%TEST_CASE%\sys$rom.zip
	if not exist sys$rom.zip echo ERROR: ROM failed to build.
goto :EOF

:addRestartResultFile
	echo data=DATAZ_\shmatest\Restart_ResultFile.txt  \shmatest\Restart_ResultFile.txt >> %shmatest_IBY%
goto :EOF

:addRegisterResultFile
	echo data=DATAZ_\shmatest\Register_ResultFile.txt  \shmatest\Register_ResultFile.txt >> %shmatest_IBY%
goto :EOF



:buildTestRom
	cd\
	if exist \rom\%TEST_CASE%\testrom rd /s/q \rom\%TEST_CASE%\testrom
	md \rom\%TEST_CASE%\testrom
	cd \rom\%TEST_CASE%\testrom
 
	echo Building Test ROM for Shma test case: %TEST_CASE%
	if exist %RESTART_RESULT_FILE% call :addRestartResultFile
	if exist %REGISTER_RESULT_FILE% call :addRegisterResultFile
	call buildrom -D_FULL_DEBUG -D_LFFSC %HARDWARE_REFERENCE_PLATFORM% techview.oby platsec.oby sysmontest.iby -orombld.img> NUL
	call :zipRom
	if exist %RESTART_RESULT_FILE% del %RESTART_RESULT_FILE% 
	if exist %REGISTER_RESULT_FILE% del %REGISTER_RESULT_FILE% 
	copy %AUTOEXEC_LOC% \rom\%TEST_CASE%\testrom
goto :EOF

:build
	if not exist %TEST_RSC% echo Shma Test: ERROR - test resource file does not exist & goto :EOF

	call :restore
	call :configure

	if "%TEST_ROM_FLAG%"=="false" call :buildRom
	if "%TEST_ROM_FLAG%"=="true"  call :buildTestRom

	call :restore
goto :EOF

:buildRom
	set LFFSC_FLAG=
	if /i "%TEST_FLAG%"=="checkEpocWind" set LFFSC_FLAG=-D_LFFSC

	if exist \rom\%TEST_CASE% rd /s/q \rom\%TEST_CASE%

	cd\
	md \rom\%TEST_CASE%
	cd \rom\%TEST_CASE%
	if /i "%TEST_FLAG%"=="checkEpocWind" md \rom\%TEST_CASE%\TEST_ROM_REQUIRED

	echo Building ROM for Shma test case: %TEST_CASE%
	call buildrom -D_FULL_DEBUG %LFFSC_FLAG% %HARDWARE_REFERENCE_PLATFORM% techview.oby platsec.oby sysmontest.iby -orombld.img> rombuild.txt
	call :zipRom
	copy %AUTOEXEC_LOC% \rom\%TEST_CASE%
	echo %HARDWARE_REFERENCE_PLATFORM% UIF-Techview Shma - %TEST_CASE% > rom_desc.txt
goto :EOF

:failcount2failquick
:failcount2failwait
:faileiofcount0
:faileiofcount1fail
:faileroscount1fail
:faileroswmcount1fail
:restartfail
:restartfailwithmode1
:restartfailwithmode2
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
 
:skipFunctions

call :build

endlocal
