// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalComponent
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "pdptiermanagerfactoryTraces.h"
#endif

#include <comms-infras/ss_log.h>
#include <ecom/ecom.h>
#include "pdptiermanagerfactory.h"
#include "pdptiermanager.h"

// ---------------- Factory Methods ----------------
/**
Creates a PDP Tier Manager Factory
This function also acts as the single ECom entry point into this object.
@param aParentContainer The parent factory container which owns this factory
@return
*/
CPDPTierManagerFactory* CPDPTierManagerFactory::NewL(TAny* aParentContainer)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGERFACTORY_NEWL_1, "CPDPTierManagerFactory::\tNewL(%08x)", aParentContainer);
 	return new (ELeave) CPDPTierManagerFactory(TUid::Uid(CPDPTierManagerFactory::EUid), TUid::Uid(CPDPTierManagerFactory::EUid),*(reinterpret_cast<ESock::CTierManagerFactoryContainer*>(aParentContainer)));
	}
/**
Constructor for the PDP Tier Manager Factory
@param aFactoryId The UID which this factory can be looked up by
@param aParentContainer The parent factory container which owns this factory
*/
CPDPTierManagerFactory::CPDPTierManagerFactory(TUid aTierTypeId, TUid aFactoryUid, ESock::CTierManagerFactoryContainer& aParentContainer)
	: CTierManagerFactoryBase(aTierTypeId, aFactoryUid, aParentContainer)
	{
	}
/**
Creates the actual Tier Manager
*/
ESock::ACommsFactoryNodeId* CPDPTierManagerFactory::DoCreateObjectL(ESock::TFactoryQueryBase& /* aQuery */)
	{
	return CPDPTierManager::NewL(*this);
	}
