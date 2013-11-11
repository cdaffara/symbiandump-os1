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
// This file contains the test steps for Unit Test Suite 01 : Parseline.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "script.h"
#include "parseline.h"
#include "Filename.h" 

// Specific includes for this test suite
#include "TSU_MmTsthSuite01.h"

// Specific includes for these test steps
#include "TSU_MmTsth01.h"

// --------------------------------------------

// Unit Test Suite 01 : Parseline.cpp
// Depends on : none

// Tests :-
// 1 Initialise a CParseLine
// 2 Process line : load & run a sample step
// 3 Run a sample panic step
// 4 Run a utility
// 11 CSuiteDLL initialise and verify

// (NB these tests are to test the parser ONLY; the underlying functionality of the
// commands parsed is tested in CTestStep, CTestSuite, CTestUtils etc. suites)

// ---------------------
// RTestMmTsthU0101

RTestMmTsthU0101* RTestMmTsthU0101::NewL()
	{
	RTestMmTsthU0101* self = new(ELeave) RTestMmTsthU0101;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0101::RTestMmTsthU0101()
	{
	iTestStepName = _L("MM-TSTH-U-0101");
	}

// preamble
TVerdict RTestMmTsthU0101::OpenL()
	{
	// don't call the default preamble
	return iTestStepResult = EPass;
	}

// postamble
void RTestMmTsthU0101::Close()
	{
	// don't call the default postamble
	}

// Do the test step.
TVerdict RTestMmTsthU0101::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for Parseline"));

	TVerdict currentVerdict = EPass;

	// initialise a CParseLine
	CLog* theLogClient  = iSuite->LogSystem();
	CTestUtils* theTestUtils  = CTestUtils::NewL(theLogClient);
	TInt64 theGuardTimer (-1);

	CleanupStack::PushL(theTestUtils);
	CScript* theParseScript = CScript::NewLC(theTestUtils, theLogClient, theGuardTimer, KNullDesC);
	CParseLine* theParser = CParseLine::NewL(theParseScript, theTestUtils, theLogClient, theGuardTimer, KNullDesC);
	CleanupStack::PushL(theParser);

	// if we got here, we're constructed successfully
	INFO_PRINTF1(_L("CParseLine constructed successfully"));

	CleanupStack::PopAndDestroy(3); // theParser, theParseScript, theTestUtils

	return iTestStepResult = currentVerdict;
	}


// ---------------------
// RTestMmTsthU0102

RTestMmTsthU0102* RTestMmTsthU0102::NewL()
	{
	RTestMmTsthU0102* self = new(ELeave) RTestMmTsthU0102;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0102::RTestMmTsthU0102()
	{
	iTestStepName = _L("MM-TSTH-U-0102");
	}

// Do the test step.
TVerdict RTestMmTsthU0102::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for ProcessLineL 1"));

	TVerdict currentVerdict = EPass;
	_LIT8(KTestStepScriptLine1, "LOAD_SUITE TSU_MMTSTH01");
	_LIT8(KTestStepScriptLine2, "RUN_TEST_STEP 1000 TSU_MMTSTH01 RTestStepDummy");

	TRAPD(err1, iParseLine->ProcessLineL(KTestStepScriptLine1, 1));
	if(err1 != KErrNone)
	{
		ERR_PRINTF2(_L("ProcessLineL for LOAD_SUITE left, error code %d"), err1);
		return iTestStepResult = EFail;
	}
	TRAPD(err2, iParseLine->ProcessLineL(KTestStepScriptLine2, 2));
	if(err2 != KErrNone)
	{
		ERR_PRINTF2(_L("ProcessLineL for RUN_TEST_STEP left, error code %d"), err2);
		return iTestStepResult = EFail;
	}

	return iTestStepResult = currentVerdict;
	}

// ---------------------
// RTestMmTsthU0103

