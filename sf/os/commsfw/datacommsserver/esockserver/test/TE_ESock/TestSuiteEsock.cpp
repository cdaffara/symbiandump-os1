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
// This main DLL entry point for the TE_ESock.dll
// 
//



// EPOC includes
#include <e32base.h>

// Test system includes
#include "TestSuiteEsock.h"
#include "esockopenserver.h"
#include "EsockCloseServer.h"
#include "EsockTestSection1.h"
#include "EsockTestSection2.h"
#include "EsockTestSection3.h"
#include "EsockTestSection4.h"
#include "EsockTestSection5.h"
#include "EsockTestSection6.h"
#include "EsockTestSection7.h"
#include "EsockTestSection8.h"
#include "EsockTestSection9.h"
#include "EsockTestSection10.h"
#include "EsockTestSection11.h"
#include "EsockTestSection12.h"
#include "EsockTestSection13.h"
#include "EsockTestSection14.h"
#include "EsockTestSection15.h"
#include "EsockTestSection16.h"
#include "EsockTestSection17.h"
#include "EsockTestSection18.h"
#include "EsockTestSection19.h"
#include "EsockTestSection20.h"
#include "EsockTestSection21.h"
#include "EsockTestSection22.h"
#include "EsockTestSection23.h"
#include "EsockTestSection24.h"
#include "EsockTestSection25.h"
#include "EsockTestSection26.h"
#include "EsockTestSection27.h"
#include "EsockTestSection28.h"
#include "EsockTestSection29.h"

_LIT(KServerName,"TE_ESock");

CTestSuiteEsock* CTestSuiteEsock::NewL()
/**
* @return - Instance of the test server
* Called inside the MainL() function to create and start the
* CTestServer derived server.
*/
	{
	CTestSuiteEsock * server = new (ELeave) CTestSuiteEsock();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.
	
	//server->StartL(KServerName); 
	server->ConstructL(KServerName);
	server->InitialiseL();
	CleanupStack::Pop(server);
	return server;
	}

// Third phase constructor for ESOCK test suite
void CTestSuiteEsock::InitialiseL( void )
	{
	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
	// it needs a different CommDB
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
    TInt ret = StartC32WithCMISuppressions(KPhbkSyncCMI);
	if ( KErrNone != ret && KErrAlreadyExists != ret )
		{
		User::Leave( ret );
		}

	iTestSuiteHeap = &User::Heap();
	}


// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
* Much simpler, uses the new Rendezvous() call to sync with the client
*/
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	CTestSuiteEsock* testsuite = NULL;
	// Create the CTestServer derived server
	
	TRAPD(err, testsuite=CTestSuiteEsock::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete testsuite;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
* @return - Standard Epoc error code on exit
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

// destructor
CTestSuiteEsock::~CTestSuiteEsock()
	{
	// Delete all the sockets we made
	while(GetSocketListCount())
		{
		CloseSocket(GetSocketListCount());
		}
	
	//	delete iSocketList;
	iSocketArray.Close();
	}


void CTestSuiteEsock::CloseSocket(TInt aIndex)
	{
	const TInt realIndex = aIndex-1;
	
	iSocketArray[realIndex].Close();
	iSocketArray.Remove(realIndex);
	}


/**
Adds the supplied RSocket reference to the suite's socket container
@param RSocket& reference to (just opened) RSocket
@return KErrNone if reference successfully added to container, otherwise one of the system error codes.
 */
TInt CTestSuiteEsock::AddSocketToList(RSocket& aSocket)
	{
	RHeap* testStepHeap = User::SwitchHeap(iTestSuiteHeap);
	TInt res = iSocketArray.Append(aSocket);
	(void)User::SwitchHeap(testStepHeap);
	return res;
	}


TInt CTestSuiteEsock::AddSocketToListL(RSocket& aSocket)
	{
	User::LeaveIfError(AddSocketToList(aSocket));
	return iSocketArray.Count();
	}


TInt CTestSuiteEsock::GetSocketListCount()
	{
	//	return iSocketList->Count();
	return iSocketArray.Count();
	}


RSocket& CTestSuiteEsock::GetSocketHandle(TInt aIndex)
	{
	return iSocketArray[aIndex-1];
	}


// make a version string available for test system 

TPtrC CTestSuiteEsock::GetVersion( void )
	{
	
#ifdef _DEBUG
	_LIT(KTxtVersion,"1.007 (udeb)");
#else
	_LIT(KTxtVersion,"1.007");
#endif
	
	return KTxtVersion();
	}

CTestStep* CTestSuiteEsock::CreateTestStep(const TDesC& aStepName)
/**
* @return - A CTestStep derived instance
* Implementation of CTestServer pure virtual
*/
	{
	START_TEST_STEP_LIST
		// add test steps
		ADD_TEST_STEP(CEsockTest1_1 )
		ADD_TEST_STEP(CEsockTest1_2 )
		ADD_TEST_STEP(CEsockTest1_3 )
		ADD_TEST_STEP(CEsockTest1_4 )
		ADD_TEST_STEP(CEsockTest1_5 )
		ADD_TEST_STEP(CEsockTest1_6 )
		ADD_TEST_STEP(CEsockTest1_7 )
		ADD_TEST_STEP(CEsockTest1_8 )
		ADD_TEST_STEP(CEsockTest1_9 )
		ADD_TEST_STEP(CEsockTest1_10 )
		ADD_TEST_STEP(CEsockTest1_11 )
		ADD_TEST_STEP(CEsockTest1_12 )
		ADD_TEST_STEP(CEsockTest1_13 )
		ADD_TEST_STEP(CEsockTest1_14 )
		ADD_TEST_STEP(CEsockTest1_15 )
		ADD_TEST_STEP(CEsockTest1_16 )
		ADD_TEST_STEP(CEsockTest1_17 )
		ADD_TEST_STEP(CEsockTest2_1 )
		ADD_TEST_STEP(CEsockTest2_2 )
		ADD_TEST_STEP(CEsockTest2_3 )
		ADD_TEST_STEP(CEsockTest2_4 )
		ADD_TEST_STEP(CEsockTest2_5 )
		ADD_TEST_STEP(CEsockTest3_1 )
		ADD_TEST_STEP(CEsockTest3_2 )
		ADD_TEST_STEP(CEsockTest3_3 )
		ADD_TEST_STEP(CEsockTest3_4 )
		ADD_TEST_STEP(CEsockTest3_5 )
		ADD_TEST_STEP(CEsockTest3_6 )
		ADD_TEST_STEP(CEsockTest4_1 )
		ADD_TEST_STEP(CEsockTest4_2 )
		ADD_TEST_STEP(CEsockTest5_1 )
		ADD_TEST_STEP(CEsockTest5_2 )
		ADD_TEST_STEP(CEsockTest5_3 )
		ADD_TEST_STEP(CEsockTest5_4 )
		ADD_TEST_STEP(CEsockTest5_5 )
		ADD_TEST_STEP(CEsockTest5_6 )
		ADD_TEST_STEP(CEsockTest5_7 )
		ADD_TEST_STEP(CEsockTest5_8 )
		ADD_TEST_STEP(CEsockTest5_9 )
		ADD_TEST_STEP(CEsockTest6_1 )
		ADD_TEST_STEP(CEsockTest6_2 )
		ADD_TEST_STEP(CEsockTest6_3 )
		ADD_TEST_STEP(CEsockTest6_4 )
		ADD_TEST_STEP(CEsockTest6_5 )
		ADD_TEST_STEP(CEsockTest6_6 )
		ADD_TEST_STEP(CEsockTest6_7 )
		ADD_TEST_STEP(CEsockTest6_8 )
		ADD_TEST_STEP(CEsockTest7_1 )
		ADD_TEST_STEP(CEsockTest7_2 )
		ADD_TEST_STEP(CEsockTest7_3 )
		ADD_TEST_STEP(CEsockTest7_4 )
		ADD_TEST_STEP(CEsockTest7_5 )
		ADD_TEST_STEP(CEsockTest7_6 )
		ADD_TEST_STEP(CEsockTest7_7 )
		ADD_TEST_STEP(CEsockTest8_1 )
		ADD_TEST_STEP(CEsockTest8_2 )
		ADD_TEST_STEP(CEsockTest8_3 )
		ADD_TEST_STEP(CEsockTest8_4 )
		ADD_TEST_STEP(CEsockTest8_5 )
		ADD_TEST_STEP(CEsockTest8_6 )
		ADD_TEST_STEP(CEsockTest8_7 )
		ADD_TEST_STEP(CEsockTest8_8 )
		ADD_TEST_STEP(CEsockTest8_9 )
		ADD_TEST_STEP(CEsockTest8_10 )
		ADD_TEST_STEP(CEsockTest8_13 )
		ADD_TEST_STEP(CEsockTest8_14 )
		ADD_TEST_STEP(CEsockTest8_15 )
		ADD_TEST_STEP(CEsockTest8_16 )
		ADD_TEST_STEP(CEsockTest9_1 )
		ADD_TEST_STEP(CEsockTest9_2 )
		ADD_TEST_STEP(CEsockTest9_3 )
		ADD_TEST_STEP(CEsockTest9_4 )
		ADD_TEST_STEP(CEsockTest9_5 )
		ADD_TEST_STEP(CEsockTest9_6 )
		ADD_TEST_STEP(CEsockTest10_1 )
		ADD_TEST_STEP(CEsockTest10_2 )
		ADD_TEST_STEP(CEsockTest10_3 )
		ADD_TEST_STEP(CEsockTest11_1 )
		ADD_TEST_STEP(CEsockTest11_2 )
		ADD_TEST_STEP(CEsockTest11_3 )
		ADD_TEST_STEP(CEsockTest11_4 )
		ADD_TEST_STEP(CEsockTest12_1 )
		ADD_TEST_STEP(CEsockTest12_2 )
		ADD_TEST_STEP(CEsockTest12_3 )
		ADD_TEST_STEP(CEsockTest12_4 )
		ADD_TEST_STEP(CEsockTest13_1 )
		ADD_TEST_STEP(CEsockTest13_2 )
		ADD_TEST_STEP(CEsockTest14_1 )
		ADD_TEST_STEP(CEsockTest14_2 )
		ADD_TEST_STEP(CEsockTest14_3 )
		ADD_TEST_STEP(CEsockTest14_4 )
		ADD_TEST_STEP(CEsockTest15_1 )
		ADD_TEST_STEP(CEsockTest15_2 )
		ADD_TEST_STEP(CEsockTest15_3 )
		ADD_TEST_STEP(CEsockTest16_1 )
		ADD_TEST_STEP(CEsockTest16_2 )
		ADD_TEST_STEP(CEsockTest16_3 )
		ADD_TEST_STEP(CEsockTest16_4 )
		ADD_TEST_STEP(CEsockTest16_5 )
		ADD_TEST_STEP(CEsockTest17_1 )
		ADD_TEST_STEP(CEsockTest17_2 )
		ADD_TEST_STEP(CEsockTest17_3 )
		ADD_TEST_STEP(CEsockTest17_4 )
		ADD_TEST_STEP(CEsockTest17_5 )
		ADD_TEST_STEP(CEsockTest17_6 )
		ADD_TEST_STEP(CEsockTest17_7 )
		ADD_TEST_STEP(CEsockTest17_8 )
		ADD_TEST_STEP(CEsockTest17_9 )
		ADD_TEST_STEP(CEsockTest17_10 )
		ADD_TEST_STEP(CEsockTest18_1 )
		ADD_TEST_STEP(CEsockTest19_1 )
		ADD_TEST_STEP(CEsockTest19_2 )
		ADD_TEST_STEP(CEsockTest19_3 )
		ADD_TEST_STEP(CEsockTest19_4 )
		ADD_TEST_STEP(CEsockTest19_5 )
		ADD_TEST_STEP(CEsockTest19_6 )
		ADD_TEST_STEP(CEsockTest19_7 )
		ADD_TEST_STEP(CEsockTest19_8 )
		ADD_TEST_STEP(CEsockTest19_9 )
		ADD_TEST_STEP(CEsockTest20_1 )
		ADD_TEST_STEP(CEsockTest20_2 )
		ADD_TEST_STEP(CEsockTest20_3 )
		ADD_TEST_STEP(CEsockTest20_4 )
		ADD_TEST_STEP(CEsockTest21_1 )
		ADD_TEST_STEP(CEsockTest21_2 )
		ADD_TEST_STEP(CEsockTest21_3 )
		ADD_TEST_STEP(CEsockTest22_1 )
		ADD_TEST_STEP(CEsockTest23_1 )
		ADD_TEST_STEP(CEsockTest23_2 )
		ADD_TEST_STEP(CEsockTest23_3 )
		ADD_TEST_STEP(CEsockTest23_4 )
		ADD_TEST_STEP(CEsockTest23_5 )
		ADD_TEST_STEP(CEsockTest23_6 )
		ADD_TEST_STEP(CEsockTest23_7 )	
		ADD_TEST_STEP(CEsockTest23_8 )
		ADD_TEST_STEP(CEsockTest23_9 )
		ADD_TEST_STEP(CEsockTest23_10 )
		ADD_TEST_STEP(CEsockTest24_1 )
		ADD_TEST_STEP(CEsockTest24_2 )
		ADD_TEST_STEP(CEsockTest25_1 )
		ADD_TEST_STEP(CEsockTest25_2 )
		ADD_TEST_STEP(CEsockTest25_3 )
		ADD_TEST_STEP(CEsockTest25_4 )
		ADD_TEST_STEP(CEsockTest25_5 )
		
		ADD_TEST_STEP(CEsockTest26_1 )
		ADD_TEST_STEP(CEsockTest26_2 )
		ADD_TEST_STEP(CEsockTest26_3 )
		ADD_TEST_STEP(CEsockTest26_4 )
		ADD_TEST_STEP(CEsockTest26_5 )
		ADD_TEST_STEP(CEsockTest26_6 )
		ADD_TEST_STEP(CEsockTest26_7 )
		
		ADD_TEST_STEP(CEsockTest27_1 )
		ADD_TEST_STEP(CEsockTest27_2 )
		ADD_TEST_STEP(CEsockTest27_3 )
		ADD_TEST_STEP(CEsockTest27_4 )
		ADD_TEST_STEP(CEsockTest27_5 )
		ADD_TEST_STEP(CEsockTest27_6 )
		
		ADD_TEST_STEP(CEsockTest27_1_ExplConn)
		ADD_TEST_STEP(CEsockTest27_1_MThread)
		
		
		ADD_TEST_STEP(CEsockTest28_1 )
		ADD_TEST_STEP(CEsockTest28_2 )
		
		ADD_TEST_STEP(CEsockTest29)
		ADD_TEST_STEP(CEsockTest29_10)
		ADD_TEST_STEP(CEsockTest29_11)
		
		ADD_TEST_STEP(CEsockOpenServer )
		ADD_TEST_STEP(CEsockCloseServer )
		
		ADD_TEST_STEP(CEsockSendData )
		ADD_TEST_STEP(CEsockOpenUdpSocket )
		ADD_TEST_STEP(CEsockSendUdpData )
		ADD_TEST_STEP(CEsockSendTcpData )
	END_TEST_STEP_LIST
    }
		

