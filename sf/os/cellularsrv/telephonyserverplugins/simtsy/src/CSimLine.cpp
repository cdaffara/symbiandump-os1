
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
// This file contains the implementation for the functionality common to all 
// the specific line classes, such as CSimVoiceLine and CSimDataline.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimLineTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimLine.h"
#include "CSimPhone.h"
#include "CSimCall.h"
#include "CSimPubSub.h"

void CSimLine::CloseLine(TAny* aObj)
/**
*	Utility func for cleanup stack
*
* @param aObj a pointer to the CObject to close
*/
	{
	((CObject*)aObj)->Close();
	}

CSimLine::CSimLine(CSimPhone* aPhone)
			:iPhone(aPhone), iState(RMobileCall::EStatusIdle), 
			iHookState(ConvertStateToHook(iState))
	{
	iNameOfLastCallAdded.Zero();
	iNotifyStatusChange.iNotifyPending = EFalse;
	iNotifyMobileStatusChange.iNotifyPending = EFalse;
	}

void CSimLine::ConstructL(const TName& aName)
/**
* Second phase of two phase constructor.
* Creates the System Agent object for incoming call notification and other notifications.
*
* @param aName name of the Line to be created.
*/
	{
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CONSTRUCTL_1, ">>CSimLine::ConstructL [aLineName=%S]", aName);
	iLineName = aName;
	iSimPubSubIC = CSimPubSub::NewL(this,iICProperty);
	iSimPubSubIC->Start();

	iSimPubSubRH = CSimPubSub::NewL(this,iRHProperty);
	iSimPubSubRH->Start();
	
	iNotifyIncomingCallPause=this->CfgFile()->ItemValue(KNotifyIncomingCallPauseDuration,KDefaultNotifyIncomingCallPauseDuration);
	
	iTimer = CSimTimer::NewL(iPhone);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CONSTRUCTL_2, "<<CSimLine::ConstructL");
}

CSimLine::~CSimLine()
/**
*	Standard destructor.
*/
	{
	delete iSimPubSubIC;
	delete iSimPubSubRH;
	iTimer->Cancel();
	delete iTimer;
	}


void CSimLine::UpdatePhoneNotifiers(CSimCall* aCall,TUint /*aStatus*/)
	{
		TInt count=	iCalls->Count();
		TInt i=0;
		while(i<count)
		{
 		if(iCalls->At(i) != aCall && (iCalls->At(i)->iState == RMobileCall::EStatusHold || iCalls->At(i)->iState == RMobileCall::EStatusConnected))
			iCalls->At(i)->UpdateNotifiers();
		i++;
		}	
	}

CTelObject::TReqMode CSimLine::ReqModeL(const TInt aIpc)
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
	switch (aIpc)
		{
//
// No Flow Control NOR Multiple Completion
//
	case EMobileLineGetMobileLineStatus:
		break;

//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileLineNotifyMobileLineStatusChange:
		reqMode=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Must be a core API request so get request mode from Etel
//
	default:
		reqMode=CLineBase::ReqModeL(aIpc);
		break;
		}

	return reqMode;
	}


TInt CSimLine::NumberOfSlotsL(const TInt aIpc)
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
	case EMobileLineNotifyMobileLineStatusChange:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NUMBEROFSLOTSL_1, "CLineMobile: Registered with the default number of slots");
		return KDefaultNumberOfSlots;

	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NUMBEROFSLOTSL_2, "CSimLine::NumberOfSlotsL: No match for IPC, defering to base function");
		break;
		}
	return CLineBase::NumberOfSlotsL(aIpc);
	}


TInt CSimLine::RegisterNotification(const TInt /*aIpc*/)
/**
* The ETel Server calls this function when the first client makes a notification request.
* If supported by the underlying protocol controlling the signalling stack, 
* this can be used to start requesting updates for the relevant service.
*/
	{
	return KErrNone;
	}

TInt CSimLine::DeregisterNotification(const TInt /*aIpc*/)
/**
* The ETel Server calls this function when the last client that had previously
* made a notification request closes its ETel Server handle.  If supported by
* the underlying protocol controlling the	signalling stack, this can be used
* to stop requesting updates for the relevant service.
*/
	{
	return KErrNone;
	}

