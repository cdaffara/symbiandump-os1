// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <elements/nm_messages_errorrecovery.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/mobilitymcpractivities.h>
#include <comms-infras/mobilitymcprstates.h>
#include <comms-infras/ss_nodemessages_selector.h>
#include <comms-infras/ss_nodemessages_mobility.h>
#include <comms-infras/ss_nodemessages_availability.h>
#include <comms-infras/ss_logext.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockMbCrMCPRAct, "ESockMbCrMCPRAct");
#endif

#ifdef __CFLOG_ACTIVE
	#define KCoreMCprStatesTag KESockMetaConnectionTag
	_LIT8(KCoreMCprStatesSubTag, "coremcprstate");
#endif

using namespace ESock;
using namespace CorePanics;
using namespace MCprStates;
using namespace NetStateMachine;
using namespace MCprActivities;
using namespace MobilityMCprActivities;
using namespace Messages;
using namespace MeshMachine;

///////////////////////////////////////////////////////////////////////////////
//Panics
#ifdef _DEBUG
_LIT (KCoreMobileMCprPanic,"CoreMobileMCprPanic");
#endif

namespace MobilityMCprPrioritisedSelectActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySelect, MCprPrioritisedSelect, TCFSelector::TSimpleSelect, CSelectNextLayerActivity::NewL)
	//Reply from TAwaitingSelectNextLayer if no choices, otherwise accept
	FIRST_NODEACTIVITY_ENTRY(MCprStates::TAwaitingSelectNextLayer, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CSelectNextLayerActivity::TProcessPrioritisedSelectionPolicy, MCprStates::TSelectedProvider)
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

