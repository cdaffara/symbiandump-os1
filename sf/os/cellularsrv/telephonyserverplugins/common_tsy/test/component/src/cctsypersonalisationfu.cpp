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
// The TEFUnit test suite for Personalisation in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsypersonalisationfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyPersonalisationFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPersonalisationFU, TestGetPersonalisationCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPersonalisationFU, TestGetPersonalisationStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPersonalisationFU, TestSetPersonalisationStatus0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PERS-MGPC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetPersonalisationCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPersonalisationCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPersonalisationFU::TestGetPersonalisationCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TUint32 caps;
	TInt ret(iPhone.GetPersonalisationCaps(caps));
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-PERS-MGPS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetPersonalisationStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPersonalisationStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPersonalisationFU::TestGetPersonalisationStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	TUint32 persSchemes;
	iPhone.GetPersonalisationStatus(requestStatus, persSchemes);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-PERS-MSPS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetPersonalisationStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetPersonalisationStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPersonalisationFU::TestSetPersonalisationStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf16 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhonePersonalisation persSchemes(RMobilePhone::EPersonaliseNetwork);
	iPhone.SetPersonalisationStatus(requestStatus, persSchemes, data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



