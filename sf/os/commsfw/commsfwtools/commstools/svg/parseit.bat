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

set LOGFILE=\epoc32\winscw\c\logs\ulogger.txt
set ARGS=

:getopts
if /I "x%1" == "x" goto nomoreopts
if /I "x%1" == "x/log" (
    set LOGFILE=%2
    shift
    shift
    goto getopts
)

set CURRENTARG=%1
shift

if /I "%CURRENTARG:~-7%" == "ulogger.txt" (
    set LOGFILE=%CURRENTARG%
    goto getopts
)

set ARGS=%ARGS% %CURRENTARG%
goto getopts

:nomoreopts

@echo Parsing log
perl -S parselog.pl %ARGS% -x parse.ini -s63 %LOGFILE% > log.seq 2> log.err
type log.err
perl -S parseseq.pl %ARGS% -x parse.ini -p -V log.seq
perl -S relations.pl -x relationships.txt log.seq > relateit.out 2>&1


set LOGFILE=
set ARGS=
set CURRENTARG=
