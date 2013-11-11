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
#include "trtc_adaptationplugin_step.h"
#include <test/susadaptionclisrv.h>

const TInt KAsyncMessageSlots = -1; // Taken from ssmadaptationcli.cpp

/** */
CRtcAdaptPlugin* CRtcAdaptPlugin::NewL(CTestRtcAdaptStep& aTestRtcAdaptStep) 
	{
	CRtcAdaptPlugin* self = new(ELeave) CRtcAdaptPlugin(aTestRtcAdaptStep);
	CleanupStack::PushL( self );
	self->Construct();
	CleanupStack::Pop();
	return self;
	}

/** */
void CRtcAdaptPlugin::CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusRtcAdaptionServerRequests aRequest)
	{
	aTrs = KRequestPending;
	iRtcStatus = &aTrs;
	TBool validity = ETrue;
	
	iPckgValidity() = validity;
	
	switch(aRequest)
		{
		case ETestValidateRtc:
			iRtcAdaptation.ValidateRtc(iPckgValidity,iStatus);
			SetActive();
			break;	
		case ETestSetWakeupAlarm:
			iRtcAdaptation.SetWakeupAlarm(iPckgAlarmTime,iStatus);;
			SetActive();
			break;	
		case ETestUnsetWakeupAlarm:
			iRtcAdaptation.UnsetWakeupAlarm(iStatus);
			SetActive();
			break;	
		case ETestRtcCancel:
			iRtcAdaptation.ValidateRtc(iPckgValidity,iStatus);
			iRtcAdaptation.SetWakeupAlarm(iPckgAlarmTime,iStatus);
			iRtcAdaptation.Cancel(); 
			SetActive();
			break;
		default:
			User::Panic(KTCTestRtcAdaptStep, KErrArgument);		
		}
	}

/** */
CRtcAdaptPlugin::CRtcAdaptPlugin(CTestRtcAdaptStep& aTestRtcAdaptStep)
	:CActive(EPriorityStandard),iTestRtcAdaptStep(aTestRtcAdaptStep)
	{
	CActiveScheduler::Add(this);
	}

/** */
CRtcAdaptPlugin::~CRtcAdaptPlugin()
	{
	Cancel();
	iRtcAdaptation.Close();
	}

/** */
void CRtcAdaptPlugin::RunL()
	{
	iTestRtcAdaptStep.SetActivationCompleted();
	User::LeaveIfError(iStatus.Int());
	if (iRtcStatus  && (iRtcStatus->Int() == KRequestPending))
		{
		User::RequestComplete(iRtcStatus, iStatus.Int());
		}
	}

/** */
void CRtcAdaptPlugin::DoCancel()
	{
	}

/** */
TInt CRtcAdaptPlugin::RunError(TInt aError)
	{
	if (iRtcStatus)
		{
		User::RequestComplete(iRtcStatus, aError);
		}
	return KErrNone;
	}

/** */
void CRtcAdaptPlugin::Construct()
	{		
	iRtcAdaptation.Connect();
	}

/***************************************
*	RTestSsmRtcAdaptation functions    *
****************************************/

EXPORT_C TInt RTestSsmRtcAdaptation::Connect()
	{
	const TInt err = Connect(KAsyncMessageSlots);
	return err;
	}

TInt RTestSsmRtcAdaptation::Connect(TInt aAsyncMessageSlotCount)
	{
	TInt err = RSsmAdaptationBase::Connect(aAsyncMessageSlotCount);
	if(err == KErrNone)
		{
		err = HeapMark();
		}
	return err;
	}

void RTestSsmRtcAdaptation::Close()
	{
	TInt ret = CleanupAdaptations();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMRTC"), ret));
	ret = HeapMarkEnd();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMRTC"), ret));
	RSsmAdaptationBase::Close();
	}

TInt RTestSsmRtcAdaptation::HeapMark()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeap);
		}
	return ret;
	}

TInt RTestSsmRtcAdaptation::HeapMarkEnd()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeapEnd);
		}
	return ret;
	}

TInt RTestSsmRtcAdaptation::CleanupAdaptations()
	{
	TInt ret = KErrNone;
    // Since actual plugins are being used, they cannot be unloaded.
    // Hence return without doing anything. There will be HeapMarkEnd to check that
    // memory is not leaked.
	return ret;
	}

/** */
CTestRtcAdaptStep::CTestRtcAdaptStep ()
	{
	SetTestStepName(KTCTestRtcAdaptStep);
	}

/** */
CTestRtcAdaptStep::~CTestRtcAdaptStep ()
	{
	delete iRtcAdapt;
	delete iActiveScheduler;
	delete iAsyncStopScheduler;
	}
/**
Old Test CaseID 		APPFWK-SAS-0004
New Test CaseID 		DEVSRVS-SSPLUGINS-SAS-0004
 */

void CTestRtcAdaptStep::TestRtcState()
	{
	for (TInt opcode=0; opcode<ETestRtcCancel; opcode++)
    	{
    	TRequestStatus trs;
    	TTestSusRtcAdaptionServerRequests rfsType = static_cast<TTestSusRtcAdaptionServerRequests>(opcode);
    	iRtcAdapt->CallCommandsOfAdaptation(trs,rfsType);
  		//this callback will not run until the test is finished because it has priority idle
 		iAsyncStopScheduler->CallBack();
   		iActiveScheduler->Start();
    	User::WaitForRequest(trs);
    	iAdptTransitionCompleted = EFalse;
    	TInt res = trs.Int();
    	switch(rfsType)
			{
			case ETestValidateRtc:
				TEST(KErrNotSupported == res);
				INFO_PRINTF3(_L("TestValidateRtc completed with %d error, Expected %d "),res,KErrNotSupported);
				break;	
			case ETestSetWakeupAlarm:
				TEST(KErrNotSupported == res);	
				INFO_PRINTF3(_L("TestSetWakeupAlarm completed with %d error, Expected %d "),res,KErrNotSupported);
				break;					
			case ETestUnsetWakeupAlarm:
				TEST(KErrNotSupported == res);	
				INFO_PRINTF3(_L("TestUnsetWakeupAlarm completed with %d error, Expected %d "),res,KErrNotSupported);
				break;	
			case ETestRtcCancel:
				TEST(KErrNotSupported == res);	
				INFO_PRINTF3(_L("TestRtcCancel completed with %d error, Expected %d "),res,KErrNotSupported);		
				break;
			default:
				break;
			}
		}
	}

/** */
TVerdict CTestRtcAdaptStep::doTestStepL()
	{
	iRtcAdapt = CRtcAdaptPlugin::NewL(*this);	

	__UHEAP_MARK;
	INFO_PRINTF1(_L("TestRtcAdaptStep started"));	
	TRAPD(err, TestRtcState());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("TestRtcAdaptStep completed with %d error "),err);	
	__UHEAP_MARKEND;

	return TestStepResult();
	}
