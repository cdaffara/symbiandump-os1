// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmmbmscontexttsyTraces.h"
#endif

#include "cmmmbmscontexttsy.h"
#include "cmmmbmscontextlist.h"
#include "CMmPacketTsy.h"
#include "mbmstypes.h"
#include "MmTsy_timeoutdefs.h"
#include "MmTsy_numberOfSlots.h"

// ============================ MEMBER FUNCTIONS ===============================

CMmMBMSContextTsy::CMmMBMSContextTsy()
    {        
    }

CMmMBMSContextTsy* CMmMBMSContextTsy::NewL(
        CMmPacketServiceTsy* aMmPacketService,                
        const TDes& aName,  
        const TUint8 aProxyId  ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_NEWL_1,  "TSY: CMmMBMSContextTsy::NewL" );

    CMmMBMSContextTsy* packetContext = new ( ELeave ) CMmMBMSContextTsy();
    CleanupClosePushL( *packetContext );    
    packetContext->ConstructL( aMmPacketService, aName, aProxyId );
    CleanupStack::Pop();
    
    return packetContext;
    }

void CMmMBMSContextTsy::ConstructL(CMmPacketServiceTsy* const aMmPacketService,                
							        const TDes& aName,  
							        TUint8 aProxyId)
	{
    iMmPacketService = aMmPacketService;
    iContextName = aName;
    iObjectId = aProxyId;
	
    iConfig = new ( ELeave ) RPacketMbmsContext::TContextConfigMbmsV1();

#ifdef REQHANDLE_TIMER   
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL(  this,
        iMmPacketService->PhoneTsy(), EMaxNumOfMBMSContextRequests,
        iPacketContextReqHandles );

#else
        // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        EMaxNumOfMBMSContextRequests, iPacketContextReqHandles );

#endif // REQHANDLE_TIMER
    iIsInitialiseAllowed = ETrue;
    iServicesArray = RPacketMbmsContext::CMbmsSession::NewL(); 
    iContextStatus = RPacketContext::EStatusUnknown;  
    }
    
CMmMBMSContextTsy::~CMmMBMSContextTsy()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_DTOR_1,  "TSY: CMmMBMSContextTsy::~CMmMBMSContextTsy Context name: %S", iContextName );
	
	if( iMmPacketService )
		{			
		iMmPacketService->MBMSContextList()->RemoveObject( this );

	    // Set Dynamic Caps
	    iMmPacketService->ActivePacketServiceExtension()->SetDynamicCapsFlag(
	        RPacketService::KCapsActivate, EFalse );
		}

    delete iServicesArray;
    iServicesArray = NULL;

    delete iTsyReqHandleStore;
    iTsyReqHandleStore = NULL;
    
    delete iConfig;
    iConfig = NULL;             

    delete iTempConfig;
    iTempConfig = NULL;
    
    }


// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::ExtFunc
// TRAP's all CMmMBMSContextTsy related MM Packet API requests in case that
// they fail. This method functions only as a centralized TRAP for the
// DoExtFuncL method that does the actual mapping of IPC number to TSY method
// call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//

TInt CMmMBMSContextTsy::ExtFunc( 
        TTsyReqHandle aTsyReqHandle, 
        TInt aIpc, 
        const TDataPackage& aPackage )  
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_EXTFUNC_1,  "TSY: CMmMBMSContextTsy::ExtFunc. IPC: %{TIPCNamesList} Context name:%S", (TUint)aIpc, iContextName );

    TInt ret( KErrNone );
    TInt trapError( KErrNone );

    // Reset last tsy request type
    iReqHandleType = EMultimodePacketMbmsReqHandleUnknown;
    
    // Set tsy request handle
    iTsyReqHandle = aTsyReqHandle;    

    // Trap the call of DoExtFuncL
    TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ) );

    if ( KErrNone != trapError )
        {
		// Reset request handle to indicate the request is no longer ongoing
        iTsyReqHandleStore->FindAndResetTsyReqHandle(aTsyReqHandle);
        ReqCompleted( aTsyReqHandle, trapError );
        }

    else if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else if ( EMultimodePacketMbmsReqHandleUnknown != iReqHandleType )
        {
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, aTsyReqHandle );
#endif // REQHANDLE_TIMER
        }
    iReqHandleType = EMultimodePacketMbmsReqHandleUnknown;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::DoExtFuncL
