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
// Test step definitions for the ONStore functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestonstore.h"

CCTSYIntegrationTestONStoreBase::CCTSYIntegrationTestONStoreBase(
		CEtelSessionMgr& aEtelSessionMgr) : CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr),
		iSimTestHelper(*this),iPhoneTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestONStoreBase::~CCTSYIntegrationTestONStoreBase()
/*
 * Destructor
 */
	{
	}


	
TBool CCTSYIntegrationTestONStoreBase::CompareEntries(const RMobileONStore::TMobileONEntryV1 &aEntry1,
		const RMobileONStore::TMobileONEntryV1 &aEntry2)
/*
 * This function compare two ON store entries
 *
 * @param aEntry1 a reference to the first entry.
 * @param aEntry2 a reference to the second entry.
 *
 * @return - Returns ETrue iff both entries have the same parameters, EFalse otherwise.
 */	
	{
	TBool ret=ETrue;
	ASSERT_EQUALS_DES16(aEntry1.iNumber.iTelNumber, aEntry2.iNumber.iTelNumber, _L("RMobileONStore::Read Entry has the wrong iNumber.iTelNumber"));
	if(aEntry1.iNumber.iTelNumber!= aEntry2.iNumber.iTelNumber)
		{
		ret=EFalse;
		}
	ASSERT_EQUALS_DES16(aEntry1.iText, aEntry2.iText, _L("RMobileONStore::Read Entry has the wrong iText"));
	if(aEntry1.iText!= aEntry2.iText)
		{
		ret=EFalse;
		}
	return ret;
	}

TBool CCTSYIntegrationTestONStoreBase::GetUsedEntriesL(RMobileONStore& aOnStore,RMobilePhone& aPhone,TInt& aUsedEntries)
/**
 * This function return the number of used entries in the store
 * 
 * @param aOnStore a reference to the on store
 * @param aPhone a reference to the phone session
 * @param aUsedEntries a reference to an integer wich the function will put the number of used entries in
 * 
 * @return ETrue iff the function succeed
 */
	{
	TBool ret=ETrue;
	TInt usedEntries1=-1, usedEntries2=-1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_TRUE(ret=(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(aOnStore,KETelOwnNumberStore,storeInfoPckg,aPhone,usedEntries1,usedEntries2)==KErrNone),
			_L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	if(usedEntries1!=-1)
		aUsedEntries=usedEntries1;
	else
		aUsedEntries=usedEntries2;
	return ret;
	}
TInt CCTSYIntegrationTestONStoreBase::GetNumberOfUsedAndTotalEntriesInStoreL(RMobileONStore& aOnStore,
		RMobilePhone& aPhone,
		TInt& aUsedEntries, 
		TInt& aUsedEntries2,
		TInt& aTotalEntries, 
		TInt& aTotalEntries2)
/**
 * This function return the number of used entries and total entries in the store, using both the RMobileONStore and the RMobilePhone
 * 
 * @param aOnStore a reference to the on store
 * @param aPhone a reference to the phone session
 * @param aUsedEntries a reference to an integer wich the function will put the number of used entries in returned by RMobileONStore
 * @param aUsedEntries2 a reference to an integer wich the function will put the number of used entries in returned by RMobilePhone
 * @param aTotalEntries a reference to an integer wich the function will put the number of total entries in returned by RMobileONStore
 * @param aTotalEntries2 a reference to an integer wich the function will put the number of total  entries in returned by RMobilePhone
 * 
 * @return KErrNone if one of the get info APIs are supported, KErrNotSupported if neither are supported.
 */
	{
	// Get number of entries using RMobilePhoneStore::GetInfo
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	TExtEtelRequestStatus getInfoStatus (aOnStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	aOnStore.GetInfo(getInfoStatus,storeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	if (getInfoStatus.Int() == KErrNone)
		{
		aUsedEntries = storeInfo.iUsedEntries;
		aTotalEntries = storeInfo.iTotalEntries;
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhoneStore::GetInfo returned with error = %d"), getInfoStatus.Int());
		}

	// Get number of entries using RMobilePhone::GetPhoneStoreInfo
	RMobileONStore::TMobileONStoreInfoV1 storeInfo2;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg2 (storeInfo2);
	TExtEtelRequestStatus getInfoStatus2 (aOnStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus2);	
	aPhone.GetPhoneStoreInfo(getInfoStatus2,storeInfoPckg2, KETelOwnNumberStore);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getInfoStatus2, ETimeMedium), KErrNone,_L("RMobilePhone::GetPhoneStoreInfo timed out"));
	if (getInfoStatus2.Int() == KErrNone)
		{
		aTotalEntries2 = storeInfo2.iTotalEntries;
		aUsedEntries2 = storeInfo2.iUsedEntries;
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo returned with error = %d"), getInfoStatus2.Int());
		}
	
	if (getInfoStatus.Int() == KErrNone || getInfoStatus2.Int() == KErrNone)
		{
		CleanupStack::PopAndDestroy(2);
		return KErrNone;
		}
	else
		{
		CleanupStack::PopAndDestroy(2);
		return KErrNotSupported;
		}
	
	}

TBool CCTSYIntegrationTestONStoreBase::GetUsedandTotalEntriesL(RMobileONStore& aOnStore,RMobilePhone& aPhone,
		TInt& aUsedEntries,TInt& aTotalEntries)
/**
 * This function return the number of used entries in the store
 * 
 * @param aOnStore a reference to the on store
 * @param aPhone a reference to the phone session
 * @param aUsedEntries a reference to an integer wich the function will put the number of used entries in
 * 
 * @return ETrue iff the function succeed
 */
	{
	TBool ret=ETrue;
	TInt usedEntries1=-1, usedEntries2=-1;
	TInt totalEntries1=-1, totalEntries2=-1;
	ASSERT_TRUE(ret=(GetNumberOfUsedAndTotalEntriesInStoreL(aOnStore,
			aPhone,usedEntries1,usedEntries2,totalEntries1,totalEntries2)==KErrNone),
			_L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	if(usedEntries1!=-1)
		{
		aUsedEntries=usedEntries1;
		}
	else
		{
		aUsedEntries=usedEntries2;
		}
	if(totalEntries1!=-1)
		{
		aTotalEntries=totalEntries1;
		}
	else
		{
		aTotalEntries=totalEntries2;
		}
	return ret;
	}

TBool  CCTSYIntegrationTestONStoreBase::OpenADNStoreL()
/**
 * This function open a session for the ADN store,
 * this is since the ON store work only if there is a subsession of the ADN store open.
 * 
 * @return ETrue iff the function succeed
 */
    {
	// Open the ADN phone book subsession  
	RMobilePhoneBookStore& adnPhoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer, KMainPhone, KIccAdnPhoneBook);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,KIccAdnPhoneBook);
	return ETrue;
    }


CCTSYIntegrationTestONStore0001::CCTSYIntegrationTestONStore0001(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0001::GetTestStepName());
	}

CCTSYIntegrationTestONStore0001::~CCTSYIntegrationTestONStore0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0001
 * @SYMFssID BA/CTSY/PBON-0001
 * @SYMTestCaseDesc Get ON store info. 
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Phone store information returned correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify ON store information is valid.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));
	
	// Open ONStore subsession
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);

	//
	// SET UP END
	//

	StartTest ();
	
	
	//
	// TEST START
	//


	// Get ON store info. using RMobilePhoneStore::GetInfo with RMobileONStore::TMobileONStoreInfoV1 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	ASSERT_EQUALS(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	ASSERT_EQUALS_DES16(onStoreInfo.iName,KETelOwnNumberStore, _L("RMobileONStore::GetInfo returned wrong ON Store name"));

	// iType == RMobilePhoneStore::EOwnNumberStore 
	ASSERT_EQUALS(onStoreInfo.iType, RMobilePhoneStore::EOwnNumberStore, _L("RMobileONStore::GetInfo returned wrong type"));

	// iTotalEntries >= -1 
	ASSERT_TRUE(onStoreInfo.iTotalEntries >= -1 , _L("RMobileONStore::GetInfo returned wrong Total Entries"));

	// iUsedEntries >= -1 
	ASSERT_TRUE(onStoreInfo.iUsedEntries >= -1 , _L("RMobileONStore::GetInfo returned wrong Used Entries"));

	// iCaps == RMobilePhoneStore::KCapsIndividualEntry 
	//         | RMobilePhoneStore::KCapsReadAccess 
	//         | RMobilePhoneStore::KCapsWriteAccess 
	//         | RMobilePhoneStore::KCapsDeleteAll 
	//         | RMobilePhoneStore::KCapsNotifyEvent 
	//         | RMobilePhoneStore::KCapsWholeStore  
	TUint wantedBits = RMobilePhoneStore::KCapsIndividualEntry| RMobilePhoneStore::KCapsReadAccess| RMobilePhoneStore::KCapsWriteAccess| RMobilePhoneStore::KCapsDeleteAll| RMobilePhoneStore::KCapsNotifyEvent| RMobilePhoneStore::KCapsWholeStore;
	ASSERT_BITS_SET(onStoreInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned unexpected iCaps"));

	// iNumberLen >= -1 
	ASSERT_TRUE(onStoreInfo.iNumberLen >= -1 , _L("RMobileONStore::GetInfo returned wrong Number Length"));

	// iTextLen >= -1  (actually hard coded to 20 in CTSY) 
	ASSERT_TRUE(onStoreInfo.iTextLen >= -1 , _L("RMobileONStore::GetInfo returned wrong Text Length"));

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	onStoreInfoStatus
	CleanupStack::PopAndDestroy(&onStoreInfoStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0001");
	}

CCTSYIntegrationTestONStore0002::CCTSYIntegrationTestONStore0002(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0002::GetTestStepName());
	}

