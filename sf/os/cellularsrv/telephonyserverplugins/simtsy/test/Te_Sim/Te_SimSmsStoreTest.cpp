// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimSmsStoreTest.h"

CSimTestGetSmsList* CSimTestGetSmsList::NewL(RMobilePhoneStore& aStore,TBool aBatch,
								 TInt aStart,TInt aBatchSize,TInt aCount, CSimSmsStoreTest* aTestStep)
	{
    CSimTestGetSmsList* r=new(ELeave) CSimTestGetSmsList(aStore,aBatch,aStart,aBatchSize,aCount,aTestStep);
    CleanupStack::PushL(r);
    r->ConstructL();
	CleanupStack::Pop();
    return r;
    }
      
CSimTestGetSmsList::CSimTestGetSmsList(RMobilePhoneStore& aStore,TBool aBatch,
								 TInt aStart,TInt aBatchSize,TInt aCount, CSimSmsStoreTest* aTestStep)
    : CActive(EPriorityNormal),
	  iStore(aStore),
	  iBatch(aBatch),
	  iStart(aStart),
	  iBatchSize(aBatchSize),
	  iCount(aCount),
	  iTestStep(aTestStep)
    {}

void CSimTestGetSmsList::ConstructL()
	{
	iRetrieve=CRetrieveMobilePhoneSmsList::NewL(iStore,RMobileSmsStore::KETelMobileGsmSmsEntryV1);
    }

CSimTestGetSmsList::~CSimTestGetSmsList()
    {
    delete iRetrieve;
	}

void CSimTestGetSmsList::Start()
    {
    CActiveScheduler::Add(this);
	if(iBatch)
		iRetrieve->StartBatch(iStatus,iStart,iBatchSize);
	else
		iRetrieve->Start(iStatus);
    SetActive();
    }       

void CSimTestGetSmsList::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckGsmResultsL();
	User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}

void CSimTestGetSmsList::DoCancel()
    {
    iRetrieve->Cancel();
    }       

TInt CSimTestGetSmsList::CheckGsmResultsL()
	{
	CMobilePhoneGsmSmsList* smsList=NULL;
	TRAPD(ret,(smsList=iRetrieve->RetrieveGsmListL()));
	if(ret!=KErrNone)
		return ret;

	TInt cnt=smsList->Enumerate();
	for(TInt i=0;i<cnt;i++)
		{
		RMobileSmsStore::TMobileGsmSmsEntryV1 entry=smsList->GetEntryL(i);
		iTestStep->INFO_PRINTF3(_L("Entry #%d, length=%d"),i,entry.iMsgData.Length());
		}

	iTestStep->TESTL(iCount==cnt);

	delete smsList;
	return KErrNone;
	}

CSimSmsStoreTest::CSimSmsStoreTest() 
	{ 
	SetTestStepName(_L("SmsStoreTest"));
	}

