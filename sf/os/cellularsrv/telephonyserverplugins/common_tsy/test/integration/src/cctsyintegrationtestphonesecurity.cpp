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
// Test step definitions for the PhoneSecurity functional unit.
// 
//

/**
 @internalTechnology
*/
#include "cctsyintegrationtestphonesecurity.h"
#include "cctsyinidata.h"
#include <mpbutil.h>
	
CCTSYIntegrationTestPhoneSecurityBase::CCTSYIntegrationTestPhoneSecurityBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iSimTsyTestHelper(*this), iNetworkTsyTestHelper(*this), iPhoneTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPhoneSecurityBase::~CCTSYIntegrationTestPhoneSecurityBase()
/*
 * Destructor
 */
	{
	}

void CCTSYIntegrationTestPhoneSecurityBase::ManglePassword(const TDesC& aOldPassword, TDes& aNewPassword)
/**
 * member function for mangling the password returning void
 * 
 * @param aOldPassword a reference of type TDesC, holding password to be mangled
 * @param aNewPassword a reference to type TDes, holds the mangled password

 * @return void
 */
	{
	aNewPassword.Zero();
	char ch;
    for(TInt i=aOldPassword.Length()-1; i>=0; i--)
	    {
	    ch = aOldPassword[i] + 1;
	    aNewPassword.Append(ch);
	    }
	}


void CCTSYIntegrationTestPhoneSecurityBase::UnblockPinL(RMobilePhone& aMobilePhone,
		RMobilePhone::TMobilePhoneSecurityCode aPhoneSecurityCode, TDesC& aPin, TDesC& aPuk)
/**
 * member function for unblocking pin1/pin2 returing void
 * 
 * @param aMobilePhone a reference to RMobilePhone object
 * @param aPhoneSecurityCode holds the pin type to be unbolcked
 * @param aPin holds the pin password
 * @param aPuk holds the puk password
 *
 * @return void
 */

	{
	
	// post notification
	TExtEtelRequestStatus notifySecurityEventStatus(aMobilePhone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL( notifySecurityEventStatus );
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	aMobilePhone.NotifySecurityEvent(notifySecurityEventStatus, securityEvent);
	
	RMobilePhone::TMobilePhoneSecurityCode  secCode;
	RMobilePhone::TMobilePhoneSecurityEvent pukRequired;
	RMobilePhone::TMobilePhoneSecurityEvent pukVerified;
	if(aPhoneSecurityCode == RMobilePhone::ESecurityCodePuk1)
		{
		secCode = RMobilePhone::ESecurityCodePin1;
		pukRequired = RMobilePhone::EPuk1Required;
		pukVerified = RMobilePhone::EPuk1Verified;
		
		TExtEtelRequestStatus getSecurityCodeInfoStatus(aMobilePhone, EMobilePhoneGetSecurityCodeInfo);
		CleanupStack::PushL( getSecurityCodeInfoStatus );	
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5 getSecurityCodeInfo;
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg getSecurityCodeInfoPckg( getSecurityCodeInfo );
		aMobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,secCode, getSecurityCodeInfoPckg);

		ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::NotifySecurityEvent returns KErrOverflow from LTSY.<font color=Black>"));
		iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityEvent(aMobilePhone,notifySecurityEventStatus,
				securityEvent,
				pukRequired,
				KErrOverflow);

		if( pukRequired == securityEvent )
			{
		aMobilePhone.NotifySecurityEvent(notifySecurityEventStatus, securityEvent);
		TExtEtelRequestStatus verifySecurityCodeStatus(aMobilePhone, EMobilePhoneVerifySecurityCode);
		CleanupStack::PushL( verifySecurityCodeStatus );
		aMobilePhone.VerifySecurityCode( verifySecurityCodeStatus, aPhoneSecurityCode, 
										aPin, aPuk );
		ASSERT_EQUALS(WaitForRequestWithTimeOut(verifySecurityCodeStatus, ETimeLong), 
				KErrNone, _L("RMobilePhone::VerifySecurityCode timed-out"));
		ASSERT_EQUALS( verifySecurityCodeStatus.Int(), KErrNone, _L("RMobilePhone::VerifySecurityCode returned with an error"));
			
		iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityEvent(aMobilePhone,notifySecurityEventStatus,
				securityEvent,
				pukVerified,
				KErrNone);
		
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
		ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
		CleanupStack::PopAndDestroy(&verifySecurityCodeStatus);
			}
		else
			{
			ASSERT_TRUE(EFalse, _L("RMobilePhone::EPuk1Required state not reached, unblock pin failed"));
	
			}
		CleanupStack::PopAndDestroy(&getSecurityCodeInfoStatus);

		}
	else if(aPhoneSecurityCode == RMobilePhone::ESecurityCodePuk2)
		{
		secCode = RMobilePhone::ESecurityCodePin2;
		pukRequired = RMobilePhone::EPuk2Required;
		pukVerified = RMobilePhone::EPuk2Verified;
		
		TExtEtelRequestStatus verifySecurityCodeStatus(aMobilePhone, EMobilePhoneVerifySecurityCode);
		CleanupStack::PushL( verifySecurityCodeStatus );
		aMobilePhone.VerifySecurityCode( verifySecurityCodeStatus, aPhoneSecurityCode, 
										aPin, aPuk );
		ASSERT_EQUALS(WaitForRequestWithTimeOut(verifySecurityCodeStatus, ETimeLong), 
				KErrNone, _L("RMobilePhone::VerifySecurityCode timed-out"));
		ASSERT_EQUALS( verifySecurityCodeStatus.Int(), KErrNone, _L("RMobilePhone::VerifySecurityCode returned with an error"));
			
		iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityEvent(aMobilePhone,notifySecurityEventStatus,
				securityEvent,
				pukVerified,
				KErrNone);		
		CleanupStack::PopAndDestroy(&verifySecurityCodeStatus);
		}
	
	CleanupStack::PopAndDestroy(&notifySecurityEventStatus);
	
	}

