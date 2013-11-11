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
 @file lbsrootapiTest22Step.cpp
*/
#include "lbsrootapitest22step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"


const TInt KMaxNumberOfRestartsExpected = 3;


CLbsRootApiTest22Step::~CLbsRootApiTest22Step()
/**
 * Destructor
 */
	{
	iPanicRequest.Close();
	}

CLbsRootApiTest22Step::CLbsRootApiTest22Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest22Step);
	}

TVerdict CLbsRootApiTest22Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest22Step"));

__UHEAP_MARK;

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL(21);

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

TVerdict CLbsRootApiTest22Step::doTestStepL()
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

    	// Start things off
    	iLbsRootProcessApi.OpenL(KLbsTestProcessUid);

		INFO_PRINTF1(_L("CLbsRootApiTest22Step : RequestSystemStartup"));

		err = iLbsRootProcessApi.RequestSystemStartup();
		
		if(err != ELbsStartupSuccess)
			{        		
			return(EFail);
			}

		// Panic one of the processes
		iPanicRequest.Set(1);
	
		// Time delay to give LbsRoot time to restart the system
		User::After(2000000);
 			
		// Now panic one of the processes to see if LbsRoot restarts the whole system
		for (TInt i = 0; i < KMaxNumberOfRestartsExpected; i++)
			{
			// Check all of the the processes are active.   	
			INFO_PRINTF1(_L("CLbsRootApiTest22Step : Check processes are currently active..."));
			
			// Check the processes have been restarted
			if (!CheckProcessesAreRunning())
				{
				return(EFail);
				}

			// Panic one of the processes
			iPanicRequest.Set(1);
		
			// Time delay to give LbsRoot time to restart the system
			User::After(2000000);
			} // end for

		// Check the processes have not been restarted after 
		// KMaxNumberOfRestartsExpected times
		if (CheckProcessesAreRunning())
			{
			return(EFail);
			}
			
		}	// end 	if (TestStepResult()==EPass)	

	return TestStepResult();
	}

TVerdict CLbsRootApiTest22Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest22Step"));

    iLbsRootProcessApi.Close();
	// Restart lbsroot in case it's needed by any subsequent tests.
	StartLbsRootL();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

/*
*/
TBool CLbsRootApiTest22Step::CheckProcessIsRunning(const TDesC& aProcessNamePattern)
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
			}
			process.Close();
		}
	
	return runningInstanceFound;	
	}

/*
*/
TBool CLbsRootApiTest22Step::CheckProcessesAreRunning()
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


