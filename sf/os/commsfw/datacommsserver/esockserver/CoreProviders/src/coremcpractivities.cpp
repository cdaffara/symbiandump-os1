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

#include "coremcpractivities.h"


#include <comms-infras/ss_log.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/coremcprstates.h>
#include <comms-infras/esock_params.h>
#include <comms-infras/ss_nodemessages_selector.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <comms-infras/ss_nodemessages_availability.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_tiermanagerfactory.h>
#include <comms-infras/ss_nodemessages_mcpr.h>

#include <elements/nm_messages_peer.h>
#include <elements/nm_messages_errorrecovery.h>
#include <elements/nm_messages_child.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <ss_glob.h>
#include "ss_internal_activities.h"
#include <commsdattypesv1_1.h>

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCoreProvcprac, "ESockCrPrvcprac");
#endif

#ifdef __CFLOG_ACTIVE
	#define KCoreMCprStatesTag KESockMetaConnectionTag
	_LIT8(KCoreMCprStatesSubTag, "coremcprstate");
#endif

using namespace ESock;
using namespace MCprActivities;
using namespace NetStateMachine;
using namespace PRActivities;
using namespace CorePanics;
using namespace Messages;
using namespace MeshMachine;


//
//Panics
void CoreMCprPanic(TInt aCode)
	{
	_LIT(KCoreMCprPanic, "CoreMCprPanic");
	User::Panic(KCoreMCprPanic, aCode);
	}


namespace MCprControlClientJoinActivity
{ //This activity needs the activity object (& it can fail on AddClientL, so no point converting)
DECLARE_DEFINE_NODEACTIVITY(ECFActivityClientJoin, MCprControlClientJoin, Messages::TNodeSignal::TNullMessageId) //May be waiting for both messages
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingControlClientJoin, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MCprStates::TDecrementBlockingDestroyAndAddControlClientAndSendJoinCompleteIfRequest)
NODEACTIVITY_END()
}

namespace MCprDataClientJoinActivity
{ //This activity needs the activity object (& it can fail on AddClientL, so no point converting)
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDataClientJoin, MCprDataClientJoin, TCFControlProvider::TJoinRequest)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientJoinRequest, MCprStates::TNoTagOrRejoinTag)
	LAST_NODEACTIVITY_ENTRY(MCprStates::KRejoinTag, MCprStates::TProcessDataClientRejoin)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MCprStates::TDecrementBlockingDestroyAndAddDataClientAndRespond)
NODEACTIVITY_END()
}

namespace MCprDestroyActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDestroy, MCprDestroy, Messages::TNodeSignal::TNullMessageId, CoreActivities::CDestroyActivity::New) //May be waiting for both messages
	FIRST_NODEACTIVITY_ENTRY(MCprStates::TAwaitingClientLeaveAndNoClients, MeshMachine::TNoTag)
    //MCprStates::TProcessClientLeave removes the client and marks the node for deletion.
	//If there are no servce providers it also terminates the activity.
	//If there are service providers it continues to following triples.
 	THROUGH_NODEACTIVITY_ENTRY(KNoTag, MCprStates::TProcessClientLeave, MeshMachine::TNoTag)
 	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingLeaveComplete, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
 	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendClientLeavingAndRemoveControlProvider)
NODEACTIVITY_END()
}

namespace MCprSimpleSelectActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySelect, MCprSimpleSelect, TCFSelector::TSimpleSelect, CSelectNextLayerActivity::NewL)
	//Reply from TAwaitingSelectNextLayer if no choices, otherwise accept
	FIRST_NODEACTIVITY_ENTRY(MCprStates::TAwaitingSelectNextLayer, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TProcessSimpleSelectionPolicy, MCprStates::TSelectedProvider)
	//Start the selection main loop
	NODEACTIVITY_ENTRY(MCprStates::KSelectedProvider, CSelectNextLayerActivity::TFindOrCreateTierManager, MCprStates::TAwaitingTierManagerCreated, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TJoinTierManager, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	//Select next provider and enter the selection internal loop if provider received. Break if SelectComplete(NULL).
	NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TSelectNextLayer, MCprStates::TAwaitingSelectComplete, CSelectNextLayerActivity::TNoTagOrSelectedProviderIsNull)
    NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TAddProviderInfo, MCprStates::TAwaitingSelectComplete, CSelectNextLayerActivity::TNoTagBackwardsOrJoinServiceProvider)
    //Break the selection internal loop if SelectComplete(NULL), otherwise stay in this tripple
    NODEACTIVITY_ENTRY(MCprStates::KJoinServiceProvider, CSelectNextLayerActivity::TJoinServiceProvider, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TSendSelectComplete, CSelectNextLayerActivity::TSelectedProviderIsNullOrJoinServiceProviderBackward)
	//Break the selection main loop if no more choices, otherwise go back again
	THROUGH_NODEACTIVITY_ENTRY(MCprStates::KSelectedProviderIsNull, CSelectNextLayerActivity::TLeaveTierManager, CSelectNextLayerActivity::TNoTagOrSelectedProviderBackward)
	//Finish the activity
	LAST_NODEACTIVITY_ENTRY(KNoTag, MCprStates::TSendFinalSelectComplete)
NODEACTIVITY_END()
}

