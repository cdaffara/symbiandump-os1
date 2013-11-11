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
// CONFERENCE.CPP
// 
//


#include "mmtsy.h"
#include "testdef.h"
#include "ETELMM.H"
#include <et_clsvr.h>
#include "Dmmlog.h"

CMobileConferenceCallDMmTsy* CMobileConferenceCallDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName)
	{
	CMobileConferenceCallDMmTsy* This=new(ELeave) CMobileConferenceCallDMmTsy(aPhone,aFac,aName);
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop();
	return This;
	}

CMobileConferenceCallDMmTsy::CMobileConferenceCallDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName) :
	CSubSessionExtDummyBase(aFac), iPhone(aPhone), iNotifyCapsChange(0),iNotifyConferenceStatusChange(0),
		iNotifyConferenceEvent(0)
	{
	iConferenceName = aName;
	}

void CMobileConferenceCallDMmTsy::ConstructL()
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy created"));
	}

CMobileConferenceCallDMmTsy::~CMobileConferenceCallDMmTsy()
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy destroyed"));
	}

CTelObject* CMobileConferenceCallDMmTsy::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CMobileConferenceCallDMmTsy::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


CTelObject::TReqMode CMobileConferenceCallDMmTsy::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{

		case EMobileConferenceCallGetCaps:
		case EMobileConferenceCallCreateConference:
		case EMobileConferenceCallAddCall:
		case EMobileConferenceCallSwap:
		case EMobileConferenceCallHangUp:
		case EMobileConferenceCallEnumerateCalls:
		case EMobileConferenceCallGetMobileCallInfo:
		case EMobileConferenceCallGetConferenceStatus:

		break;

//
// Flow Controlled Services
//

//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
		case EMobileConferenceCallNotifyCapsChange:
		case EMobileConferenceCallNotifyConferenceStatusChange:
		case EMobileConferenceCallNotifyConferenceEvent:

			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break;
//
// Cancel Requests
//	
		case EMobileConferenceCallNotifyCapsChangeCancel:
		case EMobileConferenceCallCreateConferenceCancel:
		case EMobileConferenceCallAddCallCancel:
		case EMobileConferenceCallSwapCancel:
		case EMobileConferenceCallHangUpCancel:
		case EMobileConferenceCallNotifyConferenceStatusChangeCancel:
		case EMobileConferenceCallNotifyConferenceEventCancel:
		default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;
	}

void CMobileConferenceCallDMmTsy::Init()
	{

	}

TInt CMobileConferenceCallDMmTsy::RegisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobileConferenceCallNotifyCapsChange:
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: RegisterNotification - Notify Caps Change "));
		return KErrNone;
	case EMobileConferenceCallNotifyConferenceStatusChange:
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: RegisterNotification - Notify Conference Status Change "));
		return KErrNone;
	case EMobileConferenceCallNotifyConferenceEvent:
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: RegisterNotification - Notify Conference Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CMobileConferenceCallDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobileConferenceCallNotifyCapsChange:
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: DeregisterNotification - Notify Caps Change "));
		return KErrNone;
	case EMobileConferenceCallNotifyConferenceStatusChange:
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: DeregisterNotification - Notify Conference Status Change "));
		return KErrNone;
	case EMobileConferenceCallNotifyConferenceEvent:
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: DeregisterNotification - Notify Conference Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CMobileConferenceCallDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobileConferenceCallNotifyCapsChange:
	case EMobileConferenceCallNotifyConferenceStatusChange:
	case EMobileConferenceCallNotifyConferenceEvent:
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: Registered with 2 slot"));
		numberOfSlots=2;
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		} 
	return numberOfSlots;
	}

TInt CMobileConferenceCallDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
	{
	TAny* dataPtr=aPackage.Ptr1();

	switch (aIpc)
		{
		case EMobileConferenceCallGetCaps:
			return GetCaps(aTsyReqHandle,REINTERPRET_CAST(TUint32*,dataPtr));
		case EMobileConferenceCallNotifyCapsChange:
			return NotifyCapsChange(aTsyReqHandle,REINTERPRET_CAST(TUint32*,dataPtr));
		case EMobileConferenceCallCreateConference:
			return CreateConference(aTsyReqHandle);
		case EMobileConferenceCallAddCall:
			return AddCall(aTsyReqHandle,aPackage.Des1u());
		case EMobileConferenceCallSwap:
			return Swap(aTsyReqHandle);
		case EMobileConferenceCallHangUp:
			return HangUp(aTsyReqHandle);
		case EMobileConferenceCallEnumerateCalls:
			return EnumerateCalls(aTsyReqHandle,REINTERPRET_CAST(TUint*,dataPtr));
		case EMobileConferenceCallGetMobileCallInfo:
			return GetMobileCallInfo(aTsyReqHandle,REINTERPRET_CAST(TInt*,dataPtr),aPackage.Des2n());
		case EMobileConferenceCallGetConferenceStatus:
			return GetConferenceStatus(aTsyReqHandle,REINTERPRET_CAST(RMobileConferenceCall::TMobileConferenceStatus*,dataPtr));
		case EMobileConferenceCallNotifyConferenceStatusChange:
			return NotifyConferenceStatusChange(aTsyReqHandle,REINTERPRET_CAST(RMobileConferenceCall::TMobileConferenceStatus*,dataPtr));
		case EMobileConferenceCallNotifyConferenceEvent:
			return NotifyConferenceEvent(aTsyReqHandle,REINTERPRET_CAST(RMobileConferenceCall::TMobileConferenceEvent*,dataPtr),aPackage.Des2u());

		default:
			return KErrGeneral;
		}
	}


