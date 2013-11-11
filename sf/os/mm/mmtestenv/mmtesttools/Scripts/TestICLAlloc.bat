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
rem ********* COD_UNIT_01 ALLOC TEST *************
rem 
rem

del E:\Logs\TestResults\tsu_icl_cod_01_alloc.*
md E:\Logs
md E:\Logs\TestResults

md c:\TImage\
md c:\TImage\anon\
md c:\Timage\prof\
md c:\TImage\ref\
md c:\TImage\00d4\
md c:\TImage\00d6\

md c:\TSU_ICL_COD_02\
md c:\TSU_ICL_COD_03\
md c:\TSU_ICL_COD_03\WMF\
md c:\TSU_ICL_COD_03\WMFRef\
md c:\TSU_ICL_COD_03\WMFRefMask\
md c:\TSU_ICL_COD_03\WMFArmRef\
md c:\TSU_ICL_COD_03\WMFArmRefMask\
md c:\TSU_ICL_COD_04\
md c:\ICL\
md c:\ICL\Validation\
md c:\ICL\Validation\png\

copy E:\unittest\icl\TImage\*.* c:\TImage\
copy E:\unittest\icl\TImage\Anon\*.* c:\TImage\Anon\
copy E:\unittest\icl\TImage\prof\*.* c:\TImage\prof\
copy E:\unittest\icl\TImage\ref\*.* c:\TImage\ref\
copy E:\unittest\icl\TImage\00d4\*.* c:\TImage\00d4\
copy E:\unittest\icl\TImage\00d6\*.* c:\TImage\00d6\

copy E:\unittest\icl\TSU_ICL_COD_02\*.* c:\TSU_ICL_COD_02\
copy E:\unittest\ICL\Validation\png\*.* c:\ICL\Validation\png\

copy E:\unittest\icl\TSU_ICL_COD_03\*.* c:\TSU_ICL_COD_03\
copy E:\unittest\icl\TSU_ICL_COD_03\WMF\*.* c:\TSU_ICL_COD_03\WMF\
copy E:\unittest\icl\TSU_ICL_COD_03\WMFRef\*.* c:\TSU_ICL_COD_03\WMFRef\
copy E:\unittest\icl\TSU_ICL_COD_03\WMFRefMask\*.* c:\TSU_ICL_COD_03\WMFRefMask\
copy E:\unittest\icl\TSU_ICL_COD_03\WMFArmRef\*.* c:\TSU_ICL_COD_03\WMFArmRef\
copy E:\unittest\icl\TSU_ICL_COD_03\WMFArmRefMask\*.* c:\TSU_ICL_COD_03\WMFArmRefMask\

copy E:\unittest\icl\TSU_ICL_COD_04\*.* c:\TSU_ICL_COD_04\

testframework -q tsu_icl_cod_01_alloc.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TImage\*.*
del c:\TImage\anon\*.*
del c:\Timage\prof\*.*
del c:\TImage\ref\*.*
del c:\TImage\00d4\*.*
del c:\TImage\00d6\*.*

del c:\TSU_ICL_COD_02\*.*
del c:\TSU_ICL_COD_03\*.*
del c:\TSU_ICL_COD_03\WMF\*.*
del c:\TSU_ICL_COD_03\WMFRef\*.*
del c:\TSU_ICL_COD_03\WMFRefMask\*.*
del c:\TSU_ICL_COD_03\WMFArmRef\*.*
del c:\TSU_ICL_COD_03\WMFArmRefMask\*.*
del c:\TSU_ICL_COD_04\*.*
del c:\ICL\*.*
del c:\ICL\Validation\*.*
del c:\ICL\Validation\png\*.*


REM ***********************************************
REM ********* FRM_UNIT_01 and ALLOC TESTS *************
REM ***********************************************
z:
runICLFrmUnitTests.bat


REM ***********************************************
REM ********* FRM_DRM_UNIT and ALLOC TESTS *************
REM ***********************************************
z:
runICLFrmDRMUnitTests.bat

