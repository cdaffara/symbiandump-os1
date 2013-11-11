// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation file for the 3GPP SubConnection Provider Factory
// 
//

/**
 @file
 @internalComponent
*/

#include <networking/qos3gpp_subconparams.h>
#include <ecom/implementationproxy.h>
#include <networking/qos3gpp_subconparams_factory.h>

/**
Data required for instantiating ECOM Plugin
*/
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KSubCon3GPPExtParamsFactoryUid, CSubCon3GPPExtParamsFactory::NewL)
	};


/**
ECOM Implementation Factory
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

  
