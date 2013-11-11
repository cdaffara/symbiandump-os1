// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dummy TSY testing functionality for Smart Card Application EAP support.
// 
//

/**
 @file
*/


#include "mmtsy.h"
#include "testdef.h"
#include "ETELMM.H"
#include <et_clsvr.h>
#include "Dmmlog.h"
#include <etelext.h>

//
//	CSmartCardEapDMmTsy
//
CSmartCardEapDMmTsy* CSmartCardEapDMmTsy::NewL(CPhoneDMmTsy *aPhone, RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType, CPhoneFactoryDummyBase* aFac)
	{
	CSmartCardEapDMmTsy* phone=new(ELeave) CSmartCardEapDMmTsy(aPhone, aAID, aEapType, aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CSmartCardEapDMmTsy::CSmartCardEapDMmTsy(CPhoneDMmTsy *aPhone, RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType, CPhoneFactoryDummyBase* aFac)
: CSubSessionExtDummyBase(aFac), iPhone(aPhone), iSSInitialised(EFalse),
  iAccessStatus(RMobileSmartCardEap::EEapMethodAvailable), iCliTerminationNotifier(NULL)
	{
	iAID = aAID;
	iEapType = aEapType;
	}

void CSmartCardEapDMmTsy::ConstructL()
	{
	iSemaphr.CreateGlobal(KNullDesC, EOwnerThread);
	LOGTEXT(_L8("CSmartCardEapDMmTsy created"));
	}

CSmartCardEapDMmTsy::~CSmartCardEapDMmTsy()
	{
	delete iCliTerminationNotifier;
	iSemaphr.Close();
	LOGTEXT(_L8("CSmartCardEapDMmTsy destroyed"));
	}

void CSmartCardEapDMmTsy::Init()
	{
	// server calls this function once it has created the sub-session
	// it gives the TSY chance to do any initialisation it may need to do for
	// this sub-session
	}

CTelObject* CSmartCardEapDMmTsy::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	// Server calls this function when a client is opening an object from the phone
	// for the first time.
	// Multiple clients opening handles to the same sub-session object will be dealt with
	// by the server - i.e. by reference counting
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSmartCardEapDMmTsy::OpenNewObjectL(TDes& /*aNewName*/)
	{
	// all objects opened from the phone are opened by name, hence this method
	// is not supported
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSmartCardEapDMmTsy::ReqModeL(const TInt aIpc)
	{
	// ReqModeL is called from the server's CTelObject::ReqAnalyserL
	// in order to check the type of request it has

	// The following are example request types for this dummy TSY
	// All TSYs do not have to have these request types but they have been given
	// "sensible" values in this test code

	CTelObject::TReqMode ret = 0;

	switch (aIpc)
		{
	// Non flow-controlled requests
	case EMobileSmartCardEapInitialiseEapMethod:
	case EMobileSmartCardEapGetUserIdentity:
	case EMobileSmartCardEapGetAuthenticationStatus:
	case EMobileSmartCardEapGetEapKey:
	case EMobileSmartCardEapAuthenticationPhase1:
	case EMobileSmartCardEapAuthenticationPhase2:
	case EMobileSmartCardEapReleaseEapMethod:
	case EMobileSmartCardEapGetEapMethodAccessStatus:
		break;

	case EMobileSmartCardEapNotifyEapMethodAccessStatusChange:
		ret = KReqModeMultipleCompletionEnabled;
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CSmartCardEapDMmTsy::RegisterNotification(const TInt /*aIpc*/)
	{
	// RegisterNotification is called when the server recognises that this notification
	// is being posted for the first time on this sub-session object.

	// It enables the TSY to "turn on" any regular notification messages that it may 
	// receive from the phone

	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DeregisterNotification(const TInt /*aIpc*/)
	{
	// DeregisterNotification is called when the server recognises that this notification
	// will not be posted again because the last client to have a handle on this sub-session
	// object has just closed the handle.

	// It enables the TSY to "turn off" any regular notification messages that it may
	// receive from the phone

	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	// NumberOfSlotsL is called by the server when it is registering a new notification
	// It enables the TSY to tell the server how many buffer slots to allocate for
	// "repost immediately" notifications that may trigger before clients collect them

	TInt numberOfSlots = 1;

	switch (aIpc)
		{
	case EMobileSmartCardEapNotifyEapMethodAccessStatusChange:
		numberOfSlots = 3;
		break;

	default:
		// Unknown or invalid Phone IPC
		User::Leave(KErrNotSupported);
		break;
		}

	return numberOfSlots;
	}

TInt CSmartCardEapDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
                                  const TDataPackage& aPackage)
	{
	// ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
	// for the TSY to process
	// A request handle, request type and request data are passed to the TSY

	TAny* dataPtr = aPackage.Ptr1();
	TAny* dataPtr2 = aPackage.Ptr2();

	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type

	switch(aIpc)
		{
	// Non-Flow controlled requests

	case EMobileSmartCardEapInitialiseEapMethod:
		return DMmInitialiseEapMethod(aTsyReqHandle,
		         reinterpret_cast<TThreadId*>(dataPtr));

	case EMobileSmartCardEapGetUserIdentity:
		return DMmGetUserIdentity(aTsyReqHandle,
		         reinterpret_cast<RMobileSmartCardEap::TEapUserIdType*>(dataPtr),
		         aPackage.Des2n());

	case EMobileSmartCardEapGetAuthenticationStatus:
		return DMmGetAuthenticationStatus(aTsyReqHandle, 
		         reinterpret_cast<RMobileSmartCardEap::TEapAuthStatus*>(dataPtr));

	case EMobileSmartCardEapGetEapKey:
		return DMmGetEapKey(aTsyReqHandle,
		         reinterpret_cast<RMobileSmartCardEap::TEapKeyTag*>(dataPtr),
		         aPackage.Des2n());

	case EMobileSmartCardEapAuthenticationPhase1:
		return DMmSetAuthenticateDataForPhase1(aTsyReqHandle,
		         aPackage.Des1n(), reinterpret_cast<TInt*>(dataPtr2));

	case EMobileSmartCardEapAuthenticationPhase2:
		return DMmGetAuthenticateDataForPhase2(aTsyReqHandle,
		         aPackage.Des1n(), aPackage.Des2n());

	case EMobileSmartCardEapReleaseEapMethod:
		return DMmReleaseEapMethod(aTsyReqHandle);

	case EMobileSmartCardEapGetEapMethodAccessStatus:
		return DMmGetEapMethodAccessStatus(aTsyReqHandle,
		         reinterpret_cast<RMobileSmartCardEap::TEapMethodAccessStatus*>(dataPtr));

	case EMobileSmartCardEapNotifyEapMethodAccessStatusChange:
		return DMmNotifyEapMethodAccessStatusChange(aTsyReqHandle,
		         reinterpret_cast<RMobileSmartCardEap::TEapMethodAccessStatus*>(dataPtr));

	default:
		return KErrNotSupported;
		}
	}

TInt CSmartCardEapDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	// CancelService is called by the server when it is "cleaning-up" any still outstanding
	// asynchronous requests before closing a client's sub-session.
	// This will happen if a client closes its R-class handle without cancelling outstanding
	// asynchronous requests.

	switch (aIpc)
		{
	case EMobileSmartCardEapGetUserIdentity:
		return DMmGetUserIdentityCancel(aTsyReqHandle);
	case EMobileSmartCardEapGetAuthenticationStatus:
		return DMmGetAuthenticationStatusCancel(aTsyReqHandle);
	case EMobileSmartCardEapGetEapKey:
		return DMmGetEapKeyCancel(aTsyReqHandle);
	case EMobileSmartCardEapInitialiseEapMethod:
		return DMmInitialiseEapMethodCancel(aTsyReqHandle);
	case EMobileSmartCardEapAuthenticationPhase1:
	case EMobileSmartCardEapAuthenticationPhase2:
		return DMmSmartCardEapAuthenticationCancel(aTsyReqHandle);
	case EMobileSmartCardEapNotifyEapMethodAccessStatusChange:
		return DMmNotifyEapMethodAccessStatusChangeCancel(aTsyReqHandle);
	default:
		return KErrNotSupported;
		}
	}

TInt CSmartCardEapDMmTsy::DMmInitialiseEapMethod(const TTsyReqHandle aTsyReqHandle, TThreadId* aThreadId)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmInitialiseEapMethod called"));

	if (iAID != DMMTSY_PHONE_EAPAPP_AID() || iEapType != DMMTSY_PHONE_EAP_METHOD)
		{
		ReqCompleted(aTsyReqHandle, KErrNotFound);
		}
	else if (iSSInitialised)
		{
		ReqCompleted(aTsyReqHandle, KErrNone);
		}
	else
		{
		delete iCliTerminationNotifier;
		TRAPD(err, iCliTerminationNotifier = CThreadTerminationNotifier::NewL(this, *aThreadId));
		if (err != KErrNone)
			{
			ReqCompleted(aTsyReqHandle, err);
			}
		else
			{
			iSSInitialised = ETrue;
			iCliTerminationNotifier->Start();
			iAccessStatus = RMobileSmartCardEap::EEapMethodInUseApplicationActive;
			DMmCompleteNotifyEapMethodAccessStatusChange();
			iPhone->AddDelayedReq(aTsyReqHandle, this);
			}
		}

	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmInitialiseEapMethodCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmInitialiseEapMethodCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	iSSInitialised = EFalse;
	iAccessStatus = RMobileSmartCardEap::EEapMethodAvailable;
	DMmCompleteNotifyEapMethodAccessStatusChange();
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmGetUserIdentity(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapUserIdType* aEapIdType, TDes8* aUserId)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmGetUserIdentity called"));
	RMobileSmartCardEap::TEapUserIdentityV6Pckg *userIdPckg = reinterpret_cast<RMobileSmartCardEap::TEapUserIdentityV6Pckg*>(aUserId);
	RMobileSmartCardEap::TEapUserIdentityV6 &userId = (*userIdPckg)();

	if (*aEapIdType == DMMTSY_PHONE1_EAP_IDTYPE)
		{
		userId.iEapId = DMMTSY_PHONE1_EAP_UID;
		iPhone->AddDelayedReq(aTsyReqHandle, this);
		}
	else if (*aEapIdType == DMMTSY_PHONE2_EAP_IDTYPE)
		{
		userId.iEapId = DMMTSY_PHONE2_EAP_UID;
		iPhone->AddDelayedReq(aTsyReqHandle, this);
		}
	else
		{
		ReqCompleted(aTsyReqHandle, KErrArgument);
		}

	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmGetUserIdentityCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmGetUserIdentityCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}
	
TInt CSmartCardEapDMmTsy::DMmGetAuthenticationStatus(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapAuthStatus* aAuthStatus)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmGetAuthenticationStatus called"));

	*aAuthStatus = DMMTSY_PHONE_EAPAUTHSTATUS;
	iPhone->AddDelayedReq(aTsyReqHandle, this);

	return KErrNone;
	}
	
TInt CSmartCardEapDMmTsy::DMmGetAuthenticationStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmGetAuthenticationStatusCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmGetEapKey(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapKeyTag* aEapKeyTag, TDes8* aKey)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmGetEapKey called"));

	RMobileSmartCardEap::TEapKeyV6Pckg *keyPckg = reinterpret_cast<RMobileSmartCardEap::TEapKeyV6Pckg*>(aKey);
	RMobileSmartCardEap::TEapKeyV6 &key = (*keyPckg)();

	if (*aEapKeyTag == DMMTSY_PHONE1_EAPKEYTAG)
		{
		key.iEapKey = DMMTSY_PHONE1_EAPKEY;
		iPhone->AddDelayedReq(aTsyReqHandle, this);
		}
	else if (*aEapKeyTag == DMMTSY_PHONE2_EAPKEYTAG)
		{
		key.iEapKey = DMMTSY_PHONE2_EAPKEY;
		iPhone->AddDelayedReq(aTsyReqHandle, this);
		}
	else
		{
		ReqCompleted(aTsyReqHandle, KErrArgument);
		}

	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmGetEapKeyCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmGetEapKeyCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmSetAuthenticateDataForPhase1(const TTsyReqHandle aTsyReqHandle, TDes8* aEapAuthData, TInt* aPhase1Size)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmSetAuthenticateDataForPhase1 called"));

	RMobileSmartCardEap::CEapAuthenticateRequestDataV6* authReq = NULL;
	TRAPD(err, authReq = RMobileSmartCardEap::CEapAuthenticateRequestDataV6::NewL());
	if (err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TRAP(err, authReq->InternalizeL(*aEapAuthData));
	if (err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TPtr8 reqPacket = authReq->GetEapReqPacket();
	if (reqPacket == DMMTSY_PHONE1_EAPPACKET)
		{
		iRespPtrC.Set(DMMTSY_PHONE2_EAPPACKET);
		*aPhase1Size = iRespPtrC.Length();
		}
	else if (reqPacket == DMMTSY_PHONE1_EAPPACKET2)
		{
		iRespPtrC.Set(DMMTSY_PHONE2_EAPPACKET2);
		*aPhase1Size = iRespPtrC.Length();
		}
	else
		{
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
		return KErrNone;
		}

	iPhone->AddDelayedReq(aTsyReqHandle, this);
	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmGetAuthenticateDataForPhase2(const TTsyReqHandle aTsyReqHandle, TDes8* aEapAuthData, TDes8* aPhase2Resp)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmSetAuthenticateDataForPhase2 called"));

	RMobileSmartCardEap::CEapAuthenticateRequestDataV6* authReq = NULL;
	TRAPD(err, authReq = RMobileSmartCardEap::CEapAuthenticateRequestDataV6::NewL());
	if (err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TRAP(err, authReq->InternalizeL(*aEapAuthData));
	if (err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TPtr8 reqPacket = authReq->GetEapReqPacket();
	if (reqPacket != DMMTSY_PHONE1_EAPPACKET &&
	    reqPacket != DMMTSY_PHONE1_EAPPACKET2)
		{
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
		return KErrNone;
		}

	aPhase2Resp->Copy(iRespPtrC);

	iPhone->AddDelayedReq(aTsyReqHandle, this);
 
	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmSmartCardEapAuthenticationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmSmartCardEapAuthenticationCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);
	iRespPtrC.Set(NULL, 0);
	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmReleaseEapMethod(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmReleaseEapMethod called"));
	iSSInitialised = EFalse;
	iAccessStatus = RMobileSmartCardEap::EEapMethodUnableToInitialise;
	ReqCompleted(aTsyReqHandle, KErrNone);

	DMmCompleteNotifyEapMethodAccessStatusChange();

	// Simulate availability after 2 secs.
	User::After(2000000);
	iAccessStatus = RMobileSmartCardEap::EEapMethodAvailable;
	DMmCompleteNotifyEapMethodAccessStatusChange();

	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmGetEapMethodAccessStatus(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapMethodAccessStatus* aEapState)
	{
	LOGTEXT(_L8("CSmartCardEapDMmTsy::DMmGetEapMethodAccessStatus called"));
	*aEapState = iAccessStatus;
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmNotifyEapMethodAccessStatusChange(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapMethodAccessStatus* aEapState)
	{
	__ASSERT_ALWAYS(!iEapAccessNotifyData.iNotifyPending, PanicClient(EEtelPanicRequestAsyncTwice));

	iEapAccessNotifyData.iNotifyPending = ETrue;
	iEapAccessNotifyData.iNotifyHandle = aTsyReqHandle;
	iEapAccessNotifyData.iNotifyData = aEapState;

	return KErrNone;
	}

TInt CSmartCardEapDMmTsy::DMmNotifyEapMethodAccessStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if(iEapAccessNotifyData.iNotifyPending)
		{
		iEapAccessNotifyData.iNotifyPending = EFalse;
		ReqCompleted(aTsyReqHandle, KErrCancel);
		return KErrNone;
		}	

	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

void CSmartCardEapDMmTsy::DMmCompleteNotifyEapMethodAccessStatusChange()
	{
	if(iEapAccessNotifyData.iNotifyPending)
		{
		iEapAccessNotifyData.iNotifyPending = EFalse;
		*(reinterpret_cast<RMobileSmartCardEap::TEapMethodAccessStatus*>(iEapAccessNotifyData.iNotifyData)) = iAccessStatus;
		ReqCompleted(iEapAccessNotifyData.iNotifyHandle, KErrNone);
		}
	}

RHandleBase* CSmartCardEapDMmTsy::GlobalKernelObjectHandle()
	{
	return &iSemaphr;
	}

void CSmartCardEapDMmTsy::ClientHasTerminated(TInt aExitReason)
	{
	// Can TSY do anything with the thread's exit reason?
	// Exit code can be a zero (e.g. for KERN-EXEC 0) a positive value
	// (e.g. for KERN-EXEC 3) or a negative error.
	(void) aExitReason;

	switch (iAccessStatus)
		{
	case RMobileSmartCardEap::EEapMethodInUseApplicationActive:
		iSSInitialised = EFalse;
		iAccessStatus = RMobileSmartCardEap::EEapMethodUnableToInitialise;
		DMmCompleteNotifyEapMethodAccessStatusChange();
		iSemaphr.Signal();

		// Simulate availability after 2 secs.
		User::After(2000000);
		iAccessStatus = RMobileSmartCardEap::EEapMethodAvailable;
		DMmCompleteNotifyEapMethodAccessStatusChange();
		break;

	case RMobileSmartCardEap::EEapMethodInUseApplicationInactive:
		iAccessStatus = RMobileSmartCardEap::EEapMethodAvailable;
		DMmCompleteNotifyEapMethodAccessStatusChange();
		iSemaphr.Signal();
		break;

	default:
		;
		}
	
	//iSemaphr.Signal();
	}

//
// Class definition for monitoring thread termination
//

CSmartCardEapDMmTsy::CThreadTerminationNotifier* CSmartCardEapDMmTsy::CThreadTerminationNotifier::NewL(CSmartCardEapDMmTsy* aSubSess, const TThreadId& aId)
	{
	CThreadTerminationNotifier* self = new(ELeave) CThreadTerminationNotifier(aSubSess);
	CleanupStack::PushL(self);
	self->ConstructL(aId);
	CleanupStack::Pop(self);
	return self;
	}

CSmartCardEapDMmTsy::CThreadTerminationNotifier::CThreadTerminationNotifier(CSmartCardEapDMmTsy* aSubSess)
: CActive(EPriorityStandard), iSubSess(aSubSess)
	{
	}

void CSmartCardEapDMmTsy::CThreadTerminationNotifier::ConstructL(const TThreadId& aId)
	{
	TInt openTh = iCliThread.Open(aId);
	User::LeaveIfError(openTh);
    CActiveScheduler::Add(this);
	}

void CSmartCardEapDMmTsy::CThreadTerminationNotifier::Start()
	{
	iCliThread.Logon(iStatus);
	SetActive();
	}

void CSmartCardEapDMmTsy::CThreadTerminationNotifier::RunL()
	{
	iSubSess->ClientHasTerminated(iStatus.Int());
	}

void CSmartCardEapDMmTsy::CThreadTerminationNotifier::DoCancel()
	{
	iCliThread.LogonCancel(iStatus);
	}

CSmartCardEapDMmTsy::CThreadTerminationNotifier::~CThreadTerminationNotifier()
	{
	Cancel();
	iCliThread.Close();
	}

//
// methods for CActiveListNode
//

CSmartCardEapDMmTsy::CActiveListNode::CActiveListNode(CActive *aActive, const TTsyReqHandle aTsyReqHandle) :
	iActive(aActive), iTsyReqHandle(aTsyReqHandle)
	{
	}

CSmartCardEapDMmTsy::CActiveListNode::~CActiveListNode()
	{
	delete iActive;
	}
