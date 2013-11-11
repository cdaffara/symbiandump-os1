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
// Example CTestStep derived implementation
// 
//

/**
 @file NetworkRequestHandlerTest25Step.cpp
*/
#include "lbsrootapitest25step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"



CLbsRootApiTest25Step::~CLbsRootApiTest25Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest25Step::CLbsRootApiTest25Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest25Step);
	}

TVerdict CLbsRootApiTest25Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest25Step"));
	
	__UHEAP_MARK;
	
	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL();
	
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest25Step::DoMainL()
{
}

TVerdict CLbsRootApiTest25Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TInt err;	
	if (TestStepResult()==EPass)
		{
		iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
        
		INFO_PRINTF1(_L("CLbsRootApiTest25Step : RequestSystemStartup"));
		err = iLbsRootProcessApi.RequestSystemStartup();
		if(err != ELbsStartupSuccess)
			{
			SetTestStepResult(EFail);			
			}

		RProcess proc;
		err = proc.Open(KNrhProcessName);
		if(err)
			{
			SetTestStepResult(EFail);
			}
		proc.Close();
		err = proc.Open(KAgpsProcessName);
		if(err)
			{
			SetTestStepResult(EFail);
			}
		proc.Close();
		err = proc.Open(KNgProcessName);
		if(err)
			{
			SetTestStepResult(EFail);
			}
		proc.Close();

		// Now get rid of the LBS Root Process completely.
		err = iLbsRootProcessApi.RequestCompleteCloseDown();
		if (err != ELbsCloseDownComplete)
			{
			INFO_PRINTF1(_L("***ERROR: expected ELbsCloseDownComplete from complete shutdown attempt"));
			SetTestStepResult(EFail);			
			}
		iLbsRootProcessApi.Close();
		
		// Leave a time delay to give root the chance to shutdown
		User::After(5000000);
		
		// Now try opening it up again.
		iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
        
		INFO_PRINTF1(_L("CLbsRootApiTest25Step : RequestSystemStartup again"));
		err = iLbsRootProcessApi.RequestSystemStartup();
		if(err != KErrNone)
			{
			INFO_PRINTF1(_L("***ERROR: expected KErrNone from startup attempt"));
			SetTestStepResult(EFail);
			}
		}		
		
	  return TestStepResult();
	}


TVerdict CLbsRootApiTest25Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest25Step"));
	
	iLbsRootProcessApi.Close();
	// Restart lbsroot in case it's needed by any subsequent tests.
	StartLbsRootL();


	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
