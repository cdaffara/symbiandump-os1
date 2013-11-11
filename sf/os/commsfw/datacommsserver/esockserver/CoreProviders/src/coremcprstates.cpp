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
// basic meta connection states and transitions
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//
//

/**
 @file
 @internalTechnology
*/


#include "coremcprstates.h"


#include <comms-infras/coremcpr.h>
#include <comms-infras/coremcpractivities.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_selector.h>
#include <comms-infras/ss_nodemessages_availability.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_mcpr.h>

#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <es_connpref.h>
#include <comms-infras/ss_coreprstates.h>
#include <elements/nm_messages_errorrecovery.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrPrvMCPRSC, "ESockCrPrvMCPRSC");
#endif


using namespace CoreStates;
using namespace MCprStates;
using namespace NetStateMachine;
using namespace ESock;
using namespace MCprActivities;
using namespace CorePanics;
using namespace Messages;
using namespace MeshMachine;


//
//Panics
void CoreMCprPanic(TInt aCode); // Defined in coremcpractivities.cpp


//
//Select Next Layer

EXPORT_DEFINE_SMELEMENT(TAwaitingSelectNextLayer, NetStateMachine::MState, MCprStates::TContext)
EXPORT_C TBool TAwaitingSelectNextLayer::Accept()
	{
	if (!iContext.iMessage.IsMessage<TCFSelector::TSimpleSelect>())
		{
		return EFalse;
		}

	__ASSERT_DEBUG(iContext.iPeer || iContext.iSender == iContext.NodeId(), CoreMCprPanic(KPanicPeerMessage));

	const TLayerSelectionInfo* selectInfo = static_cast<const TLayerSelectionInfo*>(iContext.Node().AccessPointConfig().FindExtension(
			STypeId::CreateSTypeId(TLayerSelectionInfo::EUid, TLayerSelectionInfo::ETypeId)));
  	__ASSERT_ALWAYS(selectInfo, CoreMCprPanic(KPanicUnexpectedExecutionPath));	
	
	//If this node is on the bottom of the stack (no mcprs below to be selected) we reply with SelectComplete(NULL).
	//Only APs at the link layer have the selectionpolicy and customselectionpolicy fields as 0.
	if (selectInfo->SelectionPolicy() == 0
		&& selectInfo->CustomSelectionPolicy() == 0
		&& iContext.Node().AccessPointConfig().FindExtension(TOverridenSelectionPrefsExt::TypeId()) == NULL)
		{
		iContext.PostToSender(TCFSelector::TSelectComplete(TNodeId::NullId(), TProviderInfo()).CRef());
		//We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).

		// swallow the message so it isn't forwarded but don't accept it
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}

	//If this node has any service providers (has already received some SelectComplete messages)
	//we forward these to the requesting client. The service providers forwarded here may include
	//all (selection has completed) or just a subset (selection still in progress) of this node's
	//possible service providers.
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
	RMetaServiceProviderInterface* serviceProvider = static_cast<RMetaServiceProviderInterface*>(iter++);
	TCFSelector::TSelectComplete msg(TNodeId::NullId(), TProviderInfo());
	while (serviceProvider)
		{
		msg.iNodeId = serviceProvider->RecipientId();
		msg.iProviderInfo = serviceProvider->ProviderInfo();
		iContext.PostToSender(msg); //We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).
		serviceProvider = static_cast<RMetaServiceProviderInterface*>(iter++);
		}

	//If this node has already completed selection, we send a final SelectComplete(NULL).
	if (iContext.Node().iIsSelectionCompleted)
		{
		iContext.PostToSender(TCFSelector::TSelectComplete(TNodeId::NullId(),TProviderInfo()).CRef());
		//We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}

	//Otherwise we perform the selection (start the activity or add the originator to the list).
	return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingSelectNextLayerSuper, NetStateMachine::MState, MCprStates::TContext)
EXPORT_C TBool TAwaitingSelectNextLayerSuper::Accept()
	{
 	if (!iContext.iMessage.IsMessage<TCFSelector::TSelect>())
		{
		return EFalse;
		}

	__ASSERT_DEBUG(iContext.iPeer || iContext.iSender == iContext.NodeId(), CoreMCprPanic(KPanicPeerMessage));

	const TLayerSelectionInfo* selectInfo = static_cast<const TLayerSelectionInfo*>(iContext.Node().AccessPointConfig().FindExtension(
			STypeId::CreateSTypeId(TLayerSelectionInfo::EUid, TLayerSelectionInfo::ETypeId)));
	__ASSERT_ALWAYS(selectInfo, CoreMCprPanic(KPanicUnexpectedExecutionPath));
	
	//If this node is on the bottom of the stack (no mcprs below to be selected) we reply with SelectComplete(NULL).
	if (selectInfo->SelectionPolicy() == 0 && selectInfo->CustomSelectionPolicy() == 0)
		{
		iContext.PostToSender(TCFSelector::TSelectComplete(TNodeId::NullId(), TProviderInfo()).CRef());
		//We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).

		// swallow the message so it isn't forwarded but don't accept it
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}

	//If this node has any service providers (has already received some SelectComplete messages)
	//we forward these to the requesting client. The service providers forwarded here may include
	//all (selection has completed) or just a subset (selection still in progress) of this node's
	//possible service providers.
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
	RMetaServiceProviderInterface* serviceProvider = static_cast<RMetaServiceProviderInterface*>(iter++);
	TCFSelector::TSelectComplete msg(TNodeId::NullId(), TProviderInfo());
	while (serviceProvider)
		{
		msg.iNodeId = serviceProvider->RecipientId();
		msg.iProviderInfo = serviceProvider->ProviderInfo();
		iContext.PostToSender(msg); //We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).
		serviceProvider = static_cast<RMetaServiceProviderInterface*>(iter++);
		}

	//If this node has already completed selection, we send a final SelectComplete(NULL).
	if (iContext.Node().iIsSelectionCompleted)
		{
		iContext.PostToSender(
			TCFSelector::TSelectComplete(
				TNodeId::NullId(),
				TProviderInfo()
				).CRef()
			);
		//We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}

	//Otherwise we perform the selection (start the activity or add the originator to the list).
	return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingTierManagerCreated, NetStateMachine::MState, MCprStates::TContext)
EXPORT_C TBool TAwaitingTierManagerCreated::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFFactory::TPeerFoundOrCreated>())
		{
	 	return ETrue;
	 	}
	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingSelectComplete, NetStateMachine::MState, MCprStates::TContext)
