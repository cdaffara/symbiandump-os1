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
#include "cmmvoicecalltsyTraces.h"
#endif

#include "cmmvoicecalltsy.h"

#include <featmgr/featurecontrol.h>
#include <featureuids.h>
#include "cmmvoicelinetsy.h"
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmcalllist.h"
#include "CMmCommonStaticUtility.h"
#include "MmTsy_numberOfSlots.h"
#include "MmTsy_timeoutdefs.h"
#include "cmmmessagemanagerbase.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/gsmerror.h>
#include "cmmcallgsmwcdmaext.h"
#include "cmmdtmftsy.h"
#include "cmmconferencecalltsy.h"
#include "CMmCustomTsy.h"
#include "cmmconferencecallgsmwcdmaext.h"
#include <etelmmerr.h>
#include <ctsy/pluginapi/mtelephonyaudiocontrol.h>

// ======== MEMBER FUNCTIONS ========

CMmVoiceCallTsy::CMmVoiceCallTsy()
    {
    }

CMmVoiceCallTsy* CMmVoiceCallTsy::NewL( 
    CMmPhoneTsy* aMmPhone, CMmVoiceLineTsy* aMmLine,
    RMobilePhone::TMobileService aMode, 
    TDes& aName, 
    CMmMessageManagerBase* aMessageManager,
    MTelephonyAudioControl* aTelephonyAudioControl )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_NEWL_1, "TSY: CMmVoiceCallTsy::NewL. Call name: %S", aName);

    CMmVoiceCallTsy* mmCall = NULL;

    //check input parameters
    if ( aMmPhone != NULL && aMmLine != NULL && ( 
        aMode == RMobilePhone::EVoiceService ||
        aMode == RMobilePhone::EAuxVoiceService ) )
        {
        mmCall = new( ELeave ) CMmVoiceCallTsy;
        CleanupClosePushL( *mmCall );
        mmCall->iMmPhone = aMmPhone;
        mmCall->iMmLine = aMmLine;
        mmCall->iCallName = aName;
        mmCall->iCallMode = aMode;
        mmCall->iMessageManager = aMessageManager;
        mmCall->iTelephonyAudioControl = aTelephonyAudioControl;
        mmCall->iTelephonyAudioControlSetup = EFalse;

        mmCall->ConstructL( aMode );
        CleanupStack::Pop();
        }
    return mmCall;
    }

CMmVoiceCallTsy::~CMmVoiceCallTsy()
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DTOR_1, "TSY: CMmVoiceCallTsy::~CMmVoiceCallTsy. Call deleted iCallId:%d iCallName:%S", iCallId, iCallName);
    }


// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::InitInternalAttributes
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::InitInternalAttributes()
    {
    //initialise internal attributes of base class
    CMmCallTsy::InitInternalAttributes();
    //set call capabilities
    iCallCaps.iFlags = RCall::KCapsVoice | RCall::KCapsDial;
    //not yet an emergency call
    iEmergencyCall = EFalse;
    
    i3rdPartyEmergencyNumberCheckDone = EFalse;
    
    iISVCallParams = NULL;
    
    iIsDialISV = EFalse;
    
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::DoExtFuncL
// DoExtFuncL is called by the Etel server when it has an "extended", i.e. 
// non-core ETel request for the TSY. To process a request handle, request  
// type and request data are passed to the TSY. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DOEXTFUNCL_1, "TSY: CMmVoiceCallTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TInt)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    TInt ret( KErrNone );

    TAny* dataPtr = aPackage.Ptr1();

    // The request data has to extracted from TDataPackage and the TAny*
    // pointers have to be "cast" to the expected request data type

    // NOTE! Do not put any Cancel methods here.
    switch ( aIpc )
        {
      // Emergency Calls
        // Dial Emergency Call
        case EMobileCallDialEmergencyCall:
            ret = DialEmergencyCall( aTsyReqHandle, aPackage );
            break;
      // Call Status
        // Get Call Status
        case EMobileCallGetMobileCallStatus:
            ret = GetMobileCallStatus( aTsyReqHandle,
              reinterpret_cast<RMobileCall::TMobileCallStatus*>( dataPtr ) );
            break;
        // Notify Call Status Change
        case EMobileCallNotifyMobileCallStatusChange:
            ret = NotifyMobileCallStatusChange(
              reinterpret_cast<RMobileCall::TMobileCallStatus*>( dataPtr ) );
            break;
        // Place Call on Hold
        case EMobileCallHold:
            ret = HoldL( aTsyReqHandle );
            break;
        // Resume a Held Call
        case EMobileCallResume:
            ret = ResumeL( aTsyReqHandle );
            break;
        // Swap Between an On-going and an Held Call
        case EMobileCallSwap:
            ret = SwapL( aTsyReqHandle );
            break;
        // Transfer One Call to Remote Party of Another Call
        case EMobileCallTransfer:
            ret = TransferL( aTsyReqHandle );
            break;
        // Go "One-to-One" within a Conference Call
        case EMobileCallGoOneToOne:
            ret = GoOneToOneL( aTsyReqHandle );
            break;
        // Notify Call Event
        case EMobileCallNotifyCallEvent:
            ret = NotifyCallEvent(
                reinterpret_cast<RMobileCall::TMobileCallEvent*>( dataPtr ) );
            break;
        // Call Control
        // Get Call Capabilities
        case EMobileCallGetMobileCallCaps: 
            ret = GetMobileCallCaps( aTsyReqHandle, aPackage.Des1n() );
            break;
        
		//Notify Call Capabilities Change
        case EMobileCallNotifyMobileCallCapsChange: 
            ret = NotifyMobileCallCapsChange(aPackage.Des1n() );
            break;
        
		// Call Information
        // Get Mobile Call Information
        case EMobileCallGetMobileCallInfo:
            ret = GetMobileCallInfo( aTsyReqHandle, aPackage.Des1n() );
            break;
        // Notify Change of Remote Party Information
        case EMobileCallNotifyRemotePartyInfoChange:
            ret = NotifyRemotePartyInfoChange(
                reinterpret_cast<
                    RMobileCall::TMobileCallRemotePartyInfoV1Pckg*>(
                        aPackage.Des1n()) );
            break;
        // Order to notify if privacy is confirmed
        case EMobileCallNotifyPrivacyConfirmation:
            ret = NotifyPrivacyConfirmation(
                reinterpret_cast<RMobilePhone::TMobilePhonePrivacy*>(
                    dataPtr ) );
            break;
        //AnswerIncomingCall ( by 3rd Party client )
        case EMobileCallAnswerISV:
                ret = AnswerIncomingCall( aTsyReqHandle, aPackage.Des1n() );
                break;
        //Dial a call ( by 3rd Party client )
        case EMobileCallDialISV:
            i3rdPartyEmergencyNumberCheckDone = EFalse;
            iIsDialISV = ETrue;
            ret = Dial( aTsyReqHandle, aPackage.Des1n(), aPackage.Des2u() );
            break;       
        // Activate User-To-User Signalling service
        case EMobileCallActivateUUS:
            ret = ActivateUUS( aTsyReqHandle, 
                REINTERPRET_CAST( RMobileCall::TMobileCallUUSRequestV1Pckg*,
                aPackage.Des1n()) );
            break;
        // Receive User-To-User Information, TDes16 descriptor pointer
        case EMobileCallReceiveUUI:
            ret = ReceiveUUI( aTsyReqHandle, 
                REINTERPRET_CAST( RMobileCall::TMobileCallUUI*,
                aPackage.Des1u() ) );
            break;            
        // Dial a call ( DialNoFdnCheck )
        case EMobileCallDialNoFdnCheck:
    		ret = DialNoFdnCheck( aTsyReqHandle, aPackage.Des1n(), 
    		    aPackage.Des2u() ); 
    		break;
          
        // Unsupported features
        case EMobileCallGetMobileDataCallCaps: 
        case EMobileCallNotifyMobileDataCallCapsChange: 
        case EMobileCallGetMobileDataCallRLPRange:
        case EMobileCallSetDynamicHscsdParams:
        case EMobileCallGetCurrentHscsdInfo:
        case EMobileCallNotifyHscsdInfoChange:
        case EMobileCallSwitchAlternatingCall:
        case EMobileCallNotifyAlternatingCallSwitch:
        case EMobileCallSetPrivacy:
        case EMobileCallSetTrafficChannel:
        case EMobileCallNotifyTrafficChannelConfirmation:
        case EMobileCallGetUUSCaps:
        case EMobileCallNotifyUUSCapsChange:
        case EMobileCallSendUUI:
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
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DOEXTFUNCL_2, "TSY: CMmVoiceCallTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::ReqModeL
// ReqModeL is called from the ETel server's CTelObject::ReqAnalyserL in order 
// to check the type of request it has.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmVoiceCallTsy::ReqModeL(
    const TInt aIpc )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_REQMODEL_1, "TSY: CMmVoiceCallTsy::ReqModeL. IPC:%{TIPCNamesList}",aIpc);

    CTelObject::TReqMode ret( 0 ); // default return value
    
    switch ( aIpc )
        {
        // No Flow Control
        // All disabled
        // TSYs wishing to implement their own buffering algorithm will
        // place all requests in this category.

        // Get methods that do not use DOS and return immediately.
        // Flow control not required.
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
        case EMobileCallSendUUI:
        case EMobileCallHangupWithUUI:
        case EMobileCallAnswerWithUUI:
        //Other methods that do not use DOS and return immediately.
        //Flow control not required.
        case EEtelCallAcquireOwnership:
        case EEtelCallTransferOwnership:
        case EEtelCallSetFaxSettings:
        //Methods that can propably take a long time and cannot therefore be
        //flow controlled. Solution: All these methods must check req handle
        //table before handling the request. In case that the request table
        //indicates that same method has been called and has not been
        //completed, the method should return KErrServerBusy.
        case EMobileCallTransfer:
        //This is also included here due to the fact that Emergency call
        //should never be blocked by flow control. KErrServerBusy returned
        //when already already dialing.
        case EMobileCallDialEmergencyCall:
        //HangUp cannot be flow controlled. Client should for example be able
        //to terminate a call while another call is e.g. in Dialling state.
        case EEtelCallHangUp:
        //Answer was mixed with Hangup.
        //Therefore this is also No flow control type.
        case EEtelCallAnswer:
        case EMobileCallAnswerISV:
        // dial is not flow controlled as compeltion may take a while and that
        // blocks for example PS data activation in 3G while dialing.
        case EEtelCallDial:
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
        case EMobileCallActivateUUS:
        //Data call related methods that should be handled one at the time.
        case EEtelCallLoanDataPort:
        case EEtelCallRecoverDataPort:
        case EMobileCallSwitchAlternatingCall:
        case EMobileCallSetDynamicHscsdParams:
        case EMobileCallSetPrivacy:
        case EMobileCallSetTrafficChannel:
        case EMobileCallDialISV:
        case EMobileCallDialNoFdnCheck:
            ret = KReqModeFlowControlObeyed;
            break;
        //ReqModePostImmediately
        //Requests that notify a client about a change of state, where
        //the TSY needs to distinguish between different clients.
            //ret = KReqModeRePostImmediately;

        //KReqModeMultipleCompletionEnabled
        //(a) commands that may take some time, but which the TSY can
        //    handle more than one of concurrently, or
        //(b) notifications that the TSY does not wish to be re-posted
        //    immediately, so the server does no buffering.
            //ret = KReqModeMultipleCompletionEnabled;

        //KReqModeMultipleCompletionEnabled | KReqModeFlowControlObeyed
        //A command that may take some time and which the TSY can only
        //deal with one at a time.
            //ret = KReqModeMultipleCompletionEnabled |
            //    KReqModeFlowControlObeyed;
            //break;

        //Notifications
        //KReqModeMultipleCompletionEnabled | ReqModePostImmediately
        //Requests that notify a client about a change of state.
        //Since these requests do not require the issuing of any modem
        //commands, they do not have to obey flow control. 
        //The TSY never gets more than one of these outstanding per CTelObject
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
        case EMobileCallReceiveUUI:
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
// CMmVoiceCallTsy::NumberOfSlotsL
// NumberOfSlotsL is called by the server when it is registering a new 
// NOTIFICATION. It enables the TSY to tell the server how many buffer slots 
// to allocate for "repost immediately" notifications that may trigger before 
// clients collect them.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::NumberOfSlotsL(
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
        case EMobileCallReceiveUUI:
            numberOfSlots = KMmCallReceiveUUISlots;
            break;
        default:
            // Unknown or invalid Call IPC
            User::Leave( KErrNotSupported );
            break;
        }
    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::CancelService
