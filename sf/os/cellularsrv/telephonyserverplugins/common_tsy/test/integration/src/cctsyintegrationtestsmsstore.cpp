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
// Test step definitions for the SMSStore functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestsmsstore.h"
#include "cctsyinidata.h"
#include "cetelsessionmgr.h"



/*
PDU LENGTH IS 140 BYTES
NO SMSC ADDRESS PRESENT
MESSAGE HEADER FLAGS
	MESSAGE TYPE :	SMS DELIVER
	MSGS WAITING IN SC :	NO
	SEND STATUS REPORT :	YES
	USER DATA HEADER :	NO UDH
	REPLY PATH :	NO

ORIGINATING ADDRESS
	NUMBER IS :	+441632960000
	TYPE OF NR. :	International
	NPI :	ISDN/Telephone (E.164/163)

PROTOCOL IDENTIFIER	(0x00)
	MESSAGE ENTITIES :	SME-to-SME
	PROTOCOL USED :	Implicit / SC-specific

DATA CODING SCHEME	(0x00)
	AUTO-DELETION :	OFF
	COMPRESSION :	OFF
	MESSAGE CLASS :	NONE
	ALPHABET USED :	7bit default

SMSC TIMESTAMP :	09/07/07 08:15 GMT+1.00

USER DATA PART OF SM
	USER DATA LENGTH : 	138 septets
	USER DATA (TEXT) :	TravelAlert 08:15: Nrthrn: Severe
		delays on the H Barnet Branch with
		minor delays on the rest of the line
		due to signal failure at Camden.
*/
_LIT8(KValidSmsPdu2, "\x24\x0c\x91\x44\x77\x79\x08\x80\x25\x00\x00\x70\x70\x90\x80\x51\x81\x40\x8a\x54\x79\xd8\x5e\x66\x07\xd9\x65\x39\x1d\x04\xc3\xe9\x62\x35\x1d\xc8\x29\xa7\xa3\xe5\x6e\x1d\x68\x5a\xb6\x97\xe5\x65\x10\xb9\xcc\x0e\xe7\xe7\xa0\xb7\x1b\x44\x47\x97\x41\x48\x90\x30\x2c\x77\x97\xe9\x20\xa1\x3c\xec\x1e\xa3\x41\xf7\x34\x1d\x0d\x6a\xa7\xdd\x6f\x39\x88\x5c\x66\x87\xf3\x73\xd0\xdb\x0d\xa2\xa3\xcb\x20\x79\x79\x4e\x07\xbd\xcd\x20\x3a\xba\x0c\x62\xa7\xdd\x65\x10\xb9\x5e\x06\xd1\xdf\xa0\x79\xfa\xec\x0e\xb3\x41\xe6\x70\x9a\x5d\x97\x97\x41\x61\x3a\x68\x18\x6e\x93\xcb\x6e\x17");

/**
PDU LENGTH IS 19 BYTES
NO SMSC ADDRESS PRESENT
MESSAGE HEADER FLAGS
	MESSAGE TYPE :	SMS DELIVER
	MSGS WAITING IN SC :	NO
	SEND STATUS REPORT :	NO
	USER DATA HEADER :	NO UDH
	REPLY PATH :	NO

ORIGINATING ADDRESS
	NUMBER IS :	+441632960000
	TYPE OF NR. :	International
	NPI :	ISDN/Telephone (E.164/163)

PROTOCOL IDENTIFIER	(0x00)
	MESSAGE ENTITIES :	SME-to-SME
	PROTOCOL USED :	Implicit / SC-specific

DATA CODING SCHEME	(0x00)
	AUTO-DELETION :	OFF
	COMPRESSION :	OFF
	MESSAGE CLASS :	NONE
	ALPHABET USED :	7bit default

SMSC TIMESTAMP :	13/09/07 17:16 GMT+1.00

USER DATA PART OF SM
	USER DATA LENGTH : 	0 septets
	USER DATA (TEXT) :	<no data>
*/
_LIT8(KValidEmptyPdu, "\x04\x0c\x91\x44\x97\x71\x57\x30\x12\x00\x00\x70\x90\x31\x71\x61\x05\x40\x00");


_LIT8(KCorruptSmsPdu, "\x01\x02\x03\x04\x05\x06");

CCTSYIntegrationTestSMSStoreBase::CCTSYIntegrationTestSMSStoreBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iSmsStoreTestHelper(*this), iSimTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestSMSStoreBase::~CCTSYIntegrationTestSMSStoreBase()
/*
 * Destructor
 */
	{
	}
	

void CCTSYIntegrationTestSMSStoreBase::CreateGsmSmsEntry(
		RMobileSmsStore::TMobileGsmSmsEntryV1& aEntry, 
		TInt aIndex,
		const TDesC8& aMsgData,
		RMobileSmsStore::TMobileSmsStoreStatus aStoreStatus,
		RMobilePhone::TMobileTON aServiceCentreTON,
		RMobilePhone::TMobileNPI aServiceCentreNumberingPlan,
		const TDesC& aServiceCentreNum)
/**
 * Populates aEntry with parameters to create a valid SMS entry which
 * can be written to the store. Note that aEntry.iMsgData is not populated.
 * 
 * @param aEntry Entry to be populated
 * @param aIndex Slot number of the entry
 * @param aServiceCentreTON Service centre type of number
 * @param aServiceCentreNumberingPlan Service centre numbering plan
 * @param aServiceCentreNum Service centre number
 */
	{
	aEntry.iMsgStatus = aStoreStatus;
	aEntry.iIndex = aIndex;
	aEntry.iMsgData = aMsgData;
	aEntry.iServiceCentre.iTypeOfNumber =  aServiceCentreTON; 
	aEntry.iServiceCentre.iNumberPlan = aServiceCentreNumberingPlan; 
	aEntry.iServiceCentre.iTelNumber = aServiceCentreNum;
	}



void CCTSYIntegrationTestSMSStoreBase::WaitForPhoneBookCachingToFinish()
	{
	User::After(KOneSecond * 20);
	}

