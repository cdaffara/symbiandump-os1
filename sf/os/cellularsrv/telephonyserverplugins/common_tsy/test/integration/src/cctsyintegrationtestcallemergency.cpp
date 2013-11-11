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
// Test step definitions for the CallEmergency functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestcallemergency.h"
#include "cctsyinidata.h"

const RMobilePhone::TMobilePassword KAltWrongPin = _L("abcd");
_LIT(KEmergencyNumber112,"112");
_LIT(KEmergencyNumber999,"999");
_LIT(KEmergencyNumber110,"110");
_LIT(KEmergencyNumber118,"118");
_LIT(KEmergencyNumber119,"119");
_LIT(KEmergencyNumber08,"08");
_LIT(KEmergencyNumber000,"000");

	
CCTSYIntegrationTestCallEmergencyBase::CCTSYIntegrationTestCallEmergencyBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iNetworkTestHelper(*this), iPhoneTestHelper(*this), iCallControlTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestCallEmergencyBase::~CCTSYIntegrationTestCallEmergencyBase()
/*
 * Destructor
 */
	{
	}




void CCTSYIntegrationTestCallEmergencyBase::MakeEmergencyCallL(RMobileCall& aCall, const TDesC& aNumber)
/**
 * Dials emergency call, checks it is successful, check the status of the call and hangs up
 * @param aCall Handle to RMobileCall object
 * @param aNumber number to be dialled.
 * 
 * @return void
 */
    {
	
	DEBUG_PRINTF2(_L("Dialing to emergency number: %S"),&aNumber);
	// Post notifier for RMobileCall::NotifyMobileCallStatusChange
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TExtEtelRequestStatus notifyCallStatusChange(aCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatusChange);
	aCall.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);

	// Dial RMobileCall::DialEmergencyCall with aNumber 
	TExtEtelRequestStatus dialStatus(aCall, EMobileCallDialEmergencyCall);
	CleanupStack::PushL(dialStatus);
	aCall.DialEmergencyCall(dialStatus, aNumber);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeVeryLong), KErrNone,  _L("RMobileCall::DialEmergencyCall timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall returned an error"));
	
	// ===  Check mobile call status ===

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	// Repost notifier
	aCall.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	// Repost notifier
	aCall.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);																	
																		
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(aCall.GetMobileCallStatus(mobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileCallStatus did not return EStatusIdle"));
			
	// Hang up. 
	TCoreEtelRequestStatus<RMobileCall> hangUpStatus (aCall, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	aCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	
	aCall.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);																	
	
	// Pop
	// notifyCallStatusChange
	// dialStatus
	// hangUpStatus	
	CleanupStack::PopAndDestroy(3, &notifyCallStatusChange);
	
}




CCTSYIntegrationTestCallEmergency0001::CCTSYIntegrationTestCallEmergency0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0001::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0001::~CCTSYIntegrationTestCallEmergency0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0001
 * @SYMFssID BA/CTSY/CEMG-0001
 * @SYMTestCaseDesc Dial an emergency call (with international emergency number) using SIM/USIM with no emergency numbers stored on it.

 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::VerifySecurityCode, RMobilePhone::NotifySecurityEvent, RMobilePhone::SetFdnSetting, RCall::HangUp, RMobileCall::GetMobileCallStatus, RMobileCall::DialEmergencyCall, RMobileCall::GetMobileCallInfo
 * @SYMTestExpectedResults Pass - Call to international emergency numbers can be made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify national and international emergency numbers can be dialled with and without FDN active including when phone is in forbidden PLMN.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


    // Check that the phone is registered with the network simulator. 
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 securityCaps;
	CHECK_EQUALS_L(phone.GetSecurityCaps(securityCaps), KErrNone,_L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(securityCaps,RMobilePhone::KCapsAccessPin2 ,KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps did not return KCapsAccessPin2 bits"));

    // get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTestHelper.GetNetworkPasswordsSectionL(phone, networkSection);

	// Get the password from the ini file
	TPtrC pin2; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin2, pin2) != EFalse, _L("GetStringFromConfig returned with an error"));

	// Lock PIN2 with RMobilePhone::SetLockSetting with aLock=ELockPin2, aSetting=ELockSetEnabled
	// $CTSYProblem RMobilePhone::SetLockSetting returns KErrNotSupported from LTSY.
	iPhoneTestHelper.ChangePhoneLockSettingL(phone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetEnabled, pin2, KErrNotSupported);

	// Ensure RMobilePhone::GetLockInfo aLock=ELockPin2 returns TMobilePhoneLockInfoV1::iStatus=EStatusLocked and TMobilePhoneLockInfoV1::iSetting = ELockSetEnabled 
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);
	TExtEtelRequestStatus getLockInfoStatus(phone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
	phone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockPin2, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	CHECK_EQUALS_L(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned an error"));
	CHECK_EQUALS_L(lockInfo.iStatus, RMobilePhone::EStatusLocked, _L("RMobilePhone::GetLockInfo did not returned iStatus = RMobilePhone::EStatusLocked"));
	CHECK_EQUALS_L(lockInfo.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo did not returned iSetting = RMobilePhone::ELockSetEnabled"));

	// Ensure the FDN is deactivated (RMobilePhone::GetFdnStatus returns EFdnNotActive) 
	RMobilePhone::TMobilePassword pin2Password;
	pin2Password.Copy(pin2);
	RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
	CHECK_EQUALS_L(phone.GetFdnStatus(fdnStatus), KErrNone, _L("RMobilePhone::GetFdnStatus returned an error"));
    
    if(fdnStatus != RMobilePhone::EFdnNotActive)
        {
        CHECK_EQUALS_L(iPhoneTestHelper.SetPhoneFdnSettingL(phone, pin2Password, RMobilePhone::EFdnSetOff), KErrNone,_L("RMobilePhone::SetFdnSetting failed"));
        CHECK_EQUALS_L(phone.GetFdnStatus(fdnStatus), KErrNone, _L("RMobilePhone::GetFdnStatus returned an error"));
        }

	CHECK_EQUALS_L(fdnStatus, RMobilePhone::EFdnNotActive, _L("RMobilePhone::GetFdnStatus did not return EfdnNotActive."));

    // Get server
    RTelServer& telServer = iEtelSessionMgr.GetServerL(KMainServer);

	// Get voice line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Dial an emergency call 112
    DoTest0001L(phone, mobileCall, KEmergencyNumber112, pin2Password);
	// Repeat whole test dialling 999 instead of 112.
    DoTest0001L(phone, mobileCall, KEmergencyNumber999, pin2Password);

	// Repeat whole test with both numbers with simulator simulating phone not camped on suitable cell (forbidden PLMN). 
	TExtEtelRequestStatus reqStatus(phone, EMobilePhoneNotifyNetworkRegistrationStatusChange );
	CleanupStack::PushL(reqStatus);
	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus;
	phone.NotifyNetworkRegistrationStatusChange(reqStatus, registrationStatus);
	DisplayUserInteractionPromptL(_L("Make sure phone is not camped on suitable cell (forbidden PLMN)"), ETimeLong);
	TInt err = WaitForRequestWithTimeOut(reqStatus,ETimeVeryLong);
	while (	(err == KErrNone) && (reqStatus.Int() == KErrNone) && (registrationStatus == RMobilePhone::ENotRegisteredSearching ) )
		{
		phone.NotifyNetworkRegistrationStatusChange(reqStatus, registrationStatus);
		err = WaitForRequestWithTimeOut(reqStatus,ETimeVeryLong);
		}
	DEBUG_PRINTF2(_L(" Network  registration status  = %d"), registrationStatus);
    DoTest0001L(phone, mobileCall, KEmergencyNumber112, pin2Password);
    DoTest0001L(phone, mobileCall, KEmergencyNumber999, pin2Password);

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	reqStatus
	//	getLockInfoStatus
	CleanupStack::PopAndDestroy(2,&getLockInfoStatus);
   
   return TestStepResult();
    }


void CCTSYIntegrationTestCallEmergency0001::DoTest0001L(RMobilePhone &aPhone,
                                                        RMobileCall& aCall,
                                                        const TDesC& aNumber,
                                                        RMobilePhone::TMobilePassword& aPIN2Password)
/**
 * Function for dialling an emergency call. This is a helper method for repeating emergency calls in test 0001.
 * 
 * @param aPhone a reference to RMobilePhone object
 * @param aCall a reference to RMobileCall object
 * @param aNumber TDesC holding the emergecy number
 * @param aPIN2Password TMobilePassword holding the PIN2 password

 * @return void
 */
	{
	// Set priority client with RTelServer::SetPriorityClientV2
	// $CTSYProblem Can't set client priority, ETEL returns KErrPermissionDenied  

	// Post Notifier for RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(aCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	aCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);

	// Dial RMobileCall::DialEmergencyCall with aNumber 
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"),&aNumber);
	TExtEtelRequestStatus dialStatus(aCall, EMobileCallDialEmergencyCall);
	CleanupStack::PushL(dialStatus);
	aCall.DialEmergencyCall(dialStatus, aNumber); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::DialEmergencyCall timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall returned with an error"));

	// ===  Check mobile call status ===

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	aCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	aCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;															
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(aCall.GetMobileCallStatus(mobileCallStatus), KErrNone,  _L("RMobileCall::GetMobileStatus returned an error."));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileStatus did not return EStatusConnected."));

	// ===  Check call info ===

	// Check RMobileCall::GetMobileCallInfo returns valid call name.
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg mobileCallInfoPckg(mobileCallInfo);
	ASSERT_EQUALS(aCall.GetMobileCallInfo(mobileCallInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iCallName.Length() > 0, _L("RMobileCall::GetMobileCallInfo returned an invalid call name"));

	// Check RMobileCall::GetMobileCallInfo returns valid line name.
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() > 0, _L("RMobileCall::GetMobileCallInfo returned an invalid line name"));

	// Check RMobileCall::GetMobileCallInfo returns iDialledParty = 112
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10064 );
	ASSERT_EQUALS_DES16(mobileCallInfo.iDialledParty.iTelNumber, aNumber, _L("RMobileCall::GetMobileCallInfo returned an invalid number"));

	// Check RMobileCall::GetMobileCallInfo returns iEmergency = ETrue
	ASSERT_TRUE(mobileCallInfo.iEmergency, _L("RMobileCall::GetMobileCallInfo did not return ETrue for iEmergency"));

	// Hang up with RCall::HangUp 
	TCoreEtelRequestStatus<RMobileCall> hangUpStatus (aCall, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	aCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	

	// ===  Activate the FDN ===

	// Call RMobilePhone::SetFdnSetting with RMobilePhone::EFdnSetOn 
    TInt err = iPhoneTestHelper.SetPhoneFdnSettingL(aPhone, aPIN2Password, RMobilePhone::EFdnSetOn);

	// Check RMobilePhone::SetFdnSetting completes with KErrNone
    ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetFdnSetting returned an error"));

    // Repost notifier for RMobileCall::NotifyMobileCallStatusChange
	aCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);

	// Dial RMobileCall::DialEmergencyCall with aNumber=112 
	aCall.DialEmergencyCall(dialStatus, aNumber); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::DialEmergencyCall timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall returned with an error"));

	// ===  Check mobile call status ===

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	aCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	aCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;															
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(aCall.GetMobileCallStatus(mobileCallStatus), KErrNone,  _L("RMobileCall::GetMobileStatus returned an error."));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileStatus did not return status EStatusConnected."));

	// Check RMobileCall::GetMobileCallInfo returns iValid of 0.
	ASSERT_EQUALS(aCall.GetMobileCallInfo(mobileCallInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10064 );
	ASSERT_TRUE(mobileCallInfo.iValid == 0, _L("RMobileCall::GetMobileCallInfo returned unexpected flags in iValid"));

	// Check RMobileCall::GetMobileCallInfo returns valid call name.
	ASSERT_TRUE(mobileCallInfo.iCallName.Length() > 0, _L("RMobileCall::GetMobileCallInfo returned an invalid call name"));

	// Check RMobileCall::GetMobileCallInfo returns valid line name.
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() > 0, _L("RMobileCall::GetMobileCallInfo returned an invalid line name"));

	// Check RMobileCall::GetMobileCallInfo returns iDialledParty = 112
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10064 );
	ASSERT_EQUALS_DES16(mobileCallInfo.iDialledParty.iTelNumber, aNumber, _L("RMobileCall::GetMobileCallInfo returned an invalid number"));

	// Check RMobileCall::GetMobileCallInfo returns iEmergency = ETrue
	ASSERT_TRUE(mobileCallInfo.iEmergency, _L("RMobileCall::GetMobileCallInfo did not return ETrue for iEmergency"));

	// Hang up with RCall::HangUp 
	aCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	

	// Pop
    // notifyMobileCallStatusChangeStatus
    // dialStatus
    // hangUpStatus
   CleanupStack::PopAndDestroy(3, &notifyMobileCallStatusChangeStatus);
    
	}


