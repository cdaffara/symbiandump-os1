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
!else
TARGETDIR=$(EPOCROOT)EPOC32\DATA\Z\SYSTEM\DATA
!endif

do_nothing :
	rem do_nothing

TARGET_FILES= \
		$(TARGETDIR)\16ROMC.MBM \
		$(TARGETDIR)\16ROM.MBM \
		$(TARGETDIR)\16RAMC.MBM \
		$(TARGETDIR)\16RAM.MBM \
		$(TARGETDIR)\paint16romc.mbm \
		$(TARGETDIR)\paint16ramc.mbm \
		$(TARGETDIR)\paint24romc.mbm \
		$(TARGETDIR)\paint24ramc.mbm \
		$(TARGETDIR)\8romc.mbm \
		$(TARGETDIR)\8ramc.mbm \
		$(TARGETDIR)\12romc.mbm \
		$(TARGETDIR)\12ramc.mbm \
		$(TARGETDIR)\BmCTest.mbm \
		$(TARGETDIR)\BitBltSrc.mbm \
		$(TARGETDIR)\BitBltMasked.mbm \
		$(TARGETDIR)\tbmp.MBM \
		$(TARGETDIR)\tile.MBM \
		$(TARGETDIR)\Corrupt000.WBMP \
		$(TARGETDIR)\ZeroSizeRam.MBM \
		$(TARGETDIR)\Compr12_2.mbm \
		$(TARGETDIR)\Compr16_2.mbm \
		$(TARGETDIR)\Compr24_2.mbm \
		$(TARGETDIR)\RaCompr12_2.mbm \
		$(TARGETDIR)\RaCompr16_2.mbm \
		$(TARGETDIR)\RaCompr24_2.mbm \
		$(TARGETDIR)\24ROMC.MBM \
		$(TARGETDIR)\24ROM.MBM \
		$(TARGETDIR)\24RAMC.MBM \
		$(TARGETDIR)\24RAM.MBM \
		$(TARGETDIR)\32RAM.MBM \
		$(TARGETDIR)\DrawBitmap16MU.MBM \
		$(TARGETDIR)\DrawBitmapComp16MU.MBM


$(TARGETDIR)\16ROMC.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	/s	$(TARGETDIR)\16ROMC.MBM	/c16..\TBIT\RLETest.bmp

$(TARGETDIR)\16ROM.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	/r	$(TARGETDIR)\16ROM.MBM	/c16..\TBIT\RLETest.bmp

$(TARGETDIR)\16RAMC.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE		$(TARGETDIR)\16RAMC.MBM	/c16..\TBIT\RLETest.bmp

$(TARGETDIR)\16RAM.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	/n	$(TARGETDIR)\16RAM.MBM	/c16..\TBIT\RLETest.bmp

$(TARGETDIR)\paint16romc.mbm : ..\TBIT\paint16romc.mbm
		@COPY ..\TBIT\paint16romc.mbm	$(TARGETDIR)\paint16romc.mbm

$(TARGETDIR)\paint16ramc.mbm : ..\TBIT\paint16ramc.mbm
		@COPY ..\TBIT\paint16ramc.mbm	$(TARGETDIR)\paint16ramc.mbm

$(TARGETDIR)\paint24romc.mbm : ..\TBIT\paint24romc.mbm
		@COPY ..\TBIT\paint24romc.mbm	$(TARGETDIR)\paint24romc.mbm

$(TARGETDIR)\paint24ramc.mbm : ..\TBIT\paint24ramc.mbm
		@COPY ..\TBIT\paint24ramc.mbm	$(TARGETDIR)\paint24ramc.mbm

$(TARGETDIR)\8ramc.mbm : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	$(TARGETDIR)\8ramc.mbm	/c8..\TBIT\RLETest.bmp

$(TARGETDIR)\8romc.mbm : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	/s	$(TARGETDIR)\8romc.mbm	/c8..\TBIT\RLETest.bmp

$(TARGETDIR)\12ramc.mbm : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	$(TARGETDIR)\12ramc.mbm	/c12..\TBIT\RLETest.bmp

$(TARGETDIR)\12romc.mbm : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	/s	$(TARGETDIR)\12romc.mbm	/c12..\TBIT\RLETest.bmp

$(TARGETDIR)\BmCTest.mbm:  ..\TBIT\agbw24.bmp  ..\TBIT\Aif24.bmp  ..\TBIT\calc24.bmp  ..\TBIT\Data24.bmp  ..\TBIT\Dilbert1.bmp  ..\TBIT\Icn24.bmp  ..\TBIT\Inst24.bmp  ..\TBIT\Jotter24.bmp
	BMCONV.EXE $(TARGETDIR)\BmCTest.mbm  /c24..\TBIT\agbw24.bmp  /c24..\TBIT\Aif24.bmp  /c24..\TBIT\calc24.bmp  /c24..\TBIT\Data24.bmp  /c24..\TBIT\Dilbert1.bmp  /c24..\TBIT\Icn24.bmp  /c24..\TBIT\Inst24.bmp  /c24..\TBIT\Jotter24.bmp