namespace MobilityMCprMobilityActivity
{
//This activity monitors availability status on this node
//NOTE: This activity assumes there is only one data client (Cpr) of this MCpr!
//NOTE: This activity can only be executed in the context of CMobilityMetaConnectionProvider (or derived)
//NOTE: TError may come from the availability activity only. It is handled by the ECFActivityError.
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityMCprMobility, MCprMobility, TCFMobilityProvider::TStartMobility, MobilityMCprActivities::CMobilityActivity::NewL)
	//The activity only makes sense after the startup sequence completed on this layer
	FIRST_NODEACTIVITY_ENTRY(MobilityMCprStates::TAwaitingStartMobility, MeshMachine::TNoTag/*BlockedByNoServiceProviderStarted*/)
	//Report to the client that we have successfully started
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, MobilityMCprStates::TReplyMobilityStarted, MeshMachine::TNoTag)
	//Register with self for availability notifications. Self will report _any_ availabilty change (even available->available) back to
	//this activity. This activity can trigger mobility (see CMobilityActivity::TNoTagOrErrorTagOrStartMobilityHandshakeBackwardsOnMobilityTriggerBlockedByErrorRecovery)
	//if it sees that the availability notification has influcenced what the currently preffered bearer should be.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CMobilityActivity::TSendAvailabilityRequest, MeshMachine::TTag<MobilityMCprStates::KStartMobilityHandshake>)

	//<BEGIN> MAIN LOOP ****************
	//The main mobility handshake loop. The loop is executed when performing migration from one service provider to another.
	//The entry condition for the loop is that:
	//- upgrade: a better then current access point is now available (a better access point reported available)
	//- downgrade:
	//  (a) the current access point is being rejected by the client (e.g.: the current access point doesn't seem to route traffic where required)
	//  (b) the current access point ceases to be available (reports availability below reasonable threshold).
	//      NOTE: if the current bearer ceases to be available completely (goes down), then this will be assisted by an error recovery request;
	//      NOTE: This tuple doesn't actually do (b), i.e.: assumes the threshold of '1' (in 0..100 availability score range)
	//Before awaitng for availability change or rejection by the client (TAwaitingCurrentCarrierRejectedOrAvailabilityChange), the activity
	//first checks (TNoTagOrAwaitMobilityBlockedByErrorRecovery) if the availability has changed since it last checked
	//(availability could have been reported amidst the previous handshake loop)
	THROUGH_NODEACTIVITY_ENTRY(MobilityMCprStates::KStartMobilityHandshake, CMobilityActivity::TClearHandshakingFlag, CMobilityActivity::TNoTagOrAwaitMobilityBlockedByErrorRecovery)
	    NODEACTIVITY_ENTRY(MobilityMCprStates::KAwaitMobility, MeshMachine::TDoNothing, CMobilityActivity::TAwaitingCurrentCarrierRejectedOrAvailabilityChange, CMobilityActivity::TNoTagOrAwaitMobilityBackwardsOnMobilityTriggerBlockedByErrorRecovery)

		//Mobility has been triggered ((a) or (b)). Start mobility handshake (set handshaking flag and inform the client about the preferred bearer)
		//ReConnect only needs to be done if the bearer is different from the curret bearer.
		NODEACTIVITY_ENTRY(KNoTag, CMobilityActivity::TInformMigrationAvailableAndSetHandshakingFlag, MobilityMCprStates::TAwaitingMigrationRequestedOrRejected, CMobilityActivity::TNoTagOrReConnectOrStartMobilityHandshakeBackwards)
		//The client accepts the new access point.
		//The client accepts the new access point. 
		//For the moment it is sufficient to use the re-connect activity, in the future we may want to
		//customise the behavior, for example start the new layer before rebinding it, etc.
		//Should rebinding fail, the mobility activity will be set to an error mode. The error mode will be cleared if
		//there are other bearers this activity can offer. If there aren't the data client will be errored.
		NODEACTIVITY_ENTRY(MobilityMCprStates::KReConnect, CMobilityActivity::TRequestReConnect, MCprStates::TAwaitingReConnectCompleteOrError, CMobilityActivity::TNoTagOrStartMobilityHandshakeBackwards)
		//Rebinding has been successful. As far as MCPR is concerned, the mobility is finished, but the MCPR must await
		//for the handshake (accept|reject) before it can offer another bearer.
	NODEACTIVITY_ENTRY(KNoTag, CMobilityActivity::TInformMigrationCompleted, MobilityMCprStates::TAwaitingMigrationAcceptedOrRejected, CMobilityActivity::TRejectedOrStartMobilityHandshakeBackwards)
	NODEACTIVITY_ENTRY(MobilityMCprStates::KRejected, CoreNetStates::TStopDataClients, CoreNetStates::TAwaitingDataClientsStopped, MeshMachine::TTag<MobilityMCprStates::KStartMobilityHandshake|NetStateMachine::EBackward>)
NODEACTIVITY_END()
}

namespace MCprConnectionStartRecoveryActivity
{
//MCprConnectionStartRecovery activity belongs to a group of Error Recovery Activities.
//Error Recovery Activities need to handle their own errors (generated as well as returned).

DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityConnectionStartRecovery, MCprConnectionStartRecovery, TEErrorRecovery::TErrorRecoveryRequest, CConnectionRecoveryActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(MCprStates::TAwaitingConnectionStartRecoveryRequest, MobilityMCprStates::TNoTagOrErrorTagIfMobilityRunning)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, CConnectionRecoveryActivity::TSendIgnoreRecoveryResponse)

	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CConnectionRecoveryActivity::TStoreErrorContext, MeshMachine::TNoTag)
	//Decide if it it possible/sensible to reconnect and retry
	//This transition will leave if not possible to recover (==TSendPropagateRecoveryResponse from Transition::Error())
	NODEACTIVITY_ENTRY(KNoTag, CConnectionRecoveryActivity::TProcessConnectionStartRecoveryRequest, MCprStates::TAwaitingReConnectCompleteOrError, MeshMachine::TNoTagOrErrorTag) //Own error handling
	//Respond with retry
	LAST_NODEACTIVITY_ENTRY(KNoTag, CConnectionRecoveryActivity::TSendRetryRecoveryResponse)
	//Respond with propagate - the reconnect failed (we could think of re-trying reconnect again though..)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, CConnectionRecoveryActivity::TSendPropagateRecoveryResponse)
