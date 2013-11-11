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


CListReadAllAttempt* CListReadAllAttempt::NewL(RMobilePhone::TClientId* aId, TTsyReqHandle aReqHandle)
	{
	CListReadAllAttempt* read=new(ELeave) CListReadAllAttempt(aId, aReqHandle);
	CleanupStack::PushL(read);
	read->ConstructL();
	CleanupStack::Pop();
	return read;
	}

CListReadAllAttempt::CListReadAllAttempt(RMobilePhone::TClientId* aId, TTsyReqHandle aReqHandle)
	:CBase()
	{
	iClient.iSessionHandle=aId->iSessionHandle;
	iClient.iSubSessionHandle=aId->iSubSessionHandle;
	iReqHandle=aReqHandle;
	iListBuf = NULL;
	}

void CListReadAllAttempt::ConstructL()
	{
	}

CListReadAllAttempt::~CListReadAllAttempt()
	{
	delete iListBuf;
	}

/**************************************************************************/
//
//	CSmsStoreDMmTsy
//
/**************************************************************************/

CSmsStoreDMmTsy* CSmsStoreDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,
									   const TDesC& aName)
	{
	CSmsStoreDMmTsy* subsession=new(ELeave) CSmsStoreDMmTsy(aPhone,aFac,aName);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CSmsStoreDMmTsy::Init()
	{}

CSmsStoreDMmTsy::CSmsStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,
								 const TDesC& aName)
	:CSubSessionExtDummyBase(aFac), iPhone(aPhone)
	{
	iSmsStoreName=aName;
	}

void CSmsStoreDMmTsy::ConstructL()
	{
	iSmsReadAll = new CArrayPtrFlat<CListReadAllAttempt>(1);
	LOGTEXT(_L8("CSmsStoreDMmTsy created"));
	}

CSmsStoreDMmTsy::~CSmsStoreDMmTsy()
	{
	for (TInt i=iSmsReadAll->Count()-1; i>=0; --i)
		{
		CListReadAllAttempt* ptr=iSmsReadAll->At(i);
		iSmsReadAll->Delete(i);
		delete ptr;
		}
	delete iSmsReadAll;
	LOGTEXT(_L8("CSmsStoreDMmTsy destroyed"));
	}

CTelObject* CSmsStoreDMmTsy::OpenNewObjectByNameL(const TDesC&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSmsStoreDMmTsy::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSmsStoreDMmTsy::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{

//
// Normal Services
//

//
// Flow Controlled Services
//
	case EMobilePhoneStoreRead:
	case EMobilePhoneStoreWrite:
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreDeleteAll:
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
	case EMobilePhoneStoreGetInfo:
		ret=KReqModeFlowControlObeyed;
		break;

//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobilePhoneStoreNotifyStoreEvent:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Cancel Requests
//
	case EMobilePhoneStoreGetInfoCancel:
	case EMobilePhoneStoreReadCancel:
	case EMobilePhoneStoreWriteCancel:
	case EMobilePhoneStoreDeleteCancel:
	case EMobilePhoneStoreDeleteAllCancel:
	case EMobilePhoneStoreReadAllCancel:
	case EMobilePhoneStoreNotifyStoreEventCancel:
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;
	}

TInt CSmsStoreDMmTsy::RegisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CSmsStoreDMmTsy: RegisterNotification - Store Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CSmsStoreDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CSmsStoreDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CSmsStoreDMmTsy: DeregisterNotification - Store Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CSmsStoreDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CSmsStoreDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CSmsStoreDMmTsy: Registered with 10 slots"));
		numberOfSlots=10;
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CSmsStoreDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CSmsStoreDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
	{
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
	case EMobilePhoneStoreRead:
		return Read(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneStoreWrite:
		return Write(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneStoreDelete:
		return Delete(aTsyReqHandle,
			REINTERPRET_CAST(TInt*,dataPtr));

	case EMobilePhoneStoreDeleteAll:
		return DeleteAll(aTsyReqHandle);

	case EMobilePhoneStoreGetInfo:
		return GetInfo(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhoneStore::TMobilePhoneStoreInfoV1*, dataPtr));

	case EMobilePhoneStoreReadAllPhase1:
		return ReadAllPhase1(aTsyReqHandle,
			REINTERPRET_CAST(CRetrieveMobilePhoneSmsList::TBatchRequestData*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobilePhoneStoreReadAllPhase2:
		return ReadAllPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr),aPackage.Des2n());

	case EMobilePhoneStoreNotifyStoreEvent:
		return NotifyStoreEvent(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhoneStore::TMobileStoreEvent*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	default:
		return KErrNotSupported;
		}
	}

TInt CSmsStoreDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		return GetInfoCancel(aTsyReqHandle);
	case EMobilePhoneStoreRead:
		return ReadCancel(aTsyReqHandle);
	case EMobilePhoneStoreWrite:
		return WriteCancel(aTsyReqHandle);
	case EMobilePhoneStoreDelete:
		return DeleteCancel(aTsyReqHandle);
	case EMobilePhoneStoreDeleteAll:
		return DeleteAllCancel(aTsyReqHandle);
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
		return ReadAllCancel(aTsyReqHandle);
	case EMobilePhoneStoreNotifyStoreEvent:
		return NotifyStoreEventCancel(aTsyReqHandle);
		}
	return KErrGeneral;
	}


TInt CSmsStoreDMmTsy::Read(const TTsyReqHandle aTsyReqHandle, TDes8* aEntry)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::Read called"));

	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* entryPckg = (RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*)aEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1& entry = (*entryPckg)();

	if (entry.iIndex!=DMMTSY_SMS_STORE_INDEX1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		entry.iMsgData = DMMTSY_SMS_MESSAGE_PDU1;
		entry.iMsgStatus =DMMTSY_SMS_STORE_STATUS1;
		entry.iServiceCentre.iTelNumber = DMMTSY_PHONE_TEL_NUMBER;
		entry.iServiceCentre.iTypeOfNumber = DMMTSY_PHONE_TON;
		entry.iServiceCentre.iNumberPlan = DMMTSY_PHONE_NPI;

		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::ReadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::ReadCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aEntry)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::Write called"));

	RMobileSmsStore::TMobileCdmaSmsEntryV1Pckg* entryPckg = (RMobileSmsStore::TMobileCdmaSmsEntryV1Pckg*)aEntry;
	RMobileSmsStore::TMobileCdmaSmsEntryV1& entry = (*entryPckg)();

	if ((entry.iIndex != DMMTSY_SMS_STORE_INDEX2) ||
		(entry.iMsgData.Compare(DMMTSY_SMS_MESSAGE_PDU2) != 0) ||
		(entry.iMsgStatus != DMMTSY_SMS_STORE_STATUS2) ||
		(entry.iRemoteParty.iTelNumber != DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER2) ||
		(entry.iTeleservice != DMMTSY_SMS_MESSAGE_TELESERVICE))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CSmsStoreDMmTsy::WriteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::WriteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::Delete called"));
	if (*aIndex!=DMMTSY_SMS_STORE_INDEX3)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::DeleteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::DeleteAll(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::DeleteAll called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::DeleteAllCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::DeleteAllCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::GetInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhoneStore::TMobilePhoneStoreInfoV1* aInfo)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::GetInfo called"));

	aInfo->iName=DMMTSY_MESSAGE_STORE_NAME;
	aInfo->iType=DMMTSY_MESSAGE_STORE_TYPE;
	aInfo->iTotalEntries=DMMTSY_MESSAGE_STORE_TOTAL_ENTRIES;
	aInfo->iUsedEntries=DMMTSY_MESSAGE_STORE_USED_ENTRIES;
	aInfo->iCaps=DMMTSY_MESSAGE_STORE_CAPS;

	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::GetInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::GetInfoCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::NotifyStoreEvent(const TTsyReqHandle aTsyReqHandle,
									   RMobilePhoneStore::TMobileStoreEvent *aEvent,
									   TInt* aIndex)
	{
	if (!iNotifyStoreEvent++)
		{
		LOGTEXT(_L8("CSmsStoreDMmTsy::NotifyStoreEvent called"));
		*aIndex=DMMTSY_SMS_STORE_INDEX4;
		*aEvent=DMMTSY_MESSAGE_STORE_EVENT;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::NotifyStoreEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::NotifyStoreEventCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsStoreDMmTsy::ReadAllPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneSmsList::TBatchRequestData* aRequest, TInt* aBufSize)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::ReadAllPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;

	if (aRequest->iBatchRequest)
		if ((aRequest->iBatchSize != DMMTSY_SMS_STORE_BATCH) ||
		    (aRequest->iStartIndex != DMMTSY_SMS_STORE_START))
			leaveCode = KErrCorrupt;

	if (leaveCode == KErrNone)
		{
		if (aRequest->iEntryType==DMMTSY_SMS_STORE_GSMENTRY_TYPE)
			{
			TRAP(leaveCode, ret=ProcessReadAllGsmPhase1L(aTsyReqHandle, &aRequest->iClient, aBufSize););
			}
		else
			{
			TRAP(leaveCode, ret=ProcessReadAllCdmaPhase1L(aTsyReqHandle, &aRequest->iClient, aBufSize););
			}
		}

	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);

	return ret;
	};

