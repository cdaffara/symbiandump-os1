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


#include "ssmstatepolicyresolver.h"
#include "ssmstatepolicyframe.h"
#include <ssm/ssmstate.h>

#include "tssm_statepolicy_simple.h"
#include "tssm_statepolicy_bad.h"
#include "tssm_step_statepolicyresolver.h"

CSsmStatePolicyResolverTest::~CSsmStatePolicyResolverTest()
	{
	}

CSsmStatePolicyResolverTest::CSsmStatePolicyResolverTest()
	{
	SetTestStepName(KTSsmStatePolicyResolverStep);
	}

/** */
TVerdict CSsmStatePolicyResolverTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0004
New Test CaseID 		DEVSRVS-SSMA-SSM-0004
 */


TVerdict CSsmStatePolicyResolverTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmStatePolicyResolverTest started...."));
	
	__UHEAP_MARK;
	
	doTestPolicyResolverForMemoryLeaksL();
	doTestPolicyResolverFileErrHandlingL();
	doTestPolicyResolverHeapFailureL();
	doTestPolicyResolverStateL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmStatePolicyResolverTest completed!!"));
	return TestStepResult();
	}
	

/** */
TVerdict CSsmStatePolicyResolverTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}



void CSsmStatePolicyResolverTest::doTestPolicyResolverForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Checking for destructor memoryleaks in CSsmStatePolicyResolver"));
	
	__UHEAP_MARK;
	CSsmStatePolicyResolver* resolver = CSsmStatePolicyResolver::NewL();
	CleanupStack::PushL(resolver);
	TEST(NULL == resolver->Policy());
	
	resolver->GetStatePolicyL(TSsmState(ESsmTestSimpleState, KSsmAnySubState));
	TEST(NULL != resolver->Policy());
	
	CleanupStack::PopAndDestroy(resolver);
	__UHEAP_MARKEND;
	}

void CSsmStatePolicyResolverTest::doTestPolicyResolverFileErrHandlingL()
	{
	__UHEAP_MARK;
	CSsmStatePolicyResolver* resolver = CSsmStatePolicyResolver::NewL();
	
	INFO_PRINTF1(_L("Checking CSsmStatePolicyResolver's handling of missing a library file"));
	const TSsmState KNonExistentPolicy = TSsmState(0x00FF,0);
	TRAPD(err, resolver->GetStatePolicyL(KNonExistentPolicy));
	TEST(KErrNotFound == err);
	
	INFO_PRINTF1(_L("Checking CSsmStatePolicyResolver's handling of a library with wrong type"));
	TRAP(err, resolver->GetStatePolicyL(TSsmState(ESsmTestBadState, KSsmAnySubState)));
	TEST(KErrNotSupported == err);
	
	resolver->ReleasePolicyResolver();
	__UHEAP_MARKEND;
	}

void CSsmStatePolicyResolverTest::doTestPolicyResolverStateL()
	{
	INFO_PRINTF1(_L("Checking for State-Id saved in policy"));
	
	__UHEAP_MARK;
	CSsmStatePolicyResolver* resolver = CSsmStatePolicyResolver::NewL();
	CleanupStack::PushL(resolver);
	
	TEST(NULL == resolver->Policy());
	
	const TSsmState testState = TSsmState(ESsmTestSimpleState, KSsmAnySubState);
	resolver->GetStatePolicyL(testState); 
	CSsmStatePolicyFrame* policy = resolver->Policy();
	TEST(NULL != policy);
	TEST(testState.MainState() == policy->StatePolicyId());
	
	CleanupStack::PopAndDestroy(resolver);
	__UHEAP_MARKEND;
	}

void CSsmStatePolicyResolverTest::doTestPolicyResolverHeapFailureL()
	{
	INFO_PRINTF1(_L("Checking CSsmStatePolicyResolver for memoryleaks during OOM"));

	// CSsmStatePolicyResolver
	INFO_PRINTF1(_L("CSsmStatePolicyResolver construction under OOM"));
	CSsmStatePolicyResolver* resolver=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, resolver = CSsmStatePolicyResolver::NewL());
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret!=KErrNone)
	        {
	        __UHEAP_MARKEND;
	        TEST(resolver==NULL);
	        }
	    else
	        {
	        TEST(resolver!=NULL);
	        CleanupStack::PushL(resolver);
			CleanupStack::PopAndDestroy(resolver);
	        resolver = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d."),failRate-1);


	// GetStatePolicyL
	INFO_PRINTF1(_L("CSsmStatePolicyResolver GetStatePolicyL under OOM"));

	for (failRate = 1;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret,
	    	{
   		    resolver = CSsmStatePolicyResolver::NewL();
			CleanupStack::PushL(resolver);
			resolver->GetStatePolicyL(TSsmState(ESsmTestSimpleState, KSsmAnySubState));
			CleanupStack::PopAndDestroy(resolver);
	    	})
	    	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
        __UHEAP_MARKEND;

	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for GetStatePolicyL: %d."),failRate-1);
	}

