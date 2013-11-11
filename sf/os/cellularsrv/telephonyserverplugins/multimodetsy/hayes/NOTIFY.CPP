// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "PHONE.H"
#include "LINE.H"
#include "CALL.H"
#include "mSLOGGER.H"
#include "mnetwork.h"
#include "GprsNotify.h"

//
//	CNotifications - handles array of notifications placed on TSY
//
CNotifications* CNotifications::NewL()
	{
	CNotifications* notifications=new(ELeave) CNotifications();
	CleanupStack::PushL(notifications);
	notifications->ConstructL();
	CleanupStack::Pop();
	return notifications;
	}

CNotifications::CNotifications()
	{}

void CNotifications::ConstructL()
	{
	iNotifications = new (ELeave) CArrayPtrFlat<CNotifyBase>(4);
	iLastEvents = new (ELeave) CArrayFixFlat<TLastEvent>(4);
	}

CNotifications::~CNotifications()
//
//	Notifications are removed by server prior to this
//
	{
	if(iNotifications)
		{
		__ASSERT_ALWAYS(iNotifications->Count()==0,Panic(ENotifications_Remaining));
		delete iNotifications;
		}
	if(iLastEvents)
		{
		__ASSERT_ALWAYS(iLastEvents->Count()==0,Panic(ELastEvents_Remaining));
		delete iLastEvents;
		}
	}

//
//	Last Events
//
//	The most recent event to occur must be stored in the notification handler to be compared
//  with the event passed with CheckNotification(), so that they can be compared (if they are 
//  equal, the notification should not be completed).
//	Each TelObject to use this Notification handler has a last event associated with it.
//	The first time a TelObject calls RegisterNotification() to add a notification to the array, 
//	an entry is made in the iLastEvents array connecting this TelObject with an event (to start
//  with this is ENoEvent).
//	When the notification completes, the LastEvent entry remains, as even if a TelObject has no
//	notifications outstanding there must still be a record of its last event. This LastEvent
//	entry must only be removed when the TelObject is destroyed, hence in the destructor of 
//	each of CPhoneHayes, CLineHayes, CCallHayes and CFaxHayes RemoveClientFromLastEvents() is
//  called.
//

TInt CNotifications::FindLastEvent(CTelObject* aTelObject)
//
// Finds the last event to occur to this TelObject
// Returns the position in the array
//
	{
	for (TInt i=0;i<iLastEvents->Count();i++)
		{
		if (iLastEvents->At(i).iTelObject==aTelObject)
			return i;
		}
	return KErrNotFound;
	}

TInt CNotifications::GetLastEvent(CTelObject* aTelObject, TLastEvent& aLastEvent)
//
//	Searches for the relevant Last Event entry by TelObject, returns the reference to entry
//  and its position
//
	{
	TInt pos = FindLastEvent(aTelObject);
	if (pos!=KErrNotFound)
		{
		aLastEvent = iLastEvents->At(pos);
		}
	return pos;
	}

TInt CNotifications::AddLastEvent(CTelObject* aTelObject,TEvent aLastEvent)
//
//	Searches for relevant entry. If exists, updates it with new event. If not, appends a new
//	entry to the array.
//
	{
	TLastEvent lastEvent;
	lastEvent.iLastEvent=aLastEvent;

	TInt pos = FindLastEvent(aTelObject);
	if (pos!=KErrNotFound)
		{
		iLastEvents->At(pos).iLastEvent=lastEvent.iLastEvent; // Update array element's iLastEvent.
		}
	else
		{	
		lastEvent.iTelObject=aTelObject;
		TRAPD(ret, iLastEvents->AppendL(lastEvent,1) );

		if (ret!=KErrNone)
			{
			LOGTEXT2(_L8("CNotifications::AddLastEvent Append failed with error %d"), ret);
			return ret; 
			}
		}

	return pos;
	}
	
//
//	public functions
//

void CNotifications::RemoveClientFromLastEvents(CTelObject* aTelObject)
//
//	The notification store needs to know when a TelObject is closed so it can remove its
//  entry in the Last Events list.
	{
	for (TInt i=0;i<iLastEvents->Count();i++)
		{
		if (iLastEvents->At(i).iTelObject==aTelObject)
			{
			iLastEvents->Delete(i);
			return;
			}
		}
	}

void CNotifications::RemoveEventFromLastEvents(TEvent aEvent)
//
// The removes all events of a particular type from the event list.  It is used when a line stops
// ringing to remove all ringing references.
//
	{
	for (TInt i=0;i<iLastEvents->Count();i++)
		{
		if (iLastEvents->At(i).iLastEvent==aEvent)
			{
			iLastEvents->Delete(i);
			return;
			}
		}
	}

