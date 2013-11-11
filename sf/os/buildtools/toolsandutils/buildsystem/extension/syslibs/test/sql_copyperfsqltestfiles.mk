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
TARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/testdata
else
TARGETDIR=$(EPOCROOT)epoc32/data/z/testdata
endif

SOURCEDIR = $(EXTENSION_ROOT)/../SCRIPT
SOURCEDIR2= $(EXTENSION_ROOT)/../TESTDATA

$(TARGETDIR):
	$(call createdir, "$@")

COPYFILES :
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Config.script,$(TARGETDIR)/sqldb_config.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_BaseDatabase.script,$(TARGETDIR)/sqldb_basedatabase.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_CreateTable.script,$(TARGETDIR)/sqldb_measurement_createtable.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_CreateTable.script,$(TARGETDIR)/sqldb_performance_createtable.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_insert.script,$(TARGETDIR)/sqldb_measurement_insert.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_insert.script,$(TARGETDIR)/sqldb_performance_insert.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_droptable.script,$(TARGETDIR)/sqldb_measurement_droptable.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_droptable.script,$(TARGETDIR)/sqldb_performance_droptable.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_delete.script,$(TARGETDIR)/sqldb_measurement_delete.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_delete.script,$(TARGETDIR)/sqldb_performance_delete.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_altertable.script,$(TARGETDIR)/sqldb_measurement_altertable.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_altertable.script,$(TARGETDIR)/sqldb_performance_altertable.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_update.script,$(TARGETDIR)/sqldb_measurement_update.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_update.script,$(TARGETDIR)/sqldb_performance_update.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Concurrency.script,$(TARGETDIR)/sqldb_measurement_concurrency.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Concurrency.script,$(TARGETDIR)/sqldb_performance_concurrency.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Select1.script,$(TARGETDIR)/sqldb_measurement_select1.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Select1.script,$(TARGETDIR)/sqldb_performance_select1.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Select2.script,$(TARGETDIR)/sqldb_measurement_select2.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Select2.script,$(TARGETDIR)/sqldb_performance_select2.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Trigger.script,$(TARGETDIR)/sqldb_measurement_trigger.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Trigger.script,$(TARGETDIR)/sqldb_performance_trigger.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Transaction_Commit.script,$(TARGETDIR)/sqldb_measurement_transaction_commit.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Transaction_Commit_Index.script,$(TARGETDIR)/sqldb_measurement_transaction_commit_index.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Transaction_Commit_Mixed.script,$(TARGETDIR)/sqldb_measurement_transaction_commit_mixed.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Transaction_Commit.script,$(TARGETDIR)/sqldb_performance_transaction_commit.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Transaction_Commit_Index.script,$(TARGETDIR)/sqldb_performance_transaction_commit_index.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Transaction_Commit_Mixed.script,$(TARGETDIR)/sqldb_performance_transaction_commit_mixed.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Transaction_Rollback.script,$(TARGETDIR)/sqldb_measurement_transaction_rollback.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Transaction_Rollback_Index.script,$(TARGETDIR)/sqldb_measurement_transaction_rollback_index.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Transaction_Rollback_Mixed.script,$(TARGETDIR)/sqldb_measurement_transaction_rollback_mixed.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Transaction_Rollback.script,$(TARGETDIR)/sqldb_performance_transaction_rollback.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Transaction_Rollback_Index.script,$(TARGETDIR)/sqldb_performance_transaction_rollback_index.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Transaction_Rollback_Mixed.script,$(TARGETDIR)/sqldb_performance_transaction_rollback_mixed.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_AttachDB.script,$(TARGETDIR)/sqldb_measurement_attachdb.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_AttachDB.script,$(TARGETDIR)/sqldb_performance_attachdb.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Measurement_Apps.script,$(TARGETDIR)/sqldb_measurement_apps.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Apps.script,$(TARGETDIR)/sqldb_performance_apps.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Apps_Create.script,$(TARGETDIR)/sqldb_performance_apps_create.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Apps_Delete.script,$(TARGETDIR)/sqldb_performance_apps_delete.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Apps_Modify.script,$(TARGETDIR)/sqldb_performance_apps_modify.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Apps_Select.script,$(TARGETDIR)/sqldb_performance_apps_select.script)
	$(call forcecopy,$(SOURCEDIR)/SqlDB_Performance_Apps_Add.script,$(TARGETDIR)/sqldb_performance_apps_add.script)
	$(call forcecopy,$(SOURCEDIR2)/TestsqlDB_comparec0.db,$(TARGETDIR)/testsqldb_comparec0.db)
	$(call forcecopy,$(SOURCEDIR2)/TestsqlDB_comparec1.db,$(TARGETDIR)/testsqldb_comparec1.db)
	$(call forcecopy,$(SOURCEDIR2)/TestsqlDB_comparec2.db,$(TARGETDIR)/testsqldb_comparec2.db)
	$(call forcecopy,$(SOURCEDIR2)/TestsqlDB_comparec3.db,$(TARGETDIR)/testsqldb_comparec3.db)
	$(call forcecopy,$(SOURCEDIR2)/TestsqlDB_comparef.db,$(TARGETDIR)/testsqldb_comparef.db)
	$(call forcecopy,$(SOURCEDIR2)/SqlDB_testexecute.ini,$(TARGETDIR)/sqldb_testexecute.ini)
	$(call forcecopy,$(SOURCEDIR2)/TestSqlDB_Stress.ini,$(TARGETDIR)/testsqldb_stress.ini)
	$(call forcecopy,$(SOURCEDIR2)/TestSqlDB_Stress_view.ini,$(TARGETDIR)/testsqldb_stress_view.ini)

DO_NOTHING :
	@echo do nothing

