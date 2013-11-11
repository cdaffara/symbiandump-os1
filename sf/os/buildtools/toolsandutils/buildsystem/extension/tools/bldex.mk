# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Generate some source files
# Note that the YACC and LEX tools used expect to see Unix-style
# path names and will hang horribly if given DOS pathnames
# 
#

FIXSLASHES:=$(subst \,/,$(EPOCROOT))
EPOCROOTABS:=$(patsubst %/,%,$(FIXSLASHES))
EPOCROOT:=$(EPOCROOTABS)/

YACC_VERSION:=$(EPOCROOT)epoc32/gcc/bin/bison
LEX_VERSION:=$(EPOCROOT)epoc32/gcc/bin/flex

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

LEXSOURCE:=$(EXTENSION_ROOT)/$(LEXSOURCE)
YACCSOURCE:=$(EXTENSION_ROOT)/$(YACCSOURCE)

GENERATED_FILES= \
	$(EPOCROOT)epoc32/build/rcomp/build/rcomp.cpp \
	$(EPOCROOT)epoc32/build/rcomp/build/rcomp.hpp \
	$(EPOCROOT)epoc32/build/rcomp/build/rcompl.cpp

$(EPOCROOT)epoc32/build/rcomp/build/rcompl.cpp : $(LEXSOURCE)
	$(call createdir,$(EPOCROOT)epoc32/build/rcomp/build)
	$(LEX_VERSION) -t $< > $@

$(EPOCROOT)epoc32/build/rcomp/build/rcomp.cpp $(EPOCROOT)epoc32/build/rcomp/build/rcomp.hpp : $(YACCSOURCE)
	$(call createdir,$(EPOCROOT)epoc32/build/rcomp/build)
	$(YACC_VERSION) -d -o $@ $<

do_nothing:
	

#
# The targets invoked by bld...
#

# Do the work in the MAKMAKE target, in the hope of getting the files
# created in time to scan them in the processing of RCOMP.MMP

MAKMAKE : $(GENERATED_FILES)

BLD : MAKMAKE

SAVESPACE : MAKMAKE

CLEAN : 
	$(call remove,$(GENERATED_FILES))

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FINAL : do_nothing

RELEASABLES : do_nothing

