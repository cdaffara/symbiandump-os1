rem Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del E:\Logs\TestResults\TSI_MMF_VCLNTAVI_PLAY.*
del E:\Logs\TestResults\TSI_MMF_VCLNTAVI_REC.*
del E:\Logs\TestResults\TSI_MMF_VCLNTAVI_ALLOC.*
del E:\Logs\TestResults\TSI_MMF_VCLNTAVI_PAR_AUDIO.*

md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM ***************** VCLNTAVI TESTS **************
REM ***********************************************

md c:\vclntavi\

copy e:\vclntavi\video_avi.ini c:\vclntavi\video_avi.ini

copy e:\inttest\mmf\vclntavi\longvideo.avi c:\vclntavi\longvideo.avi
copy e:\inttest\mmf\vclntavi\shortvideo.avi c:\vclntavi\shortvideo.avi
copy e:\inttest\mmf\vclntavi\textinput.avi c:\vclntavi\textinput.avi
copy e:\inttest\mmf\vclntavi\textoutput.avi c:\vclntavi\textoutput.avi
copy e:\inttest\mmf\vclntavi\scbtest c:\vclntavi\scbtest
copy e:\inttest\mmf\vclntavi\xvidonly.avi c:\vclntavi\xvidonly.avi
copy e:\inttest\mmf\vclntavi\corrupt.avi c:\vclntavi\corrupt.avi
copy e:\inttest\mmf\vclntavi\invalidcodecavi.avi c:\vclntavi\invalidcodecavi.avi
copy e:\inttest\mmf\vclntavi\xvid_rec.avi c:\vclntavi\xvid_rec.avi
copy e:\inttest\mmf\vclntavi\aud_vid.avi c:\vclntavi\aud_vid.avi
copy e:\inttest\mmf\vclntavi\video.dummy c:\vclntavi\video.dummy

testframework tsi_mmf_vclntavi_play.script
testframework tsi_mmf_vclntavi_rec.script
testframework tsi_mmf_vclntavi_alloc.script
testframework tsi_mmf_vclntavi_par_audio.script
testframework tsi_mmf_vclntavi_dummy_controller.script
testframework tsi_mmf_vclntavi_play_priority.script
testframework tsi_mmf_vclntavi_rec_priority.script

move C:\Logs\TestResults\*.htm E:\Logs\TestResults\

del c:\vclntavi\*.*
