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
// The TEFUnit test suite for BroadcastMessaging in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsybroadcastmessagingfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <mmretrieve.h>
#include "cctsyactiveretriever.h"
#include <ctsy/serviceapi/cmmsmsutility.h>

CTestSuite* CCTsyBroadcastMessagingFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestStoreBroadcastIdListL0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestStoreBroadcastIdListL0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestStoreBroadcastIdListL0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestStoreBroadcastIdListL0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestStoreBroadcastIdListL0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestStart0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestStart0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestStart0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestReceiveMessage0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestReceiveMessage0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestReceiveMessage0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestReceiveMessage0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestReceiveMessage0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestGetFilterSetting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestSetFilterSetting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestSetFilterSetting0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestSetFilterSetting0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestSetFilterSetting0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestSetFilterSetting0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestGetLanguageFilter0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestSetLanguageFilter0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestNotifyFilterSettingChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestNotifyFilterSettingChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestNotifyFilterSettingChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestNotifyFilterSettingChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestNotifyLanguageFilterChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestNotifyBroadcastIdListChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestGetCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyBroadcastMessagingFU, TestGetCaps0003L);

	END_SUITE;
	}


void CCTsyBroadcastMessagingFU::FillInBroadcastIdListL(CMobilePhoneBroadcastIdList& aList)
	{
	
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 entry;
	
	entry.iId = 7;
	aList.AddEntryL(entry);
	
	entry.iId = 17;
	aList.AddEntryL(entry);
	
	entry.iId = 37;
	aList.AddEntryL(entry);
	
	}

//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSBIL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::StoreBroadcastIdListL
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::StoreBroadcastIdListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestStoreBroadcastIdListL0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TRequestStatus reqStatus;

	CMobilePhoneBroadcastIdList* idList = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(idList);

	RMobileBroadcastMessaging::TMobileBroadcastIdType type
	                              = RMobileBroadcastMessaging::EGsmBroadcastId;
	
	TMockLtsyData2<CMobilePhoneBroadcastIdList*,
	               RMobileBroadcastMessaging::TMobileBroadcastIdType>
	                                             expectLtsyData(idList, type);
	FillInBroadcastIdListL(*idList);
	expectLtsyData.SerialiseL(data); 

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    messaging.StoreBroadcastIdListL(reqStatus, idList, type);

	User::WaitForRequest(reqStatus);
	
	AssertMockLtsyStatusL();

	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());


	iMockLTSY.NotifyTerminated(reqStatus);
	iMockLTSY.CompleteL(EMobileBroadcastMessagingStoreIdList, KErrNone);
	User::WaitForRequest(reqStatus);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // idList, messaging, data, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSBIL-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileBroadcastMessaging::StoreBroadcastIdListL
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileBroadcastMessaging::StoreBroadcastIdListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestStoreBroadcastIdListL0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TRequestStatus reqStatus;

	CMobilePhoneBroadcastIdList* idList = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(idList);

	RMobileBroadcastMessaging::TMobileBroadcastIdType type
	                              = RMobileBroadcastMessaging::EGsmBroadcastId;
	
	TMockLtsyData2<CMobilePhoneBroadcastIdList*,
	               RMobileBroadcastMessaging::TMobileBroadcastIdType>
	                                             expectLtsyData(idList, type);
	FillInBroadcastIdListL(*idList);
	expectLtsyData.SerialiseL(data); 

	//-------------------------------------------------------------------------
	// Test cancelling of RMobileBroadcastMessaging::StoreBroadcastIdListL
 	//-------------------------------------------------------------------------


    messaging.StoreBroadcastIdListL(reqStatus, idList, type);

	messaging.CancelAsyncRequest(EMobileBroadcastMessagingStoreIdList);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // idList, messaging, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSBIL-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::StoreBroadcastIdListL with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::StoreBroadcastIdListL with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestStoreBroadcastIdListL0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobileBroadcastMessaging::StoreBroadcastIdListL
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	CMobilePhoneBroadcastIdList* idList(NULL);
	messaging.StoreBroadcastIdListL(reqStatus, idList, RMobileBroadcastMessaging::EGsmBroadcastId);
	
	// should never get here because of etel panic
	// so if it gets test fails with the assert below
	ASSERT_TRUE(EFalse);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // messaging, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSBIL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileBroadcastMessaging::StoreBroadcastIdListL
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileBroadcastMessaging::StoreBroadcastIdListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestStoreBroadcastIdListL0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TRequestStatus reqStatus;
	TRequestStatus reqStatus2;

	CMobilePhoneBroadcastIdList* idList = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(idList);

	CMobilePhoneBroadcastIdList* idList2 = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(idList2);

	RMobileBroadcastMessaging::TMobileBroadcastIdType type
	                              = RMobileBroadcastMessaging::EGsmBroadcastId;
	
	TMockLtsyData2<CMobilePhoneBroadcastIdList*,
	               RMobileBroadcastMessaging::TMobileBroadcastIdType>
	                                             expectLtsyData(idList, type);
	FillInBroadcastIdListL(*idList);
	expectLtsyData.SerialiseL(data); 

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileBroadcastMessaging messaging2;
	err = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileBroadcastMessaging::StoreBroadcastIdListL
 	//-------------------------------------------------------------------------

    messaging.StoreBroadcastIdListL(reqStatus, idList, RMobileBroadcastMessaging::EGsmBroadcastId);
	messaging2.StoreBroadcastIdListL(reqStatus2, idList2, RMobileBroadcastMessaging::EGsmBroadcastId);

	User::WaitForRequest(reqStatus);
	User::WaitForRequest(reqStatus2);

	AssertMockLtsyStatusL();

	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	ASSERT_EQUALS(KErrNotSupported, reqStatus2.Int());

	CleanupStack::PopAndDestroy(8, this); // messaging2, phone2, telServer2, idList2, idList, messaging, data, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BRM-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::StoreBroadcastIdListL with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::StoreBroadcastIdListL and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestStoreBroadcastIdListL0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TRequestStatus reqStatus;

	CMobilePhoneBroadcastIdList* idList = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(idList);

	RMobileBroadcastMessaging::TMobileBroadcastIdType type
	                              = RMobileBroadcastMessaging::EGsmBroadcastId;
	
	TMockLtsyData2<CMobilePhoneBroadcastIdList*,
	               RMobileBroadcastMessaging::TMobileBroadcastIdType>
	                                             expectLtsyData(idList, type);
	FillInBroadcastIdListL(*idList);
	expectLtsyData.SerialiseL(data); 

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileBroadcastMessaging::StoreBroadcastIdListL
	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileBroadcastMessagingStoreIdList, data);

	messaging.StoreBroadcastIdListL(reqStatus, idList, type);

	User::WaitForRequest(reqStatus);

	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // idList, messaging, data, this

	}

