// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// COREPRSTATES.CPP
// Core PR States
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalComponent
*/


#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_tiermanager_internal.h>
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_metaconnprov_internal.h>

#include <comms-infras/ss_nodeinterfaces.h>

#include "ss_coreprstates.h"
#include <comms-infras/ss_corepractivities.h>
#include "ss_apiext_messages.h"
#include <cs_subconparams.h>

#include <comms-infras/ss_protocolparameterset.h>

#include <elements/nm_messages_errorrecovery.h>
#include <elements/nm_messages_peer.h>
#include <elements/nm_messages_child.h>

#include "ss_flowrequest.h"

#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <comms-infras/ss_nodemessages_subconn.h>
#include <comms-infras/ss_nodemessages_rejoiningprovider.h>
#include <comms-infras/ss_nodemessages_internal_esock.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_mcpr.h>
#include <comms-infras/ss_nodemessages_cpr.h>

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrStaCPRSC, "ESockCrStaCPRSC");
#endif

#if defined __CFLOG_ACTIVE || defined LOG || defined ESOCK_EXTLOG_ACTIVE
	#define KCoreProviderStatesTag KESockCoreProviderTag
	_LIT8(KCoreProviderStatesSubTag, "coreprovstate");
#endif

using namespace CoreStates;
using namespace CoreNetStates;
using namespace PRStates;
using namespace PRActivities;
using namespace CoreActivities;
using namespace NetStateMachine;
using namespace ESock;
using namespace CorePanics;
using namespace Elements;
using namespace Messages;
using namespace MeshMachine;
using namespace Factories;

const TUint32 KOrphanExcludeFlags = TCFClientType::EActive|TCFClientType::EActivating|TCFClientType::ELeaving|TCFClientType::EStarted|TCFClientType::EStarting;

//-=========================================================
//
// Panics
//
//-=========================================================
_LIT (KCorePrPanic,"CorePrPanic");

void CorePrPanic(TInt aCode)
	{
	User::Panic(KCorePrPanic, aCode);
	}


//-=========================================================
//
//Utility functions
//
//-=========================================================
//[RZ TODO] This method is a real pain. This method must go.
//This method method must be replaced with examining activity->Error()
//or similar.
TInt ExtractErrorCode(Messages::TSignatureBase& aMessage, MeshMachine::CNodeActivityBase* aActivity = NULL)
    {
    if (aActivity && aActivity->Error() != KErrNone)
        {
        TInt error = aActivity->Error();
        aActivity->SetError(KErrNone);
        return error;
        }

    TInt code = KErrCancel;

	if  ( (aMessage.IsMessage<TCFServiceProvider::TStopped>()) ||
		(aMessage.IsMessage<TCFServiceProvider::TStop>()) ||
		(aMessage.IsMessage<TCFDataClient::TStopped>()) ||
		(aMessage.IsMessage<TCFDataClient::TStop>()) )
		{
		code = static_cast<const Messages::TSigNumber&>(aMessage).iValue;
		}
	else if ( (aMessage.IsMessage<TCFControlClient::TGoneDown>()) ||
		(aMessage.IsMessage<TCFControlProvider::TDataClientGoneDown>()) )
		{
		code = static_cast<const Messages::TSigNumberNumber&>(aMessage).iValue1;
		}
	else if ( aMessage.IsMessage<TEErrorRecovery::TErrorRecoveryResponse>() )
		{
		//Action must be propagate or there is no error code (your activity flow is faulty)!
		const Messages::TSigErrResponse& sig = static_cast<const Messages::TSigErrResponse&>(aMessage);
		__ASSERT_DEBUG(sig.iErrResponse.iAction==Messages::TErrResponse::EPropagate, User::Panic(KSpecAssert_ESockCrStaCPRSC, 1));
   		code = sig.iErrResponse.iError;
		}
	return code;
    }


//-=========================================================
//
//Common States and Transitions
//
//-=========================================================

EXPORT_C TBool ASetErrorState::Accept(TNodeContextBase& aContext, TBool aSuperAccept)
	{
	TEBase::TError* msg = message_cast<TEBase::TError>(&aContext.iMessage);
	if (msg && aSuperAccept == EFalse && aContext.iNodeActivity != NULL)
    	{
		aContext.iNodeActivity->SetError(msg->iValue);
    	msg->ClearMessageId();
		return EFalse;
		}
	return aSuperAccept;
	}

EXPORT_C TInt ACancelOrErrorOrTag::TransitionTag(TNodeContextBase& aContext, TInt aTag)
   	{
	if(aContext.iMessage.IsMessage<TEBase::TCancel>())
		{
		return MeshMachine::KCancelTag;
		}
	if(aContext.iMessage.IsMessage<TEBase::TError>())
		{
		return MeshMachine::KErrorTag;
		}
   	return aTag;
   	}

EXPORT_DEFINE_SMELEMENT(TForwardToControlProvider, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TForwardToControlProvider::DoL()
	{
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(iContext.NodeId(),
		iContext.iMessage, TClientType(TCFClientType::ECtrlProvider));
	}

EXPORT_DEFINE_SMELEMENT(TForwardToControlClients, NetStateMachine::MStateTransition, PRStates::TContext)
void TForwardToControlClients::DoL()
	{
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(iContext.NodeId(), iContext.iMessage, TClientType(TCFClientType::ECtrl));
	}

EXPORT_DEFINE_SMELEMENT(TPanic, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TPanic::DoL()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCrStaCPRSC, 2));
	}

EXPORT_DEFINE_SMELEMENT(TPostToOriginators, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TPostToOriginators::DoL()
	{
	iContext.iNodeActivity->PostToOriginators(iContext.iMessage);
	}

EXPORT_DEFINE_SMELEMENT(TAddAuxClient, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TAddAuxClient::DoL()
	{
//TODO - what is this hack about? Please raise a defect.
	iContext.Node().AddClientL(TNodeId::NullId(), TClientType(TCFClientType::EAux));
	}

EXPORT_DEFINE_SMELEMENT(TRemoveAuxClient, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TRemoveAuxClient::DoL()
	{
    TNodeSignal::TMessageId noPeerIds[] = {
        TNodeSignal::TMessageId()
        };
	TEChild::TLeft leaveMsg;

//DEF117712: Reentering mesh engine from TRemoveAuxClient::DoL has been raised on this.
//Must be removed or explained (as an exception - why safe here?), as re-entering the MM engine is forbidden.
	CoreNetStates::TContext ctx(iContext.Node(), leaveMsg, iContext.iSender, iContext.iRecipient);
	iContext.Node().Received(noPeerIds, ctx);
	}

//-=========================================================
//
//Control Client Join
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TAddControlClientAndSendJoinCompleteIfRequest, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TAddControlClientAndSendJoinCompleteIfRequest::DoL()
	{
	//Make sure the client is in the client's table (add if not already there).
   	if (iContext.iMessage.IsMessage<TCFPeer::TJoinRequest>())
		{
		TCFPeer::TJoinRequest& msg = message_cast<TCFPeer::TJoinRequest>(iContext.iMessage);

        // Client type could be Messages::TClientType::EWorker (Selection Request)
        // or ESock::TCFClientType::ECtrl, possibly others but not ESock::TCFClientType::EData
        // which is handled by another activity
        __ASSERT_DEBUG(msg.iClientType.Type() != (TUint32)TCFClientType::EData, User::Panic(KSpecAssert_ESockCrStaCPRSC, 3));

		iContext.Node().AddClientL(msg.iNodeId, TClientType(TCFClientType::ECtrl,msg.iClientType.Flags()));

		//Send confirmation
		RClientInterface::OpenPostMessageClose(iContext.NodeId(), iContext.iSender, TCFPeer::TJoinComplete().CRef());
		}
	else
		{
	    TCFFactory::TPeerFoundOrCreated& msg = message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage);
		iContext.Node().AddClientL(address_cast<TNodeId>(iContext.iSender), TClientType(TCFClientType::ECtrl, msg.iValue));
		}

    //[399TODO] RZ: this call is safe as it checks for the existence of the ControlProvider
    //same as TSendDataClientIdle. Though it looks a bit not elegant, cos' either this has
    //to be a separate transition (why check ControlProvider then?) or it can always be
    //aggregated with join/leave - why separate export then?
   	TSendDataClientActive dcActive(iContext);
   	dcActive.DoL();
 	}


EXPORT_DEFINE_SMELEMENT(TAwaitingApplyResponse, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TAwaitingApplyResponse::Accept()
	{
	return (iContext.iMessage.IsMessage<TCFScpr::TApplyResponse>());
	}

EXPORT_DEFINE_SMELEMENT(TSendApplyRequest, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSendApplyRequest::DoL()
	{
   	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRSC, 4));
	if (iContext.Node().ServiceProvider() == NULL)
    	{
    	User::Leave(KErrNotReady);
    	}
    iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(),
    	TCFScpr::TApplyRequest().CRef());
	}

//-=========================================================
//
//Data Client Join
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TAddDataClient, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TAddDataClient::DoL()
	{
	Messages::TNodeId* client = NULL;
	TCFClientType::TFlags clientFlags = (TCFClientType::TFlags)0;
	TCFFactory::TPeerFoundOrCreated* dataClientJoinedMsg = message_cast<TCFFactory::TPeerFoundOrCreated>(&iContext.iMessage);

	if ( dataClientJoinedMsg )
		{
		// Message "TJoined" => data client already joined (normally when "freshly" created)
		client = &dataClientJoinedMsg->iNodeId;
		clientFlags = (TCFClientType::TFlags)(dataClientJoinedMsg->iValue);
		}
	else
		{
		// Message "TJoinRequest" => data client requesting to join (when moved from one owner to another).
		TCFPeer::TJoinRequest& dataClientJoinReqMsg =
			message_cast<TCFPeer::TJoinRequest>(iContext.iMessage);
		client = &dataClientJoinReqMsg.iNodeId;
		clientFlags = static_cast<TCFClientType::TFlags>(dataClientJoinReqMsg.iClientType.Flags());
		//[EC120TODO]: RZ When ib earth does the sender of TJoinRequest specifies a different type than
		//             EData. API classification will clear this up though scrapping explicit Contrl/Data join msgs.
		//             So each join message will indeed carry a type.
		__ASSERT_DEBUG(dataClientJoinReqMsg.iClientType.Type() == TCFClientType::EData, User::Panic(KSpecAssert_ESockCrStaCPRSC, 5));
		}

	__ASSERT_DEBUG(client, User::Panic(KSpecAssert_ESockCrStaCPRSC, 6));
	__ASSERT_DEBUG(!client->IsNull(), User::Panic(KSpecAssert_ESockCrStaCPRSC, 7)); //Don't run this transition when you have received no data client from the factory!
	iContext.iPeer = iContext.Node().AddClientL(*client, TClientType(TCFClientType::EData, clientFlags));
	}

EXPORT_C void TAddDataClient::Error(TInt aError)
    {
	TCFFactory::TPeerFoundOrCreated* joinMsg = message_cast<TCFFactory::TPeerFoundOrCreated>(&iContext.iMessage);
    if (joinMsg)
        {
        RClientInterface::OpenPostMessageClose(iContext.NodeId(), joinMsg->iNodeId,
        	TEChild::TDestroy().CRef());

        if (iContext.iPeer)
            {
            iContext.Node().RemoveClient(iContext.iPeer->RecipientId());
            }
        }
    TStateTransition<TContext>::Error(aError);
    }

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientJoinRequest, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool CoreNetStates::TAwaitingDataClientJoinRequest::Accept()
	{
	TCFPeer::TJoinRequest* msg = message_cast<TCFPeer::TJoinRequest>(&iContext.iMessage);
	if (msg != NULL)
		{
		if (msg->iClientType.Type() & TCFClientType::EData)
			return ETrue;
		}
	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAddDataClientAndRespond, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TAddDataClientAndRespond::DoL()
	{
    TAddDataClient::DoL();
    RClientInterface::OpenPostMessageClose(iContext.NodeId(), iContext.iSender,
    	TCFPeer::TJoinComplete().CRef());
	}

//-=========================================================
//
//Data Client Rejoin
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientRejoin, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingDataClientRejoin::Accept()
	{
	return iContext.iMessage.IsMessage<TCFRejoiningProvider::TRejoinDataClientRequest>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientRejoinOrApplyOrCancel, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingDataClientRejoinOrApplyOrCancel::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFRejoiningProvider::TRejoinDataClientRequest>() ||
	    iContext.iMessage.IsMessage<TCFScpr::TApplyRequest>())
    	{
    	return ETrue;
    	}
	else if (iContext.iMessage.IsMessage<TEBase::TCancel>())
    	{
    	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRSC, 8));
        iContext.iNodeActivity->SetIdle();
    	}
    return EFalse;
	}


