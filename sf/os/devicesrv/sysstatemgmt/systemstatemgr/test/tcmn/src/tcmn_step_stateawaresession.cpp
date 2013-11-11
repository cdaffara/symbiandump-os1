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

#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmdomaindefs.h>
#include <e32property.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstate.h>
#include "ssmdebug.h"
#include "cmnpanic.h"
#include "ssmatest_utils.h"
#include "tcmn_step_stateawaresession.h"
#include "tssm_ssmclient.h"
#include "tssm_startserver.h"




TInt PanicTestThisThread(TAny* aOption);

const TDmDomainId KTestDdmDomain = {KSM2AppServicesDomain3};
const TDmDomainId KTest2DdmDomain = {KSM2AppServicesDomain4};
const TDmDomainId KInvalidDdmDomain = {666};

static TInt StopScheduler(TAny* /*aAny*/)
 	{
 	CActiveScheduler::Stop();
 	return KErrNone;
 	}
 	
//----------------------------------------------------------------
void CCmnStateAwareSessionTest::StateChanged(TSsmState aSsmState)
	{
	TRAPD(err, iStateChanges.AppendL(aSsmState));
	TESTE(err==KErrNone, err);
	}

void CCmnStateAwareSessionTest::doTestRConnectL()
	{
	INFO_PRINTF1(_L("Calling RSsmStateAwareSession APIs in a bad way"));
		
	//Call Connect()  using a unknown TDdmDomainId.
	RSsmStateAwareSession rStateAwareSes;
	TEST(KDmErrBadDomainId == rStateAwareSes.Connect(KInvalidDdmDomain));
	TRequestStatus status;
	StartThread(1);
	StartThread(0);
	rStateAwareSes.Close();
	CloseAllPanicWindowsL();
	
	//Call Connect() twice with same TDdmDomainId
	TEST(KErrNone == rStateAwareSes.Connect(KTestDdmDomain));
	TEST(KErrNone == rStateAwareSes.Connect(KTestDdmDomain));
	rStateAwareSes.RequestStateNotification(status);
	TEST(status==KRequestPending);
	rStateAwareSes.AcknowledgeStateNotification(KErrNone);
	rStateAwareSes.RequestStateNotificationCancel();
	TSsmState state;
	state = rStateAwareSes.State();
	rStateAwareSes.Close();

	//Call Connect() twice with different TDdmDomainId.
	TEST(KErrNone == rStateAwareSes.Connect(KTestDdmDomain));
	TEST(KErrNone == rStateAwareSes.Connect(KTest2DdmDomain));
	state = rStateAwareSes.State();
	rStateAwareSes.Close();
	rStateAwareSes.Close();
 	}


void CCmnStateAwareSessionTest::StartThread(TInt aOption)
	{
	RThread thread;
	TRequestStatus stat;
	TBuf<32> threadNameBuf;
	// Give each thread a unique name to avoid KErrAlreadyExists error on thread creation
	_LIT(KThreadNameFormat, "CCmnStateAwareSessionTest%d");
	threadNameBuf.Format(KThreadNameFormat, aOption);
	TInt threadCreationVal = thread.Create(threadNameBuf,PanicTestThisThread,KDefaultStackSize,0x2000,0x20000,(TAny*)aOption);
	TEST(threadCreationVal==KErrNone);	

	TRequestStatus status;
	thread.Logon(status);
	TBool jit =	User::JustInTime();
	User::SetJustInTime(EFalse);
	thread.Resume();
	User::WaitForRequest(status);
	TInt reason = thread.ExitReason();
	if (aOption == 1)
		{
		TEST(ECmnErrRqstStateNotif == reason);
		}
	else
		{
		TEST(ECmnErrAcknldgStateNotif == reason);
		}
	TExitCategoryName category = thread.ExitCategory();
	TEST(category.Compare(KPanicSsmCmn) == 0);
	thread.Close();
	User::SetJustInTime(jit);
	}

TInt PanicTestThisThread(TAny* aOption)
	{
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}
		
	RSsmStateAwareSession rStateAwareSes;	
	TInt err = rStateAwareSes.Connect(KInvalidDdmDomain);
	if (aOption)
		{
		TRequestStatus status;
		rStateAwareSes.RequestStateNotification(status);
		}
	else 
		{
		rStateAwareSes.AcknowledgeStateNotification(KErrNone);
		}
	rStateAwareSes.Close();	
	delete trapCleanup;
	return KErrNone;
	}
	
