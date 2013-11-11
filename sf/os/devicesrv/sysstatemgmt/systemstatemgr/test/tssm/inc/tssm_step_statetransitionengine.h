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
 

#ifndef __TSSM_STEP_STATETRANSITIONENGINE_H__
#define __TSSM_STEP_STATETRANSITIONENGINE_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmstatetransition.h>
#include "ssmstatepolicyresolverproxy.h"
#include "clesessionproxy.h"
#include <ssm/ssmcmd.hrh>

_LIT(KTSsmStateTransitionEngineStep,"SsmStateTransitionEngineStep");

class TCall;
class CTestPolicy;
class CSsmCommandList;
class CSsmStatePolicyFrame;

class CSsmStateTransitionEngineTest : public CTestStep, public MSsmStatePolicyResolverProxy, public MCleSessionProxy
	{
public:
	CSsmStateTransitionEngineTest();
	~CSsmStateTransitionEngineTest();
	
	void LogCall(const TCall& aCall);
	void AsyncRequestCompleteL(TRequestStatus* aStatus);
	void AsyncStopScheduler(TAny* aRequester);
	
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
	//from MSsmStatePolicyResolverProxy
	void GetStatePolicyL(TSsmState aState);
	CSsmStatePolicyFrame* Policy();
	void ReleasePolicyResolver();
	
	//from MCleSessionProxy
	void ExecuteCommandList(const CSsmCommandList& aCmdList, TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity);
	void ExecuteCommandListCancel();
	void ConnectL();
	void ReleaseCle();
	void Close();
	TBool IsConnected();
	MCleSessionProxy* CreateNewProxyL();
	
	void AppendPreparedCommandList(TInt aId);
	TArray<TInt> PreparedCommandLists() const;
	
private:	
	void doTestStateTransitionEngineTestForMemoryLeaksL();
	
	void doTestQueueBuildupL();
	
	void doTestTransitionSimpleL();
	void AssertTransitionSimple();
	
	void doTestTransitionStartupL();
	void AssertTransitionStartup();
	
	void doTestTransitionStartupStartupL();
	void AssertTransitionStartupStartup();
	
	void doTestTransitionStartupFailBeforeStartL();
	void AssertTransitionStartupFailBeforeStart();
	
	void doTestTransitionStartupFailL(TInt aFailWhen);
	void AssertTransitionStartupFailBeforeInitializePolicy();
	void AssertTransitionStartupFailBeforePrepareCommandList();
	void AssertTransitionStartupFailBeforeExecuteCommandList();
	void AssertTransitionStartupFailBeforeGetNextState();
	
private:
	CSsmStatePolicyFrame* iPolicy;
	RArray<TCall> iCallbacks;
	RArray<TInt> iPreparedCommandLists;
	CAsyncCallBack* iAsyncRequestComplete;
	CAsyncCallBack* iAsyncStopScheduler;
	CActiveScheduler* iActiveScheduler;
	TInt iStopSchedulerAfterCallbackCount;
	TRequestStatus* iStatetransitionEngineTRequestStatus;
	};

class CTestPolicy : public CBase, public MSsmStatePolicy
	{
public:
	enum TTestSubstates
		{
		ESubstate1,
		ESubstate2,
		ESubstate3,
		ESubstate4,
		};
	
	static MSsmStatePolicy* NewL(CSsmStateTransitionEngineTest& aTestStep); //lint !e1511 ordinal 1
	
	//from MSsmStatePolicy
	TResponse TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage);
	void PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	TBool GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	
	void NewLL(); //for logging purposes only
	
private:
	CTestPolicy(CSsmStateTransitionEngineTest& aTestStep);
	//from CBase
	~CTestPolicy();
	
private:
	CSsmStateTransitionEngineTest& iTestStep;
	CSsmCommandList* iCommandList;
	};	

#endif