void CNotifications::RegisterNotification(TNotifications aWhichNotification,TTsyReqHandle aTsyReqHandle,CTelObject* aTelObject,TAny* aParams)
	{
	CNotifyBase* newNotify = NULL;
	TInt ret=KErrNone;
	switch (aWhichNotification)
		{
	case EModemDetection:
		TRAP(ret,newNotify = CNotifyModemDetected::NewL((RPhone::TModemDetection*)aParams,aTsyReqHandle,aTelObject));
		break;
	case EIncomingCall:
		TRAP(ret,newNotify = CNotifyIncomingCall::NewL((TDes*)aParams,aTsyReqHandle,aTelObject));
		break;
	case ELineHookChange:
		TRAP(ret,newNotify = CNotifyLineHookChange::NewL((RCall::THookStatus*)aParams,aTsyReqHandle,aTelObject));
		break;
	case ECallHookChange:
		TRAP(ret,newNotify = CNotifyCallHookChange::NewL((RCall::THookStatus*)aParams,aTsyReqHandle,aTelObject));
		break;
	case ELineStatusChange:
		TRAP(ret,newNotify = CNotifyLineStatusChange::NewL((RCall::TStatus*)aParams,aTsyReqHandle,aTelObject));
		break;
	case EMobileLineStatusChange:
		TRAP(ret,newNotify = CNotifyMobileLineStatusChange::NewL((RMobileCall::TMobileCallStatus*)aParams,aTsyReqHandle,aTelObject));
		break;
	case ECallDurationChange:
		TRAP(ret,newNotify = CNotifyCallDurationChange::NewL((TTimeIntervalSeconds*)aParams,aTsyReqHandle,aTelObject));
		break;
	case ENewCallAdded:
		TRAP(ret,newNotify = CNotifyLineNewCallAdded::NewL((TDes*)aParams,aTsyReqHandle,aTelObject));
		break;
	case ECallStatusChange:
		TRAP(ret,newNotify = CNotifyCallStatusChange::NewL((RCall::TStatus*)aParams,aTsyReqHandle,aTelObject));
		break;
	case EMobileCallStatusChange:
		TRAP(ret,newNotify = CNotifyMobileCallStatusChange::NewL((RMobileCall::TMobileCallStatus*)aParams,aTsyReqHandle,aTelObject));
		break;
	case EReadOrWriteFax:
		TRAP(ret,newNotify = CNotifyFaxReadOrWrite::NewL(aTsyReqHandle,aTelObject));
		break;
	case EEndOfFaxPage:
		TRAP(ret,newNotify = CNotifyFaxEndOfPage::NewL(aTsyReqHandle,aTelObject));
		break;
	case ECallCaps:
		TRAP(ret,newNotify = CNotifyCallCaps::NewL((RCall::TCaps*)aParams,aTsyReqHandle,aTelObject));
		break;
	case ECallMobileCaps:
		TRAP(ret,newNotify = CNotifyMobileCallCaps::NewL((TDes8*)aParams,aTsyReqHandle,aTelObject));
		break;
	case ERegistrationStatus:
		TRAP(ret,newNotify = CNotifyNetworkRegistrationStatusChange::NewL(aTsyReqHandle,aTelObject,(RMobilePhone::TMobilePhoneRegistrationStatus*)aParams));
		break;
	case ECurrentNetwork:
		TRAP(ret,newNotify = CNotifyCurrentNetworkChange::NewL(aTsyReqHandle,aTelObject,(TInt*)aParams)); 
		break;

	// GPRS
	case EPacketContextAdded:
		TRAP(ret,newNotify = CGprsContextAddedNotify::NewL(aTsyReqHandle,aTelObject,(TDes*)aParams));
		break; 
	case EPacketStatusChange:
		TRAP(ret,newNotify = CGprsStatusNotify::NewL(aTsyReqHandle,aTelObject,(RPacketService::TStatus*)aParams));
		break;
	case EPacketNtwkRegStatusChange:
		TRAP(ret,newNotify = CGprsNtwkRegStatusChangeNotify::NewL(aTsyReqHandle,aTelObject,(RPacketService::TRegistrationStatus*)aParams));
		break;

	// GPRS context
	case EPacketContextConfigChange:
		TRAP(ret,newNotify = CGprsContextConfigNotify::NewL(aTsyReqHandle,aTelObject,(TDes8*)aParams));
		break;
	case EPacketContextStatusChange:
		TRAP(ret,newNotify = CGprsContextStatusNotify::NewL(aTsyReqHandle,aTelObject,(RPacketContext::TContextStatus*)aParams));
		break;

	// GPRS QoS
	case EPacketQoSProfileChange:
		TRAP(ret,newNotify = CGprsQoSProfileNotify::NewL(aTsyReqHandle, aTelObject, (TDes8*)aParams));
		break;

	default:
		Panic(EIllegalEvent);
		break;
		}
	if (ret!=KErrNone)
		{
		aTelObject->ReqCompleted(aTsyReqHandle,ret);
		delete newNotify;
		}
	else
		{
		TRAP(ret,RegisterNotificationL(newNotify));
		if (ret!=KErrNone)
			aTelObject->ReqCompleted(aTsyReqHandle,ret);
		}
	}

