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

MAKMAKE :
	@python $(EXTENSION_ROOT)/$(RAPTOR_SCRIPTS_LOC)$(SETUP_SCRIPT) py2exe $(EXTENSION_ROOT)/$(RAPTOR_SCRIPTS_LOC)$(ENTRY_POINT) $(EXTENSION_ROOT)/$(DIST_DIR)

CLEAN :
	@del $(subst /,\, $(wildcard $(EXTENSION_ROOT)\$(DIST_DIR)*.pyd))
	@del $(subst /,\, $(EXTENSION_ROOT)\$(DIST_DIR)library.zip)
	@del $(subst /,\, $(wildcard $(EXTENSION_ROOT)\$(DIST_DIR)MSVCR*.dll))
	@del $(subst /,\, $(wildcard $(EXTENSION_ROOT)\$(DIST_DIR)python*.dll))
	@del $(subst .py,.exe, $(subst /,\, $(EXTENSION_ROOT)\$(DIST_DIR)$(ENTRY_POINT)))
	@del $(subst /,\, $(EXTENSION_ROOT)\$(DIST_DIR)w9xpopen.exe)

RELEASABLES :
	@echo $(subst /,\, $(wildcard $(EXTENSION_ROOT)\$(DIST_DIR)*.pyd))
	@echo $(subst /,\, $(EXTENSION_ROOT)\$(DIST_DIR)library.zip)
	@echo $(subst /,\, $(wildcard $(EXTENSION_ROOT)\$(DIST_DIR)MSVCR*.dll))
	@echo $(subst /,\, $(wildcard $(EXTENSION_ROOT)\$(DIST_DIR)python*.dll))
	@echo $(subst .py,.exe, $(subst /,\, $(EXTENSION_ROOT)\$(DIST_DIR)$(ENTRY_POINT)))
	@echo $(subst /,\, $(EXTENSION_ROOT)\$(DIST_DIR)w9xpopen.exe)

# Dummy target
DO_NOTHING :
	
BLD LIB SAVESPACE FREEZE CLEANLIB RESOURCE FINAL : DO_NOTHING
	
