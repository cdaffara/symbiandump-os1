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


@echo Cleaning remconadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\accessoryadaptation\group\
REM call bldmake bldfiles
REM call abld makefile remconadaptation
REM call abld reallyclean remconadaptation
if not "%1"=="" goto default
call sbs -p remconadaptation.mmp -c armv5.%1 CLEAN
:default
call sbs -p remconadaptation.mmp -c armv5 CLEAN
call sbs -p remconadaptation.mmp -c winscw CLEAN
REM Add other variants if applicable
popd
@echo Cleaning remconadaptation... Finished

@echo build/instr stubs
pushd ..\..\..\stub\group\
REM call bldmake bldfiles
REM call abld test reallyclean
call sbs -c armv5.test REALLYCLEAN
call sbs -c winscw.test REALLYCLEAN
REM call abld test build
call sbs -c armv5.test
call sbs -c winscw.test
popd
@echo build/instr stubs done