RTestMmTsthU0103* RTestMmTsthU0103::NewL()
	{
	RTestMmTsthU0103* self = new(ELeave) RTestMmTsthU0103;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0103::RTestMmTsthU0103()
	{
	iTestStepName = _L("MM-TSTH-U-0103");
	}

// Do the test step.
TVerdict RTestMmTsthU0103::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for ProcessLineL 2"));

	TVerdict currentVerdict = EPass;
	_LIT8(KTestStepScriptLine1, "LOAD_SUITE TSU_MMTSTH01");
	_LIT8(KTestStepScriptLine2, "RUN_PANIC_STEP 1000 TSU_MMTSTH01 RPanicTestStepDummy, PanicDummy, 0");

	TRAPD(err1, iParseLine->ProcessLineL(KTestStepScriptLine1, 1));
	if(err1 != KErrNone)
	{
		ERR_PRINTF2(_L("ProcessLineL for LOAD_SUITE left, error code %d"), err1);
		return iTestStepResult = EInconclusive;
	}
	TRAPD(err2, iParseLine->ProcessLineL(KTestStepScriptLine2, 2));
	if(err2 != KErrNone)
	{
		ERR_PRINTF2(_L("ProcessLineL for RUN_PANIC_STEP left, error code %d"), err2);
		return iTestStepResult = EFail;
	}

	return iTestStepResult = currentVerdict;
	}


// ---------------------
// RTestMmTsthU0104

RTestMmTsthU0104* RTestMmTsthU0104::NewL()
	{
	RTestMmTsthU0104* self = new(ELeave) RTestMmTsthU0104;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0104::RTestMmTsthU0104()
	{
	iTestStepName = _L("MM-TSTH-U-0104");
	}

// Do the test step.
TVerdict RTestMmTsthU0104::DoTestStepL()
	{

	INFO_PRINTF1(_L("This test step is not available on EKA2 - Passing test!"));
	return EPass;

	}

// ---------------------
// RTestMmTsthU0111

RTestMmTsthU0111* RTestMmTsthU0111::NewL()
	{
	RTestMmTsthU0111* self = new(ELeave) RTestMmTsthU0111;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0111::RTestMmTsthU0111()
	{
	iTestStepName = _L("MM-TSTH-U-0111");
	}

// Do the test step.
TVerdict RTestMmTsthU0111::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for CSuiteDLL"));

	TVerdict currentVerdict = EPass;

	// create a new CSuiteDll object to store info on loaded DLL
	_LIT(KTestSuiteName, "TSU_MMTSTH01");
	CSuiteDll* newRef = NULL;
	TRAPD(err, newRef = CSuiteDll::NewL(KTestSuiteName, iLogClient));
	if(err != KErrNone)
	{
		ERR_PRINTF2(_L("CSuiteDll::NewL left, error code %d"), err);
		return iTestStepResult = EFail;
	}

	// make sure it's not NULL, get its name out and see if it matches
	CTestSuite* theTestSuite = newRef->Suite();
	if(theTestSuite == NULL)
	{
		ERR_PRINTF1(_L("CSuiteDll::Suite() is NULL"));
		return iTestStepResult = EFail;
	}

	TPtrC theName = newRef->Name();
	if (theName != KTestSuiteName)
	{
		ERR_PRINTF2(_L("CSuiteDll::Name() is %S"), &theName);
		return iTestStepResult = EFail;
	}

	return iTestStepResult = currentVerdict;
	}

// ---------------------
// Dummy test step for Parseline testing

RTestStepDummy* RTestStepDummy::NewL()
	{
	RTestStepDummy* self = new(ELeave) RTestStepDummy;
	return self;
	}

// Each test step initialises its own name.
RTestStepDummy::RTestStepDummy()
	{
	iTestStepName = _L("RTestStepDummy");
	}

// Do the test step.
TVerdict RTestStepDummy::DoTestStepL()
	{
	INFO_PRINTF1(_L("Dummy step completed"));

	TVerdict currentVerdict = EPass;

	return iTestStepResult = currentVerdict;
	}


// ---------------------
// Dummy panic test steps for Parseline testing

RPanicTestStepDummy* RPanicTestStepDummy::NewL()
	{
	RPanicTestStepDummy* self = new(ELeave) RPanicTestStepDummy;
	return self;
	}

// Each test step initialises its own name.
RPanicTestStepDummy::RPanicTestStepDummy()
	{
	iTestStepName = _L("RPanicTestStepDummy");
	}

// Do the test step.
TVerdict RPanicTestStepDummy::DoTestStepL()
	{
	INFO_PRINTF1(_L("Dummy panic step running"));
	User::Panic(_L("PanicDummy"), 0);

	// NB this code cannot be reached
	TVerdict currentVerdict = EPass;
	return iTestStepResult = currentVerdict;
	}


