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
rem runICLFrmUnitTests.bat
rem 
rem

del E:\Logs\TestResults\TSU_ICL_FRM_FH*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM ***************** FRM TESTS *******************
REM ***********************************************

md c:\TSU_ICL_FRM_01\

copy E:\unittest\icl\frm\Tif2.tif c:\TSU_ICL_FRM_01\Tif2.tif
copy E:\unittest\icl\frm\Tif2.mbm c:\TSU_ICL_FRM_01\Tif2.mbm
copy E:\unittest\icl\frm\Trans_1.gif c:\TSU_ICL_FRM_01\Trans_1.gif
copy E:\unittest\icl\frm\Trans_1.mbm c:\TSU_ICL_FRM_01\Trans_1.mbm
copy E:\unittest\icl\frm\Gif87a.gif c:\TSU_ICL_FRM_01\Gif87a.gif
copy E:\unittest\icl\frm\Gif87a.mbm c:\TSU_ICL_FRM_01\Gif87a.mbm
copy E:\unittest\icl\frm\Test.tst c:\TSU_ICL_FRM_01\Test.tst
copy E:\unittest\icl\frm\Png24bpp_1.png c:\TSU_ICL_FRM_01\Png24bpp_1.png

testframework -q TSU_ICL_FRM_FH.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q TSU_ICL_FRM_FH_ALLOC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TSU_ICL_FRM_01\*.*
