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
// Test step definitions for the CallWaiting functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestcallwaiting.h"
#include "cctsyinidata.h"
#include <etelmm.h>

CCTSYIntegrationTestCallWaitingBase::CCTSYIntegrationTestCallWaitingBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iCallControlTsyTestHelper(*this), iSupplementalTsyTestHelper(*this), iNetworkTsyTestHelper(*this)
/**
 * Constructor
 */
	{

	}

CCTSYIntegrationTestCallWaitingBase::~CCTSYIntegrationTestCallWaitingBase()
/*
 * Destructor
 */
	{
	}

CCTSYIntegrationTestCallWaiting0001::CCTSYIntegrationTestCallWaiting0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0001::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0001::~CCTSYIntegrationTestCallWaiting0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0001
 * @SYMFssID BA/CTSY/CWAT-0001
 * @SYMTestCaseDesc Activate and deactivate call waiting for all services.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallWaitingStatusChange
 * @SYMTestExpectedResults Pass - Call waiting is activated for all services.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call waiting status change notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus | KCapsNotifyCWStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
					_L("RMobilePhone::GetCallServiceCaps returned with an error"))
	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus | RMobilePhone::KCapsNotifyCWStatus;
	TUint32 unwantedCaps = 0;	
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
	
	// Deactivate all call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=RMobilePhone::EAllServices and aAction=RMobilePhone::EServiceActionDeactivate 
	RMobilePhone::TMobileService mobileService = RMobilePhone::EAllServices;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))					

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//	
	
	// post notification for setCallWaitingStatus
	TExtEtelRequestStatus notifyCallWaitingStatusChangeStatus(mobilePhone,EMobilePhoneNotifyCallWaitingStatusChange);
	CleanupStack::PushL(notifyCallWaitingStatusChangeStatus);	
	RMobilePhone::TMobilePhoneCWInfoEntryV1 mobilePhoneCWInfoEntryV1;
	RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg mobilePhoneCWInfoEntryV1Pckg(mobilePhoneCWInfoEntryV1);	
	mobilePhone.NotifyCallWaitingStatusChange(notifyCallWaitingStatusChangeStatus,mobilePhoneCWInfoEntryV1Pckg);

	// Activate call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EAllServices  and aAction=EServiceActionActivate 
	mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;	
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
			_L("RMobilePhone::SetCallWaitingStatus timed-out"));
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
			_L("RMobilePhone::SetCallWaitingStatus returned with an error"));

	// Check RMobilePhone::NotifyCallWaitingStatusChange completes with iServiceGroup=EAllServices and iStatus=ECallWaitingStatusActive
	TCmpBase<RMobilePhone::TMobilePhoneCWInfoEntryV1> mobilePhoneCWInfoEntryV1CmpBase(mobilePhoneCWInfoEntryV1,mobilePhoneCWInfoEntryV1,*this);
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallWaitingStatusChange(mobilePhone,
						notifyCallWaitingStatusChangeStatus,mobilePhoneCWInfoEntryV1CmpBase, KErrNone);	
	ASSERT_EQUALS(mobilePhoneCWInfoEntryV1.iServiceGroup , mobileService, 
					_L("RMobilePhone::NotifyCallWaitingStatusChange returned with an error"));
	ASSERT_EQUALS(mobilePhoneCWInfoEntryV1.iStatus, RMobilePhone::ECallWaitingStatusActive, 
				_L("RMobilePhone::NotifyCallWaitingStatusChange returned wrong iStatus"));
				
	// Deactivate call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EAllServices  and aAction=EServiceActionDeactivate 
	mobilePhone.NotifyCallWaitingStatusChange(notifyCallWaitingStatusChangeStatus,mobilePhoneCWInfoEntryV1Pckg);
	mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
			_L("RMobilePhone::SetCallWaitingStatus timed-out"));
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
			_L("RMobilePhone::SetCallWaitingStatus returned with an error"));
	
	// Check RMobilePhone::NotifyCallWaitingStatusChange completes with iServiceGroup=EAllServices and iStatus=ECallWaitingStatusNotActive
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallWaitingStatusChange(mobilePhone,notifyCallWaitingStatusChangeStatus,mobilePhoneCWInfoEntryV1CmpBase, KErrNone);	
	ASSERT_EQUALS(mobilePhoneCWInfoEntryV1.iServiceGroup , mobileService, 
					_L("RMobilePhone::NotifyCallWaitingStatusChange returned with an error"));	
	ASSERT_EQUALS(mobilePhoneCWInfoEntryV1.iStatus, RMobilePhone::ECallWaitingStatusNotActive, 
				_L("RMobilePhone::NotifyCallWaitingStatusChange returned wrong iStatus"));

	//
	// TEST END
	//

    StartCleanup();
    
	// 1 mobilePhoneSetCallWaitingStatus
	// 2 notifyCallWaitingStatusChangeStatus
	CleanupStack::PopAndDestroy(2,&mobilePhoneSetCallWaitingStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0001");
	}