namespace MCprSimpleSelectActivitySuper
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySelect, MCprSimpleSelect, TCFSelector::TSelect, CSelectNextLayerActivity::NewL)
	//Reply from TAwaitingSelectNextLayer if no choices, otherwise accept
	FIRST_NODEACTIVITY_ENTRY(MCprStates::TAwaitingSelectNextLayerSuper, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TProcessPrioritisedSelectionPolicySuper, MCprStates::TSelectedProvider)
	//Start the selection main loop
	NODEACTIVITY_ENTRY(MCprStates::KSelectedProvider, CSelectNextLayerActivity::TFindOrCreateTierManagerSuper, MCprStates::TAwaitingTierManagerCreated, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TJoinTierManager, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	//Select next provider and enter the selection internal loop if provider received. Break if SelectComplete(NULL).
	NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TSelectNextLayerSuper, MCprStates::TAwaitingSelectComplete, CSelectNextLayerActivity::TNoTagOrSelectedProviderIsNull)
    NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TAddProviderInfo, MCprStates::TAwaitingSelectComplete, CSelectNextLayerActivity::TNoTagBackwardsOrJoinServiceProvider)
    //Break the selection internal loop if SelectComplete(NULL), otherwise stay in this tripple
    NODEACTIVITY_ENTRY(MCprStates::KJoinServiceProvider, CSelectNextLayerActivity::TJoinServiceProvider, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TSendSelectComplete, CSelectNextLayerActivity::TSelectedProviderIsNullOrJoinServiceProviderBackward)
	//Break the selection main loop if no more choices, otherwise go back again
	THROUGH_NODEACTIVITY_ENTRY(MCprStates::KSelectedProviderIsNull, CSelectNextLayerActivity::TLeaveTierManager, CSelectNextLayerActivity::TNoTagOrSelectedProviderBackwardSuper)
	//Finish the activity
	LAST_NODEACTIVITY_ENTRY(KNoTag, MCprStates::TSendFinalSelectComplete)
NODEACTIVITY_END()
}

namespace MCprNoBearerActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityNoBearer, MCprNoBearer, TCFControlProvider::TNoBearer, PRActivities::CNoBearer::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingNoBearer, CNoBearer::TNoTagOrBearerPresentBlockedByNoBearer)
	NODEACTIVITY_ENTRY(KNoTag, MCprStates::TSelectNextLayer, MCprStates::TAwaitingSelectNextLayerCompleted, CoreNetStates::TNoTagOrBearerPresent)

	//Special for the Meta Plane (don't just copy & paste)
//	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TStartServiceProvider, CoreNetStates::TAwaitingStarted, TTag<CoreNetStates::KBearerPresent>)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TRequestCommsBinderRetry, CoreNetStates::TAwaitingBinderResponse, TTag<CoreNetStates::KBearerPresent>)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TSendBindTo, CoreNetStates::TAwaitingBindToComplete, TTag<CoreNetStates::KBearerPresent>)
	THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreActivities::ABindingActivity::TSendBindToComplete, MeshMachine::TNoTag)

	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBearer)
NODEACTIVITY_END()
}

namespace MCprReConnectActivity
{
//MCprConnectionStartRecovery activity belongs to a group of Error Recovery Activities.
//Error Recovery Activities need to handle their own errors (generated as well as returned).
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityReConnect, MCprReConnect, TCFMcpr::TReConnect, CReConnectActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(MCprStates::TAwaitingReConnectRequest, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CReConnectActivity::TProcessReConnectRequest, MeshMachine::TNoTag)

	//Build lower layer
	NODEACTIVITY_ENTRY(KNoTag, CReConnectActivity::TBuildLowerLayer, CoreNetStates::TAwaitingBindToComplete, MeshMachine::TNoTag)

	//Respond
	LAST_NODEACTIVITY_ENTRY(KNoTag, CReConnectActivity::TSendReConnectResponse)
NODEACTIVITY_END()
}

namespace MCprErrorRecoveryDefaultActivity
{
//This is a default error recovery activity.
//It always replies with TErrorRecoveryResponse(EPropagate).
DECLARE_DEFINE_NODEACTIVITY(ECFActivityErrorRecovery, DefaultErrorRecovery, TEErrorRecovery::TErrorRecoveryRequest)
	NODEACTIVITY_ENTRY(KNoTag, MCprStates::TSendPropagateRecoveryResponse, MCprStates::TAwaitingErrorRecoveryRequest, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace MCprBinderRequestActivity
{
// MCPRs can only have one data client, hence the use of CoreNetStates::TNoTagOrDataClientPresent.
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBinderRequest, MCprBinderRequest, TCFServiceProvider::TCommsBinderRequest, CCommsBinderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBinderRequest, CCommsBinderActivity::TNoTagOrUseExistingBlockedByBinderRequest)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TCreateDataClient, CoreNetStates::TAwaitingDataClientJoin, MeshMachine::TNoTag)

	// Below this point we need to modify the error handling approach. If we're getting a TError on TBinderResponse,
	// this means the client requesting the binder couldn't bind to it. As far as the client is concerned, this
	// activity is finished (it has flagged an error). The standard error handling will result in erroring
	// the originator. In this case we shouoldn't error the originator, instead, wrap up quietly.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CCommsBinderActivity::TProcessDataClientCreation, TTag<CoreStates::KUseExisting>)

	NODEACTIVITY_ENTRY(CoreStates::KUseExisting, CCommsBinderActivity::TSendBinderResponse, CCommsBinderActivity::TAwaitingBindToComplete, MeshMachine::TNoTagOrErrorTag)
    LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TClearError)
NODEACTIVITY_END()
}

namespace MCprReportProviderStatusActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityReportProviderStatus, MCprReportProviderStatus, TCFMcpr::TProviderStatusChangeRegistration)
	// for other requests "joining in"
	NODEACTIVITY_ENTRY(KNoTag, MCprStates::TProcessProviderStatusChangeRegistration, MeshMachine::TAwaitingAny, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingProviderStatusChangeOrDataClientStatusChange, MCprStates::TDataClientStatusChangeOrNoTag)
	// Process the provider status change notification sent from provider on left
    THROUGH_NODEACTIVITY_ENTRY(MCprStates::KDataClientStatusChange, PRStates::THandleDataClientStatusChangeAndDestroyOrphans, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, MCprStates::TPostStatusChangeToOriginators, CoreNetStates::TAwaitingProviderStatusChangeOrDataClientStatusChange, MCprStates::TDataClientStatusChangeBackwardsOrNoTagBackwards)
	// Never get past this point
NODEACTIVITY_END()
}

namespace MCprAvailabilityNotificationActivity
{
// Reference availability notification activity.
// Forwards availability registrations down to lower MCPRs
// Then combines their responses into a single TAvailability object and sends the notification upwards if anything has changed
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityAvailabilityNotification, MCprAvailability, TCFAvailabilityProvider::TAvailabilityNotificationRegistration, CAvailabilityActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(MeshMachine::TAwaitingMessageState<TCFAvailabilityProvider::TAvailabilityNotificationRegistration>, MeshMachine::TNoTag)

