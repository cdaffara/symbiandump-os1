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
rem

del E:\Logs\TestResults\TSU_MMF_SbcCodec.htm
del E:\Logs\TestResults\TSU_MMF_SbcCodec_ALLOC.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** SBC  TESTS ******************
REM ***********************************************

md c:\sbc\
md c:\sbc\ref\

copy e:\unittest\mmf\sbc\ref\*.sbc c:\sbc\ref\

TestFrameworkNone TSU_MMF_SbcCodec.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

TestFrameworkNone TSU_MMF_SbcCodec_ALLOC.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\sbc\ref\*.*
del c:\sbc\*.*