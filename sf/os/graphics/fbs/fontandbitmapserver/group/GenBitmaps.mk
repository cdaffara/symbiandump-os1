# Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Build bitmap test files
# 
#

!if "$(PLATFORM)"=="WINS" || "$(PLATFORM)"=="WINSCW"
TARGETDIR=$(EPOCROOT)EPOC32\RELEASE\$(PLATFORM)\$(CFG)\Z\SYSTEM\DATA
TARGETDIR2=$(EPOCROOT)EPOC32\RELEASE\$(PLATFORM)\$(CFG)\Z
!else
TARGETDIR=$(EPOCROOT)EPOC32\DATA\Z\SYSTEM\DATA
TARGETDIR2=$(EPOCROOT)EPOC32\DATA\Z
!endif

do_nothing :
	rem do_nothing

L_DIR=\
		$(TARGETDIR2)\siuhsiughighifhgidfhguifhguidhfguifhghlarglaghghghghghkrjghszkjg\system\data

TARGET_FILES= \
		$(TARGETDIR)\tfbs.mbm \
		$(TARGETDIR)\testcompression.mbm \
		$(TARGETDIR)\tfbs.rbm \
		$(TARGETDIR)\8bitNoDither.mbm \
		$(TARGETDIR)\8bitDitherFs.mbm \
		$(TARGETDIR)\12bit.mbm \
		$(TARGETDIR)\16bit.mbm \
		$(TARGETDIR)\24bit.mbm \
		$(TARGETDIR)\32bit.mbm \
		$(TARGETDIR)\file.mbm \
		$(TARGETDIR)\filecomp.mbm \
		$(TARGETDIR)\rom.mbm \
		$(TARGETDIR)\romcomp.mbm \
		$(TARGETDIR)\tfbs_file1.mbm \
		$(TARGETDIR)\tfbs_file2.mbm \
		$(TARGETDIR)\tfbs_file3.mbm \
		$(TARGETDIR)\tfbs_file4.mbm \
		$(TARGETDIR)\tfbs_file4a.mbm \
		$(TARGETDIR)\tfbs_file4b.mbm \
		$(TARGETDIR)\tfbs_file4c.mbm \
		$(TARGETDIR)\tfbs_file4d.mbm \
		$(TARGETDIR)\tfbs_file4e.mbm \
		$(TARGETDIR)\tfbs_file4f.mbm \
		$(TARGETDIR)\tfbs_file4g.mbm \
		$(TARGETDIR)\tfbs_file4h.mbm \
		$(TARGETDIR)\tfbs_file5.mbm \
 		$(TARGETDIR2)\siuhsiughighifhgidfhguifhguidhfguifhghlarglaghghghghghkrjghszkjg\system\data\tfbs_file2.mbm \
		$(TARGETDIR)\compr12.mbm \
		$(TARGETDIR)\compr16.mbm \
		$(TARGETDIR)\compr24.mbm \
		$(TARGETDIR)\racompr12.mbm \
		$(TARGETDIR)\racompr16.mbm \
		$(TARGETDIR)\racompr24.mbm \
		$(TARGETDIR)\rf1.mbm \
		$(TARGETDIR)\rf2.mbm \
		$(TARGETDIR)\rf3.mbm \
		$(TARGETDIR)\rf123.mbm

TARGET_FILES2= \
 		$(TARGETDIR2)\siuhsiughighifhgidfhguifhguidhfguifhghlarglaghghghghghkrjghszkjg\system\data\tfbs_file1.mbm \
		$(TARGETDIR)\RC_ROM8.mbm \
		$(TARGETDIR)\RC_ROM12.mbm \
		$(TARGETDIR)\RC_ROM16.mbm \
		$(TARGETDIR)\RC_ROM24.mbm \
		$(TARGETDIR)\RC_RAM8.mbm \
		$(TARGETDIR)\RC_RAM12.mbm \
		$(TARGETDIR)\RC_RAM16.mbm \
		$(TARGETDIR)\RC_RAM24.mbm \
		$(TARGETDIR)\MONO_M1.mbm \
		$(TARGETDIR)\MONO_M2.mbm \
		$(TARGETDIR)\MONO_M4.mbm \
		$(TARGETDIR)\MONO_M8.mbm \
		$(TARGETDIR)\MONO_C4.mbm \
		$(TARGETDIR)\MONO_C8.mbm \
		$(TARGETDIR)\MONO_C12.mbm \
		$(TARGETDIR)\MONO_C16.mbm \
		$(TARGETDIR)\MONO_C24.mbm \
		$(TARGETDIR)\MONO_C32.mbm \
		$(TARGETDIR)\16RAM2.mbm \
		$(TARGETDIR)\16ROMC2.mbm \
		$(TARGETDIR)\RscHeader3.Bin \
		$(TARGETDIR)\DummyRscFile3.RSC \
		$(TARGETDIR)\TFBS_RSC.rsc \
		$(TARGETDIR)\blackandwhite.mbm \
