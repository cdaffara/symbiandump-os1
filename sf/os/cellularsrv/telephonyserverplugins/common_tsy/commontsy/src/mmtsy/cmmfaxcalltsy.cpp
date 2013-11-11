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
#include "cmmfaxcalltsyTraces.h"
#endif

#include "cmmfaxcalltsy.h"
#include "cmmfaxlinetsy.h"
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmcalllist.h"
#include "CMmCommonStaticUtility.h"
#include "MmTsy_numberOfSlots.h"
#include "cmmlinelist.h"
#include "cmmmessagemanagerbase.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/gsmerror.h>
#include "CMmFaxExtInterface.h" 
#include "cmmcallgsmwcdmaext.h"

// ======== MEMBER FUNCTIONS ========

CMmFaxCallTsy::CMmFaxCallTsy()
    {
    }

CMmFaxCallTsy* CMmFaxCallTsy::NewL( 
    CMmPhoneTsy* aMmPhone, CMmFaxLineTsy* aMmLine,
    RMobilePhone::TMobileService aMode, 
    TDes& aName, 
    CMmMessageManagerBase* aMessageManager )
    {
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_NEWL_1, "TSY: CMmFaxCallTsy::NewL. Call name: %S", aName);

    CMmFaxCallTsy* mmCall = NULL;

    //check input parameters
    if ( aMmPhone != NULL && aMmLine != NULL &&
         aMode == RMobilePhone::EFaxService )
        {
        mmCall = new( ELeave ) CMmFaxCallTsy;
        CleanupClosePushL( *mmCall );
        mmCall->iMmPhone = aMmPhone;
        mmCall->iMmLine = aMmLine;
        mmCall->iCallName = aName;
        mmCall->iCallMode = aMode;
        mmCall->iMessageManager = aMessageManager;
        mmCall->ConstructL( aMode );
        CleanupStack::Pop();
        }

    return mmCall;
    }

CMmFaxCallTsy::~CMmFaxCallTsy()
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_DTOR_1, "TSY: CMmFaxCallTsy::~CMmFaxCallTsy. Call deleted \iCallId:%d iCallName:%S", iCallId, iCallName);

    // If Dial fails, Symbian CSD agent will close the call immediately.
    // This means that TSY has not yet received call status indications, 
    // where call status changes to idle. Thus responses from modem are 
    // passed to call object. 
    // Here we need to assert that call object recovers dataport
    // if it is loaned.
    CCallBase::SetUnowned();

    // delete Internal Fax call extension object.
    if ( iMmFaxExt )
        {
        delete iMmFaxExt;
        }
    iMmFaxExt = NULL;

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
        iLoanedCommPort.iPort.Zero();;
        }
        
    // If this is iLastIncomingFaxCall, set iLastIncomingFaxCall
    // of CMmFaxLineTsy to NULL
    if ( this == (reinterpret_cast<CMmFaxLineTsy*>(iMmLine))->iLastIncomingFaxCall )
    	{
        reinterpret_cast<CMmFaxLineTsy*>(iMmLine)->ResetLastIncomingCall();
    	}
    }


