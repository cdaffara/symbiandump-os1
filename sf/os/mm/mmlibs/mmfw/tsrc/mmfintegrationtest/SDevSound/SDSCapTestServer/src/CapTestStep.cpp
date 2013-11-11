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

#include "CapTestStep.h"

#ifdef Required
CTestStep1* CTestStep1::NewL()
	{
	CTestStep1* self = new (ELeave) CTestStep1;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CTestStep1::ConstructL()
	{
	iIsFirstPlayed = EFalse;
	}
	

void CTestStep1::StartProcessing(TRequestStatus& aStatus)
	{
	iStatus = &aStatus;

	iVerdict = EFail;
	
	iVerdictDS = DoTestStepPreambleL();
	iVerdictDS = DoPlaySimpleTone();
	}
		
		
TVerdict CTestStep1::EndProcessingAndReturnResult(TDes8& aMessage)
	{
	aMessage.Copy(_L("Done"));
	return EPass;
	}
	
CTestStep1::~CTestStep1()
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

TVerdict CTestStep1::DoPlaySimpleTone()
	{
//	INFO_PRINTF1(_L("Testing Simple Tone Playback"));
	TInt freq = 1000;
	TTimeIntervalMicroSeconds dur(10000000);

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetPriority(0);

	TestSetVolume(iMMFDevSound->MaxVolume());
	iExpectedValue = KErrUnderflow;
	TestPlayTone(freq, dur);
	return TestPlayTone(freq, dur);
	}

/**
 *
 * TestInitialize
 * @param aMode
 * @result TVerdict
 *
 */

TVerdict CTestStep1::TestInitialize(TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
//	INFO_PRINTF1(_L("Initializing DevSound"));

	ResetCallbacks();

	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	if (err)
		{
//		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		if (iCallbackError != iExpectedValue)
			{
//			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
//			ERR_PRINTF2 (_L("DevSound InitializeComplete was called %d times, expected 1"), iCallbackArray[EInitComplete]);
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestPlayTone
 * @param aFreq
 * @param aDur
 * @result TVerdict
 *
 */
TVerdict CTestStep1::TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur)
	{
	iCallbackError = KErrNone;
	//iExpectedValue = KErrUnderflow;

	ResetCallbacks();

//	INFO_PRINTF1(_L("Playing Simple Tone"));
	TRAPD(err, iMMFDevSound->PlayToneL(aFreq, aDur));
	if (err)
		{
//		WARN_PRINTF2 (_L("DevSound PlayToneL left with error = %d"), err);
		iCallbackError = err;
		return EFail;
		}
	else
		{
//		iAL->InitialiseActiveListener();
		// Start the active scheduler and catch the callback
 		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
//			ERR_PRINTF3 (_L("DevSound ToneFinished returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EToneFinished] != 1)
			{
//			ERR_PRINTF2 (_L("DevSound ToneFinished was called %d times, expected 1"), iCallbackArray[EToneFinished]);
			return EFail;
			}
		TInt tot = GetCallbackTotal();
		if (tot > 1)
			{
//			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), tot);
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
 * InitializeComplete
 * @param aError
 *
 */
void CTestStep1::InitializeComplete (TInt aError)
	{
//	INFO_PRINTF2(_L("DevSound called InitializeComplete with aError = %d"), aError);
	iCallbackArray[EInitComplete] ++;
	iCallbackError = aError;
	}

/**
 *
 * ToneFinished
 * @param aError
 *
 */
void CTestStep1::ToneFinished (TInt aError)
	{
//	INFO_PRINTF2(_L("DevSound called ToneFinished with aError = %d"), aError);
//	TRequestStatus* stat = &(iAL->iStatus);
//	User::RequestComplete(stat, aError);

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
#endif // Required

