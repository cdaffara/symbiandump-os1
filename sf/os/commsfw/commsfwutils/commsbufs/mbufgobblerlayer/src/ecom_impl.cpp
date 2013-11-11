// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// ECOM implementation for the MbufGobbler layer provider factories
//

/**
 @file
 @internalComponent
 */

#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>

#include "mbufgobblerflowfactory.h"
#include "mbufgobblerconnproviderfactory.h"
#include "mbufgobblersubconnproviderfactory.h"
#include "mbufgobblermetaconnproviderfactory.h"
#include "mbufgobblertiermanagerfactory.h"

//
// ECOM Implementation
//

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(CMbufGobblerTierManagerFactory::iUid, CMbufGobblerTierManagerFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CMbufGobblerMetaConnectionProviderFactory::iUid, CMbufGobblerMetaConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CMbufGobblerConnProviderFactory::iUid, CMbufGobblerConnProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CMbufGobblerSubconnProviderFactory::iUid, CMbufGobblerSubconnProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CMbufGobblerFlowFactory::EUid, CMbufGobblerFlowFactory::NewL),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

