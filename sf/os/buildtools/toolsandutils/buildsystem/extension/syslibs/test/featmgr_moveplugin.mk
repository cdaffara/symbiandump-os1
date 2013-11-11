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

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	EPOCDATADIR:=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
else
	EPOCDATADIR:=$(EPOCROOT)epoc32/data
endif

BINSOURCEDIR:=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
RESOURCESOURCEDIR:=$(EPOCDATADIR)/z/resource/plugins

FILE1:=normal_plugin.dll
FILE2:=hanging_plugin.dll
FILE3:=corrupt_plugin.dll
FILE4:=reconciliation_plugin.dll
FILE5:=ab_normal_plugin.dll
FILE6:=bc_enhanced_plugin.dll
FILE7:=slowstart_plugin.dll
FILE8:=invalid_plugin.dll
FILE9:=normal_plugin.rsc
FILE10:=hanging_plugin.rsc
FILE11:=corrupt_plugin.rsc
FILE12:=reconciliation_plugin.rsc
FILE13:=ab_normal_plugin.rsc
FILE14:=bc_enhanced_plugin.rsc
FILE15:=slowstart_plugin.rsc
FILE16:=invalid_plugin.rsc

TARGETDIR:=$(EPOCDATADIR)/z/test/efm/plugins
# we copy the normal plugin files to the resource folder on C: drive for the plugin ignoring test
PLUGINTARGETDIR:=$(EPOCROOT)epoc32/winscw/c/sys/bin
RESOURCETARGETDIR:=$(EPOCROOT)epoc32/winscw/c/resource/plugins

$(TARGETDIR) :
	$(call createdir,"$@")

$(PLUGINTARGETDIR) :
	$(call createdir,"$@")

$(RESOURCETARGETDIR) :
	$(call createdir,"$@")

COPYFILES : $(TARGETDIR) $(PLUGINTARGETDIR) $(RESOURCETARGETDIR) 
	$(call forcecopy,$(BINSOURCEDIR)/$(FILE1),$(TARGETDIR)/$(FILE1))
	$(call forcecopy,$(BINSOURCEDIR)/$(FILE1),$(PLUGINTARGETDIR)/$(FILE1))
	$(call forcecopy,$(BINSOURCEDIR)/$(FILE2),$(TARGETDIR)/$(FILE2))
	$(call forcecopy,$(BINSOURCEDIR)/$(FILE3),$(TARGETDIR)/$(FILE3))
	$(call forcecopy,$(BINSOURCEDIR)/$(FILE4),$(TARGETDIR)/$(FILE4))
	$(call forcecopy,$(BINSOURCEDIR)/$(FILE5),$(TARGETDIR)/$(FILE5))
	$(call forcecopy,$(BINSOURCEDIR)/$(FILE6),$(TARGETDIR)/$(FILE6))
	$(call forcecopy,$(BINSOURCEDIR)/$(FILE7),$(TARGETDIR)/$(FILE7))
	$(call forcecopy,$(BINSOURCEDIR)/$(FILE8),$(TARGETDIR)/$(FILE8))
	$(call forcecopy,$(RESOURCESOURCEDIR)/$(FILE9),$(TARGETDIR)/$(FILE9))
	$(call forcecopy,$(RESOURCESOURCEDIR)/$(FILE9),$(RESOURCETARGETDIR)/$(FILE9))
	$(call forcecopy,$(RESOURCESOURCEDIR)/$(FILE10),$(TARGETDIR)/$(FILE10))
	$(call forcecopy,$(RESOURCESOURCEDIR)/$(FILE11),$(TARGETDIR)/$(FILE11))
	$(call forcecopy,$(RESOURCESOURCEDIR)/$(FILE12),$(TARGETDIR)/$(FILE12))
	$(call forcecopy,$(RESOURCESOURCEDIR)/$(FILE13),$(TARGETDIR)/$(FILE13))
	$(call forcecopy,$(RESOURCESOURCEDIR)/$(FILE14),$(TARGETDIR)/$(FILE14))
	$(call forcecopy,$(RESOURCESOURCEDIR)/$(FILE15),$(TARGETDIR)/$(FILE15))
	$(call forcecopy,$(RESOURCESOURCEDIR)/$(FILE16),$(TARGETDIR)/$(FILE16))

