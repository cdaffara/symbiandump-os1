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
* Description:  Main functionality of AccMonitorContainer
*
*/


#include <AccMonitorInfo.h>
#include "AccMonitorContainer.h"
#include "AccMonitorCapMapper.h"
#include "acc_debug.h"

// ======== MEMBER FUNCTIONS ========
   
// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
CAccMonitorContainer::CAccMonitorContainer()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::CAccMonitorContainer()" );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::CAccMonitorContainer - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::NewL
// ---------------------------------------------------------------------------
CAccMonitorContainer* CAccMonitorContainer::NewL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::NewL()" );
    
    CAccMonitorContainer* self = new ( ELeave ) CAccMonitorContainer();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::NewL - return self" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccMonitor::ConstructL
// 
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
void CAccMonitorContainer::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::NewL()" );
    
    User::LeaveIfError( iAccSrv.Connect() );
    User::LeaveIfError( iAccessoryConnection.CreateSubSession( iAccSrv ) );
    iObserver = CAccMonitorObserver::NewL( iAccessoryConnection );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::NewL - return" );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CAccMonitorContainer::~CAccMonitorContainer()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::~CAccMonitorContainer()" );
    if( iObserver )
        {
        delete iObserver;
        iObserver = NULL;
        }
    iAccessoryConnection.CloseSubSession();
    iAccSrv.Disconnect();
    if( iConnectedAccessories )
        {
        delete iConnectedAccessories;
        iConnectedAccessories = NULL;
        }
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::~CAccMonitorContainer - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::GetAccessoriesFromAccServerL
// 
// Gets the connected accessories. Other items commented in the header file.
// ---------------------------------------------------------------------------
TInt CAccMonitorContainer::GetAccessoriesFromAccServerL()
    {
    TInt err( KErrNone );
    
    if( iConnectedAccessories )
        {
        delete iConnectedAccessories;
        iConnectedAccessories = NULL;
        }
    iConnectedAccessories = new( ELeave ) TAccPolGenericIDArray();
    err = iAccessoryConnection.GetAccessoryConnectionStatus( 
                          *iConnectedAccessories );
    
    return err;
    }

// ---------------------------------------------------------------------------
// CAccMonitor::GetConnectedAccessoriesL
// 
// Gets the connected accessories. Other items commented in the header file.
// ---------------------------------------------------------------------------
TInt CAccMonitorContainer::GetConnectedAccessoriesL( 
    RConnectedAccessories& aAccessoriesArray )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::GetConnectedAccessoriesL()" );
    TInt err( KErrNone );
    
    err = GetAccessoriesFromAccServerL();
    if( err == KErrNotReady )
        {
        User::Leave( KErrNotReady );
        }
    CAccMonitorCapMapper* mapper = CAccMonitorCapMapper::NewL();
    CleanupStack::PushL( mapper );
    CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
    CleanupStack::PushL( nameArray );
    TInt arrayIncrementer( 0 );
  
    while( ( iConnectedAccessories->Count() ) != arrayIncrementer )
        {
        CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
        TAccPolGenericID genID;
        genID = iConnectedAccessories->GetGenericIDL( arrayIncrementer );
        iAccessoryConnection.GetSubblockNameArrayL( genID, *nameArray );
        mapper->GetCapabilitiesFromGIDHeader( genID,
                            info->iAccCapabilityArray );
        mapper->GetCapabilitiesFromSubblockNameArray( nameArray,
                                info->iAccCapabilityArray );
        TInt counter( 0 );
        counter = info->Count();
        if( counter > 0 )
            {
            info->iAccessoryDeviceType = info->AccCapabilityAtIndex( 0 );
            info->iAccessoryPhysicalConnection = info->AccCapabilityAtIndex( 1 );
            info->iDeviceAddress = genID.DeviceAddress();
            info->iAccessoryID = arrayIncrementer;
            }
        arrayIncrementer++;
        err = aAccessoriesArray.Append( info );
        COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorContainer::GetConnectedAccessoriesL - Accessory Found with device type %d", info->iAccessoryDeviceType );
        CleanupStack::Pop( info );
        }
  
    CleanupStack::PopAndDestroy( nameArray );
    CleanupStack::PopAndDestroy( mapper );
    
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorContainer::GetConnectedAccessoriesL - return err %d", err );
    
    return aAccessoriesArray.Count();
    }

