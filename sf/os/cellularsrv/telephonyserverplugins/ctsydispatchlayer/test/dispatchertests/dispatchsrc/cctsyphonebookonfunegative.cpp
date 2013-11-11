// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ctsyphonebookonfunegative.cpp
//

#include "cctsyphonebookonfunegative.h"

#include <etelmm.h>
#include <mmlist.h>
#include <mmretrieve.h>
#include "config.h"
#include <ctsy/ltsy/mltsydispatchphonebookoninterface.h>
#include <test/tmockltsydata.h>
#include "cctsyphonebookonfu.h"
#include "listretrieverao.h"

/* static */
CTestSuite* CCTsyPhonebookOnFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
  
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFUNegative, TestGetInfoIpcL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFUNegative, TestWriteIpcL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFUNegative, TestReadIpcL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFUNegative, TestDeleteEntryIpcL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFUNegative, TestWriteEntryIpcL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFUNegative, TestDeleteAllIpcL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFUNegative, TestReadEntryIpcL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFUNegative, TestReadSizeIpcL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFUNegative, TestWriteSizeIpcL);

	END_SUITE;
	}

/**
 * Cleanup
 */
void CCTsyPhonebookOnFUNegative::DoCleanup()
    {
    iAdnPhoneBookStore.Close();
    CCTsyPhonebookFU::DoCleanup();
    }

/**
 * Wraps up boilerplate code for starting tests with a clean RPhone session and RMobileONStore
 * initialised. On return this and aONStore have been pushed to the CleanupStack
 */
void CCTsyPhonebookOnFUNegative::OpenAndPushEtelAndPhoneONStoreL(RMobileONStore& aONStore)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    OpenCachingPhoneBookL(DispatcherPhonebook::EIccAdn,iAdnPhoneBookStore,KErrNone);
	
	TInt ret = aONStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(aONStore);
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-NEGATIVE-UN0001
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test support in CTSY for RMobileONStore::GetInfo() when associated PhonebookOn FU isn't supported
 * @SYMTestPriority High
 * @SYMTestActions Disables associated PhonebookOn Api, invokes RMobileONStore::GetInfo()
 * @SYMTestExpectedResults Pass
 * @SYMTestType UT
 */
void CCTsyPhonebookOnFUNegative::TestGetInfoIpcL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId, EFalse);
	
	RMobileONStore onStore;
	OpenAndPushEtelAndPhoneONStoreL(onStore);
	
	TRequestStatus status;

	RMobileONStore::TMobileONStoreInfoV1 onStoreInfoV1;
	TPckg<RMobileONStore::TMobileONStoreInfoV1> pckgInfoV1(onStoreInfoV1);
		
	onStore.GetInfo(status, pckgInfoV1);
	User::WaitForRequest(status);
	
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	
	CleanupStack::PopAndDestroy(2, this);  // onStore, this
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-NEGATIVE-UN0002
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test support in CTSY for RMobileONStore::Read() when associated PhonebookOn FU isn't supported
 * @SYMTestPriority High
 * @SYMTestActions Disables associated PhonebookOn Api, invokes RMobileONStore::Write()
 * @SYMTestExpectedResults Pass
 * @SYMTestType UT
 */
void CCTsyPhonebookOnFUNegative::TestWriteIpcL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, EFalse);
	
	RMobileONStore onStore;
	OpenAndPushEtelAndPhoneONStoreL(onStore);
	
	_LIT(KText1, "Number1");											
	_LIT(KNumber1, "11111111");
	RMobileONStore::TMobileONEntryV1 writeEntry1;
    writeEntry1.iMode = RMobilePhone::ENetworkModeGsm;
    writeEntry1.iService = RMobilePhone::ETelephony;
    writeEntry1.iNumber.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
    writeEntry1.iNumber.iNumberPlan = RMobilePhone::EDataNumberPlan;
    writeEntry1.iNumber.iTelNumber.Copy(KNumber1);
    writeEntry1.iText.Copy(KText1);
    writeEntry1.iIndex = 1;	
	
    TPckg<RMobileONStore::TMobileONEntryV1> pckgWriteEntry1(writeEntry1);
    
    TRequestStatus status;
    onStore.Write(status, pckgWriteEntry1);
	User::WaitForRequest(status);
	
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	
	CleanupStack::PopAndDestroy(2, this);  // onStore, this   
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-NEGATIVE-UN0003
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test support in CTSY for RMobileONStore::Read() when associated PhonebookOn FU isn't supported
 * @SYMTestPriority High
 * @SYMTestActions Disables associated PhonebookOn Api, invokes RMobileONStore::Read()
 * @SYMTestExpectedResults Pass
 * @SYMTestType UT
 */