NODEACTIVITY_END()
}

namespace MCprConnectionGoneDownRecoveryActivity
{
//MCprConnectionGoneDownRecovery activity belongs to a group of Error Recovery Activities.
//Error Recovery Activities need to handle their own errors (generated as well as returned).

//NOTE: This activity is only a reference one. All it does it waits for the mobility handshake to finish before
//continuing with the stack cleanup originated by TGoneDown.
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityConnectionGoneDownRecovery, MCprConnectionGoneDownRecovery, TEErrorRecovery::TErrorRecoveryRequest, CConnectionRecoveryActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(MCprStates::TAwaitingConnectionGoneDownRecoveryRequest, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CConnectionRecoveryActivity::TStoreErrorContext, CoreStates::TNoTagOrNoPeer)
	LAST_NODEACTIVITY_ENTRY(CoreStates::KNoPeer, MCprStates::TSendPropagateRecoveryResponse) //Take error codes directly from the request
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MobilityMCprStates::TNoTagBlockedByMobilityHandshaking)
	//Decide if it it possible/sensible to retry
	//This transition will leave if not possible to recover (==TSendPropagateRecoveryResponse from Transition::Error())
	LAST_NODEACTIVITY_ENTRY(KNoTag, CConnectionRecoveryActivity::TProcessConnectionGoneDownRecoveryRequest) //Take error codes from the request directly
NODEACTIVITY_END()
}

namespace MobilityMCprActivities
{
DEFINE_EXPORT_ACTIVITY_MAP(mobilityMCprActivities)
	ACTIVITY_MAP_ENTRY(MobilityMCprPrioritisedSelectActivity, MCprPrioritisedSelect)
	ACTIVITY_MAP_ENTRY(MobilityMCprMobilityActivity, MCprMobility)
	ACTIVITY_MAP_ENTRY(MCprConnectionStartRecoveryActivity,MCprConnectionStartRecovery)
	ACTIVITY_MAP_ENTRY(MCprConnectionGoneDownRecoveryActivity,MCprConnectionGoneDownRecovery)
ACTIVITY_MAP_END_BASE(MCprActivities, coreMCprActivities)
}

///////////////////////////////////////////////////////////////////////////////
// CMobilityActivity
MeshMachine::CNodeActivityBase* MobilityMCprActivities::CMobilityActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	return new (ELeave) CMobilityActivity(aActivitySig, aNode);
	}

CMobilityActivity::CMobilityActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
:	MeshMachine::CNodeRetryActivity(aActivitySig, aNode),
	//NOTE: This reference implementation will currently only react to availability oscilating around
	//the middle point on the availability scale
	iAvailabilityScoreTreshold((TAvailabilityStatus::EMinAvailabilityScore + TAvailabilityStatus::EMaxAvailabilityScore) / 2)
	{
	}

CMobilityActivity::~CMobilityActivity()
	{
	//cancel availablilty subscription.
    RClientInterface::OpenPostMessageClose(TNodeCtxId(ActivityId(), iNode.Id()), iNode.Id(), TEBase::TCancel().CRef());
	ClearHandshakingFlag();
	}

