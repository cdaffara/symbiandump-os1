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

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_CORETIERMANAGERSTATES_H
#define SYMBIAN_CORETIERMANAGERSTATES_H

#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/coretiermanager.h>

class CCoreMetaConnectionProvider;


namespace TMStates
{

typedef MeshMachine::TNodeContext<CCoreTierManager,CoreNetStates::TContext> TContext;

//-=========================================================
//
//Core TM Transition Ids 6000..10000
//
//-=========================================================
const TInt KSelectedProvider               = 6000;
const TInt KSelectedProviderIsNull         = 6001;

//-=========================================================
//
//States
//
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingSelectProvider, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingSelectProvider )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingSelectProviderSuper, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingSelectProviderSuper )

// availability - all-in-tier
EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingRegistrationForTierAvailabilityNotification, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingRegistrationForTierAvailabilityNotification )

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

EXPORT_DECLARE_SMELEMENT_HEADER( TSelectProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSelectProvider )

EXPORT_DECLARE_SMELEMENT_HEADER(TSelectProviderSuper, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TMStates::TContext)
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSelectProviderSuper )

// Join to all MCPRs. Less specific than Selection.
EXPORT_DECLARE_SMELEMENT_HEADER( TJoinToMCPRs, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TJoinToMCPRs )

EXPORT_DECLARE_SMELEMENT_HEADER( TRegisterForAvailability, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRegisterForAvailability )

EXPORT_DECLARE_SMELEMENT_HEADER( TPassAvailabilityNotificationToOriginators, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPassAvailabilityNotificationToOriginators )

DECLARE_AGGREGATED_TRANSITION2(
	TJoinToMCPRsAndRegisterForAvailability,
	TMStates::TJoinToMCPRs,
	TMStates::TRegisterForAvailability
	)

} //namespace CoreTierManagerStates


#endif //SYMBIAN_CORETIERMANAGERSTATES_H

