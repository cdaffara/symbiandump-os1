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
rem

REM Remove old results from MMC
del e:\logs\testexecute\Te_tsi_icl_cod_5suite_1.htm
del e:\logs\testexecute\Te_tsi_icl_cod_5suite_2.htm
del e:\logs\testexecute\Te_tsi_icl_cod_5suite_3.htm
del e:\logs\testexecute\Te_tsi_icl_cod_5asuite.htm
del e:\logs\testexecute\te_tsi_icl_cod_5suite_mng_1.htm
del e:\logs\testexecute\te_tsi_icl_cod_5suite_mng_2.htm
md e:\logs
md e:\logs\testexecute

REM Create destination directories for test images
md c:\Te_tsi_icl_cod_05
md c:\Te_tsi_icl_cod_05\bmp
md c:\Te_tsi_icl_cod_05\bmp\ref
md c:\Te_tsi_icl_cod_05\gif
md c:\Te_tsi_icl_cod_05\gif\ref
md c:\Te_tsi_icl_cod_05\ico
md c:\Te_tsi_icl_cod_05\ico\ref
md c:\Te_tsi_icl_cod_05\jpeg
md c:\Te_tsi_icl_cod_05\jpeg\ref
md c:\Te_tsi_icl_cod_05\mbm
md c:\Te_tsi_icl_cod_05\mbm\ref
md c:\Te_tsi_icl_cod_05\mng
md c:\Te_tsi_icl_cod_05\mng\ref
md c:\Te_tsi_icl_cod_05\ota
md c:\Te_tsi_icl_cod_05\ota\ref
md c:\Te_tsi_icl_cod_05\png
md c:\Te_tsi_icl_cod_05\png\ref
md c:\Te_tsi_icl_cod_05\tif
md c:\Te_tsi_icl_cod_05\tif\ref
md c:\Te_tsi_icl_cod_05\wbmp
md c:\Te_tsi_icl_cod_05\wbmp\ref
md c:\Te_tsi_icl_cod_05\wmf
md c:\Te_tsi_icl_cod_05\wmf\ref

