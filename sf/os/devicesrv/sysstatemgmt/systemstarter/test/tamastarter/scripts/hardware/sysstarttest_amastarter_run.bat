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

:: *******************************************************************
:: Any ADDITIONS or DELETIONS to the test cases in this script MUST
:: be copied to the partner script that is used on emulator:
:: ..\emulator\sysstarttest_amastarter_run.bat
:: *******************************************************************

testexecute.exe z:\sysstarttest\sysstarttest_amastarter_invalidsid.script
testexecute.exe z:\sysstarttest\sysstarttest_amastarter_standalone.script
testexecute.exe z:\sysstarttest\sysstarttest_amastarter_startl.script


:: Ignore the following if using this script for emulator testing.
:: Logs are automatically copied on hardware to MMC
:: which is assumed to be drive E:
@echo off
if not exist c:\logs\testexecute\ goto :EOF
md e:\logs
md e:\logs\testexecute
copy c:\logs\testexecute\sysstarttest_amastarter*.htm e:\logs\testexecute\