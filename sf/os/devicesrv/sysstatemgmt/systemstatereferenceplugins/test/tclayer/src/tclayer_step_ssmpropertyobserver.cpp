// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ssmclayer.h"

#include "ssmdebug.h"

#include "tclayer_step_ssmpropertyobserver.h"

//Include the test wrappers
#include "testpropertyobserver.h"

/**
 * Constructs a complete CCLayerTestSsmPropertyObserver object
 * @internalComponent
 */
CCLayerTestSsmPropertyObserver::CCLayerTestSsmPropertyObserver()
	{
	
	}

/**
 * Releases any resources associated with this test
 * @internalComponent
 */
CCLayerTestSsmPropertyObserver::~CCLayerTestSsmPropertyObserver()
	{
	
	}

/**
 * Test step preamble
 * @internalComponent
 */
TVerdict CCLayerTestSsmPropertyObserver::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/**
 * Test step - a single test step performs a series of tests.
 * 
 * See the individual doTest... methods on this class for details of the tests.
 * 
 * @internalComponent
 */
TVerdict CCLayerTestSsmPropertyObserver::doTestStepL()
	{
	INFO_PRINTF1(_L("Entering test for compatibility layer SSM property observer"));
	__UHEAP_MARK;
	// Cast to void as if delete fails it doesn't matter
	(void)RProperty::Delete(RProcess().SecureId(), KTestPropertyKey);
	TRAPD(err, doTestInitalizationAndDestructionL());
	TEST(err == KErrNone);
	TRAP(err, doTestNotifyAndResubscribeL());
	TEST(err == KErrNone);
	TRAP(err, doTestNotifyAndNoResubscribeL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	// Cast to void as if delete fails it doesn't matter
	(void)RProperty::Delete(RProcess().SecureId(), KTestPropertyKey);
	INFO_PRINTF1(_L("Leaving test for compatibility layer SSM property observer"));
	return TestStepResult();
	}

/**
 * Test step postamble
 * @internalComponent
 */
TVerdict CCLayerTestSsmPropertyObserver::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		AFSS-CLAYER-0020
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0020
 */	

void CCLayerTestSsmPropertyObserver::doTestInitalizationAndDestructionL()
	{
	INFO_PRINTF1(_L("Entering test for intialization and destruction of SSM property observer"));
	__UHEAP_MARK;
	CActiveScheduler* as = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);
		
	CTestPropertyObserver* propertyObserver = new (ELeave) CTestPropertyObserver();
	CleanupStack::PushL(propertyObserver);
	
	// Now cleanup
	propertyObserver->Release();
	CleanupStack::Pop(propertyObserver);
	propertyObserver = NULL;
	
	CleanupStack::PopAndDestroy(as);
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for intialization and destruction of SSM property observer"));
	}

static const TInt KInitialValue = 42; 
static const TInt KSecondValue = 7623;
static const TInt KValueA = 422; 
static const TInt KValueB = 7623;

/**
Old Test CaseID 		AFSS-CLAYER-0021
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0020
 */	

