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
set RSC_LOCATION=Z\private\10205C44
set TEST_CASE=%2
call :%TEST_CASE%
set TEST_RSC=%RSC_LOCATION%\%3
set SM0_RSC=%RSC_LOCATION%\SSCForStartupMode0.RSC

set EPOCWIND=%TEMP%\epocwind.out

goto :%COMMAND_CHOICE%

:install
	if not exist %SM0_RSC%.original copy %SM0_RSC% %SM0_RSC%.original > NUL

	if not exist "%TEST_RSC%" echo SysStart Test: ERROR - test resource file does not exist & goto :EOF

	copy %TEST_RSC% %SM0_RSC% > NUL

	if exist z\sysstarttest\TAck*.txt del z\sysstarttest\TAck*.txt

	echo dummy > %TEST_ACK1%
	echo dummy > %TEST_ACK2%

	echo count=0 > \epoc32\winscw\c\T_DLL_ResultFile.txt
	echo panic=0 > \epoc32\winscw\c\T_STFP_PanicResultFile.txt
goto :EOF

:uninstall
	call :restore
	
	if /i "%TEST_FLAG%"=="checkEpocWind" call :test
	if exist %EPOCROOT%epoc32\winscw\c\*ResultFile.txt del %EPOCROOT%epoc32\winscw\c\*ResultFile.txt
goto :EOF

:execTimeOut
	set TIMEOUT=300
	\ExecTimeOut.exe "testexecute.exe z:\sysstarttest\sysstarttest_%TEST_CASE%.script" %TIMEOUT%
goto :EOF

:testExecute
	testexecute.exe z:\sysstarttest\sysstarttest_%TEST_CASE%.script
goto :EOF

:test
	call z\sysstarttest\sysstarttest_checkEpocWind.bat %TEST_CASE%

	if exist %TEMP%\epocwind.out copy %TEMP%\epocwind.out %TEMP%\epocwind.out.temp > NUL

	:: If using SYMBIAN overnight DABS test scripts, then utilise ExecTimeOut to ensure
	:: execution does not exceed expected execution time.
	if exist \ExecTimeOut.exe call :execTimeOut

	:: If simply executing manually, then call testexecute directly.
	if not exist \ExecTimeOut.exe call :testExecute

	if exist %TEMP%\epocwind.out.temp copy %TEMP%\epocwind.out.temp %TEMP%\epocwind.out > NUL
	if exist %TEMP%\epocwind.out.temp del %TEMP%\epocwind.out.temp
goto :EOF

:restore
	if exist %SM0_RSC%.original copy %SM0_RSC%.original %SM0_RSC% > NUL
	if exist %SM0_RSC%.original del %SM0_RSC%.original
	if exist z\sysstarttest\TAck*.txt del z\sysstarttest\TAck*.txt
goto :EOF

:StfpCdToNcDefault
:StfpCdToNcRetry0Ignore
:StfpCdToNcRetry1Ignore 
	set TEST_ACK1=z\sysstarttest\TAckCriticalStatic.txt
	set TEST_ACK2=z\sysstarttest\TAckCriticalDynamic.txt
	set TEST_FLAG=
goto :EOF

:StfpCdToNcRetry0Panic 
:StfpCdToNcRetry1Panic
	set TEST_ACK1=z\sysstarttest\TAckCriticalStatic.txt
	set TEST_ACK2=z\sysstarttest\TAckCriticalDynamic.txt
	set TEST_FLAG=checkEpocWind
goto :EOF


:StfpCsToCdDefault              
:StfpCsToCdRetry0Ignore         
:StfpCsToCdRetry1Ignore   
	set TEST_ACK1=z\sysstarttest\TAckCriticalStatic.txt
	set TEST_ACK2=z\sysstarttest\TAckNonCritical.txt
	set TEST_FLAG=
goto :EOF        
      
:StfpCsToCdRetry0Panic          
:StfpCsToCdRetry1Panic  
	set TEST_ACK1=z\sysstarttest\TAckCriticalStatic.txt
	set TEST_ACK2=z\sysstarttest\TAckNonCritical.txt
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
	set TEST_ACK1=z\sysstarttest\TAckDummy1.txt
	set TEST_ACK2=z\sysstarttest\TAckDummy2.txt
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
	set TEST_ACK1=z\sysstarttest\TAckDummy1.txt
	set TEST_ACK2=z\sysstarttest\TAckDummy2.txt
	set TEST_FLAG=checkEpocWind
goto :EOF        

endlocal
