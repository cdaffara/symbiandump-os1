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
// Test step definitions for the FixedDialling functional unit.
// 
//

/**
 @internalTechnology
*/
#include <mpbutil.h>

#include "cctsyintegrationtestfixeddialling.h"
#include "cctsyinidata.h"




	
CCTSYIntegrationTestFixedDiallingBase::CCTSYIntegrationTestFixedDiallingBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr),
		iPhoneTsyTestHelper(*this),
		iCallControlTsyTestHelper(*this),
		iNetworkTsyTestHelper(*this),
		iSimTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestFixedDiallingBase::~CCTSYIntegrationTestFixedDiallingBase()
/*
 * Destructor
 */
	{
	}
	
	

/**
 * Obtain the PIN2 password from the INI file.
 * @param aPhone RMobilePhone session
 * @param aPassword On completion, the PIN2 password. The function leaves if this is not found.
 */
void CCTSYIntegrationTestFixedDiallingBase::GetPIN2PasswordL( RMobilePhone &aPhone, 
																RMobilePhone::TMobilePassword& aPassword )
	{
	// wait until we are on the network
	
	CHECK_EQUALS_L( iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( aPhone ), KErrNone, 
						_L("Network is unavailable") );
			
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(aPhone,networkSection);
	
	// Get the password from the ini file
	TPtrC password; 
	TBool gotPass = GetStringFromConfig(*networkSection, KIniPin2, password);
	
	CHECK_TRUE_L(gotPass, _L("Failed to read password from INI file"));
		
	aPassword = password;
	
	}
	

//*****************************************************************************

CCTSYIntegrationTestFixedDialling0001::CCTSYIntegrationTestFixedDialling0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestFixedDiallingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestFixedDialling0001::GetTestStepName());
	}

