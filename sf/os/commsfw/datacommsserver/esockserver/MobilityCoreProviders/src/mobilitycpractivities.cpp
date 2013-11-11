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
// mobilityactivities.cpp
// Mobility Connection Provider activity definitions.
//
//
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/mobilitycpr.h>
#include "mobilitycpractivities.h"
#include <comms-infras/mobilitycprstates.h>
#include <comms-infras/ss_coreprstates.h>

#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/corecpractivities.h>
#include <elements/nm_messages_errorrecovery.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockMbCrCPRAct, "ESockMbCrCPRAct");
#endif


using namespace ESock;
using namespace Messages;
using namespace MeshMachine;
using namespace NetStateMachine;
using namespace MobilityCprActivities;
using namespace MobilityCprStates;
using namespace CorePanics;

//
//Panics
#ifdef _DEBUG
_LIT (KCoreMobileCprPanic,"CoreMobileCprPanic");
#endif

namespace CprMobilityActivity
{
/**
	Activity responsible for interacting with the ESock client mobility extension
	API and with the MCPR for the acceptance or rejection of requests to migrate
	bearer.
*/
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityCprMobility, MobilityCprMobility, TCFMobilityProvider::TStartMobility, CMobilityActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(MobilityCprStates::TAwaitingStartMobility, MeshMachine::TNoTag)

	// Attempt to start mobility activity on the meta plane
	NODEACTIVITY_ENTRY(KNoTag, MobilityCprStates::TSendStartMobility, MobilityCprStates::TAwaitingMobilityStartedOrError, MeshMachine::TNoTagOrErrorTag)

	// Success - complete the IPC client
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CMobilityActivity::TCompleteMobilityClient, TTag<CMobilityActivity::KWaitForMobility>)

	//<BEGIN> MAIN LOOP ****************
	//Awaiting mobility triggers, which can either be:
	//(1) TMigrationNotification (PreferredCarrierAvailable) coming from the control provider (presumably as a response to changing availability)
	//(2) TMigrationRejected (NewCarrierRejected) coming from the control client (who's presumably not content with the currently offered carrier).
	//(3) TError coming from the control provider in response to the previous rejection. The client rejected the current bearer, but there are no more bearers to offer.
	NODEACTIVITY_ENTRY(CMobilityActivity::KWaitForMobility, MeshMachine::TDoNothing, MobilityCprStates::TAwaitingNewCarrierOrErrorOrRelayAndConsumeCurrentCarrierRejected, CMobilityActivity::TStartHandshakeOrErrorTagBlockedByClientNotReady)

		// Notify the ESock client that a preferred carrier is available and await the client to respond
		// The response may be:
		// (1) TCFMobilityProvider::TMigrationRequested - proceed with the migration - the tuple will jump (with KNoTag) to await for the control provider to supply the new bearer.
 		// (2) TCFMobilityProvider::TMigrationRejected - abort the migration - the tuple will jump (with KWaitForMobility | EBackaward) back to awaiting for the preferred bearer.
 		// Note that TAwaitAndRelayMigrationRequestedOrMigrationRejected does the job of forwarding both messages to the control provider.
		NODEACTIVITY_ENTRY(CMobilityActivity::KStartHandshake, CMobilityActivity::TNotifyClientPreferredCarrierAvailable, MobilityCprStates::TAwaitAndRelayMigrationRequestedOrMigrationRejected, CMobilityActivity::TNoTagOrWaitForMobilityBackwards)

		//We've just told the control provider (see above: TAwaitAndRelayMigrationRequested...) that the offered carrier has been accepted - the client is requesting migration.
		//The control provider may respond threefold:
		//(1) TMigrationComplete - Proceed with the migration - the tuple will jump (with KNoTag) to starting of the bearer;
		//(2) TMigrationNotification - offer another bearer (if the current one became out of date) - the tuple will jump back to the processing of the new bearer
		//(3) TError - abort mobility if the current bearer is out of date and there are no more bearers to offer - the tuple will jump to the KErrorTag tuple who will decide what kind of error it is.
		NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MobilityCprStates::TAwaitingMigrationCompleteOrNewCarrierOrError, MobilityCprStates::TNoTagOrStartHandshakeBackwardsOrError)
		THROUGH_NODEACTIVITY_ENTRY(KErrorTag, CMobilityActivity::TCompleteMobilityClient, MeshMachine::TTag<CMobilityActivity::KWaitForMobility|EBackward>)

		// Start the new connection
		NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStartServiceProvider, MobilityCprStates::TAwaitingStartedOrError, CMobilityActivity::TNoTagOrErrorTagBlockedByClientNotReady)
		NODEACTIVITY_ENTRY(KErrorTag, TSendErrorRecoveryReq, MeshMachine::TAwaitingErrorRecoveryResponseOrError, CMobilityActivity::TNoTagBackwardsOrRecoverableErrorOrErrorBlockedByClientNotReady)

        //If the bearer fails to start, the can be another bearer available, so we need to notify the control provider in case it can offer one.
		//This node does that by rejecting the current bearer.
		THROUGH_NODEACTIVITY_ENTRY(CoreStates::KRecoverableErrorTag, MobilityCprStates::TSendMigrationRejected, TTag<CMobilityActivity::KWaitForMobility|EBackward>)
		THROUGH_NODEACTIVITY_ENTRY(KErrorTag, CMobilityActivity::TCompleteMobilityClient, MeshMachine::TTag<CMobilityActivity::KWaitForMobility|EBackward>)

		NODEACTIVITY_ENTRY(KNoTag, CMobilityActivity::TNotifyClientNewCarrierActive, MobilityCprStates::TAwaitAndRelayMigrationAcceptedOrMigrationRejected, MeshMachine::TTag<CMobilityActivity::KWaitForMobility|EBackward>)
		//<END> MAIN LOOP **************

