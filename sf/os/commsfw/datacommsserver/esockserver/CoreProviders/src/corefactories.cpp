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
// Core Factories class definition.
// 
//

/**
 @file
 @internalComponent
*/

#include <ecom/implementationproxy.h>
#include <in_sock.h>
#include <comms-infras/ss_log.h>
#include "corefactories.h"
#include <comms-infras/corecpr.h>
#include <comms-infras/corescpr.h>

#ifdef __CFLOG_ACTIVE
	#define KCoreFactorySubTag KESockBaseFactTag
#endif // __CFLOG_ACTIVE

using namespace ESock;


//-=========================================================
//
// ECOM data
//
//-=========================================================	
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(CCoreConnectionProviderFactory::iUid, CCoreConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CCoreSubConnectionProviderFactory::iUid, CCoreSubConnectionProviderFactory::NewL)
	};

/**
ECOM Implementation Factory
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
   }

//-=========================================================
//
// CCoreConnectionProviderFactory methods
//
//-=========================================================	
CCoreConnectionProviderFactory* CCoreConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CCoreConnectionProviderFactory(TUid::Uid(CCoreConnectionProviderFactory::iUid), *reinterpret_cast<ESock::CConnectionFactoryContainer*>(aParentContainer));
    }
    
CCoreConnectionProviderFactory::CCoreConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer)
	: ESock::CConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    }

ACommsFactoryNodeId* CCoreConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
    {    
    return CCoreConnectionProvider::NewL(*this); 
    }
    

//-=========================================================
//
// CCoreSubConnectionProviderFactory methods
//
//-=========================================================	
CCoreSubConnectionProviderFactory* CCoreSubConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CCoreSubConnectionProviderFactory(TUid::Uid(CCoreSubConnectionProviderFactory::iUid), 
                                            *reinterpret_cast<ESock::CSubConnectionFactoryContainer*>(aParentContainer));
    }
    
CCoreSubConnectionProviderFactory::CCoreSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer)
	: CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    }

ACommsFactoryNodeId* CCoreSubConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& aQuery)
    {
    const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(aQuery);
    if (query.iSCPRType == RSubConnection::EAttachToDefault)
        {
        return CCoreSubConnectionProvider::NewL(*this);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    CSubConnectionProviderBase* prov = NULL;
    return prov;
    }






