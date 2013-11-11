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

// User includes
#include "char_a3f_devsound_playtest.h"

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0001
 */
RA3FDevSoundPlaySetConfigTest::RA3FDevSoundPlaySetConfigTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iSampleRate(0),
	iChannels(0)
	{
	}

RA3FDevSoundPlaySetConfigTest* RA3FDevSoundPlaySetConfigTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetConfigTest* self = new (ELeave) RA3FDevSoundPlaySetConfigTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetConfigTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KSampleRate, iSampleRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KChannel, iChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlaySetConfigTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetConfigLTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFCapabilities capabilitiesSet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
				TBuf<KMaxChannelsStringLength> stringChannelsSet;
				SampleRateFromTIntToTMMFSampleRate (iSampleRate, iESampleRate);
				capabilitiesSet.iRate = iESampleRate;
				capabilitiesSet.iChannels = iChannels;
				SampleRateFromTUintToString (capabilitiesSet.iRate,	stringSampleRateSet);
				ChannelsFromTUintToString (capabilitiesSet.iChannels, stringChannelsSet);
				INFO_PRINTF3(_L("Settings for Sample rate an channel mode : %S %S"), &stringSampleRateSet, &stringChannelsSet);
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetConfigL"));
				TRAPD(err, iMMFDevSound->SetConfigL(capabilitiesSet));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::SetConfigL left with error = %d"), err);
					StopTest(err);
					break;
					}
				TMMFCapabilities capabilitiesGet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateGet;
				TBuf<KMaxChannelsStringLength> stringChannelsGet;
				capabilitiesGet=iMMFDevSound->Config ();
				SampleRateFromTUintToString (capabilitiesGet.iRate, stringSampleRateGet);
				ChannelsFromTUintToString (capabilitiesGet.iChannels, stringChannelsGet);
				if (capabilitiesGet.iRate != capabilitiesSet.iRate || capabilitiesGet.iChannels != capabilitiesSet.iChannels)
					{
					ERR_PRINTF3(_L("Retrieved sample rate and channel : %S %S"), &stringSampleRateGet, &stringChannelsGet);
					ERR_PRINTF3(_L("Expected %S %S"), &stringSampleRateSet, &stringChannelsSet);
					StopTest(aError, EFail);
					break;
					}
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error : %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0002
 */
RA3FDevSoundPlaySetVolumeWhilePlayingTest::RA3FDevSoundPlaySetVolumeWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iVolume(0),
	iFilename(KNullDesC)
	{
	}

