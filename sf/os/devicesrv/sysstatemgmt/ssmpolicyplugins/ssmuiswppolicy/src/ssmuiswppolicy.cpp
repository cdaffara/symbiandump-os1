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
* Description: Implementation of CSsmUiSwpPolicy class.
*
*/

#include <ssm/ssmswp.h>

#include "ssmuiswppolicy.h"
#include "ssmswp.hrh"
#include "ssmmapperutility.h"
#include "trace.h"

/**
* SWP ui policy resource file path format : "\private\<SID of SSM>\swpui\"
*/
_LIT( KCommandListPath, "Z:\\private\\2000D75B\\swpui\\" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmUiSwpPolicy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmSwpPolicy* CSsmUiSwpPolicy::NewL()
    {
    CSsmUiSwpPolicy* self = new( ELeave ) CSsmUiSwpPolicy;
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSsmUiSwpPolicy::~CSsmUiSwpPolicy
// ---------------------------------------------------------------------------
//
CSsmUiSwpPolicy::~CSsmUiSwpPolicy()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmUiSwpPolicy::GetCommandListPathL
// ---------------------------------------------------------------------------
//
void CSsmUiSwpPolicy::GetCommandListPathL( TDes& aCmdListPath )
    {
    FUNC_LOG;

    aCmdListPath = KCommandListPath;
    iUtil->GetCommandListPath( aCmdListPath );
    }


// ---------------------------------------------------------------------------
// CSsmUiSwpPolicy::TransitionAllowed
// ---------------------------------------------------------------------------
//
MSsmSwpPolicy::TResponse CSsmUiSwpPolicy::TransitionAllowed(
    const TSsmSwp& aSwp,
    const RMessagePtr2& aMessage )
    {
    FUNC_LOG;
    INFO_2( "Requested SWP transition: 0x%08x::0x%08x", aSwp.Key(), aSwp.Value() );

    TResponse response = ENotAllowed;
    if ( iUtil->CheckCaps( aMessage ) )
        {
        if ( aSwp.Value() == ESsmUiPhaseStarted )
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
// CSsmUiSwpPolicy::CSsmUiSwpPolicy
// ---------------------------------------------------------------------------
//
CSsmUiSwpPolicy::CSsmUiSwpPolicy()
    {
    FUNC_LOG;
    }
