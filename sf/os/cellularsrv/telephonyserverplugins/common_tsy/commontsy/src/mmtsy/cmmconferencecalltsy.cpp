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



//INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmconferencecalltsyTraces.h"
#endif

#include "cmmconferencecalltsy.h"
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include "MmTsy_numberOfSlots.h"
#include "CMmCommonStaticUtility.h"
#include "cmmconferencecallgsmwcdmaext.h"
#include "cmmvoicecalltsy.h"
#include "cmmcallgsmwcdmaext.h"
#include "cmmcalllist.h"
#include "cmmmessagemanagerbase.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <etelmmerr.h> // etel error codes


// ======== MEMBER FUNCTIONS ========

CMmConferenceCallTsy::CMmConferenceCallTsy():
    iReqHandleType(EMultimodeConferenceCallReqHandleUnknown)
    {
    }
    
CMmConferenceCallTsy* CMmConferenceCallTsy::NewL(
    CMmPhoneTsy* aMmPhone )
    {
    CMmConferenceCallTsy* conferenceCallTsy = NULL;
    
    if ( aMmPhone != NULL )
        {
        conferenceCallTsy  = new (ELeave) CMmConferenceCallTsy();
        CleanupClosePushL( *conferenceCallTsy );
        conferenceCallTsy->iMmPhone = aMmPhone;
        conferenceCallTsy->ConstructL();
        CleanupStack::Pop();
        }

    return conferenceCallTsy;

    }


void CMmConferenceCallTsy::ConstructL()
    {

#ifdef REQHANDLE_TIMER
    //create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this, iMmPhone,
        EMultimodeConferenceCallMaxNumOfRequests, iConferenceCallReqHandles );
#else
    //create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL(
        EMultimodeConferenceCallMaxNumOfRequests, iConferenceCallReqHandles );
#endif

    //Create conference call extension
    iMmConferenceCallExtInterface = 
                                CMmConferenceCallGsmWcdmaExt::NewL( this );

    //Initialise internal attributes...
    ResetAttributes();

    //Check if conference call creation is already possible. If possible,
    //capabilities are updated by the extension.
    CheckConferenceCapability();

    //register conference call tsy in the message manager
    iMmPhone->MessageManager()->RegisterTsyObject(
        CMmMessageManagerBase::EConferenceCallTsy, this );

    }

CMmConferenceCallTsy::~CMmConferenceCallTsy()
    {

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_DTOR_1, "TSY: CMmConferenceCallTsy::~CMmConferenceCallTsy");

    if ( iMmPhone )
        {
        // deregister tsy object from message manager
        iMmPhone->MessageManager()->DeregisterTsyObject(this);        
        //set phone tsy's pointer to CC object to NULL
        iMmPhone->SetConferenceCall( NULL );
        }

    //if conference is active, close all calls that are part of the conference 
    //call. send request using transaction id 0x00.
    if ( RMobileConferenceCall::EConferenceIdle != iStatus )
        {
        //We can't do nothing if release request fails here, because CC
        //sub-session is closing down.
		TInt trapError = KErrNone;
		TRAP( trapError, 
	        iMmConferenceCallExtInterface->HangUpL( iStatus );
			);
        }

    if ( iTsyReqHandleStore )
        {
        delete iTsyReqHandleStore;
        }

    //delete conference call extensions
    if ( iMmConferenceCallExtInterface )
        {
        delete iMmConferenceCallExtInterface;
        }
	iMmConferenceCallExtInterface = NULL;

	iMmPhone = NULL;
	iTsyReqHandleStore = NULL;
	iRetCaps = NULL;
	iRetStatus = NULL;
	iRetConferenceEventCallName = NULL;
	iRetConferenceEvent = NULL;
	iConnectedCall = NULL;
	iHoldCall = NULL;
    }
    
// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::OpenNewObjectByNameL
// This method is not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmConferenceCallTsy::OpenNewObjectByNameL(
    const TDesC& )
    {
    User::Leave( KErrNotSupported );
	//lint -e{527} "unreachable code"
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::OpenNewObjectL
// This method is not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmConferenceCallTsy::OpenNewObjectL(
    TDes& )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::ExtFunc
// Dispatches extension function requests. All functions that are not 
// part of the core function set, are routed via the ExtFunc method. 
// At the beginning of this method, packaged pointers are unpacked. Then 
// the correct function is selected using the IPC number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::ExtFunc(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
    TInt ret( KErrNone );
    TInt trapError( KErrNone );

    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==EMultimodeConferenceCallReqHandleUnknown, User::Invariant());

    //Original code continues here.
    TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ); );

    if ( trapError != KErrNone )
        {
        //error handling. Object cannot be created.
		//reset request handle to indicate the request is no longer ongoing
        iTsyReqHandleStore->FindAndResetTsyReqHandle(aTsyReqHandle);
        ReqCompleted( aTsyReqHandle, trapError );
        }
    //if return value is not KErrNone
    else if ( ret != KErrNone )
        {
        //Complete request with this error value
        ReqCompleted( aTsyReqHandle, ret );
        }
    else if ( EMultimodeConferenceCallReqHandleUnknown != iReqHandleType )
        {
        //save request handle
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse ( iReqHandleType, aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, aTsyReqHandle );
#endif
        }
    
    // We've finished with this value now. Clear it so it doesn't leak
    //  up to any other instances of this method down the call stack
    iReqHandleType = EMultimodeConferenceCallReqHandleUnknown;
    
    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::DoExtFuncL
