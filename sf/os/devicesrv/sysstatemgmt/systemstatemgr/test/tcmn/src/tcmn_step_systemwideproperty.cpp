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

#include <e32property.h>
#include <s32mem.h>
#include <ssm/ssmstate.h>
#include <ssm/ssmcommandfactory.h>
#include "ssmdebug.h"
#include "ssmpropertymonitor.h"
#include "cmdreqswpchange.h"
#include "tcmn_step_systemwideproperty.h"
#include "tssm_ssmclient.h"
#include "tssm_startserver.h"
//---------------- constants  ---------------------------------------

const TUint KTestSwpKey = 123456789;
const TUint KInvalidTestSwpKey = 666;

static TInt StopScheduler(TAny* /*aAny*/)
 	{
 	CActiveScheduler::Stop();
 	return KErrNone;
 	}



//---------------- Test Steps ---------------------------------------


void CCmnSystemWidePropertyTest::doTestRConnectL()
	{
	INFO_PRINTF1(_L("Calling RSsmSystemWideProperty APIs in a bad way"));
	__UHEAP_MARK;
	
	//Call Connect()  using a non-existent system wide property.
	RSsmSystemWideProperty rPropMon;
	TEST(KErrNone == rPropMon.Connect(KInvalidTestSwpKey));

	//If the property has not been defined, the request does not complete until the property is defined and published
	TRequestStatus status;
	rPropMon.Subscribe(status);
	TEST(status == KRequestPending);
	rPropMon.SubscribeCancel();

	//Call GetValue() without having connected
	rPropMon.Close();
	TInt test;
	TEST(KErrBadHandle == rPropMon.GetValue(test));

	//Call Subscribe() and SubscribeCancel() without having connected
	rPropMon.Subscribe(status);
	TEST(status == KErrBadHandle);
	rPropMon.SubscribeCancel();
	__UHEAP_MARKEND;
	}

void CCmnSystemWidePropertyTest::doTestRSsmSystemWidePropertyL()
	{
	INFO_PRINTF1(_L("Testing RSsmSystemWideProperty functionality"));
	__UHEAP_MARK;
	
	//Create R based property monitor
	RSsmSystemWideProperty rPropMon;
	CleanupClosePushL(rPropMon);
	TEST(KErrNone == rPropMon.Connect(KTestSwpKey, RProcess().SecureId()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNotFound == rPropMon.GetValue(value));
	TEST(0 == value);
	
	//Create the SWP and publish the inital value
	const TInt KInitialValue = 999;
	RProperty property;
	CleanupClosePushL(property);
	TEST(KErrNone == property.Define(RProcess().SecureId(), KTestSwpKey, RProperty::EInt));
	TEST(KErrNone == property.Set(RProcess().SecureId(), KTestSwpKey, KInitialValue));
	
	//Read the property without waiting for notification 
	TEST(KErrNone == rPropMon.GetValue(value));
	TEST(KInitialValue == value);
	
	//Cleanup 
	TEST(KErrNone == property.Delete(RProcess().SecureId(), KTestSwpKey));
	CleanupStack::PopAndDestroy(&property);
	CleanupStack::PopAndDestroy(&rPropMon);
	
	__UHEAP_MARKEND;
	}


void CCmnSystemWidePropertyTest::doTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Testing for memoryleaks in CSsmSystemWideProperty during out-of-memory conditions"));

	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	
	// CSsmSystemWideProperty
	INFO_PRINTF1(_L("CSsmSystemWideProperty construction under OOM"));
	CSsmSystemWideProperty* prop=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, 
	    	{
	    	prop = CSsmSystemWideProperty::NewLC(KTestSwpKey);
	    	CleanupStack::Pop(prop);
	    	})
	    		   
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret!=KErrNone)
	        {
	        __UHEAP_MARKEND;
	        TEST(prop==NULL);
	        }
	    else
	        {
	        TEST(prop!=NULL);
	        delete prop;
	        prop = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d."),failRate-1);


	// AddSubscriberL
	INFO_PRINTF1(_L("CSsmSystemWideProperty AddSubscriberL under OOM"));

	for (failRate = 1;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret,
	    	{
			prop = CSsmSystemWideProperty::NewLC(KTestSwpKey);
			prop->AddSubscriberL(*this);
			CleanupStack::PopAndDestroy(prop);
	    	})
	    	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
        __UHEAP_MARKEND;

	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for AddSubscriberL: %d."),failRate-1);
	
	CleanupStack::PopAndDestroy(sched);
	}
	

