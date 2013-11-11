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

del e:\logs\testresults\tsu_mmf_devvideo*.*
md e:\logs\
md e:\logs\testresults\

REM ***********************************************
REM **************** DEVVIDEO Tests ***************
REM ***********************************************

md c:\mm\
md c:\tsi_mmf_devvideo\

copy z:\tsi_mmf_devvideo.ini c:\mm\tsi_mmf_devvideo.ini

copy e:\inttest\mmf\tsi_mmf_devvideo\xvid_decframe.mp4v c:\tsi_mmf_devvideo\xvid_decframe.mp4v
copy e:\inttest\mmf\tsi_mmf_devvideo\xvid_encframe.mbm c:\tsi_mmf_devvideo\xvid_encframe.mbm
copy e:\inttest\mmf\tsi_mmf_devvideo\xvid_encframe.bin c:\tsi_mmf_devvideo\xvid_encframe.bin
copy e:\inttest\mmf\tsi_mmf_devvideo\test_encframe.bin c:\tsi_mmf_devvideo\test_encframe.bin
copy e:\inttest\mmf\tsi_mmf_devvideo\pcm_08_16_m.raw c:\tsi_mmf_devvideo\pcm_08_16_m.raw
copy e:\inttest\mmf\tsi_mmf_devvideo\decodervideotest.in c:\tsi_mmf_devvideo\decodervideotest.in

testframework tsi_mmf_devvideo_00.script
move c:\logs\testresults\*.htm e:\logs\testresults\

testframework tsi_mmf_devvideo_01.script
move c:\logs\testresults\*.htm e:\logs\testresults\

testframework tsi_mmf_devvideo_02.script
move c:\logs\testresults\*.htm e:\logs\testresults\

testframework tsi_mmf_devvideo_alloc.script
move c:\logs\testresults\*.htm e:\logs\testresults\

del c:\tsi_mmf_devvideo\*.*
del c:\mm\*.*
