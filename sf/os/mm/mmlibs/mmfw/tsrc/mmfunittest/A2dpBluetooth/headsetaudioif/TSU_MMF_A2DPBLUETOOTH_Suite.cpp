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
// This is the main DLL entry point for the TSU_MMF_A2DPBLUETOOTH.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_A2DPBLUETOOTH_Suite.h"
#include "TSU_MMF_A2DPBLUETOOTH_Step.h"
#include "TSU_MMF_A2DPBLUETOOTH_Initialize.h"
#include "TSU_MMF_A2DPBLUETOOTH_Settings.h"
#include "TSU_MMF_A2DPBLUETOOTH_Open.h"
#include "TSU_MMF_A2DPBLUETOOTH_Play.h"
#include "TSU_MMF_A2DPBLUETOOTH_OOM.h"


/**
 *
 * Create an instance of this test suite.
 *
 * NewTestSuiteL is exported at ordinal 1.
 * This provides the interface to allow the Test Framework
 * to create instances of this test suite.
 *
 * @return	"CTestSuite_MMF_A2DPBLUETOOTH*"
 *			The newly created test suite
 */
EXPORT_C CTestSuite_MMF_A2DPBLUETOOTH* NewTestSuiteL() 
    { 
	CTestSuite_MMF_A2DPBLUETOOTH* result = new (ELeave) CTestSuite_MMF_A2DPBLUETOOTH;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }


CTestSuite_MMF_A2DPBLUETOOTH::~CTestSuite_MMF_A2DPBLUETOOTH()
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
TPtrC CTestSuite_MMF_A2DPBLUETOOTH::GetVersion( void ) const
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
 *
 * Initialiser for test suite.
 * This creates all the test steps and stores them
 * inside CTestSuiteA2DPBLUETOOTH harness
 * 
 */
void CTestSuite_MMF_A2DPBLUETOOTH::InitialiseL( void )
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_A2DPBLUETOOTH");

	// General tests
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0001 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0002 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0003 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0004 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0005 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0006 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0007 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0050 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0100 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0101 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0102 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0103 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0104 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0105 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0200 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0201 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0202 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0203 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0204 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0230 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0231 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0301 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0302 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0303 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0304 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0305 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0306 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0307 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0308 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0309 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0310 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0311 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0312 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0313 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0314 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0315 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0316 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0320 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0330 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0331 );
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0350 );
	//OOM test
	AddTestStepL( new(ELeave) CTestStep_MMF_A2DPBLUETOOTH_U_0400 );

	}


// -------------------------
