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


#include "atextsrv.h"
#include "atextclientsrv.h"
#include "atextsecpolicy.h"
#include "atextsession.h"
#include "debug.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CATExtSrv::CATExtSrv( TInt aPriority ) :
    CPolicyServer( aPriority, KSrvPolicy )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CATExtSrv::~CATExtSrv()
    {
    iSessions.Close();
    TRACE_FUNC
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CATExtSrv* CATExtSrv::NewLC()
    {
    CATExtSrv* self = new (ELeave) CATExtSrv( EPriorityNormal );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->StartL( KATExtSrvName );
    RThread().SetPriority( EPriorityNormal );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CATExtSrv::ClientClosed( CATExtSession& aSession )
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
//
// ---------------------------------------------------------------------------
//
CSession2* CATExtSrv::NewSessionL( const TVersion& aVersion,
                                   const RMessage2& /*aMessage*/ ) const
    {
    TRACE_FUNC_ENTRY
    CATExtSrv& thisServer = const_cast<CATExtSrv&>( *this );
    CATExtSession* session = CATExtSession::NewL( thisServer, aVersion );
    CleanupStack::PushL( session );
    thisServer.iSessions.AppendL( session );
    CleanupStack::Pop( session );
    TRACE_FUNC_EXIT
    return session;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CATExtSrv::ConstructL()
    {
    TRACE_FUNC_ENTRY
    TRACE_FUNC_EXIT
    }
