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
@echo off

echo replacing wstest_t_allautotests.script.
del wstest_t_allautotests.script 2> NUL
echo //Run CreateAllScript.bat to generate this script from all of the autotests. 
echo PRINT Run all autotests >> wstest_t_allautotests.script
echo LOAD_SUITE tautoserver_nga >> wstest_t_allautotests.script
echo DELAY 5000 >> wstest_t_allautotests.script
for %%x in (*_nga.script) do echo RUN_SCRIPT z:\wstest\%%x >> wstest_t_allautotests.script