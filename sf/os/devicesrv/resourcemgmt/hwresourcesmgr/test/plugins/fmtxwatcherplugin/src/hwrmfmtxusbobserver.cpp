// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#include "hwrmfmtxusbobserver.h"
#include "trace.h"

// bitmask for requesting notification of every USB device state change
const TUint KUsbAllStates = 0xFFFFFFFF;

// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// UsbConnected()
// -----------------------------------------------------------------------------
//
static TBool UsbConnected( TUsbDeviceState aDeviceState )
    {
    TBool connected( ETrue );
    
    if ( aDeviceState == EUsbDeviceStateUndefined )
        {
        connected = EFalse;
        }
    
    return connected;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::NewL()
// -----------------------------------------------------------------------------
//
CHWRMFmtxUsbObserver* CHWRMFmtxUsbObserver::NewL( MHWRMFmtxConnObserverCallback& aObserver )
    {
    FUNC_LOG;
    
    CHWRMFmtxUsbObserver* self = new( ELeave ) CHWRMFmtxUsbObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::~CHWRMFmtxUsbObserver()
// -----------------------------------------------------------------------------
//
CHWRMFmtxUsbObserver::~CHWRMFmtxUsbObserver()
    {
    FUNC_LOG;
    
    Cancel();
    iUsbMan.Close();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::GetStatusL
// -----------------------------------------------------------------------------
//
TBool CHWRMFmtxUsbObserver::GetStatusL()
    {
    FUNC_LOG;
    
    TUsbDeviceState deviceState;
    TInt err( iUsbMan.GetDeviceState( deviceState ) );
    
    LOG_IF_ERROR2( err, "CHWRMFmtxUsbObserver::StartObservingL - err %d, state %d", err, deviceState );

    User::LeaveIfError( err );    	
    
    return UsbConnected( deviceState );
    }
    

// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::StartObservingL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxUsbObserver::StartObservingL()
    {
    FUNC_LOG;
    
    if( !IsActive() )
        {
        OrderUsbNotification();
        
        // check the initial status, so that only state change is notified
        iConnected = GetStatusL();
        }
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::StopObserving
// -----------------------------------------------------------------------------
//
void CHWRMFmtxUsbObserver::StopObserving()
    {
    FUNC_LOG;
    
    Cancel();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::CHWRMFmtxUsbObserver()
// -----------------------------------------------------------------------------
//
CHWRMFmtxUsbObserver::CHWRMFmtxUsbObserver( MHWRMFmtxConnObserverCallback& aObserver ) :
    CActive( EPriorityStandard ),
    iDeviceState( EUsbDeviceStateUndefined ),
    iCallback( aObserver ),
    iConnected( ETrue )
    {
    FUNC_LOG;
    
    CActiveScheduler::Add( this );
    }
            
// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxUsbObserver::ConstructL()
    {
    FUNC_LOG;
    
    // Connect to usbman
	TInt err = iUsbMan.Connect();	
    
    LOG_IF_ERROR1( err, "CHWRMFmtxUsbObserver::ConstructL - err %d", err );
    
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::OrderUsbNotification()
// -----------------------------------------------------------------------------
//
void CHWRMFmtxUsbObserver::OrderUsbNotification()
    {
    FUNC_LOG;
    
    iUsbMan.DeviceStateNotification( KUsbAllStates, iDeviceState, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::RunL()
// -----------------------------------------------------------------------------
//
void CHWRMFmtxUsbObserver::RunL( )
    {
    FUNC_LOG;
    
    TInt status(iStatus.Int());
    
    LOG_IF_ERROR1( status, "CHWRMFmtxUsbObserver::RunL error, status=%d", status );

    TBool connected(UsbConnected( iDeviceState )); // store state before subscribing again
    
    // prevent looping due to immediate completion
    if ( status != KErrCancel && status != KErrServerTerminated )
        {
        OrderUsbNotification();    
        }
    
    if ( status == KErrNone )
        {
        INFO_LOG2( "CHWRMFmtxUsbObserver::RunL: connected=%d, iConnected=%d", connected, iConnected );
        if ( connected != iConnected ) // notify only if state has changed
            {
            iConnected = connected;    
            iCallback.HandleConnectionChange( EFmtxWatcherObserverUsb, iConnected );
            }    
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxUsbObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CHWRMFmtxUsbObserver::DoCancel()
    {
    FUNC_LOG;

    iUsbMan.DeviceStateNotificationCancel();
    }

//  End of File  
