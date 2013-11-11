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

#include "char_a3f_devsound_tonetest.h"


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0001
 */
RA3FDevSoundToneSetConfigTest::RA3FDevSoundToneSetConfigTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iSampleRate(0),
		iChannels(0)
	{
	}

RA3FDevSoundToneSetConfigTest* RA3FDevSoundToneSetConfigTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetConfigTest * self = new(ELeave)RA3FDevSoundToneSetConfigTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetConfigTest::DoKickoffTestL()
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
	}


void RA3FDevSoundToneSetConfigTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			if(aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetConfigTest"), EFsmIncorrectErrorPassed));
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
				TMMFCapabilities capabilitiesSet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
				TBuf<KMaxChannelsStringLength> stringChannelsSet;
				TMMFSampleRate eSampleRate;
				SampleRateFromTIntToTMMFSampleRate (iSampleRate, eSampleRate);
				capabilitiesSet.iRate = eSampleRate;
				capabilitiesSet.iChannels = iChannels;
				SampleRateFromTUintToString (capabilitiesSet.iRate,	stringSampleRateSet);
				ChannelsFromTUintToString (capabilitiesSet.iChannels, stringChannelsSet);
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetConfigL using sample rate = %S and channel mode =%S"), &stringSampleRateSet, &stringChannelsSet);
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
				capabilitiesGet=iMMFDevSound->Config();
				SampleRateFromTUintToString (capabilitiesGet.iRate, stringSampleRateGet);
				ChannelsFromTUintToString (capabilitiesGet.iChannels, stringChannelsGet);
				if (capabilitiesGet.iRate != capabilitiesSet.iRate && capabilitiesGet.iChannels != capabilitiesSet.iChannels)
					{
					ERR_PRINTF3(_L("Retrieved sample rate and channel %S %S"), &stringSampleRateGet, &stringChannelsGet);
					ERR_PRINTF3(_L("Expected %S %S"), &stringSampleRateSet, &stringChannelsSet);
					StopTest(aError, EFail);
					break;
					}
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0002
 */
RA3FDevSoundToneSetPrioritySettingsTest::RA3FDevSoundToneSetPrioritySettingsTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iPriority(0)
	{
	}

RA3FDevSoundToneSetPrioritySettingsTest* RA3FDevSoundToneSetPrioritySettingsTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetPrioritySettingsTest * self = new(ELeave)RA3FDevSoundToneSetPrioritySettingsTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetPrioritySettingsTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KPriority, iPriority))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KPriority);
		StopTest(KErrUnknown);
		}
	}

void RA3FDevSoundToneSetPrioritySettingsTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetPrioritySettingsTest"), EFsmIncorrectErrorPassed));
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
				priority.iPriority = iPriority;
				INFO_PRINTF2(_L("Calling CMMFDevSound::SetPrioritySettings using a priority value = %d"), iPriority);
				iMMFDevSound->SetPrioritySettings (priority);
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0003
 */
RA3FDevSoundToneSetVolumeTest::RA3FDevSoundToneSetVolumeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iVolume(0)
	{
	}

RA3FDevSoundToneSetVolumeTest* RA3FDevSoundToneSetVolumeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetVolumeTest * self = new(ELeave)RA3FDevSoundToneSetVolumeTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetVolumeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrUnknown);
		return;
		}
	}

void RA3FDevSoundToneSetVolumeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetVolumeTest"), EFsmIncorrectErrorPassed));
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
				INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolume with volume value = %d"), iVolume);
				iMMFDevSound->SetVolume(iVolume);
				TInt volumeGet;
				volumeGet=iMMFDevSound->Volume();
				if (volumeGet == iVolume)
					{
					INFO_PRINTF2(_L("CMMFDevSound::SetVolume succeeded with value = %d"), volumeGet);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::SetVolume failed with value = %d"), volumeGet);
					ERR_PRINTF2(_L("Expected value = %d"), iVolume);
					StopTest(aError, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0004
 */
RA3FDevSoundToneVolumeRampTest::RA3FDevSoundToneVolumeRampTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iVolumeRamp(0),
		iDuration(0), iFrequencyTone1(0)
	{
	}

RA3FDevSoundToneVolumeRampTest* RA3FDevSoundToneVolumeRampTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneVolumeRampTest * self = new(ELeave)RA3FDevSoundToneVolumeRampTest(aTestName);
	return self;
	}

void RA3FDevSoundToneVolumeRampTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolumeRamp, iVolumeRamp))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolumeRamp);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
			{
			ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
			StopTest(KErrNotFound);
			return;
			}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundToneVolumeRampTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneVolumeRampTest"), EFsmIncorrectErrorPassed));
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
				INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolumeRamp using volume ramp time = %d"), iVolumeRamp);
				iMMFDevSound->SetVolumeRamp(iVolumeRamp);
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneVolumeRampTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iDuration * KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0005
 */
RA3FDevSoundToneSetToneRepeatsTest::RA3FDevSoundToneSetToneRepeatsTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0),
		iFrequencyTone1(0),
		iRepeatTrailingSilence(0),
		iRepeatCount(0)
	{
	}

RA3FDevSoundToneSetToneRepeatsTest* RA3FDevSoundToneSetToneRepeatsTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetToneRepeatsTest * self = new(ELeave)RA3FDevSoundToneSetToneRepeatsTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetToneRepeatsTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KRepeatCount, iRepeatCount))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatCount);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KRepeatTrailingSilence, iRepeatTrailingSilence))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatTrailingSilence);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundToneSetToneRepeatsTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetToneRepeatsTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetToneRepeats using repeatCount = %d and trailingSilence = %d"), iRepeatCount, iRepeatTrailingSilence);
				iMMFDevSound->SetToneRepeats(iRepeatCount, iRepeatTrailingSilence);
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetToneRepeatsTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt repeatTrailingSilenceToSecs = iRepeatTrailingSilence/KMicroSecsInOneSec;
		TInt totalExpectedReproductionTime = (iRepeatCount * iDuration + iDuration + (repeatTrailingSilenceToSecs*iRepeatCount))*KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF1(_L("Play tone repeats succeded."));
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF1(_L("Play tone repeats did not work as planned."));
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0006
 */
RA3FDevSoundToneSetDTMFLengthsTest::RA3FDevSoundToneSetDTMFLengthsTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iToneOnLength(0),
		iToneOffLength(0), iPauseLength(0), iDTMFPauses(0),
		iDTMFTones(0)
	{
	}

RA3FDevSoundToneSetDTMFLengthsTest* RA3FDevSoundToneSetDTMFLengthsTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetDTMFLengthsTest * self = new(ELeave)RA3FDevSoundToneSetDTMFLengthsTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetDTMFLengthsTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KToneOnLength, iToneOnLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOnLength);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KToneOffLength, iToneOffLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOffLength);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KPauseLength, iPauseLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KPauseLength);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	for(TInt i =0 ; i< iDTMFString.Length() ; i++)
		{
		if (iDTMFString[i] == ',')
			{
			iDTMFPauses++;
			}
		else
			{
			iDTMFTones++;
			}
		}
	}