// ---------------------------------------------------------------------------
// CMmFaxCallTsy::InitExtensionModulesL
// Initialises extension modules for CMmFaxCallTsy
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmFaxCallTsy::InitExtensionModulesL(
    RMobilePhone::TMobileService aMode )
    {
    CMmCallTsy::InitExtensionModulesL( aMode );

    // Create internal fax call object if this is fax call
    // TODO: what to do when this is faxmodem fax call??
    if ( aMode == RMobilePhone::EFaxService )
        {
        // Internal Fax call extension object creation.
        iMmFaxExt = CMmFaxExtInterface::NewL( this );
        }
    else
        {
        iMmFaxExt = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::InitInternalAttributes
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmFaxCallTsy::InitInternalAttributes()
    {
    CMmCallTsy::InitInternalAttributes();

    iCallCaps.iFlags = RCall::KCapsFax | RCall::KCapsConnect;

    // Boolean that indicates if this call object is finished data call.
    // Required as Symbian CSD agent in situation where first data call
    // fails due wrong number and it then opens second data call, which
    // dials correct number. Call status notifications go to wrong call
    // object (the first one) and causes problems for clients.
    iIsFinishedDataCall = EFalse;

    //dataport params
    iLoanedCommPort.iCsy.Zero();
    iLoanedCommPort.iPort.Zero();;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::OpenNewObjectByNameL
// Creates new Fax object and returns a pointer to it.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmFaxCallTsy::OpenNewObjectByNameL(    
    const TDesC& aName )
    {
    CTelObject* telObject = NULL;

    // iMmFaxExt is non null if fax calls are supported
    if ( iMmFaxExt )
        {
        telObject = iMmFaxExt->OpenNewObjectByNameL(aName);
        }
    else
        {
        //Not supported
        User::Leave( KErrNotSupported );
        }

    return telObject;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::DoExtFuncL
// DoExtFuncL is called by the Etel server when it has an "extended", i.e. 
// non-core ETel request for the TSY. To process a request handle, request  
// type and request data are passed to the TSY. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_DOEXTFUNCL_1, "TSY: CMmFaxCallTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    TInt ret( KErrNone );

    TAny* dataPtr = aPackage.Ptr1();

    // The request data has to extracted from TDataPackage and the TAny* 
    // pointers have to be "cast" to the expected request data type

    // NOTE! Do not put any Cancel methods here.
    switch ( aIpc )
        {
      // Call Status
        // Get Call Status
        case EMobileCallGetMobileCallStatus:
            ret = GetMobileCallStatus( aTsyReqHandle,
                reinterpret_cast<RMobileCall::TMobileCallStatus*>
                    ( dataPtr ) );
            break;
        // Notify Call Status Change
        case EMobileCallNotifyMobileCallStatusChange:
            ret = NotifyMobileCallStatusChange(
                reinterpret_cast<RMobileCall::TMobileCallStatus*>
                    ( dataPtr ) );
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
            ret =  NotifyMobileCallCapsChange(aPackage.Des1n() );
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
        // Order to notify if privacy is confirmed
        case EMobileCallNotifyPrivacyConfirmation:
            ret = NotifyPrivacyConfirmation(
              reinterpret_cast<RMobilePhone::TMobilePhonePrivacy*>(
                dataPtr ) );
            break;
        // Unsupported features
        case EMobileCallDialEmergencyCall:
        case EMobileCallHold:
        case EMobileCallResume:
        case EMobileCallSwap:
        case EMobileCallGoOneToOne:
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

    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_DOEXTFUNCL_2, "TSY: CMmFaxCallTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::ReqModeL
// ReqModeL is called from the ETel server's CTelObject::ReqAnalyserL in
// order to check the type of request it has.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmFaxCallTsy::ReqModeL(
    const TInt aIpc )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_REQMODEL_1, "TSY: CMmFaxCallTsy::ReqModeL. IPC:%{TIPCNamesList}",aIpc);

    CTelObject::TReqMode ret( 0 ); // default return value
    
    switch ( aIpc )
        {
        //No Flow Control
        //All disabled
        //TSYs wishing to implement their own buffering algorithm will place
        //all requests in this category.
        //This category will also be used by the RecoverDataPort request that
        //must be passed to the TSY to indicate that it may use the
        //communications port after a loan.

        //Get methods that do not use DOS and return immediately.
        //Flow control not required.
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
        // dial is not flow controlled as compeltion may take a while and that
        // blocks for example PS data activation in 3G while dialing.
        case EEtelCallDial:
            break;
        //Flow Controlled Services
        //KReqModeFlowControlObeyed
        //Commands that change the state of the phone, e.g. clearing the
        //AoC counter; are commands that the TSY should only deal with
        //one at a time.
        
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
        //ReqModePostImmediately
        //Requests that notify a client about a change of state, where the
        //TSY needs to distinguish between different clients.
            //ret = KReqModeRePostImmediately;

        //KReqModeMultipleCompletionEnabled
        //(a) commands that may take some time, but which the TSY can handle
        //    more than one of concurrently, or
        //(b) notifications that the TSY does not wish to be re-posted
        //immediately, so the server does no buffering.
            //ret = KReqModeMultipleCompletionEnabled;

        //KReqModeMultipleCompletionEnabled | KReqModeFlowControlObeyed
        //A command that may take some time and which the TSY can only deal
        //with one at a time.
            //ret = KReqModeMultipleCompletionEnabled |
            //    KReqModeFlowControlObeyed;
            //break;

        //Notifications
        //KReqModeMultipleCompletionEnabled | ReqModePostImmediately
        //Requests that notify a client about a change of state.
        //Since these requests do not require the issuing of any modem
        //commands, they do not have to obey flow control.
        //The TSY never gets more than one of these outstanding per 
        // CTelObject.
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
        //Cancel Requests
        //It is not necessary to include the Cancel methods in ReqModeL at all
        //The ETel server never calls ReqModeL with a Cancel IPC.

        //Other variations of return values are unusable

        //Default: Call CCallBase's ReqModeL.
        default:
            //Direct the request to the CCallBase
            ret = CCallBase::ReqModeL( aIpc );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::NumberOfSlotsL
// NumberOfSlotsL is called by the server when it is registering a new 
// NOTIFICATION. It enables the TSY to tell the server how many buffer slots
// to allocate for "repost immediately" notifications that may trigger before
// clients collect them.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    //There is a buffer in the ETel server for each type of NOTIFICATION and
    //the size of the buffer is determined by the TSY. When the ETel server
    //discovers that a request is "repost immediately" it will ask the TSY
    //how big a buffer it wants by calling CTelObject::NumberOfSlotsL( ).
    //
    //It is up to the TSY developer's judgement how many buffer slots this
    //method returns for each notification. If there is a danger that
    //a particular notification could trigger frequently and in rapid
    //succession (e.g. call group change notification) then at least 10 or
    //20 slots may be required. For a notification that triggers rarely,
    //perhaps 1 or 2 slots is enough.
    //
    //So if the ETel server has stored a few occurrences of a particular
    //notification, when a client subsequently calls that notification request
    //the ETel server will complete the client's request with the saved data. 

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
// CMmFaxCallTsy::CancelService
// CancelService is called by the ETel server when it is "cleaning-up" any 
// still outstanding asynchronous requests before closing a client's 
// sub-session. This will happen if a client closes its R-class handle without 
// cancelling outstanding asynchronous requests. Only Mobile API requests are 
// handled here.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::CancelService(
    const TInt aIpc,
    const TTsyReqHandle aTsyReqHandle )
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_CANCELSERVICE_1, "TSY: CMmFaxCallTsy::CancelService. IPC: %{TIPCNamesList}, Req handle: %d",(TUint)aIpc, (TUint)aTsyReqHandle);

    TInt ret( KErrNone );

    //When the clients close their sub-sessions (eg. by calling RPhone::Close)
    //they may not have cancelled all their outstanding asynchronous requests
    //before closing.  It is up to the ETel server to clean up in this
    //situation, so the server will find the list of outstanding requests
    //related to that sub-session object and pass these outstanding IPC req.
    //numbers, one at a time, to the CancelService function in the TSY.

    switch ( aIpc )
        {
        //TSY has started a request and it is not possible to then
        //cancel this request. The best thing for the TSY to do in this case
        //is to proceed as though the Cancel never happened. The server's call
        //to the TSY cancel function will return synchronously. The TSY then
        //continues to wait for the original acknowledgement and when it
        //receives it, the TSY will complete the original request.
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
        case EMobileCallReceiveUUI:
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
        //Default case
        default:
            //call CCallBase
            ret = CCallBase::CancelService( aIpc, aTsyReqHandle );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::CompleteNotifyStatusChange
// Complete notification when status changes. If the new status requires,
// this method will ask call protocol extension to update it's status and
// capabilities and line owning this call to update it's status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmFaxCallTsy::CompleteNotifyStatusChange(
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
            CMmCommonStaticUtility::ExtendedErrorCode( aResult) );
        TInt ret( KErrNone );
        TBool timerStarted( EFalse );
        TBool statusChanged ( EFalse );
        TBool mobileStatusChanged ( EFalse );
        CMmFaxCallTsy* mmCall = NULL; 
        RMobileCall::TMobileCallStatus callStatus( RMobileCall::EStatusIdle );
        CCallDataPackage* callDataPackage = 
            reinterpret_cast<CCallDataPackage*>(aDataPackage);

        callDataPackage->UnPackData( callStatus );

        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_COMPLETENOTIFYSTATUSCHANGE_1, "TSY: CMmFaxCallTsy::CompleteNotifyStatusChange. aResult:%{TSymbianErrorCodes}",aResult );
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_COMPLETENOTIFYSTATUSCHANGE_2, "TSY: CMmFaxCallTsy::CompleteNotifyStatusChange. \Call status:%d Call name:%S", callStatus, iCallName);

        switch( callStatus )
            {
            case RMobileCall::EStatusIdle:
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

                iCallStatus = RCall::EStatusIdle;
                iMobileCallStatus = RMobileCall::EStatusIdle;

                SetUnowned();

                ClearCallStatus();
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
                        //set last exit code
                        iLastExitCode = aResult;
                        CompleteHangUp( aResult );
                        if ( this != iMmPhone->WaitingCallForData() )
                            {
                            CompleteAnswerIncomingCall( aResult );
                            }
                        }
                    CompleteDial( aResult );
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
                    CompleteHangUp( errorValue );
                    if ( this != iMmPhone->WaitingCallForData() )
                        {
                        CompleteAnswerIncomingCall( errorValue );
                        }
                    }
                //save last id. Required by Conference call implementation
                SetPreviousCallId( iCallId );
                //reset call id
                iCallId = -1;

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
                break;
                //End of case KCallStatusIdle
            case RMobileCall::EStatusDialling:
                #ifdef REQHANDLE_TIMER
                iTsyReqHandleStore->StopTimeout( EMultimodeCallDial );
                #endif
                iCallStatus = RCall::EStatusDialling;
                iMobileCallStatus = RMobileCall::EStatusDialling;
                statusChanged = ETrue;
                if ( iDialCancelFlag != CMmCallTsy::EDialCancelNotCalled )
                    {
                    TTsyReqHandle dialCancelHandle = iTsyReqHandleStore->
                        GetTsyReqHandle( EMultimodeCallDial );

                    if ( 0 < dialCancelHandle )
                        {
                        HangUp( dialCancelHandle );
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
                // If mode is fax call, there is a chance that call mode is 
                //already ringing
                if (RCall::EStatusRinging != iCallStatus ||
                    RMobileCall::EStatusRinging != iMobileCallStatus)
                    {
                    iCallStatus = RCall::EStatusRinging;
                    iMobileCallStatus = RMobileCall::EStatusRinging;
                    statusChanged = ETrue;
                    }
                break;
            case RMobileCall::EStatusAnswering:
                iCallStatus = RCall::EStatusAnswering;
                iMobileCallStatus = RMobileCall::EStatusAnswering;
                statusChanged = ETrue;

                CompleteAnswerIncomingCall( aResult );
                break;
            case RMobileCall::EStatusConnected:
                //check previous status. If status is answering
                if ( RCall::EStatusAnswering == iCallStatus )
                    {
                    //Start call duration monitoring
                    iCallTimer->Start(); 
                    //Check if start the air time duration monitoring
                    iMmPhone->AirTimeTimerCheckStart();
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
                    }

                //Don't update status if it has not changed.
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
                //Core status
                iCallStatus = RCall::EStatusHangingUp; 
                statusChanged = ETrue;
                // Mobile status
                iMobileCallStatus = RMobileCall::EStatusDisconnecting;
                mobileStatusChanged = ETrue;

                if ( iCallDirection == RMobileCall::EMobileTerminated )
                    {
                    //CompleteDial in case remote user is busy
                    CompleteDial( aResult );

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
                    }
                break;
            case RMobileCall::EStatusDisconnectingWithInband:
                // When call is released, this call object becomes "used".
                // Thus set iIsFinishedDataCall to ETrue. 
                iIsFinishedDataCall = ETrue;
                //Core status
                iCallStatus = RCall::EStatusHangingUp; 
                statusChanged = ETrue;
                // Mobile status
                iMobileCallStatus =
                    RMobileCall::EStatusDisconnectingWithInband;
                mobileStatusChanged = ETrue;
                //CompleteDial in case remote user is busy
                CompleteDial( aResult );

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
                break;
            case RMobileCall::EStatusHold:
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
        TTsyReqHandle reqHandle = iTsyReqHandleStore->
            GetTsyReqHandle( EMultimodeCallNotifyStatusChange );

        if ( ( EMultimodeCallReqHandleUnknown != reqHandle ) 
            && statusChanged )
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
            ( statusChanged || mobileStatusChanged ) )
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

        mmCall = reinterpret_cast<CMmFaxCallTsy*>(
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
                    }
                } 
            callIndex++;
            mmCall = reinterpret_cast<CMmFaxCallTsy*>(
                iMmPhone->CallList()->GetMmCallByIndex( callIndex ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::Dial
// This CORE API method dials to the given number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::Dial(
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* aCallParams,
    TDesC* aTelNumber )
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_DIAL_1, "TSY: CMmFaxCallTsy::Dial. Req handle: %u, Call name: %S", aTsyReqHandle, iCallName);

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
	SetDialFlag( ETrue );
    TTsyReqHandle dialHandle = iTsyReqHandleStore->
        GetTsyReqHandle( EMultimodeCallDial );
    
    //reset exit code
    iLastExitCode = KErrNone;

    //reset finished data call flag
    iIsFinishedDataCall = EFalse;

    if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_DIAL_2, "TSY: Offline mode ON, Dial request is not allowed" );
        TInt ret = CMmCommonStaticUtility::EpocErrorCode(
            KErrGeneral, KErrGsmOfflineOpNotAllowed );

        //Complete the request with appropiate error
        ReqCompleted ( aTsyReqHandle, ret ); 
        }

    //check that status is Idle
    else if ( RMobileCall::EStatusIdle != iMobileCallStatus )
        {
        //The request cannot be forwarded since this call object
        //is still in use.
        //Complete request with status value informing the client
        //about the situation.
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_DIAL_3, "TSY: CMmFaxCallTsy::Dial - KErrNotReady");
        ReqCompleted( aTsyReqHandle, KErrNotReady );
        }
    else if ( 0 < dialHandle )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_DIAL_4, "TSY: CMmFaxCallTsy::Dial - KErrServerBusy");
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        TInt ret( KErrNone );

        RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 = 
            reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>(
                const_cast<TDesC8*>( aCallParams ) );
        RMobileCall::TMobileCallParamsV1& paramsV1 = ( *paramsPckgV1 )();

        //save call params
        iCallParams.iSpeakerControl = paramsV1.iSpeakerControl;
        iCallParams.iSpeakerVolume = paramsV1.iSpeakerVolume;
        iCallParams.iInterval = paramsV1.iInterval;
        iCallParams.iWaitForDialTone = paramsV1.iWaitForDialTone;

        //set call direction
        iCallDirection = RMobileCall::EMobileOriginated;

        // if fax extension is loaded do a fax call, otherwise return not supported
        if (iMmFaxExt)
            {
            // Check if there already is an active or connecting call
            TInt numberOfObjects = iMmPhone->CallList()->
                GetNumberOfObjects();
            for ( TInt i = 0; i < numberOfObjects; i++ )
                {
                CMmCallTsy* mmCall = iMmPhone->CallList()->
                    GetMmCallByIndex( i );
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

            if ( KErrNone == ret )
                {
                ret = iMmFaxExt->Dial(aTsyReqHandle, aTelNumber);
                }
            }
        else
            {
            ret = KErrNotSupported;
            }

        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            ClearCallStatus();
            }
        else
            {
#ifdef REQHANDLE_TIMER
            //set timer for the request
            SetTypeOfResponse( EMultimodeCallDial, aTsyReqHandle );
#else
            //set timer
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
// CMmFaxCallTsy::DialCancel
// This CORE API method cancels an outstanding dial request.Calls HangUp 
// method to do this and set a flag so that we can know that DialCancel 
// handling is going on.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::DialCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_DIALCANCEL_1, "TSY: CMmFaxCallTsy::DialCancel. Req handle: %u, Call name: %S", aTsyReqHandle, iCallName);

    TInt ret( KErrGeneral );

    if (iMmFaxExt)
        {
        iMmFaxExt->DialCancel();
        // ingnore dialcancel for internal fax call
        return KErrNone;
        }

    ret = CMmCallTsy::DialCancel( aTsyReqHandle );

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::AnswerIncomingCall
// This CORE API method is used for answering to an incoming call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::AnswerIncomingCall(
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* /*aCallParams*/ )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_ANSWERINCOMINGCALL_1, "TSY: CMmFaxCallTsy::AnswerIncomingCall. \n\t\t\t Handle:%d",aTsyReqHandle);

    TTsyReqHandle iAnswerCallHandle = iTsyReqHandleStore->
        GetTsyReqHandle( EMultimodeCallAnswer );

    if ( 0 < iAnswerCallHandle )
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

        if ( iMmFaxExt )
            {
            // internal fax call...
            ret = iMmFaxExt->AnswerIncomingCall( aTsyReqHandle );
            }
        else
            {
            // FaxModemMode should send the ATA command itself,
            // not call answer from TSY!
            // fax receive case comes here...
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
// CMmFaxCallTsy::AnswerIncomingCallCancel
// TSY has started a request and it is not possible to then cancel this 
// request. The best thing for the TSY to do in this case is to proceed as 
// though the Cancel never happened. The server's call to the TSY cancel 
// function will return synchronously. The TSY then continues to wait for the 
// AnswerIncomingCall() acknowledgemnt and when it receives it, the TSY will 
// complete the original AnswerIncomingCall request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::AnswerIncomingCallCancel(
    const TTsyReqHandle )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::HangUp
