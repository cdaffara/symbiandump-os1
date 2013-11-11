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
#include "CmmpacketcontexttsyTraces.h"
#endif

#include "Cmmpacketcontexttsy.h"
#include "Cmmpacketcontextlist.h"
#include "cmmpacketservicegsmwcdmaext.h"
#include "CMmPacketTsy.h"
#include "MmTsy_timeoutdefs.h"
#include "MmTsy_numberOfSlots.h"

// ============================ MEMBER FUNCTIONS ===============================


CMmPacketContextTsy::CMmPacketContextTsy():
    iReqHandleType(EMultimodePacketContextReqHandleUnknown)
    {
    }

CMmPacketContextTsy* CMmPacketContextTsy::NewL(
        CMmPacketServiceTsy* const aMmPacketService,
        const TInfoName& aHostCID, 
        const TDes& aName,  
        const TUint8 aProxyId ) 
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_NEWL_1,  "TSY: CMmPacketContextTsy::NewL. Context name:%S", aName );

    CMmPacketContextTsy* packetContext = new ( ELeave ) CMmPacketContextTsy();

    CleanupClosePushL( *packetContext );
    packetContext->iMmPacketService = aMmPacketService;
    packetContext->iContextName = aName;
    packetContext->iProxyId = aProxyId;
    packetContext->iHostCID = aHostCID;
    packetContext->ConstructL();
    CleanupStack::Pop();
    
    return packetContext;
    }

void CMmPacketContextTsy::ConstructL()
    {
    InitModulesL();
    InitInternalAttributes();    
    }
    
CMmPacketContextTsy::~CMmPacketContextTsy()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_DTOR_1,  "TSY: CMmPacketContextTsy::~CMmPacketContextTsy. Context name: %S", iContextName );
	
	if( iMmPacketService )
		{
			
		iMmPacketService->PacketContextList()->RemoveObject( this );

	    // Set Dynamic Caps
	    iMmPacketService->ActivePacketServiceExtension()->SetDynamicCapsFlag(
	        RPacketService::KCapsActivate, EFalse );
	    
	    if ( iIsDialUpContext )
	        {
	        iMmPacketService->ResetPointerToDialUpContext( iContextName );
	        }
	    
		}
		
    delete iMmPacketContextGsmWcdmaExt;
    delete iTsyReqHandleStore;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::InitModulesL
// Initialises extension modules for CMmPacketContextTsy
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::InitModulesL()
    {

    iMmPacketContextGsmWcdmaExt = 
        CMmPacketContextGsmWcdmaExt::NewL( this, iMmPacketService );

#ifdef REQHANDLE_TIMER   
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL(  this,
        iMmPacketService->PhoneTsy(), EMaxNumOfPacketContextRequests,
        iPacketContextReqHandles );

#else
        // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        EMaxNumOfPacketContextRequests, iPacketContextReqHandles );

#endif // REQHANDLE_TIMER

    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::InitInternalAttributes
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::InitInternalAttributes()
    {
    iQoSProfile = NULL;
    iQoSProfileName.Zero();

    iLastErrorCause = KErrNone;
    iContextStatus = RPacketContext::EStatusUnknown;
    iContextStatusBeforeSuspending = RPacketContext::EStatusUnknown;
    iConnectionSpeed = 0;
    iNotifyConnectionSpeed = 0;
    iIsDialUpContext = EFalse;

    if ( iContextName.Compare( KStringExternal )== 0 
         || iContextName.Compare( KStringExternal2 )== 0 )
        {
        iIsDialUpContext = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::Init
// Initialisation method that is called from ETel Server.
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::Init()
    {
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ExtFunc
// TRAP's all CMmPacketContextTsy related MM Packet API requests in case that
// they fail. This method functions only as a centralized TRAP for the
// DoExtFuncL method that does the actual mapping of IPC number to TSY method
// call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::ExtFunc( 
        const TTsyReqHandle aTsyReqHandle, 
        const TInt aIpc, 
        const TDataPackage& aPackage )  
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_EXTFUNC_1,  "TSY: CMmPacketContextTsy::ExtFunc. IPC: %{TIPCNamesList} Context name:%S", (TUint)aIpc, iContextName );

    TInt ret( KErrNone );
    TInt trapError( KErrNone );

#ifdef ADD_REMOVE_PACKETFILTER_DEFECT_FIXED  // search for this up from bottom of file
    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==EMultimodePacketContextReqHandleUnknown, User::Invariant());
#else
    // Reset last tsy request type
    iReqHandleType = EMultimodePacketContextReqHandleUnknown;
#endif

    // Trap the call of DoExtFuncL
    TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ) );

    if ( KErrNone != trapError )
        {
		// Reset the request handle to indicate the request is no longer ongoing
        iTsyReqHandleStore->FindAndResetTsyReqHandle( aTsyReqHandle );
        ReqCompleted( aTsyReqHandle, trapError );
        }

    else if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }

    else if ( EMultimodePacketContextReqHandleUnknown != iReqHandleType )
        {
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, aTsyReqHandle );
#endif // REQHANDLE_TIMER
        }
#ifdef ADD_REMOVE_PACKETFILTER_DEFECT_FIXED  // search for this up from bottom of file
     // We've finished with this value now. Clear it so it doesn't leak
     //  up to any other instances of this method down the call stack
     iReqHandleType = EMultimodePacketContextReqHandleUnknown;
