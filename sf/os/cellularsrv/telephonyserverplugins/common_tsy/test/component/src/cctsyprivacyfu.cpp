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
// The TEFUnit test suite for Privacy in the Common TSY.
// 
//

/**
 @file 
*/
#include "cctsyprivacyfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyPrivacyFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestSetPrivacy0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestSetPrivacy0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestSetPrivacy0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyPrivacyConfirmation0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyPrivacyConfirmation0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyPrivacyConfirmation0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyPrivacyConfirmation0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyPrivacyConfirmation0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyPrivacyConfirmation0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyPrivacyConfirmation00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyPrivacyConfirmation00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyPrivacyConfirmation00014L);

	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestGetDefaultPrivacy0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestSetDefaultPrivacy0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPrivacyFU, TestNotifyDefaultPrivacyChange0001L);

	END_SUITE;
	}


void CCTsyPrivacyFU::DoSetPrivacyL(const TDesC& aLineName,RMobilePhone::TMobilePhonePrivacy aPrivacySetting)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	TInt err = line.Open(iPhone, aLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	RMobileCall mobileCall;
	err = mobileCall.OpenNewCall(line);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(mobileCall);
	
	// EMobileCallSetPrivacy is not passed to LTSY
	// CTSY returns KErrNotSupported immediately
	err = mobileCall.SetPrivacy(aPrivacySetting);
	ASSERT_EQUALS(KErrNotSupported, err);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // mobileCall, line, this	

	}