REM Copy test images from MMC to ROM
copy e:\te_tsi_icl_cod_05\png\bgai4a08* c:\te_tsi_icl_cod_05\png\bgai4a08*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a08_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a08_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a08_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a08_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a08_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a08_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a08_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a08_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a08_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a08_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a08_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a08_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\bgai4a16* c:\te_tsi_icl_cod_05\png\bgai4a16*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a16_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a16_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a16_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a16_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a16_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a16_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a16_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a16_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a16_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a16_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a16_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a16_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgai4a16_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgai4a16_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\bgan6a08* c:\te_tsi_icl_cod_05\png\bgan6a08*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_16map_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_16map_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_16map_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_16map_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\bgan6a16* c:\te_tsi_icl_cod_05\png\bgan6a16*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a16_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a16_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a16_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a16_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a16_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a16_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a16_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a16_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a16_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a16_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a16_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a16_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a16_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a16_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\bgbn4a08* c:\te_tsi_icl_cod_05\png\bgbn4a08*
copy e:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgbn4a08_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgbn4a08_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgbn4a08_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\bggn4a16* c:\te_tsi_icl_cod_05\png\bggn4a16*
copy e:\te_tsi_icl_cod_05\png\ref\bggn4a16_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bggn4a16_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bggn4a16_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bggn4a16_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bggn4a16_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bggn4a16_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bggn4a16_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bggn4a16_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bggn4a16_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bggn4a16_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bggn4a16_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bggn4a16_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bggn4a16_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bggn4a16_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\bgwn6a08* c:\te_tsi_icl_cod_05\png\bgwn6a08*
copy e:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16map_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16map_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16map_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16map_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgwn6a08_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgwn6a08_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgwn6a08_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\bgyn6a16* c:\te_tsi_icl_cod_05\png\bgyn6a16*
copy e:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgyn6a16_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\bgyn6a16_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgyn6a16_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\tbbn1g04* c:\te_tsi_icl_cod_05\png\tbbn1g04*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn1g04_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn1g04_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn1g04_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\tbbn2c16* c:\te_tsi_icl_cod_05\png\tbbn2c16*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn2c16_16m_mask_decode*  
copy e:\te_tsi_icl_cod_05\png\ref\tbbn2c16_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn2c16_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\tbbn3p08* c:\te_tsi_icl_cod_05\png\tbbn3p08*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn3p08_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tbbn3p08_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tbbn3p08_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\tp1n3p08* c:\te_tsi_icl_cod_05\png\tp1n3p08*
copy e:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tp1n3p08_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\tp1n3p08_mask_mask_decode* c:\te_tsi_icl_cod_05\png\ref\tp1n3p08_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\png\basn0g01* c:\te_tsi_icl_cod_05\png\basn0g01*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g01_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g01_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g01_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g01_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g01_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g01_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g01_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g01_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g01_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g01_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g01_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g01_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn0g02* c:\te_tsi_icl_cod_05\png\basn0g02*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g02_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g02_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g02_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g02_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g02_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g02_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g02_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g02_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g02_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g02_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g02_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g02_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn0g04* c:\te_tsi_icl_cod_05\png\basn0g04*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g04_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g04_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g04_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g04_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g04_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g04_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g04_16ma_mask_decode*  c:\te_tsi_icl_cod_05\png\ref\basn0g04_16ma_mask_decode* 
copy e:\te_tsi_icl_cod_05\png\ref\basn0g04_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g04_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g04_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g04_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn0g08* c:\te_tsi_icl_cod_05\png\basn0g08*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g08_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g08_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g08_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g08_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g08_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g08_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g08_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g08_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g08_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g08_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn0g16* c:\te_tsi_icl_cod_05\png\basn0g16*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g16_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g16_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g16_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g16_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g16_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g16_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g16_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g16_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g16_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g16_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn0g16_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn0g16_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn2c08* c:\te_tsi_icl_cod_05\png\basn2c08*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c08_16map_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c08_16map_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c08_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c08_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c08_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c08_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c08_16map_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c08_16map_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c08_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c08_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c08_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c08_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c08_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c08_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn2c16* c:\te_tsi_icl_cod_05\png\basn2c16*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c16_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c16_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c16_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c16_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c16_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c16_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c16_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c16_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c16_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c16_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn2c16_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn2c16_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn3p01* c:\te_tsi_icl_cod_05\png\basn3p01*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p01_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p01_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p01_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p01_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p01_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p01_16m_no_mask_decode*


REM Run the tests
testexecute z:\testdata\scripts\Te_tsi_icl_cod_5suite_1.script

REM Move the results file from the ROM to the MMC
move c:\logs\testexecute\Te_tsi_icl_cod_5suite_1.htm e:\logs\testexecute\Te_tsi_icl_cod_5suite_1.htm


REM Delete the test images
del c:\Te_tsi_icl_cod_05\mbm\ref\*.*
del c:\Te_tsi_icl_cod_05\mbm\*.*
del c:\Te_tsi_icl_cod_05\png\ref\*.*
del c:\Te_tsi_icl_cod_05\png\*.*

