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
// Definitions for creating the test suite and test steps of ETelMM's API
// test harness.
// 
//

/**
 @file
*/


#include "TE_EtelMMTestServer.h"

#include "TE_mmcanceller.h"
#include "TE_mmconference.h"
#include "TE_mmsmsstore.h"
#include "TE_mmmemsmsstore.h"
#include "TE_mmnamstore.h"
#include "TE_mmmemnamstore.h"
#include "TE_mmonstore.h"
#include "TE_mmmemonstore.h"
#include "TE_mmenstore.h"
#include "TE_mmmemenstore.h"
#include "TE_mmphonebookstore.h"
#include "TE_mmphonebookstoreutils.h"
#include "TE_mmsms.h"
#include "TE_mmmemsms.h"
#include "TE_mmbroadcast.h"
#include "TE_mmmembroadcast.h"
#include "TE_mmussd.h"
#include "TE_mmcall.h"
#include "TE_mmline.h"
#include "TE_mmphone.h"
#include "TE_mmmemphone.h"
#include "TE_mmlist.h"
#include "TE_mmmemlist.h"
#include "TE_mmloaddrv.h"
#include "TE_mmauthorization.h"
#include "TE_mmauthenticate.h"
#include "TE_mmsmartcardeap.h"
#include "te_mmsmartcardauth.h"
#include "TE_mmNotifyMtlr.h"
#include "TE_mmSendMtlrResponse.h"
#include "TE_mmNotifyMeasurementControl.h"
#include "TE_mmSendMeasurementReport.h"
#include "TE_mmSendMolr.h"
#include "TE_mmCancelLbsRequests.h"
#include "te_mmsetemergencyclient.h"
#include "te_mmsetpriorityclient.h"
#include "te_mmdialcallcontrol.h"
#include "TE_MMEtelRecorder.h"
#include "te_mmnotifyuepositioningreset.h"

CEtelMMTestServer* CEtelMMTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CEtelMMTestServer * testServer = new (ELeave) CEtelMMTestServer();
	CleanupStack::PushL(testServer);
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	testServer->ConstructL(serverName.Name());
	CleanupStack::Pop(testServer);
	return testServer;
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
	CEtelMMTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CEtelMMTestServer::NewL());
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