// ExtFunc is called by the server when it has a "extended", 
// i.e. non-core ETel request for the TSY. To process a request handle, 
// request type and request data are passed to the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_DOEXTFUNCL_1, "TSY: CMmConferenceCallTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    TInt ret( KErrNotSupported );

    TAny* dataPtr = aPackage.Ptr1();

    // The request data has to extracted from TDataPackage and the TAny* 
    // pointers have to be "cast" to the expected request data type
    switch( aIpc )
        {     
        case EMobileConferenceCallGetCaps:
            ret = GetCaps( aTsyReqHandle, 
                reinterpret_cast<TUint32*>( dataPtr ) );
            break;
        case EMobileConferenceCallNotifyCapsChange:
            ret = NotifyCapsChange( reinterpret_cast<TUint32*>( dataPtr ) );
            break;
        case EMobileConferenceCallCreateConference:
            ret = CreateConferenceL( aTsyReqHandle );
            break;
        case EMobileConferenceCallAddCall:
            ret = AddCallL( aTsyReqHandle, 
				reinterpret_cast<TName*>( aPackage.Des1u() ) );
            break;
        case EMobileConferenceCallSwap:
            ret = SwapL( aTsyReqHandle );
            break;
        case EMobileConferenceCallHangUp:
            ret = HangUpL( aTsyReqHandle );
            break;
        case EMobileConferenceCallEnumerateCalls:
            ret = EnumerateCalls( aTsyReqHandle, 
                reinterpret_cast<TInt*>( dataPtr ) );
            break;
        case EMobileConferenceCallGetMobileCallInfo:
            ret = GetMobileCallInfo( aTsyReqHandle, 
                reinterpret_cast<TInt*>( dataPtr ),
                aPackage.Des2n() );
            break;
        case EMobileConferenceCallGetConferenceStatus:
            ret = GetConferenceStatus( aTsyReqHandle, 
				reinterpret_cast<RMobileConferenceCall::TMobileConferenceStatus*>(
					dataPtr ) );
            break;
        case EMobileConferenceCallNotifyConferenceStatusChange:
            ret = NotifyConferenceStatusChange(
				reinterpret_cast<RMobileConferenceCall::TMobileConferenceStatus*>(
					dataPtr ) );
            break;
        case EMobileConferenceCallNotifyConferenceEvent:
            ret = NotifyConferenceEvent(
				reinterpret_cast<RMobileConferenceCall::TMobileConferenceEvent*>(
					dataPtr ), reinterpret_cast<TName*>( aPackage.Des2u() ) );
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_DOEXTFUNCL_2, "TSY: CMmConferenceCallTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::ReqModeL
// ReqModeL is called from the server's CTelObject::ReqAnalyserL
// in order to check the type of request it has.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmConferenceCallTsy::ReqModeL(
    const TInt aIpc )
    {
    CTelObject::TReqMode ret( 0 ); // default return value
  
    switch ( aIpc )
        {
        // No Flow Control
        // All disabled
        // TSYs wishing to implement their own buffering algorithm will place
        // all requests in this category.
        // This category will also be used by the RecoverDataPort request that
        // must be passed to the TSY to indicate that it may use the
        // communications port after a loan.

        //Get methods that do not use DOS and return immediately.
        //Flow control not required.
        case EMobileConferenceCallGetCaps:
        case EMobileConferenceCallGetMobileCallInfo:
        case EMobileConferenceCallGetConferenceStatus:
        //Other methods that do not use DOS and return immediately.
        //Flow control not required.        
        case EMobileConferenceCallEnumerateCalls:

        //Methods that can propably take a long time and cannot therefore be
        //flow controlled. Solution: All these methods must check req handle
        //table before handling the request. In case that the request table
        //indicates that same method has been called and has not been
        //completed, the method should return KErrServerBusy.
        case EMobileConferenceCallHangUp:
            break;

        // Flow Controlled Services
        // KReqModeFlowControlObeyed
        // Commands that change the state of the phone, e.g. clearing the AoC
        // counter; are commands that TSY should only deal with one at a time.
 
        //Voice call related methods that should be handled only
        //one at the time.
        case EMobileConferenceCallCreateConference:
        case EMobileConferenceCallSwap:
        case EMobileConferenceCallAddCall:
            ret = KReqModeFlowControlObeyed;
            break;
        // ReqModePostImmediately
        // Requests that notify a client about a change of state, where the 
        // TSY needs to distinguish between different clients.
        //*******************************************************************
        // (TAN) DO NOT USE THIS if not absolutely certain how to handle the 
        // corresponding method in TSY. Differs from all other situations!!! 
        //********************************************************************
            //ret=KReqModeRePostImmediately;

        // KReqModeMultipleCompletionEnabled
        // (a) commands that may take some time, but which the TSY can handle
        // more than one of concurrently, or 
        // (b) notifications that the TSY does not wish to be re-posted
        // immediately, so the server does no buffering.
            //ret=KReqModeMultipleCompletionEnabled;

        // KReqModeMultipleCompletionEnabled | KReqModeFlowControlObeyed
        // A command that may take some time and which the TSY can only deal
        // with one at a time.
        //case EEtelCallSetFaxSettings:
        //    ret = KReqModeMultipleCompletionEnabled |
        //        KReqModeFlowControlObeyed;
        //    break;

        //Notifications
        //KReqModeMultipleCompletionEnabled | ReqModePostImmediately
        //Requests that notify a client about a change of state.
        //Since these requests do not require the issuing of any modem
        //commands, they do not have to obey flow control.
        //The TSY never gets more than one of these outstanding per CTelObject
        case EMobileConferenceCallNotifyCapsChange:
        case EMobileConferenceCallNotifyConferenceStatusChange:
        case EMobileConferenceCallNotifyConferenceEvent:
            ret = KReqModeMultipleCompletionEnabled | 
                                                    KReqModeRePostImmediately;
            break;
        //Cancel Requests
        //It is not necessary to include the Cancel methods in ReqModeL at all
        //The ETel server never calls ReqModeL with a Cancel IPC.

        // Other variations of return values are unusable
        default:
            // Unknown or invalid Call IPC
            User::Leave( KErrNotSupported );
            break;
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::NumberOfSlotsL
// NumberOfSlotsL is called by the server when it is registering a 
// new NOTIFICATION. It enables the TSY to tell the server 
// how many buffer slots to allocate for "repost immediately" notifications 
// that may trigger before clients collect them.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    // There is a buffer in the ETel server for each type of NOTIFICATION and
    // the size of the buffer is determined by the TSY. When the ETel server
    // discovers that a request is "repost immediately" it will ask the TSY 
    // how big a buffer it wants by calling CTelObject::NumberOfSlotsL( ).
    //
    // It is up to the TSY developer's judgement how many buffer slots this
    // method returns for each notification. If there is a danger that a
    // particular notification could trigger frequently and in rapid 
    // succession (e.g. call group change notification) then at least 10 or 20 
    // slots may be required. For a notification that triggers rarely, perhaps 
    // 1 or 2 slots is enough.
    //
    // So if the ETel server has stored a few occurrences of a particular
    // notification, when client subsequently calls that notification request,
    // the ETel server will complete the client's request with the saved data.

    TInt numberOfSlots = 1;

    switch ( aIpc ) 
        {
        case EMobileConferenceCallNotifyCapsChange:
            numberOfSlots = KMmConferenceCallCapsChangeSlots;
            break;
        case EMobileConferenceCallNotifyConferenceStatusChange:
            numberOfSlots = KMmConferenceCallStatusChangeSlots;
            break;
        case EMobileConferenceCallNotifyConferenceEvent:
            numberOfSlots = KMmConferenceCallEventSlots;
            break;
        default:
            // Unknown or invalid Call IPC
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CancelService
// CancelService is called by the server when it is 
// "cleaning-up" any still outstanding asynchronous requests before closing 
// a client's sub-session. This will happen if a client closes its R-class 
// handle without cancelling outstanding asynchronous requests. Only 
// Mobile API requests are handled here.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::CancelService(
    const TInt aIpc,
    const TTsyReqHandle aTsyReqHandle )
    {
    //When the clients close their sub-sessions (eg. by calling RPhone::Close)
    //they may not have cancelled all their outstanding asynchronous requests
    //before closing. It is up to the ETel server to clean up in this
    //situation, so the server will find the list of outstanding requests
    //related to that sub-session object and pass these outstanding IPC req.
    //numbers, one at a time, to the CancelService function in the TSY.
    
    TInt ret( KErrNone );

    switch( aIpc )
        {
        //TSY has started a request and it is not possible to then cancel
        //this request. The best thing for the TSY to do in this case is
        //to proceed as though the Cancel never happened. The server's call
        //to the TSY cancel function will return synchronously. The TSY then
        //continues to wait for the original acknowledgement and when it
        //receives it, the TSY will complete the original request.
        case EMobileConferenceCallCreateConference:
        case EMobileConferenceCallAddCall:
        case EMobileConferenceCallSwap:
        case EMobileConferenceCallHangUp:
            break;


        //Cancel methods that are not supported
        //case XXX
        //    ret = KErrNotSupported;
        //    break;

        //Notification Cancels, no special requirements.
        case EMobileConferenceCallNotifyCapsChange:
            ret = NotifyCapsChangeCancel( aTsyReqHandle );
            break;
        case EMobileConferenceCallNotifyConferenceStatusChange:
            ret = NotifyConferenceStatusChangeCancel( aTsyReqHandle );
            break;
        case EMobileConferenceCallNotifyConferenceEvent:
            ret = NotifyConferenceEventCancel( aTsyReqHandle );
            break;
        //Everything is taken care in the method implementation.
        //Just direct the request to the method.
        //case
        //  none

        //Default case
        default:
            ret = KErrNotFound;
            break;
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::Init
// Initialisation method that is called from ETel Server.
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::Init()
    {

    // None

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::RegisterNotification
// RegisterNotification is called when the server recognises 
// that this notification is being posted for the first time on this 
// sub-session object. It enables the TSY to "turn on" any regular 
// notification messages that it may receive from DOS. Currently does not 
// really do anything but returns KErrNone to ETel server in case of known 
// notification request type. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::RegisterNotification(
    const TInt aIpc )
    {
    TInt ret( KErrNotSupported );
    
    switch( aIpc )
        {
        case EMobileConferenceCallNotifyCapsChange:
        case EMobileConferenceCallNotifyConferenceStatusChange:
        case EMobileConferenceCallNotifyConferenceEvent:
            ret = KErrNone;
            break;
        default:
            // Unknown or invalid IPC
            ret = KErrNotSupported;
            break;
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::DeregisterNotification
// DeregisterNotification is called when the server recognises 
// that this notification will not be posted again because the last client 
// to have a handle on this sub-session object has just closed the handle. 
// It enables the TSY to "turn off" any regular notification messages that 
// it may receive from DOS. Currently does not really do anything but returns 
// KErrNone to ETel server in case of known notification request type.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::DeregisterNotification(
    const TInt aIpc )
    {
    TInt ret( KErrNotSupported );
    
    switch( aIpc )
        {
        case EMobileConferenceCallNotifyCapsChange:
        case EMobileConferenceCallNotifyConferenceStatusChange:
        case EMobileConferenceCallNotifyConferenceEvent:
            ret = KErrNone;
            break;
        default:
            // Unknown or invalid IPC
            ret = KErrNotSupported;
            break;
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::GetCaps
// This method returns the current capabilities of the conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::GetCaps(
    const TTsyReqHandle aTsyReqHandle,
    TUint32* aCaps )
    {
    //Conference caps
    *aCaps = iConferenceCaps;
    //Complete the request 
    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::NotifyCapsChange
// This method allows clients to be notified when the conference 
// call capabilities change.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::NotifyCapsChange(
    TUint32* aCaps )
    {
    //save pointer where to copy the changed value
    iRetCaps = aCaps;
    //save req handle type
    iReqHandleType = EMultimodeConferenceCallCapsChangeNotification;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::NotifyCapsChangeCancel
// Cancels an outstanding asynchronous NotifyCapsChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::NotifyCapsChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    //set pointer to NULL
    iRetCaps = NULL;
    //reset req handle
    iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeConferenceCallCapsChangeNotification );

    //Complete request
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CompleteNotifyCapsChange
// Completes NotifyCapsChange request when capabilities change.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::CompleteNotifyCapsChange(
    TUint32 aCaps )
    {
    //update internal capabilities
    iConferenceCaps = aCaps;

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeConferenceCallCapsChangeNotification );

    //if req handle was found
    if ( EMultimodeConferenceCallReqHandleUnknown != reqHandle )
        {
        //copy conference caps to requested location
        *iRetCaps = iConferenceCaps;
        //complete request
        ReqCompleted( reqHandle, KErrNone );
        }

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CreateConferenceL
// Allows clients to create the initial conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::CreateConferenceL(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_CREATECONFERENCEL_1, "TSY: CMmConferenceCallTsy::CreateConferenceL");
    //Store call object with status connected
	iConnectedCall = 
		iMmPhone->CallList()->GetMmCallByStatus( RMobileCall::EStatusConnected );
    
    //Store call object with status hold        
  	iHoldCall = 
  		iMmPhone->CallList()->GetMmCallByStatus( RMobileCall::EStatusHold );
  		
    //Let extension try to create conference call
    TInt ret = iMmConferenceCallExtInterface->CreateConferenceL( iStatus );

    if ( ret != KErrNone )
        {
        //request failed.
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        //save req handle type
        iReqHandleType = EMultimodeConferenceCallCreateConference;
        }

    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CompleteCreateConference
// Complete creation of conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::CompleteCreateConference(
    TInt aResult )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETECREATECONFERENCE_1, "TSY: CMmConferenceCallTsy::CompleteCreateConference");
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeConferenceCallCreateConference );

    //if req handle was available
    if ( EMultimodeConferenceCallReqHandleUnknown != reqHandle )
        {
        //complete caps after successfully creating conference
		if ( KErrNone == aResult )
            {
            //hangup and swap are now possible
            iConferenceCaps = RMobileConferenceCall::KCapsSwap |
                RMobileConferenceCall::KCapsHangUp;

            //set flag telling that caps have changed
            iCapsChanged = ETrue;
            
            if( iConnectedCall && iHoldCall )
            	{
            	TName connectedCall = iConnectedCall->CallName();
            	TName holdCall = iHoldCall->CallName();
            	
            	if( connectedCall.Length() != 0 && holdCall.Length() != 0 )
            		{
            		// Create object including call names
            		// wich were first ones in conference 
            		TName callsInConference;
            		callsInConference.Zero();
            		callsInConference.Append( connectedCall );
            		callsInConference.Append( KDelimiter );
            		callsInConference.Append( holdCall );
            		//Complete event
            		CompleteNotifyConferenceEvent( callsInConference,
            					RMobileConferenceCall::EConferenceBuilt );            		
            		}

            	}
            }

        //complete request
        ReqCompleted( reqHandle, aResult );
        }

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::AddCallL
// Adds the single voice call specified by the aCallName parameter 
// to an existing conference call
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::AddCallL(
    const TTsyReqHandle aTsyReqHandle,
    const TName* aCallName )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_ADDCALLL_1, "TSY: CMmConferenceCallTsy::AddCallL");
    //set return value to KErrNotFound
    TInt ret( KErrNotFound );
    
    //copy the name of the call to be added, will be used when request is
    //later completed
    iAddCallName = *aCallName;

    //start adding a call to conference call
    ret = iMmConferenceCallExtInterface->AddCallL( 
        &iAddCallName, iStatus, iMmPhone->CallList() );

    if ( ret != KErrNone )
        {
        //request failed, complete with error value
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        // Store the request handle
        iReqHandleType = EMultimodeConferenceCallAddCall;
        }

    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CompleteAddCall
// Complete addition of a new call to the conference call
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::CompleteAddCall(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETEADDCALL_1, "TSY: CMmConferenceCallTsy::CompleteAddCall, Result: %{TSymbianErrorCodes}", aResult );

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeConferenceCallAddCall );

    //successful case, call added to the conference call.
    if ( EMultimodeConferenceCallReqHandleUnknown != reqHandle )
        {
        if ( (KErrNone == aResult) && (iAddCallName.Length() > 0) )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETEADDCALL_2, "TSY: AddCall succeeded. AddCall request completed");
            //notify call added using the call name received with AddCall
            //request
            CompleteNotifyConferenceEvent( iAddCallName,
                RMobileConferenceCall::EConferenceCallAdded );

            //Name of the call to be added to Conference call
            iAddCallName.Zero();

			RemoveNonParticipatingCall();
            }
		else
			{
			//something special is needed here to handle a case where 
			//CompleteAddCall fails
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETEADDCALL_3, "TSY: AddCall failed");
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETEADDCALL_4, "TSY: # of CC participants: %d", iNumOfCallsInConferenceCall );
			if ( 2 == iNumOfCallsInConferenceCall )
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETEADDCALL_5, "TSY: FIX FOR A ADDCALL & MT RELEASE CASE");
				CMmCallTsy* mmCall = NULL;
				CMmCallList* callList = iMmPhone->CallList();
				CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = NULL;
				RMobileCall::TMobileCallStatus status( RMobileCall::EStatusIdle );

				//now check all calls
				for ( TInt i = 0; i < callList->GetNumberOfObjects(); i++ )
					{
					mmCall = callList->GetMmCallByIndex(i);
					status = mmCall->MobileCallStatus();

					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETEADDCALL_6, "TSY: Handling call, Call ID: %d", mmCall->CallId() );

					if ( status == RMobileCall::EStatusHold ||
						status == RMobileCall::EStatusConnected )
						{
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETEADDCALL_7, "TSY: Status was Held or Connected, reconstructing capabilities");

						// Fetch the active call extension
						mmCallGsmWcdmaExt = static_cast<CMmCallGsmWcdmaExt*>(
							mmCall->ActiveCallExtension() )
														;
						//set caps to be added;
						TUint32 caps = RMobileCall::KCapsHangUp
							| RMobileCall::KCapsJoin 
							| RMobileCall::KCapsTransfer
							| RMobileCall::KCapsSwap;
						//add Join caps
						mmCallGsmWcdmaExt->AddGSMCallCaps( caps );
						//complete caps change notification
						mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );
						}
					}
				//Clear conference call
				//Check if conference call is possible to build
				CheckConferenceCapability();
				//rebuild capabilities of individual calls
				}
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETEADDCALL_8, "TSY: normal AddCall failure");
				}
			}
        ReqCompleted( reqHandle, aResult );
        }

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::RemoveNonParticipatingCall()
// Removes call wich is not participant in conference 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//