CCTSYIntegrationTestCallWaiting0002::CCTSYIntegrationTestCallWaiting0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0002::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0002::~CCTSYIntegrationTestCallWaiting0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0002
 * @SYMFssID BA/CTSY/CWAT-0002
 * @SYMTestCaseDesc Activate and deactivate call waiting for each provisioned basic service
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallWaitingStatusChange
 * @SYMTestExpectedResults Pass - Call waiting is activated for provisioned basic services.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call waiting status change notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus | KCapsNotifyCWStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
					 _L("RMobilePhone::GetCallServiceCaps returned with an error"))
	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus | RMobilePhone::KCapsNotifyCWStatus;
	TUint32 unwantedCaps = 0;	
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Deactivate all call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=RMobilePhone::EAllServices and aAction=RMobilePhone::EServiceActionDeactivate
	RMobilePhone::TMobileService mobileService = RMobilePhone::EAllServices;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"));
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Repeat whole test with aServiceGroup= EVoiceService,ECircuitDataService, EFaxService, ETelephony 
	
	TUint32 count = 4;
	RArray<RMobilePhone::TMobileService> mobileServiceList;
	ASSERT_EQUALS(mobileServiceList.Append(RMobilePhone::EVoiceService),
			KErrNone,_L("RArray::Append returned with an error"));
	ASSERT_EQUALS(mobileServiceList.Append(RMobilePhone::ECircuitDataService),
		KErrNone,_L("RArray::Append returned with an error"));
	ASSERT_EQUALS(mobileServiceList.Append(RMobilePhone::EFaxService),
		KErrNone,_L("RArray::Append returned with an error"));
	ASSERT_EQUALS(mobileServiceList.Append(RMobilePhone::ETelephony),
		KErrNone,_L("RArray::Append returned with an error"));

	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	err = iNetworkTsyTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));

	switch ( operatorName )
		{
		case EOperatorO2: 
			count = 1; //O2 supports only speech
			break;
		case EOperatorVodafone:
		case EOperatorNTN:
		case EOperatorOrange:
		case EOperatorTMobile: 
			count = 4; //vodafone, orange and tmobile support speech, voice and data
			break;
		case EOperatorUnknown:
		default:
			ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
			TEST_CHECK_POINT_L(_L("Failed to identify current network.")); 
			break;
		}

	
	
	for(TInt i = 0; i < count; i++)
		{	
		DEBUG_PRINTF2(_L("loop iteration for service : %d"),mobileServiceList[i]);
		// post notification for setCallWaiting status
		TExtEtelRequestStatus notifyCallWaitingStatusChangeStatus(mobilePhone,EMobilePhoneNotifyCallWaitingStatusChange);
		CleanupStack::PushL(notifyCallWaitingStatusChangeStatus);		
		RMobilePhone::TMobilePhoneCWInfoEntryV1 mobilePhoneCWInfoEntryV1;
		RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg mobilePhoneCWInfoEntryV1Pckg(mobilePhoneCWInfoEntryV1);
		mobilePhone.NotifyCallWaitingStatusChange(notifyCallWaitingStatusChangeStatus,mobilePhoneCWInfoEntryV1Pckg);
		
		// Activate call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=<current service being tested>  and aAction=EServiceActionActivate 		
		mobileService = mobileServiceList[i];
		mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;		
		mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeLong), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))
		ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))	

		// Check RMobilePhone::NotifyCallWaitingStatusChange completes with iServiceGroup=<current service being tested> and iStatus=ECallWaitingStatusActive
		TCmpBase<RMobilePhone::TMobilePhoneCWInfoEntryV1> mobilePhoneCWInfoEntryV1CmpBase(mobilePhoneCWInfoEntryV1,
															mobilePhoneCWInfoEntryV1,*this);
		iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallWaitingStatusChange(mobilePhone,
						notifyCallWaitingStatusChangeStatus,mobilePhoneCWInfoEntryV1CmpBase, KErrNone);		
		ASSERT_EQUALS(mobilePhoneCWInfoEntryV1.iServiceGroup , mobileService, 
						_L("RMobilePhone::NotifyCallWaitingStatusChange returned with an error"))	
		ASSERT_EQUALS(mobilePhoneCWInfoEntryV1.iStatus, RMobilePhone::ECallWaitingStatusActive, 
					_L("RMobilePhone::NotifyCallWaitingStatusChange returned wrong iStatus"))
					
		// Deactivate call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=<current service being tested>  and aAction=EServiceActionDeactivate
		mobilePhone.NotifyCallWaitingStatusChange(notifyCallWaitingStatusChangeStatus,mobilePhoneCWInfoEntryV1Pckg);		
		mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
		mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);		
		ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))		
		ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))	
				
		// Check RMobilePhone::NotifyCallWaitingStatusChange completes with iServiceGroup=<current service being tested> and iStatus=ECallWaitingStatusNotActive
		iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallWaitingStatusChange(mobilePhone,notifyCallWaitingStatusChangeStatus,mobilePhoneCWInfoEntryV1CmpBase, KErrNone);		
		ASSERT_EQUALS(mobilePhoneCWInfoEntryV1.iServiceGroup , mobileService, 
						_L("RMobilePhone::NotifyCallWaitingStatusChange returned with an error"))			
		ASSERT_EQUALS(mobilePhoneCWInfoEntryV1.iStatus, RMobilePhone::ECallWaitingStatusNotActive, 
					_L("RMobilePhone::NotifyCallWaitingStatusChange returned with an error"))
					
		CleanupStack::PopAndDestroy(&notifyCallWaitingStatusChangeStatus);					
		}
	
	//
	// TEST END
	//

    StartCleanup();
	CleanupStack::PopAndDestroy(&mobilePhoneSetCallWaitingStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0002");
	}



CCTSYIntegrationTestCallWaiting0003::CCTSYIntegrationTestCallWaiting0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0003::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0003::~CCTSYIntegrationTestCallWaiting0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0003
 * @SYMFssID BA/CTSY/CWAT-0003
 * @SYMTestCaseDesc Attempt to activate and deactivate Call Waiting when it is not provisioned on the SIM.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Call waiting cannot be activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Activate call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionActivate 

	// Check error is returned.

	// Deactivate call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionDeactivate 

	// Check error is returned.

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0003");
	}



CCTSYIntegrationTestCallWaiting0004::CCTSYIntegrationTestCallWaiting0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0004::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0004::~CCTSYIntegrationTestCallWaiting0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0004
 * @SYMFssID BA/CTSY/CWAT-0004
 * @SYMTestCaseDesc Attempt to register, invoke and erase call waiting.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrArgument is returned
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
			_L("RMobilePhone::GetCallServiceCaps returned with an error"))
	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus;	
	TUint32 unwantedCaps = 0;	
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Register call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionRegister 
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionRegister;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"));			

	// Check KErrArgument is returned.
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrArgument, 
		_L("RMobilePhone::SetCallWaitingStatus returned with an error"))
		
	// Invoke call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionInvoke 
	mobilePhoneServiceAction = RMobilePhone::EServiceActionInvoke;
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"));				

	// Check KErrArgument is returned.
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrArgument, 
		_L("RMobilePhone::SetCallWaitingStatus returned with an error"));
		
	// Erase call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionErase 
	mobilePhoneServiceAction = RMobilePhone::EServiceActionErase;
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"));		

	// Check KErrArgument is returned.
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrArgument, 
		_L("RMobilePhone::SetCallWaitingStatus returned with an error"))
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&mobilePhoneSetCallWaitingStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0004");
	}



