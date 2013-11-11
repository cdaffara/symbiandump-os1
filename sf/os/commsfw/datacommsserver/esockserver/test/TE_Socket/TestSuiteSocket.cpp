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
// Test cases for the socket class
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file TestStepSocket.cpp
*/
#include "TestSuiteSocket.h"
#include "SocketTestSection1.h"
#include "SocketTestSection2.h"
#include "SocketTestSection3.h"
#include "SocketTestSection4.h"
#include "SocketTestSection5.h"
#include "SocketTestSection6.h"
#include "SocketTestSection7.h"
#include "SocketTestSection8.h"
#include "SocketTestSection9.h"
#include "SocketTestSection10.h"
#include "SocketTestSection11.h"
#include "SocketTestSection12.h"
#include "SocketTestSection13.h"
#include "SocketTestSection14.h"
#include "SocketTestSection15.h"
#include "SocketTestSection16.h"
#include "SocketTestSection17.h"
#include "SocketTestSection18.h"
#include "SocketTestSection19.h"
#include "SocketTestSection20.h"
#include "SocketTestSection21.h"
#include "SocketTestSection31.h"

_LIT(KServerName,"TE_Socket");

CTestServerSocket* CTestServerSocket::NewL()
/**
@return - Instance of the test server
Called inside the MainL() function to create and start the
CTestServer derived server.
*/
	{
	CTestServerSocket * server = new (ELeave) CTestServerSocket();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.
	
	//server->StartL(KServerName); 
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
Much simpler, uses the new Rendezvous() call to sync with the client
*/
	{
	// Leave the hooks in for platform security

	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	CTestServerSocket* server = NULL;
	// Create the CTestServer derived server
	
	TRAPD(err,server = CTestServerSocket::NewL());
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
@return - Standard Epoc error code on exit
*/
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

CTestServerSocket::CTestServerSocket()
	{
	}

CTestServerSocket::~CTestServerSocket()
	{
	}


CTestStep* CTestServerSocket::CreateTestStep(const TDesC& aStepName)
/**
@return - A CTestStep derived instance
Implementation of CTestServer pure virtual
*/
	{
	START_TEST_STEP_LIST
		ADD_TEST_STEP(CSocketTest1_1)
		ADD_TEST_STEP(CSocketTest1_2)
		ADD_TEST_STEP(CSocketTest1_3)
		ADD_TEST_STEP(CSocketTest1_4)
		ADD_TEST_STEP(CSocketTest1_5)
		ADD_TEST_STEP(CSocketTest1_6)
		ADD_TEST_STEP(CSocketTest1_7)
		ADD_TEST_STEP(CSocketTest1_8)
		ADD_TEST_STEP(CSocketTest1_9)
		ADD_TEST_STEP(CSocketTest1_10)
		
		ADD_TEST_STEP(CSocketTest2_1)
		
		ADD_TEST_STEP(CSocketTest3_1)
		
		ADD_TEST_STEP(CSocketTest4_1)
		
		ADD_TEST_STEP(CSocketTest5_1)
		ADD_TEST_STEP(CSocketTest5_2)
		ADD_TEST_STEP(CSocketTest5_3)
		ADD_TEST_STEP(CSocketTest5_4)
		ADD_TEST_STEP(CSocketTest5_5)
		
		ADD_TEST_STEP(CSocketTest6_1)
		ADD_TEST_STEP(CSocketTest6_2)
		ADD_TEST_STEP(CSocketTest6_3)
		ADD_TEST_STEP(CSocketTest6_4)
		ADD_TEST_STEP(CSocketTest6_5)
		
		ADD_TEST_STEP(CSocketTest7_1)
		ADD_TEST_STEP(CSocketTest7_2)
		ADD_TEST_STEP(CSocketTest7_3)
		ADD_TEST_STEP(CSocketTest7_4)
		
		ADD_TEST_STEP(CSocketTest8_1)
		ADD_TEST_STEP(CSocketTest8_2)
		ADD_TEST_STEP(CSocketTest8_3)
		ADD_TEST_STEP(CSocketTest8_4)
		ADD_TEST_STEP(CSocketTest8_5)
		ADD_TEST_STEP(CSocketTest8_6)
		ADD_TEST_STEP(CSocketTest8_7)
		ADD_TEST_STEP(CSocketTest8_8)
		
		ADD_TEST_STEP(CSocketTest9_1)
		ADD_TEST_STEP(CSocketTest9_2)
		ADD_TEST_STEP(CSocketTest9_3)
		ADD_TEST_STEP(CSocketTest9_4)
		ADD_TEST_STEP(CSocketTest9_5)
		ADD_TEST_STEP(CSocketTest9_6)
		ADD_TEST_STEP(CSocketTest9_7)
		ADD_TEST_STEP(CSocketTest9_8)
		ADD_TEST_STEP(CSocketTest9_9)
		
		ADD_TEST_STEP(CSocketTest10_1)
		ADD_TEST_STEP(CSocketTest10_2)
		ADD_TEST_STEP(CSocketTest10_3)
		
		ADD_TEST_STEP(CSocketTest11_1)
		ADD_TEST_STEP(CSocketTest11_2)
		ADD_TEST_STEP(CSocketTest11_3)
		ADD_TEST_STEP(CSocketTest11_4)
			
		ADD_TEST_STEP(CSocketTest12_1)
		ADD_TEST_STEP(CSocketTest12_2)
		
		ADD_TEST_STEP(CSocketTest13_1)
		
		ADD_TEST_STEP(CSocketTest14_1)
		ADD_TEST_STEP(CSocketTest14_2)
		ADD_TEST_STEP(CSocketTest14_3)
		ADD_TEST_STEP(CSocketTest14_4)
		ADD_TEST_STEP(CSocketTest14_5)
		
		ADD_TEST_STEP(CSocketTest15_0)
		ADD_TEST_STEP(CSocketTest15_1)
		ADD_TEST_STEP(CSocketTest15_2)
		ADD_TEST_STEP(CSocketTest15_3)
		ADD_TEST_STEP(CSocketTest15_4)
		ADD_TEST_STEP(CSocketTest15_5)
		
		ADD_TEST_STEP(CSocketTest16_1)
		
		ADD_TEST_STEP(CSocketTest17_1)
		ADD_TEST_STEP(CSocketTest17_2)
		
		ADD_TEST_STEP(CSocketTest18_1)
		ADD_TEST_STEP(CSocketTest18_2)
		ADD_TEST_STEP(CSocketTest18_3)
		
		ADD_TEST_STEP(CSocketTest19_1)
		ADD_TEST_STEP(CSocketTest19_2)
		
		ADD_TEST_STEP(CSocketTest20_1)
		ADD_TEST_STEP(CSocketTest20_2)
		ADD_TEST_STEP(CSocketTest20_3)
		ADD_TEST_STEP(CSocketTest20_4)
		ADD_TEST_STEP(CSocketTest20_5)
		ADD_TEST_STEP(CSocketTest21_1)		
		ADD_TEST_STEP(CSocketTest21_2)
		
		ADD_TEST_STEP(CSocketTest31_1)
		ADD_TEST_STEP(CSocketTest31_2)
		ADD_TEST_STEP(CSocketTest31_3)
		ADD_TEST_STEP(CSocketTest31_4)
		ADD_TEST_STEP(CSocketTest31_5)
	END_TEST_STEP_LIST
	}



