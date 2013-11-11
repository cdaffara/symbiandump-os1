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

md c:\mm\
copy z:\mm\MMFTestFile1.dat c:\mm\MMFTestFile1.dat
copy z:\mm\MMFTestFile2.dat c:\mm\MMFTestFile2.dat
copy z:\mm\MMFTestFile3.dat c:\mm\MMFTestFile3.dat
copy z:\mm\MMFTestFile1.wav c:\mm\MMFTestFile1.wav
copy z:\mm\MMFTestFile2.wav c:\mm\MMFTestFile2.wav

copy z:\TSU_MMF_ACLNT_01.ini c:\mm\TSU_MMF_ACLNT_01.ini

testframework tsu_mmf_srssnk.script
testframework tsu_mmf_srssnk_alloc.script
testframework tsu_mmf_srssnk_fh_alloc.script
testframework tsu_mmf_srssnk_panic.script
