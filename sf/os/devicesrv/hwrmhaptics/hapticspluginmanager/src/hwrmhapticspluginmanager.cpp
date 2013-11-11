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
* Description:  Haptics server plugin handler implementation.
*
*/


#include <ecom/ecom.h>
#include <hwrmhapticscommands.h>
#include <hwrmlogicalactuators.h>
#include <centralrepository.h>

#include "hwrmhapticspluginmanager.h"
#include "hwrmhapticsclientserver.h"
#include "hwrmhapticsservice.h"
#include "hwrmhapticstrace.h"
#include "hwrmhapticstransactiondata.h"
#include "hwrmhapticsinternalcrkeys.h"
#include "hwrmhapticsuid.h"

_LIT( KPanicCategory, "HWRMHapticsPluginManager" );

// HWRMHaptics uid file
_LIT( KHapticsUidFilename, "hwrmhapticsuid.ini" );

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsPluginManager* CHWRMHapticsPluginManager::NewL(
                                CHWRMHapticsCommonData& aHapticsCommonData, 
                                TInt aRequestTimeout )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::NewL()" ) ) );
    
    CHWRMHapticsPluginManager* self = 
        new ( ELeave ) CHWRMHapticsPluginManager( aHapticsCommonData, 
                                                  aRequestTimeout );
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginManager::~CHWRMHapticsPluginManager()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::~CHWRMHapticsPluginManager()" ) ) );
    
    if ( iPlugin )
        {
        // Cancel any ongoing requests
        while ( iTransactionList->FirstItem() )
            {
            COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::~CHWRMHapticsPluginManager - Canceling transaction %d" ), iTransactionList->FirstItem()->TransactionId() ) );
            TRAPD( err, CancelCommandL(
                        iTransactionList->FirstItem()->TransactionId() ) );

            if ( err != KErrNone )
                {
                COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::~CHWRMHapticsPluginManager - Canceling transaction %d failed" ), iTransactionList->FirstItem()->TransactionId() ) );
                }
            }

        delete iPlugin;
        iPlugin = NULL;
        }
        
    delete iPluginTimer;
    // delete transaction list
    delete iTransactionList;

    delete iHapticsUid;

    delete iRepository; 

    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::~CHWRMHapticsPluginManager - return " )) );
    }

// ---------------------------------------------------------------------------
// Handles plugin requests from sessions. Only one concurrent request 
// is supported.
// ---------------------------------------------------------------------------
//
TUint8 CHWRMHapticsPluginManager::ProcessCommandL( 
                                TInt aCommandId,
                                TDesC8& aData,
                                CHWRMHapticsService* aCompletionCallback  )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::ProcessCommandL(0x%x, <aData>, 0x%x)" ), aCommandId, aCompletionCallback ) );

    if ( !iPlugin )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::ProcessCommandL - No actuator!" ) ) );
        User::Leave( KErrBadHandle );
        }

    // Generate new transaction ID.
    iTransIdCounter++; 
    if ( iTransIdCounter == 0 )
        {
        // Counter will overflow back to zero when it hits max.
        // However, zero indicates completed transaction as return value, so 
        // increase counter again.
        iTransIdCounter++; 
        }
    
    // If we run out of transIds, return server is busy.
    if ( iTransactionList->FindTransaction( iTransIdCounter, EFalse ) )
        {
        User::Leave( KErrServerBusy );
        }

    // Create transaction data before call in case it leaves
    TTime obsoletionTime;
    obsoletionTime.UniversalTime();
    obsoletionTime += iRequestTimeout;
    CHWRMHapticsTransactionData* data = 
        new (ELeave) CHWRMHapticsTransactionData( aCompletionCallback, 
                                                  iTransIdCounter, 
                                                  aCommandId, 
                                                  obsoletionTime );

    // Push transaction data to cleanup stack so that it will clean 
    // out if ProcessCommandL leaves.
    CleanupStack::PushL( data );
    
    iPlugin->ProcessCommandL( aCommandId, data->TransactionId(), aData );
    
    TUint8 retval( 0 );

    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );
    
    // Add data to list
    iTransactionList->AddTransaction( data );

    retval = data->TransactionId();

    // Start  timer if it is not already started
    if ( !iPluginTimer->IsActive() )
        {
        iPluginTimer->Set( iRequestTimeout );
        }
        
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::ProcessCommandL - return 0x%x" ), retval ) );         
    
    return retval;
        
    }
    
