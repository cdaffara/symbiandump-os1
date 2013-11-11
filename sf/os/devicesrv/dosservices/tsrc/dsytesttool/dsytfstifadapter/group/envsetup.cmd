@echo off
REM Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
REM Description:  ?Description
REM
REM
@echo on


@echo Cleaning BT Audioman Client...
pushd \sf\mw\btservices\bluetoothengine\btaudioman\group
call bldmake bldfiles
call abld makefile btaudiomanclient
call abld reallyclean btaudiomanclient
popd
@echo Cleaning BT Audioman Client... Finished


@echo Cleaning accfwuinotifier...
pushd \sf\mw\appsupport\coreapplicationuis\accfwuinotifier\group
call bldmake bldfiles
call abld makefile 
call abld reallyclean
popd
@echo Cleaning accfwuinotifier... Finished


@echo Cleaning accessoryadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\accessoryadaptation\group\
call bldmake bldfiles
call abld makefile accessoryadaptation
call abld clean accessoryadaptation
popd
@echo Cleaning accessoryadaptation... Finished


@echo Cleaning Plugged Display ASY...
pushd \sf\os\devicesrv\accessoryservices\pluggeddisplay\pluggeddisplayasy\group
call bldmake bldfiles
call abld reallyclean
popd
@echo Cleaning Plugged Display ASY... Finished


@echo Setting up stubs...
pushd ..\..\..\public\basic\stubs\group\
call bldmake bldfiles
call abld test reallyclean
call abld test build
popd
@echo Setting up stubs... Finished
