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
// The TEFUnit test suite for CallCost in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsycallcostfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyCallCostFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyCallCostFU, TestGetCostCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCostFU, TestGetCostInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCostFU, TestClearCostMeter0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCostFU, TestSetMaxCostMeter0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCostFU, TestNotifyCostCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCostFU, TestNotifyCostInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallCostFU, TestSetPuct0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-CCOS-MGCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCostCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCostCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCostFU::TestGetCostCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TUint32 caps(0);
	TInt err = iPhone.GetCostCaps(caps);
	ASSERT_EQUALS(KErrNotSupported, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOS-MGCI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCostInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCostInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCostFU::TestGetCostInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobilePhoneCostInfoV1 costInfo;
	RMobilePhone::TMobilePhoneCostInfoV1Pckg infoPckg(costInfo);
	TRequestStatus reqStatus;
	iPhone.GetCostInfo(reqStatus, infoPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOS-MCCM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ClearCostMeter
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ClearCostMeter
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCostFU::TestClearCostMeter0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobilePhoneCostMeters meter(RMobilePhone::EClearCCM);
	TRequestStatus reqStatus;
	iPhone.ClearCostMeter(reqStatus, meter);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOS-MSMCM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetMaxCostMeter
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetMaxCostMeter
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCostFU::TestSetMaxCostMeter0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TUint units(10);
	TRequestStatus reqStatus;
	iPhone.SetMaxCostMeter(reqStatus, units);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOS-MNCCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCostCapsChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCostCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCostFU::TestNotifyCostCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TUint32  caps(0);
	TRequestStatus reqStatus;
	iPhone.NotifyCostCapsChange(reqStatus, caps);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOS-MNCIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCostInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCostInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCostFU::TestNotifyCostInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobilePhoneCostInfoV1 costInfo;
	RMobilePhone::TMobilePhoneCostInfoV1Pckg infoPckg(costInfo);
	TRequestStatus reqStatus;
	iPhone.NotifyCostInfoChange(reqStatus, infoPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CCOS-MSP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetPuct
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetPuct
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallCostFU::TestSetPuct0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobilePhonePuctV1 puct;
	RMobilePhone::TMobilePhonePuctV1Pckg puctPckg(puct);
	TRequestStatus reqStatus;
	iPhone.SetPuct(reqStatus, puctPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}



