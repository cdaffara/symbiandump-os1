// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementation of the Simulator TSY call classes.  The call classes
// process the call-based requests made by ETel clients and passed down to the TSY by the
// ETel Server.
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimCallTraces.h"
#endif

#include "CSimCall.h"
#include "CSimPhone.h"

void CSimCall::CloseCall(TAny* aObj)
/**
* Utility func for cleanup stack
*
* @param aObj a pointer to the CObject to close
*/
	{
	((CObject*)aObj)->Close();
	}

CSimCall::CSimCall(CSimLine* aLine,const TName& aName, CSimPhone* aPhone)
			: iState(RMobileCall::EStatusIdle),iLine(aLine), 
			  iHookState(ConvertStateToHook(iState)), iPhone(aPhone)
	{
	iNotifyStatusChange.iNotifyPending = EFalse;
	iMobileNotifyStatusChange.iNotifyPending = EFalse;
	iAnswerIncomingCall.iNotifyPending = EFalse;
	iName.Copy(aName);
	}

void CSimCall::ConstructL()
/**
* Standard constructor
*/
	{
	iCallDurationHandler = CSimCallDuration::NewL(this);
	iNotifyRemotePartyInfoTimer = CSimCallRemotePartyInfoChange::NewL(this);
	}


CSimCall::~CSimCall()
/**
* Standard destructor
*/
	{
	iLine->CallDestructor(this);
	delete iCallDurationHandler;
	delete iNotifyRemotePartyInfoTimer;
	}

CTelObject::TReqMode CSimCall::ReqModeL(const TInt aIpc)
/**
* This function returns the Request Mode for the request with the passed IPC value.
* The ETel Server provides a function for returning the standard request modes for
* the Core API requests.
* Multimode API requests mode are handled here.
*
* @param aIpc the IPc number representing the client request
* @return CTelObject::TReqMode the request mode to be used for this IPc number
* @leave Leaves if the IPc number is not found
*/
	{
	// ReqModeL is called from the server's CTelObject::ReqAnalyserL
	// in order to check the type of request it has

	CTelObject::TReqMode reqMode=0;
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_REQMODEL_1, "CSimCall::ReqModeL called with IPC number %d",aIpc);
	switch (aIpc)
		{
	//
	// No Flow Control NOR Multiple Completion
	//
	case EMobileCallGetMobileDataCallCaps:
	case EMobileCallGetMobileCallCaps:
	case EMobileCallGetMobileCallStatus:
	case EMobileCallGetMobileCallInfo:
	case EMobileCallDialEmergencyCall:
	case EMobileCallGetMobileDataCallRLPRange:
	case EMobileCallSetDynamicHscsdParams:
	case EMobileCallGetCurrentHscsdInfo:
	case EMobileCallHold:
	case EMobileCallResume:
	case EMobileCallSwap:
	case EMobileCallAnswerISV:
		break;

	//
	// Flow Control Obeyed
	//
	case EMobileCallDialISV:
		reqMode=KReqModeFlowControlObeyed;
		break;

	//
	// Multiple Completion Services with Immediate Server Repost
	// (Usually Notifications)
	//

	case EMobileCallNotifyHscsdInfoChange:
	case EMobileCallNotifyMobileCallStatusChange:
	case EMobileCallNotifyMobileCallCapsChange:
	case EMobileCallNotifyRemotePartyInfoChange:
		reqMode=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
		
	default:
		reqMode=CCallBase::ReqModeL(aIpc);
		break;
	}
	return reqMode;
	}

TInt CSimCall::NumberOfSlotsL(const TInt aIpc)
/**
* NumberOfSlotsL is called by the server when it is registering a new notification
* It enables the TSY to tell the server how many buffer slots to allocate for
* "repost immediately" notifications that may trigger before clients collect them
*
* @param aIpc the IPc number representing the client request
* @return TInt the number of slots required
* @leave Leaves if the IPc number is not found
*/
	{
	switch (aIpc)
		{
	case EMobileCallNotifyHscsdInfoChange:
	case EMobileCallNotifyMobileDataCallCapsChange:
	case EMobileCallNotifyMobileCallStatusChange:
	case EMobileCallNotifyMobileCallCapsChange:
	case EMobileCallNotifyRemotePartyInfoChange:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NUMBEROFSLOTSL_1, "CSimCall: Registered with default number of slots");
		return KDefaultNumberOfSlots;
	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NUMBEROFSLOTSL_2, "CSimCall::NumberOfSlotsL: No match for IPC, defering to base function");
		break;
		}
	return CCallBase::NumberOfSlotsL(aIpc);
	}


