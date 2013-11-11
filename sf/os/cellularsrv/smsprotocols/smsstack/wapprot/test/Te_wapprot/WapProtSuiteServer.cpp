// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// WAP prot test suite server
// 
//

/**
    @file
    @test
*/

// Include your own server header file and step header file(s) here
#include "WapProtSuiteServer.h"

#include "WapIoctlSteps.h"
#include "WapStatusReports.h"

CWapProtSuite* CWapProtSuite::NewL()
/**
 *  @return - Instance of the test server
 *  Same code for Secure and non-secure variants
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
	{
	CWapProtSuite* server = new (ELeave) CWapProtSuite();
	CleanupStack::PushL(server);

    // CSmsStackTestServer intermediate base class call
    server->InitializeTsyAndPhonesL();
    
    // CServer base class call
    RProcess    handle = RProcess();
    TParsePtrC  serverName(handle.FileName());
	server->ConstructL(serverName.Name());
	
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CWapProtSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CWapProtSuite::NewL());
	if(!err)
		{
		// sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}



GLDEF_C TInt E32Main()
/**
 *  @return - Standard Epoc error code on process exit
 *  Secure variant only
 *  Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;

	return err;
    }

CTestStep* CWapProtSuite::CreateTestStep(const TDesC& aStepName)
/**
 *  @return - A CTestStep derived instance
 *  Secure and non-secure variants
 *  Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	//
	// Here the test step is created when it is needed. Note that this
	// function is non-leaving so we cannot use new(ELeave). Therefore
	// the new could return NULL, but that is not a problem as it implies
	// the test step is missing and this will be marked in the log file.
	//

	//*************************************
	// IOCTL tests
	//*************************************

	// IOCTL steps [1 - XYZ]
	if (aStepName == _L("IoctlStep_1"))
		{
		testStep = new CIoctlStep_1();
		}
	else if (aStepName == _L("IoctlStep_2"))
		{
		testStep = new CIoctlStep_2();
		}
	else if (aStepName == _L("IoctlStep_3"))
		{
		testStep = new CIoctlStep_3();
		}
	else if (aStepName == _L("IoctlStep_4"))
		{
		testStep = new CIoctlStep_4();
		}
	else if (aStepName == _L("IoctlStep_5"))
		{
		testStep = new CIoctlStep_5();
		}
	else if (aStepName == _L("IoctlStep_6"))
		{
		testStep = new CIoctlStep_6();
		}
	else if (aStepName == _L("IoctlStep_7"))
		{
		testStep = new CIoctlStep_7();
		}
	else if (aStepName == _L("IoctlStep_8"))
		{
		testStep = new CIoctlStep_8();
		}
	else if (aStepName == _L("IoctlStep_9"))
		{
		testStep = new CIoctlStep_9();
		}
	else if (aStepName == _L("IoctlStep_10"))
		{
		testStep = new CIoctlStep_10();
		}
	else if (aStepName == _L("IoctlStep_11"))
		{
		testStep = new CIoctlStep_11();
		}
	else if (aStepName == _L("IoctlStep_12"))
		{
		testStep = new CIoctlStep_12();
		}
	else if (aStepName == _L("IoctlStep_13"))
		{
		testStep = new CIoctlStep_13();
		}
	else if (aStepName == _L("IoctlStep_14"))
		{
		testStep = new CIoctlStep_14();
		}
	else if (aStepName == _L("IoctlStep_15"))
		{
		testStep = new CIoctlStep_15();
		}
	else if (aStepName == _L("IoctlStep_17"))
		{
		testStep = new CIoctlStep_17();
		}
	else if (aStepName == _L("IoctlStep_18"))
		{
		testStep = new CIoctlStep_18();
		}
	else if (aStepName == _L("IoctlStep_19"))
		{
		testStep = new CIoctlStep_19();
		}
	else if (aStepName == _L("IoctlStep_20"))
		{
		testStep = new CIoctlStep_20();
		}
	else if (aStepName == _L("IoctlStep_21"))
		{
		testStep = new CIoctlStep_21();
		}
	else if (aStepName == _L("IoctlStep_22"))
		{
		testStep = new CIoctlStep_22();
		}
	else if (aStepName == _L("IoctlStep_23"))
		{
		testStep = new CIoctlStep_23();
		}
	else if (aStepName == _L("IoctlStep_24"))
		{
		testStep = new CIoctlStep_24();
		}
	else if (aStepName == _L("IoctlStep_25"))
		{
		testStep = new CIoctlStep_25();
		}
	else if (aStepName == _L("IoctlStep_26"))
		{
		testStep = new CIoctlStep_26();
		}
	else if (aStepName == _L("IoctlStep_27"))
		{
		testStep = new CIoctlStep_27();
		}
	else if (aStepName == _L("IoctlStep_28"))
		{
		testStep = new CIoctlStep_28();
		}
	//Status report tests
	else if (aStepName == _L("TestRequestingStatusReports"))
		{
		testStep = new CTestRequestingStatusReports();
		}
	else if(aStepName == _L("TestRequestingStatusReportsWithOption"))
		{
		testStep = new CTestRequestingStatusReportsWithOption();
		}
	else if (aStepName == _L("TestReceivingStatusReports"))
		{
		testStep = new CTestReceivingStatusReports();
		}
	else if (aStepName == _L("TestReceivingStatusReportsSevenBitMultiplePDUs"))
		{
		testStep = new CTestReceivingStatusReportsSevenBitMultiplePDUs();
		}
	else if (aStepName == _L("TestReceivingStatusReportsWithOption"))
		{
		testStep = new CTestReceivingStatusReportsWithOption();
		}
	else if (aStepName == _L("SetDiskMonitorLimits"))
		{
		testStep = new CSetDiskMonitorLimits();
		}
	else if (aStepName == _L("SetDiskSpace"))
		{
		testStep = new CSetDiskSpace();
		}
	else if (aStepName == _L("FreeDiskSpace"))
		{
		testStep = new CFreeDiskSpace();
		}		
	else if (aStepName == _L("ReceiveWapMessage"))
		{
		testStep = new CReceiveWapMessage();
		}
	else if (aStepName == _L("TestEnumeratingVCard"))
		{
		testStep = new CTestEnumeratingVCard();
		}
	else if (aStepName == _L("Test7BitBusinessCardWithEmail"))
		{
		testStep = new CTest7BitBusinessCardWithEmail();
		}
	else if (aStepName == _L("TestOversizedDatagram"))
		{
		testStep = new CTestOversizedDatagram();
		}
	else if (aStepName == _L("TestWapDatagramSegmentContainingNoData"))
        {
        testStep = new CTestWapDatagramSegmentContainingNoData();
        }
	
	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}

	return testStep;
	}
