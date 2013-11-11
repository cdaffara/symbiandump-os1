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
// This file contains the test steps for Unit Test Suite 12 : TestSuite.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TSU_MmTsthStep12.h"
#include "TSU_MmTsthSuite12.h"

// Specific includes for these test steps
#include "TSU_MmTsth12.h"

// --------------------------------------------

// Unit Test Suite 12 : TestSuite.cpp
// Depends on : TestStep

// Tests :-
// 1 ConstructL / InitialiseL
// 2 AddTestStepL
// 3 DoTestStep
// 11 Log - not tested explicitly (if things are logging, it's working!)
// 12 LogExtra - not tested explicitly
// 21 GetVersion
// 22 accessors

// ---------------------
// RTestMmTsthU1201
RTestMmTsthU1201* RTestMmTsthU1201::NewL()
	{
	RTestMmTsthU1201* self = new(ELeave) RTestMmTsthU1201;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1201::RTestMmTsthU1201()
	{
	iTestStepName = _L("MM-TSTH-U-1201");
	}

// preamble
TVerdict RTestMmTsthU1201::OpenL()
	{
	// stub - purpose is that for this test we do not run the parent preamble
	// which initialises iStepStub
	return iTestStepResult = EPass;
	}

// postamble
void RTestMmTsthU1201::Close()
	{
	}

// do the test step
TVerdict RTestMmTsthU1201::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestSuite - ConstructL / InitialiseL"));

	TVerdict currentVerdict = EPass;

	CTestSuiteVirtualStub* theSuiteStub = new (ELeave) CTestSuiteVirtualStub;
	CleanupStack::PushL(theSuiteStub);
	TRAPD(err, theSuiteStub->ConstructL());
	if(err != KErrNone)
		{
		ERR_PRINTF1(_L("CTestSuiteVirtualStub::ConstructL() left"));
		CleanupStack::PopAndDestroy(theSuiteStub);
		return iTestStepResult = EFail;
		}
	iSuiteStub = theSuiteStub;

	// NB ensure the suite can log - set its logger to ours
	iSuiteStub->SetLogSystem(iSuite->LogSystem());

	// ConstructL calls InitialiseL.
	// the suite should be called TestSuiteVirtualStub and contain one step, TestStepVirtualStub
	// we can't access this direct but we can run DoTestStep() on it,
	// and any error code other than ETestSuiteError is a pass
	
	TVerdict stepVerdict = iSuiteStub->DoTestStep(_L("TestStepVirtualStub"), KNullDesC, KNullDesC);
	if(stepVerdict == ETestSuiteError)
	{
		ERR_PRINTF1(_L("RTestSuiteVirtualStub::InitialiseL() did not setup test step"));
		CleanupStack::PopAndDestroy(theSuiteStub);
		return iTestStepResult = EFail;
	}

	CleanupStack::PopAndDestroy(theSuiteStub);
	iSuiteStub = NULL;

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU1202

RTestMmTsthU1202* RTestMmTsthU1202::NewL()
	{
	RTestMmTsthU1202* self = new(ELeave) RTestMmTsthU1202;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1202::RTestMmTsthU1202()
	{
	iTestStepName = _L("MM-TSTH-U-1202");
	}

// do the test step
TVerdict RTestMmTsthU1202::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestSuite - AddTestStepL"));

	TVerdict currentVerdict = EPass;

	TRAPD(err, iSuiteStub->AddTestStepL(RTestStepVirtualStub2::NewL()));
	if(err != KErrNone)
		{
		ERR_PRINTF1(_L("RTestSuiteVirtualStub::AddTestStepL() left"));
		return iTestStepResult = EFail;
		}

	// find the step we just added
	TVerdict stepVerdict = iSuiteStub->DoTestStep(_L("TestStepVirtualStub2"), KNullDesC, KNullDesC);
	if(stepVerdict == ETestSuiteError)
		{
		ERR_PRINTF1(_L("RTestSuiteVirtualStub::AddTestStepL() failed"));
		return iTestStepResult = EFail;
		}

	// no pop / delete - the SUITE owns the step, we don't.
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU1203

RTestMmTsthU1203* RTestMmTsthU1203::NewL()
	{
	RTestMmTsthU1203* self = new(ELeave) RTestMmTsthU1203;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1203::RTestMmTsthU1203()
	{
	iTestStepName = _L("MM-TSTH-U-1203");
	}

// do the test step
TVerdict RTestMmTsthU1203::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestSuite - DoTestStepL"));

	TVerdict currentVerdict = EPass;

	// do the step. this time we're testing for PASS only
	TVerdict stepVerdict = iSuiteStub->DoTestStep(_L("TestStepVirtualStub"), KNullDesC, KNullDesC);
	if(stepVerdict != EPass)
		{
		ERR_PRINTF1(_L("RTestSuiteVirtualStub::DoTestStepL() failed"));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU1221

RTestMmTsthU1221* RTestMmTsthU1221::NewL()
	{
	RTestMmTsthU1221* self = new(ELeave) RTestMmTsthU1221;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1221::RTestMmTsthU1221()
	{
	iTestStepName = _L("MM-TSTH-U-1221");
	}

// do the test step
TVerdict RTestMmTsthU1221::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestSuite - GetVersion"));

	TVerdict currentVerdict = EPass;

	// get the version, compare it against our known value
	_LIT(KTestVersion,"CTestSuiteVirtualStub Version");
	TPtrC version = iSuiteStub->GetVersion();
	if(version != KTestVersion)
		{
		ERR_PRINTF1(_L("RTestSuiteVirtualStub::GetVersion() failed"));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU1222

RTestMmTsthU1222* RTestMmTsthU1222::NewL()
	{
	RTestMmTsthU1222* self = new(ELeave) RTestMmTsthU1222;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1222::RTestMmTsthU1222()
	{
	iTestStepName = _L("MM-TSTH-U-1222");
	}

// do the test step.
TVerdict RTestMmTsthU1222::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestSuite - accessors"));

	TVerdict currentVerdict = EPass;

	iSuiteStub->SetSeverity(ESevrInfo);
	TInt theSev = iSuiteStub->Severity();
	if(theSev != ESevrInfo)
		{
		ERR_PRINTF1(_L("CTestSuite::SetSeverity() failed"));
		return iTestStepResult = EFail;
		}

	iSuiteStub->SetStepStatus(EStepStatusFinished);
	TTestStepStatus theStepStatus = iSuiteStub->StepStatus();
	if(theStepStatus != EStepStatusFinished)
		{
		ERR_PRINTF1(_L("CTestSuite::SetStepStatus() failed"));
		return iTestStepResult = EFail;
		}

	CLog* theLog = CLog::NewL();
	iSuiteStub->SetLogSystem(theLog);
	CLog* theLogSystem = iSuiteStub->LogSystem();
	if(theLog != theLogSystem)
		{
		ERR_PRINTF1(_L("CTestSuite::SetLogSystem() failed"));
		delete theLog;
		return iTestStepResult = EFail;
		}

	delete theLog;
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}
