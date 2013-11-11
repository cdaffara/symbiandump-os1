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
 @file ProcessLaunchTest0Step.cpp
 @internalTechnology
*/
#include "ProcessLaunchTest0Step.h"
#include "Te_ProcessLaunchTestSuiteDefs.h"
#include "cprocesslaunch.h"
#include "tprocessstartparams.h"

CProcessLaunchTest0Step::~CProcessLaunchTest0Step()
/**
 * Destructor
 */
	{
	}

CProcessLaunchTest0Step::CProcessLaunchTest0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KProcessLaunchTest0Step);
	}

TVerdict CProcessLaunchTest0Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CProcessLaunchTest0Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_ProcessLaunchTestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CProcessLaunchTest0Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	_LIT(KThreadName, "*lbsgpslocmanager*");
	TProcessStartParams processParams;
	_LIT(KDummyFileName, "\\sys\\bin\\lbsgpslocmanager.exe");
	_LIT(KDummyProcessName, "DummyAgpsManager");
	processParams.SetProcessFileName(KDummyFileName);
	processParams.SetProcessName(KDummyProcessName);
	processParams.SetRendezvousRequired(EFalse);

	if (TestStepResult()==EPass)
		{
		CProcessLaunch::ProcessLaunch(processParams);
		// now we have to look for this thread. 
		TFindThread threadFinder(KThreadName);
		TFullName matchedThreadName;
		// see how many instances we have of the thread
		TInt matchCount = 0;
		while(threadFinder.Next(matchedThreadName) == KErrNone)
			{
			++matchCount;
			}
		// match count must be one at this point
		if(matchCount!=1)
			{
			// fail the test, its all gone very wrong - there are 2 processes
			SetTestStepResult(EFail);
			}
		// now we want to grab the ThreadID (we can just use thead id's, don't need handles)
		RThread processThread;
		User::LeaveIfError(processThread.Open(matchedThreadName));
		TThreadId tid = processThread.Id();
		// now try and break things, by starting a 2nd copy of the process.

		CProcessLaunch::ProcessLaunch(processParams); // NB we use the same process params
		matchCount = 0;
		threadFinder.Find(KThreadName);
		while(threadFinder.Next(matchedThreadName)==KErrNone)
			{
			++matchCount;
			}
		// match count must be one at this point
		if(matchCount!=1)
			{
			// fail the test, its all gone very wrong - there are 2 processes
			// this is were we will fail with the current code.
			SetTestStepResult(EFail);
			}
		// check the thread ID's
		RThread newProcessThread;
		User::LeaveIfError(newProcessThread.Open(matchedThreadName));
		TThreadId newTid = newProcessThread.Id();
		if(newTid.Id() != tid.Id())
			{
			// fail the test these are different thread id's
			// This is to be expected in the current code base
			SetTestStepResult(EFail);
			}
		else
			{
			// test passes - there is only one instance of the process
			SetTestStepResult(EPass);
			}
		}
		// now kill the process we started
		_LIT(KStar, "*");
		TFullName wildCardPattern;
		wildCardPattern.Append(KStar);
		wildCardPattern.Append(KThreadName);
		wildCardPattern.Append(KStar);
		TFindProcess pf(wildCardPattern);
		TFullName name;
		TInt findError = pf.Next(name);
		RProcess p;
		TInt pErr = 0;
		pErr = p.Open(name);
		User::LeaveIfError(pErr);
		// nuke it
		p.Kill(0);
		p.Close();
		return TestStepResult();
	}



TVerdict CProcessLaunchTest0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CProcessLaunchTest0Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_ProcessLaunchTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
