// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file LbsDebug0Step.cpp
 @internalTechnology
*/
#include "LbsDebug0Step.h"
#include "Te_LbsDebugSuiteDefs.h"

CLbsDebug0Step::~CLbsDebug0Step()
/**
 * Destructor
 */
	{
	}

CLbsDebug0Step::CLbsDebug0Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KLbsDebug0Step);
	}

TVerdict CLbsDebug0Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsDebugSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


void CLbsDebug0Step::StartLbsTestStepL(TRequestStatus& aStatus)
	{
	ROomRequest::InitializeL();
	iOomRequest.OpenL();
	
	//subscribe to OOM bus directly
	iOomRequest.OomTestNotifiyUpdates(aStatus);	

	//subscribe to OOM bus through listener
	iOomListener = CLbsOomListener::NewL();
	iOomListener->StartGettingRequests();
		
	//set property = ETrue
	iOomRequest.OomTestUpdates(ETrue);
	}


TBool CLbsDebug0Step::DoLbsTestStepL(TRequestStatus& /*aStatus*/)
	{
	HBufC* buf = NULL;
	//HBufC* buf1 = NULL;
	//HBufC* buf2 = NULL;
	//HBufC* buf3 = NULL;
	TBool done;
	
	switch (iCounter)
		{
		case 0:
			TRAPD(leavecode1, buf = HBufC::NewL(8));
			if(leavecode1 != KErrNone)
				SetResult(EFail);
			else
				{
				delete buf;
				buf = NULL;
				SetResult(EPass);
				}
			
			iCounter++;	
			done = EFalse;
			break;
		
		case 1:
			TRAPD(leavecode2, buf = HBufC::NewL(8));
			if(leavecode2 != KErrNone)
				SetResult(EPass);
			else
				{
				delete buf;
				buf = NULL;
				SetResult(EFail);
				}
			
			iCounter++;
			done = EFalse;
			break;
 
		case 2:
			iOomRequest.OomTestUpdates(EFalse);
			iCounter++;	
			done = EFalse;
			break;

		case 3:
			TRAPD(leavecode3, buf = HBufC::NewL(8));
			if(leavecode3 != KErrNone)
				SetResult(EFail);
			else
				{
				delete buf;
				buf = NULL;
				SetResult(EPass);
				}
	
			iCounter++;
			done = EFalse;
			break;

		case 4:
			iOomRequest.OomTestUpdates(EFalse);
			iCounter++;	
			done = EFalse;
			break;

		case 5:
			done = ETrue;
			break;
		}
		
	return done;
	}
	

void CLbsDebug0Step::FinishLbsTestStep(TRequestStatus& /*aStatus*/)
	{
	ROomRequest::ShutDownL();
	iOomListener->Cancel();
	delete iOomListener;
	}



TVerdict CLbsDebug0Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult() == EPass)
		{
	    CTinyTestClass* tinyClass = CTinyTestClass::NewL(this);
	    CActiveScheduler::Start();

		//set property = EFalse
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}



TVerdict CLbsDebug0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsDebugSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