void CCTsyPhonebookOnFUNegative::TestReadIpcL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId, EFalse);
	
	RMobileONStore onStore;
	OpenAndPushEtelAndPhoneONStoreL(onStore);
	
	TInt location = 1;
	RMobileONStore::TMobileONEntryV1 readEntry;
	readEntry.iIndex = location;
	TPckg<RMobileONStore::TMobileONEntryV1> pckgReadEntry(readEntry);
	
    TRequestStatus status;
    onStore.Read(status, pckgReadEntry);
	User::WaitForRequest(status);
	
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	
	CleanupStack::PopAndDestroy(2, this);  // onStore, this   
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-NEGATIVE-UN0004
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test support in CTSY for RMobileONStore::Read() when associated PhonebookOn FU isn't supported
 * @SYMTestPriority High
 * @SYMTestActions Disables associated PhonebookOn Api, invokes RMobileONStore::Read()
 * @SYMTestExpectedResults Pass
 * @SYMTestType UT
 */
void CCTsyPhonebookOnFUNegative::TestDeleteEntryIpcL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId, EFalse);
	
	RMobileONStore onStore;
	OpenAndPushEtelAndPhoneONStoreL(onStore);
	
	TRequestStatus status;
	onStore.Delete(status, 1);
	User::WaitForRequest(status);
	
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	
	CleanupStack::PopAndDestroy(2, this);  // onStore, this   
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-NEGATIVE-UN0005
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test support in CTSY for RMobileONStore::StoreAll() when associated PhonebookOn FU isn't supported
 * @SYMTestPriority High
 * @SYMTestActions Disables associated PhonebookOn Api, invokes RMobileONStore::StoreAllL()
 * @SYMTestExpectedResults Pass
 * @SYMTestType UT
 */
void CCTsyPhonebookOnFUNegative::TestWriteEntryIpcL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId, EFalse);

	RMobileONStore onStore;
	OpenAndPushEtelAndPhoneONStoreL(onStore);
	
	CMobilePhoneONList* onList = CMobilePhoneONList::NewL();
	CleanupStack::PushL(onList);
	 
	RMobileONStore::TMobileONEntryV1 writeEntry;
	onList->AddEntryL(writeEntry);		// writeEntry not initialised, not checked
	
	TInt size = onList->Enumerate();
	    
	RBuf8 completeData;
	CleanupClosePushL(completeData);
    TMockLtsyData1<TInt> sizeLTsyData(size);
    completeData.Close();
    sizeLTsyData.SerialiseL(completeData);
	    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId, KErrNone, completeData);
	
	TRequestStatus status;
	onStore.StoreAllL(status, onList);
	User::WaitForRequest(status);
	
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	
	CleanupStack::PopAndDestroy(4, this);  // completeData, onList, onStore, this   
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-NEGATIVE-UN0006
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test support in CTSY for RMobileONStore::DeleteAll() when associated PhonebookOn FU isn't supported
 * @SYMTestPriority High
 * @SYMTestActions Disables associated PhonebookOn Api, invokes RMobileONStore::DeleteAllL()
 * @SYMTestExpectedResults Pass
 * @SYMTestType UT
 */
void CCTsyPhonebookOnFUNegative::TestDeleteAllIpcL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, EFalse);

	RMobileONStore onStore;
	OpenAndPushEtelAndPhoneONStoreL(onStore);

	TRequestStatus status;	
	onStore.DeleteAll(status);
	User::WaitForRequest(status);
	
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	
	CleanupStack::PopAndDestroy(2, this);  // onStore, this   
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-NEGATIVE-UN0007
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test support in CTSY for RMobileONStore::ReadAll() when associated PhonebookOn FU isn't supported
 * @SYMTestPriority High
 * @SYMTestActions Disables associated PhonebookOn Api, invokes CRetrieveMobilePhoneONList
 * @SYMTestExpectedResults Pass
 * @SYMTestType UT
 */
