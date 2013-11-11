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
 @file The TEFUnit test suite for SecurityControl in the Common TSY.
*/

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include "mockltsyindicatorids.h"
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/rmmcustomapi.h>
#include <ctsy/serviceapi/cmmcustomsecuritytsyconstants.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include "cctsysecurityfu.h"

CTestSuite* CCTsySecurityFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	//add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUseCase0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUseCase0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUseCase0004L);
    
	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0015L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFU, TestUnit0016L);
	
	END_SUITE;
	}

//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetFdnSetting, RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetFdnSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySecurityFU::TestUseCase0001L()
	{
	/*
	 * This test uses VerifySecurityCode with SetFdnSetting to get the SetFdnSetting sent to the LTSY
	 */

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus, reqStatusNotif, reqStatusVerify;

	// Prepare data for mockLtsy for the VerifySecurityCode.
	// This mimics the scenario that Pin2 code is required to make SetFdnSetting complete successfully.
	// Therefore we use VerifySecurityCode to supply the Pin2 code.
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin2;

	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblockPwd");
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);

	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode, twoCodes);
	verifyData.SerialiseL(data);
	
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneSecurityEvent secEventRes;
	iMockLTSY.ExpectL(MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId);	
	iPhone.NotifySecurityEvent(reqStatus2, secEventRes);
	
	// Call the ipc for SetFdnSetting.
	// Also call NotifyFdnStatusChange as this is triggered by CTSY when SetFdnSetting completes
    RMobilePhone::TMobilePhoneFdnStatus  fdnStatus(RMobilePhone::EFdnUnknown);
    iPhone.NotifyFdnStatusChange(reqStatusNotif, fdnStatus);
	RMobilePhone::TMobilePhoneFdnSetting  fdnSetting(RMobilePhone::EFdnSetOff);
	iPhone.SetFdnSetting(reqStatus, fdnSetting);
	
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())		

	// Send the ExpectL and CompleteL for VerifySecurityCode
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrGeneral);

	// Call ipc for VerifySecurityCode
	iPhone.VerifySecurityCode(reqStatusVerify, secCode, pwd, unblockPwd);

	// Wait for VerifySecurityCode...
	User::WaitForRequest(reqStatusVerify);
	ASSERT_EQUALS(KErrGeneral, reqStatusVerify.Int());
	AssertMockLtsyStatusL();

	// Wait for SetFdnSetting...
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	iPhone.NotifySecurityEvent(reqStatus2, secEventRes);
	
	iPhone.SetFdnSetting(reqStatus, fdnSetting);
	
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())			
	
	// Send the ExpectL and CompleteL for VerifySecurityCode
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrNone);
	
	// Send ExpectL for SetFdnSetting
	data.Close();
	TMockLtsyData1<RMobilePhone::TMobilePhoneFdnSetting> setData(fdnSetting);
	data.Close();
	setData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId, data, KErrNone);
		
	// Call ipc for VerifySecurityCode
	iPhone.VerifySecurityCode(reqStatusVerify, secCode, pwd, unblockPwd);
	
	// Wait for VerifySecurityCode...
	User::WaitForRequest(reqStatusVerify);
	ASSERT_EQUALS(KErrNone, reqStatusVerify.Int());
	AssertMockLtsyStatusL();

	// Now send the CompleteL for SetFdnSetting since CTSY sends this ipc down to LTSY after completing VerifySecurityCode.
	iMockLTSY.CompleteL(MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId, KErrNone);

	// Wait for SetFdnSetting...
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	User::WaitForRequest(reqStatusNotif);
	ASSERT_EQUALS(KErrNone, reqStatusNotif.Int());
	ASSERT_EQUALS(fdnStatus, RMobilePhone::EFdnNotActive);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}	

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyLockInfoChange, RMobilePhone::SetLockSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetLockSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySecurityFU::TestUseCase0002L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockPhoneDevice;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfoV1;
	lockInfoV1.iSetting = RMobilePhone::ELockSetEnabled;
	lockInfoV1.iStatus = RMobilePhone::EStatusLocked;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg pckgData(lockInfoV1);
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);

	TRequestStatus status;
	
    RMobilePhone::TMobilePhoneLock lock2 = RMobilePhone::ELockHiddenKey;//ELockUSimApp;
    RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;	
	TDesC* passwordPtr = const_cast<TDesC*>(&KNullDesC);

	TMockLtsyData3<RMobilePhone::TMobilePhoneLock,RMobilePhone::TMobilePhoneLockSetting,TDesC*> phoneLockData(lock2,lockSetting,passwordPtr);
	phoneLockData.SerialiseL(data);	
	
	iMockLTSY.ExpectL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, KErrNone);

	iPhone.SetLockSetting(status, lock2, lockSetting);
	User::WaitForRequest(status);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, status.Int());
		
	TMockLtsyData3<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting, 
	     RMobilePhone::TMobilePhoneLock> lockInfoData(lockInfoV1.iStatus, lockInfoV1.iSetting, lock);
	data.Close();
	lockInfoData.SerialiseL(data);
	
	iMockLTSY.CompleteL(KMockLtsyDispatchSecurityNotifyLockInfoChangeIndId, KErrNone, data);
			
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	//now test case where SetLockSetting() has a password send down, this involves doing a VerifySecurityCode(), thus
	//the password is set to "\0" (this is for dispatcher coverage).
	
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin2;

	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblockPwd");
	RMobilePhone::TMobilePassword pwd(KPwd);
	RMobilePhone::TMobilePassword unblockPwd(KUnblockPwd);


	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode, twoCodes);
	data.Close();
	verifyData.SerialiseL(data);
	
	
	// Send the ExpectL and CompleteL for VerifySecurityCode
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrNone);
	
	// Call ipc for VerifySecurityCode
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	
	// Wait for VerifySecurityCode...
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	

	data.Close();
	 _LIT( KNullCharacter, "\0" );
	passwordPtr = const_cast<TDesC*>(&KNullCharacter);
	phoneLockData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, KErrNone);

	iPhone.SetLockSetting(status, lock2, lockSetting);
	User::WaitForRequest(status);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, status.Int());
	
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UC0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetLockSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetLockSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySecurityFU::TestUseCase0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);	

	TRequestStatus reqStatus, reqStatus2, reqStatusVerify;
	RMobilePhone::TMobilePhoneSecurityEvent secEventRes;
	iMockLTSY.ExpectL(MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId);	
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockICC;
	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;
	iPhone.SetLockSetting(reqStatus2, lock, lockSetting);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())		
		
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin2;

	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblockPwd");
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode, twoCodes);
	verifyData.SerialiseL(data);		
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data, KErrNone);	
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrAccessDenied);
	
	iPhone.VerifySecurityCode(reqStatusVerify, secCode, pwd, unblockPwd);	
	
	User::WaitForRequest(reqStatusVerify);
	ASSERT_EQUALS(KErrAccessDenied, reqStatusVerify.Int());
			
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrAccessDenied, reqStatus2.Int());
	
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	
	iPhone.SetLockSetting(reqStatus2, lock, lockSetting);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())		
	
	data.Close();
	verifyData.SerialiseL(data);		
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data, KErrNone);	
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrNone);
	
	RMobilePhone::TMobilePhoneLock lock2 = RMobilePhone::ELockICC;
	RMobilePhone::TMobilePhoneLockSetting lockSetting2 = RMobilePhone::ELockSetEnabled;
	TDesC* passwordPtr = const_cast<TDesC*>(&KPwd);
	TMockLtsyData3<RMobilePhone::TMobilePhoneLock,RMobilePhone::TMobilePhoneLockSetting,TDesC*> phoneLockData(lock2,lockSetting2,passwordPtr);
	data.Close();
	phoneLockData.SerialiseL(data);		
	iMockLTSY.ExpectL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, data);

	iPhone.VerifySecurityCode(reqStatusVerify, secCode, pwd, unblockPwd);
	
	User::WaitForRequest(reqStatusVerify);
	ASSERT_EQUALS(KErrNone, reqStatusVerify.Int());
	
	iMockLTSY.CompleteL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, KErrNone);
	
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
													
	AssertMockLtsyStatusL();		
	CleanupStack::PopAndDestroy(2, this); // data, this		
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UC0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::AbortSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AbortSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySecurityFU::TestUseCase0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus status;	
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin2;

	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblockPwd");
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);

	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode, twoCodes);
	verifyData.SerialiseL(data);
	
	// Send the ExpectL and CompleteL for VerifySecurityCode
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data, KErrNone);

	// Call ipc for VerifySecurityCode
	iPhone.VerifySecurityCode(status, secCode, pwd, unblockPwd);				
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> secCodeData(secCode);
	data.Close();
	secCodeData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId, KErrNone);

	TInt res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrNone, res)
	AssertMockLtsyStatusL();

	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrAbort);	
	User::WaitForRequest(status);	
	ASSERT_EQUALS(KErrAbort, status.Int());
	AssertMockLtsyStatusL();
	
	// aborting SetLockSetting():	
	_LIT( KNullCharacter, "\0" );	
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockHiddenKey;
	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;
	TDesC* passwordPtr = const_cast<TDesC*>(&KNullCharacter);
	TMockLtsyData3<RMobilePhone::TMobilePhoneLock,RMobilePhone::TMobilePhoneLockSetting,TDesC*> phoneLockData(lock,lockSetting,passwordPtr);
	data.Close();
	phoneLockData.SerialiseL(data);	
	
	iMockLTSY.ExpectL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, data);

	iPhone.SetLockSetting(status, lock, lockSetting);	
	
	res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrNone, res)
	
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrAbort, status.Int());
	AssertMockLtsyStatusL();
	
	// aborting SetFdnSetting():
	
	RMobilePhone::TMobilePhoneFdnSetting  fdnSetting(RMobilePhone::EFdnSetOff);
	iPhone.SetFdnSetting(status, fdnSetting);
	
	res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrNone, res)

	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrAbort, status.Int());
	AssertMockLtsyStatusL();
			
	CleanupStack::PopAndDestroy(2, this); // data, this		
	}

