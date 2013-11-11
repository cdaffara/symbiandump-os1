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
// The TEFUnit test suite for PhoneSecurity in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyphonesecurityfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>

CTestSuite* CCTsyPhoneSecurityFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetLockInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetLockInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetLockInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetLockInfo0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestClearBlacklist0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestSetLockSetting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestSetLockSetting0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestSetLockSetting0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestSetLockSetting0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestSetLockSetting0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetSecurityCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestAbortSecurityCode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestAbortSecurityCode0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestAbortSecurityCode0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestAbortSecurityCode0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestChangeSecurityCode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestChangeSecurityCode0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestChangeSecurityCode0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestChangeSecurityCode0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestVerifySecurityCode0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityEvent0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityEvent0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityEvent0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityEvent0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityEvent0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityEvent0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityEvent0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifyLockInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifyLockInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifyLockInfoChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifyLockInfoChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetSecurityCodeInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetSecurityCodeInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetSecurityCodeInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetSecurityCodeInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetSecurityCodeInfo0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetSecurityCodeInfo0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestGetSecurityCodeInfo0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityCodeInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityCodeInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityCodeInfoChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityCodeInfoChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneSecurityFU, TestNotifySecurityCapsChange0004L);


	END_SUITE;
	}

