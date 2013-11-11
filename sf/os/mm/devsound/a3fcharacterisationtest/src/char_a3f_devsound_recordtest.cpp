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

#include "char_a3f_devsound_recordtest.h"




/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0001
 */
RA3FDevSoundRecordConfigurationSetConfigTest::RA3FDevSoundRecordConfigurationSetConfigTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
	iSampleRate(0),
	iChannels(0)
	{
	}

RA3FDevSoundRecordConfigurationSetConfigTest* RA3FDevSoundRecordConfigurationSetConfigTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordConfigurationSetConfigTest* self = new (ELeave) RA3FDevSoundRecordConfigurationSetConfigTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordConfigurationSetConfigTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KSampleRate, iSampleRate) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KChannel, iChannels) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordConfigurationSetConfigTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundRecordConfigurationSetConfigTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
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
				TMMFSampleRate esampleRate;
				TBuf<KMaxSampleRateStringLength> stringSampleRate;
				TBuf<KMaxChannelsStringLength> stringChannels;
				SampleRateFromTIntToTMMFSampleRate(iSampleRate, esampleRate);
				TMMFCapabilities capabilities = iMMFDevSound->Config();
				capabilities.iRate = esampleRate;
				capabilities.iChannels = iChannels;
				SampleRateFromTUintToString(capabilities.iRate,stringSampleRate);
				ChannelsFromTUintToString(capabilities.iChannels,stringChannels);
				INFO_PRINTF3(_L("Settings for Sample rate and channel mode. %S %S"),&stringSampleRate,&stringChannels);
				INFO_PRINTF3(_L("Configuring DevSound object with the settings for sampling rate to %d HZ and channels to %d."),iSampleRate,iChannels);
				TRAPD(err, iMMFDevSound->SetConfigL(capabilities));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Configuration of DevSound object failed! Left with error = %d"), err);
					StopTest(err);
					break;
					}

				INFO_PRINTF1(_L("Getting DevSound capabilities for verifying."));
				TMMFCapabilities getCapabilities = iMMFDevSound->Config();
				if ((capabilities.iRate != getCapabilities.iRate)||(capabilities.iChannels != getCapabilities.iChannels))
					{
					ERR_PRINTF3(_L("Configuration of DevSound object does not match with set capabilities! Rate = %u Channels = %u "),
							getCapabilities.iRate, getCapabilities.iChannels);
					StopTest(err, EFail);
					}
				else
					{
					INFO_PRINTF3(_L("DevSound configuration is a match rate = %u channels = %u"), getCapabilities.iRate, getCapabilities.iChannels);
					StopTest();
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0002
 */
RA3FDevSoundRecordConfigurationSetPriorityTest::RA3FDevSoundRecordConfigurationSetPriorityTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iPriority(EMdaPriorityNormal)
	{
	}

RA3FDevSoundRecordConfigurationSetPriorityTest* RA3FDevSoundRecordConfigurationSetPriorityTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordConfigurationSetPriorityTest* self = new (ELeave) RA3FDevSoundRecordConfigurationSetPriorityTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordConfigurationSetPriorityTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KPriority, iPriority) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KPriority);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordConfigurationSetPriorityTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundRecordConfigurationSetPriorityTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
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
				INFO_PRINTF2(_L("Configuring DevSound object with the settings for Priority to %d."), iPriority);
				TMMFPrioritySettings prioritySettings;
				prioritySettings.iPriority = iPriority;
				iMMFDevSound->SetPrioritySettings(prioritySettings);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0003
 */
RA3FDevSoundRecordConfigurationSetGainTest::RA3FDevSoundRecordConfigurationSetGainTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iGain(0)
	{
	}

RA3FDevSoundRecordConfigurationSetGainTest* RA3FDevSoundRecordConfigurationSetGainTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordConfigurationSetGainTest* self = new (ELeave) RA3FDevSoundRecordConfigurationSetGainTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordConfigurationSetGainTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KGain, iGain) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KGain);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordConfigurationSetGainTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundRecordConfigurationSetGainTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
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
				INFO_PRINTF2( _L("Setting the input gain to %d using iMMFDevSound->SetGain"),iGain);
				iMMFDevSound->SetGain(iGain);

				INFO_PRINTF1(_L("Call Gain() for verifying."));
				TInt getGain = iMMFDevSound->Gain();

				if (iGain != getGain)
					{
					ERR_PRINTF2(_L("Configuration of DevSound object does not match with set gain! Gain = %d"),
							getGain);
					ERR_PRINTF2(_L("Expected Gain = %d"),
							iGain);
					StopTest(aError, EFail);
					}
				else
					{
					INFO_PRINTF2(_L("Gain has the expected value = %d"), getGain);
					StopTest();
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0004
 */
RA3FDevSoundRecordConfigurationSetRecordBalanceTest::RA3FDevSoundRecordConfigurationSetRecordBalanceTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iLRecordBalance(0), iRRecordBalance(0)
	{
	}

RA3FDevSoundRecordConfigurationSetRecordBalanceTest* RA3FDevSoundRecordConfigurationSetRecordBalanceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordConfigurationSetRecordBalanceTest* self = new (ELeave) RA3FDevSoundRecordConfigurationSetRecordBalanceTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordConfigurationSetRecordBalanceTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KLRecordBalance, iLRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KLRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KRRecordBalance, iRRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordConfigurationSetRecordBalanceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundRecordConfigurationSetRecordBalanceTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
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
				INFO_PRINTF3(_L("Calling iMMFDevSound->SetRecordBalanceL using the corresponding values %d and %d"),iLRecordBalance,iRRecordBalance);
				TRAPD(err,iMMFDevSound->SetRecordBalanceL(iLRecordBalance, iRRecordBalance));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
					StopTest(err);
					break;
					}

				INFO_PRINTF1(_L("Call GetRecordBalanceL for verifying."));
				TInt getLRecordBalance;
				TInt getRRecordBalance;
				TRAP(err,iMMFDevSound->GetRecordBalanceL(getLRecordBalance, getRRecordBalance));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
					StopTest(err);
					break;
					}
				if ((iLRecordBalance != getLRecordBalance)||(iRRecordBalance != getRRecordBalance))
					{
					ERR_PRINTF3(_L("Configuration of DevSound object does not match with set balance! LRecordBalance = %d RRecordBalance = %d"),
							getLRecordBalance, getRRecordBalance);
					StopTest(aError, EFail);
					}
				else
					{
					INFO_PRINTF3(_L("Balance configuration matches left balance = %d right balance = %d"),
							getLRecordBalance, getRRecordBalance);
					StopTest();
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0005
 */
RA3FDevSoundRecordSetGainWhileRecordingTest::RA3FDevSoundRecordSetGainWhileRecordingTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iCalledSetGain(EFalse)
	{
	}

RA3FDevSoundRecordSetGainWhileRecordingTest* RA3FDevSoundRecordSetGainWhileRecordingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordSetGainWhileRecordingTest* self = new (ELeave) RA3FDevSoundRecordSetGainWhileRecordingTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordSetGainWhileRecordingTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundRecordSetGainWhileRecordingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordSetGainWhileRecordingTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));

				INFO_PRINTF2(_L("Calling SetGain to initial value of %d"), iMMFDevSound->MaxGain()/2);
				iMMFDevSound->SetGain(iMMFDevSound->MaxGain()/2);

				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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

