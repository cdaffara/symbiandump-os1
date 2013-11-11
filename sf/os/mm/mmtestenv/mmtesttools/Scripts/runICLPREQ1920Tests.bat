@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

@rem Delete ICL output images, if any to avoid test fail with -23, -26 error
del c:\ICL\outputimages\*.*

md E:\Logs\
md E:\Logs\TestExecute\
md E:\Logs\TestExecute\imgproc\
md E:\Logs\TestExecute\imgproc\inttest\

md c:\testdata\
md c:\testdata\caps
md c:\testdata\caps\input
md c:\testdata\configs

md c:\ICL\
md c:\ICL\TestImages\
md c:\ICL\RefImages\

copy E:\inttest\icl\testimages\jfif444.jpg c:\ICL\TestImages\jfif444.jpg
copy E:\inttest\icl\refimages\jfif444.mbm c:\ICL\RefImages\jfif444.mbm
copy E:\inttest\icl\testimages\trans_8.gif c:\ICL\TestImages\trans_8.gif
copy E:\inttest\icl\refimages\trans_8.mbm c:\ICL\RefImages\trans_8.mbm
copy E:\inttest\icl\testimages\png4_8.png c:\ICL\TestImages\png4_8.png
copy E:\inttest\icl\refimages\png4_8.mbm c:\ICL\RefImages\png4_8.mbm
copy E:\inttest\icl\testimages\simple_b.mng c:\ICL\TestImages\simple_b.mng
copy E:\inttest\icl\testimages\jfif422_4_ref.JPG c:\ICL\TestImages\jfif422_4_ref.JPG
copy E:\inttest\icl\testimages\clock.gif c:\ICL\TestImages\clock.gif

e:\rta\makecontent.bat

copy z:\testdata\configs\*.* c:\testdata\configs\*.*

copy e:\inttest\icl\testdata\caps\input\caps_input01_02MP* c:\testdata\caps\input\caps_input01_02MP*
copy e:\inttest\icl\testdata\caps\input\caps_input04* c:\testdata\caps\input\caps_input04*
copy e:\inttest\icl\testdata\caps\input\caps_input01_07MP* c:\testdata\caps\input\caps_input01_07MP*
copy e:\inttest\icl\testdata\caps\input\caps_input01_07MP_zoomin01_reffile* c:\testdata\caps\input\caps_input01_07MP_zoomin01_reffile*
copy e:\inttest\icl\testdata\caps\input\caps_input01_07MP_zoomout01_reffile* c:\testdata\caps\input\caps_input01_07MP_zoomout01_reffile*
copy e:\inttest\icl\testdata\caps\input\caps_input01_07MP_panBLtoTR01_reffile* c:\testdata\caps\input\caps_input01_07MP_panBLtoTR01_reffile*
copy e:\inttest\icl\testdata\caps\input\caps_input01_07MP_panTRtoBL01_reffile* c:\testdata\caps\input\caps_input01_07MP_panTRtoBL01_reffile*
copy e:\inttest\icl\testdata\caps\input\caps_input02_07MP* c:\testdata\caps\input\caps_input02_07MP*
copy e:\inttest\icl\testdata\caps\input\sunset_sml* c:\testdata\caps\input\sunset_sml*

testexecute z:\testdata\scripts\tsi_icl_caps_alloc_1_te.script
testexecute z:\testdata\scripts\tsi_icl_caps_alloc_2_te.script
del C:\testdata\caps\output\*.*
del C:\testdata\caps\input\*.*
del c:\ICL\TestImages\*.*
del c:\ICL\RefImages\*.*

