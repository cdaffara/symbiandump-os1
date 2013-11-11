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
#include "TSU_MmTsthStep11.h"
#include "TSU_MmTsth11.h"
#include "TSU_MmTsthSuite11.h"




// Create an instance of this test suite.
// NewTSUMmTsthSuite11L is exported at ordinal 1.
// This provides the interface to allow the Test Framework
// to create instances of this test suite.
EXPORT_C CTSUMmTsthSuite11* NewTSUMmTsthSuite11L() 
    { 
	CTSUMmTsthSuite11* result = new (ELeave) CTSUMmTsthSuite11;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

// destructor
CTSUMmTsthSuite11::~CTSUMmTsthSuite11()
	{
	}

_LIT(KTxtVersion,"0.22 (build 09-Sep-2002)");

// Get test suite version.
TPtrC CTSUMmTsthSuite11::GetVersion() const
	{
	return KTxtVersion();
	}

// Initialiser for test suite.
// This creates all the test steps and stores them
// inside CTestSuite
void CTSUMmTsthSuite11::InitialiseL()
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMTSTH11");

	// add test steps
	AddTestStepL(RTestMmTsthU1101::NewL());
	AddTestStepL(RTestMmTsthU1102::NewL());
	AddTestStepL(RTestMmTsthU1103::NewL());
	AddTestStepL(RTestMmTsthU1104::NewL());
	AddTestStepL(RTestMmTsthU1105::NewL());

	AddTestStepL(RTestMmTsthU1111::NewL());
	}


// -------------------------