void CCTsyPrivacyFU::DoNotifyPrivacyConfirmationL(const TDesC& aLineName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall mobileCall;

	TInt err = line.Open(iPhone, aLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	err = mobileCall.OpenNewCall(line);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(mobileCall);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::NotifyPrivacyConfirmation when result is not cached.
 	//-------------------------------------------------------------------------

	// invoke RMobilePhone::NotifyPrivacyConfirmation
	TRequestStatus notifyStatus;

	// initialize the variable to be set by CTSY with the value other than passed to MockLTSY
	RMobilePhone::TMobilePhonePrivacy privacy = RMobilePhone::EPrivacyOn;
	mobileCall.NotifyPrivacyConfirmation(notifyStatus,privacy);

	// prepare the data to be returned from MockLTSY
	RMobilePhone::TMobilePhonePrivacy privacySetting = RMobilePhone::EPrivacyUnspecified;
	TMockLtsyCallData1<RMobilePhone::TMobilePhonePrivacy> privacySettingData(privacySetting);

	RBuf8 data;
	CleanupClosePushL(data);
	privacySettingData.SerialiseL(data);

	// trigger EMobileCallNotifyPrivacyConfirmation completion via MockLTSY 
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	iMockLTSY.CompleteL(EMobileCallNotifyPrivacyConfirmation, KErrNone,data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone,mockLtsyStatus.Int());

	// wait for RMobilePhone::NotifyPrivacyConfirmation completion
	User::WaitForRequest(notifyStatus);

	ASSERT_EQUALS(KErrNone,notifyStatus.Int());
	ASSERT_EQUALS(RMobilePhone::EPrivacyUnspecified, privacy);
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::NotifyPrivacyConfirmation
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	iMockLTSY.CompleteL(EMobileCallNotifyPrivacyConfirmation, KErrNone,data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone,mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // data, mobileCall, line, this
	}


void CCTsyPrivacyFU::DoCancelNotifyPrivacyConfirmationL(const TDesC& aLineName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall mobileCall;

	TInt err = line.Open(iPhone, aLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	err = mobileCall.OpenNewCall(line);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::NotifyPrivacyConfirmation
 	//-------------------------------------------------------------------------
	TRequestStatus notifyStatus;

	RMobilePhone::TMobilePhonePrivacy privacy;
	mobileCall.NotifyPrivacyConfirmation(notifyStatus,privacy);
	mobileCall.CancelAsyncRequest(EMobileCallNotifyPrivacyConfirmation);
	User::WaitForRequest(notifyStatus);

    ASSERT_EQUALS(KErrCancel, notifyStatus.Int());

	AssertMockLtsyStatusL(); // just in case

	CleanupStack::PopAndDestroy(3); // mobileCall, line, this
	}
	
void CCTsyPrivacyFU::DoMultiNotifyPrivacyConfirmationL(const TDesC& aLineName)
	{
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileCall::NotifyPrivacyConfirmation
 	//-------------------------------------------------------------------------

	// Open first client					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line1;
	TInt err = line1.Open(iPhone, aLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line1);
	
	RMobileCall mobileCall1;
	err = mobileCall1.OpenNewCall(line1);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall1);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line2;
	err = line2.Open(phone2, aLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line2);
	
	RMobileCall mobileCall2;
	err = mobileCall2.OpenNewCall(line2);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall2);

	// initialize variables with values other than passed to MockLTSY
	RMobilePhone::TMobilePhonePrivacy privacy1 = RMobilePhone::EPrivacyOn;
	RMobilePhone::TMobilePhonePrivacy privacy2 = RMobilePhone::EPrivacyOff;

	// request notifications
	TRequestStatus notifyStatus1;
	TRequestStatus notifyStatus2;

	mobileCall1.NotifyPrivacyConfirmation(notifyStatus1,privacy1);
	mobileCall2.NotifyPrivacyConfirmation(notifyStatus2,privacy2);

	// trigger EMobileCallNotifyPrivacyConfirmation completion via MockLTSY 
	RMobilePhone::TMobilePhonePrivacy privacySetting = RMobilePhone::EPrivacyUnspecified;
	TMockLtsyCallData1<RMobilePhone::TMobilePhonePrivacy> privacySettingData(privacySetting);

	RBuf8 data;
	CleanupClosePushL(data);
	privacySettingData.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	iMockLTSY.CompleteL(EMobileCallNotifyPrivacyConfirmation, KErrNone,data);
	User::WaitForRequest(mockLtsyStatus);
	CleanupStack::PopAndDestroy(1,&data); // data
	ASSERT_EQUALS(KErrNone,mockLtsyStatus.Int());

	
	// wait for both notifications
	User::WaitForRequest(notifyStatus1);
	ASSERT_EQUALS(KErrNone,notifyStatus1.Int());
    ASSERT_EQUALS(RMobilePhone::EPrivacyUnspecified, privacy1);

	User::WaitForRequest(notifyStatus2);
	ASSERT_EQUALS(KErrNone,notifyStatus2.Int());
    ASSERT_EQUALS(RMobilePhone::EPrivacyUnspecified, privacy2);

	// cleanup second client
	CleanupStack::PopAndDestroy(4,&telServer2); // mobileCall2, line2, phone2, telServer2

	AssertMockLtsyStatusL();
	
	// cleanup first client
	CleanupStack::PopAndDestroy(3, this); // mobileCall1, line1, this
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PRI-CSP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetPrivacy for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetPrivacy for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestSetPrivacy0001L()
	{
	DoSetPrivacyL(KMmTsyVoice1LineName,RMobilePhone::EPrivacyOn);	
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CSP-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetPrivacy for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetPrivacy for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestSetPrivacy0006L()
	{
	DoSetPrivacyL(KMmTsyDataLineName,RMobilePhone::EPrivacyOn);
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CSP-0011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetPrivacy for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetPrivacy for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestSetPrivacy0011L()
	{
	DoSetPrivacyL(KMmTsyFaxLineName,RMobilePhone::EPrivacyOn);
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CNPC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyPrivacyConfirmation for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyPrivacyConfirmation for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyPrivacyConfirmation0001L()
	{
	DoNotifyPrivacyConfirmationL(KMmTsyVoice1LineName);	
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CNPC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyPrivacyConfirmation for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyPrivacyConfirmation for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyPrivacyConfirmation0002L()
	{
	DoCancelNotifyPrivacyConfirmationL(KMmTsyVoice1LineName);
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CNPC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyPrivacyConfirmation for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyPrivacyConfirmation for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyPrivacyConfirmation0004L()
	{
	DoMultiNotifyPrivacyConfirmationL(KMmTsyVoice1LineName);	
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CNPC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyPrivacyConfirmation for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyPrivacyConfirmation for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyPrivacyConfirmation0006L()
	{
	DoNotifyPrivacyConfirmationL(KMmTsyDataLineName);	
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CNPC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyPrivacyConfirmation for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyPrivacyConfirmation for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyPrivacyConfirmation0007L()
	{
	DoCancelNotifyPrivacyConfirmationL(KMmTsyDataLineName);
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CNPC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyPrivacyConfirmation for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyPrivacyConfirmation for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyPrivacyConfirmation0009L()
	{
	DoMultiNotifyPrivacyConfirmationL(KMmTsyDataLineName);	
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CNPC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyPrivacyConfirmation for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyPrivacyConfirmation for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyPrivacyConfirmation00011L()
	{
	DoNotifyPrivacyConfirmationL(KMmTsyFaxLineName);	
	}


/**
@SYMTestCaseID BA-CTSY-PRI-CNPC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyPrivacyConfirmation for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyPrivacyConfirmation for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyPrivacyConfirmation00012L()
	{
	DoCancelNotifyPrivacyConfirmationL(KMmTsyFaxLineName);
	}

/**
@SYMTestCaseID BA-CTSY-PRI-CNPC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyPrivacyConfirmation for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyPrivacyConfirmation for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyPrivacyConfirmation00014L()
	{
	DoMultiNotifyPrivacyConfirmationL(KMmTsyFaxLineName);	
	}


/**
@SYMTestCaseID BA-CTSY-PRI-MGDP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetDefaultPrivacy
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetDefaultPrivacy
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestGetDefaultPrivacy0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// EMobilePhoneGetDefaultPrivacy is not passed to LTSY
	// CTSY returns KErrNotSupported immediately
	RMobilePhone::TMobilePhonePrivacy privacy;
	TInt err = iPhone.GetDefaultPrivacy(privacy);
	ASSERT_EQUALS(KErrNotSupported, err);
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PRI-MSDP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetDefaultPrivacy
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetDefaultPrivacy
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestSetDefaultPrivacy0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// EMobilePhoneSetDefaultPrivacy is not passed to LTSY
	// CTSY returns KErrNotSupported immediately
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhonePrivacy privacy = RMobilePhone::EPrivacyOn;
	iPhone.SetDefaultPrivacy(requestStatus,privacy);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PRI-MNDPC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyDefaultPrivacyChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyDefaultPrivacyChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPrivacyFU::TestNotifyDefaultPrivacyChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// EMobilePhoneNotifyDefaultPrivacyChange is not passed to LTSY
	// CTSY returns KErrNotSupported immediately
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhonePrivacy privacy;
	iPhone.NotifyDefaultPrivacyChange(requestStatus,privacy);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this); // data, this
	
	}