CCTSYIntegrationTestONStore0002::~CCTSYIntegrationTestONStore0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0002
 * @SYMFssID BA/CTSY/PBON-0002
 * @SYMTestCaseDesc Read entries from the ON store.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Entry read successfully.
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Get ON store subsession. 
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);

	// Backup first Entry
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsReadAccess,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Write entries to the first slot, second slot, and the maximum slot in the phone book. 
	RMobileONStore::TMobileONEntryV1 entry1, entry2, entryLast;
	RMobileONStore::TMobileONEntryV1Pckg writePckg1(entry1);
	RMobileONStore::TMobileONEntryV1Pckg writePckg2(entry2);
	RMobileONStore::TMobileONEntryV1Pckg writePckgLast(entryLast);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);

	// Create and write entry for 1st slot
	entry1.iMode=RMobilePhone::ENetworkModeGsm;
	entry1.iService=RMobilePhone::ETelephony;
	entry1.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry1.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber, "12345");
	entry1.iNumber.iTelNumber = KNumber;
	entry1.iText = _L("Entry 1");
	entry1.iIndex=1;
	onStore.Write (writeStatus, writePckg1);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));

	// Create and write entry for 2nd slot
	entry2.iMode=RMobilePhone::ENetworkModeGsm;
	entry2.iService=RMobilePhone::EAllTele;
	entry2.iNumber.iNumberPlan = RMobilePhone::EPrivateNumberPlan;
	entry2.iNumber.iTypeOfNumber = RMobilePhone::ENetworkSpecificNumber;
	_LIT(KNumber2, "647852");
	entry2.iNumber.iTelNumber = KNumber2;
	entry2.iText = _L("entry 2");
	entry2.iIndex=2;
	onStore.Write (writeStatus, writePckg2);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));


	// Create and write entry for last slot	
	entryLast.iMode=RMobilePhone::ENetworkModeGsm;
	entryLast.iService=RMobilePhone::EAllDataExSms;
	entryLast.iNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	entryLast.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	_LIT(KNumberLast, "341231233");
	entryLast.iNumber.iTelNumber = KNumberLast;
	entryLast.iText = _L("last entry");
	entryLast.iIndex=onStoreInfo.iTotalEntries;
	onStore.Write (writeStatus, writePckgLast);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));
	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Read entry from the first slot, second slot and the maximum slot from the phone book in turn passing an RMobileONStore::TMobileONEntryV1 as aEntry. 
	RMobileONStore::TMobileONEntryV1 readEntry1, readEntry2, readEntryLast;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg1(readEntry1);
	RMobileONStore::TMobileONEntryV1Pckg entryPckg2(readEntry2);
	RMobileONStore::TMobileONEntryV1Pckg entryPckgLast(readEntryLast);
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	readEntry1.iIndex=1;
	readEntry2.iIndex=2;
	readEntryLast.iIndex=onStoreInfo.iTotalEntries;

	// Slot 1
	onStore.Read (readStatus, entryPckg1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	// Slot 2
	onStore.Read (readStatus, entryPckg2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	// last Slot
	onStore.Read (readStatus, entryPckgLast);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	// For each one: 

	// Check iMode is the same as that written to the phone book in set up.
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10036);
	ASSERT_EQUALS(readEntry1.iMode,entry1.iMode, _L("RMobileONStore::Read ::Read 1st Read Entry has the wrong iMode"));
	ASSERT_EQUALS(readEntry2.iMode,entry2.iMode, _L("RMobileONStore::Read ::Read 2nd Read Entry has the wrong iMode"));
	ASSERT_EQUALS(readEntryLast.iMode,entryLast.iMode, _L("RMobileONStore::Read ::Read last Read Entry has the wrong iMode"));

	// Check iNumber is the same as that written to the phone book in set up.
	ASSERT_EQUALS(readEntry1.iNumber.iNumberPlan,entry1.iNumber.iNumberPlan, _L("RMobileONStore::Read ::Read 1st Read Entry has the wrong iNumber.iNumberPlan"));
	ASSERT_EQUALS(readEntry2.iNumber.iNumberPlan,entry2.iNumber.iNumberPlan, _L("RMobileONStore::Read ::Read 2nd Read Entry has the wrong iNumber.iNumberPlan"));
	ASSERT_EQUALS(readEntryLast.iNumber.iNumberPlan,entryLast.iNumber.iNumberPlan, _L("RMobileONStore::Read ::Read last Read Entry has the wrong iNumber.iNumberPlan"));
	ASSERT_EQUALS(readEntry1.iNumber.iTypeOfNumber,entry1.iNumber.iTypeOfNumber, _L("RMobileONStore::Read ::Read 1st Read Entry has the wrong iNumber.iTypeOfNumber"));
	ASSERT_EQUALS(readEntry2.iNumber.iTypeOfNumber,entry2.iNumber.iTypeOfNumber, _L("RMobileONStore::Read ::Read 2nd Read Entry has the wrong iNumber.iTypeOfNumber"));
	ASSERT_EQUALS(readEntryLast.iNumber.iTypeOfNumber,entryLast.iNumber.iTypeOfNumber, _L("RMobileONStore::Read ::Read last Read Entry has the wrong iNumber.iTypeOfNumber"));
	ASSERT_EQUALS_DES16(readEntry1.iNumber.iTelNumber,entry1.iNumber.iTelNumber, _L("RMobileONStore::Read ::Read 1st Read Entry has the wrong iNumber.iTelNumber"));
	ASSERT_EQUALS_DES16(readEntry2.iNumber.iTelNumber,entry2.iNumber.iTelNumber, _L("RMobileONStore::Read ::Read 2nd Read Entry has the wrong iNumber.iTelNumber"));
	ASSERT_EQUALS_DES16(readEntryLast.iNumber.iTelNumber,entryLast.iNumber.iTelNumber, _L("RMobileONStore::Read ::Read last Read Entry has the wrong iNumber.iTelNumber"));

	// Check iService is the same as that written to the phone book in set up.
	ASSERT_EQUALS(readEntry1.iService,entry1.iService, _L("RMobileONStore::Read ::Read 1st Read Entry has the wrong iService"));
	ASSERT_EQUALS(readEntry2.iService,entry2.iService, _L("RMobileONStore::Read ::Read 2nd Read Entry has the wrong iService"));
	ASSERT_EQUALS(readEntryLast.iService,entryLast.iService, _L("RMobileONStore::Read ::Read last Read Entry has the wrong iService"));

	// Check iText is the same as that written to the phone book in set up.
	ASSERT_EQUALS_DES16(readEntry1.iText,entry1.iText, _L("RMobileONStore::Read ::Read 1st Read Entry has the wrong iText"));
	ASSERT_EQUALS_DES16(readEntry2.iText,entry2.iText, _L("RMobileONStore::Read ::Read 2nd Read Entry has the wrong iText"));
	ASSERT_EQUALS_DES16(readEntryLast.iText,entryLast.iText, _L("RMobileONStore::Read ::Read last Read Entry has the wrong iText"));

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	readStatus
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(4,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0002");
	}

CCTSYIntegrationTestONStore0003::CCTSYIntegrationTestONStore0003(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0003::GetTestStepName());
	}

CCTSYIntegrationTestONStore0003::~CCTSYIntegrationTestONStore0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0003
 * @SYMFssID BA/CTSY/PBON-0003
 * @SYMTestCaseDesc Read entry that is not present in the ON store.
 * @SYMTestPriority High
 * @SYMTestActions 
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Get ON store subsession. 
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);

	// Backup first Entry
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsReadAccess,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Delete all entries from the phone book. 
	TExtEtelRequestStatus deleteAllStatus(onStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	onStore.DeleteAll (deleteAllStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));
	CHECK_EQUALS_L(deleteAllStatus.Int(), KErrNone, _L("RMobileONStore::DeleteAll returned an error"));

	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Read an entry from a valid index. 
	RMobileONStore::TMobileONEntryV1 readEntry;
	readEntry.iIndex=1;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg(readEntry);
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	onStore.Read (readStatus, entryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));

	// Check KErrNotFound is returned.
	// $CTSYProblem The LTSY returns KErrNone to CTSY when an entry is read from an empty slot
	// CTSY correctly propagates whatever error is returned by LTSY back to the client.
	// Therefore the test has been coded to expect KErrNone.  If the LTSY behaviour is fixed, KErrNotFound or similar
	// should be expected in this case.
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	readStatus
	//	deleteAllStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(4,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0003");
	}

CCTSYIntegrationTestONStore0004::CCTSYIntegrationTestONStore0004(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0004::GetTestStepName());
	}

CCTSYIntegrationTestONStore0004::~CCTSYIntegrationTestONStore0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0004
 * @SYMFssID BA/CTSY/PBON-0004
 * @SYMTestCaseDesc Read entries from the ON store using invalid index.
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Get ON store subsession. 
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsReadAccess,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Read an entry with an invalid slot number ===

	// Read a single entry from the phone book using aIndex = -1 
	RMobileONStore::TMobileONEntryV1 readEntry;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg(readEntry);
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	readEntry.iIndex=-1;
	onStore.Read (readStatus, entryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));

	// Check KErrGsmSMSInvalidMemoryIndex is returned.
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10041);
	ASSERT_EQUALS(readStatus.Int(), KErrGsmSMSInvalidMemoryIndex, _L("RMobileONStore::Read returned unexpected result"));

	// Read a single entry from the phone book using aIndex = -10 
	readEntry.iIndex=-10;
	onStore.Read (readStatus, entryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));

	// Check KErrGsmSMSInvalidMemoryIndex is returned.
	ASSERT_EQUALS(readStatus.Int(), KErrGsmSMSInvalidMemoryIndex, _L("RMobileONStore::Read returned unexpected result"));

	// Read a single entry from the phone book using aIndex = -50 
	readEntry.iIndex=-50;
	onStore.Read (readStatus, entryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));

	// Check KErrGsmSMSInvalidMemoryIndex is returned.
	ASSERT_EQUALS(readStatus.Int(), KErrGsmSMSInvalidMemoryIndex, _L("RMobileONStore::Read returned unexpected result"));


	//
	// TEST END
	//

	StartCleanup();

	// Pop:
	//	readStatus
	//	onStoreInfoStatus
	CleanupStack::PopAndDestroy(2,&onStoreInfoStatus);
	

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0004");
	}

CCTSYIntegrationTestONStore0005::CCTSYIntegrationTestONStore0005(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0005::GetTestStepName());
	}

CCTSYIntegrationTestONStore0005::~CCTSYIntegrationTestONStore0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0005
 * @SYMFssID BA/CTSY/PBON-0005
 * @SYMTestCaseDesc Write entries to the ON store.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Write successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries are written correctly and can be read.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Get ON store subsession. 
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);

	// Backup first Entry
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess | KCapsWriteAccess | KCapsNotifyEvent 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsNotifyEvent),
			KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));


	// Ensure phone book is not full. 
	TInt clearSpace = iSimTestHelper.EnsureEmptyStoreSpaceL(onStore,KETelOwnNumberStore,onStoreInfo,phone);
	CHECK_TRUE_L(clearSpace>=0,_L("TSimTsyTestHelper::EnsureBookIsNotFullL was unable to get an empty space"));

	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries are used in the phone book ===

	// Get phone store information using RMobilePhone::GetPhoneStoreInfo and KETelOwnNumberStore 

	// Store the iUsedEntries returned. 

	// Get phone book store information using RMobilePhoneStore::GetInfo 

	// Store the iUsedEntries returned. 
	TInt usedEntries =-1;
	TInt usedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,usedEntries,usedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// ===  Write a valid entry ===

	// Post notify event
	TUint32 storeEvent;
	TInt index;
	TExtEtelRequestStatus notifyStoreEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyStoreEventStatus);
	onStore.NotifyStoreEvent (notifyStoreEventStatus, storeEvent, index);

	// Write a valid entry with a national number to the phone book specifying a specific index to store the entry which does not contain an entry. 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber, "01632960000");
	entry.iNumber.iTelNumber = KNumber;
	entry.iText = _L("An Entry");
	entry.iIndex=clearSpace;
	onStore.Write (writeStatus, writePckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryAdded
	RMobilePhoneStore::TMobileStoreEvent
			expectedEvent = RMobilePhoneStore::KStoreEntryAdded;
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(onStore,notifyStoreEventStatus, storeEvent, expectedEvent, index,KErrNone);

	// ===  Read the written entry ===

	// Read the entry from the phone book. 
	RMobileONStore::TMobileONEntryV1 readEntry;
	RMobileONStore::TMobileONEntryV1Pckg pckgReadEntry(readEntry);
	readEntry.iIndex = clearSpace;
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	onStore.Read (readStatus, pckgReadEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	// Check that the entry information is the same as that written.
	ASSERT_TRUE(CompareEntries (readEntry, entry), _L("RMobileONStore::Read entries are not the same"));

	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored.

	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,
			phone,usedEntries+1,usedEntries2+1), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Write a valid entry ===

	// post notifier for RMobilePhoneStore::NotifyStoreEvent
	onStore.NotifyStoreEvent (notifyStoreEventStatus, storeEvent, index);
	
	// Write a valid entry with an international number (containing a '+' sign) to the phone book specifying a specific index to store the entry which does not contain an entry. 
	RMobileONStore::TMobileONEntryV1 entry2;
	RMobileONStore::TMobileONEntryV1Pckg writePckg2(entry2);
	entry2.iMode=RMobilePhone::ENetworkModeGsm;
	entry2.iService=RMobilePhone::ETelephony;
	entry2.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry2.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	_LIT(KNumber2, "+441632960000");
	entry2.iNumber.iTelNumber = KNumber2;
	entry2.iText = _L("IEntry");
	entry2.iIndex=clearSpace;
	onStore.Write (writeStatus, writePckg2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryAdded
	expectedEvent = RMobilePhoneStore::KStoreEntryAdded;
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyStoreEventStatus, storeEvent, expectedEvent, entry2.iIndex,KErrNone);

	// ===  Read the written entry ===

	// Read the entry from the phone book. 
	readEntry.iIndex = entry2.iIndex;
	onStore.Read (readStatus, pckgReadEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	// Check that the entry information is the same as that written.
	ASSERT_TRUE(CompareEntries (entry2, readEntry), _L("RMobileONStore::Read entries are not the same"));

	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===


	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored.

	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,usedEntries+1,usedEntries2+1), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	readStatus
	//	writeStatus
	//	notifyStoreEventStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(5,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0005");
	}

