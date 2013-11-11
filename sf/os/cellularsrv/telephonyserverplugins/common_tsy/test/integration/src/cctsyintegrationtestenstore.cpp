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
// Test step definitions for the ENStore functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestenstore.h"

#include "cetelsessionmgr.h"
#include "tetelrequeststatus.h"
#include "listretrievalhelpers.h"

	
CCTSYIntegrationTestENStoreBase::CCTSYIntegrationTestENStoreBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iSimTsyTestHelper(*this), iPhoneTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestENStoreBase::~CCTSYIntegrationTestENStoreBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestENStore0001::CCTSYIntegrationTestENStore0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0001::GetTestStepName());
	}

CCTSYIntegrationTestENStore0001::~CCTSYIntegrationTestENStore0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0001
 * @SYMFssID BA/CTSY/EN-0001
 * @SYMTestCaseDesc Get EN store info.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Store information returned correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify EN store info. retrieved is valid.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, KIccAdnPhoneBook);
	
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,KIccAdnPhoneBook);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Get EN store info. using RMobilePhoneStore::GetInfo with RMobilePhoneStore::TMobilePhoneStoreInfoV1 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);
	
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg (info);
	TExtEtelRequestStatus getEnStoreInfoStatus(enstore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(getEnStoreInfoStatus);
	enstore.GetInfo(getEnStoreInfoStatus, infoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getEnStoreInfoStatus, ETimeMedium), KErrNone,_L("RMobileENStore::GetInfo timed out"));
	ASSERT_EQUALS(getEnStoreInfoStatus.Int(), KErrNone, _L("RMobileENStore::GetInfo returned an error"));
	
	// iName == KETelEmergencyNumberStore 
	ASSERT_EQUALS_DES16(info.iName, KETelEmergencyNumberStore, _L("RMobileENStore::GetInfo returned an unexpected name of iName "));

	// iType == RMobilePhoneStore::EEmergencyNumberStore 
	ASSERT_EQUALS(info.iType, RMobilePhoneStore::EEmergencyNumberStore , _L("RMobileENStore::GetInfo returned an unexpected type of iType "));

	// iTotalEntries >= -1 
	ASSERT_TRUE(info.iTotalEntries >= -1, _L("RMobileENStore::GetInfo returned iTotalEntries < -1  "));

	// iUsedEntries >= -1 
	ASSERT_TRUE(info.iUsedEntries >= -1, _L("RMobileENStore::GetInfo returned iUsedEntries < -1  "));

	// iCaps == RMobilePhoneStore::KCapsReadAccess 
	TInt32 expectedCaps =  RMobilePhoneStore::KCapsReadAccess;
	ASSERT_BITS_SET(info.iCaps, expectedCaps, KNoUnwantedBits, _L("RMobileENStore::GetInfo returned incorrect Store Info Caps"));
	
	//
	// TEST END
	//

    StartCleanup();
	
		
	// Pop
	// getEnStoreInfoStatus
	CleanupStack::PopAndDestroy(&getEnStoreInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0001");
	}



CCTSYIntegrationTestENStore0002::CCTSYIntegrationTestENStore0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0002::GetTestStepName());
	}