#endif
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::DoExtFuncL
// Packet Data specific functionality of CMmPacketContextTsy::ExtFunc
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::DoExtFuncL( 
        const TTsyReqHandle aTsyReqHandle,      
        const TInt aIpc,      
        const TDataPackage& aPackage )   
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_DOEXTFUNCL_1,  "TSY: CMmPacketContextTsy::DoExtFuncL. IPC: %{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, (TInt)aTsyReqHandle, (TUint)this );

    TInt ret( KErrNotSupported );

    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();

    switch ( aIpc )
        {
        case EPacketContextNotifyStatusChange:
            ret = NotifyStatusChange( 
                REINTERPRET_CAST( RPacketContext::TContextStatus*, dataPtr ) );
            break;
        case EPacketContextNotifyConfigChanged:
            ret = NotifyConfigChanged( 
                REINTERPRET_CAST( TPacketDataConfigBase*, dataPtr ) );
            break;
        case EPacketContextNotifyConnectionSpeedChange:
            ret = NotifyConnectionSpeedChange( 
                REINTERPRET_CAST( TUint*, dataPtr ) );
            break;
        case EPacketContextInitialiseContext:
            {
            TDes8* paramData = aPackage.Des1n();
            
            if(paramData != NULL)
            	{
            	if(sizeof(RPacketContext::TDataChannelV2) > paramData->MaxLength())
            		{
            		ret = KErrArgument;
            		}
            	else
            		{
            		ret = InitialiseContextL( reinterpret_cast< RPacketContext::TDataChannelV2*>( dataPtr ));
            		}
            	}
            else
            	{
            	ret = KErrArgument;
            	}
            break;
            }
        case EPacketContextSetConfig:
            ret = SetConfigL( 
                REINTERPRET_CAST( TPacketDataConfigBase*, dataPtr ) );
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
                REINTERPRET_CAST( RPacketContext::TContextStatus*, dataPtr ) );
            break;
        case EPacketContextGetConfig:
            ret = GetConfig( aTsyReqHandle, 
                REINTERPRET_CAST( TPacketDataConfigBase*, dataPtr ) );
            break;
        case EPacketContextGetProfileName:
            ret = GetProfileName( aTsyReqHandle, 
                REINTERPRET_CAST( TInfoName*, dataPtr ) );
            break;
        case EPacketContextGetConnectionSpeed:
            ret = GetConnectionSpeed( aTsyReqHandle, 
                REINTERPRET_CAST( TUint*, dataPtr ) );
            break;
        case EPacketContextGetLastErrorCause:
            ret = GetLastErrorCause( aTsyReqHandle, 
                REINTERPRET_CAST( TInt*, dataPtr ) );
            break;
        case EPacketContextGetDataVolumeTransferred:
            ret = GetDataVolumeTransferredL( aTsyReqHandle,
                REINTERPRET_CAST( RPacketContext::TDataVolume*, dataPtr ) );
            break;
        case EPacketContextEnumeratePacketFilters:
            ret = EnumeratePacketFilters( aTsyReqHandle,
                REINTERPRET_CAST( TInt*, dataPtr ) );
            break;
        case EPacketContextGetPacketFilterInfo:
            {
            TDes8* paramData = aPackage.Des2n();
            
            if (paramData)
            	{
            	TInt paramLength = paramData->MaxLength();
            	
            	RPacketContext::TPacketFilterV2 temp;
            	RPacketContext::TPacketFilterV2Pckg tempPckg(temp);
            	
            	TInt expectedLength = tempPckg.MaxLength();

				if (paramLength == expectedLength)
					{
		            ret = GetPacketFilterInfo( aTsyReqHandle, REINTERPRET_CAST( TInt*, dataPtr ), REINTERPRET_CAST( TPacketBase*, dataPtr2 ) );
					}
				else
					{
					ret = KErrArgument;
					}
            	}
            else
            	{
            	ret = KErrArgument;
            	}
            	
            }
            break;
        case EPacketContextAddPacketFilter:
            ret = AddPacketFilterL( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EPacketContextRemovePacketFilter:
#ifdef USING_CTSY_DISPATCHER            
        	ret = RemovePacketFilter( REINTERPRET_CAST( TInt*, dataPtr ) );
#else
        	ret = RemovePacketFilter( aTsyReqHandle,
                REINTERPRET_CAST( TInt*, dataPtr ) );     
#endif        	
        	break;
        case EPacketContextModifyActiveContext:
            ret = ModifyActiveContextL();
            break;
        case EPacketContextGetDNSInfo:
            ret = GetDnsInfo( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EPacketAddMediaAuthorization:
	        ret = AddMediaAuthorizationL( aTsyReqHandle, aPackage.Des1n() );
	        break;
	    case EPacketRemoveMediaAuthorization:
	        ret = RemoveMediaAuthorization( aTsyReqHandle, 
	        		REINTERPRET_CAST( RPacketContext::TAuthorizationToken*,
                    dataPtr ) );
	        break;	        
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextGetConnectionInfo:
            ret = GetConnectionInfo( aTsyReqHandle, 
                reinterpret_cast<TConnectionInfoBase*>( dataPtr ) );            
            break;
        case EPacketContextNotifyConnectionInfoChange:
            ret = NotifyConnectionInfoChange(
                reinterpret_cast<TConnectionInfoBase*>( dataPtr ) ); 
            break;      
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        // These requests currently not supported
        //case EPacketContextLoanCommPort:
        //case EPacketContextRecoverCommPort:
        //case EPacketContextNotifyDataTransferred:
        default:
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_DOEXTFUNCL_2, "<-- TSY: CMmPacketContextTsy::DoExtFuncL, error = %{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ReqModeL
// Returns request mode for given IPC.
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmPacketContextTsy::ReqModeL( 
        const TInt aIpc ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_REQMODEL_1,  "TSY: CMmPacketContextTsy::ReqModeL. IPC: %{TIPCNamesList}", aIpc );

    CTelObject::TReqMode reqMode( 0 );
    TBool doLeave( EFalse );

    switch ( aIpc )
        {
        // All disabled
        // TSYs wishing to implement their own buffering algorithm will place 
        // all requests in this category.  

        // Get methods that do not use DOS and return immediately. 
        // Flow control not required.
        case EPacketContextGetConnectionSpeed:
        case EPacketContextGetLastErrorCause:
        case EPacketContextGetProfileName:
        case EPacketContextEnumeratePacketFilters:
        case EPacketContextGetPacketFilterInfo:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextGetConnectionInfo:                      
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

        // Other methods that do not use DOS and return immediately. 
        // Flow control not required.
    
        // Methods that can propably take a long time and cannot therefore be
        // flow controlled. Solution: All these methods must check req handle
        // table before handling the request. In case that the request table 
        // indicates that same method has been called and has not been 
        // completed, the method should return KErrServerBusy.
            if ( iIsDialUpContext )
                {
                doLeave = ETrue;
                break;
                }
        case EPacketContextGetStatus:
        case EPacketContextGetConfig:
            break;

        // KReqModeFlowControlObeyed
        // Commands that change the state of the phone, e.g. clearing the AoC 
        // counter, are commands that the TSY should only deal with one at 
        // a time.

        case EPacketContextInitialiseContext:
        case EPacketContextActivate:
        case EPacketContextDelete:
        case EPacketContextSetConfig:
        case EPacketContextAddPacketFilter:
        case EPacketContextRemovePacketFilter:
        case EPacketContextModifyActiveContext:
        case EPacketContextGetDNSInfo:
        case EPacketAddMediaAuthorization:
        case EPacketRemoveMediaAuthorization:
            if ( iIsDialUpContext )
                {
                doLeave = ETrue;
                break;
                }
        case EPacketContextDeactivate:
        case EPacketContextGetDataVolumeTransferred:

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
        case EPacketContextNotifyConnectionSpeedChange:
            if ( iIsDialUpContext )
                {
                doLeave = ETrue;
                break;
                }
        case EPacketContextNotifyStatusChange:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange:           
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
            reqMode = KReqModeMultipleCompletionEnabled |
                KReqModeRePostImmediately;
            break;

        // Cancel Requests
        // It's not necessary to include the Cancel methods in ReqModeL at all.
        // The ETel server never calls ReqModeL with a Cancel IPC.

        // Other variations of return values are unusable.
        // Following requests are not supported
        //case EPacketContextNotifyDataTransferred:
        //case EPacketContextLoanCommPort:
        //case EPacketContextRecoverCommPort:
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
// CMmPacketContextTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::NumberOfSlotsL(    
        const TInt aIpc )                            
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
        case EPacketContextNotifyConnectionSpeedChange:
            numberOfSlots = KMmPacketContextNotifyConnectionSpeedChangeSlots;
            break;
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange:
            numberOfSlots = KMmPacketContextNotifyConnectionInfoChangeSlots;
            break;                       
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        default:
            // Unknown or invalid IPC
            User::Leave( KErrNotSupported );
            break;

        }  
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_NUMBEROFSLOTSL_1,  "TSY: CMmPacketContextTsy::NumberOfSlotsL. IPC: %{TIPCNamesList} Number of slots: %d",aIpc, numberOfSlots );

    return numberOfSlots;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CancelService
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
TInt CMmPacketContextTsy::CancelService(    
        const TInt aIpc,                            
        const TTsyReqHandle aTsyReqHandle )         
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_CANCELSERVICE_1,  "TSY: CMmPacketContextTsy::CancelService. IPC: %{TIPCNamesList} Handle:%d",aIpc, aTsyReqHandle );

    TInt ret( KErrNone );
	TTsyReqHandle reqHandle( NULL );

