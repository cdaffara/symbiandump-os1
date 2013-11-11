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
#include "cmmcalltsyTraces.h"
#endif

#include "cmmcalltsy.h"
#include <featmgr/featurecontrol.h>
#include <featureuids.h>
#include "cmmphonetsy.h"
#include "cmmlinetsy.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmcalllist.h"
#include "CMmCommonStaticUtility.h"
#include "cmmlinelist.h"
#include "cmmmessagemanagerbase.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/gsmerror.h>
#include "cmmcallgsmwcdmaext.h"
#include <ctsy/pluginapi/mtelephonyaudiocontrol.h>

// ======== MEMBER FUNCTIONS ========

CMmCallTsy::CMmCallTsy() :
    iCallParamsPckg(iCallParams),
    iReqHandleType(EMultimodeCallReqHandleUnknown)
    {
    }

void CMmCallTsy::ConstructL( 
	RMobilePhone::TMobileService aMode )
    {    
#ifdef REQHANDLE_TIMER   
    //Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this, iMmPhone, 
        EMultimodeCallMaxNumOfRequests, iCallReqHandles);
#else
    //Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        EMultimodeCallMaxNumOfRequests, iCallReqHandles);
#endif

    iCallMode = aMode;

    //Call ownership change request list
    iList = CAcquireOwnerList::NewL();

    //initialise call duration notifier
    iCallTimer = CHeartbeatRunner::NewL( this );

    //Initialise extension modules
    InitExtensionModulesL( aMode );

    //Initialise miscellaneous internal attributes
    InitInternalAttributes();
    }

CMmCallTsy::~CMmCallTsy()
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_DTOR_1, "TSY: CMmCallTsy::~CMmCallTsy. Call deleted iCallId:%d iCallName:%S",iCallId, iCallName);

    TInt trapError( KErrNone );

    //Hang up call if call is still connecting or active and hang up is not
    //called
    if( iTsyReqHandleStore )
        {
        if ( !( RMobileCall::EStatusIdle == iMobileCallStatus ||
            RMobileCall::EStatusDisconnecting == iMobileCallStatus ||
            RMobileCall::EStatusDisconnectingWithInband == iMobileCallStatus ) 
            && ( 0 == iTsyReqHandleStore->GetTsyReqHandle( 
                                                    EMultimodeCallHangUp ) ) )
            {
                TBool timerStarted = EFalse;
                if( iCallTimer )
                    {
                    timerStarted = iCallTimer->Stop();
                    }
            //Check air time timer only if call was in active state(call timer 
            //was started)
            if ( timerStarted )
                {
                if( iMmPhone )
                	{
                	iMmPhone->AirTimeTimerCheckStop();
                	//TODO: attention reviewers - please check if UpdateLifeTimer should/shouldn't be called here
                    UpdateLifeTimer();
                    }
                }
    
            //Send call release request before deleting this object
            TInt symbianHangUpCause( KErrGsmReleaseByUser );
    
    		//Create package
    		CCallDataPackage package;
    		//Set call id and call mode
    		package.SetCallIdAndMode( iCallId, iCallMode );
    		
    		TBool autoStChangeDisable = EFalse;
    		//Pack call parameters and mobile call info
    		package.PackData( &symbianHangUpCause, &autoStChangeDisable );
    		//Send request to the Domestic OS layer.
            if( iMessageManager )
            	{
            	TRAP( trapError,
                iMessageManager->HandleRequestL( EEtelCallHangUp, &package );
                );
                // inform telephony audio control that we're hanging up the call	
                if ( iTelephonyAudioControlSetup && iTelephonyAudioControl )
                    {
                    iTelephonyAudioControl->TeardownTelephonyAudio( iCallName, 
                                                                    KErrNone );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_DTOR_2, "TSY: CMmCallTsy::~CMmCallTsy Call routing tear down: Call name:%S", iCallName );
                    }
                
            	}
    
            // We can't do nothing if hanguprequest fails here,
            // because call object is closed.
            }
        }

    //delete req handle store
    if ( iTsyReqHandleStore )
        {
        delete iTsyReqHandleStore;
        }
    iTsyReqHandleStore = NULL;
    
    //delete req handle store
    if ( iList )
        {
        delete iList;
        }
    iList = NULL;

    //delete call ownership change request list
    if ( iCallTimer )
        {
        delete iCallTimer;
        }
    iCallTimer = NULL;

    //Decrement number of opened calls from line.
    if( iMmLine )
    	{
    	iMmLine->DecrementNumberOfCalls();
    	}
    
    // Remove the call from the list of calls
    if( iMmPhone )
    	{
    	iMmPhone->CallList()->RemoveObject( this );
    	}

    //delete call extension
    if ( iMmCallExtInterface )
        {
    	delete iMmCallExtInterface;
        }
    iMmCallExtInterface = NULL;

    iMmPhone = NULL;
    iMmLine = NULL;
    iCallTimer = NULL;
    iRetNotifyMobileCallCaps = NULL;
    iRetCaps = NULL;
    iRetStatus = NULL;
    iRetMobileCallStatus = NULL;
    iRetRemotePartyInfo = NULL;
    iRetTimeIntervalSeconds = NULL;
    iRetCallEvent = NULL;
    iList = NULL;
    iTsyReqHandleStore = NULL;
    iRetPrivacyStatus = NULL;
    iMessageManager = NULL;
    iRetUUI = NULL; 
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::InitExtensionModulesL
// Initialises extension modules for CMmCallTsy
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::InitExtensionModulesL(
    RMobilePhone::TMobileService aMode )
    {
    RMobilePhone::TMobilePhoneNetworkMode currentMode;
    iMmPhone->GetStaticExtensionMode( &currentMode );

    iMmCallExtInterface  = reinterpret_cast<CMmCallExtInterface*> 
        (CMmCallGsmWcdmaExt::NewL( this, aMode, iMessageManager ) );

    }

