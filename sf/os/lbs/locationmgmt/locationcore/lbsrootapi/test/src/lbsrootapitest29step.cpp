// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file lbsrootapiTest29Step.cpp
*/
#include "lbscommon.h" 
#include "LbsInternalInterface.h" 
#include "lbsrootapitest29step.h"
#include "te_lbsrootapiTestSuiteDefs.h"
#include "lbsrootapi.h"
#include "lbsqualityprofile.h"



CLbsRootApiTest29Step::~CLbsRootApiTest29Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest29Step::CLbsRootApiTest29Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest29Step);
	}

TVerdict CLbsRootApiTest29Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest29Step"));

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL();
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest29Step::DoMainL()
{
}

TVerdict CLbsRootApiTest29Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TInt retVal;
	if (TestStepResult()==EPass)
		{

		INFO_PRINTF1(_L("CLbsRootApiTest29Step : open RLbsSystemController"));

		RLbsSystemController lbsRootProcessApi;
		lbsRootProcessApi.OpenL(KLbsTestProcessUid);
		CleanupClosePushL(lbsRootProcessApi);
		lbsRootProcessApi.RequestCompleteCloseDown();

		INFO_PRINTF1(_L("CLbsRootApiTest29Step : Request SystemStartup "));
		// For this test, we expect startup to fail in debug builds
		// but startup OK in UREL
		retVal = lbsRootProcessApi.RequestSystemStartup();
#ifdef _DEBUG
		if(retVal == ELbsStartupSuccess)
			{
			SetTestStepResult(EFail);
			}
#else
		if(retVal != ELbsStartupSuccess)
			{
			SetTestStepResult(EFail);
			}
#endif
			
		//INFO_PRINTF1(_L("CLbsRootApiTest29Step : close system down"));
		//lbsRootProcessApi.RequestSystemCloseDown();

		CleanupStack::PopAndDestroy(&lbsRootProcessApi);

		}		
		
	return TestStepResult();
	}

TVerdict CLbsRootApiTest29Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest29Step"));
	// process something post setting to the test step

	// Restart lbsroot in case it's needed by any subsequent tests.
	//StartLbsRootL();

	// uncomment the following line if you have post process or remove the following line if no post process
	return TestStepResult();
	}