RA3FDevSoundPlaySetVolumeWhilePlayingTest* RA3FDevSoundPlaySetVolumeWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetVolumeWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlaySetVolumeWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetVolumeWhilePlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest (KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy (filename);
	TInt err = iFile.Open (iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest (KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlaySetVolumeWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetVolumePlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error: %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with: %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize ())
					{
					iBuffer->SetLastBuffer (ETrue);
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData ();
				}

			else if (aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetVolume"));
				iMMFDevSound->SetVolume(iVolume);
				TInt volumeGet;
				volumeGet=iMMFDevSound->Volume();
				if (volumeGet == iVolume)
					{
					INFO_PRINTF2(_L("CMMFDevSound::SetVolume during playback succeeded with %d"), volumeGet);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::SetVolume during playback failed with %d"), volumeGet);
					ERR_PRINTF2(_L("Expected value = %d"), iVolume);
					StopTest(aError, EFail);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlaySetVolumeWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm (EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0003
 */
RA3FDevSoundPlaySetVolumeTest::RA3FDevSoundPlaySetVolumeTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iVolume(0)
	{
	}

RA3FDevSoundPlaySetVolumeTest* RA3FDevSoundPlaySetVolumeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetVolumeTest* self = new (ELeave) RA3FDevSoundPlaySetVolumeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetVolumeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlaySetVolumeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetVolumeTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetVolume"));
				iMMFDevSound->SetVolume(iVolume);
				TInt volumeGet;
				volumeGet = iMMFDevSound->Volume();
				if (volumeGet == iVolume)
					{
					INFO_PRINTF2(_L("CMMFDevSound::SetVolume succeeded with %d"), volumeGet);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::SetVolume failed with %d"), volumeGet);
					ERR_PRINTF2(_L("Expected value = %d"), iVolume);
					StopTest(aError, EFail);
					}
				}
		else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0004
 */
RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest::RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC),
	iVolume(0)
	{
	}

RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest* RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest* self = new (ELeave) RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TRAPD(err, iMMFDevSound->PlayInitL());
				if ( err != KErrNone)
					{
					StopTest (err);
					break;
					}
				StartTimer (KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if ( aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest (aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer (ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetVolume"));
				iMMFDevSound->SetVolume(iVolume);
				TInt volumeGet;
				volumeGet=iMMFDevSound->Volume ();
				if (volumeGet == iVolume)
					{
					INFO_PRINTF2(_L("CMMFDevSound::SetVolume succeeded with %d"), volumeGet);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::SetVolume failed with %d"), volumeGet);
					ERR_PRINTF2(_L("Expected volume: %d"), iVolume);
					StopTest(aError, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm (EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0005
 */
RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest::RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), 
	iReinitialized(EFalse), iVolume(0)
	{
	}

RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest* RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest	* self = new (ELeave) RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest (KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC (fourccCode);
	}

void RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetVolume"));
				iMMFDevSound->SetVolume(iVolume);
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if (!iReinitialized)
					{
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
					if (err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					TInt volumeGet;
					volumeGet=iMMFDevSound->Volume();
					if (volumeGet == iVolume)
						{
						INFO_PRINTF2(_L("CMMFDevSound::SetVolume is kept after reinitialise Devsound. Volume = %d "), volumeGet);
						StopTest();
						}
					else
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetVolume was not kept. Volume = %d"), volumeGet);
						ERR_PRINTF2(_L("Expected value = %d"),iVolume);
						StopTest(aError, EFail);
						}
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
					StopTest (aError);
					}
				else
					{
					ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
					StopTest (aError, EFail);
					}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest (aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0006
 */
RA3FDevSoundPlaySetVolumeRampWhilePlayingTest::RA3FDevSoundPlaySetVolumeRampWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC),
	iVolumeRamp(0), iPlayInitTwice(EFalse)
	{
	}

RA3FDevSoundPlaySetVolumeRampWhilePlayingTest* RA3FDevSoundPlaySetVolumeRampWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetVolumeRampWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlaySetVolumeRampWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetVolumeRampWhilePlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest (KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open (iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	if (!GetIntFromConfig(iTestStepName, KVolumeRamp, iVolumeRamp))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolumeRamp);
		StopTest(KErrNotFound);
		return;
		}

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL (CActive::EPriorityHigh);
	}

void RA3FDevSoundPlaySetVolumeRampWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetVolumePlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest (err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if ( aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest (aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer (ETrue);
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData ();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetVolumeRamp"));
				iMMFDevSound->SetVolumeRamp (iVolumeRamp*KMicroSecsInOneSec);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlaySetVolumeRampWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

void RA3FDevSoundPlaySetVolumeRampWhilePlayingTest::PlayError(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound PlayError() callback =========="));
	INFO_PRINTF3(KMsgErrorDevSoundCallback, &KPlayErrorText, aError);
	if (iBuffer->LastBuffer ()&& aError == KErrUnderflow)
		{
		TInt filePosition = 0;
		TInt err = iFile.Seek (ESeekStart, filePosition);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("iFile.Seek() returned %d"), err);
			StopTest(err);
			}
		else
			{
			if (!iPlayInitTwice)
				{
				INFO_PRINTF1(_L("Playback completed normally"));
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Calling again CMMFDevSound::PlayInitL left with error = %d"), err);
					StopTest(err);
					}
				iPlayInitTwice=ETrue;
				}
			else
				{
				StopTest(err);
				}
			}
		}
	else
		{
		INFO_PRINTF1(_L("Playback completed with error"));
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0007
 */
RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest::RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest* RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest* self = new (ELeave) RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC (fourccCode);
	}

void RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetVolume()to set volume values beyond CMMFDevSound::MaxVolume()"));
				TInt maxVolume = iMMFDevSound->MaxVolume();
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume()+ KExtraVolume);
				TInt volumeGet = iMMFDevSound->Volume();
				if (volumeGet == maxVolume)
					{
					INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolume to set volume beyond CMMFDevSound::MaxVolume succeeded with %d"), volumeGet);
					StopTest();
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::SetVolume to set volume beyond CMMFDevSound::MaxVolume failed with %d"), volumeGet);
					ERR_PRINTF2(_L("Expected value = %d"), maxVolume);
					StopTest(aError, EFail);
					break;
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0008
 */
RA3FDevSoundPlaySetVolumeToNegativeTest::RA3FDevSoundPlaySetVolumeToNegativeTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName),
	iVolume(0)
	{
	}

RA3FDevSoundPlaySetVolumeToNegativeTest* RA3FDevSoundPlaySetVolumeToNegativeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetVolumeToNegativeTest* self = new (ELeave) RA3FDevSoundPlaySetVolumeToNegativeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetVolumeToNegativeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest (KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC (fourccCode);
	}

void RA3FDevSoundPlaySetVolumeToNegativeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetVolumeToNegativeTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolume to set volume value to a negative: %d"), iVolume);
				iMMFDevSound->SetVolume (iVolume);
				if (iMMFDevSound->Volume ()== 0)
					{
					INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolume succeeded with %d"), iMMFDevSound->Volume());
					StopTest();
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::SetVolume failed with %d"), iMMFDevSound->Volume());
					ERR_PRINTF2(_L("Expected value = %d"), 0);
					StopTest (aError, EFail);
					break;
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest (aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest (aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0009
 */
RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest::RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), 
	iLSpeakerBalance(0), iRSpeakerBalance(0)
	{
	}

RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest* RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest* self = new (ELeave) RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KLSpeakerBalance, iLSpeakerBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KLSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KRSpeakerBalance, iRSpeakerBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetPlayBalanceL using left = %d right = %d values"), iLSpeakerBalance, iRSpeakerBalance);
				TRAPD(err, iMMFDevSound->SetPlayBalanceL(iLSpeakerBalance, iRSpeakerBalance));
				if (err == KErrNone)
					{
					TInt getBalanceL, getBalanceR;
					TRAPD(err, iMMFDevSound->GetPlayBalanceL(getBalanceL, getBalanceR));
					if (err == KErrNone)
						{
						if (getBalanceL == KMaxSpeakerBalanceValue && getBalanceR == KMaxSpeakerBalanceValue)
							{
							INFO_PRINTF3(_L("Calling CMMFDevSound::SetPlayBalanceL to set balance values beyond 100 succeeded, left = %d right = %d values"), getBalanceL, getBalanceR);
							StopTest();
							}
						else
							{
							ERR_PRINTF3(_L("Calling CMMFDevSound::SetPlayBalanceL to set balance values beyond 100 failed, left = %d right = %d values"), getBalanceL, getBalanceR);
							ERR_PRINTF3(_L("Expected values left = %d right = %d values"), KMaxSpeakerBalanceValue, KMaxSpeakerBalanceValue);
							StopTest (err, EFail);
							}
						}
					else
						{
						ERR_PRINTF1(_L("Error in getting left and right play balance."));
						StopTest (err);
						}
					}
				else
					{
					ERR_PRINTF1(_L("Error in setting left and right play balance."));
					StopTest (err);
					}
				}
			else if ( aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest (aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0010
 */
RA3FDevSoundPlaySetPlayBalanceToNegativeTest::RA3FDevSoundPlaySetPlayBalanceToNegativeTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), 
	iLSpeakerBalance(0), iRSpeakerBalance(0)
	{
	}

RA3FDevSoundPlaySetPlayBalanceToNegativeTest* RA3FDevSoundPlaySetPlayBalanceToNegativeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetPlayBalanceToNegativeTest* self = new (ELeave) RA3FDevSoundPlaySetPlayBalanceToNegativeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetPlayBalanceToNegativeTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KLSpeakerBalance, iLSpeakerBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KLSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KRSpeakerBalance, iRSpeakerBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC (fourccCode);
	}

void RA3FDevSoundPlaySetPlayBalanceToNegativeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetPlayBalanceToNegativeTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetPlayBalanceL using left = %d right = %d values"), iLSpeakerBalance, iRSpeakerBalance);
				TRAPD(err, iMMFDevSound->SetPlayBalanceL(iLSpeakerBalance, iRSpeakerBalance));
				if (err == KErrNone)
					{
					TInt getBalanceL, getBalanceR;
					TRAPD(err, iMMFDevSound->GetPlayBalanceL(getBalanceL, getBalanceR));
					if (err == KErrNone)
						{
						if (getBalanceL == KMinSpeakerBalanceValue && getBalanceR == KMinSpeakerBalanceValue)
							{
							INFO_PRINTF3(_L("Calling CMMFDevSound::SetPlayBalanceL to set balance values to negative succeeded, left = %d right = %d values"), getBalanceL, getBalanceR);
							StopTest();
							}
						else
							{
							ERR_PRINTF3(_L("Calling CMMFDevSound::SetPlayBalanceL to set balance values to negative failed, left = %d right = %d values"), getBalanceL, getBalanceR);
							ERR_PRINTF3(_L("Expected values, left = %d right = %d values"), KMinSpeakerBalanceValue, KMinSpeakerBalanceValue);
							StopTest(err, EFail);
							}
						}
					else
						{
						ERR_PRINTF1(_L("Error in getting left and right play balance."));
						StopTest (err);
						}
					}
				else
					{
					ERR_PRINTF1(_L("Error in setting left and right play balance."));
					StopTest (err);
					}
				}
			else if ( aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest (aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest (aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0011
 */
RA3FDevSoundPlaySetPrioritySettingsTest::RA3FDevSoundPlaySetPrioritySettingsTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iPriority(0)
	{
	}

RA3FDevSoundPlaySetPrioritySettingsTest* RA3FDevSoundPlaySetPrioritySettingsTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySetPrioritySettingsTest* self = new (ELeave) RA3FDevSoundPlaySetPrioritySettingsTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySetPrioritySettingsTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KPriority, iPriority))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KPriority);
		StopTest(KErrNotFound);
		return;
		}

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlaySetPrioritySettingsTest::Fsm(
		TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetPrioritySettingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFPrioritySettings priority;
				priority.iPriority = iPriority;
				INFO_PRINTF2(_L("Setting Priority to %d calling  CMMFDevSound::SetPrioritySettings"), iPriority);
				iMMFDevSound->SetPrioritySettings(priority);
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if ( aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest (aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0012
 */
RA3FDevSoundPlayCapabilitiesTest::RA3FDevSoundPlayCapabilitiesTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayCapabilitiesTest* RA3FDevSoundPlayCapabilitiesTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayCapabilitiesTest* self = new (ELeave) RA3FDevSoundPlayCapabilitiesTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayCapabilitiesTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExCapChannels, iExCapChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExCapChannels);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExCapRate, iExCapRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExCapRate);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC (fourccCode);
	}

void RA3FDevSoundPlayCapabilitiesTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayCapabilitiesTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Capabilities"));
				TMMFCapabilities capabilitiesGet;
				capabilitiesGet = iMMFDevSound->Capabilities ();

				INFO_PRINTF3(_L("Rate Sum:0x%x Channels Sum:0x%x"), capabilitiesGet.iRate, capabilitiesGet.iChannels);
				PrintSupportedCapabilities (capabilitiesGet.iRate, capabilitiesGet.iChannels);
				if(capabilitiesGet.iRate == iExCapRate && capabilitiesGet.iChannels == iExCapChannels)
					{
					INFO_PRINTF3(_L("Capabilities returned match expected capabilities which are Sum:0x%x Channels Sum:0x%x"), capabilitiesGet.iRate, capabilitiesGet.iChannels);
					StopTest ();
					}
				else
					{
					ERR_PRINTF1(_L("Capabilities returned do not match the expected capabilities"));
					ERR_PRINTF3(_L("Expected Rate Sum:0x%x Channels Sum:0x%x"), iExCapRate, iExCapChannels);
					StopTest (KErrNone, EFail);
					}
				}
			else if ( aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest (aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest (aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0013
 */
RA3FDevSoundPlayConfigTest::RA3FDevSoundPlayConfigTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName),iExCapRate(0),
	iExCapChannels(0)
	{
	}

RA3FDevSoundPlayConfigTest* RA3FDevSoundPlayConfigTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayConfigTest* self = new (ELeave) RA3FDevSoundPlayConfigTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayConfigTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExCapRate, iExCapRate))
				{
				ERR_PRINTF2(KMsgErrorGetParameter, &KExCapRate);
				StopTest(KErrNotFound);
				return;
				}
		if (!GetIntFromConfig(iTestStepName, KExCapChannels, iExCapChannels))
			{
			ERR_PRINTF2(KMsgErrorGetParameter, &KExCapChannels);
			StopTest(KErrNotFound);
			return;
			}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC (fourccCode);
	}

void RA3FDevSoundPlayConfigTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayConfigTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFCapabilities capabilitiesGet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateGet;
				TBuf<KMaxChannelsStringLength> stringChannelsGet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateExpected;
				TBuf<KMaxChannelsStringLength> stringChannelsExpected;
				INFO_PRINTF1(_L("Calling CMMFDevSound::Config while playing"));
				capabilitiesGet=iMMFDevSound->Config ();
				SampleRateFromTUintToString (capabilitiesGet.iRate, stringSampleRateGet);
				ChannelsFromTUintToString (capabilitiesGet.iChannels, stringChannelsGet);
				SampleRateFromTUintToString (capabilitiesGet.iRate, stringSampleRateExpected);
				ChannelsFromTUintToString (capabilitiesGet.iChannels, stringChannelsExpected);
				if (stringSampleRateExpected.Compare(stringSampleRateGet) == 0)
					{
					INFO_PRINTF2(_L("Sample rate value match default as expected with %S"), &stringSampleRateGet);
					if (stringChannelsExpected.Compare(stringChannelsGet) == 0)
						{
						INFO_PRINTF2(_L("Channel mode value match default as expected with %S"), &stringChannelsGet);
						StopTest();
						}
					else
						{
						INFO_PRINTF2(_L("Channel mode value failed with %S"), &stringChannelsGet);
						ERR_PRINTF2(_L("Expected channel mode %S"), &stringChannelsExpected);
						StopTest(KErrNone, EFail);
						}
					}
				else
					{
					ERR_PRINTF2(_L("Sample rate value failed with %S"), &stringSampleRateGet);
					ERR_PRINTF2(_L("Expected sample rate %S"), &stringSampleRateExpected);
					StopTest(KErrNone, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0014
 */
RA3FDevSoundPlayGetPlayBalanceTest::RA3FDevSoundPlayGetPlayBalanceTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName),
	iExLSpeakerBalance(0),
	iExRSpeakerBalance(0)
	{
	}

RA3FDevSoundPlayGetPlayBalanceTest* RA3FDevSoundPlayGetPlayBalanceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayGetPlayBalanceTest* self = new (ELeave) RA3FDevSoundPlayGetPlayBalanceTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayGetPlayBalanceTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExLSpeakerBalance, iExLSpeakerBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExLSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExRSpeakerBalance, iExRSpeakerBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExRSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayGetPlayBalanceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayGetPlayBalanceTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TInt getBalanceL, getBalanceR;
				INFO_PRINTF1(_L("Calling CMMFDevSound::GetPlayBalance"));
				TRAPD(err, iMMFDevSound->GetPlayBalanceL(getBalanceL, getBalanceR));
				if (err == KErrNone)
					{
					if(getBalanceL == iExLSpeakerBalance && getBalanceR == iExRSpeakerBalance)
						{
						INFO_PRINTF3(_L("Balance values match expected values which are, right = %d, left = %d"), getBalanceL, getBalanceR);
						StopTest();
						}
					else
						{
						ERR_PRINTF3(_L("Balance values do not match expected values, left = %d right = %d"), getBalanceL, getBalanceR);
						ERR_PRINTF3(_L("Expected values, left = %d right = %d"), iExLSpeakerBalance, iExRSpeakerBalance);
						StopTest(KErrNone, EFail);
						}
					}
				else
					{
					ERR_PRINTF1(_L("Error in getting left and right play balance."));
					StopTest(err);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0015
 */
RA3FDevSoundPlayGetSupportedInputDataTypesLTest::RA3FDevSoundPlayGetSupportedInputDataTypesLTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName),
	iCurrentTypeInTest(0)
	{
	}

RA3FDevSoundPlayGetSupportedInputDataTypesLTest* RA3FDevSoundPlayGetSupportedInputDataTypesLTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayGetSupportedInputDataTypesLTest* self = new (ELeave) RA3FDevSoundPlayGetSupportedInputDataTypesLTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayGetSupportedInputDataTypesLTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayGetSupportedInputDataTypesLTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayGetSupportedInputDataTypesLTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(iCurrentTypeInTest == 0)
					{
					TMMFPrioritySettings prioritySettings;
					INFO_PRINTF1(_L("Calling CMMFDevSound::GetSupportedInputDataTypes"));
					TRAPD(err,iMMFDevSound->GetSupportedInputDataTypesL(iSupportedDataTypes, prioritySettings));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::GetSupportedInputDataTypes data types falied ! Left with error = %d"), err);
						StopTest (err);
						break;
						}
					for(TInt x = 0; x < iSupportedDataTypes.Count (); x++)
						{
						TFourCC fourCC = iSupportedDataTypes[x];
						TBuf<KTFourCC> name;
						for (TInt i=0; i<= KFourCCMaxBitDisplacement; i+=K8Bits)
							{
							name.Append ( (TUint8)(fourCC.FourCC() >> i));
							}
						INFO_PRINTF3(_L("Supported Input Data types: 0x%x  %S "), fourCC.FourCC(), &name);
						}
					}
				
				// Complete test when all types have been used of initialization
				if(iCurrentTypeInTest == iSupportedDataTypes.Count())
					{
					INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
					StopTest();
					}
				else
					{
					TFourCC fourCC = iSupportedDataTypes[iCurrentTypeInTest];
					TBuf<KTFourCC> name;
					for (TInt i=0; i<= KFourCCMaxBitDisplacement; i+=K8Bits)
						{
						name.Append ( (TUint8)(fourCC.FourCC() >> i));
						}
					INFO_PRINTF3(_L("Initializing for Input Data type: 0x%x  %S "), fourCC.FourCC(), &name);
					TRAPD(err, iMMFDevSound->InitializeL(*this, fourCC, EMMFStatePlaying));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
						StopTest(err);
						break;
						}
					}
				iCurrentTypeInTest++;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0016
 */
RA3FDevSoundPlayMaxVolumeTest::RA3FDevSoundPlayMaxVolumeTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iExVolume(0)
	{
	}

RA3FDevSoundPlayMaxVolumeTest* RA3FDevSoundPlayMaxVolumeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayMaxVolumeTest* self = new (ELeave) RA3FDevSoundPlayMaxVolumeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayMaxVolumeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC (fourccCode);
	}

void RA3FDevSoundPlayMaxVolumeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayMaxVolumeTest"), EFsmIncorrectErrorPassed));
			if ( aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TInt maxVolume = iMMFDevSound->MaxVolume();
				INFO_PRINTF1(_L("Calling CMMFDevSound::MaxVolume"));
				if(maxVolume == iExVolume)
					{
					INFO_PRINTF2(_L("Calling CMMFDevSound::MaxVolume succeeded with the expected value %d"), maxVolume);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::MaxVolume failed with value %d"), maxVolume);
					ERR_PRINTF2(_L("Expected value = %d"), iExVolume);
					StopTest(KErrNone, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0017
 */
RA3FDevSoundPlaySamplesPlayedTest::RA3FDevSoundPlaySamplesPlayedTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName),
	iExSamplesPlayed(0)
	{
	}

RA3FDevSoundPlaySamplesPlayedTest* RA3FDevSoundPlaySamplesPlayedTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySamplesPlayedTest* self = new (ELeave) RA3FDevSoundPlaySamplesPlayedTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySamplesPlayedTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExSamplesPlayed, iExSamplesPlayed))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExSamplesPlayed);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlaySamplesPlayedTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySamplesPlayedTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::SamplesPlayed"));
				TInt samplesPlayed = iMMFDevSound->SamplesPlayed();
				INFO_PRINTF2(_L("Samples returned by CMMFDevSound::SamplesPlayed = %d"), samplesPlayed);
				if(samplesPlayed == iExSamplesPlayed)
					{
					INFO_PRINTF3(_L("Samples returned by CMMFDevSound::SamplesPlayed: %d matches the expected samples played: %d"), samplesPlayed, iExSamplesPlayed);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Call to CMMFDevSound::SamplesPlayed do not match the expected value = %d"), samplesPlayed);
					ERR_PRINTF2(_L("Expected Samples played value = %d"), iExSamplesPlayed);
					StopTest(KErrNone, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0018
 */
RA3FDevSoundPlayVolumeTest::RA3FDevSoundPlayVolumeTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName),
	iExVolume(0)
	{
	}

RA3FDevSoundPlayVolumeTest* RA3FDevSoundPlayVolumeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayVolumeTest* self = new (ELeave) RA3FDevSoundPlayVolumeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayVolumeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayVolumeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayVolumeTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Volume"));
				TInt volumeGet = iMMFDevSound->Volume();
				INFO_PRINTF2(_L("Volume returned by CMMFDevSound::Volume() = %d"), volumeGet);
				if(volumeGet == iExVolume)
					{
					INFO_PRINTF3(_L("Volume returned by CMMFDevSound::Volume(): %d  matches the default device volume: %d"), volumeGet, iExVolume);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Default device volume failed with %d"), volumeGet);
					ERR_PRINTF2(_L("Expected value = % d"), iExVolume);
					StopTest(aError , EFail);
					}
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0019
 */
RA3FDevSoundPlayCapabilitiesWhilePlayingTest::RA3FDevSoundPlayCapabilitiesWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC),
	iExCapChannels(0), iExCapRate(0)
	{
	}

RA3FDevSoundPlayCapabilitiesWhilePlayingTest* RA3FDevSoundPlayCapabilitiesWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayCapabilitiesWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlayCapabilitiesWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayCapabilitiesWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExCapChannels, iExCapChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExCapChannels);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExCapRate, iExCapRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExCapRate);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC filename;
	// Get the filename of the audio file to play
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open (iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayCapabilitiesWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayCapabilitiesPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer (ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData()"));
				iMMFDevSound->PlayData();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				TMMFCapabilities capabilitiesGet;
				INFO_PRINTF1(_L("Calling CMMFDevSound::Capabilities while playing"));
				capabilitiesGet = iMMFDevSound->Capabilities ();
				INFO_PRINTF3(_L("Rate Sum:0x%x Channels Sum:0x%x"), capabilitiesGet.iRate, capabilitiesGet.iChannels);
				PrintSupportedCapabilities (capabilitiesGet.iRate, capabilitiesGet.iChannels);
				if(capabilitiesGet.iRate == iExCapRate && capabilitiesGet.iChannels == iExCapChannels)
					{
					INFO_PRINTF3(_L("Capabilities returned match expected capabilities which are Sum:0x%x Channels Sum:0x%x"), capabilitiesGet.iRate, capabilitiesGet.iChannels);
					}
				else
					{
					ERR_PRINTF3(_L("Capabilities returned do not match the expected capabilities. The total capabilities Sum:0x%x Channels Sum:0x%x "), capabilitiesGet.iRate, capabilitiesGet.iChannels);
					ERR_PRINTF3(_L("Expected Rate Sum:0x%x Channels Sum:0x%x"), iExCapRate, iExCapChannels);
					StopTest (KErrNone, EFail);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest (aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayCapabilitiesWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0020
 */
RA3FDevSoundPlayConfigWhilePlayingTest::RA3FDevSoundPlayConfigWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC), iExCapRate(0),
	iExCapChannels(0)
	{
	}

RA3FDevSoundPlayConfigWhilePlayingTest* RA3FDevSoundPlayConfigWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayConfigWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlayConfigWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayConfigWhilePlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open (iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	if (!GetIntFromConfig(iTestStepName, KExCapRate, iExCapRate))
			{
			ERR_PRINTF2(KMsgErrorGetParameter, &KExCapRate);
			StopTest(KErrNotFound);
			return;
			}
	if (!GetIntFromConfig(iTestStepName, KExCapChannels, iExCapChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExCapChannels);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayConfigWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayCapabilitiesPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest (err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData()"));
				iMMFDevSound->PlayData ();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				TMMFCapabilities capabilitiesGet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateGet;
				TBuf<KMaxChannelsStringLength> stringChannelsGet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateExpected;
				TBuf<KMaxChannelsStringLength> stringChannelsExpected;
				INFO_PRINTF1(_L("Calling CMMFDevSound::Config while playing"));
				capabilitiesGet=iMMFDevSound->Config ();
				SampleRateFromTUintToString (capabilitiesGet.iRate, stringSampleRateGet);
				ChannelsFromTUintToString (capabilitiesGet.iChannels, stringChannelsGet);
				SampleRateFromTIntToTMMFSampleRate (iExCapRate, iExSampleRate);
				SampleRateFromTUintToString (iExSampleRate, stringSampleRateExpected);
				ChannelsFromTUintToString (iExCapChannels, stringChannelsExpected);
				if (stringSampleRateExpected.Compare(stringSampleRateGet) == 0)
					{
					INFO_PRINTF2(_L("Sample rate value match default as expected with %S"), &stringSampleRateGet);
					if (stringChannelsExpected.Compare(stringChannelsGet) == 0)
						{
						INFO_PRINTF2(_L("Channel mode value match default as expected with %S"), &stringChannelsGet);
						StopTest();
						}
					else
						{
						INFO_PRINTF2(_L("Channel mode value failed with %S"), &stringChannelsGet);
						ERR_PRINTF2(_L("Expected channel mode %S"), &stringChannelsExpected);
						StopTest(KErrNone, EFail);
						}
					}
				else
					{
					ERR_PRINTF2(_L("Sample rate value failed with %S"), &stringSampleRateGet);
					ERR_PRINTF2(_L("Expected sample rate %S"), &stringSampleRateExpected);
					StopTest(KErrNone, EFail);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayConfigWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm (EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0021
 */
RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest::RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), 
	iFilename(KNullDesC),
	iExLSpeakerBalance(0),
	iExRSpeakerBalance(0)
	{
	}

RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest* RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExLSpeakerBalance, iExLSpeakerBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExLSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExRSpeakerBalance, iExRSpeakerBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExRSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy (filename);
	TInt err = iFile.Open (iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayCapabilitiesPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest (err);
					break;
					}
				StartTimer (KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData()"));
				iMMFDevSound->PlayData ();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				TInt getBalanceL, getBalanceR;
				INFO_PRINTF1(_L("Calling CMMFDevSound::GetPlayBalance while playing"));
				TRAPD(err, iMMFDevSound->GetPlayBalanceL(getBalanceL, getBalanceR));
				INFO_PRINTF3(_L("Balance returned by CMMFDevSound::GetPlayBalance left = %d right = %d"), getBalanceL, getBalanceR);
				if (err == KErrNone)
					{
					if(getBalanceL == iExLSpeakerBalance && getBalanceR == iExRSpeakerBalance)
						{
						INFO_PRINTF5(_L("Balance returned by CMMFDevSound::GetPlayBalance: left %d right %d matches the default device balance: left %d right %d"), getBalanceL, getBalanceR, iExLSpeakerBalance, iExRSpeakerBalance);
						}
					else
						{
						ERR_PRINTF3(_L("Balance values do not match expected values, left %d right %d"), getBalanceL, getBalanceR);
						ERR_PRINTF3(_L("Expected values, left %d right %d"), iExLSpeakerBalance, iExRSpeakerBalance);
						StopTest(KErrNone, EFail);
						}
					}
				else
					{
					ERR_PRINTF1(_L("Error in getting left and right play balance."));
					StopTest(err);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0022
 */
RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest::RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC)
	{
	}

RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest* RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open (iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayCapabilitiesPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest (err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer (ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData()"));
				iMMFDevSound->PlayData ();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				RArray< TFourCC > supportedDataTypes;
				TMMFPrioritySettings prioritySettings;
				TRAPD(err,iMMFDevSound->GetSupportedInputDataTypesL(supportedDataTypes, prioritySettings));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Getting supported data types falied ! Left with error = %d"), err);
					StopTest(err);
					break;
					}
				for(TInt x = 0; x < supportedDataTypes.Count (); x++)
					{
					TFourCC fourCC = supportedDataTypes[x];
					TBuf<KTFourCC> name;
					for (TInt i=0; i<= KFourCCMaxBitDisplacement; i+=K8Bits)
						{
						name.Append ( (TUint8)(fourCC.FourCC() >> i));
						}
					INFO_PRINTF3(_L("Supported Input Data types: 0x%x  %S "), fourCC.FourCC(), &name);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0023
 */
RA3FDevSoundPlayMaxVolumeWhilePlayingTest::RA3FDevSoundPlayMaxVolumeWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC),
	iExVolume(0)
	{
	}

RA3FDevSoundPlayMaxVolumeWhilePlayingTest* RA3FDevSoundPlayMaxVolumeWhilePlayingTest::NewL(	const TDesC& aTestName)
	{
	RA3FDevSoundPlayMaxVolumeWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlayMaxVolumeWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayMaxVolumeWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayMaxVolumeWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayCapabilitiesPlayingTest"), EFsmIncorrectErrorPassed));
			if ( aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete was NOT received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData()"));
				iMMFDevSound->PlayData();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				TInt maxVolume = iMMFDevSound->MaxVolume();
				INFO_PRINTF1(_L("Calling CMMFDevSound::MaxVolume"));
				if(maxVolume == iExVolume)
					{
					INFO_PRINTF2(_L("Call to CMMFDevSound::MaxVolume succeeded with %d"), maxVolume);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Call to CMMFDevSound::MaxVolume failed with %d"), maxVolume);
					ERR_PRINTF2(_L("Expected Max volume value = %d"), iExVolume);
					StopTest(KErrNone, EFail);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayMaxVolumeWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm (EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0024
 */
RA3FDevSoundPlaySamplesPlayedWhilePlayingTest::RA3FDevSoundPlaySamplesPlayedWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC),
	iExSamplesPlayed(0)
	{
	}

RA3FDevSoundPlaySamplesPlayedWhilePlayingTest* RA3FDevSoundPlaySamplesPlayedWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlaySamplesPlayedWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlaySamplesPlayedWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlaySamplesPlayedWhilePlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open (iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlaySamplesPlayedWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayCapabilitiesPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest (err);
					break;
					}
				if (buffer->Data().Length()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData()"));
				iMMFDevSound->PlayData();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::SamplesPlayed"));
				TInt samplesPlayed = iMMFDevSound->SamplesPlayed();
				INFO_PRINTF2(_L("Samples played so far %d"), samplesPlayed);
				if(samplesPlayed > 0)
					{
					INFO_PRINTF2(_L("Samples returned by CMMFDevSound::SamplesPlayed: %d matches the expected value which is > 0"), samplesPlayed);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Samples played returned an invalid value %d"), samplesPlayed);
					ERR_PRINTF1(_L("Expected value must be a positive value greater than 0"));
					StopTest(aError, EFail);
					
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlaySamplesPlayedWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0025
 */

RA3FDevSoundPlayVolumeWhilePlayingTest::RA3FDevSoundPlayVolumeWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC), 
	iExVolume(0)
	{
	}

RA3FDevSoundPlayVolumeWhilePlayingTest* RA3FDevSoundPlayVolumeWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayVolumeWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlayVolumeWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayVolumeWhilePlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayVolumeWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayCapabilitiesPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				StartTimer (KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
					StopTest(aError);
					}
				else
					{
					ERR_PRINTF2(_L("DevSound EEventInitComplete NOT received as expected. Received event: %d"), aDevSoundEvent);
					StopTest(aError, EFail);
					}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData()"));
				iMMFDevSound->PlayData();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Volume"));
				TInt volumeGet = iMMFDevSound->Volume();
				if(volumeGet == iExVolume)
					{
					INFO_PRINTF2(_L("Default device volume succeeded with = %d"), volumeGet);
					}
				else
					{
					ERR_PRINTF2(_L("Default device volume failed with = %d"), volumeGet);
					ERR_PRINTF2(_L("Expected value = % d"), iExVolume);
					StopTest(aError , EFail);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayVolumeWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0026
 */
RA3FDevSoundPlayTest::RA3FDevSoundPlayTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC)
	{
	}

RA3FDevSoundPlayTest* RA3FDevSoundPlayTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayTest* self = new (ELeave) RA3FDevSoundPlayTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest (KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy (filename);
	TInt err = iFile.Open (iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC (fourccCode);
	}

void RA3FDevSoundPlayTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayCapabilitiesPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest (err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData()"));
				iMMFDevSound->PlayData();
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0027
 */
RA3FDevSoundPlayEmptyBufferWhilePlayingTest::RA3FDevSoundPlayEmptyBufferWhilePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC)
	{
	}

RA3FDevSoundPlayEmptyBufferWhilePlayingTest* RA3FDevSoundPlayEmptyBufferWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayEmptyBufferWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlayEmptyBufferWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayEmptyBufferWhilePlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayEmptyBufferWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayEmptyBufferPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError, EFail);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize ())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData()"));
				iMMFDevSound->PlayData();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::EmptyBuffers()"));
				TInt err = iMMFDevSound->EmptyBuffers();
				if (err == KErrNone)
					{
					INFO_PRINTF2(_L("Call EmptyBuffers() while playing returned error = %d"), err);
					StopTest();
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Call EmptyBuffers() while playing returned  error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNone);
					StopTest(err);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest (aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayEmptyBufferWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0028
 */
RA3FDevSoundPlayEmptyBufferBeforePlayingTest::RA3FDevSoundPlayEmptyBufferBeforePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayEmptyBufferBeforePlayingTest* RA3FDevSoundPlayEmptyBufferBeforePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayEmptyBufferBeforePlayingTest* self = new (ELeave) RA3FDevSoundPlayEmptyBufferBeforePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayEmptyBufferBeforePlayingTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayEmptyBufferBeforePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayEmptyBufferPlayingTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying))	;
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::EmptyBuffers()"));
				TInt err = iMMFDevSound->EmptyBuffers ();
				if (err == KErrNotReady)
					{
					INFO_PRINTF2(_L("Calling CMMFDevSound::EmptyBuffers after initialization and before playback returned with the expected error = %d"), err);
					StopTest();
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::EmptyBuffers after initialization and before playback returned with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError, EFail);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0029
 */
RA3FDevSoundPlayPauseTest::RA3FDevSoundPlayPauseTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC),
	iPaused(EFalse), iSamplesPlayedPaused(0)
	{
	}

RA3FDevSoundPlayPauseTest* RA3FDevSoundPlayPauseTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayPauseTest* self = new (ELeave) RA3FDevSoundPlayPauseTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayPauseTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayPauseTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayPauseTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest (err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Calling CMMFDevSound::SamplesPlayed"));
				iSamplesPlayedPaused = iMMFDevSound->SamplesPlayed();
				INFO_PRINTF2(_L("The number of samples played is %d"), iSamplesPlayedPaused);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventTimerComplete)
				{
				TInt samplesPlayed = iMMFDevSound->SamplesPlayed();
				INFO_PRINTF2(_L("Samples returned by CMMFDevSound::SamplesPlayed = %d"), samplesPlayed);
				if(iSamplesPlayedPaused == samplesPlayed)
					{
					INFO_PRINTF3(_L("Samples returned by CMMFDevSound::SamplesPlayed: %d matches the expected samples played: %d"), samplesPlayed, iSamplesPlayedPaused);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Call to CMMFDevSound::SamplesPlayed do not match the expected value = %d"), samplesPlayed);
					ERR_PRINTF2(_L("Expected Samples played value = %d"), iSamplesPlayedPaused);
					StopTest(KErrNone, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayPauseTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if (!iPaused)
		{
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		iPaused = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0030
 */
RA3FDevSoundPlayStopTest::RA3FDevSoundPlayStopTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC), iStopped(EFalse)
	{
	}

RA3FDevSoundPlayStopTest* RA3FDevSoundPlayStopTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayStopTest* self = new (ELeave) RA3FDevSoundPlayStopTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayStopTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayStopTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayStopTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if (!iStopped)
					{
					iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
					INFO_PRINTF1(_L("CMMFDevSound::PlayInitL"));
					TRAPD(err, iMMFDevSound->PlayInitL());
					if (err != KErrNone)
						{
						StopTest(err);
						break;
						}
					StartTimer(KMicroSecsTwoSec);
					INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
					iDevSoundState = EStatePlaying;
					}
				else
					{
					INFO_PRINTF1(_L("InitializeComplete returned KErrNone after calling CMMFDevSound::Stop then CMMFDevSound::InitializeL"));
					StopTest();
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				if (!iStopped)
					{
					ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
					StopTest(aError);
					}
				else
					{
					INFO_PRINTF2(_L("InitializeComplete returned %d after calling CMMFDevSound::Stop then CMMFDevSound::InitializeL"), aError);
					StopTest(aError);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData();
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("CMMFDevSound::Stop"));
				iMMFDevSound->Stop();
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayStopTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if (!iStopped)
		{
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		iStopped = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("Initializing again after stop successful. Hence passing the test"));
		TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
			StopTest(err);
			}
		INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
		iDevSoundState = EStateInitializing;
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0031
 */
RA3FDevSoundPlayPreemptionTest::RA3FDevSoundPlayPreemptionTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC),
	iOtherFilename(KNullDesC)
	{
	}

RA3FDevSoundPlayPreemptionTest* RA3FDevSoundPlayPreemptionTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayPreemptionTest* self = new (ELeave) RA3FDevSoundPlayPreemptionTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayPreemptionTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		StopTest(err);
		return;
		}
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	// Get the filename of the second devsound instance
	TPtrC tempString;
	if (!GetStringFromConfig(iTestStepName, KOtherFilename, tempString))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KOtherFilename);
		StopTest(KErrNotFound);
		return;
		}
	iOtherFilename.Copy (tempString);
	INFO_PRINTF2(_L("File under test  -> %S"), &iOtherFilename);
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundPlayClient = CA3FDevSoundPlayClient::NewL(*this, iFourCCCode, iFs, iSuite);
	}

void RA3FDevSoundPlayPreemptionTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayPreemptionTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFPrioritySettings priority;
				priority.iPriority = KMinimumPriority;
				INFO_PRINTF2(_L("Setting Priority to %d using CMMFDevSound::SetPrioritySettings"), priority.iPriority);
				iMMFDevSound->SetPrioritySettings (priority);
				iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				StartTimer(KMicroSecsTwoSec);
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest (err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData();
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayPreemptionTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("Starting higher priority devsound client"));
	iDevsoundPlayClient->SetPriority(KMaximumPriority);
	INFO_PRINTF1(_L("Initializing to play audio thru second devsound client"));
	TInt err = iDevsoundPlayClient->PlayInit(iOtherFilename);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Initializating DevSound to play audio thru second devsound client failed with error = %d"), err);
		StopTest(err);
		}
	}

void RA3FDevSoundPlayPreemptionTest::PlayError(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound PlayError() callback =========="));
	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("========== Preempting 1st DevSound Instance=========="));
		INFO_PRINTF2(_L("First DevSound client PlayError succeeded with error = %d "), aError);
		}
	else
		{
		ERR_PRINTF2(_L("First DevSound client PlayError and failed with error = %d "), aError);
		ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}

void RA3FDevSoundPlayPreemptionTest::ClientInitializeCompleteCallback(TInt aError)                               
	{
	INFO_PRINTF1(_L("========== Second Client DevSound InitializeComplete() callback =========="));
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("Second Devsound InitializeComplete received an error = %d"), aError);
		StopTest(aError);
		}
	else
		{
		INFO_PRINTF1(_L("Starting to play audio thru second devsound client"));
		TInt err = iDevsoundPlayClient->PlayAudio();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Starting to play audio thru second devsound client failed with error = %d"), err);
			StopTest(err);
			}
		}
	}

void RA3FDevSoundPlayPreemptionTest::ClientBufferToBeFilledCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound BufferToBeFilled() callback =========="));
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("Second DevSound client called BufferToBeFilled and failed with error = %d"), aError);
		StopTest(aError);
		}
	}

