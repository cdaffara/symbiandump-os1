//ImplementationUidTable.cpp

// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Save as expressly licensed to you by Symbian Software Ltd, all rights reserved.
// 
//

#include <e32std.h>
#include <ecom/implementationproxy.h>

// Table of implementation UIDs associated with the FourCc to Format UID mapping. 
// Note: these resources are never instantiated, and thus the associated constructor method can be given as NULL.

const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(0x1028347D, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283482, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283481, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283475, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283484, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283486, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283485, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283468, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x1028347F, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283480, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283487, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283488, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x10283489, NULL),
		IMPLEMENTATION_PROXY_ENTRY(0x1028348A, NULL)
	};

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