REM Copy test images from MMC to ROM
copy e:\te_tsi_icl_cod_05\png\basn3p01* c:\te_tsi_icl_cod_05\png\basn3p01*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p01_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p01_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p01_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p01_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p01_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p01_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn3p02* c:\te_tsi_icl_cod_05\png\basn3p02*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p02_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p02_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p02_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p02_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p02_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p02_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p02_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p02_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p02_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p02_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p02_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p02_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn3p04* c:\te_tsi_icl_cod_05\png\basn3p04*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p04_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p04_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p04_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p04_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p04_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p04_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p04_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p04_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p04_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p04_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p04_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p04_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\png\basn3p08* c:\te_tsi_icl_cod_05\png\basn3p08*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p08_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p08_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p08_16m_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p08_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\png\basn3p08* c:\te_tsi_icl_cod_05\png\basn3p08*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p08_16ma_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p08_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p08_16mu_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p08_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\basn3p08_16m_mask_decode* c:\te_tsi_icl_cod_05\png\ref\basn3p08_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask* c:\te_tsi_icl_cod_05\png\ref\rgbonly_8bit_mask*
copy e:\te_tsi_icl_cod_05\mbm\16masrc_to_16madest_uncompressed* c:\te_tsi_icl_cod_05\mbm\16masrc_to_16madest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16masrc_to_16madest_uncompressed* c:\te_tsi_icl_cod_05\mbm\ref\16masrc_to_16madest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16masrc_to_16mu_dest_uncompressed* c:\te_tsi_icl_cod_05\mbm\ref\16masrc_to_16mu_dest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16masrc_to16mdest_uncompressed* c:\te_tsi_icl_cod_05\mbm\ref\16masrc_to16mdest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\16musrc_to_16mudest_uncompressed* c:\te_tsi_icl_cod_05\mbm\16musrc_to_16mudest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16musrc_to_16madest_uncompressed* c:\te_tsi_icl_cod_05\mbm\ref\16musrc_to_16madest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16musrc_to_16mudest_uncompressed* c:\te_tsi_icl_cod_05\mbm\ref\16musrc_to_16mudest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16musrc_to_16mdest_uncompressed* c:\te_tsi_icl_cod_05\mbm\ref\16musrc_to_16mdest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\16msrc_to_16mdest_uncompressed* c:\te_tsi_icl_cod_05\mbm\16msrc_to_16mdest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16msrc_to_16madest_uncompressed* c:\te_tsi_icl_cod_05\mbm\ref\16msrc_to_16madest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16msrc_to_16mudest_uncompressed* c:\te_tsi_icl_cod_05\mbm\ref\16msrc_to_16mudest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16msrc_to_16mdest_uncompressed*  c:\te_tsi_icl_cod_05\mbm\ref\16msrc_to_16mdest_uncompressed*
copy e:\te_tsi_icl_cod_05\jpeg\jpeg_sequential* c:\te_tsi_icl_cod_05\jpeg\jpeg_sequential*
copy e:\te_tsi_icl_cod_05\jpeg\ref\sequential_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\jpeg\ref\sequential_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\jpeg\ref\sequential_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\jpeg\ref\sequential_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\jpeg\ref\sequential_16m_no_mask_decode* c:\te_tsi_icl_cod_05\jpeg\ref\sequential_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\jpeg\jpeg_progressive* c:\te_tsi_icl_cod_05\jpeg\jpeg_progressive*
copy e:\te_tsi_icl_cod_05\jpeg\ref\progressive_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\jpeg\ref\progressive_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\jpeg\ref\progressive_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\jpeg\ref\progressive_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\jpeg\ref\progressive_16m_no_mask_decode* c:\te_tsi_icl_cod_05\jpeg\ref\progressive_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\animated_interlaced* c:\te_tsi_icl_cod_05\gif\animated_interlaced*
copy e:\te_tsi_icl_cod_05\gif\ref\animated_interlaced_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\animated_interlaced_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\animated_interlaced_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\animated_interlaced_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\animated_interlaced_16m_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\animated_interlaced_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\animated_transparency* c:\te_tsi_icl_cod_05\gif\animated_transparency*
copy e:\te_tsi_icl_cod_05\gif\ref\animated_transparency_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\animated_transparency_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\animated_transparency_16ma_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\animated_transparency_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\animated_transparency_16mu_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\animated_transparency_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\animated_transparency_16m_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\animated_transparency_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\clock* c:\te_tsi_icl_cod_05\gif\clock*
copy e:\te_tsi_icl_cod_05\gif\ref\clock_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\clock_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\clock_16ma_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\clock_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\clock_16mu_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\clock_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\clock_16m_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\clock_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\gifcomment* c:\te_tsi_icl_cod_05\gif\gifcomment*
copy e:\te_tsi_icl_cod_05\gif\ref\gifcomment_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\gifcomment_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\gifcomment_16ma_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\gifcomment_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\gifcomment_16mu_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\gifcomment_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\gifcomment_16m_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\gifcomment_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\single_frame_transparency* c:\te_tsi_icl_cod_05\gif\single_frame_transparency*
copy e:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16ma_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16mu_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16m_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\single_interlaced* c:\te_tsi_icl_cod_05\gif\single_interlaced*
copy e:\te_tsi_icl_cod_05\gif\ref\single_interlaced_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\single_interlaced_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\single_interlaced_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\single_interlaced_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\single_interlaced_16m_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\single_interlaced_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\1bpp_with_transparency* c:\te_tsi_icl_cod_05\ico\1bpp_with_transparency*
copy e:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16m_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16ma_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16mu_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16m_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_mask_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\1bpp_with_transparency_mask_mask_decode*