EXPORT_DEFINE_SMELEMENT(TAwaitingRejoinDataClientComplete, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingRejoinDataClientComplete::Accept()
	{
	return iContext.iMessage.IsMessage<TCFRejoiningProvider::TRejoinComplete>();
	}

EXPORT_DEFINE_SMELEMENT(TSendRejoinComplete, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendRejoinComplete::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRSC, 9));
    iContext.iNodeActivity->PostToOriginators(TCFRejoiningProvider::TRejoinComplete().CRef());
    }


//-=========================================================
//
//Client Leave && Client Leave Request
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TAwaitingClientLeave, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TAwaitingClientLeave::Accept()
	{
	return iContext.iMessage.IsMessage<TEPeer::TLeaveRequest>() || iContext.iMessage.IsMessage<TEChild::TLeft>();
	}

EXPORT_DEFINE_SMELEMENT(TDestroyOrphanedDataClients, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TDestroyOrphanedDataClients::DoL()
	{
	iContext.Node().DestroyOrphanedDataClients();
	}

EXPORT_DEFINE_SMELEMENT(TSendDataClientIdle, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendDataClientIdle::DoL()
	{
    RNodeInterface* cp = iContext.Node().ControlProvider();
	if (cp)
		{ //If there is no Control Provider we probably are an MCPR/Tier Manager/etc
    	cp->PostMessage(iContext.NodeId(), TCFControlProvider::TIdle().CRef());
    	}
	}

EXPORT_DEFINE_SMELEMENT(TSendDataClientActive, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendDataClientActive::DoL()
	{
    RNodeInterface* cp = iContext.Node().ControlProvider();
	if (cp)
		{ //If there is no Control Provider we probably are an MCPR/Tier Manager/etc
    	cp->PostMessage(iContext.NodeId(), TCFControlProvider::TActive().CRef());
    	}
	}


EXPORT_DEFINE_SMELEMENT(TSendDataClientIdleIfNoClients, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendDataClientIdleIfNoClients::DoL()
	{
	//A node is idle if it has no EData, ECtrl nor EAux clients.
    //If a node is idle it should send TDataClientIdle to its Control Provider,
    //which (in a typical scenario) decides to send TDestroy as a response.
    //We should process TDestroy by sending TClientLeaving back and destroying self.
    //Should we be the last client of our Control Provider, the Control Provider
    //sends TDataClientIdle to its Control Provider triggering a similar cycle.
	if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl|TCFClientType::EAux))==NULL
		&& iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EActive)) == NULL)
		{
		TSendDataClientIdle::DoL();
		return;
        }

    //In the somewhat similar scenario, if the node loses its last control client
    //and discovers it still has dataclients that are not bound to and are not
    //started, it will assume the dataclients are idle (unbound and not started
    //dataclients will never admit to being idle themselves).
    RNodeInterface* cc = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl));
    if (cc==NULL)
        {
        TClientIter<TDefaultClientMatchPolicy> dciter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData),
        	TClientType(0, TCFClientType::EActive|TCFClientType::EActivating|TCFClientType::EStarted|TCFClientType::EStarting|TCFClientType::ELeaving));

        TInt i = 0;
        RNodeInterface* dc;
        while ((dc = dciter[i++]) != NULL)
            {
			RClientInterface::OpenPostMessageClose(dc->RecipientId(), iContext.iNode.Id(),
				TCFControlProvider::TIdle().CRef());
            }
        }
	}

EXPORT_DEFINE_SMELEMENT(TSendLeaveCompleteIfRequest, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendLeaveCompleteIfRequest::DoL()
	{
	if (iContext.iMessage.IsMessage<TEPeer::TLeaveRequest>())
   		{
		iContext.PostToSender(TEPeer::TLeaveComplete().CRef());
		}
	}


//-=========================================================
//
//Data Client Idle
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientIdle, MState, PRStates::TContext)
EXPORT_C TBool TAwaitingDataClientIdle::Accept()
	{
	if (! iContext.iMessage.IsMessage<TCFControlProvider::TIdle>())
    	{
    	return EFalse;
    	}
    __ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrStaCPRSC, 11)); //TDataClientIdle is a peer message
    iContext.iPeer->ClearFlags(TCFClientType::EActive);
    return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientActive, MState, PRStates::TContext)
EXPORT_C TBool TAwaitingDataClientActive::Accept()
	{
	if (! iContext.iMessage.IsMessage<TCFControlProvider::TActive>())
    	{
    	return EFalse;
    	}
    __ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrStaCPRSC, 12)); //TDataClientActive is a peer message
    iContext.iPeer->SetFlags(TCFClientType::EActive);
    return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(THandleDataClientIdle, MStateTransition, PRStates::TContext)
EXPORT_C void THandleDataClientIdle::DoL()
	{
	// Send Destroy to the node which has sent data client idle, but only if the node
	// is not in use, i.e. not started/starting/activating and it has not already
	// been sent destroy.

	//This will result in a data client leaving which in turn will trigger sending
	//of TDataClientIdle to the Control Provider if there is no more clients joined to this node.

	//Activity id does not matter for TDestroy message since there is no response to it.
	//The only action that may result from sending destroy is reception of a "no peer"
	//data clent leaving message which is always going to be delivered to the client leaving
	//activity.
	//Please do not ASSERT(iContext.iNodeActivity) here!

	//This transition may only be triggered by a peer message from a data client
	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrStaCPRSC, 13));
	__ASSERT_DEBUG(iContext.iPeer->Type()==TCFClientType::EData, User::Panic(KSpecAssert_ESockCrStaCPRSC, 14));

	if (!(iContext.iPeer->Flags() &
			(TCFClientType::EActivating|TCFClientType::EStarting|TCFClientType::ELeaving|TCFClientType::EStarted)))
    	{
		// if dataclient is default and there is a non default present, don't kill the default.
      	if (!( iContext.iPeer->Flags() & TCFClientType::EDefault &&
      		   iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData),
      		                                                           TClientType(0, TCFClientType::EDefault)) > 0))
      		{
			// Send from null activity so no cancel message can ever get at it.
			iContext.iPeer->PostMessage(Messages::TNodeCtxId(MeshMachine::KActivityNull, iContext.NodeId()), TEChild::TDestroy().CRef());
	    	iContext.iPeer->SetFlags(TClientType::ELeaving);
	    	}
	    else
	    	{
	    	__CFLOG_VAR((KCoreProviderStatesTag, KCoreProviderStatesSubTag, _L8("TSendDestroyToSendingDataClient::DoL - default client not destroyed, there is an active non default one")));
	    	}
    	}
    else
    	{
		__CFLOG_VAR((KCoreProviderStatesTag, KCoreProviderStatesSubTag, _L8("TSendDestroyToSendingDataClient::DoL - client not destroyed, because it is started or has been requested again [flags %x]"), iContext.iPeer->Flags()));
		TSendDataClientIdleIfNoClients(iContext).DoL();
    	}
	}

EXPORT_DEFINE_SMELEMENT(TSendGoneDown, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendGoneDown::DoL()
	{
	if (   !(iContext.iMessage.IsMessage<TCFServiceProvider::TStopped>())
		&& !(iContext.iMessage.IsMessage<TCFServiceProvider::TStop>())
		&& !(iContext.iMessage.IsMessage<TCFDataClient::TStopped>())
		&& !(iContext.iMessage.IsMessage<TCFDataClient::TStop>())
		&& !(iContext.iMessage.IsMessage<TCFControlClient::TGoneDown>())
		&& !(iContext.iMessage.IsMessage<TCFControlProvider::TDataClientGoneDown>())
		&& !(iContext.iMessage.IsMessage<TEErrorRecovery::TErrorRecoveryResponse>()) )
		{
        CorePrPanic(KPanicIncorrectMessage);
		}

	iStopCode = ExtractErrorCode(iContext.iMessage);
	const TProviderInfo& providerInfo = static_cast<const TProviderInfoExt&>(iContext.Node().AccessPointConfig().FindExtensionL(
	        STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId))).iProviderInfo;
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(iContext.NodeId(), TCFControlClient::TGoneDown(iStopCode, providerInfo.APId()).CRef(), TClientType(TCFClientType::ECtrl));

	// Tell control provider that we have stopped.. for Tier Notification
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(iContext.NodeId(), TCFControlProvider::TDataClientStatusChange(TCFControlProvider::TDataClientStatusChange::EStopped).CRef(), TClientType(TCFClientType::ECtrlProvider));
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(iContext.NodeId(), TCFControlProvider::TDataClientGoneDown(iStopCode).CRef(), TClientType(TCFClientType::ECtrlProvider));
	}


