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
// Core PR States
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_UPSPRSTATES_H
#define SYMBIAN_UPSPRSTATES_H

// @TODO PREQ1116 - how many of these do we need ?

#include <comms-infras/ss_mmcommsprov.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <elements/mm_states.h>

namespace UpsStates
{
//
// UPS Support
//

// Support for User Prompt Service (TPolicyCheckRequest and TPolicyCheckResponse messages).
//
// @TODO 1116 Should these be in CoreStates rather than CoreNetStates?

typedef MeshMachine::TNodeContext<ESock::ACFMMNodeIdBase> TContext;

//
// States
//

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingPolicyCheckRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingPolicyCheckRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingPolicyCheckResponse, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( AwaitingPolicyCheckResponse )

//
// Forks
//
const TInt KUpsControlClientOriginator    = 10014;
const TInt KUpsDataClientClientOriginator = 10015;
EXPORT_DECLARE_SMELEMENT_HEADER( TControlOrDataClientOriginator, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TControlOrDataClientOriginator )
//
// Transitions
//

EXPORT_DECLARE_SMELEMENT_HEADER( TPostToControlProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostToControlProvider )

EXPORT_DECLARE_SMELEMENT_HEADER( TPostToServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostToServiceProvider )

} //namespace UpsPrStates

#endif //SYMBIAN_UPSPRSTATES_H