REM Run the tests
testexecute z:\testdata\scripts\Te_tsi_icl_cod_5suite_2.script

REM Move the results file from the ROM to the MMC
move c:\logs\testexecute\Te_tsi_icl_cod_5suite_2.htm e:\logs\testexecute\Te_tsi_icl_cod_5suite_2.htm

REM Delete the test images
del c:\Te_tsi_icl_cod_05\bmp\ref\*.*
del c:\Te_tsi_icl_cod_05\bmp\*.*
del c:\Te_tsi_icl_cod_05\gif\ref\*.*
del c:\Te_tsi_icl_cod_05\gif\*.*
del c:\Te_tsi_icl_cod_05\ico\ref\*.*
del c:\Te_tsi_icl_cod_05\ico\*.*
del c:\Te_tsi_icl_cod_05\jpeg\ref\*.*
del c:\Te_tsi_icl_cod_05\jpeg\*.*
del c:\Te_tsi_icl_cod_05\mbm\ref\*.*
del c:\Te_tsi_icl_cod_05\mbm\*.*
del c:\Te_tsi_icl_cod_05\mng\ref\*.*
del c:\Te_tsi_icl_cod_05\mng\*.*
del c:\Te_tsi_icl_cod_05\ota\ref\*.*
del c:\Te_tsi_icl_cod_05\ota\*.*
del c:\Te_tsi_icl_cod_05\png\ref\*.*
del c:\Te_tsi_icl_cod_05\png\*.*
del c:\Te_tsi_icl_cod_05\tif\ref\*.*
del c:\Te_tsi_icl_cod_05\tif\*.*
del c:\Te_tsi_icl_cod_05\wbmp\ref\*.*
del c:\Te_tsi_icl_cod_05\wbmp\*.*
del c:\Te_tsi_icl_cod_05\wmf\ref\*.*
del c:\Te_tsi_icl_cod_05\wmf\*.*
del c:\Te_tsi_icl_cod_05\*.*

