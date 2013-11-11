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
* Description:  Main functionality of AccMonitorObserver
*
*/


#include <AccMonitor.h>
#include <AccMonitorInfo.h>
#include <AccessoryConnection.h>
#include "AccMonitorObserver.h"
#include "AccMonitorCapMapper.h"
#include "AccMonitorContainer.h"
#include "acc_debug.h"

// ======== MEMBER FUNCTIONS ========
   
// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
CAccMonitorObserver::CAccMonitorObserver(
    RAccessoryConnection& aAccConnection )
    :CActive( CActive::EPriorityStandard ),
     iAccConnection( aAccConnection )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::CAccMonitorObserver()" );
    
    CActiveScheduler::Add( this );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::CAccMonitorObserver - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::NewL
// ---------------------------------------------------------------------------
CAccMonitorObserver* CAccMonitorObserver::NewL( 
             RAccessoryConnection& aConnection )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::NewL()" );
    
    CAccMonitorObserver* self = 
        new ( ELeave ) CAccMonitorObserver( aConnection );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::NewL - return self" );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CAccMonitorObserver::~CAccMonitorObserver()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::~CAccMonitorObserver()" );
    if( IsActive() )
        {
        Cancel();
        }
    if( iAccessoryInfo )
        {
        delete iAccessoryInfo;
        }
    TInt count = iAccMonConnectedAccessories.Count();    
    iAccMonConnectedAccessories.ResetAndDestroy();
    iAccessoryCapArray.Close();
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::~CAccMonitorObserver - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::StartObserverL
// 
// Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorObserver::StartObserverL(
     MAccMonitorObserver* aObserver,
     RConnectedAccessories& aAccMonConnectedAccessories )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::StartObserverL() - All accessories" );
    
    iAmountAccessories = AmountOfAccessories();
    iObserver = aObserver;
    iAccMonConnectedAccessories.ResetAndDestroy();
    for( TInt i = 0; i != aAccMonConnectedAccessories.Count(); i++ )
      {
      CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
      info->CopyL( aAccMonConnectedAccessories[ i ] );
      iAccMonConnectedAccessories.AppendL( info );
      CleanupStack::Pop( info );
      }
    // Request notifications from new accessory connection status changes
    iAccConnection.NotifyAccessoryConnectionStatusChanged( iStatus,
                                 iGenericIdArray );
    SetActive();
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::StartObserverL() - All accessories - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::StartObservingL
// 
// Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorObserver::StartObservingL(
     MAccMonitorObserver* aObserver,
     RConnectedAccessories& aAccMonConnectedAccessories )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::StartObservingL() - All accessories" );
    
    StartObserverL( aObserver, aAccMonConnectedAccessories );
    iObserverFlag = EEveryConnect;
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::StartObservingL() - All accessories - return" );
    }
  
// ---------------------------------------------------------------------------
// CAccMonitorObserver::StartObservingL
// 
// Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorObserver::StartObservingL( 
     MAccMonitorObserver* aObserver,
     const RAccMonCapabilityArray& aArray,
     RConnectedAccessories& aAccMonConnectedAccessories,
     TBool aInternalCall )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::StartObservingL() - Accessories types" );
    
    if ( !aInternalCall )
        {
        iAccessoryCapArray.Reset();
        for( TInt i = 0; i != aArray.Count(); i++ )
            {
            iAccessoryCapArray.AppendL( aArray[ i ] );
            }
        }
    iObserverFlag = EAccessoryType;
    StartObserverL( aObserver, aAccMonConnectedAccessories );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::StartObservingL() - Accessories types - return" );
    }
  
// ---------------------------------------------------------------------------
// CAccMonitorObserver::StartObservingL
// 
// Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorObserver::StartObservingL( 
     MAccMonitorObserver* aObserver,
     const CAccMonitorInfo* aInfo,
     RConnectedAccessories& aAccMonConnectedAccessories )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::StartObservingL() - Specific accessories" );
    
    if ( iAccessoryInfo == NULL )
        {
        iAccessoryInfo = CAccMonitorInfo::NewL( aInfo );            
        }
	else
        {
        iAccessoryInfo->CopyL ( aInfo );        
        }
        
    iObserverFlag = ESpecificAccessory;
    
    StartObserverL( aObserver, aAccMonConnectedAccessories );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::StartObservingL() - Specific accessories - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::RunL