//-=========================================================
//
//Stop
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TSendDataClientStopped, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendDataClientStopped::DoL()
	{
	// Extract stop code according to the message type that initiated the sending of TCFDataClient::TStopped.
	// Default to KErrCancel.
	iStopCode = ExtractErrorCode(iContext.iMessage, iContext.iNodeActivity);

   	TCFDataClient::TStopped dataClientStoppedMessage(iStopCode);
	if (iContext.iNodeActivity)
    	{
    	iContext.iNodeActivity->PostToOriginators(dataClientStoppedMessage);
    	}
	else
    	{
    	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrStaCPRSC, 15));
    	iContext.iPeer->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), dataClientStoppedMessage);
    	}

    // Tell control provider that we have stopped.. for Tier Notification
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
		TCFControlProvider::TDataClientStatusChange(TCFControlProvider::TDataClientStatusChange::EStopped).CRef(), TClientType(TCFClientType::ECtrlProvider));

	TInt numStartActivities = iContext.Node().CountActivities(ECFActivityStart);
	TInt numStopActivities = iContext.Node().CountActivities(ECFActivityStop);

   	if (numStartActivities == 0 &&
   		numStopActivities == 0 &&
   	    (iContext.iNodeActivity == NULL || iContext.iNodeActivity->FindOriginator(iContext.NodeId()) == KErrNotFound))
       	{
       	//Send TGoneDown to every control client unless the originator is the local node,
       	//in which case the originator will be responsible for sending TGoneDowns.
       	
       	const TProviderInfo& providerInfo = static_cast<const TProviderInfoExt&>(iContext.Node().AccessPointConfig().FindExtensionL(
       	     STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId))).iProviderInfo;
       	
       	TCFControlClient::TGoneDown goneDown(iStopCode, providerInfo.APId());
    	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
    		goneDown, TClientType(TCFClientType::ECtrl));
       	}

    if (iContext.iNodeActivity)
        {
        iContext.iNodeActivity->SetError(KErrNone);
        }
	}

EXPORT_DEFINE_SMELEMENT(TStopDataClients, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TStopDataClients::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
	TInt aStopCode = ExtractErrorCode(iContext.iMessage);
    // Stop all non-default data clients before the default data client, as there are some cases where non-default data clients
    // have a reference to the default data client.  Also, stop non-default data clients unconditionally (i.e. whether started or
    // not) and the default data client only if started.  This ensures that a non-default data client that is still starting
    // will receive the stop, so preventing a hang.
    //
    // NOTE: the logic in this method is coupled to the logic in TNoTagOrDataClientsToStop.
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),TCFDataClient::TStop(aStopCode).CRef(), TClientType(TCFClientType::EData), TClientType(0, TClientType::ELeaving|TCFClientType::EDefault), TCFClientType::EStopping);
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),TCFDataClient::TStop(aStopCode).CRef(), TClientType(TCFClientType::EData, TCFClientType::EStarted|TCFClientType::EDefault), TClientType(0, TClientType::ELeaving), TCFClientType::EStopping);
    iContext.iNodeActivity->ClearPostedTo();
	}

void TStopDataClients::StopDataClient(RNodeInterface& aDataClient, TInt aStopCode)
    {
    aDataClient.SetFlags(TCFClientType::EStopping);
    aDataClient.PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), TCFDataClient::TStop(aStopCode).CRef());
    }


EXPORT_DEFINE_SMELEMENT(TStopSelf, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TStopSelf::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
	TInt stopCode = ExtractErrorCode(iContext.iMessage);

	iContext.iNodeActivity->PostRequestTo(iContext.NodeId(),
	    TCFDataClient::TStop(stopCode).CRef());
	}

//Awaiting one data client stopped
EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientStopped, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingDataClientStopped::Accept()
	{
	TCFDataClient::TStopped* dcStopped = message_cast<TCFDataClient::TStopped>(&iContext.iMessage);
	if (dcStopped == NULL)
    	{
    	return EFalse;
    	}
	if (iContext.iPeer)
		{
		iContext.iPeer->ClearFlags(TCFClientType::EStarted | TCFClientType::EStopping); 		
		}
	if (iContext.iNodeActivity &&
	    (iContext.iNodeActivity->ActivitySigId() == ECFActivityStop ||
	     iContext.iNodeActivity->ActivitySigId() == ECFActivityStopDataClient))
    	{
    	iContext.iNodeActivity->SetError(dcStopped->iValue);
    	}
    return ETrue;
	}

//Awaiting all of the data clients stopped
EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientsStopped, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingDataClientsStopped::Accept()
	{
	TCFDataClient::TStopped* dcStopped = message_cast<TCFDataClient::TStopped>(&iContext.iMessage);
	if (dcStopped == NULL)
    	{
    	return EFalse;
    	}
	if (iContext.iPeer)
		{
		iContext.iPeer->ClearFlags(TCFClientType::EStarted | TCFClientType::EStopping); 
		}
	if (iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EStopping)))  
		{
		//There are more to wait for
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}
	if (iContext.iNodeActivity &&
	    (iContext.iNodeActivity->ActivitySigId() == ECFActivityStop ||
	     iContext.iNodeActivity->ActivitySigId() == ECFActivityStopDataClient))
    	{
    	iContext.iNodeActivity->SetError(dcStopped->iValue);
    	}
	//Last one - accept
    return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TSendStop, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TSendStop::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
	__ASSERT_DEBUG(iContext.Node().ServiceProvider(), CorePrPanic(KPanicNoServiceProvider));

	TInt stopCode = ExtractErrorCode(iContext.iMessage);

	iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(), TCFServiceProvider::TStop(stopCode).CRef());
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientGoneDown, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingDataClientGoneDown::Accept()
	{
	if (! iContext.iMessage.IsMessage<TCFControlProvider::TDataClientGoneDown>())
    	{
    	return EFalse;
    	}

	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrStaCPRSC, 16)); //This is a peer message
	iContext.iPeer->ClearFlags(TCFClientType::EStarted);
    return ETrue;
	}

//-=========================================================
//
//NoBearer
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TAwaitingNoBearer, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool TAwaitingNoBearer::Accept()
	{
	return iContext.iMessage.IsMessage<TCFControlProvider::TNoBearer>();
	}

EXPORT_DEFINE_SMELEMENT(TSendNoBearer, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendNoBearer::DoL()
	{
	__ASSERT_DEBUG(iContext.Node().ServiceProvider()==NULL, CorePrPanic(KPanicUnexpectedExecutionPath));
	RNodeInterface* ctrlProvider = iContext.Node().ControlProvider();
	__ASSERT_DEBUG(ctrlProvider, CorePrPanic(KPanicNoControlProvider));
	User::LeaveIfError(ctrlProvider? KErrNone : KErrCorrupt);
	iContext.iNodeActivity->PostRequestTo(*ctrlProvider, TCFControlProvider::TNoBearer().CRef());
	}

EXPORT_DEFINE_SMELEMENT(TSendBindTo, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSendBindTo::DoL()
	{
	//This is assumed to be executed as a direct response to TCommsBinderResponse
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));

	//Create a binder to send.
	TCFServiceProvider::TCommsBinderResponse& binderResponse = message_cast<TCFServiceProvider::TCommsBinderResponse>(iContext.iMessage);

	//TODO: TNodePeerId should become internal. When this happens, FirstOriginator should return
	//      RNodeInterface.
	iContext.iNodeActivity->PostRequestTo(
		iContext.iNodeActivity->SoleOriginator().Peer(),
		TCFDataClient::TBindTo(binderResponse.iNodeId).CRef());
	}

EXPORT_DEFINE_SMELEMENT(TSendBearer, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSendBearer::DoL()
	{
	if (iContext.iNodeActivity)
		{
	    iContext.iNodeActivity->PostToOriginators(TCFControlProvider::TBearer().CRef());
		}
	else
		{
		RClientInterface::OpenPostMessageClose(iContext.NodeId(), iContext.iSender, TCFControlProvider::TBearer().CRef());
		}
	}

EXPORT_DEFINE_SMELEMENT(TSendControlClientJoinRequest, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSendControlClientJoinRequest::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));

	//The transition can be placed after (to process) the following messages:

	//(1)TCFDataClient::TBindTo - carrying the new service provider for this node.
	const TCFDataClient::TBindTo* bindToMsg(message_cast<const TCFDataClient::TBindTo>(&iContext.iMessage));
	//(2)TCFServiceProvider::TCommsBinderResponse - carrying the auxilary service provider for this node (this node
	//may sometimes want to join the data client of its service provider.
	const TCFServiceProvider::TCommsBinderResponse* binderResponseMsg(message_cast<const TCFServiceProvider::TCommsBinderResponse>(&iContext.iMessage));

    __ASSERT_DEBUG(bindToMsg || binderResponseMsg , User::Panic(KSpecAssert_ESockCrStaCPRSC, 17));

    TNodeId newServiceProvider = bindToMsg ? bindToMsg->iNodeId : binderResponseMsg->iNodeId;
	RNodeInterface* sp = iContext.Node().AddClientL(newServiceProvider, TClientType(TCFClientType::EServProvider, TCFClientType::EActive));
	__ASSERT_DEBUG(sp != NULL, CorePrPanic(KPanicNoServiceProvider));
	User::LeaveIfError(sp? KErrNone : KErrCorrupt);

	//Join the service provider
	iContext.iNodeActivity->PostRequestTo(*sp, TCFPeer::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::ECtrl)).CRef());
	}

EXPORT_DEFINE_SMELEMENT(TRequestCommsBinder, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TRequestCommsBinder::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));

	RNodeInterface* sp = iContext.Node().ServiceProvider();
	__ASSERT_DEBUG(sp != NULL, CorePrPanic(KPanicNoServiceProvider));
	User::LeaveIfError(sp? KErrNone : KErrCorrupt);

	iContext.iNodeActivity->PostRequestTo(*sp, TCFServiceProvider::TCommsBinderRequest().CRef());
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientStarted, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TAwaitingDataClientStarted::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFDataClient::TStarted>())
    	{
		__ASSERT_DEBUG(iContext.iPeer || iContext.iSender == iContext.NodeId(), User::Panic(KSpecAssert_ESockCrStaCPRSC, 18));
		if (iContext.iPeer)
			{
			iContext.iPeer->ClearFlags(TCFClientType::EStarting);
			iContext.iPeer->SetFlags(TCFClientType::EStarted);
			}
		return ETrue;
    	}

	//If this is TError, clean the EStarting flag but do not accept, clean or otherwise process
	if (iContext.iMessage.IsMessage<TEBase::TError>())
    	{
	    if (iContext.iPeer)
	        {
	        iContext.iPeer->ClearFlags(TCFClientType::EStarting);
	        }
    	}

	return EFalse;
	}

// Status in which it waits for all the DataClients which haven't yet Started (flagged as "Starting") to Start
EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientsStarted, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingDataClientsStarted::Accept()
	{
	CoreNetStates::TAwaitingDataClientStarted state(iContext);
	if (state.Accept())
		{
		if (iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData,TCFClientType::EStarting)))
		/* Meaning of the line above: 'Do I have any dataclient which has already sent the TStartDataClient message to but hasn't
		answered with TStarted yet?' */
			{
			// YES, so there are more to wait for
			iContext.iMessage.ClearMessageId();
			return EFalse;
			}
		return ETrue;
		}
	return EFalse;
	}

//-=========================================================
//
//Destroy
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TAbortAllActivitiesNodeDeletion, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TAbortAllActivitiesNodeDeletion::DoL()
	{//aborts all running activities fr all originator except me
	iContext.Node().AbortActivitiesOriginatedBy(iContext, TNodeId::NullId(), ETrue);
	}

