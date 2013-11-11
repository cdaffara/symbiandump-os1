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
 @file The TEFUnit test suite for PhonebookControl in the Common TSY.
*/

#include "cctsyphonebookfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>

#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>

#include <ctsy/ltsy/cphonebookentry.h>

#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/mltsydispatchsmsinterface.h>

#include "mockltsyindicatorids.h"

#include <ctsy/ltsy/cctsydispatchercallback.h>

#include "listretrieverao.h"

const TInt KReadBufLength = 400;
const TInt KMaxTextLength = 50;
const TInt KNewMaxTextLength = 55;
const TInt KMaxNumberLength = 25;
const TInt KNumberOfEnries = 100;

CTestSuite* CCTsyPhonebookFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	//add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUseCase0014L);
	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookFU, TestUnit0012L);

	END_SUITE;
	}


//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Opening all supported phonebooks
@SYMTestPriority High
@SYMTestActions Opens caching and non caching phonebooks in different orders
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0001L()
	{
	CreateEntriesAndStoreDataL();
	
	/*********************Open all phone books - caching PB first*****************************/

	OpenEtelServerL(EUseExtendedError);

	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RArray<DispatcherPhonebook::TPhonebook> names;
	CleanupClosePushL(names);
	names.AppendL(DispatcherPhonebook::EIccAdn); 
	names.AppendL(DispatcherPhonebook::EIccFdn);
	names.AppendL(DispatcherPhonebook::EIccBdn); 
	names.AppendL(DispatcherPhonebook::EIccSdn);
	names.AppendL(DispatcherPhonebook::EIccVmb); 
	names.AppendL(DispatcherPhonebook::EIccMbdn);

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	TInt nameCount = names.Count();
	for(TInt i = 0; i < nameCount; ++i)
		{
		RMobilePhoneBookStore phonebookStore;
		CleanupClosePushL(phonebookStore);
		OpenPhoneBookL(names[i], phonebookStore, customApi);
		}

	CleanupStack::PopAndDestroy(nameCount); //the phonebooks
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&names);
	CleanupStack::PopAndDestroy(this);

	/*********************Open all phone books - non caching PB first*****************************/
	
	OpenEtelServerL(EUseExtendedError);

	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	
	CleanupClosePushL(names);
	names.AppendL(DispatcherPhonebook::EIccBdn); 
	names.AppendL(DispatcherPhonebook::EIccFdn); 
	names.AppendL(DispatcherPhonebook::EIccAdn); 
	names.AppendL(DispatcherPhonebook::EIccSdn);
	names.AppendL(DispatcherPhonebook::EIccVmb); 
	names.AppendL(DispatcherPhonebook::EIccMbdn);

	OpenCustomApiLC(customApi);
	
	nameCount = names.Count();
	for(TInt j = 0; j < nameCount; ++j)
		{
		RMobilePhoneBookStore phonebookStore;
		CleanupClosePushL(phonebookStore);
		OpenPhoneBookL(names[j], phonebookStore, customApi);
		}

	CleanupStack::PopAndDestroy(nameCount); //the phonebooks
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&names);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Opening all supported phonebooks interleaving the cache completes
@SYMTestPriority High
@SYMTestActions Open several phonebooks completing the cache operations between non caching phonebooks
being opened.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneL();
	
	CreateEntriesAndStoreDataL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus requestStatus;

	//repeat to test stored CTSY pointers are reset to NULL in the dispatcher
	for(TInt i = 0; i < 2; ++i)
		{
		RMobilePhoneBookStore adnPhoneBookStore;
		CleanupClosePushL(adnPhoneBookStore);

		RMobilePhoneBookStore fdnPhoneBookStore;
		CleanupClosePushL(fdnPhoneBookStore);

		RMobilePhoneBookStore bdnPhoneBookStore;
		CleanupClosePushL(bdnPhoneBookStore);

		RMobilePhoneBookStore sdnPhoneBookStore;
		CleanupClosePushL(sdnPhoneBookStore);

		RMobilePhoneBookStore vmbPhoneBookStore;
		CleanupClosePushL(vmbPhoneBookStore);

		OpenCachingPhoneBookL(DispatcherPhonebook::EIccAdn,adnPhoneBookStore,KErrNone);

		OpenNonCachingPhoneBookL(DispatcherPhonebook::EIccVmb,vmbPhoneBookStore,KErrNone);

		OpenCachingPhoneBookL(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,KErrNone);
	
		OpenNonCachingPhoneBookL(DispatcherPhonebook::EIccBdn,bdnPhoneBookStore,KErrNone);

		//Custom API can only wait on one phonebook at a time.
		TName adnPhonebookName(PhonebookName(DispatcherPhonebook::EIccAdn));
		customApi.NotifyPndCacheReady(requestStatus,adnPhonebookName);
		CompleteCacheL(DispatcherPhonebook::EIccAdn, KErrNone, EFalse);
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();
		
		OpenNonCachingPhoneBookL(DispatcherPhonebook::EIccSdn,sdnPhoneBookStore,KErrNone);

		TName fdnPhonebookName(PhonebookName(DispatcherPhonebook::EIccFdn));
		customApi.NotifyPndCacheReady(requestStatus,fdnPhonebookName);
		CompleteCacheL(DispatcherPhonebook::EIccFdn, KErrNone, EFalse);
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();

		CleanupStack::PopAndDestroy(&vmbPhoneBookStore);
		CleanupStack::PopAndDestroy(&sdnPhoneBookStore);
		CleanupStack::PopAndDestroy(&bdnPhoneBookStore);
		CleanupStack::PopAndDestroy(&fdnPhoneBookStore);
		CleanupStack::PopAndDestroy(&adnPhoneBookStore);
		}

	CleanupStack::PopAndDestroy(&customApi);
	
	CleanupStack::PopAndDestroy(this);
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test Read for all supported phonebooks
@SYMTestPriority High
@SYMTestActions Open the phone book and read all entries from caching and non-caching phonebooks - RMobilePhoneBookStore::Read()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	CreateEntriesAndStoreDataL();

	OpenPhoneL();
	
	ReadL(DispatcherPhonebook::EIccAdn,KErrNone);
	ReadL(DispatcherPhonebook::EIccAdn,KErrGeneral);

	ReadL(DispatcherPhonebook::EIccFdn,KErrNone);
	ReadL(DispatcherPhonebook::EIccFdn,KErrGeneral);

	ReadL(DispatcherPhonebook::EIccBdn,KErrNone);
	ReadL(DispatcherPhonebook::EIccBdn,KErrGeneral);

	ReadL(DispatcherPhonebook::EIccSdn,KErrNone);
	ReadL(DispatcherPhonebook::EIccSdn,KErrGeneral);

	ReadL(DispatcherPhonebook::EIccVmb,KErrNone);
	ReadL(DispatcherPhonebook::EIccVmb,KErrGeneral);

	ReadL(DispatcherPhonebook::EIccMbdn,KErrNone);
	ReadL(DispatcherPhonebook::EIccMbdn,KErrGeneral);
	
	CleanupStack::PopAndDestroy(this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test Read for all supported phonebooks interleaving the non-cache reads
@SYMTestPriority High
@SYMTestActions Open all phonebooks and read enties interleaving the reads - RMobilePhoneBookStore::Read()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	CreateEntriesAndStoreDataL();

	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RMobilePhoneBookStore adnPhoneBookStore;
	CleanupClosePushL(adnPhoneBookStore);
	OpenPhoneBookL(DispatcherPhonebook::EIccAdn,adnPhoneBookStore,customApi);

	RMobilePhoneBookStore fdnPhoneBookStore;
	CleanupClosePushL(fdnPhoneBookStore);
	OpenPhoneBookL(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,customApi);

	RMobilePhoneBookStore bdnPhoneBookStore;
	CleanupClosePushL(bdnPhoneBookStore);
	OpenPhoneBookL(DispatcherPhonebook::EIccBdn,bdnPhoneBookStore,customApi);

	RMobilePhoneBookStore sdnPhoneBookStore;
	CleanupClosePushL(sdnPhoneBookStore);
	OpenPhoneBookL(DispatcherPhonebook::EIccSdn,sdnPhoneBookStore,customApi);

	RMobilePhoneBookStore vmbPhoneBookStore;
	CleanupClosePushL(vmbPhoneBookStore);
	OpenPhoneBookL(DispatcherPhonebook::EIccVmb,vmbPhoneBookStore,customApi);

	RMobilePhoneBookStore mbdnPhoneBookStore;
	CleanupClosePushL(mbdnPhoneBookStore);
	OpenPhoneBookL(DispatcherPhonebook::EIccMbdn,mbdnPhoneBookStore,customApi);

	//repeat to test stored CTSY pointers are reset to NULL in the dispatcher
	for(TInt i = 0; i < 2; ++i)
		{
		TRequestStatus adnRequestStatus;
		TBuf8<KReadBufLength> adnData;
		ExpectNonCacheReadL(DispatcherPhonebook::EIccAdn,adnPhoneBookStore,1,1,adnRequestStatus,adnData);
	
		TRequestStatus fdnRequestStatus;
		TBuf8<KReadBufLength> fdnData;
		ExpectNonCacheReadL(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,1,1,fdnRequestStatus,fdnData);
	
		TRequestStatus bdnRequestStatus;
		TBuf8<KReadBufLength> bdnData;
		ExpectNonCacheReadL(DispatcherPhonebook::EIccBdn,bdnPhoneBookStore,1,1,bdnRequestStatus,bdnData);
	
		TRequestStatus sdnRequestStatus;
		TBuf8<KReadBufLength> sdnData;
		ExpectNonCacheReadL(DispatcherPhonebook::EIccSdn,sdnPhoneBookStore,1,1,sdnRequestStatus,sdnData);
	
		TRequestStatus vmbRequestStatus;
		TBuf8<KReadBufLength> vmbData;
		ExpectNonCacheReadL(DispatcherPhonebook::EIccVmb,vmbPhoneBookStore,1,1,vmbRequestStatus,vmbData);
	
		TRequestStatus mbdnRequestStatus;
		TBuf8<KReadBufLength> mbdnData;
		ExpectNonCacheReadL(DispatcherPhonebook::EIccMbdn,mbdnPhoneBookStore,1,1,mbdnRequestStatus,mbdnData);
		
		//we now have size outstanding reads (thus six CTSY pointers held in the dispatcher)
		
		CompleteNonCacheReadL(DispatcherPhonebook::EIccAdn,	1, 1, adnRequestStatus, adnData, KErrNone);
		CompleteNonCacheReadL(DispatcherPhonebook::EIccFdn,	1, 1, fdnRequestStatus, fdnData, KErrNone);
		CompleteNonCacheReadL(DispatcherPhonebook::EIccBdn,	1, 1, bdnRequestStatus, bdnData, KErrNone);
		CompleteNonCacheReadL(DispatcherPhonebook::EIccSdn,	1, 1, sdnRequestStatus, sdnData, KErrNone);
		CompleteNonCacheReadL(DispatcherPhonebook::EIccVmb,	1, 1, vmbRequestStatus, vmbData, KErrNone);
		CompleteNonCacheReadL(DispatcherPhonebook::EIccMbdn,	1, 1, mbdnRequestStatus, mbdnData, KErrNone);
		}

	CleanupStack::PopAndDestroy(&mbdnPhoneBookStore); //phonebooks
	CleanupStack::PopAndDestroy(&vmbPhoneBookStore); //phonebooks
	CleanupStack::PopAndDestroy(&sdnPhoneBookStore); //phonebooks
	CleanupStack::PopAndDestroy(&bdnPhoneBookStore); //phonebooks
	CleanupStack::PopAndDestroy(&fdnPhoneBookStore); //phonebooks
	CleanupStack::PopAndDestroy(&adnPhoneBookStore); //phonebooks

	CleanupStack::PopAndDestroy(&customApi);
	
	CleanupStack::PopAndDestroy(this); // data, this

	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for deleting entries from phonebooks
@SYMTestPriority High
@SYMTestActions Open all phone books (excluding SDN, can't be updated by user) and delete all entires one by one - RMobilePhoneBookStore::Delete()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	CreateEntriesAndStoreDataL();

	OpenPhoneL();
	
	TInt indexToDelete = 1;
	
	DeleteL(DispatcherPhonebook::EIccAdn, indexToDelete, KErrNone);
	DeleteL(DispatcherPhonebook::EIccAdn, indexToDelete, KErrNotFound);
		
	DeleteL(DispatcherPhonebook::EIccFdn, indexToDelete, KErrNone);
	DeleteL(DispatcherPhonebook::EIccFdn, indexToDelete, KErrNotFound);

	DeleteL(DispatcherPhonebook::EIccBdn, indexToDelete, KErrNone);
	DeleteL(DispatcherPhonebook::EIccBdn, indexToDelete, KErrNotFound);

	DeleteL(DispatcherPhonebook::EIccVmb, indexToDelete, KErrNone);
	DeleteL(DispatcherPhonebook::EIccVmb, indexToDelete, KErrNotFound);

	DeleteL(DispatcherPhonebook::EIccMbdn, indexToDelete, KErrNone);
	DeleteL(DispatcherPhonebook::EIccMbdn, indexToDelete, KErrNotFound);

	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for deleting all entries from phonebooks
@SYMTestPriority High
@SYMTestActions Open all phonebooks (excluding SDN, can't be updated by user) and delete all the entries - RMobilePhoneBookStore::DeleteAll()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	CreateEntriesAndStoreDataL();

	OpenPhoneL();
	
	DeleteAllL(DispatcherPhonebook::EIccAdn, KErrNone);
	DeleteAllL(DispatcherPhonebook::EIccAdn, KErrNotFound);
	
	DeleteAllL(DispatcherPhonebook::EIccFdn, KErrNone);
	DeleteAllL(DispatcherPhonebook::EIccFdn, KErrNotFound);

	DeleteAllL(DispatcherPhonebook::EIccBdn, KErrNone);
	DeleteAllL(DispatcherPhonebook::EIccBdn, KErrNotFound);

	DeleteAllL(DispatcherPhonebook::EIccVmb, KErrNone);
	DeleteAllL(DispatcherPhonebook::EIccVmb, KErrNotFound);

	DeleteAllL(DispatcherPhonebook::EIccMbdn, KErrNone);
	DeleteAllL(DispatcherPhonebook::EIccMbdn, KErrNotFound);

	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for writing entries to all phonebooks
@SYMTestPriority High
@SYMTestActions Open the phonebook and test successful and unsuccessful write operations - RMobilePhoneBookStore::Write()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneL();
	
	CreateEntriesAndStoreDataL();

	CPhoneBookEntry* phoneBookEntry = CPhoneBookEntry::NewLC();
	phoneBookEntry->SetIndex(5);
	phoneBookEntry->SetFirstName(_L("Symbian5"));
	phoneBookEntry->SetDiallingNumber(_L("123456789"));
	phoneBookEntry->AddEmailAddress(_L("a@a.com"));
	phoneBookEntry->AddSecondName(_L("secondname"));
	phoneBookEntry->AddAdditionalNumber(_L("3874387"));
	
	WriteL(DispatcherPhonebook::EIccAdn, *phoneBookEntry, KErrNone);
	WriteL(DispatcherPhonebook::EIccAdn, *phoneBookEntry, KErrOverflow);
	WriteL(DispatcherPhonebook::EIccAdn, *phoneBookEntry, KErrGsmSimServSneFull);
	WriteL(DispatcherPhonebook::EIccAdn, *phoneBookEntry, KErrGsmSimServAnrFull);
	WriteL(DispatcherPhonebook::EIccAdn, *phoneBookEntry, KErrGsmSimServEmailFull);
	
	WriteL(DispatcherPhonebook::EIccFdn, *phoneBookEntry, KErrNone);
	WriteL(DispatcherPhonebook::EIccFdn, *phoneBookEntry, KErrOverflow);
	WriteL(DispatcherPhonebook::EIccFdn, *phoneBookEntry, KErrGsmSimServSneFull);
	WriteL(DispatcherPhonebook::EIccFdn, *phoneBookEntry, KErrGsmSimServAnrFull);
	WriteL(DispatcherPhonebook::EIccFdn, *phoneBookEntry, KErrGsmSimServEmailFull);
	
	//text and number lengths are only set for adn, fdn, vmb, and mbdn phone books
	//thus we can't do the below tests
//	WriteL(DispatcherPhonebook::EIccBdn, *phoneBookEntry, KErrNone);
//	WriteL(DispatcherPhonebook::EIccSdn, *phoneBookEntry, KErrNone);

	WriteL(DispatcherPhonebook::EIccVmb, *phoneBookEntry, KErrNone);
	WriteL(DispatcherPhonebook::EIccVmb, *phoneBookEntry, KErrOverflow);
	WriteL(DispatcherPhonebook::EIccVmb, *phoneBookEntry, KErrGsmSimServSneFull);
	WriteL(DispatcherPhonebook::EIccVmb, *phoneBookEntry, KErrGsmSimServAnrFull);
	WriteL(DispatcherPhonebook::EIccVmb, *phoneBookEntry, KErrGsmSimServEmailFull);

	WriteL(DispatcherPhonebook::EIccMbdn, *phoneBookEntry, KErrNone);
	WriteL(DispatcherPhonebook::EIccMbdn, *phoneBookEntry, KErrOverflow);
	WriteL(DispatcherPhonebook::EIccMbdn, *phoneBookEntry, KErrGsmSimServSneFull);
	WriteL(DispatcherPhonebook::EIccMbdn, *phoneBookEntry, KErrGsmSimServAnrFull);
	WriteL(DispatcherPhonebook::EIccMbdn, *phoneBookEntry, KErrGsmSimServEmailFull);

	CleanupStack::PopAndDestroy(phoneBookEntry);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for sending a Reset Cache IPC
@SYMTestPriority High
@SYMTestActions
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
		
	CreateEntriesAndStoreDataL();
	
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	//the custom API state to RMmCustomAPI::ECacheNotReady
		
	RArray<DispatcherPhonebook::TPhonebook> cachingPhonebooks;
	CleanupClosePushL(cachingPhonebooks);
	cachingPhonebooks.AppendL(DispatcherPhonebook::EIccAdn);
	cachingPhonebooks.AppendL(DispatcherPhonebook::EIccFdn);
		
	// test all caching phonebooks
	for(TInt i = 0; i < cachingPhonebooks.Count(); ++i)
		{
		RMobilePhoneBookStore phoneBookStore;
		CleanupClosePushL(phoneBookStore);
		
		OpenPhoneBookL(cachingPhonebooks[i],phoneBookStore,customApi);
		
	    TMockLtsyData1<DispatcherPhonebook::TPhonebook> resetCacheCompLtsyData(cachingPhonebooks[i]);
	    resetCacheCompLtsyData.SerialiseL(data);
		iMockLTSY.CompleteL(KMockLtsyDispatchPhonebookStoreResetCacheIndId,KErrNone,data);
		data.Close();
		
		WaitForMockLTSYTerminated();
		AssertMockLtsyStatusL();
			
		RMmCustomAPI::TPndCacheStatus cacheStatus;
		customApi.GetPndCacheStatus(requestStatus,cacheStatus,PhonebookName(cachingPhonebooks[i]));
		User::WaitForRequest(requestStatus);
		ASSERT_EQUALS(cacheStatus,RMmCustomAPI::ECacheNotReady);

		// destroying the phonebook store, while one of the caching phonebooks
		// ADN/FDN is in the not-ready state will result in a store cache cancel
		// message being generated by the ctsy

		TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(cachingPhonebooks[i]);
		expLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
		data.Close();
			
		CleanupStack::PopAndDestroy(&phoneBookStore);
		}

	CleanupStack::PopAndDestroy(&cachingPhonebooks);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for setting the FDN settings
@SYMTestPriority High
@SYMTestActions
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	CreateEntriesAndStoreDataL();
	
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RMobilePhoneBookStore phoneBookStore;
	CleanupClosePushL(phoneBookStore);
		
	OpenPhoneBookL(DispatcherPhonebook::EIccFdn,phoneBookStore,customApi);

	SetFdnInfoL(phoneBookStore,KErrNone);

	SetFdnInfoL(phoneBookStore,KErrGeneral);

	// destroying the phonebook store, while one of the caching phonebooks
	// ADN/FDN is in the not-ready state will result in a store cache cancel
	// message being generated by the ctsy

	RBuf8 data;
	CleanupClosePushL(data);
	
	DispatcherPhonebook::TPhonebook pB = DispatcherPhonebook::EIccFdn;
	TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(pB);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
	data.Close();
	CleanupStack::PopAndDestroy(&data);
	
	CleanupStack::PopAndDestroy(&phoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this); 
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for getting phonebook store information
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	CreateEntriesAndStoreDataL();

	OpenPhoneL();

	GetInfoL(DispatcherPhonebook::EIccAdn,KErrNone);
	GetInfoL(DispatcherPhonebook::EIccAdn,KErrGeneral);

	GetInfoL(DispatcherPhonebook::EIccFdn,KErrNone);
	GetInfoL(DispatcherPhonebook::EIccFdn,KErrGeneral);

	GetInfoL(DispatcherPhonebook::EIccSdn,KErrNone);
	GetInfoL(DispatcherPhonebook::EIccSdn,KErrGeneral);

	//causes an ETel Panic - 5 (apparent known defect - Id 40502)
	//GetInfoL(DispatcherPhonebook::EIccBdn,KErrNone);
	//GetInfoL(DispatcherPhonebook::EIccBdn,KErrGeneral);

	GetInfoL(DispatcherPhonebook::EIccVmb,KErrNone);
	GetInfoL(DispatcherPhonebook::EIccVmb,KErrGeneral);

	GetInfoL(DispatcherPhonebook::EIccMbdn,KErrNone);
	GetInfoL(DispatcherPhonebook::EIccMbdn,KErrGeneral);
	
	CleanupStack::PopAndDestroy(this); 
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for getting store information
@SYMTestPriority High
@SYMTestActions RMobilePhone::GetPhoneStoreInfo()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	
	CreateEntriesAndStoreDataL();
	OpenPhoneL();

	TName storeName(KETelIccAdnPhoneBook);
	RMobilePhoneBookStore::TMobilePhoneStoreInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneStoreInfoV1Pckg bookInfoPckg(bookInfo);

	/********************Test Normal case******************************/

	
	TDesC* storeNamePtr = &storeName;
    TMockLtsyData1<TDesC*> getInfoExpLtsyData(storeNamePtr);
    getInfoExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId,data);
	data.Close();
	
	iPhone.GetPhoneStoreInfo(requestStatus,bookInfoPckg,storeName);

	RMobilePhoneBookStore::TMobilePhoneStoreInfoV1 compBookInfo;
	compBookInfo.iType 			= RMobilePhoneStore::EPhoneBookStore;
	compBookInfo.iTotalEntries 	= iPhonebookStoreData.iAdnTotalEntries;
	compBookInfo.iUsedEntries 	= iEntries.Count();
	compBookInfo.iCaps 			= 0;
	compBookInfo.iName 			= KETelIccAdnPhoneBook;
	
    TMockLtsyData1<RMobilePhoneBookStore::TMobilePhoneStoreInfoV1> getInfoCompLtsyData(compBookInfo);
    getInfoCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId,KErrNone,data);
	data.Close();
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KErrNone)

	
	ASSERT_EQUALS(compBookInfo.iType, 			bookInfo.iType);
	ASSERT_EQUALS(compBookInfo.iTotalEntries, 	bookInfo.iTotalEntries);
	ASSERT_EQUALS(compBookInfo.iUsedEntries, 	bookInfo.iUsedEntries);
	ASSERT_EQUALS(compBookInfo.iCaps,		 	bookInfo.iCaps);
	ASSERT_EQUALS(compBookInfo.iName, 			bookInfo.iName);
	
	AssertMockLtsyStatusL();

	/********************Test Error case******************************/
	
    getInfoExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId,data);
	data.Close();
	
	iPhone.GetPhoneStoreInfo(requestStatus,bookInfoPckg,storeName);

	const TInt KError = KErrGeneral;
	
    getInfoCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId,KError,data);
	data.Close();
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KError)

	AssertMockLtsyStatusL();
	
	
	CleanupStack::PopAndDestroy(&data); 
	CleanupStack::PopAndDestroy(this); 
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for a SIM refresh of the Cached phonebooks
@SYMTestPriority High
@SYMTestActions Invokes SIM refresh IPC, 
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	CreateEntriesAndStoreDataL();
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	

	//open the caching phonebooks
	RMobilePhoneBookStore adnPhoneBookStore;
	CleanupClosePushL(adnPhoneBookStore);
	OpenPhoneBookL(DispatcherPhonebook::EIccAdn,adnPhoneBookStore,customApi);

	RMobilePhoneBookStore fdnPhoneBookStore;
	CleanupClosePushL(fdnPhoneBookStore);
	OpenPhoneBookL(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,customApi);

	
	//test the refresh
	
	//add a new entry
	CPhoneBookEntry* phoneBookEntry = CPhoneBookEntry::NewLC();
	iEntries.AppendL(phoneBookEntry);
	CleanupStack::Pop(phoneBookEntry); //takes ownership
	phoneBookEntry->SetIndex(5);
	phoneBookEntry->SetFirstName(_L("Symbian4"));
	phoneBookEntry->SetDiallingNumber(_L("897654321"));
	phoneBookEntry->AddEmailAddress(_L("symbian1@symbian.com11"));
	phoneBookEntry->AddSecondName(_L("Phone book311"));
	phoneBookEntry->AddAdditionalNumber(_L("12311"));

	iPhonebookStoreData.iAdnTotalEntries = iEntries.Count(); 
	iPhonebookStoreData.iFdnTotalEntries = iEntries.Count();
	
	RefreshL(DispatcherPhonebook::EIccAdn);
	//test the refresh changed the CTSY state
	GetInfoFromCtsy(DispatcherPhonebook::EIccAdn,adnPhoneBookStore,iEntries.Count());
	ReadL(DispatcherPhonebook::EIccAdn,adnPhoneBookStore,KErrNone);
	
	RefreshL(DispatcherPhonebook::EIccFdn);	
	//test the refresh changed the CTSY state
	GetInfoFromCtsy(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,iEntries.Count());
	ReadL(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,KErrNone);
	
	CleanupStack::PopAndDestroy(&fdnPhoneBookStore); 
	CleanupStack::PopAndDestroy(&adnPhoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Write(), RMobileSmsStore::Read(), RMobileSmsStore::GetInfo() and RMobileSmsStore::Delete() 
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Write(), RMobileSmsStore::Read(), RMobileSmsStore::GetInfo() and RMobileSmsStore::Delete()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0013L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	// open the message store
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	//-------------------------------------------------------------------------
	// RMobileSmsStore::Write()
 	//-------------------------------------------------------------------------

	TInt index = 0;

	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
		
	entryV1.iServiceCentre.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	entryV1.iServiceCentre.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	entryV1.iServiceCentre.iTelNumber    = _L("TelNumber1");
	entryV1.iMsgData = _L8("Hello, World!");
	entryV1.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
	entryV1.iIndex = index;
	
	TMockLtsyData1<RMobileSmsStore::TMobileGsmSmsEntryV1> 
			expMockData1(entryV1);

	TInt location = 0;
	TBool  receivedClass2ToBeResent = ETrue;

	TMockLtsyData2<TInt, TBool> completeMockData2(location, receivedClass2ToBeResent);

	expData.Close();
	expMockData1.SerialiseL(expData);
	completeData.Close();
	completeMockData2.SerialiseL(completeData);


	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, KErrNone, completeData);
	iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

	smsStore.Write(requestStatus, pckgEntryV1);

    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(entryV1.iIndex, location);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// RMobileSmsStore::Read()
 	//-------------------------------------------------------------------------
		
	
	TMockLtsyData1<TInt> mockIndex(index);
	
	DispatcherPhonebook::TSmsData smsData;
	
	smsData.iSmsMsg= _L8("Hello, World!"); 
	smsData.iServiceCentre = _L("ServiceCenter1");
	smsData.iMobileScTON = RMobilePhone::EAlphanumericNumber; 
	smsData.iMobileScNPI = RMobilePhone::ENationalNumberPlan;
	smsData.iMessageStatus = RMobileSmsStore::EStoredMessageUnread;
		
	TMockLtsyData1<DispatcherPhonebook::TSmsData> mockMsg(smsData);
	
	completeData.Close();
	mockMsg.SerialiseL(completeData);
	
	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1Read;
	entryV1Read.iIndex=index;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1Read(entryV1Read);
	
	TMockLtsyData1<TInt> mockIndexRead(index);
	expData.Close();
	mockIndexRead.SerialiseL(expData);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, KErrNone, completeData);
	
	smsStore.Read(requestStatus, pckgEntryV1Read);
	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(0, smsData.iSmsMsg.Compare(entryV1Read.iMsgData));	
	ASSERT_EQUALS(0, smsData.iServiceCentre.Compare(entryV1Read.iServiceCentre.iTelNumber));	
	ASSERT_EQUALS(smsData.iMobileScTON, entryV1Read.iServiceCentre.iTypeOfNumber);	
	ASSERT_EQUALS(smsData.iMobileScNPI, entryV1Read.iServiceCentre.iNumberPlan);	
	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageRead, entryV1Read.iMsgStatus);	
	
 	//-------------------------------------------------------------------------
	// RMobileSmsStore::GetInfo()
 	//-------------------------------------------------------------------------
	
	TInt totalEntries = 1;
	TInt usedEntries = 0;
	TMockLtsyData2<TInt,TInt> smsStoreGetInfoLtsyData(totalEntries, usedEntries);
	completeData.Close();
	smsStoreGetInfoLtsyData.SerialiseL(completeData);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, KErrNone, completeData);

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV1;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV1(phoneBookInfoV1);
	smsStore.GetInfo(requestStatus, pckgInfoV1);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(requestStatus.Int(),KErrNone);
	ASSERT_EQUALS(phoneBookInfoV1.iTotalEntries, totalEntries);
	ASSERT_EQUALS(phoneBookInfoV1.iUsedEntries, usedEntries);
	
 	//-------------------------------------------------------------------------
	// RMobileSmsStore::Delete()
 	//-------------------------------------------------------------------------
	
	TMockLtsyData1<TInt> expMockData1Delete(index);
	expMockData1Delete.SerialiseL(expData);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, KErrNone);
	
	smsStore.Delete(requestStatus, index);

	User::WaitForRequest(requestStatus);        
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(5, this); // expData, completeData, 
	                                      // smsStore, messaging, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UC0014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Write(), RMobileSmsStore::Write(), RMobileSmsStore::GetInfo(), CRetrieveMobilePhoneSmsList::Start() and RMobileSmsStore::DealeteAll()  
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Write(), RMobileSmsStore::Write(), RMobileSmsStore::GetInfo(), CRetrieveMobilePhoneSmsList::Start() and RMobileSmsStore::DealeteAll() 
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookFU::TestUseCase0014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	// open the message store
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	//-------------------------------------------------------------------------
	// RMobileSmsStore::Write()
 	//-------------------------------------------------------------------------

	TInt index1 = 0;
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry1V1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckg1EntryV1(entry1V1);
		
	entry1V1.iServiceCentre.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	entry1V1.iServiceCentre.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	entry1V1.iServiceCentre.iTelNumber    = _L("TelNumber1");
	entry1V1.iMsgData = _L8("Hello, World!");
	entry1V1.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
	entry1V1.iIndex = index1;
	
	TMockLtsyData1<RMobileSmsStore::TMobileGsmSmsEntryV1> 
			expMockData1Write1(entry1V1);

	TInt location1 = 0;
	TBool  receivedClass2ToBeResent1 = EFalse;

	TMockLtsyData2<TInt, TBool> completeMockData2Write1(location1, receivedClass2ToBeResent1);

	expData.Close();
	expMockData1Write1.SerialiseL(expData);
	completeData.Close();
	completeMockData2Write1.SerialiseL(completeData);

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, KErrNone, completeData);
	
	smsStore.Write(requestStatus, pckg1EntryV1);

    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(entry1V1.iIndex, location1);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// RMobileSmsStore::Write()
 	//-------------------------------------------------------------------------
	
	TInt index2 = 1;
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry2V1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckg2EntryV1(entry2V1);
		
	entry2V1.iServiceCentre.iNumberPlan   = RMobilePhone::EServiceCentreSpecificPlan2;
	entry2V1.iServiceCentre.iTypeOfNumber = RMobilePhone::ESubscriberNumber;
	entry2V1.iServiceCentre.iTelNumber    = _L("TelNumber2");
	entry2V1.iMsgData = _L8("Hello, World number 2!");
	entry2V1.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
	entry2V1.iIndex = index2;
	
	TMockLtsyData1<RMobileSmsStore::TMobileGsmSmsEntryV1> 
			expMockData1Write2(entry2V1);

	TInt location2 = 1;
	TBool  receivedClass2ToBeResent2 = ETrue;

	TMockLtsyData2<TInt, TBool> completeMockData2Write2(location2, receivedClass2ToBeResent2);

	expData.Close();
	expMockData1Write2.SerialiseL(expData);
	completeData.Close();
	completeMockData2Write2.SerialiseL(completeData);


	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, KErrNone, completeData);
	iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId); //because receivedClass2ToBeResent1 = ETrue;
	
	smsStore.Write(requestStatus, pckg2EntryV1);

    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(entry2V1.iIndex, location2);
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// RMobileSmsStore::GetInfo()
 	//-------------------------------------------------------------------------
	
	TInt totalEntries = 2;
	TInt usedEntries = 0;
	TMockLtsyData2<TInt,TInt> smsStoreGetInfoLtsyData(totalEntries, usedEntries);
	completeData.Close();
	smsStoreGetInfoLtsyData.SerialiseL(completeData);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, KErrNone, completeData);

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV1;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV1(phoneBookInfoV1);
	smsStore.GetInfo(requestStatus, pckgInfoV1);
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(requestStatus.Int(),KErrNone);
	ASSERT_EQUALS(phoneBookInfoV1.iTotalEntries, totalEntries);
	ASSERT_EQUALS(phoneBookInfoV1.iUsedEntries, usedEntries);

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// CRetrieveMobilePhoneSmsList::Start
 	//-------------------------------------------------------------------------
	
	CGetSMSListAO* getSmsListAO = CGetSMSListAO::NewLC(smsStore);

	TBool receivedClass2ToBeResent = ETrue;
	
	DispatcherPhonebook::TSmsData smsData1;
		
	smsData1.iSmsMsg= _L8("Hello, World!"); 
	smsData1.iServiceCentre = _L("TelNumber1");
	smsData1.iMobileScTON = RMobilePhone::EAlphanumericNumber;
	smsData1.iMobileScNPI = RMobilePhone::ENationalNumberPlan;
	smsData1.iMessageStatus = RMobileSmsStore::EStoredMessageUnread;
		
	TBool moreToCome1 = ETrue; 
	    
	TMockLtsyData4<DispatcherPhonebook::TSmsData, TInt, TBool, TBool> completeSmsMsgLtsyData1(smsData1, index1, moreToCome1, receivedClass2ToBeResent);
	
	DispatcherPhonebook::TSmsData smsData2;
		
	smsData2.iSmsMsg= _L8("Hello, World number 2!"); 
	smsData2.iServiceCentre = _L("TelNumber2");
	smsData2.iMobileScTON = RMobilePhone::ESubscriberNumber; 
	smsData2.iMobileScNPI = RMobilePhone::EServiceCentreSpecificPlan2;
	smsData2.iMessageStatus = RMobileSmsStore::EStoredMessageUnread;
		
	TBool moreToCome2 = EFalse; 
	    
	TMockLtsyData4<DispatcherPhonebook::TSmsData, TInt, TBool, TBool> completeSmsMsgLtsyData2(smsData2, index2, moreToCome2, receivedClass2ToBeResent);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNone);
	//complete with 1st sms data
	completeData.Close();
	completeSmsMsgLtsyData1.SerialiseL(completeData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNone, completeData);
	//complete with 2nd sms data
	completeData.Close();
	completeSmsMsgLtsyData2.SerialiseL(completeData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNone, completeData);
	iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

	getSmsListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNone, getSmsListAO->RetrieveLastError());
	//assert:
	RMobileSmsStore::TMobileGsmSmsEntryV1 retrievedSmsMsg;
	CMobilePhoneGsmSmsList* gsmSmsList;
	gsmSmsList=getSmsListAO->RetrieveListL();
	retrievedSmsMsg=gsmSmsList->GetEntryL(0);
	
	ASSERT_EQUALS(0, smsData1.iSmsMsg.Compare(retrievedSmsMsg.iMsgData));
	ASSERT_EQUALS(0, smsData1.iServiceCentre.Compare(retrievedSmsMsg.iServiceCentre.iTelNumber));	
	ASSERT_EQUALS(retrievedSmsMsg.iServiceCentre.iTypeOfNumber, smsData1.iMobileScTON);
	ASSERT_EQUALS(retrievedSmsMsg.iServiceCentre.iNumberPlan, smsData1.iMobileScNPI);
	ASSERT_EQUALS(retrievedSmsMsg.iMsgStatus, RMobileSmsStore::EStoredMessageRead);
	ASSERT_EQUALS(retrievedSmsMsg.iIndex, index1);
	
	retrievedSmsMsg=gsmSmsList->GetEntryL(1);
	
	ASSERT_EQUALS(0, smsData2.iSmsMsg.Compare(retrievedSmsMsg.iMsgData));
	ASSERT_EQUALS(0, smsData2.iServiceCentre.Compare(retrievedSmsMsg.iServiceCentre.iTelNumber));	
	ASSERT_EQUALS(retrievedSmsMsg.iServiceCentre.iTypeOfNumber, smsData2.iMobileScTON);
	ASSERT_EQUALS(retrievedSmsMsg.iServiceCentre.iNumberPlan, smsData2.iMobileScNPI);
	ASSERT_EQUALS(retrievedSmsMsg.iMsgStatus, RMobileSmsStore::EStoredMessageRead);
	ASSERT_EQUALS(retrievedSmsMsg.iIndex, index2);

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// RMobileSmsStore::DeleteAll()
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId, KErrNone);
	
	smsStore.DeleteAll(requestStatus);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	CleanupStack::PopAndDestroy(6, this); // getSmsListAO, completeData, expData, 
	                                      // smsStore, messaging,  this 
	}