copy E:\inttest\icl\testdata\caps\input\justrawtext.jpg c:\testdata\caps\input\justrawtext.jpg
copy E:\inttest\icl\testdata\caps\input\justrawtext.jpg c:\testdata\caps\input\justrawtext.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input02_5MP.jpg c:\testdata\caps\input\caps_input02_5MP.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP.jpg c:\testdata\caps\input\caps_input01_07MP.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input02_07MP.jpg c:\testdata\caps\input\caps_input02_07MP.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input04.mbm c:\testdata\caps\input\caps_input04.mbm
copy E:\inttest\icl\testdata\caps\input\drm_reference.jpg c:\testdata\caps\input\drm_reference.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input01_02MP.jpg c:\testdata\caps\input\caps_input01_02MP.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_zoomin01_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_zoomin01_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_zoomout01_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_zoomout01_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_panBLtoTR01_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_panBLtoTR01_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_panTRtoBL01_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_panTRtoBL01_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_zoomin03_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_zoomin03_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_zoomout03_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_zoomout03_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_panBLtoTR03_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_panBLtoTR03_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_panTRtoBL03_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_panTRtoBL03_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_zoomin05_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_zoomin05_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_zoomout05_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_zoomout05_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_panBLtoTR05_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_panBLtoTR05_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP_panTRtoBL05_reffile.mbm c:\testdata\caps\input\caps_input01_07MP_panTRtoBL05_reffile.mbm
copy E:\inttest\icl\testdata\caps\input\preview_skew.jpg c:\testdata\caps\input\preview_skew.jpg
copy E:\inttest\icl\testdata\caps\input\preview_skew_ref.mbm c:\testdata\caps\input\preview_skew_ref.mbm
copy E:\inttest\icl\testdata\caps\input\reference_01.jpg c:\testdata\caps\input\reference_01.jpg
copy E:\inttest\icl\testdata\caps\input\ref_operation00.jpg c:\testdata\caps\input\ref_operation00.jpg
copy E:\inttest\icl\testdata\caps\input\ref_operation01.jpg c:\testdata\caps\input\ref_operation01.jpg
copy E:\inttest\icl\testdata\caps\input\ref_operation02.jpg c:\testdata\caps\input\ref_operation02.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input03_10MP.jpg c:\testdata\caps\input\caps_input03_10MP.jpg
copy E:\inttest\icl\testdata\caps\input\ref_operation04.jpg c:\testdata\caps\input\ref_operation04.jpg
copy E:\inttest\icl\testdata\caps\input\ref_operation08.jpg c:\testdata\caps\input\ref_operation08.jpg
copy E:\inttest\icl\testdata\caps\input\ref_operation10.jpg c:\testdata\caps\input\ref_operation10.jpg
copy E:\inttest\icl\testdata\caps\input\ref_operation20.jpg c:\testdata\caps\input\ref_operation20.jpg
copy E:\inttest\icl\testdata\caps\input\ref_operation40.jpg c:\testdata\caps\input\ref_operation40.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input03_2MP.jpg c:\testdata\caps\input\caps_input03_2MP.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input02_5MP.jpg c:\testdata\caps\input\caps_input02_5MP.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input03_2MP_spmo.jpg c:\testdata\caps\input\caps_input03_2MP_spmo.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input03_10MP.jpg c:\testdata\caps\input\caps_input03_10MP.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input02_5MP_spmo.jpg c:\testdata\caps\input\caps_input02_5MP_spmo.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input03_10MP_spmo.jpg c:\testdata\caps\input\caps_input03_10MP_spmo.jpg
copy E:\inttest\icl\testdata\caps\input\sunset_sml.png c:\testdata\caps\input\sunset_sml.png
copy E:\inttest\icl\testdata\caps\input\caps_input04.mbm c:\testdata\caps\input\caps_input04.mbm
copy E:\inttest\icl\testdata\caps\input\ref_setrect0.jpg c:\testdata\caps\input\ref_setrect0.jpg
copy E:\inttest\icl\testdata\caps\input\sunset.bmp c:\testdata\caps\input\sunset.bmp
copy E:\inttest\icl\testdata\caps\input\sunset.png c:\testdata\caps\input\sunset.png
copy E:\inttest\icl\testdata\caps\input\sunset.gif c:\testdata\caps\input\sunset.gif
copy E:\inttest\icl\testdata\caps\input\ref_persistance* c:\testdata\caps\input\ref_persistance*
copy E:\inttest\icl\testdata\caps\input\sunset_sml.png c:\testdata\caps\input\sunset_sml.png
copy E:\inttest\icl\testdata\caps\input\geometric_reference* c:\testdata\caps\input\geometric_reference*

testexecute z:\testdata\scripts\tsi_icl_caps_imgproc01_te.script
del C:\testdata\caps\output\*.*
del C:\testdata\caps\input\*.*

copy e:\inttest\icl\testdata\caps\input\caps_input01_07MP* c:\testdata\caps\input\caps_input01_07MP*
copy e:\inttest\icl\testdata\caps\input\sunset* c:\testdata\caps\input\sunset*
copy e:\inttest\icl\testdata\caps\input\sunset* c:\testdata\caps\input\sunset*
copy e:\inttest\icl\testdata\caps\input\sunset.wbmp c:\testdata\caps\input\sunset.wbmp
copy e:\inttest\icl\testdata\caps\input\caps_input01_02MP* c:\testdata\caps\input\caps_input01_02MP*
copy e:\inttest\icl\testdata\caps\input\jpeg_multiscan* c:\testdata\caps\input\jpeg_multiscan*
copy e:\inttest\icl\testdata\caps\input\jpeg_restartinterop* c:\testdata\caps\input\jpeg_restartinterop*
copy e:\inttest\icl\testdata\caps\input\jpeg_progressive* c:\testdata\caps\input\jpeg_progressive*
copy e:\inttest\icl\testdata\caps\input\reference_01* c:\testdata\caps\input\reference_01*
copy e:\inttest\icl\testdata\caps\input\zerolengthfile* c:\testdata\caps\input\zerolengthfile*


testexecute z:\testdata\scripts\tsi_icl_caps_interop_te.script
del C:\testdata\caps\output\*.*
del C:\testdata\caps\input\*.*

