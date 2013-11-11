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
// The TEFUnit test suite for CallBarring in the Common TSY.
// 
//

/**
 @file 
*/


#include "cctsycallbarringfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <mmlist.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "cctsyactiveretriever.h"



CTestSuite* CCTsyCallBarringFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestStart0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestStart0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestStart0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestStart0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetSSPassword0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetSSPassword0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetSSPassword0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetSSPassword0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetSSPassword0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestGetCompMethodName0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetCallBarringStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetCallBarringStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetCallBarringStatus0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetCallBarringStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetCallBarringStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestSetCallBarringPassword0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestNotifyCallBarringStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestNotifyCallBarringStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallBarringFU, TestNotifyCallBarringStatusChange0004L);

	END_SUITE;
	}

void CCTsyCallBarringFU::OpenPhoneCBListLC()
	{
	iCBList = CRetrieveMobilePhoneCBList::NewL(iPhone);
	CleanupStack::PushL(iCBList);
	}

//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-CBAR-RMPCB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneCBList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCBList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestStart0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	OpenPhoneCBListLC();
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewLC(*iCBList);
	scheduler.AddRetrieverL(*activeRetriever);
	
	
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;

	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> conditionData(condition);
	conditionData.SerialiseL(data);
	
	CMobilePhoneCBList* cbList = CMobilePhoneCBList::NewL();
	CleanupStack::PushL(cbList);
	RMobilePhone::TMobilePhoneCBInfoEntryV1 listEntry;
	listEntry.iStatus = RMobilePhone::ECallBarringStatusActive;
	listEntry.iCondition = RMobilePhone::EBarAllIncoming;
	listEntry.iServiceGroup = RMobilePhone::EVoiceService;
	cbList->AddEntryL(listEntry);
	
	CBufBase* completeCBListBuf = cbList->StoreLC();
	dataComplete.Create(completeCBListBuf->Ptr(0));
	CleanupStack::PopAndDestroy(completeCBListBuf);
  	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetBarringStatusPhase1, data, KErrNotSupported);
	iCBList->Start(activeRetriever->Status(), condition);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber())
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->Status().Int())

	AssertMockLtsyStatusL();
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
 	iMockLTSY.ExpectL(EMobilePhoneGetBarringStatusPhase1, data);
	iMockLTSY.CompleteL(EMobilePhoneGetBarringStatusPhase1, KErrGeneral);
	iCBList->Start(activeRetriever->Status(), condition);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber())
	ASSERT_EQUALS(KErrGeneral, activeRetriever->Status().Int())
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B1: cover condition branch for KErrNotFound
 	//------------------------------------------------------------------------- 	

	iMockLTSY.ExpectL(EMobilePhoneGetBarringStatusPhase1, data);
	iMockLTSY.CompleteL(EMobilePhoneGetBarringStatusPhase1, KErrNone, KNullDesC8);
	iCBList->Start(activeRetriever->Status(), condition);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber())
	ASSERT_EQUALS(KErrNotFound, activeRetriever->Status().Int())
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneCBList::Start when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetBarringStatusPhase1, data);
	iMockLTSY.CompleteL(EMobilePhoneGetBarringStatusPhase1, KErrNone, dataComplete);
	iCBList->Start(activeRetriever->Status(), condition);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber())
	ASSERT_EQUALS(KErrNone, activeRetriever->Status().Int())
	CMobilePhoneCBList* resList = iCBList->RetrieveListL();
	CBufBase* resCBListBuf = resList->StoreLC();
	completeCBListBuf = cbList->StoreLC();
	ASSERT_EQUALS(completeCBListBuf->Ptr(0), resCBListBuf->Ptr(0))
	CleanupStack::PopAndDestroy(2); //resCBListBuf, completeCBListBuf
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CRetrieveMobilePhoneCBList::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(EMobilePhoneGetBarringStatusPhase1, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(EMobilePhoneGetBarringStatusPhase1, data, KErrNotSupported);
	iCBList->Start(activeRetriever->Status(), condition, RMobilePhone::EInfoLocationCache);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber())
	ASSERT_EQUALS(KErrNotFound, activeRetriever->Status().Int())
	
	CleanupStack::PopAndDestroy(3);
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(3, this); // data, this
	}
	