CCTSYIntegrationTestENStore0002::~CCTSYIntegrationTestENStore0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0002
 * @SYMFssID BA/CTSY/EN-0002
 * @SYMTestCaseDesc Read entries from the EN store.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Entries read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries are read correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, KIccAdnPhoneBook);
	
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,KIccAdnPhoneBook);

	// Get EN store subsession. 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess
	TExtEtelRequestStatus getEnStoreInfoStatus(enstore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(getEnStoreInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg (info);
	enstore.GetInfo(getEnStoreInfoStatus, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getEnStoreInfoStatus, ETimeMedium),KErrNone,_L("RMobileENStore::GetInfo timed out"));
	CHECK_EQUALS_L(getEnStoreInfoStatus.Int(), KErrNone, _L("RMobileENStore::GetInfo returned an error"));
	TInt32 expectedCaps =  RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(info.iCaps, expectedCaps, KNoUnwantedBits, _L("RMobileENStore::GetInfo returned incorrect Store Info Caps"));

	// Check Used Entries are greater than zero
	CHECK_TRUE_L(info.iUsedEntries > 0, _L("RMobileENStore::GetInfo returned wrong iUsedEntries, should be > 0"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Read entry from the first slot with RMobileENStore::TMobileENEntryV1 as aEntry. 
	RMobileENStore::TMobileENEntryV1 readFirstEntry;
	RMobileENStore::TMobileENEntryV1Pckg readFirstPckgEntry(readFirstEntry);
	readFirstEntry.iIndex = 1; 
	readFirstEntry.iAlphaId.Zero();
	readFirstEntry.iNetworkSpecific = ETrue;
	TExtEtelRequestStatus readStatus (enstore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(readStatus);
	enstore.Read(readStatus,readFirstPckgEntry);		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileENStore::Read returned an error"));

	// Check iNumber is valid with length>0.
	ASSERT_TRUE(readFirstEntry.iNumber.Length()>0, _L("RMobileENStore::Read First Read Entry has invalid iNumber"));
	
	// Check iNetworkSpecific returns false.
	ASSERT_TRUE(!(readFirstEntry.iNetworkSpecific), _L("RMobileENStore::Read First Read Entry has the wrong iNetworkSpecific"));
		
	// If iNetworkSpecific == ETrue: 
	if(readFirstEntry.iNetworkSpecific)
		{
		// Check iCountryCode is valid with length>0.
		ASSERT_TRUE(readFirstEntry.iCountryCode.Length()>0, _L("RMobileENStore::Read First Read Entry has invalid iCountryCode"));
		
		// Check iIdentity is valid with length>0.
		ASSERT_TRUE(readFirstEntry.iIdentity.Length()>0, _L("RMobileENStore::Read First Read Entry has invalid iIdentity"));
		
		// Check iMode is RMobilePhone::ENetworkModeGsm
		ASSERT_EQUALS(readFirstEntry.iMode, RMobilePhone::ENetworkModeGsm, _L("RMobileENStore::Read First Read Entry has the wrong iMode"));
		}

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
    // getEnStoreInfoStatus
	// readStatus
    CleanupStack::PopAndDestroy(2, &getEnStoreInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0002");
	}



CCTSYIntegrationTestENStore0003::CCTSYIntegrationTestENStore0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0003::GetTestStepName());
	}

CCTSYIntegrationTestENStore0003::~CCTSYIntegrationTestENStore0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0003
 * @SYMFssID BA/CTSY/EN-0003
 * @SYMTestCaseDesc Read entry that is not present in the EN store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - KErrNotFound returned when reading a non-existent entry.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get EN store subsession. 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess
	TExtEtelRequestStatus getEnStoreInfoStatus(enstore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(getEnStoreInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg(info);
	enstore.GetInfo(getEnStoreInfoStatus, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getEnStoreInfoStatus, ETimeMedium), KErrNone,_L("RMobileENStore::GetInfo timed out"));
	CHECK_EQUALS_L(getEnStoreInfoStatus.Int(), KErrNone, _L("RMobileENStore::GetInfo returned an error"));
	 
	TInt32 expectedCaps =  RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(info.iCaps, expectedCaps, KNoUnwantedBits, _L("RMobileENStore::GetInfo returned incorrect Store Info Caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Read an entry from a valid index. 
	RMobileENStore::TMobileENEntryV1 readEntry;
	RMobileENStore::TMobileENEntryV1Pckg readPckgEntry(readEntry);
	TExtEtelRequestStatus readStatus(enstore, EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);
	readEntry.iIndex = info.iTotalEntries; 
	enstore.Read(readStatus,readPckgEntry);

	// Check KErrNotFound is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone, _L("RMobileENStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNotFound, _L("RMobileENStore::Read returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
    // getEnStoreInfoStatus
	// readStatus

	CleanupStack::PopAndDestroy(2, &getEnStoreInfoStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0003");
	}



CCTSYIntegrationTestENStore0004::CCTSYIntegrationTestENStore0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0004::GetTestStepName());
	}

CCTSYIntegrationTestENStore0004::~CCTSYIntegrationTestENStore0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0004
 * @SYMFssID BA/CTSY/EN-0004
 * @SYMTestCaseDesc Read entries from the EN store specifying invalid indexes.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Error returned on read.
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

	
	// Get EN store subsession. 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	TExtEtelRequestStatus getEnStoreInfoStatus(enstore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(getEnStoreInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg(info);
	enstore.GetInfo(getEnStoreInfoStatus, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getEnStoreInfoStatus, ETimeMedium), KErrNone,_L("RMobileENStore::GetInfo timed out"));
	CHECK_EQUALS_L(getEnStoreInfoStatus.Int(), KErrNone, _L("RMobileENStore::GetInfo returned an error"));
 
	TInt32 expectedCaps =  RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(info.iCaps, expectedCaps, KNoUnwantedBits, _L("RMobileENStore::GetInfo returned incorrect Store Info Caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Read an entry with an invalid slot number ===

	// Read a single entry from the phone book using aIndex = -1 
	RMobileENStore::TMobileENEntryV1 readEntry;
	RMobileENStore::TMobileENEntryV1Pckg readPckgEntry(readEntry);
	TExtEtelRequestStatus readStatus(enstore, EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);
	readEntry.iIndex = -1; 
	enstore.Read(readStatus,readPckgEntry);
	
	// Check KErrArgument is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone, _L("RMobileENStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrArgument, _L("RMobileENStore::Read did not return KErrArgument as expected"));
	
	// Read a single entry from the phone book using aIndex = -10 
	readEntry.iIndex=-10;
	enstore.Read (readStatus, readPckgEntry);
		
	// Check KErrArgument is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone, _L("RMobileENStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrArgument, _L("RMobileENStore::Read did not return KErrArgument as expected"));
	
	// Read a single entry from the phone book using aIndex = -50 
	readEntry.iIndex=-50;
	enstore.Read (readStatus, readPckgEntry);
		
	// Check KErrArgument is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone, _L("RMobileENStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrArgument, _L("RMobileENStore::Read did not return KErrArgument as expected"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
    // getEnStoreInfoStatus
	// readStatus
	CleanupStack::PopAndDestroy(2, &getEnStoreInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0004");
	}



CCTSYIntegrationTestENStore0005::CCTSYIntegrationTestENStore0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0005::GetTestStepName());
	}

CCTSYIntegrationTestENStore0005::~CCTSYIntegrationTestENStore0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0005
 * @SYMFssID BA/CTSY/EN-0005
 * @SYMTestCaseDesc Cancel delete all entries from the EN store.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Some entries deleted, request cancelled successfully. Store not empty.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotSupported returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
			
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Get EN store subsession. 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Call RMobilePhoneStore::DeleteAll 
	TExtEtelRequestStatus deleteAllStatus(enstore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteAllStatus);
	enstore.DeleteAll(deleteAllStatus);

	// Call RTelSubSessionBase::CancelAsyncRequest (EMobilePhoneStoreDeleteAll); 
	mobilePhone.CancelAsyncRequest (EMobilePhoneStoreDeleteAll); 

	// Check status of RMobilePhoneStore::Delete is KErrNotSupported.
	ASSERT_EQUALS(deleteAllStatus.Int(), KErrNotSupported, _L("RMobileENStore::DeleteAll returned other than KErrNotSupported") );
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// deleteAllStatus
	
	CleanupStack::PopAndDestroy(&deleteAllStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0005");
	}



CCTSYIntegrationTestENStore0006::CCTSYIntegrationTestENStore0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0006::GetTestStepName());
	}