CCTSYIntegrationTestCallWaiting0005::CCTSYIntegrationTestCallWaiting0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0005::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0005::~CCTSYIntegrationTestCallWaiting0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0005
 * @SYMFssID BA/CTSY/CWAT-0005
 * @SYMTestCaseDesc Interrogate call waiting for all services.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallWaitingStatus
 * @SYMTestExpectedResults Pass - Call waiting status list is retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call waiting status list is correct.
 *
 * @return - TVerdict code
 */
	{
	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus | KCapsGetCWStatusNetwork 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
			_L("RMobilePhone::GetCallServiceCaps returned with an error"))
	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus
						| RMobilePhone::KCapsGetCWStatusNetwork;	
	TUint32 unwantedCaps = 0;	
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Deactivate all call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=RMobilePhone::EAllServices and aAction=RMobilePhone::EServiceActionDeactivate 
	RMobilePhone::TMobileService mobileService = RMobilePhone::EAllServices;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))					

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Activate call waiting for all services with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EAllServices  and aAction=EServiceActionActivate 
	mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
			_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
			_L("RMobilePhone::SetCallWaitingStatus returned with an error"))	
	
	// Retrieve the call waiting status list using the helper class for CRetrieveMobilePhoneCWList with aLocation=RMobilePhone::EInfoLocationCachePreferred
	CRetrieveMobilePhoneCWListExec *cwListExec;
	cwListExec = CRetrieveMobilePhoneCWListHelper::NewL(mobilePhone);
	CleanupStack::PushL(cwListExec);
	CMobilePhoneCWList* cwList;
	TInt error = 0;
	cwList = cwListExec->DoGetList(error);
	ASSERT_EQUALS(error, KErrNone, _L("CRetrieveMobilePhoneCWList::RetrieveListL returned with an error"))

	// Check CMobilePhoneCWList::Enumerate on the retrieved list returns number of entries > 0
	TInt noOfEntries = cwList->Enumerate();
	ASSERT_TRUE( noOfEntries >  0, 
				_L("CRetrieveMobilePhoneCWList::Enumerate returned zero entries"))

	// Check CMobilePhoneCWList::GetEntryL with aIndex=0 returns a RMobilePhone::TMobilePhoneCWInfoEntryV1 with iStatus=ECallWaitingStatusActive for atleast ETelephony and EFaxService
	TBool faxFlag = EFalse;
	TBool telFlag = EFalse;
	for(TInt i = 0; i < noOfEntries; ++i)
		{
		const RMobilePhone::TMobilePhoneCWInfoEntryV1&  cwInfoEntry = cwList->GetEntryL(i);		
		switch(cwInfoEntry.iServiceGroup)
			{
			case RMobilePhone::EFaxService:
				ASSERT_EQUALS(cwInfoEntry.iServiceGroup, RMobilePhone::EFaxService, 
						_L("CRetrieveMobilePhoneCWList::GetEntryL returned wrong iServiceGroup"))
				ASSERT_EQUALS(cwInfoEntry.iStatus, RMobilePhone::ECallWaitingStatusActive, 
						_L("CRetrieveMobilePhoneCWList::GetEntryL returned wrong iStatus"))
				faxFlag = ETrue;
				break;
			case RMobilePhone::ETelephony:
				ASSERT_EQUALS(cwInfoEntry.iServiceGroup, RMobilePhone::ETelephony, 
						_L("CRetrieveMobilePhoneCWList::GetEntryL returned wrong iServiceGroup"))
				ASSERT_EQUALS(cwInfoEntry.iStatus, RMobilePhone::ECallWaitingStatusActive, 
						_L("CRetrieveMobilePhoneCWList::GetEntryL returned wrong iStatus"))
				telFlag = ETrue;
				break;
			default:
				break;
			}
		}
	ASSERT_TRUE(telFlag && faxFlag, _L("CRetrieveMobilePhoneCWList::GetEntryL EFaxService or ETelephony are not activated"))

	// Deactivate all call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=RMobilePhone::EAllServices and aAction=RMobilePhone::EServiceActionDeactivate 
	mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
			_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
			_L("RMobilePhone::SetCallWaitingStatus returned with an error"))

	// Retrieve the call waiting status list using the helper class for CRetrieveMobilePhoneCWList with aLocation=RMobilePhone::EInfoLocationCachePreferred 
	error = 0;
	cwList = cwListExec->DoGetList(error);
	ASSERT_EQUALS(error, KErrNone, _L("CRetrieveMobilePhoneCWList::RetrieveListL is not able to get the list"))

	// Check CMobilePhoneCWList::Enumerate returns number of entries > 0
	noOfEntries = cwList->Enumerate();
	ASSERT_TRUE( noOfEntries >  0, 
				_L("CRetrieveMobilePhoneCWList::Enumerate returned with an error"));

	// Check CMobilePhoneCWList::GetEntryL with aIndex=0 returns a RMobilePhone::TMobilePhoneCWInfoEntryV1 with iServiceGroup=EAllServices and iStatus=ECallWaitingStatusNotActive
	const RMobilePhone::TMobilePhoneCWInfoEntryV1& cwInfoEntry1 = cwList->GetEntryL(0);	
	ASSERT_EQUALS(cwInfoEntry1.iServiceGroup, mobileService, 
				_L("CRetrieveMobilePhoneCWList::GetEntryL returned with an error"))	
	ASSERT_EQUALS(cwInfoEntry1.iStatus, RMobilePhone::ECallWaitingStatusNotActive, 
				_L("CRetrieveMobilePhoneCWList::GetEntryL returned with an error"))
				
	//
	// TEST END
	//

    StartCleanup();

	CleanupStack::PopAndDestroy(2, &mobilePhoneSetCallWaitingStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0005");
	}



