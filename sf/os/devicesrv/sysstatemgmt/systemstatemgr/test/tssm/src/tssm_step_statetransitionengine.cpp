// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <ssm/ssmcommandlist.h>

#include "ssmstatepolicyframe.h"
#include "ssmstatetransitionengine.h"

#include "tssm_step_statetransitionengine.h"
#include <ssm/ssmcmd.hrh>

const TInt KFailStateCommandList1Id = 456346321;
	  

//----------------------------------------------------------------------------------------------------------------
//-------------- TCall - a util class for logging the calls made by the CSsmStateTrasnitionEngine under test -----
//----------------------------------------------------------------------------------------------------------------

// from MSsmStatePolicyResolverProxy --------------------------------------------------
//		void  CSsmStateTransitionEngineTest::GetStatePolicyL(TSsmState aState)
typedef void (CSsmStateTransitionEngineTest::*TFuncPtrType1)(TSsmState aState);
//		CSsmStatePolicy&  CSsmStateTransitionEngineTest::StatePolicy    (void)
typedef MSsmStatePolicy& (CSsmStateTransitionEngineTest::*TFuncPtrType2)(void);

// from MCleSessionProxy ------------------------------------------------------------------
//		void  CSsmStateTransitionEngineTest::ExecuteCommandListL(const CSsmCommandList& aCmdList, TRequestStatus& aStatus)
typedef void (CSsmStateTransitionEngineTest::*TFuncPtrType3)    (const CSsmCommandList& aCmdList, TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity );
//		void  CSsmStateTransitionEngineTest::ExecuteCommandListCancel(void)
typedef void (CSsmStateTransitionEngineTest::*TFuncPtrType10)      (void);

// from MSsmStatePolicy --------------------------------------------------------------
//		void  PrepareCommandList		  (TSsmState aState, TInt aReason, TRequestStatus& aStatus);
typedef void (CTestPolicy::*TFuncPtrType4)(TSsmState aState, TInt aReason, TRequestStatus& aStatus);
//		TBool  CTestPolicy::GetNextState     (TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
typedef TBool (CTestPolicy::*TFuncPtrType5)  (TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);
//		CSsmStatePolicy::TResponse  CTestPolicy::TransitionAllowed  (const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
typedef MSsmStatePolicy::TResponse (CTestPolicy::*TFuncPtrType8)	(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage);
//		void Cancel();
//		void NewL();
typedef void (CTestPolicy::*TFuncPtrType6)(void);
//		void Initialize					  (TRequestStatus& aStatus);
typedef void (CTestPolicy::*TFuncPtrType7)(TRequestStatus& aStatus);
//		CSsmCommandList* CommandList();
typedef CSsmCommandList* (CTestPolicy::*TFuncPtrType9)(void);

class TCall
	{
public:
	TCall(const TFuncPtrType1& aFunction, TSsmState aState);
	TCall(const TFuncPtrType2& aFunction);
	TCall(const TFuncPtrType3& aFunction, const TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity);
	TCall(const TFuncPtrType4& aFunction, TSsmState aState, TInt aReason, const TRequestStatus& aStatus);
	TCall(const TFuncPtrType5& aFunction, TSsmState aCurrentTransition, TInt aReason, TInt aError);
	TCall(const TFuncPtrType6& aFunction);
	TCall(const TFuncPtrType7& aFunction, const TRequestStatus& aStatus);
	TCall(const TFuncPtrType8& aFunction, MSsmStatePolicy::TResponse aResponse);
	TCall(const TFuncPtrType9& aFunction);
	TCall(const TFuncPtrType10& aFunction);
	union
		{
		TFuncPtrType1 iFunctionType1;
		TFuncPtrType2 iFunctionType2;
		TFuncPtrType3 iFunctionType3;
		TFuncPtrType4 iFunctionType4;
		TFuncPtrType5 iFunctionType5;
		TFuncPtrType6 iFunctionType6;
		TFuncPtrType7 iFunctionType7;
		TFuncPtrType8 iFunctionType8;
		TFuncPtrType9 iFunctionType9;
		TFuncPtrType10 iFunctionType10;
		} iFunction;
	TSsmState iState;
	TRequestStatus iReqStatus;
	TCmdErrorSeverity iSeverity;
	TInt iCleValue;
	TInt iReason;
	MSsmStatePolicy::TResponse iResponse;
	};