void RA3FDevSoundToneSetDTMFLengthsTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetDTMFLengthsTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				TTimeIntervalMicroSeconds32 ToneOnLength(iToneOnLength);
				TTimeIntervalMicroSeconds32 ToneOffLength(iToneOffLength);
				TTimeIntervalMicroSeconds32 PauseLength(iPauseLength);
				INFO_PRINTF4(_L("Calling CMMFDevSound::SetDTMFLengths. ToneOnLength = %d, ToneOffLength = %d and PauseLength = %d"), ToneOnLength.Int(), ToneOffLength.Int(), PauseLength.Int());
				iMMFDevSound->SetDTMFLengths(ToneOnLength, ToneOffLength, PauseLength);
				iStartTime.HomeTime();
				INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetDTMFLengthsTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iToneOnLength*iDTMFTones + iToneOffLength * iDTMFTones + iDTMFPauses * iPauseLength;
		if (iPlayToneTime.Int64() < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime.Int64() > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF1(_L("Calling CMMFDevSound::SetDTMFLengths succeded."));
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF2(_L("Calling CMMFDevSound::SetDTMFLengths failed with %Ld"), iPlayToneTime.Int64());
			ERR_PRINTF2(_L("Expected time %d"), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0007
 */
RA3FDevSoundToneSetVolumeWhilePlayingTest::RA3FDevSoundToneSetVolumeWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iVolume(0), iFrequencyTone1(0)
	{
	}

RA3FDevSoundToneSetVolumeWhilePlayingTest* RA3FDevSoundToneSetVolumeWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetVolumeWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneSetVolumeWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetVolumeWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneSetVolumeWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetVolumeWhilePlayingTest"), EFsmIncorrectErrorPassed));
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
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using frequency = %d Hz and duration = %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetVolumeWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolume with volume value = %d"), iVolume);
	iMMFDevSound->SetVolume(iVolume);
	TInt volumeGet;
	volumeGet=iMMFDevSound->Volume();
	if (volumeGet == iVolume)
		{
		INFO_PRINTF2(_L("CMMFDevSound::SetVolume succeeded with value = %d"), volumeGet);
		}
	else
		{
		ERR_PRINTF2(_L("CMMFDevSound::SetVolume failed with value = %d"), volumeGet);
		ERR_PRINTF2(_L("Expected value = %d"), iVolume);
		StopTest(KErrNone, EFail);
		}
	}

void RA3FDevSoundToneSetVolumeWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iDuration * KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0008
 */
RA3FDevSoundToneSetPlayBalanceWhilePlayingTest::RA3FDevSoundToneSetPlayBalanceWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iVolume(0), iFrequencyTone1(0), iLSpeakerBalance(0),
		iRSpeakerBalance(0)

	{
	}

RA3FDevSoundToneSetPlayBalanceWhilePlayingTest* RA3FDevSoundToneSetPlayBalanceWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetPlayBalanceWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneSetPlayBalanceWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetPlayBalanceWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KLSpeakerBalance, iLSpeakerBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KLSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KRSpeakerBalance, iRSpeakerBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRSpeakerBalance);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneSetPlayBalanceWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetPlayBalanceWhilePlayingTest"), EFsmIncorrectErrorPassed));
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
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using frequency = %d Hz and duration = %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetPlayBalanceWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	INFO_PRINTF3(_L("Calling CMMFDevSound::SetPlayBalanceL using left = %d right = %d"), iLSpeakerBalance, iRSpeakerBalance);
	TRAPD(err, iMMFDevSound->SetPlayBalanceL(iLSpeakerBalance, iRSpeakerBalance));
	if (err == KErrNone)
		{
		TInt getBalanceL, getBalanceR;
		TRAPD(err, iMMFDevSound->GetPlayBalanceL(getBalanceL, getBalanceR));
		if (err == KErrNone)
			{
			if(iLSpeakerBalance == getBalanceL && iRSpeakerBalance == getBalanceR)
				{
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetPlayBalanceL to set balance values succeeded, left = %d right = %d"), getBalanceL, getBalanceR);
				}
			else
				{
				ERR_PRINTF3(_L("Calling CMMFDevSound::SetPlayBalanceL to set balance values failed, left = %d right = %d"), getBalanceL, getBalanceR);
				ERR_PRINTF3(_L("Expected balances values for right and left respectively %d, %d"),iLSpeakerBalance, iRSpeakerBalance);
				StopTest(err,EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF1(_L("Error in setting left and right play balance."));
		StopTest (err, EFail);
		}
	}

void RA3FDevSoundToneSetPlayBalanceWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iDuration * KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *  **========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0009
 */
RA3FDevSoundToneSetVolumeWhilePausePlayingTest::RA3FDevSoundToneSetVolumeWhilePausePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iVolume(0),
		iFrequencyTone1(0),
		iTimeToEnterPauseElapsed(EFalse)
	{
	}

RA3FDevSoundToneSetVolumeWhilePausePlayingTest* RA3FDevSoundToneSetVolumeWhilePausePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetVolumeWhilePausePlayingTest * self = new(ELeave)RA3FDevSoundToneSetVolumeWhilePausePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetVolumeWhilePausePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneSetVolumeWhilePausePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetVolumeWhilePausePlayingTest"), EFsmIncorrectErrorPassed));
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
				INFO_PRINTF3(_L("CMMFDevSound::PlayToneL %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"),  aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolume with volume value = %d"), iVolume);
				iMMFDevSound->SetVolume(iVolume);
				TInt volumeGet;
				volumeGet=iMMFDevSound->Volume();
				if (volumeGet == iVolume)
					{
					INFO_PRINTF2(_L("CMMFDevSound::SetVolume succeeded with value = %d"), volumeGet);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::SetVolume failed with value = %d"), volumeGet);
					ERR_PRINTF2(_L("Expected value = %d"), iVolume);
					StopTest(aError, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundToneSetVolumeWhilePausePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iTimeToEnterPauseElapsed)
		{
		iTimeToEnterPauseElapsed = ETrue;
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		iTimeToEnterPauseElapsed = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		}
	}

void RA3FDevSoundToneSetVolumeWhilePausePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to be paused"));
	StopTest(aError, EFail);
	}
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0010
 */
RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest::RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iVolume(0),
		iReinitialized(EFalse)
	{
	}

RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest* RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest * self = new(ELeave)RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest"), EFsmIncorrectErrorPassed));
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
				if(!iReinitialized)
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
					iReinitialized = ETrue;
					INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolume with volume value = %d"), iVolume);
					iMMFDevSound->SetVolume(iVolume);
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					TInt volumeGet;
					volumeGet=iMMFDevSound->Volume();
					if (volumeGet == iVolume)
						{
						INFO_PRINTF1(_L("After calling CMMFDevSound::InitializeL for playing audio, "));
						INFO_PRINTF2(_L("the volume which was set when calling CMMFDevSound::InitializeL for playing tones is kept. Volume = %d"), volumeGet);
						StopTest();
						}
					else
						{
						INFO_PRINTF1(_L("After calling CMMFDevSound::InitializeL for playing audio, "));
						INFO_PRINTF2(_L("the volume which was set when calling CMMFDevSound::InitializeL for playing tones is  NOT kept. Volume = %d"), volumeGet);
						ERR_PRINTF2(_L("Expected value = %d"), iVolume);
						StopTest(aError, EFail);
						}
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"),  aDevSoundEvent);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0011
 */
RA3FDevSoundToneSetVolumeRampWhilePlayingTest::RA3FDevSoundToneSetVolumeRampWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iVolume(0), iFrequencyTone1(0),iTonePlayedTwice(EFalse),
		iTimesToneFinishedCalled(0)
	{
	}

RA3FDevSoundToneSetVolumeRampWhilePlayingTest* RA3FDevSoundToneSetVolumeRampWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetVolumeRampWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneSetVolumeRampWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetVolumeRampWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KVolumeRamp, iVolumeRamp))
			{
			ERR_PRINTF2(KMsgErrorGetParameter, &KVolumeRamp);
			StopTest(KErrNotFound);
			return;
			}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneSetVolumeRampWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetVolumeRampWhilePlayingTest"), EFsmIncorrectErrorPassed));
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
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
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using frequency = %d Hz and duration = %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Play tone left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetVolumeRampWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolumeRamp using volume ramp time = %d"), iVolumeRamp);
	iMMFDevSound->SetVolumeRamp(iVolumeRamp);
	}

void RA3FDevSoundToneSetVolumeRampWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		TInt totalExpectedReproductionTime;
		iTimesToneFinishedCalled++;
		totalExpectedReproductionTime = iDuration * iTimesToneFinishedCalled * KMicroSecsInOneSec;
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		if(!iTonePlayedTwice)
			{
			INFO_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
			INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using frequency = %d Hz and duration = %d secs "), iFrequencyTone1, iDuration);
			TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CMMFDevSound::PlayToneL with error : %d!"), err);
				StopTest(err);
				}
			iTonePlayedTwice = ETrue;
			}
		else if (iPlayToneTime < totalExpectedReproductionTime + (2*KPlayVarianceTime) && iPlayToneTime > totalExpectedReproductionTime - (2*KPlayVarianceTime))
			{
			INFO_PRINTF2(_L("DevSound called ToneFinished twice with error = %d"), aError);
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0012
 */
RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest::RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0),
		iFrequencyTone1(0),
		iRepeatTrailingSilence(0),
		iRepeatCount(0)
	{
	}

RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest* RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KRepeatCount, iRepeatCount))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatCount);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KRepeatTrailingSilence, iRepeatTrailingSilence))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatTrailingSilence);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetToneRepetitionsWhilePlayingConfigurationTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	INFO_PRINTF3(_L("Calling CMMFDevSound::SetToneRepeats using RepeatCount = %d and RepeatTrailingSilence = %d microsecs"), iRepeatCount, iRepeatTrailingSilence);
	iMMFDevSound->SetToneRepeats(iRepeatCount, iRepeatTrailingSilence);
	}

void RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	const TInt KtimesPlayToneIsCalled = 2;
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt repeatTrailingSilenceToSecs = iRepeatTrailingSilence/KMicroSecsInOneSec;
		TInt totalExpectedReproductionTime = (KtimesPlayToneIsCalled*iDuration+iRepeatCount*iDuration + (repeatTrailingSilenceToSecs*iRepeatCount))*KMicroSecsInOneSec;
		if(iPlayToneTime < ((iDuration*KMicroSecsInOneSec) + KPlayVarianceTime)&& iPlayToneTime > (iDuration*KMicroSecsInOneSec) - KPlayVarianceTime)
			{
			INFO_PRINTF1(_L("First play tone succeded."));
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CMMFDevSound::PlayToneL failed with error : %d!"), err);
				StopTest(err);
				}
			}
		else if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF1(_L("Play tone repeats succeded."));
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF1(_L("Play tone repeats did not work as planned."));
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0013
 */
RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest::RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iFirstToneFinishedCall(EFalse),
		iToneOnLength(0), iToneOffLength(0),
		iPauseLength(0), iDTMFString(KNullDesC),
		iDTMFPauses(0), iDTMFTones(0),
		iTotalExpectedPlayToneTime(0)
	{
	}
RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest* RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KToneOnLength, iToneOnLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOnLength);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KToneOffLength, iToneOffLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOffLength);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KPauseLength, iPauseLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KPauseLength);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	for(TInt i =0 ; i< iDTMFString.Length() ; i++)
		{
		if (iDTMFString[i] == ',')
			{
			iDTMFPauses++;
			}
		else
			{
			iDTMFTones++;
			}
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetDTMFToneRepetitionsWhilePlayingConfigurationTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				iStartTime.HomeTime();
				INFO_PRINTF2(_L("Calling CMMFDevSound::L %S"), &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState=EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	TTimeIntervalMicroSeconds32 ToneOnLength(iToneOnLength);
	TTimeIntervalMicroSeconds32 ToneOffLength(iToneOffLength);
	TTimeIntervalMicroSeconds32 PauseLength(iPauseLength);
	INFO_PRINTF4(_L("Calling CMMFDevSound::SetDTMFLengths. ToneOnLength = %d, ToneOffLength = %d and PauseLength = %d"), ToneOnLength.Int(), ToneOffLength.Int(), PauseLength.Int());
	iMMFDevSound->SetDTMFLengths(ToneOnLength, ToneOffLength, PauseLength);
	}

void RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		if (!iFirstToneFinishedCall)
			{
			INFO_PRINTF1(_L("First play tone succeded"));
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			iFirstToneFinishedCall = ETrue;
			INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
			TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error = %d"), err);
				StopTest(err);
				}
			iTotalExpectedPlayToneTime = iToneOnLength*iDTMFTones + iToneOffLength * iDTMFTones + iDTMFPauses * iPauseLength + iPlayToneTime.Int64();
			return;
			}
		if (iPlayToneTime.Int64() < iTotalExpectedPlayToneTime + KPlayVarianceTime && iPlayToneTime.Int64() > iTotalExpectedPlayToneTime - KPlayVarianceTime)
			{
			INFO_PRINTF1(_L("Calling CMMFDevSound::SetDTMFLengths succeded."));
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF2(_L("Calling CMMFDevSound::SetDTMFLengths failed with %Ld"), iPlayToneTime.Int64());
			ERR_PRINTF2(_L("Expected time %d"), iTotalExpectedPlayToneTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %Ld"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0014
 */
RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest::RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0),
		iRepeatTrailingSilence(0), iRepeatCount(0),
		iInitializedToPlayTones(EFalse)
	{
	}

RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest* RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest * self = new(ELeave)RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KRepeatCount, iRepeatCount))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatCount);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KRepeatTrailingSilence, iRepeatTrailingSilence))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatTrailingSilence);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeConfigurationTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
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
				if(!iInitializedToPlayTones)
					{
					INFO_PRINTF3(_L("Calling CMMFDevSound::SetToneRepeats using RepeatCount = %d and RepeatTrailingSilence = %d microsecs"), iRepeatCount, iRepeatTrailingSilence);
					iMMFDevSound->SetToneRepeats(iRepeatCount, iRepeatTrailingSilence);
					INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL for playing tones"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
						StopTest(err);
						break;
						}
					iInitializedToPlayTones = ETrue;
					}
				else
					{
					iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
					iStartTime.HomeTime();
					INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL %d Hz during %d secs "), iFrequencyTone1, iDuration);
					TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
					iDevSoundState = EStatePlaying;
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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


void RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt repeatTrailingSilenceToSecs = iRepeatTrailingSilence/KMicroSecsInOneSec;
		TInt totalExpectedReproductionTime = (iDuration + iRepeatCount * iDuration+ (repeatTrailingSilenceToSecs*iRepeatCount)) * KMicroSecsInOneSec;
		if(iPlayToneTime < (totalExpectedReproductionTime + KPlayVarianceTime) && iPlayToneTime > (totalExpectedReproductionTime - KPlayVarianceTime))
			{
			INFO_PRINTF1(_L("Play tone repeats succeded."));
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF1(_L("Play tone repeats did not work as planned."));
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0015
 */
RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest::RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iInitializedToPlayTones(EFalse),
		iDTMFString(KNullDesC), iToneOnLength(0),
		iToneOffLength(0), iPauseLength(0),
		iDTMFPauses(0), iDTMFTones(0)

	{
	}

RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest* RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest * self = new(ELeave)RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KToneOnLength, iToneOnLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOnLength);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KToneOffLength, iToneOffLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOffLength);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KPauseLength, iPauseLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KPauseLength);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	for(TInt i =0 ; i< iDTMFString.Length() ; i++)
		{
		if (iDTMFString[i] == ',')
			{
			iDTMFPauses++;
			}
		else
			{
			iDTMFTones++;
			}
		}
	}

void RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeConfigurationTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording));
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
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"),  aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iInitializedToPlayTones)
					{
					TTimeIntervalMicroSeconds32 ToneOnLength(iToneOnLength);
					TTimeIntervalMicroSeconds32 ToneOffLength(iToneOffLength);
					TTimeIntervalMicroSeconds32 PauseLength(iPauseLength);
					INFO_PRINTF4(_L("Calling CMMFDevSound::SetDTMFLengths. ToneOnLength = %d, ToneOffLength = %d and PauseLength = %d"), ToneOnLength.Int(), ToneOffLength.Int(), PauseLength.Int());
					iMMFDevSound->SetDTMFLengths(ToneOnLength, ToneOffLength, PauseLength);
					INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
						StopTest(err);
						break;
						}
					iInitializedToPlayTones = ETrue;
					}
				else
					{
					iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
					iStartTime.HomeTime();
					INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
					TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Play DTMF string failed with error : %d!"), err);
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
					iDevSoundState = EStatePlaying;
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iToneOnLength*iDTMFTones + iToneOffLength * iDTMFTones + iDTMFPauses * iPauseLength;
		if (iPlayToneTime.Int64() < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime.Int64() > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF1(_L("CMMFDevSound::SetDTMFLengths settings were remembered after re-initialization."));
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF1(_L("CMMFDevSound::SetDTMFLengths settings were not remembered after re-initialization."));
			ERR_PRINTF2(_L("Expected time %d"), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0016
 */
RA3FDevSoundToneSetConfigWhilePlayingTest::RA3FDevSoundToneSetConfigWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iVolume(0), iFrequencyTone1(0),
		iSampleRate(0), iChannels(0)
	{
	}

RA3FDevSoundToneSetConfigWhilePlayingTest* RA3FDevSoundToneSetConfigWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetConfigWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneSetConfigWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetConfigWhilePlayingTest::DoKickoffTestL()
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
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneSetConfigWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetConfigWhilePlayingTest"), EFsmIncorrectErrorPassed));
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
				StartTimer(KMicroSecsTwoSec);
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
				INFO_PRINTF1(_L("CMMFDevSound::PlayToneL"));
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1,  iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetConfigWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	TMMFCapabilities capabilitiesSet;
	TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
	TBuf<KMaxChannelsStringLength> stringChannelsSet;
	TMMFSampleRate eSampleRate;
	SampleRateFromTIntToTMMFSampleRate (iSampleRate, eSampleRate);
	capabilitiesSet.iRate = eSampleRate;
	capabilitiesSet.iChannels = iChannels;
	SampleRateFromTUintToString (capabilitiesSet.iRate,	stringSampleRateSet);
	ChannelsFromTUintToString (capabilitiesSet.iChannels, stringChannelsSet);
	INFO_PRINTF3(_L("Calling CMMFDevSound::SetConfigL using sample rate = %S and channel mode = %S"), &stringSampleRateSet, &stringChannelsSet);
	TRAPD(err, iMMFDevSound->SetConfigL(capabilitiesSet));
	if (err == KErrNotReady)
		{
		INFO_PRINTF2(_L("CMMFDevSound::SetConfigL succeeded with error = %d"), err);
		StopTest(err, EPass);
		}
	else
		{
		ERR_PRINTF2(_L("CMMFDevSound::SetConfigL failed with error = %d"), err);
		ERR_PRINTF2(_L("Expected  error = %d"), KErrNotReady);
		StopTest(err, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0017
 */
RA3FDevSoundToneSetPriorityWhilePlayingTest::RA3FDevSoundToneSetPriorityWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iFrequencyTone1(0)
	{
	}

RA3FDevSoundToneSetPriorityWhilePlayingTest* RA3FDevSoundToneSetPriorityWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetPriorityWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneSetPriorityWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetPriorityWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundToneClient = CA3FDevSoundToneClient::NewL(*this);
	}

void RA3FDevSoundToneSetPriorityWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetPriorityWhilePlayingConfigurationTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume ());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneL"));
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				INFO_PRINTF2(_L("Calling CMMFDevSound::SetPrioritySettings using a priority(highest) value = %d"), KMaximumPriority);
				TMMFPrioritySettings priority;
				priority.iPriority = KMaximumPriority;
				iMMFDevSound->SetPrioritySettings(priority);
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError,  EFail);
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

void RA3FDevSoundToneSetPriorityWhilePlayingTest::ClientInitializeCompleteCallback(TInt aError)
	{
	INFO_PRINTF1(_L("==========Second DevSound client called InitializeComplete()=========="));
	if (aError == KErrNone)
		{
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

void RA3FDevSoundToneSetPriorityWhilePlayingTest::ClientToneFinishedCallback(TInt aError)
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
		StopTest(aError, EFail);
		}
	}

void RA3FDevSoundToneSetPriorityWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("Starting another devsound client using the highest priority"));
	iDevsoundToneClient->SetPriority(KMaximumPriority);
	TInt err = iDevsoundToneClient->InitTonePlay(iFrequencyTone1, iDuration*KMicroSecsInOneSec);
	if(err != KErrNone)
		{
		ERR_PRINTF1(_L("Initialization to play tone thru second devsound client failed"));
		StopTest(err);
		}
	}

void RA3FDevSoundToneSetPriorityWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("========== Preempting 1st DevSound Instance=========="));
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished and failed with error = %d"), aError);
		ERR_PRINTF4(_L("Expected errors posibilities %d %d %d"), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0018
 */
RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue::RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iSampleRate(0),
		iChannels(0)
	{
	}

RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue* RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue * self = new(ELeave)RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue(aTestName);
	return self;
	}

void RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue::DoKickoffTestL()
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
	}

void RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
#ifdef MM_TB101_L
		//Reason: Check remains since this is a valid test case, but we need to know why we are not testing invalid rates.
		INFO_PRINTF1(_L("MM_TB101_L defined: All sample rates are valid for MM_TB101_L (HW), therefore no invalid rates can be tested."));
		StopTest(KErrNotSupported, EPass);