#ifdef ADD_REMOVE_PACKETFILTER_DEFECT_FIXED  // search for this up from bottom of file
    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==EMultimodePacketContextReqHandleUnknown, User::Invariant());
#else
    // Reset last tsy request type
    iReqHandleType = EMultimodePacketContextReqHandleUnknown;
#endif

    switch ( aIpc )
        {
        // TSY has started a request and it is not possible to then cancel 
        // this request. The best thing for the TSY to do in this case is to
        // proceed as though the Cancel never happened. The server's call to
        // the TSY cancel function will return synchronously. The TSY then
        // continues to wait for the original acknowledgement and when it
        // receives it, the TSY will complete the original request.
        case EPacketContextInitialiseContext:
        case EPacketContextActivate:
        case EPacketContextDeactivate:
        case EPacketContextDelete:
        case EPacketContextSetConfig:
        case EPacketContextGetConfig:
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
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextGetConnectionInfo:                     
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
            break;
        // Notification Cancels, no special requirements.
        case EPacketContextNotifyStatusChange:
            iRetNotifyStatus = NULL;
            iReqHandleType = EMultimodePacketContextNotifyStatusChange;
            break;
        case EPacketContextNotifyConfigChanged:
            reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
                EMultimodePacketContextNotifyConfigChanged );
            if ( aTsyReqHandle == reqHandle )
                {
                iRetNotifyConfig = NULL;
                iReqHandleType = EMultimodePacketContextNotifyConfigChanged;
                }
            else
                {
                reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
                    EMultimodePacketContextNotifyConfigChanged99 );
                if ( aTsyReqHandle == reqHandle )
                    {
                    iRetNotifyConfig2 = NULL;
                    iReqHandleType =
                        EMultimodePacketContextNotifyConfigChanged99;
                    }
                }
            break;
        case EPacketContextNotifyConnectionSpeedChange:
            iRetNotifyConnectionSpeed = NULL;
            iReqHandleType = 
                EMultimodePacketContextNotifyConnectionSpeedChange;
            break;
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange:
            iRetNotifyConnectionInfo = NULL;
            iReqHandleType =
                EMultimodePacketContextNotifyConnectionInfoChange;
            break;                       
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        // IPCs that are not supported
        //case EPacketContextLoanCommPort:
        //case EPacketContextRecoverCommPort:
        //case EPacketContextNotifyDataTransferred:
        // Default
        default:
            ret = KErrNotSupported;
            break;
        }

    if ( EMultimodePacketContextReqHandleUnknown != iReqHandleType )
        {
        // Reset tsy request handle
        iTsyReqHandleStore->ResetTsyReqHandle( iReqHandleType );

        // Complete request
        CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, KErrCancel );
#ifdef ADD_REMOVE_PACKETFILTER_DEFECT_FIXED  // search for this up from bottom of file
        // We've finished with this value now. Clear it so it doesn't leak
        //  up to any other instances of this method down the call stack
        iReqHandleType = EMultimodePacketContextReqHandleUnknown;
#endif
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::RegisterNotification
// Register notification. RegisterNotification is called when the ETel Server
// recognises that this notification is being posted for the first time on
// this subsession object.It enables the TSY to "turn on" any regular 
// notification messages that it may receive from DOS. If this object is
// Dial-Up context only supported notification is 
// EPacketContextNotifyStatusChange.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::RegisterNotification( 
        const TInt aIpc )              
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_REGISTERNOTIFICATION_1,  "TSY: CMmPacketContextTsy::RegisterNotification. IPC: %{TIPCNamesList}", aIpc );

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EPacketContextNotifyConfigChanged:
        case EPacketContextNotifyConnectionSpeedChange:
            if ( iIsDialUpContext )
                {
                ret = KErrNotSupported;
                break;
                }
        case EPacketContextNotifyStatusChange:          
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange:                       
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
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
// CMmPacketContextTsy::DeregisterNotification
// Deregister notification. DeregisterNotification is called when the ETel
// Server recognises that this notification will not be posted again because 
// the last client to have a handle on this sub-session object has just closed 
// the handle. It enables the TSY to "turn off" any regular notification
// messages that it may receive from DOS. If this object is Dial-Up context
// only supported notification is EPacketContextNotifyStatusChange.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::DeregisterNotification(
        const TInt aIpc )                          
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_DEREGISTERNOTIFICATION_1,  "TSY: CMmPacketContextTsy::DeregisterNotification. IPC: %{TIPCNamesList}", aIpc );

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EPacketContextNotifyConfigChanged:
        case EPacketContextNotifyConnectionSpeedChange:
            if ( iIsDialUpContext )
                {
                ret = KErrNotSupported;
                break;
                }
        case EPacketContextNotifyStatusChange:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange:                    
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
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
// CMmPacketContextTsy::OpenNewObjectL
// Create new QoS object and return a pointer to it.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmPacketContextTsy::OpenNewObjectL(    
        TDes& aNewName )                          
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_OPENNEWOBJECTL_1,  "TSY: CMmPacketContextTsy::OpenNewObjectL." );

    // Each context can have only one QoS
    if ( NULL != iQoSProfile )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_OPENNEWOBJECTL_2,  "TSY: CMmPacketContextTsy::OpenNewObjectL. Leaves with: %{TSymbianErrorCodes}", KErrAlreadyExists );
        User::Leave( KErrAlreadyExists );
        }
    
    else
        {
        // Create new QoS object
        iQoSProfile = CMmPacketQoSTsy::NewL( iMmPacketService, this );

        iQoSProfileName.Copy( iContextName );
        aNewName.Copy( iQoSProfileName );
        }
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_OPENNEWOBJECTL_3,  "TSY: CMmPacketContextTsy::OpenNewObjectL. aNewName: %S", aNewName );

    return iQoSProfile;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::OpenNewObjectByNameL
// Return a pointer to QoS if it exists. The ETel Server calls this method 
// after some client has requested to open existing QoS object. The TSY 
// classes must not use this method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmPacketContextTsy::OpenNewObjectByNameL(    
        const TDesC& aName )                     
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_OPENNEWOBJECTBYNAMEL_1,  "TSY: CMmPacketContextTsy::OpenNewObjectByNameL. aName: %S", aName );

    if ( aName != iQoSProfileName )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_OPENNEWOBJECTBYNAMEL_2,  "TSY: CMmPacketContextTsy::OpenNewObjectByNameL. Leaves with: %{TSymbianErrorCodes}", KErrNotFound );
        User::Leave( KErrNotFound );
        }

    return iQoSProfile;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::InitialiseContextL
