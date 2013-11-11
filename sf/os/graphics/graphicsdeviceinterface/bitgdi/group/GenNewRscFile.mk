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
# Generate test RSC files using new RSC file format: RSC file header + RSC file section + Bitmap file section
# 
#

!if "$(PLATFORM)"=="WINS" || "$(PLATFORM)"=="WINSCW"
TARGETDIR=$(EPOCROOT)EPOC32\RELEASE\$(PLATFORM)\$(CFG)\Z\SYSTEM\DATA
!else
TARGETDIR=$(EPOCROOT)EPOC32\DATA\Z\SYSTEM\DATA
!endif

do_nothing :
	rem do_nothing

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

TARGET_FILES=\
		$(TARGETDIR)\RscHeader2.Bin \
		$(TARGETDIR)\DummyRscFile.RSC \
		..\TBIT\8ROMC.MBM \
		..\TBIT\8RAMC.MBM \
		$(TARGETDIR)\RomRsc_RomMbm.rsc \
		$(TARGETDIR)\RamRsc_RamMbm.rsc \
		..\TBIT\8RAMC2.MBM \
		$(TARGETDIR)\RamRsc_RamMbm2.rsc \
		..\TBIT\8ROMC2.MBM \
		$(TARGETDIR)\RomRsc_RomMbm2.rsc \
		$(TARGETDIR)\RomRsc_RamMbm.rsc \
		$(TARGETDIR)\RamRsc_RomMbm.rsc \
		$(TARGETDIR)\RamRsc_RomMbm2.rsc \
		$(TARGETDIR)\RomRsc_RamMbm2.rsc

$(TARGETDIR)\RscHeader2.Bin : ..\TBIT\RscHeader2.Bin
		@copy ..\TBIT\RscHeader2.Bin			$(TARGETDIR)\RscHeader2.Bin

$(TARGETDIR)\DummyRscFile.RSC : ..\TBIT\DummyRscFile.RSC
		@copy ..\TBIT\DummyRscFile.RSC			$(TARGETDIR)\DummyRscFile.RSC

..\TBIT\8ROMC.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE		/s		..\TBIT\8ROMC.MBM		/c8..\TBIT\RLETest.bmp

..\TBIT\8RAMC.MBM : ..\TBIT\RLETest.bmp
		@BMCONV.EXE				..\TBIT\8RAMC.MBM		/c8..\TBIT\RLETest.bmp

$(TARGETDIR)\RomRsc_RomMbm.rsc : ..\TBIT\RscHeader2.Bin ..\TBIT\DummyRscFile.RSC ..\TBIT\8ROMC.MBM
		@copy ..\TBIT\RscHeader2.Bin /b + ..\TBIT\DummyRscFile.RSC /b + ..\TBIT\8ROMC.MBM /b $(TARGETDIR)\RomRsc_RomMbm.rsc

$(TARGETDIR)\RamRsc_RamMbm.rsc : ..\TBIT\RscHeader2.Bin ..\TBIT\DummyRscFile.RSC ..\TBIT\8RAMC.MBM
		@copy ..\TBIT\RscHeader2.Bin /b + ..\TBIT\DummyRscFile.RSC /b + ..\TBIT\8RAMC.MBM /b $(TARGETDIR)\RamRsc_RamMbm.rsc

..\TBIT\8RAMC2.MBM : ..\TBIT\TBmp.bmp ..\TBIT\TGdiGen.bmp
		@BMCONV.EXE				..\TBIT\8RAMC2.MBM		/c8..\TBIT\TBmp.bmp		/c8..\TBIT\TGdiGen.bmp

$(TARGETDIR)\RamRsc_RamMbm2.rsc : ..\TBIT\RscHeader2.Bin ..\TBIT\DummyRscFile.RSC ..\TBIT\8RAMC2.MBM
		@copy ..\TBIT\RscHeader2.Bin /b + ..\TBIT\DummyRscFile.RSC /b + ..\TBIT\8RAMC2.MBM /b $(TARGETDIR)\RamRsc_RamMbm2.rsc

..\TBIT\8ROMC2.MBM : ..\TBIT\TBmp.bmp ..\TBIT\TGdiGen.bmp
		@BMCONV.EXE		/s		..\TBIT\8ROMC2.MBM		/c8..\TBIT\TBmp.bmp		/c8..\TBIT\TGdiGen.bmp

$(TARGETDIR)\RomRsc_RomMbm2.rsc : ..\TBIT\RscHeader2.Bin ..\TBIT\DummyRscFile.RSC ..\TBIT\8ROMC2.MBM
		@copy ..\TBIT\RscHeader2.Bin /b + ..\TBIT\DummyRscFile.RSC /b + ..\TBIT\8ROMC2.MBM /b $(TARGETDIR)\RomRsc_RomMbm2.rsc

$(TARGETDIR)\RomRsc_RamMbm.rsc : ..\TBIT\RscHeader2.Bin ..\TBIT\DummyRscFile.RSC ..\TBIT\8RAMC.MBM
		@copy ..\TBIT\RscHeader2.Bin /b + ..\TBIT\DummyRscFile.RSC /b + ..\TBIT\8RAMC.MBM /b $(TARGETDIR)\RomRsc_RamMbm.rsc

$(TARGETDIR)\RamRsc_RomMbm.rsc : ..\TBIT\RscHeader2.Bin ..\TBIT\DummyRscFile.RSC ..\TBIT\8ROMC.MBM
		@copy ..\TBIT\RscHeader2.Bin /b + ..\TBIT\DummyRscFile.RSC /b + ..\TBIT\8ROMC.MBM /b $(TARGETDIR)\RamRsc_RomMbm.rsc

$(TARGETDIR)\RamRsc_RomMbm2.rsc : ..\TBIT\RscHeader2.Bin ..\TBIT\DummyRscFile.RSC ..\TBIT\8ROMC2.MBM
		@copy ..\TBIT\RscHeader2.Bin /b + ..\TBIT\DummyRscFile.RSC /b + ..\TBIT\8ROMC2.MBM /b $(TARGETDIR)\RamRsc_RomMbm2.rsc

$(TARGETDIR)\RomRsc_RamMbm2.rsc : ..\TBIT\RscHeader2.Bin ..\TBIT\DummyRscFile.RSC ..\TBIT\8RAMC2.MBM
		@copy ..\TBIT\RscHeader2.Bin /b + ..\TBIT\DummyRscFile.RSC /b + ..\TBIT\8RAMC2.MBM /b $(TARGETDIR)\RomRsc_RamMbm2.rsc

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



