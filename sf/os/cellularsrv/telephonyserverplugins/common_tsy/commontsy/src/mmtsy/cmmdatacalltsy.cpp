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
#include "cmmdatacalltsytraces.h"
#endif

#include "cmmdatacalltsy.h"
#include "cmmdatalinetsy.h"
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmcalllist.h"
#include "CMmCommonStaticUtility.h"
#include "MmTsy_numberOfSlots.h"
#include "cmmlinelist.h"
#include "cmmmessagemanagerbase.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/gsmerror.h>
#include "cmmcallgsmwcdmaext.h"
#include <etelmmerr.h>


// ======== MEMBER FUNCTIONS ========

CMmDataCallTsy::CMmDataCallTsy()
    {
    }

CMmDataCallTsy* CMmDataCallTsy::NewL( 
    CMmPhoneTsy* aMmPhone, CMmDataLineTsy* aMmLine,
    RMobilePhone::TMobileService aMode, 
    TDes& aName, 
    CMmMessageManagerBase* aMessageManager )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_NEWL_1, "TSY: CMmDataCallTsy::NewL. Call name: %S", aName);

    CMmDataCallTsy* mmCall = NULL;

    //check input parameters
    if ( NULL != aMmPhone && NULL != aMmLine &&
         RMobilePhone::ECircuitDataService == aMode )
        {
        mmCall = new( ELeave ) CMmDataCallTsy;
        CleanupClosePushL( *mmCall );
        mmCall->iMmPhone = aMmPhone;
        mmCall->iMmLine = aMmLine;
        mmCall->iCallName = aName;
        mmCall->iCallMode = aMode;
        mmCall->iMessageManager = aMessageManager;
        mmCall->ConstructL( aMode );        
        CleanupStack::Pop( mmCall );
        }

    return mmCall;
    }

CMmDataCallTsy::~CMmDataCallTsy()
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DTOR_1, "TSY: CMmDataCallTsy::~CMmDataCallTsy - Call deleted iCallId: %d iCallName: %S",iCallId, iCallName);

    // If Dial fails, Symbian CSD agent will close the call immediately.
    // This means that TSY has not yet received call status indications, 
    // where call status changes to idle. Thus responses from modem are 
    // passed to call object. 
    // Here we need to recover dataport if it is used.
    CCallBase::SetUnowned();

    // Check if dataport is loaned!
    if ( iLoanedCommPort.iPort.Compare( KNullDesC) != 0 )
        {
        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode( iCallId, iCallMode );
        //Pack commport
        package.PackData( &iLoanedCommPort );

        //Send request to the Domestic OS layer.
        if( iMessageManager )
        	{
        	TRAP_IGNORE( 
            		iMessageManager->HandleRequestL(
                		EEtelCallRecoverDataPort, &package );
            		   );
        	}
        	
        iLoanedCommPort.iCsy.Zero();
        iLoanedCommPort.iPort.Zero();
        }

    iRetDataCallCaps = NULL;
    iRetHscsdInfo = NULL;
    }
    

