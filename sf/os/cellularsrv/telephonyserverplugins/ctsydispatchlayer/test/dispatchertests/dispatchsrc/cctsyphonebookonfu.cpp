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
 @file The TEFUnit test suite for PhonebookOnControl in the Common TSY.
*/

#include "cctsyphonebookonfu.h"
#include <etel.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchphonebookoninterface.h>
#include "mockltsyindicatorids.h"
#include "listretrieverao.h"

CTestSuite* CCTsyPhonebookOnFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	//add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFU, TestUseCase0002L);
  
	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFU, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFU, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFU, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookOnFU, TestUnit0007L);
	
	END_SUITE;
	}

	
void CCTsyPhonebookOnFU::GenerateONList3L(CMobilePhoneONList* aONList)
/*
 * Create a list of 3 ON entries
 * @param aONList The list to be returned.
 * 
 */
	{
	RMobileONStore::TMobileONEntryV1 writeEntry;

	writeEntry.iMode = RMobilePhone::ENetworkModeGsm; 							
	writeEntry.iService = RMobilePhone::ETelephony;								
	writeEntry.iNumber.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;		
	writeEntry.iNumber.iNumberPlan = RMobilePhone::EDataNumberPlan;				
	
	for (TInt i=0; i<3; i++)
		{
		writeEntry.iNumber.iTelNumber.Format(KText,i+1);								
		writeEntry.iText.Format(KNumber,i+1);												
		writeEntry.iIndex = i+1;														
		aONList->AddEntryL(writeEntry);
		}
	}


void CCTsyPhonebookOnFU::StoreAllL(RMobileONStore& aOnStore)
/*
 * Store all entries into the ON Storage.
 * 
 * @param aOnStore The ON storage where the entries will be stored.
 */
	{
	TRequestStatus reqStatus;
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	TRequestStatus notifyStatus;
	TUint32 notifyEvent;
	TInt notifyIndex;
	
	RBuf8 expectEntryData;
	CleanupClosePushL(expectEntryData);	

    CMobilePhoneONList* onList = CMobilePhoneONList::NewL();
    CleanupStack::PushL(onList);
    
    
    GenerateONList3L(onList);
    TInt size = onList->Enumerate();
    
    TMockLtsyData1<TInt> sizeLTsyData(size);
    completeData.Close();
    sizeLTsyData.SerialiseL(completeData);
           
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId, KErrNone, completeData);	
    
    RMobileONStore::TMobileONEntryV1 writeEntry[3];
    for (TInt i=0; i<size; i++)
    	{
    	writeEntry[i]=onList->GetEntryL(i);
    	
    	TMockLtsyData1<RMobileONStore::TMobileONEntryV1> entryLTsyData(writeEntry[i]);
    	expectEntryData.Close();
    	entryLTsyData.SerialiseL(expectEntryData);
    	
    	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId, expectEntryData);
    	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId, KErrNone);
    	}
	
	aOnStore.NotifyStoreEvent(notifyStatus, notifyEvent, notifyIndex);
	
	aOnStore.StoreAllL(reqStatus, onList);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	
	User::WaitForRequest(notifyStatus);
	
	TUint32 tempEventStoreAll = RMobilePhoneStore::KStoreDoRefresh;
	ASSERT_EQUALS(notifyEvent, tempEventStoreAll); 
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, &completeData); //  onList, expectEntryData, completeData
	}

