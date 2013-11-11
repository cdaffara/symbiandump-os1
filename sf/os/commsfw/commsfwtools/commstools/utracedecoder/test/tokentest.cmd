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

set TEST=%1

.\debug\messageparser tokenizer_%TEST%_test.txt > tokenizer_%TEST%_test_results.txt
C:\Progra~1\Beyond~1\BC2.exe /qc tokenizer_%TEST%_test_results.txt tokenizer_%TEST%_test_expectedresults.txt
if not %errorlevel% == 0 goto fail
echo %TEST% passed
goto done

:fail
echo %TEST% failed

:done