// CancelService is called by the ETel server when it is "cleaning-up" any 
// still outstanding asynchronous requests before closing a client's 
// sub-session. This will happen if a client closes its R-class handle without 
// cancelling outstanding asynchronous requests. Only Mobile API requests are 
// handled here.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::CancelService(
    const TInt aIpc,
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_CANCELSERVICE_1, "TSY: CMmVoiceCallTsy::CancelService. IPC: %{TIPCNamesList}, Req handle: %d", (TUint)aIpc, (TUint)aTsyReqHandle);

    TInt ret( KErrNone );

    //When the clients close their sub-sessions (eg. by calling RPhone::Close)
    //they may not have cancelled all their outstanding asynchronous requests
    //before closing.  It is up to the ETel server to clean up in this
    //situation, so the server will find the list of outstanding requests
    //related to that sub-session object and pass these outstanding IPC 
    //request numbers,one at a time,to the CancelService function in the TSY.
    switch ( aIpc )
        {
        //TSY has started a request and it is not possible to then cancel this
        //request. The best thing for the TSY to do in this case is to proceed
        //as though the Cancel never happened. The server's call to the TSY
        //cancel function will return synchronously. The TSY then continues to
        //wait for the original acknowledgement and when it receives it,
        //the TSY will complete the original request.
        case EMobileCallHold:
        case EMobileCallResume:
        case EMobileCallSwap:
        case EMobileCallGoOneToOne:
        case EMobileCallDeflect:
        case EMobileCallTransfer:
        case EMobileCallNotifyTrafficChannelConfirmation:
            break;
        //Cancel methods that are not supported
        case EMobileCallSetDynamicHscsdParams:
        case EMobileCallNotifyMobileDataCallCapsChange:
        case EMobileCallNotifyHscsdInfoChange:

        case EMobileCallSwitchAlternatingCall:
        case EMobileCallNotifyAlternatingCallSwitch:
        case EMobileCallGetMobileDataCallRLPRange:
        case EMobileCallNotifyVoiceFallback:
        case EMobileCallNotifyUUSCapsChange:
        case EMobileCallActivateUUS:
        case EMobileCallSendUUI:
        case EMobileCallHangupWithUUI:
        case EMobileCallAnswerWithUUI:
            ret = KErrNotSupported;
            break;
        //Notification Cancels, no special requirements.
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
        case EMobileCallReceiveUUI:
            ret = ReceiveUUICancel( aTsyReqHandle );
            break;         
        //Everything is taken care in the method implementation.
        //Just direct the request to the method.
        case EMobileCallDialEmergencyCall:
            ret = DialEmergencyCallCancel( aTsyReqHandle );
            break;
        case EMobileCallDialISV:
            iMmPhone->GetCustomTsy()->SetObjectForISVDialNumberCheck( NULL );
            i3rdPartyEmergencyNumberCheckDone = EFalse;
            ret = DialCancel( aTsyReqHandle );
            break;   
        case EMobileCallDialNoFdnCheck:
            ret = DialCancel( aTsyReqHandle );
            break;
        case EMobileCallAnswerISV:
        	ret = AnswerIncomingCallCancel( aTsyReqHandle );
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
// CMmVoiceCallTsy::CompleteNotifyStatusChange
// Complete notification when status changes. If the new status requires,
// this method will ask call protocol extension to update it's status and 
// capabilities and line owning this call to update it's status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::CompleteNotifyStatusChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    if ( iGhostCall ) 
        {
        HandleGhostCallStatusChange( aResult, aDataPackage );
        }
    else
        {
        TInt callId( -1 );
        TInt callIndex( 0 ); 
        TInt extendedError( 
            CMmCommonStaticUtility::ExtendedErrorCode( aResult) );
        TInt ret( KErrNone );
        TBool timerStarted( EFalse );
        TBool statusChanged ( EFalse );
        TBool mobileStatusChanged ( EFalse );
        CMmVoiceCallTsy* mmCall = NULL; 
        RMobileCall::TMobileCallStatus callStatus( RMobileCall::EStatusIdle );
        RMobilePhone::TMobileService callMode( RMobilePhone::EVoiceService );
        CCallDataPackage* callDataPackage = 
            reinterpret_cast<CCallDataPackage*>(aDataPackage);

        callDataPackage->GetCallIdAndMode( callId, callMode ); 
        callDataPackage->UnPackData( callStatus );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_1, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. aResult:%{TSymbianErrorCodes}", aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_2, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. Call status:%d Call name:%S", callStatus, iCallName);

        TTsyReqHandle holdHandle = 
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodeMobileCallHold );
        TTsyReqHandle resumeHandle = 
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodeMobileCallResume );
        TTsyReqHandle swapHandle = 
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodeMobileCallSwap );
        TBool localSwapRequested = 
            IsServiceLocallyRequested( EMultimodeMobileCallSwap );

        switch( callStatus )
            {
            case RMobileCall::EStatusIdle:
            	{          	
            	RFeatureControl featureControl;
            	TInt err = featureControl.Open();
            	if ( (err == KErrNone) && (featureControl.FeatureSupported(NFeature::KEmergencyCallsEnabledInOfflineMode) == KFeatureSupported) && EEmergencyCallMade == iMmPhone->GetRfStatus())
                    {
                    CMmCallList* callList = iMmPhone->CallList();

                    TInt numberOfEmergencyCalls( 0 );
    
                    for( TInt i=0; i< callList->GetNumberOfObjects();i++)
                        {
                        CMmVoiceCallTsy* call = 
                        reinterpret_cast<CMmVoiceCallTsy*>(
                            iMmPhone->CallList()->GetMmCallByIndex( i ) );
                        
                        if( call->IsEmergencyCall() )
                            {
                            numberOfEmergencyCalls++;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_3, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. numberOfEmergencyCalls:%d", numberOfEmergencyCalls );
                            }
                        }

                    // There might be another emergency call Dial started and
                    // iEmergencyCall flag is updated to false in
                    // CompleteHangUp, thus only if there is one emergency call
                    // flag indicating ETrue, RF's should be set off.
                    if( 1 == numberOfEmergencyCalls )
                        {
                         TCtsySsmCallbackData dummyData;
                    	 reinterpret_cast<CMmCallGsmWcdmaExt*>
                       		( iMmCallExtInterface )->SetRfState(ERfsStateInfoInactive,
                       											reinterpret_cast<CMmCallGsmWcdmaExt*>( iMmCallExtInterface ),
                       											dummyData);
        				//set the rf status in phone
                    	iMmPhone->SetRfStatus( ERfsInActive );
                        }
                    }
            	
                //reset caps. 
                iCallCaps.iFlags &= ~(
                    RCall::KCapsHangUp | RCall::KCapsAnswer );
                iCallCaps.iFlags |= RCall::KCapsDial;

                //stop the call duration timer
                timerStarted = iCallTimer->Stop();
                //Check air time timer only if call was in active state
                //(call timer was started)
                if ( timerStarted )
                    {
                    iMmPhone->AirTimeTimerCheckStop();
                    UpdateLifeTimer();                                        
                    }

                if ( iTelephonyAudioControl && iTelephonyAudioControlSetup )
                    {
                    //Inform call routing control if there were error or cancelling in dial
                    if ( RMobileCall::EStatusDialling == iMobileCallStatus )
                        {
                        iTelephonyAudioControl->CallStateChange(
                        iCallName, RMobileCall::EStatusDisconnecting );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_4, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. Call routing state changed to EStatusDisconnecting: Call name:%S", iCallName );
                        }
                    iTelephonyAudioControl->TeardownTelephonyAudio(
                    iCallName, aResult );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_5, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. Call routing teared down: Call name:%S", iCallName);
                    iTelephonyAudioControlSetup = EFalse;
                    }

                iCallStatus = RCall::EStatusIdle;
                iMobileCallStatus = RMobileCall::EStatusIdle;

                ClearCallStatus();
                statusChanged = ETrue;

                if ( KErrNone != aResult )
                    {
                    if ( KErrGsmReleaseByUser == aResult || 
                        KErrGsmBusyUserRequest == aResult )
                        {
                        //aResult must be KErrNone to indicate the client that
                        //HangUp has successfully completed 
                        CompleteHangUp( KErrNone );
                        CompleteAnswerIncomingCall( KErrGsmBusyUserRequest );
                        }
                    else
                        {
                        //set last exit code
                        iLastExitCode = aResult;
                        CompleteHangUp( aResult );
                        CompleteAnswerIncomingCall( aResult );
                        }
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_6, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - CompleteDial's called");
                    CompleteDial( aResult );
                    CompleteDialNoFdn( aResult );
                    CompleteDialEmergencyCall( aResult );
                    }
                // Try to complete Dial and HangUp; If completes happens from 
                // here, something has gone wrong. Done to prevent TSY from 
                // hanging.
                else
                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_7, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - Try to complete Dial and HangUp");
                    iLastExitCode = KErrGeneral;
                    TInt errorValue = CMmCommonStaticUtility::EpocErrorCode( 
                        KErrNotReady, KErrNotFound );
                    CompleteDial( errorValue );
                    CompleteDialNoFdn( errorValue );
                    CompleteDialEmergencyCall( errorValue );
                    CompleteHangUp( errorValue );
                    CompleteAnswerIncomingCall( errorValue );
                    }
                //save last id. Required by Conference call implementation
                SetPreviousCallId( iCallId );
                //reset call id
                iCallId = -1;
                //update DTMF capabilities.
                iMmPhone->GetDtmfTsy()->CompleteNotifyDTMFCaps();

                //Check if call capability KCapsHangup still exists
                //If it does, remove it and return the KCapsDial capability.
                //If call mode is data, then return also KCapsConnect 
                //capability
                if ( RCall::KCapsHangUp == 
                    ( iCallCaps.iFlags & RCall::KCapsHangUp ) )
                    {
                    // Remove KCapsHangUp if it exists
                    iCallCaps.iFlags &= ~( RCall::KCapsHangUp );
                    // return the KCapsDial capability.
                    iCallCaps.iFlags |= RCall::KCapsDial;

                    CompleteNotifyCapsChange();
                    }

                //Call Transfer handling
                if ( ServiceRequested( EMultimodeCallTransfer ) )
                    {
                    //Complete Transfer 
                    CompleteTransfer( KErrNone );
                    }
            	}
                break; 
                //End of case KCallStatusIdle
            case RMobileCall::EStatusDialling:
