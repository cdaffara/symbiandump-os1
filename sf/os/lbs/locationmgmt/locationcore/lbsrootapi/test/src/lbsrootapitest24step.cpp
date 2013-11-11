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
 @file lbsrootapiTest24Step.cpp
*/
#include "lbsrootapitest24step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"





CLbsRootApiTest24Step::~CLbsRootApiTest24Step()
/**
 * Destructor
 */
	{
	iPanicRequest.Close();
	}

CLbsRootApiTest24Step::CLbsRootApiTest24Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest24Step);
	}

TVerdict CLbsRootApiTest24Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest24Step"));

	__UHEAP_MARK;

	iLbsRootProcessApi.OpenL(KLbsTestProcessUid);

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL(24);

	const TSecurityPolicy KReadPolicy(ECapabilityLocation);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	TRAPD(err,RProperty::Define(KTestLbsPanicRequestKey, 
	                                           RProperty::EInt, 
	                                               KReadPolicy, 
	                                             KWritePolicy));
    if(err != KErrNone)
        {
        User::LeaveIfError(err);
        }

	iPanicRequest.Attach(KLbsTestProcessUid, KTestLbsPanicRequestKey);
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CLbsRootApiTest24Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		// Test to check that the startup request will correctly
		// report if LbsRoot is not available when a 'client'
		// makes a startup request 
		
		// First, shutdown and kill the root process, (using
		// the test-only full shutdown api)
		
		INFO_PRINTF1(_L("CLbsRootApiTest24Step : RequestCompleteCloseDown"));
		
		TInt err = iLbsRootProcessApi.RequestCompleteCloseDown();
		TESTL(err == ELbsCloseDownComplete);

		// Now try to start the Lbs system again;
		// this should fail and report that root could not be found.
		
		INFO_PRINTF1(_L("CLbsRootApiTest24Step : RequestSystemStartup"));
		
		err = iLbsRootProcessApi.RequestSystemStartup();
		if (err != ELbsStartupRootNotFound)
			{
			return (EFail);
			}	
		}

	return TestStepResult();
	}

TVerdict CLbsRootApiTest24Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest24Step"));

    iLbsRootProcessApi.Close();
	// Restart lbsroot in case it's needed by any subsequent tests.
	StartLbsRootL();

	__UHEAP_MARKEND;

	return TestStepResult();
	}


