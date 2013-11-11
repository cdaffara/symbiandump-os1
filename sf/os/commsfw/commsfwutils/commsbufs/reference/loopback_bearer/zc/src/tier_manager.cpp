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
#include "e32property.h"
#include <comms-infras/ss_log.h>
#include <comms-infras/coretiermanagerstates.h>
#include <comms-infras/coretiermanageractivities.h>

#include "var.h"
#include "params.h"
#include "tier_manager.h"
#include "tier_manager_selector.h"

#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/esock_params.h>



using namespace ESock;


#ifdef __CFLOG
#define KZeroCopyLoopbackTierMgrTag KESockMetaConnectionTag
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace MeshMachine;
using namespace Messages;

namespace ZeroCopyLoopbackTMSelectProviderActivity
{ //Simple parallel activity provider selection, waits untill selection completes (via ISelectionNotify), then gets destroyed
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySelect, ZeroCopyLoopbackTMSelectProvider, TCFSelector::TSelect, TMActivities::CSelectProviderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(TMStates::TAwaitingSelectProviderSuper, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ZeroCopyLoopbackTMStates::TSelectionTest, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, TMStates::TSelectProviderSuper, CoreStates::TNeverAccept, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}


DEFINE_SMELEMENT(ZeroCopyLoopbackTMStates::TSelectionTest, NetStateMachine::MStateTransition, ZeroCopyLoopbackTMStates::TContext)
void ZeroCopyLoopbackTMStates::TSelectionTest::DoL()
	{
	RConnPrefList handle = message_cast<TCFSelector::TSelect>(iContext.iMessage).iConnPrefList;
	RConnPrefList::TIter<TZeroCopyLoopbackPref> iter = handle.getIter<TZeroCopyLoopbackPref>();
	while(!iter.IsEnd())
		{
		TInt AP = iter->AP();

		RProperty property;
		TInt result = property.Attach(TUid::Uid(0x10203FDD), 0);
		if(result == KErrNone)
			{
			TInt propertyValue;
			result = property.Get(propertyValue);
			if(result == KErrNone && propertyValue == AP)
				{
				property.Set(KErrNone);
				}
			}
		iter++;
		}
	}

namespace ZeroCopyLoopbackTierManagerActivities
{
	DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
	ACTIVITY_MAP_ENTRY(ZeroCopyLoopbackTMSelectProviderActivity,ZeroCopyLoopbackTMSelectProvider)
	ACTIVITY_MAP_END_BASE(TMActivities, coreTMActivities)
}


/**
Factory function for the factory which manages network level meta connection providers.
This function also acts as the single ECom entry point into this object.
@param aParentContainer The parent factory container which owns this factory
@return Factory for IP level meta connection providers
*/
CZeroCopyLoopBackTierManager* CZeroCopyLoopBackTierManager::NewL(ESock::CTierManagerFactoryBase& aFactory)
	{
 	return new (ELeave) CZeroCopyLoopBackTierManager(aFactory, ZeroCopyLoopbackTierManagerActivities::stateMap::Self());
	}


/**
Constructor for Network level meta connection providers.
@param aParentContainer The parent factory container which owns this factory
*/
CZeroCopyLoopBackTierManager::CZeroCopyLoopBackTierManager(ESock::CTierManagerFactoryBase& aFactory,
                                   const MeshMachine::TNodeActivityMap& aActivityMap)
:	CCoreTierManager(aFactory,aActivityMap)
	{
	LOG_NODE_CREATE(KZeroCopyLoopbackTierMgrTag, CZeroCopyLoopBackTierManager);
	}

CZeroCopyLoopBackTierManager::~CZeroCopyLoopBackTierManager()
	{
	LOG_NODE_DESTROY(KZeroCopyLoopbackTierMgrTag, CZeroCopyLoopBackTierManager);
	}

/**
Create selector for this Tier.
*/
MProviderSelector* CZeroCopyLoopBackTierManager::DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences)
	{
	__CFLOG_VAR((KZeroCopyLoopbackTierMgrTag, KZeroCopyLoopbackTierMgrSubTag, _L8("CZeroCopyLoopBackTierManager[%08x]::DoSelectProvider()"), this));
  	return TZeroCopyLoopbackSelectorFactory::NewSelectorL(aSelectionPreferences);
	}


void CZeroCopyLoopBackTierManager::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
	{
	TNodeContext<CZeroCopyLoopBackTierManager> ctx(*this, aMessage, aSender, aRecipient);
	CCoreTierManager::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}


