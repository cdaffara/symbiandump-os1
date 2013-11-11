// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// For (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file
*/

// EPOC includes
#include <e32base.h>
#include <e32cmn.h> 
#include <c32root.h>



#if (!defined TS_RootServerPlatSecSuite_SERVER_H_)
	#include "CSuite.h"
#endif


//ADD TEST STEP HEADERS HERE
#include "RSSetMBufPoolSize1_CStep.h"
#include "RSIsCallerConfigurator1_CStep.h"
#include "RSLoadModule1_CStep.h"
#include "RSUnloadModule1_CStep.h"
#include "RSBind1_CStep.h"
#include "RSUnbind1_CStep.h"
#include "RSShutdown1_CStep.h"
#include "RSGetModuleInfo1_CStep.h"
#include "RSEnumerateModules1_CStep.h"
#include "RSEnumerateSubModules1_CStep.h"
#include "RSEnumerateBindings1_CStep.h"
#include "RSCancelLoadModule1_CStep.h"
#include "RSCancelUnloadModule1_CStep.h"
#include "RSCancelBind1_CStep.h"
#include "RSCancelUnbind1_CStep.h"
#include "RSCloseSession1_CStep.h"
#include "RSDbgMarkHeap1_CStep.h"
#include "RSDbgCheckHeap1_CStep.h"
#include "RSDbgMarkEnd1_CStep.h"
#include "RSDbgFailNext1_CStep.h"
#include "RSDbgFailNextMbuf1_CStep.h"
#include "RSDbgSetMbufPoolLimit1_CStep.h"
#include "RSDbgCheckMbuf1_CStep.h"
#include "RSDbgMbufFreeSpace1_CStep.h"
#include "RSDbgMbufTotalSpace1_CStep.h"

// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"Te_Cap_RootServer_sc");
// __EDIT_ME__ - Use your own server class name

_LIT(KRootServerExe, "c32exe.exe");



/**
 @return - Instance of the test server
 Same code for Secure and non-secure variants
 Called inside the MainL() function to create and start the
 CTestServer derived server.
*/
CTestRootServerPlatSecSuiteServer* CTestRootServerPlatSecSuiteServer::NewL()
	{
// __EDIT_ME__ - Use your own server class name
	CTestRootServerPlatSecSuiteServer * server = new (ELeave) CTestRootServerPlatSecSuiteServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

/**
 @return - A CTestStep derived instance
 Secure and non-secure variants
 Implementation of CTestServer pure virtual
*/
CTestStep* CTestRootServerPlatSecSuiteServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	// add test steps
	
	// remove ELeave as harness will test ptr. This is more efficient
	// than using TRAP_IGNORE

	if (aStepName == _L("CRSSetMBufPoolSize1Step"))
		{
		testStep =  new CRSSetMBufPoolSize1Step;
 		}
	else			 
	if (aStepName == _L("CRSIsCallerConfigurator1Step"))
		{
		testStep =  new CRSIsCallerConfigurator1Step;
		}
	else
	if (aStepName == _L("CRSLoadModule1Step"))
		{
		testStep =  new CRSLoadModule1Step;
 		}
	else
	if (aStepName == _L("CRSUnloadModule1Step"))
		{
		testStep =  new CRSUnloadModule1Step;
 		}
	else			 			 
	if (aStepName == _L("CRSBind1Step"))
		{
		testStep =  new CRSBind1Step;
 		}
	else			 
	if (aStepName == _L("CRSUnbind1Step"))
		{
		testStep =  new CRSUnbind1Step;
 		}
	else			 
	if (aStepName == _L("CRSShutdown1Step"))
		{
		testStep =  new CRSShutdown1Step;
 		}
	else			 
	if (aStepName == _L("CRSGetModuleInfo1Step"))
		{
		testStep =  new CRSGetModuleInfo1Step;
 		}
	else			 
	if (aStepName == _L("CRSEnumerateModules1Step"))
		{
		testStep =  new CRSEnumerateModules1Step;
 		}
	else			 
	if (aStepName == _L("CRSEnumerateSubModules1Step"))
		{
		testStep =  new CRSEnumerateSubModules1Step;
 		}
	else			 
	if (aStepName == _L("CRSEnumerateBindings1Step"))
		{
		testStep =  new CRSEnumerateBindings1Step;
 		}
	else			 
	if (aStepName == _L("CRSCancelLoadModule1Step"))
		{
		testStep =  new CRSCancelLoadModule1Step;
 		}
	else			 
	if (aStepName == _L("CRSCancelUnloadModule1Step"))
		{
		testStep =  new CRSCancelUnloadModule1Step;
 		}
	else			 
	if (aStepName == _L("CRSCancelBind1Step"))
		{
		testStep =  new CRSCancelBind1Step;
 		}
	else			 
	if (aStepName == _L("CRSCancelUnbind1Step"))
		{
		testStep =  new CRSCancelUnbind1Step;
 		}
	else			 
	if (aStepName == _L("CRSCloseSession1Step"))
		{
		testStep =  new CRSCloseSession1Step;
 		}
	else			 
	if (aStepName == _L("CRSDbgMarkHeap1Step"))
		{
		testStep =  new CRSDbgMarkHeap1Step;
 		}
	else			 
	if (aStepName == _L("CRSDbgCheckHeap1Step"))
		{
		testStep =  new CRSDbgCheckHeap1Step;
 		}
	else			 
	if (aStepName == _L("CRSDbgMarkEnd1Step"))
		{
		testStep =  new CRSDbgMarkEnd1Step;
 		}
	else			 
	if (aStepName == _L("CRSDbgFailNext1Step"))
		{
		testStep =  new CRSDbgFailNext1Step;
 		}
	else			 
	if (aStepName == _L("CRSDbgFailNextMbuf1Step"))
		{
		testStep =  new CRSDbgFailNextMbuf1Step;
 		}
	else			 
	if (aStepName == _L("CRSDbgSetMbufPoolLimit1Step"))
		{
		testStep =  new CRSDbgSetMbufPoolLimit1Step;
 		}
	else			 
	if (aStepName == _L("CRSDbgCheckMbuf1Step"))
		{
		testStep =  new CRSDbgCheckMbuf1Step;
 		}
	else			 
	if (aStepName == _L("CRSDbgMbufFreeSpace1Step"))
		{
		testStep =  new CRSDbgMbufFreeSpace1Step;
 		}
	else			 
	if (aStepName == _L("CRSDbgMbufTotalSpace1Step"))
		{
		testStep =  new CRSDbgMbufTotalSpace1Step;
 		}

	return testStep;
	}

	

