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

del E:\Logs\TestResults\TSU_MMF_SWCodecDevices.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM *************** SW CODEC DEVICES **************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\SWCodecDevices

copy e:\unittest\mmf\SWCodecDevices\*.* c:\mm\mmf\testfiles\SWCodecDevices\
copy z:\TSU_MMF_SwCodecDevices.ini c:\mm\TSU_MMF_SwCodecDevices.ini
attrib c:\mm\mmf\testfiles\SWCodecDevices\*.* -r

testframework tsu_mmf_SWCodecDevices.script

move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\SWCodecDevices\*.*
del c:\mm\TSU_MMF_SwCodecDevices.ini
