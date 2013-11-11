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

set RSC_LOCATION=\epoc32\data\Z\private\10205C44
set TEST_RSC=%RSC_LOCATION%\%2
set SM0_RSC=%RSC_LOCATION%\SSCForStartupMode0_target.RSC

set TEST_ROM_FLAG=false
if /i "%3"=="TEST_ROM_ONLY" set TEST_ROM_FLAG=true

call :%TEST_CASE%
set TEST_ACK_LOC1=\epoc32\data\z\sysstarttest\%TEST_ACK1%
set TEST_ACK_LOC2=\epoc32\data\z\sysstarttest\%TEST_ACK2%

set AUTOEXEC=autoexec.bat
set AUTOEXEC_LOC=\epoc32\data\z\sysstarttest\%AUTOEXEC%

set SYSSTARTTEST_IBY=\epoc32\rom\include\sysstarttest.iby

set EPOCWIND=%TEMP%\epocwind.out

set DLL_RESULT_FILE=\epoc32\data\z\sysstarttest\T_DLL_ResultFile.txt
set STFP_RESULT_FILE=\epoc32\data\z\sysstarttest\T_STFP_PanicResultFile.txt
set APP_RESULT_FILE=\epoc32\data\z\sysstarttest\T_AppPanicResultFile.txt

set DABS_SCRIPT=\logs\autorom\TESTEXECUTEsysstarttest_%TEST_CASE%.script.txt

set DABS_FLAG=%4

set HARDWARE_REFERENCE_PLATFORM=%5

goto :skipFunctions

:restore
	cd\
	if exist %SYSSTARTTEST_IBY%.original copy %SYSSTARTTEST_IBY%.original %SYSSTARTTEST_IBY% > NUL
	if exist %SYSSTARTTEST_IBY%.original del %SYSSTARTTEST_IBY%.original 

	if exist %SM0_RSC%.original copy %SM0_RSC%.original %SM0_RSC% > NUL
	if exist %SM0_RSC%.original del %SM0_RSC%.original 

	if exist \epoc32\data\z\sysstarttest\TAckCriticalStatic.txt  del \epoc32\data\z\sysstarttest\TAckCriticalStatic.txt 
	if exist \epoc32\data\z\sysstarttest\TAckCriticalDynamic.txt del \epoc32\data\z\sysstarttest\TAckCriticalDynamic.txt 
	if exist \epoc32\data\z\sysstarttest\TAckNonCritical.txt     del \epoc32\data\z\sysstarttest\TAckNonCritical.txt
	if exist \epoc32\data\z\sysstarttest\TAckDummy1.txt          del \epoc32\data\z\sysstarttest\TAckDummy1.txt
	if exist \epoc32\data\z\sysstarttest\TAckDummy2.txt          del \epoc32\data\z\sysstarttest\TAckDummy2.txt

	if exist %DABS_SCRIPT% del %DABS_SCRIPT% 
	if exist %AUTOEXEC_LOC% del %AUTOEXEC_LOC%
goto :EOF

:configure
	if not exist %SM0_RSC%.original copy %SM0_RSC% %SM0_RSC%.original > NUL
	copy %TEST_RSC% %SM0_RSC% > NUL

	if not exist %SYSSTARTTEST_IBY%.original copy %SYSSTARTTEST_IBY% %SYSSTARTTEST_IBY%.original > NUL
	echo dummy > %TEST_ACK_LOC1%
	echo data=DATAZ_\sysstarttest\%TEST_ACK1%  \sysstarttest\%TEST_ACK1% >> %SYSSTARTTEST_IBY%
	echo dummy > %TEST_ACK_LOC2%
	echo data=DATAZ_\sysstarttest\%TEST_ACK2%  \sysstarttest\%TEST_ACK2% >> %SYSSTARTTEST_IBY%
	echo data=DATAZ_\sysstarttest\%AUTOEXEC% \%AUTOEXEC% >> %SYSSTARTTEST_IBY%

	if exist %DLL_RESULT_FILE% echo copy z:\sysstarttest\T_DLL_ResultFile.txt c:\ >> %AUTOEXEC_LOC%
	if exist %APP_RESULT_FILE% echo copy z:\sysstarttest\T_AppPanicResultFile.txt c:\ >> %AUTOEXEC_LOC%
	if exist %STFP_RESULT_FILE% echo copy z:\sysstarttest\T_STFP_PanicResultFile.txt c:\ >> %AUTOEXEC_LOC%
	echo c: >> %AUTOEXEC_LOC%
	echo attrib -r >> %AUTOEXEC_LOC%

	if /i "%DABS_FLAG%"=="true" call :configureDabs & goto :EOF

	echo testexecute z:\sysstarttest\sysstarttest_%TEST_CASE%.script >> %AUTOEXEC_LOC%
	echo md e:\logs >> %AUTOEXEC_LOC%
	echo md e:\logs\testexecute >> %AUTOEXEC_LOC%
	echo copy c:\logs\testexecute\sysstarttest_%TEST_CASE%.htm e:\logs\testexecute\ >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="false" echo format k: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo c: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo del c:\*.txt >> %AUTOEXEC_LOC%
