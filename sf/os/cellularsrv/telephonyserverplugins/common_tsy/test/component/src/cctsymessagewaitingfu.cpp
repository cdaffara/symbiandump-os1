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
// The TEFUnit test suite for MessageWaiting in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsymessagewaitingfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyMessageWaitingFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestNotifyMessageWaiting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestGetIccMessageWaitingIndicators0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestGetIccMessageWaitingIndicators0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestGetIccMessageWaitingIndicators0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestGetIccMessageWaitingIndicators0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestGetIccMessageWaitingIndicators0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestGetIccMessageWaitingIndicators0001aL);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestSetIccMessageWaitingIndicators0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestSetIccMessageWaitingIndicators0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestSetIccMessageWaitingIndicators0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestSetIccMessageWaitingIndicators0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestSetIccMessageWaitingIndicators0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestNotifyIccMessageWaitingIndicatorsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestNotifyIccMessageWaitingIndicatorsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestNotifyIccMessageWaitingIndicatorsChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMessageWaitingFU, TestNotifyIccMessageWaitingIndicatorsChange0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-MSGW-MCMW-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyMessageWaiting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyMessageWaiting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestNotifyMessageWaiting0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TInt count;
	
	//Unsupported CDMA features
	iPhone.NotifyMessageWaiting(reqStatus, count);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MGIMWI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIccMessageWaitingIndicators
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIccMessageWaitingIndicators
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestGetIccMessageWaitingIndicators0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNotSupported);

	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators(messageWaitingIndicators);
	iPhone.GetIccMessageWaitingIndicators(reqStatus, msgIndicators);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetIccMessageWaitingIndicators when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);
	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
	expectedMessageIndicators.iDisplayStatus = 1;
	expectedMessageIndicators.iVoiceMsgs = 2;
	expectedMessageIndicators.iAuxVoiceMsgs = 3;
	expectedMessageIndicators.iDataMsgs = 4;
	expectedMessageIndicators.iFaxMsgs = 5;
	expectedMessageIndicators.iEmailMsgs = 6;
	expectedMessageIndicators.iOtherMsgs = 7;
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);

	iPhone.GetIccMessageWaitingIndicators(reqStatus, msgIndicators);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, messageWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     messageWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  messageWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      messageWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       messageWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     messageWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     messageWaitingIndicators.iOtherMsgs);

 	//-------------------------------------------------------------------------
	// TEST: Increase coverage
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);

	iPhone.GetIccMessageWaitingIndicators(reqStatus, msgIndicators);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, messageWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     messageWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  messageWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      messageWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       messageWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     messageWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     messageWaitingIndicators.iOtherMsgs);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrGeneral, data);
	iPhone.GetIccMessageWaitingIndicators(reqStatus, msgIndicators);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, messageWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     messageWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  messageWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      messageWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       messageWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     messageWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     messageWaitingIndicators.iOtherMsgs);

 	//-------------------------------------------------------------------------
	// TEST: Increase coverage
 	//-------------------------------------------------------------------------
	iPhone.GetIccMessageWaitingIndicators(reqStatus, msgIndicators);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, messageWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     messageWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  messageWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      messageWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       messageWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     messageWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     messageWaitingIndicators.iOtherMsgs);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetIccMessageWaitingIndicators
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(reqStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);
	// wait for completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MGIMWI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetIccMessageWaitingIndicators
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetIccMessageWaitingIndicators
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestGetIccMessageWaitingIndicators0002L()
	{

// This test should test cancellation of GetIccMessageWaitingIndicators
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	TRequestStatus reqStatus;
 	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetIccMessageWaitingIndicators
 	//------------------------------------------------------------------------- 	
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);
	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data, 10);

	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators(messageWaitingIndicators);
	iPhone.GetIccMessageWaitingIndicators(reqStatus, msgIndicators);

	iPhone.CancelAsyncRequest(EMobilePhoneGetIccMessageWaitingIndicators);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MGIMWI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIccMessageWaitingIndicators with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIccMessageWaitingIndicators with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestGetIccMessageWaitingIndicators0003L()
	{

// This test should test sending bad parameter data for GetIccMessageWaitingIndicators
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	TRequestStatus reqStatus;


	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobilePhone::GetIccMessageWaitingIndicators
 	//-------------------------------------------------------------------------

	TInt wrongMsgIndicators;
	TPckg<TInt> wrongMsgIndicatorsPckg(wrongMsgIndicators);
	iPhone.GetIccMessageWaitingIndicators(reqStatus, wrongMsgIndicatorsPckg);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetIccMessageWaitingIndicators
 	//-------------------------------------------------------------------------

	TBuf8<1> buf;
	iPhone.GetIccMessageWaitingIndicators(reqStatus, buf);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	
	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MGIMWI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetIccMessageWaitingIndicators
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetIccMessageWaitingIndicators
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestGetIccMessageWaitingIndicators0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetIccMessageWaitingIndicators
 	//-------------------------------------------------------------------------
	// setting and execute 1st request
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);
	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);

	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators(messageWaitingIndicators);
 	TRequestStatus reqStatus;
	iPhone.GetIccMessageWaitingIndicators(reqStatus, msgIndicators);

	// setting and execute 2nd request
	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators2;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators2(messageWaitingIndicators2);
 	TRequestStatus reqStatus2;
	phone2.GetIccMessageWaitingIndicators(reqStatus2, msgIndicators2);

	// wait for completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, messageWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     messageWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  messageWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      messageWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       messageWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     messageWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     messageWaitingIndicators.iOtherMsgs);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, messageWaitingIndicators2.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     messageWaitingIndicators2.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  messageWaitingIndicators2.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      messageWaitingIndicators2.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       messageWaitingIndicators2.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     messageWaitingIndicators2.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     messageWaitingIndicators2.iOtherMsgs);

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MGIMWI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIccMessageWaitingIndicators with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIccMessageWaitingIndicators and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestGetIccMessageWaitingIndicators0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetIccMessageWaitingIndicators
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);

	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators(messageWaitingIndicators);
 	TRequestStatus reqStatus;
	iPhone.GetIccMessageWaitingIndicators(reqStatus, msgIndicators);

	// wait for completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, messageWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     messageWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  messageWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      messageWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       messageWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     messageWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     messageWaitingIndicators.iOtherMsgs);

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this

	}

