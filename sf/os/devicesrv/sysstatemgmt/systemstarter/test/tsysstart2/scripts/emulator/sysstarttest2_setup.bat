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

::TKAS	echo count=0 > \epoc32\winscw\c\T_DLL_ResultFile.txt
::TKAS	echo panic=0 > \epoc32\winscw\c\T_STFP_PanicResultFile.txt
goto :EOF


:uninstall
	call :restore
	if exist %EPOCROOT%epoc32\winscw\c\*ResultFile.txt del %EPOCROOT%epoc32\winscw\c\*ResultFile.txt
goto :EOF


:test
	testexecute.exe z:\sysstarttest\sysstarttest_tsysstart2_%TEST_CASE%.script
goto :EOF


:restore
	if exist %SM0_RSC%.original copy %SM0_RSC%.original %SM0_RSC% > NUL
	if exist %SM0_RSC%.original del %SM0_RSC%.original
goto :EOF


:procstart		
:procmon		
:procmondeferred
:: Anchor for future parsing of epocwind.out.
::	set TEST_FLAG=checkEpocWind
goto :EOF        


endlocal