goto :EOF

:configureDabs 
	echo testexecute z:\sysstarttest\sysstarttest_%TEST_CASE%.script >> %DABS_SCRIPT% 
	echo thindump -nop c:\logs\testexecute\sysstarttest_%TEST_CASE%.htm >> %DABS_SCRIPT% 

	echo #include ^<thindump.iby^> >> %SYSSTARTTEST_IBY%
	echo data=\logs\autorom\TESTEXECUTEsysstarttest_%TEST_CASE%.script.txt   \test\TESTEXECUTEsysstarttest_%TEST_CASE%.script.txt >> %SYSSTARTTEST_IBY%
	echo data=\logs\autorom\dummytest.txt   \test\dummytest.txt >> %SYSSTARTTEST_IBY%
	echo file=ABI_DIR\BUILD_DIR\runtests.exe \test\runtests.exe >> %SYSSTARTTEST_IBY%

	echo md c:\logs >> %AUTOEXEC_LOC%
	echo md c:\logs\testresults >> %AUTOEXEC_LOC%
	echo md c:\system >> %AUTOEXEC_LOC%
	echo md c:\system\data >> %AUTOEXEC_LOC%
	echo z: >> %AUTOEXEC_LOC%
	echo cd test >> %AUTOEXEC_LOC%
	echo RUNTESTS TESTEXECUTEsysstarttest_%TEST_CASE%.script.txt -t 13 >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="false" echo format k: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo c: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo del c:\*.txt >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo z: >> %AUTOEXEC_LOC%
	echo RUNTESTS dummytest.txt -p >> %AUTOEXEC_LOC%
goto :EOF

:zipRom
	if exist rombld.img call zip sys$rom.zip rombld.img > NUL
	if exist rombld.symbol call zip symbol.zip rombld.symbol > NUL
	if exist rombld.img echo ROM built: \rom\%TEST_CASE%\sys$rom.zip
	if not exist sys$rom.zip echo ERROR: ROM failed to build.
goto :EOF

:addDllResultFile
	echo data=DATAZ_\sysstarttest\T_DLL_ResultFile.txt  \sysstarttest\T_DLL_ResultFile.txt >> %SYSSTARTTEST_IBY%
goto :EOF

:addStfpResultFile
	echo data=DATAZ_\sysstarttest\T_STFP_PanicResultFile.txt  \sysstarttest\T_STFP_PanicResultFile.txt >> %SYSSTARTTEST_IBY%
goto :EOF

:addAppResultFile
	echo data=DATAZ_\sysstarttest\T_AppPanicResultFile.txt  \sysstarttest\T_AppPanicResultFile.txt >> %SYSSTARTTEST_IBY%
goto :EOF

:buildTestRom
	cd\
	if exist \rom\%TEST_CASE%\testrom rd /s/q \rom\%TEST_CASE%\testrom
	md \rom\%TEST_CASE%\testrom
	cd \rom\%TEST_CASE%\testrom
 
	echo Building Test ROM for SysStart test case: %TEST_CASE%
	if exist %DLL_RESULT_FILE% call :addDllResultFile
	if exist %STFP_RESULT_FILE% call :addStfpResultFile
	if exist %APP_RESULT_FILE% call :addAppResultFile
	call buildrom -D_FULL_DEBUG -D_LFFSC %HARDWARE_REFERENCE_PLATFORM% techview.oby platsec.oby sysstarttest.iby -orombld.img> NUL
	call :zipRom
	if exist %DLL_RESULT_FILE% del %DLL_RESULT_FILE% 
	if exist %STFP_RESULT_FILE% del %STFP_RESULT_FILE% 
	copy %AUTOEXEC_LOC% \rom\%TEST_CASE%\testrom
goto :EOF