//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for a cache cancel
@SYMTestPriority High
@SYMTestActions Open the caching phonebooks and close before the cache has completed
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhonebookFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneL();
	
	CreateEntriesAndStoreDataL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	//repeat for all caching PBs
	for(TInt i = 0; i < 2; ++i)
		{
		DispatcherPhonebook::TPhonebook phonebook 
		= (	i == 0 ? DispatcherPhonebook::EIccAdn : DispatcherPhonebook::EIccFdn);

		RMobilePhoneBookStore phoneBookStore;
		CleanupClosePushL(phoneBookStore);

		//just the open, don't complete the cache
		OpenCachingPhoneBookL(phonebook,phoneBookStore,KErrNone);

		// destroying the phonebook store, while one of the caching phonebooks
		// ADN/FDN is in the not-ready state will result in a store cache cancel
		// message being generated by the ctsy
		
		TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(phonebook);
		expLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
		data.Close();
		
		//close them without the cache yet completed
		CleanupStack::PopAndDestroy(&phoneBookStore);

		AssertMockLtsyStatusL();
		}

	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this); 
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Opening a caching phonebook with initialisation failure
@SYMTestPriority High
@SYMTestActions 
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhonebookFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneL();
	
	CreateEntriesAndStoreDataL();
	
	TRequestStatus requestStatus;
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RMobilePhoneBookStore fdnPhoneBookStore;
	CleanupClosePushL(fdnPhoneBookStore);
	
	OpenCachingPhoneBookL(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,KErrGeneral);
	
	//can't read
	TBuf8<KReadBufLength> phonebookReadData;
	fdnPhoneBookStore.Read(requestStatus,1,1,phonebookReadData);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KErrInUse)

	//can't write
	RBuf8 writeData;
	CleanupClosePushL(writeData);
	CreateTlvFromEntryL(*iEntries[1],writeData);
	TInt index = 1;
	fdnPhoneBookStore.Write(requestStatus,writeData,index);
	User::WaitForRequest(requestStatus);
	ASSERT_TRUE(requestStatus.Int() != KErrNone);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(&writeData);
	CleanupStack::PopAndDestroy(&fdnPhoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this); 
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Opening a non caching phonebook with initialisation failure
@SYMTestPriority High
@SYMTestActions 
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhonebookFU::TestUnit0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TRequestStatus requestStatus;
	
	OpenPhoneL();
	
	CreateEntriesAndStoreDataL();

	RMobilePhoneBookStore vmbPhoneBookStore;
	CleanupClosePushL(vmbPhoneBookStore);

	OpenNonCachingPhoneBookL(DispatcherPhonebook::EIccVmb,vmbPhoneBookStore,KErrGeneral);

	//cannot read, increased error checking in C-TSY result in this read being stopped. 
	RBuf8 readBuf;
	CleanupClosePushL(readBuf);
	readBuf.CreateL(KReadBufLength * 1);	

	vmbPhoneBookStore.Read(requestStatus,1,1,readBuf);
	ASSERT_EQUALS(requestStatus.Int(),KErrNotFound);	
	
	//can't write
	RBuf8 writeData;
	CleanupClosePushL(writeData);
	CreateTlvFromEntryL(*iEntries[1],writeData);
	TInt index = 1;
	vmbPhoneBookStore.Write(requestStatus,writeData,index);
	User::WaitForRequest(requestStatus);
	ASSERT_TRUE(requestStatus.Int() != KErrNone);
	
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&writeData);
	CleanupStack::PopAndDestroy(&readBuf);	
	CleanupStack::PopAndDestroy(&vmbPhoneBookStore);
	CleanupStack::PopAndDestroy(this); 
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Opening a caching phonebook with cache failure
@SYMTestPriority High
@SYMTestActions
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhonebookFU::TestUnit0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneL();
	
	CreateEntriesAndStoreDataL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RMobilePhoneBookStore fdnPhoneBookStore;
	CleanupClosePushL(fdnPhoneBookStore);

	TRequestStatus requestStatus;
	OpenCachingPhoneBookL(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,KErrNone);
	
	//cache completed with a failure
	CompleteCacheL(DispatcherPhonebook::EIccFdn, KErrGeneral, EFalse);
	WaitForMockLTSYTerminated();
	
	//can't read
	TBuf8<KReadBufLength> readBuf;
	fdnPhoneBookStore.Read(requestStatus,1,1,readBuf);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KErrInUse);
	
	CompleteCacheL(DispatcherPhonebook::EIccFdn, KErrNone, EFalse);
	WaitForMockLTSYTerminated();
	
	//can read
	ReadL(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,KErrNone);

	// destroying the phonebook store, while one of the caching phonebooks
	// ADN/FDN is in the not-ready state will result in a store cache cancel
	// message being generated by the ctsy

	RBuf8 data;
	CleanupClosePushL(data);
	
	DispatcherPhonebook::TPhonebook pB = DispatcherPhonebook::EIccFdn;
	TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(pB);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
	data.Close();
	
	CleanupStack::PopAndDestroy(&data);

	
	CleanupStack::PopAndDestroy(&fdnPhoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this); 
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Gets the stores info using the Custom API
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::Get3GPBInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhonebookFU::TestUnit0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneL();
	
	CreateEntriesAndStoreDataL();
	
	TRequestStatus requestStatus;

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RMobilePhoneBookStore fdnPhoneBookStore;
	CleanupClosePushL(fdnPhoneBookStore);
	
	OpenPhoneBookL(DispatcherPhonebook::EIccFdn,fdnPhoneBookStore,customApi);
	
	RMmCustomAPI::T3GPBInfo info3GPB;
	customApi.Get3GPBInfo(requestStatus,info3GPB);
	
	User::WaitForRequest(requestStatus);
	
	ASSERT_TRUE(info3GPB.iMaxLenEmail 	== iPhonebookStoreData.iEmailMaximumLength);
	ASSERT_TRUE(info3GPB.iMaxLenSne		== iPhonebookStoreData.iSecondNameMaximumLength);
	ASSERT_TRUE(info3GPB.iMaxLenAnr 	== iPhonebookStoreData.iAdditionalNumberMaximumLength);

	ASSERT_TRUE(info3GPB.iMaxNumEmail 	== iPhonebookStoreData.iEmailMaximumFieldsPerEntry);
	ASSERT_TRUE(info3GPB.iMaxNumSne		== iPhonebookStoreData.iSecondNameMaximumFieldsPerEntry);
	ASSERT_TRUE(info3GPB.iMaxNumAnr 	== iPhonebookStoreData.iAdditionalNumberMaximumFieldsPerEntry);

	// destroying the phonebook store, while one of the caching phonebooks
	// ADN/FDN is in the not-ready state will result in a store cache cancel
	// message being generated by the ctsy

	RBuf8 data;
	CleanupClosePushL(data);
	
	DispatcherPhonebook::TPhonebook pB = DispatcherPhonebook::EIccFdn;
	TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(pB);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
	data.Close();
	
	CleanupStack::PopAndDestroy(&data);
	
	CleanupStack::PopAndDestroy(&fdnPhoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of dispatcher when sending a junk TLV from the LTSY on a read request
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::Read()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhonebookFU::TestUnit0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	CreateEntriesAndStoreDataL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RMobilePhoneBookStore fdnPhoneBookStore;
	CleanupClosePushL(fdnPhoneBookStore);

	DispatcherPhonebook::TPhonebook phonebook = DispatcherPhonebook::EIccFdn;

	OpenPhoneBookL(phonebook,fdnPhoneBookStore,customApi);

	TRequestStatus requestStatus;
	TBuf8<KReadBufLength> readBuf;
	
	ExpectNonCacheReadL(phonebook,fdnPhoneBookStore,1,1,requestStatus,readBuf);
	
	TBuf8<KReadBufLength> tlvEntry = _L8("Junk Data");
	
	TDesC8* tlvEntryPtr = &tlvEntry;
	TMockLtsyData2<DispatcherPhonebook::TPhonebook,TDesC8*> readEntryLtsyData(phonebook,tlvEntryPtr);
	readEntryLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId,KErrNone,data);
	data.Close();
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KErrArgument); //KErrArgument generated from CPhoneBookEntry::InternalizeFromTlvEntry()

	// destroying the phonebook store, while one of the caching phonebooks
	// ADN/FDN is in the not-ready state will result in a store cache cancel
	// message being generated by the ctsy

	TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(phonebook);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
	data.Close();
	
	CleanupStack::PopAndDestroy(&fdnPhoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::GetInfo()
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::GetInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFU::TestUnit0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);
	
	TInt totalEntries = 100;
	TInt usedEntries = 10;
	TMockLtsyData2<TInt,TInt> smsStoreGetInfoLtsyData(totalEntries, usedEntries);
	data.Close();
	smsStoreGetInfoLtsyData.SerialiseL(data);
			
	TRequestStatus getInfoStatus;
    TRequestStatus mockLtsyStatus;
    
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV1;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV1(phoneBookInfoV1);
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, KErrNotSupported);

	smsStore.GetInfo(getInfoStatus, pckgInfoV1);
	    
	User::WaitForRequest(getInfoStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, getInfoStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, KErrGeneral, data);

	smsStore.GetInfo(getInfoStatus, pckgInfoV1);
	    
	User::WaitForRequest(getInfoStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, getInfoStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::GetInfo() when result is not cached.
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, KErrNone, data);

	smsStore.GetInfo(getInfoStatus, pckgInfoV1);
	User::WaitForRequest(getInfoStatus);
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(getInfoStatus.Int(),KErrNone);
	ASSERT_EQUALS(phoneBookInfoV1.iTotalEntries, totalEntries);
	ASSERT_EQUALS(phoneBookInfoV1.iUsedEntries, usedEntries);


 	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileSmsStore::GetInfo()
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CleanupStack::PopAndDestroy(4, this); // smsStore, messaging, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Read()
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Read()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFU::TestUnit0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------
	TInt index = 5;
	
	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	entryV1.iIndex=index;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
	
	TMockLtsyData1<TInt> mockIndex(index);
	
	DispatcherPhonebook::TSmsData smsData;
	
	smsData.iSmsMsg= _L8("Hello, World!"); 
	smsData.iServiceCentre = _L("ServiceCenter1");
	smsData.iMobileScTON = RMobilePhone::ENetworkSpecificNumber;
	smsData.iMobileScNPI = RMobilePhone::EServiceCentreSpecificPlan2;
	smsData.iMessageStatus = KSimSmsMtRead;
		
	TMockLtsyData1<DispatcherPhonebook::TSmsData> mockMsg(smsData);
	
	expData.Close();
	mockIndex.SerialiseL(expData);
	completeData.Close();
	mockMsg.SerialiseL(completeData);

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, expData, KErrNotSupported);

	smsStore.Read(requestStatus, pckgEntryV1);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, KErrGeneral, completeData);

	smsStore.Read(requestStatus, pckgEntryV1);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::Read() when result is not cached.
 	//-------------------------------------------------------------------------
		
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, KErrNone, completeData);

	smsStore.Read(requestStatus, pckgEntryV1);
	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(0, smsData.iSmsMsg.Compare(entryV1.iMsgData));	
	ASSERT_EQUALS(0, smsData.iServiceCentre.Compare(entryV1.iServiceCentre.iTelNumber));	
	ASSERT_EQUALS(smsData.iMobileScTON, entryV1.iServiceCentre.iTypeOfNumber);	
	ASSERT_EQUALS(smsData.iMobileScNPI, entryV1.iServiceCentre.iNumberPlan);	
	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageRead, entryV1.iMsgStatus);	

 	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileSmsStore::Read()
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CleanupStack::PopAndDestroy(5, this); // completeData, expData, smsStore, messaging, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Write()
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Write()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFU::TestUnit0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	// open the message store
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------

	TInt index = 5;

	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
	
	entryV1.iServiceCentre.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	entryV1.iServiceCentre.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	entryV1.iServiceCentre.iTelNumber    = _L("TelNumber1");
	entryV1.iMsgData = _L8("Hello, World!");
	entryV1.iMsgStatus = RMobileSmsStore::EStoredMessageUnsent;
	entryV1.iIndex = index;
	
	TMockLtsyData1<RMobileSmsStore::TMobileGsmSmsEntryV1> 
		expMockData1(entryV1);

    TInt location = 100;
	TBool  receivedClass2ToBeResent = ETrue;

	TMockLtsyData2<TInt, TBool> completeMockData2(location, receivedClass2ToBeResent);

	expData.Close();
	expMockData1.SerialiseL(expData);
	completeData.Close();
	completeMockData2.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, expData, KErrNotSupported);

	smsStore.Write(requestStatus, pckgEntryV1);

    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, KErrGeneral, completeData);
	iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

	smsStore.Write(requestStatus, pckgEntryV1);

    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::Write() when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, KErrNone, completeData);
	iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

	smsStore.Write(requestStatus, pckgEntryV1);

    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(entryV1.iIndex, location);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileSmsStore::Write()
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(5, this); // expData, completeData, this
	                                      // smsStore, messaging	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Delete()
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Delete()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFU::TestUnit0010L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	// open the message store
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);

	RBuf8 expData;
	CleanupClosePushL(expData);

	//-------------------------------------------------------------------------

	TInt index = 1;

	TMockLtsyData1<TInt> expMockData1(index);
	expMockData1.SerialiseL(expData);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, expData, KErrNotSupported);

	smsStore.Delete(requestStatus, index);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, KErrGeneral);

	smsStore.Delete(requestStatus, index);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::Delete() when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, expData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, KErrNone);
	
	smsStore.Delete(requestStatus, index);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
 	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileSmsStore::Delete()
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CleanupStack::PopAndDestroy(4, this); // expData, smsStore, messaging, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::DeleteAll()
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::DeleteAll()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFU::TestUnit0011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	// open the message store
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId, KErrNotSupported);

	smsStore.DeleteAll(requestStatus);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId, KErrGeneral);

	smsStore.DeleteAll(requestStatus);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::DeleteAll() when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId, KErrNone);
	
	smsStore.DeleteAll(requestStatus);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
 	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileSmsStore::DeleteAll()
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CleanupStack::PopAndDestroy(3, this); // smsStore, messaging, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOK-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneSmsList::Start()
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneSmsList::Start()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookFU::TestUnit0012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open the message store
	RMobileSmsMessaging messaging;
	CleanupClosePushL(messaging);

	RMobileSmsStore smsStore;
	CleanupClosePushL(smsStore);

	OpenSmsStoreL(messaging, smsStore, KETelIccSmsStore);

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	CGetSMSListAO* getSmsListAO = CGetSMSListAO::NewLC(smsStore);
	
	//-------------------------------------------------------------------------
	DispatcherPhonebook::TSmsData smsData;
	TInt index = 0;
	
	smsData.iSmsMsg= _L8("Hello, World!"); 
	smsData.iServiceCentre = _L("ServiceCenter");
	smsData.iMobileScTON = RMobilePhone::ENetworkSpecificNumber;
	smsData.iMobileScNPI = RMobilePhone::EServiceCentreSpecificPlan2;
	smsData.iMessageStatus = KSimSmsMtRead;
	
    TBool moreToCome = ETrue; 
    TBool receivedClass2ToBeResent = ETrue;
    
	TMockLtsyData4<DispatcherPhonebook::TSmsData, TInt, TBool, TBool> completeSmsMsgLtsyData(smsData, index, moreToCome, receivedClass2ToBeResent);

	TInt totalEntries(1);
	TInt usedEntries(0);

	TMockLtsyData2<TInt, TInt> dataGetInfo(totalEntries, usedEntries);
	completeData.Close();
	dataGetInfo.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNotSupported);

	getSmsListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNotSupported, getSmsListAO->RetrieveLastError());

	AssertMockLtsyStatusL();
		
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
	//-------------------------------------------------------------------------
	//failure when there are no more entries to be returned 
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNone);
	completeData.Close();
	completeSmsMsgLtsyData.SerialiseL(completeData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrGeneral, completeData);
	iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

	getSmsListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrGeneral, getSmsListAO->RetrieveLastError());
	AssertMockLtsyStatusL();
	
	//failure when there are more entries to be returned 
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrGeneral, completeData);
	moreToCome = EFalse;
	TMockLtsyData4<DispatcherPhonebook::TSmsData, TInt, TBool, TBool> completeSmsMsgLtsyData2(smsData, index, moreToCome, receivedClass2ToBeResent);
	completeData.Close();
	completeSmsMsgLtsyData2.SerialiseL(completeData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNone, completeData);
	iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

	getSmsListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrGeneral, getSmsListAO->RetrieveLastError());
	AssertMockLtsyStatusL();
		
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneSmsList::Start when result is not cached.
	//-------------------------------------------------------------------------
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNone, completeData);
	iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

	getSmsListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNone, getSmsListAO->RetrieveLastError());

	//assert:
	RMobileSmsStore::TMobileGsmSmsEntryV1 retrievedSmsMsg;
	CMobilePhoneGsmSmsList* gsmSmsList;
	gsmSmsList=getSmsListAO->RetrieveListL();
	retrievedSmsMsg=gsmSmsList->GetEntryL(0);
	
	ASSERT_EQUALS(0, smsData.iSmsMsg.Compare(retrievedSmsMsg.iMsgData));
	ASSERT_EQUALS(0, smsData.iServiceCentre.Compare(retrievedSmsMsg.iServiceCentre.iTelNumber));	
	ASSERT_EQUALS(retrievedSmsMsg.iServiceCentre.iTypeOfNumber, smsData.iMobileScTON);
	ASSERT_EQUALS(retrievedSmsMsg.iServiceCentre.iNumberPlan, smsData.iMobileScNPI);
	ASSERT_EQUALS(retrievedSmsMsg.iMsgStatus, RMobileSmsStore::EStoredMessageRead);
	ASSERT_EQUALS(retrievedSmsMsg.iIndex, index);

	AssertMockLtsyStatusL();


	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of CRetrieveMobilePhoneSmsList::Start
	// from LTSY.
	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	completeSmsMsgLtsyData.SerialiseL(completeData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId, KErrNone, completeData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(5, this); // gsmSmsList, completeData, store, messaging, this	
	}