void CCmnStateAwareSessionTest::doTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Testing for memoryleaks in CSsmStateAwareSession during out-of-memory conditions"));

	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	// CSsmStateAwareSession
	INFO_PRINTF1(_L("CSsmStateAwareSession construction under OOM"));
	CSsmStateAwareSession* sas=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, sas = CSsmStateAwareSession::NewL(KAppServicesDomain3));
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret!=KErrNone)
	        {
	        __UHEAP_MARKEND;
	        TEST(sas==NULL);
	        }
	    else
	        {
	        TEST(sas!=NULL);
			delete sas;
	        sas = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d."),failRate-1);

	// AddSubscriberL
	INFO_PRINTF1(_L("CSsmStateAwareSession AddSubscriberL under OOM"));

	for (failRate = 1;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret,
	    	{
			sas = CSsmStateAwareSession::NewLC(KSM2AppServicesDomain3);
			sas->AddSubscriberL(*this);
			CleanupStack::PopAndDestroy(sas);
			})
	    	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
        
	    if (ret==KErrNone)
	        {
	        TEST(sas!=NULL);
	        break;
	        }
	    __UHEAP_MARKEND;
	    }

	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for AddSubscriberL: %d."),failRate-1);
	
	CleanupStack::PopAndDestroy(sched);
	}

void CCmnStateAwareSessionTest::doTestCSsmStateAwareSessionL()
	{
	INFO_PRINTF1(_L("Testing CSsmStateAwareSession functionality"));
	iStateChanges.Reset();
	__UHEAP_MARK;
	
	//Create C based monitor
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CSsmStateAwareSession* sas = CSsmStateAwareSession::NewLC(KTestDdmDomain);
	sas->AddSubscriberL(*this);
	
	//Test that RemoveSubscriber works by verifying we don't get double notifications
	sas->AddSubscriberL(*this);
	sas->RemoveSubscriber(*this);

	//Publish the new value
	const TInt KFirstValue = 0x00010001;
	RProperty property;
	CleanupClosePushL(property);
	TEST(KErrNone == property.Set(KUidDmPropertyCategory, 0x000002E6, KFirstValue));//DmStatePropertyKey =( aHierarchyId <<  | ((aDomainId << 8& 0xff0000) | (aDomainId & 0xff)

	CAsyncCallBack* stopper = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	CleanupStack::PushL(stopper);
	TCallBack stopSchedulerCallback(StopScheduler, this);
	stopper->Set(stopSchedulerCallback);
	
	//this callback will not run until the test is finished because it has priority idle
	stopper->CallBack();
	//will cause CSsmStateMonitor::RunL to run as sas is subscribing on 'state' changes
	sched->Start();
	
	//Assert expected values
	TEST(1 == iStateChanges.Count());
	TEST(1 == iStateChanges[0].MainState());
	TEST(1 == iStateChanges[0].SubState());

	//Now change the value of the State twice
	const TInt KSecondValue = 0x00FFFFFF;
	const TInt KThirdValue = 0x00ADF00D;
	TEST(KErrNone == property.Set(KUidDmPropertyCategory, 0x000002E6, KSecondValue));//DmStatePropertyKey =( aHierarchyId <<  | ((aDomainId << 8& 0xff0000) | (aDomainId & 0xff)
	TEST(KErrNone == property.Set(KUidDmPropertyCategory, 0x000002E6, KThirdValue));//DmStatePropertyKey =( aHierarchyId <<  | ((aDomainId << 8& 0xff0000) | (aDomainId & 0xff)

	//will cause CSsmStateMonitor::RunL to run as sas is subscribing on 'state' changes
	stopper->CallBack();
	sched->Start();
	CleanupStack::PopAndDestroy(stopper);

	//Assert expected values
	TEST(2 == iStateChanges.Count()); //there should only be two notifications even though we published 3 values
	TEST(173 == iStateChanges[1].MainState());
	TEST(61453 == iStateChanges[1].SubState()); //the second value should be lost

	//Cleanup 
	CleanupStack::PopAndDestroy(&property);
	CleanupStack::PopAndDestroy(sas);
	CleanupStack::PopAndDestroy(sched);
	
	iStateChanges.Reset();
	__UHEAP_MARKEND;
	}

//----------------------------------------------------------------


CCmnStateAwareSessionTest::~CCmnStateAwareSessionTest()
	{
	}

CCmnStateAwareSessionTest::CCmnStateAwareSessionTest()
	{
	SetTestStepName(KTCmnStateAwareSessionStep);
	}

TVerdict CCmnStateAwareSessionTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CCmnStateAwareSessionTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-CMN-0005
New Test CaseID 		DEVSRVS-SSMA-CMN-0005
 */

TVerdict CCmnStateAwareSessionTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CCmnStateAwareSessionTest started..."));

	doTestRConnectL();
	
	__UHEAP_MARK;
	doTestCSsmStateAwareSessionL();
	doTestForMemoryLeaksL();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CCmnStateAwareSessionTest completed!"));
	return TestStepResult();
	}

