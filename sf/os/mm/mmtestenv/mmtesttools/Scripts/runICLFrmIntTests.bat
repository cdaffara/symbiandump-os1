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

del E:\Logs\TestResults\TSI_ICL_FRM_00.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** FRM TESTS *******************
REM ***********************************************

md c:\ICL\
md c:\ICL\TestImages\
md c:\ICL\RefImages\
REM The following directory is required for DRM tests
md c:\system\data\

copy z:\ICL\TSI_ICL_FRM_00.ini c:\ICL\TSI_ICL_FRM_00.ini

copy E:\inttest\icl\testimages\*.* c:\ICL\TestImages\
copy E:\inttest\icl\refimages\*.* c:\ICL\RefImages\

testframework -q TSI_ICL_FRM_00.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\icl\testimages\*.*
del c:\icl\refimages\*.*
del c:\icl\*.*
