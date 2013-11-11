@echo off

rem Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem Description:
rem This batch file should be used only when installed on
rem \epoc32\tools directory in order to convert one
rem or more CFG files to XML using the XML Schema files
rem in \epoc32\tools\commdb-schema\Xschema directory
rem 
rem

echo
set version=\epoc32\tools\commdb-schema\%2%schema\
rem edit above line of batch file if schema files located in different directory or just run the jar file directly using java.
rem for  'version=\epoc32\tools\commdb-schema\93schema\' for 9.3
echo cfg2xml takes 4 parameters:
echo mode, either file or dir
echo version, 92 or 93 etc
echo input cfg file
echo output xml file
echo current version is 1.1.1
java -jar \epoc32\tools\cfg2xml.jar %1 1.1.1 %3 %4
