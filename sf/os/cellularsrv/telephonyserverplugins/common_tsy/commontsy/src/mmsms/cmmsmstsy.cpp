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
#include "cmmsmstsyTraces.h"
#endif

#include "cmmsmstsy.h"
#include "cmmphonetsy.h"
#include "cmmsmsstoragetsy.h"
#include "cmmsmsgsmext.h"
#include "cmmtsyreqhandlestore.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "MmTsy_numberOfSlots.h"
#include <ctsy/serviceapi/cmmsmsutility.h>
#include "cmmsmssendrequest.h"
#include <ctsy/serviceapi/mstktsysatservice.h>  
#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <etelsat.h>  // for Sat MO SM Control error values


// ======== MEMBER FUNCTIONS ========

CMmSmsTsy::CMmSmsTsy():
    iReqHandleType(EMultimodeSmsReqHandleUnknown)
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_CTOR_1, "TSY: CMmSmsTsy::CMmSmsTsy: constructor");
    }

void CMmSmsTsy::ConstructL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_CONSTRUCTL_1, "TSY: CMmSmsTsy::ConstructL");
#ifdef REQHANDLE_TIMER   
    //create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this, iMmPhone, 
        EMultimodeSmsMaxNumOfRequests, iSmsReqHandles );
#else
    // create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        EMultimodeSmsMaxNumOfRequests, iSmsReqHandles );
#endif // REQHANDLE_TIMER   

    // Reset all CMmSmsTsy variables
    ResetVariables();

    // SMS message array
    iSmsMsgArray = new ( ELeave ) CArrayPtrFlat<TSmsMsg>( 1 );

    // Create GSM mode SMS extension
    iMmSmsExtInterface = CMmSmsGsmExt::NewL();


    // register sms tsy in the message manager
    iMmPhone->MessageManager()->RegisterTsyObject( 
        CMmMessageManagerBase::ESmsMessagingTsy, this );

    // at startup, there is no pending Send request
    iSmsSendReq = NULL;

    // Initialise off-line/on-line mode variable with information
    // from PhoneTsy. After object construction, this information
    // will be updated using DOS indications.
    TRfStateInfo phoneRfInfo = iMmPhone->GetRfStateInfo();
    if ( ERfsStateInfoInactive == phoneRfInfo )
        {
        iIsOffline = ETrue;
        }
    else
        {
        iIsOffline = EFalse;
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_CONSTRUCTL_2, "TSY: CMmSmsTsy::ConstructL: iIsOffline has been initialised to %d", iIsOffline);

    // there's no pending ResumeSmsReception request at startup
    iResumeSmsReceptionPending = EFalse;
  
    // Initialization for sms NoFdnCheck
    iSmsNoFdnCheckFlag = ESmsNoFdnCheckUnknown;
    
    iExpectAckOrNack = 0;
    
    }

CMmSmsTsy* CMmSmsTsy::NewL(
    CMmPhoneTsy* aMmPhone )
    {
    CMmSmsTsy* aMmSmsTsy = new ( ELeave ) CMmSmsTsy();
    CleanupClosePushL(*aMmSmsTsy);
    aMmSmsTsy->iMmPhone = aMmPhone;
    aMmSmsTsy->ConstructL();
    CleanupStack::Pop();

    return aMmSmsTsy;
    }

CMmSmsTsy::~CMmSmsTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_DTOR_1, "TSY: CMmSmsTsy::~CMmSmsTsy");
    if ( iMmPhone )
        {
        // deregister tsy object from message manager
        iMmPhone->MessageManager()->DeregisterTsyObject(this);

        // Release the SMS routing from DOS to TSY.
        // Send request to the Domestic OS layer. Ignore if it leaves
        TInt trapError = KErrNone;
        TRAP( trapError, iMmPhone->MessageManager()->HandleRequestL( 
                EMmTsyDeactivateSmsRouting ); );
        }

    // try to complete send sms message.
    if ( iTsySatMessaging )
        {
        iTsySatMessaging->CompleteSendSmsMessage( KErrAccessDenied );
        }

    if ( iTsyReqHandleStore )
        {
        delete iTsyReqHandleStore;
        iTsyReqHandleStore = NULL;
        }

    if ( iSmsMsgArray )
        {
        iSmsMsgArray->ResetAndDestroy();
        delete iSmsMsgArray;
        iSmsMsgArray = NULL;
        }

    // Delete extension object
    if ( iMmSmsExtInterface )
        {
        delete iMmSmsExtInterface;
        iMmSmsExtInterface = NULL;
        }

    if ( iMmPhone )
        {
        iMmPhone->SetSmsSession( NULL );
        iMmPhone = NULL;
        }

    if ( iSmsSendReq )
        {
        delete iSmsSendReq;
        iSmsSendReq = NULL;
        }
    
    if ( iSMSPClientId )
        {
        delete iSMSPClientId;
        iSMSPClientId = NULL;
        }

	// set pointers to NULL
	iMmSmsStorageTsy = NULL;
	iTsySatMessaging = NULL;
	iNotifyReceiveModeChangePtr = NULL;
	iReceiveMessageParamsPtr = NULL;
	iReceiveMessagePduPtr = NULL;
	iNotifySmsBearerPtr = NULL;
	iSendMessageMsgAttrPckgPtr = NULL;
	iGetMessageStoreInfoPtr = NULL;
	iStoreSmspBufferPtr = NULL;
    }

// --------------------------------------------------------------------------- 
// CMmSmsTsy::Init 
// Initialisation method that is called from ETel Server
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::Init()
    {
    }

// --------------------------------------------------------------------------- 
// CMmSmsTsy::OpenNewObjectByNameL 
// Creates new object and returns a pointer to it
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
CTelObject* CMmSmsTsy::OpenNewObjectByNameL( 
    const TDesC& aName )
    {
    if ( aName.Compare( KETelIccSmsStore ) == KErrNone )
        {
        iMmSmsStorageTsy = CMmSmsStorageTsy::NewL( this, iMmPhone, 
            iMmSmsExtInterface, iTsyReqHandleStore );
        
        return iMmSmsStorageTsy;
        }

    // ME memory (KETelMeSmsStore) and combined memory (KETelCombinedSmsStore) 
    // are not supported
    User::Leave( KErrNotSupported );
    //lint -e{527} "unreachable code"
    
    return NULL;
    }

// --------------------------------------------------------------------------- 
// CMmSmsTsy::OpenNewObjectL 
// Creates new object and returns a pointer to it
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
CTelObject* CMmSmsTsy::OpenNewObjectL( 
    TDes& /*aNewName*/ )   
    {
    // Not supported
    User::Leave( KErrNotSupported );
    //lint -e{527} "unreachable code"
    
    return NULL;
    }

// --------------------------------------------------------------------------- 
// CMmSmsTsy::ExtFunc 
// ExtFunc is called by the server when it has a "extended", 
// i.e. non-core ETel request for the TSY. To process a request handle, 
// request type and request data are passed to the TSY
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::ExtFunc(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
    TAny* dataPtr = aPackage.Ptr1();
    TInt ret( KErrNone );

    // before processing further the request, check if offline mode status
    // is enabled and if the given request can be perfomed in that case.
    if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() &&
        !IsRequestPossibleInOffline( aIpc ) )  
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_EXTFUNC_1, "TSY: Offline mode ON, request is not allowed: %{TIPCNamesList}", aIpc );
        TInt error = CMmCommonStaticUtility::EpocErrorCode( KErrGeneral, 
                KErrGsmOfflineOpNotAllowed );

        // Complete the request with appropiate error
        ReqCompleted ( aTsyReqHandle, error ); 
        }
    else
        {
        switch ( aIpc )
            {
            // SMS messaging requests that doesn't need trapping
            case EMobileSmsMessagingGetCaps:
                ret = GetCaps( aTsyReqHandle, aPackage.Des1n() );
                break;
            case EMobileSmsMessagingGetReceiveMode:
                ret = GetReceiveMode( aTsyReqHandle, reinterpret_cast< 
					RMobileSmsMessaging::TMobileSmsReceiveMode* >( dataPtr ) );
                break;
            case EMobileSmsMessagingGetMoSmsBearer:
                ret = GetMoSmsBearer( aTsyReqHandle, reinterpret_cast< 
                    RMobileSmsMessaging::TMobileSmsBearer* >( dataPtr) );
                break;
            case EMobileSmsMessagingSetMoSmsBearer:
                ret = SetMoSmsBearer( aTsyReqHandle, reinterpret_cast< 
                    RMobileSmsMessaging::TMobileSmsBearer*>( dataPtr) );
                break;
            case EMobileSmsMessagingEnumerateMessageStores:
                ret = EnumerateMessageStores( aTsyReqHandle, 
                    reinterpret_cast< TInt* >( dataPtr ) );
                break;
            case EMobileSmsMessagingGetSmspListPhase2:
                ret = ReadSmspListPhase2( aTsyReqHandle, reinterpret_cast< 
                    RMobilePhone::TClientId*>( dataPtr ), aPackage.Des2n() );
                break;
            // SMS messaging requests that need trapping
            default:
                // Ensure the ReqHandleType is unset.
                // This will detect cases where this method indirectly calls itself
                // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
                // Such cases are not supported because iReqHandleType is in the context of this class instance,
                // not this request, and we don't want the values set by the inner request and the outer request
                // interfering with each other.
                __ASSERT_DEBUG(iReqHandleType==EMultimodeSmsReqHandleUnknown, User::Invariant());

                TInt leaveCode( KErrNone );
                TRAP( leaveCode, ret = DoExtFuncL( aTsyReqHandle, aIpc, 
                    aPackage ); );

                if ( KErrNone != leaveCode )
                    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_EXTFUNC_2, "CMmSmsTsy: Leave trapped!, IPC=%{TIPCNamesList}, error value:%{TSymbianErrorCodes}", aIpc, leaveCode );
                    //reset request handle to indicate the request is no longer ongoing
					iTsyReqHandleStore->FindAndResetTsyReqHandle( aTsyReqHandle );
                    ReqCompleted( aTsyReqHandle, leaveCode );
                    }

                //save request handle
                else if ( EMultimodeSmsReqHandleUnknown != iReqHandleType )
                    {
#ifdef REQHANDLE_TIMER
                    SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
                    //Never comes here. See SetTypeOfResponse.
                    iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, 
                        aTsyReqHandle );
#endif // REQHANDLE_TIMER
                    }
                // We've finished with this value now. Clear it so it doesn't leak
                //  up to any other instances of this method down the call stack
                iReqHandleType = EMultimodeSmsReqHandleUnknown;
                break;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsTsy::DoExtFuncL 
