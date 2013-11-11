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
#include "cmmlinetsyTraces.h"
#endif

#include "cmmlinetsy.h"
#include "cmmphonetsy.h"
#include "cmmcalltsy.h"
#include "cmmlinelist.h"
#include "cmmcalllist.h"
#include "cmmtsyreqhandlestore.h"
#include "MmTsy_numberOfSlots.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <et_struct.h>


// ======== MEMBER FUNCTIONS ========

CMmLineTsy::CMmLineTsy()
    {
    }

void CMmLineTsy::ConstructL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_CONSTRUCTL_1, "TSY: CMmLineTsy::ConstructL");
    //Initialise miscellaneous internal attributes
    InitInternalAttributesL();

    // Create and store a Call Object for incoming calls.
    TInt ret = CreateCallObjectForIncomingCall();
    
    if ( KErrNone != ret )
        {
        //if this fails, incoming calls cannot be called -> leave
        User::Leave( KErrNoMemory );
        }

    //create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        EMultimodeLineMaxNumOfRequests, iLineReqHandles );
    }

CMmLineTsy::~CMmLineTsy()
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_DTOR_1, "TSY: CMmLineTsy::~CMmLineTsy. Line name: %S", iLineName);

    //delete req handle store
    delete iTsyReqHandleStore;

    //delete incoming call object
    if ( iCallForIncomingCall )
        {
        iCallForIncomingCall->Close();
        }

    // Remove all calls still open from this line
    iMmPhone->CallList()->RemoveCallsByLine( &iLineName );

    //Inform phone that this line has been removed.
    iMmPhone->RemoveLine( iLineName );

    iMmPhone = NULL;
    iCallForIncomingCall = NULL;
    iRetLineStatus = NULL;
    iRetCaps = NULL;
    iRetIncomingCallName = NULL;
    iRetCallAdded = NULL;
    iRetMobileLineStatus = NULL;
    iTsyReqHandleStore = NULL;
    iMessageManager = NULL;
    }