void CCLayerTestSsmPropertyObserver::doTestNotifyAndResubscribeL()
	{
	INFO_PRINTF1(_L("Entering test for notify and resubscribe of SSM property observer"));
	__UHEAP_MARK;
	CActiveScheduler* as = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);
	
	CTestPropertyObserver* propertyObserver = new (ELeave) CTestPropertyObserver();
	CleanupStack::PushL(propertyObserver);
	INFO_PRINTF1(_L("Initializing test SSM property observer"));
	propertyObserver->InitializeL();
	TEST(propertyObserver->IsAdded());
	
	INFO_PRINTF1(_L("Starting test SSM property observer without defined property"));
	TInt err = KErrNone;
	TRAP(err, propertyObserver->StartL());
	TEST(err == KErrNotFound);
	TEST(propertyObserver->GetPropertyChangedCount() == 0);
	
	INFO_PRINTF1(_L("Defining test property"));
	err = RProperty::Define(RProcess().SecureId(), KTestPropertyKey, RProperty::EInt);
	TEST(err == KErrNone);
	err = RProperty::Set(RProcess().SecureId(), KTestPropertyKey, KInitialValue);
	TEST(err == KErrNone);	
	
	INFO_PRINTF1(_L("Starting test SSM property observer with defined property"));
	propertyObserver->SetNextPropertyChangedReturn(ETrue);
	propertyObserver->StartL();
	TEST(propertyObserver->GetLastPropertyChangedValue() == KInitialValue);
	TEST(propertyObserver->GetPropertyChangedCount() == 1);
	// Check that it's resubscribed
	TEST(propertyObserver->IsActive());
	TEST(propertyObserver->iStatus == KRequestPending);
	
	INFO_PRINTF1(_L("Changing test property value"));
	err = RProperty::Set(RProcess().SecureId(), KTestPropertyKey, KSecondValue);
	
	User::After(500000); // Wait for 0.5sec 
	
	TEST(err == KErrNone);
	// Test that definition has completed the request
	TEST(propertyObserver->IsActive());
	TEST(propertyObserver->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Calling RunL for test property"));
	propertyObserver->SetNextPropertyChangedReturn(ETrue);
	// Start the active scheduler, CTestPropertyObserver will stop it for us in it's RunL
	CActiveScheduler::Start();
	TEST(propertyObserver->GetLastPropertyChangedValue() == KSecondValue);
	TEST(propertyObserver->GetPropertyChangedCount() == 2);
	// Check that it's resubscribed
	TEST(propertyObserver->IsActive());
	TEST(propertyObserver->iStatus == KRequestPending);
	
	INFO_PRINTF1(_L("Destroying test SSM property observer"));
	propertyObserver->Release();
	CleanupStack::Pop(propertyObserver);
	propertyObserver = NULL;
	
	INFO_PRINTF1(_L("Checking for stray signal by changing property again"));
	err = RProperty::Set(RProcess().SecureId(), KTestPropertyKey, KInitialValue);
	TEST(err == KErrNone);
	
	CleanupStack::PopAndDestroy(as);
	
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for notify and resubscribe of SSM property observer"));
	}
	
/**
Old Test CaseID 		AFSS-CLAYER-0022
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0020
 */	
void CCLayerTestSsmPropertyObserver::doTestNotifyAndNoResubscribeL()
	{
	INFO_PRINTF1(_L("Entering test for notify and no resubscribe of SSM property observer"));
	__UHEAP_MARK;
	CActiveScheduler* as = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(as);	
	CActiveScheduler::Install(as);
	
	CTestPropertyObserver* propertyObserver = new (ELeave) CTestPropertyObserver();
	CleanupStack::PushL(propertyObserver);
	INFO_PRINTF1(_L("Initializing test SSM property observer"));
	propertyObserver->InitializeL();
	TEST(propertyObserver->IsAdded());
	
	INFO_PRINTF1(_L("Starting test SSM property observer"));
	propertyObserver->SetNextPropertyChangedReturn(ETrue);
	propertyObserver->StartL();
	// We don't use the active scheduler to test to allow greater control of the API
	TEST(propertyObserver->GetLastPropertyChangedValue() == KInitialValue);
	TEST(propertyObserver->GetPropertyChangedCount() == 1);
	TEST(propertyObserver->IsActive());
	TEST(propertyObserver->iStatus == KRequestPending);
	
	INFO_PRINTF1(_L("Setting test property to KValueA"));
	TInt err = RProperty::Set(RProcess().SecureId(), KTestPropertyKey, KValueA);

	User::After(500000); // Wait for 0.5sec 
	
	TEST(err == KErrNone);
	// Test that definition has completed the request
	TEST(propertyObserver->IsActive());
	TEST(propertyObserver->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Calling RunL for test property"));
	propertyObserver->SetNextPropertyChangedReturn(EFalse);
	// Start the active scheduler, CTestPropertyObserver will stop it for us in it's RunL
	CActiveScheduler::Start();
	TEST(propertyObserver->GetLastPropertyChangedValue() == KValueA);
	TEST(propertyObserver->GetPropertyChangedCount() == 2);
	// Check that it's not resubscribed
	TEST(!propertyObserver->IsActive());
	
	INFO_PRINTF1(_L("Changing test property value to KValueB")); 
	err = RProperty::Set(RProcess().SecureId(), KTestPropertyKey, KValueB);
	TEST(err == KErrNone);
	TEST(propertyObserver->GetPropertyChangedCount() == 2);
	
	// Nothing should happen (there will be a stray signal later on if something has gone wrong)
	
	INFO_PRINTF1(_L("Destroying test SSM property observer"));
	propertyObserver->Release();
	CleanupStack::Pop(propertyObserver);
	propertyObserver = NULL;

	CleanupStack::PopAndDestroy(as);
	
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for notify and no resubscribe of SSM property observer"));	
	}
