// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ECOM implementation for the IP Protocol provider factories
// 
//

/**
 @file
 @internalComponent
*/


#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>

#include "pdpmcprfactory.h"
#include "PDPCPRFactory.h"
#include "PDPSCPRFactory.h"
#include "pdptiermanagerfactory.h"

//
// ECOM Implementation
//

const TImplementationProxy ImplementationTable[] =
	{   
	IMPLEMENTATION_PROXY_ENTRY(CPdpMetaConnectionProviderFactory::iUid, CPdpMetaConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CPDPConnectionProviderFactory::iUid, CPDPConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CPDPSubConnectionProviderFactory::iUid, CPDPSubConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CPDPTierManagerFactory::EUid, CPDPTierManagerFactory::NewL)
   	};


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
   {
   aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
   return ImplementationTable;
   }

