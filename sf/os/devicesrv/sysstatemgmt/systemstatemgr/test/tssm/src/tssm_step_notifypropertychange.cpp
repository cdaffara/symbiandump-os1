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

#include "tssm_step_notifypropertychange.h"
#include "tssm_ssmclient.h"
#include "tssm_startserver.h"
//---------------- contants  ---------------------------------------

const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)
const TUid KPropertyCategory=KMySID;

const TUint KTestSwp1={0x00B0BF};
_LIT(KTestSwpPolicyCommonFile, "ssm.swp.policy.test.common.dll");

static TInt StopScheduler(TAny* /*aAny*/)
 	{
 	CActiveScheduler::Stop();
 	return KErrNone;
 	}


//---------------- Test Steps ---------------------------------------

void CSsmNotifyPropertyChangeTest::SwpChanged(TSsmSwp aSwp)
	{
	TRAPD(err, iSwpChanges.AppendL(aSwp));
	TESTE(err==KErrNone, err);
	} //lint !e1746 Suppress parameter 'aSwp' could be made const reference



//---------------- CSsmNotifyPropertyChangeTest step ---------------------------------------


CSsmNotifyPropertyChangeTest::~CSsmNotifyPropertyChangeTest()
	{
	iSwpChanges.Close();
	}

CSsmNotifyPropertyChangeTest::CSsmNotifyPropertyChangeTest()
	{
	SetTestStepName(KTSsmNotifyPropertyChangeStep);
	}

TVerdict CSsmNotifyPropertyChangeTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmNotifyPropertyChangeTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

 void CSsmNotifyPropertyChangeTest::doTestSwpChangeL()
 	{
 	INFO_PRINTF1(_L("....CSsmNotifyPropertyChangeTest::doTestSwpChange() started!!"));
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	RSsmStateManager client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);

	// Create swps
	TSsmSwp swp(KTestSwp1, 1);
	RProperty property;
	TEST(KErrNone == property.Define(KPropertyCategory, swp.Key(), RProperty::EInt));
	TEST(KErrNone == property.Set(KPropertyCategory, swp.Key(), swp.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KPropertyCategory, swp.Key(), value));
	TESTE(1 == value, value);

	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	TEST(KErrNone == client.RegisterSwpMapping(KTestSwp1, KTestSwpPolicyCommonFile));
	
	CSsmSystemWideProperty* cProp = CSsmSystemWideProperty::NewLC(KTestSwp1);
	
	//Subscribe to changes in swp
	cProp->AddSubscriberL(*this);
	
	//Let client request swp changes
	swp.Set(KTestSwp1, 2);
	TRequestStatus status;
	client.RequestSwpChange(swp, status);
	TEST(status == KRequestPending);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	INFO_PRINTF2(_L("  Error is: %d."),status.Int());
	
	User::After(500000); // Wait for 0.5sec to allow transitions to fully complete
	
	CAsyncCallBack* stopper = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	CleanupStack::PushL(stopper);
	TCallBack stopSchedulerCallback(StopScheduler, this);
	stopper->Set(stopSchedulerCallback);
	
	//this callback will not run until the test is finished because it has priority idle
	stopper->CallBack();
	//will cause CSsmSystemWideProperty::RunL to run as cProp is subscribing on 'property' changes
	sched->Start();

	//Assert expected values
	TEST(1 == iSwpChanges.Count());
	TEST(KTestSwp1 == iSwpChanges[0].Key());
	TEST(2 == iSwpChanges[0].Value());

	// clean-up
	TEST(KErrNone == property.Delete(KPropertyCategory, swp.Key()));
	property.Close();
	client.Close();
	iSwpChanges.Reset();

	CleanupStack::PopAndDestroy(3, sched);
 	}

/**
Old Test CaseID 		APPFWK-SSM-0020
New Test CaseID 		DEVSRVS-SSMA-SSM-0020
 */


TVerdict CSsmNotifyPropertyChangeTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmNotifyPropertyChangeTest started...."));
	
	__UHEAP_MARK;
	
	doTestSwpChangeL();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmNotifyPropertyChangeTest completed!!"));
	
	return TestStepResult();
	}




