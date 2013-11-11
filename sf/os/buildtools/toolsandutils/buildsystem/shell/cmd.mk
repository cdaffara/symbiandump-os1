# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

# Host platform dependant variables
/:=$(shell echo \)
;:=;
CP=copy
RM=del
RMDIR=rmdir
MKDIR=mkdir
ERASE=@erase /q 2>>nul


# Some tools do not work with slash but only dollar slash.  
# That is actually a defect for them and should be fixed.  
# Then this macro should be abandoned.
define slash2generic
$(subst /,$/,$(1))
endef

# Call perl script to create directory. 
# Used in base/config.mk and many others
define createdir
perl $(EPOCROOT)epoc32/tools/emkdir.pl $(1)
endef

# Check if not exist directory then create it first. 
# Used in BASE/lubbock_miniboot
define ifnotexistd
if not exist $(1) md $(subst /,\,$(1))
endef

# This means there are something to add for SBSv2 here.
# For abld, there is nothing.
# Used in base/lubbock_miniboot and should be used in similar situation.
define sbsadd

endef

# Add double quotes for abld target. No quote for SBSv2.
# Used in base/lubbock_miniboot
define abldquote
"$(1)"
endef

# Used in Syslibs/conversiontable.mk
define generated
$(EPOCROOT)epoc32/build/$(1)
endef

# Used in syslibs/generate_cpp.mk
define formatreleasables
@echo $(1)
endef

# Used in BASE/config.mk
define generatedcpp
$(EPOCROOT)epoc32/build/generatedcpp/hal
endef

# Set path. Used in BASE/bootstrap.mk
define epocbldabs
$(1)$(2)
endef

# Call perl script ecopyfile.pl to copy.  
# Used in BASE/bootstrap.mk 
define ecopy
perl $(EPOCROOT)epoc32/tools/ecopyfile.pl 
endef

# Abld does not support pipe symbol | while SBSv2 does.  So for Abld it is nothing.
# Used in Base/bootstrap.mk. 
define pipe

endef

# Call perl script copyfeaturevariants.pl.  Used in BASE/copy_default.mk.
define cpfeature
perl $(EPOCROOT)epoc32/tools/copyfeaturevariants.pl $(1) $(2)
endef

# Used in Syslibs/generate_cpp.mk at the bottom to deal with different
# way of includeing TEM in Abld and SBSv2.
define include
$(EPOCROOT)epoc32/tools/makefile_templates/syslibs/generate_cpp.mk
endef

# Macro to change working directory. Used for TOOLS/stlport.mk
# The path needs to be fixed before passing to cd command
define chdir
-cd $(subst /,\,$(1))
endef

# Macro to remove files. All paths need to be corrected before deleting.
# Used in TOOLS/stlport.mk
define remove
-$(ERASE) $(subst /,\,$(1))
endef

# Macro to copy files. Needed for sbsv2 build 
# Used in PDS components in syslibs/*.mk
define forcecopy
$(CP) $(subst /,\,$(1)) $(subst /,\,$(2))
endef

# Macro to remove files. Needed for sbsv2 build
# Used in PDS components in syslibs/*.mk
define forceremove
-$(ERASE) $(subst /,\,$(1))
endef

define tmpmacro
$(call chdir,$(BUILD_DIR));configure.bat -c gcc;
endef

define settPath
1
endef

# Configuration needs to be returned as upper case for abld
CONFIGURATION:=REL
