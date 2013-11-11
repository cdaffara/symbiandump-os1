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
 

#ifndef __TSSM_STEP_SWPTRANSITIONENGINE_H__
#define __TSSM_STEP_SWPTRANSITIONENGINE_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmswp.h>
#include "ssmswppolicyresolverproxy.h"
#include "clesessionproxy.h"
#include <ssm/ssmcmd.hrh>
#include "ssmswppolicyproxy.h"
#include <e32property.h>

_LIT(KTSsmSwpTransitionEngineStep,"SsmSwpTransitionEngineStep");

//--------------------- Thread creation framework ---------------------

// part of the framework
enum TFunction
	{
	EThreadFirst,
	EThreadSubmitTwice,
	EThreadLast
	};
TInt StartTestInThreadL(TInt aFunction);
TInt ThreadStartFn(TAny* aPtr);
void ThreadDispatchFunctionL(TInt aFunction);	

// functions that test something
void ThreadSubmitTwiceL();


//--------------------- CSsmSwpTransitionEngineTest ---------------------

class CSsmSwpTransitionEngineTest : public CTestStep, public MSsmSwpPolicyResolverProxy
	{
public:
	CSsmSwpTransitionEngineTest();
	~CSsmSwpTransitionEngineTest();
	
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
	//from MSwpPolicyResolverProxy
	CSsmSwpPolicyFrame* GetSwpPolicyL(const TSsmSwp& aSwp);
	CSsmSwpPolicyFrame* CreatePolicyLC(const RLibrary& aLibrary) const;
	
	void StopActiveScheduler();
private:	
	void doTestSwpTransitionEngineTestForMemoryLeaksL();
	void doTestRequestSimpleL();
	void doTestRequestCancelL();
	void doTestRequestSubmitTwiceL();
	
private:
	CAsyncCallBack* iAsyncStopScheduler;
	CActiveScheduler* iActiveScheduler;
	RProperty iProperty;
	};


#endif
