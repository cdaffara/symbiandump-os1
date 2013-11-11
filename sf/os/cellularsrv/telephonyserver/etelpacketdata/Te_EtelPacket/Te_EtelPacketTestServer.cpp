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
//

#include "Te_EtelPacketTestServer.h"

#include "testdef.h"
#include "Te_EtelPacketTest1_1.h"
#include "Te_EtelPacketTest1_2.h"
#include "Te_EtelPacketTest1_3.h"
#include "Te_EtelPacketTest3g_1.h"
#include "Te_EtelPacketTest3g_2.h"
#include "Te_EtelPacketTest3g_3.h"
#include "Te_EtelPacketTest1_1Extended.h"
#include "Te_EtelPacketTestMultipleClients.h"
#include "Te_EtelPacketTest_Rel99.h"
#include "Te_EtelPacketTestAuthenticationAndDNS.h"
#include "Te_EtelPacketTestPCO_IE.h"
#include "Te_EtelPacketTest_Rel5.h"
#include "Te_EtelPacketMbmsSupport.h"
#include "Te_EtelPacketMbmsService.h"
#include "Te_EtelPacketMbmsContext.h"
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
#include "Te_EtelPacketTestContextConnectionInfo.h"
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
#include "Te_EtelPacketTestSteps.h"
#include <etel.h>

_LIT(KServerName,"Te_EtelPacket");

CEtelPacketTestServer* CEtelPacketTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CEtelPacketTestServer * testServer = new (ELeave) CEtelPacketTestServer();
	CleanupStack::PushL(testServer);

	testServer->ConstructL(KServerName);
	CleanupStack::Pop(testServer);
	return testServer;
	}

CEtelPacketTestServer::~CEtelPacketTestServer()
	{
	iContext.Close();
	iPhone.Close();
	iPacketService.Close();
	iTelServer.Close();
	}
	
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
   	TInt result = StartC32();
	if (result != KErrNone  &&  result != KErrAlreadyExists)
		{
		User::Leave(result);
		}

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CEtelPacketTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CEtelPacketTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete testServer;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
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

CTestStep* CEtelPacketTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == _L("Authentication_And_DNS_Server_Names"))
		{
		testStep = new CEtelPacketTestAuthenticationAndDNS;
    	}
	else if(aStepName == _L("Release_99_Service"))
		{
		testStep = new CEtelPacketTest_Rel99;
    	}
	else if(aStepName == _L("Service"))
		{
		testStep = new CEtelPacketTest1_1;
    	}
	else if(aStepName == _L("Context"))
		{
		testStep = new CEtelPacketTest1_2;
    	}
	else if(aStepName == _L("QoS"))
		{
		testStep = new CEtelPacketTest1_3;
    	}
	else if(aStepName == _L("3GPacketService"))
		{
		testStep = new CEtelPacketTest3G_1;
    	}
	else if(aStepName == _L("3GPacketContext"))
		{
		testStep = new CEtelPacketTest3G_2;
    	}
	else if(aStepName == _L("3GPacketQoS"))
		{
		testStep = new CEtelPacketTest3G_3;
    	}
	else if(aStepName == _L("Extended_Service"))
		{
		testStep = new CEtelPacketTest1_1Extended;
    	}
	else if(aStepName == _L("Multiple_Clients"))
		{
		testStep = new CEtelPacketTestMultipleClients;
    	}
	else if(aStepName == _L("PCO_IE"))
		{
		testStep = new CEtelPacketTestPCO_IE;
    	}
	else if(aStepName == _L("Release_5_Service"))
		{
		testStep = new CEtelPacketTest_Rel5;	
		}
	//Defect Fix Test Case
	else if(aStepName == KConnectEtelLoadTSY)
		{
		testStep = new CTestConnectEtelLoadTSY(iTelServer);	
		}
	else if(aStepName == KOpenPhone)
		{
		testStep = new CTestOpenPhone(iTelServer, iPhone);	
		}
	else if(aStepName == KClosePhone)
		{
		testStep = new CTestClosePhone(iPhone);	
		}
	else if(aStepName == KDisconnectEtelUnloadTSY)
		{
		testStep = new CTestDisconnectEtelUnloadTSY(iTelServer);	
		}
	else if(aStepName == KOpenPacketService)
		{
		testStep = new CTestOpenPacketService(iPacketService, iPhone);	
		}
	else if(aStepName == KClosePacketService)
		{
		testStep = new CTestClosePacketService(iPacketService);	
		}			
	else if(aStepName == KOpenContext)
		{
		testStep = new CTestOpenContext(iTelServer, iPacketService, iContext, iContextName);	
		}
	else if(aStepName == KCloseContext)
		{
		testStep = new CTestCloseContext(iContext);	
		}
	else if(aStepName == KMbmsSupport)
		{
		testStep = new CTestMbmsSupport;	
		}
	else if(aStepName == KMbmsService)
		{
		testStep = new CTestMbmsService;	
		}
	else if(aStepName == KMbmsContext)
		{
		testStep = new CTestMbmsContext;	
		}
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	else if(aStepName == KContextConnectionInfo)
		{
		testStep = new CEtelPacketTestContextConnectionInfo;	
		}
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}

	return testStep;
	}