// This CORE API method disconnects the call. Used with normal voice calls, 
// emergency calls as well as data calls. DialCancel also uses this method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::HangUp(
    const TTsyReqHandle aTsyReqHandle )
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_HANGUP_1, "TSY: CMmFaxCallTsy::HangUp. Req handle: %u, Call name: %S", aTsyReqHandle, iCallName);

    TInt hangUpCause( KErrNone );
    TInt ret( KErrNone );
    TInt trapError ( KErrNone );

    TTsyReqHandle hangUpHandle = iTsyReqHandleStore->
        GetTsyReqHandle( EMultimodeCallHangUp );
    TTsyReqHandle dialHandle = iTsyReqHandleStore->
        GetTsyReqHandle( EMultimodeCallDial );
            
    if ( 0 < hangUpHandle )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else if ( RCall::EStatusIdle == iCallStatus && 
        EMultimodeCallReqHandleUnknown == dialHandle )
        {
        //Call object is already in idle state.
        //Complete HangUp request with error.
        ReqCompleted( aTsyReqHandle, KErrNotReady );
        }
    else
        {
        //if this was not called by DialCancel
        if ( CMmCallTsy::EDialCancelNotCalled == iDialCancelFlag ) 
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
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_HANGUP_2, "TSY: CMmFaxCallTsy::HangUp - Reject incoming call");
            }
        //Phone Application is the first client that is started, it 
        //will always be the priority client and thus able to hangup calls
        //Owner of the call is also able to hangup the call.
        else if ( CCallBase::CheckPriorityClient( aTsyReqHandle ) ||
            ( CheckOwnership( aTsyReqHandle ) != EOwnedFalse ) )
            {
            if ( iMmFaxExt != NULL )
                {
                // internal fax hangup handling
                iMmFaxExt->HangUp();
                }
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
            //in case of memory error
            if ( KErrNone != trapError )
                {
                // Object cannot be created.
                ret = trapError;
                }
            else
                {
                //Create package
                CCallDataPackage package;
                //Set call id and call mode
                package.SetCallIdAndMode( iCallId, iCallMode );
                
                TBool autoStChangeDisable = EFalse;
                //Pack call parameters and mobile call info
                package.PackData( &hangUpCause, &autoStChangeDisable );
                //Send request to the Domestic OS layer.
                TRAP(trapError,
                    ret = iMessageManager->HandleRequestL(
                        EEtelCallHangUp,  &package );
                    );
                }

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
                }
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::GetOwnershipStatus
// Get call ownership status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::GetOwnershipStatus(
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
// CMmFaxCallTsy::TransferOwnership
// Transfers the call ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::TransferOwnership(
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
// CMmFaxCallTsy::AcquireOwnership
// Acquire the call ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::AcquireOwnership(
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
        TRAP( trapError,
            entry = CAcquireEntry::NewL( aTsyReqHandle );
            );
        if ( trapError != KErrNone )
            {
            // Object cannot be created.
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
// CMmFaxCallTsy::AcquireOwnershipCancel
// Cancel the call ownership acquire.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::AcquireOwnershipCancel(
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
// CMmFaxCallTsy::RelinquishOwnership
// Relinquish the call ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::RelinquishOwnership()
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
// CMmFaxCallTsy::RegisterNotification
// RegisterNotification is called when the server recognises that this 
// notification is being posted for the first time on this sub-session object. 
// It enables the TSY to "turn on" any regular notification messages that it 
// may receive from DOS. Currently does not really do anything but returns 
// KErrNone to ETel server in case of known notification request type. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::RegisterNotification(
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
// CMmFaxCallTsy::DeregisterNotification
// DeregisterNotification is called when the server recognises that this 
// notification will not be posted again because the last client to have a 
// handle on this sub-session object has just closed the handle. It enables
// the TSY to "turn off" any regular notification messages that it may receive
// from DOS. Currently does not really do anything but returns KErrNone to 
// ETel server in case of known notification request type.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::DeregisterNotification(
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
// CMmFaxCallTsy::ClearCallStatus
// Clears internal call status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmFaxCallTsy::ClearCallStatus()
    {
    SetUnowned();
    CMmCallTsy::ClearCallStatus();
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmFaxCallTsy::SetTypeOfResponse
// Sets the type of response for a given Handle. Automatic mode includes an
// automatic response in case of non response from the Domestic OS Layer in
// a specified time.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmFaxCallTsy::SetTypeOfResponse(
    const TInt aReqHandleType,
    const TTsyReqHandle aTsyReqHandle )
    {
    //Sets the type of response for a given Handle.
    //Automatic mode includes an automatic response in case of
    //non response from the DOS in a specified time.
    TInt timeOut( 0 );

    //example switch
    switch ( aReqHandleType )
        {
        case EMultimodeCallDial:
            timeOut = KMmCallDialTimeOut;
            break;
        case EMultimodeCallAnswer:
            timeOut = KMmCallAnswerTimeOut;
            break;
        case EMultimodeCallHangUp:
            timeOut = KMmCallHangUpTimeOut;
            break;
        case EMultimodeCallTransfer: 
            timeOut = KMmCallTransferTimeOut;
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
            aReqHandleType, 
            aTsyReqHandle, 
            timeOut );
        }
    }
