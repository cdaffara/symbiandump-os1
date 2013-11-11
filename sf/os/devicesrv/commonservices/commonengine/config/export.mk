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
# Description:
#


# commonengine's actual configuration export makefile

MAKEFILE = 	$(EPOCROOT)/sf/os/devicesrv/commonservices/commonengine/config/export.mk
$(call push,MAKEFILE_STACK,$(MAKEFILE))

CONFFILES =		$(MAKEFILEDIR)../conf/commonengine.confml              	$(EPOCROOT)/epoc32/rom/config/confml_data/s60/ \
			$(MAKEFILEDIR)../conf/ci_commonengine.confml            $(EPOCROOT)/epoc32/rom/config/confml_data/s60/ \
							$(MAKEFILEDIR)../conf/commonengine_1020503B.crml 	     	$(EPOCROOT)/epoc32/rom/config/confml_data/s60/
		
commonengine_config					:: commonengine_config-conf
commonengine_config-conf 		::

$(call addfiles, $(CONFFILES), commonengine_config-conf)

$(call popout,MAKEFILE_STACK)                                        