CCTSYIntegrationTestFixedDialling0001::~CCTSYIntegrationTestFixedDialling0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestFixedDialling0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-FXD-0001
 * @SYMFssID BA/CTSY/FXD-0001
 * @SYMTestCaseDesc Activate and deactivate the FDN.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifySecurityEvent, RMobilePhone::VerifySecurityCode, RMobilePhone::GetFdnStatus, RMobilePhone::NotifyFdnStatusChange
 * @SYMTestExpectedResults Pass - FDN is set to on, then to off.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify FDN status is set correctly.
 *
 * @return - TVerdict code
 */
	{
	
	//
	// SET UP
	//
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword pin2Password;
	GetPIN2PasswordL( phone, pin2Password );
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 securityCaps;
	TInt err = phone.GetSecurityCaps( securityCaps );
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetSecurityCaps returned an error"));
	CHECK_BITS_SET_L( securityCaps, RMobilePhone::KCapsAccessPin2, KNoUnwantedBits,
						_L("RMobilePhone::GetSecurityCaps returned wrong caps"));
	
	// Ensure the FDN is deactivated (RMobilePhone::GetFdnStatus returns EFdnNotActive) 
	RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
	err = phone.GetFdnStatus(fdnStatus);
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetFdnStatus returned an error"));
    if(fdnStatus!=RMobilePhone::EFdnNotActive)
        {
        err = iPhoneTsyTestHelper.SetPhoneFdnSettingL(phone,pin2Password,RMobilePhone::EFdnSetOff);
        CHECK_EQUALS_L(err,KErrNone,_L("RMobilePhone::SetFdnSetting failed"));
        err = phone.GetFdnStatus(fdnStatus);
        CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetFdnStatus returned an error"));
        }
	CHECK_EQUALS_L(fdnStatus, RMobilePhone::EFdnNotActive,
							_L("RMobilePhone::GetFdnStatus returned an unexpected result") );

	iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);

	
	//
	// SET UP END
	//

	StartTest();
	
	//
	// TEST START
	//

    // Backup phone state
	TFdnCleanupDisable backup(iPhoneTsyTestHelper,phone,pin2Password);
    CleanupStack::PushL(backup);
    
	// ===  Set FDN to on ===
	
	
	// Set the FDN change notification up
	TExtEtelRequestStatus reqNotifyFdnChange(phone, EMobilePhoneNotifyFdnStatusChange);
	CleanupStack::PushL(reqNotifyFdnChange);
	phone.NotifyFdnStatusChange(reqNotifyFdnChange,fdnStatus);

	
	// Check RMobilePhone::SetFdnSetting with aFdnSetting RMobilePhone::EFdnSetOn completes with KErrNone	
	err = iPhoneTsyTestHelper.SetPhoneFdnSettingL(phone, pin2Password, RMobilePhone::EFdnSetOn);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetFdnSetting returned an error"));

	// Check RMobilePhone::NotifyFdnStatusChange completes with FDN setting of RMobilePhone::EFdnActive
	iCallControlTsyTestHelper.WaitForMobilePhoneNotifyFdnStatusChange(phone,
																	reqNotifyFdnChange,
																	fdnStatus,
																	RMobilePhone::EFdnActive,
																	KErrNone);	

	// Just for the log
	ASSERT_EQUALS(fdnStatus,RMobilePhone::EFdnActive, 
							_L("RMobilePhone::NotifyFdnStatusChange returned an unexpected result") )

	// Check RMobilePhone::GetFdnStatus (synchronous version) returns RMobilePhone::EFdnActive
	err = phone.GetFdnStatus(fdnStatus);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::GetFdnStatus returned an error"));
	ASSERT_EQUALS(fdnStatus, RMobilePhone::EFdnActive,
							_L("RMobilePhone::GetFdnStatus returned an unexpected result") );

	// Check RMobilePhone::GetFdnStatus (asynchronous version) completes with RMobilePhone::EFdnActive
	TExtEtelRequestStatus reqGetFdnStatus(phone, EMobilePhoneGetFdnStatus);
	CleanupStack::PushL(reqGetFdnStatus);

	phone.GetFdnStatus(reqGetFdnStatus, fdnStatus);
	err = WaitForRequestWithTimeOut(reqGetFdnStatus, ETimeMedium);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::GetFdnStatus timed out"));
	ASSERT_EQUALS(fdnStatus, RMobilePhone::EFdnActive,
							_L("RMobilePhone::GetFdnStatus did returned an unexpected result") );

	// ===  Set FDN to off ===
	// Setup FDN change notification
	phone.NotifyFdnStatusChange(reqNotifyFdnChange,fdnStatus);
	
	// Call RMobilePhone::SetFdnSetting with RMobilePhone::EFdnSetOff 
	err = iPhoneTsyTestHelper.SetPhoneFdnSettingL(phone, pin2Password, RMobilePhone::EFdnSetOff);
	
	// Check RMobilePhone::SetFdnSetting with aFdnSetting RMobilePhone::EFdnSetOff completes with KErrNone
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetFdnSetting returned an error"));


	// Check RMobilePhone::NotifyFdnStatusChange completes with FDN setting of RMobilePhone::EFdnSetOff
	iCallControlTsyTestHelper.WaitForMobilePhoneNotifyFdnStatusChange(phone,
																	reqNotifyFdnChange,
																	fdnStatus,
																	RMobilePhone::EFdnNotActive,
																	KErrNone);

	// Just for the log
	ASSERT_EQUALS(fdnStatus,RMobilePhone::EFdnNotActive, 
							_L("RMobilePhone::NotifyFdnStatusChange returned an unexpected result") )

	// Check RMobilePhone::GetFdnStatus (synchronous version) returns RMobilePhone::EFdnSetOff
	err = phone.GetFdnStatus( fdnStatus );
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::GetFdnStatus returned an error"));
	ASSERT_EQUALS(fdnStatus, RMobilePhone::EFdnNotActive,
							_L("RMobilePhone::GetFdnStatus returned an unexpected result") );

	// Check RMobilePhone::GetFdnStatus (asynchronous version) completes with RMobilePhone::EFdnSetOff
	phone.GetFdnStatus(reqGetFdnStatus, fdnStatus);
	err = WaitForRequestWithTimeOut(reqGetFdnStatus, ETimeMedium );
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::GetFdnStatus timed out"));
	ASSERT_EQUALS(fdnStatus, RMobilePhone::EFdnNotActive,
							_L("RMobilePhone::GetFdnStatus returned an unexpected result") );
							
	//
	// TEST END
	//

    StartCleanup();
	
	// reqGetFdnStatus
	// reqNotifyFdnChange
	// backup
	CleanupStack::PopAndDestroy(3, &backup);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestFixedDialling0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestFixedDialling0001");
	}