/**
*/
TCall::TCall(const TFuncPtrType1& aFunction, TSsmState aState)
	: iState(aState), iReqStatus(0), iCleValue(0), iReason(0), iResponse(MSsmStatePolicy::ENotAllowed)
	{
	iFunction.iFunctionType1 = aFunction;
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
*/
TCall::TCall(const TFuncPtrType2& aFunction)
	: iState(TSsmState(0,0)), iReqStatus(0), iCleValue(0), iReason(0), iResponse(MSsmStatePolicy::ENotAllowed)
	{
	iFunction.iFunctionType2 = aFunction;
	}		

/**
*/	
TCall::TCall(const TFuncPtrType3& aFunction, const TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity)
	: iState(TSsmState(0,0)), iReqStatus(aStatus), iSeverity(aSeverity), iCleValue(0), iReason(0), iResponse(MSsmStatePolicy::ENotAllowed)
	{
	iFunction.iFunctionType3 = aFunction;
	}
	
/**
*/
TCall::TCall(const TFuncPtrType4& aFunction, TSsmState aState, TInt aReason, const TRequestStatus& aStatus)
	: iState(aState), iReqStatus(aStatus), iCleValue(0), iReason(aReason), iResponse(MSsmStatePolicy::ENotAllowed)
	{
	iFunction.iFunctionType4 = aFunction;
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
*/	
TCall::TCall(const TFuncPtrType5& aFunction, TSsmState aCurrentTransition, TInt aReason, TInt aError)
	: iState(aCurrentTransition), iReqStatus(0), iCleValue(aError), iReason(aReason), iResponse(MSsmStatePolicy::ENotAllowed)
	{
	iFunction.iFunctionType5 = aFunction;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference

/**
 */
TCall::TCall(const TFuncPtrType6& aFunction)
 	: iState(TSsmState(0,0)), iReqStatus(0), iCleValue(0), iReason(0), iResponse(MSsmStatePolicy::ENotAllowed)
 	{
 	iFunction.iFunctionType6 = aFunction;
 	}		

/**
 */
TCall::TCall(const TFuncPtrType7& aFunction, const TRequestStatus& aStatus)
	: iState(TSsmState(0,0)), iReqStatus(aStatus), iCleValue(0), iReason(0), iResponse(MSsmStatePolicy::ENotAllowed)
 	{
 	iFunction.iFunctionType7 = aFunction;
 	}		

/**
*/
TCall::TCall(const TFuncPtrType8& aFunction, MSsmStatePolicy::TResponse aResponse)
	: iState(TSsmState(0,0)), iReqStatus(0), iCleValue(0), iReason(0), iResponse(aResponse)
	{
	iFunction.iFunctionType8 = aFunction;
	}

/**
*/	
TCall::TCall(const TFuncPtrType9& aFunction)
 	: iState(TSsmState(0,0)), iReqStatus(0), iCleValue(0), iReason(0), iResponse(MSsmStatePolicy::ENotAllowed)
 	{
 	iFunction.iFunctionType9 = aFunction;
 	}

/**
*/	
TCall::TCall(const TFuncPtrType10& aFunction)
  	: iState(TSsmState(0,0)), iReqStatus(0), iCleValue(0), iReason(0), iResponse(MSsmStatePolicy::ENotAllowed)
  	{
  	iFunction.iFunctionType10 = aFunction;
  	}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------- static functions to aid the test -------------------------------------------
//----------------------------------------------------------------------------------------------------------------

/**
*/
static TInt CompleteRequest(TAny* aRequestStatus)
	{
	TRequestStatus* status = static_cast<TRequestStatus*>(aRequestStatus);
	User::RequestComplete(status, KErrNone);
	return KErrNone;
	}

/**
*/
static TInt StopScheduler(TAny* /*aSsmStateTransitionEngineTest*/)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}

/**
*/
static TInt RequestTransitionToFailL(TAny* aStateTransitionEngine)
	{
	CSsmStateTransitionEngine* engine = static_cast<CSsmStateTransitionEngine*>(aStateTransitionEngine);
	
	//these statements reflect case EReplaceCurrentClearQueue in CSsmServer::RequestStateTransitionL
	const TSsmState requestStateFail(ESsmFail, KSsmAnySubState);
	TSsmStateTransition requestFail(requestStateFail, 0);
	engine->Cancel(NULL);
	engine->SubmitL(requestFail);
	
	return KErrNone;
	}

/** 
Help function to delete the unknown state policy object implementing our MSsmStatePolicy interface 
*/
static void ReleaseStatePolicy(TAny* aPolicy)
	{
	MSsmStatePolicy* policy = static_cast<MSsmStatePolicy*>(aPolicy);
	if(policy)
		{
		policy->Release();	
		}
	}

//----------------------------------------------------------------------------------------------------------------
//---------------------------------------- CTestStep implemenation---- -------------------------------------------
//----------------------------------------------------------------------------------------------------------------

/**
*/
CSsmStateTransitionEngineTest::~CSsmStateTransitionEngineTest()
	{
	delete iAsyncRequestComplete;
	delete iAsyncStopScheduler;
	delete iActiveScheduler;
	delete iPolicy;
	iCallbacks.Close();
	iPreparedCommandLists.Close();
	}

/**
*/
CSsmStateTransitionEngineTest::CSsmStateTransitionEngineTest()
	{
	SetTestStepName(KTSsmStateTransitionEngineStep);
	}

/**
*/
void CSsmStateTransitionEngineTest::AsyncRequestCompleteL(TRequestStatus* aStatus)
	{
	delete iAsyncRequestComplete;
	iAsyncRequestComplete = NULL;
	iAsyncRequestComplete = new(ELeave) CAsyncCallBack(CActive::EPriorityLow);
	TCallBack callBack(CompleteRequest, aStatus);
	iAsyncRequestComplete->Set(callBack);
	iAsyncRequestComplete->CallBack();
	}

/**
Log callbacks as function pointer, also provide the service to stop the 
current scheduler when a specific number of callbacks have been logged.
*/
void CSsmStateTransitionEngineTest::LogCall(const TCall& aCall)
	{
	const TInt err = iCallbacks.Append(aCall);
	if(KErrNone != err)
		{
		ERR_PRINTF2(_L("Failed to log call, reason: %d"), err);
		TEST(EFalse);
		}
		
	if(iCallbacks.Count() == iStopSchedulerAfterCallbackCount)
		{
		CActiveScheduler::Stop();
		}
	}
		
/**
*/
TVerdict CSsmStateTransitionEngineTest::doTestStepPreambleL()
	{		
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopScheduler, this);
	iAsyncStopScheduler->Set(stop);
	
	return CTestStep::doTestStepPreambleL();
	}

/**
*/
const TInt KBeforeInitializePolicy =  6;
const TInt KBeforePrepareCommandList =  7;
const TInt KBeforeExecuteCommandList = 8;
const TInt KBeforeGetNextState = 10;


/**
Old Test CaseID 		APPFWK-SSM-0005
New Test CaseID 		DEVSRVS-SSMA-SSM-0005
 */

TVerdict CSsmStateTransitionEngineTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmStateTransitionEngineTest started...."));
	
	__UHEAP_MARK;
	
	doTestStateTransitionEngineTestForMemoryLeaksL();
	doTestQueueBuildupL();
	doTestTransitionSimpleL();
	doTestTransitionStartupL();
	doTestTransitionStartupStartupL();
	doTestTransitionStartupFailBeforeStartL();
	doTestTransitionStartupFailL(KBeforeInitializePolicy);
	doTestTransitionStartupFailL(KBeforePrepareCommandList);
	doTestTransitionStartupFailL(KBeforeExecuteCommandList);
	doTestTransitionStartupFailL(KBeforeGetNextState);

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmStateTransitionEngineTest completed!!"));
	return TestStepResult();
	}