EXPORT_DEFINE_SMELEMENT(TAbortAllActivities, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TAbortAllActivities::DoL()
	{//aborts all running activities fr all originator except me
	iContext.Node().AbortActivitiesOriginatedBy(iContext);
	}

EXPORT_DEFINE_SMELEMENT(TSetIdleIfNoServiceProviders, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSetIdleIfNoServiceProviders::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
   	if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider))==NULL)
       	{
       	iContext.iNodeActivity->SetIdle();
       	}
	}

EXPORT_DEFINE_SMELEMENT(TSetIdleIfMoreClients, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSetIdleIfMoreClients::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
   	if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl|TCFClientType::EData))!=NULL)
       	{
       	iContext.iNodeActivity->SetIdle();
       	}
	}

EXPORT_DEFINE_SMELEMENT(TSendClientLeavingAndRemoveControlProvider, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSendClientLeavingAndRemoveControlProvider::DoL()
	{
   	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrlProvider));
   	RNodeInterface* cl = iter[0];
   	//It is perfectly possible that there is no Control Provider at all.
   	if (cl)
   		{
		__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
   		iContext.iNodeActivity->PostToOriginators(TEChild::TLeft().CRef());
   		iContext.Node().RemoveClient(cl->RecipientId(),iContext);
   		__ASSERT_DEBUG(iter[1] == NULL, User::Panic(KSpecAssert_ESockCrStaCPRSC, 19)); //But it is not possible to have two Control Providers!
   		}
	}

EXPORT_DEFINE_SMELEMENT(TSendClientLeavingAndRemoveControlProviderIfNoServiceProviders, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSendClientLeavingAndRemoveControlProviderIfNoServiceProviders::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
   	if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider))==NULL)
       	{
       	TSendClientLeavingAndRemoveControlProvider::DoL();
       	}
	}

EXPORT_DEFINE_SMELEMENT(TSendClientLeavingRequestToServiceProviders, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSendClientLeavingRequestToServiceProviders::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));

	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
	for (TInt i = 0; iter[i]; i++)
		{
		RNodeInterface* cl = iter[i];

		// Do not send client leaving requests to service providers which are already leaving
		// (e.g., gone down).
 		if (!(cl->Flags() & TClientType::ELeaving))
		    {
	  		cl->PostMessage(
	   				TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
	   				TEPeer::TLeaveRequest().CRef()
	   				);

			//We are expecting answers from many service providers, set to NullCommsId.
			iContext.iNodeActivity->ClearPostedTo();
			iter[i]->SetFlags(TCFClientType::ELeaving);
		    }
		else
			{
			MESH_LOG((KCoreProviderStatesSubTag, _L8("TSendClientLeavingRequestToServiceProviders:\tDoL - IGNORING POST!")));
			}
		}
	}

EXPORT_DEFINE_SMELEMENT(TSendClientLeavingRequestToServiceProvider, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void TSendClientLeavingRequestToServiceProvider::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));

	//If the sender is a service provider, send to it (there may be more than one)
    RNodeInterface* sp = iContext.Node().FindClient(iContext.iSender);
    if (sp == NULL || !(sp->Type()&TCFClientType::EServProvider))
    	{
    	//Otherwise there must be only one sp on the node
    	TClientIter<TDefaultClientMatchPolicy> servProviders = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
    	__ASSERT_DEBUG(servProviders[0] && servProviders[1] == NULL, CorePrPanic(KPanicNoServiceProvider));
    	sp = servProviders[0];
    	}

    __ASSERT_DEBUG(sp, CorePrPanic(KPanicNoServiceProvider));
	iContext.iNodeActivity->PostRequestTo(*sp,
	TEPeer::TLeaveRequest().CRef());
	sp->SetFlags(TCFClientType::ELeaving);
	}

//-=========================================================
//
//Service Provider Going Up
//
//-=========================================================
DEFINE_SMELEMENT(TAwaitingGoneUp, NetStateMachine::MState, CoreStates::TContext)
TBool TAwaitingGoneUp::Accept()
	{
	if (! iContext.iMessage.IsMessage<TCFControlClient::TGoneUp>())
		{
    	return EFalse;
		}
	 iContext.iPeer->SetFlags(TCFClientType::EStarted);
	 return ETrue;
	}


//-=========================================================
//
//Service Provider Going Down
//
//-=========================================================
EXPORT_DEFINE_SMELEMENT(TAwaitingGoneDown, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TAwaitingGoneDown::Accept()
	{
	if (! iContext.iMessage.IsMessage<TCFControlClient::TGoneDown>())
    	{
    	return EFalse;
    	}
    if (iContext.iPeer)
        {
        iContext.iPeer->ClearFlags(TCFClientType::EStarted);
        }
    if (iContext.Node().CountActivities(ECFActivityStop) ||
        iContext.Node().CountActivities(ECFActivityStopDataClient) ||
        iContext.Node().CountActivities(ECFActivityDestroy))
        {
        //we're already stopping - eat the message
        iContext.iMessage.ClearMessageId();
        return EFalse;
        }
    return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TSendClientLeavingAndRemoveServiceProvider, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TSendClientLeavingAndRemoveServiceProvider::DoL()
	{
	//This must be a peer message
	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrStaCPRSC, 20));

    if (iContext.Node().ServiceProvider())
        {
    	//Activity id does not matter for TClientLeaving message since there is no response to it.
    	//Please do not ASSERT(iContext.iNodeActivity) here!
    	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
    		TEChild::TLeft().CRef(), TClientType(TCFClientType::EServProvider));
		// we should usually set the ELeaving flag on the RNodeInterface since we've sent ClientLeaving to
		// the node, but there's no need here, as the client is being deleted anyhow.
    	iContext.Node().RemoveClient(iContext.Node().ServiceProvider()->RecipientId(),iContext);
        }
	}

//-=========================================================
//
//Other
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TAwaitingStopped, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool TAwaitingStopped::Accept()
	{
    if (! iContext.iMessage.IsMessage<TCFServiceProvider::TStopped>() )
        {
        return EFalse;
        }

    if (iContext.iPeer)
        {
        iContext.iPeer->ClearFlags(TCFClientType::EStarted);
        }
    return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingStop, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool TAwaitingStop::Accept()
	{
	if (!iContext.iMessage.IsMessage<TCFServiceProvider::TStop>())
    	{
    	return EFalse;
    	}
    if (iContext.Node().ServiceProvider())
        {
        iContext.Node().ServiceProvider()->SetFlags(TCFClientType::EStarted);
        }
    return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingParamResponse, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TAwaitingParamResponse::Accept()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	if((iContext.iMessage.IsMessage<TCFScpr::TSetParamsResponse>()) || (iContext.iMessage.IsMessage<TCFScpr::TGetParamsResponse>()))
		{
		return ETrue;
		}
	return EFalse;
#else
	return iContext.iMessage.IsMessage<TCFScpr::TParamsResponse>();
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	}

EXPORT_DEFINE_SMELEMENT(TRespondWithRetrievedParams, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TRespondWithRetrievedParams::DoL()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	iContext.PostToSender(TCFScpr::TGetParamsResponse(iContext.Node().iParameterBundle).CRef());
#endif
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingConEvent, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TAwaitingConEvent::Accept()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	return iContext.iMessage.IsMessage<TCFSubConnControlClient::TPlaneNotification>();
#else
	return EFalse;
#endif
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingSubConEvent, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TAwaitingSubConEvent::Accept()
	{
	return iContext.iMessage.IsMessage<TCFSubConnControlClient::TSubConnNotification>();
	}



//-=========================================================
//
//Binder request
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TAwaitingBinderRequest, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingBinderRequest::Accept()
	{
	if (! iContext.iMessage.IsMessage<TCFServiceProvider::TCommsBinderRequest>())
		{
		return EFalse;
		}

	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrStaCPRSC, 21)); //TCommsBinderRequest is a peer message only!
	return ETrue;
	}

// Start all the DataClients
EXPORT_DEFINE_SMELEMENT(TStartDataClients, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TStartDataClients::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));

	//there should be a panic if we don't have any dataclients which are NOT in in EStarting or in EStarted state.
	//Reason: there is no dataclients which is possible to start (because all of them have been already started). Then why are we here?????
	TInt cliNum = iContext.Node().CountClients<TExcludeTypeAndFlagClientMatchPolicy>(
		TClientType(
			TCFClientType::EData,
			(TCFClientType::EStarted | TCFClientType::EStarting | TCFClientType::ELeaving)
			)
		);

	// TODO When called from SCprNoBearer before the node is started the node is panic'ed.
	// Rather than panic, ensure that the node is started and thus the service provider is started.
	// Thus started dataclients can be ignored.
	// __ASSERT_DEBUG(cliNum != 0, CorePrPanic(KNoClients));

	if (cliNum > 0)
		{
		//start all of the dataclients which haven't been started yet
		TClientIter<TExcludeTypeAndFlagClientMatchPolicy> iter = iContext.Node().GetClientIter<TExcludeTypeAndFlagClientMatchPolicy>(
				TClientType(
						TCFClientType::EData,
						(TCFClientType::EStarted | TCFClientType::EStarting | TCFClientType::ELeaving)
				)
			);

		RNodeInterface* ctl = NULL;
		while ((ctl = iter++) != NULL)
			{
			iContext.iNodeActivity->PostRequestTo(*ctl, TCFDataClient::TStart().CRef());
			ctl->SetFlags(TCFClientType::EStarting);
			}
		}
	else
		{
		// Self Post TStarted if no clients need to be started.
		iContext.iNodeActivity->PostRequestTo(iContext.NodeId(), TCFDataClient::TStarted().CRef());
		}

	if (cliNum >1)
		{
		iContext.iNodeActivity->ClearPostedTo();
		}
	}

EXPORT_DEFINE_SMELEMENT(TSendDataClientStarted, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendDataClientStarted::DoL()
  	{
  	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
  	iContext.iNodeActivity->PostToOriginators(TCFDataClient::TStarted().CRef());

	// Tell control provider that we have started.. for Tier Notification

	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
		TCFControlProvider::TDataClientStatusChange(TCFControlProvider::TDataClientStatusChange::EStarted).CRef(), TClientType(TCFClientType::ECtrlProvider));
  	}

EXPORT_DEFINE_SMELEMENT(TStartServiceProvider, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TStartServiceProvider::DoL()
	{
	RNodeInterface* sp = iContext.Node().ServiceProvider();
    __ASSERT_DEBUG(sp, CorePrPanic(KPanicNoServiceProvider));
	User::LeaveIfError(sp? KErrNone : KErrCorrupt);

	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
    iContext.iNodeActivity->PostRequestTo(*sp, TCFServiceProvider::TStart().CRef());

	sp->SetFlags(TCFClientType::EStarting);
	}

//-=========================================================
//
//  Send TGoneUp
//
//-=========================================================
DEFINE_SMELEMENT(TSendGoneUp, NetStateMachine::MStateTransition, CoreNetStates::TContext)
void TSendGoneUp::DoL()
	{
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl));
	for (TInt i = 0; iter[i]; i++)
		{
		// Send TGoneUp to every Ctrl client except the originator (who would be recieving TStarted)
		// (activity might be the GoneUp Activity rather than the Start Activity, but then the originator
		// would be a ServiceProvider, and so the ControlClient will not be found)
		if (iContext.iNodeActivity && iContext.iNodeActivity->FindOriginator(*iter[i]) == KErrNotFound)
			{
			iter[i]->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), TCFControlClient::TGoneUp());
			}
		}
	}