	//Select Service Providers (availability sources - if any)
	NODEACTIVITY_ENTRY(KNoTag, MCprStates::TSelectNextLayer, MCprStates::TAwaitingSelectNextLayerCompleted, MeshMachine::TNoTagOrErrorTag)

	//Register for notifications with all Service Providers (if any)
	NODEACTIVITY_ENTRY(KNoTag, CAvailabilityActivity::TRegisterForNotifications, MCprStates::TAwaitingAvailabilityNotificationOrError, TErrorTagOr<TTag<MCprStates::KProcessAvailability> >)

	//Process the availability notification and loop (if success, or finish on failure)
	NODEACTIVITY_ENTRY(MCprStates::KProcessAvailability, CAvailabilityActivity::TProcessNotification, MCprStates::TAwaitingAvailabilityNotificationOrError, TErrorTagOr<TTag<MCprStates::KProcessAvailability|EBackward> >)

	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TRaiseAndClearActivityError)
NODEACTIVITY_END()
}

// Also if this Message is not expected to be received from the MCpr, we
//	manage it.
namespace MCprDataClientGoneDownActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDataClientGoneDown, MCprDataClientGoneDown, TCFControlProvider::TDataClientGoneDown)
	// The only thing we do is to clear(unset) the "Flags" of
	//	the relative DataClient from "EStart": this is done
	//	in "TAwaitingDataClientGoneDown".
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientGoneDown, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace MCprStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, MCprStart, TCFServiceProvider::TStart, PRActivities::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStart, CoreNetStates::TNoTagOrBearerPresentBlockedByStop)
	NODEACTIVITY_ENTRY(KNoTag, MCprStates::TSelectNextLayer, MCprStates::TAwaitingSelectNextLayerCompleted, CoreNetStates::TNoTagOrBearerPresent)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TBindSelfToPresentBearer, CoreNetStates::TAwaitingBindToComplete, TTag<CoreNetStates::KBearerPresent>)

	//Start the service provider, use the default cancellation.
	//Forward TCancel to the service provider, wait for TStarted or TError (via the Error Activity)
	//When TStarted arrives after TCancel the activity will move to the nearest KErrorTag
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TStartServiceProviderRetry, CoreNetStates::TAwaitingStarted, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
	//Start data clients, use the default cancellation.
	//Forward TCancel to the self, wait for TCFDataClient::TStarted or TError (via the Error Activity)
	//When TCFDataClient::TStarted arrives after TCancel the activity will move to the nearest KErrorTag
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStartSelf, CoreNetStates::TAwaitingDataClientStarted, MeshMachine::TNoTagOrErrorTag)
	NODEACTIVITY_ENTRY(KErrorTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStarted)
NODEACTIVITY_END()
}

namespace MCprStopActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStop, MCprStop, TCFServiceProvider::TStop, MeshMachine::CNodeRetryActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStop, CoreNetStates::TActiveOrNoTagBlockedByBindTo)
	THROUGH_NODEACTIVITY_ENTRY(KActiveTag, CoreNetStates::TCancelDataClientStart, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, CoreNetStates::TNoTagOrNoBearer)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStop, CoreNetStates::TAwaitingStopped, TTag<CoreNetStates::KNoBearer>)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KNoBearer, PRStates::TSendStoppedAndGoneDown)
NODEACTIVITY_END()
}

namespace MCprActivities
{
//This activity map should be used if the MCPR only supports the
//selection message TSelectNextLayerConnPrefList. Nodes using this activity
//map will not support legacy selection
DEFINE_EXPORT_ACTIVITY_MAP(coreMCprActivitiesSuper)
	ACTIVITY_MAP_ENTRY(MCprDestroyActivity, MCprDestroy)
	ACTIVITY_MAP_ENTRY(MCprSimpleSelectActivitySuper, MCprSimpleSelect)
	ACTIVITY_MAP_ENTRY(MCprNoBearerActivity, MCprNoBearer)
	ACTIVITY_MAP_ENTRY(MCprReConnectActivity, MCprReConnect)
	ACTIVITY_MAP_ENTRY(MCprAvailabilityNotificationActivity, MCprAvailability)
	ACTIVITY_MAP_ENTRY(MCprBinderRequestActivity, MCprBinderRequest)
	ACTIVITY_MAP_ENTRY(MCprReportProviderStatusActivity, MCprReportProviderStatus)
	ACTIVITY_MAP_ENTRY(MCprDataClientJoinActivity, MCprDataClientJoin)
	ACTIVITY_MAP_ENTRY(MCprControlClientJoinActivity, MCprControlClientJoin)
	ACTIVITY_MAP_ENTRY(MCprErrorRecoveryDefaultActivity, DefaultErrorRecovery)
	ACTIVITY_MAP_ENTRY(MCprDataClientGoneDownActivity, MCprDataClientGoneDown)
	ACTIVITY_MAP_ENTRY(MCprStopActivity, MCprStop)
	ACTIVITY_MAP_ENTRY(MCprStartActivity, MCprStart)
ACTIVITY_MAP_END_BASE(PRActivities, coreActivitiesMCpr)

//This activiy map supports legacy selection and the additional activity
//is that support. It should be used where tier managers need to support
//selection message TSelectNextLayer
DEFINE_EXPORT_ACTIVITY_MAP(coreMCprActivities)
	ACTIVITY_MAP_ENTRY(MCprSimpleSelectActivity, MCprSimpleSelect)
ACTIVITY_MAP_END_BASE(MCprActivities, coreMCprActivitiesSuper)
}

