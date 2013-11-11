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
 @file lbsrootapiTest20Step.cpp
*/
#include "lbsrootapitest20step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"



CLbsRootApiTest20Step::~CLbsRootApiTest20Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest20Step::CLbsRootApiTest20Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest20Step);
	}

TVerdict CLbsRootApiTest20Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest20Step"));

__UHEAP_MARK;

	// Closedown LbsRoot before changing settings
	iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
	iLbsRootProcessApi.RequestCompleteCloseDown();

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL(20);

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

TVerdict CLbsRootApiTest20Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{

	if (TestStepResult()==EPass)
		{
		TInt err = iLbsRootProcessApi.RequestSystemStartup();	
		
		if(err != ELbsStartupFail)
			{        		
			SetTestStepResult(EFail);
			}

		// Check none of the processes is active. 	
    
		INFO_PRINTF1(_L("CLbsRootApiTest20Step : Check no processes are still active..."));

		TFindThread* findThread=new TFindThread(_L("t_lbsrootapi_lbs*"));
		CleanupStack::PushL(findThread);
		TFullName* result=new TFullName;
		CleanupStack::PushL(result);

		TInt retVal = findThread->Next(*result);
		if(retVal != KErrNotFound)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest20Step : ERROR: Process not all stopped"));		
			SetTestStepResult(EFail);		
			}
		CleanupStack::PopAndDestroy(result);
		CleanupStack::PopAndDestroy(findThread);
		}

	  return TestStepResult();
	}

TVerdict CLbsRootApiTest20Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest20Step"));

    iLbsRootProcessApi.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

