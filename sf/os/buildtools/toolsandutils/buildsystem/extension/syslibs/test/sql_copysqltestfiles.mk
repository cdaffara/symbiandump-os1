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
# Copy SQL test files
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	TARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/test
	TARGETDIR2=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/private/10281e17
	TARGETDIR3=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/private/21212124
else
	TARGETDIR=$(EPOCROOT)epoc32/data/z/system/test
	TARGETDIR2=$(EPOCROOT)epoc32/data/z/private/10281e17
	TARGETDIR3=$(EPOCROOT)epoc32/data/z/private/21212124

endif

SOURCEDIR = $(EXTENSION_ROOT)/../TEST

$(TARGETDIR):
	$(call createdir, "$@")
$(TARGETDIR2):
	$(call createdir, "$@")
$(TARGETDIR3):	
	$(call createdir, "$@")

COPYFILES :
	$(call forcecopy,$(SOURCEDIR)/TestDb1.db,$(TARGETDIR)/testdb1.db)
	$(call forcecopy,$(SOURCEDIR)/CorruptDb.db,$(TARGETDIR)/corruptdb.db)
	$(call forcecopy,$(SOURCEDIR)/contacts_schema_to_vendors.sql,$(TARGETDIR)/contacts_schema_to_vendors.sql)
	$(call forcecopy,$(SOURCEDIR)/add_simple_contacts.sql,$(TARGETDIR)/add_simple_contacts.sql)
	$(call forcecopy,$(SOURCEDIR)/t_SqlShortNonDb.db,$(TARGETDIR)/t_sqlshortnondb.db)
	$(call forcecopy,$(SOURCEDIR)/contacts_startup_time.sql,$(TARGETDIR)/contacts_startup_time.sql)
	$(call forcecopy,$(SOURCEDIR)/t_inc095412.db,$(TARGETDIR)/t_inc095412.db)
	$(call forcecopy,$(SOURCEDIR)/{21212122}AADb2.db,$(TARGETDIR2)/[21212122]aadb2.db)
	$(call forcecopy,$(SOURCEDIR)/{21212122}BBDb2.db,$(TARGETDIR2)/[21212122]bbdb2.db)
	$(call forcecopy,$(SOURCEDIR)/{21212125}T_AB.db,$(TARGETDIR2)/[21212125]t_ab.db)
	$(call forcecopy,$(SOURCEDIR)/{1111CCCC}T_AB.db,$(TARGETDIR2)/[1111CCCC]t_ab.db)
	$(call forcecopy,$(SOURCEDIR)/{21212125}T_OneDefPolicy.db,$(TARGETDIR2)/[21212125]t_onedefpolicy.db)
#	$(call forcecopy,$(SOURCEDIR)/SqlServer.cfg,$(TARGETDIR2)/sqlserver.cfg)
	$(call forcecopy,$(SOURCEDIR)/t_sqldb1.db,$(TARGETDIR3)/t_sqldb1.db)
	$(call forcecopy,$(SOURCEDIR)/{98765432}t_2defaultpolicies.db,$(TARGETDIR2)/[98765432]t_2defaultpolicies.db)
	$(call forcecopy,$(SOURCEDIR)/{98765432}t_emptysettings.db,$(TARGETDIR2)/[98765432]t_emptysettings.db)
	$(call forcecopy,$(SOURCEDIR)/{98765432}t_invobject.db,$(TARGETDIR2)/[98765432]t_invobject.db)
	$(call forcecopy,$(SOURCEDIR)/{98765432}t_invversion.db,$(TARGETDIR2)/[98765432]t_invversion.db)
	$(call forcecopy,$(SOURCEDIR)/{98765432}t_longcollation.db,$(TARGETDIR2)/[98765432]t_longcollation.db)
	$(call forcecopy,$(SOURCEDIR)/{98765432}t_nocollation.db,$(TARGETDIR2)/[98765432]t_nocollation.db)
	$(call forcecopy,$(SOURCEDIR)/{98765432}t_nodefaultpolicy.db,$(TARGETDIR2)/[98765432]t_nodefaultpolicy.db)
	$(call forcecopy,$(SOURCEDIR)/{98765432}t_nosettings.db,$(TARGETDIR2)/[98765432]t_nosettings.db)
	$(call forcecopy,$(SOURCEDIR)/default_avacon.dbSQL,$(TARGETDIR)/default_avacon.dbSQL)
	$(call forcecopy,$(SOURCEDIR)/t_sqlbur_backup_ver0.bak,$(TARGETDIR)/t_sqlbur_backup_ver0.bak)
	$(call forcecopy,$(SOURCEDIR)/t_sqlperformance4.sql,$(TARGETDIR)/t_sqlperformance4.sql)

	
