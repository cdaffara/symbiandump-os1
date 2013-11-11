rem Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del E:\Logs\TestResults\tsu_icl_cod_02*.*

REM ***********************************************
REM **************** COD02 TESTS ******************
REM ***********************************************

md c:\TSU_ICL_COD_02\
md c:\ICL\
md c:\ICL\Validation\
md c:\ICL\Validation\png\

copy E:\unittest\icl\TSU_ICL_COD_02\*.* c:\TSU_ICL_COD_02\
copy E:\unittest\ICL\Validation\png\*.* c:\ICL\Validation\png\

testframework -q tsu_icl_cod_02.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframeworkrecognizer -q tsu_icl_cod_02_Recog.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframeworkrecognizer -q tsu_icl_cod_02_recog_alloc.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\ICL\Validation\png\*.*
del c:\TSU_ICL_COD_02\*.*