// Packet Data specific functionality of CMmMBMSContextTsy::ExtFunc
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::DoExtFuncL( 
        TTsyReqHandle aTsyReqHandle,      
        TInt aIpc,      
        const TDataPackage& aPackage )   
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_DOEXTFUNCL_1,  "TSY: CMmMBMSContextTsy::DoExtFuncL. IPC: %{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, (TInt)aTsyReqHandle, (TUint)this );

    TInt ret( KErrNotSupported );

    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();

    switch ( aIpc )
        {
        case EPacketContextNotifyStatusChange:
            ret = NotifyStatusChange( 
                reinterpret_cast< RPacketContext::TContextStatus*>( dataPtr ) );
            break;
        case EPacketContextNotifyConfigChanged:
            ret = NotifyConfigChanged( 
                reinterpret_cast< TPacketDataConfigBase*>( dataPtr ) );
            break;
        case EPacketContextInitialiseContext:
            ret = InitialiseContextL( reinterpret_cast< RPacketContext::TDataChannelV2*>( dataPtr ));
            break;
        case EPacketContextSetConfig:
            ret = SetConfigL( 
                reinterpret_cast< TPacketDataConfigBase*>( dataPtr ) );
            break;
        case EPacketContextActivate:
            ret = ActivateL();
            break;
        case EPacketContextDeactivate:
            ret = DeactivateL();
            break;
        case EPacketContextDelete:
            ret = DeleteL();
            break;
        case EPacketContextGetStatus:
            ret = GetStatus( aTsyReqHandle, 
                reinterpret_cast< RPacketContext::TContextStatus*>( dataPtr ) );
            break;
        case EPacketContextGetConfig:
            ret = GetConfig( aTsyReqHandle, 
                reinterpret_cast< TPacketDataConfigBase*>( dataPtr ) );
            break;
        case EPacketContextGetLastErrorCause:
            ret = GetLastErrorCause( aTsyReqHandle, 
                reinterpret_cast< TInt*>( dataPtr ) );
            break;
		case EPacketContextUpdateMbmsSessionList:
            ret = UpdateMbmsSessionList( 
             reinterpret_cast< TMbmsAction*>( dataPtr ),        
             reinterpret_cast< TMbmsSessionId*>( dataPtr2 ) );
            break;
        case EPacketGetMbmsSessionListPhase1:
            ret = GetMbmsActiveServicesPhase1L( aTsyReqHandle,
                reinterpret_cast< RMobilePhone::TClientId*>( dataPtr ),
                reinterpret_cast< TInt*>( dataPtr2 ) );
            break;
            
        case EPacketGetMbmsSessionListPhase2:
            ret = GetMbmsActiveServicesPhase2L( aTsyReqHandle,
                reinterpret_cast< RMobilePhone::TClientId*>( dataPtr ),
                aPackage.Des2n()  );
            break;            
        // These requests currently not supported
        case EPacketContextLoanCommPort:
        case EPacketContextRecoverCommPort:
        case EPacketContextNotifyDataTransferred:
        case EPacketContextGetProfileName:
        case EPacketContextGetConnectionSpeed:
        case EPacketContextGetDataVolumeTransferred:
        case EPacketContextEnumeratePacketFilters:
        case EPacketContextGetPacketFilterInfo:
        case EPacketContextAddPacketFilter:
        case EPacketContextRemovePacketFilter:
        case EPacketContextModifyActiveContext:
        case EPacketContextGetDNSInfo:
        case EPacketAddMediaAuthorization:
        case EPacketRemoveMediaAuthorization:
        default:
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_DOEXTFUNCL_2, "TSY: CMmMBMSContextTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }



// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::ReqModeL
// Returns request mode for given IPC.
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmMBMSContextTsy::ReqModeL( 
        TInt aIpc ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_REQMODEL_1,  "TSY: CMmMBMSContextTsy::ReqModeL. IPC: %{TIPCNamesList}", aIpc );

    CTelObject::TReqMode reqMode( 0 );
    TBool doLeave( EFalse );

    switch ( aIpc )
        {
        // All disabled
        // TSYs wishing to implement their own buffering algorithm will place 
        // all requests in this category.  

        // Get methods that do not use DOS and return immediately. 
        // Flow control not required.
        case EPacketContextGetLastErrorCause:

        // Other methods that do not use DOS and return immediately. 
        // Flow control not required.
    
        // Methods that can propably take a long time and cannot therefore be
        // flow controlled. Solution: All these methods must check req handle
        // table before handling the request. In case that the request table 
        // indicates that same method has been called and has not been 
        // completed, the method should return KErrServerBusy.

        case EPacketContextGetStatus:
        case EPacketContextGetConfig:
            break;

        // KReqModeFlowControlObeyed
        // Commands that change the state of the phone, e.g. clearing the AoC 
        // counter, are commands that the TSY should only deal with one at 
        // a time.
        case EPacketContextUpdateMbmsSessionList:
        case EPacketContextInitialiseContext:
        case EPacketContextActivate:
        case EPacketContextDelete:
        case EPacketContextSetConfig:
        case EPacketContextAddPacketFilter:
        case EPacketContextRemovePacketFilter:
        
        case EPacketContextDeactivate:
            {
            reqMode = KReqModeFlowControlObeyed;
            break;
            }

        // KReqModeMultipleCompletionEnabled | ReqModePostImmediately
        // Requests that notify a client about a change of state.
        // Since these requests do not require the issuing of any modem 
        // commands, they do not have to obey flow control.
        // The TSY never gets more than one of these outstanding.
        case EPacketContextNotifyConfigChanged:

        case EPacketContextNotifyStatusChange:
            reqMode = KReqModeMultipleCompletionEnabled |
                KReqModeRePostImmediately;
            break;

        case EPacketGetMbmsSessionListPhase1:
        case EPacketGetMbmsSessionListPhase2:
            reqMode = 0;
            break;
        // Cancel Requests
        // It's not necessary to include the Cancel methods in ReqModeL at all.
        // The ETel server never calls ReqModeL with a Cancel IPC.

        // Other variations of return values are unusable.
        // Following requests are not supported
        //case EPacketContextNotifyDataTransferred:
        //case EPacketContextLoanCommPort:
        //case EPacketContextRecoverCommPort:
        //case EPacketContextModifyActiveContext:
        // Default
        default:
            doLeave = ETrue;
            break;
        }

    if ( doLeave )
        {
        User::Leave( KErrNotSupported );
        }

    return reqMode;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::NumberOfSlotsL(    
        TInt aIpc )                            
    {
    TInt numberOfSlots( 0 );

    switch ( aIpc )
        {
        case EPacketContextNotifyStatusChange:
            numberOfSlots = KMmPacketContextNotifyStatusChangeSlots;
            break;
        case EPacketContextNotifyConfigChanged:
            numberOfSlots = KMmPacketContextNotifyConfigChangedSlots;
            break;

        default:
            // Unknown or invalid IPC
            User::Leave( KErrNotSupported );
            break;

        }  
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_NUMBEROFSLOTSL_1,  "TSY: CMmMBMSContextTsy::NumberOfSlotsL. IPC: %{TIPCNamesList} Number of slots: %d", aIpc, numberOfSlots );

    return numberOfSlots;

    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CancelService
// Cancel the outstanding request.ETel Server when it is "cleaning-up" any
//  outstanding asynchronous requests before closing a client's sub-session.
//  This will happen if a client closes its R-class handle without cancelling
//  outstanding asynchronous requests. Only Packet API requests are handled 
//  here. Client can also cancel a request by calling CancelAsyncRequest 
//  (defined in etel.h). The ETel server passes this kind of cancel requests
//  to CancelService.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::CancelService(    
        TInt aIpc,                            
        TTsyReqHandle aTsyReqHandle )         
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_CANCELSERVICE_1,  "TSY: CMmMBMSContextTsy::CancelService. IPC: %{TIPCNamesList} Handle:%d", aIpc, aTsyReqHandle );

    TInt ret( KErrNone );
	//TTsyReqHandle reqHandle( NULL );
    // Reset last tsy request type
    iReqHandleType = EMultimodePacketMbmsReqHandleUnknown;

    switch ( aIpc )
        {
        // TSY has started a request and it is not possible to then cancel 
        // this request. The best thing for the TSY to do in this case is to
        // proceed as though the Cancel never happened. The server's call to
        // the TSY cancel function will return synchronously. The TSY then
        // continues to wait for the original acknowledgement and when it
        // receives it, the TSY will complete the original request.
        case EPacketContextUpdateMbmsSessionList:
        case EPacketContextInitialiseContext:
        case EPacketContextActivate:
        case EPacketContextDeactivate:
        case EPacketContextDelete:
        case EPacketContextSetConfig:
        case EPacketContextGetConfig:
            break;
        case EPacketGetMbmsSessionListPhase1:
            ret = CancelGetMbmsActiveServicesPhase1();
            break;
        // Notification Cancels, no special requirements.
        case EPacketContextNotifyStatusChange:
            iRetNotifyStatus = NULL;
            iReqHandleType = EMultimodeMbmsContextNotifyStatusChange;
            break;
        case EPacketContextNotifyConfigChanged:
            iRetNotifyConfig = NULL;
            iReqHandleType = EMultimodeMbmsContextNotifyConfigChanged;
            break;

        // IPCs that are not supported
        //case EPacketContextLoanCommPort:
        //case EPacketContextRecoverCommPort:
        //case EPacketContextNotifyDataTransferred:
        //case EPacketContextModifyActiveContext:
        // Default
        default:
            ret = KErrNotSupported;
            break;
        }

    if ( EMultimodePacketMbmsReqHandleUnknown != iReqHandleType )
        {
        // Reset tsy request handle
        iTsyReqHandleStore->ResetTsyReqHandle( iReqHandleType );

        // Complete request
        CMmMBMSContextTsy::ReqCompleted( aTsyReqHandle, KErrCancel );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::RegisterNotification
// Register notification. RegisterNotification is called when the ETel Server
// recognises that this notification is being posted for the first time on
// this subsession object.It enables the TSY to "turn on" any regular 
// notification messages that it may receive from DOS. If this object is
// Dial-Up context only supported notification is 
// EPacketContextNotifyStatusChange.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::RegisterNotification( 
        TInt aIpc )              
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_REGISTERNOTIFICATION_1,  "TSY: CMmMBMSContextTsy::RegisterNotification. IPC: %{TIPCNamesList}", aIpc );

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EPacketContextNotifyConfigChanged:
        case EPacketContextNotifyStatusChange:
            break;
        //case EPacketContextNotifyDataTransferred:
        default:
            // Not supported
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::DeregisterNotification
// Deregister notification. DeregisterNotification is called when the ETel
// Server recognises that this notification will not be posted again because 
// the last client to have a handle on this sub-session object has just closed 
// the handle. It enables the TSY to "turn off" any regular notification
// messages that it may receive from DOS. If this object is Dial-Up context
// only supported notification is EPacketContextNotifyStatusChange.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::DeregisterNotification(
        TInt aIpc )                          
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_DEREGISTERNOTIFICATION_1,  "TSY: CMmMBMSContextTsy::DeregisterNotification. IPC: %{TIPCNamesList}", aIpc );

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EPacketContextNotifyConfigChanged:
        case EPacketContextNotifyStatusChange:
            break;
        //case EPacketContextNotifyDataTransferred:
        default:
            // Not supported
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::InitialiseContextL
// This function initialises the context created previously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::InitialiseContextL( RPacketContext::TDataChannelV2* aDataChannel )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_INITIALISECONTEXTL_1,  "TSY: CMmMBMSContextTsy::InitialiseContextL. %S", iContextName );

    TInt ret( KErrArgument ); 
   
    if ( iIsInitialiseAllowed )
       {
           CMmDataPackage dataPackage;
           dataPackage.PackData( &iContextName);
           iDataChannelV2 = aDataChannel;
        
           ret = iMmPacketService->MessageManager()->HandleRequestL(
        		  ECtsyPacketMbmsInitialiseContextReq, &dataPackage );
                    
           if( KErrNone == ret )
        	  {
        	  iIsInitialiseAllowed = EFalse;
        	  }	        
         }

        if ( KErrNone == ret )
          {
          // Store the request handle
          iReqHandleType = EMultimodePacketMbmsInitialiseContext;
          }
       
     
 
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CompleteInitialiseContext
// Complete the asynchronous InitialiseContext request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//