// Secure variants much simpler
// Just an E32Main and a MainL()
/**
 Secure variant
 Much simpler, uses the new Rendezvous() call to sync with the client
*/
LOCAL_C void MainL()
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
// __EDIT_ME__ - Use your own server class name
	CTestRootServerPlatSecSuiteServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CTestRootServerPlatSecSuiteServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


/**
 @return - Standard Epoc error code on process exit
 Secure variant only
 Process entry point. Called by client using RProcess API
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


TVerdict CCapabilityTestStep::doTestStepPreambleL( void )
	{
	//If Preamble is not required just pass a success value
	(void) CTestStep::doTestStepPreambleL();
	
	if (StartRootServer() != KErrNone)
		{
		SetTestStepResult(EInconclusive);
		}
	return TestStepResult();
	}


TVerdict CCapabilityTestStep::doTestStepPostambleL( void )
	{
	//If Postamble is not required just pass a success value
	(void) CTestStep::doTestStepPostambleL();

	return TestStepResult();
	}


TInt CCapabilityTestStep::StartRootServer()
    {    
    TInt res;
    RRootServ rootServer;

	// find server, if its there the job is done
	res = rootServer.Connect ();
	if (res == KErrNone) 
		{
    	INFO_PRINTF1(_L("RootServer already running."));
    	rootServer.Close ();
    	return KErrNone;
		}
		
    if (res == KErrNotFound) 
    	{
		INFO_PRINTF1(_L("RootServer not running, attempting to start it."));

    	RProcess process;
    	res = process.Create (KRootServerExe, KNullDesC);

		TRequestStatus status;
    	if (res == KErrNone)
			{
			process.Rendezvous (status);
			process.Resume ();
			User::WaitForRequest (status);
			process.Close ();
			res = status.Int ();
			}
    	}
   	INFO_PRINTF2(_L("RootServer startup completed with [Return Value: %d] "), res);
    
	res = rootServer.Connect ();
   	INFO_PRINTF2(_L("RootServer Connect completed with [Return Value: %d] "), res);
	if (res == KErrNone) rootServer.Close ();
	
    return res;
    }



enum TVerdict CCapabilityTestStep::doTestStepL()
	{
	//DEF! INFO_PRINTF2(_L("%S - Starting ..."), &iTestStepName);

	//The MainThread()creates a separate thread that executes SendReceive
	TVerdict vResult = MainThread();
	
	SetTestStepResult(vResult);
	return TestStepResult();
	}

/*
ThreadStartFn:
Called by:	The Child thread
Function:	Calls the Exec_SendReceive
*/
static TInt ThreadStartFn( TAny * ptr )
	{
	return(((CCapabilityTestStep *)ptr)->Exec_SendReceive());
	}