void RA3FDevSoundPlayPreemptionTest::ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
	{
	switch (aError)
		{
		case EInvalidClientFSMEvent:
			ERR_PRINTF1(_L("Second devsound client received an invalid Event..."));
			break;
		case EInvalidClientFSMState:
			ERR_PRINTF1(_L("Second devsound client received an invalid State..."));
			break;
		case EReadFileErrorInClient:
			ERR_PRINTF1(_L("Second devsound client could not read to file..."));
			break;
		default:
			break;
		}
	StopTest(aError, EFail);
	}

void RA3FDevSoundPlayPreemptionTest::ClientPlayErrorCallback(TInt aError, TBool aLastBuffer)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound PlayError() callback =========="));

	if (aError == KErrUnderflow && aLastBuffer)
		{
		INFO_PRINTF1(_L("Second DevSound client called PlayError and completed normally"));
		StopTest();
		}
	else
		{
		ERR_PRINTF2(_L("Second DevSound PlayError failed with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d "), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * 3F-DEVSOUND-CHRTZ-PLAY-RAW-0032
 */
RA3FDevSoundPlayPreemptedByToneTest::RA3FDevSoundPlayPreemptedByToneTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName),iFilename(KNullDesC),
	iFrequencyTone1(0), iDuration(0)
	{
	}

RA3FDevSoundPlayPreemptedByToneTest* RA3FDevSoundPlayPreemptedByToneTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayPreemptedByToneTest* self = new (ELeave) RA3FDevSoundPlayPreemptedByToneTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayPreemptedByToneTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundToneClient = CA3FDevSoundToneClient::NewL(*this);
	}

void RA3FDevSoundPlayPreemptedByToneTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,	TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayPreemptedByToneTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFPrioritySettings priority;
				priority.iPriority = KMinimumPriority;
				INFO_PRINTF2(_L("Setting Priority to %d using CMMFDevSound::SetPrioritySettings"), priority.iPriority);
				iMMFDevSound->SetPrioritySettings (priority);
				iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest (err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				StartTimer (KMicroSecsTwoSec);
				}
			else if ( aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData();
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayPreemptedByToneTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("Starting higher priority devsound client"));
	iDevsoundToneClient->SetPriority(KMaximumPriority);
	INFO_PRINTF1(_L("Initializing to play tones thru second devsound client"));
	TInt err = iDevsoundToneClient->InitTonePlay(iFrequencyTone1,iDuration*KMicroSecsInOneSec);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Initializating DevSoud to play tones thru second devsound client failed with error = %d"), err);
		StopTest(err);
		}
	}

void RA3FDevSoundPlayPreemptedByToneTest::PlayError(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound PlayError() callback =========="));
	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("========== Preempting 1st DevSound Instance=========="));
		INFO_PRINTF2(_L("First DevSound client PlayError succeeded with error = %d "), aError);
		}
	else
		{
		ERR_PRINTF2(_L("First DevSound client called PlayError and failed with error = %d "), aError);
		ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}

