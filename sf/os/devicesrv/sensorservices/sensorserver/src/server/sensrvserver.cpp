/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor server main class
*
*/

#include <ecom/ecom.h>
#include "sensrvdefines.h"
#include "sensrvserver.h"
#include "sensrvsession.h"
#include "sensrvproxymanager.h"
#include "sensrvtrace.h"
#include "sensrvchannelinfo.h"


// -----------------------------------------------------------------------------
// CSensrvServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensrvServer* CSensrvServer::NewL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::NewL()" ) ) );

    CSensrvServer* self = CSensrvServer::NewLC();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CSensrvServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensrvServer* CSensrvServer::NewLC()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::NewLC()" ) ) );

    CSensrvServer* self = new( ELeave ) CSensrvServer(EUnsharableSessions);
    
    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::NewLC - return 0x%x" ), self ) );

    return self;
    }
    

// -----------------------------------------------------------------------------
// CSensrvServer::CSensrvServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSensrvServer::CSensrvServer( const TServerType aType  )
    : CPolicyServer( KSensrvCActivePriority, KSensrvPolicy, aType )
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CSensrvServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSensrvServer::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::ConstructL()" ) ) );
 
    // Add server to active scheduler before doing anything else so there won't be synchronization problems with multiple clients.
    StartL( KSensrvProcessName );

    // Create proxy manager
    iProxyManager = CSensrvProxyManager::NewL(*this);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSensrvServer::~CSensrvServer()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::~CSensrvServer()" ) ) );

    // Close all open sessions
    CSensrvSession* session=NULL;
    iSessionIter.SetToFirst();
    while((session = static_cast<CSensrvSession*>(iSessionIter++)) != NULL)
    {
        delete session;
    }

    delete iProxyManager;

    // Signal final closure of ecom session for server main thread
    REComSession::FinalClose();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::~CSensrvServer - return") ) );
    }


// ---------------------------------------------------------------------------
// Notifies all client sessions that there is a change in available channels
// Sessions will notify clients if they are interested.
// ---------------------------------------------------------------------------
//
void CSensrvServer::NotifyChannelChange(const TSensrvResourceChannelInfo& aChangedChannel,
                                        TSensrvChannelChangeType aChangeType )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::NotifyChannelChange(aChangedChannel.iChannelId: %d, aChangeType: %d)"), aChangedChannel.iChannelId, aChangeType ) );

    // Iterate through all sessions and notify them
    CSensrvSession* session=NULL;
    iSessionIter.SetToFirst();
    while((session = static_cast<CSensrvSession*>(iSessionIter++)) != NULL)
    {
        session->NotifyChannelChange(aChangedChannel, aChangeType);
    }
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::NotifyChannelChange - return" )) );
    }
        
// -----------------------------------------------------------------------------
// CSensrvServer::NewSessionL
// Creates a new CSession2
// -----------------------------------------------------------------------------
//
CSession2* CSensrvServer::NewSessionL( const TVersion& aVersion,
                                       const RMessage2& aMessage ) const
    {
#if defined(_DEBUG) && defined(COMPONENT_TRACE_FLAG)
    TVersionName name = aVersion.Name();
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::NewSessionL(%S)" ), &name) );
#endif

    if ( !User::QueryVersionSupported( TVersion( KSensrvVersionMajor, 
                                                 KSensrvVersionMinor, 
                                                 KSensrvVersionBuild ), 
                                                 aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
            
    CSession2* session = CSensrvSession::NewL( *const_cast<CSensrvServer*>(this), 
                                               *iProxyManager,
                                               aMessage.SecureId() );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::NewSessionL - return 0x%x" ), session ) );

    return( session );
    }



   
