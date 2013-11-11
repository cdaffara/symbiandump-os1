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


@echo Start copying testframework.ini...
copy TestFramework.ini \epoc32\data\Z\system\data
@echo testframework.ini done

@echo Start copying testframework.ini...
copy CommonDSY.ini \epoc32\release\armv5\urel\z\private\101f6efa\CommonDSY.ini
copy CommonDSY.ini \epoc32\release\armv5\udeb\z\private\101f6efa\CommonDSY.ini
@echo testframework.ini done
