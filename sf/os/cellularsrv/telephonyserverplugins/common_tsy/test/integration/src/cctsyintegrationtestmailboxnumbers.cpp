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
// Test step definitions for the MailboxNumbers functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestmailboxnumbers.h"



	
CCTSYIntegrationTestMailboxNumbersBase::CCTSYIntegrationTestMailboxNumbersBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestMailboxNumbersBase::~CCTSYIntegrationTestMailboxNumbersBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestMailboxNumbers0001::CCTSYIntegrationTestMailboxNumbers0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMailboxNumbersBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMailboxNumbers0001::GetTestStepName());
	}

CCTSYIntegrationTestMailboxNumbers0001::~CCTSYIntegrationTestMailboxNumbers0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMailboxNumbers0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MBXN-0001
 * @SYMFssID BA/CTSY/MBXN-0001
 * @SYMTestCaseDesc Get the voice mailbox number
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Valid voice mailbox number returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify voice mailbox number is returned.
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
	
	// Check RMobilePhone::GetMailboxNumbers returns TMobilePhoneVoicemailIdsV3::iVoice >= 0
	RMobilePhone::TMobilePhoneVoicemailIdsV3 mailboxNumbers;
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg mailboxNumbersPckg(mailboxNumbers);
	// Before retrieving into TMobilePhoneVoicemailIdsV3::iVoice, set its value to 111 and then test
	// it has been changed by CTSY. It may be returning as 0.
	const TUint8 KInvalidMxNum = 111;
	mailboxNumbers.iVoice = KInvalidMxNum;
	
	TExtEtelRequestStatus getMailboxNumbersStatus(phone, EMobilePhoneGetMailboxNumbers);
	CleanupStack::PushL(getMailboxNumbersStatus);
	phone.GetMailboxNumbers(getMailboxNumbersStatus, mailboxNumbersPckg);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getMailboxNumbersStatus, ETimeLong), KErrNone, 
			_L("RMobilePhone::GetMailboxNumbers timed-out"));	
	ASSERT_EQUALS(getMailboxNumbersStatus.Int(), KErrNone,			 
			_L("RMobilePhone::GetMailboxNumbers returned error status."));
	
	ASSERT_TRUE(mailboxNumbers.iVoice != KInvalidMxNum,
			_L("RMobilePhone::GetMailboxNumbers returned invalid mailbox number."));
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1, &getMailboxNumbersStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMailboxNumbers0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMailboxNumbers0001");
	}



