rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem

del E:\logs\testexecute\*
del C:\logs\testexecute\*

md E:\Logs\
md E:\Logs\testexecute\

REM ***********************************************
REM ****************** 3GP TESTS ******************
REM ***********************************************

REM Copy Test Scripts
md c:\mm\
copy E:\mm\* c:\mm\

md C:\3gplibrary\
copy E:\3gplibrary\* c:\3gplibrary\

testexecute Z:\mm\tsu_3gp_parse_large_file.script
move C:\logs\testexecute\tsu_3gp_parse_large_file* E:\Logs\testexecute\

del e:\3gplibrary\format\valid\large\*

testexecute Z:\mm\tsu_3gp_compose_large_file.script
move C:\logs\testexecute\tsu_3gp_compose_large_file* E:\Logs\testexecute\

rem testexecute Z:\mm\tsu_3gp_compose_format_large_te.script
rem move C:\logs\testexecute\tsu_3gp_compose_format_large* E:\Logs\testexecute\

del C:\3gplibrary\*

del C:\mm\*

del e:\3gplibrary\temp\*
