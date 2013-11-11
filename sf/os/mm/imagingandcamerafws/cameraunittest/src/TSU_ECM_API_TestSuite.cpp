
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
// This main DLL entry point for the TSU_ECM_API_TestSuite
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_ECM_API_TestSuite.h"

#include "ECamInfoTest.h"
#include "ECamTest.h"
#include "ecamreservetest.h"


/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite_ECam_API* NewTestSuiteL() 
    { 
	CTestSuite_ECam_API* self = new(ELeave) CTestSuite_ECam_API;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self); 
	return self;
    }

/**
 * Destructor
 */
CTestSuite_ECam_API::~CTestSuite_ECam_API()
	{
	}

/**
 * Get Test Suite version
 */
TPtrC CTestSuite_ECam_API::GetVersion()
	{
	_LIT(KTxtVersion, "1.0");
	return KTxtVersion();
	}

/**
 * This function initialises the test suite
 * this creates all the test steps and stores them inside CTestSuite_ECam_API 
 */
void CTestSuite_ECam_API::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_ECM_API");

	User::LeaveIfError(ifsSession.Connect());
#ifdef SYMBIAN_MULTIMEDIA_THREEPLANEARCH
	ifsSession.Delete(KMMCameraPluginName);
#endif //SYMBIAN_MULTIMEDIA_THREEPLANEARCH

	ifsSession.Delete(KTestCameraPluginName);	
	ifsSession.Close();

	AddTestStepL(RECamInfoTest::NewL(EFalse));
	AddTestStepL(RECamInnerInfoTest::NewL(EFalse));

	AddTestStepL(RECamTest::NewL(EFalse));
	AddTestStepL(RECamInnerTest::NewL(EFalse));
	
	//Reserve Subscription Test
	AddTestStepL(RECamReserveTest::NewL(EFalse));
	}

