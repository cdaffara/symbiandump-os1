// Copyright (c) 2105-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file lbsrootapiTest21Step.cpp
*/
#include "lbsrootapitest21step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"



CLbsRootApiTest21Step::~CLbsRootApiTest21Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest21Step::CLbsRootApiTest21Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest21Step);
	}

TVerdict CLbsRootApiTest21Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest21Step"));

__UHEAP_MARK;

	// Closedown LbsRoot before changing settings
	iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
	iLbsRootProcessApi.RequestCompleteCloseDown();

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL(21);

	const TSecurityPolicy KReadPolicy(ECapabilityLocation);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	TRAPD(err,RProperty::Define(KTestLbsResetFunctionCalledKey, 
	                                           RProperty::EInt, 
	                                               KReadPolicy, 
	                                             KWritePolicy));
    if(err != KErrNone)
        {
        User::LeaveIfError(err);
        }
    
    iResetFunctionCalled.Attach(KLbsTestProcessUid, KTestLbsResetFunctionCalledKey);
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest21Step::CheckProcessesAreUp()
	{
	RProcess process;
	TInt err = process.Open(KNrhProcessName);
	if(err)
		{
		INFO_PRINTF1(_L("CLbsRootApiTest3Step : ***ERROR - NRH not found"));
		SetTestStepResult(EFail);
		}
	process.Close();
    err = process.Open(KAgpsProcessName);
    if(err)
        {
        INFO_PRINTF1(_L("CLbsRootApiTest3Step : ***ERROR - GPS LOC Manager not found"));
		SetTestStepResult(EFail);
        }
	process.Close();
    err = process.Open(KNgProcessName);
    if(err)
        {
	    INFO_PRINTF1(_L("CLbsRootApiTest3Step : ***ERROR - Network Gateway not found"));
		SetTestStepResult(EFail);
        }
	process.Close();	
	}

TVerdict CLbsRootApiTest21Step::doTestStepL()
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
		
		if(err != ELbsStartupSuccess)
			{
			SetTestStepResult(EFail);
			}
		CheckProcessesAreUp();
		
		if(TestStepResult() == EPass)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest21Step : Processes up OK"));    	
			}
			
		TRequestStatus status;
		iResetFunctionCalled.Set(0);
		iResetFunctionCalled.Subscribe(status);
		iLbsRootProcessApi.RequestSystemReset();
		User::WaitForRequest(status);


    	// Check the status value indicates that the ResetProcess was
    	// seen by the dummy NRH.
    	TInt resetCalledUid;
    	iResetFunctionCalled.Get(resetCalledUid);
    	if(resetCalledUid != KLbsNetRequestHandlerUid.iUid)
			{
			INFO_PRINTF1(_L("****ERROR: Expected process did not receive reset command"));
			SetTestStepResult(EFail);
			}
				
		if(TestStepResult() == EPass)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest21Step : Processes still up OK"));    	
			}

		INFO_PRINTF1(_L("Check processes are still up"));
		CheckProcessesAreUp();
		
		err = iLbsRootProcessApi.RequestSystemCloseDown();
		if(err != ELbsCloseDownComplete)
			{
			INFO_PRINTF1(_L("****ERROR: Closedown failed after reset"));
			SetTestStepResult(EFail);			
			}
			
		}		
		
	return TestStepResult();
	}

TVerdict CLbsRootApiTest21Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest21Step"));

    iLbsRootProcessApi.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

