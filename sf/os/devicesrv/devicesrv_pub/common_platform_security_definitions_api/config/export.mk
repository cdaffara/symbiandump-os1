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


# common_platform_security_definitions_api's actual configuration export makefile

MAKEFILE = 	$(EPOCROOT)/sf/os/devicesrv/devicesrv_pub/common_platform_security_definitions_api/config/export.mk
$(call push,MAKEFILE_STACK,$(MAKEFILE))

INCFILES =		$(MAKEFILEDIR)../inc/data_caging_path_literals.hrh    $(EPOCROOT)/epoc32/include/ \
							$(MAKEFILEDIR)../inc/data_caging_paths_for_iby.hrh    $(EPOCROOT)/epoc32/include/ \
							$(MAKEFILEDIR)../inc/data_caging_paths.hrh            $(EPOCROOT)/epoc32/include/ \
							$(MAKEFILEDIR)../inc/data_caging_paths_strings.hrh    $(EPOCROOT)/epoc32/include/ \
							$(MAKEFILEDIR)../inc/data_caged_locations.mk          $(EPOCROOT)/epoc32/include/


common_platform_security_config				:: common_platform_security_config-inc
common_platform_security_config-inc 	::

$(call addfiles, $(INCFILES), common_platform_security_config-inc)

$(call popout,MAKEFILE_STACK)                                        