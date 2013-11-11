/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Connection Controller
*
*/



// INCLUDE FILES
#include "AccSrvConnectionController.h"
#include "AccPolAccessoryPolicy.h"
#include "AccSrvServerModel.h"
#include "AccSrvNotificationQueue.h"
#include "AccSrvConnectionStatusHandler.h"
#include "AccSrvModeHandler.h"
#include "AccPolGenericIDArrayAccessor.h"
#include "acc_debug.h"
#include "AccSrvASYProxyHandler.h"
#include "AccPolGenericIDAccessor.h"
#include "AccSrvSettingsHandler.h"
#include "AccPolSubblockNameArrayAccessor.h"
#include <AccConfigFileParser.h>
#include <s32mem.h>
#include "AccSrvWiredConnectionPublisher.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::CAccSrvConnectionController
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvConnectionController::CAccSrvConnectionController()
    : iPolicy( NULL ),
      iServerModel( NULL ),
      iNotificationQueue( NULL ),
      iConnectionStatusHandler( NULL ),
      iModeHandler( NULL ),
      iASYProxyHandler( NULL ),
      iWiredConnPublisher( NULL )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::CAccSrvConnectionController()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::CAccSrvConnectionController - return" );
    }


// -----------------------------------------------------------------------------
// CAccSrvConnectionController::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

void CAccSrvConnectionController::ConstructL( CAccSrvServerModel* aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::ConstructL()" );

    iServerModel             = aServerModel;
    iPolicy                  = CAccPolAccessoryPolicy::NewL( &iServerModel->CapabilityStorage());
    iNotificationQueue       = CAccSrvNotificationQueue::NewL();
    iConnectionStatusHandler = CAccSrvConnectionStatusHandler::NewL( this );
    iModeHandler             = CAccSrvModeHandler::NewL( this );
    iASYProxyHandler         = CAccSrvASYProxyHandler::NewL( this );
    iSettingsHandler         = CAccSrvSettingsHandler::NewL( this, *aServerModel, iPolicy );
    iWiredConnPublisher      = CAccSrvWiredConnectionPublisher::NewL();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvConnectionController* CAccSrvConnectionController::NewL(
    CAccSrvServerModel* aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::NewL()" );

    CAccSrvConnectionController* self = new( ELeave ) CAccSrvConnectionController;

    CleanupStack::PushL( self );
    self->ConstructL( aServerModel );
    CleanupStack::Pop( self );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::NewL - return self" );

    return self;
    }

// Destructor
CAccSrvConnectionController::~CAccSrvConnectionController()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::~CAccSrvConnectionController()" );

    if( NULL != iPolicy )
        {
        delete iPolicy;
        }

    if( NULL != iNotificationQueue )
        {
        delete iNotificationQueue;
        }

    if( NULL != iConnectionStatusHandler )
        {
        delete iConnectionStatusHandler;
        }

    if( NULL != iModeHandler )
        {
        delete iModeHandler;
        }

    if( NULL != iASYProxyHandler )
        {
        delete iASYProxyHandler;
        }

    if( NULL != iSettingsHandler )
        {
        delete iSettingsHandler;
        }
    
    delete iWiredConnPublisher;

    // Handlers must be deleted after iPolicy and iASYProxyHandler
    // (used from handlers).
    iConnectionHandler.ResetAndDestroy();
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::~CAccSrvConnectionController - return" );
    }