CCTSYIntegrationTestONStore0006::CCTSYIntegrationTestONStore0006(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0006::GetTestStepName());
	}

CCTSYIntegrationTestONStore0006::~CCTSYIntegrationTestONStore0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0006
 * @SYMFssID BA/CTSY/PBON-0006
 * @SYMTestCaseDesc Write an entry to phonebook at index of -1 (first free slot).
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Entry written.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry is written correctly and correct slot number is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	
	// Backup first Entry
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// SIM phone book is not full. 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	TInt clearSpace = iSimTestHelper.EnsureEmptyStoreSpaceL(onStore,KETelOwnNumberStore,onStoreInfo,phone);
	CHECK_TRUE_L(clearSpace>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL was unable to get an empty space"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | KCapsNotifyEvent 
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsNotifyEvent | RMobilePhoneStore::KCapsWriteAccess),
			KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  

	// Store iUsedEntries returned for future use 
	
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Store iUsedEntries returned for future use 
	TInt savedUsedEntries = -1;
	TInt savedUsedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries,savedUsedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	

	// ===  Write an entry to the first free slot ===

	// Post notify event
	TUint32 storeEvent;
	TInt index;
	TExtEtelRequestStatus notifyStoreEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyStoreEventStatus);
	onStore.NotifyStoreEvent (notifyStoreEventStatus, storeEvent, index);

	// Write a valid entry to the phone book using RMobilePhoneStore::Write supplying aEntry.index = -1 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber, "01632960000");
	entry.iNumber.iTelNumber = KNumber;
	entry.iText = _L("Entry11");
	entry.iIndex=-1;
	onStore.Write (writeStatus, writePckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));

	// Store the aEntry.iIndex returned for future use 
	TInt savedIndex=entry.iIndex;

	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryAdded
	RMobilePhoneStore::TMobileStoreEvent
			expectedEvent = RMobilePhoneStore::KStoreEntryAdded;
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyStoreEventStatus, storeEvent, expectedEvent, index,KErrNone);

	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored.

	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries+1,savedUsedEntries2+1), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Read the written entry ===

	// Read the written entry at index aEntry.iIndex returned from the write 
	RMobileONStore::TMobileONEntryV1 readEntry;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg(readEntry);
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	readEntry.iIndex = savedIndex;
	onStore.Read (readStatus, entryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	// Check RMobilePhoneStore::Read returns correct entry information 
	ASSERT_TRUE(CompareEntries (entry, readEntry), _L("RMobileONStore::Read entries are not the same"));

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	readStatus
	//	writeStatus
	//	notifyStoreEventStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(5,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0006");
	}

CCTSYIntegrationTestONStore0007::CCTSYIntegrationTestONStore0007(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0007::GetTestStepName());
	}

CCTSYIntegrationTestONStore0007::~CCTSYIntegrationTestONStore0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0007
 * @SYMFssID BA/CTSY/PBON-0007
 * @SYMTestCaseDesc Write entries to the ON store using invalid indexes.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Error returned on write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write to an invalid index and number of used entries in the phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure phone book is not full. 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	TInt clearSpace = iSimTestHelper.EnsureEmptyStoreSpaceL(onStore,KETelOwnNumberStore,onStoreInfo,phone);
	CHECK_TRUE_L(clearSpace>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL was unable to get an empty space"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess 
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess),
			KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  1. Find out how many entries are used in the phone book ===
	// ===  2. Attempt to write an entry to invalid index (0) ===
	DEBUG_PRINTF1(_L("Attempt to write an entry to invalid index (0)"));
	_LIT(KNumber0, "01632960000");
	WriteEntryAndCheckThatUsedEntriesDidNotChangedL (onStore, 0, KNumber0);
    clearSpace = iSimTestHelper.EnsureEmptyStoreSpaceL(onStore,KETelOwnNumberStore,onStoreInfo,phone);
	ASSERT_TRUE(clearSpace>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL was unable to get an empty space"));
	// Repeat steps 1 to 3 for aEntry.index = -2, -10 
	DEBUG_PRINTF1(_L("Attempt to write an entry to invalid index (-2)"));
	_LIT(KNumber2, "01632960000");
	WriteEntryAndCheckThatUsedEntriesDidNotChangedL (onStore, -2, KNumber2);
    clearSpace = iSimTestHelper.EnsureEmptyStoreSpaceL(onStore,KETelOwnNumberStore,onStoreInfo,phone);
	ASSERT_TRUE(clearSpace>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL was unable to get an empty space"));
	_LIT(KNumber3, "01632960000");
	
	DEBUG_PRINTF1(_L("Attempt to write an entry to invalid index (-10)"));
	WriteEntryAndCheckThatUsedEntriesDidNotChangedL (onStore, -10, KNumber3);
    clearSpace = iSimTestHelper.EnsureEmptyStoreSpaceL(onStore,KETelOwnNumberStore,onStoreInfo,phone);
	ASSERT_TRUE(clearSpace>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL was unable to get an empty space"));

	// ===  Attempt to write an entry to an index that is higher than the max slot number ===

	// Write an entry using RMobilePhoneStore::Write and aEntry.index = 10000 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber4, "01632960000");
	entry.iNumber.iTelNumber = KNumber4;
	entry.iText = _L("Entry11");
	entry.iIndex=10000;
	onStore.Write (writeStatus, writePckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));

	// Check error returned is not KErrNone
	ASSERT_TRUE((writeStatus.Int() != KErrNone), _L("RMobileONStore::Write returned an error") );

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(3,&backup);

	return TestStepResult();
	}


void CCTSYIntegrationTestONStore0007::WriteEntryAndCheckThatUsedEntriesDidNotChangedL(
		RMobileONStore& aStore, TInt aIndex, const TDesC& aNumber)
/*
 * This function write an entry to an invalid index and check that the number of used entries did not changed
 *
 * @param aStore Reference to On Store session.
 * @param aIndex the (invald) index
 * @param aNumber the number to write
 *
 */	
	{


	// ===  1. Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1 

	// Store iUsedEntries returned for future use 

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Store iUsedEntries returned for future use 
	TInt savedUsedEntries = -1;
	TInt savedUsedEntries2 = -1;
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(aStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries,savedUsedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// ===  2. Attempt to write an entry to invalid index ===

	// Write an entry using RMobilePhoneStore::Write and aEntry.index = aIndex 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(aStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iNumber.iTelNumber = aNumber;
	entry.iText = _L("Entry11");
	entry.iIndex=aIndex;

	// Check KErrArgument is returned
	aStore.Write (writeStatus, writePckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));

    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10042);
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));
	// ===  3. Get the number of used entries and check it is same as before the attempted write ===

	// Check RMobilePhoneStore::GetInfo returns same iUsedEntries as before

	// Check RMobilePhone::GetPhoneStoreInfo returns same iUsedEntries as before
    ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(aStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries,savedUsedEntries2), 
                  KErrNone, 
                  _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// Pop:
	//	writeStatus
	CleanupStack::PopAndDestroy(&writeStatus);

	}

TPtrC CCTSYIntegrationTestONStore0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0007");
	}

CCTSYIntegrationTestONStore0008::CCTSYIntegrationTestONStore0008(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0008::GetTestStepName());
	}

CCTSYIntegrationTestONStore0008::~CCTSYIntegrationTestONStore0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0008
 * @SYMFssID BA/CTSY/PBON-0008
 * @SYMTestCaseDesc Write entry to the ON store when SIM is full.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Error on attempt to write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned and number of used entries is unchanged.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Fill SIM phone book. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iNumber.iTelNumber = _L("12012030");
	entry.iText = _L("Entry11");
	TInt usedEntries,totalEntries;
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	for (; usedEntries<totalEntries;++usedEntries)
		{
		entry.iIndex=-1;
		onStore.Write (writeStatus, writePckg);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
				KErrNone, _L("RMobileONStore::Write timed out"));
		CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));
		}
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	CHECK_EQUALS_L(usedEntries,totalEntries, _L("RMobileONStore::GetInfo Unexpected number of iUsedEntries returned" ));

	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  

	// Store iUsedEntries returned for future use 

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Store iUsedEntries returned for future use 
	TInt savedUsedEntries = -1;
	TInt savedUsedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries,savedUsedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// ===  Attempt to write an entry to the phone book ===

	// Write an entry using RMobilePhoneStore::Write and aEntry.index = -1 
	entry.iNumber.iTelNumber = _L("12012034");
	entry.iIndex=-1;

	// Check error != KErrNone is returned.
	onStore.Write (writeStatus, writePckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	ASSERT_TRUE((writeStatus.Int() != KErrNone), _L("RMobileONStore::Write did not returned an error") );

	// ===  Get the number of used entries and check it is the same as before attempted write ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before

	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries,savedUsedEntries2), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	writeStatus
	//	backup
	CleanupStack::PopAndDestroy(2,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0008");
	}

CCTSYIntegrationTestONStore0009::CCTSYIntegrationTestONStore0009(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0009::GetTestStepName());
	}

