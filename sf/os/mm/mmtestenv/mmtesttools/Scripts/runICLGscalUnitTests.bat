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

del E:\Logs\TestResults\TSU_ICL_GSCAL*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM **************** GSCAL TESTS *****************
REM ***********************************************

md c:\TSU_ICL_GSCAL_01\

copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2.png c:\TSU_ICL_GSCAL_01\png2_2.png
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2t.mbm c:\TSU_ICL_GSCAL_01\png2_2t.mbm
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2m.mbm c:\TSU_ICL_GSCAL_01\png2_2m.mbm
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2_8pThmb.mbm c:\TSU_ICL_GSCAL_01\png2_2_8pThmb.mbm
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2_8pmThmb.mbm c:\TSU_ICL_GSCAL_01\png2_2_8pmThmb.mbm
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2_8pm.mbm c:\TSU_ICL_GSCAL_01\png2_2_8pm.mbm
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2_8p.mbm c:\TSU_ICL_GSCAL_01\png2_2_8p.mbm
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2_8.mbm c:\TSU_ICL_GSCAL_01\png2_2_8.mbm
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2.mbm c:\TSU_ICL_GSCAL_01\png2_2.mbm
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2_8pSmall.mbm c:\TSU_ICL_GSCAL_01\png2_2_8pSmall.mbm
copy E:\unittest\icl\TSU_ICL_GSCAL_01\png2_2_8pmThrsh.mbm c:\TSU_ICL_GSCAL_01\png2_2_8pmThrsh.mbm

testframework -q tsu_icl_gscal_01.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_gscal_01_ALLOC.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

testframework -q tsu_icl_gscal_01_Panic.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TSU_ICL_GSCAL_01\*.*