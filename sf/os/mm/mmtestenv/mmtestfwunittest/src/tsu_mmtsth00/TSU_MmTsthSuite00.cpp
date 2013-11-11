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
#include "TSU_MmTsth00.h"
#include "TSU_MmTsthSuite00.h"




// Create an instance of this test suite.
// NewTSUMmTsthSuite00L is exported at ordinal 1.
// This provides the interface to allow the Test Framework
// to create instances of this test suite.
EXPORT_C CTSUMmTsthSuite00* NewTSUMmTsthSuite00L() 
    { 
	CTSUMmTsthSuite00* result = new (ELeave) CTSUMmTsthSuite00;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

// destructor
CTSUMmTsthSuite00::~CTSUMmTsthSuite00()
	{
	}

_LIT(KTxtVersion,"0.22 (build 09-Sep-2002)");

// Get test suite version.
TPtrC CTSUMmTsthSuite00::GetVersion() const
	{
	return KTxtVersion();
	}

// Initialiser for test suite.
// This creates all the test steps and stores them
// inside CTestSuite
void CTSUMmTsthSuite00::InitialiseL()
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMTSTH00");

	// add test steps
	AddTestStepL(RTestMmTsthU0001::NewL());

	}
