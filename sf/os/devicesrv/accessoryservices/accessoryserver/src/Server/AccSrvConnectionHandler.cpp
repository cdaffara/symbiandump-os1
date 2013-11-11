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
* Description:  Connection Handler
*
*/



// INCLUDE FILES
#include "AccSrvConnectionHandler.h"
#include "AccSrvConnectionController.h" //MAccSrvConnectionControllerObserver
#include "AccPolSubblockNameArrayAccessor.h"
#include "AccPolAccessoryPolicy.h"
#include "AccSrvASYProxyHandler.h"
#include "AccSrvServerModel.h"
#include "AccPolGenericIDAccessor.h"
#include "AccPolicyDB.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

const TUid KAccFwUiDialogNotifierUid = { 0x10205062 };

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::CAccSrvConnectionHandler
// -----------------------------------------------------------------------------
//
CAccSrvConnectionHandler::CAccSrvConnectionHandler(
    MAccSrvConnectionControllerObserver* aCallback,
    CAccSrvServerModel * aModel,
    CAccPolAccessoryPolicy* aPolicy )
    : CAccSrvHandlerBase (aCallback),
      iPolicy( aPolicy ),
      iModel( aModel),
      iReplyValue(0),
      iReplyPck( iReplyValue)
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::CAccSrvConnectionHandler()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ConstructL()" );
    }


// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvConnectionHandler* CAccSrvConnectionHandler::NewL( 
    MAccSrvConnectionControllerObserver* aCallback,
	CAccSrvServerModel* aModel,
    CAccPolAccessoryPolicy* aPolicy )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::NewL()" );

    CAccSrvConnectionHandler* self = new( ELeave ) CAccSrvConnectionHandler( 
                                                        aCallback, aModel, aPolicy );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::NewL - return self" );
    return self;
    }