#else
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			if(aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue"), EFsmIncorrectErrorPassed));
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
				TMMFCapabilities capabilitiesSet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
				TBuf<KMaxChannelsStringLength> stringChannelsSet;
				TMMFSampleRate eSampleRate;
				SampleRateFromTIntToTMMFSampleRate (iSampleRate, eSampleRate);
				capabilitiesSet.iRate = eSampleRate;
				capabilitiesSet.iChannels = iChannels;
				SampleRateFromTUintToString (capabilitiesSet.iRate,	stringSampleRateSet);
				ChannelsFromTUintToString (capabilitiesSet.iChannels, stringChannelsSet);
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetConfigL using an invalid sample rate = %S and channel mode = %S"), &stringSampleRateSet, &stringChannelsSet);
				TRAPD(err, iMMFDevSound->SetConfigL(capabilitiesSet));
				if (err == KErrNotSupported || err == KErrArgument)
					{
					INFO_PRINTF2(_L("CMMFDevSound::SetConfigL succeeded with error = %d"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::SetConfigL failed with error = %d"), err);
					ERR_PRINTF3(_L("Expected  error posibilities %d %d"), KErrArgument, KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
#endif// MM_TB101_L
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0019
 */
RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest::RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iSampleRate(0),
		iChannels(0)
	{
	}

RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest* RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest * self = new(ELeave)RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest::DoKickoffTestL()
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
	}

void RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			if(aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest"), EFsmIncorrectErrorPassed));
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
				TMMFCapabilities capabilitiesSet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
				TBuf<KMaxChannelsStringLength> stringChannelsSet;
				TMMFSampleRate eSampleRate;
				SampleRateFromTIntToTMMFSampleRate (iSampleRate, eSampleRate);
				capabilitiesSet.iRate = eSampleRate;
				capabilitiesSet.iChannels = iChannels;
				SampleRateFromTUintToString (capabilitiesSet.iRate,	stringSampleRateSet);
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetConfigL using a sample rate = %S and an invalid channel mode = %d"), &stringSampleRateSet, iChannels);
				TRAPD(err, iMMFDevSound->SetConfigL(capabilitiesSet));
				if (err == KErrNotSupported || err == KErrArgument)
					{
					INFO_PRINTF2(_L("CMMFDevSound::SetConfigL succeeded with error = %d"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::SetConfigL failed with error = %d"), err);
					ERR_PRINTF3(_L("Expected  error posibilities %d %d"), KErrArgument, KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0020
 */
RA3FDevSoundToneSetDTMFLengthsToNegativeTest::RA3FDevSoundToneSetDTMFLengthsToNegativeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iToneOnLength(0),
		iToneOffLength(0), iPauseLength(0),
		iDTMFString(KNullDesC)
	{
	}

RA3FDevSoundToneSetDTMFLengthsToNegativeTest* RA3FDevSoundToneSetDTMFLengthsToNegativeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetDTMFLengthsToNegativeTest * self = new(ELeave)RA3FDevSoundToneSetDTMFLengthsToNegativeTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetDTMFLengthsToNegativeTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KToneOnLength, iToneOnLength) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOnLength);
		StopTest(KErrNotFound);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KToneOffLength, iToneOffLength) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOffLength);
		StopTest(KErrNotFound);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KPauseLength, iPauseLength) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KPauseLength);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC DTMF;
	if ( !GetStringFromConfig(iTestStepName,  KDTMFString,  DTMF) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	}

void RA3FDevSoundToneSetDTMFLengthsToNegativeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetDTMFLengthsToNegativeConfigurationTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling iMMFDevSound::InitializeL"));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				TTimeIntervalMicroSeconds32 ToneOnLength(iToneOnLength);
				TTimeIntervalMicroSeconds32 ToneOffLength(iToneOffLength);
				TTimeIntervalMicroSeconds32 PauseLength(iPauseLength);
				INFO_PRINTF4(_L("Setting DTMF tone to negative lengths. ToneOnLength %d, ToneOffLength %d and PauseLength %d"), iToneOnLength, iToneOffLength, iPauseLength);
				iMMFDevSound->SetDTMFLengths(ToneOnLength, ToneOffLength, PauseLength);
				iStartTime.HomeTime();
				INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error : %d!"),  err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetDTMFLengthsToNegativeTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	INFO_PRINTF3(KMsgErrorDevSoundCallback, &KToneFinishedText, aError);

	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		if(iPlayToneTime <= KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Setting DTMF tone lengths to negative gives a reproduction time of %Ld microseconds which is the expected result"), iPlayToneTime.Int64());
			StopTest(aError, EPass);
			}
		}
	else
		{
		INFO_PRINTF2(_L("DevSound called ToneFinished with error = %d that was NOT expected"), aError);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0021
 */
RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest::RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iFirstToneFinishedCall(EFalse),
		iToneOnLength(0), iToneOffLength(0),
		iPauseLength(0), iDTMFString(KNullDesC)
	{
	}
RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest* RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest * self = new(ELeave)RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KToneOnLength, iToneOnLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOnLength);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KToneOffLength, iToneOffLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KToneOffLength);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KPauseLength, iPauseLength))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KPauseLength);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				iStartTime.HomeTime();
				TTimeIntervalMicroSeconds32 ToneOnLength(iToneOnLength);
				TTimeIntervalMicroSeconds32 ToneOffLength(iToneOffLength);
				TTimeIntervalMicroSeconds32 PauseLength(iPauseLength);
				INFO_PRINTF4(_L("Calling CMMFDevSound::SetDTMFLengths. ToneOnLength = %Ld, ToneOffLength = %Ld and PauseLength = %Ld"), iToneOnLength, iToneOffLength, iPauseLength);
				iMMFDevSound->SetDTMFLengths(ToneOnLength, ToneOffLength, PauseLength);
				INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicrosecsInTenSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState=EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	iEndTime.HomeTime();
	iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
	INFO_PRINTF1(_L("SetDTMF lengths succeded."));
	INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
	StopTest();
	}

void RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		INFO_PRINTF1(_L("ToneFinished was called before the timer event."));
		StopTest(aError);
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0022
 */
RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest::RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0),
		iRepeatTrailingSilence(0), iRepeatCount(0),
		iInitializedToPlayTones(EFalse)
	{
	}

RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest* RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest * self = new(ELeave)RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KRepeatCount, iRepeatCount))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatCount);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KRepeatTrailingSilence, iRepeatTrailingSilence))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatTrailingSilence);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetToneRepeats using RepeatCount = %d and RepeatTrailingSilence = %d microsecs"), iRepeatCount, iRepeatTrailingSilence);
				iMMFDevSound->SetToneRepeats(iRepeatCount, iRepeatTrailingSilence);
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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


void RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt repeatTrailingSilenceToSecs = iRepeatTrailingSilence/KMicroSecsInOneSec;
		TInt totalExpectedReproductionTime = (iRepeatCount * iDuration + iDuration + (repeatTrailingSilenceToSecs*iRepeatCount))*KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF1(_L("Play tone repeats succeded."));
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF1(_L("Play tone repeats did not work as planned."));
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0023
 */
RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest::RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0),
		iRepeatTrailingSilence(0), iRepeatCount(0),
		iInitializedToPlayTones(EFalse)
	{
	}

RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest* RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest * self = new(ELeave)RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest(aTestName);
	return self;
	}

void RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KRepeatCount, iRepeatCount))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatCount);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KRepeatTrailingSilence, iRepeatTrailingSilence))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRepeatTrailingSilence);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicrosecsInTenSec);
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
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetToneRepeats using RepeatCount = %d and RepeatTrailingSilence = %d microsecs"), iRepeatCount, iRepeatTrailingSilence);
				iMMFDevSound->SetToneRepeats(iRepeatCount, iRepeatTrailingSilence);
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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


void RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	iEndTime.HomeTime();
	iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
	INFO_PRINTF1(_L("Setting repeatCount value to KMdaRepeatForeverTest succeded."));
	INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
	iMMFDevSound->Stop();
	StopTest();
	}

void RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		INFO_PRINTF1(_L("ToneFinished was called before the timer event."));
		StopTest(aError);
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0024
 */
RA3FDevSoundToneCapsTest::RA3FDevSoundToneCapsTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iExCapRate(0), iExCapChannels(0)
	{
	}

RA3FDevSoundToneCapsTest* RA3FDevSoundToneCapsTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneCapsTest * self = new(ELeave)RA3FDevSoundToneCapsTest(aTestName);
	return self;
	}

void RA3FDevSoundToneCapsTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExCapRate, iExCapRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExCapChannels, iExCapChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundToneCapsTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneCapsTest"), EFsmIncorrectErrorPassed));
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
				StopTest(aError,  EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				TMMFCapabilities capabilities;
				INFO_PRINTF1(_L("Calling CMMFDevSound::Capabilities"));
				capabilities = iMMFDevSound->Capabilities();
				PrintSupportedCapabilities(capabilities.iRate, capabilities.iChannels);
				if(capabilities.iRate == iExCapRate)
					{
					INFO_PRINTF2(_L("The total of possible sample rate values is 0x%08x"), capabilities.iRate);
					}
				else
					{
					ERR_PRINTF2(_L("The total of possible sample rate values failed with 0x%08x"), capabilities.iRate);
					ERR_PRINTF2(_L("Expected value = %d"), iExCapRate);
					StopTest(aError, EFail);
					}
				if(capabilities.iChannels == iExCapChannels)
					{
					INFO_PRINTF2(_L("The total of possible channel values is 0x%08x"),capabilities.iChannels );
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("The total of possible channel values failed with 0x%08x"), capabilities.iChannels );
					ERR_PRINTF2(_L("Expected value = %d"), iExCapChannels);
					StopTest(aError, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0025
 */
RA3FDevSoundToneConfigDefaultTest::RA3FDevSoundToneConfigDefaultTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iExCapRate(0),
		iExCapChannels(0)
	{
	}

RA3FDevSoundToneConfigDefaultTest* RA3FDevSoundToneConfigDefaultTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneConfigDefaultTest * self = new(ELeave)RA3FDevSoundToneConfigDefaultTest(aTestName);
	return self;
	}

void RA3FDevSoundToneConfigDefaultTest::DoKickoffTestL()
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
	}

void RA3FDevSoundToneConfigDefaultTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
		switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneConfigDefaultTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState=EStateInitializing;
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
				INFO_PRINTF1(_L("Calling CMMFDevSound::Config"));
				capabilitiesGet=iMMFDevSound->Config();
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
						StopTest(aError, EFail);
						}
					}
				else
					{
					ERR_PRINTF2(_L("Sample rate value failed with %S"), &stringSampleRateGet);
					ERR_PRINTF2(_L("Expected sample rate %S"), &stringSampleRateExpected);
					StopTest(aError, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0026
 */
RA3FDevSoundToneMaxVolumeTest::RA3FDevSoundToneMaxVolumeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iExVolume(0)
	{
	}

RA3FDevSoundToneMaxVolumeTest* RA3FDevSoundToneMaxVolumeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneMaxVolumeTest * self = new(ELeave)RA3FDevSoundToneMaxVolumeTest(aTestName);
	return self;
	}

void RA3FDevSoundToneMaxVolumeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundToneMaxVolumeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneMaxVolumeTest"), EFsmIncorrectErrorPassed));
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
				INFO_PRINTF1(_L("Calling CMMFDevSound::MaxVolume"));
				TInt maxVolumeGet = iMMFDevSound->MaxVolume();
				if (maxVolumeGet == iExVolume)
					{
					INFO_PRINTF2(_L("The max device supported volume is %d."), maxVolumeGet);
					StopTest();
					}
				else
					{
					INFO_PRINTF2(_L("The max device supported volume failed with %d."), maxVolumeGet);
					ERR_PRINTF2(_L("Expected value = %d."), iExVolume);
					StopTest(aError, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0027
 */
RA3FDevSoundToneVolumeTest::RA3FDevSoundToneVolumeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iExVolume(0)
	{
	}

RA3FDevSoundToneVolumeTest* RA3FDevSoundToneVolumeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneVolumeTest * self = new(ELeave)RA3FDevSoundToneVolumeTest(aTestName);
	return self;
	}

void RA3FDevSoundToneVolumeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundToneVolumeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneVolumeTest"), EFsmIncorrectErrorPassed));
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
				INFO_PRINTF1(_L("Calling CMMFDevSound::Volume"));
				TInt volumeGet = iMMFDevSound->Volume();
				if(volumeGet == iExVolume)
					{
					INFO_PRINTF2(_L("Default device volume succeeded with = %d"), volumeGet);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Default device volume failed with = %d"), volumeGet);
					ERR_PRINTF2(_L("Expected value % d"), iExVolume);
					StopTest(aError , EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0028
 */
RA3FDevSoundToneQueryCapsWhilePlayingTest::RA3FDevSoundToneQueryCapsWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iExCapRate(0), iExCapChannels(0)
	{
	}

RA3FDevSoundToneQueryCapsWhilePlayingTest* RA3FDevSoundToneQueryCapsWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneQueryCapsWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneQueryCapsWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneQueryCapsWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExCapRate, iExCapRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExCapChannels, iExCapChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneQueryCapsWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneQueryCapsWhilePlayingTest"), EFsmIncorrectErrorPassed));
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
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneQueryCapsWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	TMMFCapabilities capabilities;
	INFO_PRINTF1(_L("Calling CMMFDevSound::Capabilities"));
	capabilities = iMMFDevSound->Capabilities();
	PrintSupportedCapabilities(capabilities.iRate, capabilities.iChannels);
	if(capabilities.iRate == iExCapRate)
		{
		INFO_PRINTF2(_L("The total of possible sample rate values is 0x%08x"), capabilities.iRate);
		}
	else
		{
		ERR_PRINTF2(_L("The total of possible sample rate values failed with 0x%08x"), capabilities.iRate);
		ERR_PRINTF2(_L("Expected value = %d"), iExCapRate);
		StopTest(KErrNone, EFail);
		}
	if(capabilities.iChannels == iExCapChannels)
		{
		INFO_PRINTF2(_L("The total of possible channel values is 0x%08x"),capabilities.iChannels );
		}
	else
		{
		ERR_PRINTF2(_L("The total of possible channel values failed with 0x%08x"), capabilities.iChannels );
		ERR_PRINTF2(_L("Expected value = %d"), iExCapChannels);
		StopTest(KErrNone, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0029
 */
RA3FDevSoundToneConfigWhilePlayingTest::RA3FDevSoundToneConfigWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0)
	{
	}
RA3FDevSoundToneConfigWhilePlayingTest* RA3FDevSoundToneConfigWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneConfigWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneConfigWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneConfigWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
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
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneConfigWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneConfigWhilePlayingTest"), EFsmIncorrectErrorPassed));
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
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"),  err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneConfigWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	TMMFCapabilities capabilitiesGet;
	TBuf<KMaxSampleRateStringLength> stringSampleRateGet;
	TBuf<KMaxChannelsStringLength> stringChannelsGet;
	TBuf<KMaxSampleRateStringLength> stringSampleRateExpected;
	TBuf<KMaxChannelsStringLength> stringChannelsExpected;
	INFO_PRINTF1(_L("Calling CMMFDevSound::Config while playing"));
	capabilitiesGet=iMMFDevSound->Config();
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

void RA3FDevSoundToneConfigWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iDuration * KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0030
 */
RA3FDevSoundToneMaxVolumeWhilePlayingTest::RA3FDevSoundToneMaxVolumeWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iExVolume(0)
	{
	}

RA3FDevSoundToneMaxVolumeWhilePlayingTest* RA3FDevSoundToneMaxVolumeWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneMaxVolumeWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneMaxVolumeWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneMaxVolumeWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneMaxVolumeWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneMaxVolumeWhilePlayingTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"),  err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneMaxVolumeWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	INFO_PRINTF1(_L("Calling CMMFDevSound::MaxVolume"));
	TInt maxVolumeGet = iMMFDevSound->MaxVolume();
	if (maxVolumeGet == iExVolume)
		{
		INFO_PRINTF2(_L("The max device supported volume is %d."), maxVolumeGet);
		}
	else
		{
		INFO_PRINTF2(_L("The max device supported volume failed with %d."), maxVolumeGet);
		ERR_PRINTF2(_L("Expected value = %d."), iExVolume);
		StopTest(KErrNone, EFail);
		}
	}

void RA3FDevSoundToneMaxVolumeWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iDuration * KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0031
 */
RA3FDevSoundToneVolumeWhilePlayingTest::RA3FDevSoundToneVolumeWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iExVolume(0)
	{
	}

RA3FDevSoundToneVolumeWhilePlayingTest* RA3FDevSoundToneVolumeWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneVolumeWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneVolumeWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneVolumeWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneVolumeWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneVolumeWhilePlayingTest"), EFsmIncorrectErrorPassed));
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
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundToneVolumeWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	INFO_PRINTF1(_L("Calling CMMFDevSound::Volume"));
	TInt maxVolumeGet = iMMFDevSound->Volume();
	if (maxVolumeGet == iExVolume)
		{
		INFO_PRINTF2(_L("The device default supported volume value is %d."), maxVolumeGet);
		}
	else
		{
		INFO_PRINTF2(_L("The device default supported volume value failed with %d."), maxVolumeGet);
		ERR_PRINTF2(_L("Expected value = %d."), iExVolume);
		StopTest(KErrNone, EFail);
		}
	}

void RA3FDevSoundToneVolumeWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iDuration * KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0032
 */
RA3FDevSoundToneQueryCapsWhilePausePlayingTest::RA3FDevSoundToneQueryCapsWhilePausePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iExCapRate(0), iExCapChannels(0),
		iTimeToEnterPauseElapsed(EFalse)
	{
	}

RA3FDevSoundToneQueryCapsWhilePausePlayingTest* RA3FDevSoundToneQueryCapsWhilePausePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneQueryCapsWhilePausePlayingTest * self = new(ELeave)RA3FDevSoundToneQueryCapsWhilePausePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneQueryCapsWhilePausePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExCapRate, iExCapRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExCapChannels, iExCapChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneQueryCapsWhilePausePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneQueryCapsWhilePausePlayingTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if(aDevSoundEvent == EEventTimerComplete)
				{
				TMMFCapabilities capabilities;
				INFO_PRINTF1(_L("Calling CMMFDevSound::Capabilities"));
				capabilities = iMMFDevSound->Capabilities();
				PrintSupportedCapabilities(capabilities.iRate, capabilities.iChannels);
				if(capabilities.iRate == iExCapRate)
					{
					INFO_PRINTF2(_L("The total of possible sample rate values is 0x%08x"), capabilities.iRate);
					if(capabilities.iChannels == iExCapChannels)
						{
						INFO_PRINTF2(_L("The total of possible channel values is 0x%08x"),capabilities.iChannels );
						StopTest();
						}
					else
						{
						ERR_PRINTF2(_L("The total of possible channel values failed with 0x%08x"), capabilities.iChannels );
						ERR_PRINTF2(_L("Expected value = %d"), iExCapChannels);
						StopTest(aError, EFail);
						}
					}
				else
					{
					ERR_PRINTF2(_L("The total of possible sample rate values failed with 0x%08x"), capabilities.iRate);
					ERR_PRINTF2(_L("Expected value = %d"), iExCapRate);
					StopTest(aError, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundToneQueryCapsWhilePausePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iTimeToEnterPauseElapsed)
		{
		iTimeToEnterPauseElapsed = ETrue;
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		iTimeToEnterPauseElapsed = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		}
	}

void RA3FDevSoundToneQueryCapsWhilePausePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to be paused then stopped"));
	StopTest(aError, EFail);
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0033
 */
RA3FDevSoundToneConfigWhilePausePlayingTest::RA3FDevSoundToneConfigWhilePausePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iTimeToEnterPauseElapsed(EFalse),
		iExCapRate(0), iExCapChannels(0)
	{
	}

RA3FDevSoundToneConfigWhilePausePlayingTest* RA3FDevSoundToneConfigWhilePausePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneConfigWhilePausePlayingTest * self = new(ELeave)RA3FDevSoundToneConfigWhilePausePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneConfigWhilePausePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
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
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneConfigWhilePausePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneConfigWhilePausePlayingTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if(aDevSoundEvent == EEventTimerComplete)
				{
				TMMFCapabilities capabilitiesGet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateGet;
				TBuf<KMaxChannelsStringLength> stringChannelsGet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateExpected;
				TBuf<KMaxChannelsStringLength> stringChannelsExpected;
				INFO_PRINTF1(_L("Calling CMMFDevSound::Config while playing"));
				capabilitiesGet=iMMFDevSound->Config();
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
						StopTest(aError, EFail);
						}
					}
				else
					{
					ERR_PRINTF2(_L("Sample rate value failed with %S"), &stringSampleRateGet);
					ERR_PRINTF2(_L("Expected sample rate %S"), &stringSampleRateExpected);
					StopTest(aError, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundToneConfigWhilePausePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iTimeToEnterPauseElapsed)
		{
		iTimeToEnterPauseElapsed = ETrue;
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		iTimeToEnterPauseElapsed = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		}
	}

void RA3FDevSoundToneConfigWhilePausePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to be paused then stopped"));
	StopTest(aError, EFail);
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0034
 */
RA3FDevSoundToneMaxVolumeWhilePausePlayingTest::RA3FDevSoundToneMaxVolumeWhilePausePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iExVolume(0),
		iTimeToEnterPauseElapsed(EFalse)
	{
	}

RA3FDevSoundToneMaxVolumeWhilePausePlayingTest* RA3FDevSoundToneMaxVolumeWhilePausePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneMaxVolumeWhilePausePlayingTest * self = new(ELeave)RA3FDevSoundToneMaxVolumeWhilePausePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneMaxVolumeWhilePausePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneMaxVolumeWhilePausePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneMaxVolumeWhilePausePlayingTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::MaxVolume"));
				TInt maxVolumeGet = iMMFDevSound->MaxVolume();
				if (maxVolumeGet == iExVolume)
					{
					INFO_PRINTF2(_L("The max device supported volume is %d."), maxVolumeGet);
					StopTest();
					}
				else
					{
					INFO_PRINTF2(_L("The max device supported volume failed with %d."), maxVolumeGet);
					ERR_PRINTF2(_L("Expected value = %d."), iExVolume);
					StopTest(aError, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundToneMaxVolumeWhilePausePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iTimeToEnterPauseElapsed)
		{
		iTimeToEnterPauseElapsed = ETrue;
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		iTimeToEnterPauseElapsed = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		}
	}

void RA3FDevSoundToneMaxVolumeWhilePausePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to be paused then stopped"));
	StopTest(aError, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0035
 */
RA3FDevSoundToneVolumeWhilePausePlayingTest::RA3FDevSoundToneVolumeWhilePausePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iExVolume(0),
		iTimeToEnterPauseElapsed(EFalse)
	{
	}

RA3FDevSoundToneVolumeWhilePausePlayingTest* RA3FDevSoundToneVolumeWhilePausePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneVolumeWhilePausePlayingTest * self = new(ELeave)RA3FDevSoundToneVolumeWhilePausePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneVolumeWhilePausePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExVolume, iExVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExVolume);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneVolumeWhilePausePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneVolumeWhilePausePlayingTest"), EFsmIncorrectErrorPassed));
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
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Volume"));
				TInt maxVolumeGet = iMMFDevSound->Volume();
				if (maxVolumeGet == iExVolume)
					{
					INFO_PRINTF2(_L("The device default supported volume value is %d."), maxVolumeGet);
					StopTest();
					}
				else
					{
					INFO_PRINTF2(_L("The device default supported volume value failed with %d."), maxVolumeGet);
					ERR_PRINTF2(_L("Expected value = %d."), iExVolume);
					StopTest(aError, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundToneVolumeWhilePausePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iTimeToEnterPauseElapsed)
		{
		iTimeToEnterPauseElapsed = ETrue;
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		iTimeToEnterPauseElapsed = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		}
	}

void RA3FDevSoundToneVolumeWhilePausePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to be paused then stopped"));
	StopTest(aError, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0036
 */
RA3FDevSoundTonePlayToneOnceTest::RA3FDevSoundTonePlayToneOnceTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iFrequencyTone1(0)
	{
	}

RA3FDevSoundTonePlayToneOnceTest* RA3FDevSoundTonePlayToneOnceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneOnceTest * self = new(ELeave)RA3FDevSoundTonePlayToneOnceTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneOnceTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundTonePlayToneOnceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneOnceTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz  during %d secs "), iFrequencyTone1, iDuration);
				iStartTime.HomeTime();
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL failed with error : %d!"), err);
					StopTest(err);
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundTonePlayToneOnceTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iDuration * KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0037
 */
RA3FDevSoundTonePlayDualToneTest::RA3FDevSoundTonePlayDualToneTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iFrequencyTone1(0),
		iFrequencyTone2(0)
	{
	}
RA3FDevSoundTonePlayDualToneTest* RA3FDevSoundTonePlayDualToneTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDualToneTest * self = new(ELeave)RA3FDevSoundTonePlayDualToneTest(aTestName);
	return self;
	}
void RA3FDevSoundTonePlayDualToneTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundTonePlayDualToneTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDualToneTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				iStartTime.HomeTime();
				INFO_PRINTF4(_L("Calling CMMFDevSound::PlayDualToneL using %d Hz %d Hz during %d secs "), iFrequencyTone1, iFrequencyTone2, iDuration);
				TRAPD(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1, iFrequencyTone2, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with error : %d!"),  err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundTonePlayDualToneTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iDuration * KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0038
 */
RA3FDevSoundTonePlayDTMFToneTest::RA3FDevSoundTonePlayDTMFToneTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),iDTMFString(KNullDesC),
		iDTMFPauses(0), iDTMFTones(0)

	{
	}

RA3FDevSoundTonePlayDTMFToneTest* RA3FDevSoundTonePlayDTMFToneTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDTMFToneTest * self = new(ELeave)RA3FDevSoundTonePlayDTMFToneTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDTMFToneTest::DoKickoffTestL()
	{
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	for(TInt i =0 ; i< iDTMFString.Length() ; i++)
		{
		if (iDTMFString[i] == ',')
			{
			iDTMFPauses++;
			}
		else
			{
			iDTMFTones++;
			}
		}
	}

void RA3FDevSoundTonePlayDTMFToneTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDTMFToneTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError,  EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				iStartTime.HomeTime();
				INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundTonePlayDTMFToneTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt defaultToneOn = 250000;
		TInt defaultToneOff = 50000;
		TInt defaultPauseLength = 250000;
		TInt totalExpectedReproductionTime = defaultToneOn*iDTMFTones + defaultToneOff * iDTMFTones + iDTMFPauses * defaultPauseLength;
		if (iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL failed with %Ld"), iPlayToneTime.Int64());
			ERR_PRINTF2(_L("Expected time %d"), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0039
 */
RA3FDevSoundTonePlayToneSequenceTest::RA3FDevSoundTonePlayToneSequenceTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundTonePlayToneSequenceTest* RA3FDevSoundTonePlayToneSequenceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSequenceTest * self = new(ELeave)RA3FDevSoundTonePlayToneSequenceTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneSequenceTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundTonePlayToneSequenceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneSequenceTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling MMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,  EMMFStateTonePlaying));
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
				TUint8* tablePointer = const_cast<TUint8*>(&(KFixedSequenceTestSequenceDataX[0]));
				TPtrC8 KFixedSequenceData(tablePointer, sizeof(KFixedSequenceTestSequenceDataX));
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
				TRAPD(err, iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Play tone sequence failed with error = %d!"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0040
 */
RA3FDevSoundToneNewToneRequestWhilePlayingTest::RA3FDevSoundToneNewToneRequestWhilePlayingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iFrequencyTone1(0)
	{
	}

RA3FDevSoundToneNewToneRequestWhilePlayingTest* RA3FDevSoundToneNewToneRequestWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneNewToneRequestWhilePlayingTest * self = new(ELeave)RA3FDevSoundToneNewToneRequestWhilePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundToneNewToneRequestWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneNewToneRequestWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneNewToneRequestWhilePlayingTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				iStartTime.HomeTime();
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err == KErrNone)
					{
					INFO_PRINTF2(_L("CMMFDevSound::PlayToneL left with the expected error = %d"), err);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error value = %d!"), KErrNotReady);
					StopTest(err);
					break;
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

void RA3FDevSoundToneNewToneRequestWhilePlayingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

void RA3FDevSoundToneNewToneRequestWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		TInt totalExpectedReproductionTime = iDuration * KMicroSecsInOneSec;
		if(iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(KErrNone, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0041
 */
RA3FDevSoundTonePreemptionTest::RA3FDevSoundTonePreemptionTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iFrequencyTone1(0)
	{
	}

RA3FDevSoundTonePreemptionTest* RA3FDevSoundTonePreemptionTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePreemptionTest * self = new(ELeave)RA3FDevSoundTonePreemptionTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePreemptionTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundToneClient = CA3FDevSoundToneClient::NewL(*this);
	}

void RA3FDevSoundTonePreemptionTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePreemptionTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				TMMFPrioritySettings prioritySettings;
				prioritySettings.iPriority = KMinimumPriority;
				prioritySettings.iPref = EMdaPriorityPreferenceTime;
				prioritySettings.iState = EMMFStateIdle;
				iMMFDevSound->SetPrioritySettings(prioritySettings);
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Starting lower priority devsound client"));
				iDevsoundToneClient->SetPriority(KMaximumPriority);
				TInt err = iDevsoundToneClient->InitTonePlay(iFrequencyTone1, iDuration*KMicroSecsInOneSec);
				if(err != KErrNone)
					{
					ERR_PRINTF1(_L("Initialization to play tone thru second devsound client failed"));
					StopTest(err);
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

void RA3FDevSoundTonePreemptionTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

void RA3FDevSoundTonePreemptionTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("========== Preempting 1st DevSound Instance=========="));
		}
	else
		{
		ERR_PRINTF2(_L("First DevSound client called ToneFinished() and failed with error = %d "), aError);
		ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}


void RA3FDevSoundTonePreemptionTest::ClientInitializeCompleteCallback(TInt aError)
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
		TInt err = iDevsoundToneClient->PlayTone();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Starting to play tones thru second devsound client left with error = %d"), err);
			StopTest(err);
			}
		}
	}

void RA3FDevSoundTonePreemptionTest::ClientToneFinishedCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
		if (aError == KErrUnderflow)
			{
			INFO_PRINTF2(_L("Second DevSound instance called ToneFinished with error = %d"), aError);
			StopTest(aError,EPass);
			}
		else
			{
			ERR_PRINTF2(_L("Second DevSound instance called ToneFinished with error = %d that was NOT expected"), aError);
			ERR_PRINTF2(_L("Expected error = %d"), aError);
			StopTest(aError);
			}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0042
 */
RA3FDevSoundToneRejectionTest::RA3FDevSoundToneRejectionTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iFrequencyTone1(0)
	{
	}

RA3FDevSoundToneRejectionTest* RA3FDevSoundToneRejectionTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneRejectionTest * self = new(ELeave)RA3FDevSoundToneRejectionTest(aTestName);
	return self;
	}

void RA3FDevSoundToneRejectionTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iDevsoundToneClient = CA3FDevSoundToneClient::NewL(*this);
	}

void RA3FDevSoundToneRejectionTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundToneRejectionTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				TMMFPrioritySettings prioritySettings;
				prioritySettings.iPriority = KMinimumPriority;
				prioritySettings.iPref = EMdaPriorityPreferenceTime;
				prioritySettings.iState = EMMFStateIdle;
				iMMFDevSound->SetPrioritySettings(prioritySettings);
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Starting lower priority devsound client"));
				iDevsoundToneClient->SetPriority(KMinimumPriority);
				TInt err = iDevsoundToneClient->InitTonePlay(iFrequencyTone1, iDuration*KMicroSecsInOneSec);
				if(err != KErrNone)
					{
					ERR_PRINTF1(_L("Initialization to play tone thru second devsound client failed"));
					StopTest(err);
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

void RA3FDevSoundToneRejectionTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

void RA3FDevSoundToneRejectionTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		INFO_PRINTF2(_L("First DevSound instance called ToneFinished with error = %d"), aError);
		StopTest(aError,EPass);
		}
	else
		{
		ERR_PRINTF2(_L("First DevSound instance called ToneFinished with error = %d that was NOT expected"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), aError);
		StopTest(aError, EFail);
		}
	}


void RA3FDevSoundToneRejectionTest::ClientInitializeCompleteCallback(TInt aError)
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
		TInt err = iDevsoundToneClient->PlayTone();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Starting to play tones thru second devsound client left with error = %d"), err);
			StopTest(err);
			}
		}
	}

