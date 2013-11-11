// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TE_c32server.h"
#include "TE_c32.h"

_LIT(KServerName,"TE_C32");

CC32TestServer* CC32TestServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CC32TestServer * server = new (ELeave) CC32TestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CC32TestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CC32TestServer::NewL());
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
 * @return - Standard Epoc error code on exit
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

CTestStep* CC32TestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep*  testStep = NULL;

	//
	// Here the test step is created when it is needed. Note that this
	// function is non-leaving so we cannot use new(ELeave). Therefore
	// the new could return NULL, but that is not a problem as it implies
	// the test step is missing and this will be marked in the log file.
	//

	// Loopback test
	if(aStepName == _L("TestLoopback"))
		{
		testStep = new CTestLoopback;
		}
				
	// Old TCOMM tests
	
	if(aStepName == _L("TestInit"))
		{
		testStep = new CTestInit;
		}

	if(aStepName == _L("TestDoubleStart"))
		{
		testStep = new CTestDoubleStart;
		}

	if(aStepName == _L("TestOpen"))
		{
		testStep = new CTestOpen;
		}

	if(aStepName == _L("TestInfo"))
		{
		testStep = new CTestInfo;
		}

	if(aStepName == _L("TestDoubleRead"))
		{
		testStep = new CTestDoubleRead;	
		}
			
	if(aStepName == _L("TestConfigPanic"))
		{
		testStep = new CTestConfigPanic;			
		}
			
	if(aStepName == _L("TestBadDescriptorPanic"))
		{
		testStep = new CTestBadDescriptorPanic;			
		}

	if(aStepName == _L("TestBusyReturn"))
		{
		testStep = new CTestBusyReturn;						
		}

	if(aStepName == _L("TestCancel"))
		{
		testStep = new CTestCancel;						
		}

	if(aStepName == _L("TestGetSetConfig"))
		{
		testStep = new CTestGetSetConfig;					
		}

	if(aStepName == _L("TestTimeOut"))
		{
		testStep = new CTestTimeOut;				
		}

	if(aStepName == _L("TestThreadPanic"))
		{
		testStep = new CTestThreadPanic;				
		}

	if(aStepName == _L("TestBufferOptions"))
		{
		testStep = new CTestBufferOptions;				
		}

	if(aStepName == _L("TestOOMConnection"))
		{
		testStep = new CTestOOMConnection;							
		}
			
	if(aStepName == _L("TestOOMPortCreation"))
		{
		testStep = new CTestOOMPortCreation	;
		}

	if(aStepName == _L("TestSetSignalsPanicBug"))
		{
		testStep = new CTestSetSignalsPanicBug;			
		}

	if(aStepName == _L("TestStartServerThread"))
		{
		testStep = new CTestStartServerThread;						
		}
						
	if(aStepName == _L("TestCapsV02"))
		{
		testStep = new CTestCapsV02;						
		}

	if(aStepName == _L("TestOpenPortsWithHighNumbers"))
		{
		testStep = new CTestOpenPortsWithHighNumbers;									
		}

	if(aStepName == _L("TestTimerAllocation"))
		{
		testStep = new CTestTimerAllocation;									
		}

	if(aStepName == _L("TestReadWrite"))
		{
		testStep = new CTestReadWrite;									
		}

	if(aStepName == _L("TestReadWrite2"))
		{
		testStep = new CTestReadWrite2;									
		}

	if(aStepName == _L("TestSignalLines"))
		{
		testStep = new CTestSignalLines	;
		}

	if(aStepName == _L("TestQueryreceiveBuffer"))
		{
		testStep = new CTestQueryreceiveBuffer;
		}

	if(aStepName == _L("TestWrite0"))
		{
		testStep = new CTestWrite0;			
		}
			
	if(aStepName == _L("TestReadOneOrMore"))
		{
		testStep = new CTestReadOneOrMore;		
		}

	if(aStepName == _L("TestBufferReadWrite"))
		{
		testStep = new CTestBufferReadWrite;					
		}

	if(aStepName == _L("TestBufferreadOneOrMore"))
		{
		testStep = new CTestBufferreadOneOrMore;	
		}

	if(aStepName == _L("TestNotifySignals"))
		{
		testStep = new CTestNotifySignals;	
		}

	if(aStepName == _L("TestRxDataAvailable"))
		{
		testStep = new CTestRxDataAvailable;	
		}

	if(aStepName == _L("TestHelloEllo"))
		{
		testStep = new CTestHelloEllo;
		}

	// Old T_LSC32 tests

	if(aStepName == _L("Lsc32ListAvailablePorts"))
		{
		testStep = new CLsc32ListAvailablePorts;
		}

	if(aStepName == _L("Lsc32ListNumberOfLoadedCSYs"))
		{
		testStep = new CLsc32ListNumberOfLoadedCSYs;			
		}
			
	if(aStepName == _L("Lsc32LoadUnloadCsy"))
		{
		testStep = new CLsc32LoadUnloadCsy;			
		}
			
	if(aStepName == _L("Lsc32GetPortInfo"))
		{
		testStep = new CLsc32GetPortInfo;			
		}
			
	// Old TDUMMY tests
	
	if(aStepName == _L("DummyTest"))
		{
		testStep = new CDummyTest;	
		}
			
	// Old TCOMMNEW tests
	
	if(aStepName == _L("TestDCEPort"))
		{
		testStep = new CTestDCEPort;						
		}
		
	if(aStepName == _L("TestPlatSecOpenPortDummy"))
		{
		testStep = new CTestPlatSecOpenPortDummy;						
		}

	if(aStepName == _L("TestAllocMoreWriteMem"))
		{
		testStep = new CTestAllocMoreWriteMem;
		}
		
	if(aStepName == _L("TestMemoryWriteFail"))
		{
		testStep = new CTestMemoryWriteFail;
		}
		
	if(aStepName == _L("TestResetBuffers"))
		{
		testStep = new CTestResetBuffers;
		}
	
	if(aStepName == _L("TestCorrectDataRead"))
		{
		testStep = new CTestCorrectDataRead;
		}
				
	if(aStepName == _L("TestMaxBufferOverflow"))
		{
		testStep = new CTestMaxBufferOverflow;
		}
	if(aStepName == _L("TestOpenWhenAvailable"))
		{
		testStep = new CTestOpenWhenAvailable;			
		}
	