:build
	if not exist %TEST_RSC% echo SysStart Test: ERROR - test resource file does not exist & goto :EOF

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

	echo Building ROM for SysStart test case: %TEST_CASE%
	call buildrom -D_FULL_DEBUG %LFFSC_FLAG% %HARDWARE_REFERENCE_PLATFORM% techview.oby platsec.oby sysstarttest.iby -orombld.img> rombuild.txt
	call :zipRom
	copy %AUTOEXEC_LOC% \rom\%TEST_CASE%
	echo %HARDWARE_REFERENCE_PLATFORM% UIF-Techview SysStart - %TEST_CASE% > rom_desc.txt
	copy \rom\%TEST_CASE%\rombld.log \logs\armv5\%TEST_CASE%_rombld.log
goto :EOF

:StfpCdToNcDefault
:StfpCdToNcRetry0Ignore
:StfpCdToNcRetry1Ignore 
	set TEST_ACK1=TAckCriticalStatic.txt
	set TEST_ACK2=TAckCriticalDynamic.txt
	set TEST_FLAG=
goto :EOF

:StfpCdToNcRetry0Panic 
:StfpCdToNcRetry1Panic
	set TEST_ACK1=TAckCriticalStatic.txt
	set TEST_ACK2=TAckCriticalDynamic.txt
	set TEST_FLAG=checkEpocWind
goto :EOF


:StfpCsToCdDefault              
:StfpCsToCdRetry0Ignore         
:StfpCsToCdRetry1Ignore   
	set TEST_ACK1=TAckCriticalStatic.txt
	set TEST_ACK2=TAckNonCritical.txt
	set TEST_FLAG=
goto :EOF        
      
:StfpCsToCdRetry0Panic          
:StfpCsToCdRetry1Panic  
	set TEST_ACK1=TAckCriticalStatic.txt
	set TEST_ACK2=TAckNonCritical.txt
	set TEST_FLAG=checkEpocWind
goto :EOF        


:AppFailRetry0Ignore
:AppFailRetry1Ignore
:AppStartCd
:AppStartCs
:AppStartNc
:AppTimeoutRetry3Ignore
:DefMultGrp
:DefRendFailIgnore
:DefStartAllState
:DefStartCd
:DefStartCs
:DefStartFailIgnore
:DefStartNc
:DefTimeoutAppIgnore
:DefTimeoutProcIgnore
:DllCdFailIgnore  
:DllCdNormal             
:DllCsFailIgnore     
:DllCsFnMultipleRetry
:DllCsNormal             
:DllFnSharedData             
:DllNcFailIgnore              
:DllNcNormal 
:InitApparc
:ProcFailRetry0Ignore
:ProcFailRetry1Ignore
:ProcStartCd
:ProcStartCs
:ProcStartNc
:ProcTimeoutRetry3Ignore
:ReRunSysStart
	set TEST_ACK1=TAckDummy1.txt
	set TEST_ACK2=TAckDummy2.txt
	set TEST_FLAG=
goto :EOF        

  
:AppFailRetry0Panic
:AppFailRetry1Panic  
:AppStartFailPanic
:AppTimeoutRetry3Panic 
:DefInvalidAppRetry
:DefInvalidAppTimeout
:DefInvalidMultFail
:DefInvalidMultWait
:DefInvalidProcRetry
:DefInvalidProcTimeout
:DefNoMult1   
:DefNoMult2 
:DefNoMult3
:DefRendFailAppPanic
:DefRendFailProcPanic
:DefStartFailAppPanic
:DefStartFailProcPanic
:DefTimeoutAppPanic
:DefTimeoutProcPanic
:DllCdFailPanic           
:DllCsFailPanic         
:DllCsFnInvalidFail        
:DllInvalidOrdinal0      
:DllInvalidOrdinalRetry0Ignore 
:DllInvalidOrdinalRetry0Panic  
:DllInvalidOrdinalRetry1Ignore 
:DllInvalidOrdinalRetry1Panic     
:DllInvalidRetry0Ignore     
:DllInvalidRetry1Ignore     
:DllInvalidRetry0Panic      
:DllInvalidRetry1Panic         
:DllInvalidCustomData       
:DllNcFailPanic     
:InitApparcBeforeServer
:ProcFailRetry0Panic
:ProcFailRetry1Panic  
:ProcStartFailPanic      
:ProcTimeoutRetry3Panic
	set TEST_ACK1=TAckDummy1.txt
	set TEST_ACK2=TAckDummy2.txt
	set TEST_FLAG=checkEpocWind
goto :EOF        
:skipFunctions

call :build

endlocal
