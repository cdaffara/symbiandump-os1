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
// TestSuiteEsock.cpp
// This main DLL entry point for the TS_Esock.dll
// 
//



// EPOC includes
#include <e32base.h>

// Test system includes
#include "networking/log.h"
#include "networking/teststep.h"
#include "TestStepPsdAgt.h"
#include "networking/testsuite.h"
#include "TestSuitePsdAgt.h"

// Test Cases
#include "Psdagttestcommdbparameters.h"
#include "Psdagttestcontextconfigchange.h"
#include "PsdAgtTestQoSChange.h"
#include "PsdAgtTestNotificationDisabled.h"
#include "PsdAgtTestLogging.h"
#include "PsdAgtTestSMInitialisation.h"
#include "PsdAgtTestNetworkCheck.h"
#include "PsdAgtTestCreateContext.h"
#include "PsdAgtTestCreateQoS.h"
#include "PsdAgtTestActivateContext.h"
#include "PsdAgtTestOpen.h"
#include "PsdAgtTestCloseLog.h"
#include "PsdAgtTestClosure.h"
#include "PsdAgtTestReconnect.h"
#include "PsdAgtTestOOM.h"



// NewTestEtelPacket is exported at ordinal 1
// this provides the interface to allow schedule test
// to create instances of this test suite
EXPORT_C CTestSuitePsdAgt* NewTestSuitePsdAgt( void ) 
    { 
	return new (ELeave) CTestSuitePsdAgt;
    }

// destructor
CTestSuitePsdAgt::~CTestSuitePsdAgt()
	{
	}

// Add a test step into the suite
void CTestSuitePsdAgt::AddTestStepL( CTestStepPsdAgt * ptrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	ptrTestStep->iPsdAgtSuite = this; 

	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(ptrTestStep);
	}


