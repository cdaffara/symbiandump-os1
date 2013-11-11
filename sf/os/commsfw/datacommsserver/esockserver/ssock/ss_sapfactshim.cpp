// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file SS_SAPFACTSHIM.CPP
*/

#define SYMBIAN_NETWORKING_UPS

#include "ss_sapfactshim.h"
#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <comms-infras/ss_sapshim.h>

#include <comms-infras/ss_nodemessages_serviceprovider.h>

using namespace ESock;
using namespace Factories;
using namespace Messages;

MFactoryQuery::TMatchResult XProviderFactoryQueryProvider::Match(TFactoryObjectInfo& aProviderInfo)
	{
	CTransportFlowShim* prov = static_cast<CTransportFlowShim*>(aProviderInfo.iInfo.iFactoryObject);
	return prov->Provider() == &iSSP ? EMatch : EContinue;
	}

// ==============================================================================================

CTransportFlowShimFactory* CTransportFlowShimFactory::NewL(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer)
	{
	CTransportFlowShimFactory* fact = new (ELeave) CTransportFlowShimFactory(aFactoryId, aParentContainer);
	CleanupStack::PushL(fact);
	fact->ConstructL();
	CleanupStack::Pop(fact);
	return fact;
	}

CTransportFlowShimFactory::CTransportFlowShimFactory(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer)
 : CSubConnectionFlowFactoryBase(aFactoryId, aParentContainer)
 	{
 	}

CTransportFlowShimFactory::~CTransportFlowShimFactory()
	{
 	}

CSubConnectionFlowBase* CTransportFlowShimFactory::DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery)
	{
	const TDefaultFlowFactoryQuery& query = static_cast<const TDefaultFlowFactoryQuery&>(aQuery);
#ifdef SYMBIAN_NETWORKING_UPS	
	return CUpsTransportFlowShim::NewL(*this, query.iSCprId, aProtocolIntf);
#else
	return CTransportFlowShim::NewL(*this, query.iSCprId, aProtocolIntf);
#endif
	}

ACommsFactoryNodeId* CTransportFlowShimFactory::DoFindOrCreateObjectL(TFactoryQueryBase& aQuery)
	{
	const TDefaultFlowFactoryQuery& query = static_cast<const TDefaultFlowFactoryQuery&>(aQuery);	

	if(query.iMessageId == TCFServiceProvider::TCommsBinderRequest::Id())
		{
		return DoCreateObjectL(aQuery);
		}
	return NULL;
	}


