// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
  
#include "tstate_adaptationplugin_step.h"
#include "ssmdebug.h"
#include <test/susadaptionclisrv.h>

const TInt KAsyncMessageSlots = -1; // Taken from ssmadaptationcli.cpp

//
// TestStep
//
/** */
CStateAdaptPlugin* CStateAdaptPlugin::NewL(CTestStateAdaptStep& aTestStateAdaptStep) 
	{
	CStateAdaptPlugin* self = new(ELeave) CStateAdaptPlugin(aTestStateAdaptStep);
	CleanupStack::PushL( self );
	self->Construct();
	CleanupStack::Pop();
	return self;
	}

/** */	
void CStateAdaptPlugin::CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusStateAdaptionServerRequests aRequest)
	{
	
	aTrs = KRequestPending;
	iStateStatus = &aTrs;
	TInt reason = 0;
	TSsmRfsType rfsType = ESsmShallowRfs;
	TSsmCoopSysEventType evType = ESsmFatalCoopSysError;
	
	iPckgEvType() = evType;
	
	switch(aRequest)
		{
		case ETestRequestCoopSysStateChange:
			iStateAdaptation.RequestCoopSysStateChange(TSsmState(ESsmStartup, KSsmAnySubState), iStatus);	
			SetActive();
            break;
        case ETestRequestCoopSysSelfTest:
			iStateAdaptation.RequestCoopSysSelfTest(iStatus);
			SetActive();        	
			break;
        case ETestRequestCoopSysPerformRestartActions:
			iStateAdaptation.RequestCoopSysPerformRestartActions(reason,iStatus);
			SetActive();
        	break;
        case ETestRequestCoopSysPerformShutdownActions:
			iStateAdaptation.RequestCoopSysPerformShutdownActions(reason,iStatus);
			SetActive();
        	break;
        case ETestRequestCoopSysPerformRfsActions:
			iStateAdaptation.RequestCoopSysPerformRfsActions(rfsType,iStatus);
			SetActive();
        	break;
		default:
			User::Panic(KTCTestStateAdaptStep, KErrArgument);			  			
					
		}
	}
	
/** */	
CStateAdaptPlugin::CStateAdaptPlugin(CTestStateAdaptStep& aTestStateAdaptStep)
	:CActive(EPriorityStandard),iTestStateAdaptStep(aTestStateAdaptStep)		
	{
	CActiveScheduler::Add(this);
	}

/** */
CStateAdaptPlugin::~CStateAdaptPlugin()
	{
	Cancel();
	iStateAdaptation.Close();
	}

/** */
void CStateAdaptPlugin::RunL()
	{
	iTestStateAdaptStep.SetActivationCompleted();
	User::LeaveIfError(iStatus.Int());
	if (iStateStatus  && (iStateStatus->Int() == KRequestPending))
		{
		User::RequestComplete(iStateStatus, iStatus.Int());
		}
	}

/** */
void CStateAdaptPlugin::DoCancel()
	{
	}

/** */
TInt CStateAdaptPlugin::RunError(TInt aError)
	{
	if (iStateStatus)
		{
		User::RequestComplete(iStateStatus, aError);
		}
	return KErrNone;
	}

/** */
void CStateAdaptPlugin::Construct()
	{		
	iStateAdaptation.Connect();	
	}

/*****************************************
*	RTestSsmStateAdaptation functions    *
******************************************/

EXPORT_C TInt RTestSsmStateAdaptation::Connect()
	{
	const TInt err = Connect(KAsyncMessageSlots);
	return err;
	}

TInt RTestSsmStateAdaptation::Connect(TInt aAsyncMessageSlotCount)
	{
	TInt err = RSsmAdaptationBase::Connect(aAsyncMessageSlotCount);
	if(err == KErrNone)
		{
		err = HeapMark();
		}
	return err;
	}

void RTestSsmStateAdaptation::Close()
	{
	TInt ret = CleanupAdaptations();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMSTATE"), ret));
	ret = HeapMarkEnd();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMSTATE"), ret));
	RSsmAdaptationBase::Close();
	}

TInt RTestSsmStateAdaptation::HeapMark()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeap);
		}
	return ret;
	}

TInt RTestSsmStateAdaptation::HeapMarkEnd()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeapEnd);
		}
	return ret;
	}

TInt RTestSsmStateAdaptation::CleanupAdaptations()
	{
	TInt ret = KErrNone;
    // Since actual plugins are being used, they cannot be unloaded.
    // Hence return without doing anything. There will be HeapMarkEnd to check that
    // memory is not leaked.
	return ret;
	}

