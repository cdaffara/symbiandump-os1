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


#include <ssm/ssmswppolicy.h>
#include <ssm/ssmswp.h>

#include "ssmswppolicyresolver.h"
#include "ssmswppolicyframe.h"

#include "tssm_swppolicy_simple.h"
#include "tssm_swppolicy_bad.h"
#include "tssm_step_swppolicyresolver.h"

const TUint KSwpTestKeyGood = {0x11};
const TUint KSwpTestKeyBad = {0x22};
const TUint KSwpTestKeyNotExist = {0x666};
_LIT(KTestSwpPolicyDoesNotExistFile,"doesnotexist");


//--------------------------------------------------------------

CSsmSwpPolicyResolverTest::~CSsmSwpPolicyResolverTest()
	{
	}

CSsmSwpPolicyResolverTest::CSsmSwpPolicyResolverTest()
	{
	SetTestStepName(KTSsmSwpPolicyResolverStep);
	}

/** */
TVerdict CSsmSwpPolicyResolverTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0006
New Test CaseID 		DEVSRVS-SSMA-SSM-0006
 */

TVerdict CSsmSwpPolicyResolverTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSwpPolicyResolverTest started...."));
	
	__UHEAP_MARK;
	
	doTestPolicyResolverForMemoryLeaksL();
	doTestPolicyResolverFileErrHandlingL();
	doTestPolicyResolverHeapFailureL();
	doTestPolicyResolverStateL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmSwpPolicyResolverTest completed!!"));
	return TestStepResult();
	}

/** */
TVerdict CSsmSwpPolicyResolverTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}



void CSsmSwpPolicyResolverTest::doTestPolicyResolverForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Checking for destructor memoryleaks in CSsmStatePolicyResolver"));
	
	__UHEAP_MARK;
	CSsmSwpPolicyResolver* resolver = CSsmSwpPolicyResolver::NewL();
	CleanupStack::PushL(resolver);
	
	resolver->RegisterSwpMappingL(KSwpTestKeyGood, KTestSwpPolicySimpleFile);

	CSsmSwpPolicyFrame* testFrame1 = resolver->GetSwpPolicyL(TSsmSwp(KSwpTestKeyGood, 0));
	TEST(NULL != testFrame1);
	
	CSsmSwpPolicyFrame* testFrame2 = resolver->GetSwpPolicyL(TSsmSwp(KSwpTestKeyGood, 0)); 
	TEST(NULL != testFrame2);

	// These are owned by us
	delete testFrame1;
	delete testFrame2;
	
	CleanupStack::PopAndDestroy(resolver);
	__UHEAP_MARKEND;
	}

void CSsmSwpPolicyResolverTest::doTestPolicyResolverFileErrHandlingL()
	{
	__UHEAP_MARK;
	CSsmSwpPolicyResolver* resolver = CSsmSwpPolicyResolver::NewL();
	CleanupStack::PushL(resolver);

	//test of missing registration
	INFO_PRINTF1(_L("Checking CSsmSwpPolicyResolver's handling of missing registration"));
	TRAPD(err, resolver->GetSwpPolicyL(TSsmSwp(KSwpTestKeyNotExist, 0)));
	TESTE(KErrNotFound == err, err);

	//test of missing policy file
	INFO_PRINTF1(_L("Checking CSsmSwpPolicyResolver's handling of missing policy file"));
	resolver->RegisterSwpMappingL(KSwpTestKeyNotExist, KTestSwpPolicyDoesNotExistFile);
	TRAP(err, resolver->GetSwpPolicyL(TSsmSwp(KSwpTestKeyNotExist, 0)));
	TESTE(KErrNotFound == err, err);
		
	//test of wrong policy file
	INFO_PRINTF1(_L("Checking CSsmSwpPolicyResolver's handling of wrong policy file"));
	resolver->RegisterSwpMappingL(KSwpTestKeyBad, KTestSwpPolicyBadFile);
	TRAP(err, resolver->GetSwpPolicyL(TSsmSwp(KSwpTestKeyBad, 0)));
	TESTE(KErrNotSupported == err, err);
			
	CleanupStack::PopAndDestroy(resolver);
	__UHEAP_MARKEND;
	}

void CSsmSwpPolicyResolverTest::doTestPolicyResolverStateL()
	{
	INFO_PRINTF1(_L("Checking for State saved in policy"));
	
	__UHEAP_MARK;
	CSsmSwpPolicyResolver* resolver = CSsmSwpPolicyResolver::NewL();
	CleanupStack::PushL(resolver);
	
	resolver->RegisterSwpMappingL(KSwpTestKeyGood, KTestSwpPolicySimpleFile);

	const TSsmSwp testChange = TSsmSwp(KSwpTestKeyGood, 0);
	CSsmSwpPolicyFrame* policy = resolver->GetSwpPolicyL(testChange); 
	TEST(NULL != policy);
	TEST(testChange.Key() == policy->SwpKey());
	
	delete policy;
	
	CleanupStack::PopAndDestroy(resolver);
	__UHEAP_MARKEND;
	}

void CSsmSwpPolicyResolverTest::doTestPolicyResolverHeapFailureL()
	{
	INFO_PRINTF1(_L("Checking CSsmSwpPolicyResolver for memoryleaks during OOM"));

	// CSsmSwpPolicyResolver
	INFO_PRINTF1(_L("CSsmSwpPolicyResolver construction under OOM"));
	CSsmSwpPolicyResolver* resolver=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, resolver = CSsmSwpPolicyResolver::NewL());
	    
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


	// RegisterSwpMappingL
	INFO_PRINTF1(_L("CSsmSwpPolicyResolver RegisterSwpMappingL under OOM"));

	for (failRate = 1;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret,
	    	{
   		    resolver = CSsmSwpPolicyResolver::NewL();
			CleanupStack::PushL(resolver);
			resolver->RegisterSwpMappingL(KSwpTestKeyGood, KTestSwpPolicySimpleFile);
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
	INFO_PRINTF2(_L("  #allocs for RegisterSwpMappingL: %d."),failRate-1);


	// GetSwpPolicyL
	INFO_PRINTF1(_L("CSsmSwpPolicyResolver GetSwpPolicyL under OOM"));

	CSsmSwpPolicyFrame* pol=NULL;
	for (failRate = 1;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret,
	    	{
   		    resolver = CSsmSwpPolicyResolver::NewL();
			CleanupStack::PushL(resolver);
			resolver->RegisterSwpMappingL(KSwpTestKeyGood, KTestSwpPolicySimpleFile);
			pol = resolver->GetSwpPolicyL(TSsmSwp(KSwpTestKeyGood, 0));
			CleanupStack::PopAndDestroy(resolver);
	    	})
	    	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret!=KErrNone)
	        {
	        __UHEAP_MARKEND;
	        TEST(pol==NULL);
	        }
	    else
	        {
	        TEST(pol!=NULL);
	        delete pol;
	        pol = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for GetSwpPolicyL: %d."),failRate-1);
	}

