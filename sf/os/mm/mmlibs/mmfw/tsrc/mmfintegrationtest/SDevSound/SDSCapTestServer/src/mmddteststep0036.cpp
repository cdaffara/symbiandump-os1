// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mmddteststep0036.h"

CSecDevSndTS0036* CSecDevSndTS0036::NewL(TThreadId aClientTid)
	{
	CSecDevSndTS0036* self = new (ELeave) CSecDevSndTS0036(aClientTid);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSecDevSndTS0036::CSecDevSndTS0036(TThreadId aClientTid)
	{
	iClientTid = aClientTid;
	}

void CSecDevSndTS0036::ConstructL()
	{
	}
	
void CSecDevSndTS0036::StartProcessing(TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
	
	User::After(3000000);	// allow client side devsound to play for 3sec
	if( (iVerdict = DoTestStepPreambleL() ) == EPass )
		{
		iVerdict = DoPlaySimpleTone();
		}
	}

CSecDevSndTS0036::~CSecDevSndTS0036()
	{
	}

/******************************************************************************
 *
 * DevSound methods
 *
 *****************************************************************************/
 
TVerdict CSecDevSndTS0036::DoPlaySimpleTone()
	{
	TInt freq = 100;	// arbitrary freq
	TTimeIntervalMicroSeconds dur(2000000);	//play for 2sec
	TVerdict verdict;
	
	TInt err = iMMFDevSound->SetClientThreadInfo(iClientTid);
	if (err != KErrNone)
		{
		verdict = EInconclusive;
		}
	else
		{
		verdict = TestInitialize(EMMFStateTonePlaying);
		if (verdict != EPass)
			{
			verdict = EInconclusive;
			}
		else
			{
			TestSetPriority(KDevSoundPriorityLow);
			TestSetVolume(iMMFDevSound->MaxVolume());
			iExpectedValue = KErrUnderflow;
			verdict = TestPlayTone(freq, dur);
			}
		}
	if (verdict == EInconclusive)
		{
		User::RequestComplete(iStatus, KErrGeneral);
		}
	return verdict;	
	}

/**
 *
 * TestPlayTone
 * @param aFreq
 * @param aDur
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0036::TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur)
	{
	iCallbackError = KErrNone;

	ResetCallbacks();

	TRAPD(err, iMMFDevSound->PlayToneL(aFreq, aDur));
	if (err)
		{
		iCallbackError = err;
		return EFail;
		}
	else
		{
		// Start the active scheduler and catch the callback
 		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			return EFail;
			}
		if (iCallbackArray[EToneFinished] != 1)
			{
			return EFail;
			}
		TInt tot = GetCallbackTotal();
		if (tot > 1)
			{
			return EFail;
			}
		}
	return EPass;
	}

/******************************************************************************
 *
 * DevSound mixin methods
 *
 *****************************************************************************/
 
/**
 *
 * ToneFinished
 * @param aError
 *
 */
void CSecDevSndTS0036::ToneFinished (TInt aError)
	{
	User::RequestComplete(iStatus, aError);
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	CActiveScheduler::Stop();
	}
