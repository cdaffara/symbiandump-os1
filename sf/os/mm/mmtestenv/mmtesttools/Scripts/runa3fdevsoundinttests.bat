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

del e:\logs\testresults\tsi_a3f_devsound_pcm_record.htm
del e:\logs\testresults\tsi_a3f_devsound_pcm_play.htm
del e:\logs\testresults\tsi_a3f_devsound_tone.htm
md e:\logs\
md e:\logs\testresults\

md c:\tsi_a3f_devsound\

REM *** A3F Devsound IntegrationTests ***********************************************

copy z:\tsi_a3f_devsound_record.ini c:\tsi_a3f_devsound\tsi_a3f_devsound_record.ini
copy z:\tsi_a3f_devsound_play.ini c:\tsi_a3f_devsound\tsi_a3f_devsound_play.ini
copy z:\tsi_a3f_devsound_tone.ini c:\tsi_a3f_devsound\tsi_a3f_devsound_tone.ini

REM Copy these files over separately as copying all files causes record to crash
copy e:\inttest\a3f\tsi_a3f_devsound\tsi_a3f_devsound_pcm_record.script c:\tsi_a3f_devsound\
copy e:\inttest\a3f\tsi_a3f_devsound\tsi_a3f_devsound_record.ini c:\tsi_a3f_devsound\

testframework tsi_a3f_devsound_pcm_record.script
move c:\logs\testresults\*.htm e:\logs\testresults\

REM Move the generated record files back to the MMC
move c:\tsi_a3f_devsound\output\*.* e:\inttest\a3f\tsi_a3f_devsound\output\

REM Test data
copy e:\inttest\a3f\tsi_a3f_devsound\*.* c:\tsi_a3f_devsound\

testframework tsi_a3f_devsound_pcm_play.script
move c:\logs\testresults\*.htm e:\logs\testresults\

testframework tsi_a3f_devsound_tone.script
move c:\logs\testresults\*.htm e:\logs\testresults\

del c:\tsi_a3f_devsound\*.*