// Destructor
CAccSrvConnectionHandler::~CAccSrvConnectionHandler()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::~CAccSrvConnectionHandler()" );

	//closes notfier connection
    iNotifier.Close();
    iRequestDataArray.Close();
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::RunL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::RunL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::RunL()" );

	switch ( iState )
	    {	
		/*
		Get response from the user (the notifier completes asyncronous command). Send
		accessory update command to ASY. 
		*/
	
		case EGetSelectionFromUser:
	    	{
		    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::RunL() - Get selection from user" );
				
			if ( iReplyValue )
			    {
			    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::RunL() - User selection %d", iReplyValue );

				//update generic id
				iPolicy->UpdateGenericIDL( iGenericID, iReplyValue);
		
				TASYCommandParamRecord asyCommandParamRecord;
    		    asyCommandParamRecord.iCmdValue   = 0;//Not used in update command
        		asyCommandParamRecord.iGenericID  = iGenericID;		

			    //Send request to ASY Proxy Handler
			    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::RunL() - Send update request");
			    TInt trId = iCallback->HandleASYCommsL( ECmdAccessoryUpdated,
        		                               		asyCommandParamRecord );

	    		//Stored request data is used when response is received from ASY
		    	//(i.e. HandleValueMessageL)
    			StoreRequestDataL( asyCommandParamRecord.iNameRecord, trId, ETrue );

	    		//Store this object to queue to be able to receive response from ASY
    			iCallback->RegisterControlMessageL( ESetValue, this );

    			//closes notfier connection
                iNotifier.Close();
			    }
			else
			    {
                // If this is KErrCancel, cancel is already done. Handle only if user has cancelled the dialog
                if ( KErrNone == iStatus.Int() )
                    {
                    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::RunL() - User cancel");
    	      	    iCallback->ConnectionHandlingCancel( this);	
                    }
			    }
            
	    	}
            break;

		case EInitializeConnection:
		    {
		    if( iCallback->ServerModelHandle().ASYsLoadCompleted() )
	            {
   	        	if ( iConnectionUpdate )
   	        	    {
   	        		//get new capabilities to use
				    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::RunL() - Connection update, set new capabilities");
   	        		iModel->CapabilityStorage().UseUpdatedGenericIDL( iGenericID);
   	        	    }
   	        
    	    	if ( InitializeCriticalCapsL() )
            	    {	
	            	//ASY query started
    	    	    }
        		else
    	            {
    	        	if ( iConnectionUpdate )
    	        	    {
	    	        	//No critical caps, continue connection handling
	    	        	TAccPolGenericID oldGenericId; 
	    	        	iModel->FindWithUniqueIDL(  iGenericID.UniqueID(), oldGenericId);
    		        	iCallback->HandleConnectionUpdateValidationL( 
                            iGenericID, oldGenericId, this, KErrNone );    	        		
    	        	    }
    	        	else
    	        	    {
	    	        	//No critical caps, continue connection handling
    		        	iCallback->HandleConnectValidationL( iGenericID, this, KErrNone );
	   	        	    }
	                }
	    	    }
	    	else
        	    {
	    	    //ASY is not ready yet, wait for an another RunL-loop
    	    	CAccSrvHandlerBase::IssueRequest();
        	    }			
		    }
		    break;

		case EWaitForUINotifier:
		    {
            // If cancelled, do not show dialog
            if ( KErrCancel != iStatus.Int() )
                {
                COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::RunL() - Change in boot state, try again open selection dialog" );
			    OpenSelectionDialogL();			
		        }
			}
		    break;

		default:
            TRACE_ASSERT_ALWAYS;
            break;
	    }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::RunL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::StartConnectionHandling
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::StartConnectionHandlingL( 
    TAccPolGenericID& aGenericID, 
    TBool aEvaluateConnectionRules, 
    TBool aUpdatedConnection )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StartConnectionHandling()" );

    iGenericID = aGenericID;
    iConnectionUpdate = aUpdatedConnection;
    
    //evaluate connection rules
    if ( aEvaluateConnectionRules )
        {
  		COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StartConnectionHandling() - Evaluate connection rules" );

        }
    else
        {
  		COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StartConnectionHandling() - No connection rules" );
  		
        //Check default selection
        TInt defaultSelection( iModel->DeviceType() );
        
        // Device Type Supplied
        if( (iGenericID.PhysicalConnectionCaps() & KPCWired) && 
            (iGenericID.DeviceTypeCaps(KDTHeadset)) && 
            (iPolicy->IsCapabilityDefinedL(iGenericID,KAccIntegratedAudioInput)) )		
            {
            // This is a Headset
            if(KASTTY == defaultSelection)
                {
                iReplyValue = defaultSelection;
                }
            else
                {
                iReplyValue = KASHeadset;
                }
            
            if(KASHeadset != iReplyValue)
                {
                COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StartConnectionHandling- TTY Case" );
                //only one selection, don't show selection dialog
                iState = EGetSelectionFromUser; 
                iCallback->SetDefaultAccessoryInformation( iGenericID, iReplyValue );
                CAccSrvHandlerBase::IssueRequest();
                }
            else
                {
                COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StartConnectionHandling- Headset Case" );
                iState = EInitializeConnection;    
                }
            }
        else if( (iGenericID.PhysicalConnectionCaps() & KPCWired) && 
                 (iGenericID.DeviceTypeCaps(KDTHeadset)) && 
                 !(iPolicy->IsCapabilityDefinedL(iGenericID,KAccIntegratedAudioInput)) )                    
            {
            // This is a Headphone
            if((KASLineOut == defaultSelection) || 
               (KASMusicStand == defaultSelection) ||
               (KASHeadphones == defaultSelection))
                {
                iReplyValue = defaultSelection;
                }
            else
                {
                iReplyValue = KASHeadphones;                
                }
            
            if(KASHeadphones != iReplyValue)
                {
                COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StartConnectionHandling- LineOut/MusicStand Case" );
                //only one selection, don't show selection dialog
                iState = EGetSelectionFromUser; 
                iCallback->SetDefaultAccessoryInformation( iGenericID, iReplyValue );
                CAccSrvHandlerBase::IssueRequest();
                
                }
            else
                {
                COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StartConnectionHandling- Headphone Case" );
                iState = EInitializeConnection;
                }
            }

        else
            {
            iState = EInitializeConnection;
            }
        }

    // Atleas one Asynchronous request is made for ourselves,
    // clients ConnectAccessory asynchronous request will be completed
    // during it.
    if ( iState == EInitializeConnection )
        {
 	    CAccSrvHandlerBase::IssueRequest();
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StartConnectionHandling- return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::OpenSelectionDialogL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::OpenSelectionDialogL()
    {
	COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::OpenSelectionDialogL() - start" );
			    

	if ( !iModel->IdleDetected() )	
	    {
		COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::OpenSelectionDialogL() - Idle is not detected" );

		//boot is ongoing, wait for that ui notifier is ready	
		iState = EWaitForUINotifier;
		
		//wait for idle
		Cancel();
		iStatus = KRequestPending;
		iModel->WaitForIdle( &iStatus );
		SetActive();
	    }
	else
	    {
		COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::OpenSelectionDialogL() - Boot ok, connect notifier" );

		iState = EGetSelectionFromUser;
		
   		//selection bit mask package
	    TPckg<TUint32> selectionBitmaskPackage( iSelectionListBitmask );
	
		//activate active object
		Cancel();
		iStatus = KRequestPending;
	
		//open ui		
		COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::OpenSelectionDialogL() - Open selection dialog" );
        User::LeaveIfError( iNotifier.Connect() );
		iNotifier.StartNotifierAndGetResponse( 
                        iStatus, 
                        KAccFwUiDialogNotifierUid, 
                        selectionBitmaskPackage, 
                        iReplyPck);		
		SetActive();
	    }

	COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::OpenSelectionDialogL() - end" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::OpenSelectionDialogL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::SetupSelectionDialogL( TUint32 aSelectionListBitmask )
    {
	COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::SetupSelectionDialogL() - start" );
	iSelectionListBitmask = aSelectionListBitmask;		    
	//open dialog	
	OpenSelectionDialogL();
	COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::SetupSelectionDialogL() - end" );	
    }


// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::UniqueIDForConnectionHandler
// -----------------------------------------------------------------------------
//
TInt CAccSrvConnectionHandler::UniqueIDForConnectionHandler()
    {
	return iGenericID.UniqueID();
    }
        
// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::CancelConnectionHandling
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::CancelConnectionHandling() 
    {
	COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::CancelConnectionHandling() - start" );

    if ( iNotifier.Handle() )
        {
        // If notifier exists, it must be cancelled
        iNotifier.CancelNotifier( KAccFwUiDialogNotifierUid );
        iNotifier.Close();
        // No need to cancel notifier request, cancel comes from notifier
        }

    else if ( IsActive() )
        {
        if ( EWaitForUINotifier == iState )
            {
            // Internal request, cancel in different way
            iModel->CancelWaitForIdle();
            }
        else
            {
            Cancel();   
            }
        }
    
	// remove this from NotificationQueue
    iCallback->CancelControlMessage( ESetValue, this );
	COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::CancelConnectionHandling() - end" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::ResetHandler
// Resets member variables and is free to handle next connection
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::ResetHandler()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResetHandler() - start" );
    // Reset member variables related to connection
    iSelectionListBitmask = 0;
    iReplyValue = 0;
    iState = EGetSelectionFromUser;
    iRequestDataArray.Reset();
    // Reset generic id. This also indicates that this hander has no connection
    TAccPolGenericID temp;
    iGenericID = temp;
    // No need to reset iPolicy, iModel or iGenericID
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResetHandler() - end" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::IsFree
// -----------------------------------------------------------------------------
//
TBool CAccSrvConnectionHandler::IsFree()
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::IsFree() - UniqueID(%i)", iGenericID.UniqueID() );
    return ( KErrNotFound == iGenericID.UniqueID() );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::InitializeCriticalCapsL
// -----------------------------------------------------------------------------
//
TBool CAccSrvConnectionHandler::InitializeCriticalCapsL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::InitializeCriticalCapsL()" );

    TBool                    ASYqueryStarted( EFalse );
    TAccPolNameRecord        nameRecord;
    TUint32					 name( 0 );
    TASYCommandParamRecord   asyCommandParamRecord;

    CAccPolSubblockNameArray* criticalCapabilities = CAccPolSubblockNameArray::NewL();
    CleanupStack::PushL( criticalCapabilities );

    //After this query, criticalCapabilities contains all critical capabilities from iGenericID
    iPolicy->ResolveCriticalCapabilitiesL( iGenericID, *criticalCapabilities );

    TInt nameCount( TAccPolSubblockNameArrayAccessor::GetCount( *criticalCapabilities ) );

    for ( TInt i( 0 ) ; i<nameCount ; ++i )
        {
        TAccPolSubblockNameArrayAccessor::GetName( *criticalCapabilities,
                                                    i,
                                                    name );
  	    nameRecord.SetName( name );
        asyCommandParamRecord.iNameRecord = nameRecord;
        asyCommandParamRecord.iCmdValue   = 0;//Not used in Getters
        asyCommandParamRecord.iGenericID  = iGenericID;

		    CCapValue* capValue = iModel->CapabilityStorage().GetCapability( iGenericID, name );
		
		    if ( !capValue)
		        {
			      User::Leave( KErrArgument );
		        }

		    if ( capValue->Type() == EAPVBool || 
             capValue->Type() == EAPVUInt || 
             capValue->Type() == EAPVInt )
		        {
			      TInt32 value( 0 );
			      TInt err( capValue->ValueInt( value ) );
		
			      if ( value == KErrUnknown && err == KErrNone )
			          {
    		        StartASYCommandHandlingL( capValue->Type(), asyCommandParamRecord );
		            ASYqueryStarted = ETrue;
			          }
		        }
		    else
		       {
    	     StartASYCommandHandlingL( capValue->Type(), asyCommandParamRecord );
	         ASYqueryStarted = ETrue;
		       }
       }

    CleanupStack::PopAndDestroy( criticalCapabilities );

    //In case of BT, query supported BT-profiles from BT-ASY
    if ( iGenericID.PhysicalConnectionCaps( KPCBluetooth ) )
        {
        TAccPolNameRecord emptyNameRecord;

        asyCommandParamRecord.iNameRecord = emptyNameRecord;
        asyCommandParamRecord.iCmdValue   = iGenericID.DeviceAddress();
        asyCommandParamRecord.iGenericID  = iGenericID;

        StartASYCommandHandlingL( EAPVDes8, asyCommandParamRecord );

        ASYqueryStarted = ETrue;
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::InitializeCriticalCapsL - return %d", ASYqueryStarted );
    return ( ASYqueryStarted );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::StartASYCommandHandlingL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::StartASYCommandHandlingL( 
    TAccPolValueType aValueType,
    TASYCommandParamRecord& aASYCommandParam )
    {
    TProcessCmdId command( ECmdBadFunction );
    TInt          trId( 0 );

    switch ( aValueType )
        {
        case EAPVBool:
            {
            command = ECmdGetValueBool;
            }
            break;

        case EAPVInt:
            {
            command = ECmdGetValueTInt;
            }
            break;

        case EAPVDes8:
            {
            if ( aASYCommandParam.iGenericID.PhysicalConnectionCaps( KPCBluetooth ) )
                {
                //Supported profiles are queried from BT-ASY
                command = ECmdGetSupportedBTProfiles;
                }
            else
                {
                command = ECmdGetValueTDes8;
                }
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            }
            break;
        }

    //Send request to ASY Proxy Handler
    trId = iCallback->HandleASYCommsL( command, aASYCommandParam );

    //Stored request data is used when response is received from ASY
    //(i.e. HandleValueMessageL)
    StoreRequestDataL( aASYCommandParam.iNameRecord, trId, EFalse );

    //Store this object to queue to be able to receive response from ASY
    iCallback-> RegisterControlMessageL( EGetValue, this );

    }
// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::StoreRequestDataL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::StoreRequestDataL( TAccPolNameRecord aName,
                                                 TInt aTrId,
                                                 TBool aUpdateConnectionNotification )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StoreRequestDataL()" );

    TAccSrvRequestData requestData;
    requestData.iName = aName;
    requestData.iTrId = aTrId;
    requestData.iUpdateNotification = aUpdateConnectionNotification;

    iRequestDataArray.AppendL( requestData );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::StoreRequestDataL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::FindRequestData
//
// Find the location of aTrId from iRequestDataArray
// -----------------------------------------------------------------------------
//
TInt CAccSrvConnectionHandler::FindRequestData( TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::FindRequestData()" );

    TInt index( KErrNotFound );
    TInt count( iRequestDataArray.Count() );

    for ( TInt i( 0 ); i < count ; ++i )
        {
        if( iRequestDataArray.operator[]( i ).iTrId == aTrId )
            {
            index = i;//match found
            }
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::FindRequestData - return %d", index );
    return index;
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::HandleValueMessageL (From MQueueObserver)
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::HandleValueMessageL( TMsgID aMsgID,
                                                    TAccValueTypeTBool aValue,
                                                    TInt aErrorCode,
                                                    TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessageL() - TBool" );

    HandleValueMessagesL( aMsgID, aValue.iValue, aErrorCode, aTrId );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessageL - TBool - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::HandleValueMessageL (From MQueueObserver)
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::HandleValueMessageL( TMsgID aMsgID,
                                                    TAccValueTypeTInt aValue,
                                                    TInt aErrorCode,
                                                    TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessageL() - TInt" );

    HandleValueMessagesL( aMsgID, aValue.iValue, aErrorCode, aTrId );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessageL - TInt - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::HandleValueMessagesL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::HandleValueMessagesL( TMsgID /*aMsgID*/,
                                                     TInt aValue,
                                                     TInt aErrorCode,
                                                     TInt aTrId )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessagesL(value=%d)", aValue );
    
    TInt index( FindRequestData( aTrId ) );
    TAccSrvRequestData requestData;
    TBool isUpdate( EFalse );
    
    if ( KErrNotFound != index )
        {
        requestData.iName = iRequestDataArray.operator[]( index ).iName;
        requestData.iTrId = iRequestDataArray.operator[]( index ).iTrId;
        requestData.iUpdateNotification = 
            iRequestDataArray.operator[]( index ).iUpdateNotification;

        if ( KErrNone == aErrorCode )
            {
        
	        if ( requestData.iUpdateNotification )
                {
    	    	//ASY response to accessory update command, continue connection process....
	        	iState = EInitializeConnection;
            	IssueRequest();	
            	isUpdate = ETrue;
        	    }
	        else
                {
        		//Store value to capability storage
        		TUint32 name;
    	    	requestData.iName.GetName( name );
	        
            	CCapValue* value = iModel->CapabilityStorage().GetCapability( iGenericID, name );
        
    	    	if ( !value)
	        	    {
            		User::Leave( KErrArgument );
        		    }
	        	value->SetValueL( aValue );
                }
            }
        else
            {
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessagesL - Error response received(err=%d)", aErrorCode );    
            }

        iRequestDataArray.Remove( index );//Remove this request from iRequestDataArray
        }
    else
        {
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessagesL - Unidentified response received(TrID=%d)", aTrId );    
        TRACE_ASSERT_ALWAYS;
        }
    
    if ( 0 == iRequestDataArray.Count() && !isUpdate )
        {
        //No more pending requests
        iCallback->HandleConnectValidationL( iGenericID, this, KErrNone );
        }
    else
        {
        //Wait next response
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessagesL - return void " );
    }            

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::HandleValueMessageL (From MQueueObserver)
//
// Resolve the Generic ID according to the profile information
// received from BT-ASY.
//
// TASYBTAccInfo type is only supported object in aValue. Its assumed that ASY
// uses package buffer to make type safe transfer for that object i.e.
//      TASYBTAccInfo btInfo
//      TASYBTAccInfoPckgC btInfoPckgC( btInfo )
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::HandleValueMessageL( TMsgID /*aMsgID*/,
                                                    TDesC8& aValue,
                                                    TInt aErrorCode,
                                                    TInt aTrId )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessageL(err=%d) - TDesC8", aErrorCode );

    TInt index( FindRequestData( aTrId ) );
    
    if ( KErrNotFound != index)
        {
        iRequestDataArray.Remove( index );//Remove this request from iRequestDataArray
        //RequestData was not used in this case
        }
    else
        {
        TRACE_ASSERT_ALWAYS;//iRequestDataArray is out of sync
        }
        
    if ( KErrNone == aErrorCode )
        {
        ResolveGenericIDL( aValue );    
        } 

    if ( 0 == iRequestDataArray.Count() )
        {
        //No more pending requests
        iCallback->HandleConnectValidationL( iGenericID, this, aErrorCode );
        }
    else
        {
        //Wait next response
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::HandleValueMessageL - TDesC8 - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::HandleValueMessagesL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::HandleValueMessageL( TMsgID /*aMsgID*/,
                                                    TInt /*aTrId*/,
                                                    TPtr8* /*aPtrBuf*/,
                                                    TInt /*aErrorCode*/ )
    {
    User::Leave( KErrNotSupported );
    }                              
                                   
// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::ResolveGenericIDL
//
// Fill Generic ID according supported profiles received from BT-ASY.
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::ResolveGenericIDL( TDesC8& aValue )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveGenericIDL()" );

    TPckg<TASYBTAccInfo>    btInfoPckg( *static_cast<TPckg<TASYBTAccInfo> *>( &aValue ) );

    COM_TRACE_3( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveGenericIDL - model=%d type=%d prof=%d", btInfoPckg().iDeviceInfo, btInfoPckg().iDeviceType, btInfoPckg().iProfiles );
    COM_TRACE_4( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveGenericIDL - HSP=%d HFP=%d AVRCP=%d A2DP=%d", btInfoPckg().iSupportedFeatures[0], btInfoPckg().iSupportedFeatures[1], btInfoPckg().iSupportedFeatures[2], btInfoPckg().iSupportedFeatures[3]);

    TAccPolGenericIDAccessor::SetHWDeviceID( iGenericID, 
                                             btInfoPckg().iDeviceInfo );
    TAccPolGenericIDAccessor::SetFeatureAttributeL( iGenericID,
                                                    KAccPolDTDeviceType,
                                                    btInfoPckg().iDeviceType );
            
    if ( btInfoPckg().iReserved & KAccInfoDRM )
        {
        //If accessory have recording possibility, KAccNoDRMOutput capability is set.
        //i.e. Capturing Major Service Class is indicated by accessory.
        COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveGenericIDL - DRM output not allowed" );
        iModel->CapabilityStorage().SetCapabilityL( iGenericID, KAccNoDRMOutput );
        }

    if ( btInfoPckg().iReserved & KAccInfoLatency )
        {
        //If accessory provides latency information, KAccAudioLatency capability is set.
        //Initialize capability value as 0. Will be updated via notification.
        COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveGenericIDL - Latency set" );
        iModel->CapabilityStorage().SetCapabilityL( iGenericID, KAccAudioLatency, EAPVAccessory, EAPVInt, 0 );
        }

    ResolveGidL();

    // Fill the Audio subblock and Bluetooth subblock
    if ( KAccInfoHSP & btInfoPckg().iProfiles )//Headset Profile (HSP)
        {
        ResolveHSPSpecificGidL( btInfoPckg().iSupportedFeatures.At( EAccInfoHSPIndex ) );
        }

    if ( KAccInfoHFP & btInfoPckg().iProfiles )//Handsfree Profile (HFP)
        {
        ResolveHFPSpecificGidL( btInfoPckg().iSupportedFeatures.At( EAccInfoHFPIndex ) );
        }

    if ( KAccInfoAVRCP & btInfoPckg().iProfiles )//Audio Video Remote Control Profile (AVRCP)
        {
        ResolveAVRCPSpecificGidL( btInfoPckg().iSupportedFeatures.At( EAccInfoAVRCPIndex ) );
        }

    if ( KAccInfoA2DP & btInfoPckg().iProfiles )//Advanced Audio Distribution Profile (A2DP)
        {
        ResolveA2DPSpecificGidL( btInfoPckg().iSupportedFeatures.At( EAccInfoA2DPIndex ) );
        }

    //Update GenericID to CapabilityStorage 
    iModel->CapabilityStorage().SetGenericIDHeaderL( 
        *( reinterpret_cast<TAccPolGIDHeader*>( &iGenericID ) ) );
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveGenericIDL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::ResolveGidL
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::ResolveGidL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveGidL()" );
    TInt value;

    //Set Audio Output Type capability. For CarKits its public, otherwise private.
    if ( iGenericID.DeviceTypeCaps( KDTCarKit ) )
        {
        value = EAccAudioOutPutTypePublic;
        }
    else
        {
        value = EAccAudioOutPutTypePrivate;
        }

    //Set KAccAudioOutputType capability 
	iModel->CapabilityStorage().SetCapabilityL( 
        iGenericID, KAccAudioOutputType, EAPVPolicy, EAPVInt, value );
	
	//Set KAccSetVolumeLevel capability with zero value.
	//Real values are filled according to supported features.
	iModel->CapabilityStorage().SetCapabilityL( 
        iGenericID, KAccSetVolumeLevel, EAPVPolicy, EAPVInt, 0 );
	
	//Set KSBAudioSubblock capability group
	TAccPolGenericIDAccessor::SetFeatureAttributeL( iGenericID, 
	                                                KAccPolSBCapabilities, 
	                                                iGenericID.SubblockCaps() | 
                                                  KSBAudioSubblock );
	
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveGidL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::ResolveHSPSpecificGidL
//
// HEADSET PROFILE 1.1 / supported features:
//   Remote audio volume control
//     -Boolean
//     -Optional
//     -Default=False
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::ResolveHSPSpecificGidL( TUint16 aSupportedFeatures )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveHSPSpecificGidL()" );

    /******************
     * Audio subblock *
     ******************/
    //following capabilities are fixed (because of Headset profile is supported)
   	iModel->CapabilityStorage().SetCapabilityL( iGenericID, KAccMonoAudio );
    iModel->CapabilityStorage().SetCapabilityL( iGenericID,	KAccIntegratedAudioInput );//i.e. all HSP accessories has microphone
    
    if ( aSupportedFeatures )
        {
        //Remote audio volume control is supported.
        UpdateSetVolumeLevelCapabilityL( KAccSetVolumeLevelHSP );
        }
    
    //KSBAudioSubblock capability group is allready set in CAccSrvConnectionHandler::ResolveGidL()
    
    /**********************
     * Bluetooth subblock *
     **********************/
    //Set KAccBTHSP capability and KSBBluetoothSubblock capability group
   	iModel->CapabilityStorage().SetCapabilityL( iGenericID, KAccBTHSP);
   	TAccPolGenericIDAccessor::SetFeatureAttributeL( iGenericID, 
	                                                  KAccPolSBCapabilities, 
	                                                  iGenericID.SubblockCaps() | 
                                                    KSBBluetoothSubblock );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveHSPSpecificGidL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::ResolveHFPSpecificGidL
//
// Accessory device type is resolved according to this table:
// Profile supported          Accessory type
// -----------------          --------------
// both headset and handsfree headset
// headset                    headset
// handsfree                  carkit
//
// Hands-Free Profile 1.0 / supported features bitmap:
// 000000
// ^^^^^^
// ||||||
// ||||||--Bit 0----- EC and/or NR function
// |||||---Bit 1----- Call waiting and 3-way calling
// ||||----Bit 2----- CLI presentation capability
// |||-----Bit 3----- Voice recognition activation
// ||------Bit 4----- Remote volume control
// |-------Bit 5-31-- Unused (available for PAP and other extensibility)
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::ResolveHFPSpecificGidL( TUint16 aSupportedFeatures )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccBTConnectionHandler::ResolveHFPSpecificGidL()" );

    /******************
     * Audio subblock *
     ******************/
    // The default value for supported features of a HF device according to the
    // HF profile specification is 0.
    if ( aSupportedFeatures & 0x1 )//Bit 0 -> EC and/or NR function
        {
		iModel->CapabilityStorage().SetCapabilityL( iGenericID, 
		                                            KAccAcousticEchoControl, 
		                                            EAPVAccessory, 
		                                            EAPVBool, 
		                                            EFalse  );
		iModel->CapabilityStorage().SetCapabilityL( iGenericID, 
		                                            KAccNoiseReduction,
		                                            EAPVAccessory,
		                                            EAPVBool,
		                                            EFalse  );
        }


    if ( aSupportedFeatures & 0x8 )//Bit 3 -> Voice recognition
       {
       iModel->CapabilityStorage().SetCapabilityL( iGenericID, 
                                                   KAccBTVoiceRecognition );       
       COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveHFPSpecificGidL - KAccVoiceRecognition supported." );                                                   
       }
    else
       {
    	 COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveHFPSpecificGidL - KAccVoiceRecognition not supported." );                                                   
       }
    

    if ( aSupportedFeatures & 0x10 )//Bit 4 -> Remote volume control
        {
        //Remote audio volume control is supported.
        UpdateSetVolumeLevelCapabilityL( KAccSetVolumeLevelHFP );
        }
        
    //following capabilities are fixed (because of Handsfree profile is supported)
	iModel->CapabilityStorage().SetCapabilityL( iGenericID, KAccMonoAudio );
	iModel->CapabilityStorage().SetCapabilityL( iGenericID,	KAccIntegratedAudioInput );//i.e. all HFP accessories has microphone
	
	//KSBAudioSubblock capability group is allready set in CAccSrvConnectionHandler::ResolveGidL()
	
    /**********************
     * Bluetooth subblock *
     **********************/
    //Set KAccBTHSP capability and KSBBluetoothSubblock capability group
	iModel->CapabilityStorage().SetCapabilityL( iGenericID, KAccBTHFP );
	TAccPolGenericIDAccessor::SetFeatureAttributeL( iGenericID, 
	                                                KAccPolSBCapabilities, 
	                                                iGenericID.SubblockCaps() | 
                                                  KSBBluetoothSubblock );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveHFPSpecificGidL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::ResolveAVRCPSpecificGidL
//
// AUDIO/VIDEO REMOTE CONTROL PROFILE 1.0 / supported features bitmap:
// AVRCP
// Service Record for TG (Accessory is TG)
//        Bit 0 = Category 1
//        Bit 1 = Category 2
//        Bit 2 = Category 3
//        Bit 3 = Category 4
//        Bit 4-15 = Reserved for Future Additions
//
// Service Record for CT
//        Bit 0 = Category 1
//        Bit 1 = Category 2
//        Bit 2 = Category 3
//        Bit 3 = Category 4
//        Bit 4-15 = Reserved for Future Additions
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::ResolveAVRCPSpecificGidL(
    TUint16 /*aSupportedFeatures*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveAVRCPSpecificGidL()" );

    /******************
     * Audio subblock *
     ******************/
    //AVRCP doesn't support remote audio volume control.

    /**********************
     * Bluetooth subblock *
     **********************/
    //Set KAccBTAVRCP capability and KSBBluetoothSubblock capability group
    iModel->CapabilityStorage().SetCapabilityL( iGenericID, KAccBTAVRCP );
   	TAccPolGenericIDAccessor::SetFeatureAttributeL( iGenericID, 
	                                                  KAccPolSBCapabilities, 
	                                                  iGenericID.SubblockCaps() | 
                                                    KSBBluetoothSubblock );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveAVRCPSpecificGidL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::ResolveA2DPSpecificGidL
//
// ADVANCED AUDIO DISTRIBUTION PROFILE SPECIFICATION 1.0 / supported features bitmap:
// A2DP
//
// Service Record for Sink (Accessory is sink)
//        Bit 0 = Headphone
//        Bit 1 = Speaker
//        Bit 2 = Recorder
//        Bit 3 = Amplifier
//        Bit 4-15 = Reserved for Future Additions
//
// Service Record for Source
//        Bit 0 = Player
//        Bit 1 = Microphone
//        Bit 2 = Tuner
//        Bit 3 = Mixer
//        Bit 4-15 = Reserved for Future Additions
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::ResolveA2DPSpecificGidL( TUint16 /*aSupportedFeatures*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveA2DPSpecificGidL()" );

    /******************
     * Audio subblock *
     ******************/
    iModel->CapabilityStorage().SetCapabilityL( iGenericID, KAccStereoAudio );
    //KSBAudioSubblock capability group is allready set in CAccSrvConnectionHandler::ResolveGidL()
     
    /**********************
     * Bluetooth subblock *
     **********************/
    //Set KAccBTA2DP capability and KSBBluetoothSubblock capability group
    iModel->CapabilityStorage().SetCapabilityL( iGenericID, KAccBTA2DP );
   	TAccPolGenericIDAccessor::SetFeatureAttributeL( iGenericID, 
	                                                  KAccPolSBCapabilities, 
	                                                  iGenericID.SubblockCaps() | 
                                                    KSBBluetoothSubblock );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::ResolveA2DPSpecificGidL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::UpdateSetVolumeLevelCapabilityL
//
// Read the existing KAccSetVolumeLevel capability value and update it
// according to the aCapability parameter.
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionHandler::UpdateSetVolumeLevelCapabilityL( TInt32 aCapability )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::UpdateSetVolumeLevelCapabilityL(%d)", aCapability ); 
               
    CCapValue* capValue = iModel->CapabilityStorage().GetCapability( 
                                                        iGenericID, KAccSetVolumeLevel );
    
    if ( capValue )
        {    
        TInt32 currentValue( 0 );
        if( KErrNone == capValue->ValueInt( currentValue ) )
            {
            currentValue = currentValue | aCapability;
            iModel->CapabilityStorage().SetCapabilityL( 
                                            iGenericID, 
                                            KAccSetVolumeLevel,
                                            EAPVPolicy, 
                                            EAPVInt, 
                                            currentValue );            
            }
        }
            
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::UpdateSetVolumeLevelCapabilityL - return void" );
    }
    
// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::RunError
//
// Called if RunL Leaves
// -----------------------------------------------------------------------------
//
TInt CAccSrvConnectionHandler::RunError( TInt aError )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionHandler::RunError(%d)", aError );
    
    aError = KErrNone;// Avoid Panic in CActiveScheduler

    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::RunError - return KErrNone" );

    return aError;
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionHandler::CompleteMessageL (From MQueueObserver)
// -----------------------------------------------------------------------------
//
TBool CAccSrvConnectionHandler::CompleteMessageL( TMsgID /*aMsgID*/,
                                                  TInt /*aErrorCode*/,
                                                  TInt /*aUniqueID*/ )
    {
    //Not used, must define it here because pure virtual in base class (MQueueObserver)
    TRACE_ASSERT_ALWAYS;
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
//CAccSrvConnectionHandler::CompleteProcessCommandL
// -----------------------------------------------------------------------------
//
TBool CAccSrvConnectionHandler::CompleteProcessCommandL( 
    TProcessCmdId /*aCmdn*/,
    TInt /*aTrId*/,
    TASYCommandParamRecord& /*aASYCommandParamRecord*/,
    TPtr8* /*aPtrBuf*/ )
    {
    //Not used, must define it here because pure virtual in base class (MQueueObserver)
    TRACE_ASSERT_ALWAYS;
    return EFalse;
    }
// End of File