// 
// Called in accessory connection/disconnection.
// ---------------------------------------------------------------------------
void CAccMonitorObserver::RunL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::RunL()" );
  
    TInt err( iStatus.Int() );
    if( err == KErrNone )
        {
        // Notify observer
        NotifyObserverL();
        }
    else
        {
        NotifyObserverError( err );
        }
    // Continue observing until stopped or error
    RConnectedAccessories accMonConnectedAccessories;
    CleanupResetAndDestroyPushL( accMonConnectedAccessories );
    GetConnectedAccessoriesL( accMonConnectedAccessories );
    
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorObserver::RunL - iObserverFlag %d", iObserverFlag );
    
    switch( iObserverFlag )
        {
        case EEveryConnect:
            {
            COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::RunL - Starts observing all accessories" );
            StartObservingL( iObserver, accMonConnectedAccessories );
            break;
            }
        case ESpecificAccessory:
            {
            COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::RunL - Starts observing specific accessories" );
            StartObservingL( iObserver, iAccessoryInfo, accMonConnectedAccessories );
            break;
            }
        case EAccessoryType:
            {
            COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::RunL - Starts observing accessory types" );
            StartObservingL( iObserver, iAccessoryCapArray, accMonConnectedAccessories, ETrue );
            break;
            }
        default:
            {
            User::Leave( KErrGeneral );
            break;
            }
        }
    CleanupStack::PopAndDestroy( &accMonConnectedAccessories );
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::RunL - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::DoCancel
// 
// Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorObserver::DoCancel()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::DoCancel()" );
    // Cancel listening to accessory server
    iAccConnection.CancelNotifyAccessoryConnectionStatusChanged();
    
    //Reset members depending on Client provided input data.
    if ( iAccessoryInfo )
        {
        iAccessoryInfo->Reset ();
        }
    
    iObserver = NULL;
    iAccessoryCapArray.Reset ();
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::DoCancel - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::RunError
// 
// Other items commented in the header file.
// ---------------------------------------------------------------------------
TInt CAccMonitorObserver::RunError( TInt aError )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::RunError()" );
  
    TInt err = aError;
    if( err != KErrNone )
        {
        NotifyObserverError( err );
        }
      
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorObserver::RunError - return err %d", err );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::NotifyObserverL
// 
// Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorObserver::NotifyObserverL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverL()" );
    
    TInt val = AmountOfAccessories();
    // Check if it a was a connect or a disconnect
    if( val > iAmountAccessories )
        {
        iConnect = ETrue;
        }
    if( val < iAmountAccessories )
        {
        iConnect = EFalse;
        }
    if( val == iAmountAccessories )
        {
        // Some accessory was connected/disconnected, so we can't get here
        User::Leave( KErrGeneral );
        }
    
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverL - Amount of Accessories before %d", iAmountAccessories );
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverL - Amount of Accessories after %d", val );
    
    iAmountAccessories = val;
    
    // Check if this the connected accessory was the one that was listened.
    TBool notifyObserver = CheckIfNotifyL();
      
    if( notifyObserver )
        {
        CAccMonitorInfo* accessoryInfoReturn = CAccMonitorInfo::NewLC();
        RConnectedAccessories accessoriesArray;
        CleanupResetAndDestroyPushL( accessoriesArray );
        GetConnectedAccessoriesL( accessoriesArray );
        if( iConnect )
            {
            if( accessoriesArray.Count() != 0 )
                {
                accessoryInfoReturn->CopyL( accessoriesArray[ 0 ] );
                COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverL - Notify connected" );
                COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverL - iAccessoryInfoReturn %d", accessoryInfoReturn->AccDeviceType() );
                iObserver->ConnectedL( accessoryInfoReturn );
                }
            }
        else
            {
            GetDisconnectedAccessoryL( *accessoryInfoReturn );
            COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverL - Notify disconnected" );
            COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverL - iAccessoryInfoReturn %d", accessoryInfoReturn->AccDeviceType() );
            iObserver->DisconnectedL( accessoryInfoReturn );
            }
        CleanupStack::PopAndDestroy( &accessoriesArray );
        CleanupStack::PopAndDestroy( accessoryInfoReturn );
        }
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverL - return" );
    }
  
// ---------------------------------------------------------------------------
// CAccMonitorObserver::NotifyObserverError
// 
// Notify observer in error situations.
// ---------------------------------------------------------------------------
void CAccMonitorObserver::NotifyObserverError( TInt aError )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverError()" );
    
    iObserver->AccMonitorObserverError( aError );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::NotifyObserverError - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::AmountOfAccessories
// 
// Get the amount of connected accessories. Other items commented in header.
// ---------------------------------------------------------------------------
TInt CAccMonitorObserver::AmountOfAccessories()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::AmountOfAccessories()" );
  
    TAccPolGenericIDArray connectedAccessories;
    iAccConnection.GetAccessoryConnectionStatus( connectedAccessories );
    TInt amountOfAccessories = connectedAccessories.Count();
    
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorObserver::AmountOfAccessories - return amountOfAccessories %d", amountOfAccessories );
    return amountOfAccessories;
    }
  