// ---------------------------------------------------------------------------
// CMmLineTsy::OpenNewObjectByNameL
// Returns a pointer to an existing call. The TSY classes must not use this 
// method. This method is called when the client uses RCall::OpenExistingCall 
// method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmLineTsy::OpenNewObjectByNameL(
    const TDesC& aName )
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_OPENNEWOBJECTBYNAMEL_1, "TSY: CMmLineTsy::OpenNewObjectByNameL %S", aName);

    TName mmCallName( aName );
    CMmCallTsy* mmCall = iMmPhone->CallList()->GetMmCallByName( &mmCallName );

    //if not found, Leave...
    if ( NULL == mmCall )
        {
        User::Leave( KErrNotFound );
        }
    else
        {
        //Update iUnownedCallObject flag
        mmCall->SetUnownedCallObjectFlag( EFalse );
        }

    return mmCall;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::InitInternalAttributesL
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmLineTsy::InitInternalAttributesL()
    {
    // Set object attributes
    iLineStatus = RCall::EStatusIdle;    
    iMobileLineStatus = RMobileCall::EStatusIdle;

    //reset the name for answering call
    iNameOfCallForAnswering.Zero();

    iNumCalls = 0;    // Number of calls created from line;
    iCallSequenceNumber = 0;    // The sequence number for the calls
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::CallObjectForIncomingCall
// This method returns a Call object that is used for a new incoming call. 
// If there are no incoming calls this method creates a new call object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmLineTsy::CallObjectForIncomingCall()
    {
    CMmCallTsy* callForIncomingCall = NULL;

    if ( iCallForIncomingCall == NULL )
        {
        CreateCallObjectForIncomingCall();
        }

    callForIncomingCall = iCallForIncomingCall;

    return callForIncomingCall;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::ReqModeL
// ReqModeL is called from the server's CTelObject::ReqAnalyserL in order to 
// check the type of request it has
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmLineTsy::ReqModeL(
    const TInt aIpc )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_REQMODEL_1, "TSY: CMmLineTsy::ReqModeL IPC:%{TIPCNamesList}",aIpc);
    
    CTelObject::TReqMode ret( 0 );    // default return value
    
    switch( aIpc )
        {
        // Non-flow control requests
        // All disabled
        // TSYs wishing to implement their own buffering algorithm will place
        // all requests in this category. This category will also be used by
        // the RecoverDataPort request that must be passed to the TSY to
        // indicate that it may use the communications port after a loan.
                
        //Get methods that do not use DOS and return immediately.
        //Flow control not required.
        case EEtelLineGetStatus:
        case EEtelLineGetCaps:
        case EEtelLineGetCallInfo:
        case EEtelLineGetInfo:
        case EEtelLineGetHookStatus:
        case EMobileLineGetMobileLineStatus:

        //Other methods that do not use DOS and return immediately.
        //Flow control not required.
        case EEtelLineEnumerateCall:

        //Methods that can propably take a long time and cannot therefore be
        //flow controlled. Solution: All these methods must check req handle
        //table before handling the request. In case that the request table
        //indicates that same method has been called and has not been
        //completed, the method should return KErrServerBusy.
            break;
        // Flow Controlled Services
        // KReqModeFlowControlObeyed
        // Commands that change the state of the phone, e.g. clearing the AoC
        // counter; are commands that the TSY should only deal with one at
        // a time.
        
        //    ret = KReqModeFlowControlObeyed;
        //    break;

        // ReqModePostImmediately
        // Requests that notify a client about a change of state, where the
        // TSY needs to distinguish between different clients.
            //ret=KReqModeRePostImmediately;
            //break;

        // KReqModeMultipleCompletionEnabled
        // (a) commands that may take some time, but which the TSY can handle
        // more than one of concurrently, or 
        // (b) notifications that the TSY does not wish to be re-posted
        // immediately, so the server does no buffering.
            //ret=KReqModeMultipleCompletionEnabled;
            //break;

        // KReqModeMultipleCompletionEnabled | KReqModeFlowControlObeyed
        // A command that may take some time and which the TSY can only deal 
        // with one at a time.
            //ret = KReqModeMultipleCompletionEnabled |
            //    KReqModeFlowControlObeyed;
            //break;

        // Notification Requests
        // KReqModeMultipleCompletionEnabled | ReqModePostImmediately
        // Requests that notify a client about a change of state.
        // Since these requests do not require the issuing of any modem
        // commands, they do not have to obey flow control. The TSY never
        // gets more than one of these outstanding per CTelObject.
        case EEtelLineNotifyStatusChange:
        case EEtelLineNotifyCallAdded:
        case EETelLineCapsChangeNotification:
        case EEtelLineNotifyHookChange:
        case EEtelLineNotifyIncomingCall:
        case EMobileLineNotifyMobileLineStatusChange:
            ret = KReqModeMultipleCompletionEnabled |
                KReqModeRePostImmediately;
            break;
        // Cancel Requests
        //It is not necessary to include the Cancel methods in ReqModeL at all
        //The ETel server never calls ReqModeL with a Cancel IPC.

        //Other variations of return values are unusable

        // Default: Call CLineBase's ReqModeL.
        default:
            ret = CLineBase::ReqModeL( aIpc );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::ExtFunc
// TRAP's all CMmLineTsy related MM API requests in cases that they fail. This 
// method functions only as a centralized TRAP for the DoExtFuncL method that 
// does the actual mapping of IPC number to TSY method call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::ExtFunc(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
    TInt ret( KErrNone );
    TInt trapError( KErrNone );

    //reset last tsy request type
    iReqHandleType = EMultimodeLineReqHandleUnknown;

    //Original code continues here.
    TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ); );

    if ( trapError != KErrNone )
        {
        // Object cannot be created.
		// Reset request handle to indicate the request is no longer ongoing
        iTsyReqHandleStore->FindAndResetTsyReqHandle( aTsyReqHandle );
        ReqCompleted( aTsyReqHandle, trapError );
        }
    else if ( ret != KErrNone )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else if ( EMultimodeLineReqHandleUnknown != iReqHandleType )
        {
        //save request handle
        iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, aTsyReqHandle );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::DoExtFuncL