REM Copy test images from MMC to ROM
copy e:\te_tsi_icl_cod_05\ico\4bpp_with_transparency* c:\te_tsi_icl_cod_05\ico\4bpp_with_transparency*
copy e:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16m_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16ma_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16mu_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16m_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_mask_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency* c:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16m_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16ma_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16mu_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16m_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale* c:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16m_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale* c:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16mu_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16m_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\transparent_businessman* c:\te_tsi_icl_cod_05\wmf\transparent_businessman*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16m_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16ma_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16mu_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16m_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_mask_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\transparent_forklift* c:\te_tsi_icl_cod_05\wmf\transparent_forklift*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16m_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16ma_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16mu_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16m_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_mask_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_forklift_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\transparent_house* c:\te_tsi_icl_cod_05\wmf\transparent_house*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16m_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16ma_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16mu_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16m_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_house_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_house_mask_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_house_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\transparent_plane* c:\te_tsi_icl_cod_05\wmf\transparent_plane*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16m_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16ma_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16mu_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16m_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_mask_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_plane_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\bmp\bmp* c:\te_tsi_icl_cod_05\bmp\bmp*
copy e:\te_tsi_icl_cod_05\bmp\ref\bmp_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\bmp\ref\bmp_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\bmp\ref\bmp_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\bmp\ref\bmp_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\bmp\ref\bmp_16m_no_mask_decode* c:\te_tsi_icl_cod_05\bmp\ref\bmp_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ota\ota* c:\te_tsi_icl_cod_05\ota\ota*
copy e:\te_tsi_icl_cod_05\ota\ref\ota_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ota\ref\ota_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ota\ref\ota_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\ota\ref\ota_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ota\ref\ota_16m_no_mask_decode* c:\te_tsi_icl_cod_05\ota\ref\ota_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\tif\tif* c:\te_tsi_icl_cod_05\tif\tif*
copy e:\te_tsi_icl_cod_05\tif\ref\tif_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\tif\ref\tif_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\tif\ref\tif_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\tif\ref\tif_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\tif\ref\tif_16m_no_mask_decode* c:\te_tsi_icl_cod_05\tif\ref\tif_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wbmp\wbmp* c:\te_tsi_icl_cod_05\wbmp\wbmp*
copy e:\te_tsi_icl_cod_05\wbmp\ref\wbmp_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\wbmp\ref\wbmp_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wbmp\ref\wbmp_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\wbmp\ref\wbmp_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wbmp\ref\wbmp_16m_no_mask_decode* c:\te_tsi_icl_cod_05\wbmp\ref\wbmp_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mbm\16masrc_uncompressed* c:\te_tsi_icl_cod_05\mbm\16masrc_uncompressed*
copy e:\te_tsi_icl_cod_05\bmp\ref\16masrc_uncompressed_16ma_encode* c:\te_tsi_icl_cod_05\bmp\ref\16masrc_uncompressed_16ma_encode*
copy e:\te_tsi_icl_cod_05\jpeg\ref\16masrc_uncompressed_16ma_encode* c:\te_tsi_icl_cod_05\jpeg\ref\16masrc_uncompressed_16ma_encode*
copy e:\te_tsi_icl_cod_05\gif\ref\16masrc_uncompressed_16ma_encode* c:\te_tsi_icl_cod_05\gif\ref\16masrc_uncompressed_16ma_encode*
copy e:\te_tsi_icl_cod_05\png\ref\16masrc_uncompressed_16ma_encode* c:\te_tsi_icl_cod_05\png\ref\16masrc_uncompressed_16ma_encode*
copy e:\te_tsi_icl_cod_05\mbm\ref\16masrc_uncompressed_16ma_encode* c:\te_tsi_icl_cod_05\mbm\ref\16masrc_uncompressed_16ma_encode*
copy e:\te_tsi_icl_cod_05\mbm\ref\16masrc_uncompressed_16mu_encode* c:\te_tsi_icl_cod_05\mbm\ref\16masrc_uncompressed_16mu_encode*
copy e:\te_tsi_icl_cod_05\mbm\ref\16masrc_uncompressed_16m_encode* c:\te_tsi_icl_cod_05\mbm\ref\16masrc_uncompressed_16m_encode*
copy e:\te_tsi_icl_cod_05\bmp\8bpp_bmp_to_16ma_dest* c:\te_tsi_icl_cod_05\bmp\8bpp_bmp_to_16ma_dest*
copy e:\te_tsi_icl_cod_05\bmp\ref\8bpp_bmp_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\bmp\ref\8bpp_bmp_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\bmp\ref\8bpp_bmp_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\bmp\ref\8bpp_bmp_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\bmp\ref\8bpp_bmp_16m_no_mask_decode* c:\te_tsi_icl_cod_05\bmp\ref\8bpp_bmp_16m_no_mask_decode*

REM Run the tests
testexecute z:\testdata\scripts\Te_tsi_icl_cod_5suite_3.script

REM Move the results file from the ROM to the MMC
move c:\logs\testexecute\Te_tsi_icl_cod_5suite_3.htm e:\logs\testexecute\Te_tsi_icl_cod_5suite_3.htm

REM Delete the test images
del c:\Te_tsi_icl_cod_05\bmp\ref\*.*
del c:\Te_tsi_icl_cod_05\bmp\*.*
del c:\Te_tsi_icl_cod_05\gif\ref\*.*
del c:\Te_tsi_icl_cod_05\gif\*.*
del c:\Te_tsi_icl_cod_05\ico\ref\*.*
del c:\Te_tsi_icl_cod_05\ico\*.*
del c:\Te_tsi_icl_cod_05\jpeg\ref\*.*
del c:\Te_tsi_icl_cod_05\jpeg\*.*
del c:\Te_tsi_icl_cod_05\mbm\ref\*.*
del c:\Te_tsi_icl_cod_05\mbm\*.*
del c:\Te_tsi_icl_cod_05\mng\ref\*.*
del c:\Te_tsi_icl_cod_05\mng\*.*
del c:\Te_tsi_icl_cod_05\ota\ref\*.*
del c:\Te_tsi_icl_cod_05\ota\*.*
del c:\Te_tsi_icl_cod_05\png\ref\*.*
del c:\Te_tsi_icl_cod_05\png\*.*
del c:\Te_tsi_icl_cod_05\tif\ref\*.*
del c:\Te_tsi_icl_cod_05\tif\*.*
del c:\Te_tsi_icl_cod_05\wbmp\ref\*.*
del c:\Te_tsi_icl_cod_05\wbmp\*.*
del c:\Te_tsi_icl_cod_05\wmf\ref\*.*
del c:\Te_tsi_icl_cod_05\wmf\*.*
del c:\Te_tsi_icl_cod_05\*.*

