// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <apgtask.h>

#include <apastarter.h>
#include "tapstart_step.h"
#include "testapps.h"

_LIT(KApStartDLL, "apstart.dll");
typedef CApaStarter* (*TFuncNewL)();

CApStartTestStep::CApStartTestStep()
	{
	SetTestStepName(KCTestCaseApStart);
	}

TVerdict CApStartTestStep::doTestStepPreambleL()
	{
	// Load the CApaStarter implementation
	RLibrary lib;
	User::LeaveIfError(lib.Load(KApStartDLL)); // load the library
	CleanupClosePushL(lib);		
	TLibraryFunction ord1 = lib.Lookup(1); // This is the NewL function
	TFuncNewL funcL = reinterpret_cast<TFuncNewL>(ord1);
	iAppStart = static_cast<CApaStarter*> (funcL());	
	CleanupStack::PopAndDestroy(&lib);
		
	return CTestStep::doTestStepPreambleL();	
	}

TVerdict CApStartTestStep::doTestStepPostambleL()
	{
	delete iAppStart;
	iAppStart = NULL;
	
 	return CTestStep::doTestStepPostambleL();
	}

TVerdict CApStartTestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("CApStartTestStep started....\n"));
	__UHEAP_MARK;
	
	RWsSession wsSession;
	User::LeaveIfError(wsSession.Connect());
	CleanupClosePushL(wsSession);	
	
	TRAPD(err, TestStartAndForgetL(wsSession));
	TESTE(KErrNone == err, err);

	TRAP(err, TestStartAndFollowUpL(wsSession));
	TESTE(KErrNone == err, err);
	
	CleanupStack::PopAndDestroy();	//wsSession.Close();
	
	TRAP(err, TestInitApparcL());
	TESTE(KErrNone == err, err);

	__UHEAP_MARKEND; 
	INFO_PRINTF1(_L(".... CApStartTestStep finished!!"));
	
	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-APSTART-0001
New Test CaseID 		DEVSRVS-SYSSTART-APSTART-0001
 */


void CApStartTestStep::TestStartAndForgetL(RWsSession& aWsSession)
	{
	INFO_PRINTF1(_L("Entering test case: APPFWK-APSTART-0001"));
	
	TApaTask task = TestStartL(NULL, aWsSession);
	
	if(task.Exists())
		{
		task.KillTask();
		INFO_PRINTF1(_L("Leaving test case: APPFWK-APSTART-0001\n"));
		}
	else
		{
		ERR_PRINTF1(_L("APPFWK-APSTART-0001 failed\n"));  
		}
	}

/**
Old Test CaseID 		APPFWK-APSTART-0002
New Test CaseID 		DEVSRVS-SYSSTART-APSTART-0001
 */

void CApStartTestStep::TestStartAndFollowUpL(RWsSession& aWsSession)
	{
	INFO_PRINTF1(_L("Entering test case: APPFWK-APSTART-0002"));	
	
	TRequestStatus status;
	TApaTask task = TestStartL(&status, aWsSession);
	TESTE(KErrNone == status.Int(), status.Int());
	
	if(task.Exists())
		{
		task.KillTask();
		INFO_PRINTF1(_L("Leaving test case: APPFWK-APSTART-0002\n"));
		}
	else
		{
		ERR_PRINTF1(_L("APPFWK-APSTART-0002 failed\n"));  
		}
	}

/**
Old Test CaseID 		APPFWK-APSTART-0003
New Test CaseID 		DEVSRVS-SYSSTART-APSTART-0001
 */

void CApStartTestStep::TestInitApparcL()
	{ // Leavescan, this implementation of this method does not leave but further derived implementations may if they choose
	INFO_PRINTF1(_L("Entering test case: APPFWK-APSTART-0003"));
	INFO_PRINTF1(_L("Going to sleep until Apparc finished initialisation."));
	TRAPD(err, iAppStart->WaitForApparcToInitialiseL());
	TEST(err == KErrNone);
	if(err)
		ERR_PRINTF2(_L("APPFWK-APSTART-0003 finished with errorcode = '%d'\n"), err);  
	else
		INFO_PRINTF1(_L("Leaving test case: APPFWK-APSTART-0003\n"));
	}

/**
 * Common implementation for test case 0001 and 0002. See INFO_PRINTs for further information.
 */
TApaTask CApStartTestStep::TestStartL(TRequestStatus* aRequestStatus, RWsSession& aWsSession)
	{
	INFO_PRINTF1(_L("Requesting start of KTestAppGood in foreground."));
	const TDesC& cmdLineParams = KNullDesC;
	const TBool viewLess = EFalse;
	const TBool startInBackground = EFalse;
	TThreadId tId;
	if(aRequestStatus)
		{
		iAppStart->StartAppL(KTestAppGood, cmdLineParams, viewLess, startInBackground, tId, *aRequestStatus);
		}
	else
		{
		iAppStart->StartAppL(KTestAppGood, cmdLineParams, viewLess, startInBackground, tId);
		}
		
	RThread thread;
	RProcess process;
	INFO_PRINTF1(_L("Verifying that the process was created."));
	TEST(KErrNone == thread.Open(tId));
	TEST(KErrNone == thread.Process(process)); 
	
	INFO_PRINTF1(_L("Waiting for KTestAppGood to resume."));
	if(aRequestStatus)
		{
		User::WaitForRequest(*aRequestStatus);
		}
	else
		{
		TRequestStatus status;
		process.Rendezvous(status);
		User::WaitForRequest(status);
		}
	process.Close();
	thread.Close();
	
	INFO_PRINTF1(_L("Verifying that KTestAppGood is now up and running."));
	TApaTaskList taskList(aWsSession); 
	const TUid appUid = {KTestAppGoodUid};
	TApaTask task = taskList.FindApp(appUid);
	TEST(task.Exists());
	
	return task;
	}