TInt CSmsStoreDMmTsy::ProcessReadAllGsmPhase1L(const TTsyReqHandle aTsyReqHandle,
											   RMobilePhone::TClientId* aId,
											   TInt* aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneGsmSmsList* list=CMobilePhoneGsmSmsList::NewL();
	CleanupStack::PushL(list);

	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_SMS_STORE_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iIndex = DMMTSY_SMS_STORE_INDEX1;
			entry.iMsgData = DMMTSY_SMS_MESSAGE_PDU1;
			entry.iServiceCentre.iTelNumber = DMMTSY_PHONE_TEL_NUMBER;
			entry.iServiceCentre.iTypeOfNumber = DMMTSY_PHONE_TON;
			entry.iServiceCentre.iNumberPlan = DMMTSY_PHONE_NPI;
			entry.iMsgStatus = DMMTSY_SMS_STORE_STATUS1;
			break;
		case 1:
			entry.iIndex = DMMTSY_SMS_STORE_INDEX2;
			entry.iMsgData = DMMTSY_SMS_MESSAGE_PDU2;
			entry.iServiceCentre.iTelNumber = DMMTSY_SMSP_STORE_SCA;
			entry.iServiceCentre.iTypeOfNumber = DMMTSY_PHONE_TON;
			entry.iServiceCentre.iNumberPlan = DMMTSY_PHONE_NPI;
			entry.iMsgStatus = DMMTSY_SMS_STORE_STATUS2;
			break;
		case 2:
			// No need to recopy other info parameters
			// as they are identical to the ones in case 1 for this test
			entry.iIndex=DMMTSY_SMS_STORE_INDEX3;
			break;
		case 3:
		default:
			// No need to recopy other info parameters
			// as they are identical to the ones in case 1 for this test
			entry.iIndex=DMMTSY_SMS_STORE_INDEX4;
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aId, aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iSmsReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	// Complete first phase of list retrieval
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}


TInt CSmsStoreDMmTsy::ProcessReadAllCdmaPhase1L(const TTsyReqHandle aTsyReqHandle,
											   RMobilePhone::TClientId* aId,
											   TInt* aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneCdmaSmsList* list=CMobilePhoneCdmaSmsList::NewL();
	CleanupStack::PushL(list);

	RMobileSmsStore::TMobileCdmaSmsEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_SMS_STORE_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iIndex = DMMTSY_SMS_STORE_INDEX1;
			entry.iMsgData = DMMTSY_SMS_MESSAGE_PDU1;
			entry.iMsgStatus = DMMTSY_SMS_STORE_STATUS1;
			entry.iRemoteParty.iTelNumber = DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER1;
			entry.iTeleservice = DMMTSY_SMS_MESSAGE_TELESERVICE;
			break;
		case 1:
			entry.iIndex = DMMTSY_SMS_STORE_INDEX2;
			entry.iMsgData = DMMTSY_SMS_MESSAGE_PDU2;
			entry.iMsgStatus = DMMTSY_SMS_STORE_STATUS2;
			break;
		case 2:
			// No need to recopy other info parameters
			// as they are identical to the ones in case 1 for this test
			entry.iIndex=DMMTSY_SMS_STORE_INDEX3;
			break;
		case 3:
		default:
			// No need to recopy other info parameters
			// as they are identical to the ones in case 1 for this test
			entry.iIndex=DMMTSY_SMS_STORE_INDEX4;
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aId,aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iSmsReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	// Complete first phase of list retrieval
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}