/**
@SYMTestCaseID          BA-CTSY-MSGW-MGIMWI-0001a
@SYMCreationDate        01/04/2009
@SYMAuthor              manasendun
@SYMTestStatus          3. Released
@SYMTestPriority        Medium
@SYMTestType            CT
@SYMTestCaseDesc        Test for RMobilePhone::GetIccMessageWaitingIndicators when RMobilePhone::SetIccMessageWaitingIndicators request is active.
@SYMTestExpectedResults Pass.
@SYMTestActions         Invokes RMobilePhone::GetIccMessageWaitingIndicators when RMobilePhone::SetIccMessageWaitingIndicators request is active.
*/
void CCTsyMessageWaitingFU::TestGetIccMessageWaitingIndicators0001aL()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators1;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
		indicatorsData(messageWaitingIndicators1);
	messageWaitingIndicators1.iDisplayStatus = 1;
	messageWaitingIndicators1.iVoiceMsgs = 2;
	messageWaitingIndicators1.iAuxVoiceMsgs = 3;
	messageWaitingIndicators1.iDataMsgs = 4;
	messageWaitingIndicators1.iFaxMsgs = 5;
	messageWaitingIndicators1.iEmailMsgs = 6;
	messageWaitingIndicators1.iOtherMsgs = 7;
	indicatorsData.SerialiseL(data); 

	iMockLTSY.ExpectL(EMobilePhoneSetIccMessageWaitingIndicators, data, KErrNone);

	//Send Set Request
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators1(messageWaitingIndicators1);
	iPhone.SetIccMessageWaitingIndicators(reqStatus, msgIndicators1);

	//Send Get Request
	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators2;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators2(messageWaitingIndicators2);
 	TRequestStatus reqStatus2;
	phone2.GetIccMessageWaitingIndicators(reqStatus2, msgIndicators2);

	//Complete the set request after receiving the get request in CTSY.
	//But the get request will never go to LTSY. When Set request completes,
	//the get request should complete with the set value.
	iMockLTSY.CompleteL(EMobilePhoneSetIccMessageWaitingIndicators, KErrNone);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(messageWaitingIndicators1.iDisplayStatus, messageWaitingIndicators2.iDisplayStatus);
	ASSERT_EQUALS(messageWaitingIndicators1.iVoiceMsgs,     messageWaitingIndicators2.iVoiceMsgs);
	ASSERT_EQUALS(messageWaitingIndicators1.iAuxVoiceMsgs,  messageWaitingIndicators2.iAuxVoiceMsgs);
	ASSERT_EQUALS(messageWaitingIndicators1.iDataMsgs,      messageWaitingIndicators2.iDataMsgs);
	ASSERT_EQUALS(messageWaitingIndicators1.iFaxMsgs,       messageWaitingIndicators2.iFaxMsgs);
	ASSERT_EQUALS(messageWaitingIndicators1.iEmailMsgs,     messageWaitingIndicators2.iEmailMsgs);
	ASSERT_EQUALS(messageWaitingIndicators1.iOtherMsgs,     messageWaitingIndicators2.iOtherMsgs);

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-MSGW-MSIMWI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetIccMessageWaitingIndicators
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetIccMessageWaitingIndicators
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestSetIccMessageWaitingIndicators0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(messageWaitingIndicators);
	messageWaitingIndicators.iDisplayStatus = 1;
	messageWaitingIndicators.iVoiceMsgs = 2;
	messageWaitingIndicators.iAuxVoiceMsgs = 3;
	messageWaitingIndicators.iDataMsgs = 4;
	messageWaitingIndicators.iFaxMsgs = 5;
	messageWaitingIndicators.iEmailMsgs = 6;
	messageWaitingIndicators.iOtherMsgs = 7;
    indicatorsData.SerialiseL(data); 
	iMockLTSY.ExpectL(EMobilePhoneSetIccMessageWaitingIndicators, data, KErrNotSupported);

	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators(messageWaitingIndicators);
	TRequestStatus reqStatus;
	iPhone.SetIccMessageWaitingIndicators(reqStatus, msgIndicators);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetIccMessageWaitingIndicators when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetIccMessageWaitingIndicators, data, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneSetIccMessageWaitingIndicators, KErrNone);

	iPhone.SetIccMessageWaitingIndicators(reqStatus, msgIndicators);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetIccMessageWaitingIndicators, data, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneSetIccMessageWaitingIndicators, KErrGeneral);

	iPhone.SetIccMessageWaitingIndicators(reqStatus, msgIndicators);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: Increase coverage
 	//-------------------------------------------------------------------------
	iPhone.SetIccMessageWaitingIndicators(reqStatus, msgIndicators);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetIccMessageWaitingIndicators
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(reqStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneSetIccMessageWaitingIndicators, KErrNone);
	// wait for completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MSIMWI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SetIccMessageWaitingIndicators
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SetIccMessageWaitingIndicators
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestSetIccMessageWaitingIndicators0002L()
	{

// This test should test cancellation of SetIccMessageWaitingIndicators
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SetIccMessageWaitingIndicators
 	//------------------------------------------------------------------------- 
 	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(messageWaitingIndicators);
	messageWaitingIndicators.iDisplayStatus = 1;
	messageWaitingIndicators.iVoiceMsgs = 2;
	messageWaitingIndicators.iAuxVoiceMsgs = 3;
	messageWaitingIndicators.iDataMsgs = 4;
	messageWaitingIndicators.iFaxMsgs = 5;
	messageWaitingIndicators.iEmailMsgs = 6;
	messageWaitingIndicators.iOtherMsgs = 7;
    indicatorsData.SerialiseL(data); 
	iMockLTSY.ExpectL(EMobilePhoneSetIccMessageWaitingIndicators, data, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneSetIccMessageWaitingIndicators, KErrNone, 10);

	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators(messageWaitingIndicators);
	TRequestStatus reqStatus;
	iPhone.SetIccMessageWaitingIndicators(reqStatus, msgIndicators);

	iPhone.CancelAsyncRequest(EMobilePhoneSetIccMessageWaitingIndicators);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MSIMWI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetIccMessageWaitingIndicators with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetIccMessageWaitingIndicators with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestSetIccMessageWaitingIndicators0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::SetIccMessageWaitingIndicators
 	//-------------------------------------------------------------------------
	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	messageWaitingIndicators.iDisplayStatus = 1;
	messageWaitingIndicators.iVoiceMsgs = 0;
	messageWaitingIndicators.iAuxVoiceMsgs = 0;
	messageWaitingIndicators.iDataMsgs = 0;
	messageWaitingIndicators.iFaxMsgs = 0;
	messageWaitingIndicators.iEmailMsgs = 0;
	messageWaitingIndicators.iOtherMsgs = 0;

	TInt wrongMsgIndicators;
	memcpy(&wrongMsgIndicators, &messageWaitingIndicators, sizeof(wrongMsgIndicators));
	TPckg<TInt> wrongMsgIndicatorsPckg(wrongMsgIndicators);
	iPhone.SetIccMessageWaitingIndicators(reqStatus, wrongMsgIndicatorsPckg);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MSIMWI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetIccMessageWaitingIndicators
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetIccMessageWaitingIndicators
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestSetIccMessageWaitingIndicators0004L()
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

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetIccMessageWaitingIndicators
	//-------------------------------------------------------------------------
	// setting and execute 1st request
	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(messageWaitingIndicators);
	messageWaitingIndicators.iDisplayStatus = 1;
	messageWaitingIndicators.iVoiceMsgs = 0;
	messageWaitingIndicators.iAuxVoiceMsgs = 0;
	messageWaitingIndicators.iDataMsgs = 0;
	messageWaitingIndicators.iFaxMsgs = 0;
	messageWaitingIndicators.iEmailMsgs = 0;
	messageWaitingIndicators.iOtherMsgs = 0;
    indicatorsData.SerialiseL(data); 
	iMockLTSY.ExpectL(EMobilePhoneSetIccMessageWaitingIndicators, data, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneSetIccMessageWaitingIndicators, KErrNone);

	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators(messageWaitingIndicators);
 	TRequestStatus reqStatus;
	iPhone.SetIccMessageWaitingIndicators(reqStatus, msgIndicators);

	// setting and execute 2nd request
	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators2;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators2(messageWaitingIndicators2);
 	TRequestStatus reqStatus2;
	phone2.SetIccMessageWaitingIndicators(reqStatus2, msgIndicators2);

	// wait for completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MSIMWI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetIccMessageWaitingIndicators with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetIccMessageWaitingIndicators and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestSetIccMessageWaitingIndicators0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetIccMessageWaitingIndicators
 	//-------------------------------------------------------------------------
 
 	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(messageWaitingIndicators);
	messageWaitingIndicators.iDisplayStatus = 1;
	messageWaitingIndicators.iVoiceMsgs = 2;
	messageWaitingIndicators.iAuxVoiceMsgs = 3;
	messageWaitingIndicators.iDataMsgs = 4;
	messageWaitingIndicators.iFaxMsgs = 5;
	messageWaitingIndicators.iEmailMsgs = 6;
	messageWaitingIndicators.iOtherMsgs = 7;
    indicatorsData.SerialiseL(data); 
	iMockLTSY.ExpectL(EMobilePhoneSetIccMessageWaitingIndicators, data, KErrNone);

	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators(messageWaitingIndicators);
	TRequestStatus reqStatus;
	iPhone.SetIccMessageWaitingIndicators(reqStatus, msgIndicators);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MNIMWC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestNotifyIccMessageWaitingIndicatorsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
	// from LTSY.
 	//-------------------------------------------------------------------------

 	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus);	

	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
	expectedMessageIndicators.iDisplayStatus = 111;
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange, KErrNone, data);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyIccMessageWaitingIndicatorsChange when result is not cached.
 	//-------------------------------------------------------------------------

	// send notify request
	RMobilePhone::TMobilePhoneMessageWaitingV1 notifyWaitingIndicators;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg notifyMsgIndicators(notifyWaitingIndicators);
	iPhone.NotifyIccMessageWaitingIndicatorsChange(reqStatus, notifyMsgIndicators);

	// setting and execute get request
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);
	expectedMessageIndicators.iDisplayStatus = 1;
	expectedMessageIndicators.iVoiceMsgs = 2;
	expectedMessageIndicators.iAuxVoiceMsgs = 3;
	expectedMessageIndicators.iDataMsgs = 4;
	expectedMessageIndicators.iFaxMsgs = 5;
	expectedMessageIndicators.iEmailMsgs = 6;
	expectedMessageIndicators.iOtherMsgs = 7;
    data.Close();
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);
	RMobilePhone::TMobilePhoneMessageWaitingV1 messageWaitingIndicators;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicators(messageWaitingIndicators);
	TRequestStatus mockLtsyStatus;
	iPhone.GetIccMessageWaitingIndicators(mockLtsyStatus, msgIndicators);
	
	// wait for get request completion and check results
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, messageWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     messageWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  messageWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      messageWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       messageWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     messageWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     messageWaitingIndicators.iOtherMsgs);

	// wait for notify request completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, notifyWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     notifyWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  notifyWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      notifyWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       notifyWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     notifyWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     notifyWaitingIndicators.iOtherMsgs);

 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::NotifyIccMessageWaitingIndicatorsChange again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	// send notify request
	iPhone.NotifyIccMessageWaitingIndicatorsChange(reqStatus, notifyMsgIndicators);

	// setting and execute set request
	expectedMessageIndicators.iDisplayStatus = 8;
	expectedMessageIndicators.iVoiceMsgs = 7;
	expectedMessageIndicators.iAuxVoiceMsgs = 6;
	expectedMessageIndicators.iDataMsgs = 5;
	expectedMessageIndicators.iFaxMsgs = 4;
	expectedMessageIndicators.iEmailMsgs = 3;
	expectedMessageIndicators.iOtherMsgs = 2;
    data.Close();
    indicatorsData.SerialiseL(data); 
	iMockLTSY.ExpectL(EMobilePhoneSetIccMessageWaitingIndicators, data, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneSetIccMessageWaitingIndicators, KErrNone);
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg expectedMsgIndicators(expectedMessageIndicators);
	iPhone.SetIccMessageWaitingIndicators(mockLtsyStatus, expectedMsgIndicators);
	
	// wait for set request completion and check results
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	// wait for notify request completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, notifyWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     notifyWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  notifyWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      notifyWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       notifyWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     notifyWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     notifyWaitingIndicators.iOtherMsgs);

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MNIMWC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestNotifyIccMessageWaitingIndicatorsChange0002L()
	{

// This test should test cancellation of NotifyIccMessageWaitingIndicatorsChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
 	//-------------------------------------------------------------------------
 	
	// send notify request
	RMobilePhone::TMobilePhoneMessageWaitingV1 notifyWaitingIndicators;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg notifyMsgIndicators(notifyWaitingIndicators);
 	TRequestStatus reqStatus;
	iPhone.NotifyIccMessageWaitingIndicatorsChange(reqStatus, notifyMsgIndicators);

	// invoke notify request completion
	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange, KErrNone, data, 10);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange);

	// wait for canceling notify request completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MNIMWC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyIccMessageWaitingIndicatorsChange with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyIccMessageWaitingIndicatorsChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestNotifyIccMessageWaitingIndicatorsChange0003L()
	{

// This test should test sending bad parameter data for NotifyIccMessageWaitingIndicatorsChange
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
 	//-------------------------------------------------------------------------

	TInt wrongMsgIndicators;
	TPckg<TInt> wrongMsgIndicatorsPckg(wrongMsgIndicators);
 	TRequestStatus reqStatus;
	iPhone.NotifyIccMessageWaitingIndicatorsChange(reqStatus, wrongMsgIndicatorsPckg);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	
	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-MSGW-MNIMWC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMessageWaitingFU::TestNotifyIccMessageWaitingIndicatorsChange0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
 	//-------------------------------------------------------------------------

	// send 1st notify request
	RMobilePhone::TMobilePhoneMessageWaitingV1 notifyWaitingIndicators;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg notifyMsgIndicators(notifyWaitingIndicators);
 	TRequestStatus reqStatus;
	iPhone.NotifyIccMessageWaitingIndicatorsChange(reqStatus, notifyMsgIndicators);

	// send 2nd notify request
	RMobilePhone::TMobilePhoneMessageWaitingV1 notifyWaitingIndicators2;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg notifyMsgIndicators2(notifyWaitingIndicators2);
 	TRequestStatus reqStatus2;
	phone2.NotifyIccMessageWaitingIndicatorsChange(reqStatus2, notifyMsgIndicators2);

	// invoke notify request completion
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
	expectedMessageIndicators.iDisplayStatus = 18;
	expectedMessageIndicators.iVoiceMsgs = 17;
	expectedMessageIndicators.iAuxVoiceMsgs = 16;
	expectedMessageIndicators.iDataMsgs = 15;
	expectedMessageIndicators.iFaxMsgs = 14;
	expectedMessageIndicators.iEmailMsgs = 13;
	expectedMessageIndicators.iOtherMsgs = 12;
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	// wait for notify requests completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, notifyWaitingIndicators.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     notifyWaitingIndicators.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  notifyWaitingIndicators.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      notifyWaitingIndicators.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       notifyWaitingIndicators.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     notifyWaitingIndicators.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     notifyWaitingIndicators.iOtherMsgs);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(expectedMessageIndicators.iDisplayStatus, notifyWaitingIndicators2.iDisplayStatus);
	ASSERT_EQUALS(expectedMessageIndicators.iVoiceMsgs,     notifyWaitingIndicators2.iVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iAuxVoiceMsgs,  notifyWaitingIndicators2.iAuxVoiceMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iDataMsgs,      notifyWaitingIndicators2.iDataMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iFaxMsgs,       notifyWaitingIndicators2.iFaxMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iEmailMsgs,     notifyWaitingIndicators2.iEmailMsgs);
	ASSERT_EQUALS(expectedMessageIndicators.iOtherMsgs,     notifyWaitingIndicators2.iOtherMsgs);

	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}
