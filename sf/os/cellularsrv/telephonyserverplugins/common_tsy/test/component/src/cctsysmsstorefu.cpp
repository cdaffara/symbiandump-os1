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
// The TEFUnit test suite for SmsStore in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsysmsstorefu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "cctsyactiveretriever.h"
#include <mmretrieve.h>
#include <ctsy/serviceapi/cmmsmsutility.h>

CTestSuite* CCTsySmsStoreFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestNotifyStoreEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestNotifyStoreEvent0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestNotifyStoreEvent0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestRead0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestRead0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestRead0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestRead0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestRead0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestDelete0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestDelete0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestDelete0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestDelete0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestGetInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestGetInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestGetInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestGetInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestGetInfo0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestDeleteAll0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestDeleteAll0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestDeleteAll0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestDeleteAll0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestWrite0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestWrite0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestWrite0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestWrite0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestWrite0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestStart0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestStart0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestStart0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestStart0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsStoreFU, TestStartBatch0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSNSE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::NotifyStoreEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::NotifyStoreEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestNotifyStoreEvent0001L()
	{
	AuxNotifyStoreEvent1L(KETelIccSmsStore);

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

    RMobileSmsStore store;
    res = store.Open(messaging, KETelMeSmsStore);
	ASSERT_EQUALS(KErrNotSupported, res);

	CleanupStack::PopAndDestroy(2, this); // this, messaging
	
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSNSE-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsStore::NotifyStoreEvent
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsStore::NotifyStoreEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestNotifyStoreEvent0002L()
	{
	AuxNotifyStoreEvent2L(KETelIccSmsStore);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSNSE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsStore::NotifyStoreEvent
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsStore::NotifyStoreEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestNotifyStoreEvent0004L()
	{
	AuxNotifyStoreEvent4L(KETelIccSmsStore);	
	}

/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Read
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Read
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestRead0001L()
	{
	AuxRead1L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsStore::Read
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsStore::Read
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestRead0002L()
	{
	AuxRead2L(KETelIccSmsStore);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Read with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Read with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestRead0003L()
	{
	AuxRead3L(KETelIccSmsStore);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsStore::Read
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsStore::Read
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestRead0004L()
	{
	AuxRead4L(KETelIccSmsStore);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Read with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Read and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestRead0005L()
	{
	AuxRead5L(KETelIccSmsStore);	
	}

/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Delete
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestDelete0001L()
	{
	AuxDelete1L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSD-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsStore::Delete
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsStore::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestDelete0002L()
	{
	AuxDelete2L(KETelIccSmsStore);
	}

/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSD-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsStore::Delete
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsStore::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestDelete0004L()
	{
	AuxDelete4L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSD-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Delete with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Delete and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestDelete0005L()
	{
	AuxDelete5L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSGI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::GetInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::GetInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestGetInfo0001L()
	{
	AuxGetInfo1L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSGI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsStore::GetInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsStore::GetInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestGetInfo0002L()
	{
	AuxGetInfo2L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSGI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::GetInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::GetInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestGetInfo0003L()
	{
	AuxGetInfo3L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSGI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsStore::GetInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsStore::GetInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestGetInfo0004L()
	{
	AuxGetInfo4L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSGI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::GetInfo with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::GetInfo and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestGetInfo0005L()
	{
	AuxGetInfo5L(KETelIccSmsStore);
	}

/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSDA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::DeleteAll
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::DeleteAll
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestDeleteAll0001L()
	{
	AuxDeleteAll1L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSDA-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsStore::DeleteAll
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsStore::DeleteAll
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestDeleteAll0002L()
	{
	AuxDeleteAll2L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSDA-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsStore::DeleteAll
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsStore::DeleteAll
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestDeleteAll0004L()
	{
	AuxDeleteAll4L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSDA-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::DeleteAll with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::DeleteAll and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestDeleteAll0005L()
	{
	AuxDeleteAll5L(KETelIccSmsStore);
	}

/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSW-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Write
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Write
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestWrite0001L() 
	{
	AuxWrite1L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSW-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsStore::Write
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsStore::Write
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestWrite0002L()
	{
	AuxWrite2L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSW-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Write with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Write with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestWrite0003L()
	{
	
	/*
	Test passing wrong descriptor size to parameter fails. 
	Function execution invokes KERN:4 panic.
	See line 691 [TInt CMmSmsStorageTsy::CompleteWriteSms()] in file CMmSmsStorageTsy.cpp.
	*/
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 420601);
	
	AuxWrite3L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSW-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsStore::Write
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsStore::Write
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestWrite0004L()
	{
	AuxWrite4L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSW-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsStore::Write with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsStore::Write and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestWrite0005L()
	{
	AuxWrite5L(KETelIccSmsStore);
	}

/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneSmsList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneSmsList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestStart0001L()
	{
	AuxStart1L(KETelIccSmsStore);
	}

/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSS-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneSmsList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneSmsList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestStart0001bL()
	{
	AuxStart1bL(KETelIccSmsStore);
	}

/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrieveMobilePhoneSmsList::Start
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrieveMobilePhoneSmsList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestStart0002L()
	{
	AuxStart2L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to CRetrieveMobilePhoneSmsList::Start
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to CRetrieveMobilePhoneSmsList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestStart0004L()
	{
	AuxStart4L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneSmsList::Start with timeout
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneSmsList::Start and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestStart0005L()
	{
	AuxStart5L(KETelIccSmsStore);
	}


/**
@SYMTestCaseID BA-CTSY-PBSMS-SMSSSB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneSmsList::StartBatch 
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneSmsList::StartBatch 
@SYMTestExpectedResults KErrNotSupported is expected.
@SYMTestType CT
*/
void CCTsySmsStoreFU::TestStartBatch0001L()
	{
	AuxStartBatch1L(KETelIccSmsStore);
	}


void CCTsySmsStoreFU::AuxRead1L(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------
	
	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
	
	TInt index = -1;
	TMockLtsyData1<TInt> mockIndex(index);

	TSmsMsg smsMsg;
	smsMsg.iSmsMsg = _L8("Hello, World!"); 
	smsMsg.iServiceCentre = _L("ServiceSenter1"); 
	smsMsg.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
	smsMsg.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
	smsMsg.iMessageStatus = KSimSmsMtRead; //0x01, RMobileSmsStore::EStoredMessageRead
	smsMsg.iLocation      = 200;
	smsMsg.iDeleteAfterClientAck = EFalse;
	smsMsg.iSmsClass2            = ETrue;

	TMockLtsyData1<TSmsMsg> mockMsg(smsMsg);

	mockIndex.SerialiseL(expData);
	mockMsg.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreRead, expData, KErrNotSupported);

	store.Read(requestStatus, pckgEntryV1);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreRead, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreRead, KErrGeneral, completeData);

    store.Read(requestStatus, pckgEntryV1);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileENStore::Read when result is not cached.
 	//-------------------------------------------------------------------------
		
	iMockLTSY.ExpectL(EMobilePhoneStoreRead, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreRead, KErrNone, completeData);

	store.Read(requestStatus, pckgEntryV1);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	ASSERT_EQUALS(0, smsMsg.iSmsMsg.Compare(entryV1.iMsgData));	
	ASSERT_EQUALS(0, smsMsg.iServiceCentre.Compare(entryV1.iServiceCentre.iTelNumber));	
	ASSERT_EQUALS(smsMsg.iMobileScTON, entryV1.iServiceCentre.iTypeOfNumber);	
	ASSERT_EQUALS(smsMsg.iMobileScNPI, entryV1.iServiceCentre.iNumberPlan);	
	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageRead, entryV1.iMsgStatus);	

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileENStore::Read
	// from LTSY.
	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreRead, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------
	
	RArray< TUint> messageStatusArray;
	CleanupClosePushL(messageStatusArray);
	
	messageStatusArray.AppendL(KSimSmsMoNotSent);
	messageStatusArray.AppendL(KSimSmsStatusReportReqNotReceived);
	messageStatusArray.AppendL(KSimSmsMoSent);
	messageStatusArray.AppendL(KSimSmsMtNotRead);
	messageStatusArray.AppendL(KSimSmsStatusReportReqReceivedNotStored);
	messageStatusArray.AppendL(KSimSmsStatusReportReqReceivedStored);
	
	for (TInt i = 0; i < messageStatusArray.Count(); i++)
		{
		smsMsg.iMessageStatus = messageStatusArray[i];
		TMockLtsyData1<TSmsMsg> mockMsgb(smsMsg);
		
		completeData.Close();
		mockMsgb.SerialiseL(completeData);
		
		iMockLTSY.ExpectL(EMobilePhoneStoreRead, expData);
		iMockLTSY.CompleteL(EMobilePhoneStoreRead, KErrNone, completeData);

	    store.Read(requestStatus, pckgEntryV1);
	    
	    User::WaitForRequest(requestStatus);        
	    AssertMockLtsyStatusL();
		ASSERT_EQUALS(KErrNone, requestStatus.Int());	

		ASSERT_EQUALS(0, smsMsg.iSmsMsg.Compare(entryV1.iMsgData));	
		ASSERT_EQUALS(0, smsMsg.iServiceCentre.Compare(entryV1.iServiceCentre.iTelNumber));	
		ASSERT_EQUALS(smsMsg.iMobileScTON  , entryV1.iServiceCentre.iTypeOfNumber);	
		ASSERT_EQUALS(smsMsg.iMobileScNPI  , entryV1.iServiceCentre.iNumberPlan);	
		
	    switch( smsMsg.iMessageStatus )
	        {
	        case KSimSmsMoNotSent: //0x07
	        	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageUnsent, entryV1.iMsgStatus);	
	            break;
	        case KSimSmsStatusReportReqNotReceived: //0x0D
	        case KSimSmsMoSent: //0x05
	        	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageSent, entryV1.iMsgStatus);	
	            break;
	        case KSimSmsMtRead: //0x01
	        	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageRead, entryV1.iMsgStatus);	
	            break;
	        case KSimSmsMtNotRead: //0x03
	        	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageUnread, entryV1.iMsgStatus);	
	            break;
		    case KSimSmsStatusReportReqReceivedNotStored: //0x15
			case KSimSmsStatusReportReqReceivedStored: //0x1D
	        	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageDelivered, entryV1.iMsgStatus);	
	            break;
	        default:
	        	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageUnknownStatus, entryV1.iMsgStatus);	
	            break;
	        }	
		}
	

	CleanupStack::PopAndDestroy(6, this); // expData, completeData, this, messaging, store
	                                      // messageStatusArray
	
	}

void CCTsySmsStoreFU::AuxRead2L(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------
	
	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
	
	TInt index = -1;
	TMockLtsyData1<TInt> mockIndex(index);

	TSmsMsg smsMsg;
    smsMsg.iSmsMsg = _L8("Hello, World!"); 
    smsMsg.iServiceCentre = _L("ServiceSenter1"); 
    smsMsg.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
    smsMsg.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
    smsMsg.iMessageStatus = KSimSmsMtRead; //0x01, RMobileSmsStore::EStoredMessageRead
    smsMsg.iLocation      = 200;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsClass2            = ETrue;

	TMockLtsyData1<TSmsMsg> mockMsg(smsMsg);

	mockIndex.SerialiseL(expData);
	mockMsg.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileSmsStore::Read
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	iMockLTSY.ExpectL(EMobilePhoneStoreRead, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreRead, KErrNone, completeData);
	
    store.Read(requestStatus, pckgEntryV1);
    store.CancelAsyncRequest(EMobilePhoneStoreRead);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());


	CleanupStack::PopAndDestroy(5, this); // expData, completeData, this, messaging, store
		
	}

void CCTsySmsStoreFU::AuxRead3L(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------
	
	TInt index = 56;
	TMockLtsyData1<TInt> mockIndex(index);

	TSmsMsg smsMsg;
    smsMsg.iSmsMsg = _L8("Hello, World!"); 
    smsMsg.iServiceCentre = _L("ServiceSenter1"); 
    smsMsg.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
    smsMsg.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
    smsMsg.iMessageStatus = KSimSmsMtRead; //0x01, RMobileSmsStore::EStoredMessageRead
    smsMsg.iLocation      = 200;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsClass2            = ETrue;

	TMockLtsyData1<TSmsMsg> mockMsg(smsMsg);

	mockIndex.SerialiseL(expData);
	mockMsg.SerialiseL(completeData);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileSmsStore::Read
 	//-------------------------------------------------------------------------

	TBuf8<1> badParam;
    store.Read(requestStatus, badParam);
    
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	
	
	CleanupStack::PopAndDestroy(5, this); // expData, completeData, this, messaging, store
		
	}
	
void CCTsySmsStoreFU::AuxRead4L(const TDesC& aName)
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2; 
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    // open the message store1
    RMobileSmsMessaging messaging1;
    TInt res = messaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging1);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store1;
    res = store1.Open(messaging1, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store1);

    // open the message store2
    RMobileSmsMessaging messaging2;
    res = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging2);

    RMobileSmsStore store2;
    res = store2.Open(messaging2, KETelIccSmsStore);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store2);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------
	// param for first function
	
	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV11;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV11(entryV11);
	
	TInt index = -1;
	TMockLtsyData1<TInt> mockIndex(index);

	TSmsMsg smsMsg1;
    smsMsg1.iSmsMsg = _L8("Hello, World!"); 
    smsMsg1.iServiceCentre = _L("ServiceSenter1"); 
    smsMsg1.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
    smsMsg1.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
    smsMsg1.iMessageStatus = KSimSmsMtRead; // RMobileSmsStore::EStoredMessageRead
    smsMsg1.iLocation      = 200;
    smsMsg1.iDeleteAfterClientAck = EFalse;
    smsMsg1.iSmsClass2            = ETrue;

	TMockLtsyData1<TSmsMsg> mockMsg1(smsMsg1);

 	//-------------------------------------------------------------------------
	// param for second function
	
	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV12;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV12(entryV12);
	
	TSmsMsg smsMsg2;
    smsMsg2.iSmsMsg = _L8("Hello, World!"); 
    smsMsg2.iServiceCentre = _L("ServiceSenter1"); 
    smsMsg2.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
    smsMsg2.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
    smsMsg2.iMessageStatus = KSimSmsMtRead; // RMobileSmsStore::EStoredMessageRead
    smsMsg2.iLocation      = 200;
    smsMsg2.iDeleteAfterClientAck = EFalse;
    smsMsg2.iSmsClass2            = ETrue;

	TMockLtsyData1<TSmsMsg> mockMsg2(smsMsg2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileSmsStore::Read
 	//-------------------------------------------------------------------------

	mockIndex.SerialiseL(expData);
	mockMsg1.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMobilePhoneStoreRead, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreRead, KErrNone, completeData);

    store1.Read(requestStatus1, pckgEntryV11);

	completeData.Close();
	mockMsg2.SerialiseL(completeData);

	iMockLTSY.CompleteL(EMobilePhoneStoreRead, KErrNone, completeData);

    store2.Read(requestStatus2, pckgEntryV12);
    
    User::WaitForRequest(requestStatus1);        
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

    User::WaitForRequest(requestStatus2);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

	ASSERT_EQUALS(0, smsMsg1.iSmsMsg.Compare(entryV11.iMsgData));	
	ASSERT_EQUALS(0, smsMsg1.iServiceCentre.Compare(entryV11.iServiceCentre.iTelNumber));	
	ASSERT_EQUALS(smsMsg1.iMobileScTON, entryV11.iServiceCentre.iTypeOfNumber);	
	ASSERT_EQUALS(smsMsg1.iMobileScNPI, entryV11.iServiceCentre.iNumberPlan);	
	ASSERT_EQUALS(RMobileSmsStore::EStoredMessageRead, entryV11.iMsgStatus);	
	
	CleanupStack::PopAndDestroy(9, this); // phone2, telServer2, expData, this, completeData
	                                      // store1, messaging1, store2, messaging2

	}
	
void CCTsySmsStoreFU::AuxRead5L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	//-------------------------------------------------------------------------

	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
	
	TInt index = -1;
	TMockLtsyData1<TInt> mockIndex(index);

	mockIndex.SerialiseL(expData);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileSmsStore::Read
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreRead, expData);

	store.Read(requestStatus, pckgEntryV1);
	
	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(4, this); // expData, this, messaging, store

	}	
	
void CCTsySmsStoreFU::AuxWrite1L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------

	TInt index = -1;

	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
	
	entryV1.iServiceCentre.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	entryV1.iServiceCentre.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	entryV1.iServiceCentre.iTelNumber    = _L("TelNumber1");
	entryV1.iMsgData = _L8("Hello, World!");
	entryV1.iMsgStatus = RMobileSmsStore::EStoredMessageUnsent;
	entryV1.iIndex = index;
	
	TMockLtsyData2<RMobileSmsStore::TMobileGsmSmsEntryV1, TInt> 
		expMockData2(entryV1, entryV1.iIndex);

    TUint8 location = 100;
	TBool  receivedClass2ToBeResent = ETrue;

	TMockLtsyData2<TUint8, TBool> completeMockData2(location, receivedClass2ToBeResent);

	expMockData2.SerialiseL(expData);
	completeMockData2.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreWrite, expData, KErrNotSupported);

	store.Write(requestStatus, pckgEntryV1);

    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreWrite, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreWrite, KErrGeneral, completeData);
	
	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteWriteSms().
	 */
	iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

	store.Write(requestStatus, pckgEntryV1);

    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::Write when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreWrite, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreWrite, KErrNone, completeData);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteWriteSms().
	 */
	iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

	store.Write(requestStatus, pckgEntryV1);

    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(static_cast<TUint8>(entryV1.iIndex), location);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileSmsStore::Write
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreWrite, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(5, this); // expData, completeData, this
	                                      // store, messaging
	
	}

void CCTsySmsStoreFU::AuxWrite2L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------

	TInt index = -1;

	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
	
	entryV1.iServiceCentre.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	entryV1.iServiceCentre.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	entryV1.iServiceCentre.iTelNumber    = _L("TelNumber1");
	entryV1.iMsgData = _L8("Hello, World!");
	entryV1.iMsgStatus = RMobileSmsStore::EStoredMessageUnsent;
	entryV1.iIndex = index;
	
	TMockLtsyData2<RMobileSmsStore::TMobileGsmSmsEntryV1, TInt> 
		expMockData2(entryV1, entryV1.iIndex);

    TUint8 location = 100;
	TBool  receivedClass2ToBeResent = ETrue;

	TMockLtsyData2<TUint8, TBool> completeMockData2(location, receivedClass2ToBeResent);

	expMockData2.SerialiseL(expData);
	completeMockData2.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileSmsStore::Write
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	iMockLTSY.ExpectL(EMobilePhoneStoreWrite, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreWrite, KErrNone, completeData, 10);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteWriteSms().
	 */
	iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

	store.Write(requestStatus, pckgEntryV1);

    store.CancelAsyncRequest(EMobilePhoneStoreWrite);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	CleanupStack::PopAndDestroy(5, this); // expData, completeData, this
	                                      // store, messaging
	}
	
