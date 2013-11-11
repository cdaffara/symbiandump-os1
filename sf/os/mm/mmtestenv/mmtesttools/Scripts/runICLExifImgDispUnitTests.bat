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

del E:\Logs\TestResults\TSU_ICL_ExifImgDisp*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM ************* ImageDisplay TESTS **************
REM ***********************************************

md c:\tsu_icl_ExifImgdisp_01\
md c:\tsu_icl_ExifImgdisp_01\ref\

copy E:\tsu_icl_Exifimgdisp_01\*.* c:\tsu_icl_Exifimgdisp_01\*.*
copy E:\tsu_icl_Exifimgdisp_01\ref\*.* c:\tsu_icl_Exifimgdisp_01\ref\*.*

testframework -q tsu_icl_Exifimgdisp_01.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_Exifimgdisp_01_ALLOC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_Exifimgdisp_01_panic.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\tsu_icl_ExifImgdisp_01\ref\*.*
del c:\tsu_icl_ExifImgdisp_01\*.*