// ---------------------------------------------------------------------------
// CAccMonitorObserver::CheckIfNotify
// 
// Check if observer needs to be notified. Other items commented in header.
// ---------------------------------------------------------------------------
TBool CAccMonitorObserver::CheckIfNotifyL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::CheckIfNotifyL()" );
    TBool retVal( ETrue );
    switch( iObserverFlag )
        {
        case EEveryConnect:
            {
            COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::CheckIfNotify - Get every accessory" );
            retVal = ETrue;
            break;
            }
        case ESpecificAccessory:
            {
            COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::CheckIfNotify - Get specific accessory" );
            RConnectedAccessories con;
            CleanupResetAndDestroyPushL( con );
            CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
            if( iConnect )
                {
                GetConnectedAccessoriesL( con );
                if( con.Count() != 0 )
                    {
                    info->CopyL( con[ 0 ] );
                    }
                }
            else
                {
                GetDisconnectedAccessoryL( *info );
                }
            if( !( info->CompareL( iAccessoryInfo ) ) )
                {
                retVal = EFalse;
                }
            CleanupStack::PopAndDestroy( info );
            CleanupStack::PopAndDestroy( &con );
            break;
            }
        
        case EAccessoryType:
            {
            CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
            COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::CheckIfNotify - Get accessory type" );
            if( !iConnect )
                {
                GetDisconnectedAccessoryL( *info );
                retVal = info->Exists( iAccessoryCapArray );
                }
          
            if( ( iAccessoryCapArray.Count() != 0 ) && iConnect )
                {
                RConnectedAccessories con;
                CleanupResetAndDestroyPushL( con );
                GetConnectedAccessoriesL( con );
                if( con.Count() != 0 )
                    {
                    info->CopyL( con[ 0 ] );
                    }
                retVal = info->Exists( iAccessoryCapArray );
                CleanupStack::PopAndDestroy( &con );
                }
            CleanupStack::PopAndDestroy( info );
            break;
            }
        default:
            {
            User::Leave( KErrGeneral );
            break;
            }
        }
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorObserver::CheckIfNotify - return retVal %d", retVal );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::GetDisconnectedAccessoryL
// 
// Gets the accessory that was disconnected. Other items commented in header.
// ---------------------------------------------------------------------------
void CAccMonitorObserver::GetDisconnectedAccessoryL( 
    CAccMonitorInfo& aAccMonitorInfo )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::GetDisconnectedAccessoryL()" );
    
    RConnectedAccessories accMonConnectedAccessories;
    CleanupResetAndDestroyPushL( accMonConnectedAccessories );
    GetConnectedAccessoriesL( accMonConnectedAccessories );
    // Go through all the accessories the are now connected
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
    CAccMonitorInfo* info1 = CAccMonitorInfo::NewLC();
    for( TInt i = 0; i != iAccMonConnectedAccessories.Count(); i++ )
        {
        info->CopyL( iAccMonConnectedAccessories[ i ] );
        TBool flagFound( EFalse );
        // Go through all the accessories the were previously connected
        for( TInt p = 0; p != accMonConnectedAccessories.Count(); p++ )
            {
            info1->CopyL( accMonConnectedAccessories[ p ] );
            if( info->CompareL( info1 ) )
                {
                flagFound = ETrue;
                break;
                }
            }
        // If no matching accessory was found it is the one that was disconnected.
        if( !flagFound )
            {
            COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::GetDisconnectedAccessoryL - Matching accessory was found" );
            COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorObserver::GetDisconnectedAccessoryL - Type: %d", info->AccDeviceType() );
            aAccMonitorInfo.CopyL( info );
            }
        }
    CleanupStack::PopAndDestroy( info1 );
    CleanupStack::PopAndDestroy( info );
    if( accMonConnectedAccessories.Count() == 0 )
        {
        aAccMonitorInfo.CopyL( iAccMonConnectedAccessories[ 0 ] );
        }
    CleanupStack::PopAndDestroy( &accMonConnectedAccessories );
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorObserver::GetDisconnectedAccessoryL - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorObserver::GetConnectedAccessoriesL
// 
// Gets connected accessories. Other items commented in header.
// ---------------------------------------------------------------------------  
void CAccMonitorObserver::GetConnectedAccessoriesL( RConnectedAccessories& aConnectedAccessories )
    {
    CAccMonitor* acc = CAccMonitor::NewL();
    CleanupStack::PushL( acc );
    acc->GetConnectedAccessoriesL( aConnectedAccessories );
    CleanupStack::PopAndDestroy( acc );
    }

// End of file