// ---------------------------------------------------------------------------
// CMmCallTsy::InitInternalAttributes
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::InitInternalAttributes()
    {
    //Set status parameters
    iCallStatus = RCall::EStatusIdle;
    iMobileCallStatus = RMobileCall::EStatusIdle;

    //Initialise call params
    iCallParams.iSpeakerControl = RCall::EMonitorSpeakerControlOnUntilCarrier;
    iCallParams.iSpeakerVolume = RCall::EMonitorSpeakerVolumeLow;
    iCallParams.iInterval = 4;
    iCallParams.iWaitForDialTone = RCall::EDialToneWait;
    iCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    iCallParams.iCug.iExplicitInvoke = EFalse;
    iCallParams.iCug.iCugIndex = 0;
    iCallParams.iCug.iSuppressPrefCug = EFalse;
    iCallParams.iCug.iSuppressOA = EFalse;
    iCallParams.iAutoRedial = EFalse;
    // CallParamsV2
    iCallParams.iBearerMode = RMobileCall::EMulticallNotSupported;

    // CallParamsV7
    iCallParams.iCallParamOrigin = RMobileCall::EOriginatorUnknown;
    iCallParams.iBCRepeatIndicator = RMobileCall::EBCAlternateMode;
    iCallParams.iIconId.iQualifier = RMobileCall::EIconQualifierNotSet;
    iCallParams.iIconId.iIdentifier = 0;
    iCallParams.iAlphaId.Zero();
    iCallParams.iSubAddress.Zero();
    iCallParams.iBearerCap1.Zero();
    iCallParams.iBearerCap2.Zero();
    iCallParams.iBCRepeatIndicator = RMobileCall::EBCAlternateMode;
    
    iDialCancelFlag = CMmCallTsy::EDialCancelNotCalled;
    iAnswerCancelFlag = EFalse;
    iHangUpFlag = EFalse; 

    //this is required for the Conference call.
    iPreviousCallId = -1;

    //Initialize last exit code
    iLastExitCode = KErrNotFound;
    //this is required for custom tsy
    iDiagnosticOctet = NULL;

    //by default a call object has been created through ETel
    iGhostCall = EFalse;
    iEtelOriginated = ETrue;

    iPreviousCallControlCaps = 0;
	iIsRemoteReleasedCall = EFalse;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::OpenNewObjectByNameL
// Creates new  object and returns a pointer to it. Not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmCallTsy::OpenNewObjectByNameL(    
    const TDesC& )
    {
    //Not supported
    User::Leave( KErrNotSupported );
	//lint -e{527} "unreachable code"
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::OpenNewObjectL
// Creates new object and returns a pointer to it. Not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmCallTsy::OpenNewObjectL(
    TDes& )
    {
    // Not supported
    User::Leave( KErrNotSupported );
	//lint -e{527} "unreachable code"
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::ExtFunc
// TRAP's all CMmCallTsy related MM API requests in case that they fail. This 
// method functions only as a centralized TRAP for the DoExtFuncL method that 
// does the actual mapping of IPC number to TSY method call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::ExtFunc( 
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
    __ASSERT_DEBUG(iReqHandleType==EMultimodeCallReqHandleUnknown, User::Invariant());


    //before processing further the request, check if offline mode status
    //is enabled and if the given request can be perfomed in that case.
    if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo () &&
        !IsRequestPossibleInOffline( aIpc ) )  
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_EXTFUNC_1, "TSY: Offline mode ON, request is not allowed: %{TIPCNamesList}", aIpc );
        ret = CMmCommonStaticUtility::EpocErrorCode(
            KErrGeneral, KErrGsmOfflineOpNotAllowed );

        //Complete the request with appropiate error
        ReqCompleted ( aTsyReqHandle, ret ); 
        }
    else
        {
        TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ); );

        if ( trapError != KErrNone )
            {
            //error handling. Object cannot be created. 
			//reset request handle to indicate the request is no longer ongoing
            iTsyReqHandleStore->FindAndResetTsyReqHandle(aTsyReqHandle);
            ReqCompleted( aTsyReqHandle, trapError );
            }
        else if ( ret != KErrNone )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }   
        else if ( EMultimodeCallReqHandleUnknown != iReqHandleType )
            {
            //save request handle
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle(
                iReqHandleType, aTsyReqHandle );
#endif
            }
        // We've finished with this value now. Clear it so it doesn't leak
        //  up to any other instances of this method down the call stack
        iReqHandleType = EMultimodeCallReqHandleUnknown;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetInfo
// This CORE API method returns core Call information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetInfo(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TCallInfo* aCallInfo )
    {
    //Fill Core Call information
    aCallInfo->iCallName = iCallName;
    aCallInfo->iStatus = iCallStatus;
    aCallInfo->iLineName = iMmLine->LineName();
    aCallInfo->iHookStatus = RCall::EHookStatusUnknown;
    aCallInfo->iDuration = GetCallDurationInSeconds();

    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyCapsChange
// This CORE API method requests notifications when core caps change.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyCapsChange(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TCaps* aCaps )
    {
    iRetCaps = aCaps;
    //Store the current capabilities
    iPreviousCaps =  iCallCaps;

    iTsyReqHandleStore->SetTsyReqHandle(
        EMultimodeCallCapsChangeNotification, aTsyReqHandle );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyCapsChangeCancel
// This CORE API method cancels core caps change notifications.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyCapsChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {    
    iRetCaps = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallCapsChangeNotification );

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteNotifyCapsChange
// Complete notification when caps change.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteNotifyCapsChange()
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallCapsChangeNotification );

    //if caps have changed and client has requested capability notifications
    if ( EMultimodeCallReqHandleUnknown != reqHandle &&
        iRetCaps != NULL &&
        iCallCaps.iFlags != iPreviousCaps.iFlags )
        {
        iRetCaps->iFlags = iCallCaps.iFlags;
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyHookChange
// This CORE API method requests notifications on hook change.
// Feature not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyHookChange(
    const TTsyReqHandle aTsyReqHandle,
    RCall::THookStatus* )
    {
    ReqCompleted( aTsyReqHandle, KErrNotSupported );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyHookChange
// This CORE API method cancels hook change notifications.
// Feature not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyHookChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    ReqCompleted( aTsyReqHandle, KErrNotSupported );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyStatusChange
// This CORE API method requests notifications when Core status changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyStatusChange(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TStatus* aStatus )
    {
    // Save request handle and ptr to aStatus. 
    iRetStatus = aStatus;
    iTsyReqHandleStore->SetTsyReqHandle(
        EMultimodeCallNotifyStatusChange, aTsyReqHandle );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyStatusChangeCancel
// This CORE API method cancels core status change notifications.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyStatusChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetStatus = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallNotifyStatusChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::HandleGhostCallStatusChange
// Handles status change notifications of a call object that is not monitored 
// by any client (handle to this call object has not been opened by any client).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::HandleGhostCallStatusChange(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    RMobileCall::TMobileCallStatus callStatus( RMobileCall::EStatusIdle );
    RMobileCall::TMobileCallStatus receivedCallStatus( 
        RMobileCall::EStatusIdle );

    aDataPackage->UnPackData( receivedCallStatus );

    TTsyReqHandle mobileStatusHandle = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodeCallNotifyMobileCallStatusChange );

    //client is ready for receiving status change indications
    //or call has entered either idle or active status -> no
    //more status changes are assumed soon.
    if ( mobileStatusHandle ||
        ( receivedCallStatus == RMobileCall::EStatusConnected ) ||
        ( receivedCallStatus == RMobileCall::EStatusIdle ) )
        {
        //this call is not anymore a ghost call
        iGhostCall = EFalse;

        //idle state indicated -> no more indications waited.
        if ( RMobileCall::EStatusIdle == receivedCallStatus )
            {
            //ghost call has terminated, inform client
            CompleteNotifyStatusChange( aResult, aDataPackage );
            }
        //complete one or more simulated status change indications
        else
            {
            if ( RMobileCall::EStatusDisconnecting == receivedCallStatus ||
                RMobileCall::EStatusDisconnectingWithInband == 
                                                          receivedCallStatus )
                {
                CompleteNotifyStatusChange( aResult, aDataPackage );                
                }
            else 
                {
                //Send simulated status indications to the client
                //since it has missed the real ones.
                if ( receivedCallStatus >= RMobileCall::EStatusDialling )
                    {
                    callStatus = RMobileCall::EStatusDialling;
                    aDataPackage->PackData( &callStatus );
                    CompleteNotifyStatusChange( aResult, aDataPackage );
                    }
                if ( receivedCallStatus >= RMobileCall::EStatusConnecting )
                    {
                    callStatus = RMobileCall::EStatusConnecting;
                    aDataPackage->PackData( &callStatus );
                    CompleteNotifyStatusChange( aResult, aDataPackage );
                    }
                if ( receivedCallStatus >= RMobileCall::EStatusConnected )
                    {
                    callStatus = RMobileCall::EStatusConnected;
                    aDataPackage->PackData( &callStatus );
                    CompleteNotifyStatusChange( aResult, aDataPackage );
                    }                
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyDurationChange
// This CORE API method requests notification when call duration changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyDurationChange(
    const TTsyReqHandle aTsyReqHandle,
    TTimeIntervalSeconds* aTime )
    {
    iRetTimeIntervalSeconds = aTime;
    iTsyReqHandleStore->SetTsyReqHandle(
        EMultimodeCallNotifyDurationChange, aTsyReqHandle );

    if ( iCallStatus == RCall::EStatusConnected && iCallTimer != NULL)
        {
        //start duration notifications immediately
        iCallTimer->Start();
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyDurationChangeCancel
// This CORE API method cancels call duration change notifications.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyDurationChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetTimeIntervalSeconds = NULL;

    if ( iCallTimer != NULL )
        {
        //stop notifications
        iCallTimer->Stop();
        }

    iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallNotifyDurationChange );

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteNotifyCallDurationChange
// Complete notification when duration changes. Timer causes calling of
// this completion every second. Type is not defined because this is
// called by iCallTimer.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteNotifyCallDurationChange()
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeCallNotifyDurationChange );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        //get current duration.
        *iRetTimeIntervalSeconds = GetCallDurationInSeconds();
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetStatus
// This CORE API methods returns core call status through the aStatus param.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetStatus(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TStatus* aStatus )
    {
    *aStatus = iCallStatus;
    ReqCompleted( aTsyReqHandle, KErrNone );    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetCaps
// This CORE API method returns Core call capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetCaps(
    const TTsyReqHandle aTsyReqHandle,
    RCall::TCaps* aCaps )
    {
    *aCaps = iCallCaps;
    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::DialCancel
// This CORE API method cancels an outstanding dial request.Calls HangUp 
// method to do this and set a flag so that we can know that DialCancel 
// handling is going on.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::DialCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_DIALCANCEL_1, "TSY: CMmCallTsy::DialCancel. Req handle: %u, Call name: %S", aTsyReqHandle, iCallName);

    TInt ret( KErrGeneral );

    //check if dial cancel has been called
    if ( CMmCallTsy::EDialCancelNotCalled != iDialCancelFlag )
        {       
        ret = KErrServerBusy;
        }
    else
        {
        ret = KErrNone;
        iDialCancelFlag = CMmCallTsy::EDialCancelCallCalled;

        if ( iCallId > 0 || (GetDialFlag()&& iCallStatus!=RMobileCall::EStatusIdle) )
            {
            if ( RMobileCall::EStatusDisconnecting != iMobileCallStatus )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_DIALCANCEL_2, "TSY: CMmCallTsy::DialCancel Hangup called");
            HangUp( aTsyReqHandle );
                }
            else
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_DIALCANCEL_3, "TSY: CMmCallTsy::DialCancel skipped Hangup call");
                }      
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteDial
// Completes a Dial request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteDial(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEDIAL_1, "TSY: CMmCallTsy::CompleteDial. Result:%{TSymbianErrorCodes}", aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEDIAL_2, "TSY: CMmCallTsy::CompleteDial. Call Id:%d Call name:%S", iCallId, iCallName);

	//Set dial flag to false
	SetDialFlag( EFalse );

	TTsyReqHandle reqHandle ( EMultimodeCallReqHandleUnknown );
	
	if ( KETelExt3rdPartyV1 == iExtensionId )
		{
		//3rd Party Client
		//reset req handle. Returns the deleted req handle
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEDIAL_3, "TSY: CMmCallTsy::CompleteDial. EMultimodeCallDialISV");
	    reqHandle =
    	    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallDialISV );
    	
		}
	else
		{
		//reset req handle. Returns the deleted req handle
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEDIAL_4, "TSY: CMmCallTsy::CompleteDial. EMultimodeCallDial");
	    reqHandle =
    	    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallDial );	
		}
   

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
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::AnswerIncomingCallCancel
// TSY has started a request and it is not possible to then cancel this 
// request. The best thing for the TSY to do in this case is to call
// HangUp method to do this and set a flag so that we can know that
// AnswerIncomingCallCancel handling is going on.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::AnswerIncomingCallCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_ANSWERINCOMINGCALLCANCEL_1, "TSY: CMmCallTsy::AnswerIncomingCallCancel. Req handle: %d,  Call name: %S", aTsyReqHandle, iCallName);

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
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteAnswerIncomingCall
// Completes answer request to client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteAnswerIncomingCall(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEANSWERINCOMINGCALL_1, "TSY: CMmCallTsy::CompleteAnswerIncomingCall. Result:%{TSymbianErrorCodes}", aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEANSWERINCOMINGCALL_2, "TSY: CMmCallTsy::CompleteAnswerIncomingCall. Call Id:%d Call name:%S", iCallId, iCallName);

	TTsyReqHandle reqHandle ( EMultimodeCallReqHandleUnknown );
	
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEANSWERINCOMINGCALL_3, "TSY: CMmCallTsy::CompleteAnswerIncomingCall. iExtensionId:%d", iExtensionId );
	
	if ( KETelExt3rdPartyV1 == iExtensionId )
		{
		//3rd Party Client
		//reset req handle. Returns the deleted req handle
	    reqHandle =
    	    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallAnswerISV );	
		}
	else
		{
		//reset req handle. Returns the deleted req handle
	    reqHandle =
    	    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallAnswer );
		}

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
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::HangUpCancel
// CORE API method. TSY has started a request and it is not possible to then 
// cancel this request. The best thing for the TSY to do in this case is to 
// proceed as though the Cancel never happened. The server's call to the TSY 
// cancel function will return synchronously. The TSY then continues to wait 
// for the HangUp() acknowledgemnt and when it receives it, the TSY will 
// complete the original HangUp request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::HangUpCancel(
    const TTsyReqHandle )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteHangUp
// Completes call hangup (both normal and emergency calls). If the cause value 
// is different than the sent one, the call release requset has failed. In 
// this case the causeValue describes the fail cause. Otherwise the call 
// releasing  has succeeded.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteHangUp(
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEHANGUP_1, "TSY: CMmCallTsy::CompleteHangUp.\n  \t\t\t Result:%{TSymbianErrorCodes}", aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEHANGUP_2, "TSY: CMmCallTsy::CompleteHangUp. Call Id:%d Call name:%S", iCallId, iCallName);

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
            }
        }
    // Complete for DialCancel or AnswerIncomingCallCancel, success
    else if ( KErrNone == aResult ) 
        { 
        // Find out if this is cancelling of Dial
        TTsyReqHandle cancelHandle = iTsyReqHandleStore->
            ResetTsyReqHandle( EMultimodeCallDial ); 

        if ( cancelHandle == NULL ) 
            { 
            // Find out if this is cancelling of AnswerIncomingCall
            cancelHandle = iTsyReqHandleStore->
                ResetTsyReqHandle( EMultimodeCallAnswer );

			if( NULL == cancelHandle )
				{
				// Find out if this is cancelling of AnswerIncomingCallISV
            	cancelHandle = iTsyReqHandleStore->
            	    ResetTsyReqHandle( EMultimodeCallAnswerISV );
            	    
                if( NULL == cancelHandle )
                	{
                	// Find out if this is cancelling of DialISV
            		cancelHandle = iTsyReqHandleStore->
            		    ResetTsyReqHandle( EMultimodeCallDialISV );
                                            
                    if ( NULL == cancelHandle )
                        {   
                        // Find out if this is cancelling of DialNoFdnCheck
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
        } 
    // Cancel failed and the request succeeded (connected)
    else if ( RCall::EStatusConnected == iCallStatus ) 
        { 
        // Find out if this is cancelling of Dial
        TTsyReqHandle cancelHandle = iTsyReqHandleStore->
            ResetTsyReqHandle( EMultimodeCallDial ); 

         if ( cancelHandle == NULL ) 
            { 
            // Find out if this is cancelling of AnswerIncomingCall
            cancelHandle = iTsyReqHandleStore->
                ResetTsyReqHandle( EMultimodeCallAnswer );

			if( NULL == cancelHandle )
				{
				// Find out if this is cancelling of AnswerIncomingCallISV
            	cancelHandle = iTsyReqHandleStore->
            	    ResetTsyReqHandle( EMultimodeCallAnswerISV );
            	    
                if( NULL == cancelHandle )
                	{
                	// Find out if this is cancelling of DialISV
            		cancelHandle = iTsyReqHandleStore->
            		    ResetTsyReqHandle( EMultimodeCallDialISV );
                	                                            
                    if ( NULL == cancelHandle )
                        {   
                        // Find out if this is cancelling of DialNoFdnCheck
            		    cancelHandle = iTsyReqHandleStore->
            		        ResetTsyReqHandle( EMultimodeCallDialNoFdnCheck );
                	    }
                	}
				}
            }

        // Cancel to Dial or AnswerIncomingCall
        if ( 0 < cancelHandle ) 
            { 
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
    // Reset HangUp flag
    iHangUpFlag = EFalse;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetOwnershipStatus
// Get call ownership status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetOwnershipStatus(
    const TTsyReqHandle,
    RCall::TOwnershipStatus* )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::TransferOwnership
// Transfers the call ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::TransferOwnership(
    const TTsyReqHandle )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::AcquireOwnership
// Acquire the call ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::AcquireOwnership(
    const TTsyReqHandle )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::AcquireOwnershipCancel
// Cancel the call ownership acquire.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::AcquireOwnershipCancel(
    const TTsyReqHandle )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::RelinquishOwnership
// Relinquish the call ownership.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::RelinquishOwnership()
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetCallParams
// This CORE API method is used for getting Core call parameters.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetCallParams(
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aParams )
    {
    
    if(sizeof(RMobileCall::TMobileCallParamsV1) > aParams->MaxLength())
	  	{
	  	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_GETCALLPARAMS_1, "TSY: CMmCallTsy::GetCallParams bad size argument");
	  	// Complete the request with appropiate error        
	  	return KErrArgument;
	  	}
    
    RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 = 
        reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>( aParams );
    RMobileCall::TMobileCallParamsV1& paramsV1 = ( *paramsPckgV1 )(); 

    paramsV1.iSpeakerControl = iCallParams.iSpeakerControl; 
    paramsV1.iSpeakerVolume = iCallParams.iSpeakerVolume; 
    paramsV1.iInterval = iCallParams.iInterval; 
    paramsV1.iWaitForDialTone = iCallParams.iWaitForDialTone; 

    //let extension to update mode specific params as well
    iMmCallExtInterface->GetCallParams( aParams ); 
        
    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetCallDuration
// This CORE API method is used for getting call duration. Calls 
// GetCallDurationInSeconds() method to return the current call duration.
// See details from there.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetCallDuration(
    const TTsyReqHandle aTsyReqHandle,
    TTimeIntervalSeconds* aTime )
    {
    //get duration
    *aTime = GetCallDurationInSeconds();
    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CallId
// Returns call ID.of this call
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::CallId() const
    {
    return iCallId;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::SetCallId
// Sets call ID.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::SetCallId(
    TInt aCallId )
    {
    TInt ret( KErrGeneral );

    if ( aCallId <= KMaxCallIdValue )
        {
        iCallId = aCallId;
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::SetCallStatus
// Sets call Status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::SetCallStatus(
    RCall::TStatus aCallStatus,
    RMobileCall::TMobileCallStatus aMobileCallStatus )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_SETCALLSTATUS_1,  "TSY: CMmCallTsy::SetCallStatus: iMobileCallStatus = %d, iCallStatus = %d", aMobileCallStatus, aCallStatus );
    iCallStatus = aCallStatus;
    iMobileCallStatus = aMobileCallStatus;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::SetPreviousCallId
// Sets previous active call ID. The Call ID is set when the call goes to idle 
// state. Before the previous call ID is set, all other calls are checked and 
// if they have the same previous call ID, their call id will be cleared. This 
// is required by Conference call implementation.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::SetPreviousCallId(
    TInt aCurrentId )
    {
    if ( aCurrentId > 0 )
        {
        CMmCallTsy* mmCall = NULL;

        for ( TInt i = 0; i < iMmPhone->CallList()->
                  GetNumberOfObjects(); i++ )
            {
            mmCall = iMmPhone->CallList()->GetMmCallByIndex( i );

            if ( mmCall->PreviousCallId() == aCurrentId )
                {
                mmCall->SetPreviousCallId( -1 );
                }
            }
        }
    iPreviousCallId = aCurrentId;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::PreviousCallId
// Returns previous active call ID. The Call ID is reset when the call goes to 
// idle state. This is required by Conference call implementation.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::PreviousCallId() const
    {
    return iPreviousCallId;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::ResetPreviousCallId
// Resets previous active call ID. The Call ID is reset when the call goes to 
// idle state. This is required by Conference call implementation.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::ResetPreviousCallId()
    {
    iPreviousCallId = -1;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CallMode
// Returns call mode.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RMobilePhone::TMobileService CMmCallTsy::CallMode() const
    {
    return iCallMode;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CallDirection
// Returns call direction.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RMobileCall::TMobileCallDirection CMmCallTsy::CallDirection() const
    { 
    return iCallDirection; 
    } 

// ---------------------------------------------------------------------------
// CMmCallTsy::SetCallDirection
// Sets call direction.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::SetCallDirection(
    RMobileCall::TMobileCallDirection aCallDirection )
    {
    iCallDirection = aCallDirection;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CallName
// Returns call name.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
const TDesC& CMmCallTsy::CallName() const
    {
    return iCallName;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::SetCallName
// Sets call name.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::SetCallName(
    const TName& aCallName )
    {
    iCallName = aCallName;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::SetCallCaps
// Sets call capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::SetCallCaps(
    TUint32 aCallCaps )
    {
    iCallCaps.iFlags = aCallCaps;
    CompleteNotifyCapsChange();
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CallCaps
// Returns call capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUint32 CMmCallTsy::CallCaps() const
    {
    //get call caps
    TUint32 aCallCaps = iCallCaps.iFlags;
    //return them
    return aCallCaps;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetMobileCallCaps
// This function returns the call control and call event dynamic capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetMobileCallCaps(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aCaps )
    {
    TInt ret( KErrArgument );
    
    if ( sizeof ( RMobileCall::TMobileCallCapsV1 ) <= aCaps->MaxLength() )
        {
        RMobileCall::TMobileCallCapsV1Pckg* callCapsV1Pckg = 
            reinterpret_cast<RMobileCall::TMobileCallCapsV1Pckg*>( aCaps );
        RMobileCall::TMobileCallCapsV1& callCapsV1 = 
            ( *callCapsV1Pckg )();

        if ( KETelExtMultimodeV1 == callCapsV1.ExtensionId() )
            {   
            ret = KErrNone;
            iMmCallExtInterface->GetMobileCallCapsV1( callCapsV1 );
            }
        }
        
    ReqCompleted( aTsyReqHandle, ret );
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyMobileCallCapsChange
// Notifies the change of Mobile Call Capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyMobileCallCapsChange(
    TDes8* aCaps )
    {
    TInt ret( KErrArgument ); 
    
    if ( sizeof ( RMobileCall::TMobileCallCapsV1 ) <= aCaps->MaxLength() )
        {
        RMobileCall::TMobileCallCapsV1Pckg* callCapsV1Pckg = 
            reinterpret_cast<RMobileCall::TMobileCallCapsV1Pckg*>( aCaps );
        RMobileCall::TMobileCallCapsV1& callCapsV1 = 
            ( *callCapsV1Pckg )();  
     
        if ( KETelExtMultimodeV1 == callCapsV1.ExtensionId() )
            {    
            ret = KErrNone;
            iRetNotifyMobileCallCaps = aCaps;
            iReqHandleType = EMultimodeCallNotifyMobileCallCapsChange;
            }
        }
    return ret;

    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyMobileCallCapsChangeCancel
// Cancels an outstanding asynchronous NotifyMobileCallCapsChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyMobileCallCapsChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    //reset pointer
    iRetNotifyMobileCallCaps = NULL; 
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeCallNotifyMobileCallCapsChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteNotifyMobileCallCapsChange
// Completes a NotifyMobileCallCapsChange method. The caps request is directed 
// to the currently active protocol extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteNotifyMobileCallCapsChange(
    TInt aErrorCode )
    {
      //get req handle. 
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
                                EMultimodeCallNotifyMobileCallCapsChange );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        RMobileCall::TMobileCallCapsV1Pckg* callCapsV1Pckg = 
            reinterpret_cast<RMobileCall::TMobileCallCapsV1Pckg*>( 
                iRetNotifyMobileCallCaps );
        RMobileCall::TMobileCallCapsV1& callCapsV1 = 
            ( *callCapsV1Pckg )();
                        
        iMmCallExtInterface->GetMobileCallCapsV1( callCapsV1 );
        
        if (callCapsV1.iCallControlCaps != iPreviousCallControlCaps )
            {           
            //reset req handle. 
            iTsyReqHandleStore->ResetTsyReqHandle( 
                EMultimodeCallNotifyMobileCallCapsChange );
                
            iPreviousCallControlCaps = callCapsV1.iCallControlCaps;
            iRetNotifyMobileCallCaps = NULL;
            ReqCompleted( reqHandle, aErrorCode );  
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteNotifyMobileCallCapsChange
// Returns a pointer to the active extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmCallExtInterface* CMmCallTsy::ActiveCallExtension()
    {
    return iMmCallExtInterface;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::ClearCallStatus
// Clears internal call status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::ClearCallStatus()
    {
    iCallDirection = RMobileCall::EDirectionUnknown;
    iEtelOriginated = ETrue;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::Status
// Returns the current core call status. Can be called from TSY classes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RCall::TStatus CMmCallTsy::Status() const
    {
    return iCallStatus;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::MobileCallStatus
// Returns the current mobile call status. Can be called from TSY classes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RMobileCall::TMobileCallStatus CMmCallTsy::MobileCallStatus() const
    {
    return iMobileCallStatus;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetMobileCallStatus
// Returns the current status of the call through the aStatus argument.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetMobileCallStatus(
    const TTsyReqHandle aTsyReqHandle,
    RMobileCall::TMobileCallStatus* aStatus )
    {
    *aStatus = iMobileCallStatus;
    ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyMobileCallStatusChange
// Allows a client to be notified when the call changes state. The request 
// completes when the call changes state, the new state being passed in the 
// aStatus parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyMobileCallStatusChange(
    RMobileCall::TMobileCallStatus* aStatus )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_NOTIFYMOBILECALLSTATUSCHANGE_1,  "TSY: CMmCallTsy::NotifyMobileCallStatusChange. iMobileCallStatus new = %d, old = %d", (TInt)aStatus, (TInt)iMobileCallStatus );
    iRetMobileCallStatus = aStatus;
    iReqHandleType = EMultimodeCallNotifyMobileCallStatusChange;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyMobileCallStatusChangeCancel
// Cancels an outstanding asynchronous NotifyMobileCallStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyMobileCallStatusChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetMobileCallStatus = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeCallNotifyMobileCallStatusChange );

    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyCallEvent
// Allows clients to be notified when various call events occur. These events 
// may be generated locally (i.e. from within the ME) or remotely (by the 
// remote connected party). The request completes when an event occurs. Upon 
// completion, the aEvent parameter will return the event type.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyCallEvent(
    RMobileCall::TMobileCallEvent* aEvent )
    {
    iRetCallEvent = aEvent;
    iReqHandleType = EMultimodeCallNotifyCallEvent;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyCallEventCancel
// This method cancels an outstanding asynchronous NotifyCallEvent request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyCallEventCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetCallEvent = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallNotifyCallEvent );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteNotifyCallEvent
// Allows clients to be notified when various call events occur.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteNotifyCallEvent(
    RMobileCall::TMobileCallEvent aEvent )
    {    
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallNotifyCallEvent );

    if ( EMultimodeCallReqHandleUnknown != reqHandle &&
        iRetCallEvent != NULL )
        {
		if( IsPartOfConference() && RMobileCall::ERemoteTerminated == aEvent )
			{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETENOTIFYCALLEVENT_1, "TSY: iIsRemoteReleasedCall = ETrue");
			iIsRemoteReleasedCall = ETrue;
			}
        *iRetCallEvent = aEvent;
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteNotifyRemoteCallEvent
// Informs common TSY about a remote event (that cannot be understood from
// status indication.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteNotifyRemoteCallEvent(
    const CMmDataPackage* aDataPackage )
    {    
    RMobileCall::TMobileCallEvent callEvent( RMobileCall::ERemoteHold );
    aDataPackage->UnPackData( callEvent );

    switch ( callEvent )
        {
        case RMobileCall::ERemoteHold:
        case RMobileCall::ERemoteResume:
        case RMobileCall::ERemoteTerminated:
        case RMobileCall::ERemoteConferenceCreate:
            CompleteNotifyCallEvent( callEvent );
            break;
        //local events handled by CMmCallTsy::CompleteNotifyStatusChange 
        case RMobileCall::ELocalHold:
        case RMobileCall::ELocalResume:
        case RMobileCall::ELocalDeflectCall:
        case RMobileCall::ELocalTransfer:
        //not supported cases
        case RMobileCall::ERemoteTransferring:
        case RMobileCall::ERemoteTransferAlerting:
		case RMobileCall::ERemoteBarred:
		case RMobileCall::ERemoteForwarding:
		case RMobileCall::ERemoteWaiting:
		case RMobileCall::ELocalBarred:
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetMobileCallInfo
// The method returns a current snapshot of the call information
// described in the TMobileCallInfoVx class. The class is packaged into
// a TMobileCallInfoVxPckg before being passed to the GetMobileCallInfo method.
// This method actually calls FillMobileCallInfo method to get mobile call 
// information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetMobileCallInfo(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aInfo )
    {
    //get the mobile call information
    TInt ret = FillMobileCallInfo( aInfo );

    ReqCompleted( aTsyReqHandle, ret );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::FillMobileCallInfo
// The method fills a current snapshot of the call information described in 
// the TMobileCallInfoVx class to the aInfo. The mode specific fields will
// be filled by the extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::FillMobileCallInfo(
    TDes8* aInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_FILLMOBILECALLINFO_1, "TSY: CMmCallTsy::FillMobileCallInfo");

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
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_FILLMOBILECALLINFO_2, "TSY: CMmCallTsy::FillMobileCallInfo - extensionid=%d",callInfo.ExtensionId());
        }
 
    //TMobileCallInfoV1
    if ( KETelExtMultimodeV1 == extensionId ||
    	 KETelExtMultimodeV3 == extensionId ||
    	 KEtelExtMultimodeV7 == extensionId ||
    	 KEtelExtMultimodeV8 == extensionId )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_FILLMOBILECALLINFO_3, "TSY: CMmCallTsy::FillMobileCallInfo - V1");

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
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_FILLMOBILECALLINFO_4, "TSY: CMmCallTsy::FillMobileCallInfo - V3");
            
        RMobileCall::TMobileCallInfoV3Pckg* paramsPckgV3 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV3Pckg*>( aInfo );
        RMobileCall::TMobileCallInfoV3& info = ( *paramsPckgV3 )();        
            
        // fill specific data
        iMmCallExtInterface->FillMobileCallInfoV3( &info ) ;
        }

    // TMobileCallInfoV7
    if ( KEtelExtMultimodeV7 == extensionId ||
    	 KEtelExtMultimodeV8 == extensionId )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_FILLMOBILECALLINFO_5, "TSY: CMmCallTsy::FillMobileCallInfo - V7");
            
        RMobileCall::TMobileCallInfoV7Pckg* paramsPckgV7 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV7Pckg*>( aInfo );
        RMobileCall::TMobileCallInfoV7& info = ( *paramsPckgV7 )();
            
        // fill specific data
        iMmCallExtInterface->FillMobileCallInfoV7( &info );
        }
    
    // TMobileCallInfoV8
    if ( KEtelExtMultimodeV8 == extensionId )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_FILLMOBILECALLINFO_6, "TSY: CMmCallTsy::FillMobileCallInfo - V8");
                
        RMobileCall::TMobileCallInfoV8Pckg* paramsPckgV8 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV8Pckg*>( aInfo );
        RMobileCall::TMobileCallInfoV8& info = ( *paramsPckgV8 )();
                
        // fill specific data
        iMmCallExtInterface->FillMobileCallInfoV8( &info );
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_FILLMOBILECALLINFO_7, "TSY: CMmCallTsy::FillMobileCallInfo end. ret=%{TSymbianErrorCodes}",ret);
      
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyRemotePartyInfoChange
// This method allows the client to be notified of any change in the remote 
// party information. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyRemotePartyInfoChange(
    TDes8* aRemotePartyInfo )
    {
    TInt ret = KErrNone;
    
    if( sizeof( RMobileCall::TMobileCallRemotePartyInfoV1 ) > aRemotePartyInfo->MaxLength() )
    	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_NOTIFYREMOTEPARTYINFOCHANGE_1, "TSY: CMmCallTsy::NotifyRemotePartyInfoChange Bad size argument");
        
        // Complete the request with appropiate error        
        ret = KErrArgument;
    	}
    else
        {
        iRetRemotePartyInfo = aRemotePartyInfo;
        iReqHandleType = EMultimodeCallNotifyRemotePartyInfoChange;
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyRemotePartyInfoChangeCancel
// This method cancels an outstanding asynchronous NotifyRemotePartyInfoChange 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyRemotePartyInfoChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetRemotePartyInfo = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeCallNotifyRemotePartyInfoChange );

    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteNotifyRemotePartyInfoChange
// Completes a NotifyRemotePartyInfoChange method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteNotifyRemotePartyInfoChange()
    {     
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
                                EMultimodeCallNotifyRemotePartyInfoChange );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        iMmCallExtInterface->GetRemotePartyInfo( 
            reinterpret_cast<RMobileCall::TMobileCallRemotePartyInfoV1Pckg*> 
            ( iRetRemotePartyInfo ) );
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteNotifyMobileCallInfoChange
// Notifies common TSY about mobile call information change.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteNotifyMobileCallInfoChange(
    CMmDataPackage* aDataPackage )
    {
    //set data to gsm extension
    //( direction added because not uppdated in messagehandler)
    iMmCallExtInterface->SetMobileCallData(
        aDataPackage, iCallDirection );
    //call completenotify remotepartyinfochange
    CompleteNotifyRemotePartyInfoChange();
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::TransferL
// This method allows a client to transfer a call so that the remote party of 
// one call ends up connected to the remote party of another call while this 
// user drops out of both calls. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::TransferL(
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt ret( KErrServerBusy );

    TTsyReqHandle transferHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeCallTransfer );

    if ( !transferHandle )
        {
        ret = iMmCallExtInterface->TransferL();

        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            //save tsy req handle type
            iReqHandleType = EMultimodeCallTransfer;
            }
        }
    else
        {
        ReqCompleted( aTsyReqHandle, ret );
        }      
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteTransfer
// This method completes an outstanding asynchronous Transfer request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteTransfer(
    TInt aErrorCode )
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallTransfer );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aErrorCode );
        }    
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyPrivacyConfirmation
// This method allows the client to be notified when the network has confirmed 
// the privacy setting.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyPrivacyConfirmation(
    RMobilePhone::TMobilePhonePrivacy* aPrivacyStatus )
    {
    iRetPrivacyStatus = aPrivacyStatus;
    iReqHandleType = EMultimodeCallNotifyPrivacyConfirmation;    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::NotifyPrivacyConfirmationCancel
// Cancels an outstanding asynchronous NotifyPrivacyConfirmation request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::NotifyPrivacyConfirmationCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetPrivacyStatus = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeCallNotifyPrivacyConfirmation );

    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteNotifyPrivacyConfirmation
// Completes a NotifyPrivacyConfirmation request. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteNotifyPrivacyConfirmation(
    RMobilePhone::TMobilePhonePrivacy aStatus )
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeCallNotifyPrivacyConfirmation );

    if ( EMultimodeCallReqHandleUnknown != reqHandle ) 
        {
        *iRetPrivacyStatus = aStatus;
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::Line
// Return the line from which this call object has been opened from.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmLineTsy* CMmCallTsy::Line()
    {
    //Return the line from which this call object has been opened from.
    return iMmLine;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::Phone
// This method returns pointer to the Phone object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneTsy* CMmCallTsy::Phone()
    {
    //This method returns pointer to the Phone object.
    return iMmPhone;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetCallDurationInSeconds
// Returns call duration in seconds. If call is not active, the duration of 
// the last successful call will be returned.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TTimeIntervalSeconds CMmCallTsy::GetCallDurationInSeconds()
    {
    // If the call is active, NumberOfBeats will return the current duration
    // If the call is not active, NumberOfBeats will return the last 
    // call duration
    TTimeIntervalSeconds duration ( iCallTimer->NumberOfBeats() );
    return duration;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::IsServiceLocallyRequested
// Return ETrue if this type of request is requested by any of TSY's client. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCallTsy::IsServiceLocallyRequested(
    CMmCallTsy::TCallRequestTypes aReqType )
    {
    //Return ETrue if this type of request is requested by any TSY's client.
    //create parameter callCount and get number of objects from call list to 
    //it
    TInt callCount = iMmPhone->CallList()->GetNumberOfObjects();
    CMmCallTsy* mmCall = NULL;
    TBool ret( EFalse);

    for ( TInt i = 0; i < callCount; i++ )
        {
        mmCall = reinterpret_cast<CMmCallTsy*>(
            iMmPhone->CallList()->GetObjectByIndex(i));

        if( mmCall->ServiceRequested( aReqType ) )
            {
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetGhostCall
// Return ETrue if this call is a Ghost Call
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCallTsy::GetGhostCall() const
    {    
    return iGhostCall;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetDiagnostics
// Returns value of iDiagnosticOctect internal attribute
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUint8 CMmCallTsy::GetDiagnostics() const
    {
    return iDiagnosticOctet;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::SetDiagnostics
// Sets aDiagnosticOctet into internal variable iDiagnosticOctet
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::SetDiagnostics( 
    TUint8 aDiagnosticOctet )
    {
    iDiagnosticOctet = aDiagnosticOctet;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetCallReqHandleStore
// Returns pointer to iTsyReqHandleStore
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
CMmTsyReqHandleStore* CMmCallTsy::GetCallReqHandleStore()
    {
    return iTsyReqHandleStore;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::SetUnownedCallObjectFlag
// TSY creates automatically call object for incoming call or ghost call that 
// is created using AT-commands. This method sets flag indicating if a client 
// has't opened existing call object for this call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CMmCallTsy::SetUnownedCallObjectFlag(
    TBool aIsUnownedCallObject )
    {   
    iUnownedCallObject = aIsUnownedCallObject;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::IsUnownedCallObject
// TSY creates automatically call object for incoming call or ghost call that 
// is created using AT-commands. This flag indicates that client haven't 
// opened existing call object for this call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCallTsy::IsUnownedCallObject() const
    {  
    return iUnownedCallObject;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::SetGhostCall
// Sets a flag that indicates that this call is not created through ETel
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::SetGhostCall(
    TBool aValue )
    {
    //Sets a flag that indicates that this call is not created through ETel
    iGhostCall = aValue;

    if ( aValue )
        {
        iEtelOriginated = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::ETelOriginated
// Returns ETrue is this call has been originated through ETel API
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCallTsy::ETelOriginated() const
    {    
    return iEtelOriginated;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::ServiceRequested
// Returns ETrue if this service is pending on this call object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCallTsy::ServiceRequested(
    CMmCallTsy::TCallRequestTypes aReqType )
    {
    TBool ret( EFalse );

    if( CMmCallTsy::EMultimodeCallReqHandleUnknown != 
        iTsyReqHandleStore->GetTsyReqHandle( aReqType ) )
        {
        //Returns ETrue if this service is pending on this call object
        ret = ETrue;
        }
    return ret;
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmCallTsy::Complete
// Completes the request due timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::Complete(
    TInt aReqHandleType,
    TInt aError )
    {
    //All possible TSY req handle types are listed in the
    //switch case below. 
    switch( aReqHandleType )
        {
        //cases handled with automatic completion
        case EMultimodeCallDial:
        case EMultimodeCallDialISV:
            CompleteDial( aError );
            break;
        case EMultimodeCallDialNoFdnCheck:
            CompleteDialNoFdn( aError );
            break;
        case EMultimodeCallAnswer:
        case EMultimodeCallAnswerISV:
            CompleteAnswerIncomingCall( aError );
            break;
        case EMultimodeCallHangUp:
            CompleteHangUp( aError );
            break;
        case EMultimodeCallTransfer: 
            CompleteTransfer( aError );
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
            ReqCompleted(
                iTsyReqHandleStore->ResetTsyReqHandle( aReqHandleType ),
                aError );
            break;
        }
    }
#endif


#ifdef TF_LOGGING_ENABLED
// ---------------------------------------------------------------------------
// CMmCallTsy::ReqCompleted
// Overloads CTelObject::ReqCompleted for logging purposes. It prints the 
// aTsyReqHandle and aError variable in the log file and then calls 
// CTelObject::ReqCompleted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::ReqCompleted(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aError )
    {
    //Overloads CTelObject::ReqCompleted for logging purposes. It 
    //prints the aTsyReqHandle and aError variable in the log file and then 
    //calls CTelObject::ReqCompleted.
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_REQCOMPLETED_1, "TSY: CMmCallTsy::ReqCompleted. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", aTsyReqHandle, aError, this);

    CTelObject::ReqCompleted(aTsyReqHandle,aError);

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_REQCOMPLETED_2, "<-- TSY: CMmCallTsy::ReqCompleted");
    }
#endif

// ---------------------------------------------------------------------------
// CMmCallTsy::IsPartOfConference
// Returns ETrue if this call is part of a conference
// ---------------------------------------------------------------------------
//
TBool CMmCallTsy::IsPartOfConference() const
	{
	return iIsPartOfConference;
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::SetPartOfConference
// Sets the flag indicating that this call is part of a MO conference
// ---------------------------------------------------------------------------
//
void CMmCallTsy::SetPartOfConference( TBool aPartOfConference )
{
	iIsPartOfConference = aPartOfConference;

#ifdef TF_LOGGING_METHOD
	if ( iIsPartOfConference )
		{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_SETPARTOFCONFERENCE_1,  "TSY: CMmCallTsy::SetPartOfConference: Call %d has joined the conference", iCallId );
		}
	else
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_SETPARTOFCONFERENCE_2,  "TSY: CMmCallTsy::SetPartOfConference: Call has left the conference" );
		}
#endif // TF_LOGGING_METHOD

}
// ---------------------------------------------------------------------------
// CMmCallTsy::IsRequestPossibleInOffLine
// Checks wether a ETel request can be performed or not while offline mode is
// enabled
// ---------------------------------------------------------------------------
//
TBool CMmCallTsy::IsRequestPossibleInOffline( TInt aIpc ) const
    {
    // we assume here that most of the requests are possible, thefore
    // we switch for the not possible ones, optimizing a bit the checking.
    TBool isRequestPossible ( ETrue );

    switch ( aIpc )
        {
        case EMobileCallDialEmergencyCall://OK (when flag is enabled)
        	{
        	isRequestPossible = EFalse;        	
        	RFeatureControl featureControl;
        	TInt err = featureControl.Open();
        	if ( (err == KErrNone) && (featureControl.FeatureSupported(NFeature::KEmergencyCallsEnabledInOfflineMode) == KFeatureSupported) )
                {
                isRequestPossible = ETrue;
                }
            featureControl.Close();    
        	}
            break;
        //case EMobileCallGetMobileCallStatus: //OK
        //case EMobileCallNotifyMobileCallStatusChange://OK
        case EMobileCallHold://NO
        case EMobileCallResume://NO
        case EMobileCallSwap://NO
        case EMobileCallTransfer://NO
        case EMobileCallGoOneToOne://NO
        //case EMobileCallNotifyCallEvent://OK
        //case EMobileCallGetMobileCallCaps: //OK
        //case EMobileCallNotifyMobileCallCapsChange: //OK
        //case EMobileCallGetMobileCallInfo://OK
        //case EMobileCallNotifyRemotePartyInfoChange://OK
        //case EMobileCallNotifyPrivacyConfirmation://OK
        //case EMobileCallGetMobileDataCallCaps: //OK
        //case EMobileCallNotifyMobileDataCallCapsChange: //OK
        //case EMobileCallGetMobileDataCallRLPRange://OK
        //case EMobileCallSetDynamicHscsdParams://OK
        //case EMobileCallGetCurrentHscsdInfo://OK
        //case EMobileCallNotifyHscsdInfoChange://OK
            isRequestPossible = EFalse;
            break;
        default:
            break;
        }
        
    return isRequestPossible;

    }

// ---------------------------------------------------------------------------
// CMmCallTsy::GetFaxSettings
// This CORE API method is used for getting fax settings
// (not supported by default).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetFaxSettings(
    const TTsyReqHandle,
    RCall::TFaxSessionSettings* )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmCallTsy::SetFaxSettings
// This CORE API method is used for set fax settings
// (not supported by default).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::SetFaxSettings(
    const TTsyReqHandle,
    const RCall::TFaxSessionSettings*)
    {
    return KErrNotSupported;
    }
    

// ---------------------------------------------------------------------------
// CMmCallTsy::SetExtensionId
// Set iExtensionId attribute
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::SetExtensionId(
	TInt aExtensionId )
	{
	iExtensionId = aExtensionId;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_SETEXTENSIONID_1, "TSY: CMmCallTsy::SetExtensionId.iExtensionId:%d",iExtensionId );
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::GetExtensionId
// Get iExtensionId attribute
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::GetExtensionId()
	{
	return iExtensionId; 
	}
	
// ---------------------------------------------------------------------------
// CMmCallTsy::SetDialFlag
// Sets iDialFlag boolean value
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//	
void CMmCallTsy::SetDialFlag(
	TBool aDialFlag )
	{	
	iDialFlag= aDialFlag;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_SETDIALFLAG_1, "TSY: CMmCallTsy::SetDialFlag. iDialFlag:%d",iDialFlag );
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::GetDialFlag
// Returns iDialFlag boolean value
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCallTsy::GetDialFlag()
	{	
	
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_GETDIALFLAG_1, "TSY: CMmCallTsy::GetDialFlag. iDialFlag:%d",iDialFlag );
	return iDialFlag;	
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::IsRemoteReleasedCall
// Returns iIsRemoteReleasedCall boolean value
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCallTsy::IsRemoteReleasedCall() const
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_ISREMOTERELEASEDCALL_1, "TSY: CMmCallTsy::IsRemoteReleasedCall()");
	return iIsRemoteReleasedCall;
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::ActivateUUS
// Specifies which User-To-User Signalling service(s) should be activated.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::ActivateUUS( const TTsyReqHandle aTsyReqHandle,
    RMobileCall::TMobileCallUUSRequestV1Pckg* aUUSRequest )
	{
    // Support only implicit-UUS1 service,
    // save the UUS-Request and send it during the Dial so that
    // UUS-Request can be mapped on LTSY side to correct phone call (with id).
    // details of the message send, see CMmVoiceCallTsy::Dial 
    
    // To support UUS2 and UUS3 services in future the logic must be
    // changed so that the UUS information can be sended to LTSY immediately.
    
    // Unpack UUS parameters
	RMobileCall::TMobileCallUUSRequestV1 requestUUS = ( *aUUSRequest )();	
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_ACTIVATEUUS_1, "TSY: CMmCallTsy::ActivateUUS() -ServiceReq:%u  -UUI:%S", requestUUS.iServiceReq, requestUUS.iUUI );
    // handle implicit UUS1 request only
    if ( requestUUS.iServiceReq & RMobileCall::KUUS1Implicit )
        {
        // take a copy of the UUS request
        iUUSRequest = requestUUS; 
        // complete req, with KErrNone
        ReqCompleted( aTsyReqHandle, KErrNone );
        }
    else
        {
        // other UUS services are not supported in this phase
        ReqCompleted( aTsyReqHandle, KErrNotSupported );
        }
        
	return KErrNone;
	}	
	
// ---------------------------------------------------------------------------
// CMmCallTsy::ReceiveUUI
// Enables the client to receive the next incoming UUI message or in case 
// already received, return the buffered UUI.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::ReceiveUUI( const TTsyReqHandle aTsyReqHandle,
    RMobileCall::TMobileCallUUI* aUUI )
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_RECEIVEUUI_1, "TSY: CMmCallTsy::ReceiveUUI()" );
    
    // check are there any UUI information already in the buffer (UUS1)
    if ( 0 < iReceivedUUI.Length() )
        {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_RECEIVEUUI_2, "TSY: CMmCallTsy::ReceiveUUI() - information in BUFFER:%S", iReceivedUUI );
        // copy and empty the buffered information
        *aUUI = iReceivedUUI;
        iReceivedUUI.Zero(); 
        
        // complete the request
        ReqCompleted( aTsyReqHandle, KErrNone );        
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_RECEIVEUUI_3, "TSY: CMmCallTsy::ReceiveUUI() - wait for next incoming UUI message" );
        // Save request handle and pointer of the UUI message. 
        iRetUUI = aUUI;
        iTsyReqHandleStore->SetTsyReqHandle(
            EMultimodeCallReceiveUUI, aTsyReqHandle );
        }
         
    return KErrNone;
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::ReceiveUUICancel
// Use this method to cancel a previously placed asynchronous 
// EMobileCallReceiveUUI request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::ReceiveUUICancel( const TTsyReqHandle aTsyReqHandle )
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_RECEIVEUUICANCEL_1, "TSY: CMmCallTsy::ReceiveUUICancel()");
    
    // reset the client side pointer, empty buffer and cancel the handle
    iRetUUI = NULL;
    iReceivedUUI.Zero(); 
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallReceiveUUI );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteReceiveUUI
// This method completes an outstanding ReceiveUUI request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::CompleteReceiveUUI( CMmDataPackage* aDataPackage, 
    TInt aResult )
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETERECEIVEUUI_1, "TSY: CMmCallTsy::CompleteReceiveUUI()" );

    RMobileCall::TMobileCallUUI* receiveUUI = NULL; 
    aDataPackage->UnPackData( &receiveUUI );
    
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETERECEIVEUUI_2, "TSY: CMmCallTsy::CompleteReceiveUUI() - receiveUUI:%S", *receiveUUI );
    
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeCallReceiveUUI );

    if ( EMultimodeCallReqHandleUnknown != reqHandle )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETERECEIVEUUI_3, "TSY: CMmCallTsy::CompleteReceiveUUI() - Handle found, complete" );
        // Update the client side data through pointer        
        *iRetUUI = *receiveUUI;
        // Null client side pointer, data copied
        iRetUUI = NULL;
        // complete req
        ReqCompleted( reqHandle, aResult );
        }
    else if ( KErrNone == aResult )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETERECEIVEUUI_4, "TSY: CMmCallTsy::CompleteReceiveUUI() - Handle NOT found, buffer" );
        // set initial information to the buffer
        iReceivedUUI = *receiveUUI; 
        }
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::SetDialTypeId
// Set iDialTypeId attribute
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallTsy::SetDialTypeId(
	TUint8 aDialType )
	{
	iDialTypeId = aDialType;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_SETDIALTYPEID_1, "TSY: CMmCallTsy::SetDialTypeId.iDialType:%d",aDialType );
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::GetDialTypeId
// Returns iDialTypeId value
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUint8 CMmCallTsy::GetDialTypeId()
	{	
	
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_GETDIALTYPEID_1, "TSY: CMmCallTsy::GetDialType. iDialType:%d",iDialTypeId );
	return iDialTypeId;	
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::CompleteDialNoFdn
// Completes a Dial request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------	
//
void CMmCallTsy::CompleteDialNoFdn( 
    TInt aResult )
	{	
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEDIALNOFDN_1, "TSY: CMmCallTsy::CompleteDialNoFdn. Result:%{TSymbianErrorCodes}", aResult );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_COMPLETEDIALNOFDN_2, "TSY: CMmCallTsy::CompleteDialNoFdn. Call Id:%d Call name:%S", iCallId, iCallName);

	//Set dial flag to false
	SetDialFlag( EFalse );

	TTsyReqHandle reqHandle ( EMultimodeCallReqHandleUnknown );	
	
	// Reset req handle. Returns the deleted req handle
	reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
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
        //Complete request
        ReqCompleted( reqHandle, aResult );
        }
        	
	//Set dial type to none 
	SetDialTypeId( KMultimodeCallTypeDialNone );	
	}