# these bitmaps are used ins TSecureFBS with 3rd uid=10273364
		$(TARGETDIR2)\private\10273364\RC_ROM12.mbm \
		$(TARGETDIR2)\private\10273364\RC_RAM16.mbm \
		$(TARGETDIR2)\private\10273364\RscHeader3.bin \
		$(TARGETDIR2)\private\10273364\DummyRscFile3.RSC \
		$(TARGETDIR2)\private\10273364\TFBS_RSC.rsc \
# these additional private data cage is purposely created for testing		
		$(TARGETDIR2)\private\00999999\RC_ROM12.mbm \
		$(TARGETDIR2)\private\00999999\RC_RAM16.mbm \
# additional files for Palette Compression Testing 
		$(TARGETDIR)\24bitSkin.mbm \
		$(TARGETDIR)\16bitSkin.mbm \
		$(TARGETDIR)\24bit6col.mbm \
		$(TARGETDIR)\24bit2col.mbm \
		$(TARGETDIR)\24bit3col.mbm \
		$(TARGETDIR)\24bit20col.mbm \
		$(TARGETDIR)\16bit6col.mbm \
		$(TARGETDIR)\16bit2col.mbm \
		$(TARGETDIR)\16bit3col.mbm \
		$(TARGETDIR)\16bit20col.mbm \
		$(TARGETDIR)\32bitSkin.mbm \
		$(TARGETDIR)\32bit2col.mbm \
		$(TARGETDIR)\32bit3col.mbm \
		$(TARGETDIR)\32bit6col.mbm \
		$(TARGETDIR)\32bit20col.mbm 
		
$(L_DIR) :
	@IF NOT EXIST $(L_DIR) @MD $(L_DIR)

$(TARGETDIR)\tfbs.mbm : ..\TFBS\tfbs.mbm
		@COPY ..\TFBS\tfbs.mbm  $(TARGETDIR)\tfbs.mbm

$(TARGETDIR)\testcompression.mbm : ..\TFBS\testcompression.mbm
		@COPY ..\TFBS\testcompression.mbm  $(TARGETDIR)\testcompression.mbm

$(TARGETDIR)\tfbs.rbm : ..\TFBS\tfbs.rbm
		@COPY ..\TFBS\tfbs.rbm	$(TARGETDIR)\tfbs.rbm

$(TARGETDIR)\8bitNoDither.mbm : ..\TFBS\8bitNoDither.mbm
		@COPY ..\TFBS\8bitNoDither.mbm	$(TARGETDIR)\8bitNoDither.mbm

$(TARGETDIR)\8bitDitherFs.mbm : ..\TFBS\8bitDitherFs.mbm
		@COPY ..\TFBS\8bitDitherFs.mbm	$(TARGETDIR)\8bitDitherFs.mbm

$(TARGETDIR)\12bit.mbm : ..\TFBS\12bit.mbm
		@COPY ..\TFBS\12bit.mbm	$(TARGETDIR)\12bit.mbm

$(TARGETDIR)\16bit.mbm : ..\TFBS\16bit.mbm
		@COPY ..\TFBS\16bit.mbm	$(TARGETDIR)\16bit.mbm

$(TARGETDIR)\24bit.mbm : ..\TFBS\24bit.mbm
		@COPY ..\TFBS\24bit.mbm	$(TARGETDIR)\24bit.mbm

$(TARGETDIR)\32bit.mbm : ..\TFBS\32bit.mbm
		@COPY ..\TFBS\32bit.mbm	$(TARGETDIR)\32bit.mbm

$(TARGETDIR)\file.mbm : ..\TFBS\file.mbm
		@COPY ..\TFBS\file.mbm	$(TARGETDIR)\file.mbm

$(TARGETDIR)\rom.mbm : ..\TFBS\rom.mbm
		@COPY ..\TFBS\rom.mbm	$(TARGETDIR)\rom.mbm

