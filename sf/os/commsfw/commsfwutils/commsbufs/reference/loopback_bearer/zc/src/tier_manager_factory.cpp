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
#define KZeroCopyLoopbackTierMgrTag KESockMetaConnectionTag
_LIT(KZeroCopyLoopbackTierMgrSubTag, "ZeroCopyLoopbackTierMgrSubTag");
#endif

// ---------------- Factory Methods ----------------

/**
Creates a Agent Tier Manager Factory
This function also acts as the single ECom entry point into this object.
@param aParentContainer The parent factory container which owns this factory
@return Factory for IP level meta connection providers
*/
CZeroCopyLoopBackTierManagerFactory* CZeroCopyLoopBackTierManagerFactory::NewL(TAny* aParentContainer)
	{
	__CFLOG_VAR((KZeroCopyLoopbackTierMgrTag, KZeroCopyLoopbackTierMgrSubTag, _L8("CZeroCopyLoopBackTierManagerFactory::\tNewL(%08x)"), aParentContainer));
 	return new (ELeave) CZeroCopyLoopBackTierManagerFactory(TUid::Uid(CZeroCopyLoopBackTierManagerFactory::iUid), TUid::Uid(CZeroCopyLoopBackTierManagerFactory::iUid),*(reinterpret_cast<ESock::CTierManagerFactoryContainer*>(aParentContainer)));
	}



/**
Constructor for the Agent Tier Manager Factory
@param aFactoryId The UID which this factory can be looked up by
@param aParentContainer The parent factory container which owns this factory
*/
CZeroCopyLoopBackTierManagerFactory::CZeroCopyLoopBackTierManagerFactory(TUid aTierTypeId, TUid aFactoryUid, ESock::CTierManagerFactoryContainer& aParentContainer)
	: CTierManagerFactoryBase(aTierTypeId, aFactoryUid, aParentContainer)
	{
	__CFLOG_VAR((KZeroCopyLoopbackTierMgrTag, KZeroCopyLoopbackTierMgrSubTag, _L8("CZeroCopyLoopBackTierManagerFactory %08x:\tCZeroCopyLoopBackTierManagerFactory"), this));
	}



/**
Creates the actual Tier Manager
*/
ESock::ACommsFactoryNodeId* CZeroCopyLoopBackTierManagerFactory::DoCreateObjectL(ESock::TFactoryQueryBase& /* aQuery */)
	{
	return CZeroCopyLoopBackTierManager::NewL(*this);
	}