#ifdef REQHANDLE_TIMER
                if ( KETelExt3rdPartyV1 != GetExtensionId() )
                    {
                    //non 3rd party client
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_8, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - StopTimeOuts");
                    iTsyReqHandleStore->StopTimeout( EMultimodeCallDial );
                    iTsyReqHandleStore->StopTimeout(
                    EMultimodeMobileCallDialEmergencyCall );
                	iTsyReqHandleStore->StopTimeout( 
                	    EMultimodeCallDialNoFdnCheck );
                    }
                else
                    {
                    //3rd party client
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_9, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - StopTimeOut EMultimodeCallDialISV");
                    iTsyReqHandleStore->StopTimeout( EMultimodeCallDialISV );
                    }
#endif // REQHANDLE_TIMER
                iCallStatus = RCall::EStatusDialling;
                iMobileCallStatus = RMobileCall::EStatusDialling;
                statusChanged = ETrue;
                UpdateCallRoutingControl(  iCallName, iMobileCallStatus );
                
                if ( iDialCancelFlag != CMmCallTsy::EDialCancelNotCalled )
                    {
                    TTsyReqHandle dialCancelHandle = 
                        iTsyReqHandleStore->GetTsyReqHandle( 
                            EMultimodeCallDial );

                    TTsyReqHandle dialEmergCancelHandle = 
                        iTsyReqHandleStore->GetTsyReqHandle( 
                            EMultimodeMobileCallDialEmergencyCall );
                    
                    // 3rd party client
                    TTsyReqHandle dialCancelHandleISV = 
                        iTsyReqHandleStore->GetTsyReqHandle( 
                            EMultimodeCallDialISV );
                    
                    // DialNoFdnCheck                            
                    TTsyReqHandle dialCancelHandleNoFdn = 
                        iTsyReqHandleStore->GetTsyReqHandle( 
                        EMultimodeCallDialNoFdnCheck );

                    if ( 0 < dialCancelHandle ) 
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_10, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - HangUp - EMultimodeCallDial");
                        HangUp( dialCancelHandle );
                        }
                    else if ( 0 < dialCancelHandleISV )
                        {
                        //3rd party client
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_11, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - HangUp - EMultimodeCallDialISV");
                        HangUp( dialCancelHandleISV );
                        }
                    else if ( 0 < dialEmergCancelHandle )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_12, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - HangUp - EMultimodeMobileCallDialEmergencyCall");
                        HangUp( dialEmergCancelHandle );
                        }
					else if ( 0 < dialCancelHandleNoFdn )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_13, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - HangUp - EMultimodeCallDialNoFdn");
                        HangUp( dialCancelHandleNoFdn );
                        }  
                    else
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_14, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - iDialCancelFlag = CMmCallTsy::EDialCancelNotCalled");
                        iDialCancelFlag = CMmCallTsy::EDialCancelNotCalled;
                        }
                    }
                break;
            case RMobileCall::EStatusConnecting:
                iCallStatus = RCall::EStatusConnecting;
                iMobileCallStatus = RMobileCall::EStatusConnecting;
                statusChanged = ETrue;
                UpdateCallRoutingControl(  iCallName, iMobileCallStatus );
                break;
            case RMobileCall::EStatusRinging:
                iCallStatus = RCall::EStatusRinging;
                iMobileCallStatus = RMobileCall::EStatusRinging;
                statusChanged = ETrue;
                break;
            case RMobileCall::EStatusAnswering:
                iCallStatus = RCall::EStatusAnswering;
                iMobileCallStatus = RMobileCall::EStatusAnswering;
                statusChanged = ETrue;
                UpdateCallRoutingControl(  iCallName, iMobileCallStatus );
                break;
            case RMobileCall::EStatusConnected:
                if ( KErrMMEtelCallForbidden == aResult )
                    {
                    CMmCallList* callList = iMmPhone->CallList();
                    for( TInt i=0; i< callList->GetNumberOfObjects();i++)
                        {
                        CMmVoiceCallTsy* call = 
                        reinterpret_cast<CMmVoiceCallTsy*>(
                            iMmPhone->CallList()->GetMmCallByIndex( i ) );
                        // if there is call with status Answering, complete
                        // it with CALL_CAUSE_NOT_ALLOWED    
                        if( RCall::EStatusAnswering == call->Status() )
                            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_15, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange Answering not allowed!");
                            TInt errorValue = 
                                CMmCommonStaticUtility::EpocErrorCode( 
                                   KErrAccessDenied, KErrMMEtelCallForbidden );
                            call->CompleteAnswerIncomingCall( errorValue );
                            }
                        }
                    }
                
                //check previous status. If status is answering
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_16, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. - EStatusConnected");
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
                    //complete answer incoming call
                    CompleteAnswerIncomingCall( aResult );
                    }
                //if it was connecting and dial cancel has not been activated 
                else if ( ( iCallStatus == RCall::EStatusConnecting ||
                    iCallStatus == RCall::EStatusDialling ) &&
                    CMmCallTsy::EDialCancelNotCalled == iDialCancelFlag )
                    {          
                    //Start call duration monitoring
                    iCallTimer->Start(); 
                    //start air time duration monitoring
                    iMmPhone->AirTimeTimerCheckStart();

                    //if emergency call has been made, complete it
                    if ( iTsyReqHandleStore->GetTsyReqHandle(
                        EMultimodeMobileCallDialEmergencyCall ) )
                        {
                        //CompleteDialEmergencyCall
                        CompleteDialEmergencyCall( aResult );
                        }
                    //if normal call has been made (by 3rd party client or not
                    //complete it
                    if ( ( iTsyReqHandleStore->GetTsyReqHandle( 
                            EMultimodeCallDial ) ) 
                        || ( iTsyReqHandleStore->GetTsyReqHandle( 
                            EMultimodeCallDialISV ) ) )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_17, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange.EStatusConnected - Before CompleteDial");
                        CompleteDial( aResult ); 
                        }
                     if( iTsyReqHandleStore->GetTsyReqHandle( 
                            EMultimodeCallDialNoFdnCheck ) )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_18, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange.EStatusConnected - Before CompleteDialNoFdn");
                        CompleteDialNoFdn( aResult );                                                 

                        }
                    }
                //if status was Hold (-> resume or swap has been asked)
                else if ( iMobileCallStatus == RMobileCall::EStatusHold )
                    {
                    //local resume has been requested
                    CompleteNotifyCallEvent( RMobileCall::ELocalResume );

                    //complete resume but not if swap is going on
                    if ( swapHandle == 0 && resumeHandle != 0 )
                        {
                        //complete resume
                        CompleteResume( aResult );
                        }
                    else if ( localSwapRequested )
                        {
                        (reinterpret_cast<CMmVoiceLineTsy*>( iMmLine ))->
                            IsSwapReady( callStatus );
                        }
                    }
                //Don't update status if it has not changed.
                if ( RCall::EStatusConnected != iCallStatus ||
                     RMobileCall::EStatusConnected != iMobileCallStatus )
                    {
                    if(RMobileCall::EStatusConnecting != iMobileCallStatus)
                        {
                        // RMobileCall::EStatusConnected without RMobileCall::EStatusConnecting 
                        if ( KErrNone == iMmCallExtInterface-> 
                            CompleteNotifyStatusChange( RMobileCall::EStatusConnecting ) )
                            { 
                            CompleteNotifyMobileCallCapsChange( KErrNone );
                            }
                        
                        }
                    UpdateCallRoutingControl( iCallName, RMobileCall::EStatusConnected );
                    iCallStatus = RCall::EStatusConnected;
                    iMobileCallStatus = RMobileCall::EStatusConnected;
                    statusChanged = ETrue;
                    }

                //update DTMF capabilities.
                iMmPhone->GetDtmfTsy()->CompleteNotifyDTMFCaps();
                break;
            case RMobileCall::EStatusDisconnecting:
                //Core status
                iCallStatus = RCall::EStatusHangingUp; 
                statusChanged = ETrue;
                // Mobile status
                iMobileCallStatus = RMobileCall::EStatusDisconnecting;
                mobileStatusChanged = ETrue;
                //update DTMF capabilities.
                iMmPhone->GetDtmfTsy()->CompleteNotifyDTMFCaps();

                if ( iCallDirection == RMobileCall::EMobileTerminated )
                    {
                    if(  KMultimodeCallTypeIDNoFdnCheck == GetDialTypeId() )
                   	    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_19, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. RMobileCall::EStatusDisconnecting - Before CompleteDialNoFdn");
                   	    //CompleteDialNoFdn in case remote user is busy
                        CompleteDialNoFdn( aResult );
                   	    }
                    else
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_20, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. RMobileCall::EStatusDisconnecting - Before CompleteDial");
                        //CompleteDial in case remote user is busy
                        CompleteDial( aResult );
                        }                   	    

                    }

                //Set last exit code
                if ( KErrGsmReleaseByUser == aResult ||
                    KErrGsmBusyUserRequest == aResult ||
                    KErrNone == aResult )
                    {
                    iLastExitCode = KErrNone;
                    }
                else
                    {
                    //set last exit code
                    iLastExitCode = aResult;
                    }
                    
                UpdateCallRoutingControl( iCallName, iMobileCallStatus );
                    
                break;
            case RMobileCall::EStatusDisconnectingWithInband:
                //Core status
                iCallStatus = RCall::EStatusHangingUp; 
                statusChanged = ETrue;
                // Mobile status
                iMobileCallStatus =
                    RMobileCall::EStatusDisconnectingWithInband;
                mobileStatusChanged = ETrue;
                
                if(  KMultimodeCallTypeIDNoFdnCheck == GetDialTypeId() )
                   	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_21, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. RMobileCall::EStatusDisconnectingWithInband - Before CompleteDialNoFdn");
                   	//CompleteDialNoFdn in case remote user is busy
                    CompleteDialNoFdn( aResult );
                   	}
                else
                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETENOTIFYSTATUSCHANGE_22, "TSY: CMmVoiceCallTsy::CompleteNotifyStatusChange. RMobileCall::EStatusDisconnectingWithInband - Before  CompleteDial");
                    //CompleteDial in case remote user is busy
                    CompleteDial( aResult );        
                    }

                //update DTMF capabilities.
                iMmPhone->GetDtmfTsy()->CompleteNotifyDTMFCaps();

                //Set last exit code
                if ( KErrGsmReleaseByUser == aResult ||
                    KErrGsmBusyUserRequest == aResult ||
                    KErrGsmCCNormalUnspecified == extendedError ||
                    KErrNone == aResult )
                    {
                    iLastExitCode = KErrNone;
                    }
                else
                    {
                    //set last exit code
                    iLastExitCode = aResult;
                    }
                    
                UpdateCallRoutingControl( iCallName, iMobileCallStatus );

                break;
            case RMobileCall::EStatusHold:
                if ( iMobileCallStatus != RMobileCall::EStatusHold )
                    {
                    UpdateCallRoutingControl( iCallName, RMobileCall::EStatusHold );
                    
                    //remote party requested this
                    CompleteNotifyCallEvent( RMobileCall::ELocalHold );
                    }

                //complete hold but not if swap is going on
                if ( swapHandle == 0 && holdHandle != 0 )
                    {
                    CompleteHold ( aResult );
                    }
                else if ( localSwapRequested )
                    {
                    (reinterpret_cast<CMmVoiceLineTsy*>( iMmLine ) )->
                        IsSwapReady( callStatus );
                    }

                // Mobile API state
                iMobileCallStatus = RMobileCall::EStatusHold;
                mobileStatusChanged = ETrue;

                //update DTMF capabilities.
                iMmPhone->GetDtmfTsy()->CompleteNotifyDTMFCaps();
                break;
            case RMobileCall::EStatusUnknown:
            case RMobileCall::EStatusReconnectPending:
            case RMobileCall::EStatusWaitingAlternatingCallSwitch:
            case RMobileCall::EStatusTransferring:
            case RMobileCall::EStatusTransferAlerting:
            default:
                //nothing to do
                break;
            }

        //reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
            EMultimodeCallNotifyStatusChange );

        if ( ( EMultimodeCallReqHandleUnknown != reqHandle ) && statusChanged )
            {
            *iRetStatus = iCallStatus;
            //reset req handle. 
            iTsyReqHandleStore->ResetTsyReqHandle(
                EMultimodeCallNotifyStatusChange );

            ReqCompleted( reqHandle, ret );
            }

        //reset req handle. Returns the deleted req handle
        reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
            EMultimodeCallNotifyMobileCallStatusChange );

        if ( ( EMultimodeCallReqHandleUnknown != reqHandle ) &&
            (statusChanged || mobileStatusChanged) )
            {
            *iRetMobileCallStatus = iMobileCallStatus;
            //reset req handle. 
            iTsyReqHandleStore->ResetTsyReqHandle(
                EMultimodeCallNotifyMobileCallStatusChange );
            ReqCompleted( reqHandle, ret );
            }

        //Update also line's status
        if ( statusChanged || mobileStatusChanged )
            {
            iMmLine->CompleteNotifyStatusChange();
            }

        //Inform extension dll about the current status. Enables dynamic 
        //capability updates. Must be done before Notifying caps change 
        if ( KErrNone == iMmCallExtInterface-> 
            CompleteNotifyStatusChange( iMobileCallStatus ) )
            { 
            CompleteNotifyMobileCallCapsChange( KErrNone );
            }

        mmCall = reinterpret_cast<CMmVoiceCallTsy*>(
            iMmPhone->CallList()->GetMmCallByIndex( callIndex ) );

        while ( mmCall ) 
            { 
            if ( RMobileCall::EStatusIdle != mmCall->MobileCallStatus() && 
                this != mmCall )
                { 
                if ( KErrNone == mmCall->ActiveCallExtension()->
                    CompleteNotifyStatusChange( mmCall->MobileCallStatus() ) )
                    { 
                    mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );
                    }
                } 
            callIndex++;
            mmCall = reinterpret_cast<CMmVoiceCallTsy*>(
                iMmPhone->CallList()->GetMmCallByIndex( callIndex ) );
            }

        // Inform Conference Call Tsy about status indication of
        // a single call, might cause change of conference status
        CMmConferenceCallTsy* confCall = iMmPhone->ConferenceCall();
        if ( confCall )
            {
            if ( RMobileCall::EStatusIdle == callStatus )
                {
                // if the call has gone to idle state, iPreviousCallId must
                // be used instead of iCallId, because iCallId has already 
                // been reset
                confCall->CallStatusChanged( callStatus, iPreviousCallId );
                }
            else
                {
                confCall->CallStatusChanged( callStatus, iCallId );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::Dial
// This CORE API method dials to the given number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::Dial(
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* aCallParams,
    TDesC* aTelNumber )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_1, "TSY: CMmVoiceCallTsy::Dial. Req handle: %u, Call name: %S", aTsyReqHandle, iCallName);

    TBool isDialISV = iIsDialISV;
    iIsDialISV = EFalse;
    
	if ( aCallParams->Length()!= 0 )
		{
		if ( sizeof( RMobileCall::TCallParams ) > aCallParams->Length() )
			{
	      	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_2, "TSY: CMmVoiceCallTsy::Dial bad size argument");
	      	// Complete the request with appropiate error        
	      	return KErrArgument;
	      	}
		}
	    CMmCallList* callList = iMmPhone->CallList();

    TBool dialFlag( EFalse );
    
    for ( TInt i = 0; i < callList->GetNumberOfObjects(); i++ )
        {
        CMmCallTsy* call = callList->GetMmCallByIndex( i );
        
        if ( call->GetDialFlag() )
            {
            dialFlag = ETrue;
            i = callList->GetNumberOfObjects();
            }
        }
    
    if ( !dialFlag )
        {
        TInt ret( KErrNone );
        SetDialFlag( ETrue );
        TInt trapError( KErrNone );
        //Default value
        TInt extensionId ( -1 );

        TTsyReqHandle dialHandle = 
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallDial );
            
        TTsyReqHandle dialHandleISV = 
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallDialISV );

        
        RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 = NULL;

        if ( 0 < aCallParams->Length() )
            {
            paramsPckgV1 = 
                reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>(
                    const_cast<TDesC8*>( aCallParams ) );
            }
        else
            {
            iCallParams.iSpeakerControl = 
                RCall::EMonitorSpeakerControlOnUntilCarrier;
            iCallParams.iSpeakerVolume = RCall::EMonitorSpeakerVolumeLow;
            iCallParams.iInterval = 0;
            iCallParams.iWaitForDialTone = RCall::EDialToneWait;
            iCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
            iCallParams.iCug.iExplicitInvoke = EFalse;
            iCallParams.iCug.iCugIndex = 0xFFFF;
            iCallParams.iCug.iSuppressPrefCug = EFalse;
            iCallParams.iCug.iSuppressOA = EFalse;
            iCallParams.iAutoRedial = EFalse;
            
                
            paramsPckgV1 = &iCallParamsPckg;
            }
        
        RMobileCall::TMobileCallParamsV1& paramsV1 = ( *paramsPckgV1 )();
        
        //Retrieve the extensionId of the call        
        extensionId = paramsV1.ExtensionId();
     
        //Set the private member variable iExtensionId
        SetExtensionId( extensionId );

        //Check if there is already an active call
        CMmCallTsy* activeCall = iMmPhone->CallList()->
            GetMmCallByStatus( RMobileCall::EStatusConnected );
                  
        //reset exit code
        iLastExitCode = KErrNone;
        
        if ( KETelExt3rdPartyV1 == extensionId && !isDialISV )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_3, "TSY: CMmVoiceCallTsy::Dial - KErrArgument");
            //Complete the request with appropiate error
            ReqCompleted ( aTsyReqHandle, KErrArgument );
            SetDialFlag( EFalse ); 
            }

        else if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )  
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_4, "TSY: Offline mode ON, Dial request is not allowed" );
            ret = CMmCommonStaticUtility::EpocErrorCode(
                KErrGeneral, KErrGsmOfflineOpNotAllowed );

            //Complete the request with appropiate error
            ReqCompleted ( aTsyReqHandle, ret ); 
            SetDialFlag( EFalse );
            }

        //check that status is Idle
        else if ( RMobileCall::EStatusIdle != iMobileCallStatus )
            {
            //The request cannot be forwarded since this call object
            //is still in use.
            //Complete request with status value informing the client about 
            //the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_5, "TSY: CMmVoiceCallTsy::Dial - KErrNotReady");
            ReqCompleted( aTsyReqHandle, KErrNotReady );
            SetDialFlag( EFalse );
            }
        
        else if ( ( 0 < dialHandle ) || ( ( KETelExt3rdPartyV1 == extensionId )
            && ( 0 < dialHandleISV ) && !i3rdPartyEmergencyNumberCheckDone  ) )
            {
            //The request is already processing because of previous request
            //Complete request with status value informing the client about 
            //the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_6, "TSY: CMmVoiceCallTsy::Dial - KErrServerBusy");
            ReqCompleted( aTsyReqHandle, KErrServerBusy );
            SetDialFlag( EFalse );
            }
        
        else if ( ( activeCall ) && ( KETelExt3rdPartyV1 == extensionId )
            && ( KETelExt3rdPartyV1 != activeCall->GetExtensionId() ) )
            {
            //a 3rd party client call cannot put on Hold a "normal" call
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_7, "TSY: CMmVoiceCallTsy::Dial - KErrServerBusy");
            ReqCompleted( aTsyReqHandle, KErrServerBusy );
            SetDialFlag( EFalse );
            }
            //3rd party client is not allowed to call to a emergency call number
            //Number check for 3rd clients must be done always     
        
        else if ( extensionId == KETelExt3rdPartyV1 && 
                                        !i3rdPartyEmergencyNumberCheckDone )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_8, "TSY: CMmVoiceCallTsy::Dial - For 3rd party client number check must be done every time ");
            
            TInt ret( KErrGeneral );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_9, "TSY: CMmVoiceCallTsy::Dial - 3rd party client number check iNumberCheckMode.iCheckMode = RMmCustomAPI::EEmerNumberCheckNormal ");
            iNumberCheckMode.iCheckMode = RMmCustomAPI::EEmerNumberCheckNormal;
            iNumberCheckMode.iNumber.Copy( *aTelNumber );
                    
            //Save parameter for Dial req after number check
            iISVCallParams = aCallParams;
            
            CMmDataPackage package;
            package.PackData( &iNumberCheckMode );
            
            CMmCustomTsy* mmCustom = iMmPhone->GetCustomTsy();
            
            if ( NULL == mmCustom )
            	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_10, "TSY: CMmVoiceCallTsy::Dial - CustomTSY is not yet created!");
            	ReqCompleted( aTsyReqHandle, KErrNotReady );
            	SetDialFlag( EFalse );
            	}
            
            else        
    	        {
    	        //Save call object pointer to Customtsy for completion
    	        mmCustom->SetObjectForISVDialNumberCheck( this );
    	            
    	        TRAP( trapError, ret = mmCustom->CheckEmergencyNumberL( 0, 
    	                &iNumberCheckMode, &i3rdPartyEmergencyNumberCheckDone );
    	            );
    	        
    	        if ( KErrNone == ret )
    	            {
    	            //Set dial flag to false because of ISV dial waits
    	            //number check to be completed 
    	            SetDialFlag( EFalse );
    	            
#ifdef REQHANDLE_TIMER
                    //Set timer for the request if 3rd party client
                    SetTypeOfResponse( EMultimodeCallDialISV, aTsyReqHandle );                
#else
                    //Set timer if 3rd party client
                    iTsyReqHandleStore->SetTsyReqHandle(
                        EMultimodeCallDialISV, aTsyReqHandle );
#endif
    	            }
    	        
    	        else
    	            {
    	            ReqCompleted( aTsyReqHandle, KErrServerBusy );    
    	            }	
    	        }
            }
        
        else
            {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_11, "TSY: CMmVoiceCallTsy::Dial - UUS services: %u", iUUSRequest.iServiceReq );

            // UUS1 will be embedded within the call set-up message
            if ( iUUSRequest.iServiceReq & RMobileCall::KUUS1Implicit )
                {
                // Create package
                CCallDataPackage package;
                // Set call id and call mode  
                package.SetCallIdAndMode( CallId(), CallMode() );
                package.PackData( &iUUSRequest );
                 // Send UUS request to the Domestic OS layer.
            	TRAPD(err, ret = iMessageManager->HandleRequestL( 
                    EMobileCallActivateUUS, &package ));

                // clear/empty the UUS request buffer
                iUUSRequest.iServiceReq = 0; 
                iUUSRequest.iUUI.Zero(); 
                
				if( err != KErrNone)
					{
					ret = err;
					}
                // in case the LTSY does not support required UUS,
                // complete the dial with error value 
                if ( KErrNone != ret )
                    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIAL_12, "TSY: CMmVoiceCallTsy::Dial - UUS ERROR:%{TSymbianErrorCodes}", ret );
                    // Complete the request with appropiate error
                    ReqCompleted ( aTsyReqHandle, KErrGsmCCResourceNotAvailable );
                    ClearCallStatus(); 
                    return KErrNone; 
                    }
                }       
                        
            //save call params
            iCallParams.iSpeakerControl = paramsV1.iSpeakerControl;
            iCallParams.iSpeakerVolume = paramsV1.iSpeakerVolume;
            iCallParams.iInterval = paramsV1.iInterval;
            iCallParams.iWaitForDialTone = paramsV1.iWaitForDialTone;

            //set call direction
            iCallDirection = RMobileCall::EMobileOriginated;
            // Dial
            TRAP( trapError,
                ret = iMmCallExtInterface->DialL(
                    iCallMode, paramsPckgV1, aTelNumber, extensionId );
                );
            

        if ( KErrNone == ret && KErrNone == trapError )
            {
            UpdateCallRoutingControl( iCallName, iMobileCallStatus );

                //update core status
                iCallCaps.iFlags &= ~( RCall::KCapsDial );
                iCallCaps.iFlags |= RCall::KCapsHangUp;
                //complete core caps change
                CompleteNotifyCapsChange();

#ifdef REQHANDLE_TIMER
                if ( KETelExt3rdPartyV1 != extensionId )
                    {
                    //set timer for the request if 3rd party client
                    SetTypeOfResponse( EMultimodeCallDial, aTsyReqHandle );    
                    }
                
#else
                if ( KETelExt3rdPartyV1 != extensionId )
                    {
                    //set timer if 3rd party client
                    iTsyReqHandleStore->SetTsyReqHandle(
                        EMultimodeCallDial, aTsyReqHandle );        
                    }
#endif
                }
            else
                {
                if ( KErrNone != trapError )
                    {
                    //Error Handling.DialL() leaves
                    ret = trapError;
                    }
                ReqCompleted( aTsyReqHandle, ret );
                ClearCallStatus();
                }
            }
        
        return KErrNone;
        }

    else // if dialFlag set
        {
        return KErrServerBusy;
        }

    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::AnswerIncomingCall
