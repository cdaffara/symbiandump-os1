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
// @file  TS_lbsprivfwcapServer.cpp
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

// EPOC includes
#include <e32base.h>
#include <e32cmn.h> 
#include <lbs/lbsadmin.h>

#include "lbsnetinternalapi.h"
#include "cprocesslaunch.h"
#include "tprocessstartparams.h"
#include "lbsassistancedatacacheapi.h"

#include "lbssystemcontroller.h"

#if (!defined TS_lbsprivfwcap_SERVER_H_)
	#include "CSuite.h"
#endif


//ADD TEST STEP HEADERS HERE
#include "ELbsPrivacyServerConnect1_CStep.h"
#include "ELbsPrivacyServerDisconnect1_CStep.h"
#include "ELbsPrivacyServerNotifyLocation1_CStep.h"
#include "ELbsPrivacyServerVerifyLocation1_CStep.h"
#include "LbsPrvacyServerNotifyVerificationTimeout1_CStep.h"
#include "LbsPrvcySrverCancelVerifyLocationRequest1_CStep.h"
#include "ELbsPrivacyServerNewPrivacyRequestLocal1_CStep.h"
#include "ELbsPrivacyServerRepeatPrivacyRequestLocal1_CStep.h"
#include "ELbsPrivacyServerCompleteRequest1_CStep.h"
#include "ELbsPrivacyServerNewPrivacyRequestNetwork1_CStep.h"
#include "ELbsPrivacyServerRepeatPrivacyRequestNetwork1_CStep.h"


//_LIT(KNetGatewayExeName, "lbsnetgateway.exe");
_LIT(KServerName,"Cap_lbsprivfwcap_sc");

CTestlbsprivfwcapServer* CTestlbsprivfwcapServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestlbsprivfwcapServer * server = new (ELeave) CTestlbsprivfwcapServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestStep* CTestlbsprivfwcapServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	 CTestStep* testStep = NULL;
	// add test steps
		if (aStepName == _L("CELbsPrivacyServerConnect1Step"))
			{
			testStep =  new(ELeave) CELbsPrivacyServerConnect1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CELbsPrivacyServerDisconnect1Step"))
			{
			testStep =  new(ELeave) CELbsPrivacyServerDisconnect1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CELbsPrivacyServerNotifyLocation1Step"))
			{
			testStep =  new(ELeave) CELbsPrivacyServerNotifyLocation1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CELbsPrivacyServerVerifyLocation1Step"))
			{
			testStep =  new(ELeave) CELbsPrivacyServerVerifyLocation1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CLbsPrvacyServerNotifyVerificationTimeout1Step"))
			{
			testStep =  new(ELeave) CLbsPrvacyServerNotifyVerificationTimeout1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CLbsPrvcySrverCancelVerifyLocationRequest1Step"))
			{
			testStep =  new(ELeave) CLbsPrvcySrverCancelVerifyLocationRequest1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CELbsPrivacyServerNewPrivacyRequestLocal1Step"))
			{
			testStep =  new(ELeave) CELbsPrivacyServerNewPrivacyRequestLocal1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CELbsPrivacyServerRepeatPrivacyRequestLocal1Step"))
			{
			testStep =  new(ELeave) CELbsPrivacyServerRepeatPrivacyRequestLocal1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CELbsPrivacyServerCompleteRequest1Step"))
			{
			testStep =  new(ELeave) CELbsPrivacyServerCompleteRequest1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CELbsPrivacyServerNewPrivacyRequestNetwork1Step"))
			{
			testStep =  new(ELeave) CELbsPrivacyServerNewPrivacyRequestNetwork1Step  ;
			return testStep;
			 }
		 
		if (aStepName == _L("CELbsPrivacyServerRepeatPrivacyRequestNetwork1Step"))
			{
			testStep =  new(ELeave) CELbsPrivacyServerRepeatPrivacyRequestNetwork1Step  ;
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
	CTestlbsprivfwcapServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestlbsprivfwcapServer::NewL());
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
	return err;
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

					//break;
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

					//break;
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
	CTrapCleanup* mycleanup = CTrapCleanup::New();
	if(mycleanup == NULL)
		{
		return KErrNoMemory;
		}
	TInt ret = KErrGeneral;	// in case next line leaves and ret doesn't get set
	TRAPD(err, ret = doStartServer());
	(void)err;
	delete mycleanup;
	return ret;
	}
	

TInt CCapabilityTestStep::doStartServer()
	{
	RLbsSystemController systemController;
	
	systemController.OpenL(KLbsNetworkPrivacyApiUid);
	
	CleanupClosePushL(systemController);
	
	TInt startErr = systemController.RequestSystemStartup();
	
	CleanupStack::PopAndDestroy(&systemController);
	
	systemController.Close();
	if (ELbsStartupSuccess == startErr)
		{
		startErr = KErrNone;
		}
	return (startErr);
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
	
 