void RA3FDevSoundPlayPreemptedByToneTest::ClientInitializeCompleteCallback(TInt aError)
	{
	INFO_PRINTF1(_L("==========Second DevSound client called InitializeComplete()=========="));
	if (aError == KErrNone)
		{
		INFO_PRINTF1(_L("Starting to play tone thru second devsound client"));
		TInt err = iDevsoundToneClient->PlayTone ();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Second Devsound client called PlayTone and failed with error = %d"), err);
			StopTest(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Second Devsound InitializeComplete received an error = %d"), aError);
		StopTest(aError);
		}
	}

void RA3FDevSoundPlayPreemptedByToneTest::ClientToneFinishedCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second  DevSound client called ToneFinished()=========="));
	if (aError == KErrUnderflow)
		{
		INFO_PRINTF2(_L("Second DevSound client called ToneFinished and succeeded with error = %d"), aError);
		StopTest(aError, EPass);
		}
	else
		{
		ERR_PRINTF2(_L("Second DevSound client called ToneFinished and failed with error = %d"), aError);
		StopTest(aError);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0033
 */
RA3FDevSoundPlayPreemptedByRecordTest::RA3FDevSoundPlayPreemptedByRecordTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC)

	{
	}

RA3FDevSoundPlayPreemptedByRecordTest* RA3FDevSoundPlayPreemptedByRecordTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayPreemptedByRecordTest* self = new (ELeave) RA3FDevSoundPlayPreemptedByRecordTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayPreemptedByRecordTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		StopTest(err);
		return;
		}
	// Get the filename of the audio file to record
	if (!GetStringFromConfig(iTestStepName, KOtherFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KOtherFilename);
		StopTest(KErrNotFound);
		return;
		}

	iOtherFilename.Copy(filename);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundRecordClient = CA3FDevSoundRecordClient::NewL(*this, iFourCCCode, iFs, iSuite);
	}

void RA3FDevSoundPlayPreemptedByRecordTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayPreemptedByRecordTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFPrioritySettings priority;
				priority.iPriority = KMinimumPriority;
				INFO_PRINTF2(_L("Setting Priority to %d using CMMFDevSound::SetPrioritySettings"), priority.iPriority);
				iMMFDevSound->SetPrioritySettings (priority);
				iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				StartTimer(KMicroSecsTwoSec);
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData();
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayPreemptedByRecordTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if (!iOtherClientStarted)
		{
		INFO_PRINTF1(_L("Initializing higher priority devsound client..."));
		iDevsoundRecordClient->SetPriority (KMaximumPriority);
		TInt err = iDevsoundRecordClient->InitializeRecordClient (iOtherFilename);
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Initializating DevSoud to record audio thru second devsound client failed with error = %d"));
			StopTest(err);
			}
		iOtherClientStarted = ETrue;
		}
	else
		{
		iTimer->Cancel();
		iDevsoundRecordClient->StopRecord();
		StopTest();
		}
	}

void RA3FDevSoundPlayPreemptedByRecordTest::PlayError(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound PlayError() callback =========="));
	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("========== Preempting 1st DevSound Instance=========="));
		INFO_PRINTF2(_L("First DevSound client PlayError succeeded with error = %d "), aError);
		}
	else
		{
		ERR_PRINTF2(_L("First DevSound client called PlayError and failed with error = %d "), aError);
		ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}

void RA3FDevSoundPlayPreemptedByRecordTest::ClientInitializeCompleteCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound InitializeComplete() callback =========="));
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("Second Devsound InitializeComplete received an error = %d"), aError);
		StopTest(aError);
		}
	else
		{
		INFO_PRINTF2(_L("Second DevSound client called InitializeComplete with error = %d"), aError);
		INFO_PRINTF1(_L("Starting to record audio thru second devsound client"));
		TInt err = iDevsoundRecordClient->RecordAudio ();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Starting to record audio thru second devsound client failed with error = %d"), err);
			StopTest(err);
			}
		}
	}

void RA3FDevSoundPlayPreemptedByRecordTest::ClientBufferToBeEmptiedCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound ClientBufferToBeEmptied() callback =========="));
	switch (aError)
		{
		case KErrGeneral:
			{
			INFO_PRINTF1(_L("Second DevSound client called BufferToBeEmptied and received a NULL CMMFBuffer"));
			StopTest(aError);
			break;
			}
		case KErrUnknown:
			{
			INFO_PRINTF1(_L("Second DevSound client called BufferToBeEmptied: Unknown behaviour: Last buffer flag set before calling CMMFDevSound->Pause()"));
			StopTest(aError);
			break;
			}
		}
	}

void RA3FDevSoundPlayPreemptedByRecordTest::ClientRecordErrorCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound RecordError() callback =========="));
	if (aError == KErrUnderflow)
		{
		INFO_PRINTF2(_L("Second DevSound client called RecordError with error = %d"), aError);
		StopTest(aError);
		}
	}

