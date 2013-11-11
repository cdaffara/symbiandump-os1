#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
# Description:  ?Description
#
#


# platform_path_definitions_api's actual configuration export makefile

MAKEFILE = 	$(EPOCROOT)/sf/os/devicesrv/devicesrv_pub/platform_path_definitions_api/config/export.mk
$(call push,MAKEFILE_STACK,$(MAKEFILE))

INCFILES =		$(MAKEFILEDIR)../inc/platform_paths.hrh               $(EPOCROOT)/epoc32/include/ \
							$(MAKEFILEDIR)../inc/non_foundation_paths.hrh         $(EPOCROOT)/epoc32/include/ \
							$(MAKEFILEDIR)../inc/iconlocations.hrh                $(EPOCROOT)/epoc32/include/
		
platform_path_config				:: platform_path_config-inc
platform_path_config-inc 		::

$(call addfiles, $(INCFILES), platform_path_config-inc)

$(call popout,MAKEFILE_STACK)                                        