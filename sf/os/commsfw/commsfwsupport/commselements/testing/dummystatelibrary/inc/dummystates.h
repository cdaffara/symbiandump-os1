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
//

#ifndef DUMMYSTATES_H
#define DUMMYSTATES_H

#include "dummynode.h"
#include "dummymessages.h"
#include <elements/mm_context.h>
#include <elements/mm_states.h>
#include <elements/mm_mutexpolicies.h>
#include <elements/nm_messages_peer.h>
#include <elements/nm_messages_child.h>

/*
IMPORTANT NOTES ABOUT STATES, FORKS & TRANSITIONS

1)
Please make all of your transitions that you place
in a state libraries atomic.
If you couple two or more transitions into one, you
must have a good reason for that and you should not
put them into the state library.
Coupled transitions usually reside with the specialised
activities using them.
We have given some examplea of a coupled transition below.
The usual reasons for using aggregated transitions are the
same also for conditional transitions, please see below.

2)
Conditional transitions are only justified when they contribute
to a significant complexity decrease resulting from use of so called
"single triple" activities.
A single triple activity that needs conditional routing can not
use forks since it does not have the activity object and must finish
in "one shot".
Conditional transitions are not acceptable in any other scenarios
since in every other scenario a fork should be used for activity routing.
*/


namespace DummyStates
{

typedef MeshMachine::TNodeContext<Dummy::CTestNodeBase, MeshMachine::TContext> TContext;

//-=========================================================
//
//Dummy transition ids 0..1000
//
//-=========================================================
const TUint KNoPeer            = 1;
const TUint KStarting          = 2;
const TUint KStopping          = 3;
const TUint KStarted           = 4;
const TUint KNoServiceProvider = 5;

//-=========================================================
//
//States
//
//-=========================================================
typedef MeshMachine::TAwaitingMessageState<Dummy::TStartProtocol::TStart> TAwaitingStart;
typedef MeshMachine::TAwaitingMessageState<Dummy::TBindProtocol::TClientJoiningRequest> TAwaitingClientJoiningRequest;
typedef MeshMachine::TAwaitingMessageState<Dummy::TBindProtocol::TJoinComplete> TAwaitingJoinComplete;
typedef MeshMachine::TAwaitingMessageState<Dummy::TBindProtocol::TFactoryObjectCreated> TAwaitingFactoryObjectCreated;

DECLARE_SMELEMENT_HEADER( TAwaitingStop, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingStop )

DECLARE_SMELEMENT_HEADER( TAwaitingStarted, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingStart )

DECLARE_SMELEMENT_HEADER( TAwaitingStopped, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingStopped )

DECLARE_SMELEMENT_HEADER( TAwaitingDestroy, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDestroy )

//-=========================================================
//
//Stateforks
//
//-=========================================================
DECLARE_SMELEMENT_HEADER( TNoTagOrStartedOrNoServiceProvider, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrStartedOrNoServiceProvider )

DECLARE_SMELEMENT_HEADER( TNoTagOrNoServiceProvider, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrNoServiceProvider )

DECLARE_SMELEMENT_HEADER( TNoTagBackwardsOrNoServiceProvider, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagBackwardsOrNoServiceProvider )

//We use serialised states (and not serialised transitions) as the
//subsequent activity flow must be decided only after the activity
//is woken up again.
DECLARE_SERIALIZABLE_STATE(
	TNoTagOrStartedOrNoServiceProviderBlockedByStart,
	MeshMachine::TActivityIdMutex<Dummy::EActivityStart>,
	DummyStates::TNoTagOrStartedOrNoServiceProvider
	)

DECLARE_SERIALIZABLE_STATE(
	TNoTagOrStartedOrNoServiceProviderBlockedByStop,
	MeshMachine::TActivityIdMutex<Dummy::EActivityStop>,
	DummyStates::TNoTagOrStartedOrNoServiceProvider
	)

//-=========================================================
//
//State Transitions
//
//-=========================================================

//Send Started setting the EActive flag on the Client(s)
typedef MeshMachine::TPostMessageToOriginators<Dummy::TStartProtocol::TStarted, Dummy::TDummyClientType::EActive> TPostStarted;
//Send Stopped clearing the EActive flag on the Client(s)
typedef MeshMachine::TPostMessageToOriginators<Dummy::TStopProtocol::TStopped, 0, Dummy::TDummyClientType::EActive> TPostStopped;

DECLARE_SMELEMENT_HEADER( TPostStart, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostStart )

DECLARE_SMELEMENT_HEADER( TPostStop, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostStop )

DECLARE_SMELEMENT_HEADER( TCreateServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCreateServiceProvider )

DECLARE_SMELEMENT_HEADER( TAddClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
	virtual void Error(TInt aError); //This transition will post TDestroy on error
DECLARE_SMELEMENT_FOOTER( TAddClient )

DECLARE_SMELEMENT_HEADER( TAddServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
	virtual void Error(TInt aError); //This transition will post TDestroy on error
DECLARE_SMELEMENT_FOOTER( TAddServiceProvider )

DECLARE_SMELEMENT_HEADER( TPostClientJoiningRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostClientJoiningRequest )

DECLARE_SMELEMENT_HEADER( TPostClientLeavingRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostClientLeavingRequest )

DECLARE_SMELEMENT_HEADER( TCommenceJoin, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCommenceJoin )

DECLARE_SMELEMENT_HEADER( TPostDestroyToSelfIfLastClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostDestroyToSelfIfLastClient )

//Aggregated transitions are used to combine 2 or more separate transitions into one.
DECLARE_AGGREGATED_TRANSITION3(
	TCommenceJoinAndAddClientAndPostJoinComplete,
	DummyStates::TCommenceJoin,
	DummyStates::TAddClient,
	MeshMachine::TPostMessageToSender<Dummy::TBindProtocol::TJoinComplete>
	)

DECLARE_AGGREGATED_TRANSITION3(
	TPostLeaveCompleteAndRemoveClientAndPostDestroyIfLast,
	MeshMachine::TPostMessageToSender<Messages::TEPeer::TLeaveComplete>,
	MeshMachine::TRemoveClient,
	DummyStates::TPostDestroyToSelfIfLastClient
	)

DECLARE_AGGREGATED_TRANSITION2(
	TAddServiceProviderAndPostStart,
	DummyStates::TAddServiceProvider,
	DummyStates::TPostStart
	)

} //namespace DummyStates


#endif
//DUMMYSTATES_H