#endif

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::IsFinishedData
// Returns boolean that indicates if call object is already been used in
// fax calls. This means that call has been already released or disconnected.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmFaxCallTsy::IsFinishedData() const
    {
    return iIsFinishedDataCall;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::LoanDataPort
// This CORE API method loans dataport to client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::LoanDataPort(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TCommPort* aCommPort )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_LOANDATAPORT_1, "TSY: CMmFaxCallTsy::LoanDataPort - Client taking control: %S",iCallName );

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
// CMmFaxCallTsy::LoanDataPortCancel
// Cancels dataport loaning to client. LoanDataPort always
// completes immediately, this is never used.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::LoanDataPortCancel(
    const TTsyReqHandle )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::RecoverDataPort
// This CORE API method recovers dataport from client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::RecoverDataPort(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_RECOVERDATAPORT_1, "TSY: CMmFaxCallTsy::RecoverDataPort - Client returning control: %S",iCallName );

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
        iLoanedCommPort.iPort.Zero();;

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

    ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::RecoverDataPortAndRelinquishOwnership
// Recovers dataport from client and relinquishes ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::RecoverDataPortAndRelinquishOwnership()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCALLTSY_RECOVERDATAPORTANDRELINQUISHOWNERSHIP_1, "TSY: CMmFaxCallTsy::RecoverDataPortAndRelinquishOwnership - \Client returning control: %S", iCallName );

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
        iLoanedCommPort.iPort.Zero();;

        if ( iList->iAcquireList.IsEmpty() )
            {
            SetUnowned();
            
            if ( KErrNone == ret )
                {
                // Call owner is closing fax call handle. Call 
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
// CMmFaxCallTsy::Connect
// Set correct data call attributes, depending on parameter extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::Connect(
    const TTsyReqHandle aTsyReqHandle,
    const TDesC8* aCallParams )
    {
    TInt ret(KErrArgument);
    
    if(sizeof(RMobileCall::TMobileCallParamsV1) <= aCallParams->Length())
       	{
       	ret = KErrNone;
       	
        if ( CheckOwnership( aTsyReqHandle ) == CCallBase::EOwnedUnowned )
            {
            SetOwnership( aTsyReqHandle );
            }

        RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 = 
            reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>(
                const_cast<TDesC8*>( aCallParams ) );
        RMobileCall::TMobileCallParamsV1& paramsV1 = ( *paramsPckgV1 )();

        iCallParams.iSpeakerControl = paramsV1.iSpeakerControl;
        iCallParams.iSpeakerVolume = paramsV1.iSpeakerVolume;
        iCallParams.iInterval = paramsV1.iInterval;
        iCallParams.iWaitForDialTone = paramsV1.iWaitForDialTone;

        if ( paramsV1.ExtensionId() == KETelExtMultimodeV1 ||
            paramsV1.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV1 ||
            paramsV1.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV2 ||
            paramsV1.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV8 || 
            paramsV1.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV1 ||
            paramsV1.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV2 ||
            paramsV1.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV7 ||
            paramsV1.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV8 )
            {
            iCallParams.iIdRestrict = paramsV1.iIdRestrict;
            iCallParams.iCug = paramsV1.iCug;
            iCallParams.iAutoRedial = paramsV1.iAutoRedial;
            }
        if ( paramsV1.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV1 ||
        	 paramsV1.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV2 ||
        	 paramsV1.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV8 || 
             paramsV1.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV1 ||
             paramsV1.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV2 ||
             paramsV1.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV7 ||
             paramsV1.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV8 )
            {
            iMmCallExtInterface->Connect( aCallParams );
            }

        ReqCompleted( aTsyReqHandle, KErrNone );
       	
       	}
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::ConnectChancel
// Cancels connecting of a (fax) call (Not Supported).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::ConnectCancel(
    const TTsyReqHandle )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::GetBearerServiceInfo
// Get bearer service info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::GetBearerServiceInfo(
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
// CMmFaxCallTsy::GetFaxSettings
// This CORE API method is used for getting fax settings.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::GetFaxSettings(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TFaxSessionSettings* aSettings )
    {
    TInt errorCode ( KErrNotSupported );

    if ( iMmFaxExt )
        {
        // if fax is supported, handle this request
        errorCode = iMmFaxExt->GetFaxSettings( aSettings );
        }

    ReqCompleted( aTsyReqHandle, errorCode );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::SetFaxSettings
// This CORE API method is used for set fax settings.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::SetFaxSettings(
    const TTsyReqHandle aTsyReqHandle,
    const RCall::TFaxSessionSettings* aSettings )
    {
    TInt errorCode(KErrNotSupported);

    if ( iMmFaxExt )
        {
        // if fax is supported, handle this request
        errorCode = iMmFaxExt->SetFaxSettings( aSettings );
        }

    ReqCompleted( aTsyReqHandle, errorCode );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmFaxCallTsy::GetMobileDataCallRLPRange
// Get Data Call RLP Range.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxCallTsy::GetMobileDataCallRLPRange(
    const TTsyReqHandle aTsyReqHandle,
    TInt*,
    TDes8* )
    {
    ReqCompleted( aTsyReqHandle, KErrNotSupported );

    return KErrNone;
    }


//  End of File  