//
//CSelectNextLayerActivity
EXPORT_C MeshMachine::CNodeActivityBase* CSelectNextLayerActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	CSelectNextLayerActivity* self = new (ELeave) CSelectNextLayerActivity(aActivitySig, aNode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CSelectNextLayerActivity::ConstructL()
	{
	iDbs = CommsDat::CMDBSession::NewL(KCDVersion1_2);

	}

EXPORT_C CSelectNextLayerActivity::~CSelectNextLayerActivity()
	{
	if (!iTierManager.IsNull())
		{
		// Only leave if the iTierManager is still a client - only happens in some
		// error cases for example if the Join fails the generic TError handling
		// does not call iTierManager.SetNull(), also if AddClientL() leaves.
		// TierManager is always a special case because the relationship with
		// MCPR is always temporary.
		RNodeInterface* client = iNode.FindClient(iTierManager);
		if (client)
			{
			RClientInterface::OpenPostMessageClose(iNode.Id(), iTierManager, TEChild::TLeft().CRef());
			iNode.RemoveClient(iTierManager);
			}
		iTierManager.SetNull();
		}
	delete iDbs;
	iSelectCompleteMessages.Close();
	}

EXPORT_C TConnIdList& CSelectNextLayerActivity::SelectionChoices()
	{
	return iSelectionChoices;
	}

EXPORT_C ESock::RConnPrefList& CSelectNextLayerActivity::SelectionConnPrefList()
	{
	return iSelectionConnPrefList;
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TNoTagOrSelectedProviderIsNull, NetStateMachine::MStateFork, CSelectNextLayerActivity::TContext)
TInt CSelectNextLayerActivity::TNoTagOrSelectedProviderIsNull::TransitionTag()
	{
	//We proceede forward through a transision that will process our concrete message received (NULL or not NULL).
	TInt tag = MeshMachine::KNoTag | NetStateMachine::EForward;
	if (message_cast<TCFSelector::TSelectComplete>(iContext.iMessage).iNodeId.IsNull())
		{
		tag = MCprStates::KSelectedProviderIsNull | NetStateMachine::EForward;
		}
	return tag;
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TNoTagBackwardsOrJoinServiceProvider, NetStateMachine::MStateFork, CSelectNextLayerActivity::TContext)
TInt CSelectNextLayerActivity::TNoTagBackwardsOrJoinServiceProvider::TransitionTag()
	{
	TInt tag = MeshMachine::KNoTag | NetStateMachine::EBackward;
	if (message_cast<TCFSelector::TSelectComplete>(iContext.iMessage).iNodeId.IsNull())
		{
		//This is the final select complete that we have received from the selector.
		tag = MCprStates::KJoinServiceProvider | NetStateMachine::EForward;
		}
	return tag;
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TSelectedProviderIsNullOrJoinServiceProviderBackward, NetStateMachine::MStateFork, CSelectNextLayerActivity::TContext)
TInt CSelectNextLayerActivity::TSelectedProviderIsNullOrJoinServiceProviderBackward::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);
	if (ac.iSelectCompleteMessages.Count() > 0)
		{
		return MCprStates::KJoinServiceProvider | NetStateMachine::EBackward;
		}

	return MCprStates::KSelectedProviderIsNull | NetStateMachine::EForward;
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TNoTagOrSelectedProviderBackward, NetStateMachine::MStateFork, CSelectNextLayerActivity::TContext)
TInt CSelectNextLayerActivity::TNoTagOrSelectedProviderBackward::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);
	if (ac.SelectionChoices().Count() > (ac.iCurrentIndex + 1))
		{
		return MCprStates::KSelectedProvider | NetStateMachine::EBackward;
		}

	return MeshMachine::KNoTag | NetStateMachine::EForward;
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TProcessSimpleSelectionPolicy, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TProcessSimpleSelectionPolicy::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);
	__ASSERT_DEBUG(ac.SelectionChoices().Count() == 0, User::Panic(KSpecAssert_ESockCoreProvcprac, 1)); //It should be empty

	//We are going to process our own selection policy to determine choices
	//for selection of the next layer.

	//This simple transition interprets the selection policy id
	//directly as a next layer access point id.

	//This transition is an entry point to the main selection loop which
	//will process all the inforamtion gathered in this transition.
	//We initialise the current index to start from the beginning of the list
	//(next step preincrements).
	ac.iCurrentIndex = -1;

	if (iContext.Node().AccessPointConfig().FindExtension(TOverridenSelectionPrefsExt::TypeId()))
		{
		//Our selection has been overriden by special preferences (so called pass-through
		//preference) given to us by our control client.
		__CFLOG_VAR((KCoreMCprStatesTag, KCoreMCprStatesSubTag, _L8("TProcessSimpleSelectionPolicy::DoL() - Selection policy overriden!.")));
		return;
		}

	//Now we can interpret the selection policy and store the results for further
	//processing
	const TLayerSelectionInfo* selectInfo = static_cast<const TLayerSelectionInfo*>(iContext.Node().AccessPointConfig().FindExtension(
			STypeId::CreateSTypeId(TLayerSelectionInfo::EUid, TLayerSelectionInfo::ETypeId)));
	__ASSERT_ALWAYS(selectInfo, CoreMCprPanic(KPanicUnexpectedExecutionPath));

	TUint selectionPolicyId = selectInfo->CustomSelectionPolicy();

	//Running this transition, we can't be at the bottom of the stack!
	__ASSERT_DEBUG(selectionPolicyId, User::Panic(KSpecAssert_ESockCoreProvcprac, 2));

	//And, of course, for nodes using this transition, it is only one single
	//access point below.
	ac.SelectionChoices().Append(selectionPolicyId);
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TProcessPrioritisedSelectionPolicy, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TProcessPrioritisedSelectionPolicy::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);
	__ASSERT_DEBUG(ac.SelectionChoices().Count() == 0, User::Panic(KSpecAssert_ESockCoreProvcprac, 3)); //It should be empty

	//We are going to process our own selection policy to determine choices
	//for selection of the next layer.

	//This simple transition interprets the selection policy id
	//directly as a next layer access point id.

	//This transition is an entry point to the main selection loop which
	//will process all the inforamtion gathered in this transition.
	//We initialise the current index to start from the beginning of the list
	//(next step preincrements).
	ac.iCurrentIndex = -1;

	if (iContext.Node().AccessPointConfig().FindExtension(TOverridenSelectionPrefsExt::TypeId()))
		{
		//Our selection has been overriden by special preferences (so called pass-through
		//preference) given to us by our control client.
		__CFLOG_VAR((KCoreMCprStatesTag, KCoreMCprStatesSubTag, _L8("TProcessPrioritisedSelectionPolicy::DoL() - Selection policy overriden!.")));
		return;
		}

	//Now we can interpret the selection policy and store the results for further
	//processing
	const TLayerSelectionInfo* selectInfo = static_cast<const TLayerSelectionInfo*>(iContext.Node().AccessPointConfig().FindExtension(
			STypeId::CreateSTypeId(TLayerSelectionInfo::EUid, TLayerSelectionInfo::ETypeId)));
	__ASSERT_ALWAYS(selectInfo, CoreMCprPanic(KPanicUnexpectedExecutionPath));

	TUint selectionPolicyId = selectInfo->SelectionPolicy();

	//Running this transition, we can't be at the bottom of the stack!
	__ASSERT_DEBUG(selectionPolicyId, User::Panic(KSpecAssert_ESockCoreProvcprac, 4));

	//And, of course, for nodes using this transition, all of the choices come
	//from the simple prioritised selection policy
	TierManagerUtils::FillListL(ac.SelectionChoices(),selectionPolicyId,ac.Dbs());
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TFindOrCreateTierManager, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TFindOrCreateTierManager::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	//Preincrement the current index
	++ac.iCurrentIndex;

	//If we do not have our choices now, this may only mean that our selection
	//policy has been overriden by our control client.
	TUid tierId;
	if (ac.SelectionChoices().Count())
		{
		__ASSERT_DEBUG(ac.SelectionChoices().Count() > ac.iCurrentIndex, User::Panic(KSpecAssert_ESockCoreProvcprac, 5)); //Specified choice must be present
		__ASSERT_DEBUG(iContext.Node().AccessPointConfig().FindExtension(TOverridenSelectionPrefsExt::TypeId()) == NULL, User::Panic(KSpecAssert_ESockCoreProvcprac, 6));
		tierId = TierManagerUtils::ReadTierIdL(ac.SelectionChoices().Get(ac.iCurrentIndex), ac.Dbs()) ;
		}
	else
		{
		//Our policy has been overriden
		const TOverridenSelectionPrefsExt& override = static_cast<const TOverridenSelectionPrefsExt&>(iContext.Node().AccessPointConfig().FindExtensionL(TOverridenSelectionPrefsExt::TypeId()));
		tierId = override.iTierId;
		}

	//The tier id must be set now!
	__ASSERT_DEBUG(tierId.iUid!=0, User::Panic(KSpecAssert_ESockCoreProvcprac, 7));
	User::LeaveIfError(tierId.iUid!=0? KErrNone : KErrCorrupt); //Check your configuration!
	TAlwaysFindFactoryQuery query;
	iContext.iNodeActivity->PostRequestTo(SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)), TCFFactory::TFindOrCreatePeer(TCFPlayerRole::ETierMgrPlane, tierId, &query).CRef(), EFalse);
	}