void RA3FDevSoundPlayPreemptedByRecordTest::ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
	{
	switch (aError)
		{
		case EInvalidClientFSMEvent:
			{
			ERR_PRINTF1(_L("Second devsound client received an invalid Event..."));
			StopTest(KErrNone, EFail);
			break;
			}
		case EInvalidClientFSMState:
			{
			ERR_PRINTF1(_L("Second devsound client received an invalid State..."));
			StopTest(KErrNone, EFail);
			break;
			}
		case EWriteFileErrorInClient:
			{
			ERR_PRINTF1(_L("Second devsound client could not write to file..."));
			StopTest(KErrNone, EFail);
			break;
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0034
 */
RA3FDevSoundPlayRejectionTest::RA3FDevSoundPlayRejectionTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC),
	iOtherFilename(KNullDesC)
	{
	}

RA3FDevSoundPlayRejectionTest* RA3FDevSoundPlayRejectionTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayRejectionTest* self = new (ELeave) RA3FDevSoundPlayRejectionTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayRejectionTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);

	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		StopTest(err);
		return;
		}

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	// Get the filename of the second devsound instance
	TPtrC tempString;
	if (!GetStringFromConfig(iTestStepName, KOtherFilename, tempString))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KOtherFilename);
		StopTest(KErrNotFound);
		return;
		}
	
	iOtherFilename.Copy (tempString);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundPlayClient = CA3FDevSoundPlayClient::NewL(*this, iFourCCCode, iFs, iSuite);
	}

void RA3FDevSoundPlayRejectionTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayRejectionTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFPrioritySettings priority;
				priority.iPriority = KMaximumPriority;
				INFO_PRINTF2(_L("Setting Priority to %d using CMMFDevSound::SetPrioritySettings"), priority.iPriority);
				iMMFDevSound->SetPrioritySettings (priority);
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if ( err != KErrNone)
					{
					INFO_PRINTF2(_L("CMMFDevSound::PlayInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				StartTimer (KMicroSecsTwoSec);
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest(err);
					break;
					}
				if (buffer->Data().Length()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer (ETrue);
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData();
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayRejectionTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("Starting higher priority devsound client"));
	iDevsoundPlayClient->SetPriority(KMinimumPriority);
	INFO_PRINTF1(_L("Initializing to play audio thru second devsound client"));
	TInt err = iDevsoundPlayClient->PlayInit(iOtherFilename);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Initialization to play audio thru second devsound client failed with %d"), err);
		StopTest(err);
		}
	}

void RA3FDevSoundPlayRejectionTest::ClientInitializeCompleteCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound InitializeComplete() callback =========="));
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("Second Devsound InitializeComplete received an error = %d"), aError);
		StopTest(aError);
		}
	else
		{
		INFO_PRINTF2(_L("Second DevSound client called InitializeComplete with error = %d"), aError);
		INFO_PRINTF1(_L("Starting to play audio thru second devsound client"));
		TInt err = iDevsoundPlayClient->PlayAudio ();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Starting to play audio thru second devsound client failed with error = %d"), err);
			StopTest(err);
			}
		}
	}

void RA3FDevSoundPlayRejectionTest::ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
	{
	switch (aError)
		{
		case EInvalidClientFSMEvent:
			ERR_PRINTF1(_L("Second devsound client received an invalid Event..."));
			break;
		case EInvalidClientFSMState:
			ERR_PRINTF1(_L("Second devsound client received an invalid State..."));
			break;
		case EReadFileErrorInClient:
			ERR_PRINTF1(_L("Second devsound client could not read to file..."));
			break;
		default:
			break;
		}
	StopTest(aError, EFail);
	}

void RA3FDevSoundPlayRejectionTest::ClientBufferToBeFilledCallback(TInt aError)
	{
	INFO_PRINTF2(_L("Second DevSound client should not call BufferToBeFilled in rejection. Error = %d"), aError);
	StopTest(aError, EFail);
	}

void RA3FDevSoundPlayRejectionTest::ClientPlayErrorCallback(TInt aError, TBool aLastBuffer)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound PlayError() callback =========="));

	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied && !aLastBuffer)
		{
		INFO_PRINTF1(_L("========== Rejecting 2nd DevSound Instance=========="));
		INFO_PRINTF2(_L("Second DevSound client PlayError succeeded with error = %d"), aError);
		}
	else
		{
		ERR_PRINTF2(_L("Second DevSound PlayError failed with error = %d"), aError);
		ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0035
 */
RA3FDevSoundPlayRejectedByToneTest::RA3FDevSoundPlayRejectedByToneTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC)
	{
	}

RA3FDevSoundPlayRejectedByToneTest* RA3FDevSoundPlayRejectedByToneTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayRejectedByToneTest* self = new (ELeave) RA3FDevSoundPlayRejectedByToneTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayRejectedByToneTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy (filename);

	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundPlayClient = CA3FDevSoundPlayClient::NewL(*this, iFourCCCode, iFs, iSuite);
	}

void RA3FDevSoundPlayRejectedByToneTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayRejectedByToneTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFPrioritySettings priority;
				priority.iPriority = KMinimumPriority;
				INFO_PRINTF2(_L("Setting Priority to %d using CMMFDevSound::SetPrioritySettings"), priority.iPriority);
				iMMFDevSound->SetPrioritySettings (priority);
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneL"));
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error =%d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				StartTimer(KMicroSecsTwoSec);
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
					StopTest(aError);
					}
				else
					{
					ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
					StopTest(aError, EFail);
					}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayRejectedByToneTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("Starting higher priority devsound client"));
	iDevsoundPlayClient->SetPriority(KMinimumPriority);
	INFO_PRINTF1(_L("Initializing to play audio thru second devsound client"));
	TInt err = iDevsoundPlayClient->PlayInit(iFilename);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Initialization to play audio thru second devsound client failed with %d"), err);
		StopTest(err);
		}
	}

void RA3FDevSoundPlayRejectedByToneTest::ClientInitializeCompleteCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound InitializeComplete() callback =========="));
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("Second Devsound InitializeComplete received an error = %d"), aError);
		StopTest (aError);
		}
	else
		{
		INFO_PRINTF2(_L("Second DevSound client called InitializeComplete with error = %d"), aError);
		INFO_PRINTF1(_L("Starting to play audio thru second devsound client"));
		TInt err = iDevsoundPlayClient->PlayAudio ();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Starting to play audio thru second devsound client failed with error = %d"), err);
			StopTest(err);
			}
		}
	}

void RA3FDevSoundPlayRejectedByToneTest::ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
	{
	switch (aError)
		{
		case EInvalidClientFSMEvent:
			ERR_PRINTF1(_L("Second devsound client received an invalid Event..."));
			break;
		case EInvalidClientFSMState:
			ERR_PRINTF1(_L("Second devsound client received an invalid State..."));
			break;
		case EReadFileErrorInClient:
			ERR_PRINTF1(_L("Second devsound client could not read to file..."));
			break;
		default:
			break;
		}
	StopTest(aError, EFail);
	}

void RA3FDevSoundPlayRejectedByToneTest::ClientBufferToBeFilledCallback(TInt aError)
	{
	INFO_PRINTF2(_L("Second DevSound client should not call BufferToBeFilled in rejection. Error = %d"), aError);
	StopTest(aError, EFail);
	}

void RA3FDevSoundPlayRejectedByToneTest::ClientPlayErrorCallback(TInt aError,TBool aLastBuffer)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound PlayError() callback =========="));
	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied && !aLastBuffer)
		{
		INFO_PRINTF1(_L("========== Rejecting 2nd DevSound Instance=========="));
		INFO_PRINTF2(_L("Second DevSound client PlayError succeeded with error = %d"), aError);
		}
	else
		{
		ERR_PRINTF2(_L("Second DevSound PlayError failed with error = %d"), aError);
		ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0036
 */
RA3FDevSoundPlayRejectedByRecordTest::RA3FDevSoundPlayRejectedByRecordTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC), 
	iOtherFilename(KNullDesC), iOtherClientStarted(EFalse)
	{
	}

RA3FDevSoundPlayRejectedByRecordTest* RA3FDevSoundPlayRejectedByRecordTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayRejectedByRecordTest* self = new (ELeave) RA3FDevSoundPlayRejectedByRecordTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayRejectedByRecordTest::DoKickoffTestL()
	{
	TPtrC filename;
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// Open file for recording using RFile 
	
	iFilename.Copy (filename);
	
	// Get the filename of the second audio file to play
	if (!GetStringFromConfig(iTestStepName, KOtherFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KOtherFilename);
		StopTest(KErrNotFound);
		return;
		}
	INFO_PRINTF2(_L("Opening file for recording %S"), &iFilename);
	TInt err = iFile.Replace (iFs, iFilename, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}

	iOtherFilename.Copy (filename);

	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest (KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundPlayClient = CA3FDevSoundPlayClient::NewL(*this, iFourCCCode, iFs, iSuite);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundPlayRejectedByRecordTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayRejectedByRecordTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Initialization of DevSound failed with error : %d!"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("CMMFDevSound::RecordInitL"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if (aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback"));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				// Continue recording data
				iAsyncWriteBTFAO->Start (buffer);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayRejectedByRecordTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if (!iOtherClientStarted)
		{
		INFO_PRINTF1(_L("Starting higher priority devsound client"));
		iDevsoundPlayClient->SetPriority(KMinimumPriority);
		INFO_PRINTF1(_L("Initializing to play audio thru second devsound client"));
		TInt err = iDevsoundPlayClient->PlayInit(iFilename);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Initialization to play audio thru second devsound client failed with %d"), err);
			StopTest(err);
			}
		iOtherClientStarted = ETrue;
		}
	else
		{
		iTimer->Cancel();
		Fsm(EEventTimerComplete, KErrNone);
		}
	}

void RA3FDevSoundPlayRejectedByRecordTest::ClientInitializeCompleteCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound InitializeComplete() callback =========="));
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("Second Devsound InitializeComplete received an error = %d"), aError);
		StopTest(aError);
		}
	else
		{
		INFO_PRINTF2(_L("Second DevSound client called InitializeComplete and with error = %d"), aError);
		INFO_PRINTF1(_L("Starting to play audio thru second devsound client"));
		TInt err = iDevsoundPlayClient->PlayAudio();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Starting to play audio thru second devsound client failed with error = %d"), err);
			StopTest(err);
			}
		}
	}

