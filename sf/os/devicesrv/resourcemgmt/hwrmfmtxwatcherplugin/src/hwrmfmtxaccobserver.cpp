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
* Description:  Accessory observer implementation for fmtx watcher plugin.
*
*/


// INCLUDE FILES
#include <accessoryservicesinternalpskeys.h>
#include "hwrmfmtxaccobserver.h"
#include "trace.h"

// ============================ LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// AccessoryConnected()
// -----------------------------------------------------------------------------
//
static TBool AccessoryConnected( TInt aWiredConnStatus )
    {
    TBool wiredConnected( ETrue );
    
    if( aWiredConnStatus == EAccSrvcsWiredConnUninitialized ||
        aWiredConnStatus == EAccSrvcsWiredConnDisconnected )
        {
        wiredConnected = EFalse;
        }
    
    return wiredConnected;
    }

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMFmtxAccObserver* CHWRMFmtxAccObserver::NewL( MHWRMFmtxConnObserverCallback& aCallback )
    {
    FUNC_LOG;
    
    CHWRMFmtxAccObserver* self = new( ELeave ) CHWRMFmtxAccObserver( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::~CHWRMFmtxAccObserver()
// -----------------------------------------------------------------------------
//
CHWRMFmtxAccObserver::~CHWRMFmtxAccObserver()
    {
    FUNC_LOG;
    iProperty.Cancel();
    Cancel();
    iProperty.Close();
    }    

// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::StartObservingL
// -----------------------------------------------------------------------------
//
TBool CHWRMFmtxAccObserver::GetStatusL()
    {
    FUNC_LOG;
      
    TInt wiredConnStatus( EAccSrvcsWiredConnUninitialized );        
    TInt err( iProperty.Get( KPSUidAccessoryServices,
                             KAccSrvcsWiredConnection,
                             wiredConnStatus ) ); 

    LOG_IF_ERROR1( err, "CHWRMFmtxAccObserver::GetStatusL: err=%d", err );
    
    User::LeaveIfError( err );
    
    return AccessoryConnected( wiredConnStatus );
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::StartObservingL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxAccObserver::StartObservingL()
    {
    FUNC_LOG;
    
    if( IsActive() )
        {
        ERROR_LOG( "CHWRMFmtxAccObserver::StartObservingL: already active" );
        }
    else
        {
        OrderAccessoryNotification();
        
        iConnected = GetStatusL(); // only connection change will be notified
        }        
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::StopObserving
// -----------------------------------------------------------------------------
//
void CHWRMFmtxAccObserver::StopObserving()
    {
    FUNC_LOG;
    
    Cancel();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::CHWRMFmtxAccObserver()
// -----------------------------------------------------------------------------
//
CHWRMFmtxAccObserver::CHWRMFmtxAccObserver( MHWRMFmtxConnObserverCallback& aCallback ) :
    CActive( EPriorityStandard ),
    iCallback( aCallback )
    {
    FUNC_LOG;
    
    CActiveScheduler::Add( this );
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxAccObserver::ConstructL()
    {
    FUNC_LOG;
    
    // Connect to accessory server
    TInt err = iProperty.Attach( KPSUidAccessoryServices, KAccSrvcsWiredConnection );
    
    LOG_IF_ERROR1( err, "CHWRMFmtxAccObserver::ConstructL: RProperty::Attach err=%d", err );

    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::OrderAccessoryNotification()
// -----------------------------------------------------------------------------
//
void CHWRMFmtxAccObserver::OrderAccessoryNotification()
    {
    FUNC_LOG;
    
    iProperty.Subscribe( iStatus );
    SetActive();
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::RunL()
// -----------------------------------------------------------------------------
//
void CHWRMFmtxAccObserver::RunL()
    {
    FUNC_LOG;
    
    TInt status( iStatus.Int() );
    
    LOG_IF_ERROR1( status, "CHWRMFmtxAccObserver::RunL: error, status=%d", status );
    
    // prevent looping due to immediate completion
    if ( status != KErrCancel )
        {
        OrderAccessoryNotification();    
        }
    
    TInt wiredConnStatus( EAccSrvcsWiredConnUninitialized );
    TInt err = iProperty.Get( wiredConnStatus );
    TBool connected( AccessoryConnected( wiredConnStatus ) );
    
    INFO_LOG2( "CHWRMFmtxAccObserver::RunL: err=%d, wiredConnStatus=%d", err, wiredConnStatus );
    
    if ( err == KErrNotFound || err == KErrNone )
        {
        INFO_LOG2( "CHWRMFmtxAccObserver::RunL: connected=%d, iConnected=%d", connected, iConnected );
        
        if ( connected != iConnected )
            {
            iCallback.HandleConnectionChange( EFmtxWatcherObserverAccessory, connected );    
            iConnected = connected; // store the status to be used on next completion    
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmtxAccObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CHWRMFmtxAccObserver::DoCancel()
    {
    FUNC_LOG;

    iProperty.Cancel();
    }

//  End of File  
