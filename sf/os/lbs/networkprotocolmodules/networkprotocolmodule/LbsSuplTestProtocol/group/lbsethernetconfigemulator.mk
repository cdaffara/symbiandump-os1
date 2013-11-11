#
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
# lbsethernetconfigemulator.mk
#
# makefile for configuring ethernet on the emulator
#

# Shell makefile
include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

SOURCE=$(EXTENSION_ROOT)..\data\winscwethernetced.xml

do_nothing :
	rem do_nothing

ifeq ($(PLATFORM), WINSCW)
 ifdef AUTOSUPL
    CONDITIONS = TRUE
 endif
endif


ifeq ($(CONDITIONS), TRUE)
BLD: $(SOURCE)

	$(CP) "$?" "$(EPOCROOT)epoc32$/winscw$/c$/ethernetced.xml"
	perl $(EPOCROOT)epoc32$/tools$/configchange.pl --config ethernetWithCommDB --kernel EKA2 --target $(PLATFORM) --variant $(CFG)

else
# Ethernet for ARMV5 is not configured here
BLD: do_nothing

endif

MAKMAKE SAVESPACE FREEZE LIB CLEANLIB RELEASABLES FINAL CLEAN RESOURCE : do_nothing

