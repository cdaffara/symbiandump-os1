
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
// This main DLL entry point for the TSU_MMF_AFMT.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_AFMTSuite.h"
#include "TSU_MMF_AFMT.h"
#include "TSU_MMF_AFMT_TestSteps.h"


/** @xxxx
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite_MMF_AFMT* NewTestSuiteL() 
    { 
	CTestSuite_MMF_AFMT* result = new (ELeave) CTestSuite_MMF_AFMT;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

//destructor
CTestSuite_MMF_AFMT::~CTestSuite_MMF_AFMT()
	{
	}

//Get Test Suite version
_LIT(KTxtVersion,"0.1");
TPtrC CTestSuite_MMF_AFMT::GetVersion( void )
	{
	return KTxtVersion();
	}

/** @xxxx
 * Add a test step into the suite
 * @param	"CTestStep_MMF_AFMT* aPtrTestStep" 
 *			Test step pointer
 */
void CTestSuite_MMF_AFMT::AddTestStepL( CTestStep_MMF_AFMT* aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this);
	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}


//------------------------------------------------------------------

/** @xxxx 
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestSuite_MMF_AFMT 
 */
void CTestSuite_MMF_AFMT::InitialiseL( void )
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_AFMT");

	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0001() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0002() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0003() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0004() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0016() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0018() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0020() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0025() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0027() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0028() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0029() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0030() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0031() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0032() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0033() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0036() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0037() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0038() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0042() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0043() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0044() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0045() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0056() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0058() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0065() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0067() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0069() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0074() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0075() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0076() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0077() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0078() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0079() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0082() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0083() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0084() );

	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0090() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0091() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0092() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0093() );

	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0105() );

	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0111() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0113() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0114() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0115() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0116() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0119() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0125() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0126() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0127() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0128() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0139() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0145() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0149() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0150() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0151() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0152() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0153() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0154() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0165() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0189() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0194() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0195() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0196() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0197() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0198() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0199() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0203() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0204() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0205() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0209() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0223() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0232() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0241() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0242() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0243() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0244() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0245() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0246() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0257() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0266() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0276() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0284() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0294() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0302() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0315() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0317() );
	AddTestStepL( new(ELeave) CTest_MMF_AFMT_U_0319() );

	}


