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
#

MAKMAKE :
	cd "$(subst /,\,$(EXTENSION_ROOT)/..)"; python python/sbs_dist.py $(RAPTOR_DIST) $(RAPTOR_DIST_INCLUDE_DIRS)

# Dummy target
DO_NOTHING :

RELEASABLES CLEAN BLD LIB SAVESPACE FREEZE CLEANLIB RESOURCE FINAL : DO_NOTHING

