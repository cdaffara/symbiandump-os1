// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The TEFUnit test suite for ENStore in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyenstorefu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "cctsyactiveretriever.h"
#include "mmretrieve.h"



const TInt KEccStorageNumberMaxCount(5);


CTestSuite* CCTsyENStoreFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestNotifyStoreEvent0001L);    //not supported
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestNotifyStoreEvent0002L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestNotifyStoreEvent0003L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestNotifyStoreEvent0004L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestNotifyStoreEvent0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestRead0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestRead0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestRead0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestRead0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestRead0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDelete0001L);  //not supported
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDelete0002L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDelete0003L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDelete0004L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDelete0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestGetInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestGetInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestGetInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestGetInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestGetInfo0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDeleteAll0001L);   // not supported
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDeleteAll0002L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDeleteAll0003L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDeleteAll0004L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestDeleteAll0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestWrite0001L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestWrite0002L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestWrite0003L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestWrite0004L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestWrite0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestStart0002L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestStart0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestStart0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyENStoreFU, TestStart0005L);
	
	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PBEN-ESNSE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::NotifyStoreEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::NotifyStoreEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestNotifyStoreEvent0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);
	

	TRequestStatus requestStatus;
	TUint32 event;
	TInt index(-1);
	
    enStore.NotifyStoreEvent( requestStatus, event, index);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // this, enStore
	
	}



