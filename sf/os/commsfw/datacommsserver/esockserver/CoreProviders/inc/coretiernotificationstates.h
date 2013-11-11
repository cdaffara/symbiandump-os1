/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file coretiernotificationstates.h
 @internalTechnology
*/

#ifndef SYMBIAN_CORETIERNOTIFICATIONSTATES_H
#define SYMBIAN_CORETIERNOTIFICATIONSTATES_H

#include <comms-infras/coretiermanagerstates.h>
#include <comms-infras/coretiernotificationactivity.h>

namespace TierNotification
{

typedef MeshMachine::TNodeContext<ESock::CTierManagerBase,TMStates::TContext,TierNotification::CTierNotificationActivity> TContext;

//-=========================================================
//
//States
//
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingTierNotificationRegistration, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingTierNotificationRegistration )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataCollectors, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataCollectors )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingCancelOrErrorOrDestroy, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingCancelOrErrorOrDestroy )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDestroy, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( AwaitingDestroy )


//-=========================================================
//
//State Decision Points
//
//-=========================================================


//-=========================================================
//
//Transitions
//
//-=========================================================

// Tier Notification activity
//
EXPORT_DECLARE_SMELEMENT_HEADER( TInitTierNotifications, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TInitTierNotifications )

EXPORT_DECLARE_SMELEMENT_HEADER( TStartTierNotifications, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStartTierNotifications )

EXPORT_DECLARE_SMELEMENT_HEADER( TInitiateTierNotificationCancel, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TInitiateTierNotificationCancel )

// Get Data Collectors activity
//
EXPORT_DECLARE_SMELEMENT_HEADER( TGetDataCollectors, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TGetDataCollectors )




} //namespace TierNotification


#endif //SYMBIAN_CORETIERNOTIFICATIONSTATES_H