/**
*/
TVerdict CSsmStateTransitionEngineTest::doTestStepPostambleL()
	{	
	return CTestStep::doTestStepPostambleL();
	}

/**
*/
void CSsmStateTransitionEngineTest::doTestStateTransitionEngineTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Checking for destructor memoryleaks in CSsmStateTransitionEngine"));
	
	// CSsmStateTransitionEngine
	INFO_PRINTF1(_L("CSsmStateTransitionEngine construction under OOM"));
	CSsmStateTransitionEngine* engine=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, engine = CSsmStateTransitionEngine::NewL(this, this));
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret!=KErrNone)
	        {
	        __UHEAP_MARKEND;
	        TEST(engine==NULL);
	        }
	    else
	        {
	        TEST(engine!=NULL);
			delete engine;
	        engine = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d."),failRate-1);
	}


/**
*/
void CSsmStateTransitionEngineTest::doTestQueueBuildupL()
	{
	INFO_PRINTF1(_L("Checking Queue build up functionality"));
	
	__UHEAP_MARK;
	CSsmStateTransitionEngine* engine = CSsmStateTransitionEngine::NewLC(this, this);
	
	// first request will add to current slot
	const TInt reason1 = 1;
	const TSsmState startState(ESsmStartup, KSsmAnySubState);
	TSsmStateTransition request1(startState, reason1);
	TSsmStateTransition const* current = engine->CurrentTransition();
	TSsmStateTransition const* queued = engine->QueuedTransition();
	TEST(NULL == current);
	TEST(NULL == queued);	
	engine->SubmitL(request1);
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	TEST(NULL != current);
	TEST(NULL == queued);		
	
	// second request will add to queued slot
	const TInt reason2 = 2;
	const TSsmState normState(ESsmNormal, KSsmAnySubState);
	TSsmStateTransition request2(normState, reason2);
	engine->SubmitL(request2);
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	TEST(NULL != current);	
	TEST(NULL != queued);	
	if(current)
		{
		TEST(current->State() == startState);
		TEST(current->Reason()== reason1);
		}
	if(queued)
		{
		TEST(queued->State()  == normState);
		TEST(queued->Reason() == reason2);
		}

	// third request will overwrite previsously queued request
	const TInt reason3 = 3;
	const TSsmState shutState(ESsmShutdown, KSsmAnySubState);
	TSsmStateTransition request3(shutState, reason3);
	engine->SubmitL(request3);
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	TEST(NULL != current);	
	TEST(NULL != queued);
	if(current)
		{
		TEST(current->State() == startState);
		TEST(current->Reason()== reason1);
		}
	if(queued)
		{
		TEST(queued->State()  == shutState);
		TEST(queued->Reason() == reason3);
		}
	
	CleanupStack::PopAndDestroy(engine);
	__UHEAP_MARKEND;
	}

/**
Takes CSsmStateTransitionEngine through the last sub-state transition in a TechView startup
*/
void CSsmStateTransitionEngineTest::doTestTransitionSimpleL()
	{
	INFO_PRINTF1(_L("Checking requesthandler with a single request"));
		
	__UHEAP_MARK;
	MSsmStatePolicy* policyImpl = CTestPolicy::NewL(*this);
	CleanupStack::PushL(TCleanupItem(ReleaseStatePolicy, policyImpl)); 
	iPolicy = new (ELeave) CSsmStatePolicyFrame(policyImpl);
	CleanupStack::Pop(policyImpl);
	
	CSsmStateTransitionEngine* engine = CSsmStateTransitionEngine::NewLC(this, this);
	engine->PerformCommandListValidation(EFalse);
		
	//Create a transition request for the last sub state startup step
	const TInt reason = 1;
	const TSsmState requestState(ESsmStartup, CTestPolicy::ESubstate4);
	TSsmStateTransition request(requestState, reason);
	TSsmStateTransition const* current = engine->CurrentTransition();
	TSsmStateTransition const* queued = engine->QueuedTransition();
	RMessagePtr2 dummy;
	const MSsmStatePolicy::TResponse response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TESTE(MSsmStatePolicy::EDefinitelyAllowed == response, response);
	engine->SubmitL(request);
		
	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack();
	//run engine
	iActiveScheduler->Start();
	
	//Test that the queue ha been emptied
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	TEST(NULL == current);
	TEST(NULL == queued);	

	delete iPolicy;
	iPolicy = NULL;
	
	//Verify that the expected calls were made 
	AssertTransitionSimple(); 
	
	//Cleanup test
	CleanupStack::PopAndDestroy(engine);

	iCallbacks.Reset();
	iPreparedCommandLists.Reset();

	delete iAsyncRequestComplete;
	iAsyncRequestComplete = NULL;

	__UHEAP_MARKEND;
	}

