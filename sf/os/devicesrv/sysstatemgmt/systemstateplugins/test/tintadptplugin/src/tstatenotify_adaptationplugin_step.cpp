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
  
#include "tstatenotify_adaptationplugin_step.h"
#include "ssmdebug.h"

/** */
CStateNotifyAdaptPlugin* CStateNotifyAdaptPlugin::NewL() 
	{
	CStateNotifyAdaptPlugin* self = new(ELeave) CStateNotifyAdaptPlugin();
	CleanupStack::PushL( self );
	self->Construct();
	CleanupStack::Pop();
	return self;
	}

/** */	
void CStateNotifyAdaptPlugin::CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusStateNotifyAdaptionServerRequests aRequest)
	{
	aTrs = KRequestPending;
	iStateNotifyStatus = &aTrs;
	iEventTypePckg() = ESsmFatalCoopSysError;
	
	switch(aRequest)
		{	
        case ETestNotifyCoopSysEvent:
			iStateNotifyAdaptation.GetLastCoopSysEvent(iEventTypePckg); 
			iStateNotifyAdaptation.NotifyCoopSysEvent(iEventTypePckg,iStatus);
			SetActive();
			break;	
        case ETestNotifyCoopSysCancel:
        	iStateNotifyAdaptation.NotifyCoopSysEvent(iEventTypePckg,iStatus);
			iStateNotifyAdaptation.NotifyCancel();
			SetActive(); 
			break;
			
		default:
			User::Panic(KTCTestStateNotifyAdaptStep, KErrArgument);			  			
					
		}
	User::After(KTestStateNotifyStepDelay);	
	}
	
/** */	
CStateNotifyAdaptPlugin::CStateNotifyAdaptPlugin()
	:CActive(EPriorityStandard)		
	{
	CActiveScheduler::Add(this);
	}

/** */
CStateNotifyAdaptPlugin::~CStateNotifyAdaptPlugin()
	{
	iStateNotifyAdaptation.Close();
	}

/** */
void CStateNotifyAdaptPlugin::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	if (iStateNotifyStatus  && (iStateNotifyStatus->Int() == KRequestPending))
		{
		User::RequestComplete(iStateNotifyStatus, iStatus.Int());
		}
	iStateNotifyStatus = NULL;
	CActiveScheduler::Stop();
	}

/** */
void CStateNotifyAdaptPlugin::DoCancel()
	{
	}

/** */
TInt CStateNotifyAdaptPlugin::RunError(TInt aError)
	{
	if (iStateNotifyStatus && (iStateNotifyStatus->Int() == KRequestPending))
		{
		User::RequestComplete(iStateNotifyStatus, aError);
		}
	iStateNotifyStatus = NULL;

	CActiveScheduler::Stop();
	
	return KErrNone;
	}

/** */
void CStateNotifyAdaptPlugin::Construct()
	{		
	iStateNotifyAdaptation.Connect();	
	}

CTestStateNotifyAdaptStep::CTestStateNotifyAdaptStep ()
	{
	SetTestStepName(KTCTestStateNotifyAdaptStep);
	}

CTestStateNotifyAdaptStep::~CTestStateNotifyAdaptStep ()
	{
	delete iStateNotifyAdapt;
	delete iActiveScheduler;
	}

/**
Old Test CaseID 		APPFWK-SAS-0001
New Test CaseID 		DEVSRVS-SSPLUGINS-SAS-0006
 */

void CTestStateNotifyAdaptStep::TestRequestSysStateNotifyChange()
	{	
	for (TInt opcode=0; opcode<=ETestNotifyCoopSysCancel; opcode++)
    	{
    	INFO_PRINTF2(_L("Test %d started"),opcode);
    	TRequestStatus trs;
    	TTestSusStateNotifyAdaptionServerRequests rfsType = static_cast<TTestSusStateNotifyAdaptionServerRequests>(opcode);
    	iStateNotifyAdapt->CallCommandsOfAdaptation(trs,rfsType);
    	INFO_PRINTF1(_L("Starting active scheduler"));
   		iActiveScheduler->Start();
   		INFO_PRINTF1(_L("Active scheduler returned"));
    	User::WaitForRequest(trs);
    	TInt res = trs.Int();
    	
    	switch(rfsType)
			{
        	case ETestNotifyCoopSysEvent:
        		TEST(KErrNone == res);
				INFO_PRINTF3(_L("Test NotifyCoopSysEventStep completed with %d error , Expected %d "),res,KErrNone);	
			break;	
        	case ETestNotifyCoopSysCancel:
        		TEST(KErrCancel == res);
				INFO_PRINTF3(_L("Test NotifyCoopSysCancel completed with %d error , Expected %d "),res,KErrCancel);	
			break;
			default:
			break;
			}
    	}
	}

TVerdict CTestStateNotifyAdaptStep::doTestStepL()
	{
	iStateNotifyAdapt = CStateNotifyAdaptPlugin::NewL();	

	__UHEAP_MARK;
	INFO_PRINTF1(_L("TestStateNotifyAdaptStep started"));
	TRAPD(err, TestRequestSysStateNotifyChange());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("TestNotifyCoopSysEvent completed with %d error"),err);        
	__UHEAP_MARKEND;

	return TestStepResult();
	}

