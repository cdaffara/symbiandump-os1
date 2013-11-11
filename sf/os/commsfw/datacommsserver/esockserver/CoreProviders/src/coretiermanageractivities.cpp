// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "coretiermanageractivities.h"

#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/coremcprstates.h>
#include <comms-infras/coretiermanagerstates.h>
#include <comms-infras/coretiernotificationactivity.h>
#include <comms-infras/ss_nodemessages_selector.h>
#include <comms-infras/ss_nodemessages_availability.h>
#include <comms-infras/ss_nodemessages_internal.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrPrvTMgrAC, "ESockCrPrvTMgrAC");
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace TMActivities;
using namespace Messages;
using namespace MeshMachine;
using namespace Elements;


namespace TMClientLeaveActivity
{ //This activity does not need the activity object and cannot fail
DEFINE_EXPORT_NODEACTIVITY(ECFActivityClientLeave, TMClientLeave, Messages::TNodeSignal::TNullMessageId)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TRemoveClient, CoreStates::TAwaitingClientLeave, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace TMSelectProviderActivitySuper
{ //Simple parallel activity provider selection, waits untill selection completes (via ISelectionNotify), then gets destroyed
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySelect, TMSelectProvider, TCFSelector::TSelect, CSelectProviderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(TMStates::TAwaitingSelectProviderSuper, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, TMStates::TSelectProviderSuper, CoreStates::TNeverAccept, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace TMSelectProviderActivity
{ //Simple parallel activity provider selection, waits untill selection completes (via ISelectionNotify), then gets destroyed
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySelect, TMSelectProvider, TCFSelector::TSimpleSelect, CSelectProviderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(TMStates::TAwaitingSelectProvider, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, TMStates::TSelectProvider, CoreStates::TNeverAccept, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

//
// Availability Notification
//
// The reference tier manager availability notification activity.
//  Forwards availability registrations down to MCPRs in tier
//  Then forwards their responses upwards to originators
//
namespace TMAvailabilityNotificationActivity_Forward
{
DEFINE_EXPORT_NODEACTIVITY(ECFActivityAvailabilityNotification, AvailabilityActivity_Forward, TCFAvailabilityProvider::TAvailabilityNotificationRegistration)

	FIRST_NODEACTIVITY_ENTRY(TMStates::TAwaitingRegistrationForTierAvailabilityNotification, MeshMachine::TNoTag)

	// Registers for notifications with all DataClient MCPRs
	NODEACTIVITY_ENTRY(KNoTag, TMStates::TJoinToMCPRsAndRegisterForAvailability, MCprStates::TAwaitingAvailabilityNotification, MeshMachine::TNoTag)

	// pass on all availability messages to originators
	NODEACTIVITY_ENTRY(KNoTag, TMStates::TPassAvailabilityNotificationToOriginators, MCprStates::TAwaitingAvailabilityNotification, MeshMachine::TNoTagBackward)

	// it may be possible that the tier can have MCPRs added / removed - this activity would have to have states to cope with this

	LAST_NODEACTIVITY_ENTRY(KNoTag, TMStates::TPassAvailabilityNotificationToOriginators)

NODEACTIVITY_END()
}




namespace TMActivities
{
//This activity map should be used if the Tier Manager only supports the
//selection message TSelectConnPrefList. Nodes using this activity
//map will not support legacy selection
DEFINE_EXPORT_ACTIVITY_MAP(coreTMActivitiesSuper)
	ACTIVITY_MAP_ENTRY(TMSelectProviderActivitySuper, TMSelectProvider)
	ACTIVITY_MAP_ENTRY(TMClientLeaveActivity, TMClientLeave)
	ACTIVITY_MAP_ENTRY(TMAvailabilityNotificationActivity_Forward, AvailabilityActivity_Forward)
	ACTIVITY_MAP_ENTRY(TMTierNotificationActivity, TierNotify)
	ACTIVITY_MAP_ENTRY(TMTierNotificationGetDataCollectorsActivity, GetCollectors)
ACTIVITY_MAP_END_BASE(PRActivities,coreActivitiesTM)

//This activity map should be used if the Tier Manager supports the legacy
//selection as well as TSelectConnPrefList message. Nodes using this activity
//map will support both forms of selection (this includes TSelect)
DEFINE_EXPORT_ACTIVITY_MAP(TMActivities::coreTMActivities)
	ACTIVITY_MAP_ENTRY(TMSelectProviderActivity, TMSelectProvider)
ACTIVITY_MAP_END_BASE(TMActivities,coreTMActivitiesSuper)
}


//
//CSelectProviderActivity
const TAnyFn CSelectProviderActivity::iInterfaceVTable[] =
	{
	(TAnyFn)ISelectionNotify::KCallbacksCount,
	(TAnyFn)(TSelectionNotify<CSelectProviderActivity>::SelectComplete),
	(TAnyFn)(TSelectionNotify<CSelectProviderActivity>::StateChangeNotification),
	(TAnyFn)(TSelectionNotify<CSelectProviderActivity>::SelectError)
	};

EXPORT_C MeshMachine::CNodeActivityBase* CSelectProviderActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
	CSelectProviderActivity* self = new (ELeave) CSelectProviderActivity(aActivitySig, aNode, c);
	return self;
	}

CSelectProviderActivity::~CSelectProviderActivity()
	{
	delete iSelector;
	}

EXPORT_C const TAnyFn& CSelectProviderActivity::InterfaceVTable()
	{
	return iInterfaceVTable[0];
	}

void CSelectProviderActivity::SelectComplete(MProviderSelector* aSelector, CMetaConnectionProviderBase* aProvider)
	{
	__ASSERT_DEBUG(iSelector==aSelector, User::Panic(KSpecAssert_ESockCrPrvTMgrAC, 1));
	(void)aSelector;
	if (aProvider)
		{
		PostToOriginators(TCFSelector::TSelectComplete(aProvider->Id(),aProvider->ProviderInfo()).CRef());
		}
	else
		{
		//This is the final SelectComplete()
		PostToOriginators(TCFSelector::TSelectComplete(TNodeId::NullId(),TProviderInfo()).CRef()); //KActivityNull will be overwritten
		delete iSelector;
		iSelector = NULL;
		SetIdle();
		}
	}

void CSelectProviderActivity::StateChangeNotification(MProviderSelector* aSelector, TInt aStage, TInt aError)
	{
	__ASSERT_DEBUG(iSelector==aSelector, User::Panic(KSpecAssert_ESockCrPrvTMgrAC, 2));
	(void)aSelector;
	PostToOriginators(TCFMessage::TStateChange(TStateChange(aStage,aError)).CRef()); //KActivityNull will be overwritten
	}

void CSelectProviderActivity::SelectError(MProviderSelector* aSelector, TInt aError)
	{
	__ASSERT_DEBUG(iSelector==aSelector, User::Panic(KSpecAssert_ESockCrPrvTMgrAC, 3));
	(void)aSelector;
	PostToOriginators(TEBase::TError(TCFSelector::TSelect::Id(),aError).CRef()); //KActivityNull will be overwritten
	delete iSelector;
	iSelector = NULL;
	SetIdle();
	}



