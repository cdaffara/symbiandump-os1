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

testexecute.exe z:\ssmatest\ssmatest_tss_procstart.script			
testexecute.exe z:\ssmatest\ssmatest_tss_appstart.script
testexecute.exe z:\ssmatest\ssmatest_tss_procstartmon.script
testexecute.exe z:\ssmatest\ssmatest_tss_apptimeout.script
testexecute.exe z:\ssmatest\ssmatest_tss_startandcancel.script
testexecute.exe z:\ssmatest\ssmatest_tss_appstartmultiple.script
testexecute.exe z:\ssmatest\ssmatest_tss_appstartapparcinit.script
testexecute.exe z:\ssmatest\ssmatest_tss_procstartwithfandf.script
testexecute.exe z:\ssmatest\ssmatest_tss_errors.script

:: Ignore the following if using this script for emulator testing.
:: Logs are automatically copied on hardware to MMC
:: which is assumed to be drive E:
@echo off
if not exist c:\logs\testexecute\ goto :EOF
md e:\logs
md e:\logs\testexecute

copy c:\logs\testexecute\ssmatest_tss_*.htm e:\logs\testexecute\