// DoExtFuncL calls messaging methods that can leave
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_DOEXTFUNCL_1, "TSY: CMmSmsTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);
    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        // SMS messaging requests
        case EMobileSmsMessagingSetReceiveMode:
            ret = SetReceiveMode( aTsyReqHandle, reinterpret_cast<
				RMobileSmsMessaging::TMobileSmsReceiveMode* >( dataPtr ) );
            break;
        case EMobileSmsMessagingNotifyReceiveModeChange:
            ret = NotifyReceiveModeChange( reinterpret_cast< 
                RMobileSmsMessaging::TMobileSmsReceiveMode*>( dataPtr) );
            break;
        case EMobileSmsMessagingReceiveMessage:
            ret = ReceiveMessageL( aTsyReqHandle, aPackage.Des1n(), 
                aPackage.Des2n() );
            break;
        case EMobileSmsMessagingAckSmsStored:
            ret = AckSmsStoredL( aTsyReqHandle, aPackage.Des1n(),
                reinterpret_cast< TBool* >( dataPtr2 ) );
            break;
        case EMobileSmsMessagingNackSmsStored:
            ret = NackSmsStoredL( aTsyReqHandle, aPackage.Des1n(),
                reinterpret_cast< TInt* >( dataPtr2 ) );
            break;
        case EMobileSmsMessagingResumeSmsReception:
            ret = ResumeSmsReceptionL( aTsyReqHandle );
            break;
        case EMobileSmsMessagingNotifyMoSmsBearerChange:
            ret = NotifyMoSmsBearerChange( reinterpret_cast< 
                RMobileSmsMessaging::TMobileSmsBearer*> ( dataPtr ) );
            break;
        case EMobileSmsMessagingSendMessage:
            {
            iSmsNoFdnCheckFlag = ESmsNoFdnCheckNotUsed;
            ret = SendMessageL( aTsyReqHandle, aPackage.Des1n(),
                aPackage.Des2n() );
            }
            break; 
        case EMobileSmsMessagingSendMessageNoFdnCheck:
            {
            iSmsNoFdnCheckFlag = ESmsNoFdnCheckUsed;
            ret = SendMessageL( aTsyReqHandle, aPackage.Des1n(),
                aPackage.Des2n() );
            }
            break;
        case EMobileSmsMessagingGetMessageStoreInfo:
            ret = GetMessageStoreInfoL( aTsyReqHandle,
                reinterpret_cast< TInt* >( dataPtr ), aPackage.Des2n() );
            break;
        case EMobileSmsMessagingGetSmspListPhase1:
            ret = ReadSmspListPhase1L( aTsyReqHandle,
                reinterpret_cast< RMobilePhone::TClientId*>( dataPtr ),
                reinterpret_cast< TInt* >( dataPtr2 ) );
            break;
        case EMobileSmsMessagingNotifySmspListChange:
            ret = NotifySmspListChange();
            break;
        case EMobileSmsMessagingStoreSmspList:
            ret = StoreSmspList( aTsyReqHandle, aPackage.Des1n() );
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_DOEXTFUNCL_2, "TSY: CMmSmsTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsTsy::ReqModeL 
// When the ETel server receives an "extension" client request, 
// it will pass the IPC request number down to the TSY in order to find out 
// what type of request it is
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
CTelObject::TReqMode CMmSmsTsy::ReqModeL( 
    const TInt aIpc )     
    {
    CTelObject::TReqMode ret = 0;
    switch ( aIpc )
        {
        // Non-flow Controlled Services
        case EMobileSmsMessagingGetCaps:
        case EMobileSmsMessagingGetMoSmsBearer:
        case EMobileSmsMessagingSetMoSmsBearer:
        case EMobileSmsMessagingGetReceiveMode:
        case EMobileSmsMessagingEnumerateMessageStores:
        case EMobileSmsMessagingGetSmspListPhase2:
        case EMobileSmsMessagingAckSmsStored:
        case EMobileSmsMessagingNackSmsStored:
        case EMobileSmsMessagingSendMessage:
        case EMobileSmsMessagingSendMessageNoFdnCheck:
        case EMobileSmsMessagingResumeSmsReception:
        case EMobileSmsMessagingGetMessageStoreInfo:
        case EMobileSmsMessagingGetSmspListPhase1:
        case EMobileSmsMessagingStoreSmspList:
        case EMobileSmsMessagingSetReceiveMode:
            break;
        // Immediate Server Repost - but not multiple completion enabled
        case EMobileSmsMessagingReceiveMessage:
            ret = KReqModeRePostImmediately;
            break;
        // Multiple Completion Services with Immediate Server Repost
        case EMobileSmsMessagingNotifyReceiveModeChange:
        case EMobileSmsMessagingNotifyMoSmsBearerChange:
        case EMobileSmsMessagingNotifySmspListChange:
            ret = KReqModeMultipleCompletionEnabled | 
                KReqModeRePostImmediately;
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return ret;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::NumberOfSlotsL 
// When the ETel server discovers that a request is "repost 
// immediately" it will ask the TSY how big a buffer it wants. 
// NumberOfSlotsL Returns number of slots to be used for given requests
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::NumberOfSlotsL( 
    const TInt aIpc )     
    {
    TInt numberOfSlots = 1;
    switch ( aIpc )
        {
        case EMobileSmsMessagingReceiveMessage:
            numberOfSlots = KMmSmsMessagingReceiveMessageSlots;
            break; 
        // It is safer that normal notification trigger value is three
        // => Slotlimits are defined according to the requirements of
        // environment, five for messagereception and three for normal
        // notifications
        case EMobileSmsMessagingNotifyReceiveModeChange:
            numberOfSlots = KMmSmsMessagingNotifyReceiveModeChangeSlots;
            break; 
        case EMobileSmsMessagingNotifyMoSmsBearerChange:
            numberOfSlots = KMmSmsMessagingNotifyMoSmsBearerChangeSlots;
            break; 
        case EMobileSmsMessagingNotifySmspListChange:
            numberOfSlots = KMmSmsMessagingNotifySmspListChangeSlots;
            break; 
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CancelService 
// CancelService is called by the server when it is 
// "cleaning-up" any still outstanding asynchronous requests before closing 
// a client's sub-session. This will happen if a client closes its R-class 
// handle without cancelling outstanding asynchronous requests on   
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::CancelService( 
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobileSmsMessagingSetReceiveMode:
            ret = SetReceiveModeCancel( aTsyReqHandle );
            break;
        case EMobileSmsMessagingNotifyReceiveModeChange:
            ret = NotifyReceiveModeChangeCancel( aTsyReqHandle );
            break;
        case EMobileSmsMessagingSetMoSmsBearer:
            ret = SetMoSmsBearerCancel( aTsyReqHandle );
            break;
        case EMobileSmsMessagingNotifyMoSmsBearerChange:
            ret = NotifyMoSmsBearerChangeCancel( aTsyReqHandle );
            break;
        case EMobileSmsMessagingGetMessageStoreInfo:
            ret = GetMessageStoreInfoCancel( aTsyReqHandle );
            break;
        case EMobileSmsMessagingGetSmspListPhase1:
        case EMobileSmsMessagingGetSmspListPhase2:
            ret = ReadAllSmspCancel( aTsyReqHandle );
            break;
        case EMobileSmsMessagingNotifySmspListChange:
            ret = NotifySmspListChangeCancel( aTsyReqHandle );
            break;
        case EMobileSmsMessagingSendMessage:
        case EMobileSmsMessagingAckSmsStored:
        case EMobileSmsMessagingNackSmsStored:
        case EMobileSmsMessagingResumeSmsReception:
        case EMobileSmsMessagingStoreSmspList:
            break;
        case EMobileSmsMessagingReceiveMessage:
            ret = ReceiveMessageCancel( aTsyReqHandle );
            break;
        case EMobileSmsMessagingSendMessageNoFdnCheck:
            ret = SendMessageNoFdnCheckCancel( aTsyReqHandle );
            break;
        default:
            ret = KErrGeneral; 
            break;
        } 

    return ret;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::RegisterNotification 
// Called when the server recognises that this notification
// is being posted for the first time on this sub-session object.It enables 
// the TSY to "turn on" any regular notification messages that it may 
// receive from DOS 
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::RegisterNotification( 
    const TInt aIpc )
    {
    TInt ret( KErrNotSupported );

    switch ( aIpc )
        {
        case EMobileSmsMessagingReceiveMessage:
        case EMobileSmsMessagingNotifyReceiveModeChange:
        case EMobileSmsMessagingNotifyMoSmsBearerChange:
        case EMobileSmsMessagingNotifySmspListChange:
            ret = KErrNone;
            break;
        default:
            ret = KErrNotSupported;
        }

    return ret;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::DeregisterNotification 
// Called when the server recognises that this notification
// will not be posted again because the last client to have 
// a handle on this sub-session object has just closed the handle. 
// It enables the TSY to "turn off" any regular notification messages that 
// it may receive from the DOS
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::DeregisterNotification( 
    const TInt aIpc )
    {
    TInt ret( KErrNotSupported );

    switch ( aIpc )
        {
        case EMobileSmsMessagingReceiveMessage:
        case EMobileSmsMessagingNotifyReceiveModeChange:
        case EMobileSmsMessagingNotifyMoSmsBearerChange:
        case EMobileSmsMessagingNotifySmspListChange:
            ret = KErrNone;
            break;
        default:
            ret = KErrNotSupported;
        }

    return ret;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::GetCaps 
// This method returns SMS messaging capabilities of the phone
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::GetCaps( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aCaps )
    {
    if(sizeof(RMobileSmsMessaging::TMobileSmsCapsV1) > aCaps->Size())
    	{
    	return KErrArgument;    	
    	}
    
    RMobileSmsMessaging::TMobileSmsCapsV1Pckg* smsCapsPckg =
        reinterpret_cast< RMobileSmsMessaging::TMobileSmsCapsV1Pckg* >( aCaps );
    RMobileSmsMessaging::TMobileSmsCapsV1& smsCaps = ( *smsCapsPckg )();

    // Capabilities depend on the TSY implementation.  
    smsCaps.iSmsMode = KSmsGsmModeCaps;
    smsCaps.iSmsControl = KSmsControlCaps;

    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::GetReceiveMode 
// This method retrieves the current setting of the TSY-phone 
// incoming SMS receive mode
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::GetReceiveMode( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode )
    {
    *aReceiveMode = iMobileSmsReceiveMode;

    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::SetReceiveMode
// This method sets the setting of the TSY-phone incoming SMS receive mode
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::SetReceiveMode( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobileSmsMessaging::TMobileSmsReceiveMode const* aReceiveMode )
    {
    if ( RMobileSmsMessaging::EReceiveUnstoredClientAck == *aReceiveMode )
        {
        iMobileSmsReceiveMode = *aReceiveMode;

        CompleteNotifyReceiveModeChange();

        ReqCompleted( aTsyReqHandle, KErrNone );
        }
    else
        {
        // EReceiveModeUnspecified, EReceiveStored, EReceiveUnstoredPhoneAck &
        // EReceiveAny modes not supported because of CS functionality.
        ReqCompleted( aTsyReqHandle, KErrNotSupported );
        }

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::SetReceiveModeCancel 
// This method is used to cancel the outstanding asynchronous 
// SetReceiveMode request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::SetReceiveModeCancel( 
    const TTsyReqHandle aTsyReqHandle ) 
    {
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::NotifyReceiveModeChange 
// This method is used to notify the client if there is a 
// change to the receive mode setting
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::NotifyReceiveModeChange( 
    RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode )
    {
    iReqHandleType = EMultimodeSmsNotifyReceiveModeChange;
    iNotifyReceiveModeChangePtr = aReceiveMode;

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::NotifyReceiveModeChangeCancel 
// This method cancel notify receive mode change request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::NotifyReceiveModeChangeCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsNotifyReceiveModeChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    iNotifyReceiveModeChangePtr = NULL;

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteNotifyReceiveModeChange 
// This method completes notify receive mode change request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteNotifyReceiveModeChange()
    {
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsNotifyReceiveModeChange );

    if ( reqHandle )
        {
        if ( iNotifyReceiveModeChangePtr )
			{
			*iNotifyReceiveModeChangePtr = iMobileSmsReceiveMode;
			}
        ReqCompleted( reqHandle, KErrNone );
        }
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::ReceiveMessageL 
// This method starts the reception of incoming unstored SMS
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::ReceiveMessageL( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aMsgData, 
    TDes8* aMsgAttributes )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_RECEIVEMESSAGEL_1, "TSY: CMmSmsTsy::ReceiveMessageL\n");
	
	if(sizeof(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1) > aMsgAttributes->Size())
		{
		return KErrArgument;
		}
	
    iReceiveMessagePduPtr = aMsgData;
    iReceiveMessageParamsPtr = aMsgAttributes;

    if ( iMmPhone->IsModemStatusReady() )
        {
        // activate smsrouting only when modem status is ready        
        ActivateSmsRoutingL();
        }
 
    // Store request handle here. DeliverClass2ToSmsStack-method may complete
    // ReceiveMessage request
#ifdef REQHANDLE_TIMER
    SetTypeOfResponse( EMultimodeSmsReceiveMessage, aTsyReqHandle );
#else
    iTsyReqHandleStore->SetTsyReqHandle( 
        EMultimodeSmsReceiveMessage, aTsyReqHandle );
#endif // REQHANDLE_TIMER

    // Check if there is class 2 messages waiting in TSY's internal memory.
    DeliverClass2ToSmsStack();

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::ReceiveMessageCancel 
// This method cancels an MT message routing to the SMS
// stack. TSY can not stop routing from SMS server because 
// otherwise SMS server routes incoming messages to SIM server.
// Negative acknowledgement is sent to the network if message
// comes when SMS stack routing is not activated
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::ReceiveMessageCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeSmsReceiveMessage );

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::ActivateSmsRoutingL 
// Activate SMS routing
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::ActivateSmsRoutingL()
    {
    if ( ( ERoutingNotActivated == iServerRoutingActivity ) || 
        ( ERoutingActivating == iServerRoutingActivity ) )
        {
        //Send request to the Domestic OS layer..
        User::LeaveIfError( iMmPhone->MessageManager()->HandleRequestL( 
            EMmTsyActivateSmsRouting ) );
        iServerRoutingActivity = ERoutingActivating;
        }    
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteActivateSmsRouting
// This method completes Activation of SMS routing
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteActivateSmsRouting( 
    TInt aError, 
    CMmDataPackage* aDataPackage )
    {
	TUint8 aSmsRoutingStatus ( 0 );
	aDataPackage->UnPackData( aSmsRoutingStatus ) ;

    if ( ( KErrNone == aError ) && 
        ( KSmsRoutingActivated == aSmsRoutingStatus ) )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETEACTIVATESMSROUTING_1, "TSY: CMmSmsTsy::CompleteActivateSmsRouting SMS routing activated");
        iServerRoutingActivity = ERoutingActivated;
        }
	else if ( ( KErrNone == aError ) && 
	    ( KSmsRoutingDeactivated == aSmsRoutingStatus ) )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETEACTIVATESMSROUTING_2, "TSY: CMmSmsTsy::CompleteActivateSmsRouting SMS routing not activated");
        iServerRoutingActivity = ERoutingNotActivated;
		}     
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETEACTIVATESMSROUTING_3, "TSY: CMmSmsTsy::CompleteActivateSmsRouting SMS routing activation failed");
        iServerRoutingActivity = ERoutingNotActivated;

        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
            EMultimodeSmsReceiveMessage );

        if ( reqHandle )
            {
            ReqCompleted( reqHandle, aError );
            }
        }
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteReceiveMessage 
// This method completes reception of incoming unstored SMS
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteReceiveMessage( 
    TInt aError, 
    CMmDataPackage* aDataPackage )
    {   
    TInt ret( KErrNone );
    TBool smsInd( EFalse );
    TSmsMsg* smsMsg;

    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeSmsReceiveMessage );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETERECEIVEMESSAGE_1, "TSY: CMmSmsTsy::CompleteReceiveMessage. iClientStorageFull: %d", iClientStorageFull );

    // SMS successfully received
    if ( KErrNone == aError )
        {
        //unpack received data 
        aDataPackage->UnPackData( smsInd, smsMsg );

        // is the SMS a class2 SMS or not.
        TBool smsClass2 = smsMsg->iSmsClass2;
        
		 // if store is not full, receive new class2 message and get 
         // notification
		 if ( smsClass2 && ( 0 != smsMsg->iLocation ) && iMmSmsStorageTsy ) 
    		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETERECEIVEMESSAGE_2, "TSY: CMmSmsTsy::CompleteReceiveMessage. CompleteNotifyStoreEvent happens");
			iMmSmsStorageTsy->CMmSmsStorageTsy::CompleteNotifyStoreEvent(
			smsMsg->iLocation, RMobilePhoneStore::KStoreEntryAdded	);
    		}

        // if SMS is a Class2 and SMS Memories are full and SIM SMS Memory 
        // not full
        if ( iClientStorageFull && smsClass2 )
            {
            if ( reqHandle )
                {
                // Store SMS in TSY's internal memory
                TRAP( ret, StoreClass2MessageL( smsMsg ) );

                // Message is not delivered to SMS Stack.Deliver message to 
                // SMS. Stack after SMS Stack has called ReceiveMessage..
                smsMsg->iDeleteAfterClientAck = EFalse;
            
#ifdef USING_CTSY_DISPATCHER
                CMmDataPackage package;
                TDesC8* msgData = NULL;
                package.PackData( &msgData );

				// ignore if Acking fails
				TRAP_IGNORE(
							ret = iMmPhone->MessageManager()->HandleRequestL(EMobileSmsMessagingAckSmsStored, &package); 
							);
                iTsyReqHandleStore->ResetTsyReqHandle(EMultimodeSmsReceiveMessage);
                ReqCompleted(reqHandle, aError);
#else            
				// ignore if Acking fails
				TRAP_IGNORE( 
					ret = iMmPhone->MessageManager()->HandleRequestL( 
						EMobileSmsMessagingAckSmsStored );
					);
#endif //USING_CTSY_DISPATCHER			
                }
            else
                {
                // Routing is activated, but SMS stack hasn't called 
                // ReceiveMessage. Nack received message internally with cause 
                // value "protocol error", otherwise SMS server releases 
                // routings.
                CMmDataPackage package;
                TSmsMsg* nullSms = NULL;
                TInt rpCause( KErrGsmSMSMemoryCapacityExceeded );
                package.PackData( &nullSms, &rpCause );

				// ignore if Nacking fails
				TRAP_IGNORE( 
					ret = iMmPhone->MessageManager()->HandleRequestL( 
						EMobileSmsMessagingNackSmsStored, &package );
					);
                }
            }
        else 
            {
            // Class 0 or Class 1 or Class 2 message received.
            // This message has to be acknowledged to the network
            TInt rpError( KErrNone );

            if ( reqHandle && !iClientStorageFull )
                {
                // Deliver message to SMS stack
                if(smsMsg->iSmsMsg.Length() > iReceiveMessagePduPtr->MaxLength())
                   	{
                   	ret = KErrArgument;
                   	}
                else
                  	{
                   	ret = iMmSmsExtInterface->CompleteReceiveMessage( smsMsg,
									iReceiveMessageParamsPtr, iReceiveMessagePduPtr );                	
                  	}
                
                iTsyReqHandleStore->ResetTsyReqHandle( 
                    EMultimodeSmsReceiveMessage );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETERECEIVEMESSAGE_3, "TSY: CMmSmsTsy::CompleteReceiveMessage. Deliver SMS to the SMS stack");
                ReqCompleted( reqHandle, ret ); 
                
                // increase the count of expected acknowledgements
                if(ret == KErrNone)
					{
					iExpectAckOrNack++;
					}
                }
            else
                {
                // Routing is activated, but SMS stack hasn't called 
                // ReceiveMessage. Nack received message internally with cause 
                // value "protocol error", otherwise SMS server releases 
                // routings.
                if (iClientStorageFull)
                    {
                    // SMS stack's memory is full. Nack message with error
                    // MemoryExceeded
                    rpError = KErrGsmSMSMemoryCapacityExceeded;
                    }
                else
                    {
                    rpError = KErrGsmSMSUnspecifiedProtocolError;
                    }                
                }

            if ( KErrNone != rpError )
                {
                CMmDataPackage package;
                TSmsMsg* nullSms = NULL;
                package.PackData( &nullSms, &rpError );

				// ignore if Nacking fails
				TRAP_IGNORE( 
					ret = iMmPhone->MessageManager()->HandleRequestL( 
						EMobileSmsMessagingNackSmsStored, &package );
					);
                }
            }
     
        // end of successful SMS reception
        }
    // error in SMS reception
    else
        {
        //unpack received data - only one parameter information is needed
        aDataPackage->UnPackData( smsInd );

        // if SIM SMS Memory is full
        if  ( ( KErrGsmSMSUnspecifiedProtocolError == aError ) && 
            ( reqHandle ) )
            {
            // SMS Memories not full
            if ( !iClientStorageFull )
                {
                CMmDataPackage package;
                TSmsMsg* nullSms = NULL;
                TInt rpCause( KErrGsmSMSUnspecifiedProtocolError );
                package.PackData( &nullSms, &rpCause );

				// ignore if Nacking fails
				TRAPD( trapError,
					ret = iMmPhone->MessageManager()->HandleRequestL( 
					EMobileSmsMessagingNackSmsStored, &package ); );
				if (  KErrNone != trapError )
					{
					ret = trapError;
					}
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETERECEIVEMESSAGE_4, "TSY: CMmSmsTsy::CompleteReceiveMessage.KErrGsmSMSUnspecifiedProtocolError ");
                }
            else // SMS Memory is full
                {
                CMmDataPackage package;
                TSmsMsg* nullSms = NULL;
                TInt rpCause( KErrGsmSMSMemoryCapacityExceeded );
                package.PackData( &nullSms, &rpCause );

				// ignore if Nacking fails
				TRAPD( trapError,
					ret = iMmPhone->MessageManager()->HandleRequestL( 
						EMobileSmsMessagingNackSmsStored, &package ); );
				if (  KErrNone != trapError )
					{
					ret = trapError;
					}
                }
            }
        else if ( ( KErrGsmSMSMemoryCapacityExceeded == aError ) && 
            ( reqHandle ) )
            {
            CMmDataPackage package;
            TSmsMsg* nullSms = NULL;
            TInt rpCause( KErrGsmSMSMemoryCapacityExceeded );
            package.PackData( &nullSms, &rpCause );

			// ignore if Nacking fails
			TRAPD( trapError,
				ret = iMmPhone->MessageManager()->HandleRequestL( 
					EMobileSmsMessagingNackSmsStored, &package );
				);
			if (  KErrNone != trapError )
				{
				ret = trapError;
				}
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETERECEIVEMESSAGE_5, "TSY: CMmSmsTsy::CompleteReceiveMessage.KErrGsmSMSMemoryCapacityExceeded ");
            }
        else if ( !reqHandle )
            {
            // Routing is activated, but SMS stack hasn't called 
            // ReceiveMessage. Nack received message internally with cause 
            // value "protocol error", otherwise SMS server releases 
            // routings
            CMmDataPackage package;
            TSmsMsg* nullSms = NULL;
            TInt rpCause;
            if (iClientStorageFull)
                {
                rpCause = KErrGsmSMSMemoryCapacityExceeded;
                }
            else
                {
                rpCause = KErrGsmSMSUnspecifiedProtocolError;
                }                
            package.PackData( &nullSms, &rpCause );

			// ignore if Nacking fails
			TRAPD( trapError,
				ret = iMmPhone->MessageManager()->HandleRequestL( 
					EMobileSmsMessagingNackSmsStored, &package );
				);
			if (  KErrNone != trapError )
				{
				ret = trapError;
				}
            }
        else if ( !smsInd ) 
            {
            // Error occurred while handling incoming message or acknowledging 
            // received message
            // Request handle exists (already checked above)
            iTsyReqHandleStore->ResetTsyReqHandle( 
                EMultimodeSmsReceiveMessage );

            ReqCompleted( reqHandle, aError );

            // Incoming message handling failed. If client or TSY doesn't .
            // ack incoming message, SMS Server may release all routings. 
            // Set iSmsServerRoutingActivity to FALSE so client can activate 
            // routings again.
            iServerRoutingActivity = ERoutingNotActivated;
            }

        if ( KErrNone != ret )
            {
            // Nack of received message failed. SMS Server releases all 
            // routings. Set iSmsServerRoutingActivity to FALSE so client
            // can activate routings again
            iServerRoutingActivity = ERoutingNotActivated;
            }
        }   
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::StoreClass2MessageL 
// This method stores received Class 2 message in TSY's internal memory
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::StoreClass2MessageL(
    TSmsMsg const* aSmsMsg )
    {
    TSmsMsg* smsMsg = new ( ELeave ) TSmsMsg();
    CleanupStack::PushL( smsMsg );

    smsMsg->iDeleteAfterClientAck = aSmsMsg->iDeleteAfterClientAck;
    smsMsg->iLocation = aSmsMsg->iLocation;
    smsMsg->iMessageStatus = aSmsMsg->iMessageStatus;
    smsMsg->iMobileScNPI = aSmsMsg->iMobileScNPI;
    smsMsg->iMobileScTON = aSmsMsg->iMobileScTON;
    smsMsg->iServiceCentre.Copy( aSmsMsg->iServiceCentre );
    smsMsg->iSmsMsg.Copy( aSmsMsg->iSmsMsg );

    iSmsMsgArray->AppendL( smsMsg ); 

    CleanupStack::Pop( smsMsg );
    // note: Lint doesn't understand the use of Pop and 'thinks'
    // that there is a memory leak for smsMsg, we disable that warning with
    // the following command
    // lint -e429
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::DeliverClass2ToSmsStack 
// This method delivers a received Class 2 messages that are 
// stored in TSY's internal memory to the SMS stack
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::DeliverClass2ToSmsStack()
    {
    // Check if there is Class 2 messages in TSY's internal memory that can be 
    // now delivered to the SMS stack
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeSmsReceiveMessage );

    if ( reqHandle && !iClientStorageFull )
        {
        for ( TInt i = 0; i < iSmsMsgArray->Count(); i++ )
            {
            if ( EFalse == iSmsMsgArray->At( i )->iDeleteAfterClientAck )
                {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_DELIVERCLASS2TOSMSSTACK_1, "TSY: CMmSmsTsy::DeliverClass2ToSmsStack. Deliver SMS to the SMS stack. Array count: %d", iSmsMsgArray->Count());
				// TSY can now delete the message if SMS stack ack message
                // successfully
                iSmsMsgArray->At( i )->iDeleteAfterClientAck = ETrue;

                TInt ret = iMmSmsExtInterface->CompleteReceiveMessage( 
                    iSmsMsgArray->At( i ), iReceiveMessageParamsPtr, 
                    iReceiveMessagePduPtr );

                // ReceiveMessage request completed Reset request handle
                iTsyReqHandleStore->ResetTsyReqHandle( 
                    EMultimodeSmsReceiveMessage );
                
                // increase the count of expected acknowledgements
                if(ret == KErrNone)
					{
					iExpectAckOrNack++;
					}
                
                ReqCompleted( reqHandle, ret ); 
                break;
                }
            }
        }
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::AckSmsStoredL 
// Route RP-Ack request to active messagehandler
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::AckSmsStoredL( 
    const TTsyReqHandle aTsyReqHandle, 
    const TDesC8* aMsgData, 
    TBool const * aMemoryFull )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_ACKSMSSTOREDL_1, "TSY: CMmSmsTsy::AckSmsStored. aMemoryFull: %d",*aMemoryFull);
    TTsyReqHandle getAckStoredMessageHandle = 
       iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsAckStored );

    if ( EMultimodeSmsReqHandleUnknown < getAckStoredMessageHandle )
        {
        // Send request is already ongoing
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
    if ( iExpectAckOrNack <= 0 )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_ACKSMSSTOREDL_2, "TSY: CMmSmsTsy::AckSmsStoredL. -> ReceiveMessage not requested - error returned ");
        ReqCompleted( aTsyReqHandle, KErrNotReady );
        return KErrNone; 
        }
    else
        {
        iClientStorageFull = *aMemoryFull;
        TBool ackMessage( ETrue );

        // Check if TSY has stored received Class 2 message to it's internal 
        // memory. TSY can now delete message from it's internal memory
        // because SMS stack received message successfully.
        for ( TInt i = 0; i < iSmsMsgArray->Count(); i++ )
            {
            if ( EFalse != iSmsMsgArray->At( i )->iDeleteAfterClientAck && 
                 KErrNone == aMsgData->CompareF( 
                 iSmsMsgArray->At( i )->iSmsMsg ) )
                {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_ACKSMSSTOREDL_3, "TSY: CMmSmsTsy::AckSmsStored. Delete SMS: %d, Array count: %d",i,iSmsMsgArray->Count());
                delete iSmsMsgArray->At( i ); // Delete object from memory
                iSmsMsgArray->Delete( i );    // Delete pointer from array
                iSmsMsgArray->Compress(); 

                // Class 2 message is already acknowledged to the network 
                // Complete request here and don't send ack to the .
                // network.
                ReqCompleted( aTsyReqHandle, KErrNone );    
                ackMessage = EFalse;

                // Check if there is more class 2 messages waiting in TSY's 
                // internal memory
                DeliverClass2ToSmsStack();
                break;
                }     
            }

        if ( ackMessage )
            {
            // Pack data
            CMmDataPackage package;
            TDesC8* msgData = const_cast<TDesC8*>( aMsgData );
            package.PackData( &msgData );

            TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobileSmsMessagingAckSmsStored, &package );

            if ( KErrNone == ret )
                {
                iReqHandleType = EMultimodeSmsAckStored;
                }
            else
                {
                // Message construction failed or phonet returned error
                ReqCompleted( aTsyReqHandle, ret );

                // Acknowledging failed. SMS Server will now release routings. 
                // Complete receive message request with KErrGeneral and 
                // set routing activity to false. SMS Stack makes new 
                // ReceiveMessage request.
                TBool smsInd( EFalse );
                CMmDataPackage data;
                TSmsMsg* nullSms = NULL;
                data.PackData( &smsInd, &nullSms );
                CompleteReceiveMessage( KErrGeneral, &data );
                }
            }
        }

    // decrease the count of expeced acknowledgements
    iExpectAckOrNack--;
    
    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteAckSmsStored 
