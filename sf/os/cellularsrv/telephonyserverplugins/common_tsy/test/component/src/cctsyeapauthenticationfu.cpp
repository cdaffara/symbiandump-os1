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
// The TEFUnit test suite for EAPAuthentication in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyeapauthenticationfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyEAPAuthenticationFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyEAPAuthenticationFU, TestOpen0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-EAP-EO-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmartCardEap::Open
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmartCardEap::Open
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyEAPAuthenticationFU::TestOpen0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileSmartCardEap smartCardEap;
	RMobilePhone::TAID AID;
	RMobileSmartCardEap::TEapType eapType;
	TInt err = smartCardEap.Open(iPhone, AID, eapType);
	ASSERT_EQUALS(KErrNotSupported, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);

	}