void CNotifications::RegisterNotificationL(CNotifyBase* aNotify)
	{
	iNotifications->AppendL(aNotify);
	TInt pos = FindLastEvent(aNotify->TelObject());
	if (pos==KErrNotFound)
		{
		TLastEvent lastEvent;
		lastEvent.iTelObject=aNotify->TelObject();
		lastEvent.iLastEvent=ENoEvent;
		iLastEvents->AppendL(lastEvent,1);
		}
	}

void CNotifications::CheckNotification(CTelObject* aTelObject,TEvent aEvent)
//
//	Due to the possibility of immediate re-posting of notification from server which may
//	initiate another CheckNotification synchronously causing the problem that the last event
//  has not yet been set to current event - an infinite loop occurs. Fixed here by setting
//  a boolean on entry and resetting on exit
//
	{
	if (iAlreadyChecking)
		return;
	iAlreadyChecking = ETrue;
	TLastEvent lastEvent;
	TInt ret = GetLastEvent(aTelObject,lastEvent);
	if (ret==KErrNotFound)
		lastEvent.iLastEvent=ENoEvent;	// to be passed to each notify object
	TBool flag=EFalse;
	for (TInt i=iNotifications->Count(); i>0; i--)
		{
		CNotifyBase* notify = iNotifications->At(i-1);
		if(notify->CheckAndCompleteNotification(aTelObject,aEvent,lastEvent.iLastEvent))
			{
			iNotifications->Delete(i-1);
			delete notify;
			flag=ETrue;
			}
		}
	if (flag)
		iNotifications->Compress();
	ret = AddLastEvent(aTelObject,aEvent);
	__ASSERT_ALWAYS( ret == KErrNotFound  || ret >= 0,Panic(EGeneral));
	iAlreadyChecking = EFalse;
	}

void CNotifications::CheckNotification(CCallBase* aCallObject,TEvent aEvent)
	{
	if (iAlreadyChecking)
		return;
	iAlreadyChecking = ETrue;
	TLastEvent lastEvent;
	TInt ret = GetLastEvent(aCallObject,lastEvent);
	if (ret==KErrNotFound)
		lastEvent.iLastEvent=ENoEvent;
	TBool flag=EFalse;
	for (TInt i=iNotifications->Count(); i>0; i--)
		{
		CNotifyBase* notify = iNotifications->At(i-1);
		if (notify->CheckAndCompleteNotification(aCallObject,aEvent,lastEvent.iLastEvent))
			{
			iNotifications->Delete(i-1);
			delete notify;
			flag=ETrue;
			}
		}
	if (flag)
		iNotifications->Compress();
	ret = AddLastEvent(aCallObject,aEvent);
	__ASSERT_ALWAYS( ret == KErrNotFound  || ret >= 0,Panic(EGeneral));
	iAlreadyChecking = EFalse;
	}

void CNotifications::CheckNotification(CFaxSession* aETelFaxObject,TEvent aEvent,TInt aError,TAny* aParams)
	{
	iAlreadyChecking = ETrue;
	TBool flag=EFalse;
	for (TInt i=iNotifications->Count(); i>0; i--)
		{
		CNotifyBase* notify = iNotifications->At(i-1);
		if (notify->CheckAndCompleteNotification(aETelFaxObject,aEvent,aError,aParams))
			{
			iNotifications->Delete(i-1);
			delete notify;
			flag=ETrue;
			}
		}
	if (flag)
		iNotifications->Compress();
	iAlreadyChecking = EFalse;
	}

