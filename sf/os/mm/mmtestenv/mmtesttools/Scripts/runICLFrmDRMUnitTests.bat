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

del E:\Logs\TestResults\TSU_ICL_FRM_DRM*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM ***************** FRM DRM TESTS****************
REM ***********************************************

md c:\TSU_ICL_FRM_01\

copy E:\unittest\icl\frm\jfif444.jpg c:\TSU_ICL_FRM_01\jfif444.jpg
copy E:\unittest\icl\frm\jfif444.mbm c:\TSU_ICL_FRM_01\jfif444.mbm

copy E:\unittest\icl\frm\trans_8.gif c:\TSU_ICL_FRM_01\trans_8.gif
copy E:\unittest\icl\frm\trans_8.mbm c:\TSU_ICL_FRM_01\trans_8.mbm

copy E:\unittest\icl\frm\png4_8.png c:\TSU_ICL_FRM_01\png4_8.png
copy E:\unittest\icl\frm\png4_8.mbm c:\TSU_ICL_FRM_01\png4_8.mbm

REM  *** for making DRM content ***
md c:\ICL\
md c:\ICL\TestImages\
copy E:\unittest\icl\frm\jfif444.jpg c:\ICL\TestImages\jfif444.jpg
copy E:\unittest\icl\frm\trans_8.gif c:\ICL\TestImages\trans_8.gif
copy E:\unittest\icl\frm\png4_8.png c:\ICL\TestImages\png4_8.png
copy E:\unittest\icl\frm\simple_b.mng c:\ICL\TestImages\simple_b.mng
copy E:\unittest\icl\frm\jfif422_4_ref.JPG c:\ICL\TestImages\jfif422_4_ref.JPG
copy E:\unittest\icl\frm\clock.gif c:\ICL\TestImages\clock.gif

e:\rta\makeContent.bat

copy c:\ICL\TestImages\ImageContentNoRights.content c:\TSU_ICL_FRM_01\ImageContentNoRights.content
copy c:\ICL\TestImages\Image.content c:\TSU_ICL_FRM_01\Image.content
copy c:\ICL\TestImages\SingleImage.content c:\TSU_ICL_FRM_01\SingleImage.content

testframework -q TSU_ICL_FRM_DRM_DEBUG.script

move C:\Logs\TestResults\*.htm 	E:\Logs\TestResults\

REM*****Always run this at the end************
REM*****************LONG!!!!******************
testframework -q TSU_ICL_FRM_DRM_ALLOC.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\TSU_ICL_FRM_01\*.*
del c:\ICL\TestImages\*.*