CCTSYIntegrationTestSMSStore0001::CCTSYIntegrationTestSMSStore0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0001::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0001::~CCTSYIntegrationTestSMSStore0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0001
 * @SYMFssID BA/CTSY/SMSS-0001
 * @SYMTestCaseDesc Get phone store information.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Phone store information returned correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone store information returned is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);	
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//	
	
	// Get SMS store info. using RMobilePhoneStore::GetInfo with RMobilePhoneStore::TMobilePhoneStoreInfoV1 
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);

	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::GetInfo timed out"));
	ASSERT_EQUALS(getInfoStatus.Int(), KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));				

	// Check: 
	// iName == KETelIccSmsStore 
	ASSERT_TRUE(storeInfo.iName == KETelIccSmsStore, _L("RMobilePhoneStore::GetInfo returned wrong store name"));
	
	// iType == RMobilePhoneStore::EShortMessageStore 
	ASSERT_EQUALS(storeInfo.iType, RMobilePhoneStore::EShortMessageStore, _L("RMobilePhoneStore::GetInfo returned wrong Type"));
	
	// iTotalEntries >= -1 
	ASSERT_TRUE(storeInfo.iTotalEntries >= -1, _L("RMobilePhoneStore::GetInfo returned wrong Total Entries"));
	
	// iUsedEntries >= -1 
	ASSERT_TRUE(storeInfo.iUsedEntries >= -1, _L("RMobilePhoneStore::GetInfo returned wrong Used Entries"));
	
	// iCaps == RMobilePhoneStore::KCapsIndividualEntry  
	//     | RMobilePhoneStore::KCapsDeleteAll 
	//     | RMobilePhoneStore::KCapsReadAccess 
	//     | RMobilePhoneStore::KCapsWriteAccess 
	//     | RMobilePhoneStore::KCapsNotifyEvent 
	//     | RMobilePhoneStore::KCapsWholeStore 				   
	TUint wantedBits = RMobilePhoneStore::KCapsIndividualEntry  
	    			 | RMobilePhoneStore::KCapsDeleteAll 
	    			 | RMobilePhoneStore::KCapsReadAccess 
	    			 | RMobilePhoneStore::KCapsWriteAccess 
	    			 | RMobilePhoneStore::KCapsNotifyEvent 
	    			 | RMobilePhoneStore::KCapsWholeStore; 
	    			 
	ASSERT_BITS_SET(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned unexpected iCaps"));			

	//
    // TEST END
    //

    StartCleanup();
	
	// Pop
	// getInfoStatus
	CleanupStack::PopAndDestroy(&getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0001");
	}



CCTSYIntegrationTestSMSStore0002::CCTSYIntegrationTestSMSStore0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0002::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0002::~CCTSYIntegrationTestSMSStore0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0002
 * @SYMFssID BA/CTSY/SMSS-0002
 * @SYMTestCaseDesc Read SMS message from the SMS store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read
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
	
	
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	WaitForPhoneBookCachingToFinish();
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(storeInfo.iCaps, RMobilePhoneStore::KCapsReadAccess,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect store caps"));
				   
	// Write entries to the first slot, slot 4 and the maximum slot in the phone book. PDU in the entry should be coded with valid  
	// * TP-Message parameters 
	// * Type of number 
	// * Type of number plan 
	// * TP Originator address 
	// * TP Protocol Identifier 
	// * TP Data Coding Scheme 
	// * TP Service Center Time Stamp 
	// * TP User data length 

	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry); 

	// Create and write entry for 1st slot
	CreateGsmSmsEntry(entry, 1, KValidSmsPdu1(), RMobileSmsStore::EStoredMessageRead);

	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	smsStore.Write(writeStatus,pckgEntry);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium)==KErrNone, _L("RMobileSmsStore::Write timed out"));
	CHECK_TRUE_L(writeStatus.Int() == KErrNone, _L("RMobileSmsStore::Write returned an error"));
	
	// Create and write entry for ninth slot
	CreateGsmSmsEntry(	entry, 9, KValidSmsPdu2(), RMobileSmsStore::EStoredMessageUnread, 
						RMobilePhone::EInternationalNumber, 
						RMobilePhone::EIsdnNumberPlan);
	smsStore.Write(writeStatus,pckgEntry);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Write timed out"));
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));	

	// Create and write entry for last slot
	CreateGsmSmsEntry(	entry, storeInfo.iTotalEntries, KValidSmsPdu1(), 
						RMobileSmsStore::EStoredMessageUnread, 
						RMobilePhone::EAbbreviatedNumber, 
						RMobilePhone::EIsdnNumberPlan);
	smsStore.Write(writeStatus,pckgEntry);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Write timed out"));	
	CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Read entry from the first slot, slot 9 and the maximum slot from the phone book in turn passing an RMobileSmsStore::TMobileGsmSmsEntryV1 as aEntry. 

	// Set up and read from slot1
	RMobileSmsStore::TMobileGsmSmsEntryV1 readFirstEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg readFirstEntryPckg(readFirstEntry);
	readFirstEntry.iIndex = 1;
	readFirstEntry.iMsgData.Zero();
	readFirstEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;
	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);	
	smsStore.Read(readStatus,readFirstEntryPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),KErrNone,_L("RMobileSmsStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));
	
	// Set up and read from slot 9	
	RMobileSmsStore::TMobileGsmSmsEntryV1 readNinthEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg readNinthEntryPckg(readNinthEntry);
	readNinthEntry.iIndex = 9;
	readNinthEntry.iMsgData.Zero();
	readNinthEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;	
	smsStore.Read(readStatus,readNinthEntryPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),KErrNone, _L("RMobileSmsStore::Read Could not read SMS store"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));

	// Set up and read from last slot
	RMobileSmsStore::TMobileGsmSmsEntryV1 readLastEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg readLastEntryPckg(readLastEntry);
	readLastEntry.iIndex = storeInfo.iTotalEntries;
	readLastEntry.iMsgData.Zero();
	readLastEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;	
	smsStore.Read(readStatus,readLastEntryPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),KErrNone, _L("RMobileSmsStore::Read Could not read SMS store"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));
	
	// For each one: 

	// Check iMsgStatus is the same as that written to the phone book in set up.
	ASSERT_EQUALS(readFirstEntry.iMsgStatus, RMobileSmsStore::EStoredMessageRead, _L("RMobileSmsStore::Read First Read Entry has the wrong iMsgStatus"));
	ASSERT_EQUALS(readNinthEntry.iMsgStatus, RMobileSmsStore::EStoredMessageUnread, _L("RMobileSmsStore::Read Fourth Read Entry has the wrong iMsgStatus"));
	ASSERT_EQUALS(readLastEntry.iMsgStatus, RMobileSmsStore::EStoredMessageUnread, 	_L("RMobileSmsStore::Read Last Read Entry has the wrong iMsgStatus"));	        																

	// Check iMsgData is the same as that written to the phone book in set up.
	ASSERT_EQUALS_DES8(readFirstEntry.iMsgData, KValidSmsPdu1,_L("RMobileSmsStore::Read First Read Entry has the wrong iMsgData"));
	ASSERT_EQUALS_DES8(readNinthEntry.iMsgData, KValidSmsPdu2,_L("RMobileSmsStore::Read Fourth Read Entry has the wrong iMsgData"));
	ASSERT_EQUALS_DES8(readLastEntry.iMsgData, KValidSmsPdu1,_L("RMobileSmsStore::Read Last Read Entry has the wrong iMsgData"));	
							                                                                                                                            		
	// Check iServiceCentre is the same as that written to the phone book in set up.
	ASSERT_EQUALS(readFirstEntry.iServiceCentre.iTypeOfNumber, RMobilePhone::ENationalNumber,_L("RMobileSmsStore::Read First Read Entry has the wrong iServiceCentre.iTypeOfNumber"));
	ASSERT_EQUALS(readFirstEntry.iServiceCentre.iNumberPlan, RMobilePhone::EIsdnNumberPlan,	_L("RMobileSmsStore::Read First Read Entry has the wrong iServiceCentre.iNumberPlan"));
	ASSERT_EQUALS_DES16(readFirstEntry.iServiceCentre.iTelNumber, KNumber,_L("RMobileSmsStore::Read First Read Entry has the wrong iServiceCentre.iTelNumber"));
		
	ASSERT_EQUALS(readNinthEntry.iServiceCentre.iTypeOfNumber, RMobilePhone::EInternationalNumber, _L("RMobileSmsStore::Read Ninth Read Entry has the wrong iServiceCentre.iTypeOfNumber"));
	ASSERT_EQUALS(readNinthEntry.iServiceCentre.iNumberPlan, RMobilePhone::EIsdnNumberPlan,_L("RMobileSmsStore::Read Ninth Read Entry has the wrong iServiceCentre.iNumberPlan"));
	ASSERT_EQUALS_DES16(readNinthEntry.iServiceCentre.iTelNumber, KNumberInternational, _L("RMobileSmsStore::Read Ninth Read Entry has the wrong iServiceCentre.iTelNumber"));
		
	ASSERT_EQUALS(readLastEntry.iServiceCentre.iTypeOfNumber, RMobilePhone::EAbbreviatedNumber, _L("RMobileSmsStore::Read Last Read Entry has the wrong iServiceCentre.iTypeOfNumber"))	;
	ASSERT_EQUALS(readLastEntry.iServiceCentre.iNumberPlan, RMobilePhone::EIsdnNumberPlan,_L("RMobileSmsStore::Read Last Read Entry has the wrong iServiceCentre.iNumberPlan"))	;
	ASSERT_EQUALS_DES16(readLastEntry.iServiceCentre.iTelNumber, KNumber,_L("RMobileSmsStore::Read Last Read Entry has the wrong iServiceCentre.iTelNumber"));

	//
	// TEST END
	//

    StartCleanup();	
	
	// Pop:
	// getInfoStatus
	// writeStatus
	// readStatus	
	CleanupStack::PopAndDestroy(3,&getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0002");
	}



CCTSYIntegrationTestSMSStore0003::CCTSYIntegrationTestSMSStore0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0003::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0003::~CCTSYIntegrationTestSMSStore0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0003
 * @SYMFssID BA/CTSY/SMSS-0003
 * @SYMTestCaseDesc Read entry that is not present in the SMS store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read, RMobilePhoneStore::Read
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

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	WaitForPhoneBookCachingToFinish();
	
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out")); 
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(storeInfo.iCaps, RMobilePhoneStore::KCapsReadAccess,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect store caps"));
				   
	// Delete an entry from a known slot. 
	// check if it is empty
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgReadEntry(readEntry);
	TInt knownSlotIndex = 1;
	readEntry.iIndex = knownSlotIndex;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;
	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);
		
	smsStore.Read(readStatus,pckgReadEntry);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(readStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::Read timed out")) ;
	CHECK_EQUALS_L(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));

	// If the entry exists, delete it				
	if(readStatus.Int() == KErrNone)
		{		
		TExtEtelRequestStatus deleteStatus (smsStore,EMobilePhoneStoreDelete);
		CleanupStack::PushL(deleteStatus);		
		smsStore.Delete(deleteStatus,knownSlotIndex);
		CHECK_TRUE_L(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium) == KErrNone, _L("RMobileSmsStore::Delete timed out"));
		CHECK_TRUE_L(deleteStatus.Int() == KErrNone,_L("RMobileSmsStore::Delete returned an error"));
		CleanupStack::PopAndDestroy(&deleteStatus);
		}

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Read an entry from a known empty slot. 
	readEntry.iIndex = knownSlotIndex;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;	
	smsStore.Read(readStatus,pckgReadEntry);
	
	// Check KErrNotFound is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNotFound, _L("RMobileSmsStore::Read did not return KErrNotFound"));	

	//
    // TEST END
    //

    StartCleanup();
	// Pop:
	// getInfoStatus
	// readStatus
	CleanupStack::PopAndDestroy(2,&getInfoStatus);		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0003");
	}



