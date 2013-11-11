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
set RSC_LOCATION=Z\private\10205C44\test
set TEST_RSC=%RSC_LOCATION%\%2
set SM0_RSC=Z\private\10205C44\SSCForStartupMode0.RSC

goto :%COMMAND_CHOICE%

:install
	if not exist %SM0_RSC%.original copy %SM0_RSC% %SM0_RSC%.original > NUL
	if not exist "%TEST_RSC%" echo SysStart AmaStarter Test: ERROR - test resource file does not exist & goto :EOF
	copy %TEST_RSC% %SM0_RSC% > NUL
goto :EOF

:uninstall
	if exist %SM0_RSC%.original copy %SM0_RSC%.original %SM0_RSC% > NUL
	if exist %SM0_RSC%.original del %SM0_RSC%.original
goto :EOF

endlocal