# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Definitions common to all shells
# *_PATH variables to support filename policy conformance in makefile templates
# NOTE: These should all be replaced by calls to a function that lower-cases
# the current setting of the original variable
# 
#

PLATFORM_PATH=$(PLATFORM)
CFG_PATH=$(CFG)

ifeq ($(PLATFORM_PATH),WINS) 
	PLATFORM_PATH=wins
else
ifeq ($(PLATFORM_PATH),WINSCW) 
	PLATFORM_PATH=winscw
else
ifeq ($(PLATFORM_PATH),ARMV5) 
	PLATFORM_PATH=armv5
else
ifeq ($(PLATFORM_PATH),ARMV7) 
	PLATFORM_PATH=armv7
else
ifeq ($(PLATFORM_PATH),TOOLS) 
	PLATFORM_PATH=tools
else
ifeq ($(PLATFORM_PATH),TOOLS2) 
	PLATFORM_PATH=tools2
endif
endif
endif
endif
endif
endif

ifeq ($(CFG_PATH),UREL) 
	CFG_PATH=urel
else
ifeq ($(CFG_PATH),UDEB) 
	CFG_PATH=udeb
else
ifeq ($(CFG_PATH),DEB) 
	CFG_PATH=deb
else
ifeq ($(CFG_PATH),REL) 
	CFG_PATH=rel
endif
endif
endif
endif