/**
*/
void CSsmStateTransitionEngineTest::AssertTransitionSimple()
	{
	INFO_PRINTF1(_L("    1-- running AssertTransitionSimple"));
	const TInt count = iCallbacks.Count();
	if(count >= 9)
		{
		const TArray<TInt> preparedCmdLists = PreparedCommandLists();
		TEST(1 == preparedCmdLists.Count());
		TInt i = 0;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);

		TEST(iCallbacks[++i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EDefinitelyAllowed);

		TEST(iCallbacks[++i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));

		TEST(iCallbacks[++i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);

		TEST(iCallbacks[++i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);

		TEST(iCallbacks[++i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));
		TEST(iCallbacks[i].iReason == 1);

		TEST(iCallbacks[++i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);

		TEST(iCallbacks[++i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);

		TEST(iCallbacks[++i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);

		}	
	TESTE(9 == count, count);
	}

/**
Takes CSsmStateTransitionEngine through a complete TechView startup
*/
void CSsmStateTransitionEngineTest::doTestTransitionStartupL()
	{
	INFO_PRINTF1(_L("Checking requesthandler with a typical Techview startup request."));
		
	__UHEAP_MARK;
	MSsmStatePolicy* policyImpl = CTestPolicy::NewL(*this);
	CleanupStack::PushL(TCleanupItem(ReleaseStatePolicy, policyImpl)); 
	iPolicy = new (ELeave) CSsmStatePolicyFrame(policyImpl);
	CleanupStack::Pop(policyImpl);

	CSsmStateTransitionEngine* engine = CSsmStateTransitionEngine::NewLC(this, this);
	engine->PerformCommandListValidation(EFalse);
		
	//Create a transition request that will go through all startup sub states
	const TInt reason = 1;
	const TSsmState requestState(ESsmStartup, KSsmAnySubState);
	TSsmStateTransition request(requestState, reason);
	TSsmStateTransition const* current = engine->CurrentTransition();
	TSsmStateTransition const* queued = engine->QueuedTransition();
	RMessagePtr2 dummy;
	const MSsmStatePolicy::TResponse response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TEST(MSsmStatePolicy::EDefinitelyAllowed == response);
	engine->SubmitL(request);
		
	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack();
	//run engine
	iActiveScheduler->Start();
	
	//Test that the queue ha been emptied
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	TEST(NULL == current);
	TEST(NULL == queued);	
	
	delete iPolicy;
	iPolicy = NULL;
	
	//Verify that the expected calls were made 
	AssertTransitionStartup(); 
	
	//Cleanup test
	CleanupStack::PopAndDestroy(engine);

	iCallbacks.Reset();
	iPreparedCommandLists.Reset();
	
	delete iAsyncRequestComplete;
	iAsyncRequestComplete = NULL;

	__UHEAP_MARKEND;
	}
	
/**
*/
void CSsmStateTransitionEngineTest::AssertTransitionStartup()
	{
	INFO_PRINTF1(_L("    2---- running AssertStateMachineStartup"));
	const TInt count = iCallbacks.Count();
	if(count >= 30)
		{
		const TArray<TInt> preparedCmdLists = PreparedCommandLists();
		TEST(4 == preparedCmdLists.Count());
		TEST(preparedCmdLists[0] == CTestPolicy::ESubstate1);
		TEST(preparedCmdLists[1] == CTestPolicy::ESubstate2);
		TEST(preparedCmdLists[2] == CTestPolicy::ESubstate3);
		TEST(preparedCmdLists[3] == CTestPolicy::ESubstate4);
		
		TInt i = 0;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EDefinitelyAllowed);
		i++;
		//First sub state transition	
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;		
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//Second sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate2));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;			
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate2));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;		
		//Third sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate3));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;				
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate3));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//Fourth sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;			
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		}	
		
	TESTE(30 == count, count);
	}

/**
Queue two TechView startup transitions, try to queue a third (should be rejected).
When the first is complete, queue a third (should be accepted).
Let CSsmStateTransitionEngine complete all three requests.
*/
void CSsmStateTransitionEngineTest::doTestTransitionStartupStartupL()
	{
	INFO_PRINTF1(_L("Checking requesthandler's handling of queued transitions."));
		
	__UHEAP_MARK;
	MSsmStatePolicy* policyImpl = CTestPolicy::NewL(*this);
	CleanupStack::PushL(TCleanupItem(ReleaseStatePolicy, policyImpl)); 
	iPolicy = new (ELeave) CSsmStatePolicyFrame(policyImpl);
	CleanupStack::Pop(policyImpl);

	CSsmStateTransitionEngine* engine = CSsmStateTransitionEngine::NewLC(this, this);
	engine->PerformCommandListValidation(EFalse);
		
	//Create a transition request that will go through all startup sub states
	const TInt reason = 1;
	const TSsmState requestState(ESsmStartup, KSsmAnySubState);
	TSsmStateTransition request(requestState, reason);
	TSsmStateTransition const* current = engine->CurrentTransition();
	TSsmStateTransition const* queued = engine->QueuedTransition();
	RMessagePtr2 dummy;
	MSsmStatePolicy::TResponse response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TEST(MSsmStatePolicy::EDefinitelyAllowed == response);
	engine->SubmitL(request);
	
	//Queue a second transition (our testpolicy will accept this)
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TEST(MSsmStatePolicy::ECurrentRemainReplaceQueued == response);
	engine->SubmitL(request);
	
	//Try to queue a third request (the queue should now be full)
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TESTE(MSsmStatePolicy::ENotAllowed == response, response);
	
	//run engine, until first transition has completed
	iStopSchedulerAfterCallbackCount = 32;
	iActiveScheduler->Start();
	
	//Now there should be a free slot in the queue
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TESTE(MSsmStatePolicy::ECurrentRemainReplaceQueued == response, response);
	engine->SubmitL(request);
	
	//restart the scheduler, run until completion
	iStopSchedulerAfterCallbackCount = 0;
	iAsyncStopScheduler->CallBack();
	iActiveScheduler->Start();
	
	//Test that the queue has been emptied
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	TEST(NULL == current);
	TEST(NULL == queued);	
	
	delete iPolicy;
	iPolicy = NULL;
	
	//Verify that the expected calls were made 
	AssertTransitionStartupStartup(); 
	
	//Cleanup test
	CleanupStack::PopAndDestroy(engine);

	iCallbacks.Reset();
	iPreparedCommandLists.Reset();
	;
	delete iAsyncRequestComplete;
	iAsyncRequestComplete = NULL;

	__UHEAP_MARKEND;
	}

