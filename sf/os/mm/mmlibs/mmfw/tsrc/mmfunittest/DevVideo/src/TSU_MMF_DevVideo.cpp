// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed to test DevVideo.
// 
//

/**
 @file TSU_MMF_DEVVIDEO.cpp
*/
#include "TSU_MMF_DevVideo.h"




/**
 * Timeout function
 */
void CTestDevVideoStep::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
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
TBool CTestDevVideoStep::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
	{
	// save unnecessary conditions
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
 * Test Preamble routines.
 *
 * Creates our own Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Preamble complete.
 */
TVerdict CTestDevVideoStep::DoTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);

	return EPass;
	}

/**
 * Test Postamble routines.
 *
 * Destroys our Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Postamble complete.
 */
TVerdict CTestDevVideoStep::DoTestStepPostambleL()
	{
	delete iActiveScheduler;
	iActiveScheduler = NULL;

	return EPass;
	}

/**
 * CTestDevVideoStep Implementation
 */
CTestDevVideoStep::CTestDevVideoStep()
	:iActiveScheduler( NULL )
	{}

CTestDevVideoStep::~CTestDevVideoStep()
	{
	}


