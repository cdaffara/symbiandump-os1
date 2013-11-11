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
// RawIP MCPR Factory
// 
//

/**
 @file
 @internalComponent
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "rawipmcprfactoryTraces.h"
#endif

#include "rawipmcprfactory.h"
#include "rawipmcpr.h"
#include <comms-infras/ss_log.h>
#include <ss_glob.h>

#include <comms-infras/ss_msgintercept.h>


using namespace ESock;

//-=========================================================
//
// CRawIpMetaConnectionProviderFactory methods
//
//-=========================================================	
CRawIpMetaConnectionProviderFactory* CRawIpMetaConnectionProviderFactory::NewL(TAny* aParentContainer)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPMETACONNECTIONPROVIDERFACTORY_NEWL_1, "CRawIpMetaConnectionProviderFactory ::\tNewL(%08x)",(TUint)aParentContainer);
 	return new (ELeave) CRawIpMetaConnectionProviderFactory(TUid::Uid(CRawIpMetaConnectionProviderFactory::iUid), *(reinterpret_cast<CMetaConnectionFactoryContainer*>(aParentContainer)));
	}

CRawIpMetaConnectionProviderFactory::CRawIpMetaConnectionProviderFactory(TUid aFactoryId, CMetaConnectionFactoryContainer& aParentContainer)
	: CMetaConnectionProviderFactoryBase(aFactoryId,aParentContainer)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPMETACONNECTIONPROVIDERFACTORY_NEWL_2, "CRawIpMetaConnectionProviderFactory %08x:\tCRawIpMetaConnectionProviderFactory Constructor",(TUint)this);

	}

ACommsFactoryNodeId* CRawIpMetaConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& aQuery)
	{
	const TMetaConnectionFactoryQuery& query = static_cast<const TMetaConnectionFactoryQuery&>(aQuery);
	CMetaConnectionProviderBase* provider = CRawIpMetaConnectionProvider::NewL(*this, query.iProviderInfo);	
	
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);

	return provider;
	}