// This CORE API method is used for answering to an incoming call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::AnswerIncomingCall(
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* aCallParams )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_ANSWERINCOMINGCALL_1, "TSY: CMmVoiceCallTsy::AnswerIncomingCall. \n\t\t\t Handle:%d", aTsyReqHandle);

    TInt trapError( KErrNone );
    
    //ExtensionId unknown.Set to -1
    TInt extensionId ( -1 );
    
    TTsyReqHandle answerCallHandle( iTsyReqHandleStore->
            GetTsyReqHandle( EMultimodeCallAnswer ) );
    
    //3rd party client
    TTsyReqHandle answerCallHandleISV( iTsyReqHandleStore->
            GetTsyReqHandle( EMultimodeCallAnswerISV ) );
    
    if (NULL != aCallParams )
        {
        RMobileCall::TMobileCallParamsV1Pckg* mmParamsPckgV1 = 
            reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>(
                const_cast<TDesC8*> ( aCallParams  ) );
        RMobileCall::TMobileCallParamsV1& callParams = 
            (*mmParamsPckgV1)();

        //Retrieve information about the extensionId
        extensionId = callParams.ExtensionId();
        
        //Set the private member variable iExtensionId
        SetExtensionId( extensionId );
        }
    /*no else*/
    
    // check if there is an active and a held call already
    CMmCallTsy* activeCall = iMmPhone->CallList()->
        GetMmCallByStatus( RMobileCall::EStatusConnected );
    CMmCallTsy* heldCall = iMmPhone->CallList()->
        GetMmCallByStatus( RMobileCall::EStatusHold );

    if ( 0 < answerCallHandle )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    //if 3rd party client 
    else if ( 0 < answerCallHandleISV )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else if ( activeCall && heldCall )
        {
        // if there is an active and a held call, we cannot answer 
        // yet another one
        ReqCompleted( aTsyReqHandle, KErrGsmNotAllowed );
        }
   else if( activeCall && 
               ( !( KETelExt3rdPartyV1 == activeCall->GetExtensionId() ) &&
               ( KETelExt3rdPartyV1 == extensionId ) ) )
        {
        //if the active call is handled by a non 3rd party client and that this
        //AnswerIncomingCall is handled by a 3rd party client
        //=> a 3rd party call cannot put on hold a non 3rd party one.
        ReqCompleted( aTsyReqHandle, KErrGsmNotAllowed );
        }
    else
        {
        TInt ret( KErrNone );

        //reset exit code
        iLastExitCode = KErrNone;

        // Check that call object is same as object created for last 
        // incoming call
        if ( RMobileCall::EMobileTerminated == iCallDirection &&
            RMobileCall::EStatusRinging == iMobileCallStatus )
            {       
            //Answer incoming call
            TRAP( trapError,
                ret = iMmCallExtInterface->
                          AnswerIncomingCallL( iCallId, extensionId );
                );
            
            if ( KErrNone != trapError )
                {
                //Object cannot be created.
                ret = trapError;
                }

            if ( KErrNone == ret )
                {
                UpdateCallRoutingControl( iCallName, iMobileCallStatus );
                
                iCallCaps.iFlags |= RCall::KCapsHangUp;
                iCallCaps.iFlags &= ~( 
                                      RCall::KCapsAnswer | RCall::KCapsDial );
                CompleteNotifyCapsChange();
                }
            }
        else
            {
            ret = KErrNotSupported;
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
            if ( KETelExt3rdPartyV1 == extensionId )
                {
                //3rd party client
                SetTypeOfResponse( EMultimodeCallAnswerISV, aTsyReqHandle );
                }
            else
                {
                SetTypeOfResponse( EMultimodeCallAnswer, aTsyReqHandle );            
                }
#else
            //timer set
            if ( KETelExt3rdPartyV1 == extensionId )
                {
                //3rd party client
                iTsyReqHandleStore->SetTsyReqHandle(
                    EMultimodeCallAnswerISV, aTsyReqHandle );    
                }
            else
                {
                iTsyReqHandleStore->SetTsyReqHandle(
                    EMultimodeCallAnswer, aTsyReqHandle );
                }
#endif
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::HangUp
// This CORE API method disconnects the call. Used with normal voice calls, 
// emergency calls as well as data calls. DialCancel also uses this method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::HangUp(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_HANGUP_1, "TSY: CMmVoiceCallTsy::HangUp. Req handle: %u, Call name: %S", aTsyReqHandle, iCallName);

    TInt ret( KErrNone );
    TInt trapError( KErrNone );
    TInt hangUpCause( KErrNone );

    TTsyReqHandle hangUpHandle = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodeCallHangUp );
    TTsyReqHandle dialHandle = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodeCallDial );
    TTsyReqHandle dialEmergencyHandle = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodeMobileCallDialEmergencyCall );
        
    // 3rd party client
    TTsyReqHandle dialHandleISV = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodeCallDialISV );
        
    // Dial NoFdnCheck
    TTsyReqHandle dialHandleNoFdnCheck = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodeCallDialNoFdnCheck );            
                           
    if ( 0 < hangUpHandle || iHangUpFlag )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else if ( RCall::EStatusIdle == iCallStatus &&
        ( EMultimodeCallReqHandleUnknown == dialHandle ||
        EMultimodeCallReqHandleUnknown == dialEmergencyHandle
        || EMultimodeCallReqHandleUnknown == dialHandleISV 
        || EMultimodeCallReqHandleUnknown == dialHandleNoFdnCheck ) )
        {
        //Call object is already in idle state.
        //Complete HangUp request with error.
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

        // Avoid held call from automatically becoming active call
        TBool autoStChangeDisable = ETrue;

        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode( iCallId, RMobilePhone::EVoiceService );
        //Pack call parameters and mobile call info
        package.PackData( &hangUpCause, &autoStChangeDisable );
        //Send request to the Domestic OS layer.
        TRAP( trapError,
            ret = iMessageManager->HandleRequestL( 
                EEtelCallHangUp, &package );
            );

        //send failure
        if ( KErrNone != ret || KErrNone != trapError )
            {
            iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallHangUp );

            //in case of memory error
            if ( KErrNone != trapError )
                {
                // Object cannot be created.
                ret = trapError;
                }

            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            //update core status - hangup not possible now.
            //remove also dataport caps
            iCallCaps.iFlags &= ~(
                RCall::KCapsHangUp |
                RCall::KCapsLoanDataPort |
                RCall::KCapsRecoverDataPort );
            //complete core caps change
            CompleteNotifyCapsChange();
            // set hangup flag to ongoing
            iHangUpFlag = ETrue; 
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::CompleteHangUp
// Completes call hangup (both normal and emergency calls). If the cause value 
// is different than the sent one, the call release requset has failed. In 
// this case the causeValue describes the fail cause. Otherwise the call  
// releasing has succeeded.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::CompleteHangUp(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETEHANGUP_1, "TSY: CMmVoiceCallTsy::CompleteHangUp.\n  \t\t\t Result:%{TSymbianErrorCodes}", aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETEHANGUP_2, "TSY: CMmVoiceCallTsy::CompleteHangUp. Call Id:%d Call name:%S", iCallId, iCallName);

    TInt ret( KErrNone );

    // Complete for HangUp request
    // (not to DialCancel or AnswerIncomingCallCancel )
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

                //If client refuses to answer, remove the answering capability
                iCallCaps.iFlags &= ~( RCall::KCapsAnswer );
                }
            ReqCompleted( reqHandle, ret );

            //complete caps change notification
            CompleteNotifyCapsChange();

            iEmergencyCall = EFalse;
            }
        }
    // Complete for DialCancel or AnswerIncomingCallCancel, success
    else if ( KErrNone == aResult ) 
        { 
        // Find out if this is cancelling of Dial or DialEmergencyCall
        TTsyReqHandle cancelHandle = iTsyReqHandleStore->
            ResetTsyReqHandle( EMultimodeCallDial );
        TTsyReqHandle dialEmergCancelHandle = iTsyReqHandleStore->
            ResetTsyReqHandle( EMultimodeMobileCallDialEmergencyCall );
        
        if ( ( NULL == cancelHandle ) && ( NULL == dialEmergCancelHandle ) )
            {
            // Find out if this is cancelling of AnswerIncomingCall
               cancelHandle = iTsyReqHandleStore->
                   ResetTsyReqHandle( EMultimodeCallAnswer );
            
            if ( NULL == cancelHandle )
                {
                // Find out if this is cancelling of DialISV
                cancelHandle = iTsyReqHandleStore->
                    ResetTsyReqHandle( EMultimodeCallDialISV );
            
                if ( NULL == cancelHandle )
                    {
                    // Find out if this is cancelling of AnswerIncomingCallISV
                    cancelHandle = iTsyReqHandleStore->
                        ResetTsyReqHandle( EMultimodeCallAnswerISV );    
                    
                    // Find out if this is cancelling DialNoFdnCheck
                    if ( NULL == cancelHandle )
                        {
                        cancelHandle = iTsyReqHandleStore->
                            ResetTsyReqHandle( EMultimodeCallDialNoFdnCheck );
                        }
                    }    
                }
            }
        if ( 0 < cancelHandle ) 
            { 
            // Complete with error value KErrCancel
            ReqCompleted( cancelHandle, KErrCancel );

            // Cancel has succeeded, update core status -
            // dial possible again
            iCallCaps.iFlags |= RCall::KCapsDial;
            // If client refuses to answer, remove the answering capability
            iCallCaps.iFlags &= ~( RCall::KCapsAnswer );
            // Complete caps change notification
            CompleteNotifyCapsChange();
            }
        // Cancel to DialEmergencyCall
        else if ( 0 < dialEmergCancelHandle ) 
            { 
            // Complete DialEmergencyCall with error value KErrCancel
            ReqCompleted( dialEmergCancelHandle, KErrCancel ); 

            // Update last exit code
            iLastExitCode = KErrCancel;

            // Dial cancel has succeeded,
            // update core status - dial possible again
            iCallCaps.iFlags |= RCall::KCapsDial;
            // Complete caps change notification
            CompleteNotifyCapsChange();

            iEmergencyCall = EFalse;
            }
        } 
    // Cancel failed and the request succeeded (connected)
    else if ( RCall::EStatusConnected == iCallStatus ) 
        { 
          // Find out if this is cancelling of Dial
        TTsyReqHandle cancelHandle = iTsyReqHandleStore->
            ResetTsyReqHandle( EMultimodeCallDial );
        
        // Find out if this is cancelling of DialEmergencyCall
        TTsyReqHandle dialEmergCancelHandle = iTsyReqHandleStore->
            ResetTsyReqHandle( EMultimodeMobileCallDialEmergencyCall );
        
        if ( ( NULL == cancelHandle ) && ( NULL == dialEmergCancelHandle ) )
            {
            // Find out if this is cancelling of AnswerIncomingCall
               cancelHandle = iTsyReqHandleStore->
                   ResetTsyReqHandle( EMultimodeCallAnswer );
            
            if ( NULL == cancelHandle )
                {
                // Find out if this is cancelling of DialISV
                cancelHandle = iTsyReqHandleStore->
                    ResetTsyReqHandle( EMultimodeCallDialISV );
            
                if ( NULL == cancelHandle )
                    {
                    // Find out if this is cancelling of AnswerIncomingCallISV
                    cancelHandle = iTsyReqHandleStore->
                        ResetTsyReqHandle( EMultimodeCallAnswerISV );    
                    
                    if (NULL == cancelHandle )
                        {
                        // Find out if this is cancelling DialNoFdnCheck
                        cancelHandle = iTsyReqHandleStore->
                            ResetTsyReqHandle( EMultimodeCallDialNoFdnCheck );  
                        }
                    }    
                }
            }
        // Cancel to Dial(ISV) or AnswerIncomingCall(ISV)
        if ( 0 < cancelHandle ) 
            { 
            // Complete Dial or AnswerIncomingCall with success (KErrNone)
            ReqCompleted( cancelHandle, KErrNone ); 

            // Succeeded, update core status - hangup possible again
            iCallCaps.iFlags |= RCall::KCapsHangUp;
            // Complete caps change notification
            CompleteNotifyCapsChange();
            } 
        // Cancel to DialEmergencyCall
        else if ( 0 < dialEmergCancelHandle ) 
            { 
            // Complete DialEmergencyCall with success (KErrNone)
            ReqCompleted( dialEmergCancelHandle, KErrNone ); 

            // Dial has succeeded, update core status - hangup possible again
            iCallCaps.iFlags |= RCall::KCapsHangUp;
            // Complete caps change notification
            CompleteNotifyCapsChange();
            } 
        } 

    // Reset cancel flags
    iDialCancelFlag = CMmCallTsy::EDialCancelNotCalled;
    iAnswerCancelFlag = EFalse;
    // reset hangup flag
    iHangUpFlag = EFalse;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::RegisterNotification