void RA3FDevSoundPlayRejectedByRecordTest::ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
	{
	switch (aError)
		{
		case EInvalidClientFSMEvent:
			ERR_PRINTF1(_L("Second devsound client received an invalid Event..."));
			break;
		case EInvalidClientFSMState:
			ERR_PRINTF1(_L("Second devsound client received an invalid State..."));
			break;
		case EReadFileErrorInClient:
			ERR_PRINTF1(_L("Second devsound client could not read to file..."));
			break;
		default:
			break;
		}
	StopTest(aError, EFail);
	}

void RA3FDevSoundPlayRejectedByRecordTest::ClientBufferToBeFilledCallback(TInt aError)
	{
	INFO_PRINTF2(_L("Second DevSound client should not call BufferToBeFilled in rejection. Error = %d"), aError);
	StopTest(aError, EFail);
	}

void RA3FDevSoundPlayRejectedByRecordTest::ClientPlayErrorCallback(TInt aError, TBool aLastBuffer)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound PlayError() callback =========="));

	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied && !aLastBuffer)
		{
		INFO_PRINTF1(_L("========== Rejecting 2nd DevSound Instance=========="));
		INFO_PRINTF2(_L("Second DevSound client PlayError succeeded with error = %d"), aError);
		}
	else
		{
		ERR_PRINTF2(_L("Second DevSound PlayError failed with error = %d"), aError);
		ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0037
 */
RA3FDevSoundPlayDataBeforePlayInitTest::RA3FDevSoundPlayDataBeforePlayInitTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayDataBeforePlayInitTest* RA3FDevSoundPlayDataBeforePlayInitTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayDataBeforePlayInitTest* self = new (ELeave) RA3FDevSoundPlayDataBeforePlayInitTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayDataBeforePlayInitTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayDataBeforePlayInitTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayPlayDatabeforePlayInitLTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData out of sequence"));
				iMMFDevSound->PlayData();
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0038
 */
RA3FDevSoundPlayInitTwiceTest::RA3FDevSoundPlayInitTwiceTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName),iFirstPlayInitErr(KErrGeneral),
	iErr(KErrGeneral)
	{
	}

RA3FDevSoundPlayInitTwiceTest* RA3FDevSoundPlayInitTwiceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayInitTwiceTest* self = new (ELeave) RA3FDevSoundPlayInitTwiceTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayInitTwiceTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC (fourccCode);
	}

void RA3FDevSoundPlayInitTwiceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayPlayInitLTwiceTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAP(iFirstPlayInitErr, iMMFDevSound->PlayInitL());
				if (iFirstPlayInitErr != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL left with error = %d"), iFirstPlayInitErr);
					StopTest(iFirstPlayInitErr);
					break;
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayInitL twice (more than once)"));
				TRAP(iErr, iMMFDevSound->PlayInitL());
				if (iErr == KErrNotReady)
					{
					INFO_PRINTF2(_L("CMMFDevSound::PlayInitL left with the expected error = %d"), iErr);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Second CMMFDevSound::PlayInitL did NOT leave with the expected error = %d"), iErr);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(iErr, EFail);
					break;
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayInitTwiceTest::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("========== DevSound BufferToBeFilled() callback =========="));
	if (iErr == KErrNotReady && iFirstPlayInitErr == KErrNone)
		{
		INFO_PRINTF1(_L("BufferToBeFilled is called because the leave generated due to second CMMFDevSound::PlayInitL is ignored"));
		StopTest();
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0039
 */
RA3FDevSoundPlayARNTest::RA3FDevSoundPlayARNTest(const TDesC& aTestName) :
	RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC),
	iOtherFilename(KNullDesC)
	{
	}

RA3FDevSoundPlayARNTest* RA3FDevSoundPlayARNTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayARNTest* self = new (ELeave) RA3FDevSoundPlayARNTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayARNTest::DoKickoffTestL()
	{
	iRepeat = EFalse;
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		StopTest(err);
		return;
		}
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	// Get the filename of the second devsound instance
	TPtrC tempString;
	if (!GetStringFromConfig(iTestStepName, KOtherFilename, tempString))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KOtherFilename);
		StopTest(KErrNotFound);
		return;
		}
	iOtherFilename.Copy (tempString);
	INFO_PRINTF2(_L("File under test  -> %S"), &iOtherFilename);
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundPlayClient = CA3FDevSoundPlayClient::NewL(*this, iFourCCCode, iFs, iSuite);
	}

void RA3FDevSoundPlayARNTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayPreemptionTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFPrioritySettings priority;
				priority.iPriority = KMinimumPriority;
				INFO_PRINTF2(_L("Setting Priority to %d using CMMFDevSound::SetPrioritySettings"), priority.iPriority);
				iMMFDevSound->SetPrioritySettings (priority);
				iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				INFO_PRINTF1(_L("Registering first client"));
				iMMFDevSound->RegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
				iDevSoundState = EStatePlaying;
				StartTimer(KMicroSecsTwoSec);
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data ());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
					StopTest (err);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
				iMMFDevSound->PlayData();
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateResumePlaying:
			{
//			if (aDevSoundEvent == KMMFEventCategoryAudioResourceAvailable)
			if (aDevSoundEvent == EResourceAvailable)
				{
				INFO_PRINTF1(_L("First Client Received Notification as expected"));
				//INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
				//iMMFDevSound->PlayData ();
				StopTest();
				}
			else
				{
				ERR_PRINTF2(_L("DevSound KMMFEventCategoryAudioResourceAvailable not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(KErrGeneral);
			}
		}
	}

void RA3FDevSoundPlayARNTest::DoTimerCallback()
	{
	if(!iRepeat)
		{
		INFO_PRINTF1(_L("TimerEvent called"));
		//iTimer->Cancel();
		INFO_PRINTF1(_L("Starting higher priority devsound client"));
		iDevsoundPlayClient->SetPriority(KMaximumPriority);
		INFO_PRINTF1(_L("Initializing to play audio thru second devsound client"));
		TInt err = iDevsoundPlayClient->PlayInit(iOtherFilename);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Initializating DevSound to play audio thru second devsound client failed with error = %d"), err);
			StopTest(err);
			}
		iRepeat = ETrue;
		}
	else
		{
		INFO_PRINTF1(_L("Deleting second devsound client"));
		iDevsoundPlayClient->DeleteClient();
		iDevSoundState = EStateResumePlaying;
		}
	}

void RA3FDevSoundPlayARNTest::PlayError(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound PlayError() callback =========="));
	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("========== Preempting 1st DevSound Instance=========="));
		INFO_PRINTF2(_L("First DevSound client PlayError succeeded with error = %d "), aError);
		}
	else
		{
		ERR_PRINTF2(_L("First DevSound client PlayError and failed with error = %d "), aError);
		ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}

void RA3FDevSoundPlayARNTest::ClientInitializeCompleteCallback(TInt aError)                               
	{
	INFO_PRINTF1(_L("========== Second Client DevSound InitializeComplete() callback =========="));
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("Second Devsound InitializeComplete received an error = %d"), aError);
		StopTest(aError);
		}
	else
		{
		INFO_PRINTF1(_L("Starting to play audio thru second devsound client"));
		TInt err = iDevsoundPlayClient->PlayAudio();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Starting to play audio thru second devsound client failed with error = %d"), err);
			StopTest(err);
			}
		}
	}

void RA3FDevSoundPlayARNTest::ClientBufferToBeFilledCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound BufferToBeFilled() callback =========="));
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("Second DevSound client called BufferToBeFilled and failed with error = %d"), aError);
		StopTest(aError);
		}
	}

void RA3FDevSoundPlayARNTest::ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
	{
	switch (aError)
		{
		case EInvalidClientFSMEvent:
			ERR_PRINTF1(_L("Second devsound client received an invalid Event..."));
			break;
		case EInvalidClientFSMState:
			ERR_PRINTF1(_L("Second devsound client received an invalid State..."));
			break;
		case EReadFileErrorInClient:
			ERR_PRINTF1(_L("Second devsound client could not read to file..."));
			break;
		default:
			break;
		}
	StopTest(aError, EFail);
	}