/**
*/	
void CSsmStateTransitionEngineTest::AssertTransitionStartupStartup()
	{
	INFO_PRINTF1(_L("    3------ running AssertTransitionStartupStartup"));
	const TInt count = iCallbacks.Count();
	if(count >= 62)
		{
		const TArray<TInt> preparedCmdLists = PreparedCommandLists();
		TEST(12 == preparedCmdLists.Count());
		TEST(preparedCmdLists[0] == CTestPolicy::ESubstate1);
		TEST(preparedCmdLists[1] == CTestPolicy::ESubstate2);
		TEST(preparedCmdLists[2] == CTestPolicy::ESubstate3);
		TEST(preparedCmdLists[3] == CTestPolicy::ESubstate4);
		TEST(preparedCmdLists[4] == CTestPolicy::ESubstate1);
		TEST(preparedCmdLists[5] == CTestPolicy::ESubstate2);
		TEST(preparedCmdLists[6] == CTestPolicy::ESubstate3);
		TEST(preparedCmdLists[3] == CTestPolicy::ESubstate4);
		TEST(preparedCmdLists[8] == CTestPolicy::ESubstate1);
		TEST(preparedCmdLists[9] == CTestPolicy::ESubstate2);
		TEST(preparedCmdLists[10] == CTestPolicy::ESubstate3);
		TEST(preparedCmdLists[11] == CTestPolicy::ESubstate4);
		
		TInt i = 0;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EDefinitelyAllowed);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ECurrentRemainReplaceQueued);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ENotAllowed);
		i++;
		//  ***First major transition***
		//First sub state transition	
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;	
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//Second sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate2));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;		
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate2));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;		
		//Third sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate3));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;		
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate3));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//Fourth sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;				
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//  ***New request submitted***
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ECurrentRemainReplaceQueued);
		i++;
		//  ***Second major transition***
		//First sub state transition	
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;		
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//Second sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate2));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;		
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate2));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;		
		//Third sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate3));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;		
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate3));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//Fourth sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;			
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//  ***Third major transition***

		//First sub state transition	
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;			
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//Second sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate2));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;			
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate2));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;		
		//Third sub state transition
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate3));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;			
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate3));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		i++;
		//Fourth sub state transition		
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;			
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, CTestPolicy::ESubstate4));
		TEST(iCallbacks[i].iReason == 1);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		}	
		
	TESTE(89 == count, count);	
	}

/**
*/
void CSsmStateTransitionEngineTest::doTestTransitionStartupFailBeforeStartL()
	{
	INFO_PRINTF1(_L("Checking requesthandler's handling of cancelling current before start and going to fail."));
		
	__UHEAP_MARK;
	MSsmStatePolicy* policyImpl = CTestPolicy::NewL(*this);
	CleanupStack::PushL(TCleanupItem(ReleaseStatePolicy, policyImpl)); 
	iPolicy = new (ELeave) CSsmStatePolicyFrame(policyImpl);
	CleanupStack::Pop(policyImpl);

	CSsmStateTransitionEngine* engine = CSsmStateTransitionEngine::NewLC(this, this);
	engine->PerformCommandListValidation(EFalse);
		
	//Create a transition request that will go through all startup sub states
	const TInt reason = 1;
	const TSsmState requestState(ESsmStartup, KSsmAnySubState);
	TSsmStateTransition request(requestState, reason);
	TSsmStateTransition const* current = engine->CurrentTransition();
	TSsmStateTransition const* queued = engine->QueuedTransition();
	RMessagePtr2 dummy;
	MSsmStatePolicy::TResponse response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TEST(MSsmStatePolicy::EDefinitelyAllowed == response);
	engine->SubmitL(request);
	
	//Queue a second transition (our testpolicy will accept this)
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TEST(MSsmStatePolicy::ECurrentRemainReplaceQueued == response);
	engine->SubmitL(request);
	
	//A request to fail should be accepted, anything ongoing need to be cancelled
	const TSsmState requestStateFail(ESsmFail, KSsmAnySubState);
	TSsmStateTransition requestFail(requestStateFail, reason);
	response = iPolicy->CallTransitionAllowed(requestFail, current, queued, dummy);
	TEST(MSsmStatePolicy::EReplaceCurrentClearQueue == response);
	
	//Kick off a request to state-fail (will occur as soon as we start the scheduler)
	CAsyncCallBack* async = new(ELeave) CAsyncCallBack(CActive::EPriorityHigh);
	CleanupStack::PushL(async);
	TCallBack fail(RequestTransitionToFailL, engine);
	async->Set(fail);
	async->CallBack();
	
	//start the scheduler, run until completion
	iStopSchedulerAfterCallbackCount = 0;
	iAsyncStopScheduler->CallBack();
	iActiveScheduler->Start();
	
	//Test that the queue has been emptied
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	TEST(NULL == current);
	TEST(NULL == queued);	
	
	delete iPolicy;
	iPolicy = NULL;
	
	//Verify that the expected calls were made 
	AssertTransitionStartupFailBeforeStart();
	
	//Cleanup test
	CleanupStack::PopAndDestroy(async);
	CleanupStack::PopAndDestroy(engine);

	iCallbacks.Reset();
	iPreparedCommandLists.Reset();
	
	delete iAsyncRequestComplete;
	iAsyncRequestComplete = NULL;

	__UHEAP_MARKEND;
	}

/**
*/	
void CSsmStateTransitionEngineTest::AssertTransitionStartupFailBeforeStart()
	{
	
	INFO_PRINTF1(_L("   4-------- running AssertTransitionStartupFailBeforeStart"));
	const TInt count = iCallbacks.Count();
	if(count >= 11)
		{
		const TArray<TInt> preparedCmdLists = PreparedCommandLists();
		const TInt cmdListCount = preparedCmdLists.Count();
		TESTE(1 == cmdListCount, cmdListCount);
		TEST(preparedCmdLists[0] == KFailStateCommandList1Id);
		
		TInt i = 0;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EDefinitelyAllowed);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ECurrentRemainReplaceQueued);
		i++;
		//  *** 2 new requests submitted, the first rejected the other will cancel current job ***
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EReplaceCurrentClearQueue);
		i++;
		
		//  ***Start transition to fail***
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 0);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;			
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 0);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		}	
		
	TESTE(11 == count, count);	
	}