CCTSYIntegrationTestENStore0006::~CCTSYIntegrationTestENStore0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0006
 * @SYMFssID BA/CTSY/EN-0006
 * @SYMTestCaseDesc Attempt to read emergency number into a wrongly sized buffer
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrArgument returned on attempt to read into an invalid buffer.
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

	
	// Get EN store subsession. 

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 

	// Ensure the EN store is not empty. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Read an entry using RMobilePhoneStore::Read with aEntry as a TBuf8<1>. 

	// Check KErrArgument is returned.

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0006");
	}



CCTSYIntegrationTestENStore0007::CCTSYIntegrationTestENStore0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0007::GetTestStepName());
	}

CCTSYIntegrationTestENStore0007::~CCTSYIntegrationTestENStore0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0007
 * @SYMFssID BA/CTSY/EN-0007
 * @SYMTestCaseDesc Read from the EN store with no SIM present
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Error returned on attempt to read from EN store when there is no SIM present.
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

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Get EN store subsession. 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);

	// Ensure RMobilePhoneStore::GetInfo returns KErrGsmMMServiceOptionTemporaryOutOfOrder  
	TExtEtelRequestStatus getEnStoreInfoStatus(enstore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(getEnStoreInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 mobilePhoneStoreinfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg mobilePhoneStoreinfoPckg(mobilePhoneStoreinfo);
	enstore.GetInfo(getEnStoreInfoStatus, mobilePhoneStoreinfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getEnStoreInfoStatus, ETimeMedium), KErrNone, _L("RMobileENStore::GetInfo timed out"));
	CHECK_EQUALS_L(getEnStoreInfoStatus.Int(),  KErrGsmMMServiceOptionTemporaryOutOfOrder , _L("RMobileENStore::GetInfo returned an error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Remove the SIM. 
	DisplayUserInteractionPromptL(_L("Please remove the SIM card before running this test"), ETimeMedium);

	// Attempt to read an entry from the EN store. 
	RMobileENStore::TMobileENEntryV1 readEntry;
	RMobileENStore::TMobileENEntryV1Pckg readPckgEntry(readEntry);
	TExtEtelRequestStatus readEnEntryStatus(enstore, EMobilePhoneStoreRead);	
	CleanupStack::PushL(readEnEntryStatus);
	readEntry.iIndex = mobilePhoneStoreinfo.iTotalEntries; 
	enstore.Read(readEnEntryStatus, readPckgEntry);

	// Verify error is returned. 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readEnEntryStatus, ETimeMedium), KErrNone, _L("RMobileENStore::Read timed out"));
	ASSERT_TRUE(readEnEntryStatus.Int() != KErrNone, _L("RMobileENStore::Read did not return error as expected"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getEnStoreInfoStatus
	// readEnEntryStatus
	CleanupStack::PopAndDestroy(2, &getEnStoreInfoStatus);	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0007");
	}



CCTSYIntegrationTestENStore0008::CCTSYIntegrationTestENStore0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0008::GetTestStepName());
	}

CCTSYIntegrationTestENStore0008::~CCTSYIntegrationTestENStore0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0008
 * @SYMFssID BA/CTSY/EN-0008
 * @SYMTestCaseDesc Cancel write one entry to EN Store.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel
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

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Get EN store subsession. 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobilePhoneStore::Write specifying a valid entry to write. 
	TExtEtelRequestStatus writeStatus (enstore, EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	RMobileENStore::TMobileENEntryV1 entry;
	RMobileENStore::TMobileENEntryV1Pckg pckgEntry(entry);
	entry.iIndex = 1;
	enstore.Write(writeStatus,pckgEntry);
	
	// Call RTelSubSessionBase::CancelAsyncRequest (EMobilePhoneStoreWrite); 
	mobilePhone.CancelAsyncRequest(EMobilePhoneStoreWrite);
	
	// Check status of RMobilePhoneStore::Write is KErrNotSupported.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone, _L("RMobileENStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNotSupported, _L("RMobileENStore::Write did not return KErrNotSupported"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// writeStatus
	
	CleanupStack::PopAndDestroy(&writeStatus);	
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0008");
	}



CCTSYIntegrationTestENStore0009::CCTSYIntegrationTestENStore0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0009::GetTestStepName());
	}