copy e:\inttest\icl\testdata\caps\input\caps_input02_07MP* c:\testdata\caps\input\caps_input02_07MP*
copy e:\inttest\icl\testdata\caps\input\caps_input03_10MP* c:\testdata\caps\input\caps_input03_10MP*
copy e:\inttest\icl\testdata\caps\input\justrawtext* c:\testdata\caps\input\justrawtext*
copy e:\inttest\icl\testdata\caps\input\caps_input01_07MP* c:\testdata\caps\input\caps_input01_07MP*
copy e:\inttest\icl\testdata\caps\input\sunset.wbmp c:\testdata\caps\input\sunset.wbmp
copy e:\inttest\icl\testdata\caps\input\sunset* c:\testdata\caps\input\sunset*
copy e:\inttest\icl\testdata\caps\input\caps_input04* c:\testdata\caps\input\caps_input04*
copy e:\inttest\icl\testdata\caps\input\caps_input03_2MP* c:\testdata\caps\input\caps_input03_2MP*
copy e:\inttest\icl\testdata\caps\input\caps_input02_5MP* c:\testdata\caps\input\caps_input02_5MP*
copy e:\inttest\icl\testdata\caps\input\jpeg_multiscan* c:\testdata\caps\input\jpeg_multiscan*
copy e:\inttest\icl\testdata\caps\input\jpeg_restartinterop* c:\testdata\caps\input\jpeg_restartinterop*
copy e:\inttest\icl\testdata\caps\input\jpeg_progressive* c:\testdata\caps\input\jpeg_progressive*
copy e:\inttest\icl\testdata\caps\input\caps_input01_02MP* c:\testdata\caps\input\caps_input01_02MP*
copy e:\inttest\icl\testdata\caps\input\sunset.wbmp c:\testdata\caps\input\sunset.wbmp
copy e:\inttest\icl\testdata\caps\input\reference_01* c:\testdata\caps\input\reference_01*
copy e:\inttest\icl\testdata\caps\input\zerolengthfile* c:\testdata\caps\input\zerolengthfile*
copy e:\inttest\icl\testdata\caps\input\reference_01* c:\testdata\caps\input\reference_01*
copy e:\inttest\icl\temp* c:\temp*
copy e:\inttest\icl\temp.wbmp c:\temp.wbmp

testexecute z:\testdata\scripts\tsi_icl_caps_inttest01_te.script
del C:\testdata\caps\output\*.*
del C:\testdata\caps\input\*.*

copy E:\inttest\icl\testdata\caps\input\caps_input01_02MP.jpg  c:\testdata\caps\input\caps_input01_02MP.jpg
copy E:\inttest\icl\testdata\caps\input\jpeg_restartinterop.jpg  c:\testdata\caps\input\jpeg_restartinterop.jpg
copy E:\inttest\icl\testdata\caps\input\conehead.wrp c:\testdata\caps\input\conehead.wrp
copy E:\inttest\icl\testdata\caps\input\caps_input01_07MP.jpg  c:\testdata\caps\input\caps_input01_07MP.jpg
copy E:\inttest\icl\testdata\caps\input\border.PNG  c:\testdata\caps\input\border.PNG
copy E:\inttest\icl\testdata\caps\input\border.MBM  c:\testdata\caps\input\border.MBM
copy E:\inttest\icl\testdata\caps\input\reference_01.jpg  c:\testdata\caps\input\reference_01.jpg
copy E:\inttest\icl\testdata\caps\input\geometric_reference*  c:\testdata\caps\input\geometric_reference*
copy E:\inttest\icl\testdata\caps\input\stack_reference*  c:\testdata\caps\input\stack_reference*
copy E:\inttest\icl\testdata\caps\input\ref_operation00.jpg  c:\testdata\caps\input\ref_operation00.jpg
copy E:\inttest\icl\testdata\caps\input\befreset_reference0.jpg  c:\testdata\caps\input\befreset_reference0.jpg
copy E:\inttest\icl\testdata\caps\input\aftreset_reference0.jpg  c:\testdata\caps\input\aftreset_reference0.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input02_5MP.jpg  c:\testdata\caps\input\caps_input02_5MP.jpg
copy E:\inttest\icl\testdata\caps\input\caps_input02_07mp.jpg  c:\testdata\caps\input\caps_input02_07mp.jpg
copy E:\inttest\icl\testdata\caps\input\jpeg_progressive.jpg  c:\testdata\caps\input\jpeg_progressive.jpg

testexecute z:\testdata\scripts\tsi_icl_caps_photoart_te.script


move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\imgproc\inttest\
del C:\testdata\caps\output\*.*
del c:\testdata\caps\input\*.*

REM caps inttests finished

md E:\Logs\TestExecute\imgproc\unittest\

md c:\testdata\
md c:\testdata\images

copy E:\inttest\icl\testdata\images\*.*     c:\testdata\images\*.*
copy E:\inttest\icl\testdata\images\sunset_sml.png c:\testdata\caps\input\sunset_sml.png

testexecute z:\testdata\scripts\tsu_icl_capswrapper_te.script

del C:\testdata\images\*.*
del C:\testdata\caps\output\*.*
del C:\testdata\caps\input\*.*

REM caps unittests finished

move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\imgproc\unittest\


md E:\Logs\TestExecute\imgtrans\
md E:\Logs\TestExecute\imgtrans\inttest\

md c:\testdata\
md c:\testdata\images
md c:\testdata\testref

copy E:\inttest\icl\testdata\images\*.*     c:\testdata\images\*.*
copy E:\inttest\icl\testdata\testref\*.*      c:\testdata\testref\*.*