NODEACTIVITY_END()
} // namespace CprMobilityActivity

namespace MobilityCprActivities
{
DEFINE_EXPORT_ACTIVITY_MAP(mobilityCprActivities)
	ACTIVITY_MAP_ENTRY(CprMobilityActivity, MobilityCprMobility)
ACTIVITY_MAP_END_BASE(CprActivities, coreCprActivities)
}

//
// CMobilityActivity
CMobilityActivity::CMobilityActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
:	MeshMachine::CNodeRetryActivity(aActivitySig, aNode)
	{
	__ASSERT_DEBUG(static_cast<CMobilityConnectionProvider&>(iNode).iMobilityActivity == NULL, User::Panic(KCoreMobileCprPanic, KPanicActivity));
	}

CMobilityActivity::~CMobilityActivity()
	{
	//This pointer becomes invalid now, clear if still set.
	static_cast<CMobilityConnectionProvider&>(iNode).iMobilityActivity = NULL;
	CCommsApiExtResponder::Complete(iResponder, Error() ? Error() : KErrAbort); //Safe if NULL
	SetError(KErrNone);
	}

MeshMachine::CNodeActivityBase* CMobilityActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	CMobilityActivity* self = new(ELeave) CMobilityActivity(aActivitySig, aNode);
	return self;
	}

void CMobilityActivity::SetResponder(ESock::CCommsApiExtResponder& aResponder)
	{
	__ASSERT_DEBUG(iResponder==NULL, User::Panic(KSpecAssert_ESockMbCrCPRAct, 7));
	__ASSERT_DEBUG(&aResponder, User::Panic(KSpecAssert_ESockMbCrCPRAct, 8));
	iResponder = &aResponder;

	//CMobilityActivity::TMobilityClientNotReadyMutex may have CMobilityActivity await for
	//iResponder to pop up and if it is waiting, we need to signal it.
	TNodeNullContext context(iNode, this);
	Signal(context);
	}

TBool CMobilityActivity::TMobilityClientNotReadyMutex::IsBlocked(MeshMachine::TNodeContextBase& aContext)
	{
	__ASSERT_DEBUG(aContext.iNodeActivity, User::Panic(KCoreMobileCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*aContext.iNodeActivity);
	return activity.iResponder? EFalse : ETrue;
	}

//Cpr::CMobilityActivity has no originators
void CMobilityActivity::StartL(TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const TStateTriple& aFirst)
	{
	//This activity does not support multiple clients. Any subsequent client should be completed with KErrNotSupported
	//from CMobilityConnectionProvider::OpenExtensionInterface().
	__ASSERT_DEBUG(iOriginators.Count()==0, User::Panic(KSpecAssert_ESockMbCrCPRAct, 1));

	MeshMachine::CNodeRetryActivity::StartL(aContext, aOriginator, aFirst);

	TCFMobilityProvider::TStartMobility& msg = message_cast<TCFMobilityProvider::TStartMobility>(aContext.iMessage);
	iResponder = static_cast<CCommsApiExtResponder*>(msg.iPtr);
	iClientId = msg.iValue;
	__ASSERT_DEBUG(msg.iValue != 0, User::Panic(KSpecAssert_ESockMbCrCPRAct, 2)); //Client id must be valid here.

	//Set a pointer to the mobility activity in the node on which it's running
	__ASSERT_DEBUG(static_cast<CMobilityConnectionProvider&>(iNode).iMobilityActivity == NULL, User::Panic(KSpecAssert_ESockMbCrCPRAct, 3));
	static_cast<CMobilityConnectionProvider&>(iNode).iMobilityActivity = this;
	}

void CMobilityActivity::Cancel(TNodeContextBase& aContext)
	{
	RNodeInterface* cp = iNode.GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrlProvider));
	__ASSERT_DEBUG(cp, User::Panic(KSpecAssert_ESockMbCrCPRAct, 4)); //We are a Cpr, must exist.

	//PostedTo() could be our service provider or possibly other peer
	if (PostedToPeer() != cp)
		{
		cp->PostMessage(TNodeCtxId(ActivityId(), iNode.Id()),
			TEBase::TCancel().CRef());
		}

	MeshMachine::CNodeRetryActivity::Cancel(aContext); //Send TCancel to iPostedTo
	}


