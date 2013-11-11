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
// This test case will execute the command list containing conditional information under OOM.
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 
#ifndef __TSSM_STEP_CMDEXECUTIONOOM_H__
#define __TSSM_STEP_CMDEXECUTIONOOM_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmconditionalcallback.h>
#include "ssmstatepolicyresolverproxy.h"
#include "clesessionproxy.h"
#include "tcle_cleclient.h"

_LIT(KTSsmCmdExecutionOomStep,"SsmCmdExecutionOomStep");

class CSsmCommandListResourceReader;
class CTestStatePolicy;
class CSsmCommandList;
class CSsmStatePolicyFrame;

class CSsmCmdExecutionOomTest : public CTestStep, public MSsmStatePolicyResolverProxy, public MCleSessionProxy
	{
public:	
	static CSsmCmdExecutionOomTest* NewL();
	CSsmCmdExecutionOomTest();
	~CSsmCmdExecutionOomTest();
	
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
	void doTestCleOOML();
private:
	CSsmStatePolicyFrame* iPolicy;
	RArray<TInt> iPreparedCommandLists;
	CAsyncCallBack* iAsyncRequestComplete;
	CAsyncCallBack* iAsyncStopScheduler;
	CActiveScheduler* iActiveScheduler;
	TRequestStatus* iStatetransitionEngineTRequestStatus;
	RCleSessionTest iTestCleSession;
	TBool iConnected;
	};

class CTestStatePolicy : public CBase, public MSsmStatePolicy, public MSsmConditionalCallback
	{
public:
	static MSsmStatePolicy* NewL(CSsmCmdExecutionOomTest& aTestStep); //lint !e1511 ordinal 1
	
	//from MSsmStatePolicy
	TResponse TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage);
	void PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	TBool GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	
	// from MSsmConditionalCallback
	TBool ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId);
	
private:
	void ConstructL();
	CTestStatePolicy(CSsmCmdExecutionOomTest& aTestStep);
	//from CBase
	~CTestStatePolicy();
private:
	RFs iFs;
	CSsmCommandListResourceReader* iCommandListResourceReader;
	CSsmCmdExecutionOomTest& iTestStep;
	};	

#endif