ERASEFILES : $(call slash2generic,$(foreach FILE, $(FILE1) $(FILE2) $(FILE3) $(FILE4) $(FILE5) $(FILE6) $(FILE7) $(FILE8), $(TARGETDIR)/$(FILE)) $(PLUGINTARGETDIR)/$(FILE1))
	$(call forceremove,$(BINSOURCEDIR)/$(FILE1))
	$(call forceremove,$(BINSOURCEDIR)/$(FILE2))
	$(call forceremove,$(BINSOURCEDIR)/$(FILE3))
	$(call forceremove,$(BINSOURCEDIR)/$(FILE4))
	$(call forceremove,$(BINSOURCEDIR)/$(FILE5))
	$(call forceremove,$(BINSOURCEDIR)/$(FILE6))
	$(call forceremove,$(BINSOURCEDIR)/$(FILE7))
	$(call forceremove,$(BINSOURCEDIR)/$(FILE8))
	
DO_NOTHING:
	@echo do nothing

#
# The targets invoked by bld...
#

BLD : DO_NOTHING

CLEAN :
	$(call forceremove,$(TARGETDIR)/$(FILE1))
	$(call forceremove,$(PLUGINTARGETDIR)/$(FILE1))
	$(call forceremove,$(TARGETDIR)/$(FILE2))
	$(call forceremove,$(TARGETDIR)/$(FILE3))
	$(call forceremove,$(TARGETDIR)/$(FILE4))
	$(call forceremove,$(TARGETDIR)/$(FILE5))
	$(call forceremove,$(TARGETDIR)/$(FILE6))
	$(call forceremove,$(TARGETDIR)/$(FILE7))
	$(call forceremove,$(TARGETDIR)/$(FILE8))
	$(call forceremove,$(TARGETDIR)/$(FILE9))
	$(call forceremove,$(RESOURCETARGETDIR)/$(FILE9))
	$(call forceremove,$(TARGETDIR)/$(FILE10))
	$(call forceremove,$(TARGETDIR)/$(FILE11))
	$(call forceremove,$(TARGETDIR)/$(FILE12))
	$(call forceremove,$(TARGETDIR)/$(FILE13))
	$(call forceremove,$(TARGETDIR)/$(FILE14))
	$(call forceremove,$(TARGETDIR)/$(FILE15))
	$(call forceremove,$(TARGETDIR)/$(FILE16))
	$(call forceremove,$(RESOURCESOURCEDIR)/$(FILE9))
	$(call forceremove,$(RESOURCESOURCEDIR)/$(FILE10))
	$(call forceremove,$(RESOURCESOURCEDIR)/$(FILE11))
	$(call forceremove,$(RESOURCESOURCEDIR)/$(FILE12))
	$(call forceremove,$(RESOURCESOURCEDIR)/$(FILE13))
	$(call forceremove,$(RESOURCESOURCEDIR)/$(FILE14))
	$(call forceremove,$(RESOURCESOURCEDIR)/$(FILE15))
	$(call forceremove,$(RESOURCESOURCEDIR)/$(FILE16))

RELEASABLES :
	@echo $(TARGETDIR)/$(FILE1)
	@echo $(PLUGINTARGETDIR)/$(FILE1)
	@echo $(TARGETDIR)/$(FILE2)
	@echo $(TARGETDIR)/$(FILE3)
	@echo $(TARGETDIR)/$(FILE4)
	@echo $(TARGETDIR)/$(FILE5)
	@echo $(TARGETDIR)/$(FILE6)
	@echo $(TARGETDIR)/$(FILE7)
	@echo $(TARGETDIR)/$(FILE8)
	@echo $(TARGETDIR)/$(FILE9)
	@echo $(RESOURCETARGETDIR)/$(FILE9)
	@echo $(TARGETDIR)/$(FILE10)
	@echo $(TARGETDIR)/$(FILE11)
	@echo $(TARGETDIR)/$(FILE12)
	@echo $(TARGETDIR)/$(FILE13)
	@echo $(TARGETDIR)/$(FILE14)
	@echo $(TARGETDIR)/$(FILE15)
	@echo $(TARGETDIR)/$(FILE16)

MAKMAKE : DO_NOTHING

SAVESPACE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FREEZE : DO_NOTHING

RESOURCE : DO_NOTHING

FINAL : COPYFILES ERASEFILES

ROMFILE : DO_NOTHING