// ---------------------------------------------------------------------------
// CMmDataCallTsy::InitInternalAttributes
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::InitInternalAttributes()
    {
    CMmCallTsy::InitInternalAttributes();

    //set call capabilities
    iCallCaps.iFlags =
        RCall::KCapsData | RCall::KCapsDial | RCall::KCapsConnect;

    // Boolean that indicates if this call object is finished data call.
    // Required as Symbian CSD agent in situation where first data call
    // fails due wrong number and it then opens second data call, which
    // dials correct number. Call status notifications go to wrong call
    // object (the first one) and causes problems for clients.
    iIsFinishedDataCall = EFalse;

    //data call params
    iRetDataCallCaps = NULL;
    iLoanedCommPort.iCsy.Zero();
    iLoanedCommPort.iPort.Zero();
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::DoExtFuncL
// DoExtFuncL is called by the Etel server when it has an "extended", i.e. 
// non-core ETel request for the TSY. To process a request handle, request  
// type and request data are passed to the TSY. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DOEXTFUNCL_1, "TSY: CMmDataCallTsy::DoExtFuncL.IPC:%{TIPCNamesList}  Handle:%u Object:0x%08x",(TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    TInt ret( KErrNone );

    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();

    // The request data has to extracted from TDataPackage and the TAny* 
    // pointers have to be "cast" to the expected request data type

    // NOTE! Do not put any Cancel methods here.
    switch ( aIpc )
        {
      // Call Status
        // Get Call Status
        case EMobileCallGetMobileCallStatus:
            ret = GetMobileCallStatus( aTsyReqHandle,
                reinterpret_cast<RMobileCall::TMobileCallStatus*>(dataPtr) );
            break;
        // Notify Call Status Change
        case EMobileCallNotifyMobileCallStatusChange:
            ret = NotifyMobileCallStatusChange(
                reinterpret_cast<RMobileCall::TMobileCallStatus*>(dataPtr) );
            break;
        // Transfer One Call to Remote Party of Another Call
        case EMobileCallTransfer:
            ret = TransferL( aTsyReqHandle );
            break;
        // Notify Call Event
        case EMobileCallNotifyCallEvent:
            ret = NotifyCallEvent(
                reinterpret_cast<RMobileCall::TMobileCallEvent*>( dataPtr) );
            break;
        // Call Control
        // Get Call Capabilities
        case EMobileCallGetMobileCallCaps: 
             ret = GetMobileCallCaps( aTsyReqHandle, aPackage.Des1n() );
			 break;

	    //Notify Call Capabilities Change
        case EMobileCallNotifyMobileCallCapsChange:
             ret =  NotifyMobileCallCapsChange(aPackage.Des1n());
             break;
       
	    // Call Information
        // Get Mobile Call Information
        case EMobileCallGetMobileCallInfo:
            ret = GetMobileCallInfo( aTsyReqHandle, aPackage.Des1n() );
            break;
        // Notify Change of Remote Party Information
        case EMobileCallNotifyRemotePartyInfoChange:
            ret = NotifyRemotePartyInfoChange(
            reinterpret_cast<RMobileCall::TMobileCallRemotePartyInfoV1Pckg*>(
                    aPackage.Des1n()) );
            break;
        // Data Call Capabilities
        // Get Data Call Capabilities
        case EMobileCallGetMobileDataCallCaps: 
            ret = GetMobileDataCallCaps( aTsyReqHandle, aPackage.Des1n() );
            break;
        // Notify Change of Data Call Capabilities
        case EMobileCallNotifyMobileDataCallCapsChange:
            ret = NotifyMobileDataCallCapsChange( aTsyReqHandle,
                aPackage.Des1n() );
            break;
        // Data Call RLP Parameters
        // Get Data Call RLP Range
        case EMobileCallGetMobileDataCallRLPRange:
            ret = GetMobileDataCallRLPRange( aTsyReqHandle, 
                reinterpret_cast<TInt*>(dataPtr),
                reinterpret_cast<RMobileCall::TMobileDataRLPRangesV1Pckg*>(
                    dataPtr2) );
            break;
        // High Speed Circuit Switched Data (HCSCD)
        // Set Dynamic HSCSD Call Parameters
        case EMobileCallSetDynamicHscsdParams:
            ret = SetDynamicHscsdParams( aTsyReqHandle, 
                reinterpret_cast<RMobileCall::TMobileCallAiur*>(dataPtr),
                reinterpret_cast<TInt*>(dataPtr2) );
            break;
        // Get Current HSCSD Call Information
        case EMobileCallGetCurrentHscsdInfo:
            ret = GetCurrentHscsdInfo( aTsyReqHandle, aPackage.Des1n() );
            break;
        // Notify Change of HSCSD Call Information
        case EMobileCallNotifyHscsdInfoChange:
            ret = NotifyHscsdInfoChange( aTsyReqHandle, aPackage.Des1n() );
            break;
        // Order to notify if privacy is confirmed
        case EMobileCallNotifyPrivacyConfirmation:
            ret = NotifyPrivacyConfirmation(
              reinterpret_cast<RMobilePhone::TMobilePhonePrivacy*>(dataPtr) );
            break;
        // Dial a call ( DialNoFdnCheck )
        case EMobileCallDialNoFdnCheck:
    		ret = DialNoFdnCheck( aTsyReqHandle, aPackage.Des1n(), 
    		    aPackage.Des2u() ); 
    		break;            
        // Unsupported features
        case EMobileCallDialEmergencyCall:
        case EMobileCallHold:
        case EMobileCallResume:
        case EMobileCallSwap:
        case EMobileCallGoOneToOne:

        case EMobileCallSwitchAlternatingCall:
        case EMobileCallNotifyAlternatingCallSwitch:
        case EMobileCallSetPrivacy:
        case EMobileCallSetTrafficChannel:
        case EMobileCallNotifyTrafficChannelConfirmation:
        case EMobileCallGetUUSCaps:
        case EMobileCallNotifyUUSCapsChange:
        case EMobileCallActivateUUS:
        case EMobileCallSendUUI:
        case EMobileCallReceiveUUI:
        case EMobileCallHangupWithUUI:
        case EMobileCallAnswerWithUUI:
        case EMobileCallNotifyVoiceFallback:
        case EMobileCallDeflect:
        case EMobileCallActivateCCBS:
        case EMobileCallRejectCCBS:
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DOEXTFUNCL_2, "TSY: CMmDataCallTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::ReqModeL
// ReqModeL is called from the ETel server's CTelObject::ReqAnalyserL in order 
// to check the type of request it has.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmDataCallTsy::ReqModeL(
    const TInt aIpc )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_REQMODEL_1, "TSY: CMmDataCallTsy::ReqModeL - IPC: %{TIPCNamesList}", aIpc);

    CTelObject::TReqMode ret( 0 ); // default return value
    
    switch ( aIpc )
        {
        // No Flow Control
        // All disabled
        // TSYs wishing to implement their own buffering algorithm will
        // place all requests in this category.
        // This category will also be used by the RecoverDataPort request
        // that must be passed to the TSY to indicate that it may use
        // the communications port after a loan.

        // Get methods that do not use Licencee specific TSY  and return 
        // immediately. Flow control not required.
        case EEtelCallGetInfo:
        case EEtelCallGetStatus:
        case EEtelCallGetCaps:
        case EEtelCallGetBearerServiceInfo:
        case EEtelCallGetOwnershipStatus:
        case EEtelCallGetCallParams:
        case EEtelCallGetCallDuration:
        case EEtelCallGetFaxSettings:
        case EMobileCallGetMobileCallStatus:
        case EMobileCallGetMobileCallInfo:
        case EMobileCallGetMobileDataCallCaps:
        case EMobileCallGetMobileCallCaps:
        case EMobileCallGetCurrentHscsdInfo:
        case EMobileCallGetMobileDataCallRLPRange:
        case EMobileCallGetUUSCaps:
        case EMobileCallActivateUUS:
        case EMobileCallSendUUI:
        case EMobileCallReceiveUUI:
        case EMobileCallHangupWithUUI:
        case EMobileCallAnswerWithUUI:
        // Other methods that do not use Licencee specific TSY and return 
        // immediately.
        // Flow control not required.
        case EEtelCallAcquireOwnership:
        case EEtelCallTransferOwnership:
        case EEtelCallSetFaxSettings:
        // Methods that can propably take a long time and cannot therefore be 
        // flow controlled. Solution: All these methods must check req handle 
        // table before handling the request. In case that the request table 
        // indicates that same method has been called and has not been 
        // completed, the method should return KErrServerBusy.
        case EMobileCallTransfer:
        // This is also included here due to the fact that Emergency call
        // should never be blocked by flow control. KErrServerBusy returned
        // when already already dialing.
        case EMobileCallDialEmergencyCall:
        // HangUp cannot be flow controlled. Client should for example be able
        // to terminate a call while another call is e.g. in Dialling state.
        case EEtelCallHangUp:
        // Answer was mixed with Hangup. Therefore this is also No flow 
        // control type
        case EEtelCallAnswer:
        // dial is not flow controlled as compeltion may take a while and that
        // blocks for example PS data activation in 3G while dialing.
        case EEtelCallDial:
        case EMobileCallDialNoFdnCheck:        
            break;
        // Flow Controlled Services
        // KReqModeFlowControlObeyed
        // Commands that change the state of the phone, e.g. clearing the
        // AoC counter; are commands that the TSY should only deal with
        // one at a time.
        
        //Voice call related methods that should be handled one at the time.
        case EEtelCallConnect:
        case EMobileCallHold:
        case EMobileCallResume:
        case EMobileCallSwap:
        case EMobileCallDeflect:
        case EMobileCallGoOneToOne:
        //Data call related methods that should be handled one at the time.
        case EEtelCallLoanDataPort:
        case EEtelCallRecoverDataPort:
        case EMobileCallSwitchAlternatingCall:
        case EMobileCallSetDynamicHscsdParams:
        case EMobileCallSetPrivacy:
        case EMobileCallSetTrafficChannel:
            ret = KReqModeFlowControlObeyed;
            break;
        // ReqModePostImmediately
        // Requests that notify a client about a change of state, where
        // the TSY needs to distinguish between different clients.
            //ret=KReqModeRePostImmediately;

        // KReqModeMultipleCompletionEnabled
        // (a) commands that may take some time, but which the TSY can
        //     handle more than one of concurrently, or 
        // (b) notifications that the TSY does not wish to be re-posted
        //     immediately, so the server does no buffering.
            //ret=KReqModeMultipleCompletionEnabled;

        // KReqModeMultipleCompletionEnabled | KReqModeFlowControlObeyed
        // A command that may take some time and which the TSY can only
        // deal with one at a time.
            //ret = KReqModeMultipleCompletionEnabled |
            //      KReqModeFlowControlObeyed;
            //break;

        // Notifications
        // KReqModeMultipleCompletionEnabled | ReqModePostImmediately
        // Requests that notify a client about a change of state.
        // Since these requests do not require the issuing of any modem
        // commands, they do not have to obey flow control.
        // The TSY never gets more than one of these outstanding per 
        // CTelObject
        case EEtelCallNotifyHookChange:        
        case EEtelCallNotifyStatusChange:
        case EEtelCallNotifyDurationChange: 
        case EEtelCallCapsChangeNotification:
        case EMobileCallNotifyCallEvent:
        case EMobileCallNotifyMobileCallStatusChange:
        case EMobileCallNotifyRemotePartyInfoChange:
        case EMobileCallNotifyPrivacyConfirmation:
        case EMobileCallNotifyTrafficChannelConfirmation:
        case EMobileCallNotifyHscsdInfoChange:
        case EMobileCallNotifyMobileDataCallCapsChange:
        case EMobileCallNotifyAlternatingCallSwitch:
        case EMobileCallNotifyMobileCallCapsChange:
        case EMobileCallNotifyVoiceFallback:
        case EMobileCallNotifyUUSCapsChange:
            ret = KReqModeMultipleCompletionEnabled |
                KReqModeRePostImmediately;
            break;
        // Cancel Requests
        // It is not necessary to include the Cancel methods in ReqModeL.
        // The ETel server never calls ReqModeL with a Cancel IPC.

        // Other variations of return values are unusable

        // Default: Call CCallBase's ReqModeL.
        default:
            //Direct the request to the CCallBase
            ret = CCallBase::ReqModeL( aIpc );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::NumberOfSlotsL
// NumberOfSlotsL is called by the server when it is registering a new 
// NOTIFICATION. It enables the TSY to tell the server how many buffer slots 
// to allocate for "repost immediately" notifications that may trigger before 
// clients collect them.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    // There is a buffer in the ETel server for each type of NOTIFICATION and
    // the size of the buffer is determined by the TSY. When the ETel server
    // discovers that a request is "repost immediately" it will ask the TSY
    // how big a buffer it wants by calling CTelObject::NumberOfSlotsL( ).
    //
    // It is up to the TSY developer's judgement how many buffer slots this
    // method returns for each notification. If there is a danger that
    // a particular notification could trigger frequently and in rapid
    // succession (e.g. call group change notification) then at least 10 or
    // 20 slots may be required. For a notification that triggers rarely,
    // perhaps 1 or 2 slots is enough.
    //
    // So if the ETel server has stored a few occurrences of a particular
    // notification, when a client subsequently calls that notification
    // request, the ETel server will complete the client's request with
    // the saved data.

    TInt numberOfSlots( 1 );

    switch ( aIpc ) 
        {
        case EEtelCallNotifyHookChange:
            numberOfSlots = KMmCallHookChangeSlots;
            break;
        case EEtelCallNotifyStatusChange:
            numberOfSlots = KMmCallStatusChangeSlots;
            break;
        case EEtelCallNotifyDurationChange: 
            numberOfSlots = KMmCallDurationChangeSlots;
            break;
        case EEtelCallCapsChangeNotification:
            numberOfSlots = KMmCallCapsChangeSlots;
            break;
        case EMobileCallNotifyCallEvent:
            numberOfSlots = KMmCallCallEventSlots;
            break;
        case EMobileCallNotifyMobileCallStatusChange:
            numberOfSlots = KMmCallMobileCallStatusChangeSlots;
            break;
        case EMobileCallNotifyRemotePartyInfoChange:
            numberOfSlots = KMmCallRemotePartyInfoChangeSlots;
            break;
        case EMobileCallNotifyPrivacyConfirmation:
            numberOfSlots = KMmCallPrivacyConfirmationSlots;
            break;
        case EMobileCallNotifyTrafficChannelConfirmation:
            numberOfSlots = KMmCallTrafficChannelConfirmationSlots;
            break;
        case EMobileCallNotifyHscsdInfoChange:
            numberOfSlots = KMmCallHscsdInfoChangeSlots;
            break;
        case EMobileCallNotifyMobileDataCallCapsChange:
            numberOfSlots = KMmCallMobileDataCallCapsChangeSlots;
            break;
        case EMobileCallNotifyAlternatingCallSwitch:
            numberOfSlots = KMmCallAlternatingCallSwitchSlots;
            break;
        case EMobileCallNotifyMobileCallCapsChange:
            numberOfSlots = KMmCallMobileCallCapsChangeSlots;
            break;
        case EMobileCallNotifyVoiceFallback:
            numberOfSlots = KMmCallVoiceFallbackSlots;
            break;
        case EMobileCallNotifyUUSCapsChange:
            numberOfSlots = KMmCallUUSCapsChangeSlots;
            break;
        default:
            // Unknown or invalid Call IPC
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CancelService
// CancelService is called by the ETel server when it is "cleaning-up" any 
// still outstanding asynchronous requests before closing a client's 
// sub-session. This will happen if a client closes its R-class handle without 
// cancelling outstanding asynchronous requests. Only Mobile API requests are 
// handled here.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::CancelService(
    const TInt aIpc,
    const TTsyReqHandle aTsyReqHandle )
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_CANCELSERVICE_1, "TSY: CMmDataCallTsy::CancelService - IPC: %{TIPCNamesList}, Req handle: %d",aIpc, aTsyReqHandle);

    TInt ret( KErrNone );

    //When the clients close their sub-sessions (eg. by calling RPhone::Close)
    //they may not have cancelled all their outstanding asynchronous requests
    //before closing.  It is up to the ETel server to clean up in this
    //situation, so the server will find the list of outstanding requests
    //related to that sub-session object and pass these outstanding IPC req
    //numbers, one at a time, to the CancelService function in the TSY.
    switch ( aIpc )
        {
        //TSY has started a request and it is not possible to then cancel this
        //request. The best thing for the TSY to do in this case is to proceed
        //as though the Cancel never happened. The server's call to the TSY
        //cancel function will return synchronously. The TSY then continues to
        //wait for the original acknowledgement and when it receives it,
        //the TSY will complete the original request.
        case EMobileCallSetDynamicHscsdParams:
        case EMobileCallHold:
        case EMobileCallResume:
        case EMobileCallSwap:
        case EMobileCallGoOneToOne:
        case EMobileCallDeflect:
        case EMobileCallTransfer:
        case EMobileCallNotifyTrafficChannelConfirmation:
            break;
        //Cancel methods that are not supported
        case EMobileCallDialEmergencyCall:
        case EMobileCallSwitchAlternatingCall:
        case EMobileCallNotifyAlternatingCallSwitch:
        case EMobileCallGetMobileDataCallRLPRange:
        case EMobileCallNotifyVoiceFallback:
        case EMobileCallNotifyUUSCapsChange:
        case EMobileCallActivateUUS:
        case EMobileCallSendUUI:
        case EMobileCallReceiveUUI:
        case EMobileCallHangupWithUUI:
        case EMobileCallAnswerWithUUI:
            ret = KErrNotSupported;
            break;
        //Notification Cancels, no special requirements.
        case EMobileCallNotifyMobileDataCallCapsChange:
            ret = NotifyMobileDataCallCapsChangeCancel( aTsyReqHandle );
            break;
        case EMobileCallNotifyHscsdInfoChange:
            ret = NotifyHscsdInfoChangeCancel( aTsyReqHandle );
            break;
        case EMobileCallNotifyMobileCallCapsChange:
            ret = NotifyMobileCallCapsChangeCancel( aTsyReqHandle );
            break;
        case EMobileCallNotifyMobileCallStatusChange:
            ret = NotifyMobileCallStatusChangeCancel( aTsyReqHandle );
            break;
        case EMobileCallNotifyCallEvent:
            ret = NotifyCallEventCancel( aTsyReqHandle );
            break;
        case EMobileCallNotifyRemotePartyInfoChange:
            ret = NotifyRemotePartyInfoChangeCancel( aTsyReqHandle );
            break;
        case EMobileCallNotifyPrivacyConfirmation:
            ret = NotifyPrivacyConfirmationCancel( aTsyReqHandle );
            break;
        //Default case
        default:
            //call CCallBase
            ret = CCallBase::CancelService( aIpc, aTsyReqHandle );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CompleteNotifyStatusChange
// Complete notification when status changes. If the new status requires, this
// method will ask call protocol extension to update it's status and 
// capabilities and line owning this call to update it's status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::CompleteNotifyStatusChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    if ( iGhostCall ) 
        {
        HandleGhostCallStatusChange( aResult, aDataPackage );
        }
    else
        {
        TInt callIndex( 0 ); 
        TInt extendedError(
            CMmCommonStaticUtility::ExtendedErrorCode( aResult ) );
        TInt ret( KErrNone );
        TBool timerStarted( EFalse );
        TBool statusChanged ( EFalse );
        TBool mobileStatusChanged ( EFalse );
        CMmDataCallTsy* mmCall = NULL; 
        RMobileCall::TMobileCallStatus callStatus( RMobileCall::EStatusIdle );
        CCallDataPackage* callDataPackage = 
            reinterpret_cast<CCallDataPackage*>( aDataPackage );

        callDataPackage->UnPackData( callStatus );

        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETENOTIFYSTATUSCHANGE_1, "TSY: CMmDataCallTsy::CompleteNotifyStatusChange - \aResult: %{TSymbianErrorCodes}", aResult );
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETENOTIFYSTATUSCHANGE_2, "TSY: CMmDataCallTsy::CompleteNotifyStatusChange - \Call status: %d, Call name: %S", callStatus, iCallName);

        switch( callStatus )
            {
            case RMobileCall::EStatusIdle:
                // stop the call duration timer
                timerStarted = iCallTimer->Stop();
                // Check air time timer only if call was in active state
                // (call timer was started)
                if ( timerStarted )
                    {
                    iMmPhone->AirTimeTimerCheckStop();
                    UpdateLifeTimer();                    
                    }

                iCallStatus = RCall::EStatusIdle;
                iMobileCallStatus = RMobileCall::EStatusIdle;

                SetUnowned();
                ClearCallStatus();
				RecoverDataPort(0);
                statusChanged = ETrue;

                if ( KErrNone != aResult )
                    {
                    if ( KErrGsmReleaseByUser == aResult || 
                         KErrGsmBusyUserRequest == aResult )
                        {
                        // aResult must be KErrNone to indicate the client
                        // that HangUp has successfully completed 
                        CompleteHangUp( KErrNone );
                        if ( this != iMmPhone->WaitingCallForData() )
                            {
                            CompleteAnswerIncomingCall( 
                                KErrGsmBusyUserRequest );
                            }
                        }
                    else
                        {
                        // Set last exit code
                        iLastExitCode = aResult;
                        CompleteHangUp( aResult );
                        if ( this != iMmPhone->WaitingCallForData() )
                            {
                            CompleteAnswerIncomingCall( aResult );
                            }
                        }
                    CompleteDial( aResult );
                    CompleteDialNoFdn( aResult );
                    }
                // Try to complete Dial and HangUp; If completes happens from
                // here, something has gone wrong. Done to prevent TSY from
                // hanging.
                else
                    {
                    iLastExitCode = KErrGeneral;
                    TInt errorValue = CMmCommonStaticUtility::EpocErrorCode( 
                        KErrNotReady, KErrNotFound );
                    CompleteDial( errorValue );
                    CompleteDialNoFdn( errorValue );
                    CompleteHangUp( errorValue );
                    if ( this != iMmPhone->WaitingCallForData() )
                        {
                        CompleteAnswerIncomingCall( errorValue );
                        }
                    }
                // save last id. Required by Conference call implementation
                SetPreviousCallId( iCallId );
                // reset call id
                iCallId = -1;

                // reset caps. 
                iCallCaps.iFlags |=
                    RCall::KCapsDial | RCall::KCapsConnect;
                iCallCaps.iFlags &= ~( RCall::KCapsAnswer );

                // Check if KCapsHangup or dataport caps still exists
                // If it does, remove and complete notification.
                if ( ( RCall::KCapsHangUp ==
                        ( iCallCaps.iFlags & RCall::KCapsHangUp ) ) ||
                    ( RCall::KCapsLoanDataPort ==
                        ( iCallCaps.iFlags & RCall::KCapsLoanDataPort ) ) ||
                    ( RCall::KCapsRecoverDataPort ==
                        ( iCallCaps.iFlags & RCall::KCapsRecoverDataPort) ) )
                    {
                    iCallCaps.iFlags &= ~(
                        RCall::KCapsHangUp |
                        RCall::KCapsLoanDataPort |
                        RCall::KCapsRecoverDataPort );
                    CompleteNotifyCapsChange();
                    }

                // Call Transfer handling
                if ( ServiceRequested( EMultimodeCallTransfer ) )
                    {
                    // Complete Transfer 
                    CompleteTransfer( KErrNone );
                    }
                break;
                // End of case KCallStatusIdle
            case RMobileCall::EStatusDialling:
#ifdef REQHANDLE_TIMER
                iTsyReqHandleStore->StopTimeout( EMultimodeCallDial );
                iTsyReqHandleStore->StopTimeout( 
                    EMultimodeCallDialNoFdnCheck ); 
#endif
                iCallStatus = RCall::EStatusDialling;
                iMobileCallStatus = RMobileCall::EStatusDialling;
                statusChanged = ETrue;
                if ( iDialCancelFlag != CMmCallTsy::EDialCancelNotCalled )
                    {
                    TTsyReqHandle dialCancelHandle = 
                        iTsyReqHandleStore->GetTsyReqHandle( 
                            EMultimodeCallDial );
                    TTsyReqHandle dialCancelHandleNoFdn = 
                        iTsyReqHandleStore->GetTsyReqHandle( 
                        EMultimodeCallDialNoFdnCheck );                   

                    if ( 0 < dialCancelHandle )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETENOTIFYSTATUSCHANGE_3, "TSY: CMmDataCallTsy::CompleteNotifyStatusChange. - HangUp - EMultimodeCallDial");
                        HangUp( dialCancelHandle );
                        } 
                    else if ( 0 < dialCancelHandleNoFdn )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETENOTIFYSTATUSCHANGE_4, "TSY: CMmDataCallTsy::CompleteNotifyStatusChange. - HangUp - EMultimodeCallDialNoFdnCheck");
                        HangUp( dialCancelHandleNoFdn );
                        } 
                    else
                        {
                        iDialCancelFlag = CMmCallTsy::EDialCancelNotCalled;
                        }
                    }
                break;
            case RMobileCall::EStatusConnecting:
                iCallStatus = RCall::EStatusConnecting;
                iMobileCallStatus = RMobileCall::EStatusConnecting;
                statusChanged = ETrue;
                break;
            case RMobileCall::EStatusRinging:
                iCallStatus = RCall::EStatusRinging;
                iMobileCallStatus = RMobileCall::EStatusRinging;
                statusChanged = ETrue;

                // Set caps. 
                iCallCaps.iFlags &= ~(
                    RCall::KCapsDial | RCall::KCapsConnect );
                   
                iCallCaps.iFlags |= RCall::KCapsHangUp;
                if ( ( NULL == iMmPhone->CallList()->GetMmCallByStatus(
                        RMobileCall::EStatusConnected ) ) &&
                     ( NULL == iMmPhone->CallList()->GetMmCallByStatus(
                            RMobileCall::EStatusHold ) ) )                            
                    {
                    iCallCaps.iFlags |= RCall::KCapsAnswer;
                    }
                //complete caps change notification
                CompleteNotifyCapsChange();

                // Check if the call is for Call Back functionality.
                // E.g. client has dialled server, which calls back to 
                // save user the data call charging.
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETENOTIFYSTATUSCHANGE_5, "TSY: CMmDataCallTsy::Check for waiting call");
                if ( this == iMmPhone->WaitingCallForData() )
                    {
                    // Client has already tried to answer the incoming 
                    // call, so do it now
                    CallComingForWaitingCall();
                    }
                break;
            case RMobileCall::EStatusAnswering:
                // Set caps
                iCallCaps.iFlags |= RCall::KCapsHangUp;
                iCallCaps.iFlags &= ~(
                    RCall::KCapsDial |
                    RCall::KCapsAnswer );


                iCallStatus = RCall::EStatusAnswering;
                iMobileCallStatus = RMobileCall::EStatusAnswering;
                statusChanged = ETrue;
                break;
            case RMobileCall::EStatusConnected:
                if ( KErrMMEtelCallForbidden == aResult )
                    {
                    CMmCallList* callList = iMmPhone->CallList();
                    
                    for( TInt i=0; i< callList->GetNumberOfObjects();i++)
                        {
                        CMmDataCallTsy* call = 
                        reinterpret_cast<CMmDataCallTsy*>(
                            iMmPhone->CallList()->GetMmCallByIndex( i ) );
                    
                        // if there is call with status Answering, complete
                        // it with CALL_CAUSE_NOT_ALLOWED    
                        if( RCall::EStatusAnswering == call->Status() )
                            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETENOTIFYSTATUSCHANGE_6, "TSY: CMmDataCallTsy::CompleteNotifyStatusChange Answering not allowed!");
                            TInt errorValue = 
                                CMmCommonStaticUtility::EpocErrorCode( 
                                   KErrAccessDenied, KErrMMEtelCallForbidden );
                            call->CompleteAnswerIncomingCall( errorValue );
                            }
                        }
                    }
                // check previous status. If status is answering
                if ( RCall::EStatusAnswering == iCallStatus )
                    {
                	// don't start timers if error occurred
                	if ( KErrNone == aResult )
                		{
                    	//Start call duration monitoring
                    	iCallTimer->Start(); 
                    	//Check if start the air time duration monitoring
                    	iMmPhone->AirTimeTimerCheckStart();
                		}

                    }
                // If it was connecting and dial cancel has not been activated
                else if ( ( iCallStatus == RCall::EStatusConnecting ||
                    iCallStatus == RCall::EStatusDialling ) &&
                    CMmCallTsy::EDialCancelNotCalled == iDialCancelFlag )
                    {
                    // Start call duration monitoring
                    iCallTimer->Start(); 
                    // start air time duration monitoring
                    iMmPhone->AirTimeTimerCheckStart();
                    }

                // Don't update status if it has not changed.
                if ( RCall::EStatusConnected != iCallStatus ||
                     RMobileCall::EStatusConnected != iMobileCallStatus )
                    {
                    iCallStatus = RCall::EStatusConnected;
                    iMobileCallStatus = RMobileCall::EStatusConnected;
                    statusChanged = ETrue;
                    }
                break;
            case RMobileCall::EStatusDisconnecting:
                // When call is released, this call object becomes "used".
                // Thus set iIsFinishedDataCall to ETrue. 
                iIsFinishedDataCall = ETrue;

                // Core status
                iCallStatus = RCall::EStatusHangingUp; 
                statusChanged = ETrue;
                // Mobile status
                iMobileCallStatus = RMobileCall::EStatusDisconnecting;
                mobileStatusChanged = ETrue;

                if ( iCallDirection == RMobileCall::EMobileTerminated )
                    {
                    // CompleteDial in case remote user is busy
                    CompleteDial( aResult );
                    CompleteDialNoFdn( aResult );
                    }

                // Set last exit code
                if ( KErrGsmReleaseByUser == aResult ||
                    KErrGsmBusyUserRequest == aResult ||
                    KErrGsmCCNormalUnspecified == extendedError ||
                    KErrNone == aResult )
                    {
                    iLastExitCode = KErrNone;
                    }
                else
                    {
                    // set last exit code
                    iLastExitCode = aResult;
                    }

                // reset caps.
                if ( RCall::KCapsLoanDataPort == 
                    ( iCallCaps.iFlags & RCall::KCapsLoanDataPort ) )
                    {
                    iCallCaps.iFlags &= ~( RCall::KCapsLoanDataPort );
                    CompleteNotifyCapsChange();
                    }
                break;
            case RMobileCall::EStatusDisconnectingWithInband:
                // When call is released, this call object becomes "used".
                // Thus set iIsFinishedDataCall to ETrue. 
                iIsFinishedDataCall = ETrue;
                // Core status
                iCallStatus = RCall::EStatusHangingUp; 
                statusChanged = ETrue;
                // Mobile status
                iMobileCallStatus = 
                    RMobileCall::EStatusDisconnectingWithInband;
                mobileStatusChanged = ETrue;
                // CompleteDial in case remote user is busy
                CompleteDial( aResult );
                CompleteDialNoFdn( aResult );
                // Set last exit code
                if ( KErrGsmReleaseByUser == aResult ||
                    KErrGsmBusyUserRequest == aResult ||
                    KErrGsmCCNormalUnspecified == extendedError ||
                    KErrNone == aResult )
                    {
                    iLastExitCode = KErrNone;
                    }
                else
                    {
                    // set last exit code
                    iLastExitCode = aResult;
                    }

                //reset caps.
                if ( RCall::KCapsLoanDataPort == 
                     ( iCallCaps.iFlags & RCall::KCapsLoanDataPort ) )
                    {
                    iCallCaps.iFlags &= ~( RCall::KCapsLoanDataPort );
                    CompleteNotifyCapsChange();
                    }
                break;
            case RMobileCall::EStatusHold:
            case RMobileCall::EStatusUnknown:
            case RMobileCall::EStatusReconnectPending:
            case RMobileCall::EStatusWaitingAlternatingCallSwitch:
            case RMobileCall::EStatusTransferring:
            case RMobileCall::EStatusTransferAlerting:
            default:
                // nothing to do
                break;
            }

        //reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
            EMultimodeCallNotifyStatusChange );

        if ( ( EMultimodeCallReqHandleUnknown != reqHandle ) 
                                                        && statusChanged )
            {
            *iRetStatus = iCallStatus;
            //reset req handle. 
            iTsyReqHandleStore->ResetTsyReqHandle(
                EMultimodeCallNotifyStatusChange );

            ReqCompleted( reqHandle, ret );
            }

        // reset req handle. Returns the deleted req handle
        reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
            EMultimodeCallNotifyMobileCallStatusChange );

        if ( ( EMultimodeCallReqHandleUnknown != reqHandle ) 
                && (statusChanged || mobileStatusChanged) )
            {
            *iRetMobileCallStatus = iMobileCallStatus;
            // reset req handle. 
            iTsyReqHandleStore->ResetTsyReqHandle( 
                EMultimodeCallNotifyMobileCallStatusChange );
            ReqCompleted( reqHandle, ret );
            }

        // Update also line's status
        if ( statusChanged || mobileStatusChanged )
            {
            iMmLine->CompleteNotifyStatusChange();
            }

        // Inform extension dll about the current status. Enables dynamic 
        // capability updates. Must be done before Notifying caps change 
        if ( KErrNone == iMmCallExtInterface-> 
            CompleteNotifyStatusChange( iMobileCallStatus ) )
            { 
            CompleteNotifyMobileCallCapsChange( KErrNone );
            CompleteNotifyMobileDataCallCapsChange();
            }

        mmCall = reinterpret_cast<CMmDataCallTsy*>(
            iMmPhone->CallList()->GetMmCallByIndex( callIndex ) );
        while ( mmCall ) 
            { 
            if ( RMobileCall::EStatusIdle != mmCall->MobileCallStatus() && 
                 this != mmCall )
                { 
                if ( KErrNone ==
                    mmCall->ActiveCallExtension()-> 
                    CompleteNotifyStatusChange( mmCall->MobileCallStatus() ) )
                    { 
                    mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );
                    mmCall->CompleteNotifyMobileDataCallCapsChange();
                    }
                } 
            callIndex++;
            mmCall = reinterpret_cast<CMmDataCallTsy*>(
                iMmPhone->CallList()->GetMmCallByIndex( callIndex ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::Dial
// This CORE API method dials to the given number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::Dial(
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* aCallParams,
    TDesC* aTelNumber )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIAL_1, "TSY: CMmDataCallTsy::Dial - Req handle: %u, Call name: %S",aTsyReqHandle, iCallName);

	if( aCallParams->Length()!= 0)
		{
		if(sizeof(RCall::TCallParams) > aCallParams->Length())
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIAL_2, "TSY: CMmDataCallTsy::Dial bad size argument");
			// Complete the request with appropiate error        
			return KErrArgument;
			}
		}
	CMmCallList* callList = iMmPhone->CallList();
	TBool dialFlag( EFalse );
	
	for(TInt i=0; i< callList->GetNumberOfObjects();i++)
		{		
		CMmCallTsy* call = callList->GetMmCallByIndex( i );
		if( call->GetDialFlag() )
			{
			dialFlag = ETrue;
			i= callList->GetNumberOfObjects();
			}
		}
	
	if(!dialFlag )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIAL_3, "TSY: CMmDataCallTsy::Dial ONGOING" );
		SetDialFlag( ETrue );
		TTsyReqHandle dialHandle = 
			iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallDial );
    
		// reset exit code
		iLastExitCode = KErrNone;
	
		// reset finished data call flag
		iIsFinishedDataCall = EFalse;
	
		if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )  
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIAL_4, "TSY: Offline mode ON, Dial request is not allowed" );
			TInt ret = CMmCommonStaticUtility::EpocErrorCode(
				KErrGeneral, KErrGsmOfflineOpNotAllowed );
	
			// Complete the request with appropiate error
			ReqCompleted ( aTsyReqHandle, ret ); 
			}
	
		// check that status is Idle
		else if ( RMobileCall::EStatusIdle != iMobileCallStatus )
			{
			// The request cannot be forwarded since this call object
			// is still in use.
			// Complete request with status value informing the client
			// about the situation.
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIAL_5, "TSY: CMmDataCallTsy::Dial - KErrNotReady");
			ReqCompleted( aTsyReqHandle, KErrNotReady );
			}
		else if ( 0 < dialHandle )
			{
			// The request is already in processing because of previous request
			// Complete request with status value informing the client about 
			// the situation.
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIAL_6, "TSY: CMmDataCallTsy::Dial - KErrServerBusy");
			ReqCompleted( aTsyReqHandle, KErrServerBusy );
			}
		else
			{
		   
			TInt ret( KErrNone );
			TInt trapError( KErrNone );
			TCallOwnership ownerShip = EOwnedFalse;
	
			if(aCallParams->Length() >0)
				{
				RCall::TCallParamsPckg* paramsPckg = reinterpret_cast<RCall::TCallParamsPckg*>(
					const_cast<TDesC8*>( aCallParams ) );
				RCall::TCallParams& params = ( *paramsPckg )();
	
				// save call params
				iCallParams.iSpeakerControl = params.iSpeakerControl;
				iCallParams.iSpeakerVolume = params.iSpeakerVolume;
				iCallParams.iInterval = params.iInterval;
				iCallParams.iWaitForDialTone = params.iWaitForDialTone;
				}
			else
				{
				iCallParams.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
				iCallParams.iSpeakerVolume = RCall::EMonitorSpeakerVolumeOff;
				iCallParams.iInterval = 0;
				iCallParams.iWaitForDialTone = RCall::EDialToneWait;
				}
		  
			// set call direction
			iCallDirection = RMobileCall::EMobileOriginated;
	
			// Check if there already is an active or connecting call
			TInt numberOfObjects = iMmPhone->CallList()->
				GetNumberOfObjects();
			for ( TInt i = 0; i < numberOfObjects; i++ )
				{
				CMmDataCallTsy* mmCall = reinterpret_cast<CMmDataCallTsy*>(
					iMmPhone->CallList()->GetMmCallByIndex( i ) );
				if ( !( RMobileCall::EStatusUnknown == 
							mmCall->MobileCallStatus() ||
						RMobileCall::EStatusIdle == 
							mmCall->MobileCallStatus() ||
						RMobileCall::EStatusDisconnecting == 
							mmCall->MobileCallStatus() ||
						RMobileCall::EStatusDisconnectingWithInband == 
							mmCall->MobileCallStatus() ) )
					{
					ret = KErrEtelCallAlreadyActive;
					break;
					}
				}
	
			// Return value can also be priorityclient! 
			ownerShip = CheckOwnership( aTsyReqHandle );
	
			if ( KErrNone == ret && ( EOwnedUnowned == ownerShip || 
				 EOwnedPriorityClient == ownerShip ) ) 
				{
				SetOwnership( aTsyReqHandle );
	
				if ( KErrNone == ret )
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIAL_7, "TSY: CMmDataCallTsy::Dial - DialDataCall");
					TRAP( trapError,
						ret = iMmCallExtInterface->DialDataCallL(
							iCallMode, aCallParams, aTelNumber );
						);
					if ( KErrNone != trapError )
						{
						//error handling. DialDataCall() leaved.
						ret = trapError;
						}
					// If dial succeeded, continue
					if ( KErrNone == ret )
						{
						RPhone::TStatus modemStatus;
						modemStatus.iModemDetected = RPhone::EDetectedPresent;
						modemStatus.iMode = RPhone::EModeEstablishingLink;
						iMmPhone->CompleteNotifyModemDetected( modemStatus );
	
						//update core status
						iCallCaps.iFlags &= ~(
							RCall::KCapsDial | RCall::KCapsConnect );
						iCallCaps.iFlags |= RCall::KCapsHangUp;
						//complete core caps change
						CompleteNotifyCapsChange();
						}
					}
				}
	
			if ( KErrNone != ret )
				{
				ReqCompleted( aTsyReqHandle, ret );
				ClearCallStatus();
				}
			else
				{
#ifdef REQHANDLE_TIMER
				// set timer for the request
				SetTypeOfResponse( EMultimodeCallDial, aTsyReqHandle );
#else
				// set timer
				iTsyReqHandleStore->SetTsyReqHandle(
					EMultimodeCallDial, aTsyReqHandle );
#endif
				}
			}
	
		return KErrNone;
		}
    else
       	return KErrServerBusy;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CompleteDial