CCTSYIntegrationTestCallWaiting0006::CCTSYIntegrationTestCallWaiting0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0006::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0006::~CCTSYIntegrationTestCallWaiting0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0006
 * @SYMFssID BA/CTSY/CWAT-0006
 * @SYMTestCaseDesc Interrogate call waiting for provisioned basic services.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallWaitingStatus
 * @SYMTestExpectedResults Pass - Call waiting status list is retrieved and is correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call waiting status list is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus | KCapsGetCWStatusNetwork
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
					 _L("RMobilePhone::GetCallServiceCaps returned with an error"))
	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus | RMobilePhone::KCapsNotifyCWStatus;
	TUint32 unwantedCaps = 0;	
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Deactivate all call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=RMobilePhone::EAllServices and aAction=RMobilePhone::EServiceActionDeactivate
	RMobilePhone::TMobileService mobileService = RMobilePhone::EAllServices;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"));
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Repeat whole test with aServiceGroup=EVoiceService, ECircuitDataService, EFaxService, active and checking for the appropriate active and not active services. 
	TUint32 count = 3;
	RArray<RMobilePhone::TMobileService> mobileServiceList;
	ASSERT_EQUALS(mobileServiceList.Append(RMobilePhone::EVoiceService),
			KErrNone,_L("RArray::Append returned with an error"));
	ASSERT_EQUALS(mobileServiceList.Append(RMobilePhone::ECircuitDataService),
		KErrNone,_L("RArray::Append returned with an error"));
	ASSERT_EQUALS(mobileServiceList.Append(RMobilePhone::EFaxService),
		KErrNone,_L("RArray::Append returned with an error"));


	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	err = iNetworkTsyTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));

	switch ( operatorName )
		{
		case EOperatorO2: 
			count = 1; //O2 supports only speech
			break;
		case EOperatorVodafone:
		case EOperatorNTN:
		case EOperatorOrange:
		case EOperatorTMobile: 
			count = 3; //vodafone, orange and tmobile support speech, voice and data
			break;
		case EOperatorUnknown:
		default:
			CHECK_TRUE_L( EFail, _L("Failed to identify current network"));
			break;
		}

	CRetrieveMobilePhoneCWListExec *cwListExec;
	cwListExec = CRetrieveMobilePhoneCWListHelper::NewL(mobilePhone);
	CleanupStack::PushL(cwListExec);
	
	for(TInt i=0; i < count; ++i)
	{	
		DEBUG_PRINTF2(_L("loop iteration for service : %d"),mobileServiceList[i]);
		// Activate call waiting for all services with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionActivate
		mobileService = mobileServiceList[i];
		mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;
		mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))
		ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))	
		
		// Retrieve the call waiting status list with CRetrieveMobilePhoneCWList::Start with aLocation=RMobilePhone::EInfoLocationCachePreferred 
				
		// ===  Retrieve the call waiting status list and check that it is active ===
		// ===  for the voice service and not active for the other basic services ===		
		// Use the helper class for CRetrieveMobilePhoneCWList to retrieve call waiting list
		TInt error = 0;
		CMobilePhoneCWList* cwList;
		cwList = cwListExec->DoGetList(error);
		ASSERT_EQUALS(error, KErrNone, _L("CRetrieveMobilePhoneCWList::RetrieveListL is not able to get the list"))
		
		// Check CMobilePhoneCWList::Enumerate returns number of entries >= 1
		TInt noOfEntries = cwList->Enumerate();
		ASSERT_TRUE( noOfEntries >=  1 , 
					_L("CRetrieveMobilePhoneCWList::Enumerate returned incorrect information"))
		
		// Check CMobilePhoneCWList::GetEntryL returns status of service as active 
		const RMobilePhone::TMobilePhoneCWInfoEntryV1& cwInfoEntry = cwList->GetEntryL(0);
		ASSERT_EQUALS(cwInfoEntry.iStatus, RMobilePhone::ECallWaitingStatusActive, 
				_L("CRetrieveMobilePhoneCWList::GetEntryL returned wrong iStatus"))

		// ===  Deactivate all call waiting and repeat ===
		// Deactivate all call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=RMobilePhone::EAllServices and aAction=RMobilePhone::EServiceActionDeactivate 
		mobileService = RMobilePhone::EAllServices;
		mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
		mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))
		ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))		
	}
	
	//
	// TEST END
	//

    StartCleanup();
	
    CleanupStack::PopAndDestroy(2,&mobilePhoneSetCallWaitingStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0006");
	}