void CMmConferenceCallTsy::RemoveNonParticipatingCall()
	{

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_REMOVENONPARTICIPATINGCALL_1, "TSY: CMmConferenceCallTsy::RemoveNonParticipatingCall");

	RMobileCall::TMobileCallStatus statusOfCallsInConference(
		RMobileCall::EStatusConnected);
	CMmCallTsy* mmCall = NULL;
	CMmCallList* callList = iMmPhone->CallList();
	RMobileCall::TMobileCallStatus status (RMobileCall::EStatusIdle);
	TInt numberOfCallInConference(0);
	TBool callReleased = EFalse;

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_REMOVENONPARTICIPATINGCALL_2, "TSY: Conference status: %d", iStatus);

	if ( iStatus == RMobileConferenceCall::EConferenceHold )
		{
		statusOfCallsInConference = RMobileCall::EStatusHold;
		}

	//now check all calls
	for ( TInt c = 0; c < callList->GetNumberOfObjects(); c++ )
		{
		mmCall = callList->GetMmCallByIndex(c);
		status = mmCall->MobileCallStatus();

		if ( mmCall->IsPartOfConference() &&
			status != statusOfCallsInConference )
			{
			//send removed event
			CompleteNotifyConferenceEvent( 
				mmCall->CallName(), 
				RMobileConferenceCall::EConferenceCallRemoved );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_REMOVENONPARTICIPATINGCALL_3, "TSY: Call removed from conference." );
			}
		else if ( status == statusOfCallsInConference )
			{
			numberOfCallInConference++;
			
			}
		if ( mmCall->IsRemoteReleasedCall() )
			{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_REMOVENONPARTICIPATINGCALL_4, "TSY: One call is remote released");
			callReleased = ETrue;
			}
		}
		if ( numberOfCallInConference == 2 &&
			iNumOfCallsInConferenceCall == 3 && callReleased )
			{
			iNumOfCallsInConferenceCall--;
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_REMOVENONPARTICIPATINGCALL_5, "TSY: number of calls in conference decremented");
		}

	}


// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::SwapL
// Allows a client to switch a conference call between "Active" and
// "Hold" states.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::SwapL(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_SWAPL_1, "TSY: CMmConferenceCallTsy::SwapL");
    //direct request to extension
    TInt ret = iMmConferenceCallExtInterface->SwapL(
        iStatus, iMmPhone->CallList() );

    if ( ret != KErrNone )
        {
        //complete request with error value
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        // Store the request handle
        iReqHandleType = EMultimodeConferenceCallSwap;
        }

    return KErrNone;

    }
        
// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CompleteSwap
// Complete the ongoing swap request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::CompleteSwap(
    TInt aResult )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETESWAP_1, "TSY: CMmConferenceCallTsy::CompleteSwap");
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeConferenceCallSwap );

    if ( EMultimodeConferenceCallReqHandleUnknown != reqHandle )
        {
#ifdef USING_CTSY_DISPATCHER
        if (aResult == KErrNone) // Conference event should only be completed if swap was successful
        	{
#endif // USING_CTSY_DISPATCHER
        TName emptyBuf;
        emptyBuf.Zero();
        CompleteNotifyConferenceEvent( emptyBuf,
            				RMobileConferenceCall::EConferenceSwapped );
#ifdef USING_CTSY_DISPATCHER
        	}
#endif // USING_CTSY_DISPATCHER         
        //complete request
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::HangUpL
// Terminates the whole conference call (all calls participating in the 
// conference call)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::HangUpL(
    const TTsyReqHandle aTsyReqHandle )
    {
	TInt ret( KErrServerBusy );

	// Check if request handle already exists
    TTsyReqHandle conferenceCallHangUpHandle = iTsyReqHandleStore->
        GetTsyReqHandle( EMultimodeConferenceCallHangUp );

    if ( conferenceCallHangUpHandle != EMultimodeConferenceCallReqHandleUnknown )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
	{
        //Close all calls that are part of the conference call. Held call 
        //is not released if there is one...
        ret = iMmConferenceCallExtInterface->HangUpL( iStatus );

        //if request failed
        if ( ret != KErrNone )
            {
            //complete request with error value
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            //save last request type
            iReqHandleType = EMultimodeConferenceCallHangUp;
            }
	}
	
	//Reset pointer
    iHoldCall = NULL;
	//Reset pointer
	iConnectedCall = NULL;

    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CompleteHangUp
// Completes call hangup.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::CompleteHangUp(
    TInt aResult )
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeConferenceCallHangUp );

    if ( EMultimodeConferenceCallReqHandleUnknown != reqHandle )
        {
        // ignore KErrGsmReleaseByUser, which means that the call ended
        // because the local user released the call, and must be treated
        // as a normal return code from the LTSY
        TInt ret = ( (aResult == KErrGsmReleaseByUser) ? KErrNone : aResult );
        
        //complete request
        ReqCompleted( reqHandle, ret );
        }

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::EnumerateCalls
// This method returns the number of calls that are currently 
// part of the conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::EnumerateCalls(
    const TTsyReqHandle aTsyReqHandle,
    TInt* aCount )
    {
    *aCount = iNumOfCallsInConferenceCall;

    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::GetMobileCallInfo
// This method returns a current snapshot of the call 
// information associated with the call specified by the aIndex parameter.
// NOTE! Conference call has to be in active state (Held or Active) before
// this method can return anything. Idle conference call is not considered 
// to contain any calls.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::GetMobileCallInfo(
    const TTsyReqHandle aTsyReqHandle,
    TInt* aIndex,
    TDes8* aCallInfo )
    {
    TInt ret( KErrNotReady );

    if ( RMobileConferenceCall::EConferenceIdle != iStatus )
        {
        //Let extension get the information
        ret = iMmConferenceCallExtInterface->GetMobileCallInfo( 
            aIndex, aCallInfo, iMmPhone->CallList(), iStatus );
        }

    ReqCompleted( aTsyReqHandle, ret );
    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::GetConferenceStatus
// Allows clients to retrieve the current status of the conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::GetConferenceStatus(
    const TTsyReqHandle aTsyReqHandle,
    RMobileConferenceCall::TMobileConferenceStatus* aStatus )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_GETCONFERENCESTATUS_1, "TSY: CMmConferenceCallTsy::GetConferenceStatus. iStatus:%d", iStatus );
    *aStatus = iStatus;

    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::Status