void CCTsyPhonebookOnFU::ReadAllL(RMobileONStore& aOnStore)
/*
 * Read all entries from the ON Storage.
 * 
 * @param aAsyncRetrieveList 
 * @param aActiveRetriever 
 */
	{
    TInt size = 3;
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RBuf8 expectReadData;
	CleanupClosePushL(expectReadData);	

	TMockLtsyData1<TInt> sizeReadAllLTsyData(size);
    completeData.Close();
    sizeReadAllLTsyData.SerialiseL(completeData);
    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId, KErrNone, completeData);

    TInt location[3];
    TBuf<RMobileONStore::KOwnNumberTextSize> name[3];
    TBuf<KPBStoreNumSize> telNumber[3];
   
    for (TInt i=0; i<size; i++)
    	{
    	TInt index = i+1;
    	   	
    	location[i] = index;	
    	name[i].Format(KText,index);	
    	telNumber[i].Format(KNumber,index);
    	
    	TMockLtsyData1<TInt> locationLTsyData(location[i]);
    	expectReadData.Close();
    	locationLTsyData.SerialiseL(expectReadData);
    	    
        TMockLtsyData3< TInt, TBuf<RMobileONStore::KOwnNumberTextSize>, TBuf<KPBStoreNumSize> > readLTsyDataComplete(location[i], name[i], telNumber[i]);
        completeData.Close();
        readLTsyDataComplete.SerialiseL(completeData);
    	
        iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId, expectReadData);
        iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId, KErrNone, completeData);
    	}
    
    CGetONListAO* getOnListAO = CGetONListAO::NewLC(aOnStore);

    getOnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNone, getOnListAO->RetrieveLastError());

	CMobilePhoneONList* list = getOnListAO->RetrieveListL();
	
	for (TInt j=0; j< size; j++)
		{
		RMobileONStore::TMobileONEntryV1 readEntry=list->GetEntryL(j);
		ASSERT_EQUALS(location[j], readEntry.iIndex);
		ASSERT_EQUALS(name[j], readEntry.iText);
		ASSERT_EQUALS(telNumber[j], readEntry.iNumber.iTelNumber);
		}

	AssertMockLtsyStatusL();
    
	CActiveScheduler::Install(NULL);
	
	CleanupStack::PopAndDestroy(3, &completeData); // getOnListAO, expectReadData, completeData
	}



