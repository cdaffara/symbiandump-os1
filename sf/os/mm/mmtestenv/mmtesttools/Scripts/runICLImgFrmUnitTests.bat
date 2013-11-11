rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del E:\Logs\TestResults\TSU_ICL_IMAGEFRAME*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM ***************** IMAGEe FRAME TESTS *******************
REM ***********************************************

testframework -q TSU_ICL_IMAGEFRAME.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q TSU_ICL_IMAGEFRAME_ALLOC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\