// Complete clients AckSmsStored request 
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteAckSmsStored( 
    TInt aError )
    {
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsAckStored );     
    if ( reqHandle )
        {
        ReqCompleted( reqHandle, aError );
        }

    if (aError != KErrNone)
        {
        // Ack error from LTSY. Need to reject receive messege request, to force the client to repost it.
        reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(EMultimodeSmsReceiveMessage);
        if( reqHandle != 0 )
            {
            ReqCompleted(reqHandle,  KErrGeneral);
            }
        iServerRoutingActivity = ERoutingNotActivated;
        }
    }   

//---------------------------------------------------------------------------- 
// CMmSmsTsy::NackSmsStored 
// Route RP-Error request to messagehandler
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::NackSmsStoredL( 
    const TTsyReqHandle aTsyReqHandle, 
    const TDesC8* aMsgData, 
    TInt* aRpCause )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_NACKSMSSTOREDL_1, "TSY: CMmSmsTsy::NackSmsStored. aRpCause: %d",*aRpCause);
    TTsyReqHandle nackHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsNackStored );

    if ( 0 < nackHandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
    if ( iExpectAckOrNack <= 0 )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_NACKSMSSTOREDL_2, "TSY: CMmSmsTsy::AckSmsStoredL. -> ReceiveMessage not requested - error returned ");
        ReqCompleted( aTsyReqHandle, KErrNotReady );
        return KErrNone; 
        }
    else
        {
        TBool nackMessage( ETrue );
        
        // Check if TSY has stored received Class 2 message to it's internal 
        // memory. TSY can now delete message from it's internal memory
        // because SMS stack received message successfully
        for ( TInt i = 0; i < iSmsMsgArray->Count(); i++ )
            {
            // Check if message is waiting SMS stack's acknowledging and 
            // message contents is same as aMsgData.
            if ( EFalse != iSmsMsgArray->At( i )->iDeleteAfterClientAck && 
                 KErrNone == aMsgData->CompareF( 
                 iSmsMsgArray->At( i )->iSmsMsg ) )
                {
                switch ( *aRpCause )
                    {
                    // ETel specific value
                    case KErrGsmSMSMemoryCapacityExceeded: 
                    // Defect in SMS stack. Stack sets TP cause value to the  
                    // RP cause value TP-FCS 0xD3, RP cause 0x16
                    // 0xD3:Memory Capacity Exceeded
                        iClientStorageFull = ETrue;
                       // without break statement, it will run "iSmsMsgArray->At( i )->iDeleteAfterClientAck = EFalse" in next case block.
                    case 0xD3:
                        // Deliver this message to SMS stack when SMS stack  
                        // has enough memory.
                        iSmsMsgArray->At( i )->iDeleteAfterClientAck = EFalse; 
                        break;
                    default:
                        // Delete message because SMS stack couldn't handle it.
                        // Delete object from memory
                        delete iSmsMsgArray->At( i ); 
                        // Delete pointer from array
                        iSmsMsgArray->Delete( i );    
                        iSmsMsgArray->Compress();
                        break;
                    }

                // Class 2 message is already acknowledged to the network by 
                // adaptation. Complete request here and don't send ack to the 
                // network.
                ReqCompleted( aTsyReqHandle, KErrNone );    
                nackMessage = EFalse;
                break;
                }     
            }

        if ( nackMessage )
            {
            CMmDataPackage package;
            // packed parameter: pointer to TDesC8 (TPDU data)
            // and TInt (RP cause)
            TDesC8* tempMsgDataPtr = const_cast<TDesC8*>( aMsgData );
            package.PackData( &tempMsgDataPtr, aRpCause );

            TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobileSmsMessagingNackSmsStored, &package );

            if ( KErrNone == ret )
                {
                iReqHandleType = EMultimodeSmsNackStored;
                }
            else 
                {
                // Message construction failed or phonet sender returned 
                // error
                ReqCompleted( aTsyReqHandle, ret );

                // Acknowledging failed. SMS Server will now release routings. 
                // Complete receive message request with KErrGeneral and 
                // set routing activity to false. SMS Stack makes new 
                // ReceiveMessage request.
                TBool smsInd( EFalse );
                CMmDataPackage data;
                TSmsMsg* nullSms = NULL;
                data.PackData( &smsInd, &nullSms );
                CompleteReceiveMessage( KErrGeneral, &data );
                }
            }
        }
    
    
    // decrease the count of expeced acknowledgements
    iExpectAckOrNack--;
    
    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteNackSmsStored 
