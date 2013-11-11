// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <comms-infras/esock_params.h>
#include <comms-infras/esock_params_internal.h>
#include <ecom/implementationproxy.h>


const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(TConnAPPref::EUid, CESockParamaterFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(TConnCSRPref::EUid, CESockParamaterFactory::NewL),
	};

/**
ECOM Implementation Factories
*/

//extern "C" { IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount); }

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

