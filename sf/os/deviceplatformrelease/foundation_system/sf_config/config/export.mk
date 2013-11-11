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
# sf_config's actual configuration export makefile

MAKEFILE = 	/sf/config/sf_config/config/export.mk
$(call push,MAKEFILE_STACK,$(MAKEFILE))

REGION = western


		
INCFILES =	$(MAKEFILEDIR)inc/bldvariant.hrh                                    /epoc32/include/	\
            $(MAKEFILEDIR)inc/privateruntimeids.hrh                             /epoc32/include/ \
            $(MAKEFILEDIR)inc/publicruntimeids.hrh                              /epoc32/include/ \
            $(MAKEFILEDIR)inc/ProductVariant.hrh                                /epoc32/include/ \
            $(MAKEFILEDIR)inc/defaultcaps.hrh                                   /epoc32/include/ \
            $(MAKEFILEDIR)inc/s60_features.xml                                  /epoc32/include/ \
            $(MAKEFILEDIR)inc/bldpublic.hrh                                     /epoc32/include/ \
            $(MAKEFILEDIR)inc/bldprivate.hrh                                    /epoc32/include/ \
            $(MAKEFILEDIR)inc/bldcodeline.hrh                                   /epoc32/include/ \
            $(MAKEFILEDIR)inc/features.hrh                                      /epoc32/include/ \
            $(MAKEFILEDIR)inc/s60_features.csv                                  /epoc32/include/ \
            $(MAKEFILEDIR)inc/variant.cfg                                       /epoc32/tools/variant/ \
            $(MAKEFILEDIR)$(REGION)/inc/bldregional.hrh 				                /epoc32/include/ 
            
            
sf_config      :: sf_config-data sf_config-inc sf_config-rom
sf_config-data ::
sf_config-inc  ::
sf_config-rom  ::

$(call addfiles, $(DATAFILES), sf_config-data)
$(call addfiles, $(INCFILES), sf_config-inc)
$(call addfiles, $(ROMFILES), sf_config-rom)

$(call popout,MAKEFILE_STACK)