/***********************************************Helper functions***************************************/

/**
 * Helper funtion to open a phonebook
 * 
 * @param aPhonebook The phonebook being tested
 * @param aPhoneBookStore The opened store
 * @param aCustomApi The open custome API
 */
void CCTsyPhonebookFU::OpenPhoneBookL(	DispatcherPhonebook::TPhonebook aPhonebook, 
										RMobilePhoneBookStore& aPhoneBookStore, 
										RMmCustomAPI& aCustomApi)
	{
	TRequestStatus requestStatus;

	if(CachingPhonebook(aPhonebook))
		{
		//so we are notified when the cache is ready
		TName phonebookName(PhonebookName(aPhonebook));
		aCustomApi.NotifyPndCacheReady(requestStatus,phonebookName);
		
		OpenCachingPhoneBookL(aPhonebook,aPhoneBookStore,KErrNone);
		CompleteCacheL(aPhonebook, KErrNone, EFalse);
		
		//wait for the cache to become ready, then the phonebook is fully open
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();
		
		RMmCustomAPI::TPndCacheStatus cacheStatus;
		aCustomApi.GetPndCacheStatus(requestStatus,cacheStatus,PhonebookName(aPhonebook));
		User::WaitForRequest(requestStatus);
		ASSERT_EQUALS(cacheStatus,RMmCustomAPI::ECacheReady);
		
		}
	else
		{
		OpenNonCachingPhoneBookL(aPhonebook,aPhoneBookStore,KErrNone);

		RMmCustomAPI::TPndCacheStatus cacheStatus;
		aCustomApi.GetPndCacheStatus(requestStatus,cacheStatus,PhonebookName(aPhonebook));
		User::WaitForRequest(requestStatus);
		
		if(aPhonebook != DispatcherPhonebook::EIccMbdn)
			{
			ASSERT_EQUALS(cacheStatus,RMmCustomAPI::ECacheNotUsed);
			}
		}
	}

