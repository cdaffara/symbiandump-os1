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
#include "temergencycallrf_adaptationplugin_step.h"
#include <test/susadaptionclisrv.h>

#include <e32debug.h>

const TInt KAsyncMessageSlots = -1; // Taken from ssmadaptationcli.cpp


/** */
CEmergencyAdaptPlugin* CEmergencyAdaptPlugin::NewL(CTestEmergencyAdaptStep& aTestEmergencyAdaptStep) 
	{
	CEmergencyAdaptPlugin* self = new(ELeave) CEmergencyAdaptPlugin(aTestEmergencyAdaptStep);
	CleanupStack::PushL( self );
	self->Construct();
	CleanupStack::Pop();
	return self;
	}

/** */	
void CEmergencyAdaptPlugin::CallCommandsOfAdaptation(TRequestStatus& aStatus,TTestSusEmergencyCallAdaptionServerRequests aRequest)
	{
	aStatus = KRequestPending;
	iEmergencyStatus = &aStatus;
	switch(aRequest)
		{
		case ETestActivateRfForEmergencyCall:
			iEmergencyAdaptation.ActivateRfForEmergencyCall(iStatus);
			SetActive();
			break;
		case ETestDeactivateRfForEmergencyCall:
			iEmergencyAdaptation.DeactivateRfForEmergencyCall(iStatus);
			SetActive();
			break;
		case ETestEmergencyCallRfActAndDeActCall:
			iEmergencyAdaptation.ActivateRfForEmergencyCall(iStatus);
			iEmergencyAdaptation.DeactivateRfForEmergencyCall(iStatus);
			SetActive();
			break;
		default:
			User::Panic(KTCTestEmergencyAdaptStep, KErrArgument);			
		}
	}

/** */
CEmergencyAdaptPlugin::CEmergencyAdaptPlugin(CTestEmergencyAdaptStep& aTestEmergencyAdaptStep)
	:CActive(EPriorityStandard), iTestEmergencyAdaptStep(aTestEmergencyAdaptStep)
	{
	CActiveScheduler::Add(this);
	}

/** */
CEmergencyAdaptPlugin::~CEmergencyAdaptPlugin()
	{
	Cancel();
	iEmergencyAdaptation.Close();
	}

/** */
void CEmergencyAdaptPlugin::RunL()
	{
	iTestEmergencyAdaptStep.SetActivationCompleted();
	User::LeaveIfError(iStatus.Int());
	User::RequestComplete(iEmergencyStatus, iStatus.Int());
	}

/** */
void CEmergencyAdaptPlugin::DoCancel()
	{
	}

/** */
TInt CEmergencyAdaptPlugin::RunError(TInt aError)
	{
	if (iEmergencyStatus)
		{
		User::RequestComplete(iEmergencyStatus, aError);
		}
	return KErrNone;
	}

/** */
void CEmergencyAdaptPlugin::Construct()
	{		
	iEmergencyAdaptation.Connect();
	}


/***************************************************
*	RTestSsmEmergencyCallRfAdaptation functions    *
****************************************************/

EXPORT_C TInt RTestSsmEmergencyCallRfAdaptation::Connect()
	{
	const TInt err = Connect(KAsyncMessageSlots);
	return err;
	}

TInt RTestSsmEmergencyCallRfAdaptation::Connect(TInt aAsyncMessageSlotCount)
	{
	TInt err = RSsmAdaptationBase::Connect(aAsyncMessageSlotCount);
	if(err == KErrNone)
		{
		err = HeapMark();
		}
	return err;
	}

void RTestSsmEmergencyCallRfAdaptation::Close()
	{
	TInt ret = CleanupAdaptations();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMEMERCALL"), ret));
	ret = HeapMarkEnd();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMEMERCALL"), ret));
	RSsmAdaptationBase::Close();
	}

TInt RTestSsmEmergencyCallRfAdaptation::HeapMark()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeap);
		}
	return ret;
	}

TInt RTestSsmEmergencyCallRfAdaptation::HeapMarkEnd()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeapEnd);
		}
	return ret;
	}

TInt RTestSsmEmergencyCallRfAdaptation::CleanupAdaptations()
	{
	TInt ret = KErrNone;
	// Since actual plugins are being used, they cannot be unloaded.
	// Hence return without doing anything. There will be HeapMarkEnd to check that
	// memory is not leaked.
	return ret;
	}

