# os_ver.mk
# 
# Copyright (c) 2010 Accenture. All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Accenture - Initial contribution
#
MAKMAKE :
	perl $(EPOCROOT)epoc32\tools\emkdir.pl $(EPOCROOT)epoc32\build\fshell/win32cons\generated
	perl .\os_version_h.PL > $(EPOCROOT)epoc32\build\fshell/win32cons\generated\os_version.h

CLEAN :
	-del ..\src\os_version.h

BLD FREEZE LIB CLEANLIB RESOURCE SAVESPACE RELEASABLES FINAL : 
