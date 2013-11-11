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
// This file is a helper for TestFramework Unit Test classes.
// It subclasses the abstract CTestSuite to produce a working class we can test
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "TestStepVirtualStub.h"
#include "TestSuiteVirtualStub.h"


// do not export if Unit Testing
#if defined (__TSU_TESTFRAMEWORK__)
#undef EXPORT_C
#define EXPORT_C

// define only if not Unit Testing
#else


// Create an instance of this test suite.
// NewTestSuiteVirtualStubL is exported at ordinal 1.
// This provides the interface to allow the Test Framework
// to create instances of this test suite.
EXPORT_C CTestSuiteVirtualStub* NewTestSuiteVirtualStubL() 
    { 
	CTestSuiteVirtualStub* result = new (ELeave) CTestSuiteVirtualStub;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }
#endif

// destructor
CTestSuiteVirtualStub::~CTestSuiteVirtualStub()
	{
	}

_LIT(KTxtVersion,"CTestSuiteVirtualStub Version");

// Get test suite version.
TPtrC CTestSuiteVirtualStub::GetVersion() const
	{
	return KTxtVersion();
	}

// Initialiser for test suite.
// This creates all the test steps and stores them
// inside CTestSuiteVirtualStub
void CTestSuiteVirtualStub::InitialiseL()
	{

	// store the name of this test suite
	iSuiteName = KTestSuiteVirtualStub;

	// add test steps
	AddTestStepL(RTestStepVirtualStub::NewL());

	}