CCTSYIntegrationTestENStore0009::~CCTSYIntegrationTestENStore0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0009
 * @SYMFssID BA/CTSY/EN-0009
 * @SYMTestCaseDesc Cancel delete one entry from EN Store location by index
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel, entry not deleted.
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
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);

	// No need to write an entry. Deleting is cancelled.
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Index to delete does not have to exist due deletion is cancelled
	TInt indexToDelete = 1; 

	// Call RMobilePhoneStore::Delete 		
	TExtEtelRequestStatus deleteStatus (enstore, EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStatus);
	enstore.Delete(deleteStatus, indexToDelete);

	// Call RTelSubSessionBase::CancelAsyncRequest (EMobilePhoneStoreDelete); 
	mobilePhone.CancelAsyncRequest (EMobilePhoneStoreDelete); 
	
	// Check status of RMobilePhoneStore::Delete is KErrNotSupported.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileENStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(), KErrNotSupported, _L("RMobileENStore::Delete did not return KErrNotSupported"));
		
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// deleteStatus
	CleanupStack::PopAndDestroy(&deleteStatus);	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0009");
	}



CCTSYIntegrationTestENStore0010::CCTSYIntegrationTestENStore0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0010::GetTestStepName());
	}

CCTSYIntegrationTestENStore0010::~CCTSYIntegrationTestENStore0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0010
 * @SYMFssID BA/CTSY/EN-0010
 * @SYMTestCaseDesc Cancel notify EN Store event
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel
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
	
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Call RMobilePhoneStore::NotifyStoreEvent 
	TExtEtelRequestStatus notifyStatus(enstore, EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL(notifyStatus);
	TInt index;
	TUint32 event;
	enstore.NotifyStoreEvent(notifyStatus, event, index);
	
	// Call RTelSubSessionBase::CancelAsyncRequest (EMobilePhoneStoreNotifyStoreEvent); 
	phone.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);

	// Check status of RMobilePhoneStore::NotifyStoreEvent is KErrNotSupported.
	ASSERT_EQUALS(notifyStatus.Int(), KErrNotSupported, _L("RMobileENStore::NotifyStoreEvent did not return KErrNotSupported"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyStatus
	CleanupStack::PopAndDestroy(&notifyStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0010");
	}



CCTSYIntegrationTestENStore0011::CCTSYIntegrationTestENStore0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0011::GetTestStepName());
	}