// ---------------------------------------------------------------------------
// Cancels the currently executing request
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginManager::CancelCommandL( TUint8 aTransId )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::CancelCommandL()" )) );

    if ( !iPlugin )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::CancelCommandL - No actuator!" ) ) );
        User::Leave( KErrBadHandle );
        }
    
    // Find correct transaction data and remove it from list
    CHWRMHapticsTransactionData* data = 
        static_cast<CHWRMHapticsTransactionData*>(
            iTransactionList->FindTransaction( aTransId, ETrue ) );
   
    // cancel request timer if no more transactions open
    if ( iTransactionList->Count() == 0 )
        {
        iPluginTimer->Cancel();
        }

    // If transaction is not open, do nothing
    // Do not cancel if request has no callback (i.e. final destructor 
    // state restorings)
    // (these are canceled by timeout if they do not complete successfully)
    if ( data && data->CompletionCallback() )
        {            
        CleanupStack::PushL( data );
        
        iPlugin->CancelCommandL( data->TransactionId(), data->CommandId() );
        
        // Destroy the transaction data, since transaction is over.
        CleanupStack::PopAndDestroy( data );
        }
    else
        {
        if ( data )
            {
            COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::CancelCommandL - Not canceled because no callback" ) ) );
            
            // Push data back to list
            iTransactionList->AddTransaction( data );
            }
        else
            {
            COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::CancelCommandL - Not canceled because no transaction found" ) ) );
            }
        }
        
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::CancelCommandL - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// Called by the plugin to inform its state. Sends notification of the state
// to all listening clients.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginManager::PluginEnabled( 
                                    THWRMLogicalActuators aActuator,
                                    TBool aEnabled )
    {
    MHWRMHapticsActuatorObserver::THWRMActuatorEvents event = 
        MHWRMHapticsActuatorObserver::EHWRMActuatorDisabled;
    
    if ( aEnabled )
        {
        event = MHWRMHapticsActuatorObserver::EHWRMActuatorEnabled;
        }
    
    iHapticsCommonData.BroadcastActuatorEvent( event, aActuator );
    }

// ---------------------------------------------------------------------------
// Routes response from plugin to correct service instance
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginManager::ProcessResponseL( TInt aCommandId,
                                                  TUint8 aTransId,
                                                  const TDesC8& aData  )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::ProcessResponseL(0x%x, 0x%x, <aData> )" ), aCommandId, aTransId ) );

    // Find and remove correct transaction data and remove it from queue
    CHWRMHapticsTransactionData* data = 
        static_cast<CHWRMHapticsTransactionData*>(
            iTransactionList->FindTransaction( aTransId, ETrue ) );
    
    // cancel request timer if no more transactions open
    if ( iTransactionList->Count() == 0 )
        {
        iPluginTimer->Cancel();
        }

    // If transaction is not open, response not expected.
    if ( data )
        {
        CleanupStack::PushL( data );

        // Check that command ID is the expected one        
        if ( data->CommandId() != aCommandId )
            {
            COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::ProcessResponseL - Command ID mismatch, expected: 0x%x, got 0x%x" ), data->CommandId(), aCommandId ) );
            User::Leave( KErrBadHandle );
            }

        // Route data to callback service if one is needed
        if ( data->CompletionCallback() )
            {
            data->CompletionCallback()->ProcessResponseL( aCommandId, 
                                                         aTransId, 
                                                         aData );
            }
        
        CleanupStack::PopAndDestroy( data );
        data = NULL;
        }
    else
        {
        // There is problem in adaptation, as unexpected transaction was completed.
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::ProcessResponseL - No transaction data found!" ) ) );
        User::Leave( KErrBadHandle );
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::ProcessResponseL - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// Cancels all obsolete transactions. TimerId is irrelevant as only one timer.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginManager::GenericTimerFired( TInt /*aTimerId*/, 
                                                   TBool /*aCutOff*/ )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::GenericTimerFired()" ) ) );

    __ASSERT_ALWAYS( iPlugin, User::Panic( KPanicCategory, 
                                           EPanicBadHandle ) );

    // Since requests are added to end of list always, they are in order 
    // of obsolescense.
    // Cancel requests that are obsolete and notify callback service
    TInt err( KErrNone );
    TTime now;
    now.UniversalTime();

    CHWRMHapticsTransactionData* data = 
        static_cast<CHWRMHapticsTransactionData*>( 
            iTransactionList->FirstItem() );
    
    while ( data && ( data->TransactionObsoletionTime() < now ) )   
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::GenericTimerFired - Canceling obsolete transaction 0x%x" ), data->TransactionId() ) );

        // Cancel transaction. 
        TUint8 trId( data->TransactionId() );
        TRAP(err, iPlugin->CancelCommandL( trId, data->CommandId() ) );

        if ( err != KErrNone )
            {
            // Ignore errors as we cannot do anything about it 
            // anyway. Just trace.
            COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::GenericTimerFired - Canceling obsolete transaction FAILED (%d)!" ), err ) );
            }

        if ( data->CompletionCallback() )
            {
            data->CompletionCallback()->CancelRequest( trId );
            }

        // Destroy the transaction data, since transaction is over.
        iTransactionList->RemoveFirstItem();
        delete data;
        
        // get next data to check
        data = static_cast<CHWRMHapticsTransactionData*>(
                                    iTransactionList->FirstItem() );
        }

    // Restart timer if there is more transactions in list
    if ( data )
        {
        iPluginTimer->Set( iRequestTimeout );
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::GenericTimerFired - return" ) ) );
    }

