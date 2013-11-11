// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include <ecom/implementationproxy.h>
#include "renderstagefactory.h"


LOCAL_C const TImplementationProxy KImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(CRenderStageFactory::EImplUid,CRenderStageFactory::CreateL)
	};


//Single exported function for an ecom dll. It returns a table of implementation
//uids along with the implementation's factory function. Each implementation in
//the dll has an entry.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = (sizeof(KImplementationTable) / sizeof(TImplementationProxy));
	return KImplementationTable;
	}
