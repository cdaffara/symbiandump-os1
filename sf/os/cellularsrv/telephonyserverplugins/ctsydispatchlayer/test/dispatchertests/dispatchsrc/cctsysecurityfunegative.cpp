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
 @file The TEFUnit test suite for CallControl in the Common TSY.
*/

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>

#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include "mockltsyindicatorids.h"
#include "config.h"

#include "cctsysecurityfunegative.h"

CTestSuite* CCTsySecurityFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
  	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0004L);	

	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0006L);

	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsySecurityFUNegative, TestUnit0012L);
	END_SUITE;
	}

//
// 'Negative' unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetSecurityCodeInfo is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSecurityCodeInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0001L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
    TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;
	TPckg<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> pckgData(codeV5);
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	
	iPhone.GetSecurityCodeInfo(requestStatus, secCode, pckgData);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());   
	
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetLockInfo is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetLockInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0002L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
    TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneLock currentLock = RMobilePhone::ELockICC;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoPckg(lockInfo);
	
	iPhone.GetLockInfo(requestStatus, currentLock, lockInfoPckg);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());   
	
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneAbortSecurityCode is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AbortSecurityCode()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0003L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin2;
	
	TInt err = iPhone.AbortSecurityCode(secCode);
    ASSERT_EQUALS(KErrNotSupported, err);   
	
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMmTsySecurityGetSimActivePinStateIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetActivePin()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0004L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	// investigate further. KErrNotSupported returned somewhere?
	RMobilePhone::TMobilePhoneSecurityCode code;
	customApi.GetActivePin(code);
	
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customAPI, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneVerifySecurityCode is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::VerifySecurityCode()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0006L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
    TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin2;
	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblockPwd");
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);
	
	iPhone.VerifySecurityCode(requestStatus, secCode, pwd, unblockPwd);
	User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());   
	
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomCheckSecurityCodeIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckSecurityCode()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0008L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
    
    customApi.CheckSecurityCode(requestStatus, type);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomDisablePhoneLockIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DisablePhoneLock()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0009L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	_LIT(KPassword, "password");
    RMobilePhone::TMobilePassword pswd(KPassword);
    
    customApi.DisablePhoneLock(requestStatus, pswd);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomGetCipheringInfoIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCipheringInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0010L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	TRequestStatus requestStatus;	
	RMmCustomAPI::TCipheringInfo info;
    
	customApi.GetCipheringInfo(requestStatus, info);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomIsBlockedIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::IsBlocked()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0011L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	TRequestStatus requestStatus;	
    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
	TBool result;
  
	TInt err = customApi.IsBlocked(type, result);
	ASSERT_EQUALS(KErrNotSupported, err);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SECURITY-NEGATIVE-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomCheckSecurityCodeCancelIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckSecurityCode()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySecurityFUNegative::TestUnit0012L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

    DispatcherSecurity::TSecurityCodeId secCodeID = DispatcherSecurity::ESecCodePin;
    RMobilePhone::TMobilePassword secCode;    
    TMockLtsyData2 <DispatcherSecurity::TSecurityCodeId, RMobilePhone::TMobilePassword> ltsyData(secCodeID, secCode);
	ltsyData.SerialiseL(data);	

    iMockLTSY.ExpectL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, data);    
    iMockLTSY.CompleteL( MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId, KErrGeneral);
    
//	RBuf8 data2;
//	CleanupClosePushL(data2);
//    
//	TMockLtsyData1 <DispatcherSecurity::TSecurityCodeId > ltsyData2(secCodeID);
//	ltsyData2.SerialiseL(data2);	
//
//	// this is because HandleRequest(ECustomCheckSecurityCodeCancelIPC) is called when completing by error
//    iMockLTSY.ExpectL( MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId, data2);  
 
    TRequestStatus requestStatus;
    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
    
    customApi.CheckSecurityCode(requestStatus, type);
//	customApi.CancelAsyncRequest(ECustomCheckSecurityCodeIPC);
	User::WaitForRequest(requestStatus);
	// as stated in the CTSY test:
	// Request ECustomCheckSecurityCodeCancelIPC is not cancelled properly - CMmCustomSecurityTsy::CheckSecurityCodeCancelL() is never called.
	// Return value id KErrGeneral instead od KErrCancel.
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}
