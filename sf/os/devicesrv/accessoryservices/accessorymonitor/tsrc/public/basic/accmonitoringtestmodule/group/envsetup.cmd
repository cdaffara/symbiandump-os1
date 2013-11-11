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


@echo Cleaning accfwuinotifier...
pushd \sf\mw\appsupport\coreapplicationuis\accfwuinotifier\group\
REM call bldmake bldfiles
REM call abld reallyclean
call sbs -c armv5 REALLYCLEAN
call sbs -c winscw REALLYCLEAN
popd
@echo Cleaning accfwuinotifier... Finished

@echo Cleaning accessoryadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\group\
REM call bldmake bldfiles
REM call abld clean armv5 accessoryadaptation
call sbs -p accessoryadaptation -c armv5 CLEAN
call sbs -p accessoryadaptation -c winscw CLEAN
popd
@echo Cleaning accessoryadaptation... Finished

@echo Setting up stubs...
pushd ..\..\stubs\group\
REM call bldmake bldfiles
REM call abld test reallyclean
call sbs -c armv5.test REALLYCLEAN
call sbs -c winscw.test REALLYCLEAN
REM call abld test build
call sbs -c armv5.test
call sbs -c winscw.test
popd
@echo Setting up stubs... Finished
