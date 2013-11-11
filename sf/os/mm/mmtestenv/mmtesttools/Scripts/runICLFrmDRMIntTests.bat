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

del E:\Logs\TestResults\TSI_ICL_FRM_DRM.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** FRM DRM TESTS****************
REM ***********************************************

md c:\ICL\
md c:\ICL\TestImages\
md c:\ICL\RefImages\

copy z:\ICL\TSI_ICL_FRM_00.ini c:\ICL\TSI_ICL_FRM_00.ini

copy E:\inttest\icl\testimages\jfif444.jpg c:\ICL\TestImages\jfif444.jpg
copy E:\inttest\icl\refimages\jfif444.mbm c:\ICL\RefImages\jfif444.mbm

copy E:\inttest\icl\testimages\trans_8.gif c:\ICL\TestImages\trans_8.gif
copy E:\inttest\icl\refimages\trans_8.mbm c:\ICL\RefImages\trans_8.mbm

copy E:\inttest\icl\testimages\png4_8.png c:\ICL\TestImages\png4_8.png
copy E:\inttest\icl\refimages\png4_8.mbm c:\ICL\RefImages\png4_8.mbm

copy E:\inttest\icl\testimages\simple_b.mng c:\ICL\TestImages\simple_b.mng

copy E:\inttest\icl\testimages\jfif422_4_ref.JPG c:\ICL\TestImages\jfif422_4_ref.JPG

copy E:\inttest\icl\testimages\clock.gif c:\ICL\TestImages\clock.gif

e:\rta\makeContent.bat

testframework -q TSI_ICL_FRM_DRM.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\icl\testimages\*.*
del c:\icl\refimages\*.*
del c:\icl\*.*