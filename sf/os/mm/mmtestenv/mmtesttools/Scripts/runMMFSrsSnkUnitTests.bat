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

del E:\Logs\TestResults\tsu_mmf_srssnk_alloc.htm
del E:\Logs\TestResults\tsu_mmf_srssnk_panic.htm
del E:\Logs\TestResults\tsu_mmf_srssnk_fh_alloc.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** SRSSNK TESTS *****************
REM ***********************************************

md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\srssnk\

copy E:\unittest\mmf\srssnk\MMFTestFile1.dat c:\mm\mmf\testfiles\srssnk\MMFTestFile1.dat
copy E:\unittest\mmf\srssnk\MMFTestFile2.dat c:\mm\mmf\testfiles\srssnk\MMFTestFile2.dat
copy E:\unittest\mmf\srssnk\MMFTestFile3.dat c:\mm\mmf\testfiles\srssnk\MMFTestFile3.dat
copy E:\unittest\mmf\srssnk\MMFTestFile1.wav c:\mm\mmf\testfiles\srssnk\MMFTestFile1.wav
copy E:\unittest\mmf\srssnk\MMFTestFile2.wav c:\mm\mmf\testfiles\srssnk\MMFTestFile2.wav
copy z:\TSU_MMF_ACLNT_01.ini c:\mm\TSU_MMF_ACLNT_01.ini


testframework tsu_mmf_srssnk_alloc.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

testframework tsu_mmf_srssnk_panic.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

# Script Re-inserted after removing tests due to defect scrub of DEF059332
testframework tsu_mmf_srssnk_fh_alloc.script
move c:\Logs\TestResults\*.htm e:\Logs\TestResults\

del c:\mm\mmf\testfiles\srssnk\*.*
del c:\mm\TSU_MMF_ACLNT_01.ini