void CSimLine::Init()
/**
*	This function can be used to perform any necessary synchronous initialisation.
*/
	{}

TInt CSimLine::NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName* aName)
/**
* Register a client's interest in an incoming call notification.
* Core ETel API Request.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aName pointer to the name of the call
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYINCOMINGCALL_1, ">>CSimLine::NotifyIncomingCall");
	iNotifyIncomingCall.iNotifyPending=ETrue;
	iNotifyIncomingCall.iNotifyHandle=aTsyReqHandle;
	iNotifyIncomingCall.iNotifyData=(TAny*)aName;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYINCOMINGCALL_2, "<<CSimLine::NotifyIncomingCall");
	return KErrNone;
	}

TInt CSimLine::NotifyIncomingCallCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a client's interest in an incoming call.
* Core ETel API Request.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYINCOMINGCALLCANCEL_1, ">>CSimLine::NotifyIncomingCallCancel");
	if(iNotifyIncomingCall.iNotifyPending)
		{
		iTimer->Cancel();
		iNotifyIncomingCall.iNotifyPending=EFalse;
		ReqCompleted(iNotifyIncomingCall.iNotifyHandle, KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYINCOMINGCALLCANCEL_2, "<<CSimLine::NotifyIncomingCallCancel");
	return KErrNone;
	}

TInt CSimLine::NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus)
/**
* Record a client's interst in being notified when the line hook changes state.
* Core ETel API Request.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aHookStatus pointer to the line hook
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYHOOKCHANGE_1, ">>CSimLine::NotifyHookChange");
	iNotifyHookChange.iNotifyPending=ETrue;
	iNotifyHookChange.iNotifyHandle=aTsyReqHandle;
	iNotifyHookChange.iNotifyData=(TAny*)aHookStatus;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYHOOKCHANGE_2, "<<CSimLine::NotifyHookChange");
	return KErrNone;
	}

TInt CSimLine::NotifyHookChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified when the line hook changes state.
* Core ETel API Request.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYHOOKCHANGECANCEL_1, ">>CSimLine::NotifyHookChangeCancel");
	if(iNotifyHookChange.iNotifyPending)
		{
		iNotifyHookChange.iNotifyPending=EFalse;
		ReqCompleted(iNotifyHookChange.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYHOOKCHANGECANCEL_2, "<<CSimLine::NotifyHookChangeCancel");
	return KErrNone;
	}


TInt CSimLine::NotifyMobileLineStatusChange(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallStatus* aStatus)
/**
* Record a client's interst in being notified when the line changes state. (Multimode)
* MM ETel API Request.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the line status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYMOBILELINESTATUSCHANGE_1, ">>CSimLine::NotifyMobileLineStatusChange");
	iNotifyMobileStatusChange.iNotifyPending=ETrue;
	iNotifyMobileStatusChange.iNotifyHandle=aTsyReqHandle;
	iNotifyMobileStatusChange.iNotifyData=aStatus;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYMOBILELINESTATUSCHANGE_2, "<<CSimLine::NotifyMobileLineStatusChange");
	return KErrNone;
	}

TInt CSimLine::NotifyMobileLineStatusChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified when the line changes state.(Multimode Request)
* MM ETel API Request.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYMOBILELINESTATUSCHANGECANCEL_1, ">>CSimLine::NotifyMobileLineStatusChangeCancel");
	if(iNotifyMobileStatusChange.iNotifyPending)
		{
		iNotifyMobileStatusChange.iNotifyPending=EFalse;
		ReqCompleted(iNotifyMobileStatusChange.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYMOBILELINESTATUSCHANGECANCEL_2, "<<CSimLine::NotifyMobileLineStatusChangeCancel");
	return KErrNone;
	}

TInt CSimLine::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle, RCall::TStatus* aStatus)
/**
* Record a client's interst in being notified when the line changes state. (Core)
* Core ETel API Request.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the line status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYSTATUSCHANGE_1, ">>CSimLine::NotifyStatusChange");
	iNotifyStatusChange.iNotifyPending=ETrue;
	iNotifyStatusChange.iNotifyHandle=aTsyReqHandle;
	iNotifyStatusChange.iNotifyData=aStatus;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYSTATUSCHANGE_2, "<<CSimLine::NotifyStatusChange");
	return KErrNone;
	}

TInt CSimLine::NotifyStatusChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified when the line changes state.(Multimode Request)
* Core ETel API Request.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYSTATUSCHANGECANCEL_1, ">>CSimLine::NotifyStatusChangeCancel");
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		ReqCompleted(iNotifyStatusChange.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYSTATUSCHANGECANCEL_2, "<<CSimLine::NotifyStatusChangeCancel");
	return KErrNone;
	}

TInt CSimLine::NotifyCallAdded(const TTsyReqHandle aTsyReqHandle,TName* aName)
/**
* Record a client's interest in being notified when a new call is added.
* Core ETel API Request.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aName pointer to the name of the call
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYCALLADDED_1, ">>CSimLine::NotifyCallAdded");
	iNotifyCallAdded.iNotifyPending=ETrue;
	iNotifyCallAdded.iNotifyHandle=aTsyReqHandle;
	iNotifyCallAdded.iNotifyData=(TAny*)aName;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYCALLADDED_2, "<<CSimLine::NotifyCallAdded");
	return KErrNone;
	}

TInt CSimLine::NotifyCallAddedCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified when a new call is added.
* Core ETel API Request.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYCALLADDEDCANCEL_1, ">>CSimLine::NotifyCallAddedCancel");
	if(iNotifyCallAdded.iNotifyPending)
		{
		iNotifyCallAdded.iNotifyPending=EFalse;
		ReqCompleted(iNotifyCallAdded.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_NOTIFYCALLADDEDCANCEL_2, "<<CSimLine::NotifyCallAddedCancel");
	return KErrNone;
	}


void CSimLine::HandleNewCallAddedNotification(const TDesC& aNewName)
/**
* Process a potential "New Call Added Notification" completion.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_HANDLENEWCALLADDEDNOTIFICATION_1, ">>CSimLine::HandleNewCallAddedNotification");
	iNameOfLastCallAdded.Copy(aNewName);
	if(iNotifyCallAdded.iNotifyPending)
		{
		iNotifyCallAdded.iNotifyPending=EFalse;
		((TName*)iNotifyCallAdded.iNotifyData)->Copy(aNewName);
		ReqCompleted(iNotifyCallAdded.iNotifyHandle,KErrNone);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_HANDLENEWCALLADDEDNOTIFICATION_2, "<<CSimLine::HandleNewCallAddedNotification");
	}

TInt CSimLine::GetCaps(const TTsyReqHandle aTsyReqHandle, RLine::TCaps* aCaps)
/**
 * Retrieve the current line capabilities.
 * Core ETel API Request.
 *
 * @param aTsyReqHandle		TSY request handle associated with this request.
 * @param aCaps				Point to capability structure that will be populated with the caps.
 * @return TInt				Standard return value.
 */
	{
	aCaps->iFlags=iCaps;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimLine::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RLine::TCaps* aCaps)
