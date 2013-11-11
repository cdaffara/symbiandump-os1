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
// Test step definitions for the SignalStrength functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestsignalstrength.h"
#include "cetelsessionmgr.h"


	
CCTSYIntegrationTestSignalStrengthBase::CCTSYIntegrationTestSignalStrengthBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestSignalStrengthBase::~CCTSYIntegrationTestSignalStrengthBase()
/*
 * Destructor
 */
	{
	}

/**************************************************************/	

CCTSYIntegrationTestSignalStrength0001::CCTSYIntegrationTestSignalStrength0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSignalStrengthBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSignalStrength0001::GetTestStepName());
	}

CCTSYIntegrationTestSignalStrength0001::~CCTSYIntegrationTestSignalStrength0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSignalStrength0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SIGS-0001
 * @SYMFssID BA/CTSY/SIGS-0001
 * @SYMTestCaseDesc Get the signal strength
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySignalStrengthChange
 * @SYMTestExpectedResults Pass - Signal strength can be retrieved and notifications are received
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Check signal strength is updated
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	// Ensure phone has RMobilePhone::GetSignalCaps returns caps in set of KCapsGetSignalStrength | KCapsNotifySignalStrengthChange 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	TUint32 caps; 
	TInt err = phone.GetSignalCaps(caps);
	
	CHECK_EQUALS_L( err, KErrNone, _L("RMobilePhone::GetSignalCaps failed."))
	
	TUint neededCaps = 	RMobilePhone::KCapsGetSignalStrength | 
						RMobilePhone::KCapsNotifySignalStrengthChange;	
						
	CHECK_BITS_SET_L( caps, neededCaps, KNoUnwantedBits, _L("RMobilePhone::GetSignalCaps returned wrong mask") )
	
	// Ensure RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork 

	TExtEtelRequestStatus reqNetworkRegStatus(phone, EMobilePhoneGetNetworkRegistrationStatus);
	CleanupStack::PushL(reqNetworkRegStatus);
	
	// We will give the phone some time to give us the expected status
	RMobilePhone::TMobilePhoneRegistrationStatus phoneRegStatus;
	
	phone.GetNetworkRegistrationStatus(reqNetworkRegStatus, phoneRegStatus);
	err = WaitForRequestWithTimeOut(reqNetworkRegStatus, ETimeShort);
	ASSERT_EQUALS( err, KErrNone,_L("RMobilePhone::GetNetworkRegistrationStatus timed out"));
								
	CHECK_EQUALS_L( phoneRegStatus, RMobilePhone::ERegisteredOnHomeNetwork,
			_L("RMobilePhone::GetNetworkRegistrationStatus did not return ERegisteredOnHomeNetwork ") )

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
		
	TInt32 sigStrength=0;
	TInt8 sigBars=0;
	
	TExtEtelRequestStatus reqGetSigStrengthStatus(phone, EMobilePhoneGetSignalStrength);
	CleanupStack::PushL(reqGetSigStrengthStatus);	
	
	phone.GetSignalStrength( reqGetSigStrengthStatus, sigStrength, sigBars );
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqGetSigStrengthStatus, ETimeMedium), KErrNone, 
							_L("RMobilePhone::GetSignalStrength timed out"))
	ASSERT_EQUALS( reqGetSigStrengthStatus.Int(), KErrNone, 
							_L("RMobilePhone::GetSignalStrength returned with an error"))

	// Check RMobilePhone::GetSignalStrength returns aSignalStrength > 0
	ASSERT_TRUE( sigStrength > 0, _L("RMobilePhone::GetSignalStrength - signal strength was <= 0 ") )				
	
	// Check RMobilePhone::GetSignalStrength returns aBar > 0
	ASSERT_TRUE( sigBars > 0,_L("RMobilePhone::GetSignalStrength - bars was <= 0 ") )

	// ===  Listen for 30 seconds for a changes in signal strength. ===
	TExtEtelRequestStatus reqSigStrengthChange( phone, EMobilePhoneNotifySignalStrengthChange );
	CleanupStack::PushL(reqSigStrengthChange);
	
	phone.NotifySignalStrengthChange(reqSigStrengthChange,sigStrength,sigBars);
	err = WaitForRequestWithTimeOut(reqSigStrengthChange, ETimeShort);		

	// Check RMobilePhone::GetSignalStrength returns aSignalStrength > 0
	phone.GetSignalStrength( reqGetSigStrengthStatus, sigStrength, sigBars );
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqGetSigStrengthStatus, ETimeMedium), KErrNone, 
							_L("RMobilePhone::GetSignalStrength timed out"))
	ASSERT_EQUALS( reqGetSigStrengthStatus.Int(), KErrNone, 
							_L("RMobilePhone::GetSignalStrength returned with an error"))

		
	ASSERT_TRUE( sigStrength > 0, _L("RMobilePhone::GetSignalStrength - signal strength was <= 0 ") )				
	
	// Check RMobilePhone::GetSignalStrength returns aBar > 0
	ASSERT_TRUE( sigBars > 0,_L("RMobilePhone::GetSignalStrength - bars was <= 0 ") )

	
	//
	// TEST END
	//

    StartCleanup();
	
    //
    // CleanupStack items
    // reqSigStrengthChange
    // reqGetSigStrengthStatus
    // reqNetworkRegStatus
	CleanupStack::PopAndDestroy(3, &reqNetworkRegStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSignalStrength0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSignalStrength0001");
	}



CCTSYIntegrationTestSignalStrength0002::CCTSYIntegrationTestSignalStrength0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSignalStrengthBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSignalStrength0002::GetTestStepName());
	}

CCTSYIntegrationTestSignalStrength0002::~CCTSYIntegrationTestSignalStrength0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSignalStrength0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SIGS-0002
 * @SYMFssID BA/CTSY/SIGS-0002
 * @SYMTestCaseDesc Get signal caps.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetSignalCaps
 * @SYMTestExpectedResults Pass - Signal caps retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify signal caps are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	//Get signal caps.
	//Verify signal caps are correct.
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	// Check RMobilePhone::GetSignalCaps returns caps in set of 
	// KCapsGetSignalStrength | KCapsNotifySignalStrengthChange
	TUint32 signalCaps;
	ASSERT_EQUALS(mobilePhone.GetSignalCaps(signalCaps), KErrNone, 
					_L("RMobilePhone::GetSignalCaps returned error"))
	TUint32 expectedCaps = 
							  RMobilePhone::KCapsGetSignalStrength
					        | RMobilePhone::KCapsNotifySignalStrengthChange;
	TUint32 unwantedCaps = 0;
					
	ASSERT_BITS_SET(signalCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetSignalCaps returned incorrect caps"))
	
	

	
	//
	// TEST END
	//

    StartCleanup();
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSignalStrength0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSignalStrength0002");
	}



