// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Telephony Watchers Integration Test base test code.
// 
//

/**
 @file
*/


#include <e32uid.h>
#include <simtsy.h>


#include "TE_TelWatchersIntBase.h"


const TInt  KTsyShutdownGuardTimer = 500000;

CTelWatchersIntTestStep::CTelWatchersIntTestStep()
	{
	// NOP
	} // CTelWatchersIntTestStep::CTelWatchersIntTestStep


CTelWatchersIntTestStep::~CTelWatchersIntTestStep()
	{
	// NOP
	} // CTelWatchersIntTestStep::~CTelWatchersIntTestStep


TVerdict CTelWatchersIntTestStep::doTestStepPreambleL()
	{
	//
	// Mark for memory leaks!!!
	//
	__UHEAP_MARK;

	//
	// Create an Active Scheduler...
	//
	iScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);

	//
	// Initialise to a known state...
	//
	StartWatcher();

	return TestStepResult();
	} // CTelWatchersIntTestStep::doTestStepPreambleL


TVerdict CTelWatchersIntTestStep::doTestStepPostambleL()
	{
	//
	// Ensure the Watcher is stopped so future tests can continue past
	// failures...
	//
	StopWatcher();
	
	delete iScheduler;
	iScheduler = NULL;
	
	//
	// Check the heap for memory leaks...
	//
	__UHEAP_MARKEND;

	return TestStepResult();
	} // CTelWatchersIntTestStep::doTestStepPostambleL


/**
 *  Start the Watcher.
 */
void CTelWatchersIntTestStep::StartWatcher()
	{
	INFO_PRINTF1(_L("Start the Watchers..."));

	if (iWatcherExeRunning == EFalse)
		{
		//
		// We can no longer move the watcher.exe after sysbin platform security enforcement
		// so the alternative method is to prevent the watchers from starting, in this case
		// we can start watchers from system/bin without worry, older builds still require z:
		TInt ret = iWatcherExe.Create(_L("z:\\watcher.exe"), _L(""));
		if (ret == KErrNotFound)
			{
			ret = iWatcherExe.Create(_L("watcher.exe"), _L(""));
			if (ret == KErrNotFound)
				{
				ret = iWatcherExe.Create(_L("z:\\System\\libs\\watcher.exe"), _L(""));
				}
			}

		INFO_PRINTF2(_L("Create watcher exe returned %d"), ret);
		iWatcherExe.Logon(iWatcherExeStatus);
		iWatcherExe.Resume();
		iWatcherExeRunning = ETrue;
		}
	} // CTelWatchersIntTestStep::StartWatcher


/**
 *  Stop the Watchers.
 */
void CTelWatchersIntTestStep::StopWatcher()
	{
	INFO_PRINTF1(_L("Stop the Watchers..."));

	//
	// This is needed to ensure if ETel panics a dead client thread - that
	// the test code contines.
	//
	TBool oldJustInTimeStatus = User::JustInTime();

	User::SetJustInTime(EFalse);

	//
	// Stop the watchers...
	//
	if (iWatcherExeRunning)
		{
		iWatcherExe.Kill(KErrAbort);
		User::WaitForRequest(iWatcherExeStatus);
		iWatcherExeRunning = EFalse;
		}
	User::After(KTsyShutdownGuardTimer);

	//
	// Reset the Just-In-Time debugging value.
	//
	User::SetJustInTime(oldJustInTimeStatus);
	} // CTelWatchersIntTestStep::StopWatcher


/**
 *  Set the test number given by the parameter aTestNumber.  The test number
 *  indicates which section in the config.txt file should be used.
 *
 *  @param aTestNumber	The test number section within the config.txt file
 *                      that should be used.
 */
void CTelWatchersIntTestStep::SetSimTsyTestNumberL(TInt aTestNumber)
  	{
	INFO_PRINTF2(_L("Setting SIMTSY config to number %d..."), aTestNumber);
	
	User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,aTestNumber));
	} // CTelWatchersIntTestStep::SetSimTsyTestNumberL

void CTelWatchersIntTestStep::CheckSimTsyTestNumberL(TInt aTestNumber)
  	{
	INFO_PRINTF2(_L("Checking SIMTSY config number to be %d..."), aTestNumber);

	// Ensure that the test number has been set properly
	RProperty testNumberInUseProperty;
	User::LeaveIfError(testNumberInUseProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumberInUse));
	CleanupClosePushL(testNumberInUseProperty);

	TInt testNumberInUseCheck;
	User::LeaveIfError(testNumberInUseProperty.Get(KUidPSSimTsyCategory, KPSSimTsyTestNumberInUse,testNumberInUseCheck));
	if (aTestNumber != testNumberInUseCheck)
		{
		INFO_PRINTF2(_L("Incorrect SIMTSY config number (%d)..."), testNumberInUseCheck);
		User::Leave(KErrNotFound);		
		}

	CleanupStack::PopAndDestroy(&testNumberInUseProperty);
	} // CTelWatchersIntTestStep::CheckSimTsyTestNumberL
