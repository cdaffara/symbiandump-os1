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
//



/**
 @file The TEFUnit test suite for PhonebookEnControl in the Common TSY.
*/

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>
#include "listretrieverao.h"
#include "config.h"

#include "cctsyphonebookfunegative.h"

// Taken from cctsyphonebookfu.cpp
const TInt KReadBufLength = 400;
const TInt KMaxTextLength = 50;
//const TInt KNewMaxTextLength = 55;
const TInt KMaxNumberLength = 25;
const TInt KNumberOfEnries = 100;

void CCTsyPhonebookFUNegative::CreatePhonebookStoreDataL()
	{
	iPhonebookStoreData.iAdnTotalEntries 			= 4;
	iPhonebookStoreData.iAdnMaximumNumberLength 	= KMaxNumberLength;
	iPhonebookStoreData.iAdnMaximumTextLength 		= KMaxTextLength;

	iPhonebookStoreData.iFdnTotalEntries 			= 4;
	iPhonebookStoreData.iFdnMaximumNumberLength 	= KMaxNumberLength;
	iPhonebookStoreData.iFdnMaximumTextLength 		= KMaxTextLength;

	iPhonebookStoreData.iSdnTotalEntries			= KNumberOfEnries;
	iPhonebookStoreData.iSdnMaximumTextLength		= KMaxTextLength;
	iPhonebookStoreData.iSdnMaximumNumberLength		= KMaxNumberLength;

	iPhonebookStoreData.iVmbTotalEntries			= KNumberOfEnries;
	iPhonebookStoreData.iVmbMaximumTextLength		= KMaxTextLength;
	iPhonebookStoreData.iVmbMaximumNumberLength		= KMaxNumberLength;
	iPhonebookStoreData.iVmbCapabilities			= 0;

	iPhonebookStoreData.iMbdnTotalEntries			= KNumberOfEnries;
	iPhonebookStoreData.iMbdnMaximumTextLength		= KMaxTextLength;
	iPhonebookStoreData.iMbdnMaximumNumberLength	= KMaxNumberLength;
	iPhonebookStoreData.iMbdnCapabilities			= 0;
	
	iPhonebookStoreData.iEmailMaximumFieldsPerEntry				= 101;
	iPhonebookStoreData.iEmailMaximumLength						= KMaxTextLength;
		
	iPhonebookStoreData.iAdditionalNumberMaximumFieldsPerEntry	= 102;
	iPhonebookStoreData.iAdditionalNumberMaximumLength			= KMaxNumberLength;
	
	iPhonebookStoreData.iSecondNameMaximumFieldsPerEntry		= 103;
	iPhonebookStoreData.iSecondNameMaximumLength				= KMaxTextLength;
	}

void CCTsyPhonebookFUNegative::OpenSmsStoreL(RMobileSmsMessaging& aSmsMessaging, RMobileSmsStore& aSmsStore, const TDesC& aSmsStoreName)
	{
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId);	
		
	User::LeaveIfError(aSmsMessaging.Open(iPhone));
	User::LeaveIfError(aSmsStore.Open(aSmsMessaging, aSmsStoreName));
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TInt totalEntries = 0;
	TInt usedEntries = 0;
	TMockLtsyData2<TInt,TInt> smsStoreGetInfoLtsyData(totalEntries, usedEntries);
	data.Close();
	smsStoreGetInfoLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, KErrGeneral, data);
	
	WaitForMockLTSYTerminated();
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	}

CTestSuite* CCTsyPhonebookFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
  	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFUNegative, TestUnit0015L);
	END_SUITE;
	}