//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PSEC-MGLI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetLockInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetLockInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetLockInfo0001L()
	{
	OpenEtelServerL();
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	RMobilePhone::TMobilePhoneLock currentLock = RMobilePhone::ELockICC;
	TMockLtsyData1<RMobilePhone::TMobilePhoneLock> phoneLockType(currentLock);
	phoneLockType.SerialiseL(dataExpect);

	iMockLTSY.ExpectL(EMobilePhoneGetLockInfo, dataExpect, KErrNotSupported);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoPckg(lockInfo);
	iPhone.GetLockInfo(reqStatus, currentLock, lockInfoPckg);
    User::WaitForRequest(reqStatus);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetLockInfo, dataExpect);

	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;
	RMobilePhone::TMobilePhoneLockStatus lockStatus = RMobilePhone::EStatusLocked;

    TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> phoneLockData(lockStatus, lockSetting);
	phoneLockData.SerialiseL(dataComplete);

	iMockLTSY.CompleteL(EMobilePhoneGetLockInfo, KErrGeneral , dataComplete);
	iPhone.GetLockInfo(reqStatus, currentLock, lockInfoPckg);
	User::WaitForRequest(reqStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetLockInfo when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetLockInfo, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneGetLockInfo, KErrNone, dataComplete);

	iPhone.GetLockInfo(reqStatus, currentLock, lockInfoPckg);
    User::WaitForRequest(reqStatus);
    
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(lockInfo.iSetting, lockSetting);
	ASSERT_EQUALS(lockInfo.iStatus, lockStatus);	
   	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	// TEST E:
	//-------------------------------------------------------------------------	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetLockInfo, KErrNone, dataComplete);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(3); // dataComplete, dataExpect, this
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MGLI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetLockInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetLockInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetLockInfo0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetLockInfo
 	//-------------------------------------------------------------------------
	// Expect request in LTSY
	RMobilePhone::TMobilePhoneLock currentLock = RMobilePhone::ELockICC;
	TMockLtsyData1<RMobilePhone::TMobilePhoneLock> phoneLockType(currentLock);
	phoneLockType.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetLockInfo, data);

	// Complete request after a delay
	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;
	RMobilePhone::TMobilePhoneLockStatus lockStatus = RMobilePhone::EStatusLocked;
    TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> phoneLockData(lockStatus, lockSetting);
	data.Close();
	phoneLockData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetLockInfo, KErrNone , data, 20);

	// Make request and cancel it
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoPckg(lockInfo);
	iPhone.GetLockInfo(reqStatus, currentLock, lockInfoPckg);
	iPhone.CancelAsyncRequest(EMobilePhoneGetLockInfo);
	User::WaitForRequest(reqStatus);

	// Check status is KErrCancel
	ASSERT_EQUALS(KErrCancel,reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	CleanupStack::PopAndDestroy(2); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MGLI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetLockInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetLockInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetLockInfo0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetLockInfo
 	//-------------------------------------------------------------------------
 	
	// CTSY should return an error straight away
	
	TTestMultimodeType badType(KETelExtMultimodeV1);
	TTestMultimodeTypePckg badTypePckg(badType);

	TRequestStatus reqStatus;
	iPhone.GetLockInfo(reqStatus, RMobilePhone::ELockICC, badTypePckg);
    User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
   	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(this); // this
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MGLI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetLockInfo with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetLockInfo and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetLockInfo0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetLockInfo
 	//-------------------------------------------------------------------------
	// Expect request in LTSY
	RMobilePhone::TMobilePhoneLock currentLock = RMobilePhone::ELockICC;
	TMockLtsyData1<RMobilePhone::TMobilePhoneLock> phoneLockType(currentLock);
	phoneLockType.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetLockInfo, data);

	// Make request and wait for it to timeout
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoPckg(lockInfo);
	iPhone.GetLockInfo(reqStatus, currentLock, lockInfoPckg);
	User::WaitForRequest(reqStatus);

	// Check status is KErrTimedOut
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int())

	// Done !
	CleanupStack::PopAndDestroy(2); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MCB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ClearBlacklist
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ClearBlacklist
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestClearBlacklist0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	iPhone.ClearBlacklist(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	// Test API is not supported

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MSLS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetLockSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetLockSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestSetLockSetting0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	TRequestStatus status;
	
	// data for ExpectL for test B
	RMobilePhone::TMobilePhoneLockStatus lockStatusB = RMobilePhone::EStatusLocked;
	RMobilePhone::TMobilePhoneLock lockB = RMobilePhone::ELockPhoneDevice;
	RMobilePhone::TMobilePhoneLockSetting lockSettingB = RMobilePhone::ELockSetEnabled;	

	// data for CompleteL
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> 
															lockStatusDataB(lockStatusB,lockSettingB);
	lockStatusDataB.SerialiseL(dataComplete);
	
		
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 70305);
	iPhone.SetLockSetting(status, lockB, lockSettingB);	
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrGeneral, dataComplete);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrGeneral, status.Int());
	AssertMockLtsyStatusL();
		 	 
	
	// data for ExpectL
	TLockSettingPassword lockSettingPassword;
	lockSettingPassword.iPassword.Copy(KNullDesC);
	RMobilePhone::TMobilePhoneLock& lock( lockSettingPassword.iLock );
	RMobilePhone::TMobilePhoneLockSetting& lockSetting( lockSettingPassword.iSetting );
	lock        = RMobilePhone::ELockHiddenKey;//ELockUSimApp;
	lockSetting = RMobilePhone::ELockSetEnabled;	
	TMockLtsyData1<TLockSettingPassword> phoneLockData(lockSettingPassword);
	phoneLockData.SerialiseL(data);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetLockSetting
	// from LTSY.
 	//------------------------------------------------------------------------- 	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, dataComplete);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	

  	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetLockSetting when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetLockSetting, data);

	iPhone.SetLockSetting(status, lock, lockSetting);
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, dataComplete);
	User::WaitForRequest(status);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, status.Int());	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 70302);
		
	//-------------------------------------------------------------------------
	// TEST E2: Unsolicited completion of RMobilePhone::SetLockSetting
	// from LTSY.
 	//------------------------------------------------------------------------- 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, dataComplete);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetLockSetting, data);		
	iPhone.SetLockSetting(status, lock, lockSetting);
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, dataComplete);
	User::WaitForRequest(status);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 70302);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, status.Int());
	
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(3, this); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MSLS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SetLockSetting
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SetLockSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestSetLockSetting0002L()
	{

// This test should test cancellation of SetLockSetting
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SetLockSetting
 	//-------------------------------------------------------------------------  	
 	RMobilePhone::TMobilePhoneLockStatus lockStatus = RMobilePhone::EStatusLocked;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockPhoneDevice;
	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;
 	
 	TMockLtsyData2 <RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> ltsyData(lockStatus, lockSetting);
	ltsyData.SerialiseL(data);	
	
	TRequestStatus requestStatus;
	//send request
	iPhone.SetLockSetting(requestStatus, lock, lockSetting);	
	// send canceling...
	iPhone.CancelAsyncRequest(EMobilePhoneSetLockSetting);
	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(2); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MSLS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetLockSetting
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetLockSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestSetLockSetting0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	RBuf8 dataExpect2;
	CleanupClosePushL(dataExpect2);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);	
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetLockSetting
 	//-------------------------------------------------------------------------
	// initialize data for first mobile phone's SetLockSetting
	TLockSettingPassword lockSettingPassword;
	lockSettingPassword.iPassword.Copy(KNullDesC);
	RMobilePhone::TMobilePhoneLock& lock( lockSettingPassword.iLock );
	RMobilePhone::TMobilePhoneLockSetting& lockSetting( lockSettingPassword.iSetting );
	lock        = RMobilePhone::ELockHiddenKey;
	lockSetting = RMobilePhone::ELockSetEnabled;
	TMockLtsyData1<TLockSettingPassword> expData(lockSettingPassword);
	expData.SerialiseL(dataExpect);	
 
	// initialize data for second mobile phone's SetLockSetting
	TLockSettingPassword lockSettingPassword2;
	lockSettingPassword2.iPassword.Copy(KNullDesC);
	RMobilePhone::TMobilePhoneLock& lock2( lockSettingPassword2.iLock );
	RMobilePhone::TMobilePhoneLockSetting& lockSetting2( lockSettingPassword2.iSetting );
	lock2        = RMobilePhone::ELockUSimApp;
	lockSetting2 = RMobilePhone::ELockSetDisabled;
	TMockLtsyData1<TLockSettingPassword> expData2(lockSettingPassword2);
	expData2.SerialiseL(dataExpect2);
	
	// data for complete
	RMobilePhone::TMobilePhoneLockStatus complete_lockStatus = RMobilePhone::EStatusLocked;
	RMobilePhone::TMobilePhoneLockSetting complete_lockSetting = RMobilePhone::ELockSetEnabled;	
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> 
															lockStatusData(complete_lockStatus,complete_lockSetting);
	lockStatusData.SerialiseL(dataComplete);
	TRequestStatus	requestStatus; 	
	TRequestStatus	requestStatus2;	
			
	// send first request		
	iMockLTSY.ExpectL(EMobilePhoneSetLockSetting, dataExpect);	
	iPhone.SetLockSetting(requestStatus, lock, lockSetting);
	
	// send second request	
	phone2.SetLockSetting(requestStatus2, lock2, lockSetting2);	
	
	// send completion for first phone SetlockSetting
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, dataComplete);	
		
	// wait for first phone SetLockSetting
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
		
	// wait for second phone SetLockSetting.
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	AssertMockLtsyStatusL();	
	// Done !
	CleanupStack::PopAndDestroy(6, this); //  this ...

	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MSLS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetLockSetting with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetLockSetting and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestSetLockSetting0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetLockSetting
 	//-------------------------------------------------------------------------
	RMobilePhone::TMobilePhoneLockStatus lockStatus = RMobilePhone::EStatusLocked;
	TLockSettingPassword lockSettingPassword;
	lockSettingPassword.iPassword.Copy(KNullDesC);
	RMobilePhone::TMobilePhoneLock& lock( lockSettingPassword.iLock );
	RMobilePhone::TMobilePhoneLockSetting& lockSetting( lockSettingPassword.iSetting );
	lock = RMobilePhone::ELockPhoneDevice;
	lockSetting = RMobilePhone::ELockSetEnabled; 	
	TMockLtsyData1<TLockSettingPassword> expData(lockSettingPassword);
	expData.SerialiseL(dataExpect);
	
 	TMockLtsyData2 <RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> ltsyData(lockStatus, lockSetting);
	ltsyData.SerialiseL(data);
	
	TRequestStatus	requestStatus; 
		
	iMockLTSY.ExpectL(EMobilePhoneSetLockSetting, dataExpect);
	
	iPhone.SetLockSetting(requestStatus, lock, lockSetting);			
		
	// wait for first phone SetLockSetting
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 70303);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(3, this); // data, this

	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MSLS-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetLockSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetLockSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestSetLockSetting0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
		
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	
	//-------------------------------------------------------------------------
	// TEST for case RMobilePhone::ELockICC:
 	//-------------------------------------------------------------------------	
 	// data for ExpectL 
	RMobilePhone::TMobilePhoneLockStatus lockStatus = RMobilePhone::EStatusLocked;
	TLockSettingPassword lockSettingPassword;
	lockSettingPassword.iPassword.Copy(KNullDesC);
	RMobilePhone::TMobilePhoneLock& lock( lockSettingPassword.iLock );
	RMobilePhone::TMobilePhoneLockSetting& lockSetting( lockSettingPassword.iSetting );
	lock        = RMobilePhone::ELockICC;
	lockSetting = RMobilePhone::ELockSetEnabled;	
	TMockLtsyData1<TLockSettingPassword> phoneLockData(lockSettingPassword);
	phoneLockData.SerialiseL(data);
	
	// data for CompleteL
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> 
															lockStatusData(lockStatus,lockSetting);
	lockStatusData.SerialiseL(dataComplete);
	TRequestStatus status;
	iPhone.SetLockSetting(status, lock, lockSetting);
	// send completion
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, dataComplete);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST for case RMobilePhone::ELockPin2
 	//-------------------------------------------------------------------------	
 	// data for ExpectL 
	lockStatus = RMobilePhone::EStatusLocked;
	lock = RMobilePhone::ELockPin2;
	lockSetting = RMobilePhone::ELockSetEnabled;	
	data.Close();
	phoneLockData.SerialiseL(data);
	
	// data for CompleteL
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> 
															lockStatusData1(lockStatus,lockSetting);
	dataComplete.Close();
	lockStatusData.SerialiseL(dataComplete);
	iPhone.SetLockSetting(status, lock, lockSetting);
	// send completion
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, dataComplete);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	

	//-------------------------------------------------------------------------
	// TEST for case RMobilePhone::ELockUniversalPin 
 	//-------------------------------------------------------------------------	
 	// data for ExpectL 
	lockStatus = RMobilePhone::EStatusLocked;
	lock = RMobilePhone::ELockUniversalPin;
	lockSetting = RMobilePhone::ELockSetEnabled;	
	data.Close();
	phoneLockData.SerialiseL(data);
	
	// data for CompleteL
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> 
															lockStatusData2(lockStatus,lockSetting);
	dataComplete.Close();
	lockStatusData2.SerialiseL(dataComplete);
	iPhone.SetLockSetting(status, lock, lockSetting);
	// send completion
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, dataComplete);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	// TEST for case RMobilePhone::ELockUniversalPin and with  lockSetting == ELockReplaced
 	//-------------------------------------------------------------------------	
 	// data for ExpectL 
	lockStatus = RMobilePhone::EStatusLocked;
	lock = RMobilePhone::ELockUniversalPin;
	lockSetting = RMobilePhone::ELockReplaced;	
	data.Close();
	phoneLockData.SerialiseL(data);
	
	// data for CompleteL
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> 
															lockStatusData3(lockStatus,lockSetting);
	dataComplete.Close();
	lockStatusData3.SerialiseL(dataComplete);
	iPhone.SetLockSetting(status, lock, lockSetting);
	// send completion
	iMockLTSY.CompleteL(EMobilePhoneSetLockSetting, KErrNone, dataComplete);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	// TEST for case RMobilePhone::ELockPhoneToFirstICC 
 	//-------------------------------------------------------------------------	
 	// data for ExpectL 
	lockStatus = RMobilePhone::EStatusLocked;
	lock = RMobilePhone::ELockPhoneToFirstICC;
	lockSetting = RMobilePhone::ELockReplaced;	
	data.Close();
	phoneLockData.SerialiseL(data);
	
	iPhone.SetLockSetting(status, lock, lockSetting);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	AssertMockLtsyStatusL();  
	
	
	//-------------------------------------------------------------------------
	// TEST for condition RMobilePhone::ELockSetUnknown != *setting
 	//-------------------------------------------------------------------------	
 	// data for ExpectL 
	lockStatus = RMobilePhone::EStatusLocked;
	lock = RMobilePhone::ELockPhoneToFirstICC;
	lockSetting = RMobilePhone::ELockSetUnknown;	
	data.Close();
	phoneLockData.SerialiseL(data);

	iPhone.SetLockSetting(status, lock, lockSetting);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	AssertMockLtsyStatusL();  

	//-------------------------------------------------------------------------
	// TEST for condition
	//		 if (
  	//          ( RMobilePhone::ELockSetDisabled == *setting ) &&
 	//          ( RMobilePhone::ELockICC == *lock ||
 	//            RMobilePhone::ELockUniversalPin == *lock ) &&
  	//            !iPin1DisableSupported )
 	//-------------------------------------------------------------------------	
 	
 	//------- Set iPin1DisableSupported-member to False state ----------------- 	
 	// data for CompleteL
 	TBool supported = EFalse;
	TMockLtsyData1<TBool> ltsyData(supported);
	dataComplete.Close();
	ltsyData.SerialiseL(dataComplete);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	// send completion for calling SetPin1DisableNotSupported
	iMockLTSY.CompleteL(EMmTsyPhoneGetPin1DisableSupportedIPC, KErrNone, dataComplete);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	//-------------------------------------------------------------------------
 	
 	// data for ExpectL 
	lockStatus = RMobilePhone::EStatusLocked;
	lock = RMobilePhone::ELockICC;
	lockSetting = RMobilePhone::ELockSetDisabled;	
	data.Close();
	phoneLockData.SerialiseL(data);
 	
 	// call SetLockSetting itself
	iPhone.SetLockSetting(status, lock, lockSetting);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrGsm0707OperationNotAllowed, status.Int());
	AssertMockLtsyStatusL();  
	
	CleanupStack::PopAndDestroy(3, this); // data, this
	}