// ---------------------------------------------------------------------------
// CMmCallTsy::UpdateLifeTimer
// When the last call from conference call becomes idle, we are updating
// life time duration param with remaining call duration
// ---------------------------------------------------------------------------	
//     
void CMmCallTsy::UpdateLifeTimer()
	{
    if ( iMmPhone->NumberOfActiveCalls() == 0 )
    	{                    	                                        
        TTimeIntervalSeconds tis;
		iMmPhone->GetAirTimeDuration(tis);
		TUint32 duration;
		duration = tis.Int();
		duration = duration % iMmPhone->KLifeTimeUpdateInterval;

		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_UPDATELIFETIMER_1, "TSY: CMmCallTsy::UpdateLifeTimer - \duration: %d", duration);
				
		//errors: trap and ignore
        CMmDataPackage dataPackage;
        dataPackage.PackData( &duration );
		
#ifdef USING_CTSY_DISPATCHER	
    TRAP_IGNORE(iMessageManager->HandleRequestL( 
    				ECtsyUpdateLifeTimeReq, &dataPackage );
        ); 
#else	
    TRAP_IGNORE(iMessageManager->HandleRequestL( 
    				EMmTsyUpdateLifeTimeIPC, &dataPackage );
        ); 
#endif //USING_CTSY_DISPATCHER 
    	}	
	}
