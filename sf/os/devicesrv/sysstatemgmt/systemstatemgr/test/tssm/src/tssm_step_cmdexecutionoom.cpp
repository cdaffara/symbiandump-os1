// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test the command execution under OOM conditions
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <ssm/ssmcommandlist.h>
#include <e32property.h>
#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/conditionevaluate.h>
#include "ssmstatepolicyframe.h"
#include "ssmstatetransitionengine.h"
#include "tssm_step_cmdexecutionoom.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "ssmdebug.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
  
_LIT(KTestRscFileName, "z:\\resource\\ssmatest\\cmdexecutionoomtest\\");		
_LIT(KSsmCleTestServerName, "TestSsmCleServer");
const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)
const TUint KPropertyKey = 0x1028359B;        
/*
call back function to complete the request.
*/
static TInt CompleteRequest(TAny* aRequestStatus)
	{
	TRequestStatus* status = static_cast<TRequestStatus*>(aRequestStatus);
	User::RequestComplete(status, KErrNone);
	return KErrNone;
	}

static TInt StopScheduler(TAny* /*aSsmStateTransitionEngineTest*/)
	{
	CActiveScheduler::Stop();
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

//CTestStep implemenation

CSsmCmdExecutionOomTest::~CSsmCmdExecutionOomTest()
	{
	delete iAsyncRequestComplete;
	delete iAsyncStopScheduler;
	delete iActiveScheduler;
	delete iPolicy;
	iPreparedCommandLists.Close();
	iTestCleSession.Close();
	}

CSsmCmdExecutionOomTest::CSsmCmdExecutionOomTest()
	{
	SetTestStepName(KTSsmCmdExecutionOomStep);
	if(iConnected)
		iTestCleSession.Close();
	}

void CSsmCmdExecutionOomTest::AsyncRequestCompleteL(TRequestStatus* aStatus)
	{
	delete iAsyncRequestComplete;
	iAsyncRequestComplete = NULL;
	iAsyncRequestComplete = new(ELeave) CAsyncCallBack(CActive::EPriorityLow);
	TCallBack callBack(CompleteRequest, aStatus);
	iAsyncRequestComplete->Set(callBack);
	iAsyncRequestComplete->CallBack();
	}	

TVerdict CSsmCmdExecutionOomTest::doTestStepPreambleL()
	{
	//KTestRscFileName resource file has a conditional information
	TInt err = RProperty::Define(KMySID, KPropertyKey, RProperty::EInt, EFalse);
	if(err != KErrNone && err != KErrAlreadyExists)
		{	
		INFO_PRINTF3(_L("Error when defining property 0x%x: %d"), KPropertyKey, err);
		User::Leave(err);
		}
	User::LeaveIfError(RProperty::Set(KMySID, KPropertyKey, 1));
	
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopScheduler, this);
	iAsyncStopScheduler->Set(stop);
	
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmCmdExecutionOomTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmCmdExecutionOomTest started...."));
	
	__UHEAP_MARK;
	doTestCleOOML();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmCmdExecutionOomTest completed!!"));
	return TestStepResult();
	}

TVerdict CSsmCmdExecutionOomTest::doTestStepPostambleL()
	{
	// delete the key
	TInt err = RProperty::Delete(KMySID, KPropertyKey);
	TEST(err == KErrNone);
	return CTestStep::doTestStepPostambleL();
	}

/**
Test case will perform following steps
	step1:Construct CSsmStateTransitionEngine
	step2:Create a transition request 
	step3:Prepare command list using resource file
	step4:Submit the transition to engine
	step5:Execute the command list under OOM conditions
*/
void CSsmCmdExecutionOomTest::doTestCleOOML()
	{
	INFO_PRINTF1(_L("doTestCleOOML."));
	__UHEAP_MARK;
	MSsmStatePolicy* policyImpl = CTestStatePolicy::NewL(*this);
	CleanupStack::PushL(TCleanupItem(ReleaseStatePolicy, policyImpl)); 
	iPolicy = new (ELeave) CSsmStatePolicyFrame(policyImpl);
	CleanupStack::Pop(policyImpl);

	CSsmStateTransitionEngine* engine = CSsmStateTransitionEngine::NewLC(this, this);
	engine->PerformCommandListValidation(EFalse);
		
	//Create a transition request 
	const TInt reason = 1;
	const TSsmState requestState(ESsmStartup, KSsmAnySubState);
	TSsmStateTransition request(requestState, reason);

	engine->SubmitL(request);
	
	//run engine, stop when there are no further transition
	iAsyncStopScheduler->CallBack();
	iActiveScheduler->Start();
	
	//Test that the queue has been emptied
	TSsmStateTransition const* current = engine->CurrentTransition();
	TEST(NULL == current);
	
	delete iPolicy;
	iPolicy = NULL;
	
	iTestCleSession.Close();
	CleanupStack::PopAndDestroy(engine);
	
	delete iAsyncRequestComplete;
	iAsyncRequestComplete = NULL;
	__UHEAP_MARKEND;
	}
	
TArray<TInt> CSsmCmdExecutionOomTest::PreparedCommandLists() const
 	{
 	return iPreparedCommandLists.Array();
 	}