TInt CSimCall::RegisterNotification(const TInt /*aIpc*/)
/**
* The ETel Server calls this function when the first client makes a notification
* request.  If supported by the underlying protocol controlling the
* signalling stack, this can be used to start requesting updates for the relevant service.
*/
	{
	return KErrNone;
	}

TInt CSimCall::DeregisterNotification(const TInt /*aIpc*/)
/**
* The ETel Server calls this function when the last client that had previously
* made a notification request closes its ETel Server handle.  If supported by
* the underlying protocol controlling the	signalling stack, this can be used
* to stop requesting updates for the relevant service.
*/
	{
	return KErrNone;
	}


void CSimCall::Init()
/**
* This function can be used to perform any necessary synchronous initialisation.
*/
	{}

TInt CSimCall::GetCaps(const TTsyReqHandle aTsyReqHandle,RCall::TCaps* aCallCaps)
/**
* Retrieve the Call capabilities
*
* @param aTsyReqHandle	TSY handle associated with this request.s
* @param aCallCaps		Pointer to the call capability
* @return KErrNone
*/
	{
	aCallCaps->iFlags=iCaps;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimCall::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RCall::TCaps* aCaps)
/**
* Register a client's interest in being notified when the call caps change.
* @param aTsyReqHandle	The TSY handle associated with this request.
* @param aCaps			The capability structure that will be populated with the new capability
*						information.
* @return TInt			Standard error code.
*/
	{
	__ASSERT_ALWAYS(!iNotifyCapsChange.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iNotifyCapsChange.iNotifyPending=ETrue;
	iNotifyCapsChange.iNotifyHandle=aTsyReqHandle;
	iNotifyCapsChange.iNotifyData=aCaps;
	return KErrNone;
	}

TInt CSimCall::NotifyCapsChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified when the call capabilities change.
*/
	{
	if(iNotifyCapsChange.iNotifyPending)
		{
		iNotifyCapsChange.iNotifyPending=EFalse;
		ReqCompleted(iNotifyCapsChange.iNotifyHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimCall::GetMobileCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
/**
* Retrieve the Mobile Call capabilities
*
* @param aTsyReqHandle	TSY handle associated with this request.
* @param aCaps			Pointer to the call capability
* @return KErrNone
*/
	{
	TPckg<RMobileCall::TMobileCallCapsV1>* capsPckg=(TPckg<RMobileCall::TMobileCallCapsV1>*)aCaps;
	RMobileCall::TMobileCallCapsV1& caps=(*capsPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(caps);
	if(err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	caps.iCallControlCaps=iCaps;		// None of the extended caps are supported.
	caps.iCallEventCaps=0;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimCall::NotifyMobileCallCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
/**
* Register a client's interest in being notified when the RMobileCall capabilities change.
* @param aTsyReqHandle	The TSY handle associated with this request.
* @param aCaps			The capability structure that will be populated with the new capability
*						information.
* @return TInt			Standard error code.
*/
	{
	__ASSERT_ALWAYS(!iNotifyMobileCapsChange.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iNotifyMobileCapsChange.iNotifyPending=ETrue;
	iNotifyMobileCapsChange.iNotifyHandle=aTsyReqHandle;
	iNotifyMobileCapsChange.iNotifyData=aCaps;
	return KErrNone;
	}

TInt CSimCall::NotifyMobileCallCapsChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified when the RMobileCall capabilities change.
*/
	{
	if(iNotifyMobileCapsChange.iNotifyPending)
		{
		iNotifyMobileCapsChange.iNotifyPending=EFalse;
		ReqCompleted(iNotifyMobileCapsChange.iNotifyHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimCall::NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus)
/**
* Record a client's interst in being notified when the hook changes state.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aHookStatus pointer to the hook status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYHOOKCHANGE_1, ">>CSimCall::NotifyHookChange");
	iNotifyHookChange.iNotifyPending = ETrue;
	iNotifyHookChange.iNotifyHandle = aTsyReqHandle;
	iNotifyHookChange.iNotifyData = aHookStatus;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYHOOKCHANGE_2, "<<CSimCall::NotifyHookChange");
	return KErrNone;
	}

TInt CSimCall::NotifyHookChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified when the hook state changes.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYHOOKCHANGECANCEL_1, ">>CSimCall::NotifyHookChangeCancel");
	if(iNotifyHookChange.iNotifyPending)
		{
		iNotifyHookChange.iNotifyPending=EFalse;
		ReqCompleted(iNotifyHookChange.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYHOOKCHANGECANCEL_2, "<<CSimCall::NotifyHookChangeCancel");
	return KErrNone;
	}

TInt CSimCall::NotifyMobileCallStatusChange(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus)
/**
* Record a client's interest in being notified when the call status changes.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the call status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYMOBILECALLSTATUSCHANGE_1, ">>CSimCall::NotifyMobileCallStatusChange");
	__ASSERT_ALWAYS(!iMobileNotifyStatusChange.iNotifyPending,SimPanic(ENotificationAlreadyPending));

	iMobileNotifyStatusChange.iNotifyPending = ETrue;
	iMobileNotifyStatusChange.iNotifyHandle = aTsyReqHandle;
	iMobileNotifyStatusChange.iNotifyData = aStatus;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYMOBILECALLSTATUSCHANGE_2, "<<CSimCall::NotifyMobileCallStatusChange");
	return KErrNone;
	}

TInt CSimCall::NotifyMobileCallStatusChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified when the call status changes.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYMOBILECALLSTATUSCHANGECANCEL_1, ">>CSimCall::NotifyMobileCallStatusChangeCancel");
	if(iMobileNotifyStatusChange.iNotifyPending)
		{
		iMobileNotifyStatusChange.iNotifyPending=EFalse;
		ReqCompleted(iMobileNotifyStatusChange.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYMOBILECALLSTATUSCHANGECANCEL_2, "<<CSimCall::NotifyMobileCallStatusChangeCancel");
	return KErrNone;
	}


TInt CSimCall::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
/**
* Record a client's interest in being notified when the call status changes.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the call status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYSTATUSCHANGE_1, ">>CSimCall::NotifyStatusChange");
	__ASSERT_ALWAYS(!iNotifyStatusChange.iNotifyPending,SimPanic(ENotificationAlreadyPending));

	iNotifyStatusChange.iNotifyPending = ETrue;
	iNotifyStatusChange.iNotifyHandle = aTsyReqHandle;
	iNotifyStatusChange.iNotifyData = aStatus;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYSTATUSCHANGE_2, "<<CSimCall::NotifyStatusChange");
	return KErrNone;
	}

TInt CSimCall::NotifyStatusChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
*	Cancel a client's interest in being notified when the call status changes.
*	This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYSTATUSCHANGECANCEL_1, ">>CSimCall::NotifyStatusChangeCancel");
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		ReqCompleted(iNotifyStatusChange.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYSTATUSCHANGECANCEL_2, "<<CSimCall::NotifyStatusChangeCancel");
	return KErrNone;
	}



TInt CSimCall::NotifyDurationChange(const TTsyReqHandle aTsyReqHandle,TTimeIntervalSeconds* aTime)
/**
* Record a client's interest in being notified when the call duration changes.
* 
* @param aTsyReqHandle
* @param aTime
* @return KErrNone or symbian wide error code
*/
	{
	iCallDurationHandler->StartNotification(aTsyReqHandle, aTime);
	return KErrNone;
	}

TInt CSimCall::NotifyDurationChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Not Supported in this TSY
*/
	{
	iCallDurationHandler->StopNotification();
	return KErrNone;
	}

TInt CSimCall::GetInfo(const TTsyReqHandle aTsyReqHandle, RCall::TCallInfo* aCallInfo)
/**
* Retrieve the Call Information.
*
* @param aTsyReqHandle
* @param aCallInfo pointer to the call information to be returned to client
* @return KErrNone
*/
	{
	aCallInfo->iCallName.Copy(iName);
	aCallInfo->iLineName.Copy(iLine->iLineName);
	aCallInfo->iHookStatus=ConvertStateToHook(iState);
	aCallInfo->iStatus=GetCoreCallStatus();
	aCallInfo->iDuration=0;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimCall::GetMobileCallInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aMobileCallInfo)
/**
* Retrieve the Mobile Call Information
*
* @param aTsyReqHandle
* @param aCallInfo pointer to the call information to be returned to client
* @return KErrNone
*/
	{
	RMobileCall::TMobileCallInfoV1Pckg* mobileCallInfoV1=(RMobileCall::TMobileCallInfoV1Pckg*)aMobileCallInfo;
	RMobileCall::TMobileCallInfoV1& mobileCallInfo=(*mobileCallInfoV1)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(mobileCallInfo);
	if(err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TUint caps=Caps();
	if(caps&RCall::KCapsVoice)
		mobileCallInfo.iService=RMobilePhone::EVoiceService;
	else if(caps&RCall::KCapsData)
		mobileCallInfo.iService=RMobilePhone::ECircuitDataService;
	else if(caps&RCall::KCapsFax)
		mobileCallInfo.iService=RMobilePhone::EFaxService;
	else
		mobileCallInfo.iService=RMobilePhone::EServiceUnspecified;

	mobileCallInfo.iValid=0x0;
	mobileCallInfo.iStatus=iState;
	mobileCallInfo.iCallName.Copy(iName);
	mobileCallInfo.iLineName.Copy(iLine->iLineName);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_GETMOBILECALLINFO_1, "CSimCall::GetMobileCallInfo request completed with %d",iState);
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

RCall::TStatus CSimCall::GetCoreCallStatus()
/**
* Converts Multimode call status (RMobileCall::TMobileCallStatus) to 
*          Core call Status (RCall::TStatus)
*
* @return RCall::TStatus The core call status
*/
	{
// All status enums with values of Disconnecting and below are identical in
// ETelMM and Core, so the mapping function is simple.
	RCall::TStatus coreStatus;
	if (iState <= RMobileCall::EStatusDisconnecting)
		coreStatus = (RCall::TStatus)iState;
	else
		switch (iState)
		{
		case RMobileCall::EStatusReconnectPending:
		case RMobileCall::EStatusHold:
			coreStatus = RCall::EStatusConnected;
			break;
		case RMobileCall::EStatusWaitingAlternatingCallSwitch:
			coreStatus = RCall::EStatusIdle;
			break;
		default:
			coreStatus = RCall::EStatusUnknown;
			break;
		}
	return coreStatus;
	}

TInt CSimCall::GetStatus(const TTsyReqHandle aTsyReqHandle, RCall::TStatus* aCallStatus)
/**
* Return the current call state. (Core API request)
*
* @param aTsyReqHandle
* @param aCallStatus pointer to the call status
* @return KErrNone
*/
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_GETSTATUS_1, ">>CSimCall::GetStatus");
	*aCallStatus=GetCoreCallStatus();
	ReqCompleted(aTsyReqHandle,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_GETSTATUS_2, "<<CSimCall::GetStatus");
	return KErrNone;
    }

TInt CSimCall::GetMobileCallStatus(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallStatus* aCallStatus)
/**
* Return the current call state. (Multimode API request)
*
* @param aTsyReqHandle
* @param aCallStatus pointer to the call status
* @return KErrNone
*/
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_GETMOBILECALLSTATUS_1, ">>CSimCall::GetMobileCallStatus");
	*aCallStatus=iState;
	ReqCompleted(aTsyReqHandle,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_GETMOBILECALLSTATUS_2, "<<CSimCall::GetMobileCallStatus");
	return KErrNone;
    }

TInt CSimCall::TransferOwnership(const TTsyReqHandle aTsyReqHandle)
/**
* Transfer call ownership
* Not supported in this version of the Simulator TSY
*/
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimCall::AcquireOwnership(const TTsyReqHandle aTsyReqHandle)
/**
* Acquires call ownership
* Not supported in this version of the Simulator TSY
*/
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimCall::AcquireOwnershipCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* cancel AcquireOwnership request
* Not supported in this version of the Simulator TSY
*/
	{
	return KErrNone;
	}

TInt CSimCall::GetCallDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime)
/**
* Retrieves the last call duration
* Not supported in this version of the Simulator TSY
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_GETCALLDURATION_1, ">>CSimCall::GetCallDuration");
	
	iCallDurationHandler->GetDuration(aTime);
	
	ReqCompleted(aTsyReqHandle,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_GETCALLDURATION_2, "<<CSimCall::GetCallDuration");
	return KErrNone;
	}

TInt CSimCall::RecoverDataPortAndRelinquishOwnership()
/**
* Recovers the comm port.
* This is a data call specific request so not supported here.
*/
	{
	return KErrNotSupported;
	}

TInt CSimCall::GetFaxSettings(const TTsyReqHandle,RCall::TFaxSessionSettings*)
/**
* Retrieves the last fax settings
* Not supported in this version of the Simulator TSY
*/
	{	
	return KErrNotSupported;
	}

TInt CSimCall::SetFaxSettings(const TTsyReqHandle,const RCall::TFaxSessionSettings*)
/**
* Sets the fax settings
* Not supported in this version of the Simulator TSY
*/
	{	
	return KErrNotSupported;
	}

CTelObject* CSimCall::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
* Only Fax Calls can be opened from the Call object, KErrNotSupported is returned.
*/

	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimCall::OpenNewObjectL(TDes& /*aNewName*/)
/**
* Only Fax objects can be opened from the Call object, and we are not supporting fax calls 
* in this TSY. KErrNotSupported is returned.
*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

TBool CSimCall::Used()
	{
	return (AccessCount()>1);
	}

void CSimCall::SetUsed()
	{
	(void)Open();
	}

void CSimCall::SetUnused()
	{
	Close();
	}

TInt CSimCall::ChangeStateL(RMobileCall::TMobileCallStatus aNewState,TBool aSwap, TBool aNoPropagation)
/**
* Attempt to change state.
* First validate that the requested state change is ok.  If it is then proceed to change
* the state and complete any pending state change notification.
*
* @param aState the new state to change to
* @param aSwap indicates that state change takes place as a consequence of swap operation on the call
* @param aNoPropagation  indicates whether change propagate to the holder line object
* @return Error indication if change of state is successful or not
*/
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_CHANGESTATEL_1, ">>CSimCall::ChangeState 0x%08x [newState=%d] entry", (TUint)this,aNewState);
	
	if(!aNoPropagation)
		{
		TInt ret=iLine->ChangeStateL(aNewState,aSwap,this);
		if(ret!=KErrNone)
			return ret;
		}
	
// Check for call duration change
	if (!iCallDurationHandler)
		iCallDurationHandler=CSimCallDuration::NewL(this);
	if ((iState != RMobileCall::EStatusConnected) && (iState != RMobileCall::EStatusHold) &&  (aNewState == RMobileCall::EStatusConnected))
		iCallDurationHandler->StartDuration();
	else if ((aNewState != RMobileCall::EStatusConnected) && (aNewState != RMobileCall::EStatusHold) && ((iState == RMobileCall::EStatusConnected) || (iState == RMobileCall::EStatusHold)))
		iCallDurationHandler->StopDuration();
		
// Actually change the state.
	iState=aNewState;
	
// Check for a pending state change notification (core)
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		*(RCall::TStatus*)iNotifyStatusChange.iNotifyData=GetCoreCallStatus();
		ReqCompleted(iNotifyStatusChange.iNotifyHandle,KErrNone);
		}

// Check for a pending state change notification (multimode)
	if(iMobileNotifyStatusChange.iNotifyPending)
		{
		iMobileNotifyStatusChange.iNotifyPending=EFalse;
		*(RMobileCall::TMobileCallStatus*)iMobileNotifyStatusChange.iNotifyData=iState;
		ReqCompleted(iMobileNotifyStatusChange.iNotifyHandle,KErrNone);
		}

// Check for a pending hook change notification.
	RCall::THookStatus hookStatus=ConvertStateToHook(iState);
	if(iHookState!=hookStatus)
		{
		iHookState=hookStatus;
		if(iNotifyHookChange.iNotifyPending)
			{
			iNotifyHookChange.iNotifyPending=EFalse;
			*(RCall::THookStatus*)iNotifyHookChange.iNotifyData=iHookState;
			ReqCompleted(iNotifyHookChange.iNotifyHandle,KErrNone);
			}
		}

// Check for a possible change in capabilities.
	TUint caps=Caps();
	if(iCaps!=caps)
		{
		iCaps=caps;
		if(iNotifyCapsChange.iNotifyPending)
			{
			iNotifyCapsChange.iNotifyPending=EFalse;
			((RCall::TCaps*)iNotifyCapsChange.iNotifyData)->iFlags=iCaps;
			ReqCompleted(iNotifyCapsChange.iNotifyHandle,KErrNone);
			}
		if(iNotifyMobileCapsChange.iNotifyPending)
			{
			iNotifyMobileCapsChange.iNotifyPending=EFalse;
			TPckg<RMobileCall::TMobileCallCapsV1>* mobileCallCapsPckg=(TPckg<RMobileCall::TMobileCallCapsV1>*)iNotifyMobileCapsChange.iNotifyData;
			RMobileCall::TMobileCallCapsV1& mobileCallCaps=(*mobileCallCapsPckg)();
	
			mobileCallCaps.iCallControlCaps=iCaps;		// None of the extended caps are supported.
			mobileCallCaps.iCallEventCaps=0;
			ReqCompleted(iNotifyMobileCapsChange.iNotifyHandle,KErrNone);
			}
		}
	if((aNewState == RMobileCall::EStatusConnected && !aSwap)|| aNewState == RMobileCall::EStatusDisconnecting)
		iLine->UpdatePhoneNotifiers(this,aNewState);		

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_CHANGESTATEL_2, "<<CSimCall::ChangeState exit %d",iState);
	return KErrNone;
	}


void CSimCall::UpdateNotifiers()
/**
	Update notifiers of other voice call when it gets swapped
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_UPDATENOTIFIERS_1, ">>CSimCall::UpdateNotifiers 0x%08x entry", this);
	
// Check for call duration change
	if (!iCallDurationHandler)
		{
		TRAP_IGNORE(iCallDurationHandler=CSimCallDuration::NewL(this));
		}
	
			
// Check for a pending state change notification (core)
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		*(RCall::TStatus*)iNotifyStatusChange.iNotifyData=GetCoreCallStatus();
		ReqCompleted(iNotifyStatusChange.iNotifyHandle,KErrNone);
		}

// Check for a pending state change notification (multimode)
	if(iMobileNotifyStatusChange.iNotifyPending)
		{
		iMobileNotifyStatusChange.iNotifyPending=EFalse;
		*(RMobileCall::TMobileCallStatus*)iMobileNotifyStatusChange.iNotifyData=iState;
		ReqCompleted(iMobileNotifyStatusChange.iNotifyHandle,KErrNone);
		}

// Check for a pending hook change notification.
	RCall::THookStatus hookStatus=ConvertStateToHook(iState);
	if(iHookState!=hookStatus)
		{
		iHookState=hookStatus;
		if(iNotifyHookChange.iNotifyPending)
			{
			iNotifyHookChange.iNotifyPending=EFalse;
			*(RCall::THookStatus*)iNotifyHookChange.iNotifyData=iHookState;
			ReqCompleted(iNotifyHookChange.iNotifyHandle,KErrNone);
			}
		}

// Check for a possible change in capabilities.
	TUint caps=Caps();
	if(iCaps!=caps)
		{
		iCaps=caps;
		if(iNotifyCapsChange.iNotifyPending)
			{
			iNotifyCapsChange.iNotifyPending=EFalse;
			((RCall::TCaps*)iNotifyCapsChange.iNotifyData)->iFlags=iCaps;
			ReqCompleted(iNotifyCapsChange.iNotifyHandle,KErrNone);
			}
		if(iNotifyMobileCapsChange.iNotifyPending)
			{
			iNotifyMobileCapsChange.iNotifyPending=EFalse;
			TPckg<RMobileCall::TMobileCallCapsV1>* mobileCallCapsPckg=(TPckg<RMobileCall::TMobileCallCapsV1>*)iNotifyMobileCapsChange.iNotifyData;
			RMobileCall::TMobileCallCapsV1& mobileCallCaps=(*mobileCallCapsPckg)();

			mobileCallCaps.iCallControlCaps=iCaps;		// None of the extended caps are supported.
			mobileCallCaps.iCallEventCaps=0;
			ReqCompleted(iNotifyMobileCapsChange.iNotifyHandle,KErrNone);
			}
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_UPDATENOTIFIERS_2, "<<CSimCall::UpdateNotifiers exit %d",iState);
	}

TInt CSimCall::ActionEvent(TCallEvent /*aEvent*/, TInt /*aStatus*/)
/**
* This is an empty shell function. Each line should implement its own
* state machine.
*/
	{
	return KErrNotSupported;
	}

void CSimCall::GenerateCoreCallCaps(TUint& aCaps)
/**
 * Populate the variable capabilities that are dependant upon the call state.
 * @param TUint		The capability variable that will be populated with the core capabilities.
 */
	{
	if(iState==RMobileCall::EStatusIdle)
		aCaps |= RCall::KCapsDial;

	if(iState==RMobileCall::EStatusConnected)
		aCaps |= RCall::KCapsHangUp;

	if((iLine->IsAnswerCallObjectSpare())&&(iState==RMobileCall::EStatusIdle))
		aCaps |= RCall::KCapsAnswer;
	}

void CSimCall::ResetIfRingingL()
	{
	if(iState==RMobileCall::EStatusRinging)
		__ASSERT_ALWAYS(ChangeStateL(RMobileCall::EStatusIdle,EFalse,ETrue) == KErrNone, SimPanic(EGeneral));
	}

void CSimCallDuration::TimerCallBack(TInt /*aId*/)
  	{
  	//increase duration by 1 sec
  	iCallDuration=static_cast<TTimeIntervalSeconds>(iCallDuration.Int()+1);
  	
  	if(iNotifyDurationChange.iNotifyPending)
  		{
  		*(TTimeIntervalSeconds*)iNotifyDurationChange.iNotifyData=iCallDuration;
  		iNotifyDurationChange.iNotifyPending=EFalse;
  		iCall->ReqCompleted(iNotifyDurationChange.iNotifyHandle,KErrNone);		
  		}
  	iDurationTimer->Start(1 , this);
  	}

CSimCallDuration* CSimCallDuration::NewL(CSimCall* aCall)
/**
 * Standard two-phase constructor.
 * @param aPhone				The parent phone object.
 * @return CSimCallDuration		The new CallDuration class 
 */
	{
	CSimCallDuration* self=new(ELeave) CSimCallDuration(aCall);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimCallDuration::CSimCallDuration(CSimCall* aCall)
		:iCallDuration(0),iCall(aCall)
/**
 * Trivial first phase construction.
 * @param aPhone				The parent phone object.
 */
	{
	iNotifyDurationChange.iNotifyPending=EFalse;
	iNotifyDurationChange.iNotifyData=0;
	}


void CSimCallDuration::ConstructL()
	{
	iDurationTimer = CSimTimer::NewL(iCall->iLine->iPhone);
	}


CSimCallDuration::~CSimCallDuration()
/**
 * Standard destructor.  Destroy the heap-based object owned by this object.
 */
	{
	if (iDurationTimer)
		{
		if (iDurationTimer->IsActive())
			iDurationTimer->Cancel();
		delete iDurationTimer;
		}
	}

void CSimCallDuration::StartDuration()
	{
	iCallDuration = 0;
	iDurationTimer->Start(1 , this);
	}

void CSimCallDuration::StopDuration()
	{	
	if (iDurationTimer)
		{
		if (iDurationTimer->IsActive())
			iDurationTimer->Cancel();
		}
	}

void CSimCallDuration::StartNotification(TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime)
	{
	__ASSERT_ALWAYS(!iNotifyDurationChange.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iNotifyDurationChange.iNotifyData=aTime;
	iNotifyDurationChange.iNotifyPending=ETrue;
	iNotifyDurationChange.iNotifyHandle=aTsyReqHandle;
	}

void CSimCallDuration::StopNotification()
	{
	if(iNotifyDurationChange.iNotifyPending)
		{
		iNotifyDurationChange.iNotifyPending=EFalse;
		iCall->ReqCompleted(iNotifyDurationChange.iNotifyHandle,KErrCancel);
		}
	}

void CSimCallDuration::GetDuration(TTimeIntervalSeconds* aTime)
	{
	*aTime = iCallDuration;
	}
	
CSimCallRemotePartyInfoChange* CSimCallRemotePartyInfoChange::NewL(CSimCall* aCall)
/**
 * Standard two-phase constructor.
 * @param aCall on upon remote party info is needed
 * @return CSimCallRemotePartyInfoChange		
 */
	{
	CSimCallRemotePartyInfoChange* self=new(ELeave) CSimCallRemotePartyInfoChange(aCall);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimCallRemotePartyInfoChange::CSimCallRemotePartyInfoChange(CSimCall* aCall)
		:iCall(aCall)
/**
 * Trivial first phase construction.
 * @param aCall is call object upon remote party info is needed.
 */
	{
	iNotifyRemotePartyInfo.iNotifyPending=EFalse;
	iNotifyRemotePartyInfo.iNotifyData=0;
	}


void CSimCallRemotePartyInfoChange::ConstructL()
	{
	iRemoteInfoTimer = CSimTimer::NewL(iCall->iLine->iPhone);
	}

void CSimCallRemotePartyInfoChange::Start()
	{
	iRemoteInfoTimer->Start(iDelay, this);
	}

CSimCallRemotePartyInfoChange::~CSimCallRemotePartyInfoChange()
/**
 * Standard destructor.  Destroy the heap-based object owned by this object.
 */
	{
	if (iRemoteInfoTimer)
		{
		if (iRemoteInfoTimer->IsActive())
			iRemoteInfoTimer->Cancel();
		delete iRemoteInfoTimer;
		}
	}
	
void CSimCallRemotePartyInfoChange::TimerCallBack(TInt /*aId*/)
	{
	if(iRemotePartyInfoV1.iRemoteIdStatus != RMobileCall::ERemoteIdentityUnknown && iNotifyRemotePartyInfo.iNotifyPending)	
		{
		iNotifyRemotePartyInfo.iNotifyPending=EFalse;
		*(RMobileCall::TMobileCallRemotePartyInfoV1*)iNotifyRemotePartyInfo.iNotifyData=iRemotePartyInfoV1;
		iCall->ReqCompleted(iNotifyRemotePartyInfo.iNotifyHandle,KErrNone);
		}	
	}
	
TInt CSimCall::NotifyRemotePartyInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aRemoteParty)
/**
* Record a client's interest in being notified when the remote party info changes.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the call status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYREMOTEPARTYINFOCHANGE_1, ">>CSimCall::NotifyRemotePartyInfoChange");
	__ASSERT_ALWAYS(iNotifyRemotePartyInfoTimer, SimPanic(EOjectNotConstructed));
	__ASSERT_ALWAYS(!iNotifyRemotePartyInfoTimer->iNotifyRemotePartyInfo.iNotifyPending,SimPanic(ENotificationAlreadyPending));
	
	RMobileCall::TMobileCallRemotePartyInfoV1Pckg* remotepartyPckg = (RMobileCall::TMobileCallRemotePartyInfoV1Pckg*) aRemoteParty;
	RMobileCall::TMobileCallRemotePartyInfoV1& remoteparty = (*remotepartyPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(remoteparty);
	if(err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	//start timer
	iNotifyRemotePartyInfoTimer->Start();	
	
	iNotifyRemotePartyInfoTimer->iNotifyRemotePartyInfo.iNotifyPending = ETrue;
	iNotifyRemotePartyInfoTimer->iNotifyRemotePartyInfo.iNotifyHandle = aTsyReqHandle;
	iNotifyRemotePartyInfoTimer->iNotifyRemotePartyInfo.iNotifyData = &remoteparty;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYREMOTEPARTYINFOCHANGE_2, "<<CSimCall::NotifyRemotePartyInfoChange");
	return KErrNone;
	}
	
TInt CSimCall::NotifyRemotePartyInfoChangeCancel()
/**
* Cancel a client's interest in being notified when the remote party info changes.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYREMOTEPARTYINFOCHANGECANCEL_1, ">>CSimCall::NotifyRemotePartyInfoChangeCancel");
	if(iNotifyRemotePartyInfoTimer && iNotifyRemotePartyInfoTimer->iNotifyRemotePartyInfo.iNotifyPending)
		{
		iNotifyRemotePartyInfoTimer->iNotifyRemotePartyInfo.iNotifyPending=EFalse;
		ReqCompleted(iNotifyRemotePartyInfoTimer->iNotifyRemotePartyInfo.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALL_NOTIFYREMOTEPARTYINFOCHANGECANCEL_2, "<<CSimCall::NotifyRemotePartyInfoChangeCancel");
	return KErrNone;
	}