TPtrC CCTSYIntegrationTestCallEmergency0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0001");
	}



CCTSYIntegrationTestCallEmergency0002::CCTSYIntegrationTestCallEmergency0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0002::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0002::~CCTSYIntegrationTestCallEmergency0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0002
 * @SYMFssID BA/CTSY/CEMG-0002
 * @SYMTestCaseDesc Dial an emergency call using SIM/USIM with no emergency numbers stored on it.

 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::VerifySecurityCode, RMobilePhone::NotifySecurityEvent, RMobilePhone::SetFdnSetting, RCall::HangUp, RMobileCall::GetMobileCallStatus, RMobileCall::DialEmergencyCall, RTelServer::SetPriorityClient
 * @SYMTestExpectedResults Pass - Emergency call can be made to only when FDN is not active and phone is camped on a valid cell. 
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify numbers can be dialled when FDN is inactive but not when active.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 

	// Lock PIN2 with RMobilePhone::SetLockSetting with aLock=ELockPin2, aSetting=ELockSetEnabled 

	// Enusre RMobilePhone::GetLockInfo aLock=ELockPin2 returns TMobilePhoneLockInfoV1::iStatus=EStatusLocked and TMobilePhoneLockInfoV1::iSetting = ELockSetEnabled 

	// Ensure the FDN is deactivated (RMobilePhone::GetFdnStatus returns EFdnNotActive) 

	// Get voice line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Dial RMobileCall::DialEmergencyCall with aNumber=08 

	// Simulator should let call ring then answer the call 

	// ===  Check mobile call status ===

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.

	// Hang up with RCall::HangUp 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle

	// ===  Activate the FDN ===

	// Call RMobilePhone::SetFdnSetting with RMobilePhone::EFdnSetOn 

	// Check RMobilePhone::NotifySecurityEvent completes with RMobilePhone::EPin2Required

	// Verify PIN2 security code with RMobilePhone::VerifySecurityCode with RMobilePhone::ESecurityCodePin2 

	// Check RMobilePhone::NotifySecurityEvent completes with RMobilePhone::EPin2Verified

	// Check RMobilePhone::SetFdnSetting completes with KErrNone

	// Dial RMobileCall::DialEmergencyCall with aNumber=08 

	// Check dialling fails.

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusIdle.

	// Repeat whole test dialling 000, then 110, then 118, then 119 instead of 08. 

	// Repeat whole test with both numbers with simulator simulating phone not camped on suitable cell (forbidden PLMN). 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallEmergency0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0002");
	}