DEFINE_SMELEMENT(CMobilityActivity::TNoTagBackwardsOrRecoverableErrorOrError, NetStateMachine::MStateFork, CMobilityActivity::TContext)
TInt CMobilityActivity::TNoTagBackwardsOrRecoverableErrorOrError::TransitionTag()
	{
	if (iContext.iMessage.IsMessage<TEErrorRecovery::TErrorRecoveryResponse>())
		{
		TErrResponse& resp = message_cast<TEErrorRecovery::TErrorRecoveryResponse>(iContext.iMessage).iErrResponse;
		if (resp.iAction == TErrResponse::ERetry)
			{
			iContext.Activity()->SetError(KErrNone);
      		return KNoTag | NetStateMachine::EBackward;
			}
		else if  (resp.iAction == TErrResponse::EPropagate || resp.iError == KErrCancel)
			{
            if (resp.iError != KErrNone)
                {
                //The activity is already errored. The errored state needs to be reset first.
                iContext.Activity()->SetError(KErrNone);
                iContext.Activity()->SetError(resp.iError);
                }
            
			return KErrorTag;
			}
		else if (resp.iAction == TErrResponse::EIgnore) // and or iError is KErrCancel
			{
			//TODO RZ: This looks inappropriate. EIgnore and EPropagate flags are not 
			//really complimentary. They're simply invalid in soem scenarios.
			//Here we use ignore to an activity that attempted error recovery,
			//it technically cannot ignore the error. It will propagate it back
			//to the MCPR (and not to the client) by rejecting the current bearer.
			//It feels thought clearing the error doesn't belong here. It probably
			//belongs to the error activity. 
			iContext.Activity()->SetError(KErrNone);
			return CoreStates::KRecoverableErrorTag;
			}
		}
	return KNoTag;
	}


DEFINE_SMELEMENT(CMobilityActivity::TNoTagOrWaitForMobilityBackwards, NetStateMachine::MStateFork, CMobilityActivity::TContext)
TInt CMobilityActivity::TNoTagOrWaitForMobilityBackwards::TransitionTag()
	{
	if(iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRequested>())
		{
		return KNoTag;
		}
	else
		{
		__ASSERT_DEBUG(iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>(), User::Panic(KSpecAssert_ESockMbCrCPRAct, 6));
		return CMobilityActivity::KWaitForMobility|EBackward;
		}
	}


//Transitions

DEFINE_SMELEMENT(CMobilityActivity::TCompleteMobilityClient, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
void CMobilityActivity::TCompleteMobilityClient::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);
	CCommsApiExtResponder::Complete(activity.iResponder, activity.Error());
	
	//There is only one way to kill the mobility activity and it is by the client closing the API extension.
	//This will be manifested with KErrCancel.
	if (activity.Error() == KErrCancel)
	    {
	    activity.SetIdle();
	    }
	activity.SetError(KErrNone); //The error has been handled
	activity.ClearPostedTo();
	}

DEFINE_SMELEMENT(CMobilityActivity::TNotifyClientPreferredCarrierAvailable, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
void CMobilityActivity::TNotifyClientPreferredCarrierAvailable::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);
	TCFMobilityControlClient::TMigrationNotification& msg = message_cast<TCFMobilityControlClient::TMigrationNotification>(iContext.iMessage);

	activity.iCurrentAp = msg.iValue1;
	activity.iPreferredAp = msg.iValue2;
	activity.iIsUpgrade = msg.iValue3;

	CCommsMobilitySrvResp* responder = static_cast<CCommsMobilitySrvResp*>(activity.iResponder);
	activity.iResponder = NULL;
	CCommsMobilitySrvResp::PreferredCarrierAvailable(responder, TAccessPointInfo(activity.iCurrentAp), TAccessPointInfo(activity.iPreferredAp), activity.iIsUpgrade, EFalse);
	activity.ClearPostedTo();
	}

DEFINE_SMELEMENT(CMobilityActivity::TNotifyClientNewCarrierActive, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
void CMobilityActivity::TNotifyClientNewCarrierActive::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileCprPanic, KPanicNoActivity));
	CMobilityActivity& activity = static_cast<CMobilityActivity&>(*iContext.iNodeActivity);
	CCommsMobilitySrvResp* responder = static_cast<CCommsMobilitySrvResp*>(activity.iResponder);
	activity.iResponder = NULL;
	CCommsMobilitySrvResp::NewCarrierActive(responder, TAccessPointInfo(activity.iPreferredAp), EFalse);
	activity.ClearPostedTo();
	}