// ---------------------------------------------------------------------------
// It retrieves the supported actuator information from plugins.
// ---------------------------------------------------------------------------
//
TUint32 CHWRMHapticsPluginManager::GetSupportedActuatorInfo()
    {
    return iSupActuators;
    }

// ---------------------------------------------------------------------------
// Opens the plugin appropriate for the actuator to be opened.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsPluginManager::OpenPluginToActuatorL( 
                                            THWRMLogicalActuators aActuator )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::OpenPluginToActuator - Actuator = %d " ), aActuator) );
    
    TBool created = EFalse;

    // check if actuator type is supported
    if ( !( aActuator & iSupActuators ) )
        {
        // not supported
        User::Leave( KErrNotSupported );
        }

    if ( !iPlugin )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::OpenPluginToActuator - Creating plugin instance." ) ) );

        // map actuator type to type cenrep id
        TInt adaptationCenrepId(0);
        switch( aActuator )
            {
            case EHWRMLogicalActuatorAny:
                {
                adaptationCenrepId = KHWRMHapticsPluginAnyAdaptation;
                break;
                }
            case EHWRMLogicalActuatorDevice:
                {
                adaptationCenrepId = KHWRMHapticsPluginDeviceAdaptation;
                break;
                }
            case EHWRMLogicalActuatorPrimaryDisplay:
                {
                adaptationCenrepId = KHWRMHapticsPluginPrimaryDisplayAdaptation;
                break;
                }
            case EHWRMLogicalActuatorSecondaryDisplay:
                {
                adaptationCenrepId = KHWRMHapticsPluginSecondaryDisplayAdaptation;
                break;
                }
            case EHWRMLogicalActuatorGame:
                {
                adaptationCenrepId = KHWRMHapticsPluginGameAdaptation;
                break;
                }
            case EHWRMLogicalActuatorGameLeft:
                {
                adaptationCenrepId = KHWRMHapticsPluginGameLeftAdaptation;
                break;
                }
            case EHWRMLogicalActuatorGameRight:
                {
                adaptationCenrepId = KHWRMHapticsPluginGameRightAdaptation;
                break;
                }
            case EHWRMLogicalActuatorExternalVibra:
                {
                adaptationCenrepId = KHWRMHapticsPluginExternalVibraAdaptation;
                break;
                }
            default:
                adaptationCenrepId = KHWRMHapticsPluginAnyAdaptation;
                break;
           }

        TInt adaptationPluginUid( 0 );
        User::LeaveIfError( iRepository->Get( adaptationCenrepId, 
                                              adaptationPluginUid ) );

        // create plugin instance with the given uid
        TRAPD( err, iPlugin = CHWRMHapticsPluginService::NewL( 
                                  TUid::Uid( adaptationPluginUid ), this ) );

        // if the plugin with given uid was not found, the 
        // actuator type is not supported
        if ( err == KErrNotFound )
            {
            err = KErrNotSupported;
            }
        
        User::LeaveIfError( err );
        
        created = ETrue;
        }
    
    return created;
    }

