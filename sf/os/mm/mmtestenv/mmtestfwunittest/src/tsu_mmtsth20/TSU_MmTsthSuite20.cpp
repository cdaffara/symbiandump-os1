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
#include "TSU_MmTsth20.h"
#include "TSU_MmTsthSuite20.h"




// Create an instance of this test suite.
// NewTSUMmTsthSuite20L is exported at ordinal 1.
// This provides the interface to allow the Test Framework
// to create instances of this test suite.
EXPORT_C CTSUMmTsthSuite20* NewTSUMmTsthSuite20L() 
    { 
	CTSUMmTsthSuite20* result = new (ELeave) CTSUMmTsthSuite20;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

// destructor
CTSUMmTsthSuite20::~CTSUMmTsthSuite20()
	{
	}

_LIT(KTxtVersion,"0.22 (build 09-Sep-2002)");

// Get test suite version.
TPtrC CTSUMmTsthSuite20::GetVersion() const
	{
	return KTxtVersion();
	}

// Initialiser for test suite.
// This creates all the test steps and stores them
// inside CTestSuite
void CTSUMmTsthSuite20::InitialiseL()
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMTSTH20");

	// add test steps
	AddTestStepL(RTestMmTsthU2001::NewL());
	AddTestStepL(RTestMmTsthU2002::NewL());

	}