/**
 * Helper funtion to open a phonebook, this expects and completes the initialisation phase
 * 
 * @param aPhonebook The phonebook being tested
 * @param aPhoneBookStore The opened store
 * @param aError the Error to return from the LTSY
 */
void CCTsyPhonebookFU::OpenPhoneBookWithInitialisationL(DispatcherPhonebook::TPhonebook aPhonebook, 
														RMobilePhoneBookStore& aPhoneBookStore, 
														TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	//initialise IPC generated from phonebook store open
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);

	ASSERT_EQUALS(aPhoneBookStore.Open(iPhone,PhonebookName(aPhonebook)),KErrNone);

	AssertMockLtsyStatusL();
		
	//complete initialise IPC
    TMockLtsyData1<DispatcherPhonebook::TPhonebookStoreInfoV1> phonebookCompLtsyData(iPhonebookStoreData);
    phonebookCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId,aError,data);
	data.Close();
	
	CleanupStack::PopAndDestroy(&data);
	
	iInititalised = ETrue;
	}

/**
 * Helper funtion to open a caching phonebook (without the cache being completed)
 * 
 * @param aPhonebook The phonebook being tested
 * @param aPhoneBookStore The opened store
 * @param aError the Error to return from the LTSY
 */
void CCTsyPhonebookFU::OpenCachingPhoneBookL(	DispatcherPhonebook::TPhonebook aPhonebook, 
												RMobilePhoneBookStore& aPhoneBookStore, 
												TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	if(!iInititalised && InitialisationRequired(aPhonebook))
		{
		OpenPhoneBookWithInitialisationL(aPhonebook,aPhoneBookStore,aError);
		
		if(aError == KErrNone)
			{
			//cache IPC generated from complete initialise IPC
			TMockLtsyData1<DispatcherPhonebook::TPhonebook> cacheExpLtsyData(aPhonebook);
			cacheExpLtsyData.SerialiseL(data);
			iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId,data);
			data.Close();
			}

		//fire up the initialise complete
		WaitForMockLTSYTerminated();
		}
	else
		{
		//here the expect needs to be queued before the phone book open as if initialised then the open
		//generated the cache
		TMockLtsyData1<DispatcherPhonebook::TPhonebook> cacheExpLtsyData(aPhonebook);
		cacheExpLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId,data);
		data.Close();

		ASSERT_EQUALS(aPhoneBookStore.Open(iPhone,PhonebookName(aPhonebook)),KErrNone);
		}

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * Helper funtion to complete the cache phase of a cache request
 * 
 * @param aPhonebook The phonebook being tested
 * @param aError the Error to return from the LTSY
 * @param aIsRefresh If the cache request came from a SIM refresh request
 */
