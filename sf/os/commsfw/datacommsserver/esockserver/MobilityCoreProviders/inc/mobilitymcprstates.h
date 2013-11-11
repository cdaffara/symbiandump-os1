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
// Mobility Meta Connection Provider States
//
//

/**
 @file
 @internalComponent
*/


#ifndef MOBILITYMCPRSTATES_H
#define MOBILITYMCPRSTATES_H

#include <elements/mm_states.h>
#include <comms-infras/mobilitymcpr.h>
#include <comms-infras/coremcprstates.h>

namespace MobilityMCprStates
{

typedef MeshMachine::TNodeContext<CMobilityMetaConnectionProvider, MCprStates::TContext> TContext;

//-=========================================================
//
//Core Mobile Meta Connection Provider Transition Ids 10000..11000
//
//-=========================================================

const TInt KStartMobilityHandshake	= 10000;
const TInt KAwaitMobility        	= 10001;
const TInt KRejected                = 10002;
const TInt KReConnect               = 10003;

//-=========================================================
//
//Mutexes
//
//-=========================================================

//This mutex blocks when the mobility is handshaking on the node
class THandshakingMobilityMutex
	{
public:
	static TBool IsBlocked(MeshMachine::TNodeContextBase& aContext);
	};

//-=========================================================
//
//States
//
//-=========================================================

DECLARE_SMELEMENT_HEADER(TAwaitingStartMobility, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingStartMobility)

/**
	Mobility base state implementing generic cancellation behaviour.
*/
class TStateBase : public MeshMachine::TState<TContext>
	{
public:
	explicit TStateBase(TContext& aContext) : MeshMachine::TState<TContext>(aContext)
		{
		}
	virtual void Cancel();
	};

DECLARE_SMELEMENT_HEADER(TAwaitingMigrationRequestedOrRejected, MobilityMCprStates::TStateBase, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingMigrationRequestedOrRejected)

DECLARE_SMELEMENT_HEADER(TAwaitingMigrationAcceptedOrRejected, MobilityMCprStates::TStateBase, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingMigrationAcceptedOrRejected)

//-=========================================================
//
//State Forks
//
//-=========================================================
DECLARE_SMELEMENT_HEADER(TNoTagOrErrorTagIfMobilityRunning, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
    virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER(TNoTagOrErrorTagIfMobilityRunning)

DECLARE_SERIALIZABLE_STATE(
   TNoTagBlockedByMobilityHandshaking,
   THandshakingMobilityMutex,
   MeshMachine::TNoTag
   )

//-=========================================================
//
//Transitions
//
//-=========================================================

DECLARE_SMELEMENT_HEADER(TReplyMobilityStarted, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TReplyMobilityStarted)

} //namespace MobilityMCprStates

#endif // MOBILITYMCPRSTATES_H