void CCmnSystemWidePropertyTest::doTestCSsmSystemWidePropertyL()
	{
	INFO_PRINTF1(_L("Testing CSsmSystemWideProperty functionality"));
	iSwpChanges.Reset();
	__UHEAP_MARK;
	
	//Create C based monitor
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CSsmSystemWideProperty* cPropMon = CSsmSystemWideProperty::NewLC(KTestSwpKey, RProcess().SecureId());
	cPropMon->AddSubscriberL(*this);
	
	//Test that RemoveSubscriber works by verifying we don't get double notifications
	cPropMon->AddSubscriberL(*this);
	cPropMon->RemoveSubscriber(*this);

	//Ensure the environment is clean
	TSsmSwp swpValue(0,0);
	TEST(KErrNotFound == cPropMon->GetValue(swpValue));
	TEST(swpValue.Key() == 0);
	TEST(swpValue.Value() == 0);
	
	//Create the SWP and publish the inital value
	const TInt KInitialValue = 999;
	RProperty property;
	CleanupClosePushL(property);
	TEST(KErrNone == property.Define(RProcess().SecureId(), KTestSwpKey, RProperty::EInt));
	TEST(KErrNone == property.Set(RProcess().SecureId(), KTestSwpKey, KInitialValue));

	CAsyncCallBack* stopper = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	CleanupStack::PushL(stopper);
	TCallBack stopSchedulerCallback(StopScheduler, this);
	stopper->Set(stopSchedulerCallback);
	
	//this callback will not run until the test is finished because it has priority idle
	stopper->CallBack();
	//will cause CSsmSystemWideProperty::RunL to run as cPropMon is subscribing on 'property' changes
	sched->Start();
	
	//Assert expected values
	TEST(1 == iSwpChanges.Count());
	TEST(KTestSwpKey == iSwpChanges[0].Key());
	TEST(KInitialValue == iSwpChanges[0].Value());

	//Now change the value of the Swp twice
	const TInt KSecondValue = 666;
	const TInt KThirdValue = 333;
	TEST(KErrNone == property.Set(RProcess().SecureId(), KTestSwpKey, KSecondValue));
	TEST(KErrNone == property.Set(RProcess().SecureId(), KTestSwpKey, KThirdValue));

	//will cause CSsmSystemWideProperty::RunL to run as cPropMon is subscribing on 'property' changes
	stopper->CallBack();
	sched->Start();
	CleanupStack::PopAndDestroy(stopper);

	//Assert expected values
	TEST(2 == iSwpChanges.Count()); //there should only be two notifications even though we published 3 values
	TEST(KTestSwpKey == iSwpChanges[1].Key());
	TEST(KThirdValue == iSwpChanges[1].Value()); //the second value should be lost

	//Cleanup 
	TEST(KErrNone == property.Delete(RProcess().SecureId(), KTestSwpKey));
	CleanupStack::PopAndDestroy(&property);
	CleanupStack::PopAndDestroy(cPropMon);
	CleanupStack::PopAndDestroy(sched);
	
	iSwpChanges.Reset();
	__UHEAP_MARKEND;
	}


void CCmnSystemWidePropertyTest::SwpChanged(TSsmSwp aSwp)
	{
	TRAPD(err, iSwpChanges.AppendL(aSwp));
	TESTE(err==KErrNone, err);
	} //lint !e1746 Suppress parameter 'aSwp' could be made const reference



//---------------- CCmnSystemWidePropertyTest step ---------------------------------------


CCmnSystemWidePropertyTest::~CCmnSystemWidePropertyTest()
	{
	iSwpChanges.Close();
	}

CCmnSystemWidePropertyTest::CCmnSystemWidePropertyTest()
	{
	SetTestStepName(KTCmnSystemWidePropertyStep);
	}

TVerdict CCmnSystemWidePropertyTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CCmnSystemWidePropertyTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}
/**
Old Test CaseID 		APPFWK-CMN-0007
New Test CaseID 		DEVSRVS-SSMA-CMN-0007
 */

TVerdict CCmnSystemWidePropertyTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CCmnSystemWidePropertyTest started...."));
	
	__UHEAP_MARK;
	
	//Entering tests for CSystemWideProperty
	doTestRConnectL();
	doTestRSsmSystemWidePropertyL();

	//Entering tests for RSystemWideProperty"
	doTestForMemoryLeaksL();
	doTestCSsmSystemWidePropertyL();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CCmnSystemWidePropertyTest completed!!"));
	
	return TestStepResult();
	}




