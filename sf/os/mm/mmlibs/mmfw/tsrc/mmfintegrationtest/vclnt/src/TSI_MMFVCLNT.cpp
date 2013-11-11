
// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed the test of the MMF_VCLNT.
// 
//

/**
 @file TSI_MMFVCLNT.cpp
*/

#include "TSI_MMFVCLNT.h"


TInt CVideoCallbackHandler::ID()
	{
	return iID;
	}

void CVideoCallbackHandler::MvpuoOpenComplete(TInt aError)
	{
	iMchObserver->MchoComplete(ID(),aError);
	}

void CVideoCallbackHandler::MvpuoFrameReady(CFbsBitmap& /*aFrame*/)
	{
	iMchObserver->MchoComplete(ID(),0); 
	}

void CVideoCallbackHandler::MvpuoPlayComplete(TInt aError)
	{
	iMchObserver->MchoComplete(ID(),aError); 
	}

CVideoRecorderCallbackHandler::CVideoRecorderCallbackHandler(const TInt aID, MCallbackHandlerObserver* aMchObserver)
	: iMchObserver(aMchObserver)
	, iID(aID) 
	{}

TInt CVideoRecorderCallbackHandler::ID() 
	{ 
	return iID; 
	}

void CVideoRecorderCallbackHandler::MvruoOpenComplete(TInt aError)
	{
	iMchObserver->MchoComplete(ID(),aError); 
	}

void CVideoRecorderCallbackHandler::MvruoRecordComplete(TInt aError)
	{
	iMchObserver->MchoComplete(ID(),aError); 
	}

/**
 * Timeout function
 */
void CTestMmfVclntStep::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
	{
	TRequestStatus timerStatus;
	RTimer timer ;
	timer.CreateLocal() ;
	timer.After(timerStatus,aNumberOfMicroSeconds);

	User::WaitForRequest(aStatus, timerStatus);
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		INFO_PRINTF1(_L("Time is over!!!")) ;
		}
	timer.Close() ;
	}

/**
 * Time comparison utility function
 *
 * @param	"const TUint aActual"
 *			The actual timer value produced
 * @param	"const TUint aExpected"
 *			Expected timer value
 * @param	"const TUint aDeviation"
 *			Allowed deviation of the expected value
 *			from the actual value.
 * @return	"TBool"
 *			Did actual timed value fall within deviation limits
 */ 
TBool CTestMmfVclntStep::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
	{
	// save unnessary conditions
	if(aActual == aExpected)
		return ETrue;	

	// Prevent unsigned wrapping errors 
	TUint difference;
	if(aActual > aExpected)
		difference = aActual - aExpected;
	else
		difference = aExpected - aActual;

	// comapare
	if(difference < aDeviation)
		return ETrue;
	return EFalse;
	}

/**
 * Test Preample routines.
 *
 * Creates our own Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Preamble complete.
 */
TVerdict CTestMmfVclntStep::DoTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);

	return EPass;
	}

/**
 * Test Postample routines.
 *
 * Destroys our Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Postamble complete.
 */
TVerdict CTestMmfVclntStep::DoTestStepPostambleL()
	{
	delete iActiveScheduler;
	iActiveScheduler = NULL;

	delete iScreen;
	delete iWindow;
	iWs.Close();

	return EPass;
	}

/**
 * CTestMMFVCLNTStep Implementation
 */
CTestMmfVclntStep::CTestMmfVclntStep()
	:iActiveScheduler( NULL )
	{}

CTestMmfVclntStep::~CTestMmfVclntStep()
	{
	}
	
void CTestMmfVclntStep::InitWservL()
	{
	TInt err = iWs.Connect();
	if (err != KErrNone)
		{
		// Access violation if ws is null
		INFO_PRINTF1(_L("Cannot test, no window server available"));
		User::Leave(err);
		}

	iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen->Construct()); // and complete its construction

	RWindowGroup rootWindow = RWindowGroup(iWs);
	User::LeaveIfError(rootWindow.Construct((TUint32)this, ETrue));

	iWindow = new(ELeave) RWindow(iWs);
	User::LeaveIfError(((RWindow*)iWindow)->Construct(rootWindow,((TUint32)(this)) + 1));
	iWindow->SetExtent(TPoint(0,0), TSize(100,100));
	iWindow->SetVisible(ETrue);
	iWindow->Activate();
	iWs.Flush();
	}
