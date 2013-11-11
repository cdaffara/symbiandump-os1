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
* Description:  Subconnection class for session of ASYComms
*
*/


// INCLUDE FILES
#include "AccSrvSubAsyComms.h"
#include "AccSrvMainSession.h"
#include "AccClientServerMessages.h"
#include "acc_debug.h"
#include "AccSrvServerModel.h"
#include "AccSrvASYProxyHandler.h"
#include <AccessoryTypes.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
//Main service implementation uid of BT-ASY stub for MT
const TUid KBTAsyMainServUidStub = {0x10200c7c};
//Main service implementation uid of BT ASY
const TUid KBTAsyMainServUid = {0x1020897c};

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::CAccSrvSubAsyComms
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubAsyComms::CAccSrvSubAsyComms(
            CASYProxyAPI* aProxy,
            CAccSrvConnectionController* aConnectionController,
            CAccSrvServerModel* aServerModel,
            TUint aASYThreadID,
            TUid aMainServiceUid )
    : CAccSrvSubBase( aProxy, aConnectionController,aServerModel ),
      iOutstandingTrId( 0 ),
      iTimeOutTrId ( 0 ),
      iOutstandingProcessCmdId( ECmdBadFunction ),
      iASYThreadID( aASYThreadID ),
      iMainServiceUid( aMainServiceUid )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::CAccSrvSubAsyComms()" );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::CAccSrvSubAsyComms" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSubAsyComms::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ConstructL()" );

    iInitCommandTimer = CAccSrvProcessCommandTimer::NewL( this, KInitCmdTimeOut );
    iServerModel->AddASYThreadIdL( iASYThreadID );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvSubAsyComms* CAccSrvSubAsyComms::NewL(
                    CASYProxyAPI* aProxy,
                    CAccSrvConnectionController* aConnectionController,
                    CAccSrvServerModel* aServerModel,
                    TUint aASYThreadID,
                    TUid aMainServiceUid )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::NewL()" );
    if ( !aProxy )
        {
        User::Leave( KErrArgument );
        }

    CAccSrvSubAsyComms* self = new ( ELeave ) CAccSrvSubAsyComms(
                                                aProxy,
                                                aConnectionController,
                                                aServerModel,
                                                aASYThreadID,
                                                aMainServiceUid );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubAsyComms::NewL - return 0x%x", self );
    return self;
    }

// Destructor
CAccSrvSubAsyComms::~CAccSrvSubAsyComms()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::~CAccSrvSubAsyComms()" );

    if ( iInitCommandTimer )
        {
        delete iInitCommandTimer;
        }

    if ( iProcessCommandTimer )
        {
        delete iProcessCommandTimer;
        }
        
    if( iObjectBuf )
        {
        delete iObjectBuf;
        }

    CancelNotifyProcessCommand( iMessage );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::~CAccSrvSubAsyComms" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::DispatchMessageL
