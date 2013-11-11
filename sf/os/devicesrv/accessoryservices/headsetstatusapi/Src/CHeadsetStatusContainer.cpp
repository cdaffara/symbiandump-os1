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
*     Main functionality of CHeadsetStatusContainer
*
*
*/


// INCLUDE FILES
#include "CHeadsetStatusContainer.h"

#ifdef __ACCESSORY_FW
#include <AccPolAccessoryMode.h>
#include "CHeadsetStatusObserver.h"
#endif

// ========================= MEMBER FUNCTIONS ================================
   
// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
CHeadsetStatusContainer::CHeadsetStatusContainer( )
    {
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusContainer::NewL
// 
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
CHeadsetStatusContainer* CHeadsetStatusContainer::NewL( )
    {
    CHeadsetStatusContainer* self = new ( ELeave ) CHeadsetStatusContainer( );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusContainer::ConstructL
// 
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
void CHeadsetStatusContainer::ConstructL()
    {
    #ifdef __ACCESSORY_FW
    User::LeaveIfError( iAccSrv.Connect() );
    User::LeaveIfError( iAccMode.CreateSubSession( iAccSrv ) );
    iObserver = CHeadsetStatusObserver::NewL( iAccMode );
    #else
    User::Leave( KErrNotSupported );
    #endif
    }

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
CHeadsetStatusContainer::~CHeadsetStatusContainer()
    {
    #ifdef __ACCESSORY_FW
    // will cancel itself when deleted
    // must remain before closing iAccMode!
    delete iObserver;
    iAccMode.CloseSubSession();
    iAccSrv.Disconnect();
    #endif
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusContainer::QueryHeadsetStatus
// ---------------------------------------------------------------------------
TInt CHeadsetStatusContainer::QueryHeadsetStatus( TBool& aState )
    {
    TInt err( KErrNone );
    
    #ifdef __ACCESSORY_FW
    // Accessory framework in use
    TAccPolAccessoryMode accMode;
    err = iAccMode.GetAccessoryMode( accMode );
    if( err == KErrNone )
        {
        switch( accMode.iAccessoryMode )
            {
            // Headset connected
            case EAccModeWiredHeadset:
            case EAccModeWirelessHeadset:
            case EAccModeLoopset:
            case EAccModeHeadphones:
                {
                aState = ETrue;
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
                aState = EFalse;
                break;
                }
            }
        }
    #endif
    
    return err;
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusContainer::SetObserver
// ---------------------------------------------------------------------------
void CHeadsetStatusContainer::SetObserver( MHeadsetStatusObserver* aObserver )
    {
    #ifdef __ACCESSORY_FW
    iObserver->SetObserver( aObserver );
    #endif
    }
    
// ---------------------------------------------------------------------------
// CHeadsetStatusContainer::StartObserving
// ---------------------------------------------------------------------------
void CHeadsetStatusContainer::StartObserving()
    {
    #ifdef __ACCESSORY_FW
    iObserver->StartObserving();
    #endif
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusContainer::StopObserving
// ---------------------------------------------------------------------------
void CHeadsetStatusContainer::StopObserving()
    {
    #ifdef __ACCESSORY_FW
    iObserver->Cancel();
    #endif
    }

// ---------------------------------------------------------------------------
// CHeadsetStatusContainer::IsObserving
// ---------------------------------------------------------------------------
TBool CHeadsetStatusContainer::IsObserving() const
    {
    TBool observing( EFalse );
    
    #ifdef __ACCESSORY_FW
    observing = iObserver->IsActive();
    #endif
    
    return observing;
    }

// End of File