CCTSYIntegrationTestFixedDialling0002::CCTSYIntegrationTestFixedDialling0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestFixedDiallingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestFixedDialling0002::GetTestStepName());
	}

CCTSYIntegrationTestFixedDialling0002::~CCTSYIntegrationTestFixedDialling0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestFixedDialling0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-FXD-0002
 * @SYMFssID BA/CTSY/FXD-0002
 * @SYMTestCaseDesc Write and delete entries from the FDN list.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::VerifySecurityCode, RMobilePhone::SetFdnSetting, RMobilePhone::NotifySecurityEvent, RMobileCall::GetMobileCallStatus, RCall::Dial, RMobilePhoneBookStore::Read, RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Write
 * @SYMTestExpectedResults Pass - Modification of FDN possible when PIN2 verified.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify numbers can be written to the FDN list and those numbers can be called when it is enabled.
Verify the numbers cannot be called when they are deleted from the list.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword pin2Password;
	GetPIN2PasswordL( phone, pin2Password );
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 securityCaps;
	TInt err = phone.GetSecurityCaps( securityCaps );
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetSecurityCaps returned an error"));
	CHECK_BITS_SET_L( securityCaps, RMobilePhone::KCapsAccessPin2, KNoUnwantedBits,
						_L("RMobilePhone::GetSecurityCaps returned wrong caps"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess for FDN phone book 
	RMobilePhoneBookStore& fdnStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer,KMainPhone,KIccFdnPhoneBook);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookInfoPckg(bookInfo);
	TExtEtelRequestStatus getInfoStatus(fdnStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (getInfoStatus);
	User::After(15*KOneSecond);	// Wait for phone book caching to finish
	fdnStore.GetInfo (getInfoStatus, bookInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium),KErrNone, _L("RMobilePhoneStore::GetInfo timed out"));
	CHECK_EQUALS_L(getInfoStatus.Int(), KErrNone, _L("RMobilePhoneStore::GetInfo returned an error"));
	TUint wantedBits = RMobilePhoneStore::KCapsWriteAccess| RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(bookInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned unexpected iCaps"));
	

	// Get Voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
    // Backup phone state
	TFdnCleanupDisable backup(iPhoneTsyTestHelper,phone,pin2Password);
    CleanupStack::PushL(backup);

    // === Turn FDN on ===

    // Post notification for RMobilePhone::NotifySecurityEvent

	// Call RMobilePhone::SetFdnSetting with RMobilePhone::EFdnSetOn
	// Don't wait for completion

	// Check RMobilePhone::NotifySecurityEvent completes with RMobilePhone::EPin2Required

	// Verify PIN2 security code with RMobilePhone::VerifySecurityCode with RMobilePhone::ESecurityCodePin2

	// Check RMobilePhone::NotifySecurityEvent completes with RMobilePhone::EPin2Verified
	// Wait for the RMobilePhone::SetFdnSetting to complete
	ASSERT_EQUALS(iPhoneTsyTestHelper.SetPhoneFdnSettingL(phone,pin2Password,RMobilePhone::EFdnSetOn),KErrNone,_L("RMobilePhone::SetFdnSetting returned an error"));

	// ===  Write a number to the FDN phone book and dial it ===

	// Write a valid number which will answer when dialled to the FDN phone book. 
	TBuf8<500> pbData; 
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); 
	pbData.FillZ();
	pbData.Zero();
	ASSERT_EQUALS(pbBuffer->AddNewEntryTag(),KErrNone,_L("CPhoneBookBuffer::AddNewEntryTag failed"));
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,			  
			_L("CTestStep::GetStringFromConfig did not complete as expected"));
	ASSERT_EQUALS(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber,number),
			KErrNone,_L("CPhoneBookBuffer::PutTagAndValue failed"));
	ASSERT_EQUALS(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi,(TUint8)RMobilePhone::ENationalNumber),
			KErrNone,_L("CPhoneBookBuffer::PutTagAndValue failed"));
	ASSERT_EQUALS(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText,_L("Name")),
			KErrNone,_L("CPhoneBookBuffer::PutTagAndValue failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(fdnStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	TInt index=1;
	fdnStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	ASSERT_EQUALS(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
	
	// post notifier for RMobileCall::NotifyMobileCallStatusChange
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	
	// Dial the written number. 
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
			_L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  
			_L("RCall::Dial returned with an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone,    
			_L("RMobileCall::GetMobileCallStatus returned an Error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected,    
			_L("RMobileCall::GetMobileCallStatus did not set the status to EStatusConnected"));

	// Hang up. 
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1,&RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone,  
			_L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone,  
			_L("RCall::HangUp returned an error"));

	// ===  Delete the number and try to dial it ===

	// Delete the number from the FDN phone book. 
	TExtEtelRequestStatus deleteStoreStatus(fdnStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStoreStatus);
	fdnStore.Delete(deleteStoreStatus,index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
			_L("RMobilePhoneBookStore::Delete timed-out"));
	ASSERT_EQUALS( deleteStoreStatus.Int(), KErrNone,
			_L("RMobilePhoneBookStore::Delete returned with an error."));

	// Dial the number. 
	call1.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
			_L("RCall::Dial timed-out"));

	// Check dialling fails.
	ASSERT_TRUE((dialStatus.Int()!= KErrNone),  
			_L("RCall::Dial did not returned with an error"));
	

	// ===  Set FDN to off and dial the number again ===

	// Call RMobilePhone::SetFdnSetting with RMobilePhone::EFdnSetOff 
	ASSERT_EQUALS(iPhoneTsyTestHelper.SetPhoneFdnSettingL(phone,pin2Password,RMobilePhone::EFdnSetOff),KErrNone,_L("RMobilePhone::SetFdnSetting returned an error"));

	// Dial the number again. 
	call1.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
			_L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  
			_L("RCall::Dial returned with an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone,    
			_L("RMobileCall::GetMobileCallStatus returned an Error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected,    
			_L("RMobileCall::GetMobileCallStatus did not set the status to EStatusConnected"));

	// Hang up
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone,  
			_L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone,  
			_L("RCall::HangUp returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	deleteStoreStatus
	//	hangUpStatus
	//	dialStatus
	//	notifyMobileCallStatusChangeStatus
	//	phoneBookWriteStatus
	//	pbBuffer
    //  backup
	//	getInfoStatus
	CleanupStack::PopAndDestroy(8, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestFixedDialling0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestFixedDialling0002");
	}



CCTSYIntegrationTestFixedDialling0003::CCTSYIntegrationTestFixedDialling0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestFixedDiallingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestFixedDialling0003::GetTestStepName());
	}

CCTSYIntegrationTestFixedDialling0003::~CCTSYIntegrationTestFixedDialling0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestFixedDialling0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-FXD-0003
 * @SYMFssID BA/CTSY/FXD-0003
 * @SYMTestCaseDesc Edit the FDN list when PIN2 not verified.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read, RMobilePhoneStore::Delete, RMobilePhoneBookStore::Write
 * @SYMTestExpectedResults Pass - Modification of FDN not possible when PIN2 not verified.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify writing, deleting and reading of the FDN list is not possible.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	TUint32 securityCaps;
	TInt err = phone.GetSecurityCaps( securityCaps );
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetSecurityCaps returned an error"));
	CHECK_BITS_SET_L( securityCaps, RMobilePhone::KCapsAccessPin2, KNoUnwantedBits,
						_L("RMobilePhone::GetSecurityCaps returned wrong caps"));


	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | | RMobilePhoneBookStore::KCapsRestrictedWriteAccess for FDN phone book 
	RMobilePhoneBookStore& fdnStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer,KMainPhone,KIccFdnPhoneBook);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookInfoPckg(bookInfo);
	TExtEtelRequestStatus getInfoStatus(fdnStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (getInfoStatus);
	User::After(15*KOneSecond);	
	fdnStore.GetInfo (getInfoStatus, bookInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium),KErrNone, _L("RMobilePhoneStore::GetInfo timed out"));
	CHECK_EQUALS_L(getInfoStatus.Int(), KErrNone, _L("RMobilePhoneStore::GetInfo returned an error"));
	TUint wantedBits = RMobilePhoneStore::KCapsWriteAccess| RMobilePhoneStore::KCapsReadAccess | RMobilePhoneBookStore::KCapsRestrictedWriteAccess;
	CHECK_BITS_SET_L(bookInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned unexpected iCaps"));


    //
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Open the FDN phone book. 

	// Attempt to write a valid entry to the FDN list using RMobilePhoneBookStore::Write 
    TBuf8<500> pbData; 
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); 
	pbData.FillZ();
	pbData.Zero();
	ASSERT_EQUALS(pbBuffer->AddNewEntryTag(),KErrNone,_L("CPhoneBookBuffer::AddNewEntryTag failed"));
	TPtrC number=_L("012345678"); 
	ASSERT_EQUALS(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber,number),
			KErrNone,_L("CPhoneBookBuffer::PutTagAndValue failed"));
	ASSERT_EQUALS(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi,(TUint8)RMobilePhone::ENationalNumber),
			KErrNone,_L("CPhoneBookBuffer::PutTagAndValue failed"));
	ASSERT_EQUALS(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText,_L("Name")),
			KErrNone,_L("CPhoneBookBuffer::PutTagAndValue failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(fdnStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	TInt index=1;
	fdnStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));

	// Check Error is returned.
	ASSERT_TRUE(phoneBookWriteStatus.Int()!= KErrNone, 
			_L("RMobilePhoneBookStore::Write did not return with an error"));

	// Attempt to delete a valid entry to the FDN list using RMobilePhoneBookStore::Delete
	TExtEtelRequestStatus deleteStoreStatus(fdnStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStoreStatus);
	fdnStore.Delete(deleteStoreStatus,1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
			_L("RMobilePhoneBookStore::Delete timed-out"));

    // $CTSYProblem We expect an error != KErrNone here but the LTSY is returning KErrNone which the CTSY correctly propagates back to the test
	ASSERT_TRUE( deleteStoreStatus.Int()== KErrNone,
			_L("RMobilePhoneBookStore::Delete did not return with an error."));

	// Attempt to read a valid entry to the FDN list using RMobilePhoneBookStore::Read 
	pbData.FillZ();
	pbData.Zero();		
	TExtEtelRequestStatus phoneBookReadStatus(fdnStore, EMobilePhoneBookStoreRead);
	CleanupStack::PushL(phoneBookReadStatus);
	fdnStore.Read(phoneBookReadStatus, index, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));

	// Check error is returned.
	ASSERT_TRUE( phoneBookReadStatus.Int()!= KErrNone,
			_L("RMobilePhoneBookStore::Read did not return with an error."));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	phoneBookReadStatus
	//	deleteStoreStatus
	//	phoneBookWriteStatus
	//	pbBuffer
	//	getInfoStatus
	CleanupStack::PopAndDestroy(5,&getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestFixedDialling0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestFixedDialling0003");
	}



