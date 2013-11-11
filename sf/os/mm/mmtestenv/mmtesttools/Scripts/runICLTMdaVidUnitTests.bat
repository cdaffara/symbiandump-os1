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

del E:\Logs\TestResults\TSU_ICL_TMDAVID*.*
md E:\Logs
md E:\Logs\TestResults

REM ***********************************************
REM ************** TMDAVID TESTS ******************
REM ***********************************************

md c:\tmdavid\
md c:\tmdavid\Anon
md c:\tmdavid\Ref

copy E:\unittest\icl\tmdavid\bmp1bpp.bmp c:\tmdavid\bmp1bpp.bmp
copy E:\unittest\icl\tmdavid\bmp24bpp.bmp c:\tmdavid\bmp24bpp.bmp
copy E:\unittest\icl\tmdavid\bmp4bpp.bmp c:\tmdavid\bmp4bpp.bmp
copy E:\unittest\icl\tmdavid\bmp8bpp.bmp c:\tmdavid\bmp8bpp.bmp
copy E:\unittest\icl\tmdavid\bmprle4.bmp c:\tmdavid\bmprle4.bmp
copy E:\unittest\icl\tmdavid\bmprle8.bmp c:\tmdavid\bmprle8.bmp
copy E:\unittest\icl\tmdavid\jfif10.jpg c:\tmdavid\jfif10.jpg
copy E:\unittest\icl\tmdavid\jfif100.jpg c:\tmdavid\jfif100.jpg
copy E:\unittest\icl\tmdavid\jfif30.jpg c:\tmdavid\jfif30.jpg
copy E:\unittest\icl\tmdavid\jfif420.jpg c:\tmdavid\jfif420.jpg
copy E:\unittest\icl\tmdavid\jfif422.jpg c:\tmdavid\jfif422.jpg
copy E:\unittest\icl\tmdavid\jfif444.jpg c:\tmdavid\jfif444.jpg
copy E:\unittest\icl\tmdavid\jfif60.jpg c:\tmdavid\jfif60.jpg
copy E:\unittest\icl\tmdavid\jfifMono.jpg c:\tmdavid\jfifMono.jpg
copy E:\unittest\icl\tmdavid\jfifProg.jpg c:\tmdavid\jfifProg.jpg
copy E:\unittest\icl\tmdavid\mbm12bpp.mbm c:\tmdavid\mbm12bpp.mbm
copy E:\unittest\icl\tmdavid\mbm16bpp.mbm c:\tmdavid\mbm16bpp.mbm
copy E:\unittest\icl\tmdavid\mbm1bpp.mbm c:\tmdavid\mbm1bpp.mbm
copy E:\unittest\icl\tmdavid\mbm24bpp.mbm c:\tmdavid\mbm24bpp.mbm
copy E:\unittest\icl\tmdavid\mbm2bpp.mbm c:\tmdavid\mbm2bpp.mbm
copy E:\unittest\icl\tmdavid\mbm4bppc.mbm c:\tmdavid\mbm4bppc.mbm
copy E:\unittest\icl\tmdavid\mbm4bppm.mbm c:\tmdavid\mbm4bppm.mbm
copy E:\unittest\icl\tmdavid\mbm8bppc.mbm c:\tmdavid\mbm8bppc.mbm
copy E:\unittest\icl\tmdavid\mbm8bppm.mbm c:\tmdavid\mbm8bppm.mbm
copy E:\unittest\icl\tmdavid\png1.png c:\tmdavid\png1.png
copy E:\unittest\icl\tmdavid\png2.png c:\tmdavid\png2.png
copy E:\unittest\icl\tmdavid\png3.png c:\tmdavid\png3.png
copy E:\unittest\icl\tmdavid\png4.png c:\tmdavid\png4.png
copy E:\unittest\icl\tmdavid\tif1.tif c:\tmdavid\tif1.tif
copy E:\unittest\icl\tmdavid\tif2.tif c:\tmdavid\tif2.tif
copy E:\unittest\icl\tmdavid\tif3.tif c:\tmdavid\tif3.tif
copy E:\unittest\icl\tmdavid\tmdavid.mbm c:\tmdavid\tmdavid.mbm
copy E:\unittest\icl\tmdavid\trans.gif c:\tmdavid\trans.gif
copy E:\unittest\icl\tmdavid\wmf1.wmf c:\tmdavid\wmf1.wmf
copy E:\unittest\icl\tmdavid\wmf2.wmf c:\tmdavid\wmf2.wmf
copy E:\unittest\icl\tmdavid\wmf3.wmf c:\tmdavid\wmf3.wmf
copy E:\unittest\icl\tmdavid\wmf4.wmf c:\tmdavid\wmf4.wmf
copy E:\unittest\icl\tmdavid\wmf5.wmf c:\tmdavid\wmf5.wmf
copy E:\unittest\icl\tmdavid\wmf6.wmf c:\tmdavid\wmf6.wmf

