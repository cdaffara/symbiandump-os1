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
// @file  TS_PARAM_SVR_SUITENAMEServer.cpp
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

// EPOC includes
#include <e32base.h>
#include <e32cmn.h>
#include <c32startshared.h>
#include <comms-infras/c32startcli.h>
#include <c32root.h>

#if (!defined TS_PARAM_SVR_SUITENAME_SERVER_H_)
	#include "CSuite.h"
#endif


//ADD TEST STEP HEADERS HERE
PARAM_FOREACH_MESS_BEGIN
#include "PARAM_MESS_NAME_CStep.h"
PARAM_FOREACH_MESS_END

// __EDIT_ME__ - Substitute the name of your test server
_LIT(KServerName,"Cap_PARAM_SVR_SUITENAME_sc");
// __EDIT_ME__ - Use your own server class name

CTestPARAM_SVR_SUITENAMEServer* CTestPARAM_SVR_SUITENAMEServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
// __EDIT_ME__ - Use your own server class name
	CTestPARAM_SVR_SUITENAMEServer * server = new (ELeave) CTestPARAM_SVR_SUITENAMEServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestStep* CTestPARAM_SVR_SUITENAMEServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	 CTestStep* testStep = NULL;
	// add test steps
PARAM_FOREACH_MESS_BEGIN
		if (aStepName == _L("CPARAM_MESS_NAMEStep"))
			{
			// removed ELeave as harness will test ptr. This is more efficient
			// than using TRAP_IGNORE
			testStep = new CPARAM_MESS_NAMEStep;

			//return because of PARAM_FOREACH_MESS_BEGIN keyword 
			//which is used by capTestWiz.pl to retrive information from table. 
			return testStep;
			}

PARAM_FOREACH_MESS_END
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
	CTestPARAM_SVR_SUITENAMEServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestPARAM_SVR_SUITENAMEServer::NewL());
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
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

TVerdict CCapabilityTestStep::doTestStepPreambleL( void )
	{

	RRsConfigurator iConfigurator;
	TInt result = iConfigurator.Connect();

	if(result == KErrNone)
		{
		INFO_PRINTF1(_L("iConfigurator Connection Sucessful"));
		}
		else
		{
			SetTestStepResult(EFail);
		}
		User::After(100000);
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
	TSecurityInfo info;
	info.Set(RProcess());
	//	INFO_PRINTF4(_L("The loaded Process has: capability: %x with SID:%x and VID: %x"), info.iCaps,info.iSecureId, info.iVendorId);
	INFO_PRINTF1(_L(" "));
	INFO_PRINTF2(_L("The capability of the loaded Process is       [%x] "), info.iCaps);
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
		 //INFO_PRINTF5(_L("Connected to Server(%S) for %S Test [MessageID: %d,Req.Cap: 0x%x,Present.Cap: 0x%x]"),&SR_ServerName,&TestStyle,SR_MESSAGE_ID,iStepCap,TSecurityInfo(RProcess()));
		
		}
		else if(SR_MESSAGE_ID < 0)
		{
			//DEF INFO_PRINTF5(_L("Testing Connection capabilities[%S Test] for Server(%S)  [Req.Cap: 0x%x,Present.Cap: 0x%x]"),&TestStyle,
			//&SR_ServerName,TSecurityInfo(RProcess()));
		}
		else if(!iSessionCreated)// NO Connection
			{
			TSecurityInfo info;
	         info.Set(RProcess());
			RRsConfigurator iConfigurator;
	       TInt result = iConfigurator.Connect();
		   INFO_PRINTF1(_L("iConfigurator Connection Sucessful"));
		   INFO_PRINTF2(_L("Rejected for insufficient capabilities [Return Value : %d] "),tExitReason);
			//INFO_PRINTF4(_L("connected to the Server(%S) ErrorCode - ServerRet: %d C32ret: %d"),&SR_ServerName,iResult_Server,iResult_C32);
		   INFO_PRINTF2(_L("The capability of the loaded Process is       [%x] "), info.iCaps);
	       INFO_PRINTF2(_L("The required capability for the test step is  [%x] "), iStepCap);
	       INFO_PRINTF1(_L("Therefore ...  "));	
		   INFO_PRINTF1(_L("Test Expected to Fail due to lack of capabilities"));
			//INFO_PRINTF3(_L("Child Thread: ExitCategory : %S ExitReason : %d"),&tExitCategory,tExitReason);
  			return EPass;
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

					break;
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

					break;
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
	serverFile.Copy(_L("C32start"));
	//serverFile.Copy(_L("PARAM_SVR_SUITENAME"));

	_LIT(KExe,".exe");
	serverFile.Append(KExe);
	RProcess server;
	err = server.Create(serverFile,_L(""));
	if(err != KErrNone)
		return err;
	// Synchronise with the server
	TRequestStatus reqStatus;
	server.Rendezvous(reqStatus);
	server.Resume();
	 //Server will call the reciprocal static synchronise call
	User::WaitForRequest(reqStatus);
	//server.Close();
	if(reqStatus.Int() != KErrNone)
		return reqStatus.Int();
	server.Close();
	return err;
}




TInt CCapabilityTestStep::TestDebugHeap(TInt* iDbgIPCNo)
 	{

 	//TDbgFns  {MarkHeapStart, MarkHeapEnd, CheckHeap, FailNext, ResetFailNext};
 	TInt aFnToTest= iDbgIPCNo[5];


 	TInt iResult_SR [6] ={0};
 	TInt i = 1;
 	TInt testedFn = 0;

 	TInt dbgTestSequence[5][6]	=	{	{MarkHeapStart	,2,0,1,-1,-1},
 										{MarkHeapEnd	,2,0,1,-1,-1},
 										{CheckHeap		,3,0,2, 1,-1},
 										{FailNext		,4,0,3, 4, 1},
 										{ResetFailNext	,4,0,3, 4, 1}

 									};


 	TInt aCount = dbgTestSequence[aFnToTest][i];

 	while(aCount--  )
 		{
 		testedFn =  dbgTestSequence[aFnToTest][(++i)];

			iResult_SR[testedFn ]= SendReceive( iDbgIPCNo[testedFn],TIpcArgs(((iDbgIPCNo[testedFn]==3 )?4:0),0,0,0));


 		if( ((testedFn !=aFnToTest)?iResult_SR[testedFn]:KErrNone) == KErrPermissionDenied)

			User::Panic(_L("Failed at Initialization"),iResult_SR[testedFn]);

		}

	return iResult_SR[aFnToTest];
	}


