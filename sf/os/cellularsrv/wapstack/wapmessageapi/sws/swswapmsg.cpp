// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "WapBoundWDP.h"
#include "WapFullySpecWDP.h"
#include "WapBoundCLPush.h"
#include "WapFullySpecCLPush.h"
#include "WapBoundCLWSP.h"
#include "WapFullySpecCLWSP.h"

// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x101FBB37,	CSWSWapBoundWDPService::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x101FBB38,	CSWSWapFullySpecWDPService::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x101FBB39,	CSWSWapBoundCLPushService::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x101FBB3A,	CSWSWapFullySpecCLPushService::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x101FBB3C,	CSWSWapBoundCLWSPService::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x101FBB3E,	CSWSWapFullySpecCLWSPService::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

