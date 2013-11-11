@ECHO OFF
rem Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem This batch file builds the CommDB schema with no Hidden / ReadOnly support.
rem 
rem

REM The base schema is transformed using XSLT to remove elements as required.


@ECHO ON
SET SCHEMA_SRC=..\..\com\symbian\commdb\base_schema
SET SCHEMA_DST=..\..\com\symbian\commdb\schema
SET XSL_DIR=..\data\xsl

ECHO Y|del %SCHEMA_DST%\*.* 

copy %SCHEMA_SRC%\*.* %SCHEMA_DST%\
ECHO Y|del %SCHEMA_DST%\AccessTypeTableTable.xsd 

REM Remove Hidden / ReadOnly fields from all the records
FOR %%t	IN (%SCHEMA_DST%\*Table.xsd) DO MSXSL -o %%t %%t %XSL_DIR%\hidden_read_only.xsl

REM Remove reference to AccessTypeTable
MSXSL -v -o %SCHEMA_DST%\CommDb.xsd       %SCHEMA_DST%\CommDb.xsd       %XSL_DIR%\protection_table.xsl
MSXSL -v -o %SCHEMA_DST%\CommTypes.xsd    %SCHEMA_DST%\CommTypes.xsd    %XSL_DIR%\protection_table.xsl
MSXSL -v -o %SCHEMA_DST%\Contents.xml     %SCHEMA_DST%\Contents.xml     %XSL_DIR%\protection_table.xsl









 