// Completes a Dial request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::CompleteDial(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEDIAL_1, "TSY: CMmDataCallTsy::CompleteDial - Result: %{TSymbianErrorCodes}", aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEDIAL_2, "TSY: CMmDataCallTsy::CompleteDial - Call Id: %d, Call name: %S",iCallId, iCallName);
    
    // Set dial flag to false
    SetDialFlag( EFalse );
    
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallDial );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        if ( aResult != KErrNone )
            {
            // The creation has failed.
            iLastExitCode = aResult;
            ClearCallStatus();

            //Dial has failed, update core status - dial possible again
            iCallCaps.iFlags |= RCall::KCapsDial;
            iCallCaps.iFlags &= ~( RCall::KCapsHangUp );

            //complete caps change notification
            CompleteNotifyCapsChange();
            }

        ReqCompleted( reqHandle, aResult );

        }

    // Set also new call caps to enable dataport loaning!
    if ( RCall::EStatusConnected == iCallStatus &&
         KErrNone == aResult )
        {
        iCallCaps.iFlags |= RCall::KCapsLoanDataPort;
        iMmCallExtInterface->AddGSMCallCaps(
            RCall::KCapsLoanDataPort );

        //complete caps change notification
        CompleteNotifyCapsChange();
        CompleteNotifyMobileCallCapsChange( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::AnswerIncomingCall
// This CORE API method is used for answering to an incoming call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::AnswerIncomingCall(
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_ANSWERINCOMINGCALL_1, "TSY: CMmDataCallTsy::AnswerIncomingCall. \n\t\t\t Handle: %d",aTsyReqHandle);

    TTsyReqHandle answerCallHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallAnswer );

    if ( 0 < answerCallHandle )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        TInt ret( KErrNone );
        //call object is under use again
        iIsFinishedDataCall = EFalse;
        //reset exit code
        iLastExitCode = KErrNone;

        if ( EOwnedUnowned == CheckOwnership( aTsyReqHandle ) )
            {
            // If Answering is allowed (KCapsAnswer is set) or
            // if automatic answer is to be enabled (call status idle)
            if ( iCallCaps.iFlags & RCall::KCapsAnswer ||
                RCall::EStatusIdle == iCallStatus )
                {
                SetOwnership( aTsyReqHandle );

                if ( KErrNone == ret )
                    {
                    //Answer incoming call
                    TRAPD( trapError,
                        ret = iMmCallExtInterface->
                                  AnswerIncomingDataCallL( iCallId );
                        );

                    if ( KErrNone != trapError )
                        {
                        //error handling. Object cannot be created.
                        ret = trapError;
                        }

                    if ( KErrNone == ret )
                        {
                        //Remove answering, dialing and connecting 
                        //capabilities
                        iCallCaps.iFlags |= RCall::KCapsHangUp;
                        iCallCaps.iFlags &= ~( RCall::KCapsAnswer | 
                            RCall::KCapsDial | RCall::KCapsConnect );
                        CompleteNotifyCapsChange();
                        }
                    }
                }
            else
                {
                ret = KErrNotReady;
                }
            }
        else if ( EOwnedTrue == CheckOwnership( aTsyReqHandle ) ||
            EOwnedPriorityClient == CheckOwnership( aTsyReqHandle ) )
            {
            // SOS architechture Call Back functionality tries to answer 
            // incoming call even before the current MO call is fully 
            // released. So, save pointer to this CMmDataCallTsy object 
            // into CMmPhoneTsy.
            iMmPhone->SetWaitingCallForData( this );
            //Remove answering, dialing and connecting capabilities.
            iCallCaps.iFlags |= RCall::KCapsHangUp;
            iCallCaps.iFlags &= ~(
                RCall::KCapsAnswer | RCall::KCapsDial | RCall::KCapsConnect );
            CompleteNotifyCapsChange();
            }
        // Else return error
        else
            {
            ret = KErrEtelNotCallOwner;
            }

        //error handling
        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            ClearCallStatus();
            }
        else
            {
#ifdef REQHANDLE_TIMER
            //set timer for the request
            SetTypeOfResponse( EMultimodeCallAnswer, aTsyReqHandle );
#else
            //timer set
            iTsyReqHandleStore->SetTsyReqHandle(
                EMultimodeCallAnswer, aTsyReqHandle );
#endif
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::AnswerIncomingCallCancel
// TSY has started a request and it is not possible to then cancel this 
// request. The best thing for the TSY to do in this case is to call
// HangUp method to do this and set a flag so that we can know that
// AnswerIncomingCallCancel handling is going on.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::AnswerIncomingCallCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_ANSWERINCOMINGCALLCANCEL_1, "TSY: CMmDataCallTsy::AnswerIncomingCallCancel. Req handle: %u, Call name: %S",aTsyReqHandle, iCallName);

    TInt ret( KErrNone );

    if( iAnswerCancelFlag )
        {
        ret = KErrServerBusy;
        }
    else
        {
        iAnswerCancelFlag = ETrue;

        if ( 0 < iCallId )
            {
            HangUp( aTsyReqHandle );
            }
        else 
            {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_ANSWERINCOMINGCALLCANCEL_2, "TSY: CMmDataCallTsy::AnswerIncomingCallCancel -- iCallStatus = %d", iCallStatus);
            if ( RCall::EStatusUnknown == iCallStatus )
                {               
                // Cancel automatic answering

                //Create package
                CCallDataPackage package;
                //Set call id and call mode
                package.SetCallIdAndMode( iCallId, iCallMode );
                // This is needed, messhandler tries to get two pointers. 
                // Without this second pointer is NULL and crashes
                TBool autoStChangeDisable( EFalse );
                TInt hangUpCause( KErrGsmReleaseByUser );
                //Pack call parameters and mobile call info
                package.PackData( &hangUpCause, &autoStChangeDisable );

                //Send request to the Domestic OS layer.
                TRAPD( leaveError,
                    ret = iMessageManager->HandleRequestL( 
                         EEtelCallHangUp, &package );
                    );

                //send failure
                if ( KErrNone != ret || KErrNone != leaveError )
                    {
                    ReqCompleted( aTsyReqHandle, ret );
                    }
                }
            CompleteAnswerIncomingCall( KErrCancel );
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CompleteAnswerIncomingCall
// Completes answer request to client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::CompleteAnswerIncomingCall(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEANSWERINCOMINGCALL_1, "TSY: CMmDataCallTsy::CompleteAnswerIncomingCall - Result: %{TSymbianErrorCodes}",aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEANSWERINCOMINGCALL_2, "TSY: CMmDataCallTsy::CompleteAnswerIncomingCall - Call Id: %d, Call name: %S",iCallId, iCallName);

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallAnswer );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        if ( KErrNone != aResult )
            {
            TInt extendedError(
                CMmCommonStaticUtility::ExtendedErrorCode( aResult ) );

            if ( KErrGsmCCNormalUnspecified == extendedError )
                {
                //Call MT released while answer request was ongoing
                //Set Symbian error value to disconnected.
                aResult = CMmCommonStaticUtility::EpocErrorCode( 
                    KErrGeneral, KErrDisconnected );
                }
            else
                {
                iCallCaps.iFlags |= RCall::KCapsAnswer | RCall::KCapsDial;
                iCallCaps.iFlags &= ~( RCall::KCapsHangUp );

                CompleteNotifyCapsChange();
                }

            iLastExitCode = aResult;

            ClearCallStatus();
            }

        ReqCompleted( reqHandle, aResult );

        }

    // Set also new call caps to enable dataport loaning!
    if ( RCall::EStatusConnected == iCallStatus &&
         KErrNone == aResult )
        {
        iCallCaps.iFlags |= RCall::KCapsLoanDataPort;
        iMmCallExtInterface->AddGSMCallCaps(
            RCall::KCapsLoanDataPort );

        //complete caps change notification
        CompleteNotifyCapsChange();
        CompleteNotifyMobileCallCapsChange( KErrNone );
        }
      
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::HangUp
// This CORE API method disconnects the call. Used with normal voice calls, 
// emergency calls as well as data calls. DialCancel also uses this method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::HangUp(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_HANGUP_1, "TSY: CMmDataCallTsy::HangUp - Req. handle: %u, Call name: %S",aTsyReqHandle, iCallName);

    TInt hangUpCause( KErrNone );
    TInt ret( KErrNone );

    TTsyReqHandle hangUpHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallHangUp );
    TTsyReqHandle dialHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallDial );
    TTsyReqHandle dialHandleNoFdnCheck = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallDialNoFdnCheck );        
            
    if ( 0 < hangUpHandle || iHangUpFlag )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_HANGUP_2, "TSY: CMmDataCallTsy::HangUp - KErrServerBusy");
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else if ( RCall::EStatusIdle == iCallStatus &&
        EMultimodeCallReqHandleUnknown == dialHandle )
        {
        //Call object is already in idle state. Complete HangUp request with
        //error.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_HANGUP_3, "TSY: CMmDataCallTsy::HangUp - KErrNotReady");
        ReqCompleted( aTsyReqHandle, KErrNotReady );
        }
    else
        {
        // If this was not called by DialCancel or AnswerIncomingCallCancel
        if ( CMmCallTsy::EDialCancelNotCalled == iDialCancelFlag &&
            !iAnswerCancelFlag )
            {
#ifdef REQHANDLE_TIMER
            //set timer for the request
            SetTypeOfResponse( EMultimodeCallHangUp, aTsyReqHandle );
#else
            //save HangUp request handle, set timer
            iTsyReqHandleStore->SetTsyReqHandle(
                EMultimodeCallHangUp, aTsyReqHandle );
#endif
            }

        //decide the hangup cause
        if ( RCall::EStatusRinging == iCallStatus )
            {
            iLastExitCode = KErrGsmCallRejected;
            hangUpCause = KErrGsmBusyUserRequest;
            }
        else
            {
            hangUpCause = KErrGsmReleaseByUser;
            }

        // Call handling
        //If the call is ringing e.g. MT call, we do not have ownership
        //yet. So let the client reject the call without checking 
        //ownership.
        if ( RCall::EStatusRinging == iCallStatus )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_HANGUP_4, "TSY: CMmDataCallTsy::HangUp - Reject incoming call");
            }
        //Phone Application is the first client that is started, it 
        //will always be the priority client and thus able to hangup calls
        //Owner of the call is also able to hangup the call.
        else if ( ( CCallBase::CheckPriorityClient( aTsyReqHandle ) ) ||
            ( CheckOwnership( aTsyReqHandle ) != EOwnedFalse ) )
            {
            ret = KErrNone;
            }
        // If call is not owned, complete with error
        else if ( CCallBase::EOwnedTrue != 
            CheckOwnership( aTsyReqHandle ) )
            {
            iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallHangUp );
            ret = KErrEtelNotCallOwner;
            ReqCompleted( aTsyReqHandle, KErrEtelNotCallOwner );
            }    

        if ( KErrNone == ret )
            {
            //Create package
            CCallDataPackage package;
            //Set call id and call mode
            package.SetCallIdAndMode( iCallId, iCallMode );

            // This is needed, messhandler tries to get two pointers. Without 
            // this second pointer is NULL and crashes
            TBool autoStChangeDisable = EFalse;
            //Pack call parameters and mobile call info
            package.PackData( &hangUpCause, &autoStChangeDisable );
            //Send request to the Domestic OS layer.
            TRAPD( leaveError,
                ret = iMessageManager->HandleRequestL( 
                                            EEtelCallHangUp, &package );
                );

            //send failure
            if ( KErrNone != ret || KErrNone != leaveError )
                {
                iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallHangUp );
                ReqCompleted( aTsyReqHandle, ret );
                }
            else
                {
                //update core status - hangup not possible now.
                //remove also dataport caps
                iCallCaps.iFlags &= ~(
                    RCall::KCapsHangUp | RCall::KCapsLoanDataPort );

                iMmCallExtInterface->RemoveGSMCallCaps(
                    RCall::KCapsHangUp | RCall::KCapsLoanDataPort );

                //complete core caps change
                CompleteNotifyCapsChange();
                CompleteNotifyMobileCallCapsChange( KErrNone );
                //set HangUp flag to ongoing     
                iHangUpFlag = ETrue;
                }
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CompleteHangUp
// Completes call hangup (both normal and emergency calls). If the cause value 
// is different than the sent one, the call release requset has failed. In 
// this case the causeValue describes the fail cause. Otherwise the call  
// releasing has succeeded.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::CompleteHangUp(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEHANGUP_1, "TSY: CMmDataCallTsy::CompleteHangUp.\n  \t\t\t Result: %{TSymbianErrorCodes}",aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEHANGUP_2, "TSY: CMmDataCallTsy::CompleteHangUp - Call Id: %d, Call name: %S", iCallId, iCallName);

    TInt ret( KErrNone );

    // Complete for HangUp request
    // (not to DialCancel or AnswerIncomingCallCancel)
    if ( CMmCallTsy::EDialCancelNotCalled == iDialCancelFlag &&
         !iAnswerCancelFlag )
        {
        //reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle =
            iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallHangUp );

        if ( EMultimodeCallReqHandleUnknown != reqHandle )
            {
            //if hangup request failed
            if ( KErrNone != aResult )
                {
                //Hangup failed, it should be still possible to carry out
                iCallCaps.iFlags |= RCall::KCapsHangUp;
                iMmCallExtInterface->AddGSMCallCaps(
                    RCall::KCapsHangUp );

                //Call status is not idle. HangUp request failed, complete 
                //HangUp request with error.
                if ( RCall::EStatusIdle != iCallStatus )
                	{
                    // ignore KErrGsmReleaseByUser, which means that the call ended
                    // because the local user released the call, and must be treated
                    // as a normal return code from the LTSY
                    ret = ( (aResult == KErrGsmReleaseByUser) ? KErrNone : aResult );
                	}
                }
            else
                {
                //Dial hangup has succeeded, update core status - 
                //dial possible again
                iCallCaps.iFlags |= RCall::KCapsDial;

                if ( KErrGsmCallRejected != iLastExitCode )
                    {
                    iLastExitCode = KErrNone;
                    }

                //Bring back also the connecting capability
                iCallCaps.iFlags |= RCall::KCapsConnect;
                //If client refuses to answer, remove the answering capability
                iCallCaps.iFlags &= ~( RCall::KCapsAnswer );
                }
            ReqCompleted( reqHandle, ret );

            //complete caps change notification
            CompleteNotifyCapsChange();
            CompleteNotifyMobileCallCapsChange( KErrNone );

            //Change modem state to idle
            RPhone::TStatus modemStatus;
            modemStatus.iModemDetected = RPhone::EDetectedPresent;
            modemStatus.iMode = RPhone::EModeIdle;
            iMmPhone->CompleteNotifyModemDetected( modemStatus );
            // Reset call extension attributes
            iMmCallExtInterface->ResetInternalAttributes();
            }
        }
    // Complete for DialCancel or AnswerIncomingCallCancel, success
    else if ( KErrNone == aResult ) 
        { 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEHANGUP_3, "TSY: CMmDataCallTsy::CompleteHangUp - EMultimodeCallDial");
        // Find out if this is cancelling of Dial
        TTsyReqHandle cancelHandle =
            iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallDial ); 

        if ( NULL == cancelHandle ) 
            { 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEHANGUP_4, "TSY: CMmDataCallTsy::CompleteHangUp - EMultimodeCallDialNoFdnCheck");
	        // Find out if this is cancelling of DialNoFdnCheck
	        cancelHandle = iTsyReqHandleStore->
            ResetTsyReqHandle( EMultimodeCallDialNoFdnCheck );
        
            if ( NULL == cancelHandle )
                { 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEHANGUP_5, "TSY: CMmDataCallTsy::CompleteHangUp - EMultimodeCallAnswer");
                // Find out if this is cancelling of AnswerIncomingCall
                cancelHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                    EMultimodeCallAnswer );
                }
        	}
        
        if ( 0 < cancelHandle ) 
            { 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEHANGUP_6, "TSY: CMmDataCallTsy::CompleteHangUp with KErrCancel");
             
            // Complete with error value KErrCancel
            ReqCompleted( cancelHandle, KErrCancel );

            // Cancel has succeeded, update core status -
            // dial possible again
            iCallCaps.iFlags |= RCall::KCapsDial;
            // Bring back also the connecting capability
            iCallCaps.iFlags |= RCall::KCapsConnect;
            // If client refuses to answer, remove the answering capability
            iCallCaps.iFlags &= ~( RCall::KCapsAnswer );
            // Complete caps change notification
            CompleteNotifyCapsChange();

            // Change modem state to idle
            RPhone::TStatus modemStatus;
            modemStatus.iModemDetected = RPhone::EDetectedPresent;
            modemStatus.iMode = RPhone::EModeIdle;
            iMmPhone->CompleteNotifyModemDetected( modemStatus );
            // Reset call extension attributes
            iMmCallExtInterface->ResetInternalAttributes();
            }
        } 
    // Cancel failed and the request succeeded (connected)
    else if ( RCall::EStatusConnected == iCallStatus ) 
        { 
        // Find out if this is cancelling of Dial
        TTsyReqHandle cancelHandle =
            iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallDial ); 

        if ( NULL == cancelHandle ) 
            { 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEHANGUP_7, "TSY: CMmDataCallTsy::CompleteHangUp - cancelling of EMultimodeCallDialNoFdnCheck");
            // Find out if this is cancelling of DialNoFdnCheck
            TTsyReqHandle cancelHandle =
                iTsyReqHandleStore->ResetTsyReqHandle( 
                    EMultimodeCallDialNoFdnCheck ); 
            
            if ( NULL == cancelHandle )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEHANGUP_8, "TSY: CMmDataCallTsy::CompleteHangUp - cancelling of AnswerIncomingCall");
                // Find out if this is cancelling of AnswerIncomingCall
                cancelHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
                    EMultimodeCallAnswer );
                }
            }


        // Cancel to Dial or AnswerIncomingCall
        if ( 0 < cancelHandle ) 
            { 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEHANGUP_9, "TSY: CMmDataCallTsy::CompleteHangUp - with KErrNone");
            // Complete with success (KErrNone)
            ReqCompleted( cancelHandle, KErrNone ); 

            // Succeeded, update core status - hangup possible again
            iCallCaps.iFlags |= RCall::KCapsHangUp;
            // Complete caps change notification
            CompleteNotifyCapsChange();
            } 
        } 

    // Reset cancel flags
    iDialCancelFlag = CMmCallTsy::EDialCancelNotCalled;
    iAnswerCancelFlag = EFalse;
    // reset the HangUp flag
    iHangUpFlag = EFalse;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::GetOwnershipStatus