void RA3FDevSoundToneRejectionTest::ClientToneFinishedCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("========== Rejection 2nd DevSound Instance=========="));
		}
	else
		{
		ERR_PRINTF2(_L("Second DevSound client called ToneFinished() and failed with error = %d "), aError);
		ERR_PRINTF4(_L("Expected posibilities %d %d %d "), KErrInUse, KErrDied, KErrAccessDenied);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0043
 */
RA3FDevSoundTonePlayToneOnceAndStopTest::RA3FDevSoundTonePlayToneOnceAndStopTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iFrequencyTone1(0),
		iToneStopped(EFalse)
	{
	}

RA3FDevSoundTonePlayToneOnceAndStopTest* RA3FDevSoundTonePlayToneOnceAndStopTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneOnceAndStopTest * self = new(ELeave)RA3FDevSoundTonePlayToneOnceAndStopTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneOnceAndStopTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayToneOnceAndStopTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneOnceAndStopTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,  EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
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
				if (!iToneStopped)
					{
					iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
					INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
					TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error : %d!"), err);
						StopTest(err);
						break;
						}
					StartTimer(KMicroSecsTwoSec);
					INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
					iDevSoundState = EStatePlaying;
					}
				else
					{
					INFO_PRINTF1(_L("Calling CMMFDevSound::Stop succeeded"));
					StopTest(aError);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Stop"));
				iMMFDevSound->Stop();
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundTonePlayToneOnceAndStopTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	if(!iToneStopped)
		{
		Fsm(EEventTimerComplete, KErrNone);
		iToneStopped = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
		TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
			ERR_PRINTF2(_L("Expected error = %d"), KErrNone);
			StopTest(err);
			}
		INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
		iDevSoundState = EStateInitializing;
		}
	}

void RA3FDevSoundTonePlayToneOnceAndStopTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to have been stopped"));
	StopTest(aError, EFail);
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0044
 */
RA3FDevSoundTonePlayDualToneOnceAndStopTest::RA3FDevSoundTonePlayDualToneOnceAndStopTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iFrequencyTone1(0),
		iFrequencyTone2(0), iToneStopped(EFalse)
	{
	}

RA3FDevSoundTonePlayDualToneOnceAndStopTest* RA3FDevSoundTonePlayDualToneOnceAndStopTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDualToneOnceAndStopTest * self = new(ELeave)RA3FDevSoundTonePlayDualToneOnceAndStopTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDualToneOnceAndStopTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayDualToneOnceAndStopTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDualToneOnceAndStopTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
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
				if (!iToneStopped)
					{
					iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
					INFO_PRINTF4(_L("Calling CMMFDevSound::PlayDualToneL using %d Hz %d Hz during %d secs "), iFrequencyTone1, iFrequencyTone2, iDuration);
					TRAPD(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1, iFrequencyTone2, iDuration*KMicroSecsInOneSec));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with error = %d!"),  err);
						StopTest(err);
						break;
						}
					StartTimer(KMicroSecsTwoSec);
					INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
					iDevSoundState = EStatePlaying;
					}
				else
					{
					INFO_PRINTF1(_L("Calling CMMFDevSound::Stop succeeded"));
					StopTest(aError);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Stop"));
				iMMFDevSound->Stop();
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundTonePlayDualToneOnceAndStopTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	if(!iToneStopped)
		{
		Fsm(EEventTimerComplete, KErrNone);
		iToneStopped = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
		TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
			ERR_PRINTF2(_L("Expected error = %d"), KErrNone);
			StopTest(err);
			}
		INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
		iDevSoundState = EStateInitializing;
		}
	}

void RA3FDevSoundTonePlayDualToneOnceAndStopTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to have been stopped"));
	StopTest(aError, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0045
 */
RA3FDevSoundTonePlayDTMFStringOnceAndStopTest::RA3FDevSoundTonePlayDTMFStringOnceAndStopTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),iDTMFString(KNullDesC),
		iToneStopped(EFalse)
	{
	}

RA3FDevSoundTonePlayDTMFStringOnceAndStopTest* RA3FDevSoundTonePlayDTMFStringOnceAndStopTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDTMFStringOnceAndStopTest * self = new(ELeave)RA3FDevSoundTonePlayDTMFStringOnceAndStopTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDTMFStringOnceAndStopTest::DoKickoffTestL()
	{
	TPtrC DTMF;
	if ( !GetStringFromConfig(iTestStepName, KDTMFString, DTMF) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayDTMFStringOnceAndStopTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDTMFStringOnceAndStopTest"), EFsmIncorrectErrorPassed));
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
				if (!iToneStopped)
					{
					iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
					INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
					TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error = %d"), err);
						StopTest(err);
						break;
						}
					StartTimer(KMicroSecsTwoSec);
					INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
					iDevSoundState = EStatePlaying;
					}
				else
					{
					INFO_PRINTF1(_L("Calling CMMFDevSound::Stop succeeded"));
					StopTest(aError);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Stop"));
				iMMFDevSound->Stop();
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundTonePlayDTMFStringOnceAndStopTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	if(!iToneStopped)
		{
		Fsm(EEventTimerComplete, KErrNone);
		iToneStopped = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
		TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
			ERR_PRINTF2(_L("Expected error = %d"), KErrNone);
			StopTest(err);
			}
		INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
		iDevSoundState = EStateInitializing;
		}
	}

void RA3FDevSoundTonePlayDTMFStringOnceAndStopTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to have been stopped"));
	StopTest(aError, EFail);
	}
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0046
 */
RA3FDevSoundTonePlayToneSequenceOnceAndStopTest::RA3FDevSoundTonePlayToneSequenceOnceAndStopTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iToneStopped(EFalse)
	{
	}

RA3FDevSoundTonePlayToneSequenceOnceAndStopTest* RA3FDevSoundTonePlayToneSequenceOnceAndStopTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSequenceOnceAndStopTest * self = new(ELeave)RA3FDevSoundTonePlayToneSequenceOnceAndStopTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneSequenceOnceAndStopTest::DoKickoffTestL()
	{
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayToneSequenceOnceAndStopTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneSequenceOnceAndStopTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
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
				if (!iToneStopped)
					{
					iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
					TUint8* tablePointer = const_cast<TUint8*>( &(KFixedSequenceTestSequenceDataX[0]));
					TPtrC8 KFixedSequenceData(tablePointer,  sizeof(KFixedSequenceTestSequenceDataX));
					INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
					TRAPD(err, iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL left with error = %d!"), err);
						StopTest(err);
						break;
						}
					StartTimer(KMicroSecsTwoSec);
					INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
					iDevSoundState = EStatePlaying;
					}
				else
					{
					INFO_PRINTF1(_L("Calling CMMFDevSound::Stop succeeded"));
					StopTest(aError);
					}

				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Stop"));
				iMMFDevSound->Stop();
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"),  iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundTonePlayToneSequenceOnceAndStopTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	if(!iToneStopped)
		{
		Fsm(EEventTimerComplete, KErrNone);
		iToneStopped = ETrue;
		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
		TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
			ERR_PRINTF2(_L("Expected error = %d"), KErrNone);
			StopTest(err);
			}
		INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
		iDevSoundState = EStateInitializing;
		}
	}

void RA3FDevSoundTonePlayToneSequenceOnceAndStopTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to have been stopped"));
	StopTest(aError, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0047
 */
RA3FDevSoundTonePlayToneOncePauseTest::RA3FDevSoundTonePlayToneOncePauseTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),iDuration(0),
		iFrequencyTone1(0), iPaused(EFalse),
		iSamplesPlayedPaused(0)
	{
	}