// RegisterNotification is called when the server recognises that this 
// notification is being posted for the first time on this sub-session object. 
// It enables the TSY to "turn on" any regular notification messages that it 
// may receive from DOS. Currently does not really do anything but returns 
// KErrNone to ETel server in case of known notification request type. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::RegisterNotification(
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
        case EMobileCallReceiveUUI:
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
// CMmVoiceCallTsy::DeregisterNotification
// DeregisterNotification is called when the server recognises that this 
// notification will not be posted again because the last client to have a 
// handle on this sub-session object has just closed the handle. It enables
// the TSY to "turn off" any regular notification messages that it may receive
// from DOS. Currently does not really do anything but returns KErrNone to
// ETel server in case of known notification request type.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::DeregisterNotification(
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
        case EMobileCallReceiveUUI:
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
// CMmVoiceCallTsy::HoldL
// This method can be used to hold an active call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::HoldL(
    const TTsyReqHandle aTsyReqHandle )
    {
    //Call hold from extension
    TInt ret = iMmCallExtInterface->HoldL( iCallId );

    //request failed
    if ( ret != KErrNone )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else //success in sending to Phonet
        {
        // Store the request handle
        iReqHandleType = EMultimodeMobileCallHold;
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::ResumeL
// Resumes a held call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::ResumeL(
    const TTsyReqHandle aTsyReqHandle )
    {
    //Call resume from extension
    TInt ret = iMmCallExtInterface->ResumeL( iCallId );

    //if failed
    if ( ret != KErrNone )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else //success in sending to Phonet
        {
        // Store the request handle
        iReqHandleType = EMultimodeMobileCallResume;
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::SwapL
// Swaps between active and held call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::SwapL(
    const TTsyReqHandle aTsyReqHandle )
    {
    //direct request to extension
    TInt ret = iMmCallExtInterface->SwapL( iCallId );

    //request failed
    if ( ret != KErrNone )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else //success in sending to Phonet
        {
        // Store the request handle
        iReqHandleType = EMultimodeMobileCallSwap;
        //inform Line that swap handling has started
        (reinterpret_cast<CMmVoiceLineTsy*>( iMmLine ))->
            SetSwapStatus( CMmVoiceLineTsy::EWaitingHoldAndResumeOk );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::DialEmergencyCallL
// Dials an emergency call. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::DialEmergencyCall(
    const TTsyReqHandle aTsyReqHandle, 
    const TDataPackage& aNumber )
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIALEMERGENCYCALL_1, "TSY: CMmVoiceCallTsy::DialEmergencyCall number %S", *(aNumber.Des1u()));
        
    if(aNumber.Des1u()->MaxLength() > RMobileENStore::KEmergencyNumberSize)
        {
        // The emergency call number is too big 
        ReqCompleted( aTsyReqHandle, KErrArgument);
        }
    else
        {
        SetDialFlag(ETrue);
        //check that status is Idle
        if ( iMobileCallStatus != RMobileCall::EStatusIdle )
           {
           //The request cannot be forwarded since this call object
           //is still in use.
           //Complete request with status value informing the client
           //about the situation.
           ReqCompleted( aTsyReqHandle, KErrNotReady );
           SetDialFlag(EFalse);
       }
        // If dial already proceeding, 
        else if ( NULL != iTsyReqHandleStore->GetTsyReqHandle(EMultimodeCallDial) ||
                  NULL != iTsyReqHandleStore->GetTsyReqHandle(EMultimodeMobileCallDialEmergencyCall ) )
           {
           ReqCompleted( aTsyReqHandle, KErrServerBusy );
           SetDialFlag(EFalse);
           }

        // Because the Phone Application is the first client that is started,
        // it will always be the priority client. 3rd party applications are
        // now allowed to make emergency calls too.
        else 
           {
           iLastExitCode = KErrNone;
           iCallDirection = RMobileCall::EMobileOriginated;
                
           // Prepare data to pass throught all loop
           TCtsySsmCallbackData callbackData;       
           callbackData.iNumberPackage.SetPacketData(&aNumber);     
           callbackData.iReqHandle = aTsyReqHandle;                     
        
           iMmCallExtInterface->DialEmergencyCall(callbackData );        
           }
        }
        return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::CompleteDialEmergencyCall
// Completes emergency call dialling.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::CompleteDialEmergencyCall(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETEDIALEMERGENCYCALL_1, "TSY: CMmVoiceCallTsy::CompleteDialEmergencyCall. Result:%{TSymbianErrorCodes}", aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETEDIALEMERGENCYCALL_2, "TSY: CMmVoiceCallTsy::CompleteDialEmergencyCall. Call Id:%d Call name:%S", iCallId, iCallName);

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->
        ResetTsyReqHandle( EMultimodeMobileCallDialEmergencyCall );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        if ( aResult != KErrNone )
            {
            iLastExitCode = aResult;

            //clear flag indicating that this is a emergency call
            iEmergencyCall = EFalse;
            ClearCallStatus();
            }
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::DialEmergencyCallCancel
// Cancels DialEmergencyCall request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::DialEmergencyCallCancel(
    const TTsyReqHandle aTsyReqHandle )
    {    
    // Let callback to handle canceling
    TCtsySsmCallbackData callbackData;
    callbackData.iReqHandle = aTsyReqHandle;    
    SsmPluginCallback(KErrCancel,callbackData);     
            
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::FillMobileCallInfo
// The method fills a current snapshot of the call information described in 
// the TMobileCallInfoVx class to the aInfo. The mode specific fields will be 
// filled by the extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::FillMobileCallInfo(
    TDes8* aInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_FILLMOBILECALLINFO_1, "TSY: CMmVoiceCallTsy::FillMobileCallInfo");

    TInt ret = KErrArgument;
    TInt extensionId = 0;
    
	if ( sizeof ( RMobilePhone::TMultimodeType ) <= aInfo->MaxLength() )
    	{
        RMobilePhone::TMultimodeTypePckg* infoParamsPckg =
            reinterpret_cast<RMobilePhone::TMultimodeTypePckg*>(
            aInfo );
        RMobilePhone::TMultimodeType& callInfo = ( *infoParamsPckg )();
            
        // get extensionid from the recieved data
        extensionId = callInfo.ExtensionId();
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_FILLMOBILECALLINFO_2, "TSY: CMmVoiceCallTsy::FillMobileCallInfo - extensionid=%d",callInfo.ExtensionId());
        }
    
    //TMobileCallInfoV1
    if ( KETelExtMultimodeV1 == extensionId ||
    	 KETelExtMultimodeV3 == extensionId ||
    	 KEtelExtMultimodeV7 == extensionId ||
    	 KEtelExtMultimodeV8 == extensionId )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_FILLMOBILECALLINFO_3, "TSY: CMmVoiceCallTsy::FillMobileCallInfo - V1");
        // Set ret to KErrNone. We can fill this parameter class.
        ret = KErrNone;                
            
        RMobileCall::TMobileCallInfoV1Pckg* paramsPckgV1 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV1Pckg*>( aInfo );
        RMobileCall::TMobileCallInfoV1& info = ( *paramsPckgV1 )();
          
        // set mobilecallinfo default values
        FillMobileCallInfoDefaults( &info );
                    
        // fill specific data
        iMmCallExtInterface->FillMobileCallInfo( &info );
        }
            
    // TMobileCallInfoV3
    if ( KETelExtMultimodeV3 == extensionId ||
       	 KEtelExtMultimodeV7 == extensionId ||
       	 KEtelExtMultimodeV8 == extensionId )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_FILLMOBILECALLINFO_4, "TSY: CMmVoiceCallTsy::FillMobileCallInfo - V3");
            
        RMobileCall::TMobileCallInfoV3Pckg* paramsPckgV3 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV3Pckg*>( aInfo );
        RMobileCall::TMobileCallInfoV3& info = ( *paramsPckgV3 )();        
            
        // fill specific data
        iMmCallExtInterface->FillMobileCallInfoV3( &info );
        }

    // TMobileCallInfoV7
    if ( KEtelExtMultimodeV7 == extensionId ||
    	 KEtelExtMultimodeV8 == extensionId	)
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_FILLMOBILECALLINFO_5, "TSY: CMmVoiceCallTsy::FillMobileCallInfo - V7");
            
        RMobileCall::TMobileCallInfoV7Pckg* paramsPckgV7 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV7Pckg*>( aInfo );
        RMobileCall::TMobileCallInfoV7& info = ( *paramsPckgV7 )();
            
        // fill specific data
        iMmCallExtInterface->FillMobileCallInfoV7( &info );
        }
    
    // TMobileCallInfoV8
    if ( KEtelExtMultimodeV8 == extensionId )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_FILLMOBILECALLINFO_6, "TSY: CMmVoiceCallTsy::FillMobileCallInfo - V8");
                
        RMobileCall::TMobileCallInfoV8Pckg* paramsPckgV8 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV8Pckg*>( aInfo );
        RMobileCall::TMobileCallInfoV8& info = ( *paramsPckgV8 )();
                
        // fill specific data
        iMmCallExtInterface->FillMobileCallInfoV8( &info );
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_FILLMOBILECALLINFO_7, "TSY: CMmVoiceCallTsy::FillMobileCallInfo end. ret=%{TSymbianErrorCodes}",ret);
      
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::GoOneToOneL
// This method requests a private communication to the remote party of one 
// call within a conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::GoOneToOneL(
    const TTsyReqHandle aTsyReqHandle )
    {
    //direct request to the extension
    TInt ret( iMmCallExtInterface->GoOneToOneL( iCallId ) );

    if ( ret != KErrNone )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        //save tsy req handle type
        iReqHandleType = EMultimodeCallGoOneToOne;
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::CompleteGoOneToOne
// This method completes an outstanding asynchronous GoOneToOne request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::CompleteGoOneToOne(
    TInt aErrorCode )
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->
        ResetTsyReqHandle( EMultimodeCallGoOneToOne );

    if (KErrNone != aErrorCode)
		{
		CMmConferenceCallTsy* mmConference = iMmPhone->ConferenceCall();
		CMmConferenceCallExtInterface* confCallExt =
            mmConference->GetActiveConferenceCallExtension();
        if ( confCallExt )
            {
            // This cast is safe: As we are here in GSM/WCDMA Call 
            // extension, we know that the active Conference Call 
            // extension is also GSM/WCDMA
            static_cast<CMmConferenceCallGsmWcdmaExt*>( confCallExt )->
                SetGoOneToOneHandlingStarted( EFalse, iCallId );
            }
        }
        
    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::CompleteHold