void RA3FDevSoundPlayARNTest::ClientPlayErrorCallback(TInt aError, TBool aLastBuffer)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound PlayError() callback =========="));

	if (aError == KErrUnderflow && aLastBuffer)
		{
		INFO_PRINTF1(_L("Second DevSound client called PlayError and completed normally"));
		iDevSoundState = EStateResumePlaying;
		//StopTest();
		}
	else
		{
		ERR_PRINTF2(_L("Second DevSound PlayError failed with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d "), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}	

	/*
	 *========================================================================================================
	 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0040
	 */                                      
	RA3FDevSoundImmediatePauseAfterCreation::RA3FDevSoundImmediatePauseAfterCreation(const TDesC& aTestName) :
	    RA3FDevSoundTestBase(aTestName), iFilename(KNullDesC)
	    {
	    }

	RA3FDevSoundImmediatePauseAfterCreation* RA3FDevSoundImmediatePauseAfterCreation::NewL(const TDesC& aTestName)
	    {
	    RA3FDevSoundImmediatePauseAfterCreation* self = new (ELeave) RA3FDevSoundImmediatePauseAfterCreation(aTestName);
	    return self;
	    }

	void RA3FDevSoundImmediatePauseAfterCreation::DoKickoffTestL()
	    {
	    iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	    }

	void RA3FDevSoundImmediatePauseAfterCreation::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	    {
	    switch (iDevSoundState)
	        {
	        case EStateCreated:
	            {
	            __ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundImmediatePauseAfterCreation"), EFsmIncorrectErrorPassed));
	            if (aDevSoundEvent == EEventInitialize)
	                {
	                INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
	                
	                iMMFDevSound->Pause();
	                StartTimer(KMicroSecsInOneSec);
	        
	                iDevSoundState = EStatePause;
	                }
	            else
	                {
	                ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
	                StopTest(aError, EFail);
	                }
	            break;
	            }
	        
	        default:
	            {
	            ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
	            StopTest(aError, EFail);
	            }
	        }
	    }

	void RA3FDevSoundImmediatePauseAfterCreation::DoTimerCallback()
	    {
	    INFO_PRINTF1(_L("TimerEvent called"));
	    iTimer->Cancel();
	    INFO_PRINTF1(_L("Pause did not panic DevSound"));
	    StopTest(KErrNone);
	    }

	
	/*
	 *========================================================================================================
	 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0041
	 */
	RA3FDevSoundPlayEmptyBuffersWhilePlayingTest::RA3FDevSoundPlayEmptyBuffersWhilePlayingTest(const TDesC& aTestName) :
	    RA3FDevSoundTestBase(aTestName), iVolume(0),
	    iFilename(KNullDesC)
	    {
	    }

	RA3FDevSoundPlayEmptyBuffersWhilePlayingTest* RA3FDevSoundPlayEmptyBuffersWhilePlayingTest::NewL(const TDesC& aTestName)
	    {
	    RA3FDevSoundPlayEmptyBuffersWhilePlayingTest* self = new (ELeave) RA3FDevSoundPlayEmptyBuffersWhilePlayingTest(aTestName);
	    return self;
	    }

	void RA3FDevSoundPlayEmptyBuffersWhilePlayingTest::DoKickoffTestL()
	    {
	    TPtrC filename;
	    // Get the filename of the audio file to play
	    if (!GetStringFromConfig(iTestStepName, KFilename, filename))
	        {
	        ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
	        StopTest (KErrNotFound);
	        return;
	        }
	    // open using RFile for playback
	    iFilename.Copy (filename);
	    TInt err = iFile.Open (iFs, iFilename, EFileRead);
	    if (err != KErrNone)
	        {
	        ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
	        iFs.Close();
	        StopTest(err);
	        return;
	        }

	    INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	    if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
	        {
	        ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
	        StopTest (KErrNotFound);
	        return;
	        }
	    TPtrC fourccCode;
	    if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
	        {
	        ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
	        StopTest(KErrNotFound);
	        return;
	        }
	    EncodingFromStringToTFourCC(fourccCode);
	    }

	void RA3FDevSoundPlayEmptyBuffersWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	    {
	    switch (iDevSoundState)
	        {
	        case EStateCreated:
	            {
	            __ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlaySetVolumePlayingTest"), EFsmIncorrectErrorPassed));
	            if (aDevSoundEvent == EEventInitialize)
	                {
	                INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
	                TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
	                if (err != KErrNone)
	                    {
	                    ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error: %d"), err);
	                    StopTest(err);
	                    break;
	                    }
	                INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
	                iDevSoundState = EStateInitializing;
	                }
	            else
	                {
	                ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
	                StopTest(aError, EFail);
	                }
	            break;
	            }
	        case EStateInitializing:
	            {
	            if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
	                {
	                INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
	                TRAPD(err, iMMFDevSound->PlayInitL());
	                if (err != KErrNone)
	                    {
	                    StopTest(err);
	                    break;
	                    }
	                iDevSoundState = EStatePlaying;
	                }
	            else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
	                {
	                ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
	                StopTest(aError);
	                }
	            else
	                {
	                ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
	                StopTest(aError, EFail);
	                }
	            break;
	            }
	        case EStatePlaying:
	            {
	            if (aDevSoundEvent == EEventBTBF)
	                {
	                
	                if(iBtbfCount == 3)
	                    {
	                    INFO_PRINTF2(_L("BTBF count = %d, Calling CMMFDevSound::EmptyBuffers"),iBtbfCount);
	                    iMMFDevSound->EmptyBuffers();
	                    }
	                
	                else
	                    {
                        CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
                        TInt err = iFile.Read (buffer->Data ());
                        if (err != KErrNone)
                            {
                            ERR_PRINTF2(_L("ERROR : iFile.Read() failed with: %d"), err);
                            StopTest(err);
                            break;
                            }
                        if (buffer->Data().Length ()!= buffer->RequestSize ())
                            {
                            iBuffer->SetLastBuffer (ETrue);
                            }
                        INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
                        iMMFDevSound->PlayData ();
	                    }
	                
	                ++iBtbfCount;
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
	            ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
	            StopTest(aError, EFail);
	            }
	        }
	    }

	/*
	 *========================================================================================================
	 * 3F-DEVSOUND-CHRTZ-PLAY-RAW-0042
	 */
	RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest::RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest(const TDesC& aTestName) :
	    RA3FDevSoundTestBase(aTestName),iFilename(KNullDesC),
	    iExVolume(0), iFrequencyTone1(0), iDuration(0),  
	    iSetVolume(EFalse), iResume(EFalse)
	    {
	    }

	RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest* RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest::NewL(const TDesC& aTestName)
	    {
	    RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest* self = new (ELeave) RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest(aTestName);
	    return self;
	    }

	void RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest::DoKickoffTestL()
	    {
	    TPtrC filename;
	    // Get the filename of the audio file to play
	    if (!GetStringFromConfig(iTestStepName, KFilename, filename))
	        {
	        ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
	        StopTest(KErrNotFound);
	        return;
	        }
	    // open using RFile for playback
	    iFilename.Copy(filename);
	    TInt err = iFile.Open(iFs, iFilename, EFileRead);
	    if (err != KErrNone)
	        {
	        ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
	        StopTest(err);
	        return;
	        }

	    INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);

	    if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
	        {
	        ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
	        StopTest(KErrNotFound);
	        return;
	        }
	    if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
	        {
	        ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
	        StopTest(KErrNotFound);
	        return;
	        }
	    TPtrC fourccCode;
	    if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
	        {
	        ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
	        StopTest(KErrNotFound);
	        return;
	        }
	    
	    if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
	        {
	        ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
	        StopTest(KErrNotFound);
	        return;
	        }
	    
	    EncodingFromStringToTFourCC(fourccCode);
	    iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	    iDevsoundToneClient = CA3FDevSoundToneClient::NewL(*this);
	    }

	void RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	    {
	    switch (iDevSoundState)
	        {
	        case EStateCreated:
	            {
	            __ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundPlayPreemptedByToneTest"), EFsmIncorrectErrorPassed));
	            if (aDevSoundEvent == EEventInitialize)
	                {
	                INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
	                TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
	                if (err != KErrNone)
	                    {
	                    ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
	                    StopTest(err);
	                    break;
	                    }
	                INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
	                iDevSoundState = EStateInitializing;
	                }
	            else
	                {
	                ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
	                StopTest(aError, EFail);
	                }
	            break;
	            }
	        case EStateInitializing:
	            {
	            if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
	                {
	                TMMFPrioritySettings priority;
	                priority.iPriority = KMinimumPriority;
	                INFO_PRINTF2(_L("Setting Priority to %d using CMMFDevSound::SetPrioritySettings"), priority.iPriority);
	                iMMFDevSound->SetPrioritySettings (priority);
	                iMMFDevSound->SetVolume (iMMFDevSound->MaxVolume ());
	                INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
	                TRAPD(err, iMMFDevSound->PlayInitL());
	                if (err != KErrNone)
	                    {
	                    StopTest (err);
	                    break;
	                    }
	                INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
	                iDevSoundState = EStatePlaying;
	                StartTimer (KMicroSecsTwoSec);
	                }
	            else if ( aDevSoundEvent == EEventInitComplete && aError != KErrNone)
	                {
	                ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error: %d"), aError);
	                StopTest(aError);
	                }
	            else
	                {
	                ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
	                StopTest(aError, EFail);
	                }
	            break;
	            }
	        case EStatePlaying:
	            {
	            if (aDevSoundEvent == EEventBTBF)
	                {
                    if(iResume)
                        {
                        INFO_PRINTF1(_L("First Client resumed after pre-emption as expected"));
                        StopTest();
                        break;
                        }
                    
                    if(!iSetVolume)
                        {
                        iMMFDevSound->SetVolume (iExVolume);
                        iSetVolume = ETrue;
                        }
                    
	                CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
	                TInt err = iFile.Read (buffer->Data());
	                if (err != KErrNone)
	                    {
	                    ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
	                    StopTest(err);
	                    break;
	                    }
	                if (buffer->Data().Length ()!= buffer->RequestSize())
	                    {
	                    iBuffer->SetLastBuffer(ETrue);
	                    }
	                INFO_PRINTF1(_L("Calling CMMFDevSound::PlayData"));
	                iMMFDevSound->PlayData();
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
	            ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
	            StopTest(aError, EFail);
	            }
	        }
	    }

	void RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest::DoTimerCallback()
	    {
	    INFO_PRINTF1(_L("TimerEvent called"));
	    iTimer->Cancel();
	    INFO_PRINTF1(_L("Starting higher priority devsound client"));
	    iDevsoundToneClient->SetPriority(KMaximumPriority);
	    INFO_PRINTF1(_L("Initializing to play tones thru second devsound client"));
	    TInt err = iDevsoundToneClient->InitTonePlay(iFrequencyTone1,iDuration*KMicroSecsInOneSec);
	    if (err != KErrNone)
	        {
	        ERR_PRINTF2(_L("Initializating DevSoud to play tones thru second devsound client failed with error = %d"), err);
	        StopTest(err);
	        }
	    }

	void RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest::PlayError(TInt aError)
	    {
	    INFO_PRINTF1(_L("========== DevSound PlayError() callback =========="));
	    if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
	        {
	        INFO_PRINTF1(_L("========== Preempting 1st DevSound Instance=========="));
	        INFO_PRINTF2(_L("First DevSound client PlayError succeeded with error = %d "), aError);
	        }
	    else
	        {
	        ERR_PRINTF2(_L("First DevSound client called PlayError and failed with error = %d "), aError);
	        ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
	        StopTest(aError, EFail);
	        }
	    }

	void RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest::ClientInitializeCompleteCallback(TInt aError)
	    {
	    INFO_PRINTF1(_L("==========Second DevSound client called InitializeComplete()=========="));
	    if (aError == KErrNone)
	        {
	        INFO_PRINTF1(_L("Starting to play tone thru second devsound client"));
	        TInt err = iDevsoundToneClient->PlayTone ();
	        if (err != KErrNone)
	            {
	            ERR_PRINTF2(_L("Second Devsound client called PlayTone and failed with error = %d"), err);
	            StopTest(err);
	            }
	        }
	    else
	        {
	        ERR_PRINTF2(_L("Second Devsound InitializeComplete received an error = %d"), aError);
	        StopTest(aError);
	        }
	    }

	void RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest::ClientToneFinishedCallback(TInt aError)
	    {
	    INFO_PRINTF1(_L("========== Second  DevSound client called ToneFinished()=========="));
	    if (aError == KErrUnderflow)
	        {
	        INFO_PRINTF2(_L("Second DevSound client called ToneFinished and succeeded with error = %d"), aError);
            iDevSoundState = EStatePlaying;
            iResume = ETrue;
            TRAPD(err, iMMFDevSound->PlayInitL());
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("Call PlayInitL failed after Second DevSound client called ToneFinished with error = %d"), err);
                StopTest(aError);
                }
	        }
	    else
	        {
	        ERR_PRINTF2(_L("Second DevSound client called ToneFinished and failed with error = %d"), aError);
	        StopTest(aError);
	        }
	    }

