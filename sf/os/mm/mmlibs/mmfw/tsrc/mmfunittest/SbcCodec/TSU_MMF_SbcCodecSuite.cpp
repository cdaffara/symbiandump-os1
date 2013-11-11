
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This main DLL entry point for the TSU_MMF_SBCCODEC.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_SbcCodecSuite.h"
#include "TSU_MMF_SbcCodec.h"


/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite_MMF_SbcCodec* NewTestSuiteL() 
    { 
	CTestSuite_MMF_SbcCodec* self = new(ELeave) CTestSuite_MMF_SbcCodec;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // result
	return self;
    }

/**
 * Destructor
 */
CTestSuite_MMF_SbcCodec::~CTestSuite_MMF_SbcCodec()
	{
	}

/**
 * Get Test Suite version
 */
TPtrC CTestSuite_MMF_SbcCodec::GetVersion()
	{
	_LIT(KTxtVersion, "1.0");
	return KTxtVersion();
	}

/**
 * This function initialises the test suite
 * this creates all the test steps and stores them inside CTestSuite_MMF_SbcCodec 
 */
void CTestSuite_MMF_SbcCodec::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_SbcCodec");

	// NewL() tests
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_001);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_002);
	
	// ProcessL() tests
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_010);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_011);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_012);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_013);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_014);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_015);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_016);
	
	// Memory tests
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_021);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_022);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_023);
	AddTestStepL(new(ELeave) CTest_MMF_SbcCodec_U_024);
	}


