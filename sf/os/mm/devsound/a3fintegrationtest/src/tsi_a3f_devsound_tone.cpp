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
// Implementation of Integraton test step classes for A3F DevSound Tone test cases.
// 
//

#include "tsi_a3f_devsound_tone.h"

/*
 *
 * RTestStepA3FDevSoundToneBase - Test step constructor
 *
 */
RTestStepA3FDevSoundToneBase::RTestStepA3FDevSoundToneBase(const TDesC& aTestName, const TDesC& aSectName)
:RAsyncTestStep(),
 iMMFDevSound(NULL),
 iSectName(aSectName),
 iTimer(NULL),
 iCallBack(NULL, NULL),
 iState(EStateToneStart)
	{
	// From RTestStep
	iTestStepName = aTestName; // store the name of the test case

	}

/*
 *
 * RTestStepA3FDevSoundToneBase - Test step constructor
 *
 */
RTestStepA3FDevSoundToneBase::RTestStepA3FDevSoundToneBase(const TDesC& aTestName)
:RAsyncTestStep(),
 iMMFDevSound(NULL),
 iSectName(KNullDesC),
 iTimer(NULL),
 iCallBack(NULL, NULL),
 iState(EStateToneStart)
	{
	// From RTestStep
	iTestStepName = aTestName; // store the name of the test case

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RTestStepA3FDevSoundToneBase::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	
	DoKickoffTestL();

	Fsm(EEventToneStart,KErrNone); // call to start the DevSound finite state machine for tone playing

	}

/*
 *
 * CloseTest
 *
 */
void RTestStepA3FDevSoundToneBase::CloseTest()
	{
	if (iMMFDevSound)
		{
		INFO_PRINTF1(_L("--- Deleting DevSound object..."));
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	
	if (iTimer)
		{
		delete iTimer;
		iTimer = NULL;
		}
		
	}

/*
 *
 * InitializeComplete
 *
 */
void RTestStepA3FDevSoundToneBase::InitializeComplete(TInt aError)
	{
	Fsm(EEventInitCompleteTone,aError);

	}

/*
 *
 * ToneFinished
 *
 */
void RTestStepA3FDevSoundToneBase::ToneFinished(TInt aError)
	{
	if ((aError == KErrUnderflow))
		{
		INFO_PRINTF2(_L("DevSound called ToneFinished with error = %d as expected"), aError);
		StopTest(aError,EPass);
		}
	else
		{
		INFO_PRINTF2(_L("DevSound called ToneFinished with error = %d that was NOT expected"), aError);
		StopTest(aError);
		}

	}

/*
 *
 * BufferToBeFilled
 *
 */
void RTestStepA3FDevSoundToneBase::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundToneBase"),EInvalidCallbackCall));

	}

/*
 *
 * PlayError
 *
 */
void RTestStepA3FDevSoundToneBase::PlayError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundToneBase"),EInvalidCallbackCall));

	}

/*
 *
 * RecordError
 *
 */

void RTestStepA3FDevSoundToneBase::RecordError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundToneBase"),EInvalidCallbackCall));

	}
/*
 *
 * BufferToBeEmptied
 *
 */

void RTestStepA3FDevSoundToneBase::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundToneBase"),EInvalidCallbackCall));

	}

/*
 *
 * ConvertError
 *
 */
void RTestStepA3FDevSoundToneBase::ConvertError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundToneBase"),EInvalidCallbackCall));

	}

/*
 *
 * DeviceMessage
 *
 */
void RTestStepA3FDevSoundToneBase::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundToneBase"),EInvalidCallbackCall));

	}
	
/*
 *
 * TimerCallback
 *
 */
TInt RTestStepA3FDevSoundToneBase::TimerCallback(TAny* aPtr)
	{
	static_cast<RTestStepA3FDevSoundToneBase*>(aPtr)->DoTimerCallback();
	return KErrNone;

	}

/*
 *
 * DoTimerCallback
 *
 */
