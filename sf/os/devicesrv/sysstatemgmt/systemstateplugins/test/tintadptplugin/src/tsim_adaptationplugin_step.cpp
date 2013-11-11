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

#include "tsim_adaptationplugin_step.h"
#include <test/susadaptionclisrv.h>

const TInt KAsyncMessageSlots = -1; // Taken from ssmadaptationcli.cpp

/** */
CSimAdaptPlugin* CSimAdaptPlugin::NewL(CTestSimAdaptStep& aTestSimAdaptStep) 
	{
	CSimAdaptPlugin* self = new(ELeave) CSimAdaptPlugin(aTestSimAdaptStep);
	CleanupStack::PushL( self );
	self->Construct();
	CleanupStack::Pop();
	return self;
	}

/** */	
void CSimAdaptPlugin::CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusSimAdaptionServerRequests aRequest)
	{
	aTrs = KRequestPending;
	iSimStatus = &aTrs;
	
	iSimStatusPckg() = ETrue;
	iEventPckg() = ESsmSimUsable;
	switch(aRequest)
		{
		case ETestGetSimOwned:	
			iSimAdaptation.GetSimOwned(iSimStatusPckg,iStatus);	
			SetActive();
			break;
		case ETestGetLastSimEvent:
			iSimAdaptation.GetLastSimEvent(iEventPckg);
			iSimAdaptation.NotifySimEvent(iEventPckg,iStatus);
			SetActive();
			break;
		case ETestNotifySimCancel:	
			iSimAdaptation.NotifySimEvent(iEventPckg,iStatus);
			iSimAdaptation.NotifyCancel();
			SetActive();
			break;
		default:
			User::Panic(KTCTestSimAdaptStep, KErrArgument);	
			break;	
		}
	}

/** */
CSimAdaptPlugin::CSimAdaptPlugin(CTestSimAdaptStep& aTestSimAdaptStep)
	:CActive(EPriorityStandard),iTestSimAdaptStep(aTestSimAdaptStep)
	{
	CActiveScheduler::Add(this);
	}

/** */
CSimAdaptPlugin::~CSimAdaptPlugin()
	{
	Cancel();
	iSimAdaptation.Close();
	}

/** */
void CSimAdaptPlugin::RunL()
	{
	iTestSimAdaptStep.SetActivationCompleted();
	User::LeaveIfError(iStatus.Int());
	if (iSimStatus  && (iSimStatus->Int() == KRequestPending))
		{
		User::RequestComplete(iSimStatus, iStatus.Int());
		}
	}

/** */
void CSimAdaptPlugin::DoCancel()
	{
	}

/** */
TInt CSimAdaptPlugin::RunError(TInt aError)
	{
	if (iSimStatus)
		{
		User::RequestComplete(iSimStatus, aError);
		}
	return KErrNone;
	}

/** */
void CSimAdaptPlugin::Construct()
	{		
	iSimAdaptation.Connect();
	}

/***************************************
*	RTestSsmSimAdaptation functions    *
****************************************/

EXPORT_C TInt RTestSsmSimAdaptation::Connect()
	{
	const TInt err = Connect(KAsyncMessageSlots);
	return err;
	}

TInt RTestSsmSimAdaptation::Connect(TInt aAsyncMessageSlotCount)
	{
	TInt err = RSsmAdaptationBase::Connect(aAsyncMessageSlotCount);
	if(err == KErrNone)
		{
		err = HeapMark();
		}
	return err;
	}

void RTestSsmSimAdaptation::Close()
	{
	TInt ret = CleanupAdaptations();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMSIM"), ret));
	ret = HeapMarkEnd();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMSIM"), ret));
	RSsmAdaptationBase::Close();
	}

TInt RTestSsmSimAdaptation::HeapMark()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeap);
		}
	return ret;
	}

TInt RTestSsmSimAdaptation::HeapMarkEnd()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeapEnd);
		}
	return ret;
	}

TInt RTestSsmSimAdaptation::CleanupAdaptations()
	{
	TInt ret = KErrNone;
    // Since actual plugins are being used, they cannot be unloaded.
    // Hence return without doing anything. There will be HeapMarkEnd to check that
    // memory is not leaked.
	return ret;
	}

/** */
CTestSimAdaptStep::CTestSimAdaptStep ()
	{
	SetTestStepName(KTCTestSimAdaptStep);
	}

/** */
CTestSimAdaptStep::~CTestSimAdaptStep ()
	{
	delete iSimAdapt;
	delete iActiveScheduler;
	delete iAsyncStopScheduler;
	}
	
/**
Old Test CaseID 		APPFWK-SAS-0002
New Test CaseID 		DEVSRVS-SSPLUGINS-SAS-0002
 */

void CTestSimAdaptStep::TestSimStateChange()
	{
	for (TInt opcode=0; opcode<=ETestGetLastSimEvent; opcode++)
    	{
    	TRequestStatus trs;
    	TTestSusSimAdaptionServerRequests rfsType = static_cast<TTestSusSimAdaptionServerRequests>(opcode);
    	iSimAdapt->CallCommandsOfAdaptation(trs,rfsType);
  		//this callback will not run until the test is finished because it has priority idle
 		iAsyncStopScheduler->CallBack();
   		iActiveScheduler->Start();
    	User::WaitForRequest(trs);
    	iAdptTransitionCompleted = EFalse;
       	TInt res = trs.Int();
    	TEST(KErrNone == res);
    	switch(rfsType)
		{
		case ETestGetSimOwned:	
			INFO_PRINTF3(_L("TestGetSimOwned completed with %d error, Expected %d"),res,KErrNone);
		break;
		case ETestGetLastSimEvent:
			INFO_PRINTF3(_L("TestGetLastSimEvent completed with %d error, Expected %d"),res,KErrNone);
		break;
		case ETestNotifySimCancel:	
			INFO_PRINTF3(_L("TestNotifySimCancel completed with %d error, Expected %d"),res,KErrNone);
		break;
		default:
		break;
		}
		}
	}

/** */
TVerdict CTestSimAdaptStep::doTestStepL()
	{
	iSimAdapt = CSimAdaptPlugin::NewL(*this);	

	__UHEAP_MARK;
	INFO_PRINTF1(_L("TestSimAdaptStep started"));
	TRAPD(err, TestSimStateChange());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("TestSimAdaptStep completed with %d error "),err);	
	__UHEAP_MARKEND;

	return TestStepResult();
	}
