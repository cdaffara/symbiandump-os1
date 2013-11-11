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
 @file NetworkRequestHandlerTest0Step.cpp
*/
#include "lbsrootapitest0step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"



CLbsRootApiTest0Step::~CLbsRootApiTest0Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest0Step::CLbsRootApiTest0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest0Step);
	}

TVerdict CLbsRootApiTest0Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest0Step"));
	
	__UHEAP_MARK;
	
	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL();
	
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest0Step::DoMainL()
{
}

TVerdict CLbsRootApiTest0Step::doTestStepL()
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
        
		INFO_PRINTF1(_L("CLbsRootApiTest0Step : RequestSystemStartup"));
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

		INFO_PRINTF1(_L("CLbsRootApiTest0Step : RequestSystemCloseDown"));
		iLbsRootProcessApi.RequestSystemCloseDown();

		iLbsRootProcessApi.Close();

		}		
		
	  return TestStepResult();
	}


TVerdict CLbsRootApiTest0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest0Step"));
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