/**
@SYMTestCaseID BA-CTSY-PBEN-ESR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::Read
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::Read
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestRead0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);

		
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    const TInt KIndex(1);
    _LIT(KAlpha, "Alpha");
    _LIT(KCountry, "CNTR");
    _LIT(KNetwork, "NTWRK");
    const RMobilePhone::TMobilePhoneNetworkCountryCode KCountryCode(KCountry);
	const RMobilePhone::TMobilePhoneNetworkIdentity KNwIdentity(KNetwork);
	const TInt KCallType(777); 
    

	TRequestStatus requestStatus;
    RMobileENStore::TMobileENEntryV1 info;    
	TInt readIndex(KIndex); 
	
    info.iIndex = readIndex;
    const TBuf<RMobileENStore::KEmergencyAlphaTagSize> KAlphaBuf(KAlpha);
    info.iAlphaId = KAlphaBuf;
    info.iCountryCode = KCountryCode;
    info.iIdentity = KNwIdentity;
    info.iCallType = KCallType;
    
    RMobileENStore::TMobileENEntryV1Pckg infoPckg(info);

    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

	iMockLTSY.ExpectL(EMmTsyENStoreReadIPC, expData, KErrNotSupported);
	
    enStore.Read(requestStatus, infoPckg);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	
    

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    _LIT(KEmergNum, "911");
    const TBuf<RMobileENStore::KEmergencyNumberSize> KEmergBuf(KEmergNum);
    
    TENStoreResponse ENStoreResp;
    ENStoreResp.iLocation = KIndex;
    ENStoreResp.iECCNumber = KEmergBuf;
    
    TMockLtsyData1<TENStoreResponse> tsyData2(ENStoreResp);
    tsyData2.SerialiseL(completeData);


	iMockLTSY.ExpectL(EMmTsyENStoreReadIPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreReadIPC, KErrGeneral, completeData);

    enStore.Read(requestStatus, infoPckg);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileENStore::Read when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMmTsyENStoreReadIPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreReadIPC, KErrNone, completeData);

    enStore.Read(requestStatus, infoPckg);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	ASSERT_EQUALS(KEmergBuf, info.iNumber);	  //changed
	ASSERT_EQUALS(KIndex, info.iIndex);	      // changed
	ASSERT_EQUALS(KAlphaBuf, info.iAlphaId);  // NOT  changed
	ASSERT_EQUALS(KCountryCode, info.iCountryCode); // NOT  changed 	
	ASSERT_EQUALS(KNwIdentity, info.iIdentity);	// NOT  changed
	ASSERT_EQUALS(KCallType, info.iCallType);// NOT  changed	
	ASSERT_TRUE(EFalse == info.iNetworkSpecific); // there is hard-coding of this value in CTSY
	ASSERT_TRUE(RMobilePhone::ENetworkModeUnknown == info.iMode);// there is hard-coding of this value in CTSY

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileENStore::Read
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(EMmTsyENStoreReadIPC, KErrNone, completeData);

	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileENStore::Read
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileENStore::Read
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestRead0002L()
	{

// This test should test cancellation of Read
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileENStore::Read
 	//------------------------------------------------------------------------- 	
    const TInt KIndex(1);
	TRequestStatus requestStatus;
    RMobileENStore::TMobileENEntryV1 info;    
	TInt readIndex(KIndex); 	
    info.iIndex = readIndex;    
    RMobileENStore::TMobileENEntryV1Pckg infoPckg(info);

    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

	iMockLTSY.ExpectL(EMmTsyENStoreReadIPC, expData);
	
    _LIT(KEmergNum, "911");
    const TBuf<RMobileENStore::KEmergencyNumberSize> KEmergBuf(KEmergNum);
    
    TENStoreResponse ENStoreResp;
    ENStoreResp.iLocation = KIndex;
    ENStoreResp.iECCNumber = KEmergBuf;
    
    TMockLtsyData1<TENStoreResponse> tsyData2(ENStoreResp);
    tsyData2.SerialiseL(completeData);

	iMockLTSY.CompleteL(EMmTsyENStoreReadIPC, KErrNone, completeData, 12);

    enStore.Read(requestStatus, infoPckg);
    
    enStore.CancelAsyncRequest(EMobilePhoneStoreRead);
    
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	
	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(4); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::Read with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::Read with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestRead0003L()
	{

// This test should test sending bad parameter data for Read
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);
 	
	// Test 1: Bad index
    const TInt KIndex(0);   //bad index
	TRequestStatus requestStatus;
    RMobileENStore::TMobileENEntryV1 info;    
	TInt readIndex(KIndex); 	
    info.iIndex = readIndex;    
    RMobileENStore::TMobileENEntryV1Pckg infoPckg(info);

    enStore.Read(requestStatus, infoPckg);
        
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	// Test 2: Bad package size
    RMobileNamStore::TMobileNamEntryV1 info2;    
    RMobileNamStore::TMobileNamEntryV1Pckg infoPckg2(info2);

    enStore.Read(requestStatus, infoPckg2);
        
    User::WaitForRequest(requestStatus);        
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // enStore, this
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileENStore::Read
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileENStore::Read
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestRead0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RMobileENStore  enStore;
    ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);

    RMobileENStore  enStore2;
    ret = enStore2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore2);

 	
    const TInt KIndex(1);
    _LIT(KAlpha, "Alpha");
    _LIT(KCountry, "CNTR");
    _LIT(KNetwork, "NTWRK");
    const RMobilePhone::TMobilePhoneNetworkCountryCode KCountryCode(KCountry);
	const RMobilePhone::TMobilePhoneNetworkIdentity KNwIdentity(KNetwork);
	const TInt KCallType(777); 
    
    RMobileENStore::TMobileENEntryV1 info;    
	TInt readIndex(KIndex); 
	
    info.iIndex = readIndex;
    const TBuf<RMobileENStore::KEmergencyAlphaTagSize> KAlphaBuf(KAlpha);
    info.iAlphaId = KAlphaBuf;
    info.iCountryCode = KCountryCode;
    info.iIdentity = KNwIdentity;
    info.iCallType = KCallType;
    
    RMobileENStore::TMobileENEntryV1Pckg infoPckg(info);

    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);
	
    _LIT(KEmergNum, "911");
    const TBuf<RMobileENStore::KEmergencyNumberSize> KEmergBuf(KEmergNum);
    
    TENStoreResponse ENStoreResp;
    ENStoreResp.iLocation = KIndex;
    ENStoreResp.iECCNumber = KEmergBuf;
    
    TMockLtsyData1<TENStoreResponse> tsyData2(ENStoreResp);
    tsyData2.SerialiseL(completeData);


	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	iMockLTSY.ExpectL(EMmTsyENStoreReadIPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreReadIPC, KErrNone, completeData, 12);

    enStore.Read(requestStatus, infoPckg);

	iMockLTSY.ExpectL(EMmTsyENStoreReadIPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreReadIPC, KErrNone, completeData, 12);

    RMobileENStore::TMobileENEntryV1 info2(info);    
    RMobileENStore::TMobileENEntryV1Pckg infoPckg2(info2);
    
    enStore2.Read(requestStatus2, infoPckg2);
        
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

    User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	

	
	ASSERT_EQUALS(KEmergBuf, info.iNumber);	  //changed
	ASSERT_EQUALS(KIndex, info.iIndex);	      // changed
	ASSERT_EQUALS(KAlphaBuf, info.iAlphaId);  // NOT  changed
	ASSERT_EQUALS(KCountryCode, info.iCountryCode); // NOT  changed 	
	ASSERT_EQUALS(KNwIdentity, info.iIdentity);	// NOT  changed
	ASSERT_EQUALS(KCallType, info.iCallType);// NOT  changed	
	ASSERT_TRUE(EFalse == info.iNetworkSpecific); // there is hard-coding of this value in CTSY
	ASSERT_TRUE(RMobilePhone::ENetworkModeUnknown == info.iMode);// there is hard-coding of this value in CTSY

	ASSERT_EQUALS(KEmergBuf, info2.iNumber);	  //changed
	ASSERT_EQUALS(KIndex, info2.iIndex);	      // changed
	ASSERT_EQUALS(KAlphaBuf, info2.iAlphaId);  // NOT  changed
	ASSERT_EQUALS(KCountryCode, info2.iCountryCode); // NOT  changed 	
	ASSERT_EQUALS(KNwIdentity, info2.iIdentity);	// NOT  changed
	ASSERT_EQUALS(KCallType, info2.iCallType);// NOT  changed	
	ASSERT_TRUE(EFalse == info2.iNetworkSpecific); // there is hard-coding of this value in CTSY
	ASSERT_TRUE(RMobilePhone::ENetworkModeUnknown == info2.iMode);// there is hard-coding of this value in CTSY

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::Read with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::Read and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestRead0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);

    const TInt KIndex(1);   
	TRequestStatus requestStatus;
    RMobileENStore::TMobileENEntryV1 info;    
	TInt readIndex(KIndex); 	
    info.iIndex = readIndex;    
    RMobileENStore::TMobileENEntryV1Pckg infoPckg(info);

    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

	iMockLTSY.ExpectL(EMmTsyENStoreReadIPC, expData);
	
    enStore.Read(requestStatus, infoPckg);
        
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);	
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::Delete
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestDelete0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);
		
		
	TRequestStatus requestStatus;
	TInt index(-1);
    enStore.Delete(requestStatus, index);		
    
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESGI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::GetInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::GetInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestGetInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);
		
		
	TRequestStatus requestStatus;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg storePckg(storeInfo);

    TInt readIndex(-1);
    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMmTsyENStoreGetInfoIPC, expData, KErrNotSupported);
	
	enStore.GetInfo(requestStatus, storePckg);		
    
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
 	TInt usedEntries(0);
    TMockLtsyData1<TInt> tsyData2(usedEntries);           
    tsyData2.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMmTsyENStoreGetInfoIPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreGetInfoIPC, KErrGeneral, completeData);
	
	enStore.GetInfo(requestStatus, storePckg);		
    
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileENStore::GetInfo when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMmTsyENStoreGetInfoIPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreGetInfoIPC, KErrNone, completeData);
	
	enStore.GetInfo(requestStatus, storePckg);		
    
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
    ASSERT_EQUALS(RMobileENStore::EEmergencyNumberStore, storeInfo.iType);
    ASSERT_TRUE(usedEntries == storeInfo.iUsedEntries);
    ASSERT_EQUALS(KEccStorageNumberMaxCount, storeInfo.iTotalEntries);
    ASSERT_EQUALS(KENStoreCaps, storeInfo.iCaps);
    ASSERT_TRUE(0 == storeInfo.iName.Compare(KETelEmergencyNumberStore));
		
 	//-------------------------------------------------------------------------
	// TEST D: RMobileENStore::GetInfo again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
    // changing some data
    usedEntries = 10;
    TMockLtsyData1<TInt> tsyData3(usedEntries);           
    completeData.Close();
    tsyData3.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMmTsyENStoreGetInfoIPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreGetInfoIPC, KErrNone, completeData);

	enStore.GetInfo(requestStatus, storePckg);		
    
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

    ASSERT_EQUALS(RMobileENStore::EEmergencyNumberStore, storeInfo.iType);
    ASSERT_TRUE(usedEntries == storeInfo.iUsedEntries);
    ASSERT_EQUALS(KEccStorageNumberMaxCount, storeInfo.iTotalEntries);
    ASSERT_EQUALS(KENStoreCaps, storeInfo.iCaps);
    ASSERT_TRUE(0 == storeInfo.iName.Compare(KETelEmergencyNumberStore));
    
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileENStore::GetInfo
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.CompleteL(EMmTsyENStoreGetInfoIPC, KErrNone, completeData);

	User::WaitForRequest(mockLtsyStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // this, etc...
	
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESGI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileENStore::GetInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileENStore::GetInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestGetInfo0002L()
	{

// This test should test cancellation of GetInfo
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileENStore::GetInfo
 	//------------------------------------------------------------------------- 	
	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);
		
		
	TRequestStatus requestStatus;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg storePckg(storeInfo);

    TInt readIndex(-1);
    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);
	
	TInt usedEntries(0);
    TMockLtsyData1<TInt> tsyData2(usedEntries);           
    tsyData2.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMmTsyENStoreGetInfoIPC, expData); 
	iMockLTSY.CompleteL(EMmTsyENStoreGetInfoIPC, KErrNone, completeData);
	
	enStore.GetInfo(requestStatus, storePckg);		
	
	enStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
    
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	

	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // data, etc...
	}

/**
@SYMTestCaseID BA-CTSY-PBEN-ESGI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Bad Params test for RMobileENStore::GetInfo
@SYMTestPriority High
@SYMTestActions Invokes bad params test for RMobileENStore::GetInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestGetInfo0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);
	
	TRequestStatus requestStatus;
	TInt8 storeInfo;    // bad param
	TPckg<TInt8> storePckg(storeInfo);

	enStore.GetInfo(requestStatus, storePckg);		
	
    User::WaitForRequest(requestStatus);        
	ASSERT_TRUE(KErrArgument == requestStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // enStore, completeData, expData, this
	}



/**
@SYMTestCaseID BA-CTSY-PBEN-ESGI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileENStore::GetInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileENStore::GetInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestGetInfo0004L()
	{
				
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RMobileENStore  enStore;
    ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);

    RMobileENStore  enStore2;
    ret = enStore2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore2);
		
		
	TRequestStatus requestStatus;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg storePckg(storeInfo);

	TRequestStatus requestStatus2;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo2;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg storePckg2(storeInfo2);

    TInt readIndex(-1);
    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

 	TInt usedEntries(0);
    TMockLtsyData1<TInt> tsyData2(usedEntries);           
    tsyData2.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMmTsyENStoreGetInfoIPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreGetInfoIPC, KErrNone, completeData);
	
	enStore.GetInfo(requestStatus, storePckg);		

	iMockLTSY.ExpectL(EMmTsyENStoreGetInfoIPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreGetInfoIPC, KErrNone, completeData);
	
	enStore2.GetInfo(requestStatus2, storePckg2);		
	    
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

    ASSERT_EQUALS(RMobileENStore::EEmergencyNumberStore, storeInfo.iType);
    ASSERT_TRUE(usedEntries == storeInfo.iUsedEntries);
    ASSERT_EQUALS(KEccStorageNumberMaxCount, storeInfo.iTotalEntries);
    ASSERT_EQUALS(KENStoreCaps, storeInfo.iCaps);
    ASSERT_TRUE(0 == storeInfo.iName.Compare(KETelEmergencyNumberStore));

    User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	

    ASSERT_EQUALS(RMobileENStore::EEmergencyNumberStore, storeInfo2.iType);
    ASSERT_TRUE(usedEntries == storeInfo2.iUsedEntries);
    ASSERT_EQUALS(KEccStorageNumberMaxCount, storeInfo2.iTotalEntries);
    ASSERT_EQUALS(KENStoreCaps, storeInfo2.iCaps);
    ASSERT_TRUE(0 == storeInfo2.iName.Compare(KETelEmergencyNumberStore));


	CleanupStack::PopAndDestroy(7, this); // this, etc...
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESGI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::GetInfo with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::GetInfo and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestGetInfo0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);
			
	TRequestStatus requestStatus;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg storePckg(storeInfo);

    TInt readIndex(-1);
    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

	iMockLTSY.ExpectL(EMmTsyENStoreGetInfoIPC, expData);

	enStore.GetInfo(requestStatus, storePckg);			    
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(3, this); // enStore, expData, this 
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESDA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::DeleteAll
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::DeleteAll
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestDeleteAll0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();


    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);
	
	
	TRequestStatus requestStatus;
	
	enStore.DeleteAll(requestStatus);
	
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // this, etc...
	
	}



/**
@SYMTestCaseID BA-CTSY-PBEN-ESW-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::Write
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::Write
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestWrite0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);
	
	
	TRequestStatus requestStatus;
    RMobileENStore::TMobileENEntryV1 entry;
    RMobileENStore::TMobileENEntryV1Pckg entryPckg(entry);
	
	enStore.Write(requestStatus, entryPckg);
	
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // this, etc...

	}




/**
@SYMTestCaseID BA-CTSY-PBEN-ESS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneENList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneENList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);


	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneENList* enList = CRetrieveMobilePhoneENList::NewL(enStore);    
	CleanupStack::PushL(enList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*enList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    TInt readIndex(-1);
    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

	iMockLTSY.ExpectL(EMmTsyENStoreReadAllPhase1IPC, expData, KErrNotSupported);

    enList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMmTsyENStoreReadAllPhase1IPC, expData);
	
	CArrayPtrFlat<TENStoreResponse>* arrayPtr = new ( ELeave ) CArrayPtrFlat<TENStoreResponse>(1);
	CleanupStack::PushL(arrayPtr);
	TMockLtsyData1<CArrayPtrFlat<TENStoreResponse>*> tsyData2(arrayPtr);           
	tsyData2.SerialiseL(completeData);
	
	iMockLTSY.CompleteL(EMmTsyENStoreReadAllPhase1IPC, KErrGeneral, completeData);

    enList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrGeneral, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneENList::Start when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMmTsyENStoreReadAllPhase1IPC, expData);	
	iMockLTSY.CompleteL(EMmTsyENStoreReadAllPhase1IPC, KErrNone, completeData);

    enList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();


	CMobilePhoneENList* list = enList->RetrieveListL();
	
	CMobilePhoneENList* listToCompare = CMobilePhoneENList::NewL();
	CleanupStack::PushL(listToCompare);

	ASSERT_TRUE(list->Enumerate() == listToCompare->Enumerate());
	ASSERT_TRUE(list->StoreLC()->Ptr(0) == listToCompare->StoreLC()->Ptr(0));
    	
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CRetrieveMobilePhoneENList::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

	CArrayPtrFlat<TSmsMsg>* smsMsgArray = new ( ELeave ) CArrayPtrFlat<TSmsMsg>( 1 );
	CleanupStack::PushL(smsMsgArray);

	TSmsMsg smsMsg;
    smsMsg.iSmsMsg = _L8("Hello, World!"); 
    smsMsg.iServiceCentre = _L("ServiceSenter"); 
    smsMsg.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
    smsMsg.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
    smsMsg.iMessageStatus = KSimSmsMtRead; // RMobileSmsStore::EStoredMessageRead
    smsMsg.iLocation      = 200;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsClass2            = ETrue;
	smsMsgArray->AppendL(&smsMsg, 1);
    
	TBool receivedClass2ToBeReSent(ETrue);
	TMockLtsyData2<CArrayPtrFlat< TSmsMsg >*, TBool> 
		completeSmsMsgLtsyData(smsMsgArray, receivedClass2ToBeReSent);

	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);
	completeData.Close();
	completeSmsMsgLtsyData.SerialiseL(completeData);
    iMockLTSY.CompleteL(EMobilePhoneStoreReadAllPhase1, KErrNone, completeData);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();



 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// CRetrieveMobilePhoneENList::Start when result is not cached.
 	//-------------------------------------------------------------------------

   _LIT(KEmergNum, "911");
   	TENStoreResponse response;
    response.iLocation = 0;
    response.iECCNumber = KEmergNum;
    arrayPtr->AppendL(&response);

	iMockLTSY.ExpectL(EMmTsyENStoreReadAllPhase1IPC, expData);	
	
	completeData.Close();
	tsyData2.SerialiseL(completeData);
	
	iMockLTSY.CompleteL(EMmTsyENStoreReadAllPhase1IPC, KErrNone, completeData);

    enList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	RMobileENStore::TMobileENEntryV1 entry = enList->RetrieveListL()->GetEntryL(0);
	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode;
	RMobilePhone::TMobilePhoneNetworkIdentity identity;
	TBuf<RMobileENStore::KEmergencyAlphaTagSize> alphaId;

	ASSERT_TRUE  (entry.iNetworkSpecific);
	ASSERT_EQUALS(RMobilePhone::ENetworkModeGsm, entry.iMode);
	ASSERT_EQUALS(0, entry.iCountryCode.Compare(countryCode));
	ASSERT_EQUALS(0, entry.iIdentity.Compare(identity));
	ASSERT_EQUALS(0, entry.iNumber.Compare(response.iECCNumber));
	ASSERT_EQUALS(0, entry.iAlphaId.Compare(alphaId));
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // smsMsgArray
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(10, this); // expData, completeData, enStore, enList
	                                       // activeRetriever, arrayPtr, listToCompare
	
	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrieveMobilePhoneENList::Start
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrieveMobilePhoneENList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestStart0002L()
	{

// This test should test cancellation of Start
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneENList* enList = CRetrieveMobilePhoneENList::NewL(enStore);    
	CleanupStack::PushL(enList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*enList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

    TInt readIndex(-1);
    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

	CArrayPtrFlat<TENStoreResponse>* arrayPtr = new ( ELeave ) CArrayPtrFlat<TENStoreResponse>(1);
	CleanupStack::PushL(arrayPtr);
	TMockLtsyData1<CArrayPtrFlat<TENStoreResponse>*> tsyData2(arrayPtr);           
	tsyData2.SerialiseL(completeData);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneENList::Start
 	//-------------------------------------------------------------------------
 	
    iMockLTSY.ExpectL(EMmTsyENStoreReadAllPhase1IPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreReadAllPhase1IPC, KErrNone, completeData, 10);


	enList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	enList->Cancel();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->Status().Int())
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneENList::Start
	// this time cancel on 2nd phase
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMmTsyENStoreReadAllPhase1IPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreReadAllPhase1IPC, KErrNone, completeData);

	enList->Start(activeRetriever->iStatus);
	activeRetriever->Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());

	AssertMockLtsyStatusL();

	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(7, this); 
	
	}




/**
@SYMTestCaseID BA-CTSY-PBEN-ESS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to CRetrieveMobilePhoneENList::Start
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to CRetrieveMobilePhoneENList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestStart0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);


	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
 	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    RMobileENStore  enStore;
    ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);

    RMobileENStore  enStore2;
    ret = enStore2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore2);

	CRetrieveMobilePhoneENList* enList = CRetrieveMobilePhoneENList::NewL(enStore);    
	CleanupStack::PushL(enList);


	CRetrieveMobilePhoneENList* enList2 = CRetrieveMobilePhoneENList::NewL(enStore2);   
	CleanupStack::PushL(enList2);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*enList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	CActiveRetriever* activeRetriever2 = CActiveRetriever::NewL(*enList2);
	CleanupStack::PushL(activeRetriever2);
	scheduler.AddRetrieverL(*activeRetriever2);
 	
	//-------------------------------------------------------------------------
	// Test multiple clients requesting CRetrieveMobilePhoneENList::Start
 	//-------------------------------------------------------------------------

    // setting and execute 1st request
    TInt readIndex(-1);
    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

   _LIT(KEmergNum, "911");
	CArrayPtrFlat<TENStoreResponse>* arrayPtr = new ( ELeave ) CArrayPtrFlat<TENStoreResponse>(1);
	CleanupStack::PushL(arrayPtr);

   	TENStoreResponse response;
    response.iLocation = 0;
    response.iECCNumber = KEmergNum;    
    arrayPtr->AppendL(&response);

	TMockLtsyData1<CArrayPtrFlat<TENStoreResponse>*> tsyData2(arrayPtr);           
	tsyData2.SerialiseL(completeData);
	
    iMockLTSY.ExpectL(EMmTsyENStoreReadAllPhase1IPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreReadAllPhase1IPC, KErrNone, completeData);

	enList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();



	
   _LIT(KEmergNum2, "112");
	CArrayPtrFlat<TENStoreResponse>* arrayPtr2 = new ( ELeave ) CArrayPtrFlat<TENStoreResponse>(1);
	CleanupStack::PushL(arrayPtr2);

   	TENStoreResponse response2;
    response2.iLocation = 0;
    response2.iECCNumber = KEmergNum2;    
    arrayPtr2->AppendL(&response2);

	TMockLtsyData1<CArrayPtrFlat<TENStoreResponse>*> tsyData3(arrayPtr2);           
	completeData.Close();
	tsyData3.SerialiseL(completeData);

    iMockLTSY.ExpectL(EMmTsyENStoreReadAllPhase1IPC, expData);
	iMockLTSY.CompleteL(EMmTsyENStoreReadAllPhase1IPC, KErrNone, completeData);

    // setting and execute 2nd request
	enList2->Start(activeRetriever2->iStatus);
	activeRetriever2->Activate();

	// invoke scheduler
	scheduler.StartScheduler();

	// check results
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	ASSERT_EQUALS(KErrNone, activeRetriever2->iStatus.Int());
	
	//cause defect prevents from data transfer
	CMobilePhoneENList* list = enList->RetrieveListL();

	RMobileENStore::TMobileENEntryV1 entry1 = list->GetEntryL(0);
	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode;
	RMobilePhone::TMobilePhoneNetworkIdentity identity;
	TBuf<RMobileENStore::KEmergencyAlphaTagSize> alphaId;

	ASSERT_TRUE  (entry1.iNetworkSpecific);
	ASSERT_EQUALS(RMobilePhone::ENetworkModeGsm, entry1.iMode);
	ASSERT_EQUALS(0, entry1.iCountryCode.Compare(countryCode));
	ASSERT_EQUALS(0, entry1.iIdentity.Compare(identity));
	ASSERT_EQUALS(0, entry1.iNumber.Compare(response.iECCNumber));
	ASSERT_EQUALS(0, entry1.iAlphaId.Compare(alphaId));
		
	CMobilePhoneENList* list2 = enList2->RetrieveListL();
	ASSERT_TRUE( 1 == list2->Enumerate() );

	RMobileENStore::TMobileENEntryV1 entry2 = list2->GetEntryL(0);
	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode2;
	RMobilePhone::TMobilePhoneNetworkIdentity identity2;
	TBuf<RMobileENStore::KEmergencyAlphaTagSize> alphaId2;

	ASSERT_TRUE  (entry2.iNetworkSpecific);
	ASSERT_EQUALS(RMobilePhone::ENetworkModeGsm, entry2.iMode);
	ASSERT_EQUALS(0, entry2.iCountryCode.Compare(countryCode2));
	ASSERT_EQUALS(0, entry2.iIdentity.Compare(identity2));
	ASSERT_EQUALS(0, entry2.iNumber.Compare(response2.iECCNumber));
	ASSERT_EQUALS(0, entry2.iAlphaId.Compare(alphaId2));

	AssertMockLtsyStatusL();

	// Done !
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(13, this); 

	}


/**
@SYMTestCaseID BA-CTSY-PBEN-ESS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneENList::Start with timeout
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneENList::Start and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyENStoreFU::TestStart0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 expData;
	CleanupClosePushL(expData);

    RMobileENStore  enStore;
    TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(enStore);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneENList* enList = CRetrieveMobilePhoneENList::NewL(enStore);    
	CleanupStack::PushL(enList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*enList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

    TInt readIndex(-1);
    TMockLtsyData1<TInt> tsyData(readIndex);           
    tsyData.SerialiseL(expData);

    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 150703);

    iMockLTSY.ExpectL(EMmTsyENStoreReadAllPhase1IPC, expData);

	enList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrArgument, activeRetriever->Status().Int())
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	AssertMockLtsyStatusL();
	

	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(6); 

	}







