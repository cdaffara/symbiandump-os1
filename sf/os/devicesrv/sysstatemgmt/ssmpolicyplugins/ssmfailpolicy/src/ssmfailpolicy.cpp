/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CSsmFailPolicy class.
*
*/

#include <ssm/ssmstate.h>
#include <ssm/ssmstatetransition.h>

#include "ssmfailpolicy.h"
#include "ssmsubstateext.hrh"
#include "ssmmapperutility.h"
#include "trace.h"

/**
* Fail state policy resource file path format :
* "\private\<SID of SSM>\fail\"
*/
_LIT( KCommandListPath, "Z:\\private\\2000D75B\\fail\\" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmFailPolicy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmStatePolicy* CSsmFailPolicy::NewL()
    {
    CSsmFailPolicy* self = new( ELeave ) CSsmFailPolicy;
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSsmFailPolicy::~CSsmFailPolicy
// ---------------------------------------------------------------------------
//
CSsmFailPolicy::~CSsmFailPolicy()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmFailPolicy::GetNextState
// ---------------------------------------------------------------------------
//
TBool CSsmFailPolicy::GetNextState(
    TSsmState INFO_PARAM( aCurrentTransition ),
    TInt /*aReason*/,
    TInt ERROR_PARAM( aError ),
    TInt ERROR_PARAM( aSeverity ),
    TSsmState& /*aNextState*/ )
    {
    FUNC_LOG;
    INFO_2( "Current state %04x.%04x",
        aCurrentTransition.MainState(), aCurrentTransition.SubState() );
    ERROR_1( aError, "State transition resulted in an error, severity %d", aSeverity );

	return EFalse;
    }


// ---------------------------------------------------------------------------
// CSsmFailPolicy::CSsmFailPolicy
// ---------------------------------------------------------------------------
//
CSsmFailPolicy::CSsmFailPolicy()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmFailPolicy::GetCommandListPathL
// ---------------------------------------------------------------------------
//
void CSsmFailPolicy::GetCommandListPathL( TDes& aCmdListPath )
    {
    FUNC_LOG;

	aCmdListPath = KCommandListPath;
	iUtil->GetCommandListPath( aCmdListPath );
    }


// ---------------------------------------------------------------------------
// CSsmFailPolicy::IsAllowedTargetState
// ---------------------------------------------------------------------------
//
TBool CSsmFailPolicy::IsAllowedTargetState(
    const TSsmStateTransition& aRequest ) const
    {
    FUNC_LOG;

    TUint16 mainState( aRequest.State().MainState() );
    INFO_2( "Requested target state = %04x.%04x", mainState, aRequest.State().SubState() );

    if ( mainState == ESsmShutdown )
        {
        return ETrue;
        }

    return EFalse;
    }


// ---------------------------------------------------------------------------
// CSsmFailPolicy::TargetSubState
// ---------------------------------------------------------------------------
//
TUint16 CSsmFailPolicy::TargetSubState( const TUint16 aRequestedSubState )
    {
    FUNC_LOG;
    INFO_1( "Requested sub state %04x", aRequestedSubState );

	TUint16 subState = ( aRequestedSubState == KSsmAnySubState ?
        ESsmStateFail : aRequestedSubState );

    INFO_1( "Target sub state %04x", subState );
    return subState;
    }
