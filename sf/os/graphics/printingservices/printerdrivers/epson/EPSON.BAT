REM EPSON.BAT
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
del epson.pdr

set _par1=%1
if "%1"=="" set _par1=ENGLISH
if "%1"=="english" set _par1=ENGLISH
if "%1"=="german" set _par1=GERMAN
if "%1"=="french" set _par1=FRENCH
if "%1"=="dutch" set _par1=DUTCH
if "%1"=="italian" set _par1=ITALIAN
if "%1"=="spanish" set _par1=SPANISH

call cl /P /EP /D "_%_par1%" epson.pd 

pdrtran epson.i epson.pdr

rem del *.i

copy epson.pdr \epoc32\release\wins\urel\z\system\printers\epson.pdr
copy epson.pdr \epoc32\release\wins\udeb\z\system\printers\epson.pdr
copy epson.pdr \epoc32\release\marm\urel\epson.pdr
copy epson.pdr \epoc32\release\marm\udeb\epson.pdr