// This function returns conference call status. For internal use.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RMobileConferenceCall::TMobileConferenceStatus CMmConferenceCallTsy::Status() const
    {

    //return conference call status
    return iStatus;
 
    }
    
// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::NotifyConferenceStatusChange
// Allows clients to be notified of a change in the status of a 
// conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::NotifyConferenceStatusChange(
    RMobileConferenceCall::TMobileConferenceStatus* aStatus )
    {
    iRetStatus = aStatus;
    iReqHandleType = EMultimodeConferenceCallStatusChangeNotification;

    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::NotifyConferenceStatusChangeCancel
// Cancels an outstanding asynchronous NotifyConferenceStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::NotifyConferenceStatusChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetStatus = NULL;

    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeConferenceCallStatusChangeNotification );

    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CompleteNotifyConferenceStatusChange
// Completes conference status change notification. Extension
// should already have updated status so this method retrieves the new 
// status from the extension and does the actions required by new and old 
// statuses.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::CompleteNotifyConferenceStatusChange()
    {
    TBool conferenceStatusChanged( EFalse );

    RMobileConferenceCall::TMobileConferenceStatus oldStatus = iStatus;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETENOTIFYCONFERENCESTATUSCHANGE_1, "TSY: CMmConferenceCallTsy::CompleteNotifyConferenceStatusChange. oldStatus:%d", oldStatus );
    iMmConferenceCallExtInterface->GetNewStatus( &iStatus );

    switch( iStatus )
        {
        //new status is EConferenceIdle
        case RMobileConferenceCall::EConferenceIdle:
            //This might be caused by HangUp
            CompleteHangUp( KErrNone );
            //caps have changed
            iCapsChanged = ETrue;
            //status has changed
            conferenceStatusChanged = ETrue;
            //reset internal attributes
            ResetAttributes();
            break;
        //new status is EConferenceActive
        case RMobileConferenceCall::EConferenceActive:
            // if previous status was EConferenceIdle
            if ( oldStatus == RMobileConferenceCall::EConferenceIdle )
                {
                //is CreateConference request pending
                if ( iTsyReqHandleStore->GetTsyReqHandle( 
                            EMultimodeConferenceCallCreateConference ) > 0 )
                    {
                    //CreateConference handling
                    CompleteCreateConference( KErrNone );
                    }
                //status has changed
                conferenceStatusChanged = ETrue;
                }
            //if previous state was EConferenceHold
            else if ( oldStatus == RMobileConferenceCall::EConferenceHold )
                {
                //is AddCall request pending
                if ( iTsyReqHandleStore->
                    GetTsyReqHandle( EMultimodeConferenceCallAddCall ) > 0 )
                    {
                    //AddCall handling
                    CompleteAddCall ( KErrNone );
                    }
                //is Swap request pending.
                else if ( iTsyReqHandleStore->
                    GetTsyReqHandle( EMultimodeConferenceCallSwap ) > 0 )
                    {
                    //resuming held conference call.
                    CompleteSwap ( KErrNone );
                    }
                //status has changed
                conferenceStatusChanged = ETrue;
                }
            break;
        //new status is EConferenceHold
        case RMobileConferenceCall::EConferenceHold:
            //Complete Swap
            CompleteSwap ( KErrNone );
            //status has changed
            conferenceStatusChanged = ETrue;
            break;
        default:
            //nothing to do
            break;
        }

    //get TSY req handle. 
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodeConferenceCallStatusChangeNotification );

    if ( ( EMultimodeConferenceCallReqHandleUnknown != reqHandle ) &&
        conferenceStatusChanged )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETENOTIFYCONFERENCESTATUSCHANGE_2, "TSY: CMmConferenceCallTsy::CompleteNotifyConferenceStatusChange. Cur. iStatus:%d", iStatus );
        //reset req handle.
        iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodeConferenceCallStatusChangeNotification );
        *iRetStatus = iStatus;            
        ReqCompleted( reqHandle, KErrNone );
        }

    if ( iCapsChanged )
        {
        //if conference caps are not 0, notify it
        if ( 0 != iConferenceCaps )
            {
            CompleteNotifyCapsChange( iConferenceCaps );
            }
        else
            {
            //Otherwise we still have to check that is Conference creation
            //now possible
            CheckConferenceCapability();
            //If CreateConference is now possible, iConferenceCaps is not 0.
            //Completion has already been done.
            if ( 0 == iConferenceCaps )
                {
                //Otherwise we have to complete the caps change with 0 value.
                CompleteNotifyCapsChange( iConferenceCaps );
                }
            }

        iCapsChanged = EFalse;
        }

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::NotifyConferenceEvent
// Allows a client to be notified when a conference call event occurs.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::NotifyConferenceEvent(
    RMobileConferenceCall::TMobileConferenceEvent* aEvent,
    TName* aCallName )
    {
    iRetConferenceEventCallName = aCallName; 
    iRetConferenceEvent = aEvent;

    iReqHandleType = EMultimodeConferenceCallConferenceEventNotification;

    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::NotifyConferenceEventCancel
// Cancels an outstanding asynchronous NotifyConferenceEvent request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::NotifyConferenceEventCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetConferenceEventCallName = NULL; 
    iRetConferenceEvent = NULL;

    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeConferenceCallConferenceEventNotification );

    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CompleteNotifyConferenceEvent