//
//
// This function is added to deal specifically with notifications only (ie: no events)
// An incoming SMS message notification for example!
//
//
void CNotifications::CheckNotification(CTelObject* aTelObject, TNotifications aNotification)
	{
	if (iAlreadyChecking)
		return;
	iAlreadyChecking = ETrue;
	TBool flag = EFalse;
	for (TInt i = 0; i<iNotifications->Count(); i++)
		{
		CNotifyBase* notify = iNotifications->At(i);
		if (notify->CheckAndCompleteNotification(aTelObject, aNotification))
			{
			iNotifications->Delete(i);
			delete notify;
			flag = ETrue;
			} 
		} 
	if (flag)
		iNotifications->Compress();
	iAlreadyChecking = EFalse;
}

void CNotifications::RemoveNotification(TTsyReqHandle aTsyReqHandle)
//
//	Cancel a notification by its TsyReqHandle (unique to that notification)
//
	{
	for (TInt i=0;i<iNotifications->Count();i++)
		{
		CNotifyBase* notify = iNotifications->At(i);
		if (notify->TsyReqHandle() == aTsyReqHandle)
			{
			notify->TelObject()->ReqCompleted(notify->TsyReqHandle(),KErrCancel);
			iNotifications->Delete(i);
			delete notify;
			break;
			}
		}
	}

void CNotifications::CompleteNotificationsWithError(TInt aError)
	{
	for (TInt i=0;i<iNotifications->Count();i++)
		{
		CNotifyBase* notify = iNotifications->At(0);
		notify->TelObject()->ReqCompleted(notify->TsyReqHandle(),aError);
		iNotifications->Delete(0);
		delete notify;
		}
	}
	
//
//	CNotifyBase class - base class for all notifications
//

CNotifyBase::CNotifyBase(TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	: iReqHandle(aReqHandle),iTelObject(aTelObject)
	{}

CNotifyBase::~CNotifyBase()
	{}

TTsyReqHandle CNotifyBase::TsyReqHandle()
	{
	return iReqHandle;
	}

CTelObject* CNotifyBase::TelObject()
	{
	return iTelObject;
	}

TBool CNotifyBase::CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent /*aEvent*/,TEvent /*aLastEvent*/)
	{
	return EFalse;
	}
	
TBool CNotifyBase::CheckAndCompleteNotification(CCallBase* /*aCallObject*/,TEvent /*aEvent*/,TEvent /*aLastEvent*/)
	{
	return EFalse;
	}

TBool CNotifyBase::CheckAndCompleteNotification(CFaxSession* /*aETelFaxObject*/,TEvent /*aEvent*/,TInt /*aError*/,TAny* /*aParams*/)
	{
	return EFalse;
	}

TBool CNotifyBase::CheckAndCompleteNotification(CTelObject* /*aTelObject*/, TNotifications /*aNotification*/)
	{
	return EFalse;
	}

//
// CNotifyPhoneDetection
//
CNotifyModemDetected* CNotifyModemDetected::NewL(RPhone::TModemDetection* aDetection,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyModemDetected(aDetection,aReqHandle,aTelObject);
	}

CNotifyModemDetected::CNotifyModemDetected(RPhone::TModemDetection* aDetection,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iDetection(aDetection)
	{}

CNotifyModemDetected::~CNotifyModemDetected()
	{}

TBool CNotifyModemDetected::CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent aEvent,TEvent aLastEvent)
//
//	After phone has been in the "Not detected" state, the next state should be "Detected"
//  or a "Ring Occurred"
//  
	{
	if ((aEvent==ERingOccurred || aEvent==EPhoneDetected)
		&&(aLastEvent==EPhoneNotDetected || aLastEvent==ENoEvent))
		{
		*iDetection=RPhone::EDetectedPresent;
		}
	else if (aEvent==EPhoneNotDetected && aLastEvent!=EPhoneNotDetected)
		{
		*iDetection=RPhone::EDetectedNotPresent;
		}
	else return EFalse;
	LOGTEXT2(_L8("Event %d:\tModem Detection Change Notification completed"),aEvent);
	iTelObject->ReqCompleted(iReqHandle,KErrNone);
	return ETrue;
	}

TBool CNotifyModemDetected::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent)
	{
	return CheckAndCompleteNotification(STATIC_CAST(CTelObject*,aCallObject),aEvent,aLastEvent);
	}
//
// CNotifyIncomingCall
//
CNotifyIncomingCall* CNotifyIncomingCall::NewL(TDes* aName,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyIncomingCall(aName,aReqHandle,aTelObject);
	}

