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
# TzLocalTestServer - makefile for TimeZoneLocalization test data
# 
#

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

MBSDIR = $(EPOCROOT)epoc32/tools/shell
include $(MBSDIR)/$(notdir $(basename $(SHELL))).mk


# Erase resources command
erase_res  = $(ERASE) $(EPOCROOT)epoc32$/release$/$(PLATFORM)$/$(CFG)$/z$/resource$/timezonelocalization
erase_data = $(ERASE) $(EPOCROOT)epoc32$/data$/z$/resource$/timezonelocalization
do_nothing : 

RESOURCE :
	-$(erase_res)$/$(TZ_RSC)
	-$(erase_data)$/$(TZ_RSC)
	-$(erase_res)$/$(TZ_R01)
	-$(erase_data)$/$(TZ_R01)
	-$(erase_res)$/$(TZ_R02)
	-$(erase_data)$/$(TZ_R02)
	-$(erase_res)$/$(TZ_GRP_RSC)
	-$(erase_data)$/$(TZ_GRP_RSC)
	-$(erase_res)$/$(TZ_GRP_R01)
	-$(erase_data)$/$(TZ_GRP_R01)
	-$(erase_res)$/$(TZ_GRP_R02)
	-$(erase_data)$/$(TZ_GTP_R02)

BLD : do_nothing

MAKMAKE :  do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

CLEAN : do_nothing

FINAL : do_nothing

SAVESPACE : do_nothing

RELEASEABLES : do_nothing