CCTSYIntegrationTestCallWaiting0007::CCTSYIntegrationTestCallWaiting0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0007::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0007::~CCTSYIntegrationTestCallWaiting0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0007
 * @SYMFssID BA/CTSY/CWAT-0007
 * @SYMTestCaseDesc Interrogate call waiting status after activation and deactivation.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallWaitingStatus
 * @SYMTestExpectedResults Pass - Call waiting status is correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call waiting status list is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus | KCapsGetCWStatusNetwork
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
					 _L("RMobilePhone::GetCallServiceCaps returned with an error"))
	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus | RMobilePhone::KCapsNotifyCWStatus;
	TUint32 unwantedCaps = 0;	
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Deactivate all call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=RMobilePhone::EAllServices and aAction=RMobilePhone::EServiceActionDeactivate
	RMobilePhone::TMobileService mobileService = RMobilePhone::EAllServices;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"));
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"));
				
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Activate call waiting for voice, circuit data ===

	// Activate call waiting for all services with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionActivate 
	mobileService = RMobilePhone::EVoiceService;
	mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
			_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
			_L("RMobilePhone::SetCallWaitingStatus returned with an error"))		
	
	// Activate call waiting for all services with RMobilePhone::SetCallWaitingStatus with aServiceGroup=ECircuitDataService  and aAction=EServiceActionActivate 
	mobileService = RMobilePhone::ECircuitDataService;
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
			_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
			_L("RMobilePhone::SetCallWaitingStatus returned with an error"))		
	
	// Retrieve the call waiting status list with CRetrieveMobilePhoneCWList::Start with aLocation=RMobilePhone::EInfoLocationCachePreferred 
	CRetrieveMobilePhoneCWListExec *cwListExec;
	cwListExec = CRetrieveMobilePhoneCWListHelper::NewL(mobilePhone);
	CleanupStack::PushL(cwListExec);
	CMobilePhoneCWList* cwList;
	TInt error = 0;
	cwList = cwListExec->DoGetList(error);
	ASSERT_EQUALS(error, KErrNone, _L("CRetrieveMobilePhoneCWList::RetrieveListL returned with an error"))		

	// ===  Retrieve the call waiting status list and check that it is active ===

	// ===  for the voice service and not active for the other basic services ===

	// Retrieve the call waiting status list using the helper class for CRetrieveMobilePhoneCWList

	// Check CMobilePhoneCWList::Enumerate returns number of entries >= 2
	TInt noOfEntries = cwList->Enumerate();
	ASSERT_TRUE( noOfEntries >=  2, _L("CRetrieveMobilePhoneCWList::Enumerate returned incorrect information"))

	const RMobilePhone::TMobilePhoneCWInfoEntryV1& cwInfoEntry = cwList->GetEntryL(0);
	const RMobilePhone::TMobilePhoneCWInfoEntryV1& cwInfoEntry1 = cwList->GetEntryL(1);
	// Check CMobilePhoneCWList::GetEntryL with aIndex=0 returns a RMobilePhone::TMobilePhoneCWInfoEntryV1 with iServiceGroup=EVoiceService and iStatus=ECallWaitingStatusActive
	// Check CMobilePhoneCWList::GetEntryL with aIndex=1 returns a RMobilePhone::TMobilePhoneCWInfoEntryV1 with iServiceGroup=ECircuitDataService and iStatus=ECallWaitingStatusActive
	// or opposite.
	TBool serviceGroupFlag1(EFalse);
	TBool serviceGroupFlag2(EFalse);
	serviceGroupFlag1 = ((cwInfoEntry.iServiceGroup  ==  RMobilePhone::EVoiceService 
            || cwInfoEntry.iServiceGroup  ==  RMobilePhone::ETelephony)
            && ( cwInfoEntry1.iServiceGroup == RMobilePhone::ECircuitDataService
            || cwInfoEntry1.iServiceGroup == RMobilePhone::ESyncData
            || cwInfoEntry1.iServiceGroup == RMobilePhone::EAsyncData) );
	serviceGroupFlag2 = ((cwInfoEntry1.iServiceGroup  ==  RMobilePhone::EVoiceService 
            || cwInfoEntry1.iServiceGroup  ==  RMobilePhone::ETelephony)
            && ( cwInfoEntry.iServiceGroup == RMobilePhone::ECircuitDataService
            || cwInfoEntry.iServiceGroup == RMobilePhone::ESyncData
            || cwInfoEntry.iServiceGroup == RMobilePhone::EAsyncData) );
	
	ASSERT_TRUE((serviceGroupFlag1 || serviceGroupFlag2), 
            _L("CRetrieveMobilePhoneCWList::GetEntryL returned wrong iServiceGroup"));
    ASSERT_EQUALS(cwInfoEntry.iStatus, RMobilePhone::ECallWaitingStatusActive, 
            _L("CRetrieveMobilePhoneCWList::GetEntryL returned wrong iStatus"));
    ASSERT_EQUALS(cwInfoEntry1.iStatus, RMobilePhone::ECallWaitingStatusActive, 
            _L("CRetrieveMobilePhoneCWList::GetEntryL returned wrong iStatus"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// 1 mobilePhoneSetCallWaitingStatus
	// 2 cwListExec
	CleanupStack::PopAndDestroy(2,&mobilePhoneSetCallWaitingStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0007");
	}



CCTSYIntegrationTestCallWaiting0008::CCTSYIntegrationTestCallWaiting0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0008::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0008::~CCTSYIntegrationTestCallWaiting0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0008
 * @SYMFssID BA/CTSY/CWAT-0008
 * @SYMTestCaseDesc Interrogation of Call Waiting status where CW is not provisioned on the SIM
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - NULL status list returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify no call waiting status list is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Retrieve the call waiting status list with helper class for CRetrieveMobilePhoneCWList with aLocation=RMobilePhone::EInfoLocationCachePreferred 

	// ===  Retrieve the call waiting status list and check that it is active ===

	// ===  for the voice service and not active for the other basic services ===

	// Check retrieved list is NULL

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0008");
	}



CCTSYIntegrationTestCallWaiting0009::CCTSYIntegrationTestCallWaiting0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0009::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0009::~CCTSYIntegrationTestCallWaiting0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0009
 * @SYMFssID BA/CTSY/CWAT-0009
 * @SYMTestCaseDesc Accept waiting call after active call is hung up.
 * @SYMTestPriority High
 * @SYMTestActions RCall::AnswerIncomingCall, RCall::HangUp, RCall::Dial, RLine::NotifyIncomingCall, RMobilePhone::SetCallWaitingStatus
 * @SYMTestExpectedResults Pass - Call that is waiting can be answered when the original active call terminates.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/rps
 *
 * Reason for test: Verify incoming call notification completes and waiting call can be answered.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus capabilities. 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
			_L("RMobilePhone::GetCallServiceCaps returned with an errorr"))

	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus;
	TUint32 unwantedCaps = 0;
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	// register on home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
	
	// Get voice line 1. 
	RMobileLine& mobileLine1 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone,KVoiceLine);

	// Get call 1. 
	RMobileCall& mobileCall1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone,KVoiceLine,KCall1);

	// Get voice line 2. 
	RMobileLine& mobileLine2 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone,KVoiceLine);
	
	// Get call 2. 
	RMobileCall& mobileCall2 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone,KVoiceLine,KCall2);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Activate call waiting for all services with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionActivate 
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))	

	// post notification for call status change
	TExtEtelRequestStatus notifyDialStatus(mobileCall1,EMobileCallNotifyMobileCallStatusChange);	
	CleanupStack::PushL(notifyDialStatus);		
	RMobileCall::TMobileCallStatus callStatus;
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	
	// Dial a number that answers on call 1 with RCall::Dial 
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);	
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig returned with an error"));
	mobileCall1.Dial(dialStatus, number); 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,
			_L("RMobileCall::GetMobileCallStatus did not complete as expected"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusDialling,KErrNone);	
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,
			_L("RMobileCall::GetMobileCallStatus did not complete as expected"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusConnecting,KErrNone);
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"))
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,			
			_L("RMobileCall::GetMobileCallStatus returned with an error"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusConnected,KErrNone);
	
	// check status of dail api
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"))
	
	// Register for notification of an incoming call on line 2 with RLine::NotifyIncomingCall 
	TCoreEtelRequestStatus<RLine> incomingCallStatus(mobileLine2, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(incomingCallStatus);
	TName callName;
	mobileLine2.NotifyIncomingCall(incomingCallStatus,callName);

    // Request incoming call from RPS.
	RPSRequestIncomingCallL(EVoiceLine);
	
	// Check RLine::NotifyIncomingCall on line 2 completes with valid call name in aName.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(incomingCallStatus,ETimeMedium), KErrNone, 
			_L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(incomingCallStatus.Int(), KErrNone,  
			_L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(callName.Length() > 0,  
			_L("RLine::NotifyIncomingCall did not set the name"));
	
	// post notification for call status change
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	
	// Hang up call 1 with RCall::HangUp 
	mobileCall1.HangUp();
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDisconnecting -> EStatusIdle
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusDisconnecting,KErrNone);	
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusIdle,KErrNone);

	
	// obtain reference to incoming call object
	TCallId incomingCallId;
	RMobileCall& incomingCall = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, callName, incomingCallId);

	// post notification for call status change
	TExtEtelRequestStatus notifyIncomingCallStatus(incomingCall,EMobileCallNotifyMobileCallStatusChange);	
	CleanupStack::PushL(notifyIncomingCallStatus);
	incomingCall.NotifyMobileCallStatusChange(notifyIncomingCallStatus,callStatus);
	
	// Answer call on call 2 with RCall::AnswerIncomingCall 
	TExtEtelRequestStatus answerIncomingCalllStatus(incomingCall,EEtelCallAnswer);	
	CleanupStack::PushL(answerIncomingCalllStatus);
	incomingCall.AnswerIncomingCall(answerIncomingCalllStatus);

	// Check RMobileCall::NotifyMobileCallStatusChange on call 2 completes with EStatusAnswering -> EStatusConnected
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(incomingCall,
			notifyIncomingCallStatus,callStatus,RMobileCall::EStatusAnswering,KErrNone);
	incomingCall.NotifyMobileCallStatusChange(notifyIncomingCallStatus,callStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(incomingCall,
			notifyIncomingCallStatus,callStatus,RMobileCall::EStatusConnected,KErrNone);
	
	ASSERT_EQUALS(answerIncomingCalllStatus.Int(), KErrNone, _L("RCall::AnswerIncomingCall returned with an error"))

	// Hang up call 2 with RCall::HangUp 
	ASSERT_EQUALS(incomingCall.HangUp(), KErrNone,  
		_L("RCall::HangUp returned with and error"));

	//
	// TEST END
	//

    StartCleanup();
    
    //1 mobilePhoneSetCallWaitingStatus
	//2 notifyDialStatus
    //3 dialStatus
    //4 incomingCallStatus
    //5 notifyIncomingCallStatus
    //6 AnswerIncomingCall
	
	CleanupStack::PopAndDestroy(6, &mobilePhoneSetCallWaitingStatus);
	
	StartCleanup();

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0009");
	}