void CCTsySmsStoreFU::AuxWrite3L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

	RMobileSmsStore store;
	res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	//-------------------------------------------------------------------------

	RMobileSmsStore::TMobileCdmaSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileCdmaSmsEntryV1> pckgEntryV1(entryV1);
	
	entryV1.iTeleservice = 0;
	entryV1.iServiceCategory = 0;
	entryV1.iRemoteParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;;
	entryV1.iRemoteParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	entryV1.iRemoteParty.iTelNumber = _L("123456");
	entryV1.iMsgData = _L8("Hello, World!");
	
	TMockLtsyData2<RMobileSmsStore::TMobileCdmaSmsEntryV1, TInt> 
		expMockData2(entryV1, entryV1.iIndex);

	TUint8 location = 100;
	TBool  receivedClass2ToBeResent = ETrue;

	TMockLtsyData2<TUint8, TBool> completeMockData2(location, receivedClass2ToBeResent);

	expMockData2.SerialiseL(expData);
	completeMockData2.SerialiseL(completeData);
	
	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobileSmsStore::Write
	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreWrite, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreWrite, KErrNone, completeData);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteWriteSms().
	 */
	iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

	store.Write(requestStatus, pckgEntryV1);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileSmsStore::Write
	//-------------------------------------------------------------------------
    TBuf8<1> badSize;
	store.Write(requestStatus, badSize);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	

	CleanupStack::PopAndDestroy(5, this); // expData, completeData, this
	                                      // store, messaging

	}

