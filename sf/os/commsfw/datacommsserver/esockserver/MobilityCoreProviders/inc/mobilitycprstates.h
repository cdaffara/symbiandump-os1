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
// mobilitystates.h
// Mobility Connection Provider state declarations.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef MOBILITYCPR_STATES_H
#define MOBILITYCPR_STATES_H

#include <elements/mm_states.h>
#include <comms-infras/ss_activities.h>
#include <comms-infras/corecpr.h>
#include <comms-infras/corecprstates.h>
#include <comms-infras/ss_nodemessages_mobility.h>

class CMobilityConnectionProvider;

namespace MobilityCprStates
{
typedef MeshMachine::TNodeContext<CMobilityConnectionProvider, CprStates::TContext> TContext;

/*******************
 Mobility states
********************/

/**
	Mobility base state implementing generic cancellation behaviour.
*/
class TStateBase : public MeshMachine::TState<TContext>
	{
public:
	explicit TStateBase(TContext& aContext) : MeshMachine::TState<TContext>(aContext)
		{
		}

	// From TState
	virtual void Cancel();
	};

typedef MeshMachine::TAwaitingMessageState<ESock::TCFMobilityProvider::TStartMobility> TAwaitingStartMobility;
typedef MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingStarted> TAwaitingStartedOrError;

DECLARE_SMELEMENT_HEADER(TAwaitingMobilityStartedOrError, MobilityCprStates::TStateBase, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingMobilityStartedOrError)

DECLARE_SMELEMENT_HEADER(TAwaitingNewCarrierOrRelayAndConsumeCurrentCarrierRejected, MobilityCprStates::TStateBase, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingNewCarrierOrRelayAndConsumeCurrentCarrierRejected)
typedef MeshMachine::TAcceptErrorState<TAwaitingNewCarrierOrRelayAndConsumeCurrentCarrierRejected> TAwaitingNewCarrierOrErrorOrRelayAndConsumeCurrentCarrierRejected;

DECLARE_SMELEMENT_HEADER(TAwaitingMigrationCompleteOrNewCarrierOrError, MobilityCprStates::TStateBase, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingMigrationCompleteOrNewCarrierOrError)

DECLARE_SMELEMENT_HEADER(TAwaitAndRelayMigrationRequestedOrMigrationRejected, MobilityCprStates::TStateBase, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitAndRelayMigrationRequestedOrMigrationRejected)

DECLARE_SMELEMENT_HEADER(TAwaitAndRelayMigrationAcceptedOrMigrationRejected, MobilityCprStates::TStateBase, NetStateMachine::MState, TContext)
 	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitAndRelayMigrationAcceptedOrMigrationRejected)

/*******************
 Mobility transitions
********************/

DECLARE_SMELEMENT_HEADER(TSendStartMobility, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TSendStartMobility)

DECLARE_SMELEMENT_HEADER(TResetPostedTo, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TResetPostedTo)

DECLARE_SMELEMENT_HEADER(TSendErrorRecoveryReq, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TSendErrorRecoveryReq)

DECLARE_SMELEMENT_HEADER(TSendMigrationRejected, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TSendMigrationRejected)

/*******************
 Mobility forks
********************/
DECLARE_SMELEMENT_HEADER(TNoTagOrStartHandshakeBackwardsOrError, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
    virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER(TNoTagOrStartHandshakeBackwardsOrError)

} // MobilityCprStates

#endif
// MOBILITYCPR_STATES_H

