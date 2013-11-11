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
// Core SCPR States
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_CORESCPRSTATES_H
#define SYMBIAN_CORESCPRSTATES_H

#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/corescpr.h>

namespace SCprStates
{

typedef MeshMachine::TNodeContext<CCoreSubConnectionProvider, PRStates::TContext> TContext;

//-=========================================================
//
//Core Sub Connection Provider Transition Ids 6000..10000
//
//-=========================================================
const TInt KParamsPresent				= 6000;
const TInt KParamsAbsent				= 6001;
const TInt KPassToServiceProvider       = 6002;
const TInt KClientsLeaving				= 6003;
const TInt KClientsJoining				= 6004;

//-=========================================================
//
//States
//
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingParamRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingParamRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingApplyRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
 	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingApplyRequest )

//-=========================================================
//
//State Forks
//
//-=========================================================

//-=========================================================
//
//Transitions
//
//-=========================================================

//-=========================================================
//
//Start/Stop subconnection
//
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TSendStopped, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendStopped )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendStopSubConnection, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendStopSubConnection )


//-=========================================================
//
//Stop Flow
//
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TStopYourFlows, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStopYourFlows )

EXPORT_DECLARE_SMELEMENT_HEADER( TStopSendingFlow, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStopSendingFlow )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendDataClientGoneDown, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientGoneDown )

//-=========================================================
//
//Set Parameters / Raise Events - pass thru function
//
//-=========================================================


EXPORT_DECLARE_SMELEMENT_HEADER( TPassEventToControlClients, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPassEventToControlClients )


EXPORT_DECLARE_SMELEMENT_HEADER( TStoreParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreParams )

EXPORT_DECLARE_SMELEMENT_HEADER( TRespondWithCurrentParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRespondWithCurrentParams )

EXPORT_DECLARE_SMELEMENT_HEADER( TPassToServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPassToServiceProvider )

DECLARE_AGGREGATED_TRANSITION2(
	TStoreAndRespondWithCurrentParams,
	SCprStates::TStoreParams,
	SCprStates::TRespondWithCurrentParams
	)

DECLARE_AGGREGATED_TRANSITION2(
	TStoreParamsAndPostToOriginators,
	SCprStates::TStoreParams,
	CoreStates::TPostToOriginators
	)
 
EXPORT_DECLARE_SMELEMENT_HEADER( TSendApplyResponse, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
   	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendApplyResponse )

}


#endif //SYMBIAN_CORESCPRSTATES_H