void CMmMBMSContextTsy::CompleteInitialiseContext(TInt aResult, RPacketContext::TDataChannelV2* aDataChannel )     
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETEINITIALISECONTEXT_1,  "TSY: CMmMBMSContextTsy::CompleteInitialiseContext. Error: %{TSymbianErrorCodes}", aResult );

    iIsActivateAllowed = ETrue;
    if (KErrNone == aResult )
        {
        CompleteNotifyStatusChange( RPacketContext::EStatusInactive );
        if((aDataChannel != NULL) && (iDataChannelV2 != NULL))
        	{
        	*iDataChannelV2 = *aDataChannel;
        	iDataChannelV2 = NULL;
        	}
        // Set Dynamic Caps
        iMmPacketService->ActivePacketServiceExtension()->SetDynamicCapsFlag(
        		RPacketService::KCapsActivate, iIsActivateAllowed );
        }
    else
        {
        iIsInitialiseAllowed = ETrue;
        }        

    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketContextInitialiseContext );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        ReqCompleted( reqHandle, aResult );
        }
    }
       
// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::ActivateL
// Activate a context. This function activates a context that has been
// previously configured using the SetConfig() method from RPacketMbmsContext.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::ActivateL()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_ACTIVATEL_1,  "TSY: CMmMBMSContextTsy::ActivateL. Context name:%S", iContextName );
	
    TInt ret( KErrNotReady );

    if ( iIsActivateAllowed )
        {        
        // verify that max activate context is not exceed
        if( iMmPacketService->MaximumActiveServices() >
            iMmPacketService->EnumerateMbmsActiveServices() )
            {            
            // mbmscontext activation        
            CMmDataPackage data;
            data.PackData( &iContextName );
            
            ret = iMmPacketService->MessageManager()->HandleRequestL(
                    EPacketContextActivate, &data );            
            } 
        else
            {
            // max number of possible active context exceed
            ret = KErrUmtsMaxNumOfContextExceededByPhone;
            }   
        if ( KErrNone == ret )
            {
            iIsActivateAllowed = EFalse;
            CompleteNotifyStatusChange( RPacketContext::EStatusActivating ); 
            }                    
        }

    if ( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = EMultimodeMbmsContextActivate;
        }

    return ret;        
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CompleteActivate
// Complete the asynchronous Activate request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMBMSContextTsy::CompleteActivate(TInt aResult)
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETEACTIVATE_1,  "TSY: CMmMBMSContextTsy::CompleteActivate. Error: %{TSymbianErrorCodes}", aResult );
   
    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeMbmsContextActivate );
        
    if ( KErrNone == aResult )
        {
        
        CompleteNotifyConfigChanged();

        // Notify status change
        CompleteNotifyStatusChange( RPacketContext::EStatusActive );
        }        
    else
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETEACTIVATE_2,  "TSY: CMmMBMSContextTsy::CompleteActivate. ErrorCause: %{TSymbianErrorCodes}",aResult );
        // Re-activation is allowed because activation has not succeeded
        iIsActivateAllowed = ETrue;
        }

             
    if ( EMultimodePacketMbmsReqHandleUnknown != reqHandle )
        {
      
        // Complete the client request
        CMmMBMSContextTsy::ReqCompleted( reqHandle, aResult );
        }
    }
    
    
// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::Deactivate
// Deactivate a context previously configured on the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::DeactivateL()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_DEACTIVATEL_1,  "TSY: CMmMBMSContextTsy::DeactivateL. Context name:%S", iContextName );

    TInt ret( KErrArgument );

    CMmDataPackage data;
    data.PackData( &iContextName, &iServicesArray );
    ret = iMmPacketService->MessageManager()->HandleRequestL(
        EPacketContextDeactivate, &data );

    if ( KErrNone == ret )
        {
        iContextStatusBeforeDeactivation = ContextStatus();
        // Notify status change
        CompleteNotifyStatusChange( RPacketContext::EStatusDeactivating );        

        // Store the request handle
        iReqHandleType = EMultimodeMbmsContextDeactivate;
        }

    return ret;  
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CompleteDeactivate
// Complete the asynchronous Deactivate request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//

void CMmMBMSContextTsy::CompleteDeactivate( 
        CMmDataPackage* aDataPackage,
        TInt aResult )    
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETEDEACTIVATE_1,  "TSY: CMmMBMSContextTsy::CompleteDeactivate. Error: %{TSymbianErrorCodes}", aResult );
    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeMbmsContextDeactivate );

    // Notify status change
    if ( KErrNone == aResult )
        {                
        CompleteNotifyStatusChange( RPacketContext::EStatusInactive );
        // and reset possible session list
        if( iServicesArray )
            {                        
            iServicesArray->iSessionIdList.Reset();
            iServicesArray->iSessionIdList.Compress();
            }
        }
    else
        {
        CompleteNotifyStatusChange( iContextStatusBeforeDeactivation );
        
        // remove only entries which were succesfully removed
        if( (aDataPackage) && (aResult == KErrMbmsImpreciseServiceEntries) )
	        {
	        TInfoName* contextName = NULL;
	        RPacketMbmsContext::CMbmsSession* listOfFailedEntries = NULL;
	        aDataPackage->UnPackData( &contextName, &listOfFailedEntries);
	        if( listOfFailedEntries )
		        {
		        for( TInt i = 0; i < listOfFailedEntries->iSessionIdList.Count(); i++ )
		    	    {
		        	for( TInt ii = 0; ii < iServicesArray->iSessionIdList.Count(); ii++ )
			        	{
			        	if( listOfFailedEntries->iSessionIdList[i] == iServicesArray->iSessionIdList[ii] )	
					    	{
					        iServicesArray->iSessionIdList.Remove(ii);	
							iServicesArray->iSessionIdList.Compress();
							ii = iServicesArray->iSessionIdList.Count();
					        }	
				        }
			        }
		        }
	        }
        }

    // Complete the client request
    if ( EMultimodePacketMbmsReqHandleUnknown != reqHandle )
        {                
        CMmMBMSContextTsy::ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::DeleteL
// Delete a context id previously created on the phone.
// Note that although the context is considered as 'deleted', the 
// client's RPacketMbmsContext subsession remains. To remove this, a client must
// call Close() from RPacketMbmsContext.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::DeleteL()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_DELETEL_1,  "TSY: CMmMBMSContextTsy::DeleteL. Context name:%S", iContextName );

    TInt ret( KErrNotReady );
    if( RPacketContext::EStatusUnknown != ContextStatus() )
        {
        CMmDataPackage data;
        data.PackData( &iContextName );

        ret = iMmPacketService->MessageManager()->HandleRequestL(
            EPacketContextDelete, &data );

        }

    if ( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = EMultimodeMbmsContextDelete;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CompleteDelete
// Complete the asynchronous Delete request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//

void CMmMBMSContextTsy::CompleteDelete( 
        TInt aResult )        
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETEDELETE_1,  "TSY: CMmMBMSContextTsy::CompleteDelete. Error: %{TSymbianErrorCodes}", aResult );

    if( KErrNone == aResult )
        {
        iContextStatusBeforeDeactivation = RPacketContext::EStatusDeleted;

        // Notify status change
        CompleteNotifyStatusChange( RPacketContext::EStatusDeleted );

        delete iConfig;
        iConfig = NULL;            
        }

    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeMbmsContextDelete );

    if ( EMultimodePacketMbmsReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        CMmMBMSContextTsy::ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::GetConfig
// Requests the current configuration of the context.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::GetConfig(
        TTsyReqHandle aTsyReqHandle,    
        TPacketDataConfigBase* aConfig )  
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_GETCONFIG_1,  "TSY: CMmMBMSContextTsy::GetConfig. Context name:%S", iContextName );
    TInt ret( KErrNone );
    
    // Check that the mode given in aConfig is KConfigMBMS
    if ( TPacketDataConfigBase::KConfigMBMS == aConfig->ExtensionId() )
        {
        // Set configuration data to ETel side memory
        *( RPacketMbmsContext::TContextConfigMbmsV1* )aConfig =
        	*iConfig;
        }
    else
        {
        // Given mode is not supported
        ret = KErrArgument;
        }    

    // Complete Request
    CMmMBMSContextTsy::ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::GetLastErrorCause
