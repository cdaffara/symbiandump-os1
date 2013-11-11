REM WBld.BAT
REM Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
REM 
@setlocal
@set plat=thumb
@set build=urel
@rem set op=build -v
@set op=build -v
call abld %op% %plat% %build%
call abld test %op% %plat% %build% tlib
call abld test %op% %plat% %build% shell
call abld test %op% %plat% %build% auto
call abld test %op% %plat% %build% autodll
call abld test %op% %plat% %build% tman
call abld test %op% %plat% %build% deblog
call abld test %op% %plat% %build% deblogfl
call abld test %op% %plat% %build% deblogsr
call abld test %op% %plat% %build% connect
call abld test %op% %plat% %build% animdll
call abld test %op% %plat% %build% econs
@endlocal
