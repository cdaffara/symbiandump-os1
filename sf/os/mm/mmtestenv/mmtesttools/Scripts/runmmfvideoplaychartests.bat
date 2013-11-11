rem Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del E:\Logs\TestResults\char_mmf_videoplay*.*
md E:\Logs\
md E:\Logs\TestResults\

REM *******************************************************************
REM ************** VIDEO PLAYER TESTS FOR cHARACTERISATION **********
REM *******************************************************************

md c:\char\
md c:\char\mmf\
md c:\char\mmf\data\

copy E:\char\mmf\data\aud_vid.avi c:\char\mmf\data\aud_vid.avi
copy E:\char\mmf\data\jfif10_4.jpg c:\char\mmf\data\jfif10_4.jpg
copy E:\char\mmf\data\invalidcodecavi.avi c:\char\mmf\data\invalidcodecavi.avi
copy E:\char\mmf\data\xvidonly.avi c:\char\mmf\data\xvidonly.avi
copy E:\char\mmf\data\maintst.wav c:\char\mmf\data\maintst.wav
copy E:\char\mmf\data\maintst3.wav c:\char\mmf\data\maintst3.wav
copy E:\char\mmf\data\longvideoplay.avi c:\char\mmf\data\longvideoplay.avi
copy E:\char\mmf\data\Invalid c:\char\mmf\data\Invalid
copy E:\char\mmf\data\text.avi c:\char\mmf\data\text.avi
copy E:\char\mmf\data\xvidmp3.avi c:\char\mmf\data\xvidmp3.avi

testframework char_mmf_videoplay.script

testframework char_mmf_videoplay_notification.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\
del c:\char\mmf\data\*.*