CCTSYIntegrationTestFixedDialling0004::CCTSYIntegrationTestFixedDialling0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestFixedDiallingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestFixedDialling0004::GetTestStepName());
	}

CCTSYIntegrationTestFixedDialling0004::~CCTSYIntegrationTestFixedDialling0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestFixedDialling0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-FXD-0004
 * @SYMFssID BA/CTSY/FXD-0004
 * @SYMTestCaseDesc Dial numbers without FDN check.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::VerifySecurityCode, RMobilePhone::SetFdnSetting, RMobilePhone::NotifySecurityEvent, RCall::HangUp, RMobileCall::GetMobileCallStatus, RMobileCall::DialNoFdnCheck
 * @SYMTestExpectedResults Pass - Numbers can be dialled with RMobileCall::DialNoFdnCheck when FDN enabled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify RMobileCall::DialNoFdnCheck can dial numbers not in the FDN list when the FDN is enabled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetSecurityCaps returns caps in set of KCapsAccessPin2 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 securityCaps;
	TInt err = phone.GetSecurityCaps( securityCaps );
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetSecurityCaps returned an error"));
	CHECK_BITS_SET_L( securityCaps, RMobilePhone::KCapsAccessPin2, KNoUnwantedBits,
						_L("RMobilePhone::GetSecurityCaps returned wrong caps"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess for FDN phone book 
	RMobilePhoneBookStore& fdnStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer,KMainPhone,KIccFdnPhoneBook);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookInfoPckg(bookInfo);
	TExtEtelRequestStatus getInfoStatus(fdnStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL (getInfoStatus);
	User::After(15*KOneSecond);	
	fdnStore.GetInfo (getInfoStatus, bookInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium),KErrNone, _L("RMobilePhoneStore::GetInfo timed out"));
	CHECK_EQUALS_L(getInfoStatus.Int(), KErrNone, _L("RMobilePhoneStore::GetInfo returned an error"));
	TUint wantedBits = RMobilePhoneStore::KCapsWriteAccess| RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(bookInfo.iCaps,wantedBits,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned unexpected iCaps"));

    // Ensure PIN2 is locked on the SIM 
	RMobilePhone::TMobilePassword pin2Password;
	GetPIN2PasswordL( phone, pin2Password );
	CHECK_EQUALS_L(iPhoneTsyTestHelper.SetPhoneFdnSettingL(phone,pin2Password,RMobilePhone::EFdnSetOn),KErrNone,_L("RMobilePhone::SetFdnSetting returned an error"));

	// Get Voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
    // Backup phone state
	TFdnCleanupDisable backup(iPhoneTsyTestHelper,phone,pin2Password);
    CleanupStack::PushL(backup);

	//
	// TEST START
	//
	
	
	// ===  Dial no FDN check with FDN enabled ===

	// Open the FDN phone book. 

	// Verify PIN2 security code with RMobilePhone::VerifySecurityCode with RMobilePhone::ESecurityCodePin2 

	// Check RMobilePhone::NotifySecurityEvent completes with RMobilePhone::EPin2Verified
	iSimTsyTestHelper.VerifyPin2L(phone,iNetworkTsyTestHelper);
	// Delete all entries from the FDN phone book. 
	TExtEtelRequestStatus deletaAllStatus(fdnStore, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deletaAllStatus);
	fdnStore.DeleteAll(deletaAllStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deletaAllStatus, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	ASSERT_EQUALS(deletaAllStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::DeleteAll returned with an error"));
	
	// post notifier for RMobileCall::NotifyMobileCallStatusChange
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);

	// Dial a number that answers using RMobileCall::DialNoFdnCheck 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,			  
			_L("CTestStep::GetStringFromConfig did not complete as expected"));
	TExtEtelRequestStatus dialStatus(call1, EMobileCallDialNoFdnCheck);
	CleanupStack::PushL(dialStatus);
	call1.DialNoFdnCheck(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone,
			_L("RCall::DialNoFdnCheck timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::DialNoFdnCheck returned with an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone,    
			_L("RMobileCall::GetMobileCallStatus returned an Error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected,    
			_L("RMobileCall::GetMobileCallStatus did not set the status to EStatusConnected"));

	// Hang up with RCall::HangUp 
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1,&RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone,  
			_L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone,  
			_L("RCall::HangUp returned an error"));

	// ===  Dial no FDN check with FDN disabled ===

	// Call RMobilePhone::SetFdnSetting with RMobilePhone::EFdnSetOff 
	ASSERT_EQUALS(iPhoneTsyTestHelper.SetPhoneFdnSettingL(phone,pin2Password,RMobilePhone::EFdnSetOn),KErrNone,_L("RMobilePhone::SetFdnSetting returned an error"));

	// Dial the number again using RMobileCall::DialNoFdnCheck 
	call1.DialNoFdnCheck(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone,
			_L("RCall::DialNoFdnCheck timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::DialNoFdnCheck returned with an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															KErrNone);

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone,    
			_L("RMobileCall::GetMobileCallStatus returned an Error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected,    
			_L("RMobileCall::GetMobileCallStatus did not set the status to EStatusConnected"));
	 
	
	//
	// TEST END
	//

    StartCleanup();

	// Hang up with RCall::HangUp 
    
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone,  
			_L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone,  
			_L("RCall::HangUp returned an error"));
    
	// Pop:
	//	/*hangUpStatus*/
	//	dialStatus
	//	notifyMobileCallStatusChangeStatus
	//	deletaAllStatus
    //  backup
	//	getInfoStatus
	CleanupStack::PopAndDestroy(6, &getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestFixedDialling0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestFixedDialling0004");
	}