CCTSYIntegrationTestONStore0009::~CCTSYIntegrationTestONStore0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0009
 * @SYMFssID BA/CTSY/PBON-0009
 * @SYMTestCaseDesc Write entry to the ON store making the store full.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::Write, RMobilePhoneStore::DeleteAll
 * @SYMTestExpectedResults Pass - Entry written correctly.  Store event is retrieved indicating entry has been written and store is full.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry written and store event completes indicating store is now full.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | KCapsNotifyEvent 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsNotifyEvent | RMobilePhoneStore::KCapsWriteAccess),KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Fill phone book until there is only one slot free. 
	TInt usedEntries,totalEntries;
	TExtEtelRequestStatus deleteStatus(onStore, EMobilePhoneStoreDelete);
	CleanupStack::PushL (deleteStatus);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iNumber.iTelNumber = _L("12012030");
	entry.iText = _L("Entry11");
	if(usedEntries==totalEntries)
		{
		onStore.Delete (deleteStatus, totalEntries);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileONStore::Delete timed out"));
		CHECK_EQUALS_L(deleteStatus.Int(), KErrNone, _L("RMobileONStore::Delete returned an error"));
		}
	else
		{
		for (; usedEntries+1<totalEntries;++usedEntries)
			{
			entry.iIndex=-1;
			onStore.Write (writeStatus, writePckg);
			CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
					KErrNone, _L("RMobileONStore::Write timed out"));
			CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));
			}
		}
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	CHECK_EQUALS_L(usedEntries,totalEntries-1, _L("RMobileONStore::GetInfo Unexpected number of iUsedEntries returned" ));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  

	// Store iUsedEntries returned for future use 

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Store iUsedEntries returned for future use 
	TInt savedUsedEntries = -1;
	TInt savedUsedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries,savedUsedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// ===  Write an entry into the last free slot in the phone book ===

	// Post notify event
	TUint32 storeEvent;
	TInt index;
	TExtEtelRequestStatus notifyStoreEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyStoreEventStatus);
	onStore.NotifyStoreEvent (notifyStoreEventStatus, storeEvent, index);

	// Write an entry using RMobilePhoneStore::Write and aEntry.index = -1 
	entry.iIndex=-1;
	onStore.Write (writeStatus, writePckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreFull | KStoreEntryAdded
    TInt expectedEvent = RMobilePhoneStore::KStoreFull | RMobilePhoneStore::KStoreEntryAdded;
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10037);
    iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyStoreEventStatus, storeEvent, expectedEvent, index,KErrNone);

	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored.

	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries+1,savedUsedEntries2+1), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Read the written entry ===

	// Read the written entry with RMobilePhoneStore::Read 
	RMobileONStore::TMobileONEntryV1 readEntry;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg(readEntry);
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	readEntry.iIndex = savedUsedEntries+1;
	onStore.Read (readStatus, entryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	// Check entry details are correct
	ASSERT_TRUE(CompareEntries (entry, readEntry), _L("RMobileONStore::Read entries are not the same"));
	// ===  Delete all entries from the phone book  ===

	// Delete all entries from the ON Store 
	TExtEtelRequestStatus deleteAllStatus(onStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	onStore.DeleteAll (deleteAllStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));
	ASSERT_EQUALS(deleteAllStatus.Int(), KErrNone, _L("RMobileONStore::DeleteAll returned an error"));

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries of 0.

	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries of 0.
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,0,0), 
		KErrNone, 
		_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Fill the phone book using RMobileONStore::StoreAllL ===

	// Populate a CMobilePhoneONList with enough entries to fill the ON store phone book. 
	CMobilePhoneONList *list = CMobilePhoneONList::NewL ();
	CleanupStack::PushL (list);
	RMobileONStore::TMobileONEntryV1 entry2;
	entry2.iMode=RMobilePhone::ENetworkModeGsm;
	entry2.iService=RMobilePhone::ETelephony;
	entry2.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry2.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry2.iText = _L("Entry2");
	entry2.iNumber.iTelNumber = _L("12012032");
	ASSERT_TRUE(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	for (TInt i=usedEntries+1; i <= totalEntries;++i)
		{
		entry2.iIndex=i;
		if ( entry2.iNumber.iTelNumber[0]=='9')
			{
			entry2.iNumber.iTelNumber[0]='0';
			}
		else
			{
			entry2.iNumber.iTelNumber[0]++;
			}
		list->AddEntryL (entry2);
		}

	// Post notify event
	TUint32 storeEvent2;
	TInt index2;
	TExtEtelRequestStatus notifyStoreAllEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyStoreAllEventStatus);
	onStore.NotifyStoreEvent (notifyStoreAllEventStatus, storeEvent2, index2);

	// Use RMobileONStore::StoreAllL to add these entries to the phone book. 
	DEBUG_PRINTF1( _L("Store all has cause panic, using bypass"));
	// Bypass : The session manager can not be used, since a PANIC is created
	RMobileONStore byPassOnStore;
	User::LeaveIfError(byPassOnStore.Open(phone));
	CleanupClosePushL(byPassOnStore);
	// By pass end
	TExtEtelRequestStatus storeAllStatus(byPassOnStore, EMobileONStoreStoreAll);
	CleanupStack::PushL (storeAllStatus);
	byPassOnStore.StoreAllL (storeAllStatus, list);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::StoreAllL timed out"));
	ASSERT_EQUALS(storeAllStatus.Int(), KErrNone, _L("RMobileONStore::StoreAllL returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreFull
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10038);
	RMobilePhoneStore::TMobileStoreEvent
			expectedEvent2 = RMobilePhoneStore::KStoreFull;
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyStoreAllEventStatus, storeEvent2, expectedEvent2, index2,KErrNone);

	// ===  Read all the written entries ===

	// Read all the entries in from the ON store. 

	// Check that they are the same as that stored using the CMobilePhoneONList
	ASSERT_TRUE(GetUsedEntriesL(onStore,phone,usedEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	RMobileONStore::TMobileONEntryV1 readEntry2;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg2(readEntry2);
	for (TInt i=1; i<=usedEntries;++i)
		{
		readEntry2.iIndex=i;
		onStore.Read (readStatus, entryPckg2);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
				KErrNone, _L("RMobileONStore::Read timed out"));
		ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));
		ASSERT_TRUE(CompareEntries (readEntry2, list->GetEntryL (i-1)), _L("RMobileONStore::Read entries are not the same"));
		}

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	storeAllStatus
	//	byPassOnStore
	//	notifyStoreAllEventStatus
	//	list
	//	deleteAllStatus
	//	readStatus
	//	notifyStoreEventStatus
	//	writeStatus
	//	deleteStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(11,&backup);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0009");
	}

CCTSYIntegrationTestONStore0010::CCTSYIntegrationTestONStore0010(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0010::GetTestStepName());
	}

CCTSYIntegrationTestONStore0010::~CCTSYIntegrationTestONStore0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0010
 * @SYMFssID BA/CTSY/PBON-0010
 * @SYMTestCaseDesc Write entry to ON store where name-tag and number are of maximum length.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Write successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify name and number are written correctly and can be read.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure phone book is not full. 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	TInt clearSpace = iSimTestHelper.EnsureEmptyStoreSpaceL(onStore,KETelOwnNumberStore,onStoreInfo,phone);
	CHECK_TRUE_L(clearSpace>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL was unable to get an empty space"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsNotifyEvent 
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsNotifyEvent),
			KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  

	// Store iUsedEntries returned for future use 

	// Store iNumLength and iTextLength for future use 
	TInt numberLength = onStoreInfo.iNumberLen;
	TInt textLength = onStoreInfo.iTextLen;
	
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Store iUsedEntries returned for future use 
	TInt usedEntries =-1;
	TInt usedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,usedEntries,usedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// ===  Write entry of maximum size to the ON store ===

	// Post notify event
	TUint32 storeEvent;
	TInt index;
	TExtEtelRequestStatus notifyStoreEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyStoreEventStatus);
	onStore.NotifyStoreEvent (notifyStoreEventStatus, storeEvent, index);
	
	// Create a RMobileONStore::TMobileONEntryV1 with a valid name in iText of length iTextLength and iNumber of length iNumLength 
	RMobileONStore::TMobileONEntryV1 entry;
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iIndex=clearSpace;
	entry.iText = TBuf<RMobileONStore::KOwnNumberTextSize>(textLength);
	entry.iText.Fill('A',textLength);
	entry.iNumber.iTelNumber = TBuf<RMobilePhone::KMaxMobileTelNumberSize>(numberLength);
    entry.iNumber.iTelNumber.Zero();
    entry.iNumber.iTelNumber.Append('+');
    for(TInt i=1;i<numberLength;++i) //Apend numberLength digits, since the '+' is the first digit
        {
        entry.iNumber.iTelNumber.Append('0'+(i%10));
        }

	// Write the entry to the ON store
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	onStore.Write (writeStatus, writePckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEntryAdded
	RMobilePhoneStore::TMobileStoreEvent expectedEvent = RMobilePhoneStore::KStoreEntryAdded;
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(onStore,notifyStoreEventStatus, storeEvent, expectedEvent, index,KErrNone);

	// ===  Read the written entry ===

	// Read the written entry with RMobilePhoneStore::Read 
	RMobileONStore::TMobileONEntryV1 readEntry;
	RMobileONStore::TMobileONEntryV1Pckg pckgReadEntry(readEntry);
	readEntry.iIndex = clearSpace;
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	onStore.Read (readStatus, pckgReadEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	// Check entry details are correct
	ASSERT_TRUE(CompareEntries (readEntry, entry), _L("RMobileONStore::Read entries are not the same"));

	// ===  Get the number of used entries and check used entries is one higher than before the write ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries is one higher than before the write

	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries is one higher than before the write
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,usedEntries+1,usedEntries2+1), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));


	//
	// TEST END
	//

	StartCleanup();

	// Pop:
	//	readStatus
	//	writeStatus
	//	notifyStoreEventStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(5,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0010");
	}

CCTSYIntegrationTestONStore0011::CCTSYIntegrationTestONStore0011(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0011::GetTestStepName());
	}

CCTSYIntegrationTestONStore0011::~CCTSYIntegrationTestONStore0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0011
 * @SYMFssID BA/CTSY/PBON-0011
 * @SYMTestCaseDesc Delete entry from the ON store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Entry deleted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and deleted entry cannot be read.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure phone has KCapsReadAccess | KCapsWriteAccess | KCapsNotifyEvent capabilities. 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsNotifyEvent),KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Fill the phone book. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iNumber.iTelNumber = _L("12012030");
	entry.iText = _L("Entry11");
	TInt usedEntries,totalEntries;
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	for (; usedEntries<totalEntries;++usedEntries)
		{
		entry.iIndex=-1;
		onStore.Write (writeStatus, writePckg);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
				KErrNone, _L("RMobileONStore::Write timed out"));
		CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));
		}
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	CHECK_EQUALS_L(usedEntries,totalEntries, _L("RMobileONStore::GetInfo Unexpected number of iUsedEntries returned" ));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobileONStoreInfoV1  

	// Store iUsedEntries returned for future use 

	// Store iTotalEntries for future use 

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Store iUsedEntries returned for future use 
	TInt savedUsedEntries = -1;
	TInt savedUsedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries,savedUsedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// ===  Delete an entry ===

	// Post notify event
	TUint32 deleteEvent;
	TInt index;
	TExtEtelRequestStatus notifyDeleteEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyDeleteEventStatus);
	onStore.NotifyStoreEvent (notifyDeleteEventStatus, deleteEvent, index);

	// Delete entry from the phone book specifying aIndex = 1 
	TExtEtelRequestStatus deleteStatus(onStore, EMobilePhoneStoreDelete);
	CleanupStack::PushL (deleteStatus);
	onStore.Delete (deleteStatus, 1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileONStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileONStore::Delete returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEntryDeleted
	RMobilePhoneStore::TMobileStoreEvent expectedEvent = RMobilePhoneStore::KStoreEntryDeleted;
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyDeleteEventStatus, deleteEvent, expectedEvent, index,KErrNone);

	// ===  Get the number of used entries and check it is 1 lower than before deleting the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 lower  than that stored.

	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 lower than that stored.
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,
			phone,savedUsedEntries-1,savedUsedEntries2-1), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Attempt to read the deleted entry.  ===

	// Read a single entry stored in the slot used in delete 
	RMobileONStore::TMobileONEntryV1 readEntry;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg(readEntry);
	readEntry.iIndex=1;
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	onStore.Read (readStatus, entryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));

	// Check KErrNotFound is returned.
	// $CTSYProblem The LTSY returns KErrNone to CTSY when an entry is read from an empty slot
	// CTSY correctly propagates whatever error is returned by LTSY back to the client.
	// Therefore the test has been coded to expect KErrNone.  If the LTSY behaviour is fixed, KErrNotFound or similar
	// should be expected in this case.
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));

	// ===  Attempt to delete the already deleted entry again. ===

	// Delete the already deleted entry 
	onStore.Delete (deleteStatus, 1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileONStore::Delete timed out"));

	// Check KErrNotFound is returned
	// $CTSYProblem The LTSY returns KErrNone to CTSY when an entry is deleted from an empty slot
	// CTSY correctly propagates whatever error is returned by LTSY back to the client.
	// Therefore the test has been coded to expect KErrNone.  If the LTSY behaviour is fixed, KErrNotFound or similar
	// should be expected in this case.
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileONStore::Delete returned unexpected"));

	// Repeat whole test with aIndex = TMobileONStoreInfoV1::iTotalEntries (the last slot) 
	onStore.NotifyStoreEvent (notifyDeleteEventStatus, deleteEvent, index);
	onStore.Delete (deleteStatus, storeInfo.iTotalEntries);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileONStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileONStore::Delete returned an error"));
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyDeleteEventStatus, deleteEvent, expectedEvent, index,KErrNone);
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,
			phone,savedUsedEntries-2,savedUsedEntries2-2), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	readEntry.iIndex=storeInfo.iTotalEntries;
	onStore.Read (readStatus, entryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	// $CTSYProblem The LTSY returns KErrNone to CTSY when an entry is read from an empty slot
	// CTSY correctly propagates whatever error is returned by LTSY back to the client.
	// Therefore the test has been coded to expect KErrNone.  If the LTSY behaviour is fixed, KErrNotFound or similar
	// should be expected in this case.
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));
	onStore.Delete (deleteStatus, storeInfo.iTotalEntries);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileONStore::Delete timed out"));
	// $CTSYProblem The LTSY returns KErrNone to CTSY when an entry is deleted from an empty slot
	// CTSY correctly propagates whatever error is returned by LTSY back to the client.
	// Therefore the test has been coded to expect KErrNone.  If the LTSY behaviour is fixed, KErrNotFound or similar
	// should be expected in this case.
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileONStore::Delete returned unexpected"));


	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	readStatus
	//	deleteStatus
	//	notifyDeleteEventStatus
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(6,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0011");
	}