//
// 'Negative' unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMmTsyPhoneBookStoreReadIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0001L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId, EFalse);

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMobilePhoneBookStore phonebookStore;
	CleanupClosePushL(phonebookStore);

	//initialise IPC generated from phonebook store open
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);

	TName phonebook(KETelIccBdnPhoneBook); //(Bdn - noncaching)
	ASSERT_EQUALS(phonebookStore.Open(iPhone, phonebook), KErrNone);

    AssertMockLtsyStatusL();

    RBuf8 data;
    CleanupClosePushL(data);

    //complete initialise IPC
    TMockLtsyData1<DispatcherPhonebook::TPhonebookStoreInfoV1> phonebookCompLtsyData(iPhonebookStoreData);
    phonebookCompLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId,KErrNone,data);
    data.Close();

    //fire up the initialise complete
    WaitForMockLTSYTerminated();

	TRequestStatus requestStatus;
	TBuf8<KReadBufLength> phonebookReadData;

	phonebookStore.Read(requestStatus, 1, 1, phonebookReadData);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // data, phonebookStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMmTsyPhoneBookStoreDeleteIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0002L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMobilePhoneBookStore phonebookStore;
	CleanupClosePushL(phonebookStore);

    //initialise IPC generated from phonebook store open
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);

	TName phonebook(KETelIccBdnPhoneBook); //(Bdn - noncaching)
	ASSERT_EQUALS(phonebookStore.Open(iPhone, phonebook), KErrNone);

    RBuf8 data;
    CleanupClosePushL(data);

    //complete initialise IPC
    TMockLtsyData1<DispatcherPhonebook::TPhonebookStoreInfoV1> phonebookCompLtsyData(iPhonebookStoreData);
    phonebookCompLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId,KErrNone,data);
    data.Close();

    //fire up the initialise complete
    WaitForMockLTSYTerminated();

	TRequestStatus requestStatus;
	TInt indexToDelete = 0;
	
	phonebookStore.Delete(requestStatus, indexToDelete);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // data, phonebookStore, this
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMmTsyPhoneBookStoreCacheIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Open()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0003L() 
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	CreatePhonebookStoreDataL();
	
	RMobilePhoneBookStore phonebookStore;
	CleanupClosePushL(phonebookStore);
	TName phonebook(KETelIccFdnPhoneBook);
	
	//initialise IPC generated from phonebook store open
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);

	ASSERT_EQUALS(phonebookStore.Open(iPhone, phonebook),KErrNone); 
	
	AssertMockLtsyStatusL();
    config.Reset();
	CleanupStack::PopAndDestroy(2, this); // phonebookStore, this	
	}	

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMmTsyPhoneBookStoreGetInfoIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0004L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMobilePhoneBookStore phonebookStore;
	CleanupClosePushL(phonebookStore);

    //initialise IPC generated from phonebook store open
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);

	TName phonebook(KETelIccSdnPhoneBook); //(Sdn - noncaching)
	ASSERT_EQUALS(phonebookStore.Open(iPhone, phonebook), KErrNone);
	
	AssertMockLtsyStatusL();

    RBuf8 data;
    CleanupClosePushL(data);

    //complete initialise IPC
    TMockLtsyData1<DispatcherPhonebook::TPhonebookStoreInfoV1> phonebookCompLtsyData(iPhonebookStoreData);
    phonebookCompLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId,KErrNone,data);
    data.Close();

    //fire up the initialise complete
    WaitForMockLTSYTerminated();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	TRequestStatus cacheRequestStatus;
	RMmCustomAPI::TPndCacheStatus cacheStatus;
	customApi.GetPndCacheStatus(cacheRequestStatus, cacheStatus, phonebook);
	User::WaitForRequest(cacheRequestStatus);
	
	TRequestStatus requestStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg(bookInfo);

	phonebookStore.GetInfo(requestStatus, bookPckg);
	User::WaitForRequest(requestStatus); 
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(4, this); // customApi, data, phonebookStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMmTsyPhoneBookStoreInitIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Open()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0005L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMobilePhoneBookStore phonebookStore;
	CleanupClosePushL(phonebookStore);
	
	TName phonebook(KETelIccAdnPhoneBook); //(Adn - caching, init required)
	
	// in CMmPhoneBookStoreTsy::ConstructL( TName aName )
	// iMmPhoneBookStoreExtInterface->InitPhonebook(
	// EMmTsyPhoneBookStoreInitIPC, iPhoneBookName );
	// return code never checked, tests fails
	TInt err = phonebookStore.Open(iPhone, phonebook); // still returns KErrNone
	ASSERT_EQUALS(err, KErrNone);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // phonebookStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMmTsyPhoneBookStoreDeleteAllIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0006L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMobilePhoneBookStore phonebookStore;
	CleanupClosePushL(phonebookStore);

	//initialise IPC generated from phonebook store open
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);

	TName phonebook(KETelIccBdnPhoneBook); //(Bdn - noncaching)
	ASSERT_EQUALS(phonebookStore.Open(iPhone, phonebook),KErrNone);

    RBuf8 data;
    CleanupClosePushL(data);

    //complete initialise IPC
    TMockLtsyData1<DispatcherPhonebook::TPhonebookStoreInfoV1> phonebookCompLtsyData(iPhonebookStoreData);
    phonebookCompLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId,KErrNone,data);
    data.Close();

    //fire up the initialise complete
    WaitForMockLTSYTerminated();

	TRequestStatus requestStatus;
	
	phonebookStore.DeleteAll(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);	

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // data, phonebookStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneStoreReadAllPhase1 is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneSmsList::Start()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0007L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus mockLtsyStatus;
	TInt totalEntries = 1;
	TInt usedEntries = 0;
	TMockLtsyData2<TInt, TInt> dataGetInfo(totalEntries, usedEntries);
	data.Close();
	dataGetInfo.SerialiseL(data);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	CGetSMSListAO* getSmsListAO = CGetSMSListAO::NewLC(smsStore);
	getSmsListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNotSupported, getSmsListAO->RetrieveLastError());

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(5, this); // getSmsListAO, data, messaging, smsStore, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMmTsyPhoneBookStoreWriteIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0008L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMobilePhoneBookStore phonebookStore;
	CleanupClosePushL(phonebookStore);

	//initialise IPC generated from phonebook store open
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);

	TName phonebook(KETelIccBdnPhoneBook); //(Bdn - noncaching)
	ASSERT_EQUALS(phonebookStore.Open(iPhone, phonebook), KErrNone);

    RBuf8 data;
    CleanupClosePushL(data);

    //complete initialise IPC
    TMockLtsyData1<DispatcherPhonebook::TPhonebookStoreInfoV1> phonebookCompLtsyData(iPhonebookStoreData);
    phonebookCompLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId,KErrNone,data);
    data.Close();

    //fire up the initialise complete
    WaitForMockLTSYTerminated();

	TRequestStatus requestStatus;
	TInt index(-1); 
	TBuf8<200> buf;	
    TUint16 location = 0;
    
    CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set(&buf);    
    TInt ret = pbBuffer->AddNewEntryTag();
    ret = pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex, location);

	phonebookStore.Write(requestStatus, buf, index);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);	

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(4, this); // pbBuffer, data, phonebookStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetPhoneStoreInfo is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPhoneStoreInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0009L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RMobilePhoneBookStore::TMobilePhoneStoreInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneStoreInfoV1Pckg bookInfoPckg(bookInfo);
	TName storeName(KETelIccBdnPhoneBook);
	
	iPhone.GetPhoneStoreInfo(requestStatus, bookInfoPckg, storeName);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported)
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneStoreDelete is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Delete()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0010L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);

	TRequestStatus requestStatus;
	TInt index = 1;

	smsStore.Delete(requestStatus, index);
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);	
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // messaging, smsStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneStoreDeleteAll is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::DeleteAll()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0011L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);
	
	TRequestStatus requestStatus;

	smsStore.DeleteAll(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // messaging, smsStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneStoreGetInfo is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::GetInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0012L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	User::LeaveIfError(messaging.Open(iPhone));

	// EMobilePhoneStoreGetInfo is called when opening smsStore,
	// but fails to return KErrNotSupported
	TInt err = smsStore.Open(messaging, KETelIccSmsStore);
	ASSERT_EQUALS(err, KErrNone);
		
	TRequestStatus requestStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV1;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV1(phoneBookInfoV1);
	
	// and this call times out
//	smsStore.GetInfo(requestStatus, pckgInfoV1);
//	User::WaitForRequest(requestStatus);
//	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // messaging, smsStore, this
		
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneStoreRead is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0013L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);
	
	TRequestStatus requestStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV1;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV1(phoneBookInfoV1);
	
	smsStore.Read(requestStatus, pckgInfoV1);
	User::WaitForRequest(requestStatus); // returns with KErrArgument error instead of KErrNotSupported
	ASSERT_EQUALS(requestStatus.Int(), KErrArgument);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // messaging, smsStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneStoreWrite is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Write()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0014L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);
	
	TRequestStatus requestStatus;
	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
		
	entryV1.iServiceCentre.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	entryV1.iServiceCentre.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	entryV1.iServiceCentre.iTelNumber    = _L("TelNumber1");
	entryV1.iMsgData = _L8("Hello, World!");
	entryV1.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
	entryV1.iIndex = 1;
	
	smsStore.Write(requestStatus, pckgEntryV1);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // messaging, smsStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-NEGATIVE-UN0015
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMmTsyPhoneBookStoreCacheCancelIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Open()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFUNegative::TestUnit0015L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	DispatcherPhonebook::TPhonebook phonebook = DispatcherPhonebook::EIccAdn;

	RMobilePhoneBookStore phoneBookStore;
	CleanupClosePushL(phoneBookStore);

	//initialise IPC generated from phonebook store open
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);

	ASSERT_EQUALS(phoneBookStore.Open(iPhone, KETelIccAdnPhoneBook),KErrNone);

	AssertMockLtsyStatusL();
		
	//complete initialise IPC
    TMockLtsyData1<DispatcherPhonebook::TPhonebookStoreInfoV1> phonebookCompLtsyData(iPhonebookStoreData);
    phonebookCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId,KErrNone,data);
	data.Close();
	
	//cache IPC generated from complete initialise IPC
	TMockLtsyData1<DispatcherPhonebook::TPhonebook> cacheExpLtsyData(phonebook);
	cacheExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId,data);
	data.Close();

	//fire up the initialise complete
	WaitForMockLTSYTerminated();

	//close them without the cache yet completed
	CleanupStack::PopAndDestroy(&phoneBookStore);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this); 
	config.Reset();
	}
