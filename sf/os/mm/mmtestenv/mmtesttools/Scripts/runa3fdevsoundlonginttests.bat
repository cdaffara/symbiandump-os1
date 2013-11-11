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

del e:\logs\testresults\tsi_a3f_devsound_pcm_long_record.htm
del e:\logs\testresults\tsi_a3f_devsound_pcm_long_play.htm
md e:\logs\
md e:\logs\testresults\

md c:\tsi_a3f_devsound\

REM *** A3F Devsound Long IntegrationTests ***********************************************

copy z:\tsi_a3f_devsound_record.ini c:\tsi_a3f_devsound\tsi_a3f_devsound_record.ini
REM To be uncommented if long playback test(s) required
REM copy z:\tsi_a3f_devsound_play.ini c:\tsi_a3f_devsound\tsi_a3f_devsound_play.ini


REM Run the tests
testframework tsi_a3f_devsound_pcm_long_record.script
move c:\logs\testresults\*.htm e:\logs\testresults\
REM Move the generated record files back to the MMC
move c:\tsi_a3f_devsound\output\*.* e:\inttest\a3f\tsi_a3f_devsound\output\

REM To be uncommented if long playback script required
REM testframework tsi_a3f_devsound_pcm_long_play.script
REM To be uncommented if long playback test(s) required
REM move c:\logs\testresults\*.htm e:\logs\testresults\

del c:\tsi_a3f_devsound\*.*
