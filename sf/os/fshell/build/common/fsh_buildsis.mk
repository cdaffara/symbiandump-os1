# fsh_buildsis.mk
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
FINAL :
ifneq ($(PLATFORM), WINSCW)
	..\..\tools\fsh-buildsis -u $(PLATFORM) $(EPOCROOT)epoc32\rom\include\fshell.iby $(EPOCROOT)epoc32\fshell\fshell.sis
endif

RELEASABLES :
ifneq ($(PLATFORM), WINSCW)
	@echo $(EPOCROOT)epoc32\fshell\fshell.unsigned.sis
endif

MAKMAKE BLD FREEZE LIB CLEANLIB RESOURCE CLEAN SAVESPACE : 