testexecute z:\testdata\scripts\tsi_icl_caps_jpgsqueeze_te.script
del C:\testdata\testoutput\*.*
testexecute z:\testdata\scripts\tsi_icl_caps_jpgoverlay_te.script
del C:\testdata\testoutput\*.*
testexecute z:\testdata\scripts\tsi_icl_caps_jpgrotate_te.script
del C:\testdata\testoutput\*.*
testexecute Z:\testdata\scripts\tsi_icl_caps_jpg_interop_te.script
del C:\testdata\testoutput\*.*
del c:\testdata\images\*.*
del c:\testdata\testref\*.*
move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\imgtrans\inttest\
REM DEF132243 - copy ony required testdata for the following 3 scripts
copy e:\inttest\icl\testdata\images\capstest.jpg c:\testdata\images\capstest.jpg
copy e:\inttest\icl\testdata\images\overlay.jpg c:\testdata\images\overlay.jpg
copy e:\inttest\icl\testdata\testref\overlayjpg_jpegref_filetofile.jpg c:\testdata\testref\overlayjpg_jpegref_filetofile.jpg
copy e:\inttest\icl\testdata\testref\overlay_jpegref_buffertobuffer.jpg c:\testdata\testref\overlay_jpegref_buffertobuffer.jpg
copy e:\inttest\icl\testdata\testref\overlay_jpegref_filetobuffer_ovlbuf.jpg c:\testdata\testref\overlay_jpegref_filetobuffer_ovlbuf.jpg
copy e:\inttest\icl\testdata\testref\overlay_jpegref_buffertofile_ovlfile.jpg c:\testdata\testref\overlay_jpegref_buffertofile_ovlfile.jpg
copy e:\inttest\icl\testdata\testref\overlay_jpegref_filetofile_ovlbuf.jpg c:\testdata\testref\overlay_jpegref_filetofile_ovlbuf.jpg
copy e:\inttest\icl\testdata\testref\overlay_jpegref_buffertobuffer_ovlfile.jpg c:\testdata\testref\overlay_jpegref_buffertobuffer_ovlfile.jpg
copy e:\inttest\icl\testdata\testref\overlay_jpegref_filetobuffer_ovlfile.jpg c:\testdata\testref\overlay_jpegref_filetobuffer_ovlfile.jpg
copy e:\inttest\icl\testdata\testref\overlay_jpegref_buffertofile_ovlbuf.jpg c:\testdata\testref\overlay_jpegref_buffertofile_ovlbuf.jpg
testexecute z:\testdata\scripts\tsi_icl_caps_jpg_overlay_alloc_te.script
del C:\testdata\testoutput\*.*
del c:\testdata\images\*.*
del c:\testdata\testref\*.*
copy e:\inttest\icl\testdata\images\datetree.jpg c:\testdata\images\datetree.jpg
copy e:\inttest\icl\testdata\testref\datetree_5ref_filetofile.jpg c:\testdata\testref\datetree_5ref_filetofile.jpg
copy e:\inttest\icl\testdata\testref\datetree_5ref_capsalloc_filetofile.jpg c:\testdata\testref\datetree_5ref_capsalloc_filetofile.jpg
copy e:\inttest\icl\testdata\images\jpeg_small03.jpg c:\testdata\images\jpeg_small03.jpg
copy e:\inttest\icl\testdata\testref\small03_20ref_filetofile.jpg c:\testdata\testref\small03_20ref_filetofile.jpg
copy e:\inttest\icl\testdata\images\jpeg_small13.jpg c:\testdata\images\jpeg_small13.jpg
copy e:\inttest\icl\testdata\testref\small13_40ref_buffertobuffer.jpg c:\testdata\testref\small13_40ref_buffertobuffer.jpg
copy e:\inttest\icl\testdata\testref\small03_60ref_filetofile.jpg c:\testdata\testref\small03_60ref_filetofile.jpg
copy e:\inttest\icl\testdata\testref\small13_40ref_buffertofile.jpg c:\testdata\testref\small13_40ref_buffertofile.jpg
copy e:\inttest\icl\testdata\testref\small13_40ref_filetobuffer.jpg c:\testdata\testref\small13_40ref_filetobuffer.jpg
testexecute z:\testdata\scripts\tsi_icl_caps_jpg_squeeze_alloc_te.script
del C:\testdata\testoutput\*.*
del c:\testdata\images\*.*
del c:\testdata\testref\*.*
copy e:\inttest\icl\testdata\testref\capstest_0deghflipref_filetofile.jpg c:\testdata\testref\capstest_0deghflipref_filetofile.jpg
copy e:\inttest\icl\testdata\testref\capstest_alloccaps_ref_filetofile.jpg c:\testdata\testref\capstest_alloccaps_ref_filetofile.jpg
copy e:\inttest\icl\testdata\testref\capstest_alloc_0deghflipref_filetofile.jpg c:\testdata\testref\capstest_alloc_0deghflipref_filetofile.jpg
copy e:\inttest\icl\testdata\testref\capstest_alloc_270ref_buffertobuffer.jpg c:\testdata\testref\capstest_alloc_270ref_buffertobuffer.jpg
copy e:\inttest\icl\testdata\testref\capstest_alloc_270ref_buffertofile.jpg c:\testdata\testref\capstest_alloc_270ref_buffertofile.jpg
copy e:\inttest\icl\testdata\images\capstest.jpg c:\testdata\images\capstest.jpg
copy e:\inttest\icl\testdata\testref\capstest_alloc_270ref_filetobuffer.jpg c:\testdata\testref\capstest_alloc_270ref_filetobuffer.jpg
testexecute z:\testdata\scripts\tsi_icl_caps_jpg_rotate_alloc_te.script
del C:\testdata\testoutput\*.*

