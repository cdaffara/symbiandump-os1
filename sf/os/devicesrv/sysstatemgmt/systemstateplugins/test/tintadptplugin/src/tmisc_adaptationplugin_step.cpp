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
#include "tmisc_adaptationplugin_step.h"
#include <test/susadaptionclisrv.h>

//delay required for completing the outstanding request
const TInt KAsyncMessageSlots = -1; // Taken from ssmadaptationcli.cpp

/** */	
CMiscAdaptPlugin* CMiscAdaptPlugin::NewL(CTestMiscAdaptStep& aTestMiscAdaptStep) 
	{
	CMiscAdaptPlugin* self = new(ELeave) CMiscAdaptPlugin(aTestMiscAdaptStep);
	CleanupStack::PushL( self );
	self->Construct();
	CleanupStack::Pop();
	return self;
	}

/** */	
void CMiscAdaptPlugin::CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusMiscAdaptionServerRequests aRequest)
	{
	aTrs = KRequestPending;
	iMiscStatus = &aTrs;
	
	TInt response = 1;
	TInt mode = 1;
	TInt size = 1;
	TInt list = 1;
	TBool hiddenReset = 0;

	iPriority = EPrimaryLanguages;
	iState = 1;
		
	iPckgResponse() = response;
	iPckgMode() = mode;
	iPckgSize() = size;
	iPckgList() = list;
	iPckgHiddenReset() = hiddenReset;

	switch(aRequest)
		{
		case ETestSecurityStateChange:
			iMiscAdaptation.SecurityStateChange(iState, iPckgResponse, iStatus);
			SetActive();
			break;
		case ETestGetGlobalStartupMode:
			iMiscAdaptation.GetGlobalStartupMode(iPckgMode,iStatus);
			SetActive();
			break;
		case ETestPrepareSimLanguages:
			iMiscAdaptation.PrepareSimLanguages(iPriority,iPckgSize,iStatus);
			SetActive();
			break;
		case ETestGetSimLanguages:
			iMiscAdaptation.PrepareSimLanguages(iPriority,iPckgSize,iStatus);
			iGetSimLanguages = ETrue;
			SetActive();
			break;
		case ETestGetHiddenReset:
			iMiscAdaptation.GetHiddenReset(iPckgHiddenReset,iStatus);
			SetActive();
			break;
		default:
			User::Panic(KTCTestMiscAdaptStep, KErrArgument);
			break;				
		}
	}

/** */	
CMiscAdaptPlugin::CMiscAdaptPlugin(CTestMiscAdaptStep& aTestMiscAdaptStep)
	:CActive(EPriorityStandard),iTestMiscAdaptStep(aTestMiscAdaptStep),iGetSimLanguages(EFalse)
	{
	CActiveScheduler::Add(this);
	}

/** */	
CMiscAdaptPlugin::~CMiscAdaptPlugin()
	{
	Cancel();
	iMiscAdaptation.Close();
	}

/** */	
void CMiscAdaptPlugin::RunL()
	{
	iTestMiscAdaptStep.SetActivationCompleted();
	//If the event request is to get the sim languages, call the synchronous method SimLanguagesArrayL(iSimLanguageCount)
	if (iGetSimLanguages)
		{
		//Set the flag back to EFalse for any further call for getting SIM supported languages
		iGetSimLanguages = EFalse;
		if (KErrNotSupported == iStatus.Int())
			{
			//reset iPckgSize as the server would not have written any response in it and hence it will contain 0
			iPckgSize = 50;   //ASCII Value of integer 2
			//Extracting the count of the SIM supported languages from descriptor into an integer
			TInt simLanguageCount;
			TLex8 languageCountLex(iPckgSize);
		    languageCountLex.Val(simLanguageCount);
			iMiscAdaptation.SimLanguagesArrayL(simLanguageCount);
			}
		}
	User::LeaveIfError(iStatus.Int());
	if (iMiscStatus  && (iMiscStatus->Int() == KRequestPending))
		{
		User::RequestComplete(iMiscStatus, iStatus.Int());
		}
	}

/** */	
void CMiscAdaptPlugin::DoCancel()
	{
	}

/** */	
TInt CMiscAdaptPlugin::RunError(TInt aError)
	{
	if (iMiscStatus)
		{
		User::RequestComplete(iMiscStatus, aError);
		}

	return KErrNone;
	}

