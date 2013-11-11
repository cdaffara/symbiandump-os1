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
pushd ..\..\..\stubs\group\
REM call bldmake bldfiles
REM call abld test reallyclean
call sbs -c armv5.test REALLYCLEAN
call sbs -c winscw.test REALLYCLEAN
popd
@echo Cleaning stubs... Finished


@echo build BT Audioman Client...
pushd \sf\mw\btservices\bluetoothengine\btaudioman\group
REM call bldmake bldfiles
REM call abld build
call sbs
popd
@echo build BT Audioman Client... Finished


@echo build accfwuinotifier...
pushd \sf\mw\appsupport\coreapplicationuis\accfwuinotifier\group
REM call bldmake bldfiles
REM call abld build
call sbs
popd
@echo build accfwuinotifier... Finished


@echo build accessoryadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\accessoryadaptation\group\
REM call bldmake bldfiles
REM call abld cleanexport
call sbs -c armv5 CLEANEXPORT
call sbs -c winscw CLEANEXPORT
REM call abld export
call sbs -c armv5 EXPORT
call sbs -c winscw EXPORT
REM call abld build accessoryadaptation
call sbs -p accessoryadaptation.mmp
popd
@echo build accessoryadaptation... Finished


@echo build Plugged Display ASY...
pushd \sf\os\devicesrv\accessoryservices\pluggeddisplay\pluggeddisplayasy\group
REM call bldmake bldfiles
REM call abld build
call sbs
popd
@echo build Plugged Display ASY... Finished