EXPORT_C TBool TAwaitingSelectComplete::Accept()
	{
	return iContext.iMessage.IsMessage<TCFSelector::TSelectComplete>();
	}

EXPORT_DEFINE_SMELEMENT(TPostStatusChangeToOriginators, NetStateMachine::MStateTransition, MCprStates::TContext)
EXPORT_C void TPostStatusChangeToOriginators::DoL()
	{
	CNodeActivityBase* act = iContext.iNodeActivity;
	__ASSERT_DEBUG(act, User::Panic(KSpecAssert_ESockCrPrvMCPRSC, 1));

	TCFControlProvider::TDataClientStatusChange* msg = message_cast<TCFControlProvider::TDataClientStatusChange>(&iContext.iMessage);
	if (msg)
		{
		act->PostToOriginators(TCFMcpr::TProviderStatusChange(msg->iValue == TCFControlProvider::TDataClientStatusChange::EStarted ? TCFMcpr::EStarted : TCFMcpr::EStopped).CRef());
		}
	else
		{
		act->PostToOriginators(iContext.iMessage);
		}
	}

EXPORT_DEFINE_SMELEMENT(TSendFinalSelectComplete, NetStateMachine::MStateTransition, MCprStates::TContext)
EXPORT_C void TSendFinalSelectComplete::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));

	//This is the final select complete, so selected mcpr must be NULL
//	ASSERT(message_cast<TCFSelector::TSelectComplete>(iContext.iMessage).iNodeId.IsNull());

	//There is no need to remember the channel (SetPostedTo()) because we do not expect any answer.
    iContext.iNodeActivity->PostToOriginators(TCFSelector::TSelectComplete(TNodeId::NullId(),TProviderInfo()).CRef());
    iContext.Node().iIsSelectionCompleted = ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TDecrementBlockingDestroy, NetStateMachine::MStateTransition, MCprStates::TContext)
EXPORT_C void TDecrementBlockingDestroy::DoL()
    {
	iContext.Node().DecrementBlockingDestroy();
    }

EXPORT_DEFINE_SMELEMENT(TProcessDataClientRejoin, NetStateMachine::MStateTransition, MCprStates::TContext)
EXPORT_C void TProcessDataClientRejoin::DoL()
	{
	TCFControlProvider::TJoinRequest& msg = message_cast<TCFControlProvider::TJoinRequest>(iContext.iMessage);
	RCFNodePriorityInterface* client = static_cast<RCFNodePriorityInterface*>(iContext.Node().FindClient(iContext.iSender));

	client->SetPriority(msg.iValue);
	RClientInterface::OpenPostMessageClose(iContext.NodeId(), iContext.iSender, TCFControlProvider::TJoinComplete().CRef());
	}


