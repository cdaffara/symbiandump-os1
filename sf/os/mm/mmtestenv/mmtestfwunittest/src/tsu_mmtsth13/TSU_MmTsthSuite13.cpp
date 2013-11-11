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
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "TSU_MmTsthStep13.h"
#include "TSU_MmTsth13.h"
#include "TSU_MmTsthSuite13.h"



// Create an instance of this test suite.
// NewTSUMmTsthSuite13L is exported at ordinal 1.
// This provides the interface to allow the Test Framework
// to create instances of this test suite.
EXPORT_C CTSUMmTsthSuite13* NewTSUMmTsthSuite13L() 
    { 
	CTSUMmTsthSuite13* result = new (ELeave) CTSUMmTsthSuite13;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

// destructor
CTSUMmTsthSuite13::~CTSUMmTsthSuite13()
	{
	}

_LIT(KTxtVersion,"0.22 (build 09-Sep-2002)");

// Get test suite version.
TPtrC CTSUMmTsthSuite13::GetVersion() const
	{
	return KTxtVersion();
	}

// Initialiser for test suite.
// This creates all the test steps and stores them
// inside CTestSuite
void CTSUMmTsthSuite13::InitialiseL()
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMTSTH13");

	// add test steps
	AddTestStepL(RTestMmTsthU1301::NewL());
	AddTestStepL(RTestMmTsthU1302::NewL());
	AddTestStepL(RTestMmTsthU1303::NewL());
	AddTestStepL(RTestMmTsthU1304::NewL());
	AddTestStepL(RTestMmTsthU1305::NewL());
	AddTestStepL(RTestMmTsthU1306::NewL());

	AddTestStepL(RTestMmTsthU1311::NewL());
	}