REM Copy test images from MMC to ROM for Alloc Tests
copy e:\te_tsi_icl_cod_05\mbm\16masrc_to_16madest_uncompressed* c:\te_tsi_icl_cod_05\mbm\16masrc_to_16madest_uncompressed*
copy e:\te_tsi_icl_cod_05\mbm\ref\16masrc_to_16madest_uncompressed* c:\te_tsi_icl_cod_05\mbm\ref\16masrc_to_16madest_uncompressed*
copy e:\te_tsi_icl_cod_05\gif\single_frame_transparency* c:\te_tsi_icl_cod_05\gif\single_frame_transparency*
copy e:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16ma_no_mask_decode_frame_0* c:\te_tsi_icl_cod_05\gif\ref\single_frame_transparency_16ma_no_mask_decode_frame_0*
copy e:\te_tsi_icl_cod_05\png\bgan6a08* c:\te_tsi_icl_cod_05\png\bgan6a08*
copy e:\te_tsi_icl_cod_05\png\ref\bgan6a08_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\png\ref\bgan6a08_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\wmf\transparent_businessman* c:\te_tsi_icl_cod_05\wmf\transparent_businessman*
copy e:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\wmf\ref\transparent_businessman_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\4bpp_with_transparency* c:\te_tsi_icl_cod_05\ico\4bpp_with_transparency*
copy e:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\4bpp_with_transparency_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale* c:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale* c:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale* c:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency_scale*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\gifcomment* c:\te_tsi_icl_cod_05\gif\gifcomment*
copy e:\te_tsi_icl_cod_05\gif\ref\gifcomment_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\gifcomment_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency* c:\te_tsi_icl_cod_05\ico\8bpp_multi_frame_with_transparency*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\24bit* c:\te_tsi_icl_cod_05\mng\24bit*
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_scale_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\gif\ref\gifcomment_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\gif\ref\gifcomment_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\ico\ref\8bpp_multi_frame_with_transparency_16ma_no_mask_decode*


REM Run the alloc tests
testexecute z:\testdata\scripts\Te_tsi_icl_cod_5asuite.script

REM Move the results file from the ROM to the MMC
move c:\logs\testexecute\Te_tsi_icl_cod_5asuite.htm e:\logs\testexecute\Te_tsi_icl_cod_5asuite.htm

REM Delete the test images
del c:\Te_tsi_icl_cod_05\bmp\ref\*.*
del c:\Te_tsi_icl_cod_05\bmp\*.*
del c:\Te_tsi_icl_cod_05\gif\ref\*.*
del c:\Te_tsi_icl_cod_05\gif\*.*
del c:\Te_tsi_icl_cod_05\ico\ref\*.*
del c:\Te_tsi_icl_cod_05\ico\*.*
del c:\Te_tsi_icl_cod_05\jpeg\ref\*.*
del c:\Te_tsi_icl_cod_05\jpeg\*.*
del c:\Te_tsi_icl_cod_05\mbm\ref\*.*
del c:\Te_tsi_icl_cod_05\mbm\*.*
del c:\Te_tsi_icl_cod_05\mng\ref\*.*
del c:\Te_tsi_icl_cod_05\mng\*.*
del c:\Te_tsi_icl_cod_05\ota\ref\*.*
del c:\Te_tsi_icl_cod_05\ota\*.*
del c:\Te_tsi_icl_cod_05\png\ref\*.*
del c:\Te_tsi_icl_cod_05\png\*.*
del c:\Te_tsi_icl_cod_05\tif\ref\*.*
del c:\Te_tsi_icl_cod_05\tif\*.*
del c:\Te_tsi_icl_cod_05\wbmp\ref\*.*
del c:\Te_tsi_icl_cod_05\wbmp\*.*
del c:\Te_tsi_icl_cod_05\wmf\ref\*.*
del c:\Te_tsi_icl_cod_05\wmf\*.*
del c:\Te_tsi_icl_cod_05\*.*