//This is a default implementation of the TSelectNextLayer transition,
//Other mcprs, especially those with custom selection policies may want
//to use their own logic to populate the selection preferences for the next layer.
EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TSelectNextLayer, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TSelectNextLayer::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	if (ac.SelectionChoices().Count())
		{
		//We use choices derived from our own selection policy
		__ASSERT_DEBUG(ac.SelectionChoices().Count() > ac.iCurrentIndex, User::Panic(KSpecAssert_ESockCoreProvcprac, 8)); //Specified choice must be present
		__ASSERT_DEBUG(iContext.Node().AccessPointConfig().FindExtension(TOverridenSelectionPrefsExt::TypeId())==NULL, User::Panic(KSpecAssert_ESockCoreProvcprac, 9));

		TConnIdList list;
		list.Append(ac.SelectionChoices().Get(ac.iCurrentIndex));

		TSelectionPrefs newPrefs;
		newPrefs.SetPrefs(list);

		iContext.iNodeActivity->PostRequestTo(ac.iTierManager, TCFSelector::TSimpleSelect(newPrefs).CRef());
		}
	else
		{
		//We have been given preferences for the next layer and we use them here
		const TOverridenSelectionPrefsExt& override = static_cast<const TOverridenSelectionPrefsExt&>(iContext.Node().AccessPointConfig().FindExtensionL(TOverridenSelectionPrefsExt::TypeId()));

		iContext.iNodeActivity->PostRequestTo(ac.iTierManager, TCFSelector::TSimpleSelect(override.iPrefs).CRef());
		}
	}
EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TProcessSimpleSelectionPolicySuper, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TProcessSimpleSelectionPolicySuper::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	//We are going to process our own selection policy to determine choices
	//for selection of the next layer.

	//This simple transition interprets the selection policy id
	//directly as a next layer access point id.

	//This transition is an entry point to the main selection loop which
	//will process all the inforamtion gathered in this transition.

	//Now we can interpret the selection policy and store the results for further
	//processing
	const TLayerSelectionInfo* selectInfo = static_cast<const TLayerSelectionInfo*>(iContext.Node().AccessPointConfig().FindExtension(
			STypeId::CreateSTypeId(TLayerSelectionInfo::EUid, TLayerSelectionInfo::ETypeId)));
	__ASSERT_ALWAYS(selectInfo, CoreMCprPanic(KPanicUnexpectedExecutionPath));

	TUint selectionPolicyId = selectInfo->CustomSelectionPolicy();

	//Running this transition, we can't be at the bottom of the stack!
	__ASSERT_DEBUG(selectionPolicyId, User::Panic(KSpecAssert_ESockCoreProvcprac, 10));

	//And, of course, for nodes using this transition, it is only one single
	//access point below.

	//This constructs a TConnAPPref to represent the access point to be created
 	ac.SelectionConnPrefList() = message_cast<TCFSelector::TSelect>(iContext.iMessage).iConnPrefList;

	if (selectionPolicyId != (TUint)CommsDat::CCDAccessPointRecord::KNoPolicy)
		{
		TConnAPPref* nextAP = TConnAPPref::NewL(selectionPolicyId);
		CleanupStack::PushL(nextAP);
		ac.SelectionConnPrefList().AppendL(nextAP);
		CleanupStack::Pop();
		}

	//The current index is set to the length of the list, this is decremented
	//in the next step so the correct tier manager is created (the one
	//relating to this access point
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TProcessPrioritisedSelectionPolicySuper, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TProcessPrioritisedSelectionPolicySuper::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	ac.SelectionConnPrefList() = message_cast<TCFSelector::TSelect>(iContext.iMessage).iConnPrefList;

	//We are going to process our own selection policy to determine choices
	//for selection of the next layer.

	//This simple transition interprets the selection policy id
	//directly as a next layer access point id.

	//Now we can interpret the selection policy and store the results for further
	//processing
	const TLayerSelectionInfo* selectInfo = static_cast<const TLayerSelectionInfo*>(iContext.Node().AccessPointConfig().FindExtension(
			STypeId::CreateSTypeId(TLayerSelectionInfo::EUid, TLayerSelectionInfo::ETypeId)));
	__ASSERT_ALWAYS(selectInfo, CoreMCprPanic(KPanicUnexpectedExecutionPath));
	TUint selectionPolicyId = selectInfo->SelectionPolicy();

	//Running this transition, we can't be at the bottom of the stack!
	__ASSERT_DEBUG(selectionPolicyId, User::Panic(KSpecAssert_ESockCoreProvcprac, 11));

	//And, of course, for nodes using this transition, all of the choices come
	//from the simple prioritised selection policy

	TierManagerUtils::FillListL(ac.SelectionConnPrefList(),selectionPolicyId,ac.Dbs());
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TFindOrCreateTierManagerSuper, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TFindOrCreateTierManagerSuper::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	//This takes the first TConnAPPref from the list and creates the tier manager it
	//is associated with.
	ESock::RConnPrefList::TIter<TConnAPPref> iterAP = ac.SelectionConnPrefList().getIter<TConnAPPref>();

	__ASSERT_DEBUG(!iterAP.IsEnd(), User::Panic(KSpecAssert_ESockCoreProvcprac, 12));

	TUid tierId = TierManagerUtils::ReadTierIdL(iterAP->GetAP(), ac.Dbs()) ;

	//The tier id must be set now!
	__ASSERT_DEBUG(tierId.iUid!=0, User::Panic(KSpecAssert_ESockCoreProvcprac, 13));
	User::LeaveIfError(tierId.iUid!=0? KErrNone : KErrCorrupt); //Check your configuration!
	TAlwaysFindFactoryQuery query;
	iContext.iNodeActivity->PostRequestTo(SockManGlobals::Get()->GetPlaneFC(
		TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)),
		TCFFactory::TFindOrCreatePeer(TCFPlayerRole::ETierMgrPlane, tierId, &query).CRef(), EFalse);
	}


EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TSelectNextLayerSuper, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TSelectNextLayerSuper::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	// We should not have got here if the list is empty
	__ASSERT_DEBUG(ac.SelectionConnPrefList().Count() != 0, User::Panic(KSpecAssert_ESockCoreProvcprac, 14));

	// The handle to the complete list is passed to the tier manager
	iContext.iNodeActivity->PostRequestTo(ac.iTierManager, TCFSelector::TSelect(ac.SelectionConnPrefList()).CRef());
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TNoTagOrSelectedProviderBackwardSuper, NetStateMachine::MStateFork, CSelectNextLayerActivity::TContext)
EXPORT_C TInt CSelectNextLayerActivity::TNoTagOrSelectedProviderBackwardSuper::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);
	ESock::RConnPrefList::TIter<TConnAPPref> iterAP = ac.SelectionConnPrefList().getIter<TConnAPPref>();

	if (!iterAP.IsEnd())
		{
		return MCprStates::KSelectedProvider | NetStateMachine::EBackward;
		}

	return MeshMachine::KNoTag | NetStateMachine::EForward;
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TAddProviderInfo, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TAddProviderInfo::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	TCFSelector::TSelectComplete& msg = message_cast<TCFSelector::TSelectComplete>(iContext.iMessage);
	ac.iSelectCompleteMessages.AppendL(msg);
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TJoinTierManager, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TJoinTierManager::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	ac.iTierManager = message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage).iNodeId;
    __ASSERT_DEBUG(!ac.iTierManager.IsNull(), User::Panic(KSpecAssert_ESockCoreProvcprac, 15)); //Must always be valid.

	//Ensure that the MCPR remains active by adding the lower TierManager as an
	//auxiliary client.  This is only a temporary association with the TierManager
    //in the layer below and is only active for the duration of the select.
    //Note that this might leave resulting in ac.iTierManager non-Null in destructor
    iContext.Node().AddClientL(ac.iTierManager, TClientType(TCFClientType::EAux));
    ac.PostRequestTo(ac.iTierManager, TCFPeer::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::ECtrl)).CRef());
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TJoinServiceProvider, NetStateMachine::MStateTransition, MCprStates::TContext)
void CSelectNextLayerActivity::TJoinServiceProvider::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	__ASSERT_DEBUG(ac.iSelectCompleteMessages.Count() > 0, User::Panic(KSpecAssert_ESockCoreProvcprac, 16));
	ESock::TSigSelectComplete& msg = ac.iSelectCompleteMessages[0];

	//This transition assumes that the first service provider supplied will be the chosen one (EActive denotes 'the' service provider).
	TInt flags = 0;
	if (iContext.Node().ServiceProvider() == NULL)
		{
		flags = TCFClientType::EActive;
		}

	RNodeInterface* client = iContext.Node().AddClientL(msg.iNodeId, TClientType(TCFClientType::EServProvider, flags), &msg.iProviderInfo);

	iContext.iNodeActivity->PostRequestTo(*client, TCFServiceProvider::TJoinRequest(iContext.NodeId(), TCFClientType::ECtrl).CRef());
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TSendSelectComplete, NetStateMachine::MStateTransition, MCprStates::TContext)
void CSelectNextLayerActivity::TSendSelectComplete::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);

	__ASSERT_DEBUG(ac.iSelectCompleteMessages.Count() > 0, User::Panic(KSpecAssert_ESockCoreProvcprac, 17));
	ESock::TSigSelectComplete& selectcompletemsg = ac.iSelectCompleteMessages[0];
	iContext.iNodeActivity->PostToOriginators(TCFSelector::TSelectComplete(iContext.iPeer->RecipientId(), selectcompletemsg.iProviderInfo).CRef());
	ac.iSelectCompleteMessages.Remove(0);
	}

