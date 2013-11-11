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
# Copy test files to test sortbytable functionality.
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
TARGET_DIRECTORY=$(EPOCROOT)epoc32/release/$(PLATFORM)/$(CFG)/z/system/data
else
TARGET_DIRECTORY=$(EPOCROOT)epoc32/data/z/system/data
endif

SOURCEDIR=$(EXTENSION_ROOT)/../tsrc

$(TARGET_DIRECTORY) :
	$(call createdir,"$@")

TARGET_FILES=\
	$(TARGET_DIRECTORY)/t_compressed_unicode_1.rsc $(EPOCROOT)epoc32\include\T_COMPRESSED_UNICODE_1.RSG \
	$(TARGET_DIRECTORY)/t_compressed_unicode_2.rsc $(EPOCROOT)epoc32\include\T_COMPRESSED_UNICODE_2.RSG \
	$(TARGET_DIRECTORY)/t_dictionary_compressed_versio_of_2.rsc \
	$(TARGET_DIRECTORY)/t_calypso_test_resource_file_1.rsc \
	$(TARGET_DIRECTORY)/t_notrscfile.rsc \
	$(TARGET_DIRECTORY)/rscheader.bin \
	$(TARGET_DIRECTORY)/newrscformat.rsc \

$(TARGET_DIRECTORY)/t_compressed_unicode_1.rsc : $(SOURCEDIR)/T_COMPRESSED_UNICODE_1.RPP
	@RCOMP.EXE -s$? -o$@ -h$(EPOCROOT)epoc32/include/T_COMPRESSED_UNICODE_1.RSG -u -{0x000eb205,*}
$(EPOCROOT)epoc32/include/T_COMPRESSED_UNICODE_1.RSG : $(SOURCEDIR)/T_COMPRESSED_UNICODE_1.RPP
	@RCOMP.EXE -s$? -o$(TARGET_DIRECTORY)/t_compressed_unicode_1.rsc -h$@ -u -{0x000eb205,*}

$(TARGET_DIRECTORY)/t_compressed_unicode_2.rsc : $(SOURCEDIR)/T_COMPRESSED_UNICODE_2.RPP
	@RCOMP.EXE -s$? -o$@ -h$(EPOCROOT)epoc32/include/T_COMPRESSED_UNICODE_2.RSG -u
$(EPOCROOT)epoc32/include/T_COMPRESSED_UNICODE_2.RSG : $(SOURCEDIR)/T_COMPRESSED_UNICODE_2.RPP
	@RCOMP.EXE -s$? -o$(TARGET_DIRECTORY)/t_compressed_unicode_2.rsc -h$@ -u

$(TARGET_DIRECTORY)/t_dictionary_compressed_versio_of_2.rsc : $(SOURCEDIR)/T_GENERATE_DICTIONARY_COMPRESSED_VERSION_OF_2.PL
	@perl -w $(SOURCEDIR)/T_GENERATE_DICTIONARY_COMPRESSED_VERSION_OF_2.PL $(TARGET_DIRECTORY)/t_dictionary_compressed_versio_of_2.rsc

$(TARGET_DIRECTORY)/t_calypso_test_resource_file_1.rsc : $(SOURCEDIR)/T_CALYPSO_COMPILED_RESOURCE_FILE_1.RSC
	$(CP) $(call slash2generic,$(SOURCEDIR)/T_CALYPSO_COMPILED_RESOURCE_FILE_1.RSC $(TARGET_DIRECTORY)/t_calypso_test_resource_file_1.rsc)

$(TARGET_DIRECTORY)/t_notrscfile.rsc  : $(SOURCEDIR)/T_NotRscFile.RSC
	$(CP) $(call slash2generic,$(SOURCEDIR)/T_NotRscFile.RSC $(TARGET_DIRECTORY)/t_notrscfile.rsc)
	
$(TARGET_DIRECTORY)/rscheader.bin : 
	$(CP) $(call slash2generic,$(SOURCEDIR)/RscHeader.Bin $(TARGET_DIRECTORY)/rscheader.bin)

$(TARGET_DIRECTORY)/newrscformat.rsc :  $(SOURCEDIR)/RscHeader.Bin  $(SOURCEDIR)/RLETest.BMP $(TARGET_DIRECTORY)/TRSC.rsc
	@BMCONV.EXE  $(SOURCEDIR)/16RAMC.MBM /c16$(SOURCEDIR)/RLETest.BMP
	$(CP) $(call slash2generic,$(SOURCEDIR)/RscHeader.Bin)/b + $(call slash2generic,$(TARGET_DIRECTORY)/TRSC.rsc)/b + $(call slash2generic,$(SOURCEDIR)/16RAMC.MBM) /b $(call slash2generic,$(TARGET_DIRECTORY)/newrscformat.rsc)

