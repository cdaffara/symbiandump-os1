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
rem  - This file is both useful for overnight automated testing in DABS as well as a reference
rem		for the correct setup of the tests during a manual execution. 
rem
rem USAGE:
rem  :install -> Installs to the correct location all data needed by the test. This is executed as 
rem		part of the "pre cmd" specified in the DABS tests lists.
rem  :uninstall -> Retrieves all the output from the test, copying it to the appropriate 
rem		location. This is executed as part of the "post cmd" specified in the DABS tests lists.
rem
rem ================================================================================================

setlocal

:: Initialise local variables
set TARGET_PLATFORM=%1
set COMMAND_CHOICE=%2

set CDRIVE=%EMULATOR_C_DIR%
set ZDRIVE=%EPOCROOT%epoc32\data\Z

rem install / uninstall iType rasteriser
if not exist "%ZDRIVE%\ityperast_config.cmd" echo INFO: %ZDRIVE%\ityperast_config.cmd not found & goto :EOF
call %ZDRIVE%\ityperast_config.cmd %COMMAND_CHOICE%  %TARGET_PLATFORM%

goto :%COMMAND_CHOICE%

:install
	rem nothing else to do
	goto :EOF
	
:uninstall
	chdir %EPOCROOT%
	rem copy reference image mbms from source to directory used for comparisons
	unzip -qo %CDRIVE%\img\reference.zip -d %CDRIVE%\img\ref
	rem the next one overwrites some of the mbm copied in the previous copy.
	unzip -qo %CDRIVE%\img\replacements_from_directgdi\replacement.zip -d %CDRIVE%\img\ref

	rem convert .mbm files to .bmp files and run the bitmap comparison tests, and create html reports
	chdir %CDRIVE%\img
	perl runtest.pl %TARGET_PLATFORM%
	
	set LOGDIR=logs
	PUSHD \
	FOR /D %%G IN (logs*) DO (IF EXIST %%G\%TARGET_PLATFORM% SET LOGDIR=%%G)
	POPD
	
	set RESULTS=\%LOGDIR%\%TARGET_PLATFORM%\directgdi_results
	set ONBSUMMARY=\%LOGDIR%\%TARGET_PLATFORM%
	if defined COMMANDER_WORKSPACE set RESULTS=%EPOCROOT%epoc32\logs\directgditest\directgdi_results
	if defined COMMANDER_WORKSPACE set ONBSUMMARY=%EPOCROOT%epoc32\logs\directgditest
	rem copy difference bitmaps, test image bitmaps and reference image bitmaps to results directories
	xcopy /IYR %CDRIVE%\img\ref\*.bmp %RESULTS%\img\ref
	xcopy /IYR %CDRIVE%\img\test\*.bmp %RESULTS%\img\test
	xcopy /IYR %CDRIVE%\img\RGBDiff*.bmp %RESULTS%\img
	rem copy results web pages
	xcopy /SIYR %CDRIVE%\logs\*.html %RESULTS%\logs
	copy %CDRIVE%\directgditest_image_results.htm %ONBSUMMARY%
	goto :EOF

goto :EOF        

endlocal
