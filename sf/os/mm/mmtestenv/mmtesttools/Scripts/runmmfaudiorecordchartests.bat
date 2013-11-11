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

del E:\Logs\TestResults\char_mmf_audiorec_wav.htm
del E:\Logs\TestResults\char_mmf_audiorec_ogg.htm
del E:\Logs\TestResults\char_mmf_audiorec_amr.htm
del E:\Logs\TestResults\char_mmf_audiorec_aac.htm
del E:\Logs\TestResults\char_mmf_audiorec_wav_notification.htm
del E:\Logs\TestResults\char_mmf_audiorec_ogg_notification.htm
del E:\Logs\TestResults\char_mmf_audiorec_amr_notification.htm
del E:\Logs\TestResults\char_mmf_audiorec_aac_notification.htm
del E:\Logs\TestResults\char_mmf_audiorec_panic_emu_debug.htm
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
copy E:\char\mmf\data\*.aac c:\char\mmf\data\
copy E:\char\mmf\data\*.amr c:\char\mmf\data\

REM Codecs for AMR, MP3 and AAC are not normally present, so comment out.

testframework char_mmf_audiorec_wav.script
testframework char_mmf_audiorec_ogg.script
REM testframework char_mmf_audiorec_amr.script
REM testframework char_mmf_audiorec_aac.script

testframework char_mmf_audiorec_wav_notification.script
testframework char_mmf_audiorec_ogg_notification.script
REM testframework char_mmf_audiorec_amr_notification.script
REM testframework char_mmf_audiorec_aac_notification.script

REM Panic test is only for emulator UDEB, they should not run on hardware, so commented here.
REM testframework char_mmf_audiorec_panic_emu_debug.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\
del c:\char\mmf\data\*.*
