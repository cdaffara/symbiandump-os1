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


#ifndef __SSMSWPREQUESTHANDLERTEST_H__
#define __SSMSWPREQUESTHANDLERTEST_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmswp.h>
#include "ssmswppolicyresolverproxy.h"
#include "clesessionproxy.h"
#include "ssmswptransitionscheduler.h"
#include "ssmswprequesthandler.h"
#include <ssm/ssmcmd.hrh>
#include "ssmswppolicyproxy.h"

_LIT(KTSsmSwpRequestHandlerStep,"SsmSwpRequestHandlerStep");

class TSwpCall;
class CSsmStatePolicyFrame;
class CSsmCommandList;

class CSsmSwpRequestHandlerTest : public CTestStep, public MSsmSwpPolicyResolverProxy, public MCleSessionProxy, public MSwpSchedulerMonitor, public MSwpHandlerMonitor
							
	{
public:
	CSsmSwpRequestHandlerTest();
	~CSsmSwpRequestHandlerTest();
	
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
	//from MPolicyResolverProxy
	CSsmSwpPolicyFrame* GetSwpPolicyL(const TSsmSwp& aSwp);
	CSsmSwpPolicyFrame*	CreatePolicyLC(const RLibrary& aLibrary) const;
	
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
	
	// from MSwpHandlerMonitor
	void SwpRequestMade(const TSsmSwp& aRequest);
	void StopActiveScheduler();

private:	
	void doTestSwpRequestHandlerTestForMemoryLeaksL();
	void doTestSimpleSubmissionL();
	void AssertSimpleSubmission();
	void doTestMultipleSubmissionL();
	void AssertMultipleSubmission();
	void doTestCancelSubmissionL();
	void AssertCancelSubmission();
	
	void ResetCounters();
	
private:
	CActiveScheduler* iActiveScheduler;
	CAsyncCallBack* iAsyncStopScheduler;
	
	// test counters

	TUint iNumTransitionsQueued;
	TUint iNumTransitionsStarted;
	TUint iNumTransitionsCompleted;
	TUint iNumTransitionsFailed;

	TUint iNumRequestsMade;
	TUint iExpectedNoOfTransitions;
};


#endif
