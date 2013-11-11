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
// Implementation of Integraton test step classes for A3f DevSound Play test cases.
// 
//

#include "tsi_a3f_devsound_play.h"

/*
 *
 * RTestStepA3FDevSoundPlayBase - Test step constructor
 *
 */
RTestStepA3FDevSoundPlayBase::RTestStepA3FDevSoundPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RAsyncTestStep(),
 iMMFDevSound(NULL),
 iBuffer(NULL),
 iTimer(NULL),
 iCallBack(NULL, NULL),
 iInputDataType(aInputDataType),
 iSectName(aSectName),
 iFilename(KNullDesC),
 iSampleRate(KNullDesC),
 iMonoStereo(KNullDesC),
 iPlayState(EStatePlayStart),
 iBufferCount(0),
 iConfig(EFalse),
 iInvalidConfig(EFalse),
 iUseFourCC(EFalse)
	{
	// From RTestStep
	iTestStepName = aTestName; // store the name of the test case

	}

/*
 *
 * RTestStepA3FDevSoundPlayBase - Test step constructor
 *
 */
RTestStepA3FDevSoundPlayBase::RTestStepA3FDevSoundPlayBase(const TDesC& aTestName, const TDesC& aSectName)
:RAsyncTestStep(),
 iMMFDevSound(NULL),
 iBuffer(NULL),
 iTimer(NULL),
 iCallBack(NULL, NULL),
 iInputDataType(KMMFFourCCCodePCM16),
 iSectName(aSectName),
 iFilename(KNullDesC),
 iSampleRate(KNullDesC),
 iMonoStereo(KNullDesC),
 iPlayState(EStatePlayStart),
 iBufferCount(0),
 iConfig(EFalse),
 iInvalidConfig(EFalse),
 iUseFourCC(EFalse)
	{
	// From RTestStep
	iTestStepName = aTestName; // store the name of the test case

	}

/*
 *
 * RTestStepA3FDevSoundPlayBase - Test step constructor
 *
 */
RTestStepA3FDevSoundPlayBase::RTestStepA3FDevSoundPlayBase(const TDesC& aTestName, const TFourCC& aInputDataType)
:RAsyncTestStep(),
 iMMFDevSound(NULL),
 iBuffer(NULL),
 iTimer(NULL),
 iCallBack(NULL, NULL),
 iInputDataType(aInputDataType),
 iSectName(KNullDesC),
 iFilename(KNullDesC),
 iSampleRate(KNullDesC),
 iMonoStereo(KNullDesC),
 iPlayState(EStatePlayStart),
 iBufferCount(0),
 iConfig(EFalse),
 iInvalidConfig(EFalse),
 iUseFourCC(EFalse)
	{
	// From RTestStep
	iTestStepName = aTestName; // store the name of the test case

	}


/*
 *
 * RTestStepA3FDevSoundPlayBase - Test step constructor
 *
 */
RTestStepA3FDevSoundPlayBase::RTestStepA3FDevSoundPlayBase(const TDesC& aTestName)
:RAsyncTestStep(),
 iMMFDevSound(NULL),
 iBuffer(NULL),
 iTimer(NULL),
 iCallBack(NULL, NULL),
 iInputDataType(KMMFFourCCCodePCM16),
 iSectName(KNullDesC),
 iFilename(KNullDesC),
 iSampleRate(KNullDesC),
 iMonoStereo(KNullDesC),
 iPlayState(EStatePlayStart),
 iBufferCount(0),
 iConfig(EFalse),
 iInvalidConfig(EFalse),
 iUseFourCC(EFalse)
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
void RTestStepA3FDevSoundPlayBase::KickoffTestL()
	{
	TInt err = iFs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not connect to Filesystem. Error = %d"), err);
		StopTest(err);
		}

	// Get the file name of the audio file to play
	#ifdef __WINS__
		if(!GetStringFromConfig(iSectName, _L("EmuPlayFile"), iFilename))
		{
		TPtrC keyName(_L("EmuPlayFile"));
		ERR_PRINTF3(_L("Error in getting file name from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	#elif __MARM__
		if(!GetStringFromConfig(iSectName, _L("HwPlayFile"), iFilename))
		{
		TPtrC keyName(_L("HwPlayFile"));
		ERR_PRINTF3(_L("Error in getting file name from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}
	#endif

	// Check if SetConfig and FourCC are to be used or not
	CheckConfigAndFourCCUsage();

	INFO_PRINTF2(_L("Section Name is :%S"), &iSectName);

	// Open using RFile for playback
	err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not Open the File: Error = %d"), err);
		StopTest(err);
		}

	// Create instance of CMMFDevSound
	iMMFDevSound = CMMFDevSound::NewL();

	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iCallBack = TCallBack(TimerCallback, this);

	DoKickoffTestL();
	Fsm(EEventPlayStart, KErrNone); // call to initialize DevSound

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RTestStepA3FDevSoundPlayBase::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be played -> %S"), &iFilename);
	DisplayCodecInfo();

	}

/*
 * ToneFinished
 *
 */
void RTestStepA3FDevSoundPlayBase::ToneFinished(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundPlayBase"),EInvalidCallbackCall));

	}

/*
 *
 * RecordError
 *
 */
void RTestStepA3FDevSoundPlayBase::RecordError(TInt)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundPlayBase"),EInvalidCallbackCall));

	}

/*
 *
 * BufferToBeEmptied
 *
 */
void RTestStepA3FDevSoundPlayBase::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundPlayBase"),EInvalidCallbackCall));

	}

/*
 *
 * ConvertError
 *
 */
void RTestStepA3FDevSoundPlayBase::ConvertError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundPlayBase"),EInvalidCallbackCall));

	}

/*
 *
 * DeviceMessage
 *
 */
void RTestStepA3FDevSoundPlayBase::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundPlayBase"),EInvalidCallbackCall));

	}

/*
 *
 * TimerCallback
 *
 */
TInt RTestStepA3FDevSoundPlayBase::TimerCallback(TAny* aPtr)
	{
	static_cast<RTestStepA3FDevSoundPlayBase*>(aPtr)->DoTimerCallback();
	return KErrNone;

	}

/*
 *
 * DoTimerCallback
 *
 */
void RTestStepA3FDevSoundPlayBase::DoTimerCallback()
	{
	// The derived classes may provide the implementation if needed
	}

/*
 *
 * CallStopTest
 *
 */
void RTestStepA3FDevSoundPlayBase::CloseTest()
	{
	if (iMMFDevSound)
		{
		INFO_PRINTF1(_L("Deleting DevSound object..."));
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}

	if (iTimer)
		{
		delete iTimer;
		iTimer = NULL;
		}

	iFile.Close();
	iFs.Close();

	}

/*
 *
 * InitializeComplete
 *
 */
void RTestStepA3FDevSoundPlayBase::InitializeComplete(TInt aError)
	{
	if (iPlayState == EStatePlayInitializing)
		{
		Fsm(EEventInitCompletePlay,aError);
		}

	}

/*
 *
 * BufferToBeFilled
 *
 */
void RTestStepA3FDevSoundPlayBase::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		StopTest(KErrGeneral);
		return;
		}

	iBuffer = aBuffer;

	// Call to continue the Play process
	Fsm(EEventBTBF,KErrNone);

	}

/*
 *
 * PlayError
 *
 */