CCTSYIntegrationTestSMSStore0004::CCTSYIntegrationTestSMSStore0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0004::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0004::~CCTSYIntegrationTestSMSStore0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0004
 * @SYMFssID BA/CTSY/SMSS-0004
 * @SYMTestCaseDesc Read SMS message from the SMS store using invalid slot numbers.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - Error returned on read.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument or KErrGsmSMSInvalidMemoryIndex or KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	WaitForPhoneBookCachingToFinish();
	
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(storeInfo.iCaps, RMobilePhoneStore::KCapsReadAccess,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect store caps"));
				   
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Read an entry with an invalid slot number ===

	// Read a single entry from the phone book using aIndex = -1 
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgReadEntry(readEntry);
	readEntry.iIndex = -1;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;
	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);	
	smsStore.Read(readStatus,pckgReadEntry);
	// Check KErrArgument or KErrGsmSMSInvalidMemoryIndex or KErrNotFound is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Read timed out"));
	ASSERT_TRUE((readStatus.Int() == KErrArgument) || 
				(readStatus.Int() == KErrGsmSMSInvalidMemoryIndex) || 
				(readStatus.Int() == KErrNotFound),
				_L("RMobileSmsStore::Read Did not get the expected KErrArgument || KErrGsmSMSInvalidMemoryIndex || KErrNotFound when reading from -1 index"));
	

	// Read a single entry from the phone book using aIndex = -10 
	readEntry.iIndex = -10;
	smsStore.Read(readStatus,pckgReadEntry);	
	// Check KErrArgument or KErrGsmSMSInvalidMemoryIndex or KErrNotFound is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Read timed out"));
	ASSERT_TRUE((readStatus.Int() == KErrArgument) || 
				(readStatus.Int() == KErrGsmSMSInvalidMemoryIndex) || 
				(readStatus.Int() == KErrNotFound),
				_L("RMobileSmsStore::Read Did not get the expected KErrArgument || KErrGsmSMSInvalidMemoryIndex || KErrNotFound when reading from -10 index"));
	

	// Read a single entry from the phone book using aIndex = -50 
	readEntry.iIndex = -50;
	smsStore.Read(readStatus,pckgReadEntry);	
	// Check KErrArgument or KErrGsmSMSInvalidMemoryIndex or KErrNotFound is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Read timed out"));
	ASSERT_TRUE((readStatus.Int() == KErrArgument) || 
				(readStatus.Int() == KErrGsmSMSInvalidMemoryIndex) || 
				(readStatus.Int() == KErrNotFound),
				_L("RMobileSmsStore::Read Did not get the expected KErrArgument || KErrGsmSMSInvalidMemoryIndex || KErrNotFound when reading from -50 index"));

	//
    // TEST END
    //

    StartCleanup();
	
	// Pop:
	// getInfoStatus
	// readStatus
	CleanupStack::PopAndDestroy(2,&getInfoStatus);		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0004");
	}



CCTSYIntegrationTestSMSStore0005::CCTSYIntegrationTestSMSStore0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0005::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0005::~CCTSYIntegrationTestSMSStore0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0005
 * @SYMFssID BA/CTSY/SMSS-0005
 * @SYMTestCaseDesc Write SMS message to the SMS store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Write, RMobilePhoneStore::Read, RMobilePhoneStore::NotifyStoreEvent
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

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess | KCapsWriteAccess | KCapsNotifyEvent 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	WaitForPhoneBookCachingToFinish();
	
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
					
	TUint wantedBits = RMobilePhoneStore::KCapsReadAccess | 
					   RMobilePhoneStore::KCapsWriteAccess | 
					   RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(storeInfo.iCaps, wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	// Ensure phone store is not full. 	
	TInt clearSpace = iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone);
	CHECK_TRUE_L(clearSpace>0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone store ===
	
	// Get phone store information using RMobilePhone::GetPhoneStoreInfo and KETelIccSmsStore 
	// Store the iUsedEntries returned.
	// Get phone book store information using RMobilePhoneStore::GetInfo 
	// Store the iUsedEntries returned.
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	TInt usedEntries = -1;
	TInt usedEntries2 = -1;
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
			phoneStoreInfoPckg,
			phone, 
			usedEntries, 
			usedEntries2), 
			KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// ===  Write a valid entry (RMobileSmsStore::TMobileGsmSmsEntryV1) ===

	// post notifier for RMobilePhoneStore::NotifyStoreEvent
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);
	
	// Write a valid entry with a entry into the phone store specifying a specific index to store the entry which does not contain an entry. 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry); 
	CreateGsmSmsEntry(entry, clearSpace, KValidSmsPdu1(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
			RMobilePhone::EIsdnNumberPlan);
	
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);	
	smsStore.Write(writeStatus,pckgEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Write Timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));

	TEST_CHECK_POINT_L(_L("RMobileSmsStore::Write returned an error, not proceeding with test"));
		
	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryAdded
	RMobilePhoneStore::TMobileStoreEvent expectedEvent = RMobilePhoneStore::KStoreEntryAdded ;
	iSmsStoreTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
													notifyStoreEventStatus,
													storeEvent,
													expectedEvent,
													index);

	// ===  Read the written entry ===

	// Read the entry from the phone store. 
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgReadEntry(readEntry);
	readEntry.iIndex = index;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;
	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);
		
	smsStore.Read(readStatus,pckgReadEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Read, Timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));			
	
	// Check that the entry information is the same as that written.
	ASSERT_EQUALS(readEntry.iMsgStatus, RMobileSmsStore::EStoredMessageUnread, 	_L("RMobileSmsStore::Read Read Entry has the wrong iMsgStatus"));
	ASSERT_EQUALS_DES8(readEntry.iMsgData, KValidSmsPdu1, _L("RMobileSmsStore::Read Read Entry has the wrong iMsgData"));
									
	// Check iServiceCentre is the same as that written to the phone book in set up.
	ASSERT_EQUALS(readEntry.iServiceCentre.iTypeOfNumber,  RMobilePhone::ENationalNumber, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iTypeOfNumber"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iNumberPlan, RMobilePhone::EIsdnNumberPlan, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iNumberPlan"));
	ASSERT_EQUALS_DES16(readEntry.iServiceCentre.iTelNumber, KNumber, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iTelNumber"));

	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored if this API is supported
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, phoneStoreInfoPckg,  
				phone, usedEntries +1, usedEntries2 +1), 
				KErrNone, 
				_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Write an entry to the same slot number as before ===

	// post notifier for RMobilePhoneStore::NotifyStoreEvent
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);
	
	// Write a valid entry with a entry into the phone store specifying a specific index to 
	// store the entry which is the same as the index used in the first write. 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry2;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry2(entry2);
	CreateGsmSmsEntry(entry2, clearSpace, KValidSmsPdu1(), 
			RMobileSmsStore::EStoredMessageRead, RMobilePhone::EInternationalNumber,
			RMobilePhone::EIsdnNumberPlan);
	smsStore.Write(writeStatus,pckgEntry2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));
	
	TEST_CHECK_POINT_L(_L("RMobileSmsStore::Write returned an error"));
	
	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryChanged
	expectedEvent = RMobilePhoneStore::KStoreEntryChanged;
	iSmsStoreTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
													notifyStoreEventStatus,
													storeEvent,
													expectedEvent,
													entry2.iIndex);
	
	// DEFECT HERE!
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10003);

	// ===  Read the written entry ===

	// Read the entry from the phone store. 
	readEntry.iIndex = entry2.iIndex; 
	smsStore.Read(readStatus,pckgReadEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Read, Timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));
	
	// Check that the entry information is the same as that written.
	ASSERT_EQUALS(readEntry.iMsgStatus, RMobileSmsStore::EStoredMessageRead, _L("RMobileSmsStore::Read Read Entry has the wrong iMsgStatus"));
	ASSERT_EQUALS_DES8(readEntry.iMsgData, KValidSmsPdu1, _L("RMobileSmsStore::Read Read Entry has the wrong iMsgData"));	
	ASSERT_EQUALS(readEntry.iServiceCentre.iTypeOfNumber,  RMobilePhone::EInternationalNumber, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iTypeOfNumber"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iNumberPlan, RMobilePhone::EIsdnNumberPlan,	_L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iNumberPlan"));
	ASSERT_EQUALS_DES16(readEntry.iServiceCentre.iTelNumber, KNumberInternational, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iTelNumber"));

	// ===  Get the number of used entries and check it is the same as the previous call to get number of used entries ===
	
	// Check RMobilePhone::GetPhoneStoreInfo returns correct iUsedEntries
	// Check RMobilePhoneStore::GetInfo returns correct iUsedEntries 
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore,  phoneStoreInfoPckg, phone, usedEntries +1, usedEntries2 +1), 
		KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	//
    // TEST END
    //

    StartCleanup();

	// Pop
	// getInfoStatus
	// notifyStoreEventStatus
	// writeStatus
	// readStatus
	CleanupStack::PopAndDestroy(4,&getInfoStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0005");
	}



