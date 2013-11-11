/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Main functionality of CHeadsetStatusObserver
*
*
*/


#ifdef __ACCESSORY_FW

// INCLUDE FILES
#include "CHeadsetStatusObserver.h"
#include "HeadsetStatus.h"

#include <AccessoryMode.h>

// UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    //  CONSTANTS
    #ifdef _DEBUG
    _LIT( KPanicCat, "HeadsetStatusObserver" );
    #endif
    //  DATA TYPES
    enum TPanicCode
        {
        EAlreadyObserving,
        EInvalidObserver
        };
    }

// ========================= MEMBER FUNCTIONS ================================
   
// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
CHeadsetStatusObserver::CHeadsetStatusObserver( RAccessoryMode& aAccMode ):
    CActive( CActive::EPriorityStandard ),
    iAccMode( aAccMode )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusObserver::NewL
// 
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
CHeadsetStatusObserver* CHeadsetStatusObserver::NewL( RAccessoryMode& aAccMode )
    {
    CHeadsetStatusObserver* self = new ( ELeave )
        CHeadsetStatusObserver( aAccMode );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusObserver::ConstructL
// 
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
void CHeadsetStatusObserver::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
CHeadsetStatusObserver::~CHeadsetStatusObserver()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusObserver::SetObserver
// ---------------------------------------------------------------------------
void CHeadsetStatusObserver::SetObserver( MHeadsetStatusObserver* aObserver )
    {
    __ASSERT_DEBUG( aObserver, User::Panic( KPanicCat, EInvalidObserver ) );
    
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusObserver::StartObserving
// ---------------------------------------------------------------------------
void CHeadsetStatusObserver::StartObserving()
    {
    __ASSERT_DEBUG( !IsActive(), User::Panic( KPanicCat, EAlreadyObserving ) );
    
    // Set iStatus to pending and request notifications
    iAccMode.NotifyAccessoryModeChanged( iStatus, iAccPolMode );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusObserver::RunL
// ---------------------------------------------------------------------------
void CHeadsetStatusObserver::RunL()
    {
    TInt err( iStatus.Int() );
    if( err == KErrNone )
        {
        // Notify observer
        NotifyObserverL();
        }
    else
        {
        // Notify error
        NotifyObserverError( err );
        }
        
    // continue observing
    StartObserving();
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusObserver::DoCancel
// ---------------------------------------------------------------------------
void CHeadsetStatusObserver::DoCancel()
    {
    // cancel acc mode changed notifications
    iAccMode.CancelNotifyAccessoryModeChanged();
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusObserver::RunError
// ---------------------------------------------------------------------------
TInt CHeadsetStatusObserver::RunError( TInt aError )
    {
    // error notified to observer
    NotifyObserverError( aError );
    
    // continue observing
    StartObserving();
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusObserver::NotifyObserverL
// ---------------------------------------------------------------------------
void CHeadsetStatusObserver::NotifyObserverL()
    {
    __ASSERT_DEBUG( iObserver, User::Panic( KPanicCat, EInvalidObserver ) );
    
    TBool connected( EFalse );
    
    switch( iAccPolMode.iAccessoryMode )
        {
        // Headset connected
        case EAccModeWiredHeadset:
        case EAccModeWirelessHeadset:
        case EAccModeLoopset:
        case EAccModeHeadphones:
            {
            connected = ETrue;
            break;
            }
        // Headset disconnected
        case EAccModeHandPortable:
        case EAccModeWiredCarKit:
        case EAccModeWirelessCarKit:
        case EAccModeTextDevice:
        case EAccModeMusicStand:
        default:
            {
            // nothing to do
            break;
            }
        }
    
    // notify observer
    iObserver->HeadsetStatusChangedL( connected );
    }
    
// ---------------------------------------------------------------------------
// CHeadsetStatusObserver::NotifyObserverError
// ---------------------------------------------------------------------------
void CHeadsetStatusObserver::NotifyObserverError( TInt aError )
    {
    __ASSERT_DEBUG( iObserver, User::Panic( KPanicCat, EInvalidObserver ) );
    
    iObserver->HeadsetStatusError( aError );
    }
#endif  // __ACCESSORY_FW

// End of File