move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\imgtrans\inttest\
del c:\testdata\images\*.*
del c:\testdata\testref\*.*


REM caps imagetrans tests finished


md E:\Logs\TestExecute\imagePan\
md E:\Logs\TestExecute\imagePan\inttest\

md c:\testdata\
md c:\testdata\images
md c:\testdata\images\panorama
md c:\testdata\testref
md c:\testdata\testref\panorama

copy E:\inttest\icl\testdata\images\panorama\upbottom1.mbm     c:\testdata\images\panorama\upbottom1.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom2.mbm     c:\testdata\images\panorama\upbottom2.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom3.mbm     c:\testdata\images\panorama\upbottom3.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom4.mbm     c:\testdata\images\panorama\upbottom4.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom5.mbm     c:\testdata\images\panorama\upbottom5.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom6.mbm     c:\testdata\images\panorama\upbottom6.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom7.mbm     c:\testdata\images\panorama\upbottom7.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom8.mbm     c:\testdata\images\panorama\upbottom8.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom.mbm     c:\testdata\images\panorama\upbottom.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom1.jpg     c:\testdata\images\panorama\upbottom1.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom2.jpg     c:\testdata\images\panorama\upbottom2.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom3.jpg     c:\testdata\images\panorama\upbottom3.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom4.jpg     c:\testdata\images\panorama\upbottom4.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom5.jpg     c:\testdata\images\panorama\upbottom5.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom6.jpg     c:\testdata\images\panorama\upbottom6.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom7.jpg     c:\testdata\images\panorama\upbottom7.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom8.jpg     c:\testdata\images\panorama\upbottom8.jpg

copy E:\inttest\icl\testdata\testref\panorama\bottomup_panorama_stitch_ref.jpg     c:\testdata\testref\panorama\bottomup_panorama_stitch_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\upbottom_panorama_stitch_ref.jpg     c:\testdata\testref\panorama\upbottom_panorama_stitch_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\upbottom_bitmap_panorama_stitch_ref.mbm     c:\testdata\testref\panorama\upbottom_bitmap_panorama_stitch_ref.mbm
copy E:\inttest\icl\testdata\testref\panorama\upbottom_imageframe_panorama_stitch_ref.mbm     c:\testdata\testref\panorama\upbottom_imageframe_panorama_stitch_ref.mbm
copy E:\inttest\icl\testdata\testref\panorama\upbottom_bitmap_panorama_stitch_aspectratio_ref.mbm     c:\testdata\testref\panorama\upbottom_bitmap_panorama_stitch_aspectratio_ref.mbm
copy E:\inttest\icl\testdata\testref\panorama\upbottom_imageframe_panorama_stitch_aspectratio_ref.mbm     c:\testdata\testref\panorama\upbottom_imageframe_panorama_stitch_aspectratio_ref.mbm

testexecute z:\testdata\scripts\tsi_icl_imagepanorama_2_te.script
move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\imagePan\inttest\
del c:\testdata\images\panorama\*.*
del c:\testdata\testref\*.*
del c:\testdata\testref\panorama\*.*
del c:\testdata\testoutput\*.*

md c:\testdata\
md c:\testdata\images
md c:\testdata\images\panorama
md c:\testdata\testref
md c:\testdata\testref\panorama

