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
rem This batch file builds the CommDB schema with Hidden / ReadOnly support.
rem The base schema is copied to the schema directory.
rem 
rem

@ECHO ON
SET SCHEMA_SRC=..\..\com\symbian\commdb\base_schema
SET SCHEMA_DST=..\..\com\symbian\commdb\schema

ECHO Y|del %SCHEMA_DST%\*.*
copy %SCHEMA_SRC%\*.* %SCHEMA_DST%\






 