/**
* Register a client's interest in being notified when the line caps change.
* Core ETel API Request.
*
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

TInt CSimLine::NotifyCapsChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified when the call capabilities change.
* Core ETel API Request.
*/
	{
	if(iNotifyCapsChange.iNotifyPending)
		{
		iNotifyCapsChange.iNotifyPending=EFalse;
		ReqCompleted(iNotifyCapsChange.iNotifyHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimLine::GetInfo(const TTsyReqHandle aTsyReqHandle, RMobileLine::TLineInfo* aLineInfo)
/**
* Retrieve the Line Information
* Core ETel API Request.
*
* @param aTsyReqHandle
* @param aLineInfo pointer to the line information to be returned to client
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_GETINFO_1, ">>CSimLine::GetInfo");
	aLineInfo->iHookStatus=iHookState;
	aLineInfo->iStatus=(RCall::TStatus)iState;
	aLineInfo->iNameOfLastCallAdded.Copy(iNameOfLastCallAdded);
	if(iAnswerNextIncomingCall)
		aLineInfo->iNameOfCallForAnswering.Copy(iAnswerNextIncomingCall->iName);
	else
		aLineInfo->iNameOfCallForAnswering.Zero();

	ReqCompleted(aTsyReqHandle,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_GETINFO_2, "<<CSimLine::GetInfo");
	return KErrNone;
	}


RCall::TStatus CSimLine::GetCoreLineStatus()
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

TInt CSimLine::GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aLineStatus)
/**
* Return the current line state. (Core API request)
* Core ETel API Request.
*
* @param aTsyReqHandle
* @param aLineStatus pointer to the line status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_GETSTATUS_1, ">>CSimLine::GetStatus");
	*aLineStatus=GetCoreLineStatus();
	ReqCompleted(aTsyReqHandle,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_GETSTATUS_2, "<<CSimLine::GetStatus");
	return KErrNone;
	}

TInt CSimLine::GetMobileLineStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aLineStatus)
/**
* Return the current line state. (MultiMode API request)
* MM ETel API Request.
*
* @param aTsyReqHandle
* @param aLineStatus pointer to the line status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_GETMOBILELINESTATUS_1, ">>CSimLine::GetMobileLineStatus");
	*aLineStatus=iState;
	ReqCompleted(aTsyReqHandle,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_GETMOBILELINESTATUS_2, "<<CSimLine::GetMobileLineStatus");
	return KErrNone;
	}

TInt CSimLine::GetHookStatus(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus* aHookStatus)
/**
* Return the current Hook state. 
* Core ETel API Request.
*
* @param aTsyReqHandle
* @param aHookStatus pointer to the hook status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_GETHOOKSTATUS_1, ">>CSimLine::GetHookStatus");
	*aHookStatus=iHookState;
	ReqCompleted(aTsyReqHandle,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_GETHOOKSTATUS_2, "<<CSimLine::GetHookStatus");
	return KErrNone;
	}

const CTestConfigSection* CSimLine::CfgFile()
/**
* Returns a pointer to the config file section
*
* @return CTestConfig a pointer to the configuration file data section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CFGFILE_1, ">>CSimLine::CfgFile");
	return iPhone->CfgFile();
	}

TInt CSimLine::ChangeStateL(RMobileCall::TMobileCallStatus aNewState,TBool aSwap,CSimCall* aOriginatingCall)
/**
* Validate and change the Line State
*
* @param aNewState the new state to change to
* @param aSwap indicates whether change comes from swap operation on the call
* @param aOriginatingCall original call that change comes from
* @return Error indication if change of state is successful or not
*/
	{
	__ASSERT_ALWAYS(aNewState!=RMobileCall::EStatusUnknown,SimPanic(ECallStatusUnknownIllegal));
	__ASSERT_ALWAYS(iState!=RMobileCall::EStatusUnknown,SimPanic(ECallStatusUnknownIllegal));
	CSimCall* swappedCall=NULL;
	TInt i=0;
	TInt count=iCalls->Count();
	for(i=0;i<count;i++)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CHANGESTATEL_1, ">>CSimLine::ChangeState0 0x%08x %d",(TUint)iCalls->At(i),iCalls->At(i)->iState);
		}
	
	TInt ret=KErrGeneral;
	const TStateChangeValidity* stateChangePnt=KLineStateChangeValidity;
	while(stateChangePnt->iOldState!=KStateTableTerminator)
		{
		if((stateChangePnt->iOldState==iState) && 
			((stateChangePnt->iNewState==aNewState) || aNewState==RMobileCall::EStatusIdle))
			{
			ret=stateChangePnt->iError;
			break;
			}
		stateChangePnt++;
		}

    //coverity[cannot_single]	
	if(ret!=KErrNone && !(aSwap && iState==RMobileCall::EStatusConnected && aNewState==RMobileCall::EStatusConnected))
		return ret;
 
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CHANGESTATEL_2, ">>CSimLine::ChangeState 0x%08x",aOriginatingCall);
	if(aNewState==RMobileCall::EStatusIdle)
		{
		if(iState==RMobileCall::EStatusIdle)
			return KErrAlreadyExists;
		else if(iState==RMobileCall::EStatusDisconnecting || iState==RMobileCall::EStatusDialling)
			{
			TInt i=0;
			TInt count=iCalls->Count();
			for(i=0;i<count;i++)
				{
				OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CHANGESTATEL_3, ">>CSimLine::ChangeState1 0x%08x %d",(TUint)iCalls->At(i),iCalls->At(i)->iState);
	
				if(aOriginatingCall!=iCalls->At(i) && iCalls->At(i)->iState != RMobileCall::EStatusIdle)
					{
					aNewState=iCalls->At(i)->iState;
					break;
					}
				}
			}		
		}
	else if(aNewState==RMobileCall::EStatusHold)
		{
		if((iState == RMobileCall::EStatusRinging) && 
			(aNewState == RMobileCall::EStatusHold))
			{
			if(aOriginatingCall->iState != RMobileCall::EStatusConnected)
				return KErrGeneral;			
			aNewState=RMobileCall::EStatusRinging;
			}
		TInt i=0;
		TInt count=iCalls->Count();
		for(i=0;i<count;i++)
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CHANGESTATEL_4, ">>CSimLine::ChangeState1 0x%08x %d",(TUint)iCalls->At(i),iCalls->At(i)->iState);
			if(iCalls->At(i) != aOriginatingCall && iCalls->At(i)->iState == RMobileCall::EStatusHold)
				{
				if(aSwap)
					{
					swappedCall=iCalls->At(i);
					swappedCall->iState=RMobileCall::EStatusConnected;
					aNewState=RMobileCall::EStatusConnected;
					}
				else
					return KErrAlreadyExists;
				}
			}	
		}
	else if(aNewState==RMobileCall::EStatusConnected)
		{
		TInt i=0;
		TInt count=iCalls->Count();
		for(i=0;i<count;i++)
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CHANGESTATEL_5, ">>CSimLine::ChangeState1 0x%08x %d",(TUint)iCalls->At(i),iCalls->At(i)->iState);
			if(iCalls->At(i) != aOriginatingCall)
				{
				if(iCalls->At(i)->iState == RMobileCall::EStatusConnected)
					{
                    //coverity[dead_error_condition]					
					if(aSwap)
						{
						swappedCall=iCalls->At(i);
						swappedCall->iState=RMobileCall::EStatusHold;
						aNewState=RMobileCall::EStatusConnected;
						break;
						}
					else
                        //coverity[dead_error_line]					    
						return KErrAlreadyExists;
					}			
				}
			}	
		}

	// Actually change the state.
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CHANGESTATEL_6, ">>CSimLine::ChangeState [oldState=%d]  [newState=%d]", iState, aNewState);
	
	if(iState != aNewState)
		{
		iState=aNewState;

		// Check for a pending line state notification (core)
		if(iNotifyStatusChange.iNotifyPending)
			{
			iNotifyStatusChange.iNotifyPending=EFalse;
			*(RCall::TStatus*)iNotifyStatusChange.iNotifyData=GetCoreLineStatus();
			ReqCompleted(iNotifyStatusChange.iNotifyHandle,KErrNone);
			}

		// Check for a pending line state notification (multimode)
		if(iNotifyMobileStatusChange.iNotifyPending)
			{
			iNotifyMobileStatusChange.iNotifyPending=EFalse;
			*(RMobileCall::TMobileCallStatus*)iNotifyMobileStatusChange.iNotifyData=iState;
			ReqCompleted(iNotifyMobileStatusChange.iNotifyHandle,KErrNone);
			}

		// Check for a pending hook state notification.
		if(iHookState!=ConvertStateToHook(iState))
			{
			iHookState=ConvertStateToHook(iState);
			if(iNotifyHookChange.iNotifyPending)
				{
				iNotifyHookChange.iNotifyPending=EFalse;
				*(RCall::THookStatus*)iNotifyHookChange.iNotifyData=iHookState;
				ReqCompleted(iNotifyHookChange.iNotifyHandle,KErrNone);
				}
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
			((RLine::TCaps*)iNotifyCapsChange.iNotifyData)->iFlags=iCaps;
			ReqCompleted(iNotifyCapsChange.iNotifyHandle,KErrNone);
			}
		}

	if(aSwap && swappedCall)
		swappedCall->UpdateNotifiers();
	// Request that the phone change its state, to ensure its in step.
	if((ret = iPhone->ValidateChangeState(this,aNewState)) != KErrNone)
		return ret;

	for(i=0;i<iCalls->Count();i++)
		{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CHANGESTATEL_7, ">>CSimLine::ChangeState5 0x%08x %d",(TUint)iCalls->At(i),iCalls->At(i)->iState);
		}
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_CHANGESTATEL_8, "<<CSimLine::ChangeState");
	return KErrNone;
	}