//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::GetInfo(), RMobileONStore::Write(), RMobileONStore::Read(), RMobileONStore::Delete()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::GetInfo(), RMobileONStore::Write(), RMobileONStore::Read(), RMobileONStore::Delete()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookOnFU::TestUseCase0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
    RMobilePhoneBookStore adnPhoneBookStore;
    CleanupClosePushL(adnPhoneBookStore);
    OpenCachingPhoneBookL(DispatcherPhonebook::EIccAdn,adnPhoneBookStore,KErrNone);
	
	RMobileONStore onStore;
	TInt ret = onStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(onStore);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);	
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);	

	TRequestStatus reqStatus;
	
	TRequestStatus notifyStatus;
	TUint32 notifyEvent;
	TInt notifyIndex;
	
	//-------------------------------------------------------------------------
	// TEST A: getinfo	
 	//-------------------------------------------------------------------------
	
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfoV1;
	TPckg<RMobileONStore::TMobileONStoreInfoV1> pckgInfoV1(onStoreInfoV1);
		
	TInt numOfEntries = 0;
	TInt usedEntries = 0;
	TInt maxNameLen = 25;
	TInt maxNumLen = 20;
		
	TMockLtsyData4<TInt, TInt, TInt, TInt> onStoreInfoReturn(numOfEntries, usedEntries, maxNameLen, maxNumLen);
	completeData.Close();
	onStoreInfoReturn.SerialiseL(completeData);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId, KErrNone, completeData);
	
	onStore.GetInfo(reqStatus, pckgInfoV1);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(onStoreInfoV1.iTotalEntries, numOfEntries);
	ASSERT_EQUALS(onStoreInfoV1.iUsedEntries, usedEntries);
	ASSERT_EQUALS(onStoreInfoV1.iTextLen, maxNameLen);
	ASSERT_EQUALS(onStoreInfoV1.iNumberLen, maxNumLen);
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: write	
 	//-------------------------------------------------------------------------
	TInt location = 1;
		
	RMobileONStore::TMobileONEntryV1 writeEntry1;

    writeEntry1.iMode = RMobilePhone::ENetworkModeGsm;
    writeEntry1.iService = RMobilePhone::ETelephony;
    writeEntry1.iNumber.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
    writeEntry1.iNumber.iNumberPlan = RMobilePhone::EDataNumberPlan;
    writeEntry1.iNumber.iTelNumber.Copy(KNumber1);
    writeEntry1.iText.Copy(KText1);
    writeEntry1.iIndex = location;
    
    TPckg<RMobileONStore::TMobileONEntryV1> pckgWriteEntry1(writeEntry1);
    
    TMockLtsyData1<RMobileONStore::TMobileONEntryV1> writeLTsyData1(writeEntry1);
    expectData.Close();
    writeLTsyData1.SerialiseL(expectData);
    
    TMockLtsyData1<TInt> writeLTsyDataComplete(location);
    completeData.Close();
    writeLTsyDataComplete.SerialiseL(completeData);
	
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, KErrNone, completeData);
	
	onStore.NotifyStoreEvent(notifyStatus, notifyEvent, notifyIndex);	
	
	onStore.Write(reqStatus, pckgWriteEntry1);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(writeEntry1.iIndex, location);
	
	User::WaitForRequest(notifyStatus);
	TUint32 tempEventEntryAdded = RMobilePhoneStore::KStoreEntryAdded;
	ASSERT_EQUALS(notifyEvent, tempEventEntryAdded);
	ASSERT_EQUALS(notifyIndex, location);
	
	AssertMockLtsyStatusL();
    
	//-------------------------------------------------------------------------
	// TEST C: read	
 	//-------------------------------------------------------------------------
	
	RMobileONStore::TMobileONEntryV1 readEntry;
	readEntry.iIndex = location;
	TPckg<RMobileONStore::TMobileONEntryV1> pckgReadEntry(readEntry);
	
	TBuf<RMobileONStore::KOwnNumberTextSize> name;
	name.Copy(KText1);
	TBuf<KPBStoreNumSize> telNumber;
	telNumber.Copy(KNumber1);
	
    TMockLtsyData1<TInt> readLTsyData(location);
    expectData.Close();
    readLTsyData.SerialiseL(expectData);
    
    TMockLtsyData3< TInt, TBuf<RMobileONStore::KOwnNumberTextSize>, TBuf<KPBStoreNumSize> > readLTsyDataComplete(location, name, telNumber);
    completeData.Close();
    readLTsyDataComplete.SerialiseL(completeData);
        
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId, KErrNone, completeData);
	
	onStore.Read(reqStatus, pckgReadEntry);	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(location, readEntry.iIndex);
	ASSERT_EQUALS(name, readEntry.iText);
	ASSERT_EQUALS(telNumber, readEntry.iNumber.iTelNumber);
	
	AssertMockLtsyStatusL();
    
	//-------------------------------------------------------------------------
	// TEST D: delete	
 	//-------------------------------------------------------------------------
	
    TMockLtsyData1<TInt> deleteLTsyData(location);
    expectData.Close();
    deleteLTsyData.SerialiseL(expectData);

    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId, KErrNone);
	
	onStore.NotifyStoreEvent(notifyStatus, notifyEvent, notifyIndex);	
	
	onStore.Delete(reqStatus, location);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);

	User::WaitForRequest(notifyStatus);
	
	TUint32 tempEventEntryDeleted = RMobilePhoneStore::KStoreEntryDeleted;
	ASSERT_EQUALS(notifyEvent, tempEventEntryDeleted); 
	ASSERT_EQUALS(notifyIndex, location);
	
	AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(5, this); // completeData, expectData, onStore, adnPhoneBookStore, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::GetInfo(), RMobileONStore::StoreAll, CRetrieveMobilePhoneONList::Start(), RMobileONStore::DeleteAll()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::GetInfo(), RMobileONStore::StoreAll, CRetrieveMobilePhoneONList::Start(), RMobileONStore::DeleteAll()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonebookOnFU::TestUseCase0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
    RMobilePhoneBookStore adnPhoneBookStore;
    CleanupClosePushL(adnPhoneBookStore);
    OpenCachingPhoneBookL(DispatcherPhonebook::EIccAdn,adnPhoneBookStore,KErrNone);
	
	RMobileONStore onStore;
	TInt ret = onStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(onStore);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);	

	TRequestStatus reqStatus;
	
	TRequestStatus notifyStatus;
	TUint32 notifyEvent;
	TInt notifyIndex;
	
	//-------------------------------------------------------------------------
	// TEST A: getinfo	
 	//-------------------------------------------------------------------------
	
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfoV1;
	TPckg<RMobileONStore::TMobileONStoreInfoV1> pckgInfoV1(onStoreInfoV1);
		
	TInt numOfEntries = 3;
	TInt usedEntries = 3;
	TInt nameLen = 25;
	TInt numLen = 20;
		
	TMockLtsyData4<TInt, TInt, TInt, TInt> onStoreInfoReturn(numOfEntries, usedEntries, nameLen, numLen);
	completeData.Close();
	onStoreInfoReturn.SerialiseL(completeData);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId, KErrNone, completeData);
	
	onStore.GetInfo(reqStatus, pckgInfoV1);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(onStoreInfoV1.iTotalEntries, numOfEntries);
	ASSERT_EQUALS(onStoreInfoV1.iUsedEntries, usedEntries);
	ASSERT_EQUALS(onStoreInfoV1.iTextLen, nameLen);
	ASSERT_EQUALS(onStoreInfoV1.iNumberLen, numLen);
	
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: storeall	
 	//-------------------------------------------------------------------------
	StoreAllL(onStore);

	//-------------------------------------------------------------------------
	// TEST C: readall	
 	//-------------------------------------------------------------------------
	
	ReadAllL(onStore);

	//-------------------------------------------------------------------------
	// TEST D: deleteall	
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);
	
	onStore.NotifyStoreEvent(notifyStatus, notifyEvent, notifyIndex);
	
	onStore.DeleteAll(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	
	User::WaitForRequest(notifyStatus);
	
	TUint32 tempEventDeleteAll = RMobilePhoneStore::KStoreEmpty;
	ASSERT_EQUALS(notifyEvent, tempEventDeleteAll); 
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // completeData, onStore, adnPhoneBookStore, this
	}

	
