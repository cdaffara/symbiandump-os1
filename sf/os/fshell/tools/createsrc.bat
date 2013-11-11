@REM createsrc.bat
@REM 
@REM Copyright (c) 2010 Accenture. All rights reserved.
@REM This component and the accompanying materials are made available
@REM under the terms of the "Eclipse Public License v1.0"
@REM which accompanies this distribution, and is available
@REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
@REM 
@REM Initial Contributors:
@REM Accenture - Initial contribution
@REM
@echo off
set script=%0%
set script=%script:.bat=%
perl -Swx "%script%" %*

