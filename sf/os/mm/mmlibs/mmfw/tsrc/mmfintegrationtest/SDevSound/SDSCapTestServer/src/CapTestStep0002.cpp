// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CapTestStep0002.h"

CSecDevSndTS0002* CSecDevSndTS0002::NewL()
	{
	CSecDevSndTS0002* self = new (ELeave) CSecDevSndTS0002;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CSecDevSndTS0002::ConstructL()
	{
	iIsFirstPlayed = EFalse;
	}
	

void CSecDevSndTS0002::StartProcessing(TRequestStatus& aStatus)
	{
	iStatus = &aStatus;

	iVerdict = EFail;
	if( (iVerdict = DoTestStepPreambleL() ) == EPass )
		{
		iVerdict = DoPlaySimpleTone();
		}
	}

CSecDevSndTS0002::~CSecDevSndTS0002()
	{
	}

/******************************************************************************
 *
 * DevSound methods
 *
 *****************************************************************************/
 
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */

const TInt	KShortTimeInterval = 1000000;
const TInt	KLongTimeInterval = 10000000;

TVerdict CSecDevSndTS0002::DoPlaySimpleTone()
	{
	TInt freq1 = 500;	// arbitrary freq
	TTimeIntervalMicroSeconds dur1(KShortTimeInterval);
	TInt freq2 = 1000;	// arbitrary freq (different from #1)
	TTimeIntervalMicroSeconds dur2(KLongTimeInterval);

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetPriority(KDevSoundPriorityHigh);
	TestSetVolume(iMMFDevSound->MaxVolume());

	iExpectedValue = KErrUnderflow;
	TestPlayTone(freq1, dur1);
	iExpectedValue = KErrInUse;
	return TestPlayTone(freq2, dur2);
	}

/**
 *
 * TestPlayTone
 * @param aFreq
 * @param aDur
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0002::TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur)
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
void CSecDevSndTS0002::ToneFinished (TInt aError)
	{
	if( !iIsFirstPlayed )
		{
		// after first tone played inform client so we can be interrupted during second play
		iIsFirstPlayed = ETrue;
		User::RequestComplete(iStatus, aError);
		}
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	CActiveScheduler::Stop();
	}