// Complete the ongoing hold request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::CompleteHold(
    TInt aResult )
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->
        ResetTsyReqHandle( EMultimodeMobileCallHold );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::CompleteResume
// Complete the ongoing resume request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::CompleteResume(
    TInt aResult )
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->
        ResetTsyReqHandle( EMultimodeMobileCallResume );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        // The Call Server has rejected the retrieve request 
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::CompleteResume
// Complete the ongoing swap request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::CompleteSwap(
    TInt aResult )
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->
        ResetTsyReqHandle( EMultimodeMobileCallSwap );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETESWAP_1, "TSY: Swap complete, Call ID: %d", iCallId );
        ReqCompleted( reqHandle, aResult );
        }
        (reinterpret_cast<CMmVoiceLineTsy*>( iMmLine ))->
        SetSwapStatus( CMmVoiceLineTsy::EStatusSwapNotRequested );
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::SetEmergencyMode
// Call mode may change during call creation. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::SetEmergencyMode(
    TBool aIsEmergency )
    {
    //Call mode may change during call creation.
    iEmergencyCall = aIsEmergency;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::IsEmergencyCall
// Is this call an emergency call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmVoiceCallTsy::IsEmergencyCall() const
    {
    //TRUE if emergency call, otherwise FALSE
    return iEmergencyCall;
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::SetTypeOfResponse
// Sets the type of response for a given Handle. Automatic mode includes an 
// automatic response in case of non response from the Domestic OS Layer in a 
// specified time.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::SetTypeOfResponse(
    const TInt aReqHandleType,
    const TTsyReqHandle aTsyReqHandle )
    {
    //Sets the type of response for a given Handle. Automatic mode includes an 
    //automatic response in case of non response from the DOS in a specified 
    //time.
    TInt timeOut( 0 );

    //example switch
    switch ( aReqHandleType )
        {
        case EMultimodeCallDial:
        case EMultimodeCallDialISV:
        case EMultimodeCallDialNoFdnCheck:
            timeOut = KMmCallDialTimeOut;
            break;
        case EMultimodeCallAnswer:
        case EMultimodeCallAnswerISV:
            timeOut = KMmCallAnswerTimeOut;
            break;
        case EMultimodeCallHangUp:
            timeOut = KMmCallHangUpTimeOut;
            break;
        case EMultimodeMobileCallHold:
            timeOut = KMmCallHoldTimeOut;
            break;
        case EMultimodeMobileCallResume:
            timeOut = KMmCallResumeTimeOut;
            break;
        case EMultimodeMobileCallSwap:
            timeOut = KMmCallSwapTimeOut;
            break;
        case EMultimodeMobileCallDeflectCall:
            timeOut = KMmCallSwapTimeOut;
            break;
        case EMultimodeMobileCallDialEmergencyCall:
            timeOut = KMmCallDialEmergencyCallTimeOut;
            break;
        case EMultimodeCallTransfer: 
            timeOut = KMmCallTransferTimeOut;
            break;
        case EMultimodeCallGoOneToOne:
            timeOut = KMmCallGoOneToOneTimeOut;
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
            //case EMobileCallReceiveUUI:

        case EMultimodeCallSetDynamicHscsdParams:  
        default:
            //does not use timer
            iTsyReqHandleStore->SetTsyReqHandle(
                aReqHandleType, aTsyReqHandle );
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
// CMmVoiceCallTsy::Complete
// Completes the request due timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::Complete(
    TInt aReqHandleType,
    TInt aError )
    {
    //All possible TSY req handle types are listed in the
    //switch case below. 
    switch( aReqHandleType )
        {
        //cases handled with automatic completion
        case EMultimodeMobileCallHold:
            CompleteHold( aError );
            break;
        case EMultimodeMobileCallResume:
            CompleteResume( aError );
            break;
        case EMultimodeMobileCallSwap:
            CompleteSwap( aError );
            break;
        case EMultimodeMobileCallDialEmergencyCall:
            CompleteDialEmergencyCall( aError );
            break;
        case EMultimodeCallGoOneToOne:
            CompleteGoOneToOne( aError );
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
            //case EMobileCallReceiveUUI:

        case EMultimodeCallDial:
        case EMultimodeCallDialNoFdnCheck:
        case EMultimodeCallDialISV:
        case EMultimodeCallAnswer:
        case EMultimodeCallAnswerISV:
        case EMultimodeCallHangUp:
        case EMultimodeMobileCallDeflectCall:
        case EMultimodeCallTransfer: 
        case EMultimodeCallSetDynamicHscsdParams:
        default:
            {
            CMmCallTsy::Complete( aReqHandleType, aError );
            break;
            }
        }
    }
#endif

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::GetBearerServiceInfo
// Get bearer service info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::GetBearerServiceInfo(
    const TTsyReqHandle,
    RCall::TBearerService* )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::Connect
// Set correct data call attributes, depending on parameter extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::Connect(
    const TTsyReqHandle,
    const TDesC8* )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::ConnectChancel
