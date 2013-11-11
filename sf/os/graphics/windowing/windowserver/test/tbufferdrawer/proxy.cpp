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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <ecom/implementationproxy.h>
#include "bufferdrawer.h"


LOCAL_C const TImplementationProxy KImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(CWsBufferGraphicDrawer::EImplUid, CWsBufferGraphicDrawer::NewL),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(KImplementationTable)/sizeof(TImplementationProxy);
	return KImplementationTable;
	}
