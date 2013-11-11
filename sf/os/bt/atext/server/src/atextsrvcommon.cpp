/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "atextsrvcommon.h"
#include "atextclientsrvcommon.h"
#include "atextsecpolicy.h"
#include "atextcommonsession.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CATExtSrvCommon::CATExtSrvCommon( TInt aPriority ) :
    CPolicyServer( aPriority, KSrvPolicy )
    {
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CATExtSrvCommon::~CATExtSrvCommon()
    {
    iSessions.Close();
    TRACE_FUNC
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CATExtSrvCommon* CATExtSrvCommon::NewLC()
    {
    CATExtSrvCommon* self = new (ELeave) CATExtSrvCommon( EPriorityNormal );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->StartL( KATExtSrvName );
    RThread().SetPriority( EPriorityNormal );
    return self;
    }

// ---------------------------------------------------------------------------
// Removes session from array of instantiated sessions
// ---------------------------------------------------------------------------
//
void CATExtSrvCommon::ClientClosed( CATExtCommonSession& aSession )
	{
	TRACE_FUNC_ENTRY
	TInt index = iSessions.Find( &aSession );
	if ( index >= 0 )
	    {
	    iSessions.Remove( index );
	    }
	TRACE_FUNC_EXIT
	}

// ---------------------------------------------------------------------------
// Creates a new session to client
// ---------------------------------------------------------------------------
//
CSession2* CATExtSrvCommon::NewSessionL( const TVersion& aVersion,
                                         const RMessage2& /*aMessage*/ ) const
    {
    TRACE_FUNC_ENTRY
    CATExtSrvCommon& thisServer = const_cast<CATExtSrvCommon&>( *this );
    CATExtCommonSession* session = CATExtCommonSession::NewL( thisServer,
                                                              aVersion );
    CleanupStack::PushL( session );
    thisServer.iSessions.AppendL( session );
    CleanupStack::Pop( session );
    TRACE_FUNC_EXIT
    return session;
    }

// ---------------------------------------------------------------------------
// CATExtSrvCommon::ConstructL
// ---------------------------------------------------------------------------
//
void CATExtSrvCommon::ConstructL()
    {
    TRACE_FUNC_ENTRY
    TRACE_FUNC_EXIT
    }