CTestStateAdaptStep::CTestStateAdaptStep ()
	{
	SetTestStepName(KTCTestStateAdaptStep);
	}

CTestStateAdaptStep::~CTestStateAdaptStep ()
	{
	delete iStateAdapt;
	delete iActiveScheduler;
	delete iAsyncStopScheduler;
	}
/**
Old Test CaseID 		APPFWK-SAS-0001
New Test CaseID 		DEVSRVS-SSPLUGINS-SAS-0001
 */

void CTestStateAdaptStep::TestRequestSysStateChange()
	{
	for (TInt opcode=0; opcode<=ETestRequestCoopSysPerformRfsActions; opcode++)
    	{
    	TRequestStatus trs;
    	TTestSusStateAdaptionServerRequests rfsType = static_cast<TTestSusStateAdaptionServerRequests>(opcode);
    	iStateAdapt->CallCommandsOfAdaptation(trs,rfsType);
  		//this callback will not run until the test is finished because it has priority idle
 		iAsyncStopScheduler->CallBack();
   		iActiveScheduler->Start();
    	User::WaitForRequest(trs);
    	iAdptTransitionCompleted = EFalse;
    	TInt res = trs.Int();
    	TEST(KErrNone == res);
    	switch(rfsType)
			{
			case ETestRequestCoopSysStateChange:
				INFO_PRINTF3(_L("TestRequestCoopSysStateChange completed with %d error , Expected %d "),res,KErrNone);	
            break;
        	case ETestRequestCoopSysSelfTest:
				INFO_PRINTF3(_L("TestRequestCoopSysSelfTest completed with %d error , Expected %d "),res,KErrNone);	
			break;
        	case ETestRequestCoopSysPerformRestartActions:
				INFO_PRINTF3(_L("TestRequestCoopSysPerformRestartActions completed with %d error , Expected %d "),res,KErrNone);	
        	break;
        	case ETestRequestCoopSysPerformShutdownActions:
				INFO_PRINTF3(_L("TestRequestCoopSysPerformShutdownActions completed with %d error , Expected %d "),res,KErrNone);	
        	break;
        	case ETestRequestCoopSysPerformRfsActions:
				INFO_PRINTF3(_L("TestRequestCoopSysPerformRfsActions completed with %d error , Expected %d "),res,KErrNone);	
			break;	
			default:
			break;
			}
    	}
    	
	}
	
/**
Old Test CaseID 		APPFWK-SAS-0001
New Test CaseID 		DEVSRVS-SSPLUGINS-SAS-0001
 */

void CTestStateAdaptStep::TestRequestSysStateChangeCancel()
	{
	//The following test case has been moved from TestRequestSysStateChange() 
	//since the test cases were failing only on NaviEngine. 
	//In NaviEngine, the requests were executed much earlier than expected. 
	//The test case expected the requests to be queued and then canceled.
	TRequestStatus tReq;
	TRequestStatus tReqCancel, tReqCancel1, tReqCancel2, tReqCancel3, tReqCancel4, tReqCancel5 ;

    RTestSsmStateAdaptation stateAdaptation;
    stateAdaptation.Connect();

    stateAdaptation.RequestCoopSysStateChange(TSsmState(ESsmStartup, KSsmAnySubState), tReq);	
    
	stateAdaptation.RequestCoopSysSelfTest(tReqCancel);
	stateAdaptation.RequestCoopSysSelfTest(tReqCancel1);
	stateAdaptation.RequestCoopSysSelfTest(tReqCancel2);
	stateAdaptation.RequestCoopSysSelfTest(tReqCancel3);
	stateAdaptation.RequestCoopSysSelfTest(tReqCancel4);
	stateAdaptation.RequestCoopSysSelfTest(tReqCancel5);

	stateAdaptation.RequestCancel();
	
    User::WaitForRequest(tReqCancel5);
	
	TEST(KErrCancel == tReqCancel5.Int());	
	
    stateAdaptation.Close();

	INFO_PRINTF3(_L("TestRequestCoopSysCancel completed with %d error , Expected %d "),tReqCancel5.Int(),KErrCancel);	
			
	}	

TVerdict CTestStateAdaptStep::doTestStepL()
	{
	iStateAdapt = CStateAdaptPlugin::NewL(*this);	

	__UHEAP_MARK;
	INFO_PRINTF1(_L("TestStateAdaptStep started"));
	TRAPD(err, TestRequestSysStateChange());
	TEST(err == KErrNone);
	TRAP(err, TestRequestSysStateChangeCancel());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("TestRequestCoopSysStateChange completed with %d error"),err);
	__UHEAP_MARKEND;

	return TestStepResult();
	}