void CCTsySmsStoreFU::AuxWrite4L(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2; 
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    // open the message store1
    RMobileSmsMessaging messaging1;
    TInt res = messaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging1);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store1;
    res = store1.Open(messaging1, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store1);

    // open the message store2
    RMobileSmsMessaging messaging2;
    res = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging2);

    RMobileSmsStore store2;
    res = store2.Open(messaging2, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store2);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------
	// parameters for first function

	TInt index = -1;

	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV11;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV11(entryV11);
	
	entryV11.iServiceCentre.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	entryV11.iServiceCentre.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	entryV11.iServiceCentre.iTelNumber    = _L("TelNumber1");
	entryV11.iMsgData = _L8("Hello, World!");
	entryV11.iMsgStatus = RMobileSmsStore::EStoredMessageUnsent;
	entryV11.iIndex = index;
	
	TMockLtsyData2<RMobileSmsStore::TMobileGsmSmsEntryV1, TInt> 
		expMockData21(entryV11, entryV11.iIndex);

    TUint8 location = 100;
	TBool  receivedClass2ToBeResent = ETrue;

	TMockLtsyData2<TUint8, TBool> completeMockData21(location, receivedClass2ToBeResent);

 	//-------------------------------------------------------------------------
	// parameters for second function

	index = 1;

	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV12;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV12(entryV12);
	
	entryV12.iServiceCentre.iNumberPlan   = RMobilePhone::EServiceCentreSpecificPlan1;
	entryV12.iServiceCentre.iTypeOfNumber = RMobilePhone::ESubscriberNumber;
	entryV12.iServiceCentre.iTelNumber    = _L("TelNumber2");
	entryV12.iMsgData = _L8("Good-bye, World!");
	entryV12.iMsgStatus = RMobileSmsStore::EStoredMessageDelivered;
	entryV12.iIndex = index;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileSmsStore::Write
 	//-------------------------------------------------------------------------

	expMockData21.SerialiseL(expData);
	completeMockData21.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMobilePhoneStoreWrite, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreWrite, KErrNone, completeData, 10);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteWriteSms().
	 */
	iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

	store1.Write(requestStatus1, pckgEntryV11);

	store2.Write(requestStatus2, pckgEntryV12);

    User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

    User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(9, this); // phone2, telServer2, expData, this, completeData
	                                      // store1, messaging1, store2, messaging2
	}	
	
