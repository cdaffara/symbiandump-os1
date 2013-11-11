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
#include <comms-infras/ss_log.h>
#include <ecom/ecom.h>

#include "tier_manager_factory.h"
#include "tier_manager.h"


#if defined(__CFLOG_ACTIVE)
#define KLegacyLoopbackTierMgrTag KESockMetaConnectionTag
_LIT(KLegacyLoopbackTierMgrSubTag, "LegacyLoopbackTierMgrSubTag");
#endif

// ---------------- Factory Methods ----------------

/**
Creates a Agent Tier Manager Factory
This function also acts as the single ECom entry point into this object.
@param aParentContainer The parent factory container which owns this factory
@return Factory for IP level meta connection providers
*/
CLegacyLoopBackTierManagerFactory* CLegacyLoopBackTierManagerFactory::NewL(TAny* aParentContainer)
	{
	__CFLOG_VAR((KLegacyLoopbackTierMgrTag, KLegacyLoopbackTierMgrSubTag, _L8("CLegacyLoopBackTierManagerFactory::\tNewL(%08x)"), aParentContainer));
 	return new (ELeave) CLegacyLoopBackTierManagerFactory(TUid::Uid(CLegacyLoopBackTierManagerFactory::iUid),TUid::Uid(CLegacyLoopBackTierManagerFactory::iUid),*(reinterpret_cast<ESock::CTierManagerFactoryContainer*>(aParentContainer)));
	}



/**
Constructor for the Agent Tier Manager Factory
@param aFactoryId The UID which this factory can be looked up by
@param aParentContainer The parent factory container which owns this factory
*/
CLegacyLoopBackTierManagerFactory::CLegacyLoopBackTierManagerFactory(TUid aTierTypeId, TUid aFactoryUid, ESock::CTierManagerFactoryContainer& aParentContainer)
	: CTierManagerFactoryBase(aTierTypeId, aFactoryUid, aParentContainer)
	{
	__CFLOG_VAR((KLegacyLoopbackTierMgrTag, KLegacyLoopbackTierMgrSubTag, _L8("CLegacyLoopBackTierManagerFactory %08x:\tCLegacyLoopBackTierManagerFactory"), this));
	}



/**
Creates the actual Tier Manager
*/
ESock::ACommsFactoryNodeId* CLegacyLoopBackTierManagerFactory::DoCreateObjectL(ESock::TFactoryQueryBase& /* aQuery */)
	{
	return CLegacyLoopBackTierManager::NewL(*this);
	}