// Get call ownership status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::GetOwnershipStatus(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TOwnershipStatus* aOwnershipStatus )
    {
    TCallOwnership ownerShip = CheckOwnership( aTsyReqHandle );
    
    switch ( ownerShip )
        {
        case EOwnedUnowned:
            *aOwnershipStatus = RCall::EOwnershipUnowned;
            break;
        case EOwnedTrue:
            *aOwnershipStatus = RCall::EOwnershipOwnedByThisClient;
            break;
        case EOwnedFalse:
            *aOwnershipStatus = RCall::EOwnershipOwnedByAnotherClient;
            break;
        case EOwnedPriorityClient:
            if ( CCallBase::CheckPriorityClient( aTsyReqHandle ) )
                {
                *aOwnershipStatus = RCall::EOwnershipOwnedByThisClient;
                }
            else
                {
                *aOwnershipStatus = RCall::EOwnershipOwnedByAnotherClient;
                }
            break;
        default:
            //all possible values are switched, nothing to do here...
            break;
        }

    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::TransferOwnership
// Transfers the call ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::TransferOwnership(
    const TTsyReqHandle aTsyReqHandle )
    {
    // If call is not owned, complete with error
    if ( CCallBase::EOwnedTrue != CheckOwnership( aTsyReqHandle ) )
        {
        ReqCompleted( aTsyReqHandle, KErrEtelNotCallOwner );
        }
    // If list is empty, complete with error
    else if ( iList->iAcquireList.IsEmpty() )
        {
        ReqCompleted( aTsyReqHandle, KErrEtelNoClientInterestedInThisCall );
        }
    else
        {
        CAcquireEntry* entry = iList->iAcquireList.First();
        if ( entry )
            {
            SetOwnership( entry->iTsyReqHandle );
            ReqCompleted( entry->iTsyReqHandle, KErrNone );
            iList->Remove( entry );
            ReqCompleted( aTsyReqHandle, KErrNone );
            }
        else 
            {
            ReqCompleted( aTsyReqHandle, KErrGeneral );
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::AcquireOwnership
// Acquire the call ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::AcquireOwnership(
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt trapError;
    // Check if the call is unowned?
    if ( CheckOwnership( aTsyReqHandle ) == CCallBase::EOwnedUnowned )
        {
        // It is, return with error
        ReqCompleted( aTsyReqHandle, KErrEtelCallNotActive );
        }
    else 
        {
        CAcquireEntry* entry = NULL;
        // Call is owned, add this req handle to acquire list

// TRAP macro releases memory while exception caught and trapError != KErrNone
        TRAP( trapError, ( entry = CAcquireEntry::NewL( aTsyReqHandle ) ) );
        if ( trapError != KErrNone )
            {
            //error handling. Object cannot be created.
            ReqCompleted( aTsyReqHandle, trapError );
            }
        else if ( NULL != entry )
            {
            iList->iAcquireList.AddLast( *entry );
            }
        // coverity[leaked_storage]
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::AcquireOwnershipCancel
// Cancel the call ownership acquire.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::AcquireOwnershipCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    CAcquireEntry* entry = iList->FindByTsyReqHandle( aTsyReqHandle );
    if ( NULL != entry )
        {
        iList->Remove( entry );
        ReqCompleted( aTsyReqHandle, KErrCancel );
        }
    else
        {
        ReqCompleted( aTsyReqHandle, KErrNotFound );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::RelinquishOwnership
// Relinquish the call ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::RelinquishOwnership()
    {
    if ( iList->iAcquireList.IsEmpty() ) 
        {
        SetUnowned();
        if ( RCall::EStatusConnected != iCallStatus )
            {
            RelinquishOwnershipCompleted( KErrNone );
            }
        }
    else
        {
        CAcquireEntry* entry = iList->iAcquireList.First();
        if ( entry ) 
            {
            SetOwnership( entry->iTsyReqHandle );
            ReqCompleted( entry->iTsyReqHandle, KErrNone );
            iList->Remove( entry );
            }
        RelinquishOwnershipCompleted( KErrNone );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::RegisterNotification
// RegisterNotification is called when the server recognises that this 
// notification is being posted for the first time on this sub-session object. 
// It enables the TSY to "turn on" any regular notification messages that it 
// may receive from DOS. Currently does not really do anything but returns 
// KErrNone to ETel server in case of known notification request type. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::RegisterNotification(
    const TInt aIpc )
    {
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EEtelCallNotifyHookChange:        
        case EEtelCallNotifyStatusChange:
        case EEtelCallNotifyDurationChange: 
        case EEtelCallCapsChangeNotification:
        case EMobileCallNotifyCallEvent:
        case EMobileCallNotifyMobileCallStatusChange:
        case EMobileCallNotifyRemotePartyInfoChange:
        case EMobileCallNotifyPrivacyConfirmation:
        case EMobileCallNotifyTrafficChannelConfirmation:
        case EMobileCallNotifyHscsdInfoChange:
        case EMobileCallNotifyMobileDataCallCapsChange:
        case EMobileCallNotifyAlternatingCallSwitch:
        case EMobileCallNotifyMobileCallCapsChange:
        case EMobileCallNotifyVoiceFallback:
        case EMobileCallNotifyUUSCapsChange:
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
// CMmDataCallTsy::DeregisterNotification
// DeregisterNotification is called when the server recognises that this 
// notification will not be posted again because the last client to have a 
// handle on this sub-session object has just closed the handle. It enables 
// the TSY to "turn off" any regular notification messages that it may receive  
// from DOS. Currently does not really do anything but returns KErrNone to  
// ETel server in case of known notification request type. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::DeregisterNotification(
    const TInt aIpc )
    {
    TInt ret ( KErrNone );

    switch ( aIpc )
        {
        case EEtelCallNotifyHookChange:        
        case EEtelCallNotifyStatusChange:
        case EEtelCallNotifyDurationChange: 
        case EEtelCallCapsChangeNotification:
        case EMobileCallNotifyCallEvent:
        case EMobileCallNotifyMobileCallStatusChange:
        case EMobileCallNotifyRemotePartyInfoChange:
        case EMobileCallNotifyPrivacyConfirmation:
        case EMobileCallNotifyTrafficChannelConfirmation:
        case EMobileCallNotifyHscsdInfoChange:
        case EMobileCallNotifyMobileDataCallCapsChange:
        case EMobileCallNotifyAlternatingCallSwitch:
        case EMobileCallNotifyMobileCallCapsChange:
        case EMobileCallNotifyVoiceFallback:
        case EMobileCallNotifyUUSCapsChange:
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
// CMmDataCallTsy::ClearCallStatus
// Clears internal call status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::ClearCallStatus()
    {
    SetUnowned();
    CMmCallTsy::ClearCallStatus();
    }

#ifdef REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmDataCallTsy::SetTypeOfResponse
// Sets the type of response for a given Handle. Automatic mode includes an 
// automatic response in case of non response from the Domestic OS Layer in a 
// specified time.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::SetTypeOfResponse(
    const TInt aReqHandleType,
    const TTsyReqHandle aTsyReqHandle )
    {
    //Sets the type of response for a given Handle. Automatic mode includes an 
    //automatic response in case of non response from the Licencee specific
    //TSY in a specified time.
    TInt timeOut( 0 );

    //example switch
    switch ( aReqHandleType )
        {
        case EMultimodeCallDial:
        case EMultimodeCallDialNoFdnCheck:
            timeOut = KMmCallDialTimeOut;
            break;
        case EMultimodeCallAnswer:
            // If call mode is data and client is expecting call back from
            // server, set timeout value to KMmCallAnswerCallBackTimeOut
            if ( this == iMmPhone->WaitingCallForData() )
                {
                timeOut = KMmCallAnswerCallBackTimeOut;
                }
            else
                {
                timeOut = KMmCallAnswerTimeOut;
                }
            break;
        case EMultimodeCallHangUp:
            timeOut = KMmCallHangUpTimeOut;
            break;
        case EMultimodeCallTransfer:
            timeOut = KMmCallTransferTimeOut;
            break;
        case EMultimodeCallSetDynamicHscsdParams:
            timeOut = KMmCallSetDynamicHscsdParamsTimeOut;
            break;
        //Can't use timer:
        // - all notifications
            //case EMultimodeCallNotifyStatusChange: 
            //case EMultimodeCallNotifyDurationChange:
            //case EMultimodeCallCapsChangeNotification:
            //case EMultimodeCallNotifyMobileCallStatusChange: 
            //case EMultimodeCallNotifyCallEvent:
            //case EMultimodeCallNotifyRemotePartyInfoChange:
            //case EMultimodeCallNotifyMobileCallCapsChange:
            //case EMultimodeCallNotifyDataCallCapsChange:
            //case EMultimodeCallNotifyHscsdInfoChange:
            //case EMultimodeCallNotifyPrivacyConfirmation:

        case EMultimodeMobileCallHold:
        case EMultimodeMobileCallResume:
        case EMultimodeMobileCallSwap:
        case EMultimodeMobileCallDeflectCall:
        case EMultimodeMobileCallDialEmergencyCall:
        case EMultimodeCallGoOneToOne:
        default:
            //does not use timer
            iTsyReqHandleStore->SetTsyReqHandle( 
                aReqHandleType, 
                aTsyReqHandle );
            break;
        }

    if ( timeOut > 0 )
        {
        //the timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle(
            aReqHandleType, aTsyReqHandle, timeOut );
        }
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::Complete
// Completes the request due timer expiration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::Complete(
    TInt aReqHandleType,
    TInt aError )
    {
    //All possible TSY req handle types are listed in the
    //switch case below. 
    switch( aReqHandleType )
        {
        //cases handled with automatic completion
        case EMultimodeCallSetDynamicHscsdParams:
            CompleteSetDynamicHscsdParams( aError );
            break;

        //Can't use timer:
        // - all notifications
            //case EMultimodeCallNotifyStatusChange: 
            //case EMultimodeCallNotifyDurationChange:
            //case EMultimodeCallCapsChangeNotification:
            //case EMultimodeCallNotifyMobileCallStatusChange: 
            //case EMultimodeCallNotifyCallEvent:
            //case EMultimodeCallNotifyRemotePartyInfoChange:
            //case EMultimodeCallNotifyMobileCallCapsChange:
            //case EMultimodeCallNotifyDataCallCapsChange:
            //case EMultimodeCallNotifyHscsdInfoChange:
            //case EMultimodeCallNotifyPrivacyConfirmation:

        case EMultimodeCallDial:
        case EMultimodeCallDialNoFdnCheck:
        case EMultimodeCallAnswer:
        case EMultimodeCallHangUp:
        case EMultimodeMobileCallHold:
        case EMultimodeMobileCallResume:
        case EMultimodeMobileCallSwap:
        case EMultimodeMobileCallDeflectCall:
        case EMultimodeMobileCallDialEmergencyCall:
        case EMultimodeCallTransfer: 
        case EMultimodeCallGoOneToOne:
        default:
            CMmCallTsy::Complete( aReqHandleType, aError );
            break;
        }
    }
#endif

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CallComingForWaitingCall
// Starts to answer an incoming call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::CallComingForWaitingCall()
    {
    //Incoming call detected, which is directed to this call object
    //which should be waiting for incoming call
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_CALLCOMINGFORWAITINGCALL_1, "TSY: CMmDataCallTsy::CallComingForWaitingCall");

    TInt ret( KErrNone );

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_CALLCOMINGFORWAITINGCALL_2, "TSY: CMmDataCallTsy::AnswerIncomingCall - Data call");
    //Answer incoming call
    TRAPD( trapError,
        ret = iMmCallExtInterface->AnswerIncomingDataCallL( iCallId );
        );

    if ( KErrNone != trapError )
        {
        //error handling. Object cannot be created.
        ret = trapError;
        }

    if ( KErrNone == ret )
        {
        // SOS architechture Call Back functionality tries to answer 
        // incoming call even before the current MO call is fully 
        // released. This is done by dialup agent (CsdAgx/CsdAgt), 
        // which does not wait for the previous call to get 
        // disconnected. As the call is now answered, NULL the pointer
        // to this CMmDataCallTsy object from CMmPhoneTsy.
        iMmPhone->SetWaitingCallForData( NULL );
        }
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::IsFinishedData
// Returns boolean that indicates if call object is already been used in
// data calls. This means that call has been already released or disconnected.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmDataCallTsy::IsFinishedData() const
    {
    return iIsFinishedDataCall;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::LoanDataPort
// This CORE API method loans dataport to client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::LoanDataPort(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TCommPort* aCommPort )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_LOANDATAPORT_1, "TSY: CMmDataCallTsy::LoanDataPort - Client taking control: %S",iCallName );

    TInt ret( KErrNone );

    // Check if the port is loaned!
    if ( iLoanedCommPort.iPort.Compare( KNullDesC) == 0 )
        {
        // Check ownership
        TCallOwnership ownerShip = CheckOwnership( aTsyReqHandle );
        // If not yet owned, take ownership.
        if ( EOwnedUnowned == ownerShip || EOwnedPriorityClient == ownerShip )
            {
            SetOwnership( aTsyReqHandle );
            }
        // Else check if this req handle already owns the call.
        else if ( CCallBase::EOwnedTrue != ownerShip )
            {
            ret = KErrEtelNotCallOwner;
            }

        // If port can be loaned, request access to dataport
        if ( ret == KErrNone )
            {
            //Create package
            CCallDataPackage package;
            //Set call id and call mode
            package.SetCallIdAndMode( iCallId, iCallMode );
            //Pack commport
            package.PackData( aCommPort );

            //Send request to the Domestic OS layer.
            TRAPD( trapError,
                ret = iMessageManager->HandleRequestL(
                    EEtelCallLoanDataPort, &package );
                );

            if ( KErrNone != trapError )
                {
                //error handling, leaved.
                ret = trapError;               
                }

            if ( KErrNone == ret )
                {
                iLoanedCommPort.iCsy.Copy( aCommPort->iCsy );
                iLoanedCommPort.iPort.Copy( aCommPort->iPort );

                // If dataport was available, changed new call caps and 
                // complete notification!
                iCallCaps.iFlags |=
                    RCall::KCapsData |
                    RCall::KCapsHangUp |
                    RCall::KCapsRecoverDataPort;
                iCallCaps.iFlags &= ~( RCall::KCapsLoanDataPort );

                iMmCallExtInterface->AddGSMCallCaps(
                    RCall::KCapsRecoverDataPort );
                iMmCallExtInterface->RemoveGSMCallCaps(
                    RCall::KCapsLoanDataPort );

                CompleteNotifyCapsChange();
                CompleteNotifyMobileCallCapsChange( KErrNone );
                }
            else
            	{
            	SetUnowned();
            	}
            }
        }
    else
        {
        ret = KErrEtelPortAlreadyLoaned;
        }

    // Complete request if result is KErrNone.
    // Return possible error to CCallBase base class because it sets
    // iLoanDataport to TRUE if KErrNone is returned. This may cause
    // serious problems when the data call object is closed and data port
    // is not really loaned. ETel completes request in case where error is
    // returned.
    if ( KErrNone == ret )    
        {
        ReqCompleted( aTsyReqHandle, KErrNone );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::LoanDataPortCancel
// Cancels dataport loaning to client. LoanDataPort always
// completes immediately, this is never used.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::LoanDataPortCancel(
    const TTsyReqHandle )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::RecoverDataPort
// This CORE API method recovers dataport from client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::RecoverDataPort(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_RECOVERDATAPORT_1, "TSY: CMmDataCallTsy::RecoverDataPort - Client returning control: %S",iCallName );

    TInt ret( KErrNone );

    // Check if the port is loaned!
    if ( iLoanedCommPort.iPort.Compare( KNullDesC) != 0 )
        {
        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode( iCallId, iCallMode );
        //Pack commport
        package.PackData( &iLoanedCommPort );

        //Send request to the Domestic OS layer.
        TRAPD( trapError,
            ret = iMessageManager->HandleRequestL(
                EEtelCallRecoverDataPort, &package );
            );

        if ( KErrNone != trapError )
            {
            //error handling, leaved.
            ret = trapError;
            }

        iLoanedCommPort.iCsy.Zero();
        iLoanedCommPort.iPort.Zero();

        // Set new call caps!
        iCallCaps.iFlags |= RCall::KCapsLoanDataPort;
        iCallCaps.iFlags &= ~( RCall::KCapsRecoverDataPort );

        iMmCallExtInterface->AddGSMCallCaps(
            RCall::KCapsLoanDataPort );
        iMmCallExtInterface->RemoveGSMCallCaps(
            RCall::KCapsRecoverDataPort );
        }
    else
        {
        ret = KErrEtelPortNotLoanedToClient;
        }

    if(0 != aTsyReqHandle)
        {
        ReqCompleted( aTsyReqHandle, ret );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::RecoverDataPortAndRelinquishOwnership
// Recovers dataport from client and relinquishes ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::RecoverDataPortAndRelinquishOwnership()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_RECOVERDATAPORTANDRELINQUISHOWNERSHIP_1, "TSY: CMmDataCallTsy::RecoverDataPortAndRelinquishOwnership - \Client returning control: %S", iCallName );

    TInt ret( KErrNone );

    // Check if the port is loaned!
    if ( iLoanedCommPort.iPort.Compare( KNullDesC) != 0 )
        {
        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode( iCallId, iCallMode );
        //Pack commport
        package.PackData( &iLoanedCommPort );

        //Send request to the Domestic OS layer.
        TRAPD( trapError,
            ret = iMessageManager->HandleRequestL(
                EEtelCallRecoverDataPort, &package );
            );

        if ( KErrNone != trapError )
            {
            //error handling, leaved.
            ret = trapError;
            }

        iLoanedCommPort.iCsy.Zero();
        iLoanedCommPort.iPort.Zero();

        if ( iList->iAcquireList.IsEmpty() )
            {
            SetUnowned();
            
            if ( KErrNone == ret )
                {
                // Call owner is closing data call handle. Call 
                // RecoverDataPortAndRelinquishOwnershipCompleted method.
                RecoverDataPortAndRelinquishOwnershipCompleted( KErrNone );
                }            
            }
        else
            {
            CAcquireEntry* entry = iList->iAcquireList.First();
            if ( entry )
                {
                SetOwnership( entry->iTsyReqHandle );
                ReqCompleted( entry->iTsyReqHandle, KErrNone );
                iList->Remove( entry );
                }
            // Set new call caps!
            iCallCaps.iFlags |= RCall::KCapsLoanDataPort;
            iCallCaps.iFlags &= ~( RCall::KCapsRecoverDataPort );

            iMmCallExtInterface->AddGSMCallCaps(
                RCall::KCapsLoanDataPort );
            iMmCallExtInterface->RemoveGSMCallCaps(
                RCall::KCapsRecoverDataPort );
            }
        }
    else
        {
        ret = KErrEtelPortNotLoanedToClient;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::Connect
// Set correct data call attributes, depending on parameter extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::Connect(
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* aCallParams )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_CONNECT_1, "TSY: CMmDataCallTsy::Connect");
    if ( CheckOwnership( aTsyReqHandle ) == CCallBase::EOwnedUnowned )
        {
        SetOwnership( aTsyReqHandle );
        }
    TDes8* callParams = reinterpret_cast<TDes8*>(const_cast<TDesC8*> (aCallParams));
    if(callParams->MaxLength() == 0)
    	{
    	// default parameters...
        iCallParams.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
        iCallParams.iSpeakerVolume = RCall::EMonitorSpeakerVolumeOff;
        iCallParams.iInterval = 0;
        iCallParams.iWaitForDialTone = RCall::EDialToneWait;
        ReqCompleted( aTsyReqHandle, KErrNone );

        return KErrNone;
    	}
    if(callParams->MaxLength() == sizeof(RCall::TCallParams))
    	{
    	// only RCall parameters
        RCall::TCallParamsPckg* paramsPckg = 
            reinterpret_cast<RCall::TCallParamsPckg*>(callParams);
        RCall::TCallParams& params = ( *paramsPckg )();

        iCallParams.iSpeakerControl = params.iSpeakerControl;
        iCallParams.iSpeakerVolume = params.iSpeakerVolume;
        iCallParams.iInterval = params.iInterval;
        iCallParams.iWaitForDialTone = params.iWaitForDialTone;
        ReqCompleted( aTsyReqHandle, KErrNone );

        return KErrNone;
    	}
    if(sizeof(RMobileCall::TMobileCallParamsV1) > aCallParams->Length())
       	{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_CONNECT_2, "TSY: CMmDataCallTsy::Connect bad size argument");
        // Complete the request with appropiate error        
        return KErrArgument;
    	}
    RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 = 
        reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*> ( callParams );
    RMobileCall::TMobileCallParamsV1& paramsV1 = ( *paramsPckgV1 )();

        if ( CheckOwnership( aTsyReqHandle ) == CCallBase::EOwnedUnowned )
            {
            SetOwnership( aTsyReqHandle );
            }
        

        iCallParams.iSpeakerControl = paramsV1.iSpeakerControl;
        iCallParams.iSpeakerVolume = paramsV1.iSpeakerVolume;
        iCallParams.iInterval = paramsV1.iInterval;
        iCallParams.iWaitForDialTone = paramsV1.iWaitForDialTone;

        if ( ( paramsV1.ExtensionId() == KETelExtMultimodeV1 ) ||
            ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileDataCallParamsV1 ) ||
            ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileDataCallParamsV2 ) ||
            ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileDataCallParamsV8 ) ||
            ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileHscsdCallParamsV1 )||
            ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileHscsdCallParamsV2 )||
            ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileHscsdCallParamsV7 )||
            ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileHscsdCallParamsV8 ))
            {
            iCallParams.iIdRestrict = paramsV1.iIdRestrict;
            iCallParams.iCug = paramsV1.iCug;
            iCallParams.iAutoRedial = paramsV1.iAutoRedial;
            }

        if ( ( paramsV1.ExtensionId() ==
                RMobileCall::KETelMobileDataCallParamsV1 ) ||
             ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileDataCallParamsV2 ) ||
             ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileDataCallParamsV8 ) ||
             ( paramsV1.ExtensionId() ==
                RMobileCall::KETelMobileHscsdCallParamsV1 )||
             ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileHscsdCallParamsV2 )||
             ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileHscsdCallParamsV7 )||
             ( paramsV1.ExtensionId() == 
                RMobileCall::KETelMobileHscsdCallParamsV8 ))
            {
            iMmCallExtInterface->Connect( aCallParams );
            }

        ReqCompleted( aTsyReqHandle, KErrNone );

    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::ConnectChancel
