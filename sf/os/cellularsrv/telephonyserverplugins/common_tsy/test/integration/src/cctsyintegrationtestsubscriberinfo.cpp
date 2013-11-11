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
// Test step definitions for the SubscriberInfo functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestsubscriberinfo.h"

#include "cetelsessionmgr.h"
#include "tetelrequeststatus.h"

	
CCTSYIntegrationTestSubscriberInfoBase::CCTSYIntegrationTestSubscriberInfoBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestSubscriberInfoBase::~CCTSYIntegrationTestSubscriberInfoBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestSubscriberInfo0001::CCTSYIntegrationTestSubscriberInfo0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSubscriberInfoBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSubscriberInfo0001::GetTestStepName());
	}

CCTSYIntegrationTestSubscriberInfo0001::~CCTSYIntegrationTestSubscriberInfo0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSubscriberInfo0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUBI-0001
 * @SYMFssID BA/CTSY/SUBI-0001
 * @SYMTestCaseDesc Get the customer service profile with no SIM inserted.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrNotFound returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Remove SIM from phone. 
	
	DisplayUserInteractionPromptL(_L("The SIM should have been removed before starting this test"));
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	

	//
	// SET UP END
	//
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RMobilePhone::GetCustomerServiceProfile returns KErrNotFound
	RMobilePhone::TMobilePhoneCspFileV1 serviceProfile;
	RMobilePhone::TMobilePhoneCspFileV1Pckg serviceProfilePkg( serviceProfile );
	TExtEtelRequestStatus reqStatus( phone, EMobilePhoneGetCustomerServiceProfile );
	CleanupStack::PushL(reqStatus);
	phone.GetCustomerServiceProfile( reqStatus, serviceProfilePkg );
	
	TInt err = WaitForRequestWithTimeOut(reqStatus, ETimeMedium);
	ASSERT_EQUALS(err,KErrNotFound,_L("RMobilePhone::GetCustomerServiceProfile returned an error"));	

	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1,&reqStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSubscriberInfo0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSubscriberInfo0001");
	}



CCTSYIntegrationTestSubscriberInfo0002::CCTSYIntegrationTestSubscriberInfo0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSubscriberInfoBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSubscriberInfo0002::GetTestStepName());
	}

CCTSYIntegrationTestSubscriberInfo0002::~CCTSYIntegrationTestSubscriberInfo0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSubscriberInfo0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUBI-0002
 * @SYMFssID BA/CTSY/SUBI-0002
 * @SYMTestCaseDesc Get the subscriber ID (IMSI)
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - A subscriber ID is returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify a value is returned.
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

	// Check RMobilePhone::GetSubscriberId returns a valid subscriber Id in aId
	TExtEtelRequestStatus subscriberIdStatus(phone, EMobilePhoneGetSubscriberId);
	CleanupStack::PushL(subscriberIdStatus);
	
	RMobilePhone::TMobilePhoneSubscriberId subscriberId;
	phone.GetSubscriberId(subscriberIdStatus, subscriberId);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(subscriberIdStatus, ETimeLong),
			KErrNone, _L("RMobilePhone::GetSubscriberId timed-out."));
	ASSERT_EQUALS(subscriberIdStatus.Int(), KErrNone, _L("RMobilePhone::GetSubscriberId returned with an error"));
	ASSERT_TRUE(subscriberId.Length() > 0, _L("RMobilePhone::GetSubscriberId returned invalid IMSI in aId"));
	
    //
    // Test end
    //

    StartCleanup();

	CleanupStack::PopAndDestroy(1,&subscriberIdStatus);  

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSubscriberInfo0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSubscriberInfo0002");
	}



CCTSYIntegrationTestSubscriberInfo0003::CCTSYIntegrationTestSubscriberInfo0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSubscriberInfoBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSubscriberInfo0003::GetTestStepName());
	}