CCTSYIntegrationTestSMSStore0006::CCTSYIntegrationTestSMSStore0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0006::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0006::~CCTSYIntegrationTestSMSStore0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0006
 * @SYMFssID BA/CTSY/SMSS-0006
 * @SYMTestCaseDesc Write an entry to SMS store at index of -1 (first free slot).
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Write, RMobilePhoneStore::GetInfo, RMobilePhoneStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
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

	WaitForPhoneBookCachingToFinish();
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure SIM phone book is not full. 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	TInt nextFreeSpace = iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone);
	CHECK_TRUE_L(nextFreeSpace>=0,_L("TSimTsyTestHelper::GetSmsMessagingL returned an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | KCapsNotifyEvent 
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);

	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));				
	TUint wantedBits = RMobilePhoneStore::KCapsReadAccess | 
					   RMobilePhoneStore::KCapsWriteAccess | 
					   RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(storeInfo.iCaps, wantedBits, KNoUnwantedBits, 
					_L("RMobileSmsStore::GetInfo, wrong bits set"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1 	
	TInt storedUsedEntries = -1;
	TInt storedPhoneUsedEntries = -1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
			phoneStoreInfoPckg,
			phone,
			storedUsedEntries,
			storedPhoneUsedEntries),
			KErrNone,  
			_L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// post notifier for RMobilePhoneStore::NotifyStoreEvent
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);
	
		
	// ===  Write an entry to the first free slot (RMobileSmsStore::TMobileGsmSmsEntryV1) ===

	// Write a valid entry to the phone store using RMobilePhoneStore::Write supplying aEntry.index = -1 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry); 
	CreateGsmSmsEntry(entry, -1, KValidSmsPdu1(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
			RMobilePhone::EIsdnNumberPlan);
	
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	smsStore.Write(writeStatus,pckgEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));

										
	// Store the aEntry.iIndex returned for future use 
	TInt storedIndex = entry.iIndex;
	
	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryAdded
	RMobilePhoneStore::TMobileStoreEvent expectedEvent = RMobilePhoneStore::KStoreEntryAdded ;
	iSmsStoreTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
													notifyStoreEventStatus,
													storeEvent,
													expectedEvent,
													index);
													
	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored if this API is supported
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, 
			 phoneStoreInfoPckg, phone, storedUsedEntries +1, storedPhoneUsedEntries +1),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	// ===  Read the written entry ===

	// Read the written entry at index aEntry.iIndex returned from the write 
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgReadEntry(readEntry);
	readEntry.iIndex = storedIndex;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;
	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);	
	smsStore.Read(readStatus,pckgReadEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone,	_L("RMobileSmsStore::Read, timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));

	// Check RMobilePhoneStore::Read returns correct entry information 
	ASSERT_EQUALS(readEntry.iMsgStatus, RMobileSmsStore::EStoredMessageUnread, _L("RMobileSmsStore::Read Read Entry has the wrong iMsgStatus"));
	ASSERT_EQUALS_DES8(readEntry.iMsgData, KValidSmsPdu1, _L("RMobileSmsStore::Read Read Entry has the wrong iMsgData"));			
	ASSERT_EQUALS(readEntry.iServiceCentre.iTypeOfNumber,  RMobilePhone::ENationalNumber, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iTypeOfNumber"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iNumberPlan, RMobilePhone::EIsdnNumberPlan, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iNumberPlan"));
	ASSERT_EQUALS_DES16(readEntry.iServiceCentre.iTelNumber, KNumber, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iTelNumber"));
	
	//
    // TEST END
    //

    StartCleanup();
	// Pop:
	// getInfoStatus
	// writeStatus
	// notifyStoreEventStatus
	// readStatus
	CleanupStack::PopAndDestroy(4, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0006");
	}



CCTSYIntegrationTestSMSStore0007::CCTSYIntegrationTestSMSStore0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0007::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0007::~CCTSYIntegrationTestSMSStore0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0007
 * @SYMFssID BA/CTSY/SMSS-0007
 * @SYMTestCaseDesc Write SMS message to the SMS store using invalid slot numbers.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Write, RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - Error returned on write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write to an invalid index and number of used entries in the store is unchanged.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone book is not full. 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	TInt nextFreeSpace = iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone);
	CHECK_TRUE_L(nextFreeSpace>=0,_L("TSimTsyTestHelper::GetSmsMessagingL returned an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess 
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
	TUint wantedBits = RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess;
	CHECK_BITS_SET_L(storeInfo.iCaps, wantedBits,KNoUnwantedBits, _L("RMobileSmsStore::GetInfo, wrong bits set"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// ===  1. Find out how many entries are used in the phone book ===
	// ===  2. Attempt to write an entry to invalid index ===
	// ===  3. Get the number of used entries and check it is same as before the attempted write ===
	// Repeat steps 1 to 3 for aEntry.index = 0, -2, -10 	

	TInt indexArray[3] = {0,-2,-10};	
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);	
	TInt storedUsedEntries, storedPhoneUsedEntries;
	
	// create the entry, the only thing that will change in each iteration is the iIndex
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg entryPckg(entry);
 	CreateGsmSmsEntry(entry, 1, KValidSmsPdu1(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
			RMobilePhone::EIsdnNumberPlan);
 	
	for (TInt i = 0; i< 3; i++)
		{
		// ===  1. Find out how many entries are used in the phone book ===
		
		// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1 
		// Store iUsedEntries returned for future use
		// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
		// Store iUsedEntries returned for future use
		RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
		RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
		ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
				phoneStoreInfoPckg,
				phone,
				storedUsedEntries,
				storedPhoneUsedEntries),
				KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
				
		// ===  2. Attempt to write an entry to invalid index ===
		
		// Write an entry using RMobilePhoneStore::Write and aEntry.index = 0, -2, -10 
		entry.iIndex = indexArray[i];			
		smsStore.Write(writeStatus,entryPckg);
										
		// Check KErrArgument or KErrGsmSMSInvalidMemoryIndex is returned
		DEBUG_PRINTF2(_L("just wrote to index = %d"), entry.iIndex);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),KErrNone, _L("RMobileSmsStore::Write, timed out"));
		ASSERT_TRUE((writeStatus.Int() == KErrArgument) || (writeStatus.Int() == KErrGsmSMSInvalidMemoryIndex), _L("RMobileSmsStore::Write did not return KErrArgument | KErrGsmSMSInvalidMemoryIndex"));
		DEBUG_PRINTF2(_L("write status = %d"), writeStatus.Int());
										
		// ===  3. Get the number of used entries and check it is same as before the attempted write ===
		
		// Check RMobilePhoneStore::GetInfo returns same iUsedEntries as before	
		// Check RMobilePhone::GetPhoneStoreInfo returns same iUsedEntries as before
		ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, phoneStoreInfoPckg, phone, 
				storedUsedEntries, storedPhoneUsedEntries),
				KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
		} // end for (TInt i = 0; i< 3; i++)
		
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10004);
	
	// ===  Attempt to write an entry to an index that is higher than the max slot number ===

	// Write an entry using RMobilePhoneStore::Write and aEntry.index = 10000 
	entry.iIndex = 10000;
	smsStore.Write(writeStatus,entryPckg);
	// Check error returned is not KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Write, timed out"));
	ASSERT_TRUE(writeStatus.Int() != KErrNone, _L("RMobileSmsStore::Write should have returned an error but didnt"));

	
	//
    // TEST END
    //

    StartCleanup();
	// Pop
	// getInfoStatus
	// writeStatus
	CleanupStack::PopAndDestroy(2, &getInfoStatus);	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0007");
	}



CCTSYIntegrationTestSMSStore0008::CCTSYIntegrationTestSMSStore0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0008::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0008::~CCTSYIntegrationTestSMSStore0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0008
 * @SYMFssID BA/CTSY/SMSS-0008
 * @SYMTestCaseDesc Write SMS message to the SMS store when SIM is full.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Write, RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - Error on attempt to write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned and number of used entries is unchanged.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Fill SIM phone book. 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);
    CHECK_TRUE_L(iSmsStoreTestHelper.EnsureStoreFullL(smsStore),_L("TSmsStoreTsyTestHelper::EnsureStoreFullL returned an error"));
    

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1 
	// Store iUsedEntries returned for future use 
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Store iUsedEntries returned for future use 
	TInt storeUsedEntries = -1;
	TInt phoneUsedEntries = -1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
			phoneStoreInfoPckg,
			phone,
			storeUsedEntries,
			phoneUsedEntries),
			KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	

	// ===  Attempt to write an entry to the phone book ===

	// Write an entry using RMobilePhoneStore::Write and aEntry.index = -1 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
	CreateGsmSmsEntry(entry, -1, KValidSmsPdu1,
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::EUnknownNumber,
			RMobilePhone::EIsdnNumberPlan);
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);	
	smsStore.Write(writeStatus,pckgEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),KErrNone, _L("RMobileSmsStore::Write timed out"));

	// Check KErrDiskFull is returned.
	ASSERT_EQUALS(writeStatus.Int(), KErrDiskFull, _L("RMobileSmsStore::Write did not return the expected status != KErrNone"));

	// ===  Get the number of used entries and check it is the same as before attempted write ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore,  phoneStoreInfoPckg, phone, 
		storeUsedEntries, phoneUsedEntries),
		KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// writeStatus
	CleanupStack::PopAndDestroy(&writeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0008");
	}



