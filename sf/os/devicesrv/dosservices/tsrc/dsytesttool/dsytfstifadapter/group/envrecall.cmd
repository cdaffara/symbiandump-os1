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


@echo Cleaning stubs...
pushd ..\..\..\public\basic\stubs\group\
call bldmake bldfiles
call abld test reallyclean
popd
@echo Cleaning stubs... Finished


@echo build BT Audioman Client...
pushd \sf\mw\btservices\bluetoothengine\btaudioman\group
call bldmake bldfiles
call abld build
popd
@echo build BT Audioman Client... Finished


@echo build accfwuinotifier...
pushd \sf\mw\appsupport\coreapplicationuis\accfwuinotifier\group
call bldmake bldfiles
call abld build
popd
@echo build accfwuinotifier... Finished


@echo build accessoryadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\accessoryadaptation\group\
call bldmake bldfiles
call abld cleanexport
call abld export
call abld build accessoryadaptation
popd
@echo build accessoryadaptation... Finished


@echo build Plugged Display ASY...
pushd \sf\os\devicesrv\accessoryservices\pluggeddisplay\pluggeddisplayasy\group
call bldmake bldfiles
call abld build
popd
@echo build Plugged Display ASY... Finished