// ---------------------------------------------------------------------------
// CAccMonitor::GetValueForCapability
// 
// Gets a value for capability. Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorContainer::GetValueForCapabilityL( 
    const CAccMonitorInfo* aInfo, 
    const TUint32 aCapability, 
    TInt& aValue )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::GetValueForCapabilityL()" );
    // Check if accessory in question is still connected
    const TInt zeroConnected( 0 );
    RConnectedAccessories array;
    CleanupResetAndDestroyPushL( array );
    TInt retVal( 0 );
    TBool returnValue( EFalse );
    TInt amount( 0 );
    amount = GetConnectedAccessoriesL( array );
    if( amount == zeroConnected )
        {
        User::Leave( KErrArgument );
        }
    TBool isStillConnected( EFalse );
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
    for( TInt i = 0; i != amount; i++ )
        {
        info->CopyL( array[ i ] );
        if( info->CompareL( aInfo ) )
            {
            isStillConnected = ETrue;
            break;
            }
        }
    CleanupStack::PopAndDestroy( info );
    CleanupStack::PopAndDestroy( &array );
    
    CAccMonitorCapMapper* mapper = CAccMonitorCapMapper::NewL();
    CleanupStack::PushL( mapper );
    CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
    CleanupStack::PushL( nameArray );
    TUint32 genIdCapability( 0 );
    TAccValueTypeTInt gettedValue;
    TAccPolGenericID genID;
    TAccPolNameRecord name;
    
    // Get the capability value
    if( ( isStillConnected ) && ( aInfo->iAccessoryID >= 0 ) )
        {
        genID = iConnectedAccessories->GetGenericIDL( aInfo->iAccessoryID );
        iAccessoryConnection.GetSubblockNameArrayL( genID, *nameArray );
        if( aCapability < KAccMonStereoAudio )
            {
            User::Leave( KErrNotFound );
            }
        else
            {
            genIdCapability = mapper->GetCapabilityFromDefinitionL( aCapability );
            }
        if( retVal != KErrNotFound )
            {
            returnValue = nameArray->HasName( genIdCapability );
            TRAPD( err1, name.SetNameL( genIdCapability ) );
            if( returnValue && ( err1 == KErrNone ) )
                {
                iAccessoryConnection.GetValueL( genID, name, gettedValue );
                aValue = gettedValue.iValue;
                }
            else
                {
                User::Leave( KErrNotFound );
                }
            }
        }
    else
        {
        User::Leave( KErrArgument );     
        }
    CleanupStack::PopAndDestroy( nameArray );
    CleanupStack::PopAndDestroy( mapper );
    
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorContainer::GetValueForCapabilityL - return aValue %d", aValue );
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorContainer::GetValueForCapabilityL - return retVal %d", retVal );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::StartObserving
// 
// Starts observing. Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorContainer::StartObservingL( 
    MAccMonitorObserver* aObserver )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::StartObservingL() - All accessory connections" );
    
    RConnectedAccessories accessories;
    CleanupResetAndDestroyPushL( accessories );
    TRAP_IGNORE( GetConnectedAccessoriesL( accessories ) );
    iObserver->StartObservingL( aObserver, accessories );
    CleanupStack::PopAndDestroy( &accessories );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::StartObserving - All accessory connections - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::StartObserving
// 
// Starts observing. Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorContainer::StartObservingL( 
    MAccMonitorObserver* aObserver,
    const CAccMonitorInfo* aInfo )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::StartObservingL() - Accessory type connections" );
  
    RConnectedAccessories accessories;
    CleanupResetAndDestroyPushL( accessories );
    TRAP_IGNORE( GetConnectedAccessoriesL( accessories ) );
    iObserver->StartObservingL( aObserver, aInfo, accessories );
    CleanupStack::PopAndDestroy( &accessories );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::StartObserving - Accessory type connections - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::StartObserving
// 
// Starts observing. Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorContainer::StartObservingL( 
    MAccMonitorObserver* aObserver, 
    const RAccMonCapabilityArray& aArray )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::StartObservingL() - Specific accessory connections" );
    
    RConnectedAccessories accessories;
    CleanupResetAndDestroyPushL( accessories );
    TRAP_IGNORE( GetConnectedAccessoriesL( accessories ) );
    iObserver->StartObservingL( aObserver, aArray, accessories, EFalse );
    CleanupStack::PopAndDestroy( &accessories );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::StartObserving - Specific accessory connections - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::StopObserving
// 
// Stops observing. Other items commented in the header file.
// ---------------------------------------------------------------------------
void CAccMonitorContainer::StopObserving()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::StopObserving()" );
    
    iObserver->Cancel();
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::StopObserving - return" );
    }
  
// ---------------------------------------------------------------------------
// CAccMonitor::IsObserving
// 
// Checks if observer is active. Other items commented in the header file.
// ---------------------------------------------------------------------------
TBool CAccMonitorContainer::IsObserving() const
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::IsObserving()" );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorContainer::IsObserving - return" );
    return iObserver->IsActive();
    }
  
// End of file