CTestStep* CEtelMMTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
           
	//Load any components used by the different tests
    if(aStepName == _L("TestMMLoadDrv"))
    	{
    	testStep = new CTestMMLoadDrv;
    	}

	//Test Mobile Phone Functionality
    else if(aStepName == _L("TestPhone"))
    	{
    	testStep = new CTestPhone;
    	}

	// Test Line Functions
    else if(aStepName == _L("TestLine"))
    	{
    	testStep = new CTestLine;
    	}

	// Test Call Functions
    else if(aStepName == _L("TestCall"))
    	{
    	testStep = new CTestCall;
    	}
    else if(aStepName == _L("TestCall-2nd-part"))
    	{
    	testStep = new CTestCallb;
    	}
    else if(aStepName == _L("TestCall-3rd-part"))
    	{
    	testStep = new CTestCallc;
    	}
    else if(aStepName == _L("TestCall-ISV"))
    	{
    	testStep = new CTestCallISV;
    	}
  
	// Test Broadcast Messaging Functions
    else if(aStepName == _L("TestBrdcastMessaging"))
    	{
    	testStep = new CTestBroadcastMessaging;
    	}
	// Test USSD Messaging Functions
    else if(aStepName == _L("TestUssdMessaging"))
    	{
    	testStep = new CTestUssdMessaging;
    	}
	// Test SMS Messaging Functions
    else if(aStepName == _L("TestSmsMessaging"))
    	{
    	testStep = new CTestSmsMessaging;
    	}
	// Test SMS Storage Functions
    else if(aStepName == _L("TestSmsStore"))
    	{
    	testStep = new CTestSmsStore;
    	}
	// Test Nam Storage Functions
    else if(aStepName == _L("TestNamStore"))
    	{
    	testStep = new CTestNamStore;
    	}
	// Test ON Storage Functions
    else if(aStepName == _L("TestOnStore"))
    	{
    	testStep = new CTestOnStore;
    	}
	// Test Emergency Number Functions
    else if(aStepName == _L("TestEnStore"))
    	{
    	testStep = new CTestEnStore;
    	}
	// Test Phonebook Functions
    else if(aStepName == _L("TestPhonebookStore"))
    	{
    	testStep = new CTestPhonebookStore;
    	}
	//Test PhoneBook TLV Utility Library Functionality
    else if(aStepName == _L("TestPhonebookStoreU"))
    	{
    	testStep = new CTestPhonebookStoreUtils;
    	}
	//Test Mobile Phone Conference Call Functionality
    else if(aStepName == _L("TestConferenceCall"))
    	{
    	testStep = new CTestConferenceCall;
    	}
	//Test API List class functionality
    else if(aStepName == _L("TestMMLists"))
    	{
    	testStep = new CTestMMLists;
    	}
    //Test EAP functionality
    else if(aStepName == _L("TestMMSmartCardBasedEap"))
    	{
    	testStep = new CTestSmartCardEap;
    	}
    //Test EAP functionality with bad client
    else if(aStepName == _L("TestMMSmartCardBasedEapBadClient"))
    	{
    	testStep = new CTestInitEapWithoutClosing;
    	}
    //Test Smart Card Authenticate
    else if(aStepName == _L("TestMMSmartCardAuthenticate"))
    	{
    	testStep = new CTestSmartCardAuthenticate;
    	}

	// Out of Memory tests

	//Test Out Of Memory Mobile Phone Functionality
    else if(aStepName == _L("TestMemPhone"))
    	{
    	testStep = new CTestMemPhone;
    	}
	//Test Out Of Memory Broadcast Messaging Functions
    else if(aStepName == _L("TestMemBrdcastMess"))
    	{
    	testStep = new CTestMemBroadcastMessaging;
    	}
	//Test Out Of Memory SMS Messaging Functions
    else if(aStepName == _L("TestMemSmsMessaging"))
    	{
    	testStep = new CTestMemSmsMessaging;
    	}
	//Test Out Of Memory SMS Storage Functions
    else if(aStepName == _L("TestMemSmsStore"))
    	{
    	testStep = new CTestMemSmsStore;
    	}
	//Test Out Of Memory Nam Storage Functions
    else if(aStepName == _L("TestMemNamStore"))
    	{
    	testStep = new CTestMemNamStore;
    	}
	//Test Out Of Memory ON Storage Functions
    else if(aStepName == _L("TestMemOnStore"))
    	{
    	testStep = new CTestMemOnStore;
    	}
	//Test Out Of Memory Emergency Number Functions
    else if(aStepName == _L("TestMemEnStore"))
    	{
    	testStep = new CTestMemEnStore;
    	}
	//Test Out Of Memory API List class functionality
    else if(aStepName == _L("TestMemMMLists"))
    	{
    	testStep = new CTestMemMMLists;
    	}
	// Test Out Of Authorization Data Retrieval
    else if(aStepName == _L("TestAuthorizationData"))
    	{
    	testStep = new CEtelMMTestAuthorizationData;
    	}
	// Test Out Of Authenticate
    else if(aStepName == _L("TestAuthenticate"))
    	{
    	testStep = new CEtelMMTestAuthenticate;
    	}
 
	//Unload the components used by the different tests
    else if(aStepName == _L("TestMMUnLoadDrv"))
    	{
    	testStep = new CTestMMUnLoadDrv;
    	}

    //Test for LocationServices

    else if(aStepName == _L("NotifyMtlr"))
    	{
    	testStep = new CTestNotifyMtlr;
    	}

    else if(aStepName == _L("SendMtlrResponse"))
    	{
    	testStep = new CTestSendMtlrResponse;
    	}

    else if(aStepName == _L("NotifyMeasurementControl"))
    	{
    	testStep = new CTestNotifyMeasurementControl;
    	}

    else if(aStepName == _L("SendMeasurementReport"))
    	{
    	testStep = new CTestSendMeasurementReport;
    	}

    else if(aStepName == _L("SendMolr"))
    	{
    	testStep = new CTestSendMolr;
    	}    	

    else if(aStepName == _L("CancelLbsRequests"))
    	{
    	testStep = new CTestCancelLbsRequests;
    	}  

    else if(aStepName == _L("TestMMSetEmgClientLcsReqPreallocHeap"))
    	{
    	testStep = new CTestMMSetEmgClientLcsReqPreallocHeap;
    	}

    else if(aStepName == _L("TestMMSetEmgClientLcsReqAndSetPrClient"))
    	{
    	testStep = new CTestMMSetEmgClientLcsReqAndSetPrClient;
    	}

    else if(aStepName == _L("TestMMSetEmgClientVoiceReqAndSetPrClient"))
    	{
    	testStep = new CTestMMSetEmgClientVoiceReqAndSetPrClient;
    	}

    else if(aStepName == _L("TestMMSetEmgClientHeapSuffForLBS"))
    	{
    	testStep = new CTestMMSetEmgClientHeapSuffForLBS;
    	}

    else if(aStepName == _L("TestMMSetEmgClientVoiceReqPreallocHeap"))
    	{
    	testStep = new CTestMMSetEmgClientVoiceReqPreallocHeap;
    	}  	

    else if(aStepName == _L("TestMMSetPrClientPreallocHeap"))
    	{
    	testStep = new CTestMMSetPrClientPreallocHeap;
    	}   

    else if(aStepName == _L("TestMMSetPrClientAndSetEmgClient"))
    	{
    	testStep = new CTestMMSetPrClientAndSetEmgClient;
    	} 

    else if(aStepName == _L("TestMMSetPrClientV2AndSetEmgClientLcsReq"))
    	{
    	testStep = new CTestMMSetPrClientV2AndSetEmgClientLcsReq;
    	}   

    else if(aStepName == _L("TestMMSetPrClientV2AndSetEmgClientVoiceReq"))
    	{
    	testStep = new CTestMMSetPrClientV2AndSetEmgClientVoiceReq;
    	} 

    else if(aStepName == _L("TestMMSetPrClientV2AndSetEmgClientVoiceOrLbsReq"))
    	{
    	testStep = new CTestMMSetPrClientV2AndSetEmgClientVoiceOrLbsReq;
    	}
    // Caps and SID checking for RMobileCall::DialCallControl
    else if (aStepName == _L("TestDialCallControlCorrectSIDCorrectCap"))
    	{
    	testStep = new CTestDialCallControlCorrectSIDCorrectCap();
    	}
    else if (aStepName == _L("TestDialCallControlIncorrectSIDCorrectCap"))
    	{
    	testStep = new CTestDialCallControlIncorrectSIDCorrectCap();
    	}

    else if (aStepName == _L("TestDialCallControlCorrectSIDIncorrectCap"))
    	{
    	testStep = new CTestDialCallControlCorrectSIDIncorrectCap();
    	}
    else if (aStepName == _L("TestDialCallControlIncorrectSIDIncorrectCap"))
    	{
    	testStep = new CTestDialCallControlIncorrectSIDIncorrectCap();
    	}
    else if (aStepName == _L("TestRecorder"))
		{
		testStep = new CTestRecorder();
		}
    else if (aStepName == _L("TestNotifyUePositioningReset"))
    	{
    	testStep = new CTestNotifyUePositioningReset();
    	}
 	return testStep;
	}