// Completes conference event notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::CompleteNotifyConferenceEvent(
    const TName& aName,
    RMobileConferenceCall::TMobileConferenceEvent aEvent )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_COMPLETENOTIFYCONFERENCEEVENT_1, "TSY: CMmConferenceCallTsy::CompleteNotifyConferenceEvent.aEvent:%d", aEvent );
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeConferenceCallConferenceEventNotification );

    //if req handle was set
    if ( EMultimodeConferenceCallReqHandleUnknown != reqHandle )
        {    
        //copy name of the call to which the event happened to the requested
        //address
        iRetConferenceEventCallName->Copy( aName );
        //copy event to the requested address
        *iRetConferenceEvent = aEvent;
   
        ReqCompleted( reqHandle, KErrNone );
        }

    //if event was add
    if ( aEvent == RMobileConferenceCall::EConferenceCallAdded )
        {

		// inform the call object that it is now part of the conference
		CMmCallTsy* addedCall = iMmPhone->CallList()-> GetMmCallByName(
            &aName );
		if ( addedCall )
			{
			addedCall->SetPartOfConference( ETrue );
			}

        //increment iNumOfCallsInConferenceCall
        iNumOfCallsInConferenceCall++;
        }

    //if event was removed
    else if ( aEvent == RMobileConferenceCall::EConferenceCallRemoved )
        {

		// inform the call object that it is not part of the conference anymore
		CMmCallTsy* removedCall = iMmPhone->CallList()->GetMmCallByName(
            &aName );
		if ( removedCall )
			{
			removedCall->SetPartOfConference( EFalse );
			}

        //decrement iNumOfCallsInConferenceCall
        iNumOfCallsInConferenceCall--;
        }

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::IsGoOneToOneSupported
// Checks if given call can be splitted from the conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmConferenceCallTsy::IsGoOneToOneSupported(
    TInt aCallId )
    {
    TBool supported( EFalse );

    //The action is only valid if the MS is in GSM mode, the selected call 
    //is a voice call and it is a member of an ongoing conference call.
    TInt ret = iMmConferenceCallExtInterface->IsGoOneToOneSupported( 
        aCallId, iStatus, iMmPhone->CallList() );

    if ( KErrNone == ret )
        {
        //GoOneToOne is supported to this call
        supported = ETrue;
        }

    return supported;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::GetActiveConferenceCallExtension
// Returns a pointer to the active conference call extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmConferenceCallExtInterface* CMmConferenceCallTsy::GetActiveConferenceCallExtension()
    {

    return iMmConferenceCallExtInterface;
    
    }


// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::SetActiveConferenceCallExtension
// Sets active Conference call extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::SetActiveConferenceCallExtension(
    CMmConferenceCallExtInterface* aMmConferenceCallExt )
    {
    // Used if active conference call extension pointer is NULL
    TInt ret( KErrGeneral );

    //if not null
    if ( aMmConferenceCallExt )
        {
        //set pointer
        iMmConferenceCallExtInterface = aMmConferenceCallExt;
        ret = KErrNone;
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CheckConferenceCapability
// Checks if conference call creation is possible. The request 
// is directed to the active extension which can make the decision.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::CheckConferenceCapability()
    {

    //Let extension decide if conference call creation is possible. 
    iMmConferenceCallExtInterface->CheckConferenceCapability( 
        iStatus, iMmPhone->CallList() );

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::NumberOfCallsInConference
// This method returns the number of calls that are currently part of 
// the conference call. For internal use.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallTsy::NumberOfCallsInConference() const
    {

    return iNumOfCallsInConferenceCall;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::ServiceRequested
// Returns ETrue if this service is pending on conference call object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmConferenceCallTsy::ServiceRequested(
    CMmConferenceCallTsy::TConferenceCallRequestTypes aReqType )
    {
    TBool ret( EFalse );

    //if req handle is found from req handle store
    if ( CMmConferenceCallTsy::EMultimodeConferenceCallReqHandleUnknown != 
        iTsyReqHandleStore->GetTsyReqHandle( aReqType ) )
        {
        //set value to ETrue -> request is pending
        ret = ETrue;
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::CallStatusChanged
// After the status of a single call has changed, extension decides if
// the status of the conference call should be changed as well.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::CallStatusChanged(
    RMobileCall::TMobileCallStatus aNewStatus,
    TInt aCallId )
    {
	// only releasing, holding and connecting a call can cause a change
	// of conference status
	if ( ( RMobileCall::EStatusIdle == aNewStatus ) ||
		 ( RMobileCall::EStatusHold == aNewStatus ) ||
		 ( RMobileCall::EStatusConnected == aNewStatus ) )
		{
		TBool conferenceStatusChanged = iMmConferenceCallExtInterface->
			CallStatusChanged( aNewStatus, aCallId );

		if ( conferenceStatusChanged )
			{
			CompleteNotifyConferenceStatusChange();
			}
		}

	//conference call creation might be possible, check it.
	CheckConferenceCapability();

	}

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::Phone
// Returns pointer to the Multimode Phone object. For internal use.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneTsy* CMmConferenceCallTsy::Phone()
    {

    return iMmPhone;

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::ResetAttributes
// Resets internal attributes of Conference call object. Calls also protocol 
// extension to do the same.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::ResetAttributes()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_RESETATTRIBUTES_1, "TSY: CMmConferenceCallTsy::ResetAttributes." );
    //Name of the call to be added to Conference call
    iAddCallName.Zero();
    //Conference call capabilities
    iConferenceCaps = 0;
    //Number of calls in conference call
    iNumOfCallsInConferenceCall = 0;
    //Conference call status
    iStatus = RMobileConferenceCall::EConferenceIdle;
    //Reset pointer
    iHoldCall = NULL;
	//Reset pointer
	iConnectedCall = NULL;

    if ( 0 != iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeConferenceCallAddCall ) )
        {
		TInt error = CMmCommonStaticUtility::EpocErrorCode(
            KErrEtelCallNotActive, KErrMMEtelCallTerminated );
        CompleteAddCall( error );
        }
    if ( 0 != iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeConferenceCallCreateConference ) )
        {
		TInt error = CMmCommonStaticUtility::EpocErrorCode(
            KErrEtelCallNotActive, KErrMMEtelCallTerminated );
        CompleteCreateConference( error );
        }
    if ( 0 != iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeConferenceCallSwap ) )
        {
		TInt error = CMmCommonStaticUtility::EpocErrorCode(
            KErrEtelCallNotActive, KErrMMEtelCallTerminated );
        CompleteSwap( error );
        }

    //Direct reset also to the extension
    iMmConferenceCallExtInterface->ResetAttributes();

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::Caps
// This function returns conference call capabilities. For internal use.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUint32 CMmConferenceCallTsy::Caps() const
    {

    //return conference call caps
    return iConferenceCaps;
 
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::SetTypeOfResponse
// Sets the type of response for a given Handle. Automatic
// mode includes an automatic response in case of no response
// from the DOS in a specified time
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::SetTypeOfResponse(
    const TInt aReqHandleType,
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut( 0 );

    switch ( aReqHandleType )
        {
        case EMultimodeConferenceCallCreateConference:
            timeOut = KMmConferenceCallCreateConferenceTimeOut;
            break;
        case EMultimodeConferenceCallAddCall:
            timeOut = KMmConferenceCallAddCallTimeOut;
            break;
        case EMultimodeConferenceCallSwap:
            timeOut = KMmConferenceCallSwapTimeOut;
            break;
        case EMultimodeConferenceCallHangUp:
            timeOut = KMmConferenceCallHangUpTimeOut;
            break;
        //Can't use timer:
        // - all notifications
            //case EMultimodeConferenceCallStatusChangeNotification: 
            //case EMultimodeConferenceCallCapsChangeNotification:
            //case EMultimodeConferenceCallConferenceEventNotification:
        default:
            iTsyReqHandleStore->SetTsyReqHandle( 
                aReqHandleType, aTsyReqHandle );
            break;
        }

    //if timeout is requested
    if ( timeOut > 0 )
        {
        //set time out
        iTsyReqHandleStore->SetTsyReqHandle(
            aReqHandleType, aTsyReqHandle, timeOut );
        }

    }

// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::Complete
// Completes the request due timer expiration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::Complete(
    TInt aReqHandleType,
    TInt aError )
    {
    //All possible TSY req handle types of Conference call are listed in the
    //switch case below. 

    switch( aReqHandleType )
        {
        case EMultimodeConferenceCallCreateConference:
			CompleteCreateConference( aError );
            break;
        case EMultimodeConferenceCallAddCall:
			CompleteAddCall( aError );
            break;
        case EMultimodeConferenceCallSwap:
			CompleteSwap( aError );
            break;
        case EMultimodeConferenceCallHangUp:
			CompleteHangUp( aError );
            break;
        //Can't use timer:
        // - all notifications
            //case EMultimodeConferenceCallStatusChangeNotification: 
            //case EMultimodeConferenceCallCapsChangeNotification:
            //case EMultimodeConferenceCallConferenceEventNotification:
        default:
            ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle( 
                aReqHandleType ), aError );
            break;
        }
    
    }
#endif // REQHANDLE_TIMER

#ifdef TF_LOGGING_ENABLED
// ---------------------------------------------------------------------------
// CMmConferenceCallTsy::ReqCompleted
// Overrides CTelObject::ReqCompleted for test logging purposes. 
// It prints the aTsyReqHandle and aError variable in the log file and then 
// calls CTelObject::ReqCompleted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallTsy::ReqCompleted(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aError )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_REQCOMPLETED_1, "TSY: CMmConferenceCallTsy::ReqCompleted. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", aTsyReqHandle, aError, this);

    //call original ReqCompleted from CTelObject
    CTelObject::ReqCompleted( aTsyReqHandle, aError );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLTSY_REQCOMPLETED_2, "<-- TSY: CMmConferenceCallTsy::ReqCompleted" );
    }
#endif // TF_LOGGING_ENABLED


//  End of File  