TBool CMobilityActivity::EvaluatePreference(CMobilityActivity::TContext& aContext)
	{
	//Find the most preferred Service Provider
	TClientIter<TDefaultClientMatchPolicy> iter = iNode.GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
	__ASSERT_DEBUG(iter[0], User::Panic(KCoreMobileMCprPanic, KPanicNoServiceProvider)); //A Service Provider must exist!

	//If we are evaluating the preferences as a result of carrier rejection, we will
	//not propose the most recently rejected one.
	//NOTE: This implementation does not provide a blacklisting mechanism.
	//It does not store any blacklisting information.
	//lastRejected is only the recently rejected carrier that may not be proposed again for the client
	//to be able to renegotiate the old bearer and continue using the connection.
	//NOTE: This reference implementation will work only when at least one of the two most preferred carriers
	//can be used (accepted) by the mobility client at any given time.
	RMetaServiceProviderInterface* candidate = NULL;
	RMetaServiceProviderInterface* lastRejected = NULL;
	if ( Error() != KErrNone )
	    {
	    //The activity is running in an error mode attempting to recover from it.
	    //There's a couple of reasons why the activity may be in an error mode:
	    //- rejection
	    //  - current bearer rejected;
	    //  - proposed bearer rejected;
	    //  - failure to migrate to the proposed bearer;
        lastRejected = iAvailable ? iAvailable :
                                    static_cast<RMetaServiceProviderInterface*>(aContext.Node().ServiceProvider());
	    }

	iCandidate = iAvailable;
	iAvailable = NULL; //Do not remember rejected candidate any longer
	while ((candidate = static_cast<RMetaServiceProviderInterface*>(iter++)) != NULL)
		{
		const TAvailabilityStatus& status = candidate->AvailabilityStatus();
		if (!status.IsKnown())
			{
			//We are still waiting for the availability check results for this AP
			//Ignore the whole evaluation now as we may soon receive a better candidate
			//to propose to the mobility client.
			return EFalse;
			}

		if (status.Score() > iAvailabilityScoreTreshold
			&& candidate!=lastRejected)
			{
			if (candidate==aContext.Node().ServiceProvider()
			    && Error() == KErrNone )
				{
				//The preferred one is the current one, is still available and was not just rejected.
				//No need to do anything more.
				return EFalse;
				}

			//A new match found
			iAvailable = candidate;
			return ETrue;
			}
		}

	//There is no choice for migration
	return EFalse; //No match found
	}

void CMobilityActivity::SetHandshakingFlag()
	{
	static_cast<CMobilityMetaConnectionProvider&>(iNode).iIsHandshakingNow = ETrue;
	}

void CMobilityActivity::ClearHandshakingFlag()
	{
	static_cast<CMobilityMetaConnectionProvider&>(iNode).iIsHandshakingNow = EFalse;
	}

DEFINE_SMELEMENT(CMobilityActivity::TNoTagOrReConnectOrStartMobilityHandshakeBackwards, NetStateMachine::MStateFork, CMobilityActivity::TContext)
TInt CMobilityActivity::TNoTagOrReConnectOrStartMobilityHandshakeBackwards::TransitionTag()
	{
	if (iContext.Activity()->Error() == KErrNone &&
	    (message_cast<TCFMobilityProvider::TMigrationRequested>(&iContext.iMessage) ||
	     message_cast<TCFMcpr::TReConnectComplete>(&iContext.iMessage)))
		{
	        CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);
			if( activity.iCurrent!=activity.iAvailable )
			    return MobilityMCprStates::KReConnect | NetStateMachine::EForward;
			else
		        return MeshMachine::KNoTag | NetStateMachine::EForward;
		}
	return MobilityMCprStates::KStartMobilityHandshake | NetStateMachine::EBackward;
	}

DEFINE_SMELEMENT(CMobilityActivity::TNoTagOrStartMobilityHandshakeBackwards, NetStateMachine::MStateFork, CMobilityActivity::TContext)
TInt CMobilityActivity::TNoTagOrStartMobilityHandshakeBackwards::TransitionTag()
	{
	if (iContext.Activity()->Error() == KErrNone &&
	    (message_cast<TCFMobilityProvider::TMigrationRequested>(&iContext.iMessage) ||
	     message_cast<TCFMcpr::TReConnectComplete>(&iContext.iMessage)))
		{
		return MeshMachine::KNoTag | NetStateMachine::EForward;
		}
	return MobilityMCprStates::KStartMobilityHandshake | NetStateMachine::EBackward;
	}

