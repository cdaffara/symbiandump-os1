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
	DEST_DIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/system/xmltest/syncml
else
	DEST_DIR = $(EPOCROOT)epoc32/data/z/system/xmltest/syncml
endif

SOURCE_DIR = $(EXTENSION_ROOT)/../test/rtest/data/syncml

# Ensure we have a clean canvas - this also avoids us having to specify
# switches to commands that are not recognised across platforms.
CLEAN_DEST :
	$(call createdir,$(DEST_DIR))

MAKE_DIRS :
	$(call createdir,$(DEST_DIR)/1.1/add-to-client)
	$(call createdir,$(DEST_DIR)/1.1/add-to-server)
	$(call createdir,$(DEST_DIR)/1.1/Atomic)
	$(call createdir,$(DEST_DIR)/1.1/AuthBasicFail)
	$(call createdir,$(DEST_DIR)/1.1/AuthBasicFailFirst)
	$(call createdir,$(DEST_DIR)/1.1/AuthMD5Fail)
	$(call createdir,$(DEST_DIR)/1.1/AuthMD5FailFirst)
	$(call createdir,$(DEST_DIR)/1.1/client-large)
	$(call createdir,$(DEST_DIR)/1.1/client-large-multiple)
	$(call createdir,$(DEST_DIR)/1.1/DevManAdd)
	$(call createdir,$(DEST_DIR)/1.1/DevManAlertDisplay)
	$(call createdir,$(DEST_DIR)/1.1/DevManAlertMultiChoice)
	$(call createdir,$(DEST_DIR)/1.1/DevManAlertSingleChoice)
	$(call createdir,$(DEST_DIR)/1.1/DevManAlertTextInput)
	$(call createdir,$(DEST_DIR)/1.1/DevManAlertUserAccept)
	$(call createdir,$(DEST_DIR)/1.1/DevManAlertUserReject)
	$(call createdir,$(DEST_DIR)/1.1/DevManAtomic)
	$(call createdir,$(DEST_DIR)/1.1/DevManAtomicAlertUserAccept)
	$(call createdir,$(DEST_DIR)/1.1/DevManAtomicAlertUserReject)
	$(call createdir,$(DEST_DIR)/1.1/DevManAtomicFail)
	$(call createdir,$(DEST_DIR)/1.1/DevManDelete)
	$(call createdir,$(DEST_DIR)/1.1/DevManGet)
	$(call createdir,$(DEST_DIR)/1.1/DevManLargeObjectAdd)
	$(call createdir,$(DEST_DIR)/1.1/DevManLargeObjectGet)
	$(call createdir,$(DEST_DIR)/1.1/DevManReplace)
	$(call createdir,$(DEST_DIR)/1.1/DevManSequence)
	$(call createdir,$(DEST_DIR)/1.1/DevManSequenceAlertUserAccept)
	$(call createdir,$(DEST_DIR)/1.1/DevManSequenceAlertUserReject)
	$(call createdir,$(DEST_DIR)/1.1/DevManSequenceFail)
	$(call createdir,$(DEST_DIR)/1.1/DevManSimple)
	$(call createdir,$(DEST_DIR)/1.1/Large-object-from-client)
	$(call createdir,$(DEST_DIR)/1.1/Large-object-from-server)
	$(call createdir,$(DEST_DIR)/1.1/Large-object-from-server2)
	$(call createdir,$(DEST_DIR)/1.1/Multiple-Db-Sync)
	$(call createdir,$(DEST_DIR)/1.1/One-way-client-refresh-sync)
	$(call createdir,$(DEST_DIR)/1.1/One-way-client-sync)
	$(call createdir,$(DEST_DIR)/1.1/One-way-server-refresh-sync)
	$(call createdir,$(DEST_DIR)/1.1/One-way-server-sync)
	$(call createdir,$(DEST_DIR)/1.1/Pref-Tx-Rx)
	$(call createdir,$(DEST_DIR)/1.1/server-busy)
	$(call createdir,$(DEST_DIR)/1.1/server-large)
	$(call createdir,$(DEST_DIR)/1.1/server-large-multiple)
	$(call createdir,$(DEST_DIR)/1.1/slow-sync)
	$(call createdir,$(DEST_DIR)/1.1/two-way-add)
	$(call createdir,$(DEST_DIR)/1.1/two-way-delete)
	$(call createdir,$(DEST_DIR)/1.1/two-way-replace)
	$(call createdir,$(DEST_DIR)/1.1/two-way-sync)
	$(call createdir,$(DEST_DIR)/1.2/defects)
	$(call createdir,$(DEST_DIR)/unknown)