REM Copy test images from MMC to ROM for MNG Tests - 1
copy e:\te_tsi_icl_cod_05\mng\24bit.mng c:\te_tsi_icl_cod_05\mng\24bit.mng
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16m_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16mu_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16m_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\advisory_bg.mng c:\te_tsi_icl_cod_05\mng\advisory_bg.mng
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16m_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16ma_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16mu_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16m_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\no_any_bg.mng c:\te_tsi_icl_cod_05\mng\no_any_bg.mng
copy e:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16m_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16ma_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16mu_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16m_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\fram1.mng c:\te_tsi_icl_cod_05\mng\fram1.mng
copy e:\te_tsi_icl_cod_05\mng\ref\fram1_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\fram1_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\fram1_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\fram1_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\fram1_16m_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\fram1_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\fram1_16ma_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\fram1_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\fram1_16mu_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\fram1_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\fram1_16m_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\fram1_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\subframes.mng c:\te_tsi_icl_cod_05\mng\subframes.mng
copy e:\te_tsi_icl_cod_05\mng\ref\subframes_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\subframes_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\subframes_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\subframes_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\subframes_16m_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\subframes_16m_no_mask_decode*

REM Run the tests
testexecute z:\testdata\scripts\te_tsi_icl_cod_5suite_mng_1.script

REM Move the results file from the ROM to the MMC
move c:\logs\testexecute\te_tsi_icl_cod_5suite_mng_1.htm e:\logs\testexecute\te_tsi_icl_cod_5suite_mng_1.htm

del c:\Te_tsi_icl_cod_05\mng\ref\*.*
del c:\Te_tsi_icl_cod_05\mng\*.*

REM Copy test images from MMC to ROM for MNG Tests - 2
copy e:\te_tsi_icl_cod_05\mng\subframes.mng c:\te_tsi_icl_cod_05\mng\subframes.mng
copy e:\te_tsi_icl_cod_05\mng\ref\subframes_16ma_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\subframes_16ma_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\subframes_16mu_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\subframes_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\subframes_16m_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\subframes_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\animated_no_alpha.mng c:\te_tsi_icl_cod_05\mng\animated_no_alpha.mng
copy e:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16m_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\24bit.mng c:\te_tsi_icl_cod_05\mng\24bit.mng
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16mu_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16mu_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\24bit_16m_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\24bit_16m_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\advisory_bg.mng c:\te_tsi_icl_cod_05\mng\advisory_bg.mng
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16ma_no_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16ma_no_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16mu_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16mu_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16m_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_16m_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\advisory_bg_mask_mask_decode* c:\te_tsi_icl_cod_05\mng\ref\advisory_bg_mask_mask_decode*
copy e:\te_tsi_icl_cod_05\mng\animated_no_alpha.mng c:\te_tsi_icl_cod_05\mng\animated_no_alpha.mng
copy e:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16ma_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16ma_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16mu_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16mu_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16m_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\animated_no_alpha_16m_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\fram1.mng c:\te_tsi_icl_cod_05\mng\fram1.mng
copy e:\te_tsi_icl_cod_05\mng\ref\fram1_16ma_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\fram1_16ma_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\fram1_16mu_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\fram1_16mu_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\fram1_16m_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\fram1_16m_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\no_any_bg.mng c:\te_tsi_icl_cod_05\mng\no_any_bg.mng
copy e:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16ma_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16ma_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16mu_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16mu_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16m_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\no_any_bg_16m_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\subframes.mng c:\te_tsi_icl_cod_05\mng\subframes.mng
copy e:\te_tsi_icl_cod_05\mng\ref\subframes_16ma_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\subframes_16ma_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\subframes_16mu_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\subframes_16mu_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\ref\subframes_16m_image_display_decode* c:\te_tsi_icl_cod_05\mng\ref\subframes_16m_image_display_decode*
copy e:\te_tsi_icl_cod_05\mng\animated_no_alpha.mng c:\te_tsi_icl_cod_05\mng\animated_no_alpha.mng

REM Run the tests
testexecute z:\testdata\scripts\te_tsi_icl_cod_5suite_mng_2.script

REM Move the results file from the ROM to the MMC
move c:\logs\testexecute\te_tsi_icl_cod_5suite_mng_2.htm e:\logs\testexecute\te_tsi_icl_cod_5suite_mng_2.htm

del c:\Te_tsi_icl_cod_05\mng\ref\*.*
del c:\Te_tsi_icl_cod_05\mng\*.*