EXPORT_DEFINE_SMELEMENT(TSendStarted, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TSendStarted::DoL()
	{
	TCFServiceProvider::TStarted msg;
	if (iContext.iNodeActivity)
		{
		iContext.iNodeActivity->PostToOriginators(msg);

		TSendGoneUp(iContext).DoL();
		}
	else
		{
		//This transition can also be used from a single tripple activity
		iContext.PostToSender(msg);

		TSendGoneUp(iContext).DoL();
		}
	}

EXPORT_DEFINE_SMELEMENT(TPassPlaneEventToControlClients, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TPassPlaneEventToControlClients::DoL()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	TCFSubConnControlClient::TPlaneNotification& event = message_cast<TCFSubConnControlClient::TPlaneNotification>(iContext.iMessage);

   	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl));
    RNodeInterface* ctl;
	while ((ctl = iter++) != NULL)
		{
		event.iRefCountOwnedNotification->Open();
		ctl->PostMessage(iContext.NodeId(), event);
		}

	// This Close() matches the Open performed by the service provider node Open() function
    event.iRefCountOwnedNotification->Close();
#endif
	}

EXPORT_DEFINE_SMELEMENT(TSendStopped, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendStopped::DoL()
	{
	//Please note that PostToOriginators overwrites the activity id,
	//RNodeInterface::PostMessage does not!
	TCFServiceProvider::TStopped msg(KErrNone);
	if (iContext.iNodeActivity)
		{
		iContext.iNodeActivity->PostToOriginators(msg);
		}
	else
		{
		//This transition can also be used from a single tripple activity
		iContext.PostToSender(msg);
		}
	}

EXPORT_DEFINE_SMELEMENT(TSendStoppedAndGoneDown, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendStoppedAndGoneDown::DoL()
	{
	TInt stopCode = ExtractErrorCode(iContext.iMessage, iContext.iNodeActivity);

	//Please note that PostToOriginators overwrites the activity id,
	//RNodeInterface::PostMessage does not!
	TCFServiceProvider::TStopped msg(stopCode);
	if (iContext.iNodeActivity)
		{
		iContext.iNodeActivity->PostToOriginators(msg);
		}
	else
		{
		//This transition can also be used from a single tripple activity
		iContext.PostToSender(msg);
		}

    const TProviderInfo& providerInfo = static_cast<const TProviderInfoExt&>(iContext.Node().AccessPointConfig().FindExtensionL(
            STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId))).iProviderInfo;

	TCFControlClient::TGoneDown goneDown(stopCode, providerInfo.APId());
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl));
    CNodeActivityBase* startActivity = iContext.Node().FindActivityById(ECFActivityStart);

    for (TInt i = 0; iter[i]; i++)
		{
		//Send TGoneDown to every Ctrl client except
		// * the originator (who would be recieving TStopped)
		// * originators of the start activity (these will be errored separately)
        if (iContext.iNodeActivity && iContext.iNodeActivity->FindOriginator(*iter[i]) >= 0)
            {
            continue; // ControlClient is a Stop originator
            }
		
        // So far the control client is not a Stop originator
        if (startActivity == NULL || startActivity->FindOriginator(*iter[i]) == KErrNotFound)
			{
            // ControlClient is not a Start originator
			iter[i]->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), goneDown);
			}
		}

	if (iContext.iNodeActivity)
    	{
        iContext.iNodeActivity->SetError(KErrNone);
    	}
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingStarted, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool TAwaitingStarted::Accept()
    {
    if (iContext.iMessage.IsMessage<TCFServiceProvider::TStarted>())
        {
        if (iContext.iPeer)
            {
            iContext.iPeer->ClearFlags(TCFClientType::EStarting);
            iContext.iPeer->SetFlags(TCFClientType::EStarted);
            }
        if (iContext.iNodeActivity)
            {//The node has responded. Core providers doesn't support the notion of multiple service providers,
             //therefore this code has been written to expect that TCFServiceProvider::TStart would only ever be
             //sent to one service provider as a simple request<>response handshake. The client has now responded
             //and its job is now done. ClearPostedTo so that TCancels aren't forwarded.
            iContext.iNodeActivity->ClearPostedTo();
            }
        return ETrue;
        }

    //If this is TError, clean the EStarting flag but do not accept, clean or otherwise process
    if (iContext.iMessage.IsMessage<TEBase::TError>())
        {
        if (iContext.iPeer)
            {
            iContext.iPeer->ClearFlags(TCFClientType::EStarting);
            }
        if (iContext.iNodeActivity)
            {//The node has responded. Core providers doesn't support the notion of multiple service providers,
             //therefore this code has been written to expect that TCFServiceProvider::TStart would only ever be
             //sent to one service provider as a simple request<>response handshake. The client has now responded
             //and its job is now done. ClearPostedTo so that TCancels aren't forwarded.
            iContext.iNodeActivity->ClearPostedTo();
            }
        }

    return EFalse;
    }

EXPORT_DEFINE_SMELEMENT(TSendBindToComplete, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TSendBindToComplete::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
	iContext.iNodeActivity->PostToOriginators(TCFDataClient::TBindToComplete().CRef());
	}

EXPORT_DEFINE_SMELEMENT(TBindSelfToPresentBearer, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TBindSelfToPresentBearer::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));

	__ASSERT_DEBUG(iContext.Node().ServiceProvider(), User::Panic(KSpecAssert_ESockCrStaCPRSC, 22));
	TNodeId bearer = iContext.Node().ServiceProvider()->RecipientId();
	iContext.iNodeActivity->PostRequestTo(iContext.NodeId(), TCFDataClient::TBindTo(bearer).CRef());
	}

EXPORT_DEFINE_SMELEMENT(TBindSelf, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TBindSelf::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
	const TCFDataClient::TBindTo& bindToMessage = message_cast<TCFDataClient::TBindTo>(iContext.iMessage);
	iContext.iNodeActivity->PostRequestTo(iContext.NodeId(), TCFDataClient::TBindTo(bindToMessage.iNodeId).CRef());
	}

EXPORT_DEFINE_SMELEMENT(TStartSelf, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TStartSelf::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
	iContext.iNodeActivity->PostRequestTo(iContext.NodeId(), TCFDataClient::TStart().CRef());
	}

EXPORT_DEFINE_SMELEMENT(TStoreParams, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TStoreParams::DoL()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	if (iContext.Node().CountActivities(ECFActivityApplyChanges))
    	{
    	User::Leave(KErrInUse);
    	}
	TCFScpr::TSetParamsRequest* setParamsRequest = message_cast<TCFScpr::TSetParamsRequest>(&iContext.iMessage);
	TCFScpr::TSetParamsResponse* setParamsResponse = message_cast<TCFScpr::TSetParamsResponse>(&iContext.iMessage);
	__ASSERT_DEBUG(setParamsRequest || setParamsResponse, User::Panic(KSpecAssert_ESockCrStaCPRSC, 23));

	RCFParameterFamilyBundleC newParamBundle = setParamsRequest ? setParamsRequest->iFamilyBundle : setParamsResponse->iFamilyBundle;

	if ( ! newParamBundle.IsNull()
		 && (iContext.Node().iParameterBundle.IsNull() ||
			 iContext.Node().iParameterBundle != newParamBundle))
    	{
    	if ( ! iContext.Node().iParameterBundle.IsNull())
        	{
        	iContext.Node().iParameterBundle.Close();
        	}

    	iContext.Node().iParameterBundle.Open(newParamBundle);
    	}
#endif
	}

EXPORT_DEFINE_SMELEMENT(TRespondWithCurrentParams, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TRespondWithCurrentParams::DoL()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	TCFScpr::TSetParamsRequest& paramRequest = message_cast<TCFScpr::TSetParamsRequest>(iContext.iMessage);
	if ( (! paramRequest.iFamilyBundle.IsNull()) && iContext.Node().iParameterBundle != paramRequest.iFamilyBundle )
    	{
    	//default behaviour. Parameters where supplied, though haven't been processed
    	User::Leave(KErrNotSupported);
    	}
	iContext.PostToSender(TCFScpr::TSetParamsResponse(iContext.Node().iParameterBundle).CRef());
#endif
	}

EXPORT_DEFINE_SMELEMENT(TSendApplyResponse, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendApplyResponse::DoL()
    {
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRSC, 24));
  	iContext.iNodeActivity->PostToOriginators(TCFScpr::TApplyResponse().CRef());
    }

EXPORT_DEFINE_SMELEMENT(TPassToServiceProvider, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TPassToServiceProvider::DoL()
	{
	//If you don't have the service provider, don't use this transition.
	__ASSERT_DEBUG(iContext.Node().ServiceProvider(), User::Panic(KSpecAssert_ESockCrStaCPRSC, 25));
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRSC, 26));
  	iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(), iContext.iMessage);
	}


EXPORT_DEFINE_SMELEMENT(TSendParamsToServiceProvider, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendParamsToServiceProvider::DoL()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	__ASSERT_DEBUG( ! iContext.Node().iParameterBundle.IsNull(), User::Panic(KSpecAssert_ESockCrStaCPRSC, 27));
	iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(),
	    TCFScpr::TSetParamsRequest(iContext.Node().iParameterBundle).CRef());
#endif
	}


//-=========================================================
//
//States
//
//-=========================================================

// This allows an activity to hang (for cases where it must
//  remain in existence in order to respond to a cancel)
EXPORT_DEFINE_SMELEMENT(TNeverAccept, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TNeverAccept::Accept()
	{
	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientStart, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool TAwaitingDataClientStart::Accept()
	{
	return iContext.iMessage.IsMessage<TCFDataClient::TStart>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingStart, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool TAwaitingStart::Accept()
	{
 	if (iContext.iMessage.IsMessage<TCFServiceProvider::TStart>())
 		{
 		__ASSERT_DEBUG(iContext.iPeer || iContext.iSender == iContext.NodeId(), CorePrPanic(KPanicPeerMessage));
 		if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EStarted)) == NULL)
 			{
 			return ETrue;
			}

 		iContext.iPeer->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), TCFServiceProvider::TStarted().CRef());
 		//Legacy progress notification - will hopefully be ignored by anything else than CConnection.
        iContext.iPeer->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), TCFMessage::TStateChange(TStateChange(KLinkLayerOpen, KErrNone)).CRef());
 		iContext.iMessage.ClearMessageId();
 		}
 	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientStop, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool TAwaitingDataClientStop::Accept()
	{
	return iContext.iMessage.IsMessage<TCFDataClient::TStop>();
	}