// This function initialises the context created previously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::InitialiseContextL(RPacketContext::TDataChannelV2* aDataChannel )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_INITIALISECONTEXTL_1,  "TSY: CMmPacketContextTsy::InitialiseContextL." );

    TInt ret( KErrNone );

    // Initialise Context
    ret = iMmPacketContextGsmWcdmaExt->InitialiseContextL(aDataChannel);

    if ( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = EMultimodePacketContextInitialiseContext;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteInitialiseContext
// Complete the asynchronous InitialiseContext request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteInitialiseContext(const TInt aResult )     
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETEINITIALISECONTEXT_1,  "TSY: CMmPacketContextTsy::CompleteInitialiseContext. Error: %{TSymbianErrorCodes}", aResult );

    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketContextInitialiseContext );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
               // Complete the client request
        CMmPacketContextTsy::ReqCompleted( reqHandle, aResult );
        }

    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ActivateL
// Activate a context. This function activates a context that has been
// previously configured using the SetConfig() method from RPacketContext.
// Once this function completes, the context will remain in an 'Activating'
// state until PPP has been successfully negotiated between the phone & the
// packet network gateway.Once PPP is negotiated, the context can be 
// considered to be 'Activated'.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::ActivateL()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_ACTIVATEL_1,  "TSY: CMmPacketContextTsy::ActivateL. Context name:%S", iContextName );

    TInt ret( KErrGprsServicesNotAllowed );

    if ( iMmPacketService->IsActivationAllowed() )
        {
        // Call ActivateL
        ret = iMmPacketContextGsmWcdmaExt->ActivateL();
        }

    if ( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = EMultimodePacketContextActivate;
        }

    return ret;        
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteActivate
// Complete the asynchronous Activate request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteActivate( 
        const TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETEACTIVATE_1,  "TSY: CMmPacketContextTsy::CompleteActivate. Error: %{TSymbianErrorCodes}", aResult );
    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketContextActivate );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
       
        // Complete the client request
        CMmPacketContextTsy::ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::Deactivate
// Deactivate a context previously configured on the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::DeactivateL()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_DEACTIVATEL_1,  "TSY: CMmPacketContextTsy::DeactivateL. Context name:%S", iContextName );
    TInt ret( KErrNone );

    // Call Deactivate
    ret = iMmPacketContextGsmWcdmaExt->DeactivateL();

    if ( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = EMultimodePacketContextDeactivate;
        }

    return ret;  
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteDeactivate
// Complete the asynchronous Deactivate request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteDeactivate( 
        const TInt aResult )    
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETEDEACTIVATE_1,  "TSY: CMmPacketContextTsy::CompleteDeactivate. Error: %{TSymbianErrorCodes}", aResult );
    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketContextDeactivate );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
        
        // Complete the client request
        CMmPacketContextTsy::ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::DeleteL
// Delete a context id previously created on the phone.
// Note that although the context is considered as 'deleted', the 
// client's RPacketContext subsession remains. To remove this, a client must
// call Close() from RPacketContext.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::DeleteL()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_DELETEL_1,  "TSY: CMmPacketContextTsy::DeleteL. Context name:%S", iContextName );

    TInt ret( KErrNone );

    // Call DeleteL
    ret = iMmPacketContextGsmWcdmaExt->DeleteL();

    if ( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = EMultimodePacketContextDelete;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteDelete
// Complete the asynchronous Delete request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteDelete( 
        const TInt aResult )        
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETEDELETE_1,  "TSY: CMmPacketContextTsy::CompleteDelete. Error: %{TSymbianErrorCodes}", aResult );
    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketContextDelete );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        CMmPacketContextTsy::ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::GetConfig
// Requests the current configuration of the context.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::GetConfig(
        const TTsyReqHandle aTsyReqHandle,    
        TPacketDataConfigBase* const aConfig )  
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_GETCONFIG_1,  "TSY: CMmPacketContextTsy::GetConfig. Context name:%S", iContextName );

    // Get config
    TInt ret = iMmPacketContextGsmWcdmaExt->GetConfig( aConfig );

    // Complete Request
    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::GetConnectionSpeed
// Allows a client to get the current connection speed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::GetConnectionSpeed(
        const TTsyReqHandle aTsyReqHandle,         
        TUint* const aRate ) 
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_GETCONNECTIONSPEED_1,  "TSY: CMmPacketContextTsy::GetConnectionSpeed. Context name: %S Connection Speed:%u bps",iContextName, iConnectionSpeed );

    // Get current connection speed rate
    *aRate = iConnectionSpeed;

    // Complete Request
    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::GetDataVolumeTransferred
// This function gets the data volume transferred.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::GetDataVolumeTransferredL(
        const TTsyReqHandle aTsyReqHandle,   
        RPacketContext::TDataVolume* const aVolume ) 
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_GETDATAVOLUMETRANSFERREDL_1,  "TSY: CMmPacketContextTsy::GetDataVolumeTransferredL. Context name:%S", iContextName );

    TInt ret( KErrNone );

    if ( RPacketContext::EStatusDeleted != iContextStatus )
        {
        // Get data volume transferred
        ret = iMmPacketContextGsmWcdmaExt->GetDataVolumeTransferredL();

        if ( KErrNone == ret )
            {
            // Store pointer
            iRetDataVolume = aVolume;
            // Store the request handle type
            iReqHandleType = EMultimodePacketContextGetDataVolumeTransferred;
            }
        }
    else
        {
        // Get current data volume
        *aVolume = iDataVolume;
        // Complete request
        CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, KErrNone );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteGetDataVolumeTransferred
// Complete the asynchronous GetDataVolumeTransferred request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteGetDataVolumeTransferred( 
        const TInt aResult )  
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETEGETDATAVOLUMETRANSFERRED_1,  "TSY: CMmPacketContextTsy::CompleteGetDataVolumeTransferred. Error: %{TSymbianErrorCodes}", aResult );

    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketContextGetDataVolumeTransferred );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
        
        if ( KErrNone == aResult )
            {
            *iRetDataVolume = iDataVolume;
            }

        // Null ret pointer
        iRetDataVolume = NULL;

        // Complete the client request
        CMmPacketContextTsy::ReqCompleted( reqHandle, aResult );
        }

    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::GetLastErrorCause
// This function retrieves the last error cause to have occurred.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::GetLastErrorCause(
        const TTsyReqHandle aTsyReqHandle,  
        TInt* const aError )    
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_GETLASTERRORCAUSE_1,  "TSY: CMmPacketContextTsy::GetLastErrorCause. Context name:%S Last error cause: %{TSymbianErrorCodes}",iContextName, iLastErrorCause );

    *aError = iLastErrorCause;
    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::GetProfileName
// Allows a client to retrieve the name of the QoS profile. The QoS profile is
// associated with the context. If no QoS profile has been defined by the
// client, the name to be returned will be NULL.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::GetProfileName(
        const TTsyReqHandle aTsyReqHandle, 
        TInfoName* const aQoSProfile )  
    {
    if ( NULL != iQoSProfile )
        {
        *aQoSProfile = iQoSProfileName;
        }
    else
        {
        // QoS profile not found
        aQoSProfile->Zero();
        }

OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_GETPROFILENAME_1,  "TSY: CMmPacketContextTsy::GetProfileName. Context name: %S QoS Profile Name: %S",iContextName, *aQoSProfile );

    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::GetStatus