DEFINE_SMELEMENT(CMobilityActivity::TNoTagOrAwaitMobilityBackwardsOnMobilityTrigger, NetStateMachine::MStateFork, CMobilityActivity::TContext)
TInt CMobilityActivity::TNoTagOrAwaitMobilityBackwardsOnMobilityTrigger::TransitionTag()
	{
	//This is where the judgement is made on whether to trigger mobility (offer the client another bearer)
	//or ignore and come back waiting.
	__ASSERT_DEBUG(iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>() ||
			iContext.iMessage.IsMessage<TCFAvailabilityControlClient::TAvailabilityNotification>(),
			User::Panic(KCoreMobileMCprPanic, KPanicIncorrectMessage));
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);

	if (activity.EvaluatePreference(iContext))
		{
        activity.SetError(KErrNone);
		return KNoTag;
		}
    else if (activity.Error() != KErrNone )
        {
        activity.PostToOriginators(TEBase::TError(activity.Error()).CRef());
        activity.SetError(KErrNone);
        }
	return MobilityMCprStates::KAwaitMobility | NetStateMachine::EBackward;
	}

DEFINE_SMELEMENT(CMobilityActivity::TNoTagOrAwaitMobility, NetStateMachine::MStateFork, CMobilityActivity::TContext)
TInt CMobilityActivity::TNoTagOrAwaitMobility::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);

	if (activity.EvaluatePreference(iContext))
		{
		activity.SetError(KErrNone);
		return KNoTag;
		}
	else if (activity.Error() != KErrNone )
	    {
	    activity.PostToOriginators(TEBase::TError(activity.Error()).CRef());
	    activity.SetError(KErrNone);
	    }
	return MobilityMCprStates::KAwaitMobility;
	}


DEFINE_SMELEMENT(CMobilityActivity::TRejectedOrStartMobilityHandshakeBackwards, NetStateMachine::MStateFork, MobilityMCprStates::TContext)
TInt CMobilityActivity::TRejectedOrStartMobilityHandshakeBackwards::TransitionTag()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);

	// if rejected last ap and there's no more
	if (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>())
		{
		TBool otherSP = EFalse;

		// Find if there anymore available non rejected service providers
		TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
		__ASSERT_DEBUG(iter[0], User::Panic(KCoreMobileMCprPanic, KPanicNoServiceProvider)); //A Service Provider must exist!
		RMetaServiceProviderInterface* rejected = static_cast<RMetaServiceProviderInterface*>(iContext.Node().ServiceProvider());
		RMetaServiceProviderInterface* candidate = NULL;

		while ((candidate = static_cast<RMetaServiceProviderInterface*>(iter++)) != NULL)
			{
			if (candidate == rejected)
				{
				continue;
				}

			const TAvailabilityStatus& status = candidate->AvailabilityStatus();
			if (!status.IsKnown())
				{
				continue;
				}

			if (status.Score() > activity.iAvailabilityScoreTreshold)
				{
				otherSP=ETrue;
				break;
				}
			}

		if (!otherSP)
			{
			return MobilityMCprStates::KRejected;
			}
		}

    return MobilityMCprStates::KStartMobilityHandshake | NetStateMachine::EBackward;
    }

DEFINE_SMELEMENT(CMobilityActivity::TSendAvailabilityRequest, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
void CMobilityActivity::TSendAvailabilityRequest::DoL()
	{
	//Issue availability notification registration to start the availability activity on this node.
	//NOTE: since we've requested availability from self, we are interested in any change (even available->available)
	//since we could be switching from AP1 available to AP2 available. Either way we must recalculate.
	//We're hence interested in TAvailabilitySubscriptionOptions::EAnyNestedChange.
	TAvailabilitySubscriptionOptions availabilityOptions(TAvailabilitySubscriptionOptions::EAnyNestedChange);
	TCFAvailabilityProvider::TAvailabilityNotificationRegistration msg(availabilityOptions);
	RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.NodeId(), msg);
	//Do not set iPostedTo. We are not waiting for the responses.
	}