$(TARGETDIR)\romcomp.mbm : ..\TFBS\romcomp.mbm
		@COPY ..\TFBS\romcomp.mbm	$(TARGETDIR)\romcomp.mbm

$(TARGETDIR)\filecomp.mbm : ..\TFBS\filecomp.mbm
		@COPY ..\TFBS\filecomp.mbm	$(TARGETDIR)\filecomp.mbm

$(TARGETDIR)\tfbs_file1.mbm : ..\TFBS\tfbs_file1.mbm
		@COPY ..\TFBS\tfbs_file1.mbm	$(TARGETDIR)\tfbs_file1.mbm

$(TARGETDIR)\tfbs_file2.mbm : ..\TFBS\tfbs_file2.mbm
		@COPY ..\TFBS\tfbs_file2.mbm	$(TARGETDIR)\tfbs_file2.mbm

$(TARGETDIR)\tfbs_file3.mbm : ..\TFBS\tfbs_file3.mbm
		@COPY ..\TFBS\tfbs_file3.mbm	$(TARGETDIR)\tfbs_file3.mbm

$(TARGETDIR)\tfbs_file4.mbm : ..\TFBS\tfbs_file4.mbm
		@COPY ..\TFBS\tfbs_file4.mbm	$(TARGETDIR)\tfbs_file4.mbm
$(TARGETDIR)\tfbs_file4a.mbm : ..\TFBS\tfbs_file4.mbm
		@COPY ..\TFBS\tfbs_file4.mbm	$(TARGETDIR)\tfbs_file4a.mbm
$(TARGETDIR)\tfbs_file4b.mbm : ..\TFBS\tfbs_file4.mbm
		@COPY ..\TFBS\tfbs_file4.mbm	$(TARGETDIR)\tfbs_file4b.mbm
$(TARGETDIR)\tfbs_file4c.mbm : ..\TFBS\tfbs_file4.mbm
		@COPY ..\TFBS\tfbs_file4.mbm	$(TARGETDIR)\tfbs_file4c.mbm
$(TARGETDIR)\tfbs_file4d.mbm : ..\TFBS\tfbs_file4.mbm
		@COPY ..\TFBS\tfbs_file4.mbm	$(TARGETDIR)\tfbs_file4d.mbm
$(TARGETDIR)\tfbs_file4e.mbm : ..\TFBS\tfbs_file4.mbm
		@COPY ..\TFBS\tfbs_file4.mbm	$(TARGETDIR)\tfbs_file4e.mbm
$(TARGETDIR)\tfbs_file4f.mbm : ..\TFBS\tfbs_file4.mbm
		@COPY ..\TFBS\tfbs_file4.mbm	$(TARGETDIR)\tfbs_file4f.mbm
$(TARGETDIR)\tfbs_file4g.mbm : ..\TFBS\tfbs_file4.mbm
		@COPY ..\TFBS\tfbs_file4.mbm	$(TARGETDIR)\tfbs_file4g.mbm
$(TARGETDIR)\tfbs_file4h.mbm : ..\TFBS\tfbs_file4.mbm
		@COPY ..\TFBS\tfbs_file4.mbm	$(TARGETDIR)\tfbs_file4h.mbm

$(TARGETDIR)\tfbs_file5.mbm : ..\TFBS\tfbs_file5.mbm
		@COPY ..\TFBS\tfbs_file5.mbm	$(TARGETDIR)\tfbs_file5.mbm

$(TARGETDIR)\compr12.mbm : ..\TFBS\compr12.mbm
		@COPY ..\TFBS\compr12.mbm	$(TARGETDIR)\compr12.mbm

$(TARGETDIR)\compr16.mbm : ..\TFBS\compr16.mbm
		@COPY ..\TFBS\compr16.mbm	$(TARGETDIR)\compr16.mbm

$(TARGETDIR)\compr24.mbm : ..\TFBS\compr24.mbm
		@COPY ..\TFBS\compr24.mbm	$(TARGETDIR)\compr24.mbm

$(TARGETDIR)\racompr12.mbm : ..\TFBS\racompr12.mbm
		@COPY ..\TFBS\racompr12.mbm	$(TARGETDIR)\racompr12.mbm

$(TARGETDIR)\racompr16.mbm : ..\TFBS\racompr16.mbm
		@COPY ..\TFBS\racompr16.mbm	$(TARGETDIR)\racompr16.mbm

