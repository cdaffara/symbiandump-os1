/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CSsmStatePolicyBase class.
*
*/

#include <bautils.h>
#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/ssmstate.h>
#include <ssm/ssmstatetransition.h>

#include "ssmstatepolicybase.h"
#include "ssmmapperutilitystatic.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmStatePolicyBase::CSsmStatePolicyBase
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmStatePolicyBase::CSsmStatePolicyBase()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmStatePolicyBase::~CSsmStatePolicyBase
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmStatePolicyBase::~CSsmStatePolicyBase()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmStatePolicyBase::Initialize
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmStatePolicyBase::Initialize( TRequestStatus& aStatus )
    {
    FUNC_LOG;
    ASSERT_TRACE( iCommandListResourceReader );

	// initialise command list resource reader.
	iCommandListResourceReader->Initialise( aStatus );
    }


// ---------------------------------------------------------------------------
// CSsmStatePolicyBase::InitializeCancel
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmStatePolicyBase::InitializeCancel()
    {
    FUNC_LOG;

    iCommandListResourceReader->InitialiseCancel();
    }


// ---------------------------------------------------------------------------
// CSsmStatePolicyBase::TransitionAllowed
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmStatePolicy::TResponse CSsmStatePolicyBase::TransitionAllowed(
    const TSsmStateTransition& aRequest,
    TSsmStateTransition const* aCurrent,
    TSsmStateTransition const* aQueued,
    const RMessagePtr2& aMessage )
    {
    FUNC_LOG;
    INFO_2( "Requested state transition: %04x.%04x", aRequest.State().MainState(), aRequest.State().SubState() );

    SsmMapperUtility::CheckCaps( aMessage );

    if ( IsAllowedTargetState( aRequest ) )
        {
        if ( aCurrent )
            {
            INFO_2( "Current state transition: %04x.%04x", aCurrent->State().MainState(), aCurrent->State().SubState() );

            TUint16 mainState( aRequest.State().MainState() );
            if ( mainState == ESsmFail || mainState == ESsmShutdown )
                {
    		    INFO( "Failure / shutdown transition requested -> start immediately" );
                return EReplaceCurrentClearQueue;
                }

            if ( aQueued ) // There propably can be a queued transition only if there is a current transition(?)
                {
                INFO_2( "Queued state transition: %04x.%04x", aQueued->State().MainState(), aQueued->State().SubState() );
                }

            INFO( "Queuing state transition" );
            return ECurrentRemainReplaceQueued;
            }

		INFO( "No current state transition -> start immediately" );
        return EDefinitelyAllowed;
        }
    else
		{
		INFO( "State is not reachable" );
		return ENotAllowed;
		}
    }


// ---------------------------------------------------------------------------
// CSsmStatePolicyBase::PrepareCommandList
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmStatePolicyBase::PrepareCommandList(
    TSsmState aState,
#ifdef INFO_TRACE
    TInt aReason,
#else
    TInt /*aReason*/,
#endif
    TRequestStatus& aStatus )
    {
    FUNC_LOG;
    INFO_3( "State %04x.%04x, reason %04x", aState.MainState(), aState.SubState(), aReason );

	TSsmState publishState(
	    aState.MainState(), TargetSubState( aState.SubState() ) );

	// Build the commandlist from resource.
	// Substate id is used as a command list id.
	iCommandListResourceReader->PrepareCommandList(
	    publishState.SubState(), publishState, aStatus );
    }


// ---------------------------------------------------------------------------
// CSsmStatePolicyBase::PrepareCommandListCancel
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmStatePolicyBase::PrepareCommandListCancel()
    {
    FUNC_LOG;

    iCommandListResourceReader->PrepareCommandListCancel();
    }


// ---------------------------------------------------------------------------
// CSsmStatePolicyBase::CommandList
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmCommandList* CSsmStatePolicyBase::CommandList()
    {
    FUNC_LOG;

	return iCommandListResourceReader->GetCommandList();
    }


// ---------------------------------------------------------------------------
// CSsmStatePolicyBase::Release
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmStatePolicyBase::Release()
    {
    FUNC_LOG;

    delete this;
    }
