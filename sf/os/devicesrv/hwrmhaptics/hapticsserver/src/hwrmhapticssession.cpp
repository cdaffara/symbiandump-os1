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
* Description:  Haptics server session implementation.
*
*/


#include "hwrmhapticssession.h"
#include "hwrmhapticsserver.h"
#include "hwrmhapticsservice.h"
#include "hwrmhapticstrace.h"


// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsSession::CHWRMHapticsSession()    
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::CHWRMHapticsSession()" ) ) );
    }    

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsSession* CHWRMHapticsSession::NewL()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::NewL()" ) ) );

    CHWRMHapticsSession* self = new ( ELeave ) CHWRMHapticsSession();

    COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// CreateL calls server's AddSession to increment session count.
// This method is called by CServer2 when creating new session.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsSession::CreateL() 
    {
    MyServer().AddSession();
    }


// ---------------------------------------------------------------------------
// This method is called by CServer2 when disconnecting the session.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsSession::Disconnect(const RMessage2& aMessage)
    {
    // close device as client may fail to call CloseActuator()
    if ( iService )
        {
        TRAP_IGNORE( iService->ExecuteMessageL( aMessage ) );
        }
    CSession2::Disconnect( aMessage );
    }

   
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsSession::~CHWRMHapticsSession()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::~CHWRMHapticsSession()" ) ) );

    delete iService;
    iService = NULL;

    // reduce the amount of session the server is handling
    MyServer().DropSession();


    COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::~CHWRMHapticsSession - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Calls request handling functions. Also traps any leaves and signals 
// client if error occurs.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsSession::ServiceL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::ServiceL(0x%x)" ), aMessage.Handle() ) );

    // Reset completion need flag
    iCompletionNeeded = ETrue;
    
    TRAPD( error, DispatchMessageL( aMessage ) );

    if ( error != KErrNone )
        {
        COMPONENT_TRACE( ( _L("CHWRMHapticsSession::ServiceL(): Error in DispatchMessageL: %d"), error ) );
        iCompletionNeeded = ETrue;
        }

    if ( iCompletionNeeded )
        {
        aMessage.Complete( error );
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::ServiceL - return" )) );
    }

// ---------------------------------------------------------------------------
// Handles commands received in RMessage2.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsSession::DispatchMessageL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::DispatchMessageL(0x%x), client SID: 0x%x" ), aMessage.Function(), aMessage.SecureId().iId ) );

    // Check command code and call appropriate function
    if ( EHWRMHapticsService == aMessage.Function() )
        {
        if ( iService )
            {
            User::Leave( KErrAlreadyExists );
            }

        // Initialize haptics. This will initialize plugin manager, 
        // recervation handler and common data
        MyServer().InitHaptics();

        // get a pointer to the plugin manager
        CHWRMHapticsPluginManager* pluginHandler =
            MyServer().PluginManager();

        // get a pointer to the reservation handler
        CHWRMHapticsReservationHandler* reservationHandler = 
            MyServer().ReservationHandler();

        // get a pointer to the common data
        CHWRMHapticsCommonData* hapticsCommonData = 
            MyServer().HapticsCommonData();
        
        // if handlers or common data is NULL, this resource is not supported 
        // by device
        if ( !pluginHandler || !reservationHandler || !hapticsCommonData )
            {
            User::Leave( KErrNotSupported );
            }
        else
            {
            COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::DispatchMessageL - creating iService" )) );

            // create haptics service
            iService = CHWRMHapticsService::NewL( pluginHandler, 
                                                  reservationHandler, 
                                                  *hapticsCommonData,
                                                  aMessage );
            }
        }
    else // Cannot identify the message, it must be for service.
        {
        if ( iService )
            {
            iCompletionNeeded = iService->ExecuteMessageL( aMessage );
            }
        else
            {
            // iService not yet created
            User::Leave( KErrBadHandle );
            }
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsSession::DispatchMessageL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Convenience method for returning a reference to the Server object that 
// created this session
// ---------------------------------------------------------------------------
//
CHWRMHapticsServer& CHWRMHapticsSession::MyServer()
    {
    return *static_cast<CHWRMHapticsServer*>( 
        const_cast<CServer2*>( CSession2::Server() ) );
    }

//  End of File  
