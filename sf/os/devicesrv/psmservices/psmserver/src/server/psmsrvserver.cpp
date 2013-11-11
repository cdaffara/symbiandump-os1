/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Power save mode server main class
*
*/


#include <ecom/ecom.h>
#include "psmsrvserver.h"
#include "psmmanager.h" // CPsmManager
#include "psmsrvsession.h" // CPsmSession
#include "psmsrvshutdown.h"
#include "psmtrace.h"
#include "psmdefines.h"

// -----------------------------------------------------------------------------
// CPsmSrvServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmSrvServer* CPsmSrvServer::NewL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::NewL()" ) ) );

    CPsmSrvServer* self = CPsmSrvServer::NewLC();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmSrvServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmSrvServer* CPsmSrvServer::NewLC()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::NewLC()" ) ) );

    CPsmSrvServer* self = new( ELeave ) CPsmSrvServer( EUnsharableSessions );

    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::NewLC - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmSrvServer::CPsmSrvServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmSrvServer::CPsmSrvServer( const TServerType aType  )
    : CPolicyServer( CActive::EPriorityStandard, KPsmSrvPolicy, aType )
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CPsmSrvServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmSrvServer::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::ConstructL()" ) ) );

    // Add server to active scheduler before doing anything else so there won't 
    // be synchronization problems with multiple clients.
    StartL( KPsmProcessName );

    // Create an instance of the manager class
    iPsmManager = CPsmManager::NewL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmSrvServer::~CPsmSrvServer()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::~CPsmSrvServer()" ) ) );

    delete iPsmManager;
    delete iShutdown;

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::~CPsmSrvServer - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmSrvServer::NewSessionL
// Creates a new CSession2
// -----------------------------------------------------------------------------
//
CSession2* CPsmSrvServer::NewSessionL( const TVersion& aVersion,
                                       const RMessage2& /*aMessage*/ ) const
    {
#if defined(_DEBUG) && defined(COMPONENT_TRACE_FLAG)
    TVersionName name = aVersion.Name();
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::NewSessionL(%S)" ), &name) );
#endif

    if ( !User::QueryVersionSupported( TVersion( KPsmVersionMajor, 
                                                 KPsmVersionMinor, 
                                                 KPsmVersionBuild ), 
                                                 aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    CSession2* session = CPsmSession::NewL( *const_cast<CPsmManager*>(iPsmManager ), 
                                            *const_cast<CPsmSrvServer*>(this ) );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::NewSessionL - return 0x%x" ), session ) );

    return( session );
    }

// -----------------------------------------------------------------------------
// CPsmSrvServer::SessionClosed
// -----------------------------------------------------------------------------
//
void CPsmSrvServer::SessionClosed()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::SessionClosed()" ) ) );

    if ( !iShutdown )
        {
        iShutdown = new CPsmSrvShutdown( *this );
        // Start shutdown
        if ( iShutdown )
            {
            iShutdown->Start();
            }
        }
    else
        {
        iShutdown->Restart();
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::SessionClosed - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSrvServer::ShutdownServer
// -----------------------------------------------------------------------------
//
TInt CPsmSrvServer::ShutdownServer()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::ShutdownServer()" ) ) );

    iSessionIter.SetToFirst();
    CPsmSession* session = NULL;
    session = static_cast<CPsmSession*>(iSessionIter++);

    TInt returnValue;

    if ( !session )
        {
        CActiveScheduler::Stop();
        returnValue = KErrNone;
        }
    else
        {
        returnValue = KErrNotReady;
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvServer::ShutdownServer - return: %d" ), returnValue ) );
    return returnValue;
    }

// End of file
