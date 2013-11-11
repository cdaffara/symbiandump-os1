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
// CORECPRSTATES.CPP
// 
//

#include "dummystates.h"
#include "dummynode.h"
#include "dummymessages.h"
#include "dummyruntimectxaddressbook.h"
#include <elements/mm_activities.h>

using namespace Dummy;
using namespace DummyStates;
using namespace Messages;

//
//Panics
#ifdef _DEBUG
_LIT (KDummyNodePanic,"DummyNodePanic");
static const TInt KPanicNoPeerMessage = 1;
static const TInt KPanicNoActivity = 2;
static const TInt KPanicUnexpectedServiceProviderState = 3;
#endif

/*
IMPORTANT NOTES ABOUT ASSERTING WITHIN STATES & TRANSITIONS

When asserting structural correctness of your activity you must
use __ASSERT_DEBUG.
When asserting dynamic or algorithmic correctness you would normally
use __ASSERT_DEBUG + raise an error for UREL.
__ASSERT_ALWAYS is reserved purely for non-structural unexpected
conditions which should result in the whole thread being taken down.

For example, for asserting that the transition or state are not used
as in the activity's first tripple, you must use:
__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyNodePanic, KPanicNoActivity));

For asserting that there is only one ServiceProvider (etc) you must use:
__ASSERT_DEBUG(1 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(EServProvider)),

For asserting there is only one started ServiceProvider (or any other) you must use:
__ASSERT_DEBUG(1 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(EServProvider, EStarted)),

For asserting that you do have a particular peer from a transition you should use:
__ASSERT_DEBUG(peer, User::Panic(KDummyNodePanic, KPanicNoPeerMessage)); //UDEB
User::LeaveIfError(peer? KErrNone : KErrCorrupt); //UREL
*/

//
//States
DEFINE_SMELEMENT(TAwaitingStop, NetStateMachine::MState, DummyStates::TContext)
TBool TAwaitingStop::Accept()
	{
 	if (!iContext.iMessage.IsMessage<TStopProtocol::TStop>())
 		{
 		return EFalse;
 		}

 	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KDummyNodePanic, KPanicNoPeerMessage));
 	//The client must have started us in the first place!!
 	__ASSERT_DEBUG(iContext.iPeer->Flags() & TDummyClientType::EActive, User::Panic(KDummyNodePanic, KPanicNoPeerMessage));
	if (1 < iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EClient, TDummyClientType::EActive)))
		{
		//If there are more Clients that have started us - we do not commence the full stop sequence
		RClientInterface::OpenPostMessageClose(iContext.NodeId(), iContext.iSender, TStopProtocol::TStopped().CRef());
		iContext.iPeer->ClearFlags(TDummyClientType::EActive);
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}

	//We only commence the full stop sequence if this is the only Client which has started us
	//EActive will be cleared when we post TStopped.
 	return ETrue;
	}

DEFINE_SMELEMENT(TAwaitingStarted, NetStateMachine::MState, DummyStates::TContext)
TBool TAwaitingStarted::Accept()
	{
	//If this is an error message addressed to our activity, clear the EStarting flag
	//Do not accept the error as we rely on the default error handling
	if (iContext.iMessage.IsMessage<TEBase::TError>())
		{
		__ASSERT_DEBUG(iContext.iPeer, User::Panic(KDummyNodePanic, KPanicNoPeerMessage));
		iContext.iPeer->ClearFlags(TDummyClientType::EStarting);
		return EFalse;
		}

	if (iContext.iMessage.IsMessage<TStartProtocol::TStarted>())
 		{
 		__ASSERT_DEBUG(iContext.iPeer, User::Panic(KDummyNodePanic, KPanicNoPeerMessage));
		iContext.iPeer->ClearFlags(TDummyClientType::EStarting);
		iContext.iPeer->SetFlags(TDummyClientType::EStarted);
 		return ETrue;
 		}

 	return EFalse;
	}

DEFINE_SMELEMENT(TAwaitingStopped, NetStateMachine::MState, DummyStates::TContext)
TBool TAwaitingStopped::Accept()
	{
 	if (iContext.iMessage.IsMessage<TStopProtocol::TStopped>())
 		{
 		__ASSERT_DEBUG(iContext.iPeer, User::Panic(KDummyNodePanic, KPanicNoPeerMessage));
		iContext.iPeer->ClearFlags(TDummyClientType::EStopping);
		iContext.iPeer->ClearFlags(TDummyClientType::EStarted);
 		return ETrue;
 		}
 	return EFalse;
	}

