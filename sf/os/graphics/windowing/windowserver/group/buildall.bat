REM buildall.bat
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
@echo off
if "%1"=="rclean" goto rclean
if "%1"=="clean" goto clean
if "%1"=="test" goto min
if "%1"=="t" goto thumb
call bldmake bldfiles
echo Building Window Server
call abld build > h:\wserv\group\BUILD.LOG 2>&1
if "%1"=="" goto :min
echo Building Test Code
call abld test build -v >> \wserv\group\BUILD.LOG 2>&1
goto end
:min
echo Building Test Code for Wins
call abld test build wins >> \wserv\group\BUILD.LOG 2>&1
:thumb
echo Building Test Code for ARM4
call abld test build -v arm4 >> \wserv\group\BUILD.LOG 2>&1
echo Building Test Code for Thumb
call abld test build -v thumb >> \wserv\group\BUILD.LOG 2>&1
goto end
:clean
echo Doing a Clean
call abld clean
call abld test clean
:rclean
echo Doing a Clean (reallyclean)
call abld reallyclean
call abld test reallyclean
:end
