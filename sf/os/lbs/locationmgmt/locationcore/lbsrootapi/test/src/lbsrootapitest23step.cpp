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
 @file lbsrootapiTest23Step.cpp
*/
#include "lbsrootapitest23step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"



CLbsRootApiTest23Step::~CLbsRootApiTest23Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest23Step::CLbsRootApiTest23Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest23Step);
	}

TVerdict CLbsRootApiTest23Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest23Step"));

	__UHEAP_MARK;

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL(22);
	
	iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest23Step::DoMainL()
{
}
TVerdict CLbsRootApiTest23Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
    	// Test to check that LbsRoot keeps track of the number
    	// of 'clients' (i.e. the number of start requests)
    	// that are outstanding, so that it only shuts down Lbs
    	// when there are none left.    	

		INFO_PRINTF1(_L("CLbsRootApiTest23Step : Start Lbs system"));

		TInt err = iLbsRootProcessApi.RequestSystemStartup();
		if (err != ELbsStartupSuccess
			|| !CheckProcessesAreRunning())
			{        		
			return(EFail);
			}
			
		INFO_PRINTF1(_L("CLbsRootApiTest23Step : Second start of Lbs system"));
		
		err = iLbsRootProcessApi.RequestSystemStartup();
		if (err != ELbsStartupSuccess
			|| !CheckProcessesAreRunning())
			{        		
			return(EFail);
			}
		
		INFO_PRINTF1(_L("CLbsRootApiTest23Step : First shutdown of the Lbs system"));

		// The first shutdown request should be rejected
		// because StartSystemL was called twice, so there
		// is still one 'client' that wants to use LBS
		err = iLbsRootProcessApi.RequestSystemCloseDown();
		if (err != ELbsCloseDownRejected
			|| !CheckProcessesAreRunning())
			{
			return (EFail);
			}
		
		INFO_PRINTF1(_L("CLbsRootApiTest23Step : Second shutdown of the Lbs system"));
		
		// Root should shutdown Lbs on the second shutdown request since
		// now the number of start requests equals the number of
		// shutdown requests.
		err = iLbsRootProcessApi.RequestSystemCloseDown();
		if (err != ELbsCloseDownComplete
			|| CheckProcessesAreRunning())
			{
			SetTestStepResult(EFail);
			}
		}
		
	  return TestStepResult();
	}

TVerdict CLbsRootApiTest23Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest23Step"));

    iLbsRootProcessApi.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

/*
*/
TBool CLbsRootApiTest23Step::CheckProcessIsRunning(const TDesC& aProcessNamePattern)
	{
	TInt err = KErrNone;
	RProcess process;
	TFullName fullName;
	TFindProcess findProcess;
	TBool runningInstanceFound = EFalse;
	
	findProcess.Find(aProcessNamePattern);
	while (err = findProcess.Next(fullName), err == KErrNone)
		{
		TInt processOpenErr = process.Open(findProcess);
		if (processOpenErr == KErrNone)
			{
			TExitType exitType = process.ExitType();
			if (exitType == EExitPending)
				{
				// found a running instance of the process,
				// so assume everything is ok; return ETrue
				runningInstanceFound = ETrue;
				process.Close();
				break;
				}
			process.Close();
			}
		}
	
	return runningInstanceFound;	
	}

/*
*/
TBool CLbsRootApiTest23Step::CheckProcessesAreRunning()
	{
	_LIT(KNrhProcessNamePattern, "t_lbsrootapi_lbsnetworkrequesthandler.exe*");
	_LIT(KAgpsProcessNamePattern, "t_lbsrootapi_lbsgpslocmanager.exe*");
	_LIT(KNgProcessNamePattern,   "t_lbsrootapi_lbsnetgateway.exe*");
	
	if (!CheckProcessIsRunning(KNrhProcessNamePattern))
		{
	    INFO_PRINTF1(_L("*** NRH not found"));
	    return EFalse;		
		}

	if (!CheckProcessIsRunning(KAgpsProcessNamePattern))
		{
	    INFO_PRINTF1(_L("*** AGPS not found"));
	    return EFalse;		
		}

	if (!CheckProcessIsRunning(KNgProcessNamePattern))
		{
	    INFO_PRINTF1(_L("*** NG not found"));
	    return EFalse;		
		}
	
	return ETrue;	
	}


