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
// The TEFUnit test suite for TrafficChannel in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsytrafficchannelfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyTrafficChannelFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyTrafficChannelFU, TestSetTrafficChannel0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyTrafficChannelFU, TestSetTrafficChannel0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyTrafficChannelFU, TestSetTrafficChannel00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyTrafficChannelFU, TestNotifyTrafficChannelConfirmation0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyTrafficChannelFU, TestNotifyTrafficChannelConfirmation0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyTrafficChannelFU, TestNotifyTrafficChannelConfirmation00011L);

	END_SUITE;
	}


/**
Tests that RMobileCall::SetTrafficChannel is not supported for a call
@param aLineName The name of the line from which the call is opened
*/
void CCTsyTrafficChannelFU::TestSetTrafficChannelNotSupportedL(const TDesC& aLineName)
	{

	RLine line;
	OpenLineLC(line, aLineName);

	RMobileCall mobileCall;
	OpenNewCallLC(line, mobileCall);

	RMobileCall::TMobileCallTch tchRequest(RMobileCall::ETchUnknown);
	TInt err = mobileCall.SetTrafficChannel(tchRequest);
	ASSERT_EQUALS(KErrNotSupported, err)

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line

	}


/**
Tests that RMobileCall::NotifyTrafficChannelConfirmation is not supported for a call
@param aLineName The name of the line from which the call is opened
*/
void CCTsyTrafficChannelFU::TestNotifyTrafficChannelConfirmationNotSupportedL(const TDesC& aLineName)
	{

	RLine line;
	OpenLineLC(line, aLineName);

	RMobileCall mobileCall;
	OpenNewCallLC(line, mobileCall);

	RMobileCall::TMobileCallTch tchType(RMobileCall::ETchUnknown);
	TRequestStatus reqStatus;
	mobileCall.NotifyTrafficChannelConfirmation(reqStatus, tchType);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line

	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-TRCH-CSTC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetTrafficChannel for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetTrafficChannel for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyTrafficChannelFU::TestSetTrafficChannel0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TestSetTrafficChannelNotSupportedL(KMmTsyVoice1LineName);

	TestSetTrafficChannelNotSupportedL(KMmTsyVoice2LineName);

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-TRCH-CSTC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetTrafficChannel for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetTrafficChannel for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyTrafficChannelFU::TestSetTrafficChannel0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TestSetTrafficChannelNotSupportedL(KMmTsyDataLineName);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-TRCH-CSTC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetTrafficChannel for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetTrafficChannel for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyTrafficChannelFU::TestSetTrafficChannel00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TestSetTrafficChannelNotSupportedL(KMmTsyFaxLineName);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-TRCH-CNTCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyTrafficChannelConfirmation for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyTrafficChannelConfirmation for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyTrafficChannelFU::TestNotifyTrafficChannelConfirmation0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TestNotifyTrafficChannelConfirmationNotSupportedL(KMmTsyVoice1LineName);

	TestNotifyTrafficChannelConfirmationNotSupportedL(KMmTsyVoice2LineName);

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-TRCH-CNTCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyTrafficChannelConfirmation for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyTrafficChannelConfirmation for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyTrafficChannelFU::TestNotifyTrafficChannelConfirmation0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TestNotifyTrafficChannelConfirmationNotSupportedL(KMmTsyDataLineName);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-TRCH-CNTCC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyTrafficChannelConfirmation for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyTrafficChannelConfirmation for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyTrafficChannelFU::TestNotifyTrafficChannelConfirmation00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TestNotifyTrafficChannelConfirmationNotSupportedL(KMmTsyFaxLineName);

	CleanupStack::PopAndDestroy(this);
	
	}