EXPORT_DEFINE_SMELEMENT(CSelectNextLayerActivity::TLeaveTierManager, NetStateMachine::MStateTransition, CSelectNextLayerActivity::TContext)
void CSelectNextLayerActivity::TLeaveTierManager::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>(*iContext.iNodeActivity);
    __ASSERT_DEBUG(!ac.iTierManager.IsNull(), User::Panic(KSpecAssert_ESockCoreProvcprac, 18)); //Must always be valid.
    ac.PostRequestTo(ac.iTierManager, TEChild::TLeft().CRef());
    iContext.Node().RemoveClient(ac.iTierManager);
    ac.iTierManager.SetNull();

	}
//
//CReConnectActivity
CReConnectActivity::CReConnectActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
:	MeshMachine::CNodeActivityBase(aActivitySig, aNode)
	{
	}

MeshMachine::CNodeActivityBase* CReConnectActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	return new(ELeave)CReConnectActivity(aActivitySig, aNode);
	}

void CReConnectActivity::StartL(TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const TStateTriple& aFirst)
	{
	//This activity provides service for only one single requestor at a time.
	__ASSERT_DEBUG(iOriginators.Count()==0, User::Panic(KSpecAssert_ESockCoreProvcprac, 19)); //Diagnostic panic
	User::LeaveIfError(iOriginators.Count()? KErrInUse : KErrNone);
  	iOriginators.AppendL(aOriginator);
  	MESH_LOG_CONTEXT_EXT(KESockMeshMachine, aContext, (_L8("CReConnectActivity %08x:\tStartL->starting activity"), this));
	__ASSERT_DEBUG(IsIdle(), User::Panic(KSpecAssert_ESockCoreProvcprac, 20));
	NetStateMachine::ACore::Start(&aContext, aFirst);
  	MESH_LOG((KESockMeshMachine, _L8("CReConnectActivity %08x:\tStartL->activity started"), this));
	}

DEFINE_SMELEMENT(CReConnectActivity::TProcessReConnectRequest, NetStateMachine::MStateTransition, CReConnectActivity::TContext)
void CReConnectActivity::TProcessReConnectRequest::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CReConnectActivity& activity = static_cast<CReConnectActivity&>(*iContext.iNodeActivity);
	TCFMcpr::TReConnect& msg = message_cast<TCFMcpr::TReConnect>(iContext.iMessage);

	activity.iStoppingSP = iContext.Node().FindClientL(msg.iNodeId1);
	__ASSERT_DEBUG(activity.iStoppingSP->Type()&TCFClientType::EServProvider, User::Panic(KSpecAssert_ESockCoreProvcprac, 21));

	activity.iStartingSP = iContext.Node().FindClientL(msg.iNodeId2);
	__ASSERT_DEBUG(activity.iStartingSP->Type()&TCFClientType::EServProvider, User::Panic(KSpecAssert_ESockCoreProvcprac, 22));
	}

DEFINE_SMELEMENT(CReConnectActivity::TBuildLowerLayer, NetStateMachine::MStateTransition, CReConnectActivity::TContext)
void CReConnectActivity::TBuildLowerLayer::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CReConnectActivity& activity = static_cast<CReConnectActivity&>(*iContext.iNodeActivity);

	//Post a TBuildLowerLayer to self, initiating building of the new layer below
	//Use the new service provider to prime the new layer
	__ASSERT_DEBUG(activity.iStartingSP, User::Panic(KSpecAssert_ESockCoreProvcprac, 25));
	iContext.iNodeActivity->PostRequestTo(iContext.NodeId(), TCFDataClient::TBindTo(activity.iStartingSP->RecipientId()).CRef());
	}


DEFINE_SMELEMENT(CReConnectActivity::TSendReConnectResponse, NetStateMachine::MStateTransition, CReConnectActivity::TContext)
void CReConnectActivity::TSendReConnectResponse::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	iContext.iNodeActivity->PostToOriginators(TCFMcpr::TReConnectComplete().CRef());
	}


//
//CAvailabilityActivity
MeshMachine::CNodeActivityBase* CAvailabilityActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    return new (ELeave) CAvailabilityActivity(aActivitySig, aNode);
    }

CAvailabilityActivity::~CAvailabilityActivity()
	{
	static_cast<CCoreMetaConnectionProvider&>(iNode).CancelAvailabilityMonitoring();

	//Unmark all remaining availability providers
	TClientIter<TFlagsOnlyClientMatchPolicy> iter = iNode.GetClientIter<TFlagsOnlyClientMatchPolicy>(TClientType(0, TCFClientType::EAvailabilityProvider));
	RNodeInterface* provider = iter++;
	while (provider)
		{
		provider->ClearFlags(TCFClientType::EAvailabilityProvider);
		provider = static_cast<RNodeAvailabilityProviderInterface*>(iter++);
		}
	}

void CAvailabilityActivity::StartL(TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const TStateTriple& aFirst)
	{
	TCFAvailabilityProvider::TAvailabilityNotificationRegistration& msg =
		message_cast<TCFAvailabilityProvider::TAvailabilityNotificationRegistration>(aContext.iMessage);

	CSubscriberInfo* info = new (ELeave) CSubscriberInfo;
	const_cast<Messages::XNodePeerId&>(aOriginator).iInfo = info;
	info->SetSubscriptionOptions(msg.iAvailabilitySubscriptionOptions);
	CleanupStack::PushL(info);
	MeshMachine::CNodeActivityBase::StartL(aContext, aOriginator, aFirst);
	CleanupStack::Pop(info);

	if (IsAvailabilityKnown())
		{
		info->SetReportedStatus(iCurrentAvailabilityStatus);
		aContext.PostToSender(TCFAvailabilityControlClient::TAvailabilityNotification(iCurrentAvailabilityStatus).CRef());
		}
	}

void CAvailabilityActivity::Cancel(TNodeContextBase& aContext)
	{
	//Post TCancel to all availability providers
	iNode.PostToClients<TFlagsOnlyClientMatchPolicy>(TNodeCtxId(ActivityId(), iNode.Id()), TEBase::TCancel().CRef(),
		TClientType(0, TCFClientType::EAvailabilityProvider));
	MeshMachine::CNodeActivityBase::Cancel(aContext);
	}