void RA3FDevSoundRecordSetGainWhileRecordingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(iCalledSetGain == EFalse)
		{
		INFO_PRINTF2(_L("Calling SetGain to value of %d"), iMMFDevSound->MaxGain());
		iMMFDevSound->SetGain(iMMFDevSound->MaxGain());

		INFO_PRINTF2(_L("Calling Gain to verify gain value"), iMMFDevSound->MaxGain());
		TInt gain = iMMFDevSound->Gain();
		if(gain != iMMFDevSound->MaxGain())
			{
			ERR_PRINTF2(_L("Gain value does not match, gain = %d"),gain);
			StopTest(KErrNone, EFail);
			}
		else
			{
			INFO_PRINTF2(_L("Gain values match, gain = %d"),gain);
			}

		iCalledSetGain = ETrue;
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0006
 */
RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest::RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName), iGain(0)
	{
	}

RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest* RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest* self = new (ELeave) RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest(aTestName);
	return self;
	}

void RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KGain, iGain) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KGain);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
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
				TInt maxGain = iMMFDevSound->MaxGain();
				INFO_PRINTF2(_L("MaxGain returns a value of %d"),maxGain);
				INFO_PRINTF2(_L("Call iMMFDevSound->SetGain using the gain value %d"),iGain + maxGain);
				iMMFDevSound->SetGain(iGain+iMMFDevSound->MaxGain());

				INFO_PRINTF1(_L("Call Gain() for verifying."));
				if(iMMFDevSound->Gain() == iMMFDevSound->MaxGain())
					{
					INFO_PRINTF2(_L("Gain is set to the expected value %d "),iMMFDevSound->Gain());
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Gain is NOT set to expected value %d"),iMMFDevSound->Gain());
					StopTest(KErrNone, EFail);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0007
 */
RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest::RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName), iGain(0), iExGain(0)
	{
	}

RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest* RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest* self = new (ELeave) RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest(aTestName);
	return self;
	}

void RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KGain, iGain))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KGain);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExGain, iExGain))
			{
			ERR_PRINTF2(KMsgErrorGetParameter, &KExGain);
			StopTest(KErrNotFound, ETestSuiteError);
			return;
			}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
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
				INFO_PRINTF2(_L("Call iMMFDevSound->SetGain using the negative gain value %d"),iGain);
				iMMFDevSound->SetGain(iGain);
				INFO_PRINTF1(_L("Call Gain() for verifying."));
				TInt gainGet = iMMFDevSound->Gain();
				if(gainGet == iExGain)
					{
					INFO_PRINTF2(_L("Gain is set to the expected value which is %d "), gainGet);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Gain is not set to the expected value, gain = %d"), gainGet);
					ERR_PRINTF2(_L("Expected value = %d"), iExGain);
					StopTest(KErrNone, EFail);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0008
 */
RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest::RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName), iLRecordBalance(0),
		iRRecordBalance(0)
	{
	}

RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest* RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest* self = new (ELeave) RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest(aTestName);
	return self;
	}

void RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KLRecordBalance, iLRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KLRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KRRecordBalance, iRRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
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
				INFO_PRINTF3(_L("Calling SetRecordBalanceL using values beyond upper boundaries: left = %d right = %d"), iLRecordBalance, iRRecordBalance);
				TRAPD(err,iMMFDevSound->SetRecordBalanceL(iLRecordBalance, iRRecordBalance));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Setting balance failed ! Leave with error = %d"), err);
					StopTest(err);
					break;
					}

				INFO_PRINTF1(_L("Call GetRecordBalanceL() for verifying."));
				TRAP(err,iMMFDevSound->GetRecordBalanceL(iLRecordBalance, iRRecordBalance));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
					StopTest(err);
					break;
					}
				if(iLRecordBalance == KMaxRecordBalanceValue &&  iRRecordBalance == KMaxRecordBalanceValue)
					{
					INFO_PRINTF3(_L("Record balance is set to expected values %d and %d "),iLRecordBalance,iRRecordBalance);
					StopTest();
					}
				else
					{
					ERR_PRINTF3(_L("Record balance is NOT set to expected values"),iLRecordBalance,iRRecordBalance);
					StopTest(KErrNone, EFail);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0009
 */
RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest::RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName), iLRecordBalance(0),
		iRRecordBalance(0), iExLSpeakerBalance(0),
		iExRSpeakerBalance(0)
	{
	}

RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest* RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest* self = new (ELeave) RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest(aTestName);
	return self;
	}

void RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KLRecordBalance, iLRecordBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KLRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KRRecordBalance, iRRecordBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExLSpeakerBalance, iExLSpeakerBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExLSpeakerBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KExRSpeakerBalance, iExRSpeakerBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExRSpeakerBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
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
				INFO_PRINTF3(_L("Calling CMMFDevSound::SetRecordBalanceL using negative values %d and %d"),iLRecordBalance, iRRecordBalance);
				TRAPD(err,iMMFDevSound->SetRecordBalanceL(iLRecordBalance, iRRecordBalance));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
					StopTest(err);
					break;
					}
				TInt lRecordBalanceGet, rRecordBalanceGet;
				INFO_PRINTF1(_L("Call CMMFDevSound::GetRecordBalanceL for verifying."));
				TRAP(err,iMMFDevSound->GetRecordBalanceL(lRecordBalanceGet, rRecordBalanceGet));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Getting balance failed ! Leave error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					if(lRecordBalanceGet == iExLSpeakerBalance &&  rRecordBalanceGet == iExRSpeakerBalance)
						{
						INFO_PRINTF3(_L("Values for left = %d and right = %d record balance values are the expected"), lRecordBalanceGet, rRecordBalanceGet);
						StopTest();
						}
					else
						{
						ERR_PRINTF3(_L("Values for left = %d and right = %d record balance values are not the expected"), lRecordBalanceGet, rRecordBalanceGet);
						ERR_PRINTF3(_L("Expected values for left = %d and right = %d balance"), iExLSpeakerBalance, iExRSpeakerBalance);
						StopTest(KErrNone,EFail);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0010
 */
RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest::RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iGain(0), iInitializedToRecord(EFalse)
	{
	}

RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest* RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest* self = new (ELeave) RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KGain, iGain) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KGain);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Play Audio"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStatePlaying));
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
				if (iInitializedToRecord == EFalse)
					{
					INFO_PRINTF1(_L("DevSound Initialized to play audio."));
					INFO_PRINTF2( _L("Setting the input gain to %d using iMMFDevSound->SetGain"),iGain);
					iMMFDevSound->SetGain(iGain);

					INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
					TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Initialization of DevSound failed with error : %d!"), err);
						StopTest(err);
						break;
						}
					iInitializedToRecord = ETrue;
					}
				else
					{
					INFO_PRINTF1(_L("DevSound Initialized to record audio."));
					INFO_PRINTF1(_L("Call Gain() for verifying."));
					TInt getGain = iMMFDevSound->Gain();

					if (iGain != getGain)
						{
						ERR_PRINTF2(_L("Configuration of DevSound object does not match with set gain! Gain = %d"),
								getGain);
						ERR_PRINTF2(_L("Expected Gain = %d"), iGain);
						StopTest(aError, EFail);
						}
					else
						{
						INFO_PRINTF2(_L("Gain value is as expected = %d"),
								getGain);
						StopTest();
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0011
 */
RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest::RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iGain(0), iInitializedToRecord(EFalse)
	{
	}

RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest* RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest* self = new (ELeave) RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KGain, iGain) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KGain);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Play Tones"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,EMMFStateTonePlaying));
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
				if (iInitializedToRecord == EFalse)
					{
					INFO_PRINTF1(_L("DevSound Initialized to play tones."));
					INFO_PRINTF2( _L("Setting the input gain to %d using iMMFDevSound->SetGain"),iGain);
					iMMFDevSound->SetGain(iGain);

					INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
					TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Initialization of DevSound failed with error : %d!"), err);
						StopTest(err);
						break;
						}
					iInitializedToRecord = ETrue;
					}
				else
					{
					INFO_PRINTF1(_L("DevSound Initialized to record audio."));
					INFO_PRINTF1(_L("Call Gain() for verifying."));
					TInt getGain = iMMFDevSound->Gain();

					if (iGain != getGain)
						{
						ERR_PRINTF2(_L("Configuration of DevSound object does not match with set gain! Gain = %d"),
								getGain);
						ERR_PRINTF2(_L("Expected Gain = %d"), iGain);
						StopTest(aError, EFail);
						}
					else
						{
						INFO_PRINTF2(_L("Gain value is as expected = %d"),
								getGain);
						StopTest();
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0012
 */
RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest::RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iLRecordBalance(0),
		iRRecordBalance(0),
		iInitializedToRecord(EFalse)
	{
	}

RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest* RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest* self = new (ELeave) RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KLRecordBalance, iLRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KLRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KRRecordBalance, iRRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Play Audio"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStatePlaying));
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
				if (iInitializedToRecord == EFalse)
					{
					INFO_PRINTF1(_L("DevSound Initialized to play audio."));

					INFO_PRINTF3(_L("Calling iMMFDevSound->SetRecordBalanceL using the corresponding values %d and %d"),iLRecordBalance,iRRecordBalance);
					TRAPD(err,iMMFDevSound->SetRecordBalanceL(iLRecordBalance, iRRecordBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}

					INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
					TRAP(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Initialization of DevSound failed with error : %d!"), err);
						StopTest(err);
						break;
						}
					iInitializedToRecord = ETrue;
					}
				else
					{
					INFO_PRINTF1(_L("DevSound Initialized to record audio."));

					INFO_PRINTF1(_L("Call GetRecordBalanceL for verifying."));
					TInt getLRecordBalance;
					TInt getRRecordBalance;
					TRAPD(err,iMMFDevSound->GetRecordBalanceL(getLRecordBalance, getRRecordBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					if ((iLRecordBalance != getLRecordBalance)||(iRRecordBalance != getRRecordBalance))
						{
						ERR_PRINTF3(_L("Configuration of DevSound object does not match with set balance! LRecordBalance = %d RRecordBalance = %d"),
								getLRecordBalance, getRRecordBalance);
						StopTest(aError, EFail);
						}
					else
						{
						INFO_PRINTF3(_L("Balance values are as expected, Left = %d Right = %d"),
								getLRecordBalance, getRRecordBalance);
						StopTest();
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0013
 */
RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest::RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iLRecordBalance(0),
		iRRecordBalance(0),
		iInitializedToRecord(EFalse)
	{
	}

RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest* RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest* self = new (ELeave) RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KLRecordBalance, iLRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KLRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KRRecordBalance, iRRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KRRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}

	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Play Tones"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,EMMFStateTonePlaying));
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
				if (iInitializedToRecord == EFalse)
					{
					INFO_PRINTF1(_L("DevSound Initialized to play tones."));

					INFO_PRINTF3(_L("Calling iMMFDevSound->SetRecordBalanceL using the corresponding values %d and %d"),iLRecordBalance,iRRecordBalance);
					TRAPD(err,iMMFDevSound->SetRecordBalanceL(iLRecordBalance, iRRecordBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}

					INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
					TRAP(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Initialization of DevSound failed with error : %d!"), err);
						StopTest(err);
						break;
						}
					iInitializedToRecord = ETrue;
					}
				else
					{
					INFO_PRINTF1(_L("DevSound Initialized to record audio."));

					INFO_PRINTF1(_L("Call GetRecordBalanceL for verifying."));
					TInt getLRecordBalance;
					TInt getRRecordBalance;
					TRAPD(err,iMMFDevSound->GetRecordBalanceL(getLRecordBalance, getRRecordBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					if ((iLRecordBalance != getLRecordBalance)||(iRRecordBalance != getRRecordBalance))
						{
						ERR_PRINTF3(_L("Configuration of DevSound object does not match with set balance! LRecordBalance = %d RRecordBalance = %d"),
								getLRecordBalance, getRRecordBalance);
						StopTest(aError, EFail);
						}
					else
						{
						INFO_PRINTF3(_L("Balance values are as expected, Left = %d Right = %d"),
								getLRecordBalance, getRRecordBalance);
						StopTest();
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0014
 */
RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest::RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0),
		iSampleRate(0),
		iChannels(0),
		iCallConfig(EFalse)
	{
	}

RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest* RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest* self = new (ELeave) RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest(aTestName);
	return self;
	}

void RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KSampleRate, iSampleRate) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KChannel, iChannels) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));

				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Write(buffer->Data());
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("iFile.Write() returned the error %d"), err);
					StopTest(err);
					break;
					}
				// Continue recording data
				iAsyncWriteBTFAO->Start(buffer);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iCallConfig)
		{
		TMMFCapabilities capabilities = iMMFDevSound->Config();
		TMMFSampleRate esampleRate;
		TBuf<KMaxSampleRateStringLength> stringSampleRate;
		TBuf<KMaxChannelsStringLength> stringChannels;
		SampleRateFromTIntToTMMFSampleRate(iSampleRate,esampleRate);
		capabilities.iRate = esampleRate;
		capabilities.iChannels = iChannels;
		SampleRateFromTUintToString(capabilities.iRate,stringSampleRate);
		ChannelsFromTUintToString(capabilities.iChannels,stringChannels);
		INFO_PRINTF3(_L("Settings for Sample rate and channel mode. %S %S"),&stringSampleRate,&stringChannels);
		INFO_PRINTF3(_L("Configuring DevSound object with the settings for sampling rate to %d HZ and channels to %d."), iSampleRate, iChannels);
		TRAPD(err, iMMFDevSound->SetConfigL(capabilities));
		if (err == KErrNotReady)
			{
			INFO_PRINTF2(_L("Configuration of DevSound object left with expected error = %d"), err);
			StopTest();
			return;
			}
		else
			{
			ERR_PRINTF1(_L("Configuration of DevSound object did not fail!"));
			StopTest(err, EFail);
			}
		iTimer->Cancel();
		iFile.Close();
		iCallConfig = ETrue;
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0015
 */
RA3FDevSoundQueryRecordCapabilitiesTest::RA3FDevSoundQueryRecordCapabilitiesTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundQueryRecordCapabilitiesTest* RA3FDevSoundQueryRecordCapabilitiesTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordCapabilitiesTest* self = new (ELeave) RA3FDevSoundQueryRecordCapabilitiesTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordCapabilitiesTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundQueryRecordCapabilitiesTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordCapabilitiesTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));

				INFO_PRINTF1(_L("Calling query method Capabilities()"));
				TMMFCapabilities capabilities = iMMFDevSound->Capabilities();
				PrintSupportedCapabilities(capabilities.iRate, capabilities.iChannels);

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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0016
 */
RA3FDevSoundQueryRecordConfigTest::RA3FDevSoundQueryRecordConfigTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName), iExCapRate(0),
		iExCapChannels(0)
	{
	}

