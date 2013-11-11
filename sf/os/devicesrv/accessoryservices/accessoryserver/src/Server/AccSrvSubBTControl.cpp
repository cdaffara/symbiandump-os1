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
* Description:  Server-side sub-session of Accessory BT Control
*
*/



// INCLUDE FILES
#include "AccSrvSubBTControl.h"
#include "acc_debug.h"
#include "AccPolGenericIDArray.h"
#include "AccPolGenericIDArrayAccessor.h"
#include "AccSrvServerModel.h"
#include "AccPolGenericIDAccessor.h"
#include "AccPolicyDB.h"
#include "AccPolAccessoryPolicy.h"
#include "AccPolCapabilityStorage.h"

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
// CAccSrvSubBTControl::CAccSrvSubBTControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubBTControl::CAccSrvSubBTControl( 
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    : CAccSrvSubControl ( aProxy, aConnectionController,aServerModel ),
    iAudioLinkOpenCancelFlagResetCalledTwice( EFalse ),
    iAudioLinkCloseCancelFlagResetCalledTwice( EFalse )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvSubBTControl::CAccSrvSubBTControl(0x%x, 0x%x)", aProxy, &aConnectionController );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::CAccSrvSubBTControl - return void" );
    }
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::ConstructL()" );  
    
    CAccSrvSubControl::ConstructL();
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::ConstructL- return void" ); 
    }

// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CAccSrvSubBTControl* CAccSrvSubBTControl::NewL( 
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvSubBTControl::NewL(0x%x, 0x%x)", aProxy, &aConnectionController );

    __ASSERT_ALWAYS( aProxy, User::Leave( KErrArgument ) );
    CAccSrvSubBTControl* self = new (ELeave) CAccSrvSubBTControl( aProxy, 
                                                                  aConnectionController,
                                                                  aServerModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubBTControl::NewL - return 0x%x", self );

    return self;
    }

// Destructor
CAccSrvSubBTControl::~CAccSrvSubBTControl()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::~CAccSrvSubBTControl()" );
    
    RMessage2 dummy;
    
    CAccSrvSubBTControl::CancelConnectAccessory( dummy );//For iConnectBluetoothAccessoryMsg
    CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkCloseReq( dummy );//For iNotifyBluetoothAudioLinkCloseReqMsg
    CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkOpenReq( dummy );//For iNotifyBluetoothAudioLinkOpenReqMsg
    
    if ( iServerModel )
        {
        iServerModel->SetAudioLinkOpenCancelled( EFalse );
        iServerModel->SetAudioLinkCloseCancelled( EFalse );
        }
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::~CAccSrvSubBTControl - return" );
    }
    
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::BluetoothConnectAccessoryL
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::BluetoothConnectAccessoryL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothConnectAccessoryL()" );
    
    TBTDevAddr              BTDevAddr;
    TPckg<TBTDevAddr>       BTDevAddrPckg( BTDevAddr );
    TUint                   callerThreadID( 0 );
    TPckg<TUint>            callerThreadIDPckg( callerThreadID );
    TBuf8<KGIDIntMaxLength> GIDIntEmpty( KNullDesC8 );
        
    aMessage.ReadL( KAccServParamPosFirst, BTDevAddrPckg );
    aMessage.ReadL( KAccServParamPosSecond, callerThreadIDPckg );

    // A new GenericID for connection made without GenericID.
    // Rest of it is filled after supported profiles has been queried from BT-ASY
    // (see CAccSrvConnectionHandler::ResolveGenericIDL() ).
    TAccPolGenericID genericID;
    TAccPolGenericIDAccessor::SetDeviceAddress( genericID, 
                                                MakeTUint64( BTDevAddr ) );
    TAccPolGenericIDAccessor::SetFeatureAttributeL( genericID, 
                                                    KAccPolPCPhysicalConnection, 
                                                    KPCBluetooth );

    iServerModel->CapabilityStorage().GenerateUniqueId( genericID );
    iServerModel->CapabilityStorage().NewCapabilityListL( genericID );
                                                    
    // Pass handling to Connection Controller.
    // If Leave occurs, aMessage is completed with error code.
    iConnectionController->HandleConnectL( genericID, callerThreadID, EFalse, EFalse );
                                                 
    // Add this subsession to "ConnectAccessory"-queue for later i completion
    iConnectionController->NotificationQueue().RegisterControlMessage( EConnectAccessory,
                                                                       this );
    iConnectBluetoothAccessoryMsg = aMessage;//Store RMessage2 for future completion
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothConnectAccessoryL - return void" ); 
    }
    
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::BluetoothDisconnectAccessoryL
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::BluetoothDisconnectAccessoryL( const RMessage2& aMessage )
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothDisconnectAccessoryL()" ); 
    
    TInt                  index( KErrNotFound );
    TAccPolGenericID      genericId;
    TAccPolGenericIDArray genericIdArray;
    TBTDevAddr            BTDevAddr;
    TPckg<TBTDevAddr>     BTDevAddrPckg( BTDevAddr );
    
    aMessage.ReadL( KAccServParamPosFirst, BTDevAddrPckg );
    
    //Find the GenericID in question
    iServerModel->CurrentConnectionStatusL( genericIdArray );//Fill the array	
    index = TAccPolGenericIDArrayAccessor::FindWithDeviceAddressL(
                                                genericIdArray,
                                                MakeTUint64( BTDevAddr ) );
               
    if ( index < 0 )
        {
        User::Leave( KErrArgument );        
        }
        
    //Pass handling to Connection Controller   
    iConnectionController->HandleDisconnectL( genericIdArray.GetGenericIDL( index ) );
    
    Write( aMessage, KAccSrvDisconnectBTAccessoryBit );
                        
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothDisconnectAccessoryL - return void" );	
	}

// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::BluetoothAudioLinkOpenRespL
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::BluetoothAudioLinkOpenRespL( const RMessage2& aMessage )
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothAudioLinkOpenRespL()" );
    
    TInt              index( KErrNotFound );
    TInt              uniqueID( KErrNotFound );
    TBTDevAddr        BTDevAddr;
   	TPckg<TBTDevAddr> BTDevAddrPckg( BTDevAddr );
   	TInt              response( 0 );
    TPckg<TInt>       responsePckg( response );
     
    aMessage.ReadL( KAccServParamPosFirst, BTDevAddrPckg );
    aMessage.ReadL( KAccServParamPosSecond, responsePckg ); 
	
	//Find the GenericID in question	
    index = iServerModel->FindWithDeviceAddressL( 
                                MakeTUint64( BTDevAddr ),
                                uniqueID /*Unique DB id of the found accessory*/ );

    if ( index < 0 )
        {
        User::Leave( KErrArgument );        
        }
         
    if ( iServerModel->AudioLinkOpenCancelled() )
        {
        //AccessoryAudioLinkOpen() request was cancelled before this response was received.
        //This response can be completed with error code.
        ResetAudioLinkOpenCancelFlag( response );
        User::Leave( KErrCancel ); 
        }
    else
        {
   	    iConnectionController->NotificationQueue().CompleteControlMessageL( 
            EAccessoryAudioLinkOpen, response, uniqueID );
        }
           	                                                                    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothAudioLinkOpenRespL - return void" );
	}

// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::BluetoothAudioLinkCloseRespL
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::BluetoothAudioLinkCloseRespL( const RMessage2& aMessage )
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothAudioLinkCloseRespL()" );
    
    TInt              index( KErrNotFound );
    TInt              uniqueID( KErrNotFound );
    TBTDevAddr        BTDevAddr;
   	TPckg<TBTDevAddr> BTDevAddrPckg( BTDevAddr );
   	TInt              response( 0 );
    TPckg<TInt>       responsePckg( response );
     
    aMessage.ReadL( KAccServParamPosFirst, BTDevAddrPckg );
    aMessage.ReadL( KAccServParamPosSecond, responsePckg ); 
	
	//Find the GenericID in question	
    index = iServerModel->FindWithDeviceAddressL( 
                                MakeTUint64( BTDevAddr ),
                                uniqueID /*Unique DB id of the found accessory*/ );

    if ( index < 0 )
        {
        User::Leave( KErrArgument );        
        }
                
    if ( iServerModel->AudioLinkCloseCancelled() )
        {
        //AccessoryAudioLinkClose() request was cancelled before this response was received.
        //This response can be completed with error code.
        ResetAudioLinkCloseCancelFlag( response );
        User::Leave( KErrCancel ); 
        }
    else
        {
   	    iConnectionController->NotificationQueue().CompleteControlMessageL( 
            EAccessoryAudioLinkClose, response, uniqueID );
        }
   	                                                                    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothAudioLinkCloseRespL - return void" );	
	}

// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::BluetoothAudioLinkOpenedNotifyL
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::BluetoothAudioLinkOpenedNotifyL( const RMessage2& aMessage )
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothAudioLinkOpenedNotifyL()" );

    TInt                 index( KErrNotFound );
    TInt                 uniqueID( KErrNotFound );
    TInt                 currentAccessoryModeUniqueID = iServerModel->UniqueID();//Preferred audio output status is assigned for this uniqueID
    TBTDevAddr           BTDevAddr;
   	TPckg<TBTDevAddr>    BTDevAddrPckg( BTDevAddr );
   	TAccAudioType        audioType( EAccMonoAudio ); // Initialization value
    TAccSrvAudioTypePckg audioTypePckg( audioType );		
    
    aMessage.ReadL( KAccServParamPosFirst, BTDevAddrPckg );
    aMessage.ReadL( KAccServParamPosSecond, audioTypePckg );
    
    //Find the GenericID in question	
    index = iServerModel->FindWithDeviceAddressL( 
                                MakeTUint64( BTDevAddr ),
                                uniqueID /*Unique id of the found accessory*/ );               
    if ( index < 0 )
        {
        User::Leave( KErrArgument );        
        }

            
    if ( iServerModel->AudioLinkOpenCancelled() )
        {
        //AccessoryAudioLinkOpen() request was cancelled before this response was received.
        //This response can be completed with error code.
        ResetAudioLinkOpenCancelFlag( KErrNone );
        User::Leave( KErrCancel ); 
        }
    else
        {
        iServerModel->SetAudioType( audioType );//Notice, This works for single client (Server model can store only one value)            
        iConnectionController->NotificationQueue().CompleteControlMessageL( 
            EAccessoryAudioLinkOpened, KErrNone, uniqueID );
        }
   
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothAudioLinkOpenedNotifyL - return void" );	
	}
	
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::BluetoothAudioLinkClosedNotify
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::BluetoothAudioLinkClosedNotifyL( const RMessage2& aMessage )
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothAudioLinkClosedNotifyL()" );

    TInt                 index( KErrNotFound );
    TInt                 uniqueID( KErrNotFound );
    TBTDevAddr           BTDevAddr;
   	TPckg<TBTDevAddr>    BTDevAddrPckg( BTDevAddr );
   	TAccAudioType        audioType( EAccMonoAudio ); // Initialization value
    TAccSrvAudioTypePckg audioTypePckg( audioType );		
    
    aMessage.ReadL( KAccServParamPosFirst, BTDevAddrPckg );
    aMessage.ReadL( KAccServParamPosSecond, audioTypePckg );
    
    //Find the GenericID in question
    index = iServerModel->FindWithDeviceAddressL( 
                                MakeTUint64( BTDevAddr ),
                                uniqueID /*Unique id of the found accessory*/ );
               
    if ( index < 0 )
        {
        User::Leave( KErrArgument );        
        }
          
    if ( iServerModel->AudioLinkCloseCancelled() )
        {
        //AccessoryAudioLinkClose() request was cancelled before this response was received.
        //This response can be completed with error code.
        ResetAudioLinkCloseCancelFlag( KErrNone );
        User::Leave( KErrCancel ); 
        }
    else
        {
        iServerModel->SetAudioType( audioType );//Notice, This works for single client (Server model can store only one value)            
   	    iConnectionController->NotificationQueue().CompleteControlMessageL( 
            EAccessoryAudioLinkClosed, KErrNone, uniqueID );
        }
       
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::BluetoothAudioLinkClosedNotifyL - return void" );	
	}

// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::NotifyBluetoothAudioLinkOpenReqL
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::NotifyBluetoothAudioLinkOpenReqL( 
    const RMessage2& aMessage )
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::NotifyBluetoothAudioLinkOpenReqL()" );
 
    iNotifyBluetoothAudioLinkOpenReqMsg = aMessage;//Store RMessage2 for future completion
    
    iConnectionController->NotificationQueue().RegisterControlMessage( 
        EBluetoothAudioLinkOpenReq, this );    
    
    if ( iServerModel->BluetoothAudioLinkOpenReqPostponed() )
        {        
        //Complete notification immediately if audio link open is still in progress
        if ( !iServerModel->AudioLinkOpenCancelled() &&
              iConnectionController->NotificationQueue().ControlMessagesWaiting( EAccessoryAudioLinkOpen ) > 0 )
            {
        	TInt              uniqueID( KErrNotFound );
            TBTDevAddr        BTDevAddr;
            TPckg<TBTDevAddr> BTDevAddrPckg( BTDevAddr );
            
            aMessage.ReadL( KAccServParamPosFirst, BTDevAddrPckg );
			// coverity[check_return]
			// coverity[unchecked_value]
            iServerModel->FindWithDeviceAddressL( MakeTUint64( BTDevAddr ), uniqueID );
            iConnectionController->NotificationQueue().CompleteControlMessageL(
                EBluetoothAudioLinkOpenReq, KErrNone, uniqueID );
            }
        }
    iServerModel->SetBluetoothAudioLinkOpenReqPostponed( EFalse );//Postpone is not needed anymore
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::NotifyBluetoothAudioLinkOpenReqL - return void" );	
	}

// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::NotifyBluetoothAudioLinkCloseReqL
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::NotifyBluetoothAudioLinkCloseReqL( 
    const RMessage2& aMessage )
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::NotifyBluetoothAudioLinkCloseReqL()" );

    iNotifyBluetoothAudioLinkCloseReqMsg = aMessage;//Store RMessage2 for future completion
                
    iConnectionController->NotificationQueue().RegisterControlMessage( 
        EBluetoothAudioLinkCloseReq, this );
        
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::NotifyBluetoothAudioLinkCloseReqL - return void" );	
	}
		
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::NotifyBTAccessoryValueChangedTBoolL
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::NotifyBTAccessoryValueChangedTBoolL( 
    const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::NotifyBTAccessoryValueChangedTBoolL()" );
    
    TInt                         index( KErrNotFound );
    TAccPolGenericIDArray        genericIdArray;
    TAccPolGenericID             genericID;
    TBTDevAddr                   BTDevAddr;
   	TPckg<TBTDevAddr>            BTDevAddrPckg( BTDevAddr );
    TAccPolNameRecord            nameRecord;
    TAccSrvNameRecordPckg        nameRecordPckg( nameRecord );
    TAccSrvValueTypeTBoolPckgBuf valuePckgBuf;
            
    aMessage.ReadL( KAccServParamPosFirst, BTDevAddrPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    aMessage.ReadL( KAccServParamPosThird, valuePckgBuf );
          
    //Find the GenericID in question
    iServerModel->CurrentConnectionStatusL( genericIdArray );//Fill the array
    index = TAccPolGenericIDArrayAccessor::FindWithDeviceAddressL(
                                                genericIdArray,
                                                MakeTUint64( BTDevAddr ) );
    if ( index < 0 )
        {
        User::Leave( KErrArgument );        
        }
    genericID = genericIdArray.GetGenericIDL( index );
 
    //Check is the capability assigned for this accessory
	TUint32 name;
	nameRecord.GetName( name );
    
    CCapValue * value = iServerModel->CapabilityStorage().GetCapability( genericID, name );
    
    if ( !value ) 
        {
        User::Leave( KErrArgument );
        }
 	
    //Store a new value
    value->SetValueL( valuePckgBuf().iValue );
                                                    
    //Store Value Data for AccessoryValueChanged notification
    TAccValueTypeTInt dummyTIntValue = { 0 };
    TAccValueTypeObject dummyObjectValue( TPtr8( NULL, 0, 0 ) );
    iServerModel->SetValueData( genericID,
    	                        nameRecord,
    	                        dummyTIntValue, /*Not used*/
    	                        valuePckgBuf(),
    	                        dummyObjectValue/*Not used*/ ); 
    		                                       
    //complete possible pending notifications about change of accessory value    		        
    iConnectionController->NotificationQueue().CompleteControlMessageL(
        EAccessoryValueChangedTBool, KErrNone, genericID.UniqueID() );
    
    //Update Connection status and Accessory mode (if changed)
    iConnectionController->HandleConnectionChangeValidationL( genericID );
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::NotifyBTAccessoryValueChangedTBoolL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::NotifyBTAccessoryValueChangedTIntL
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::NotifyBTAccessoryValueChangedTIntL( 
    const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::NotifyBTAccessoryValueChangedTIntL()" );
    
    TInt                         index( KErrNotFound );
    TAccPolGenericIDArray        genericIdArray;
    TAccPolGenericID             genericID;
    TBTDevAddr                   BTDevAddr;
   	TPckg<TBTDevAddr>            BTDevAddrPckg( BTDevAddr );
    TAccPolNameRecord            nameRecord;
    TAccSrvNameRecordPckg        nameRecordPckg( nameRecord );
    TAccSrvValueTypeTIntPckgBuf  valuePckgBuf;
            
    aMessage.ReadL( KAccServParamPosFirst, BTDevAddrPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    aMessage.ReadL( KAccServParamPosThird, valuePckgBuf );
          
    //Find the GenericID in question
    iServerModel->CurrentConnectionStatusL( genericIdArray );//Fill the array
    index = TAccPolGenericIDArrayAccessor::FindWithDeviceAddressL(
                                                genericIdArray,
                                                MakeTUint64( BTDevAddr ) );
    if ( index < 0 )
        {
        User::Leave( KErrArgument );        
        }
    genericID = genericIdArray.GetGenericIDL( index );
 
    //Check is the capability assigned for this accessory
	TUint32 name;
	nameRecord.GetName( name );
    
    CCapValue * value = iServerModel->CapabilityStorage().GetCapability( genericID, name );
    
    if ( !value ) 
        {
        User::Leave( KErrArgument );
        }
 	
    //Store a new value
    value->SetValueL( valuePckgBuf().iValue );
                                                    
    //Store Value Data for AccessoryValueChanged notification
    TAccValueTypeTBool dummyTBoolValue = { 0 };
    TAccValueTypeObject dummyObjectValue( TPtr8( NULL, 0, 0 ) );
    iServerModel->SetValueData( genericID,
    	                        nameRecord,
    	                        valuePckgBuf(),
    	                        dummyTBoolValue /*Not used*/,
    	                        dummyObjectValue/*Not used*/); 
    		                                       
    //complete possible pending notifications about change of accessory value    		        
    iConnectionController->NotificationQueue().CompleteControlMessageL(
        EAccessoryValueChangedTInt, KErrNone, genericID.UniqueID() );
    
    //Update Connection status and Accessory mode (if changed)
    iConnectionController->HandleConnectionChangeValidationL( genericID );
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::NotifyBTAccessoryValueChangedTIntL - return void" );
    }
	
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkOpenReq
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkOpenReq( 
    const RMessage2& /*aMessage*/ )
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkOpenReq()" );

    // If request is Null, there is nothing to cancel
    if ( !iNotifyBluetoothAudioLinkOpenReqMsg.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
            EBluetoothAudioLinkOpenReq, this );
        Write( iNotifyBluetoothAudioLinkOpenReqMsg, KAccSrvNotifyBluetoothAudioLinkOpenReqBit );
        iNotifyBluetoothAudioLinkOpenReqMsg.Complete( KErrCancel );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkOpenReq - return void" );	
	}
	
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkCloseReq
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkCloseReq( 
    const RMessage2& /*aMessage*/ )
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkCloseReq()" );

    // If request is Null, there is nothing to cancel
    if ( !iNotifyBluetoothAudioLinkCloseReqMsg.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
            EBluetoothAudioLinkCloseReq, this );
        Write( iNotifyBluetoothAudioLinkCloseReqMsg, KAccSrvNotifyBluetoothAudioLinkCloseReqBit );
        iNotifyBluetoothAudioLinkCloseReqMsg.Complete( KErrCancel );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::CancelNotifyBluetoothAudioLinkCloseReq - return void" );	
	}
	
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::CancelConnectAccessory
// 
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::CancelConnectAccessory( const RMessage2& /*aMessage*/)
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::CancelConnectAccessory()" );

    // If request is Null, there is nothing to cancel
    if ( !iConnectBluetoothAccessoryMsg.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( EConnectAccessory,
                                                                         this);
                                                                       
        //Read the accessory in question
        TBTDevAddr              BTDevAddr;
        TPckg<TBTDevAddr>       BTDevAddrPckg( BTDevAddr );
        TInt err = iConnectBluetoothAccessoryMsg.Read( 0, BTDevAddrPckg );
           
        if ( KErrNone == err )
            {
            TRAP( err, iServerModel->SetCancelledConnectAccessoryL( MakeTUint64( BTDevAddr ) ) );
            Write( iConnectBluetoothAccessoryMsg, KAccSrvConnectBTAccessoryBit );
            iConnectBluetoothAccessoryMsg.Complete( KErrCancel );
            }
        else
            {
            TRACE_ASSERT_ALWAYS;
            }            
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::CancelConnectAccessory - return void" );	
	}
	
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::CancelDisconnectAccessory
// 
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::CancelDisconnectAccessory( const RMessage2& /*aMessage*/)
	{
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::CancelDisconnectAccessory()" );

    // Current implementation doesn't support DisconnectAccessory() cancel
    // i.e. DisconnectAccessory() is processed before cancel can be processed.
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::CancelDisconnectAccessory - return void" );	
	}

// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::DispatchMessageL
// Handles client request. If request is not handled here, it is passed to 
// parent class for handling
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::DispatchMessageL()" );

    TInt function = aMessage.Function();

    switch ( function )
        {
        case EAccSrvBluetoothConnectAccessory:
            {
            BluetoothConnectAccessoryL( aMessage );
            break;
            }
        case EAccSrvBluetoothDisconnectAccessory:
            {
            BluetoothDisconnectAccessoryL( aMessage );
            break;
            }
        case EAccSrvBluetoothAudioLinkOpenResp:
            {
            BluetoothAudioLinkOpenRespL( aMessage );
            break;
            }
        case EAccSrvBluetoothAudioLinkCloseResp:
            {
            BluetoothAudioLinkCloseRespL( aMessage );
            break;
            }
        case EAccSrvBluetoothAudioLinkOpenedNotify:
            {
            BluetoothAudioLinkOpenedNotifyL( aMessage );
            break;
            }
        case EAccSrvBluetoothAudioLinkClosedNotify:
            {
            BluetoothAudioLinkClosedNotifyL( aMessage );
            break;
            }
        case EAccSrvNotifyBluetoothAudioLinkOpenReq:
            {
            NotifyBluetoothAudioLinkOpenReqL( aMessage );
            break;
            }
        case EAccSrvNotifyBluetoothAudioLinkCloseReq:
            {
            NotifyBluetoothAudioLinkCloseReqL( aMessage );
            break;
            }
        case EAccSrvBTAccessoryValueChangedTBoolNotify:
            {
            NotifyBTAccessoryValueChangedTBoolL( aMessage );
            break;
            }
        case EAccSrvBTAccessoryValueChangedTIntNotify:
            {
            NotifyBTAccessoryValueChangedTIntL( aMessage );
            break;
            }
        case EAccSrvCancelNotifyBluetoothAudioLinkOpenReq:
            {
            CancelNotifyBluetoothAudioLinkOpenReq( aMessage );
            break;
            }
        case EAccSrvCancelNotifyBluetoothAudioLinkCloseReq:
            {
            CancelNotifyBluetoothAudioLinkCloseReq( aMessage );
            break;
            }
        case EAccSrvCancelConnectAccessory:
            {
            CancelConnectAccessory( aMessage );
            break;
            }
        case EAccSrvCancelDisconnectAccessory:
            {
            CancelDisconnectAccessory( aMessage );
            break;
            }
        default:
            {
            // Not handled here, pass to base class
            CAccSrvSubControl::DispatchMessageL( aMessage );
            break;
            }
        } // switch( function )

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::DispatchMessageL - return void" );
    }
	
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::CompleteMessageL
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubBTControl::CompleteMessageL( TMsgID aMsgID,
                                             TInt aErrorCode,
                                             TInt aUniqueID )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvSubBTControl::CompleteMessageL(MsgID=%d Err=%d)", aMsgID, aErrorCode );
     
    TBool messageConsumption( EFalse );// Return value tells should this subsession be removed from the queue
     
    switch ( aMsgID )
    	{
    	case EConnectAccessory:
    		{
    		//Complete outstanding ConnectAccessory request for BT
    		if ( !iConnectBluetoothAccessoryMsg.IsNull() )
                {
                Write( iConnectBluetoothAccessoryMsg, KAccSrvConnectBTAccessoryBit );
    		    iConnectBluetoothAccessoryMsg.Complete( aErrorCode );
    		    messageConsumption = ETrue;
                }
    		}
    		break;
    	case EBluetoothAudioLinkOpenReq:
    		{
    		//Complete outstanding AccessoryAudioLinkOpen request
    		COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubBTControl::CompleteMessageL - AudioLinkOpenReq AudioType=%d)", iServerModel->AudioType() );    		
    		
    		TAccPolGenericID genericID;
    		iServerModel->FindWithUniqueIDL( aUniqueID, genericID );
    		TBTDevAddr BTDevAddr( genericID.DeviceAddress() );
    		TPckg<TBTDevAddr> BTDevAddrPackage( BTDevAddr );
        	iNotifyBluetoothAudioLinkOpenReqMsg.WriteL( KAccServParamPosFirst, BTDevAddrPackage );
    		
    		TAccSrvAudioTypePckg audioTypePackage( iServerModel->AudioType() );//Notice, This works for single client (Server model can store only one value)
    		iNotifyBluetoothAudioLinkOpenReqMsg.WriteL( KAccServParamPosSecond, audioTypePackage );
    		Write( iNotifyBluetoothAudioLinkOpenReqMsg,
                   KAccSrvNotifyBluetoothAudioLinkOpenReqBit );
    		
    		iNotifyBluetoothAudioLinkOpenReqMsg.Complete( aErrorCode );
    		messageConsumption = ETrue;
    		}
    		break;
    		
    	case EBluetoothAudioLinkCloseReq:
    		{
    	    //Complete outstanding AccessoryAudioLinkOpen request
    	    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubBTControl::CompleteMessageL - AudioLinkCloseReq AudioType=%d)", iServerModel->AudioType() );    		
    	    
    	    TAccPolGenericID genericID;
    		iServerModel->FindWithUniqueIDL( aUniqueID, genericID );
    		TBTDevAddr BTDevAddr( genericID.DeviceAddress() );
    		TPckg<TBTDevAddr> BTDevAddrPackage( BTDevAddr );
        	iNotifyBluetoothAudioLinkCloseReqMsg.WriteL( KAccServParamPosFirst, BTDevAddrPackage );
        	
    		TAccSrvAudioTypePckg audioTypePackage( iServerModel->AudioType() );//Notice, This works for single client (Server model can store only one value)
    		iNotifyBluetoothAudioLinkCloseReqMsg.WriteL( KAccServParamPosSecond, audioTypePackage );    	    
    	    Write( iNotifyBluetoothAudioLinkCloseReqMsg, 
                   KAccSrvNotifyBluetoothAudioLinkCloseReqBit );
    	    
    		iNotifyBluetoothAudioLinkCloseReqMsg.Complete( aErrorCode );
    		messageConsumption = ETrue;
    		}
    		break;
    		
    	default:
    		{
    		//Pass handling to parent class
    		messageConsumption = CAccSrvSubControl::CompleteMessageL( aMsgID, 
    		                                                          aErrorCode, 
    		                                                          aUniqueID );
    		}
    	}
    	
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubBTControl::CompleteMessageL - return 0x%x", messageConsumption );
    
    return messageConsumption;
    }
    
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::ResetAudioLinkOpenCancelFlag
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::ResetAudioLinkOpenCancelFlag( TInt aResponse )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubBTControl::ResetAudioLinkOpenCancelFlag(response=%d)", aResponse );

    if ( iAudioLinkOpenCancelFlagResetCalledTwice || ( KErrNone != aResponse ) )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::ResetAudioLinkOpenCancelFlag - second call" );
        iServerModel->SetAudioLinkOpenCancelled( EFalse );
        iAudioLinkOpenCancelFlagResetCalledTwice = EFalse;
        }
    else
        {
        iAudioLinkOpenCancelFlagResetCalledTwice = ETrue;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::ResetAudioLinkOpenCancelFlag - return void" );
    }
    
// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::ResetAudioLinkCloseCancelFlag
// -----------------------------------------------------------------------------
//
void CAccSrvSubBTControl::ResetAudioLinkCloseCancelFlag( TInt aResponse )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubBTControl::ResetAudioLinkCloseCancelFlag(response=%d)", aResponse );

    if ( iAudioLinkCloseCancelFlagResetCalledTwice || ( KErrNone != aResponse ) )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::ResetAudioLinkCloseCancelFlag - second call" );
        iServerModel->SetAudioLinkCloseCancelled( EFalse );
        iAudioLinkCloseCancelFlagResetCalledTwice = EFalse;
        }
    else
        {
        iAudioLinkCloseCancelFlagResetCalledTwice = ETrue;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBTControl::ResetAudioLinkCloseCancelFlag - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubBTControl::MakeTUint64
// -----------------------------------------------------------------------------
//
TUint64 CAccSrvSubBTControl::MakeTUint64( const TBTDevAddr& aBTDevAddr )
    {
#ifdef _DEBUG
    TBuf<12> bdaddrbuf;
    aBTDevAddr.GetReadable( bdaddrbuf );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubBTControl::MakeTUint64(%S)", &bdaddrbuf );
#endif
    
    TUint32 bTDevAddrHigh( 0 );
    TUint32 bTDevAddrLow( 0 );
    
    bTDevAddrHigh += aBTDevAddr[1]<<(0*8);
    bTDevAddrHigh += aBTDevAddr[0]<<(1*8);	        
    //bTDevAddrHigh += 0<<(2*8)//The size of a bluetooth device address in bytes is 6 (KBTDevAddrSize)
    //bTDevAddrHigh += 0<<(3*8)//The size of a bluetooth device address in bytes is 6 (KBTDevAddrSize)
    
    bTDevAddrLow += aBTDevAddr[5]<<(0*8);
    bTDevAddrLow += aBTDevAddr[4]<<(1*8);	        
    bTDevAddrLow += aBTDevAddr[3]<<(2*8);
    bTDevAddrLow += aBTDevAddr[2]<<(3*8);
                               
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvSubBTControl::MakeTUint64 - return (h:0x%x, l:0x%x)", bTDevAddrHigh, bTDevAddrLow );	
        
    return MAKE_TUINT64 (bTDevAddrHigh, bTDevAddrLow);
    }
            
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