#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

BLD : $(TARGETDIR) COPYFILES

CLEAN : 
	$(call forceremove,$(TARGETDIR)/sqldb_config.script)
	$(call forceremove,$(TARGETDIR)/sqldb_basedatabase.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_createtable.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_createtable.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_insert.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_insert.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_droptable.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_droptable.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_delete.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_delete.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_altertable.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_altertable.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_update.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_update.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_concurrency.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_concurrency.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_select1.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_select1.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_select2.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_select2.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_trigger.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_trigger.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_transaction_commit.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_transaction_commit_index.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_transaction_commit_mixed.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_transaction_commit.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_transaction_commit_mixed.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_transaction_rollback.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_transaction_rollback_index.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_transaction_rollback_mixed.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_transaction_rollback.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_transaction_rollback_index.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_transaction_rollback_mixed.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_attachdb.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_attachdb.script)
	$(call forceremove,$(TARGETDIR)/sqldb_measurement_apps.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_apps.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_apps_create.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_apps_delete.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_apps_modify.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_apps_select.script)
	$(call forceremove,$(TARGETDIR)/sqldb_performance_apps_add.script)
	$(call forceremove,$(TARGETDIR)/testsqldb_comparec0.db)
	$(call forceremove,$(TARGETDIR)/testsqldb_comparec1.db)
	$(call forceremove,$(TARGETDIR)/testsqldb_comparec2.db)
	$(call forceremove,$(TARGETDIR)/testsqldb_comparec3.db)
	$(call forceremove,$(TARGETDIR)/testsqldb_comparef.db)
	$(call forceremove,$(TARGETDIR)/sqldb_testexecute.ini)
	$(call forceremove,$(TARGETDIR)/testsqldb_stress.ini)
	$(call forceremove,$(TARGETDIR)/testsqldb_stress_view.ini)

SAVESPACE : DO_NOTHING

RESOURCE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING

RELEASABLES :
	@echo $(TARGETDIR)/sqldb_config.script
	@echo $(TARGETDIR)/sqldb_basedatabase.script
	@echo $(TARGETDIR)/sqldb_measurement_createtable.script
	@echo $(TARGETDIR)/sqldb_performance_createtable.script
	@echo $(TARGETDIR)/sqldb_measurement_insert.script
	@echo $(TARGETDIR)/sqldb_performance_insert.script
	@echo $(TARGETDIR)/sqldb_measurement_droptable.script
	@echo $(TARGETDIR)/sqldb_performance_droptable.script
	@echo $(TARGETDIR)/sqldb_measurement_delete.script
	@echo $(TARGETDIR)/sqldb_performance_delete.script
	@echo $(TARGETDIR)/sqldb_measurement_altertable.script
	@echo $(TARGETDIR)/sqldb_performance_altertable.script
	@echo $(TARGETDIR)/sqldb_measurement_update.script
	@echo $(TARGETDIR)/sqldb_performance_update.script
	@echo $(TARGETDIR)/sqldb_measurement_concurrency.script
	@echo $(TARGETDIR)/sqldb_performance_concurrency.script
	@echo $(TARGETDIR)/sqldb_measurement_select1.script
	@echo $(TARGETDIR)/sqldb_performance_select1.script
	@echo $(TARGETDIR)/sqldb_measurement_select2.script
	@echo $(TARGETDIR)/sqldb_performance_select2.script
	@echo $(TARGETDIR)/sqldb_measurement_trigger.script
	@echo $(TARGETDIR)/sqldb_performance_trigger.script
	@echo $(TARGETDIR)/sqldb_measurement_transaction_commit.script
	@echo $(TARGETDIR)/sqldb_measurement_transaction_commit_index.script
	@echo $(TARGETDIR)/sqldb_measurement_transaction_commit_mixed.script
	@echo $(TARGETDIR)/sqldb_performance_transaction_commit.script
	@echo $(TARGETDIR)/sqldb_performance_transaction_commit_mixed.script
	@echo $(TARGETDIR)/sqldb_measurement_transaction_rollback.script
	@echo $(TARGETDIR)/sqldb_measurement_transaction_rollback_index.script
	@echo $(TARGETDIR)/sqldb_measurement_transaction_rollback_mixed.script
	@echo $(TARGETDIR)/sqldb_performance_transaction_rollback.script
	@echo $(TARGETDIR)/sqldb_performance_transaction_rollback_index.script
	@echo $(TARGETDIR)/sqldb_performance_transaction_rollback_mixed.script
	@echo $(TARGETDIR)/sqldb_measurement_attachdb.script
	@echo $(TARGETDIR)/sqldb_performance_attachdb.script
	@echo $(TARGETDIR)/sqldb_measurement_apps.script
	@echo $(TARGETDIR)/sqldb_performance_apps.script
	@echo $(TARGETDIR)/sqldb_performance_apps_create.script
	@echo $(TARGETDIR)/sqldb_performance_apps_delete.script
	@echo $(TARGETDIR)/sqldb_performance_apps_modify.script
	@echo $(TARGETDIR)/sqldb_performance_apps_select.script
	@echo $(TARGETDIR)/sqldb_performance_apps_add.script
	@echo $(TARGETDIR)/testsqldb_comparec0.db
	@echo $(TARGETDIR)/testsqldb_comparec1.db
	@echo $(TARGETDIR)/testsqldb_comparec2.db
	@echo $(TARGETDIR)/testsqldb_comparec3.db
	@echo $(TARGETDIR)/testsqldb_comparef.db
	@echo $(TARGETDIR)/sqldb_testexecute.ini
	@echo $(TARGETDIR)/testsqldb_stress.ini
	@echo $(TARGETDIR)/testsqldb_stress_view.ini
