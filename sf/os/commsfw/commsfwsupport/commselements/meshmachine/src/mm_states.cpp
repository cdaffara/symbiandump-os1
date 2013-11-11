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

/**
 @file
*/

#include "mm_states.h"
#include "mm_activities.h"
#include <elements/nm_messages_base.h>
#include <elements/nm_messages_peer.h>
#include <elements/nm_messages_child.h>
#include <elements/nm_messages_errorrecovery.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemMeshMachStaC, "ElemMeshMachStaC");
#endif

using namespace MeshMachine;
using namespace Messages;

//-=========================================================
//
//Panics
//
//-=========================================================
#ifdef _DEBUG
	_LIT (KMMStatePanic,"MMStatePanic");
#endif

enum
	{
	EPanicNoPeerPresent = 1,
	KPanicNoActivity = 2
	};

//-=========================================================
//
//Commonly used states
//
//-=========================================================

EXPORT_C TBool AAcceptErrorState::Accept(TNodeContextBase& aContext, TBool aSuperAccept)
	{
	TEBase::TError* msg = message_cast<TEBase::TError>(&aContext.iMessage);
	if (msg && aSuperAccept == EFalse && aContext.iNodeActivity != NULL)
    	{
    	aContext.iNodeActivity->SetError(msg->iValue);
		return ETrue;
		}
	return aSuperAccept;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingNull, NetStateMachine::MState, TContext)
EXPORT_C TBool TAwaitingNull::Accept()
	{
	return iContext.iMessage.IsMessage<TEBase::TNull>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingAny, NetStateMachine::MState, TContext)
EXPORT_C TBool TAwaitingAny::Accept()
	{
	return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingClientLeavingRequest, NetStateMachine::MState, TContext)
EXPORT_C TBool TAwaitingClientLeavingRequest::Accept()
	{
	if (!iContext.iMessage.IsMessage<TEPeer::TLeaveRequest>())
    	{
    	return EFalse;
    	}

	//TClientLeavingRequest is a "peer" message.
	//You must not allow your "noPeerIds" table to include it.
	__ASSERT_DEBUG(iContext.Node().FindClient(iContext.iSender), User::Panic(KMMStatePanic, EPanicNoPeerPresent));
	return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingLeaveComplete, NetStateMachine::MState, TContext)
EXPORT_C TBool TAwaitingLeaveComplete::Accept()
	{
	//Requests that have been posted and require this response (i.e. TDestroy
	//or TLeaveRequest) are never cancellable and must complete.
	//The activity must however be informed about the cancellation and take
	//proper steps (if required) after the TLeaveComplete has been received.
	if (iContext.iMessage.IsMessage<TEBase::TCancel>())
		{
		//For "single triple" activities with no activity object we will never receive TCancel
		//because of "one shot" completion.
		__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KMMStatePanic, KPanicNoActivity));
		iContext.iNodeActivity->SetError(KErrCancel);
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}

	if (!iContext.iMessage.IsMessage<TEPeer::TLeaveComplete>())
    	{
    	return EFalse;
    	}

	//This is TLeaveComplete which is always being awaited for in a running activity.
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KMMStatePanic, KPanicNoActivity));
	TInt error = iContext.iNodeActivity->Error();

	//The activity could be operating in the error mode that as resulted from
	//unsuccessful addition of the peer to the iClients array.
	__ASSERT_DEBUG(iContext.iPeer || error, User::Panic(KMMStatePanic, EPanicNoPeerPresent));

	//If this assert fires - check your implementations - they are not safe.
	//You _must_ mark client as leaving after sending it the leaving request.
	__ASSERT_DEBUG(error || iContext.iPeer->Flags() & TClientType::ELeaving, User::Panic(KMMStatePanic, EPanicNoPeerPresent));

	if (iContext.iPeer)
		{
		iContext.Node().RemoveClient(iContext.iSender, iContext);
		}
    return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDestroy, NetStateMachine::MState, TContext)
EXPORT_C TBool TAwaitingDestroy::Accept()
	{
	return iContext.iMessage.IsMessage<TEChild::TDestroy>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingCancel, NetStateMachine::MState, TContext)
