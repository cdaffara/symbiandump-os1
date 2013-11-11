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
// The TEFUnit test suite for UserNetworkAccess in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyusernetworkaccessfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyUserNetworkAccessFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestGetFeatureCode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestProgramFeatureCode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequest0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequest0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequest0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequest0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequest0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestNotifySendNetworkServiceRequest0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestNotifySendNetworkServiceRequest0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestNotifySendNetworkServiceRequest0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestNotifySendNetworkServiceRequest0004L);	
    ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequestNoFdnCheck0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequestNoFdnCheck0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequestNoFdnCheck0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequestNoFdnCheck0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyUserNetworkAccessFU, TestSendNetworkServiceRequestNoFdnCheck0005L);


	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-USNA-MGFC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetFeatureCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetFeatureCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestGetFeatureCode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TBuf<1> fcString;
	RMobilePhone::TMobilePhoneNetworkService service(RMobilePhone::ENetworkServiceUnspecified);
	RMobilePhone::TMobilePhoneServiceAction action(RMobilePhone::EServiceActionUnspecified);
	iPhone.GetFeatureCode(reqStatus, fcString, service, action);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-USNA-MPFC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ProgramFeatureCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ProgramFeatureCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestProgramFeatureCode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TBuf<1> fcString;
	RMobilePhone::TMobilePhoneNetworkService service(RMobilePhone::ENetworkServiceUnspecified);
	RMobilePhone::TMobilePhoneServiceAction action(RMobilePhone::EServiceActionUnspecified);
	iPhone.ProgramFeatureCode(reqStatus, fcString, service, action);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-USNA-MSNSR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendNetworkServiceRequest
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendNetworkServiceRequest
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequest0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TInfoName expServiceString(_L("barracuda"));
 	TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequest, data, KErrNotSupported);

	TRequestStatus reqStatus;
	iPhone.SendNetworkServiceRequest(reqStatus, expServiceString);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequest, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequest, KErrGeneral);

	iPhone.SendNetworkServiceRequest(reqStatus, expServiceString);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SendNetworkServiceRequest when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequest, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequest, KErrNone);

	iPhone.SendNetworkServiceRequest(reqStatus, expServiceString);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SendNetworkServiceRequest
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequest, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-USNA-MSNSR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SendNetworkServiceRequest
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SendNetworkServiceRequest
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequest0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SendNetworkServiceRequest
 	//-------------------------------------------------------------------------
 	
	TInfoName expServiceString(_L("barracuda"));
 	TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequest, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequest, KErrNone, 10);

	TRequestStatus reqStatus;
	iPhone.SendNetworkServiceRequest(reqStatus, expServiceString);
	
	iPhone.CancelAsyncRequest(EMobilePhoneSendNetworkServiceRequest);
	
	User::WaitForRequest(reqStatus);
	// there is no cancel for this ipc in ctsy
	// so completing with KErrNone status is a normal behaviour
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-USNA-MSNSR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendNetworkServiceRequest with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendNetworkServiceRequest with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequest0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::SendNetworkServiceRequest
 	//-------------------------------------------------------------------------

	TInfoName expServiceString(KNullDesC);
 	TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequest, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequest, KErrNone);

	TRequestStatus reqStatus;
	iPhone.SendNetworkServiceRequest(reqStatus, KNullDesC);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-USNA-MSNSR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SendNetworkServiceRequest
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SendNetworkServiceRequest
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequest0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
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
	// Test A: Test multiple clients requesting RMobilePhone::SendNetworkServiceRequest
 	//-------------------------------------------------------------------------

	// setting and execute 1st request
	TInfoName expServiceString(_L("barracuda"));
 	TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequest, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequest, KErrNone, 10);

	TRequestStatus reqStatus;
	iPhone.SendNetworkServiceRequest(reqStatus, expServiceString);

	// setting and execute 2nd request
	TRequestStatus reqStatus2;
	_LIT(KServiceString2, "cudabarra");
	phone2.SendNetworkServiceRequest(reqStatus2, KServiceString2);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-USNA-MSNSR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendNetworkServiceRequest with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendNetworkServiceRequest and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequest0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SendNetworkServiceRequest
 	//-------------------------------------------------------------------------

	TInfoName expServiceString(_L("barracuda"));
 	TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequest, data);

	TRequestStatus reqStatus;
	iPhone.SendNetworkServiceRequest(reqStatus, expServiceString);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-USNA-MNSNSR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySendNetworkServiceRequest
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySendNetworkServiceRequest
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestNotifySendNetworkServiceRequest0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// prepare test data
	
	TRequestStatus reqStatus;
	TRequestStatus mockLtsyStatus;
	RMobilePhone::TMobilePhoneNotifySendSSOperation operation(RMobilePhone::ESendSSInvoke);

	RMobilePhone::TMobilePhoneSendSSRequestV3 mockSSReqData;
	mockSSReqData.iOpCode = RMobilePhone::ESendSSInvoke;
	TMockLtsyData1<RMobilePhone::TMobilePhoneSendSSRequestV3> mockData(mockSSReqData);
	
	RBuf8 mockDataBuf;
	CleanupClosePushL(mockDataBuf); // PUSH
	mockData.SerialiseL(mockDataBuf);

	RMobilePhone::TMobilePhoneSendSSRequestV3 actSSReqData;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg actSSReqDataPckg(actSSReqData);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iPhone.NotifySendNetworkServiceRequest(reqStatus, operation, actSSReqDataPckg);
	
	iMockLTSY.CompleteL(ECtsyPhoneSendNetworkServiceRequestInd, KErrGeneral, mockDataBuf);
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
 	
 	AssertMockLtsyStatusL();
 	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifySendNetworkServiceRequest when result is not cached.
 	//-------------------------------------------------------------------------

	iPhone.NotifySendNetworkServiceRequest(reqStatus, operation, actSSReqDataPckg);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(ECtsyPhoneSendNetworkServiceRequestInd, KErrNone, mockDataBuf);
	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(actSSReqData.iOpCode, mockSSReqData.iOpCode);
		
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifySendNetworkServiceRequest
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECtsyPhoneSendNetworkServiceRequestInd, KErrNone);
	
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // mockDataBuf, this
	}

