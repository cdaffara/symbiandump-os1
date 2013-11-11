REM Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

@echo off
echo Replacing epoc.ini with rawip version
copy epoc.ini epoc_backup.ini
copy z\testdata\configs\epocrawip.ini %EPOC_INI%