// Handles client request
// -----------------------------------------------------------------------------
//
void CAccSrvSubAsyComms::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::DispatchMessageL()" );

    TInt function = aMessage.Function();

    switch ( function )
        {
        case EAccSrvNotifyProcessCommand:
            {
            NotifyProcessCommand( aMessage );
            break;
            }
        case EAccSrvCancelNotifyProcessCommand:
            {
            CancelNotifyProcessCommand( aMessage );
            break;
            }
        case EAccSrvProcessResponseTBool:
            {
            ProcessResponseL( aMessage, EProcessResponseTBool );
            break;
            }
        case EAccSrvProcessResponseTInt:
            {
            ProcessResponseL( aMessage, EProcessResponseTInt );
            break;
            }
        case EAccSrvProcessResponseTDes:
            {
            ProcessResponseL( aMessage, EProcessResponseTDes );
            break;
            }        
        case EAccSrvProcessResponseObject:
            {
            ProcessResponseL( aMessage, EProcessResponseObject );
            break;
            }
         
        case EAccSrvGetObjectBuffer:
            {
            TPtr8 ptrBuf( iObjectBuf->Ptr( 0 ) );
            aMessage.WriteL( KAccServParamPosFirst, ptrBuf );
            delete iObjectBuf;
            iObjectBuf = NULL;
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS;
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubAsyComms::DispatchMessageL - unhandled function %d", function );
            User::Leave( KErrNotSupported );
            break;
            }
        } // switch( function )

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::DispatchMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::NotifyProcessCommand
// Sets the NotifyProcessCommand to a handling queue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSubAsyComms::NotifyProcessCommand( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::NotifyProcessCommand()" );

    // Add session to queue.
    // Queue will call CAccSrvSubAsyComms::CompleteProcessCommandL() when client
    // has sent request to ASY.

    // Exception: In initialization case asy-proxy-server makes
    //  the completition instead of client ( Loading complete )

    iConnectionController->NotificationQueue().RegisterControlMessage(
                                                    ENotifyProcessCommand,
                                                    this );

    // Copy client message for completion when client has sent request to ASY.
    // Completion is done in CAccSrvSubAsyComms::CompleteProcessCommandL()

    // Exception: In initialization case asy-proxy-server makes
    //  the completition instead of client ( Loading complete )

    iMessage = aMessage;

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::NotifyProcessCommand - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::CancelNotifyProcessCommand
// -----------------------------------------------------------------------------
//
void CAccSrvSubAsyComms::CancelNotifyProcessCommand( const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::CancelNotifyProcessCommand()" );

    // Is command pending
    if ( !iMessage.IsNull() )
        {
        // Remove NotifyProcessCommand from queue.
        iConnectionController->NotificationQueue().CancelControlMessage(
                                                        ENotifyProcessCommand,
                                                        this );
        iMessage.Complete( KErrCancel );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::CancelNotifyProcessCommand - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::ProcessResponseL
// Handles responses of NotifyProcessCommand
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSubAsyComms::ProcessResponseL( const RMessage2& aMessage,
                                           TProcessResponseType aResponseType,
                                           TBool aTimeOut )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ProcessResponseL()" );

    if ( iProcessCommandTimer )
        {
        iProcessCommandTimer->Cancel();
        delete iProcessCommandTimer;
        iProcessCommandTimer = NULL;
        }

    TMsgID                       messageId( EUnknown );
    TPckgBuf<TInt>               tridPckgBuf;
    TPckgBuf<TInt>               errPckgBuf;    
    TAccSrvValueTypeTBoolPckgBuf valueTBoolPckgBuf;// TBool    
    TPckgBuf<TAccValueTypeTInt>  valueTIntPckgBuf;// TInt    
    TASYBTAccInfo                btInfo;// TDes
    TPckg<TASYBTAccInfo>         valueBTPckg( btInfo );
    TInt asyDataSize ( 0 );
    HBufC8 *asyData = NULL;    
    CBufFlat* objectBuf = NULL;    
    
        		    
    if ( !aTimeOut )
        {
       	//
	    	// Allocated the buffer which is really needed!
    		//     
  	  	if ( EProcessResponseTDes == aResponseType && 
        	ECmdGetSupportedBTProfiles != iOutstandingProcessCmdId )
	        {
					COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ProcessResponseL() - Before RMessage with EProcessResponse" );
	    		asyDataSize = aMessage.GetDesLengthL( KAccServParamPosSecond );
	        asyData = HBufC8::NewL( asyDataSize );
	        CleanupStack::PushL( asyData );        
					COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ProcessResponseL() - After RMessage with EProcessResponse" );
	        }		
		  	else if ( EProcessResponseObject == aResponseType )
					{
					COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ProcessResponseL() - Before RMessage with EProcessResponseObject" );
					TInt bufLength = ( aMessage.GetDesLengthL( KAccServParamPosSecond ) );
			    objectBuf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
				  CleanupStack::PushL( objectBuf );
				  objectBuf->ResizeL( bufLength );
					COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ProcessResponseL() - After RMessage with EProcessResponseObject" );		
					}
				else
				  {
				  COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ProcessResponseL() - Buffer is not needed!" );
				  }
			
        	        	
        aMessage.ReadL( KAccServParamPosFirst, tridPckgBuf );
        if ( iOutstandingTrId != tridPckgBuf() )
            {
            // There are no correcponding SetValueL()/GetValueL() pending
            User::Leave( KErrNotFound );
            }

        switch ( aResponseType )
            {
            case EProcessResponseTBool :
                aMessage.ReadL( KAccServParamPosSecond, valueTBoolPckgBuf );
                aMessage.ReadL( KAccServParamPosThird, errPckgBuf );
                break;
            case EProcessResponseTInt :
                aMessage.ReadL( KAccServParamPosSecond, valueTIntPckgBuf );
                aMessage.ReadL( KAccServParamPosThird, errPckgBuf );
                break;
            case EProcessResponseTDes :
                if( ECmdGetSupportedBTProfiles == iOutstandingProcessCmdId )
                    {
                    aMessage.ReadL( KAccServParamPosSecond, valueBTPckg ); 
                    }
                else
                    {             
                    TPtr8 asyDataPtr = asyData->Des();	        
                    aMessage.ReadL( KAccServParamPosSecond, asyDataPtr );       
                    }
                aMessage.ReadL( KAccServParamPosThird, errPckgBuf );         
                break;
            case EProcessResponseObject :   
                {  
                // In set case do not read object buf ptr.	                      
                if( ECmdGetObjectValue == iOutstandingProcessCmdId ) 	
                   {
                   TPtr8 objectBufPtr( objectBuf->Ptr(0) );			    		
                   aMessage.ReadL( KAccServParamPosSecond, objectBufPtr );
                   aMessage.ReadL( KAccServParamPosThird, errPckgBuf );
                   }
                else
                   {                   	
                   aMessage.ReadL( KAccServParamPosThird, errPckgBuf );
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
        }
    else // Timed out
        {
        tridPckgBuf = iOutstandingTrId;
        errPckgBuf = KErrTimedOut;
        iTimeOutTrId = iOutstandingTrId;
        }
        
    // This ASY command is handled so no more new requests can not be linked to this one
    // ->Transaction filter can be removed
    iConnectionController->ASYProxyHandler().RemoveTransactionID( iOutstandingTrId );
        
    switch ( iOutstandingProcessCmdId )
        {
        case ECmdSetValueBool:
        case ECmdSetValueTInt:
        case ECmdAccessoryUpdated:
        case ECmdSetObjectValue:
            {
            messageId = ESetValue;
            }
            break;
        case ECmdGetValueBool:
        case ECmdGetValueTInt:
        case ECmdGetValueTDes8:
        case ECmdGetSupportedBTProfiles:
        case ECmdGetObjectValue:
            {
            messageId = EGetValue;
            }
            break;
        case ECmdProcessCommandInit:
            {
            if ( iInitCommandTimer->IsActive() )
                {
                iInitCommandTimer->Cancel();
                CompleteProcessCommandInitL();
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

    if ( iOutstandingProcessCmdId != ECmdProcessCommandInit )
        {
        switch ( aResponseType )
            {
            case EProcessResponseTBool :
                iConnectionController->NotificationQueue().CompleteValueMessageL(
                                                   messageId,
                                                   valueTBoolPckgBuf(),
                                                   errPckgBuf(),
                                                   iOutstandingTrId );
                break;
            case EProcessResponseTInt :
                iConnectionController->NotificationQueue().CompleteValueMessageL(
                                                   messageId,
                                                   valueTIntPckgBuf(),
                                                   errPckgBuf(),
                                                   iOutstandingTrId );
                break;
            case EProcessResponseTDes :
                if( ECmdGetSupportedBTProfiles == iOutstandingProcessCmdId )
                    {
                    iConnectionController->NotificationQueue().CompleteValueMessageL(
                                                   messageId,
                                                   valueBTPckg,
                                                   errPckgBuf(),
                                                   iOutstandingTrId );
                    }
                else
                    {
                    TPtr8 asyDataPtr ( NULL, 0, 0 );                    
                    if ( NULL != asyData )
                    	{
                    	asyDataPtr.Set ( asyData->Des() );
                    	}
                    iConnectionController->NotificationQueue().CompleteValueMessageL(
                                                   messageId,
                                                   asyDataPtr,
                                                   errPckgBuf(),
                                                   iOutstandingTrId );
                    
                    if ( NULL != asyData )
                    	{                            
                    	CleanupStack::PopAndDestroy( asyData );                               
                    	}	
                    }
                break;
            case EProcessResponseObject :
                {                
                TPtr8 objectBufPtr ( NULL, 0, 0 ); 
                
                if(	NULL != objectBuf )
                	{
                	objectBufPtr.Set ( objectBuf->Ptr(0) );
                	}	      
                	          
	                iConnectionController->NotificationQueue().CompleteValueMessageL( 
                                   messageId,
                                   iOutstandingTrId,
                                   &objectBufPtr,
                                   errPckgBuf() );                	                	

                if( NULL != objectBuf )
                	{                                   
                	CleanupStack::PopAndDestroy( objectBuf );
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

        }

    iOutstandingProcessCmdId = ECmdBadFunction;
    iOutstandingTrId = 0;
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ProcessResponseL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::CompleteMessageL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubAsyComms::CompleteMessageL( TMsgID /*aMsgID*/,
                                            TInt /*aErrorCode*/,
                                            TInt /*aUniqueID*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::CompleteMessageL()" );

    TRACE_ASSERT_ALWAYS;

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::CompleteMessageL - return False" );

    return EFalse; //message was not consumed
    }
// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::CompleteProcessCommandL
// Completes queued message
// Preconditions:
// 1. ECmdProcessCommandInit command is allways handled
// 2. Request must be meant for this subsession
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubAsyComms::CompleteProcessCommandL(
    TProcessCmdId aCmdn,
    TInt aTrId,
    TASYCommandParamRecord& aASYCommandParamRecord,
    TPtr8* aPtrBuf )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::CompleteProcessCommandL()" );

    TBool messageConsumption( EFalse );// Return value tells should this subsession be removed from the queue

    if (  ECmdProcessCommandInit == aCmdn )
        {
        messageConsumption = HandleCommandL( aCmdn, aTrId, aASYCommandParamRecord, aPtrBuf );
        }
    else
        {
        if ( iServerModel->IsMyRequest( 
                iASYThreadID, aASYCommandParamRecord.iGenericID.UniqueID() ) )
            {
            messageConsumption = HandleCommandL( aCmdn, aTrId, aASYCommandParamRecord, aPtrBuf );
            }
        else
            {
            //Not for us
            //Special handling for BT-ASY
            if ( ( KBTAsyMainServUidStub.iUid == iMainServiceUid.iUid ) || 
                 ( KBTAsyMainServUid.iUid == iMainServiceUid.iUid ) )
                {
                if ( aASYCommandParamRecord.iGenericID.PhysicalConnectionCaps( KPCBluetooth ) )//BT
                    {
                    messageConsumption = HandleCommandL( aCmdn, aTrId, aASYCommandParamRecord, aPtrBuf );
                    }
                }
            else
                {
                //This subsession is not serving BT-ASY
                }
            }
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubAsyComms::CompleteProcessCommandL - return %d", messageConsumption );
    return messageConsumption;
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::HandleCommandL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CAccSrvSubAsyComms::HandleCommandL( TProcessCmdId aCmdn,
                                          TInt aTrId,
                                          TASYCommandParamRecord& aASYCommandParamRecord,
                                          TPtr8* aPtrBuf )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::HandleCommandL()" );
    TInt err( KErrNone );

    iOutstandingProcessCmdId = aCmdn;
    iOutstandingTrId = aTrId;

    TProxyCommandPckgBuf params;
    params().iCmdId        = aCmdn;
    params().iTrId         = aTrId;
    params().iTimeOutTrId  = iTimeOutTrId;
    params().iAsyCmdParams = aASYCommandParamRecord;
    iMessage.WriteL( KAccServParamPosFirst, params ); 


    if( ECmdProcessCommandInit !=  aCmdn )
        {    
        err = HandleObjectBufferWriteL( aPtrBuf );
        iProcessCommandTimer = CAccSrvProcessCommandTimer::NewL( this );
        iProcessCommandTimer->StartTimer();
        }
    else
        {
        iInitCommandTimer->StartTimer();
        }

    iMessage.Complete( err );

    iTimeOutTrId = 0;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::HandleCommandL - return ETrue" );
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::HandleObjectBufferWriteL()
// -----------------------------------------------------------------------------
TInt CAccSrvSubAsyComms::HandleObjectBufferWriteL( TPtr8* aPtrBuf )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubAsyComms::HandleObjectBufferWriteL(0x%x)", aPtrBuf );
    TInt err( KErrNone );
    
    if( !aPtrBuf )
        {
        // There are no buffer to handle
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubAsyComms::HandleObjectBufferWriteL - no buffer, return %d", err );
        return err;
        }
    
    TInt size=iMessage.GetDesMaxLength( KAccServParamPosSecond );    
    
    if( aPtrBuf->Size() <=  size )
        {
        iMessage.WriteL( KAccServParamPosSecond, *aPtrBuf );
        }
    else
        {
        //Return a new buffer size to client
        err = KErrOverflow;
        CBufFlat*  buf = CBufFlat::NewL( sizeof( KMaxTUint ) ); 
        CleanupStack::PushL( buf );  
        buf->ResizeL( sizeof(KMaxTUint) );
            
        RBufWriteStream wStrm;    
        (void) wStrm.Open( *buf );
        CleanupClosePushL( wStrm );    
        wStrm.WriteInt32L( aPtrBuf->Size() );//new buffer size
        wStrm.CommitL();         
        CleanupStack::PopAndDestroy( &wStrm );
         
        TPtr8 ptr( buf->Ptr( 0 ) );
        iMessage.WriteL( KAccServParamPosSecond, ptr );        
        CleanupStack::PopAndDestroy( buf );           
         
        //Store current buffer for later retrival        
        iObjectBuf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
        iObjectBuf->ResizeL( aPtrBuf->Size() );
        iObjectBuf->Write( 0, *aPtrBuf );     
        
        }
        
        
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubAsyComms::HandleObjectBufferWriteL - return %d", err );
    return err;
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::CompleteProcessCommandInitL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAccSrvSubAsyComms::CompleteProcessCommandInitL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::CompleteProcessCommandInitL()" );

    // Note, Must NOT leave before next call
    iServerModel->SetASYsLoadCompleted();

    iConnectionController->InitializeConnectionStatusL();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::CompleteProcessCommandInitL" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::ProcessCommandTimeOutCompleteL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAccSrvSubAsyComms::ProcessCommandTimeOutCompleteL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ProcessCommandTimeOutCompleteL()" );

    switch( iOutstandingProcessCmdId )
        {
        case ECmdSetValueBool:
        case ECmdGetValueBool:
            ProcessResponseL( iMessage, EProcessResponseTBool, ETrue );
            break;
        case ECmdSetValueTInt:
        case ECmdGetValueTInt:
            ProcessResponseL( iMessage, EProcessResponseTInt, ETrue );
            break;
        case ECmdGetValueTDes8:
        case ECmdGetSupportedBTProfiles:
            ProcessResponseL( iMessage, EProcessResponseTDes, ETrue );
            break;
        case ECmdSetObjectValue:
        case ECmdGetObjectValue:
            ProcessResponseL( iMessage, EProcessResponseObject, ETrue );
            break;    
        default:
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAsyComms::ProcessCommandTimeOutCompleteL" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