// Cancels connecting of a (data) call (Not Supported).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::ConnectCancel(
    const TTsyReqHandle )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::GetMobileDataCallCaps
// Get data call caps.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::GetMobileDataCallCaps(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aCaps )
    {
    TInt ret( KErrArgument );   
    
    if ( sizeof ( RMobileCall::TMobileCallDataCapsV1 ) <= aCaps->MaxLength() )
        {
        RMobileCall::TMobileCallDataCapsV1Pckg* callDataCapsV1Pckg = 
            reinterpret_cast<RMobileCall::TMobileCallDataCapsV1Pckg*>( aCaps );
        RMobileCall::TMobileCallDataCapsV1& callDataCapsV1 = 
            ( *callDataCapsV1Pckg )();
        
        if ( KETelExtMultimodeV1 == callDataCapsV1.ExtensionId() )
            {
            // The call will only have data capabilities if this call has been 
            // opened from the phone's data line.
            ret = KErrNone;
            iMmCallExtInterface->GetMobileDataCallCapsV1( callDataCapsV1 );
            }
        }

    ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::NotifyMobileDataCallCapsChange
// Notifies change of the data call capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::NotifyMobileDataCallCapsChange(
    const TTsyReqHandle,
    TDes8* aCaps )
    {
    TInt ret( KErrArgument );   
    
    if ( sizeof ( RMobileCall::TMobileCallDataCapsV1 ) <= aCaps->MaxLength() )
        {
        RMobileCall::TMobileCallDataCapsV1Pckg* callDataCapsV1Pckg = 
            reinterpret_cast<RMobileCall::TMobileCallDataCapsV1Pckg*>( aCaps );
        RMobileCall::TMobileCallDataCapsV1& callDataCapsV1 = 
            ( *callDataCapsV1Pckg )();  
        
        if ( KETelExtMultimodeV1 == callDataCapsV1.ExtensionId() )
            {
            ret = KErrNone;
            iRetDataCallCaps = aCaps;
            iReqHandleType = EMultimodeCallNotifyDataCallCapsChange;
            }
        }    
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::NotifyMobileDataCallCapsChangeCancel
// Cancels MobileDataCallCaps change notifications.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::NotifyMobileDataCallCapsChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetDataCallCaps = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallNotifyDataCallCapsChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CompleteNotifyMobileDataCallCapsChange
// Complete a NotifyMobileDataCallCapsChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::CompleteNotifyMobileDataCallCapsChange()
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallNotifyDataCallCapsChange );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        RMobileCall::TMobileCallDataCapsV1Pckg* callDataCapsV1Pckg = 
            reinterpret_cast<RMobileCall::TMobileCallDataCapsV1Pckg*>( 
                iRetDataCallCaps );
        RMobileCall::TMobileCallDataCapsV1& callDataCapsV1 = 
            ( *callDataCapsV1Pckg )();
                        
        iMmCallExtInterface->GetMobileDataCallCapsV1( callDataCapsV1 );
        
        iRetDataCallCaps = NULL;

        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::GetBearerServiceInfo