/**
@SYMTestCaseID BA-CTSY-BRMSG-RMPBLN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneBroadcastIdList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneBroadcastIdList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	CRetrieveMobilePhoneBroadcastIdList* retrieveMobilePhoneBroadcastIdList =
	                                    CRetrieveMobilePhoneBroadcastIdList::NewL(messaging);
	CleanupStack::PushL(retrieveMobilePhoneBroadcastIdList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever activeMobilePhoneBroadcastIdListRetriever(*retrieveMobilePhoneBroadcastIdList);
	scheduler.AddRetrieverL(activeMobilePhoneBroadcastIdListRetriever);

	// prepare mockLtsy data
	RMobileBroadcastMessaging::TMobileBroadcastIdType type(RMobileBroadcastMessaging::EGsmBroadcastId);
	TMockLtsyData1<RMobileBroadcastMessaging::TMobileBroadcastIdType> expectLtsyData(type);
	RBuf8 expData;
	CleanupClosePushL(expData);
	expectLtsyData.SerialiseL(expData);

	CMobilePhoneBroadcastIdList* completeList = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(completeList);
	FillInBroadcastIdListL(*completeList);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	TSerializer<CMobilePhoneBroadcastIdList>::SerialiseL(*completeList, completeData);


	retrieveMobilePhoneBroadcastIdList->Start(activeMobilePhoneBroadcastIdListRetriever.Status(), type);
	activeMobilePhoneBroadcastIdListRetriever.Activate();
	scheduler.StartScheduler();

	AssertMockLtsyStatusL();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeMobilePhoneBroadcastIdListRetriever.iStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CAsyncRetrieveAuthorizationInfo::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileBroadcastMessagingGetIdListPhase1, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, &messaging); // list, completeData, completeList, expData, retrieveMobilePhoneBroadcastIdList, messaging
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-RMPBLN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrieveMobilePhoneBroadcastIdList::Start
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrieveMobilePhoneBroadcastIdList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestStart0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	CRetrieveMobilePhoneBroadcastIdList* retrieveMobilePhoneBroadcastIdList =
	                                      CRetrieveMobilePhoneBroadcastIdList::NewL(messaging);
	CleanupStack::PushL(retrieveMobilePhoneBroadcastIdList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever activeMobilePhoneBroadcastIdListRetriever(*retrieveMobilePhoneBroadcastIdList);
	scheduler.AddRetrieverL(activeMobilePhoneBroadcastIdListRetriever);

	// prepare mockLtsy data
	RMobileBroadcastMessaging::TMobileBroadcastIdType type(RMobileBroadcastMessaging::EGsmBroadcastId);
	TMockLtsyData1<RMobileBroadcastMessaging::TMobileBroadcastIdType> expectLtsyData(type);
	RBuf8 expData;
	CleanupClosePushL(expData);
	expectLtsyData.SerialiseL(expData);

	CMobilePhoneBroadcastIdList* completeList = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(completeList);
	FillInBroadcastIdListL(*completeList);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	TSerializer<CMobilePhoneBroadcastIdList>::SerialiseL(*completeList, completeData);

 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneBroadcastIdList::Start
 	//-------------------------------------------------------------------------
 	
	retrieveMobilePhoneBroadcastIdList->Start(activeMobilePhoneBroadcastIdListRetriever.Status(), type);
	activeMobilePhoneBroadcastIdListRetriever.Activate(CActiveRetriever::ECaseGeneralCancelCase);
	scheduler.StartScheduler();

	AssertMockLtsyStatusL();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeMobilePhoneBroadcastIdListRetriever.iStatus.Int());
	

	retrieveMobilePhoneBroadcastIdList->Start(activeMobilePhoneBroadcastIdListRetriever.Status(),
	                                          RMobileBroadcastMessaging::EGsmBroadcastId);
	activeMobilePhoneBroadcastIdListRetriever.Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeMobilePhoneBroadcastIdListRetriever.iStatus.Int());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5); // completeData, completeList, expData, retrieveMobilePhoneBroadcastIdList, messaging
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-RMPBLN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to CRetrieveMobilePhoneBroadcastIdList::Start
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to CRetrieveMobilePhoneBroadcastIdList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestStart0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	CRetrieveMobilePhoneBroadcastIdList* retrieveMobilePhoneBroadcastIdList =
	                                             CRetrieveMobilePhoneBroadcastIdList::NewL(messaging);
	CleanupStack::PushL(retrieveMobilePhoneBroadcastIdList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever activeMobilePhoneBroadcastIdListRetriever(*retrieveMobilePhoneBroadcastIdList);
	scheduler.AddRetrieverL(activeMobilePhoneBroadcastIdListRetriever);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileBroadcastMessaging messaging2;
	err = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging2);
	
	CRetrieveMobilePhoneBroadcastIdList* retrieveMobilePhoneBroadcastIdList2 =
	                                              CRetrieveMobilePhoneBroadcastIdList::NewL(messaging2);
	CleanupStack::PushL(retrieveMobilePhoneBroadcastIdList2);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever activeMobilePhoneBroadcastIdListRetriever2(*retrieveMobilePhoneBroadcastIdList2);
	scheduler.AddRetrieverL(activeMobilePhoneBroadcastIdListRetriever2);

	// prepare mockLtsy data
	RMobileBroadcastMessaging::TMobileBroadcastIdType type(RMobileBroadcastMessaging::EGsmBroadcastId);
	TMockLtsyData1<RMobileBroadcastMessaging::TMobileBroadcastIdType> expectLtsyData(type);
	RBuf8 expData;
	CleanupClosePushL(expData);
	expectLtsyData.SerialiseL(expData);

	CMobilePhoneBroadcastIdList* completeList = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(completeList);
	FillInBroadcastIdListL(*completeList);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	TSerializer<CMobilePhoneBroadcastIdList>::SerialiseL(*completeList, completeData);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting CRetrieveMobilePhoneBroadcastIdList::Start
 	//-------------------------------------------------------------------------

	retrieveMobilePhoneBroadcastIdList->Start(activeMobilePhoneBroadcastIdListRetriever.Status(),
	                                          RMobileBroadcastMessaging::EGsmBroadcastId);
	activeMobilePhoneBroadcastIdListRetriever.Activate();

	retrieveMobilePhoneBroadcastIdList2->Start(activeMobilePhoneBroadcastIdListRetriever2.Status(),
	                                           RMobileBroadcastMessaging::ECdmaBroadcastId);
	activeMobilePhoneBroadcastIdListRetriever2.Activate();

	scheduler.StartScheduler();

	AssertMockLtsyStatusL();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeMobilePhoneBroadcastIdListRetriever.iStatus.Int());
	ASSERT_EQUALS(KErrNotSupported, activeMobilePhoneBroadcastIdListRetriever2.iStatus.Int());
	

	CleanupStack::PopAndDestroy(9, &messaging); // completeData, completeList, expData
	                    // retrieveMobilePhoneBroadcastIdList2, messaging2, phone2, telServer2
	                    // retrieveMobilePhoneBroadcastIdList, messaging
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-RMPBLN-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneBroadcastIdList::Start with timeout
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneBroadcastIdList::Start and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestStart0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	CRetrieveMobilePhoneBroadcastIdList* retrieveMobilePhoneBroadcastIdList =
	                                    CRetrieveMobilePhoneBroadcastIdList::NewL(messaging);
	CleanupStack::PushL(retrieveMobilePhoneBroadcastIdList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever activeMobilePhoneBroadcastIdListRetriever(*retrieveMobilePhoneBroadcastIdList);
	scheduler.AddRetrieverL(activeMobilePhoneBroadcastIdListRetriever);

	// prepare mockLtsy data
	RMobileBroadcastMessaging::TMobileBroadcastIdType type(RMobileBroadcastMessaging::EGsmBroadcastId);
	TMockLtsyData1<RMobileBroadcastMessaging::TMobileBroadcastIdType> expectLtsyData(type);
	RBuf8 expData;
	CleanupClosePushL(expData);
	expectLtsyData.SerialiseL(expData);

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneBroadcastIdList::Start.
	//-------------------------------------------------------------------------

	retrieveMobilePhoneBroadcastIdList->Start(activeMobilePhoneBroadcastIdListRetriever.Status(), type);
	activeMobilePhoneBroadcastIdListRetriever.Activate();
	scheduler.StartScheduler();

	AssertMockLtsyStatusL();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeMobilePhoneBroadcastIdListRetriever.iStatus.Int());

	CleanupStack::PopAndDestroy(3, &messaging); // expData, retrieveMobilePhoneBroadcastIdList, messaging
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BRM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::ReceiveMessage
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestReceiveMessage0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data, KErrNotSupported);

	TRequestStatus reqStatus;
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes(attributes);
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data);

    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrGeneral);

	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: increase coverage (completion with EMmTsyGsmBroadcastNotifyMessageReceived IPC)
 	//-------------------------------------------------------------------------

    data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data);

	TGsmCbsMsg cbsMsg;
	cbsMsg.iCbsMsg.Copy(_L("Cool!"));
	TMockLtsyData1<TGsmCbsMsg> completeLtsyData(cbsMsg);
	data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMmTsyGsmBroadcastNotifyMessageReceived, KErrGeneral, data);

	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: increase coverage (completion with EMmTsyWcdmaBroadcastNotifyMessageReceived IPC)
 	//-------------------------------------------------------------------------

    data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data);

    iMockLTSY.CompleteL(EMmTsyWcdmaBroadcastNotifyMessageReceived, KErrGeneral, KNullDesC8);

	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 wcdmaAttributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg wcdmaMsgAttributes(wcdmaAttributes);
	messaging.ReceiveMessage(reqStatus, msgData, wcdmaMsgAttributes);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileBroadcastMessaging::ReceiveMessage when result is not cached.
 	//-------------------------------------------------------------------------

    data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data);

    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);

    data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMmTsyGsmBroadcastNotifyMessageReceived, KErrNone, data);

	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(cbsMsg.iCbsMsg, msgData);
	TUint32 expectedFlags(RMobileBroadcastMessaging::KBroadcastDataFormat);
	RMobileBroadcastMessaging::TMobileBroadcastDataFormat
	                      expectedFormat(RMobileBroadcastMessaging::EFormatGsmTpdu);
	ASSERT_EQUALS(expectedFlags, attributes.iFlags);
	ASSERT_EQUALS(expectedFormat, attributes.iFormat);

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST C: increase coverage (completion with multypage message type)
 	//-------------------------------------------------------------------------

    expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
    data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessageCancel, data, KErrNotSupported);
    
    expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
    data.Close();
    expectLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data);

	messaging.ReceiveMessage(reqStatus, msgData, wcdmaMsgAttributes);

	CArrayPtrFlat<TWcdmaCbsMsg>* wcdmaCbsMsgArray = new ( ELeave ) CArrayPtrFlat<TWcdmaCbsMsg>( 1 );
	CleanupStack::PushL(wcdmaCbsMsgArray);

	TWcdmaCbsMsg wcdmaCbsMsg0;
	wcdmaCbsMsg0.iWcdmaCbsData.Copy(_L("0cool0"));
	wcdmaCbsMsg0.iSbNumber = 1;
	wcdmaCbsMsg0.iNumberOfPages = 2;
	wcdmaCbsMsg0.iMessageType = 3;
	wcdmaCbsMsg0.iMessageId = 4;
	wcdmaCbsMsg0.iSerialNum = 5;
	wcdmaCbsMsg0.iDCS = 6;
	wcdmaCbsMsg0.iInfoLength = wcdmaCbsMsg0.iWcdmaCbsData.Length();
	wcdmaCbsMsgArray->AppendL(&wcdmaCbsMsg0, 1);
    
	TUint8 wcdmaPageNumber(1);
	TMockLtsyData2<CArrayPtrFlat< TWcdmaCbsMsg >*, TUint8> completeWcdmaLtsyData(wcdmaCbsMsgArray, wcdmaPageNumber);
	data.Close();
	completeWcdmaLtsyData.SerialiseL(data);

	iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMmTsyWcdmaBroadcastNotifyMessageReceived, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	// generate a message from the original message with the page number at the end
	TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize>  messageWithPageNumber;
	messageWithPageNumber.Copy(wcdmaCbsMsg0.iWcdmaCbsData);
	_LIT8(KFormat, "%c");
	TBuf8<2> abc(KFormat);
	// Append pagenumber to end of CBS message     
	messageWithPageNumber.AppendFormat( abc, wcdmaPageNumber);

	ASSERT_EQUALS(messageWithPageNumber,  msgData);

	ASSERT_EQUALS(expectedFlags,               wcdmaAttributes.iFlags);
	expectedFormat = RMobileBroadcastMessaging::EFormatWcdmaTpdu;
	ASSERT_EQUALS(expectedFormat,              wcdmaAttributes.iFormat);
	ASSERT_EQUALS(wcdmaCbsMsg0.iNumberOfPages, wcdmaAttributes.iNumberOfPages);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageType,   wcdmaAttributes.iMessageType);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageId,     wcdmaAttributes.iMessageId);
	ASSERT_EQUALS(wcdmaCbsMsg0.iSerialNum,     wcdmaAttributes.iSerialNum);
	ASSERT_EQUALS(wcdmaCbsMsg0.iDCS,           wcdmaAttributes.iDCS);

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST C: increase coverage (completion with multypage message type)
	// this time 3 pages
 	//-------------------------------------------------------------------------

	messaging.ReceiveMessage(reqStatus, msgData, wcdmaMsgAttributes);

	TWcdmaCbsMsg wcdmaCbsMsg1;
	wcdmaCbsMsg1.iWcdmaCbsData.Copy(_L("1cool1"));
	wcdmaCbsMsg1.iSbNumber = 8;
	wcdmaCbsMsg1.iNumberOfPages = 7;
	wcdmaCbsMsg1.iMessageType = 6;
	wcdmaCbsMsg1.iMessageId = 5;
	wcdmaCbsMsg1.iSerialNum = 4;
	wcdmaCbsMsg1.iDCS = 3;
	wcdmaCbsMsg1.iInfoLength = wcdmaCbsMsg1.iWcdmaCbsData.Length();
	wcdmaCbsMsgArray->AppendL(&wcdmaCbsMsg1, 1);
	
	TWcdmaCbsMsg wcdmaCbsMsg2;
	wcdmaCbsMsg2.iWcdmaCbsData.Copy(_L("2cool2"));
	wcdmaCbsMsg2.iSbNumber = 80;
	wcdmaCbsMsg2.iNumberOfPages = 70;
	wcdmaCbsMsg2.iMessageType = 60;
	wcdmaCbsMsg2.iMessageId = 50;
	wcdmaCbsMsg2.iSerialNum = 40;
	wcdmaCbsMsg2.iDCS = 30;
	wcdmaCbsMsg2.iInfoLength = wcdmaCbsMsg2.iWcdmaCbsData.Length();
	wcdmaCbsMsgArray->AppendL(&wcdmaCbsMsg2, 1);

	wcdmaPageNumber = 3;
    data.Close();
	completeWcdmaLtsyData.SerialiseL(data); 

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    iMockLTSY.CompleteL(EMmTsyWcdmaBroadcastNotifyMessageReceived, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
    
	
	ASSERT_EQUALS(messageWithPageNumber,  msgData);
	ASSERT_EQUALS(expectedFlags,               wcdmaAttributes.iFlags);
	expectedFormat = RMobileBroadcastMessaging::EFormatWcdmaTpdu;
	ASSERT_EQUALS(expectedFormat,              wcdmaAttributes.iFormat);
	ASSERT_EQUALS(wcdmaCbsMsg0.iNumberOfPages, wcdmaAttributes.iNumberOfPages);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageType,   wcdmaAttributes.iMessageType);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageId,     wcdmaAttributes.iMessageId);
	ASSERT_EQUALS(wcdmaCbsMsg0.iSerialNum,     wcdmaAttributes.iSerialNum);
	ASSERT_EQUALS(wcdmaCbsMsg0.iDCS,           wcdmaAttributes.iDCS);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST D: RMobileBroadcastMessaging::ReceiveMessage again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
 	// recieve 2nd page
 	//-------------------------------------------------------------------------

	messaging.ReceiveMessage(reqStatus, msgData, wcdmaMsgAttributes);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

    messageWithPageNumber.Copy(wcdmaCbsMsg1.iWcdmaCbsData);
    messageWithPageNumber.AppendFormat( abc, 2);

    ASSERT_EQUALS(messageWithPageNumber,  msgData);
	ASSERT_EQUALS(expectedFlags,               wcdmaAttributes.iFlags);
	ASSERT_EQUALS(expectedFormat,              wcdmaAttributes.iFormat);
	ASSERT_EQUALS(wcdmaCbsMsg1.iNumberOfPages, wcdmaAttributes.iNumberOfPages);
	ASSERT_EQUALS(wcdmaCbsMsg1.iMessageType,   wcdmaAttributes.iMessageType);
	ASSERT_EQUALS(wcdmaCbsMsg1.iMessageId,     wcdmaAttributes.iMessageId);
	ASSERT_EQUALS(wcdmaCbsMsg1.iSerialNum,     wcdmaAttributes.iSerialNum);
	ASSERT_EQUALS(wcdmaCbsMsg1.iDCS,           wcdmaAttributes.iDCS);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
 	// recieve 3rd page
 	//-------------------------------------------------------------------------

	messaging.ReceiveMessage(reqStatus, msgData, wcdmaMsgAttributes);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
    messageWithPageNumber.Copy(wcdmaCbsMsg2.iWcdmaCbsData);
    messageWithPageNumber.AppendFormat( abc, 3);
	ASSERT_EQUALS(messageWithPageNumber,  msgData);
	ASSERT_EQUALS(expectedFlags,               wcdmaAttributes.iFlags);
	ASSERT_EQUALS(expectedFormat,              wcdmaAttributes.iFormat);
	ASSERT_EQUALS(wcdmaCbsMsg2.iNumberOfPages, wcdmaAttributes.iNumberOfPages);
	ASSERT_EQUALS(wcdmaCbsMsg2.iMessageType,   wcdmaAttributes.iMessageType);
	ASSERT_EQUALS(wcdmaCbsMsg2.iMessageId,     wcdmaAttributes.iMessageId);
	ASSERT_EQUALS(wcdmaCbsMsg2.iSerialNum,     wcdmaAttributes.iSerialNum);
	ASSERT_EQUALS(wcdmaCbsMsg2.iDCS,           wcdmaAttributes.iDCS);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileBroadcastMessaging::ReceiveMessage
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: increase coverage (completion with EMmTsyGsmBroadcastNotifyMessageReceived IPC)
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(reqStatus);	
	data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMmTsyGsmBroadcastNotifyMessageReceived, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: increase coverage (completion with EMmTsyWcdmaBroadcastNotifyMessageReceived IPC)
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(reqStatus);	
	data.Close();
    completeWcdmaLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMmTsyWcdmaBroadcastNotifyMessageReceived, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // wcdmaCbsMsgArray, messaging, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BRM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileBroadcastMessaging::ReceiveMessage
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileBroadcastMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestReceiveMessage0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileBroadcastMessaging::ReceiveMessage
 	//-------------------------------------------------------------------------
 	
	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data);

	TGsmCbsMsg cbsMsg;
	cbsMsg.iCbsMsg.Copy(_L("Cool!"));
	TMockLtsyData1<TGsmCbsMsg> completeLtsyData(cbsMsg);
	data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMmTsyGsmBroadcastNotifyMessageReceived, KErrNone, data, 10);

	TRequestStatus reqStatus;
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes(attributes);
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);

	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Test cancelling, increase coverage
 	//-------------------------------------------------------------------------

	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// execute request
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);

	// setting cancel request
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessageCancel, data, KErrNotSupported);
  
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);

	// ReceiveMessage completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Test cancelling, increase coverage
 	//-------------------------------------------------------------------------

	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// execute request
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);

	// setting cancel request
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessageCancel, data);
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessageCancel, KErrGeneral);
  
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);

	// ReceiveMessage completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Test cancelling, increase coverage
 	//-------------------------------------------------------------------------

	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// execute request
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);

	// setting cancel request
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessageCancel, data);
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessageCancel, KErrNone);
  
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);

	// ReceiveMessage completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // messaging, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BRM-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::ReceiveMessage with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::ReceiveMessage with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestReceiveMessage0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	//-------------------------------------------------------------------------
	// Test passing wrong descriptor size to parameter aMsgData in
	// RMobileBroadcastMessaging::ReceiveMessage
	//-------------------------------------------------------------------------

	TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize+1> wrongMsgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 wcdmaAttributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg wcdmaMsgAttributes(wcdmaAttributes);
	TRequestStatus reqStatus;
	messaging.ReceiveMessage(reqStatus, wrongMsgData, wcdmaMsgAttributes);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong descriptor size to parameter aMsgAttributes in
	// RMobileBroadcastMessaging::ReceiveMessage
	//-------------------------------------------------------------------------

	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
	expectLtsyData.SerialiseL(data); 

	CArrayPtrFlat<TWcdmaCbsMsg>* wcdmaCbsMsgArray = new ( ELeave ) CArrayPtrFlat<TWcdmaCbsMsg>( 1 );
	CleanupStack::PushL(wcdmaCbsMsgArray);

	TWcdmaCbsMsg wcdmaCbsMsg0;
	wcdmaCbsMsg0.iWcdmaCbsData.Copy(_L("0cool0"));
	wcdmaCbsMsg0.iSbNumber = 1;
	wcdmaCbsMsg0.iNumberOfPages = 2;
	wcdmaCbsMsg0.iMessageType = 3;
	wcdmaCbsMsg0.iMessageId = 4;
	wcdmaCbsMsg0.iSerialNum = 5;
	wcdmaCbsMsg0.iDCS = 6;
	wcdmaCbsMsg0.iInfoLength = wcdmaCbsMsg0.iWcdmaCbsData.Length();
	wcdmaCbsMsgArray->AppendL(&wcdmaCbsMsg0, 1);
	
	TUint8 wcdmaPageNumber(1);
	TMockLtsyData2<CArrayPtrFlat< TWcdmaCbsMsg >*, TUint8> completeWcdmaLtsyData(wcdmaCbsMsgArray, wcdmaPageNumber);
	
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	
	TBuf8<1> wrongMsgAttributes;

	messaging.ReceiveMessage(reqStatus, msgData, wrongMsgAttributes);
	User::WaitForRequest(reqStatus);

	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test B: Test passing out of bounds parameters from LTSY in
	// request completion data
 	//-------------------------------------------------------------------------
 	data.Close();
	expectLtsyData.SerialiseL(data); 
	iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data);

	wcdmaPageNumber = 4; // greater
	data.Close();
	completeWcdmaLtsyData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMmTsyWcdmaBroadcastNotifyMessageReceived, KErrNone, data);

	messaging.ReceiveMessage(reqStatus, msgData, wcdmaMsgAttributes);
	User::WaitForRequest(reqStatus);

	ASSERT_EQUALS(KErrCorrupt, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // wcdmaCbsMsgArray, messaging, data, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BRM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileBroadcastMessaging::ReceiveMessage
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileBroadcastMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestReceiveMessage0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileBroadcastMessaging messaging2;
	err = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging2);

	// Open third client
	RTelServer telServer3;
	ret = telServer3.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer3);

	RMobilePhone phone3;
	ret = phone3.Open(telServer3, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone3);

	RMobileBroadcastMessaging messaging3;
	err = messaging3.Open(phone3);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging3);

	// Open fourth client
	RTelServer telServer4;
	ret = telServer4.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer4);

	RMobilePhone phone4;
	ret = phone4.Open(telServer4, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone4);

	RMobileBroadcastMessaging messaging4;
	err = messaging4.Open(phone4);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging4);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileBroadcastMessaging::ReceiveMessage
	//         Using RMobileBroadcastMessaging::TMobileBroadcastAttributesV1
 	//-------------------------------------------------------------------------

	// Setting up multiple Gsm requests
	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data);

    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
    TGsmCbsMsg cbsMsg;
	cbsMsg.iCbsMsg.Copy(_L("Cool!"));
	TMockLtsyData1<TGsmCbsMsg> completeLtsyData(cbsMsg);
	data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMmTsyGsmBroadcastNotifyMessageReceived, KErrNone, data);

	TRequestStatus reqStatus;
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes(attributes);
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);

	TRequestStatus reqStatus2;
	RMobileBroadcastMessaging::TBroadcastPageData msgData2;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes2;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes2(attributes2);
	messaging2.ReceiveMessage(reqStatus2, msgData2, msgAttributes2);
	
	// completion
	RMobileBroadcastMessaging::TMobileBroadcastDataFormat
	                      expectedFormat(RMobileBroadcastMessaging::EFormatGsmTpdu);	
    TUint32 expectedFlags(RMobileBroadcastMessaging::KBroadcastDataFormat);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(cbsMsg.iCbsMsg, msgData2);
	ASSERT_EQUALS(expectedFlags, attributes2.iFlags);
	ASSERT_EQUALS(expectedFormat, attributes2.iFormat);	
	
		
	User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    ASSERT_EQUALS(cbsMsg.iCbsMsg, msgData);
	ASSERT_EQUALS(expectedFlags, attributes.iFlags);
	ASSERT_EQUALS(expectedFormat, attributes.iFormat);	

		
	//-------------------------------------------------------------------------
	// Test B: Test multiple clients requesting RMobileBroadcastMessaging::ReceiveMessage
	//         Using RMobileBroadcastMessaging::TMobileBroadcastAttributesV2
 	//-------------------------------------------------------------------------
		
	// Setting up multiple Wcdma requests 
	data.Close();
	expectLtsyData.SerialiseL(data); 
	
	CArrayPtrFlat<TWcdmaCbsMsg>* wcdmaCbsMsgArray = new ( ELeave ) CArrayPtrFlat<TWcdmaCbsMsg>( 1 );
	CleanupStack::PushL(wcdmaCbsMsgArray);

	TWcdmaCbsMsg wcdmaCbsMsg0;
	wcdmaCbsMsg0.iWcdmaCbsData.Copy(_L("0cool0"));
	wcdmaCbsMsg0.iSbNumber = 1;
	wcdmaCbsMsg0.iNumberOfPages = 2;
	wcdmaCbsMsg0.iMessageType = 3;
	wcdmaCbsMsg0.iMessageId = 4;
	wcdmaCbsMsg0.iSerialNum = 5;
	wcdmaCbsMsg0.iDCS = 6;
	wcdmaCbsMsg0.iInfoLength = wcdmaCbsMsg0.iWcdmaCbsData.Length();
	wcdmaCbsMsgArray->AppendL(&wcdmaCbsMsg0, 1);
	
	TUint8 wcdmaPageNumber(1);
	TMockLtsyData2<CArrayPtrFlat< TWcdmaCbsMsg >*, TUint8> completeWcdmaLtsyData(wcdmaCbsMsgArray, wcdmaPageNumber);

	data.Close();
	completeWcdmaLtsyData.SerialiseL(data); 
	
	iMockLTSY.CompleteL(EMmTsyWcdmaBroadcastNotifyMessageReceived, KErrNone, data);
    

	TRequestStatus reqStatus3;
	RMobileBroadcastMessaging::TBroadcastPageData msgData3;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 attributes3;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg msgAttributes3(attributes3);
	messaging3.ReceiveMessage(reqStatus3, msgData3, msgAttributes3);

	TRequestStatus reqStatus4;
	RMobileBroadcastMessaging::TBroadcastPageData msgData4;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 attributes4;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg msgAttributes4(attributes4);
	messaging4.ReceiveMessage(reqStatus4, msgData4, msgAttributes4);

	// completion
	TUint32 expectedFlags2(RMobileBroadcastMessaging::KBroadcastDataFormat);
	RMobileBroadcastMessaging::TMobileBroadcastDataFormat
	                      expectedFormat2(RMobileBroadcastMessaging::EFormatWcdmaTpdu);

	User::WaitForRequest(reqStatus4);
	ASSERT_EQUALS(KErrNone, reqStatus4.Int());
	
	// generate a message from the original message with the page number at the end
	TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize>  messageWithPageNumber;
	messageWithPageNumber.Copy(wcdmaCbsMsg0.iWcdmaCbsData);
	_LIT8(KFormat, "%c");
	TBuf8<2> abc(KFormat);
	// Append pagenumber to end of CBS message     
	messageWithPageNumber.AppendFormat( abc, wcdmaPageNumber);
	
	ASSERT_EQUALS(messageWithPageNumber,  msgData4);	

	ASSERT_EQUALS(expectedFlags2,               attributes4.iFlags);
	ASSERT_EQUALS(expectedFormat2,              attributes4.iFormat);
	ASSERT_EQUALS(wcdmaCbsMsg0.iNumberOfPages,  attributes4.iNumberOfPages);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageType,    attributes4.iMessageType);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageId,      attributes4.iMessageId);
	ASSERT_EQUALS(wcdmaCbsMsg0.iSerialNum,      attributes4.iSerialNum);
	ASSERT_EQUALS(wcdmaCbsMsg0.iDCS,            attributes4.iDCS);
		

	User::WaitForRequest(reqStatus3);
    ASSERT_EQUALS(KErrNone, reqStatus3.Int());
	ASSERT_EQUALS(messageWithPageNumber,  msgData3);
		
	ASSERT_EQUALS(expectedFlags2,               attributes3.iFlags);
	ASSERT_EQUALS(expectedFormat2,              attributes3.iFormat);
	ASSERT_EQUALS(wcdmaCbsMsg0.iNumberOfPages,  attributes3.iNumberOfPages);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageType,    attributes3.iMessageType);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageId,      attributes3.iMessageId);
	ASSERT_EQUALS(wcdmaCbsMsg0.iSerialNum,      attributes3.iSerialNum);
	ASSERT_EQUALS(wcdmaCbsMsg0.iDCS,            attributes3.iDCS);

    
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(13, this); // wcdmaCbsMsgArray, messaging(4), phone(3), telServer(3), data, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BRM-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::ReceiveMessage with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::ReceiveMessage and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestReceiveMessage0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileBroadcastMessaging::ReceiveMessage
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes(attributes);
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);

	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessageCancel, data);
  
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // messaging, data, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BGFS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::GetFilterSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::GetFilterSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestGetFilterSetting0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileBroadcastMessaging::GetFilterSetting.
 	//-------------------------------------------------------------------------

	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting;
	err = messaging.GetFilterSetting(setting);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(RMobileBroadcastMessaging::EBroadcastAcceptAll, setting);
	AssertMockLtsyStatusL();

	// change setting
	TRequestStatus reqStatus;
	messaging.SetFilterSetting(reqStatus, RMobileBroadcastMessaging::EBroadcastAcceptNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// check if the setting is changed
	err = messaging.GetFilterSetting(setting);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(RMobileBroadcastMessaging::EBroadcastAcceptNone, setting);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // messaging, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSFS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::SetFilterSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::SetFilterSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestSetFilterSetting0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	TRequestStatus reqStatus;

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// setting request and execute
	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data, KErrNotSupported);

	messaging.SetFilterSetting(reqStatus, expectData.iSetting);
	
	// completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	// no need in setting environement ( CMmBroadcastTsy::iCbRoutingActivated is ETrue)

	// setting request and execute
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data);
	iMockLTSY.CompleteL(EMobileBroadcastMessagingSetFilterSetting, KErrGeneral);

	messaging.SetFilterSetting(reqStatus, expectData.iSetting);
	
	// completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileBroadcastMessaging::SetFilterSetting.
 	//-------------------------------------------------------------------------

	// no need in setting environement ( CMmBroadcastTsy::iCbRoutingActivated is ETrue)

	// setting request and execute
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data);
	iMockLTSY.CompleteL(EMobileBroadcastMessagingSetFilterSetting, KErrNone);

	messaging.SetFilterSetting(reqStatus, expectData.iSetting);
	
	// completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: increase coverage ( SetFilterSetting(EBroadcastAcceptNone) )
 	//-------------------------------------------------------------------------
 	
	// no need in setting environement ( CMmBroadcastTsy::iCbRoutingActivated is ETrue)

	// setting request and execute
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data);
	iMockLTSY.CompleteL(EMobileBroadcastMessagingSetFilterSetting, KErrNone);

	messaging.SetFilterSetting(reqStatus, expectData.iSetting);
	
	// completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();
 	
 	//-------------------------------------------------------------------------
	// TEST C: increase coverage ( SetFilterSetting(EBroadcastAcceptNone) when
	// CMmBroadcastTsy::iCbRoutingActivated equals EFalse )
 	//-------------------------------------------------------------------------

	// no need in setting environement ( CMmBroadcastTsy::iCbRoutingActivated is EFalse)

	// setting request and execute
	messaging.SetFilterSetting(reqStatus, RMobileBroadcastMessaging::EBroadcastAcceptNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: increase coverage ( SetFilterSetting(EBroadcastAcceptNone) when
	// CMmBroadcastTsy::iCbRoutingActivated equals EFalse
	// but there is pending ReceiveMessage request )
 	//-------------------------------------------------------------------------

	// no need in setting environement by change CMmBroadcastTsy::iCbRoutingActivated
	// ( CMmBroadcastTsy::iCbRoutingActivated is EFalse )

	// setting and execute ReceiveMessage request
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingReceiveMessage, data);
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes(attributes);
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);
	AssertMockLtsyStatusL();
	
	// setting and execute SetFilterSetting
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data);
	iMockLTSY.CompleteL(EMobileBroadcastMessagingSetFilterSetting, KErrNone);
	messaging.SetFilterSetting(reqStatus, expectData.iSetting);

	// completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	// cancel ReceiveMessage request
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileBroadcastMessaging::SetFilterSetting
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingSetFilterSetting, KErrNone);

	User::WaitForRequest(reqStatus); // KERN:4 in CMmBroadcastTsy.cpp line 998
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // messaging, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSFS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileBroadcastMessaging::SetFilterSetting
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileBroadcastMessaging::SetFilterSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestSetFilterSetting0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	TRequestStatus reqStatus;

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileBroadcastMessaging::SetFilterSetting
 	//-------------------------------------------------------------------------
 	
	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	// setting request and execute
	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data);
    iMockLTSY.CompleteL(EMobileBroadcastMessagingSetFilterSetting, KErrNone, 10);

	messaging.SetFilterSetting(reqStatus, expectData.iSetting);

	// cancelling
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingSetFilterSetting);
	
	// completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // messaging, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSFS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::SetFilterSetting with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::SetFilterSetting with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestSetFilterSetting0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMobileBroadcastMessaging::SetFilterSetting
 	//-------------------------------------------------------------------------
 	
	// test for support with RMobileBroadcastMessaging::EBroadcastFilterUnspecified parameter value
	TRequestStatus reqStatus;
	messaging.SetFilterSetting(reqStatus, RMobileBroadcastMessaging::EBroadcastFilterUnspecified);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	// check if the setting is not changed
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting;
	err = messaging.GetFilterSetting(setting);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(RMobileBroadcastMessaging::EBroadcastAcceptAll, setting);
	AssertMockLtsyStatusL();

	// test for support with RMobileBroadcastMessaging::EBroadcastAcceptFilter parameter value
	messaging.SetFilterSetting(reqStatus, RMobileBroadcastMessaging::EBroadcastAcceptFilter);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	// check if the setting is not changed
	err = messaging.GetFilterSetting(setting);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(RMobileBroadcastMessaging::EBroadcastAcceptAll, setting);
	AssertMockLtsyStatusL();

	// test for support with RMobileBroadcastMessaging::EBroadcastRejectFilter parameter value
	messaging.SetFilterSetting(reqStatus, RMobileBroadcastMessaging::EBroadcastRejectFilter);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	// check if the setting is not changed
	err = messaging.GetFilterSetting(setting);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(RMobileBroadcastMessaging::EBroadcastAcceptAll, setting);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // messaging, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSFS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileBroadcastMessaging::SetFilterSetting
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileBroadcastMessaging::SetFilterSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestSetFilterSetting0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt ret = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(messaging);

	// Open second client
	RTelServer telServer2;
	ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileBroadcastMessaging messaging2;
	ret = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(messaging2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileBroadcastMessaging::SetFilterSetting
 	//-------------------------------------------------------------------------

	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// setting 1st request
	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data);
    iMockLTSY.CompleteL(EMobileBroadcastMessagingSetFilterSetting, KErrNone, 5);

	// setting 2nd request data
	TCbsCbmiAndLangAndFilter expectData2;
	expectData2.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	expectData2.iCbmiStorage = KNullDesC; // deprecated
	expectData2.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData2(expectData2);
	data.Close();
    expectLtsyData2.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data);
    iMockLTSY.CompleteL(EMobileBroadcastMessagingSetFilterSetting, KErrNone, 10);

	TRequestStatus reqStatus2;
	messaging.SetFilterSetting(reqStatus, expectData.iSetting);
	messaging2.SetFilterSetting(reqStatus2, expectData2.iSetting);

    // completion of 1st request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	// check how the setting is changed
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting;
	ret = messaging.GetFilterSetting(setting);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(RMobileBroadcastMessaging::EBroadcastAcceptAll, setting);

    // completion of 2nd request
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());

	// check how the setting is changed
	ret = messaging.GetFilterSetting(setting);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(RMobileBroadcastMessaging::EBroadcastAcceptNone, setting);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // messaging2, phone2, telServer2, messaging, data, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSFS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::SetFilterSetting with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::SetFilterSetting and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestSetFilterSetting0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileBroadcastMessaging::SetFilterSetting
 	//-------------------------------------------------------------------------

	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// setting request and execute
	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated

	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data);

	messaging.SetFilterSetting(reqStatus, expectData.iSetting);

	// completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // messaging, data, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BGLF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::GetLanguageFilter
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::GetLanguageFilter
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestGetLanguageFilter0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TBuf<1> buf;
	TRequestStatus reqStatus;
	messaging.GetLanguageFilter(reqStatus, buf);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // messaging, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BSLF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::SetLanguageFilter
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::SetLanguageFilter
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestSetLanguageFilter0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TRequestStatus reqStatus;
	TBuf<1> langFilter;
	messaging.SetLanguageFilter(reqStatus, langFilter);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // messaging, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BNFSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::NotifyFilterSettingChange
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::NotifyFilterSettingChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestNotifyFilterSettingChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TRequestStatus reqStatus;

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileBroadcastMessaging::NotifyFilterSettingChange.
 	//-------------------------------------------------------------------------

	// setting request and execute
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting;
	messaging.NotifyFilterSettingChange(reqStatus, setting);

	// invoke FilterSettingChange
	TRequestStatus mockLtsyStatus;
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter newSetting =
	                                RMobileBroadcastMessaging::EBroadcastAcceptNone;
	messaging.SetFilterSetting(mockLtsyStatus, newSetting);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(newSetting, setting);

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion when CMmBroadcastTsy::iCbRoutingActivated is ETrue
 	//-------------------------------------------------------------------------
	// setting request and execute
	messaging.NotifyFilterSettingChange(reqStatus, setting);

	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    iMockLTSY.CompleteL(EMobileBroadcastMessagingReceiveMessage, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	// invoke FilterSettingChange
	TCbsCbmiAndLangAndFilter expectData;
	expectData.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	expectData.iCbmiStorage = KNullDesC; // deprecated
	expectData.iLanguageStorage = KNullDesC; // deprecated
	TMockLtsyData1<TCbsCbmiAndLangAndFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobileBroadcastMessagingSetFilterSetting, data);
	iMockLTSY.CompleteL(EMobileBroadcastMessagingSetFilterSetting, KErrNone);
	messaging.SetFilterSetting(mockLtsyStatus, expectData.iSetting);
	User::WaitForRequest(mockLtsyStatus); // KERN:4 in CMmBroadcastTsy.cpp line 1059
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(expectData.iSetting, setting);

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); // messaging, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BNFSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileBroadcastMessaging::NotifyFilterSettingChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileBroadcastMessaging::NotifyFilterSettingChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestNotifyFilterSettingChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test cancelling of RMobileBroadcastMessaging::NotifyFilterSettingChange
	//-------------------------------------------------------------------------
 
	// setting request and execute
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting;
	messaging.NotifyFilterSettingChange(reqStatus, setting);
	
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingNotifyFilterSettingChange);

	// completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // messaging, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BNFSC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::NotifyFilterSettingChange with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::NotifyFilterSettingChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestNotifyFilterSettingChange0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobileBroadcastMessaging::NotifyFilterSettingChange
 	//-------------------------------------------------------------------------

	// setting request and execute
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* settingPtr(NULL);
	messaging.NotifyFilterSettingChange(reqStatus, *settingPtr);

	// completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrBadDescriptor, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // messaging, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BNFSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileBroadcastMessaging::NotifyFilterSettingChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileBroadcastMessaging::NotifyFilterSettingChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestNotifyFilterSettingChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	// Open second client
	RTelServer telServer2;
	err = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	err = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(phone2);

	RMobileBroadcastMessaging messaging2;
	err = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileBroadcastMessaging::NotifyFilterSettingChange
 	//-------------------------------------------------------------------------

	// execute 1st request
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting;
	TRequestStatus reqStatus;
	messaging.NotifyFilterSettingChange(reqStatus, setting);

	// execute 2nd request
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting2;
	TRequestStatus reqStatus2;
	messaging2.NotifyFilterSettingChange(reqStatus2, setting2);

	// invoke FilterSettingChange
	TRequestStatus mockLtsyStatus;
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter newSetting =
	                                RMobileBroadcastMessaging::EBroadcastAcceptNone;
	messaging.SetFilterSetting(mockLtsyStatus, newSetting);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(newSetting, setting);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(newSetting, setting2);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // messaging2, phone2, telServer2, messaging, this

	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BNLFC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::NotifyLanguageFilterChange
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::NotifyLanguageFilterChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestNotifyLanguageFilterChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TBuf<1> buf;
	TRequestStatus reqStatus;
	messaging.NotifyLanguageFilterChange(reqStatus, buf);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // messaging, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-MNBILC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::NotifyBroadcastIdListChange
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::NotifyBroadcastIdListChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestNotifyBroadcastIdListChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	TRequestStatus reqStatus;
	messaging.NotifyBroadcastIdListChange(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // messaging, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BGC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::GetCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::GetCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestGetCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileBroadcastMessaging::GetCaps.
 	//-------------------------------------------------------------------------

	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 caps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capsPckg(caps);
	err = messaging.GetCaps(capsPckg);
	
	// check results
	ASSERT_EQUALS(KErrNone, err);

	TUint32 expModeCaps = RMobileBroadcastMessaging::KCapsGsmTpduFormat;
#ifdef __WINS__
	expModeCaps = RMobileBroadcastMessaging::KCapsWcdmaTpduFormat
        | RMobileBroadcastMessaging::KCapsGsmTpduFormat;
#endif // __WINS__       
	ASSERT_EQUALS(expModeCaps, caps.iModeCaps);

	TUint32 expFilterCaps = RMobileBroadcastMessaging::KCapsSimpleFilter;
	ASSERT_EQUALS(expFilterCaps, caps.iFilterCaps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // messaging, this
	
	}


/**
@SYMTestCaseID BA-CTSY-BRMSG-BGC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileBroadcastMessaging::GetCaps with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileBroadcastMessaging::GetCaps with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyBroadcastMessagingFU::TestGetCaps0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobileBroadcastMessaging::GetCaps
 	//-------------------------------------------------------------------------

	RMobileBroadcastMessaging::TMobileBroadcastCapsV1* caps(NULL);
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capsPckg(*caps);
	err = messaging.GetCaps(capsPckg);
	ASSERT_EQUALS(KErrBadDescriptor, err);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileBroadcastMessaging::GetCaps
 	//-------------------------------------------------------------------------

	TInt intCaps;
	TPckg<TInt> intCapsPckg(intCaps);
	err = messaging.GetCaps(intCapsPckg);
	ASSERT_EQUALS(KErrArgument, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // messaging, this

	}
