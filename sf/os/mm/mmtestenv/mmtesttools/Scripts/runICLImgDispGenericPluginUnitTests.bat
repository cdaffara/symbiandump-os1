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

del E:\Logs\TestResults\TSU_ICL_GenIclImgDisp*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM ****** Generic ICL wrapper plugi-in TESTS ****
REM ***********************************************

md c:\tsu_icl_ExifImgdisp_01\
md c:\tsu_icl_ExifImgdisp_01\ref\
md c:\tsu_icl_ExifImgdisp_01\genIcl\
md c:\ICL\
md c:\ICL\RefImages\
md c:\ICL\TestImages\ 
md c:\TSU_ICL_COD_03\
md c:\TSU_ICL_COD_03\WMFArmRef\
md c:\TSU_ICL_COD_03\WMFArmRefMask\
md c:\TSU_ICL_COD_03\WMF\
md c:\TSU_ICL_COD_03\WMFREF\
md c:\TSU_ICL_COD_03\WMFREFMASK\

copy E:\tsu_icl_Exifimgdisp_01\*.* c:\tsu_icl_Exifimgdisp_01\*.*
copy E:\tsu_icl_Exifimgdisp_01\ref\*.* c:\tsu_icl_Exifimgdisp_01\ref\*.*
copy E:\tsu_icl_ExifImgdisp_01\genIcl\*.* c:\tsu_icl_ExifImgdisp_01\genIcl\*.*
copy E:\inttest\icl\RefImages\*.* c:\ICL\RefImages\*.*
copy E:\inttest\icl\TestImages\*.* c:\ICL\TestImages\*.*

copy e:\unittest\icl\TSU_ICL_COD_03\WMF\DibPatternBrush.wmf c:\TSU_ICL_COD_03\WMF\DibPatternBrush.wmf
copy e:\unittest\icl\TSU_ICL_COD_03\WMFArmRef\DibPatternBrush.mbm  c:\TSU_ICL_COD_03\WMFArmRef\DibPatternBrush.mbm
copy e:\unittest\icl\TSU_ICL_COD_03\WMFArmRefMask\DibPatternBrush.mbm  c:\TSU_ICL_COD_03\WMFArmRefMask\DibPatternBrush.mbm
REM For X86 Platform , Emulator version of mbm files are used.
REM Copying Emulator version of DibPatternBrush.mbm for X86
copy e:\unittest\icl\TSU_ICL_COD_03\WMFRef\DibPatternBrush.mbm  c:\TSU_ICL_COD_03\WMFRef\DibPatternBrush.mbm
copy e:\unittest\icl\TSU_ICL_COD_03\WMFRefMask\DibPatternBrush.mbm  c:\TSU_ICL_COD_03\WMFRefMask\DibPatternBrush.mbm

testframework -q TSU_ICL_GenIclImgDisp_01.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q TSU_ICL_GenIclImgDisp_01_Alloc.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\tsu_icl_ExifImgdisp_01\ref\*.*
del c:\tsu_icl_ExifImgdisp_01\genIcl\*.*
del c:\tsu_icl_ExifImgdisp_01\*.*
del c:\TSU_ICL_COD_03\WMFArmRef\*.*
del c:\TSU_ICL_COD_03\WMFArmRefMask\*.*
del c:\TSU_ICL_COD_03\WMFRef\*.*
del c:\TSU_ICL_COD_03\WMFRefMask\*.*
del c:\TSU_ICL_COD_03\WMF\*.*
del c:\TSU_ICL_COD_03\*.*
del c:\ICL\RefImages\*.*
del c:\ICL\TestImages\*.*
del c:\ICL\*.*