// Get bearer service info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::GetBearerServiceInfo(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TBearerService* aBearerService )
    {
    // If the call is not connected, the request will return KErrNotFound.
    TInt ret( KErrNotFound );
    // Reset info variables. Info is also unknown for voice call.
    aBearerService->iBearerCaps =
        RCall::KBearerCapsCompressionNone | RCall::KBearerCapsProtocolUnknown;
    aBearerService->iBearerSpeed = RCall::EBearerDataUnknown;

    if ( RCall::EStatusConnected == iCallStatus )
        {
        ret = iMmCallExtInterface->GetBearerServiceInfo( aBearerService );
        }

    ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::SetDynamicHscsdParams
// Set HSCSD dynamic data parameters.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::SetDynamicHscsdParams(
    const TTsyReqHandle aTsyReqHandle,
    RMobileCall::TMobileCallAiur* aAiur,
    TInt* aRxTimeslots )
    {
    TInt ret( KErrNone );
    // HSCSD parameters can be set only if this call has been opened
    // from the phone's data line.
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_SETDYNAMICHSCSDPARAMS_1, "TSY: CMmDataCallTsy::SetDynamicHscsdParams.\n  \t\t\t Air Intrf: %d\n  \t\t\t RX slots: %d", *aAiur, *aRxTimeslots);

    iReqHandleType = EMultimodeCallSetDynamicHscsdParams;

    TRAP_IGNORE( 
        ret = iMmCallExtInterface->SetDynamicHscsdParamsL(
            aAiur, aRxTimeslots );
        );

    // If not a data call or extension returned error.
    if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, KErrNotFound );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::SetDynamicHscsdParamsCancel
