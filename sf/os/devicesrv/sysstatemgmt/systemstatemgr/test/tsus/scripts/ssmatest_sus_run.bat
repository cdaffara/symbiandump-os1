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

testexecute.exe z:\ssmatest\ssmatest_sus_adaptionserver.script
testexecute.exe z:\ssmatest\ssmatest_sus_loadplugin.script
testexecute.exe z:\ssmatest\ssmatest_sus_platsec.script
testexecute.exe z:\ssmatest\ssmatest_sus_platsec2.script
testexecute.exe z:\ssmatest\ssmatest_sus_supinfo.script
testexecute.exe z:\ssmatest\ssmatest_sus_suploader.script
testexecute.exe z:\ssmatest\ssmatest_sus_utilserver.script
testexecute.exe z:\ssmatest\ssmatest_sus_adaptationpluginloader.script
testexecute.exe z:\ssmatest\ssmatest_sus_adptpluginloaderoom.script

:: Ignore the following if using this script for emulator testing.
:: Logs are automatically copied on hardware to MMC
:: which is assumed to be drive E:
@echo off
if not exist c:\logs\testexecute\ goto :EOF
md e:\logs
md e:\logs\testexecute
copy c:\logs\testexecute\ssmatest_sus*.htm e:\logs\testexecute\