TInt CMobileConferenceCallDMmTsy::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy: - CancelService called"));
	switch (aIpc)
		{
		case EMobileConferenceCallNotifyCapsChange:
			return NotifyCapsChangeCancel(aTsyReqHandle);
		case EMobileConferenceCallCreateConference:
			return CreateConferenceCancel(aTsyReqHandle);
		case EMobileConferenceCallAddCall:
			return AddCallCancel(aTsyReqHandle);
		case EMobileConferenceCallSwap:
			return SwapCancel(aTsyReqHandle);
		case EMobileConferenceCallHangUp:
			return HangUpCancel(aTsyReqHandle);
		case EMobileConferenceCallNotifyConferenceStatusChange:
			return NotifyConferenceStatusChangeCancel(aTsyReqHandle);
		case EMobileConferenceCallNotifyConferenceEvent:
			return NotifyConferenceEventCancel(aTsyReqHandle);

		default:
			return KErrGeneral;
		}
	}

TInt CMobileConferenceCallDMmTsy::GetCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::GetCaps Called"));
	*aCaps = DMMTSY_CONFERENCE_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	if (!iNotifyCapsChange++)
		{
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy::NotifyCapsChange Called"));
		*aCaps = DMMTSY_CONFERENCE_CAPS;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::NotifyCapsChangeCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::CreateConference(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::CreateConference Called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::CreateConferenceCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::CreateConferenceCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::AddCall(const TTsyReqHandle aTsyReqHandle, TDes* aCallName)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::AddCall Called"));
	if (aCallName->Compare(DMMTSY_CONFERENCE_CALL_NAME) != 0)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::AddCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::AddCallCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::Swap(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::Swap Called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::SwapCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::SwapCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::HangUp(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::HangUp Called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::HangUpCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::HangUpCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::EnumerateCalls(const TTsyReqHandle aTsyReqHandle, TUint* aCount)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::EnumerateCalls Called"));
	*aCount = DMMTSY_CONFERENCE_CALL_COUNT;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::GetMobileCallInfo(const TTsyReqHandle aTsyReqHandle, TInt *aIndex, TDes8* aCallInfo)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::GetMobileCallInfo Called"));
	if (*aIndex != DMMTSY_CONFERENCE_CALL_INDEX)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		RMobileCall::TMobileCallInfoV1Pckg* ciPckg = STATIC_CAST(RMobileCall::TMobileCallInfoV1Pckg*,aCallInfo);
		RMobileCall::TMobileCallInfoV1& ci = (*ciPckg)();

		ci.iCallName = DMMTSY_CONFERENCE_CALL_NAME;
		ci.iLineName = DMMTSY_CONFERENCE_CALL_LINE;

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::GetConferenceStatus(const TTsyReqHandle aTsyReqHandle, RMobileConferenceCall::TMobileConferenceStatus* aStatus)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::GetConferenceStatus Called"));
	*aStatus = DMMTSY_CONFERENCE_CALL_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::NotifyConferenceStatusChange(const TTsyReqHandle aTsyReqHandle, RMobileConferenceCall::TMobileConferenceStatus* aStatus)
	{
	if (!iNotifyConferenceStatusChange++)
		{
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy::NotifyConferenceStatusChange Called"));
		*aStatus = DMMTSY_CONFERENCE_CALL_STATUS;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::NotifyConferenceStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::NotifyConferenceStatusChangeCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CMobileConferenceCallDMmTsy::NotifyConferenceEvent(const TTsyReqHandle aTsyReqHandle, RMobileConferenceCall::TMobileConferenceEvent* aEvent, TDes* aCallName)
	{
	if (!iNotifyConferenceEvent++)
		{
		LOGTEXT(_L8("CMobileConferenceCallDMmTsy::NotifyConferenceEvent Called"));
		*aEvent = DMMTSY_CONFERENCE_CALL_EVENT;
		*aCallName = DMMTSY_CONFERENCE_CALL_NAME;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CMobileConferenceCallDMmTsy::NotifyConferenceEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileConferenceCallDMmTsy::NotifyConferenceEventCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