// Complete clients NackSmsStored request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteNackSmsStored( 
    TInt aError )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsNackStored );
    if ( reqHandle )
        {
        ReqCompleted( reqHandle, aError );
        }

    if (aError != KErrNone)
        {
        // Nack error from LTSY. Need to reject receive messege request, to force the client to repost it.
        reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(EMultimodeSmsReceiveMessage);
        if( reqHandle != 0 )
            {
            ReqCompleted(reqHandle,  KErrGeneral);
            }
        iServerRoutingActivity = ERoutingNotActivated;
        }
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::ResumeSmsReception 
// Notify SMS Server, that client have again free memory for incoming SMSes
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::ResumeSmsReceptionL( 
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_RESUMESMSRECEPTIONL_1, "TSY: CMmSmsTsy::ResumeSmsReceptionL");
    TTsyReqHandle resumeHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsResumeReception );

    if ( 0 < resumeHandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about 
        // the situation
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else if ( iIsOffline )
        {
        // CS hardware is in off-line mode. We cannot issue this request
        // now, but we will do it as soon as we are on-line again. See
        // method SetOffline.
        iResumeSmsReceptionPending = ETrue;

        // Complete with KErrNone. Client will never notice that the
        // request was not yet sent to DOS.
        ReqCompleted( aTsyReqHandle, KErrNone );
        }
    else
        {
        TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobileSmsMessagingResumeSmsReception );
   
        if ( KErrNone == ret )
            {
            iReqHandleType = EMultimodeSmsResumeReception;
            }
        else 
            {
             // Message construction failed or phonet sender returned error
            ReqCompleted( aTsyReqHandle, ret );
            }
        }

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteResumeSmsReception 
// This method completes clients ResumeSmsReception request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteResumeSmsReception( 
    TInt aError )
    {
    if ( KErrNone == aError )
        {
        iClientStorageFull = EFalse;
        }

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsResumeReception );
    if ( reqHandle  )
        {
        ReqCompleted( reqHandle, aError );
        }

    // Suspended SMS reception resumed. Check if there is class 2 messages 
    // waiting in TSY's internal memory.
    DeliverClass2ToSmsStack();
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::SetOffline 
// Sets information about CS hardware mode (off-line/on-line)
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::SetOffline( 
    TBool aIsOffline )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_SETOFFLINE_1, "TSY: CMmSmsTsy::SetOffline has been called with %d", aIsOffline);

    if ( !aIsOffline && iIsOffline )
        {
        // changing from off-line to on-line
        if ( iResumeSmsReceptionPending ) 
            {
            // now send the pending ResumeSmsReception
            // request to DOS. We are not interested if the 
            // request has failed, and it has already been completed
            // to the client (with KErrNone). See method ResumeSmsReception.
			TInt trapError = KErrNone;
			TRAP( trapError, iMmPhone->MessageManager()->HandleRequestL( 
                    EMobileSmsMessagingResumeSmsReception );
			    );
            iResumeSmsReceptionPending = EFalse;
            }
        }
    iIsOffline = aIsOffline;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::GetMoSmsBearer 
