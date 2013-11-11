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
// PDP SubConnection Provider factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#include <ecom/implementationproxy.h>
#include "PDPSCPRFactory.h"
#include "PDPSCPR.h"
#include "PDPDeftSCPR.h"
#include <comms-infras/ss_log.h>

#include <comms-infras/ss_msgintercept.h>

using namespace ESock;

//-=========================================================
//
// CPDPSubConnectionProviderFactory methods
//
//-=========================================================	
CPDPSubConnectionProviderFactory* CPDPSubConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CPDPSubConnectionProviderFactory(TUid::Uid(CPDPSubConnectionProviderFactory::iUid), 
                                            *reinterpret_cast<ESock::CSubConnectionFactoryContainer*>(aParentContainer));
    }
    
CPDPSubConnectionProviderFactory::CPDPSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer)
	: CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    }

ESock::ACommsFactoryNodeId* CPDPSubConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& aQuery)
    {
    const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(aQuery);
    CSubConnectionProviderBase* provider = NULL;
    if (query.iSCPRType == RSubConnection::ECreateNew)
        {
        provider = CPDPSubConnectionProvider::NewL(*this);
		ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
        }
    else if (query.iSCPRType == RSubConnection::EAttachToDefault)
        {
        provider = CPDPDefaultSubConnectionProvider::NewL(*this);
		ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    return provider;
    }