void CCTsyPhonebookFU::CompleteCacheL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aError, TBool aIsRefresh)
	{
	//generate the TLV
	RBuf8 tlv;
	CleanupClosePushL(tlv);
	
	CPhoneBookBuffer* pbBuffer = new (ELeave) CPhoneBookBuffer();
	CleanupStack::PushL(pbBuffer);
	
	pbBuffer->Set(&tlv);
	
	TInt tlvLength = 0;
	for(TInt i = 0; i < iEntries.Count(); ++i)
		{
		tlvLength += iEntries[i]->TlvLength();
		}

	tlv.ReAllocL(tlv.Length() + tlvLength);
	
	for(TInt j = 0; j < iEntries.Count(); ++j)
		{
		ASSERT_EQUALS(iEntries[j]->ExternalizeToTlvEntry(*pbBuffer),KErrNone);
		}
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TDesC8* tlvPtr = &tlv;
	TMockLtsyData2<DispatcherPhonebook::TPhonebook,TDesC8*> phonebookLtsyData(aPhonebook,tlvPtr);
	phonebookLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId,aError,data);
	CleanupStack::PopAndDestroy(&data);

	
	CleanupStack::PopAndDestroy(pbBuffer);
	CleanupStack::PopAndDestroy(&tlv);
			
	/********************************************************************************/
	
	if(aIsRefresh)
		{
		//the Refresh is complete	
		
		RBuf8 data2;
		CleanupClosePushL(data2);

		TRequestStatus reqStatusTerminated;
		iMockLTSY.NotifyTerminated(reqStatusTerminated); 
		
		TMockLtsyData1<TInt> simRefreshDoneExpLtsyData(aError);
		simRefreshDoneExpLtsyData.SerialiseL(data2);
		iMockLTSY.ExpectL(MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId,data2);
		CleanupStack::PopAndDestroy(&data2);

		User::WaitForRequest(reqStatusTerminated);
		ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());

		//no notification when refresh is originated from the SIM
		AssertMockLtsyStatusL();
		}
	
	//in the open case wait via a RMmCustomAPI::NotifyPndCacheReady()
	
	}

/**
 * Helper funtion to Open a non-caching phonebook
 * 
 * @param aPhonebook The phonebook being tested
 * @param aPhoneBookStore The opened store
 * @param aError the Error to return from the LTSY
 */
