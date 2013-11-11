// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// code at the __EDIT_ME__ tags
// Secure and non-secure variants
// Secure EKA2 versions will be a simpler xxxServer.exe running in its own process.
// Non-secure versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file
*/
#include "te_gsmu.h"
#include "te_gsmuStep.h"

_LIT(KServerName,"te_gsmu");

CTgsmuServer* CTgsmuServer::NewL()
/**
 *  @return - Instance of the test server
 *  Same code for Secure and non-secure variants
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
{
	CTgsmuServer * server = new (ELeave) CTgsmuServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
}


LOCAL_C void MainL()
/**
 *  Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	// __EDIT_ME__ Your server name
	CTgsmuServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTgsmuServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 *  @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;

	return err;
    }


CTestStep* CTgsmuServer::CreateTestStep(const TDesC& aStepName)
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
	if(aStepName == KTgsmuTestAlphabetConversion7Bit)
		{
		testStep = new CTgsmuTestAlphabetConversion7Bit();
		}
	else if(aStepName == KTgsmuTestAlphabetConversion8Bit)
		{
		testStep = new CTgsmuTestAlphabetConversion8Bit();
		}
	else if(aStepName == KTgsmuTestAlphabetConversionUCS2)
		{
		testStep = new CTgsmuTestAlphabetConversionUCS2();
		}
	else if(aStepName == KTgsmuTestAlphabetConversionBinary)
		{
		testStep = new CTgsmuTestAlphabetConversionBinary();
		}
   	else if(aStepName == KTgsmuTestAlphabetConversionSplit)
		{
		testStep = new CTgsmuTestAlphabetConversionSplit();
		}
   	else if(aStepName == KTgsmuTestSmsHeader)
		{
		testStep = new CTgsmuTestSmsHeader();
		}
   	else if(aStepName == KTgsmuTestDecodePDU)
		{
		testStep = new CTgsmuTestDecodePDU();
		}
   	else if(aStepName == KTgsmuTestCommandDataHeader)
		{
		testStep = new CTgsmuTestCommandDataHeader();
		}
   	else if(aStepName == KTgsmuTestSmsProtocolIdentifier)
		{
		testStep = new CTgsmuTestSmsProtocolIdentifier();
		}
   	else if(aStepName == KTgsmuTestSmsTONandNPI)
		{
		testStep = new CTgsmuTestSmsTONandNPI();
		}
   	else if(aStepName == KTgsmuTestSmsOptimiseSettings)
		{
		testStep = new CTgsmuTestSmsOptimiseSettings();
		}
   	else if(aStepName == KTgsmuTestSmsDefaultVersion)
		{
		testStep = new CTgsmuTestSmsDefaultVersion();
		}
   	else if(aStepName == KTgsmuTestSmsCurrentVersion)
		{
		testStep = new CTgsmuTestSmsCurrentVersion();
		}
   	else if(aStepName == KTgsmuTestSmsVersionBackwards)
		{
		testStep = new CTgsmuTestSmsVersionBackwards();
		}
   	else if(aStepName == KTgsmuTestSMSCCtrlParameterOperations)
		{
		testStep = new CTgsmTgsmuTestSMSCCtrlParameterOperations();
		}
   	else if(aStepName == KTgsmuTestTPSRROperations)
		{
		testStep = new CTgsmTgsmuTestTPSRROperations();
		}
	else if(aStepName == KTgsmuTestCSmsEditorBuffer)
		{
		testStep = new CTgsmuTestCSmsEditorBuffer();
		}
	else if(aStepName == KTgsmuTestBCDAddress)
		{
		testStep = new CTgsmuTestBCDAddress();
		}
	else if(aStepName == _L("TestAlternative7bitEncodingAPIs"))
		{
		testStep = new CTestAlternative7bitEncodingAPIs();
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
