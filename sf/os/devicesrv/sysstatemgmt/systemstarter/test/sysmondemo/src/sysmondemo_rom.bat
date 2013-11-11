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
set RSC_LOCATION=\epoc32\data\Z\private\10205C44


set TEST_RSC=%RSC_LOCATION%\sysmondemo_hardware.rsc
set TEST1_RSC=%RSC_LOCATION%\sysmondemo109.RSC
set TEST2_RSC=%RSC_LOCATION%\sysmondemo108_hardware.rsc

set SM0_RSC=%RSC_LOCATION%\SSCForStartupMode0_target.RSC
set SM109_RSC=%RSC_LOCATION%\SSCForStartupMode109_target.RSC
set SM108_RSC=%RSC_LOCATION%\SSCForStartupMode108_target.RSC

call copy %TEST_RSC% %SM0_RSC%
call copy %TEST1_RSC% %SM109_RSC%
call copy %TEST2_RSC% %SM108_RSC%

call buildrom.cmd  -D_SERIAL_DOWNLOAD -D_DEBUG PlatSec   h4hrp techview.oby sysmondemo.iby -orombld.img

call copy rombld.img sys$rom.img
call del sys$rom.zip

call zip sys$rom.zip sys$rom.img