CCTSYIntegrationTestSubscriberInfo0003::~CCTSYIntegrationTestSubscriberInfo0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSubscriberInfo0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUBI-0003
 * @SYMFssID BA/CTSY/SUBI-0003
 * @SYMTestCaseDesc Get the ICC identity
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - The ICC identity is returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify a value is returned.
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
	
	ASSERT_EQUALS(1,0, _L("NOTE: CTSY doesn't actually support this API - test case can be used if support is added"));
	TEST_CHECK_POINT_L(_L("Test leaving for now,  remove this line when this test is valid once more"));
	
	// Check RMobilePhone::GetIccIdentity returns aIdentity with length > 0
	RMobilePhone::TIccIdentity iccIdentity;
	TExtEtelRequestStatus reqStatus( phone, EMobilePhoneGetIccIdentity );
	CleanupStack::PushL( reqStatus );
	phone.GetIccIdentity( reqStatus, iccIdentity );
	
	TInt err = WaitForRequestWithTimeOut(reqStatus, ETimeMedium);
	ASSERT_EQUALS(err,KErrNone,_L("RMobilePhone::GetIccIdentity returned an error"))	
	ASSERT_TRUE( iccIdentity.Length() > 0, _L("RMobilePhone::GetIccIdentity returned identity with length 0"))
	
	//
	// TEST END
	//

    StartCleanup();
	CleanupStack::PopAndDestroy(1,&reqStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSubscriberInfo0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSubscriberInfo0003");
	}



CCTSYIntegrationTestSubscriberInfo0004::CCTSYIntegrationTestSubscriberInfo0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSubscriberInfoBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSubscriberInfo0004::GetTestStepName());
	}

CCTSYIntegrationTestSubscriberInfo0004::~CCTSYIntegrationTestSubscriberInfo0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSubscriberInfo0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUBI-0004
 * @SYMFssID BA/CTSY/SUBI-0004
 * @SYMTestCaseDesc Get ICC access caps with a 2G SIM
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetIccAccessCaps
 * @SYMTestExpectedResults Pass - Correct ICC caps returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify SIM access caps present but not USIM access caps
 *
 * @return - TVerdict code
 */
	{
	//
	// SET UP
	//

	// Insert a 2G Sim. 
	// Boot the phone. 
	DisplayUserInteractionPromptL( _L("Please ensure a 2G SIM has been inserted before starting this test"), ETimeVeryLong );

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
		
	// Post a notification RMobilePhone::NotifySecurityEvent
	// Triggers CTSY to call down to LTSY to get the ICC type
	// which allows this test to be run.
	RMobilePhone::TMobilePhoneSecurityEvent event;
	TExtEtelRequestStatus notifySecurityEventStatus(phone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL(notifySecurityEventStatus);
	phone.NotifySecurityEvent(notifySecurityEventStatus, event); 
	
	User::After(KOneSecond*5);
	
	//
	// SET UP END
	//

	StartTest();
	
	//
	// TEST START
	//

	// ===  Check ICC access caps ===
	
	// Check RMobilePhone::GetIccAccessCaps returns caps in set of KCapsSimAccessSupported
	// Check RMobilePhone::GetIccAccessCaps returns caps NOT in set of
	// KCapsUSimAccessSupported | KCapsRUimAccessSupported 
	
	TUint32 iccAccessCaps;
	TInt err = phone.GetIccAccessCaps( iccAccessCaps );

	ASSERT_EQUALS(err,KErrNone,_L("RMobilePhone::GetIccAccessCaps failed"))	
	ASSERT_BITS_SET(iccAccessCaps, 
							RMobilePhone::KCapsSimAccessSupported,
							RMobilePhone::KCapsUSimAccessSupported | RMobilePhone::KCapsRUimAccessSupported,
							_L("RMobilePhone::GetIccAccessCaps returned unexpected bits") )
	


							
	//
	// TEST END
	//

    StartCleanup();
    CleanupStack::PopAndDestroy(&notifySecurityEventStatus);	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSubscriberInfo0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSubscriberInfo0004");
	}



CCTSYIntegrationTestSubscriberInfo0005::CCTSYIntegrationTestSubscriberInfo0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSubscriberInfoBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSubscriberInfo0005::GetTestStepName());
	}

