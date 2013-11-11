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
// The TEFUnit test suite for CallCompletion in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsycallcompletionfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <mmretrieve.h>
#include "cctsyactiveretriever.h"

CTestSuite* CCTsyCallCompletionFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestRejectCCBS0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestRejectCCBS0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestRejectCCBS0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestActivateCCBS0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestActivateCCBS0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestActivateCCBS0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestGetCCBSStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestDeactivateCCBS0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestAcceptCCBSRecall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestNotifyCCBSRecall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestRefuseCCBSRecall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCompletionFU, TestNotifyCCBSStatusChange0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-CCOM-RMPCL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneCcbsList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCcbsList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneCcbsList::Start when result is not cached.
 	//-------------------------------------------------------------------------

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	CRetrieveMobilePhoneCcbsList* retrieveMobilePhoneCcbsList = CRetrieveMobilePhoneCcbsList::NewL(iPhone);    
	CleanupStack::PushL(retrieveMobilePhoneCcbsList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever activeMobilePhoneCcbsListRetriever(*retrieveMobilePhoneCcbsList);
	scheduler.AddRetrieverL(activeMobilePhoneCcbsListRetriever);

	retrieveMobilePhoneCcbsList->Start(activeMobilePhoneCcbsListRetriever.Status());
	activeMobilePhoneCcbsListRetriever.Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeMobilePhoneCcbsListRetriever.iStatus.Int());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1); // retrieveMobilePhoneCcbsList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-CRC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::RejectCCBS for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::RejectCCBS for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestRejectCCBS0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	err = mobileCall.RejectCCBS();
	ASSERT_EQUALS(KErrNotSupported, err)

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line

	err = line.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	err = mobileCall.RejectCCBS();
	ASSERT_EQUALS(KErrNotSupported, err)

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-CRC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::RejectCCBS for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::RejectCCBS for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestRejectCCBS0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	err = mobileCall.RejectCCBS();
	ASSERT_EQUALS(KErrNotSupported, err)

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-CRC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::RejectCCBS for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::RejectCCBS for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestRejectCCBS0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	err = mobileCall.RejectCCBS();
	ASSERT_EQUALS(KErrNotSupported, err)

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-CAC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::ActivateCCBS for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ActivateCCBS for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestActivateCCBS0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	TInt index(0);
	mobileCall.ActivateCCBS(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line

	err = line.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	mobileCall.ActivateCCBS(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-CAC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::ActivateCCBS for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ActivateCCBS for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestActivateCCBS0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	TInt index(0);
	mobileCall.ActivateCCBS(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-CAC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::ActivateCCBS for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ActivateCCBS for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestActivateCCBS0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	TInt index(0);
	mobileCall.ActivateCCBS(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-MGCCBSS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCCBSStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCCBSStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestGetCCBSStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCCBSStatus ccbsStatus(RMobilePhone::ECcbsActive);
	RMobilePhone::TMobileInfoLocation location(RMobilePhone::EInfoLocationCachePreferred);
	iPhone.GetCCBSStatus(reqStatus, ccbsStatus, location);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-MDCCBS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::DeactivateCCBS
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::DeactivateCCBS
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestDeactivateCCBS0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TInt index(0);
	iPhone.DeactivateCCBS(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-MACCBSR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::AcceptCCBSRecall
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AcceptCCBSRecall
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestAcceptCCBSRecall0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TInt index(0);
	TName callName;
	iPhone.AcceptCCBSRecall(reqStatus, index, callName);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-MNCCBSR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCCBSRecall
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCCBSRecall
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestNotifyCCBSRecall0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TBuf8<1> buf;
	iPhone.NotifyCCBSRecall(reqStatus, buf);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-MRCCBSR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::RefuseCCBSRecall
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::RefuseCCBSRecall
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestRefuseCCBSRecall0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TInt index(0);
	TInt err = iPhone.RefuseCCBSRecall(index);
	ASSERT_EQUALS(KErrNotSupported, err)

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOM-MNCCBSSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCCBSStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCCBSStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCompletionFU::TestNotifyCCBSStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCCBSStatus ccbsStatus(RMobilePhone::ECcbsActive);
	iPhone.NotifyCCBSStatusChange(reqStatus, ccbsStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}