// ---------------------------------------------------------------------------
// Checks if automatic license setting is allowed. The actual check is done
// in CHWRMHapticsUid helper class.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsPluginManager::LicenseAutoSettingAllowed( 
    const RMessage2& aMessage )
    {
    return iHapticsUid->LicenseAutoSetAllowed( aMessage );
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginManager::CHWRMHapticsPluginManager( 
                                CHWRMHapticsCommonData& aHapticsCommonData, 
                                TInt aRequestTimeout )
    : iHapticsCommonData( aHapticsCommonData ), 
      iTransIdCounter( 0 ),
      iRequestTimeout( aRequestTimeout )
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginManager::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::ConstructL(<aMatch>)" )) );

    // create transaction list and timer
    iTransactionList = new( ELeave ) CHWRMHapticsPluginTransactionList();
    iPluginTimer = CHWRMHapticsGenericTimer::NewL( this, iRequestTimeout, 0 );

    iRepository = CRepository::NewL( KCRUidHWRMHapticsPlugins ); 

    // get all supported actuator information
    iSupActuators = FindSupportedActuatorInfoL();
    
    // create UID file parser
    iHapticsUid = CHWRMHapticsUid::NewL( KHapticsUidFilename );
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginManager::ConstructL - return" )) );
    }

// ---------------------------------------------------------------------------
// Finds the supported actuator information from plugins.
// ---------------------------------------------------------------------------
//
TUint32 CHWRMHapticsPluginManager::FindSupportedActuatorInfoL()
    {
    TUint32 supportedActuators( 0 );
    
    // Any
    AddInfoIfSupportedL( KHWRMHapticsPluginAnyAdaptation, 
                         EHWRMLogicalActuatorAny, 
                         supportedActuators );

    // Device
    AddInfoIfSupportedL( KHWRMHapticsPluginDeviceAdaptation, 
                         EHWRMLogicalActuatorDevice, 
                         supportedActuators );

    // Primary Display
    AddInfoIfSupportedL( KHWRMHapticsPluginPrimaryDisplayAdaptation, 
                         EHWRMLogicalActuatorPrimaryDisplay, 
                         supportedActuators );

    // Secondary Display
    AddInfoIfSupportedL( KHWRMHapticsPluginSecondaryDisplayAdaptation, 
                         EHWRMLogicalActuatorSecondaryDisplay, 
                         supportedActuators );

    // Game
    AddInfoIfSupportedL( KHWRMHapticsPluginGameAdaptation, 
                         EHWRMLogicalActuatorGame, 
                         supportedActuators );

    // Game Left
    AddInfoIfSupportedL( KHWRMHapticsPluginGameLeftAdaptation, 
                         EHWRMLogicalActuatorGameLeft, 
                         supportedActuators );

    // Game Right
    AddInfoIfSupportedL( KHWRMHapticsPluginGameRightAdaptation, 
                         EHWRMLogicalActuatorGameRight, 
                         supportedActuators );

    // External Vibra
    AddInfoIfSupportedL( KHWRMHapticsPluginExternalVibraAdaptation, 
                         EHWRMLogicalActuatorExternalVibra, 
                         supportedActuators );

    return supportedActuators;
    }

// ---------------------------------------------------------------------------
// If given actuator type is found, adds it to the actuator mask integer.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginManager::AddInfoIfSupportedL( TInt aMatch,
                                                THWRMLogicalActuators aMask,
                                                TUint32& aActuators )
    {
    TInt pluginUid( 0 );
    User::LeaveIfError( iRepository->Get( aMatch, pluginUid ) );

    // if the adaptation plugin UID is not zero, implementation is found,
    // i.e. actuator is supported
    if( pluginUid != 0 ) 
        {
        // add actuator to the mask
        aActuators = aActuators | aMask;
        }    
    }

//  End of File  
