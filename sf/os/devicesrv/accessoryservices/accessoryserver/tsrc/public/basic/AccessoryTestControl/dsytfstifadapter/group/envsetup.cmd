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
REM call bldmake bldfiles
REM call abld makefile btaudiomanclient
REM call abld reallyclean btaudiomanclient
call sbs -p btaudiomanclient.mmp -c armv5 CLEAN
call sbs -p btaudiomanclient.mmp -c winscw CLEAN
popd
@echo Cleaning BT Audioman Client... Finished


@echo Cleaning accfwuinotifier...
pushd \sf\mw\appsupport\coreapplicationuis\accfwuinotifier\group
REM call bldmake bldfiles
REM call abld makefile 
REM call abld reallyclean
call sbs -c armv5 CLEAN
call sbs -c winscw CLEAN
popd
@echo Cleaning accfwuinotifier... Finished


@echo Cleaning accessoryadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\accessoryadaptation\group\
REM call bldmake bldfiles
REM call abld makefile accessoryadaptation
REM call abld clean accessoryadaptation
call sbs -p accessoryadaptation.mmp -c armv5 CLEAN
call sbs -p accessoryadaptation.mmp -c winscw CLEAN
popd
@echo Cleaning accessoryadaptation... Finished


@echo Cleaning Plugged Display ASY...
pushd \sf\os\devicesrv\accessoryservices\pluggeddisplay\pluggeddisplayasy\group
REM call bldmake bldfiles
REM call abld reallyclean
call sbs -c armv5 CLEAN
call sbs -c winscw CLEAN
popd
@echo Cleaning Plugged Display ASY... Finished


@echo Setting up stubs...
pushd ..\..\..\stubs\group\
REM call bldmake bldfiles
REM call abld test reallyclean
call sbs -c armv5.test REALLYCLEAN
call sbs -c winscw.test REALLYCLEAN
REM call abld test build
call sbs -c armv5.test
call sbs -c winscw.test
popd
@echo Setting up stubs... Finished
