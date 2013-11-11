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
 @file NetworkRequestHandlerTest1Step.cpp
*/
#include "lbsrootapitest1step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"


CLbsRootApiTest1Step::~CLbsRootApiTest1Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest1Step::CLbsRootApiTest1Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest1Step);
	}

TVerdict CLbsRootApiTest1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest1Step"));

__UHEAP_MARK;

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL();

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

void CLbsRootApiTest1Step::CheckProcessesAreUp()
	{
	RProcess process;
	TInt err = process.Open(KNrhProcessName);
	if(err)
		{
		INFO_PRINTF1(_L("CLbsRootApiTest1Step : ***ERROR - NRH not found"));
		SetTestStepResult(EFail);
		}
	process.Close();
    err = process.Open(KAgpsProcessName);
    if(err)
        {
        INFO_PRINTF1(_L("CLbsRootApiTest1Step : ***ERROR - GPS LOC Manager not found"));
		SetTestStepResult(EFail);
        }
	process.Close();
    err = process.Open(KNgProcessName);
    if(err)
        {
	    INFO_PRINTF1(_L("CLbsRootApiTest1Step : ***ERROR - Network Gateway not found"));
		SetTestStepResult(EFail);
        }
	process.Close();	
	}
TVerdict CLbsRootApiTest1Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TInt err = KErrNone;
	if (TestStepResult()==EPass)
		{
		INFO_PRINTF1(_L("CLbsRootApiTest1Step : open RLbsSystemController"));

    	iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
        
		INFO_PRINTF1(_L("CLbsRootApiTest1Step : RequestSystemStartup"));
    	err = iLbsRootProcessApi.RequestSystemStartup();
		if(err != ELbsStartupSuccess)
			{
			SetTestStepResult(EFail);
			}

	    RProcess process;
        
		INFO_PRINTF1(_L("CLbsRootApiTest1Step : Check processes are up..."));
            
		CheckProcessesAreUp();
		
		if(TestStepResult() == EPass)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest1Step : Processes up OK"));    	
			}

		INFO_PRINTF1(_L("CLbsRootApiTest1Step : Trigger panic; wait for the system to be brought down and come back, then close it"));

		iPanicRequest.Set(1);
		User::After(3000000);

		INFO_PRINTF1(_L("CLbsRootApiTest1Step : Check processes have been brought back up..."));

		CheckProcessesAreUp();
		
		if(TestStepResult() == EPass)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest1Step : Processes back up OK"));    	
			}

		INFO_PRINTF1(_L("CLbsRootApiTest1Step : RequestSystemCloseDown"));
    	err = iLbsRootProcessApi.RequestSystemCloseDown();
		if(err != ELbsCloseDownComplete)
			{
			SetTestStepResult(EFail);
			}
		}
	return TestStepResult();
	}


TVerdict CLbsRootApiTest1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest1Step"));

    iLbsRootProcessApi.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}
