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
 

#ifndef __SSMSWPTRANSITIONSCHEDULERTEST_H__
#define __SSMSWPTRANSITIONSCHEDULERTEST_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmswp.h>
#include "ssmswppolicyresolverproxy.h"
#include "clesessionproxy.h"
#include "ssmswptransitionscheduler.h"
#include <ssm/ssmcmd.hrh>
#include "ssmswppolicyproxy.h"

_LIT(KTSsmSwpTransitionSchedulerStep,"SsmSwpTransitionSchedulerStep");

class TSwpCall;
class CSsmStatePolicyFrame;
class CSsmCommandList;

class CSsmSwpTransitionSchedulerTest : public CTestStep, public MSsmSwpPolicyResolverProxy, public MCleSessionProxy, public MSwpSchedulerMonitor
	{
public:
	CSsmSwpTransitionSchedulerTest();
	~CSsmSwpTransitionSchedulerTest();
	
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	void StopActiveScheduler();
	
	//from MSwpPolicyResolverProxy
	CSsmSwpPolicyFrame* GetSwpPolicyL(const TSsmSwp& aSwp);
	CSsmSwpPolicyFrame* CreatePolicyLC(const RLibrary& aLibrary) const;
	
	//from MSysCleProxy
	void ExecuteCommandList(const CSsmCommandList& aCmdList, TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity);
	void ExecuteCommandListCancel();
	void ConnectL();
	void ReleaseCle();
	void Close();
	TBool IsConnected();
	MCleSessionProxy* CreateNewProxyL();
		
	// from MSwpSchedulerMonitor
	void SwpTransitionQueued();
	void SwpTransitionStarted();
	void SwpTransitionCompleted(TUint aNumberCompleted);
	void SwpTransitionFailed(TUint aNumberFailed);
	
private:	
	void doTestSwpTransitionSchedulerTestForMemoryLeaksL();
	void doTestQueueL();
	void AssertQueue();
	void doTestRequestSimpleL();
	void AssertRequestSimple();
	void doTestRequestMultipleL();
	void AssertRequestMultiple();
	void doTestCancelL();
	void AssertCancel();

	void CreateAndSubmitEngineL(CSsmSwpTransitionScheduler* aScheduler, TSsmSwp& aSwp);
	void ResetCounters();
	
private:
	CActiveScheduler* iActiveScheduler;
	CAsyncCallBack* iAsyncStopScheduler;
	// test counters
	TUint iNumTransitionsQueued;
	TUint iNumTransitionsStarted;
	TUint iNumTransitionsCompleted;
	TUint iNumTransitionsFailed;
	TUint iExpectedNoOfTransitions;
	};


#endif
