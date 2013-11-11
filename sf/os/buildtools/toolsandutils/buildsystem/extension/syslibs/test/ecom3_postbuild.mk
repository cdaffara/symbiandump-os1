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

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
TARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z
RAMONLYTARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/ramonly
SOURCEDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
SOURCEDIR2=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/plugins
else
TARGETDIR=$(EPOCROOT)epoc32/data/z
RAMONLYTARGETDIR=$(EPOCROOT)epoc32/data/z/ramonly
SOURCEDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
SOURCEDIR2=$(EPOCROOT)epoc32/data/z/resource/plugins
endif

DO_NOTHING:
	@echo do_nothing
	
$(RAMONLYTARGETDIR) :
	$(call createdir,"$@")

COPYFILES :
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/HeapTestImpl.dll $(TARGETDIR)/heaptestimpl.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/HeapTestImpl.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample5.dll $(RAMONLYTARGETDIR)/ecomexample5.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample5.dll $(RAMONLYTARGETDIR)/invalidsidplugin.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExample5.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample12.dll $(RAMONLYTARGETDIR)/ecomexample12.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExample12.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample12Upgraded.dll $(RAMONLYTARGETDIR)/ecomexample12Upgraded.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExample12Upgraded.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample12Downgraded.dll $(RAMONLYTARGETDIR)/ecomexample12Downgraded.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExample12Downgraded.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample14.dll $(RAMONLYTARGETDIR)/ecomexample14.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExample14.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample14Upgraded.dll $(RAMONLYTARGETDIR)/ecomexample14Upgraded.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExample14Upgraded.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample15.dll $(RAMONLYTARGETDIR)/ecomexample15.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExample15.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExampleBadData.dll	$(RAMONLYTARGETDIR)/ecomexamplebaddata.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExampleBadData.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExampleBadData1.dll $(RAMONLYTARGETDIR)/ecomexamplebaddata1.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExampleBadData1.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExampleBadData2.dll	$(RAMONLYTARGETDIR)/ecomexamplebaddata2.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExampleBadData2.dll)
				$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample3.dll $(RAMONLYTARGETDIR)/ecomexample3.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample2.dll $(RAMONLYTARGETDIR)/ecomexample2.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample.dll $(RAMONLYTARGETDIR)/ecomexample.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComNullExample.dll $(RAMONLYTARGETDIR)/ecomnullexample.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComRomOnlyExampleOnC.dll $(RAMONLYTARGETDIR)/ecomromonlyexampleonc.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComRomOnlyExampleOnC.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComRomRslvrExampleOnC.dll $(RAMONLYTARGETDIR)/ecomromrslvrexampleonc.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComRomRslvrExampleOnC.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComRomRslvrExampleOnZ.dll $(RAMONLYTARGETDIR)/ecomromrslvrexampleonz.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/T_PlatSecResolverC.dll $(RAMONLYTARGETDIR)/t_platsecresolverc.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/T_PlatSecResolverC.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/T_PlatSecEcom4.dll $(RAMONLYTARGETDIR)/t_platsececom4.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/T_PlatSecEcom4.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EcomUpgradeExample1.dll $(RAMONLYTARGETDIR)/ecomupgradeexample1.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EcomUpgradeExample1.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EcomUpgradeExample2.dll $(RAMONLYTARGETDIR)/ecomupgradeexample2.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EcomUpgradeExample3.dll $(RAMONLYTARGETDIR)/ecomupgradeexample3.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EcomUpgradeExample3.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EcomUpgradeROExample1.dll $(RAMONLYTARGETDIR)/ecomupgraderoexample1.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EcomUpgradeROExample1.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EcomUpgradeROExample2.dll $(RAMONLYTARGETDIR)/ecomupgraderoexample2.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EcomUpgradeROExample3.dll $(RAMONLYTARGETDIR)/ecomupgraderoexample3.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EcomUpgradeROExample3.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/DefectPlugin.dll $(RAMONLYTARGETDIR)/defectplugin.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/DefectPlugin.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample7.dll $(RAMONLYTARGETDIR)/ecomexample7.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExample7.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComExample8.dll $(RAMONLYTARGETDIR)/ecomexample8.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComExample8.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComCR629Example1.dll	$(RAMONLYTARGETDIR)/ecomcr629example1.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComCR629Example1.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComCR629Example2.dll	$(RAMONLYTARGETDIR)/ecomcr629example2.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComCR629Example2.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/LanguagePlugin.dll $(RAMONLYTARGETDIR)/languageplugin.dll)
		-$(ERASE) $(call slash2generic $(SOURCEDIR)/LanguagePlugin.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComHashExample.dll $(RAMONLYTARGETDIR)/ecomhashexample.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComHashExample.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/EComSwiExample.dll $(RAMONLYTARGETDIR)/ecomswiexample.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/EComSwiExample.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/exampleNine.dll $(RAMONLYTARGETDIR)/examplenine.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/exampleNine.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR)/dummycustomresolver1.dll $(RAMONLYTARGETDIR)/dummycustomresolver1.dll)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR)/dummycustomresolver1.dll)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/HeapTestImpl.rsc $(TARGETDIR)/heaptestimpl.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/HeapTestImpl.rsc $(RAMONLYTARGETDIR)/invalidsidplugin.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/HeapTestImpl.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample5.RSC $(RAMONLYTARGETDIR)/ecomexample5.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExample5.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample12.RSC $(RAMONLYTARGETDIR)/ecomexample12.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExample12.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample12Upgraded.RSC $(RAMONLYTARGETDIR)/ecomexample12Upgraded.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExample12Upgraded.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample12Downgraded.RSC $(RAMONLYTARGETDIR)/ecomexample12Downgraded.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExample12Downgraded.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample14.RSC $(RAMONLYTARGETDIR)/ecomexample14.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExample14.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample14Upgraded.RSC $(RAMONLYTARGETDIR)/ecomexample14Upgraded.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExample14Upgraded.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample15.RSC $(RAMONLYTARGETDIR)/ecomexample15.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExample15.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExampleBadData.RSC $(RAMONLYTARGETDIR)/ecomexamplebaddata.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExampleBadData.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExampleBadData1.RSC $(RAMONLYTARGETDIR)/ecomexamplebaddata1.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExampleBadData1.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExampleBadData2.RSC $(RAMONLYTARGETDIR)/ecomexamplebaddata2.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComExampleBadData2.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample3.RSC $(RAMONLYTARGETDIR)/ecomexample3.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample2.RSC $(RAMONLYTARGETDIR)/ecomexample2.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample.RSC $(RAMONLYTARGETDIR)/ecomexample.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComRomOnlyExampleOnC.RSC $(RAMONLYTARGETDIR)/ecomromonlyexampleonc.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComRomOnlyExampleOnC.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComRomRslvrExampleOnC.RSC $(RAMONLYTARGETDIR)/ecomromrslvrexampleonc.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComRomRslvrExampleOnC.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComRomRslvrExampleOnZ.RSC $(RAMONLYTARGETDIR)/ecomromrslvrexampleonz.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/T_PlatSecResolverC.RSC $(RAMONLYTARGETDIR)/t_platsecresolverc.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/T_PlatSecResolverC.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/T_PlatSecEcom4.RSC $(RAMONLYTARGETDIR)/t_platsececom4.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/T_PlatSecEcom4.RSC)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EcomNullExample.RSC $(RAMONLYTARGETDIR)/ecomnullexample.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeExample1.rsc $(RAMONLYTARGETDIR)/ecomupgradeexample1.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeExample1.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeExample2.rsc $(RAMONLYTARGETDIR)/ecomupgradeexample2.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeExample3.rsc $(RAMONLYTARGETDIR)/ecomupgradeexample3.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeExample3.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeROExample1.rsc $(RAMONLYTARGETDIR)/ecomupgraderoexample1.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeROExample1.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeROExample2.rsc $(RAMONLYTARGETDIR)/ecomupgraderoexample2.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeROExample3.rsc $(RAMONLYTARGETDIR)/ecomupgraderoexample3.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EcomUpgradeROExample3.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/DefectPlugin.rsc $(RAMONLYTARGETDIR)/defectplugin.rsc)
		-$(ERASE) $(SOURCEDIR2)/DefectPlugin.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample7.rsc $(RAMONLYTARGETDIR)/ecomexample7.rsc)
		-$(ERASE) $(SOURCEDIR2)/EComExample7.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComExample8.rsc $(RAMONLYTARGETDIR)/ecomexample8.rsc)
		-$(ERASE) $(SOURCEDIR2)/EComExample8.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComCR629Example1.rsc	$(RAMONLYTARGETDIR)/ecomcr629example1.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComCR629Example1.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComCR629Example2.rsc	$(RAMONLYTARGETDIR)/ecomcr629example2.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComCR629Example2.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/LanguagePlugin.rsc $(RAMONLYTARGETDIR)/languageplugin.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/LanguagePlugin.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/LanguagePlugin.r01 $(RAMONLYTARGETDIR)/languageplugin.r01)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/LanguagePlugin.r01)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/LanguagePlugin.r02 $(RAMONLYTARGETDIR)/languageplugin.r02)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/LanguagePlugin.r02)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/LanguagePlugin.r03 $(RAMONLYTARGETDIR)/languageplugin.r03)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/LanguagePlugin.r03)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComHashExample.rsc $(RAMONLYTARGETDIR)/ecomhashexample.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComHashExample.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/EComSwiExample.rsc $(RAMONLYTARGETDIR)/ecomswiexample.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/EComSwiExample.rsc)
		$(CP) /B $(call slash2generic, $(SOURCEDIR2)/dummycustomresolver1.rsc $(RAMONLYTARGETDIR)/dummycustomresolver1.rsc)
		-$(ERASE) $(call slash2generic, $(SOURCEDIR2)/dummycustomresolver1.rsc)
		
