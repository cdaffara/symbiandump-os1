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


@echo Cleaning stubs
pushd ..\..\..\stub\group\
REM call bldmake bldfiles
REM call abld test reallyclean
call sbs -c armv5.test REALLYCLEAN
call sbs -c winscw.test REALLYCLEAN
popd
@echo Cleaning stubs done

@echo build remconadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\accessoryadaptation\group\
REM call bldmake bldfiles
REM call abld build remconadaptation
if not "%1"=="" goto default
call sbs -p remconadaptation.mmp -c armv5.%1
:default
call sbs -p remconadaptation.mmp -c armv5
call sbs -p remconadaptation.mmp -c winscw
REM Add other variants if applicable
popd
@echo build remconadaptation... Finished

@echo build phoneclient...
pushd \sf\mw\phonesrv\phoneclientserver\phoneclient\group
REM call bldmake bldfiles
REM call abld build
call sbs
popd
@echo build phoneclient... Finished

@echo build phonecmdhandler
pushd \sf\mw\phonesrv\phonecmdhandler\group\
REM call bldmake bldfiles
REM call abld makefile 
REM call abld reallyclean
call sbs -c armv5 REALLYCLEAN
call sbs -c winscw REALLYCLEAN
REM call abld build
call sbs
popd
@echo build phonecmdhandler done