void CCTsySmsStoreFU::AuxWrite5L(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------

	TInt index = -1;

	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
	
	entryV1.iServiceCentre.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	entryV1.iServiceCentre.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	entryV1.iServiceCentre.iTelNumber    = _L("TelNumber1");
	entryV1.iMsgData = _L8("Hello, World!");
	entryV1.iMsgStatus = RMobileSmsStore::EStoredMessageUnsent;
	entryV1.iIndex = index;
	
	TMockLtsyData2<RMobileSmsStore::TMobileGsmSmsEntryV1, TInt> 
		expMockData2(entryV1, entryV1.iIndex);

	expMockData2.SerialiseL(expData);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileSmsStore::Write
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreWrite, expData);

	store.Write(requestStatus, pckgEntryV1);

    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(5, this); // expData, completeData, this
	                                      // store, messaging

	}

void CCTsySmsStoreFU::AuxDelete1L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	//-------------------------------------------------------------------------

	TInt index = 1;

	TMockLtsyData1<TInt> expMockData1(index);
	expMockData1.SerialiseL(expData);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreDelete, expData, KErrNotSupported);

    store.Delete(requestStatus, index);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreDelete, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreDelete, KErrGeneral);

    store.Delete(requestStatus, index);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::Delete when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreDelete, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreDelete, KErrNone);
	
	store.Delete(requestStatus, index);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileSmsStore::Delete
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreDelete, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CleanupStack::PopAndDestroy(4, this); // expData, this, messaging, store
	}

void CCTsySmsStoreFU::AuxDelete2L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	//-------------------------------------------------------------------------

	TInt index = 1;

	TMockLtsyData1<TInt> expMockData1(index);
	expMockData1.SerialiseL(expData);

	//-------------------------------------------------------------------------
	// Test cancelling of RMobileSmsStore::Delete
	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.ExpectL(EMobilePhoneStoreDelete, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreDelete, KErrNone);
	
	store.Delete(requestStatus, index);
	store.CancelAsyncRequest(EMobilePhoneStoreDelete);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expData, this, messaging, store

	}

void CCTsySmsStoreFU::AuxDelete4L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2; 
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    // open the message store1
    RMobileSmsMessaging messaging1;
    TInt res = messaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging1);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store1;
    res = store1.Open(messaging1, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store1);

    // open the message store2
    RMobileSmsMessaging messaging2;
    res = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging2);

    RMobileSmsStore store2;
    res = store2.Open(messaging2, KETelIccSmsStore);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store2);

	RBuf8 expData;
	CleanupClosePushL(expData);

	//-------------------------------------------------------------------------

	TInt index1 = 1;
	TInt index2 = 2;

	TMockLtsyData1<TInt> expMockData11(index1);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileSmsStore::Delete
 	//-------------------------------------------------------------------------

	expMockData11.SerialiseL(expData);
	iMockLTSY.ExpectL(EMobilePhoneStoreDelete, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreDelete, KErrNone, 10);
	store1.Delete(requestStatus1, index1);

	store2.Delete(requestStatus2, index2);

	User::WaitForRequest(requestStatus1);        
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

	User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); // phone2, telServer2, expData, this
	                                      // store1, messaging1, store2, messaging2

	}

void CCTsySmsStoreFU::AuxDelete5L(const TDesC& aName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 expData;
	CleanupClosePushL(expData);

	//-------------------------------------------------------------------------

	TInt index = 1;

	TMockLtsyData1<TInt> expMockData1(index);
	expMockData1.SerialiseL(expData);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileSmsStore::Delete
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreDelete, expData);

    store.Delete(requestStatus, index);
    
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(4, this); // expData, this, messaging, store

	}

void CCTsySmsStoreFU::AuxDeleteAll1L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll, KErrNotSupported);

    store.DeleteAll(requestStatus);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll);
	iMockLTSY.CompleteL(EMobilePhoneStoreDeleteAll, KErrGeneral);

    store.DeleteAll(requestStatus);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::DeleteAll when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll);
	iMockLTSY.CompleteL(EMobilePhoneStoreDeleteAll, KErrNone);
	
	store.DeleteAll(requestStatus);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileSmsStore::DeleteAll
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreDeleteAll, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // this, messaging, store

	}

void CCTsySmsStoreFU::AuxDeleteAll2L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileSmsStore::DeleteAll
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll);
	iMockLTSY.CompleteL(EMobilePhoneStoreDeleteAll, KErrNone);
	
    store.DeleteAll(requestStatus);
    store.CancelAsyncRequest(EMobilePhoneStoreDeleteAll);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // this, messaging, store

	}

void CCTsySmsStoreFU::AuxDeleteAll4L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2; 
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    // open the message store1
    RMobileSmsMessaging messaging1;
    TInt res = messaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging1);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store1;
    res = store1.Open(messaging1, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store1);

    // open the message store2
    RMobileSmsMessaging messaging2;
    res = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging2);

    RMobileSmsStore store2;
    res = store2.Open(messaging2, KETelIccSmsStore);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileSmsStore::DeleteAll
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll);
	iMockLTSY.CompleteL(EMobilePhoneStoreDeleteAll, KErrNone, 10);
    store1.DeleteAll(requestStatus1);

    store2.DeleteAll(requestStatus2);
    
    User::WaitForRequest(requestStatus1);        
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

    User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, this
	                                      // store1, messaging1, store2, messaging2

	}

void CCTsySmsStoreFU::AuxDeleteAll5L(const TDesC& aName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileSmsStore::DeleteAll
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll);

    store.DeleteAll(requestStatus);
    
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(3, this); // this, messaging, store

	}