/** */
CTestEmergencyAdaptStep::CTestEmergencyAdaptStep ()
	{
	SetTestStepName(KTCTestEmergencyAdaptStep);
	}

/** */
CTestEmergencyAdaptStep::~CTestEmergencyAdaptStep ()
	{
	delete iEmergencyAdapt;
	delete iActiveScheduler;
	delete iAsyncStopScheduler;
	}
/**
Old Test CaseID 		APPFWK-SAS-0005
New Test CaseID 		DEVSRVS-SSPLUGINS-SAS-0005
 */

void CTestEmergencyAdaptStep::TestEmergencyAdaptState()
	{
	for (TInt opcode = 0; opcode<=ETestEmergencyCallRfActAndDeActCall; ++opcode)
    	{
    	TRequestStatus trs;
    	TTestSusEmergencyCallAdaptionServerRequests rfsType = static_cast<TTestSusEmergencyCallAdaptionServerRequests>(opcode);
    	iEmergencyAdapt->CallCommandsOfAdaptation(trs,rfsType);
  		//this callback will not run until the test is finished because it has priority idle
 		iAsyncStopScheduler->CallBack();
   		iActiveScheduler->Start();
    	User::WaitForRequest(trs);
    	iAdptTransitionCompleted = EFalse;
    	RDebug::Print(_L("Status...%d"),trs.Int());
    	TInt res = trs.Int();
   		TEST(KErrNone == res);	

    	switch(rfsType)
			{
			case ETestActivateRfForEmergencyCall:
				INFO_PRINTF3(_L("TestActivateRfForEmergencyCall completed with %d error, Expected %d"),res,KErrNone);
				break;
			case ETestDeactivateRfForEmergencyCall:
				INFO_PRINTF3(_L("TestDeactivateRfForEmergencyCall completed with %d error, Expected %d"),res,KErrNone);
				break;
			case ETestEmergencyCallRfActAndDeActCall:
				INFO_PRINTF3(_L("TestEmergencyCallRfActAndDeActCall completed with %d error, Expected %d"),res,KErrNone);
				break;
			default:
				break;
			}
		}
		
	}

/**
Old Test CaseID 		APPFWK-SAS-0005
New Test CaseID 		DEVSRVS-SSPLUGINS-SAS-0005
 */

void CTestEmergencyAdaptStep::TestEmergencyAdaptStateCancel()
	{
	//The following test case has been moved from TestEmergencyAdaptState() 
	//since the test cases were failing only on NaviEngine. 
	//In NaviEngine, the requests were executed much earlier than expected. 
	//The test case expected the requests to be queued and then canceled.
	
	TRequestStatus tReq;
	TRequestStatus tReqCancel, tReqCancel1, tReqCancel2, tReqCancel3, tReqCancel4, tReqCancel5 ;

	RTestSsmEmergencyCallRfAdaptation callRfAdaptation;
	callRfAdaptation.Connect();

	callRfAdaptation.ActivateRfForEmergencyCall(tReq);
	    
	callRfAdaptation.DeactivateRfForEmergencyCall(tReqCancel);
	callRfAdaptation.DeactivateRfForEmergencyCall(tReqCancel1);
	callRfAdaptation.DeactivateRfForEmergencyCall(tReqCancel2);
	callRfAdaptation.DeactivateRfForEmergencyCall(tReqCancel3);
	callRfAdaptation.DeactivateRfForEmergencyCall(tReqCancel4);
	callRfAdaptation.DeactivateRfForEmergencyCall(tReqCancel5);
	
	callRfAdaptation.Cancel();
	
	User::WaitForRequest(tReqCancel5);
	
	TEST(KErrCancel == tReqCancel5.Int());	
		
	callRfAdaptation.Close();

	INFO_PRINTF3(_L("TestRequestCoopSysCancel completed with %d error , Expected %d "),tReqCancel5.Int(),KErrCancel);
	}

/** */
TVerdict CTestEmergencyAdaptStep::doTestStepL()
	{
	iEmergencyAdapt = CEmergencyAdaptPlugin::NewL(*this);
	__UHEAP_MARK;
	INFO_PRINTF1(_L("TestEmergencyAdaptStep started"));	
	TRAPD(err, TestEmergencyAdaptState());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("TestEmergencyAdaptState completed with %d error "),err);
	TRAP(err, TestEmergencyAdaptStateCancel());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("TestEmergencyAdaptStateCancel completed with %d error "),err);
	__UHEAP_MARKEND;

	return TestStepResult();
	}

