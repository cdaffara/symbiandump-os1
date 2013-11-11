/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CSsmNonCriticalSwpPolicy class.
*
*/

#include <ssm/ssmswp.h>
#include <ssm/ssmstate.h>

#include "ssmnoncriticalswppolicy.h"
#include "ssmswp.hrh"
#include "ssmmapperutility.h"
#include "trace.h"
#include "ssmsubstateext.hrh"

/**
* SWP noncritical phase policy resource file path format : "\private\<SID of SSM>\swpnoncritical\"
*/
_LIT( KCommandListPath, "Z:\\private\\2000D75B\\swpnoncritical\\" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmNonCriticalSwpPolicy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmSwpPolicy* CSsmNonCriticalSwpPolicy::NewL()
    {
	FUNC_LOG;

    CSsmNonCriticalSwpPolicy* self = new( ELeave ) CSsmNonCriticalSwpPolicy;
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSsmNonCriticalSwpPolicy::~CSsmNonCriticalSwpPolicy
// ---------------------------------------------------------------------------
//
CSsmNonCriticalSwpPolicy::~CSsmNonCriticalSwpPolicy()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmNonCriticalSwpPolicy::GetCommandListPathL
// ---------------------------------------------------------------------------
//
void CSsmNonCriticalSwpPolicy::GetCommandListPathL( TDes& aCmdListPath )
    {
    FUNC_LOG;

    aCmdListPath = KCommandListPath;
    iUtil->GetCommandListPath( aCmdListPath );
    }


// ---------------------------------------------------------------------------
// CSsmNonCriticalSwpPolicy::TransitionAllowed
// ---------------------------------------------------------------------------
//
MSsmSwpPolicy::TResponse CSsmNonCriticalSwpPolicy::TransitionAllowed(
    const TSsmSwp& aSwp,
    const RMessagePtr2& aMessage )
    {
    FUNC_LOG;
    INFO_2( "Requested SWP transition: 0x%08x::0x%08x", aSwp.Key(), aSwp.Value() );

    TResponse response = ENotAllowed;
    if ( iUtil->CheckCaps( aMessage ) )
        {
	    TSsmState currentState;
		const TInt errorCode = GetCurrentState(currentState);
		ERROR( errorCode, "Failed to get current state" );

		// this SwP transition would be allowed only once per boot(during the system startup).
        if ((KErrNone == errorCode) && (ESsmStartup == currentState.MainState()) && (aSwp.Value() == ESsmNonCriticalPhaseStarted))
            {
            response = EAllowed;
            }
        else
            {
            INFO( "Requested SWP transition not allowed" );
            }
        }
    else
        {
        INFO( "Capability Check Failed" );
        }

    return response;
    }


// ---------------------------------------------------------------------------
// CSsmNonCriticalSwpPolicy::CSsmNonCriticalSwpPolicy
// ---------------------------------------------------------------------------
//
CSsmNonCriticalSwpPolicy::CSsmNonCriticalSwpPolicy()
    {
    FUNC_LOG;
    }
