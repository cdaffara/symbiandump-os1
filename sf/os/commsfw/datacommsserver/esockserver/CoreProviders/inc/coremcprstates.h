/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* generic framework node state machine states
* THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
* 
*
*/



/**
 @file coremcprstates.h
 @internalTechnology
*/

#ifndef SYMBIAN_COREMCPRSTATES_H
#define SYMBIAN_COREMCPRSTATES_H

#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/coremcpr.h>
#include <comms-infras/coremcpractivities.h>



namespace MCprStates
{

typedef MeshMachine::TNodeContext<CCoreMetaConnectionProvider,PRStates::TContext> TContext;

//-=========================================================
//
//Core Meta Connection Provider Transition Ids 6000..10000
//
//-=========================================================
const TInt KSelectedProvider               = 6000;
const TInt KSelectedProviderIsNull         = 6001;
const TInt KSelectionComplete              = 6002;
const TInt KSelectionIncomplete            = 6003;
const TInt KProcessAvailability			   = 6004;
const TInt KCancelAvailability			   = 6005;
const TInt KJoinServiceProvider            = 6006;
const TInt KDataClientStatusChange         = 6007;
const TInt KRejoinTag                      = 6008;
//-=========================================================
//
//States
//
//-=========================================================
DECLARE_SMELEMENT_HEADER( TAwaitingClientLeaveAndNoClients, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingClientLeaveAndNoClients )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingSelectNextLayer, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingSelectNextLayer )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingSelectNextLayerSuper, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingSelectNextLayerSuper )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingTierManagerCreated, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingTierManagerCreated )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingErrorRecoveryRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingErrorRecoveryRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingConnectionStartRecoveryRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingConnectionStartRecoveryRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingConnectionGoneDownRecoveryRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingConnectionGoneDownRecoveryRequest )

DECLARE_SMELEMENT_HEADER( TAwaitingReConnectRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingReConnectRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingReConnectCompleteOrError, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingReConnectCompleteOrError )

//The SelectNextLayerIfNotYetDone transition & AwaitingSelectNextLayerCompleted state
//are used to ensure that the node has performed a full selection before taking any
//further action (monitoring availability, etc).
//Usage: NODEACTIVITY_ENTRY(KNoTag, MCprStates::SelectNextLayerIfNotYetDone, MCprStates::AwaitingSelectNextLayerCompleted, RelevantStateTag)
EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingSelectNextLayerCompleted, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingSelectNextLayerCompleted )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingSelectComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
	//virtual void Cancel();
DECLARE_SMELEMENT_FOOTER( TAwaitingSelectComplete )

//Availability
// mid-tier
DECLARE_SMELEMENT_HEADER( TAwaitingAvailabilityRegistration, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingAvailabilityRegistration )

DECLARE_SMELEMENT_HEADER( TAwaitingAvailabilityNotification, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingAvailabilityNotification )

typedef MeshMachine::TAcceptErrorState<TAwaitingAvailabilityNotification> TAwaitingAvailabilityNotificationOrError;

//-=========================================================
//
//State Forks
//
//-=========================================================
DECLARE_SMELEMENT_HEADER( TDataClientStatusChangeOrNoTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TDataClientStatusChangeOrNoTag )

DECLARE_SMELEMENT_HEADER( TDataClientStatusChangeBackwardsOrNoTagBackwards, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TDataClientStatusChangeBackwardsOrNoTagBackwards )

EXPORT_DECLARE_SMELEMENT_HEADER( TSelectedProvider, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TSelectedProvider )

EXPORT_DECLARE_SMELEMENT_HEADER( TSelectionIncomplete, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TSelectionIncomplete )

EXPORT_DECLARE_SMELEMENT_HEADER( TProcessAvailability, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TProcessAvailability )

DECLARE_SMELEMENT_HEADER( TNoTagOrContentionTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrContentionTag )

DECLARE_SMELEMENT_HEADER( TNoTagOrRejoinTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrRejoinTag )

//-=========================================================
//
//Transitions
//
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TSendFinalSelectComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendFinalSelectComplete )

EXPORT_DECLARE_SMELEMENT_HEADER( TSelectNextLayer, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSelectNextLayer )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendPropagateRecoveryResponse, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendPropagateRecoveryResponse )

EXPORT_DECLARE_SMELEMENT_HEADER( TDecrementBlockingDestroy, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TDecrementBlockingDestroy )

EXPORT_DECLARE_SMELEMENT_HEADER( TPostStatusChangeToOriginators, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostStatusChangeToOriginators )

EXPORT_DECLARE_SMELEMENT_HEADER( TProcessDataClientRejoin, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessDataClientRejoin )


DECLARE_SMELEMENT_HEADER( TSendProviderStatusActivated, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendProviderStatusActivated )

DECLARE_SMELEMENT_HEADER( TSendProviderStatusDeactivated, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendProviderStatusDeactivated )

DECLARE_AGGREGATED_TRANSITION2(
	TDecrementBlockingDestroyAndAddControlClientAndSendJoinCompleteIfRequest,
	TDecrementBlockingDestroy,
	CoreNetStates::TAddControlClientAndSendJoinCompleteIfRequest
	)

DECLARE_AGGREGATED_TRANSITION2(
	TDecrementBlockingDestroyAndAddDataClientAndRespond,
	TDecrementBlockingDestroy,
	CoreNetStates::TAddDataClientAndRespond
	)

// Connection Status Notification
EXPORT_DECLARE_SMELEMENT_HEADER( TProcessProviderStatusChangeRegistration, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessProviderStatusChangeRegistration )

//INFO:
// - DEF093256 - RVCT2.2 [Build 435] crashes on templates (initialised with namespaced types)
//is no longer effecting us
DECLARE_AGGREGATED_TRANSITION5(
	TProcessClientLeave,
	MeshMachine::TRemoveClient, //First remove the client
	CoreStates::TAbortAllActivitiesNodeDeletion, //Then cancel all of the other remaining activities
	CoreNetStates::TSendLeaveCompleteIfRequest, //Respond
	CoreNetStates::TSendClientLeavingRequestToServiceProviders,
	CoreNetStates::TSetIdleIfNoServiceProviders
	)

} //namespace MCprStates




#endif //SYMBIAN_COREMCPRSTATES_H