void CCTsySmsStoreFU::AuxGetInfo1L(const TDesC& aName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	//-------------------------------------------------------------------------

	TInt maxNumLength(-1);
	TInt maxTextLength(-1);
	RMobilePhoneBookStore::TMobilePhoneBookLocation location(RMobilePhoneBookStore::ELocationUnknown);
	TUint16 changeCounter(0);
	RMobilePhoneBookStore::TMobilePhoneBookIdentity identity; 
	RMobilePhoneBookStore::TMobilePhoneStoreType type(RMobilePhoneBookStore::EShortMessageStore);
	TUint8 totalEntries(1);
	TInt usedEntries(2);
	TUint32 caps(KSmsStoreInfoCaps);
	RMobilePhone::TMobileName name(KETelIccSmsStore);

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV1;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV1(phoneBookInfoV1);

    TMockLtsyData2<TUint8, TInt> data(totalEntries, usedEntries);
    data.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo, KErrNotSupported);

	store.GetInfo(requestStatus, pckgInfoV1);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::GetInfoL().
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrGeneral, completeData);

	store.GetInfo(requestStatus, pckgInfoV1);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::GetInfo when result is not cached.
 	//-------------------------------------------------------------------------

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::GetInfoL().
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	
	store.GetInfo(requestStatus, pckgInfoV1);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	ASSERT_EQUALS(maxNumLength , phoneBookInfoV1.iMaxNumLength  );
	ASSERT_EQUALS(maxTextLength, phoneBookInfoV1.iMaxTextLength );
	ASSERT_EQUALS(location     , phoneBookInfoV1.iLocation      );
	ASSERT_EQUALS(changeCounter, phoneBookInfoV1.iChangeCounter );
	ASSERT_EQUALS(0, phoneBookInfoV1.iIdentity.Compare(identity));
	ASSERT_EQUALS(type         , phoneBookInfoV1.iType          );
	ASSERT_EQUALS(totalEntries , static_cast<TUint8>(phoneBookInfoV1.iTotalEntries));
	ASSERT_EQUALS(usedEntries  , phoneBookInfoV1.iUsedEntries   );
	ASSERT_EQUALS(caps, phoneBookInfoV1.iCaps);
	ASSERT_EQUALS(0, phoneBookInfoV1.iName.Compare(name));
	
 	//-------------------------------------------------------------------------
	// TEST D: RMobileSmsStore::GetInfo again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	usedEntries = 0;
	totalEntries = 0;
	
	completeData.Close();
	
    TMockLtsyData2<TUint8, TInt> data2(totalEntries, usedEntries);
    data2.SerialiseL(completeData);
	
	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::GetInfoL().
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	
	store.GetInfo(requestStatus, pckgInfoV1);
    
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	ASSERT_EQUALS(maxNumLength , phoneBookInfoV1.iMaxNumLength  );
	ASSERT_EQUALS(maxTextLength, phoneBookInfoV1.iMaxTextLength );
	ASSERT_EQUALS(location     , phoneBookInfoV1.iLocation      );
	ASSERT_EQUALS(changeCounter, phoneBookInfoV1.iChangeCounter );
	ASSERT_EQUALS(0, phoneBookInfoV1.iIdentity.Compare(identity));
	ASSERT_EQUALS(type         , phoneBookInfoV1.iType          );
	ASSERT_EQUALS(totalEntries , static_cast<TUint8>(phoneBookInfoV1.iTotalEntries));
	ASSERT_EQUALS(usedEntries  , phoneBookInfoV1.iUsedEntries   );
	ASSERT_EQUALS(caps, phoneBookInfoV1.iCaps);
	ASSERT_EQUALS(0, phoneBookInfoV1.iName.Compare(name));

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileSmsStore::GetInfo
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST F: Successful completion request of RMobileSmsStore::GetInfo
	// with RMobilePhoneStore::TMobilePhoneStoreInfoV1
	//-------------------------------------------------------------------------
	
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfoV1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg( storeInfoV1 );
	
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	    
	store.GetInfo(requestStatus, storeInfoPckg);
	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	    
	ASSERT_EQUALS(KErrNone, requestStatus.Int());   
	
	ASSERT_EQUALS(type         , storeInfoV1.iType          );
	ASSERT_EQUALS(totalEntries , static_cast<TUint8>(storeInfoV1.iTotalEntries));
	ASSERT_EQUALS(usedEntries  , storeInfoV1.iUsedEntries   );
	ASSERT_EQUALS(caps, storeInfoV1.iCaps);
	ASSERT_EQUALS(0, storeInfoV1.iName.Compare(name));
	   
	CleanupStack::PopAndDestroy(4, this); // this, messaging, store, completeData
	
	}

void CCTsySmsStoreFU::AuxGetInfo2L(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------
	
	TUint8 totalEntries(0);
	TInt usedEntries(0);
	
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV1;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV1(phoneBookInfoV1);

    TMockLtsyData2<TUint8, TInt> data(totalEntries, usedEntries);
    data.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileSmsStore::GetInfo
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::GetInfoL().
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	
	store.GetInfo(requestStatus, pckgInfoV1);
    store.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // completeData, this, messaging, store
		
	}

void CCTsySmsStoreFU::AuxGetInfo3L(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------
	
	TUint8 totalEntries(0);
	TInt usedEntries(0);
	
    TMockLtsyData2<TUint8, TInt> data(totalEntries, usedEntries);
    data.SerialiseL(completeData);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileSmsStore::GetInfo
 	//-------------------------------------------------------------------------
	TBuf8<1> badParam;
	store.GetInfo(requestStatus, badParam);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	
	
	CleanupStack::PopAndDestroy(4, this); // completeData, this, messaging, store
		
	}

void CCTsySmsStoreFU::AuxGetInfo4L(const TDesC& aName)
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2; 
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    // open the message store1
    RMobileSmsMessaging messaging1;
    TInt res = messaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging1);

    RMobileSmsStore store1;
    res = store1.Open(messaging1, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store1);

    // open the message store2
    RMobileSmsMessaging messaging2;
    res = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging2);

    RMobileSmsStore store2;
    res = store2.Open(messaging2, KETelIccSmsStore);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store2);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

 	//-------------------------------------------------------------------------
	// It is common value for both function.

	RMobilePhoneBookStore::TMobilePhoneBookIdentity identity; 
	
 	//-------------------------------------------------------------------------
	// param for first function
	
	TUint8 totalEntries1(1);
	TInt   usedEntries1(2);

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV11;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV11(phoneBookInfoV11);

    TMockLtsyData2<TUint8, TInt> data1(totalEntries1, usedEntries1);

 	//-------------------------------------------------------------------------
	// param for second function
	
	TUint8 totalEntries2(10);
	TInt   usedEntries2(20);

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV12;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV12(phoneBookInfoV12);

    TMockLtsyData2<TUint8, TInt> data2(totalEntries2, usedEntries2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileSmsStore::GetInfo
 	//-------------------------------------------------------------------------

	data1.SerialiseL(completeData);
	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::GetInfoL().
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);

    store1.GetInfo(requestStatus1, pckgInfoV11);

	completeData.Close();
	data2.SerialiseL(completeData);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);

    store2.GetInfo(requestStatus2, pckgInfoV12);
    
    User::WaitForRequest(requestStatus1);        
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

    User::WaitForRequest(requestStatus2);        
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

	CleanupStack::PopAndDestroy(8, this); // phone2, telServer2, this, completeData
	                                      // store1, messaging1, store2, messaging2

	}

void CCTsySmsStoreFU::AuxGetInfo5L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	//-------------------------------------------------------------------------

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfoV1;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckgInfoV1(phoneBookInfoV1);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileSmsStore::GetInfo
 	//-------------------------------------------------------------------------

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::GetInfoL().
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

	store.GetInfo(requestStatus, pckgInfoV1);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(3, this); // this, messaging, store

	}	