CNotifyIncomingCall::CNotifyIncomingCall(TDes* aName,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iName(aName)
	{}

CNotifyIncomingCall::~CNotifyIncomingCall()
	{}

TBool CNotifyIncomingCall::CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent)
//
//	Checks that the last event to happen was not an incoming call. 
//  Adv: If the first Notification for an incoming call is placed after the line started ringing,
//		 it can check the line status and complete immediately.
	{
	if (aEvent==ERingOccurred && aLastEvent!=ERingOccurred
		&& aTelObject==iTelObject)
		{
		CLineHayes* line = STATIC_CAST(CLineHayes*,iTelObject);
		line->GetNameOfCallForAnswering(*iName);
		line->ResetNotifyIncomingCall();	
		LOGTEXT2(_L8("Event %d:\tIncoming Call Notification completed"),aEvent);
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}

TBool CNotifyIncomingCall::CheckAndCompleteNotification(CCallBase* aCallBase,TEvent aEvent,TEvent aLastEvent)
	{
	return CheckAndCompleteNotification(REINTERPRET_CAST(CLineBase*,aCallBase->Owner()),aEvent,aLastEvent);
	}

//
// CNotifyLineHookChange
//
CNotifyLineHookChange* CNotifyLineHookChange::NewL(RCall::THookStatus* aHookStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyLineHookChange(aHookStatus,aReqHandle,aTelObject);
	}

CNotifyLineHookChange::CNotifyLineHookChange(RCall::THookStatus* aHookStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iHookStatus(aHookStatus)
	{}

CNotifyLineHookChange::~CNotifyLineHookChange()
	{}

TBool CNotifyLineHookChange::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent)
//
//	Line hook can only change if a call-owned object (eg ATDial) is in progress
//
	{
	if (iTelObject != REINTERPRET_CAST(CLineHayes*,aCallObject->Owner()))
		return EFalse;
	if (aEvent==EBegunConnecting && aLastEvent!=EBegunConnecting)
		*iHookStatus = RCall::EHookStatusOff;
	else if (aEvent==EBecomeIdle && aLastEvent!=EBecomeIdle)
		*iHookStatus = RCall::EHookStatusOn;
	else 
		return EFalse;
	LOGTEXT2(_L8("Event %d:\tLine Hook Change Notification completed"),aEvent);
	iTelObject->ReqCompleted(iReqHandle,KErrNone);
	return ETrue;
	}
//
// CNotifyCallHookChange
//
CNotifyCallHookChange* CNotifyCallHookChange::NewL(RCall::THookStatus* aHookStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyCallHookChange(aHookStatus,aReqHandle,aTelObject);
	}
	
CNotifyCallHookChange::CNotifyCallHookChange(RCall::THookStatus* aHookStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iHookStatus(aHookStatus)
	{}

CNotifyCallHookChange::~CNotifyCallHookChange()
	{}

TBool CNotifyCallHookChange::CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent aEvent,TEvent aLastEvent)
	{
	if (aEvent==EBegunConnecting && aLastEvent!=EBegunConnecting)
		*iHookStatus = RCall::EHookStatusOff;
	else if (aEvent==EBecomeIdle && aLastEvent!=EBecomeIdle)
		*iHookStatus = RCall::EHookStatusOn;
	else 
		return EFalse;
	LOGTEXT2(_L8("Event %d:\tCall Hook Change Notification completed"),aEvent);
	iTelObject->ReqCompleted(iReqHandle,KErrNone);
	return ETrue;
	}

TBool CNotifyCallHookChange::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent)
	{
	if (iTelObject != aCallObject)
		return EFalse;
	return CheckAndCompleteNotification(STATIC_CAST(CTelObject*,aCallObject),aEvent,aLastEvent);
	}
//
// CNotifyLineStatusChange
//
CNotifyLineStatusChange* CNotifyLineStatusChange::NewL(RCall::TStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyLineStatusChange(aStatus,aReqHandle,aTelObject);
	}

CNotifyLineStatusChange::CNotifyLineStatusChange(RCall::TStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iStatus(aStatus)
	{}
	
CNotifyLineStatusChange::~CNotifyLineStatusChange()
	{}

TBool CNotifyLineStatusChange::CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent aEvent,TEvent aLastEvent)
	{
	if ((aEvent==ERingOccurred && aLastEvent!=ERingOccurred) ||
		(aEvent==EBegunConnecting && aLastEvent!=EBegunConnecting) ||
		(aEvent==EConnected && aLastEvent!=EConnected) ||
		(aEvent==EBegunHangingUp && aLastEvent!=EBegunHangingUp) ||
		(aEvent==EBecomeIdle && aLastEvent!=EBecomeIdle))
		{
		REINTERPRET_CAST(CLineHayes*,iTelObject)->GetLineStatus(*iStatus);
		LOGTEXT2(_L8("Event %d:\tLine Status change Notification completed"),aEvent);
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}

TBool CNotifyLineStatusChange::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent)
	{
	if (iTelObject == REINTERPRET_CAST(CLineHayes*,aCallObject->Owner()))
		return CheckAndCompleteNotification(STATIC_CAST(CTelObject*,aCallObject),aEvent,aLastEvent);
	else 
		return EFalse;
	}


