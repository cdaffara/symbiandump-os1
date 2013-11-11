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
// The TEFUnit test suite for MultimediaCallControl in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsymultimediacallcontrolfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyMultimediaCallControlFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyMultimediaCallControlFU, TestNotifyCallPreferenceChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMultimediaCallControlFU, TestAnswerMultimediaCallAsVoice0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMultimediaCallControlFU, TestAnswerMultimediaCallAsVoice0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMultimediaCallControlFU, TestAnswerMultimediaCallAsVoice0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMultimediaCallControlFU, TestGetMultimediaCallPreference0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMultimediaCallControlFU, TestSetMultimediaCallPreference0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-MMCC-MNMCPC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyMultimediaCallPreferenceChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyMultimediaCallPreferenceChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMultimediaCallControlFU::TestNotifyCallPreferenceChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMultimediaSettings
	                        mmSettings(RMobilePhone::EAcceptMMCallsVoiceFallback);
	iPhone.NotifyMultimediaCallPreferenceChange(reqStatus, mmSettings);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-MMCC-MAMCAV-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerMultimediaCallAsVoice for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerMultimediaCallAsVoice for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMultimediaCallControlFU::TestAnswerMultimediaCallAsVoice0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	TName callName;
	TBuf8<1> callParams;
	mobileCall.AnswerMultimediaCallAsVoice(reqStatus, callParams, callName);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line

	err = line.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	mobileCall.AnswerMultimediaCallAsVoice(reqStatus, callParams, callName);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-MMCC-MAMCAV-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerMultimediaCallAsVoice for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerMultimediaCallAsVoice for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMultimediaCallControlFU::TestAnswerMultimediaCallAsVoice0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	TName callName;
	TBuf8<1> callParams;
	mobileCall.AnswerMultimediaCallAsVoice(reqStatus, callParams, callName);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-MMCC-MAMCAV-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerMultimediaCallAsVoice for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerMultimediaCallAsVoice for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMultimediaCallControlFU::TestAnswerMultimediaCallAsVoice0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	TName callName;
	TBuf8<1> callParams;
	mobileCall.AnswerMultimediaCallAsVoice(reqStatus, callParams, callName);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-MMCC-MGMCP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetMultimediaCallPreference
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMultimediaCallPreference
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMultimediaCallControlFU::TestGetMultimediaCallPreference0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobilePhoneMultimediaSettings
	                         mmSettings(RMobilePhone::EAcceptMMCallsVoiceFallback);
	TInt ret = iPhone.GetMultimediaCallPreference(mmSettings);
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-MMCC-MSMCP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetMultimediaCallPreference
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetMultimediaCallPreference
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMultimediaCallControlFU::TestSetMultimediaCallPreference0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMultimediaSettings
	                        mmSettings(RMobilePhone::EAcceptMMCallsVoiceFallback);
	iPhone.SetMultimediaCallPreference(reqStatus, mmSettings);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}