DEFINE_SMELEMENT(CMobilityActivity::TInformMigrationAvailableAndSetHandshakingFlag, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
void CMobilityActivity::TInformMigrationAvailableAndSetHandshakingFlag::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);

	//Inform the CPR that a potential migration is available. We only support a single data client
	//in this implementation.
	__ASSERT_DEBUG(activity.iAvailable, User::Panic(KCoreMobileMCprPanic, KPanicNoServiceProvider));

	//Compute all this here to keep EvaluatePreference() as fast as possible
	activity.iCurrent = static_cast<RMetaServiceProviderInterface*>(iContext.Node().ServiceProvider());
	__ASSERT_DEBUG(activity.iCurrent, User::Panic(KCoreMobileMCprPanic, KPanicNoServiceProvider));

	//Perform a simple check if this is an upgrade or not
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
	RNodeInterface* sp = iter++;
	while (sp && sp!=activity.iCurrent && sp!=activity.iAvailable)
		{
		sp = iter++;
		}

	TBool isUpgrade = (sp != activity.iCurrent); //If current was found first -> this is not an upgrade
	if( activity.iCurrent == activity.iAvailable && activity.iCandidate )
	    {
		// The available client is the same as the current and a candidate exists, this indicates that
		// an error has occured when trying to start the candidate bearer and the control as reverted to
		// the current bearer. In this situation the notification needs to look as if the bearer has
		// migrated from the failed candidate to the current bearer.
		TCFMobilityControlClient::TMigrationNotification msg(activity.iCandidate->ProviderInfo().APId(),
											   activity.iAvailable->ProviderInfo().APId(),
											   isUpgrade, EFalse);
		activity.PostToOriginators(msg);
	    }
	else
	    {
		// Standard case where migration is going from current to available.
		TCFMobilityControlClient::TMigrationNotification msg(activity.iCurrent->ProviderInfo().APId(),
											   activity.iAvailable->ProviderInfo().APId(),
											   isUpgrade, EFalse);
		activity.PostToOriginators(msg);
	    }

	activity.ClearPostedTo();
	activity.SetHandshakingFlag();
	}

DEFINE_SMELEMENT(CMobilityActivity::TAwaitingCurrentCarrierRejectedOrAvailabilityChange, NetStateMachine::MState, CMobilityActivity::TContext)
TBool CMobilityActivity::TAwaitingCurrentCarrierRejectedOrAvailabilityChange::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>())
	    {
	    iContext.Activity()->SetError(KErrNotFound);
	    return ETrue;
	    }
	return  iContext.iMessage.IsMessage<TCFAvailabilityControlClient::TAvailabilityNotification>();
	}


DEFINE_SMELEMENT(CMobilityActivity::TRequestReConnect, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
void CMobilityActivity::TRequestReConnect::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);

	__ASSERT_DEBUG(activity.iAvailable, User::Panic(KCoreMobileMCprPanic, KPanicNoServiceProvider));
	__ASSERT_DEBUG(activity.iCurrent, User::Panic(KCoreMobileMCprPanic, KPanicNoServiceProvider));
	__ASSERT_DEBUG(activity.iCurrent!=activity.iAvailable, User::Panic(KSpecAssert_ESockMbCrMCPRAct, 1));

	// For the moment it is sufficient to use the re-connect activity, in the future we may want to
	// customise the behavior, for example start the new layer before rebinding it, etc.
	TCFMcpr::TReConnect msg(activity.iCurrent->RecipientId(), activity.iAvailable->RecipientId());
	activity.PostRequestTo(iContext.NodeId(), msg);
	}

DEFINE_SMELEMENT(CMobilityActivity::TInformMigrationCompleted, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
void CMobilityActivity::TInformMigrationCompleted::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	iContext.iNodeActivity->PostToOriginators(TCFMobilityProvider::TMigrationComplete().CRef());
	iContext.iNodeActivity->ClearPostedTo();
	}

DEFINE_SMELEMENT(CMobilityActivity::TClearHandshakingFlag, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
void CMobilityActivity::TClearHandshakingFlag::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);
	activity.ClearHandshakingFlag();
	}


///////////////////////////////////////////////////////////////////////////////
//CConnectionRecoveryActivity
MeshMachine::CNodeActivityBase* CConnectionRecoveryActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	return new (ELeave) CConnectionRecoveryActivity(aActivitySig, aNode);
	}

