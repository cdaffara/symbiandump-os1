# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

do_nothing:

MAKMAKE : do_nothing

FREEZE : do_nothing

LIB :
	unzip -o $(FILE) -d $(TODIR)

CLEANLIB : do_nothing

RESOURCE : do_nothing

FINAL : do_nothing

BLD : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
		@perl -S $(EPOCROOT)epoc32/tools/listzip.pl $(TODIR) $(FILE)

CLEAN : do_nothing

