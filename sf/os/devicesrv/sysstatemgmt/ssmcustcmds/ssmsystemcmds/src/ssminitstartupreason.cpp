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
* Description:
* Implementation of CSsmInitStartupReason class.
*
*/

#include "ssminitstartupreason.h"
#include "ssmmapperutility.h"
#include "trace.h"

#include <starterdomaincrkeys.h>
#include <startupdomainpskeys.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmInitStartupReason::NewL
// ---------------------------------------------------------------------------
//
CSsmInitStartupReason* CSsmInitStartupReason::NewL()
	{
    FUNC_LOG;
	return new ( ELeave ) CSsmInitStartupReason();
	}


// ---------------------------------------------------------------------------
// CSsmInitStartupReason::~CSsmInitStartupReason
// ---------------------------------------------------------------------------
//
CSsmInitStartupReason::~CSsmInitStartupReason()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmInitStartupReason::Initialize
// ---------------------------------------------------------------------------
//
TInt CSsmInitStartupReason::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
    FUNC_LOG;

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CSsmInitStartupReason::Execute
// ---------------------------------------------------------------------------
//
void CSsmInitStartupReason::Execute(
    const TDesC8& /*aParams*/,
    TRequestStatus& aRequest )
    {
    FUNC_LOG;
    aRequest = KRequestPending;

    TRAPD( errorCode, InitStartupReasonL() );
    ERROR( errorCode, "Failed to initialize startup reason P&S keys" );

    TRequestStatus* request = &aRequest;
    User::RequestComplete( request, errorCode );
    }


// ---------------------------------------------------------------------------
// CSsmInitStartupReason::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CSsmInitStartupReason::ExecuteCancel()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmInitStartupReason::Close
// ---------------------------------------------------------------------------
//
void CSsmInitStartupReason::Close()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmInitStartupReason::Release
// ---------------------------------------------------------------------------
//
void CSsmInitStartupReason::Release()
    {
    FUNC_LOG;

	delete this;
    }
// ---------------------------------------------------------------------------
// CSsmInitStartupReason::InitStartupReasonL
// ---------------------------------------------------------------------------
//
void CSsmInitStartupReason::InitStartupReasonL()
    {
    FUNC_LOG;

    TInt val( 0 );
    TInt errorCode = MapperUtilityL().CrValue( KCRUidStartup, KStartupReason, val );
    ERROR( errorCode, "Failed to get KStartupReason CenRep key" );

    if ( errorCode == KErrNone )
        {
        SetL( KPSUidStartup, KPSStartupReason, val );        
        INFO_1( "Value of startup reason is %d", val );
        }
    User::LeaveIfError( errorCode );
    }