void CCTsySmsStoreFU::AuxStart1L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	RMobileSmsStore::TMobileSmsEntryExtensionId type = RMobileSmsStore::KETelMobileGsmSmsEntryV1;
	CRetrieveMobilePhoneSmsList* retrieveMobilePhoneSmsList = 
		CRetrieveMobilePhoneSmsList::NewL(store, type);
	CleanupStack::PushL(retrieveMobilePhoneSmsList);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*retrieveMobilePhoneSmsList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

 	//-------------------------------------------------------------------------

	CArrayPtrFlat<TSmsMsg>* smsMsgArray = new ( ELeave ) CArrayPtrFlat<TSmsMsg>( 1 );
	CleanupStack::PushL(smsMsgArray);

	TSmsMsg smsMsg;
    smsMsg.iSmsMsg = _L8("Hello, World!"); 
    smsMsg.iServiceCentre = _L("ServiceSenter"); 
    smsMsg.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
    smsMsg.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
    smsMsg.iMessageStatus = KSimSmsMtRead; // RMobileSmsStore::EStoredMessageRead
    smsMsg.iLocation      = 200;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsClass2            = ETrue;
	smsMsgArray->AppendL(&smsMsg, 1);
    
	TBool receivedClass2ToBeReSent(ETrue);
	TMockLtsyData2<CArrayPtrFlat< TSmsMsg >*, TBool> 
		completeSmsMsgLtsyData(smsMsgArray, receivedClass2ToBeReSent);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CRetrieveMobilePhoneSmsList::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	completeSmsMsgLtsyData.SerialiseL(completeData);
    iMockLTSY.CompleteL(EMobilePhoneStoreReadAllPhase1, KErrNone, completeData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TUint8 totalEntries(1);
	TInt usedEntries(2);

	TMockLtsyData2<TUint8, TInt> dataGetInfo(totalEntries, usedEntries);
	completeData.Close();
	dataGetInfo.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	iMockLTSY.ExpectL(EMobilePhoneStoreReadAllPhase1, KErrNotSupported);

	retrieveMobilePhoneSmsList->Start(activeRetriever->Status());
	activeRetriever->Activate();

	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreReadAllPhase1, KErrNone);
	completeData.Close();
	completeSmsMsgLtsyData.SerialiseL(completeData);
	iMockLTSY.CompleteL(EMobilePhoneStoreReadAllPhase1, KErrGeneral, completeData);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteReadAllSmsPhase1().
	 */
	iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

    retrieveMobilePhoneSmsList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrGeneral, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneSmsList::Start when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreReadAllPhase1, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneStoreReadAllPhase1, KErrNone, completeData);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteReadAllSmsPhase1().
	 */
	iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

    retrieveMobilePhoneSmsList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3); // retrieveMobilePhoneSmsList, 
	                                // activeRetriever, smsMsgArray
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(4, this); // this, messaging, store, completeData

	}	

void CCTsySmsStoreFU::AuxStart2L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	RMobileSmsStore::TMobileSmsEntryExtensionId type = RMobileSmsStore::KETelMobileGsmSmsEntryV1;
	CRetrieveMobilePhoneSmsList* retrieveMobilePhoneSmsList = 
		CRetrieveMobilePhoneSmsList::NewL(store, type);
	CleanupStack::PushL(retrieveMobilePhoneSmsList);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*retrieveMobilePhoneSmsList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
	
 	//-------------------------------------------------------------------------

	CArrayPtrFlat<TSmsMsg>* smsMsgArray = new ( ELeave ) CArrayPtrFlat<TSmsMsg>( 1 );
	CleanupStack::PushL(smsMsgArray);

	TSmsMsg smsMsg;
    smsMsg.iSmsMsg = _L8("Hello, World!"); 
    smsMsg.iServiceCentre = _L("ServiceSenter"); 
    smsMsg.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
    smsMsg.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
    smsMsg.iMessageStatus = KSimSmsMtRead; // RMobileSmsStore::EStoredMessageRead
    smsMsg.iLocation      = 200;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsClass2            = ETrue;
	smsMsgArray->AppendL(&smsMsg, 1);
    
	TBool receivedClass2ToBeReSent(ETrue);
	TMockLtsyData2<CArrayPtrFlat< TSmsMsg >*, TBool> 
		completeSmsMsgLtsyData(smsMsgArray, receivedClass2ToBeReSent);

	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneSmsList::Start
 	//-------------------------------------------------------------------------

	TUint8 totalEntries(1);
	TInt usedEntries(2);

	TMockLtsyData2<TUint8, TInt> dataGetInfo(totalEntries, usedEntries);
	dataGetInfo.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.ExpectL(EMobilePhoneStoreReadAllPhase1, KErrNone);
	completeData.Close();
	completeSmsMsgLtsyData.SerialiseL(completeData);
    iMockLTSY.CompleteL(EMobilePhoneStoreReadAllPhase1, KErrNone, completeData, 10);

	retrieveMobilePhoneSmsList->Start(activeRetriever->Status());
	activeRetriever->Activate(CActiveRetriever::ECaseGeneralCancelCase);

	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

    iMockLTSY.ExpectL(EMobilePhoneStoreReadAllPhase1, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneStoreReadAllPhase1, KErrNone, completeData);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteReadAllSmsPhase1().
	 */
    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

    retrieveMobilePhoneSmsList->Start(activeRetriever->Status());
	activeRetriever->Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3); // retrieveMobilePhoneSmsList, 
	                                // activeRetriever, smsMsgArray
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(4, this); // this, messaging, store, completeData

	}

void CCTsySmsStoreFU::AuxStart4L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2; 
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    // open the message store1
    RMobileSmsMessaging messaging1;
    TInt res = messaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging1);

    RMobileSmsStore store1;
    res = store1.Open(messaging1, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store1);

    // open the message store2
    RMobileSmsMessaging messaging2;
    res = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging2);

    RMobileSmsStore store2;
    res = store2.Open(messaging2, KETelIccSmsStore);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store2);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	RMobileSmsStore::TMobileSmsEntryExtensionId type = RMobileSmsStore::KETelMobileGsmSmsEntryV1;
	
	CRetrieveMobilePhoneSmsList* retrieveMobilePhoneSmsList1 = 
		CRetrieveMobilePhoneSmsList::NewL(store1, type);
	CleanupStack::PushL(retrieveMobilePhoneSmsList1);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever1 = 
						CActiveRetriever::NewL(*retrieveMobilePhoneSmsList1);
	CleanupStack::PushL(activeRetriever1);
	scheduler.AddRetrieverL(*activeRetriever1);

	CRetrieveMobilePhoneSmsList* retrieveMobilePhoneSmsList2 = 
		CRetrieveMobilePhoneSmsList::NewL(store2, type);
	CleanupStack::PushL(retrieveMobilePhoneSmsList2);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever2 = 
						CActiveRetriever::NewL(*retrieveMobilePhoneSmsList2);
	CleanupStack::PushL(activeRetriever2);
	scheduler.AddRetrieverL(*activeRetriever2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting CRetrieveMobilePhoneSmsList::Start
 	//-------------------------------------------------------------------------

	TUint8 totalEntries(1);
	TInt usedEntries(2);

	TMockLtsyData2<TUint8, TInt> dataGetInfo(totalEntries, usedEntries);
	dataGetInfo.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

 	//-------------------------------------------------------------------------

	CArrayPtrFlat<TSmsMsg>* smsMsgArray = new ( ELeave ) CArrayPtrFlat<TSmsMsg>( 1 );
	CleanupStack::PushL(smsMsgArray);

	TSmsMsg smsMsg;
    smsMsg.iSmsMsg = _L8("Hello, World!"); 
    smsMsg.iServiceCentre = _L("ServiceSenter"); 
    smsMsg.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
    smsMsg.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
    smsMsg.iMessageStatus = KSimSmsMtRead; // RMobileSmsStore::EStoredMessageRead
    smsMsg.iLocation      = 200;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsClass2            = ETrue;
	smsMsgArray->AppendL(&smsMsg, 1);
    
	TBool receivedClass2ToBeReSent(ETrue);
	TMockLtsyData2<CArrayPtrFlat< TSmsMsg >*, TBool> 
		completeSmsMsgLtsyData(smsMsgArray, receivedClass2ToBeReSent);
	
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreReadAllPhase1, KErrNone);
	completeData.Close();
	completeSmsMsgLtsyData.SerialiseL(completeData);
    iMockLTSY.CompleteL(EMobilePhoneStoreReadAllPhase1, KErrNone, completeData);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteReadAllSmsPhase1().
	 */
    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

	retrieveMobilePhoneSmsList1->Start(activeRetriever1->Status());
	activeRetriever1->Activate();

	retrieveMobilePhoneSmsList2->Start(activeRetriever2->Status());
	activeRetriever2->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever1->iStatus.Int());
	ASSERT_EQUALS(KErrServerBusy, activeRetriever2->iStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(5); // retrieveMobilePhoneSmsList1, 
									// retrieveMobilePhoneSmsList2,
									// activeRetriever1,
									// activeRetriever2, smsMsgArray
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(8, this); // phone2, telServer2, this
	                                      // store1, messaging1, store2, messaging2, completeData
	
	}

