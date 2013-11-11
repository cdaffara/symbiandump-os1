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
// Core CPR States
// 
//

/**
 @file
 @internalComponent
*/
#define SYMBIAN_NETWORKING_UPS

#ifndef SYMBIAN_CORECPRSTATES_H
#define SYMBIAN_CORECPRSTATES_H

#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/corecpr.h>

namespace CprStates
{

typedef MeshMachine::TNodeContext<CCoreConnectionProvider, PRStates::TContext> TContext;

//-=========================================================
//
//Core Connection Provider Transition Ids 6000..10000
//
//-=========================================================
const TUint KCreateAdditionalDataClient = 6001;
const TUint KCreatingAdditionalDataClient = 6002;
const TUint KSendPriorityToCtrlProvider	= 6003;
const TUint KSendPriorityToServProvider	= 6004;
const TUint KSendActive			= 6005;
const TUint KAddClient			= 6006;
const TUint KUpdatePriority		= 6007;

//-=========================================================
//
//States
//
//-=========================================================

//-=========================================================
//
//Stateforks
//
//-=========================================================
EXPORT_DECLARE_SMELEMENT_HEADER( TCreateAdditionalDataClientOrDataClientReady, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TCreateAdditionalDataClientOrDataClientReady )

EXPORT_DECLARE_SMELEMENT_HEADER( TCreateAdditionalDataClientBackwardOrDataClientReady, CprStates::TCreateAdditionalDataClientOrDataClientReady, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TCreateAdditionalDataClientBackwardOrDataClientReady )
	
EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrBearerPresent, CprStates::TCreateAdditionalDataClientOrDataClientReady, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrBearerPresent )
	
//-=========================================================
//
//State Transitions
//
//-=========================================================
EXPORT_DECLARE_SMELEMENT_HEADER( THandleDataClientIdle, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( THandleDataClientIdle )


EXPORT_DECLARE_SMELEMENT_HEADER( TCreateAdditionalDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCreateAdditionalDataClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendBinderRequestCPR, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBinderRequestCPR )

EXPORT_DECLARE_SMELEMENT_HEADER( TBindToPrimaryDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TBindToPrimaryDataClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TCreateDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCreateDataClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendDataClientStatusStoppedIfNoControlClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientStatusStoppedIfNoControlClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TAddControlClient,  MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TAddControlClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendControlClientJoinRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendControlClientJoinRequest )

#ifdef SYMBIAN_NETWORKING_UPS
EXPORT_DECLARE_SMELEMENT_HEADER( TPostPolicyCheckResponseToOriginators, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostPolicyCheckResponseToOriginators )
#endif

DECLARE_AGGREGATED_TRANSITION2(
	TProcessAdditionalDataClientCreationAndBindToPrimary,
	PRStates::TProcessDataClientCreation,
	CprStates::TBindToPrimaryDataClient
	)

} //namespace CprStates




#endif
//SYMBIAN_CORECPRSTATES_H