// CMmCallTsy::FillMobileCallInfoDefaults
// The method fills a current snapshot of the call information described in 
// the TMobileCallInfoV1 class to the aInfo. The mode specific fields will
// be filled by the extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallTsy::FillMobileCallInfoDefaults(
    RMobileCall::TMobileCallInfoV1* aInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_FILLMOBILECALLINFODEFAULTS_1, "TSY: CMmCallTsy::FillMobileCallInfoDefaults");

    //set validity fields to 0 in the beginning.
    aInfo->iValid = 0;
    //The name of the call
    aInfo->iCallName.Copy( iCallName );
    //The name of the line on which the call is open
    aInfo->iLineName.Copy( iMmLine->LineName() );
    //The current status of the call
    aInfo->iStatus = iMobileCallStatus;

    //The time & date the call started.
    TDateTime temp( iCallTimer->GetTimerStartTime()->DateTime() );
    aInfo->iStartTime.Set(
        temp.Year(), temp.Month(), temp.Day(),
        temp.Hour(), temp.Minute(), temp.Second(), temp.MicroSecond() );

    //The current duration of the call
    aInfo->iDuration = GetCallDurationInSeconds();

    //Set validity indication
    aInfo->iValid |= RMobileCall::KCallStartTime | RMobileCall::KCallDuration;

    //Client call Id. Note! Call server uses different call Id.
    aInfo->iCallId = iCallId;
    aInfo->iValid |= RMobileCall::KCallId; 

    //The reason for termination of a finished call. Will equal KErrNone if
    //the call ended normally and KErrNotFound if the call has not ended.
    aInfo->iExitCode = iLastExitCode;
    aInfo->iValid |= RMobileCall::KCallExitCode;

    //ETrue if the call is an emergency call or EFalse if it is not.
    aInfo->iEmergency = iEmergencyCall;
    aInfo->iValid |= RMobileCall::KCallEmergency;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLTSY_FILLMOBILECALLINFODEFAULTS_2, "TSY: CMmCallTsy::FillMobileCallInfoDefaults end");
    return KErrNone;
    }
    
//  End of File  
