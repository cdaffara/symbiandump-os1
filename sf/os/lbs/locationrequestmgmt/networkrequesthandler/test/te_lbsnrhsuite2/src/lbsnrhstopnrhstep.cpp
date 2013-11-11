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
 @file lbsnnrhprivacystopnrhStep.cpp
*/
 
#include <lbs/lbsadmin.h>
#include "lbsnrhstopnrhstep.h"
#include "Te_lbsnrhsuite2defs.h"
#include "lbsinternalinterface.h"
#include "lbsnetregstatusint.h"


CLbsNrhStopNrhStep::~CLbsNrhStopNrhStep()
/**
 * Destructor
 */
	{
	}

CLbsNrhStopNrhStep::CLbsNrhStopNrhStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsNrhStopNrhStep);
	}

TVerdict CLbsNrhStopNrhStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsNrhSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsNrhStopNrhStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//
		// Special test step to shutdown the LBS
		// sub-system once all of the tests in this
		// test suite have been run.
		//
		
		// Shutdown NRH
		User::LeaveIfError(StopNrh());
		
		// Shutdown system controller interface
		RLbsProcessSupervisor::ShutDownL();

		// Shutdown the network registration status interface
		RLbsNetworkRegistrationStatus::ShutDownL();
		
		// Shutdown the NRH-AGPS interface
		RLbsPositionUpdateRequests::ShutDownL();
		RLbsPositionUpdates::ShutDownL();
		RLbsGpsMeasurementUpdates::ShutDownL();
		
		// Shutdown NRH-NG interface
		RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2NRH);
		RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNRH2NG);
		
		// Reset the LbsAdmin settings
		iAdmin->ResetToDefault();

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}

TVerdict CLbsNrhStopNrhStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CLbsNrhStopNrhStep"));
	// process something post setting to the test step
	
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_LbsNrhSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

/* Forces the shutdown of lbsnetworkrequesthandler.exe.
 */
 TInt CLbsNrhStopNrhStep::StopNrh()
 	{
 	// Shutdown the network request handler via the process 
 	// supervisor api that LbsRoot uses.
 	TRAPD(err, iProcessSupervisor.OpenL(KLbsNetRequestHandlerUid));
 	if (err == KErrNone)
 		{
 		// Find the network gateway manager process
 		RProcess process;
 		TFullName fullName;
 		TFindProcess findProcess;
 		_LIT(KNrhProcessPattern, "t_lbsnetworkrequesthandler.exe*");
 		TBool runningInstanceFound = EFalse;
 		
 		findProcess.Find(KNrhProcessPattern);
 		while (err = findProcess.Next(fullName), err == KErrNone)
 			{
 			TInt processOpenErr = process.Open(findProcess);
 			if (processOpenErr == KErrNone)
 				{
 				TExitType exitType = process.ExitType();
 				if (exitType == EExitPending)
 					{
 					// found a running instance of the process,
 					// so assume everything is ok;
 					runningInstanceFound = ETrue;
 					break;
 					}
 				}
 				process.Close();
 			}
 		
 		if (runningInstanceFound)
 			{
 			TRequestStatus status;
 			process.Logon(status);
 			iProcessSupervisor.CloseDownProcess();
 			User::WaitForRequest(status);
 			process.Close();
 			iProcessSupervisor.Close();
 			}
 		}
 	
 	return err;
 	}
