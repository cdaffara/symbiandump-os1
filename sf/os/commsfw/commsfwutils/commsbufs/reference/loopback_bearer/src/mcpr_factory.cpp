/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include <ecom/implementationproxy.h>
#include "mcpr_factory.h"
#include "mcpr.h"
#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <ecom/ecom.h>

#include <comms-infras/ss_msgintercept.h>

using namespace ESock;

//-=========================================================
//
// CLegacyLoopbackMetaConnectionProviderFactory methods
//
//-=========================================================	
CLegacyLoopbackMetaConnectionProviderFactory* CLegacyLoopbackMetaConnectionProviderFactory::NewL(TAny* aParentContainer)
	{
 	return new (ELeave) CLegacyLoopbackMetaConnectionProviderFactory(TUid::Uid(CLegacyLoopbackMetaConnectionProviderFactory::iUid), *(reinterpret_cast<CMetaConnectionFactoryContainer*>(aParentContainer)));
	}

CLegacyLoopbackMetaConnectionProviderFactory::CLegacyLoopbackMetaConnectionProviderFactory(TUid aFactoryId, CMetaConnectionFactoryContainer& aParentContainer)
	: CMetaConnectionProviderFactoryBase(aFactoryId, aParentContainer)
	{
	}

ESock::ACommsFactoryNodeId* CLegacyLoopbackMetaConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& aQuery)
	{
	const TMetaConnectionFactoryQuery& query = static_cast<const TMetaConnectionFactoryQuery&>(aQuery);
	CMetaConnectionProviderBase* provider = CLegacyLoopbackMetaConnectionProvider::NewL(*this, query.iProviderInfo);
	
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);

	return provider;
	}


