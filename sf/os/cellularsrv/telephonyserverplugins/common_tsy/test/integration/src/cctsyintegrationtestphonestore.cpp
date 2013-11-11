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
// Test step definitions for the PhoneStore functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestphonestore.h"



	
CCTSYIntegrationTestPhoneStoreBase::CCTSYIntegrationTestPhoneStoreBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPhoneStoreBase::~CCTSYIntegrationTestPhoneStoreBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestPhoneStore0001::CCTSYIntegrationTestPhoneStore0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneStore0001::GetTestStepName());
	}

CCTSYIntegrationTestPhoneStore0001::~CCTSYIntegrationTestPhoneStore0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneStore0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSTR-0001
 * @SYMFssID BA/CTSY/PSTR-0001
 * @SYMTestCaseDesc Get phone store information from unsupported phone books.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrNotSupported returned for unsupported phone books.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotSupported is returned.
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
	
	// Check RMobilePhone::GetPhoneStoreInfo returns KErrNotSupported when aStoreName is set to the following:	
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg storeInfoPkg(storeInfo);
	TExtEtelRequestStatus phoneStoreInfoRequest(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest);
	
	
	// Check RMobilePhone::GetPhoneStoreInfo returns KErrNotSupported when aStoreName is set to KETelMeAdnPhoneBook 
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest,storeInfoPkg,KETelMeAdnPhoneBook);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest, ETimeLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_EQUALS(phoneStoreInfoRequest.Int(), KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status"));

 	// Check RMobilePhone::GetPhoneStoreInfo returns KErrNotSupported when aStoreName is set to KETelMeDialledPhoneBook 
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest,storeInfoPkg,KETelMeDialledPhoneBook);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."))
	ASSERT_EQUALS(phoneStoreInfoRequest.Int(), KErrNotSupported, _L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status"))
 
  	// Check RMobilePhone::GetPhoneStoreInfo returns KErrNotSupported when aStoreName is set to KETelMeMissedPhoneBook 
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest,storeInfoPkg,KETelMeMissedPhoneBook);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."))
	ASSERT_EQUALS(phoneStoreInfoRequest.Int(), KErrNotSupported, _L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status"))
  
  	// Check RMobilePhone::GetPhoneStoreInfo returns KErrNotSupported when aStoreName is set to KETelMeReceivedPhoneBook 
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest,storeInfoPkg,KETelMeReceivedPhoneBook);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."))
	ASSERT_EQUALS(phoneStoreInfoRequest.Int(), KErrNotSupported, _L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status"))
  
  	// Check RMobilePhone::GetPhoneStoreInfo returns KErrNotSupported when aStoreName is set to KETelCombinedAdnPhoneBook 
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest,storeInfoPkg,KETelCombinedAdnPhoneBook);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."))
	ASSERT_EQUALS(phoneStoreInfoRequest.Int(), KErrNotSupported, _L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status"))
  	
	// Check RMobilePhone::GetPhoneStoreInfo returns KErrNotSupported when aStoreName is set to KETelCombinedSmsStore 
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest,storeInfoPkg,KETelCombinedSmsStore);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."))
	ASSERT_EQUALS(phoneStoreInfoRequest.Int(), KErrNotSupported, _L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status"))
   
  	// Check RMobilePhone::GetPhoneStoreInfo returns KErrNotSupported when aStoreName is set to KETelMeSmsStore 
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest,storeInfoPkg,KETelMeSmsStore);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."))
	ASSERT_EQUALS(phoneStoreInfoRequest.Int(), KErrNotSupported, _L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status"))
   
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1);					//phoneStoreInfoRequest
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneStore0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneStore0001");
	}



CCTSYIntegrationTestPhoneStore0002::CCTSYIntegrationTestPhoneStore0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneStore0002::GetTestStepName());
	}