DO_NOTHING :
	@echo do nothing

#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

BLD : $(TARGETDIR) $(TARGETDIR2) $(TARGETDIR3) $(SOURCEDIR) COPYFILES

CLEAN : 
	$(call forceremove,$(TARGETDIR)/testdb1.db)
	$(call forceremove,$(TARGETDIR)/corruptdb.db)
	$(call forceremove,$(TARGETDIR)/contacts_schema_to_vendors.sql)
	$(call forceremove,$(TARGETDIR)/add_simple_contacts.sql)
	$(call forceremove,$(TARGETDIR)/t_sqlshortnondb.db)
	$(call forceremove,$(TARGETDIR)/contacts_startup_time.sql)
	$(call forceremove,$(TARGETDIR)/t_inc095412.db)
	$(call forceremove,$(TARGETDIR2)/[21212122]aadb2.db)
	$(call forceremove,$(TARGETDIR2)/[21212122]bbdb2.db)
	$(call forceremove,$(TARGETDIR2)/[21212125]t_ab.db)
	$(call forceremove,$(TARGETDIR2)/[1111CCCC]t_ab.db)
	$(call forceremove,$(TARGETDIR2)/[21212125]t_onedefpolicy.db)
#	$(call forceremove,$(TARGETDIR2)/sqlserver.cfg)
	$(call forceremove,$(TARGETDIR3)/t_sqldb1.db)
	$(call forceremove,$(TARGETDIR2)/[98765432]t_2defaultpolicies.db)
	$(call forceremove,$(TARGETDIR2)/[98765432]t_emptysettings.db)
	$(call forceremove,$(TARGETDIR2)/[98765432]t_invobject.db)
	$(call forceremove,$(TARGETDIR2)/[98765432]t_invversion.db)
	$(call forceremove,$(TARGETDIR2)/[98765432]t_longcollation.db)
	$(call forceremove,$(TARGETDIR2)/[98765432]t_nocollation.db)
	$(call forceremove,$(TARGETDIR2)/[98765432]t_nodefaultpolicy.db)
	$(call forceremove,$(TARGETDIR2)/[98765432]t_nosettings.db)
	$(call forceremove,$(TARGETDIR)/default_avacon.dbSQL)
	$(call forceremove,$(TARGETDIR)/t_sqlbur_backup_ver0.bak)
	$(call forceremove,$(TARGETDIR)/t_sqlperformance4.sql)
	
SAVESPACE : DO_NOTHING

RESOURCE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING

RELEASABLES : 
	@echo $(TARGETDIR)/testdb1.db
	@echo $(TARGETDIR)/corruptdb.db
	@echo $(TARGETDIR)/contacts_schema_to_vendors.sql
	@echo $(TARGETDIR)/add_simple_contacts.sql
	@echo $(TARGETDIR)/t_sqlshortnondb.db
	@echo $(TARGETDIR)/contacts_startup_time.sql
	@echo $(TARGETDIR)/t_inc095412.db
	@echo $(TARGETDIR2)/[21212122]aadb2.db
	@echo $(TARGETDIR2)/[21212122]bbdb2.db
	@echo $(TARGETDIR2)/[21212125]t_ab.db
	@echo $(TARGETDIR2)/[1111CCCC]t_ab.db
	@echo $(TARGETDIR2)/[21212125]t_onedefpolicy.db
#	@echo $(TARGETDIR2)/sqlserver.cfg
	@echo $(TARGETDIR3)/t_sqldb1.db
	@echo $(TARGETDIR2)/[98765432]t_2defaultpolicies.db
	@echo $(TARGETDIR2)/[98765432]t_emptysettings.db
	@echo $(TARGETDIR2)/[98765432]t_invobject.db
	@echo $(TARGETDIR2)/[98765432]t_invversion.db
	@echo $(TARGETDIR2)/[98765432]t_longcollation.db
	@echo $(TARGETDIR2)/[98765432]t_nocollation.db
	@echo $(TARGETDIR2)/[98765432]t_nodefaultpolicy.db
	@echo $(TARGETDIR2)/[98765432]t_nosettings.db
	@echo $(TARGETDIR)/default_avacon.dbSQL
	@echo $(TARGETDIR)/t_sqlbur_backup_ver0.bak
	@echo $(TARGETDIR)/t_sqlperformance4.sql