// This function retrieves the last error cause to have occurred.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::GetLastErrorCause(
        TTsyReqHandle aTsyReqHandle,  
        TInt* aError )    
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_GETLASTERRORCAUSE_1,  "TSY: CMmMBMSContextTsy::GetLastErrorCause. Context name:%S Last error cause: %{TSymbianErrorCodes}", iContextName, iLastErrorCause );

    *aError = iLastErrorCause;
    CMmMBMSContextTsy::ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::SetLastErrorCause
// Set the last error cause.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMBMSContextTsy::SetLastErrorCause(
        TInt aErrorCause ) 
    {
    
     iLastErrorCause =  aErrorCause;           
    
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_SETLASTERRORCAUSE_1,  "TSY: CMmMBMSContextTsy::SetLastErrorCause.  aErrorCause: %{TSymbianErrorCodes} iLastErrorCause:%{TSymbianErrorCodes}", aErrorCause, iLastErrorCause );

    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::GetStatus
// Allows a client to get the current status of the context.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::GetStatus(
        TTsyReqHandle aTsyReqHandle,   
        RPacketContext::TContextStatus* aContextStatus )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_GETSTATUS_1,  "TSY: CMmMBMSContextTsy::GetStatus. Context name: %S Context Status:%d", iContextName, iContextStatus );
    *aContextStatus = iContextStatus;
    CMmMBMSContextTsy::ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::NotifyConfigChanged
// Requests notification when configuration of context changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::NotifyConfigChanged(
        TPacketDataConfigBase* aConfig )  
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_NOTIFYCONFIGCHANGED_1,  "TSY: CMmMBMSContextTsy::NotifyConfigChanged." );

    // Check that used mode is MBMS
    if (  TPacketDataConfigBase::KConfigMBMS != aConfig->ExtensionId() ) 
        {
        return KErrArgument;
        }

    // Store pointer
    iRetNotifyConfig = aConfig;
    // Store req handle type
    iReqHandleType = EMultimodeMbmsContextNotifyConfigChanged;        

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CompleteNotifyConfigChanged
// Complete notification when the context configuration changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMBMSContextTsy::CompleteNotifyConfigChanged(const CMmDataPackage* aDataPackage,
        TInt aResult)
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETENOTIFYCONFIGCHANGED_1,  "TSY: CMmMBMSContextTsy::CompleteNotifyConfigChanged.  Context name:%S",iContextName );
	if(aDataPackage != NULL)
		{
		if ( KErrNone == aResult )
		    {
		    RPacketMbmsContext::TContextConfigMbmsV1* aConfig = NULL;
		    TInfoName* name = NULL;
		    aDataPackage->UnPackData( &name, &aConfig );
		
		    if( NULL != aConfig )
		        {
		        iConfig->iTmgi = aConfig->iTmgi;
		        iConfig->iMbmsAccessBearer = aConfig->iMbmsAccessBearer;
		        iConfig->iMbmsServicePriority = aConfig->iMbmsServicePriority;
		        iConfig->iMbmsServiceMode = aConfig->iMbmsServiceMode;
		        iConfig->iMbmsSessionFlag = aConfig->iMbmsSessionFlag;
		       
		        }
		    }
		}

    // Get and reset req handle 
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeMbmsContextNotifyConfigChanged );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
#ifdef USING_CTSY_DISPATCHER
    	//CTSY defect, request is completed with KErrNone, aResult ignored.
    	if( (iConfig) && (aResult == KErrNone) )
#else
        if( NULL != iConfig )
#endif //USING_CTSY_DISPATCHER
	        {
	        	        
	        *( RPacketMbmsContext::TContextConfigMbmsV1* )iRetNotifyConfig = *iConfig;
	        // Null ret pointer
	        iRetNotifyConfig = NULL;	
	        }
        
        // Complete the client request
#ifdef USING_CTSY_DISPATCHER    	
        CMmMBMSContextTsy::ReqCompleted( reqHandle, aResult );
#else
        CMmMBMSContextTsy::ReqCompleted( reqHandle, KErrNone );
#endif //USING_CTSY_DISPATCHER
        }
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::NotifyStatusChange
// Requests notification when the status of the context changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::NotifyStatusChange(
        RPacketContext::TContextStatus* aContextStatus )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_NOTIFYSTATUSCHANGE_1,  "TSY: CMmMBMSContextTsy::NotifyStatusChange. Context name:%S", iContextName );
    iRetNotifyStatus = aContextStatus;
    iReqHandleType = EMultimodeMbmsContextNotifyStatusChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CompleteNotifyStatusChange