void RTestStepA3FDevSoundToneBase::DoTimerCallback()
	{
	// The derived classes may provide the implementation if needed
	}

/*
 *
 * ConvertDesToTMMFSampleRate
 *
 */
TUint RTestStepA3FDevSoundToneBase::ConvertDesToTMMFSampleRate(const TDesC& aSampleRateString)
	{
	TUint rate = 0;

	STR_SWITCH(aSampleRateString)

	STR_CASE(_L("EMMFSampleRate8000Hz"))
		{
		rate = EMMFSampleRate8000Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate11025Hz"))
		{
		rate = EMMFSampleRate11025Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate12000Hz"))
		{
		rate = EMMFSampleRate12000Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate16000Hz"))
		{
		rate = EMMFSampleRate16000Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate22050Hz"))
		{
		rate = EMMFSampleRate22050Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate24000Hz"))
		{
		rate = EMMFSampleRate24000Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate32000Hz"))
		{
		rate = EMMFSampleRate32000Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate44100Hz"))
		{
		rate = EMMFSampleRate44100Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate48000Hz"))
		{
		rate = EMMFSampleRate48000Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate64000Hz"))
		{
		rate = EMMFSampleRate64000Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate88200Hz"))
		{
		rate = EMMFSampleRate88200Hz;
		break;
		}
	STR_CASE(_L("EMMFSampleRate96000Hz"))
		{
		rate = EMMFSampleRate96000Hz;
		break;
		}
	STR_CASE(_L("InvalidRate"))
		{
		rate = 0x90000000;
		break;
		}

	DEFAULT_CASE()
		{
		ERR_PRINTF1(_L("Attempt to convert invalid sample rate"));
		StopTest(KErrGeneral);
		}

	STR_SWITCH_END()

	INFO_PRINTF2(_L("Sample Rate = %S"), &aSampleRateString);
	return rate;

	}

//
// RStepA3FDevSoundDTMFTonePlay
//

/*
 *
 * RStepA3FDevSoundDTMFTonePlay - Test step constructor
 *
 */
RStepA3FDevSoundDTMFTonePlay::RStepA3FDevSoundDTMFTonePlay(const TDesC& aTestName)
:RTestStepA3FDevSoundToneBase(aTestName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundDTMFTonePlay* RStepA3FDevSoundDTMFTonePlay::NewL(const TDesC& aTestName)
	{
	RStepA3FDevSoundDTMFTonePlay* self = new (ELeave) RStepA3FDevSoundDTMFTonePlay(aTestName);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundDTMFTonePlay::DoKickoffTestL()
	{
	}

/*
 *
 * Fsm
 * - Executes DTMF tone playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundDTMFTonePlay::Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateToneStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundDTMFTonePlay"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventToneStart)
				{
				// Initializing DevSound object for tone play
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateToneInitializing"));
				iState = EStateToneInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventToneStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateToneInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteTone && aError == KErrNone)
				{
				TPtrC dtmfString = (_L("0123456789,abcdef,*#"));
				TTimeIntervalMicroSeconds32 on(KMicroSecsInOneSec), off(KMicroSecsInOneSec), pause(0);
				// Define the duration of tone on, tone off and tone pause to be used during the DTMF tone playback operation
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetDTMFLengths"));
				iMMFDevSound->SetDTMFLengths(on,off,pause);
				// Initialize the audio device and start playing the DTMF string
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayDTMFStringL"));
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(dtmfString));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateTonePlaying"));
				iState = EStateTonePlaying;

				}
			else if (aDevSoundEvent == EEventInitCompleteTone && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteTone not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundNormalAndDualTonePlay
//

/*
 *
 * RStepA3FDevSoundNormalAndDualTonePlay - Test step constructor
 *
 */
RStepA3FDevSoundNormalAndDualTonePlay::RStepA3FDevSoundNormalAndDualTonePlay(const TDesC& aTestName, const TDesC& aSectName)
:RTestStepA3FDevSoundToneBase(aTestName, aSectName),
 iRepeat(0),
 iDualTone(EFalse)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundNormalAndDualTonePlay* RStepA3FDevSoundNormalAndDualTonePlay::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RStepA3FDevSoundNormalAndDualTonePlay* self = new (ELeave) RStepA3FDevSoundNormalAndDualTonePlay(aTestName, aSectName);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundNormalAndDualTonePlay::DoKickoffTestL()
	{
	//Getting the number of times the audio is to be repeated during the tone playback operation
	if (GetIntFromConfig(iSectName,_L("Repeat"),iRepeat))
		{
		INFO_PRINTF2(_L("Repeat count taken from config file: %d"),iRepeat);
		}
	else
		{
		INFO_PRINTF2(_L("Default repeat count used: %d"), iRepeat);
		}

	// Getting the boolean flag value indicating whether dual tone playback should occur instead of the normal tone playback
	if (GetBoolFromConfig(iSectName,_L("DualTone"),iDualTone))
		{
		INFO_PRINTF2(_L("Boolean value indicating whether to use dual tone playback taken from config file: %d"),iDualTone);
		}
	else
		{
		INFO_PRINTF2(_L("Default boolean value indicating whether to use dual tone playback used: %d"), iDualTone);
		}

	}

/*
 *
 * Fsm
 * - Executes either the normal or dual tone playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundNormalAndDualTonePlay::Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateToneStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundNormalAndDualTonePlay"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventToneStart)
				{
				// Initializing DevSound object for tone play
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateToneInitializing"));
				iState = EStateToneInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventToneStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateToneInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteTone && aError == KErrNone)
				{
				TTimeIntervalMicroSeconds repeatTrailingSilence(KMicroSecsInOneSec), duration(KMicroSecsInOneSec);
				// Define the number of times the audio is to be repeated during the tone playback operation
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetToneRepeats"));
				iMMFDevSound->SetToneRepeats(iRepeat, repeatTrailingSilence);
				if (iDualTone)
					{
					// Initialize audio device and start playing a dual tone
					INFO_PRINTF1(_L("Calling CMMFDevSound::PlayDualToneL"));
					TRAPD(err, iMMFDevSound->PlayDualToneL(KFrequency1, KFrequency2, duration));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with error = %d"), err);
						StopTest(err);
						break;
						}
					}
				else
					{
					// Initialize the audio device and start playing a tone
					INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneL"));
					TRAPD(err, iMMFDevSound->PlayToneL(KFrequency1, duration));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error = %d"), err);
						StopTest(err);
						break;
						}
					}
				INFO_PRINTF1(_L("State: EStateTonePlaying"));
				iState = EStateTonePlaying;

				}
			else if (aDevSoundEvent == EEventInitCompleteTone && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteTone not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundInitializeWhileTonePlaying
//

/*
 *
 * RStepA3FDevSoundInitializeWhileTonePlaying - Test step constructor
 *
 */
RStepA3FDevSoundInitializeWhileTonePlaying::RStepA3FDevSoundInitializeWhileTonePlaying(const TDesC& aTestName)
:RTestStepA3FDevSoundToneBase(aTestName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundInitializeWhileTonePlaying* RStepA3FDevSoundInitializeWhileTonePlaying::NewL(const TDesC& aTestName)
	{
	RStepA3FDevSoundInitializeWhileTonePlaying* self = new (ELeave) RStepA3FDevSoundInitializeWhileTonePlaying(aTestName);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundInitializeWhileTonePlaying::DoKickoffTestL()
	{
	}

/*
 *
 * Fsm
 *
 */
void RStepA3FDevSoundInitializeWhileTonePlaying::Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateToneStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInitializeWhileTonePlaying"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventToneStart)
				{
				// Initializing DevSound object for tone play
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateToneInitializing"));
				iState = EStateToneInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventToneStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateToneInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteTone && aError == KErrNone)
				{
				TTimeIntervalMicroSeconds duration(KMicroSecsInTenSecs);
				// Initialize the audio device and start playing a tone
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneL"));
				TRAPD(err, iMMFDevSound->PlayToneL(KFrequency1, duration));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateTonePlaying"));
				iState = EStateTonePlaying;

				// Calling CMMFDevSound::InitializeL again while tone playing!
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL called again while tone playing!"));
				TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err == KErrNotReady)
					{
					INFO_PRINTF2(_L("CMMFDevSound::InitializeL left as expected with error = %d"), err);
					StopTest(err,EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL did NOT leave with KErrNotReady as expected. Error = %d"), err);
					StopTest(KErrUnknown);
					}
				}
			else if (aDevSoundEvent == EEventInitCompleteTone && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteTone not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundNotSupportedPlayFixedSeq
//

/*
 *
 * RStepA3FDevSoundNotSupportedPlayFixedSeq - Test step constructor
 *
 */
RStepA3FDevSoundNotSupportedPlayFixedSeq::RStepA3FDevSoundNotSupportedPlayFixedSeq(const TDesC& aTestName)
:RTestStepA3FDevSoundToneBase(aTestName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundNotSupportedPlayFixedSeq* RStepA3FDevSoundNotSupportedPlayFixedSeq::NewL(const TDesC& aTestName)
	{
	RStepA3FDevSoundNotSupportedPlayFixedSeq* self = new (ELeave) RStepA3FDevSoundNotSupportedPlayFixedSeq(aTestName);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundNotSupportedPlayFixedSeq::DoKickoffTestL()
	{
	}

/*
 *
 * Fsm
 *
 */
void RStepA3FDevSoundNotSupportedPlayFixedSeq::Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateToneStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundNotSupportedPlayFixedSeq"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventToneStart)
				{
				// Initializing DevSound object for tone play
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateToneInitializing"));
				iState = EStateToneInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventToneStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateToneInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteTone && aError == KErrNone)
				{
				TInt seqCount = iMMFDevSound->FixedSequenceCount();
				if (seqCount != 0)
					{
					ERR_PRINTF2(_L("CMMFDevSound::FixedSequenceCount() did not return 0 as always expected. It returned %d instead!"), seqCount);
					}
				else
					{
					INFO_PRINTF1(_L("CMMFDevSound::FixedSequenceCount() call returned 0 as always expected"));
					}
				// Initialize the audio device and start playing the specified pre-defined tone sequence.
				INFO_PRINTF1(_L("Calling the not supported CMMFDevSound::PlayFixedSequenceL"));
				TRAPD(err, iMMFDevSound->PlayFixedSequenceL(0));
				if (err == KErrNotSupported)
					{
					INFO_PRINTF2(_L("CMMFDevSound::PlayFixedSequenceL left as expected with error = %d"), err);
					StopTest(err,EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayFixedSequenceL did NOT leave with KErrNotSupported as expected. Error = %d"), err);
					StopTest(KErrUnknown);
					}
				}
			else if (aDevSoundEvent == EEventInitCompleteTone && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteTone not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundInvalidInitializeForTone
//

/*
 *
 * RStepA3FDevSoundInvalidInitializeForTone - Test step constructor
 *
 */
RStepA3FDevSoundInvalidInitializeForTone::RStepA3FDevSoundInvalidInitializeForTone(const TDesC& aTestName)
:RTestStepA3FDevSoundToneBase(aTestName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundInvalidInitializeForTone* RStepA3FDevSoundInvalidInitializeForTone::NewL(const TDesC& aTestName)
	{
	RStepA3FDevSoundInvalidInitializeForTone* self = new (ELeave) RStepA3FDevSoundInvalidInitializeForTone(aTestName);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundInvalidInitializeForTone::DoKickoffTestL()
	{
	}

/*
 *
 * Fsm
 *
 */
void RStepA3FDevSoundInvalidInitializeForTone::Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateToneStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInvalidInitializeForTone"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventToneStart)
				{
				TFourCC fourCCCodeMP3(KMMFFourCCCodeMP3);
				// Initializing DevSound object for tone play using invalid version of CMMFDevSound::InitializeL
				INFO_PRINTF1(_L("Calling invalid version of CMMFDevSound::InitializeL for tone play..."));
				TRAPD(err, iMMFDevSound->InitializeL(*this, fourCCCodeMP3, EMMFStateTonePlaying));
				if (err == KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left as expected with error = %d"), err);
					StopTest(err,EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL did NOT leave with KErrNotSupported as expected. Error = %d"), err);
					StopTest(KErrUnknown);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventToneStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundCancelInitializeTone
//

/*
 *
 * RStepA3FDevSoundCancelInitializeTone - Test step constructor
 *
 */
RStepA3FDevSoundCancelInitializeTone::RStepA3FDevSoundCancelInitializeTone(const TDesC& aTestName, const TDesC& aSectName)
:RTestStepA3FDevSoundToneBase(aTestName, aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundCancelInitializeTone* RStepA3FDevSoundCancelInitializeTone::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RStepA3FDevSoundCancelInitializeTone* self = new (ELeave) RStepA3FDevSoundCancelInitializeTone(aTestName, aSectName);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundCancelInitializeTone::DoKickoffTestL()
	{
	}
	
/*
 *
 * InitializeComplete
 *
 */
void RStepA3FDevSoundCancelInitializeTone::InitializeComplete(TInt aError)
	{
	INFO_PRINTF1(_L("InitializeComplete callback was received. This is not the expected behaviour"));
	INFO_PRINTF1(_L("InitializeL was not cancelled"));
	StopTest(aError, EFail);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RStepA3FDevSoundCancelInitializeTone::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	INFO_PRINTF1(_L("InitializeComplete callback was not received. This is the expected behaviour"));
	INFO_PRINTF1(_L("Use SetConfigL to verify that DevSound has not been initialised"));
	TMMFCapabilities capabilities;

	if(!GetStringFromConfig(iSectName, _L("SampleRate"), iSampleRate))
		{
		TPtrC keyName(_L("SampleRate"));
		ERR_PRINTF3(_L("Error in getting sample rate from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
	TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));	
	if(errConfig == KErrNotReady)
		{
		INFO_PRINTF2(_L("SetConfigL returned with %d as expected"), errConfig);
		INFO_PRINTF1(_L("InitializeL was cancelled successfully"));
		StopTest(KErrNone);
		}
	else if (errConfig == KErrNone)
		{
		INFO_PRINTF2(_L("SetConfigL returned with KErrNone %d"), errConfig);
		INFO_PRINTF1(_L("InitializeL was not cancelled successfully"));
		StopTest(errConfig, EFail);
		}
	else
		{
		INFO_PRINTF2(_L("SetConfigL returned with unexpected error %d"), errConfig);
		StopTest(errConfig, EFail);
		}
	}
	
/*
 *
 * Fsm
 *	- Executes Toneing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundCancelInitializeTone::Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateToneStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundCancelInitializeTone"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventToneStart)
				{
				// Initializing DevSound object for TonePlaying mode
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF1(_L("State: EStateToneInitializing"));
					iState = EStateToneInitializing;					
					INFO_PRINTF1(_L("Calling CMMFDevSound::CancelInitialize()"));
					iMMFDevSound->CancelInitialize();
					INFO_PRINTF1(_L("Start timer to wait for InitializeComplete"));
					TCallBack callback (TimerCallback, this);
					iTimer->Start(KMicroSecsInTenSecs, 0, callback);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventToneStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundEmptyBuffersInitializedForTone
//

/*
 *
 * RStepA3FDevSoundInitializeWhileTonePlaying - Test step constructor
 *
 */
RStepA3FDevSoundEmptyBuffersInitializedForTone::RStepA3FDevSoundEmptyBuffersInitializedForTone(const TDesC& aTestName, const TDesC& aSectName)
:RTestStepA3FDevSoundToneBase(aTestName, aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundEmptyBuffersInitializedForTone* RStepA3FDevSoundEmptyBuffersInitializedForTone::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RStepA3FDevSoundEmptyBuffersInitializedForTone* self = new (ELeave) RStepA3FDevSoundEmptyBuffersInitializedForTone(aTestName, aSectName);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundEmptyBuffersInitializedForTone::DoKickoffTestL()
	{
	}
	
/*
 *
 * Fsm
 *
 */
void RStepA3FDevSoundEmptyBuffersInitializedForTone::Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateToneStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInitializeWhileTonePlaying"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventToneStart)
				{
				// Initializing DevSound object for tone play
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateToneInitializing"));
				iState = EStateToneInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventToneStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateToneInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteTone && aError == KErrNone)
				{
				// Calling EmptyBuffers
				INFO_PRINTF1(_L("Calling CMMFDevSound::EmptyBuffers()"));
				TInt err;
				err = iMMFDevSound->EmptyBuffers();
				if (err == KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::EmptyBuffers() returned expected error = %d"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::EmptyBuffers() did not return KErrNotSupported as expected. Returned with unexpected error = %d"), err);
					StopTest(err);
					}
				}
			else if (aDevSoundEvent == EEventInitCompleteTone && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteTone not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}
	}

//
// RStepA3FDevSoundEmptyBuffersWhileTonePlaying
//

/*
 *
 * RStepA3FDevSoundEmptyBuffersWhileTonePlaying - Test step constructor
 *
 */
RStepA3FDevSoundEmptyBuffersWhileTonePlaying::RStepA3FDevSoundEmptyBuffersWhileTonePlaying(const TDesC& aTestName)
:RTestStepA3FDevSoundToneBase(aTestName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundEmptyBuffersWhileTonePlaying* RStepA3FDevSoundEmptyBuffersWhileTonePlaying::NewL(const TDesC& aTestName)
	{
	RStepA3FDevSoundEmptyBuffersWhileTonePlaying* self = new (ELeave) RStepA3FDevSoundEmptyBuffersWhileTonePlaying(aTestName);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundEmptyBuffersWhileTonePlaying::DoKickoffTestL()
	{
	}

/*
 *
 * Fsm
 *
 */
void RStepA3FDevSoundEmptyBuffersWhileTonePlaying::Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateToneStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundEmptyBuffersWhileTonePlaying"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventToneStart)
				{
				// Initializing DevSound object for tone play
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateToneInitializing"));
				iState = EStateToneInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventToneStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateToneInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteTone && aError == KErrNone)
				{
				TTimeIntervalMicroSeconds duration(KMicroSecsInTenSecs);
				// Initialize the audio device and start playing a tone
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneL"));
				TRAPD(err, iMMFDevSound->PlayToneL(KFrequency1, duration));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateTonePlaying"));
				iState = EStateTonePlaying;

				// Calling CMMFDevSound::EmptyBuffers while tone playing
				INFO_PRINTF1(_L("CMMFDevSound::EmptyBuffers called while tone playing"));
				err = iMMFDevSound->EmptyBuffers();
				if (err == KErrNotSupported)
					{
					INFO_PRINTF2(_L("CMMFDevSound::EmptyBuffers left as expected with error = %d"), err);
					StopTest(err,EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::EmptyBuffers did NOT leave with KErrNotSupported as expected. Error = %d"), err);
					StopTest(KErrUnknown);
					}
				}
			else if (aDevSoundEvent == EEventInitCompleteTone && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteTone not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}

	}
