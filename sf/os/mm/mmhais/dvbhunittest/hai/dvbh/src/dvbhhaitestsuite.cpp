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
// dvbhhaitestsuite.h.cpp
// This main DLL entry point for the DVB-H HAI test suite.
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "dvbhhaitestsuite.h"
#include "dvbhhaitest.h"
#include "dvbhhaiasynctest.h"

/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuiteDvbhHai* NewTestSuiteL() 
    { 
	CTestSuiteDvbhHai* self = new(ELeave) CTestSuiteDvbhHai;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self); 
	return self;
    }

/**
 * Destructor
 */
CTestSuiteDvbhHai::~CTestSuiteDvbhHai()
	{
	}

/**
 * Get Test Suite version
 */
TPtrC CTestSuiteDvbhHai::GetVersion() const
	{
	_LIT(KTxtVersion, "1.0");
	return KTxtVersion();
	}

/**
 * This function initialises the test suite
 * this creates all the test steps and stores them inside CTestSuiteDvbhHai 
 */
void CTestSuiteDvbhHai::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_DVBH_HAI");
	
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-001")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-002")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-003")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-004")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-005")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-006")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-007")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-008")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-009")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-010")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-011")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-012")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-013")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-014")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-015")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-016")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-017")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-018")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-019")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-020")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-022")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-023")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-024")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-025")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-026")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-027")));
	
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-101")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-102")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-103")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-104")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-105")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-106")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-107")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-108")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-109")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-110")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-111")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-112")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-113")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-114")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-115")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-116")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-117")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-118")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-119")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-120")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-121")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-122")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-123")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-124")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-125")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-126")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-127")));
	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-128")));

	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-202")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-203")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-204")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-205")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-206")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-207")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-208")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-209")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-210")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-211")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-212")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-213")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-214")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-215")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-216")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-217")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-218")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-219")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-220")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-221")));
	AddTestStepL( RDvbhHaiTest::NewL(EFalse,_L("MM-TV-DVBH-U-222")));

	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-301")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-302")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-303")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-304")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-305")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-306")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-307")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-308")));
	AddTestStepL( RDvbhHaiTest::NewL(ETrue,_L("MM-TV-DVBH-U-309")));

	AddTestStepL( RDvbhHaiAsyncTest::NewL(ETrue,_L("MM-TV-DVBH-U-401")));

	}


