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

setlocal

set COMMAND_CHOICE=%1
set RSC_LOCATION=Z\private\10205C44

set TEST_RSC=%RSC_LOCATION%\sysmondemo.RSC
set TEST1_RSC=%RSC_LOCATION%\sysmondemo109.RSC
set TEST2_RSC=%RSC_LOCATION%\sysmondemo108.RSC

set SM0_RSC=%RSC_LOCATION%\SSCForStartupMode0.RSC
set SM109_RSC=%RSC_LOCATION%\SSCForStartupMode109.RSC
set SM108_RSC=%RSC_LOCATION%\SSCForStartupMode108.RSC


call copy %SM0_RSC% bak_up0.rsc
call copy %SM109_RSC% bak_up109.rsc
call copy %SM108_RSC% bak_up108.rsc


call copy %TEST_RSC% %SM0_RSC%
call copy %TEST1_RSC% %SM109_RSC%
call copy %TEST2_RSC% %SM108_RSC%


call epoc.exe

copy bak_up0.rsc %SM0_RSC%
copy bak_up109.rsc %SM109_RSC%
copy bak_up108.rsc %SM108_RSC%