// Allows a client to get the current status of the context.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::GetStatus(
        const TTsyReqHandle aTsyReqHandle,   
        RPacketContext::TContextStatus* const aContextStatus )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_GETSTATUS_1,  "TSY: CMmPacketContextTsy::GetStatus. Context name: %S Context Status:%d",iContextName, iContextStatus );
    *aContextStatus = iContextStatus;
    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::NotifyConfigChanged
// Requests notification when configuration of context changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::NotifyConfigChanged(
        TPacketDataConfigBase* const aConfig )  
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_NOTIFYCONFIGCHANGED_1,  "TSY: CMmPacketContextTsy::NotifyConfigChanged." );

    // Call NotifyConfigChanged from extension
    TInt ret = iMmPacketContextGsmWcdmaExt->NotifyConfigChanged( *aConfig );

    if ( KErrNone == ret )
        {
        if ( TPacketDataConfigBase::KConfigGPRS == aConfig->ExtensionId() )
            {
            // Store pointer
            iRetNotifyConfig = aConfig;
            // Store req handle type
            iReqHandleType = EMultimodePacketContextNotifyConfigChanged;
            }

        else
            {
            // Store pointer
            iRetNotifyConfig2 = aConfig;
            // Store req handle type
            iReqHandleType = EMultimodePacketContextNotifyConfigChanged99;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteNotifyConfigChanged
// Complete notification when the context configuration changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteNotifyConfigChanged()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETENOTIFYCONFIGCHANGED_1,  "TSY: CMmPacketContextTsy::CompleteNotifyConfigChanged.  Context name:%S",iContextName );

    // Get and reset req handle for R97/98 config notify
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketContextNotifyConfigChanged );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
        // Get current configuration
        iMmPacketContextGsmWcdmaExt->GetConfig( iRetNotifyConfig );
        // Null ret pointer
        iRetNotifyConfig = NULL;
        // Complete the client request
        CMmPacketContextTsy::ReqCompleted( reqHandle, KErrNone );
        }
    
    // Get and reset req handle for R99_R4 config notify
    reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketContextNotifyConfigChanged99 );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
        // Get current configuration
        iMmPacketContextGsmWcdmaExt->GetConfig( iRetNotifyConfig2 );
        // Null ret pointer
        iRetNotifyConfig2 = NULL;
        // Complete the client request
       CMmPacketContextTsy::ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::NotifyConnectionSpeedChange
// Requests notification when connection speed changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::NotifyConnectionSpeedChange(
        TUint* const aRate )    
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_NOTIFYCONNECTIONSPEEDCHANGE_1,  "TSY: CMmPacketContextTsy::NotifyConnectionSpeedChange." );

    // Store pointer
    iRetNotifyConnectionSpeed = aRate;
    // Store req handle type
    iReqHandleType = EMultimodePacketContextNotifyConnectionSpeedChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteNotifyConnectionSpeedChange
// Complete the notification when the connection speed changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteNotifyConnectionSpeedChange(
        TInt const aConnectionSpeed )
    {
    iConnectionSpeed = aConnectionSpeed;
    
    if ( iNotifyConnectionSpeed != iConnectionSpeed )
        {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETENOTIFYCONNECTIONSPEEDCHANGE_1,  "TSY: CMmPacketContextTsy::CompleteNotifyConnectionSpeedChange. Context name:%S Connection Speed:%u bps",iContextName, iConnectionSpeed );

        // Store the connection speed internally
        iNotifyConnectionSpeed = iConnectionSpeed;

        // Get and reset req handle
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePacketContextNotifyConnectionSpeedChange );

        if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
            {
            // Get current connection speed
            *iRetNotifyConnectionSpeed = iNotifyConnectionSpeed;
            // Null ret pointer
            iRetNotifyConnectionSpeed = NULL;
            // Complete the client request
            CMmPacketContextTsy::ReqCompleted( reqHandle, KErrNone );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::NotifyStatusChange
// Requests notification when the status of the context changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::NotifyStatusChange(
        RPacketContext::TContextStatus* const aContextStatus )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_NOTIFYSTATUSCHANGE_1,  "TSY: CMmPacketContextTsy::NotifyStatusChange. Context name:%S", iContextName );
    iRetNotifyStatus = aContextStatus;
    iReqHandleType = EMultimodePacketContextNotifyStatusChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteNotifyStatusChange
// Complete the notification when the context status changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteNotifyStatusChange(
        const RPacketContext::TContextStatus aContextStatus ) 
    {

    if ( aContextStatus != iContextStatus )
        {
        iContextStatus = aContextStatus;
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETENOTIFYSTATUSCHANGE_1,  "TSY: CMmPacketContextTsy::CompleteNotifyStatusChange. Context name: %S  Context status:%d",iContextName, iContextStatus );

        if ( RPacketContext::EStatusActive == iContextStatus ||
             RPacketContext::EStatusInactive == iContextStatus ||
             RPacketContext::EStatusDeleted == iContextStatus )
            {
            iMmPacketService->ContextStatusChanged( iContextStatus );
            if( RPacketContext::EStatusDeleted == iContextStatus )
                {
                iMmPacketContextGsmWcdmaExt->SetContextStatusBeforeDeactivation(
                    RPacketContext::EStatusDeleted );
                }
            }

        // Reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
            EMultimodePacketContextNotifyStatusChange );

        if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
            {
            // Get current status
            *iRetNotifyStatus = aContextStatus;
            // Null ret pointer
            iRetNotifyStatus = NULL;
            // Complete the client request
            CMmPacketContextTsy::ReqCompleted( reqHandle, KErrNone );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::SetConfigL
// Configure a context to the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::SetConfigL(
        TPacketDataConfigBase* const aConfig ) 
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_SETCONFIGL_1,  "TSY: CMmPacketContextTsy::SetConfigL. Context name:%S", iContextName );

    // Call SetConfigL
    TInt ret = iMmPacketContextGsmWcdmaExt->SetConfigL( aConfig );

    if ( KErrNone == ret )
        {
        // Store the request handle type
        iReqHandleType = EMultimodePacketContextSetConfig;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteSetConfig
// Complete the asynchronous SetConfig request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteSetConfig( 
        const TInt aError,   
        TBool aIsAddMediaAuthorizationCalled )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETESETCONFIG_1,  "TSY: CMmPacketContextTsy::CompleteSetConfig. Error: %{TSymbianErrorCodes}", aError );
    
    TTsyReqHandle reqHandle( EMultimodePacketContextReqHandleUnknown );
    //if context exists eg. its name length is bigger than zero
    if( 0 == CMmPacketContextTsy::HostCID().Length() )
        {
        reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePacketContextSetConfig );
        }

    else
        {
        if( aIsAddMediaAuthorizationCalled )
        	{
        	reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            				EMultimodePacketContextAddMediaAuthorization );	
        	}
		else
			{       
#ifdef USING_CTSY_DISPATCHER
			// Distinguish between RemovePacketFilter and AddPacketFilter..
			
// There is a hang defect lurking here.
//
//	To show the defect, add and run the following test case (you could
//         copy from CCTsyPacketServicesFU::TestUseCase0007L):
//	- Add a packet filter. Wait for the request to complete.
//  - Remove the packet filter. (don't wait for completion)
//  - Add another packet filter.
//  - Wait for removal to complete (WILL HANG but obviously shouldn't).
//
// The reason for this hang is that there's a single completion method
// for both Add and Remove packet filter functions down in the CTSY
// (dispatcher mode only).
//
// If a client calls RemovePacketFilter, then immediately
// calls AddPacketFilter (before the RemovePacketFilter has
// completed), the iReqHandleType data member will already have been
// updated to be EMultimodePacketContextAddPacketFilter..
// So the code under the else below (that searches for the
// RemovePacketFilter reqHandle and completes it) will never get run.
// Thus the RemovePacketFilter call will hang.
//
// Fixing this requires that some other variable (NOT iReqHandleType)
//  is used to record that an add or a remove action is outstanding.
//
// When this defect has been addressed, please treat all the sections marked
//  ADD_REMOVE_PACKETFILTER_DEFECT_FIXED above as defined, and delete any 
//  else blocks. This will then assert that the iReqHandleType doesn't
//  leak between calls. Which will protect us against any other such hang
//  bugs in the future.
// This is the change that I was trying to apply when I ran into this defect.
//  See Perforce CL#1755436
//     Rob Lundie Hill

			if (iReqHandleType == EMultimodePacketContextRemovePacketFilter)
				{
				reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePacketContextRemovePacketFilter );
				}
			else
				{
				reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePacketContextAddPacketFilter );
				}