// Complete the notification when the context status changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMBMSContextTsy::CompleteNotifyStatusChange(
            RPacketContext::TContextStatus aContextStatus  )
    {
    if ( aContextStatus != iContextStatus )
        {
        iContextStatus = aContextStatus;
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETENOTIFYSTATUSCHANGE_1,  "TSY: CMmMBMSContextTsy::CompleteNotifyStatusChange. Context name: %S  Context status:%d",iContextName, iContextStatus );
                    
        if ( RPacketContext::EStatusActive == iContextStatus ||
             RPacketContext::EStatusInactive == iContextStatus ||
             RPacketContext::EStatusDeleted == iContextStatus )
            {
            iMmPacketService->ContextStatusChanged( iContextStatus );
            if( RPacketContext::EStatusDeleted == iContextStatus )
                {
                iContextStatusBeforeDeactivation = RPacketContext::EStatusDeleted;
                }
            }

		// if context status is receiving, update dynamic caps
        if ( RPacketContext::EStatusReceiving == iContextStatus)
            {
            iMmPacketService->ActivePacketServiceExtension()->SetDynamicCapsFlag( RPacketService::KCapsMBMS, ETrue );
            }
        // context status is changing from receiving to something else
        else
	        {
	        iMmPacketService->ActivePacketServiceExtension()->SetDynamicCapsFlag( RPacketService::KCapsMBMS, EFalse );
	        }
		
        // Reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
            EMultimodeMbmsContextNotifyStatusChange );

        if (( EMultimodePacketMbmsReqHandleUnknown != reqHandle ) && (iRetNotifyStatus != NULL)) 
            {
            // Get current status
            *iRetNotifyStatus = aContextStatus;
            // Null ret pointer
            iRetNotifyStatus = NULL;
            // Complete the client request
            CMmMBMSContextTsy::ReqCompleted( reqHandle, KErrNone );
            }
        }    
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CompleteNotifyStatusChange
// Complete the notification when the context status changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMBMSContextTsy::CompleteNotifyStatusChange(
        const CMmDataPackage* aDataPackage,
        TInt aResult ) 
    {
    if( KErrNone != aResult )
        {
        SetLastErrorCause( aResult );
        }
    else
    	{
    	TInfoName* null= NULL;
    	RPacketContext::TContextStatus contextStatus;
    
    	//initialise struct for completion params   
    	TContextMisc misc;
    
    	aDataPackage->UnPackData( null, misc );
    	contextStatus = misc.iStatus;

    	CMmMBMSContextTsy::CompleteNotifyStatusChange(contextStatus);
    	}
                         
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::SetConfigL
// Configure a context to the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextTsy::SetConfigL(
        TPacketDataConfigBase* aConfig ) 
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_SETCONFIGL_1,  "TSY: CMmMBMSContextTsy::SetConfigL. Context name:%S", iContextName );

    TInt ret( KErrArgument );
           
    if ( TPacketDataConfigBase::KConfigMBMS == aConfig->ExtensionId() )
        {
        if( !iIsInitialiseAllowed )
            {
            
            RPacketMbmsContext::TContextConfigMbmsV1* configMbms = 
                ( RPacketMbmsContext::TContextConfigMbmsV1* )aConfig;  

            if ( NULL == iTempConfig )
                {
                // Allocate memory for iTempConfig
                iTempConfig = 
                    new ( ELeave ) RPacketMbmsContext::TContextConfigMbmsV1();
                }

             // Temporary store the configMBMS
            *iTempConfig = *configMbms;

            CMmDataPackage data;
            data.PackData( &( *aConfig ), &iContextName );

OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_SETCONFIGL_2, "TSY: CMmMBMSContextTsy::SetConfigL. Context: %S", iContextName);

            ret = iMmPacketService->MessageManager()->HandleRequestL(
                EPacketContextSetConfig, &data ); 
    
            if ( KErrNone != ret )
                {
                // Release the memory allocated for iTempConfig
                delete iTempConfig;
                iTempConfig = NULL;
                }
            }
        else
            {
            ret = KErrNotReady;
            }
        }
                 
    if ( KErrNone == ret )
        {
        // Store the request handle type
        iReqHandleType = EMultimodePacketMbmsContextSetConfig;
        }             

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CompleteSetConfig
// Complete the asynchronous SetConfig request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMBMSContextTsy::CompleteSetConfig( 
        TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETESETCONFIG_1,  "TSY: CMmMBMSContextTsy::CompleteSetConfig. Error: %{TSymbianErrorCodes}", aError );
    
    if( (KErrNone == aError) && (iConfig) && (iTempConfig) )
	    {
	    *iConfig = *iTempConfig;  
	    // Release the memory allocated for iTempConfig
	    delete iTempConfig;
	    iTempConfig = NULL;
	    CompleteNotifyConfigChanged();    
	    }

    TTsyReqHandle reqHandle( EMultimodePacketMbmsReqHandleUnknown );
    reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(EMultimodePacketMbmsContextSetConfig );

    if ( EMultimodePacketMbmsReqHandleUnknown != reqHandle )
    	{
    	// Complete the client request
    	CMmMBMSContextTsy::ReqCompleted( reqHandle, aError );
    	}
	    	
    }


// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::ContextStatus
// Return current context status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RPacketContext::TContextStatus CMmMBMSContextTsy::ContextStatus() const
    {
    return iContextStatus;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::ReqCompleted
// Overloads CTelObject::ReqCompleted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//

void CMmMBMSContextTsy::ReqCompleted(
        TTsyReqHandle aTsyReqHandle, 
        TInt aError )   
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_REQCOMPLETED_1,  "TSY: CMmMBMSContextTsy::ReqCompleted. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", (TUint)aTsyReqHandle, aError, (TUint)this);

    // Set last error cause
    if ( KErrNone != aError )
        {
        iLastErrorCause = aError;
        }

    CTelObject::ReqCompleted( aTsyReqHandle, aError );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_REQCOMPLETED_2,  "<-- TSY: CMmMBMSContextTsy::ReqCompleted");
    }

