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

del E:\Logs\TestResults\icl\imageencoder\TSI_ICL_IMAGEENCODER_STREAM.htm

md  E:\Logs\
md  E:\Logs\TestResults\
md  E:\Logs\TestResults\icl\
md  E:\Logs\TestResults\icl\imageencoder\

md  c:\icl\
md  c:\icl\testimages\
md  c:\icl\testimages\mbm\
md  c:\icl\testimages\jpeg\


copy E:\inttest\icl\testimages\jpeg\jpeg_small15.jpg    c:\icl\testimages\jpeg\jpeg_small15.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small10.jpg    c:\icl\testimages\jpeg\jpeg_small10.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small01.jpg    c:\icl\testimages\jpeg\jpeg_small01.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_a.jpg  c:\icl\testimages\jpeg\jpeg_small18_a.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_b.jpg  c:\icl\testimages\jpeg\jpeg_small18_b.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_c.jpg  c:\icl\testimages\jpeg\jpeg_small18_c.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_d.jpg  c:\icl\testimages\jpeg\jpeg_small18_d.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_e.jpg  c:\icl\testimages\jpeg\jpeg_small18_e.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_f.jpg  c:\icl\testimages\jpeg\jpeg_small18_f.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_g.jpg  c:\icl\testimages\jpeg\jpeg_small18_g.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_h.jpg  c:\icl\testimages\jpeg\jpeg_small18_h.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_i.jpg  c:\icl\testimages\jpeg\jpeg_small18_i.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small18_j.jpg  c:\icl\testimages\jpeg\jpeg_small18_j.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small14_a.jpg  c:\icl\testimages\jpeg\jpeg_small14_a.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small10_a.jpg  c:\icl\testimages\jpeg\jpeg_small10_a.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small09_a.jpg  c:\icl\testimages\jpeg\jpeg_small09_a.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small13_a.jpg  c:\icl\testimages\jpeg\jpeg_small13_a.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_supersize01.jpg c:\icl\testimages\jpeg\jpeg_supersize01.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_multiscan_a.jpg c:\icl\testimages\jpeg\jpeg_multiscan_a.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_multiscan_b.jpg c:\icl\testimages\jpeg\jpeg_multiscan_b.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_multiscan_c.jpg c:\icl\testimages\jpeg\jpeg_multiscan_c.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_multiscan_d.jpg c:\icl\testimages\jpeg\jpeg_multiscan_d.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_wideandthin17.jpg c:\icl\testimages\jpeg\jpeg_wideandthin17.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_wideandthin02.jpg c:\icl\testimages\jpeg\jpeg_wideandthin02.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_wideandthin03.jpg c:\icl\testimages\jpeg\jpeg_wideandthin03.jpg


testframework -q \icl\imageencoder\tsi_icl_imageencoder_stream.script
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