CCTSYIntegrationTestSMSStore0009::CCTSYIntegrationTestSMSStore0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0009::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0009::~CCTSYIntegrationTestSMSStore0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0009
 * @SYMFssID BA/CTSY/SMSS-0009
 * @SYMTestCaseDesc Write SMS message to the SMS store making the store full.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Write, RMobilePhoneStore::Read, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Entry written correctly.  Store event is retrieved indicating entry has been written and store is full.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry written and KStoreFull event occurs.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | KCapsNotifyEvent 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
					
	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess | 
						RMobilePhoneStore::KCapsReadAccess | 
						RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(storeInfo.iCaps, wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	
	// Fill phone book until there is only one slot free. 
	// Create an entry
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
	CreateGsmSmsEntry(entry, -1, KValidSmsPdu2(),
			RMobileSmsStore::EStoredMessageUnread, 
			RMobilePhone::ENetworkSpecificNumber,
			RMobilePhone::ETelexNumberPlan);

	TExtEtelRequestStatus deleteStatus (smsStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStatus);
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	
	if (storeInfo.iUsedEntries == storeInfo.iTotalEntries) // if already full, delete one entry
		{
		smsStore.Delete(deleteStatus,storeInfo.iUsedEntries);			
		CHECK_TRUE_L(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium) == KErrNone, _L("RMobileSmsStore::Delete timed out"));
		CHECK_TRUE_L(deleteStatus.Int() == KErrNone, _L("RMobileSmsStore::Delete returned an error"));
		}
	else
		{
		while (storeInfo.iUsedEntries != (storeInfo.iTotalEntries))
			{
			entry.iIndex = -1; // need to reset this every time

			smsStore.Write(writeStatus,pckgEntry);
			CHECK_TRUE_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium)==KErrNone, _L("RMobileSmsStore::Write timed out"));
			CHECK_TRUE_L(writeStatus.Int() == KErrNone, _L("RMobileSmsStore::Write returned an error"));
		
			smsStore.GetInfo(getInfoStatus,storeInfoPckg);
			CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
			CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
			}
		}


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1  
	// Store iUsedEntries returned for future use 
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Store iUsedEntries returned for future use 
	TInt storeUsedEntries = -1;
	TInt phoneUsedEntries = -1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
			phoneStoreInfoPckg,
			phone,
			storeUsedEntries,
			phoneUsedEntries),
			KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	
	// ===  Write an entry into the last free slot in the phone book ===

	// Post a notifier
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);	
	
	// Write an entry using RMobilePhoneStore::Write and aEntry.index = -1 
	entry.iIndex = -1; 
	smsStore.Write(writeStatus,pckgEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),KErrNone, _L("RMobileSmsStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreFull | KStoreEntryAdded
	TUint32 expectedEvent = RMobilePhoneStore::KStoreEntryAdded | RMobilePhoneStore::KStoreFull;
	iSmsStoreTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
													notifyStoreEventStatus,
													storeEvent,
													expectedEvent,
													index);

	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored.
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, phoneStoreInfoPckg, phone,
		storeUsedEntries+1, phoneUsedEntries+1),
		KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	// ===  Read the written entry ===

	// Read the written entry with RMobilePhoneStore::Read 
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgReadEntry(readEntry);
	readEntry.iIndex = index;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;

	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);
		
	smsStore.Read(readStatus,pckgReadEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone, _L("RMobilePhoneStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobilePhoneStore::Read returned an error"));
	
	// Check entry details are correct
	ASSERT_EQUALS(readEntry.iMsgStatus, RMobileSmsStore::EStoredMessageUnread,_L("RMobilePhoneStore::Read Read Entry has the wrong iMsgStatus"));
	ASSERT_EQUALS_DES8(readEntry.iMsgData, KValidSmsPdu2, _L("RMobilePhoneStore::Read Read Entry has the wrong iMsgData"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iTypeOfNumber,  RMobilePhone::ENetworkSpecificNumber,_L("RMobilePhoneStore::Read Read Entry has the wrong iServiceCentre.iTypeOfNumber"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iNumberPlan, RMobilePhone::ETelexNumberPlan,_L("RMobilePhoneStore::Read Read Entry has the wrong iServiceCentre.iNumberPlan"));
	ASSERT_EQUALS_DES16(readEntry.iServiceCentre.iTelNumber, KNumber, _L("RMobilePhoneStore::Read Read Entry has the wrong iServiceCentre.iTelNumber"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// deleteStatus
	// writeStatus
	// notifyStoreEventStatus
	// readStatus
	CleanupStack::PopAndDestroy(5, &getInfoStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0009");
	}



CCTSYIntegrationTestSMSStore0010::CCTSYIntegrationTestSMSStore0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0010::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0010::~CCTSYIntegrationTestSMSStore0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0010
 * @SYMFssID BA/CTSY/SMSS-0010
 * @SYMTestCaseDesc Delete entry from the SMS store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
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

	
	// Ensure phone has KCapsReadAccess | KCapsWriteAccess | KCapsNotifyEvent capabilities. 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
					
	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess | 
						RMobilePhoneStore::KCapsReadAccess | 
						RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(storeInfo.iCaps, wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));


	// Ensure there is an entry in slot 4 of the phone book. 
    // Create an entry 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
	CreateGsmSmsEntry(entry, 4, KValidSmsPdu1(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENetworkSpecificNumber,
			RMobilePhone::ETelexNumberPlan);    
 	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);   

	smsStore.Write(writeStatus,pckgEntry);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium)==KErrNone, _L("RMobileSmsStore::Write timed out"));
	// This may return a value other than KErrNone, if the space was already full
	CHECK_TRUE_L((writeStatus.Int() == KErrNone ||
				  writeStatus.Int() == KErrGsmSMSSimSMSStorageFull), _L("RMobileSmsStore::Write status was not set to KErrNone or KErrGsmSMSSimSMSStorageFull as expected"));
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// === Delete entry from the phone store specifying aIndex = 4 ===

	// used for RMobilePhoneStore::NotifyStoreEvent
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	
	// used for RMobileSmsStore::Delete
	TExtEtelRequestStatus deleteStatus (smsStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStatus);

	// used to check RMobilePhoneStore::NotifyStoreEvent
	TUint32 expectedEvent;		
	
	// used for RMobilePhoneStore::Read 
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgReadEntry(readEntry);	
	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone store information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1 
	// Store iUsedEntries returned for future use 
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Store iUsedEntries returned for future use 
	TInt storeUsedEntries = -1;
	TInt phoneUsedEntries = -1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
			phoneStoreInfoPckg,
			phone,
			storeUsedEntries,
			phoneUsedEntries),
			KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	
	// ===  Delete an entry ===
	// Post a notifier
	TInt storeEventIndex;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, storeEventIndex);	

	// Delete entry from the phone store specifying aIndex = 4
	smsStore.Delete(deleteStatus,4);			
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileSmsStore::Delete returned an error"));
	
	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEntryDeleted
	expectedEvent = RMobilePhoneStore::KStoreEntryDeleted;
	iSmsStoreTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
													notifyStoreEventStatus,
													storeEvent,
													expectedEvent,
													4);
													
	// ===  Get the number of used entries and check it is 1 lower than before deleting the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 lower  than that stored.
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 lower than that stored.
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore,  phoneStoreInfoPckg, phone,
			storeUsedEntries-1, phoneUsedEntries-1),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	
	// ===  Attempt to read the deleted entry.  ===

	// Read a single entry stored in the slot used in delete 
	readEntry.iIndex = 4;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;		
	smsStore.Read(readStatus,pckgReadEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone, _L("RMobilePhoneStore::Read timed out"));

	// Check KErrNotFound is returned.
	ASSERT_EQUALS(readStatus.Int(), KErrNotFound, _L("RMobilePhoneStore::Read did not return KErrNotFound as expected"));

	// ===  Attempt to delete the already deleted entry again. ===
	
	// Delete the already deleted entry 
	smsStore.Delete(deleteStatus,4);			
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Delete timed out"));

	// $CTSYProblem The LTSY returns KErrNone to CTSY when an entry is deleted from a slot which is already empty
	// CTSY correctly propagates whatever error is returned by LTSY back to the client.
	// Therefore the test has been coded to expect KErrNone.  If the LTSY behaviour is fixed, KErrNotFound or similar
	// should be expected in this case.
	
	// Check KErrNone is returned
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileSmsStore::Delete did not return KErrNone as expected"));	
		

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// writeStatus
    // notifyStatus
	// deleteStatus
	// readStatus
	CleanupStack::PopAndDestroy(5, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0010");
	}