/**
@SYMTestCaseID BA-CTSY-CBAR-RMPCB-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrieveMobilePhoneCBList::Start
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrieveMobilePhoneCBList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestStart0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	OpenPhoneCBListLC();
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewLC(*iCBList);
	scheduler.AddRetrieverL(*activeRetriever);
	
	
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;

	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> conditionData(condition);
	conditionData.SerialiseL(data);
	
	CMobilePhoneCBList* cbList = CMobilePhoneCBList::NewL();
	CleanupStack::PushL(cbList);
	RMobilePhone::TMobilePhoneCBInfoEntryV1 listEntry;
	listEntry.iStatus = RMobilePhone::ECallBarringStatusActive;
	listEntry.iCondition = RMobilePhone::EBarAllIncoming;
	listEntry.iServiceGroup = RMobilePhone::EVoiceService;
	cbList->AddEntryL(listEntry);

	CBufBase* completeCBListBuf = cbList->StoreLC();
	dataComplete.Create(completeCBListBuf->Ptr(0));
	CleanupStack::PopAndDestroy(completeCBListBuf);

 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneCBList::Start
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.ExpectL(EMobilePhoneGetBarringStatusPhase1, data);
	iMockLTSY.CompleteL(EMobilePhoneGetBarringStatusPhase1, KErrNone, dataComplete, 20);
	iCBList->Start(activeRetriever->Status(), condition);
	activeRetriever->Activate();
	iCBList->Cancel();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->Status().Int())
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	AssertMockLtsyStatusL();

// Cancel the second phase
	iMockLTSY.ExpectL(EMobilePhoneGetBarringStatusPhase1, data);
	iMockLTSY.CompleteL(EMobilePhoneGetBarringStatusPhase1, KErrNone, dataComplete);
	iCBList->Start(activeRetriever->Status(), condition);
	activeRetriever->Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->Status().Int())
	
	AssertMockLtsyStatusL();


	CleanupStack::PopAndDestroy(3); 
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(3, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-CBAR-RMPCB-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneCBList::Start with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCBList::Start with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestStart0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	OpenPhoneCBListLC();
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewLC(*iCBList);
	scheduler.AddRetrieverL(*activeRetriever);
	
	
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllCases;

 	//-------------------------------------------------------------------------
	// Test passing wrong condition to CRetrieveMobilePhoneCBList::Start
 	//-------------------------------------------------------------------------
 	
	iCBList->Start(activeRetriever->Status(), condition);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrArgument, activeRetriever->Status().Int())
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(); 
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-CBAR-RMPCB-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to CRetrieveMobilePhoneCBList::Start
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to CRetrieveMobilePhoneCBList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestStart0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	OpenPhoneCBListLC();
	
	CRetrieveMobilePhoneCBList* retrieveCBList2 = CRetrieveMobilePhoneCBList::NewL(phone2);
	CleanupStack::PushL(retrieveCBList2);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewLC(*iCBList);
	scheduler.AddRetrieverL(*activeRetriever);
	
	CActiveRetriever* activeRetriever2 = CActiveRetriever::NewLC(*retrieveCBList2);
	scheduler.AddRetrieverL(*activeRetriever2);
	
	
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;

	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> conditionData(condition);
	conditionData.SerialiseL(data);
	
	CMobilePhoneCBList* cbList = CMobilePhoneCBList::NewL();
	CleanupStack::PushL(cbList);
	RMobilePhone::TMobilePhoneCBInfoEntryV1 listEntry;
	listEntry.iStatus = RMobilePhone::ECallBarringStatusActive;
	listEntry.iCondition = RMobilePhone::EBarAllIncoming;
	listEntry.iServiceGroup = RMobilePhone::EVoiceService;
	cbList->AddEntryL(listEntry);

	CBufBase* completeCBListBuf = cbList->StoreLC();
	dataComplete.Create(completeCBListBuf->Ptr(0));
	CleanupStack::PopAndDestroy(completeCBListBuf);
	
	RMobilePhone::TMobilePhoneCBCondition condition2 = RMobilePhone::EBarIncomingRoaming;

	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> conditionData2(condition2);
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting CRetrieveMobilePhoneCBList::Start
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetBarringStatusPhase1, data);
	iMockLTSY.CompleteL(EMobilePhoneGetBarringStatusPhase1, KErrNone, dataComplete);
	
	iCBList->Start(activeRetriever->Status(), condition);
	activeRetriever->Activate();
	
	retrieveCBList2->Start(activeRetriever2->Status(), condition2);
	activeRetriever2->Activate();
	
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->Status().Int())
	ASSERT_EQUALS(KErrServerBusy, activeRetriever2->Status().Int())
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(5); 
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CBAR-RMPCB-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneCBList::Start with timeout
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCBList::Start and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestStart0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	OpenPhoneCBListLC();
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewLC(*iCBList);
	scheduler.AddRetrieverL(*activeRetriever);
	
	
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;

	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> conditionData(condition);
	conditionData.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of CRetrieveMobilePhoneCBList::Start
 	//-------------------------------------------------------------------------

	
	iMockLTSY.ExpectL(EMobilePhoneGetBarringStatusPhase1, data);
	iCBList->Start(activeRetriever->Status(), condition);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrTimedOut, activeRetriever->Status().Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2);
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSSP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetSSPassword
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetSSPassword
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetSSPassword0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	_LIT(KOldPassword, "OldPwd");
	_LIT(KNewPassword, "NewPwd");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhonePasswordChangeV2 pwdChange;
	pwdChange.iOldPassword.Copy(KOldPassword);
	pwdChange.iNewPassword.Copy(KNewPassword);
	pwdChange.iVerifiedPassword.Copy(KNewPassword);
	TPckg<RMobilePhone::TMobilePhonePasswordChangeV2> password(pwdChange);
	TUint16 service = 0;
	
	TMockLtsyData2<RMobilePhone::TMobilePhonePasswordChangeV2, TUint16> ssData(pwdChange, service);
	ssData.SerialiseL(data);
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetSSPassword, data, KErrNotSupported);
	iPhone.SetSSPassword(reqStatus, password, service);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetSSPassword, data);
	iMockLTSY.CompleteL(EMobilePhoneSetSSPassword, KErrGeneral);
	iPhone.SetSSPassword(reqStatus, password, service);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetSSPassword when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetSSPassword, data);
	iMockLTSY.CompleteL(EMobilePhoneSetSSPassword, KErrNone);
	iPhone.SetSSPassword(reqStatus, password, service);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetSSPassword
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneSetSSPassword, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSSP-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SetSSPassword
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SetSSPassword
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetSSPassword0002L()
	{

// This test should test cancellation of SetSSPassword
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);


	_LIT(KOldPassword, "OldPwd");
	_LIT(KNewPassword, "NewPwd");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhonePasswordChangeV2 pwdChange;
	pwdChange.iOldPassword.Copy(KOldPassword);
	pwdChange.iNewPassword.Copy(KNewPassword);
	pwdChange.iVerifiedPassword.Copy(KNewPassword);
	TPckg<RMobilePhone::TMobilePhonePasswordChangeV2> password(pwdChange);
	TUint16 service = 0;
	
	TMockLtsyData2<RMobilePhone::TMobilePhonePasswordChangeV2, TUint16> ssData(pwdChange, service);
	ssData.SerialiseL(data);
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SetSSPassword
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.ExpectL(EMobilePhoneSetSSPassword, data);
	iMockLTSY.CompleteL(EMobilePhoneSetSSPassword, KErrNone, 20);
	iPhone.SetSSPassword(reqStatus, password, service);
	iPhone.CancelAsyncRequest(EMobilePhoneSetSSPassword);
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSSP-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetSSPassword with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetSSPassword with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetSSPassword0003L()
	{

// This test should test sending bad parameter data for SetSSPassword
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	_LIT(KOldPassword, "OldPwd");
	_LIT(KNewPassword, "NewPwd");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhonePasswordChangeV2 pwdChange;
	pwdChange.iOldPassword.Copy(KOldPassword);
	pwdChange.iNewPassword.Copy(KNewPassword);
	pwdChange.iVerifiedPassword.Copy(KNewPassword);
	TPckg<RMobilePhone::TMobilePhonePasswordChangeV2> password(pwdChange);
	TInt service = -666;
	
	TMockLtsyData2<RMobilePhone::TMobilePhonePasswordChangeV2, TInt> ssData(pwdChange, service);
	ssData.SerialiseL(data);
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMobilePhone::SetSSPassword
 	//-------------------------------------------------------------------------
 	
	iPhone.SetSSPassword(reqStatus, password, service);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int())


	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSSP-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetSSPassword
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetSSPassword
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetSSPassword0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	_LIT(KOldPassword, "OldPwd");
	_LIT(KNewPassword, "NewPwd");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhonePasswordChangeV2 pwdChange;
	pwdChange.iOldPassword.Copy(KOldPassword);
	pwdChange.iNewPassword.Copy(KNewPassword);
	pwdChange.iVerifiedPassword.Copy(KNewPassword);
	TPckg<RMobilePhone::TMobilePhonePasswordChangeV2> password(pwdChange);
	TUint16 service = 0;
	
	TMockLtsyData2<RMobilePhone::TMobilePhonePasswordChangeV2, TUint16> ssData(pwdChange, service);
	ssData.SerialiseL(data);
	
	
	_LIT(KOldPassword2, "OldPwd2");
	_LIT(KNewPassword2, "NewPwd2");
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhonePasswordChangeV2 pwdChange2;
	pwdChange2.iOldPassword.Copy(KOldPassword2);
	pwdChange2.iNewPassword.Copy(KNewPassword2);
	pwdChange2.iVerifiedPassword.Copy(KNewPassword2);
	TPckg<RMobilePhone::TMobilePhonePasswordChangeV2> password2(pwdChange2);
	TUint16 service2 = 1;
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetSSPassword
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetSSPassword, data);
	iMockLTSY.CompleteL(EMobilePhoneSetSSPassword, KErrNone);
	iPhone.SetSSPassword(reqStatus, password, service);
	phone2.SetSSPassword(reqStatus2, password2, service2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int())
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSSP-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetSSPassword with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetSSPassword and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetSSPassword0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	_LIT(KOldPassword, "OldPwd");
	_LIT(KNewPassword, "NewPwd");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhonePasswordChangeV2 pwdChange;
	pwdChange.iOldPassword.Copy(KOldPassword);
	pwdChange.iNewPassword.Copy(KNewPassword);
	pwdChange.iVerifiedPassword.Copy(KNewPassword);
	TPckg<RMobilePhone::TMobilePhonePasswordChangeV2> password(pwdChange);
	TUint16 service = 0;
	
	TMockLtsyData2<RMobilePhone::TMobilePhonePasswordChangeV2, TUint16> ssData(pwdChange, service);
	ssData.SerialiseL(data);
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetSSPassword
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetSSPassword, data);
	iPhone.SetSSPassword(reqStatus, password, service);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int())
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MGCMN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCompMethodName
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCompMethodName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestGetCompMethodName0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	
	TRequestStatus reqStatus;
	RBuf alphaTag;
	TUint8 cmi = 0;
	iPhone.GetCompMethodName(reqStatus, alphaTag, cmi);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSCBS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallBarringStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallBarringStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetCallBarringStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	_LIT(KPwd, "pass");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;
	RMobilePhone::TMobilePhoneCBChangeV1 info;
	info.iAction = RMobilePhone::EServiceActionActivate;
	info.iPassword.Copy(KPwd);
	info.iServiceGroup = RMobilePhone::EPacketDataService;
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneCBCondition, RMobilePhone::TMobilePhoneCBChangeV1> dataToSet(condition, info);
	dataToSet.SerialiseL(data);
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetCallBarringStatus, data, KErrNotSupported);
	iPhone.SetCallBarringStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetCallBarringStatus, data);
	iMockLTSY.CompleteL(EMobilePhoneSetCallBarringStatus, KErrGeneral);
	iPhone.SetCallBarringStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetCallBarringStatus when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetCallBarringStatus, data);
	iMockLTSY.CompleteL(EMobilePhoneSetCallBarringStatus, KErrNone);
	iPhone.SetCallBarringStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetCallBarringStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneSetCallBarringStatus, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSCBS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SetCallBarringStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SetCallBarringStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetCallBarringStatus0002L()
	{

// This test should test cancellation of SetCallBarringStatus
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	_LIT(KPwd, "pass");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;
	RMobilePhone::TMobilePhoneCBChangeV1 info;
	info.iAction = RMobilePhone::EServiceActionActivate;
	info.iPassword.Copy(KPwd);
	info.iServiceGroup = RMobilePhone::EPacketDataService;
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneCBCondition, RMobilePhone::TMobilePhoneCBChangeV1> dataToSet(condition, info);
	dataToSet.SerialiseL(data);
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SetCallBarringStatus
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.ExpectL(EMobilePhoneSetCallBarringStatus, data);
	iMockLTSY.CompleteL(EMobilePhoneSetCallBarringStatus, KErrNone, 20);
	iPhone.SetCallBarringStatus(reqStatus, condition, info);
	iPhone.CancelAsyncRequest(EMobilePhoneSetCallBarringStatus);
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSCBS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallBarringStatus with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallBarringStatus with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetCallBarringStatus0003L()
	{

// This test should test sending bad parameter data for SetCallBarringStatus
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	_LIT(KPwd, "password"); //The length of the password doesn't equal KMmTsyMaxLengthOfSSPassword.
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;
	RMobilePhone::TMobilePhoneCBChangeV1 info;
	info.iAction = RMobilePhone::EServiceActionActivate;
	info.iPassword.Copy(KPwd);
	info.iServiceGroup = RMobilePhone::EPacketDataService;
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::SetCallBarringStatus
 	//-------------------------------------------------------------------------

	iPhone.SetCallBarringStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGsmSSPasswordRegistrationFailure, reqStatus.Int())

//Covering conditions
	_LIT(KPwd2,"1234");
	info.iAction = RMobilePhone::EServiceActionInvoke;
	info.iPassword.Copy(KPwd2);
	iPhone.SetCallBarringStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int())
	
	info.iAction = RMobilePhone::EServiceActionActivate;
	condition = RMobilePhone::EBarAllIncomingServices;
	iPhone.SetCallBarringStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSCBS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetCallBarringStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetCallBarringStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetCallBarringStatus0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	_LIT(KPwd, "pass");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;
	RMobilePhone::TMobilePhoneCBChangeV1 info;
	info.iAction = RMobilePhone::EServiceActionActivate;
	info.iPassword.Copy(KPwd);
	info.iServiceGroup = RMobilePhone::EPacketDataService;
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneCBCondition, RMobilePhone::TMobilePhoneCBChangeV1> dataToSet(condition, info);
	dataToSet.SerialiseL(data);
	
	
	_LIT(KPwd2, "abcd");
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneCBCondition condition2 = RMobilePhone::EBarIncomingRoaming;
	RMobilePhone::TMobilePhoneCBChangeV1 info2;
	info2.iAction = RMobilePhone::EServiceActionInvoke;
	info2.iPassword.Copy(KPwd2);
	info2.iServiceGroup = RMobilePhone::EShortMessageService;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetCallBarringStatus
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetCallBarringStatus, data);
	iMockLTSY.CompleteL(EMobilePhoneSetCallBarringStatus, KErrNone);
	iPhone.SetCallBarringStatus(reqStatus, condition, info);
	phone2.SetCallBarringStatus(reqStatus2, condition2, info2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int())
	
	AssertMockLtsyStatusL();

	// To write this test:
	// If this API has parameters which have different versions, then each
	// client in this test should use a different version of parameter.
	// Otherwise, they should use the same version.
	// All versions of parameters should be used. i.e. If there are three
	// versions of a parameter, there should be three clients, one using
	// each one. If the parameter does not have different versions, use
	// two clients each using the same version.


	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSCBS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallBarringStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallBarringStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetCallBarringStatus0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	_LIT(KPwd, "pass");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;
	RMobilePhone::TMobilePhoneCBChangeV1 info;
	info.iAction = RMobilePhone::EServiceActionActivate;
	info.iPassword.Copy(KPwd);
	info.iServiceGroup = RMobilePhone::EPacketDataService;
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneCBCondition, RMobilePhone::TMobilePhoneCBChangeV1> dataToSet(condition, info);
	dataToSet.SerialiseL(data);
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetCallBarringStatus
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetCallBarringStatus, data);
	iPhone.SetCallBarringStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int())
	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MSCBP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallBarringPassword
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallBarringPassword
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestSetCallBarringPassword0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	_LIT(KOldPwd, "old");
	_LIT(KNewPwd, "new");
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhonePasswordChangeV1 password;
	password.iOldPassword.Copy(KOldPwd);
	password.iNewPassword.Copy(KNewPwd);
	
	iPhone.SetCallBarringPassword(reqStatus, password);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	
	// Test API is not supported

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MNCBSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCallBarringStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCallBarringStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestNotifyCallBarringStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCBCondition cond;
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> conditionData(condition);
	conditionData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyCallBarringStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifyCallBarringStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyCallBarringStatusChange when result is not cached.
 	//-------------------------------------------------------------------------
	iPhone.NotifyCallBarringStatusChange(reqStatus, cond);
	iMockLTSY.CompleteL(EMobilePhoneNotifyCallBarringStatusChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(condition, cond)
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MNCBSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyCallBarringStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyCallBarringStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestNotifyCallBarringStatusChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCBCondition cond;
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> conditionData(condition);
	conditionData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyCallBarringStatusChange
 	//-------------------------------------------------------------------------
 	
	iPhone.NotifyCallBarringStatusChange(reqStatus, cond);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallBarringStatusChange);
	iMockLTSY.CompleteL(EMobilePhoneNotifyCallBarringStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CBAR-MNCBSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyCallBarringStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyCallBarringStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallBarringFU::TestNotifyCallBarringStatusChange0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCBCondition cond;
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> conditionData(condition);
	conditionData.SerialiseL(data);
	
	
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneCBCondition cond2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyCallBarringStatusChange
 	//-------------------------------------------------------------------------
	
	iPhone.NotifyCallBarringStatusChange(reqStatus, cond);
	phone2.NotifyCallBarringStatusChange(reqStatus2, cond2);
	iMockLTSY.CompleteL(EMobilePhoneNotifyCallBarringStatusChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())
	ASSERT_EQUALS(condition, cond)
	ASSERT_EQUALS(condition, cond2)
	AssertMockLtsyStatusL();


	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

