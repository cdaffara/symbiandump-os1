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
// The TEFUnit test suite for USIMApplication in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyusimapplicationfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyUSIMApplicationFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestGetUSimApplicationInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestGetUSimAppsSelectionMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestSetUSimApplicationStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestSetUSimAppsSelectionMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestEnumerateUSimApplications0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestNotifyUSimApplicationsInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestNotifyUSimAppsSelectionModeChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestGetCurrentActiveUSimApplication0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestGetCurrentActiveUSimApplication0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestGetCurrentActiveUSimApplication0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUSIMApplicationFU, TestGetCurrentActiveUSimApplication0005L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-USIMA-MGUAI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetUSimApplicationInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetUSimApplicationInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestGetUSimApplicationInfo0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TInt uSimAppIndex(0);
	RMobilePhone::TUSimApplicationInfoV2 info;
	RMobilePhone::TUSimApplicationInfoV2Pckg infoPckg(info);
	iPhone.GetUSimApplicationInfo(reqStatus, uSimAppIndex, infoPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);	
	}


/**
@SYMTestCaseID BA-CTSY-USIMA-MGUASM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetUSimAppsSelectionMode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetUSimAppsSelectionMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestGetUSimAppsSelectionMode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TUSimSelectionMode usimSelectionMode(RMobilePhone::EUSimSelectionAutomatic);
	TInt ret = iPhone.GetUSimAppsSelectionMode(usimSelectionMode);
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-USIMA-MSUAS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetUSimApplicationStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetUSimApplicationStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestSetUSimApplicationStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TAID aid;
	RMobilePhone::TUSimAppAction usimAppAction(RMobilePhone::EUSimActivate);
	iPhone.SetUSimApplicationStatus(reqStatus, aid, usimAppAction);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-USIMA-MSUASM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetUSimAppsSelectionMode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetUSimAppsSelectionMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestSetUSimAppsSelectionMode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TUSimSelectionMode usimSelectionMode(RMobilePhone::EUSimSelectionAutomatic);
	TInt ret = iPhone.SetUSimAppsSelectionMode(usimSelectionMode);
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-USIMA-MEUA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::EnumerateUSimApplications
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::EnumerateUSimApplications
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestEnumerateUSimApplications0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TInt count(0);
	RMobilePhone::TAID aid;
	iPhone.EnumerateUSimApplications(reqStatus, count, aid);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-USIMA-MNUAIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyUSimApplicationsInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyUSimApplicationsInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestNotifyUSimApplicationsInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TInt count(0);
	RMobilePhone::TAID aid;
	iPhone.NotifyUSimApplicationsInfoChange(reqStatus, count, aid);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-USIMA-MNUASMC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyUSimAppsSelectionModeChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyUSimAppsSelectionModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestNotifyUSimAppsSelectionModeChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TUSimSelectionMode usimSelectionMode(RMobilePhone::EUSimSelectionAutomatic);
	iPhone.NotifyUSimAppsSelectionModeChange(reqStatus, usimSelectionMode);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}

/**
@SYMTestCaseID BA-CTSY-SUBI-MGIK-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCurrentActiveUSimApplication
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCurrentActiveUSimApplication
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestGetCurrentActiveUSimApplication0001L()
	{
	OpenEtelServerL();
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Test data
	_LIT8(KSER,"0123456789");

	// Do the failures first because once it has successfully retrievd the active USim application ID
	// it caches it and makes no calls to LTSY.

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetCurrentActiveUSimApplication, KErrNotSupported);

	TRequestStatus requestStatus;
	RMobilePhone::TAID aid;
	iPhone.GetCurrentActiveUSimApplication(requestStatus, aid);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetCurrentActiveUSimApplication);
	iMockLTSY.CompleteL(EMobilePhoneGetCurrentActiveUSimApplication, KErrNotSupported);

	iPhone.GetCurrentActiveUSimApplication(requestStatus, aid);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetCurrentActiveUSimApplication
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetCurrentActiveUSimApplication);
	RMobilePhone::TAID retAid(KNullDesC8);
	TMockLtsyData1<RMobilePhone::TAID > retAidData(retAid);
	retAidData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetCurrentActiveUSimApplication, KErrNone, data);

	iPhone.GetCurrentActiveUSimApplication(requestStatus, aid);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	// Check result
	ASSERT_TRUE( 0 == KNullDesC8().Compare(aid) );

	//-------------------------------------------------------------------------
	// this time send not empty SID
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetCurrentActiveUSimApplication);
	retAid.Copy(KSER);
	data.Close();
	retAidData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetCurrentActiveUSimApplication, KErrNone, data);

	iPhone.GetCurrentActiveUSimApplication(requestStatus, aid);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	// Check result
	ASSERT_TRUE( 0 == KSER().Compare(aid) );

	// Done !
	CleanupStack::PopAndDestroy(2); // data, this
	}

/**
@SYMTestCaseID BA-CTSY-SUBI-MGIK-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetCurrentActiveUSimApplication
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetCurrentActiveUSimApplication
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestGetCurrentActiveUSimApplication0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: cancelling of RMobilePhone::GetCurrentActiveUSimApplication
 	//-------------------------------------------------------------------------

	// Expect call down to LTSY for this IPC
	iMockLTSY.ExpectL(EMobilePhoneGetCurrentActiveUSimApplication);

	// Queue response to be delayed to give time for cancel to come through
	// Return data is needed because CTSY does not pass the cancel to
	// LTSY
	RMobilePhone::TAID retAid = _L8("0123456789");	
	TMockLtsyData1<RMobilePhone::TAID> retAidData(retAid);
	retAidData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetCurrentActiveUSimApplication, KErrNone, data, 10);

	TRequestStatus requestStatus;
	RMobilePhone::TAID aid;
	iPhone.GetCurrentActiveUSimApplication(requestStatus, aid);
	iPhone.CancelAsyncRequest(EMobilePhoneGetCurrentActiveUSimApplication);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, this
	}

/**
@SYMTestCaseID BA-CTSY-SUBI-MGIK-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetCurrentActiveUSimApplication
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetCurrentActiveUSimApplication
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestGetCurrentActiveUSimApplication0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetCurrentActiveUSimApplication
 	//-------------------------------------------------------------------------

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	// First client request
	iMockLTSY.ExpectL(EMobilePhoneGetCurrentActiveUSimApplication);
	_LIT8(KRetAid,"0123456789");	
	RMobilePhone::TAID retAid;
	retAid.Copy(KRetAid());	
	TMockLtsyData1<RMobilePhone::TAID> retAidData(retAid);
	retAidData.SerialiseL(data);

	TRequestStatus requestStatus;
	RMobilePhone::TAID aid;
	iPhone.GetCurrentActiveUSimApplication(requestStatus, aid);
	
	iMockLTSY.CompleteL(EMobilePhoneGetCurrentActiveUSimApplication, KErrNone, data);	

	//second get expected to fail with KErrServerBusy
	TRequestStatus requestStatus2;
	RMobilePhone::TAID aid2;
	phone2.GetCurrentActiveUSimApplication(requestStatus2, aid2);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KRetAid().Compare(aid) );
	
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	
	
	// Done !
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCNG-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCurrentActiveUSimApplication with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCurrentActiveUSimApplication and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUSIMApplicationFU::TestGetCurrentActiveUSimApplication0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetCurrentActiveUSimApplication
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetCurrentActiveUSimApplication);

	TRequestStatus requestStatus;
	RMobilePhone::TAID aid;
	iPhone.GetCurrentActiveUSimApplication(requestStatus, aid);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}