void CCTsyPhonebookFU::OpenNonCachingPhoneBookL(DispatcherPhonebook::TPhonebook aPhonebook, RMobilePhoneBookStore& aPhoneBookStore, TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	if(!iInititalised && InitialisationRequired(aPhonebook))
		{
		OpenPhoneBookWithInitialisationL(aPhonebook,aPhoneBookStore,aError);
		
		//fire up the initialise complete
		WaitForMockLTSYTerminated();
		}
	else
		{
		ASSERT_EQUALS(aPhoneBookStore.Open(iPhone,PhonebookName(aPhonebook)),KErrNone);
		}
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * Function to create the test data, this should be called at the start of a test case
 */
void CCTsyPhonebookFU::CreateEntriesAndStoreDataL()
	{
	ASSERT_EQUALS(iEntries.Count(),0);
	ASSERT_TRUE(iInititalised == EFalse);
	
	//for CPhoneBookEntry::NewL coverage
	CPhoneBookEntry* phoneBookEntry = CPhoneBookEntry::NewL();
	delete phoneBookEntry;
	phoneBookEntry = NULL;
		
	/************************Add Entry 0***********************************/
	phoneBookEntry = CPhoneBookEntry::NewLC();
	iEntries.AppendL(phoneBookEntry);
	CleanupStack::Pop(phoneBookEntry); //takes ownership
	phoneBookEntry->SetIndex(1);
	phoneBookEntry->SetFirstName(_L("Symbian0"));
	phoneBookEntry->SetDiallingNumber(_L("123456789"));
	phoneBookEntry->AddEmailAddress(_L("symbian1@symbian.com"));
	phoneBookEntry->AddSecondName(_L("Phone book3"));
	phoneBookEntry->AddAdditionalNumber(_L("123"));

	/************************Add Entry 1***********************************/
	phoneBookEntry = CPhoneBookEntry::NewLC();
	iEntries.AppendL(phoneBookEntry);
	CleanupStack::Pop(phoneBookEntry); //takes ownership
	phoneBookEntry->SetIndex(2);
	phoneBookEntry->SetFirstName(_L("Symbian1"));
	phoneBookEntry->SetDiallingNumber(_L("12345678910"));
	phoneBookEntry->AddEmailAddress(_L("symbian3@symbian.com"));
	phoneBookEntry->AddEmailAddress(_L("symbian4@symbian.com"));
	phoneBookEntry->AddSecondName(_L("Phone book3"));
	phoneBookEntry->AddSecondName(_L("Phone book4"));
	phoneBookEntry->AddAdditionalNumber(_L("789"));
	phoneBookEntry->AddAdditionalNumber(_L("101112"));

	/************************Add Entry 2***********************************/
	phoneBookEntry = CPhoneBookEntry::NewLC();
	iEntries.AppendL(phoneBookEntry);
	CleanupStack::Pop(phoneBookEntry); //takes ownership
	phoneBookEntry->SetIndex(3);
	phoneBookEntry->SetFirstName(_L("Symbian2"));
	phoneBookEntry->SetDiallingNumber(_L("12345678910"));
	phoneBookEntry->AddEmailAddress(_L("symbian3@symbian.com"));
	phoneBookEntry->AddEmailAddress(_L("symbian4@symbian.com"));
	phoneBookEntry->AddEmailAddress(_L("symbian4@symbian.com"));
	phoneBookEntry->AddSecondName(_L("Phone book3"));
	phoneBookEntry->AddSecondName(_L("Phone book4"));
	phoneBookEntry->AddSecondName(_L("Phone book4"));
	phoneBookEntry->AddAdditionalNumber(_L("789"));
	phoneBookEntry->AddAdditionalNumber(_L("101112"));
	phoneBookEntry->AddAdditionalNumber(_L("101112"));
	
	/************************Add Entry 3***********************************/
	phoneBookEntry = CPhoneBookEntry::NewLC();
	iEntries.AppendL(phoneBookEntry);
	CleanupStack::Pop(phoneBookEntry); //takes ownership
	phoneBookEntry->SetIndex(4);
	phoneBookEntry->SetFirstName(_L("Symbian3"));
	phoneBookEntry->SetDiallingNumber(_L("12345678910"));
	
	/************************Test TLV creation*********************************/
	
	for(TInt i = 0; i < iEntries.Count(); ++i)
		{
		TestTlvWriteReadL(*iEntries[i]);
		}
	
	/************************************************************************/
	
	
	iPhonebookStoreData.iAdnTotalEntries 			= iEntries.Count();
	iPhonebookStoreData.iAdnMaximumNumberLength 	= KMaxNumberLength;
	iPhonebookStoreData.iAdnMaximumTextLength 		= KMaxTextLength;

	iPhonebookStoreData.iFdnTotalEntries 			= iEntries.Count();
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

	
	iInititalised = EFalse;
	}

/**
 * Helper funtion to test the Read functionality
 * 
 * @param aPhonebook The phonebook being tested
 * @param aPhoneBookStore The opened store
 * @param aError the Error to return from the LTSY
 */
void CCTsyPhonebookFU::ReadL(DispatcherPhonebook::TPhonebook aPhonebook, const RMobilePhoneBookStore& aPhonebookStore, TInt aError)
	{
	if(CachingPhonebook(aPhonebook))
		{
		//read one entry, when one entry is read this does a non-cache read
		//thus down to the LTSY
		for(TInt i = 1; i <= iEntries.Count(); ++i)
			{
			NonCacheReadL(aPhonebook,aPhonebookStore,i,1,aError);
			}
				
		//read two entries, (> 1) thus a cache read
		CacheReadL(aPhonebookStore,1,iEntries.Count());
		}
	else //non caching phonebook
		{
		
		//test a single entry
		for(TInt i = 1; i <= iEntries.Count(); ++i)
			{
			NonCacheReadL(aPhonebook,aPhonebookStore,i,1,aError);
			}
		
		//test all entries
		NonCacheReadL(aPhonebook,aPhonebookStore,1,iEntries.Count(),aError);

		}
	}

/**
 * Helper funtion to test the Read functionality
 * 
 * @param aPhonebook The phonebook being tested
 * @param aError the Error to return from the LTSY
 */
void CCTsyPhonebookFU::ReadL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aError)
	{
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RMobilePhoneBookStore phoneBookStore;
	CleanupClosePushL(phoneBookStore);
	OpenPhoneBookL(aPhonebook,phoneBookStore,customApi);
	
	ReadL(aPhonebook,phoneBookStore,aError);

	// destroying the phonebook store, while one of the caching phonebooks
	// ADN/FDN is in the not-ready state will result in a store cache cancel
	// message being generated by the ctsy
	
	if (CachingPhonebook(aPhonebook))
		{
		RBuf8 data;
		CleanupClosePushL(data);
		
		TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(aPhonebook);
		expLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
		data.Close();
		
		CleanupStack::PopAndDestroy(&data);
		}
	
	CleanupStack::PopAndDestroy(&phoneBookStore);	
	CleanupStack::PopAndDestroy(&customApi);
	}

/**
 * Helper funtion to do a cache read
 * 
 * @param aPhoneBookStore The opened store
 * @param aIndex The index being read
 * @param aNumberOfSlots The number of slots being read
 */
void CCTsyPhonebookFU::CacheReadL(const RMobilePhoneBookStore& aPhoneBookStore, TInt aIndex, TInt aNumberOfSlots)
	{
	TRequestStatus requestStatus;
	
	RBuf8 phonebookTlv;
	CleanupClosePushL(phonebookTlv);
	phonebookTlv.CreateL(KReadBufLength * aNumberOfSlots);
	
	//the cache read
	aPhoneBookStore.Read(requestStatus,aIndex,aNumberOfSlots,phonebookTlv);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KErrNone)
	
	RPointerArray<CPhoneBookEntry> phonebookArray;
	TCleanupItem arrayCleanup( PhoneBookEntryArrayCleanup, &phonebookArray);
	CleanupStack::PushL(arrayCleanup);

	//convert TLV -> CPhoneBookEntry(s)
	FillPhonebookEntryArrayFromTlvL(phonebookTlv,phonebookArray);
	
	//check the TLV is the what we expect
	ASSERT_EQUALS(aNumberOfSlots,phonebookArray.Count());
	for(TInt i = 0; i < phonebookArray.Count(); ++i)
		{
		ASSERT_EQUALS(*phonebookArray[i],*iEntries[aIndex + i -1]);
		}

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&phonebookArray);
	CleanupStack::PopAndDestroy(&phonebookTlv);
	}

/**
 * Helper funtion to do a complete non-cache read
 * 
 * @param aPhonebook The phonebook being tested
 * @param aPhoneBookStore The opened store
 * @param aIndex The index being read
 * @param aNumberOfSlots The number of slots being read
 * @param aError The error to return from the LTSY
 */
void CCTsyPhonebookFU::NonCacheReadL(	DispatcherPhonebook::TPhonebook aPhonebook, 
										const RMobilePhoneBookStore& aPhoneBookStore, 
										TInt aIndex, 
										TInt aNumberOfSlots,
										TInt aError)
	{
	TRequestStatus requestStatus;

	RBuf8 readBuf;
	CleanupClosePushL(readBuf);
	readBuf.CreateL(KReadBufLength * aNumberOfSlots);
	
	ExpectNonCacheReadL(aPhonebook,aPhoneBookStore,aIndex,aNumberOfSlots,requestStatus,readBuf);
	CompleteNonCacheReadL(aPhonebook,aIndex,aNumberOfSlots,requestStatus,readBuf,aError);
	
	CleanupStack::PopAndDestroy(&readBuf);
	}

/**
 * Helper funtion to start the non-cache read
 * 
 * @param aPhonebook The phonebook being tested
 * @param aPhoneBookStore The opened store
 * @param aIndex The index being read
 * @param aNumberOfSlots The number of slots being read
 * @param aRequestStatus The pending TRequestStatus to wait on
 * @param aReadData The data read (Returned to ETel)
 * 
 */
void CCTsyPhonebookFU::ExpectNonCacheReadL(	DispatcherPhonebook::TPhonebook aPhonebook, 
											const RMobilePhoneBookStore& aPhoneBookStore,
											TInt aIndex,
											TInt aNumberOfSlots,
											TRequestStatus& aRequestStatus,
											TDes8& aReadData)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TMockLtsyData3<DispatcherPhonebook::TPhonebook,TInt,TInt> readEntryData(aPhonebook,aIndex,aNumberOfSlots);
	readEntryData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId,data);
	data.Close();
	
	aPhoneBookStore.Read(aRequestStatus,aIndex,aNumberOfSlots,aReadData);
	ASSERT_EQUALS(aRequestStatus.Int(),KRequestPending);
	
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * Helper funtion to complete the non-cache read
 * 
 * @param aPhonebook The phonebook being tested
 * @param aIndex The index being read
 * @param aNumberOfSlots The number of slots being read
 * @param aRequestStatus The pending TRequestStatus to wait on
 * @param aReadData The data read (Returned to ETel)
 * @param aError The error to return from the LTSY
 */
void CCTsyPhonebookFU::CompleteNonCacheReadL(	DispatcherPhonebook::TPhonebook aPhonebook, 
												TInt aIndex,
												TInt aNumberOfSlots,
												TRequestStatus& aRequestStatus, 
												const TDesC8& aReadData,
												TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	RPointerArray<CPhoneBookEntry> phonebook;
	CleanupClosePushL(phonebook);
	
	for(TInt i = 0; i < aNumberOfSlots; ++i)
		{
		phonebook.AppendL(iEntries[i + aIndex - 1]); //not owned
		}
	
	RBuf8 tlvEntry;
	CleanupClosePushL(tlvEntry);
	
	CreateTlvFromPhonebookL(phonebook,tlvEntry);
	
	TDesC8* tlvEntryPtr = &tlvEntry;
	TMockLtsyData2<DispatcherPhonebook::TPhonebook,TDesC8*> readEntryLtsyData(aPhonebook,tlvEntryPtr);
	readEntryLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId,aError,data);
	data.Close();
	
	User::WaitForRequest(aRequestStatus);

	ASSERT_EQUALS(aRequestStatus.Int(),aError)
	if(aError == KErrNone)
		{
		ASSERT_TRUE(tlvEntry == aReadData);
		}
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&tlvEntry);
	CleanupStack::PopAndDestroy(&phonebook);
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * Helper funtion to Test the Delete function on a phonebook
 * 
 * @param aPhonebook The phonebook to test
 * @param aIndexToDelete The index to remove
 * @param aError The error to return from the LTSY
 */