CConnectionRecoveryActivity::CConnectionRecoveryActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
:	MeshMachine::CNodeRetryActivity(aActivitySig, aNode)
	{
	}

void CConnectionRecoveryActivity::ReplyToOriginators(TEErrorRecovery::TErrorRecoveryResponse& aCFMessageSig)
	{
	NM_LOG_START_BLOCK(KESockMeshMachine, _L8("CConnectionRecoveryActivity::ReplyToOriginators"));
	NM_LOG((KESockMeshMachine, _L8("[this=0x%08x] "), this));
	NM_LOG_MESSAGE(KESockMeshMachine, aCFMessageSig);
	NM_LOG_END_BLOCK(KESockMeshMachine, _L8("CConnectionRecoveryActivity::ReplyToOriginators"));
	for (TInt n = iOriginators.Count() - 1;n>=0; n--)
		{
		Messages::TNodePeerId& peerId = iOriginators[n];
		TCFSafeMessage::TResponseCarrierWest<TEErrorRecovery::TErrorRecoveryResponse> resp(aCFMessageSig, peerId.RecipientId());
		peerId.PostMessage(iNode.Id(), resp);
		}
	}

DEFINE_SMELEMENT(CConnectionRecoveryActivity::TAwaitingReConnectComplete, NetStateMachine::MState, CConnectionRecoveryActivity::TContext)
TBool CConnectionRecoveryActivity::TAwaitingReConnectComplete::Accept()
	{
   	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
   	TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
   	if(msg)
    	{
		CConnectionRecoveryActivity& ac = static_cast<CConnectionRecoveryActivity&>(*iContext.iNodeActivity);
		TErrResponse propagateResp(TErrResponse::EPropagate,ac.iOriginalErrContext.iStateChange.iError,ac.iOriginalErrContext.iMessageId);
		TEErrorRecovery::TErrorRecoveryResponse errResp(propagateResp);
		ac.ReplyToOriginators(errResp);
    	ac.SetIdle();
   		iContext.iMessage.ClearMessageId();
   		return EFalse;
   		}
	return (iContext.iMessage.IsMessage<TCFMcpr::TReConnectComplete>())? ETrue : EFalse;
	}

void CConnectionRecoveryActivity::TTransitionBase::Error(TInt /*aError*/)
	{
	//Reply to the Error Activity and terminate
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CConnectionRecoveryActivity& ac = static_cast<CConnectionRecoveryActivity&>(*iContext.iNodeActivity);
	TEErrorRecovery::TErrorRecoveryResponse errResp(TErrResponse(TErrResponse::EPropagate,ac.iOriginalErrContext.iStateChange.iError,ac.iOriginalErrContext.iMessageId));
	ac.ReplyToOriginators(errResp);
	iContext.iNodeActivity->SetIdle();
	}

DEFINE_SMELEMENT(CConnectionRecoveryActivity::TStoreErrorContext, NetStateMachine::MStateTransition, CConnectionRecoveryActivity::TContext)
void CConnectionRecoveryActivity::TStoreErrorContext::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CConnectionRecoveryActivity& activity = static_cast<CConnectionRecoveryActivity&>(*iContext.iNodeActivity);
	activity.iOriginalErrContext = message_cast<TEErrorRecovery::TErrorRecoveryRequest>(iContext.iMessage).iErrContext;
	}

DEFINE_SMELEMENT(CConnectionRecoveryActivity::TProcessConnectionStartRecoveryRequest, NetStateMachine::MStateTransition, CConnectionRecoveryActivity::TContext)
void CConnectionRecoveryActivity::TProcessConnectionStartRecoveryRequest::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	RNodeInterface* newSP = NULL;
	RNodeInterface* curSP = iContext.Node().ServiceProvider(); //Our current started Service Provider.

	//Choose Service Providers to work on
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
	RNodeInterface* itf = NULL;
	for (itf = iter++; itf!=NULL && newSP==NULL; itf = iter++)
		{
		if (itf==curSP)
			{
			newSP = iter++; //And the new one to try next
			}
		}

	//Sanity check.
	//The new provider must not be started, there can be only one started at a time.
	__ASSERT_DEBUG(newSP==NULL || (newSP->Flags() & TCFClientType::EStarted)==0, User::Panic(KSpecAssert_ESockMbCrMCPRAct, 3));

	//If there is no other Service Provider to try, return KErrNotFound
	if (newSP==NULL || curSP == NULL)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KCoreMCprStatesTag, KCoreMCprStatesSubTag, _L8("WARNING: CConnectionRecoveryActivity::TProcessConnectionStartRecoveryRequest::DoL() - no more choices, abandoning recovery.")));
