# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

ifeq ($(PLATFORM),GCCXML)
MAKEDATA:
echo do nothing
else
ifeq ($(PLATFORM),TOOLS)
MAKEDATA:
echo do nothing
else
ifeq ($(PLATFORM),TOOLS2)
MAKEDATA:
echo do nothing
else
MAKEDATA:
	@echo $(PLATFORM)
	echo --------------------------------------------------------------
	echo Building Stub SIS file for cen rep
	echo --------------------------------------------------------------

	echo makesis
	makesis -s stubcentrep.pkg

endif
endif
endif


DO_NOTHING:
	rem do nothing

#
# The targets invoked by abld...
#

MAKMAKE : DO_NOTHING
FREEZE : DO_NOTHING
LIB : DO_NOTHING
RESOURCE : DO_NOTHING
CLEANLIB : DO_NOTHING
FINAL : DO_NOTHING
RELEASABLES : DO_NOTHING

SAVESPACE : BLD

BLD : MAKEDATA

CLEAN : 
	if exist %EPOCROOT%epoc32\release\"$(PLATFORM)"\"$(CFG)"\z\tups\integ\packages\ctlbsinstallabletrackingpsy.sis 	del /q %EPOCROOT%epoc32\release\"$(PLATFORM)"\"$(CFG)"\z\tups\integ\packages\ctlbsinstallabletrackingpsy.sis 