copy E:\unittest\icl\tmdavid\ref\bmp1bpp.mbm c:\tmdavid\ref\bmp1bpp.mbm
copy E:\unittest\icl\tmdavid\ref\bmp24bpp.mbm c:\tmdavid\ref\bmp24bpp.mbm
copy E:\unittest\icl\tmdavid\ref\bmp4bpp.mbm c:\tmdavid\ref\bmp4bpp.mbm
copy E:\unittest\icl\tmdavid\ref\bmp8bpp.mbm c:\tmdavid\ref\bmp8bpp.mbm
copy E:\unittest\icl\tmdavid\ref\bmprle4.mbm c:\tmdavid\ref\bmprle4.mbm
copy E:\unittest\icl\tmdavid\ref\bmprle8.mbm c:\tmdavid\ref\bmprle8.mbm
copy E:\unittest\icl\tmdavid\ref\jfif10.mbm c:\tmdavid\ref\jfif10.mbm
copy E:\unittest\icl\tmdavid\ref\jfif100.mbm c:\tmdavid\ref\jfif100.mbm
copy E:\unittest\icl\tmdavid\ref\jfif30.mbm c:\tmdavid\ref\jfif30.mbm
copy E:\unittest\icl\tmdavid\ref\jfif420.mbm c:\tmdavid\ref\jfif420.mbm
copy E:\unittest\icl\tmdavid\ref\jfif422.mbm c:\tmdavid\ref\jfif422.mbm
copy E:\unittest\icl\tmdavid\ref\jfif444.mbm c:\tmdavid\ref\jfif444.mbm
copy E:\unittest\icl\tmdavid\ref\jfif60.mbm c:\tmdavid\ref\jfif60.mbm
copy E:\unittest\icl\tmdavid\ref\jfifMono.mbm c:\tmdavid\ref\jfifMono.mbm
copy E:\unittest\icl\tmdavid\ref\jfifProg.mbm c:\tmdavid\ref\jfifProg.mbm
copy E:\unittest\icl\tmdavid\ref\mbm12bpp.mbm c:\tmdavid\ref\mbm12bpp.mbm
copy E:\unittest\icl\tmdavid\ref\mbm16bpp.mbm c:\tmdavid\ref\mbm16bpp.mbm
copy E:\unittest\icl\tmdavid\ref\mbm1bpp.mbm c:\tmdavid\ref\mbm1bpp.mbm
copy E:\unittest\icl\tmdavid\ref\mbm24bpp.mbm c:\tmdavid\ref\mbm24bpp.mbm
copy E:\unittest\icl\tmdavid\ref\mbm2bpp.mbm c:\tmdavid\ref\mbm2bpp.mbm
copy E:\unittest\icl\tmdavid\ref\mbm4bppc.mbm c:\tmdavid\ref\mbm4bppc.mbm
copy E:\unittest\icl\tmdavid\ref\mbm4bppm.mbm c:\tmdavid\ref\mbm4bppm.mbm
copy E:\unittest\icl\tmdavid\ref\mbm8bppc.mbm c:\tmdavid\ref\mbm8bppc.mbm
copy E:\unittest\icl\tmdavid\ref\mbm8bppm.mbm c:\tmdavid\ref\mbm8bppm.mbm
copy E:\unittest\icl\tmdavid\ref\png1.mbm c:\tmdavid\ref\png1.mbm
copy E:\unittest\icl\tmdavid\ref\png2.mbm c:\tmdavid\ref\png2.mbm
copy E:\unittest\icl\tmdavid\ref\png3.mbm c:\tmdavid\ref\png3.mbm
copy E:\unittest\icl\tmdavid\ref\png4.mbm c:\tmdavid\ref\png4.mbm
copy E:\unittest\icl\tmdavid\ref\scaled0.mbm c:\tmdavid\ref\scaled0.mbm
copy E:\unittest\icl\tmdavid\ref\scaled1.mbm c:\tmdavid\ref\scaled1.mbm
copy E:\unittest\icl\tmdavid\ref\scaled2.mbm c:\tmdavid\ref\scaled2.mbm
copy E:\unittest\icl\tmdavid\ref\scaled3.mbm c:\tmdavid\ref\scaled3.mbm
copy E:\unittest\icl\tmdavid\ref\scaled4.mbm c:\tmdavid\ref\scaled4.mbm
copy E:\unittest\icl\tmdavid\ref\scaled5.mbm c:\tmdavid\ref\scaled5.mbm
copy E:\unittest\icl\tmdavid\ref\tif1.mbm c:\tmdavid\ref\tif1.mbm
copy E:\unittest\icl\tmdavid\ref\tif2.mbm c:\tmdavid\ref\tif2.mbm
copy E:\unittest\icl\tmdavid\ref\tif3.mbm c:\tmdavid\ref\tif3.mbm
copy E:\unittest\icl\tmdavid\ref\tmdavid.mbm c:\tmdavid\ref\tmdavid.mbm
copy E:\unittest\icl\tmdavid\ref\trans.mbm c:\tmdavid\ref\trans.mbm
copy E:\unittest\icl\tmdavid\ref\wmf1.mbm c:\tmdavid\ref\wmf1.mbm
copy E:\unittest\icl\tmdavid\ref\wmf2.mbm c:\tmdavid\ref\wmf2.mbm
copy E:\unittest\icl\tmdavid\ref\wmf3.mbm c:\tmdavid\ref\wmf3.mbm
copy E:\unittest\icl\tmdavid\ref\wmf4.mbm c:\tmdavid\ref\wmf4.mbm
copy E:\unittest\icl\tmdavid\ref\wmf5.mbm c:\tmdavid\ref\wmf5.mbm
copy E:\unittest\icl\tmdavid\ref\wmf6.mbm c:\tmdavid\ref\wmf6.mbm

copy E:\unittest\icl\tmdavid\anon\otabitmap c:\tmdavid\anon\otabitmap
copy E:\unittest\icl\tmdavid\anon\otabitmap.mbm c:\tmdavid\anon\otabitmap.mbm
copy E:\unittest\icl\tmdavid\anon\wirelessbitmap c:\tmdavid\anon\wirelessbitmap
copy E:\unittest\icl\tmdavid\anon\wirelessbitmap.mbm c:\tmdavid\anon\wirelessbitmap.mbm

testframework -q TSU_ICL_TMDAVID.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\tmdavid\Anon\*.*
del c:\tmdavid\Ref\*.*
del c:\tmdavid\*.*