CCTSYIntegrationTestPhoneSecurity0001::CCTSYIntegrationTestPhoneSecurity0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0001::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0001::~CCTSYIntegrationTestPhoneSecurity0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0001
 * @SYMFssID BA/CTSY/PSEC-0001
 * @SYMTestCaseDesc Change PIN1.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySecurityCodeInfoChange, RMobilePhone::GetSecurityCodeInfo
 * @SYMTestExpectedResults Pass - PIN1 is changed
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify PIN1 can be changed and security notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	//register with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 
    TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin1; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin1, pin1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	
	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC,RMobilePhone::ELockSetEnabled, pin1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// post notification for NotifySecurityCodeInfoChange
	TExtEtelRequestStatus notifySecurityCodeInfoChangeStatus(mobilePhone, EMobilePhoneNotifySecurityCodeInfoChange);
	CleanupStack::PushL( notifySecurityCodeInfoChangeStatus );
	RMobilePhone::TMobilePhoneSecurityCode securityCode;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg securityCodeInfoPckg( securityCodeInfo );
	mobilePhone.NotifySecurityCodeInfoChange(notifySecurityCodeInfoChangeStatus, securityCode,securityCodeInfoPckg);
	
	// Change the security code with RMobilePhone::ChangeSecurityCode with aType=ESecurityCodePin1 , aChange.iOldPassword=old PIN1 and aChange.iNewPassword=new valid PIN1 
	TExtEtelRequestStatus changeSecurityCodeStatus(mobilePhone, EMobilePhoneChangeSecurityCode);
	CleanupStack::PushL( changeSecurityCodeStatus );
	RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;
	passwordChange.iOldPassword = pin1;
	ManglePassword(pin1,passwordChange.iNewPassword);
	mobilePhone.ChangeSecurityCode(changeSecurityCodeStatus,RMobilePhone::ESecurityCodePin1,passwordChange);
	
	// Check RMobilePhone::NotifySecurityCodeInfoChange with aSecurityCode=ESecurityCodePin1 completes with aSecurityCodeInfo.iRemainingEntryAttempts=3
	RMobilePhone::TMobilePhoneSecurityCode expectedSecurityCode = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 expectedSecurityCodeInfo;
	expectedSecurityCodeInfo.iRemainingEntryAttempts = 3;
	TCmpBase<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> mobilePhoneSecurityCodeInfoCmpBase(expectedSecurityCodeInfo,securityCodeInfo,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityCodeInfoChange(mobilePhone,notifySecurityCodeInfoChangeStatus,
			expectedSecurityCode,mobilePhoneSecurityCodeInfoCmpBase, KErrNone);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(changeSecurityCodeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ChangeSecurityCode timed out"));
	ASSERT_EQUALS(changeSecurityCodeStatus.Int(), KErrNone, _L("RMobilePhone::ChangeSecurityCode returned an error"));

	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin1 returns aSecurityCodeInfo.iRemainingEntryAttempts=3
	TExtEtelRequestStatus getSecurityCodeInfoStatus(mobilePhone, EMobilePhoneGetSecurityCodeInfo);
	CleanupStack::PushL( getSecurityCodeInfoStatus );
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 getSecurityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg getSecurityCodeInfoPckg( getSecurityCodeInfo );
	mobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,RMobilePhone::ESecurityCodePin1, getSecurityCodeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
	ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	ASSERT_EQUALS(getSecurityCodeInfo.iRemainingEntryAttempts, 3, _L("RMobilePhone::GetSecurityCodeInfo returned incorrect remaining entry attempts"));
	
	//
	// TEST END
	//

    StartCleanup();
    
	passwordChange.iOldPassword = passwordChange.iNewPassword;
	passwordChange.iNewPassword = pin1;
    mobilePhone.ChangeSecurityCode(changeSecurityCodeStatus,RMobilePhone::ESecurityCodePin1,passwordChange);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(changeSecurityCodeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ChangeSecurityCode timed out"));
	ASSERT_EQUALS(changeSecurityCodeStatus.Int(), KErrNone, _L("RMobilePhone::ChangeSecurityCode returned an error"));

    iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled, pin1);

	// 1 notifySecurityCodeInfoChangeStatus
    // 2 changeSecurityCodeStatus
    // 3 getSecurityCodeInfoStatus
    
	CleanupStack::PopAndDestroy(3, &notifySecurityCodeInfoChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0001");
	}



CCTSYIntegrationTestPhoneSecurity0002::CCTSYIntegrationTestPhoneSecurity0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0002::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0002::~CCTSYIntegrationTestPhoneSecurity0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0002
 * @SYMFssID BA/CTSY/PSEC-0002
 * @SYMTestCaseDesc Change PIN1 with incorrect old PIN1.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySecurityCodeInfoChange, RMobilePhone::GetSecurityCodeInfo
 * @SYMTestExpectedResults Pass - KErrAccessDenied returned on attempt to change PIN with incorrect old PIN.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrAccessDenied returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//register with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 
    TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin1; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin1, pin1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC,RMobilePhone::ELockSetEnabled, pin1);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// post notification for NotifySecurityCodeInfoChange
	TExtEtelRequestStatus notifySecurityCodeInfoChangeStatus(mobilePhone, EMobilePhoneNotifySecurityCodeInfoChange);
	CleanupStack::PushL( notifySecurityCodeInfoChangeStatus );
	RMobilePhone::TMobilePhoneSecurityCode securityCode;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg securityCodeInfoPckg( securityCodeInfo );
	mobilePhone.NotifySecurityCodeInfoChange(notifySecurityCodeInfoChangeStatus, securityCode,securityCodeInfoPckg);
	
	// Attempt to change PIN1 with incorrect old PIN1 with RMobilePhone::ChangeSecurityCode with aType=ESecurityCodePin1 , aChange.iOldPassword=incorrect old PIN1 and aChange.iNewPassword=new valid PIN1 	
	TExtEtelRequestStatus changeSecurityCodeStatus(mobilePhone, EMobilePhoneChangeSecurityCode);
	CleanupStack::PushL( changeSecurityCodeStatus );
	RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;
	passwordChange.iOldPassword = _L("abcd"); // incorrect pin
	passwordChange.iNewPassword = pin1;
	mobilePhone.ChangeSecurityCode(changeSecurityCodeStatus,RMobilePhone::ESecurityCodePin1,passwordChange);

	// Check KErrGsm0707IncorrectPassword / KErrAccessDenied  is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(changeSecurityCodeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ChangeSecurityCode timed out"));
	ASSERT_TRUE( ( changeSecurityCodeStatus.Int() == KErrAccessDenied
					|| changeSecurityCodeStatus.Int() == KErrGsm0707IncorrectPassword ), _L("RMobilePhone::ChangeSecurityCode did not return an error"));

	// Check RMobilePhone::NotifySecurityCodeInfoChange with aSecurityCode=ESecurityCodePin1 completes with aSecurityCodeInfo.iRemainingEntryAttempts=2
	RMobilePhone::TMobilePhoneSecurityCode expectedSecurityCode = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 expectedSecurityCodeInfo;
	expectedSecurityCodeInfo.iRemainingEntryAttempts = 2;
	TCmpBase<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> mobilePhoneSecurityCodeInfoCmpBase(expectedSecurityCodeInfo,securityCodeInfo,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityCodeInfoChange(mobilePhone,notifySecurityCodeInfoChangeStatus,
			expectedSecurityCode,mobilePhoneSecurityCodeInfoCmpBase, KErrNone);


	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin1 returns aSecurityCodeInfo.iRemainingEntryAttempts=2
	TExtEtelRequestStatus getSecurityCodeInfoStatus(mobilePhone, EMobilePhoneGetSecurityCodeInfo);
	CleanupStack::PushL( getSecurityCodeInfoStatus );
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 getSecurityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg getSecurityCodeInfoPckg( getSecurityCodeInfo );
	mobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,RMobilePhone::ESecurityCodePin1, getSecurityCodeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
	ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	ASSERT_EQUALS(getSecurityCodeInfo.iRemainingEntryAttempts, 2, _L("RMobilePhone::GetSecurityCodeInfo returned incorrect remaining entry attempts"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled, pin1);

	// 1 notifySecurityCodeInfoChangeStatus
    // 2 changeSecurityCodeStatus
    // 3 getSecurityCodeInfoStatus
    
	CleanupStack::PopAndDestroy(3, &notifySecurityCodeInfoChangeStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0002");
	}



CCTSYIntegrationTestPhoneSecurity0003::CCTSYIntegrationTestPhoneSecurity0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0003::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0003::~CCTSYIntegrationTestPhoneSecurity0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0003
 * @SYMFssID BA/CTSY/PSEC-0003
 * @SYMTestCaseDesc Change PIN1 when new PIN1 is only 3 digits long.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySecurityCodeInfoChange, RMobilePhone::GetSecurityCodeInfo
 * @SYMTestExpectedResults Pass - PIN1 is not changed
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 

	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Attempt to change PIN1 to a new 3 digit PIN1 with RMobilePhone::ChangeSecurityCode with aType=ESecurityCodePin1, aChange.iOldPassword=old PIN1 and aChange.iNewPassword=3 digit number 

	// Check error is returned.

	// Check RMobilePhone::NotifySecurityCodeInfoChange with aSecurityCode=ESecurityCodePin1 completes with aSecurityCodeInfo.iRemainingEntryAttempts=2

	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin1 returns aSecurityCodeInfo.iRemainingEntryAttempts=2

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0003");
	}



CCTSYIntegrationTestPhoneSecurity0004::CCTSYIntegrationTestPhoneSecurity0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0004::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0004::~CCTSYIntegrationTestPhoneSecurity0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0004
 * @SYMFssID BA/CTSY/PSEC-0004
 * @SYMTestCaseDesc Disable PIN1.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetLockInfo, RMobilePhone::NotifyLockInfoChange, RMobilePhone::GetSubscriberId
 * @SYMTestExpectedResults Pass - PIN1 is disabled
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify lock info change notification completes and lock info correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//register with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 
    TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin1; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin1, pin1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	
	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC,RMobilePhone::ELockSetEnabled, pin1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// post notification
	RMobilePhone::TMobilePhoneLock mobilePhoneLock;
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
    RMobilePhone::TMobilePhoneLockInfoV1 expectedLockInfo;
    expectedLockInfo.iSetting = RMobilePhone::ELockSetDisabled;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);
	TExtEtelRequestStatus notifyLockInfoChangeStatus(mobilePhone, EMobilePhoneNotifyLockInfoChange);
	CleanupStack::PushL(notifyLockInfoChangeStatus);
	mobilePhone.NotifyLockInfoChange(notifyLockInfoChangeStatus, mobilePhoneLock, infoPckg);
	
	// Disable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC and aSetting=ELockSetDisabled 
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled, pin1);
	  
	// Check RMobilePhone::NotifyLockInfoChange with aLock=ELockICC completes with aLockInfo.iStatus=EStatusUnlocked and aLockInfo.iSetting=ELockSetDisabled
	TCmpTMobilePhoneLockInfoV1NoStatus lockInfoCmp(expectedLockInfo,lockInfo,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifyLockInfoChange(mobilePhone,notifyLockInfoChangeStatus,
			mobilePhoneLock,RMobilePhone::ELockICC,lockInfoCmp,KErrNone);
	ASSERT_EQUALS(lockInfo.iSetting, RMobilePhone::ELockSetDisabled, _L("RMobilePhone::NotifyLockInfoChange TMobilePhoneLockInfoV1.iSetting is not ELockSetDisabled as expected"));

	// Check RMobilePhone::GetLockInfo with aLock=ELockICC  returs aLockInfo.iStatus=EStatusUnlocked and aLockInfo.iSetting=ELockSetDisabled
	TExtEtelRequestStatus getLockInfoStatus(mobilePhone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockICC, infoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	ASSERT_EQUALS(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	ASSERT_EQUALS(lockInfo.iStatus, RMobilePhone::EStatusUnlocked, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iStatus is not EStatusUnlocked as expected"));
	ASSERT_EQUALS(lockInfo.iSetting, RMobilePhone::ELockSetDisabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetDisabled as expected"));

	// ===  Ensure success of disabling PIN1 by carrying out an operation which would normally require PIN1 ===

	// Check RMobilePhone::GetSubscriberId returns an IMSI with length > 0
	TExtEtelRequestStatus getSubscriberIdStatus(mobilePhone, EMobilePhoneGetSubscriberId);
	CleanupStack::PushL(getSubscriberIdStatus);
	RMobilePhone::TMobilePhoneSubscriberId mobilePhoneSubscriberId;
	mobilePhone.GetSubscriberId(getSubscriberIdStatus, mobilePhoneSubscriberId);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSubscriberIdStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetSubscriberId timed out"));
	ASSERT_EQUALS(getSubscriberIdStatus.Int(), KErrNone, _L("RMobilePhone::GetSubscriberId returned with an error"));
	ASSERT_TRUE(mobilePhoneSubscriberId.Length() > 0 , _L("RMobilePhone::GetSubscriberId returns an IMSI with length not greater than 0"));

	//
	// TEST END
	//

    StartCleanup();
	
	// 1 notifyLockInfoChangeStatus
    // 2 getLockInfoStatus
    // 3 getSubscriberIdStatus
    
	CleanupStack::PopAndDestroy(3, &notifyLockInfoChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0004");
	}



CCTSYIntegrationTestPhoneSecurity0005::CCTSYIntegrationTestPhoneSecurity0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0005::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0005::~CCTSYIntegrationTestPhoneSecurity0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0005
 * @SYMFssID BA/CTSY/PSEC-0005
 * @SYMTestCaseDesc Change PIN1 when it is deactivated.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySecurityCodeInfoChange, RMobilePhone::GetSecurityCodeInfo
 * @SYMTestExpectedResults Pass - PIN 1 changed successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify PIN1 can be changed and security notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//register with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 
    TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin1; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin1, pin1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	
	// Disable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetDisabled 
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC,RMobilePhone::ELockSetDisabled, pin1);

	// Ensure RMobilePhone::GetLockInfo with aLock=ELockICC returns iSetting=ELockSetDisabled 
	TExtEtelRequestStatus getLockInfoStatus(mobilePhone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockICC, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	CHECK_EQUALS_L(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	CHECK_EQUALS_L(lockInfo.iSetting, RMobilePhone::ELockSetDisabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetDisabled as expected"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// post notification for NotifySecurityCodeInfoChange
	TExtEtelRequestStatus notifySecurityCodeInfoChangeStatus(mobilePhone, EMobilePhoneNotifySecurityCodeInfoChange);
	CleanupStack::PushL( notifySecurityCodeInfoChangeStatus );
	RMobilePhone::TMobilePhoneSecurityCode securityCode;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg securityCodeInfoPckg( securityCodeInfo );
	mobilePhone.NotifySecurityCodeInfoChange(notifySecurityCodeInfoChangeStatus, securityCode,securityCodeInfoPckg);
	
	// Change the security code with RMobilePhone::ChangeSecurityCode with aType=ESecurityCodePin1 , aChange.iOldPassword=old PIN1 and aChange.iNewPassword=new valid PIN1 
	TExtEtelRequestStatus changeSecurityCodeStatus(mobilePhone, EMobilePhoneChangeSecurityCode);
	CleanupStack::PushL( changeSecurityCodeStatus );
	RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;
	passwordChange.iOldPassword = pin1;
	ManglePassword(pin1,passwordChange.iNewPassword);
	mobilePhone.ChangeSecurityCode(changeSecurityCodeStatus,RMobilePhone::ESecurityCodePin1,passwordChange);
	
	// Check RMobilePhone::NotifySecurityCodeInfoChange with aSecurityCode=ESecurityCodePin1 completes with aSecurityCodeInfo.iRemainingEntryAttempts=3
	RMobilePhone::TMobilePhoneSecurityCode expectedSecurityCode = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 expectedSecurityCodeInfo;
	expectedSecurityCodeInfo.iRemainingEntryAttempts = 3;
	TCmpBase<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> mobilePhoneSecurityCodeInfoCmpBase(expectedSecurityCodeInfo,securityCodeInfo,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityCodeInfoChange(mobilePhone,notifySecurityCodeInfoChangeStatus,
			expectedSecurityCode,mobilePhoneSecurityCodeInfoCmpBase, KErrNone);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(changeSecurityCodeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ChangeSecurityCode timed out"));
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::ChangeSecurityCode returns KErrGeneral from LTSY.<font color=Black>"));
	ASSERT_EQUALS(changeSecurityCodeStatus.Int(), KErrGeneral, _L("RMobilePhone::ChangeSecurityCode returned an error"));

	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin1 returns aSecurityCodeInfo.iRemainingEntryAttempts=3
	TExtEtelRequestStatus getSecurityCodeInfoStatus(mobilePhone, EMobilePhoneGetSecurityCodeInfo);
	CleanupStack::PushL( getSecurityCodeInfoStatus );
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 getSecurityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg getSecurityCodeInfoPckg( getSecurityCodeInfo );
	mobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,RMobilePhone::ESecurityCodePin1, getSecurityCodeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
	ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	ASSERT_EQUALS(getSecurityCodeInfo.iRemainingEntryAttempts, 3, _L("RMobilePhone::GetSecurityCodeInfo returned incorrect remaining entry attempts"));

	//
	// TEST END
	//

    StartCleanup();
	
	passwordChange.iOldPassword = passwordChange.iNewPassword;
	passwordChange.iNewPassword = pin1;
    mobilePhone.ChangeSecurityCode(changeSecurityCodeStatus,RMobilePhone::ESecurityCodePin1,passwordChange);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(changeSecurityCodeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ChangeSecurityCode timed out"));
	ASSERT_EQUALS(changeSecurityCodeStatus.Int(), KErrNone, _L("RMobilePhone::ChangeSecurityCode returned an error"));

    // 1 getLockInfoStatus
    // 2 notifySecurityCodeInfoChangeStatus
    // 3 changeSecurityCodeStatus
    // 4 getSecurityCodeInfoStatus
    
	CleanupStack::PopAndDestroy(4, &getLockInfoStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0005");
	}



CCTSYIntegrationTestPhoneSecurity0006::CCTSYIntegrationTestPhoneSecurity0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0006::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0006::~CCTSYIntegrationTestPhoneSecurity0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0006
 * @SYMFssID BA/CTSY/PSEC-0006
 * @SYMTestCaseDesc Enable PIN1.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetLockInfo, RMobilePhone::NotifyLockInfoChange, RMobilePhone::VerifySecurityCode, RMobilePhone::NotifySecurityEvent
 * @SYMTestExpectedResults Pass - PIN1 is activated
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify lock info change notification completes and lock info correct.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10001</font>"));

	//
	// SET UP
	//
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 
    RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    TUint32 secCaps;
    TUint32 expectedCaps = (RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC);
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));
    
	//
	// SET UP END
	//
		
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Enable PIN1 ===
	
	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin1; 
	ASSERT_TRUE(GetStringFromConfig(*networkSection, KIniPin1, pin1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));

	// post notification
	RMobilePhone::TMobilePhoneLock mobilePhoneLock1;
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo1;
    RMobilePhone::TMobilePhoneLockInfoV1 expectedLockInfo;
    expectedLockInfo.iSetting = RMobilePhone::ELockSetEnabled;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg1(lockInfo1);
	TExtEtelRequestStatus notifyLockInfoChangeStatus1(mobilePhone, EMobilePhoneNotifyLockInfoChange);
	CleanupStack::PushL(notifyLockInfoChangeStatus1);
	mobilePhone.NotifyLockInfoChange(notifyLockInfoChangeStatus1, mobilePhoneLock1, infoPckg1);

	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC,RMobilePhone::ELockSetEnabled, pin1);

	// Check RMobilePhone::NotifyLockInfoChange with aLock=ELockICC completes with aLockInfo.iSetting=ELockSetEnabled
	TCmpTMobilePhoneLockInfoV1NoStatus lockInfoCmp1(expectedLockInfo,lockInfo1,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifyLockInfoChange(mobilePhone,notifyLockInfoChangeStatus1,
					mobilePhoneLock1,RMobilePhone::ELockICC,lockInfoCmp1,KErrNone);
	ASSERT_EQUALS(lockInfo1.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::NotifyLockInfoChange TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));

	// Check RMobilePhone::GetLockInfo with aLock=ELockICC  returns aLockInfo.iSetting=ELockSetEnabled
	TExtEtelRequestStatus getLockInfoStatus(mobilePhone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockICC, infoPckg1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	ASSERT_EQUALS(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	ASSERT_EQUALS(lockInfo1.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));

	// ===  Disable PIN1 ===
	// post notification
	RMobilePhone::TMobilePhoneLock mobilePhoneLock2;
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo2;
    expectedLockInfo.iSetting = RMobilePhone::ELockSetDisabled;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg2(lockInfo2);
	TExtEtelRequestStatus notifyLockInfoChangeStatus2(mobilePhone, EMobilePhoneNotifyLockInfoChange);
	CleanupStack::PushL(notifyLockInfoChangeStatus2);
	mobilePhone.NotifyLockInfoChange(notifyLockInfoChangeStatus2, mobilePhoneLock2, infoPckg2);

	// Disable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetDisabled 
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled,pin1);

	// Check RMobilePhone::NotifyLockInfoChange with aLock=ELockICC completes with aLockInfo.iStatus=EStatusUnlocked and aLockInfo.iSetting=ELockSetDisabled
	TCmpTMobilePhoneLockInfoV1NoStatus lockInfoCmp2(expectedLockInfo,lockInfo2,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifyLockInfoChange(mobilePhone,notifyLockInfoChangeStatus2,
			mobilePhoneLock2,RMobilePhone::ELockICC,lockInfoCmp2,KErrNone);
	ASSERT_EQUALS(lockInfo2.iSetting, RMobilePhone::ELockSetDisabled, _L("RMobilePhone::NotifyLockInfoChange TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));
	
	// Check RMobilePhone::GetLockInfo with aLock=ELockICC  returns  aLockInfo.iSetting=ELockSetDisabled
    mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockICC, infoPckg2);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
    ASSERT_EQUALS(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	ASSERT_EQUALS(lockInfo2.iSetting, RMobilePhone::ELockSetDisabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyLockInfoChangeStatus1
	// getLockInfoStatus
	// notifyLockInfoChangeStatus2
	CleanupStack::Pop(3, &notifyLockInfoChangeStatus1);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0006");
	}



CCTSYIntegrationTestPhoneSecurity0007::CCTSYIntegrationTestPhoneSecurity0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0007::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0007::~CCTSYIntegrationTestPhoneSecurity0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0007
 * @SYMFssID BA/CTSY/PSEC-0007
 * @SYMTestCaseDesc Check failure of PIN1 change procedure when PIN1 is blocked
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Cannot unlock a blocked PIN.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify changing PIN1 fails.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC. 

	// ===  Block PIN1 ===

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 

	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 

	// Block PIN1 by calling RMobilePhone::VerifySecurityCode with aType=ESecurityCodePin1, aCode = wrong PIN1 until RMobilePhone::GetLockInfo with aLock=ELockICC returns TMobilePhoneSecurityCodeInfoV5::iRemainingEntryAttempts=0 then calling RMobilePhone::VerifySecurityCode one more time with aCode=wrong PIN1. 

	// Check RMobilePhone::GetLockInfo with aLock=ELockICC returns iSetting=ELockSetEnabled and iStatus=EStatusBlocked


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Change the security code with RMobilePhone::ChangeSecurityCode with aType=ESecurityCodePin1 , aChange.iOldPassword=old PIN1 and aChange.iNewPassword=new valid PIN1 

	// Check error is returned.

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0007");
	}



CCTSYIntegrationTestPhoneSecurity0008::CCTSYIntegrationTestPhoneSecurity0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0008::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0008::~CCTSYIntegrationTestPhoneSecurity0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0008
 * @SYMFssID BA/CTSY/PSEC-0008
 * @SYMTestCaseDesc Unblock PIN1.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetSecurityCodeInfo, RMobilePhone::NotifySecurityEvent, RMobilePhone::GetLockInfo, RMobilePhone::NotifyLockInfoChange, RMobilePhone::NotifySecurityCapsChange
 * @SYMTestExpectedResults Pass - PIN1 is unblocked
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify PIN1 can be unblocked and number of retry attempts is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//reigster with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 
	TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin1;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// ===  Block PIN1 ===

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 
	expectedCaps = RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	
	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin1; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin1, pin1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	
	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC,RMobilePhone::ELockSetEnabled, pin1);

	// Block PIN1 by calling RMobilePhone::VerifySecurityCode with aType=ESecurityCodePin1, aCode = wrong PIN1 until RMobilePhone::GetLockInfo with aLock=ELockICC returns TMobilePhoneSecurityCodeInfoV5::iRemainingEntryAttempts=0 then calling RMobilePhone::VerifySecurityCode one more time with aCode=wrong PIN1. 
	iPhoneTsyTestHelper.BlockPinL(mobilePhone,RMobilePhone::ESecurityCodePin1);
	
	// Check RMobilePhone::GetLockInfo with aLock=ELockICC returns iSetting=ELockSetEnabled and iStatus=EStatusBlocked
	TExtEtelRequestStatus getLockInfoStatus(mobilePhone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockICC, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	CHECK_TRUE_L(getLockInfoStatus.Int() != KErrNone, _L("RMobilePhone::GetLockInfo did not returned with an error"));
	CHECK_EQUALS_L(lockInfo.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iStatus is not EStatusBlocked as expected.<font color=Black>"));
	// CHECK_EQUALS_L(lockInfo.iStatus, RMobilePhone::EStatusBlocked, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iStatus is not EStatusBlocked as expected"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin1 returns aSecurityCodeInfo.iRemainingEntryAttempts=0
	TExtEtelRequestStatus getSecurityCodeInfoStatus(mobilePhone, EMobilePhoneGetSecurityCodeInfo);
	CleanupStack::PushL( getSecurityCodeInfoStatus );
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 getSecurityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg getSecurityCodeInfoPckg( getSecurityCodeInfo );
	mobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,RMobilePhone::ESecurityCodePin1, getSecurityCodeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
	ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	ASSERT_EQUALS(getSecurityCodeInfo.iRemainingEntryAttempts, 0, _L("RMobilePhone::GetSecurityCodeInfo returned incorrect aSecurityCodeInfo.iRemainingEntryAttempts"));
	
	//post notification for security caps change
	TExtEtelRequestStatus notifySecurityCapsChangeStatus(mobilePhone, EMobilePhoneNotifySecurityCapsChange);
	CleanupStack::PushL( notifySecurityCapsChangeStatus );		
	secCaps = 0;
	mobilePhone.NotifySecurityCapsChange(notifySecurityCapsChangeStatus,secCaps);

	// post notification for lock info change
	RMobilePhone::TMobilePhoneLock lock;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfoChange;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoChangePckg(lockInfoChange);
	TExtEtelRequestStatus notifyNotifyLockInfoChangeStatus(mobilePhone, EMobilePhoneNotifyLockInfoChange);
	CleanupStack::PushL(notifyNotifyLockInfoChangeStatus);
	mobilePhone.NotifyLockInfoChange(notifyNotifyLockInfoChangeStatus, lock, lockInfoChangePckg);
	
	// Unblock PIN1 with RMobilePhone::VerifySecurityCode with aType=ESecurityCodePuk1, aCode=new PIN1, aUnblockCode=PUK1 
	// Check RMobilePhone::NotifySecurityEvent completes with aEvent=EPuk1Verified	
	TPtrC puk1;
	ASSERT_TRUE(GetStringFromConfig(KIniSectionSIMParams, KIniPuk1, puk1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	UnblockPinL(mobilePhone,RMobilePhone::ESecurityCodePuk1, pin1, puk1);
	// Check RMobilePhone::NotifySecurityCapsChange completes with caps in set of KCapsLockICC | KCapsAccessPin1 | KCapsAccessPin2
    expectedCaps = RMobilePhone::KCapsLockICC | RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsAccessPin2;
    iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityCapsChange(mobilePhone,notifySecurityCapsChangeStatus,
    						secCaps,expectedCaps,KNoUnwantedBits, KErrNone);

	// Check RMobilePhone::NotifyLockInfoChange with aLock=ELockICC completes with aLockInfo.iStatus=EStatusLocked and aLockInfo.iSetting=ELockSetEnabled
	RMobilePhone::TMobilePhoneLockInfoV1 expectedLockInfo;
	expectedLockInfo.iSetting = RMobilePhone::ELockSetEnabled;
	TCmpTMobilePhoneLockInfoV1NoStatus mobilePhoneLockInfoV1CmpBase(lockInfoChange,expectedLockInfo,*this);
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::NotifyLockInfoChange returns incorrect lock information from LTSY.<font color=Black>"));
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifyLockInfoChange(mobilePhone,notifyNotifyLockInfoChangeStatus,
			lock,RMobilePhone::ELockICC,mobilePhoneLockInfoV1CmpBase,KErrNone);

	// Check RMobilePhone::GetLockInfo with aLock=ELockICC  returs aLockInfo.iStatus=EStatusLocked and aLockInfo.iSetting=ELockSetEnabled
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockICC, infoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	ASSERT_EQUALS(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	ASSERT_EQUALS(lockInfo.iStatus, RMobilePhone::EStatusLocked, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iStatus is not EStatusLocked as expected"));
	ASSERT_EQUALS(lockInfo.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));

	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin1 returns aSecurityCodeInfo.iRemainingEntryAttempts=3
	mobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,RMobilePhone::ESecurityCodePin1, getSecurityCodeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
	ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	ASSERT_EQUALS(getSecurityCodeInfo.iRemainingEntryAttempts, 3, _L("RMobilePhone::GetSecurityCodeInfo returned incorrect aSecurityCodeInfo.iRemainingEntryAttempts"));

	//
	// TEST END
	//

    StartCleanup();
	
    iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled, pin1);

	// 1 getLockInfoStatus
    // 2 getSecurityCodeInfoStatus
    // 3 notifySecurityCapsChangeStatus
    // 4 notifyNotifyLockInfoChangeStatus
    
	CleanupStack::PopAndDestroy(4, &getLockInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0008");
	}



CCTSYIntegrationTestPhoneSecurity0009::CCTSYIntegrationTestPhoneSecurity0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0009::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0009::~CCTSYIntegrationTestPhoneSecurity0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0009
 * @SYMFssID BA/CTSY/PSEC-0009
 * @SYMTestCaseDesc Change PIN2.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySecurityCodeInfoChange, RMobilePhone::GetSecurityCodeInfo
 * @SYMTestExpectedResults Pass - PIN2 is changed
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify PIN2 is changed and security code info notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//register with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
    TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin2;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin2; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin2, pin2) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	
	// Enable PIN2 with RMobilePhone::SetLockSetting with aLock=ELockPin2, aSetting=ELockSetEnabled 
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::SetLockSetting returns KErrNotSupported from LTSY.<font color=Black>"));
	//iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2,RMobilePhone::ELockSetEnabled, pin2);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// post notification for NotifySecurityCodeInfoChange
	TExtEtelRequestStatus notifySecurityCodeInfoChangeStatus(mobilePhone, EMobilePhoneNotifySecurityCodeInfoChange);
	CleanupStack::PushL( notifySecurityCodeInfoChangeStatus );
	RMobilePhone::TMobilePhoneSecurityCode securityCode;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg securityCodeInfoPckg( securityCodeInfo );
	mobilePhone.NotifySecurityCodeInfoChange(notifySecurityCodeInfoChangeStatus, securityCode,securityCodeInfoPckg);
	
	// Change the security code with RMobilePhone::ChangeSecurityCode with aType=ESecurityCodePin2 , aChange.iOldPassword=old PIN2 and aChange.iNewPassword=new valid PIN2 
	TExtEtelRequestStatus changeSecurityCodeStatus(mobilePhone, EMobilePhoneChangeSecurityCode);
	CleanupStack::PushL( changeSecurityCodeStatus );
	RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;
	passwordChange.iOldPassword = pin2; 
	ManglePassword(pin2,passwordChange.iNewPassword );
	mobilePhone.ChangeSecurityCode(changeSecurityCodeStatus,RMobilePhone::ESecurityCodePin2,passwordChange);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(changeSecurityCodeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ChangeSecurityCode timed out"));
	ASSERT_EQUALS( changeSecurityCodeStatus.Int(),KErrNone, _L("RMobilePhone::ChangeSecurityCode returned an error"));

	// Check RMobilePhone::NotifySecurityCodeInfoChange with aSecurityCode=ESecurityCodePin2 completes with aSecurityCodeInfo.iRemainingEntryAttempts=3
	RMobilePhone::TMobilePhoneSecurityCode expectedSecurityCode = RMobilePhone::ESecurityCodePin2;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 expectedSecurityCodeInfo;
	expectedSecurityCodeInfo.iRemainingEntryAttempts = 3;
	TCmpBase<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> mobilePhoneSecurityCodeInfoCmpBase(expectedSecurityCodeInfo,securityCodeInfo,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityCodeInfoChange(mobilePhone,notifySecurityCodeInfoChangeStatus,
			expectedSecurityCode,mobilePhoneSecurityCodeInfoCmpBase, KErrNone);


	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin2 returns aSecurityCodeInfo.iRemainingEntryAttempts=3
	TExtEtelRequestStatus getSecurityCodeInfoStatus(mobilePhone, EMobilePhoneGetSecurityCodeInfo);
	CleanupStack::PushL( getSecurityCodeInfoStatus );
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 getSecurityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg getSecurityCodeInfoPckg( getSecurityCodeInfo );
	mobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,RMobilePhone::ESecurityCodePin2, getSecurityCodeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
	ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	ASSERT_EQUALS(getSecurityCodeInfo.iRemainingEntryAttempts, 3, _L("RMobilePhone::GetSecurityCodeInfo returned incorrect remaining entry attempts"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	passwordChange.iOldPassword = passwordChange.iNewPassword;
	passwordChange.iNewPassword = pin2;
    mobilePhone.ChangeSecurityCode(changeSecurityCodeStatus,RMobilePhone::ESecurityCodePin2,passwordChange);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(changeSecurityCodeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ChangeSecurityCode timed out"));
	ASSERT_EQUALS(changeSecurityCodeStatus.Int(), KErrNone, _L("RMobilePhone::ChangeSecurityCode returned an error"));

    //iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetDisabled, pin2);

	// 1 notifySecurityCodeInfoChangeStatus
    // 2 changeSecurityCodeStatus
    // 3 getSecurityCodeInfoStatus
    
	CleanupStack::PopAndDestroy(3, &notifySecurityCodeInfoChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0009");
	}



CCTSYIntegrationTestPhoneSecurity0010::CCTSYIntegrationTestPhoneSecurity0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0010::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0010::~CCTSYIntegrationTestPhoneSecurity0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0010
 * @SYMFssID BA/CTSY/PSEC-0010
 * @SYMTestCaseDesc Change PIN2 supplying incorrect old PIN2.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySecurityCodeInfoChange, RMobilePhone::GetSecurityCodeInfo
 * @SYMTestExpectedResults Pass - PIN2 is not changed
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrAccessDenied returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//register with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
    TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin2;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin2; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin2, pin2) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	
	// Enable PIN2 with RMobilePhone::SetLockSetting with aLock=ELockPin2, aSetting=ELockSetEnabled 
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::SetLockSetting returns KErrNotSupported from LTSY.<font color=Black>"));
	//iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2,RMobilePhone::ELockSetEnabled, pin2);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// post notification for NotifySecurityCodeInfoChange
	TExtEtelRequestStatus notifySecurityCodeInfoChangeStatus(mobilePhone, EMobilePhoneNotifySecurityCodeInfoChange);
	CleanupStack::PushL( notifySecurityCodeInfoChangeStatus );
	RMobilePhone::TMobilePhoneSecurityCode securityCode;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg securityCodeInfoPckg( securityCodeInfo );
	mobilePhone.NotifySecurityCodeInfoChange(notifySecurityCodeInfoChangeStatus, securityCode,securityCodeInfoPckg);
	
	// Attempt to change PIN2 with incorrect old PIN2 with RMobilePhone::ChangeSecurityCode with aType=ESecurityCodePin2 , aChange.iOldPassword=incorrect old PIN2 and aChange.iNewPassword=new valid PIN2 
	TExtEtelRequestStatus changeSecurityCodeStatus(mobilePhone, EMobilePhoneChangeSecurityCode);
	CleanupStack::PushL( changeSecurityCodeStatus );
	RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;
	passwordChange.iOldPassword = _L("abcd"); // incorrect pin
	passwordChange.iNewPassword = pin2;
	mobilePhone.ChangeSecurityCode(changeSecurityCodeStatus,RMobilePhone::ESecurityCodePin2,passwordChange);
	
	// Check KErrGsm0707IncorrectPassword/ KErrAccessDenied  is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(changeSecurityCodeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ChangeSecurityCode timed out"));
	ASSERT_TRUE( ( changeSecurityCodeStatus.Int() == KErrAccessDenied
			|| changeSecurityCodeStatus.Int() == KErrGsm0707IncorrectPassword ), _L("RMobilePhone::ChangeSecurityCode did not return an error"));

	// Check RMobilePhone::NotifySecurityCodeInfoChange with aSecurityCode=ESecurityCodePin2 completes with aSecurityCodeInfo.iRemainingEntryAttempts=2
	RMobilePhone::TMobilePhoneSecurityCode expectedSecurityCode = RMobilePhone::ESecurityCodePin2;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 expectedSecurityCodeInfo;
	expectedSecurityCodeInfo.iRemainingEntryAttempts = 2;
	TCmpBase<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> mobilePhoneSecurityCodeInfoCmpBase(expectedSecurityCodeInfo,securityCodeInfo,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityCodeInfoChange(mobilePhone,notifySecurityCodeInfoChangeStatus,
			expectedSecurityCode,mobilePhoneSecurityCodeInfoCmpBase, KErrNone);


	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin2 returns aSecurityCodeInfo.iRemainingEntryAttempts=2
	TExtEtelRequestStatus getSecurityCodeInfoStatus(mobilePhone, EMobilePhoneGetSecurityCodeInfo);
	CleanupStack::PushL( getSecurityCodeInfoStatus );
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 getSecurityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg getSecurityCodeInfoPckg( getSecurityCodeInfo );
	mobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,RMobilePhone::ESecurityCodePin2, getSecurityCodeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
	ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	ASSERT_EQUALS(getSecurityCodeInfo.iRemainingEntryAttempts, 2, _L("RMobilePhone::GetSecurityCodeInfo returned incorrect remaining entry attempts"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetDisabled, pin2);

	// 1 notifySecurityCodeInfoChangeStatus
    // 2 changeSecurityCodeStatus
    // 3 getSecurityCodeInfoStatus
    
	CleanupStack::PopAndDestroy(3, &notifySecurityCodeInfoChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0010");
	}



CCTSYIntegrationTestPhoneSecurity0011::CCTSYIntegrationTestPhoneSecurity0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0011::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0011::~CCTSYIntegrationTestPhoneSecurity0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0011
 * @SYMFssID BA/CTSY/PSEC-0011
 * @SYMTestCaseDesc Check failure of PIN2 change procedure when PIN2 is blocked
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Cannot change a blocked PIN2.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify PIN2 cannot be changed.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// ===  Block PIN2 ===
	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//register with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin2;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin2; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin2, pin2) != EFalse, 
			_L("GetStringFromConfig returned with an error"));

	// Enable PIN2 with RMobilePhone::SetLockSetting with aLock=ELockPin2, aSetting=ELockSetEnabled 
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::SetLockSetting returns KErrNotSupported from LTSY.<font color=Black>"));
	//iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2,RMobilePhone::ELockSetEnabled, pin2);

	// Block PIN2 by calling RMobilePhone::VerifySecurityCode with aType=ESecurityCodePin2, aCode = wrong PIN2 until RMobilePhone::GetLockInfo with aLock=ELockICC returns TMobilePhoneSecurityCodeInfoV5::iRemainingEntryAttempts=0 then calling RMobilePhone::VerifySecurityCode one more time with aCode=wrong PIN2. 
	iPhoneTsyTestHelper.BlockPinL(mobilePhone,RMobilePhone::ESecurityCodePin2);
	
	// Check RMobilePhone::GetLockInfo with aLock=ELockPin2 returns iSetting=ELockSetEnabled and iStatus=EStatusBlocked
	TExtEtelRequestStatus getLockInfoStatus(mobilePhone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockPin2, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	CHECK_TRUE_L(getLockInfoStatus.Int() != KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	CHECK_EQUALS_L(lockInfo.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iStatus is not EStatusBlocked as expected.<font color=Black>"));
	// CHECK_EQUALS_L(lockInfo.iStatus, RMobilePhone::EStatusBlocked, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iStatus is not EStatusBlocked as expected"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Change the security code with RMobilePhone::ChangeSecurityCode with aType=ESecurityCodePin2 , aChange.iOldPassword=old PIN2 and aChange.iNewPassword=new valid PIN2 
	TExtEtelRequestStatus changeSecurityCodeStatus(mobilePhone, EMobilePhoneChangeSecurityCode);
	CleanupStack::PushL( changeSecurityCodeStatus );
	RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;
	passwordChange.iOldPassword = pin2; 
	ManglePassword(pin2,passwordChange.iNewPassword);
	mobilePhone.ChangeSecurityCode(changeSecurityCodeStatus,RMobilePhone::ESecurityCodePin2,passwordChange);
	
	// Check error is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(changeSecurityCodeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ChangeSecurityCode timed out"));
	ASSERT_TRUE( changeSecurityCodeStatus.Int()!= KErrNone, _L("RMobilePhone::ChangeSecurityCode returned KErrNone"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	TPtrC puk2; 
	ASSERT_TRUE(GetStringFromConfig(KIniSectionSIMParams, KIniPuk2, puk2) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	UnblockPinL(mobilePhone,RMobilePhone::ESecurityCodePuk2, pin2, puk2);

    //iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetDisabled, pin2);

	// 1 getLockInfoStatus
    // 2 changeSecurityCodeStatus
    
	CleanupStack::PopAndDestroy(2, &getLockInfoStatus);

	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0011");
	}



CCTSYIntegrationTestPhoneSecurity0012::CCTSYIntegrationTestPhoneSecurity0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0012::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0012::~CCTSYIntegrationTestPhoneSecurity0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0012
 * @SYMFssID BA/CTSY/PSEC-0012
 * @SYMTestCaseDesc Unblock PIN2.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetSecurityCodeInfo, RMobilePhone::GetLockInfo, RMobilePhone::NotifySecurityEvent, RMobilePhone::NotifySecurityCapsChange
 * @SYMTestExpectedResults Pass - PIN2 is unblocked
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify PIN2 is unblocked and number of retry attempts is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	// ===  Block PIN2 ===
	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//register with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin2;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin2; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin2, pin2) != EFalse, 
			_L("GetStringFromConfig returned with an error"));

	// Enable PIN2 with RMobilePhone::SetLockSetting with aLock=ELockPin2, aSetting=ELockSetEnabled 	
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::SetLockSetting returns KErrNotSupported from LTSY.<font color=Black>"));
	//iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2,RMobilePhone::ELockSetEnabled, pin2);

	// Block PIN2 by calling RMobilePhone::VerifySecurityCode with aType=ESecurityCodePin2, aCode = wrong PIN2 until RMobilePhone::GetLockInfo with aLock=ELockICC returns TMobilePhoneSecurityCodeInfoV5::iRemainingEntryAttempts=0 then calling RMobilePhone::VerifySecurityCode one more time with aCode=wrong PIN2. 
	iPhoneTsyTestHelper.BlockPinL(mobilePhone,RMobilePhone::ESecurityCodePin2);
	
	// Check RMobilePhone::GetLockInfo with aLock=ELockPin2 returns iSetting=ELockSetEnabled and iStatus=EStatusBlocked
	TExtEtelRequestStatus getLockInfoStatus(mobilePhone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);
	DEBUG_PRINTF2(_L("AAA %d"),__LINE__);
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockPin2, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	DEBUG_PRINTF2(_L("AAA %d"),__LINE__);
	CHECK_TRUE_L(getLockInfoStatus.Int() != KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	CHECK_EQUALS_L(lockInfo.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iStatus is not EStatusBlocked as expected.<font color=Black>"));
	// CHECK_EQUALS_L(lockInfo.iStatus, RMobilePhone::EStatusBlocked, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iStatus is not EStatusBlocked as expected"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin2 returns aSecurityCodeInfo.iRemainingEntryAttempts=0
	TExtEtelRequestStatus getSecurityCodeInfoStatus(mobilePhone, EMobilePhoneGetSecurityCodeInfo);
	CleanupStack::PushL( getSecurityCodeInfoStatus );
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 getSecurityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg getSecurityCodeInfoPckg( getSecurityCodeInfo );
	mobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,RMobilePhone::ESecurityCodePin2, getSecurityCodeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
	ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	ASSERT_EQUALS(getSecurityCodeInfo.iRemainingEntryAttempts, 0, _L("RMobilePhone::GetSecurityCodeInfo returned incorrect aSecurityCodeInfo.iRemainingEntryAttempts"));
	
	//post notification for security caps change
	TExtEtelRequestStatus notifySecurityCapsChangeStatus(mobilePhone, EMobilePhoneNotifySecurityCapsChange);
	CleanupStack::PushL( notifySecurityCapsChangeStatus );		
	secCaps = 0;
	mobilePhone.NotifySecurityCapsChange(notifySecurityCapsChangeStatus,secCaps);

	// post notification for lock info change
	RMobilePhone::TMobilePhoneLock lock;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfoChange;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoChangePckg(lockInfoChange);
	TExtEtelRequestStatus notifyNotifyLockInfoChangeStatus(mobilePhone, EMobilePhoneNotifyLockInfoChange);
	CleanupStack::PushL(notifyNotifyLockInfoChangeStatus);
	mobilePhone.NotifyLockInfoChange(notifyNotifyLockInfoChangeStatus, lock, lockInfoChangePckg);
	
	// Unblock PIN2 with RMobilePhone::VerifySecurityCode with aType=ESecurityCodePuk2, aCode=new PIN2, aUnblockCode=PUK2 
	// Check RMobilePhone::NotifySecurityEvent completes with aEvent=EPuk2Verified
	TPtrC puk2; 
	ASSERT_TRUE(GetStringFromConfig(KIniSectionSIMParams, KIniPuk2, puk2) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	UnblockPinL(mobilePhone,RMobilePhone::ESecurityCodePuk2, pin2, puk2);
	
	// Check RMobilePhone::NotifySecurityCapsChange completes with caps in set of KCapsLockICC | KCapsAccessPin1 | KCapsAccessPin2
    expectedCaps = RMobilePhone::KCapsLockICC | RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsAccessPin2;
    iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityCapsChange(mobilePhone,notifySecurityCapsChangeStatus,
    						secCaps,expectedCaps,KNoUnwantedBits, KErrNone);

    // Check RMobilePhone::NotifyLockInfoChange with aLock=ELockPin2 completes with aLockInfo.iStatus=EStatusLocked and aLockInfo.iSetting=ELockSetEnabled
	RMobilePhone::TMobilePhoneLockInfoV1 expectedLockInfo;
	expectedLockInfo.iSetting = RMobilePhone::ELockSetEnabled;
	TCmpTMobilePhoneLockInfoV1NoStatus mobilePhoneLockInfoV1CmpBase(lockInfoChange,expectedLockInfo,*this);
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::NotifyLockInfoChange returns incorrect lock information from LTSY.<font color=Black>"));
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifyLockInfoChange(mobilePhone,notifyNotifyLockInfoChangeStatus,
			lock,RMobilePhone::ELockPin2,mobilePhoneLockInfoV1CmpBase,KErrNone);

	// Check RMobilePhone::GetLockInfo with aLock=ELockPin2 returns aLockInfo.iStatus=EStatusLocked and aLockInfo.iSetting=ELockSetEnabled
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockPin2, infoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	ASSERT_EQUALS(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	ASSERT_EQUALS(lockInfo.iStatus, RMobilePhone::EStatusLocked, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iStatus is not EStatusLocked as expected"));
	ASSERT_EQUALS(lockInfo.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));

	// Check RMobilePhone::GetSecurityCodeInfo with aSecurityCode=ESecurityCodePin2 returns aSecurityCodeInfo.iRemainingEntryAttempts=3
	mobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus,RMobilePhone::ESecurityCodePin1, getSecurityCodeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
	ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	ASSERT_EQUALS(getSecurityCodeInfo.iRemainingEntryAttempts, 3, _L("RMobilePhone::GetSecurityCodeInfo returned incorrect aSecurityCodeInfo.iRemainingEntryAttempts"));

	//
	// TEST END
	//

    StartCleanup();
	
    // iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetDisabled, pin2);

	// 1 getLockInfoStatus
    // 2 getSecurityCodeInfoStatus
    // 3 notifySecurityCapsChangeStatus
    // 4 notifyNotifyLockInfoChangeStatus
    
	CleanupStack::PopAndDestroy(4, &getLockInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0012");
	}



CCTSYIntegrationTestPhoneSecurity0013::CCTSYIntegrationTestPhoneSecurity0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0013::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0013::~CCTSYIntegrationTestPhoneSecurity0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0013
 * @SYMFssID BA/CTSY/PSEC-0013
 * @SYMTestCaseDesc Abort unlock PIN1 procedure
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::AbortSecurityCode, RMobilePhone::NotifySecurityEvent
 * @SYMTestExpectedResults Pass - Fails for SetLockSetting .
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify operation that requires PIN1 completes with an error.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//register with home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 
    TUint32 secCaps = 0;
    TUint32 expectedCaps = RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin1; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin1, pin1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	
	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC,RMobilePhone::ELockSetEnabled, pin1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Make a request to something that requires PIN1 i.e. SetLockSetting ===

	// post notification
	RMobilePhone::TMobilePhoneSecurityEvent event;
	TExtEtelRequestStatus notifySecurityEventStatus(mobilePhone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL(notifySecurityEventStatus);
	mobilePhone.NotifySecurityEvent(notifySecurityEventStatus, event);
	
	// Call RMobilePhone::SetLockSetting 
    TExtEtelRequestStatus setLockSettingStatus(mobilePhone, EMobilePhoneSetLockSetting);
	CleanupStack::PushL(setLockSettingStatus);
	mobilePhone.SetLockSetting(setLockSettingStatus, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled);


	// Check RMobilePhone::NotifySecurityEvent completes with aEvent=EPin1Required
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityEvent(mobilePhone,notifySecurityEventStatus,
			event,
			RMobilePhone::EPin1Required,
			KErrNone);

	// Abort security code with RMobilePhone::AbortSecurityCode with aType=ESecurityCodePin1 
	ASSERT_EQUALS( mobilePhone.AbortSecurityCode(RMobilePhone::ESecurityCodePin1), KErrNone, _L("RMobilePhone::AbortSecurityCode returned KErrNone"));
	
	// Check RMobilePhone::SetLockSetting completes with an error.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setLockSettingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetLockSetting timed out"));
	ASSERT_TRUE(setLockSettingStatus.Int() != KErrNone, _L("RMobilePhone::SetLockSetting returned KErrNone"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled, pin1);

	// 1 notifySecurityEventStatus
    // 2 setLockSettingStatus
    
	CleanupStack::PopAndDestroy(2, &notifySecurityEventStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0013");
	}



CCTSYIntegrationTestPhoneSecurity0014::CCTSYIntegrationTestPhoneSecurity0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0014::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0014::~CCTSYIntegrationTestPhoneSecurity0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0014
 * @SYMFssID BA/CTSY/PSEC-0014
 * @SYMTestCaseDesc Verify a code that doesn't need verifying
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetLockInfo, RMobilePhone::SetLockSetting
 * @SYMTestExpectedResults Pass - Fails to verify PIN2
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotReady is returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10005</font>"));

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 
    RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    TUint32 secCaps;
    TUint32 expectedCaps = (RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC);
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(secCaps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(secCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned with unexpected caps"));
    
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Verify PIN1 when phone is not asking for it ===

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin1; 
	ASSERT_TRUE(GetStringFromConfig(*networkSection, KIniPin1, pin1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	
	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC, RMobilePhone::ELockSetEnabled, pin1);

	// Verify PIN1 with RMobilePhone::VerifySecurityCode with aType=ESecurityCodePin1, aCode=PIN1 code 
    TExtEtelRequestStatus verifySecCodeStatus(mobilePhone, EMobilePhoneVerifySecurityCode);
	CleanupStack::PushL(verifySecCodeStatus);
	mobilePhone.VerifySecurityCode(verifySecCodeStatus, RMobilePhone::ESecurityCodePin1, pin1, pin1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(verifySecCodeStatus, ETimeLong), KErrNone,_L("RMobilePhone::VerifySecurityCode timed out"));
	// Check KErrNotReady is returned	
	ASSERT_EQUALS(verifySecCodeStatus.Int(), KErrNotReady, _L("RMobilePhone::VerifySecurityCode did not return KErrNotReady as expected"));

	// Check RMobilePhone::GetLockInfo with aLock=ELockICC returns aLockInfo.iSetting=ELockSetEnabled
    RMobilePhone::TMobilePhoneLockInfoV1 mobilePhoneLockInfoV1;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoV1Pckg(mobilePhoneLockInfoV1);
	TExtEtelRequestStatus getLockInfoStatus(mobilePhone, EMobilePhoneVerifySecurityCode);
	CleanupStack::PushL(getLockInfoStatus);
    mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockICC, infoV1Pckg);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
    ASSERT_EQUALS(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	ASSERT_EQUALS(mobilePhoneLockInfoV1.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));
	
	// ===  Verify PIN2 when phone is not asking for it ===

	// Enable PIN2 with RMobilePhone::SetLockSetting with aLock=ELockPin2, aSetting=ELockSetEnabled 
	// Get the password from the ini file
	TPtrC pin2; 
	ASSERT_TRUE(GetStringFromConfig(*networkSection, KIniPin2, pin2) != EFalse, 
			_L("GetStringFromConfig returned with an error"));
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetEnabled, pin2);

	// Verify PIN2 with RMobilePhone::VerifySecurityCode with aType=ESecurityCodePin2, aCode=PIN2 code 
    mobilePhone.VerifySecurityCode(verifySecCodeStatus, RMobilePhone::ESecurityCodePin2, pin2, pin2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(verifySecCodeStatus, ETimeLong), KErrNone, _L("RMobilePhone::VerifySecurityCode timed out"));
	
	// Check KErrNotReady is returned
    ASSERT_EQUALS(verifySecCodeStatus.Int(), KErrNotReady, _L("RMobilePhone::VerifySecurityCode did not return KErrNotReady as expected"));
	
	// Check RMobilePhone::GetLockInfo with aLock=ELockPin2 returns aLockInfo.iSetting=ELockSetEnabled
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);		
    mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockPin2, infoPckg);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
    ASSERT_EQUALS(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	ASSERT_EQUALS(lockInfo.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo TMobilePhoneLockInfoV1.iSetting is not ELockSetEnabled as expected"));
	
	//
	// TEST END
	//

    StartCleanup();
    iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled, pin1);
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetDisabled, pin2);

	// Pop
	// getVerifySecCodeStatus
	// getLockInfoStatus
	CleanupStack::Pop(2, &verifySecCodeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0014");
	}



CCTSYIntegrationTestPhoneSecurity0015::CCTSYIntegrationTestPhoneSecurity0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0015::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0015::~CCTSYIntegrationTestPhoneSecurity0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0015
 * @SYMFssID BA/CTSY/PSEC-0015
 * @SYMTestCaseDesc Abort a code that doesn't need aborting
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetLockSetting, RMobilePhone::AbortSecurityCode, RMobilePhone::NotifySecurityEvent, RMobilePhone::SetFdnSetting
 * @SYMTestExpectedResults Pass - Fails to abort PIN1 security event notification
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10030</font>"));
	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 caps;
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC 
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(caps), KErrNone, _L("RMobilePhone::GetSecurityCaps returned an error"));
	TInt32 expectedCaps =  RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC;
	CHECK_BITS_SET_L(caps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned incorrect Security Caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(mobilePhone, networkSection);
	// Get the password from the ini file
	TPtrC pin2; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin2, pin2) != EFalse, 
			_L("GetStringFromConfig returned with an error"));


	// part I
	// ===  Make a request to something that requires PIN2 i.e. SetLockSetting but abort PIN1 ===	
	RMobilePhone::TMobilePhoneSecurityEvent event;
	TExtEtelRequestStatus notifySecurityEventStatus(mobilePhone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL(notifySecurityEventStatus);
	mobilePhone.NotifySecurityEvent(notifySecurityEventStatus, event);
		
	// Enable PIN2 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 
    TExtEtelRequestStatus setLockSettingStatus(mobilePhone, EMobilePhoneSetLockSetting);
	CleanupStack::PushL(setLockSettingStatus);
	mobilePhone.SetLockSetting(setLockSettingStatus, RMobilePhone::ELockPin2, RMobilePhone::ELockSetEnabled);

	// Check RMobilePhone::NotifySecurityEvent completes with aEvent=EPin2Required
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityEvent(mobilePhone,notifySecurityEventStatus,
			event,
			RMobilePhone::EPin2Required,
			KErrNone);

	// Check RMobilePhone::AbortSecurityCode with aType=ESecurityCodePin1 returns an error
	ASSERT_TRUE( mobilePhone.AbortSecurityCode(RMobilePhone::ESecurityCodePin1)!= KErrNone, _L("RMobilePhone::AbortSecurityCode returned KErrNone"));
	
	// Check RMobilePhone::SetLockSetting completes with an error
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setLockSettingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetLockSetting timed out"));
	ASSERT_TRUE(setLockSettingStatus.Int() != KErrNone, _L("RMobilePhone::SetLockSetting returned KErrNone"));
	
	// part II
	// ===  Make a request to something that requires PIN2 i.e. SetFdnStatus but abort PIN1 ===	
	RMobilePhone::TMobilePhoneFdnSetting fdnSetting = RMobilePhone::EFdnSetOn;
    TExtEtelRequestStatus setFdnSettingStatus(mobilePhone, EMobilePhoneSetFdnSetting);
	CleanupStack::PushL(setFdnSettingStatus);
	mobilePhone.NotifySecurityEvent(notifySecurityEventStatus, event);
	mobilePhone.SetFdnSetting(setFdnSettingStatus, fdnSetting);

	// Check RMobilePhone::NotifySecurityEvent completes with aEvent=EPin2Required
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifySecurityEvent(mobilePhone,notifySecurityEventStatus,
			event,
			RMobilePhone::EPin2Required,
			KErrNone);

	// Check RMobilePhone::AbortSecurityCode with aType=ESecurityCodePin1 returns an error
	ASSERT_TRUE( mobilePhone.AbortSecurityCode(RMobilePhone::ESecurityCodePin1)!= KErrNone, _L("RMobilePhone::AbortSecurityCode returned KErrNone"));
	
	// Check RMobilePhone::SetFdnSetting completes with an error
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFdnSettingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetFdnSetting timed out"));
	ASSERT_TRUE(setFdnSettingStatus.Int() != KErrNone, _L("RMobilePhone::SetFdnSetting returned KErrNone"));

	//
	// TEST END
	//

    StartCleanup();
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetDisabled, pin2);

	// Pop
	// setLockSettingStatus
	// notifySecurityEventStatus
	// setFdnSettingStatus
	CleanupStack::Pop(3, &notifySecurityEventStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0015");
	}



CCTSYIntegrationTestPhoneSecurity0016::CCTSYIntegrationTestPhoneSecurity0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0016::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0016::~CCTSYIntegrationTestPhoneSecurity0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0016
 * @SYMFssID BA/CTSY/PSEC-0016
 * @SYMTestCaseDesc Get the security capabilities.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Correct security caps returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify initial security caps correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobilePhone::GetSecurityCaps returns caps in set of RMobilePhone::KCapsLockPhone

	//         | RMobilePhone::KCapsLockICC 

	//         | RMobilePhone::KCapsLockPhoneToICC 

	//         | RMobilePhone::KCapsAccessPin1 

	//         | RMobilePhone::KCapsAccessPin2 

	//         | RMobilePhone::KCapsAccessPhonePassword 
	
	TUint32 securityCaps=0;
	ASSERT_EQUALS(phone.GetSecurityCaps(securityCaps), KErrNone, _L("MobilePhone::GetSecurityCaps returned with an error"));
	TUint32 expectedCaps= RMobilePhone::KCapsLockPhone | RMobilePhone::KCapsLockICC 
							| RMobilePhone::KCapsLockPhoneToICC	| RMobilePhone::KCapsAccessPin1
							| RMobilePhone::KCapsAccessPin2  | RMobilePhone::KCapsAccessPhonePassword;
	ASSERT_BITS_SET(securityCaps, expectedCaps,KNoUnwantedBits,_L("RMobilePhone::GetSecurityCaps miniumum security capabilites not found:KCapsLockPhone,KCapsLockICC,KCapsLockPhoneToICC,KCapsAccessPin1,KCapsAccessPin2,KCapsAccessPhonePassword."))
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0016");
	}



CCTSYIntegrationTestPhoneSecurity0017::CCTSYIntegrationTestPhoneSecurity0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0017::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0017::~CCTSYIntegrationTestPhoneSecurity0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0017
 * @SYMFssID BA/CTSY/PSEC-0017
 * @SYMTestCaseDesc Enable and disable the phone lock and get lock info.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyLockInfoChange, RMobilePhone::SetLockSetting, 
 * @SYMTestExpectedResults Pass - Phone lock setting and status correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone lock information is correct.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10001</font>"));
	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 caps;
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of RMobilePhone::KCapsLockICC | RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsAccessPin2 
	CHECK_TRUE_L(mobilePhone.GetSecurityCaps(caps) == KErrNone, _L("RMobilePhone::GetSecurityCaps returned an error"))		
	TInt32 expectedCaps =  RMobilePhone::KCapsLockICC | RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsAccessPin2 ;
	CHECK_BITS_SET_L(caps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned incorrect Security Caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Enable phone lock with RMobilePhone::SetLockSetting with aLock=RMobilePhone::ELockPhoneDevice, aSetting=ELockSetEnabled 	
	RMobilePhone::TMobilePhoneLock lock;
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo1;
	RMobilePhone::TMobilePhoneLockInfoV1 expectedLockInfo;
	expectedLockInfo.iSetting = RMobilePhone::ELockSetEnabled;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoPckg1(lockInfo1);
	TExtEtelRequestStatus notifyNotifyLockInfoChangeStatus(mobilePhone, EMobilePhoneNotifyLockInfoChange);
	CleanupStack::PushL(notifyNotifyLockInfoChangeStatus);
	mobilePhone.NotifyLockInfoChange(notifyNotifyLockInfoChangeStatus, lock, lockInfoPckg1); 	

	TPtrC phoneLock; 
	CHECK_TRUE_L(GetStringFromConfig(KIniSectionBoardParams, KIniPhoneLock, phoneLock) != EFalse, 
			_L("GetStringFromConfig returned with an error"));

	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPhoneDevice, RMobilePhone::ELockSetEnabled, phoneLock);

	// Check RMobilePhone::NotifyLockInfoChange with aLock=ELockPhoneDevice completes with aLockInfo.iSetting=ELockSetEnabled
	TCmpTMobilePhoneLockInfoV1NoStatus mobilePhoneLockInfoV1CmpBase(lockInfo1,expectedLockInfo,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifyLockInfoChange(mobilePhone,notifyNotifyLockInfoChangeStatus,
			lock,RMobilePhone::ELockPhoneDevice,mobilePhoneLockInfoV1CmpBase,KErrNone);
	ASSERT_EQUALS(lock, RMobilePhone::ELockPhoneDevice, _L("RMobilePhone::NotifyLockInfoChange returned wrong aLock"));
	ASSERT_EQUALS(lockInfo1.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::NotifyLockInfoChange returned wrong iSetting"));
	
	// Check RMobilePhone::GetLockInfo with aLock=ELockPhoneDevice returns aLockInfo.iSetting=ELockSetEnabled
	TExtEtelRequestStatus getLockInfoStatus(mobilePhone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockPhoneDevice, lockInfoPckg1); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetLockInfo timed-out"));
	ASSERT_EQUALS(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	ASSERT_EQUALS(lockInfo1.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo returned wrong iSetting"));

	// Enable phone lock with RMobilePhone::SetLockSetting with aLock=RMobilePhone::ELockPhoneDevice, aSetting=ELockSetDisabled 
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo2;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoPckg2(lockInfo2);
	mobilePhone.NotifyLockInfoChange(notifyNotifyLockInfoChangeStatus, lock, lockInfoPckg2); 
	
	iPhoneTsyTestHelper.ChangePhoneLockSettingL(mobilePhone, RMobilePhone::ELockPhoneDevice, RMobilePhone::ELockSetDisabled, phoneLock);

	// Check RMobilePhone::NotifyLockInfoChange with aLock=ELockPhoneDevice completes with aLockInfo.iSetting=ELockSetDisabled	
	expectedLockInfo.iSetting = RMobilePhone::ELockSetDisabled;
	expectedLockInfo.iStatus = RMobilePhone::EStatusUnlocked;
	TCmpTMobilePhoneLockInfoV1NoStatus mobilePhoneLockInfoV1CmpBase2(lockInfo2,expectedLockInfo,*this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifyLockInfoChange(mobilePhone,notifyNotifyLockInfoChangeStatus,
			lock,RMobilePhone::ELockPhoneDevice,mobilePhoneLockInfoV1CmpBase2,KErrNone);
	ASSERT_EQUALS(lock, RMobilePhone::ELockPhoneDevice, _L("RMobilePhone::NotifyLockInfoChange returned wrong aLock"));
	ASSERT_EQUALS(lockInfo2.iSetting, RMobilePhone::ELockSetDisabled, _L("RMobilePhone::NotifyLockInfoChange returned wrong iSetting"));
	
	// Check RMobilePhone::GetLockInfo with aLock=ELockPhoneDevice returns aLockInfo.iSetting=ELockSetDisabled
	mobilePhone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockPhoneDevice, lockInfoPckg2); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetLockInfo timed-out"));
	ASSERT_EQUALS(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned with an error"));
	ASSERT_EQUALS(lockInfo2.iSetting, RMobilePhone::ELockSetDisabled, _L("RMobilePhone::GetLockInfo returned wrong iSetting"));

	//
	// TEST END
	//

    StartCleanup();
    //ChangePhoneLockSetting(mobilePhone, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled, pin1);

	// Pop
	// notifyNotifyLockInfoChangeStatus
	// kStatus
	CleanupStack::PopAndDestroy(2, &notifyNotifyLockInfoChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0017");
	}



CCTSYIntegrationTestPhoneSecurity0018::CCTSYIntegrationTestPhoneSecurity0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneSecurity0018::GetTestStepName());
	}

CCTSYIntegrationTestPhoneSecurity0018::~CCTSYIntegrationTestPhoneSecurity0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneSecurity0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSEC-0018
 * @SYMFssID BA/CTSY/PSEC-0018
 * @SYMTestCaseDesc Verify PIN1 needed at startup when PIN1 locked
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySecurityEvent
 * @SYMTestExpectedResults Pass - PIN1 entry is required at boot
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify PIN1 notification is received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled 

	// Close down all subsessions and unload TSY 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Load the TSY 

	// Open a phone session 

	// Check RMobilePhone::NotifySecurityEvent completes with aEvent=EPin1Required

	// Verify PIN1 with RMobilePhone::VerifySecurityCode with aType=ESecurityCodePin1, aCode=PIN1 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneSecurity0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneSecurity0018");
	}



