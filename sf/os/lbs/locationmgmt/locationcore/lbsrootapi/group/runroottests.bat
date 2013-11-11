@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
rem @echo off

::
:: TESTS 0-7, 9-14
::
copy /y I:\epoc32\WINSCW\C\testdata\moduleini\teststepall\10282266.cre I:\epoc32\WINSCW\C\private\10202be9\10282266.cre
copy /y I:\epoc32\WINSCW\C\testdata\moduleini\teststepall\12345678.ini I:\epoc32\WINSCW\C\private\10282253\lbs\locmods\12345678.ini
copy /y I:\epoc32\WINSCW\C\testdata\moduleini\teststepall\98765432.ini I:\epoc32\WINSCW\C\private\10282253\lbs\locmods\98765432.ini

i:\epoc32\release\winscw\udeb\testexecute c:\testdata\scripts\te_lbsrootapiTestSuite2

copy /y I:\epoc32\WINSCW\C\testdata\moduleini\teststepall\12345678.ini I:\epoc32\WINSCW\C\private\10282253\lbs\locmods\12345678.ini
copy /y I:\epoc32\WINSCW\C\testdata\moduleini\teststepall\98765432.ini I:\epoc32\WINSCW\C\private\10282253\lbs\locmods\98765432.ini
::
:: TEST 8
::
:: Testing behaviour when the .cre file is missing
del I:\epoc32\WINSCW\C\private\10202be9\10282266.cre
i:\epoc32\release\winscw\udeb\testexecute c:\testdata\scripts\te_lbsrootapiTestSuite1

::
:: TEST 15
::
:: Process entered twice in initialization file
::
copy /y I:\epoc32\WINSCW\C\testdata\moduleini\teststep15\10282266.cre I:\epoc32\WINSCW\C\private\10202be9\10282266.cre
i:\epoc32\release\winscw\udeb\testexecute c:\testdata\scripts\te_lbsrootapiTestSuite3

::
:: TEST 16
::
:: Process UID missing from initialization file
::
copy /y I:\epoc32\WINSCW\C\testdata\moduleini\teststep16\10282266.cre I:\epoc32\WINSCW\C\private\10202be9\10282266.cre
i:\epoc32\release\winscw\udeb\testexecute c:\testdata\scripts\te_lbsrootapiTestSuite4

::
:: TEST 17
::
:: Test panic in process requesting system startup
::
copy /y I:\epoc32\WINSCW\C\testdata\moduleini\teststep17\10282266.cre I:\epoc32\WINSCW\C\private\10202be9\10282266.cre
i:\epoc32\release\winscw\udeb\testexecute c:\testdata\scripts\te_lbsrootapiTestSuite5

::
:: TEST 18
::
:: Entire system started on boot, with only on process (profile for 3.40 delivery)
:: 
copy /y I:\epoc32\WINSCW\C\testdata\moduleini\teststep18\10282266.cre I:\epoc32\WINSCW\C\private\10202be9\10282266.cre
i:\epoc32\release\winscw\udeb\testexecute c:\testdata\scripts\te_lbsrootapiTestSuite6
