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
// TSIMmfCtlfrmSuite.cpp
// 
//


// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

#include "TSI_MmfRecogSuite.h"
#include "TSI_MmfRecog.h"



/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
	CTestMmfRecogSuite* result = new (ELeave) CTestMmfRecogSuite;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 * destructor
 */
CTestMmfRecogSuite::~CTestMmfRecogSuite()
	{
	}

/**
 * make a version string available for test system
 * @return version string
 */
TPtrC CTestMmfRecogSuite::GetVersion() const
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestMmfRecogSuite 
 */
void CTestMmfRecogSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSIMmfRecog");

	// positive step
	AddTestStepL(RTestMmfRecog::NewL(_L("MM-MMF-RECOG-I-0001-CP")));
	
	// negative test
	AddTestStepL(RTestMmfRecogNeg::NewL(_L("MM-MMF-RECOG-I-0002-CP")));

	// negative test #2
	AddTestStepL(RTestMmfRecogNegDirect::NewL(_L("MM-MMF-RECOG-I-0003-CP")));

	// recognition test
	AddTestStepL(RTestMmfRecogData::NewL(_L("MM-MMF-RECOG-I-0004-CP")));
	
	//test for recognizing the format
	AddTestStepL(RTestMmfFormat::NewL(_L("MM-MMF-RECOG-I-0005-CP")));

	// alloc step
	AddTestStepL(RTestMmfRecogAlloc::NewL(_L("MM-MMF-RECOG-I-0101-CP")));
	
	AddTestStepL(RTestMmfRecogTestFileHandle::NewL(_L("MM-MMF-RECOG-I-0010-CP")));
	}


