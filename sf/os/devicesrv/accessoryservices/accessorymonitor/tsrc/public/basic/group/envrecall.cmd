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
pushd ..\stubs\group\
REM call bldmake bldfiles
REM call abld test reallyclean
call sbs -c armv5.test REALLYCLEAN
call sbs -c winscw.test REALLYCLEAN
popd
@echo Cleaning stubs... Finished

@echo build accfwuinotifier...
pushd \sf\mw\appsupport\coreapplicationuis\accfwuinotifier\group\
REM call bldmake bldfiles
REM call abld build
call sbs
popd
@echo build accfwuinotifier... Finished

@echo build accessoryadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\group\
REM call bldmake bldfiles
REM call abld build accessoryadaptation
call sbs -p accessoryadaptation
popd
@echo build accessoryadaptation... Finished