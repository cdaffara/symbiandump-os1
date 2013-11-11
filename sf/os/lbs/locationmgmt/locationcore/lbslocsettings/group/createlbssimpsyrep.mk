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
# createlbssimpsyrep.mk
#
# makefile for creating the binary version of the LbsAdmin cenrep file
#

TARGET_REP_UID=101F500C

TARGET=$(EPOCROOT)epoc32\winscw\c\private\10202be9\$(TARGET_REP_UID).cre

SOURCE=$(EXTENSION_ROOT)..\data\$(TARGET_REP_UID).txt

$(TARGET) : $(SOURCE)
	@copy "$?" "$(EPOCROOT)epoc32\winscw\c\$(TARGET_REP_UID).txt"
	@$(EPOCROOT)epoc32\release\winscw\udeb\centrepconv -nowait -o c:\$(TARGET_REP_UID).cre c:\$(TARGET_REP_UID).txt
	@copy "$(EPOCROOT)epoc32\winscw\c\$(TARGET_REP_UID).cre" "$@"
	@erase "$(EPOCROOT)epoc32\winscw\c\$(TARGET_REP_UID).*"

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : $(TARGET)

SAVESPACE : BLD

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

CLEAN :
	@erase $(TARGET)

RELEASABLES :
	@echo $(TARGET)

FINAL : do_nothing