copy E:\inttest\icl\testdata\images\panorama\viewfinder0.mbm     c:\testdata\images\panorama\viewfinder0.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder10.mbm     c:\testdata\images\panorama\viewfinder10.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder20.mbm     c:\testdata\images\panorama\viewfinder20.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder30.mbm     c:\testdata\images\panorama\viewfinder30.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder40.mbm     c:\testdata\images\panorama\viewfinder40.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder50.mbm     c:\testdata\images\panorama\viewfinder50.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder60.mbm     c:\testdata\images\panorama\viewfinder60.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder70.mbm     c:\testdata\images\panorama\viewfinder70.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder80.mbm     c:\testdata\images\panorama\viewfinder80.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder90.mbm     c:\testdata\images\panorama\viewfinder90.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder100.mbm     c:\testdata\images\panorama\viewfinder100.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder110.mbm     c:\testdata\images\panorama\viewfinder110.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder120.mbm     c:\testdata\images\panorama\viewfinder120.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder130.mbm     c:\testdata\images\panorama\viewfinder130.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder140.mbm     c:\testdata\images\panorama\viewfinder140.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder150.mbm     c:\testdata\images\panorama\viewfinder150.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder160.mbm     c:\testdata\images\panorama\viewfinder160.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder170.mbm     c:\testdata\images\panorama\viewfinder170.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder180.mbm     c:\testdata\images\panorama\viewfinder180.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder190.mbm     c:\testdata\images\panorama\viewfinder190.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder200.mbm     c:\testdata\images\panorama\viewfinder200.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder210.mbm     c:\testdata\images\panorama\viewfinder210.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder220.mbm     c:\testdata\images\panorama\viewfinder220.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder0.jpg     c:\testdata\images\panorama\viewfinder0.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder10.jpg     c:\testdata\images\panorama\viewfinder10.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder20.jpg     c:\testdata\images\panorama\viewfinder20.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder30.jpg     c:\testdata\images\panorama\viewfinder30.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder40.jpg     c:\testdata\images\panorama\viewfinder40.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder50.jpg     c:\testdata\images\panorama\viewfinder50.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder60.jpg     c:\testdata\images\panorama\viewfinder60.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder70.jpg     c:\testdata\images\panorama\viewfinder70.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder80.jpg     c:\testdata\images\panorama\viewfinder80.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder90.jpg     c:\testdata\images\panorama\viewfinder90.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder100.jpg     c:\testdata\images\panorama\viewfinder100.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder110.jpg     c:\testdata\images\panorama\viewfinder110.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder120.jpg     c:\testdata\images\panorama\viewfinder120.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder130.jpg     c:\testdata\images\panorama\viewfinder130.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder140.jpg     c:\testdata\images\panorama\viewfinder140.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder150.jpg     c:\testdata\images\panorama\viewfinder150.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder160.jpg     c:\testdata\images\panorama\viewfinder160.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder170.jpg     c:\testdata\images\panorama\viewfinder170.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder180.jpg     c:\testdata\images\panorama\viewfinder180.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder190.jpg     c:\testdata\images\panorama\viewfinder190.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder200.jpg     c:\testdata\images\panorama\viewfinder200.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder210.jpg     c:\testdata\images\panorama\viewfinder210.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder220.jpg     c:\testdata\images\panorama\viewfinder220.jpg
copy E:\inttest\icl\testdata\testref\panorama\sydney_panorama_stitch_lr_ref.jpg     c:\testdata\testref\panorama\sydney_panorama_stitch_lr_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\sydney_panorama_stitch_rl_ref.jpg     c:\testdata\testref\panorama\sydney_panorama_stitch_rl_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\sydney_panorama_stitch_aspectratio_lr_ref.jpg     c:\testdata\testref\panorama\sydney_panorama_stitch_aspectratio_lr_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\sydney_panorama_stitch_aspectratio_rl_ref.jpg     c:\testdata\testref\panorama\sydney_panorama_stitch_aspectratio_rl_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\sydney_panorama_filehandle_stitch_lr_ref.jpg     c:\testdata\testref\panorama\sydney_panorama_filehandle_stitch_lr_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\sydney_panorama_filehandle_aspectratio_stitch_lr_ref.jpg     c:\testdata\testref\panorama\sydney_panorama_filehandle_aspectratio_stitch_lr_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\sydney_basic_stitch_direction_viewTrack_lr_ref.jpg     c:\testdata\testref\panorama\sydney_basic_stitch_direction_viewTrack_lr_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\sydney_panorama_overlap_stitch_lr_ref.jpg     c:\testdata\testref\panorama\sydney_panorama_overlap_stitch_lr_ref.jpg

testexecute z:\testdata\scripts\tsi_icl_imagepanorama_1_te.script
move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\imagePan\inttest\
del c:\testdata\images\panorama\*.*
del c:\testdata\testref\panorama\*.*
del c:\testdata\testref\*.*
del c:\testdata\testoutput\*.*

md c:\testdata\
md c:\testdata\images
md c:\testdata\images\panorama
md c:\testdata\testref
md c:\testdata\testref\panorama