//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::GetInfo()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::GetInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookOnFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
    RMobilePhoneBookStore adnPhoneBookStore;
    CleanupClosePushL(adnPhoneBookStore);
    OpenCachingPhoneBookL(DispatcherPhonebook::EIccAdn,adnPhoneBookStore,KErrNone);
	
	RMobileONStore onStore;
	TInt ret = onStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(onStore);
		
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TRequestStatus reqStatus;
	
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfoV1;
	TPckg<RMobileONStore::TMobileONStoreInfoV1> pckgInfoV1(onStoreInfoV1);
	
    TInt numOfEntries = 100;
    TInt usedEntries = 10;
    TInt maxNameLen = 25;
    TInt maxNumLen = 20;
	
    TMockLtsyData4<TInt, TInt, TInt, TInt> onStoreInfoReturn(numOfEntries, usedEntries, maxNameLen, maxNumLen);
    data.Close();
    onStoreInfoReturn.SerialiseL(data);
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId, KErrNotSupported);
	
	onStore.GetInfo(reqStatus, pckgInfoV1);		
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY	
 	//-------------------------------------------------------------------------
 	
 	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId, KErrGeneral, data);
	
	onStore.GetInfo(reqStatus, pckgInfoV1);		
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());	

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileONStore::GetInfo when result is not cached.
	//-------------------------------------------------------------------------
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId, KErrNone, data);
	
	onStore.GetInfo(reqStatus, pckgInfoV1);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(onStoreInfoV1.iTotalEntries, numOfEntries);
	ASSERT_EQUALS(onStoreInfoV1.iUsedEntries, usedEntries);
	ASSERT_EQUALS(onStoreInfoV1.iTextLen, maxNameLen);
	ASSERT_EQUALS(onStoreInfoV1.iNumberLen, maxNumLen);
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileONStore::GetInfo
	// from LTSY.
	//-------------------------------------------------------------------------
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId, KErrNone, data);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // data, onStore, adnPhoneBookStore, this

	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::Write()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::Write()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookOnFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileONStore onStore;
	TInt ret = onStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(onStore);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);	
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);	
	
	TRequestStatus reqStatus;
	
	TInt location = 10;
	
	RMobileONStore::TMobileONEntryV1 writeEntry1;

    writeEntry1.iMode = RMobilePhone::ENetworkModeGsm;
    writeEntry1.iService = RMobilePhone::ETelephony;
    writeEntry1.iNumber.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
    writeEntry1.iNumber.iNumberPlan = RMobilePhone::EDataNumberPlan;
    writeEntry1.iNumber.iTelNumber.Format(KNumber,111);
    writeEntry1.iText.Format(KText,111);
    writeEntry1.iIndex = location;
    
    TPckg<RMobileONStore::TMobileONEntryV1> pckgWriteEntry1(writeEntry1);
    
    TMockLtsyData1<RMobileONStore::TMobileONEntryV1> writeLTsyData1(writeEntry1);
    expectData.Close();
    writeLTsyData1.SerialiseL(expectData);
    
    TMockLtsyData1<TInt> writeLTsyDataComplete(location);
    completeData.Close();
    writeLTsyDataComplete.SerialiseL(completeData);
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, expectData, KErrNotSupported);

	onStore.Write(reqStatus, pckgWriteEntry1);		
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY	
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, KErrGeneral, completeData);
		
	onStore.Write(reqStatus, pckgWriteEntry1);		
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileONStore::Write when result is not cached.
	//-------------------------------------------------------------------------
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, KErrNone, completeData);
	
	onStore.Write(reqStatus, pckgWriteEntry1);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(writeEntry1.iIndex, location);
	
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileONStore::Write
	// from LTSY.
	//-------------------------------------------------------------------------
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST E: additional test with index = -1
	//-------------------------------------------------------------------------
	
	RMobileONStore::TMobileONEntryV1 writeEntry2;
	TPckg<RMobileONStore::TMobileONEntryV1> pckgWriteEntry2(writeEntry2);

    writeEntry2.iMode = RMobilePhone::ENetworkModeGsm;
    writeEntry2.iService = RMobilePhone::ETelephony;
    writeEntry2.iNumber.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
    writeEntry2.iNumber.iNumberPlan = RMobilePhone::EDataNumberPlan;
    writeEntry2.iNumber.iTelNumber.Format(KNumber,123);
    writeEntry2.iText.Format(KText,123);
    writeEntry2.iIndex = -1;
        
    TMockLtsyData1<RMobileONStore::TMobileONEntryV1> writeLTsyData2(writeEntry2);
    expectData.Close();
    writeLTsyData2.SerialiseL(expectData);
	
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, expectData);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId, KErrNone, completeData);
    	
    onStore.Write(reqStatus, pckgWriteEntry2);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(reqStatus.Int(), KErrNone);
    ASSERT_EQUALS(writeEntry2.iIndex, location);
    	
    AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(4, this); // completeData, expectData, onStore, this

	}	
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::Read()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::Read()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookOnFU::TestUnit0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileONStore onStore;
	TInt ret = onStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(onStore);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);	
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);	
	
	TRequestStatus reqStatus;
	
	TInt location = 10;
	
	RMobileONStore::TMobileONEntryV1 readEntry;
	readEntry.iIndex = location;
	TPckg<RMobileONStore::TMobileONEntryV1> pckgReadEntry(readEntry);
		
	_LIT(KName, "My number");
	_LIT(KTelNumber, "1234567");
	
	TBuf<RMobileONStore::KOwnNumberTextSize> name;
	name.Copy(KName);
	TBuf<KPBStoreNumSize> telNumber;
	telNumber.Copy(KTelNumber);
		
	
    TMockLtsyData1<TInt> readLTsyData(location);
    expectData.Close();
    readLTsyData.SerialiseL(expectData);
    
    TMockLtsyData3< TInt, TBuf<RMobileONStore::KOwnNumberTextSize>, TBuf<KPBStoreNumSize> > readLTsyDataComplete(location, name, telNumber);
    completeData.Close();
    readLTsyDataComplete.SerialiseL(completeData);
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId, expectData, KErrNotSupported);

	onStore.Read(reqStatus, pckgReadEntry);		
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY	
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId, KErrGeneral, completeData);
		
	onStore.Read(reqStatus, pckgReadEntry);			
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileONStore::Read when result is not cached.
	//-------------------------------------------------------------------------
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId, KErrNone, completeData);
	
	onStore.Read(reqStatus, pckgReadEntry);	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(location, readEntry.iIndex);
	ASSERT_EQUALS(name, readEntry.iText);
	ASSERT_EQUALS(telNumber, readEntry.iNumber.iTelNumber);
	
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileONStore::Read
	// from LTSY.
	//-------------------------------------------------------------------------
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(4, this); // completeData, expectData, onStore, this

	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::Delete()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::Delete()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookOnFU::TestUnit0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileONStore onStore;
	TInt ret = onStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(onStore);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);	
	
	TRequestStatus reqStatus;
	
	TInt index = 10;
    TMockLtsyData1<TInt> deleteLTsyData(index);
    expectData.Close();
    deleteLTsyData.SerialiseL(expectData);
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId, expectData, KErrNotSupported);

	onStore.Delete(reqStatus, index);		
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY	
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId, KErrGeneral);
		
	onStore.Delete(reqStatus, index);		
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileONStore::Delete when result is not cached.
	//-------------------------------------------------------------------------
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId, KErrNone);
	
	onStore.Delete(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileONStore::Delete
	// from LTSY.
	//-------------------------------------------------------------------------
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId, KErrNone);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(3, this); // expectData, onStore, this

	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::StoreAllL()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::StoreAllL()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookOnFU::TestUnit0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileONStore onStore;
	TInt ret = onStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(onStore);
	
	RBuf8 expectEntryData1;
	CleanupClosePushL(expectEntryData1);	

	RBuf8 completeData;
	CleanupClosePushL(completeData);	
	
	TRequestStatus reqStatus;

    CMobilePhoneONList* onList = CMobilePhoneONList::NewL();
    CleanupStack::PushL(onList);
    
    GenerateONList3L(onList);
    TInt size=onList->Enumerate();
    RMobileONStore::TMobileONEntryV1 entry = onList->GetEntryL(0);
    
    TMockLtsyData1<RMobileONStore::TMobileONEntryV1> entryLTsyData1(entry);
    expectEntryData1.Close();
    entryLTsyData1.SerialiseL(expectEntryData1);
    
    TMockLtsyData1<TInt> sizeLTsyData(size);
    completeData.Close();
    sizeLTsyData.SerialiseL(completeData);
    
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId, KErrNotSupported);

	onStore.StoreAllL(reqStatus, onList);		
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	//second step test
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId, KErrNone, completeData);
	
	//write1	
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId, expectEntryData1, KErrNotSupported);

	onStore.StoreAllL(reqStatus, onList);		
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY	
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId, KErrGeneral, completeData);
		
	onStore.StoreAllL(reqStatus, onList);		
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());	
	
	//second step test
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);

	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId, KErrNone, completeData);

	//write1
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId, expectEntryData1);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId, KErrGeneral);
	
	onStore.StoreAllL(reqStatus, onList);		
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	
	AssertMockLtsyStatusL();
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileONStore::StoreAllL when result is not cached.
	//-------------------------------------------------------------------------

	StoreAllL(onStore);
	
	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileONStore::StoreAllL
	// from LTSY.
	//-------------------------------------------------------------------------
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);
	
	//second step test
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId, KErrNone);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
   
	CleanupStack::PopAndDestroy(5, this); // onList, completeData, expectEntryData1, onStore, this

	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::DeleteAll()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::DeleteAll()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookOnFU::TestUnit0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileONStore onStore;
	TInt ret = onStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(onStore);
		
	TRequestStatus reqStatus;
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNotSupported);
	
	onStore.DeleteAll(reqStatus);		
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY	
 	//-------------------------------------------------------------------------
 	
 	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrGeneral);
	
	onStore.DeleteAll(reqStatus);	
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());	

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileONStore::DeleteAll when result is not cached.
	//-------------------------------------------------------------------------
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);
	
	onStore.DeleteAll(reqStatus);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileONStore::DeleteAll
	// from LTSY.
	//-------------------------------------------------------------------------
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId, KErrNone);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // onStore, this

	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKON-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneONList::Start()
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneONList::Start()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookOnFU::TestUnit0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileONStore onStore;
	TInt err = onStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(onStore);
    
	RBuf8 expectData;
	CleanupClosePushL(expectData);	
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);	
	
	RBuf8 completeEntryData1;
	CleanupClosePushL(completeEntryData1);	

	TInt size = 3;
	TMockLtsyData1<TInt> sizeLTsyData(size);
    completeData.Close();
    sizeLTsyData.SerialiseL(completeData);

    TInt location;
    TBuf<RMobileONStore::KOwnNumberTextSize> name;
    TBuf<KPBStoreNumSize> telNumber;

    location = 1;
    name.Copy(KText1);
    telNumber.Copy(KNumber1);

	TMockLtsyData1<TInt> locationLTsyData1(location);
	expectData.Close();
	locationLTsyData1.SerialiseL(expectData);
	    
    TMockLtsyData3< TInt, TBuf<RMobileONStore::KOwnNumberTextSize>, TBuf<KPBStoreNumSize> > readLTsyDataComplete1(location, name, telNumber);
    completeEntryData1.Close();
    readLTsyDataComplete1.SerialiseL(completeEntryData1);
     
    CGetONListAO* getOnListAO = CGetONListAO::NewLC(onStore);
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId, KErrNotSupported);
    
    getOnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNotSupported, getOnListAO->RetrieveLastError());

    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId, KErrNone, completeData);
    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId, expectData, KErrNotSupported);
    
    getOnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNotSupported, getOnListAO->RetrieveLastError());
 	
	//-------------------------------------------------------------------------
	// TEST B2: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId, KErrGeneral, completeData);
    
    getOnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrGeneral, getOnListAO->RetrieveLastError());

    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId, KErrNone, completeData);
    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId, expectData);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId, KErrGeneral, completeEntryData1);
    
    getOnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrGeneral, getOnListAO->RetrieveLastError());
    CleanupStack::PopAndDestroy(1, getOnListAO);
    
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneONList::Start when result is not cached.
 	//-------------------------------------------------------------------------
    
    ReadAllL(onStore);
    
    //-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CAsyncRetrieveStoreList::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

    TRequestStatus requestStatus;
    
	iMockLTSY.NotifyTerminated(requestStatus);	
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId, KErrNone, completeEntryData1);
    
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // completeEntryData1, completeData, expectData, 
										   // onStore, this
	
	}

