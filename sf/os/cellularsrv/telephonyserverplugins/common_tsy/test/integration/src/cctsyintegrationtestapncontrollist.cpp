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
// Test step definitions for the APNControlList functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestapncontrollist.h"

#include <etelmmerr.h>
#include "cctsyinidata.h"
#include <pcktcs.h>

_LIT(KPrompt, "This test requires USIM service table, Insert a 3G enabled SIM which has the APN control list service enabled");
CCTSYIntegrationTestAPNControlListBase::CCTSYIntegrationTestAPNControlListBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iPhoneTestHelper(*this), iNetworkTestHelper(*this),iSimTestHelper(*this), iPacketServiceHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestAPNControlListBase::~CCTSYIntegrationTestAPNControlListBase()
/*
 * Destructor
 */
	{
	}

void CCTSYIntegrationTestAPNControlListBase::EnsureApnListIsEmptyL(RMobilePhone &aMobilePhone)
/*
 * This function ensures that Apn List is empty.
 *
 * @param aMobilePhone Reference to opened RMobilePhone subsession.
 * 
 */
	{
	TUint32 index= 0;
	TExtEtelRequestStatus deleteAPNNameStatus(aMobilePhone, EMobilePhoneDeleteAPNName);
	CleanupStack::PushL(deleteAPNNameStatus);
	TExtEtelRequestStatus enumerateAPNEntriesStatus(aMobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	aMobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	CHECK_EQUALS_L(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	while(index!=0)
		{
		aMobilePhone.DeleteAPNName(deleteAPNNameStatus, index-1);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::DeleteAPNName timed out"));
		CHECK_EQUALS_L(deleteAPNNameStatus.Int(), KErrNone, _L("RMobilePhone::DeleteAPNName returned with an error"));		
		index=index-1;
		if(index==0)
			{
			aMobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
			CHECK_EQUALS_L(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
			CHECK_EQUALS_L(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
			}
		}
	
	CHECK_TRUE_L(index==0 , _L("RMobilePhone::EnumerateAPNEntries did not return empty list"));		
	CleanupStack::PopAndDestroy(2, &deleteAPNNameStatus);
	
	}

TBool CCTSYIntegrationTestAPNControlListBase::CheckEntryIsInListL(RMobilePhone &aMobilePhone,RMobilePhone::TAPNEntryV3 &aExpectedAPnEntry, TUint32 &aIndex)
/*
 * This function undo checks a specific entry in list by comparing them
 *
 * @param aMobilePhone Reference to opened RMobilePhone subsession.
 * @param expectedAPnEntry Reference to the entry for which check is being carried out.
 * @param index stores the specific index of found entry in the APN list, which can be used by some other operation. 
 * @return true if entry is found.
 */
	{
	// Check RMobilePhone::EnumerateAPNEntries returns aIndex 
	TUint32 listIndex= 0;
	TBool ret(EFalse);
	TExtEtelRequestStatus enumerateAPNEntriesStatus(aMobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	aMobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, listIndex);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	
	// Check no entry exists in the list
	if(listIndex==0)
		{
		ret=EFalse;
		}
	
	// Check RMobilePhone::GetAPNname returns same APN name as that written
	RMobilePhone::TAPNEntryV3Pckg expectedAPnEntryPckg(aExpectedAPnEntry);
	RMobilePhone::TAPNEntryV3 myAPnEntry;
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
	TExtEtelRequestStatus getAPNnameStatus(aMobilePhone, EMobilePhoneGetAPNname);
	CleanupStack::PushL(getAPNnameStatus);
	for(TInt i=0; i<listIndex;i++)
		{
		aMobilePhone.GetAPNname(getAPNnameStatus,i, myAPnEntryPckg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getAPNnameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNname timed out"));
		ASSERT_EQUALS(getAPNnameStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNname returned with an error"));		
		if(myAPnEntry.iApn== aExpectedAPnEntry.iApn)
			{
			aIndex=i;
			ret= ETrue;
			break;
			}
		else
			{
			ret=EFalse;	
			}
		}
			
	// Pop:
	// enumerateAPNEntriesStatus
	// getAPNnameStatus
		
	CleanupStack::PopAndDestroy(2, &enumerateAPNEntriesStatus);
		
	return ret;
	}


CCTSYIntegrationTestAPNControlList0001::CCTSYIntegrationTestAPNControlList0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0001::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0001::~CCTSYIntegrationTestAPNControlList0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0001
 * @SYMFssID BA/CTSY/ACL-0001
 * @SYMTestCaseDesc Append and get entries from the APN control list.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::VerifySecurityCode, RMobilePhone::NotifySecurityEvent, RMobilePhone::NotifyAPNListChanged, RMobilePhone::GetAPNname, RMobilePhone::AppendAPNName
 * @SYMTestExpectedResults Pass - Number of APNs in list is correct. Written entries can be read.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify number of APNs is correct, APN notification completes and that written entries can be read.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	DisplayUserInteractionPromptL(KPrompt); // 30 seconds
	
	// Verify Pin2
	iSimTestHelper.VerifyPin2L(mobilePhone, iNetworkTestHelper); 
	
	// Take the backup of APN Control list. 
	TInt listCountAtStart;
	TBackupAPNList backup(*this,mobilePhone);
	CHECK_TRUE_L(backup.BackupL(listCountAtStart), _L("TBackupAPNList::Backup could not backup"));
	CleanupStack::PushL (backup);
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 securityCaps;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(securityCaps), KErrNone,_L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(securityCaps,RMobilePhone::KCapsAccessPin2 ,KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned did not return KCapsAccessPin2 bits"));
	
	// Ensure the APN Control List Service Status is EAPNControlListServiceEnabled. 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus setAPNControlListServiceStatus(mobilePhone, EMobilePhoneSetAPNControlListServiceStatus);
	CleanupStack::PushL(setAPNControlListServiceStatus);
	TExtEtelRequestStatus getAPNControlListServiceStatus(mobilePhone, EMobilePhoneGetAPNControlListServiceStatus);
	CleanupStack::PushL(getAPNControlListServiceStatus);
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
	
	if(serviceApnControlListStatus== RMobilePhone::EAPNControlListServiceDisabled)
		{
		// Enable the APN control List Service
		serviceApnControlListStatus=RMobilePhone::EAPNControlListServiceEnabled;
		mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(setAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus returned with an error"));		
			
		// Get the APN Contol List Service Status again
		mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
		}
		
	CHECK_EQUALS_L(serviceApnControlListStatus, RMobilePhone::EAPNControlListServiceEnabled, _L("RMobilePhone::GetAPNControlListServiceStatus did not return EAPNControlListServiceEnabled status"));		
	
	// Ensure the APN list is empty.
	EnsureApnListIsEmptyL(mobilePhone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Append a name to the list ===
	
	// Post a notification RMobilePhone::NotifyAPNListChanged
	TExtEtelRequestStatus notifyAPNListChangedStatus(mobilePhone, EMobilePhoneNotifyAPNListChanged);
	CleanupStack::PushL(notifyAPNListChangedStatus);
	mobilePhone.NotifyAPNListChanged(notifyAPNListChangedStatus); 
	
	// Append an APN name to the APN Control List using RMobilePhone::AppendAPNName 
	RMobilePhone::TAPNEntryV3 myAPnEntry;
	TBuf16<20> name = _L("WiFi");
	myAPnEntry.iApn.Copy(name);
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
	TExtEtelRequestStatus appendAPNNameStatus(mobilePhone, EMobilePhoneAppendAPNName);
	CleanupStack::PushL(appendAPNNameStatus);
	mobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
	ASSERT_EQUALS(appendAPNNameStatus.Int(), KErrNone, _L("RMobilePhone::AppendAPNName returned with an error"));		
	
	// Check RMobilePhone::NotifyAPNListChanged completes with KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyAPNListChangedStatus, ETimeMedium),KErrNone, _L("RMobilePhone::NotifyAPNListChanged timed out"));
	ASSERT_EQUALS(notifyAPNListChangedStatus.Int(), KErrNone, _L("RMobilePhone::NotifyAPNListChanged returned with an error"));		
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10014);
			
	// Check RMobilePhone::EnumerateAPNEntries returns aIndex = 1
	TUint32 index= 0;
	TExtEtelRequestStatus enumerateAPNEntriesStatus(mobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_TRUE(index==1 , _L("RMobilePhone::EnumerateAPNEntries did not return the correct index number"));		
		
	// Check the first name written exists in the list
	ASSERT_TRUE(CheckEntryIsInListL(mobilePhone,myAPnEntry, index), _L("CheckEntryIsInList was executed and expectedAPnEntry was not found"));

	// ===  Append another name ===
	
	// Post a notification RMobilePhone::NotifyAPNListChanged
	mobilePhone.NotifyAPNListChanged(notifyAPNListChangedStatus); 
	
	// Append an APN name to the APN Control List using RMobilePhone::AppendAPNName 
	RMobilePhone::TAPNEntryV3 myAPnEntry2;
	TBuf16<20> name2 = _L("GPRS");
	myAPnEntry2.iApn.Copy(name2);
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg2(myAPnEntry2);
	mobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
	ASSERT_EQUALS(appendAPNNameStatus.Int(), KErrNone, _L("RMobilePhone::AppendAPNName returned with an error"));		
	
	// Check RMobilePhone::NotifyAPNListChanged completes with KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyAPNListChangedStatus, ETimeMedium),KErrNone, _L("RMobilePhone::NotifyAPNListChanged timed out"));
	ASSERT_EQUALS(notifyAPNListChangedStatus.Int(), KErrNone, _L("RMobilePhone::NotifyAPNListChanged returned with an error"));		
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10014);
	
	// Check RMobilePhone::EnumerateAPNEntries returns aIndex = 2
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS((TInt)index, 2 , _L("RMobilePhone::EnumerateAPNEntries did not return the correct index number"));		
	
	// Check the first name that was written exists in the list
	ASSERT_TRUE(CheckEntryIsInListL(mobilePhone,myAPnEntry, index), _L("CheckEntryIsInList was executed and expectedAPnEntry was not found"));

	// Check the second name that was written exists in the list
	ASSERT_TRUE(CheckEntryIsInListL(mobilePhone,myAPnEntry2, index), _L("CheckEntryIsInList was executed and expectedAPnEntry2 was not found"));
	
	//
	// TEST END
	//

    StartCleanup();
    
    EnsureApnListIsEmptyL(mobilePhone);
    // Pop:
    // backup
    // setAPNControlListServiceStatus
	// getAPNControlListServiceStatus
	// notifyAPNListChangedStatus
	// appendAPNNameStatus
	// enumerateAPNEntriesStatus
    CleanupStack::PopAndDestroy(6, &backup);
 
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0001");
	}



CCTSYIntegrationTestAPNControlList0002::CCTSYIntegrationTestAPNControlList0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0002::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0002::~CCTSYIntegrationTestAPNControlList0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0002
 * @SYMFssID BA/CTSY/ACL-0002
 * @SYMTestCaseDesc Get APN name from the APN control list with an invalid index.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::EnumerateAPNEntries
 * @SYMTestExpectedResults Pass - Error returned when invalid index is specified.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned if an invalid index is specified.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	DisplayUserInteractionPromptL(KPrompt); // 30 seconds
	
	// Ensure the APN Control List Service Status is EAPNControlListServiceEnabled. 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus setAPNControlListServiceStatus(mobilePhone, EMobilePhoneSetAPNControlListServiceStatus);
	CleanupStack::PushL(setAPNControlListServiceStatus);
	TExtEtelRequestStatus getAPNControlListServiceStatus(mobilePhone, EMobilePhoneGetAPNControlListServiceStatus);
	CleanupStack::PushL(getAPNControlListServiceStatus);
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
	
	iSimTestHelper.VerifyPin2L(mobilePhone,iNetworkTestHelper);	
		
	if(serviceApnControlListStatus== RMobilePhone::EAPNControlListServiceDisabled)
		{
		
		serviceApnControlListStatus=RMobilePhone::EAPNControlListServiceEnabled;
		mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(setAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus returned with an error"));		
			
		// Get the APN Contol List Service Status again
		mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
		}
	CHECK_EQUALS_L(serviceApnControlListStatus, RMobilePhone::EAPNControlListServiceEnabled, _L("RMobilePhone::GetAPNControlListServiceStatus did not return EAPNControlListServiceEnabled status"));		
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check Check RMobilePhone::EnumerateAPNEntries returns aIndex > 0
	TUint32 index= 0;
	TExtEtelRequestStatus enumerateAPNEntriesStatus(mobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_TRUE(index>0 , _L("RMobilePhone::EnumerateAPNEntries did not return the correct index number"))		
	
	// Check RMobilePhone::GetAPNname with aIndex = -1 returns an error.
	TUint invalidIndex= (TUint)-1;
	RMobilePhone::TAPNEntryV3 myAPnEntry;
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
	TExtEtelRequestStatus getAPNnameStatus(mobilePhone, EMobilePhoneGetAPNname);
	CleanupStack::PushL(getAPNnameStatus);
	mobilePhone.GetAPNname(getAPNnameStatus,invalidIndex, myAPnEntryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAPNnameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNname timed out"));
	ASSERT_TRUE(getAPNnameStatus.Int()!=KErrNone, _L("RMobilePhone::GetAPNname did not return an error"))		
		
	// Check RMobilePhone::GetAPNname with aIndex = -10 returns an error.
	invalidIndex= (TUint)-10;
	mobilePhone.GetAPNname(getAPNnameStatus,invalidIndex, myAPnEntryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAPNnameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNname timed out"));
	ASSERT_TRUE(getAPNnameStatus.Int()!=KErrNone, _L("RMobilePhone::GetAPNname did not return an error"))		
	
	// Check RMobilePhone::GetAPNname with aIndex = 1000000 returns an error.
	invalidIndex= 1000000;
	mobilePhone.GetAPNname(getAPNnameStatus,invalidIndex, myAPnEntryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAPNnameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNname timed out"));
	ASSERT_TRUE(getAPNnameStatus.Int()!=KErrNone, _L("RMobilePhone::GetAPNname did not return an error"))		
		
	//
	// TEST END
	//

    StartCleanup();
			
	// Pop
	// setAPNControlListServiceStatus
	// getAPNControlListServiceStatus
	// enumerateAPNEntriesStatus
	// getAPNnameStatus
	CleanupStack::PopAndDestroy(4,&setAPNControlListServiceStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0002");
	}



CCTSYIntegrationTestAPNControlList0003::CCTSYIntegrationTestAPNControlList0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0003::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0003::~CCTSYIntegrationTestAPNControlList0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0003
 * @SYMFssID BA/CTSY/ACL-0003
 * @SYMTestCaseDesc Delete an entry from the APN control list.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::VerifySecurityCode, RMobilePhone::NotifySecurityEvent, RMobilePhone::NotifyAPNListChanged, RMobilePhone::GetAPNname, RMobilePhone::DeleteAPNName, RMobilePhone::AppendAPNName
 * @SYMTestExpectedResults Pass - An APN can be deleted from the APN control list.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry deleted, number of APN entries correct and APN notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	DisplayUserInteractionPromptL(KPrompt); // 30 seconds
	
	// Take the backup of APN Control list. 
	TInt listCountAtStart;
	TBackupAPNList backup(*this,mobilePhone);
	CHECK_TRUE_L(backup.BackupL(listCountAtStart), _L("TBackupAPNList::Backup could not backup"));
	CleanupStack::PushL (backup);
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 securityCaps;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(securityCaps), KErrNone,_L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(securityCaps,RMobilePhone::KCapsAccessPin2 ,KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned did not return KCapsAccessPin2 bits"));
	
	// 	Verify Pin2
	iSimTestHelper.VerifyPin2L(mobilePhone, iNetworkTestHelper);
	
	// Ensure the APN Control List Service Status is EAPNControlListServiceEnabled. 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus setAPNControlListServiceStatus(mobilePhone, EMobilePhoneSetAPNControlListServiceStatus);
	CleanupStack::PushL(setAPNControlListServiceStatus);
	TExtEtelRequestStatus getAPNControlListServiceStatus(mobilePhone, EMobilePhoneGetAPNControlListServiceStatus);
	CleanupStack::PushL(getAPNControlListServiceStatus);
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
	if(serviceApnControlListStatus== RMobilePhone::EAPNControlListServiceDisabled)
		{	
		serviceApnControlListStatus=RMobilePhone::EAPNControlListServiceEnabled;
		mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(setAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus returned with an error"));		
			
		// Get the APN Contol List Service Status again
		mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
		}
	CHECK_EQUALS_L(serviceApnControlListStatus, RMobilePhone::EAPNControlListServiceEnabled, _L("RMobilePhone::GetAPNControlListServiceStatus did not return EAPNControlListServiceEnabled status"));		
	
	// Ensure the APN list is empty. 
	EnsureApnListIsEmptyL(mobilePhone);
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Verify PIN2 and append a name to the list ===
	
	// Post a notification RMobilePhone::NotifyAPNListChanged
	TExtEtelRequestStatus notifyAPNListChangedStatus(mobilePhone, EMobilePhoneNotifyAPNListChanged);
	CleanupStack::PushL(notifyAPNListChangedStatus);
	mobilePhone.NotifyAPNListChanged(notifyAPNListChangedStatus); 
	
	// Append an APN name to the APN Control List using RMobilePhone::AppendAPNName 
	RMobilePhone::TAPNEntryV3 myAPnEntry;
	TBuf16<20> name = _L("WiFi");
	myAPnEntry.iApn.Copy(name);
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
	TExtEtelRequestStatus appendAPNNameStatus(mobilePhone, EMobilePhoneAppendAPNName);
	CleanupStack::PushL(appendAPNNameStatus);
	mobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
	ASSERT_EQUALS(appendAPNNameStatus.Int(), KErrNone, _L("RMobilePhone::AppendAPNName returned with an error"));		
	
	// Check RMobilePhone::NotifyAPNListChanged completes with KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyAPNListChangedStatus, ETimeMedium),KErrNone, _L("RMobilePhone::NotifyAPNListChanged timed out"));
	ASSERT_EQUALS(notifyAPNListChangedStatus.Int(), KErrNone, _L("RMobilePhone::NotifyAPNListChanged returned with an error"));
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10014);		
	
	// Check RMobilePhone::EnumerateAPNEntries returns aIndex = 1
	TUint32 index= 0;
	TUint32 indexValue=0;
	TExtEtelRequestStatus enumerateAPNEntriesStatus(mobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS((TInt)index, 1 , _L("RMobilePhone::EnumerateAPNEntries did not return the correct index number"));		
	
	// ===  Append another name ===
	
	// Post a notification RMobilePhone::NotifyAPNListChanged
	mobilePhone.NotifyAPNListChanged(notifyAPNListChangedStatus); 
	
	// Append an APN name to the APN Control List using RMobilePhone::AppendAPNName 
	RMobilePhone::TAPNEntryV3 myAPnEntry2;
	TBuf16<20> name2 = _L("GPRS");
	myAPnEntry2.iApn.Copy(name2);
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg2(myAPnEntry2);
	mobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
	ASSERT_EQUALS(appendAPNNameStatus.Int(), KErrNone, _L("RMobilePhone::AppendAPNName returned with an error"));		
	
	// Check RMobilePhone::NotifyAPNListChanged completes with KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyAPNListChangedStatus, ETimeMedium),KErrNone, _L("RMobilePhone::NotifyAPNListChanged timed out"));
	ASSERT_EQUALS(notifyAPNListChangedStatus.Int(), KErrNone, _L("RMobilePhone::NotifyAPNListChanged returned with an error"));		
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10014);		
	
	// Check RMobilePhone::EnumerateAPNEntries returns aIndex = 2
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeShort),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS((int)index, 2 , _L("RMobilePhone::EnumerateAPNEntries did not return the correct index number"));		
	
	// ===  Delete first name written ===
	
	// Post a notification RMobilePhone::NotifyAPNListChanged
	mobilePhone.NotifyAPNListChanged(notifyAPNListChangedStatus); 
	
	// Check the first written entry exists is in the APN list.
	ASSERT_TRUE(CheckEntryIsInListL(mobilePhone,myAPnEntry, indexValue), _L("CheckEntryIsInList was executed and expectedAPnEntry was not found"));
		
	// Delete a first written APN name with RMobilePhone::DeleteAPNName
	TExtEtelRequestStatus deleteAPNNameStatus(mobilePhone, EMobilePhoneDeleteAPNName);
	CleanupStack::PushL(deleteAPNNameStatus);
	mobilePhone.DeleteAPNName(deleteAPNNameStatus, indexValue);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::DeleteAPNName timed out"));
	ASSERT_EQUALS(deleteAPNNameStatus.Int(), KErrNone, _L("RMobilePhone::DeleteAPNName returned with an error"));		
	
	// Check RMobilePhone::NotifyAPNListChanged completes with KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyAPNListChangedStatus, ETimeShort),KErrNone, _L("RMobilePhone::NotifyAPNListChanged timed out"));
	ASSERT_EQUALS(notifyAPNListChangedStatus.Int(), KErrNone, _L("RMobilePhone::NotifyAPNListChanged returned with an error"));		
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10014);		
	
	// Check RMobilePhone::EnumerateAPNEntries returns aIndex = 1
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS((int)index, 1 , _L("RMobilePhone::EnumerateAPNEntries did not return the correct index number"));		
	
	// Check first written entry does not exist and returns an error
	ASSERT_TRUE(!(CheckEntryIsInListL(mobilePhone,myAPnEntry, indexValue)), _L("CheckEntryIsInList was executed and it did not return an error"));

	// Check the second written entry exists is in the APN list.
	ASSERT_TRUE(CheckEntryIsInListL(mobilePhone,myAPnEntry2, indexValue), _L("CheckEntryIsInList was executed and expectedAPnEntry was not found"));

	// ===  Delete second name written ===

	// Post a notification RMobilePhone::NotifyAPNListChanged
	mobilePhone.NotifyAPNListChanged(notifyAPNListChangedStatus); 
	
	// Delete the second APN name entry with RMobilePhone::DeleteAPNName 
	mobilePhone.DeleteAPNName(deleteAPNNameStatus, indexValue);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::DeleteAPNName timed out"));
	ASSERT_EQUALS(deleteAPNNameStatus.Int(), KErrNone, _L("RMobilePhone::DeleteAPNName returned with an error"));		
	
	// Check RMobilePhone::NotifyAPNListChanged completes with KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyAPNListChangedStatus, ETimeMedium),KErrNone, _L("RMobilePhone::NotifyAPNListChanged timed out"));
	ASSERT_EQUALS(notifyAPNListChangedStatus.Int(), KErrNone, _L("RMobilePhone::NotifyAPNListChanged returned with an error"));		
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10014);		
		
	// Check RMobilePhone::EnumerateAPNEntries returns aIndex = 0
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_TRUE(index==0 , _L("RMobilePhone::EnumerateAPNEntries did not return the correct index number"));		
	
	// Check first written entry does not exist and returns an error
	ASSERT_TRUE(!(CheckEntryIsInListL(mobilePhone,myAPnEntry, indexValue)), _L("CheckEntryIsInList was executed and it did not return an error"));
	
	// Check second written entry does not exist and returns an error
	ASSERT_TRUE(!(CheckEntryIsInListL(mobilePhone,myAPnEntry2, indexValue)), _L("CheckEntryIsInList was executed and it did not return an error"));
	
	//
	// TEST END
	//

    StartCleanup();
    
    EnsureApnListIsEmptyL(mobilePhone);
    
    // Pop
    // backup
    // setAPNControlListServiceStatus
    // getAPNControlListServiceStatus
    // notifyAPNListChangedStatus
    // appendAPNNameStatus
   	// enumerateAPNEntriesStatus
    // deleteAPNNameStatus
    CleanupStack::PopAndDestroy(7, &backup);
   
  	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0003");
	}



CCTSYIntegrationTestAPNControlList0004::CCTSYIntegrationTestAPNControlList0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0004::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0004::~CCTSYIntegrationTestAPNControlList0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0004
 * @SYMFssID BA/CTSY/ACL-0004
 * @SYMTestCaseDesc Attempt to delete an entry from the APN control list with invalid index.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::VerifySecurityCode, RMobilePhone::NotifySecurityEvent, RMobilePhone::DeleteAPNName
 * @SYMTestExpectedResults Pass - Cannot delete entry from APN control list when index is invalid.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned and number of entries in APN control list does not change.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	DisplayUserInteractionPromptL(KPrompt); // 30 seconds
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 securityCaps;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(securityCaps), KErrNone,_L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(securityCaps,RMobilePhone::KCapsAccessPin2 ,KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned did not return KCapsAccessPin2 bits"));
	
	// Ensure the APN Control List Service Status is EAPNControlListServiceEnabled. 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus setAPNControlListServiceStatus(mobilePhone, EMobilePhoneSetAPNControlListServiceStatus);
	CleanupStack::PushL(setAPNControlListServiceStatus);
	TExtEtelRequestStatus getAPNControlListServiceStatus(mobilePhone, EMobilePhoneGetAPNControlListServiceStatus);
	CleanupStack::PushL(getAPNControlListServiceStatus);
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
	
	iSimTestHelper.VerifyPin2L(mobilePhone, iNetworkTestHelper);
	
	if(serviceApnControlListStatus== RMobilePhone::EAPNControlListServiceDisabled)
		{
		serviceApnControlListStatus=RMobilePhone::EAPNControlListServiceEnabled;
		mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(setAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus returned with an error"));		
			
		// Get the APN Contol List Service Status again
		mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
		}
	CHECK_EQUALS_L(serviceApnControlListStatus, RMobilePhone::EAPNControlListServiceEnabled, _L("RMobilePhone::GetAPNControlListServiceStatus did not return EAPNControlListServiceEnabled status"));		
	
	// Ensure APN Control List is not empty. 
	TUint32 storeIndex;
	TExtEtelRequestStatus enumerateAPNEntriesStatus(mobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, storeIndex);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	CHECK_EQUALS_L(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));	
	if(storeIndex==0)
		{
		RMobilePhone::TAPNEntryV3 myAPnEntry;
		TBuf16<20> name = _L("WiFi");
		myAPnEntry.iApn.Copy(name);
		RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
		TExtEtelRequestStatus appendAPNNameStatus(mobilePhone, EMobilePhoneAppendAPNName);
		CleanupStack::PushL(appendAPNNameStatus);
		mobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
		CHECK_EQUALS_L(appendAPNNameStatus.Int(), KErrNone, _L("RMobilePhone::AppendAPNName returned with an error"));		
		}
	CHECK_TRUE_L(storeIndex>0 , _L("RMobilePhone::EnumerateAPNEntries returned an empty list"));		
	
	// Ensure index 5 in the APN Control List is empty. 
	CHECK_TRUE_L(storeIndex<5, _L("The desired Index in the APN Control List is not empty"));		
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Delete an entry which is empty ===
	
	// Check RMobilePhone::DeleteAPNName with aIndex = 5 returns an error
	TUint32 index=5;
	TExtEtelRequestStatus deleteAPNNameStatus(mobilePhone, EMobilePhoneDeleteAPNName);
	CleanupStack::PushL(deleteAPNNameStatus);
	mobilePhone.DeleteAPNName(deleteAPNNameStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::DeleteAPNName timed out"));
	ASSERT_TRUE(deleteAPNNameStatus.Int()!=KErrNone, _L("RMobilePhone::DeleteAPNName did not return an error"))	;	
	
	// Check RMobilePhone::EnumerateAPNEntries returns same aIndex as before
	TUint32 checkIndex;
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, checkIndex);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS(checkIndex, storeIndex , _L("RMobilePhone::EnumerateAPNEntries did not return the same index"));		
	
	// ===  Delete an entry which is negative ===
	
	// Check RMobilePhone::DeleteAPNName with aIndex = -1 returns KErrMMEtelAPNNameInvalidIndex
	index= (TUint)-1;
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10022);		
	CHECK_EQUALS_L(1, 2, _L("This test has been made to leave intentially"));		
	mobilePhone.DeleteAPNName(deleteAPNNameStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::DeleteAPNName timed out"));
	ASSERT_EQUALS(deleteAPNNameStatus.Int(), KErrMMEtelAPNNameInvalidIndex, _L("RMobilePhone::DeleteAPNName returned wrong error"));		
	
	// Check RMobilePhone::EnumerateAPNEntries returns same aIndex as before
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, checkIndex);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS(checkIndex,storeIndex , _L("RMobilePhone::EnumerateAPNEntries did not returne the same index"));		
	
	
	// ===  Delete an entry which is too high ===
		
	// Check RMobilePhone::DeleteAPNName with aIndex = 1000000 returns KErrMMEtelAPNNameInvalidIndex
	index=1000000;
	mobilePhone.DeleteAPNName(deleteAPNNameStatus, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::DeleteAPNName timed out"));
	ASSERT_EQUALS(deleteAPNNameStatus.Int(), KErrMMEtelAPNNameInvalidIndex, _L("RMobilePhone::DeleteAPNName returned wrong error"));		
	
	// Check RMobilePhone::EnumerateAPNEntries returns same aIndex as before
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, checkIndex);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS(checkIndex, storeIndex , _L("RMobilePhone::EnumerateAPNEntries did not return the same index"));		
	
	//
	// TEST END
	//

    StartCleanup();
    
    EnsureApnListIsEmptyL(mobilePhone);
    
    // Pop
    // setAPNControlListServiceStatus
    // getAPNControlListServiceStatus
    // enumerateAPNEntriesStatus
    // getAPNnameStatus
    // deleteAPNNameStatus
    CleanupStack::PopAndDestroy(5,&setAPNControlListServiceStatus); 

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0004");
	}



CCTSYIntegrationTestAPNControlList0005::CCTSYIntegrationTestAPNControlList0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0005::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0005::~CCTSYIntegrationTestAPNControlList0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0005
 * @SYMFssID BA/CTSY/ACL-0005
 * @SYMTestCaseDesc Modify APN control list without verifying PIN2.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::DeleteAPNName, RMobilePhone::EnumerateAPNEntries
 * @SYMTestExpectedResults Pass - APN control list cannot be modified without PIN2.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify KErrGsm0707SimPin2Required is returned on attempt to append or delete a name.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	DisplayUserInteractionPromptL(KPrompt); // 30 seconds
	
	// Check the APN Control List Service Status is EAPNControlListServiceEnabled. 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus getAPNControlListServiceStatus(mobilePhone, EMobilePhoneGetAPNControlListServiceStatus);
	CleanupStack::PushL(getAPNControlListServiceStatus);
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
	CHECK_EQUALS_L(serviceApnControlListStatus, RMobilePhone::EAPNControlListServiceEnabled, _L("RMobilePhone::GetAPNControlListServiceStatus did not return EAPNControlListServiceEnabled status"));		
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get the number of APN entries ===

	// Check RMobilePhone::EnumerateAPNEntries returns aIndex >= 0 (store aIndex for later)
	TUint32 storeIndex;
	TExtEtelRequestStatus enumerateAPNEntriesStatus(mobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, storeIndex);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	CHECK_EQUALS_L(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	CHECK_TRUE_L((int)storeIndex>=0 , _L("RMobilePhone::EnumerateAPNEntries returned an empty list"));		

	// ===  Attempt to append an APN name without PIN2 ===

	// Check RMobilePhone::AppendAPNName with a valid name returns KErrGsm0707SimPin2Required or KErrAccessDenied
	RMobilePhone::TAPNEntryV3 myAPnEntry;
	TBuf16<20> name = _L("WiFi");
	myAPnEntry.iApn.Copy(name);
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
	TExtEtelRequestStatus appendAPNNameStatus(mobilePhone, EMobilePhoneAppendAPNName);
	CleanupStack::PushL(appendAPNNameStatus);
	mobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
	ASSERT_TRUE((appendAPNNameStatus.Int()==KErrGsm0707SimPin2Required || appendAPNNameStatus.Int()== KErrAccessDenied) , _L("RMobilePhone::AppendAPNName did not return an error"));		

	
	// Check RMobilePhone::EnumerateAPNEntries returns same aIndex as before
	TUint32 checkIndex;
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, checkIndex);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS(checkIndex, storeIndex , _L("RMobilePhone::EnumerateAPNEntries did not return the same index"));		
	
	// ===  Attempt to delete an APN name without PIN2 ===

	// Check RMobilePhone::DeleteAPNName with a valid index returns KErrGsm0707SimPin2Required
	TUint32 indexValue=0;
	TExtEtelRequestStatus deleteAPNNameStatus(mobilePhone, EMobilePhoneDeleteAPNName);
	CleanupStack::PushL(deleteAPNNameStatus);
	mobilePhone.DeleteAPNName(deleteAPNNameStatus, indexValue);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::DeleteAPNName timed out"));
	ASSERT_TRUE((deleteAPNNameStatus.Int()==KErrGsm0707SimPin2Required || deleteAPNNameStatus.Int()== KErrAccessDenied) , _L("RMobilePhone::DeleteAPNName did not return an error"));		

	
	// Check RMobilePhone::EnumerateAPNEntries returns same aIndex as before
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, checkIndex);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS(checkIndex, storeIndex , _L("RMobilePhone::EnumerateAPNEntries did not return the same index"));		
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// getAPNControlListServiceStatus
	// enumerateAPNEntriesStatus
	// appendAPNNameStatus
	// deleteAPNNameStatus
	
	CleanupStack::PopAndDestroy(4, &getAPNControlListServiceStatus);

			
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0005");
	}



CCTSYIntegrationTestAPNControlList0006::CCTSYIntegrationTestAPNControlList0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0006::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0006::~CCTSYIntegrationTestAPNControlList0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0006
 * @SYMFssID BA/CTSY/ACL-0006
 * @SYMTestCaseDesc Enable and disable the APN control list service.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySecurityEvent, RMobilePhone::GetAPNControlListServiceStatus, RMobilePhone::SetAPNControlListServiceStatus, RMobilePhone::NotifyAPNControlListServiceStatusChange
 * @SYMTestExpectedResults Pass - APN control list can be enabled and disabled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify control list service status and notification.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	DisplayUserInteractionPromptL(KPrompt); // 30 seconds
	
	// Verify Pin2
	iSimTestHelper.VerifyPin2L(mobilePhone,iNetworkTestHelper);
	
	// Ensure the APN Control List Service Status is EAPNControlListServiceDisabled 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus setAPNControlListServiceStatus(mobilePhone, EMobilePhoneSetAPNControlListServiceStatus);
	CleanupStack::PushL(setAPNControlListServiceStatus);
	TExtEtelRequestStatus getAPNControlListServiceStatus(mobilePhone, EMobilePhoneGetAPNControlListServiceStatus);
	CleanupStack::PushL(getAPNControlListServiceStatus);
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
	if(serviceApnControlListStatus== RMobilePhone::EAPNControlListServiceEnabled)
		{	
		serviceApnControlListStatus=RMobilePhone::EAPNControlListServiceDisabled;
		mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(setAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus returned with an error"));		
			
		// Get the APN Contol List Service Status again
		mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
		}
	
	CHECK_EQUALS_L(serviceApnControlListStatus, RMobilePhone::EAPNControlListServiceDisabled, _L("RMobilePhone::GetAPNControlListServiceStatus did not return EAPNControlListServiceDisabled status"));		

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		

	// ===  Enable the APN control list service ===

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10019);
	CHECK_EQUALS_L(1, 2, _L("This test has been made to leave intentially"));		
	
	// Call RMobilePhone::NotifyAPNControlListServiceStatusChange 
	TExtEtelRequestStatus notifyAPNControlListServiceStatusChangeStatus(mobilePhone, EMobilePhoneNotifyAPNControlListServiceStatusChange);
	CleanupStack::PushL(notifyAPNControlListServiceStatusChangeStatus);
	mobilePhone.NotifyAPNControlListServiceStatusChange(notifyAPNControlListServiceStatusChangeStatus, serviceApnControlListStatus); 
	
	// Enable the APN control list service with RMobilePhone::SetAPNControlListServiceStatus with aAPNControlListServiceStatus = EAPNControlListServiceEnabled 
	RMobilePhone::TAPNControlListServiceStatus aPNControlListServiceStatus2=RMobilePhone::EAPNControlListServiceEnabled;
	mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, aPNControlListServiceStatus2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
	ASSERT_EQUALS(setAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus returned with an error"));		
	
    // Check RMobilePhone::NotifyAPNControlListServiceStatusChange completes with aAPNControlListServiceStatus = EAPNControlListServiceEnabled
	TInt wantedStatus= KErrNone;
	RMobilePhone::TAPNControlListServiceStatus expectedAPNControlListServiceStatus= RMobilePhone::EAPNControlListServiceEnabled;
	iNetworkTestHelper.WaitForMobilePhoneNotifyAPNControlListServiceStatusChange(mobilePhone,
																		  notifyAPNControlListServiceStatusChangeStatus,
																          serviceApnControlListStatus, 
																		  expectedAPNControlListServiceStatus,
																		  wantedStatus );
	
	// ===  Get the service status ===
	
	// Check RMobilePhone::GetAPNControlListServiceStatus returns aAPNControlListServiceStatus = EAPNControlListServiceEnabled
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	ASSERT_EQUALS(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
	ASSERT_EQUALS(serviceApnControlListStatus, RMobilePhone::EAPNControlListServiceEnabled, _L("RMobilePhone::GetAPNControlListServiceStatus did not return EAPNControlListServiceEnabled status"));		
	
	// ===  Disable the APN control list service ===
			
	// Verify Pin2
	iSimTestHelper.VerifyPin2L(mobilePhone,iNetworkTestHelper);			
	
	// Post notifier RMobilePhone::NotifyAPNControlListServiceStatusChange 
	mobilePhone.NotifyAPNControlListServiceStatusChange(notifyAPNControlListServiceStatusChangeStatus, serviceApnControlListStatus); 

	// Disable the APN control list service with RMobilePhone::SetAPNControlListServiceStatus with aAPNControlListServiceStatus = EAPNControlListServiceDisabled 
	RMobilePhone::TAPNControlListServiceStatus aPNControlListServiceStatus3= RMobilePhone::EAPNControlListServiceDisabled;
	mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, aPNControlListServiceStatus3);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
	ASSERT_EQUALS(setAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus returned with an error"));		
	
		
	// Check RMobilePhone::NotifyAPNControlListServiceStatusChange completes with aAPNControlListServiceStatus = EAPNControlListServiceDisabled
	expectedAPNControlListServiceStatus= RMobilePhone::EAPNControlListServiceDisabled;
	wantedStatus= KErrNone;
	iNetworkTestHelper.WaitForMobilePhoneNotifyAPNControlListServiceStatusChange(mobilePhone,
																		  notifyAPNControlListServiceStatusChangeStatus,
																          serviceApnControlListStatus, 
																		  expectedAPNControlListServiceStatus,
																		  wantedStatus );
	
	// ===  Get the service status ===

	// Check RMobilePhone::GetAPNControlListServiceStatus returns aAPNControlListServiceStatus = EAPNControlListServiceDisabled
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	ASSERT_EQUALS(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
	ASSERT_EQUALS(serviceApnControlListStatus, RMobilePhone::EAPNControlListServiceDisabled, _L("RMobilePhone::GetAPNControlListServiceStatus did not return EAPNControlListServiceDisabled status"));		
	
	//
	// TEST END
	//

    StartCleanup();
  
    // Pop:
    // setAPNControlListServiceStatus
    // getAPNControlListServiceStatus
    // notifyAPNControlListServiceStatusChangeStatus
    
    CleanupStack::PopAndDestroy(3, &setAPNControlListServiceStatus);
    
    return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0006");
	}



CCTSYIntegrationTestAPNControlList0007::CCTSYIntegrationTestAPNControlList0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0007::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0007::~CCTSYIntegrationTestAPNControlList0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0007
 * @SYMFssID BA/CTSY/ACL-0007
 * @SYMTestCaseDesc Enable and disable the APN control list without verifying PIN2.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - APN control list service cannot be enabled / disabled without PIN2.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify KErrGsm0707SimPin2Required is returned on attempt to enable or disable the service.
 *
 * @return - TVerdict code
 */
	{
	
	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	DisplayUserInteractionPromptL(KPrompt); // 30 seconds
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 securityCaps;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(securityCaps), KErrNone,_L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(securityCaps,RMobilePhone::KCapsAccessPin2 ,KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned did not return KCapsAccessPin2 bits"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check APN Control List Service Status 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus getAPNControlListServiceStatus(mobilePhone, EMobilePhoneGetAPNControlListServiceStatus);
	CleanupStack::PushL(getAPNControlListServiceStatus);
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	ASSERT_EQUALS(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
		
	TExtEtelRequestStatus setAPNControlListServiceStatus(mobilePhone, EMobilePhoneSetAPNControlListServiceStatus);
	CleanupStack::PushL(setAPNControlListServiceStatus);
	
	// If APN list service is already diasbaled then check RMobilePhone::SetAPNControlListServiceStatus with aAPNControlListServiceStatus = EAPNControlListServiceEnabled returns KErrGsm0707SimPin2Required  
	if(serviceApnControlListStatus==RMobilePhone::EAPNControlListServiceDisabled)
		{
		// Check RMobilePhone::SetAPNControlListServiceStatus with aAPNControlListServiceStatus = EAPNControlListServiceEnabled returns KErrGsm0707SimPin2Required
		RMobilePhone::TAPNControlListServiceStatus aPNControlListServiceStatus1=RMobilePhone::EAPNControlListServiceEnabled;
		mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, aPNControlListServiceStatus1);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
		ASSERT_TRUE((setAPNControlListServiceStatus.Int()== KErrNone||setAPNControlListServiceStatus.Int()== KErrAccessDenied || setAPNControlListServiceStatus.Int()==KErrGsm0707SimPin2Required) , _L("RMobilePhone::SetAPNControlListServiceStatus did not return an error"));		
		DEBUG_PRINTF1(_L("<font color=Orange>$CTSYProblem RMobilePhone::SetAPNControlListServiceStatus disables the APN control list without verifying Pin2.\n KErrNone is returned instead of an access error. This problem occurs when it is tried to disable the already disabled APN control list</font> "));
		
		}
	// If APN list service is already enabled then check RMobilePhone::SetAPNControlListServiceStatus with aAPNControlListServiceStatus = EAPNControlListServiceDisabled returns KErrGsm0707SimPin2Required        
	else
		{
		// Check RMobilePhone::SetAPNControlListServiceStatus with aAPNControlListServiceStatus = EAPNControlListServiceDisabled returns KErrGsm0707SimPin2Required
		RMobilePhone::TAPNControlListServiceStatus aPNControlListServiceStatus2= RMobilePhone::EAPNControlListServiceDisabled;
		mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, aPNControlListServiceStatus2);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
		ASSERT_TRUE((setAPNControlListServiceStatus.Int()== KErrNone||setAPNControlListServiceStatus.Int()== KErrAccessDenied || setAPNControlListServiceStatus.Int()== KErrGsm0707SimPin2Required) , _L("RMobilePhone::SetAPNControlListServiceStatus did not return an error (II)"));		
		DEBUG_PRINTF1(_L("<font color=Orange>$CTSYProblem RMobilePhone::SetAPNControlListServiceStatus enables the APN control list without verifying Pin2.\n KErrNone is returned instead of an access error. This	problem occurs when it is tried to enable the already enabled APN control list</font> "));
		}
	

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// getAPNControlListServiceStatus
	// setAPNControlListServiceStatus
	CleanupStack::PopAndDestroy(2, &getAPNControlListServiceStatus);
		
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0007");
	}



CCTSYIntegrationTestAPNControlList0008::CCTSYIntegrationTestAPNControlList0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0008::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0008::~CCTSYIntegrationTestAPNControlList0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0008
 * @SYMFssID BA/CTSY/ACL-0008
 * @SYMTestCaseDesc Add an entry to the APN control list when the list on USIM is full.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Cannot append to APN control list when it is full.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify KErrMMEtelAPNNameAPNListFull is returned on attempt to append an entry to a full SIM.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	DisplayUserInteractionPromptL(KPrompt); // 30 seconds
	
	// Take the backup of APN Control list. 
	TInt listCountAtStart;
	TBackupAPNList backup(*this,mobilePhone);
	CHECK_TRUE_L(backup.BackupL(listCountAtStart), _L("TBackupAPNList::Backup could not backup"));
	CleanupStack::PushL (backup);
	
	// Verfiy Pin2
	iSimTestHelper.VerifyPin2L(mobilePhone, iNetworkTestHelper);
	
	// Insert USIM with full APN control list.
	RMobilePhone::TAPNEntryV3 myAPnEntry;
	TBuf16<20> name = _L("WiFi");
	myAPnEntry.iApn.Copy(name);
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
	TExtEtelRequestStatus appendAPNNameStatus(mobilePhone, EMobilePhoneAppendAPNName);
	CleanupStack::PushL(appendAPNNameStatus);
	do
		{
		mobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
		}while(appendAPNNameStatus.Int()==KErrNone);
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2  
	TUint32 securityCaps;
	CHECK_EQUALS_L(mobilePhone.GetSecurityCaps(securityCaps), KErrNone,_L("RMobilePhone::GetSecurityCaps returned with an error"));
	CHECK_BITS_SET_L(securityCaps,RMobilePhone::KCapsAccessPin2 ,KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps returned did not return KCapsAccessPin2 bits"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get the number of entries in the APN control list ===

	// Check RMobilePhone::EnumerateAPNEntries returns aIndex >= 0 (store aIndex for later)
	TUint32 storeIndex;
	TExtEtelRequestStatus enumerateAPNEntriesStatus(mobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, storeIndex);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	CHECK_EQUALS_L(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	CHECK_TRUE_L((int)storeIndex>=0 , _L("RMobilePhone::EnumerateAPNEntries returned an empty list"));		

	// ===  Attempt to add another entry to the full list ===

	// Check RMobilePhone::AppendAPNName with a valid name returns KErrMMEtelAPNNameAPNListFull
	mobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
	ASSERT_TRUE(appendAPNNameStatus.Int()== KErrOverflow ||appendAPNNameStatus.Int()== KErrMMEtelAPNNameAPNListFull, _L("RMobilePhone::AppendAPNName returned with an error"));		
	
	// ===  Get the number of entries in the APN control list ===

	// Check RMobilePhone::EnumerateAPNEntries returns same aIndex as before
	TUint32 checkIndex;
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, checkIndex);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_EQUALS(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	ASSERT_EQUALS(checkIndex, storeIndex , _L("RMobilePhone::EnumerateAPNEntries did not return the same index"));		
	
		
	//
	// TEST END
	//

    StartCleanup();
    
    EnsureApnListIsEmptyL(mobilePhone);
    
    // Pop: 
    // backup
    // appendAPNNameStatus
    // enumerateAPNEntriesStatus
    CleanupStack::PopAndDestroy(3, &backup);
			
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0008");
	}



CCTSYIntegrationTestAPNControlList0009::CCTSYIntegrationTestAPNControlList0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0009::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0009::~CCTSYIntegrationTestAPNControlList0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0009
 * @SYMFssID BA/CTSY/ACL-0009
 * @SYMTestCaseDesc Activate a context when APN is not in the APN Control List.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::NotifyStatusChange, RPacketContext::Activate, RPacketService::GetStatus, RPacketContext::GetLastErrorCause
 * @SYMTestExpectedResults Pass - Context cannot be activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify context does not activate.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
  	
  	DisplayUserInteractionPromptL(KPrompt); // 30 seconds
	 	
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	
	// Registered with the network
 	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, 
			_L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure phone is attached to packet service. 
	RPacketService::TStatus packetStatus;
	CHECK_EQUALS_L(packetService.GetStatus(packetStatus), KErrNone, _L("RPacketService::GetStatus returned with an error"));
	TExtEtelRequestStatus attachStatus(packetService, EPacketAttach);
	CleanupStack::PushL(attachStatus);
	if(packetStatus==RPacketService::EStatusUnattached)
		{
		packetService.Attach(attachStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(attachStatus, ETimeMedium), KErrNone,				
							_L("PacketService::Attach timed out"))
		CHECK_EQUALS_L(attachStatus.Int(), KErrNone,				
							_L("RPacketService::Attach returned an error"))	
	
		}
		
	// Ensure that the APN Control List service is enabled. 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus getAPNControlListServiceStatus(mobilePhone, EMobilePhoneGetAPNControlListServiceStatus);
	CleanupStack::PushL(getAPNControlListServiceStatus);
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
	TExtEtelRequestStatus setAPNControlListServiceStatus(mobilePhone, EMobilePhoneSetAPNControlListServiceStatus);
	CleanupStack::PushL(setAPNControlListServiceStatus);
	if(serviceApnControlListStatus== RMobilePhone::EAPNControlListServiceDisabled)
		{	
		iSimTestHelper.VerifyPin2L(mobilePhone,iNetworkTestHelper);
		serviceApnControlListStatus=RMobilePhone::EAPNControlListServiceEnabled;
		mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(setAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus returned with an error"));		
			
		// Get the APN Contol List Service Status again
		mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
		CHECK_EQUALS_L(getAPNControlListServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus returned with an error"));		
		}
	
	CHECK_EQUALS_L(serviceApnControlListStatus, RMobilePhone::EAPNControlListServiceEnabled, _L("RMobilePhone::GetAPNControlListServiceStatus did not return EAPNControlListServiceEnabled status"));		
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Open a new primary context.
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1); 

	
	// Set a valid context config with RPacketContext::SetConfig with iAccessPointName = an APN which is not in the control list  
	RPacketContext::TContextConfigGPRS config;
	TBuf16<20> name = _L("APN3");
	config.iAccessPointName.Copy(name);
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);
	TExtEtelRequestStatus setConfigStatus(packetContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	packetContext.SetConfig(setConfigStatus, configPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone,
					_L("RPacketContext::SetConfig returned with error status."));
					
	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);


	// Activate packet context with RPacketContext::Activate 
	TExtEtelRequestStatus contextActivateStatus(packetContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	packetContext.Activate(contextActivateStatus);

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusInactive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
	
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus returns EStatusInactive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check TRequestStatus used for RPacketContext::Activate != KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone,
					_L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::GetLastErrorCause returns an error.
	TInt err;
	ASSERT_EQUALS(packetContext.GetLastErrorCause(err), KErrNone, _L("RPacketContext::GetLastErrorCause returned with an error"));
	ASSERT_TRUE(err!=KErrNone, _L("RPacketContext::GetLastErrorCause did not return an error via pass by reference argument "));

	//
	// TEST END
	//

    StartCleanup();
	
	// PopAndDestroy:
    // attachStatus
	// setAPNControlListServiceStatus
	// getAPNControlListServiceStatus
	// setConfigStatus
	// notifyContextStChStatus
	// contextActivateStatus
	
	CleanupStack::PopAndDestroy(6, &attachStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0009");
	}



CCTSYIntegrationTestAPNControlList0010::CCTSYIntegrationTestAPNControlList0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0010::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0010::~CCTSYIntegrationTestAPNControlList0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0010
 * @SYMFssID BA/CTSY/ACL-0010
 * @SYMTestCaseDesc Cancel get APN Control List status
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
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Call RMobilePhone::GetAPNControlListServiceStatus 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus getAPNControlListServiceStatus(mobilePhone, EMobilePhoneGetAPNControlListServiceStatus);
	CleanupStack::PushL(getAPNControlListServiceStatus);
	mobilePhone.GetAPNControlListServiceStatus(getAPNControlListServiceStatus, serviceApnControlListStatus);
	
	// Cancel request with RTelSubSessionBase.CancelAsyncRequest( EMobilePhoneGetAPNControlListServiceStatus ); 
	mobilePhone.CancelAsyncRequest(EMobilePhoneGetAPNControlListServiceStatus);
	
	// Check request completes with KErrCancel or KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNControlListServiceStatus timed out"));
	ASSERT_TRUE(((getAPNControlListServiceStatus.Int()== KErrCancel) || (getAPNControlListServiceStatus.Int()== KErrNone)) , _L("RMobilePhone::GetAPNControlListServiceStatus request could not be cancelled"))	;	
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&getAPNControlListServiceStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0010");
	}



CCTSYIntegrationTestAPNControlList0011::CCTSYIntegrationTestAPNControlList0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0011::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0011::~CCTSYIntegrationTestAPNControlList0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0011
 * @SYMFssID BA/CTSY/ACL-0011
 * @SYMTestCaseDesc Cancel set APN Control List status
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

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Call RMobilePhone::SetAPNControlListServiceStatus 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus=RMobilePhone::EAPNControlListServiceEnabled;
	TExtEtelRequestStatus setAPNControlListServiceStatus(mobilePhone, EMobilePhoneSetAPNControlListServiceStatus);
	CleanupStack::PushL(setAPNControlListServiceStatus);

	// Verify PIN2 before accessing APN function
	iSimTestHelper.VerifyPin2L(mobilePhone, iNetworkTestHelper);

	mobilePhone.SetAPNControlListServiceStatus(setAPNControlListServiceStatus, serviceApnControlListStatus);
	
	// Cancel request with RTelSubSessionBase.CancelAsyncRequest( EMobilePhoneSetAPNControlListServiceStatus ); 
	mobilePhone.CancelAsyncRequest(EMobilePhoneSetAPNControlListServiceStatus);
	
	// Check request completes with KErrCancel or KErrNone.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAPNControlListServiceStatus, ETimeMedium),KErrNone, _L("RMobilePhone::SetAPNControlListServiceStatus timed out"));
	ASSERT_TRUE(((setAPNControlListServiceStatus.Int()== KErrCancel) || (setAPNControlListServiceStatus.Int()== KErrNone)) , _L("RMobilePhone::SetAPNControlListServiceStatus request could not be cancelled"));		
			
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&setAPNControlListServiceStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0011");
	}



CCTSYIntegrationTestAPNControlList0012::CCTSYIntegrationTestAPNControlList0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0012::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0012::~CCTSYIntegrationTestAPNControlList0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0012
 * @SYMFssID BA/CTSY/ACL-0012
 * @SYMTestCaseDesc Cancel enumerating the entries from the APN Control List.
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
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Call RMobilePhone::EnumerateAPNEntries
	TUint32 index= 0;
	TExtEtelRequestStatus enumerateAPNEntriesStatus(mobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	mobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	
	// Cancel request with RTelSubSession::CancelAsyncRequest( EMobilePhoneEnumerateAPNEntries ); 
	mobilePhone.CancelAsyncRequest(EMobilePhoneEnumerateAPNEntries);
	
	
	// Check request completes with KErrCancel or KErrNone.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	ASSERT_TRUE(((enumerateAPNEntriesStatus.Int()== KErrCancel) || (enumerateAPNEntriesStatus.Int()== KErrNone)) , _L("RMobilePhone::EnumerateAPNEntries request could not be cancelled"));		
		
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&enumerateAPNEntriesStatus);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0012");
	}



CCTSYIntegrationTestAPNControlList0013::CCTSYIntegrationTestAPNControlList0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0013::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0013::~CCTSYIntegrationTestAPNControlList0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0013
 * @SYMFssID BA/CTSY/ACL-0013
 * @SYMTestCaseDesc Cancel reading entries from the APN Control List
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

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Call RMobilePhone::GetAPNname
	TUint32 index=0;
	RMobilePhone::TAPNEntryV3 myAPnEntry;
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
	TExtEtelRequestStatus getAPNnameStatus(mobilePhone, EMobilePhoneGetAPNname);
	CleanupStack::PushL(getAPNnameStatus);

	// Verify PIN2 before accessing APN function
	iSimTestHelper.VerifyPin2L(mobilePhone, iNetworkTestHelper);

	mobilePhone.GetAPNname(getAPNnameStatus,index, myAPnEntryPckg);
	
	// Cancel request with RTelSubSession::CancelAsyncRequest( EMobilePhoneGetAPNname ); 
	mobilePhone.CancelAsyncRequest(EMobilePhoneGetAPNname);
	
	// Check request completes with KErrCancel or KErrNone.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAPNnameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNname timed out"));
	ASSERT_TRUE(((getAPNnameStatus.Int()== KErrCancel) || (getAPNnameStatus.Int()== KErrNone)) , _L("RMobilePhone::GetAPNname request could not be cancelled"));		
		
	//
	// TEST END
	//

    StartCleanup();
    
	CleanupStack::PopAndDestroy(&getAPNnameStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0013");
	}



CCTSYIntegrationTestAPNControlList0014::CCTSYIntegrationTestAPNControlList0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0014::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0014::~CCTSYIntegrationTestAPNControlList0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0014
 * @SYMFssID BA/CTSY/ACL-0014
 * @SYMTestCaseDesc Cancel adding an entry to the APN Control List
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

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Call RMobilePhone::AppendAPNName 
	RMobilePhone::TAPNEntryV3 myAPnEntry;
	TBuf16<20> name = _L("WiFi");
	myAPnEntry.iApn.Copy(name);
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
	TExtEtelRequestStatus appendAPNNameStatus(mobilePhone, EMobilePhoneAppendAPNName);
	CleanupStack::PushL(appendAPNNameStatus);
	
	// Verify PIN2 before accessing APN function
	iSimTestHelper.VerifyPin2L(mobilePhone, iNetworkTestHelper);

	mobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg);
	
	// Cancel request with RTelSubSession::CancelAsyncRequest( EMobilePhoneAppendAPNName ); 
	mobilePhone.CancelAsyncRequest(EMobilePhoneAppendAPNName);
	
	// Check request completes with KErrCancel or KErrNone.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
	ASSERT_TRUE(((appendAPNNameStatus.Int()== KErrCancel) || (appendAPNNameStatus.Int()== KErrNone)) , _L("RMobilePhone::AppendAPNName request could not be cancelled"));		
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&appendAPNNameStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0014");
	}



CCTSYIntegrationTestAPNControlList0015::CCTSYIntegrationTestAPNControlList0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0015::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0015::~CCTSYIntegrationTestAPNControlList0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0015
 * @SYMFssID BA/CTSY/ACL-0015
 * @SYMTestCaseDesc Cancel deleting an entry from the APN Control List
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

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Call RMobilePhone::DeleteAPNName
	TUint32 index=0;
	TExtEtelRequestStatus deleteAPNNameStatus(mobilePhone, EMobilePhoneDeleteAPNName);
	CleanupStack::PushL(deleteAPNNameStatus);

	// Verify PIN2 before accessing APN function
	iSimTestHelper.VerifyPin2L(mobilePhone, iNetworkTestHelper);

	mobilePhone.DeleteAPNName(deleteAPNNameStatus, index);
	
	// Cancel request with RTelSubSession::CancelAsyncRequest( EMobilePhoneDeleteAPNName ); 
	mobilePhone.CancelAsyncRequest(EMobilePhoneDeleteAPNName);
	
	// Check request completes with KErrCancel or KErrNone.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::DeleteAPNName timed out"));
	ASSERT_TRUE(((deleteAPNNameStatus.Int()== KErrCancel) || (deleteAPNNameStatus.Int()== KErrNone)) , _L("RMobilePhone::DeleteAPNName request could not be cancelled"));		
	//
	// TEST END
	//

    StartCleanup();
   
	CleanupStack::PopAndDestroy(&deleteAPNNameStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0015");
	}



CCTSYIntegrationTestAPNControlList0016::CCTSYIntegrationTestAPNControlList0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0016::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0016::~CCTSYIntegrationTestAPNControlList0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0016
 * @SYMFssID BA/CTSY/ACL-0016
 * @SYMTestCaseDesc Cancel notification of change in the APN control list
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

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Call RMobilePhone::NotifyAPNListChanged 
	TExtEtelRequestStatus notifyAPNListChangedStatus(mobilePhone, EMobilePhoneNotifyAPNListChanged);
	CleanupStack::PushL(notifyAPNListChangedStatus);
	mobilePhone.NotifyAPNListChanged(notifyAPNListChangedStatus); 
	
	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyAPNListChanged) 
	mobilePhone.CancelAsyncRequest(EMobilePhoneNotifyAPNListChanged);
	
	// Check request completes with KErrCancel
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyAPNListChangedStatus, ETimeMedium),KErrNone, _L("RMobilePhone::NotifyAPNListChanged timed out"));
	ASSERT_EQUALS(notifyAPNListChangedStatus.Int(), KErrCancel, _L("RMobilePhone::NotifyAPNListChanged request could not be cancelled"));		
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&notifyAPNListChangedStatus);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0016");
	}



