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
// This file contains the test steps for Unit Test Suite 02 : Script.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
// Test system includes
#include <testframework.h>
#include "script.h"
#include "parseline.h"
#include "Filename.h"

// Specific includes for this test suite
#include "TSU_MmTsthSuite02.h"

// Specific includes for these test steps
#include "TSU_MmTsth02.h"

// --------------------------------------------

// Unit Test Suite 02: Script.cpp
// Depends on : Parseline, FileName

// Tests :-
// 1 construct a CScript
// 2 open a script file
// 3 execute a script file
// 12 add / display results	(removed - the functionality is covered by Parseline tests 0102 and 0103)
 
// ---------------------
// RTestMmTsthU0201

RTestMmTsthU0201* RTestMmTsthU0201::NewL()
	{
	RTestMmTsthU0201* self = new(ELeave) RTestMmTsthU0201;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0201::RTestMmTsthU0201()
	{
	iTestStepName = _L("MM-TSTH-U-0201");
	}

// Do the test step.
TVerdict RTestMmTsthU0201::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for Script : construction"));

	TVerdict currentVerdict = EPass;

	// initialise a CParseLine
	CLog* theLogClient  = iSuite->LogSystem();
	CTestUtils* theTestUtils  = CTestUtils::NewL(theLogClient);
	CleanupStack::PushL(theTestUtils);
	TInt64 defTime(-1);
	CScript* theTestScript = CScript::NewLC(theTestUtils, theLogClient, defTime, KNullDesC);

	// if we got here, we're constructed successfully
	INFO_PRINTF1(_L("CScript constructed successfully"));

	CleanupStack::PopAndDestroy(2); // theTestScript, theTestUtils
	theTestScript = NULL; // just to remove warning

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ---------------------
// RTestMmTsthU0202

RTestMmTsthU0202* RTestMmTsthU0202::NewL()
	{
	RTestMmTsthU0202* self = new(ELeave) RTestMmTsthU0202;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0202::RTestMmTsthU0202()
	{
	iTestStepName = _L("MM-TSTH-U-0202");
	}

// Do the test step.
TVerdict RTestMmTsthU0202::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for Script - OpenScriptFile"));

	TVerdict currentVerdict = EPass;

	_LIT(KDummyScriptName, "TSU_MMTSTH02_DUMMY");
	CFileName* scriptFileName = CFileName::NewLC();
	*scriptFileName = KDummyScriptName;
	if (iTestScript->OpenScriptFile(scriptFileName) == EFalse)
	{
		ERR_PRINTF2(_L("OpenScriptFile failed to open %S"), &KDummyScriptName());
		currentVerdict = EFail;
	}
	CleanupStack::PopAndDestroy(scriptFileName);
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ---------------------
// RTestMmTsthU0203

RTestMmTsthU0203* RTestMmTsthU0203::NewL()
	{
	RTestMmTsthU0203* self = new(ELeave) RTestMmTsthU0203;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0203::RTestMmTsthU0203()
	{
	iTestStepName = _L("MM-TSTH-U-0203");
	}

// Do the test step.
TVerdict RTestMmTsthU0203::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for Script - ExecuteScriptL"));

	TVerdict currentVerdict = EPass;

	_LIT(KDummyScriptName, "TSU_MMTSTH02_DUMMY");
	CFileName* scriptFileName = CFileName::NewLC();
	*scriptFileName = KDummyScriptName;
	if (iTestScript->OpenScriptFile(scriptFileName) == EFalse)
		{
		ERR_PRINTF2(_L("OpenScriptFile failed to open %S"), &KDummyScriptName());
		CleanupStack::PopAndDestroy(scriptFileName);
		return iTestStepResult = EInconclusive;
		}

	// execute the script
	TVerdict theTestVerdict=EFail;
	TRAPD(err, theTestVerdict = iTestScript->ExecuteScriptL());

	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("ExecuteScriptL left with error code %d"), err);
		currentVerdict = EFail;
		}
	else if (theTestVerdict!=EPass)
		{
		ERR_PRINTF2(_L("ExecuteScriptL returned with error  %d"), theTestVerdict);
		currentVerdict =theTestVerdict;
		}

	CleanupStack::PopAndDestroy(scriptFileName);
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

