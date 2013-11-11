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

REM Batch file used to remove all built Multimedia components (i.e.
REM not test code).  Typically used after a new CBR environment has 
REM been installed.  Subsequent use of it (on the same envrionment)
REM will do nothing.

call removerel multimedia_ecam

call removerel multimedia_gifscaler

call removerel multimedia_icl

call removerel multimedia_imagedisplay

call removerel multimedia_imagetransform

call removerel multimedia_mdf

call removerel multimedia_common

call removerel multimedia_mmf_asr

call removerel multimedia_mmf_devasr

call removerel multimedia_mmf_devvideo

call removerel multimedia_mmf

call removerel multimedia_mvs_agents

call removerel multimedia_openmax

call removerel multimedia_tuner

REM Use the CBR command "ListComponents" to show all components.
REM Some of the components above no longer apply to M04344 builds and later, but they are required for earlier builds.
REM PREQ775 restructure of Multimedia components listed below (delivered into build M04344).
REM Appended to the end of the file to allow PRE and POST PREQ775 builds to work 
REM Components below apply to build M04344 (OS 9.4, 9.5, etc) and later. 

call removerel multimedia_ecam_framework
call removerel multimedia_ecam_plugins
call removerel multimedia_icl_plugin
call removerel multimedia_icl_capsadaptation
call removerel multimedia_mdf_devsound_api
call removerel multimedia_mdf_devsound_hwdev
call removerel multimedia_mdf_devsound_pluginsupport
call removerel multimedia_mdf_devsound_refbtplugin
call removerel multimedia_mdf_devsound_refplugin
call removerel multimedia_mdf_devvideo
call removerel multimedia_videohai
call removerel multimedia_mmf_char
call removerel multimedia_mmf_plugin
call removerel multimedia_mobiletv_dvbh_hai
call removerel multimedia_mvs_app
call removerel multimedia_testframework_source
call removerel multimedia_transmitter
call removerel multimedia_transmitter_plugin


call removerel multimedia_testtools_source
call removerel multimedia_unittest_dvbhhai
call removerel multimedia_unittest_ecam_source
call removerel multimedia_unittest_icl_source
call removerel multimedia_unittest_mdf_source
call removerel multimedia_unittest_mmf_source
call removerel multimedia_unittest_testframework_source
call removerel multimedia_unittest_tuner_source
call removerel multimedia_unittest_transmitter_source
call removerel multimedia_inttest_icl_source
call removerel multimedia_inttest_mmf_source
call removerel multimedia_inttest_mvs_source
call removerel multimedia_inttest_transmitter_source

REM A3F Components
call removerel multimedia_a3f_acl
call removerel multimedia_a3f_devsoundadaptationinfo
call removerel multimedia_a3f_serverstart
call removerel multimedia_a3f_refmmrc
call removerel multimedia_a3f_devsound
call removerel multimedia_a3f_acf
call removerel multimedia_a3f_traces
call removerel multimedia_a3f_refacladaptation
call removerel multimedia_inttest_a3f_devsound_source
REM MUF Components
call removerel multimedia_muf
call removerel metadata_parser_plugin
call removerel multimedia_inttest_muf_source
call removerel multimedia_unittest_muf_source

REM PREQ1509 OpenMAX IL Components
call removerel multimedia_pv_aac
call removerel multimedia_omx_il_api
call removerel multimedia_omx_il_core
call removerel multimedia_omx_il_comp_ref

call removerel multimedia_omx_il_comp_test
call removerel multimedia_il_api
call removerel multimedia_unittest_omx_il
call removerel multimedia_unittest_milif
call removerel multimedia_omx_il_core_loader_conf_suite
call removerel multimedia_il_api_pv
call removerel multimedia_omx_il_pv_comps 


call removerel multimedia_videorenderer

call removerel multimedia_mmrcfw

REM 3GP library
call removerel multimedia_3gplibrary
call removerel multimedia_unittest_3gplibrary_source

REM More 3G
call removerel multimedia_data_plane_nodes 
call removerel multimedia_mgtengine 
call removerel multimedia_mm-infras 
call removerel multimedia_mmhp_audio_client 
call removerel multimedia_mmhp_camera_client 
call removerel multimedia_mmhp_video_client
call removerel multimedia_omx_il_comp 
call removerel multimedia_openmax_il_translators 