//
// CNotifyMobileLineStatusChange
//
CNotifyMobileLineStatusChange* CNotifyMobileLineStatusChange::NewL(RMobileCall::TMobileCallStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyMobileLineStatusChange(aStatus,aReqHandle,aTelObject);
	}

CNotifyMobileLineStatusChange::CNotifyMobileLineStatusChange(RMobileCall::TMobileCallStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iStatus(aStatus)
	{}
	
CNotifyMobileLineStatusChange::~CNotifyMobileLineStatusChange()
	{}

TBool CNotifyMobileLineStatusChange::CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent aEvent,TEvent aLastEvent)
	{
	if ((aEvent==ERingOccurred && aLastEvent!=ERingOccurred) ||
		(aEvent==EBegunConnecting && aLastEvent!=EBegunConnecting) ||
		(aEvent==EConnected && aLastEvent!=EConnected) ||
		(aEvent==EBegunHangingUp && aLastEvent!=EBegunHangingUp) ||
		(aEvent==EBecomeIdle && aLastEvent!=EBecomeIdle))
		{
		RCall::TStatus coreStatus;
		REINTERPRET_CAST(CLineHayes*,iTelObject)->GetLineStatus(coreStatus);
		//*iStatus = static_cast<RMobileCall::TMobileCallStatus>(coreStatus);
		*iStatus = (RMobileCall::TMobileCallStatus)coreStatus;
		LOGTEXT2(_L8("Event %d:\tMobile Line Status Change Notification completed"),aEvent);
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}

TBool CNotifyMobileLineStatusChange::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent)
	{
	if (iTelObject == REINTERPRET_CAST(CLineHayes*,aCallObject->Owner()))
		return CheckAndCompleteNotification(STATIC_CAST(CTelObject*,aCallObject),aEvent,aLastEvent);
	else 
		return EFalse;
	}

//
// CNotifyLineNewCallAdded
//
CNotifyLineNewCallAdded* CNotifyLineNewCallAdded::NewL(TDes* aName,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyLineNewCallAdded(aName,aReqHandle,aTelObject);
	}