$(TARGETDIR)\racompr24.mbm : ..\TFBS\racompr24.mbm
		@COPY ..\TFBS\racompr24.mbm	$(TARGETDIR)\racompr24.mbm

$(TARGETDIR2)\siuhsiughighifhgidfhguifhguidhfguifhghlarglaghghghghghkrjghszkjg\system\data\tfbs_file2.mbm : ..\TFBS\tfbs_file2.mbm
		@COPY ..\TFBS\tfbs_file2.mbm	$(TARGETDIR2)\siuhsiughighifhgidfhguifhguidhfguifhghlarglaghghghghghkrjghszkjg\system\data\tfbs_file2.mbm

$(TARGETDIR2)\siuhsiughighifhgidfhguifhguidhfguifhghlarglaghghghghghkrjghszkjg\system\data\tfbs_file1.mbm : ..\TFBS\tfbs_file1.mbm
		@COPY ..\TFBS\tfbs_file1.mbm	$(TARGETDIR2)\siuhsiughighifhgidfhguifhguidhfguifhghlarglaghghghghghkrjghszkjg\system\data\tfbs_file1.mbm

$(TARGETDIR)\RC_ROM8.mbm : ..\TFBS\RC_ROM8.mbm
		@COPY ..\TFBS\RC_ROM8.mbm	$(TARGETDIR)\RC_ROM8.mbm

$(TARGETDIR)\RC_ROM12.mbm : ..\TFBS\RC_ROM12.mbm
		@COPY ..\TFBS\RC_ROM12.mbm	$(TARGETDIR)\RC_ROM12.mbm

$(TARGETDIR)\RC_ROM16.mbm : ..\TFBS\RC_ROM16.mbm
		@COPY ..\TFBS\RC_ROM16.mbm	$(TARGETDIR)\RC_ROM16.mbm

$(TARGETDIR)\RC_ROM24.mbm : ..\TFBS\RC_ROM24.mbm
		@COPY ..\TFBS\RC_ROM24.mbm	$(TARGETDIR)\RC_ROM24.mbm

$(TARGETDIR)\RC_RAM8.mbm : ..\TFBS\RC_RAM8.mbm
		@COPY ..\TFBS\RC_RAM8.mbm	$(TARGETDIR)\RC_RAM8.mbm

$(TARGETDIR)\RC_RAM12.mbm : ..\TFBS\RC_RAM12.mbm
		@COPY ..\TFBS\RC_RAM12.mbm	$(TARGETDIR)\RC_RAM12.mbm

$(TARGETDIR)\RC_RAM16.mbm : ..\TFBS\RC_RAM16.mbm
		@COPY ..\TFBS\RC_RAM16.mbm	$(TARGETDIR)\RC_RAM16.mbm

$(TARGETDIR)\RC_RAM24.mbm : ..\TFBS\RC_RAM24.mbm
		@COPY ..\TFBS\RC_RAM24.mbm	$(TARGETDIR)\RC_RAM24.mbm

$(TARGETDIR)\MONO_M1.mbm : ..\TFBS\MONO_M1.mbm
		@COPY ..\TFBS\MONO_M1.mbm	$(TARGETDIR)\MONO_M1.mbm

$(TARGETDIR)\MONO_M2.mbm : ..\TFBS\MONO_M2.mbm
		@COPY ..\TFBS\MONO_M2.mbm	$(TARGETDIR)\MONO_M2.mbm

$(TARGETDIR)\MONO_M4.mbm : ..\TFBS\MONO_M4.mbm
		@COPY ..\TFBS\MONO_M4.mbm	$(TARGETDIR)\MONO_M4.mbm

$(TARGETDIR)\MONO_M8.mbm : ..\TFBS\MONO_M8.mbm
		@COPY ..\TFBS\MONO_M8.mbm	$(TARGETDIR)\MONO_M8.mbm

$(TARGETDIR)\MONO_C4.mbm : ..\TFBS\MONO_C4.mbm
		@COPY ..\TFBS\MONO_C4.mbm	$(TARGETDIR)\MONO_C4.mbm

$(TARGETDIR)\MONO_C8.mbm : ..\TFBS\MONO_C8.mbm
		@COPY ..\TFBS\MONO_C8.mbm	$(TARGETDIR)\MONO_C8.mbm

$(TARGETDIR)\MONO_C12.mbm : ..\TFBS\MONO_C12.mbm
		@COPY ..\TFBS\MONO_C12.mbm	$(TARGETDIR)\MONO_C12.mbm

