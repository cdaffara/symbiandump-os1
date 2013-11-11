/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server-side sub-session of Accessory Control
*
*/


// INCLUDE FILES
#include "AccSrvSubControl.h"
#include "AccSrvMainSession.h"
#include "AccClientServerMessages.h"
#include "AccPolGenericID.h"
#include "AccPolGenericIDAccessor.h"
#include "acc_debug.h"
#include "AccSrvServerModel.h"
#include "AccSrvASYProxyHandler.h"
#include "AccPolAccessoryPolicy.h"
#include "AccPolNameValueArraySerial.h"
#include <AccessoryTypes.h>
#include <s32mem.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

const TUid KAccFwUiNoteNotifierUid   = { 0x10205061 };
//const TUid KAccFwUiDialogNotifierUid = { 0x10205062 }
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvSubControl::CAccSrvSubControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubControl::CAccSrvSubControl(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    : CAccSrvSubConnection ( aProxy, aConnectionController, aServerModel ),
    iOutstandingTrId( 0 )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::CAccSrvSubControl()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::ConstructL()" );
    CAccSrvSubConnection::ConstructL();
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvSubControl* CAccSrvSubControl::NewL(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::NewL()" );

    if ( !aProxy )
        {
        User::Leave( KErrArgument );
        }

    CAccSrvSubControl* self = new (ELeave) CAccSrvSubControl( aProxy,
                                                              aConnectionController,
                                                              aServerModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::NewL - return self" );
    return self;
    }

// Destructor
CAccSrvSubControl::~CAccSrvSubControl()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::~CAccSrvSubControl()" );
    // Cancel any outstanding request
    if ( !iConnectAccessoryMsg.IsNull() )
        {
        iConnectAccessoryMsg.Complete( KErrCancel );
        }

    if ( !iSetValueTBoolMsg.IsNull() )
        {
        iSetValueTBoolMsg.Complete( KErrCancel );
        }

    if ( !iSetValueTIntMsg.IsNull() )
        {
        iSetValueTIntMsg.Complete( KErrCancel );
        }
        
    if ( !iSetValueObjectMsg.IsNull() )
        {
        iSetValueObjectMsg.Complete( KErrCancel );
        }
        
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::~CAccSrvSubControl - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::DispatchMessageL
// Handles client request. If request is not handled here, it is passed to 
// parent class for handling
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::DispatchMessageL()" );

    TInt function = aMessage.Function();

    switch ( function )
        {
        case EAccSrvConnectAccessory2:
            {
            ConnectAccessory2L( aMessage );
            break;
            }
        case EAccSrvDisconnectAccessory:
            {
            DisconnectAccessoryL( aMessage );
            break;
            }
        case EAccSrvSetValueTBool:
            {
            SetValueTBoolL( aMessage );
            break;
            }
        case EAccSrvSetValueTInt:
            {
            SetValueTIntL( aMessage );
            break;
            }
        case EAccSrvSetValueObject:
            {
            SetValueObjectBaseL( aMessage );
            break;   
            }
        case EAccSrvAccessoryValueChangedTBoolNotify:
            {
            AccessoryValueChangedTBoolNotifyL( aMessage );
            break;
            }
        case EAccSrvAccessoryValueChangedTIntNotify:
            {
            AccessoryValueChangedTIntNotifyL( aMessage );
            break;
            }
        case EAccSrvAccessoryValueChangedObjectNotify:
            {
            AccessoryValueChangedObjectNotifyL( aMessage );
            break;
            }
        default:
            {
            // Not handled here, pass to base class
            CAccSrvSubConnection::DispatchMessageL( aMessage );
            break;
            }
        } // switch( function )

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::DispatchMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::ConnectAccessoryL
//
// Preconditions: if unsupported accessory (Unique Id = KErrNotFound)
//                is connected, "Unsupported accessory connected"
//                note is shown in UI.
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::ConnectAccessory2L( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::ConnectAccessoryL()" );

    TAccPolGenericID         genericID;
    TAccSrvGenericIDPckg     genericIDPckg( genericID );
    TAccSrvConnectRecord     connectRecord;
    TAccSrvConnectRecordPckg connectRecordPckg( connectRecord );

    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, connectRecordPckg );

    //Preconditions
    if ( KErrNotFound == genericID.UniqueID() )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::ConnectAccessoryL - Accessory not supported!" );       
	    // Show a notification to the user
        RNotifier notifier;
        TInt err = notifier.Connect();
        if ( err != KErrNone )
            {
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubControl::ConnectAccessoryL - Couldn't connect to notifier: err = %d", err);	
            }
        else
            {
            TPckg<TInt>iReplyPck(0);
			TRequestStatus iStatus =KRequestPending;
			notifier.StartNotifierAndGetResponse(iStatus,
						KAccFwUiNoteNotifierUid,
						KNullDesC8,iReplyPck);
			User::WaitForRequest(iStatus);
			err = iStatus.Int();
			if(KErrNone != err) 
				{
				COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubControl::ConnectAccessoryL - Couldn't start notifier: err = %d", err);				
				}
			notifier.Close();
            }
	    }
    else
        {
		//generate uniques if for connection if not update...
		if ( genericID.UniqueID() == KAccSrvGenerateGID )
		    {	
	    	COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::ConnectAccessoryL() Generate new unique ID" );	
		    iServerModel->CapabilityStorage().GenerateUniqueId( genericID );
		    }
		else
		    {
		    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::ConnectAccessoryL() Updated connection" );	
		    }

    	//read capability buffer
    	CBufFlat* buf = CBufFlat::NewL( connectRecord.iNameValueBufSize );
    	CleanupStack::PushL( buf );
    	buf->ResizeL( connectRecord.iNameValueBufSize );
   		buf->Write( 0, connectRecord.iCapsPtr );

		//open capability steam and read caps to storage
		RBufReadStream rStrm;
		(void) rStrm.Open( *buf ); // void is used to prevent warnings from code scanner.
        rStrm.PushL();             // RBufReadStream::Open does not return any values 			
		TBool isUpdate = iServerModel->CapabilityStorage().NewCapabilityListL( genericID, rStrm);
		rStrm.Close();
		rStrm.Pop(); 

		CleanupStack::PopAndDestroy( buf);

        //Pass handling to Connection Controller
        iConnectionController->HandleConnectL( genericIDPckg(),
                                               connectRecord.iThreadID,
                                               connectRecord.iEvaluateConnectionRules,
                                               isUpdate );

        // return modified generic id to client
        genericIDPckg() = genericID;
        aMessage.WriteL( KAccServParamPosFirst, genericIDPckg );
        }

    Write( aMessage, KAccSrvConnectAccessoryBit );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::ConnectAccessoryL - return void" );
    }