COPYFILES :
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRSC_Inv1.RSC $(TARGET_DIRECTORY)/trsc_inv1.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRSC_Inv2.RSC $(TARGET_DIRECTORY)/trsc_inv2.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRSC_Inv3.RSC $(TARGET_DIRECTORY)/trsc_inv3.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRSC_Inv4.RSC $(TARGET_DIRECTORY)/trsc_inv4.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRSC_Inv5.RSC $(TARGET_DIRECTORY)/trsc_inv5.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRSC_Inv6.RSC $(TARGET_DIRECTORY)/trsc_inv6.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRSC_Inv7.RSC $(TARGET_DIRECTORY)/trsc_inv7.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRSC_Inv8.RSC $(TARGET_DIRECTORY)/trsc_inv8.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRSC_Inv9.RSC $(TARGET_DIRECTORY)/trsc_inv9.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRscCalypso_Inv10.RSC $(TARGET_DIRECTORY)/trsccalypso_inv10.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRscCalypso_Inv11.RSC $(TARGET_DIRECTORY)/trsccalypso_inv11.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRscCalypso_Inv12.RSC $(TARGET_DIRECTORY)/trsccalypso_inv12.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRscComprU_Inv13.RSC $(TARGET_DIRECTORY)/trsccompru_inv13.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRscComprU_Inv14.RSC $(TARGET_DIRECTORY)/trsccompru_inv14.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRscComprU_Inv15.RSC $(TARGET_DIRECTORY)/trsccompru_inv15.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRscCalypso_Inv16.RSC $(TARGET_DIRECTORY)/trsccalypso_inv16.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/TRscCalypso_Inv17.RSC $(TARGET_DIRECTORY)/trsccalypso_inv17.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/RscHeader.Bin)/b + $(call slash2generic,$(TARGET_DIRECTORY)/t_calypso_test_resource_file_1.rsc)/b $(call slash2generic,$(TARGET_DIRECTORY)/trscromcalypsocomprnewfmt.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/Spi_ECom.spi $(TARGET_DIRECTORY)/spi_ecom.spi)
	$(CP) $(call slash2generic,$(SOURCEDIR)/Spi_EComRsc1.RSC $(TARGET_DIRECTORY)/spi_ecomrsc1.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/Spi_EComRsc2.RSC $(TARGET_DIRECTORY)/spi_ecomrsc2.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/Spi_EComRsc3.RSC $(TARGET_DIRECTORY)/spi_ecomrsc3.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/ECom-1-0.spi $(TARGET_DIRECTORY)/ecom-1-0.spi)
	$(CP) $(call slash2generic,$(SOURCEDIR)/ECom-1-0.s02 $(TARGET_DIRECTORY)/ecom-1-0.s02)
	$(CP) $(call slash2generic,$(SOURCEDIR)/ECom-2-0.spi $(TARGET_DIRECTORY)/ecom-2-0.spi)
	$(CP) $(call slash2generic,$(SOURCEDIR)/ECom-2-0.s02 $(TARGET_DIRECTORY)/ecom-2-0.s02)
	$(CP) $(call slash2generic,$(SOURCEDIR)/Spi_ECom-0-0.spi $(TARGET_DIRECTORY)/spi_ecom-0-0.spi)
	$(CP) $(call slash2generic,$(SOURCEDIR)/Spi_ECom-1-0.spi $(TARGET_DIRECTORY)/spi_ecom-1-0.spi)
	$(CP) $(call slash2generic,$(SOURCEDIR)/CECom-0-0.spi $(TARGET_DIRECTORY)/cecom-0-0.spi)
	$(CP) $(call slash2generic,$(SOURCEDIR)/CECom-1-0.spi $(TARGET_DIRECTORY)/cecom-1-0.spi)
	$(CP) $(call slash2generic,$(SOURCEDIR)/Spi_ECom_Case-0-0.spi $(TARGET_DIRECTORY)/spi_ecom_case-0-0.spi)
	$(CP) $(call slash2generic,$(SOURCEDIR)/Spi_ECom_Case-1-0.spi $(TARGET_DIRECTORY)/spi_ecom_case-1-0.spi)


DO_NOTHING :
	@echo do nothing

# the targets below are the public ones

MAKMAKE : DO_NOTHING

BLD : $(TARGET_DIRECTORY) $(TARGET_FILES) COPYFILES

SAVESPACE : BLD

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

RESOURCE : DO_NOTHING