CCTSYIntegrationTestCallEmergency0003::CCTSYIntegrationTestCallEmergency0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0003::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0003::~CCTSYIntegrationTestCallEmergency0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0003
 * @SYMFssID BA/CTSY/CEMG-0003
 * @SYMTestCaseDesc Dial an emergency call to number not stored in emergency phone book on SIM.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileCall::GetMobileCallStatus
 * @SYMTestExpectedResults Pass - Emergency call to emergency numbers on SIM can be made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call can be dialled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Check that the phone is registered with the network simulator. 
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	
	// Get voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& voiceCall = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notification for the RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(voiceCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	voiceCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);

	// Dial a normal number that answers using RMobileCall::DialEmergencyCall 
    TPtrC number;
    ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig failed to read KIniVoiceNumberInternational from INI file."));
    TExtEtelRequestStatus dialStatus(voiceCall, EMobileCallDialEmergencyCall);
    CleanupStack::PushL(dialStatus);
    voiceCall.DialEmergencyCall(dialStatus, number);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone,  _L("RMobileCall::DialEmergencyCall timed-out."));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall returned error."));
	
	// ===  Check mobile call status ===
   
   	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
   	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(voiceCall,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
																	
   	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	voiceCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);	
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(voiceCall,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
   	
   	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	voiceCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);	
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(voiceCall,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
    expectedMobileCallStatus = RMobileCall::EStatusConnected;
	ASSERT_EQUALS(voiceCall.GetMobileCallStatus(mobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned an Error."));
	ASSERT_EQUALS(mobileCallStatus, expectedMobileCallStatus, _L("RMobileCall::GetMobileCallStatus did not set the status to EStatusConnected."));

	// Hang up with RCall::HangUp
	TCoreEtelRequestStatus<RCall> hangUpStatus (voiceCall, &RCall::HangUpCancel);
	CleanupStack::PushL(hangUpStatus);
	voiceCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out."));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::CallHangUp returned an error."));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
    voiceCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(voiceCall,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyMobileCallStatusChangeStatus
	// dialStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(3, &notifyMobileCallStatusChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallEmergency0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0003");
	}



CCTSYIntegrationTestCallEmergency0004::CCTSYIntegrationTestCallEmergency0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0004::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0004::~CCTSYIntegrationTestCallEmergency0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0004
 * @SYMFssID BA/CTSY/CEMG-0004
 * @SYMTestCaseDesc Dial an emergency call to number not stored in emergency phone book on SIM when FDN active.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Emergency call cannot be made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call cannot be made.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Check that the phone is registered with the network simulator. 
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	
    // Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
    TUint32 securityCaps;
    CHECK_EQUALS_L(phone.GetSecurityCaps(securityCaps), KErrNone,_L("RMobilePhone::GetSecurityCaps returned with an error"));
    CHECK_BITS_SET_L(securityCaps,RMobilePhone::KCapsAccessPin2 ,KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps did not return KCapsAccessPin2 bits"));

   	// Get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTestHelper.GetNetworkPasswordsSectionL(phone, networkSection);

	// Lock PIN2 with RMobilePhone::SetLockSetting with aLock=ELockPin2, aSetting=ELockSetEnabled 
	TPtrC pin2; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin2, pin2) != EFalse, _L("GetStringFromConfig failed to read KIniPin2 from INI file"));
	RMobilePhone::TMobilePassword pin2Password;
	pin2Password.Copy(pin2);
	// $CTSYProblem RMobilePhone::SetLockSetting returns KErrNotSupported from LTSY.
    iPhoneTestHelper.ChangePhoneLockSettingL(phone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetEnabled, pin2, KErrNotSupported);

	// Ensure RMobilePhone::GetLockInfo aLock=ELockPin2 returns TMobilePhoneLockInfoV1::iStatus=EStatusLocked and TMobilePhoneLockInfoV1::iSetting = ELockSetEnabled 
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);
	TExtEtelRequestStatus getLockInfoStatus(phone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
	phone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockPin2, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	CHECK_EQUALS_L(getLockInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetLockInfo returned an error"));
	CHECK_EQUALS_L(lockInfo.iStatus, RMobilePhone::EStatusLocked, _L("RMobilePhone::GetLockInfo did not returned iStatus = RMobilePhone::EStatusLocked"));
	CHECK_EQUALS_L(lockInfo.iSetting, RMobilePhone::ELockSetEnabled, _L("RMobilePhone::GetLockInfo did not returned iSetting = RMobilePhone::ELockSetEnabled"));

	// Ensure the FDN is active (RMobilePhone::GetFdnStatus returns EFdnActive)
    RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
	CHECK_EQUALS_L(phone.GetFdnStatus(fdnStatus), KErrNone, _L("RMobilePhone::GetFdnStatus returned an error"));
	
	if(fdnStatus != RMobilePhone::EFdnActive)
        {
        CHECK_EQUALS_L(iPhoneTestHelper.SetPhoneFdnSettingL(phone, pin2Password, RMobilePhone::EFdnSetOn), KErrNone,_L("RMobilePhone::SetFdnSetting failed"));
        CHECK_EQUALS_L(phone.GetFdnStatus(fdnStatus), KErrNone, _L("RMobilePhone::GetFdnStatus returned an error"));
        }
        
	CHECK_EQUALS_L(fdnStatus, RMobilePhone::EFdnActive, _L("RMobilePhone::GetFdnStatus did not return RMobilePhone::EFdnActive"));

	// Get voice line 1. 
    RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
    RMobileCall& voiceCall = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
    // Dial a normal number that answers using RMobileCall::DialEmergencyCall
    TPtrC number;
    ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig failed to read KIniVoiceNumber1 from INI file"));
    TExtEtelRequestStatus dialStatus(voiceCall, EMobileCallDialEmergencyCall);
    CleanupStack::PushL(dialStatus);
    voiceCall.DialEmergencyCall(dialStatus, number);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone,  _L("RMobileCall::DialEmergencyCall timed-out"));

	// Check no error is returned.
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall return error."));
	
	//
	// TEST END
	//

    StartCleanup();
    
    voiceCall.HangUp();
	
	// Pop:
	//	dialStatus
	//	getLockInfoStatus
	CleanupStack::PopAndDestroy(2,&getLockInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallEmergency0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0004");
	}



CCTSYIntegrationTestCallEmergency0005::CCTSYIntegrationTestCallEmergency0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0005::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0005::~CCTSYIntegrationTestCallEmergency0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0005
 * @SYMFssID BA/CTSY/CEMG-0005
 * @SYMTestCaseDesc Dial an emergency call to number not stored in emergency phone book on SIM when phone not IMSI attached.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetNetworkRegistrationStatus
 * @SYMTestExpectedResults Pass - Emergency call cannot be made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify call cannot be made.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

    // Check that the phone is registered with the network simulator. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Get voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Simulate no signal.
	DisplayUserInteractionPromptL(_L("Simulate no signal"), ETimeMedium);
 
	// RMobilePhone::GetNetworkRegistrationStatus returns ENotRegisteredNoService
	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus;
	TExtEtelRequestStatus getNWRegistrationStatusStatus(phone,EMobilePhoneGetNetworkRegistrationStatus);
	CleanupStack::PushL(getNWRegistrationStatusStatus);
	phone.GetNetworkRegistrationStatus(getNWRegistrationStatusStatus,registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNWRegistrationStatusStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus timed out"));	
	ASSERT_EQUALS(getNWRegistrationStatusStatus.Int(), KErrNone,  _L("RMobilePhone::GetNetworkRegistrationStatus returned an error"));
	TExtEtelRequestStatus reqStatus(phone, EMobilePhoneNotifyNetworkRegistrationStatusChange );
	CleanupStack::PushL(reqStatus);
	if(registrationStatus!= RMobilePhone::ENotRegisteredNoService)
		{
		phone.NotifyNetworkRegistrationStatusChange(reqStatus, registrationStatus);
		iNetworkTestHelper.WaitForMobilePhoneNotifyNetworkRegistrationStatusChange( phone, 
										reqStatus, registrationStatus,
										RMobilePhone::ENotRegisteredNoService, KErrNone );
		
		}
	ASSERT_EQUALS(registrationStatus, RMobilePhone::ENotRegisteredNoService, _L("RMobilePhone::GetNetworkRegistrationStatus did not return ENotRegisteredNoService"));
	TEST_CHECK_POINT_L(_L("We might have a signal, could not make an emergency call"));
 
	// Dial RMobileCall::DialEmergencyCall with aNumber=999 
    TExtEtelRequestStatus dialStatus(call1, EMobileCallDialEmergencyCall);
    CleanupStack::PushL(dialStatus);
    call1.DialEmergencyCall(dialStatus, KEmergencyNumber999);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeVeryLong), KErrNone,  _L("RMobileCall::DialEmergencyCall timed-out"));

	// Verify emergency call cannot be made. 
	ASSERT_TRUE(dialStatus.Int() != KErrNone, _L("RMobileCall::DialEmergencyCall did not return an error as expected."));
	
	//
	// TEST END
	//

    StartCleanup();
    
    // Pop
    // getNWRegistrationStatusStatus
    // reqStatus
    // dialStatus 	
	CleanupStack::PopAndDestroy(3, &getNWRegistrationStatusStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallEmergency0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0005");
	}