//
//Recovery

EXPORT_DEFINE_SMELEMENT(TAwaitingErrorRecoveryRequest, NetStateMachine::MState, MCprStates::TContext)
EXPORT_C TBool TAwaitingErrorRecoveryRequest::Accept()
	{
	return iContext.iMessage.IsMessage<TEErrorRecovery::TErrorRecoveryRequest>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingConnectionStartRecoveryRequest, NetStateMachine::MState, MCprStates::TContext)
EXPORT_C TBool TAwaitingConnectionStartRecoveryRequest::Accept()
	{
	TEErrorRecovery::TErrorRecoveryRequest* msg = message_cast<TEErrorRecovery::TErrorRecoveryRequest>(&iContext.iMessage);
	if (msg && msg->iErrContext.iMessageId == TCFServiceProvider::TStart::Id())
		{
		return ETrue;
		}
	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingConnectionGoneDownRecoveryRequest, NetStateMachine::MState, MCprStates::TContext)
EXPORT_C TBool TAwaitingConnectionGoneDownRecoveryRequest::Accept()
	{
	TEErrorRecovery::TErrorRecoveryRequest* msg = message_cast<TEErrorRecovery::TErrorRecoveryRequest>(&iContext.iMessage);
	if (msg)
		{
		TErrContext& errCtx = msg->iErrContext;
		if (errCtx.iActivitySigId == ECFActivityGoneDown)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TSendPropagateRecoveryResponse, NetStateMachine::MStateTransition, MCprStates::TContext)
void TSendPropagateRecoveryResponse::DoL()
	{
	TEErrorRecovery::TErrorRecoveryRequest& req = message_cast<TEErrorRecovery::TErrorRecoveryRequest>(iContext.iMessage);
	TErrResponse propagateResp(
			TErrResponse::EPropagate,
			req.iErrContext.iStateChange.iError,
			req.iErrContext.iMessageId
			);

#if defined(__GCCXML__)
	const Messages::TEErrorRecovery::TErrorRecoveryResponse message(propagateResp);
	TCFSafeMessage::TResponseCarrierWest<TEErrorRecovery::TErrorRecoveryResponse> resp(
			message,
			iContext.iSender
			);
#else
	TEErrorRecovery::TErrorRecoveryResponse msg(propagateResp);
	TCFSafeMessage::TResponseCarrierWest<TEErrorRecovery::TErrorRecoveryResponse> resp(
			msg,
			iContext.iSender
			);
#endif
	iContext.PostToSender(resp);
	}

//
//ReConnect
DEFINE_SMELEMENT(TAwaitingReConnectRequest, NetStateMachine::MState, MCprStates::TContext)
TBool TAwaitingReConnectRequest::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFMcpr::TReConnect>())
		{
		return ETrue;
		}
	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingReConnectCompleteOrError, NetStateMachine::MState, MCprStates::TContext)
TBool TAwaitingReConnectCompleteOrError::Accept()
	{
   	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
   	TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
   	if(msg)
    	{
    	iContext.iNodeActivity->SetError(msg->iValue);
   		return ETrue;
   		}
	return (iContext.iMessage.IsMessage<TCFMcpr::TReConnectComplete>())? ETrue : EFalse;
	}

//
//Other
EXPORT_DEFINE_SMELEMENT(TSelectNextLayer, NetStateMachine::MStateTransition, MCprStates::TContext)
void TSelectNextLayer::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, CoreMCprPanic(KPanicNoActivity));
	RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.NodeId(),
		TCFSelector::TSimpleSelect().CRef());
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingSelectNextLayerCompleted, NetStateMachine::MState, MCprStates::TContext)
EXPORT_C TBool TAwaitingSelectNextLayerCompleted::Accept()
	{
    if (iContext.iMessage.IsMessage<TCFSelector::TSelectComplete>())
    	{
		if (message_cast<TCFSelector::TSelectComplete>(iContext.iMessage).iNodeId.IsNull())
			{
        	return ETrue;
        	}
        //Consume any other TSelectComplete messages
        iContext.iMessage.ClearMessageId();
    	}
	return EFalse;
	}

DEFINE_SMELEMENT(TAwaitingAvailabilityRegistration, NetStateMachine::MState, MCprStates::TContext)
TBool TAwaitingAvailabilityRegistration::Accept()
	{
	return iContext.iMessage.IsMessage<TCFAvailabilityProvider::TAvailabilityNotificationRegistration>();
	}

