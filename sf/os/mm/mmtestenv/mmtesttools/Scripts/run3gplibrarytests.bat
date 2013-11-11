rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

REM ***********************************************
REM ****************** 3GP TESTS ******************
REM ***********************************************

REM NOTE: This BAT file is intended to be run in EShell
REM on HW only. On Emulator builds the test files already
REM reside in the correct directories on C: so you only
REM need to call testexecute with the desired script file.

REM =============================================================
REM Setup
REM =============================================================

REM Prepare memory card for test logs
del e:\logs\testexecute\tsu_3gp_*
md e:\logs\
md e:\logs\testexecute\

REM 3GP test scripts expect to find the INIs on c:
md c:\mm\
copy z:\mm\tsu_3gp*.ini c:\mm\

REM =============================================================
REM Run tests that use test data directly from memory card (e:)
REM =============================================================

REM Parsing tests
testexecute z:\mm\tsu_3gp_parse_format_3gp_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

testexecute z:\mm\tsu_3gp_parse_format_mp4_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

testexecute z:\mm\tsu_3gp_parse_format_valid_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

testexecute z:\mm\tsu_3gp_parse_and_check_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

REM Composing tests
testexecute z:\mm\tsu_3gp_compose_format_3g2_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

testexecute z:\mm\tsu_3gp_compose_format_3gp_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

testexecute z:\mm\tsu_3gp_compose_format_mp4_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

REM Other tests
testexecute z:\mm\tsu_3gp_performance_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

testexecute z:\mm\tsu_3gp_interop_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

REM =============================================================
REM Now run tests that need to copy data into c: first
REM =============================================================

REM Parsing tests
md c:\3gplibrary\
copy e:\3gplibrary\* c:\3gplibrary\

testexecute z:\mm\tsu_3gp_parse_api_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

testexecute z:\mm\tsu_3gp_parse_oom1_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

testexecute z:\mm\tsu_3gp_parse_oom2_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

del c:\3gplibrary\*

REM Composing tests
testexecute z:\mm\tsu_3gp_compose_api_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

testexecute z:\mm\tsu_3gp_compose_oom_te.script
move c:\logs\testexecute\tsu_3gp*.htm e:\logs\testexecute\

del c:\3gplibrary\temp\testing.mp4
rd c:\3gplibrary\temp\
rd c:\3gplibrary\

REM =============================================================
REM Clean up
REM =============================================================

REM delete INI files from C: (need to make non read-only first)
attrib c:\mm\tsu_3gp* -r
del c:\mm\tsu_3gp*