/**
Queue a TechView startup, let it start. Before the transition is complete, 
request a transition to Fail.
@param aFailWhen Decides when the request to fail should arrive, valid options are 
				 KBeforePrepareCommandList, KBeforeExecuteCommandList, KBeforeGetNextState.
*/
void CSsmStateTransitionEngineTest::doTestTransitionStartupFailL(TInt aFailWhen)
	{
	INFO_PRINTF1(_L("Checking requesthandler's handling of cancelling current and going to fail."));
		
	__UHEAP_MARK;
	MSsmStatePolicy* policyImpl = CTestPolicy::NewL(*this);
	CleanupStack::PushL(TCleanupItem(ReleaseStatePolicy, policyImpl)); 
	iPolicy = new (ELeave) CSsmStatePolicyFrame(policyImpl);
	CleanupStack::Pop(policyImpl);

	CSsmStateTransitionEngine* engine = CSsmStateTransitionEngine::NewLC(this, this);
	engine->PerformCommandListValidation(EFalse);
		
	//Create a transition request that will go through all startup sub states
	const TInt reason = 1;
	const TSsmState requestState(ESsmStartup, KSsmAnySubState);
	TSsmStateTransition request(requestState, reason);
	TSsmStateTransition const* current = engine->CurrentTransition();
	TSsmStateTransition const* queued = engine->QueuedTransition();
	RMessagePtr2 dummy;
	MSsmStatePolicy::TResponse response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TEST(MSsmStatePolicy::EDefinitelyAllowed == response);
	engine->SubmitL(request);
	
	//Queue a second transition (our testpolicy will accept this)
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TEST(MSsmStatePolicy::ECurrentRemainReplaceQueued == response);
	engine->SubmitL(request);
	
	//Try to queue a third request (the queue should now be full)
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TEST(MSsmStatePolicy::ENotAllowed == response);
	
	//run engine, stop when aFailWhen callbacks have been logged
	iStopSchedulerAfterCallbackCount = aFailWhen;
	iActiveScheduler->Start();
	
	//The queue should still be full
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	TEST(NULL != current);
	TEST(NULL != queued);
	//A normal request should still be rejected
	response = iPolicy->CallTransitionAllowed(request, current, queued, dummy);
	TEST(MSsmStatePolicy::ENotAllowed == response);
	//A request to fail should be accepted, anything ongoing need to be cancelled
	const TSsmState requestStateFail(ESsmFail, KSsmAnySubState);
	TSsmStateTransition requestFail(requestStateFail, reason);
	response = iPolicy->CallTransitionAllowed(requestFail, current, queued, dummy);
	TEST(MSsmStatePolicy::EReplaceCurrentClearQueue == response);
	
	//Kick off a request to state-fail (will occur as soon as we start the scheduler again)
	CAsyncCallBack* async = new(ELeave) CAsyncCallBack(CActive::EPriorityHigh);
	CleanupStack::PushL(async);
	TCallBack fail(RequestTransitionToFailL, engine);
	async->Set(fail);
	async->CallBack();
	
	//restart the scheduler, run until completion
	iStopSchedulerAfterCallbackCount = 0;
	iAsyncStopScheduler->CallBack();
	iActiveScheduler->Start();
	
	//Test that the queue has been emptied
	current = engine->CurrentTransition();
	queued = engine->QueuedTransition();
	TEST(NULL == current);
	TEST(NULL == queued);	
	
	delete iPolicy;
	iPolicy = NULL;
	
	//Verify that the expected calls were made 
	switch(aFailWhen)
		{
		case KBeforeInitializePolicy:
			AssertTransitionStartupFailBeforeInitializePolicy();
			break;
		case KBeforePrepareCommandList:
			AssertTransitionStartupFailBeforePrepareCommandList();
			break;
		case KBeforeExecuteCommandList:
			AssertTransitionStartupFailBeforeExecuteCommandList();
			break;
		case KBeforeGetNextState:
			AssertTransitionStartupFailBeforeGetNextState(); 
			break;
		default:
			TEST(EFalse);
			break;
		}
	
	//Cleanup test
	iStatetransitionEngineTRequestStatus = NULL;
	CleanupStack::PopAndDestroy(async);
	CleanupStack::PopAndDestroy(engine);

	iCallbacks.Reset();
	iPreparedCommandLists.Reset();
	
	delete iAsyncRequestComplete;
	iAsyncRequestComplete = NULL;

	__UHEAP_MARKEND;
	}

/**
 */	
 void CSsmStateTransitionEngineTest::AssertTransitionStartupFailBeforeInitializePolicy()
 	{
 	
 	INFO_PRINTF1(_L("   5---------- running AssertTransitionStartupFailBeforeInitializePolicy"));
 	const TInt count = iCallbacks.Count();
 	if(count >= 15)
 		{
 		const TArray<TInt> preparedCmdLists = PreparedCommandLists();
 		TEST(1 == preparedCmdLists.Count());
		TEST(preparedCmdLists[0] == KFailStateCommandList1Id);
		
 		TInt i = 0;
 		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
 		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EDefinitelyAllowed);
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
 		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ECurrentRemainReplaceQueued);
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
 		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ENotAllowed);
 		i++;
 		//  ***First major transition***

 		//First sub state transition	
 		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
 		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
 		i++;		

 		//  *** 2 new requests submitted, the first rejected the other will cancel current job ***
 		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
 		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ENotAllowed);
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
 		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EReplaceCurrentClearQueue);
 		i++;
		
 		//  ***Start transition to fail***
 		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
 		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
 		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
 		TEST(iCallbacks[i].iReason == 0);
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
 		i++;	
 		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
 		i++;
 		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
 		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
 		TEST(iCallbacks[i].iReason == 0);
 		TEST(iCallbacks[i].iCleValue == KErrNone);
 		}	
 		
 	TESTE(15 == count, count);	
 	}

/**
*/	
void CSsmStateTransitionEngineTest::AssertTransitionStartupFailBeforePrepareCommandList()
	{
	
	INFO_PRINTF1(_L("   6---------- running AssertTransitionStartupFailBeforePrepareCommandList"));
	const TInt count = iCallbacks.Count();
	if(count >= 17)
		{
		const TArray<TInt> preparedCmdLists = PreparedCommandLists();
		TEST(1 == preparedCmdLists.Count());
		TEST(preparedCmdLists[0] == KFailStateCommandList1Id);
		
		TInt i = 0;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EDefinitelyAllowed);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ECurrentRemainReplaceQueued);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ENotAllowed);
		i++;
		//  ***First major transition***

		//First sub state transition	
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
	
		//  *** 2 new requests submitted, the first rejected the other will cancel current job ***
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ENotAllowed);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EReplaceCurrentClearQueue);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::InitializeCancel);
		i++;
		//  ***Start transition to fail***
	
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 0);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;	
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 0);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		}	
		
	TESTE(17 == count, count);	
	}

