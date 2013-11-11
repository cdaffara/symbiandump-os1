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
// Implements the testexecute server for the xxBca Tests.
// For (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file
 @internalComponent
*/


#include "TE_xxBcaSvr.h"
#include "TE_xxBcaUnitSteps.h"

using namespace te_xxBcaUnitTests;

/** The server name. */
_LIT(KServerName, "TE_xxBcaSvr");


#ifndef EKA2
/**
 * Requires semaphore to sync with client as the Rendezvous() calls are not available.
 *
 * @internalComponent
 *
 * @leave On CleanupStack error 
 */
LOCAL_C void MainL()
	{
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	CTE_xxBcaSvr* server = NULL;
	
	// create the CTestServer derived server .
	TRAPD(err, (server = CTE_xxBcaSvr::NewL()));
	if (!err)
		{
		CleanupStack::PushL(server);
		RSemaphore sem;

		// the client API will already have created the semaphore
		User::LeaveIfError(sem.OpenGlobal(KServerName));
		CleanupStack::Pop(server);

		// sync with the client then enter the active scheduler
		sem.Signal();
		sem.Close();
		sched->Start();
		}

	CleanupStack::Pop(sched);
	delete server;
	delete sched;
	}
#else
// EKA2 much simpler
// just an E32Main and a MainL()

/**
 * Much simpler, uses the new Rendezvous() call to sync with the client.
 *
 * @internalComponent
 *
 * @leave When new(ELeave) leaves 
 */
LOCAL_C void MainL()
	{
	// leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CTE_xxBcaSvr* server = NULL;
	// create the CTestServer derived server
	TRAPD(err,server = CTE_xxBcaSvr::NewL());
	if (!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}
#endif

// only a DLL on emulator for typhoon and earlier
#if (defined __WINS__ && !defined EKA2)
/**
 * Main entry point.
 *
 * @internalComponent
 *
 * @return Standard Epoc error codes.
 */
GLDEF_C TInt E32Dll(enum TDllReason)
	{
	return KErrNone;
	}
#else

/**
 * Main entry point.
 *
 * @internalComponent
 *
 * @return Standard Epoc error codes.
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TInt err(KErrNone);
	TRAP(err, MainL());
	(void)err;
	delete cleanup;
	return KErrNone;
    }
#endif

// Create a thread in the calling process
// Emulator typhoon and earlier
#if (defined __WINS__ && !defined EKA2)
/**
 * Server Thread function. Guts of the code in the MainL() function.
 *
 * @internalComponent
 *
 * @param aParam Unused.
 * @return Standard Epoc error codes.
 */
TInt ThreadFunc (
	TAny* /*aParam*/)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
		
	TRAPD(err, MainL());
	delete cleanup;
	return KErrNone;
	}

/**
 * 1st and only ordinal, called by the client API to initialise the server
 *
 * @internalComponent
 *
 * @return Standard Epoc error codes
 */
EXPORT_C TInt NewServer()
	{
	_LIT(KThread, "Thread");
	RThread thread;
	TBuf<KMaxTestExecuteNameLength> threadName(KServerName);

	// create a hopefully unique thread name and use the ThreadFunc
	threadName.Append(KThread);

	// allow a 1Mb max heap
	const TInt KMaxHeapSize = 0x1000000;			
	TInt err = thread.Create(threadName, ThreadFunc, KDefaultStackSize,
		KMinHeapSize, KMaxHeapSize, NULL, EOwnerProcess);
	if (err)
		{
		return err;
		}

	thread.Resume();
	thread.Close();
	return KErrNone;
	}

#endif

/**
 * Called inside the MainL() function to create and start the CTestServer derived server.
 *
 * @internalComponent
 *
 * @return Instance of the test server.
 */
CTE_xxBcaSvr* CTE_xxBcaSvr::NewL()
	{
	CTE_xxBcaSvr * server = new (ELeave) CTE_xxBcaSvr();
	CleanupStack::PushL(server);
	
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
	
/**
 * Implementation of CTestServer virtual function.
 *
 * @internalComponent
 *
 * @return A CTestStep derived instance.
 */
CTestStep* CTE_xxBcaSvr::CreateTestStep(
	const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	TInt err(KErrNone);
	TRAP(
		err,
		{
		if(aStepName == KCClosedState1)
			{
			testStep = new (ELeave)CClosedState1;
			}
		else if(aStepName == KCShutdownState1)
			{
			testStep = new (ELeave)CShutdownState1;
			}
		else if(aStepName == KCShutdownState2)
			{
			testStep = new (ELeave)CShutdownState2;
			}
		else if(aStepName == KCOpeningState1)
			{
			testStep = new (ELeave)COpeningState1;
			}
		else if(aStepName == KCOpeningState2)
			{
			testStep = new (ELeave)COpeningState2;
			}
		else if(aStepName == KCOpeningState3)
			{
			testStep = new (ELeave)COpeningState3;
			}		
		else if(aStepName == KCOpenState1)
			{
			testStep = new (ELeave)COpenState1;
			}
		else if(aStepName == KCOpenState2)
			{
			testStep = new (ELeave)COpenState2;
			}
		else if(aStepName == KCOpenState3)
			{
			testStep = new (ELeave)COpenState3;
			}
		else if(aStepName == KCOpenState4)
			{
			testStep = new (ELeave)COpenState4;
			}
		else if(aStepName == KCReadingState1)
			{
			testStep = new (ELeave)CReadingState1;
			}
		else if(aStepName == KCReadingState2)
			{
			testStep = new (ELeave)CReadingState2;
			}
		else if(aStepName == KCReadingState3)
			{
			testStep = new (ELeave)CReadingState3;
			}
		else if(aStepName == KCWritingState1)
			{
			testStep = new (ELeave)CWritingState1;
			}
		else if(aStepName == KCWritingState2)
			{
			testStep = new (ELeave)CWritingState2;
			}	
		else if(aStepName == KCWritingState3)
			{
			testStep = new (ELeave)CWritingState3;
			}	
		else if(aStepName == KCReadingWritingState1)
			{
			testStep = new (ELeave)CReadingWritingState1;
			}
		else if(aStepName == KCReadingWritingState2)
			{
			testStep = new (ELeave)CReadingWritingState2;
			}
		else if(aStepName == KCReadingWritingState3)
			{
			testStep = new (ELeave)CReadingWritingState3;
			}
		else if(aStepName == KCReadingWritingState4)
			{
			testStep = new (ELeave)CReadingWritingState4;
			}
		else if(aStepName == KCIoctlingState1)
			{
			testStep = new (ELeave)CIoctlingState1;
			}
		else if(aStepName == KCIoctlingState2)
			{
			testStep = new (ELeave)CIoctlingState2;
			}	
		else if(aStepName == KCCleanupBcaRelease)
			{
			testStep = new (ELeave)CCleanupBcaRelease;
			}			
		}
	);
	(void)err;	
	return testStep;
	}