/*
TVerdict GetVerdict(TInt aAPIretValue)

Called by: "MainThread" for returning verdict

Parameters(TInt aRetValue) :	0 if API call gets thru without any rejection
								1 if API call is rejected for capability error	
*/
enum TVerdict CCapabilityTestStep::GetVerdict(TInt aAPIretValue)
	{
	TSecurityInfo info;
	info.Set(RProcess());

//	INFO_PRINTF4(_L("The loaded Process has: capability: %x with SID:%x and VID: %x"), info.iCaps,info.iSecureId, info.iVendorId);
	INFO_PRINTF1(_L(" "));
	TInt cap = *reinterpret_cast<TInt*>(&(info.iCaps)); //Just a naughty hack to get the capabilities out
	INFO_PRINTF2(_L("The capability of the loaded Process is       [%x] "), cap);
	INFO_PRINTF2(_L("The required capability for the test step is  [%x] "), iStepCap);
	INFO_PRINTF1(_L("Therefore ...  "));
	TVerdict vVerdict[] = {EPass, EFail};

	//please leave the following if/else block as the information printed by INFO_PRINTF1 is used bu CapTestSumm
	if(iExpect_Rejection)//[Inverse Test] EPass for 1 while EFail for 0
		{
		INFO_PRINTF1(_L("Test Expected to Fail due to lack of capabilities"));
		return vVerdict[(aAPIretValue)?0:1];
		
		}
	else //[Direct Test] EPass for 0 while EFail for 1
		{
		INFO_PRINTF1(_L("Test Expected to Pass with correct capabilities"));
		return vVerdict[(aAPIretValue)?1:0];
		}
	}


/*
TVerdict MainThread()

Called by: "doTestStepL"

Purpose:	Creates the child thread(which calls the respective function with regard
			to the server and also implements the Message Call). Then this fn.waits for the
			completion of the childthread( doesnt matter how the thread did die!)

Return Value(Verdict of the TestStep):

			A.Reporting PASS/FAIL
				Direct Test:
						When a message call gets thru. Please note that in such cases
						actually the implementation of the message has started. As we
						are passing "0" Parameters, server may panic, though our botheration
						stops once the call gets thru.
						NOTE:	The style is the same when CONNECTION capabilities
								are tested, the only diff is you dont have to expect a 
								panic from server
				Inverse Test:
						The call should be either failed or panicked with
						"KErrPermissionDenied" flag. 

				General Case:
						If a thread creation failed or if the server couldnt be connected
						apart from the above two cases, then a FAIL is reported
			
			B.Reporting INCONCLUSIVE
						Any panic say from unexpected source (eg:KERN-EXEC) will be
						reported INCONCLUSIVE														
*/
TVerdict CCapabilityTestStep::MainThread()
	{

	TBuf<100>	tExitCategory;
	TInt		tExitReason = 0;		
	TBuf<100>	TestStyle;

	iExpect_Rejection?TestStyle = _L("Inverse"):TestStyle = _L("Direct");
	const TInt KMaxTestThreadHeapSize = 0x10000;

	//Initialize return values
	iResult_SR = KErrNone;
	iResult_Server = KErrNone;
		

	// Create a child thread, with a new heap
	TInt nRes_Thread =	tChildThread.Create(
						ChildThread_SR,
						ThreadStartFn,
						KDefaultStackSize,
						KMinHeapSize,
						KMaxTestThreadHeapSize,
						this,			
						EOwnerProcess);


	if(nRes_Thread == KErrNone)//Thread Created
		{

		//Let me know when the thread is dead
		TRequestStatus ThreadStatus;
		tChildThread.Logon(ThreadStatus);
		tChildThread.Resume(); 
		//Is the thread dead?
		User::WaitForRequest( ThreadStatus );
		
		//yes, he is dead. RIP!  Now the Killer's profile
		tExitCategory	=	tChildThread.ExitCategory();
		tExitReason		=	tChildThread.ExitReason();
		
	
		//Somebody Please say what are we testing!!		
		if(iSessionCreated && (SR_MESSAGE_ID >=0))//Flag set by Child thread when connected to Server
		{
			//DEF INFO_PRINTF5(_L("Connected to Server(%S) for %S Test [MessageID: %d,Req.Cap: 0x%x,Present.Cap: 0x%x]"),&SR_ServerName,&TestStyle,SR_MESSAGE_ID,iStepCap,TSecurityInfo(RProcess()));			
		}
		else if(SR_MESSAGE_ID < 0)
		{
			//DEF INFO_PRINTF5(_L("Testing Connection capabilities[%S Test] for Server(%S)  [Req.Cap: 0x%x,Present.Cap: 0x%x]"),&TestStyle,
			//&SR_ServerName,TSecurityInfo(RProcess()));			
		}
		else if(!iSessionCreated)// NO Connection
			{
			INFO_PRINTF4(_L("Couldnt connect to the Server(%S) ErrorCode - ServerRet: %d C32ret: %d"),&SR_ServerName,iResult_Server,iResult_C32);
			//INFO_PRINTF3(_L("Child Thread: ExitCategory : %S ExitReason : %d"),&tExitCategory,tExitReason);
  			return EFail;
 			}



		switch(tChildThread.ExitType())
			{			
			case EExitPanic:
				//1.A Panic from the connected Server 
				//2.A CServer Panic normally for capability rejection
				//3.A kernel Panic (consider yourself doomed!)
				if((tExitReason == KErrPermissionDenied) ||
					//DEF ? 	it's old version (tExitReason == CServer::EClientDoesntHaveRequiredCaps))//Rejected for Insufficient Cap.
					// is it correct ?
					(tExitReason == CServer2::EClientDoesntHaveRequiredCaps))//Rejected for Insufficient Cap.
					{
					INFO_PRINTF2(_L("Rejected for insufficient capabilities [Return Value : %d] "),tExitReason);
					return(GetVerdict(API_RetValue_PermissionDenied));
					}
				else if(tExitCategory == iServer_Panic) //Panic from Server
					{
					INFO_PRINTF2(_L("Server(%S) Panic to child thread"),&tExitCategory);	
					INFO_PRINTF3(_L("Child Thread: ExitCategory : %S ExitReason : %d"),&tExitCategory,tExitReason);			
					return(GetVerdict(API_RetValue_ServerPanic));
					}
				else//A kernel Panic possibly
					{
					INFO_PRINTF3(_L("Child Thread: Panic from unexpected source (ExitCategory: %S ExitReason : %d)!"),&tExitCategory,tExitReason);
					return EInconclusive;
					}
//					break;	// commented to avoid compiler warning
					
			case EExitKill:
				if(iResult_SR != KErrPermissionDenied)
					{
					INFO_PRINTF2(_L("A Successfull call (Return Value : %d)"),((SR_MESSAGE_ID >=0)?iResult_SR:iResult_Server));
					return(GetVerdict(API_RetValue_NoCapError));
					}
				else 
					{
					INFO_PRINTF2(_L("Rejected for insufficient capabilities [Return Value : %d] "),((SR_MESSAGE_ID >=0)?iResult_SR:iResult_Server));
					return(GetVerdict(API_RetValue_PermissionDenied));			
					}
//					break;	// commented to avoid compiler warning
					
			default:					
					break;
			}
		}			
	else //Our thread couldnt start	:o(
		{
		INFO_PRINTF2(_L("ERROR: Failed to create Child thread,  ErrorCode:(%d)"),nRes_Thread);
		return EFail;
		}

	return EInconclusive;
	}


