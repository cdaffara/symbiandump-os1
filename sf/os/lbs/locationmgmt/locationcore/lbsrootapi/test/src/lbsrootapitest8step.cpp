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
 @file lbsrootapiTest8Step.cpp
*/
#include "lbsrootapitest8step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"



CLbsRootApiTest8Step::~CLbsRootApiTest8Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest8Step::CLbsRootApiTest8Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest8Step);
	}

TVerdict CLbsRootApiTest8Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest8Step"));

__UHEAP_MARK;

	const TSecurityPolicy KReadPolicy(ECapabilityLocation);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	TRAPD(err,RProperty::Define(KTestLbsSecondCloseDownRequestKey, 
	                                           RProperty::EInt, 
	                                               KReadPolicy, 
	                                             KWritePolicy));
    if(err != KErrNone)
        {
        User::LeaveIfError(err);
        }
    
    iSecondCloseDownRequest.Attach(KLbsTestProcessUid, KTestLbsSecondCloseDownRequestKey);
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CLbsRootApiTest8Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	// Shouldn't get anywhere, because the LBS Root API relies on the cenrep 
	// file being there to start
 	TRAPD(err, iLbsRootProcessApi.OpenL(KLbsTestProcessUid));
	if(err != KErrNotFound)
		{
 		SetTestStepResult(EFail);		
		}
	return TestStepResult();
	}

TVerdict CLbsRootApiTest8Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest8Step"));

    iLbsRootProcessApi.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