/**
@SYMTestCaseID BA-CTSY-PSEC-MGSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSecurityCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSecurityCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetSecurityCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	// Get default Caps
    TUint32 oldCaps;	
    iPhone.GetSecurityCaps(oldCaps);
	ASSERT_EQUALS(KMultimodeTsyGsmSecurityInitCaps, oldCaps)

	// Change securityCaps
	RMobilePhone::TMobilePhoneSecurityEvent ev(RMobilePhone::EICCTerminated);
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> evData(ev);
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
   
    // operations taken from CMmPhoneGsmWcdmaExt::UpdateSecurityCaps()
    TUint32 capsToCompare(0);	    	
    if ( oldCaps & RMobilePhone::KCapsLockPhone )
        {
        capsToCompare |= RMobilePhone::KCapsLockPhone;
        }
    if ( oldCaps & RMobilePhone::KCapsAccessPhonePassword )
        {
        capsToCompare |= RMobilePhone::KCapsAccessPhonePassword;
        }

	// Get changed Caps        
    TUint32 newCaps;
    iPhone.GetSecurityCaps(newCaps);
	
	ASSERT_EQUALS(capsToCompare, newCaps); 

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}
	

/**
@SYMTestCaseID BA-CTSY-PSEC-MASC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::AbortSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AbortSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestAbortSecurityCode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> secCodeData(secCode);
	secCodeData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(EMobilePhoneAbortSecurityCode, data, KErrNotSupported);
	TInt res = iPhone.AbortSecurityCode(secCode);
	
	ASSERT_EQUALS(KErrNotSupported, res)
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(EMobilePhoneAbortSecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneAbortSecurityCode, KErrGeneral);

	res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrGeneral, res)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::AbortSecurityCode when result is not cached.
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(EMobilePhoneAbortSecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneAbortSecurityCode, KErrNone);

	res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrNone, res)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::AbortSecurityCode
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneAbortSecurityCode, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-PSEC-MASC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::AbortSecurityCode with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AbortSecurityCode and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestAbortSecurityCode0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> secCodeData(secCode);
	secCodeData.SerialiseL(data);
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::AbortSecurityCode
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneAbortSecurityCode, data);

	TInt res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrTimedOut, res)

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MASC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::AbortSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AbortSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestAbortSecurityCode0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
		
	TRequestStatus status;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockPhoneDevice;//ELockUSimApp;
	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;	
	
	//-------------------------------------------------------------------------
	// Verify that SetLockSetting will be completed
 	//------------------------------------------------------------------------	
	iPhone.SetLockSetting(status, lock, lockSetting);
	
	TInt res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrNone, res)
	
	//SetLockSetting should be completed with KErrAbort.
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrAbort, status.Int()) 
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(1, this); //this

	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MASC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::AbortSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AbortSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestAbortSecurityCode0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	
	TRequestStatus status;
	RMobilePhone::TMobilePhoneFdnSetting fdnSetting = RMobilePhone::EFdnSetOn;
	
	//-------------------------------------------------------------------------
	// Verify that SetFdnSetting will be completed
 	//-------------------------------------------------------------------------	
	iPhone.SetFdnSetting(status, fdnSetting);
	
	TInt res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrNone, res)
	
	//SetFdnSetting should be completed with KErrAbort.
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrAbort, status.Int()) 
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MCSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ChangeSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ChangeSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestChangeSecurityCode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	TRequestStatus reqStatus;

	_LIT(KOldPwd,"old");
	_LIT(KNewPwd,"new");

	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhonePasswordChangeV1 pwdChange;
	pwdChange.iNewPassword.Copy(KNewPwd);
	pwdChange.iOldPassword.Copy(KOldPwd);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhonePasswordChangeV1> pwdData(secCode,pwdChange);
 	pwdData.SerialiseL(data);
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//------------------------------------------------------------------------- 	
 	iMockLTSY.ExpectL(EMobilePhoneChangeSecurityCode,data,KErrNotSupported);

	iPhone.ChangeSecurityCode(reqStatus,secCode, pwdChange);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
 	iMockLTSY.ExpectL(EMobilePhoneChangeSecurityCode,data);
 	iMockLTSY.CompleteL(EMobilePhoneChangeSecurityCode, KErrGeneral, dataComplete);

	iPhone.ChangeSecurityCode(reqStatus,secCode, pwdChange);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::ChangeSecurityCode when result is not cached.
 	//-------------------------------------------------------------------------		
 	iMockLTSY.ExpectL(EMobilePhoneChangeSecurityCode,data);
 	iMockLTSY.CompleteL(EMobilePhoneChangeSecurityCode, KErrNone, dataComplete);

	iPhone.ChangeSecurityCode(reqStatus,secCode, pwdChange);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::ChangeSecurityCode
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(reqStatus);
	iMockLTSY.CompleteL(EMobilePhoneChangeSecurityCode, KErrNone, dataComplete);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, dataComplete, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MCSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::ChangeSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::ChangeSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestChangeSecurityCode0002L()
	{

// This test should test cancellation of ChangeSecurityCode
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	TRequestStatus reqStatus;

	_LIT(KOldPwd,"old");
	_LIT(KNewPwd,"new");

	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhonePasswordChangeV1 pwdChange;
	pwdChange.iNewPassword.Copy(KNewPwd);
	pwdChange.iOldPassword.Copy(KOldPwd);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhonePasswordChangeV1> pwdData(secCode,pwdChange);
 	pwdData.SerialiseL(data);
 	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::ChangeSecurityCode
 	//------------------------------------------------------------------------- 	
 	iMockLTSY.ExpectL(EMobilePhoneChangeSecurityCode, data);
	iPhone.ChangeSecurityCode(reqStatus, secCode, pwdChange);	
	// send canceling...
	iPhone.CancelAsyncRequest(EMobilePhoneChangeSecurityCode);
	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneChangeSecurityCode, KErrNone, dataComplete);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); //Common TSY doesn't cancel this function.
	
	AssertMockLtsyStatusL();

	// Wait for completion of iMockLTSY.NotifyTerminated
	// User::WaitForRequest(mockLtsyStatus);
	CleanupStack::PopAndDestroy(3); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MCSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::ChangeSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::ChangeSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestChangeSecurityCode0004L()
	{					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus reqStatus;

	_LIT(KOldPwd,"old");
	_LIT(KNewPwd,"new");

	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhonePasswordChangeV1 pwdChange;
	pwdChange.iNewPassword.Copy(KNewPwd);
	pwdChange.iOldPassword.Copy(KOldPwd);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhonePasswordChangeV1> pwdData(secCode,pwdChange);
 	pwdData.SerialiseL(data);
 	
 	TRequestStatus reqStatus2;

	_LIT(KOldPwd2,"old2");
	_LIT(KNewPwd2,"new2");

	RMobilePhone::TMobilePhoneSecurityCode secCode2 = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhonePasswordChangeV1 pwdChange2;
	pwdChange2.iNewPassword.Copy(KNewPwd2);
	pwdChange2.iOldPassword.Copy(KOldPwd2);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhonePasswordChangeV1> pwdData2(secCode2,pwdChange2);
 	pwdData2.SerialiseL(data2);
 	
 	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::ChangeSecurityCode
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneChangeSecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneChangeSecurityCode, KErrNone, dataComplete);
	
	iMockLTSY.ExpectL(EMobilePhoneChangeSecurityCode, data2);
	iMockLTSY.CompleteL(EMobilePhoneChangeSecurityCode, KErrNone, dataComplete);
	
	iPhone.ChangeSecurityCode(reqStatus, secCode, pwdChange);	
	phone2.ChangeSecurityCode(reqStatus2, secCode2, pwdChange2);	
		
		
	// wait for first phone SetLockSetting
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
		
	// wait for second phone SetLockSetting.
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(6, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MCSC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ChangeSecurityCode with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ChangeSecurityCode and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestChangeSecurityCode0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;

	_LIT(KOldPwd,"old");
	_LIT(KNewPwd,"new");

	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhonePasswordChangeV1 pwdChange;
	pwdChange.iNewPassword.Copy(KNewPwd);
	pwdChange.iOldPassword.Copy(KOldPwd);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhonePasswordChangeV1> pwdData(secCode,pwdChange);
 	pwdData.SerialiseL(data);
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::ChangeSecurityCode
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneChangeSecurityCode, data);
	iPhone.ChangeSecurityCode(reqStatus, secCode, pwdChange);
	
	User::WaitForRequest(reqStatus);
	
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int())
	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data, KErrNotSupported);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrGeneral, dataComplete);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::VerifySecurityCode when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataComplete);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::VerifySecurityCode
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(reqStatus);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataComplete);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobilePhone::VerifySecurityCode
    // from LTSY.
    //-------------------------------------------------------------------------	
	AssertMockLtsyStatusL();
	
	_LIT(KGoodPin,"12345");
	_LIT(KBadPin1,"12");
	_LIT(KBadPin2,"1a345");
    _LIT(KGoodPuk,"12345678");
    _LIT(KBadPuk1,"1");
    _LIT(KBadPuk2,"12345z78");    

    // short PIN
    secCode = RMobilePhone::ESecurityCodePin1;
    iPhone.VerifySecurityCode(reqStatus, secCode, KBadPin1(), KGoodPuk());
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());
    
    // PIN with non-numeric characters
    secCode = RMobilePhone::ESecurityCodePin2;
    iPhone.VerifySecurityCode(reqStatus, secCode, KBadPin2(), KGoodPuk());
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());
    
    // short PUK
    secCode = RMobilePhone::ESecurityCodePuk1;
    iPhone.VerifySecurityCode(reqStatus, secCode, KGoodPin(), KBadPuk1());
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());
 
    // PUK with non-numeric characters
    secCode = RMobilePhone::ESecurityCodePuk2;
    iPhone.VerifySecurityCode(reqStatus, secCode, KGoodPin(), KBadPuk2());
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());
    
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0002L()
	{

// This test should test cancellation of VerifySecurityCode
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::VerifySecurityCode
 	//------------------------------------------------------------------------- 	
 	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	iPhone.CancelAsyncRequest(EMobilePhoneVerifySecurityCode);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataComplete);
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(reqStatus);
	
	ASSERT_EQUALS(KErrNone, reqStatus.Int()) //Common TSY doesn't cancel this function.
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // data, this	
	}



/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0004L()
	{					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
	_LIT(KPwd2,"pwd2");
	_LIT(KUnblockPwd2, "unblock2");

	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneSecurityCode secCode2 = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePassword pwd2;
	RMobilePhone::TMobilePassword unblockPwd2;
	
	pwd2.Copy(KPwd2);
	unblockPwd2.Copy(KUnblockPwd2);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes2;
	twoCodes2.iCode.Copy(KPwd2);
	twoCodes2.iUnblockCode.Copy(KUnblockPwd2);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData2(secCode2,twoCodes2);
	verifyData2.SerialiseL(data2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::VerifySecurityCode
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataComplete);
	
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataComplete);
	
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);	
	phone2.VerifySecurityCode(reqStatus2, secCode2, pwd2, unblockPwd2);	
		
		
	// wait for first phone SetLockSetting
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());	
		
	// wait for second phone SetLockSetting.
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());
	
	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(6, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::VerifySecurityCode
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	_LIT(KEmpty,"");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KEmpty);
	unblockPwd.Copy(KEmpty);
	
 	//-------------------------------------------------------------------------	
	// RMobilePhone::VerifySecurityCode when code length is 0.
 	//-------------------------------------------------------------------------
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePuk1;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	//
	// RMobilePhone::VerifySecurityCode when security code is ESecurityCodePuk1 or ESecurityCodePuk2.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataComplete);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePuk1;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
		
	RBuf8 dataSls;
	CleanupClosePushL(dataSls);
	
	TRequestStatus status;
	RMobilePhone::TMobilePhoneLockStatus lockStatus = RMobilePhone::EStatusLocked;
	TLockSettingPassword lockSettingPassword;
	lockSettingPassword.iPassword.Copy(KNullDesC);
	RMobilePhone::TMobilePhoneLock& lock( lockSettingPassword.iLock );
	RMobilePhone::TMobilePhoneLockSetting& lockSetting( lockSettingPassword.iSetting );
	lock        = RMobilePhone::ELockHiddenKey;
	lockSetting = RMobilePhone::ELockSetEnabled;
	TMockLtsyData1<TLockSettingPassword> phoneLockData(lockSettingPassword);
	phoneLockData.SerialiseL(dataSls);
	
	RBuf8 dataSls_;
	CleanupClosePushL(dataSls_);
	
	RBuf8 dataCompleteSls;
	CleanupClosePushL(dataCompleteSls);
	
	TLockSettingPassword lockSettingPassword_;
	lockSettingPassword_.iPassword.Copy(KPwd);
	RMobilePhone::TMobilePhoneLock& lock_( lockSettingPassword_.iLock );
	RMobilePhone::TMobilePhoneLockSetting& lockSetting_( lockSettingPassword_.iSetting );
	lock_        = RMobilePhone::ELockPhoneDevice;
	lockSetting_ = RMobilePhone::ELockSetUnknown;
	TMockLtsyData1<TLockSettingPassword> phoneLockData_(lockSettingPassword_);
	phoneLockData_.SerialiseL(dataSls_);
	
	TRequestStatus mockLtsyStatus;
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> lockStatusData(lockStatus,lockSetting);
	lockStatusData.SerialiseL(dataCompleteSls);
	
 	//-------------------------------------------------------------------------
	//
	// RMobilePhone::VerifySecurityCode when SetLockSetting is called.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetLockSetting, dataSls);
	iPhone.SetLockSetting(status, lock, lockSetting);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.ExpectL(EMobilePhoneSetLockSetting, dataSls_);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);

	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataComplete);
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePuk1;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
	
	TRequestStatus status;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockUniversalPin;
	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockReplaced;
	
 	//-------------------------------------------------------------------------
	//
	// RMobilePhone::VerifySecurityCode when security code is ESecurityCodePuk1 or ESecurityCodePuk2.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data, KErrGeneral);
	iPhone.SetLockSetting(status, lock, lockSetting);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrGeneral, status.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0010L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePuk1;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
	
	TRequestStatus status;
	RMobilePhone::TMobilePhoneFdnSetting fdnSetting = RMobilePhone::EFdnSetOn;
	
 	//-------------------------------------------------------------------------
	//
	// RMobilePhone::VerifySecurityCode when security code is ESecurityCodePuk1 or ESecurityCodePuk2.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data, KErrGeneral);
	iPhone.SetFdnSetting(status, fdnSetting);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrGeneral, status.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePuk1;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
		
	TRequestStatus status;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockUniversalPin;
	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockReplaced;
	
 	//-------------------------------------------------------------------------
	//
	// RMobilePhone::VerifySecurityCode when security code is ESecurityCodePuk1 or ESecurityCodePuk2.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrGeneral, dataComplete);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	iPhone.SetLockSetting(status, lock, lockSetting);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrAccessDenied, status.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	// Initialize data for VerifySecurityEvent

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePuk1;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
	//Initialize data for SetLockSetting	
	TRequestStatus status;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockUniversalPin;
	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockReplaced;
	
	
 	//-------------------------------------------------------------------------
	// RMobilePhone::VerifySecurityCode 
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataComplete);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	iPhone.SetLockSetting(status, lock, lockSetting);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrAccessDenied, status.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0013L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	// Initialize data for VerifySecurityEvent
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePuk1;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
	//Initialize data for SetFdnSetting	
	RBuf8 dataFdn;
	CleanupClosePushL(dataFdn);
	
	TRequestStatus status;
	RMobilePhone::TMobilePhoneFdnSetting fdnSetting = RMobilePhone::EFdnSetOff;
	TMockLtsyData1<RMobilePhone::TMobilePhoneFdnSetting> fdnData(fdnSetting);
	fdnData.SerialiseL(dataFdn);
	
 	//-------------------------------------------------------------------------
	//
	// RMobilePhone::VerifySecurityCode complete with KErrGeneral
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrGeneral, dataComplete);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	iPhone.SetFdnSetting(status, fdnSetting);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrGeneral, status.Int())
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// Now complete it with KErrNone
	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataComplete);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	iPhone.SetFdnSetting(status, fdnSetting);

	//CompleteVerifySecurityL will transfer SetFdnSetting call to MockL.
	iMockLTSY.ExpectL(EMobilePhoneSetFdnSetting, dataFdn);
	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int())
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MVSC-0014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestVerifySecurityCode0014L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblock");

	// Initialize data for VerifySecurityEvent

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(data);
	
	//Initialize data for SetLockSetting
	
 	//-------------------------------------------------------------------------
	//
	// RMobilePhone::VerifySecurityCode 
 	//-------------------------------------------------------------------------
 	TRequestStatus mockLtsyStatus;
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
 	iMockLTSY.CompleteL(EMmTsyBootNotifySecurityReadyIPC, KErrNone);
 	User::WaitForRequest(mockLtsyStatus);
 	
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, data);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrGeneral, dataComplete);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this	
	}




/**
@SYMTestCaseID BA-CTSY-PSEC-MNSE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySecurityEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityEvent0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	TRequestStatus mockLtsyStatus;

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEventRes;
	RMobilePhone::TMobilePhoneSecurityEvent secEvent = RMobilePhone::EPhonePasswordRequired;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> eventData(secEvent);
	eventData.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMmTsySimGetICCType, dataExpect);
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrGeneral, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifySecurityEvent when result is not cached.
 	//-------------------------------------------------------------------------	
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(secEvent, secEventRes)
	AssertMockLtsyStatusL();


 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifySecurityEvent
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::NotifySecurityEvent again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
 	secEventRes = RMobilePhone::EPhonePasswordVerified; //change the value to make shure that NotifySecurityEvent will write the correct value to secEventRes.
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(secEvent, secEventRes)

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MNSE-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifySecurityEvent
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifySecurityEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityEvent0002L()
	{

// This test should test cancellation of NotifySecurityEvent
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);
	

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEventRes;
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifySecurityEvent
 	//------------------------------------------------------------------------- 	
	iMockLTSY.ExpectL(EMmTsySimGetICCType, data);
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifySecurityEvent);
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int())
	AssertMockLtsyStatusL();

	// Wait for completion of iMockLTSY.NotifyTerminated
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-PSEC-MNSE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifySecurityEvent
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifySecurityEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityEvent0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEventRes;
	RMobilePhone::TMobilePhoneSecurityEvent secEvent = RMobilePhone::EPhonePasswordVerified;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> eventData(secEvent);
	eventData.SerialiseL(data);
	
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneSecurityEvent secEventRes2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifySecurityEvent
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(EMmTsySimGetICCType, dataExpect);
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	phone2.NotifySecurityEvent(reqStatus2, secEventRes2);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);
	
	User::WaitForRequest(reqStatus);
	User::WaitForRequest(reqStatus2);
	
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(secEvent, secEventRes)
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())
	ASSERT_EQUALS(secEvent, secEventRes2)
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, dataExpect, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MNSE-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySecurityEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityEvent0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	RBuf8 dataVerify;
	CleanupClosePushL(dataVerify);
	
	RBuf8 dataCompleteVerify;
	CleanupClosePushL(dataCompleteVerify);
	
	TRequestStatus mockLtsyStatus;

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEventRes;
	RMobilePhone::TMobilePhoneSecurityEvent secEvent = RMobilePhone::EPin1Verified;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> eventData(secEvent);
	eventData.SerialiseL(data);
	
	
	_LIT(KPwd,"1234");
	_LIT(KUnblockPwd, "12345678");

	TRequestStatus reqStatusVerify;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);

	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode,twoCodes);
	verifyData.SerialiseL(dataVerify);
	
	//--------------------------------------------------------------------------------
	//for coverage increase
	//--------------------------------------------------------------------------------
		
	iMockLTSY.ExpectL(EMmTsySimGetICCType, dataExpect);
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	
	iMockLTSY.ExpectL(EMobilePhoneVerifySecurityCode, dataVerify);
	iMockLTSY.CompleteL(EMobilePhoneVerifySecurityCode, KErrNone, dataCompleteVerify);
	iPhone.VerifySecurityCode(reqStatusVerify, secCode, pwd, unblockPwd);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(secEvent, secEventRes)
	
	User::WaitForRequest(reqStatusVerify);
	ASSERT_EQUALS(KErrNone, reqStatusVerify.Int())
	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(5, this); 

	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MNSE-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySecurityEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityEvent0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus mockLtsyStatus;

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEvent = RMobilePhone::EPin2Required;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> eventData(secEvent);
	eventData.SerialiseL(data);
	
	//--------------------------------------------------------------------------------
	// Unsolicited completion of RMobilePhone::NotifySecurityEvent
	// from LTSY. when event is EPin2Required.
	//--------------------------------------------------------------------------------	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())	
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(2, this); 

	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MNSE-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySecurityEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityEvent0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus mockLtsyStatus;

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEvent = RMobilePhone::EPhonePasswordRequired;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> eventData(secEvent);
	eventData.SerialiseL(data);
	
	//--------------------------------------------------------------------------------
	//for coverage increase
	//--------------------------------------------------------------------------------	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(2, this); 

	}



/**
@SYMTestCaseID BA-CTSY-PSEC-MNSE-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySecurityEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityEvent0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus mockLtsyStatus;

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEvent = RMobilePhone::EPhonePasswordRequired;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> eventData(secEvent);
	eventData.SerialiseL(data);
	
	RBuf8 dataSls;
	CleanupClosePushL(dataSls);
	
	TRequestStatus status;
	RMobilePhone::TMobilePhoneLockStatus lockStatus = RMobilePhone::EStatusLocked;
	TLockSettingPassword lockSettingPassword;
	lockSettingPassword.iPassword.Copy(KNullDesC);
	RMobilePhone::TMobilePhoneLock& lock( lockSettingPassword.iLock );
	RMobilePhone::TMobilePhoneLockSetting& lockSetting( lockSettingPassword.iSetting );
	lock        = RMobilePhone::ELockPhoneDevice;//ELockUSimApp;
	lockSetting = RMobilePhone::ELockSetEnabled;
	TMockLtsyData1<TLockSettingPassword> phoneLockData(lockSettingPassword);
	phoneLockData.SerialiseL(dataSls);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> lockStatusData(lockStatus,lockSetting);
	
	//--------------------------------------------------------------------------------
	//for coverage increase
	//--------------------------------------------------------------------------------
	iPhone.SetLockSetting(status, lock, lockSetting);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	// Request status passed to set lock setting should be completed with KErrACcessDenied.
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrAccessDenied, status.Int())
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(3, this); 

	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MNSE-0010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySecurityEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityEvent0010L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus mockLtsyStatus;

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEvent = RMobilePhone::EPhonePasswordRequired;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> eventData(secEvent);
	eventData.SerialiseL(data);
	
	//--------------------------------------------------------------------------------
	//for coverage increase
	//--------------------------------------------------------------------------------	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(2, this); 

	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MNLIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyLockInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyLockInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifyLockInfoChange0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data3G;
	CleanupClosePushL(data3G);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockPhoneDevice;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfoV1;
	lockInfoV1.iSetting = RMobilePhone::ELockSetEnabled;
	lockInfoV1.iStatus = RMobilePhone::EStatusLocked;
	
	RMobilePhone::TMobilePhoneLockInfoV1Pckg pckgData(lockInfoV1);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockInfoV1, RMobilePhone::TMobilePhoneLock> lockInfoData(lockInfoV1, lock);
	lockInfoData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyLockInfoChange when result is not cached.
 	//-------------------------------------------------------------------------
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	//Repeat the previos tesat for coverage increase	
 	//------------------------------------------------------------------------- 	
 	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	// TEST C:  for coverage increasing with RMobilePhone::ELockPhoneToICC 
	// as parameter 	
 	//-------------------------------------------------------------------------
 	lock = RMobilePhone::ELockPhoneToICC;
 	
 	TMockLtsyData2<RMobilePhone::TMobilePhoneLockInfoV1, RMobilePhone::TMobilePhoneLock> 
 								lockInfoData2(lockInfoV1, lock);
	data.Close();
	lockInfoData2.SerialiseL(data);
 	
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();		
	
	//-------------------------------------------------------------------------
	// TEST C:  repeat the test with RMobilePhone::ELockPhoneToICC 
	// as parameter for coverage increase
 	//-------------------------------------------------------------------------
 	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();		
	
	//-------------------------------------------------------------------------
	// TEST C:  for coverage increasing with RMobilePhone::ELockUniversalPin
	// as parameter 	
 	//-------------------------------------------------------------------------
 	
 	lock = RMobilePhone::ELockUniversalPin;
 	
 	TMockLtsyData2<RMobilePhone::TMobilePhoneLockInfoV1, RMobilePhone::TMobilePhoneLock> 
 								lockInfoData3(lockInfoV1, lock);
	data.Close();
	lockInfoData3.SerialiseL(data);
 	
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();	
		
	
	//-------------------------------------------------------------------------
	// TEST C:  for coverage increasing with RMobilePhone::ELockUniversalPin
	// as parameter and iCurrentICCType == EICCTypeSim3G  	
 	//-------------------------------------------------------------------------
 	
    //----------- making ICC3G ----------------------------
    TRequestStatus mockLtsyStatus;
    
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TICCType type(EICCTypeSim3G);

	TMockLtsyData1<TICCType> iccData(type);
	iccData.SerialiseL(data3G);

	iMockLTSY.CompleteL(EMmTsySimGetICCType, KErrNone, data3G);    

	iMockLTSY.ExpectL(EMmTsySecurityGetSimActivePinStateIPC);
	iMockLTSY.CompleteL(EMmTsySecurityGetSimActivePinStateIPC, KErrNone);

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
 	//------------------------------------------------------
 	
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	// TEST C:  repeat the test with RMobilePhone::ELockUniversalPin
	// as parameter for coverage increase
 	//-------------------------------------------------------------------------
 	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	// TEST C:  for coverage increasing with RMobilePhone::ELockICC 
	// as parameter 	
 	//-------------------------------------------------------------------------
 	lock = RMobilePhone::ELockICC;
 	
 	TMockLtsyData2<RMobilePhone::TMobilePhoneLockInfoV1, RMobilePhone::TMobilePhoneLock> 
 								lockInfoData4(lockInfoV1, lock);
	data.Close();
	lockInfoData4.SerialiseL(data);
 	
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();			
	
	//-------------------------------------------------------------------------
	// TEST C:  repeat the test with RMobilePhone::ELockICC 
	// as parameter for coverage increase
 	//-------------------------------------------------------------------------
 	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();		
		
	//-------------------------------------------------------------------------
	// TEST C:  for coverage increasing with RMobilePhone::ELockPhoneToFirstICC
	// as parameter 	
 	//-------------------------------------------------------------------------
 	lock = RMobilePhone::ELockPhoneToFirstICC;
 	
 	TMockLtsyData2<RMobilePhone::TMobilePhoneLockInfoV1, RMobilePhone::TMobilePhoneLock> 
 								lockInfoData5(lockInfoV1, lock);
	data.Close();
	lockInfoData5.SerialiseL(data);
 	
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();	

 	
    //-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrGeneral, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobilePhone::NotifyLockInfoChange
    // from LTSY.
    //-------------------------------------------------------------------------
    iMockLTSY.NotifyTerminated(reqStatus);
    iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int())
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data3G, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MNLIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyLockInfoChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyLockInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifyLockInfoChange0002L()
	{

// This test should test cancellation of NotifyLockInfoChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockPhoneDevice;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfoV1;
	lockInfoV1.iSetting = RMobilePhone::ELockSetEnabled;
	lockInfoV1.iStatus = RMobilePhone::EStatusLocked;
	
	RMobilePhone::TMobilePhoneLockInfoV1Pckg pckgData(lockInfoV1);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockInfoV1, RMobilePhone::TMobilePhoneLock> lockInfoData(lockInfoV1, lock);
	lockInfoData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyLockInfoChange
 	//------------------------------------------------------------------------- 	
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyLockInfoChange);
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MNLIC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyLockInfoChange with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyLockInfoChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifyLockInfoChange0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::NotifyLockInfoChange
 	//-------------------------------------------------------------------------
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockPhoneDevice;
	TBuf8<1> lockInfo;
	
	iPhone.NotifyLockInfoChange(reqStatus, lock, lockInfo);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int())
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(this); // this
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MNLIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyLockInfoChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyLockInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifyLockInfoChange0004L()
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
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus reqStatus;
	TRequestStatus reqStatus2;
	
	//Data for iMockLTSY->CompleteL
	RMobilePhone::TMobilePhoneLock lockComplete = RMobilePhone::ELockPhoneDevice;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfoV1;
	lockInfoV1.iSetting = RMobilePhone::ELockSetEnabled;
	lockInfoV1.iStatus = RMobilePhone::EStatusLocked;
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneLockInfoV1, RMobilePhone::TMobilePhoneLock> lockInfoData(lockInfoV1, lockComplete);
	lockInfoData.SerialiseL(data);
	
	//Variables that will contain result data after request status will be completed.
	RMobilePhone::TMobilePhoneLock lock1;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg pckgData(lockInfo);
	
	//Variables that will contain result data after second request status will be completed.
	RMobilePhone::TMobilePhoneLock lock2;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo2;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg pckgData2(lockInfo2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyLockInfoChange
 	//-------------------------------------------------------------------------
	
	iPhone.NotifyLockInfoChange(reqStatus, lock1, pckgData);
	phone2.NotifyLockInfoChange(reqStatus2, lock2, pckgData2);
	
	iMockLTSY.CompleteL(EMobilePhoneNotifyLockInfoChange, KErrNone, data);
	
	User::WaitForRequest(reqStatus);
	User::WaitForRequest(reqStatus2);
	
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData2().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData2().iStatus)
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MGSCI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSecurityCodeInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSecurityCodeInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetSecurityCodeInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV5;
	codeInfoV5.iRemainingEntryAttempts = 4;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData(secCode);
	codeData.SerialiseL(dataExpect);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData(secCode, codeInfoV5);
	codeInfoData.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//------------------------------------------------------------------------- 	
 	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, dataExpect, KErrNotSupported);
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	AssertMockLtsyStatusL();
 	
 	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//------------------------------------------------------------------------- 	
 	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneGetSecurityCodeInfo, KErrGeneral, data);
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetSecurityCodeInfo when result is not cached.
 	//------------------------------------------------------------------------- 	
	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneGetSecurityCodeInfo, KErrNone, data);
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(codeInfoV5.iRemainingEntryAttempts, pckgData().iRemainingEntryAttempts)
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetSecurityCodeInfo
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetSecurityCodeInfo, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MGSCI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetSecurityCodeInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetSecurityCodeInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetSecurityCodeInfo0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV5;
	codeInfoV5.iRemainingEntryAttempts = 4;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData(secCode);
	codeData.SerialiseL(dataExpect);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData(secCode, codeInfoV5);
	codeInfoData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetSecurityCodeInfo
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, dataExpect);
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);
	iPhone.CancelAsyncRequest(EMobilePhoneGetSecurityCodeInfo);
	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	// Wait for completion of GetSecurityCodeInfo
	User::WaitForRequest(reqStatus);
	// Check status is KErrCancel
	ASSERT_EQUALS(KErrCancel,reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // data, this
	}
	
/**
@SYMTestCaseID BA-CTSY-PSEC-MGSCI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSecurityCodeInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSecurityCodeInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetSecurityCodeInfo0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	TBuf8<1> smallBuf;
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetSecurityCodeInfo
 	//-------------------------------------------------------------------------
	// CTSY should return an error straight away

	iPhone.GetSecurityCodeInfo(reqStatus, secCode, smallBuf);
    User::WaitForRequest(reqStatus);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());

	// Done !
	CleanupStack::PopAndDestroy(this); // this
	}
	
	
/**
@SYMTestCaseID BA-CTSY-PSEC-MGSCI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetSecurityCodeInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetSecurityCodeInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetSecurityCodeInfo0004L()
	{			
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	RBuf8 dataComplete2;
	CleanupClosePushL(dataComplete2);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV5;
	codeInfoV5.iRemainingEntryAttempts = 4;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData(secCode);
	codeData.SerialiseL(data);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData(secCode, codeInfoV5);
	codeInfoData.SerialiseL(dataComplete);
	
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneSecurityCode secCode2 = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV52;
	codeInfoV52.iRemainingEntryAttempts = 3;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV52;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData2(codeV52);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData2(secCode2);
	codeData2.SerialiseL(data2);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData2(secCode2, codeInfoV52);
	codeInfoData2.SerialiseL(dataComplete2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetSecurityCodeInfo
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, data);
	
	iMockLTSY.CompleteL(EMobilePhoneGetSecurityCodeInfo, KErrNone, dataComplete);
	
	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, data2);
	iMockLTSY.CompleteL(EMobilePhoneGetSecurityCodeInfo, KErrNone, dataComplete2);
	
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);	
	phone2.GetSecurityCodeInfo(reqStatus2, secCode2, pckgData2);	
		
		
	// wait for first phone GetSecurityCodeInfo
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(codeInfoV5.iRemainingEntryAttempts, pckgData().iRemainingEntryAttempts)
	
		
	// wait for second phone GetsecurityCodeInfo.
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(codeInfoV52.iRemainingEntryAttempts, pckgData2().iRemainingEntryAttempts)
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, data, this

	}
	
/**
@SYMTestCaseID BA-CTSY-PSEC-MGSCI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSecurityCodeInfo with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSecurityCodeInfo and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetSecurityCodeInfo0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV5;
	codeInfoV5.iRemainingEntryAttempts = 4;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData(secCode);
	codeData.SerialiseL(data);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData(secCode, codeInfoV5);
	codeInfoData.SerialiseL(dataComplete);
	//-------------------------------------------------------------------------
	// Test A: Send multiple client requests RMobilePhone::ChangeSecurityCode
	//			One request successfully returns, other one timed out.
	//			Multiple request is sent just to test that the proper one is timed out.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, data);
	iMockLTSY.CompleteL(EMobilePhoneGetSecurityCodeInfo, KErrNone, dataComplete);

	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneSecurityCode secCode2 = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV52;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData2(codeV52);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData2(secCode2);
	codeData2.SerialiseL(data2);

	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, data2);

	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);

	phone2.GetSecurityCodeInfo(reqStatus2, secCode2, pckgData2);

	// wait for first phone GetSecurityCodeInfo
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(codeInfoV5.iRemainingEntryAttempts, pckgData().iRemainingEntryAttempts)

	// wait for second phone GetsecurityCodeInfo which will be timed out.
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrTimedOut, reqStatus2.Int())

	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(6, this); // data, this

	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MGSCI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSecurityCodeInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSecurityCodeInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetSecurityCodeInfo0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	RBuf8 dataComplete2;
	CleanupClosePushL(dataComplete2);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV5;
	codeInfoV5.iRemainingEntryAttempts = 4;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData(secCode);
	codeData.SerialiseL(data);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData(secCode, codeInfoV5);
	codeInfoData.SerialiseL(dataComplete);
	
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneSecurityCode secCode2 = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV52;
	codeInfoV52.iRemainingEntryAttempts = 3;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV52;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData2(codeV52);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData2(secCode2);
	codeData2.SerialiseL(data2);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData2(secCode2, codeInfoV52);
	codeInfoData2.SerialiseL(dataComplete2);
	
	//----------------------------------------------------------------------------
	// Covering condition when there is already one request for security code and
	// a second one is done.
	//----------------------------------------------------------------------------
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 71003);
	ASSERT_TRUE(EFalse)
	
	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, data);
	iMockLTSY.CompleteL(EMobilePhoneGetSecurityCodeInfo, KErrNone, dataComplete);
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);
	
	iMockLTSY.CompleteL(EMobilePhoneGetSecurityCodeInfo, KErrNone, dataComplete2);	
	phone2.GetSecurityCodeInfo(reqStatus2, secCode, pckgData2);	
		
		
	// wait for first phone GetSecurityCodeInfo
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(codeInfoV5.iRemainingEntryAttempts, pckgData().iRemainingEntryAttempts)
	
		
	// wait for second phone GetsecurityCodeInfo.
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(codeInfoV52.iRemainingEntryAttempts, pckgData2().iRemainingEntryAttempts)
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MGSCI-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSecurityCodeInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSecurityCodeInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestGetSecurityCodeInfo0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	RBuf8 dataComplete2;
	CleanupClosePushL(dataComplete2);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV5;
	codeInfoV5.iRemainingEntryAttempts = 4;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData(secCode);
	codeData.SerialiseL(data);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData(secCode, codeInfoV5);
	codeInfoData.SerialiseL(dataComplete);
	
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneSecurityCode secCode2 = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV52;
	codeInfoV52.iRemainingEntryAttempts = 3;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV52;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData2(codeV52);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData2(secCode2);
	codeData2.SerialiseL(data2);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData2(secCode2, codeInfoV52);
	codeInfoData2.SerialiseL(dataComplete2);
	
	//-----------------------------------------------------------------------------
	// Cover condition if there are more than one GetSecurityCodeInfo request when cancelling some request.
	//-----------------------------------------------------------------------------
	// This defect was fixed. For more information see defect description 
	//ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 71004);
	
	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, data);
	iMockLTSY.CompleteL(EMobilePhoneGetSecurityCodeInfo, KErrNone, dataComplete);
	
	iMockLTSY.ExpectL(EMobilePhoneGetSecurityCodeInfo, data2);
	
	
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);	
	phone2.GetSecurityCodeInfo(reqStatus2, secCode2, pckgData2);
	phone2.CancelAsyncRequest(EMobilePhoneGetSecurityCodeInfo);
		
	User::WaitForRequest(mockLtsyStatus);		
		
	// wait for second phone GetsecurityCodeInfo.
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrCancel, reqStatus2.Int());
	
	
	// wait for first phone GetSecurityCodeInfo
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(codeInfoV5.iRemainingEntryAttempts, pckgData().iRemainingEntryAttempts)
	
	AssertMockLtsyStatusL();
	


	// Done !
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-PSEC-MNSCIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySecurityCodeInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityCodeInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityCodeInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV5;
	codeInfoV5.iRemainingEntryAttempts = 4;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData(secCode, codeInfoV5);
	codeInfoData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//------------------------------------------------------------------------- 	
	iPhone.NotifySecurityCodeInfoChange(reqStatus, secCode, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityCodeInfoChange, KErrGeneral, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetSecurityCodeInfo when result is not cached.
 	//------------------------------------------------------------------------- 	
	iPhone.NotifySecurityCodeInfoChange(reqStatus, secCode, pckgData);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityCodeInfoChange, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(codeInfoV5.iRemainingEntryAttempts, pckgData().iRemainingEntryAttempts)
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetSecurityCodeInfo
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityCodeInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PSEC-MNSCIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetSecurityCodeInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetSecurityCodeInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityCodeInfoChange0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV5;
	codeInfoV5.iRemainingEntryAttempts = 4;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData(secCode, codeInfoV5);
	codeInfoData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetLockInfo
 	//-------------------------------------------------------------------------	
	iPhone.NotifySecurityCodeInfoChange(reqStatus, secCode, pckgData);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifySecurityCodeInfoChange);
	// Wait for completion of iMockLTSY.NotifyTerminated
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityCodeInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	// Wait for completion of GetSecurityCodeInfo
	User::WaitForRequest(reqStatus);
	// Check status is KErrCancel
	ASSERT_EQUALS(KErrCancel,reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // data, this
	}
	
/**
@SYMTestCaseID BA-CTSY-PSEC-MNSCIC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSecurityCodeInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityCodeInfoChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityCodeInfoChange0003L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	TBuf8<1> smallBuf;
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetLockInfo
 	//-------------------------------------------------------------------------

	// CTSY should return an error straight away

	iPhone.NotifySecurityCodeInfoChange(reqStatus, secCode, smallBuf);
    User::WaitForRequest(reqStatus);
    
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
   	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy( this ); // this
	}
		
	
/**
@SYMTestCaseID BA-CTSY-PSEC-MNSCIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifySecurityCodeInfoChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifySecurityCodeInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityCodeInfoChange0004L()
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
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus reqStatus;
	TRequestStatus reqStatus2;
	
	RMobilePhone::TMobilePhoneSecurityCode secCodeComplete = RMobilePhone::ESecurityCodePhonePassword;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfoV5;
	codeInfoV5.iRemainingEntryAttempts = 4;
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData(secCodeComplete, codeInfoV5);
	codeInfoData.SerialiseL(data);
	
	// For first request
	RMobilePhone::TMobilePhoneSecurityCode secCode1 = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV51;
	codeV51.iRemainingEntryAttempts = 0;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData1(codeV51);
	
	// For second request
	RMobilePhone::TMobilePhoneSecurityCode secCode2 = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV52;
	codeV52.iRemainingEntryAttempts = 0;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData2(codeV52);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifySecurityCodeInfoChange
 	//-------------------------------------------------------------------------
	
	iPhone.NotifySecurityCodeInfoChange(reqStatus, secCode1, pckgData1);	
	phone2.NotifySecurityCodeInfoChange(reqStatus2, secCode2, pckgData2);	
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityCodeInfoChange, KErrNone, data);		
		
	// Wait for the first complete.
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(codeInfoV5.iRemainingEntryAttempts, pckgData1().iRemainingEntryAttempts);	
	ASSERT_EQUALS(secCodeComplete, secCode1);	
		
	// Wait for the second complete.
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(codeInfoV5.iRemainingEntryAttempts, pckgData2().iRemainingEntryAttempts);
	ASSERT_EQUALS(secCodeComplete, secCode2);		
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}
	

/**
@SYMTestCaseID BA-CTSY-NTWS-MNSCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySecurityCapsChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);


	// Check basic expected operation
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TUint32 oldCaps;
    iPhone.GetSecurityCaps(oldCaps);
    
	TRequestStatus requestStatus;
    TUint32 newCaps;

    //----------------------------
	iPhone.NotifySecurityCapsChange(requestStatus, newCaps);

	RMobilePhone::TMobilePhoneSecurityEvent ev(RMobilePhone::ENoICCFound);
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> evData(ev);

    ev = RMobilePhone::ENoICCFound;
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

	User::WaitForRequest(requestStatus);

    TUint32 capsToCompare(0);    

    if ( oldCaps & RMobilePhone::KCapsLockPhone )
        {
        capsToCompare |= RMobilePhone::KCapsLockPhone;
        }
    if ( oldCaps & RMobilePhone::KCapsAccessPhonePassword )
        {
        capsToCompare |= RMobilePhone::KCapsAccessPhonePassword;
        }

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(capsToCompare, newCaps);
    //----------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    ev = RMobilePhone::EICCTerminated;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);


	iPhone.NotifySecurityCapsChange(requestStatus, newCaps);

    ev = RMobilePhone::EPin1Required;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

    ev = RMobilePhone::EPuk1Required;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

    ev = RMobilePhone::EPin2Required;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

    ev = RMobilePhone::EPuk2Required;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

    ev = RMobilePhone::EPhonePasswordRequired;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

    ev = RMobilePhone::EPin1Verified;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

    ev = RMobilePhone::EPin2Verified;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

    ev = RMobilePhone::EPuk1Verified;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

    ev = RMobilePhone::EPuk2Verified;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

	User::WaitForRequest(requestStatus);

    capsToCompare = 0;

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(capsToCompare, newCaps);

    //- making ICC3G ----------------------------
    
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TICCType type(EICCTypeSim3G);

	TMockLtsyData1<TICCType> iccData(type);
	data.Close();
	iccData.SerialiseL(data);

	iMockLTSY.CompleteL(EMmTsySimGetICCType, KErrNone, data);    

	iMockLTSY.ExpectL(EMmTsySecurityGetSimActivePinStateIPC);
	iMockLTSY.CompleteL(EMmTsySecurityGetSimActivePinStateIPC, KErrNone);

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

    //--------------------------------------------

    iPhone.GetSecurityCaps(oldCaps);


	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iPhone.NotifySecurityCapsChange(requestStatus, newCaps);

	ev = RMobilePhone::EUniversalPinRequired;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

	ev = RMobilePhone::EUniversalPukRequired;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

	ev = RMobilePhone::EPhonePasswordRequired;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

	ev = RMobilePhone::EUniversalPinVerified;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

	ev = RMobilePhone::EUniversalPukVerified;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

	ev = RMobilePhone::EPhonePasswordVerified;
	data.Close();
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
		
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(oldCaps, newCaps);


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this

}

/**
@SYMTestCaseID BA-CTSY-NTWS-MNSCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifySecurityCapsChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifySecurityCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityCapsChange0002L()
	{

// This test should test cancellation of NotifySecurityCapsChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifySecurityCapsChange
 	//------------------------------------------------------------------------- 	
	TRequestStatus requestStatus;
	TUint32 caps;	

	iPhone.NotifySecurityCapsChange(requestStatus, caps);

	RMobilePhone::TMobilePhoneSecurityEvent ev(RMobilePhone::EICCTerminated);
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> evData(ev);
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data, 2);

    iPhone.CancelAsyncRequest(EMobilePhoneNotifySecurityCapsChange);

	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
   	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-NTWS-MNSCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifySecurityCapsChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifySecurityCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneSecurityFU::TestNotifySecurityCapsChange0004L()
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
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifySecurityCapsChange
 	//-------------------------------------------------------------------------
	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TUint32 oldCaps;	
    iPhone.GetSecurityCaps(oldCaps);
    
	TRequestStatus requestStatus1;
	TUint32 caps1;	
	iPhone.NotifySecurityCapsChange(requestStatus1, caps1);

	TRequestStatus requestStatus2;
	TUint32 caps2;	
	phone2.NotifySecurityCapsChange(requestStatus2, caps2);

	RMobilePhone::TMobilePhoneSecurityEvent ev(RMobilePhone::EICCTerminated);
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> evData(ev);
	evData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySecurityEvent, KErrNone, data);

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();


    TUint32 capsToCompare(0);	
	
    if ( oldCaps & RMobilePhone::KCapsLockPhone )
        {
        capsToCompare |= RMobilePhone::KCapsLockPhone;
        }

    if ( oldCaps & RMobilePhone::KCapsAccessPhonePassword )
        {
        capsToCompare |= RMobilePhone::KCapsAccessPhonePassword;
        }	

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	ASSERT_EQUALS(capsToCompare, caps1);

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(capsToCompare, caps2);


	AssertMockLtsyStatusL();
	
	// Done !	
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