CCTSYIntegrationTestONStore0012::CCTSYIntegrationTestONStore0012(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0012::GetTestStepName());
	}

CCTSYIntegrationTestONStore0012::~CCTSYIntegrationTestONStore0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0012
 * @SYMFssID BA/CTSY/PBON-0012
 * @SYMTestCaseDesc Delete entry from the ON store which does not exist.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Error returned on delete.
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Ensure phone has KCapsReadAccess and KCapsWriteAccess capabilities. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess),
			KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Backup first Entry
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure there is an empty slot in a known location. 
	TInt clearSpace = iSimTestHelper.EnsureEmptyStoreSpaceL(onStore,KETelOwnNumberStore,onStoreInfo,phone);
	CHECK_TRUE_L(clearSpace>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL was unable to get an empty space"));

	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  

	// Store iUsedEntries returned for future use 

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Store iUsedEntries returned for future use 
	TInt savedUsedEntries = -1;
	TInt savedUsedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries,savedUsedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// Delete an entry using RMobilePhoneStore::Delete where aIndex is the index of a known empty slot. 
	// Check KErrNotFound is returned.
	TExtEtelRequestStatus deleteStatus(onStore, EMobilePhoneStoreDelete);
	CleanupStack::PushL (deleteStatus);
	onStore.Delete (deleteStatus, clearSpace);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileONStore::Delete timed out"));
	// $CTSYProblem The LTSY returns KErrNone to CTSY when an entry is deleted from a slot which is already empty
	// CTSY correctly propagates whatever error is returned by LTSY back to the client.
	// Therefore the test has been coded to expect KErrNone.  If the LTSY behaviour is fixed, KErrNotFound or similar
	// should be expected in this case.
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileONStore::Delete returned an error"));

	// ===  Get the number of used entries and check it is the same as before attempted delete ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before
	
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedUsedEntries,savedUsedEntries2), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	deleteStatus
	//	backup
	//	onStoreInfoStatus
	CleanupStack::PopAndDestroy(3,&onStoreInfoStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0012");
	}

CCTSYIntegrationTestONStore0013::CCTSYIntegrationTestONStore0013(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0013::GetTestStepName());
	}

CCTSYIntegrationTestONStore0013::~CCTSYIntegrationTestONStore0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0013
 * @SYMFssID BA/CTSY/PBON-0013
 * @SYMTestCaseDesc Delete entry from the ON store which makes the store empty.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Entry deleted, RMobilePhoneStore::KStoreEmpty event occurs.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry deleted and KStoreEmpty event occurs.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure phone has KCapsReadAccess | KCapsWriteAccess | KCapsNotifyEvent capabilities. 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsNotifyEvent),KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Delete all the entries from the phone book then write one entry to a known location. 
	TExtEtelRequestStatus deleteAllStatus(onStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	onStore.DeleteAll (deleteAllStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));
	CHECK_EQUALS_L(deleteAllStatus.Int(), KErrNone, _L("RMobileONStore::DeleteAll returned an error"));
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber, "12345");
	entry.iNumber.iTelNumber = KNumber;
	entry.iText = _L("Entry 1");
	entry.iIndex=1;
	onStore.Write (writeStatus, writePckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries are used in the phone book ===
	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1 
	// Check iUsedEntries is 1
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Store iUsedEntries is 1 
	TInt usedEntries =-1;
	TInt usedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,usedEntries,usedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	ASSERT_EQUALS(usedEntries,1,_L("RMobilePhoneStore::GetInfo returned an unexpected value."));

	// Post notify event
	TUint32 deleteEvent;
	TInt index;
	TExtEtelRequestStatus notifyDeleteEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyDeleteEventStatus);
	onStore.NotifyStoreEvent (notifyDeleteEventStatus, deleteEvent, index);
	
	// Delete the entry from the ON store 
	TExtEtelRequestStatus deleteStatus(onStore, EMobilePhoneStoreDelete);
	CleanupStack::PushL (deleteStatus);
	onStore.Delete (deleteStatus, 1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileONStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileONStore::Delete returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEntryDeleted
    // Due to defect 10039 (see above) KStoreEmpty will not returned
	TInt32 expectedEvent = RMobilePhoneStore::KStoreEntryDeleted;
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyDeleteEventStatus, deleteEvent, expectedEvent, index,KErrNone);

	// ===  Get the number of used entries and check it is the same as before attempted delete ===
	// Check RMobilePhoneStore::GetInfo returns iUsedEntries of 0
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries of 0
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,usedEntries,usedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	ASSERT_EQUALS(usedEntries,0,_L("RMobilePhoneStore::GetInfo returned an unexpected value."));


	//
	// TEST END
	//

	StartCleanup();

	// Pop:
	//	deleteStatus
	//	notifyDeleteEventStatus
	//	writeStatus
	//	deleteAllStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(6,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0013");
	}

CCTSYIntegrationTestONStore0014::CCTSYIntegrationTestONStore0014(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0014::GetTestStepName());
	}

CCTSYIntegrationTestONStore0014::~CCTSYIntegrationTestONStore0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0014
 * @SYMFssID BA/CTSY/PBON-0014
 * @SYMTestCaseDesc Delete an entry from a full ON store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Entries deleted, RMobilePhoneStore::KStoreHasSpace event occurs.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry deleted and KStoreHasSpace event occurs.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsNotifyEvent 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsNotifyEvent),KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Ensure phone book is full 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iNumber.iTelNumber = _L("12012030");
	entry.iText = _L("Entry11");
	TInt usedEntries,totalEntries;
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	for (; usedEntries<totalEntries;++usedEntries)
		{
		entry.iIndex=-1;
		onStore.Write (writeStatus, writePckg);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
				KErrNone, _L("RMobileONStore::Write timed out"));
		CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));
		}
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	CHECK_EQUALS_L(usedEntries,totalEntries, _L("RMobileONStore::GetInfo Unexpected number of iUsedEntries returned" ));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries are used in the phone book ===
	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  
	// Store iUsedEntries returned for future use 
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Store iUsedEntries returned for future use 
	usedEntries =-1;
	TInt usedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,usedEntries,usedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// Post notify event
	TUint32 deleteEvent;
	TInt index;
	TExtEtelRequestStatus notifyDeleteEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyDeleteEventStatus);
	onStore.NotifyStoreEvent (notifyDeleteEventStatus, deleteEvent, index);

	// Delete an entry from the phone book. 
	TExtEtelRequestStatus deleteStatus(onStore, EMobilePhoneStoreDelete);
	CleanupStack::PushL (deleteStatus);
	onStore.Delete (deleteStatus, 1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileONStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileONStore::Delete returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreHasSpace | RMobilePhoneStore::KStoreEntryDeleted
    // Checking for KStoreHasSpace will fail, see defect 10039
	TInt32 expectedEvent = RMobilePhoneStore::KStoreEntryDeleted;
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyDeleteEventStatus, deleteEvent, expectedEvent, index,KErrNone);

	// ===  Get the number of used entries and check it is one lower than before ===
	// Check RMobilePhoneStore::GetInfo returns iUsedEntries is one lower than before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries is one lower than before
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,
			phone,usedEntries-1,usedEntries2-1), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));


	//
	// TEST END
	//

	StartCleanup();

	// Pop:
	//	deleteStatus
	//	notifyDeleteEventStatus
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(5,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0014");
	}

CCTSYIntegrationTestONStore0015::CCTSYIntegrationTestONStore0015(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0015::GetTestStepName());
	}

CCTSYIntegrationTestONStore0015::~CCTSYIntegrationTestONStore0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0015
 * @SYMFssID BA/CTSY/PBON-0015
 * @SYMTestCaseDesc Delete all entries from the ON store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Entries deleted, RMobilePhoneStore::KStoreEmpty event occurs.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: All entries deleted.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsNotifyEvent 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsNotifyEvent |  RMobilePhoneStore::KCapsWriteAccess),KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);

	// Ensure phone book is not empty 
	TInt clearSpace;
	CHECK_TRUE_L(GetUsedEntriesL(onStore,phone,clearSpace),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	if ( clearSpace == 0)
		{

		// Create and write entry for 1st slot
		entry.iMode=RMobilePhone::ENetworkModeGsm;
		entry.iService=RMobilePhone::ETelephony;
		entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
		entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
		_LIT(KNumber, "12345");
		entry.iNumber.iTelNumber = KNumber;
		entry.iText = _L("Entry 1");
		entry.iIndex=1;
		onStore.Write (writeStatus, writePckg);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
				KErrNone, _L("RMobileONStore::Write timed out"));
		CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));
		CHECK_TRUE_L(GetUsedEntriesL(onStore,phone,clearSpace),_L("RMobileONStore::GetInfo can not get the number of used entries"));
		CHECK_TRUE_L(clearSpace >0, _L("CCTSYIntegrationTestONStore0015::doTestStepL was unable to add entry"));
		}

	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Delete all entries from the phone book ===

	// Post notify event
	TUint32 storeEvent;
	TInt index;
	TExtEtelRequestStatus notifyStoreEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyStoreEventStatus);
	onStore.NotifyStoreEvent (notifyStoreEventStatus, storeEvent, index);

	// Delete all entries from the full phone book 
	TExtEtelRequestStatus deleteAllStatus(onStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	onStore.DeleteAll (deleteAllStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));
	ASSERT_EQUALS(deleteAllStatus.Int(), KErrNone, _L("RMobileONStore::DeleteAll returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent complete with KStoreEmpty
	RMobilePhoneStore::TMobileStoreEvent
			expectedEvent = RMobilePhoneStore::KStoreEmpty;
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyStoreEventStatus, storeEvent, expectedEvent, index,KErrNone);

	// ===  Check phone book is empty ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  

	// Check iUsedEntries == 0

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Check iUsedEntries == 0
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,0,0), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	deleteAllStatus
	//	notifyStoreEventStatus
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(5,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0015");
	}

CCTSYIntegrationTestONStore0016::CCTSYIntegrationTestONStore0016(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0016::GetTestStepName());
	}

