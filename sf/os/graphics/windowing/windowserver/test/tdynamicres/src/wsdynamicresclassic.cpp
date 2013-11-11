// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TDISPLAYCONFIGURATION.CPP
// Test class for TDisplayConfiguration
// 
//

#include "wsdynamicresclassic.h"
#include <test/extendtef.h>
#include "teflogextensions.h"

struct TLoggerStub
{
	TLoggerStub(CTestExecuteLogger& aLogger)
	:	iLogger(&aLogger)	{}
	CTestExecuteLogger* iLogger;
	CTestExecuteLogger& operator()()const
		{
		return *iLogger;
		}
};

// This handles any non-member uses of the extended ASSERT_XXX macros
void TefUnitFailLeaveL()
	{
	
	User::Leave(KErrTEFUnitFail);
	}

// Create a suite of all the tests
CTestSuite* CWsDynamicResClassic::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE_OPT(CWsDynamicResClassic,NULL);

		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0100L);

	END_SUITE;	
	}

// Published Tests

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0100L
@SYMTestCaseDesc		GetInterface in classic
@SYMREQ					
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			GetInterface should always return NULL in classic
@SYMTestActions			
	Create an RWsSession
	Create a CWsScreenDevice
	Call GetInterface with various GUIDS.
@SYMTestExpectedResults	
	Should always return NULL
**/
void CWsDynamicResClassic::GRAPHICS_WSERV_DYNAMICRES_0100L()
	{
	RWsSession session;
	session.Connect();

	CWsScreenDevice* screenDevice = NULL;

	TRAPD(err, screenDevice = new (ELeave) CWsScreenDevice(session));
	PRINT_ON_ERROR2_L(err, _L("Failed to create screen device: %d"), err);
	if (screenDevice)
		{
		ASSERT_EQUALS_X(screenDevice->Construct(0), KErrNone);
	
		void* interface = screenDevice->GetInterface(12344321);
		ASSERT_NULL (interface);
		interface = screenDevice->GetInterface(0);
		ASSERT_NULL (interface);
		}

	delete screenDevice;
	session.Close();
	}