void CCTsySmsStoreFU::AuxStart5L(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	RMobileSmsStore::TMobileSmsEntryExtensionId type = RMobileSmsStore::KETelMobileGsmSmsEntryV1;
	CRetrieveMobilePhoneSmsList* retrieveMobilePhoneSmsList = 
		CRetrieveMobilePhoneSmsList::NewL(store, type);
	CleanupStack::PushL(retrieveMobilePhoneSmsList);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*retrieveMobilePhoneSmsList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of CRetrieveMobilePhoneSmsList::Start
 	//-------------------------------------------------------------------------

	TUint8 totalEntries(1);
	TInt usedEntries(2);

	TMockLtsyData2<TUint8, TInt> dataGetInfo(totalEntries, usedEntries);
	dataGetInfo.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStoreReadAllPhase1, KErrNone);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteReadAllSmsPhase1().
	 */
    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

	ASSERT_TRUE(EFalse);

	retrieveMobilePhoneSmsList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrTimedOut, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // retrieveMobilePhoneSmsList, 
	                                // activeRetriever,
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(3, this); // this, messaging, store
	
	}

void CCTsySmsStoreFU::AuxNotifyStoreEvent1L(const TDesC& aName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
 	//-------------------------------------------------------------------------
	
	TInt index (0);
	TUint32 storeEvent(0);

	RMobileSmsStore::TMobileGsmSmsEntryV1 entryV1;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(entryV1);
	
	entryV1.iServiceCentre.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	entryV1.iServiceCentre.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	entryV1.iServiceCentre.iTelNumber    = _L("TelNumber1");
	entryV1.iMsgData = _L8("Hello, World!");
	entryV1.iMsgStatus = RMobileSmsStore::EStoredMessageUnsent;
	entryV1.iIndex = index;
	
	TMockLtsyData2<RMobileSmsStore::TMobileGsmSmsEntryV1, TInt> 
		expMockData2(entryV1, entryV1.iIndex);

    TUint8 location = 255;
	TBool  receivedClass2ToBeResent = ETrue;

	TMockLtsyData2<TUint8, TBool> completeMockData2(location, receivedClass2ToBeResent);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileSmsStore::NotifyStoreEvent when result is not cached.
 	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Complete for Delete with RMobilePhoneStore::KStoreEntryDeleted
 	//-------------------------------------------------------------------------

	TMockLtsyData1<TInt> expMockData1(index);
	
	expMockData1.SerialiseL(expData);
	
	store.NotifyStoreEvent(requestStatus, storeEvent, index);

	iMockLTSY.ExpectL(EMobilePhoneStoreDelete, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreDelete, KErrNone);
	
	store.Delete(requestStatus2, index);

	User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    
    TUint32 event = RMobilePhoneStore::KStoreEntryDeleted;
    ASSERT_EQUALS(event, storeEvent);	
    ASSERT_EQUALS(0    , index     );	
	
	//-------------------------------------------------------------------------
	// Complete for Write with RMobilePhoneStore::KStoreEntryAdded
 	//-------------------------------------------------------------------------
	
    index = 0;
	store.NotifyStoreEvent(requestStatus, storeEvent, index);

	expData.Close();
	expMockData2.SerialiseL(expData);	
	iMockLTSY.ExpectL(EMobilePhoneStoreWrite, expData);

	completeMockData2.SerialiseL(completeData);
	iMockLTSY.CompleteL(EMobilePhoneStoreWrite, KErrNone, completeData);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteWriteSms().
	 */
	iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

	store.Write(requestStatus2, pckgEntryV1); 
	
	User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    
    event = RMobilePhoneStore::KStoreEntryAdded | RMobilePhoneStore::KStoreFull;
    ASSERT_EQUALS(event                      , storeEvent);	
    ASSERT_EQUALS(static_cast<TInt>(location), index     );	
	
	//-------------------------------------------------------------------------
	// Complete for Write with RMobilePhoneStore::KStoreFull
 	//-------------------------------------------------------------------------

    index = 0;
	entryV1.iIndex = index;
	TMockLtsyData2<RMobileSmsStore::TMobileGsmSmsEntryV1, TInt> 
		expMockData2b(entryV1, entryV1.iIndex);

	expData.Close();
	expMockData2b.SerialiseL(expData);

	store.NotifyStoreEvent(requestStatus, storeEvent, index);

	iMockLTSY.ExpectL(EMobilePhoneStoreWrite, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreWrite, KErrDiskFull, completeData);

	/* 
	 * Method HandleRequestL() with EMobileSmsMessagingResumeSmsReception IPC
	 * is called from CMmSmsStorageTsy::CompleteWriteSms().
	 */
	iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

	store.Write(requestStatus2, pckgEntryV1); 
	
	User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrDiskFull, requestStatus2.Int());
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    
    event = RMobilePhoneStore::KStoreFull;
    ASSERT_EQUALS(event, storeEvent);	
    ASSERT_EQUALS(-1   , index     );	

	//-------------------------------------------------------------------------
	// Complete for DeleteAllSns with RMobilePhoneStore::KStoreEmpty
 	//-------------------------------------------------------------------------

	store.NotifyStoreEvent(requestStatus, storeEvent, index);

	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll);
	iMockLTSY.CompleteL(EMobilePhoneStoreDeleteAll, KErrNone);
	
	store.DeleteAll(requestStatus2);

	User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    
    event = RMobilePhoneStore::KStoreEmpty;
    ASSERT_EQUALS(event, storeEvent);	
    ASSERT_EQUALS(-1   , index     );	

    //-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	TUint8 totalEntries(1);
	TInt usedEntries(1);

	TMockLtsyData2<TUint8, TInt> dataGetInfo(totalEntries, usedEntries);
	completeData.Close();
	dataGetInfo.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneStoreGetInfo, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	expData.Close();
	expMockData1.SerialiseL(expData);
	
	store.NotifyStoreEvent(requestStatus, storeEvent, index);

	iMockLTSY.ExpectL(EMobilePhoneStoreDelete, expData);
	iMockLTSY.CompleteL(EMobilePhoneStoreDelete, KErrNone);
	
	store.Delete(requestStatus2, index);

	User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    
    event = RMobilePhoneStore::KStoreEntryDeleted | RMobilePhoneStore::KStoreEmpty;
    ASSERT_EQUALS(event, storeEvent);	
    ASSERT_EQUALS(-1    , index     );	

    //-------------------------------------------------------------------------
	// for coverage increasing
	//-------------------------------------------------------------------------

    TBool smsInd( EFalse );

    TSmsMsg smsMsg;
	smsMsg.iSmsMsg = _L8("Hello, World!"); 
	smsMsg.iServiceCentre = _L("ServiceSenter1"); 
	smsMsg.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
	smsMsg.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
	smsMsg.iMessageStatus = KSimSmsMtRead; 
	smsMsg.iLocation      = 200;
	smsMsg.iDeleteAfterClientAck = EFalse;
	smsMsg.iSmsClass2            = ETrue;
    TSmsMsg* smsMsgPtr(&smsMsg);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(smsInd, smsMsgPtr);
	completeData.Close();
	compTsyData.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, completeData);
	
    TDesC8* msgPtr = NULL;
    TInt rpCause(KErrGsmSMSUnspecifiedProtocolError);
    TMockLtsyData2<TDesC8*, TInt> expTsyData(msgPtr, rpCause);

    expData.Close();
    expTsyData.SerialiseL(expData);
    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, expData);

    User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(5, this); // expData, completeData, this, messaging, store

	}