DEFINE_SMELEMENT(TAwaitingAvailabilityNotification, NetStateMachine::MState, MCprStates::TContext)
TBool TAwaitingAvailabilityNotification::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFAvailabilityControlClient::TAvailabilityNotification>())
		{
		__ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrPrvMCPRSC, 2));
		__ASSERT_DEBUG(iContext.iPeer->Flags()&TCFClientType::EAvailabilityProvider, User::Panic(KSpecAssert_ESockCrPrvMCPRSC, 3));
		return ETrue;
		}
	return EFalse;
	}

//
// Connection Status Notification
EXPORT_DEFINE_SMELEMENT(TProcessProviderStatusChangeRegistration, NetStateMachine::MStateTransition, MCprStates::TContext)
EXPORT_C void TProcessProviderStatusChangeRegistration::DoL()
	{
	// send known value to this new originator..
	__ASSERT_DEBUG(iContext.iMessage.IsMessage<TCFMcpr::TProviderStatusChangeRegistration>(), User::Panic(KSpecAssert_ESockCrPrvMCPRSC, 4));

	// if any data clients are marked as started, connection is considered to be started...
	TBool connUp = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EStarted))
		? TCFMcpr::EStarted : TCFMcpr::EStopped ;

	// the immediate status report to the new originator
	iContext.PostToSender(TCFMcpr::TProviderStatusChange(connUp).CRef());
	}

//-=========================================================
//
//States
//
//-=========================================================
DEFINE_SMELEMENT(TAwaitingClientLeaveAndNoClients, NetStateMachine::MState, MCprStates::TContext)
TBool TAwaitingClientLeaveAndNoClients::Accept()
    {
    if (iContext.iMessage.IsMessage<TEPeer::TLeaveRequest>() || iContext.iMessage.IsMessage<TEChild::TLeft>())
		{
		TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl|TCFClientType::EData));
		__ASSERT_DEBUG(iter[0], User::Panic(KSpecAssert_ESockCrPrvMCPRSC, 5));
		if (iter[1]==NULL)
			{
			if (!iContext.Node().BlockingDestroy())
				{
				// provider info is nulled, so the factory wont match any this provider
				// for any new requests
				iContext.Node().SetProviderInfo(TProviderInfo::NullProviderInfo());

				return ETrue;
				}
			}
		}
    return EFalse;
    }

//-=========================================================
//
//State Forks
//
//-=========================================================
EXPORT_DEFINE_SMELEMENT(TSelectedProvider, NetStateMachine::MStateFork, MCprStates::TContext)
TInt TSelectedProvider::TransitionTag()
    {
    return MCprStates::KSelectedProvider;
    }

EXPORT_DEFINE_SMELEMENT(TSelectionIncomplete, NetStateMachine::MStateFork, MCprStates::TContext)
TInt TSelectionIncomplete::TransitionTag()
    {
    return MCprStates::KSelectionIncomplete;
    }

EXPORT_DEFINE_SMELEMENT(TProcessAvailability, NetStateMachine::MStateFork, MCprStates::TContext)
TInt TProcessAvailability::TransitionTag()
    {
    return MCprStates::KProcessAvailability;
    }

DEFINE_SMELEMENT(TDataClientStatusChangeOrNoTag, NetStateMachine::MStateFork, MCprStates::TContext)
TInt TDataClientStatusChangeOrNoTag::TransitionTag()
	{
	if (iContext.iMessage.IsMessage<TCFControlProvider::TDataClientStatusChange>())
		{
		return KDataClientStatusChange;
		}
	return KNoTag;
	}

DEFINE_SMELEMENT(TDataClientStatusChangeBackwardsOrNoTagBackwards, NetStateMachine::MStateFork, MCprStates::TContext)
TInt TDataClientStatusChangeBackwardsOrNoTagBackwards::TransitionTag()
	{
	return TDataClientStatusChangeOrNoTag(iContext).TransitionTag() | NetStateMachine::EBackward;
	}

DEFINE_SMELEMENT(TNoTagOrRejoinTag, NetStateMachine::MStateFork, MCprStates::TContext)
TInt TNoTagOrRejoinTag::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iMessage.IsMessage<TCFControlProvider::TJoinRequest>(), User::Panic(KSpecAssert_ESockCrPrvMCPRSC, 6));
	RCFNodePriorityInterface* client = static_cast<RCFNodePriorityInterface*>(iContext.Node().FindClient(iContext.iSender));
	if (client)
		{
		return KRejoinTag;
		}
	return KNoTag;
	}

//-=========================================================
//
//Transitions
//
//-=========================================================

