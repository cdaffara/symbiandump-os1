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

del E:\Logs\TestResults\tsu_icl_ITFM_01*.*
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** ITFM TESTS *****************
REM ***********************************************

md c:\TSU_ICL_ITFM_01\
md c:\TSU_ICL_ITFM_01\ref
md c:\TSU_ICL_ITFM_01\output

copy E:\unittest\icl\TSU_ICL_ITFM_01\mbmProg.mbm    c:\TSU_ICL_ITFM_01\mbmProg.mbm
copy E:\unittest\icl\TSU_ICL_ITFM_01\png1.png        c:\TSU_ICL_ITFM_01\png1.png
copy E:\unittest\icl\TSU_ICL_ITFM_01\test.txt        c:\TSU_ICL_ITFM_01\test.txt
copy E:\unittest\icl\TSU_ICL_ITFM_01\TooShort.txt    c:\TSU_ICL_ITFM_01\TooShort.txt
copy E:\unittest\icl\TSU_ICL_ITFM_01\0thIFDTag_0000.jpg c:\TSU_ICL_ITFM_01\0thIFDTag_0000.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\0thIFDTag_7FFF.jpg c:\TSU_ICL_ITFM_01\0thIFDTag_7FFF.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\0thIFDTag_FFFF.jpg c:\TSU_ICL_ITFM_01\0thIFDTag_FFFF.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Leave1.jpg c:\TSU_ICL_ITFM_01\IllegalExifImage_Leave1.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Leave2.jpg c:\TSU_ICL_ITFM_01\IllegalExifImage_Leave2.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Leave3.jpg c:\TSU_ICL_ITFM_01\IllegalExifImage_Leave3.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Panic1.jpg c:\TSU_ICL_ITFM_01\IllegalExifImage_Panic1.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\IllegalExifImage_Panic2.jpg c:\TSU_ICL_ITFM_01\IllegalExifImage_Panic2.jpg

copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\mbmProg_100x100_m.mbm c:\TSU_ICL_ITFM_01\ref\mbmProg_100x100_m.mbm
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\mbmProg_160x90.mbm    c:\TSU_ICL_ITFM_01\ref\mbmProg_160x90.mbm
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\png1_100x100_m.png     c:\TSU_ICL_ITFM_01\ref\png1_100x100_m.png
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\0thIFDTag_0000_100x100_m.jpg c:\TSU_ICL_ITFM_01\ref\0thIFDTag_0000_100x100_m.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\0thIFDTag_7FFF_100x100_m.jpg c:\TSU_ICL_ITFM_01\ref\0thIFDTag_7FFF_100x100_m.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\0thIFDTag_FFFF_100x100_m.jpg c:\TSU_ICL_ITFM_01\ref\0thIFDTag_FFFF_100x100_m.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave1_100x100_m.jpg c:\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave1_100x100_m.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave2_100x100_m.jpg c:\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave2_100x100_m.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave3_100x100_m.jpg c:\TSU_ICL_ITFM_01\ref\IllegalExifImage_Leave3_100x100_m.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Panic1_100x100_m.jpg c:\TSU_ICL_ITFM_01\ref\IllegalExifImage_Panic1_100x100_m.jpg
copy E:\unittest\icl\TSU_ICL_ITFM_01\ref\IllegalExifImage_Panic2_100x100_m.jpg c:\TSU_ICL_ITFM_01\ref\IllegalExifImage_Panic2_100x100_m.jpg


testframeworknone -q tsu_icl_ITFM_01_nocap.script
move C:\Logs\TestResults\tsu_icl_ITFM*.htm E:\Logs\TestResults\

testframeworknone -q tsu_icl_ITFM_01_nocap_ALLOC.script
move C:\Logs\TestResults\tsu_icl_ITFM*.htm E:\Logs\TestResults\

testframeworknone -q tsu_icl_ITFM_01_panic_nocap.script
move C:\Logs\TestResults\tsu_icl_ITFM*.htm E:\Logs\TestResults\

del c:\TSU_ICL_ITFM_01\ref\*.*
del c:\TSU_ICL_ITFM_01\output\*.*
del c:\TSU_ICL_ITFM_01\*.*