// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This main DLL entry point for the TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.dll
// 
//

// System includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.h"
#include "TSU_MMF_DEVSOUND_NOTIFICATION_STEP.h"


/** 
 *
 * NewTestSuiteL
 *	NewTestSuite is exported at ordinal 1
 *	this provides the interface to allow schedule test
 *	to create instances of this test suite
 * @result CTestSuiteDevSoundNotification*
 *
 */
EXPORT_C CTestSuiteDevSoundNotification* NewTestSuiteL() 
	{ 
	CTestSuiteDevSoundNotification* result = new (ELeave) CTestSuiteDevSoundNotification;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
	}

/**
 *
 * ~CTestSuiteDevSoundNotification
 *
 */
CTestSuiteDevSoundNotification::~CTestSuiteDevSoundNotification() 
	{}

/**
 *
 * GetVersion
 *	Get Test Suite version
 * @result TPtrC
 *
 */
TPtrC CTestSuiteDevSoundNotification::GetVersion( void ) const
	{
	_LIT(KTxtVersion,"1.08");
	return KTxtVersion();
	}

/**
 *
 * AddTestStepL
 *	Add a test step into the suite
 * @param aPtrTestStep
 *	Test step pointer
 *
 */
void CTestSuiteDevSoundNotification::AddTestStepL( CTestStepDevSoundNotification* aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this);

	// add the step using the base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}
/**
 *
 * AddTestStepL
 *	Add a test step into the suite
 * @param aPtrTestStep
 *	Test step pointer
 *
 */
void CTestSuiteDevSoundNotification::AddTestStepL( CTestStepFsmDevSoundNotification* aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this);

	// add the step using the base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}
/**
 *
 * InitialiseL
 *	Constructor for test suite
 *	this creates all the test steps and 
 *	stores them inside CTestSuiteDevSoundNotification
 *
 */
void CTestSuiteDevSoundNotification::InitialiseL(void)
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_DEVSOUND_NOTIFICATION_SUITE");
	AddTestStepL(new(ELeave) CTestStepDevSound0040());
	AddTestStepL(new(ELeave) CTestStepDevSound0041());
	AddTestStepL(new(ELeave) CTestStepDevSound0042());
	AddTestStepL(new(ELeave) CTestStepDevSound0043());
	AddTestStepL(new(ELeave) CTestStepDevSound0044());
	AddTestStepL(new(ELeave) CTestStepDevSound0045());
	AddTestStepL(new(ELeave) CTestStepDevSound0046());
	AddTestStepL(new(ELeave) CTestStepDevSound0047());
	AddTestStepL(new(ELeave) CTestStepDevSound0048());
	AddTestStepL(new(ELeave) CTestStepDevSound0049());
	AddTestStepL(new(ELeave) CTestStepDevSound0050());
	AddTestStepL(new(ELeave) CTestStepDevSound0051());
	}
	