#else			
        	reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            				EMultimodePacketContextAddPacketFilter );
#endif //USING_CTSY_DISPATCHER																       
        	}
        }
        
    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
                            
        // Complete the client request
        CMmPacketContextTsy::ReqCompleted( reqHandle, aError );
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::SetDataVolume
// Set the volume of transferred data.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::SetDataVolume(
        const TInt /* aResult */,
        RPacketContext::TDataVolume dataVolume )
    {

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_SETDATAVOLUME_1,  "TSY: CMmPacketContextTsy::SetDataVolume" );
   
    if ( dataVolume.iBytesSent < iDataVolume.iBytesSent )
        {
        // The value of sent data has turned around the 4 GB
        iDataVolume.iOverflowCounterSent++;
        }
    iDataVolume.iBytesSent = dataVolume.iBytesSent;


    if ( dataVolume.iBytesReceived < iDataVolume.iBytesReceived )
        {
        // The value of received data has turned around the 4 GB
        iDataVolume.iOverflowCounterReceived++;
        }
    iDataVolume.iBytesReceived = dataVolume.iBytesReceived;
       
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::SetLastErrorCause
// Set the last error cause.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::SetLastErrorCause(
        const TInt aErrorCause ) 
    {
    if ( RPacketContext::EStatusActivating == iContextStatus  || 
     	RPacketContext::EStatusDeactivating == iContextStatus   )
        {
        iLastErrorCause =  aErrorCause;           
        }

OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_SETLASTERRORCAUSE_1,  "TSY: CMmPacketContextTsy::SetLastErrorCause.  aErrorCause: %{TSymbianErrorCodes} iLastErrorCause:%{TSymbianErrorCodes}", aErrorCause, iLastErrorCause );

    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ContextStatus
// Return current context status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RPacketContext::TContextStatus CMmPacketContextTsy::ContextStatus() const
    {
    return iContextStatus;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ContextSuspended
// Suspend the context.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::ContextSuspended()
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_CONTEXTSUSPENDED_1,  "TSY: CMmPacketContextTsy::ContextSuspended. Context status before suspending: %d",iContextStatus );

    if ( ( RPacketContext::EStatusActivating == iContextStatus ) ||
         ( RPacketContext::EStatusActive == iContextStatus ) ||
         ( RPacketContext::EStatusDeactivating == iContextStatus ) )
        {
        iContextStatusBeforeSuspending = iContextStatus;
        CMmPacketContextTsy::CompleteNotifyStatusChange( 
									RPacketContext::EStatusSuspended );
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ContextResumed
// Resume the suspended context.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::ContextResumed()
    {
    if ( RPacketContext::EStatusSuspended == iContextStatus )
        {
        CMmPacketContextTsy::CompleteNotifyStatusChange( 
									iContextStatusBeforeSuspending );
        }

    iContextStatusBeforeSuspending = RPacketContext::EStatusUnknown;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_CONTEXTRESUMED_1,  "TSY: CMmPacketContextTsy::ContextResumed. Context status when resumed: %d",iContextStatus );

    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::PacketContextGsmWcdmaExt
// Return pointer to the CMmPacketContextGsmWcdmaExt.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPacketContextGsmWcdmaExt* CMmPacketContextTsy::PacketContextGsmWcdmaExt() const 
    {
    return iMmPacketContextGsmWcdmaExt;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::PacketQoSTsy
// Return a pointer to the PacketQoSTsy.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPacketQoSTsy* CMmPacketContextTsy::PacketQoSTsy() const 
    {
    return iQoSProfile;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::RemoveQoS
// Remove the pointer to PacketQoSTsy.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::RemoveQoS()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_REMOVEQOS_1,  "TSY: CMmPacketContextTsy::RemoveQoS." );

    iQoSProfile = NULL;
    iQoSProfileName.Zero();
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ReqCompleted
// Overloads CTelObject::ReqCompleted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::ReqCompleted(
        const TTsyReqHandle aTsyReqHandle, 
        const TInt aError )   
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_REQCOMPLETED_1, "TSY: CMmPacketContextTsy::ReqCompleted. Error:%{TSymbianErrorCodes} Handle:%d Object:0x%08x", (TInt)aError, (TUint)aTsyReqHandle, (TUint)this);

    // Set last error cause
    if ( KErrNone != aError )
        {
        iLastErrorCause = aError;
        }

    CTelObject::ReqCompleted( aTsyReqHandle, aError );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_REQCOMPLETED_2, "<-- TSY: CMmPacketContextTsy::ReqCompleted");
    }