//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for VerifySecurityCode()
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0001L()
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
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data, KErrNotSupported);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrGeneral);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::VerifySecurityCode when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrNone);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::VerifySecurityCode
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(reqStatus);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
				
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for SetPin1DisableNotSupported()
@SYMTestPriority High
@SYMTestActions Invokes SetPin1DisableNotSupported()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
 	TBool supported = EFalse;
	TMockLtsyData1<TBool> ltsyData(supported);
	dataComplete.Close();
	ltsyData.SerialiseL(dataComplete);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	// send completion for calling SetPin1DisableNotSupported
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId, KErrNone, dataComplete);
	User::WaitForRequest(mockLtsyStatus);
	
	// SetLockSetting() should return an error now:
	TRequestStatus status;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockICC;
	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetDisabled;	
	iPhone.SetLockSetting(status, lock, lockSetting);	
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrGsm0707OperationNotAllowed, status.Int());
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());		
	
	CleanupStack::PopAndDestroy(2, this); // dataComplete, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::AbortSecurityCode()
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AbortSecurityCode()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0003L()
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
	iMockLTSY.ExpectL(MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId, data, KErrNotSupported);
	TInt res = iPhone.AbortSecurityCode(secCode);
	
	ASSERT_EQUALS(KErrNotSupported, res)
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId, KErrGeneral);

	res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrGeneral, res)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::AbortSecurityCode when result is not cached.
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId, KErrNone);

	res = iPhone.AbortSecurityCode(secCode);
	ASSERT_EQUALS(KErrNone, res)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::AbortSecurityCode
	// from LTSY.
 	//-------------------------------------------------------------------------	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetLockInfo()
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetLockInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0004L()
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

	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId, dataExpect, KErrNotSupported);

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
	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId, dataExpect);

	RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;
	RMobilePhone::TMobilePhoneLockStatus lockStatus = RMobilePhone::EStatusLocked;

    TMockLtsyData2<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> phoneLockData(lockStatus, lockSetting);
	phoneLockData.SerialiseL(dataComplete);

	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId, KErrGeneral , dataComplete);
	iPhone.GetLockInfo(reqStatus, currentLock, lockInfoPckg);
	User::WaitForRequest(reqStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetLockInfo when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId, dataExpect);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId, KErrNone, dataComplete);

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
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId, KErrNone, dataComplete);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(3); // dataComplete, dataExpect, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSecurityCodeInfo()
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSecurityCodeInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0005L()
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
	TInt remainingEntryAttempts = 4;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> codeData(secCode);
	codeData.SerialiseL(dataExpect);
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, TInt> codeInfoData(secCode, remainingEntryAttempts);
	codeInfoData.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//------------------------------------------------------------------------- 	
 	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId, dataExpect, KErrNotSupported);
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	AssertMockLtsyStatusL();
 	
 	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//------------------------------------------------------------------------- 	
 	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId, dataExpect);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId, KErrGeneral, data);
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetSecurityCodeInfo when result is not cached.
 	//------------------------------------------------------------------------- 	
	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId, dataExpect);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId, KErrNone, data);
	iPhone.GetSecurityCodeInfo(reqStatus, secCode, pckgData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(remainingEntryAttempts, pckgData().iRemainingEntryAttempts)
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetSecurityCodeInfo
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // dataExpect, data, this
	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyLockInfoChange()
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyLockInfoChange()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockPhoneDevice;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfoV1;
	lockInfoV1.iSetting = RMobilePhone::ELockSetEnabled;
	lockInfoV1.iStatus = RMobilePhone::EStatusLocked;
	
	RMobilePhone::TMobilePhoneLockInfoV1Pckg pckgData(lockInfoV1);
	
	TMockLtsyData3<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting, 
	     RMobilePhone::TMobilePhoneLock> lockInfoData(lockInfoV1.iStatus, lockInfoV1.iSetting, lock);
	lockInfoData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyLockInfoChange when result is not cached.
 	//-------------------------------------------------------------------------
	iPhone.NotifyLockInfoChange(reqStatus, lock, pckgData);
	iMockLTSY.CompleteL(KMockLtsyDispatchSecurityNotifyLockInfoChangeIndId, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(lockInfoV1.iSetting,pckgData().iSetting)
	ASSERT_EQUALS(lockInfoV1.iStatus,pckgData().iStatus)
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(2, this); // data, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySecurityEvent()
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySecurityEvent()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
		
	TRequestStatus mockLtsyStatus;

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEventRes;
	RMobilePhone::TMobilePhoneSecurityEvent secEvent = RMobilePhone::EPhonePasswordRequired;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> eventData(secEvent);
	eventData.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId);
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	iMockLTSY.CompleteL(KMockLtsyDispatchSecurityNotifySecurityEventIndId, KErrGeneral, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifySecurityEvent when result is not cached.
 	//-------------------------------------------------------------------------	
	iPhone.NotifySecurityEvent(reqStatus, secEventRes);
	iMockLTSY.CompleteL(KMockLtsyDispatchSecurityNotifySecurityEventIndId, KErrNone, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(secEvent, secEventRes)
	AssertMockLtsyStatusL();


 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifySecurityEvent
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSecurityNotifySecurityEventIndId, KErrNone, data);
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
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetLockSetting()
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetLockSetting()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus status;
	
	RMobilePhone::TMobilePhoneLock lockB = RMobilePhone::ELockPhoneDevice;
	RMobilePhone::TMobilePhoneLockSetting lockSettingB = RMobilePhone::ELockSetEnabled;	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
	iPhone.SetLockSetting(status, lockB, lockSettingB);	
	iMockLTSY.CompleteL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, KErrGeneral);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrGeneral, status.Int());
	AssertMockLtsyStatusL();	
		 	 		
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetLockSetting
	// from LTSY.
 	//------------------------------------------------------------------------- 	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
  	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetLockSetting when result is not cached.
 	//-------------------------------------------------------------------------
	// data for ExpectL
	
	
    RMobilePhone::TMobilePhoneLock lock = RMobilePhone::ELockHiddenKey;//ELockUSimApp;
    RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;	
	TDesC* passwordPtr = const_cast<TDesC*>(&KNullDesC);

	TMockLtsyData3<RMobilePhone::TMobilePhoneLock,RMobilePhone::TMobilePhoneLockSetting,TDesC*> phoneLockData(lock,lockSetting,passwordPtr);
	phoneLockData.SerialiseL(data);	
	
	iMockLTSY.ExpectL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId, KErrNone);

	iPhone.SetLockSetting(status, lock, lockSetting);
	User::WaitForRequest(status);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, status.Int());
	
	CleanupStack::PopAndDestroy(2, this); // data, this		
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for SetActivePin()
@SYMTestPriority High
@SYMTestActions Invokes SetActivePin()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customAPI;
	OpenCustomApiLC(customAPI);	
			
	RBuf8 data;
	CleanupClosePushL(data);	

	TRequestStatus mockLtsyStatus;	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	MLtsyDispatchSimGetActiveIccApplicationType::TIccType type = MLtsyDispatchSimGetActiveIccApplicationType::EIccTypeSim3G;

	TMockLtsyData1<MLtsyDispatchSimGetActiveIccApplicationType::TIccType> iccData(type);
	data.Close();
	iccData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId, KErrNone, data);    

	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId);
	
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin1;
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> secCodeData(secCode);
	data.Close();
	secCodeData.SerialiseL(data);	
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId, KErrNone, data);

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	
	// verify currently active PIN:
	RMobilePhone::TMobilePhoneSecurityCode code;
    customAPI.GetActivePin(code);    
    ASSERT_EQUALS(RMobilePhone::ESecurityCodePin1, code);
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); // data, customAPI, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CheckSecurityCode()
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckSecurityCode()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomApiLC(customAPI);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

    TRequestStatus requestStatus;
    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
    DispatcherSecurity::TSecurityCodeId secCodeID = DispatcherSecurity::ESecCodePin;
    RMobilePhone::TMobilePassword secCode;    
   
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TMockLtsyData2 <DispatcherSecurity::TSecurityCodeId, RMobilePhone::TMobilePassword> ltsyData(secCodeID, secCode);
	ltsyData.SerialiseL(data);	

    iMockLTSY.ExpectL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, data, KErrNotSupported);
    
    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, data);    
    iMockLTSY.CompleteL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, KErrGeneral);

	TMockLtsyData1 <DispatcherSecurity::TSecurityCodeId > ltsyData2(secCodeID);
	ltsyData2.SerialiseL(data2);	

	// this is because HandleRequest(ECustomCheckSecurityCodeCancelIPC) is called when completing by error
    iMockLTSY.ExpectL( MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId, data2);
 
    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();
		
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::CheckSecurityCode when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, data);    
    iMockLTSY.CompleteL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, KErrNone);

    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::CheckSecurityCode
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // data2, data, customAPI, this		
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::DisablePhoneLock()
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DisablePhoneLock()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomApiLC(customAPI);

	_LIT(KPassword, "password");

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePassword pswd(KPassword);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMobilePhone::TMobilePassword > ltsyData(pswd);	
	ltsyData.SerialiseL(expectData);
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId, expectData, KErrNotSupported);
    
    customAPI.DisablePhoneLock(requestStatus, pswd);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId, expectData);    
    iMockLTSY.CompleteL( MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId, KErrGeneral);

    customAPI.DisablePhoneLock(requestStatus, pswd);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::DisablePhoneLock when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId, expectData);
    iMockLTSY.CompleteL( MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId, KErrNone);

    customAPI.DisablePhoneLock(requestStatus, pswd);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::DisablePhoneLock
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // expectData, customAPI, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyCipheringInfoChange()
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyCipheringInfoChange()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomApiLC(customAPI);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);	

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RMmCustomAPI::TCipheringInfo info;
	RMmCustomAPI::TCipheringInfo completeInfo;
	completeInfo.iIndStatus = ETrue;
	completeInfo.iCiphStatus = ETrue;

	TMockLtsyData1<RMmCustomAPI::TCipheringInfo > ltsyData(completeInfo);	
	ltsyData.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyCipheringInfoChange when result is not cached.
 	//-------------------------------------------------------------------------
	customAPI.NotifyCipheringInfoChange(requestStatus, info);
	iMockLTSY.CompleteL(KLtsyDispatchSecurityNotifyCipheringInfoChangeApiId, KErrNone, completeData);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(completeInfo.iIndStatus, info.iIndStatus);
	ASSERT_EQUALS(completeInfo.iCiphStatus, info.iCiphStatus);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyCipheringInfoChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(KLtsyDispatchSecurityNotifyCipheringInfoChangeApiId, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	CleanupStack::PopAndDestroy(3, this); // completeData, customAPI, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetCipheringInfo()
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCipheringInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0013L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomApiLC(customAPI);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RBuf8 completeData2;
	CleanupClosePushL(completeData2);			

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	RMmCustomAPI::TCipheringInfo info;

	TBool indReq(ETrue);
	TMockLtsyData1<TBool > ltsyData(indReq);	
	ltsyData.SerialiseL(completeData);	

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId, KErrNotSupported);
	customAPI.GetCipheringInfo(requestStatus, info);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId, KErrGeneral, completeData);
	customAPI.GetCipheringInfo(requestStatus, info);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetCipheringInfo when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	RMmCustomAPI::TCipheringInfo ciphInfo;
	ciphInfo.iCiphStatus = ETrue;
	
	TMockLtsyData1<RMmCustomAPI::TCipheringInfo > ltsyData2(ciphInfo);	
	ltsyData2.SerialiseL(completeData2);
	
	iMockLTSY.CompleteL(KLtsyDispatchSecurityNotifyCipheringInfoChangeApiId, KErrNone, completeData2);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId, KErrNone, completeData);
	customAPI.GetCipheringInfo(requestStatus, info);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(indReq, info.iIndStatus);
	ASSERT_EQUALS(ciphInfo.iCiphStatus, info.iCiphStatus);
		
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetCipheringInfo
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	CleanupStack::PopAndDestroy(4, this); // completeData2, completeData, customAPI, this 	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::IsBlocked()
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::IsBlocked()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySecurityFU::TestUnit0014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomApiLC(customAPI);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
	TBool result;

	DispatcherSecurity::TSecurityCodeId secCodeID = DispatcherSecurity::ESecCodePin;

	TMockLtsyData1 <DispatcherSecurity::TSecurityCodeId > ltsyData(secCodeID);
	ltsyData.SerialiseL(data);	
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId, data, KErrNotSupported);

	TInt ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	TBool compResult(ETrue);
	TMockLtsyData1 <TBool> ltsyData2(compResult);
	ltsyData2.SerialiseL(completeData);

    iMockLTSY.ExpectL( MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId, data);
    iMockLTSY.CompleteL( MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId, KErrGeneral, completeData);

	ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrGeneral, ret);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::IsBlocked when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId, data);
    iMockLTSY.CompleteL( MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId, KErrNone, completeData);

	ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(compResult, result);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::IsBlocked
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId, KErrNone, completeData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // completeData, data, customAPI, this
	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0015
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CompleteNotifySecurityReady()
@SYMTestPriority High
@SYMTestActions Invokes CompleteNotifySecurityReady()
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySecurityFU::TestUnit0015L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	_LIT(KPwd,"12345");
	_LIT(KUnblockPwd, "54321");

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
	
 	//-------------------------------------------------------------------------
	//
	// RMobilePhone::VerifySecurityCode 
 	//-------------------------------------------------------------------------
 	TRequestStatus mockLtsyStatus;
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
 	iMockLTSY.CompleteL(KMockLtsyDispatchSecurityBootNotifySecurityReadyIndId, KErrNone);
 	User::WaitForRequest(mockLtsyStatus);
 	
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrNone);
	iPhone.VerifySecurityCode(reqStatus, secCode, pwd, unblockPwd);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-UN0016
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::CheckSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::CheckSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySecurityFU::TestUnit0016L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMmCustomAPI customAPI;
    OpenCustomAPILC(customAPI, iPhone);

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 data2;
    CleanupClosePushL(data2);

    TRequestStatus requestStatus;    
    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);       
    DispatcherSecurity::TSecurityCodeId secCodeID = DispatcherSecurity::ESecCodePin;
    RMobilePhone::TMobilePassword secCode;    
     
    TMockLtsyData2 <DispatcherSecurity::TSecurityCodeId, RMobilePhone::TMobilePassword> ltsyData(secCodeID, secCode);
    ltsyData.SerialiseL(data);  
    
    //-------------------------------------------------------------------------
    // Test A: Cancelling of RMmCustomAPI::CheckSecurityCode
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, data);
    customAPI.CheckSecurityCode(requestStatus, type);

    TMockLtsyData1 <DispatcherSecurity::TSecurityCodeId > ltsyData2(secCodeID);
    ltsyData2.SerialiseL(data2);        

    iMockLTSY.ExpectL( MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId, data2);
    iMockLTSY.CompleteL( MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId, KErrNone);
    iMockLTSY.CompleteL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, KErrCancel);
    
    customAPI.CancelAsyncRequest(ECustomCheckSecurityCodeIPC);

    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(4, this);               
    
    }
    
// Helper methods
void CCTsySecurityFU::OpenCustomAPILC(RMmCustomAPI& aCustomAPI, RMobilePhone& aPhone, TBool aExpectNeeded)
    {
    RBuf8 data;
    CleanupClosePushL(data);
    
    if(aExpectNeeded)
        {           
        iMockLTSY.ExpectL(MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId);                   
        }

    TInt errorCode = aCustomAPI.Open(aPhone);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupStack::PopAndDestroy (&data);
    CleanupClosePushL(aCustomAPI);

    }
