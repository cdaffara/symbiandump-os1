// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the test steps for Unit Test Suite 00 : TestFrameworkMain.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestFrameworkMain.h"
#include "TSU_MmTsthSuite00.h"

// Specific includes for these test steps
#include "TSU_MmTsth00.h"

// --------------------------------------------

// Unit Test Suite 00 : TestFrameworkMain.cpp
// Depends on : none

// Tests :-
// 1 Construct a CTestFrameworkMain

// (Other tests are specific to member classes!)

// ---------------------
// RTestMmTsthU0001

RTestMmTsthU0001* RTestMmTsthU0001::NewL()
	{
	RTestMmTsthU0001* self = new(ELeave) RTestMmTsthU0001;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0001::RTestMmTsthU0001()
	{
	iTestStepName = _L("MM-TSTH-U-0001");
	}

// preamble
TVerdict RTestMmTsthU0001::OpenL()
	{
	return iTestStepResult = EPass;
	}

// postamble
void RTestMmTsthU0001::Close()
	{
	}

// Do the test step.
TVerdict RTestMmTsthU0001::DoTestStepL()
	{
	TVerdict currentVerdict = EPass;

	INFO_PRINTF1(_L("Unit test for TestFrameworkMain"));

	CTestFrameworkMain* theTest = CTestFrameworkMain::NewLC();

	// NB we can't run a second instance of CTestFrameworkMain (i.e. call StartTestingL())
	// without causing the old one to close at cleanup, stopping all tests.
	// Therefore our useful testing is limited to ensuring a CTestFrameworkMain
	// constructs correctly

	// check a log has been constructed. if it is not NULL, we've passed
	CLog* theLog = theTest->LogClient();
	if(theLog != NULL)
		INFO_PRINTF1(_L("CTestFramework constructed correctly"));

	CleanupStack::PopAndDestroy(theTest);

	// we pass if we haven't left
	return iTestStepResult = currentVerdict; // should be EPass if we've got here

	}