void CCTsyPhonebookFU::DeleteL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aIndexToDelete, TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RMobilePhoneBookStore phoneBookStore;
	CleanupClosePushL(phoneBookStore);
	
	OpenPhoneBookL(aPhonebook,phoneBookStore,customApi);
	
	TMockLtsyData2<DispatcherPhonebook::TPhonebook,TInt> deleteExpLtsyData(aPhonebook,aIndexToDelete);
	deleteExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId,data);
	data.Close();
		
	phoneBookStore.Delete(requestStatus,aIndexToDelete);

	TInt newMaxNumberLength = (	aPhonebook == DispatcherPhonebook::EIccFdn || 
								aPhonebook == DispatcherPhonebook::EIccAdn ||
								aPhonebook == DispatcherPhonebook::EIccVmb ) ? KNewMaxTextLength : KErrNotFound;

	TMockLtsyData2<DispatcherPhonebook::TPhonebook,TInt> deleteCompLtsyData(aPhonebook,newMaxNumberLength);
	deleteCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId, aError, data);
	data.Close();
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aError, requestStatus.Int());

	AssertMockLtsyStatusL();

	//now check newMaxNumberLength is correct in the CTSY
	
	switch(aPhonebook)
		{
		case DispatcherPhonebook::EIccAdn:
			iPhonebookStoreData.iAdnMaximumNumberLength = newMaxNumberLength;
			break;
		case DispatcherPhonebook::EIccFdn:
			iPhonebookStoreData.iFdnMaximumNumberLength = newMaxNumberLength;
			break;
		case DispatcherPhonebook::EIccVmb:
			iPhonebookStoreData.iVmbMaximumNumberLength = newMaxNumberLength;
			break;
		default:
			break;
		}
	
	CheckPhonebookDetailsL(aPhonebook,phoneBookStore,iEntries.Count() - ((aError == KErrNone) ? 1 : 0),aError);


	// destroying the phonebook store, while one of the caching phonebooks
	// ADN/FDN is in the not-ready state will result in a store cache cancel
	// message being generated by the ctsy
	
	if (CachingPhonebook(aPhonebook))
		{
		TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(aPhonebook);
		expLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
		data.Close();
		}
	
	CleanupStack::PopAndDestroy(&phoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * Helper funtion to Test DeleteAll function on a phonebook
 * 
 * @param aPhonebook The phonebook to test
 * @param aError The error to return from the LTSY
 */
void CCTsyPhonebookFU::DeleteAllL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RMobilePhoneBookStore phoneBookStore;
	CleanupClosePushL(phoneBookStore);
	
	OpenPhoneBookL(aPhonebook,phoneBookStore,customApi);
	
	TMockLtsyData1<DispatcherPhonebook::TPhonebook> deleteAllExpLtsyData(aPhonebook);
	deleteAllExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId,data);
	data.Close();

	phoneBookStore.DeleteAll(requestStatus);

	TInt newMaxNumberLength = (	aPhonebook == DispatcherPhonebook::EIccFdn || 
								aPhonebook == DispatcherPhonebook::EIccAdn ||
								aPhonebook == DispatcherPhonebook::EIccVmb ) ? KNewMaxTextLength : KErrNotFound;

	
	TMockLtsyData2<DispatcherPhonebook::TPhonebook,TInt> deleteAllCompLtsyData(aPhonebook,newMaxNumberLength);
	deleteAllCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId,aError,data);
	data.Close();
		
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aError, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	//now check newMaxNumberLength is correct in the CTSY
	
	switch(aPhonebook)
		{
		case DispatcherPhonebook::EIccAdn:
			iPhonebookStoreData.iAdnMaximumNumberLength = newMaxNumberLength;
			break;
		case DispatcherPhonebook::EIccFdn:
			iPhonebookStoreData.iFdnMaximumNumberLength = newMaxNumberLength;
			break;
		case DispatcherPhonebook::EIccVmb:
			iPhonebookStoreData.iVmbMaximumNumberLength = newMaxNumberLength;
			break;
		default:
			break;
		}

	CheckPhonebookDetailsL(aPhonebook,phoneBookStore,(aError == KErrNone) ? 0 : iEntries.Count(),aError);

	// destroying the phonebook store, while one of the caching phonebooks
	// ADN/FDN is in the not-ready state will result in a store cache cancel
	// message being generated by the ctsy
	
	if (CachingPhonebook(aPhonebook))
		{
		TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(aPhonebook);
		expLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
		data.Close();
		}

	CleanupStack::PopAndDestroy(&phoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * Helper funtion to Test the writing to a phonebook
 * 
 * @param aPhonebook The phonebook to test
 * @param aPhoneBookEntry The entry to write
 * @param aError The error to return from the LTSY
 */
void CCTsyPhonebookFU::WriteL(DispatcherPhonebook::TPhonebook aPhonebook, const CPhoneBookEntry& aPhoneBookEntry, TInt aError)
	{
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RMobilePhoneBookStore phoneBookStore;
	CleanupClosePushL(phoneBookStore);

	OpenPhoneBookL(aPhonebook,phoneBookStore,customApi);
	WriteL(aPhonebook,phoneBookStore,aPhoneBookEntry,aError);

	// destroying the phonebook store, while one of the caching phonebooks
	// ADN/FDN is in the not-ready state will result in a store cache cancel
	// message being generated by the ctsy
	
	if (CachingPhonebook(aPhonebook))
		{
		RBuf8 data;
		CleanupClosePushL(data);
		
		TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(aPhonebook);
		expLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
		data.Close();
		
		CleanupStack::PopAndDestroy(&data);
		}
	
	CleanupStack::PopAndDestroy(&phoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	}

/**
 * Helper funtion to Test the writing to a phonebook
 * 
 * @param aPhonebook The phonebook to test
 * @param aPhoneBookStore The opened store
 * @param aPhoneBookEntry The entry to write
 * @param aError The error to return from the LTSY
 */
void CCTsyPhonebookFU::WriteL(	DispatcherPhonebook::TPhonebook aPhonebook, 
								const RMobilePhoneBookStore& aPhonebookStore, 
								const CPhoneBookEntry& aPhoneBookEntry, 
								TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	
	RBuf8 buf;
	CleanupClosePushL(buf);
	buf.CreateL(aPhoneBookEntry.TlvLength());
	CreateTlvFromEntryL(aPhoneBookEntry,buf);

	TDesC8* bufPtr = &buf;
	TMockLtsyData2<DispatcherPhonebook::TPhonebook,TDesC8*> writeEntryExpLtsyData(aPhonebook,bufPtr);
	writeEntryExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId,data);
	data.Close();
	
	TInt index = aPhoneBookEntry.GetIndex();
	aPhonebookStore.Write(requestStatus,buf,index);

	TUint16 writtenIndex = aPhoneBookEntry.GetIndex();
	
	TInt newMaxNumberLength = (	aPhonebook == DispatcherPhonebook::EIccFdn || 
								aPhonebook == DispatcherPhonebook::EIccAdn ||
								aPhonebook == DispatcherPhonebook::EIccVmb ) ? KNewMaxTextLength : KErrNotFound;

	TMockLtsyData3<DispatcherPhonebook::TPhonebook,TUint16,TInt> writeEntryCompLtsyData(aPhonebook,writtenIndex,newMaxNumberLength);
	writeEntryCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId,aError,data);
	data.Close();
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aError, requestStatus.Int());

	//index is passed in as a reference and returned where the entry was written
	ASSERT_TRUE(index == writtenIndex); 
	
	AssertMockLtsyStatusL();
	
	//now check newMaxNumberLength is correct in the CTSY
	
	switch(aPhonebook)
		{
		case DispatcherPhonebook::EIccAdn:
			iPhonebookStoreData.iAdnMaximumNumberLength = newMaxNumberLength;
			break;
		case DispatcherPhonebook::EIccFdn:
			iPhonebookStoreData.iFdnMaximumNumberLength = newMaxNumberLength;
			break;
		case DispatcherPhonebook::EIccVmb:
			iPhonebookStoreData.iVmbMaximumNumberLength = newMaxNumberLength;
			break;
		default:
			break;
		}
		
	const TInt newEntryCount = (aError == KErrNone || 
								aError == KErrGsmSimServSneFull || 
								aError == KErrGsmSimServAnrFull || 
								aError == KErrGsmSimServEmailFull) ? 1 : 0;
	CheckPhonebookDetailsL(aPhonebook,aPhonebookStore,iEntries.Count() + newEntryCount,aError);
	
	CleanupStack::PopAndDestroy(&buf);
	CleanupStack::PopAndDestroy(&data);
	}


/**
 * Helper funtion to check the phonebook store details.
 * 
 * @param aPhonebook The phonebook to test
 * @param aPhonebookStore The open store
 * @param aEntries The number of entries expected in the store
 * @param 
 */
void CCTsyPhonebookFU::CheckPhonebookDetailsL(	DispatcherPhonebook::TPhonebook aPhonebook, 
												const RMobilePhoneBookStore& aPhonebookStore,
												TInt aEntries,
												TInt aError)
	{
	switch(aPhonebook)
		{
		case DispatcherPhonebook::EIccAdn:
		case DispatcherPhonebook::EIccFdn:
		case DispatcherPhonebook::EIccMbdn:
			{
			GetInfoFromCtsy(aPhonebook,aPhonebookStore,aEntries);
			}
			break;
		case DispatcherPhonebook::EIccSdn:
		case DispatcherPhonebook::EIccVmb:
			{
			GetInfoFromLtsyL(aPhonebook,aPhonebookStore,aEntries,aError);
			}
		default:
			{
			}
			break;
		}
	}

/**
 * Helper funtion to Test getting the phonebook store info from the CTSY & LTSY
 * 
 * @param aPhonebook The phonebook to test
 * @param aError The error to return from the LTSY
 */
void CCTsyPhonebookFU::GetInfoL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aError)
	{
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RMobilePhoneBookStore phoneBookStore;
	CleanupClosePushL(phoneBookStore);

	OpenPhoneBookL(aPhonebook,phoneBookStore,customApi);
	
	CheckPhonebookDetailsL(aPhonebook,phoneBookStore,iEntries.Count(),aError);
	
	// destroying the phonebook store, while one of the caching phonebooks
	// ADN/FDN is in the not-ready state will result in a store cache cancel
	// message being generated by the ctsy
	
	if (CachingPhonebook(aPhonebook))
		{
		RBuf8 data;
		CleanupClosePushL(data);
		
		TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(aPhonebook);
		expLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
		data.Close();
		
		CleanupStack::PopAndDestroy(&data);
		}
	
	CleanupStack::PopAndDestroy(&phoneBookStore);
	CleanupStack::PopAndDestroy(&customApi);
	}


/**
 * Helper funtion to Test getting the phonebook store info from the CTSY
 * 
 * @param aPhonebook The phonebook to test
 */
void CCTsyPhonebookFU::GetInfoFromCtsy(DispatcherPhonebook::TPhonebook aPhonebook, 
										const RMobilePhoneBookStore& aPhonebookStore,
										TInt aUsedEntries)
	{
	TRequestStatus requestStatus;
	
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookInfoPckg(bookInfo);
	
	aPhonebookStore.GetInfo(requestStatus,bookInfoPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone,				requestStatus.Int());
	ASSERT_EQUALS(bookInfo.iType,		RMobilePhoneStore::EPhoneBookStore);
	ASSERT_EQUALS(bookInfo.iLocation, 	RMobilePhoneBookStore::ELocationIccMemory);
	
	switch(aPhonebook)
		{
		case DispatcherPhonebook::EIccAdn:
			{
			ASSERT_EQUALS(bookInfo.iTotalEntries, iPhonebookStoreData.iAdnTotalEntries);
			ASSERT_EQUALS(bookInfo.iUsedEntries,  aUsedEntries);
			ASSERT_EQUALS(bookInfo.iMaxNumLength, iPhonebookStoreData.iAdnMaximumNumberLength);
			ASSERT_EQUALS(bookInfo.iMaxTextLength,iPhonebookStoreData.iAdnMaximumTextLength);
			}
			break;
		
		case DispatcherPhonebook::EIccFdn:
			{
			ASSERT_EQUALS(bookInfo.iTotalEntries, 	iPhonebookStoreData.iFdnTotalEntries);
			ASSERT_EQUALS(bookInfo.iUsedEntries, 	aUsedEntries);
			ASSERT_EQUALS(bookInfo.iMaxNumLength, 	iPhonebookStoreData.iFdnMaximumNumberLength);
			ASSERT_EQUALS(bookInfo.iMaxTextLength, 	iPhonebookStoreData.iFdnMaximumTextLength);
			}
			break;

		case DispatcherPhonebook::EIccMbdn:
			{
			ASSERT_EQUALS(bookInfo.iTotalEntries,	iPhonebookStoreData.iMbdnTotalEntries);
			//The CTSY does not cache Mbdn entires but does not sent a request down to the LTSY
			//like other non cached phonebooks for the used entires count, possible CTSY defect?
			//ASSERT_EQUALS(bookInfo.iUsedEntries,	aUsedEntries);
			ASSERT_EQUALS(bookInfo.iMaxNumLength,	iPhonebookStoreData.iMbdnMaximumNumberLength);
			ASSERT_EQUALS(bookInfo.iMaxTextLength,	iPhonebookStoreData.iMbdnMaximumTextLength);

			}
			break;

		default:
			{
			}
			break;
		}
	}

/**
 * Helper funtion to Test getting the phonebook store info from the LTSY
 * 
 * @param aPhonebook The phonebook to test
 * @param aError The error to return from the LTSY
 */