/**
*/	
void CSsmStateTransitionEngineTest::AssertTransitionStartupFailBeforeExecuteCommandList()
	{
	
	INFO_PRINTF1(_L("   7------------ running AssertTransitionStartupFailBeforeExecuteCommandList"));
	const TInt count = iCallbacks.Count();
	if(count >= 18)
		{
		const TArray<TInt> preparedCmdLists = PreparedCommandLists();
		TEST(2 == preparedCmdLists.Count());
		TEST(preparedCmdLists[1] == KFailStateCommandList1Id);
		
		TInt i = 0;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EDefinitelyAllowed);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ECurrentRemainReplaceQueued);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ENotAllowed);
		i++;
		//  ***First major transition***
		//First sub state transition	
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		i++;
	
		//  *** 2 new requests submitted, the first rejected the other will cancel current job ***
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ENotAllowed);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EReplaceCurrentClearQueue);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::PrepareCommandListCancel);
		i++;
		
		//  ***Start transition to fail***
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 0);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 0);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		}	
		
	TESTE(18== count, count);	
	}

/**
*/	
void CSsmStateTransitionEngineTest::AssertTransitionStartupFailBeforeGetNextState()
	{
	
	INFO_PRINTF1(_L("   8-------------- running AssertTransitionStartupFailBeforeGetNextState"));
	const TInt count = iCallbacks.Count();
	if(count >= 20)
		{
		const TArray<TInt> preparedCmdLists = PreparedCommandLists();
		TEST(2 == preparedCmdLists.Count());
		TEST(preparedCmdLists[0] == CTestPolicy::ESubstate1);
		TEST(preparedCmdLists[1] == KFailStateCommandList1Id);
		
		TInt i = 0;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EDefinitelyAllowed);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ECurrentRemainReplaceQueued);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ENotAllowed);
		i++;
		//  ***First major transition***
		//First sub state transition	
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmStartup, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 1);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;	
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;

		//  *** 2 new requests submitted, the first rejected the other will cancel current job ***
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::ENotAllowed);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType8 == &CTestPolicy::TransitionAllowed);
		TEST(iCallbacks[i].iResponse == MSsmStatePolicy::EReplaceCurrentClearQueue);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType10 == &CSsmStateTransitionEngineTest::ExecuteCommandListCancel);
		i++;
		
		//  ***Start transition to fail***
		TEST(iCallbacks[i].iFunction.iFunctionType1 == &CSsmStateTransitionEngineTest::GetStatePolicyL);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType6 == &CTestPolicy::NewLL);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType7 == &CTestPolicy::Initialize);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType4 == &CTestPolicy::PrepareCommandList);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 0);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType9 == &CTestPolicy::CommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType3 == &CSsmStateTransitionEngineTest::ExecuteCommandList);
		i++;
		TEST(iCallbacks[i].iFunction.iFunctionType5 == &CTestPolicy::GetNextState);
		TEST(iCallbacks[i].iState == TSsmState(ESsmFail, KSsmAnySubState));
		TEST(iCallbacks[i].iReason == 0);
		TEST(iCallbacks[i].iCleValue == KErrNone);
		}	
		
	TESTE(20 == count, count);	
	}

/**
*/	
TArray<TInt> CSsmStateTransitionEngineTest::PreparedCommandLists() const
 	{
 	return iPreparedCommandLists.Array();
 	}

void CSsmStateTransitionEngineTest::AppendPreparedCommandList(TInt aId)
 	{
 	TRAPD(err, iPreparedCommandLists.AppendL(aId));
 	TESTE(err==KErrNone, err);
 	}


//----------------- MPolicyResolverProxy implemenation --------------------