RA3FDevSoundTonePlayToneOncePauseTest* RA3FDevSoundTonePlayToneOncePauseTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneOncePauseTest * self = new(ELeave)RA3FDevSoundTonePlayToneOncePauseTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneOncePauseTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayToneOncePauseTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneOncePauseTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
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
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error = %d!"),  err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"),  aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Calling CMMFDevSound::SamplesPlayed"));
				iSamplesPlayedPaused = iMMFDevSound->SamplesPlayed();
				INFO_PRINTF2(_L("The number of samples played is %d"), iSamplesPlayedPaused);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventTimerComplete)
				{
				TInt samplesPlayed = iMMFDevSound->SamplesPlayed();
				if(iSamplesPlayedPaused == samplesPlayed)
					{
					INFO_PRINTF2(_L("After waiting a couple of seconds, the samples played are still being the same %d"), iSamplesPlayedPaused);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("After waiting a couple of seconds, the samples played are not the same %d"), samplesPlayed);
					ERR_PRINTF2(_L("Expected samples played = %d"), iSamplesPlayedPaused);
					StopTest(aError,EFail);
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

void RA3FDevSoundTonePlayToneOncePauseTest::DoTimerCallback()
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

void RA3FDevSoundTonePlayToneOncePauseTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to be paused"));
	StopTest(aError, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0048
 */
RA3FDevSoundTonePlayDualToneOncePauseTest::RA3FDevSoundTonePlayDualToneOncePauseTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0),iFrequencyTone1(0),
		iFrequencyTone2(0), iPaused(EFalse),
		iSamplesPlayedPaused(0)
	{
	}

RA3FDevSoundTonePlayDualToneOncePauseTest* RA3FDevSoundTonePlayDualToneOncePauseTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDualToneOncePauseTest * self = new(ELeave)RA3FDevSoundTonePlayDualToneOncePauseTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDualToneOncePauseTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayDualToneOncePauseTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDualToneOncePauseTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,  EMMFStateTonePlaying));
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
				INFO_PRINTF4(_L("Calling CMMFDevSound::PlayDualToneL using %d Hz %d Hz during %d secs "), iFrequencyTone1, iFrequencyTone2, iDuration);
				TRAPD(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1, iFrequencyTone2, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with error : %d!"),  err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Calling CMMFDevSound::SamplesPlayed"));
				iSamplesPlayedPaused = iMMFDevSound->SamplesPlayed();
				INFO_PRINTF2(_L("The number of samples played is %d"), iSamplesPlayedPaused);
				StopTest();
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventTimerComplete)
				{
				TInt samplesPlayed = iMMFDevSound->SamplesPlayed();
				if(iSamplesPlayedPaused == samplesPlayed)
					{
					INFO_PRINTF2(_L("After waiting a couple of seconds, the samples played are still being the same %d"), iSamplesPlayedPaused);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("After waiting a couple of seconds, the samples played are not the same %d"), samplesPlayed);
					ERR_PRINTF2(_L("Expected samples played = %d"), iSamplesPlayedPaused);
					StopTest(aError,EFail);
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

void RA3FDevSoundTonePlayDualToneOncePauseTest::DoTimerCallback()
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

void RA3FDevSoundTonePlayDualToneOncePauseTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to be paused"));
	StopTest(aError, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0049
 */
RA3FDevSoundTonePlayDTMFStringOncePauseTest::RA3FDevSoundTonePlayDTMFStringOncePauseTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDTMFString(KNullDesC),iPaused(EFalse),
		iSamplesPlayedPaused(0),iDTMFPauses(0),
		iDTMFTones(0)
	{
	}

RA3FDevSoundTonePlayDTMFStringOncePauseTest* RA3FDevSoundTonePlayDTMFStringOncePauseTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDTMFStringOncePauseTest * self = new(ELeave)RA3FDevSoundTonePlayDTMFStringOncePauseTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDTMFStringOncePauseTest::DoKickoffTestL()
	{
	TPtrC DTMF;
	if ( !GetStringFromConfig(iTestStepName, KDTMFString, DTMF) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	for(TInt i =0 ; i< iDTMFString.Length() ; i++)
		{
		if (iDTMFString[i] == ',')
			{
			iDTMFPauses++;
			}
		else
			{
			iDTMFTones++;
			}
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayDTMFStringOncePauseTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDTMFStringOncePauseTest"), EFsmIncorrectErrorPassed));
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
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"),  &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error : %d!"),  err);
					StopTest(err);
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Calling CMMFDevSound::SamplesPlayed"));
				iSamplesPlayedPaused = iMMFDevSound->SamplesPlayed();
				INFO_PRINTF2(_L("The number of samples played is %d"), iSamplesPlayedPaused);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventTimerComplete)
				{
				TInt samplesPlayed = iMMFDevSound->SamplesPlayed();
				if(iSamplesPlayedPaused == samplesPlayed)
					{
					INFO_PRINTF2(_L("After waiting a couple of seconds, the samples played are still being the same %d"), iSamplesPlayedPaused);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("After waiting a couple of seconds, the samples played are not the same %d"), samplesPlayed);
					ERR_PRINTF2(_L("Expected samples played = %d"), iSamplesPlayedPaused);
					StopTest(aError,EFail);
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

void RA3FDevSoundTonePlayDTMFStringOncePauseTest::DoTimerCallback()
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

void RA3FDevSoundTonePlayDTMFStringOncePauseTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to be paused"));
	StopTest(aError, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0050
 */
RA3FDevSoundTonePlayToneSequenceOncePauseTest::RA3FDevSoundTonePlayToneSequenceOncePauseTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iPaused(EFalse),
		iSamplesPlayedPaused(0)
	{
	}

RA3FDevSoundTonePlayToneSequenceOncePauseTest* RA3FDevSoundTonePlayToneSequenceOncePauseTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSequenceOncePauseTest * self = new(ELeave)RA3FDevSoundTonePlayToneSequenceOncePauseTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneSequenceOncePauseTest::DoKickoffTestL()
	{
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayToneSequenceOncePauseTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneSequenceOncePauseTest"), EFsmIncorrectErrorPassed));
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
				TUint8* tablePointer = const_cast<TUint8*>( &(KFixedSequenceTestSequenceDataX[0] ) );
				TPtrC8 KFixedSequenceData(tablePointer, sizeof(KFixedSequenceTestSequenceDataX));
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
				TRAPD(err, iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL left  with error : %d!"),  err);
					StopTest(err);
					break;
					}
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			if(aDevSoundEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Calling CMMFDevSound::SamplesPlayed"));
				iSamplesPlayedPaused = iMMFDevSound->SamplesPlayed();
				INFO_PRINTF2(_L("The number of samples played is %d"), iSamplesPlayedPaused);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventTimerComplete)
				{
				TInt samplesPlayed = iMMFDevSound->SamplesPlayed();
				if(iSamplesPlayedPaused == samplesPlayed)
					{
					INFO_PRINTF2(_L("After waiting a couple of seconds, the samples played are still being the same %d"), iSamplesPlayedPaused);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("After waiting a couple of seconds, the samples played are not the same %d"), samplesPlayed);
					ERR_PRINTF2(_L("Expected samples played = %d"), iSamplesPlayedPaused);
					StopTest(aError,EFail);
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

void RA3FDevSoundTonePlayToneSequenceOncePauseTest::DoTimerCallback()
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

void RA3FDevSoundTonePlayToneSequenceOncePauseTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to be paused"));
	StopTest(aError, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0051
 */
RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest::RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0)
	{
	}

RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest* RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest * self = new(ELeave)RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL for playing audio files"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
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
				StopTest(aError,  EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs"), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err	== KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with KErrNotSupported %d which is the expected error"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0052
 */
RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest::RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iFrequencyTone2(0)
	{
	}

RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest* RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest * self = new(ELeave)RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL for playing audio files"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
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
				INFO_PRINTF4(_L("Calling CMMFDevSound::PlayDualToneL using %d Hz %d Hz during %d secs "), iFrequencyTone1, iFrequencyTone2, iDuration);
				TRAPD(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1, iFrequencyTone2, iDuration*KMicroSecsInOneSec));
				if (err	== KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with KErrNotSupported %d which is the expected error"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0053
 */
RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest::RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDTMFString(KNullDesC)
	{
	}

RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest* RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest * self = new(ELeave)RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest::DoKickoffTestL()
	{
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	}

void RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
	{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling iMMFDevSound::InitializeL for playing audio files"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
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
				INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S "), &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err	== KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with KErrNotSupported %d which is the expected error"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0054
 */
RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest::RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest* RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest * self = new(ELeave)RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest(aTestName);
	return self;
	}
void RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling iMMFDevSound::InitializeL for playing audio files"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,  EMMFStatePlaying));
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
				TUint8* tablePointer = const_cast<TUint8*>( &(KFixedSequenceTestSequenceDataX[0] ) );
				TPtrC8 KFixedSequenceData(tablePointer,  sizeof(KFixedSequenceTestSequenceDataX));
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
				TRAPD(err,  iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
				if (err == KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL left with KErrNotSupported %d which is the expected error"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("Play  dual tone failed with error = %d"),  err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0055
 */
RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest::RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0)
	{
	}

RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest* RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest * self = new(ELeave)RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL for recording audio files"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording));
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
				StopTest(aError,  EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs"), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err	== KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with KErrNotSupported %d which is the expected error"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0056
 */
RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest::RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iFrequencyTone2(0)
	{
	}

RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest* RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest * self = new(ELeave)RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL for recording audio files"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording));
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
				INFO_PRINTF4(_L("Calling CMMFDevSound::PlayDualToneL using %d Hz %d Hz during %d secs "), iFrequencyTone1, iFrequencyTone2, iDuration);
				TRAPD(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1, iFrequencyTone2, iDuration*KMicroSecsInOneSec));
				if (err	== KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with KErrNotSupported %d which is the expected error"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0057
 */
RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest::RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDTMFString(KNullDesC)
	{
	}

RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest* RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest * self = new(ELeave)RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest::DoKickoffTestL()
	{
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	}

void RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
	{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling iMMFDevSound::InitializeL for recording audio files"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording));
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
				INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S "), &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err	== KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with KErrNotSupported %d which is the expected error"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0058
 */
RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest::RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest* RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest * self = new(ELeave)RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest(aTestName);
	return self;
	}
void RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling iMMFDevSound::InitializeL for playing audio files"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,  EMMFStateRecording));
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
				TUint8* tablePointer = const_cast<TUint8*>( &(KFixedSequenceTestSequenceDataX[0] ) );
				TPtrC8 KFixedSequenceData(tablePointer,  sizeof(KFixedSequenceTestSequenceDataX));
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
				TRAPD(err,  iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
				if (err == KErrNotSupported)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL left with KErrNotSupported %d which is the expected error"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("Play  dual tone failed with error = %d"),  err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0059
 */
RA3FDevSoundTonePlayToneWithoutCallingInitializeTest::RA3FDevSoundTonePlayToneWithoutCallingInitializeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0)
	{
	}

RA3FDevSoundTonePlayToneWithoutCallingInitializeTest* RA3FDevSoundTonePlayToneWithoutCallingInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneWithoutCallingInitializeTest * self = new(ELeave)RA3FDevSoundTonePlayToneWithoutCallingInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneWithoutCallingInitializeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundTonePlayToneWithoutCallingInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneWithoutCallingInitializeTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs"), iFrequencyTone1, iDuration);
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
				if (err	== KErrNotReady)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with KErrNotReady %d which is the expected error"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"),  iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0060
 */
RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest::RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iFrequencyTone1(0), iFrequencyTone2(0)
	{
	}

RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest* RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest * self = new(ELeave)RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF4(_L("Calling CMMFDevSound::PlayDualToneL using %d Hz %d Hz during %d secs "), iFrequencyTone1, iFrequencyTone2, iDuration);
				TRAPD(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1, iFrequencyTone2, iDuration*KMicroSecsInOneSec));
				if (err == KErrNotReady)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with KErrNotReady %d which is the expected error"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0061
 */
RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest::RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDTMFString(KNullDesC)
	{
	}

RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest* RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest * self = new(ELeave)RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest::DoKickoffTestL()
	{
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	}

void RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err == KErrNotReady)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with KErrNotReady %d which is the expected error"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0062
 */
RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest::RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest* RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest * self = new(ELeave)RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				TUint8* tablePointer = const_cast<TUint8*>( &(KFixedSequenceTestSequenceDataX[0] ) );
				TPtrC8 KFixedSequenceData(tablePointer,  sizeof(KFixedSequenceTestSequenceDataX));
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
				TRAPD(err, iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
				if (err == KErrNotReady)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL left with KErrNotReady %d which is the expected error"), err);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0063
 */
RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest::RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iInvalidToneSequence(KNullDesC8)
	{
	}

RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest* RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest * self = new(ELeave)RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest::DoKickoffTestL()
	{
	TPtrC TS;
	if ( !GetStringFromConfig(iTestStepName, KInvalidToneSequence,  TS) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KInvalidToneSequence);
		StopTest(KErrNotFound);
		return;
		}
	iInvalidToneSequence.Copy(TS);
	}

void RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest"), EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling iMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"),  err);
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
				INFO_PRINTF1(_L("Calling iMMFDevSound->PlayToneSequenceL with an invalid descriptor as argument"));
				TRAPD(err, iMMFDevSound->PlayToneSequenceL(iInvalidToneSequence));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL left with error = %d and it should not leave"), err);
					StopTest(err, EFail);
					}
				}
			else if (aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
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

void RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrCorrupt)
		{
		INFO_PRINTF2(_L("ToneFinished called with KErrCorrupt %d which is the expected error"), aError);
		StopTest();
		}
	else
		{
		ERR_PRINTF3(_L("ToneFinished called with error = %d, when the expected error is = %d"), aError, KErrCorrupt);
		StopTest(aError, EFail);
		}
	}