void CCTsyPhonebookFU::GetInfoFromLtsyL(DispatcherPhonebook::TPhonebook aPhonebook, 
										const RMobilePhoneBookStore& aPhonebookStore, 
										TInt aUsedEntries, 
										TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	
	TMockLtsyData1<DispatcherPhonebook::TPhonebook> getInfoExpLtsyData(aPhonebook);
	getInfoExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId,data);
	data.Close();
	
	
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookInfoPckg(bookInfo);
	aPhonebookStore.GetInfo(requestStatus,bookInfoPckg);

	TMockLtsyData2<DispatcherPhonebook::TPhonebook,TInt> getInfoCompLtsyData(aPhonebook,aUsedEntries);
	getInfoCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId,aError,data);
	data.Close();
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aError,requestStatus.Int());
	ASSERT_EQUALS(bookInfo.iType,		RMobilePhoneStore::EPhoneBookStore);
	ASSERT_EQUALS(bookInfo.iLocation,	RMobilePhoneBookStore::ELocationIccMemory);
	
	if(aError == KErrNone)
		{
		ASSERT_EQUALS(bookInfo.iUsedEntries,aUsedEntries);
		}

	switch(aPhonebook)
		{
		case DispatcherPhonebook::EIccSdn:
			{
			ASSERT_EQUALS(bookInfo.iTotalEntries,iPhonebookStoreData.iSdnTotalEntries);
			ASSERT_EQUALS(bookInfo.iMaxNumLength,iPhonebookStoreData.iSdnMaximumNumberLength);
			ASSERT_EQUALS(bookInfo.iMaxTextLength,iPhonebookStoreData.iSdnMaximumTextLength);
			}
			break;
			
		case DispatcherPhonebook::EIccVmb:
			{
			ASSERT_EQUALS(bookInfo.iTotalEntries,iPhonebookStoreData.iVmbTotalEntries);
			ASSERT_EQUALS(bookInfo.iMaxNumLength,iPhonebookStoreData.iVmbMaximumNumberLength);
			ASSERT_EQUALS(bookInfo.iMaxTextLength,iPhonebookStoreData.iVmbMaximumTextLength);
			}
			break;
			
		default:
			{
			
			}
			break;
		}

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * Helper funtion to Test the Set Fdn Info IPC
 * 
 * @param aError The error to return from the LTSY
 */
void CCTsyPhonebookFU::SetFdnInfoL(RMobilePhoneBookStore& aPhoneBookStore, TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);

    TInt totalEntries 			= iEntries.Count();
	TInt maximumTextLength  	= KMaxTextLength;
	TInt maximumNumberLength 	= KMaxNumberLength;
		
	TMockLtsyData3<TInt,TInt,TInt> phonebookDataCompLtsyData(totalEntries,maximumTextLength,maximumNumberLength);
    phonebookDataCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhonebookStoreSetFdnPhonebookInfoIndId,aError,data);
	data.Close();
	
	WaitForMockLTSYTerminated();

	AssertMockLtsyStatusL();
	
	//now check the info
	
	if(aError == KErrNone)
		{
		iPhonebookStoreData.iFdnTotalEntries 			= totalEntries;
		iPhonebookStoreData.iFdnMaximumTextLength 	= maximumTextLength;
		iPhonebookStoreData.iFdnMaximumNumberLength 	= maximumNumberLength;
		}

	TRequestStatus requestStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookInfoPckg(bookInfo);
	aPhoneBookStore.GetInfo(requestStatus,bookInfoPckg);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone,					requestStatus.Int());
	ASSERT_EQUALS(bookInfo.iType,			RMobilePhoneStore::EPhoneBookStore);
	ASSERT_EQUALS(bookInfo.iLocation, 		RMobilePhoneBookStore::ELocationIccMemory);
	ASSERT_EQUALS(bookInfo.iTotalEntries, 	iPhonebookStoreData.iFdnTotalEntries);
	ASSERT_EQUALS(bookInfo.iUsedEntries, 	iEntries.Count());
	ASSERT_EQUALS(bookInfo.iMaxNumLength, 	iPhonebookStoreData.iFdnMaximumNumberLength);
	ASSERT_EQUALS(bookInfo.iMaxTextLength, 	iPhonebookStoreData.iFdnMaximumTextLength);

	CleanupStack::PopAndDestroy(&data);
	}

/**
 * Helper funtion to Test the Refresh IPC
 * 
 * @param aPhonebook The phonebook to test
 */
void CCTsyPhonebookFU::RefreshL(DispatcherPhonebook::TPhonebook aPhonebook)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	//KCacheAdn KCacheFdn KCacheServiceTable KCacheALSline
	TUint16 refreshFileList = 0;
	
	switch(aPhonebook)
		{
		case DispatcherPhonebook::EIccAdn:
			refreshFileList = KCacheAdn;
			break;
		case DispatcherPhonebook::EIccFdn:
			refreshFileList = KCacheFdn;
			break;
		default:
			break;
		}
	
	TMockLtsyData1<TUint16> refreshFileListData(refreshFileList);
	refreshFileListData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSimRefreshSimFilesIndId, KErrNone, data);
	data.Close();
	
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId);
	
	// Prepare data for the mockLtsy's CompleteL Note the CTSY expects the data in 8-bit format.
	_LIT8  (KIMSI8, "012012012012999");
	TBuf8<RMobilePhone::KIMSISize> idComp(KIMSI8);
	TMockLtsyData1<TBuf8<RMobilePhone::KIMSISize> > idCompData(idComp);
	idCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId, KErrNone, data);
	data.Close();
	
	TMockLtsyData1<DispatcherPhonebook::TPhonebook> expLtsyData(aPhonebook);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,data);
	data.Close();
	

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);
	
	//complete initialise IPC
    TMockLtsyData1<DispatcherPhonebook::TPhonebookStoreInfoV1> phonebookCompLtsyData(iPhonebookStoreData);
    phonebookCompLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId,KErrNone,data);
	data.Close();
	
	TMockLtsyData1<DispatcherPhonebook::TPhonebook> cacheExpLtsyData(aPhonebook);
	cacheExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId,data);
	data.Close();
	
	CompleteCacheL(aPhonebook, KErrNone, ETrue);
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * @param aPhonebook The phonebook to check
 * @return True if the phonebook sends down the initialise IPC on the first phonebook opened
 */ 
TBool CCTsyPhonebookFU::InitialisationRequired(DispatcherPhonebook::TPhonebook aPhonebook) const
	{
	switch (aPhonebook)
		{
		case DispatcherPhonebook::EIccAdn:
		case DispatcherPhonebook::EIccFdn:
		case DispatcherPhonebook::EIccVmb:
		case DispatcherPhonebook::EIccMbdn:
		case DispatcherPhonebook::EIccSdn:
		case DispatcherPhonebook::EIccBdn:
			return ETrue;
		
		default:
			return EFalse;
		}
	}

/**
 * @param aPhonebook The phonebook to check
 * @return True if the phonebook caches the entries, false otherwise
 */

TBool CCTsyPhonebookFU::CachingPhonebook(DispatcherPhonebook::TPhonebook aPhonebook) const
	{
	switch (aPhonebook)
		{
		case DispatcherPhonebook::EIccAdn:
		case DispatcherPhonebook::EIccFdn:
			return ETrue;
		default:
			return EFalse;
			
		}
	}

/**
 * Creates a TLV phonebook entry from a CPhoneBookEntry
 * 
 * @param aPhoneBookEntry The entry to create the TLV from
 * @param aBuf The buffer to write the TLV to
 */
void CCTsyPhonebookFU::CreateTlvFromEntryL(const CPhoneBookEntry& aPhoneBookEntry, RBuf8& aBuf)
	{
	RPointerArray<CPhoneBookEntry> phonebook;
	CleanupClosePushL(phonebook);
	
	phonebook.AppendL(&aPhoneBookEntry);
	
	CreateTlvFromPhonebookL(phonebook,aBuf);
	
	CleanupStack::Pop(&phonebook);
	phonebook.Close();
	}

/**
 * Creates a TLV phonebook entry from a CPhoneBookEntry array
 * 
 * @param aPhonebook The phonebook entries to create the TLV from
 * @param aBuf The buffer to write the TLV to
 */
void CCTsyPhonebookFU::CreateTlvFromPhonebookL(const RPointerArray<CPhoneBookEntry>& aPhonebook, RBuf8& aBuf)
	{
	TInt tlvSize = 0;
	for(TInt i = 0; i < aPhonebook.Count(); ++i)
		{
		tlvSize += aPhonebook[i]->TlvLength();
		}
	
	aBuf.ReAllocL(tlvSize);
	
	CPhoneBookBuffer* pbBuffer = new (ELeave) CPhoneBookBuffer();
	CleanupStack::PushL(pbBuffer);

	pbBuffer->Set(&aBuf);
	
	for(TInt j = 0; j < aPhonebook.Count(); ++j)
		{
		aPhonebook[j]->ExternalizeToTlvEntry(*pbBuffer);
		}
	
	CleanupStack::PopAndDestroy(pbBuffer);
	}

/**
 * Fills a CPhoneBookEntry array from a phonebook TLV
 * 
 * @param aEntry The entry in TLV form
 * @param aPhoneBook The entry array to be filled
 */
void CCTsyPhonebookFU::FillPhonebookEntryArrayFromTlvL(const TDesC8& aEntry, RPointerArray<CPhoneBookEntry>& aPhoneBook)
	{
	CPhoneBookBuffer* pbBuffer = new (ELeave) CPhoneBookBuffer();
	CleanupStack::PushL(pbBuffer);
	
	RBuf8 buf;
	CleanupClosePushL(buf);
	
	buf.CreateL(aEntry);
	
	pbBuffer->Set(&buf);
	pbBuffer->StartRead();

	TInt entriesCount = 0;
	while(pbBuffer->RemainingReadLength() > 0)
		{
		entriesCount++;
		CPhoneBookEntry* entry = CPhoneBookEntry::NewLC();
		aPhoneBook.AppendL(entry);
		CleanupStack::Pop(entry);

		ASSERT_EQUALS(entry->InternalizeFromTlvEntry(*pbBuffer,entriesCount > 1),KErrNone);
		}
	
	
	CleanupStack::PopAndDestroy(&buf);
	CleanupStack::PopAndDestroy(pbBuffer);
	}

/**
 * Test the CPhoneBookEntry ExternalizeToTlvEntry and InternalizeFromTlvEntry functions
 * 
 * @param aPhoneBookEntry The entry to test
 */

void CCTsyPhonebookFU::TestTlvWriteReadL(const CPhoneBookEntry& aPhoneBookEntry)
	{
	RBuf8 tlv;
	CleanupClosePushL(tlv);
	tlv.CreateL(aPhoneBookEntry.TlvLength());
		
	CPhoneBookBuffer* phonebookBuffer = new (ELeave) CPhoneBookBuffer();
	CleanupStack::PushL(phonebookBuffer);
	phonebookBuffer->Set(&tlv);
		
	//convert to a TLV
	ASSERT_EQUALS(aPhoneBookEntry.ExternalizeToTlvEntry(*phonebookBuffer),KErrNone);
	
	//the length should equal the maxlength if CPhoneBookEntry::TlvLength() is correct
	//(with 3 spare slots for worst case padding)
	ASSERT_EQUALS(tlv.Length() + 3,tlv.MaxLength());

	CPhoneBookEntry* phonebookEntry = CPhoneBookEntry::NewLC();

	phonebookBuffer->StartRead();
		
	//convert the TLV back to a phone book entry
	ASSERT_EQUALS(phonebookEntry->InternalizeFromTlvEntry(*phonebookBuffer),KErrNone);

	ASSERT_EQUALS(aPhoneBookEntry,*phonebookEntry);

	CleanupStack::PopAndDestroy(phonebookEntry);
	CleanupStack::PopAndDestroy(phonebookBuffer);
	CleanupStack::PopAndDestroy(&tlv);
	}

/**
 * Converts a Phone enum type to the ETel name
 * 
 * @param aPhonebook The phonebook type
 * @return The ETel store name
 */
TName CCTsyPhonebookFU::PhonebookName(DispatcherPhonebook::TPhonebook aPhonebook) const
	{
	
	TName phonebookName;
	
	switch(aPhonebook)
		{
		case DispatcherPhonebook::EIccAdn:
			{
			phonebookName = KETelIccAdnPhoneBook;
			}
			break;

		case DispatcherPhonebook::EIccBdn:
			{
			phonebookName = KETelIccBdnPhoneBook;
			}
			break;

		case DispatcherPhonebook::EIccSdn:
			{
			phonebookName = KETelIccSdnPhoneBook;
			}
			break;

		case DispatcherPhonebook::EIccFdn:
			{
			phonebookName = KETelIccFdnPhoneBook;
			}
			break;

		case DispatcherPhonebook::EIccVmb:
			{
			phonebookName = KETelIccVoiceMailBox;
			}
			break;
			
		case DispatcherPhonebook::EIccMbdn:
			{
			phonebookName = KETelIccMbdnPhoneBook;
			}
			break;

		case DispatcherPhonebook::EUnknown:
		default:
			{
			//do nothing;
			}
		}
	
	return phonebookName;
	}

void CCTsyPhonebookFU::PhoneBookEntryArrayCleanup(TAny* aArray)
	{
	static_cast<RPointerArray<CPhoneBookEntry>*>(aArray)->ResetAndDestroy();
	}

void CCTsyPhonebookFU::OpenSmsStoreL(RMobileSmsMessaging& aSmsMessaging, RMobileSmsStore& aSmsStore, const TDesC& aSmsStoreName)
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

void CCTsyPhonebookFU::DoCleanup()
	{
	iInititalised = EFalse;
	iEntries.ResetAndDestroy();
	CCtsyComponentTestBase::DoCleanup();
	}


