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

#include "mmtsy.h"
#include "testdef.h"
#include "ETELMM.H"
#include <et_clsvr.h>
#include "Dmmlog.h"


//
// CLineDMmTsy
//
CLineDMmTsy* CLineDMmTsy::NewL(CPhoneDMmTsy *aPhone,CPhoneFactoryDummyBase* aFac)
	{

	CLineDMmTsy* line=new(ELeave) CLineDMmTsy(aPhone,aFac);
	CleanupStack::PushL(line);
	line->ConstructL();
	CleanupStack::Pop();
	return line;
	}

CLineDMmTsy::CLineDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac)
	:CLineDummyBase(aFac), iPhone(aPhone)
	{}

void CLineDMmTsy::ConstructL()
	{
	CLineDummyBase::ConstructL();
	LOGTEXT(_L8("CLineDMmTsy created"));
	}
											  

CLineDMmTsy::~CLineDMmTsy()
	{
	LOGTEXT(_L8("CLineDMmTsy destroyed"));
	}


CTelObject* CLineDMmTsy::OpenNewObjectByNameL(const TDesC& aName)
	{
	// An existing call opened by a client - client provides name of call to open
	if((aName.Compare(DMMTSY_CALL_NAME1)==KErrNone) ||
	   (aName.Compare(DMMTSY_CALL_NAME2)==KErrNone) ||
	   (aName.Compare(DMMTSY_CALL_NAME3)==KErrNone))

		return REINTERPRET_CAST(CTelObject*,CCallDMmTsy::NewL(iPhone,FacPtr()));
	
	return NULL;
	}

CTelObject* CLineDMmTsy::OpenNewObjectL(TDes& aNewName)
	{
	// A new call opened by a client will have its name allocated by the TSY
	aNewName.Append(_L("Call"));
	aNewName.AppendNum(++iCallObjectCount);

	return CCallDMmTsy::NewL(iPhone,FacPtr());
	}


CTelObject::TReqMode CLineDMmTsy::ReqModeL(const TInt aIpc)
	{
	// ReqModeL is called from the server's CTelObject::ReqAnalyserL
	// in order to check the type of request it has

	// The following are example request types for this dummy TSY
	// All TSYs do not have to have these request types but they have been given
	// "sensible" values in this test code

	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// Non-flow control requests
//
	case EMobileLineGetMobileLineStatus:
		break;
//
// Notification Requests
//
	case EMobileLineNotifyMobileLineStatusChange:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Cancel Requests
//	
	case EMobileLineNotifyMobileLineStatusChangeCancel:
		User::Leave(KErrNotSupported);
		break;

	default:
		ret=CLineBase::ReqModeL(aIpc);
		break;
		}
	return ret;
	}


TInt CLineDMmTsy::RegisterNotification(const TInt aIpc)
	{
	// RegisterNotification is called when the server recognises that this notification
	// is being posted for the first time on this sub-session object.

	// It enables the TSY to "turn on" any regular notification messages that it may 
	// receive from the phone

	switch (aIpc)
		{
	case EMobileLineNotifyMobileLineStatusChange:
		LOGTEXT(_L8("CLineDMmTsy: RegisterNotification - Line Status Change "));
		return KErrNone;
	default:
		// Unknown or invalid sms IPC
		return KErrNotSupported;
		}
	}

TInt CLineDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	// DeregisterNotification is called when the server recognises that this notification
	// will not be posted again because the last client to have a handle on this sub-session
	// object has just closed the handle.

	// It enables the TSY to "turn off" any regular notification messages that it may 
	// receive from the phone

	switch (aIpc)
		{
	case EMobileLineNotifyMobileLineStatusChange:
		LOGTEXT(_L8("CLineDMmTsy: DeregisterNotification - Line Status Change"));
		return KErrNone;
	default:
		// Unknown or invalid sms IPC
		return KErrNotSupported;
		}
	}

TInt CLineDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	// NumberOfSlotsL is called by the server when it is registering a new notification
	// It enables the TSY to tell the server how many buffer slots to allocate for
	// "repost immediately" notifications that may trigger before clients collect them

	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobileLineNotifyMobileLineStatusChange:
		LOGTEXT(_L8("CLineDMmTsy: Registered with 5 slots"));
		numberOfSlots=5;
		break;
	default:
		// Unknown or invalid Phone IPC
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}

TInt CLineDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
						const TDataPackage& aPackage)
	{
	// ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
	// for the TSY to process
	// A request handle, request type and request data are passed to the TSY

	TAny* dataPtr=aPackage.Ptr1();

	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type
	
	switch (aIpc)
		{
//
// Non-flow control requests
//
	case EMobileLineGetMobileLineStatus:
		return GetMobileLineStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*, dataPtr));

//
// Notification Requests
//
	case EMobileLineNotifyMobileLineStatusChange:
		return NotifyMobileLineStatusChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*, dataPtr));

//
// Cancel Requests
//	
	case EMobileLineNotifyMobileLineStatusChangeCancel:
		return NotifyMobileLineStatusChangeCancel(aTsyReqHandle);

	default:
		return KErrNotSupported;
		}
	}

TInt CLineDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	// CancelService is called by the server when it is "cleaning-up" any still outstanding
	// asynchronous requests before closing a client's sub-session.
	// This will happen if a client closes its R-class handle without cancelling outstanding
	// asynchronous requests.

	switch (aIpc)
		{
//
// Cancel Requests
//	
	case EMobileLineNotifyMobileLineStatusChange:
		return NotifyMobileLineStatusChangeCancel(aTsyReqHandle);
	default:
		return CLineBase::CancelService(aIpc,aTsyReqHandle);
		}
	}

TInt CLineDMmTsy::GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aCaps)
	{
	//set the aux voice bit
	aCaps->iFlags |= DMMTSY_MOBILE_LINE_AUX_VOICE;

	ReqCompleted(aTsyReqHandle,KErrNone);
 	return KErrNone;
	}


/***********************************************************************************/
//
// The following methods are called from ExtFunc and/or CancelService.
// Each of these will process a TSY request or cancel a TSY request
// Here, example values are returned or checked within this dummy TSY in order to ensure
// that the integrity of the data passed to/from client is maintained
//
/***********************************************************************************/

TInt CLineDMmTsy::GetMobileLineStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus)
	{
	LOGTEXT(_L8("CLineDMmTsy::GetMobileLineStatus called"));
	*aStatus=DMMTSY_CALL_STATUS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDMmTsy::NotifyMobileLineStatusChange(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallStatus* aStatus)
	{
	if (!iNotifyMobileLineStatusChange++)
		{
		LOGTEXT(_L8("CLineDMmTsy::NotifyMobileLineStatusChange called"));
		*aStatus=DMMTSY_CALL_STATUS2;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CLineDMmTsy::NotifyMobileLineStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CLineDMmTsy::NotifyMobileLineStatusChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