// -----------------------------------------------------------------------------
//  CAccSrvSubControl::DisconnectAccessoryL
//
// Preconditions: if unsupported accessory (Unique Id = KErrNotFound)
//                is disconnected, RMessage2 is just completed.
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::DisconnectAccessoryL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::DisconnectAccessoryL()" );

    TAccPolGenericID genericID;
    TAccSrvGenericIDPckg disconnectGenericIDPackage( genericID );

    aMessage.ReadL( KAccServParamPosFirst, disconnectGenericIDPackage );

    //Preconditions
    if ( KErrNotFound != genericID.UniqueID() )
        {
        iServerModel->IsConnectedOrDetectedL( genericID );//Leaves with KErrArgument if the accessory is not connected or detected    
        iConnectionController->HandleDisconnectL( disconnectGenericIDPackage() ); //Pass handling to Connection Controller
        }

    Write( aMessage, KAccSrvDisconnectAccessoryBit );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::DisconnectAccessoryL - return void" );
    }

// -----------------------------------------------------------------------------
//  CAccSrvSubControl::SetValueTBoolL
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::SetValueTBoolL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::SetValueTBoolL()" );

    TAccPolGenericID             genericID;
    TAccSrvGenericIDPckg         genericIDPckg( genericID );
    TAccPolNameRecord            nameRecord;
    TAccSrvNameRecordPckg        nameRecordPckg( nameRecord );
    TAccSrvValueTypeTBoolPckgBuf valuePckgBuf;
    TASYCommandParamRecord       asyCommandParamRecord;

    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    aMessage.ReadL( KAccServParamPosThird, valuePckgBuf );

    //Precondition
    iServerModel->IsConnectedOrDetectedL( genericID );//Leaves with KErrArgument if the accessory is not connected or detected

    asyCommandParamRecord.iNameRecord = nameRecordPckg();
    asyCommandParamRecord.iCmdValue   = valuePckgBuf().iValue;
    asyCommandParamRecord.iGenericID  = genericID;

	//get capability
	TUint32 name;
	nameRecordPckg().GetName( name );
	CCapValue* value =  iServerModel->CapabilityStorage().GetCapability( genericID, name );
    if ( !value )
        {
    	User::Leave( KErrArgument );
        }

    switch ( value->Location() )
        {
        case EAPVPolicy:
            {
            //Value is located in Accessory Policy DB, request can be completed.
            //The value is stored to Accessory Policy DB in HandleValueMessageL.
            iSetValueTBoolMsg = aMessage;
            HandleValueMessageL( ESetValue,
                                 valuePckgBuf(),
                                 KErrNone, //ErrorCode,
                                 0 ); //TrId, not used at the moment
            }
            break;

        case EAPVAccessory:
            {
            //Forward request to ASY Proxy Handler
            iOutstandingTrId = iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                        ECmdSetValueBool,
                                                        asyCommandParamRecord );
            //Store SetValueTBoolL to queue
            iConnectionController->NotificationQueue().RegisterControlMessage( ESetValue,
                                                                              this);

            //Because SetValueL() is synchronous method, there shouldn't be existing request
            iSetValueTBoolMsg = aMessage;
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            User::Leave( KErrArgument );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::SetValueTBoolL - return void" );
    }