// This method is used to retrieve the current setting for the 
// bearer type used for sending SMS messages
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::GetMoSmsBearer( 
    const TTsyReqHandle /*aTsyReqHandle*/, 
    RMobileSmsMessaging::TMobileSmsBearer* /*aBearer*/ )
    {
    return KErrNotSupported;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::SetMoSmsBearer 
// This method sets the bearer type for sending SMS messages
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::SetMoSmsBearer( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobileSmsMessaging::TMobileSmsBearer* aBearer )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_SETMOSMSBEARER_1, "TSY: CMmSmsTsy::SetMoSmsBearer called");
    
    TTsyReqHandle setMoSmsBearerHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsSetMoSmsBearer );
    
    if ( 0 < setMoSmsBearerHandle )
        {
        // The request is already processing because of previous request.
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        if ( ( *aBearer == RMobileSmsMessaging::ESmsBearerPacketOnly ) || 
            ( *aBearer == RMobileSmsMessaging::ESmsBearerCircuitOnly ) || 
            ( *aBearer == RMobileSmsMessaging::ESmsBearerPacketPreferred ) || 
            ( *aBearer == RMobileSmsMessaging::ESmsBearerCircuitPreferred ) )
            {
            TInt ret = KErrNone;

            CMmDataPackage package;
            package.PackData( aBearer );
            
            // Send request to the DOS layer.
            TRAP_IGNORE( ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobileSmsMessagingSetMoSmsBearer, &package ); );
            
            if ( KErrNone == ret )
                {
#ifdef REQHANDLE_TIMER
                SetTypeOfResponse( EMultimodeSmsSetMoSmsBearer, aTsyReqHandle );
#else
                iTsyReqHandleStore->SetTsyReqHandle( 
                    EMultimodeSmsSetMoSmsBearer, aTsyReqHandle );
#endif // REQHANDLE_TIMER
                }
            else
                {
                // Call to DOS failed
                ReqCompleted( aTsyReqHandle, ret );
                }
            }
        else
            {
            ReqCompleted( aTsyReqHandle, KErrNotSupported );
            }
        }
    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteSetMoSmsBearer 
// Complete clients SetMoSmsBearer request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteSetMoSmsBearer( 
    TInt aResult )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsSetMoSmsBearer );
    
    if ( reqHandle )
        {
        ReqCompleted( reqHandle, aResult );
        }
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::SetMoSmsBearerCancel 
// This method is used to cancel an outstanding asynchronous 
// SetMoSmsBearer request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::SetMoSmsBearerCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeSmsSetMoSmsBearer );
    
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::NotifyMoSmsBearerChange 
// This method is used to notify the client if there is a 
// change to the setting for the bearer type used for sending SMS messages
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::NotifyMoSmsBearerChange( 
    RMobileSmsMessaging::TMobileSmsBearer* /*aBearer*/ )
    {
    return KErrNotSupported;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::NotifyMoSmsBearerChangeCancel 
// This method is used to cancel an outstanding asynchronous 
// NotifyMoSmsBearerChange request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::NotifyMoSmsBearerChangeCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsNotifyMoSmsBearerChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    iNotifySmsBearerPtr = NULL;
    
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::SendMessageL 
// Routes send message to network request to message handler
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::SendMessageL( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aMsgData, 
    TDes8* aMsgAttributes )
    {
    TInt ret( KErrNone );
    
    TTsyReqHandle getSendMessageHandle = 
       iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsSendMessage );
    TTsyReqHandle getSendSatMessageHandle = 
       iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsSendSatMessage );

    TTsyReqHandle getSendMessageNoFdnHandle = 
       iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsSendMessageNoFdnCheck );
    if ( ( EMultimodeSmsReqHandleUnknown < getSendMessageHandle ) 
        || ( EMultimodeSmsReqHandleUnknown < getSendSatMessageHandle )
        || ( EMultimodeSmsReqHandleUnknown < getSendMessageNoFdnHandle ) )
        {
        // Send request is already ongoing 
        // (through ETel messaging or through SAT)
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else if(sizeof(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1) > aMsgAttributes->Size())
        {
        ReqCompleted( aTsyReqHandle, KErrArgument );
        }
    else
        {
        iSendMessageMsgAttrPckgPtr = aMsgAttributes;

        //  Create package
        CMmDataPackage package;

        // typecast for aMsgAttributes
        RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg* attsPckg = 
           reinterpret_cast< 
		   RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg* >( 
		   aMsgAttributes );
        RMobileSmsMessaging::TMobileSmsSendAttributesV1& msgAttr = 
            ( *attsPckg )();
        
        // structure for all sms parameters and data
        TSendSmsDataAndAttributes sendData;
        TSmsRequestTypes reqType;

        sendData.iAttributes = &msgAttr;
        sendData.iMsgData = aMsgData;
  
        if ( iSmsNoFdnCheckFlag == ESmsNoFdnCheckUsed )
            {
            //set ipc
            sendData.iIpc = EMobileSmsMessagingSendMessageNoFdnCheck; 
            reqType = EMultimodeSmsSendMessageNoFdnCheck;
            }
        else //(iSmsNoFdnCheckFlag == ESmsNoFdnCheckNotUsed)
            {
            //set ipc
            sendData.iIpc = EMobileSmsMessagingSendMessage;
            reqType = EMultimodeSmsSendMessage;
            }

        // Pack parameters
        package.PackData( &sendData );

        iSmsSendReq = new (ELeave) CSmsSendRequest();
        iSmsSendReq->SetSmsDataAndAttributes( sendData );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_SENDMESSAGEL_1, "TSY: CMmSmsTsy::SendMessageL: Send request saved");

#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( reqType, aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle( reqType, aTsyReqHandle );
#endif // REQHANDLE_TIMER

        TInt leaveCode( KErrNone );
        // send request to DOS
        // packed parameter: TSendSmsDataAndAttributes
        TRAP(leaveCode, ret = iMmPhone->MessageManager()->HandleRequestL(sendData.iIpc, &package ););

        if ( (leaveCode != KErrNone) || (ret != KErrNone) )
            {
            iTsyReqHandleStore->ResetTsyReqHandle( reqType );
            delete iSmsSendReq;              // Delete object
            iSmsSendReq = NULL; // Reset pointer
            // reset pointer to client memory
            iSendMessageMsgAttrPckgPtr = NULL;
            iSmsNoFdnCheckFlag = ESmsNoFdnCheckUnknown;
            if (leaveCode != KErrNone)
                {
                ReqCompleted( aTsyReqHandle, leaveCode );
                }
            else
                {
                ReqCompleted( aTsyReqHandle, ret );
                }
            }
        else
            {
            iSmsSendReq->IncreaseSendCounter();
            }
        }

    return KErrNone;
    }
  
//----------------------------------------------------------------------------
// CMmSmsTsy::IsRPError
// Checks if error code is a relay protocol error
// --------------------------------------------------------------------------- 
TBool CMmSmsTsy::IsRPError(TInt aError)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_ISRPERROR_1, "CMmSmsTsy::IsRPError(): %{TSymbianErrorCodes}", aError);
    
    TBool  isRPError = EFalse;
    switch (aError)
        {
        case KErrGsmSMSShortMessageTransferRejected:
            {
            isRPError = ETrue;
            }
            break;   
        case KErrGsmSMSInvalidMandatoryInformation:
            {
            isRPError = ETrue;
            }
            break;
        case KErrGsmSMSUnidentifiedSubscriber:
            {
            isRPError = ETrue;
            }
            break;
        case KErrGsmSMSUnknownSubscriber:
            {
            isRPError = ETrue;
            }
            break;
        case KErrGsmSMSNetworkOutOfOrder:
            {
            isRPError = ETrue;
            }
            break;
        default:
            {
            // NOP
            }
            break;
        }

    return isRPError;
    } // CMmSmsTsy::IsRPError


