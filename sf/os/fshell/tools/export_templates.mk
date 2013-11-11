# export_templates.mk
# 
# Copyright (c) 2010 Accenture. All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Accenture - Initial contribution
#
BLD :
ifeq ($(PLATFORM), WINSCW)
	.\exportall .\createsrc-templates $(EPOCROOT)epoc32\tools\createsrc-templates
endif

RELEASABLES :
ifeq ($(PLATFORM), WINSCW)
	@.\exportall -w .\createsrc-templates $(EPOCROOT)epoc32\tools\createsrc-templates
endif

CLEAN :
ifeq ($(PLATFORM), WINSCW)
	.\exportall -c .\createsrc-templates $(EPOCROOT)epoc32\tools\createsrc-templates
endif

MAKMAKE FINAL FREEZE LIB CLEANLIB RESOURCE SAVESPACE : 