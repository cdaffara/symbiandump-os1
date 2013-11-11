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
* Description:  Haptic server implementation.
*
*/


#include "hwrmhapticsserver.h"
#include "hwrmhapticssession.h"
#include "hwrmhapticsconfiguration.h"
#include "hwrmhapticspluginmanager.h"  
#include "hwrmhapticscommondata.h"
#include "hwrmhapticsreservationhandler.h"
#include "hwrmhapticscommands.h"
#include "hwrmhapticsshutdown.h"
#include "hwrmhapticstrace.h"

const TInt KDefaultPluginTimeout = 3000000; // ms

// HWRMHaptics policy
_LIT( KHapticsPolicyFilename, "hwrmhapticspolicy.ini" );

/**
 * Plat sec policy configuration
 */
const TInt hapticsSecRanges[] = {0,11};
const TUint hapticsSecRangeCount = sizeof(hapticsSecRanges)/sizeof(hapticsSecRanges[0]);
const TUint8 hapticsSecElementsIndex[hapticsSecRangeCount] =
    {
    CPolicyServer::EAlwaysPass,
    CPolicyServer::EAlwaysPass
    };
const CPolicyServer::TPolicyElement hapticsSecElements[] =
    {_INIT_SECURITY_POLICY_PASS};
const CPolicyServer::TPolicy hapticsSecPolicy =
    {
    CPolicyServer::EAlwaysPass,
    hapticsSecRangeCount,
    hapticsSecRanges,
    hapticsSecElementsIndex,
    hapticsSecElements,
    };

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsServer::CHWRMHapticsServer( TInt aPriority ) 
    : CPolicyServer( aPriority, hapticsSecPolicy, EUnsharableSessions )
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsServer::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::ConstructL()" ) ) );

    // construct the shutdown timer
    iShutdown = CHWRMHapticsShutdown::NewL();
    
    // Add server to active scheduler
    StartL( KServerProcessName );
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsServer* CHWRMHapticsServer::NewL( TInt aPriority )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::NewL(0x%x)" ), aPriority ) );

    CHWRMHapticsServer* self = CHWRMHapticsServer::NewLC( aPriority );
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsServer* CHWRMHapticsServer::NewLC( TInt aPriority )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::NewLC(0x%x)" ), aPriority ) );

    CHWRMHapticsServer* self = 
        new( ELeave ) CHWRMHapticsServer( aPriority );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::NewLC - return 0x%x" ), self ) );

    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsServer::~CHWRMHapticsServer()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::~CHWRMHapticsServer()" ) ) );

    // Delete plugin manager
    delete iHapticsPluginManager;

    // Delete reservation handler
    delete iHapticsReservationHandler;

    // Delete common data
    delete iHapticsCommonData;
    
    // Delete the shutdown timer
    delete iShutdown;

    // Signal final closure of ecom session
    REComSession::FinalClose();

    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::~CHWRMHapticsServer - return") ) );
    }


// ---------------------------------------------------------------------------
// Creates a new CSession2
// ---------------------------------------------------------------------------
//
CSession2* CHWRMHapticsServer::NewSessionL( 
                                        const TVersion& aVersion,
                                        const RMessage2& /*aMessage*/ ) const
    {
    // check version
    if ( !User::QueryVersionSupported( TVersion( KServerVersionMajor, 
                                                 KServerVersionMinor, 
                                                 KServerVersionBuild ), 
                                                 aVersion ) )
        {
        // given version not supported
        User::Leave( KErrNotSupported );
        }
    
    // create session instance
    CSession2* session = CHWRMHapticsSession::NewL();


    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::NewSessionL - return 0x%x" ), session ) );

    return session;
    }
   
// ---------------------------------------------------------------------------
// Returns pointer to correct plugin handler.
// In case resource is not supported, returns NULL
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginManager* CHWRMHapticsServer::PluginManager() const
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::PluginManager - return 0x%x" ), iHapticsPluginManager ) );
    return iHapticsPluginManager;
    }

// ---------------------------------------------------------------------------
// Returns pointer to correct reservation handler.
// In case resource is not supported, returns NULL
// ---------------------------------------------------------------------------
//
CHWRMHapticsReservationHandler* CHWRMHapticsServer::ReservationHandler() const
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::ReservationHandler - return 0x%x" ), iHapticsReservationHandler ) );

    return iHapticsReservationHandler;
    }

// ---------------------------------------------------------------------------
// Returns pointer to Haptics common data
// ---------------------------------------------------------------------------
//
CHWRMHapticsCommonData* CHWRMHapticsServer::HapticsCommonData() const
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::HapticsCommonData - return 0x%x" ), iHapticsCommonData ) );
    return iHapticsCommonData;
    }

// ---------------------------------------------------------------------------
// Initialises Haptics
// ---------------------------------------------------------------------------
//
void CHWRMHapticsServer::InitHaptics()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::InitHaptics()" ) ) );

    // Initialize, if not already initialized
    if ( !iHapticsInitiated )
        {
        delete iHapticsReservationHandler;
        delete iHapticsPluginManager;
        delete iHapticsCommonData;
        iHapticsReservationHandler = NULL;
        iHapticsPluginManager = NULL;
        iHapticsCommonData = NULL;

        // create reservation handler
        TRAPD( err, iHapticsReservationHandler = 
                    CHWRMHapticsReservationHandler::NewL( 
                                                KHapticsPolicyFilename ) );
        
        COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::InitHaptics() - iHapticsReservationHandler creation: %d" ), err ) );

        // if reservation handler creation succeeded, create common data
        if ( !err && iHapticsReservationHandler )
            {
            // create haptics common data
            TRAP( err, iHapticsCommonData = 
                           CHWRMHapticsCommonData::NewL() );
            
            COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::InitHaptics() - iHapticsPluginManager creation: %d" ), err ) );

            // if above creations succeeded, create plugin manager
            if ( !err && iHapticsCommonData )
                {
                // create plugin manager
                TRAP( err, iHapticsPluginManager = 
                           CHWRMHapticsPluginManager::NewL( *iHapticsCommonData,
                                                            KDefaultPluginTimeout ) );
                if ( !err  && iHapticsPluginManager )
                    {
                    iHapticsInitiated = ETrue;
                    }

                COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::InitHaptics() - iHapticsCommonData creation: %d" ), err ) );
                }
            }
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsServer::InitHaptics() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Increases the amount of sessions connected to this server.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsServer::AddSession()
    {
    // increase session count
    ++iSessionCount;
    
    // Cancel the shutdown timer if it was running
    iShutdown->Cancel();
    }

// ---------------------------------------------------------------------------
// Decreases the amount of sessions connected to this server.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsServer::DropSession()
    {
    // decrease session count
    --iSessionCount;

    // Start the shutdown timer there are no more sessions
    if ( iSessionCount == 0 && !iShutdown->IsActive() )
        {
        iShutdown->Start();
        }
    }

//  End of File  