// constructor for ESOCK test suite
// this creates all the ESOCK test steps and
// stores them inside CTestSuiteEsock
void CTestSuitePsdAgt::InitialiseL( void )
	{

	// Start the Comms Server - the Database Server thread runs in this process
 	// When bootstrapping C32 we have to avoid starting both the normal ETEL (because we use a 
	// dummy one of the same server name) and PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KTelephonyCMIs, "phbsync.cmi,etel.cmi");
    TInt ret = StartC32WithCMISuppressions(KTelephonyCMIs);
	if (!((ret==KErrNone)||(ret==KErrAlreadyExists)))
		{
 		_LIT(KPanicFailedC32,"C32 Failed to start");
 		User::Panic(KPanicFailedC32, ret);
		}

	// add test steps
	AddTestStepL( new(ELeave) CPsdAgtTest1_1 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_2 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_3 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_4 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_5 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_6 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_7 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_8 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_9 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_10 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_11 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_12 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_13 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_14 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_15 );
	AddTestStepL( new(ELeave) CPsdAgtTest1_16 );

 	AddTestStepL( new(ELeave) CPsdAgtTest2_1 );
	AddTestStepL( new(ELeave) CPsdAgtTest2_2 );
	AddTestStepL( new(ELeave) CPsdAgtTest2_3 );
	AddTestStepL( new(ELeave) CPsdAgtTest2_4 );

	AddTestStepL( new(ELeave) CPsdAgtTest3_1 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_2 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_3 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_4 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_5 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_6 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_7 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_8 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_9 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_10 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_11 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_12 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_13 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_14 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_15 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_16 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_17 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_18 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_19 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_20 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_21 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_22 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_23 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_24 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_25 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_26 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_27 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_28 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_29 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_30 );
	AddTestStepL( new(ELeave) CPsdAgtTest3_31 );

	AddTestStepL( new(ELeave) CPsdAgtTest4_1);
	
	AddTestStepL( new(ELeave) CPsdAgtTest5_1);
	AddTestStepL( new(ELeave) CPsdAgtTest5_2);
	AddTestStepL( new(ELeave) CPsdAgtTest5_3);

	AddTestStepL( new(ELeave) CPsdAgtTest6_1);
	AddTestStepL( new(ELeave) CPsdAgtTest6_2);
	AddTestStepL( new(ELeave) CPsdAgtTest6_3);
	AddTestStepL( new(ELeave) CPsdAgtTest6_4);
	AddTestStepL( new(ELeave) CPsdAgtTest6_5);
	AddTestStepL( new(ELeave) CPsdAgtTest6_6);
	AddTestStepL( new(ELeave) CPsdAgtTest6_7);
	AddTestStepL( new(ELeave) CPsdAgtTest6_8);
	AddTestStepL( new(ELeave) CPsdAgtTest6_9);
	AddTestStepL( new(ELeave) CPsdAgtTest6_10 );
	AddTestStepL( new(ELeave) CPsdAgtTest6_11 );
	AddTestStepL( new(ELeave) CPsdAgtTest6_12 );


	AddTestStepL( new(ELeave) CPsdAgtTest7_1);
	AddTestStepL( new(ELeave) CPsdAgtTest7_2);
	AddTestStepL( new(ELeave) CPsdAgtTest7_3);
	AddTestStepL( new(ELeave) CPsdAgtTest7_4);
	AddTestStepL( new(ELeave) CPsdAgtTest7_5);
	AddTestStepL( new(ELeave) CPsdAgtTest7_6);

	AddTestStepL( new(ELeave) CPsdAgtTest7_7);
	AddTestStepL( new(ELeave) CPsdAgtTest7_8);
	AddTestStepL( new(ELeave) CPsdAgtTest7_9);
	AddTestStepL( new(ELeave) CPsdAgtTest7_10);
//	AddTestStepL( new(ELeave) CPsdAgtTest7_11);

	AddTestStepL( new(ELeave) CPsdAgtTest8_1);

	AddTestStepL( new(ELeave) CPsdAgtTest9_1);

	AddTestStepL( new(ELeave) CPsdAgtTest10_1);
	AddTestStepL( new(ELeave) CPsdAgtTest10_2);
	AddTestStepL( new(ELeave) CPsdAgtTest10_3);

	AddTestStepL( new(ELeave) CPsdAgtTest11_1);
	AddTestStepL( new(ELeave) CPsdAgtTest11_2);
	AddTestStepL( new(ELeave) CPsdAgtTest11_3);
	AddTestStepL( new(ELeave) CPsdAgtTest11_4);
	AddTestStepL( new(ELeave) CPsdAgtTest11_5);
	AddTestStepL( new(ELeave) CPsdAgtTest11_6);
	AddTestStepL( new(ELeave) CPsdAgtTest11_7);
	AddTestStepL( new(ELeave) CPsdAgtTest11_8);

	AddTestStepL( new(ELeave) CPsdAgtTest12_1);
	
	AddTestStepL( new(ELeave) CPsdAgtTest13_1);
	AddTestStepL( new(ELeave) CPsdAgtTest13_2);

	AddTestStepL( new(ELeave) CPsdAgtTest14_1);
	AddTestStepL( new(ELeave) CPsdAgtTest14_2);
	AddTestStepL( new(ELeave) CPsdAgtTest14_3);
	AddTestStepL( new(ELeave) CPsdAgtTest14_4);
	AddTestStepL( new(ELeave) CPsdAgtTest14_5);

	AddTestStepL( new(ELeave) CPsdAgtTest15_1);
	AddTestStepL( new(ELeave) CPsdAgtTest15_2);
	AddTestStepL( new(ELeave) CPsdAgtTest15_3);
	AddTestStepL( new(ELeave) CPsdAgtTest15_4);
	}

TPtrC CTestSuitePsdAgt::GetVersion( )
	{	
#ifdef _DEBUG
	_LIT(KVersion,"1.001 (udeb)");
#else
	_LIT(KVersion,"1.001");
#endif
	return KVersion();
	}
