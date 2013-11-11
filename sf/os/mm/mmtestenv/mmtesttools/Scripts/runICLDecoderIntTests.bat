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
rem Image Decoder Integration Tests
rem 
rem

del E:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_PERF.htm
del E:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_CROP.htm
del E:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_SCALE.htm
del E:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_ROTATION.htm
del E:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_MULTIPLEOP.htm
del E:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_EXTINTERFACE.htm
del E:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_AUTOROTATE.htm

md  E:\Logs\
md  E:\Logs\TestResults\
md  E:\Logs\TestResults\icl\
md  E:\Logs\TestResults\icl\imagedecoder\

md  c:\icl\
md  c:\icl\testimages\
md  c:\icl\testimages\mbm\
md  c:\icl\testimages\jpeg\
md  c:\icl\refimages\
md  C:\tsu_icl_exifimgdisp_01\

copy E:\inttest\icl\testimages\jpeg\jpeg_average01.jpg      c:\icl\testimages\jpeg\jpeg_average01.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_average02.jpg      c:\icl\testimages\jpeg\jpeg_average02.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_corrupt02.jpg      c:\icl\testimages\jpeg\jpeg_corrupt02.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_corrupt03.jpg      c:\icl\testimages\jpeg\jpeg_corrupt03.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_corrupt04.jpg      c:\icl\testimages\jpeg\jpeg_corrupt04.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_corrupt05.jpg      c:\icl\testimages\jpeg\jpeg_corrupt05.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_corrupt10.jpg      c:\icl\testimages\jpeg\jpeg_corrupt10.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_multiscan.jpg      c:\icl\testimages\jpeg\jpeg_multiscan.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_restartinterop.jpg c:\icl\testimages\jpeg\jpeg_restartinterop.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small01.jpg        c:\icl\testimages\jpeg\jpeg_small01.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small02.jpg        c:\icl\testimages\jpeg\jpeg_small02.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small03.jpg        c:\icl\testimages\jpeg\jpeg_small03.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small04.jpg        c:\icl\testimages\jpeg\jpeg_small04.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small05.jpg        c:\icl\testimages\jpeg\jpeg_small05.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small07.jpg        c:\icl\testimages\jpeg\jpeg_small07.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small09.jpg        c:\icl\testimages\jpeg\jpeg_small09.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small10.jpg        c:\icl\testimages\jpeg\jpeg_small10.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small11.jpg        c:\icl\testimages\jpeg\jpeg_small11.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small12.jpg        c:\icl\testimages\jpeg\jpeg_small12.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small13.jpg        c:\icl\testimages\jpeg\jpeg_small13.jpg
copy E:\inttest\icl\testimages\jpeg\jpeg_small14.jpg        c:\icl\testimages\jpeg\jpeg_small14.jpg

copy E:\inttest\icl\testimages\jfif444.jpg c:\ICL\TestImages\jfif444.jpg
copy E:\inttest\icl\refimages\jfif444.mbm c:\ICL\RefImages\jfif444.mbm

copy E:\inttest\icl\testimages\trans_8.gif c:\ICL\TestImages\trans_8.gif
copy E:\inttest\icl\refimages\trans_8.mbm c:\ICL\RefImages\trans_8.mbm

copy E:\inttest\icl\testimages\png4_8.png c:\ICL\TestImages\png4_8.png
copy E:\inttest\icl\refimages\png4_8.mbm c:\ICL\RefImages\png4_8.mbm

copy E:\inttest\icl\testimages\simple_b.mng c:\ICL\TestImages\simple_b.mng

copy E:\inttest\icl\testimages\jfif422_4_ref.JPG c:\ICL\TestImages\jfif422_4_ref.JPG

copy E:\inttest\icl\testimages\clock.gif c:\ICL\TestImages\clock.gif

copy E:\inttest\icl\testimages\mbm\bitmap_small01.mbm c:\ICL\TestImages\mbm\bitmap_small01.mbm

copy E:\tsu_icl_Exifimgdisp_01\auto_rot_1.jpeg  c:\tsu_icl_exifimgdisp_01\auto_rot_1.jpeg
copy E:\tsu_icl_Exifimgdisp_01\auto_rot_2.jpeg  c:\tsu_icl_exifimgdisp_01\auto_rot_2.jpeg
copy E:\tsu_icl_Exifimgdisp_01\auto_rot_3.jpeg  c:\tsu_icl_exifimgdisp_01\auto_rot_3.jpeg
copy E:\tsu_icl_Exifimgdisp_01\auto_rot_4.jpeg  c:\tsu_icl_exifimgdisp_01\auto_rot_4.jpeg
copy E:\tsu_icl_Exifimgdisp_01\auto_rot_5.jpeg  c:\tsu_icl_exifimgdisp_01\auto_rot_5.jpeg
copy E:\tsu_icl_Exifimgdisp_01\auto_rot_6.jpeg  c:\tsu_icl_exifimgdisp_01\auto_rot_6.jpeg
copy E:\tsu_icl_Exifimgdisp_01\auto_rot_7.jpeg  c:\tsu_icl_exifimgdisp_01\auto_rot_7.jpeg
copy E:\tsu_icl_Exifimgdisp_01\auto_rot_8.jpeg  c:\tsu_icl_exifimgdisp_01\auto_rot_8.jpeg

e:\rta\makecontent.bat

testframework -q \icl\imagedecoder\tsi_icl_imagedecoder_perf.script
move C:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_PERF.htm E:\Logs\TestResults\icl\imagedecoder\

testframework -q \icl\imagedecoder\tsi_icl_imagedecoder_crop.script
move C:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_CROP.htm E:\Logs\TestResults\icl\imagedecoder\

testframework -q \icl\imagedecoder\tsi_icl_imagedecoder_scale.script
move C:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_SCALE.htm E:\Logs\TestResults\icl\imagedecoder\

testframework -q \icl\imagedecoder\tsi_icl_imagedecoder_rotation.script
move C:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_ROTATION.htm E:\Logs\TestResults\icl\imagedecoder\

testframework -q \icl\imagedecoder\tsi_icl_imagedecoder_multipleop.script
move C:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_MULTIPLEOP.htm E:\Logs\TestResults\icl\imagedecoder\

testframework -q \icl\imagedecoder\tsi_icl_imagedecoder_extinterface.script
move C:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_EXTINTERFACE.htm E:\Logs\TestResults\icl\imagedecoder\

testframework -q \icl\imagedecoder\tsi_icl_imagedecoder_extinterface_panic.script
move C:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_EXTINTERFACE_PANIC.htm E:\Logs\TestResults\icl\imagedecoder\

testframework -q \icl\imagedecoder\tsi_icl_imagedecoder_autorotate.script
move C:\Logs\TestResults\icl\imagedecoder\TSI_ICL_IMAGEDECODER_AUTOROTATE.htm E:\Logs\TestResults\icl\imagedecoder\

---------------------------
Symbian OS Application Error
---------------------------
A call to User::Panic() has occured, indicating
a programming fault in the running application.
Please refer to the documentation for more details.

Program	testframework.exe::DoTestThread00000000
Error	KERN-EXEC : 3

Do you wish to Debug the error?
---------------------------
Yes   No   
---------------------------


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

runICLDecoderIntTestsStream.bat