// -----------------------------------------------------------------------------
//  CAccSrvSubControl::SetValueTInt
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::SetValueTIntL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::SetValueTIntL()" );

    TAccPolGenericID            genericID;
    TAccSrvGenericIDPckg        genericIDPckg( genericID );
    TAccPolNameRecord           nameRecord;
    TAccSrvNameRecordPckg       nameRecordPckg( nameRecord );
    TAccSrvValueTypeTIntPckgBuf valuePckgBuf;
    TASYCommandParamRecord      asyCommandParamRecord;

    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    aMessage.ReadL( KAccServParamPosThird, valuePckgBuf );

    //Precondition
    iServerModel->IsConnectedOrDetectedL( genericID );//Leaves with KErrArgument if the accessory is not connected or detected

    asyCommandParamRecord.iNameRecord = nameRecordPckg();
    asyCommandParamRecord.iCmdValue   = valuePckgBuf().iValue;
    asyCommandParamRecord.iGenericID  = genericID;

	//get capability
	TUint32 name;
	nameRecordPckg().GetName( name );
	CCapValue * value =  iServerModel->CapabilityStorage().GetCapability( genericID, name );
    
    if ( !value )
        {
    	User::Leave( KErrArgument );
        }

    switch ( value->Location() )
        {
        case EAPVPolicy:
            {
            //Value is located in Accessory Policy DB, request can be completed.
            //The value is stored to Accessory Policy DB in HandleValueMessageL.
            iSetValueTIntMsg = aMessage;
            HandleValueMessageL( ESetValue,
                                 valuePckgBuf(),
                                 KErrNone, //ErrorCode,
                                 0 ); //TrId, not used at the moment
            }
            break;

        case EAPVAccessory:
            {
            //Forward request to ASY Proxy Handler
            iOutstandingTrId = iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                        ECmdSetValueTInt,
                                                        asyCommandParamRecord );
            //Store SetValueTBoolL to queue
            iConnectionController->NotificationQueue().RegisterControlMessage( ESetValue,
                                                                               this );
            //Because SetValueL() is synchronous method, there shouldn't be existing request
            iSetValueTIntMsg = aMessage;
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            User::Leave( KErrArgument );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::CAccSrvSubControl::SetValueTIntL - return void" );
    }