RA3FDevSoundQueryRecordConfigTest* RA3FDevSoundQueryRecordConfigTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordConfigTest* self = new (ELeave) RA3FDevSoundQueryRecordConfigTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordConfigTest::DoKickoffTestL()
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
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundQueryRecordConfigTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordConfigTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				TMMFCapabilities capabilitiesGet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateGet;
				TBuf<KMaxChannelsStringLength> stringChannelsGet;
				TBuf<KMaxSampleRateStringLength> stringSampleRateExpected;
				TBuf<KMaxChannelsStringLength> stringChannelsExpected;
				INFO_PRINTF1(_L("Calling CMMFDevSound::Config"));
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0017
 */
RA3FDevSoundQueryRecordBalanceTest::RA3FDevSoundQueryRecordBalanceTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iExLRecordBalance(0),
		iExRRecordBalance(0)
	{
	}

RA3FDevSoundQueryRecordBalanceTest* RA3FDevSoundQueryRecordBalanceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordBalanceTest* self = new (ELeave) RA3FDevSoundQueryRecordBalanceTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordBalanceTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KExLRecordBalance, iExLRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExLRecordBalance);
		StopTest(KErrNotFound);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KExRRecordBalance, iExRRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExRRecordBalance);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundQueryRecordBalanceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordBalanceTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				TInt getLRecordBalance = 0;
				TInt getRRecordBalance = 0;

				INFO_PRINTF1(_L("Calling query method GetRecordBalanceL()"));
				TRAPD(err,iMMFDevSound->GetRecordBalanceL(getLRecordBalance, getRRecordBalance));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF3(_L("Record balance is set to %d and %d "),getLRecordBalance,getRRecordBalance);
				if(getLRecordBalance == iExLRecordBalance && getRRecordBalance == iExRRecordBalance)
					{
					INFO_PRINTF1(_L("Record balance values match default values"));
					StopTest();
					}
				else
					{
					ERR_PRINTF1(_L("Record balance values do not match default values"));
					ERR_PRINTF3(_L("Expected left = %d, right = %d"),iExLRecordBalance,iExRRecordBalance);
					StopTest(KErrNone, EFail);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0018
 */
RA3FDevSoundQueryRecordSupportedOutputDataTypesTest::RA3FDevSoundQueryRecordSupportedOutputDataTypesTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iCurrentTypeInTest(0)
	{
	}

RA3FDevSoundQueryRecordSupportedOutputDataTypesTest* RA3FDevSoundQueryRecordSupportedOutputDataTypesTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordSupportedOutputDataTypesTest* self = new (ELeave) RA3FDevSoundQueryRecordSupportedOutputDataTypesTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordSupportedOutputDataTypesTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundQueryRecordSupportedOutputDataTypesTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordSupportedOutputDataTypesTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));

				INFO_PRINTF1(_L("Calling query method GetSupportedOutputDataTypesL()"));

				TMMFPrioritySettings prioritySettings;
				TRAPD(err,iMMFDevSound->GetSupportedOutputDataTypesL(iSupportedFourCCs,prioritySettings));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Getting supported output data types failed ! Left with error = %d"), err);
					StopTest(err);
					break;
					}
				if(iCurrentTypeInTest == 0)
					{
					for (TInt x = 0; x < iSupportedFourCCs.Count(); x++)
						{
						TFourCC fourCC = iSupportedFourCCs[x];
						TBuf<4> name;
						for(TInt i=0; i<= KFourCCMaxBitDisplacement; i+=K8Bits)
							{
							name.Append( (TUint8)(fourCC.FourCC() >> i) );
							}
						INFO_PRINTF3(_L("Supported Input Data types: 0x%x  %S "), fourCC.FourCC(), &name);
						}
					}
				// Complete test when all types have been used of initialization
				if(iCurrentTypeInTest == iSupportedFourCCs.Count())
					{
					INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
					StopTest();
					}
				else
					{
					TFourCC fourCC = iSupportedFourCCs[iCurrentTypeInTest];
					TBuf<4> name;
					for (TInt i=0; i<= KFourCCMaxBitDisplacement; i+=K8Bits)
						{
						name.Append ( (TUint8)(fourCC.FourCC() >> i));
						}
					INFO_PRINTF3(_L("Initializing for Output Data type: 0x%x  %S "), fourCC.FourCC(), &name);
					TRAPD(err, iMMFDevSound->InitializeL(*this, fourCC, EMMFStateRecording));
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0019
 */
RA3FDevSoundQueryRecordMaxGainTest::RA3FDevSoundQueryRecordMaxGainTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iExGain(0)
	{
	}

RA3FDevSoundQueryRecordMaxGainTest* RA3FDevSoundQueryRecordMaxGainTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordMaxGainTest* self = new (ELeave) RA3FDevSoundQueryRecordMaxGainTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordMaxGainTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KExGain, iExGain) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExGain);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundQueryRecordMaxGainTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordMaxGainTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));

				INFO_PRINTF1(_L("Calling query method MaxGain()"));
				TInt maxGain = iMMFDevSound->MaxGain();
				INFO_PRINTF2(_L("Record Max Gain  %d"), maxGain);
				if(maxGain == iExGain)
					{
					INFO_PRINTF1(_L("Max Gain matches the expected value"));
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Max Gain did not match the expected gain value = %d"), iExGain);
					StopTest(KErrNone, EFail);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0020
 */
RA3FDevSoundQueryRecordSamplesRecordedTest::RA3FDevSoundQueryRecordSamplesRecordedTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName),
		iExSamplesRecorded(0)
	{
	}

