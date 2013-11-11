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


@attrib -r ..\..\..\epoc32\release\winscw\udeb\z\sys\data\estart_old.txt
@attrib -r ..\..\..\epoc32\release\winscw\udeb\z\sys\data\estart.txt
@copy /y ..\..\..\epoc32\release\winscw\udeb\z\sys\data\estart_old.txt ..\..\..\epoc32\release\winscw\udeb\z\sys\data\estart.txt
@rem
@attrib -r ..\..\..\epoc32\release\winscw\udeb\platformenv_old.dll
@attrib -r ..\..\..\epoc32\release\winscw\udeb\platformenv.dll
@copy /y ..\..\..\epoc32\release\winscw\udeb\platformenv_old.dll ..\..\..\epoc32\release\winscw\udeb\platformenv.dll