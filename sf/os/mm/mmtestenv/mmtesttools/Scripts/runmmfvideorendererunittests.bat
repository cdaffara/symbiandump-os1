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

del e:\logs\testresults\tsu_mmf_videorenderer*.*
md e:\logs\
md e:\logs\testresults\


REM *** VideoRenderer Unit Tests ************************************


testframework tsu_mmf_videorenderer.script
move c:\logs\testresults\*.htm e:\logs\testresults\

REM Panic test is only for emulator UDEB, they should not run on hardware, see DEF125363.
REM testframework tsu_mmf_videorenderer_debug.script
REM move c:\logs\testresults\*.htm e:\logs\testresults\