DEFINE_SMELEMENT(TAwaitingDestroy, NetStateMachine::MState, DummyStates::TContext)
TBool TAwaitingDestroy::Accept()
	{
 	if (iContext.iMessage.IsMessage<TEChild::TDestroy>())
 		{
 		__ASSERT_DEBUG(iContext.iPeer->RecipientId() == iContext.NodeId(), User::Panic(KDummyNodePanic, KPanicNoPeerMessage));
 		if (!iContext.Node().IsJoinOutstanding()
 			&& 0 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EClient)))
 			{
			//Accept if there are no outstanding join requests + there are no other Clients
			return ETrue;
			}
		//Ignore the message because since we have sent ourselves TDestroy,
		//the Factory has given away one or more handle(s) to this node.
		//The last Client (one that as the last handle) will destruct it.
		iContext.iMessage.ClearMessageId();
 		}
 	return EFalse;
	}

//
//State Forks
DEFINE_SMELEMENT(TNoTagOrStartedOrNoServiceProvider, NetStateMachine::MStateFork, DummyStates::TContext)
TInt TNoTagOrStartedOrNoServiceProvider::TransitionTag()
	{
	RNodeInterface* sp = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EServProvider));
	if (sp==NULL)
		{
		return KNoServiceProvider;
		}
	if (sp->Flags() & TDummyClientType::EStarted)
		{
		return KStarted;
		}
	return MeshMachine::KNoTag;
	}

DEFINE_SMELEMENT(TNoTagOrNoServiceProvider, NetStateMachine::MStateFork, DummyStates::TContext)
TInt TNoTagOrNoServiceProvider::TransitionTag()
	{
	if (0 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EServProvider)))
		{
		return KNoServiceProvider;
		}
	return MeshMachine::KNoTag;
	}

DEFINE_SMELEMENT(TNoTagBackwardsOrNoServiceProvider, NetStateMachine::MStateFork, DummyStates::TContext)
TInt TNoTagBackwardsOrNoServiceProvider::TransitionTag()
	{
	if (0 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EServProvider)))
		{
		return KNoServiceProvider;
		}
	return MeshMachine::KNoTag | NetStateMachine::EBackward;
	}

//
//Transitions
DEFINE_SMELEMENT(TPostStart, NetStateMachine::MStateTransition, DummyStates::TContext)
void TPostStart::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyNodePanic, KPanicNoActivity));
	RNodeInterface* sp = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EServProvider));
	__ASSERT_DEBUG(sp, User::Panic(KDummyNodePanic, KPanicNoPeerMessage));
	User::LeaveIfError(sp? KErrNone : KErrCorrupt); //Release mode
 	iContext.iNodeActivity->PostRequestTo(*sp, TStartProtocol::TStart(23).CRef()); //Takes a dummy param for demo purposes only
 	__ASSERT_DEBUG(!(sp->Flags() & TDummyClientType::EStarted), User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
 	sp->SetFlags(TDummyClientType::EStarting);
	}

DEFINE_SMELEMENT(TPostStop, NetStateMachine::MStateTransition, DummyStates::TContext)
void TPostStop::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyNodePanic, KPanicNoActivity));
	RNodeInterface* sp = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EServProvider, TDummyClientType::EStarted));
	__ASSERT_DEBUG(sp, User::Panic(KDummyNodePanic, KPanicNoPeerMessage));
	User::LeaveIfError(sp? KErrNone : KErrCorrupt); //Release mode
 	iContext.iNodeActivity->PostRequestTo(*sp, TStopProtocol::TStop().CRef());
 	__ASSERT_DEBUG(!(sp->Flags() & TDummyClientType::EStarting), User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
 	__ASSERT_DEBUG(sp->Flags() & TDummyClientType::EStarted, User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
 	sp->SetFlags(TDummyClientType::EStopping);
	}

DEFINE_SMELEMENT(TCreateServiceProvider, NetStateMachine::MStateTransition, DummyStates::TContext)
void TCreateServiceProvider::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyNodePanic, KPanicNoActivity));
	__ASSERT_DEBUG(0 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EServProvider)),
		User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
	iContext.iNodeActivity->PostRequestTo(TRuntimeCtxAddressBook::ObjectBroker(), TBindProtocol::TFindOrCreateFactoryObject(iContext.Node().ServiceProviderUid()).CRef());
	}