// -----------------------------------------------------------------------------
//  CAccSrvSubControl::SetValueObjectBaseL
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::SetValueObjectBaseL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::SetValueObjectBaseL()" );
    
    RBufReadStream           rStrm;  
    TAccPolGenericID         genericID;
    TAccSrvGenericIDPckg     genericIDPckg( genericID );
    TAccPolNameRecord        nameRecord;
    TAccSrvNameRecordPckg    nameRecordPckg( nameRecord );
    TASYCommandParamRecord   asyCommandParamRecord;
    
    CBufFlat *buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( aMessage.GetDesMaxLength( KAccServParamPosThird ) );
    
    TPtr8 ptrBuf( buf->Ptr( 0 ) );
    
    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    aMessage.ReadL( KAccServParamPosThird, ptrBuf );
    
    //Precondition
    iServerModel->IsConnectedOrDetectedL( genericID );//Leaves with KErrArgument if the accessory is not connected or detected
    
    asyCommandParamRecord.iNameRecord = nameRecordPckg();
    asyCommandParamRecord.iCmdValue   = 0;
    asyCommandParamRecord.iGenericID  = genericID;

	//get capability
	TUint32 name;
	nameRecordPckg().GetName( name );
	CCapValue * value =  iServerModel->CapabilityStorage().GetCapability( genericID, name );
    
    if ( !value )
        {
    	User::Leave( KErrArgument );
        }

    switch ( value->Location() )
        {
        case EAPVAccessory:
            {
            //Forward request to ASY Proxy Handler
            iOutstandingTrId = iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                        ECmdSetObjectValue,
                                                        asyCommandParamRecord,
                                                        &ptrBuf );
            //Store SetValueTBoolL to queue
            iConnectionController->NotificationQueue().RegisterControlMessage( ESetValue,
                                                                               this );
            //Because SetValueL() is synchronous method, there shouldn't be existing request
            iSetValueObjectMsg = aMessage;
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            User::Leave( KErrArgument );
            }
            break;
        }
    
    CleanupStack::PopAndDestroy( buf );    

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::SetValueObjectBaseL - return void" );
    }