$(TARGETDIR)\24ROMC.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	/s	$(TARGETDIR)\24ROMC.MBM	/c24..\TBIT\RLETest.bmp

$(TARGETDIR)\24ROM.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	/r	$(TARGETDIR)\24ROM.MBM	/c24..\TBIT\RLETest.bmp

$(TARGETDIR)\24RAMC.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE		$(TARGETDIR)\24RAMC.MBM	/c24..\TBIT\RLETest.bmp

$(TARGETDIR)\24RAM.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	/n	$(TARGETDIR)\24RAM.MBM	/c24..\TBIT\RLETest.bmp

$(TARGETDIR)\32RAM.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE	/n	$(TARGETDIR)\32RAM.MBM	/c32..\TBIT\RLETest.bmp

$(TARGETDIR)\DrawBitmap16MU.MBM : ..\TBIT\DrawBitmap.bmp
		@BMCONV.EXE	/n	$(TARGETDIR)\DrawBitmap16MU.MBM	/c32..\TBIT\DrawBitmap.bmp

$(TARGETDIR)\DrawBitmapComp16MU.MBM : ..\TBIT\DrawBitmap.bmp
		@BMCONV.EXE		$(TARGETDIR)\DrawBitmapComp16MU.MBM	/c32..\TBIT\DrawBitmap.bmp

$(TARGETDIR)\BitBltSrc.MBM : ..\TBIT\BitBltSrc.MBM
		@COPY     ..\TBIT\BitBltSrc.MBM          $(TARGETDIR)\BitBltSrc.MBM

$(TARGETDIR)\BitBltMasked.MBM : ..\TBIT\BitBltMasked.MBM
		@COPY     ..\TBIT\BitBltMasked.MBM          $(TARGETDIR)\BitBltMasked.MBM

$(TARGETDIR)\tbmp.MBM : ..\TBIT\tbmp.MBM
		@COPY     ..\TBIT\tbmp.MBM          $(TARGETDIR)\tbmp.MBM

$(TARGETDIR)\tile.MBM : ..\TBIT\tile.MBM
		@COPY     ..\TBIT\tile.MBM          $(TARGETDIR)\tile.MBM

$(TARGETDIR)\Corrupt000.WBMP : ..\TBIT\Corrupt000.WBMP
		@COPY     ..\TBIT\Corrupt000.WBMP          $(TARGETDIR)\Corrupt000.WBMP

$(TARGETDIR)\ZeroSizeRam.MBM : ..\TBIT\ZeroSizeRam.MBM
		@COPY     ..\TBIT\ZeroSizeRam.MBM          $(TARGETDIR)\ZeroSizeRam.MBM

$(TARGETDIR)\Compr12_2.mbm : ..\TBIT\Compr12_2.mbm
		@COPY ..\TBIT\Compr12_2.mbm	$(TARGETDIR)\Compr12_2.mbm

$(TARGETDIR)\Compr16_2.mbm : ..\TBIT\Compr16_2.mbm
		@COPY ..\TBIT\Compr16_2.mbm	$(TARGETDIR)\Compr16_2.mbm

$(TARGETDIR)\Compr24_2.mbm : ..\TBIT\Compr24_2.mbm
		@COPY ..\TBIT\Compr24_2.mbm	$(TARGETDIR)\Compr24_2.mbm

$(TARGETDIR)\RaCompr12_2.mbm : ..\TBIT\RaCompr12_2.mbm
		@COPY ..\TBIT\RaCompr12_2.mbm	$(TARGETDIR)\RaCompr12_2.mbm

$(TARGETDIR)\RaCompr16_2.mbm : ..\TBIT\RaCompr16_2.mbm
		@COPY ..\TBIT\RaCompr16_2.mbm	$(TARGETDIR)\RaCompr16_2.mbm

$(TARGETDIR)\RaCompr24_2.mbm : ..\TBIT\RaCompr24_2.mbm
		@COPY ..\TBIT\RaCompr24_2.mbm	$(TARGETDIR)\RaCompr24_2.mbm

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

BLD : $(TARGET_FILES)

CLEAN : 
	-@erase $(TARGET_FILES)

SAVESPACE : do_nothing

RESOURCE : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : do_nothing

RELEASABLES :
	@echo $(TARGET_FILES)



