rem Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del e:\logs\testexecute\tsi_mmf_securedrm_testdata.*
del e:\logs\testresults\tsi_mmf_securedrm_video_mmddcap.*
del e:\logs\testresults\tsi_mmf_securedrm_audio_mmddcap.*

md e:\logs\
md e:\logs\testresults\
md e:\logs\testexecute\

REM ***********************************************
REM ***************** Securm DRM Integration TESTS *****************
REM ***********************************************

md c:\securedrm\

copy z:\securedrm_testdata.ini c:\securedrm_testdata.ini
copy z:\securedrm_video.ini c:\securedrm_video.ini
copy z:\securedrm_audio.ini c:\securedrm_audio.ini
copy e:\inttest\mmf\securedrm\* c:\securedrm\*
copy z:\tsi_mmf_securedrm_testdata.script c:\tsi_mmf_securedrm_testdata.script

testexecute \tsi_mmf_securedrm_testdata.script
testframeworkmmddcap tsi_mmf_securedrm_video_mmddcap.script
testframeworkmmddcap tsi_mmf_securedrm_audio_mmddcap.script
testframework tsi_mmf_securedrm_audio.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\
move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\

del c:\securedrm\*.*
del c:\tsi_mmf_securedrm_testdata.script
del c:\securedrm_testdata.ini
del c:\securedrm_video.ini
del c:\securedrm_audio.ini