TInt CCapabilityTestStep::StartServer()
	{
	TInt err =  KErrNone ;
	
	// EKA2 is simple No path required
	TBuf<32> serverFile;
	serverFile.Copy(_L("RootServerPlatSecSuite"));
	_LIT(KExe,".exe");
	serverFile.Append(KExe);
	
	RProcess server;
	err = server.Create(serverFile, KNullDesC);
	if(err != KErrNone)	return err;
	
	// Synchronise with the server
	TRequestStatus reqStatus;
	server.Rendezvous(reqStatus);
	server.Resume();
	
	 //Server will call the reciprocal static synchronise call
	User::WaitForRequest(reqStatus);
	server.Close();
	
	if(reqStatus.Int() != KErrNone) return reqStatus.Int();
	return err;
	}
	

TInt CCapabilityTestStep::TestDebugHeap(TInt* iDbgIPCNo)
 	{
 	//TDbgFns  {MarkHeapStart, MarkHeapEnd, CheckHeap, FailNext, ResetFailNext};
 	TInt aFnToTest= iDbgIPCNo[5];
 	
 	TInt iResult_SR [6] = {0};
 	TInt i = 1;
 	TInt testedFn = 0;
 
 	TInt dbgTestSequence[5][6]	=
 		{
		{MarkHeapStart	,2,0,1,-1,-1}, 
		{MarkHeapEnd	,2,0,1,-1,-1},
		{CheckHeap		,3,0,2, 1,-1},
		{FailNext		,4,0,3, 4, 1},
		{ResetFailNext	,4,0,3, 4, 1}
		};
 
 	
 	TInt aCount = dbgTestSequence[aFnToTest][i];
 
 	while(aCount--)
 		{
 		testedFn =  dbgTestSequence[aFnToTest][(++i)];
 		
		iResult_SR[testedFn ]= SendReceive( iDbgIPCNo[testedFn],TIpcArgs(((iDbgIPCNo[testedFn]==3 )?4:0),0,0,0));
   		
 		
 		if( ((testedFn !=aFnToTest)?iResult_SR[testedFn]:KErrNone) == KErrPermissionDenied) 			
 			{
			User::Panic(_L("Failed at Initialization"),iResult_SR[testedFn]);
 			}
		}

	return iResult_SR[aFnToTest];
	}
	