RA3FDevSoundQueryRecordSamplesRecordedTest* RA3FDevSoundQueryRecordSamplesRecordedTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordSamplesRecordedTest* self = new (ELeave) RA3FDevSoundQueryRecordSamplesRecordedTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordSamplesRecordedTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KExSamplesRecorded, iExSamplesRecorded) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExSamplesRecorded);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundQueryRecordSamplesRecordedTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordSamplesRecordedTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));

				INFO_PRINTF1(_L("Calling query method SamplesRecorded()"));
				TInt samplesRecorded = iMMFDevSound->SamplesRecorded();
				INFO_PRINTF2(_L("Samples Recorded  %d"), samplesRecorded);

				if(samplesRecorded == iExSamplesRecorded)
					{
					INFO_PRINTF1(_L("Samples Recorded match the expected value"));
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Samples Recorded do not match the expected value of %d"), iExSamplesRecorded);
					StopTest(KErrNone, EFail);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0021
 */
RA3FDevSoundQueryRecordGainTest::RA3FDevSoundQueryRecordGainTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundQueryRecordGainTest* RA3FDevSoundQueryRecordGainTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordGainTest* self = new (ELeave) RA3FDevSoundQueryRecordGainTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordGainTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundQueryRecordGainTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordGainTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));

				INFO_PRINTF1(_L("Calling query method Gain()"));
				INFO_PRINTF2(_L("Current Gain = %d"),iMMFDevSound->Gain());
				if(iMMFDevSound->Gain() == (iMMFDevSound->MaxGain()+1)/2)
					{
					INFO_PRINTF2(_L("Gain equals the expected default value of MaxGain/2 = %d"),iMMFDevSound->MaxGain()/2);
					StopTest();
					}
				else
					{
					ERR_PRINTF2(_L("Gain is different than the expected default value of MaxGain/2 = %d"),iMMFDevSound->MaxGain()/2);
					StopTest(KErrNone, EFail);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0022
 */
RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest::RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0),
		iQueryRecordingSettings(EFalse),
		iExCapChannels(0),
		iExCapRate(0)
	{
	}

RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest* RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest* self = new (ELeave) RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KExCapChannels, iExCapChannels) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExCapChannels);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KExCapRate, iExCapRate) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExCapRate);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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

void RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iQueryRecordingSettings)
		{
		INFO_PRINTF1(_L("Calling query method Capabilities() while recording"));
		TMMFCapabilities capabilities = iMMFDevSound->Capabilities();

		INFO_PRINTF3(_L("Rate Sum:0x%x Channels Sum:0x%x"), capabilities.iRate, capabilities.iChannels);
		PrintSupportedCapabilities(capabilities.iRate,capabilities.iChannels);
		if(capabilities.iRate == iExCapRate && capabilities.iChannels == iExCapChannels)
			{
			INFO_PRINTF1(_L("Capabilities returned match expected capabilities"));
			}
		else
			{
			ERR_PRINTF1(_L("Capabilities returned do not match the expected capabilities"));
			ERR_PRINTF3(_L("Expected Rate Sum:0x%x Channels Sum:0x%x"), iExCapRate, iExCapChannels);
			StopTest (KErrNone, EFail);
			}
		iQueryRecordingSettings = ETrue;
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0023
 */
RA3FDevSoundQueryRecordConfigWhileRecordingTest::RA3FDevSoundQueryRecordConfigWhileRecordingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iQueryRecordingSettings(EFalse), iExCapRate(0),
		iExCapChannels(0)
	{
	}

RA3FDevSoundQueryRecordConfigWhileRecordingTest* RA3FDevSoundQueryRecordConfigWhileRecordingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordConfigWhileRecordingTest* self = new (ELeave) RA3FDevSoundQueryRecordConfigWhileRecordingTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordConfigWhileRecordingTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
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
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundQueryRecordConfigWhileRecordingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordConfigWhileRecordingTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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

void RA3FDevSoundQueryRecordConfigWhileRecordingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iQueryRecordingSettings)
		{
		TMMFCapabilities capabilitiesGet;
		TBuf<KMaxSampleRateStringLength> stringSampleRateGet;
		TBuf<KMaxChannelsStringLength> stringChannelsGet;
		TBuf<KMaxSampleRateStringLength> stringSampleRateExpected;
		TBuf<KMaxChannelsStringLength> stringChannelsExpected;
		INFO_PRINTF1(_L("Calling CMMFDevSound::Config while recording"));
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
		iQueryRecordingSettings = ETrue;
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0024
 */
RA3FDevSoundQueryRecordBalanceWhileRecordingTest::RA3FDevSoundQueryRecordBalanceWhileRecordingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iQueryRecordingSettings(EFalse),
		iExLRecordBalance(0),
		iExRRecordBalance(0)
	{
	}

RA3FDevSoundQueryRecordBalanceWhileRecordingTest* RA3FDevSoundQueryRecordBalanceWhileRecordingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordBalanceWhileRecordingTest* self = new (ELeave) RA3FDevSoundQueryRecordBalanceWhileRecordingTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordBalanceWhileRecordingTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KExLRecordBalance, iExLRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExLRecordBalance);
		StopTest(KErrNotFound);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KExRRecordBalance, iExRRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExRRecordBalance);
		StopTest(KErrNotFound);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundQueryRecordBalanceWhileRecordingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordBalanceWhileRecordingTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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