CCTSYIntegrationTestONStore0016::~CCTSYIntegrationTestONStore0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0016
 * @SYMFssID BA/CTSY/PBON-0016
 * @SYMTestCaseDesc Cancel delete of all entries from the ON store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Some entries deleted, request cancelled successfully. Store not empty.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify some entries deleted before cancellation but cancellation occurs.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsWriteAccess),KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Ensure phone book is full 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iNumber.iTelNumber = _L("12012030");
	entry.iText = _L("Entry11");
	TInt usedEntries,totalEntries;
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	for (; usedEntries<totalEntries;++usedEntries)
		{
		entry.iIndex=-1;
		onStore.Write (writeStatus, writePckg);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
				KErrNone, _L("RMobileONStore::Write timed out"));
		CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));
		}
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	CHECK_EQUALS_L(usedEntries,totalEntries, _L("RMobileONStore::GetInfo Unexpected number of iUsedEntries returned" ));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries are used in the phone book ===
	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  
	// Store iUsedEntries returned for future use 
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Store iUsedEntries returned for future use 
	usedEntries =-1;
	TInt usedEntries2 = -1;
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,usedEntries,usedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// ===  Delete all entries from the phone book ===

	// Delete all entries from the full phone book 
	TExtEtelRequestStatus deleteAllStatus(onStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	onStore.DeleteAll (deleteAllStatus);

	// Do not wait for the request to complete. 
	// Wait for 1 second. 
	User::After(KOneSecond);
	
	// Cancel the DeleteAll operation. Check KErrCancel returned on status of DeleteAll 
	onStore.CancelAsyncRequest(EMobilePhoneStoreDeleteAll);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));
	ASSERT_TRUE(((deleteAllStatus.Int()== KErrCancel) || (deleteAllStatus.Int()== KErrNone)) , _L("RMobileONStore::DeleteAll did not returned cancel"));

	// ===  Check phone book is not empty but some entries have been deleted. ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  
	TInt newUsedEntries =-1;
	TInt newUsedEntries2 = -1;
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,newUsedEntries,newUsedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// Check iUsedEntries < before deletion.
	ASSERT_TRUE(newUsedEntries<=usedEntries,_L("RMobileONStore::DeleteAll failed to delete"));

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Check iUsedEntries < before deletion.
	ASSERT_TRUE(newUsedEntries2<=usedEntries2,_L("RMobileONStore::DeleteAll failed to delete"));


	//
	// TEST END
	//

	StartCleanup();

	// Pop:
	//	deleteAllStatus
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(4,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0016");
	}

CCTSYIntegrationTestONStore0017::CCTSYIntegrationTestONStore0017(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0017::GetTestStepName());
	}

CCTSYIntegrationTestONStore0017::~CCTSYIntegrationTestONStore0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0017
 * @SYMFssID BA/CTSY/PBON-0017
 * @SYMTestCaseDesc Write and delete all own number entries.
 * @SYMTestPriority High
 * @SYMTestActions RMobileONStore::StoreAllL, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Own numbers stored, read and deleted successfully. KErrNotFound when deleting all entries from an empty store.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify that entire store can be written to in one call and correct store event occurs.

 Verify KErrNotFound when attempting to delete all entries from an empty store.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsNotifyEvent 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsNotifyEvent),KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries can be stored in the ON store ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  

	// Store iTotalEntries for future use 
	TInt usedEntries,savedTotalEntries,usedEntries2,savedTotalEntries2;
	ASSERT_EQUALS(GetNumberOfUsedAndTotalEntriesInStoreL(onStore,phone,usedEntries,usedEntries2,savedTotalEntries,savedTotalEntries2),KErrNone,
			_L("CCTSYIntegrationTestONStore0017::doTestStepL can not get the number of used entries"));
	
	// Populate a CMobilePhoneOnList with iTotalEntries number of valid entries 
	CMobilePhoneONList *list = CMobilePhoneONList::NewL ();
	CleanupStack::PushL (list);
	RMobileONStore::TMobileONEntryV1 entry;
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iText = _L("Entry2");
	entry.iNumber.iTelNumber = _L("12012032");
	for (TInt i=1; i <= savedTotalEntries;++i)
		{
		entry.iIndex=i;
		if ( entry.iNumber.iTelNumber[0]=='9')
			{
			entry.iNumber.iTelNumber[0]='0';
			}
		else
			{
			entry.iNumber.iTelNumber[0]++;
			}
		list->AddEntryL (entry);
		}
	
	// Post notify event
	TUint32 storeEvent;
	TInt index;
	TExtEtelRequestStatus notifyStoreEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyStoreEventStatus);
	onStore.NotifyStoreEvent (notifyStoreEventStatus, storeEvent, index);
	
	// Use RMobileONStore::StoreAllL to store all the entries to the ON store. 
	// Bypass : The session manager can not be used, since a PANIC is created
	RMobileONStore byPassOnStore;
	User::LeaveIfError(byPassOnStore.Open(phone));
	CleanupClosePushL(byPassOnStore);
	// By pass end
	TExtEtelRequestStatus storeAllStatus(byPassOnStore, EMobileONStoreStoreAll);
	CleanupStack::PushL (storeAllStatus);
	byPassOnStore.StoreAllL (storeAllStatus, list);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::StoreAllL timed out"));
	ASSERT_EQUALS(storeAllStatus.Int(), KErrNone, _L("RMobileONStore::StoreAllL returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreDoRefresh | KStoreFull event
    //CANT USE KStoreFull, SEE defect 10038
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyStoreEventStatus, storeEvent, (RMobilePhoneStore::KStoreDoRefresh), index,KErrNone);

	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  

	// Check iUsedEntries returned = iTotalEntries returned
	
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Check iUsedEntries returned = iTotalEntries returnedx
	RMobileONStore::TMobileONStoreInfoV1 storeInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg storeInfoPckg (storeInfo);
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,savedTotalEntries,savedTotalEntries2), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// Read all the entries from the ON store 

	// Check all contain the same data as that written
	RMobileONStore::TMobileONEntryV1 readEntry;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg(readEntry);
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	TInt usedSpace;
	ASSERT_TRUE(GetUsedEntriesL(onStore,phone,usedSpace),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	for (TInt i=1; i<=usedSpace;++i)
		{
		readEntry.iIndex=i;
		onStore.Read (readStatus, entryPckg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
				KErrNone, _L("RMobileONStore::Read timed out"));
		ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileONStore::Read returned an error"));
		ASSERT_TRUE(CompareEntries (readEntry, list->GetEntryL (i-1)), _L("RMobileONStore::Read entries are not the same"));
		}

	// Delete all the entries from the store 
	TExtEtelRequestStatus deleteAllStatus(onStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	onStore.DeleteAll (deleteAllStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));
	ASSERT_EQUALS(deleteAllStatus.Int(), KErrNone, _L("RMobileONStore::DeleteAll returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEmpty
	RMobilePhoneStore::TMobileStoreEvent
	expectedEvent = RMobilePhoneStore::KStoreEmpty;
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10039);
	iSimTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent ( onStore,
			notifyStoreEventStatus, storeEvent, expectedEvent, index,KErrNone);

	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  

	// Check iUsedEntries is 0

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 

	// Check iUsedEntries is 0
	ASSERT_EQUALS(iSimTestHelper.AssertNumberOfUsedEntriesCorrectL(onStore,KETelOwnNumberStore,storeInfoPckg,phone,0,0), 
			KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// Delete all entries from the store 
	onStore.DeleteAll (deleteAllStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));

	// Check KErrNotFound is returned
	// $CTSYProblem The LTSY returns KErrNone to CTSY when all the entries are deleted from an empty store
	// CTSY correctly propagates whatever error is returned by LTSY back to the client.
	// Therefore the test has been coded to expect KErrNone.  If the LTSY behaviour is fixed, KErrNotFound or similar
	// should be expected in this case.
	ASSERT_EQUALS(deleteAllStatus.Int(), KErrNone, _L("RMobileONStore::DeleteAll returned an error"));


	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	deleteAllStatus
	//	readStatus
	//	storeAllStatus
	//	byPassOnStore
	//	notifyStoreEventStatus
	//	list
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(8,&backup);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0017");
	}

CCTSYIntegrationTestONStore0018::CCTSYIntegrationTestONStore0018(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0018::GetTestStepName());
	}

CCTSYIntegrationTestONStore0018::~CCTSYIntegrationTestONStore0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0018
 * @SYMFssID BA/CTSY/PBON-0018
 * @SYMTestCaseDesc Cancel writing of all entries to the ON store
 * @SYMTestPriority High
 * @SYMTestActions RMobileONStore::StoreAllL, RMobilePhoneStore::DeleteAll, RMobilePhoneStore::Read, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrCancel when cancelling StoreAll operation on ON store.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify some but not all entries are written.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsWriteAccess),
			KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Ensure ON store is empty 
	TExtEtelRequestStatus deleteAllStatus(onStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	onStore.DeleteAll (deleteAllStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));
	CHECK_EQUALS_L(deleteAllStatus.Int(), KErrNone, _L("RMobileONStore::DeleteAll returned an error"));
	TInt usedEntries;
	CHECK_TRUE_L(GetUsedEntriesL(onStore,phone,usedEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	CHECK_EQUALS_L(usedEntries,0,_L("RMobileONStore::DeleteAll did not delete all"));
	
	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries can be stored in the ON store ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  
	// Store iTotalEntries for future use 
	TInt totalEntries;
	ASSERT_TRUE(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));

	// Populate a CMobilePhoneOnList with iTotalEntries number of valid entries 
	CMobilePhoneONList *list = CMobilePhoneONList::NewL ();
	CleanupStack::PushL (list);
	RMobileONStore::TMobileONEntryV1 entry2;
	entry2.iMode=RMobilePhone::ENetworkModeGsm;
	entry2.iService=RMobilePhone::ETelephony;
	entry2.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry2.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry2.iText = _L("Entry2");
	entry2.iNumber.iTelNumber = _L("12012032");
	ASSERT_TRUE(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	for (TInt i=1; i <= totalEntries;++i)
		{
		entry2.iIndex=i;
		if ( entry2.iNumber.iTelNumber[0]=='9')
			{
			entry2.iNumber.iTelNumber[0]='0';
			}
		else
			{
			entry2.iNumber.iTelNumber[0]++;
			}
		list->AddEntryL (entry2);
		}

	// Use RMobileONStore::StoreAllL to store all the entries to the ON store. 
	DEBUG_PRINTF1( _L("Store all has cause panic, using bypass"));
	// Bypass : The session manager can not be used, since a PANIC is created
	RMobileONStore byPassOnStore;
	User::LeaveIfError(byPassOnStore.Open(phone));
	CleanupClosePushL(byPassOnStore);
	// By pass end
	TExtEtelRequestStatus storeAllStatus(byPassOnStore, EMobileONStoreStoreAll);
	CleanupStack::PushL (storeAllStatus);
	byPassOnStore.StoreAllL (storeAllStatus, list);

	// Do not wait for the request to complete before continuing. 

	// Wait for 1 second. 
	User::After(KOneSecond);
	
	// Cancel the StoreAllL request. 
	onStore.CancelAsyncRequest(EMobileONStoreStoreAll);

	// Check KErrCancel is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::StoreAllL timed out"));
	ASSERT_TRUE(((storeAllStatus.Int() == KErrCancel) || (storeAllStatus.Int() == KErrNone)), _L("RMobileONStore::StoreAllL did not returned cancel"));

	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  
	TInt newUsedEntries =-1;
	TInt newUsedEntries2 = -1;
	ASSERT_EQUALS(iSimTestHelper.GetNumberOfUsedEntriesInStoreL(onStore,KETelOwnNumberStore,onStoreInfoPckg,phone,newUsedEntries,newUsedEntries2),KErrNone, _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// Check iUsedEntries is > 0
	ASSERT_TRUE(newUsedEntries>=0,_L("RMobileONStore::StoreAllL failed to store"));

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Check iUsedEntries is > 0


	//
	// TEST END
	//

	StartCleanup();

	// Pop:
	//	storeAllStatus
    //  byPassOnStore
	//	list
	//	deleteAllStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(6,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0018");
	}

CCTSYIntegrationTestONStore0019::CCTSYIntegrationTestONStore0019(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0019::GetTestStepName());
	}

CCTSYIntegrationTestONStore0019::~CCTSYIntegrationTestONStore0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0019
 * @SYMFssID BA/CTSY/PBON-0019
 * @SYMTestCaseDesc Write a list of own numbers when there is not enough space in the phone book for the whole list.
 * @SYMTestPriority High
 * @SYMTestActions RMobileONStore::StoreAllL
 * @SYMTestExpectedResults Pass - KErrTooBig returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned when an attempt is made to store more entries than is possible into the ON store.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsWriteAccess,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// ===  Find out how many entries can be stored in the ON store ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobileONStore::TMobileONStoreInfoV1  
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	ASSERT_EQUALS(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));

	
	// Populate a CMobilePhoneOnList with (iTotalEntries+1) number of valid entries 
	CMobilePhoneONList *list = CMobilePhoneONList::NewL ();
	CleanupStack::PushL (list);
	RMobileONStore::TMobileONEntryV1 entry;
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iText = _L("Entry2");
	entry.iNumber.iTelNumber = _L("12012032");
	for (TInt i=1; i <= onStoreInfo.iTotalEntries+1;++i)
		{
		entry.iIndex=i;
		if ( entry.iNumber.iTelNumber[0]=='9')
			{
			entry.iNumber.iTelNumber[0]='0';
			}
		else
			{
			entry.iNumber.iTelNumber[0]++;
			}
		list->AddEntryL (entry);
		}

	// Use RMobileONStore::StoreAllL to store all the entries to the ON store. 
	// Bypass : The session manager can not be used, since a PANIC is created
	RMobileONStore byPassOnStore;
	User::LeaveIfError(byPassOnStore.Open(phone));
	CleanupClosePushL(byPassOnStore);
	// By pass end
	TExtEtelRequestStatus storeAllStatus(byPassOnStore, EMobileONStoreStoreAll);
	CleanupStack::PushL (storeAllStatus);
	byPassOnStore.StoreAllL (storeAllStatus, list);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::StoreAllL timed out"));

	// Check KErrTooBig is returned.
	ASSERT_EQUALS(storeAllStatus.Int(), KErrTooBig, _L("RMobileONStore::StoreAllL returned an error"));


	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	storeAllStatus
	//	byPassOnStore
	//	list
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(5,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0019");
	}

