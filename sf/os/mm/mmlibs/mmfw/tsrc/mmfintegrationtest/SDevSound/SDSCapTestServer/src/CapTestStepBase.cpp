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

CDevSoundTestStepBase* CDevSoundTestStepBase::NewL()
	{
	CDevSoundTestStepBase* self = new (ELeave) CDevSoundTestStepBase;
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();
	return self;
	}
	
void CDevSoundTestStepBase::ConstructL()
	{
	}
	
CDevSoundTestStepBase::CDevSoundTestStepBase()
	{
	ResetCallbacks();
	}

CDevSoundTestStepBase::~CDevSoundTestStepBase()
	{
	delete iMMFDevSound;
	}

void CDevSoundTestStepBase::StartProcessing(TRequestStatus& /*aStatus*/)
	{
	}

TVerdict CDevSoundTestStepBase::EndProcessingAndReturnResult(TDes8& aMessage)
	{
	aMessage.Copy(_L("Done"));

	return iVerdict;
	}
	
/******************************************************************************
 *
 * DevSound methods
 *
 *****************************************************************************/
 
 TVerdict CDevSoundTestStepBase::ConstructDevSound()
	{
	TRAPD(err, iMMFDevSound = CMMFDevSound::NewL());
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		return EPass;
		}
	}

 TVerdict CDevSoundTestStepBase::DoTestStepPreambleL()
	{
	return( ConstructDevSound() );
	}

/**
 *
 * T`estInitialize
 * @param aMode
 * @result TVerdict
 *
 */

TVerdict CDevSoundTestStepBase::TestInitialize(TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;

	ResetCallbacks();
	
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestSetVolume
 * @param aVol
 *
 */
void CDevSoundTestStepBase::TestSetVolume(TInt aVol)
	{
	iMMFDevSound->SetVolume(aVol);
	}

/**
 *
 * TestSetPriority
 * @param aPriority
 *
 */
void CDevSoundTestStepBase::TestSetPriority(TInt aPriority)
	{
	TMMFPrioritySettings	prioritySettings;

	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = aPriority;

	iMMFDevSound->SetPrioritySettings( prioritySettings );
	}

/**
 *
 * TestSetGain
 * @param aGain
 *
 */
void CDevSoundTestStepBase::TestSetGain(TInt aGain)
	{
	iMMFDevSound->SetGain(aGain);
	}

/**
 *
 * TestPlayTone
 * @param aFreq
 * @param aDur
 * @result TVerdict
 *
 */
TVerdict CDevSoundTestStepBase::TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur)
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

/**
 *
 * ResetCallbacks
 *
 */
void CDevSoundTestStepBase::ResetCallbacks()
	{
	//Initialize callback array
	iCallbackArray.Reset();
	}

/**
 *
 * GetCallbackTotal
 * @result TInt
 *
 */
TInt CDevSoundTestStepBase::GetCallbackTotal()
	{
	TInt total = 0;
	for (TInt i = EInitComplete; i < EDeviceMsg; i++)
		{
		total += iCallbackArray[i];
		}
	return total;
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
void CDevSoundTestStepBase::InitializeComplete (TInt aError)
	{
	iCallbackArray[EInitComplete] ++;
	iCallbackError = aError;
	CActiveScheduler::Stop();
	}

/**
 *
 * ToneFinished
 * @param aError
 *
 */
void CDevSoundTestStepBase::ToneFinished (TInt aError)
	{
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	}

/**
 *
 * BufferToBeFilled
 * @param aBuffer
 *
 */
void CDevSoundTestStepBase::BufferToBeFilled (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	if (aBuffer != NULL) 
		{
		iCallbackError = KErrNone;
		}
	else 
		{
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToFill] ++;
	CActiveScheduler::Stop();
	}

/**
 *
 * PlayError
 * @param aError
 *
 */
void CDevSoundTestStepBase::PlayError (TInt aError)
	{
	iCallbackError = aError;
	iCallbackArray[EPlayError] ++;
	CActiveScheduler::Stop();
	}

/**
 * 
 * BufferToBeEmptied
 * @param aBuffer
 *
 */
void CDevSoundTestStepBase::BufferToBeEmptied (CMMFBuffer* /*aBuffer*/)
	{
	iCallbackArray[EBuffToEmpty] ++;
	}

/**
 *
 * RecordError
 * @param aError
 *
 */
void CDevSoundTestStepBase::RecordError (TInt aError)
	{
	iCallbackError = aError;
	iCallbackArray[ERecError] ++;
	}

/**
 *
 * ConvertError
 *
 */
void CDevSoundTestStepBase::ConvertError (TInt /*aError*/)
	{}

/**
 *
 * DeviceMessage
 *
 */
void CDevSoundTestStepBase::DeviceMessage (TUid, const TDesC8& /*aMsg*/)
	{}