CCTSYIntegrationTestCallEmergency0006::CCTSYIntegrationTestCallEmergency0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0006::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0006::~CCTSYIntegrationTestCallEmergency0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0006
 * @SYMFssID BA/CTSY/CEMG-0006
 * @SYMTestCaseDesc Dialling emergency calls with no SIM.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileCall::GetMobileCallStatus, RCall::HangUp
 * @SYMTestExpectedResults Pass - Emergency call can be made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify emergency call can be made.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    
	// Get voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Dial RMobileCall::DialEmergencyCall with aNumber=112
	MakeEmergencyCallL(call1, KEmergencyNumber112);

	// Repeat test with aNumber = 08, 000, 110, 118, 119, 999.
	MakeEmergencyCallL(call1, KEmergencyNumber08);
	MakeEmergencyCallL(call1, KEmergencyNumber000);
	MakeEmergencyCallL(call1, KEmergencyNumber110);
	MakeEmergencyCallL(call1, KEmergencyNumber118);
	MakeEmergencyCallL(call1, KEmergencyNumber119);
	MakeEmergencyCallL(call1, KEmergencyNumber999);

	//
	// TEST END
	//

    StartCleanup();
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallEmergency0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0006");
	}



CCTSYIntegrationTestCallEmergency0007::CCTSYIntegrationTestCallEmergency0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0007::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0007::~CCTSYIntegrationTestCallEmergency0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0007
 * @SYMFssID BA/CTSY/CEMG-0007
 * @SYMTestCaseDesc Dial emergency call can be made when PIN1 blocked
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileCall::GetMobileCallStatus, RCall::HangUp
 * @SYMTestExpectedResults Pass - Emergency call can be made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify emergency call can be made.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Check that the phone is registered with the network simulator. 
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin1 | KCapsLockICC
	TUint32 securityCaps;
	TUint32 wantedSecurityCaps = RMobilePhone::KCapsAccessPin1 | RMobilePhone::KCapsLockICC;
    CHECK_EQUALS_L(phone.GetSecurityCaps(securityCaps), KErrNone,_L("RMobilePhone::GetSecurityCaps returned with an error"));
    CHECK_BITS_SET_L(securityCaps, wantedSecurityCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps did not return caps in set of KCapsAccessPin1 | KCapsLockICC"));

	// get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTestHelper.GetNetworkPasswordsSectionL(phone, networkSection);

	// Get the password from the ini file
	TPtrC pin1; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin1, pin1) != EFalse, 
			_L("GetStringFromConfig returned with an error"));

	// Enable PIN1 with RMobilePhone::SetLockSetting with aLock=ELockICC, aSetting=ELockSetEnabled
    iPhoneTestHelper.ChangePhoneLockSettingL(phone, RMobilePhone::ELockICC,RMobilePhone::ELockSetEnabled, pin1);

	// Block PIN1 by calling RMobilePhone::VerifySecurityCode with aType=ESecurityCodePin1, aCode = wrong PIN1 until RMobilePhone::GetLockInfo with aLock=ELockICC returns TMobilePhoneSecurityCodeInfoV5::iRemainingEntryAttempts=0 then calling RMobilePhone::VerifySecurityCode one more time with aCode=wrong PIN1.
    iPhoneTestHelper.BlockPinL(phone, RMobilePhone::ESecurityCodePin1);

	// Check RMobilePhone::GetLockInfo returns error
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);
	TExtEtelRequestStatus getLockInfoStatus(phone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
	phone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockICC, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	CHECK_TRUE_L(getLockInfoStatus.Int() != KErrNone, _L("RMobilePhone::GetLockInfo did not returned with an error"));

	// Get voice line 1. 
    RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
    RMobileCall& voiceCall = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Dial RMobileCall::DialEmergencyCall with aNumber=112
	// $CTSYProblem Can't make ergency call when phone is PIN1 blocked, LTSY returns an error 
    CHECK_TRUE_L(EFalse,_L("$CTSYProblem Can't make ergency call when phone is PIN1 blocked, LTSY returns an error"));
	MakeEmergencyCallL(voiceCall, KEmergencyNumber112);

	// Repeat test with aNumber = 08, 000, 110, 118, 119, 999. 
	MakeEmergencyCallL(voiceCall, KEmergencyNumber08);
	MakeEmergencyCallL(voiceCall, KEmergencyNumber000);
	MakeEmergencyCallL(voiceCall, KEmergencyNumber110);
	MakeEmergencyCallL(voiceCall, KEmergencyNumber118);
	MakeEmergencyCallL(voiceCall, KEmergencyNumber119);
	MakeEmergencyCallL(voiceCall, KEmergencyNumber999);

	//
	// TEST END
	//

    StartCleanup();
    
    iPhoneTestHelper.ChangePhoneLockSettingL(phone, RMobilePhone::ELockICC, RMobilePhone::ELockSetDisabled, pin1);
	
	// Pop
	// getLockInfoStatus
	CleanupStack::PopAndDestroy(1, &getLockInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallEmergency0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0007");
	}



CCTSYIntegrationTestCallEmergency0008::CCTSYIntegrationTestCallEmergency0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0008::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0008::~CCTSYIntegrationTestCallEmergency0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0008
 * @SYMFssID BA/CTSY/CEMG-0008
 * @SYMTestCaseDesc Dial emergency call can be made (when PIN2 blocked).

 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileCall::GetMobileCallStatus, RCall::HangUp
 * @SYMTestExpectedResults Pass - Emergency call can be made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify emergency call can be made.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
    // Check that the phone is registered with the network simulator. 
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

	// ===  Block PIN2 ===

	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 securityCaps;
	CHECK_EQUALS_L(phone.GetSecurityCaps(securityCaps), KErrNone,_L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(securityCaps,RMobilePhone::KCapsAccessPin2 ,KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned did not return KCapsAccessPin2 bits"));

	// Get network operator information
	const TDesC* networkSection = NULL;
	iNetworkTestHelper.GetNetworkPasswordsSectionL(phone, networkSection);

	// Get the password from the ini file
	TPtrC pin2; 
	CHECK_TRUE_L(GetStringFromConfig(*networkSection, KIniPin2, pin2) != EFalse, _L("GetStringFromConfig returned with an error"));

	// Enable PIN2 with RMobilePhone::SetLockSetting with aLock=ELockPin2, aSetting=ELockSetEnabled 
	// $CTSYProblem RMobilePhone::SetLockSetting returns KErrNotSupported from LTSY.
	iPhoneTestHelper.ChangePhoneLockSettingL(phone, RMobilePhone::ELockPin2, RMobilePhone::ELockSetEnabled, pin2, KErrNotSupported);

	// Block PIN2 by calling RMobilePhone::VerifySecurityCode with aType=ESecurityCodePin2, aCode = wrong PIN2 
	// until RMobilePhone::GetLockInfo with aLock=ELockPin2 returns TMobilePhoneSecurityCodeInfoV5::iRemainingEntryAttempts=0 
	// then calling RMobilePhone::VerifySecurityCode one more time with aCode=wrong PIN2. 
	iPhoneTestHelper.BlockPinL(phone, RMobilePhone::ESecurityCodePin2);

	// Check RMobilePhone::GetLockInfo with aLock=ELockPin2 returns iSetting=ELockSetEnabled and iStatus=EStatusBlocked
	// $CTSYProblem RMobilePhone::GetLockInfo returns KErrGsm0707SIMPuk2Required from LTSY when the phone is locked.
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg infoPckg(lockInfo);
	TExtEtelRequestStatus getLockInfoStatus(phone, EMobilePhoneGetLockInfo);
	CleanupStack::PushL(getLockInfoStatus);
	phone.GetLockInfo(getLockInfoStatus, RMobilePhone::ELockPin2, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getLockInfoStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetLockInfo timed out"));
	CHECK_EQUALS_L(getLockInfoStatus.Int(), KErrGsm0707SIMPuk2Required, _L("RMobilePhone::GetLockInfo returned an error"));

	// Get voice line 1. 
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//


    // Dian emergency call with aNumber = 112
    MakeEmergencyCallL(call1, KEmergencyNumber112);
    
	// Repeat test with aNumber = 08, 000, 110, 118, 119, 999. 
    MakeEmergencyCallL(call1, KEmergencyNumber08);
    MakeEmergencyCallL(call1, KEmergencyNumber000);
    MakeEmergencyCallL(call1, KEmergencyNumber110);
    MakeEmergencyCallL(call1, KEmergencyNumber118);
    MakeEmergencyCallL(call1, KEmergencyNumber119);
    MakeEmergencyCallL(call1, KEmergencyNumber999);

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	getLockInfoStatus
	CleanupStack::PopAndDestroy(1,&getLockInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallEmergency0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0008");
	}



CCTSYIntegrationTestCallEmergency0009::CCTSYIntegrationTestCallEmergency0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0009::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0009::~CCTSYIntegrationTestCallEmergency0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0009
 * @SYMFssID BA/CTSY/CEMG-0009
 * @SYMTestCaseDesc Dial emergency call can be made when defective SIM is used.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileCall::GetMobileCallStatus, RCall::HangUp
 * @SYMTestExpectedResults Pass - Emergency call can be made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify emergency call can be made.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


    // Check that the phone is registered with the network simulator. 
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Get voice line 1. 
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Insert defective SIM. 

    // Dian emergency call with aNumber = 112
    MakeEmergencyCallL(call1, KEmergencyNumber112);

	// Repeat test with aNumber = 08, 000, 110, 118, 119, 999. 
    MakeEmergencyCallL(call1, KEmergencyNumber08);
    MakeEmergencyCallL(call1, KEmergencyNumber000);
    MakeEmergencyCallL(call1, KEmergencyNumber110);
    MakeEmergencyCallL(call1, KEmergencyNumber118);
    MakeEmergencyCallL(call1, KEmergencyNumber119);
	MakeEmergencyCallL(call1, KEmergencyNumber999);

	//
	// TEST END
	//

    StartCleanup();
	
	return TestStepResult();
	}


TPtrC CCTSYIntegrationTestCallEmergency0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0009");
	}



CCTSYIntegrationTestCallEmergency0010::CCTSYIntegrationTestCallEmergency0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0010::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0010::~CCTSYIntegrationTestCallEmergency0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallEmergency0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0010
 * @SYMFssID BA/CTSY/CEMG-0010
 * @SYMTestCaseDesc Request emergency call when other call active
 * @SYMTestPriority High
 * @SYMTestActions RCall::HangUp, RMobileCall::DialEmergencyCall, RCall::Dial
 * @SYMTestExpectedResults Pass - Emergency call made and other active call goes idle.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify emergency call can be made.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
    
    // Check if we are on a simulated network to run emergency call tests; Leave if not
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

	// Get voice line 1. 
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);
	
	// Get voice line 2. 
	RMobileLine& line2 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 2. 
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2);

	// Get data line 1. 
	RMobileLine& dataLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	// Get call 3.
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KDataLine, KCall3);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Post notifier for RMobileCall::NotifyMobileCallStatusChange for call1
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TExtEtelRequestStatus notifyCallStChange(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStChange);
	call1.NotifyMobileCallStatusChange(notifyCallStChange, mobileCallStatus);
	
	// ===  Dial a number on call 1 ===

	// Dial a number that answers on call 1 with RCall::Dial. 
	TPtrC number;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatus(call1, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, number);	
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	// Repost notifier
	call1.NotifyMobileCallStatusChange(notifyCallStChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	// Repost notifier
	call1.NotifyMobileCallStatusChange(notifyCallStChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);																	
																	
	// Repost notifier for call1
	call1.NotifyMobileCallStatusChange(notifyCallStChange, mobileCallStatus);

	// Post notifier for RMobileCall::NotifyMobileCallStatusChange for call2
	RMobileCall::TMobileCallStatus mobileCall2Status;
	TExtEtelRequestStatus notifyCall2StChange(call2, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCall2StChange);
	call2.NotifyMobileCallStatusChange(notifyCall2StChange, mobileCall2Status);
	
	// ===  Dial an emergency call and check that it gets connected and call 1 is disconnected ===
	// Dial an emergency call on call 2 with RMobileCall::DialEmergencyCall 
	TExtEtelRequestStatus dialStatusCall2(call2, EMobileCallDialEmergencyCall);
	CleanupStack::PushL(dialStatusCall2);
	call2.DialEmergencyCall(dialStatusCall2, KEmergencyNumber112);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10063 );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusCall2, ETimeMedium), KErrNone,  _L("RMobileCall::DialEmergencyCall timed out"));
	ASSERT_EQUALS(dialStatusCall2.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall returned with error"));

	// Check RMobileCall::NotifyMobileCallStatusChange on call 2 completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCall2Status = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCall2StChange,
																		mobileCall2Status,
																		expectedMobileCall2Status,
																		KErrNone);
	// Repost notifier
	call2.NotifyMobileCallStatusChange(notifyCall2StChange, mobileCall2Status);
	expectedMobileCall2Status = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCall2StChange,
																		mobileCall2Status,
																		expectedMobileCall2Status,
																		KErrNone);
	// Repost notifier
	call2.NotifyMobileCallStatusChange(notifyCall2StChange, mobileCall2Status);
	expectedMobileCall2Status = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCall2StChange,
																		mobileCall2Status,
																		expectedMobileCall2Status,
																		KErrNone);	
	
	
	// Check RMobileCall::NotifyMobileCallStatusChange on call 1 completes with EStatusIdle
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
																		
	// Repost notifier
	call2.NotifyMobileCallStatusChange(notifyCall2StChange, mobileCall2Status);
																		
	// Hang up call 2 with RCall::HangUp 
	TCoreEtelRequestStatus<RMobileCall> hangUpStatus (call2, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call2.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	
	
	// Check RMobileCall::NotifyMobileCallStatusChange on call 2 completes with EStatusIdle
	expectedMobileCall2Status = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCall2StChange,
																		mobileCall2Status,
																		expectedMobileCall2Status,
																		KErrNone);
	// ===  Dial a number on call 3 ===
	
	// Post notifier for RMobileCall::NotifyMobileCallStatusChange for call3
	RMobileCall::TMobileCallStatus mobileCallStatusDataCall;
	TExtEtelRequestStatus notifyDataCallStChange(call3, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyDataCallStChange);
	call3.NotifyMobileCallStatusChange(notifyDataCallStChange, mobileCallStatusDataCall);
	
	// Dial a data call to a number that answers. 
	// $CTSYProblem Can't make datacall with LTSY and the Anite SIM 
    CHECK_TRUE_L(EFalse,_L("$CTSYProblem Can't make datacall with LTSY and the Anite SIM"));
	TPtrC dataNumber;
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, dataNumber) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatusDataCall(call3, &RCall::DialCancel);
	CleanupStack::PushL(dialStatusDataCall);
	call3.Dial(dialStatusDataCall, dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusDataCall, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatusDataCall.Int(), KErrNone,  _L("RCall::Dial returned with an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange on call 3 completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedDataCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyDataCallStChange,
																		mobileCallStatusDataCall,
																		expectedDataCallStatus,
																		KErrNone);
	// Repost notifier
	call3.NotifyMobileCallStatusChange(notifyDataCallStChange, mobileCallStatusDataCall);
	expectedDataCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyDataCallStChange,
																		mobileCallStatusDataCall,
																		expectedDataCallStatus,
																		KErrNone);
	// Repost notifier
	call3.NotifyMobileCallStatusChange(notifyDataCallStChange, mobileCallStatusDataCall);
	expectedDataCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyDataCallStChange,
																		mobileCallStatusDataCall,
																		expectedDataCallStatus,
																		KErrNone);
																		
	// ===  Dial an emergency call and check that it gets connected and call 3 is disconnected ===
	
	// Repost notifiers
	call3.NotifyMobileCallStatusChange(notifyDataCallStChange, mobileCallStatusDataCall);
	call2.NotifyMobileCallStatusChange(notifyCall2StChange, mobileCall2Status);
	
	// Dial an emergency call on call 2 with RMobileCall::DialEmergencyCall 
	call2.DialEmergencyCall(dialStatusCall2, KEmergencyNumber112);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusCall2, ETimeMedium), KErrNone,  _L("RMobileCall::DialEmergencyCall timed out"));
	ASSERT_EQUALS(dialStatusCall2.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall  returned with an error"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange on call 2 completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	expectedMobileCall2Status = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCall2StChange,
																		mobileCall2Status,
																		expectedMobileCall2Status,
																		KErrNone);
	// Repost notifier
	call2.NotifyMobileCallStatusChange(notifyCall2StChange, mobileCall2Status);
	expectedMobileCall2Status = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCall2StChange,
																		mobileCall2Status,
																		expectedMobileCall2Status,
																		KErrNone);
	// Repost notifier
	call2.NotifyMobileCallStatusChange(notifyCall2StChange, mobileCall2Status);
	expectedMobileCall2Status = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCall2StChange,
																		mobileCall2Status,
																		expectedMobileCall2Status,
																		KErrNone);

	// Check RMobileCall::NotifyMobileCallStatusChange on call 3 completes with EStatusIdle
	expectedDataCallStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyDataCallStChange,
																		mobileCallStatusDataCall,
																		expectedDataCallStatus,
																		KErrNone);

	// Repost notifier
	call2.NotifyMobileCallStatusChange(notifyCall2StChange, mobileCall2Status);
																		
	// Hang up call 2 with RCall::HangUp 
	call2.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	
	
	// Check RMobileCall::NotifyMobileCallStatusChange on call 2 completes with EStatusIdle
	expectedMobileCall2Status = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCall2StChange,
																		mobileCall2Status,
																		expectedMobileCall2Status,
																		KErrNone);

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyCallStChange
	// dialStatus
	// notifyCall2StChange
	// dialStatusCall2
	// hangUpStatus
	// notifyDataCallStChange
	// dialStatusDataCall
	CleanupStack::PopAndDestroy(7, &notifyCallStChange);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallEmergency0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0010");
	}