EXPORT_DEFINE_SMELEMENT(TAwaitingBindTo, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TAwaitingBindTo::Accept()
	{
	return iContext.iMessage.IsMessage<TCFDataClient::TBindTo>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingBindToOrCancel, NetStateMachine::MState, CoreStates::TContext)
TBool TAwaitingBindToOrCancel::Accept()
	{
	TAwaitingBindTo st(iContext);
	if (st.Accept())
		{
		return ETrue;
		}
	if (iContext.iMessage.IsMessage<TEBase::TCancel>())
		{
		iContext.iNodeActivity->SetError(KErrCancel);
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}
    return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientJoin, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool TAwaitingDataClientJoin::Accept()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
    TCFFactory::TPeerFoundOrCreated* dcJoined = message_cast<TCFFactory::TPeerFoundOrCreated>(&iContext.iMessage);
	if ( dcJoined )
    	{
    	if (iContext.iNodeActivity->Error() != KErrNone)
        	{
			TNodeCtxId returnId(iContext.ActivityId(), iContext.NodeId());

            RNodeInterface* clientAlreadyAdded = iContext.Node().FindClient(dcJoined->iNodeId);
            if (clientAlreadyAdded == NULL && !dcJoined->iNodeId.IsNull())
                {
                RClientInterface::OpenPostMessageClose(returnId, dcJoined->iNodeId, TEChild::TDestroy().CRef());
                // Add the node to our client list to prevent the TLeft response to the TDestroy
                // resulting in PRClientLeaveActivity failing to find the node and panicing.
                iContext.Node().AddClientL(dcJoined->iNodeId, TClientType(TCFClientType::EData, TClientType::ELeaving));
                }
            TEBase::TError simualtedErrorMessage(TCFFactory::TFindOrCreatePeer::Id(),
                iContext.iNodeActivity->Error());

            RClientInterface::OpenPostMessageClose(iContext.iSender, returnId, simualtedErrorMessage);
            iContext.iMessage.ClearMessageId();
            return EFalse;
        	}
        else
            {
        	return ETrue;
            }
    	}
    else if (iContext.iMessage.IsMessage<TEBase::TCancel>())
        {
        //We can't send TCancel to the factory, we better eat the message
        //here and switch the activity into a cancelling mode.
        iContext.iMessage.ClearMessageId();
        iContext.iNodeActivity->SetError(KErrCancel);
        }
    return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingJoinComplete, NetStateMachine::MState, CoreStates::TContext)
EXPORT_C TBool TAwaitingJoinComplete::Accept()
	{
	if(iContext.iMessage.IsMessage<TCFPeer::TJoinComplete>())
		{
		return ETrue;
		}
    else if(iContext.iMessage.IsMessage<TEBase::TError>())
        {
	    iContext.Node().RemoveClient(iContext.iSender);
        }
	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingBinderResponse, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool TAwaitingBinderResponse::Accept()
	{
	const TCFServiceProvider::TCommsBinderResponse* binderResponseMsg = message_cast<TCFServiceProvider::TCommsBinderResponse>(&iContext.iMessage);
	if (binderResponseMsg)
    	{
    	if (iContext.iNodeActivity && iContext.iNodeActivity->SupportsExtInterface(ABindingActivity::KInterfaceId))
        	{
       	    ABindingActivity* bindingActivity = reinterpret_cast<ABindingActivity*>(iContext.iNodeActivity->FetchExtInterface(ABindingActivity::KInterfaceId));
       	    __ASSERT_DEBUG(bindingActivity, User::Panic(KSpecAssert_ESockCrStaCPRSC, 28));
       	    bindingActivity->StoreOriginator(iContext.iSender);
        	}
        return ETrue;
    	}
    return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(CoreNetStates::TAwaitingBindToComplete, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool CoreNetStates::TAwaitingBindToComplete::Accept()
	{
	return iContext.iMessage.IsMessage<TCFDataClient::TBindToComplete>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingProvision, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingProvision::Accept()
	{
	return iContext.iMessage.IsMessage<TCFDataClient::TProvisionConfig>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingControlClientJoin, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingControlClientJoin::Accept()
	{
	return iContext.iMessage.IsMessage<TCFFactory::TPeerFoundOrCreated>()
		|| iContext.iMessage.IsMessage<TCFPeer::TJoinRequest>();
	}

DEFINE_SMELEMENT(TAwaitingCSRCreated, NetStateMachine::MState, CoreNetStates::TContext)
TBool CoreNetStates::TAwaitingCSRCreated::Accept()
    {
    return iContext.iMessage.IsMessage<TCFInternalEsock::TCSRCreated>();
    }

EXPORT_DEFINE_SMELEMENT(TAwaitingDataClientStatusChange, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool CoreNetStates::TAwaitingDataClientStatusChange::Accept()
	{
	return iContext.iMessage.IsMessage<TCFControlProvider::TDataClientStatusChange>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingProviderStatusChangeOrDataClientStatusChange, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool CoreNetStates::TAwaitingProviderStatusChangeOrDataClientStatusChange::Accept()
	{
	return iContext.iMessage.IsMessage<TCFMcpr::TProviderStatusChange>() || iContext.iMessage.IsMessage<TCFControlProvider::TDataClientStatusChange>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingLegacyRMessage2Ext, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool CoreNetStates::TAwaitingLegacyRMessage2Ext::Accept()
	{
	return iContext.iMessage.IsTypeOf(Meta::STypeId::CreateSTypeId(TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId));
	}


EXPORT_DEFINE_SMELEMENT(TAwaitingRMessage2Processed, NetStateMachine::MState, CoreNetStates::TContext)
EXPORT_C TBool CoreNetStates::TAwaitingRMessage2Processed::Accept()
	{
	return iContext.iMessage.IsMessage<TCFLegacyMessage::TLegacyRMessage2Processed>();
	}

EXPORT_C void CoreNetStates::TAwaitingRMessage2Processed::Cancel()
	{
	MeshMachine::CNodeParallelMessageStoreActivityBase* act = static_cast<MeshMachine::CNodeParallelMessageStoreActivityBase*>(iContext.Activity());

	ASSERT(act->Message().IsTypeOf(Meta::STypeId::CreateSTypeId(TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId)));
	static_cast<TCFSigLegacyRMessage2Ext&>(act->Message()).Cancel(iContext);
	
	act->SetError(KErrCancel);
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingParamRequest, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingParamRequest::Accept()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW	
	if(iContext.iMessage.IsMessage<TCFScpr::TSetParamsRequest>() || iContext.iMessage.IsMessage<TCFScpr::TGetParamsRequest>())
		{
		return ETrue;
		}
#endif
	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingApplyRequest, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool TAwaitingApplyRequest::Accept()
    {
	return iContext.iMessage.IsMessage<TCFScpr::TApplyRequest>();
    }

//-=========================================================
//
//State Forks
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TPeerPresent, NetStateMachine::MStateFork, CoreStates::TContext)
TInt TPeerPresent::TransitionTag()
	{
	return CoreStates::KPeerPresent | NetStateMachine::EForward;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagOrNoPeer, NetStateMachine::MStateFork, CoreStates::TContext)
TInt TNoTagOrNoPeer::TransitionTag()
	{
	if (iContext.iPeer)
		{
		return MeshMachine::KNoTag | NetStateMachine::EForward;
		}
	return CoreStates::KNoPeer | NetStateMachine::EForward;
	}

EXPORT_DEFINE_SMELEMENT(TRetryOrIgnoreOrPropagate, NetStateMachine::MStateFork, CoreStates::TContext)
TInt TRetryOrIgnoreOrPropagate::TransitionTag()
	{
    TErrResponse& resp = message_cast<TEErrorRecovery::TErrorRecoveryResponse>(iContext.iMessage).iErrResponse;
    if (resp.iAction == TErrResponse::EPropagate)
        {
        return CoreStates::KPropagate | NetStateMachine::EForward;
        }
    if (resp.iAction == TErrResponse::EIgnore)
        {
        return CoreStates::KIgnore | NetStateMachine::EForward;
        }
    return CoreStates::KRetry | NetStateMachine::EForward;
	}

// This transition tag follows the logic that any error is recoverable except for KErrCancel
// A KErrCancel error can come from either the activity or the current message if its a TError message
// A KErrCancel takes precedence over other errors
EXPORT_DEFINE_SMELEMENT(TNoTagOrRecoverableErrorTagOrErrorTag, NetStateMachine::MStateFork, CoreStates::TContext)
EXPORT_C TInt TNoTagOrRecoverableErrorTagOrErrorTag::TransitionTag()
    {
	TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
	if (KErrNone==iContext.iNodeActivity->Error() && NULL==msg)
		{
		// Not a TError message and no error present in the activity
		return MeshMachine::KNoTag | NetStateMachine::EForward;
		}
	else if (iContext.iNodeActivity->Error() == KErrCancel || (msg && msg->iValue == KErrCancel))
		{
		// Non recoverable KErrCancel
		iContext.iNodeActivity->SetError(KErrCancel);
		return KErrorTag | NetStateMachine::EForward;
		}
	else
		{
		// Any other error
		if (msg)
			{
			// If the error came from the message then update the activity's error
			iContext.iNodeActivity->SetError(msg->iValue);
			}
		return CoreStates::KRecoverableErrorTag | NetStateMachine::EForward;
		}
    }
    
EXPORT_DEFINE_SMELEMENT(TBearerPresent, NetStateMachine::MStateFork, CoreNetStates::TContext)
EXPORT_C TInt TBearerPresent::TransitionTag()
    {
    return CoreNetStates::KBearerPresent | NetStateMachine::EForward;
    }

EXPORT_DEFINE_SMELEMENT(TNoTagOrDataClientPresent, NetStateMachine::MStateFork, CoreNetStates::TContext)
EXPORT_C TInt TNoTagOrDataClientPresent::TransitionTag()
	{
	if (iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData)))
		{
       	return CoreNetStates::KDataClientPresent | NetStateMachine::EForward;
    	}
	return KNoTag;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagOrDataClientsToStop, NetStateMachine::MStateFork, CoreNetStates::TContext)
EXPORT_C TInt TNoTagOrDataClientsToStop::TransitionTag()
	{
    // Check if there are any non-default data clients, or the default data client is started.
    // NOTE: the logic in this method is coupled to the logic in TStopDataClients - see that method for further explanation.
    if ((iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EDefault | TCFClientType::EStarted), TClientType(0, TClientType::ELeaving))) 
	 || (iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData), TClientType(0, TCFClientType::EDefault | TClientType::ELeaving))))
        {
        return CoreNetStates::KDataClientsToStop;
        }
    return KNoTag;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagOrNoDataClientsToStop, NetStateMachine::MStateFork, CoreNetStates::TContext)
TInt TNoTagOrNoDataClientsToStop::TransitionTag()
/**
Return:
KNoTag					There are data clients present that are not marked EStopped
KNoDataClientsToStop	There are no data clients present that are not marked EStopped
*/
	{
	if (iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EStarted)))
		{
		return KNoTag;
		}
	return CoreNetStates::KNoDataClientsToStop;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagOnEvent, NetStateMachine::MStateFork, CoreNetStates::TContext)
TInt TNoTagOnEvent::TransitionTag()
	{
	if (iContext.iNodeActivity)
    	{
    	iContext.iNodeActivity->RemoveOriginator(0);
    	}
	return KNoTag;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagOrNoDataClients, NetStateMachine::MStateFork, CoreNetStates::TContext)
EXPORT_C TInt TNoTagOrNoDataClients::TransitionTag()
	{
	//If we have a started flow already, we should not be in here
	if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData)))
		{
       	return MeshMachine::KNoTag | NetStateMachine::EForward;
    	}
	return CoreNetStates::KNoDataClients | NetStateMachine::EForward;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagOrBearerPresent, NetStateMachine::MStateFork, CoreNetStates::TContext)
