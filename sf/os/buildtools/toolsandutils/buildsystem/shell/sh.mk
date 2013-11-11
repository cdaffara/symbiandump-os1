# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

include $(EPOCROOT)epoc32/tools/shell/generic.mk


# Some tools do not work with slash but only dollar slash.  
# That is actually a defect for them and should be fixed.  
# Then this macro should be abandoned.
define slash2generic
$(1)
endef

# Use $(MKDIR) to create directory
# Use in base/config.mk and many others
define createdir
$(MKDIR) -p $(1) 
endef

# Check if not exist directory then create it first. 
# Use in BASE/lubbock_miniboot and many others
define ifnotexistd
if [ ! -d $(1) ]; then $(MKDIR) -p $(1); fi
endef

# This means there are something to add for SBSv2 here.
# Used in base/lubbock_miniboot and should be used in similar situation.
define sbsadd
$(1)
endef

# Add double quotes for abld target. No quote for SBSv2.
# Used in base/lubbock_miniboot
define abldquote
$(1)
endef

# Used in Syslibs/conversiontable.mk
define generated
$(EPOCROOT)epoc32/build/$(1)
endef

# Used in syslibs/generate_cpp.mk
define formatreleasables
$(if $(1),@echo $(word 1,$(1)))
$(if $(1),$(call formatreleasables,$(wordlist 2,$(words $(1)),$(1))))
endef

# Used in BASE/config.mk
define generatedcpp
$(EPOCROOT)epoc32/build/generatedcpp/hal
endef

# Set path. Used in BASE/bootstrap.mk
define epocbldabs
$(EPOCBLD)
endef

# Copy. Used in BASE/bootstrap.mk 
define ecopy
cp
endef

# Abld does not support pipe symbol | while SBSv2 does.  So for Abld it is nothing.
# Used in Base/bootstrap.mk. 
define pipe
| $(1)
endef

# Used in BASE/copy_default.mk.
define cpfeature
$(CP) $? $@
endef

# Used in Syslibs/generate_cpp.mk at the bottom to deal with different
# way of includeing TEM in Abld and SBSv2.
define include
$(TEMPLATE_EXTENSION_MAKEFILE)
endef

# Macro to change working directory. Used for TOOLS/stlport.mk
# The path needs to be fixed before passing to cd command
define chdir
-cd $(1)
endef

# Macro to remove files. Used in TOOLS/stlport.mk
define remove
-rm -f $(1)
endef

# Macro to copy files. Needed for sbsv2 build
# Used in PDS components in syslibs/*.mk
define forcecopy
$(CP) -f $(1)	$(2) && chmod a+rwx $(2)
endef

# Macro to remove files. Needed for sbsv2 build
# Used in PDS components in syslibs/*.mk
define forceremove
-$(ERASE) -f $(1)
endef

# Configuration needs to be returned as upper case for abld
CONFIGURATION:=rel
