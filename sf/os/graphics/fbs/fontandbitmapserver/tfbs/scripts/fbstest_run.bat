REM Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
REM All rights reserved.
REM This component and the accompanying materials are made available
REM under the terms of "Eclipse Public License v1.0"
REM which accompanies this distribution, and is available
REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
REM 
REM Initial Contributors:
REM Nokia Corporation - initial contribution.
REM 
REM Contributors:
REM 
REM Description:
@echo on
testexecute.exe  z:\fbstest\fbstest_t_alloc.script
testexecute.exe  z:\fbstest\fbstest_t_bitmap.script
testexecute.exe  z:\fbstest\fbstest_t_fbs.script
testexecute.exe  z:\fbstest\fbstest_t_compressed.script
testexecute.exe  z:\fbstest\fbstest_t_getallbitmapscapability.script
testexecute.exe  z:\fbstest\fbstest_t_ralc.script
testexecute.exe  z:\fbstest\fbstest_t_defect.script
testexecute.exe  z:\fbstest\fbstest_t_streamidcache.script
testexecute.exe  z:\fbstest\fbstest_t_secure.script
testexecute.exe  z:\fbstest\fbstest_t_file.script
testexecute.exe  z:\fbstest\fbstest_t_clean.script
testexecute.exe  z:\fbstest\fbstest_t_ipctest.script
testexecute.exe  z:\fbstest\fbstest_t_extendedbitmap.script
testexecute.exe  z:\fbstest\fbstest_t_fonttable.script
testexecute.exe  z:\fbstest\fbstest_t_glyphdata.script		
testexecute.exe  z:\fbstest\fbstest_t_glyphdataoutlineshadow.script	
testexecute.exe  z:\fbstest\fbstest_t_oogm.script

@echo off
md e:\logs
md e:\logs\testexecute

copy c:\logs\testexecute\fbstest_t_*.htm e:\logs\testexecute\