void RTestStepA3FDevSoundPlayBase::PlayError(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);
	if ((iBuffer->LastBuffer()) && (aError == KErrUnderflow))
		{
		StopTest();
		}
	else
		{
		StopTest(aError, EFail);
		}

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RTestStepA3FDevSoundPlayBase::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RTestStepA3FDevSoundPlayBase"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}


				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				if(iConfig)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					TMMFCapabilities capabilities;
					capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
					capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

					TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));
					if (errConfig != KErrNone && !iInvalidConfig)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with error = %d"), errConfig);
						StopTest(errConfig);
						return;
						}
					else if (errConfig == KErrArgument && iInvalidConfig)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with expected error = %d"), errConfig);
						iInvalidConfig = EFalse;
						StopTest(errConfig, EPass);
						return;
						}
					}

				// Initialize audio device and starts the playing process
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(errPlay, iMMFDevSound->PlayInitL());
				if (errPlay != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left with error = %d"), errPlay);
					StopTest(errPlay);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlaying:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RTestStepA3FDevSoundPlayBase"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBF)
				{
				// Fill the buffer with audio data and play the data in the buffer
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
					iBuffer->SetLastBuffer(ETrue);
					}

				// Playing data in the buffer at the current volume
				iMMFDevSound->PlayData();
				iBufferCount++;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;
			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

/*
 *
 * DisplayCodecInfo
 *
 */
void RTestStepA3FDevSoundPlayBase::DisplayCodecInfo()
	{
	// Display trace about codec under test
	TUint32 fourCC = iInputDataType.FourCC();
	INFO_PRINTF5(_L("Codec under test -> %c%c%c%c"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24);

	}

/*
 *
 * ConvertDesToTMMFSampleRate
 *
 */
TUint RTestStepA3FDevSoundPlayBase::ConvertDesToTMMFSampleRate(const TDesC& aSampleRateString)
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

	INFO_PRINTF2(_L("Sample Rate = %S"), &iSampleRate);
	return rate;

	}

/*
 *
 * ConvertDesToTMMFMonoStereo
 *
 */
TUint RTestStepA3FDevSoundPlayBase::ConvertDesToTMMFMonoStereo(const TDesC& aMonoStereoString)
	{
	TUint channelMode = 0;

	STR_SWITCH(aMonoStereoString)

	STR_CASE(_L("EMMFMono"))
		{
		channelMode = EMMFMono;
		break;
		}
	STR_CASE(_L("EMMFStereo"))
		{
		channelMode = EMMFStereo;
		break;
		}
	STR_CASE(_L("InvalidMode"))
		{
		channelMode = 0x90000000;
		break;
		}
	DEFAULT_CASE()
		{
		ERR_PRINTF1(_L("Attempt to convert invalid channel mode"));
		StopTest(KErrGeneral);
		}

	STR_SWITCH_END()

	INFO_PRINTF2(_L("Channel = %S"), &iMonoStereo);
	return channelMode;

	}

/*
 *
 * SetSampleRateAndChannelMode
 *
 */
void RTestStepA3FDevSoundPlayBase::SetSampleRateAndChannelMode()
	{
	if(!GetStringFromConfig(iSectName, _L("SampleRate"), iSampleRate))
		{
		TPtrC keyName(_L("SampleRate"));
		ERR_PRINTF3(_L("Error in getting sample rate from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	if(!GetStringFromConfig(iSectName, _L("MonoStereo"), iMonoStereo))
		{
		TPtrC keyName(_L("MonoStereo"));
		ERR_PRINTF3(_L("Error in getting channel mode from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	}

/*
 *
 * CheckConfigAndFourCCUsage
 *
 */
void RTestStepA3FDevSoundPlayBase::CheckConfigAndFourCCUsage()
	{
	if(!GetBoolFromConfig(iSectName, _L("SetConfig"), iConfig))
		{
		TPtrC keyName(_L("SetConfig"));
		ERR_PRINTF3(_L("Error in getting SetConfig flag value from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}
	if(!GetBoolFromConfig(iSectName, _L("UseFourCC"), iUseFourCC))
		{
		TPtrC keyName(_L("UseFourCC"));
		ERR_PRINTF3(_L("Error in getting UseFourCC flag value from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	}

//
// RStepA3FDevSoundPlaySimple
//

/*
 *
 * RStepA3FDevSoundPlaySimple - Test step constructor
 *
 */
RStepA3FDevSoundPlaySimple::RStepA3FDevSoundPlaySimple(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * RStepA3FDevSoundPlaySimple - Test step constructor
 *
 */
RStepA3FDevSoundPlaySimple::RStepA3FDevSoundPlaySimple(const TDesC& aTestName, const TDesC& aSectName)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundPlaySimple* RStepA3FDevSoundPlaySimple::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundPlaySimple* self = new (ELeave) RStepA3FDevSoundPlaySimple(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundPlaySimple* RStepA3FDevSoundPlaySimple::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RStepA3FDevSoundPlaySimple* self = new (ELeave) RStepA3FDevSoundPlaySimple(aTestName, aSectName);
	return self;

	}

//
// RStepA3FDevSoundPlayEmpty
//

/*
 *
 * RStepA3FDevSoundPlayEmpty - Test step constructor
 *
 */
RStepA3FDevSoundPlayEmpty::RStepA3FDevSoundPlayEmpty(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundPlayEmpty* RStepA3FDevSoundPlayEmpty::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundPlayEmpty* self = new (ELeave) RStepA3FDevSoundPlayEmpty(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundPlayEmpty::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundPlayEmpty"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				if(iConfig)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					TMMFCapabilities capabilities;
					capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
					capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

					TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));
					if (errConfig != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with error = %d"), errConfig);
						StopTest(errConfig);
						return;
						}
					}

				// Initialize audio device and starts the playing process
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(errPlay, iMMFDevSound->PlayInitL());
				if (errPlay != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left with error = %d"), errPlay);
					StopTest(errPlay);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlaying:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundPlayEmpty"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBF)
				{
				// Fill the buffer with audio data and play the data in the buffer
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
					// Playing empty data
					iMMFDevSound->PlayData();

					}
				iBufferCount++;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;
			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundPlayInvalidFourCC
//

/*
 *
 * RStepA3FDevSoundPlayInvalidFourCC - Test step constructor
 *
 */
RStepA3FDevSoundPlayInvalidFourCC::RStepA3FDevSoundPlayInvalidFourCC(const TDesC& aTestName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundPlayInvalidFourCC* RStepA3FDevSoundPlayInvalidFourCC::NewL(const TDesC& aTestName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundPlayInvalidFourCC* self = new (ELeave) RStepA3FDevSoundPlayInvalidFourCC(aTestName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundPlayInvalidFourCC::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	//Display the name of Codec being used
	DisplayCodecInfo();
	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundPlayInvalidFourCC::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundPlayInvalidFourCC"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
				if (err == KErrNotSupported)
					{
					INFO_PRINTF2(_L("CMMFDevSound::InitializeL left with expected error = %d"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					INFO_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}




//
// RStepA3FDevSoundInitializeWithHwDeviceUID
//

/*
 *
 * RStepA3FDevSoundInitializeWithHwDeviceUID - Test step constructor
 *
 */
RStepA3FDevSoundInitializeWithHwDeviceUID::RStepA3FDevSoundInitializeWithHwDeviceUID(const TDesC& aTestName)
:RTestStepA3FDevSoundPlayBase(aTestName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundInitializeWithHwDeviceUID* RStepA3FDevSoundInitializeWithHwDeviceUID::NewL(const TDesC& aTestName)
	{
	RStepA3FDevSoundInitializeWithHwDeviceUID* self = new (ELeave) RStepA3FDevSoundInitializeWithHwDeviceUID(aTestName);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundInitializeWithHwDeviceUID::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	//Display the name of Codec being used.
	DisplayCodecInfo();
	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundInitializeWithHwDeviceUID::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInitializeWithHwDeviceUID"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via HwDeviceUID
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL with KMmfUidHwDevicePCM16ToPCM16"));
				TInt err;
				TRAP(err, iMMFDevSound->InitializeL(*this, 0x101F9F00 , EMMFStatePlaying)); //KMmfUidHwDevicePCM16ToPCM16
				if (err == KErrNotSupported)
					{
					INFO_PRINTF2(_L("CMMFDevSound::InitializeL left with expected error = %d"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					INFO_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundPlayInvalidConfig
//

/*
 *
 * RStepA3FDevSoundPlayInvalidConfig - Test step constructor
 *
 */
RStepA3FDevSoundPlayInvalidConfig::RStepA3FDevSoundPlayInvalidConfig(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundPlayInvalidConfig* RStepA3FDevSoundPlayInvalidConfig::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundPlayInvalidConfig* self = new (ELeave) RStepA3FDevSoundPlayInvalidConfig(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundPlayInvalidConfig::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();
	
	iInvalidConfig = ETrue;
	//Display the name of Codec being used
	DisplayCodecInfo();
	CheckConfigAndFourCCUsage();
	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}

/*
 *
 * PlayError
 *
 */
void RStepA3FDevSoundPlayInvalidConfig::PlayError(TInt aError)
	{
	if ((iPlayState == EStatePlaying) && (aError == KErrArgument))
		{
		INFO_PRINTF2(_L("DevSound called PlayError with expected error = %d"), aError);
		StopTest(aError, EPass);
		}
	else
		{
		INFO_PRINTF1(_L("Expected error = -6"));
		ERR_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);
		StopTest(aError, EFail);
		}

	}

//
// RStepA3FDevSoundReadConfigDuringPlay
//

/*
 *
 * RStepA3FDevSoundReadConfigDuringPlay - Test step constructor
 *
 */
RStepA3FDevSoundReadConfigDuringPlay::RStepA3FDevSoundReadConfigDuringPlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundReadConfigDuringPlay* RStepA3FDevSoundReadConfigDuringPlay::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundReadConfigDuringPlay* self = new (ELeave) RStepA3FDevSoundReadConfigDuringPlay(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * PlayError
 *
 */
void RStepA3FDevSoundReadConfigDuringPlay::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		StopTest(KErrGeneral);
		return;
		}

	iBuffer = aBuffer;

	if (iPlayState == EStatePlaying)
		{
		TMMFCapabilities capabilities;
		capabilities = iMMFDevSound->Config();
		INFO_PRINTF1(_L("Config values during Play: "));
		if (capabilities.iRate == ConvertDesToTMMFSampleRate(iSampleRate) && capabilities.iChannels == ConvertDesToTMMFMonoStereo(iMonoStereo))
			{
			INFO_PRINTF1(_L("Config values before and during playback are same."));
			StopTest(KErrNone);
			}
		else
			{
			ERR_PRINTF1(_L("Config values before and during playback are different."));
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundSetVolPlayStopGetVol
//

/*
 *
 * RStepA3FDevSoundSetVolPlayStopGetVol - Test step constructor
 *
 */
RStepA3FDevSoundSetVolPlayStopGetVol::RStepA3FDevSoundSetVolPlayStopGetVol(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType),
 iSetVolume(0),
 iVolDuringStopped(0),
 iCheckTimer(ETrue)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundSetVolPlayStopGetVol* RStepA3FDevSoundSetVolPlayStopGetVol::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundSetVolPlayStopGetVol* self = new (ELeave) RStepA3FDevSoundSetVolPlayStopGetVol(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundSetVolPlayStopGetVol::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be played -> %S"), &iFilename);
	DisplayCodecInfo();
	CheckConfigAndFourCCUsage();

	TBool vol = GetIntFromConfig(iSectName,_L("Setvolume"),iSetVolume);
	if (vol)
		{
		INFO_PRINTF2(_L("SetVolume = %d"), iSetVolume );
		}
	else
		{
		TPtrC keyName(_L("Setvolume"));
		ERR_PRINTF3(_L("Error in getting volume to be set from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundSetVolPlayStopGetVol::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundSetVolPlayStopGetVol"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				if(iConfig)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					TMMFCapabilities capabilities;
					capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
					capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

					TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));
					if (errConfig != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with error = %d"), errConfig);
						StopTest(errConfig);
						break;
						}
					}
				// Setting Volume Before Playback
				iMMFDevSound->SetVolume(iSetVolume);
				INFO_PRINTF2(_L("Volume before playback = %d"), iSetVolume);
				// Initialize audio device and starts the playing process
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(errPlay, iMMFDevSound->PlayInitL());
				if (errPlay != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left with error = %d"), errPlay);
					StopTest(errPlay);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlaying:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundSetVolPlayStopGetVol"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBF)
				{
				// Fill the buffer with audio data and play the data in the buffer
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
					iBuffer->SetLastBuffer(ETrue);
					}

				if (iCheckTimer)
					{
					iTimer->Start(KMicroSecsInTenSecs, 0, iCallBack);
					}
				iCheckTimer = EFalse;
				// Playing data in the buffer at the current volume
				iMMFDevSound->PlayData();
				iBufferCount++;
				}
			else if (aDevSoundEvent == EEventPlayTimerComplete)
				{
				iMMFDevSound->Stop();
				iPlayState = EStatePlayStopped;
				INFO_PRINTF1(_L("State: EStatePlayStopped"));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayStopped:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundSetVolPlayStopGetVol"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStopped)
				{
				iVolDuringStopped = iMMFDevSound->Volume();
				INFO_PRINTF2(_L("Volume when play is stopped: %d"), iVolDuringStopped);
				if (iVolDuringStopped == iSetVolume)
					{
					INFO_PRINTF1(_L("Volume set before the playback is same as volume during playback stopped"));
					StopTest(KErrNone);
					}
				else
					{
					ERR_PRINTF1(_L("Volume set before the playback is not the same as volume during playback stopped"));
					StopTest(KErrGeneral);
					}					
				}
			else
				{
				ERR_PRINTF1(_L("EStatePlayStopped event not received in EStatePlayStopped state"));
				StopTest(KErrGeneral);
				break;
				}
			}
			break;

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

/*
 *
 * DoTimerCallback
 *
 */
void RStepA3FDevSoundSetVolPlayStopGetVol::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	if(!iCheckTimer)
		{
		Fsm(EEventPlayTimerComplete, KErrNone);
		Fsm(EEventPlayStopped, KErrNone);
		}

	}

//
// RStepA3FDevSoundFlushBuffersPaused
//

/*
 *
 * RStepA3FDevSoundFlushBuffersPaused - Test step constructor
 *
 */
RStepA3FDevSoundFlushBuffersPaused::RStepA3FDevSoundFlushBuffersPaused(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType),
 iCheckTimer(ETrue)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundFlushBuffersPaused* RStepA3FDevSoundFlushBuffersPaused::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundFlushBuffersPaused* self = new (ELeave) RStepA3FDevSoundFlushBuffersPaused(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundFlushBuffersPaused::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundFlushBuffersPaused"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				if(iConfig)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					TMMFCapabilities capabilities;
					capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
					capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

					TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));
					if (errConfig != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with error = %d"), errConfig);
						StopTest(errConfig);
						return;
						}
					}
				// Initialize audio device and starts the playing process
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(errPlay, iMMFDevSound->PlayInitL());
				if (errPlay != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left with error = %d"), errPlay);
					StopTest(errPlay);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlaying:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundFlushBuffersPaused"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBF)
				{
				// Fill the buffer with audio data and play the data in the buffer
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
					iBuffer->SetLastBuffer(ETrue);
					}
				if (iCheckTimer)
					{
					iTimer->Start(KMicroSecsInTenSecs, 0, iCallBack);
					}
				// Playing data in the buffer at the current volume
				iMMFDevSound->PlayData();
				iBufferCount++;
				iCheckTimer = EFalse;
				}
			else if (aDevSoundEvent == EEventPlayTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause()"));
				iMMFDevSound->Pause();
				iPlayState = EStatePlayPaused;
				INFO_PRINTF1(_L("State: EStatePlayPaused"));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;
			}

		case EStatePlayPaused:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundFlushBuffersPaused"),EFsmIncorrectErrorPassed));
			TInt err;
			if (aDevSoundEvent == EEventPlayPaused)
				{
				err = iMMFDevSound->EmptyBuffers();
				INFO_PRINTF2(_L("EmptyBuffers in paused state returned with %d"), err);
				INFO_PRINTF2(_L("Expected to return with %d"), KErrNone);
				StopTest(err);
				}
			break;
			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

/*
 *
 * BufferToBeFilled
 *
 */
void RStepA3FDevSoundFlushBuffersPaused::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		StopTest(KErrGeneral);
		return;
		}

	iBuffer = aBuffer;

	if(iPlayState != EStatePlayPaused)
		{
		Fsm(EEventBTBF, KErrNone);
		}

	}

/*
 *
 * DoTimerCallback
 *
 */
void RStepA3FDevSoundFlushBuffersPaused::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	if(!iCheckTimer)
		{
		Fsm(EEventPlayTimerComplete, KErrNone);
		Fsm(EEventPlayPaused, KErrNone);
		}
	else
		{
		StopTest(KErrGeneral);
		}

	}

//
// RStepA3FDevSoundInvalidStatePlay
//

/*
 *
 * RStepA3FDevSoundInvalidStatePlay - Test step constructor
 *
 */
RStepA3FDevSoundInvalidStatePlay::RStepA3FDevSoundInvalidStatePlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundInvalidStatePlay* RStepA3FDevSoundInvalidStatePlay::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundInvalidStatePlay* self = new (ELeave) RStepA3FDevSoundInvalidStatePlay(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundInvalidStatePlay::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	//Display the name of Codec being used
	DisplayCodecInfo();
	if(!GetBoolFromConfig(iSectName, _L("UseFourCC"), iUseFourCC))
		{
		TPtrC keyName(_L("UseFourCC"));
		ERR_PRINTF3(_L("Error in getting UseFourCC flag value from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundInvalidStatePlay::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInvalidStatePlay"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				INFO_PRINTF1(_L("Initializing DevSound in EMMFStateRecording"));
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStateRecording));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err, EFail);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				// Initialize audio device and start the playing process
				// Calling CMMFDevSound::PlayInitL() when DevSound initialized for record mode!
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(errPlay, iMMFDevSound->PlayInitL());
				if (errPlay == KErrNotReady)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left as expected with error = %d"), errPlay);
					StopTest(errPlay,EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL did NOT leave with KErrNotReady as expected. Error = %d"), errPlay);
					StopTest(KErrUnknown);
					}
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

/*
 *
 * BufferToBeFilled
 *
 */
void RStepA3FDevSoundInvalidStatePlay::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		StopTest(KErrGeneral);
		return;
		}

	iBuffer = aBuffer;

	// Call to continue the Play process
	Fsm(EEventBTBF,KErrNone);

	}

/*
 *
 * PlayError
 *
 */
void RStepA3FDevSoundInvalidStatePlay::PlayError(TInt aError)
	{
	if ((iPlayState == EStatePlaying) && (aError == KErrNotSupported))
		{
		INFO_PRINTF2(_L("DevSound called PlayError with expected error = %d"), aError);
		StopTest(aError, EPass);
		}
	else
		{
		INFO_PRINTF1(_L("Expected error = -5"));
		ERR_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);
		StopTest(aError, EFail);
		}

	}

//
// RStepA3FDevSoundFlushBuffersPlaying
//

/*
 *
 * RStepA3FDevSoundFlushBuffersPlaying - Test step constructor
 *
 */
RStepA3FDevSoundFlushBuffersPlaying::RStepA3FDevSoundFlushBuffersPlaying(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundFlushBuffersPlaying* RStepA3FDevSoundFlushBuffersPlaying::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundFlushBuffersPlaying* self = new (ELeave) RStepA3FDevSoundFlushBuffersPlaying(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundFlushBuffersPlaying::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundFlushBuffersPlaying"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				if(iConfig)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					TMMFCapabilities capabilities;
					capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
					capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

					TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));
					if (errConfig != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with error = %d"), errConfig);
						StopTest(errConfig);
						break;
						}
					}
				// Initialize audio device and starts the playing process
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(errPlay, iMMFDevSound->PlayInitL());
				if (errPlay != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left with error = %d"), errPlay);
					StopTest(errPlay);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlaying:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundFlushBuffersPlaying"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBF)
				{
				// Fill the buffer with audio data and play the data in the buffer
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
					iBuffer->SetLastBuffer(ETrue);
					}

				INFO_PRINTF1(_L("Calling CMMFDevSound::EmptyBuffers() during playing"));
				err = iMMFDevSound->EmptyBuffers();
				INFO_PRINTF2(_L("CMMFDevSound::EmptyBuffers() left with error = %d"), err);
				if(err == KErrNone)
					{
					StopTest(err, EPass);
					}
				else
					{
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundInitializeWhilePlaying
//

/*
 *
 * RStepA3FDevSoundInitializeWhilePlaying - Test step constructor
 *
 */
RStepA3FDevSoundInitializeWhilePlaying::RStepA3FDevSoundInitializeWhilePlaying(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType),
 iCheckTimer(ETrue)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundInitializeWhilePlaying* RStepA3FDevSoundInitializeWhilePlaying::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundInitializeWhilePlaying* self = new (ELeave) RStepA3FDevSoundInitializeWhilePlaying(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundInitializeWhilePlaying::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInitializeWhilePlaying"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				if(iConfig)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					TMMFCapabilities capabilities;
					capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
					capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

					TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));
					if (errConfig != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with error = %d"), errConfig);
						StopTest(errConfig);
						break;
						}
					}
				// Initialize audio device and starts the playing process
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(errPlay, iMMFDevSound->PlayInitL());
				if (errPlay != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left with error = %d"), errPlay);
					StopTest(errPlay);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlaying:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInitializeWhilePlaying"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBF)
				{
				// Fill the buffer with audio data and play the data in the buffer
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
					iBuffer->SetLastBuffer(ETrue);
					}

				if (iCheckTimer)
					{
					iTimer->Start(KMicroSecsInTenSecs, 0, iCallBack);
					}
				iCheckTimer = EFalse;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

/*
 *
 * DoTimerCallback
 *
 */
void RStepA3FDevSoundInitializeWhilePlaying::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	if(!iCheckTimer)
		{
		INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL() during playing"));
		TRAPD(errInit, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
		INFO_PRINTF2(_L("CMMFDevSound::InitializeL() left with error = %d"), errInit);
		INFO_PRINTF2(_L("Expected error = %d"), KErrNotReady);
		if(errInit == KErrNotReady)
			{
			StopTest(errInit, EPass);
			}
			else
			{
			StopTest(errInit, EFail);
			}
		}

	}

//
// RStepA3FDevSoundSetBalanceDuringPlay
//

/*
 *
 * RStepA3FDevSoundSetBalanceDuringPlay - Test step constructor
 *
 */
RStepA3FDevSoundSetBalanceDuringPlay::RStepA3FDevSoundSetBalanceDuringPlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType),
 iPlaybalanceLeft(0),
 iPlaybalanceRight(0)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundSetBalanceDuringPlay* RStepA3FDevSoundSetBalanceDuringPlay::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundSetBalanceDuringPlay* self = new (ELeave) RStepA3FDevSoundSetBalanceDuringPlay(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundSetBalanceDuringPlay::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be played -> %S"), &iFilename);
	DisplayCodecInfo();
	CheckConfigAndFourCCUsage();

	TBool balance = GetIntFromConfig(iSectName,_L("PlaybalanceLeft"),iPlaybalanceLeft );
	if (balance)
		{
		// Getting the Left balance from Config file
		INFO_PRINTF2(_L("Left Percentage Playbalance = %d"),iPlaybalanceLeft);
		}
	else
		{
		TPtrC keyName(_L("PlaybalanceLeft"));
		ERR_PRINTF3(_L("Error in getting left pc playbalance from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}
	balance = GetIntFromConfig(iSectName,_L("PlaybalanceRight"),iPlaybalanceRight );
	if (balance)
		{
		// Getting the Right balance from Config file
		INFO_PRINTF2(_L("Right Percentage Playbalance = %d"),iPlaybalanceRight);
		}
	else
		{
		TPtrC keyName(_L("PlaybalanceRight"));
		ERR_PRINTF3(_L("Error in getting right pc playbalance from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	}

/*
 *
 * BufferToBeFilled
 *
 */
void RStepA3FDevSoundSetBalanceDuringPlay::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		StopTest(KErrGeneral);
		return;
		}

	iBuffer = aBuffer;

	INFO_PRINTF3(_L("Setting left and right playbalance. left = %d right = %d"), iPlaybalanceLeft, iPlaybalanceRight );
	TRAPD(setErr, iMMFDevSound->SetPlayBalanceL(iPlaybalanceLeft, iPlaybalanceRight));
	if (setErr == KErrNone)
		{
		TInt getLeft = 0;
		TInt getRight = 0;
		INFO_PRINTF1(_L("Getting left and right playbalance"));
		TRAPD(getErr, iMMFDevSound->GetPlayBalanceL(getLeft, getRight));
		INFO_PRINTF3(_L("Got left and right playbalance. left = %d right = %d"), getLeft, getRight);
		if (getErr == KErrNone)
			{
			if ((getLeft == iPlaybalanceLeft) && (getRight == iPlaybalanceRight ))
				{
				INFO_PRINTF1(_L("Left and Right playbalanace values are same as the ones that were set"));
				StopTest(getErr);
				}
			else
				{
				ERR_PRINTF1(_L("Let and right playbalanace values are not the same as the ones that were set"));
				StopTest(KErrGeneral);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Error in getting left and right play balance."));
			StopTest(getErr);
			}
		}
	else
		{
		ERR_PRINTF1(_L("Error in setting left and right play balance."));
		StopTest(setErr);
		}

	}

//
// RStepA3FDevSoundPlayStopMultipleTimes
//

/*
 *
 * RStepA3FDevSoundPlayStopMultipleTimes - Test step constructor
 *
 */
RStepA3FDevSoundPlayStopMultipleTimes::RStepA3FDevSoundPlayStopMultipleTimes(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType),
iRepeatCount(0),
iRepeatCounter(0),
iCheckTimer(ETrue)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundPlayStopMultipleTimes* RStepA3FDevSoundPlayStopMultipleTimes::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundPlayStopMultipleTimes* self = new (ELeave) RStepA3FDevSoundPlayStopMultipleTimes(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundPlayStopMultipleTimes::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be played -> %S"), &iFilename);
	DisplayCodecInfo();
	CheckConfigAndFourCCUsage();

	TBool repeatCount = GetIntFromConfig(iSectName,_L("Repeat"),iRepeatCount);
	if (repeatCount)
		{
		INFO_PRINTF2(_L("No. of times to Repeat = %d"), iRepeatCount);
		}
	else
		{
		TPtrC keyName(_L("Repeat"));
		ERR_PRINTF3(_L("Error in getting number of times to repeat stop from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundPlayStopMultipleTimes::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundPlayStopMultipleTimes"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				if(iConfig)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					TMMFCapabilities capabilities;
					capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
					capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

					TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));
					if (errConfig != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with error = %d"), errConfig);
						StopTest(errConfig);
						break;
						}
					}
				// Initialize audio device and starts the playing process
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(errPlay, iMMFDevSound->PlayInitL());
				if (errPlay != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left with error = %d"), errPlay);
					StopTest(errPlay);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlaying:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundPlayStopMultipleTimes"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBF)
				{
				// Fill the buffer with audio data and play the data in the buffer
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
					iBuffer->SetLastBuffer(ETrue);
					}

				if ((iCheckTimer) && (iRepeatCounter < iRepeatCount))
					{
					iTimer->Start(KMicroSecsInTenSecs, 0, iCallBack);
					}
				iCheckTimer = EFalse;
				// Playing data in the buffer at the current volume
				iMMFDevSound->PlayData();
				iBufferCount++;
				}
			else if (aDevSoundEvent == EEventPlayTimerComplete)
				{
				while(iRepeatCounter <= iRepeatCount)
					{
					INFO_PRINTF1(_L("Calling iMMFDevSound->Stop()"));
					iMMFDevSound->Stop();
					iRepeatCounter++;
					INFO_PRINTF2(_L("Playback Stopped. Count = %d"), iRepeatCounter);
					INFO_PRINTF1(_L("Changing the state to: EStatePlaying "));
					iPlayState = EStatePlayInitializing;
					break;
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * DoTimerCallback
 *
 */
void RStepA3FDevSoundPlayStopMultipleTimes::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	if(!iCheckTimer)
		{
		iCheckTimer = ETrue;
		Fsm(EEventPlayTimerComplete, KErrNone);
		if (iRepeatCounter <= iRepeatCount)
			{
			Fsm(EEventInitCompletePlay, KErrNone);
			}
		}

	}

//
// RStepA3FDevSoundPlayEOFPlayMultipleTimes
//

/*
 *
 * RStepA3FDevSoundPlayEOFPlayMultipleTimes - Test step constructor
 *
 */
RStepA3FDevSoundPlayEOFPlayMultipleTimes::RStepA3FDevSoundPlayEOFPlayMultipleTimes(const TDesC& aTestName, const TDesC& aSectName)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName),
 iRepeatCount(0),
 iRepeatCounter(1)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundPlayEOFPlayMultipleTimes* RStepA3FDevSoundPlayEOFPlayMultipleTimes::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RStepA3FDevSoundPlayEOFPlayMultipleTimes* self = new (ELeave) RStepA3FDevSoundPlayEOFPlayMultipleTimes(aTestName, aSectName);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundPlayEOFPlayMultipleTimes::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be played -> %S"), &iFilename);
	DisplayCodecInfo();
	CheckConfigAndFourCCUsage();

	TBool repeatCount = GetIntFromConfig(iSectName,_L("Repeat"),iRepeatCount);
	if (repeatCount)
		{
		INFO_PRINTF2(_L("No. of times to Repeat = %d"), iRepeatCount);
		}
	else
		{
		TPtrC keyName(_L("Repeat"));
		ERR_PRINTF3(_L("Error in getting number of times to repeat playback from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	}

/*
 *
 * PlayError
 *
 */
void RStepA3FDevSoundPlayEOFPlayMultipleTimes::PlayError(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);
	if ((iBuffer->LastBuffer()) && (aError == KErrUnderflow))
		{
		INFO_PRINTF2(_L("Finished playing. Playback Count = %d"), iRepeatCounter);
		if (iRepeatCounter < iRepeatCount)
			{
			iRepeatCounter++;
			INFO_PRINTF1(_L("Starting to Play again"));
			iFile.Close();
			TInt err = iFile.Open(iFs, iFilename, EFileRead);
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("Could not Open the File: Error = %d"), err);
				StopTest(err);
				}
			iPlayState = EStatePlayInitializing;
			Fsm(EEventInitCompletePlay, KErrNone);
			}
		else
			{
			INFO_PRINTF2(_L("Finished playing %d number of times."), iRepeatCounter);
			StopTest(KErrNone);
			}
		}
	else
		{
		StopTest(aError, EFail);
		}

	}

//
// RStepA3FDevSoundInititalizeDuringInitialize
//

/*
 *
 * RStepA3FDevSoundInititalizeDuringInitialize - Test step constructor
 *
 */
RStepA3FDevSoundInititalizeDuringInitialize::RStepA3FDevSoundInititalizeDuringInitialize(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType),
 iSecondInitErr(0)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundInititalizeDuringInitialize* RStepA3FDevSoundInititalizeDuringInitialize::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundInititalizeDuringInitialize* self = new (ELeave) RStepA3FDevSoundInititalizeDuringInitialize(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundInititalizeDuringInitialize::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	//Display the name of Codec being used
	DisplayCodecInfo();

	if(!GetBoolFromConfig(iSectName, _L("UseFourCC"), iUseFourCC))
		{
		TPtrC keyName(_L("UseFourCC"));
		ERR_PRINTF3(_L("Error in getting UseFourCC flag value from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundInititalizeDuringInitialize::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInititalizeDuringInitialize"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					INFO_PRINTF1(_L("Calling first InitializeL()"));
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
						StopTest(err);
						break;
						}
					else
						{
						INFO_PRINTF1(_L("Calling second InitializeL() beofre first InitializeL() returns."));
						TRAP(iSecondInitErr, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
						}
					}
				else
					{
					INFO_PRINTF1(_L("Calling first InitializeL()"));
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
						StopTest(err);
						break;
						}
					else
						{
						INFO_PRINTF1(_L("Calling second InitializeL() beofre first InitializeL() returns."));
						TRAP(iSecondInitErr, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
						}
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

/*
 *
 * InitializeComplete
 *
 */
void RStepA3FDevSoundInititalizeDuringInitialize::InitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("First Initialize returned with = %d"), aError);
	INFO_PRINTF2(_L("Second Initialize left with = %d"), iSecondInitErr);

	if ((aError == KErrNone) && (iSecondInitErr == KErrNotReady))
		{
		INFO_PRINTF1(_L("This is expected"));
		StopTest(aError);
		}
	else
		{
		INFO_PRINTF2(_L("This is not expected. Second InitializeL should fail with %d"), KErrNotReady);
		StopTest(aError, EFail);
		}

	}

//
// RStepA3FDevSoundPlayInitDuringPlayInit
//

/*
 *
 * RStepA3FDevSoundPlayInitDuringPlayInit - Test step constructor
 *
 */
RStepA3FDevSoundPlayInitDuringPlayInit::RStepA3FDevSoundPlayInitDuringPlayInit(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType),
 iFirstPlayInitErr(0),
 iSecondPlayInitErr(0)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundPlayInitDuringPlayInit* RStepA3FDevSoundPlayInitDuringPlayInit::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundPlayInitDuringPlayInit* self = new (ELeave) RStepA3FDevSoundPlayInitDuringPlayInit(aTestName, aSectName, aInputDataType);
	return self;
	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundPlayInitDuringPlayInit::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	//Display the name of Codec being used
	DisplayCodecInfo();
	CheckConfigAndFourCCUsage();
	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundPlayInitDuringPlayInit::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundPlayInitDuringPlayInit"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				if(iConfig)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					TMMFCapabilities capabilities;
					capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
					capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

					TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));
					if (errConfig != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with error = %d"), errConfig);
						StopTest(errConfig);
						break;
						}
					}

				// Initialize audio device and starts the playing process
				INFO_PRINTF1(_L("Calling First CMMFDevSound::PlayInitL()"));
				TRAPD(iFirstPlayInitErr, iMMFDevSound->PlayInitL());
				TRAP(iSecondPlayInitErr, iMMFDevSound->PlayInitL());
				if (iFirstPlayInitErr != KErrNone)
					{
					ERR_PRINTF2(_L("First CMMFDevSound::PlayInitL() left with error = %d"), iFirstPlayInitErr);
					StopTest(iFirstPlayInitErr);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

/*
 *
 * BufferToBeFilled
 *
 */
void RStepA3FDevSoundPlayInitDuringPlayInit::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		StopTest(KErrGeneral);
		return;
		}
	iBuffer = aBuffer;

	INFO_PRINTF2(_L("First PlayInit left with = %d"), iFirstPlayInitErr);
	INFO_PRINTF2(_L("Second PlayInit left with = %d"), iSecondPlayInitErr);

	if ((iFirstPlayInitErr == KErrNone) && (iSecondPlayInitErr == KErrNotReady))
		{
		INFO_PRINTF1(_L("This is expected"));
		StopTest(KErrNone);
		}
	else
		{
		INFO_PRINTF2(_L("This is not expected. Second PlayInit should fail with  %d"), KErrNotReady);
		StopTest(iFirstPlayInitErr, EFail);
		}

	}

//
// RStepA3FDevSoundVolumeCrop
//

/*
 *
 * RStepA3FDevSoundVolumeCrop - Test step constructor
 *
 */
RStepA3FDevSoundVolumeCrop::RStepA3FDevSoundVolumeCrop(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType),
 iIncreaseMaxVolume(0),
 iMinVolume(0)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundVolumeCrop* RStepA3FDevSoundVolumeCrop::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundVolumeCrop* self = new (ELeave) RStepA3FDevSoundVolumeCrop(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 *
 */
void RStepA3FDevSoundVolumeCrop::KickoffTestL()
	{
	//Create instance of CMMFDevSound
	iMMFDevSound = CMMFDevSound::NewL();
	//Display the name of Codec being used
	DisplayCodecInfo();

	if(!GetBoolFromConfig(iSectName, _L("UseFourCC"), iUseFourCC))
		{
		TPtrC keyName(_L("UseFourCC"));
		ERR_PRINTF3(_L("Error in getting UseFourCC flag from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	TBool maxVol = GetIntFromConfig(iSectName,_L("IncreaseMaxVolume"),iIncreaseMaxVolume);
	if (maxVol)
		{
		INFO_PRINTF2(_L("Volume to be added to the MaxVolume supported by device = %d"), iIncreaseMaxVolume);
		}
	else
		{
		TPtrC keyName(_L("IncreaseMaxVolume"));
		ERR_PRINTF3(_L("Error in getting the volume to increase maxvolume by from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	TBool minVol = GetIntFromConfig(iSectName,_L("SetMinVolume"),iMinVolume);
	if (minVol)
		{
		INFO_PRINTF2(_L("Seting MinVolume value = %d"), iMinVolume);
		}
	else
		{
		TPtrC keyName(_L("SetMinVolume"));
		ERR_PRINTF3(_L("Error in getting volume below zero from config file in %S section via the %S key field."), &iSectName, &keyName);
		return;
		}

	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundVolumeCrop::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundVolumeCrop"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					TInt deviceMaxVol = iMMFDevSound->MaxVolume();
					INFO_PRINTF2(_L("Maximum volume supported by the device is %d"), deviceMaxVol);
					INFO_PRINTF2(_L("Setting the volume beyond the maximum volume supported by the device: %d"), deviceMaxVol + iIncreaseMaxVolume);
					iMMFDevSound->SetVolume(deviceMaxVol + iIncreaseMaxVolume);
					INFO_PRINTF1(_L("Getting the volume set"));
					TInt maxVolumeObtained = iMMFDevSound->Volume();
					INFO_PRINTF2(_L("iMMFDevSound->Volume(): %d"), maxVolumeObtained);
					if (maxVolumeObtained != deviceMaxVol)
						{
						ERR_PRINTF1(_L("Volume was not cropped to equal to maximum volume supported by device"));
						StopTest(KErrGeneral);
						}
					else
						{
						INFO_PRINTF1(_L("Volume was cropped to equal to maximum volume supported by device"));
						INFO_PRINTF2(_L("Setting volume to less than zero: %d"), iMinVolume);
						iMMFDevSound->SetVolume(iMinVolume);
						INFO_PRINTF1(_L("Getting the volume set"));
						TInt minVolumeObtained = iMMFDevSound->Volume();
						INFO_PRINTF2(_L("iMMFDevSound->Volume(): %d"), minVolumeObtained);
						if (minVolumeObtained != KNULLVolume)
							{
							ERR_PRINTF1(_L("Volume was not cropped to equal to minimum volume supported by device"));
							StopTest(KErrGeneral);
							}
						else
							{
							INFO_PRINTF1(_L("Volume was cropped to equal to minimum volume supported by device"));
							StopTest(KErrNone);
							}
						}
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
		break;
		}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundInitializeForConverting
//

/*
 *
 * RStepA3FDevSoundInitializeForConverting - Test step constructor
 *
 */
RStepA3FDevSoundInitializeForConverting::RStepA3FDevSoundInitializeForConverting(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundInitializeForConverting* RStepA3FDevSoundInitializeForConverting::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundInitializeForConverting* self = new (ELeave) RStepA3FDevSoundInitializeForConverting(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundInitializeForConverting::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	//Display the name of Codec being used
	DisplayCodecInfo();

	if(!GetBoolFromConfig(iSectName, _L("UseFourCC"), iUseFourCC))
		{
		TPtrC keyName(_L("UseFourCC"));
		ERR_PRINTF3(_L("Error in getting UseFourCC flag value from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundInitializeForConverting::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInitializeForConverting"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStateConverting));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStateConverting));
					}
				if (err == KErrNotSupported)
					{
					INFO_PRINTF2(_L("CMMFDevSound::InitializeL left with expected error = %d for EMMFStateConverting"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d for EMMFStateConverting"), err);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		}

	}

//
// RStepA3FDevSoundGetSampleNumsAfterStop
//

/*
 *
 * RStepA3FDevSoundGetSampleNumsAfterStop - Test step constructor
 *
 */
RStepA3FDevSoundGetSampleNumsAfterStop::RStepA3FDevSoundGetSampleNumsAfterStop(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType),
 iCheckTimer(ETrue)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundGetSampleNumsAfterStop* RStepA3FDevSoundGetSampleNumsAfterStop::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundGetSampleNumsAfterStop* self = new (ELeave) RStepA3FDevSoundGetSampleNumsAfterStop(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundGetSampleNumsAfterStop::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundGetSampleNumsAfterStop"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				if(iConfig)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					TMMFCapabilities capabilities;
					capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
					capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

					TRAPD(errConfig, iMMFDevSound->SetConfigL(capabilities));
					if (errConfig != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL() left with error = %d"), errConfig);
						StopTest(errConfig);
						break;
						}
					}
				// Initialize audio device and starts the playing process
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(errPlay, iMMFDevSound->PlayInitL());
				if (errPlay != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left with error = %d"), errPlay);
					StopTest(errPlay);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStatePlaying:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundGetSampleNumsAfterStop"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBF)
				{
				// Fill the buffer with audio data and play the data in the buffer
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
					iBuffer->SetLastBuffer(ETrue);
					}

				if (iCheckTimer)
					{
					iTimer->Start(KMicroSecsInTenSecs, 0, iCallBack);
					}
				iCheckTimer = EFalse;
				// Playing data in the buffer at the current volume
				iMMFDevSound->PlayData();
				iBufferCount++;
				}
			else if (aDevSoundEvent == EEventPlayTimerComplete)
				{
				iMMFDevSound->Stop();
				iPlayState = EStatePlayStopped;
				INFO_PRINTF1(_L("EStatePlayStopped"));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		case EStatePlayStopped:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundGetSampleNumsAfterStop"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStopped)
				{
				INFO_PRINTF1(_L("Getting SamplesPlayed after Stop"));
				TInt numSamplesStop = iMMFDevSound->SamplesPlayed();
				INFO_PRINTF2(_L("SamplesPlayed after Stop: %d"), numSamplesStop);
				if (numSamplesStop == KNULLSamples)
					{
					ERR_PRINTF2(_L("Number of samples played after stop should not be %d"), KNULLSamples );
					StopTest(KErrGeneral);
					}
				else
					{
					INFO_PRINTF2(_L("Number of samples played after stop is greater than %d"), KNULLSamples );
					StopTest(KErrNone);
					}
				}
			else
				{
				ERR_PRINTF2(_L("Invalid DevSound event received: %d"), aDevSoundEvent);
				StopTest(KErrGeneral);
				}
			}
			break;

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

/*
 *
 * DoTimerCallback
 *
 */
void RStepA3FDevSoundGetSampleNumsAfterStop::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	if(!iCheckTimer)
		{
		Fsm(EEventPlayTimerComplete, KErrNone);
		Fsm(EEventPlayStopped, KErrNone);
		}

	}

//
// RStepA3FDevSoundCancelInitializePlay
//

/*
 *
 * RStepA3FDevSoundCancelInitializePlay - Test step constructor
 *
 */
RStepA3FDevSoundCancelInitializePlay::RStepA3FDevSoundCancelInitializePlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundCancelInitializePlay* RStepA3FDevSoundCancelInitializePlay::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundCancelInitializePlay* self = new (ELeave) RStepA3FDevSoundCancelInitializePlay(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundCancelInitializePlay::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iCallBack = TCallBack(TimerCallback, this);
	
	//Display the name of Codec being used
	DisplayCodecInfo();
	CheckConfigAndFourCCUsage();

	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}
	
/*
 *
 * InitializeComplete
 *
 */
void RStepA3FDevSoundCancelInitializePlay::InitializeComplete(TInt aError)
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
void RStepA3FDevSoundCancelInitializePlay::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	INFO_PRINTF1(_L("InitializeComplete callback was not received. This is the expected behaviour"));
	INFO_PRINTF1(_L("Use SetConfigL to verify that DevSound has not been initialised"));
	SetSampleRateAndChannelMode();
	TMMFCapabilities capabilities;
	capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
	capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);

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
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundCancelInitializePlay::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundCancelInitializePlay"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF1(_L("State: EStatePlayInitializing"));
					iPlayState = EStatePlayInitializing;					
					INFO_PRINTF1(_L("Calling CMMFDevSound::CancelInitialize()"));
					iMMFDevSound->CancelInitialize();
					}
					INFO_PRINTF1(_L("Start timer to wait for InitializeComplete"));
					iTimer->Start(KMicroSecsInTenSecs, 0, iCallBack);	
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundNegCancelInitializePlay
//

/*
 *
 * RStepA3FDevSoundNegCancelInitializePlay - Test step constructor
 *
 */
RStepA3FDevSoundNegCancelInitializePlay::RStepA3FDevSoundNegCancelInitializePlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundNegCancelInitializePlay* RStepA3FDevSoundNegCancelInitializePlay::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundNegCancelInitializePlay* self = new (ELeave) RStepA3FDevSoundNegCancelInitializePlay(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundNegCancelInitializePlay::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	//Display the name of Codec being used
	DisplayCodecInfo();
	if(!GetBoolFromConfig(iSectName, _L("UseFourCC"), iUseFourCC))
		{
		TPtrC keyName(_L("UseFourCC"));
		ERR_PRINTF3(_L("Error in getting UseFourCC flag value from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}
	
/*
 *
 * InitializeComplete
 *
 */
void RStepA3FDevSoundNegCancelInitializePlay::InitializeComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		INFO_PRINTF1(_L("InitializeComplete returned with KErrNone"));
		INFO_PRINTF1(_L("Calling CancelInitialize after InitializeComplete"));
		TInt err;
		err = iMMFDevSound->CancelInitialize();
		if (err == KErrNotReady)
			{
			INFO_PRINTF1(_L("CancelInitialize returned with KErrNotReady as expected"));
			StopTest(aError);
			}
		else
			{
			INFO_PRINTF2(_L("CancelInitialize return with error %d instead of KErrNotReady as expected"), aError);
			StopTest(aError, EFail);
			}
		}
	else
		{
		INFO_PRINTF2(_L("InitializeComplete returned with error %d"), aError);
		StopTest(aError, EFail);
		}
	
	}

/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundNegCancelInitializePlay::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundNegCancelInitializePlay"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;					
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}

		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundEmptyBuffersInCreatedState
//

/*
 *
 * RStepA3FDevSoundEmptyBuffersInCreatedState - Test step constructor
 *
 */
RStepA3FDevSoundEmptyBuffersInCreatedState::RStepA3FDevSoundEmptyBuffersInCreatedState(const TDesC& aTestName)
:RTestStepA3FDevSoundPlayBase(aTestName)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundEmptyBuffersInCreatedState* RStepA3FDevSoundEmptyBuffersInCreatedState::NewL(const TDesC& aTestName)
	{
	RStepA3FDevSoundEmptyBuffersInCreatedState* self = new (ELeave) RStepA3FDevSoundEmptyBuffersInCreatedState(aTestName);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundEmptyBuffersInCreatedState::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();
	
	//Display the name of Codec being used
	DisplayCodecInfo();

	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}
	
/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundEmptyBuffersInCreatedState::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundEmptyBuffersInCreatedState"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Calling EmptyBuffers
				INFO_PRINTF1(_L("Calling CMMFDevSound::EmptyBuffers()"));
				TInt err;
				err = iMMFDevSound->EmptyBuffers();
				if (err == KErrNotReady)
					{
					ERR_PRINTF2(_L("CMMFDevSound::EmptyBuffers() returned expected error = %d"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::EmptyBuffers() did not return KErrNotReady as expected. Returned with unexpected error = %d"), err);
					StopTest(err);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}

	}

//
// RStepA3FDevSoundEmptyBuffersInInitializedState
//

/*
 *
 * RStepA3FDevSoundEmptyBuffersInCreatedState - Test step constructor
 *
 */
RStepA3FDevSoundEmptyBuffersInInitializedState::RStepA3FDevSoundEmptyBuffersInInitializedState(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundEmptyBuffersInInitializedState* RStepA3FDevSoundEmptyBuffersInInitializedState::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundEmptyBuffersInInitializedState* self = new (ELeave) RStepA3FDevSoundEmptyBuffersInInitializedState(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundEmptyBuffersInInitializedState::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();
	
	//Display the name of Codec being used
	DisplayCodecInfo();
	CheckConfigAndFourCCUsage();

	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}
	
/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundEmptyBuffersInInitializedState::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundEmptyBuffersInCreatedState"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlayInitializing"));
				iPlayState = EStatePlayInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlayInitializing:
			{
			if (aDevSoundEvent == EEventInitCompletePlay && aError == KErrNone)
				{
				// Calling EmptyBuffers
				INFO_PRINTF1(_L("Calling CMMFDevSound::EmptyBuffers()"));
				TInt err;
				err = iMMFDevSound->EmptyBuffers();
				if (err == KErrNotReady)
					{
					ERR_PRINTF2(_L("CMMFDevSound::EmptyBuffers() returned expected error = %d"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::EmptyBuffers() did not return KErrNotReady as expected. Returned with unexpected error = %d"), err);
					StopTest(err);
					}
				}
			else if (aDevSoundEvent == EEventInitCompletePlay && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompletePlay not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}
	}

//
// RStepA3FDevSoundEmptyBuffersInInitializingState
//

/*
 *
 * RStepA3FDevSoundCancelInitializePlay - Test step constructor
 *
 */
RStepA3FDevSoundEmptyBuffersInInitializingState::RStepA3FDevSoundEmptyBuffersInInitializingState(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
:RTestStepA3FDevSoundPlayBase(aTestName, aSectName, aInputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundEmptyBuffersInInitializingState* RStepA3FDevSoundEmptyBuffersInInitializingState::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType)
	{
	RStepA3FDevSoundEmptyBuffersInInitializingState* self = new (ELeave) RStepA3FDevSoundEmptyBuffersInInitializingState(aTestName, aSectName, aInputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundEmptyBuffersInInitializingState::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();
	
	//Display the name of Codec being used
	DisplayCodecInfo();
	CheckConfigAndFourCCUsage();

	Fsm(EEventPlayStart,KErrNone); // call to start the DevSound finite state machine for playing

	}
	
/*
 *
 * InitializeComplete
 *
 */
void RStepA3FDevSoundEmptyBuffersInInitializingState::InitializeComplete(TInt aError)
	{
	INFO_PRINTF1(_L("InitializeComplete callback was received. This is not the expected behaviour"));
	INFO_PRINTF1(_L("EmptyBuffers was not called before InitializeComplete "));
	StopTest(aError, EFail);
	}
	
/*
 *
 * Fsm
 *	- Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundEmptyBuffersInInitializingState::Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundCancelInitializePlay"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TInt err;
				if(iUseFourCC)
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
					}
				else
					{
					TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF1(_L("State: EStatePlayInitializing"));
					iPlayState = EStatePlayInitializing;					
					INFO_PRINTF1(_L("Calling CMMFDevSound::EmptyBuffers()"));
					TInt err;
					err = iMMFDevSound->EmptyBuffers();
					if (err == KErrNotReady)
						{
						ERR_PRINTF2(_L("CMMFDevSound::EmptyBuffers() returned expected error = %d"), err);
						StopTest();
						}
					else
						{
						ERR_PRINTF2(_L("CMMFDevSound::EmptyBuffers() did not return KErrNotReady as expected. Returned with unexpected error = %d"), err);
						StopTest(err);
						}
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventPlayStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iPlayState);
			StopTest(KErrGeneral);
			}
		}
	}