CCTSYIntegrationTestSMSStore0011::CCTSYIntegrationTestSMSStore0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0011::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0011::~CCTSYIntegrationTestSMSStore0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0011
 * @SYMFssID BA/CTSY/SMSS-0011
 * @SYMTestCaseDesc Delete entry from the SMS store which does not exist.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Error returned on delete.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound or KErrNone is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure phone has KCapsReadAccess and KCapsWriteAccess capabilities. 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
					
	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess | 
						RMobilePhoneStore::KCapsReadAccess ;
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	// Ensure slot 1 is empty. 
	TExtEtelRequestStatus deleteStatus (smsStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStatus);
	smsStore.Delete(deleteStatus,1);			
	CHECK_TRUE_L(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium) == KErrNone, _L("RMobileSmsStore::Delete timed out"));
	CHECK_TRUE_L(deleteStatus.Int() == KErrNone || deleteStatus.Int() == KErrGsmSMSInvalidMemoryIndex,_L("RMobileSmsStore::Delete returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===

	// Get phone store information using RMobilePhone::GetPhoneStoreInfo and KETelIccSmsStore
	// Store the iUsedEntries for future use.
	// Get phone book store information using RMobilePhoneStore::GetInfo
	// Store the iUsedEntries for future use.
	TInt storeUsedEntries = -1;
	TInt phoneUsedEntries = -1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
			phoneStoreInfoPckg,
			phone,
			storeUsedEntries,
			phoneUsedEntries),
			KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// Delete an entry using RMobilePhoneStore::Delete where aIndex is the index of slot 1 
	smsStore.Delete(deleteStatus,1);			
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Delete timed out"));
	// Check KErrNotFound or KErrGsmSMSInvalidMemoryIndex or KErrNone is returned.
	ASSERT_TRUE((deleteStatus.Int()== KErrNotFound) || (deleteStatus.Int() == KErrNone) || (deleteStatus.Int() == KErrGsmSMSInvalidMemoryIndex),_L("RMobileSmsStore::Delete did not return KErrNotFound or KErrGsmSMSInvalidMemoryIndex"));

	// $CTSYProblem The LTSY returns KErrNone to CTSY when an entry is deleted from a slot which is already empty
	// CTSY correctly propagates whatever error is returned by LTSY back to the client.
	// Therefore the test has been coded to expect KErrNone.  If the LTSY behaviour is fixed, KErrNotFound or similar
	// should be expected in this case.
	
	// ===  Get the number of used entries and check it is the same as before attempted delete ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, phoneStoreInfoPckg, phone,
			storeUsedEntries, phoneUsedEntries),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus,
	// deleteStatus,
	CleanupStack::PopAndDestroy(2,&getInfoStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0011");
	}



CCTSYIntegrationTestSMSStore0012::CCTSYIntegrationTestSMSStore0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0012::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0012::~CCTSYIntegrationTestSMSStore0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0012
 * @SYMFssID BA/CTSY/SMSS-0012
 * @SYMTestCaseDesc Delete entry from the SMS store which makes the store empty.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
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

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);

	// Ensure phone has KCapsReadAccess | KCapsWriteAccess | KCapsNotifyEvent capabilities. 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone, _L("RMobileSmsStore::GetInfo, timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess | 
						RMobilePhoneStore::KCapsReadAccess | 
						RMobilePhoneStore::KCapsNotifyEvent;
	
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));
	
	// Delete all the entries from the phone book then write one entry to a known location. 
	TExtEtelRequestStatus deleteStatus (smsStore,EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStatus);
	smsStore.DeleteAll(deleteStatus);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::DeleteAll timed out"));
	CHECK_TRUE_L(deleteStatus.Int() == KErrNone, _L("RMobileSmsStore::DeleteAll returned an error"));
	
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone, _L("RMobileSmsStore::GetInfo, timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
	CHECK_TRUE_L(storeInfo.iUsedEntries == 0, _L("RMobilePhoneStore::GetInfo SET UP error, iUsedEntries should be 0 at this point"));

	// Create an entry 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
	CreateGsmSmsEntry(entry, -1, KValidSmsPdu1(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENetworkSpecificNumber,
			RMobilePhone::ETelexNumberPlan);

	// Write entry
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	smsStore.Write(writeStatus,pckgEntry);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium)==KErrNone, _L("RMobileSmsStore::Write timed out"));		
	CHECK_TRUE_L(writeStatus.Int() == KErrNone, _L("RMobileSmsStore::Write returned an error"));
	TInt entryNum = entry.iIndex;	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Post a notifier
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);	

	// Delete the entry from the SMS store 
	smsStore.Delete(deleteStatus, entryNum);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileSmsStore::Delete returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEntryDeleted | KStoreEmpty
	TUint32 expectedEvent = RMobilePhoneStore::KStoreEntryDeleted | RMobilePhoneStore::KStoreEmpty;
	iSmsStoreTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
													notifyStoreEventStatus,
													storeEvent,
													expectedEvent,
													index);

	// ===  Get the number of used entries and check it is the same as before attempted delete ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries of 0
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries of 0
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, phoneStoreInfoPckg, phone, 0, 0),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
    // deleteStatus
	// writeStatus
	// notifyStoreEventStatus
	CleanupStack::PopAndDestroy(4,&getInfoStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0012");
	}



CCTSYIntegrationTestSMSStore0013::CCTSYIntegrationTestSMSStore0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0013::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0013::~CCTSYIntegrationTestSMSStore0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0013
 * @SYMFssID BA/CTSY/SMSS-0013
 * @SYMTestCaseDesc Delete an entry from a full SMS store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Entries deleted, RMobilePhoneStore::KStoreHasSpace event occurs.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry deleted and KStoreHasSpace event occurs.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsNotifyEvent 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
					
	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess | 
						RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(storeInfo.iCaps, wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	// Ensure phone book is full
    CHECK_TRUE_L(iSmsStoreTestHelper.EnsureStoreFullL(smsStore),_L("TSmsStoreTsyTestHelper::EnsureStoreFullL returned an error"));
		
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1 
	// Store iUsedEntries returned for future use 
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo				
	// Store iUsedEntries returned for future use 
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	TInt usedEntries = -1;
	TInt phoneUsedEntries = -1;
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, phoneStoreInfoPckg, phone,
			usedEntries,
			phoneUsedEntries),
			KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// Post a notifier
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);	
	
	// Delete an entry from the phone book. 
	TExtEtelRequestStatus deleteStatus (smsStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStatus);

	smsStore.Delete(deleteStatus, 1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileSmsStore::Delete returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreHasSpace | RMobilePhoneStore::KStoreEntryDeleted
	TUint32 expectedEvent = RMobilePhoneStore::KStoreHasSpace | RMobilePhoneStore::KStoreEntryDeleted;
	iSmsStoreTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
													notifyStoreEventStatus,
													storeEvent,
													expectedEvent,
													index);

	// ===  Get the number of used entries and check it is one lower than before ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries is one lower than before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries is one lower than before
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, phoneStoreInfoPckg, phone,
			usedEntries-1, phoneUsedEntries-1),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
		
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// notifyStoreEventStatus
	// deleteStatus
	CleanupStack::PopAndDestroy(3, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0013");
	}



CCTSYIntegrationTestSMSStore0014::CCTSYIntegrationTestSMSStore0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0014::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0014::~CCTSYIntegrationTestSMSStore0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0014
 * @SYMFssID BA/CTSY/SMSS-0014
 * @SYMTestCaseDesc Delete all entries from the SMS store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::DeleteAll, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
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

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsNotifyEvent 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	// Ensure phone book is not empty 
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);	

	if (storeInfo.iUsedEntries == 0)
	{
		RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
		RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);

	    // Create an entry 
		CreateGsmSmsEntry(entry, -1, KValidSmsPdu1(),
				RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENetworkSpecificNumber,
				RMobilePhone::ETelexNumberPlan);
		
		smsStore.Write(writeStatus,pckgEntry);
		CHECK_TRUE_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium)==KErrNone, _L("RMobileSmsStore::Write timed out"));
		CHECK_TRUE_L(writeStatus.Int() == KErrNone, _L("RMobileSmsStore::Write returned an error"));

		smsStore.GetInfo(getInfoStatus,storeInfoPckg);
		CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
		CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
	}
	
	CHECK_TRUE_L(storeInfo.iUsedEntries != 0, _L("SETUP: There should be at least one entry at this point"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Delete all entries from the phone book ===

	// post notifier for RMobilePhoneStore::NotifyStoreEvent
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);
	
	// Delete all entries from the full phone book 
	TExtEtelRequestStatus deleteAllStatus (smsStore,EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteAllStatus);
	smsStore.DeleteAll(deleteAllStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteAllStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::DeleteAll timed out"));
	ASSERT_EQUALS(deleteAllStatus.Int(), KErrNone, _L("RMobileSmsStore::DeleteAll returned an error"));
 	TEST_CHECK_POINT_L(_L("RMobileSmsStore::DeleteAll returned an error"));
			
	// Check RMobilePhoneStore::NotifyStoreEvent complete with KStoreEmpty
	RMobilePhoneStore::TMobileStoreEvent expectedEvent = RMobilePhoneStore::KStoreEmpty ;
	iSmsStoreTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
													notifyStoreEventStatus,
													storeEvent,
													expectedEvent,
													index);	
	
	// ===  Check phone book is empty ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1  
	// Check iUsedEntries == 0
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Check iUsedEntries == 0
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, storeInfoPckg, phone, 0, 0),
		KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// writeStatus
	// notifyStoreEventStatus
	// deleteStatus
	CleanupStack::PopAndDestroy(4,&getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0014");
	}