CCTSYIntegrationTestCallEmergency0011::CCTSYIntegrationTestCallEmergency0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallEmergencyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallEmergency0011::GetTestStepName());
	}

CCTSYIntegrationTestCallEmergency0011::~CCTSYIntegrationTestCallEmergency0011()
/**
 * Destructor.
 */
	{
	}
TVerdict CCTSYIntegrationTestCallEmergency0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CEMG-0011
 * @SYMFssID BA/CTSY/CEMG-0011
 * @SYMTestCaseDesc Dial Emergency Services at every stage of a conference call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::HangUp, RMobileCall::GetMobileCallInfo, RMobileCall::NotifyMobileCallStatusChange, RCall::NotifyStatusChange, RMobileCall::DialEmergencyCall, RMobileConferenceCall::CreateConference, RMobileConferenceCall::HangUp
 * @SYMTestExpectedResults Pass - Emergency call can be made at each stage of creating a conference.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify emergency call can be made at each stage of the conference.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Check that the phone is registered with the network simulator. 
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	
    // Get voice line 1. 
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// Get Voice line 2. 
	RMobileLine& line2 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 2. 
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2);

	// Get voice line 3. 
	RMobileLine& line3 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 3. 
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Get the conference call subsession. 
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
    
	// ===  Dial an emergency call and hang up ===

    // Post notifier for RMobileCall::NotifyMobileCallStatusChange
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TExtEtelRequestStatus notifyCallStatusChange(call3, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatusChange);
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);

	// Dial an emergency call on call 3 with RMobileCall::DialEmergencyCall to a number that answers
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10063 );
	TExtEtelRequestStatus dialStatusCall3(call3, EMobileCallDialEmergencyCall);
	CleanupStack::PushL(dialStatusCall3);
	call3.DialEmergencyCall(dialStatusCall3, KEmergencyNumber112);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusCall3, ETimeMedium), KErrNone,  _L("RMobileCall::DialEmergencyCall timed out"));
	ASSERT_EQUALS(dialStatusCall3.Int(), KErrNone, _L(" RMobileCall::DialEmergencyCall returned with an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange on call 3 completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
																		
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);	
																																			
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	// Check RMobileCall::GetMobileCallInfo on call 3 returns iEmergency = ETrue
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(mobileCallInfo);
	ASSERT_EQUALS(call3.GetMobileCallInfo(callInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iEmergency, _L("RMobileCall::GetMobileCallInfo did not return ETrue for iEmergency"));
	
	// Hang up call 3 with RCall::HangUp 
	TCoreEtelRequestStatus<RMobileCall> hangUpStatus (call3, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call3.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned with an error"));	

	// ===  Dial first call in conference ===

	// Post notifier for RMobileCall::NotifyMobileCallStatusChange for call1
	RMobileCall::TMobileCallStatus mobileCallStatusCall1;
	TExtEtelRequestStatus notifyCallStatusChangeCall1(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatusChangeCall1);
	call1.NotifyMobileCallStatusChange(notifyCallStatusChangeCall1, mobileCallStatusCall1);
	
	// Dial a number that answers using call 1. 
	TPtrC number;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatusCall1(call1, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatusCall1);
	call1.Dial(dialStatusCall1, number);	
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusCall1, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatusCall1.Int(), KErrNone, _L("RMobileCall::Dial returned with error."));
	
	// Check RCall::NotifyStatusChange for call 1 completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatusCall1 = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStatusChangeCall1,
																		mobileCallStatusCall1,
																		expectedMobileCallStatusCall1,
																		KErrNone);
																		
	call1.NotifyMobileCallStatusChange(notifyCallStatusChangeCall1, mobileCallStatusCall1);
	expectedMobileCallStatusCall1 = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStatusChangeCall1,
																		mobileCallStatusCall1,
																		expectedMobileCallStatusCall1,
																		KErrNone);
																		
	call1.NotifyMobileCallStatusChange(notifyCallStatusChangeCall1, mobileCallStatusCall1);
	expectedMobileCallStatusCall1 = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStatusChangeCall1,
																		mobileCallStatusCall1,
																		expectedMobileCallStatusCall1,
																		KErrNone);

	// ===  Dial an emergency call and hang up ===

	// Repost notifier
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	// Dial an emergency call on call 3 with RMobileCall::DialEmergencyCall to a number that answers
	call3.DialEmergencyCall(dialStatusCall3, KEmergencyNumber112);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusCall3, ETimeLong), KErrNone,  _L("RMobileCall::DialEmergencyCall timed out"));
	ASSERT_EQUALS(dialStatusCall3.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall did not return KErrNone"));

	// Check RMobileCall::NotifyMobileCallStatusChange on call 3 completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	 expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);																		
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	// Check RMobileCall::GetMobileCallInfo on call 3 returns iEmergency = ETrue
	ASSERT_EQUALS(call3.GetMobileCallInfo(callInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iEmergency, _L("RMobileCall::GetMobileCallInfo did not return ETrue for iEmergency"));
	
	// Hang up call 3 with RCall::HangUp 
	call3.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned with an error"));	
	
	// Repost notifier
	call1.NotifyMobileCallStatusChange(notifyCallStatusChangeCall1, mobileCallStatusCall1);
	
	// Hold call 1. 
	TExtEtelRequestStatus holdStatus(call1, EMobileCallHold);
	CleanupStack::PushL(holdStatus);
	call1.Hold(holdStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdStatus, ETimeShort), KErrNone, _L("RMobileCall::Hold timed-out."));
	ASSERT_EQUALS(holdStatus.Int(), KErrNone, _L("RMobileCall::Hold returned with an error."));

	// Check RMobileCall::NotifyMobileCallStatusChange for call 1 completes with EStatusHold
	expectedMobileCallStatusCall1 = RMobileCall::EStatusHold;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyCallStatusChangeCall1,
															mobileCallStatusCall1,
															expectedMobileCallStatusCall1,
															KErrNone);	

	// ===  Dial second call in conference ===

	// Post notifier for RMobileCall::NotifyMobileCallStatusChange for call2
	RMobileCall::TMobileCallStatus mobileCallStatusCall2;
	TExtEtelRequestStatus notifyCallStatusChangeCall2(call2, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatusChangeCall2);
	call2.NotifyMobileCallStatusChange(notifyCallStatusChangeCall2, mobileCallStatusCall2);

	// Dial a number that answers using call 2. 
	TPtrC number2;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber2, number2) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatusCall2(call2, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatusCall2);
	call2.Dial(dialStatusCall2, number2);	
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusCall2, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatusCall2.Int(), KErrNone, _L("RMobileCall::Dial returned with error."));
	
	
	// Check RCall::NotifyStatusChange for call 2 completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatusCall2 = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCallStatusChangeCall2,
																		mobileCallStatusCall2,
																		expectedMobileCallStatusCall2,
																		KErrNone);
																		
	call2.NotifyMobileCallStatusChange(notifyCallStatusChangeCall2, mobileCallStatusCall2);
	expectedMobileCallStatusCall2 = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCallStatusChangeCall2,
																		mobileCallStatusCall2,
																		expectedMobileCallStatusCall2,
																		KErrNone);

	call2.NotifyMobileCallStatusChange(notifyCallStatusChangeCall2, mobileCallStatusCall2);
	expectedMobileCallStatusCall2 = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		notifyCallStatusChangeCall2,
																		mobileCallStatusCall2,
																		expectedMobileCallStatusCall2,
																		KErrNone);
	
	// Create a conference with RMobileConferenceCall::CreateConference 
	TExtEtelRequestStatus createConferenceStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(createConferenceStatus);
	confCall.CreateConference(createConferenceStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(createConferenceStatus, ETimeMedium), KErrNone, _L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L(createConferenceStatus.Int(), KErrNone, _L("RMobileConferenceCall::CreateConference returned with error"));
	
	// ===  Dial an emergency call and hang up ===
	
	// Repost notifier
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);

	// Dial an emergency call on call 3 with RMobileCall::DialEmergencyCall to a number that answers
	call3.DialEmergencyCall(dialStatusCall3, KEmergencyNumber112);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusCall3, ETimeMedium), KErrNone,  _L("RMobileCall::DialEmergencyCall timed out"));
	ASSERT_EQUALS(dialStatusCall3.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall did not return KErrNone"));

	// Check RMobileCall::NotifyMobileCallStatusChange on call 3 completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);																		

	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	// Check RMobileCall::GetMobileCallInfo on call 3 returns iEmergency = ETrue
	ASSERT_EQUALS(call3.GetMobileCallInfo(callInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iEmergency, _L("RMobileCall::GetMobileCallInfo did not return ETrue for iEmergency"));
	
	// Hang up call 3 with RCall::HangUp 
	call3.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned with error"));	

	// ===  Hang up conference ===

	// Request notification for RMobileConferenceCall::NotifyConferenceStatusChange 
	TExtEtelRequestStatus notifyMobileConfStatusChangeStatus (confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyMobileConfStatusChangeStatus);
	RMobileConferenceCall::TMobileConferenceStatus mobileConfStatus;
	confCall.NotifyConferenceStatusChange(notifyMobileConfStatusChangeStatus,mobileConfStatus);
	
	// Hang up conference call with RMobileConferenceCall::HangUp 
	TExtEtelRequestStatus hangUpStatusConfCall (confCall,EMobileConferenceCallHangUp);
	CleanupStack::PushL(hangUpStatusConfCall);		
	confCall.HangUp(hangUpStatusConfCall);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatusConfCall, ETimeMedium), KErrNone, _L("RMobileConferenceCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatusConfCall.Int(), KErrNone,	_L("RMobileConferenceCall::HangUp returned with error"));

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle
	RMobileConferenceCall::TMobileConferenceStatus expectedConfStatus = RMobileConferenceCall::EConferenceIdle;
	iCallControlTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange(confCall,
													 					notifyMobileConfStatusChangeStatus,
													 					mobileConfStatus,
													 					expectedConfStatus,
													 					KErrNone);
	
	// Release conference subsession 
	iEtelSessionMgr.ReleaseConferenceCall(KMainServer,KMainPhone,KMainConferenceCall);

	// ===  Dial an emergency call and hang up ===

	// Repost notifier
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);

	// Dial an emergency call on call 3 with RMobileCall::DialEmergencyCall to a number that answers
	call3.DialEmergencyCall(dialStatusCall3, KEmergencyNumber112);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusCall3, ETimeMedium), KErrNone,  _L("RMobileCall::DialEmergencyCall timed out"));
	ASSERT_EQUALS(dialStatusCall3.Int(), KErrNone, _L("RMobileCall::DialEmergencyCall did not return KErrNone"));

	// Check RMobileCall::NotifyMobileCallStatusChange on call 3 completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
 
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);																		
	
	call3.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call3,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	// Check RMobileCall::GetMobileCallInfo on call 3 returns iEmergency = ETrue
	ASSERT_EQUALS(call3.GetMobileCallInfo(callInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iEmergency, _L("RMobileCall::GetMobileCallInfo did not return ETrue for iEmergency"));
	
	// Hang up call 3 with RCall::HangUp 
	call3.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyCallStatusChange
	// dialStatusCall3
	// hangUpStatus
	// notifyCallStatusChangeCall1
	// dialStatusCall1
	// holdStatus
	// notifyCallStatusChangeCall2
	// dialStatusCall2
	// createConferenceStatus
	// notifyMobileConfStatusChangeStatus
	// hangUpStatusConfCall
	
	CleanupStack::PopAndDestroy(11, &notifyCallStatusChange);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallEmergency0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallEmergency0011");
	}



