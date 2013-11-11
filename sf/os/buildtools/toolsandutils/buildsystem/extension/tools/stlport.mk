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

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


SOURCE_ARCHIVE:=$(EXTENSION_ROOT)/$(SOURCE_ARCHIVE)

ISPATH:=$(call settPath)

ifeq "$(ISPATH)" "1"
Path:=$(EPOCROOT)epoc32/gcc_mingw/bin;$(Path)
PATH:=$(Path)
endif


EXTRACT_DIR=$(EPOCROOT)epoc32/build
STL_DIR=$(EXTRACT_DIR)/STLport-$(STLPORT_VERSION)
BUILD_DIR=$(STL_DIR)/build/lib
UNZIPPED=$(STL_DIR)/unzipped.ok

RELEASE_DIR=$(EPOCROOT)epoc32/release/tools2
RELEASE_DIR_REL=$(RELEASE_DIR)/rel
RELEASE_DIR_DEB=$(RELEASE_DIR)/deb
RELEASE_DIRS=$(RELEASE_DIR_REL) $(RELEASE_DIR_DEB)

STL_REL_LIB_PATH=$(STL_DIR)/lib/libstlport.a
STL_DEB_LIB_PATH=$(STL_DIR)/lib/libstlportg.a

REL_LIB=$(RELEASE_DIR_REL)/$(STL_REL_LIB_NAME)
DEB_LIB=$(RELEASE_DIR_DEB)/$(STL_REL_LIB_NAME)

STL_HEADERS_PATH=$(EPOCROOT)epoc32/include/tools/stlport


$(UNZIPPED): $(SOURCE_ARCHIVE)  $(EXTRACT_DIR)
	unzip -o $(SOURCE_ARCHIVE) -d $(EXTRACT_DIR)
	

$(REL_LIB): $(UNZIPPED) $(STL_REL_LIB_PATH)  $(RELEASE_DIR_REL)
	$(call ecopy) $(STL_REL_LIB_PATH) $(REL_LIB)

$(DEB_LIB): $(UNZIPPED) $(STL_DEB_LIB_PATH)  $(RELEASE_DIR_DEB)
	$(call ecopy) $(STL_DEB_LIB_PATH) $(DEB_LIB)

LIB: $(REL_LIB) $(DEB_LIB)


$(EXTRACT_DIR) $(RELEASE_DIRS):
	$(call createdir,$@)

# -Wno-uninitialized is needed by mingw gcc v3.4.5 because it gets
#  # confused by some of the STLport source and gives this error erroneously
export EXTRA_CXXFLAGS=-Wno-uninitialized

$(STL_REL_LIB_PATH) (STL_DEB_LIB_PATH):
	$(call tmpmacro)
	make -C $(BUILD_DIR) -fgcc.mak install-release-static install-dbg-static LIB_SUFFIX=
	
CLEAN:
	$(call remove,$(REL_LIB))
	$(call remove,$(DEB_LIB))
	make -C $(BUILD_DIR) -fgcc.mak clobber LIB_SUFFIX=
	$(call remove,$(STL_DIR))


RELEASABLES:
	@echo $(REL_LIB)
	@echo $(DEB_LIB)
	

MAKMAKE BLD SAVESPACE FREEZE CLEANLIB RESOURCE FINAL:
	@echo $@ does nothing in $(TEMPLATE_EXTENSION_MAKEFILE)


