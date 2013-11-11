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
 @file lbsrootapiTest19Step.cpp
*/
#include "lbsrootapitest19step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"



CLbsRootApiTest19Step::~CLbsRootApiTest19Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest19Step::CLbsRootApiTest19Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest19Step);
	}

TVerdict CLbsRootApiTest19Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest19Step"));

__UHEAP_MARK;	

	// Closedown LbsRoot before changing settings
	iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
	iLbsRootProcessApi.RequestCompleteCloseDown();

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL(19);

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

TVerdict CLbsRootApiTest19Step::doTestStepL()
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

		// The attempt should fail because the first of the started processes panicked.
		if(err != ELbsStartupFail)
			{        		
			SetTestStepResult(EFail);
			}
       
		// Check none of the processes is active. 	
    
		INFO_PRINTF1(_L("CLbsRootApiTest19Step : Check no processes are still active..."));

		TFindThread* findThread=new TFindThread(_L("t_lbsrootapi_lbs*"));
		TFullName* result=new TFullName;

		TInt retVal = findThread->Next(*result);
		if(retVal != KErrNotFound)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest19Step : ERROR: Process not all stopped"));		
			SetTestStepResult(EFail);		
			}
		delete result;
		delete findThread;
		}		
		
	  return TestStepResult();
	}

TVerdict CLbsRootApiTest19Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest19Step"));

	iLbsRootProcessApi.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