CCTSYIntegrationTestENStore0011::~CCTSYIntegrationTestENStore0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0011
 * @SYMFssID BA/CTSY/EN-0011
 * @SYMTestCaseDesc Read all EN store entries
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - All entries read correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify valid entries are read from the EN store.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, KIccAdnPhoneBook);
	
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,KIccAdnPhoneBook);

	// Get EN store subsession. 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess
	TExtEtelRequestStatus getEnStoreInfoStatus(enstore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(getEnStoreInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg (info);
	enstore.GetInfo(getEnStoreInfoStatus, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getEnStoreInfoStatus, ETimeMedium),KErrNone,_L("RMobileENStore::GetInfo timed out"));
	CHECK_EQUALS_L(getEnStoreInfoStatus.Int(), KErrNone, _L("RMobileENStore::GetInfo returned an error"));
	TInt32 expectedCaps =  RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(info.iCaps, expectedCaps, KNoUnwantedBits, _L("RMobileENStore::GetInfo returned incorrect Store Info Caps"));

	// Check Used Entries are greater than zero
	CHECK_TRUE_L(info.iUsedEntries > 0, _L("RMobileENStore::GetInfo returned wrong iUsedEntries, should be > 0"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Use helper class for CRetrieveMobilePhoneENList to read all of the entries from the EN store. 
	CRetrieveMobilePhoneENListExec* enListExec = CRetrieveMobilePhoneENListHelper::NewL(enstore);	
	CleanupStack::PushL(enListExec);
	TInt err;
	CMobilePhoneENList* enList = enListExec ->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneENList::RetrieveListL is not able to get the list"));
	ASSERT_TRUE(enList != NULL, _L("CRetrieveMobilePhoneENList::RetrieveListL ENList is NULL"));
	TEST_CHECK_POINT_L(_L("CRetrieveMobilePhoneENList::DoGetList enList is NULL"));

	// Check the number of entries returned > 0
	ASSERT_TRUE(enList->Enumerate()>0, _L("CRetrieveMobilePhoneENList::Enumerate return value not greater than 0 "));

	// ===  Check each entry in the returned CMobilePhoneENList. ===
	
	// For each entry: 
	RMobileENStore::TMobileENEntryV1 entry;
	RMobileENStore::TMobileENEntryV1Pckg entryPckg(entry);
	for(TInt i=0;i<enList->Enumerate();i++)
		{
		entry = enList->GetEntryL(i);
				
		// Check iNumber has length > 0
		ASSERT_TRUE(entry.iNumber.Length()>0, _L("CRetrieveMobilePhoneENList::GetEntryL Entry iNumber length not greater than 0"));
		}
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getEnStoreInfoStatus
	// enListExec
	CleanupStack::PopAndDestroy(2, &getEnStoreInfoStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0011");
	}



CCTSYIntegrationTestENStore0012::CCTSYIntegrationTestENStore0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0012::GetTestStepName());
	}

CCTSYIntegrationTestENStore0012::~CCTSYIntegrationTestENStore0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0012
 * @SYMFssID BA/CTSY/EN-0012
 * @SYMTestCaseDesc Cancel read all EN store entries.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request is cancelled and some but not all entries are read before cancellation.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, KIccAdnPhoneBook);
	
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,KIccAdnPhoneBook);

	// Get EN store subsession. 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess
	TExtEtelRequestStatus getEnStoreInfoStatus(enstore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(getEnStoreInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg (info);
	enstore.GetInfo(getEnStoreInfoStatus, infoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getEnStoreInfoStatus, ETimeMedium),KErrNone,_L("RMobileENStore::GetInfo timed out"));
	CHECK_EQUALS_L(getEnStoreInfoStatus.Int(), KErrNone, _L("RMobileENStore::GetInfo returned an error"));
	TInt32 expectedCaps =  RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(info.iCaps, expectedCaps, KNoUnwantedBits, _L("RMobileENStore::GetInfo returned incorrect Store Info Caps"));

	// Check Used Entries are greater than zero
	CHECK_TRUE_L(info.iUsedEntries > 0, _L("RMobileENStore::GetInfo returned wrong iUsedEntries, should be > 0"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Find out how many entries are used in the phone book ===

	// Use helper class for  CRetrieveMobilePhoneENList to read all of the entries from the EN store. 
	TInt err;
	CRetrieveMobilePhoneENListExec* enListExec = CRetrieveMobilePhoneENListHelper::NewL(enstore);	
	CleanupStack::PushL(enListExec);
	CMobilePhoneENList* enList = enListExec ->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneENList::DoGetList is not able to get the list"));
	ASSERT_TRUE(enList != NULL, _L("CRetrieveMobilePhoneENList::DoGetList callForwardingList is NULL"));
	TEST_CHECK_POINT_L(_L("CRetrieveMobilePhoneENList::DoGetList enList is NULL"));

	// Wait for 1 second. 
	User::After(KOneSecond); 
	
	// Cancel the request  
	enListExec->CancelListRetrieval();
	
	// Check KErrCancel or KErrNone is returned.
	ASSERT_TRUE((err==KErrCancel)||(err== KErrNone), _L("CMobilePhoneENList::CancelListRetrieval returned an error"));

	//
	// TEST END
	//

    StartCleanup();
	
	//Pop
	//getEnStoreInfoStatus
	//enListExec
	CleanupStack::PopAndDestroy(2, &getEnStoreInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0012");
	}



CCTSYIntegrationTestENStore0013::CCTSYIntegrationTestENStore0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestENStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestENStore0013::GetTestStepName());
	}

CCTSYIntegrationTestENStore0013::~CCTSYIntegrationTestENStore0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestENStore0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-EN-0013
 * @SYMFssID BA/CTSY/EN-0013
 * @SYMTestCaseDesc Cancel get EN store info.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel
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
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Get EN store subsession. 
	RMobileENStore& enstore = iEtelSessionMgr.GetENStoreL(KMainServer, KMainPhone, KMainENStore);
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Get phone store info with RMobilePhoneStore::GetInfo  
	TExtEtelRequestStatus getEnStoreInfoStatus(enstore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(getEnStoreInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg(info);
	enstore.GetInfo(getEnStoreInfoStatus, infoPckg );

	// Cancel request with RMobilePhoneStore::CancelAsyncRequest(EMobilePhoneStoreGetInfo)  
	mobilePhone.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	
	// Check status of RMobilePhoneStore::GetInfo is KErrCancel or KErrNone if it was too late to cancel.	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getEnStoreInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhoneStore::GetPhoneStoreInfo, timed out"));
	ASSERT_TRUE((getEnStoreInfoStatus.Int() == KErrCancel) || (getEnStoreInfoStatus.Int() == KErrNone), _L("RMobilePhoneStore::GetInfo did not return KErrCancel or KErrNone"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getEnStoreInfoStatus
	CleanupStack::PopAndDestroy(&getEnStoreInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestENStore0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestENStore0013");
	}