/**
*/
void CSsmStateTransitionEngineTest::GetStatePolicyL(TSsmState aState)
	{
	TTime now;
	now.HomeTime();
	TCall call(&CSsmStateTransitionEngineTest::GetStatePolicyL, aState);
	LogCall(call);
	
	MSsmStatePolicy* policyImpl = CTestPolicy::NewL(*this);
	CleanupStack::PushL(TCleanupItem(ReleaseStatePolicy, policyImpl)); 
	CSsmStatePolicyFrame* policy = new (ELeave) CSsmStatePolicyFrame(policyImpl);
	CleanupStack::Pop(policyImpl);
	
	if(iPolicy)
		{
		delete iPolicy;
		iPolicy = NULL;
		}
	iPolicy = policy;
	iPolicy->SetStatePolicyId(aState.MainState());
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
*/
CSsmStatePolicyFrame* CSsmStateTransitionEngineTest::Policy()
 	{	
 	return iPolicy;
 	}

/**
*/
void CSsmStateTransitionEngineTest::ReleasePolicyResolver() 
  	{
  	}

//--------------------- MCleSessionProxy implemenation -----------------------
/**
*/
void CSsmStateTransitionEngineTest::ExecuteCommandList(const CSsmCommandList& /*aCmdList*/, TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity)
	{
	TTime now;
	now.HomeTime();
	TCall call(&CSsmStateTransitionEngineTest::ExecuteCommandList, aStatus, aSeverity);
	LogCall(call);
	
	aStatus = KRequestPending;
	iStatetransitionEngineTRequestStatus = &aStatus;
	TRAPD(err, AsyncRequestCompleteL(iStatetransitionEngineTRequestStatus));
	TESTE(err==KErrNone, err);
	}

void CSsmStateTransitionEngineTest::ExecuteCommandListCancel()
	{
	TTime now;
	now.HomeTime();
	TCall call(&CSsmStateTransitionEngineTest::ExecuteCommandListCancel);
	LogCall(call);
	
	//Cancel the callback to complete iStatetransitionEngineTRequestStatus with KErrNone
	ASSERT(iAsyncRequestComplete);
	iAsyncRequestComplete->Cancel();
	
	//Then complete iStatetransitionEngineTRequestStatus with KErrCancel
	ASSERT(iStatetransitionEngineTRequestStatus);
	User::RequestComplete(iStatetransitionEngineTRequestStatus, KErrCancel);
	iStatetransitionEngineTRequestStatus = NULL;
	}

/** Function needed when using real SysCle but not needed for this test */
void CSsmStateTransitionEngineTest::ConnectL() 
	{
	}
void CSsmStateTransitionEngineTest::ReleaseCle() 
	{
	}
void CSsmStateTransitionEngineTest::Close()
	{
	}
TBool CSsmStateTransitionEngineTest::IsConnected() 
	{
	return EFalse;
	}

MCleSessionProxy* CSsmStateTransitionEngineTest::CreateNewProxyL()
	{
	return this;
	}

//----------------------------------------------------------------------------------------------------------------
//---------------------------------------- CTestPolicy implemenation ---------------------------------------------
//----------------------------------------------------------------------------------------------------------------

/**
*/
MSsmStatePolicy* CTestPolicy::NewL(CSsmStateTransitionEngineTest& aTestStep)
	{		
	CTestPolicy* self = new (ELeave) CTestPolicy(aTestStep);
	self->NewLL();
	return self;
	}

void CTestPolicy::NewLL() 
	{
	//Log call	
	TTime now;
	now.HomeTime();
	TCall call(&CTestPolicy::NewLL);
	iTestStep.LogCall(call);	
	}

/**
*/	
CTestPolicy::CTestPolicy(CSsmStateTransitionEngineTest& aTestStep)
	: iTestStep(aTestStep)
	{
	}

/**
*/
CTestPolicy::~CTestPolicy()
	{
	delete iCommandList;
	}

void CTestPolicy::Initialize(TRequestStatus& aStatus)
	{
	//Log call	
	TTime now;
	now.HomeTime();
	TCall call(&CTestPolicy::Initialize, aStatus);
	iTestStep.LogCall(call);	
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

/**
 */
 void CTestPolicy::InitializeCancel()
 	{
 	//Log call	
 	TTime now;
 	now.HomeTime();
 	TCall call(&CTestPolicy::InitializeCancel);
 	iTestStep.LogCall(call);	

  	delete iCommandList;
  	iCommandList = NULL;
  	}

void CTestPolicy::Release()
	{
	delete this;	
	}

/**
This function is valid/possible to call at any time.
*/	
MSsmStatePolicy::TResponse CTestPolicy::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& /*aMessage*/)
	{
	TResponse response = ENotAllowed;
	
	//Check if the requested transition is supported from current state
	TSsmState state = aRequest.State();
	if((state.MainState() == ESsmStartup || state.MainState() == ESsmFail))
		{
		if((NULL == aCurrent) && (NULL == aQueued))
			{
			// SsmServer is idle
			response = EDefinitelyAllowed;
			}
		else if(aRequest.State().MainState() == ESsmFail)
			{
			// Going into failed state will override anything currently ongoing or queued
			response = EReplaceCurrentClearQueue;
			}
		else if((NULL == aQueued) || (NULL == aCurrent))
			{
			// SsmServer has a free slot in the queue (or will have when
			// the queued request gets moved into the current slot)
			response = ECurrentRemainReplaceQueued;
			}
		}	
	
	//Log call	
	TTime now;
	now.HomeTime();
	TCall call(&CTestPolicy::TransitionAllowed, response);
	iTestStep.LogCall(call);		
		
	return response;
	}

/**
*/	
void CTestPolicy::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	//Log call
	TTime now;
	now.HomeTime();
	TCall call(&CTestPolicy::PrepareCommandList, aState, aReason, aStatus);
	iTestStep.LogCall(call);
	
	TInt commandListId = KErrNotFound;
	const TUint16 substate = aState.SubState();
	if(ESsmStartup == aState.MainState())
		{
		//Let's start from the beginning if no specific sub state is selected
		commandListId = (KSsmAnySubState==substate) ? ESubstate1 : substate;
		}
	else if(ESsmFail == aState.MainState())
		{
		commandListId = KFailStateCommandList1Id;
		}
	else
		{
		ASSERT(0);
		}

	TRAPD(err, iCommandList = CSsmCommandList::NewL());
	iTestStep.AppendPreparedCommandList(commandListId);
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
*/
void CTestPolicy::PrepareCommandListCancel()
	{
	//Log call	
	TTime now;
	now.HomeTime();
	TCall call(&CTestPolicy::PrepareCommandListCancel);
	iTestStep.LogCall(call);	

 	delete iCommandList;
 	iCommandList = NULL;
 	}

/**
*/
CSsmCommandList* CTestPolicy::CommandList()
	{
	//Log call	
	TTime now;
	now.HomeTime();
	
	TCall call(&CTestPolicy::CommandList);
	iTestStep.LogCall(call);
	
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

/**
*/
TBool CTestPolicy::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	(void)aSeverity;

	TTime now;
	now.HomeTime();
	TCall call(&CTestPolicy::GetNextState, aCurrentTransition, aReason, aError);
	iTestStep.LogCall(call);	
	
	TBool moreToCome = EFalse;
	
	if(KErrNone == aError)	
		{
		//No problems occurred during previous command list execution
		if(aCurrentTransition.MainState() == ESsmFail) 
			{
			aNextState = TSsmState(0, 0);
			moreToCome = EFalse;
			}
		else
			{
			switch(aCurrentTransition.SubState())
				{
				case KSsmAnySubState: //ESubstate1
					{
					aNextState = TSsmState(ESsmStartup, ESubstate2);
					moreToCome = ETrue;
					break;
					}
				case ESubstate2:
					{
					aNextState = TSsmState(ESsmStartup, ESubstate3);
					moreToCome = ETrue;
					break;
					}
				case ESubstate3:
					{
					aNextState = TSsmState(ESsmStartup, ESubstate4);
					moreToCome = ETrue;
					break;
					}
				case ESubstate4:
					{
					moreToCome = EFalse;
					break;
					}
				default:
					{
					ASSERT(0);
					}
				}	
			}
		}
	else
		{
		//Do some alternative job due to previous error
		ASSERT(0);
		}
	
	return moreToCome;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference
