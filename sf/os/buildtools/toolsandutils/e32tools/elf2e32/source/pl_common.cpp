// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of the Class SymbolAttrib for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include <tools/elfdefs.h>
#include "pl_common.h"


VersionInfo::VersionInfo(): iSOName(0), iLinkAs(0), iVerCategory(VER_CAT_NONE) 
{}

VersionInfo::~VersionInfo()
{}

/**
hash function for ELF symbols
@param name
@internalComponent
@released
*/
unsigned long Util::elf_hash(const unsigned char *name)
{
	unsigned long h, g;
	for (h = 0; *name != 0; ++name)
	{
		h = (h << 4) + *name;
		g = h & 0xf0000000;
		if (g != 0) h ^= g >> 24;
		h &= ~g;
	}
	return h;
}