$(TARGETDIR)\MONO_C16.mbm : ..\TFBS\MONO_C16.mbm
		@COPY ..\TFBS\MONO_C16.mbm	$(TARGETDIR)\MONO_C16.mbm

$(TARGETDIR)\MONO_C24.mbm : ..\TFBS\MONO_C24.mbm
		@COPY ..\TFBS\MONO_C24.mbm	$(TARGETDIR)\MONO_C24.mbm

$(TARGETDIR)\MONO_C32.mbm : ..\TFBS\MONO_C32.mbm
		@COPY ..\TFBS\MONO_C32.mbm	$(TARGETDIR)\MONO_C32.mbm

$(TARGETDIR)\16RAM2.mbm : ..\TFBS\16RAM2.mbm
		@COPY ..\TFBS\16RAM2.mbm	$(TARGETDIR)\16RAM2.mbm

$(TARGETDIR)\16ROMC2.mbm : ..\TFBS\16ROMC2.mbm
		@COPY ..\TFBS\16ROMC2.mbm	$(TARGETDIR)\16ROMC2.mbm

$(TARGETDIR)\RscHeader3.Bin : ..\TFBS\RscHeader3.Bin
		@copy ..\TFBS\RscHeader3.Bin			$(TARGETDIR)\RscHeader3.Bin

$(TARGETDIR)\DummyRscFile3.RSC : ..\TFBS\DummyRscFile3.RSC
		@copy ..\TFBS\DummyRscFile3.RSC			$(TARGETDIR)\DummyRscFile3.RSC

$(TARGETDIR)\TFBS_RSC.rsc : ..\TFBS\RscHeader3.Bin ..\TFBS\DummyRscFile3.RSC ..\TFBS\TFBS.MBM
		@copy ..\TFBS\RscHeader3.Bin /b + ..\TFBS\DummyRscFile3.RSC /b + ..\TFBS\TFBS.MBM /b $(TARGETDIR)\TFBS_RSC.rsc
		
$(TARGETDIR)\blackandwhite.mbm : ..\TFBS\blackandwhite.mbm
		@COPY ..\TFBS\blackandwhite.mbm $(TARGETDIR)\blackandwhite.mbm

$(TARGETDIR)\rf1.mbm : ..\TFBS\rf1.mbm
		@COPY ..\TFBS\rf1.mbm	$(TARGETDIR)\rf1.mbm

$(TARGETDIR)\rf2.mbm : ..\TFBS\rf2.mbm
		@COPY ..\TFBS\rf2.mbm	$(TARGETDIR)\rf2.mbm

$(TARGETDIR)\rf3.mbm : ..\TFBS\rf3.mbm
		@COPY ..\TFBS\rf3.mbm	$(TARGETDIR)\rf3.mbm

$(TARGETDIR)\rf123.mbm : ..\TFBS\rf123.mbm
		@COPY ..\TFBS\rf123.mbm	$(TARGETDIR)\rf123.mbm

#-------------------- Used by TSecureFBS test--------------------------------
$(TARGETDIR2)\private\10273364\RC_ROM12.mbm : ..\TFBS\RC_ROM12.mbm
		perl -S emkdir.pl $(TARGETDIR2)\private\10273364
		@COPY ..\TFBS\RC_ROM12.mbm	$(TARGETDIR2)\private\10273364\RC_ROM12.mbm

$(TARGETDIR2)\private\10273364\RC_RAM16.mbm : ..\TFBS\RC_RAM16.mbm
		@COPY ..\TFBS\RC_RAM16.mbm	$(TARGETDIR2)\private\10273364\RC_RAM16.mbm

$(TARGETDIR2)\private\00999999\RC_ROM12.mbm : ..\TFBS\RC_ROM12.mbm
		perl -S emkdir.pl $(TARGETDIR2)\private\00999999
		@COPY ..\TFBS\RC_ROM12.mbm	$(TARGETDIR2)\private\00999999\RC_ROM12.mbm

$(TARGETDIR2)\private\00999999\RC_RAM16.mbm : ..\TFBS\RC_RAM16.mbm
		@COPY ..\TFBS\RC_RAM16.mbm	$(TARGETDIR2)\private\00999999\RC_RAM16.mbm

$(TARGETDIR2)\private\10273364\RscHeader3.Bin : ..\TFBS\RscHeader3.Bin
		@copy ..\TFBS\RscHeader3.Bin			$(TARGETDIR2)\private\10273364\RscHeader3.Bin

