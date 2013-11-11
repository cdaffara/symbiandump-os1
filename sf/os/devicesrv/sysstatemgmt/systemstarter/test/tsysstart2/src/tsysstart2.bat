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
set SM0_RSC=%RSC_LOCATION%\SSCForStartupMode0.RSC

call copy %SM0_RSC% bak_up0.rsc

set TEST_RSC=%RSC_LOCATION%\tsysstart2-procstart_emulator.RSC
call copy %TEST_RSC% %SM0_RSC%
testexecute.exe z:\sysstarttest\sysstarttest_tsysstart2_procstart.script

set TEST_RSC=%RSC_LOCATION%\tsysstart2-procmon_emulator.RSC
call copy %TEST_RSC% %SM0_RSC%
testexecute.exe z:\sysstarttest\sysstarttest_tsysstart2_procmon.script

set TEST_RSC=%RSC_LOCATION%\tsysstart2-procmondeferred_emulator.RSC
call copy %TEST_RSC% %SM0_RSC%
testexecute.exe z:\sysstarttest\sysstarttest_tsysstart2_procmondeferred.script


copy bak_up0.rsc %SM0_RSC%