#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

BLD : $(RAMONLYTARGETDIR) COPYFILES

CLEAN : 
		-$(ERASE) $(call slash2generic, $(TARGETDIR)/heaptestimpl.dll)
		-$(ERASE) $(call slash2generic, $(TARGETDIR)/heaptestimpl.rsc )
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample5.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample12.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample12Upgraded.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample12Downgraded.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample14.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample14Upgraded.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample15.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexamplebaddata.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexamplebaddata1.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexamplebaddata2.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample3.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample2.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample.rsc )
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomromonlyexampleonc.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomromrslvrexampleonc.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomromrslvrexampleonz.rsc)  
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample5.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample12.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample12Upgraded.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample12Downgraded.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample14.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample14Upgraded.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample15.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexamplebaddata.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexamplebaddata1.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexamplebaddata2.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample3.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample2.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomromonlyexampleonc.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomromrslvrexampleonc.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomromrslvrexampleonz.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/invalidsidplugin.rsc) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/invalidsidplugin.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/t_platsecresolverc.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/t_platsecresolverc.rsc )
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/t_platsececom4.dll )
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/t_platsececom4.rsc)
		

		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomnullexample.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomnullexample.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgradeexample1.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgradeexample1.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgradeexample2.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgradeexample2.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgradeexample3.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgradeexample3.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgraderoexample1.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgraderoexample1.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgraderoexample2.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgraderoexample2.rsc) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgraderoexample3.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomupgraderoexample3.rsc) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/defectplugin.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/defectplugin.rsc) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample7.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample7.rsc )
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample8.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomexample8.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomcr629example1.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomcr629example1.rsc) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomcr629example2.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomcr629example2.rsc) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/languageplugin.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/languageplugin.rsc) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/languageplugin.r01) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/languageplugin.r02) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/languageplugin.r03) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomhashexample.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomhashexample.rsc) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomswiexample.dll) 
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/ecomswiexample.rsc)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/examplenine.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/dummycustomresolver1.dll)
		-$(ERASE) $(call slash2generic, $(RAMONLYTARGETDIR)/dummycustomresolver1.rsc)

