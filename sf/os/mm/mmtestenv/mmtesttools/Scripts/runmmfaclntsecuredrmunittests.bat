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

del E:\Logs\TestResults\tsu_mmf_aclnt_securedrmtestdata.htm
del E:\Logs\TestResults\tsu_mmf_aclnt_securedrm.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** ACLNT SecureDRM TESTS**************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\aclnt\

copy z:\tsu_mmf_aclnt_securedrmtestdata.ini 	c:\mm\tsu_mmf_aclnt_securedrmtestdata.ini
copy z:\tsu_mmf_aclnt_securedrm.ini 			c:\mm\tsu_mmf_aclnt_securedrm.ini
copy z:\tsu_mmf_aclnt_securedrmtestdata.script  c:\mm\tsu_mmf_aclnt_securedrmtestdata.script
copy z:\tsu_mmf_aclnt_securedrm.script  c:\mm\tsu_mmf_aclnt_securedrm.script

copy e:\unittest\mmf\aclnt\small.wav c:\mm\mmf\testfiles\aclnt\small.wav
copy e:\unittest\mmf\aclnt\smallwav_audiocontent.xml c:\mm\mmf\testfiles\aclnt\smallwav_audiocontent.xml
copy e:\unittest\mmf\aclnt\smallwav_audiorights.xml c:\mm\mmf\testfiles\aclnt\smallwav_audiorights.xml

attrib c:\mm\mmf\testfiles\aclnt\*.* -r

md c:\logs\
md c:\logs\testexecute\
testexecute c:\mm\tsu_mmf_aclnt_securedrmtestdata.script
move c:\Logs\testexecute\*.htm e:\Logs\testexecute\
del c:\logs\testexecute\*.*

testframeworkmmddcap c:\mm\tsu_mmf_aclnt_securedrm.script
move c:\Logs\TestResults\mm\*.htm e:\Logs\TestResults\

del c:\Logs\TestResults\*.htm
del c:\mm\mmf\testfiles\aclnt\*.*
del c:\mm\tsu_mmf_aclnt_securedrmtestdata.ini
del c:\mm\tsu_mmf_aclnt_securedrm.ini
