
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
// This main DLL entry point for the TSU_MMF_GEF_TestSuite
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_GEF_TestSuite.h"

#include "GlobalEffectTest.h"

/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite_MMF_GEF* NewTestSuiteL() 
    { 
	CTestSuite_MMF_GEF* self = new(ELeave) CTestSuite_MMF_GEF;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self); 
	return self;
    }

/**
 * Destructor
 */
CTestSuite_MMF_GEF::~CTestSuite_MMF_GEF()
	{
	}

/**
 * Get Test Suite version
 */
TPtrC CTestSuite_MMF_GEF::GetVersion()
	{
	_LIT(KTxtVersion, "1.0");
	return KTxtVersion();
	}

/**
 * This function initialises the test suite
 * this creates all the test steps and stores them inside CTestSuite_MMF_GEF 
 */
void CTestSuite_MMF_GEF::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_GEF");
	
	AddTestStepL(RGlobalEffectPrimitiveTest::NewL(EFalse));
	AddTestStepL(RGlobalEffectStdTest::NewL(EFalse));
	AddTestStepL(RGlobalEffectMainTest::NewL(EFalse));
	AddTestStepL(RGlobalEffectCallbackTest::NewL(EFalse));
	}


