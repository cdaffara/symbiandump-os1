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

ifeq ($(PLATFORM),GCCXML)
MAKEDATA:
echo do nothing
else
ifeq ($(PLATFORM),TOOLS)
MAKEDATA:
echo do nothing
else
ifeq ($(PLATFORM),TOOLS2)
MAKEDATA:
echo do nothing
else
MAKEDATA:
	cd

	echo --------------------------------------------------------------
	echo Building SIS file to install cen rep for test psy
	echo --------------------------------------------------------------

	echo makesis
	makesis ctlbsinsttrackingpsycentrep.pkg ctlbsinsttrackingpsycentrep_unsigned.sis

	echo signsis	
	echo signsis ctlbsinsttrackingpsycentrep_unsigned.sis ctlbsinsttrackingpsycentrep.sis lbsbuildsis.cert lbsbuildsis.key
	signsis ctlbsinsttrackingpsycentrep_unsigned.sis ctlbsinsttrackingpsycentrep.sis lbsbuildsis.cert lbsbuildsis.key
	del /q ctlbsinsttrackingpsycentrep_unsigned.sis

	echo --------------------------------------------------------------
	echo need the process that owns the cen rep private data cage to be in ROM z:\sys\bin
	echo --------------------------------------------------------------
	echo if not exist %EPOCROOT%epoc32\data\z\sys\bin\centralrepositorysrv.exe copy %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\centralrepositorysrv.exe	%EPOCROOT%epoc32\data\z\sys\bin\centralrepositorysrv.exe
	if not exist %EPOCROOT%epoc32\data\z\sys\bin\centralrepositorysrv.exe copy %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\centralrepositorysrv.exe	%EPOCROOT%epoc32\data\z\sys\bin\centralrepositorysrv.exe
ifeq ($(PLATFORM), WINSCW)
	if not exist %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\sys\bin\centralrepositorysrv.exe copy %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\centralrepositorysrv.exe	%EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\sys\bin\centralrepositorysrv.exe
endif
	echo --------------------------------------------------------------
	echo Building SIS file to install test psy
	echo --------------------------------------------------------------
	
	echo Copy binary files to common location from release directory
	echo copy /y %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\ctlbsinstallabletrackingpsy.dll ctlbsinstallabletrackingpsy.dll
	copy /y %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\ctlbsinstallabletrackingpsy.dll ctlbsinstallabletrackingpsy.dll
	echo copy /y %EPOCROOT%epoc32\data\z\resource\plugins\ctlbsinstallabletrackingpsy.rsc ctlbsinstallabletrackingpsy.rsc
	copy /y %EPOCROOT%epoc32\data\z\resource\plugins\ctlbsinstallabletrackingpsy.rsc ctlbsinstallabletrackingpsy.rsc

	echo makesis
	makesis ctlbsinstallabletrackingpsy.pkg ctlbsinstallabletrackingpsy_unsigned.sis

	echo 
	echo delete binary files from common location
	del /q  ctlbsinstallabletrackingpsy.dll
	del /q  ctlbsinstallabletrackingpsy.rsc
	
	echo delete binary files from release directory
	echo del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\ctlbsinstallabletrackingpsy.dll
	del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\ctlbsinstallabletrackingpsy.dll
	echo del /q %EPOCROOT%epoc32\data\z\resource\plugins\ctlbsinstallabletrackingpsy.rsc
	del /q %EPOCROOT%epoc32\data\z\resource\plugins\ctlbsinstallabletrackingpsy.rsc

ifeq ($(PLATFORM), WINSCW)
	echo del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\plugins\ctlbsinstallabletrackingpsy.rsc
	del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\plugins\ctlbsinstallabletrackingpsy.rsc
endif
	echo signsis	
	signsis ctlbsinstallabletrackingpsy_unsigned.sis ctlbsinstallabletrackingpsy.sis lbsbuildsis.cert lbsbuildsis.key
	del /q ctlbsinstallabletrackingpsy_unsigned.sis 
		
	echo Copy winscw sis to emulator drive or armv5 to data z tree
	if not exist %EPOCROOT%epoc32\winscw\c\testdata\install md %EPOCROOT%epoc32\winscw\c\testdata\install
	if not exist %EPOCROOT%epoc32\winscw\c\system\libs\plugins md %EPOCROOT%epoc32\winscw\c\system\libs\plugins
	if not exist %EPOCROOT%epoc32\winscw\c\resource\plugins md %EPOCROOT%epoc32\winscw\c\resource\plugins
	if "$(PLATFORM)" == "WINSCW" copy /y ctlbsinstallabletrackingpsy.sis %EPOCROOT%epoc32\winscw\c\testdata\install\ctlbsinstallabletrackingpsy.sis 
	if "$(PLATFORM)" == "ARMV5"  copy /y ctlbsinstallabletrackingpsy.sis %EPOCROOT%epoc32\data\z\ctlbsinstallabletrackingpsy.sis 

endif
endif
endif


DO_NOTHING:
	rem do nothing

#
# The targets invoked by abld...
#

MAKMAKE : DO_NOTHING
FREEZE : DO_NOTHING
LIB : DO_NOTHING
RESOURCE : DO_NOTHING
CLEANLIB : DO_NOTHING

FINAL : MAKEDATA

RELEASABLES : DO_NOTHING

SAVESPACE : BLD

BLD : DO_NOTHING

CLEAN : 
	echo del ctlbsinstallabletrackingpsy.sis 
	if exist %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\tups\integ\packages\ctlbsinstallabletrackingpsy.sis del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\tups\integ\packages\ctlbsinstallabletrackingpsy.sis
	if exist %EPOCROOT%epoc32\data\z\ctlbsinstallabletrackingpsy.sis del /q %EPOCROOT%epoc32\data\z\ctlbsinstallabletrackingpsy.sis 
ifeq ($(PLATFORM), WINSCW)
	if exist %EPOCROOT%epoc32\winscw\c\testdata\install\ctlbsinstallabletrackingpsy.sis del /q %EPOCROOT%epoc32\winscw\c\testdata\install\ctlbsinstallabletrackingpsy.sis 
endif



