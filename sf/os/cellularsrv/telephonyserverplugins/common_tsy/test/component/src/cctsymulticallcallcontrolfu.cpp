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
// The TEFUnit test suite for MulticallCallControl in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsymulticallcallcontrolfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyMulticallCallControlFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyMulticallCallControlFU, TestGetMulticallParams0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMulticallCallControlFU, TestSetMulticallParams0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMulticallCallControlFU, TestNotifyMulticallParamsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMulticallCallControlFU, TestNotifyMulticallIndicatorChange0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-MCC-MGMP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetMulticallParams
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMulticallParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMulticallCallControlFU::TestGetMulticallParams0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMulticallSettingsV1 multicallSettings;
	RMobilePhone::TMobilePhoneMulticallSettingsV1Pckg
	                                    multicallParams(multicallSettings);
	iPhone.GetMulticallParams(reqStatus, multicallParams);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-MCC-MSMP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetMulticallParams
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetMulticallParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMulticallCallControlFU::TestSetMulticallParams0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	iPhone.SetMulticallParams(reqStatus, 2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-MCC-MNMPC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyMulticallParamsChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyMulticallParamsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMulticallCallControlFU::TestNotifyMulticallParamsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMulticallSettingsV1 multicallSettings;
	RMobilePhone::TMobilePhoneMulticallSettingsV1Pckg
	                                     multicallParams(multicallSettings);
	iPhone.NotifyMulticallParamsChange(reqStatus, multicallParams);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-MCC-MNMIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyMulticallIndicatorChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyMulticallIndicatorChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMulticallCallControlFU::TestNotifyMulticallIndicatorChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMulticallIndicator
	                    multicallIndicator(RMobilePhone::EBearerUserExceeded);
	iPhone.NotifyMulticallIndicatorChange(reqStatus, multicallIndicator);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}



