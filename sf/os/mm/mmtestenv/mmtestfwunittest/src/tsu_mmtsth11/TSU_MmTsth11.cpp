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
// This file contains the test steps for Unit Test Suite 11 : TestStep.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TSU_MmTsthStep11.h"
#include "TSU_MmTsthSuite11.h"

// Specific includes for these test steps
#include "TSU_MmTsth11.h"
#include "TestStepVirtualStub.h"
#include "TestSuiteVirtualStub.h"

// --------------------------------------------

// Unit Test Suite 11 : TestStep.cpp
// Depends on : none
// Requires : subclass implementing pure virtual DoTestStepL()

// Tests :-
//	RTestStep() constructor (thru stub)
//  test accessors
//	DoTestStepL
//	DoTestStepPreamble
//  DoTestStepPostamble
//	Load / unload config

// ---------------------
// RTestMmTsthU1101

RTestMmTsthU1101* RTestMmTsthU1101::NewL()
	{
	RTestMmTsthU1101* self = new(ELeave) RTestMmTsthU1101;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1101::RTestMmTsthU1101()
	{
	iTestStepName = _L("MM-TSTH-U-1101");
	}


// preamble
TVerdict RTestMmTsthU1101::OpenL()
	{
	// stub - purpose is that for this test we do not run the parent preamble
	// which initialises iStepStub
	return iTestStepResult = EPass;
	}

// postamble
void RTestMmTsthU1101::Close()
	{
	}

// do the test step
TVerdict RTestMmTsthU1101::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestStep - construct"));
	TVerdict currentVerdict = EPass;

	RTestStepVirtualStub* theStepStub = NULL;
	TRAPD(err, theStepStub = RTestStepVirtualStub::NewL());
	if(err != KErrNone)
		{
		ERR_PRINTF1(_L("RTestStepVirtualStub::NewL() left"));
		return iTestStepResult = EFail;
		}

	const TPtrC& theName = theStepStub->StepName();
	if(theName != KTestStepVirtualStubName)
		{
		ERR_PRINTF1(_L("RTestStepVirtualStub did not initialise correctly"));
		delete theStepStub;
		return iTestStepResult = EFail;
		}

	delete theStepStub;
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU1102
RTestMmTsthU1102* RTestMmTsthU1102::NewL()
	{
	RTestMmTsthU1102* self = new(ELeave) RTestMmTsthU1102;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1102::RTestMmTsthU1102()
	{
	iTestStepName = _L("MM-TSTH-U-1102");
	}

// do the test step
TVerdict RTestMmTsthU1102::DoTestStepL()
	{
	// RTestStepVirtualStub contains extra methods by which we can verify our accessors
	INFO_PRINTF1(_L("Unit test for TestStep - accessors"));

	TVerdict currentVerdict = EPass;

	// test accessor to set name - not part of RTestStep
	TBufC<KMaxLenTestStepName> KTestStepName = _L("TestStepName");
	iStepStub->TestSetStepName(KTestStepName);
	const TPtrC& theName = iStepStub->StepName();
	if(theName != KTestStepName)
		{
		ERR_PRINTF1(_L("RTestStep::StepName() failed"));
		return iTestStepResult = EFail;
		}

	TVerdict theResult = EInconclusive;
	iStepStub->SetResult(theResult);
	// test accessor to get result - not part of RTestStep
	if(theResult != iStepStub->TestGetResult())
		{
		ERR_PRINTF1(_L("RTestStep::SetResult() failed"));
		return iTestStepResult = EFail;
		}

	CTestSuiteVirtualStub* theSuite = new (ELeave) CTestSuiteVirtualStub;
	CleanupStack::PushL(theSuite);
	theSuite->ConstructL();

	iStepStub->SetSuite(theSuite);
	// test accessor to get suite - not part of RTestStep
	if(theSuite != iStepStub->TestGetSuite())
		{
		ERR_PRINTF1(_L("RTestStep::SetSuite() failed"));
		CleanupStack::PopAndDestroy(); // theSuite
		return iTestStepResult = EFail;
		}
	// cleanup
	CleanupStack::PopAndDestroy(); // theSuite

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ---------------------
// RTestMmTsthU1103

RTestMmTsthU1103* RTestMmTsthU1103::NewL()
	{
	RTestMmTsthU1103* self = new(ELeave) RTestMmTsthU1103;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1103::RTestMmTsthU1103()
	{
	iTestStepName = _L("MM-TSTH-U-1103");
	}

// do the test step.
TVerdict RTestMmTsthU1103::DoTestStepL()
	{
	// RTestStepVirtualStub contains extra methods by which we can verify our accessors
	INFO_PRINTF1(_L("Unit test for TestStep - preamble"));

	TVerdict currentVerdict = EPass;

	iStepStub->OpenL();
	if(!iStepStub->PreambleRun())
		{
		ERR_PRINTF1(_L("RTestStep::OpenL() failed"));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ---------------------
// RTestMmTsthU1104

RTestMmTsthU1104* RTestMmTsthU1104::NewL()
	{
	RTestMmTsthU1104* self = new(ELeave) RTestMmTsthU1104;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1104::RTestMmTsthU1104()
	{
	iTestStepName = _L("MM-TSTH-U-1104");
	}

// do the test step
TVerdict RTestMmTsthU1104::DoTestStepL()
	{
	// RTestStepVirtualStub contains extra methods by which we can verify our accessors
	INFO_PRINTF1(_L("Unit test for TestStep - postamble"));

	TVerdict currentVerdict = EPass;

	iStepStub->Close();
	if(!iStepStub->PostambleRun())
		{
		ERR_PRINTF1(_L("RTestStep::Close() failed"));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ---------------------
// RTestMmTsthU1105
RTestMmTsthU1105* RTestMmTsthU1105::NewL()
	{
	RTestMmTsthU1105* self = new(ELeave) RTestMmTsthU1105;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1105::RTestMmTsthU1105()
	{
	iTestStepName = _L("MM-TSTH-U-1105");
	}

// do the test step
TVerdict RTestMmTsthU1105::DoTestStepL()
	{
	// RTestStepVirtualStub contains extra methods by which we can verify our accessors
	INFO_PRINTF1(_L("Unit test for TestStep - step"));

	TVerdict currentVerdict = EPass;

	iStepStub->DoTestStepL();
	if(!iStepStub->StepRun())
		{
		ERR_PRINTF1(_L("RTestStep::DoTestStepL() failed"));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU1111
RTestMmTsthU1111* RTestMmTsthU1111::NewL()
	{
	RTestMmTsthU1111* self = new(ELeave) RTestMmTsthU1111;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1111::RTestMmTsthU1111()
	{
	iTestStepName = _L("MM-TSTH-U-1111");
	}

// do the test step
TVerdict RTestMmTsthU1111::DoTestStepL()
	{
	const TInt KTestBufSize = 64;

	// RTestStepVirtualStub contains extra methods by which we can verify our accessors
	INFO_PRINTF1(_L("Unit test for TestStep - load config"));

	TVerdict currentVerdict = EPass;

	TBufC<KTestBufSize> KConfigName = _L("TSU_MMTSTH11_config.ini");
	iStepStub->LoadConfig(KConfigName);

	// we get no return saying that the file hasn't loaded - 
	// therefore check something in the file we know is there : [SectionOne] keybool = true
	// because these functions are protected, we have to wrap them in iStepStub

	TBool aBoolResult;
	TBool returnValue = iStepStub->TestGetBoolFromConfig(_L("SectionOne"),_L("Keybool"), aBoolResult);
	if(!returnValue)
		{
		ERR_PRINTF1(_L("RTestStep::LoadConfig() failed"));
		iStepStub->UnloadConfig();
		return iTestStepResult = EFail;
		}

	iStepStub->UnloadConfig();
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------


