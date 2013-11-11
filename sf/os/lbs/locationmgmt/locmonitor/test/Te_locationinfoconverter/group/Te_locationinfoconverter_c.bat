rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:      batch file
rem

rem copy C:\TestFramework\TestFramework_locblid.ini C:\TestFramework\TestFramework.ini
rem md e:\BCTest
rem md e:\BCTest\Results

ATSINTERFACE.EXE -testmodule testcombiner -config C:\TestFramework\Te_locationinfoconverter.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\Te_locationinfoconverter.txt

