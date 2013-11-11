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
# Invoke genkif.pl to create the Kit Information File
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

do_nothing :
	

#
# The targets invoked by abld 
#

MAKMAKE : do_nothing

RESOURCE : do_nothing

SAVESPACE : BLD

BLD : 
ifeq ($(CFG),REL)
	perl ${EPOCROOT}epoc32/tools/genkif.pl -o ${EPOCROOT}epoc32/data/kif.xml
endif

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : do_nothing

CLEAN : 
ifeq ($(CFG),REL)
	$(call remove,${EPOCROOT}epoc32/data/kif.xml)
endif

RELEASABLES : 
ifeq ($(CFG),REL)	
	@echo ${EPOCROOT}epoc32/data/kif.xml	
endif