void CCTsySmsStoreFU::AuxNotifyStoreEvent2L(const TDesC& aName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

 	//-------------------------------------------------------------------------

	TInt index (0);
	TUint32 storeEvent(0);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileSmsStore::NotifyStoreEvent
 	//-------------------------------------------------------------------------

	store.NotifyStoreEvent(requestStatus, storeEvent, index);
	store.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);

	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll);
	iMockLTSY.CompleteL(EMobilePhoneStoreDeleteAll, KErrNone);
	
	store.DeleteAll(requestStatus2);

	User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // this, messaging, store
	}

void CCTsySmsStoreFU::AuxNotifyStoreEvent4L(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus requestStatus11;
	TRequestStatus requestStatus21;
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2; 
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    // open the message store1
    RMobileSmsMessaging messaging1;
    TInt res = messaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging1);

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    RMobileSmsStore store1;
    res = store1.Open(messaging1, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store1);

    // open the message store2
    RMobileSmsMessaging messaging2;
    res = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging2);

    RMobileSmsStore store2;
    res = store2.Open(messaging2, KETelIccSmsStore);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store2);

 	//-------------------------------------------------------------------------

	TInt index1 (0);
	TInt index2 (0);
	TUint32 storeEvent1(0);
	TUint32 storeEvent2(0);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileSmsStore::NotifyStoreEvent
 	//-------------------------------------------------------------------------

	store1.NotifyStoreEvent(requestStatus1, storeEvent1, index1);
	store2.NotifyStoreEvent(requestStatus2, storeEvent2, index2);

	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll);
	iMockLTSY.CompleteL(EMobilePhoneStoreDeleteAll, KErrNone);
	
	store1.DeleteAll(requestStatus11);

    User::WaitForRequest(requestStatus11);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus11.Int());

	iMockLTSY.ExpectL(EMobilePhoneStoreDeleteAll);
	iMockLTSY.CompleteL(EMobilePhoneStoreDeleteAll, KErrNone);
	
	store1.DeleteAll(requestStatus21);

    User::WaitForRequest(requestStatus21);
    ASSERT_EQUALS(KErrNone, requestStatus21.Int());
    
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    TUint32 event = RMobilePhoneStore::KStoreEmpty;

    ASSERT_EQUALS(event, storeEvent1);
    ASSERT_EQUALS(-1   , index1     );

    ASSERT_EQUALS(event, storeEvent2);
    ASSERT_EQUALS(-1   , index2     );
    
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, this,
	                                      // store1, messaging1, store2, messaging2
	
	}

void CCTsySmsStoreFU::AuxStart1bL(const TDesC& aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	/*
	 * HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	 * when RMobileSmsStore's object is opened.
	 */
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

    // open the message store
    RMobileSmsMessaging messaging;
    TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

    RMobileSmsStore store;
    res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	RMobileSmsStore::TMobileSmsEntryExtensionId type = RMobileSmsStore::KETelMobileGsmSmsEntryV1;
	CRetrieveMobilePhoneSmsList* retrieveMobilePhoneSmsList = 
		CRetrieveMobilePhoneSmsList::NewL(store, type);
	CleanupStack::PushL(retrieveMobilePhoneSmsList);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*retrieveMobilePhoneSmsList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

 	//-------------------------------------------------------------------------

	CArrayPtrFlat<TSmsMsg>* smsMsgArray = new ( ELeave ) CArrayPtrFlat<TSmsMsg>( 1 );
	CleanupStack::PushL(smsMsgArray);

	TSmsMsg smsMsg;
    smsMsg.iSmsMsg = _L8("Hello, World!"); 
    smsMsg.iServiceCentre = _L("ServiceSenter"); 
    smsMsg.iMobileScTON   = RMobilePhone::ENetworkSpecificNumber;  
    smsMsg.iMobileScNPI   = RMobilePhone::EServiceCentreSpecificPlan2;
    smsMsg.iMessageStatus = KSimSmsMtRead; // RMobileSmsStore::EStoredMessageRead
    smsMsg.iLocation      = 200;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsClass2            = ETrue;
	smsMsgArray->AppendL(&smsMsg, 1);
    
	TBool receivedClass2ToBeReSent(ETrue);
	TMockLtsyData2<CArrayPtrFlat< TSmsMsg >*, TBool> 
		completeSmsMsgLtsyData(smsMsgArray, receivedClass2ToBeReSent);

 	//-------------------------------------------------------------------------
	// TEST C: Failure of CRetrieveMobilePhoneSmsList::Start when result 
	// is not cached (or ready).
 	//-------------------------------------------------------------------------

	retrieveMobilePhoneSmsList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	// Since the cache is not ready, NO SMS is found.
	ASSERT_EQUALS(KErrNotFound, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3); // retrieveMobilePhoneSmsList, 
	                                // activeRetriever, smsMsgArray
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(4, this); // this, messaging, store, completeData

	}	

void CCTsySmsStoreFU::AuxStartBatch1L(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// HandleRequestL() with EMobilePhoneStoreGetInfo IPC is called from CMmSmsStorageTsy::Init() 
	// when RMobileSmsStore's object is opened.
	iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo);

	// open the message store
	RMobileSmsMessaging messaging;
	TInt res = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(messaging);

	RMobileSmsStore store;
	res = store.Open(messaging, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(store);

	CArrayPtrFlat<TSmsMsg>* smsMsgArray = new ( ELeave ) CArrayPtrFlat<TSmsMsg>( 1 );
	CleanupStack::PushL(smsMsgArray);

	TSmsMsg smsMsg;
	smsMsgArray->AppendL(&smsMsg, 1);
		    
	TBool receivedClass2ToBeReSent(ETrue);
	TMockLtsyData2<CArrayPtrFlat< TSmsMsg >*, TBool> 
			completeSmsMsgLtsyData(smsMsgArray, receivedClass2ToBeReSent);
	
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	RMobileSmsStore::TMobileSmsEntryExtensionId type = RMobileSmsStore::KETelMobileGsmSmsEntryV1;
	
	CRetrieveMobilePhoneSmsList* retrieveMobilePhoneSmsList = CRetrieveMobilePhoneSmsList::NewL(store, type);
	CleanupStack::PushL(retrieveMobilePhoneSmsList);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*retrieveMobilePhoneSmsList);
	CleanupStack::PushL(activeRetriever);
		
	scheduler.AddRetrieverL(*activeRetriever);
	
	TRequestStatus requestStatus;
	retrieveMobilePhoneSmsList->StartBatch(activeRetriever->Status(),1,1);
	
	activeRetriever->Activate();
	scheduler.StartScheduler();
	
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(6, this); // this, messaging, store, smsMsgArray, retrieveMobilePhoneSmsList, activeRetriever
		
	}
