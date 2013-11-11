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
// The TEFUnit test suite for AuthorisationAndAuthentication in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyauthorisationandauthenticationfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <mmretrieve.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "cctsyactiveretriever.h"

CTestSuite* CCTsyAuthorisationAndAuthenticationFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyAuthorisationAndAuthenticationFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAuthorisationAndAuthenticationFU, TestStart0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAuthorisationAndAuthenticationFU, TestStart0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAuthorisationAndAuthenticationFU, TestStart0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAuthorisationAndAuthenticationFU, TestSmartCardAuthenticate0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-AAA-ARAI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CAsyncRetrieveAuthorizationInfo::Start
@SYMTestPriority High
@SYMTestActions Invokes CAsyncRetrieveAuthorizationInfo::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAuthorisationAndAuthenticationFU::TestStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	RMobilePhone::CImsAuthorizationInfoV5* authInfo = RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfo);

	RMobilePhone::CImsAuthorizationInfoV5* expectedAuthInfo = RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(expectedAuthInfo);
	
	CAsyncRetrieveAuthorizationInfo* asyncRetrieveAuthorizationInfo = CAsyncRetrieveAuthorizationInfo::NewL(iPhone, *authInfo);    
	CleanupStack::PushL(asyncRetrieveAuthorizationInfo);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeAuthorizationInfoRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveAuthorizationInfo);
	CleanupStack::PushL(activeAuthorizationInfoRetriever);
	scheduler.AddRetrieverL(*activeAuthorizationInfoRetriever);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneAuthorizationInfoPhase1, KErrNotSupported);

	asyncRetrieveAuthorizationInfo->Start(activeAuthorizationInfoRetriever->Status());
	activeAuthorizationInfoRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeAuthorizationInfoRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneAuthorizationInfoPhase1, KErrNone);

	HBufC8* expBuf(NULL);
	expectedAuthInfo->ExternalizeL(expBuf);
	CleanupStack::PushL(expBuf);
	data.Create(*expBuf);
	CleanupStack::PopAndDestroy(expBuf);
	iMockLTSY.CompleteL(EMobilePhoneAuthorizationInfoPhase1, KErrGeneral, data, 0);

	asyncRetrieveAuthorizationInfo->Start(activeAuthorizationInfoRetriever->Status());
	activeAuthorizationInfoRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrGeneral, activeAuthorizationInfoRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CAsyncRetrieveAuthorizationInfo::Start when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneAuthorizationInfoPhase1, KErrNone);

	expectedAuthInfo->iAuthenticationDataSource = RMobilePhone::EFromISIM;
	expectedAuthInfo->iIMPI.Copy(_L("asta"));
	TBuf8<255> tmpBuf;
	tmpBuf.Copy(_L("la"));
	expectedAuthInfo->iIMPUArray.Append(tmpBuf);
	tmpBuf.Copy(_L("vista"));
	expectedAuthInfo->iIMPUArray.Append(tmpBuf);
	tmpBuf.Copy(_L("baby"));
	expectedAuthInfo->iIMPUArray.Append(tmpBuf);
	expectedAuthInfo->iHomeNetworkDomainName.Copy(_L("!!!"));

	expBuf = NULL;
	expectedAuthInfo->ExternalizeL(expBuf);
	CleanupStack::PushL(expBuf);
	data.Create(*expBuf);
	CleanupStack::PopAndDestroy(expBuf);
    iMockLTSY.CompleteL(EMobilePhoneAuthorizationInfoPhase1, KErrNone, data);

	asyncRetrieveAuthorizationInfo->Start(activeAuthorizationInfoRetriever->Status());
	activeAuthorizationInfoRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeAuthorizationInfoRetriever->iStatus.Int());
	
	HBufC8* buf(NULL);
	authInfo->ExternalizeL(buf);
	CleanupStack::PushL(buf);	
	expBuf = NULL;
	expectedAuthInfo->ExternalizeL(expBuf);
	CleanupStack::PushL(expBuf);	
	ASSERT_EQUALS(*expBuf, *buf);
	CleanupStack::PopAndDestroy(2); // expBuf, buf
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CAsyncRetrieveAuthorizationInfo::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);	

    iMockLTSY.CompleteL(EMobilePhoneAuthorizationInfoPhase1, KErrNone, data);

	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4); // activeAuthorizationInfoRetriever, asyncRetrieveAuthorizationInfo, expectedAuthInfo, authInfo
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-AAA-ARAI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CAsyncRetrieveAuthorizationInfo::Start
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CAsyncRetrieveAuthorizationInfo::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAuthorisationAndAuthenticationFU::TestStart0002L()
	{

// This test should test cancellation of Start
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	RMobilePhone::CImsAuthorizationInfoV5* authInfo = RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfo);

	RMobilePhone::CImsAuthorizationInfoV5* expectedAuthInfo = RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(expectedAuthInfo);
	
	CAsyncRetrieveAuthorizationInfo* asyncRetrieveAuthorizationInfo = CAsyncRetrieveAuthorizationInfo::NewL(iPhone, *authInfo);    
	CleanupStack::PushL(asyncRetrieveAuthorizationInfo);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeAuthorizationInfoRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveAuthorizationInfo);
	CleanupStack::PushL(activeAuthorizationInfoRetriever);
	scheduler.AddRetrieverL(*activeAuthorizationInfoRetriever);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of CAsyncRetrieveAuthorizationInfo::Start
 	//-------------------------------------------------------------------------
 	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.ExpectL(EMobilePhoneAuthorizationInfoPhase1, KErrNone);

	HBufC8* expBuf(NULL);
	expectedAuthInfo->ExternalizeL(expBuf);
	CleanupStack::PushL(expBuf);
	data.Create(*expBuf);
	CleanupStack::PopAndDestroy(expBuf);
    iMockLTSY.CompleteL(EMobilePhoneAuthorizationInfoPhase1, KErrNone, data, 10);

	asyncRetrieveAuthorizationInfo->Start(activeAuthorizationInfoRetriever->Status());
	activeAuthorizationInfoRetriever->Activate(CActiveRetriever::ECaseGeneralCancelCase);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeAuthorizationInfoRetriever->iStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

    iMockLTSY.ExpectL(EMobilePhoneAuthorizationInfoPhase1, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneAuthorizationInfoPhase1, KErrNone, data);

	asyncRetrieveAuthorizationInfo->Start(activeAuthorizationInfoRetriever->Status());
	activeAuthorizationInfoRetriever->Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeAuthorizationInfoRetriever->iStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4); // activeAuthorizationInfoRetriever, asyncRetrieveAuthorizationInfo, expectedAuthInfo, authInfo
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-AAA-ARAI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to CAsyncRetrieveAuthorizationInfo::Start
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to CAsyncRetrieveAuthorizationInfo::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAuthorisationAndAuthenticationFU::TestStart0004L()
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

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	RMobilePhone::CImsAuthorizationInfoV5* authInfo = RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfo);

	RMobilePhone::CImsAuthorizationInfoV5* expectedAuthInfo = RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(expectedAuthInfo);
	
	CAsyncRetrieveAuthorizationInfo* asyncRetrieveAuthorizationInfo = CAsyncRetrieveAuthorizationInfo::NewL(iPhone, *authInfo);    
	CleanupStack::PushL(asyncRetrieveAuthorizationInfo);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeAuthorizationInfoRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveAuthorizationInfo);
	CleanupStack::PushL(activeAuthorizationInfoRetriever);
	scheduler.AddRetrieverL(*activeAuthorizationInfoRetriever);
	

	RMobilePhone::CImsAuthorizationInfoV5* authInfo2 = RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfo2);

	CAsyncRetrieveAuthorizationInfo* asyncRetrieveAuthorizationInfo2 = CAsyncRetrieveAuthorizationInfo::NewL(phone2, *authInfo2);    
	CleanupStack::PushL(asyncRetrieveAuthorizationInfo2);

	CActiveRetriever* activeAuthorizationInfoRetriever2 = 
						CActiveRetriever::NewL(*asyncRetrieveAuthorizationInfo2);
	CleanupStack::PushL(activeAuthorizationInfoRetriever2);
	scheduler.AddRetrieverL(*activeAuthorizationInfoRetriever2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting CAsyncRetrieveAuthorizationInfo::Start
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneAuthorizationInfoPhase1, KErrNone);
	HBufC8* expBuf(NULL);
	expectedAuthInfo->ExternalizeL(expBuf);
	CleanupStack::PushL(expBuf);
	data.Create(*expBuf);
	CleanupStack::PopAndDestroy(expBuf);
    iMockLTSY.CompleteL(EMobilePhoneAuthorizationInfoPhase1, KErrNone, data);

	asyncRetrieveAuthorizationInfo->Start(activeAuthorizationInfoRetriever->Status());
	activeAuthorizationInfoRetriever->Activate();

	asyncRetrieveAuthorizationInfo2->Start(activeAuthorizationInfoRetriever2->Status());
	activeAuthorizationInfoRetriever2->Activate();

	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeAuthorizationInfoRetriever->iStatus.Int());
	ASSERT_EQUALS(KErrServerBusy, activeAuthorizationInfoRetriever2->iStatus.Int());
	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(3); // activeAuthorizationInfoRetriever2, asyncRetrieveAuthorizationInfo2, authInfo2
	CleanupStack::PopAndDestroy(4); // activeAuthorizationInfoRetriever, asyncRetrieveAuthorizationInfo, expectedAuthInfo, authInfo
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-AAA-ARAI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CAsyncRetrieveAuthorizationInfo::Start with timeout
@SYMTestPriority High
@SYMTestActions Invokes CAsyncRetrieveAuthorizationInfo::Start and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAuthorisationAndAuthenticationFU::TestStart0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	RMobilePhone::CImsAuthorizationInfoV5* authInfo = RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfo);

	CAsyncRetrieveAuthorizationInfo* asyncRetrieveAuthorizationInfo = CAsyncRetrieveAuthorizationInfo::NewL(iPhone, *authInfo);    
	CleanupStack::PushL(asyncRetrieveAuthorizationInfo);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeAuthorizationInfoRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveAuthorizationInfo);
	CleanupStack::PushL(activeAuthorizationInfoRetriever);
	scheduler.AddRetrieverL(*activeAuthorizationInfoRetriever);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of CAsyncRetrieveAuthorizationInfo::Start
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneAuthorizationInfoPhase1, KErrNone);

	asyncRetrieveAuthorizationInfo->Start(activeAuthorizationInfoRetriever->Status());
	activeAuthorizationInfoRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrTimedOut, activeAuthorizationInfoRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // activeAuthorizationInfoRetriever, asyncRetrieveAuthorizationInfo, authInfo
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-AAA-MSCA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SmartCardAuthenticate
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SmartCardAuthenticate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAuthorisationAndAuthenticationFU::TestSmartCardAuthenticate0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;
	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);
	
	TRequestStatus reqStatus;
	iPhone.SmartCardAuthenticate(reqStatus, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	RMobilePhone::TAID AID;
	iPhone.SmartCardAuthenticate(reqStatus, AID, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}