void CAvailabilityActivity::CalculateCurrentAvailabilityStatus()
	{
	iCurrentAvailabilityStatus.SetUnknown();
	TClientIter<TFlagsOnlyClientMatchPolicy> iter = iNode.GetClientIter<TFlagsOnlyClientMatchPolicy>(TClientType(0, TCFClientType::EAvailabilityProvider));
	RNodeAvailabilityProviderInterface* provider = static_cast<RNodeAvailabilityProviderInterface*>(iter++);
	__ASSERT_DEBUG(provider, User::Panic(KSpecAssert_ESockCoreProvcprac, 26)); //There must be at least availability provider registered at this stage (or there are no originators == this activity should not run)
	while (provider)
		{
		__ASSERT_DEBUG(provider->Type()&(TCFClientType::EServProvider|TClientType::ERegistrar), User::Panic(KSpecAssert_ESockCoreProvcprac, 27));
		const TAvailabilityStatus& availabilityStatus = provider->AvailabilityStatus();
		if (!availabilityStatus.IsKnown())
			{
			//If the immediate response from any of the remaining providers is still in transport,
			//set the whole as unknown.
			iCurrentAvailabilityStatus.SetUnknown();
			return;
			}
		else if (availabilityStatus.Score() > iCurrentAvailabilityStatus.Score())
			{
			iCurrentAvailabilityStatus.SetScore(availabilityStatus.Score());
			}
		provider = static_cast<RNodeAvailabilityProviderInterface*>(iter++);
		}
	}

TBool CAvailabilityActivity::IsAvailabilityKnown() const
	{
	return iCurrentAvailabilityStatus.IsKnown();
	}

void CAvailabilityActivity::NotifyInterestedSubscribers()
	{
	__ASSERT_DEBUG(IsAvailabilityKnown(), User::Panic(KSpecAssert_ESockCoreProvcprac, 28)); //Use only with known availability!
	TCFAvailabilityControlClient::TAvailabilityNotification msg(iCurrentAvailabilityStatus); //KActivityNull will be overriden

	for (TInt i = iOriginators.Count() - 1; i >= 0; --i)
		{
		XNodePeerId& originator = iOriginators[i];
		CSubscriberInfo* info = static_cast<CSubscriberInfo*>(originator.iInfo);
		__ASSERT_DEBUG(info, User::Panic(KSpecAssert_ESockCoreProvcprac, 29));

		if (!info->ReportedStatus().IsKnown()
			|| info->SubscriptionOptions().IsChangeSignificant(info->ReportedStatus(), iCurrentAvailabilityStatus))
			{
			info->SetReportedStatus(iCurrentAvailabilityStatus);
			originator.PostMessage(TNodeCtxId(ActivityId(), iNode.Id()), msg);
			}
		}
	}

void CAvailabilityActivity::RegisterForNotifications(TClientIterBase& aClientIter) const
	{
	RNodeAvailabilityProviderInterface* provider = static_cast<RNodeAvailabilityProviderInterface*>(aClientIter++);

	//We can ignore the fact that tere is no service providers we can register with, provided
	//the node has ensured there are some other availability providers!
	//If there are no service nor other availability providers the condition is serious.
	//__ASSERT_ALWAYS is required as the verified conditions are of a run time nature
	__ASSERT_ALWAYS(provider!=NULL || iNode.CountClients<TFlagsOnlyClientMatchPolicy>(TClientType(0, TCFClientType::EAvailabilityProvider))>0,
		CoreMCprPanic(KPanicNoAvailabilityProvider));

	TAvailabilitySubscriptionOptions subscriptionOptions; //By default register for all notifications - may need to be modified in the future
	TCFAvailabilityProvider::TAvailabilityNotificationRegistration msg(subscriptionOptions);

	//Iterate through service providers if any, setting EAvailabilityProvider flags
	while (provider)
		{
		provider->SetFlags(TCFClientType::EAvailabilityProvider);
		provider->PostMessage(TNodeCtxId(ActivityId(), iNode.Id()), msg);
		provider = static_cast<RNodeAvailabilityProviderInterface*>(aClientIter++);
		}
	}

DEFINE_SMELEMENT(CAvailabilityActivity::TRegisterForNotifications, NetStateMachine::MStateTransition, CAvailabilityActivity::TContext)
void CAvailabilityActivity::TRegisterForNotifications::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CAvailabilityActivity& activity = static_cast<CAvailabilityActivity&>(*iContext.iNodeActivity);

	//Start monitoring on the node if necessary
	iContext.Node().StartAvailabilityMonitoringL(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()));

	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
	activity.RegisterForNotifications(iter);
	}

DEFINE_SMELEMENT(CAvailabilityActivity::TProcessNotification, NetStateMachine::MStateTransition, CAvailabilityActivity::TContext)
void CAvailabilityActivity::TProcessNotification::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	CAvailabilityActivity& activity = static_cast<CAvailabilityActivity&>(*iContext.iNodeActivity);

	//Register with any new service providers (if any) == not with EAvailabilityProvider(s)
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider), TClientType(0, TCFClientType::EAvailabilityProvider));
	activity.RegisterForNotifications(iter);

	//Now the iPeer must be set (and we need to have the RNodeAvailabilityProviderInterface behind it)
	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCoreProvcprac, 30));
	TCFAvailabilityControlClient::TAvailabilityNotification& msg = message_cast<TCFAvailabilityControlClient::TAvailabilityNotification>(iContext.iMessage);

	//RNodeAvailabilityProviderInterface stores information (on this node) about the availability provider
	RNodeAvailabilityProviderInterface* provider = static_cast<RNodeAvailabilityProviderInterface*>(iContext.iPeer);
	__ASSERT_DEBUG(provider->Flags()&TCFClientType::EAvailabilityProvider, User::Panic(KSpecAssert_ESockCoreProvcprac, 31));
	provider->SetAvailabilityStatus(msg.iAvailabilityStatus);

	//Compute the overal availability and report if known already
	activity.CalculateCurrentAvailabilityStatus();
	if (activity.IsAvailabilityKnown())
		{
		activity.NotifyInterestedSubscribers();
		}
	}

