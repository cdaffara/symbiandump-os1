// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file man6Step.cpp
*/
#include "man6Step.h"
#include "Te_manSuiteDefs.h"
#include "man1Step.h"
#include "LbsInternalInterface.h"
#include "lbsprocesssupervisor.h"
#include "lbslocsourcetestchannel.h"

Cman6Step::~Cman6Step()
/**
 * Destructor
 */
	{
	}

Cman6Step::Cman6Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kman6Step);
	}

TVerdict Cman6Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman6Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TTime time;
		time.UniversalTime();
		RDebug::Printf("%02d:%02d:%02d:%06d  [%Ld] xxxxx Cman6Step::doTestStepL start",
			time.DateTime().Hour(), time.DateTime().Minute(), time.DateTime().Second(),time.DateTime().MicroSecond(),
			RProcess().Id().Id());
		
		//This should be run as the last test in the te_man suite.  This test uses the Root API
		// CLbsCloseDownRequestDetector to simulate the Root telling the AGPS Manager to shutdown.
		// The Manager should send a CancelLocationRequest message and a CloseDown power message
		// to the integration module.  The Manager should then wait until the module makes the 
		// Shutdown() call before beginning the shut down process.
		

		//Setup the test AGPS Module Channel
		RAgpsModuleTestChannel moduleInt;
		moduleInt.OpenL();
		CleanupClosePushL(moduleInt);
		
		//Setup the root process ShutDown
		RLbsProcessSupervisor shutDownProcess;
		TUid managerModuleId = TUid::Uid(0x10281D44);	//KLbsGpsLocManagerUidValue
		shutDownProcess.OpenL(managerModuleId);
		CleanupClosePushL(shutDownProcess);

		//Open up the test AGPS Manager Process
		RProcess proc;
		_LIT(KMatchName,"testlbsgpslocmanager*");
		TFindProcess procName;
		TFullName procFullName;
		procName.Find(KMatchName);
		procName.Next(procFullName);
		
		//Check to see whether the manager process is still up and running
		TInt rc = proc.Open(procFullName);
		if(rc != KErrNone)
			{
			//Error, process has already closed down before receiving the shutdown from the module
			INFO_PRINTF1(_L("Error - cannot find the GPS Location Manager Process, failing test"));
			SetTestStepResult(EFail);
			}
		else
			{
			//Send the Shut Down to the Manager
			shutDownProcess.CloseDownProcess();
				
			//The test integration module will wait 5 seconds before sending the shutdown message to
			// the manager.  So the AGPS Manager should still be open.  The test should wait here
			// until the AGPS Manager process is closed down.  If the test times out, it is likely that
			// the AGPS Manager process was never closed, so the test cannot progress past this part
			TRequestStatus procStatus;
			proc.Logon(procStatus);
			
			time.UniversalTime();
			RDebug::Printf("%02d:%02d:%02d:%06d  [%Ld] xxxxx Cman6Step::doTestStepL after logon",
				time.DateTime().Hour(), time.DateTime().Minute(), time.DateTime().Second(),time.DateTime().MicroSecond(),
				RProcess().Id().Id());
					
			User::WaitForRequest(procStatus);
			
			time.UniversalTime();
			RDebug::Printf("%02d:%02d:%02d:%06d  [%Ld] xxxxx Cman6Step::doTestStepL after wait",
				time.DateTime().Hour(), time.DateTime().Minute(), time.DateTime().Second(),time.DateTime().MicroSecond(),
				RProcess().Id().Id());
						
			
			//When the Integration Module sends the shutdown message to the manager it updates the test
			// channel (so this message should also take 5 seconds before appearing on the channel and should
			// only appear when the manager has been given permission to close down).  Check the channel
			// to make sure the right message is present (ShutdownEvent)
			RAgpsModuleTestChannel::TTestMsgType messageType = RAgpsModuleTestChannel::ETestMsgUnknown;
			moduleInt.GetMsg(messageType);
			if(messageType == RAgpsModuleTestChannel::ETestMsgShutdownEvent)
				{
				INFO_PRINTF1(_L("Module correctly waited for Shutdown call from Module to shutdown and then shutdown the manager process."));
				SetTestStepResult(EPass);
				}
			else
				{
				INFO_PRINTF1(_L("Error - AGPS Manager has already been shutdown before receiving Shutdown() from Module"));
				SetTestStepResult(EFail);
				}
			}
		proc.Close();
		
		CleanupStack::Pop(2, &moduleInt);	//moduleInt, shutDownProcess
		}
	  return TestStepResult();
	}



TVerdict Cman6Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
