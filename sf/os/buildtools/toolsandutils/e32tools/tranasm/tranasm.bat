:: Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
:: All rights reserved.
:: This component and the accompanying materials are made available
:: under the terms of "Eclipse Public License v1.0"
:: which accompanies this distribution, and is available
:: at the URL "http://www.eclipse.org/legal/epl-v10.html".
::
:: Initial Contributors:
:: Nokia Corporation - initial contribution.
::
:: Contributors:
::
:: Description:
::

@ECHO OFF

perl -S tranasm.pl %1 %2 %3 %4 %5 %6 %7 %8 %9
if errorlevel==1 goto CheckPerl
goto End

:CheckPerl
perl -v >NUL
if errorlevel==1 echo Is Perl, version 5.6.1 or later, installed?
goto End

:End
