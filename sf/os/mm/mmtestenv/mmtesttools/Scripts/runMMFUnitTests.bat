rem Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem N.B. Tabs must NOT be used in this batch file!
rem (This is due to a bug in EShell.exe, from which this batch file is run)
rem Create directories on CF card for the result logs
rem Don't ask me why, but immediately after a test has run, attempting to
rem copy the log file returns -14 Not Found. Hence we copy all logs to the CF
rem after each test has run.
rem 
rem

runMMFAclntUnitTests.bat
runMMFActrlUnitTests.bat
runMMFAcodUnitTests.bat
runMMFAfmtUnitTests.bat
runMMFBaseClUnitTests.bat
runMMFCtlFrmUnitTests.bat
runMMFVclntUnitTests.bat
runMMFVclntDRMUnitTests.bat
runMMFDevSoundUnitTests.bat
runMMFSDevSoundUnitTests.bat
runMMFMIDIClntUnitTests.bat
runMMFClntUnitTests.bat
runMMFSwCodecWrapperUnitTests.bat
runMMFSwCodecDevicesUnitTests.bat
runMMFTestFrameworkUnitTests.bat
runMMFDevVideoUnitTests.bat
runMMFAsrClientUnitTests.bat
runMMFSbcUnitTests.bat
runMMFMIDIClntRfUnitTests.bat
runMMFGefUnitTests.bat
runMMFAclntDRMUnitTests.bat
runMMFSrsSnkUnitTests.bat
runMMFOggctrlUnitTests.bat
runMMFAvictrlUnitTests.bat
runmmfrufunittests.bat
runMMFSecureDrmUnitTests.bat
runmmfaclntsecuredrmunittests.bat
REM run runmmfvideorendererunittests.bat if SYMBIAN_GRAPHICS_USE_GCE is enabled
runmmfvideorendererunittests.bat

REM following bat file should be run separately when SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT is enabled
REM runmmfsubtitleunittests.bat