void CCTsyPhonebookOnFUNegative::TestReadEntryIpcL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId, EFalse);

	RMobileONStore onStore;
	OpenAndPushEtelAndPhoneONStoreL(onStore);
	
	//CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler();
	//CleanupStack::PushL(activeScheduler);
	//CActiveScheduler::Install(activeScheduler);
	
		RBuf8 completeData;
		CleanupClosePushL(completeData);	
		TInt size = 1;
		TMockLtsyData1<TInt> sizeLTsyData(size);
		completeData.Close();
		sizeLTsyData.SerialiseL(completeData);
		
		iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId);
		iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId, KErrNone, completeData);
//	
//		_LIT(KText, "Number%d");
//		_LIT(KNumber, "0000000%d");
//		TInt location[1];
//		TBuf<RMobileONStore::KOwnNumberTextSize> name[1];
//		TBuf<KPBStoreNumSize> telNumber[1];
//		TInt index = 1;
//		location[0] = index;	
//		name[0].Format(KText,index);	
//		telNumber[0].Format(KNumber,index);
//		
//		RBuf8 expectReadData;
//		CleanupClosePushL(expectReadData);
//		TMockLtsyData1<TInt> locationLTsyData(location[0]);
//		expectReadData.Close();
//		locationLTsyData.SerialiseL(expectReadData);
//		
//		TMockLtsyData3< TInt, TBuf<RMobileONStore::KOwnNumberTextSize>, TBuf<KPBStoreNumSize> > readLTsyDataComplete(location[0], name[0], telNumber[0]);
//		completeData.Close();
//		readLTsyDataComplete.SerialiseL(completeData);
//		iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId, expectReadData);
//		iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId, KErrNone, completeData);

	//	CRetrieveMobilePhoneONList* onListRetriever = CRetrieveMobilePhoneONList::NewL(onStore);
	//	CleanupStack::PushL(onListRetriever);
	TRequestStatus status;
	//onListRetriever->Start(status);
	//TBool test = onListRetriever->IsActive();
	//onListRetriever->RetrieveListL();
	//CActiveScheduler::Start();
	
	CGetONListAO* getOnListAO = CGetONListAO::NewLC(onStore);
	getOnListAO->TestRetrieveL();
	ASSERT_EQUALS(KErrNotSupported, getOnListAO->RetrieveLastError());
	
	//User::WaitForRequest(status);
	
	//ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();
	
	config.Reset();
	
	//CActiveScheduler::Stop();
	CleanupStack::PopAndDestroy(4, this);  // getOnListAO, completeData, onStore, this   
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-NEGATIVE-UN0008
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test support in CTSY for RMobileONStore::ReadAll() when associated PhonebookOn FU isn't supported
 * @SYMTestPriority High
 * @SYMTestActions Disables associated PhonebookOn Api, invokes CRetrieveMobilePhoneONList
 * @SYMTestExpectedResults Pass
 * @SYMTestType UT
 */
void CCTsyPhonebookOnFUNegative::TestReadSizeIpcL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId, EFalse);

	RMobileONStore onStore;
	OpenAndPushEtelAndPhoneONStoreL(onStore);	

	TRequestStatus status;
	CGetONListAO* getOnListAO = CGetONListAO::NewLC(onStore);
	getOnListAO->TestRetrieveL();
	ASSERT_EQUALS(KErrNotSupported, getOnListAO->RetrieveLastError());
	
	AssertMockLtsyStatusL();
	
	config.Reset();
	
	CleanupStack::PopAndDestroy(3, this);  // getOnListAO, onStore, this   
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-NEGATIVE-UN0009
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test support in CTSY for RMobileONStore::StoreAll() when associated PhonebookOn FU isn't supported
 * @SYMTestPriority High
 * @SYMTestActions Disables associated PhonebookOn Api, invokes RMobileONStore::StoreAllL()
 * @SYMTestExpectedResults Pass
 * @SYMTestType UT
 */
void CCTsyPhonebookOnFUNegative::TestWriteSizeIpcL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId, EFalse);

	RMobileONStore onStore;
	OpenAndPushEtelAndPhoneONStoreL(onStore);
	
	CMobilePhoneONList* onList = CMobilePhoneONList::NewL();
	CleanupStack::PushL(onList);

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);
	
	TRequestStatus status;	
	onStore.StoreAllL(status, onList);
	User::WaitForRequest(status);
	
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	
	CleanupStack::PopAndDestroy(3, this);  // onList, onStore, this   
	}
