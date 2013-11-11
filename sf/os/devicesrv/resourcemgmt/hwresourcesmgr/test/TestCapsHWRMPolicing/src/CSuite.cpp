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
// @file
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

// EPOC includes
#include <e32base.h>
#include <e32cmn.h>

// Type definitions
#define UNUSED_VAR(a) a = a

#if (!defined TS_HWRM_API_PPLICING_SERVER_H_)
	#include "CSuite.h"
#endif


//ADD TEST STEP HEADERS HERE
#include "HWRMFmTxOn_CStep.h"
#include "HWRMFmTxAutoFreqOff_CStep.h"
#include "HWRMFmTxAutoFreqOn_CStep.h"
#include "HWRMFmTxCancelGetNextClearFreq_CStep.h"
#include "HWRMFmTxCancelSetNextClearFreq_CStep.h"
#include "HWRMFmTxCleanup_CStep.h"
#include "HWRMFmTxClearRdsData_CStep.h"
#include "HWRMFmTxClearRtData_CStep.h"
#include "HWRMFmTxGetFreqRange_CStep.h"
#include "HWRMFmTxGetNextClearFreq_CStep.h"
#include "HWRMFmTxGetNextClearFreqArray_CStep.h"
#include "HWRMFmTxOff_CStep.h"
#include "HWRMFmTxOn_CStep.h"
#include "HWRMFmTxRelease_CStep.h"
#include "HWRMFmTxReserve_CStep.h"
#include "HWRMFmTxSetFreq_CStep.h"
#include "HWRMFmTxSetNextClearFreq_CStep.h"
#include "HWRMFmTxSetRadiotext_CStep.h"
#include "HWRMFmTxSetRadiotextPlus1_CStep.h"
#include "HWRMFmTxSetRadiotextPlus2_CStep.h"
#include "HWRMFmTxSetRadiotextPlus3_CStep.h"
#include "HWRMFmTxSetRdsLanguageId_CStep.h"
#include "HWRMFmTxSetRdsMs_CStep.h"
#include "HWRMFmTxSetRdsPty_CStep.h"
#include "HWRMFmTxSetRdsPtyn_CStep.h"



// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"Cap_HWRM_API_Policing_sc");
// __EDIT_ME__ - Use your own server class name

CTestHWRM_API_PolicingServer* CTestHWRM_API_PolicingServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
// __EDIT_ME__ - Use your own server class name
	CTestHWRM_API_PolicingServer * server = new (ELeave) CTestHWRM_API_PolicingServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestStep* CTestHWRM_API_PolicingServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	 CTestStep* testStep = NULL;
	// add test steps

		if (aStepName == _L("CHWRMFmTxOnStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxOnStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxOffStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxOffStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetFreqStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetFreqStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetNextClearFreqStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetNextClearFreqStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxAutoFreqOnStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxAutoFreqOnStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetRdsPtyStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetRdsPtyStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetRdsPtynStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetRdsPtynStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetRdsMsStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetRdsMsStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetRdsLanguageIdStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetRdsLanguageIdStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxClearRdsDataStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxClearRdsDataStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetRadiotextStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetRadiotextStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetRadiotextPlus1Step"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetRadiotextPlus1Step  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetRadiotextPlus2Step"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetRadiotextPlus2Step  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxSetRadiotextPlus3Step"))
			{
			testStep =  new(ELeave) CHWRMFmTxSetRadiotextPlus3Step  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxClearRtDataStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxClearRtDataStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxReserveStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxReserveStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxReleaseStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxReleaseStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxGetFreqRangeStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxGetFreqRangeStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxGetNextClearFreqStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxGetNextClearFreqStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxGetNextClearFreqArrayStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxGetNextClearFreqArrayStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxCancelGetNextClearFreqStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxCancelGetNextClearFreqStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxCancelSetNextClearFreqStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxCancelSetNextClearFreqStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxCleanupStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxCleanupStep  ;
			return testStep;
			}
		else if (aStepName == _L("CHWRMFmTxAutoFreqOffStep"))
			{
			testStep =  new(ELeave) CHWRMFmTxAutoFreqOffStep  ;
			return testStep;
			}

		return testStep;
	 }

	

// Secure variants much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
// __EDIT_ME__ - Use your own server class name
	CTestHWRM_API_PolicingServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestHWRM_API_PolicingServer::NewL());
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
	UNUSED_VAR(err); //Used to supress build warnings
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


TVerdict CCapabilityTestStep::doTestStepPreambleL( void )
	{
	//If Preamble is not required just pass a success value
	TVerdict testResult = CTestStep::doTestStepPreambleL();
	
	
	return TestStepResult();
	
	}
TVerdict CCapabilityTestStep::doTestStepPostambleL( void )
	{
	//If Postamble is not required just pass a success value
	TVerdict testResult = CTestStep::doTestStepPostambleL();

	return TestStepResult();
	
	}

// Moved from CStep.cpp

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
	TCapabilitySet theCaps =  TSecurityInfo(RProcess()).iCaps ;
	const TInt KMaxTestThreadHeapSize = 0x10000;

	//Initialize return values
	iResult_SR = iResult_Server = KErrNone;	
		

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
	serverFile.Copy(_L("hwrmserver"));
	_LIT(KExe,".exe");
	serverFile.Append(KExe);
	RProcess server;
	err = server.Create(serverFile,_L(""));
		    
	if(err != KErrNone)
		{
		return err;
		}
	
	// Synchronise with the server
	TRequestStatus reqStatus;
	server.Rendezvous(reqStatus);
	server.Resume();
	
	// Server will call the reciprocal static synchronise call
	User::WaitForRequest(reqStatus);
	
	if(reqStatus.Int() != KErrNone)
		{
		return reqStatus.Int();
		}
	return err;
	}

TInt CCapabilityTestStep::TestDebugHeap(TInt* aDbgIPCNo)
	{
     
	//TDbgFns  {MarkHeapStart, MarkHeapEnd, CheckHeap, FailNext, ResetFailNext};
	TInt aFnToTest= aDbgIPCNo[5];
     
	TInt iResult_SR [6] ={0};
    TInt i = 1;
	TInt testedFn = 0;

	TInt dbgTestSequence[5][6]  =   {   {MarkHeapStart  ,2,0,1,-1,-1}, 
										{MarkHeapEnd    ,2,0,1,-1,-1},
										{CheckHeap      ,3,0,2, 1,-1},
										{FailNext       ,4,0,3, 4, 1},
										{ResetFailNext  ,4,0,3, 4, 1}
									};
     
	TInt aCount = dbgTestSequence[aFnToTest][i];

	while(aCount--  )
		{
		testedFn =  dbgTestSequence[aFnToTest][(++i)];
     
		iResult_SR[testedFn ]= SendReceive( aDbgIPCNo[testedFn],TIpcArgs(((aDbgIPCNo[testedFn]==3 )?4:0),0,0,0));
    
		if( ((testedFn !=aFnToTest)?iResult_SR[testedFn]:KErrNone) == KErrPermissionDenied)             
        	User::Panic(_L("Failed at Initialization"),iResult_SR[testedFn]);       

		}

	return iResult_SR[aFnToTest];
	}