void RA3FDevSoundQueryRecordBalanceWhileRecordingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iQueryRecordingSettings)
		{
		INFO_PRINTF1(_L("Calling query method GetRecordBalanceL() while recording"));

		TInt getLRecordBalance = 0;
		TInt getRRecordBalance = 0;
		TRAPD(err, iMMFDevSound->GetRecordBalanceL(getLRecordBalance, getRRecordBalance));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
			StopTest(err);
			return;
			}
		INFO_PRINTF3(_L("Record balance is set to %d and %d "), getLRecordBalance, getRRecordBalance);
		if(getLRecordBalance == iExLRecordBalance && getRRecordBalance == iExRRecordBalance)
			{
			INFO_PRINTF1(_L("Record balance values match default values"));
			}
		else
			{
			ERR_PRINTF1(_L("Record balance values do not match default values"));
			ERR_PRINTF3(_L("Expected left = %d, right = %d"),iExLRecordBalance,iExRRecordBalance);
			StopTest(KErrNone, EFail);
			}
		iQueryRecordingSettings = ETrue;
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0025
 */
RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest::RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iQueryRecordingSettings(EFalse),
		iCurrentTypeInTest(0)
	{
	}

RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest* RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest* self = new (ELeave) RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				if(iQueryRecordingSettings == EFalse)
					{
					INFO_PRINTF1(_L("DevSound Initialized to record audio."));
					INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
					TRAPD(err, iMMFDevSound->RecordInitL());
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
						StopTest(err);
						break;
						}
					else
						{
						INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
						StartTimer((iDuration * KMicroSecsInOneSec));
						}
					INFO_PRINTF1(_L("DevSound State: EStateRecording"));
					iDevSoundState = EStateRecording;
					}
				else
					{
					// Complete test when all types have been used of initialization
					if(iCurrentTypeInTest == iSupportedFourCCs.Count())
						{
						INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
						StopTest();
						}
					else
						{
						TFourCC fourCC = iSupportedFourCCs[iCurrentTypeInTest];
						TBuf<4> name;
						for (TInt i=0; i<= KFourCCMaxBitDisplacement; i+=K8Bits)
							{
							name.Append ( (TUint8)(fourCC.FourCC() >> i));
							}
						INFO_PRINTF3(_L("Initializing for Output Data type: 0x%x  %S "), fourCC.FourCC(), &name);
						TRAPD(err, iMMFDevSound->InitializeL(*this, fourCC, EMMFStateRecording));
						if (err != KErrNone)
							{
							ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
							StopTest(err);
							break;
							}
						}
					iCurrentTypeInTest++;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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

void RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iQueryRecordingSettings)
		{
		INFO_PRINTF1(_L("Calling query method GetSupportedOutputDataTypesL() while recording"));

		TMMFPrioritySettings prioritySettings;
		TRAPD(err,iMMFDevSound->GetSupportedOutputDataTypesL(iSupportedFourCCs,prioritySettings));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Getting supported output data types failed ! Left with error = %d"), err);
			StopTest(err);
			return;
			}
		for (TInt x = 0; x < iSupportedFourCCs.Count(); x++)
			{
			TFourCC fourCC = iSupportedFourCCs[x];
			TBuf<4> name;
			for(TInt i=0; i<= KFourCCMaxBitDisplacement; i+=K8Bits)
				{
				name.Append( (TUint8)(fourCC.FourCC() >> i) );
				}
			INFO_PRINTF3(_L("Supported Input Data types: 0x%x  %S "), fourCC.FourCC(), &name);
			}

		iQueryRecordingSettings = ETrue;

		}
	else
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
		Fsm(EEventTimerComplete, KErrNone);
		}
	}

void RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("DevSound called BufferToBeEmptied."));
	if (!aBuffer)
		{
		INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
		StopTest(KErrNone,EFail);
		return;
		}
	iBuffer = aBuffer;
	if(aBuffer->LastBuffer())
		{
		if(iDevSoundState == EStatePause)
			{
			// We need to call CMMFDevSound->Stop() here if last buffer flag set
			INFO_PRINTF1(_L("In Paused state and have received last CMMFBuffer"));
			INFO_PRINTF1(_L("iMMFDevSound->Stop()"));
			iMMFDevSound->Stop();
			iDevSoundState = EStateInitializing;
			Fsm(EEventInitComplete, KErrNone);
			}
		else
			{
			INFO_PRINTF1(_L("***** Unknown behaviour: Last buffer flag set before calling CMMFDevSound->Pause()"));
			StopTest(KErrNone,EFail);
			}
		}
	else
		{
		INFO_PRINTF1(_L("DevSound Event: EEventBTBE"));
		Fsm(EEventBTBE, KErrNone);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0026
 */
RA3FDevSoundQueryRecordMaxGainWhileRecordingTest::RA3FDevSoundQueryRecordMaxGainWhileRecordingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iQueryRecordingSettings(EFalse),
		iExGain(0)
	{
	}

RA3FDevSoundQueryRecordMaxGainWhileRecordingTest* RA3FDevSoundQueryRecordMaxGainWhileRecordingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordMaxGainWhileRecordingTest* self = new (ELeave) RA3FDevSoundQueryRecordMaxGainWhileRecordingTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordMaxGainWhileRecordingTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KExGain, iExGain) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KExGain);
		StopTest(KErrNotFound);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundQueryRecordMaxGainWhileRecordingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordMaxGainWhileRecordingTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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
			StopTest(aError,EFail);
			}
		}
	}

void RA3FDevSoundQueryRecordMaxGainWhileRecordingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iQueryRecordingSettings)
		{
		INFO_PRINTF1(_L("Calling query method MaxGain() while recording"));

		TInt maxGain = iMMFDevSound->MaxGain();
		INFO_PRINTF2(_L("Record Max Gain  %d"), maxGain);
		if(maxGain == iExGain)
			{
			INFO_PRINTF1(_L("Max Gain matches the expected value"));
			}
		else
			{
			ERR_PRINTF2(_L("Max Gain did not match the expected gain value = %d"), iExGain);
			StopTest(KErrNone, EFail);
			}
		iQueryRecordingSettings = ETrue;
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0027
 */
RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest::RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iQueryRecordingSettings(EFalse)
	{
	}

RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest* RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest* self = new (ELeave) RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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
			StopTest(aError,EFail);
			}
		}
	}

void RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iQueryRecordingSettings)
		{
		INFO_PRINTF1(_L("Calling query method SamplesRecorded() while recording"));

		TInt samplesRecorded = iMMFDevSound->SamplesRecorded();
		INFO_PRINTF2(_L("Samples Recorded  %d"), samplesRecorded);
		if(samplesRecorded > 0)
			{
			INFO_PRINTF1(_L("SamplesRecorded() returned an expected value of greater than zero"));
			}
		else
			{
			ERR_PRINTF1(_L("SamplesRecorded() returned an invalid value not greater than zero"));
			StopTest(KErrNone, EFail);
			}
		iQueryRecordingSettings = ETrue;

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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0028
 */
RA3FDevSoundQueryRecordGainWhileRecordingTest::RA3FDevSoundQueryRecordGainWhileRecordingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0),
		iQueryRecordingSettings(EFalse)
	{
	}

RA3FDevSoundQueryRecordGainWhileRecordingTest* RA3FDevSoundQueryRecordGainWhileRecordingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundQueryRecordGainWhileRecordingTest* self = new (ELeave) RA3FDevSoundQueryRecordGainWhileRecordingTest(aTestName);
	return self;
	}

void RA3FDevSoundQueryRecordGainWhileRecordingTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundQueryRecordGainWhileRecordingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundQueryRecordGainWhileRecordingTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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
			StopTest(aError,EFail);
			}
		}
	}

void RA3FDevSoundQueryRecordGainWhileRecordingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iQueryRecordingSettings)
		{
		INFO_PRINTF1(_L("Calling query method Gain() while recording"));

		INFO_PRINTF2(_L("Current Gain = %d"), iMMFDevSound->Gain());
		if(iMMFDevSound->Gain() == (iMMFDevSound->MaxGain()+1)/2)
			{
			INFO_PRINTF2(_L("Gain equals the expected default value of MaxGain/2 = %d"),iMMFDevSound->MaxGain()/2);
			}
		else
			{
			ERR_PRINTF2(_L("Gain is different than the expected default value of MaxGain/2 = %d"),iMMFDevSound->MaxGain()/2);
			StopTest(KErrNone, EFail);
			}
		iQueryRecordingSettings = ETrue;

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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0029
 */
RA3FDevSoundRecordingTest::RA3FDevSoundRecordingTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0)
	{
	}

RA3FDevSoundRecordingTest* RA3FDevSoundRecordingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordingTest* self = new (ELeave) RA3FDevSoundRecordingTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordingTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundRecordingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordingTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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
			StopTest(aError,EFail);
			}
		}
	}

void RA3FDevSoundRecordingTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0030
 */
RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest::RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0),
		iRecordAgainToConfirmInitializedState(EFalse)
	{
	}

RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest* RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest* self = new (ELeave) RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);

	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Write(buffer->Data());
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("iFile.Write() returned the error %d"), err);
					StopTest(err);
					break;
					}
				// Continue recording data
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Write(buffer->Data());
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("RFile::Write returned the error %d"), err);
					StopTest(err);
					return;
					}
				// Continue recording data
				iAsyncWriteBTFAO->Start(buffer);
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
			StopTest(aError,EFail);
			}
		}
	}

void RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

void RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("DevSound called BufferToBeEmptied."));
	if (!aBuffer)
		{
		INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
		StopTest(KErrNone,EFail);
		return;
		}
	iBuffer = aBuffer;
	if(aBuffer->LastBuffer())
		{
		if(iDevSoundState == EStatePause)
			{
			// We need to call CMMFDevSound->Stop() here if last buffer flag set
			INFO_PRINTF1(_L("In Paused state and have received last CMMFBuffer"));
			INFO_PRINTF1(_L("iMMFDevSound->Stop()"));
			iMMFDevSound->Stop();
			if(!iRecordAgainToConfirmInitializedState)
				{
				INFO_PRINTF1(_L("To test whether or not Devsound is in the initialized state, a new record process is launched"));
				INFO_PRINTF1(_L("iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					return;
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
				iRecordAgainToConfirmInitializedState = ETrue;
				INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
				StartTimer(iDuration * KMicroSecsInOneSec);
				}
			else
				{
				StopTest();
				}
			}
		else
			{
			INFO_PRINTF1(_L("***** Unknown behaviour: Last buffer flag set before calling CMMFDevSound->Pause()"));
			StopTest(KErrUnknown);
			}
		}
	else
		{
		INFO_PRINTF1(_L("DevSound Event: EEventBTBE"));
		Fsm(EEventBTBE, KErrNone);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0031
 */
RA3FDevSoundRecordPreemptionTest::RA3FDevSoundRecordPreemptionTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0),
		iOtherClientStarted(EFalse),
		iOtherFilename(KNullDesC),
		iRecordStopped(EFalse)
	{
	}

RA3FDevSoundRecordPreemptionTest* RA3FDevSoundRecordPreemptionTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordPreemptionTest* self = new (ELeave) RA3FDevSoundRecordPreemptionTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordPreemptionTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	// Get the filename of the second audio file to record
	if ( !GetStringFromConfig(iTestStepName, KOtherFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KOtherFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	iOtherFilename.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	iDevsoundRecordClient = CA3FDevSoundRecordClient::NewL(*this,iFourCCCode,iFs,iSuite);
	}

void RA3FDevSoundRecordPreemptionTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordPreemptionTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Initializing DevSound..."));
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
				INFO_PRINTF1(_L("iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
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

void RA3FDevSoundRecordPreemptionTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iOtherClientStarted)
		{
		INFO_PRINTF1(_L("Initializing higher priority devsound client..."));
		iDevsoundRecordClient->SetPriority(KMaximumPriority);
		TInt err = iDevsoundRecordClient->InitializeRecordClient(iOtherFilename);
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Failed initializing to record audio thru second devsound client..."));
			StopTest(err);
			}
		iOtherClientStarted = ETrue;
		}
	else
		{
		if (!iRecordStopped)
			{
			iDevsoundRecordClient->StopRecord();
			iRecordStopped = ETrue;
			}
		else
			{
			iTimer->Cancel();
			StopTest();
			}
		}
	}

void RA3FDevSoundRecordPreemptionTest::ClientInitializeCompleteCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound InitializeComplete() callback =========="));
	if (aError != KErrNone)
		{
		StopTest(aError);
		}
	else
		{
		INFO_PRINTF2(_L("Second DevSound client callback received error = %d"), aError);
		INFO_PRINTF1(_L("Starting to record audio thru second devsound client..."));
		TInt err = iDevsoundRecordClient->RecordAudio();
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Failed starting to record audio thru second devsound client..."));
			StopTest(err);
			}
		}
	}

void RA3FDevSoundRecordPreemptionTest::ClientBufferToBeEmptiedCallback(TInt aError)
	{
	if (iOtherClientStarted && aError == KErrNone)
		{
		INFO_PRINTF1(_L("Second DevSound instance called BufferToBeEmptied."));
		}
	else if(iOtherClientStarted && aError == KErrGeneral)
		{
		INFO_PRINTF1(_L("Second DevSound instance BufferToBeEmptied callback received a NULL CMMFBuffer"));
		StopTest(aError);
		}
	if(iOtherClientStarted && aError == KErrUnknown)
		{
		INFO_PRINTF1(_L("***** Second DevSound instance :Unknown behaviour: Last buffer flag set before calling CMMFDevSound->Pause()"));
		StopTest(aError);
		}
	}

void RA3FDevSoundRecordPreemptionTest::ClientRecordErrorCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound RecordError() callback =========="));
	if(aError == KErrUnderflow)
		{
		INFO_PRINTF2(_L("Other DevSound client called RecordError with error = %d"), aError);
		StopTest(aError);
		}
	}

void RA3FDevSoundRecordPreemptionTest::ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
	{
	switch(aError)
		{
		case EInvalidClientFSMEvent:
			ERR_PRINTF1(_L("Second devsound client received an invalid Event..."));
			break;
		case EInvalidClientFSMState:
			ERR_PRINTF1(_L("Second devsound client received an invalid State..."));
			break;
		case EWriteFileErrorInClient:
			ERR_PRINTF1(_L("Second devsound client could not write to file..."));
			break;
		default:
			break;
		}
	StopTest(KErrNone, EFail);
	}

void RA3FDevSoundRecordPreemptionTest::RecordError(TInt aError)
	{
	INFO_PRINTF1(_L("========== First instance of DevSound called RecordError()=========="));
	if (aError == KErrUnderflow)
		{
		ERR_PRINTF1(_L("First instance of DevSound was not preempted"));
		StopTest(aError);
		}
	else if(aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
		{
		INFO_PRINTF2(_L("DevSound called RecordError preemption with error = %d as expected"), aError);
		}
	else
		{
		ERR_PRINTF2(_L("First instance of DevSound called RecordError() with unexpected error code: %d"), aError);
		StopTest(aError, EFail);
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0032
 */
RA3FDevSoundRecordRejectionTest::RA3FDevSoundRecordRejectionTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName),
		iDuration(0), iOtherClientStarted(EFalse),
		iOtherFilename(KNullDesC)
	{
	}

RA3FDevSoundRecordRejectionTest* RA3FDevSoundRecordRejectionTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordRejectionTest* self = new (ELeave) RA3FDevSoundRecordRejectionTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordRejectionTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	// Get the filename of the second audio file to record
	if ( !GetStringFromConfig(iTestStepName, KOtherFilename, filename) )
		{
	 	ERR_PRINTF2(KMsgErrorGetParameter, &KOtherFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
	 	}
	iOtherFilename.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	iDevsoundRecordClient = CA3FDevSoundRecordClient::NewL(*this,iFourCCCode,iFs,iSuite);
	}

void RA3FDevSoundRecordRejectionTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordRejectionTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Initializing DevSound..."));
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
				INFO_PRINTF1(_L("iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
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
				iAsyncWriteBTFAO->Start(buffer);
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

void RA3FDevSoundRecordRejectionTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	if(!iOtherClientStarted)
		{
		INFO_PRINTF1(_L("Starting lower priority devsound client..."));
		iDevsoundRecordClient->SetPriority(KMinimumPriority);
		TInt err = iDevsoundRecordClient->InitializeRecordClient(iOtherFilename);
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Failed starting to record audio thru second devsound client..."));
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

void RA3FDevSoundRecordRejectionTest::ClientInitializeCompleteCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound InitializeComplete() callback =========="));
	if (aError != KErrNone)
		{
		StopTest(aError);
		}
	else
		{
		INFO_PRINTF2(_L("Second DevSound client callback received error = %d"), aError);
		INFO_PRINTF1(_L("Starting to record audio thru second devsound client..."));
		TInt err = iDevsoundRecordClient->RecordAudio();
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Failed starting to record audio thru second devsound client..."));
			StopTest(err);
			}
		}
	}

void RA3FDevSoundRecordRejectionTest::ClientRecordErrorCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound RecordError() callback =========="));
	INFO_PRINTF2(_L("Second DevSound client callback with error = %d"), aError);
	if(aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied && iOtherClientStarted)
		{
		INFO_PRINTF1(_L("========== Rejecting 2nd DevSound Instance =========="));
		}
	else
		{
		ERR_PRINTF1(_L("Second DevSound client was not rejected as expected"));
		StopTest(aError,EFail);
		}
	}

void RA3FDevSoundRecordRejectionTest::ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
	{
	INFO_PRINTF2(_L("In rejection, ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack should not be called %d"), aError);
	StopTest(aError, EFail);
	}

void RA3FDevSoundRecordRejectionTest::ClientBufferToBeEmptiedCallback(TInt aError)
	{
	INFO_PRINTF2(_L("In rejection, BufferToBeEmptied Callback of 2nd DevSound client should not be called %d"), aError);
	StopTest(aError, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0033
 */
RA3FDevSoundRecordDataWithoutRecordInitTest::RA3FDevSoundRecordDataWithoutRecordInitTest(const TDesC& aTestName)
	: 	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundRecordDataWithoutRecordInitTest* RA3FDevSoundRecordDataWithoutRecordInitTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordDataWithoutRecordInitTest* self = new (ELeave) RA3FDevSoundRecordDataWithoutRecordInitTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordDataWithoutRecordInitTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordDataWithoutRecordInitTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(_L("RA3FDevSoundRecordDataWithoutRecordInitTest"), EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this,iFourCCCode,EMMFStateRecording));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));

				INFO_PRINTF1(_L("Calling RecordData without RecordInitL"));
				iMMFDevSound->RecordData();
				StopTest(KErrNone, EFail);
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
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0034
 */
RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest::RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest(const TDesC& aTestName)
	:	RA3FDevSoundTestBase(aTestName), iDuration(0)
	{
	}

RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest* RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest* self = new (ELeave) RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	// Open file for recording using RFile
	TFileName fileNameType;
	fileNameType.Copy(filename);
	INFO_PRINTF2(_L("Opening file for recording %S"), &fileNameType);
	TInt err = iFile.Replace(iFs, fileNameType, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
		iFs.Close();
		StopTest(err);
		return;
		}
	TPtrC fourccCode;
	if ( !GetStringFromConfig(iTestStepName, KFourccCode, fourccCode) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);
	}

void RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to Record Audio"));
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
				INFO_PRINTF1(_L("DevSound Initialized to record audio."));
				INFO_PRINTF1(_L("Calling iMMFDevSound->RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Starting timer %d uS"), (iDuration * KMicroSecsInOneSec));
					StartTimer((iDuration * KMicroSecsInOneSec));
					}
				INFO_PRINTF1(_L("DevSound State: EStateRecording"));
				iDevSoundState = EStateRecording;

				INFO_PRINTF1(_L("Second call to iMMFDevSound->RecordInitL()"));
				TRAP(err, iMMFDevSound->RecordInitL());
				if (err == KErrNotReady)
					{
					ERR_PRINTF2(_L("DevSound RecordInitL was called more than once, error is the expected= %d"), err);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("DevSound RecordInitL was called more than once, error code different than expected= %d"), err);
					StopTest(err, EFail);
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
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
				break;
				}
			case EStatePause:
				{
				if (aDevSoundEvent == EEventBTBE)
					{
					CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
					iAsyncWriteBTFAO->Start(buffer);
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

void RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("TimerEvent called"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}




