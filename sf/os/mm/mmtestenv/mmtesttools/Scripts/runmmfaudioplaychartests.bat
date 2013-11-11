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

del E:\Logs\TestResults\char_mmf_audioplay_none.htm
del E:\Logs\TestResults\char_mmf_audioplay_wav.htm
del E:\Logs\TestResults\char_mmf_audioplay_ogg.htm
del E:\Logs\TestResults\char_mmf_audioplay_amr.htm
del E:\Logs\TestResults\char_mmf_audioplay_aac.htm
del E:\Logs\TestResults\char_mmf_audioplay_mp3.htm
del E:\Logs\TestResults\char_mmf_audioplay_wav_notification.htm
del E:\Logs\TestResults\char_mmf_audioplay_ogg_notification.htm
del E:\Logs\TestResults\char_mmf_audioplay_amr_notification.htm
del E:\Logs\TestResults\char_mmf_audioplay_aac_notification.htm
del E:\Logs\TestResults\char_mmf_audioplay_mp3_notification.htm
del E:\Logs\TestResults\char_mmf_audioplay_panic_debug.htm

md E:\Logs\
md E:\Logs\TestResults\

REM *******************************************************************
REM ************** AUDIO PLAYER TESTS FOR cHARACTERISATION **********
REM *******************************************************************

md c:\char\
md c:\char\mmf\
md c:\char\mmf\data\

copy E:\char\mmf\data\mainTst.wav c:\char\mmf\data\mainTst.wav
copy E:\char\mmf\data\Corrupt.wav c:\char\mmf\data\Corrupt.wav
copy E:\char\mmf\data\maintst.wma c:\char\mmf\data\maintst.wma
copy E:\char\mmf\data\OneSample.wav c:\char\mmf\data\OneSample.wav
copy E:\char\mmf\data\maintst.ogg c:\char\mmf\data\maintst.ogg
copy E:\char\mmf\data\corrupt.ogg c:\char\mmf\data\corrupt.ogg
copy E:\char\mmf\data\small.ogg c:\char\mmf\data\small.ogg
copy E:\char\mmf\data\metadata.ogg c:\char\mmf\data\metadata.ogg

copy E:\char\mmf\data\*.mp3 c:\char\mmf\data\
copy E:\char\mmf\data\*.amr c:\char\mmf\data\
copy E:\char\mmf\data\*.aac c:\char\mmf\data\

REM Codecs for AMR, MP3 and AAC are not normally present, so comment out.

testframework char_mmf_audioplay_none.script
testframework char_mmf_audioplay_wav.script
testframework char_mmf_audioplay_ogg.script
REM testframework char_mmf_audioplay_amr.script
REM testframework char_mmf_audioplay_aac.script
REM testframework char_mmf_audioplay_mp3.script

testframework char_mmf_audioplay_wav_notification.script
testframework char_mmf_audioplay_ogg_notification.script
REM testframework char_mmf_audioplay_amr_notification.script
REM testframework char_mmf_audioplay_aac_notification.script
REM testframework char_mmf_audioplay_mp3_notification.script

REM Panic test is only for emulator UDEB, they should not run on hardware, so commented here.
REM testframework char_mmf_audioplay_panic_debug.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\
del c:\char\mmf\data\*.*
