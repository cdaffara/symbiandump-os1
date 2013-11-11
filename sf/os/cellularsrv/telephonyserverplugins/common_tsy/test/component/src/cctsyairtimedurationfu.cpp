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
// The TEFUnit test suite for AirTimeDuration in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyairtimedurationfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyAirTimeDurationFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyAirTimeDurationFU, TestGetAirTimeDuration0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAirTimeDurationFU, TestNotifyAirTimeDurationChange0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-ATD-MGATD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetAirTimeDuration
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetAirTimeDuration
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAirTimeDurationFU::TestGetAirTimeDuration0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TTimeIntervalSeconds time;
	TInt ret(iPhone.GetAirTimeDuration(time));
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-ATD-MNATDC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyAirTimeDurationChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyAirTimeDurationChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAirTimeDurationFU::TestNotifyAirTimeDurationChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	TTimeIntervalSeconds time;
	iPhone.NotifyAirTimeDurationChange(requestStatus, time);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); 
	
	}