//----------------------------------------------------------------------------
// CMmSmsTsy::CompleteSendMessage 
// Complete SendMessage request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteSendMessage( 
    TInt aError, 
    CMmDataPackage* aDataPackage )
    {
    TInt extendedError = CMmCommonStaticUtility::ExtendedErrorCode(aError);
    TBool res = IsRPError(extendedError);
    if(res)
        {
        // Delete send message entry from send array
        if ( iSmsSendReq )
            {
            delete iSmsSendReq;   // Delete object
            iSmsSendReq = NULL;   // Reset pointer
            }
        // reset req handle and complete request
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
                   EMultimodeSmsSendMessage );

        if ( EMultimodeSmsReqHandleUnknown < reqHandle ) 
            {
            ReqCompleted( reqHandle, extendedError );
            // reset pointer to client memory
            iSendMessageMsgAttrPckgPtr = NULL;
            iSmsNoFdnCheckFlag = ESmsNoFdnCheckUnknown;
            }
        }        
    else if ( ( KErrNone != aError ) && ( KErrTimedOut != aError ) 
        && ( KErrGsmSMSOperationNotAllowed != 
            CMmCommonStaticUtility::ExtendedErrorCode ( aError ) )
            // FDB check failed
        && ( KErrGsmSMSUnspecifiedProtocolError != 
            CMmCommonStaticUtility::ExtendedErrorCode ( aError ) ) 
        && ( KErrSatControl != 
            CMmCommonStaticUtility::ExtendedErrorCode ( aError ) ) 
        && ( !( iMmPhone->GetSatMessaging() && 
            iMmPhone->GetSatMessaging()->IsMoSmControlBySimActivated() ) ) 
        && ( NULL != iSmsSendReq ) && ( 2 >= 
            iSmsSendReq->GetSendCounter() ) ) 
        {
        // DOS returned error to send request. Message might be tried to be 
		// resent (see method ResendSms).
        // Timeout mechanism cannot access this part of code, ever.
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETESENDMESSAGE_1, "TSY: CMmSmsTsy::CompleteSendMessage. Resend counter: %d", iSmsSendReq->GetSendCounter());
        }
    else
        {
        // This is executed
		// - if sending was successful
		// - if there are no more resending attempts
		// - if timer expires (called from the Complete of CMmSmsTsy class)

        // Delete send message entry from send array
        if ( iSmsSendReq )
            {
            delete iSmsSendReq;   // Delete object
            iSmsSendReq = NULL;   // Reset pointer
            }

        // reset req handle and complete request
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
            EMultimodeSmsSendMessage );
            
        if ( EMultimodeSmsReqHandleUnknown < reqHandle ) 
            {
            // get values only if there was no error, and if return pointer to 
            // client is available
            if ( KErrNone == aError && iSendMessageMsgAttrPckgPtr )
                {
                TInt16 msgRef( 0 );
                TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg;

                aDataPackage->UnPackData( msgRef, smsMsg );

               RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg* attsPckg = 
                    reinterpret_cast< 
					RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg* >(
                    iSendMessageMsgAttrPckgPtr );
                RMobileSmsMessaging::TMobileSmsSendAttributesV1& msgAttr = 
                    ( *attsPckg )();

                msgAttr.iFlags = RMobileSmsMessaging::KMessageReference;
                msgAttr.iMsgRef = static_cast< TUint16 >( msgRef );

                if ( NULL != smsMsg.Length() )
                    {
                    msgAttr.iSubmitReport.Copy(smsMsg);
                    msgAttr.iFlags |= RMobileSmsMessaging::KGsmSubmitReport;
                    }
                }

            ReqCompleted( reqHandle, aError );
            // reset pointer to client memory
            iSendMessageMsgAttrPckgPtr = NULL;
            iSmsNoFdnCheckFlag = ESmsNoFdnCheckUnknown;
            }
        }

    // Resend unsent message
    ResendSms();
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::SendSatMessage 
// SimAtkTsy can send SMS messages to the network by calling this method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::SendSatMessage( 
	MStkTsySatService& aStkTsySatService, 
    TDes8* aSmsTpdu, 
    TDes16* aScAddress, 
    RMobilePhone::TMobileTON* aMobileTON, 
    RMobilePhone::TMobileNPI* aMobileNPI, 
    TBool aMoreToSend, 
    TTsyReqHandle aTsyReqHandle )
    {
    iTsySatMessaging = &aStkTsySatService;

    TTsyReqHandle getSendMessageHandle = 
       iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsSendMessage );
    TTsyReqHandle getSendSatMessageHandle = 
       iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsSendSatMessage );

    if ( ( EMultimodeSmsReqHandleUnknown < getSendMessageHandle ) 
        || ( EMultimodeSmsReqHandleUnknown < getSendSatMessageHandle ) )
        {
        // Send request is already ongoing 
        // (through ETel messaging or through SAT)
        iTsySatMessaging->CompleteSendSmsMessage( KErrServerBusy );
        }
    else
        {

        TRAPD( leaveCode, DoSendSatMessageL( aSmsTpdu, aScAddress, aMobileTON, 
            aMobileNPI, aMoreToSend, aTsyReqHandle ); );

        if ( KErrNone != leaveCode )
            {
            // Call to DOS failed
            CompleteSendSatMessage( leaveCode );
            }
        }

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::DoSendSatMessageL 
// SimAtkTsy can send SMS messages to the network by calling this method.
// This method TRAPs failures.
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::DoSendSatMessageL( 
    TDes8* aSmsTpdu, 
    TDes16* aScAddress, 
    RMobilePhone::TMobileTON* aMobileTON, 
    RMobilePhone::TMobileNPI* aMobileNPI, 
    TBool aMoreToSend, 
    TTsyReqHandle aTsyReqHandle )
    {
    // Leave if SendSatMessageL returns an error
    User::LeaveIfError( SendSatMessageL( iMmPhone, aSmsTpdu, 
        aScAddress, aMobileTON, aMobileNPI, aMoreToSend ) );

#ifdef REQHANDLE_TIMER
    SetTypeOfResponse( EMultimodeSmsSendSatMessage, aTsyReqHandle );
#endif // REQHANDLE_TIMER
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteSendSatMessage 
// Compelete SendSatMessage request to the MMSAT
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteSendSatMessage( 
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETESENDSATMESSAGE_1, "TSY: CMmSmsTsy::CompleteSendSatMessage. Complete SAT SMS send Error: %{TSymbianErrorCodes}", aError);
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeSmsSendSatMessage );
    
    if ( iTsySatMessaging )
        {
        iTsySatMessaging->CompleteSendSmsMessage( aError );
        }
    
#ifdef USING_CTSY_DISPATCHER
    
    //the iTsySatMessaging is now Nulled on the complete
    //open issue that if the iTsySatMessaging pointer (to the SIM ATK) is not nulled (i.e. the callback is
    //not completed) and the CTSY is destructed after the SIM ATK then the CMmSmsTsy has a invalid pointer which 
    //is used in the CMmSmsTsy destructor
    
    iTsySatMessaging = NULL;
#endif //USING_CTSY_DISPATCHER
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::EnumerateMessageStores 
// This methods returns the number of phone-side SMS message 
// stores supported by the phone
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::EnumerateMessageStores( 
    const TTsyReqHandle aTsyReqHandle, 
    TInt* aCount )
    {
    *aCount = KSmsStoreNumber;
    ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::GetMessageStoreInfo 
// This method gets used SMS entries and total number of SMS entries on SIM
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::GetMessageStoreInfoL( 
    const TTsyReqHandle aTsyReqHandle, 
    TInt const* aIndex, 
    TDes8* aInfo )
    {
    TTsyReqHandle getStoreInfoHandle = 
       iTsyReqHandleStore->GetTsyReqHandle( 
       EMultimodeSmsGetMessageStoreInfo );

    if ( EMultimodeSmsReqHandleUnknown < getStoreInfoHandle )
        {
        // The request is already in processing because of previous request.
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else if ( (*aIndex >= KSmsStoreNumber) || (*aIndex < 0) )    
        {
        // First store number is "0" and SIM is only supported store
        ReqCompleted( aTsyReqHandle, KErrArgument );
        }
    else
        {
        iGetMessageStoreInfoPtr = aInfo;
        // allow for EMobileSmsMessagingGetMessageStoreInfo immediate 
        // completion
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( EMultimodeSmsGetMessageStoreInfo, aTsyReqHandle );
#else
        // Never comes here. See SetTypeOfResponse.
        iTsyReqHandleStore->SetTsyReqHandle( EMultimodeSmsGetMessageStoreInfo, 
            aTsyReqHandle );
#endif // REQHANDLE_TIMER

        //Send request to the Domestic OS layer.
        TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobileSmsMessagingGetMessageStoreInfo );
        
        // DOS layer returned with error without completing request
        if ( KErrNone != ret && iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodeSmsGetMessageStoreInfo) )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        }
        
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::GetMessageStoreInfoCancel 
// Cancel pending GetMessagestoreInfo request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::GetMessageStoreInfoCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeSmsGetMessageStoreInfo );

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::CompleteGetMessageStoreInfo 
// Complete GetMessageStoreInfo request to client
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteGetMessageStoreInfo( 
    TInt aError, 
    CMmDataPackage* aDataPackage )
    {
    TUint8 totalEntries( 0 );
    TInt usedEntries( 0 );

	TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
		 EMultimodeSmsGetMessageStoreInfo );

	if ( reqHandle  )
        {
		// Request was successful
		if ( ( KErrNone == aError ) || ( KErrNotFound == aError ) )
        	{
        	aDataPackage->UnPackData( totalEntries, usedEntries );

			aError = iMmSmsExtInterface->MessageStoreInfo( 
				iGetMessageStoreInfoPtr, totalEntries, usedEntries );
        	}
    
		iSmsNumOfLoc = totalEntries;
	
		ReqCompleted( reqHandle, aError );
		}
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::ReadSmspListPhase1L 
// This method starts first phase of SMS Parameter sets 
// retrieval On the first phase method store locations 
// temporarely to the TSY. On the second phase method copies the
// list to the client
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::ReadSmspListPhase1L(
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TClientId const* aId, 
    TInt* aBufSize )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_READSMSPLISTPHASE1L_1, "TSY: CMmSmsTsy::ReadSmspListPhase1L");

    TTsyReqHandle readSmspHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeSmsReadSmspListPhase1 );

    if ( 0 < readSmspHandle )
        {
        // The request is already processing because of previous request
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // Lets check how many SMSP sets are there.
        TInt ret = iMmPhone->MessageManager()->HandleRequestL(
            EMobileSmsMessagingGetSmspListPhase1 );

        if ( KErrNone == ret )
            {
            iReqHandleType = EMultimodeSmsReadSmspListPhase1;
            
            iRetSMSPSize = aBufSize;
            
            // Copy client id (session and subsession handle). it's used for
            // matching phase 1 and 2 of a request
            
            if ( iSMSPClientId )
                {
                delete iSMSPClientId;
                iSMSPClientId = NULL;
                }
                
            iSMSPClientId = new ( ELeave ) RMobilePhone::TClientId( *aId );
            }
        else
            {
            // Call to DOS failed
            ReqCompleted( aTsyReqHandle, ret );
            }
        }

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::ReadSmspListPhase2 
// This method starts second phase of SMS Parameter sets retrieval. It is
// guaranteed that this is only called if phase 1 was completed with KErrNone.
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::ReadSmspListPhase2( 
    TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TClientId const* aId, 
    TDes8* aBuffer )
    {
    // Check if client id matches between phase 1 and phase 2
    if ( ( iSMSPClientId->iSessionHandle == aId->iSessionHandle ) &&
        ( iSMSPClientId->iSubSessionHandle == aId->iSubSessionHandle ) )
        {
        // Copy the streamed list to the client
        aBuffer->Copy( iSMSPList->Ptr( 0 ) );

        iMmPhone->ReqCompleted( aTsyReqHandle, KErrNone );
        }
    else
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrCorrupt );
        }

    // Reset the client Id
    delete iSMSPClientId;
    iSMSPClientId = NULL;

    // Reset the streamed list
    delete iSMSPList;
    iSMSPList = NULL;

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::ReadAllSmspCancel 
// This method cancels read all SMS Parameter sets request. 
// This method can be called between phase1 and phase2
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::ReadAllSmspCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeSmsReadSmspListPhase1 );

    iRetSMSPSize = NULL;
    
    if ( iSMSPClientId )
        {
        delete iSMSPClientId;
        iSMSPClientId = NULL;
        }

    if ( iSMSPList )
        {
        delete iSMSPList;
        iSMSPList = NULL;
        }

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CreateSmspListL 
// This method creates SMSP list from read entries. 
// Method reads the list, and stores its content in iSMSPList.
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CreateSmspListL(
    CArrayPtrFlat<TSmsParameters>& aSmspList )
    {
    CMobilePhoneSmspList* list = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL( list );

    TUint8 amountOfSets = static_cast< TUint8 >( aSmspList.Count() );

    RMobileSmsMessaging::TMobileSmspEntryV1 entry;

    // Runs until TSY's internal SMSP storage is empty 
    while ( 0 != aSmspList.Count() )
        {
        TSmsParameters* smsParam = aSmspList.At( 0 );
        entry.iIndex = ( smsParam->iLocationNumber );

        if ( smsParam->iAlphaTagPresent )
            {                       
            entry.iText.Copy( smsParam->iAlphaTagData );
            }
        else
            {
            entry.iText.SetLength( 0 );
            }

        entry.iValidParams = ( smsParam->iParameterIndicator );

        TInt paramask = 
            KSmsGsmParametersIndMask & ( smsParam->iParameterIndicator );

        if ( paramask & KDestinationAddress )
            {
            entry.iDestination.iTypeOfNumber = ( smsParam->iMobileDeTON );
            entry.iDestination.iNumberPlan = ( smsParam->iMobileDeNPI );
            entry.iDestination.iTelNumber = ( smsParam->iDestinationAddress );
            }

        if ( paramask & KServiceCentreAddress )
            {
            entry.iServiceCentre.iTypeOfNumber = ( smsParam->iMobileScTON );
            entry.iServiceCentre.iNumberPlan = ( smsParam->iMobileScNPI );
            entry.iServiceCentre.iTelNumber = 
                ( smsParam->iServiceCenterAddress );
            }

        if ( paramask & KProtocolID )
            {
            entry.iProtocolId = ( smsParam->iProtocolId );
            }

        if ( paramask & KDataCodingScheme )
            {
            entry.iDcs = ( smsParam->iDataCodingScheme );
            }

        if ( paramask & KValidityPeriod )
            {
            entry.iValidityPeriod = ( smsParam->iValidityPeriod );
            }

        list->AddEntryL( entry );

        // Delete handled SMSP set
        delete smsParam;                // Delete object
        aSmspList.Delete( 0 );          // Delete pointer from array
        aSmspList.Compress();
        }

    // Set maximum amount of SMSP sets to the list.
    list->SetMaxNumberEntries( amountOfSets );
    
    iSMSPList = list->StoreLC(); // creates new CBufFlat buffer and 
                                 // pushes it on cleanup stack
    CleanupStack::Pop(); // pop the iSMSPList
    
    CleanupStack::PopAndDestroy(); // pop&destroy CMobilePhoneSmspList* list
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteReadAllSmspPhase1 
// Complete first phase of read all SMSP sets request, i.e. returns 
// size of iSMSPList buffer to client.
// Parameter aDataPackage contains SMSP list in CArrayPtrFlat<TSmsParameters>.
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteReadAllSmspPhase1(
    TInt aError, 
    CMmDataPackage* aDataPackage )
    {
    CArrayPtrFlat<TSmsParameters>* smsParams;

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsReadSmspListPhase1 );
    
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETEREADALLSMSPPHASE1_1, "TSY: CMmSmsTsy::CompleteReadAllSmspPhase1 Complete read first phase error: %{TSymbianErrorCodes}",aError);
    if ( KErrNone == aError )
        {
        aDataPackage->UnPackData( &smsParams );
        if ( reqHandle )
            {
            // Create the Smsp list
            TInt trapError;
            if( NULL == smsParams)
                {
                trapError = KErrGeneral;
                }
            else
                {
                TRAP( trapError, CreateSmspListL( *smsParams ); );
                }
           
            if ( KErrNone == trapError )
                {
                *iRetSMSPSize = iSMSPList->Size();
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETEREADALLSMSPPHASE1_2, "TSY: CMmSmsTsy::CompleteReadAllSmspPhase1: Phase 1 OK.");
                // Complete first phase of read all SMSP sets
                ReqCompleted( reqHandle, KErrNone );
                }
            else
                {   
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETEREADALLSMSPPHASE1_3, "TSY: CMmSmsTsy::CompleteReadAllSmspPhase1: Could not create SMSP list, trapError=%{TSymbianErrorCodes}",trapError);
                // Complete with error
                ReqCompleted( reqHandle, trapError );
                }
            }
        }
    else
        {
        if ( reqHandle )
            {
            // Complete first phase of read all SMSP sets.
            ReqCompleted( reqHandle, aError );
            }
        }
	iRetSMSPSize = NULL;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::StoreSmspList 