// Cancel SetDynamicHscsdParams request.
// Not possible, as SetDynamicHscsdParams request is completed immediately.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::SetDynamicHscsdParamsCancel(
    const TTsyReqHandle ) const
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CompleteSetDynamicHscsdParams
// Complete a SetDynamicHscsdParams request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::CompleteSetDynamicHscsdParams(
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETESETDYNAMICHSCSDPARAMS_1, "TSY: CMmDataCallTsy::CompleteSetDynamicHscsdParams");
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallSetDynamicHscsdParams );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::GetCurrentHscsdInfo
// Get current HSCSD info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::GetCurrentHscsdInfo(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aHSCSDInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_GETCURRENTHSCSDINFO_1, "TSY: CMmDataCallTsy::GetCurrentHscsdInfo");
    TInt ret( KErrNone );
    // HSCSD info can be read only if this call has been opened from the 
    // phone's data line and call is connected.
    // Connected here means that TSY gas has received CONNECT: XXXX from modem
    // When CONNECT: XXXX has been received, TSY has completed clients request
    // which can be dial or answer.
    // Thus those req handles should be unknown, otherwise the call is not yet
    // connected. 
    if ( ( EMultimodeCallReqHandleUnknown ==
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallDial ) ) &&
        ( EMultimodeCallReqHandleUnknown ==
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallAnswer ) ) &&
        ( EMultimodeCallReqHandleUnknown ==
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallDialNoFdnCheck ) ) )
        {
        ret = iMmCallExtInterface->GetCurrentHscsdInfo( aHSCSDInfo );
        }
    else
        {
        RMobileCall::TMobileCallHscsdInfoV1Pckg* hscsdInfoPckg = 
            reinterpret_cast<RMobileCall::TMobileCallHscsdInfoV1Pckg*>(
                aHSCSDInfo );
        RMobileCall::TMobileCallHscsdInfoV1& hscsdInfo = (*hscsdInfoPckg)();

        hscsdInfo.iAiur = RMobileCall::EAiurBpsUnspecified;
        hscsdInfo.iRxTimeSlots = 0;
        hscsdInfo.iTxTimeSlots = 0;
        hscsdInfo.iCodings = RMobileCall::ETchCodingUnspecified;

        ret = KErrEtelCallNotActive;
        }

    ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::NotifyHscsdInfoChange
