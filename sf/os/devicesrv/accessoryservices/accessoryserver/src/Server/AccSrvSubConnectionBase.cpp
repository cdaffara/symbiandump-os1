/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server-side sub-session of Accessory Connection Base
*
*/



// INCLUDE FILES
#include "AccSrvSubConnectionBase.h"
#include "AccPolGenericIDAccessor.h"
#include "acc_debug.h"
#include "AccSrvASYProxyHandler.h"
#include "AccPolAccessoryPolicy.h"
#include <s32mem.h>
#include <accpolobjectcon.h>

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
// CAccSrvSubConnectionBase::CAccSrvSubConnectionBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubConnectionBase::CAccSrvSubConnectionBase(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    : CAccSrvSubBase( aProxy, aConnectionController, aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::CAccSrvSubConnectionBase()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::CAccSrvSubConnectionBase - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::ConstructL" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::ConstructL - return void" );
    }

// Destructor
CAccSrvSubConnectionBase::~CAccSrvSubConnectionBase()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::~CAccSrvSubConnectionBase()" );

    // Complete possible outstanding request
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( KErrCancel );
        }
        
     if( iDataBuf )
        {
        delete iDataBuf;
        iDataBuf = NULL;
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::~CAccSrvSubConnectionBase - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::GetSubblockNameArrayL
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::GetSubblockNameArrayL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetSubblockNameArrayL()" );
    
    TInt                   err( KErrNone );
    TAccPolGenericID       genericID;
    TAccSrvGenericIDPckg   genericIDPckg( genericID );
    RBufWriteStream        wStrm;
    TInt                   bufSize( aMessage.Int1() );
    CBufFlat*              buf = CBufFlat::NewL( bufSize );
    CleanupStack::PushL( buf );
    
    //Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
       
    // Externalize
    (void) wStrm.Open(*buf); 
    CleanupClosePushL( wStrm );
    iServerModel->CapabilityStorage().ExternalizeCapabilityNameListL( genericID, wStrm);
    wStrm.CommitL();
    
    if ( buf->Size() > bufSize )
        {        
        wStrm.Truncate( *buf );
        //A new attempt is made from client side with a bigger buffer.
        //Write only a new buffer size to client for new memory allocation
        COM_TRACE_2( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetSubblockNameArrayL - size=%d reserved size=%d", buf->Size(), bufSize );
        TInt newBufSize( buf->Size() );
        buf->Reset();
        buf->ResizeL( bufSize );
        wStrm.WriteInt32L( newBufSize );
        wStrm.CommitL();
        err = KErrEof;
        }
    
    TPtr8 ptr( buf->Ptr( 0 ) );
    aMessage.Write( KAccServParamPosThird, ptr );
            
    CleanupStack::PopAndDestroy( &wStrm );
    CleanupStack::PopAndDestroy( buf );
    
    User::LeaveIfError( err );
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetSubblockNameArrayL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::GetValueTBoolL
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::GetValueTBoolL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetValueTBoolL()" );

    TAccPolGenericID          genericID;
    TAccSrvGenericIDPckg      genericIDPckg( genericID );
    TAccPolNameRecord         nameRecord;
    TAccSrvNameRecordPckg     nameRecordPckg( nameRecord );
    TAccValueTypeTBool        getValue;
    TAccSrvValueTypeTBoolPckg getValuePckg( getValue );
    TASYCommandParamRecord    asyCommandParamRecord;
    TInt32                    value( 0 );
    TAccValueTypeTBool        boolValue = { EFalse };
    
    //Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    aMessage.ReadL( KAccServParamPosThird, getValuePckg );

    //Precondition
    iServerModel->IsConnectedOrDetectedL( genericID );//Leaves with KErrArgument if the accessory is not connected or detected
    
    //Copy client parameters to format in which they are sent to ASY
    asyCommandParamRecord.iNameRecord = nameRecordPckg();
    asyCommandParamRecord.iCmdValue   = getValue.iValue;
    asyCommandParamRecord.iGenericID  = genericID;

	//get capability
	TUint32 name;
	asyCommandParamRecord.iNameRecord.GetName( name );
	
	CCapValue * capValue = iServerModel->CapabilityStorage().GetCapability( genericID, name );	

	if ( !capValue )
    	{
		User::Leave( KErrArgument );
    	}

    User::LeaveIfError( capValue->ValueInt( value ) );
    
    switch ( capValue->Location() )
        {
        case EAPVPolicy:
            {
            iMessage = aMessage;//copy message for later completion

            if ( KErrUnknown == value )
                {                
                //Value initialization is needed, forward Get-request to ASY
                iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                ECmdGetValueBool,
                                                asyCommandParamRecord );
                //Store this sub-session to queue so that it will be informed when ASY has served request
                iConnectionController->NotificationQueue().RegisterControlMessage(
                                                EGetValue,
                                                this );
                }
            else
                {
                //Value exist in Accessory Policy DB, request can be completed                
                boolValue.iValue = value;
                HandleValueMessageL( EGetValue,
                                     boolValue,
                                     KErrNone,
                                     0 );
                }
            }
            break;

        case EAPVAccessory:
            {
            iMessage = aMessage;//copy message for later completion

            if ( KErrUnknown == value )
                {
                //Forward request to ASY Proxy Handler
                iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                  ECmdGetValueBool,
                                                  asyCommandParamRecord );

                //Store session to queue so that it will be informed when ASY has
                //served request
                iConnectionController->NotificationQueue().RegisterControlMessage(
                                                    EGetValue,
                                                    this );
                }
            else
                {
                 //Value exist in Accessory Policy DB, request can be completed
                boolValue.iValue = value;
                HandleValueMessageL( EGetValue,
                                     boolValue,
                                     KErrNone,
                                     0 );
                }
            }
            break;

        default:
            {
            //Capability given as a parameter doesn't contain a value
            User::Leave( KErrArgument );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetValueTBoolL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::GetValueTIntL
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::GetValueTIntL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetValueTIntL()" );

    TAccPolGenericID         genericID;
    TAccSrvGenericIDPckg     genericIDPckg( genericID );
    TAccPolNameRecord        nameRecord;
    TAccSrvNameRecordPckg    nameRecordPckg( nameRecord );
    TAccValueTypeTInt        getValue;
    TAccSrvValueTypeTIntPckg getValuePckg( getValue );
    TASYCommandParamRecord   asyCommandParamRecord;
    TInt32                   value( 0 );
    TAccValueTypeTInt        tintValue = { 0 };

    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    aMessage.ReadL( KAccServParamPosThird, getValuePckg );

    //Precondition
    iServerModel->IsConnectedOrDetectedL( genericID );//Leaves with KErrArgument if the accessory is not connected or detected
    
    //Copy client parameters to format in which they are sent to ASY
    asyCommandParamRecord.iNameRecord = nameRecordPckg();
    asyCommandParamRecord.iCmdValue   = getValue.iValue;
    asyCommandParamRecord.iGenericID  = genericID;
    
    
	//get capability
	TUint32 name;
	asyCommandParamRecord.iNameRecord.GetName( name );
	
	CCapValue * capValue = iServerModel->CapabilityStorage().GetCapability( genericID, name );	

	if ( !capValue )
	    {
		User::Leave( KErrArgument );
	    }

    User::LeaveIfError( capValue->ValueInt( value ) );    

    switch ( capValue->Location() )
        {
        case EAPVPolicy:
            {
            iMessage = aMessage;//copy message for later completion

            if ( KErrUnknown == value )
                {
                //Value initialization is needed, forward Get-request to ASY
                iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                ECmdGetValueTInt,
                                                asyCommandParamRecord );
                //Store session to queue so that it will be informed when ASY has
                //served request
                iConnectionController->NotificationQueue().RegisterControlMessage(
                                                EGetValue,
                                                this );
                }
            else
                {
                //Value has been initialized, request can be completed
                tintValue.iValue = value;
                HandleValueMessageL( EGetValue,
                                     tintValue,
                                     KErrNone,
                                     0 );
                }
            }
            break;

        case EAPVAccessory:
            {
            iMessage = aMessage;//copy message for later completion

            if ( KErrUnknown == value )
                {
                //Forward request to ASY Proxy Handler
                iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                ECmdGetValueTInt,
                                                asyCommandParamRecord );
                //Store session to queue so that it will be informed when ASY has
                //served request
                iConnectionController->NotificationQueue().RegisterControlMessage(
                                                EGetValue,
                                                this );
                }
            else
                {
                //Value exist in Accessory Policy DB, request can be completed
                tintValue.iValue = value;
                HandleValueMessageL( EGetValue,
                                     tintValue,
                                     KErrNone,
                                     0 );
                }
            }
            break;

        default:
            {
            //Capability given as a parameter doesn't contain a value
            User::Leave( KErrArgument );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetValueTIntL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::GetValueTDes8L
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::GetValueTDes8L( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetValueTDes8L()" );
   
    TAccSrvGIDNameRecord     GIDNameRecord;
    TAccSrvGIDNameRecordPckg GIDNameRecordPckg( GIDNameRecord );
    TBuf8<KSBMaxNameLength>  valueBuf; //Just For testing
    TASYCommandParamRecord   asyCommandParamRecord;

    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, GIDNameRecordPckg );
    aMessage.ReadL( KAccServParamPosSecond, valueBuf ); //Just For testing    

    //Precondition
    iServerModel->IsConnectedOrDetectedL( GIDNameRecord.iGenericID );//Leaves with KErrArgument if the accessory is not connected or detected
    
    //Copy client parameters to format in which they are sent to ASY
    asyCommandParamRecord.iNameRecord = GIDNameRecord.iNameRecord;
    asyCommandParamRecord.iCmdValue   = 0;
    asyCommandParamRecord.iGenericID  = GIDNameRecord.iGenericID;

	//get capability
	TUint32 name;
	asyCommandParamRecord.iNameRecord.GetName( name);
	
	CCapValue * capValue = iServerModel->CapabilityStorage().GetCapability( 
                                                                GIDNameRecord.iGenericID, 
                                                                name );	
	if ( !capValue )
	    {
		User::Leave( KErrArgument );
	    }

    switch ( capValue->Location() )
        {
        case EAPVPolicy:
            {
            TPtrC8 ptrc8value;
            User::LeaveIfError( capValue->ValueDesc( ptrc8value ) );
                     
            
            //Value is located in Accessory Policy DB, request can be completed
            iMessage = aMessage;
            HandleValueMessageL( EGetValue,
                                 ptrc8value,
                                 KErrNone, //ErrorCode,
                                 0 ); //TrId, not used at the moment
            }
            break;

        case EAPVAccessory:
            {
            //Forward request to ASY Proxy Handler
            iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                ECmdGetValueTDes8,
                                                asyCommandParamRecord );

            //Store session to queue so that it will be informed when ASY has
            //served request
            iConnectionController->NotificationQueue().RegisterControlMessage(
                                                EGetValue,
                                                this );

            //Copy client message for completion after ASY has served request
            iMessage = aMessage;
            }
            break;

        default:
            {
            //Capability given as a parameter doesn't contain a value
            User::Leave( KErrArgument );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetValueTDes8L - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::GetValueObjectBaseL
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::GetValueObjectBaseL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetValueObjectBaseL()" );
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
        
    //Copy client parameters to format in which they are sent to ASY
    asyCommandParamRecord.iNameRecord = nameRecordPckg();
    asyCommandParamRecord.iCmdValue   = 0;
    asyCommandParamRecord.iGenericID  = genericIDPckg();

	//get capability
	TUint32 name;
	asyCommandParamRecord.iNameRecord.GetName( name);
	
	CCapValue * capValue = iServerModel->CapabilityStorage().GetCapability( genericIDPckg(), name );	
                                                                
	if ( !capValue )
	    {
		User::Leave( KErrArgument );
	    }

    switch ( capValue->Location() )
        {      
        case EAPVPolicy:
            {            
            
            TPtrC8 ptrc8value;                        
            User::LeaveIfError( capValue->ValueDesc( ptrc8value ) );
            
            TPtr8 polBufPtr ( const_cast< TUint8* >( ptrc8value.Ptr() ), 
                              ptrc8value.Length(), ptrc8value.Length() );
            
            //Value is located in Accessory Policy DB, request can be completed
            iMessage = aMessage;

            HandleValueMessageL( EGetValue,
                                 0, 
                                 &polBufPtr,
                                 KErrNone ); 
            }
            break;

        case EAPVAccessory:
            {
            //Forward request to ASY Proxy Handler
            iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                ECmdGetObjectValue,
                                                asyCommandParamRecord,
                                                &ptrBuf );
                                                
                                           

            //Store session to queue so that it will be informed when ASY has
            //served request
            iConnectionController->NotificationQueue().RegisterControlMessage(
                                                EGetValue,
                                                this );

            //Copy client message for completion after ASY has served request
            iMessage = aMessage;
            }
            break;

        default:
            {
            //Capability given as a parameter doesn't support other locations
            User::Leave( KErrArgument );
            }
            break;
        }
    
    CleanupStack::PopAndDestroy( buf );
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetValueObjectBaseL - return void" );
    }
// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::GetValueObjectBaseL
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::GetObjectValueL( const RMessage2& aMessage )
    {
     
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetObjectValueL() - Get Object" );
    
    if( iDataBuf )
        {
        // Write data to client.
        aMessage.WriteL( KAccServParamPosFirst, iDataBuf->Ptr(0) );
        //Delete stored data.
        delete iDataBuf;
        iDataBuf = NULL;
        }
    else
        {
        //Write NULL to the buffer
        _LIT( KNull, "" );
        aMessage.WriteL( KAccServParamPosFirst, KNull );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::GetObjectValueL - Get Object - return void" );   
   
    }    
// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::DispatchMessageL
// Handles client request
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::DispatchMessageL()" );

    TInt function = aMessage.Function();

    switch ( function )
        {
        case EAccSrvGetSubblockNameArray:
            {
            GetSubblockNameArrayL( aMessage );
            break;
            }
        case EAccSrvGetValueTBool:
            {
            GetValueTBoolL( aMessage );
            break;
            }
        case EAccSrvGetValueTInt:
            {
            GetValueTIntL( aMessage );
            break;
            }
        case EAccSrvGetValueTDes8:
            {
            GetValueTDes8L( aMessage );
            break;
            }        
        case EAccSrvGetValueObjectBase:
            {
            GetValueObjectBaseL( aMessage );
            break;   
            }        
        case EAccSrvGetObjectValue:
            {
            GetObjectValueL( aMessage );
            break;   
            }        
            
        default:
            {
            TRACE_ASSERT_ALWAYS;
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubConnectionBase::DispatchMessageL - unhandled function %d", function );
            User::Leave( KErrGeneral );
            break;
            }
        } // switch( function )

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::DispatchMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::HandleValueMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::HandleValueMessageL( TMsgID aMsgID,
                                                    TAccValueTypeTBool aCmdResponse,
                                                    TInt aErrorCode,
                                                    TInt /*aTrId*/ )
    {
    COM_TRACE_3( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL(msgID=%d value=%d err=%d) - TBool", aMsgID, aCmdResponse.iValue, aErrorCode );

    TAccPolGenericID        genericID;    
    TAccSrvGenericIDPckg    genericIDPckg( genericID );
    TAccPolNameRecord       nameRecord;
    TAccSrvNameRecordPckg   nameRecordPckg( nameRecord );       

    // Read client parameters
    iMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    iMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );

    switch ( aMsgID )
        {
        case EGetValue:
            {
            //Store value to cap storage
            StoreValueL( genericID, nameRecord, aCmdResponse.iValue );
            
            TPckg<TAccValueTypeTBool> valuePckg( aCmdResponse );
            iMessage.WriteL( KAccServParamPosThird, valuePckg );
            iMessage.Complete( aErrorCode );            
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            User::Leave( KErrGeneral );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL - TBool - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::HandleValueMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::HandleValueMessageL( TMsgID aMsgID,
                                                    TAccValueTypeTInt aCmdResponse,
                                                    TInt aErrorCode,
                                                    TInt /*aTrId*/ )
    {
    COM_TRACE_3( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL(msgID=%d value=%d err=%d) - TInt", aMsgID, aCmdResponse.iValue, aErrorCode );

    TAccPolGenericID        genericID;    
    TAccSrvGenericIDPckg    genericIDPckg( genericID );
    TAccPolNameRecord       nameRecord;
    TAccSrvNameRecordPckg   nameRecordPckg( nameRecord );       

    // Read client parameters
    iMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    iMessage.ReadL( KAccServParamPosSecond, nameRecordPckg );
    
    switch ( aMsgID )
        {
        case EGetValue:
            {
            //Value for this name is stored to Policy DB if it doesn't exist there yet
            StoreValueL( genericID, nameRecord, aCmdResponse.iValue );
            
            TPckgBuf<TAccValueTypeTInt> valueBuf( aCmdResponse );
            iMessage.WriteL( KAccServParamPosThird, valueBuf );
            iMessage.Complete( aErrorCode );
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            User::Leave( KErrGeneral );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL - TInt - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::HandleValueMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::HandleValueMessageL( TMsgID aMsgID,
                                                    TDesC8& aValue,
                                                    TInt aErrorCode,
                                                    TInt /*aTrId*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - TDesC8t" );

    switch ( aMsgID )
        {
        case EGetValue:
            {            
            TInt        writeSize( aValue.Size() );//Size of the data to be written to client side 
            TInt        reservedSize( 0 );//Client has reserved buffer of this size
            TPckg<TInt> reservedSizePckg( reservedSize );
            iMessage.ReadL( KAccServParamPosThird, reservedSizePckg );
            COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - Stage 1" );
            
            if( writeSize > 0 )
                {
                COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - Stage 2" );
                if( writeSize <= reservedSize )
                    {
                    iMessage.WriteL( KAccServParamPosSecond, aValue );
                    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - Stage 2.1" );
                    iMessage.Complete( aErrorCode );
                    }
                else
                    {
                    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - Stage 2.2" );
                    User::Leave( KErrOverflow );
	                }
                }
	          else	              
	              {
	              COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - Stage 3" );
	              if( aErrorCode != KErrNone )
	                  {
	                  COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - Stage 3.1" );
	                  iMessage.Complete( aErrorCode );	                  
	                  }
	              else	
	                  {
	                  COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - Stage 3.2" );
	                  User::Leave( KErrArgument );
	                  }
	              }
            }
            break;

        default:
            {
            COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - Stage 4" );
            TRACE_ASSERT_ALWAYS;
            User::Leave( KErrGeneral );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL - TDesC8 - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::HandleValueMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::HandleValueMessageL( TMsgID aMsgID,
                                                    TInt /*aTrId*/,
                                                    TPtr8* aPtrBuf,
                                                    TInt aErrorCode )
    {
 
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL() - Object" );

    switch ( aMsgID )
        {
        case EGetValue:
            {            
            TInt clientBufSize( iMessage.GetDesLength( KAccServParamPosThird ) );   
            TInt dataBufSize ( aPtrBuf->Size() );           
            
            if( dataBufSize > 0 )
                {
                if( dataBufSize <= clientBufSize )
                    {
                    // Write data to client.
                    iMessage.WriteL( KAccServParamPosThird, *aPtrBuf );

                    //Complete message.
                    iMessage.Complete( aErrorCode );
                    }
                else
                    {
                    
                    
                    CBufFlat*  buf = CBufFlat::NewL( sizeof( KMaxTUint ) ); 
                    CleanupStack::PushL( buf );  
                    buf->ResizeL( sizeof(KMaxTUint) );
            
                    RBufWriteStream wStrm;    
                    (void) wStrm.Open( *buf );
                    CleanupClosePushL( wStrm );    
                    wStrm.WriteInt32L( dataBufSize );//new buffer size
                    wStrm.CommitL();         
                    CleanupStack::PopAndDestroy( &wStrm );
        
                    //write a new buffer size to client
                    TPtr8 ptr( buf->Ptr( 0 ) );
                    iMessage.WriteL( KAccServParamPosThird, ptr );                            
                    iMessage.Complete( KErrOverflow );            
                    
                    CleanupStack::PopAndDestroy( buf );        
                    // Store data to connection base subsession.
                    iDataBuf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                    iDataBuf->ResizeL( dataBufSize );                                        
                    iDataBuf->Write( 0, *aPtrBuf );     
	                }
	              }
	          else
	              {
	              if( aErrorCode != KErrNone )
	                  { 
	                  iMessage.Complete(  aErrorCode );//Complete message.	              
	                  }
	              else
	                  {
	              	  iMessage.Complete(  KErrArgument );//Complete message.
	                  }
	              	
                  }
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            User::Leave( KErrGeneral );
            }
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::HandleValueMessageL - Object - return void" );   
    }


// -----------------------------------------------------------------------------
// CAccSrvSubConnectionBase::StoreValueToPolicyDB
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnectionBase::StoreValueL( 
    const TAccPolGenericID& aGenericID,
    TAccPolNameRecord& aNameRecord,
    TInt aValue )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::StoreValueL()" );
    
    TUint32 name;
    aNameRecord.GetName( name );
    
	CCapValue * capValue = iServerModel->CapabilityStorage().GetCapability( aGenericID, name);

	if ( !capValue )
	    {
		User::Leave( KErrArgument );
	    }
	
	capValue->SetValueL( aValue );    
    
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnectionBase::StoreValueL - return void" );
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