#ifdef REQHANDLE_TIMER

    
// ---------------------------------------------------------------------------
// CMmPacketContextTsy::SetTypeOfResponse
// Sets the type of response for a given request handle. Automatic mode 
// includes an automatic response in case of non response from the DOS in a
// specified time. This method is used only if REQHANDLE_TIMER is in use.
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::SetTypeOfResponse(
        const TInt aReqHandleType,               
        const TTsyReqHandle aTsyReqHandle )       
    {
    TInt timeOut( 0 );

    // Set timeout by req handle type
    switch ( aReqHandleType )
        {
        case EMultimodePacketContextInitialiseContext:
            timeOut = KMmPacketContextInitialiseContextTimeOut;
            break;
        case EMultimodePacketContextSetConfig:
            timeOut = KMmPacketContextSetConfigTimeOut;
            break;
        case EMultimodePacketContextActivate:
            timeOut = KMmPacketContextActivateTimeOut;
            break;
        case EMultimodePacketContextDeactivate:
            timeOut = KMmPacketContextDeactivateTimeOut;
            break;
        case EMultimodePacketContextDelete:
            timeOut = KMmPacketContextDeleteTimeOut;
            break;
        case EMultimodePacketContextGetDataVolumeTransferred:
            timeOut = KMmPacketContextGetDataVolumeTransferredTimeOut;
            break;
        case EMultimodePacketContextModifyActiveContext:
            timeOut = KMmPacketContextModifyActiveContext;
            break;
        case EMultimodePacketContextAddMediaAuthorization:
            timeOut = KMmPacketContextAddMediaAuthorization;
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
// CMmPacketContextTsy::Complete
// Completes the request due the timer expiration. This method is used only if
// REQHANDLE_TIMER is in use. 
// ---------------------------------------------------------------------------
// 
void CMmPacketContextTsy::Complete(
        const TInt aReqHandleType,    
        const TInt aError )        
    {
    // All possible TSY req handle types are listed in the
    // switch case below.
    switch ( aReqHandleType )
        {
        case EMultimodePacketContextInitialiseContext:
            CMmPacketContextTsy::CompleteInitialiseContext( aError );
            break;
        case EMultimodePacketContextSetConfig:
            CMmPacketContextTsy::CompleteSetConfig( aError );
            break;
        case EMultimodePacketContextActivate:
            CMmPacketContextTsy::CompleteActivate( aError );
            break;
        case EMultimodePacketContextDeactivate:
            CMmPacketContextTsy::CompleteDeactivate( aError );
            break;
        case EMultimodePacketContextDelete:
            CMmPacketContextTsy::CompleteDelete( aError );
            break;
        case EMultimodePacketContextGetDataVolumeTransferred:
            CMmPacketContextTsy::CompleteGetDataVolumeTransferred( aError );
            break;
        case EMultimodePacketContextModifyActiveContext:
            CMmPacketContextTsy::CompleteModifyActiveContext( aError );
            break;
       	case EMultimodePacketContextAddMediaAuthorization:
            CMmPacketContextTsy::CompleteSetConfig( aError, ETrue );
            break;

        // Can't use timer:
        // - all notifications
        //case EMultimodePacketContextNotifyConfigChanged:
        //case EMultimodePacketContextNotifyConnectionSpeedChange:
        //case EMultimodePacketContextNotifyStatusChange:
        default:
            CMmPacketContextTsy::ReqCompleted( iTsyReqHandleStore->
				ResetTsyReqHandle( aReqHandleType ), aError );
            break;
        }

    }

#endif // REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::IsDialUpContext
// Return ETrue if this is dial-up context, otherwise EFalse.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//  
TBool CMmPacketContextTsy::IsDialUpContext(  
        const TInfoName* const aContextName ) const
    {

    TBool ret( EFalse );

    if ( NULL != aContextName )
        {
        if ( *aContextName == iContextName )
            {
            ret = ETrue;
            }
        }
    else
        {
        ret = iIsDialUpContext;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ResetDialUpContext
// Reset the externally created dial-up context's data.
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::ResetDialUpContext()
    {

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_RESETDIALUPCONTEXT_1,  "TSY: CMmPacketContextTsy::ResetDialUpContext." );

    iDataVolume.iOverflowCounterSent = 0;
    iDataVolume.iBytesSent = 0;
    iDataVolume.iOverflowCounterReceived = 0;
    iDataVolume.iBytesReceived = 0;

    iContextStatus = RPacketContext::EStatusInactive;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::HostCID
// Return the host context name
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInfoName CMmPacketContextTsy::HostCID() const
    {

    return iHostCID;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::AddPacketFilterL
// This method sets up a packet filter in the TFT belonging to this context, 
// with the Id specified in the packet filter information. New packet filter
// info is stored in CMmPacketContextGsmWcdmaExt. This method takes care of 
// adding packet filter info and storing this information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::AddPacketFilterL(
        const TTsyReqHandle aTsyReqHandle,  
        TDes8* const aPacketFilter )  
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_ADDPACKETFILTERL_1,  "TSY: CMmPacketContextTsy::AddPacketFilterL. Context name:%S", iContextName );

    TInt ret( KErrGeneral );

    if ( (0 < iHostCID.Length()) && (aPacketFilter->Length() > 0) )
        {
        ret = iMmPacketContextGsmWcdmaExt->AddPacketFilterL( aPacketFilter );

        if ( KErrNone == ret )
            {
            // if operation is meant for already configurated context
            if ( RPacketContext::EStatusUnknown != ContextStatus() &&
                RPacketContext::EStatusInactive != ContextStatus() ) 
                {
                // request must complete to client
                CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, KErrNone );
                }

            // else operation is context configuration and complete method
            // handles request complete
            else
                {
                // Store the request handle type
                iReqHandleType = EMultimodePacketContextAddPacketFilter;
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::RemovePacketFilter
// This method removes a packet filter from the TFT belonging to this context.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
#ifndef USING_CTSY_DISPATCHER 
TInt CMmPacketContextTsy::RemovePacketFilter(
        const TTsyReqHandle aTsyReqHandle,  
        TInt *aID ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_REMOVEPACKETFILTER_1,  "TSY: CMmPacketContextTsy::RemovePacketFilter. RemovedFilter:%d", *aID );

    TInt ret( KErrArgument );

    // id must be value from 1 to 8
    if ( 0 < *aID && 8 >= *aID )
        {
        ret = iMmPacketContextGsmWcdmaExt->RemovePacketFilter( *aID );
        }

    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }
#endif // #ifndef USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ModifyActiveContextL
// Invokes context modification.  This method invokes MS-initiated 
// modification of an active context in the network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::ModifyActiveContextL()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_MODIFYACTIVECONTEXTL_1,  "TSY: CMmPacketContextTsy::ModifyActiveContextL. Context name:%S", iContextName );
    
    TInt ret( KErrNotReady );

    if ( RPacketContext::EStatusUnknown != ContextStatus() &&
        RPacketContext::EStatusInactive != ContextStatus() &&
        RPacketContext::EStatusDeleted != ContextStatus() )
        {
        ret = iMmPacketContextGsmWcdmaExt->ModifyActiveContextL();
        }

    if ( KErrNone == ret )
        {
        // Store the request handle type
        iReqHandleType = EMultimodePacketContextModifyActiveContext;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteModifyActiveContext
// When all Tft operations are done method completes to client 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteModifyActiveContext(
        const TInt aResult )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETEMODIFYACTIVECONTEXT_1,  "TSY: CMmPacketContextTsy::CompleteModifyActiveContext. Context name:%S", iContextName );

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
           EMultimodePacketContextModifyActiveContext );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {

        CMmPacketContextTsy::ReqCompleted( reqHandle, aResult );

        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::EnumeratePacketFilters
// Returns number of defined packet filters to client
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::EnumeratePacketFilters(
        const TTsyReqHandle aTsyReqHandle,
        TInt *aCount )  
    {
    TInt ret = iMmPacketContextGsmWcdmaExt->EnumeratePacketFilters ( aCount );

    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::GetPacketFilterInfo
// Gives information about the packet filter specified by aIndex
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::GetPacketFilterInfo(
        const TTsyReqHandle aTsyReqHandle,   
        TInt* aIndex,               
        TPacketBase* aPacketFilterInfo )   
    {
    TInt ret = iMmPacketContextGsmWcdmaExt->GetPacketFilterInfo ( *aIndex, 
        aPacketFilterInfo);

    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::GetDnsInfo
// This method gets the DNS info for Etel side request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::GetDnsInfo(
        const TTsyReqHandle aTsyReqHandle,
        TDes8* const aDnsInfo )
    {
    ASSERT(aDnsInfo);
    
    RPacketContext::TDnsInfoV2 temp;
    RPacketContext::TTDnsInfoV2Pckg tempPckg(temp);

	TInt ret = KErrArgument;
	if (tempPckg.MaxLength() == aDnsInfo->MaxLength() )
		{
	    ret = iMmPacketContextGsmWcdmaExt->GetDnsInfoEtel( aDnsInfo );
		}

    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, ret ); 

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ContextName
// This method returns name of this context
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInfoName CMmPacketContextTsy::ContextName() const
    {
    return iContextName;
    }


// ---------------------------------------------------------------------------
// CMmPacketContextTsy::AddMediaAuthorizationL
// Makes request for adding authorization params to licensee tsy
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::AddMediaAuthorizationL(
        const TTsyReqHandle aTsyReqHandle,  
        TDes8* aMediaAuthorization )
	{
	
	TInt ret( KErrArgument );
	TBool hasMediaAuthorizationBeenCached=EFalse;
	
	
	//Can be done only for secondary context
	if ( 0 < iHostCID.Length() )
		{
				
	    //Create buffers
	    RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization = 
            RPacketContext::CTFTMediaAuthorizationV3::NewL();
	    CleanupStack::PushL( mediaAuthorization );
	    mediaAuthorization->InternalizeL( *aMediaAuthorization );
	 	
	 	ret = iMmPacketContextGsmWcdmaExt->AddMediaAuthorizationL( 
            mediaAuthorization, hasMediaAuthorizationBeenCached);
	 	
	 	//Cleanup created object.  If it has been cached we do not delete it, because
	 	// the array (cache) will be cleaned in the destructor
	 	// If we were unable to cache it, we need to destroy it, otherwise we will leak 
	 	// memory.
	 	if(hasMediaAuthorizationBeenCached)
	 		{
	 		CleanupStack::Pop(mediaAuthorization);
	 		}
	 	else
	 		{
	 		CleanupStack::PopAndDestroy(mediaAuthorization);
	 		}
	 	
	 	if ( KErrNone == ret )
	    	{
	        //
	        if ( RPacketContext::EStatusUnknown != ContextStatus() &&
	            RPacketContext::EStatusInactive != ContextStatus() ) 
	            {
	            // request must complete to client
	            CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, KErrNone );
	            }

	        // else operation is context configuration and complete method
	        // handles request complete
	        else
	            {
	            // Store the request handle type
	            iReqHandleType = EMultimodePacketContextAddMediaAuthorization;
	            }
	        }
		}
	return ret;
	 	        
 	}
        
// ---------------------------------------------------------------------------
// CMmPacketContextTsy::RemoveMediaAuthorization
// Makes request for removing authorization params to licensee tsy
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::RemoveMediaAuthorization(
        const TTsyReqHandle aTsyReqHandle,  
        RPacketContext::TAuthorizationToken* const aAuthorizationToken )
	{
	TInt ret( KErrGeneral );
	
	//Can be done only for secondary context
	if ( 0 < iHostCID.Length() )
		{
	 	
	 	ret = iMmPacketContextGsmWcdmaExt->RemoveMediaAuthorization( 
            aAuthorizationToken );
            
		}
		
	CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, ret );
	
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CMmPacketContextTsy::ContextType
// return the context type
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPacketContextTsy::TPacketContextType CMmPacketContextTsy::ContextType() const
{
	return EContextTypePacketContext;
}

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
// ---------------------------------------------------------------------------
// CMmPacketContextTsy::GetConnectionInfo
// Gets the current connection information. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::GetConnectionInfo(
        const TTsyReqHandle aTsyReqHandle,    
        TConnectionInfoBase* const aInfo )  
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_GETCONNECTIONINFO_1,  "TSY: CMmPacketContextTsy::GetConnectionInfo. Context name:%S", iContextName );

    // Get config
    TInt ret = FillConnectionInfo( aInfo );

    // Complete Request
    CMmPacketContextTsy::ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }
   
