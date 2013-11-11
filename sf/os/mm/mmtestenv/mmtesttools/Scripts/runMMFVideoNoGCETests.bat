rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem ************** VIDEO PLAYER NON-GCE  TESTS **********
rem 
rem

md E:\Logs\
md E:\Logs\TestResults\
md c:\mm\
md c:\mm\mmf\
md c:\mm\mmf\testfiles\
md c:\mm\mmf\testfiles\vclnt\
md c:\mm\mmf\testfiles\avictrl\
md c:\vclntavi\

REM ================tsi_mmf_vclntavi_surf_noGCE.script===========================
del E:\Logs\TestResults\tsi_mmf_vclntavi_surf_noGCE.*
copy e:\unittest\mmf\vclnt\mm\xvidonly.avi c:\vclntavi\xvidonly.avi
testframework tsi_mmf_vclntavi_surf_noGCE.script
del c:\vclntavi\*.*
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

REM ================tsu_mmf_vclnt_u_01_gce_unavailable.script===========================
del E:\Logs\TestResults\tsu_mmf_vclnt_u_01_gce_unavailable.*
copy z:\TSU_MMF_VCLNT_01_gce_unavailable.ini c:\mm\TSU_MMF_VCLNT_01_gce_unavailable.ini
copy e:\inttest\mmf\vclntavi\* c:\vclntavi\
copy E:\unittest\mmf\vclnt\mm\clock.dummy c:\mm\mmf\testfiles\vclnt\clock.dummy
copy E:\unittest\mmf\vclnt\mm\smallv.wav c:\mm\mmf\testfiles\vclnt\smallv.wav
copy E:\unittest\mmf\vclnt\mm\LongVideo.dummy c:\mm\mmf\testfiles\vclnt\LongVideo.dummy
copy E:\unittest\mmf\vclnt\mm\ShortVideo.dummy c:\mm\mmf\testfiles\vclnt\ShortVideo.dummy
testframework tsu_mmf_vclnt_u_01_gce_unavailable.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\
del c:\vclntavi\*.*
del c:\mm\mmf\testfiles\vclnt\*.*

REM ================tsu_mmf_avictrl_gce_disable.script===========================
del E:\Logs\TestResults\tsu_mmf_avictrl_gce_disable.*
copy z:\tsu_mmf_avictrl_gce_disable.ini c:\tsu_mmf_avictrl_gce_disable.ini
copy E:\unittest\mmf\avictrl\aud_vid.avi  c:\mm\mmf\testfiles\avictrl\aud_vid.avi
copy E:\unittest\mmf\avictrl\video_only.avi  c:\mm\mmf\testfiles\avictrl\video_only.avi
copy E:\unittest\mmf\avictrl\invalid.avi  c:\mm\mmf\testfiles\avictrl\invalid.avi
copy E:\unittest\mmf\avictrl\low_mem_vdo_test.avi c:\mm\mmf\testfiles\avictrl\low_mem_vdo_test.avi
copy E:\unittest\mmf\avictrl\aud_vid_vprp.avi  c:\mm\mmf\testfiles\avictrl\aud_vid_vprp.avi
testframework tsu_mmf_avictrl_gce_disable.script
move C:\Logs\TestResults\*.htm E:\Logs\TestResults\
del c:\tsu_mmf_avictrl_gce_disable.ini
del c:\mm\mmf\testfiles\avictrl\*.*