copy E:\inttest\icl\testdata\images\panorama\viewfinder0.mbm     c:\testdata\images\panorama\viewfinder0.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder10.mbm     c:\testdata\images\panorama\viewfinder10.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder20.mbm     c:\testdata\images\panorama\viewfinder20.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder30.mbm     c:\testdata\images\panorama\viewfinder30.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder40.mbm     c:\testdata\images\panorama\viewfinder40.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder50.mbm     c:\testdata\images\panorama\viewfinder50.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder60.mbm     c:\testdata\images\panorama\viewfinder60.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder70.mbm     c:\testdata\images\panorama\viewfinder70.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder80.mbm     c:\testdata\images\panorama\viewfinder80.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder90.mbm     c:\testdata\images\panorama\viewfinder90.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder100.mbm     c:\testdata\images\panorama\viewfinder100.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder110.mbm     c:\testdata\images\panorama\viewfinder110.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder120.mbm     c:\testdata\images\panorama\viewfinder120.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder130.mbm     c:\testdata\images\panorama\viewfinder130.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder140.mbm     c:\testdata\images\panorama\viewfinder140.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder150.mbm     c:\testdata\images\panorama\viewfinder150.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder160.mbm     c:\testdata\images\panorama\viewfinder160.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder170.mbm     c:\testdata\images\panorama\viewfinder170.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder180.mbm     c:\testdata\images\panorama\viewfinder180.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder190.mbm     c:\testdata\images\panorama\viewfinder190.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder200.mbm     c:\testdata\images\panorama\viewfinder200.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder210.mbm     c:\testdata\images\panorama\viewfinder210.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder220.mbm     c:\testdata\images\panorama\viewfinder220.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder0.jpg     c:\testdata\images\panorama\viewfinder0.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder10.jpg     c:\testdata\images\panorama\viewfinder10.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder20.jpg     c:\testdata\images\panorama\viewfinder20.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder30.jpg     c:\testdata\images\panorama\viewfinder30.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder40.jpg     c:\testdata\images\panorama\viewfinder40.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder50.jpg     c:\testdata\images\panorama\viewfinder50.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder60.jpg     c:\testdata\images\panorama\viewfinder60.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder70.jpg     c:\testdata\images\panorama\viewfinder70.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder80.jpg     c:\testdata\images\panorama\viewfinder80.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder90.jpg     c:\testdata\images\panorama\viewfinder90.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder100.jpg     c:\testdata\images\panorama\viewfinder100.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder110.jpg     c:\testdata\images\panorama\viewfinder110.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder120.jpg     c:\testdata\images\panorama\viewfinder120.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder130.jpg     c:\testdata\images\panorama\viewfinder130.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder140.jpg     c:\testdata\images\panorama\viewfinder140.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder150.jpg     c:\testdata\images\panorama\viewfinder150.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder160.jpg     c:\testdata\images\panorama\viewfinder160.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder170.jpg     c:\testdata\images\panorama\viewfinder170.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder180.jpg     c:\testdata\images\panorama\viewfinder180.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder190.jpg     c:\testdata\images\panorama\viewfinder190.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder200.jpg     c:\testdata\images\panorama\viewfinder200.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder210.jpg     c:\testdata\images\panorama\viewfinder210.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder220.jpg     c:\testdata\images\panorama\viewfinder220.jpg
REM copying reference images for panoramic stitch with effect and preview tests
copy E:\inttest\icl\testdata\testref\panorama\panorama_preview_noeffects.mbm  c:\testdata\testref\panorama\panorama_preview_noeffects.mbm
copy E:\inttest\icl\testdata\testref\panorama\panorama_preview_effects.mbm  c:\testdata\testref\panorama\panorama_preview_effects.mbm
copy E:\inttest\icl\testdata\testref\panorama\panorama_process_noeffects.jpg  c:\testdata\testref\panorama\panorama_process_noeffects.jpg
copy E:\inttest\icl\testdata\testref\panorama\panorama_process_effects.jpg  c:\testdata\testref\panorama\panorama_process_effects.jpg
copy E:\inttest\icl\testdata\testref\panorama\sydney_panorama_stitch_lr_ref.jpg  c:\testdata\testref\panorama\sydney_panorama_stitch_lr_ref.jpg
copy E:\inttest\icl\testdata\testref\panorama\viewfinder_witheffect_0027.jpg c:\testdata\testref\panorama\viewfinder_witheffect_0027.jpg


testexecute z:\testdata\scripts\tsi_icl_imagepanorama_3_te.script
move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\imagePan\inttest\
del c:\testdata\images\panorama\*.*
del c:\testdata\testref\panorama\*.*
del c:\testdata\testref\*.*
del c:\testdata\testoutput\*.*


md c:\testdata\
md c:\testdata\images
md c:\testdata\images\panorama
md c:\testdata\testref
md c:\testdata\testref\panorama