// -----------------------------------------------------------------------------
// CAccSrvConnectionController::ServerModel
// -----------------------------------------------------------------------------
//
CAccSrvServerModel& CAccSrvConnectionController::ServerModel() const
    {
    return *iServerModel;
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::NotificationQueue
// -----------------------------------------------------------------------------
//
CAccSrvNotificationQueue& CAccSrvConnectionController::NotificationQueue() const
    {
    return *iNotificationQueue;
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::ASYProxyHandler
// -----------------------------------------------------------------------------
//
CAccSrvASYProxyHandler& CAccSrvConnectionController::ASYProxyHandler() const
    {
    return *iASYProxyHandler;
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::SettingsHandler
// -----------------------------------------------------------------------------
//
CAccSrvSettingsHandler& CAccSrvConnectionController::SettingsHandler() const
    {
    return *iSettingsHandler;
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::Policy
// -----------------------------------------------------------------------------
//
CAccPolAccessoryPolicy& CAccSrvConnectionController::Policy() const
    {
    return *iPolicy;
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::HandleConnectL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::HandleConnectL( TAccPolGenericID& aGenericID,
                                                  TUint aCallerThreadID,
                                                  TBool aEvaluateConnectionRules,
                                                  TBool aUpdateConnection )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectL()" );

	if ( aUpdateConnection )
	    {
	    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectL() Update connection" );
		
		//check is there connection handling ongoing for unique id
		TInt index( KErrNotFound );
		for ( TInt i(0); i < iConnectionHandler.Count(); i++ )
	    	{
			if ( aGenericID.UniqueID() == 
                 iConnectionHandler.operator[]( i )->UniqueIDForConnectionHandler() )
			    {
				index = i;
				break;
			    }
	    	}
		
		//cancel current connection handling and reset connection handler
		if ( KErrNotFound == index)
		    {
            //Create a new connection handler for this connection and start the handling
            CAccSrvConnectionHandler* connectionHandler = CAccSrvConnectionHandler::NewL( this, iServerModel, iPolicy );
            CleanupStack::PushL( connectionHandler );
            iConnectionHandler.InsertL( connectionHandler, 0 );
            CleanupStack::Pop( connectionHandler );
            index = 0;
            }
        else
            {
            COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectL() Connection handling ongoing - delete current connection handler" );
			iConnectionHandler.operator[]( index )->CancelConnectionHandling();
            iConnectionHandler.operator[]( index )->ResetHandler();
   		    }
   		
	    // Restart connection handling
    	iConnectionHandler.operator[]( index )->StartConnectionHandlingL( 
                                                    aGenericID,
                                                    aEvaluateConnectionRules, 
                                                    iServerModel->IsConnectedOrDetected( 
                                                        aGenericID ) );
    	}
	else
    	{
	    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectL() New connection" );

	    // Store ThreadId for ASY handling
	    iServerModel->StoreASYThreadIDL( aCallerThreadID, aGenericID.UniqueID() );

	    iServerModel->AddPhysicalConnectionL( aGenericID );		

	    iWiredConnPublisher->HandleConnectL( aGenericID );

        // Search for free connection handlers
        TInt index( KErrNotFound);
		for ( TInt i(0); i < iConnectionHandler.Count(); i++ )
		    {
			if ( iConnectionHandler.operator[]( i )->IsFree() )
			    {
				index = i;
				break;
			    }
		    }

        // Create new connection handler if available connection handler is not found
        if ( KErrNotFound == index )
            {
            COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectL - Available handler not found, creating new" );
            CAccSrvConnectionHandler* handler = CAccSrvConnectionHandler::NewL( 
                this, iServerModel, iPolicy );
            CleanupStack::PushL( handler );
            handler->StartConnectionHandlingL( aGenericID, aEvaluateConnectionRules, EFalse );
            iConnectionHandler.AppendL( handler );
            CleanupStack::Pop( handler );
            }
        else
            {
            // Available hander found, using that
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectL - Available handler found at %i, using that", index );
            iConnectionHandler.operator[]( index )->StartConnectionHandlingL( 
                aGenericID, aEvaluateConnectionRules, EFalse );
            }	
    	}

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::HandleConnectionUpdateValidationL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::HandleConnectionUpdateValidationL( 
    const TAccPolGenericID& aGenericID, 
	TAccPolGenericID& aOldGenericId,
	const CAccSrvConnectionHandler* aCaller,	
	TInt aError )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectionUpdateValidationL()" );
    
    TInt err ( KErrNone );
    TAccSrvConnection isValidConnection(EAccSrvConnect);
    TBool isConnected( iServerModel->IsConnected( aGenericID ) );

    if( KErrNone == aError )
        {   
        isValidConnection = iPolicy->IsValidConnectionL( aGenericID, err );
        }
    else
        {
        err = aError;
        isValidConnection = EAccSrvDeny;
        }
            
    if( iServerModel->FindAndRemoveCancelledConnectAccessory( aGenericID.UniqueID() ) )
        {
        //Connection of this accessory is cancelled.
        err = KErrCancel;
        isValidConnection = EAccSrvDeny;
        }

    switch( isValidConnection )
        {
        case EAccSrvDetect:
            {
            if( isConnected )
                {
                //accessory was connected, but now it is detected
 			    iServerModel->AddPhysicalConnectionL( aGenericID ); //Move GID from physical connection array
			    iServerModel->RemoveConnectionL( aGenericID, EFalse );            //to connection array
 		        
  		        // Complete all possible connection status related requests
        		iNotificationQueue->CompleteControlMessageL( EAccessoryConnectionStatusChanged,
                                                		     KErrNone,
	                                                    	 aGenericID.UniqueID() );
        		iNotificationQueue->CompleteControlMessageL( EAccessoryDisconnected,
                		                                     KErrNone,
                        		                             aOldGenericId.UniqueID() );
                }
            else
                {
                //Accessory remains in detected state.
                }
            }
            break;

        case EAccSrvConnect:
            {
            if( isConnected )
                {
                //update connection generic id array
                iServerModel->UpdateConnectionL( aGenericID);
                
 		        // Complete all possible connection status related requests
        		iNotificationQueue->CompleteControlMessageL( EAccessoryConnectionStatusChanged,
                                                		     KErrNone,
	                                                    	 aGenericID.UniqueID() );
        		iNotificationQueue->CompleteControlMessageL( EAccessoryDisconnected,
                		                                     KErrNone,
                        		                             aOldGenericId.UniqueID() );
    		    iNotificationQueue->CompleteControlMessageL( ENewAccessoryConnected,
                                        		             KErrNone,
                                		                     aGenericID.UniqueID() );
                }
            else
                {
                //accessory was detected, but now it is connected
			    iServerModel->RemovePhysicalConnection( aGenericID ); //Move GID from physical connection array
			    iServerModel->AddConnectionL( aGenericID );            //to connection array
  
 		        // Complete all possible connection status related requests
        		iNotificationQueue->CompleteControlMessageL( EAccessoryConnectionStatusChanged,
                                                		     KErrNone,
	                                                    	 aGenericID.UniqueID() );
    		    iNotificationQueue->CompleteControlMessageL( ENewAccessoryConnected,
                                        		             KErrNone,
                                		                     aGenericID.UniqueID() );
                }
            }
            break;

        case EAccSrvDeny:
            {
            iServerModel->RemovePhysicalConnection( aGenericID );
            iServerModel->RemoveASYThreadID( aGenericID.UniqueID() );//Remove ThreadID GID mapping
            iServerModel->CapabilityStorage().RemoveCapabilityList( aGenericID);
            
         
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;//unhandled enum
            }
            break;
        }

    iNotificationQueue->CompleteControlMessageL( EConnectAccessory,
                                                 err,
                                                 aGenericID.UniqueID() );

    //"This" connection handler has done its job
    TInt index( iConnectionHandler.Find( aCaller ) );
    if( KErrNotFound != index )
        {
        delete iConnectionHandler.operator[]( index );
        iConnectionHandler.Remove( index );
        }
    else
        {
        TRACE_ASSERT_ALWAYS; //iConnectionHandler array is not in sync
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectionUpdateValidationL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::HandleConnectionUpdateValidationL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::HandleConnectionUpdateValidationL( 
    const TAccPolGenericID& aGenericID, 
    TAccPolGenericID& aOldGenericId,
    const CAccSrvSettingsHandler* /*aCaller*/,    
    TInt aError )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectionUpdateValidationL()" );
    
    TInt err ( KErrNone );
    TAccSrvConnection isValidConnection(EAccSrvConnect);
    TBool isConnected( iServerModel->IsConnected( aGenericID ) );

    if( KErrNone == aError )
        {   
        isValidConnection = iPolicy->IsValidConnectionL( aGenericID, err );
        }
    else
        {
        err = aError;
        isValidConnection = EAccSrvDeny;
        }

    if( iServerModel->FindAndRemoveCancelledConnectAccessory( aGenericID.UniqueID() ) )
        {
        //Connection of this accessory is cancelled.
        err = KErrCancel;
        isValidConnection = EAccSrvDeny;
        }
    
    switch( isValidConnection )
        {
        case EAccSrvDetect:
            {
            }
            break;

        case EAccSrvConnect:
            {
            if( isConnected )
                {
                //update connection generic id array
                iServerModel->UpdateConnectionL( aGenericID);
                
                // Complete all possible connection status related requests
                iNotificationQueue->CompleteControlMessageL( EAccessoryConnectionStatusChanged,
                                                             KErrNone,
                                                             aGenericID.UniqueID() );
                iNotificationQueue->CompleteControlMessageL( EAccessoryDisconnected,
                                                             KErrNone,
                                                             aOldGenericId.UniqueID() );
                iNotificationQueue->CompleteControlMessageL( ENewAccessoryConnected,
                                                             KErrNone,
                                                             aGenericID.UniqueID() );
                }

            }
            break;

        case EAccSrvDeny:
            {
            iServerModel->RemovePhysicalConnection( aGenericID );
            iServerModel->RemoveASYThreadID( aGenericID.UniqueID() );//Remove ThreadID GID mapping
            iServerModel->CapabilityStorage().RemoveCapabilityList( aGenericID);
            
         
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;//unhandled enum
            }
            break;
        }

    iNotificationQueue->CompleteControlMessageL( EConnectAccessory,
                                                 err,
                                                 aGenericID.UniqueID() );
    

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectionUpdateValidationL - return void" );
    }



// -----------------------------------------------------------------------------
// CAccSrvConnectionController::ConnectionHandlingCancel
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::ConnectionHandlingCancel(
    const CAccSrvConnectionHandler* aCaller )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::ConnectionHandlingCancel - start" );
    //"This" connection handler has done its job
    TInt index( iConnectionHandler.Find( aCaller ) );
    if( KErrNotFound != index )
        {
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionController::ConnectionHandlingCancel - Handler found at %i", index );
        iConnectionHandler.operator[]( index )->CancelConnectionHandling();
        iConnectionHandler.operator[]( index )->ResetHandler();
        }
    else
        {
        TRACE_ASSERT_ALWAYS; //iConnectionHandler array is not in sync
        }
	COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::ConnectionHandlingCancel - end" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::HandleConnectValidationL.
//
// During AccessoryConnection this is the phase where all critical capabilities
// are initialized and decision for connection acceptance can be done.
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::HandleConnectValidationL(
    const TAccPolGenericID& aGenericID,
    const CAccSrvConnectionHandler* aCaller,
    TInt aError )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectValidationL()" );
    
    TInt err ( KErrNone );
    TAccSrvConnection isValidConnection( EAccSrvConnect );

    if( KErrNone == aError )
        {   
        isValidConnection = iPolicy->IsValidConnectionL( aGenericID, err );
        }
    else
        {
        err = aError;
        isValidConnection = EAccSrvDeny;
        }
            
    if( iServerModel->FindAndRemoveCancelledConnectAccessory( aGenericID.UniqueID() ) )
        {
        //Connection of this accessory is cancelled.
        err = KErrCancel;
        isValidConnection = EAccSrvDeny;
        }

    iNotificationQueue->CompleteControlMessageL( EConnectAccessory,
                                                 err,
                                                 aGenericID.UniqueID() );
    switch( isValidConnection )
        {
        case EAccSrvDetect:
            {
            // Accessory is detected but not connected.
            // Further changes to capabilities may change this
            // i.e. connection status changes from detected to connected.
            // Detected accessory is allready in physical connection table
            // (AddPhysicalConnectionL), so there is nothing to do here.
            }
            break;

        case EAccSrvConnect:
            {
            PromoteToConnectedL( aGenericID );
            }
            break;

        case EAccSrvDeny:
            {
            iServerModel->RemovePhysicalConnection( aGenericID );
            iServerModel->RemoveASYThreadID( aGenericID.UniqueID() );//Remove ThreadID GID mapping
            iServerModel->CapabilityStorage().RemoveCapabilityList( aGenericID );
            
         
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;//unhandled enum
            }
            break;
        }

    //"This" connection handler has done its job
    TInt index( iConnectionHandler.Find( aCaller ) );
    if( KErrNotFound != index )
        {
        delete iConnectionHandler.operator[]( index );
        iConnectionHandler.Remove( index );
        }
    else
        {
        TRACE_ASSERT_ALWAYS; //iConnectionHandler array is not in sync
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectValidationL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::HandleConnectionChangeValidationL
//
// Accessory is allready connected and a capability is changed.
// This may change a single accessory connection
// - from detected to connected
// - from connected to detected
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::HandleConnectionChangeValidationL(
    const TAccPolGenericID& aGenericID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectionChangeValidationL()" );

    TBool isConnected( iServerModel->IsConnected( aGenericID ) );
    TInt err( KErrNone );

    switch( iPolicy->IsValidConnectionL( aGenericID, err ) )
        {
        case EAccSrvConnect:
            {
            if( isConnected )
                {
                //accessory is allready connected -> no need to do anything
                }
            else
                {
                //accessory was detected, but now it is connected
                PromoteToConnectedL( aGenericID );
                }
            }
            break;

        case EAccSrvDetect:
            {
            if( isConnected )
                {
                //accessory was connected, but now it is detected
                LowerToDetectedL( aGenericID );
                }
            else
                {
                //Accessory remains in detected state.
                }
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;//Logigal error in connection/detection handling
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectionChangeValidationL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::PromoteToConnectedL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::PromoteToConnectedL(
    const TAccPolGenericID& aGenericID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::PromoteToConnectedL()" );

    if( KErrNotFound != iServerModel->RemovePhysicalConnection( aGenericID )) //Move GID from physical connection array
      {
	    iServerModel->AddConnectionL( aGenericID );            //to connection array

	    iNotificationQueue->CompleteControlMessageL( ENewAccessoryConnected,
	                                                 KErrNone,
	                                                 aGenericID.UniqueID() );

	    iConnectionStatusHandler->IssueRequest();
	  }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::PromoteToConnectedL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::LowerToDetectedL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::LowerToDetectedL(
    const TAccPolGenericID& aGenericID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::LowerToDetectedL()" );

    iServerModel->RemoveConnectionL( aGenericID, EFalse );//Move GID from connection array to physical connection array
    iServerModel->AddPhysicalConnectionL( aGenericID );

    iNotificationQueue->CompleteControlMessageL( EAccessoryDisconnected,
                                                 KErrNone,
                                                 aGenericID.UniqueID() );
                                                 
    iConnectionStatusHandler->IssueRequest();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::LowerToDetectedL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::HandleDisconnectL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::HandleDisconnectL(
    const TAccPolGenericID& aGenericID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleDisconnectL()" );

	//check is there connection handling ongoing for unique id
	TInt index( KErrNotFound);
	for ( TInt i(0); i < iConnectionHandler.Count(); i++ )
	    {
		if ( aGenericID.UniqueID() == 
             iConnectionHandler.operator[]( i )->UniqueIDForConnectionHandler() )
		    {
			index = i;
			break;
	    	}
	    }

	//cancel current connection handling and remove connection handler
	if ( index >= 0 )
	    {
		iConnectionHandler.operator[]( index )->CancelConnectionHandling();
        iConnectionHandler.operator[]( index )->ResetHandler();
        }

    iServerModel->RemoveASYThreadID( aGenericID.UniqueID() );//Remove ThreadID GID mapping

    iServerModel->RemoveConnectionL( aGenericID, ETrue/*store last disconnect accessory*/ );
    iServerModel->RemovePhysicalConnection( aGenericID );

    iWiredConnPublisher->HandleDisconnectL( aGenericID );

    iNotificationQueue->CompleteControlMessageL( EAccessoryDisconnected,
                                                 KErrNone,
                                                 aGenericID.UniqueID() );

    //Remove names/values from capability storage
	iServerModel->CapabilityStorage().RemoveCapabilityList( 
        iServerModel->LastDisconnectedAccessory() );

    iServerModel->ClearLastDisconnectedAccessory();

    iConnectionStatusHandler->IssueRequest();

#ifdef _DEBUG
    //Check how many accessories are allready connected
    TAccPolGenericIDArray genericIDArray;
    iServerModel->CurrentConnectionStatusL( genericIDArray );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionController::HandleDisconnectL - Still connected accesories:%d", genericIDArray.Count() );
#endif // _DEBUG

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleDisconnectL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::HandleConnectionStatusChangedL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::HandleConnectionStatusChangedL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectionStatusChangedL()" );

    TAccPolGenericIDArray genericIDArray;

    iServerModel->CurrentConnectionStatusL( genericIDArray );
    iNotificationQueue->CompleteControlMessageL( EAccessoryConnectionStatusChanged,
                                                 KErrNone,
                                                 KErrNotFound );
    iSettingsHandler->ConnectionStatusChangedL( genericIDArray );

    iModeHandler->IssueRequest();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleConnectionStatusChangedL - return void" );
    }
    
// -----------------------------------------------------------------------------
// CAccSrvConnectionController::HandleAccessoryModeChangedL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::HandleAccessoryModeChangedL(
    TInt aDbId,
    TBool aAudioOutputStatus )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleAccessoryModeChangedL()" );

    TAccPolGenericIDArray genericIDArray;
    TAccPolAccessoryMode accMode;

    iServerModel->CurrentConnectionStatusL( genericIDArray );
    
    TUint count;
    TBool isHDMIConnected = EFalse;
    
    count = genericIDArray.Count();
    if((1 < count) && (EFalse == aAudioOutputStatus) && (KErrUnknown != aDbId))
        {
        for(TInt i( 0 ); i < count; ++i)
            {
            if(genericIDArray.GetGenericIDL(i).PhysicalConnectionCaps() & KPCHDMI)
                {
                isHDMIConnected = ETrue;
                break;
                }        
            }
        }
    
    if(isHDMIConnected)
        {
        accMode = iServerModel->AccessoryMode();
        }    
    else
        {
        accMode = iPolicy->ResolveAccessoryModeL( genericIDArray, 
                                                  aDbId, 
                                                  aAudioOutputStatus, 
                                                  iServerModel->AccessoryMode(), 
                                                  iServerModel->UniqueID() );
        }

    

    if( iServerModel->SetAccessoryMode( accMode, aDbId ) ) //Store accessory mode
        {
        if( iModeHandler->IsActive() || iConnectionStatusHandler->IsActive() )
            {
            //Model will be updated after pending ModeHandler or ConnectionStatusHandler is executed.
            //This Mode update can de filtered out for that reason.
            COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleAccessoryModeChangedL - Mode update filtered" );
            }
        else
            {
            iNotificationQueue->CompleteControlMessageL( EAccessoryModeChanged,
                                                         KErrNone,
                                                         KErrNotFound );
            }
        }        
    else
        {
        // Accessory Mode was allready up-to-date,
        // no need to complete Accessory Mode changed notification.
        COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleAccessoryModeChangedL - Mode allready up-to-date" );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::HandleAccessoryModeChangedL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::SetDefaultAccessoryInformation
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::SetDefaultAccessoryInformation(
    const TAccPolGenericID& aGenericID, 
	const TUint32 aDefaultValue )
    {
	COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::SetDefaultAccessoryInformation()" );
	iInformationNoteUID = aGenericID.UniqueID();
	iInformationNoteDefault = aDefaultValue;
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::RegisterControlMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::RegisterControlMessageL( TMsgID aMsgID,
                                                           MQueueObserver* aCallback )
    {
    iNotificationQueue->RegisterControlMessage( aMsgID, aCallback );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::CancelControlMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::CancelControlMessage( TMsgID aMsgID,
                                                        MQueueObserver* aCallback )
    {
    iNotificationQueue->CancelControlMessage( aMsgID, aCallback );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::HandleASYCommsL
// -----------------------------------------------------------------------------
//
TInt CAccSrvConnectionController::HandleASYCommsL( 
    TProcessCmdId aCmdId,
    TASYCommandParamRecord& aASYCommandParamRecord,
    TPtr8* aPtrBuf )
    {
    return iASYProxyHandler->HandleASYCommsL( aCmdId,
                                              aASYCommandParamRecord,
                                              aPtrBuf );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::ServerModelHandle
// -----------------------------------------------------------------------------
//
CAccSrvServerModel& CAccSrvConnectionController::ServerModelHandle()
    {
    return CAccSrvConnectionController::ServerModel();
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionController::InitializeConnectionStatusL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionController::InitializeConnectionStatusL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::InitializeConnectionStatusL()" );
    iNotificationQueue->CompleteControlMessageL( 
        EAccessoryModeAsyncChanged, KErrNone, KErrNotFound );
    iNotificationQueue->CompleteControlMessageL( 
        EAccessoryConnectionStatusAsyncChanged, KErrNone, KErrNotFound );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionController::InitializeConnectionStatusL - return void" );
    }

//  End of File