CCTSYIntegrationTestSubscriberInfo0005::~CCTSYIntegrationTestSubscriberInfo0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSubscriberInfo0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUBI-0005
 * @SYMFssID BA/CTSY/SUBI-0005
 * @SYMTestCaseDesc Get ICC access caps with a 3G SIM
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetIccAccessCaps
 * @SYMTestExpectedResults Pass - Correct ICC caps returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify correct caps are returned.
 *
 * @return - TVerdict code
 */
	{
	// Insert a 3G Sim. 
	// Boot the phone. 
	DoPauseL( _L("Please ensure a 3G SIM has been inserted before starting this test"), ETimeVeryLong );

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// In the CTSY, the security event triggers a call to 
	// get the ICC type from the SIM
	TExtEtelRequestStatus notifySecurityEventStatus(phone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL(notifySecurityEventStatus);
	RMobilePhone::TMobilePhoneSecurityEvent event;
	phone.NotifySecurityEvent(notifySecurityEventStatus, event); 
	User::After(KOneSecond*5);
	
	//
	// SET UP END
	//	
	
	StartTest();
	
	//
	// TEST START
	//
	

	// ===  Check for change in ICC access caps ===

	// Check RMobilePhone::GetIccAccessCaps returns caps in set of KCapsUSimAccessSupported | KCapsSimAccessSupported
	// Check RMobilePhone::GetIccAccessCaps returns caps NOT in set of KCapsRUimAccessSupported
	TUint32 iccAccessCaps;
	TInt err = phone.GetIccAccessCaps( iccAccessCaps );
	ASSERT_EQUALS(err,KErrNone,_L("RMobilePhone::GetIccAccessCaps returned an error"));
	ASSERT_BITS_SET(iccAccessCaps, 
							RMobilePhone::KCapsSimAccessSupported | RMobilePhone::KCapsUSimAccessSupported,
							 RMobilePhone::KCapsRUimAccessSupported,
							_L("RMobilePhone::GetIccAccessCaps returned unexpected bits") );
	
	//
	// TEST END
	//

    StartCleanup();
	
    CleanupStack::PopAndDestroy(&notifySecurityEventStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSubscriberInfo0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSubscriberInfo0005");
	}



CCTSYIntegrationTestSubscriberInfo0006::CCTSYIntegrationTestSubscriberInfo0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSubscriberInfoBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSubscriberInfo0006::GetTestStepName());
	}

CCTSYIntegrationTestSubscriberInfo0006::~CCTSYIntegrationTestSubscriberInfo0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSubscriberInfo0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUBI-0006
 * @SYMFssID BA/CTSY/SUBI-0006
 * @SYMTestCaseDesc Cancel notification of change of ICC access caps
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request is cancelled.
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
	
	// Make a request for RMobilePhone::NotifyIccAccessCapsChange 
	TUint32 accessCaps;
	TExtEtelRequestStatus reqAccessCapsStatus(phone, EMobilePhoneNotifyIccAccessCapsChange );
	CleanupStack::PushL(reqAccessCapsStatus);
	phone.NotifyIccAccessCapsChange( reqAccessCapsStatus, accessCaps );
	
	// Cancel the request. 
	phone.CancelAsyncRequest( EMobilePhoneNotifyIccAccessCapsChange );

	// Check request status used for the request is KErrCancel
	ASSERT_EQUALS( reqAccessCapsStatus.Int(), KErrCancel, 
							_L("Cancelled RMobilePhone::NotifyIccAccessCapsChange gave unexpected status"));

	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1,&reqAccessCapsStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSubscriberInfo0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSubscriberInfo0006");
	}



CCTSYIntegrationTestSubscriberInfo0007::CCTSYIntegrationTestSubscriberInfo0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSubscriberInfoBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSubscriberInfo0007::GetTestStepName());
	}