#endif
		User::Leave(KErrNotFound);
		}

	//Diagnostinc - there must be a data client or we cannot be here
	__ASSERT_DEBUG(iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData)), User::Panic(KCoreMobileMCprPanic, KPanicNoDataClient));
	iContext.iNodeActivity->PostRequestTo(iContext.NodeId(),
			TCFMcpr::TReConnect(curSP->RecipientId(), newSP->RecipientId()).CRef());
	}

DEFINE_SMELEMENT(CConnectionRecoveryActivity::TProcessConnectionGoneDownRecoveryRequest, NetStateMachine::MStateTransition, CConnectionRecoveryActivity::TContext)
void CConnectionRecoveryActivity::TProcessConnectionGoneDownRecoveryRequest::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CConnectionRecoveryActivity& activity = static_cast<CConnectionRecoveryActivity&>(*iContext.iNodeActivity);

	RNodeInterface* started = iContext.Node().ServiceProvider();
	TUint apId = (TUint)activity.iOriginalErrContext.iInfo;
	RNodeInterface* gonedownsp = iContext.Node().FindServiceProvider(apId);
	if (started && started != gonedownsp)
		{
		CConnectionRecoveryActivity::TSendRetryRecoveryResponse tr(iContext);
		tr.DoL();
		}
	else
		{
		CConnectionRecoveryActivity::TSendPropagateRecoveryResponse tr(iContext);
		tr.DoL();
		}
	}

DEFINE_SMELEMENT(CConnectionRecoveryActivity::TSendRetryRecoveryResponse, NetStateMachine::MStateTransition, CConnectionRecoveryActivity::TContext)
void CConnectionRecoveryActivity::TSendRetryRecoveryResponse::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CConnectionRecoveryActivity& activity = static_cast<CConnectionRecoveryActivity&>(*iContext.iNodeActivity);
	TEErrorRecovery::TErrorRecoveryResponse err(TErrResponse(TErrResponse::ERetry,KErrNone,activity.iOriginalErrContext.iMessageId));
	activity.ReplyToOriginators(err);
	}

DEFINE_SMELEMENT(CConnectionRecoveryActivity::TSendPropagateRecoveryResponse, NetStateMachine::MStateTransition, CConnectionRecoveryActivity::TContext)
void CConnectionRecoveryActivity::TSendPropagateRecoveryResponse::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CConnectionRecoveryActivity& activity = static_cast<CConnectionRecoveryActivity&>(*iContext.iNodeActivity);
	TEErrorRecovery::TErrorRecoveryResponse err(TErrResponse(TErrResponse::EPropagate,
		activity.iOriginalErrContext.iStateChange.iError,activity.iOriginalErrContext.iMessageId));
	activity.ReplyToOriginators(err);
	}

DEFINE_SMELEMENT(CConnectionRecoveryActivity::TSendIgnoreRecoveryResponse, NetStateMachine::MStateTransition, CConnectionRecoveryActivity::TContext)
void CConnectionRecoveryActivity::TSendIgnoreRecoveryResponse::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	CConnectionRecoveryActivity& activity = static_cast<CConnectionRecoveryActivity&>(*iContext.iNodeActivity);
	TEErrorRecovery::TErrorRecoveryResponse err(TErrResponse(TErrResponse::EIgnore,KErrNone,activity.iOriginalErrContext.iMessageId));
	activity.ReplyToOriginators(err);
	}
