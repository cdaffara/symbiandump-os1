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
# Build DBMS test files
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	TARGETDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/private/100012a5/policy
	TARGETDIR2 = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/private/100012a5
	TARGETDIR3 = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/test
else
	TARGETDIR = $(EPOCROOT)epoc32/data/z/private/100012a5/policy
	TARGETDIR2 = $(EPOCROOT)epoc32/data/z/private/100012a5
	TARGETDIR3 = $(EPOCROOT)epoc32/data/z/system/test
endif

TARGET_FILES = \
	$(TARGETDIR)/11335577.spd \
	$(TARGETDIR)/11335578.spd \
	$(TARGETDIR)/11335579.spd \
	$(TARGETDIR)/1133557a.spd \
	$(TARGETDIR)/12344321.spd \
	$(TARGETDIR2)/dbs_11335578_a.db \
	$(TARGETDIR2)/dbs_11335578_b.db \
	$(TARGETDIR2)/dbs_11335578_z.db \
	$(TARGETDIR2)/dbs_11335579_testdb.db \
	$(TARGETDIR2)/dbs_1133557a_zzz.db \
	$(TARGETDIR2)/dbs_12344321_1234567890123456789012345678901234567890123456789012345678901234567890.db\
	$(TARGETDIR3)/11335577.txt\
	$(TARGETDIR3)/11335578.txt\
	$(TARGETDIR3)/11335579.txt\
	$(TARGETDIR3)/1133557A.txt\
	$(TARGETDIR3)/t_dbperf1.sql\
	$(TARGETDIR3)/t_script.txt\
	$(TARGETDIR3)/typetextktests44.dat\
	$(TARGETDIR3)/typetextktests46.dat\
	$(TARGETDIR3)/typetextktests47.dat


$(TARGETDIR) :
	$(call createdir, "$@")

$(TARGETDIR2) :
	$(call createdir, "$@")
	
$(TARGETDIR3) :
	$(call createdir, "$@")

COPYFILES :
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/11335577.spd,$(TARGETDIR)/11335577.spd)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/11335578.spd,$(TARGETDIR)/11335578.spd)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/11335579.spd,$(TARGETDIR)/11335579.spd)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/1133557A.spd,$(TARGETDIR)/1133557a.spd)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/12344321.spd,$(TARGETDIR)/12344321.spd)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/dbs_11335578_A.DB,$(TARGETDIR2)/dbs_11335578_a.db)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/dbs_11335578_B.DB,$(TARGETDIR2)/dbs_11335578_b.db)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/dbs_11335578_Z.DB,$(TARGETDIR2)/dbs_11335578_z.db)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/dbs_11335579_TESTDB.DB,$(TARGETDIR2)/dbs_11335579_testdb.db)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/dbs_1133557A_ZZZ.DB,$(TARGETDIR2)/dbs_1133557a_zzz.db)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/dbs_12344321_1234567890123456789012345678901234567890123456789012345678901234567890.DB,$(TARGETDIR2)/dbs_12344321_1234567890123456789012345678901234567890123456789012345678901234567890.db)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/11335577.txt,$(TARGETDIR3)/11335577.txt)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/11335578.txt,$(TARGETDIR3)/11335578.txt)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/11335579.txt,$(TARGETDIR3)/11335579.txt)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/1133557A.txt,$(TARGETDIR3)/1133557A.txt)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/t_dbperf1.sql,$(TARGETDIR3)/t_dbperf1.sql)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/t_script.txt,$(TARGETDIR3)/t_script.txt)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/typetextktests44.dat,$(TARGETDIR3)/typetextktests44.dat)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/typetextktests46.dat,$(TARGETDIR3)/typetextktests46.dat)
		$(call forcecopy,$(EXTENSION_ROOT)/../TDBMS/typetextktests47.dat,$(TARGETDIR3)/typetextktests47.dat)
		
DO_NOTHING :
	@echo do nothing
#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

BLD : $(TARGETDIR) $(TARGETDIR2) $(TARGETDIR3) COPYFILES

CLEAN : 
	$(call forceremove,$(TARGET_FILES))

SAVESPACE : DO_NOTHING

RESOURCE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING

RELEASABLES :
	@echo $(TARGETDIR)/11335577.spd
	@echo $(TARGETDIR)/11335578.spd
	@echo $(TARGETDIR)/11335579.spd
	@echo $(TARGETDIR)/1133557a.spd
	@echo $(TARGETDIR)/12344321.spd
	@echo $(TARGETDIR2)/dbs_11335578_a.db
	@echo $(TARGETDIR2)/dbs_11335578_b.db
	@echo $(TARGETDIR2)/dbs_11335578_z.db
	@echo $(TARGETDIR2)/dbs_11335579_testdb.db
	@echo $(TARGETDIR2)/dbs_1133557a_zzz.db
	@echo $(TARGETDIR2)/dbs_12344321_1234567890123456789012345678901234567890123456789012345678901234567890.db
	@echo $(TARGETDIR3)/11335577.txt
	@echo $(TARGETDIR3)/11335578.txt
	@echo $(TARGETDIR3)/11335579.txt
	@echo $(TARGETDIR3)/1133557A.txt
	@echo $(TARGETDIR3)/t_dbperf1.sql
	@echo $(TARGETDIR3)/t_script.txt
	@echo $(TARGETDIR3)/typetextktests44.dat
	@echo $(TARGETDIR3)/typetextktests46.dat
	@echo $(TARGETDIR3)/typetextktests47.dat
