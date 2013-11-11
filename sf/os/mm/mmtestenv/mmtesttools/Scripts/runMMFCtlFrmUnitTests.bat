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

del E:\Logs\TestResults\tsu_mmf_ctlfrm.htm
del E:\Logs\TestResults\tsu_mmf_ctlfrm_sdrm.htm
del E:\Logs\TestResults\tsu_mmf_ctlfrm_sdrm_alloc.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** CTLFRM TESTS *****************
REM ***********************************************

testframework tsu_mmf_ctlfrm.script
testframeworkmmddcap tsu_mmf_ctlfrm_sdrm.script
testframeworkmmddcap tsu_mmf_ctlfrm_sdrm_alloc.script

move c:\Logs\TestResults\*.htm e:\Logs\TestResults\