CCTSYIntegrationTestCallWaiting0010::CCTSYIntegrationTestCallWaiting0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0010::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0010::~CCTSYIntegrationTestCallWaiting0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0010
 * @SYMFssID BA/CTSY/CWAT-0010
 * @SYMTestCaseDesc Accept waiting call after active call is held.
 * @SYMTestPriority High
 * @SYMTestActions RCall::HangUp, RCall::AnswerIncomingCall, RCall::Dial, RMobileCall::Hold, RLine::NotifyIncomingCall, RMobilePhone::SetCallWaitingStatus
 * @SYMTestExpectedResults Pass - Can put a call on hold and answer a waiting call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/rps
 *
 * Reason for test: Verify incoming call notification completes and waiting call can be answered
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus capabilities.
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
			_L("RMobilePhone::GetCallServiceCaps returned with an errorr"))

	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus;
	TUint32 unwantedCaps = 0;
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	// register on home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
	
	// Get voice line 1. 	
	RMobileLine& mobileLine1 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone,KVoiceLine);

	// Get call 1. 
	RMobileCall& mobileCall1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone,KVoiceLine,KCall1);

	// Get voice line 2. 
	RMobileLine& mobileLine2 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone,KVoiceLine);
	
	// Get call 2. 
	RMobileCall& mobileCall2 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone,KVoiceLine,KCall2);
	 
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//	
	
	// Activate call waiting for all services with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionActivate 
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))	
				
	// post notification for call status change
	RMobileCall::TMobileCallStatus callStatus;	
	TExtEtelRequestStatus notifyDialStatus(mobileCall1,EMobileCallNotifyMobileCallStatusChange);	
	CleanupStack::PushL(notifyDialStatus);		
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	
	// Dial a number that answers on call 1 with RCall::Dial 
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);	
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig returned with an error"));
	mobileCall1.Dial(dialStatus, number); 
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,
			_L("RMobileCall::GetMobileCallStatus did not complete as expected"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusDialling,KErrNone);
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,
			_L("RMobileCall::GetMobileCallStatus did not complete as expected"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusConnecting,KErrNone);
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"))
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,			
			_L("RMobileCall::GetMobileCallStatus returned with an error"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusConnected,KErrNone);

	// Register for notification of an incoming call on line 2 with RLine::NotifyIncomingCall 
	TCoreEtelRequestStatus<RLine> incomingCallStatus(mobileLine2, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(incomingCallStatus);
	TName callName;
	mobileLine2.NotifyIncomingCall(incomingCallStatus,callName);

	// Request incoming call from RPS. 
	RPSRequestIncomingCallL(EVoiceLine);

	// Check RLine::NotifyIncomingCall on line 2 completes with valid call name in aName.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(incomingCallStatus,ETimeMedium), KErrNone, 
			_L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(incomingCallStatus.Int(), KErrNone,  
			_L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(callName.Length() > 0,  
			_L("RLine::NotifyIncomingCall did not set the name"));
	
	// post notification for call status change
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);

	// Hold call 1 with RMobileCall::Hold 
	TExtEtelRequestStatus holdStatus(mobileCall1,EMobileCallHold);	
	CleanupStack::PushL(holdStatus);
	mobileCall1.Hold(holdStatus);

	// Check RMobileCall::NotifyMobileCallStatusChange on call 1 completes with EStatusHold
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusHold,KErrNone);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdStatus, ETimeMedium), KErrNone, _L("RMobileCall::Hold timed-out"))
	ASSERT_EQUALS(holdStatus.Int(), KErrNone,			
			_L("RMobileCall::Hold returned with an error"));
	
	// obtain reference to incoming call object
	TCallId incomingCallId;
	RMobileCall& incomingCall = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, callName, incomingCallId);

	// post notification for call status change
	TExtEtelRequestStatus notifyIncomingCallStatus(incomingCall,EMobileCallNotifyMobileCallStatusChange);	
	CleanupStack::PushL(notifyIncomingCallStatus);
	incomingCall.NotifyMobileCallStatusChange(notifyIncomingCallStatus,callStatus);
	
	// Answer call on call 2 with RCall::AnswerIncomingCall 
	TExtEtelRequestStatus answerIncomingCalllStatus(incomingCall,EEtelCallAnswer);	
	CleanupStack::PushL(answerIncomingCalllStatus);
	incomingCall.AnswerIncomingCall(answerIncomingCalllStatus);

	// Check RMobileCall::NotifyMobileCallStatusChange on call 2 completes with EStatusAnswering -> EStatusConnected
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(incomingCall,
			notifyIncomingCallStatus,callStatus,RMobileCall::EStatusAnswering,KErrNone);
	incomingCall.NotifyMobileCallStatusChange(notifyIncomingCallStatus,callStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(incomingCall,
			notifyIncomingCallStatus,callStatus,RMobileCall::EStatusConnected,KErrNone);
	
	ASSERT_EQUALS(answerIncomingCalllStatus.Int(), KErrNone, _L("RCall::AnswerIncomingCall returned with an error"))

	// Hang up call 2 with RCall::HangUp 
	ASSERT_EQUALS(incomingCall.HangUp(), KErrNone,  
		_L("RCall::HangUp returned with and error"));
	ASSERT_EQUALS(mobileCall1.HangUp(), KErrNone,  
			_L("RCall::HangUp returned with and error"));

	//
	// TEST END
	//

    StartCleanup();
	
    // 1 mobilePhoneSetCallWaitingStatus
    // 2 notifyDialStatus
    // 3 dialStatus
    // 4 incomingCallStatus
    // 5 holdStatus
    // 6 notifyIncomingCallStatus
    // 7 answerIncomingCalllStatus
    
    CleanupStack::PopAndDestroy(7, &mobilePhoneSetCallWaitingStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0010");
	}



CCTSYIntegrationTestCallWaiting0011::CCTSYIntegrationTestCallWaiting0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0011::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0011::~CCTSYIntegrationTestCallWaiting0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0011
 * @SYMFssID BA/CTSY/CWAT-0011
 * @SYMTestCaseDesc Accept waiting call after active call is hung up by remote party.
 * @SYMTestPriority High
 * @SYMTestActions RCall::HangUp, RCall::AnswerIncomingCall, RCall::Dial, RLine::NotifyIncomingCall, RMobilePhone::SetCallWaitingStatus
 * @SYMTestExpectedResults Pass - Waiting call can be answered.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify incoming call notification completes and waiting call can be answered
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus capabilities.
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
			_L("RMobilePhone::GetCallServiceCaps returned with an errorr"))

	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus;
	TUint32 unwantedCaps = 0;
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	// register on home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Get voice line 1. 
	RMobileLine& mobileLine1 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone,KVoiceLine);

	// Get call 1. 
	RMobileCall& mobileCall1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone,KVoiceLine,KCall1);

	// Get voice line 2. 
	RMobileLine& mobileLine2 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone,KVoiceLine);
	
	// Get call 2. 
	RMobileCall& mobileCall2 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone,KVoiceLine,KCall2);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Activate call waiting for all services with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionActivate 
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))	
				
	// post notification for call status change
	RMobileCall::TMobileCallStatus callStatus;	
	TExtEtelRequestStatus notifyDialStatus(mobileCall1,EMobileCallNotifyMobileCallStatusChange);	
	CleanupStack::PushL(notifyDialStatus);		
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	
	// Dial a number and answer the call on call 1 with RCall::Dial	
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);	
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig returned with an error"));
	mobileCall1.Dial(dialStatus, number); 
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,
			_L("RMobileCall::GetMobileCallStatus did not complete as expected"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusDialling,KErrNone);
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,
			_L("RMobileCall::GetMobileCallStatus did not complete as expected"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusConnecting,KErrNone);
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"))
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,			
			_L("RMobileCall::GetMobileCallStatus returned with an error"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusConnected,KErrNone);

    // Register for notification of an incoming call on line 2 with RLine::NotifyIncomingCall 
	TCoreEtelRequestStatus<RLine> incomingCallStatus(mobileLine2, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(incomingCallStatus);
	TName callName;
	mobileLine2.NotifyIncomingCall(incomingCallStatus,callName);

	// Dial the board manually. 
	RPSRequestIncomingCallL(EVoiceLine);
	
	// Check RLine::NotifyIncomingCall on line 2 completes with valid call name in aName.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(incomingCallStatus,ETimeMedium), KErrNone, 
			_L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(incomingCallStatus.Int(), KErrNone,  
			_L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(callName.Length() > 0,  
			_L("RLine::NotifyIncomingCall did not set the name"));
	
	// post notification for call status change
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);

	// Hang up call 1 as the remote party. 
	DisplayUserInteractionPromptL(_L("Please hangup the active call"));

	// Check RMobileCall::NotifyMobileCallStatusChange on call 1 completes with EStatusDisconnecting -> EStatusIdle
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusDisconnecting,KErrNone);
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,
			_L("RMobileCall::GetMobileCallStatus did not complete as expected"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusIdle,KErrNone);

	// obtain reference to incoming call object
	TCallId incomingCallId;
	RMobileCall& incomingCall = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, callName, incomingCallId);

	// post notification for call status change
	TExtEtelRequestStatus notifyIncomingCallStatus(incomingCall,EMobileCallNotifyMobileCallStatusChange);	
	CleanupStack::PushL(notifyIncomingCallStatus);
	incomingCall.NotifyMobileCallStatusChange(notifyIncomingCallStatus,callStatus);

	// Answer call on call 2 with RCall::AnswerIncomingCall 
	TExtEtelRequestStatus answerIncomingCalllStatus(incomingCall,EEtelCallAnswer);	
	CleanupStack::PushL(answerIncomingCalllStatus);
	incomingCall.AnswerIncomingCall(answerIncomingCalllStatus);

	// Check RMobileCall::NotifyMobileCallStatusChange on call 2 completes with EStatusAnswering -> EStatusConnected
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(incomingCall,
			notifyIncomingCallStatus,callStatus,RMobileCall::EStatusAnswering,KErrNone);
	incomingCall.NotifyMobileCallStatusChange(notifyIncomingCallStatus,callStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(incomingCall,
			notifyIncomingCallStatus,callStatus,RMobileCall::EStatusConnected,KErrNone);	
	ASSERT_EQUALS(answerIncomingCalllStatus.Int(), KErrNone, _L("RCall::AnswerIncomingCall returned with an error"))
	
	// Hang up call 2 with RCall::HangUp 
	ASSERT_EQUALS(incomingCall.HangUp(), KErrNone,  
		_L("RCall::HangUp returned with and error"));
		
	//
	// TEST END
	//

    StartCleanup();
	
    // 1 mobilePhoneSetCallWaitingStatus
    // 2 notifyDialStatus
    // 3 dialStatus
    // 4 incomingCallStatus
    // 5 notifyIncomingCallStatus
    // 6 answerIncomingCalllStatus
    
    CleanupStack::PopAndDestroy(6, &mobilePhoneSetCallWaitingStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0011");
	}



CCTSYIntegrationTestCallWaiting0012::CCTSYIntegrationTestCallWaiting0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallWaiting0012::GetTestStepName());
	}