// This method start storing of new version of the entire list 
// of SMSP entries prosess
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::StoreSmspList( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aBuffer )
    {
    TTsyReqHandle storeSmspListHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeSmsStoreSmspList );

    if ( 0 < storeSmspListHandle )
        {
        // The request is already processing because of previous request.
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        iStoreSmspBufferPtr = aBuffer;
        iStoreSmspIndex = 0;

        // We must store request handle here and not in ExtFunc-method. 
        // If SMSP list is empty we must complete store SMSP list request in 
        // ProcessStoreSmspListL-method and then we need request handle.
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( EMultimodeSmsStoreSmspList, aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle( EMultimodeSmsStoreSmspList, 
            aTsyReqHandle );
#endif // REQHANDLE_TIMER
        CallProcessStoreSmspList();
        }

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::InternalStoreSmspList 
// This method check if there was error while updating last SMSP
//set. If there was no error it calls CompleteStoreSmspListL- method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::InternalStoreSmspList( 
    TInt aError )
    {
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeSmsStoreSmspList );

    if ( reqHandle )
        {
        if ( KErrNone == aError )
            {
            // Lets continue storing prosess
            CallProcessStoreSmspList();
            }
        else
            {
            CompleteStoreSmspList( aError );
            }
        }
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CallProcessStoreSmspList 
// This method calls ProcessStoreSmspListL-method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CallProcessStoreSmspList()
    {
    TRAPD ( ret, ProcessStoreSmspListL(); );
    if ( ret != KErrNone )
        {
        CompleteStoreSmspList( ret );
        }
    }   