// ---------------------------------------------------------------------------
// CMmPacketContextTsy::NotifyConnectionInfoChange
// Requests notification when connection information changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::NotifyConnectionInfoChange(
        TConnectionInfoBase* const aInfo )    
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_NOTIFYCONNECTIONINFOCHANGE_1,  "TSY: CMmPacketContextTsy::NotifyConnectionInfoChange." );

    // Store pointer
    iRetNotifyConnectionInfo = aInfo;
    // Store req handle type
    iReqHandleType = EMultimodePacketContextNotifyConnectionInfoChange;

    return KErrNone;
    }   
    
// ---------------------------------------------------------------------------
// CMmPacketContextTsy::CompleteNotifyConnectionInfoChange
// Complete notification when the connection information changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextTsy::CompleteNotifyConnectionInfoChange(
        TConnectionInfoBase* const aInfo )    
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_COMPLETENOTIFYCONNECTIONINFOCHANGE_1,  "TSY: CMmPacketContextTsy::CompleteNotifyConnectionInfoChange." );

    if ( NULL != aInfo &&
        TConnectionInfoBase::KConnectionInfoV1 == aInfo->ExtensionId() )
        {
    	RPacketContext::TConnectionInfoV1 connectionInfoV1;
        connectionInfoV1 = *( static_cast<RPacketContext::TConnectionInfoV1*>( aInfo ) );

		//Check if connection info has changed
		if ( iConnectionInfo.iHSDPACategory != connectionInfoV1.iHSDPACategory ||
	         iConnectionInfo.iHSUPACategory != connectionInfoV1.iHSUPACategory ||
	         iConnectionInfo.iValid != connectionInfoV1.iValid )
			{
			//Set new connection info
			iConnectionInfo = connectionInfoV1;
			
			//Get req handle
			TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        		EMultimodePacketContextNotifyConnectionInfoChange );
        		
			if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        		{	
        		// Fill current connection information
        		TInt ret = FillConnectionInfo( iRetNotifyConnectionInfo );
			
				if ( KErrNone == ret )
					{
					//reset req handle
					iTsyReqHandleStore->ResetTsyReqHandle(
        				EMultimodePacketContextNotifyConnectionInfoChange );
        			// Null ret pointer
        			iRetNotifyConnectionInfo = NULL;
        			// Complete the client request
        			CMmPacketContextTsy::ReqCompleted( reqHandle, KErrNone );
					}
        		}
			}
        }
    }    
    
// ---------------------------------------------------------------------------
// CMmPacketContextTsy::FillConnectionInfo
// Fills the current connection information to TConnectionInfoXXX structure. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextTsy::FillConnectionInfo(   
        TConnectionInfoBase* const aInfo )  
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTTSY_FILLCONNECTIONINFO_1,  "TSY: CMmPacketContextTsy::FillConnectionInfo" );

    TInt ret = KErrNone;

    if ( NULL != aInfo &&
        TConnectionInfoBase::KConnectionInfoV1 == aInfo->ExtensionId() )
        {
        //Set connection info to ETel side memory
        *( static_cast<RPacketContext::TConnectionInfoV1*>( aInfo ) ) = iConnectionInfo;        
        }
    else
        {
        ret = KErrArgument;    
        }

    return ret;
    }        
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
//  End of File 
