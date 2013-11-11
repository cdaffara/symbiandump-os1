// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains tests for RMobileSmsStore API
// 
//

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmsmsstore.h"
#include "testdef.h"

CTestSmsStore::CTestSmsStore()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSmsStore"));
	}

enum TVerdict CTestSmsStore::doTestStepL()
//
// Test SMS Storage Functions
//
    {
	
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile SMS Storage Functionality"));
//	LOGTEXT(_L8("Test Mobile SMS Storage Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileSmsMessaging smsMessaging;
	ret=smsMessaging.Open(mmPhone);
    TEST(ret==KErrNone);

	RMobileSmsStore smsStore;
	ret=smsStore.Open(smsMessaging, KETelIccSmsStore);
    TEST(ret==KErrNone);

	TRequestStatus reqStatus;

	// Get SMS store information
	
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 asyncInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg asyncInfoPckg(asyncInfo);
	
	// asynchronous

	smsStore.GetInfo(reqStatus, asyncInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidateSmsStoreInfo(asyncInfo);

	// asynchronous & cancel

	smsStore.GetInfo(reqStatus, asyncInfoPckg);
	smsStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::GetInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateSmsStoreInfo(asyncInfo);
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::GetInfo (async & cancel (Request not cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileSmsStore's Get Info"));

	// Read one entry

	RMobileSmsStore::TMobileGsmSmsEntryV1 asyncEntry1;
	asyncEntry1.iIndex = DMMTSY_SMS_STORE_INDEX1;

	// asynchronous

	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg asyncEntry1Pckg(asyncEntry1);
	smsStore.Read(reqStatus,asyncEntry1Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	ValidateSmsRead(asyncEntry1);

	// asynchronous cancel

	asyncEntry1.iIndex = DMMTSY_SMS_STORE_INDEX1;
	smsStore.Read(reqStatus, asyncEntry1Pckg);
	smsStore.CancelAsyncRequest(EMobilePhoneStoreRead);
	User::WaitForRequest(reqStatus);

	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::Read (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateSmsRead(asyncEntry1);
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::Read (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileSmsStore's Read Entry"));

	// Write one entry

	RMobileSmsStore::TMobileCdmaSmsEntryV1 entry2;
	RMobileSmsStore::TMobileCdmaSmsEntryV1Pckg entry2Pckg(entry2);
	
	// asynchronous

	entry2.iIndex = DMMTSY_SMS_STORE_INDEX2;
	entry2.iMsgData = DMMTSY_SMS_MESSAGE_PDU2;
	entry2.iMsgStatus =DMMTSY_SMS_STORE_STATUS2;
	entry2.iRemoteParty.iTelNumber =DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER2;
	entry2.iTeleservice=DMMTSY_SMS_MESSAGE_TELESERVICE;

	smsStore.Write(reqStatus, entry2Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	smsStore.Write(reqStatus, entry2Pckg);
	smsStore.CancelAsyncRequest(EMobilePhoneStoreWrite);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int() == KErrCancel);

	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::Write (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::Write (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobileSmsStore's Write Entry"));

	// Delete one entry

	// asynchronous

	smsStore.Delete(reqStatus, DMMTSY_SMS_STORE_INDEX3);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asyncrhonous & cancel

	smsStore.Delete(reqStatus, DMMTSY_SMS_STORE_INDEX3);
	smsStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int() == KErrCancel);


	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::Delete (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::Delete (async & cancel (Request not Cancelled)) passed"), iTestCount++);

	// Delete all entries

	// asynchronous

	smsStore.DeleteAll(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous and cancel

	smsStore.DeleteAll(reqStatus);
	smsStore.CancelAsyncRequest(EMobilePhoneStoreDeleteAll);
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::DeleteAll (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::DeleteAll (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobileSmsStore's Delete Entries"));

	// Notify Entry Changed

	// asynchronous

	TUint32 event;
	TInt index;
	smsStore.NotifyStoreEvent(reqStatus,event,index);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(index==DMMTSY_SMS_STORE_INDEX4);
	TEST(event == DMMTSY_MESSAGE_STORE_EVENT);

	// asynchronous cancel

	smsStore.NotifyStoreEvent(reqStatus,event,index);
	smsStore.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::NotifyStoreEvent (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(index==DMMTSY_SMS_STORE_INDEX4);
		TEST(event == DMMTSY_MESSAGE_STORE_EVENT);
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::NotifyStoreEvent (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileSmsStore's Notify Store Event"));

	// Get SMS list using asynchronous AO version
	CTestGetSmsList* getSmsList=CTestGetSmsList::NewLC(smsStore,EFalse,DMMTSY_SMS_STORE_GSMENTRY_TYPE, this);

    getSmsList->Start();
    CActiveScheduler::Start();

	// Ensure that the same AO can be used to retrieve more than one list

    getSmsList->Start();
    CActiveScheduler::Start();

	// I only need to delete the retrieval object because the destructor
	// of this will also delete the list object.
    CleanupStack::PopAndDestroy();  // getSmsList
    
    //Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneSmsList* retrieve=CRetrieveMobilePhoneSmsList::NewL(smsStore,DMMTSY_SMS_STORE_GSMENTRY_TYPE);
    retrieve->Start(reqStatus);
    delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus == KErrCancel);

	// Now test the cancelling

	getSmsList=CTestGetSmsList::NewLC(smsStore,EFalse,DMMTSY_SMS_STORE_GSMENTRY_TYPE, this);
    getSmsList->Start();

	CTestCanceller* cancelGetSmsList = getSmsList->RetrieveCancelPtr();
	cancelGetSmsList->Call();

    CActiveScheduler::Start();
	TBool cancelled = (getSmsList->iStatus == KErrCancel);

	// Ensure that an AO can be used to retrieve a list after the previous request was cancelled

    getSmsList->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::ReadAll (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::ReadAll (sync & async & cancel (Request Not Cancelled)) passed"), iTestCount++);

    CleanupStack::PopAndDestroy();  // getSmsList

	// Get SMS list using asynchronous AO version
	getSmsList=CTestGetSmsList::NewLC(smsStore,ETrue,DMMTSY_SMS_STORE_CDMAENTRY_TYPE, this);
    getSmsList->Start();
    CActiveScheduler::Start();

    getSmsList->Start();
    CActiveScheduler::Start();

	// I only need to delete the retrieval object because the destructor
	// of this will also delete the list object.
    CleanupStack::PopAndDestroy();  // getSmsList

	// Now test the cancelling

	getSmsList=CTestGetSmsList::NewLC(smsStore,ETrue,DMMTSY_SMS_STORE_CDMAENTRY_TYPE, this);
    getSmsList->Start();

	cancelGetSmsList = getSmsList->RetrieveCancelPtr();
	cancelGetSmsList->Call();

    CActiveScheduler::Start();
	cancelled = (getSmsList->iStatus == KErrCancel);

    getSmsList->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::ReadBatch (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsStore::ReadBatch (sync & async & cancel (Request Not Cancelled)) passed"), iTestCount++);

    CleanupStack::PopAndDestroy();  // getSmsList

//	test.Next(_L("OK: RMobileSmsStore's Read All Entries"));

	smsStore.Close();
	smsMessaging.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}	


void CTestSmsStore::ValidateSmsStoreInfo(const RMobilePhoneStore::TMobilePhoneStoreInfoV1 &aSyncInfo)
	{
	TEST(aSyncInfo.iName.Compare(DMMTSY_MESSAGE_STORE_NAME)==0);
	TEST(aSyncInfo.iType==DMMTSY_MESSAGE_STORE_TYPE);
	TEST(aSyncInfo.iTotalEntries==DMMTSY_MESSAGE_STORE_TOTAL_ENTRIES);
	TEST(aSyncInfo.iUsedEntries==DMMTSY_MESSAGE_STORE_USED_ENTRIES);
	TEST(aSyncInfo.iCaps==DMMTSY_MESSAGE_STORE_CAPS);
	TEST(aSyncInfo.ExtensionId()==RMobilePhoneStore::KETelMobilePhoneStoreV1);
	}

void CTestSmsStore::ValidateSmsRead(const RMobileSmsStore::TMobileGsmSmsEntryV1 &aSyncEntry)
	{
	TEST(aSyncEntry.iIndex==DMMTSY_SMS_STORE_INDEX1);
	TEST(aSyncEntry.iMsgData.Compare(DMMTSY_SMS_MESSAGE_PDU1)==0);
	TEST(aSyncEntry.iServiceCentre.iTelNumber.Compare(DMMTSY_PHONE_TEL_NUMBER) == 0);
	TEST(aSyncEntry.iServiceCentre.iTypeOfNumber == DMMTSY_PHONE_TON);
	TEST(aSyncEntry.iServiceCentre.iNumberPlan == DMMTSY_PHONE_NPI);
	TEST(aSyncEntry.iMsgStatus==DMMTSY_SMS_STORE_STATUS1);
	}


/**************************************************************/
//
// Testing asynchronous retrieve of SMS list
//
/**************************************************************/

CTestSmsStore::CTestGetSmsList* CTestSmsStore::CTestGetSmsList::NewLC(RMobilePhoneStore& aStore, TBool aBatch,RMobileSmsStore::TMobileSmsEntryExtensionId aEntryType, CTestSmsStore* aTestSmsStore)
	{
    CTestGetSmsList* r=new(ELeave) CTestGetSmsList(aStore,aBatch,aEntryType,aTestSmsStore);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestSmsStore::CTestGetSmsList::CTestGetSmsList(RMobilePhoneStore& aStore, TBool aBatch,RMobileSmsStore::TMobileSmsEntryExtensionId aEntryType, CTestSmsStore* aTestSmsStore)
    : CActive(EPriorityNormal), iStore(aStore), iBatch(aBatch), iEntryType(aEntryType), iTestSmsStore(aTestSmsStore)
    {}
      

void CTestSmsStore::CTestGetSmsList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneSmsList::NewL(iStore,iEntryType);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestSmsStore::CTestGetSmsList::~CTestGetSmsList()
    {
    Cancel();
    delete iRetrieve;
	delete iCanceller;
	}
      
void CTestSmsStore::CTestGetSmsList::Start()
    {
	if (!iBatch)
		{
		iRetrieve->Start(iStatus);
		TRequestStatus tmpStatus;
		iRetrieve->Start(tmpStatus);
		User::WaitForRequest(tmpStatus);
		iTestSmsStore->TEST(tmpStatus.Int() == KErrInUse);
		}
	else
		{
		iRetrieve->StartBatch(iStatus,DMMTSY_SMS_STORE_START,DMMTSY_SMS_STORE_BATCH);
		TRequestStatus tmpStatus;
		iRetrieve->StartBatch(tmpStatus,DMMTSY_SMS_STORE_START,DMMTSY_SMS_STORE_BATCH);
		User::WaitForRequest(tmpStatus);
		iTestSmsStore->TEST(tmpStatus.Int() == KErrInUse);
		}
    SetActive();
    }       
      
CTestCanceller* CTestSmsStore::CTestGetSmsList::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestSmsStore::CTestGetSmsList::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	if (iEntryType==DMMTSY_SMS_STORE_CDMAENTRY_TYPE)
		ret=CheckCdmaResults();
	else
		ret=CheckGsmResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}
      

void CTestSmsStore::CTestGetSmsList::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      
TInt CTestSmsStore::CTestGetSmsList::CheckGsmResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneGsmSmsList* smsList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, smsList=iRetrieve->RetrieveGsmListL(););
	
	if (smsList)
		{
		iTestSmsStore->TEST(smsList->Enumerate()==DMMTSY_SMS_STORE_COUNT);

		RMobileSmsStore::TMobileGsmSmsEntryV1 entry;

		for (TInt i=0; i<DMMTSY_SMS_STORE_COUNT; ++i)
			{
			TRAP(ret,entry=smsList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestSmsStore->TEST(entry.iIndex==DMMTSY_SMS_STORE_INDEX1);
					iTestSmsStore->TEST(entry.iMsgData.Compare(DMMTSY_SMS_MESSAGE_PDU1) == 0);
					iTestSmsStore->TEST(entry.iServiceCentre.iTelNumber.Compare(DMMTSY_PHONE_TEL_NUMBER) == 0);
					iTestSmsStore->TEST(entry.iServiceCentre.iTypeOfNumber == DMMTSY_PHONE_TON);
					iTestSmsStore->TEST(entry.iServiceCentre.iNumberPlan == DMMTSY_PHONE_NPI);
					iTestSmsStore->TEST(entry.iMsgStatus==DMMTSY_SMS_STORE_STATUS1);
					break;
					}
				case 1:
					{
					iTestSmsStore->TEST(entry.iIndex==DMMTSY_SMS_STORE_INDEX2);
					iTestSmsStore->TEST(entry.iMsgData.Compare(DMMTSY_SMS_MESSAGE_PDU2) == 0);
					iTestSmsStore->TEST(entry.iServiceCentre.iTelNumber.Compare(DMMTSY_SMSP_STORE_SCA) == 0);
					iTestSmsStore->TEST(entry.iServiceCentre.iTypeOfNumber == DMMTSY_PHONE_TON);
					iTestSmsStore->TEST(entry.iServiceCentre.iNumberPlan == DMMTSY_PHONE_NPI);
					iTestSmsStore->TEST(entry.iMsgStatus==DMMTSY_SMS_STORE_STATUS2);
					break;
					}
				case 2:
					{
					iTestSmsStore->TEST(entry.iIndex==DMMTSY_SMS_STORE_INDEX3);
					break;
					}
				case 3:
					{
					iTestSmsStore->TEST(entry.iIndex==DMMTSY_SMS_STORE_INDEX4);
					break;
					}
				default:
					break;
				}
			}
		delete smsList;
		}
	
   return ret;
   }


TInt CTestSmsStore::CTestGetSmsList::CheckCdmaResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneCdmaSmsList* smsList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, smsList=iRetrieve->RetrieveCdmaListL(););

	if (smsList)
		{
		iTestSmsStore->TEST(smsList->Enumerate()==DMMTSY_SMS_STORE_COUNT);

		RMobileSmsStore::TMobileCdmaSmsEntryV1 entry;

		for (TInt i=0; i<DMMTSY_SMS_STORE_COUNT; ++i)
			{
			TRAP(ret,entry=smsList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestSmsStore->TEST(entry.iIndex==DMMTSY_SMS_STORE_INDEX1);
					iTestSmsStore->TEST(entry.iMsgData.Compare(DMMTSY_SMS_MESSAGE_PDU1) == 0);
					iTestSmsStore->TEST(entry.iMsgStatus==DMMTSY_SMS_STORE_STATUS1);
					iTestSmsStore->TEST(entry.iRemoteParty.iTelNumber==DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER1);
					iTestSmsStore->TEST(entry.iTeleservice==DMMTSY_SMS_MESSAGE_TELESERVICE);
					break;
					}
				case 1:
					{
					iTestSmsStore->TEST(entry.iIndex==DMMTSY_SMS_STORE_INDEX2);
					iTestSmsStore->TEST(entry.iMsgData.Compare(DMMTSY_SMS_MESSAGE_PDU2) == 0);
					iTestSmsStore->TEST(entry.iMsgStatus==DMMTSY_SMS_STORE_STATUS2);
					iTestSmsStore->TEST(entry.iRemoteParty.iTelNumber==DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER1);
					iTestSmsStore->TEST(entry.iTeleservice==DMMTSY_SMS_MESSAGE_TELESERVICE);
					break;
					}
				case 2:
					{
					iTestSmsStore->TEST(entry.iIndex==DMMTSY_SMS_STORE_INDEX3);
					iTestSmsStore->TEST(entry.iMsgData.Compare(DMMTSY_SMS_MESSAGE_PDU2) == 0);
					iTestSmsStore->TEST(entry.iMsgStatus==DMMTSY_SMS_STORE_STATUS2);
					iTestSmsStore->TEST(entry.iRemoteParty.iTelNumber==DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER1);
					iTestSmsStore->TEST(entry.iTeleservice==DMMTSY_SMS_MESSAGE_TELESERVICE);
					break;
					}
				case 3:
					{
					iTestSmsStore->TEST(entry.iIndex==DMMTSY_SMS_STORE_INDEX4);
					iTestSmsStore->TEST(entry.iMsgData.Compare(DMMTSY_SMS_MESSAGE_PDU2) == 0);
					iTestSmsStore->TEST(entry.iMsgStatus==DMMTSY_SMS_STORE_STATUS2);
					iTestSmsStore->TEST(entry.iRemoteParty.iTelNumber==DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER1);
					iTestSmsStore->TEST(entry.iTeleservice==DMMTSY_SMS_MESSAGE_TELESERVICE);
					break;
					}
				default:
					break;
				}
			}
		delete smsList;
		}
	
   return ret;
   }
