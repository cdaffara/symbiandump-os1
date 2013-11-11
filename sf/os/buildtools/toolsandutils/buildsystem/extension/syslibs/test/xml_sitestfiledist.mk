# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/


include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(OS), "Windows_NT")
RMDIR := @rmdir 2>>nul
else
RMDIR := @rmdir
endif

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	DEST_DIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/system/xmltest/serviceindication
else
	DEST_DIR = $(EPOCROOT)epoc32/data/z/system/xmltest/serviceindication
endif

SOURCE_DIR = $(EXTENSION_ROOT)/../test/rtest/data/serviceindication

# Ensure we have a clean canvas - this also avoids us having to specify
# switches to commands that are not recognised across platforms.
CLEAN_DEST :
	$(call createdir,$(DEST_DIR))

MAKE_DIRS :
	$(call createdir,$(DEST_DIR)/1.0)
	$(call createdir,$(DEST_DIR)/corrupt)

COPYFILES : CLEAN_DEST MAKE_DIRS
	$(CP) $(call slash2generic, $(SOURCE_DIR)/1.0/*.bdy $(DEST_DIR)/1.0/*.bdy)
	$(CP) $(call slash2generic, $(SOURCE_DIR)/corrupt/*.bdy $(DEST_DIR)/corrupt/*.bdy)

DO_NOTHING:
	@echo do nothing

#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

RESOURCE : COPYFILES

CLEAN : 
	$(call createdir,$(DEST_DIR))

BLD : DO_NOTHING

SAVESPACE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

RESOURCE : DO_NOTHING

FINAL : DO_NOTHING

RELEASABLES :
	@echo $(DEST_DIR)/1.0/si_invalid_cdate.bdy
	@echo $(DEST_DIR)/1.0/si_inv_act_multiple.bdy
	@echo $(DEST_DIR)/1.0/si_inv_act_multiple2.bdy
	@echo $(DEST_DIR)/1.0/si_inv_cd_day.bdy
	@echo $(DEST_DIR)/1.0/si_inv_cd_hour.bdy
	@echo $(DEST_DIR)/1.0/si_inv_cd_len.bdy
	@echo $(DEST_DIR)/1.0/si_inv_cd_min.bdy
	@echo $(DEST_DIR)/1.0/si_inv_cd_sec.bdy
	@echo $(DEST_DIR)/1.0/si_inv_cd_year.bdy
	@echo $(DEST_DIR)/1.0/si_inv_ind.bdy
	@echo $(DEST_DIR)/1.0/si_inv_msg_emp.bdy
	@echo $(DEST_DIR)/1.0/si_valid.bdy
	@echo $(DEST_DIR)/1.0/si_valid_chi.bdy
	@echo $(DEST_DIR)/1.0/si_val_act_de.bdy
	@echo $(DEST_DIR)/1.0/si_val_act_no.bdy
	@echo $(DEST_DIR)/1.0/si_val_act_sh.bdy
	@echo $(DEST_DIR)/1.0/si_val_act_sl.bdy
	@echo $(DEST_DIR)/1.0/si_val_act_sm.bdy
	@echo $(DEST_DIR)/1.0/si_val_act_sn.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_iso_8859_1.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_iso_8859_2.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_iso_8859_3.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_iso_8859_4.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_iso_8859_5.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_iso_8859_6.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_iso_8859_7.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_iso_8859_8.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_iso_8859_9.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_shift_JIS.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_us_ascii.bdy
	@echo $(DEST_DIR)/1.0/si_val_char_utf_8.bdy
	@echo $(DEST_DIR)/1.0/si_val_expired.bdy
	@echo $(DEST_DIR)/1.0/si_val_exp_null.bdy
	@echo $(DEST_DIR)/1.0/si_val_ind_opq.bdy
	@echo $(DEST_DIR)/1.0/si_val_msg_emp_indatt.bdy
	@echo $(DEST_DIR)/1.0/si_val_no_cd.bdy
	@echo $(DEST_DIR)/1.0/si_val_no_exp.bdy
	@echo $(DEST_DIR)/1.0/si_val_only_href.bdy
	@echo $(DEST_DIR)/1.0/si_val_sender_encoding.bdy
	@echo $(DEST_DIR)/1.0/si_val_wbx_ver_10.bdy
	@echo $(DEST_DIR)/1.0/si_val_wbx_ver_11.bdy
	@echo $(DEST_DIR)/1.0/si_val_wbx_ver_12.bdy
	@echo $(DEST_DIR)/1.0/si_val_wbx_ver_13.bdy
	@echo $(DEST_DIR)/corrupt/si_inv_act_undf.bdy
	@echo $(DEST_DIR)/corrupt/si_inv_cd_token.bdy
	@echo $(DEST_DIR)/corrupt/si_inv_cd_type.bdy
	@echo $(DEST_DIR)/corrupt/si_inv_char.bdy
	@echo $(DEST_DIR)/corrupt/si_inv_hrf_ter.bdy
	@echo $(DEST_DIR)/corrupt/si_inv_msg_emp2.bdy
	@echo $(DEST_DIR)/corrupt/si_inv_msg_ter.bdy
	@echo $(DEST_DIR)/corrupt/si_inv_msg_ter2.bdy
	@echo $(DEST_DIR)/corrupt/si_val_wbx_ver_14.bdy
	@echo $(DEST_DIR)/corrupt/si_val_wbx_ver_30.bdy
	@echo $(DEST_DIR)/corrupt/si_val_wbx_ver_FF.bdy