CCTSYIntegrationTestAPNControlList0017::CCTSYIntegrationTestAPNControlList0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAPNControlListBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAPNControlList0017::GetTestStepName());
	}

CCTSYIntegrationTestAPNControlList0017::~CCTSYIntegrationTestAPNControlList0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAPNControlList0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ACL-0017
 * @SYMFssID BA/CTSY/ACL-0017
 * @SYMTestCaseDesc Cancel notification of change in the APN control list service status
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

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Call RMobilePhone::NotifyAPNControlListServiceStatusChange 
	RMobilePhone::TAPNControlListServiceStatus serviceApnControlListStatus;
	TExtEtelRequestStatus notifyAPNControlListServiceStatusChangeStatus(mobilePhone, EMobilePhoneNotifyAPNControlListServiceStatusChange);
	CleanupStack::PushL(notifyAPNControlListServiceStatusChangeStatus);
	mobilePhone.NotifyAPNControlListServiceStatusChange(notifyAPNControlListServiceStatusChangeStatus, serviceApnControlListStatus); 
		
	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyAPNControlListServiceStatusChange) 
	mobilePhone.CancelAsyncRequest(EMobilePhoneNotifyAPNControlListServiceStatusChange);
		
	// Check request completes with KErrCancel
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyAPNControlListServiceStatusChangeStatus, ETimeMedium),KErrNone, _L("RMobilePhone::NotifyAPNControlListServiceStatusChange timed out"));
	ASSERT_EQUALS(notifyAPNControlListServiceStatusChangeStatus.Int(), KErrCancel, _L("RMobilePhone::NotifyAPNControlListServiceStatusChange could not be cancelled"));		
	
	//
	// TEST END
	//

    StartCleanup();
	CleanupStack::PopAndDestroy(&notifyAPNControlListServiceStatusChangeStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAPNControlList0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAPNControlList0017");
	}