EXPORT_C TBool TAwaitingCancel::Accept()
	{
	return iContext.iMessage.IsMessage<TEBase::TCancel>();
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingErrorRecoveryResponseOrError, NetStateMachine::MState, TContext)
EXPORT_C TBool TAwaitingErrorRecoveryResponseOrError::Accept()
    {
    //Awaiting TErrorRecoveryResponse or TError in case there was no error recovery activity
    //on the MCpr willing to accept our TErrorRecoveryRequest.
    //In case of TError it is crutial to check if the message is adressed to our activity.
    //There could be any other TError message coming to the node since we sent TErrorRecoveryRequest.
    //And because of TErrorRecoveryRequest's forwarding, the SetSentTo is set to NULL, so this state
    //will be presented with all of the potential TError messages.
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KMMStatePanic, KPanicNoActivity));
	TBool match = iContext.iMessage.IsMessage<TEErrorRecovery::TErrorRecoveryResponse>();
	if (!match
		&& iContext.iMessage.IsMessage<TEBase::TError>()
		&& address_cast<const TNodeCtxId>(&iContext.iRecipient))
		{
		match = address_cast<const TNodeCtxId>(iContext.iRecipient).NodeCtx()==iContext.ActivityId();
		}
	return match;
    }

//-=========================================================
//
//Commonly used state forks
//
//-=========================================================

EXPORT_C TInt AErrorTagOr::TransitionTag(TNodeContextBase& aContext)
 	{
 	if (aContext.iNodeActivity->Error()!=KErrNone)
 		{
 		return KErrorTag;
 		}
 	TEBase::TError* msg = message_cast<TEBase::TError>(&aContext.iMessage);
    if (msg)
        {
        aContext.iNodeActivity->SetError(msg->iValue);
        return KErrorTag;
        }
 	return 0;
 	}

EXPORT_DEFINE_SMELEMENT(TNoTag, NetStateMachine::MStateFork, TContext)

EXPORT_DEFINE_SMELEMENT(TErrorTag, NetStateMachine::MStateFork, TContext)
EXPORT_C TInt TErrorTag::TransitionTag()
	{
	return KErrorTag | NetStateMachine::EForward;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagBackward, NetStateMachine::MStateFork, TContext)
EXPORT_C TInt TNoTagBackward::TransitionTag()
	{
	return KNoTag | NetStateMachine::EBackward;
	}

EXPORT_DEFINE_SMELEMENT(TNoTagOrErrorTag, NetStateMachine::MStateFork, TContext)
EXPORT_C TInt TNoTagOrErrorTag::TransitionTag()
    {
	TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
	if (KErrNone==iContext.iNodeActivity->Error() && NULL==msg)
		{
		return KNoTag | NetStateMachine::EForward;
		}
	if (msg)
		{
		iContext.iNodeActivity->SetError(msg->iValue);
		}
	return KErrorTag | NetStateMachine::EForward;
    }

//-=========================================================
//
// Commonly used transitions
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(TDoNothing, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TDoNothing::DoL()
	{
	}

EXPORT_DEFINE_SMELEMENT(TSetIdle, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TSetIdle::DoL()
	{
	iContext.iNodeActivity->SetIdle();
	}

EXPORT_DEFINE_SMELEMENT(TRemoveClient, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TRemoveClient::DoL()
	{
	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KMMStatePanic, EPanicNoPeerPresent)); //Must be present now.
	iContext.Node().RemoveClient(iContext.iSender, iContext);
	}

EXPORT_DEFINE_SMELEMENT(TRaiseActivityError, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TRaiseActivityError::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KMMStatePanic, KPanicNoActivity));
	TInt err = iContext.iNodeActivity->Error();
	__ASSERT_DEBUG(err != KErrNone, User::Panic(KSpecAssert_ElemMeshMachStaC, 1));
	User::Leave(err);
	}

EXPORT_DEFINE_SMELEMENT(TRaiseAndClearActivityError, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TRaiseAndClearActivityError::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KMMStatePanic, KPanicNoActivity));
	TInt err = iContext.iNodeActivity->Error();
	__ASSERT_DEBUG(err != KErrNone, User::Panic(KSpecAssert_ElemMeshMachStaC, 2));
	iContext.iNodeActivity->SetError(KErrNone);
	User::Leave(err);
	}

EXPORT_DEFINE_SMELEMENT(TStoreError, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TStoreError::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ElemMeshMachStaC, 3));
	__ASSERT_DEBUG(iContext.iMessage.IsMessage<TEBase::TError>(), User::Panic(KSpecAssert_ElemMeshMachStaC, 4));
	TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
	iContext.iNodeActivity->SetError(msg->iValue);
	}

EXPORT_DEFINE_SMELEMENT(TClearError, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TClearError::DoL()
	{
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KMMStatePanic, KPanicNoActivity));
	iContext.iNodeActivity->SetError(KErrNone);
	}

EXPORT_DEFINE_SMELEMENT(TForwardMessageToOriginators, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TForwardMessageToOriginators::DoL()
	{
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KMMStatePanic, KPanicNoActivity));
    iContext.iNodeActivity->PostToOriginators(iContext.iMessage);
	}