SAVESPACE : DO_NOTHING

RESOURCE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING

RELEASABLES :
		@echo $(TARGETDIR)/heaptestimpl.dll
		@echo $(TARGETDIR)/heaptestimpl.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample5.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample12.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample12Upgraded.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample12Downgraded.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample14.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample14Upgraded.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample15.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexamplebaddata.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexamplebaddata1.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexamplebaddata2.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample3.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample2.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample.rsc 
		@echo $(RAMONLYTARGETDIR)/ecomromonlyexampleonc.rsc
		@echo $(RAMONLYTARGETDIR)/ecomromrslvrexampleonc.rsc
		@echo $(RAMONLYTARGETDIR)/ecomromrslvrexampleonz.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample5.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample12.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample12Upgraded.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample12Downgraded.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample14.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample14Upgraded.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample15.dll
		@echo $(RAMONLYTARGETDIR)/ecomexamplebaddata.dll
		@echo $(RAMONLYTARGETDIR)/ecomexamplebaddata1.dll
		@echo $(RAMONLYTARGETDIR)/ecomexamplebaddata2.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample3.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample2.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample.dll
		@echo $(RAMONLYTARGETDIR)/ecomromonlyexampleonc.dll
		@echo $(RAMONLYTARGETDIR)/ecomromrslvrexampleonc.dll
		@echo $(RAMONLYTARGETDIR)/ecomromrslvrexampleonz.dll
		@echo $(RAMONLYTARGETDIR)/invalidsidplugin.rsc 
		@echo $(RAMONLYTARGETDIR)/invalidsidplugin.dll 
		@echo $(RAMONLYTARGETDIR)/t_platsecresolverc.dll
		@echo $(RAMONLYTARGETDIR)/t_platsecresolverc.rsc 
		@echo $(RAMONLYTARGETDIR)/t_platsececom4.dll
		@echo $(RAMONLYTARGETDIR)/t_platsececom4.rsc
		@echo $(RAMONLYTARGETDIR)/ecomnullexample.dll
		@echo $(RAMONLYTARGETDIR)/ecomnullexample.rsc
		@echo $(RAMONLYTARGETDIR)/ecomupgradeexample1.dll
		@echo $(RAMONLYTARGETDIR)/ecomupgradeexample1.rsc
		@echo $(RAMONLYTARGETDIR)/ecomupgradeexample2.dll
		@echo $(RAMONLYTARGETDIR)/ecomupgradeexample2.rsc
		@echo $(RAMONLYTARGETDIR)/ecomupgradeexample3.dll
		@echo $(RAMONLYTARGETDIR)/ecomupgradeexample3.rsc
		@echo $(RAMONLYTARGETDIR)/ecomupgraderoexample1.dll
		@echo $(RAMONLYTARGETDIR)/ecomupgraderoexample1.rsc
		@echo $(RAMONLYTARGETDIR)/ecomupgraderoexample2.dll
		@echo $(RAMONLYTARGETDIR)/ecomupgraderoexample2.rsc
		@echo $(RAMONLYTARGETDIR)/ecomupgraderoexample3.dll
		@echo $(RAMONLYTARGETDIR)/ecomupgraderoexample3.rsc
		@echo $(RAMONLYTARGETDIR)/defectplugin.dll
		@echo $(RAMONLYTARGETDIR)/defectplugin.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample7.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample7.rsc
		@echo $(RAMONLYTARGETDIR)/ecomexample8.dll
		@echo $(RAMONLYTARGETDIR)/ecomexample8.rsc
		@echo $(RAMONLYTARGETDIR)/ecomcr629example1.dll
		@echo $(RAMONLYTARGETDIR)/ecomcr629example1.rsc
		@echo $(RAMONLYTARGETDIR)/ecomcr629example2.dll
		@echo $(RAMONLYTARGETDIR)/ecomcr629example2.rsc
		@echo $(RAMONLYTARGETDIR)/languageplugin.dll
		@echo $(RAMONLYTARGETDIR)/languageplugin.rsc
		@echo $(RAMONLYTARGETDIR)/languageplugin.r01
		@echo $(RAMONLYTARGETDIR)/languageplugin.r02
		@echo $(RAMONLYTARGETDIR)/languageplugin.r03
		@echo $(RAMONLYTARGETDIR)/ecomhashexample.dll
		@echo $(RAMONLYTARGETDIR)/ecomhashexample.rsc
		@echo $(RAMONLYTARGETDIR)/ecomswiexample.dll
		@echo $(RAMONLYTARGETDIR)/ecomswiexample.rsc
		@echo $(RAMONLYTARGETDIR)/examplenine.dll
		@echo $(RAMONLYTARGETDIR)/dummycustomresolver1.dll
		@echo $(RAMONLYTARGETDIR)/dummycustomresolver1.rsc
