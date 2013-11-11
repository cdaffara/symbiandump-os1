rem Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del E:\Logs\TestResults\char_mmf_audiorec_ogg_overflow.htm
md E:\Logs\
md E:\Logs\TestResults\

REM *******************************************************************
REM ************** AUDIO RECORDER TESTS FOR cHARACTERISATION **********
REM *******************************************************************

md c:\char\
md c:\char\mmf\
md c:\char\mmf\data\

copy E:\char\mmf\data\maintst2.wav c:\char\mmf\data\mainTst2.wav
copy E:\char\mmf\data\maintst2.ogg c:\char\mmf\data\mainTst2.ogg
copy E:\char\mmf\data\metadata.ogg c:\char\mmf\data\metadata.ogg

testframework char_mmf_audiorec_ogg_overflow.script

move C:\Logs\TestResults\char_mmf_audiorec_ogg_overflow.htm E:\Logs\TestResults\
del c:\char\mmf\data\*.*