// CAccSrvSubControl::AccessoryValueChangedTBoolNotifyL
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::AccessoryValueChangedTBoolNotifyL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::AccessoryValueChangedTBoolNotifyL()" );

    TAccPolGenericID             genericID;
    TAccSrvGenericIDPckg         genericIDPckg( genericID );
    TAccPolNameRecord            nameRecord;
    TAccSrvNameRecordPckg        nameRecordPckg( nameRecord );
    TAccSrvValueTypeTBoolPckgBuf valuePckgBuf;

    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    aMessage.ReadL( KAccServParamPosThird, valuePckgBuf );

    //Precondition
    iServerModel->IsConnectedOrDetectedL( genericID );//Leaves with KErrArgument if the accessory is not connected or detected

    //get capability and store new value
	TUint32 name;
	nameRecordPckg().GetName( name );

	CCapValue* value =  iServerModel->CapabilityStorage().GetCapability( genericID, name );
    if ( !value )
        {
    	User::Leave( KErrArgument );
        }

 	value->SetValueL( valuePckgBuf().iValue );   

    //Store Value Data for AccessoryValueChanged notification
    TAccValueTypeTInt dummyTIntValue = { 0 };
    TAccValueTypeObject dummyObjectValue( TPtr8( NULL, 0, 0 ) );
    iServerModel->SetValueData( genericID,
                                nameRecordPckg(),
                                dummyTIntValue, /*Not used*/
                                valuePckgBuf(),
                                dummyObjectValue/* Not used*/);

    //complete possible pending notifications about change of accessory value
    iConnectionController->NotificationQueue().CompleteControlMessageL( 
                                                    EAccessoryValueChangedTBool,
                                                    KErrNone,
                                                    genericID.UniqueID() );

    //Update Connection status and Accessory mode (if changed)
    iConnectionController->HandleConnectionChangeValidationL( genericID );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::AccessoryValueChangedTBoolNotifyL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::AccessoryValueChangedTIntNotifyL
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::AccessoryValueChangedTIntNotifyL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::AccessoryValueChangedTIntNotifyL()" );

    TAccPolGenericID            genericID;
    TAccSrvGenericIDPckg        genericIDPckg( genericID );
    TAccPolNameRecord           nameRecord;
    TAccSrvNameRecordPckg       nameRecordPckg( nameRecord );
    TAccSrvValueTypeTIntPckgBuf valuePckgBuf;

    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    aMessage.ReadL( KAccServParamPosThird, valuePckgBuf );

    //Precondition
    iServerModel->IsConnectedOrDetectedL( genericID );//Leaves with KErrArgument if the accessory is not connected or detected

    //get capability and store new value
	TUint32 name;
	nameRecordPckg().GetName( name );

	CCapValue* value = iServerModel->CapabilityStorage().GetCapability( genericID, name );
    if ( !value )
        {
    	User::Leave( KErrArgument );
        }
 
 	value->SetValueL( valuePckgBuf().iValue );       
 	
    //Store Value Data for AccessoryValueChanged notification
    TAccValueTypeTBool dummyTBoolValue = { EFalse };
    TAccValueTypeObject dummyObjectValue( TPtr8( NULL, 0, 0 ) );
    iServerModel->SetValueData( genericID,
                                nameRecordPckg(),
                                valuePckgBuf(),
                                dummyTBoolValue,/*Not used*/
                                dummyObjectValue/*Not used*/ );

    //complete possible pending notifications about change of accessory value
    iConnectionController->NotificationQueue().CompleteControlMessageL( 
                                                    EAccessoryValueChangedTInt,
                                                    KErrNone,
                                                    genericID.UniqueID() );

    //Update Connection status and Accessory mode (if changed)
    iConnectionController->HandleConnectionChangeValidationL( genericID );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::AccessoryValueChangedTIntNotifyL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::AccessoryValueChangedObjectNotifyL
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::AccessoryValueChangedObjectNotifyL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::AccessoryValueChangedObjectNotifyL()" );

    TAccPolGenericID            genericID;
    TAccSrvGenericIDPckg        genericIDPckg( genericID );
    TAccPolNameRecord           nameRecord;
    TAccSrvNameRecordPckg       nameRecordPckg( nameRecord );
    CBufFlat*                   valueBuf = NULL;

    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    
    valueBuf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( valueBuf );    
    valueBuf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
    TPtr8 valueBufPtr( valueBuf->Ptr( 0 ) );
    aMessage.ReadL( KAccServParamPosThird, valueBufPtr );

    //Precondition
    iServerModel->IsConnectedOrDetectedL( genericID );//Leaves with KErrArgument if the accessory is not connected or detected

    //get capability and store new value
    TUint32 name;
    nameRecordPckg().GetName( name );

    CCapValue* value = iServerModel->CapabilityStorage().GetCapability( genericID, name );
    if ( !value )
        {
        User::Leave( KErrArgument );
        }
 
    value->SetValueL( valueBufPtr );
    
    //Store Value Data for AccessoryValueChanged notification
    TAccValueTypeTBool  dummyTBoolValue = { EFalse };
    TAccValueTypeTInt   dummyTIntValue  = { 0 };
    TAccValueTypeObject objectValue( valueBufPtr );
    iServerModel->SetValueData( genericID,
                                nameRecordPckg(),
                                dummyTIntValue, /*Not used*/ 
                                dummyTBoolValue,/*Not used*/
                                objectValue );

    //complete possible pending notifications about change of accessory value
    iConnectionController->NotificationQueue().CompleteControlMessageL( 
                                                    EAccessoryValueChangedObject,
                                                    KErrNone,
                                                    genericID.UniqueID() );

    //Update Connection status and Accessory mode (if changed)
    iConnectionController->HandleConnectionChangeValidationL( genericID );
    
    // Cleanup
    CleanupStack::PopAndDestroy( valueBuf );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::AccessoryValueChangedObjectNotifyL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::CompleteMessageL
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubControl::CompleteMessageL( TMsgID aMsgID,
                                           TInt aErrorCode,
                                           TInt aUniqueID )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubControl::CompleteMessageL(%d)", aMsgID );
    return CAccSrvSubConnection::CompleteMessageL( aMsgID, aErrorCode, aUniqueID );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::HandleValueMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::HandleValueMessageL( TMsgID aMsgID,
                                             TAccValueTypeTBool aValueTBool,
                                             TInt aErrorCode,
                                             TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleValueMessageL() - TBool" );

    switch ( aMsgID )
        {
        case ESetValue:
            {
            HandleSetValueTBoolCompletionL( aErrorCode, aTrId );
            }
            break;

        default:
            {
            //Pass handling to parent class
            CAccSrvSubConnectionBase::HandleValueMessageL( 
                                            aMsgID, 
                                            aValueTBool, 
                                            aErrorCode, 
                                            aTrId );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleValueMessageL - TBool - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::HandleValueMessageL
//
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::HandleValueMessageL( TMsgID aMsgID,
                                             TAccValueTypeTInt aValueTInt,
                                             TInt aErrorCode,
                                             TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleValueMessageL() - TInt" );

    switch ( aMsgID )
        {
        case ESetValue:
            {
            HandleSetValueTIntCompletionL( aErrorCode, aTrId );
            }
            break;

        default:
            {
            //Pass handling to parent class
            CAccSrvSubConnectionBase::HandleValueMessageL( 
                                            aMsgID, 
                                            aValueTInt, 
                                            aErrorCode, 
                                            aTrId );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleValueMessageL - TInt - return void" );
    }
    
// -----------------------------------------------------------------------------
// CAccSrvSubControl::HandleValueMessageL
//
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::HandleValueMessageL( TMsgID aMsgID,
                                             TInt aTrId,
                                             TPtr8* aPtrBuf,
                                             TInt aErrorCode )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleValueMessageL() - Obj" );

    switch ( aMsgID )
        {
        case ESetValue:
            {
            HandleSetValueObjectCompletion( aErrorCode, aTrId );
            }
            break;

        default:
            {
            //Pass handling to parent class
            CAccSrvSubConnectionBase::HandleValueMessageL( aMsgID, 
                                                           aTrId,
                                                           aPtrBuf,
                                                           aErrorCode );
            }
            break;
        }
   
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleValueMessageL - Obj - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::HandleSetValueTIntCompletionL
//
// Complete outstanding SetValueL( TAccValueTypeTInt ) request.
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::HandleSetValueTIntCompletionL( TInt aErrorCode, TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleSetValueTIntCompletionL()" );

    __ASSERT_DEBUG( !iSetValueTIntMsg.IsNull(), User::Invariant() );

    //Check is this correct SetValueL(), i.e. Some other CAccSrvSubControl-subsession
    //might also have a pending SetValueL() request.
    if ( iOutstandingTrId == aTrId )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleSetValueTIntCompletionL - TrId match" );
        //Store a new value to Accessory Policy DB.
        //Storing is done when value location is Accessory Policy DB or accessory.
        TAccPolGenericID            gid;
        TAccSrvGenericIDPckg        gidPckg( gid );
        TAccPolNameRecord           nameRecord;
        TAccSrvNameRecordPckg       nameRecordPckg( nameRecord );
        TAccSrvValueTypeTIntPckgBuf valuePckgBuf;

        iSetValueTIntMsg.ReadL( KAccServParamPosFirst, gidPckg );
        iSetValueTIntMsg.ReadL( KAccServParamPosSecond, nameRecordPckg );
        iSetValueTIntMsg.ReadL( KAccServParamPosThird, valuePckgBuf );

	    //get capability and store new value
		TUint32 name;
		nameRecordPckg().GetName( name);

		CCapValue * value = iServerModel->CapabilityStorage().GetCapability( gid, name );

    	if ( !value )
    	    {
    		User::Leave( KErrArgument );
        	}

 		value->SetValueL( valuePckgBuf().iValue );            

        iOutstandingTrId = 0;//reset transaction ID of this session
        iSetValueTIntMsg.Complete( aErrorCode );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleSetValueTIntCompletionL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::HandleSetValueTBoolCompletionL
//
// Complete outstanding SetValueL( TAccValueTypeTBool ) request.
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::HandleSetValueTBoolCompletionL( TInt aErrorCode, TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleSetValueTBoolCompletionL()" );

    __ASSERT_DEBUG( !iSetValueTBoolMsg.IsNull(), User::Invariant() );

    //Check is this correct SetValueL(), i.e. Some other CAccSrvSubControl-subsession
    //might also have a pending SetValueL() request.
    if ( iOutstandingTrId == aTrId )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleSetValueTBoolCompletionL - TrId match" );
        //Store a new value to Accessory Policy DB.
        //Storing is done when value location is Accessory Policy DB or accessory.
        TAccPolGenericID            gid;
        TAccSrvGenericIDPckg        gidPckg( gid );
        TAccPolNameRecord           nameRecord;
        TAccSrvNameRecordPckg       nameRecordPckg( nameRecord );
        TAccSrvValueTypeTBoolPckgBuf valuePckgBuf;

        iSetValueTBoolMsg.ReadL( KAccServParamPosFirst, gidPckg );
        iSetValueTBoolMsg.ReadL( KAccServParamPosSecond, nameRecordPckg );
        iSetValueTBoolMsg.ReadL( KAccServParamPosThird, valuePckgBuf );

	    //get capability and store new value
		TUint32 name;
		nameRecordPckg().GetName( name );

		CCapValue * value = iServerModel->CapabilityStorage().GetCapability( gid, name );

    	if ( !value )
            {
    		User::Leave( KErrArgument );
        	}

 		value->SetValueL( valuePckgBuf().iValue ); 

        iOutstandingTrId = 0;//reset transaction ID of this session
        iSetValueTBoolMsg.Complete( aErrorCode );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleSetValueTBoolCompletionL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubControl::HandleSetValueObjectCompletion
//
// Complete outstanding SetValueL( Object type ) request.
// -----------------------------------------------------------------------------
//
void CAccSrvSubControl::HandleSetValueObjectCompletion( TInt aErrorCode, TInt aTrId )
    {

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleSetValueObjectCompletion()" );

     __ASSERT_DEBUG( !iSetValueObjectMsg.IsNull(), User::Invariant() );

    //Check is this correct SetValueL(), i.e. Some other CAccSrvSubControl-subsession
    //might also have a pending SetValueL() request.
    if ( iOutstandingTrId == aTrId )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleSetValueTIntCompletionL - TrId match" );

        iOutstandingTrId = 0;//reset transaction ID of this session
        iSetValueObjectMsg.Complete( aErrorCode );
        }    

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubControl::HandleSetValueObjectCompletion - return void" );
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