CLEAN : 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/t_compressed_unicode_1.rsc $(EPOCROOT)epoc32/include/t_compressed_unicode_1.rsg)
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/t_compressed_unicode_2.rsc $(EPOCROOT)epoc32/include/t_compressed_unicode_2.rsg)
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/t_dictionary_compressed_versio_of_2.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/t_calypso_test_resource_file_1.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/t_notrscfile.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/rscheader.bin) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/newrscformat.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsc_inv1.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsc_inv2.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsc_inv3.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsc_inv4.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsc_inv5.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsc_inv6.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsc_inv7.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsc_inv8.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsc_inv9.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsccalypso_inv10.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsccalypso_inv11.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsccalypso_inv12.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsccompru_inv13.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsccompru_inv14.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsccompru_inv15.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsccalypso_inv16.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trsccalypso_inv17.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/trscromcalypsocomprnewfmt.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/spi_ecom.spi) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/spi_ecomrsc1.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/spi_ecomrsc2.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/spi_ecomrsc3.rsc) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/ecom-1-0.spi)  
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/ecom-1-0.s02) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/ecom-2-0.spi) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/ecom-2-0.s02) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/spi_ecom-0-0.spi) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/spi_ecom-1-0.spi) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/cecom-0-0.spi) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/cecom-1-0.spi)
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/spi_ecom_case-0-0.spi) 
	-$(ERASE) $(call slash2generic,$(TARGET_DIRECTORY)/spi_ecom_case-1-0.spi)

RELEASABLES : 
	@echo $(TARGET_DIRECTORY)/t_compressed_unicode_1.rsc $(EPOCROOT)epoc32/include/t_compressed_unicode_1.rsg 
	@echo $(TARGET_DIRECTORY)/t_compressed_unicode_2.rsc $(EPOCROOT)epoc32/include/t_compressed_unicode_2.rsg 
	@echo $(TARGET_DIRECTORY)/t_dictionary_compressed_versio_of_2.rsc 
	@echo $(TARGET_DIRECTORY)/t_calypso_test_resource_file_1.rsc 
	@echo $(TARGET_DIRECTORY)/t_notrscfile.rsc 
	@echo $(TARGET_DIRECTORY)/rscheader.bin 
	@echo $(TARGET_DIRECTORY)/newrscformat.rsc 
	@echo $(TARGET_DIRECTORY)/trsc_inv1.rsc 
	@echo $(TARGET_DIRECTORY)/trsc_inv2.rsc 
	@echo $(TARGET_DIRECTORY)/trsc_inv3.rsc 
	@echo $(TARGET_DIRECTORY)/trsc_inv4.rsc 
	@echo $(TARGET_DIRECTORY)/trsc_inv5.rsc 
	@echo $(TARGET_DIRECTORY)/trsc_inv6.rsc 
	@echo $(TARGET_DIRECTORY)/trsc_inv7.rsc 
	@echo $(TARGET_DIRECTORY)/trsc_inv8.rsc 
	@echo $(TARGET_DIRECTORY)/trsc_inv9.rsc 
	@echo $(TARGET_DIRECTORY)/trsccalypso_inv10.rsc 
	@echo $(TARGET_DIRECTORY)/trsccalypso_inv11.rsc 
	@echo $(TARGET_DIRECTORY)/trsccalypso_inv12.rsc 
	@echo $(TARGET_DIRECTORY)/trsccompru_inv13.rsc 
	@echo $(TARGET_DIRECTORY)/trsccompru_inv14.rsc 
	@echo $(TARGET_DIRECTORY)/trsccompru_inv15.rsc 
	@echo $(TARGET_DIRECTORY)/trsccalypso_inv16.rsc 
	@echo $(TARGET_DIRECTORY)/trsccalypso_inv17.rsc 
	@echo $(TARGET_DIRECTORY)/trscromcalypsocomprnewfmt.rsc 
	@echo $(TARGET_DIRECTORY)/spi_ecom.spi 
	@echo $(TARGET_DIRECTORY)/spi_ecomrsc1.rsc 
	@echo $(TARGET_DIRECTORY)/spi_ecomrsc2.rsc 
	@echo $(TARGET_DIRECTORY)/spi_ecomrsc3.rsc 
	@echo $(TARGET_DIRECTORY)/ecom-1-0.spi  
	@echo $(TARGET_DIRECTORY)/ecom-1-0.s02 
	@echo $(TARGET_DIRECTORY)/ecom-2-0.spi 
	@echo $(TARGET_DIRECTORY)/ecom-2-0.s02 
	@echo $(TARGET_DIRECTORY)/spi_ecom-0-0.spi 
	@echo $(TARGET_DIRECTORY)/spi_ecom-1-0.spi 
	@echo $(TARGET_DIRECTORY)/cecom-0-0.spi
	@echo $(TARGET_DIRECTORY)/cecom-1-0.spi
	@echo $(TARGET_DIRECTORY)/spi_ecom_case-0-0.spi 
	@echo $(TARGET_DIRECTORY)/spi_ecom_case-1-0.spi 
	
FINAL : DO_NOTHING