#ifdef REQHANDLE_TIMER

    
// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::SetTypeOfResponse
// Sets the type of response for a given request handle. Automatic mode 
// includes an automatic response in case of non response from the DOS in a
// specified time. This method is used only if REQHANDLE_TIMER is in use.
// ---------------------------------------------------------------------------
//
void CMmMBMSContextTsy::SetTypeOfResponse(
        TInt aReqHandleType,               
        TTsyReqHandle aTsyReqHandle )       
    {
    TInt timeOut( 0 );

    // Set timeout by req handle type
    switch ( aReqHandleType )
        {
    	case EMultimodePacketMbmsInitialiseContext:
            timeOut = KMmPacketContextInitialiseContextTimeOut;
            break;
        case EMultimodePacketMbmsContextSetConfig:
            timeOut = KMmPacketContextSetConfigTimeOut;
            break;
        case EMultimodeMbmsContextActivate:
            timeOut = KMmPacketContextActivateTimeOut;
            break;
        case EMultimodeMbmsContextDeactivate:
            timeOut = KMmPacketContextDeactivateTimeOut;
            break;
        case EMultimodeMbmsContextDelete:
            timeOut = KMmPacketContextDeleteTimeOut;
            break;
        case EMultimodeMbmsContextUpdateMbmsSessionList:
            timeOut = KMmPacketContextModifyActiveContext;
            break;        
        // Can't use timer:
        // - all notifications
        //case EMultimodePacketContextNotifyConfigChanged:
        //case EMultimodePacketContextNotifyConnectionSpeedChange:
        //case EMultimodePacketContextNotifyStatusChange:
        default:
            // Do not use timer
            iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType, 
                aTsyReqHandle );
            break;
        }

    if ( 0 < timeOut )
        {
        // The timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType, aTsyReqHandle, 
            timeOut );
        }

    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::Complete
// Completes the request due the timer expiration. This method is used only if
// REQHANDLE_TIMER is in use. 
// ---------------------------------------------------------------------------
// 
void CMmMBMSContextTsy::Complete(
        const TInt aReqHandleType,    
        const TInt aError )        
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETE_1,  "TSY: CMmMBMSContextTsy::Complete. Req type %d", aReqHandleType );
    
    
    // All possible TSY req handle types are listed in the
    // switch case below.
    switch ( aReqHandleType )
        {
        case EMultimodePacketMbmsInitialiseContext:
            CMmMBMSContextTsy::CompleteInitialiseContext( aError, NULL );
            break;
        case EMultimodePacketMbmsContextSetConfig:
            CMmMBMSContextTsy::CompleteSetConfig( aError );
            break;
        case EMultimodeMbmsContextActivate:
            CMmMBMSContextTsy::CompleteActivate( aError );
            break;
        case EMultimodeMbmsContextDeactivate:
            CMmMBMSContextTsy::CompleteDeactivate( NULL, aError );
            break;
        case EMultimodeMbmsContextDelete:
            CMmMBMSContextTsy::CompleteDelete( aError );
            break;
        case EMultimodeMbmsContextUpdateMbmsSessionList:
            CMmMBMSContextTsy::CompleteUpdateMbmsSessionList( aError );
            break;
        // Can't use timer:
        // - all notifications
        //case EMultimodePacketContextNotifyConfigChanged:
        //case EMultimodePacketContextNotifyConnectionSpeedChange:
        //case EMultimodePacketContextNotifyStatusChange:
        default:
            CMmMBMSContextTsy::ReqCompleted( iTsyReqHandleStore->
				ResetTsyReqHandle( aReqHandleType ), aError );
            break;
        }

    }