CCTSYIntegrationTestSubscriberInfo0007::~CCTSYIntegrationTestSubscriberInfo0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSubscriberInfo0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUBI-0007
 * @SYMFssID BA/CTSY/SUBI-0007
 * @SYMTestCaseDesc Get SIM service table with a SIM.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SIM service table returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify SMS and ADN service bits in SIM service table set.
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

	// ===  Check the SIM has SMS service and ADN service ===

	// Check RMobilePhone::GetServiceTable with aTable=ESIMServiceTable 
	// returns iServices1To8 with bit in set of KSstSMS | KSstADN
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg serviceTablePkg( serviceTable );
	TExtEtelRequestStatus reqGetServiceTableStatus( phone, EMobilePhoneGetServiceTable );
	CleanupStack::PushL( reqGetServiceTableStatus );
	phone.GetServiceTable( reqGetServiceTableStatus, RMobilePhone::ESIMServiceTable, serviceTablePkg );
	TInt err = WaitForRequestWithTimeOut(reqGetServiceTableStatus, ETimeMedium);
	ASSERT_EQUALS(err,KErrNone,_L("RMobilePhone::GetServiceTable timed out"));
	
	ASSERT_BITS_SET( serviceTable.iServices1To8, 
					RMobilePhone::KSstSMS | RMobilePhone::KSstADN, 
					KNoUnwantedBits,
					_L("RMobilePhone::GetServiceTable returned incorrect bits in iServices1To8") );


	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1,&reqGetServiceTableStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSubscriberInfo0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSubscriberInfo0007");
	}



CCTSYIntegrationTestSubscriberInfo0008::CCTSYIntegrationTestSubscriberInfo0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSubscriberInfoBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSubscriberInfo0008::GetTestStepName());
	}

CCTSYIntegrationTestSubscriberInfo0008::~CCTSYIntegrationTestSubscriberInfo0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSubscriberInfo0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUBI-0008
 * @SYMFssID BA/CTSY/SUBI-0008
 * @SYMTestCaseDesc Get SIM service table with a USIM.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - USIM service table returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify correct bits set in USIM service table.
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
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg serviceTablePkg( serviceTable );
	TExtEtelRequestStatus reqGetServiceTableStatus( phone, EMobilePhoneGetServiceTable );
	CleanupStack::PushL(reqGetServiceTableStatus);
	
	phone.GetServiceTable( reqGetServiceTableStatus, RMobilePhone::EUSIMServiceTable, serviceTablePkg );

	TInt err = WaitForRequestWithTimeOut(reqGetServiceTableStatus, ETimeMedium);
	ASSERT_EQUALS(err,KErrNone,_L("RMobilePhone::GetServiceTable timed out"));
	
	// ===  Check for SMS service support (something usually present) ===
	// Check RMobilePhone::GetServiceTable with aTable=EUSIMServiceTable returns 
	// iServices9To16 with bit in set of KUstSMS
	ASSERT_BITS_SET( serviceTable.iServices9To16, 
						RMobilePhone::KUstSMS, 
						KNoUnwantedBits,
						_L("RMobilePhone::GetServiceTable returned incorrect bits in iServices9To16") );
							
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1, &reqGetServiceTableStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSubscriberInfo0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSubscriberInfo0008");
	}



CCTSYIntegrationTestSubscriberInfo0009::CCTSYIntegrationTestSubscriberInfo0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSubscriberInfoBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSubscriberInfo0009::GetTestStepName());
	}

CCTSYIntegrationTestSubscriberInfo0009::~CCTSYIntegrationTestSubscriberInfo0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSubscriberInfo0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUBI-0009
 * @SYMFssID BA/CTSY/SUBI-0009
 * @SYMTestCaseDesc Cancel get SIM service table.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Request cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request cancelled.
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
	
	// Request to get service table with RMobilePhone::GetServiceTable with aTable=ESIMServiceTable 
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg serviceTablePkg( serviceTable );
	TExtEtelRequestStatus reqGetServiceTableStatus( phone, EMobilePhoneGetServiceTable );
	CleanupStack::PushL( reqGetServiceTableStatus );
	
	phone.GetServiceTable( reqGetServiceTableStatus, RMobilePhone::ESIMServiceTable, serviceTablePkg );

	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetServiceTable) 
	phone.CancelAsyncRequest(EMobilePhoneGetServiceTable);

	// Check request completes with KErrCancel or KErrNone
	ASSERT_TRUE( KErrNone == reqGetServiceTableStatus.Int() || KErrCancel == reqGetServiceTableStatus.Int(),
						 _L("Cancelled RMobilePhone::GetServiceTable request returned bad status") );
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1,&reqGetServiceTableStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSubscriberInfo0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSubscriberInfo0009");
	}