TVerdict CSimSmsStoreTest::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Enumerating SMS Messaging Stores"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(8);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSmsMessaging.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened SMS Messaging object"));

	TInt count;
	TESTL(iSmsMessaging.EnumerateMessageStores(count)==KErrNone);
	TESTL(count==1);

	INFO_PRINTF1(_L("Testing Retrieving SMS Messaging Store Informtion"));
	TRequestStatus stat0,stat1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg(storeInfo);
	iSmsMessaging.GetMessageStoreInfo(stat0,0,storeInfoPckg);
	User::WaitForRequest(stat0);
	TESTL(storeInfo.iTotalEntries==10);
	TESTL(storeInfo.iType==RMobilePhoneStore::EShortMessageStore);
	TESTL(storeInfo.iUsedEntries==1);
	TESTL(storeInfo.iCaps & KSmsStoreCaps);
	TESTL(iSmsStore.Open(iSmsMessaging,storeInfo.iName)==KErrNone);
	INFO_PRINTF1(_L("Opened SMS Store object"));

	INFO_PRINTF1(_L("Testing SMS Store GetInfo"));
	iSmsStore.GetInfo(stat0,storeInfoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	CHECKPOINTL(stat0, KErrNone, CHP_SMS_CASE("C.24"));

	TESTL(storeInfo.iType==RMobilePhoneStore::EShortMessageStore);
	TESTL(storeInfo.iTotalEntries==10);
	TESTL(storeInfo.iUsedEntries==1);
	TESTL(storeInfo.iCaps & KSmsStoreCaps);
	TESTL(storeInfo.iName.MatchF(KETelIccSmsStore)==KErrNone);

// Test writing an SMS to the store
	INFO_PRINTF1(_L("Testing SMS Store Write"));

	TUint32 event;
	TInt index;
	iSmsStore.NotifyStoreEvent(stat1,event,index);

	RMobileSmsStore::TMobileGsmSmsEntryV1 smsEntry;
	smsEntry.iMsgData.Copy(KTestPduC);
	smsEntry.iMsgStatus=RMobileSmsStore::EStoredMessageRead;
	smsEntry.iIndex=-1;
	smsEntry.iServiceCentre.iTelNumber=KVodafoneSca;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsEntryPckg(smsEntry);
	iSmsStore.Write(stat0,smsEntryPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	CHECKPOINTL(stat0, KErrNone, CHP_SMS_CASE("C.19"));

	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);
	TESTL(event==(RMobilePhoneStore::KStoreEntryAdded | RMobilePhoneStore::KStoreHasSpace));
	TESTL(index==2);

	iSmsStore.GetInfo(stat0,storeInfoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);

	TESTL(storeInfo.iType==RMobilePhoneStore::EShortMessageStore);
	TESTL(storeInfo.iTotalEntries==10);
	TESTL(storeInfo.iUsedEntries==2);
	TESTL(storeInfo.iCaps & KSmsStoreCaps);
	TESTL(storeInfo.iName.MatchF(KETelIccSmsStore)==KErrNone);

	RMobileSmsStore::TMobileGsmSmsEntryV1 smsEntryforRead;
	smsEntryforRead.iIndex=2;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsEntryforReadPckg(smsEntryforRead);
	INFO_PRINTF1(_L("Testing SMS Store Read"));
	iSmsStore.Read(stat0, smsEntryforReadPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);

// Test reading an empty entry
	smsEntryforRead.iIndex=3;
	iSmsStore.Read(stat0, smsEntryforReadPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNotFound);
	
	INFO_PRINTF1(_L("Testing SMS Store Read Cancel"));
//test get smsstore Cancel
	CSimTestGetSmsList* getSmsList2=CSimTestGetSmsList::NewL(iSmsStore,ETrue,1,2,2,this);
	getSmsList2->Start();//	CHECKPOINT_STR(CHP_SMS_CASE("C.18"));
	getSmsList2->Cancel();//	CHECKPOINT_STR(CHP_SMS_CASE("C.19"));
	User::After(1500000L);
	delete getSmsList2;


// Check retrieve batch SMSs functionality
	INFO_PRINTF1(_L("Testing SMS Store Read All"));
	CSimTestGetSmsList* getSmsList0=CSimTestGetSmsList::NewL(iSmsStore,ETrue,1,2,2,this);
	getSmsList0->Start();
	CActiveScheduler::Start();
	delete getSmsList0;

	INFO_PRINTF1(_L("Testing SMS Store Batch Read"));
// Check retrieve all SMSs functionality
	CSimTestGetSmsList* getSmsList1=CSimTestGetSmsList::NewL(iSmsStore,EFalse,0,0,2,this);
	getSmsList1->Start();
	CActiveScheduler::Start();
	delete getSmsList1;

	INFO_PRINTF1(_L("Testing SMS Store DeleteAll"));
// Now delete everything and check that there are no used entries
	iSmsStore.NotifyStoreEvent(stat1,event,index);
	iSmsStore.DeleteAll(stat0);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	CHECKPOINTL(stat0, KErrNone, CHP_SMS_CASE("C.22"));

	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);
	TESTL(event==(RMobilePhoneStore::KStoreEntryDeleted | RMobilePhoneStore::KStoreEmpty | RMobilePhoneStore::KStoreHasSpace));
	TESTL(index==KErrNotFound);

	iSmsStore.GetInfo(stat0,storeInfoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);

	TESTL(storeInfo.iType==RMobilePhoneStore::EShortMessageStore);
	TESTL(storeInfo.iTotalEntries==10);
	TESTL(storeInfo.iUsedEntries==0);
	TESTL(storeInfo.iCaps & KSmsStoreCaps);
	TESTL(storeInfo.iName.MatchF(KETelIccSmsStore)==KErrNone);

	iSmsStore.Close();
	iSmsMessaging.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