TInt CSmsStoreDMmTsy::ReadAllPhase2(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::ReadAllPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the read attempt from this client
	for (TInt i=0; i<iSmsReadAll->Count(); ++i)
		{
		read = iSmsReadAll->At(i);
		if ((read->iClient.iSessionHandle==aId->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aId->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iSmsReadAll->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CSmsStoreDMmTsy::ReadAllCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsStoreDMmTsy::ReadAllCancel called"));

	iPhone->RemoveDelayedReq(aTsyReqHandle);

	// Remove the read all attempt from iSmsReadAll
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iSmsReadAll->Count(); ++i)
		{
		read = iSmsReadAll->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iSmsReadAll->Delete(i);
			break;
			}
		}

	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

/**************************************************************************/
//
//	CNamStoreDMmTsy
//
/**************************************************************************/

CNamStoreDMmTsy* CNamStoreDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	{
	CNamStoreDMmTsy* subsession=new(ELeave) CNamStoreDMmTsy(aPhone,aFac,aName);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CNamStoreDMmTsy::Init()
	{}

CNamStoreDMmTsy::CNamStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	:CSubSessionExtDummyBase(aFac), iPhone(aPhone)
	{
	iNamStoreName=aName;
	}

void CNamStoreDMmTsy::ConstructL()
	{
	iNamReadAll = new CArrayPtrFlat<CListReadAllAttempt>(1);
	LOGTEXT(_L8("CNamStoreDMmTsy created"));
	}

CNamStoreDMmTsy::~CNamStoreDMmTsy()
	{
	for (TInt i=iNamReadAll->Count()-1; i>=0; --i)
		{
		CListReadAllAttempt* ptr=iNamReadAll->At(i);
		iNamReadAll->Delete(i);
		delete ptr;
		}
	delete iNamReadAll;
	LOGTEXT(_L8("CNamStoreDMmTsy destroyed"));
	}

CTelObject* CNamStoreDMmTsy::OpenNewObjectByNameL(const TDesC&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CNamStoreDMmTsy::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CNamStoreDMmTsy::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{

	case EMobileNamStoreSetActiveNam:
		break;

//
// Flow Controlled Services
//
	case EMobilePhoneStoreRead:
	case EMobilePhoneStoreWrite:
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreDeleteAll:
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
	case EMobilePhoneStoreReadAllV4Phase1:
	case EMobilePhoneStoreReadAllV4Phase2:
	case EMobilePhoneStoreGetInfo:
	case EMobileNamStoreStoreAll:
	case EMobileNamStoreStoreAllV4:
		ret=KReqModeFlowControlObeyed;
		break;
//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobilePhoneStoreNotifyStoreEvent:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Cancel Requests
//
	case EMobilePhoneStoreGetInfoCancel:
	case EMobilePhoneStoreReadCancel:
	case EMobilePhoneStoreWriteCancel:
	case EMobilePhoneStoreDeleteCancel:
	case EMobilePhoneStoreDeleteAllCancel:
	case EMobilePhoneStoreReadAllCancel:
	case EMobilePhoneStoreNotifyStoreEventCancel:
	case EMobileNamStoreStoreAllCancel:
	case EMobileNamStoreStoreAllV4Cancel:
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;
	}

TInt CNamStoreDMmTsy::RegisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CNamStoreDMmTsy: RegisterNotification - Store Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CSmsStoreDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CNamStoreDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CNamStoreDMmTsy: DeregisterNotification - Store Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CNamStoreDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CNamStoreDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CNamStoreDMmTsy: Registered with 2 slot"));
		numberOfSlots=2;
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CNamStoreDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CNamStoreDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
	{
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
	case EMobilePhoneStoreRead:
		return Read(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneStoreWrite:
		return Write(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneStoreDelete:
		return Delete(aTsyReqHandle,
			REINTERPRET_CAST(TInt*,dataPtr));

	case EMobilePhoneStoreDeleteAll:
		return DeleteAll(aTsyReqHandle);

	case EMobilePhoneStoreGetInfo:
		return GetInfo(aTsyReqHandle,
			REINTERPRET_CAST(RMobileNamStore::TMobileNamStoreInfoV1*, dataPtr));

	case EMobilePhoneStoreReadAllPhase1:
		return ReadAllPhase1(aTsyReqHandle,
			REINTERPRET_CAST(CRetrieveMobilePhoneNamList::TNamRequestData*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobilePhoneStoreReadAllPhase2:
		return ReadAllPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr),aPackage.Des2n());

	case EMobilePhoneStoreReadAllV4Phase1:
		return ReadAllPhase1V4(aTsyReqHandle,
			REINTERPRET_CAST(CRetrieveMobilePhoneNamList::TNamRequestData*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobilePhoneStoreReadAllV4Phase2:
		return ReadAllPhase2V4(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr),aPackage.Des2n());

	case EMobilePhoneStoreNotifyStoreEvent:
		return NotifyStoreEvent(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhoneStore::TMobileStoreEvent*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobileNamStoreSetActiveNam:
		return SetActiveNam(aTsyReqHandle,
				REINTERPRET_CAST(TInt*,dataPtr));

	case EMobileNamStoreStoreAll:
		return StoreAll(aTsyReqHandle,REINTERPRET_CAST(TInt*,dataPtr),aPackage.Des2n());

	case EMobileNamStoreStoreAllV4:
		return StoreAllV4(aTsyReqHandle,REINTERPRET_CAST(TInt*,dataPtr),aPackage.Des2n());

	default:
		return KErrNotSupported;
		}
	}

TInt CNamStoreDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		return GetInfoCancel(aTsyReqHandle);
	case EMobilePhoneStoreRead:
		return ReadCancel(aTsyReqHandle);
	case EMobilePhoneStoreWrite:
		return WriteCancel(aTsyReqHandle);
	case EMobilePhoneStoreDelete:
		return DeleteCancel(aTsyReqHandle);
	case EMobilePhoneStoreDeleteAll:
		return DeleteAllCancel(aTsyReqHandle);
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
	case EMobilePhoneStoreReadAllV4Phase1:
	case EMobilePhoneStoreReadAllV4Phase2:
		return ReadAllCancel(aTsyReqHandle);
	case EMobilePhoneStoreNotifyStoreEvent:
		return NotifyStoreEventCancel(aTsyReqHandle);
	case EMobileNamStoreSetActiveNam:
		return SetActiveNamCancel(aTsyReqHandle);
	case EMobileNamStoreStoreAll:
	case EMobileNamStoreStoreAllV4:
		return StoreAllCancel(aTsyReqHandle);
		}
	return KErrGeneral;
	}


void CNamStoreDMmTsy::InitNamEntry(RMobileNamStore::TMobileNamEntryV1& aEntry)
	{
	aEntry.iNamId = DMMTSY_NAM_STORE_NAMID;
	aEntry.iParamIdentifier = DMMTSY_NAM_STORE_PARAM;
	aEntry.iData = DMMTSY_NAM_STORE_DATA;
	}

TInt CNamStoreDMmTsy::ValidateNamEntry(RMobileNamStore::TMobileNamEntryV1& aEntry)
	{
	if ((aEntry.iNamId != DMMTSY_NAM_STORE_NAMID) ||
		(aEntry.iParamIdentifier != DMMTSY_NAM_STORE_PARAM) ||
		(aEntry.iData.Compare(DMMTSY_NAM_STORE_DATA) != 0))
		return KErrCorrupt;
	else
		return KErrNone;
	}

void CNamStoreDMmTsy::InitNamEntry(RMobileNamStore::TMobileNamEntryV4& aEntry)
	{
	aEntry.iNamId = DMMTSY_NAM_STORE_NAMID;
	aEntry.iParamIdentifier = DMMTSY_NAM_STORE_PARAM;
	aEntry.iData = DMMTSY_NAM_STORE_DATA_V4;
	}

TInt CNamStoreDMmTsy::ValidateNamEntry(RMobileNamStore::TMobileNamEntryV4& aEntry)
	{
	if ((aEntry.iNamId != DMMTSY_NAM_STORE_NAMID) ||
		(aEntry.iParamIdentifier != DMMTSY_NAM_STORE_PARAM) ||
		(aEntry.iData.Compare(DMMTSY_NAM_STORE_DATA_V4) != 0))
		return KErrCorrupt;
	else
		return KErrNone;
	}

TInt CNamStoreDMmTsy::Read(const TTsyReqHandle aTsyReqHandle, TDes8* aEntry)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::Read called"));
	RMobileNamStore::TMobileNamEntryV1Pckg* entryPckg = (RMobileNamStore::TMobileNamEntryV1Pckg*)aEntry;
	RMobileNamStore::TMobileNamEntryV4Pckg* entryV4Pckg = (RMobileNamStore::TMobileNamEntryV4Pckg*)aEntry;

	RMobilePhone::TMultimodeType& entry = (*entryPckg)();
	RMobileNamStore::TMobileNamEntryV1& entryV1 = (*entryPckg)();
	RMobileNamStore::TMobileNamEntryV4& entryV4 = (*entryV4Pckg)();

	if(entry.ExtensionId() == KETelExtMultimodeV1)
		{
		if (entryV1.iIndex!=DMMTSY_NAM_STORE_INDEX1)
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{
			InitNamEntry(entryV1);
			iPhone->AddDelayedReq(aTsyReqHandle,this);
			}
		}
	else if(entry.ExtensionId() == KETelExtMultimodeV4)
		{
		if (entryV4.iIndex!=DMMTSY_NAM_STORE_INDEX1)
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{
			InitNamEntry(entryV4);
			iPhone->AddDelayedReq(aTsyReqHandle,this);
			}
		}
	else
		{
		// if not multimode v1 or v4, there is a problem
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}

	return KErrNone;
	}

TInt CNamStoreDMmTsy::ReadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::ReadCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aEntry)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::Write called"));
	RMobileNamStore::TMobileNamEntryV1Pckg* entryPckg = (RMobileNamStore::TMobileNamEntryV1Pckg*)aEntry;
	RMobileNamStore::TMobileNamEntryV4Pckg* entryV4Pckg = (RMobileNamStore::TMobileNamEntryV4Pckg*)aEntry;

	RMobilePhone::TMultimodeType& entry = (*entryPckg)();
	RMobileNamStore::TMobileNamEntryV1& entryV1 = (*entryPckg)();
	RMobileNamStore::TMobileNamEntryV4& entryV4 = (*entryV4Pckg)();

	if(entry.ExtensionId() == KETelExtMultimodeV1)
		{
		if ((entryV1.iIndex!=DMMTSY_NAM_STORE_INDEX2) || (ValidateNamEntry(entryV1) != KErrNone))
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	else if(entry.ExtensionId() == KETelExtMultimodeV4)
		{
		if ((entryV4.iIndex!=DMMTSY_NAM_STORE_INDEX2) || (ValidateNamEntry(entryV4) != KErrNone))
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{
			iPhone->AddDelayedReq(aTsyReqHandle,this);
			}
		}
	else
		{
		// if not multimode v1 or v4, there is a problem
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}

TInt CNamStoreDMmTsy::WriteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::WriteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::Delete called"));
	if (*aIndex!=DMMTSY_NAM_STORE_INDEX1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::DeleteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::DeleteAll(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::DeleteAll called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::DeleteAllCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::DeleteAllCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::GetInfo(const TTsyReqHandle aTsyReqHandle, RMobileNamStore::TMobileNamStoreInfoV1* aInfo)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::GetInfo called"));
	aInfo->iName=DMMTSY_NAM_STORE_NAME;
	aInfo->iType=DMMTSY_NAM_STORE_TYPE;
	aInfo->iTotalEntries=DMMTSY_NAM_STORE_ENTRIES;
	aInfo->iUsedEntries=DMMTSY_NAM_STORE_USED;
	aInfo->iCaps=DMMTSY_NAM_STORE_CAPS;
	aInfo->iNamCount = DMMTSY_NAM_STORE_COUNT;
	aInfo->iActiveNam = DMMTSY_NAM_STORE_ACTIVE;
	iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CNamStoreDMmTsy::GetInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::GetInfoCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::NotifyStoreEvent(const TTsyReqHandle aTsyReqHandle,
									   RMobilePhoneStore::TMobileStoreEvent *aEvent,
									   TInt* aIndex)
	{
	if (!iNotifyStoreEvent++)
		{
		LOGTEXT(_L8("CNamStoreDMmTsy::NotifyStoreEvent called"));
		*aIndex=DMMTSY_NAM_STORE_INDEX2;
		*aEvent = DMMTSY_NAM_STORE_EVENT;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CNamStoreDMmTsy::NotifyStoreEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::NotifyStoreEventCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::ReadAllPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneNamList::TNamRequestData* aRequestData, TInt* aBufSize)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::ReadAllPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	if (aRequestData->iNamId != DMMTSY_NAM_STORE_NAMID)
		leaveCode = KErrCorrupt;
	else
		{
		TRAP(leaveCode, ret=ProcessReadAllPhase1L(aTsyReqHandle, &aRequestData->iClient, aBufSize););
		}
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	};

TInt CNamStoreDMmTsy::ReadAllPhase1V4(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneNamList::TNamRequestData* aRequestData, TInt* aBufSize)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::ReadAllPhase1V4 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	if (aRequestData->iNamId != DMMTSY_NAM_STORE_NAMID)
		leaveCode = KErrCorrupt;
	else
		{
		TRAP(leaveCode, ret=ProcessReadAllPhase1V4L(aTsyReqHandle, &aRequestData->iClient, aBufSize););
		}
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	};

TInt CNamStoreDMmTsy::ProcessReadAllPhase1L(const TTsyReqHandle aTsyReqHandle,
											 RMobilePhone::TClientId* aId,
											 TInt* aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneNamList* list=CMobilePhoneNamList::NewL();
	CleanupStack::PushL(list);

	RMobileNamStore::TMobileNamEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_NAM_STORE_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			InitNamEntry(entry);
			entry.iIndex=DMMTSY_NAM_STORE_INDEX1;
			break;
		case 1:
			InitNamEntry(entry);
			entry.iIndex=DMMTSY_NAM_STORE_INDEX2;
		default:
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aId,aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iNamReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	// Complete first phase of list retrieval
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CNamStoreDMmTsy::ProcessReadAllPhase1V4L(const TTsyReqHandle aTsyReqHandle,
											 RMobilePhone::TClientId* aId,
											 TInt* aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneNamListV4* list=CMobilePhoneNamListV4::NewL();
	CleanupStack::PushL(list);

	RMobileNamStore::TMobileNamEntryV4 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_NAM_STORE_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			InitNamEntry(entry);
			entry.iIndex=DMMTSY_NAM_STORE_INDEX1;
			break;
		case 1:
			InitNamEntry(entry);
			entry.iIndex=DMMTSY_NAM_STORE_INDEX2;
		default:
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aId,aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iNamReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	// Complete first phase of list retrieval
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CNamStoreDMmTsy::ReadAllPhase2(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::ReadAllPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the read attempt from this client
	for (TInt i=0; i<iNamReadAll->Count(); ++i)
		{
		read = iNamReadAll->At(i);
		if ((read->iClient.iSessionHandle==aId->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aId->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iNamReadAll->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CNamStoreDMmTsy::ReadAllPhase2V4(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::ReadAllPhase2V4 called"));
	CListReadAllAttempt* read=NULL;
	// Find the read attempt from this client
	for (TInt i=0; i<iNamReadAll->Count(); ++i)
		{
		read = iNamReadAll->At(i);
		if ((read->iClient.iSessionHandle==aId->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aId->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iNamReadAll->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CNamStoreDMmTsy::ReadAllCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::ReadAllCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);

	// Remove the read all attempt from iNamReadAll
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iNamReadAll->Count(); ++i)
		{
		read = iNamReadAll->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iNamReadAll->Delete(i);
			break;
			}
		}

	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::SetActiveNam(const TTsyReqHandle aTsyReqHandle, TInt *aNamId)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::SetActiveNam called"));

	if (*aNamId != DMMTSY_NAM_STORE_NAMID)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CNamStoreDMmTsy::SetActiveNamCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::SetActiveCancelled called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::StoreAll(const TTsyReqHandle aTsyReqHandle, TInt* aNamId, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::StoreAll called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	if (*aNamId != DMMTSY_NAM_STORE_NAMID)
		leaveCode = KErrCorrupt;
	else
		{
		TRAP(leaveCode, ret=ProcessStoreAllL(aTsyReqHandle, aBuffer););
		}
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}

TInt CNamStoreDMmTsy::StoreAllV4(const TTsyReqHandle aTsyReqHandle, TInt* aNamId, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::StoreAll called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	if (*aNamId != DMMTSY_NAM_STORE_NAMID)
		leaveCode = KErrCorrupt;
	else
		{
		TRAP(leaveCode, ret=ProcessStoreAllV4L(aTsyReqHandle, aBuffer););
		}
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}

TInt CNamStoreDMmTsy::ProcessStoreAllL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	CMobilePhoneNamList* aNamList=CMobilePhoneNamList::NewL();
	CleanupStack::PushL(aNamList);

	aNamList->RestoreL(*aBuffer);

	RMobileNamStore::TMobileNamEntryV1 entry;

	if (aNamList->Enumerate() != DMMTSY_NAM_STORE_COUNT)
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	entry=aNamList->GetEntryL(0);

	if ((entry.iIndex != DMMTSY_NAM_STORE_INDEX1) ||
		(ValidateNamEntry(entry) != KErrNone))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	entry=aNamList->GetEntryL(1);

	if ((entry.iIndex != DMMTSY_NAM_STORE_INDEX2) ||
		(ValidateNamEntry(entry) != KErrNone))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(aNamList);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::ProcessStoreAllV4L(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	CMobilePhoneNamListV4* aNamList=CMobilePhoneNamListV4::NewL();
	CleanupStack::PushL(aNamList);

	aNamList->RestoreL(*aBuffer);

	RMobileNamStore::TMobileNamEntryV4 entry;

	if (aNamList->Enumerate() != DMMTSY_NAM_STORE_COUNT)
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	entry=aNamList->GetEntryL(0);

	if(entry.ExtensionId() != KETelExtMultimodeV4)
		{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			return KErrNone;
		}

	if ((entry.iIndex != DMMTSY_NAM_STORE_INDEX1) ||
		(ValidateNamEntry(entry) != KErrNone))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	entry=aNamList->GetEntryL(1);

	if ((entry.iIndex != DMMTSY_NAM_STORE_INDEX2) ||
		(ValidateNamEntry(entry) != KErrNone))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(aNamList);
	return KErrNone;
	}

TInt CNamStoreDMmTsy::StoreAllCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CNamStoreDMmTsy::StoreAllCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


/**************************************************************************/
//
//	CONStoreDMmTsy
//
/**************************************************************************/

CONStoreDMmTsy* CONStoreDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	{
	CONStoreDMmTsy* subsession=new(ELeave) CONStoreDMmTsy(aPhone,aFac,aName);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CONStoreDMmTsy::Init()
	{}

CONStoreDMmTsy::CONStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	:CSubSessionExtDummyBase(aFac), iPhone(aPhone)
	{
	iONStoreName=aName;
	}

void CONStoreDMmTsy::ConstructL()
	{
	iONReadAll = new CArrayPtrFlat<CListReadAllAttempt>(1);
	LOGTEXT(_L8("CONStoreDMmTsy created"));
	}

CONStoreDMmTsy::~CONStoreDMmTsy()
	{
	for (TInt i=iONReadAll->Count()-1; i>=0; --i)
		{
		CListReadAllAttempt* ptr=iONReadAll->At(i);
		iONReadAll->Delete(i);
		delete ptr;
		}
	delete iONReadAll;
	LOGTEXT(_L8("CONStoreDMmTsy destroyed"));
	}

CTelObject* CONStoreDMmTsy::OpenNewObjectByNameL(const TDesC&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CONStoreDMmTsy::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CONStoreDMmTsy::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// Flow Controlled Services
//
	case EMobilePhoneStoreRead:
	case EMobilePhoneStoreWrite:
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreDeleteAll:
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
	case EMobilePhoneStoreGetInfo:
	case EMobileONStoreStoreAll:
		ret=KReqModeFlowControlObeyed;
		break;
//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobilePhoneStoreNotifyStoreEvent:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Cancel Requests
//
	case EMobilePhoneStoreGetInfoCancel:
	case EMobilePhoneStoreReadCancel:
	case EMobilePhoneStoreWriteCancel:
	case EMobilePhoneStoreDeleteCancel:
	case EMobilePhoneStoreDeleteAllCancel:
	case EMobilePhoneStoreReadAllCancel:
	case EMobilePhoneStoreNotifyStoreEventCancel:
	case EMobileONStoreStoreAllCancel:
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;
	}

TInt CONStoreDMmTsy::RegisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CONStoreDMmTsy: RegisterNotification - Store Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CONStoreDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CONStoreDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CONStoreDMmTsy: DeregisterNotification - Store Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CONStoreDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CONStoreDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CONStoreDMmTsy: Registered with 2 slot"));
		numberOfSlots=2;
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CONStoreDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CONStoreDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
	{
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
	case EMobilePhoneStoreRead:
		return Read(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneStoreWrite:
		return Write(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneStoreDelete:
		return Delete(aTsyReqHandle,
			REINTERPRET_CAST(TInt*,dataPtr));

	case EMobilePhoneStoreDeleteAll:
		return DeleteAll(aTsyReqHandle);

	case EMobilePhoneStoreGetInfo:
		return GetInfo(aTsyReqHandle,
			REINTERPRET_CAST(RMobileONStore::TMobileONStoreInfoV1*, dataPtr));

	case EMobilePhoneStoreReadAllPhase1:
		return ReadAllPhase1(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobilePhoneStoreReadAllPhase2:
		return ReadAllPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr),aPackage.Des2n());

	case EMobilePhoneStoreNotifyStoreEvent:
		return NotifyStoreEvent(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhoneStore::TMobileStoreEvent*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobileONStoreStoreAll:
		return StoreAll(aTsyReqHandle,aPackage.Des1n());


	default:
		return KErrNotSupported;
		}
	}

TInt CONStoreDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		return GetInfoCancel(aTsyReqHandle);
	case EMobilePhoneStoreRead:
		return ReadCancel(aTsyReqHandle);
	case EMobilePhoneStoreWrite:
		return WriteCancel(aTsyReqHandle);
	case EMobilePhoneStoreDelete:
		return DeleteCancel(aTsyReqHandle);
	case EMobilePhoneStoreDeleteAll:
		return DeleteAllCancel(aTsyReqHandle);
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
		return ReadAllCancel(aTsyReqHandle);
	case EMobilePhoneStoreNotifyStoreEvent:
		return NotifyStoreEventCancel(aTsyReqHandle);
	case EMobileONStoreStoreAll:
		return StoreAllCancel(aTsyReqHandle);
		}
	return KErrGeneral;
	}


void CONStoreDMmTsy::InitONEntry(RMobileONStore::TMobileONEntryV1& aEntry)
	{
	aEntry.iMode = DMMTSY_ON_STORE_ENTRY_MODE;
	aEntry.iText =  DMMTSY_ON_STORE_ENTRY_TEXT;
	aEntry.iNumber.iTypeOfNumber = DMMTSY_PHONE_TON;
	aEntry.iNumber.iNumberPlan = DMMTSY_PHONE_NPI;
	aEntry.iNumber.iTelNumber = DMMTSY_PHONE_TEL_NUMBER;
	aEntry.iService = DMMTSY_ON_STORE_ENTRY_SERVICE;
	}

TInt CONStoreDMmTsy::ValidateONEntry(RMobileONStore::TMobileONEntryV1& aEntry)
	{
	if ((aEntry.iMode != DMMTSY_ON_STORE_ENTRY_MODE) ||
		(aEntry.iText.Compare(DMMTSY_ON_STORE_ENTRY_TEXT) != 0) ||
		(aEntry.iNumber.iTypeOfNumber != DMMTSY_PHONE_TON) ||
		(aEntry.iNumber.iNumberPlan != DMMTSY_PHONE_NPI) ||
		(aEntry.iNumber.iTelNumber.Compare(DMMTSY_PHONE_TEL_NUMBER) != 0) ||
		(aEntry.iService != DMMTSY_ON_STORE_ENTRY_SERVICE))
		return KErrCorrupt;
	else
		return KErrNone;
	}

TInt CONStoreDMmTsy::Read(const TTsyReqHandle aTsyReqHandle, TDes8* aEntry)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::Read called"));
	RMobileONStore::TMobileONEntryV1Pckg* entryPckg = (RMobileONStore::TMobileONEntryV1Pckg*)aEntry;
	RMobileONStore::TMobileONEntryV1& entry = (*entryPckg)();

	if (entry.iIndex!=DMMTSY_ON_STORE_INDEX1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		InitONEntry(entry);
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CONStoreDMmTsy::ReadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::ReadCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CONStoreDMmTsy::Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aEntry)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::Write called"));
	RMobileONStore::TMobileONEntryV1Pckg* entryPckg = (RMobileONStore::TMobileONEntryV1Pckg*)aEntry;
	RMobileONStore::TMobileONEntryV1& entry = (*entryPckg)();

	if ((entry.iIndex!=DMMTSY_ON_STORE_INDEX2) || (ValidateONEntry(entry) != KErrNone))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CONStoreDMmTsy::WriteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::WriteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CONStoreDMmTsy::Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::Delete called"));
	if (*aIndex!=DMMTSY_ON_STORE_INDEX1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CONStoreDMmTsy::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::DeleteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CONStoreDMmTsy::DeleteAll(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::DeleteAll called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CONStoreDMmTsy::DeleteAllCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::DeleteAllCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CONStoreDMmTsy::GetInfo(const TTsyReqHandle aTsyReqHandle, RMobileONStore::TMobileONStoreInfoV1* aInfo)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::GetInfo called"));
	aInfo->iName=DMMTSY_ON_STORE_NAME;
	aInfo->iType=DMMTSY_ON_STORE_TYPE;
	aInfo->iTotalEntries=DMMTSY_ON_STORE_ENTRIES;
	aInfo->iUsedEntries=DMMTSY_ON_STORE_USED;
	aInfo->iCaps=DMMTSY_ON_STORE_CAPS;

	aInfo->iNumberLen = DMMTSY_ON_STORE_INFO_GSM_MAX_NUM;
	aInfo->iTextLen = DMMTSY_ON_STORE_INFO_GSM_MAX_TEXT;
	iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CONStoreDMmTsy::GetInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::GetInfoCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CONStoreDMmTsy::NotifyStoreEvent(const TTsyReqHandle aTsyReqHandle,
									   RMobilePhoneStore::TMobileStoreEvent *aEvent,
									   TInt* aIndex)
	{
	if (!iNotifyStoreEvent++)
		{
		LOGTEXT(_L8("CONStoreDMmTsy::NotifyStoreEvent called"));
		*aIndex=DMMTSY_ON_STORE_INDEX2;
		*aEvent = DMMTSY_ON_STORE_EVENT;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CONStoreDMmTsy::NotifyStoreEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::NotifyStoreEventCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CONStoreDMmTsy::ReadAllPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::ReadAllPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessReadAllPhase1L(aTsyReqHandle, aId, aBufSize););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	};

TInt CONStoreDMmTsy::ProcessReadAllPhase1L(const TTsyReqHandle aTsyReqHandle,
											 RMobilePhone::TClientId* aId,
											 TInt* aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneONList* list=CMobilePhoneONList::NewL();
	CleanupStack::PushL(list);

	RMobileONStore::TMobileONEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_ON_STORE_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			InitONEntry(entry);
			entry.iIndex=DMMTSY_ON_STORE_INDEX1;
			break;
		case 1:
			InitONEntry(entry);
			entry.iIndex=DMMTSY_ON_STORE_INDEX2;
		default:
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aId,aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iONReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	// Complete first phase of list retrieval
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}


TInt CONStoreDMmTsy::ReadAllPhase2(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::ReadAllPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the read attempt from this client
	for (TInt i=0; i<iONReadAll->Count(); ++i)
		{
		read = iONReadAll->At(i);
		if ((read->iClient.iSessionHandle==aId->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aId->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iONReadAll->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}


TInt CONStoreDMmTsy::ReadAllCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::ReadAllCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);

	// Remove the read all attempt from iONReadAll
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iONReadAll->Count(); ++i)
		{
		read = iONReadAll->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iONReadAll->Delete(i);
			break;
			}
		}

	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CONStoreDMmTsy::StoreAll(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::StoreAll called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessStoreAllL(aTsyReqHandle, aBuffer););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}

TInt CONStoreDMmTsy::ProcessStoreAllL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	CMobilePhoneONList* aONList=CMobilePhoneONList::NewL();
	CleanupStack::PushL(aONList);

	aONList->RestoreL(*aBuffer);

	RMobileONStore::TMobileONEntryV1 entry;

	if (aONList->Enumerate() != DMMTSY_ON_STORE_COUNT)
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	entry=aONList->GetEntryL(0);

	if ((entry.iIndex != DMMTSY_ON_STORE_INDEX1) ||
		(ValidateONEntry(entry) != KErrNone))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	entry=aONList->GetEntryL(1);

	if ((entry.iIndex != DMMTSY_ON_STORE_INDEX2) ||
		(ValidateONEntry(entry) != KErrNone))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // aONList
	return KErrNone;
	}

TInt CONStoreDMmTsy::StoreAllCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CONStoreDMmTsy::StoreAllCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


/**************************************************************************/
//
//	CENStoreDMmTsy
//
/**************************************************************************/

CENStoreDMmTsy* CENStoreDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	{
	CENStoreDMmTsy* subsession=new(ELeave) CENStoreDMmTsy(aPhone,aFac,aName);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CENStoreDMmTsy::Init()
	{}

CENStoreDMmTsy::CENStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	:CSubSessionExtDummyBase(aFac), iPhone(aPhone)
	{
	iENStoreName=aName;
	}

void CENStoreDMmTsy::ConstructL()
	{
	iENReadAll = new CArrayPtrFlat<CListReadAllAttempt>(1);
	LOGTEXT(_L8("CENStoreDMmTsy created"));
	}

CENStoreDMmTsy::~CENStoreDMmTsy()
	{
	if (iENReadAll)
		{
		for (TInt i=iENReadAll->Count()-1; i>=0; --i)
			{
			CListReadAllAttempt* ptr=iENReadAll->At(i);
			iENReadAll->Delete(i);
			delete ptr;
			}
		}
	delete iENReadAll;
	LOGTEXT(_L8("CENStoreDMmTsy destroyed"));
	}

CTelObject* CENStoreDMmTsy::OpenNewObjectByNameL(const TDesC&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CENStoreDMmTsy::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CENStoreDMmTsy::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// Non Flow controlled services
//
	case EMobilePhoneStoreReadAllPhase2:
		break;
//
// Flow Controlled Services
//
	case EMobilePhoneStoreRead:
	case EMobilePhoneStoreWrite:
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreDeleteAll:
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreGetInfo:
		ret=KReqModeFlowControlObeyed;
		break;
//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobilePhoneStoreNotifyStoreEvent:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Cancel Requests
//
	case EMobilePhoneStoreGetInfoCancel:
	case EMobilePhoneStoreReadCancel:
	case EMobilePhoneStoreWriteCancel:
	case EMobilePhoneStoreDeleteCancel:
	case EMobilePhoneStoreDeleteAllCancel:
	case EMobilePhoneStoreReadAllCancel:
	case EMobilePhoneStoreNotifyStoreEventCancel:
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;
	}

TInt CENStoreDMmTsy::RegisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CENStoreDMmTsy: RegisterNotification - Store Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CENStoreDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CENStoreDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CENStoreDMmTsy: DeregisterNotification - Store Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CENStoreDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CENStoreDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CENStoreDMmTsy: Registered with 2 slot"));
		numberOfSlots=2;
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CENStoreDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CENStoreDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
	{
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
	case EMobilePhoneStoreRead:
		return Read(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneStoreWrite:
		return Write(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneStoreDelete:
		return Delete(aTsyReqHandle,
			REINTERPRET_CAST(TInt*,dataPtr));

	case EMobilePhoneStoreDeleteAll:
		return DeleteAll(aTsyReqHandle);

	case EMobilePhoneStoreGetInfo:
		return GetInfo(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhoneStore::TMobilePhoneStoreInfoV1*, dataPtr));

	case EMobilePhoneStoreReadAllPhase1:
		return ReadAllPhase1(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobilePhoneStoreReadAllPhase2:
		return ReadAllPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr),aPackage.Des2n());

	case EMobilePhoneStoreNotifyStoreEvent:
		return NotifyStoreEvent(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhoneStore::TMobileStoreEvent*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	default:
		return KErrNotSupported;
		}
	}

TInt CENStoreDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		return GetInfoCancel(aTsyReqHandle);
	case EMobilePhoneStoreRead:
		return ReadCancel(aTsyReqHandle);
	case EMobilePhoneStoreWrite:
		return WriteCancel(aTsyReqHandle);
	case EMobilePhoneStoreDelete:
		return DeleteCancel(aTsyReqHandle);
	case EMobilePhoneStoreDeleteAll:
		return DeleteAllCancel(aTsyReqHandle);
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
		return ReadAllCancel(aTsyReqHandle);
	case EMobilePhoneStoreNotifyStoreEvent:
		return NotifyStoreEventCancel(aTsyReqHandle);
		}
	return KErrGeneral;
	}


void CENStoreDMmTsy::InitENEntry(RMobileENStore::TMobileENEntryV1& aEntry)
	{
	aEntry.iNetworkSpecific = DMMTSY_EN_STORE_NS;
	aEntry.iMode            = DMMTSY_EN_STORE_MODE;
	aEntry.iCountryCode     = DMMTSY_EN_STORE_CC;
	aEntry.iIdentity        = DMMTSY_EN_STORE_ID;
	aEntry.iNumber          = DMMTSY_EN_STORE_NUM;
	aEntry.iAlphaId         = DMMTSY_EN_STORE_AMBULANCE_ID;
	aEntry.iCallType        = DMMTSY_EN_STORE_CALL_TYPE;
	}

TInt CENStoreDMmTsy::ValidateENEntry(RMobileENStore::TMobileENEntryV1& aEntry)
	{
	if ((aEntry.iNetworkSpecific != DMMTSY_EN_STORE_NS) ||
		(aEntry.iMode != DMMTSY_EN_STORE_MODE) ||
		(aEntry.iCountryCode.Compare(DMMTSY_EN_STORE_CC) != 0) ||
		(aEntry.iIdentity.Compare(DMMTSY_EN_STORE_ID) != 0) ||
		(aEntry.iNumber.Compare(DMMTSY_EN_STORE_NUM) != 0) ||
		(aEntry.iAlphaId.Compare(DMMTSY_EN_STORE_AMBULANCE_ID) != 0) ||
	    (aEntry.iCallType != DMMTSY_EN_STORE_CALL_TYPE))
		return KErrCorrupt;
	else
		return KErrNone;
	}

TInt CENStoreDMmTsy::Read(const TTsyReqHandle aTsyReqHandle, TDes8* aEntry)
	{

	LOGTEXT(_L8("CENStoreDMmTsy::Read called"));
	RMobileENStore::TMobileENEntryV1Pckg* entryPckg = (RMobileENStore::TMobileENEntryV1Pckg*)aEntry;
	RMobileENStore::TMobileENEntryV1& entry = (*entryPckg)();

	if (entry.iIndex!=DMMTSY_EN_STORE_INDEX1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		InitENEntry(entry);
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CENStoreDMmTsy::ReadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::ReadCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CENStoreDMmTsy::Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aEntry)
	{

	LOGTEXT(_L8("CENStoreDMmTsy::Write called"));
	RMobileENStore::TMobileENEntryV1Pckg* entryPckg = (RMobileENStore::TMobileENEntryV1Pckg*)aEntry;
	RMobileENStore::TMobileENEntryV1& entry = (*entryPckg)();

	if ((entry.iIndex!=DMMTSY_EN_STORE_INDEX2) || (ValidateENEntry(entry) != KErrNone))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CENStoreDMmTsy::WriteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::WriteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CENStoreDMmTsy::Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::Delete called"));

	if (*aIndex!=DMMTSY_EN_STORE_INDEX1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CENStoreDMmTsy::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::DeleteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CENStoreDMmTsy::DeleteAll(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::DeleteAll called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CENStoreDMmTsy::DeleteAllCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::DeleteAllCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CENStoreDMmTsy::GetInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhoneStore::TMobilePhoneStoreInfoV1* aInfo)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::GetInfo called"));

	aInfo->iName=DMMTSY_EN_STORE_NAME;
	aInfo->iType=DMMTSY_EN_STORE_TYPE;
	aInfo->iTotalEntries=DMMTSY_EN_STORE_ENTRIES;
	aInfo->iUsedEntries=DMMTSY_EN_STORE_USED;
	aInfo->iCaps=DMMTSY_EN_STORE_CAPS;

	iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CENStoreDMmTsy::GetInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::GetInfoCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CENStoreDMmTsy::NotifyStoreEvent(const TTsyReqHandle aTsyReqHandle,
									   RMobilePhoneStore::TMobileStoreEvent *aEvent,
									   TInt* aIndex)
	{
	if (!iNotifyStoreEvent++)
		{
		LOGTEXT(_L8("CENStoreDMmTsy::NotifyStoreEvent called"));
		*aIndex=DMMTSY_EN_STORE_INDEX2;
		*aEvent = DMMTSY_EN_STORE_EVENT;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CENStoreDMmTsy::NotifyStoreEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::NotifyStoreEventCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CENStoreDMmTsy::ReadAllPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId *aId, TInt* aBufSize)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::ReadAllPhase1 called"));

	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessReadAllPhase1L(aTsyReqHandle, aId, aBufSize););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);

	return ret;
	}

TInt CENStoreDMmTsy::ProcessReadAllPhase1L(const TTsyReqHandle aTsyReqHandle,
											 RMobilePhone::TClientId* aId,
											 TInt* aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneENList* list=CMobilePhoneENList::NewL();
	CleanupStack::PushL(list);

	RMobileENStore::TMobileENEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_EN_STORE_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			InitENEntry(entry);
			entry.iIndex=DMMTSY_EN_STORE_INDEX1;
			break;
		case 1:
			InitENEntry(entry);
			entry.iIndex=DMMTSY_EN_STORE_INDEX2;
		default:
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aId,aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iENReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	// Complete first phase of list retrieval
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}


TInt CENStoreDMmTsy::ReadAllPhase2(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::ReadAllPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the read attempt from this client
	for (TInt i=0; i<iENReadAll->Count(); ++i)
		{
		read = iENReadAll->At(i);
		if ((read->iClient.iSessionHandle==aId->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aId->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iENReadAll->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}


TInt CENStoreDMmTsy::ReadAllCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CENStoreDMmTsy::ReadAllCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);

	// Remove the read all attempt from iENReadAll
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iENReadAll->Count(); ++i)
		{
		read = iENReadAll->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iENReadAll->Delete(i);
			break;
			}
		}

	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


/**************************************************************************/
//
//	CMobilePhoneBookStoreDMmTsy
//
/**************************************************************************/

CMobilePhoneBookStoreDMmTsy* CMobilePhoneBookStoreDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName, const TDesC& aType)
	{
	CMobilePhoneBookStoreDMmTsy* subsession=new(ELeave) CMobilePhoneBookStoreDMmTsy(aPhone,aFac,aName,aType);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CMobilePhoneBookStoreDMmTsy::Init()
	{}

CMobilePhoneBookStoreDMmTsy::CMobilePhoneBookStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName, const TDesC& aType)
	:CSubSessionExtDummyBase(aFac), iPhone(aPhone)
	{
	iPhoneBookStoreName=aName;
	iIccType=aType;
	}

void CMobilePhoneBookStoreDMmTsy::ConstructL()
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy created"));
	}

CMobilePhoneBookStoreDMmTsy::~CMobilePhoneBookStoreDMmTsy()
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy destroyed"));
	}

CTelObject* CMobilePhoneBookStoreDMmTsy::OpenNewObjectByNameL(const TDesC&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CMobilePhoneBookStoreDMmTsy::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CMobilePhoneBookStoreDMmTsy::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// Flow Controlled Services
//
	case EMobilePhoneBookStoreRead:
	case EMobilePhoneBookStoreWrite:
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreGetInfo:
		ret=KReqModeFlowControlObeyed;
		break;
//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobilePhoneStoreNotifyStoreEvent:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Cancel Requests
//
	case EMobilePhoneStoreGetInfoCancel:
	case EMobilePhoneBookStoreReadCancel:
	case EMobilePhoneBookStoreWriteCancel:
	case EMobilePhoneStoreDeleteCancel:
	case EMobilePhoneStoreNotifyStoreEventCancel:
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;
	}

TInt CMobilePhoneBookStoreDMmTsy::RegisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy: RegisterNotification - Store Event "));
		return KErrNotSupported;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CMobilePhoneBookStoreDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy: DeregisterNotification - Store Event "));
		return KErrNotSupported;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CMobilePhoneBookStoreDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobilePhoneStoreNotifyStoreEvent:
		LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy: Registered with 2 slot"));
		User::Leave(KErrNotSupported);
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CMobilePhoneBookStoreDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
	{
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
	case EMobilePhoneBookStoreRead:
		return Read(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhoneBookStore::TPBIndexAndNumEntries*,dataPtr),
			aPackage.Des2n());

	case EMobilePhoneBookStoreWrite:
		return Write(aTsyReqHandle, aPackage.Des1n(), REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobilePhoneStoreDelete:
		return Delete(aTsyReqHandle,
			REINTERPRET_CAST(TInt*,dataPtr));

	case EMobilePhoneStoreGetInfo:
		return GetInfo(aTsyReqHandle,
			REINTERPRET_CAST(TDes8*, dataPtr));

	case EMobilePhoneStoreNotifyStoreEvent:
	default:
		return KErrNotSupported;
		}
	}

TInt CMobilePhoneBookStoreDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	switch (aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		return GetInfoCancel(aTsyReqHandle);
	case EMobilePhoneBookStoreRead:
		return ReadCancel(aTsyReqHandle);
	case EMobilePhoneBookStoreWrite:
		return WriteCancel(aTsyReqHandle);
	case EMobilePhoneStoreDelete:
		return DeleteCancel(aTsyReqHandle);
	case EMobilePhoneStoreNotifyStoreEvent:
		return KErrNotSupported;
		}
	return KErrGeneral;
	}

TInt CMobilePhoneBookStoreDMmTsy::Read(const TTsyReqHandle aTsyReqHandle,
									   RMobilePhoneBookStore::TPBIndexAndNumEntries* aIndexAndEntries,
									   TDes8* aPBData)
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy::Read called"));

	if (aIndexAndEntries->iIndex!=DMMTSY_PHONEBOOK_STORE_INDEX1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		if(aIndexAndEntries->iNumSlots == DMMTSY_PHONE_ONE_ENTRY)
			*aPBData = DMMTSY_PHONE_BOOK_DATA1;
		else if(aIndexAndEntries->iNumSlots == DMMTSY_PHONE_NUM_OF_ENTRIES)
			*aPBData = DMMTSY_PHONE_BOOK_MULTIPLE_DATA;

		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CMobilePhoneBookStoreDMmTsy::ReadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy::ReadCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobilePhoneBookStoreDMmTsy::Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aPBData, TInt* aIndex)
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy::Write called"));

	if ((*aIndex!=DMMTSY_PHONEBOOK_STORE_INDEX2) ||
		(aPBData->Compare(DMMTSY_PHONE_BOOK_DATA2) != 0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		// Test returned, allocated index
		*aIndex=DMMTSY_PHONEBOOK_STORE_INDEX1;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}

	return KErrNone;
	}

TInt CMobilePhoneBookStoreDMmTsy::WriteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy::WriteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobilePhoneBookStoreDMmTsy::Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex)
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy::Delete called"));

	if (*aIndex!=DMMTSY_PHONEBOOK_STORE_INDEX1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CMobilePhoneBookStoreDMmTsy::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy::DeleteCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobilePhoneBookStoreDMmTsy::GetInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo)
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy::GetInfo called"));

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1* pbInfo = (RMobilePhoneBookStore::TMobilePhoneBookInfoV1*)aInfo;

	if(pbInfo->ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV1)
		{
		pbInfo->iName=DMMTSY_PHONEBOOK_STORE_NAME;
		pbInfo->iType=DMMTSY_PHONEBOOK_STORE_TYPE;
		pbInfo->iTotalEntries=DMMTSY_PHONEBOOK_STORE_ENTRIES;
		pbInfo->iUsedEntries=DMMTSY_PHONEBOOK_STORE_USED;
		pbInfo->iCaps=DMMTSY_PHONEBOOK_STORE_CAPS;
		pbInfo->iMaxNumLength = DMMTSY_PHONEBOOK_MAXNL;
		pbInfo->iMaxTextLength = DMMTSY_PHONEBOOK_MAXTL;
		pbInfo->iLocation = DMMTSY_PHONEBOOK_LOCATION;
		pbInfo->iIdentity = DMMTSY_PHONEBOOK_IDENTITY;
		pbInfo->iChangeCounter = DMMTSY_PHONEBOOK_CHANGECOUNT;
		}
	else if(pbInfo->ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV2)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV2* pbV2Info = (RMobilePhoneBookStore::TMobilePhoneBookInfoV2*)aInfo;
		pbV2Info->iName=DMMTSY_PHONEBOOK_STORE_NAME;
		pbV2Info->iType=DMMTSY_PHONEBOOK_STORE_TYPE;
		pbV2Info->iTotalEntries=DMMTSY_PHONEBOOK_STORE_ENTRIES;
		pbV2Info->iUsedEntries=DMMTSY_PHONEBOOK_STORE_USED;
		pbV2Info->iCaps=DMMTSY_PHONEBOOK_STORE_CAPS;
		pbV2Info->iMaxNumLength = DMMTSY_PHONEBOOK_MAXNL;
		pbV2Info->iMaxTextLength = DMMTSY_PHONEBOOK_MAXTL;
		pbV2Info->iLocation = DMMTSY_PHONEBOOK_LOCATION;
		pbV2Info->iIdentity = DMMTSY_PHONEBOOK_IDENTITY;
		pbV2Info->iChangeCounter = DMMTSY_PHONEBOOK_CHANGECOUNT;
		pbV2Info->iPhBkMode = DMMTSY_PHONEBOOK_TYPE;
		}
	else if(pbInfo->ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV5)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV5* pbV5Info = (RMobilePhoneBookStore::TMobilePhoneBookInfoV5*)aInfo;
		pbV5Info->iName=DMMTSY_PHONEBOOK_STORE_NAME;
		pbV5Info->iType=DMMTSY_PHONEBOOK_STORE_TYPE;
		pbV5Info->iTotalEntries=DMMTSY_PHONEBOOK_STORE_ENTRIES;
		pbV5Info->iUsedEntries=DMMTSY_PHONEBOOK_STORE_USED;
		pbV5Info->iCaps=DMMTSY_PHONEBOOK_STORE_CAPS;
		pbV5Info->iMaxNumLength = DMMTSY_PHONEBOOK_MAXNL;
		pbV5Info->iMaxTextLength = DMMTSY_PHONEBOOK_MAXTL;
		pbV5Info->iLocation = DMMTSY_PHONEBOOK_LOCATION;
		pbV5Info->iIdentity = DMMTSY_PHONEBOOK_IDENTITY;
		pbV5Info->iChangeCounter = DMMTSY_PHONEBOOK_CHANGECOUNT;
		pbV5Info->iPhBkMode = DMMTSY_PHONEBOOK_TYPE;
		pbV5Info->iMaxSecondNames = DMMTSY_PHONEBOOK_MAXSECONDNAMES;
		pbV5Info->iMaxTextLengthSecondName = DMMTSY_PHONEBOOK_MAXTEXTLENSECONDNAME;
		pbV5Info->iMaxAdditionalNumbers = DMMTSY_PHONEBOOK_MAXADDNUM;
		pbV5Info->iMaxTextLengthAdditionalNumber = DMMTSY_PHONEBOOK_MAXTEXTLENADDNUM;
		pbV5Info->iMaxNumLengthAdditionalNumber = DMMTSY_PHONEBOOK_MAXNUMLENADDNUM;
		pbV5Info->iMaxGroupNames = DMMTSY_PHONEBOOK_MAXGROUPNAMES;
		pbV5Info->iMaxTextLengthGroupName = DMMTSY_PHONEBOOK_MAXTEXTLENGROUPNAME;
		pbV5Info->iMaxEmailAddr = DMMTSY_PHONEBOOK_MAXEMAILADDR;
		pbV5Info->iMaxTextLengthEmailAddr = DMMTSY_PHONEBOOK_MAXTEXTLENEMAILADDR;
		}
	iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CMobilePhoneBookStoreDMmTsy::GetInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobilePhoneBookStoreDMmTsy::GetInfoCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
