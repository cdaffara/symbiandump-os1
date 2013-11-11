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

CLEAN:
	$(TO_BLDINF)\bld.bat clean	
MAKMAKE:
	$(TO_BLDINF)\setupprj.bat
BLD:
	$(TO_BLDINF)\bld.bat $(CFG)
RELEASABLES:
	$(TO_BLDINF)\bld.bat releasables
LIB SAVESPACE FREEZE CLEANLIB RESOURCE FINAL:
	@echo $@ does nothing in $(TEMPLATE_EXTENSION_MAKEFILE)