COPYFILES : CLEAN_DEST MAKE_DIRS
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/add-to-client/*.xml $(DEST_DIR)/1.1/add-to-client/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/add-to-server/*.xml $(DEST_DIR)/1.1/add-to-server/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/atomic/*.xml $(DEST_DIR)/1.1/Atomic/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/authbasicfail/*.xml $(DEST_DIR)/1.1/AuthBasicFail/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/authbasicfailfirst/*.xml $(DEST_DIR)/1.1/AuthBasicFailFirst/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/authmd5fail/*.xml $(DEST_DIR)/1.1/AuthMD5Fail/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/authmd5failfirst/*.xml $(DEST_DIR)/1.1/AuthMD5FailFirst/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/client-large/*.xml $(DEST_DIR)/1.1/client-large/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/client-large-multiple/*.xml $(DEST_DIR)/1.1/client-large-multiple/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanadd/*.xml $(DEST_DIR)/1.1/DevManAdd/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertdisplay/*.xml $(DEST_DIR)/1.1/DevManAlertDisplay/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertmultichoice/*.xml $(DEST_DIR)/1.1/DevManAlertMultiChoice/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertsinglechoice/*.xml $(DEST_DIR)/1.1/DevManAlertSingleChoice/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalerttextinput/*.xml $(DEST_DIR)/1.1/DevManAlertTextInput/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertuseraccept/*.xml $(DEST_DIR)/1.1/DevManAlertUserAccept/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertuserreject/*.xml $(DEST_DIR)/1.1/DevManAlertUserReject/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanatomic/*.xml $(DEST_DIR)/1.1/DevManAtomic/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanatomicalertuseraccept/*.xml $(DEST_DIR)/1.1/DevManAtomicAlertUserAccept/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanatomicalertuserreject/*.xml $(DEST_DIR)/1.1/DevManAtomicAlertUserReject/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanatomicfail/*.xml $(DEST_DIR)/1.1/DevManAtomicFail/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmandelete/*.xml $(DEST_DIR)/1.1/DevManDelete/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanget/*.xml $(DEST_DIR)/1.1/DevManGet/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanlargeobjectadd/*.xml $(DEST_DIR)/1.1/DevManLargeObjectAdd/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanlargeobjectget/*.xml $(DEST_DIR)/1.1/DevManLargeObjectGet/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanreplace/*.xml $(DEST_DIR)/1.1/DevManReplace/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansequence/*.xml $(DEST_DIR)/1.1/DevManSequence/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansequencealertuseraccept/*.xml $(DEST_DIR)/1.1/DevManSequenceAlertUserAccept/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansequencealertuserreject/*.xml $(DEST_DIR)/1.1/DevManSequenceAlertUserReject/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansequencefail/*.xml $(DEST_DIR)/1.1/DevManSequenceFail/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansimple/*.xml $(DEST_DIR)/1.1/DevManSimple/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/large-object-from-client/*.xml $(DEST_DIR)/1.1/Large-object-from-client/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/large-object-from-server/*.xml $(DEST_DIR)/1.1/Large-object-from-server/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/large-object-from-server2/*.xml $(DEST_DIR)/1.1/Large-object-from-server2/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/multiple-db-sync/*.xml $(DEST_DIR)/1.1/Multiple-Db-Sync/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/one-way-client-refresh-sync/*.xml $(DEST_DIR)/1.1/One-way-client-refresh-sync/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/one-way-client-sync/*.xml $(DEST_DIR)/1.1/One-way-client-sync/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/one-way-server-refresh-sync/*.xml $(DEST_DIR)/1.1/One-way-server-refresh-sync/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/one-way-server-sync/*.xml $(DEST_DIR)/1.1/One-way-server-sync/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/pref-tx-rx/*.xml $(DEST_DIR)/1.1/Pref-Tx-Rx/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/server-busy/*.xml $(DEST_DIR)/1.1/server-busy/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/server-large/*.xml $(DEST_DIR)/1.1/server-large/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/server-large-multiple/*.xml $(DEST_DIR)/1.1/server-large-multiple/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/slow-sync/*.xml $(DEST_DIR)/1.1/slow-sync/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/two-way-add/*.xml $(DEST_DIR)/1.1/two-way-add/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/two-way-delete/*.xml $(DEST_DIR)/1.1/two-way-delete/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/two-way-replace/*.xml $(DEST_DIR)/1.1/two-way-replace/*.xml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/two-way-sync/*.xml $(DEST_DIR)/1.1/two-way-sync/*.xml)

	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/add-to-client/*.wbxml $(DEST_DIR)/1.1/add-to-client/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/add-to-server/*.wbxml $(DEST_DIR)/1.1/add-to-server/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/atomic/*.wbxml $(DEST_DIR)/1.1/Atomic/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/authbasicfail/*.wbxml $(DEST_DIR)/1.1/AuthBasicFail/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/authbasicfailfirst/*.wbxml $(DEST_DIR)/1.1/AuthBasicFailFirst/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)$/1.1$/authmd5fail$/*.wbxml $(DEST_DIR)$/1.1$/AuthMD5Fail$/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/authmd5failfirst/*.wbxml $(DEST_DIR)/1.1/AuthMD5FailFirst/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/client-large/*.wbxml $(DEST_DIR)/1.1/client-large/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/client-large-multiple/*.wbxml $(DEST_DIR)/1.1/client-large-multiple/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanadd/*.wbxml $(DEST_DIR)/1.1/DevManAdd/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertdisplay/*.wbxml $(DEST_DIR)/1.1/DevManAlertDisplay/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertmultichoice/*.wbxml $(DEST_DIR)/1.1/DevManAlertMultiChoice/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertsinglechoice/*.wbxml $(DEST_DIR)/1.1/DevManAlertSingleChoice/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalerttextinput/*.wbxml $(DEST_DIR)/1.1/DevManAlertTextInput/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertuseraccept/*.wbxml $(DEST_DIR)/1.1/DevManAlertUserAccept/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanalertuserreject/*.wbxml $(DEST_DIR)/1.1/DevManAlertUserReject/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanatomic/*.wbxml $(DEST_DIR)/1.1/DevManAtomic/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanatomicalertuseraccept/*.wbxml $(DEST_DIR)/1.1/DevManAtomicAlertUserAccept/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanatomicalertuserreject/*.wbxml $(DEST_DIR)/1.1/DevManAtomicAlertUserReject/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanatomicfail/*.wbxml $(DEST_DIR)/1.1/DevManAtomicFail/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmandelete/*.wbxml $(DEST_DIR)/1.1/DevManDelete/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanget/*.wbxml $(DEST_DIR)/1.1/DevManGet/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanlargeobjectadd/*.wbxml $(DEST_DIR)/1.1/DevManLargeObjectAdd/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanlargeobjectget/*.wbxml $(DEST_DIR)/1.1/DevManLargeObjectGet/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmanreplace/*.wbxml $(DEST_DIR)/1.1/DevManReplace/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansequence/*.wbxml $(DEST_DIR)/1.1/DevManSequence/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansequencealertuseraccept/*.wbxml $(DEST_DIR)/1.1/DevManSequenceAlertUserAccept/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansequencealertuserreject/*.wbxml $(DEST_DIR)/1.1/DevManSequenceAlertUserReject/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansequencefail/*.wbxml $(DEST_DIR)/1.1/DevManSequenceFail/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/devmansimple/*.wbxml	 $(DEST_DIR)/1.1/DevManSimple/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/large-object-from-client/*.wbxml $(DEST_DIR)/1.1/Large-object-from-client/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/large-object-from-server/*.wbxml $(DEST_DIR)/1.1/Large-object-from-server/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/large-object-from-server2/*.wbxml $(DEST_DIR)/1.1/Large-object-from-server2/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/multiple-db-sync/*.wbxml $(DEST_DIR)/1.1/Multiple-Db-Sync/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/one-way-client-refresh-sync/*.wbxml $(DEST_DIR)/1.1/One-way-client-refresh-sync/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/one-way-client-sync/*.wbxml $(DEST_DIR)/1.1/One-way-client-sync/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/one-way-server-refresh-sync/*.wbxml $(DEST_DIR)/1.1/One-way-server-refresh-sync/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/one-way-server-sync/*.wbxml $(DEST_DIR)/1.1/One-way-server-sync/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/pref-tx-rx/*.wbxml $(DEST_DIR)/1.1/Pref-Tx-Rx/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/server-busy/*.wbxml $(DEST_DIR)/1.1/server-busy/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/server-large/*.wbxml $(DEST_DIR)/1.1/server-large/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/server-large-multiple/*.wbxml $(DEST_DIR)/1.1/server-large-multiple/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/slow-sync/*.wbxml $(DEST_DIR)/1.1/slow-sync/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/two-way-add/*.wbxml $(DEST_DIR)/1.1/two-way-add/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/two-way-delete/*.wbxml $(DEST_DIR)/1.1/two-way-delete/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/two-way-replace/*.wbxml $(DEST_DIR)/1.1/two-way-replace/*.wbxml)
	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.1/two-way-sync/*.wbxml $(DEST_DIR)/1.1/two-way-sync/*.wbxml)

	$(CP) $(call slash2generic,$(SOURCE_DIR)/1.2/defects/*.wbxml $(DEST_DIR)/1.2/defects/*.wbxml)
	
	$(CP) $(call slash2generic,$(SOURCE_DIR)/unknown/*.wbxml $(DEST_DIR)/unknown/*.wbxml)

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
	@echo $(DEST_DIR)/1.1/add-to-client/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/add-to-client/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/add-to-client/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/add-to-server/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/add-to-server/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/add-to-server/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/atomic/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/atomic/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/atomic/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/authbasicfail/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/authbasicfail/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/authbasicfail/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/authbasicfailfirst/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/authbasicfailfirst/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/authbasicfailfirst/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/authmd5fail/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/authmd5fail/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/authmd5fail/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/authmd5failfirst/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/authmd5failfirst/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/authmd5failfirst/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/client-large/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/client-large/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/client-large/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/client-large/FromServer4.wbxml
	@echo $(DEST_DIR)/1.1/client-large-multiple/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/client-large-multiple/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/client-large-multiple/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/client-large-multiple/FromServer4.wbxml
	@echo $(DEST_DIR)/1.1/devmanadd/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanadd/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertdisplay/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertdisplay/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertmultichoice/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertmultichoice/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertsinglechoice/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertsinglechoice/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanalerttextinput/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanalerttextinput/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertuseraccept/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertuseraccept/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertuserreject/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanalertuserreject/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanatomic/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanatomic/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanatomicAlertUserAccept/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanatomicAlertUserAccept/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanatomicAlertUserReject/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanatomicAlertUserReject/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanatomicFail/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanatomicFail/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmandelete/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmandelete/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanget/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanget/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanlargeobjectadd/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanlargeobjectadd/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanlargeobjectadd/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/devmanlargeobjectadd/FromServer4.wbxml
	@echo $(DEST_DIR)/1.1/devmanlargeobjectget/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanlargeobjectget/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmanlargeobjectget/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/devmanlargeobjectget/FromServer4.wbxml
	@echo $(DEST_DIR)/1.1/devmanlargeobjectget/FromServer5.wbxml
	@echo $(DEST_DIR)/1.1/devmanreplace/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmanreplace/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmansequence/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmansequence/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmansequencealertuseraccept/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmansequencealertuseraccept/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmansequencealertuserreject/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmansequencealertuserreject/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmansequencefail/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/devmansequencefail/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/devmansimple/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-client/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-client/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-client/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-client/FromServer4.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-server/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-server/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-server/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-server/FromServer4.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-server2/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-server2/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-server2/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/large-object-from-server2/FromServer4.wbxml
	@echo $(DEST_DIR)/1.1/multiple-db-sync/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/multiple-db-sync/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/multiple-db-sync/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/one-way-client-refresh-sync/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/one-way-client-refresh-sync/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/one-way-client-refresh-sync/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/one-way-client-sync/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/one-way-client-sync/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/one-way-client-sync/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/one-way-server-refresh-sync/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/one-way-server-refresh-sync/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/one-way-server-refresh-sync/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/one-way-server-sync/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/one-way-server-sync/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/one-way-server-sync/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/pref-tx-rx/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/pref-tx-rx/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/pref-tx-rx/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/server-busy/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/server-large/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/server-large/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/server-large/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/server-large/FromServer4.wbxml
	@echo $(DEST_DIR)/1.1/server-large-multiple/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/server-large-multiple/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/server-large-multiple/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/server-large-multiple/FromServer4.wbxml
	@echo $(DEST_DIR)/1.1/server-large-multiple/FromServer5.wbxml
	@echo $(DEST_DIR)/1.1/slow-sync/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/slow-sync/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/slow-sync/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/two-way-add/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/two-way-add/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/two-way-add/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/two-way-delete/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/two-way-delete/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/two-way-delete/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/two-way-replace/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/two-way-replace/FromServer2.wbxml
	@echo $(DEST_DIR)/1.1/two-way-replace/FromServer3.wbxml
	@echo $(DEST_DIR)/1.1/two-way-sync/FromServer1.wbxml
	@echo $(DEST_DIR)/1.1/two-way-sync/FromServer2.wbxml
	@echo $(DEST_DIR)/1.2/defects/DEF063979.wbxml
	@echo $(DEST_DIR)/1.2/defects/DEF066185.wbxml
	@echo $(DEST_DIR)/1.2/defects/DEF078987_1.wbxml
	@echo $(DEST_DIR)/1.2/defects/DEF078987_2.wbxml
	@echo $(DEST_DIR)/1.2/defects/DEF078668.wbxml
	@echo $(DEST_DIR)/unknown/DEF078987_3.wbxml