//PREQ 890
//Test Case No NET-C32-I-001-HP
   if(aStepName == _L("TestLoadDealerCSY"))
		{
		testStep = new CTestLoadDealerCSY;						
		}
//Test Case No NET-C32-I-002-HP
	if(aStepName == _L("TestLoadPlayerCSY"))
		{
		testStep = new CTestLoadPlayerCSY;						
		}
//Test Case No NET-C32-I-003-HP
	if(aStepName == _L("TestLoadCSYthread"))
		{
		testStep = new CTestLoadCSYthread;						
		}		
  		
 //Test Case No NET-C32-I-004-HP
 if(aStepName == _L("TestLoadMoreCSY"))
		{
		testStep = new CTestLoadMoreCSY;						
		}
			
//Test Case No NET-C32-I-005-HP
 if(aStepName == _L("TestApiCallCSY"))
		{
		testStep = new CTestApiCallCSY;						
		}
		
//Test Case No NET-C32-I-006-HP
if(aStepName == _L("TestLoadUnknownCSY"))
		{
		testStep = new CTestLoadUnknownCSY;						
		}

//Test case No NET-C32-I-007-HP		
 if(aStepName == _L("TestMissingRoleParmCMI"))
		{
		testStep = new CTestMissingRoleParmCMI;						
		}	

//Test case No NET-C32-I-008-HP
	if(aStepName == _L("TestMissingWorkerIDParm"))
		{
		testStep = new CTestMissingWorkerIDParm;						
		}	
//Test case No NET-C32-I-009-HP		
 if(aStepName == _L("TestLoadUnLoadCsySameOrder"))
		{
		testStep = new CTestLoadUnLoadCsySameOrder;						
		}	
		
//Test case No NET-C32-I-0010-HP		
 if(aStepName == _L("TestLoadUnLoadCsyDiffOrder"))
		{
		testStep = new CTestLoadUnLoadCsyDiffOrder;						
		}
			
//Test case No NET-C32-I-0011-HP		
 if(aStepName == _L("SendDatainDealerThr"))
		{
		testStep = new CSendDatainDealerThr;						
		}
					
//Test case No NET-C32-I-0012-HP		
 if(aStepName == _L("SendDatainPlayerThr"))
		{
		testStep = new CSendDatainPlayerThr;						
		}
			
 //Test case No NET-C32-I-0013-HP		
 if(aStepName == _L("TestGetPortInfo"))
		{
		testStep = new CTestGetPortInfo;						
		}
			
 //Test case No NET-C32-I-0014-HP							
if(aStepName == _L("LoadingCsydiffSession"))
		{
		testStep = new CLoadingCsydiffSession;						
		}
								