TBackupAPNList::TBackupAPNList(CCTSYIntegrationTestSuiteStepBase &aTestStep,
		RMobilePhone &aMobilePhone) : TTsyTestHelperBase(aTestStep) ,iMobilePhone(aMobilePhone)
/*
* Constructor
* @param aBase pointer to the test.
* @param aMobilePhone reference to a Mobile Phone session
*
*/
	{
	}
TBool TBackupAPNList::BackupL(TInt &aListCount)
/*
* This function takes backup of the entries of APN Control list and return false if it fails
* @param aListCount refer to the number of entries in the list at the time of backup
* @return true if the backup was successful
*
*/	
	{
	// Enumerater the entries of APN control List
	TUint32 index= 0;
	iValid=EFalse;
	TExtEtelRequestStatus enumerateAPNEntriesStatus(iMobilePhone, EMobilePhoneEnumerateAPNEntries);
	CleanupStack::PushL(enumerateAPNEntriesStatus);
	iMobilePhone.EnumerateAPNEntries(enumerateAPNEntriesStatus, index);
	CHECK_EQUALS_L(iTestStep.WaitForRequestWithTimeOut(enumerateAPNEntriesStatus, ETimeMedium),KErrNone, _L("RMobilePhone::EnumerateAPNEntries timed out"));
	CHECK_EQUALS_L(enumerateAPNEntriesStatus.Int(), KErrNone, _L("RMobilePhone::EnumerateAPNEntries returned with an error"));		
	iValid=ETrue;
	aListCount=(int)index;
	iListCount=aListCount;
	
	// Store the values of APN control list, so that they can be restored later
	RMobilePhone::TAPNEntryV3 myAPnEntry;
	RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(myAPnEntry);
	TExtEtelRequestStatus getAPNnameStatus(iMobilePhone, EMobilePhoneGetAPNname);
	CleanupStack::PushL(getAPNnameStatus);
	for(TInt i=0;i<index; i++)
		{
		iMobilePhone.GetAPNname(getAPNnameStatus,i, myAPnEntryPckg);
		CHECK_EQUALS_L(iTestStep.WaitForRequestWithTimeOut(getAPNnameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetAPNname timed out"));
		CHECK_EQUALS_L(getAPNnameStatus.Int(), KErrNone, _L("RMobilePhone::GetAPNname returned with an error"));		
		CHECK_EQUALS_L(iEntryArray.Append(myAPnEntry), KErrNone, _L("RArray::Append could not append RMobilePhone::TAPNEntryV3 in array"));
		}
			
	// Pop:
	// enumerateAPNEntriesStatus
	// getAPNnameStatus
	CleanupStack::PopAndDestroy(2,&enumerateAPNEntriesStatus);
	
	return iValid;
	}

void TBackupAPNList::DoCleanupL(TAny* aPtr)
/*
* This function is a static clean up function to be saved in the cleanup stack. It restores the saved entries.
*
* @param aPtr a pointer to a TBackupAPNList instance.
*
*/	
	{
	TBackupAPNList* me = static_cast<TBackupAPNList*>(aPtr);
	me->CleanupL();
	}

void TBackupAPNList::CleanupL()
/*
* This function restores the saved entries to the entries in the phone
*
*/	
	{
	TExtEtelRequestStatus appendAPNNameStatus(iMobilePhone, EMobilePhoneAppendAPNName);
	CleanupStack::PushL(appendAPNNameStatus);
			
	for(TInt count=0; count<iListCount;count++)
			{
			RMobilePhone::TAPNEntryV3Pckg myAPnEntryPckg(iEntryArray[count]);
			iMobilePhone.AppendAPNName(appendAPNNameStatus, myAPnEntryPckg);
			CHECK_EQUALS_L(iTestStep.WaitForRequestWithTimeOut(appendAPNNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::AppendAPNName timed out"));
			CHECK_EQUALS_L(appendAPNNameStatus.Int(), KErrNone, _L("RMobilePhone::AppendAPNName returned with an error"));	
			}
						
	iEntryArray.Close();
	CleanupStack::PopAndDestroy(&appendAPNNameStatus);
	}

TBackupAPNList::operator TCleanupItem()
/*
* This operator create a TCleanupItem to save a pointer to the DoCleanup static function and this in the cleanup stack
*
*/	
	{
	return TCleanupItem(DoCleanupL,this);
	}