CCTSYIntegrationTestSMSStore0015::CCTSYIntegrationTestSMSStore0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0015::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0015::~CCTSYIntegrationTestSMSStore0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0015
 * @SYMFssID BA/CTSY/SMSS-0015
 * @SYMTestCaseDesc Cancel delete of all entries from the SMS store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::DeleteAll, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
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

	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess ;
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	// Ensure phone store is full
    CHECK_TRUE_L(iSmsStoreTestHelper.EnsureStoreFullL(smsStore),_L("TSmsStoreTsyTestHelper::EnsureStoreFullL returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1   
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	ASSERT_EQUALS(getInfoStatus.Int(), KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));	

	// Store iUsedEntries returned for future use 
	TInt storeUsedEntries = storeInfo.iUsedEntries;

	// ===  Delete all entries from the phone book ===

	// Delete all entries from the full phone book 
	TExtEtelRequestStatus deleteAllStatus (smsStore,EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteAllStatus);	
	smsStore.DeleteAll(deleteAllStatus);
	// Do not wait for the request to complete. 

	// Wait for 2 seconds. 
	User::After(KOneSecond*2); 
		
	// Cancel the DeleteAll operation. Check KErrCancel returned on status of DeleteAll 
	smsStore.CancelAsyncRequest(EMobilePhoneStoreDeleteAllCancel);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::GetInfo, timed out"));
	ASSERT_TRUE(	deleteAllStatus.Int()==KErrNone ||
					deleteAllStatus.Int()==KErrCancel, 
					_L("RMobileSmsStore::GetInfo returned an error"));
	
	// ===  Check phone book is not empty but some entries have been deleted. ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1   
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::GetInfo, timed out"));
	ASSERT_EQUALS(getInfoStatus.Int(), KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	// Check iUsedEntries < before deletion.
	ASSERT_TRUE(storeInfo.iUsedEntries < storeUsedEntries, _L("RMobilePhoneStore::GetInfo returned an unexpected number of iUsedEntries"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// deleteStatus
	CleanupStack::PopAndDestroy(2, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0015");
	}



CCTSYIntegrationTestSMSStore0016::CCTSYIntegrationTestSMSStore0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0016::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0016::~CCTSYIntegrationTestSMSStore0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0016
 * @SYMFssID BA/CTSY/SMSS-0016
 * @SYMTestCaseDesc Write corrupt SMS message to the SMS store.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Write, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - Error returned on write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess ;
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	TInt nextFreeSpace = iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone);
	CHECK_TRUE_L(nextFreeSpace>=0,_L("TSimTsyTestHelper::GetSmsMessagingL returned an error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1   
	// Store iUsedEntries returned for future use 
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Store iUsedEntries returned for future use 
	TInt storeUsedEntries = -1;
	TInt phoneUsedEntries = -1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
			phoneStoreInfoPckg,
			phone,
			storeUsedEntries,
			phoneUsedEntries),
			KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	
	// ===  Construct a corrupt SMS message ===

	// Create a  RMobileSmsStore::TMobileGsmSmsEntryV1 with a corrupt iMsgData. 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
       
    // Create an entry 
	CreateGsmSmsEntry(entry, -1, KCorruptSmsPdu(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
			RMobilePhone::EIsdnNumberPlan);
	
	// Write the entry to the SMS store. 
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);	

	// $CTSYProblem When the LTSY does not
	// panic when a corrupt SMS is passed to it for writing to the SMS
	// Store.
/*	smsStore.Write(writeStatus,pckgEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),KErrNone, _L("RMobileSmsStore::Write timed out"))		

	// Check error != KErrNone is returned on attempt to write.
	ASSERT_TRUE(writeStatus.Int() != KErrNone, _L("RMobileSmsStore::Write did not return the expected error != KErrNone"))					

	// ===  Get the number of used entries and check it is the same as before attempted write ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, phoneStoreInfoPckg, phone,
			storeUsedEntries, phoneUsedEntries),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
*/
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// writeStatus
	CleanupStack::PopAndDestroy(2, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0016");
	}



CCTSYIntegrationTestSMSStore0017::CCTSYIntegrationTestSMSStore0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0017::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0017::~CCTSYIntegrationTestSMSStore0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0017
 * @SYMFssID BA/CTSY/SMSS-0017
 * @SYMTestCaseDesc Store an empty SMS 
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SMS message stored
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify SMS message are written and can be read.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess | KCapsWriteAccess
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	TUint wantedBits =  RMobilePhoneStore::KCapsReadAccess |
						RMobilePhoneStore::KCapsWriteAccess;
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));
				

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// ===  Find out how many entries are used in the phone store ===
	
	// Get phone store information using RMobilePhone::GetPhoneStoreInfo and KETelIccSmsStore 
	// Store the iUsedEntries returned.
	// Get phone book store information using RMobilePhoneStore::GetInfo 
	// Store the iUsedEntries returned.
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	TInt usedEntries = -1;
	TInt usedEntries2 = -1;
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
			phoneStoreInfoPckg,
			phone, 
			usedEntries, 
			usedEntries2), 
			KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));

	// ===  Write a valid entry (RMobileSmsStore::TMobileGsmSmsEntryV1) ===

	// post notifier for RMobilePhoneStore::NotifyStoreEvent
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);
	
	// Write a valid empty entry with a entry into the phone store specifying a specific index to store the entry which does not contain an entry. 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry); 
	CreateGsmSmsEntry(entry, -1, KValidEmptyPdu(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
			RMobilePhone::EIsdnNumberPlan);
	
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);	
	smsStore.Write(writeStatus,pckgEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Write Timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));

	TEST_CHECK_POINT_L(_L("RMobileSmsStore::Write returned an error, not proceeding with test"));
		
	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryAdded
	RMobilePhoneStore::TMobileStoreEvent expectedEvent = RMobilePhoneStore::KStoreEntryAdded ;
	iSmsStoreTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
													notifyStoreEventStatus,
													storeEvent,
													expectedEvent,
													index);

	// ===  Read the written entry ===

	// Read the entry from the phone store. 
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgReadEntry(readEntry);
	readEntry.iIndex = index;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;
	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);
		
	smsStore.Read(readStatus,pckgReadEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Read, Timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));			
	
	// Check that the entry information is the same as that written.
	ASSERT_EQUALS(readEntry.iMsgStatus, RMobileSmsStore::EStoredMessageUnread, 	_L("RMobileSmsStore::Read Read Entry has the wrong iMsgStatus"));
	ASSERT_EQUALS_DES8(readEntry.iMsgData, KValidEmptyPdu, _L("RMobileSmsStore::Read Read Entry has the wrong iMsgData"));
									
	// Check iServiceCentre is the same as that written to the phone book in set up.
	ASSERT_EQUALS(readEntry.iServiceCentre.iTypeOfNumber,  RMobilePhone::ENationalNumber, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iTypeOfNumber"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iNumberPlan, RMobilePhone::EIsdnNumberPlan, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iNumberPlan"));
	ASSERT_EQUALS_DES16(readEntry.iServiceCentre.iTelNumber, KNumber, _L("RMobileSmsStore::Read Read Entry has the wrong iServiceCentre.iTelNumber"));

	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored if this API is supported
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, 
				phoneStoreInfoPckg, phone, usedEntries +1, usedEntries2 +1), 
				KErrNone, 
				_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
    // notifyStoreEventStatus
	// writeStatus
	// readStatus
	CleanupStack::PopAndDestroy(4, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0017");
	}



CCTSYIntegrationTestSMSStore0018::CCTSYIntegrationTestSMSStore0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0018::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0018::~CCTSYIntegrationTestSMSStore0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0018
 * @SYMFssID BA/CTSY/SMSS-0018
 * @SYMTestCaseDesc Delete an SMS message from an invalid slot.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Delete, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrPathNotFound returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrPathNotFound is returned and number of phone store entries is unaffected.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess  
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess;
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneStore::TMobilePhoneStoreInfoV1   
	// Store iUsedEntries returned for future use 
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Store iUsedEntries returned for future use
	TInt storeUsedEntries = -1;
	TInt phoneUsedEntries = -1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(smsStore, KETelIccSmsStore, 
			phoneStoreInfoPckg,
			phone,
			storeUsedEntries,
			phoneUsedEntries),
			KErrNone,  _L("RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo both returned an error."));
	
	// ===  Delete an entry from an invalid slot number ===

	// Delete an entry from slot -1 
	// Repeat test deleting from slot 0 and -10 
	TInt indexArray[] = {-1,0,-10};
	
	TExtEtelRequestStatus deleteStatus (smsStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStatus);
	
	for (TInt i = 0; i< 3; i++)
		{
		
		if (indexArray[i] == 0)
			{
			ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10011);
			}
		
		smsStore.Delete(deleteStatus, indexArray[i]);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Delete timed out"));
		ASSERT_TRUE(deleteStatus.Int() != KErrNone, _L("RMobileSmsStore::Delete returned KErrNone when we were expecting an error"));

		// ===  Get the number of used entries and check it is the same as before attempted delete ===

		// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
		ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(smsStore, KETelIccSmsStore, phoneStoreInfoPckg, phone,
			storeUsedEntries, phoneUsedEntries),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
		}
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// deleteStatus
	CleanupStack::PopAndDestroy(2,&getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0018");
	}



