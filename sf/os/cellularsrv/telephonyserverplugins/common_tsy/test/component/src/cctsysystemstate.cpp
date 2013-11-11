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
// The TEFUnit test for the access to the System State from the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsysystemstate.h"
#include <ctsy/mmtsy_names.h>
#include "tmockltsydata.h"

CTestSuite* CCTsySystemState::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_TEST_STEP_ISO_CPP(CCTsySystemState, TestSystemState0001L);
	END_SUITE;
	}

//
// Actual test cases
//

/**
@SYMTestCaseID BA-CTSY-SYSS-SYSSPLG-0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test that System State plugin is loaded correctly
@SYMTestPriority High
@SYMTestActions Attempts to dial an emergency call. This will cause the System State plugin to be loaded.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySystemState::TestSystemState0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this)); // PUSH #1
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);   // PUSH #2

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(line);  // PUSH #3
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall); // PUSH #4

	RMobileENStore::TEmergencyNumber number(_L("911"));
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNotSupported);

	TRequestStatus reqStatus;
	mobileCall.DialEmergencyCall(reqStatus, number);

	// failure to load system state plugin will result in DialEmergencyCall completing with KErrNotFound
	User::WaitForRequest(reqStatus);
	INFO_PRINTF2(_L("CCTsyFeatMgr::TestSystemState0001: DialEmergencyCall request completed with error %d"),reqStatus.Int());
	ASSERT_FALSE(KErrNotFound == reqStatus.Int());
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}
