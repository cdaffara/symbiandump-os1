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
rem Image Encoder Integration Tests
rem 
rem

del E:\Logs\TestResults\icl\imageencoder\TSI_ICL_IMAGEENCODER_MIRROR1.htm
del E:\Logs\TestResults\icl\imageencoder\TSI_ICL_IMAGEENCODER_MIRROR2.htm

md  E:\Logs\
md  E:\Logs\TestResults\
md  E:\Logs\TestResults\icl\
md  E:\Logs\TestResults\icl\imageencoder\

md  c:\icl\
md  c:\icl\testimages\
md  c:\icl\testimages\mbm\
md  c:\icl\testimages\jpeg\


copy E:\inttest\icl\testimages\jpeg\jpeg_small10.jpg         c:\icl\testimages\jpeg\jpeg_small10.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small08.jpg         c:\icl\testimages\jpeg\jpeg_small08.jpg
copy E:\inttest\icl\testimages\mbm\bitmap_small01.mbm        c:\icl\testimages\mbm\bitmap_small01.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_wideandthin01.mbm  c:\icl\testimages\mbm\bitmap_wideandthin01.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_tallandthin01.mbm  c:\icl\testimages\mbm\bitmap_tallandthin01.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_wideandthin14.mbm  c:\icl\testimages\mbm\bitmap_wideandthin14.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_tallandthin17.mbm  c:\icl\testimages\mbm\bitmap_tallandthin17.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_wideandthin16.mbm  c:\icl\testimages\mbm\bitmap_wideandthin16.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_tallandthin16.mbm  c:\icl\testimages\mbm\bitmap_tallandthin16.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_wideandthin14.mbm  c:\icl\testimages\mbm\bitmap_wideandthin14.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_wideandthin08.mbm  c:\icl\testimages\mbm\bitmap_wideandthin08.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_tallandthin08.mbm  c:\icl\testimages\mbm\bitmap_tallandthin08.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_wideandthin13.mbm  c:\icl\testimages\mbm\bitmap_wideandthin13.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_tallandthin06.mbm  c:\icl\testimages\mbm\bitmap_tallandthin06.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_small08.mbm        c:\icl\testimages\mbm\bitmap_small08.mbm   
copy E:\inttest\icl\testimages\mbm\bitmap_verysmall02.mbm    c:\icl\testimages\mbm\bitmap_verysmall02.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_average02.mbm      c:\icl\testimages\mbm\bitmap_average02.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_verysmall01.mbm    c:\icl\testimages\mbm\bitmap_verysmall01.mbm
copy E:\inttest\icl\testimages\mbm\jpeg_multiscan.mbm        c:\icl\testimages\mbm\jpeg_multiscan.mbm
copy E:\inttest\icl\testimages\jpeg\jpeg_multiscan.jpg       c:\icl\testimages\jpeg\jpeg_multiscan.jpg
copy E:\inttest\icl\testimages\mbm\bitmap_restartinterop.mbm c:\icl\testimages\mbm\bitmap_restartinterop.mbm

testframework -q \icl\imageencoder\tsi_icl_imageencoder_mirror1.script
move C:\Logs\TestResults\icl\imageencoder\*.htm E:\Logs\TestResults\icl\imageencoder\


del c:\icl\testimages\mbm\*.*
del c:\icl\testimages\jpeg\*.*

REM Copy output images from hardware to MMC, if output images are generated for hw
REM E:\inttest\icl\ folder will be already existing in MMC
md E:\inttest\icl\outputimages\
md E:\inttest\icl\outputimages\mbm\
md E:\inttest\icl\outputimages\jpeg\

copy c:\icl\outputimages\mbm\*.* E:\inttest\icl\outputimages\mbm\*.*
del c:\icl\outputimages\mbm\*.*
copy c:\icl\outputimages\jpeg\*.* E:\inttest\icl\outputimages\jpeg\*.*
del c:\icl\outputimages\jpeg\*.*

REM copy bitmap_average01.mbm & bitmap_small03.mbm  from MMC to HW and run tsi_icl_imageencoder_mirror2.script 

md  c:\icl\testimages\mbm\
copy E:\inttest\icl\testimages\mbm\bitmap_small03.mbm        c:\icl\testimages\mbm\bitmap_small03.mbm
copy E:\inttest\icl\testimages\mbm\bitmap_average01.mbm      c:\icl\testimages\mbm\bitmap_average01.mbm

testframework -q \icl\imageencoder\tsi_icl_imageencoder_mirror2.script
move C:\Logs\TestResults\icl\imageencoder\*.htm E:\Logs\TestResults\icl\imageencoder\

copy c:\icl\outputimages\mbm\*.* E:\inttest\icl\outputimages\mbm\*.*
del c:\icl\outputimages\mbm\*.*