/** */	
void CMiscAdaptPlugin::Construct()
	{		
	iMiscAdaptation.Connect();
	}

/****************************************
*	RTestSsmMiscAdaptation functions    *
*****************************************/

EXPORT_C TInt RTestSsmMiscAdaptation::Connect()
	{
	const TInt err = Connect(KAsyncMessageSlots);
	return err;
	}

TInt RTestSsmMiscAdaptation::Connect(TInt aAsyncMessageSlotCount)
	{
	TInt err = RSsmAdaptationBase::Connect(aAsyncMessageSlotCount);
	if(err == KErrNone)
		{
		err = HeapMark();
		}
	return err;
	}

void RTestSsmMiscAdaptation::Close()
	{
	TInt ret = CleanupAdaptations();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMMISC"), ret));
	ret = HeapMarkEnd();
	__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("TESTSSMMISC"), ret));
	RSsmAdaptationBase::Close();
	}

TInt RTestSsmMiscAdaptation::HeapMark()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeap);
		}
	return ret;
	}

TInt RTestSsmMiscAdaptation::HeapMarkEnd()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeapEnd);
		}
	return ret;
	}

TInt RTestSsmMiscAdaptation::CleanupAdaptations()
	{
	TInt ret = KErrNone;
    // Since actual plugins are being used, they cannot be unloaded.
    // Hence return without doing anything. There will be HeapMarkEnd to check that
    // memory is not leaked.
	return ret;
	}

/** */	
CTestMiscAdaptStep::CTestMiscAdaptStep ()
	{
	SetTestStepName(KTCTestMiscAdaptStep);
	}

/** */	
CTestMiscAdaptStep::~CTestMiscAdaptStep ()
	{
	delete iMiscAdapt;
	delete iActiveScheduler;
	delete iAsyncStopScheduler;
	}
	
/**
Old Test CaseID 		APPFWK-SAS-0003
New Test CaseID 		DEVSRVS-SSPLUGINS-SAS-0003
 */

void CTestMiscAdaptStep::TestMIscStateChange()
	{	
	for (TInt opcode=0; opcode<ETestMiscCancel; opcode++)
    	{
    	TRequestStatus trs;
    	TTestSusMiscAdaptionServerRequests rfsType = static_cast<TTestSusMiscAdaptionServerRequests>(opcode);
    	iMiscAdapt->CallCommandsOfAdaptation(trs,rfsType);
  		//this callback will not run until the test is finished because it has priority idle
 		iAsyncStopScheduler->CallBack();
   		iActiveScheduler->Start();
    	User::WaitForRequest(trs);
    	iAdptTransitionCompleted = EFalse;
    	TInt res = trs.Int();
    	switch(rfsType)
			{
			case ETestSecurityStateChange:
				TEST(KErrNone == res);
				INFO_PRINTF3(_L("TestSecurityStateChange completed with %d error, Expected %d "),res,KErrNone);
				break;
			case ETestGetGlobalStartupMode:
				TEST(KErrNone == res);
				INFO_PRINTF3(_L("TestGetGlobalStartupMode completed with %d error, Expected %d "),res,KErrNone);
				break;
			case ETestPrepareSimLanguages:
				TEST(KErrNotSupported == res);	
				INFO_PRINTF3(_L("TestPrepareSimLanguages completed with %d error, Expected %d "),res,KErrNotSupported);
				break;
			case ETestGetSimLanguages:
				TEST(KErrNotSupported == res);	
				INFO_PRINTF3(_L("TestGetSimLanguages completed with %d error, Expected %d "),res,KErrNotSupported);
				break;
			case ETestGetHiddenReset:
				TEST(KErrNotSupported == res);	
				INFO_PRINTF3(_L("TestGetHiddenReset completed with %d error, Expected %d "), res, KErrNotSupported);
				break;
			default:
				break;
			}	
		}
	}	

/** */	
TVerdict CTestMiscAdaptStep::doTestStepL()
	{
	iMiscAdapt = CMiscAdaptPlugin::NewL(*this);	

	__UHEAP_MARK;
	INFO_PRINTF1(_L("TestMiscAdaptStep started"));	
	TRAPD(err, TestMIscStateChange());
	TEST(err == KErrNone);
	INFO_PRINTF3(_L("TestMiscAdaptStep completed with %d error , Expected %d "),err,KErrNone);	
	__UHEAP_MARKEND;

	return TestStepResult();
	}