$(TARGETDIR2)\private\10273364\DummyRscFile3.RSC : ..\TFBS\DummyRscFile3.RSC
		@copy ..\TFBS\DummyRscFile3.RSC			$(TARGETDIR2)\private\10273364\DummyRscFile3.RSC

$(TARGETDIR2)\private\10273364\TFBS_RSC.rsc : ..\TFBS\RscHeader3.Bin ..\TFBS\DummyRscFile3.RSC ..\TFBS\TFBS.MBM
		@copy ..\TFBS\RscHeader3.Bin /b + ..\TFBS\DummyRscFile3.RSC /b + ..\TFBS\TFBS.MBM /b $(TARGETDIR2)\private\10273364\TFBS_RSC.rsc
#------------------------------------------------------------------------------

# used for Palette compression testing----

$(TARGETDIR)\24bitSkin.mbm :  ..\TFBS\24bitSkin.mbm
	@copy ..\TFBS\24bitSkin.mbm $(TARGETDIR)\24bitSkin.mbm

$(TARGETDIR)\16bitSkin.mbm : ..\TFBS\16bitSkin.mbm
	@copy ..\TFBS\16bitSkin.mbm $(TARGETDIR)\16bitSkin.mbm

$(TARGETDIR)\24bit6col.mbm : ..\TFBS\24bit6col.mbm
	@copy ..\TFBS\24bit6col.mbm $(TARGETDIR)\24bit6col.mbm 

$(TARGETDIR)\24bit2col.mbm : ..\TFBS\24bit2col.mbm
	@copy ..\TFBS\24bit2col.mbm $(TARGETDIR)\24bit2col.mbm 

$(TARGETDIR)\24bit3col.mbm : ..\TFBS\24bit3col.mbm
	@copy ..\TFBS\24bit3col.mbm $(TARGETDIR)\24bit3col.mbm 

$(TARGETDIR)\24bit20col.mbm : ..\TFBS\24bit20col.mbm
	@copy ..\TFBS\24bit20col.mbm $(TARGETDIR)\24bit20col.mbm 

$(TARGETDIR)\16bit6col.mbm : ..\TFBS\16bit6col.mbm
	@copy ..\TFBS\16bit6col.mbm $(TARGETDIR)\16bit6col.mbm 

$(TARGETDIR)\16bit2col.mbm : ..\TFBS\16bit2col.mbm
	@copy ..\TFBS\16bit2col.mbm $(TARGETDIR)\16bit2col.mbm 

$(TARGETDIR)\16bit3col.mbm : ..\TFBS\16bit3col.mbm
	@copy ..\TFBS\16bit3col.mbm $(TARGETDIR)\16bit3col.mbm 

$(TARGETDIR)\16bit20col.mbm : ..\TFBS\16bit20col.mbm
	@copy ..\TFBS\16bit20col.mbm $(TARGETDIR)\16bit20col.mbm 

$(TARGETDIR)\32bitSkin.mbm : ..\TFBS\32bit6col.mbm
	@copy ..\TFBS\32bit6col.mbm $(TARGETDIR)\32bitSkin.mbm 

$(TARGETDIR)\32bit2col.mbm : ..\TFBS\32bit2col.mbm
	@copy ..\TFBS\32bit2col.mbm $(TARGETDIR)\32bit2col.mbm 

$(TARGETDIR)\32bit3col.mbm : ..\TFBS\32bit3col.mbm
	@copy ..\TFBS\32bit3col.mbm $(TARGETDIR)\32bit3col.mbm 

$(TARGETDIR)\32bit6col.mbm : ..\TFBS\32bit6col.mbm
	@copy ..\TFBS\32bit6col.mbm $(TARGETDIR)\32bit6col.mbm 

$(TARGETDIR)\32bit20col.mbm : ..\TFBS\32bit20col.mbm
	@copy ..\TFBS\32bit20col.mbm $(TARGETDIR)\32bit20col.mbm 

#------------------------------------------------------------------------------

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

BLD : $(L_DIR) $(TARGET_FILES) $(TARGET_FILES2) 

CLEAN : 
	-@erase $(TARGET_FILES)
	-@erase $(TARGET_FILES2)

SAVESPACE : do_nothing

RESOURCE : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : do_nothing

RELEASABLES :
	@echo $(TARGET_FILES)
	@echo $(TARGET_FILES2)
