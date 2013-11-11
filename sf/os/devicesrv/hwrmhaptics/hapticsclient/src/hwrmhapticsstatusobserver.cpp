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
* Description:  Haptics status observer implementation
*
*/


#include <hwrmhapticsactuatorobserver.h>

#include "hwrmhapticstrace.h"
#include "hwrmhapticssession.h"
#include "hwrmhapticsstatusobserver.h"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsStatusObserver* CHWRMHapticsStatusObserver::NewL( 
                            MHWRMHapticsObserver* aHapticsCallback,
                            MHWRMHapticsActuatorObserver* aActuatorCallback,
                            RHWRMHapticsSession* aClient )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::NewL(0x%x, 0x%x)" ), aHapticsCallback, aActuatorCallback ) );

    CHWRMHapticsStatusObserver* self = 
            new ( ELeave ) CHWRMHapticsStatusObserver( aHapticsCallback, 
                                                       aActuatorCallback,
                                                       aClient );

    CleanupStack::PushL( self );
    
    self->ConstructL();

    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsStatusObserver::~CHWRMHapticsStatusObserver()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::~CHWRMHapticsStatusObserver()" ) ) );

    Cancel();
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::~CHWRMHapticsStatusObserver - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Returns current haptics status.
// ---------------------------------------------------------------------------
//
MHWRMHapticsObserver::THWRMHapticsStatus 
CHWRMHapticsStatusObserver::CurrentStatus() const
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::CurrentStatus()" ) ) );
    
    return iHapticsStatus;
    }

// ---------------------------------------------------------------------------
// Handle notification from pubsub session.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsStatusObserver::RunL()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::RunL()" ) ) );

    // if status was received successfully, inform status to callbacks
    if ( iStatus == KErrNone )
        {
        // notify callbacks, if available
        if ( iStatusType == EHWRMHapticsActuatorStatus && iActuatorCallback )
            {
            // actuator event value
            TActuatorStatus status = 
                static_cast<TActuatorStatus>( iLastStatus );
            
            // notify actuator event
            iActuatorCallback->ActuatorEventL( status, iActuator );
            }
        else if ( iStatusType == EHWRMHapticsSessionStatus && iHapticsCallback )
            {
            // store haptics status value so that it can be fetched
            iHapticsStatus = static_cast<THapticsStatus>( iLastStatus );
            
            // notify haptics status change
            iHapticsCallback->HapticsStatusChangedL( iHapticsStatus );
            }

        // Reorder notification
        OrderNotification();
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::RunL - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// Handle error of RunL.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsStatusObserver::RunError( TInt /*aError*/ )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::RunError()" ) ) );
    
    // Error occurred while setting new status value using callback
    // --> ignore error
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handle cancel order on this active object.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsStatusObserver::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::DoCancel()" ) ) );
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsStatusObserver::CHWRMHapticsStatusObserver( 
                            MHWRMHapticsObserver* aHapticsCallback,
                            MHWRMHapticsActuatorObserver* aActuatorCallback,
                            RHWRMHapticsSession* aClient )
    : CActive( EPriorityStandard ),
      iHapticsCallback( aHapticsCallback ),
      iActuatorCallback( aActuatorCallback ),
      iClient ( aClient ), 
      iHapticsStatus( MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ),
      iStatusPckg( iLastStatus ),
      iStatusTypePckg( iStatusType ),
      iActuatorPckg( iActuator )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::CHWRMHapticsStatusObserver(0x%x, 0x%x)" ), aHapticsCallback, aActuatorCallback ) );

    CActiveScheduler::Add( this );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::CHWRMHapticsStatusObserver - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsStatusObserver::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::ConstructL()" ) ) );

    // activate the observation
    OrderNotification();

    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Order new notification.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsStatusObserver::OrderNotification()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::OrderNotification()" ) ) );

    if ( !IsActive() && iClient && iClient->Handle() )
        {
        // update status type
        iStatusType = NotificationStatusType();
        
        // request haptics status notification
        iClient->ExecuteAsyncOperation( EHWRMHapticsStatusNotification,
                                        TIpcArgs( iStatusType,
                                                  &iStatusTypePckg,
                                                  &iStatusPckg,
                                                  &iActuatorPckg ),
                                        iStatus );

        // activate object
        SetActive();
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsStatusObserver::OrderNotification - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Based on available callback instances, returns the type
// of status notification, which should be used in notification requests.
// ---------------------------------------------------------------------------
//
THWRMHapticsStatusTypes 
CHWRMHapticsStatusObserver::NotificationStatusType() const
    {
    THWRMHapticsStatusTypes statusType = EHWRMHapticsSessionStatus;
    
    // there is always at least one callback
    if ( iHapticsCallback && iActuatorCallback )
        {
        statusType = EHWRMHapticsBothStatus;
        }
    else if ( iActuatorCallback )
        {
        statusType = EHWRMHapticsActuatorStatus;
        }
    else // iHapticsCallback
        {
        statusType = EHWRMHapticsSessionStatus;
        }
    
    return statusType;
    }

// End of File
