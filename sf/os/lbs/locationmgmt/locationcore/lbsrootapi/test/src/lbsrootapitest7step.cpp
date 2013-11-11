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
 @file lbsrootapiTest7Step.cpp
*/
#include "lbsrootapitest7step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"



CLbsRootApiTest7Step::~CLbsRootApiTest7Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest7Step::CLbsRootApiTest7Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest7Step);
	}

TVerdict CLbsRootApiTest7Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest7Step"));

__UHEAP_MARK;

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL();

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

void CLbsRootApiTest7Step::CheckProcessesAreUp()
	{
	RProcess process;
	TInt err = process.Open(KNrhProcessName);
	if(err)
		{
		INFO_PRINTF1(_L("CLbsRootApiTest7Step : ***ERROR - NRH not found"));
		SetTestStepResult(EFail);
		}
	process.Close();
    err = process.Open(KAgpsProcessName);
    if(err)
        {
        INFO_PRINTF1(_L("CLbsRootApiTest7Step : ***ERROR - GPS LOC Manager not found"));
		SetTestStepResult(EFail);
        }
	process.Close();
    err = process.Open(KNgProcessName);
    if(err)
        {
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : ***ERROR - Network Gateway not found"));
		SetTestStepResult(EFail);
        }
	process.Close();	
	}

TVerdict CLbsRootApiTest7Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{

		INFO_PRINTF1(_L("CLbsRootApiTest7Step : open RLbsSystemController"));

		RLbsSystemController lbsRootProcessApi;
		TUid uid = RProcess().SecureId();
		lbsRootProcessApi.OpenL(KLbsTestProcessUid);
        
		INFO_PRINTF1(_L("CLbsRootApiTest7Step : RequestSystemStartup"));
		TInt err = lbsRootProcessApi.RequestSystemStartup();		
		if(err != ELbsStartupSuccess)
			{
			SetTestStepResult(EFail);
			}

		CheckProcessesAreUp();
		
		if(TestStepResult() == EPass)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest7Step : Processes up OK"));    	
			}

	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : Request system closedown without confirmation"));
        lbsRootProcessApi.RequestSystemCloseDown();
		SetTestStepResult(EPass);

		}		
		
	  return TestStepResult();
	}
TVerdict CLbsRootApiTest7Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest7Step"));

    iLbsRootProcessApi.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}
