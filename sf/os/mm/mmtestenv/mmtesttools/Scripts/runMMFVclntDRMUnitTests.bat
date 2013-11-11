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

del E:\Logs\TestResults\TSU_MMF_VCLNT_DRM*.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** VCLNT DRM TESTS**************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\vclnt\

copy z:\TSU_MMF_VCLNT_01.ini c:\mm\TSU_MMF_VCLNT_01.ini
copy E:\unittest\mmf\vclnt\mm\clock.dummy c:\mm\mmf\testfiles\vclnt\clock.dummy
copy E:\unittest\mmf\vclnt\mm\LongVideo.dummy c:\mm\mmf\testfiles\vclnt\LongVideo.dummy
copy E:\unittest\mmf\vclnt\mm\ShortVideo.dummy c:\mm\mmf\testfiles\vclnt\ShortVideo.dummy
e:\rta\makeContent.bat

testframework TSU_MMF_VCLNT_DRM.script
testframework TSU_MMF_VCLNT_DRM_ALLOC.script

move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\vclnt\*.*