//Test case No NET-C32-I-0015-HP	
if(aStepName == _L("LoadingSameCsydiffSession"))
		{
		testStep = new CLoadingSameCsydiffSession;						
		}
		
//Test case No NET-C32-I-0016-HP		
if(aStepName == _L("NumPortsdiffSession"))
		{
		testStep = new CNumPortsdiffSession;						
		}
		
//Test case No NET-C32-I-0017-HP		
if(aStepName == _L("DataReadDealerPlayer"))
		{
		testStep = new CDataReadDealerPlayer;						
		}
		
//Test case No NET-C32-I-0019-HP
if(aStepName == _L("TestStackableCSY"))
		{
		testStep = new CTestStackableCSY;						
		}

//Test case No NET-C32-I-0020-HP
if(aStepName == _L("LoadCsywithOldCMI"))
		{
		testStep = new CLoadCsywithOldCMI;						
		}
//Test case No NET-C32-I-0021-HP		
if(aStepName == _L("OldandNewCMIfiles"))
		{
		testStep = new COldandNewCMIfiles;						
		}	
//Test case No NET-C32-I-0022-HP				
if(aStepName == _L("LoadCsyPlayerThread"))
		{
		testStep = new CLoadCsyPlayerThread;						
		}	
//Test case No NET-C32-I-0023-HP
if(aStepName == _L("LoadCsyDealerPlayerThreads"))
		{
		testStep = new CLoadCsyDealerPlayerThreads;						
		}
//Test case No NET-C32-I-0024-HP			
if(aStepName == _L("LoadandUnloadDiffSession"))
		{
		testStep = new CLoadandUnloadDiffSession;						
		}
//Test case No NET-C32-I-0025-HP		
if(aStepName == _L("MissingCsyListinCMI"))
		{
		testStep = new CMissingCsyListinCMI;						
		}
				
//Test case No NET-C32-I-0026-HP
if(aStepName == _L("AllCMIFileRolePlayer"))
		{
		testStep = new CAllCMIFileRolePlayer;						
		}
		
//Test case No NET-C32-I-0027-HP
if(aStepName == _L("TwoCMIfileSetWildcard"))
		{
		testStep = new CTwoCMIfileSetWildcard;						
		}
//Test case No NET-C32-I-0028-HP		
if(aStepName == _L("NoCMIfileSetWildcard"))
		{
		testStep = new CNoCMIfileSetWildcard;						
		}
//Test case No NET-C32-I-0029-HP			
if(aStepName == _L("TwoCMIsameWorkerID"))
		{
		testStep = new CTwoCMIsameWorkerID;						
		}
//Test case No NET-C32-I-0030-HP			
if(aStepName == _L("TwoNewCMIfileRoleDealer"))
		{
		testStep = new CTwoNewCMIfileRoleDealer;						
		}
//Test case No NET-C32-I-0031-HP			
if(aStepName == _L("OldNewCMIfileRoleDealer"))
		{
		testStep = new COldNewCMIfileRoleDealer;						
		}		
							
 //Test case No NET-C32-I-0034-HP			
if(aStepName == _L("LoadCsyAllPlayerThread"))
		{
		testStep = new CLoadCsyAllPlayerThread;						
		}	
		
 //Test case No NET-C32-I-0035-HP			
if(aStepName == _L("GetPortInfo"))
		{
		testStep = new CGetPortInfo;						
		}			
//Test case No NET-C32-I-0032-HP		
if(aStepName == _L("TestBlockCSY"))
		{
		testStep = new CTestBlockCSY;						
		}			
//Test case No NET-C32-I-0033-HP	
if(aStepName == _L("TestCorruptCsy"))
		{
		testStep = new CTestCorruptCsy;						
		}	
	
if(aStepName == _L("GetPortInfoByName"))
		{
		testStep = new CGetPortInfoByName;						
		}
//Test case No NET-C32-I-0036-HP					
if(aStepName == _L("UnSequentialWorkerID"))
		{
		testStep = new CUnSequentialWorkerID;						
		}
 //Test case No NET-C32-I-0037-HP					
if(aStepName == _L("TestLoadUnknownCPM"))
		{
		testStep = new CTestLoadUnknownCPM;						
		}

 //Test case No NET-C32-I-0038-HP					
if(aStepName == _L("TestLongPortName"))
		{
		testStep = new CTestLongPortName;						
		}
			
//Test case No NET-C32-I-0039-HP					
if(aStepName == _L("TestStackableCSYNoClose"))
		{
		testStep = new CTestStackableCSYNoClose;						
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
	
	

