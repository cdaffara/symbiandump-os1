REM Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
REM All rights reserved.
REM This component and the accompanying materials are made available
REM under the terms of "Eclipse Public License v1.0"
REM which accompanies this distribution, and is available
REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
REM 
REM Initial Contributors:
REM Nokia Corporation - initial contribution.
REM 
REM Contributors:
REM 
REM Description:
REM 

python generator.py -i dllmmp.ini -t mmp dllmmp.tmpl ..\..\..\hci2implementations\CommandsEvents\symbian\group\commandsevents_symbian.mmp
python generator.py -i hcidata.ini -t impl
IF "%1" == "" GOTO :END
python generator.py -i testmmp.ini -t mmp testmmp.tmpl ..\..\..\hci2implementations\CommandsEvents\symbian\group\hcitestserver.mmp
python generator.py -i testserver.ini -t test
:END