copy E:\inttest\icl\testdata\images\panorama\viewfinder0.mbm     c:\testdata\images\panorama\viewfinder0.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder10.mbm     c:\testdata\images\panorama\viewfinder10.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder20.mbm     c:\testdata\images\panorama\viewfinder20.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder30.mbm     c:\testdata\images\panorama\viewfinder30.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder40.mbm     c:\testdata\images\panorama\viewfinder40.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder50.mbm     c:\testdata\images\panorama\viewfinder50.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder60.mbm     c:\testdata\images\panorama\viewfinder60.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder70.mbm     c:\testdata\images\panorama\viewfinder70.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder80.mbm     c:\testdata\images\panorama\viewfinder80.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder90.mbm     c:\testdata\images\panorama\viewfinder90.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder100.mbm     c:\testdata\images\panorama\viewfinder100.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder110.mbm     c:\testdata\images\panorama\viewfinder110.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder120.mbm     c:\testdata\images\panorama\viewfinder120.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder130.mbm     c:\testdata\images\panorama\viewfinder130.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder140.mbm     c:\testdata\images\panorama\viewfinder140.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder150.mbm     c:\testdata\images\panorama\viewfinder150.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder160.mbm     c:\testdata\images\panorama\viewfinder160.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder170.mbm     c:\testdata\images\panorama\viewfinder170.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder180.mbm     c:\testdata\images\panorama\viewfinder180.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder190.mbm     c:\testdata\images\panorama\viewfinder190.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder200.mbm     c:\testdata\images\panorama\viewfinder200.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder210.mbm     c:\testdata\images\panorama\viewfinder210.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder220.mbm     c:\testdata\images\panorama\viewfinder220.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc0.mbm     c:\testdata\images\panorama\viewfinderalloc0.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc10.mbm     c:\testdata\images\panorama\viewfinderalloc10.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc20.mbm     c:\testdata\images\panorama\viewfinderalloc20.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc30.mbm     c:\testdata\images\panorama\viewfinderalloc30.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc40.mbm     c:\testdata\images\panorama\viewfinderalloc40.mbm
copy E:\inttest\icl\testdata\images\panorama\viewfinder0.jpg     c:\testdata\images\panorama\viewfinder0.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder10.jpg     c:\testdata\images\panorama\viewfinder10.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder20.jpg     c:\testdata\images\panorama\viewfinder20.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder30.jpg     c:\testdata\images\panorama\viewfinder30.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder40.jpg     c:\testdata\images\panorama\viewfinder40.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder50.jpg     c:\testdata\images\panorama\viewfinder50.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder60.jpg     c:\testdata\images\panorama\viewfinder60.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder70.jpg     c:\testdata\images\panorama\viewfinder70.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder80.jpg     c:\testdata\images\panorama\viewfinder80.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder90.jpg     c:\testdata\images\panorama\viewfinder90.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder100.jpg     c:\testdata\images\panorama\viewfinder100.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder110.jpg     c:\testdata\images\panorama\viewfinder110.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder120.jpg     c:\testdata\images\panorama\viewfinder120.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder130.jpg     c:\testdata\images\panorama\viewfinder130.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder140.jpg     c:\testdata\images\panorama\viewfinder140.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder150.jpg     c:\testdata\images\panorama\viewfinder150.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder160.jpg     c:\testdata\images\panorama\viewfinder160.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder170.jpg     c:\testdata\images\panorama\viewfinder170.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder180.jpg     c:\testdata\images\panorama\viewfinder180.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder190.jpg     c:\testdata\images\panorama\viewfinder190.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder200.jpg     c:\testdata\images\panorama\viewfinder200.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder210.jpg     c:\testdata\images\panorama\viewfinder210.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinder220.jpg     c:\testdata\images\panorama\viewfinder220.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc0.jpg     c:\testdata\images\panorama\viewfinderalloc0.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc10.jpg     c:\testdata\images\panorama\viewfinderalloc10.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc20.jpg     c:\testdata\images\panorama\viewfinderalloc20.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc30.jpg     c:\testdata\images\panorama\viewfinderalloc30.jpg
copy E:\inttest\icl\testdata\images\panorama\viewfinderalloc40.jpg     c:\testdata\images\panorama\viewfinderalloc40.jpg

testexecute z:\testdata\scripts\tsi_icl_imagepanorama_alloc_1_te.script
move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\imagePan\inttest\
del c:\testdata\images\panorama\*.*
del c:\testdata\testref\panorama\*.*
del c:\testdata\testref\*.*
del c:\testdata\testoutput\*.*

md c:\testdata\
md c:\testdata\images
md c:\testdata\images\panorama
md c:\testdata\testref
md c:\testdata\testref\panorama

copy E:\inttest\icl\testdata\images\panorama\upbottom1.mbm     c:\testdata\images\panorama\upbottom1.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom2.mbm     c:\testdata\images\panorama\upbottom2.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom3.mbm     c:\testdata\images\panorama\upbottom3.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom4.mbm     c:\testdata\images\panorama\upbottom4.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom5.mbm     c:\testdata\images\panorama\upbottom5.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom6.mbm     c:\testdata\images\panorama\upbottom6.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom7.mbm     c:\testdata\images\panorama\upbottom7.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom8.mbm     c:\testdata\images\panorama\upbottom8.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom.mbm     c:\testdata\images\panorama\upbottom.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc1.mbm     c:\testdata\images\panorama\upbottomalloc1.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc2.mbm     c:\testdata\images\panorama\upbottomalloc2.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc3.mbm     c:\testdata\images\panorama\upbottomalloc3.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc4.mbm     c:\testdata\images\panorama\upbottomalloc4.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc5.mbm     c:\testdata\images\panorama\upbottomalloc5.mbm
copy E:\inttest\icl\testdata\images\panorama\upbottom1.jpg     c:\testdata\images\panorama\upbottom1.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom2.jpg     c:\testdata\images\panorama\upbottom2.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom3.jpg     c:\testdata\images\panorama\upbottom3.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom4.jpg     c:\testdata\images\panorama\upbottom4.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom5.jpg     c:\testdata\images\panorama\upbottom5.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom6.jpg     c:\testdata\images\panorama\upbottom6.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom7.jpg     c:\testdata\images\panorama\upbottom7.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottom8.jpg     c:\testdata\images\panorama\upbottom8.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc1.jpg     c:\testdata\images\panorama\upbottomalloc1.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc2.jpg     c:\testdata\images\panorama\upbottomalloc2.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc3.jpg     c:\testdata\images\panorama\upbottomalloc3.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc4.jpg     c:\testdata\images\panorama\upbottomalloc4.jpg
copy E:\inttest\icl\testdata\images\panorama\upbottomalloc5.jpg     c:\testdata\images\panorama\upbottomalloc5.jpg

testexecute z:\testdata\scripts\tsi_icl_imagepanorama_alloc_2_te.script
move C:\Logs\TestExecute\*.htm E:\Logs\TestExecute\imagePan\inttest\
del c:\testdata\images\panorama\*.*
del c:\testdata\testref\*.*
del c:\testdata\testref\panorama\*.*
del c:\testdata\testoutput\*.*

REM caps imagePan tests finished