//----------------------------------------------------------------------------
// CMmSmsTsy::ProcessStoreSmspListL 
// This method gets one SMSP sets from clients list and calls 
// DOS OR completes client's request 
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::ProcessStoreSmspListL()
    {
    CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL( smspList );
    
    smspList->RestoreL( *iStoreSmspBufferPtr );

    if ( smspList->Enumerate() > iStoreSmspIndex )
        {
        RMobileSmsMessaging::TMobileSmspEntryV1 smsParam;
        CMmDataPackage package;

        // Clients SMSP lists first location is 0
        smsParam = smspList->GetEntryL( iStoreSmspIndex );

        iStoreSmspIndex++; 

        // Pack parameters
        package.PackData( &smsParam );

        // Lets make new routing request so new CBMI list can be delivered to
        // DOS
        TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobileSmsMessagingStoreSmspList, &package );  

        if ( KErrNone != ret )
            {
            // Message construction failed or phonet sender 
            // returned error. Complete request in CallProsessStoreSmspList
            // method
            CompleteStoreSmspList( ret );
            }
        }
            
    // All SMSP sets have successfully been stored
    else
        {
        // Notify that SMSP set was updated in the store
        CompleteNotifySmspListChange();   

        // Complete store SMSP list request to the client
        CompleteStoreSmspList( KErrNone );
        }
        
    CleanupStack::PopAndDestroy(); // aSmspList
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::CompleteStoreSmspList 
// Complete store SMSP list request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteStoreSmspList( 
    TInt aError )
    {
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsStoreSmspList );
    if ( reqHandle )
        {
        ReqCompleted( reqHandle, aError );
        }
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::NotifySmspListChange 
// This method activates notifying of SMS Parameter store events
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::NotifySmspListChange()
    {
    iReqHandleType = EMultimodeSmsNotifySmspListChange;

    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::NotifySmspListChangeCancel 
// This method cancels NotifyStoreEventCancel request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::NotifySmspListChangeCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsNotifySmspListChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    
    return KErrNone;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::CompleteNotifySmspListChange 
// This method is called when some of SMS parameter store events happen
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteNotifySmspListChange()
    {
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsNotifySmspListChange );
    if ( reqHandle )
        {
        ReqCompleted( reqHandle, KErrNone );
        }
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::GetSmsNumOfLoc 
// Get SMS number of Location on SIM card
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt16 CMmSmsTsy::GetSmsNumOfLoc()
    {
    return static_cast< TInt16 >( iSmsNumOfLoc );
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::SetSmsNumOfLoc 
// Set SMS number of Location on SIM card to member variable
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::SetSmsNumOfLoc(
    TInt aSmsNumOfLoc )
    {
    iSmsNumOfLoc = static_cast< TUint8 >( aSmsNumOfLoc );
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::GetSmsStorageTsy 
// Returns a pointer to the SMS Storage sub-session
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
CMmSmsStorageTsy* CMmSmsTsy::GetSmsStorageTsy()
    {
    return iMmSmsStorageTsy;
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::SetSmsStorageTsy 
// Sets a new pointer to the SMS Storage sub-session
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::SetSmsStorageTsy(
    CMmSmsStorageTsy* aNewPointer )
    {
    iMmSmsStorageTsy = aNewPointer;
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::ResetVariables 
// Reset used variables
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::ResetVariables()
    {
    // Resets routing activity variable. If routing is active in DOS  
    // this parameter is true.
    iServerRoutingActivity = ERoutingNotActivated;

    // Resets mobile sms receive mode to EReceiveUnstoredClientAck. Mobile 
    // sms receive mode includes the receive mode type.
    iMobileSmsReceiveMode = RMobileSmsMessaging::EReceiveUnstoredClientAck;

    // Resets receive mode change notify variables.
    iNotifyReceiveModeChangePtr = NULL;

    // Resets receive message variables.
    iReceiveMessagePduPtr = NULL;
    iReceiveMessageParamsPtr = NULL;

    // Resets sms bearer notify variables.
    iNotifySmsBearerPtr = NULL;

    // Resets send message variables.
    iSendMessageMsgAttrPckgPtr = NULL;

    // Resets get message store variables.
    iGetMessageStoreInfoPtr = NULL;

    // Resets smsp (sms parameters) variables.
    iRetSMSPSize = NULL;
    iSMSPClientId = NULL;
    iSMSPList = NULL;

    // Resets sms storage parameter variables. 
    iStoreSmspBufferPtr = NULL;
    iStoreSmspIndex = NULL;

    // Resets sms storage tsy variable.
    iMmSmsStorageTsy = NULL;

    // Resets SMS stack storage status variables
    iClientStorageFull = EFalse;


	//Set number of SMS locations on SIM card to 0
	iSmsNumOfLoc = 0;
    }

#ifdef REQHANDLE_TIMER
//---------------------------------------------------------------------------- 
// CMmSmsTsy::SetTypeOfResponse 
// Sets the type of response for a given Handle. Automatic
// mode includes an automatic response in case of non response
// from the DOS in a specified time
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::SetTypeOfResponse(
    const TInt aReqHandleType, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut( 0 );

    switch ( aReqHandleType )
        {
        // SMS specific requests
        case EMultimodeSmsAckStored:
        case EMultimodeSmsNackStored:
            timeOut = KMmSmsAckNackMessage;
            break;
        case EMultimodeSmsSendMessage:
            timeOut = KMmSmsSendMessage;
            break;
        case EMultimodeSmsSendMessageNoFdnCheck:            
            timeOut = KMmSmsSendMessageNoFdnCheckTimeOut;
            break;
        case EMultimodeSmsSendSatMessage:
            timeOut = KMmSmsSendSatMessage;
            break;
        case EMultimodeSmsResumeReception:
            timeOut = KMmSmsResumeSmsReception;
            break;
        case EMultimodeSmsGetMessageStoreInfo:
            timeOut = KMmSmsGetMessageStoreInfo;
            break;
        case EMultimodeSmsReadSmspListPhase1:
        case EMultimodeSmsStoreSmspList:
            timeOut = KMmSmsReadAndStoreSmspList;
            break;
        // SMS storage specific requests
        case EMultimodeSmsGetInfo:
            timeOut = KMmSimSmsGetInfo;
            break;
        case EMultimodeSimStSmsWriteSms:
            timeOut = KMmSimSmsWriteSms;
            break;
        case EMultimodeSimStSmsReadSms:
        case EMultimodeSimStSmsEraseSms:
            timeOut = KMmSimSmsReadDeleteSms;
            break;
        case EMultimodeSimStSmsEraseAllSms:
            timeOut = KMmSimSmsDeleteAllSms;
            break;
        case EMultimodeSimStSmsReadAllSms:
            timeOut = KMmSimSmsReadAllSms;
            break;
        // Must not use timer:
        // case EMultimodeSmsReceiveMessage:
        // case EMultimodeSmsNotifyReceiveModeChange:
        // case EMultimodeSmsNotifyMoSmsBearerChange:
        // case EMultimodeSmsNotifySmspListChange:
        // case EMultimodeSmsMemoryNotifyStoreEvent:
        default:
            // does not use timer
            iTsyReqHandleStore->SetTsyReqHandle( 
                aReqHandleType, aTsyReqHandle );
            break;
        }

    if ( timeOut > 0 )
        {
        // the timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType, aTsyReqHandle, 
            timeOut );
        }
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::Complete 
// Completes the request due timer expiration
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::Complete(
    TInt aReqHandleType, 
    TInt aError )
    {
    // All possible TSY req handle types are listed in the
    // switch case below. 
    switch( aReqHandleType )
        {
        // Cases handled with automatic completion
        // SMS specific requests
        case EMultimodeSmsAckStored:
            CompleteAckSmsStored( aError );
            break;
        case EMultimodeSmsNackStored:
            CompleteNackSmsStored( aError );
            break;
        case EMultimodeSmsSendMessage:
            CompleteSendMessage( KErrTimedOut, NULL );
            break;
        case EMultimodeSmsSendMessageNoFdnCheck:
            CompleteSendMessageNoFdnCheck( KErrTimedOut, NULL );
            break;
        case EMultimodeSmsSendSatMessage:
            CompleteSendSatMessage( KErrTimedOut );
            break;
        case EMultimodeSmsResumeReception:
            CompleteResumeSmsReception( aError );
            break;
        case EMultimodeSmsGetMessageStoreInfo:
            CompleteGetMessageStoreInfo( aError, NULL );
            break;
        case EMultimodeSmsReadSmspListPhase1:
            CompleteReadAllSmspPhase1( aError, NULL );
            break;
        case EMultimodeSmsStoreSmspList:
            CompleteStoreSmspList( aError );
            break;
        // SMS storage specific requests
        case EMultimodeSmsGetInfo:
            if ( iMmSmsStorageTsy )
                {
                iMmSmsStorageTsy->CompleteGetInfo( aError, NULL );
                }
            break;
        case EMultimodeSimStSmsReadSms:
            if ( iMmSmsStorageTsy )
                {
                iMmSmsStorageTsy->CompleteReadSms( aError, NULL );
                }
            break;
        case EMultimodeSimStSmsWriteSms:
            if ( iMmSmsStorageTsy )
                {
                iMmSmsStorageTsy->CompleteWriteSms( aError, NULL );
                }
            break;
        case EMultimodeSimStSmsEraseSms:
            if ( iMmSmsStorageTsy )
                {
                iMmSmsStorageTsy->CompleteDeleteSms( aError );
                }
            break;
        case EMultimodeSimStSmsEraseAllSms:
            if ( iMmSmsStorageTsy )
                {
                iMmSmsStorageTsy->CompleteDeleteAllSms( aError );
                }
            break;
        case EMultimodeSimStSmsReadAllSms:
            if ( iMmSmsStorageTsy )
                {
                iMmSmsStorageTsy->CompleteReadAllSmsPhase1( aError, EFalse );
                }
            break;
        // Can't use timer:
        // case EMultimodeSmsReceiveMessage:
        // case EMultimodeSmsNotifyReceiveModeChange:
        // case EMultimodeSmsNotifyMoSmsBearerChange:
        // case EMultimodeSmsNotifySmspListChange:
        // case EMultimodeSmsMemoryNotifyStoreEvent:
        default:
            ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle( 
                aReqHandleType ), aError );
            break;
        }
    }

#endif // REQHANDLE_TIMER

//----------------------------------------------------------------------------
// CMmSmsTsy::SendSatMessageL 
// SimAtkTsy can send SMS messages to the network by calling this method.
// This method can leave.
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsTsy::SendSatMessageL(
    CMmPhoneTsy* aMmPhone, 
    TDes8* aSmsTpdu, 
    TDes16* aScAddress, 
    RMobilePhone::TMobileTON* aMobileTON, 
    RMobilePhone::TMobileNPI* aMobileNPI, 
    TBool aMoreToSend ) 
    {
    TInt ret( KErrNone );
    // Deliver attributes to message handler by using 
    // TMobileSmsSendAttributesV1 class.
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 msgAttrib;
  
    msgAttrib.iFlags = ( RMobileSmsMessaging::KSmsDataFormat | 
        RMobileSmsMessaging::KGsmServiceCentre | 
        RMobileSmsMessaging::KMoreToSend );

    msgAttrib.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;

	// if service center address length  > 20 digits plus "+" sign
    if ( aScAddress->Length() > ( KMaxAmountOfDigits + 1 ) )
        {
        ret = KErrArgument;
        }
    else
        {
        msgAttrib.iGsmServiceCentre.iTelNumber.Copy( *aScAddress ); 
        msgAttrib.iGsmServiceCentre.iTypeOfNumber = *aMobileTON; 
        msgAttrib.iGsmServiceCentre.iNumberPlan = *aMobileNPI;   
    
        msgAttrib.iMore = aMoreToSend;

        // create package
        CMmDataPackage package;

        // structure for all sms parameters and data
        TSendSmsDataAndAttributes sendData;
        
        sendData.iAttributes = &msgAttrib;
        sendData.iMsgData = aSmsTpdu;
        sendData.iIpc = EMmTsySmsSendSatMessage;

        // Pack parameters
        package.PackData( &sendData );

        // send request to DOS
        ret = aMmPhone->MessageManager()->HandleRequestL( 
            EMmTsySmsSendSatMessage, &package );

        if ( KErrNone == ret )
            {  
            iReqHandleType = EMultimodeSmsSendSatMessage;
            }
        }

    return ret;
    }

//---------------------------------------------------------------------------- 
// CMmSmsTsy::ResendSms 
// Resends unsent message which is stored in iSmsSendReq
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::ResendSms()
    { 
    if ( NULL != iSmsSendReq )
        {
        // Create package
        CMmDataPackage package;

        // structure for all sms parameters and data
        TSendSmsDataAndAttributes sendData =
            iSmsSendReq->GetSmsDataAndAttributes();

        // Pack parameters
        package.PackData( &sendData );

        // send request to DOS
		TInt ret = KErrNone;
		TRAPD( trapError, ret = iMmPhone->MessageManager()->HandleRequestL( 
				sendData.iIpc, &package ); );

        if ( ( KErrNone != ret ) || ( KErrNone != trapError ) )
            {
            delete iSmsSendReq;  // Delete object
            iSmsSendReq = NULL;  // Reset pointer

            // Response for send SMS request
            if ( EMobileSmsMessagingSendMessage == sendData.iIpc  )
                {
                iSmsNoFdnCheckFlag = ESmsNoFdnCheckNotUsed;
                CompleteSendMessage( KErrGeneral, NULL );
                }
            // Response for send SAT SMS request
            else if ( EMmTsySmsSendSatMessage == sendData.iIpc  )
                {
                CompleteSendSatMessage( KErrGeneral );
                }
            // Response for send SMS NoFdnCheck request
            else if ( EMobileSmsMessagingSendMessageNoFdnCheck == 
                sendData.iIpc )                
                {
                iSmsNoFdnCheckFlag = ESmsNoFdnCheckUsed;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_RESENDSMS_1, "TSY: CMmSmsTsy::ResendSms. EMobileSmsMessagingSendMessageNoFdnCheck: %d", sendData.iIpc);
                CompleteSendMessageNoFdnCheck( KErrGeneral, NULL );
                }
            }
        else
            {
            iSmsSendReq->IncreaseSendCounter();
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmSmsTsy::IsRequestPossibleInOffLine
// Checks wether a ETel request can be performed or not while offline mode is
// enabled
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmSmsTsy::IsRequestPossibleInOffline( TInt aIpc ) const
    {
    TBool isRequestPossible ( ETrue );
    switch ( aIpc )
        {
        case EMobileSmsMessagingSendMessage: //NO
        case EMobileSmsMessagingSendMessageNoFdnCheck: //NO
            isRequestPossible = EFalse;
            break;
        // case EMobileSmsMessagingSetReceiveMode:
        // case EMobileSmsMessagingNotifyReceiveModeChange:
        // case EMobileSmsMessagingReceiveMessage:
        // case EMobileSmsMessagingAckSmsStored:
        // case EMobileSmsMessagingNackSmsStored:
        // case EMobileSmsMessagingResumeSmsReception: //request is possible 
                                                       //(will be delayed 
		                                               // and sent later, see
		                                               // ResumeSmsReception)
        // case EMobileSmsMessagingNotifyMoSmsBearerChange:
        // case EMobileSmsMessagingGetMessageStoreInfo:
        // case EMobileSmsMessagingGetSmspListPhase1:
        // case EMobileSmsMessagingNotifySmspListChange:
        // case EMobileSmsMessagingStoreSmspList:
        default:
            break;
        }
        
    return isRequestPossible;
    }   

// ---------------------------------------------------------------------------
// CMmSmsTsy::SendMessageNoFdnCheckCancel
// Cancels SendMessageNoFdnCheck request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CMmSmsTsy::SendMessageNoFdnCheckCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_SENDMESSAGENOFDNCHECKCANCEL_1, "TSY: CMmSmsTsy::SendMessageNoFdnCheckCancel" );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeSmsSendMessageNoFdnCheck );
    
    if ( EMultimodeSmsReqHandleUnknown < reqHandle ) 
        {
        ReqCompleted( aTsyReqHandle, KErrCancel );
        
        iSmsNoFdnCheckFlag = ESmsNoFdnCheckUnknown;
        }
    
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmSmsTsy::CompleteSendMessageNoFdnCheck 
// Complete SendMessage request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsTsy::CompleteSendMessageNoFdnCheck( 
    TInt aError, 
    CMmDataPackage* aDataPackage )
    {
    if ( ( KErrNone != aError ) && ( KErrTimedOut != aError ) 
        && ( KErrGsmSMSOperationNotAllowed != 
            CMmCommonStaticUtility::ExtendedErrorCode ( aError ) )
            // FDB check failed
        && ( KErrGsmSMSUnspecifiedProtocolError != 
            CMmCommonStaticUtility::ExtendedErrorCode ( aError ) ) 
        && ( KErrSatControl != 
            CMmCommonStaticUtility::ExtendedErrorCode ( aError ) ) 
        && ( !( iMmPhone->GetSatMessaging() && 
            iMmPhone->GetSatMessaging()->IsMoSmControlBySimActivated() ) ) 
        && ( NULL != iSmsSendReq ) && ( 2 >= 
            iSmsSendReq->GetSendCounter() ) ) 
        {
        // DOS returned error to send request. Message might be tried to be 
		// resent (see method ResendSms).
        // Timeout mechanism cannot access this part of code, ever.
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSTSY_COMPLETESENDMESSAGENOFDNCHECK_1, "TSY: CMmSmsTsy::CompleteSendMessageNoFdnCheck. Resend counter: %d", iSmsSendReq->GetSendCounter());
        }
    else
        {
        // This is executed
		// - if sending was successful
		// - if there are no more resending attempts
		// - if timer expires (called from the Complete of CMmSmsTsy class)

        // Delete send message entry from send array
        if ( iSmsSendReq )
            {
            delete iSmsSendReq;   // Delete object
            iSmsSendReq = NULL;   // Reset pointer
            }

        // reset req handle and complete request
        TTsyReqHandle reqHandle = iTsyReqHandleStore->
            ResetTsyReqHandle( EMultimodeSmsSendMessageNoFdnCheck );
            
        if ( EMultimodeSmsReqHandleUnknown < reqHandle ) 
            {
            // get values only if there was no error, and if return pointer to 
            // client is available
            if ( KErrNone == aError && iSendMessageMsgAttrPckgPtr )
                {
                TInt16 msgRef( 0 );
                TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg;

                aDataPackage->UnPackData( msgRef, smsMsg );

               RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg* attsPckg = 
                    reinterpret_cast< 
					RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg* >(
                    iSendMessageMsgAttrPckgPtr );
                RMobileSmsMessaging::TMobileSmsSendAttributesV1& msgAttr = 
                    ( *attsPckg )();

                msgAttr.iFlags = RMobileSmsMessaging::KMessageReference;
                msgAttr.iMsgRef = static_cast< TUint16 >( msgRef );

                if ( NULL != smsMsg.Length() )
                    {
                    msgAttr.iSubmitReport.Copy(smsMsg);
                    msgAttr.iFlags |= RMobileSmsMessaging::KGsmSubmitReport;
                    }
                }

            ReqCompleted( reqHandle, aError );
            // reset pointer to client memory
            iSendMessageMsgAttrPckgPtr = NULL;
            
            iSmsNoFdnCheckFlag = ESmsNoFdnCheckUnknown;
            }
        }

    // Resend unsent message
    ResendSms();
    }


//  End of File 
