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
	@echo $(PLATFORM)
	echo --------------------------------------------------------------
	echo Building SIS file to install cen rep for bt psy
	echo --------------------------------------------------------------

	echo Copy binary files to common location
	echo copy /y %EPOCROOT%epoc32\data\z\private\10202be9\101fe999.cre 101fe999.cre 
	copy /y %EPOCROOT%epoc32\data\z\private\10202be9\101fe999.cre 101fe999.cre
	
	echo delete binary files from other locations:
	echo del /q %EPOCROOT%epoc32\data\z\private\10202be9\101fe999.cre
	del /q %EPOCROOT%epoc32\data\z\private\10202be9\101fe999.cre
ifeq ($(PLATFORM), WINSCW)
	echo del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\private\10202be9\101fe999.cre
	if exist %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\private\10202be9\101fe999.cre del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\private\10202be9\101fe999.cre
endif

	echo makesis btpsycentrep.pkg
	makesis btpsycentrep.pkg btpsycentrep_unsigned.sis
	
	echo signsis btpsycentrep_unsigned.sis btpsycentrep.sis lbsbuildsis.cert lbsbuildsis.key
	signsis btpsycentrep_unsigned.sis btpsycentrep.sis lbsbuildsis.cert lbsbuildsis.key

	echo delete binary files from common location:
	echo del /q btpsycentrep_unsigned.sis
	del /q btpsycentrep_unsigned.sis

	echo del /q  101fe999.cre
	del /q  101fe999.cre
	
	echo --------------------------------------------------------------
	echo The process that owns the cen rep private data cage needs 
	echo to be in ROM
	echo --------------------------------------------------------------
	
	echo if not exist %EPOCROOT%epoc32\data\z\sys\bin\centralrepositorysrv.exe copy %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\centralrepositorysrv.exe	%EPOCROOT%epoc32\data\z\sys\bin\centralrepositorysrv.exe
	if not exist %EPOCROOT%epoc32\data\z\sys\bin\centralrepositorysrv.exe copy %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\centralrepositorysrv.exe	%EPOCROOT%epoc32\data\z\sys\bin\centralrepositorysrv.exe
	
	echo --------------------------------------------------------------
	echo Building SIS file to install removable test client
	echo --------------------------------------------------------------
	
	echo Copy binary files to common location from release directory
	echo copy /y %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\nlabtgpspsy.dll nlabtgpspsy.dll 
	copy /y %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\nlabtgpspsy.dll nlabtgpspsy.dll 
	echo copy /y %EPOCROOT%epoc32\data\z\resource\plugins\nlabtgpspsy.rsc nlabtgpspsy.rsc 
	copy /y %EPOCROOT%epoc32\data\z\resource\plugins\nlabtgpspsy.rsc nlabtgpspsy.rsc 
	echo copy /y %EPOCROOT%epoc32\data\z\resource\nlabtgpspsyconstants.rsc nlabtgpspsyconstants.rsc
	copy /y %EPOCROOT%epoc32\data\z\resource\nlabtgpspsyconstants.rsc nlabtgpspsyconstants.rsc 
	echo copy /y %EPOCROOT%epoc32\data\z\resource\nlabtgpspsy_name.rsc nlabtgpspsy_name.rsc 	
	copy /y %EPOCROOT%epoc32\data\z\resource\nlabtgpspsy_name.rsc nlabtgpspsy_name.rsc 
	
	echo makesis
	makesis btgpspsytest.pkg btgpspsytest_unsigned.sis
	
	echo delete binary files from common location
	echo del /q  nlabtgpspsy.dll
	del /q  nlabtgpspsy.dll
	echo 	del /q  nlabtgpspsy.rsc
	del /q  nlabtgpspsy.rsc
	echo 	del /q  nlabtgpspsyconstants.rsc
	del /q  nlabtgpspsyconstants.rsc
	echo del /q  nlabtgpspsy_name.rsc
	del /q  nlabtgpspsy_name.rsc
	
	echo delete binary files from release directory:
	echo del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\nlabtgpspsy.dll
	del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\nlabtgpspsy.dll
	echo del %EPOCROOT%epoc32\data\z\resource\plugins\nlabtgpspsy.rsc
	del %EPOCROOT%epoc32\data\z\resource\plugins\nlabtgpspsy.rsc
	echo del %EPOCROOT%epoc32\data\z\resource\nlabtgpspsyconstants.rsc
	del %EPOCROOT%epoc32\data\z\resource\nlabtgpspsyconstants.rsc
	echo del %EPOCROOT%epoc32\data\z\resource\nlabtgpspsy_name.rsc
	del %EPOCROOT%epoc32\data\z\resource\nlabtgpspsy_name.rsc

ifeq ($(PLATFORM), WINSCW)
	echo del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\plugins\nlabtgpspsy.rsc
	if exist %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\plugins\nlabtgpspsy.rsc		del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\plugins\nlabtgpspsy.rsc
	echo del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\nlabtgpspsyconstants.rsc
	if exist %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\nlabtgpspsyconstants.rsc	del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\nlabtgpspsyconstants.rsc
	echo del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\nlabtgpspsy_name.rsc
	if exist %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\nlabtgpspsy_name.rsc	del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\resource\nlabtgpspsy_name.rsc

endif

	echo signsis btgpspsytest_unsigned.sis btgpspsytest.sis lbsbuildsis.cert lbsbuildsis.key
	signsis btgpspsytest_unsigned.sis btgpspsytest.sis lbsbuildsis.cert lbsbuildsis.key
	del /q btgpspsytest_unsigned.sis 
		
	echo Copy winscw sis to emulator drive or armv5 to data z tree
	echo %EPOCROOT%epoc32\winscw\c\testdata\install\btgpspsytest.sis
	if not exist %EPOCROOT%epoc32\winscw\c\testdata\install md %EPOCROOT%epoc32\winscw\c\testdata\install
	if not exist %EPOCROOT%epoc32\winscw\c\system\libs\plugins md %EPOCROOT%epoc32\winscw\c\system\libs\plugins
	if not exist %EPOCROOT%epoc32\winscw\c\resource\plugins md %EPOCROOT%epoc32\winscw\c\resource\plugins
	if "$(PLATFORM)" == "WINSCW" copy /y btgpspsytest.sis %EPOCROOT%epoc32\winscw\c\testdata\install\btgpspsytest.sis 
	if "$(PLATFORM)" == "ARMV5"  copy /y btgpspsytest.sis %EPOCROOT%epoc32\data\z\btgpspsytest.sis
	

endif
endif
endif

DO_NOTHING:
	echo do nothing

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
	echo del btgpspsytest.sis 
	if exist %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\tups\integ\packages\btgpspsytest.sis del /q %EPOCROOT%epoc32\release\$(PLATFORM)\$(CFG)\z\tups\integ\packages\btgpspsytest.sis 
	if exist %EPOCROOT%epoc32\data\z\btgpspsytest.sis del /q %EPOCROOT%epoc32\data\z\btgpspsytest.sis 
ifeq ($(PLATFORM), WINSCW)
	if exist %EPOCROOT%epoc32\winscw\c\testdata\install\btgpspsytest.sis del /q %EPOCROOT%epoc32\winscw\c\testdata\install\btgpspsytest.sis 
endif
	
	