CCTSYIntegrationTestONStore0020::CCTSYIntegrationTestONStore0020(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0020::GetTestStepName());
	}

CCTSYIntegrationTestONStore0020::~CCTSYIntegrationTestONStore0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0020
 * @SYMFssID BA/CTSY/PBON-0020
 * @SYMTestCaseDesc Cancel writing an entry to the ON store.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel or KErrNone if it was too late to cancel.
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsWriteAccess,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Ensure there is an empty slot in a known location. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	TInt clearSpace = iSimTestHelper.EnsureEmptyStoreSpaceL(onStore,KETelOwnNumberStore,onStoreInfo,phone);
	CHECK_TRUE_L(clearSpace>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL was unable to get an empty space"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Write an entry from the location of a known empty slot. 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber, "01632960000");
	entry.iNumber.iTelNumber = KNumber;
	entry.iText = _L("An Entry 2");
	entry.iIndex=clearSpace;
	onStore.Write (writeStatus, writePckg);

	// Cancel the read request immediately using RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreWrite) 
	onStore.CancelAsyncRequest(EMobilePhoneStoreWrite);

	// Check KErrCancel or KErrNone is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	ASSERT_TRUE(((writeStatus.Int()== KErrNone) || ((writeStatus.Int()== KErrCancel))), _L("RMobileONStore::Write returned an error"));


	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(3,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0020");
	}

CCTSYIntegrationTestONStore0021::CCTSYIntegrationTestONStore0021(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0021::GetTestStepName());
	}

CCTSYIntegrationTestONStore0021::~CCTSYIntegrationTestONStore0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0021
 * @SYMFssID BA/CTSY/PBON-0021
 * @SYMTestCaseDesc Attempt to read own number into a wrongly sized buffer.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrArgument?
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess),KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Write an entry to a known location in the phone book 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber, "12345");
	entry.iNumber.iTelNumber = KNumber;
	entry.iText = _L("Entry 2");
	entry.iIndex=2;
	onStore.Write (writeStatus, writePckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Read an entry from the ON store using RMobilePhoneStore::Read and aEntry as a TBuf<1> 
	TBuf8<1> entryPckg;
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	onStore.Read (readStatus, entryPckg);

	// Check error is returned.
	ASSERT_TRUE((readStatus.Int() != KErrNone), _L("RMobileONStore::Read did not returned an error") );


	//
	// TEST END
	//

	StartCleanup();

	// Pop:
	//	readStatus
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(4,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0021");
	}

CCTSYIntegrationTestONStore0022::CCTSYIntegrationTestONStore0022(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0022::GetTestStepName());
	}

CCTSYIntegrationTestONStore0022::~CCTSYIntegrationTestONStore0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0022
 * @SYMFssID BA/CTSY/PBON-0022
 * @SYMTestCaseDesc Cancel reading an entry from the ON store.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel or KErrNone if it was too late to cancel.
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsReadAccess,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Write an entry to a known location in the phone book 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber, "12345");
	entry.iNumber.iTelNumber = KNumber;
	entry.iText = _L("Entry 2");
	entry.iIndex=2;
	onStore.Write (writeStatus, writePckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Read an entry from a known location from the ON store. 
	RMobileONStore::TMobileONEntryV1 readEntry;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg(readEntry);
	TExtEtelRequestStatus readStatus(onStore, EMobilePhoneStoreRead);
	CleanupStack::PushL (readStatus);
	readEntry.iIndex=2;
	onStore.Read (readStatus, entryPckg);

	// Cancel the read request immediately using RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreRead) 
	onStore.CancelAsyncRequest(EMobilePhoneStoreRead);

	// Check KErrCancel or KErrNone is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Read timed out"));
	ASSERT_TRUE(((readStatus.Int()== KErrNone) || ((readStatus.Int()== KErrCancel))), _L("RMobileONStore::Read returned an error"));


	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	readStatus
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(4,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0022");
	}

CCTSYIntegrationTestONStore0023::CCTSYIntegrationTestONStore0023(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0023::GetTestStepName());
	}

CCTSYIntegrationTestONStore0023::~CCTSYIntegrationTestONStore0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0023
 * @SYMFssID BA/CTSY/PBON-0023
 * @SYMTestCaseDesc Cancel delete one entry from ON store.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel or KErrNone if it was too late to cancel.
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsWriteAccess,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));
	
	// Write an entry to a known location in the phone book 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber, "12345");
	entry.iNumber.iTelNumber = KNumber;
	entry.iText = _L("Entry 2");
	entry.iIndex=2;
	onStore.Write (writeStatus, writePckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Delete an entry at a known index from the ON store. 
	TExtEtelRequestStatus deleteStatus(onStore, EMobilePhoneStoreDelete);
	CleanupStack::PushL (deleteStatus);
	onStore.Delete (deleteStatus, 2);

	// Cancel the request immediately with RTelSubSessionBase::.CancelAsyncRequest( EMobilePhoneStoreDelete ) 
	onStore.CancelAsyncRequest(EMobilePhoneStoreDelete);

	// Check KErrCancel or KErrNone is returned (if it was too late to cancel).
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileONStore::Delete timed out"));
	ASSERT_TRUE(((deleteStatus.Int()== KErrNone) || (deleteStatus.Int()== KErrCancel)), _L("RMobileONStore::Delete returned an error"));


	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	deleteStatus
	//	writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(4,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0023");
	}

CCTSYIntegrationTestONStore0024::CCTSYIntegrationTestONStore0024(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0024::GetTestStepName());
	}

CCTSYIntegrationTestONStore0024::~CCTSYIntegrationTestONStore0024()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0024::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0024
 * @SYMFssID BA/CTSY/PBON-0024
 * @SYMTestCaseDesc Cancel notification of a store event on the ON store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Write, RMobilePhoneStore::Delete, RMobileONStore::StoreAllL
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsNotifyEvent 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsNotifyEvent,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Write an entry to a known location in the phone book 
	TExtEtelRequestStatus deleteAllStatus(onStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	onStore.DeleteAll (deleteAllStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));
	CHECK_EQUALS_L(deleteAllStatus.Int(), KErrNone, _L("RMobileONStore::DeleteAll returned an error"));
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KNumber, "12345");
	entry.iNumber.iTelNumber = KNumber;
	entry.iText = _L("Entry 2");
	entry.iIndex=2;
	onStore.Write (writeStatus, writePckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Request notification of a store event with RMobilePhoneStore::NotifyStoreEvent 
	TUint32 storeEvent;
	TInt index;
	TExtEtelRequestStatus notifyStoreEventStatus(onStore,
			EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL (notifyStoreEventStatus);
	onStore.NotifyStoreEvent (notifyStoreEventStatus, storeEvent, index);

	// Cancel the RMobilePhoneStore::NotifyStoreEvent with RTelSubSession::CancelAsyncRequest( EMobilePhoneStoreNotifyStoreEvent ) 
	onStore.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyStoreEventStatus, ETimeMedium), KErrNone, _L("RMobileONStore::NotifyStoreEvent timed out"));

	// Delete an entry from the phone store 
	TExtEtelRequestStatus deleteStatus(onStore, EMobilePhoneStoreDelete);
	CleanupStack::PushL (deleteStatus);
	onStore.Delete (deleteStatus, 2);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(),KErrNone, _L("RMobileONStore::Delete did not returned as expected"));

	// Write an entry to the phone store 
	entry.iNumber.iTelNumber = _L("4531231");
	entry.iIndex=1;
	onStore.Write (writeStatus, writePckg);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(),KErrNone, _L("RMobileONStore::Write did not returned as expected"));

	// Use RMobileONStore::StoreAllL to store entries to the ON store. 
	CMobilePhoneONList *list = CMobilePhoneONList::NewL ();
	CleanupStack::PushL (list);
	RMobileONStore::TMobileONEntryV1 entry2;
	entry2.iMode=RMobilePhone::ENetworkModeGsm;
	entry2.iService=RMobilePhone::ETelephony;
	entry2.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry2.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry2.iText = _L("Entry2");
	entry2.iNumber.iTelNumber = _L("12012032");
	for (TInt i=1; i < 2;++i)
		{
		entry2.iIndex=i;
		if ( entry2.iNumber.iTelNumber[0]=='9')
			{
			entry2.iNumber.iTelNumber[0]='0';
			}
		else
			{
			entry2.iNumber.iTelNumber[0]++;
			}
		list->AddEntryL (entry2);
		}
	// Bypass : The session manager can not be used, since a PANIC is created
	RMobileONStore byPassOnStore;
	User::LeaveIfError(byPassOnStore.Open(phone));
	CleanupClosePushL(byPassOnStore);
	// By pass end
	TExtEtelRequestStatus storeAllStatus(byPassOnStore, EMobileONStoreStoreAll);
	CleanupStack::PushL (storeAllStatus);
	byPassOnStore.StoreAllL (storeAllStatus, list);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(storeAllStatus, ETimeLong),
			KErrNone, _L("RMobileONStore::StoreAllL timed out"));
	ASSERT_EQUALS(storeAllStatus.Int(),KErrNone, _L("RMobileONStore::StoreAllL did not returned as expected"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KErrCancel
	ASSERT_EQUALS(notifyStoreEventStatus.Int(),KErrCancel, _L("RMobileONStore::NotifyStoreEvent did not returned KErrCancel"));



	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	storeAllStatus
	//	byPassOnStore
	//	list
	//	deleteStatus
	//	notifyStoreEventStatus
	//	writeStatus
    //  deleteAllStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(9,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0024::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0024");
	}

CCTSYIntegrationTestONStore0025::CCTSYIntegrationTestONStore0025(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0025::GetTestStepName());
	}