CNotifyLineNewCallAdded::CNotifyLineNewCallAdded(TDes* aName,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iName(aName)
	{}
	
CNotifyLineNewCallAdded::~CNotifyLineNewCallAdded()
	{}

TBool CNotifyLineNewCallAdded::CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent /*aLastEvent*/)
//
//	No need to check here whether the last event for this TelObject was Call Added, because this
//	notification should complete every time a new call is added, irrespective of what has
//	happened inbetween times.
//
	{
	if (aEvent==ECallAdded && iTelObject==aTelObject)
		{
		REINTERPRET_CAST(CLineHayes*,iTelObject)->GetLastCallName(*iName);
		LOGTEXT2(_L8("Event %d:\tNew Call Added Notification completed"),aEvent);
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}

//
// CNotifyCallStatusChange
//
CNotifyCallStatusChange* CNotifyCallStatusChange::NewL(RCall::TStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyCallStatusChange(aStatus,aReqHandle,aTelObject);
	}

CNotifyCallStatusChange::CNotifyCallStatusChange(RCall::TStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iStatus(aStatus)
	{}

CNotifyCallStatusChange::~CNotifyCallStatusChange()
	{}

/*
TBool CNotifyCallStatusChange::CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent)
//
//	All calls are set ringing if a RING occurs
//
	{
	if (aEvent==ERingOccurred && aLastEvent!=ERingOccurred) 
		{	
		*iStatus = RCall::EStatusRinging;
		LOGTEXT2(_L8("Event %d:\tCall Status Change Notification completed from non-call-derived object"),aEvent);
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}
*/

TBool CNotifyCallStatusChange::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent)
	{
	if ((aEvent==ERingOccurred && aLastEvent!=ERingOccurred) ||
		(aEvent==EBegunConnecting && aLastEvent!=EBegunConnecting) ||
		(aEvent==EConnected && aLastEvent!=EConnected) ||
		(aEvent==EBegunHangingUp && aLastEvent!=EBegunHangingUp) ||
		(aEvent==EBecomeIdle && aLastEvent!=EBecomeIdle))
		{
		if (iTelObject == aCallObject)
					// correct call
			{
			LOGTEXT2(_L8("Event %d:\tCall Status Change Notification completed"),aEvent);
			RMobileCall::TMobileCallStatus callStatus = REINTERPRET_CAST(CCallHayes*,aCallObject)->CallInfo()->iMobileStatus;
			*iStatus = (RCall::TStatus)callStatus;  // should really call a proper conversion function here
			iTelObject->ReqCompleted(iReqHandle,KErrNone);
			return ETrue;
			}
		}
	return EFalse;
	}


//
// CNotifyMobileCallStatusChange
//
CNotifyMobileCallStatusChange* CNotifyMobileCallStatusChange::NewL(RMobileCall::TMobileCallStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyMobileCallStatusChange(aStatus,aReqHandle,aTelObject);
	}

CNotifyMobileCallStatusChange::CNotifyMobileCallStatusChange(RMobileCall::TMobileCallStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iStatus(aStatus)
	{}

CNotifyMobileCallStatusChange::~CNotifyMobileCallStatusChange()
	{}

TBool CNotifyMobileCallStatusChange::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent)
	{
	if ((aEvent==ERingOccurred && aLastEvent!=ERingOccurred) ||
		(aEvent==EBegunConnecting && aLastEvent!=EBegunConnecting) ||
		(aEvent==EConnected && aLastEvent!=EConnected) ||
		(aEvent==EBegunHangingUp && aLastEvent!=EBegunHangingUp) ||
		(aEvent==EBecomeIdle && aLastEvent!=EBecomeIdle))
		{
		if (iTelObject == aCallObject)
					// correct call
			{
			LOGTEXT2(_L8("Event %d:\tNotify Mobile Call Status Change completed"),aEvent);
			*iStatus = REINTERPRET_CAST(CCallHayes*,aCallObject)->CallInfo()->iMobileStatus;
			iTelObject->ReqCompleted(iReqHandle,KErrNone);
			return ETrue;
			}
		}
	return EFalse;
	}

//
// CNotifyCallDurationChange
//
CNotifyCallDurationChange* CNotifyCallDurationChange::NewL(TTimeIntervalSeconds* aTime,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyCallDurationChange(aTime,aReqHandle,aTelObject);
	}
	
CNotifyCallDurationChange::CNotifyCallDurationChange(TTimeIntervalSeconds* aTime,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iTime(aTime)
	{}

CNotifyCallDurationChange::~CNotifyCallDurationChange()
	{}

TBool CNotifyCallDurationChange::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent /*aLastEvent*/)
	{
	if (iTelObject != aCallObject)
		return EFalse;
	if (aEvent==ETimePeriodElapsed)
		{
		REINTERPRET_CAST(CCallHayes*,aCallObject)->GetCallDuration(*iTime);
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}

//
// CNotifyCallCaps 
//
CNotifyCallCaps* CNotifyCallCaps::NewL(RCall::TCaps* aCaps,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyCallCaps(aCaps,aReqHandle,aTelObject);
	}
	
CNotifyCallCaps::CNotifyCallCaps(RCall::TCaps* aCaps,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject),iCaps(aCaps)
	{}

CNotifyCallCaps::~CNotifyCallCaps()
	{}

TBool CNotifyCallCaps::CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent aEvent,TEvent aLastEvent)
//
// Dynamic call caps change when :
//	(a) Call starts to connect - cannot dial/answer/hangup
//	(b) Call has connected - cannot dial/answer. Can hangup/loan data port
//	(c) Data port is loaned - cannot dial/answer/hang up/loan dataport. Can Recover data port.
//	(d) Call begins to hang up - cannot do anything.
//	(e) Call is in idle state - depends if any other call is being used to connect.
//
	{
	if (aEvent==aLastEvent && aEvent!=ECallAdded)
		return EFalse;
	if (aEvent==EPhoneDetected || aEvent==EPhoneNotDetected ||
		aEvent==EBegunConnecting || aEvent==EConnected ||
		aEvent==EBegunHangingUp || aEvent==EBecomeIdle ||
		aEvent==EDataPortLoaned || aEvent==EDataPortRecovered)
		{
		TBool changed = REINTERPRET_CAST(CCallHayes*,iTelObject)->CollateCurrentCoreCaps(iReqHandle, reinterpret_cast<TUint32*>(&iCaps->iFlags));
		if (changed)
			{
			iTelObject->ReqCompleted(iReqHandle,KErrNone);
			return ETrue;
			}
		}
	return EFalse;
	}

TBool CNotifyCallCaps::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent)
	{
//	if (iTelObject == aCallObject)
		return CheckAndCompleteNotification(STATIC_CAST(CTelObject*,aCallObject),aEvent,aLastEvent);
//	else 
//		return EFalse;
	}