// Notifies change of the HSCSD info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::NotifyHscsdInfoChange(
    const TTsyReqHandle,
    TDes8* aHSCSDInfo )
    {
    // HSCSD info can be read only if this call has been opened
    // from the phone's data line.
    // Save request handle and ptr to aHSCSDInfo.
    iRetHscsdInfo = aHSCSDInfo;
    iReqHandleType = EMultimodeCallNotifyHscsdInfoChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::NotifyHscsdInfoChangeCancel
//  Cancels HSCSD info change notifications.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::NotifyHscsdInfoChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt ret( KErrNone );

    iRetHscsdInfo = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallNotifyHscsdInfoChange );
    ret = KErrCancel;

    ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CompleteNotifyHscsdInfoChange
// Complete a NotifyHscsdInfoChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::CompleteNotifyHscsdInfoChange()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETENOTIFYHSCSDINFOCHANGE_1, "TSY: CMmDataCallTsy::CompleteNotifyHscsdInfoChange");
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallNotifyHscsdInfoChange );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        TInt ret = iMmCallExtInterface->GetCurrentHscsdInfo( iRetHscsdInfo );
        iRetHscsdInfo = NULL;
        ReqCompleted( reqHandle, ret );
        }
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::GetMobileDataCallRLPRange
// Get Data Call RLP Range.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::GetMobileDataCallRLPRange(
    const TTsyReqHandle aTsyReqHandle,
    TInt*,
    TDes8* )
    {
    ReqCompleted( aTsyReqHandle, KErrNotSupported );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::DialNoFdnCheck
// This CORE API method dials to the given number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataCallTsy::DialNoFdnCheck(
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* aCallParams,
    TDesC* aTelNumber )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIALNOFDNCHECK_1, "TSY: CMmDataCallTsy::DialNoFdnCheck - Req handle: %u, Call name: %S",aTsyReqHandle, iCallName);

	CMmCallList* callList = iMmPhone->CallList();
	TBool dialFlag( EFalse );
	
	for(TInt i=0; i< callList->GetNumberOfObjects();i++)
		{		
		CMmCallTsy* call = callList->GetMmCallByIndex( i );
		if( call->GetDialFlag() )
			{
			dialFlag = ETrue;
			i= callList->GetNumberOfObjects();
			}
		}
	
	if( !dialFlag )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIALNOFDNCHECK_2, "TSY: CMmDataCallTsy::DialNoFdnCheck ONGOING" );
    		
    	SetDialFlag( ETrue );
    		
        TTsyReqHandle dialHandle = iTsyReqHandleStore->GetTsyReqHandle(
        		EMultimodeCallDialNoFdnCheck );
        
        // reset exit code
        iLastExitCode = KErrNone;
    
        // reset finished data call flag
        iIsFinishedDataCall = EFalse;
    
        if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )  
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIALNOFDNCHECK_3, "TSY: CMmDataCallTsy - DialNoFdnCheck - Offline mode ON, Dial request is not allowed" );
            TInt ret = CMmCommonStaticUtility::EpocErrorCode(
                KErrGeneral, KErrGsmOfflineOpNotAllowed );
            SetDialFlag( EFalse );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIALNOFDNCHECK_4, "TSY: CMmDataCallTsy::DialNoFdnCheck - RF inactive -> dial-flag false" );
            // Complete the request with appropiate error
            ReqCompleted ( aTsyReqHandle, ret ); 
            }

        // check that status is Idle
        else if ( RMobileCall::EStatusIdle != iMobileCallStatus )
            {
            // Request cannot be forwarded since this call object
            // is still in use.
            // Complete request with status value informing the client
            // about the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIALNOFDNCHECK_5, "TSY: CMmDataCallTsy::DialNoFdnCheck - KErrNotReady");
            ReqCompleted( aTsyReqHandle, KErrNotReady );
            }
        else if ( 0 < dialHandle )
            {
            // Request is already in processing because of previous request
            // Complete request with status value informing the client about 
            // the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIALNOFDNCHECK_6, "TSY: CMmDataCallTsy::DialNoFdnCheck - KErrServerBusy");
            ReqCompleted( aTsyReqHandle, KErrServerBusy );
            }
        else
            {
            TInt ret( KErrNone );
            TInt trapError( KErrNone );
            TCallOwnership ownerShip = EOwnedFalse;
    
            RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 = 
                reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>(
                    const_cast<TDesC8*>( aCallParams ) );
            RMobileCall::TMobileCallParamsV1& paramsV1 = ( *paramsPckgV1 )();
    
            // save call params
            iCallParams.iSpeakerControl = paramsV1.iSpeakerControl;
            iCallParams.iSpeakerVolume = paramsV1.iSpeakerVolume;
            iCallParams.iInterval = paramsV1.iInterval;
            iCallParams.iWaitForDialTone = paramsV1.iWaitForDialTone;
    
            // set call direction
            iCallDirection = RMobileCall::EMobileOriginated;
    
            // Check if there already is an active or connecting call
            TInt numberOfObjects = iMmPhone->CallList()->
                GetNumberOfObjects();
            for ( TInt i = 0; i < numberOfObjects; i++ )
                {
                CMmDataCallTsy* mmCall = reinterpret_cast<CMmDataCallTsy*>(
                    iMmPhone->CallList()->GetMmCallByIndex( i ) );
                if ( !( RMobileCall::EStatusUnknown == 
                            mmCall->MobileCallStatus() ||
                        RMobileCall::EStatusIdle == 
                            mmCall->MobileCallStatus() ||
                        RMobileCall::EStatusDisconnecting == 
                            mmCall->MobileCallStatus() ||
                        RMobileCall::EStatusDisconnectingWithInband == 
                            mmCall->MobileCallStatus() ) )
                    {
                    ret = KErrEtelCallAlreadyActive;
                    break;
                    }
                }
    
            // Return value can also be priorityclient! 
            ownerShip = CheckOwnership( aTsyReqHandle );
    
            if ( KErrNone == ret && ( EOwnedUnowned == ownerShip || 
                 EOwnedPriorityClient == ownerShip ) ) 
                {
                SetOwnership( aTsyReqHandle );
    
                if ( KErrNone == ret )
                    {
                    SetDialTypeId( KMultimodeCallTypeIDNoFdnCheck );
                    
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_DIALNOFDNCHECK_7, "TSY: CMmDataCallTsy::DialNoFdnCheck - DialDataCall");
                    TRAP( trapError,
                        ret = iMmCallExtInterface->DialDataCallL(
                            iCallMode, aCallParams, aTelNumber );
                        );
                    if ( KErrNone != trapError )
                        {
                        //error handling. DialDataCall() leaved.
                        ret = trapError;
                        }
                    // If dial succeeded, continue
                    if ( KErrNone == ret )
                        {
                        RPhone::TStatus modemStatus;
                        modemStatus.iModemDetected = RPhone::EDetectedPresent;
                        modemStatus.iMode = RPhone::EModeEstablishingLink;
                        iMmPhone->CompleteNotifyModemDetected( modemStatus );
    
                        //update core status
                        iCallCaps.iFlags &= ~(
                            RCall::KCapsDial | RCall::KCapsConnect );
                        iCallCaps.iFlags |= RCall::KCapsHangUp;
                        //complete core caps change
                        CompleteNotifyCapsChange();
                        }
                    }
                }
    
            if ( KErrNone != ret )
                {
                ReqCompleted( aTsyReqHandle, ret );
                ClearCallStatus();
                }
            else
                {
#ifdef REQHANDLE_TIMER
                // set timer for the request
                SetTypeOfResponse( EMultimodeCallDialNoFdnCheck,
                		aTsyReqHandle );
#else
                // set timer
                iTsyReqHandleStore->SetTsyReqHandle(
                    EMultimodeCallDialNoFdnCheck, aTsyReqHandle );
#endif
                }
            }
    
        return KErrNone;
        } 
    else
        {
        return KErrServerBusy;
        }  	
    }

// ---------------------------------------------------------------------------
// CMmDataCallTsy::CompleteDialNoFdn
// Completes a Dial request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::CompleteDialNoFdn(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEDIALNOFDN_1, "TSY: CMmDataCallTsy::CompleteDialNoFdn - Result: %{TSymbianErrorCodes}", aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEDIALNOFDN_2, "TSY: CMmDataCallTsy::CompleteDialNoFdn - Call Id: %d, Call name: %S",iCallId, iCallName);
    
    // Set dial flag to false
    SetDialFlag( EFalse );
    
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallDialNoFdnCheck );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        if ( aResult != KErrNone )
            {
            // The creation has failed.
            iLastExitCode = aResult;
            ClearCallStatus();

            //Dial has failed, update core status - dial possible again
            iCallCaps.iFlags |= RCall::KCapsDial;
            iCallCaps.iFlags &= ~( RCall::KCapsHangUp );

            //complete caps change notification
            CompleteNotifyCapsChange();
            }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEDIALNOFDN_3, "TSY: CMmDataCallTsy::CompleteDialNoFdn before ReqComplete %{TSymbianErrorCodes}", aResult );
        ReqCompleted( reqHandle, aResult );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_COMPLETEDIALNOFDN_4, "TSY: CMmDataCallTsy::CompleteDialNoFdn - set dialtype to none ");
	    //Set dial type to none 
	    SetDialTypeId( KMultimodeCallTypeDialNone );
        }

    // Set also new call caps to enable dataport loaning!
    if ( RCall::EStatusConnected == iCallStatus &&
         KErrNone == aResult )
        {
        iCallCaps.iFlags |= RCall::KCapsLoanDataPort;
        iMmCallExtInterface->AddGSMCallCaps(
            RCall::KCapsLoanDataPort );

        //complete caps change notification
        CompleteNotifyCapsChange();
        CompleteNotifyMobileCallCapsChange( KErrNone );
        }
    }

#ifdef TF_LOGGING_ENABLED
// ---------------------------------------------------------------------------
// CMmCallTsy::ReqCompleted
// Overloads CTelObject::ReqCompleted for logging purposes. It prints the 
// aTsyReqHandle and aError variable in the log file and then calls 
// CTelObject::ReqCompleted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataCallTsy::ReqCompleted(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aError )
    {
    //Overloads CTelObject::ReqCompleted for logging purposes. It 
    //prints the aTsyReqHandle and aError variable in the log file and then 
    //calls CTelObject::ReqCompleted.
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_REQCOMPLETED_1, "TSY: CMmDataCallTsy::ReqCompleted. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", aTsyReqHandle, aError, this);

    CTelObject::ReqCompleted(aTsyReqHandle,aError);

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATACALLTSY_REQCOMPLETED_2, "<-- TSY: CMmPhoneTsy::ReqCompleted");
    }
#endif

//  End of File  