DEFINE_SMELEMENT(TAddClient, NetStateMachine::MStateTransition, DummyStates::TContext)
void TAddClient::DoL()
	{
	//User::Leave(KErrNoMemory); //Uncomment this line if you want to see how the cleanup works
	__ASSERT_DEBUG(iContext.iMessage.IsMessage<TBindProtocol::TClientJoiningRequest>(), User::Panic(KDummyNodePanic, KPanicNoActivity));
	TBindProtocol::TClientJoiningRequest& msg = message_cast<TBindProtocol::TClientJoiningRequest>(iContext.iMessage);
	iContext.Node().AddClientL(address_cast<TNodeId>(iContext.iSender), TClientType(TDummyClientType::EClient));
	}

void TAddClient::Error(TInt aError)
	{
	//This node has not been able to add the Client to its peers' array (AddClientL left with aError).
	//If there are no other Clients or oustanding joins pending we will attempt destruction of this node.

	//Please note that the Factory could give away another reference to this node or another Client
	//could join successfully while TDestroy was in transport.
	//Therefore TAwaitingDestroy will only accept the message when there are no Clients and when
	//iContext.Node().IsJoinOutstanding() is EFalse.

	//Call the base class's implementation - TError will be sent to all originators when this activity gets destructed
	MeshMachine::TStateTransition<TContext>::Error(aError);
	if (0 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EClient))
		&& !iContext.Node().IsJoinOutstanding())
		{
		iContext.Node().SelfInterface().PostMessage(iContext.NodeId(), TEChild::TDestroy().CRef());
		}
	}

DEFINE_SMELEMENT(TAddServiceProvider, NetStateMachine::MStateTransition, DummyStates::TContext)
void TAddServiceProvider::DoL()
	{
	//User::Leave(KErrNoMemory); //Uncomment this line if you want to see how the cleanup works
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyNodePanic, KPanicNoActivity));
	__ASSERT_DEBUG(0 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EServProvider)),
		User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
	__ASSERT_DEBUG(iContext.iMessage.IsMessage<TBindProtocol::TJoinComplete>(), User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
	iContext.Node().AddClientL(address_cast<TNodeId>(iContext.iSender), TClientType(TDummyClientType::EServProvider));
	}

void TAddServiceProvider::Error(TInt aError)
	{
	//The Service Provider has successfully joined this node but this node
	//has not been able to add it to the peers' array (AddClientL left with aError).
	//We need to free the Service Provider by sending it TClientLeavingRequest.

	//Call the base class's implementation - TError will be sent to all originators when this activity gets destructed
	MeshMachine::TStateTransition<TContext>::Error(aError);
	__ASSERT_DEBUG(iContext.iMessage.IsMessage<TBindProtocol::TJoinComplete>(), User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
	RClientInterface::OpenPostMessageClose(iContext.NodeId(), iContext.iSender, TEPeer::TLeaveRequest().CRef());
	}

DEFINE_SMELEMENT(TPostClientJoiningRequest, NetStateMachine::MStateTransition, DummyStates::TContext)
void TPostClientJoiningRequest::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyNodePanic, KPanicNoActivity));
	__ASSERT_DEBUG(0 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EServProvider)),
		User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
	TBindProtocol::TFactoryObjectCreated& msg = message_cast<TBindProtocol::TFactoryObjectCreated>(iContext.iMessage);
	__ASSERT_DEBUG(!msg.iNodeId.IsNull(), User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
	iContext.iNodeActivity->PostRequestTo(msg.iNodeId, TBindProtocol::TClientJoiningRequest().CRef());
	}

DEFINE_SMELEMENT(TCommenceJoin, NetStateMachine::MStateTransition, DummyStates::TContext)
void TCommenceJoin::DoL()
	{
	iContext.Node().CommenceJoin(); //Decrement the outstanding join count
	}

DEFINE_SMELEMENT(TPostClientLeavingRequest, NetStateMachine::MStateTransition, DummyStates::TContext)
void TPostClientLeavingRequest::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyNodePanic, KPanicNoActivity));
	RNodeInterface* sp = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EServProvider));
	__ASSERT_DEBUG(sp, User::Panic(KDummyNodePanic, KPanicUnexpectedServiceProviderState));
	iContext.iNodeActivity->PostRequestTo(*sp, TEPeer::TLeaveRequest().CRef());
	sp->SetFlags(TDummyClientType::ELeaving);
	}

DEFINE_SMELEMENT(TPostDestroyToSelfIfLastClient, NetStateMachine::MStateTransition, DummyStates::TContext)
void TPostDestroyToSelfIfLastClient::DoL()
	{
	//Send self destroy only when there are no more Clients and no more outstanding
	if (0 == iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TDummyClientType::EClient))
		&& !iContext.Node().IsJoinOutstanding())
		{
		iContext.Node().SelfInterface().PostMessage(iContext.NodeId(), TEChild::TDestroy().CRef());
		}
	}

