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


del e:\logs\testresults\mm_a3f_devsound_init.htm
del e:\logs\testresults\mm_a3f_devsound_play_raw.htm
del e:\logs\testresults\mm_a3f_devsound_record_raw.htm
del e:\logs\testresults\mm_a3f_devsound_play_tone.htm
md E:\logs
md E:\logs\testresults
md C:\char_a3f_devsound_test_data

REM ***************************************************************************
REM **************** DEVSOUND COMPLIANCESUITE Tests **********
REM ***************************************************************************


copy e:\char_a3f_devsound_test_data\cerrodelasilla.raw c:\char_a3f_devsound_test_data\cerrodelasilla.raw
copy e:\char_a3f_devsound_test_data\recordc.raw c:\char_a3f_devsound_test_data\recordc.raw
copy e:\char_a3f_devsound_test_data\sample.aud c:\char_a3f_devsound_test_data\sample.aud
copy e:\char_a3f_devsound_test_data\testing123.raw c:\char_a3f_devsound_test_data\testing123.raw

copy z:\mm_a3f_devsound_init.ini c:\mm_a3f_devsound_init.ini
copy z:\mm_a3f_devsound_play_raw.ini c:\mm_a3f_devsound_play_raw.ini
copy z:\mm_a3f_devsound_play_tone.ini c:\mm_a3f_devsound_play_tone.ini
copy z:\mm_a3f_devsound_record_raw.ini c:\mm_a3f_devsound_record_raw.ini


testframework mm_a3f_devsound_init.script
move c:\logs\testresults\*.htm e:\logs\testresults\


testframework mm_a3f_devsound_play_raw.script
move c:\logs\testresults\*.htm e:\logs\testresults\


testframework mm_a3f_devsound_record_raw.script
move c:\logs\testresults\*.htm e:\logs\testresults\


testframework mm_a3f_devsound_play_tone.script
move c:\logs\testresults\*.htm e:\logs\testresults\