CCTSYIntegrationTestSMSStore0019::CCTSYIntegrationTestSMSStore0019(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0019::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0019::~CCTSYIntegrationTestSMSStore0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0019
 * @SYMFssID BA/CTSY/SMSS-0019
 * @SYMTestCaseDesc Cancel get SMS store info.
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
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Request to get phone store information by calling RMobilePhoneStore::GetInfo 
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);

	// Cancel get information by calling RTelSubSession::CancelAsyncRequest(EMobilePhoneStoreGetInfo) 
	smsStore.CancelAsyncRequest(EMobilePhoneStoreGetInfoCancel);
	
	// Check request completes with KErrCancel | KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::GetInfo, timed out"));
	ASSERT_TRUE((getInfoStatus.Int() == KErrCancel) || (getInfoStatus.Int() == KErrNone), _L("RMobileSmsStore::GetInfo did not return KErrCancel or KErrNone"));

	// Call RMobilePhone::GetPhoneStoreInfo with aStoreName=KETelIccSmsStore 
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	TExtEtelRequestStatus phoneGetInfoStatus (phone,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(phoneGetInfoStatus);
	phone.GetPhoneStoreInfo(phoneGetInfoStatus,phoneStoreInfoPckg, KETelIccSmsStore);

	// Cancel request with RTelSubSession::CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo) 
	phone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);

	// Check request completes with KErrCancel | KErrNone | KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneGetInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetPhoneStoreInfo, timed out"));
	ASSERT_TRUE((phoneGetInfoStatus.Int() == KErrCancel) || 
				(phoneGetInfoStatus.Int() == KErrNone) || 
				(phoneGetInfoStatus.Int() == KErrNotSupported), _L("RMobilePhone::GetPhoneStoreInfo did not return KErrCancel or KErrNone or KErrNotSupported"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// phoneGetInfoStatus
	CleanupStack::PopAndDestroy(2, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0019");
	}



CCTSYIntegrationTestSMSStore0020::CCTSYIntegrationTestSMSStore0020(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0020::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0020::~CCTSYIntegrationTestSMSStore0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0020
 * @SYMFssID BA/CTSY/SMSS-0020
 * @SYMTestCaseDesc Cancel read SMS message from SMS store.
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

	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	TUint wantedBits =  RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	// Write an entry to a known location in the phone book 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
	CreateGsmSmsEntry(entry, 1, KValidSmsPdu1(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
			RMobilePhone::EIsdnNumberPlan);
 		
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);	
	smsStore.Write(writeStatus,pckgEntry);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium)==KErrNone, _L("RMobileSmsStore::Write timed out"));
	CHECK_TRUE_L(writeStatus.Int() == KErrNone, _L("RMobileSmsStore::Write returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Read an entry from a known location from the SMS store. 
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgReadEntry(readEntry);
	readEntry.iIndex = 1; 
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;

	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);	
	smsStore.Read(readStatus,pckgReadEntry);

	// Cancel the read request immediately using RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreRead) 
	smsStore.CancelAsyncRequest(EMobilePhoneStoreReadCancel);
	
	// Check KErrCancel or KErrNone is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium), KErrNone,	_L("RMobileSmsStore::Read, timed out"));
	ASSERT_TRUE(readStatus.Int() == KErrNone || readStatus.Int() ==  KErrCancel, _L("RMobileSmsStore::Read returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// writeStatus
	// readStatus
	CleanupStack::PopAndDestroy(3, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0020");
	}



CCTSYIntegrationTestSMSStore0021::CCTSYIntegrationTestSMSStore0021(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0021::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0021::~CCTSYIntegrationTestSMSStore0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0021
 * @SYMFssID BA/CTSY/SMSS-0021
 * @SYMTestCaseDesc Cancel delete message from SMS store.
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

	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess;
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	// Write an entry to a known location in the phone book 
 	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
    
    // Create an entry 
	CreateGsmSmsEntry(entry, 5, KValidSmsPdu1(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
			RMobilePhone::EIsdnNumberPlan);
	
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	smsStore.Write(writeStatus,pckgEntry);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium)==KErrNone, _L("RMobileSmsStore::Write timed out"));
	CHECK_TRUE_L(writeStatus.Int() == KErrNone, _L("RMobileSmsStore::Write returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Delete an entry at a known index from the SMS store. 
	TExtEtelRequestStatus deleteStatus (smsStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStatus);
	smsStore.Delete(deleteStatus,entry.iIndex);			
	
	// Cancel the request immediately with RTelSubSessionBase::.CancelAsyncRequest( EMobilePhoneStoreDelete ) 
	smsStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
	
	// Check KErrCancel or KErrNone is returned (if it was too late to cancel).
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::Delete timed out"));
	ASSERT_TRUE(deleteStatus.Int() == KErrNone ||  deleteStatus.Int() == KErrCancel, _L("RMobileSmsStore::Delete returned an error"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop 
	// getInfoStatus
	// writeStatus
	// deleteStatus
	CleanupStack::PopAndDestroy(3, &getInfoStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0021");
	}



CCTSYIntegrationTestSMSStore0022::CCTSYIntegrationTestSMSStore0022(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0022::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0022::~CCTSYIntegrationTestSMSStore0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0022
 * @SYMFssID BA/CTSY/SMSS-0022
 * @SYMTestCaseDesc Cancel write message to SMS store.
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

	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	TUint wantedBits =  RMobilePhoneStore::KCapsWriteAccess;
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	// Ensure phone store is not empty. 
	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);	    
	
	if(storeInfo.iUsedEntries < 1)
		{
	    // Create an entry 
	    CreateGsmSmsEntry(entry, -1, KValidSmsPdu1(),
				RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
				RMobilePhone::EIsdnNumberPlan);

		smsStore.Write(writeStatus,pckgEntry);
		CHECK_TRUE_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium)==KErrNone, _L("RMobileSmsStore::Write timed out"));
		CHECK_TRUE_L(writeStatus.Int() == KErrNone, _L("RMobileSmsStore::Write returned an error"));

		smsStore.GetInfo(getInfoStatus,storeInfoPckg);
		CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
		CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
		}
	
	CHECK_TRUE_L(storeInfo.iUsedEntries > 0, _L("RMobileSmsStore.GetInfo returned storeInfo.iUserEntries >0"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Write an entry at a known index from the SMS store. 
	CreateGsmSmsEntry(entry, 3, KValidSmsPdu1(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
			RMobilePhone::EIsdnNumberPlan);
	smsStore.Write(writeStatus,pckgEntry);	

	// Cancel the request immediately with RTelSubSessionBase::.CancelAsyncRequest( EMobilePhoneStoreWrite ) 
	smsStore.CancelAsyncRequest(EMobilePhoneStoreWrite);
	
	// Check KErrCancel or KErrNone is returned (if it was too late to cancel).
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Write timed out"));
	ASSERT_TRUE((writeStatus.Int() == KErrCancel) || (writeStatus.Int() == KErrNone) , _L("RMobileSmsStore::Write returned an unexpected error of KErrNone or KErrCancel"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// writeStatus
	CleanupStack::PopAndDestroy(2, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0022");
	}



CCTSYIntegrationTestSMSStore0023::CCTSYIntegrationTestSMSStore0023(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSStore0023::GetTestStepName());
	}

CCTSYIntegrationTestSMSStore0023::~CCTSYIntegrationTestSMSStore0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSStore0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSS-0023
 * @SYMFssID BA/CTSY/SMSS-0023
 * @SYMTestCaseDesc Cancel notify store event on SMS store.
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

	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsNotifyEvent 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);

	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium) == KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_TRUE_L(getInfoStatus.Int() == KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

	TUint wantedBits =  RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(storeInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect Store Info Caps"));

	// Write an entry to a known location in the phone book 

    // Create an entry 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);	      
	CreateGsmSmsEntry(entry, -1, KValidSmsPdu1(),
			RMobileSmsStore::EStoredMessageUnread, RMobilePhone::ENationalNumber,
			RMobilePhone::EIsdnNumberPlan);

	TExtEtelRequestStatus writeStatus (smsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	smsStore.Write(writeStatus,pckgEntry);
	CHECK_TRUE_L(WaitForRequestWithTimeOut(writeStatus, ETimeMedium)==KErrNone, _L("RMobileSmsStore::Write timed out"));
	CHECK_TRUE_L(writeStatus.Int() == KErrNone, _L("RMobileSmsStore::Write returned an error"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Request notification of a store event with RMobilePhoneStore::NotifyStoreEvent 
	
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);
	
	// Cancel the RMobilePhoneStore::NotifyStoreEvent with RTelSubSession::CancelAsyncRequest( EMobilePhoneStoreNotifyStoreEvent ) 
	smsStore.CancelAsyncRequest (EMobilePhoneStoreNotifyStoreEvent);
	
	// Do some operations which would trigger a store event if it hadn't been cancelled. 

	// Delete an entry from the phone store 
	TExtEtelRequestStatus deleteStatus (smsStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStatus);
	smsStore.Delete(deleteStatus,5);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Delete timed out"));
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RMobileSmsStore::Delete returned an error"));

	// Write an entry to the phone store 
	entry.iIndex = -1;
	smsStore.Write(writeStatus,pckgEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with KErrCancel
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyStoreEventStatus, ETimeMedium), KErrNone, _L("RMobilePhoneStore::NotifyStoreEvent timed out"));
	ASSERT_EQUALS(notifyStoreEventStatus.Int(), KErrCancel, _L("RMobilePhoneStore::NotifyStoreEvent did not complete with KErrCancel"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getInfoStatus
	// writeStatus
	// notifyStoreEventStatus
	// deleteStatus
	CleanupStack::PopAndDestroy(4,&getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSStore0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSStore0023");
	}



