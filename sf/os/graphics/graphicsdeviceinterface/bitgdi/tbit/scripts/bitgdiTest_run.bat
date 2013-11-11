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

@echo on

testexecute.exe z:\bitgditest\bitgditest_t_accelerator.script
testexecute.exe z:\bitgditest\bitgditest_t_alphablending.script
testexecute.exe z:\bitgditest\bitgditest_t_auto.script
testexecute.exe z:\bitgditest\bitgditest_t_clip.script
testexecute.exe z:\bitgditest\bitgditest_t_clip2.script
testexecute.exe z:\bitgditest\bitgditest_t_defect.script
testexecute.exe z:\bitgditest\bitgditest_t_defect2.script
testexecute.exe z:\bitgditest\bitgditest_t_font.script
testexecute.exe z:\bitgditest\bitgditest_t_fontselect.script
testexecute.exe z:\bitgditest\bitgditest_t_gdi.script
testexecute.exe z:\bitgditest\bitgditest_t_param.script
testexecute.exe z:\bitgditest\bitgditest_t_bitblt.script
testexecute.exe z:\bitgditest\bitgditest_t_scaling.script
testexecute.exe z:\bitgditest\bitgditest_t_switch.script
testexecute.exe z:\bitgditest\bitgditest_t_extendedbitmap.script

@echo off
md e:\logs
md e:\logs\testexecute
copy c:\logs\testexecute\bitgdiTest_T_*.htm e:\logs\testexecute\
