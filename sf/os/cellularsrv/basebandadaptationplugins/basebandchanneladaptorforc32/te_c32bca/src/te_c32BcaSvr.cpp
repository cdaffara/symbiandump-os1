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
// Implements the testexecute server for the C32Bca Tests.
// For (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file
 @internalComponent
*/


#include "te_c32BcaSvr.h"
#include "te_c32BcaUnitSteps.h"

using namespace te_c32BcaUnitTests;

/** The server name. */
_LIT(KServerName, "TE_C32BcaSvr");


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

	CTE_C32BcaSvr* server = NULL;
	// create the CTestServer derived server
	TRAPD(err,server = CTE_C32BcaSvr::NewL());
	if (!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// only a DLL on emulator for typhoon and earlier

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

// Create a thread in the calling process
// Emulator typhoon and earlier

/**
 * Called inside the MainL() function to create and start the CTestServer derived server.
 *
 * @internalComponent
 *
 * @return Instance of the test server.
 */
CTE_C32BcaSvr* CTE_C32BcaSvr::NewL()
	{
	CTE_C32BcaSvr * server = new (ELeave) CTE_C32BcaSvr();
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
CTestStep* CTE_C32BcaSvr::CreateTestStep(
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
		if(aStepName == KCClosedState2)
			{
			testStep = new (ELeave)CClosedState2;
			}
		if(aStepName == KCShutdownState1)
			{
			testStep = new (ELeave)CShutdownState1;
			}
		else if(aStepName == KCOpenState1)
			{
			testStep = new (ELeave)COpenState1;
			}
		else if(aStepName == KCOpenState2)
			{
			testStep = new (ELeave)COpenState2;
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
		else if(aStepName == KCReadingWritingState5)
			{
			testStep = new (ELeave)CReadingWritingState5;
			}
		else if(aStepName == KCIoctlingState1)
			{
			testStep = new (ELeave)CIoctlingState1;
			}
		else if(aStepName == KCIoctlingState2)
			{
			testStep = new (ELeave)CIoctlingState2;
			}		
		else if(aStepName == KCPacketBoundary1)
			{
			testStep = new (ELeave)CPacketBoundary1;
			}
		else if(aStepName == KCIoctlOption1)
			{
			testStep = new (ELeave)CIoctlOption1;
			}
		else if(aStepName == KCIoctlOption2)
			{
			testStep = new (ELeave)CIoctlOption2;
			}		
		else if(aStepName == KCIoctlInvalidOption)
			{
			testStep = new (ELeave)CIoctlInvalidOption;
			}		
		else if(aStepName == KCIoctlSerialPort1)
			{
			testStep = new (ELeave)CIoctlSerialPort1;
			}
		else if(aStepName == KCIoctlSerialPort2)
			{
			testStep = new (ELeave)CIoctlSerialPort2;
			}
		else if(aStepName == KCIoctlSerialPort3)
			{
			testStep = new (ELeave)CIoctlSerialPort3;
			}
		else if(aStepName == KCIoctlSerialPort4)
			{
			testStep = new (ELeave)CIoctlSerialPort4;
			}		
		else if(aStepName == KCCommDbModemCommRole)
			{
			testStep = new (ELeave)CCommDbModemCommRole;
			}		
		else if(aStepName == KCCommDbNoChannelId)
			{
			testStep = new (ELeave)CCommDbNoChannelId;
			}
		else if(aStepName == KCNoChannelIdNoIapId)
			{
			testStep = new (ELeave)CNoChannelIdNoIapId;
			}
		else if(aStepName == KCCleanupBcaRelease)
			{
			testStep = new (ELeave)CCleanupBcaRelease;
			}
		else if(aStepName == KCValidChannelId)
			{
			testStep = new (ELeave)CValidChannelId;
			}
		else if(aStepName == KCHiddenIAPRecord)
			{
			testStep = new (ELeave)CHiddenIAPRecord;
			}					
		}
	);
	(void)err;	
	return testStep;
	}
