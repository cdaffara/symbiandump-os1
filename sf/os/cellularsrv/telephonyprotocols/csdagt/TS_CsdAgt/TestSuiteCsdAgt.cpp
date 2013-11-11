// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This main DLL entry point for the TS_CsdAgt.dll
// 
//

// EPOC includes
#include <e32base.h>
#include <nd_err.h>

// Test system includes
#include "log.h"
#include "teststep.h"
#include "TestStepCsdAgt.h"
#include "testsuite.h"
#include "TestSuiteCsdAgt.h"

// Test Cases
#include "CsdAgtTestCommDBParameters.h"
#include "CsdAgtTestStateMachineInit.h"
#include "CsdAgtTestReconnect.h"
#include "CsdAgtTestOOM.h"
#include "CsdAgtTestScriptErr.h"


/**
 * New Test CsdAgt is exported at ordinal 1.
 * This provides the interface to allow schedule test to create 
 * instances of this test suite.
 *
 */
EXPORT_C CTestSuiteCsdAgt* NewTestSuiteCsdAgt( void ) 
	{
	return new (ELeave) CTestSuiteCsdAgt;
	}

/**
 * Destructor.
 *
 */
CTestSuiteCsdAgt::~CTestSuiteCsdAgt()
	{
	}

/**
 * Add a test step into the suite.
 *
 */
void CTestSuiteCsdAgt::AddTestStepL( CTestStepCsdAgt * aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->iCsdAgtSuite = this; 

	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}

/**
 * Constructor for TS_CsdAgt test suite.
 * This creates all the TS_CsdAgt test steps and stores them inside CTestSuiteCsdAgt.
 *
 */
void CTestSuiteCsdAgt::InitialiseL( void )
	{
	TInt ret = User::LoadPhysicalDevice( PDD_NAME );
	if ( KErrNone != ret && KErrAlreadyExists != ret )
		{
		User::Leave( ret );
		}
	ret = User::LoadLogicalDevice( LDD_NAME );
	if ( KErrNone != ret && KErrAlreadyExists != ret )
		{
		User::Leave( ret );
		}
 	// When bootstrapping C32 we have to avoid starting both the normal ETEL (because we use a 
	// dummy one of the same server name) and PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KTelephonyCMIs, "phbsync.cmi,etel.cmi");
    ret = StartC32WithCMISuppressions(KTelephonyCMIs);
	if ( KErrNone != ret && KErrAlreadyExists != ret )
		{
		User::Leave( ret );
		}

	// CommDB tests
	AddTestStepL( new(ELeave) CCsdAgtTest1_1 );
	AddTestStepL( new(ELeave) CCsdAgtTest1_2 );
	AddTestStepL( new(ELeave) CCsdAgtTest1_3 );

	// State machine init
	AddTestStepL( new(ELeave) CCsdAgtTest2_1 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_2 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_3 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_4 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_5 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_6 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_7 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_8 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_9 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_10 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_11 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_12 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_13 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_14 );
	AddTestStepL( new(ELeave) CCsdAgtTest2_15 );

	// Reconnect
	AddTestStepL( new(ELeave) CCsdAgtTest3_1 );
	AddTestStepL( new(ELeave) CCsdAgtTest3_2 );

	// OOM
	AddTestStepL( new(ELeave) CCsdAgtTest4_1 );

	// Script processing errors
	AddTestStepL( new(ELeave) CCsdAgtTest5_1 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_2 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_3 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_4 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_5 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_6 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_7 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_8 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_9 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_10 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_11 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_12 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_13 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_14 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_15 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_16 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_17 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_18 );
	AddTestStepL( new(ELeave) CCsdAgtTest5_19 );
	}

TPtrC CTestSuiteCsdAgt::GetVersion( )
	{	
	_LIT(KVersion,"1.1"); // updated for addition of script error tests.
	return KVersion();
	}
