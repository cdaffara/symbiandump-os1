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
# Extension Makefile to check uiklaf generated rsg files against the generic exported uiklafgt rsg files.
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


#----------------------------------------------------------------
# Check whether the associated RSG header
# files are identical to those already exported by uiklafGT
#
#

INCDIR = $(EPOCROOT)epoc32/include
INCUIKLAFDIR = $(EPOCROOT)epoc32/include/uiklafgt

DO_NOTHING :

#
# The targets invoked by abld...
#

MAKMAKE : DO_NOTHING

RESOURCE : DO_NOTHING

SAVESPACE : DO_NOTHING

BLD : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : 
	perl -w $(EXTENSION_ROOT)/rsg_check.pl $(INCDIR)/eikcore.rsg $(INCUIKLAFDIR)/eikcore.rsg
	perl -w $(EXTENSION_ROOT)/rsg_check.pl $(INCDIR)/eikpriv.rsg $(INCUIKLAFDIR)/eikpriv.rsg


ROMFILE : DO_NOTHING

CLEAN : DO_NOTHING

RELEASABLES : DO_NOTHING