void CSimLine::SimPSEvent(const CSimPubSub::TPubSubProperty aProperty, TInt /*aStatus*/)
/**
Handles the P&S notifications for the Line class

@param aProperty The property key representing the notification. 
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_SIMPSEVENT_1, ">>CSimLine::SimPSEvent");
	if(aProperty==iICProperty) // check for this here still, start the timer for amount specified in config file.
		ProcessIncomingCallEvent();
	if(aProperty==iRHProperty)
		ProcessRemoteHangupEvent();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_SIMPSEVENT_2, "<<CSimLine::SimPSEvent");
	}

TInt CSimLine::SetAutoAnswerCallObject(CSimCall* aCallObject)
/**
* Attempt to set the call object that will be used to answer the next incoming
* call.  If there is already an AnswerIncomingCall registered, then
* return with the relevant error.
*
* @param aCallObject pointer to the call object
* @return KErrNone if successful
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_SETAUTOANSWERCALLOBJECT_1, ">>CSimLine::SetAnswerCallObject");
	if(iAnswerIncomingCallReqPending)			// Is there a request already outstanding?
		return KErrEtelAnswerAlreadyOutstanding;

	iAnswerIncomingCallReqPending=ETrue;
	if (iAnswerNextIncomingCall!=aCallObject)
		{
		if (iSpareCall != iAnswerNextIncomingCall)
			{
			iAnswerNextIncomingCall->Close();
			}
		iAnswerNextIncomingCall=aCallObject;		// Set the "answer next call" object
		iAnswerNextIncomingCall->Open();
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_SETAUTOANSWERCALLOBJECT_2, "<<CSimLine::SetAnswerCallObject");
	return KErrNone;
	}

void CSimLine::ResetAutoAnswerCallObject(CSimCall* aCallObject)
/**
* This function is used to reset, or effectively cancel, the call object that
* was to be used to automatically answer the next incoming call.
*
* @param aCallObject pointer to the call object
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_RESETAUTOANSWERCALLOBJECT_1, ">>CSimLine::ResetAnswerCallObject");
	__ASSERT_ALWAYS(iAnswerNextIncomingCall==aCallObject,SimPanic(EIllegalAnswerNextIncomingCallInconsistancy));
	iAnswerNextIncomingCall->Close();
	iAnswerNextIncomingCall=iSpareCall;
	iAnswerIncomingCallReqPending=EFalse;
	}

void CSimLine::ProcessIncomingCallEvent()
/**
* Process an incoming call event.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_PROCESSINCOMINGCALLEVENT_1, ">>CSimLine::ProcessIncomingCallEvent ");

	if (iSpareCall == iAnswerNextIncomingCall)
		{
		TName callName;
		TRAPD(err,iAnswerNextIncomingCall=CreateNewCallL(callName,ECallTypeNormalCall));
		if (err!=KErrNone)
			{
			SimPanic(EOjectNotConstructed);
			}
		}
	
	// Delay sending the notification of an incoming call if
	// one has been set.
	iTimer->Start(iNotifyIncomingCallPause, this);
	
	__ASSERT_ALWAYS(iAnswerNextIncomingCall->ActionEvent(CSimCall::ECallEventIncomingCall,KErrNone) == KErrNone, SimPanic(EGeneral));
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_PROCESSINCOMINGCALLEVENT_2, "<<CSimLine::ProcessIncomingCallEvent");
	}

TInt CSimLine::SetRemoteHangupCallObject(CSimCall* aCallObject)
/**
* Attempt to set the call object that will be hung up remotely
* If there is already a RemoteHangup registered, then
* return with the relevant error.
*
* @param aCallObject pointer to the call object
* @return KErrNone if successful
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_SETREMOTEHANGUPCALLOBJECT_1, ">>CSimLine::SetRemoteHangupCallObject");
	if(iRemoteHangupCallReqPending)			// Is there a request already outstanding?
		return KErrEtelInitialisationFailure;

	iRemoteHangupCallReqPending=ETrue;
	if (iRemoteHangupCall!=aCallObject)
		{
		iRemoteHangupCall=aCallObject;		// Set the "remote hang up" object
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_SETREMOTEHANGUPCALLOBJECT_2, "<<CSimLine::SetRemoteHangupCallObject");
	return KErrNone;
	}

void CSimLine::ResetRemoteHangupCallObject(CSimCall* aCallObject)
/**
* This function is used to reset, or effectively cancel, the call object that
* was to be remotely hung up.
*
* @param aCallObject pointer to the call object
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_RESETREMOTEHANGUPCALLOBJECT_1, ">>CSimLine::ResetRemoteHangupCallObject");
	__ASSERT_ALWAYS(iRemoteHangupCall==aCallObject,SimPanic(EIllegalAnswerRemoteHangupCallInconsistency));
	iRemoteHangupCallReqPending=EFalse;
	}

void CSimLine::ProcessRemoteHangupEvent()
/**
* Process a remote hangup event.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_PROCESSREMOTEHANGUPEVENT_1, ">>CSimLine::ProcessRemoteHangupEvent");

	if (iRemoteHangupCall == NULL)
		{
		TInt err = FindActiveCall(iRemoteHangupCall);
		if(err == KErrNone)
			{
			err = iRemoteHangupCall->ActionEvent(CSimCall::ECallEventRemoteHangup,KErrNone);
			if(err != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_PROCESSREMOTEHANGUPEVENT_2, "ERROR: CSimLine::ProcessRemoteHangupEvent returned: %d", err);
				}
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_PROCESSREMOTEHANGUPEVENT_3, "No outstanding call to hang up remotely");
			}
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_PROCESSREMOTEHANGUPEVENT_4, "<<CSimLine::ProcessRemoteHangupEvent");
	}

void CSimLine::CallDestructor(CSimCall* aCall)
/**
 * This function is called from a call's destructor.
 * It must delete the call from its list.  If the call cannot be found, an unacceptable
 * inconsistancy must have developed, and the TSY will panic.
 */
	{
	for(TInt i=0;i<iCalls->Count();i++)
		{
		if(iCalls->At(i)==aCall)
			{
			iCalls->Delete(i);
			return;
			}
		}
	SimPanic(EIllegalCallNotRegisteredWithLine);
	}

TBool CSimLine::IsAnswerCallObjectSpare()
/**
 * Is the "answer" call object actually the spare object.
 */
	{
	return (iSpareCall==iAnswerNextIncomingCall);
	}

/**
Called when iTimer has delayed the sending of the notification of the incoming call.
*/
void CSimLine::TimerCallBack(TInt /*aId*/)
	{
	ProcessNotifyIncomingCallEvent();
	}

/**
Send notification of an incoming call.
*/
void CSimLine::ProcessNotifyIncomingCallEvent()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_PROCESSNOTIFYINCOMINGCALLEVENT_1, ">>CSimLine::ProcessNotifyIncomingCallEvent");
	if(iNotifyIncomingCall.iNotifyPending)
		{
		iNotifyIncomingCall.iNotifyPending=EFalse;
		((TName*)iNotifyIncomingCall.iNotifyData)->Copy(iAnswerNextIncomingCall->iName);
		ReqCompleted(iNotifyIncomingCall.iNotifyHandle,KErrNone);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMLINE_PROCESSNOTIFYINCOMINGCALLEVENT_2, "<<CSimLine::ProcessNotifyIncomingCallEvent");
	}