void CSsmCmdExecutionOomTest::AppendPreparedCommandList(TInt aId)
 	{
 	TRAPD(err, iPreparedCommandLists.AppendL(aId));
 	TESTE(err==KErrNone, err);
 	}

//MPolicyResolverProxy implemenation 
void CSsmCmdExecutionOomTest::GetStatePolicyL(TSsmState aState)
	{
	iPolicy->SetStatePolicyId(aState.MainState());
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

CSsmStatePolicyFrame* CSsmCmdExecutionOomTest::Policy()
 	{
 	return iPolicy;
 	}

void CSsmCmdExecutionOomTest::ReleasePolicyResolver() 
  	{
  	}

//CleSessionProxy implemenation
void CSsmCmdExecutionOomTest::ExecuteCommandList(const CSsmCommandList& aCmdList, TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity)
	{
	TInt failRate = 1;
	iTestCleSession.HeapMark();
	//Execution of the coomand will happen in CLE. Mark CLE heap.
	for (;;failRate++)
		{
		iTestCleSession.HeapReset();
		iTestCleSession.SetHeapFailure(failRate);
		INFO_PRINTF2(_L("ExecuteCommandListL() started...Failrate is %d"),failRate);
		TRequestStatus trs;
		iTestCleSession.ExecuteCommandList(aCmdList, trs, aSeverity);	
		User::WaitForRequest(trs);
		INFO_PRINTF2(_L("ExecuteCommandListL() finished with status %d "),trs.Int());
		if(trs.Int() == KErrNone)
	    	{
	    	aStatus = KRequestPending;
			iStatetransitionEngineTRequestStatus = &aStatus;
			TRAPD(err, AsyncRequestCompleteL(iStatetransitionEngineTRequestStatus));
			TESTE(err==KErrNone, err);
	      	break;
	      }
		}
	iTestCleSession.HeapReset(); 
	iTestCleSession.HeapMarkEnd();
	}

void CSsmCmdExecutionOomTest::ExecuteCommandListCancel()
	{
	iTestCleSession.ExecuteCommandListCancel();
	}

void CSsmCmdExecutionOomTest::ConnectL() 
	{
	iTestCleSession.ConnectL(KSsmCleTestServerName);
	iConnected = ETrue;
	}
void CSsmCmdExecutionOomTest::ReleaseCle() 
	{
	}
void CSsmCmdExecutionOomTest::Close()
	{
	iTestCleSession.Close();
	iConnected = EFalse;
	}
TBool CSsmCmdExecutionOomTest::IsConnected() 
	{
	return iConnected;
	}

MCleSessionProxy* CSsmCmdExecutionOomTest::CreateNewProxyL()
	{
	return this;
	}

// CTestStatePolicy implemenation
MSsmStatePolicy* CTestStatePolicy::NewL(CSsmCmdExecutionOomTest& aTestStep)
	{	
	CTestStatePolicy* self = new (ELeave) CTestStatePolicy(aTestStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CTestStatePolicy::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KTestRscFileName(), *this);
	}

CTestStatePolicy::CTestStatePolicy(CSsmCmdExecutionOomTest& aTestStep)
	: iTestStep(aTestStep)
	{
	}

CTestStatePolicy::~CTestStatePolicy()
	{
	iFs.Close();
	delete iCommandListResourceReader;
	}

void CTestStatePolicy::Initialize(TRequestStatus& aStatus)
	{
	iCommandListResourceReader->Initialise(aStatus);
	}

 void CTestStatePolicy::InitializeCancel()
 	{
  	iCommandListResourceReader->InitialiseCancel();
  	}

void CTestStatePolicy::Release()
	{
	delete this;	
	}

/**
This function is valid/possible to call at any time.
*/	
MSsmStatePolicy::TResponse CTestStatePolicy::TransitionAllowed(const TSsmStateTransition& /*aRequest*/, TSsmStateTransition const* /*aCurrent*/, TSsmStateTransition const* /*aQueued*/, const RMessagePtr2& /*aMessage*/)
	{ 
	return EDefinitelyAllowed;
	}
	
TBool CTestStatePolicy::ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId)
	{
	DEBUGPRINT2(_L("CTestStatePolicy::ConditionalCommandAllowedL called with resource id: %d"), aResourceId);
	CConditionEvaluate* evaluate = CConditionEvaluate::NewLC();
	TBool ret = (evaluate->EvaluateL(aResourceFile,aResourceId)) ? ETrue : EFalse;
	CleanupStack::PopAndDestroy(evaluate);
	return ret;
	}

void CTestStatePolicy::PrepareCommandList(TSsmState aState, TInt /*aReason*/, TRequestStatus& aStatus)
	{
	TInt commandListId = KSsmAnySubState;
	iCommandListResourceReader->PrepareCommandList(commandListId, aState, aStatus);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

void CTestStatePolicy::PrepareCommandListCancel()
	{
	iCommandListResourceReader->PrepareCommandListCancel();
 	}

CSsmCommandList* CTestStatePolicy::CommandList()
	{	
	return iCommandListResourceReader->GetCommandList();
	}

TBool CTestStatePolicy::GetNextState(TSsmState /*aCurrentTransition*/, TInt /*aReason*/, TInt /*aError*/, TInt /*aSeverity*/, TSsmState& /*aNextState*/)
	{
	return EFalse;
	}