CCTSYIntegrationTestPhoneStore0002::~CCTSYIntegrationTestPhoneStore0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneStore0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSTR-0002
 * @SYMFssID BA/CTSY/PSTR-0002
 * @SYMTestCaseDesc Get phone store information and capabilities
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Phone store information returned correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone store information returned is correct.
 *
 * @return - TVerdict code
 *
 * $CTSYProblem - Getting the phone store information with
 *  RMobilePhone::GetPhoneStoreInfo is returning
 *  KErrNotSupported. The request is being passed through CTSY
 *  to LTSY, and it is LTSY which is retuning KErrNotSupported.
 *  It is happening for these phonebooks: KETelIccAdnPhoneBook
 *  KETelIccFdnPhoneBook KETelEmergencyNumberStore
 *	KETelOwnNumberStore
 *	KETelIccVoiceMailBox
 *	KETelIccSdnPhoneBook
 *	KETelIccBdnPhoneBook
 *	KETelIccVoiceMailBox
 *	KETelIccSmsStore
 *
 *  These phonebooks were expected to be supported through this IPC.
 */
	{

	//
	// SET UP
	//
	// Get the phone. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
		

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	TExtEtelRequestStatus phoneStoreInfoRequest(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg(phoneStore);
	TInt expectedMinCaps = 0;
	
	// Call RMobilePhone::GetPhoneStoreInfo with aStoreName =  KETelIccAdnPhoneBook 
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest,phoneStorePkg,KETelIccAdnPhoneBook);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest, ETimeLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
			
	if(phoneStoreInfoRequest.Int() != KErrNotSupported)
		{
		ASSERT_EQUALS(phoneStoreInfoRequest.Int(), KErrNone, 
			_L("RMobilePhone::GetPhoneStoreInfo returned error."));
		// Check RMobilePhone::GetPhoneStoreInfo returns iTotalEntries > 0
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries >= 0
		ASSERT_TRUE( phoneStore.iTotalEntries > 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns 0 total entries."));
		ASSERT_TRUE( phoneStore.iUsedEntries >= 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns invalid used entries."));
	
		// When aStoreName = KETelIccAdnPhoneBook 		
		// Check RMobilePhone::GetPhoneStoreInfo returns iType == EPhoneBookStore
		// Check RMobilePhone::GetPhoneStoreInfo returns iCaps of RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent
		ASSERT_EQUALS(phoneStore.iType, RMobilePhoneStore::EPhoneBookStore, 
			_L("RMobilePhone::GetPhoneStoreInfo completed with error."));
		expectedMinCaps = RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent;
		ASSERT_BITS_SET(phoneStore.iCaps, expectedMinCaps, KNoUnwantedBits,
			_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect caps."));
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo for KETelIccAdnPhoneBook returned %d"),
			phoneStoreInfoRequest.Int());
		}
		
	
	// Call RMobilePhone::GetPhoneStoreInfo with aStoreName =  KETelIccFdnPhoneBook 
	TExtEtelRequestStatus phoneStoreInfoRequest2(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest2);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest2,phoneStorePkg,KETelIccFdnPhoneBook);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest2, ETimeLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."))
	
	if(phoneStoreInfoRequest2.Int() != KErrNotSupported)
		{
		ASSERT_EQUALS(phoneStoreInfoRequest2.Int(), KErrNone, 
			_L("RMobilePhone::GetPhoneStoreInfo returned error."));
		// Check RMobilePhone::GetPhoneStoreInfo returns iTotalEntries > 0
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries >= 0
		ASSERT_TRUE( phoneStore.iTotalEntries > 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns 0 total entries."));
		ASSERT_TRUE( phoneStore.iUsedEntries >= 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns invalid used entries."));

		// When aStoreName = KETelIccFdnPhoneBook 
		// Check RMobilePhone::GetPhoneStoreInfo returns iType == EPhoneBookStore
		// Check RMobilePhone::GetPhoneStoreInfo returns iCaps of RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent
		ASSERT_EQUALS(phoneStore.iType, RMobilePhoneStore::EPhoneBookStore, 
			_L("RMobilePhone::GetPhoneStoreInfo completed with error."));
		expectedMinCaps = RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent;
		ASSERT_BITS_SET(phoneStore.iCaps, expectedMinCaps, KNoUnwantedBits,
			_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect caps."));
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo for KETelIccAdnPhoneBook returned %d"),
			phoneStoreInfoRequest.Int());
		}
	
	

	
	// Call RMobilePhone::GetPhoneStoreInfo with aStoreName =  KETelEmergencyNumberStore
	TExtEtelRequestStatus phoneStoreInfoRequest3(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest3);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest3,phoneStorePkg,KETelEmergencyNumberStore);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest3, ETimeLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	
	if(phoneStoreInfoRequest3.Int() != KErrNotSupported)
		{
		ASSERT_EQUALS(phoneStoreInfoRequest3.Int(), KErrNone, 
			_L("RMobilePhone::GetPhoneStoreInfo with KETelEmergencyNumberStore completed with incorrect status"));
		// Check RMobilePhone::GetPhoneStoreInfo returns iTotalEntries > 0
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries >= 0
		ASSERT_TRUE( phoneStore.iTotalEntries > 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns 0 total entries."));
		ASSERT_TRUE( phoneStore.iUsedEntries >= 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns invalid used entries."));
		
		// When aStoreName = KETelEmergencyNumberStore 
		// Check RMobilePhone::GetPhoneStoreInfo returns iType == EEmergencyNumberStore
		// Check RMobilePhone::GetPhoneStoreInfo returns iCaps of RMobilePhoneStore::KCapsReadAccess
		ASSERT_EQUALS(phoneStore.iType, RMobilePhoneStore::EEmergencyNumberStore, 
			_L("RMobilePhone::GetPhoneStoreInfo completed with error."));
		expectedMinCaps = RMobilePhoneStore::KCapsReadAccess;
		ASSERT_BITS_SET(phoneStore.iCaps, expectedMinCaps, KNoUnwantedBits,
			_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect caps."));
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo for KETelIccAdnPhoneBook returned %d"),
			phoneStoreInfoRequest.Int());
		}
		
	
	
	// Call RMobilePhone::GetPhoneStoreInfo with aStoreName =  KETelOwnNumberStore 
	TExtEtelRequestStatus phoneStoreInfoRequest4(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest4);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest4,phoneStorePkg,KETelOwnNumberStore);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest4, ETimeLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
		
	if(phoneStoreInfoRequest4.Int() != KErrNotSupported)
		{
		ASSERT_EQUALS(phoneStoreInfoRequest4.Int(), KErrNone, 
			_L("RMobilePhone::GetPhoneStoreInfo returned error."));
		// Check RMobilePhone::GetPhoneStoreInfo returns iTotalEntries > 0
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries >= 0
		ASSERT_TRUE( phoneStore.iTotalEntries > 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns 0 total entries."));
		ASSERT_TRUE( phoneStore.iUsedEntries >= 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns invalid used entries."));
		
		// When aStoreName = KETelOwnNumberStore 
		// Check RMobilePhone::GetPhoneStoreInfo returns iType == EOwnNumberStore
		// Check RMobilePhone::GetPhoneStoreInfo returns iCaps of RMobilePhoneStore::KCapsIndividualEntry | RMobilePhoneStore::KCapsReadAccess        | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEent | RMobilePhoneStore::KCapsWholeStore
		ASSERT_EQUALS(phoneStore.iType, RMobilePhoneStore::EOwnNumberStore, 
			_L("RMobilePhone::GetPhoneStoreInfo completed with error."));
		expectedMinCaps = RMobilePhoneStore::KCapsIndividualEntry | RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent | RMobilePhoneStore::KCapsWholeStore;
		ASSERT_BITS_SET(phoneStore.iCaps, expectedMinCaps, KNoUnwantedBits,
			_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect caps."));
		
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo for KETelIccAdnPhoneBook returned %d"),
			phoneStoreInfoRequest.Int());
		}
	
	
	
	
	
	// Call RMobilePhone::GetPhoneStoreInfo with aStoreName =  KETelIccVoiceMailBox 
	TExtEtelRequestStatus phoneStoreInfoRequest5(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest5);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest5,phoneStorePkg,KETelIccVoiceMailBox);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest5, ETimeLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	
	if(phoneStoreInfoRequest5.Int() != KErrNotSupported)
		{
		ASSERT_EQUALS(phoneStoreInfoRequest5.Int(), KErrNone, 
			_L("RMobilePhone::GetPhoneStoreInfo returned error."));
		// Check RMobilePhone::GetPhoneStoreInfo returns iTotalEntries > 0
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries >= 0
		ASSERT_TRUE( phoneStore.iTotalEntries > 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns 0 total entries."));
		ASSERT_TRUE( phoneStore.iUsedEntries >= 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns invalid used entries."));
		
		// When aStoreName = KETelIccVoiceMailBox 
		// Check RMobilePhone::GetPhoneStoreInfo returns iType == EPhoneBookStore
		// Check RMobilePhone::GetPhoneStoreInfo returns iCaps of RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent
		ASSERT_EQUALS(phoneStore.iType, RMobilePhoneStore::EPhoneBookStore, 
			_L("RMobilePhone::GetPhoneStoreInfo completed with error."));
		expectedMinCaps = RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent;
		ASSERT_BITS_SET(phoneStore.iCaps, expectedMinCaps, KNoUnwantedBits,
			_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect caps."));
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo for KETelIccAdnPhoneBook returned %d"),
			phoneStoreInfoRequest.Int());
		
		}

	
	
	
	// Call RMobilePhone::GetPhoneStoreInfo with aStoreName =  KETelIccSdnPhoneBook 
	TExtEtelRequestStatus phoneStoreInfoRequest6(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest6);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest6,phoneStorePkg,KETelIccSdnPhoneBook);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest6, ETimeLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	
	if(phoneStoreInfoRequest6.Int() != KErrNotSupported)
		{
		ASSERT_EQUALS(phoneStoreInfoRequest6.Int(), KErrNone, 
			_L("RMobilePhone::GetPhoneStoreInfo returned error."));
		// Check RMobilePhone::GetPhoneStoreInfo returns iTotalEntries > 0
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries >= 0
		ASSERT_TRUE( phoneStore.iTotalEntries > 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns 0 total entries."));
		ASSERT_TRUE( phoneStore.iUsedEntries >= 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns invalid used entries."));
		
		// When aStoreName = KETelIccSdnPhoneBook
		// Check RMobilePhone::GetPhoneStoreInfo returns iType == EPhoneBookStore
		// Check RMobilePhone::GetPhoneStoreInfo returns iCaps of RMobilePhoneStore::KCapsReadAccess
		ASSERT_EQUALS(phoneStore.iType, RMobilePhoneStore::EPhoneBookStore, 
			_L("RMobilePhone::GetPhoneStoreInfo completed with error."));
		expectedMinCaps = RMobilePhoneStore::KCapsReadAccess;
		ASSERT_BITS_SET(phoneStore.iCaps, expectedMinCaps, KNoUnwantedBits,
			_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect caps."));
		
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo for KETelIccAdnPhoneBook returned %d"),
			phoneStoreInfoRequest.Int());
		
		}

				
	
	
	
	// Call RMobilePhone::GetPhoneStoreInfo with aStoreName =  KETelIccBdnPhoneBook
	TExtEtelRequestStatus phoneStoreInfoRequest7(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest7);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest7,phoneStorePkg,KETelIccBdnPhoneBook);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest7, ETimeLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	
	if(phoneStoreInfoRequest7.Int() != KErrNotSupported)
		{
		ASSERT_EQUALS(phoneStoreInfoRequest7.Int(), KErrNone, 
			_L("RMobilePhone::GetPhoneStoreInfo returned error."));
		// Check RMobilePhone::GetPhoneStoreInfo returns iTotalEntries > 0
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries >= 0
		ASSERT_TRUE( phoneStore.iTotalEntries > 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns 0 total entries."));
		ASSERT_TRUE( phoneStore.iUsedEntries >= 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns invalid used entries."));
			
		// When aStoreName = KETelIccBdnPhoneBook 
		// Check RMobilePhone::GetPhoneStoreInfo returns iType == EPhoneBookStore
		// Check RMobilePhone::GetPhoneStoreInfo returns iCaps of 0
		ASSERT_EQUALS(phoneStore.iType, RMobilePhoneStore::EPhoneBookStore, 
			_L("RMobilePhone::GetPhoneStoreInfo completed with error."));
		expectedMinCaps =0x00;
		ASSERT_BITS_SET(phoneStore.iCaps, expectedMinCaps,KNoUnwantedBits,
			_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect caps."));
		
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo for KETelIccAdnPhoneBook returned %d"),
			phoneStoreInfoRequest7.Int());
		
		}
		
	

	
	
	// Call RMobilePhone::GetPhoneStoreInfo with aStoreName =  KETelIccSmsStore 
	TExtEtelRequestStatus phoneStoreInfoRequest8(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest8);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest8,phoneStorePkg,KETelIccSmsStore);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest8, ETimeVeryLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	
	if(phoneStoreInfoRequest8.Int() != KErrNotSupported)
		{
		ASSERT_EQUALS(phoneStoreInfoRequest7.Int(), KErrNone, 
			_L("RMobilePhone::GetPhoneStoreInfo returned error."));
		// Check RMobilePhone::GetPhoneStoreInfo returns iTotalEntries > 0
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries >= 0
		ASSERT_TRUE( phoneStore.iTotalEntries > 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns 0 total entries."));
		ASSERT_TRUE( phoneStore.iUsedEntries >= 0, 
			_L("RMobilePhone::GetPhoneStoreInfo returns invalid used entries."));
		
		// When aStoreName = KETelIccSmsStore 
		// Check RMobilePhone::GetPhoneStoreInfo returns iType == RMobilePhoneStore::EShortMessageStore
		// Check RMobilePhone::GetPhoneStoreInfo returns iCaps of RMobilePhoneStore::KCapsIndividualEntry | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsNotifyEvent | RMobilePhoneStore::KCapsWholeStore
		ASSERT_EQUALS(phoneStore.iType, RMobilePhoneStore::EShortMessageStore, 
			_L("RMobilePhone::GetPhoneStoreInfo completed with error."));
		expectedMinCaps = RMobilePhoneStore::KCapsIndividualEntry | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsNotifyEvent | RMobilePhoneStore::KCapsWholeStore;
		ASSERT_BITS_SET(phoneStore.iCaps, expectedMinCaps, KNoUnwantedBits,
			_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect caps."));

		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo for KETelIccAdnPhoneBook returned %d"),
			phoneStoreInfoRequest.Int());
		
		}
			
	
	
	//
	// TEST END
	//

    StartCleanup();
	
	//phoneStoreInfoRequest
	//phoneStoreInfoRequest2
	//phoneStoreInfoRequest3
	//phoneStoreInfoRequest4
	//phoneStoreInfoRequest5
	//phoneStoreInfoRequest6
	//phoneStoreInfoRequest7
	//phoneStoreInfoRequest8
	
	CleanupStack::PopAndDestroy(8, &phoneStoreInfoRequest);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneStore0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneStore0002");
	}



CCTSYIntegrationTestPhoneStore0003::CCTSYIntegrationTestPhoneStore0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneStore0003::GetTestStepName());
	}

CCTSYIntegrationTestPhoneStore0003::~CCTSYIntegrationTestPhoneStore0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneStore0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PSTR-0003
 * @SYMFssID BA/CTSY/PSTR-0003
 * @SYMTestCaseDesc Cancel getting phone store information.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel returned or KErrNone if it was too late to cancel.
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
	
	
	// Get phone store info. with RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelIccAdnPhoneBook 
	TExtEtelRequestStatus phoneStoreInfoRequest(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg(phoneStore);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest,phoneStorePkg,KETelIccAdnPhoneBook);

	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);

	// Check request completes with KErrCancel or KErrNone or KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_TRUE(phoneStoreInfoRequest.Int() == KErrCancel || phoneStoreInfoRequest.Int() == KErrNone || phoneStoreInfoRequest.Int() == KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo request completed with error."));
	
	
	
	
	// Repeat test with aStoreName = 
	// Get phone store info. with RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelIccFdnPhoneBook, 
	TExtEtelRequestStatus phoneStoreInfoRequest2(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest2);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore2;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg2(phoneStore2);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest2,phoneStorePkg2,KETelIccFdnPhoneBook);
	
	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);
	
	// Check request completes with KErrCancel or KErrNone or KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest2, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
		
	
	ASSERT_TRUE(phoneStoreInfoRequest2.Int() == KErrCancel || phoneStoreInfoRequest2.Int() == KErrNone || phoneStoreInfoRequest2.Int() == KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo request completed with error."));


	
	// Get phone store info. with RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelEmergencyNumberStore, 
	TExtEtelRequestStatus phoneStoreInfoRequest3(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest3);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore3;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg3(phoneStore3);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest3,phoneStorePkg3,KETelEmergencyNumberStore);

	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);
	
	// Check request completes with KErrCancel or KErrNone or KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest3, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_TRUE(phoneStoreInfoRequest3.Int() == KErrCancel || phoneStoreInfoRequest3.Int() == KErrNone || phoneStoreInfoRequest3.Int() == KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo request completed with error."));

	
	
	// Get phone store info. with RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelOwnNumberStore
	TExtEtelRequestStatus phoneStoreInfoRequest4(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest4);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore4;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg4(phoneStore4);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest4,phoneStorePkg4,KETelOwnNumberStore);

	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);
	
	// Check request completes with KErrCancel or KErrNone or KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest4, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_TRUE(phoneStoreInfoRequest4.Int() == KErrCancel || phoneStoreInfoRequest4.Int() == KErrNone || phoneStoreInfoRequest4.Int() == KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo request completed with error."));



	// Get phone store info. with RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelIccVoiceMailBox
	TExtEtelRequestStatus phoneStoreInfoRequest5(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest5);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore5;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg5(phoneStore5);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest5,phoneStorePkg5,KETelIccVoiceMailBox);

	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);

	// Check request completes with KErrCancel or KErrNone or KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest5, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_TRUE(phoneStoreInfoRequest5.Int() == KErrCancel || phoneStoreInfoRequest5.Int() == KErrNone || phoneStoreInfoRequest5.Int() == KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo request completed with error."));




	// Get phone store info. with RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelIccSdnPhoneBook
	TExtEtelRequestStatus phoneStoreInfoRequest6(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest6);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore6;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg6(phoneStore6);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest6,phoneStorePkg6,KETelIccSdnPhoneBook);
	
	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);
	
	// Check request completes with KErrCancel or KErrNone or KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest6, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_TRUE(phoneStoreInfoRequest6.Int() == KErrCancel || phoneStoreInfoRequest6.Int() == KErrNone || phoneStoreInfoRequest6.Int() == KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo request completed with error."));

	
	
	
	
	
	// Get phone store info. with RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelIccBdnPhoneBook, 
	TExtEtelRequestStatus phoneStoreInfoRequest7(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest7);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore7;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg7(phoneStore7);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest7,phoneStorePkg7,KETelIccBdnPhoneBook);
	
	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);
	
	// Check request completes with KErrCancel or KErrNone or KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest7, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_TRUE(phoneStoreInfoRequest7.Int() == KErrCancel || phoneStoreInfoRequest7.Int() == KErrNone || phoneStoreInfoRequest7.Int() == KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo request completed with error."));

	
	
		
	// Get phone store info. with RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelIccVoiceMailBox, 
	TExtEtelRequestStatus phoneStoreInfoRequest8(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest8);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore8;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg8(phoneStore8);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest8,phoneStorePkg8,KETelIccVoiceMailBox);

	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);

	// Check request completes with KErrCancel or KErrNone or KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest8, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_TRUE(phoneStoreInfoRequest8.Int() == KErrCancel || phoneStoreInfoRequest8.Int() == KErrNone || phoneStoreInfoRequest8.Int() == KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo request completed with error."));



	// Get phone store info. with RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelIccSmsStore 
	TExtEtelRequestStatus phoneStoreInfoRequest9(phone,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(phoneStoreInfoRequest9);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStore9;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStorePkg9(phoneStore8);
	phone.GetPhoneStoreInfo(phoneStoreInfoRequest9,phoneStorePkg9,KETelIccSmsStore);

	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);

	// Check request completes with KErrCancel or KErrNone or KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoRequest9, ETimeMedium),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_TRUE(phoneStoreInfoRequest9.Int() == KErrCancel || phoneStoreInfoRequest9.Int() == KErrNone || phoneStoreInfoRequest9.Int() == KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo request completed with error."));

	
	//
	// TEST END
	//


    StartCleanup();
	
	//phoneStoreInfoRequest
	//phoneStoreInfoRequest2
	//phoneStoreInfoRequest3
	//phoneStoreInfoRequest4
	//phoneStoreInfoRequest5
	//phoneStoreInfoRequest6
	//phoneStoreInfoRequest7
	//phoneStoreInfoRequest8
	//phoneStoreInfoRequest9
	CleanupStack::PopAndDestroy(9);					
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneStore0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneStore0003");
	}