CCTSYIntegrationTestCallWaiting0012::~CCTSYIntegrationTestCallWaiting0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallWaiting0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CWAT-0012
 * @SYMFssID BA/CTSY/CWAT-0012
 * @SYMTestCaseDesc Reject a waiting call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::HangUp, RCall::Dial, RLine::NotifyIncomingCall, RMobilePhone::SetCallWaitingStatus
 * @SYMTestExpectedResults Pass - Waiting call can be rejected.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/rps
 *
 * Reason for test: Verify incoming call notification completes and line status is correct after reject.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCWStatus capabilities.
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps;
	TInt err = mobilePhone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone,
			_L("RMobilePhone::GetCallServiceCaps returned with an error"))

	TUint32 expectedCaps = RMobilePhone::KCapsSetCWStatus;
	TUint32 unwantedCaps = 0;
	CHECK_BITS_SET_L(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))						

	// register on home network
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	// Get voice line 1. 
	RMobileLine& mobileLine1 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone,KVoiceLine);

	// Get call 1. 
	RMobileCall& mobileCall1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone,KVoiceLine,KCall1);

	// Get voice line 2. 
	RMobileLine& mobileLine2 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone,KVoiceLine);
	
	// Get call 2. 
	RMobileCall& mobileCall2 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone,KVoiceLine,KCall2);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Activate call waiting for all services with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EVoiceService  and aAction=EServiceActionActivate
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(mobilePhone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	mobilePhone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetCallWaitingStatus timed-out"))
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, 
				_L("RMobilePhone::SetCallWaitingStatus returned with an error"))	
				
	// post notification for call status change
	RMobileCall::TMobileCallStatus callStatus;	
	TExtEtelRequestStatus notifyDialStatus(mobileCall1,EMobileCallNotifyMobileCallStatusChange);	
	CleanupStack::PushL(notifyDialStatus);		
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);

	// Dial a number that answers call 1 with RCall::Dial 
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);	
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig returned with an error"));
	mobileCall1.Dial(dialStatus, number); 
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,
			_L("RMobileCall::GetMobileCallStatus did not complete as expected"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusDialling,KErrNone);
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,
			_L("RMobileCall::GetMobileCallStatus did not complete as expected"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusConnecting,KErrNone);
	mobileCall1.NotifyMobileCallStatusChange(notifyDialStatus,callStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"))
	ASSERT_EQUALS(mobileCall1.GetMobileCallStatus(callStatus), KErrNone,			
			_L("RMobileCall::GetMobileCallStatus returned with an error"));
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall1,
			notifyDialStatus,callStatus,RMobileCall::EStatusConnected,KErrNone);

    // Register for notification of an incoming call on line 2 with RLine::NotifyIncomingCall 
	TCoreEtelRequestStatus<RLine> incomingCallStatus(mobileLine2, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(incomingCallStatus);
	TName callName;
	mobileLine2.NotifyIncomingCall(incomingCallStatus,callName);
	
	// Request incoming call from RPS.
	RPSRequestIncomingCallL(EVoiceLine);

	// Check RLine::NotifyIncomingCall on line 2 completes with valid call name in aName.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(incomingCallStatus,ETimeMedium), KErrNone, 
			_L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(incomingCallStatus.Int(), KErrNone,  
			_L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(callName.Length() > 0,  
			_L("RLine::NotifyIncomingCall did not set the name"));	
	
	// post notification for call status change
	TCallId incomingCallId;
	RMobileCall& incomingCall = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, callName, incomingCallId);

	// post notification for call status change
	TExtEtelRequestStatus notifyIncomingCallStatus(incomingCall,EMobileCallNotifyMobileCallStatusChange);	
	CleanupStack::PushL(notifyIncomingCallStatus);
	incomingCall.NotifyMobileCallStatusChange(notifyIncomingCallStatus,callStatus);
	
	// Hang up call 2 with RCall::HangUp 
	ASSERT_EQUALS(incomingCall.HangUp(), KErrNone,  
			_L("RCall::HangUp returned with and error"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange on call 2 completes with EStatusIdle	
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(incomingCall,
			notifyIncomingCallStatus,callStatus,RMobileCall::EStatusIdle,KErrNone);

	//
	// TEST END
	//

    StartCleanup();
	
	mobileCall1.HangUp();
	
    // 1 mobilePhoneSetCallWaitingStatus
    // 2 notifyDialStatus
    // 3 dialStatus
    // 4 incomingCallStatus
    // 5 notifyIncomingCallStatus
    
    CleanupStack::PopAndDestroy(5, &mobilePhoneSetCallWaitingStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallWaiting0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallWaiting0012");
	}