//
// CNotifyMobileCallCaps 
//
CNotifyMobileCallCaps* CNotifyMobileCallCaps::NewL(TDes8* aCaps,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyMobileCallCaps(aCaps,aReqHandle,aTelObject);
	}
	
CNotifyMobileCallCaps::CNotifyMobileCallCaps(TDes8* aCaps,TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject)
	{
	iCapsPckg = REINTERPRET_CAST(RMobileCall::TMobileCallCapsV1Pckg *,aCaps);
	}

CNotifyMobileCallCaps::~CNotifyMobileCallCaps()
	{}

TBool CNotifyMobileCallCaps::CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent aEvent,TEvent aLastEvent)
//
// Dynamic call caps change when :
//	(a) Call starts to connect - cannot dial/answer/hangup
//	(b) Call has connected - cannot dial/answer. Can hangup/loan data port
//	(c) Data port is loaned - cannot dial/answer/hang up/loan dataport. Can Recover data port.
//	(d) Call begins to hang up - cannot do anything.
//	(e) Call is in idle state - depends if any other call is being used to connect.
//
	{
	if (aEvent==aLastEvent && aEvent!=ECallAdded)
		return EFalse;
	if (aEvent==EPhoneDetected || aEvent==EPhoneNotDetected ||
		aEvent==EBegunConnecting || aEvent==EConnected ||
		aEvent==EBegunHangingUp || aEvent==EBecomeIdle ||
		aEvent==EDataPortLoaned || aEvent==EDataPortRecovered)
		{
		RMobileCall::TMobileCallCapsV1& caps = (*iCapsPckg)();
		TBool changed = REINTERPRET_CAST(CCallHayes*,iTelObject)->CollateCurrentMobileCaps(iReqHandle, &(caps.iCallControlCaps));
		if (changed)
			{
			iTelObject->ReqCompleted(iReqHandle,KErrNone);
			return ETrue;
			}
		}
	return EFalse;
	}

TBool CNotifyMobileCallCaps::CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent)
	{
	return CheckAndCompleteNotification(STATIC_CAST(CTelObject*,aCallObject),aEvent,aLastEvent);
	}

//
// CNotifyFaxReadOrWrite 
//
CNotifyFaxReadOrWrite* CNotifyFaxReadOrWrite::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyFaxReadOrWrite(aReqHandle,aTelObject);
	}

CNotifyFaxReadOrWrite::CNotifyFaxReadOrWrite(TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject)
	{}

CNotifyFaxReadOrWrite::~CNotifyFaxReadOrWrite()
	{}

TBool CNotifyFaxReadOrWrite::CheckAndCompleteNotification(CFaxSession* /*aETelFaxObject*/,TEvent aEvent,TInt aError,TAny* /*aParams*/)
	{
	if (aEvent==EFaxReadOrWriteCompleted || aEvent==EFaxSessionTerminated)
		{
		iTelObject->ReqCompleted(iReqHandle,aError);
		return ETrue;
		}
	return EFalse;
	}

//
// CNotifyEndOfFaxPage
//
CNotifyFaxEndOfPage* CNotifyFaxEndOfPage::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	{
	return new(ELeave) CNotifyFaxEndOfPage(aReqHandle,aTelObject);
	}

CNotifyFaxEndOfPage::CNotifyFaxEndOfPage(TTsyReqHandle aReqHandle,CTelObject* aTelObject)
	 : CNotifyBase(aReqHandle,aTelObject)
	{}

CNotifyFaxEndOfPage::~CNotifyFaxEndOfPage()
	{}

TBool CNotifyFaxEndOfPage::CheckAndCompleteNotification(CFaxSession* /*aETelFaxObject*/,TEvent aEvent,TInt aError,TAny* /*aParams*/)
	{
	if (aEvent==EEndOfFaxPageCompleted || aEvent==EFaxSessionTerminated)
		{
		LOGTEXT2(_L8("Event %d:\tFax End Of Page Notification completed"),aEvent);
		iTelObject->ReqCompleted(iReqHandle,aError);
		return ETrue;
		}
	return EFalse;
	}

	
