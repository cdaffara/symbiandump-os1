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

del E:\Logs\TestResults\tsu_icl_cod_03*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM **************** COD03 TESTS ******************
REM ***********************************************

md c:\TSU_ICL_COD_03\
md c:\TSU_ICL_COD_03\WMF\
md c:\TSU_ICL_COD_03\WMFRef\
md c:\TSU_ICL_COD_03\WMFRefMask\
md c:\TSU_ICL_COD_03\WMFArmRef\
md c:\TSU_ICL_COD_03\WMFArmRefMask\

copy E:\unittest\icl\TSU_ICL_COD_03\*.* c:\TSU_ICL_COD_03\
copy E:\unittest\icl\TSU_ICL_COD_03\WMF\*.* c:\TSU_ICL_COD_03\WMF\
copy E:\unittest\icl\TSU_ICL_COD_03\WMFRef\*.* c:\TSU_ICL_COD_03\WMFRef\
copy E:\unittest\icl\TSU_ICL_COD_03\WMFRefMask\*.* c:\TSU_ICL_COD_03\WMFRefMask\
copy E:\unittest\icl\TSU_ICL_COD_03\WMFArmRef\*.* c:\TSU_ICL_COD_03\WMFArmRef\
copy E:\unittest\icl\TSU_ICL_COD_03\WMFArmRefMask\*.* c:\TSU_ICL_COD_03\WMFArmRefMask\

testframework -q tsu_icl_cod_03.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_cod_03_alloc.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TSU_ICL_COD_03\WMF\*.*
del c:\TSU_ICL_COD_03\WMFRef\*.*
del c:\TSU_ICL_COD_03\WMFRefMask\*.*
del c:\TSU_ICL_COD_03\WMFArmRef\*.*
del c:\TSU_ICL_COD_03\WMFArmRefMask\*.*
del c:\TSU_ICL_COD_03\*.*
