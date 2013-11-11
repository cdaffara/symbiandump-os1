rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del E:\Logs\TestResults\tsu_mmf_oggctrl_play.htm
del E:\Logs\TestResults\tsu_mmf_oggctrl_record.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** OGGCTRL TESTS *****************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\oggctrl\

copy E:\unittest\mmf\oggctrl\6Sec48KHz224KbsStereo.ogg c:\mm\mmf\testfiles\oggctrl\6Sec48KHz224KbsStereo.ogg
copy E:\unittest\mmf\oggctrl\6sec32khz120kbsstereo.ogg c:\mm\mmf\testfiles\oggctrl\6sec32khz120kbsstereo.ogg
copy E:\unittest\mmf\oggctrl\6sec44khz128kbsstereo.ogg c:\mm\mmf\testfiles\oggctrl\6sec44khz128kbsstereo.ogg
copy E:\unittest\mmf\oggctrl\invalid.ogg c:\mm\mmf\testfiles\oggctrl\invalid.ogg

testframework tsu_mmf_oggctrl_play.script
copy c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mmf_oggctrl_record.script
copy c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\oggctrl\*.*


