rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem

REM Remove old results from MMC
del e:\logs\testexecute\te_tsi_icl_excludedfiles_01suite.htm
md e:\logs
md e:\logs\testexecute

REM Create destination directories for test images
md c:\te_tsi_icl_excludedfiles_01
md c:\te_tsi_icl_excludedfiles_01\inputimages
md c:\te_tsi_icl_excludedfiles_01\refimages


REM Copy test images from MMC to ROM
copy e:\te_tsi_icl_excludedfiles_01\*.* c:\te_tsi_icl_excludedfiles_01
copy e:\te_tsi_icl_excludedfiles_01\inputimages\*.* c:\te_tsi_icl_excludedfiles_01\inputimages\*.*
copy e:\te_tsi_icl_excludedfiles_01\refimages\*.* c:\te_tsi_icl_excludedfiles_01\refimages\*.*

REM Run the tests
testexecute z:\testdata\scripts\te_tsi_icl_excludedfiles_01suite.script

REM Move the results file from the ROM to the MMC
move c:\logs\testexecute\te_tsi_icl_excludedfiles_01suite.htm e:\logs\testexecute\te_tsi_icl_excludedfiles_01suite.htm

REM Delete the test images
del c:\te_tsi_icl_excludedfiles_01\*.*
del c:\te_tsi_icl_excludedfiles_01\inputimages\*.*
del c:\te_tsi_icl_excludedfiles_01\refimages\*.*