/**
@SYMTestCaseID BA-CTSY-USNA-MNSNSR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifySendNetworkServiceRequest
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifySendNetworkServiceRequest
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestNotifySendNetworkServiceRequest0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();


	// prepare test data
	
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNotifySendSSOperation operation(RMobilePhone::ESendSSInvoke);

	RMobilePhone::TMobilePhoneSendSSRequestV3 mockSSReqData;
	mockSSReqData.iOpCode = RMobilePhone::ESendSSInvoke;
	TMockLtsyData1<RMobilePhone::TMobilePhoneSendSSRequestV3> mockData(mockSSReqData);
	
	RBuf8 mockDataBuf;
	CleanupClosePushL(mockDataBuf); // PUSH
	mockData.SerialiseL(mockDataBuf);

	RMobilePhone::TMobilePhoneSendSSRequestV3 actSSReqData;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg actSSReqDataPckg(actSSReqData);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifySendNetworkServiceRequest
 	//-------------------------------------------------------------------------
	iPhone.NotifySendNetworkServiceRequest(reqStatus, operation, actSSReqDataPckg);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifySendNetworkServiceRequest);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(ECtsyPhoneSendNetworkServiceRequestInd, KErrNone, mockDataBuf, 10);
	
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(reqStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());

    CleanupStack::PopAndDestroy(2, this); // mockDataBuf, this
	}

/**
@SYMTestCaseID BA-CTSY-USNA-MNSNSR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySendNetworkServiceRequest with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySendNetworkServiceRequest with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestNotifySendNetworkServiceRequest0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// prepare test data
	RMobilePhone::TMobilePhoneNotifySendSSOperation operation(RMobilePhone::ESendSSInvoke);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::NotifySendNetworkServiceRequest
 	//-------------------------------------------------------------------------

	TBuf8<1> smallDes;
	TRequestStatus reqStatus;
	iPhone.NotifySendNetworkServiceRequest(reqStatus, operation, smallDes);
	
    User::WaitForRequest(reqStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());

	CleanupStack::PopAndDestroy(this); // this
	}


/**
@SYMTestCaseID BA-CTSY-USNA-MNSNSR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifySendNetworkServiceRequest
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifySendNetworkServiceRequest
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestNotifySendNetworkServiceRequest0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();


	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2); // PUSH

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2); // PUSH

	
	// prepare test data
	
	TRequestStatus reqStatus1;

	RMobilePhone::TMobilePhoneNotifySendSSOperation operation(RMobilePhone::ESendSSInvoke);
	RMobilePhone::TMobilePhoneSendSSRequestV3 actSSReqData1;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg actSSReqData1Pckg(actSSReqData1);
	RMobilePhone::TMobilePhoneSendSSRequestV3 mockSSReqData1;
	mockSSReqData1.iOpCode = RMobilePhone::ESendSSInvoke;
	TMockLtsyData1<RMobilePhone::TMobilePhoneSendSSRequestV3> mockData1(mockSSReqData1);
	
	RBuf8 mockDataBuf1;
	CleanupClosePushL(mockDataBuf1); // PUSH
	mockData1.SerialiseL(mockDataBuf1);

	
	TRequestStatus reqStatus2;

	RMobilePhone::TMobilePhoneSendSSRequestV3 actSSReqData2;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg actSSReqData2Pckg(actSSReqData2);
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 mockSSReqData2;
	mockSSReqData2.iOpCode = RMobilePhone::ESendSSInvoke;
	TMockLtsyData1<RMobilePhone::TMobilePhoneSendSSRequestV3> mockData2(mockSSReqData2);
	
	RBuf8 mockDataBuf2;
	CleanupClosePushL(mockDataBuf2); // PUSH
	mockData2.SerialiseL(mockDataBuf2);

	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifySendNetworkServiceRequest
 	//-------------------------------------------------------------------------

	iPhone.NotifySendNetworkServiceRequest(reqStatus1, operation, actSSReqData1Pckg);
	phone2.NotifySendNetworkServiceRequest(reqStatus2, operation, actSSReqData2Pckg);
    ASSERT_EQUALS(KRequestPending, reqStatus1.Int());
    ASSERT_EQUALS(KRequestPending, reqStatus2.Int());
    
	iMockLTSY.CompleteL(ECtsyPhoneSendNetworkServiceRequestInd, KErrNone, mockDataBuf1);
	iMockLTSY.CompleteL(ECtsyPhoneSendNetworkServiceRequestInd, KErrNone, mockDataBuf2);
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	User::WaitForRequest(mockLtsyStatus);

	User::WaitForRequest(reqStatus1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, reqStatus1.Int());
	ASSERT_EQUALS(actSSReqData1.iOpCode, mockSSReqData1.iOpCode);
	
	User::WaitForRequest(reqStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(actSSReqData2.iOpCode, mockSSReqData2.iOpCode);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
	// Done !
	CleanupStack::PopAndDestroy(5, this);
	}



/**
@SYMTestCaseID BA-CTSY-USNA-MSNSRNFC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendNetworkServiceRequestNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendNetworkServiceRequestNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequestNoFdnCheck0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    TInfoName expServiceString(_L("barracuda"));
    TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, data, KErrNotSupported);

    TRequestStatus reqStatus;
    iPhone.SendNetworkServiceRequestNoFdnCheck(reqStatus, expServiceString);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, KErrGeneral);

    iPhone.SendNetworkServiceRequestNoFdnCheck(reqStatus, expServiceString);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobilePhone::SendNetworkServiceRequestNoFdnCheck when result is not cached.
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, KErrNone);

    iPhone.SendNetworkServiceRequestNoFdnCheck(reqStatus, expServiceString);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobilePhone::SendNetworkServiceRequestNoFdnCheck
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus); 
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, KErrNone);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(2, this); // data, this
    
    }


/**
@SYMTestCaseID BA-CTSY-USNA-MSNSRNFC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SendNetworkServiceRequestNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SendNetworkServiceRequestNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequestNoFdnCheck0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    RBuf8 data;
    CleanupClosePushL(data);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobilePhone::SendNetworkServiceRequestNoFdnCheck
    //-------------------------------------------------------------------------
    
    TInfoName expServiceString(_L("barracuda"));
    TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, KErrNone, 10);

    TRequestStatus reqStatus;
    iPhone.SendNetworkServiceRequestNoFdnCheck(reqStatus, expServiceString);
    
    iPhone.CancelAsyncRequest(EMobilePhoneSendNetworkServiceRequestNoFdnCheck);
    
    User::WaitForRequest(reqStatus);
    // there is no cancel for this ipc in ctsy
    // so completing with KErrNone status is a normal behaviour
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2); // data, this
    
    }


/**
@SYMTestCaseID BA-CTSY-USNA-MSNSRNFC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendNetworkServiceRequestNoFdnCheck with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendNetworkServiceRequestNoFdnCheck with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequestNoFdnCheck0003L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    TInfoName expServiceString(KNullDesC);
    TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, KErrNone);

    TRequestStatus reqStatus;
    iPhone.SendNetworkServiceRequestNoFdnCheck(reqStatus, KNullDesC);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(2, this); // data, this

    }


/**
@SYMTestCaseID BA-CTSY-USNA-MSNSRNFC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SendNetworkServiceRequestNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SendNetworkServiceRequestNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequestNoFdnCheck0004L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup, this));
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
    // Test A: Test multiple clients requesting RMobilePhone::SendNetworkServiceRequestNoFdnCheck
    //-------------------------------------------------------------------------

    // setting and execute 1st request
    TInfoName expServiceString(_L("barracuda"));
    TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, data);
    iMockLTSY.CompleteL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, KErrNone, 10);

    TRequestStatus reqStatus;
    iPhone.SendNetworkServiceRequestNoFdnCheck(reqStatus, expServiceString);

    // setting and execute 2nd request
    TRequestStatus reqStatus2;
    _LIT(KServiceString2, "cudabarra");
    phone2.SendNetworkServiceRequestNoFdnCheck(reqStatus2, KServiceString2);

    // check results
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

    }


/**
@SYMTestCaseID BA-CTSY-USNA-MSNSRNFC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendNetworkServiceRequestNoFdnCheck with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendNetworkServiceRequestNoFdnCheck and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserNetworkAccessFU::TestSendNetworkServiceRequestNoFdnCheck0005L()
    {


    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    //-------------------------------------------------------------------------
    // Test A: Test timeout of RMobilePhone::SendNetworkServiceRequestNoFdnCheck
    //-------------------------------------------------------------------------

    TInfoName expServiceString(_L("barracuda"));
    TMockLtsyData1<TInfoName> expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, data);

    TRequestStatus reqStatus;
    iPhone.SendNetworkServiceRequestNoFdnCheck(reqStatus, expServiceString);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(2, this); // data, this

    }