#endif // REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::IsContextActivationAllowed
// Tells if context activation is allowed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TBool CMmMBMSContextTsy::IsContextActivationAllowed() const
    {
        return iIsActivateAllowed;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::ContextName
// This method returns name of this context
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
const TInfoName &CMmMBMSContextTsy::ContextName() const
    {
    return iContextName;
    }


// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::ContextIndex
// This method returns name of this context
// (other items were commented in a header).
// ---------------------------------------------------------------------------
// 
TInt CMmMBMSContextTsy::ContextIndex() const
    {
    return iObjectId;
    }
     
// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::UpdateMbmsSessionList
// Method to change content of session list
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//         
TInt CMmMBMSContextTsy::UpdateMbmsSessionList( 
    TMbmsAction* aAction,
    TMbmsSessionId* aSession ) 
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_UPDATEMBMSSESSIONLIST_1, "TSY: CMmMBMSContextTsy::UpdateMbmsSessionList. Context: %S, action: %d", iContextName, *aAction);
    TInt ret( KErrNone );
    
    if( iConfig->iMbmsSessionFlag )
        {
        iAction = *aAction;
        iSession = *aSession;
        
        // changes send to LTSY
        TMmMbmsActiveServiceListParams activeServiceList;
        activeServiceList.iActionType = iAction;
        activeServiceList.iServiceList = iServicesArray;
        activeServiceList.iChangedItem = iSession;
         
        CMmDataPackage data;
        data.PackData( &iContextName, &activeServiceList );

        TRAPD(err, ret = iMmPacketService->MessageManager()->HandleRequestL(
                    EPacketContextUpdateMbmsSessionList, &data ));
		if(err !=KErrNone)
			{
			ret = err;
			}
        }
    else
        // this context does not support sessions
        {
        ret = KErrNotSupported;
        }
                
    if ( KErrNone == ret )
        {
        // Store the request handle type
        iReqHandleType = EMultimodeMbmsContextUpdateMbmsSessionList;
        }                   
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CompleteUpdateMbmsSessionList
// Complete change in content of session list to client
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//         
void CMmMBMSContextTsy::CompleteUpdateMbmsSessionList( 
        const TInt aResult )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_COMPLETEUPDATEMBMSSESSIONLIST_1, "TSY: CMmMBMSContextTsy::CompleteUpdateMbmsSessionList. Context: %S, result: %d", iContextName, aResult);
    
    TInt ret( aResult );
   
    if( iMmPacketService->MaximumActiveServices() > iMmPacketService->EnumerateMbmsActiveServices() )  
        {        
        if( KErrNone == ret  )
            {
            switch( iAction )
                {
                case EAddEntries:
                    {
                    // verify that item id does not exists
                    for( TInt i=0; i<iServicesArray->iSessionIdList.Count(); i++ )
                        {
                        if( iServicesArray->iSessionIdList[i] == iSession )
                            {
                            // item already exists
                            ret = KErrAlreadyExists;
                            i = iServicesArray->iSessionIdList.Count();
                            }
                        }
                    // if ID is a new one; add it to array
                    if( KErrNone == ret )
                        {
                        iServicesArray->iSessionIdList.Append(iSession);
                        iServicesArray->iSessionIdList.Compress();
                        }
                    break;
                    }                	
                case ERemoveEntries:
            	    {
            	    TInt idx( -1 );
            	    // find index of item to remove
                    for( TInt i=0; i<iServicesArray->iSessionIdList.Count(); i++ )
                        {
                        if( iServicesArray->iSessionIdList[i] == iSession )
                            {
                            idx = i;
                            i = iServicesArray->iSessionIdList.Count();
                            }
                        }
                    // item is in array, remove it
                    if( 0 <= idx )
                        {
                        iServicesArray->iSessionIdList.Remove(idx);
                        iServicesArray->iSessionIdList.Compress();
                        }        	    
                    else
                        {                    
                        ret = KErrNotFound;
                        }
                    break;
            	    
            	    }
                case ERemoveAllEntries:
                    {
                    // just empty array
                    iServicesArray->iSessionIdList.Reset();
                    iServicesArray->iSessionIdList.Compress();
                    break;
                    }
                default:
                    break;                
                }        
            }
        }
    else
        {
        ret = KErrOverflow;
        }
        
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodeMbmsContextUpdateMbmsSessionList );        
        
    if ( EMultimodePacketMbmsReqHandleUnknown != reqHandle )
        {                            
        // Complete the client request
        CMmMBMSContextTsy::ReqCompleted( reqHandle, ret );
        }
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::GetMbmsActiveServicesPhase1L
// First phase of MBMS session retrieval
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//     
TInt CMmMBMSContextTsy::GetMbmsActiveServicesPhase1L(
	        TTsyReqHandle aTsyReqHandle,
	        RMobilePhone::TClientId* aClient,
	        TInt* aBufSize )
    {
    TInt ret( KErrNone );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_GETMBMSACTIVESERVICESPHASE1L_1, "TSY: CMmMBMSContextTsy::GetMbmsActiveServicesPhase1L");
  
  	if (iServicesArray->iSessionIdList.Count() > 0)
  		{
  		// Presently internal attribute, used only by this method.
    	iClientId = *aClient;
   
	    // Serialize data (RPacketMbmsContext::CMbmsSession)
	    // into iActiveSessionInfoBuffer
	    iServicesArray->ExternalizeL( iActiveSessionInfoBuffer );
				
	    *aBufSize = iActiveSessionInfoBuffer->Length();                    
	                                       
	    ReqCompleted( aTsyReqHandle, KErrNone );
  		}
  	else
	  	{
	  	ret = KErrNotFound;
	  	}
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::GetMbmsActiveServicesPhase2L
// Second phase of MBMS session retrieval
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//      
TInt CMmMBMSContextTsy::GetMbmsActiveServicesPhase2L(
        	TTsyReqHandle aTsyReqHandle,
        	RMobilePhone::TClientId* aClient,
	        TDes8* aBuffer )   
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTTSY_GETMBMSACTIVESERVICESPHASE2L_1, "TSY: CMmMBMSContextTsy::GetMbmsActiveServicesPhase2L");
	
	TInt error = KErrNone;
	
	// Check client id
	if (( aClient->iSessionHandle != iClientId.iSessionHandle 
		|| aClient->iSubSessionHandle != 
                              iClientId.iSubSessionHandle  ) || (iActiveSessionInfoBuffer == NULL))
		{
		error = KErrBadHandle;
		}
	else
		{
		// Copy data to client buffer	
	  	TPtr8 bufPtr = iActiveSessionInfoBuffer->Des();
	  	if ( bufPtr.Length() <= aBuffer->Length() )
	  		{
			aBuffer->Copy( bufPtr );
			}
		else
			{
			error = KErrArgument;
			}
		
		// delete the buffer
		if ( iActiveSessionInfoBuffer )
			{
			delete iActiveSessionInfoBuffer;
			iActiveSessionInfoBuffer = NULL;
			}
		}
	ReqCompleted( aTsyReqHandle, error );
    return KErrNone;
    }



// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::CancelGetMbmsActiveServicesPhase1
// Cancelling MBMS session retrieval
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//       
TInt CMmMBMSContextTsy::CancelGetMbmsActiveServicesPhase1()   
    {
	// delete the buffer
	if ( iActiveSessionInfoBuffer )
		{
		delete iActiveSessionInfoBuffer;
		iActiveSessionInfoBuffer = NULL;
		}    

    ReqCompleted( iTsyReqHandle, KErrCancel );
    return KErrNone;	    
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::SessionCount
// Returns the session count to a MBMS context
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//       
TInt CMmMBMSContextTsy::SessionCount()
    {
    
    return iServicesArray->iSessionIdList.Count();
    
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextTsy::ContextType
// return the context type
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPacketContextTsy::TPacketContextType CMmMBMSContextTsy::ContextType() const
{
	return EContextTypeMBMS;
}

//  End of File 
