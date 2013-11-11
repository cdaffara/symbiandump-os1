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

#include "CapTestStep0006.h"

CSecDevSndTS0006* CSecDevSndTS0006::NewL()
	{
	CSecDevSndTS0006* self = new (ELeave) CSecDevSndTS0006;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CSecDevSndTS0006::ConstructL()
	{
	iIsFirstPlayed = EFalse;
	}
	

void CSecDevSndTS0006::StartProcessing(TRequestStatus& aStatus)
	{
	iStatus = &aStatus;

	iVerdict = EFail;
	if( (iVerdict = DoTestStepPreambleL() ) == EPass )
		{
		iVerdict = DoPlayDualTone();
		}
	}
		
		
CSecDevSndTS0006::~CSecDevSndTS0006()
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

TVerdict CSecDevSndTS0006::DoPlayDualTone()
	{
	TInt freq1a = 1000;
	TInt freq1b = 500;
	TTimeIntervalMicroSeconds dur1(KShortTimeInterval);
	TInt freq2a = 2000;
	TInt freq2b = 750;
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
	TestPlayTone(freq1a, freq1b, dur1);
	iExpectedValue = KErrInUse;
	return TestPlayTone(freq2a, freq2b, dur2);
	}

/**
 *
 * TestPlayTone
 * @param aFreq
 * @param aDur
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0006::TestPlayTone(TInt aFreq1, TInt aFreq2, TTimeIntervalMicroSeconds aDur)
	{
	iCallbackError = KErrNone;

	ResetCallbacks();

	TRAPD(err, iMMFDevSound->PlayDualToneL(aFreq1, aFreq2, aDur));
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
void CSecDevSndTS0006::ToneFinished (TInt aError)
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
