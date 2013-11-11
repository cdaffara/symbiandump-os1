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

#include "TSI_MmfCtlfrmSuite.h"
#include "TSI_MmfCtlfrm.h"
#include "TSI_MmfCtlfrmNeg.h"




/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
	CTestMmfCtlfrmSuite* result = new (ELeave) CTestMmfCtlfrmSuite;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 * destructor
 */
CTestMmfCtlfrmSuite::~CTestMmfCtlfrmSuite()
	{
	}

/**
 * make a version string available for test system
 * @return version string
 */
TPtrC CTestMmfCtlfrmSuite::GetVersion() const
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestMmfCtlfrmSuite 
 */
void CTestMmfCtlfrmSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSIMmfCtlfrm");

	AddTestStepL(RTestMmfCtlfrmI0001::NewL());
	AddTestStepL(RTestMmfCtlfrmI0002::NewL());
	AddTestStepL(RTestMmfCtlfrmI0003::NewL());

	AddTestStepL(RTestMmfCtlfrmI0102::NewL());
	AddTestStepL(RTestMmfCtlfrmI0105::NewL());
	AddTestStepL(RTestMmfCtlfrmI0106::NewL());
	AddTestStepL(RTestMmfCtlfrmI0107::NewL());
	AddTestStepL(RTestMmfCtlfrmI0108::NewL());
	AddTestStepL(RTestMmfCtlfrmI0109::NewL());

	AddTestStepL(RTestMmfCtlfrmI0111::NewL());
	AddTestStepL(RTestMmfCtlfrmI0112::NewL());
	AddTestStepL(RTestMmfCtlfrmI0113::NewL());
	AddTestStepL(RTestMmfCtlfrmI0114::NewL());
	AddTestStepL(RTestMmfCtlfrmI0117::NewL());
	AddTestStepL(RTestMmfCtlfrmI0118::NewL());
	AddTestStepL(RTestMmfCtlfrmI0119::NewL());
	AddTestStepL(RTestMmfCtlfrmI0120::NewL());

	AddTestStepL(RTestMmfCtlfrmI0121::NewL());
	AddTestStepL(RTestMmfCtlfrmI0122::NewL());
	AddTestStepL(RTestMmfCtlfrmI0123::NewL());
	AddTestStepL(RTestMmfCtlfrmI0124::NewL());
	AddTestStepL(RTestMmfCtlfrmI0125::NewL());
	AddTestStepL(RTestMmfCtlfrmI0128::NewL());
	AddTestStepL(RTestMmfCtlfrmI0129::NewL());

	AddTestStepL(RTestMmfCtlfrmI0131::NewL());
	AddTestStepL(RTestMmfCtlfrmI0132::NewL());

	AddTestStepL(RTestMmfCtlfrmI0141::NewL());
	AddTestStepL(RTestMmfCtlfrmI0142::NewL());
	AddTestStepL(RTestMmfCtlfrmI0143::NewL());
	AddTestStepL(RTestMmfCtlfrmI0144::NewL());

	AddTestStepL(RTestMmfCtlfrmI0151::NewL());
	AddTestStepL(RTestMmfCtlfrmI0152::NewL());

	// negative testing
	AddTestStepL(RTestMmfCtlfrmI0502::NewL());
	AddTestStepL(RTestMmfCtlfrmI0504::NewL());

	AddTestStepL(RTestMmfCtlfrmI0511::NewL());
	AddTestStepL(RTestMmfCtlfrmI0512::NewL());
	AddTestStepL(RTestMmfCtlfrmI0513::NewL());
	AddTestStepL(RTestMmfCtlfrmI0514::NewL());
	AddTestStepL(RTestMmfCtlfrmI0515::NewL());

	AddTestStepL(RTestMmfCtlfrmI0521::NewL());
	AddTestStepL(RTestMmfCtlfrmI0522::NewL());
	AddTestStepL(RTestMmfCtlfrmI0523::NewL());
	AddTestStepL(RTestMmfCtlfrmI0524::NewL());
	AddTestStepL(RTestMmfCtlfrmI0525::NewL());
	AddTestStepL(RTestMmfCtlfrmI0526::NewL());
	AddTestStepL(RTestMmfCtlfrmI0527::NewL());

	AddTestStepL(RTestMmfCtlfrmI0531::NewL());
	AddTestStepL(RTestMmfCtlfrmI0532::NewL());

	AddTestStepL(RTestMmfCtlfrmI0541::NewL());
	AddTestStepL(RTestMmfCtlfrmI0542::NewL());

	AddTestStepL(RTestMmfCtlfrmI0551::NewL());
	AddTestStepL(RTestMmfCtlfrmI0552::NewL());
	AddTestStepL(RTestMmfCtlfrmI0553::NewL());
	AddTestStepL(RTestMmfCtlfrmI0554::NewL());
	}