// Cancels connecting of a (data) call (Not Supported).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::ConnectCancel(
    const TTsyReqHandle )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::LoanDataPort
// This CORE API method loans dataport to client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::LoanDataPort(
    const TTsyReqHandle,
    RCall::TCommPort* )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::LoanDataPortCancel
// Cancels dataport loaning to client. LoanDataPort always
// completes immediately, this is never used.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::LoanDataPortCancel(
    const TTsyReqHandle )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::RecoverDataPort
// This CORE API method recovers dataport from client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::RecoverDataPort(
    const TTsyReqHandle )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::RecoverDataPortAndRelinquishOwnership
// Recovers dataport from client and relinquishes ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::RecoverDataPortAndRelinquishOwnership()
    {
    return KErrNotSupported;
    }
    
// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::Complete3rdPartyCallNbrCheck
// Completes emergency number check for 3rd party clients
// Decides if dial can be done  
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::Complete3rdPartyCallNbrCheck( TBool aIsEmergencyNbr )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETE3RDPARTYCALLNBRCHECK_1, "TSY: CMmVoiceCallTsy::Complete3rdPartyCallNbrCheck " );

    if( i3rdPartyEmergencyNumberCheckDone )
        {
        if( aIsEmergencyNbr )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETE3RDPARTYCALLNBRCHECK_2, "TSY: CMmVoiceCallTsy::Complete3rdPartyCallNbrCheck - Emergency number" );
            CompleteDial( KErrNotSupported );
            i3rdPartyEmergencyNumberCheckDone = EFalse;
            }
        else
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_COMPLETE3RDPARTYCALLNBRCHECK_3, "TSY: CMmVoiceCallTsy::Complete3rdPartyCallNbrCheck - NOT Emergency nbr - DIAL" );
            i3rdPartyEmergencyNumberCheckDone = ETrue;
            iIsDialISV = ETrue;
            
            TTsyReqHandle reqHandle = iTsyReqHandleStore->
                                    GetTsyReqHandle( EMultimodeCallDialISV );
            
            TInt ret = Dial( reqHandle, iISVCallParams, 
                                        &iNumberCheckMode.iNumber );
            
            if( KErrNone == ret )
                {
                //Dial is made reset   
                i3rdPartyEmergencyNumberCheckDone = EFalse;
                iISVCallParams = NULL;
                }
            }
            
        }
    else
        {
        CompleteDial( KErrGeneral );
        }    
    
    }

// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::DialNoFdnCheck
// This method makes a call without fdn check 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceCallTsy::DialNoFdnCheck( 
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* aCallParams,
    TDesC* aTelNumber )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIALNOFDNCHECK_1, "TSY: CMmVoiceCallTsy::DialNoFdnCheck. Req handle: %u, Call name: %S", (TUint)aTsyReqHandle, iCallName );
	TInt ret( KErrGeneral );
    	
	// Check that there is no dial on going for another call
	CMmCallList* callList = iMmPhone->CallList();

    TBool dialFlag( EFalse );
    
    for ( TInt i=0; i< callList->GetNumberOfObjects();i++ )
        {
        CMmCallTsy* call = callList->GetMmCallByIndex( i );
        
        if( call->GetDialFlag() )
            {
            dialFlag = ETrue;
            i = callList->GetNumberOfObjects();
            }
        }
	
	if( !dialFlag )
    	{
    	ret = KErrNone;
    	SetDialFlag( ETrue );
		
		RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 = NULL;

	    if ( 0 < aCallParams->Length() )
	        {
	        paramsPckgV1 = 
	            reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>(
	                const_cast<TDesC8*>( aCallParams ) );
	        }
	    else
	        {
	        iCallParams.iSpeakerControl = 
	            RCall::EMonitorSpeakerControlOnUntilCarrier;
	        iCallParams.iSpeakerVolume = RCall::EMonitorSpeakerVolumeLow;
	        iCallParams.iInterval = 0;
	        iCallParams.iWaitForDialTone = RCall::EDialToneWait;
	        iCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
	        iCallParams.iCug.iExplicitInvoke = EFalse;
	        iCallParams.iCug.iCugIndex = 0xFFFF;
	        iCallParams.iCug.iSuppressPrefCug = EFalse;
	        iCallParams.iCug.iSuppressOA = EFalse;
	        iCallParams.iAutoRedial = EFalse;
	        
	        paramsPckgV1 = &iCallParamsPckg;
	        }
	    
	    RMobileCall::TMobileCallParamsV1& paramsV1 = ( *paramsPckgV1 )();
	    
	    SetDialTypeId( KMultimodeCallTypeIDNoFdnCheck );
	    
	  	if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )  
	        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIALNOFDNCHECK_2, "TSY: Offline mode ON, DialNoFdnCheck request is not allowed" );
	        ret = CMmCommonStaticUtility::EpocErrorCode(
	            KErrGeneral, KErrGsmOfflineOpNotAllowed );

	        // Complete the request with appropiate error
	        ReqCompleted ( aTsyReqHandle, ret ); 
	        SetDialFlag(EFalse);
	        }
		    // Check that status is Idle
	    else if ( RMobileCall::EStatusIdle != iMobileCallStatus )
	        {
	        //The request cannot be forwarded since this call object
	        //is still in use.
	        //Complete request with status value informing the client about 
	        //the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIALNOFDNCHECK_3, "TSY: CMmVoiceCallTsy::DialNoFdnCheck - KErrNotReady");
	        ReqCompleted( aTsyReqHandle, KErrNotReady );
	        SetDialFlag(EFalse);
	        }
	    else
	        {            
	        // Save call params
	        iCallParams.iSpeakerControl = paramsV1.iSpeakerControl;
	        iCallParams.iSpeakerVolume = paramsV1.iSpeakerVolume;
	        iCallParams.iInterval = paramsV1.iInterval;
	        iCallParams.iWaitForDialTone = paramsV1.iWaitForDialTone;

	        // Set call direction
	        iCallDirection = RMobileCall::EMobileOriginated;
	        // Dial
	      	TRAPD(err, ret = iMmCallExtInterface->DialL(
	                iCallMode, paramsPckgV1, aTelNumber, 
	                KMultimodeCallTypeIDNoFdnCheck ));
	                
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIALNOFDNCHECK_4, "TSY: CMmVoiceCallTsy::DialNoFdnCheck ret = %{TSymbianErrorCodes}", ret);
			if(err != KErrNone)
				{
				ret = err;
				}

	        if ( KErrNone == ret )
	            {
                UpdateCallRoutingControl( iCallName, iMobileCallStatus );
	            
	            // Update core status
	            iCallCaps.iFlags &= ~( RCall::KCapsDial );
	            iCallCaps.iFlags |= RCall::KCapsHangUp;
	            // Complete core caps change
	            CompleteNotifyCapsChange();
	            
#ifdef REQHANDLE_TIMER
                // Set timer for the dialnofdncheck request
                SetTypeOfResponse( EMultimodeCallDialNoFdnCheck, 
                	aTsyReqHandle );    
#else
                
				iTsyReqHandleStore->SetTsyReqHandle(
                    EMultimodeCallDialNoFdnCheck, aTsyReqHandle );
#endif
	            }
			else
            	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_DIALNOFDNCHECK_5, "TSY: CMmVoiceCallTsy::DialNoFdnCheck Before ReqCompleted ret = %{TSymbianErrorCodes}", ret);
            	ReqCompleted( aTsyReqHandle, ret );
            	ClearCallStatus();
            	SetDialFlag(EFalse);
            	}
	        }

    	}
      else
          return KErrServerBusy;
    
    return KErrNone;
	} 
	
	
// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::SsmPluginCallback
// This is callback
// This method is called from CMmCallGsmWcdmaExt, after 
// sendinig request to LTSY       
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::SsmPluginCallback(TInt aResult, TCtsySsmCallbackData& aCallbackData)
	{
	if ( KErrNone != aResult )
    	{    	        	
    	// first of all cancel 
    	// possible request to SSM. It will do nothing if there is no request to SSM
    	
    	TRfStateInfo stateInfo = Phone()->GetRfStateInfo();    	
    	if (ERfsStateInfoNormal != stateInfo)
    		{
    		TCtsySsmCallbackData dummyData;
        	reinterpret_cast<CMmCallGsmWcdmaExt*>
          		( iMmCallExtInterface )->SetRfState(ERfsStateInfoInactive,
          											reinterpret_cast<CMmCallGsmWcdmaExt*>( iMmCallExtInterface ),
           											dummyData);    			
    		}   	    	           											
    	
    	// Then we  should check if iReqHandleType was sent
    	TTsyReqHandle tsyReqHandle = iTsyReqHandleStore->GetTsyReqHandle( EMultimodeMobileCallDialEmergencyCall );    	
    	if (EMultimodeCallReqHandleUnknown != tsyReqHandle)
    		{
    		//here it means that there do was successful request to LTSY  
    		// and we have to cancel request to LTSY (following code is from DialEmergencyCallCancel)
    		
    		// it means that we have to cancel request to LTSY
    		iDialCancelFlag = CMmCallTsy::EDialCancelCallCalled;        
    		// ??? 3 what about tsyReqHandle?
    		if ( iCallId > 0 || (GetDialFlag()&& iCallStatus!=RMobileCall::EStatusIdle) ) 
	        	{
    	    	HangUp(tsyReqHandle);
        		}
    		}    	
    	else  
    		{
    		// we here. It means that there was no successful request to LTSY    		
    		iEmergencyCall = EFalse;
    		
    		// It looks like some error occurs in synchronous branch  		
    		   		
    		// complete request, using regHandle from aCallbackData
    		if (EMultimodeCallReqHandleUnknown != aCallbackData.iReqHandle)
    			{
    			ReqCompleted( aCallbackData.iReqHandle, aResult );
    			}    		
    		aCallbackData.iReqHandle = EMultimodeCallReqHandleUnknown;   		
    		ClearCallStatus(); 
    		}    	
    	}
    else
    	{
    	// if we here it means that request to LTSY was sent successfully
    	iEmergencyCall = ETrue;
        UpdateCallRoutingControl( iCallName, iMobileCallStatus );
    	
    	//update core status
    	iCallCaps.iFlags &= ~( RCall::KCapsDial );
    	iCallCaps.iFlags |= RCall::KCapsHangUp;
        //complete core caps change
        CompleteNotifyCapsChange();
        
        // store reqHandle here, just in case of successful sending request to LTSY
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( EMultimodeMobileCallDialEmergencyCall, aCallbackData.iReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle(
               EMultimodeMobileCallDialEmergencyCall, aCallbackData.iReqHandle );
#endif
        
        }	
	}


// ---------------------------------------------------------------------------
// CMmVoiceCallTsy::UpdateCallRoutingControl
// Establishes call routing control and updates its state
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceCallTsy::UpdateCallRoutingControl( 
                            const TName& aCallName, 
                            RMobileCall::TMobileCallStatus aMobileCallStatus )
	{
    if ( iTelephonyAudioControl )
        {    
        if ( iTelephonyAudioControlSetup )
            {
            iTelephonyAudioControl->CallStateChange( aCallName, 
                                                     aMobileCallStatus );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_UPDATECALLROUTINGCONTROL_1, "TSY: CMmVoiceCallTsy::UpdateCallRoutingControl. Call routing state changed to EStatusDialling: Call name:%S", iCallName );
            }
        else
            {
            iTelephonyAudioControl->SetupTelephonyAudio( 
                                    aCallName, 
                                    MTelephonyAudioControl::ECallTypeVoice,
                                    iEmergencyCall,  
                                    iCallDirection );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICECALLTSY_UPDATECALLROUTINGCONTROL_2, "TSY: CMmVoiceCallTsy::CMmVoiceCallTsy::UpdateCallRoutingControl. Call routing setup: Call name:%S", iCallName );
            iTelephonyAudioControlSetup = ETrue; 
            }
        }
    }

//  End of File  

