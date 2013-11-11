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
// Implementation of the enumeration SymbolType for the elf2e32 tool
// @internalComponent
// @released
// 
//

#if !defined(EA_37C067EA_9B6B_4d95_84A3_ABBE88E7AD8F__INCLUDED_)
#define EA_37C067EA_9B6B_4d95_84A3_ABBE88E7AD8F__INCLUDED_

#include <tools/elfdefs.h>

enum SymbolType
{
	SymbolTypeNotDefined = STT_NOTYPE,
	SymbolTypeData = STT_OBJECT,
	SymbolTypeCode = STT_FUNC
};




#endif // !defined(EA_37C067EA_9B6B_4d95_84A3_ABBE88E7AD8F__INCLUDED_)
