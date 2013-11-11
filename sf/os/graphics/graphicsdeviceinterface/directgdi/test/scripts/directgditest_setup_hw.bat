rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

@echo off

rem ================================================================================================
rem
rem NOTES:
rem  - This batch file is used by DABS to automate DirectGDI tests.
rem  - The reason behind it is the amount of data used by the tests, which is too 
rem		big for sys$rom.bin to handle
rem  - This file is both useful for overnight automated testing in DABS as well as a reference
rem		for the correct setup of the tests during a manual execution. 
rem
rem USAGE:
rem   install -> Installs to the MMC card all data needed by the test. The process of searching 
rem		for the usb drive is automated.
rem  :copy -> Retrieves from the MMC card all the output, copying it back to the PC performing 
rem		the automated build.
rem  :parse -> Once the data is in the build PC, a set of Python scripts are run to generate 
rem		a report page to present the results in an html format.
rem
rem CAVEAT:
rem  - From dabslaunch_v6, DABS has been updated as part of the BATS project. Due to 
rem		the nature of BATS, the mechanism for sending and retrieving data to and from the MMC card
rem		has been updated, as there is no longer a 1 to 1 connection between a build machine and a
rem		hardware board (making the usb search mechanism useless). 
rem  - This results in a scenario where in a BATS system, only DABS knows in which board a 
rem		particular rom is going to run. For that reason, both :install and :copy are not used within
rem		the BATS scripts, but nevertheless, they have been left here for reference as well as for
rem		a helping tool when doing the setup manually.
rem
rem ================================================================================================

setlocal

:: Initialise local variables
set TARGET_PLATFORM=%1
set COMMAND_CHOICE=%2


goto :%COMMAND_CHOICE%


rem ===============================================================  
rem ==		  												=======
rem ==		  SUPPORT FUNCTIONS								=======
rem ==		  												=======
rem ===============================================================  

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
	for /L %%j in (2,1,%1) do call :wait1
	:wait1
	for %%i in (%time%) do set startsec=%%~ni%
	:loop_wait
	set endsec=
	for %%k in (%time%) do set endsec=%%~nk%
	if %endsec%==%startsec% goto loop_wait
goto :EOF

:searchForUSB
	for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do call :checkForUsbDrive %%i
goto :EOF

:checkAutomation
	set HARDWARE_USB=false
	if not exist "\trgtest.exe"                          echo INFO: \trgtest.exe not found & goto :EOF
	if not exist "\hardwarecontrol.exe"                   echo INFO: \hardwarecontrol.exe not found & goto :EOF
	if not exist "\devcon.exe"                           echo INFO: \devcon.exe not found & goto :EOF
	
	call \devcon.exe remove "USB\Vid_0e22&Pid_1112&Rev_0305"

	echo INFO: Hardware OFF
	call \hardwarecontrol.exe --off
	
	call :wait 2

	echo INFO: Hardware ON
	call \hardwarecontrol.exe --on

	call :wait 15
	call :searchForUSB

	if /i "%HARDWARE_USB%"=="true" goto :skipUsbSearch

	echo INFO: USB Drive not found, trying again...
	echo INFO: Hardware OFF
	call \hardwarecontrol.exe --off
	
	call :wait 2

	echo INFO: Hardware ON
	call \hardwarecontrol.exe --on

	call :wait 15
	call :searchForUSB

	if /i "%HARDWARE_USB%"=="true" goto :skipUsbSearch
	call :usbNotFound 
	goto :EOF

:skipUsbSearch
	if not exist \logs\%TARGET_PLATFORM% md \logs\%TARGET_PLATFORM% 
goto :EOF

:usbNotFound
	echo ERROR: Target USB Drive not found
	call \hardwarecontrol.exe --off
goto :EOF

:hardwareSwitchOff
	call :wait 15
	chkdsk %HARDWARE_USB_DRIVE%:
	call :wait 2
	call \devcon.exe remove "USB\Vid_0e22&Pid_1112&Rev_0305"
	call :wait 2
	echo INFO: Hardware OFF
	call \hardwarecontrol.exe --off
goto :EOF


rem ===============================================================  
rem ==		  												=======
rem ==		  INSTALL										=======
rem ==		  												=======
rem ===============================================================  

:install

	call :checkAutomation
	if /i "%HARDWARE_USB%"=="false" echo ERROR: DirectGDI test initialisation failed. USB not found! & goto :EOF
	
	 rd /s /q %HARDWARE_USB_DRIVE%:\img
	 md %HARDWARE_USB_DRIVE%:\img\test

	call :hardwareSwitchOff

	goto :EOF



rem ===============================================================  
rem ==		  												=======
rem ==		  EXTRACT RESULTS FROM MMC						=======
rem ==		  												=======
rem ===============================================================  

:copy

	call :checkAutomation
	if /i "%HARDWARE_USB%"=="false" echo ERROR: DirectGDI copy results from MMC card failed. USB not found! & goto :EOF
	
	rem copy test mbms from MMC card
	set CDRIVE=%EPOCROOT%epoc32\data\c
	xcopy /EIYR %HARDWARE_USB_DRIVE%:\img\test 	%CDRIVE%\img\test

	call :hardwareSwitchOff
	goto :EOF



rem ===============================================================  
rem ==		  												=======
rem ==		  PARSE RESULTS ONCE IN THE PC					=======
rem ==		  												=======
rem ===============================================================  

:parse

	rem copy reference mbms from source
	chdir %EPOCROOT%
	set CDRIVE=%EPOCROOT%epoc32\data\c
	unzip -qo %CDRIVE%\img\reference.zip -d %CDRIVE%\img\ref
	rem the next one overwrites some of the mbm copied in the previous copy.
	unzip -qo %CDRIVE%\img\replacements_from_directgdi\replacement.zip -d %CDRIVE%\img\ref
	
	rem create logs directory
	if not exist %CDRIVE%\logs md %CDRIVE%\logs
	
	rem convert .mbm files to .bmp files and run the bitmap comparison tests, and create html reports
	chdir %CDRIVE%\img
	perl runtest.pl %TARGET_PLATFORM%
	
	set LOGDIR=logs
	PUSHD \
	FOR /D %%G IN (logs*) DO (IF EXIST %%G\%TARGET_PLATFORM%\directgdi_flag SET LOGDIR=%%G)
	POPD
	
	set RESULTS=\%LOGDIR%\%TARGET_PLATFORM%\directgdi_results
	set ONBSUMMARY=\%LOGDIR%\%TARGET_PLATFORM%
	rem copy difference bitmaps, test image bitmaps and reference image bitmaps to results directories
	xcopy /IYR %CDRIVE%\img\ref\*.bmp %RESULTS%\img\ref
	xcopy /IYR %CDRIVE%\img\test\*.bmp %RESULTS%\img\test
	xcopy /IYR %CDRIVE%\img\RGBDiff*.bmp %RESULTS%\img
	rem copy results web pages
	xcopy /SIYR %CDRIVE%\logs\*.html %RESULTS%\logs
	xcopy /SIYR %CDRIVE%\*.htm %ONBSUMMARY%
	goto :EOF


goto :EOF        

endlocal
