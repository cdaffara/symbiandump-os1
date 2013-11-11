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

:: Copyright (c) Symbian Software Ltd 2007-2008. All rights reserved

testexecute.exe z:\ssmatest\ssmatest_cmd_commandlist.script
testexecute.exe z:\ssmatest\ssmatest_cmd_resourcereader.script
testexecute.exe z:\ssmatest\ssmatest_cmd_customcommand.script
testexecute.exe z:\ssmatest\ssmatest_cmd_finalisedrives.script
testexecute.exe z:\ssmatest\ssmatest_cmd_loadsup.script
testexecute.exe z:\ssmatest\ssmatest_cmd_persisthalattributes.script
testexecute.exe z:\ssmatest\ssmatest_cmd_poweroff.script
testexecute.exe z:\ssmatest\ssmatest_cmd_publishswp.script
testexecute.exe z:\ssmatest\ssmatest_cmd_publishsystemstate.script
testexecute.exe z:\ssmatest\ssmatest_cmd_reqswpchange.script
testexecute.exe z:\ssmatest\ssmatest_cmd_amastarter.script
testexecute.exe z:\ssmatest\ssmatest_cmd_multiplewait.script
testexecute.exe z:\ssmatest\ssmatest_cmd_startapp.script
testexecute.exe z:\ssmatest\ssmatest_cmd_startprocess.script
testexecute.exe z:\ssmatest\ssmatest_cmd_waitforapparcinit.script
testexecute.exe z:\ssmatest\ssmatest_cmd_setpandskey.script
testexecute.exe z:\ssmatest\ssmatest_cmd_createswp.script
testexecute.exe z:\ssmatest\ssmatest_cmd_custcmd_neverunload.script
testexecute.exe z:\ssmatest\ssmatest_cmd_i_bics.script
testexecute.exe z:\ssmatest\ssmatest_cmd_runcletest.script

:: Ignore the following if using this script for emulator testing.
:: Logs are automatically copied on hardware to MMC
:: which is assumed to be drive E:
@echo off
if not exist c:\logs\testexecute\ goto :eof
md e:\logs
md e:\logs\testexecute
copy c:\logs\testexecute\ssmatest_cmd*.htm e:\logs\testexecute\

:eof