EXPORT_C TInt TNoTagOrBearerPresent::TransitionTag()
    {
    return iContext.Node().ServiceProvider()? CoreNetStates::KBearerPresent : MeshMachine::KNoTag;
    }

EXPORT_DEFINE_SMELEMENT(TNoTagOrBearerPresentOrErrorTag, NetStateMachine::MStateFork, CoreNetStates::TContext)
TInt TNoTagOrBearerPresentOrErrorTag::TransitionTag()
	{
	TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
	if (KErrNone==iContext.iNodeActivity->Error() && NULL==msg)
		{
		CoreNetStates::TNoTagOrBearerPresent fork(iContext);
		return fork.TransitionTag();
		}
	if (msg)
		{
		iContext.iNodeActivity->SetError(msg->iValue);
		}
	return KErrorTag | NetStateMachine::EForward;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagOrNoBearer, NetStateMachine::MStateFork, CoreNetStates::TContext)
TInt TNoTagOrNoBearer::TransitionTag()
    {
    return iContext.Node().ServiceProvider()? MeshMachine::KNoTag : CoreNetStates::KNoBearer;
    }

EXPORT_DEFINE_SMELEMENT(TNoTagOrNonDefault, NetStateMachine::MStateFork, CoreNetStates::TContext)
TInt TNoTagOrNonDefault::TransitionTag()
/**
Returns KNoTag uif sender is marked EDefault, else CoreNetStates::KNonDefault.
*/
    {
    __ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrStaCPRSC, 29));
    return (iContext.iPeer->Flags() & TCFClientType::EDefault) ? KNoTag : CoreNetStates::KNonDefault;
    }

EXPORT_DEFINE_SMELEMENT(TNoTagOrNoClients, NetStateMachine::MStateFork, CoreNetStates::TContext)
TInt TNoTagOrNoClients::TransitionTag()
/**
Return KNoTag if there are data or control clients, else return KNoClients.
*/
    {
    return iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData | TCFClientType::ECtrl))? KNoTag : KNoClients;
    }

EXPORT_DEFINE_SMELEMENT(TNoTagBackwardsOrProviderStopped, NetStateMachine::MStateFork, CoreNetStates::TContext)
EXPORT_C TInt TNoTagBackwardsOrProviderStopped::TransitionTag()
	{
	if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(
	    TClientType(TCFClientType::EData, TCFClientType::EStarted)) != NULL)
		{
		// At least one started data client
        return MeshMachine::KNoTag | NetStateMachine::EBackward;
		}
	return CoreNetStates::KProviderStopped;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagOrUnbindOnStop, NetStateMachine::MStateFork, CoreNetStates::TContext)
EXPORT_C TInt TNoTagOrUnbindOnStop::TransitionTag()
	{
    if (iContext.Node().ServiceProvider() &&
        !(iContext.Node().ServiceProvider()->Flags() & TCFClientType::EStarted) &&
		!(iContext.Node().ServiceProvider()->Flags() & TCFClientType::ELeaving) &&
         (iContext.Node().CountActivities(ECFActivityStart) == 0))
        {
        return CoreNetStates::KUnbind;
        }
    return KNoTag;
	}


//-=========================================================
//
//Transitions
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TForwardStateChange, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TForwardStateChange::DoL()
	{
    //In some cirumstances a node can have more than one TCFClientType::EServProvider peer (for instance MCPRs can have more than one potential service provider), 
    //but within the coreprovider code there is no concept of multiple service providers per-se. There is only one service provider and it is the TCFClientType::EServProvider 
    //with TCFClientType::EActive flag set. If a progress comes from a TCFClientType::EServProvider that is not a service provider, the progress will be ignored here.

    if (iContext.iPeer && 
        iContext.iPeer->Type() == TCFClientType::EServProvider && 
        !(iContext.iPeer->Flags() & TCFClientType::EActive) )
        {
        return;
        }
	TInt ctrlClientCount = iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.iMessage, TClientType(TCFClientType::ECtrl));
	if (0==ctrlClientCount)
		{ //If there are no control clients any more, forward to the control provider
		iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.iMessage, TClientType(TCFClientType::ECtrlProvider));
		}
	}


EXPORT_DEFINE_SMELEMENT(TStoreProvision, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TStoreProvision::DoL()
	{
	TCFDataClient::TProvisionConfig& provisionMsg = message_cast<TCFDataClient::TProvisionConfig>(iContext.iMessage);
	iContext.Node().iAccessPointConfig.Close();
	iContext.Node().iAccessPointConfig.Open(provisionMsg.iConfig);
	}


EXPORT_DEFINE_SMELEMENT(TSendProvision, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendProvision::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));

	RNodeInterface* client = iContext.Node().FindClient(message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage).iNodeId);
	__ASSERT_ALWAYS(NULL != client && (client->Type() == TCFClientType::EData), CorePrPanic(KPanicIncorrectMessage));

	// If the data client was stopped in between the completion of the join and before we send the provision
	// message then the client will start leaving.  This means we should not send the provision message since
	// it is now redundant and it is not legal to send a message to a client which is leaving.
	if(!(client->Flags() & TClientType::ELeaving))
		{
		// The construction of the messagee will increase the ref count on the AccessPointConfig
		// The reference will be released once the message has dispatched
	    client->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
	            TCFDataClient::TProvisionConfig(iContext.Node().AccessPointConfig()).CRef());
		}
	else
		{
		MESH_LOG((KCoreProviderStatesSubTag, _L8("TSendProvision:\tDoL - IGNORING POST!")));
		}
	}

EXPORT_DEFINE_SMELEMENT(TCreateDataClient, NetStateMachine::MStateTransition, PRStates::TContext)

void TCreateDataClient::IssuePeerCreationRequestL(ESock::TCFServiceProvider::TCommsBinderRequest* aBinderRequestMessage /* = NULL */)
	{
	if (factoryobject_cast<CTierManagerBase>(&iContext.Node()))
		{
		RequestMetaConnPeer(iContext.Node().AccessPointConfig());	
		}
   else if (factoryobject_cast<CMetaConnectionProviderBase>(&iContext.Node()))
		{
		RequestConnPeer(iContext.Node().AccessPointConfig());	
		}
	else if (factoryobject_cast<CConnectionProviderBase>(&iContext.Node()))
		{
		RequestSubConnPeer(iContext.Node().AccessPointConfig(), aBinderRequestMessage);	
		}
	else if (factoryobject_cast<CSubConnectionProviderBase>(&iContext.Node()))
		{
		RequestDataPeer(iContext.Node().AccessPointConfig(), aBinderRequestMessage);	
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void TCreateDataClient::RequestMetaConnPeer(const RMetaExtensionContainerC& aAccessPointCfg)
	{
	TAlwaysFindFactoryQuery query;
	
  	const TLayerConfig* layerCfg = static_cast<const TLayerConfig*>(aAccessPointCfg.FindExtension(
  	      STypeId::CreateSTypeId(TLayerConfig::EUid,TLayerConfig::ETypeId)));
  	__ASSERT_ALWAYS(layerCfg, CorePrPanic(KPanicUnexpectedExecutionPath));

  	TCFFactory::TFindOrCreatePeer msg(TCFPlayerRole::EMetaConnPlane, layerCfg->MCprUid(), &query);
  	
	// Send the request to the central object broker
	iContext.iNodeActivity->PostRequestTo(SockManGlobals::Get()->iCommsFactoryContainerBroker, msg, EFalse);
	}

void TCreateDataClient::RequestConnPeer (const RMetaExtensionContainerC& aAccessPointCfg)
	{
  	TDefaultConnectionFactoryQuery query(iContext.Node().Id());
  	
  	const TLayerConfig* layerCfg = static_cast<const TLayerConfig*>(aAccessPointCfg.FindExtension(
  	      STypeId::CreateSTypeId(TLayerConfig::EUid,TLayerConfig::ETypeId)));
  	__ASSERT_ALWAYS(layerCfg, CorePrPanic(KPanicUnexpectedExecutionPath));
  	
    TCFFactory::TFindOrCreatePeer msg(TCFPlayerRole::EConnPlane, layerCfg->CprUid(), &query);
    
   	// Send the request to the central object broker
	iContext.iNodeActivity->PostRequestTo(SockManGlobals::Get()->iCommsFactoryContainerBroker, msg, EFalse);
	}

void TCreateDataClient::RequestSubConnPeer (const RMetaExtensionContainerC& aAccessPointCfg, ESock::TCFServiceProvider::TCommsBinderRequest* aBinderRequestMessage)
	{
	// The subconnection type carried in the binder request is only relevant when we are expected to create an SCPR
	TSubConnOpen::TSubConnType scprType = TSubConnOpen::EAttachToDefault;
	// If we have a protocol extension to apply then do so but default to nothing

	TUid factory = TUid::Null();
	if (aBinderRequestMessage)
		{
		scprType = (TSubConnOpen::TSubConnType)aBinderRequestMessage->iValue;
		if (!aBinderRequestMessage->iFamilyBundle.IsNull())
			{
			// Find the family we are interested in
			RParameterFamily parameterFamily = aBinderRequestMessage->iFamilyBundle.FindFamily(KProtocolExtensionFamily);

			if(!parameterFamily.IsNull())
				{
				STypeId typeId = STypeId::CreateSTypeId(CSubConnectionProtocolParameterSet::EUid, CSubConnectionProtocolParameterSet::EType);
				CSubConnectionProtocolParameterSet* protocolParams =	static_cast<CSubConnectionProtocolParameterSet*>(parameterFamily.FindParameterSet(typeId, RParameterFamily::ERequested));
				TProtocolExtensionSpecifier protocolSpecifier;
				
			  	const TLayerConfig* layerCfg = static_cast<const TLayerConfig*>(aAccessPointCfg.FindExtension(
			  	      STypeId::CreateSTypeId(TLayerConfig::EUid,TLayerConfig::ETypeId)));
			  	__ASSERT_ALWAYS(layerCfg, CorePrPanic(KPanicUnexpectedExecutionPath));
			  	
				if(protocolParams->FindAndRemoveExtension(protocolSpecifier, layerCfg->TierId()) == KErrNone)
					{
					// We have our protocol extension specifier
					factory = protocolSpecifier.iSCPRFactoryUid;
					}
				}
			}
		}
	// No special protocol extension SCPP given so revert to the default which is given in the access point config
	if(factory == TUid::Null())
		{
	  	const TLayerConfig* layerCfg = static_cast<const TLayerConfig*>(aAccessPointCfg.FindExtension(
	  	      STypeId::CreateSTypeId(TLayerConfig::EUid,TLayerConfig::ETypeId)));
	  	__ASSERT_ALWAYS(layerCfg, CorePrPanic(KPanicUnexpectedExecutionPath));
	  	
	  	factory = layerCfg->SCprUid();
		}
	TDefaultSCPRFactoryQuery query (iContext.Node().Id(), scprType);
	TCFFactory::TFindOrCreatePeer msg(TCFPlayerRole::ESubConnPlane, factory, &query);
   	// Send the request to the central object broker
	iContext.iNodeActivity->PostRequestTo(SockManGlobals::Get()->iCommsFactoryContainerBroker, msg, EFalse);
	}

void TCreateDataClient::RequestDataPeer(const RMetaExtensionContainerC& aAccessPointCfg, ESock::TCFServiceProvider::TCommsBinderRequest* aBinderRequestMessage)
	{
	TNodeId controlProvider = iContext.Node().ControlProvider() ? iContext.Node().ControlProvider()->RecipientId() : TNodeId::NullId();
	TSubConnOpen::TSubConnType protocolType = TSubConnOpen::EAttachToDefault;
	// Get the flow factory identity from our access point configuration

	const TLayerConfig* layerCfg = static_cast<const TLayerConfig*>(aAccessPointCfg.FindExtension(
	        STypeId::CreateSTypeId(TLayerConfig::EUid,TLayerConfig::ETypeId)));
  	__ASSERT_ALWAYS(layerCfg, CorePrPanic(KPanicUnexpectedExecutionPath));
  	
	TUid factory = layerCfg->ProtocolUid();

	// Determine what protocol we want the flow factory to create
	if(aBinderRequestMessage && ! aBinderRequestMessage->iFamilyBundle.IsNull())
		{
		// Find the flow parameter family of parameters
		RParameterFamily parameterFamily = aBinderRequestMessage->iFamilyBundle.FindFamily(KFlowParametersFamily);

		if(!parameterFamily.IsNull())
			{
			STypeId typeId = STypeId::CreateSTypeId(CFlowRequestParameters::EUid, CFlowRequestParameters::EType);
			CFlowRequestParameters* flowParams =	static_cast<CFlowRequestParameters*>(parameterFamily.FindParameterSet(typeId, RParameterFamily::ERequested));

			// The type of flow created by the flow factory is dependent only on the protocol type
			protocolType = (TSubConnOpen::TSubConnType)flowParams->GetFlowParams().iProtocol;
			}
		}
	TDefaultFlowFactoryQuery query(controlProvider, iContext.Node().Id(), iContext.iMessage.MessageId(), protocolType);
	TCFFactory::TFindOrCreatePeer msg(TCFPlayerRole::EDataPlane, factory, &query);
   	// Send the request to the central object broker
	iContext.iNodeActivity->PostRequestTo(SockManGlobals::Get()->iCommsFactoryContainerBroker, msg, EFalse);
	}

EXPORT_C void TCreateDataClient::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));
	TCFServiceProvider::TCommsBinderRequest* binderReq = message_cast<TCFServiceProvider::TCommsBinderRequest>(&iContext.iMessage);

	__ASSERT_DEBUG(binderReq, CorePrPanic(KPanicIncorrectMessage));

	// Sender must be one of the control clients
	// Except in the special case of a CPR being asked by one of its data clients (an SCPR) to stack another below it
	__ASSERT_DEBUG(
		iContext.iSender == iContext.NodeId() ||
		iContext.iPeer->Type() == TCFClientType::ECtrl ||
		((iContext.iPeer->Type() == TCFClientType::EData) && (factoryobject_cast<CConnectionProviderBase>(&iContext.Node()) != NULL)), User::Panic(KSpecAssert_ESockCrStaCPRSC, 30));

	IssuePeerCreationRequestL(binderReq);

	}