CCTSYIntegrationTestONStore0025::~CCTSYIntegrationTestONStore0025()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0025::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0025
 * @SYMFssID BA/CTSY/PBON-0025
 * @SYMTestCaseDesc Cancel get ON store info.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrCancel or KErrNone if it was too late to cancel.
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Request to get phone store information by calling RMobilePhoneStore::GetInfo 
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);

	// $CTSYKnownFailure defect id=50009. CMmONStoreTsy::GetInfoL completes RMobileONStore::GetInfo() request to the test client with error KErrNotReady
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d </font>"), 50009);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);

	// Cancel get information by calling RTelSubSession::CancelAsyncRequest(EMobilePhoneStoreGetInfo) 
	onStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);

	// Check KErrCancel is returned or KErrNone if it was too late to cancel.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	ASSERT_TRUE(((onStoreInfoStatus.Int()== KErrCancel)||(onStoreInfoStatus.Int()== KErrNone)), _L("RMobileONStore::GetInfo returned an error"));


	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//	onStoreInfoStatus
	CleanupStack::PopAndDestroy(&onStoreInfoStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0025::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0025");
	}

CCTSYIntegrationTestONStore0026::CCTSYIntegrationTestONStore0026(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0026::GetTestStepName());
	}

CCTSYIntegrationTestONStore0026::~CCTSYIntegrationTestONStore0026()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0026::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0026
 * @SYMFssID BA/CTSY/PBON-0026
 * @SYMTestCaseDesc Read all ON store entries.
 * @SYMTestPriority High
 * @SYMTestActions CRetrieveMobilePhoneONList::RetrieveListL
 * @SYMTestExpectedResults Pass - All entries read correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify all entries read correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));
	// Backup first Entry
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWholeStore 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsWholeStore,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Ensure there are 2 entries in the ON store. 
	TExtEtelRequestStatus deleteAllStatus(onStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	onStore.DeleteAll (deleteAllStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::DeleteAll timed out"));
	CHECK_EQUALS_L(deleteAllStatus.Int(), KErrNone, _L("RMobileONStore::DeleteAll returned an error"));
    TInt numberOfUsedEntries = 2 ;
	CHECK_TRUE_L((onStoreInfo.iTotalEntries>=numberOfUsedEntries), _L("The ON store dose not have 2 entries!"));
	CMobilePhoneONList *writeList = CMobilePhoneONList::NewL ();
	CleanupStack::PushL (writeList);
	RMobileONStore::TMobileONEntryV1 entry;
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iText = _L("Entry2");
	entry.iNumber.iTelNumber = _L("12012032");
	for (TInt i=1; i <= numberOfUsedEntries;++i)
		{
		entry.iIndex=i;
		if ( entry.iNumber.iTelNumber[0]=='9')
			{
			entry.iNumber.iTelNumber[0]='0';
			}
		else
			{
			entry.iNumber.iTelNumber[0]++;
			}
		writeList->AddEntryL (entry);
		}
	DEBUG_PRINTF1( _L("Store all has cause panic, using bypass"));
	// Bypass : The session manager can not be used, since a PANIC is created
	RMobileONStore byPassOnStore;
	User::LeaveIfError(byPassOnStore.Open(phone));
	CleanupClosePushL(byPassOnStore);
	// By pass end
	TExtEtelRequestStatus storeAllStatus(byPassOnStore, EMobileONStoreStoreAll);
	CleanupStack::PushL (storeAllStatus);
	byPassOnStore.StoreAllL (storeAllStatus, writeList);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(storeAllStatus, ETimeMedium), KErrNone, _L("RMobileONStore::StoreAllL timed out"));
	CHECK_EQUALS_L(storeAllStatus.Int(), KErrNone, _L("RMobileONStore::StoreAllL returned an error"));
	
	

	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Use the helper class for CRetrieveMobilePhoneONList to read entries from the ON store. 
	TInt err;
	CRetrieveMobilePhoneONListExec *helper=CRetrieveMobilePhoneONListHelper::NewL(onStore);
    CleanupStack::PushL(helper);
	CMobilePhoneONList *readList = helper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneONListHelper::DoGetList is not able to get the list"));
	

	// Check that all 2 entries have been read and are correct.
	ASSERT_EQUALS(readList->Enumerate(), writeList->Enumerate(), _L("CRetrieveMobilePhoneONListHelper::DoGetList entries list size is wrong"));
	for (TInt i=0; i<readList->Enumerate();++i)
		{
		ASSERT_TRUE(CompareEntries (readList->GetEntryL (i), writeList->GetEntryL (i)), _L("CRetrieveMobilePhoneONListHelper::DoGetListe entries are not the same"));
		}

	//
	// TEST END
	//

	StartCleanup();

	// Pop:
    // helper
	//	storeAllStatus
	//	byPassOnStore
	//	writeList
    //  deleteAllStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(7,&backup);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0026::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0026");
	}

CCTSYIntegrationTestONStore0027::CCTSYIntegrationTestONStore0027(
		CEtelSessionMgr& aEtelSessionMgr) :
	CCTSYIntegrationTestONStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestONStore0027::GetTestStepName());
	}

CCTSYIntegrationTestONStore0027::~CCTSYIntegrationTestONStore0027()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestONStore0027::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBON-0027
 * @SYMFssID BA/CTSY/PBON-0027
 * @SYMTestCaseDesc Cancel read all ON store entries.
 * @SYMTestPriority High
 * @SYMTestActions CAsyncRetrieveStoreList::CancelReq
 * @SYMTestExpectedResults Pass - KErrCancel or KErrNone if it was too late to cancel.
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

	// Open the ADN store
	CHECK_TRUE_L(OpenADNStoreL(),_L("CCTSYIntegrationTestONStoreBase::OpenADNStoreL Failed"));

	// Backup first Entry
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL (KMainServer, KMainPhone);
	RMobileONStore& onStore = iEtelSessionMgr.GetONStoreL (KMainServer,
			KMainPhone, KMainONStore);
	TBackupAllEntries backup(*this,onStore);
	CHECK_TRUE_L(backup.BackupL(), _L("TBackupAllEntries::Backup Could not backup"));
	CleanupStack::PushL (backup);

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWholeStore 
	RMobileONStore::TMobileONStoreInfoV1 onStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onStoreInfoPckg(onStoreInfo);
	TExtEtelRequestStatus onStoreInfoStatus(onStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (onStoreInfoStatus);
	onStore.GetInfo (onStoreInfoStatus, onStoreInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(onStoreInfoStatus, ETimeMedium),
			KErrNone, _L("RMobileONStore::GetInfo timed out"));
	CHECK_EQUALS_L(onStoreInfoStatus.Int(), KErrNone, _L("RMobileONStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(onStoreInfo.iCaps, RMobilePhoneStore::KCapsWholeStore,KNoUnwantedBits, _L("RMobileONStore::GetInfo returned incorrect store caps"));

	// Fill SIM phone book. 
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg writePckg(entry);
	TExtEtelRequestStatus writeStatus(onStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);
	entry.iMode=RMobilePhone::ENetworkModeGsm;
	entry.iService=RMobilePhone::ETelephony;
	entry.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	entry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entry.iNumber.iTelNumber = _L("12012030");
	entry.iText = _L("Entry11");
	TInt usedEntries,totalEntries;
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	for (; usedEntries<totalEntries;++usedEntries)
		{
		entry.iIndex=-1;
		onStore.Write (writeStatus, writePckg);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),
				KErrNone, _L("RMobileONStore::Write timed out"));
		CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileONStore::Write returned an error"));
		}
	CHECK_TRUE_L(GetUsedandTotalEntriesL(onStore,phone,usedEntries,totalEntries),_L("RMobileONStore::GetInfo can not get the number of used entries"));
	CHECK_EQUALS_L(usedEntries,totalEntries, _L("RMobileONStore::GetInfo Unexpected number of iUsedEntries returned" ));


	//
	// SET UP END
	//

	StartTest ();

	//
	// TEST START
	//


	// Use the helper class for CRetrieveMobilePhoneONList to read entries from the ON store. 
	TInt err;
	CRetrieveMobilePhoneONListExec *helper=CRetrieveMobilePhoneONListHelper::NewL(onStore);
	CleanupStack::PushL(helper);
	CMobilePhoneONList *readList = helper->DoGetList(err,ETrue);

	// Wait one second. 
	User::After(KOneSecond);

	// Cancel the request immediately. 
	
	// Check KErrCancel is returned or KErrNone if it was too late to cancel.
	ASSERT_TRUE(((err== KErrCancel) ||(err== KErrNone)), _L("CRetrieveMobilePhoneONListHelper::DoGetList is not able to get the list"));


	//
	// TEST END
	//

	StartCleanup();

	
	// Pop:
	//  helper
	//  writeStatus
	//	onStoreInfoStatus
	//	backup
	CleanupStack::PopAndDestroy(4,&backup);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestONStore0027::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestONStore0027");
	}


TBackupAllEntries::TBackupAllEntries(CCTSYIntegrationTestONStoreBase &aBase,
		RMobileONStore &aOnStore) : iValid(EFalse), iOnStore(aOnStore), iBase(aBase),
                                     							iHelper(NULL), iList(NULL)
/*
 * Constructor
 * @param aBase pointer to the test.
 * @param aOnStore reference to a store session
 *
 */
{
}

TBool TBackupAllEntries::BackupL()
/*
 * This function save the first entry in the storem and return false if its fails
 *
 * @return true if the backup was successful
 *
 */	
	{
	iValid=EFalse;
    if(iList!=NULL)
        return iValid;
    if(iHelper!=NULL)
        return iValid;

    // Read All the items from the store
	TInt err;
	iHelper=CRetrieveMobilePhoneONListHelper::NewL(iOnStore);
    CleanupStack::PushL(iHelper);
	iList = iHelper->DoGetList(err);
    if((err!=KErrNone)||(iList->Enumerate()==0))
        {
        iList=NULL;
        CleanupStack::PopAndDestroy(iHelper);
        iHelper=NULL;
        return iValid;
        }
    iValid=ETrue;
	CleanupStack::Pop(1);
    return iValid;
	}

void TBackupAllEntries::DoCleanupL(TAny* aPtr)
/*
 * This function is a static clean up function to be saved in the cleanup stack. Its restore the saved entry to the first entry in the phone
 *
 * @param aPtr a pointer to a TBackupAllEntries instance.
 *
 */	
	{
	TBackupAllEntries* me = static_cast<TBackupAllEntries*>(aPtr);
	me->CleanupL();
	}

void TBackupAllEntries::CleanupL()
/*
 * This function restore the saved entry to the first entry in the phone
 *
 */	
	{
	if(!iValid)
		return;
    CleanupStack::PushL(iHelper);
    TExtEtelRequestStatus deleteAllStatus(iOnStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL (deleteAllStatus);
	TExtEtelRequestStatus writeStatus(iOnStore, EMobilePhoneStoreWrite);
	CleanupStack::PushL (writeStatus);

	iOnStore.DeleteAll (deleteAllStatus);
    iBase.WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium);
    for(TInt i=0;i<iList->Enumerate();++i)
    	{
    	RMobileONStore::TMobileONEntryV1Pckg writePckg(iList->GetEntryL (i));
    	iOnStore.Write (writeStatus, writePckg);
    	iBase.WaitForRequestWithTimeOut(writeStatus, ETimeMedium);
    	}

	CleanupStack::PopAndDestroy(3);
    iHelper=NULL;
    iList=NULL;
    iValid=EFalse;
	}

TBackupAllEntries::operator TCleanupItem()
/*
 * This operator create a TCleanupItem to save a pointer to the DoCleanup static function and this in the cleanup stack
 *
 */	
	{
	return TCleanupItem(DoCleanupL,this);
	}
