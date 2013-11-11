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
// The TEFUnit test suite for LBS in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsylbsfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyLBSFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyLBSFU, TestNotifyMtlr0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLBSFU, TestSetLcsDomain0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLBSFU, TestSendMtlrResponse0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLBSFU, TestSendMtlrResponse0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLBSFU, TestSendMtlrResponse0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLBSFU, TestSendMeasurementReport0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLBSFU, TestSendMeasurementReport0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLBSFU, TestNotifyMeasurementControl0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLBSFU, TestSendMolr0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-LBS-LNM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLocationServices::NotifyMtlr
@SYMTestPriority High
@SYMTestActions Invokes RMobileLocationServices::NotifyMtlr
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLBSFU::TestNotifyMtlr0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileLocationServices locationServices;
    CleanupClosePushL(locationServices);
	TInt err = locationServices.Open(iPhone);
	ASSERT_EQUALS(KErrNotSupported, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // locationServices, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LBS-LSLD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLocationServices::SetLcsDomain
@SYMTestPriority High
@SYMTestActions Invokes RMobileLocationServices::SetLcsDomain
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLBSFU::TestSetLcsDomain0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileLocationServices locationServices;
    CleanupClosePushL(locationServices);
    TInt err = locationServices.Open(iPhone);
    ASSERT_EQUALS(KErrNotSupported, err);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // locationServices, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LBS-LSMTLRR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLocationServices::SendMtlrResponse
@SYMTestPriority High
@SYMTestActions Invokes RMobileLocationServices::SendMtlrResponse
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLBSFU::TestSendMtlrResponse0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileLocationServices locationServices;
    CleanupClosePushL(locationServices);
    TInt err = locationServices.Open(iPhone);
    ASSERT_EQUALS(KErrNotSupported, err);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // locationServices, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LBS-LSMTLRR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLocationServices::SendMtlrResponse
@SYMTestPriority High
@SYMTestActions Invokes RMobileLocationServices::SendMtlrResponse
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLBSFU::TestSendMtlrResponse0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileLocationServices locationServices;
    CleanupClosePushL(locationServices);
    TInt err = locationServices.Open(iPhone);
    ASSERT_EQUALS(KErrNotSupported, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // locationServices, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LBS-LSMTLRR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLocationServices::SendMtlrResponse
@SYMTestPriority High
@SYMTestActions Invokes RMobileLocationServices::SendMtlrResponse
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLBSFU::TestSendMtlrResponse0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileLocationServices locationServices;
    CleanupClosePushL(locationServices);
    TInt err = locationServices.Open(iPhone);
    ASSERT_EQUALS(KErrNotSupported, err);

    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // locationServices, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LBS-LSMR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLocationServices::SendMeasurementReport
@SYMTestPriority High
@SYMTestActions Invokes RMobileLocationServices::SendMeasurementReport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLBSFU::TestSendMeasurementReport0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileLocationServices locationServices;
    CleanupClosePushL(locationServices);
    TInt err = locationServices.Open(iPhone);
    ASSERT_EQUALS(KErrNotSupported, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // locationServices, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LBS-LSMR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLocationServices::SendMeasurementReport
@SYMTestPriority High
@SYMTestActions Invokes RMobileLocationServices::SendMeasurementReport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLBSFU::TestSendMeasurementReport0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileLocationServices locationServices;
    CleanupClosePushL(locationServices);
    TInt err = locationServices.Open(iPhone);
    ASSERT_EQUALS(KErrNotSupported, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // locationServices, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LBS-LMC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLocationServices::NotifyMeasurementControl
@SYMTestPriority High
@SYMTestActions Invokes RMobileLocationServices::NotifyMeasurementControl
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLBSFU::TestNotifyMeasurementControl0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileLocationServices locationServices;
    CleanupClosePushL(locationServices);
    TInt err = locationServices.Open(iPhone);
    ASSERT_EQUALS(KErrNotSupported, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // locationServices, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LBS-LSM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLocationServices::SendMolr
@SYMTestPriority High
@SYMTestActions Invokes RMobileLocationServices::SendMolr
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLBSFU::TestSendMolr0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobileLocationServices locationServices;
    CleanupClosePushL(locationServices);
    TInt err = locationServices.Open(iPhone);
    ASSERT_EQUALS(KErrNotSupported, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // locationServices, this
	
	}



