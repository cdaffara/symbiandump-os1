# fshell_builddocs.mk
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
BLD :
	..\tools\fsh-builddocs -i epoc32/include pod-list.txt

RELEASABLES :
	@..\tools\fsh-builddocs -i epoc32/include pod-list.txt -w

CLEAN :
	..\tools\fsh-builddocs -i epoc32/include pod-list.txt -c

MAKMAKE FREEZE LIB CLEANLIB RESOURCE SAVESPACE FINAL : 
