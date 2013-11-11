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
#include "mcpr_factory.h"
#include "mcpr.h"

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <implementationproxy.h>
#include <ss_log.h>
#else
#include <comms-infras/ss_log.h>
#include <ecom/implementationproxy.h>
#endif

#include <ss_glob.h>
#include <ecom/ecom.h>

#include <comms-infras/ss_msgintercept.h>


using namespace ESock;

//-=========================================================
//
// CZeroCopyLoopbackMetaConnectionProviderFactory methods
//
//-=========================================================	
CZeroCopyLoopbackMetaConnectionProviderFactory* CZeroCopyLoopbackMetaConnectionProviderFactory::NewL(TAny* aParentContainer)
	{
 	return new (ELeave) CZeroCopyLoopbackMetaConnectionProviderFactory(TUid::Uid(CZeroCopyLoopbackMetaConnectionProviderFactory::iUid), *(reinterpret_cast<CMetaConnectionFactoryContainer*>(aParentContainer)));
	}

CZeroCopyLoopbackMetaConnectionProviderFactory::CZeroCopyLoopbackMetaConnectionProviderFactory(TUid aFactoryId, CMetaConnectionFactoryContainer& aParentContainer)
	: CMetaConnectionProviderFactoryBase(aFactoryId, aParentContainer)
	{
	}

ESock::ACommsFactoryNodeId* CZeroCopyLoopbackMetaConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& aQuery)
	{
	const TMetaConnectionFactoryQuery& query = static_cast<const TMetaConnectionFactoryQuery&>(aQuery);
	CMetaConnectionProviderBase* provider = CZeroCopyLoopbackMetaConnectionProvider::NewL(*this, query.iProviderInfo);
	
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);

	return provider;
	}


