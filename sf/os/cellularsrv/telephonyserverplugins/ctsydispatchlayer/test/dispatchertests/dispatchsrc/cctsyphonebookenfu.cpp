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

#include "cctsyphonebookenfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchphonebookeninterface.h>
#include "listretrieverao.h"


CTestSuite* CCTsyPhonebookEnFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
  
	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookEnFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookEnFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookEnFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookEnFU, TestUnit0003BL);

	END_SUITE;
	}


//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKEN-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::GetInfo()
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::GetInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookEnFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileENStore enStore;
	TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(enStore);
		
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TRequestStatus reqStatus;
	
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 enStoreInfoV1;
	TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1> pckgInfoV1(enStoreInfoV1);
	
    TInt usedEntries = 10;
 	
    TMockLtsyData1<TInt> enStoreInfoReturn(usedEntries);
    data.Close();
    enStoreInfoReturn.SerialiseL(data);
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId, KErrNotSupported);
	
	enStore.GetInfo(reqStatus, pckgInfoV1);		
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY	
 	//-------------------------------------------------------------------------
 	
 	iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId, KErrGeneral, data);
	
	enStore.GetInfo(reqStatus, pckgInfoV1);		
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());	

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileENStore::GetInfo() when result is not cached.
	//-------------------------------------------------------------------------
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId, KErrNone, data);
	
	enStore.GetInfo(reqStatus, pckgInfoV1);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(enStoreInfoV1.iUsedEntries, usedEntries);
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileENStore::GetInfo()
	// from LTSY.
	//-------------------------------------------------------------------------
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId, KErrNone, data);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); // data, enStore, this

	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKEN-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::Read()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::Read()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookEnFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileENStore enStore;
	TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(enStore);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);	
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);	
	
	TRequestStatus reqStatus;
	
	TInt index = 10;
	
	RMobileENStore::TMobileENEntryV1 readEntry;
	readEntry.iIndex = index;
	TPckg<RMobileENStore::TMobileENEntryV1> pckgReadEntry(readEntry);

	_LIT(KTelNumber, "123456");
	TBuf<RMobileENStore::KEmergencyNumberSize> telNumber;
	telNumber.Copy(KTelNumber);
	
    TMockLtsyData1<TInt> readLTsyData(index);
    expectData.Close();
    readLTsyData.SerialiseL(expectData);
    
    TMockLtsyData2< TInt, TBuf<RMobileENStore::KEmergencyNumberSize> > readLTsyDataComplete(index, telNumber);
    completeData.Close();
    readLTsyDataComplete.SerialiseL(completeData);
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId, expectData, KErrNotSupported);

	enStore.Read(reqStatus, pckgReadEntry);		
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY	
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId, KErrGeneral, completeData);
		
	enStore.Read(reqStatus, pckgReadEntry);			
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileENStore::Read() when result is not cached.
	//-------------------------------------------------------------------------
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId, KErrNone, completeData);
	
	enStore.Read(reqStatus, pckgReadEntry);	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(index, readEntry.iIndex);
	ASSERT_EQUALS(telNumber, readEntry.iNumber);
	
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST D: Unsolicited completion of RMobileENStore::Read()
	// from LTSY.
	//-------------------------------------------------------------------------
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST E: Parameters out of bound test 
	//-------------------------------------------------------------------------
	index = 300;
    TMockLtsyData2< TInt, TBuf<RMobileENStore::KEmergencyNumberSize> > readLTsyDataComplete2(index, telNumber);
    completeData.Close();
    readLTsyDataComplete2.SerialiseL(completeData);
	
    
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId, expectData);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId, KErrNone, completeData);
	
    enStore.Read(reqStatus, pckgReadEntry);			
        
    User::WaitForRequest(reqStatus);        
    ASSERT_EQUALS(KErrCorrupt, reqStatus.Int());	
    	
	
	AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(4, this); // completeData, expectData, onStore, this

	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKEN-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneENList::Start()
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneENList::Start()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookEnFU::TestUnit0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileENStore enStore;
	TInt err = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(enStore);

	CGetENListAO* getEnListAO = CGetENListAO::NewLC(enStore);

	RBuf8 completeEntryData1;
	CleanupClosePushL(completeEntryData1);	
	
	RBuf8 completeEntryData2;
	CleanupClosePushL(completeEntryData2);	
	
	RBuf8 completeEntryData3;
	CleanupClosePushL(completeEntryData3);

    TInt index[3];
    TBuf<RMobileENStore::KEmergencyNumberSize> telNumber[3];
    
    TBool moreToCome = ETrue;
    index[0] = 1;
	_LIT(KNumber1, "000001");
    telNumber[0].Copy(KNumber1);
	    
    TMockLtsyData3< TInt, TBuf<RMobileENStore::KEmergencyNumberSize>, TBool > readLTsyDataComplete1(index[0], telNumber[0], moreToCome);
    completeEntryData1.Close();
    readLTsyDataComplete1.SerialiseL(completeEntryData1);
    
	
    index[1] = 2;
	_LIT(KNumber2, "111112");
    telNumber[1].Copy(KNumber2);
    
    TMockLtsyData3< TInt, TBuf<RMobileENStore::KEmergencyNumberSize>, TBool > readLTsyDataComplete2(index[1], telNumber[1], moreToCome);
    completeEntryData2.Close();
    readLTsyDataComplete2.SerialiseL(completeEntryData2);
	
    moreToCome = EFalse;
    index[2] = 3;
	_LIT(KNumber3, "111113");
    telNumber[2].Copy(KNumber3);
    
    TMockLtsyData3< TInt, TBuf<RMobileENStore::KEmergencyNumberSize>, TBool > readLTsyDataComplete3(index[2], telNumber[2], moreToCome);
    completeEntryData3.Close();
    readLTsyDataComplete3.SerialiseL(completeEntryData3);
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNotSupported);
    
    getEnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNotSupported, getEnListAO->RetrieveLastError());
 	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
 
	// failure on the 1st entry 
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrGeneral, completeEntryData1);
    
    getEnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrGeneral, getEnListAO->RetrieveLastError());

	// failure on the middle entry
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData1);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrGeneral, completeEntryData2);
    
    getEnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrGeneral, getEnListAO->RetrieveLastError());
	
	// failure on the last entry
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData1);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData2);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrGeneral, completeEntryData3);
    
    getEnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrGeneral, getEnListAO->RetrieveLastError());
    
	// parameter out of bound test 
	RBuf8 completeEntryData4;
	CleanupClosePushL(completeEntryData4);
	TInt indexOutOfBound = 300;
	TMockLtsyData3< TInt, TBuf<RMobileENStore::KEmergencyNumberSize>, TBool > readLTsyDataComplete4(indexOutOfBound, telNumber[2], moreToCome);
	completeEntryData4.Close();
	readLTsyDataComplete4.SerialiseL(completeEntryData4);
	
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData1);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData4);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData3);
    
    getEnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrCorrupt, getEnListAO->RetrieveLastError());
	
	AssertMockLtsyStatusL();
 	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneENList::Start() when result is not cached.
 	//-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData1);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData2);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData3);

    getEnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNone, getEnListAO->RetrieveLastError());

	CMobilePhoneENList* list = getEnListAO->RetrieveListL();
	RMobileENStore::TMobileENEntryV1 readEntry;
	
	for (TInt i=0; i< list->Enumerate(); i++)
		{
		readEntry=list->GetEntryL(i);
		ASSERT_EQUALS(index[i], readEntry.iIndex);
		ASSERT_EQUALS(telNumber[i], readEntry.iNumber);
		}

	AssertMockLtsyStatusL();


 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CAsyncRetrieveStoreList::Start()
	// from LTSY.
 	//-------------------------------------------------------------------------

    TRequestStatus requestStatus;
    
	iMockLTSY.NotifyTerminated(requestStatus);	
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData1);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData2);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNone, completeEntryData3);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	
	CActiveScheduler::Install(NULL);

	
	CleanupStack::PopAndDestroy(7, this); // completeEntryData4, completeEntryData3, completeEntryData2, completeEntryData1, getEnListAO,
										   // enStore, this
	
	}	

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKEN-UN0003B
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneENList::Start() when the storage is empty
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneENList::Start() when the storage is empty
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookEnFU::TestUnit0003BL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileENStore enStore;
	TInt err = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(enStore);
	
	CGetENListAO* getEnListAO = CGetENListAO::NewLC(enStore);
	
	RBuf8 completeEntryData;
	CleanupClosePushL(completeEntryData);	

	_LIT(KNumber, "");
    TInt index = 0;
    TBuf<RMobileENStore::KEmergencyNumberSize> telNumber;
    telNumber.Copy(KNumber);
    
    TBool moreToCome = EFalse;
	    
    TMockLtsyData3< TInt, TBuf<RMobileENStore::KEmergencyNumberSize>, TBool > readLTsyDataComplete(index, telNumber, moreToCome);
    completeEntryData.Close();
    readLTsyDataComplete.SerialiseL(completeEntryData);
    
    iMockLTSY.ExpectL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId);
    iMockLTSY.CompleteL(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, KErrNotFound, completeEntryData);
    
    getEnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNotFound, getEnListAO->RetrieveLastError());
    
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // completeEntryData,  getEnListAO,
										   // enStore, this
	
	}	
