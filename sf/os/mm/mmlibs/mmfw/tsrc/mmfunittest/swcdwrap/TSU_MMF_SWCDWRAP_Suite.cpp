// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the main DLL entry point for the TSU_MMF_SWCDWRAP.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_SWCDWRAP_Suite.h"
#include "TSU_MMF_SWCDWRAP_Step.h"
#include "TSU_MMF_SWCDWRAP_StepStartDecode.h"
#include "TSU_MMF_SWCDWRAP_StepStartEncode.h"
#include "TSU_MMF_SWCDWRAP_StepStartConvert.h"


/**
 *
 * Create an instance of this test suite.
 *
 * NewTestSuiteL is exported at ordinal 1.
 * This provides the interface to allow the Test Framework
 * to create instances of this test suite.
 *
 * @return	"CTestSuite_MMF_SWCDWRAP*"
 *			The newly created test suite
 */
EXPORT_C CTestSuite_MMF_SWCDWRAP* NewTestSuiteL() 
    { 
	CTestSuite_MMF_SWCDWRAP* result = new (ELeave) CTestSuite_MMF_SWCDWRAP;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }


CTestSuite_MMF_SWCDWRAP::~CTestSuite_MMF_SWCDWRAP()
	{
	}


/**
 *
 * Get test suite version.
 *
 * @return	"TPtrC"
 *			The version string.
 *
 */
TPtrC CTestSuite_MMF_SWCDWRAP::GetVersion( void ) const
	{
	_LIT(KTxtVersion,"1.6");
	return KTxtVersion();
	}

/**
 *
 * Initialiser for test suite.
 * This creates all the test steps and stores them
 * inside CTestSuiteSWCDWRAPasses
 *
 * @xxxx
 * 
 */
void CTestSuite_MMF_SWCDWRAP::InitialiseL( void )
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_SWCDWRAP");

	// General tests
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0001 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0002 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0003 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0004 );

	//play tests
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0010 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0011 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0012 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0013 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0014 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0015 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0016 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0017 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0018 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0019 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0020 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0021 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0040 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0040 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0041 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0042 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0043 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0044 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0045 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0046 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0047 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0048 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0049 );
	// call new methods StopAndDeleteCodec() and DeleteCodec()
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0050 );
	
	//record tests
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0100 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0101 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0102 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0103 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0104 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0105 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0106 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0140 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0141 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0142 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0143 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0144 );

	//convert tests
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0200 );
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0240 );

	//OOM test
	AddTestStepL( new(ELeave) CTestStep_MMF_SWCDWRAP_U_0300 );

	}


// -------------------------