EXPORT_DEFINE_SMELEMENT(TCancelDataClientStart, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TCancelDataClientStart::DoL()
	{
	CNodeActivityBase* dcstart = iContext.Node().FindActivityById(ECFActivityStartDataClient);
	if (dcstart)
		{
		dcstart->Cancel(iContext);
		}
	}

EXPORT_DEFINE_SMELEMENT(TProcessDataClientStop, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TProcessDataClientStop::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CorePrPanic(KPanicNoActivity));

	iContext.iNodeActivity->SetError(static_cast<TSigNumber&>(iContext.iMessage).iValue);
	}

EXPORT_DEFINE_SMELEMENT(TCancelAndCloseZone0ClientExtIfaces, NetStateMachine::MStateTransition, CoreNetStates::TContext)
void TCancelAndCloseZone0ClientExtIfaces::DoL()
	{
	//0 means we will cancel and close all open extensions!
	TCancelAndCloseAllClientExtItf msg(0);
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(iContext.NodeId(), msg,
		TClientType(TCFClientType::EServProvider), TClientType(0, TCFClientType::ELeaving));
	}

EXPORT_DEFINE_SMELEMENT(THandleDataClientStatusChange, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void THandleDataClientStatusChange::DoL()
 	{
    TCFControlProvider::TDataClientStatusChange& msg = message_cast<TCFControlProvider::TDataClientStatusChange>(iContext.iMessage);

   	if(msg.iValue == TCFControlProvider::TDataClientStatusChange::EStarted)
		{
		iContext.iPeer->SetFlags(TCFClientType::EStarted);

		// One of my data clients is started, therefore, I am started
		iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
			msg, TClientType(TCFClientType::ECtrlProvider));
		}
	else
		{
		iContext.iPeer->ClearFlags(TCFClientType::EStarted);
		}

    }

EXPORT_DEFINE_SMELEMENT(TSendDataClientStatusChangeStarted, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void TSendDataClientStatusChangeStarted::DoL()
  	{
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
		TCFControlProvider::TDataClientStatusChange(TCFControlProvider::TDataClientStatusChange::EStarted).CRef(), TClientType(TCFClientType::ECtrlProvider));
  	}


EXPORT_DEFINE_SMELEMENT(TProcessOrForwardRMessage2Ext, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TProcessOrForwardRMessage2Ext::DoL()
	{
	ASSERT(iContext.iMessage.IsTypeOf(Meta::STypeId::CreateSTypeId(TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId)));
	// static_cast as it will be a derivative and message_cast won't work
	TCFSigLegacyRMessage2Ext& msg = static_cast<TCFSigLegacyRMessage2Ext&>(iContext.iMessage);

	if (msg.CanProcess(iContext))
		{
		msg.ProcessL(iContext);
		}
	else
		{
		msg.ForwardL(iContext);
		}
	}

EXPORT_DEFINE_SMELEMENT(TCancelStart, NetStateMachine::MStateTransition, CoreNetStates::TContext)
EXPORT_C void TCancelStart::DoL()
	{
	CNodeActivityBase* startActivity = iContext.Node().FindActivityById(ECFActivityStart);
	if (startActivity)
		{
		startActivity->Cancel(iContext);
		}
	}

//-=========================================================
//
//  PRDestroyOrphans and PRDestroy
//
//-=========================================================

void DestroyFirstClient(PRStates::TContext& aContext, const TClientType& aIncClientType, const TClientType& aExcClientType = TClientType::NullType())
/**
Send a TDestroy to the first non-default data client, or to the default data client if there
are no non-default data clients.  We need to destroy the non-default data clients before the default data
client because there can be internal references from non-default clients to the default data client.  

The include and exclude iteration parameters are used to narrow the data client list as the caller requires.
*/
    {
    TClientIter<TDefaultClientMatchPolicy> iter = aContext.Node().GetClientIter<TDefaultClientMatchPolicy>(aIncClientType, aExcClientType);

    RNodeInterface* client = NULL;
    RNodeInterface* defaultClient = NULL;
    while ((client = iter++) != NULL)
        {
        if (!(client->Flags() & TCFClientType::EDefault))
            {
            // Found a non-default data client, so destroy it.
            break;
            }
        else
            {
            // Remember default data client.  Destroy it only if no non-default data clients.
            if (defaultClient == NULL)
                {
                defaultClient = client;
                }
            }
        }
    
    if (client == NULL)
        {
        client = defaultClient;
        }
    // Should we panic if client is NULL?
    if (client)
        {
        aContext.iNodeActivity->PostRequestTo(*client, TEChild::TDestroy().CRef());
        client->SetFlags(TClientType::ELeaving);
        }
     }

DEFINE_SMELEMENT(TDestroyFirstOrphan, NetStateMachine::MStateTransition, PRStates::TContext)
void TDestroyFirstOrphan::DoL()
/**
Destroy first orphan data client
*/
    {
    DestroyFirstClient(iContext, TClientType(TCFClientType::EData), TClientType(0, KOrphanExcludeFlags));
    }

DEFINE_SMELEMENT(TDestroyFirstClient, NetStateMachine::MStateTransition, PRStates::TContext)
void TDestroyFirstClient::DoL()
/**
Destroy first data client
*/
    {
    DestroyFirstClient(iContext, TClientType(TCFClientType::EData), TClientType(0, TCFClientType::ELeaving));
    }

DEFINE_SMELEMENT(TOrphansOrNoTag, NetStateMachine::MStateFork, PRStates::TContext)
TInt TOrphansOrNoTag::TransitionTag()
/**
If there are orphan data clients present, return KOrphans, else return KNoTag
*/
    {
    if (iContext.Node().CountClients<TDefaultClientMatchPolicy>(
            TClientType(TCFClientType::EData), TClientType(0, KOrphanExcludeFlags)))
        {
        return KOrphans;
        }
    return KNoTag;
    }

DEFINE_SMELEMENT(TOrphansBackwardsOrNoTag, NetStateMachine::MStateFork, PRStates::TContext)
TInt TOrphansBackwardsOrNoTag::TransitionTag()
/**
If there are orphan data clients present, return KOrphans|EBackward, else return KNoTag
*/
    {
    TOrphansOrNoTag orphansOrNoTag(iContext);
    TInt tag = orphansOrNoTag.TransitionTag();
    if (tag == KOrphans)
        {
        tag = KOrphans | NetStateMachine::EBackward;
        }
    return tag;
    }

DEFINE_SMELEMENT(TNoTagBackwardsOrNoClients, NetStateMachine::MStateFork, PRStates::TContext)
TInt TNoTagBackwardsOrNoClients::TransitionTag()
/**
If there are (non-leaving) data clients present, return KNoTag|EBackward, else return KNoClients
*/
    {
    TNonLeavingNoTagOrNoClients nonLeavingNoTagOrNoClients(iContext);
    TInt tag = nonLeavingNoTagOrNoClients.TransitionTag();
    if (tag == KNoTag)
        {
        tag = KNoTag | NetStateMachine::EBackward;
        }
    return tag;
    }


DEFINE_SMELEMENT(TNonLeavingNoTagOrNoClients, NetStateMachine::MStateFork, PRStates::TContext)
TInt TNonLeavingNoTagOrNoClients::TransitionTag()
/**
If there are (non-leaving) data clients left, return KNoTag, else return KNoClients
*/
    {
    if (iContext.Node().CountClients<TDefaultClientMatchPolicy>(
            TClientType(TCFClientType::EData), TClientType(0, TCFClientType::ELeaving)))
        {
        return KNoTag;
        }

    return KNoClients;
    }