// ExtFunc is called by the server when it has a "extended", i.e. non-core  
// ETel request for the TSY. To process a request handle, request type and 
// request  data are passed to the TSY. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_DOEXTFUNCL_1, "TSY: CMmLineTsy::DoExtFuncL IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    TInt ret ( KErrNone );

    TAny* dataPtr = aPackage.Ptr1();

    // The request data has to extracted from TDataPackage and the TAny* 
    // pointers have to be "cast" to the expected request data type
    switch ( aIpc )
        {
      // Mobile Line Status
        // Get Mobile Line Status
        case EMobileLineGetMobileLineStatus:
            ret = GetMobileLineStatus( aTsyReqHandle,
            REINTERPRET_CAST( RMobileCall::TMobileCallStatus*, dataPtr ) );
            break;
        //Notify Change of Mobile Line Status
        case EMobileLineNotifyMobileLineStatusChange:
            ret = NotifyMobileLineStatusChange(
            REINTERPRET_CAST( RMobileCall::TMobileCallStatus*, dataPtr ) );
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_DOEXTFUNCL_2, "TSY: CMmLineTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::CreateNewCallName
// Creates a new name for the call, which is the name of the line followed by 
// a call sequence number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::CreateNewCallName(
    TDes& aNewName )
    {
    // buffer for the name
    TBuf<KMaxName> buf;
    // append line name first
    buf.Append( iLineName );
    // append call sequence number next
    buf.AppendNum( IncrementCallSequenceNumber() );
    // copy the created name
    aNewName.Copy( buf );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::IncrementCallSequenceNumber
// Returns the sequence number of the call created from this line. This 
// sequence number will be resetted when this line object is deleted. TInt 
// allows the value to increment to the value 2^32. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUint CMmLineTsy::IncrementCallSequenceNumber()
    {
    // increment call sequence number
    iCallSequenceNumber++;
    return iCallSequenceNumber;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::EnumerateCall
// This CORE API method returns the number of calls opened from a line. The 
// number of calls will be stored in the aCount pointer.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::EnumerateCall(
    const TTsyReqHandle aTsyReqHandle,
    TInt* aCount )
    {
    *aCount = iNumCalls;
    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::DecrementNumberOfCalls
// Decrements number of calls opened from a line by one.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmLineTsy::DecrementNumberOfCalls()
    {
    if ( iNumCalls > 0 )
        {
        iNumCalls--;
        }
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyStatusChange
// This CORE API method provides notification about a change in the line 
// status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyStatusChange(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TStatus* aLineStatus )
    {
    iRetLineStatus = aLineStatus;
    iTsyReqHandleStore->SetTsyReqHandle( EMultimodeLineNotifyStatusChange,
        aTsyReqHandle );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyStatusChangeCancel
// This CORE API method cancels an outstanding line status change notification 
// request, placed using the NotifyStatusChange() method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyStatusChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetLineStatus = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeLineNotifyStatusChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::CompleteNotifyStatusChange
// This method is used to notify to the client about the line status change. 
// This method should only be called by the CMmCallTsy's 
// CompleteNotifyStatusChange method to guaratee that this is functioning 
// right. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmLineTsy::CompleteNotifyStatusChange()
    {
    TInt ret( KErrNone );
    TBool coreStatusChanged( EFalse );
    TBool mobileStatusChanged( EFalse );
    RCall::TStatus tempCoreStatus( RCall::EStatusIdle );
    RMobileCall::TMobileCallStatus tempMobileStatus( 
        RMobileCall::EStatusIdle );
    TInt numberOfObjectsInCallList = iMmPhone->CallList()->
        GetNumberOfObjects();

    for ( TInt i = 0; i < numberOfObjectsInCallList; i++ )
        {
        CMmCallTsy* mmCall = iMmPhone->CallList()->GetMmCallByIndex(i);
        
        //check that the call object has been opened from this line
        if ( mmCall->Line() == this )
            {
            RMobileCall::TMobileCallStatus mobileStatus 
                = mmCall->MobileCallStatus();
            switch( mobileStatus )
                {
                case RMobileCall::EStatusUnknown:
                    tempCoreStatus = RCall::EStatusUnknown;
                    tempMobileStatus = RMobileCall::EStatusUnknown;
                    //get out of loop, resulting status is unknown...
                    i = numberOfObjectsInCallList;
                    break;
                case RMobileCall::EStatusConnected:
                    tempCoreStatus = RCall::EStatusConnected;
                    tempMobileStatus = RMobileCall::EStatusConnected;
                    //get out of loop, resulting status is unknown...
                    i = numberOfObjectsInCallList;
                    break;
                case RMobileCall::EStatusRinging:
                    //update core status
                    if ( tempCoreStatus == RCall::EStatusIdle )
                        {
                        tempCoreStatus = RCall::EStatusRinging;
                        }
                    //update mobile status
                    if ( tempMobileStatus == RMobileCall::EStatusIdle || 
                         tempMobileStatus == RMobileCall::EStatusHold)
                        {
                        tempMobileStatus = mobileStatus;
                        }
                    break;
                case RMobileCall::EStatusDialling:
                    if ( tempCoreStatus == RCall::EStatusIdle )
                        {
                        tempCoreStatus = RCall::EStatusDialling;
                        }
                    //update mobile status
                    if ( tempMobileStatus == RMobileCall::EStatusIdle || 
                         tempMobileStatus == RMobileCall::EStatusHold)
                        {
                        tempMobileStatus = mobileStatus;
                        }
                    break;
                case RMobileCall::EStatusAnswering:
                    if ( tempCoreStatus == RCall::EStatusIdle ||
                         tempCoreStatus == RCall::EStatusRinging )
                        {
                        tempCoreStatus = RCall::EStatusAnswering;
                        }
                    //update mobile status
                    if ( tempMobileStatus == RMobileCall::EStatusIdle || 
                         tempMobileStatus == RMobileCall::EStatusRinging ||
                         tempMobileStatus == RMobileCall::EStatusHold )
                        {
                        tempMobileStatus = mobileStatus;
                        }
                    break;
                case RMobileCall::EStatusConnecting:
                    if ( tempCoreStatus == RCall::EStatusIdle || 
                            tempCoreStatus == RCall::EStatusDialling )
                        {
                        tempCoreStatus = RCall::EStatusConnecting;
                        }
                    //update mobile status
                    if ( tempMobileStatus == RMobileCall::EStatusIdle ||
                         tempMobileStatus == RMobileCall::EStatusDialling ||
                         tempMobileStatus == RMobileCall::EStatusHold )
                        {
                        tempMobileStatus = mobileStatus;
                        }
                    break;
                case RMobileCall::EStatusDisconnecting:
                    //update core status
                    if ( tempCoreStatus == RCall::EStatusIdle )
                        {
                        tempCoreStatus = RCall::EStatusHangingUp;
                        }
                    if ( tempMobileStatus == RMobileCall::EStatusIdle )
                        {
                        tempMobileStatus = mobileStatus;
                        }
                    break;
                case RMobileCall::EStatusWaitingAlternatingCallSwitch:
                    if ( tempMobileStatus == RMobileCall::EStatusIdle )
                        {
                        tempMobileStatus = mobileStatus;
                        }
                    break;
                case RMobileCall::EStatusHold:
                    if ( tempMobileStatus == RMobileCall::EStatusIdle || 
                         tempMobileStatus == 
                            RMobileCall::EStatusDisconnecting )
                        {
                        tempCoreStatus = iLineStatus;
                        tempMobileStatus = mobileStatus;
                        }
                    break;
                case RMobileCall::EStatusIdle:
                case RMobileCall::EStatusReconnectPending:
                case RMobileCall::EStatusDisconnectingWithInband:
                case RMobileCall::EStatusTransferring:
                case RMobileCall::EStatusTransferAlerting:
                default:
                    break;
                }
            }
        }

    //check if core status has changed
    if ( tempCoreStatus != iLineStatus )
        {
        coreStatusChanged = ETrue;
        iLineStatus = tempCoreStatus;
        }

    //check if mobile status has changed
    if ( tempMobileStatus != iMobileLineStatus )
        {
        mobileStatusChanged = ETrue;
        iMobileLineStatus = tempMobileStatus;
        }

    //get core status change notification req handle
    TTsyReqHandle iNotifyStatusChangeHandle = iTsyReqHandleStore->
                GetTsyReqHandle( EMultimodeLineNotifyStatusChange );

    //if req handle available and status changed
    if ( ( iNotifyStatusChangeHandle > 0 ) && coreStatusChanged )
        {
        *iRetLineStatus = iLineStatus; 
        iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodeLineNotifyStatusChange );
        ReqCompleted( iNotifyStatusChangeHandle, ret );
        }

    //get mobile status change notification req handle
    TTsyReqHandle iNotifyMobileLineStatusChangeHandle = iTsyReqHandleStore->
        GetTsyReqHandle( EMultimodeLineMobileLineStatusChange );

    //if req handle available and status changed
    if ( iNotifyMobileLineStatusChangeHandle > 0 
        && ( coreStatusChanged || mobileStatusChanged ) )
        {
        *iRetMobileLineStatus = iMobileLineStatus; 
        iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodeLineMobileLineStatusChange );
        ReqCompleted( iNotifyMobileLineStatusChangeHandle, ret );
        }
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::GetHookStatus
// This CORE API method retrieves the current hook status. Method is not 
// currently supported,
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::GetHookStatus(
    const TTsyReqHandle aTsyReqHandle,
    RCall::THookStatus* )
    {
    ReqCompleted( aTsyReqHandle, KErrNotSupported );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::GetInfo
// This CORE API method retrieves the current line information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::GetInfo(
    const TTsyReqHandle aTsyReqHandle,
    RLine::TLineInfo* aLineInfo )
    {
    // The current hook status.
    aLineInfo->iHookStatus = RCall::EHookStatusUnknown; 
    // The current line status.
    aLineInfo->iStatus = iLineStatus;
    // The name of the last call created on the line.
    aLineInfo->iNameOfLastCallAdded = iNameOfLastCallAdded;
    // The name of the call to which a new incoming call will be directed.
    aLineInfo->iNameOfCallForAnswering = iNameOfCallForAnswering;
 
    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::LineInfo
// This method retrieves the current line information. The method is used by 
// MmPhone object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RPhone::TLineInfo CMmLineTsy::LineInfo() const
    {        
    // Line info
    RPhone::TLineInfo lineInfo;
    // The current line status.
    lineInfo.iStatus = iLineStatus;
    // line capabilities
    lineInfo.iLineCapsFlags = iLineCaps.iFlags;
    // line name
    lineInfo.iName = iLineName;

    return lineInfo;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyCapsChange
// This CORE API method provides notification of a change in the line 
// capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyCapsChange(
    const TTsyReqHandle aTsyReqHandle,
    RLine::TCaps* aCaps )
    {
    // On return, contains the new line capabilities
    iRetCaps = aCaps;
    iTsyReqHandleStore->SetTsyReqHandle( EMultimodeLineCapsChangeNotification,
        aTsyReqHandle );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyCapsChangeCancel
// This CORE API method cancels an "line capabilities change" notification 
// request, placed using the NotifyCapsChange() method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyCapsChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetCaps = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeLineCapsChangeNotification );
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::CompleteNotifyCapsChange
// This method is used to notify to the client about the capabilities change.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmLineTsy::CompleteNotifyCapsChange()
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeLineCapsChangeNotification );

    if ( EMultimodeLineReqHandleUnknown != reqHandle )
        {
        *iRetCaps = iLineCaps; 
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyIncomingCall
// This CORE API method is used to notify a client when an incoming call is 
// detected.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyIncomingCall(
    const TTsyReqHandle aTsyReqHandle,
    TName* aName )
    {
    // On notification, contains the name of the incoming call.
    iRetIncomingCallName = aName;    
    iTsyReqHandleStore->SetTsyReqHandle( EMultimodeLineNotifyIncomingCall,
         aTsyReqHandle );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyIncomingCallCancel
// This CORE API method cancels an outstanding incoming call notification, 
// placed with the NotifyIncomingCall() method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyIncomingCallCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetIncomingCallName = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeLineNotifyIncomingCall );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyCallAdded
// This CORE API method provides notification that a new call has been added 
// to the line.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyCallAdded(
    const TTsyReqHandle aTsyReqHandle,
    TName* aName )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_NOTIFYCALLADDED_1, "TSY: CMmLineTsy::NotifyCallAdded requested by client, \lineMode:%d", iLineMode);
    // On return, contains the name of the new call.
    iRetCallAdded = aName;    
    iTsyReqHandleStore->SetTsyReqHandle( EMultimodeLineNotifyCallAdded,
        aTsyReqHandle );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyCallAddedCancel
// This CORE API method cancels an outstanding "new call added" notification 
// request, placed using the NotifyCallAdded() method.(other items were 
// commented in a header).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyCallAddedCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_NOTIFYCALLADDEDCANCEL_1, "TSY: CMmLineTsy::NotifyCallAddedCancel requested by client");
    iRetCallAdded = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeLineNotifyCallAdded );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::CompleteNotifyCallAdded
// This CORE API method is used to notify to the client that a call has been 
// added to the line. NOTE: Type is not defined because this method may be 
// called when incoming call occurs or when a call object has been created. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmLineTsy::CompleteNotifyCallAdded(const TDesC& aName )    
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_COMPLETENOTIFYCALLADDED_1, "TSY: CMmLineTsy::CompleteNotifyCallAdded entered, \CALL ADDED, Call name: %S, Call mode: %d", aName, iLineMode );

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
                                EMultimodeLineNotifyCallAdded );

    iNumCalls++;

    if ( EMultimodeLineReqHandleUnknown != reqHandle )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_COMPLETENOTIFYCALLADDED_2, "TSY: CMmLineTsy::CompleteNotifyCallAdded, Completed!");
        *iRetCallAdded = aName;
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::GetCaps
// This CORE API method retrieves the line capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::GetCaps(
    const TTsyReqHandle aTsyReqHandle,
    RLine::TCaps* aCaps )
    {
    *aCaps = iLineCaps;
    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::GetStatus
// This CORE API method returns core line status information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::GetStatus(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TStatus* aStatus )
    {    
    *aStatus = iLineStatus;
    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyHookChange
// This CORE API method provides notification when the hook status changes. 
// Feature is not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyHookChange(
    const TTsyReqHandle aTsyReqHandle,
    RCall::THookStatus* )
    {    
    ReqCompleted( aTsyReqHandle, KErrNotSupported );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyHookChangeCancel
// This CORE API method cancels an outstanding hook change notification req
// placed using the NotifyHookChange() method. Feature is not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyHookChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {    
    ReqCompleted( aTsyReqHandle, KErrNotSupported );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::GetCallInfo
// This CORE API method returns information about a call. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::GetCallInfo(
    const TTsyReqHandle aTsyReqHandle,
    TCallInfoIndex* aCallInfoIndex )
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_GETCALLINFO_1, "TSY: CMmLineTsy::GetCallInfo - Line name: %S, Index: %d",iLineName, aCallInfoIndex->iIndex );

    TInt ret( KErrNotFound );

    // Check if the call object can be found from call list by index
    CMmCallTsy* mmCall = REINTERPRET_CAST( CMmCallTsy*, 
            iMmPhone->CallList()->GetMmCallByIndexAndLine( 
                aCallInfoIndex->iIndex, &iLineName ) );

    // If call object was found, fill the client pointer with call information
    if ( mmCall )
        {
        aCallInfoIndex->iInfo.iCallName      = mmCall->CallName();
        aCallInfoIndex->iInfo.iStatus        = mmCall->Status();
        aCallInfoIndex->iInfo.iCallCapsFlags = mmCall->CallCaps();

        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_GETCALLINFO_2, "TSY: CMmLineTsy::GetCallInfo - Call name: %S, Status: %d",aCallInfoIndex->iInfo.iCallName, aCallInfoIndex->iInfo.iStatus );

        ret = KErrNone;
        }

    ReqCompleted( aTsyReqHandle, ret );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::CancelService
// CancelService is called by the server when it is "cleaning-up" any still 
// outstanding asynchronous requests before closing a client's sub-session. 
// This will happen if a client closes its R-class handle without cancelling 
// outstanding asynchronous requests. Core API requests are directed to the 
// LineBase. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::CancelService(
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt ret( KErrNone );
    
    // When the clients close their sub-sessions (eg. by calling RLine::Close)
    // they may not have cancelled all their outstanding asynchronous requests 
    // before closing.  It is up to the ETel server to clean up in this 
    // situation, so the server will find the list of outstanding requests 
    // related to that sub-session object and pass these outstanding IPC 
    // request numbers, one at a time, to the CancelService method in the TSY.

    switch ( aIpc )
        {
        //TSY has started a request and it is not possible to then cancel
        //this request. The best thing for the TSY to do in this case is to
        //proceed as though the Cancel never happened. The server's call to
        //the TSY cancel function will return synchronously. The TSY then
        //continues to wait for the original acknowledgement and when it
        //receives it, the TSY will complete the original request.
        //case XXX
        //    break;

        //Cancel methods that are not supported
        //case XXX:
        //    ret = KErrNotSupported;
        //    break;

        //Notification Cancels, no special requirements.
        case EMobileLineNotifyMobileLineStatusChange:
            ret =  NotifyMobileLineStatusChangeCancel( aTsyReqHandle );
            break;
        //Everything is taken care in the method implementation.
        //Just direct the request to the method.
        //case XXX:
        //    ret = XXXCancel( aTsyReqHandle );
        //    break;

        default:
            return CLineBase::CancelService( aIpc, aTsyReqHandle );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::RegisterNotification
// RegisterNotification is called when the server recognises that this 
// notification is being posted for the first time on this sub-session object. 
// It enables the TSY to "turn on" any regular notification messages that it 
// may receive from DOS. Currently does not really do anything but returns  
// KErrNone to ETel server in case of known notification request type. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::RegisterNotification(
    const TInt aIpc )
    {    
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EEtelLineNotifyStatusChange:
        case EEtelLineNotifyCallAdded:
        case EETelLineCapsChangeNotification:
        case EEtelLineNotifyHookChange:
        case EEtelLineNotifyIncomingCall:
        case EMobileLineNotifyMobileLineStatusChange:
            ret = KErrNone;
            break;
        default:
            // Unknown or invalid IPC
            ret = KErrNotSupported;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::DeregisterNotification
// DeregisterNotification is called when the server recognises that this 
// notification will not be posted again because the last client to have a 
// handle on this sub-session object has just closed the handle. It enables 
// TSY to "turn off" any regular notification messages that it may receive 
// from DOS. Currently does not really do anything but returns KErrNone to  
// ETel server in case of known notification request type. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::DeregisterNotification(
    const TInt aIpc )
    {
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EEtelLineNotifyStatusChange:
        case EEtelLineNotifyCallAdded:
        case EETelLineCapsChangeNotification:
        case EEtelLineNotifyHookChange:
        case EEtelLineNotifyIncomingCall:
        case EMobileLineNotifyMobileLineStatusChange:
            ret = KErrNone;
            break;
        default:
            // Unknown or invalid IPC
            ret = KErrNotSupported;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NumberOfSlotsL
// NumberOfSlotsL is called by the server when it is registering a new 
// NOTIFICATION. It enables the TSY to tell the server how many buffer slots 
// to allocate for "repost immediately" notifications that may trigger before 
// clients collect them.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NumberOfSlotsL(
    const TInt aIpc )
    {    
    //There is a buffer in the ETel server for each type of NOTIFICATION and
    //the size of the buffer is determined by the TSY. When the ETel server
    //discovers that a request is "repost immediately" it will ask the TSY how
    //big a buffer it wants by calling CTelObject::NumberOfSlotsL( ).
    //
    //It is up to the TSY developer's judgement how many buffer slots this
    //method returns for each notification. If there is a danger that a
    //particular notification could trigger frequently and in rapid succession
    //(e.g. call group change notification) then at least 10 or 20 slots may
    //be required. For a notification that triggers rarely, perhaps 1 or 2
    //slots is enough.
    //
    //So if the ETel server has stored a few occurrences of a particular
    //notification, when a client subsequently calls that notification request
    //the ETel server will complete the client's request with the saved data.
    
    TInt numberOfSlots( 1 );

    switch ( aIpc )
        {
        case EEtelLineNotifyStatusChange:
            numberOfSlots = KMmLineStatusChangeSlots;
            break;
        case EEtelLineNotifyCallAdded:
            numberOfSlots = KMmLineCallAddedSlots;
            break;
        case EETelLineCapsChangeNotification:
            numberOfSlots = KMmLineCapsChangeSlots;
            break;
        case EEtelLineNotifyHookChange:
            numberOfSlots = KMmLineHookChangeSlots;
            break;
        case EEtelLineNotifyIncomingCall:
            numberOfSlots = KMmLineIncomingCallSlots;
            break;
        case EMobileLineNotifyMobileLineStatusChange:
            numberOfSlots = KMmLineMobileLineStatusChangeSlots;
            break;
        default:  
            // Unknown or invalid Line IPC
            User::Leave( KErrNotSupported );
            break;
        }
    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::LineMode
// Returns line mode.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RMobilePhone::TMobileService CMmLineTsy::LineMode() const
    {
    return iLineMode;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::LineName
// Returns line name.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TName CMmLineTsy::LineName() const
    {
    return iLineName;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::GetMobileLineStatus
// Returns the current status of the line through the aStatus argument. The 
// possible line states map to the call states (see MM API spec) so that a 
// line that has one or more calls opened from it will have a status defined 
// by this table.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::GetMobileLineStatus(
    const TTsyReqHandle aTsyReqHandle,
    RMobileCall::TMobileCallStatus* aStatus )
    {
    *aStatus = iMobileLineStatus;
    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyMobileLineStatusChange
// Allows a client to be notified when the mobile line changes state. The 
// request completes when the line changes state, the new state being copied 
// to the aStatus parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyMobileLineStatusChange(
    RMobileCall::TMobileCallStatus* aStatus )
    {
    iRetMobileLineStatus = aStatus;
    iReqHandleType = EMultimodeLineMobileLineStatusChange;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::NotifyMobileLineStatusChangeCancel
// Cancels an outstanding asynchronous NotifyMobileLineStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmLineTsy::NotifyMobileLineStatusChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetMobileLineStatus = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeLineMobileLineStatusChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmLineTsy::CompleteNotifyAddBypassingCall
// Handles ghost call object creation and initialisation. Also notifies 
// clients about the ghost call (using NotifyCallAdded method)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmLineTsy::CompleteNotifyAddBypassingCall(
    CMmDataPackage* aDataPackage )
    {
    TInt callId( -1 );
    RMobilePhone::TMobileService callMode( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallInfoV1* mobileCallInfo;

    const CCallDataPackage* callData = 
        reinterpret_cast<const CCallDataPackage*>(aDataPackage);

    callData->GetCallIdAndMode( callId, callMode );
    callData->UnPackData ( &mobileCallInfo );

    if ( mobileCallInfo->iStatus == RMobileCall::EStatusDialling ||
         mobileCallInfo->iStatus == RMobileCall::EStatusConnecting ||
         mobileCallInfo->iStatus == RMobileCall::EStatusConnected )
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_COMPLETENOTIFYADDBYPASSINGCALL_1, "TSY: CMmLineTsy::CompleteNotifyAddBypassingCall, \CALL INITIATED - NOT REQUESTED BY ETEL's CLIENT, Call ID: %d",callId );

        //create new call object
        CMmCallTsy* mmGhostCall = CreateGhostCallObject(
            callId, callMode, mobileCallInfo->iStatus );

        if ( mmGhostCall )
            {
            // Add the new call to the list of calls
            TInt ret( iMmPhone->CallList()->AddObject( mmGhostCall ) );

            if ( KErrNone == ret )
                {
                mmGhostCall->CompleteNotifyMobileCallInfoChange( 
                    aDataPackage );
                CompleteNotifyStatusChange();
                //inform ETel client that someone is creating a MO call
                CompleteNotifyCallAdded( mmGhostCall->CallName() );
                mmGhostCall->SetGhostCall( ETrue );
                }
            else
                {
                delete mmGhostCall;
                }
            }
        }
    else
        {
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_COMPLETENOTIFYADDBYPASSINGCALL_2, "TSY: CMmLineTsy::CompleteNotifyAddBypassingCall, \CALL NOT INITIATED - Call ID:%d, Call status:%d", callId, mobileCallInfo->iStatus);
        }
    }

#ifdef TF_LOGGING_ENABLED
// ---------------------------------------------------------------------------
// CMmLineTsy::ReqCompleted
// Overloads CTelObject::ReqCompleted for logging purposes. It prints the 
// aTsyReqHandle and aError variable in the log file and then calls 
// CTelObject::ReqCompleted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmLineTsy::ReqCompleted(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aError )
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_REQCOMPLETED_1, "TSY: CMmLineTsy::Request Completed. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", aTsyReqHandle, aError, this);

    CTelObject::ReqCompleted(aTsyReqHandle,aError);

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINETSY_REQCOMPLETED_2, "<-- TSY: CMmLineTsy::ReqCompleted");
    }

#endif


//  End of File 
