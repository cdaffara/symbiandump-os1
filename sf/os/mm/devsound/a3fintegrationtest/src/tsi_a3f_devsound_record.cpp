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
// Implementation of Integraton test step classes for A3F DevSound Record test cases.
// 
//

#include "tsi_a3f_devsound_record.h"
#include "hal.h"
#include "hal_data.h"

/*
 *
 * RTestStepA3FDevSoundRecordBase - Test step constructor
 *
 */
RTestStepA3FDevSoundRecordBase::RTestStepA3FDevSoundRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
:RAsyncTestStep(),
 iMMFDevSound(NULL),
 iBuffer(NULL),
 iRecordTimer(NULL),
 iAsyncWriteBTFAO(NULL),
 iOutputDataType(aOutputDataType),
 iSectName(aSectName),
 iFilename(KNullDesC),
 iSampleRate(KNullDesC),
 iMonoStereo(KNullDesC),
 iState(EStateRecordStart),
 iBufferCount(0),
 iRecordTime(KMicroSecsInTenSecs),
 iRecordInterval(0),
 iLeftPercentage(0),
 iRightPercentage(0),
 iRecordCycleCounter(0),
 iConfigureSettings(EFalse),
 iAsyncWriteBTF(EFalse),
 iLogBTBECallback(ETrue)
	{
	// From RTestStep
	iTestStepName = aTestName; // store the name of the test case

	}

/*
 *
 * RTestStepA3FDevSoundRecordBase - Test step constructor
 *
 */
RTestStepA3FDevSoundRecordBase::RTestStepA3FDevSoundRecordBase(const TDesC& aTestName, const TFourCC& aOutputDataType)
:RAsyncTestStep(),
 iMMFDevSound(NULL),
 iBuffer(NULL),
 iRecordTimer(NULL),
 iAsyncWriteBTFAO(NULL),
 iOutputDataType(aOutputDataType),
 iSectName(KNullDesC),
 iFilename(KNullDesC),
 iSampleRate(KNullDesC),
 iMonoStereo(KNullDesC),
 iState(EStateRecordStart),
 iBufferCount(0),
 iRecordTime(KMicroSecsInTenSecs),
 iRecordInterval(0),
 iLeftPercentage(0),
 iRightPercentage(0),
 iRecordCycleCounter(0),
 iConfigureSettings(EFalse),
 iAsyncWriteBTF(EFalse),
 iLogBTBECallback(ETrue)
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
void RTestStepA3FDevSoundRecordBase::KickoffTestL()
	{
	// Connect to file server
	TInt err;
	err = iFs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not connect to file server. Error = %d"), err);
		StopTest(err);
		return;
		}

	// Get the file name of the audio file to record into
	TPtrC recordFileSpec;
	#ifdef __WINS__
		if(!GetStringFromConfig(iSectName, _L("EmuRecordFile"), recordFileSpec))
		{
		TPtrC keyName(_L("EmuRecordFile"));
		ERR_PRINTF3(_L("Error in getting file name from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	#elif __MARM__
		if(!GetStringFromConfig(iSectName, _L("HwRecordFile"), recordFileSpec))
		{
		TPtrC keyName(_L("HwRecordFile"));
		ERR_PRINTF3(_L("Error in getting file name from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}
	#endif

	TParse recordFile;
	recordFile.Set(recordFileSpec, NULL, NULL);

	iFilename = recordFile.FullName();

	TFileName filePath = recordFile.DriveAndPath();
	INFO_PRINTF2(_L("Output directory for recorded file (%S)"), &filePath);

	// Create output directory for recorded audio files if it doesn't already exist
	err = iFs.MkDirAll(filePath);
	if (err != KErrAlreadyExists && err != KErrNone)
		{
		ERR_PRINTF3(_L("RFs::MkDir failed creating output directory (%S) for recorded data files with %d"), err, &filePath);
		StopTest(err);
		return;
		}

	// Create/replace a file using RFile for recording into
	err = iFile.Replace(iFs, iFilename, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create output file to record into. Error = %d"), err);
		StopTest(err);
		return;
		}

	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	iRecordTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, *this);

	DoKickoffTestL();

	Fsm(EEventRecordStart,KErrNone); // call to start the DevSound finite state machine for recording

	}

/*
 *
 * CloseTest
 *
 */
void RTestStepA3FDevSoundRecordBase::CloseTest()
	{
	if (iMMFDevSound)
		{
		INFO_PRINTF1(_L("--- Deleting DevSound object..."));
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}

	if (iRecordTimer)
		{
		delete iRecordTimer;
		iRecordTimer = NULL;
		}

	if (iAsyncWriteBTFAO)
		{
		delete iAsyncWriteBTFAO;
		iAsyncWriteBTFAO = NULL;
		}

	iFile.Close();
	iFs.Close();

	}

/*
 *
 * CallStopTest
 *
 */
void RTestStepA3FDevSoundRecordBase::CallStopTest(TInt aError)
	{
	StopTest(aError);
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RTestStepA3FDevSoundRecordBase::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be recorded into -> %S"), &iFilename);
	DisplayCodecInfo();

	}

/*
 *
 * Fsm
 *	- Executes recording events of DevSound in sequence
 *
 */
void RTestStepA3FDevSoundRecordBase::Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateRecordStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RTestStepA3FDevSoundRecordBase"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventRecordStart)
				{
				// Initializing DevSound object for recording mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecordInitializing"));
				iState = EStateRecordInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventRecordStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateRecordInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteRecord && aError == KErrNone)
				{
				if (iConfigureSettings)
					{
					// Set Devsound capability settings using CMMFDevSound::SetConfigL
					SetSampleRateAndChannelMode();
					// Set the microphone gain balance for recording via CMMFDevSound::SetRecordBalanceL
					SetRecordBalanceFromIni();
					}

				// Initialize audio device and start the recording process
				INFO_PRINTF1(_L("Calling CMMFDevSound::RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::RecordInitL() left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecording"));
				iState = EStateRecording;
				StartRecordTimer();

				if (iConfigureSettings)
					{
					// Read DevSound Capability settings using CMMFDevSound::Config() and check for expected settings
					User::After(500000); // wait an approximate time (half second) for actual recording process to start
					CheckSampleRateAndChannelMode();
					// Read the microphone gain balance via CMMFDevSound::GetRecordBalanceL and check for expected gain balance set
					CheckRecordBalance();
					}
				}
			else if (aDevSoundEvent == EEventInitCompleteRecord && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteRecord not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateRecording:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RTestStepA3FDevSoundRecordBase"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file and then continue to record
				WriteBufferToFileAndRecord();
				}
			else if (aDevSoundEvent == EEventRecordTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause()"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("State: EStateRecordPaused"));
				iState = EStateRecordPaused;
				INFO_PRINTF1(_L("-- Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventRecordTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;
			}

		case EStateRecordPaused:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RTestStepA3FDevSoundRecordBase"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file and then continue to record
				WriteBufferToFileAndRecord();
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * InitializeComplete
 *
 */
void RTestStepA3FDevSoundRecordBase::InitializeComplete(TInt aError)
	{
	Fsm(EEventInitCompleteRecord,aError);

	}

/*
 *
 * ToneFinished
 *
 */
void RTestStepA3FDevSoundRecordBase::ToneFinished(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundRecordBase"),EInvalidCallbackCall));

	}

/*
 *
 * BufferToBeFilled
 *
 */
void RTestStepA3FDevSoundRecordBase::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundRecordBase"),EInvalidCallbackCall));

	}

/*
 *
 * PlayError
 *
 */
void RTestStepA3FDevSoundRecordBase::PlayError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundRecordBase"),EInvalidCallbackCall));

	}

/*
 *
 * BufferToBeEmptied
 *
 */
void RTestStepA3FDevSoundRecordBase::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
		StopTest(KErrGeneral);
		return;
		}

	iBuffer = aBuffer;
	if (iLogBTBECallback)
		{
		INFO_PRINTF2(_L("DevSound called BufferToBeEmptied, with size = %d"), aBuffer->RequestSize());
		}

	if(aBuffer->LastBuffer())
		{
		if(iState == EStateRecordPaused)
			{
			// We need to call CMMFDevSound::Stop() here if last buffer flag set
			INFO_PRINTF1(_L("In Paused state and have received last CMMFBuffer"));
			INFO_PRINTF1(_L("Calling CMMFDevSound::Stop()"));
			iMMFDevSound->Stop();

			if(iRecordCycleCounter)
				{
				iRecordCycleCounter--;
				iState = EStateRecordInitializing;
				Fsm(EEventInitCompleteRecord,KErrNone);
				}
			else
				{
				StopTest();
				}
			}
		else
			{
			INFO_PRINTF1(_L("***** Unknown behaviour: Last buffer flag set before calling CMMFDevSound::Pause()"));
			StopTest(KErrUnknown);
			}
		}
	else
		{
		Fsm(EEventBTBE,KErrNone);
		}

	}

/*
 *
 * RecordError
 *
 */
void RTestStepA3FDevSoundRecordBase::RecordError(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called RecordError with error = %d"), aError);
	TInt machineType;
	TInt err = HAL::Get(HALData::EMachineUid, machineType);
	if (err)
		{
		INFO_PRINTF1(_L("Error Getting Device information"));
		StopTest(err);
		}
	else
		{
		if (machineType==0x102864F7 && aError==KErrNotFound)
			{
			INFO_PRINTF1(_L("This is a Known Failure on the naviengine"));
			StopTest(aError, EKnownFailure);
			}
		else
			{
			StopTest(aError);			
			}
		}
	}

/*
 *
 * ConvertError
 *
 */
void RTestStepA3FDevSoundRecordBase::ConvertError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundRecordBase"),EInvalidCallbackCall));

	}

/*
 *
 * DeviceMessage
 *
 */
void RTestStepA3FDevSoundRecordBase::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	__ASSERT_ALWAYS(0,Panic(_L("RTestStepA3FDevSoundRecordBase"),EInvalidCallbackCall));

	}

/*
 *
 * TimerCallback
 *
 */
TInt RTestStepA3FDevSoundRecordBase::TimerCallback(TAny* aPtr)
	{
	static_cast<RTestStepA3FDevSoundRecordBase*>(aPtr)->DoTimerCallback();
	return KErrNone;

	}

/*
 *
 * DoTimerCallback
 *
 */
void RTestStepA3FDevSoundRecordBase::DoTimerCallback()
	{
	iRecordTimer->Cancel(); // kill timer after first event
	TInt recordTime = iRecordTime/KMicroSecsInOneSec;
	TInt recordInterval = iRecordInterval/KMicroSecsInOneSec;
	INFO_PRINTF2(_L("-- RecordTimer stopped after %d seconds..."), recordTime + recordInterval);
	INFO_PRINTF1(_L("-- Checking to see if DevSound already paused before calling CMMFDevSound::Pause()..."));
	__ASSERT_ALWAYS((iState != EStateRecordPaused),Panic(_L("RTestStepA3FDevSoundRecordBase"),EFsmPausedTwice));
	INFO_PRINTF1(_L("... DevSound not already paused, so pausing will now commence"));
	Fsm(EEventRecordTimerComplete, KErrNone);

	}

/*
 *
 * ConvertDesToTMMFSampleRate
 *
 */
TUint RTestStepA3FDevSoundRecordBase::ConvertDesToTMMFSampleRate(const TDesC& aSampleRateString)
	{
	TUint rate = 0;

	switch (SampleRateLookup(aSampleRateString))
		{
		case E8000Hz:
			rate = EMMFSampleRate8000Hz;
			break;
		case E11025Hz:
			rate = EMMFSampleRate11025Hz;
			break;
		case E12000Hz:
			rate = EMMFSampleRate12000Hz;
			break;
		case E16000Hz:
			rate = EMMFSampleRate16000Hz;
			break;
		case E22050Hz:
			rate = EMMFSampleRate22050Hz;
			break;
		case E24000Hz:
			rate = EMMFSampleRate24000Hz;
			break;
		case E32000Hz:
			rate = EMMFSampleRate32000Hz;
			break;
		case E44100Hz:
			rate = EMMFSampleRate44100Hz;
			break;
		case E48000Hz:
			rate = EMMFSampleRate48000Hz;
			break;
		case E64000Hz:
			rate = EMMFSampleRate64000Hz;
			break;
		case E88200Hz:
			rate = EMMFSampleRate88200Hz;
			break;
		case E96000Hz:
			rate = EMMFSampleRate96000Hz;
			break;
		default:
			ERR_PRINTF1(_L("Attempt to convert invalid sample rate"));
			StopTest(KErrGeneral);
		}

	return rate;

	}

/*
 *
 * ConvertDesToTMMFMonoStereo
 *
 */
TUint RTestStepA3FDevSoundRecordBase::ConvertDesToTMMFMonoStereo(const TDesC& aMonoStereoString)
	{
	TUint channelMode = 0;

	switch (ChannelModeLookup(aMonoStereoString))
		{
		case EMono:
			channelMode = EMMFMono;
			break;
		case EStereo:
			channelMode = EMMFStereo;
			break;
		default:
			ERR_PRINTF1(_L("Attempt to convert invalid channel mode"));
			StopTest(KErrGeneral);
		}

	return channelMode;

	}

/*
 *
 * SetSampleRateAndChannelMode
 *
 */
void RTestStepA3FDevSoundRecordBase::SetSampleRateAndChannelMode()
	{
	TMMFCapabilities capabilities;

	//The test reads the sampling rate from 'tsi_a3f_devsound_record.ini' for emulator , h4 and h6,
	//and from 'tsi_a3f_devsound_record_ne.ini' for the navi engine. 
	//Future hardware may also need a custom ini file tat specifies supported sample rates.
	
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

	capabilities.iRate = ConvertDesToTMMFSampleRate(iSampleRate);
	capabilities.iChannels = ConvertDesToTMMFMonoStereo(iMonoStereo);
	INFO_PRINTF3(_L("Sample rate set: %S, Channel mode set: %S"),&iSampleRate, &iMonoStereo);

	__ASSERT_ALWAYS((iState != EStateRecordPaused),Panic(_L("RTestStepA3FDevSoundRecordBase"),EFsmPausedTwice));
	iMMFDevSound->SetConfigL(capabilities);

	}

/*
 *
 * CheckSampleRateAndChannelMode
 *
 */
void RTestStepA3FDevSoundRecordBase::CheckSampleRateAndChannelMode()
	{
	TUint rate = ConvertDesToTMMFSampleRate(iSampleRate);
	TUint channelMode = ConvertDesToTMMFMonoStereo(iMonoStereo);
	TMMFCapabilities capabilities = iMMFDevSound->Config();

	if (capabilities.iRate == rate && capabilities.iChannels == channelMode)
		{
		INFO_PRINTF1(_L("Capability settings read during recording are those that were expected:"));
		INFO_PRINTF3(_L("sample rate: %S and channel mode: %S"), &iSampleRate, &iMonoStereo);
		}
	else
		{
		ERR_PRINTF1(_L("Capability settings read during recording are not what were expected:"));
		ERR_PRINTF5(_L("Expected sample rate: %S and channel mode: %S. Actual sample rate: %d and channel mode: %d"), &iSampleRate, &iMonoStereo, capabilities.iRate, capabilities.iChannels);
		StopTest(KErrUnknown);
		}

	}

/*
 *
 * SetRecordBalanceFromIni
 *
 */
void RTestStepA3FDevSoundRecordBase::SetRecordBalanceFromIni()
	{
	if (!GetIntFromConfig(iSectName, _L("LeftPcBalance"), iLeftPercentage))
		{
		TPtrC keyName(_L("LeftPcBalance"));
		ERR_PRINTF3(_L("Error in getting left record balance value from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	if (!GetIntFromConfig(iSectName, _L("RightPcBalance"), iRightPercentage))
		{
		TPtrC keyName(_L("RightPcBalance"));
		ERR_PRINTF3(_L("Error in getting right record balance value from config file in %S section via the %S key field."), &iSectName, &keyName);
		StopTest(KErrPathNotFound);
		return;
		}

	INFO_PRINTF3(_L("Left percentage record balance set: %d, Right percentage record balance set: %d"),iLeftPercentage, iRightPercentage);

	iMMFDevSound->SetRecordBalanceL(iLeftPercentage,iRightPercentage);

	}

/*
 *
 * CheckRecordBalance
 *
 */
void RTestStepA3FDevSoundRecordBase::CheckRecordBalance()
	{
	TInt leftPercentage;
	TInt rightPercentage;
	iMMFDevSound->GetRecordBalanceL(leftPercentage,rightPercentage);

	if(leftPercentage == iLeftPercentage && rightPercentage == iRightPercentage)
		{
		INFO_PRINTF3(_L("Microphone gain balance set for recording as expected. LeftPercentage: %d RightPercentage: %d"), leftPercentage,rightPercentage);
		}
	else
		{
		ERR_PRINTF3(_L("Microphone gain balance set for recording NOT as expected. LeftPercentage: %d RightPercentage: %d"), leftPercentage,rightPercentage);
		ERR_PRINTF3(_L("Expected microphone gain balance values: LeftPercentage -> %d RightPercentage -> %d"), iLeftPercentage,iRightPercentage);
		StopTest(KErrUnknown);
		}

	}

/*
 *
 * DisplayCodecInfo
 *
 */
void RTestStepA3FDevSoundRecordBase::DisplayCodecInfo()
	{
	// Display trace about codec under test
	TUint32 fourCC = iOutputDataType.FourCC();
	// Convert the TUint32 into a single character values by displaying the first byte only
	INFO_PRINTF5(_L("Codec under test -> %c%c%c%c"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24);

	}

/*
 *
 * DisplaySamplesAndBuffersRecorded
 *
 */
void RTestStepA3FDevSoundRecordBase::DisplaySamplesAndBuffersRecorded()
	{
	// Display the samples recorded
	TInt sampRec = iMMFDevSound->SamplesRecorded();
	INFO_PRINTF2(_L("Number of Samples recorded = %d"), sampRec);
	// Display the buffer count
	INFO_PRINTF2(_L("Number of Buffers recorded = %d"), iBufferCount);

	}

/*
 *
 * StartRecordTimer
 *
 */
void RTestStepA3FDevSoundRecordBase::StartRecordTimer()
	{
	// Get the record time
	if (GetIntFromConfig(iSectName, _L("RecordTime"), iRecordTime))
		{
		INFO_PRINTF2(_L("Record Time taken from config file: %d"), iRecordTime);
		}
	else
		{
		INFO_PRINTF2(_L("Default record time being used: %d secs"), (iRecordTime/KMicroSecsInOneSec));
		}

	// Start the record timer
	TCallBack callback (TimerCallback, this);
	iRecordTimer->Start(iRecordTime, iRecordInterval, callback);
	INFO_PRINTF1(_L("Record Timer has been started"));

	}

/*
 *
 * WriteBufferToFile
 *
 */
void RTestStepA3FDevSoundRecordBase::WriteBufferToFileAndRecord()
	{
	CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
	if (iAsyncWriteBTF)
		{
		// Async RFile::Write version
		iAsyncWriteBTFAO->Start(buffer);
		}
	else
		{
		// Sync RFile::Write version
		TInt err = iFile.Write(buffer->Data());
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("RFile::Write returned the error %d"), err);
			StopTest(err);
			return;
			}
		// Continue recording data to a buffer
		iMMFDevSound->RecordData();
		}

	iBufferCount++;

	}

/*
 *
 * SampleRateLookup
 *
 */
TInt RTestStepA3FDevSoundRecordBase::SampleRateLookup(const TDesC& aSampleRate)
	{
	const TPtrC KSampleRateLookupTable[] =
	{
	_L("EMMFSampleRate8000Hz"),
	_L("EMMFSampleRate11025Hz"),
	_L("EMMFSampleRate12000Hz"),
	_L("EMMFSampleRate16000Hz"),
	_L("EMMFSampleRate22050Hz"),
	_L("EMMFSampleRate24000Hz"),
	_L("EMMFSampleRate32000Hz"),
	_L("EMMFSampleRate44100Hz"),
	_L("EMMFSampleRate48000Hz"),
	_L("EMMFSampleRate64000Hz"),
	_L("EMMFSampleRate88200Hz"),
	_L("EMMFSampleRate96000Hz"),
	};
	TInt length = sizeof KSampleRateLookupTable / sizeof *KSampleRateLookupTable;

	for (TInt i=0; i<length; i++)
		{
		if(aSampleRate.CompareF(KSampleRateLookupTable[i]) == 0)
			{
			return i;
			}
		}
	return ESampleRateInvalid;
	}

/*
 *
 * ChannelModeLookup
 *
 */
TInt RTestStepA3FDevSoundRecordBase::ChannelModeLookup(const TDesC& aChannelMode)
	{
	const TPtrC KChannelModeLookupTable[] =
	{
	_L("EMMFMono"),
	_L("EMMFStereo"),
	};

	TInt length = sizeof KChannelModeLookupTable / sizeof *KChannelModeLookupTable;

	for (TInt i=0; i<length; i++)
		{
		if(aChannelMode.CompareF(KChannelModeLookupTable[i]) == 0)
			{
			return i;
			}
		}
	return EChannelModeInvalid;
	}

//
// RStepA3FDevSoundRecordSimple
//

/*
 *
 * RStepA3FDevSoundRecordSimple - Test step constructor
 *
 */
RStepA3FDevSoundRecordSimple::RStepA3FDevSoundRecordSimple(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aSectName, aOutputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundRecordSimple* RStepA3FDevSoundRecordSimple::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundRecordSimple* self = new (ELeave) RStepA3FDevSoundRecordSimple(aTestName, aSectName, aOutputDataType);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundRecordSimple::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be recorded into -> %S"), &iFilename);
	DisplayCodecInfo();
	iConfigureSettings = ETrue;

	}

//
// RStepA3FDevSoundRecordLong
//

/*
 *
 * RStepA3FDevSoundRecordLong - Test step constructor
 *
 */
RStepA3FDevSoundRecordLong::RStepA3FDevSoundRecordLong(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aSectName, aOutputDataType),
 iRecordCycleCounter(1),
 iRecordIntervalMultiplier(iRecordCycleCounter)
	{
	// From RTestStepA3FDevSoundRecordBase
	iLogBTBECallback = EFalse;
	
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundRecordLong* RStepA3FDevSoundRecordLong::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundRecordLong* self = new (ELeave) RStepA3FDevSoundRecordLong(aTestName, aSectName, aOutputDataType);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundRecordLong::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be recorded into -> %S"), &iFilename);
	DisplayCodecInfo();

	// Get the record interval
	if (GetIntFromConfig(iSectName, _L("RecordInterval"), iRecordInterval))
		{
		INFO_PRINTF2(_L("Record Interval taken from config file: %d"), iRecordInterval);
		}
	else
		{
		INFO_PRINTF2(_L("Default record interval being used: %d secs"), (iRecordInterval/KMicroSecsInOneSec));
		}

	}

/*
 *
 * DoTimerCallback
 *
 */
void RStepA3FDevSoundRecordLong::DoTimerCallback()
	{
	if(iRecordCycleCounter)
		{
		// Continue recording for another cycle
		INFO_PRINTF2(_L("Recording continued for another %d seconds"), (iRecordInterval/KMicroSecsInOneSec));
		iRecordCycleCounter--;
		}
	else
		{
		iRecordTimer->Cancel(); // kill timer
		TInt recordTime = iRecordTime/KMicroSecsInOneSec;
		TInt recordInterval = iRecordInterval/KMicroSecsInOneSec;
		INFO_PRINTF2(_L("-- RecordTimer stopped after %d seconds..."), recordTime + (recordInterval*iRecordIntervalMultiplier));
		INFO_PRINTF1(_L("-- Checking to see if DevSound already paused before calling CMMFDevSound::Pause()..."));
		__ASSERT_ALWAYS((iState != EStateRecordPaused),Panic(_L("RStepA3FDevSoundRecordLong"),EFsmPausedTwice));
		INFO_PRINTF1(_L("... DevSound not already paused, so pausing will now commence"));
		Fsm(EEventRecordTimerComplete, KErrNone);
		}
	}

//
// RStepA3FDevSoundRecordMultipleTimes
//

/*
 *
 * RStepA3FDevSoundRecordMultipleTimes - Test step constructor
 *
 */
RStepA3FDevSoundRecordMultipleTimes::RStepA3FDevSoundRecordMultipleTimes(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aSectName, aOutputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundRecordMultipleTimes* RStepA3FDevSoundRecordMultipleTimes::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundRecordMultipleTimes* self = new (ELeave) RStepA3FDevSoundRecordMultipleTimes(aTestName, aSectName, aOutputDataType);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundRecordMultipleTimes::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be recorded into -> %S"), &iFilename);
	DisplayCodecInfo();
	iRecordCycleCounter = 1;

	}

//
// RStepA3FDevSoundRecordAndPlay
//

/*
 *
 * RStepA3FDevSoundRecordAndPlay - Test step constructor
 *
 */
RStepA3FDevSoundRecordAndPlay::RStepA3FDevSoundRecordAndPlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aSectName, aOutputDataType),
 iPlayState(EStatePlayStart),
 iInputDataType(aOutputDataType),
 iAsyncReadBFFAO(NULL),
 iAsyncReadBFF(ETrue),
 iStart(0),
 iFinished(0)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundRecordAndPlay* RStepA3FDevSoundRecordAndPlay::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundRecordAndPlay* self = new (ELeave) RStepA3FDevSoundRecordAndPlay(aTestName, aSectName, aOutputDataType);
	return self;

	}

/*
 *
 * CloseTest
 *
 */
void RStepA3FDevSoundRecordAndPlay::CloseTest()
	{
	if (iMMFDevSound)
		{
		INFO_PRINTF1(_L("--- Deleting DevSound object..."));
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}

	if (iRecordTimer)
		{
		delete iRecordTimer;
		iRecordTimer = NULL;
		}

	if (iAsyncWriteBTFAO)
		{
		delete iAsyncWriteBTFAO;
		iAsyncWriteBTFAO = NULL;
		}

	if (iAsyncReadBFFAO)
		{
		delete iAsyncReadBFFAO;
		iAsyncReadBFFAO = NULL;
		}

	iFile.Close();
	iFs.Close();

	}

/*
 *
 * InfoPrinter
 *
 */
void RStepA3FDevSoundRecordAndPlay::BufferInfoPrinter(TInt aActual, TInt aExpected)
	{
	INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), aActual, aExpected);

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundRecordAndPlay::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be recorded into and played back -> %S"), &iFilename);
	DisplayCodecInfo();

	iAsyncWriteBTF = ETrue;
	iAsyncReadBFFAO = CAsyncReadBufferFromFile::NewL(iFile, iMMFDevSound, *this);

	}

/*
 *
 * FsmPlayback
 * - Executes playing events of DevSound in sequence
 *
 */
void RStepA3FDevSoundRecordAndPlay::FsmPlayback(TDevSoundPlayEvents aDevSoundEvent, TInt aError)
	{
	switch (iPlayState)
		{
		case EStatePlayStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundRecordAndPlay"),EFsmIncorrectErrorPassed));
			INFO_PRINTF1(_L("Initializing DevSound for play back of recorded audio data..."));
			if (aDevSoundEvent == EEventPlayStart)
				{
				// Initializing DevSound object for playing mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL with EMMFStatePlaying"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
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
				// Initialize audio device and start the play process
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL()"));
				TRAPD(err, iMMFDevSound->PlayInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL() left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStatePlaying"));
				iPlayState = EStatePlaying;
				iStart.HomeTime();
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
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundRecordAndPlay"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBF)
				{
				// Fill the buffer with audio data and play the data in the buffer
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				if (iAsyncReadBFF)
					{
					// Async RFile::Read version
					iAsyncReadBFFAO->Start(buffer);
					}
				else
					{
					// Sync RFile::Read version
					TInt err = iFile.Read(buffer->Data());
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("RFile::Read failed with %d"), err);
						StopTest(err);
						break;
						}

					if (buffer->Data().Length() != buffer->RequestSize())
						{
						INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
						buffer->SetLastBuffer(ETrue);
						}
					// Playing data in the buffer at the current default volume
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * InitializeComplete
 *
 */
void RStepA3FDevSoundRecordAndPlay::InitializeComplete(TInt aError)
	{
	if (iState == EStateRecordInitializing)
		{
		Fsm(EEventInitCompleteRecord,aError);
		}
	else if (iPlayState == EStatePlayInitializing)
		{
		FsmPlayback(EEventInitCompletePlay,aError);
		}

	}

/*
 *
 * BufferToBeEmptied
 *
 */
void RStepA3FDevSoundRecordAndPlay::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
		StopTest(KErrGeneral);
		return;
		}

	iBuffer = aBuffer;
	INFO_PRINTF2(_L("DevSound called BufferToBeEmptied, with size = %d"), aBuffer->RequestSize());

	if(aBuffer->LastBuffer())
		{
		if(iState == EStateRecordPaused)
			{
			// We need to call CMMFDevSound::Stop() here if last buffer flag set
			INFO_PRINTF1(_L("In Paused state and have received last CMMFBuffer"));
			INFO_PRINTF1(_L("Calling CMMFDevSound::Stop()"));
			iMMFDevSound->Stop();
			DisplaySamplesAndBuffersRecorded(); // Called to measure performance
			iBufferCount = 0; // Initialise the number of buffers played to zero
			iFile.Close();
			// Open using RFile for playback
			TInt err = iFile.Open(iFs, iFilename, EFileRead);
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("Could not open record file for playback as it does not exist. Error = %d"), err);
				StopTest(err);
				return;
				}
			FsmPlayback(EEventPlayStart, KErrNone); // call to initialize DevSound for play back
			}
		else
			{
			INFO_PRINTF1(_L("***** Unknown behaviour: Last buffer flag set before calling CMMFDevSound::Pause()"));
			StopTest(KErrUnknown);
			}
		}
	else
		{
		Fsm(EEventBTBE,KErrNone);
		}

	}

/*
 *
 * BufferToBeFilled
 *
 */
void RStepA3FDevSoundRecordAndPlay::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		StopTest(KErrGeneral);
		return;
		}

	iBuffer = aBuffer;

	// Call to continue the Play process
	FsmPlayback(EEventBTBF,KErrNone);

	}

/*
 *
 * PlayError
 *
 */
void RStepA3FDevSoundRecordAndPlay::PlayError(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);

	if ((iBuffer->LastBuffer()) && (aError == KErrUnderflow))
		{
		iFinished.HomeTime();
		TTimeIntervalMicroSeconds playbackInterval;
		playbackInterval = iFinished.MicroSecondsFrom(iStart);
		INFO_PRINTF2(_L("Time elapsed during playback: %d mircoseconds"), I64INT(playbackInterval.Int64()));
		DisplaySamplesAndBuffersPlayed();
		StopTest();
		}
	else
		{
		StopTest(aError);
		}
	}

/*
 *
 * DisplaySamplesAndBuffersRecorded
 *
 */
void RStepA3FDevSoundRecordAndPlay::DisplaySamplesAndBuffersPlayed()
	{
	// Display the samples played
	TInt sampPlay = iMMFDevSound->SamplesPlayed();
	INFO_PRINTF2(_L("Number of Samples played = %d"), sampPlay);
	// Display the buffer count
	INFO_PRINTF2(_L("Number of Buffers played = %d"), iBufferCount);

	}

//
// RStepA3FDevSoundSetGainDuringRecord
//

/*
 *
 * RStepA3FDevSoundSetGainDuringRecord - Test step constructor
 *
 */
RStepA3FDevSoundSetGainDuringRecord::RStepA3FDevSoundSetGainDuringRecord(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aSectName, aOutputDataType),
 iGain(0),
 iCounter(2),
 iCheckGain(ETrue),
 iSetGain(ETrue)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundSetGainDuringRecord* RStepA3FDevSoundSetGainDuringRecord::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundSetGainDuringRecord* self = new (ELeave) RStepA3FDevSoundSetGainDuringRecord(aTestName, aSectName, aOutputDataType);
	return self;

	}

/*
 *
 * DoKickoffTestL
 *
 */
void RStepA3FDevSoundSetGainDuringRecord::DoKickoffTestL()
	{
	INFO_PRINTF2(_L("File to be recorded into -> %S"), &iFilename);
	DisplayCodecInfo();

	iGain = iMMFDevSound->MaxGain();
	INFO_PRINTF1(_L("Calling CMMFDevSound::SetGain to CMMFDevSound::MaxGain()"));
	iMMFDevSound->SetGain(iGain);

	}

/*
 *
 * BufferToBeEmptied
 *
 */
void RStepA3FDevSoundSetGainDuringRecord::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
		StopTest(KErrGeneral);
		return;
		}

	iBuffer = aBuffer;
	INFO_PRINTF2(_L("DevSound called BufferToBeEmptied, with size = %d"), aBuffer->BufferSize());

	if(aBuffer->LastBuffer())
		{
		if(iState == EStateRecordPaused)
			{
			// We need to call CMMFDevSound::Stop() here if last buffer flag set
			INFO_PRINTF1(_L("In Paused state and have received last CMMFBuffer"));
			INFO_PRINTF1(_L("Calling CMMFDevSound::Stop()"));
			iMMFDevSound->Stop();
			StopTest();
			}
		else
			{
			INFO_PRINTF1(_L("***** Unknown behaviour: Last buffer flag set before calling CMMFDevSound::Pause()"));
			StopTest(KErrUnknown);
			}
		}
	else
		{
		if(iCheckGain)
			{
			INFO_PRINTF1(_L("Calling CMMFDevSound::Gain()"));
			TInt gain = iMMFDevSound->Gain();
			if (gain == iGain)
				{
				INFO_PRINTF2(_L("Current gain level is as expected. Current gain: %d"), gain);
				}
			else
				{
				ERR_PRINTF2(_L("Current gain level is NOT as expected. Current gain: %d"), gain);
				ERR_PRINTF2(_L("Expected current gain value: %d"), iGain);
				StopTest(KErrUnknown);
				return;
				}
			iCounter--;
			if(!iCounter)
				{
				iCheckGain = EFalse;
				}
			}

		Fsm(EEventBTBE,KErrNone);
		}

	}

/*
 *
 * Fsm
 * - Executes recording events of DevSound in sequence
 *
 */
void RStepA3FDevSoundSetGainDuringRecord::Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateRecordStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundSetGainDuringRecord"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventRecordStart)
				{
				// Initializing DevSound object for recording mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecordInitializing"));
				iState = EStateRecordInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventRecordStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateRecordInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteRecord && aError == KErrNone)
				{
				// Initialize audio device and start the recording process
				INFO_PRINTF1(_L("Calling CMMFDevSound::RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::RecordInitL() left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecording"));
				iState = EStateRecording;
				StartRecordTimer();

				// Read the current microphone gain via CMMFDevSound::Gain and check for the expected gain set before recording
				User::After(500000); // wait an approximate time (half second) for actual recording process to start
				INFO_PRINTF1(_L("Calling CMMFDevSound::Gain()"));
				TInt gain = iMMFDevSound->Gain();
				if (gain == iGain)
					{
					INFO_PRINTF2(_L("Current gain level is as expected. Current gain: %d"), gain);
					iGain = iMMFDevSound->MaxGain()/2;
					INFO_PRINTF1(_L("Calling CMMFDevSound::SetGain to CMMFDevSound::MaxGain()/2"));
					iMMFDevSound->SetGain(iGain);
					}
				else
					{
					ERR_PRINTF2(_L("Current gain level is NOT as expected. Current gain: %d"), gain);
					ERR_PRINTF2(_L("Expected current gain value: %d"), iGain);
					StopTest(KErrUnknown);
					}
				}
			else if (aDevSoundEvent == EEventInitCompleteRecord && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteRecord not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateRecording:
			{
			if(iSetGain)
				{
				iGain = 0;
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetGain to 0"));
				iMMFDevSound->SetGain(iGain);
				iSetGain = EFalse;
				}

			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundSetGainDuringRecord"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file and then continue to record
				WriteBufferToFileAndRecord();
				}
			else if (aDevSoundEvent == EEventRecordTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::Pause()"));
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("State: EStateRecordPaused"));
				iState = EStateRecordPaused;
				INFO_PRINTF2(_L("-- CMMFDevSound::Pause() called after %d seconds..."), iRecordTime/KMicroSecsInOneSec);
				INFO_PRINTF1(_L("-- Recording is Paused, waiting for the last buffer callback..."));
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE or EEventRecordTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateRecordPaused:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundSetGainDuringRecord"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventBTBE)
				{
				// Write buffer to file and then continue to record
				WriteBufferToFileAndRecord();
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
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iState);
			StopTest(KErrGeneral);
			}
		}
	}

//
// RStepA3FDevSoundInvalidModeRecord
//

/*
 *
 * RStepA3FDevSoundInvalidModeRecord - Test step constructor
 *
 */
RStepA3FDevSoundInvalidModeRecord::RStepA3FDevSoundInvalidModeRecord(const TDesC& aTestName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aOutputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundInvalidModeRecord* RStepA3FDevSoundInvalidModeRecord::NewL(const TDesC& aTestName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundInvalidModeRecord* self = new (ELeave) RStepA3FDevSoundInvalidModeRecord(aTestName, aOutputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundInvalidModeRecord::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	Fsm(EEventRecordStart,KErrNone); // call to start the DevSound finite state machine for recording

	}

/*
 *
 * Fsm
 * - Executes recording events of DevSound in sequence
 *
 */
void RStepA3FDevSoundInvalidModeRecord::Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateRecordStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInvalidModeRecord"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventRecordStart)
				{
				// Initializing DevSound object for PLAYING (instead of recording) mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStatePlaying));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecordInitializing"));
				iState = EStateRecordInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventRecordStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateRecordInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteRecord && aError == KErrNone)
				{
				// Initialize audio device and start the recording process
				// Calling CMMFDevSound::RecordInitL() when DevSound initialized for play mode!
				INFO_PRINTF1(_L("Calling CMMFDevSound::RecordInitL() when DevSound initialized for play mode!"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err == KErrNotReady)
					{
					ERR_PRINTF2(_L("CMMFDevSound::RecordInitL() left as expected with error = %d"), err);
					StopTest(err,EPass);
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::RecordInitL did NOT leave with KErrNotReady as expected. Error = %d"), err);
					StopTest(KErrUnknown);
					}
				}
			else if (aDevSoundEvent == EEventInitCompleteRecord && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteRecord not received as expected. Received event: %d"), aDevSoundEvent);
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
// RStepA3FDevSoundInitializeWhileRecording
//

/*
 *
 * RStepA3FDevSoundInitializeWhileRecording - Test step constructor
 *
 */
RStepA3FDevSoundInitializeWhileRecording::RStepA3FDevSoundInitializeWhileRecording(const TDesC& aTestName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aOutputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundInitializeWhileRecording* RStepA3FDevSoundInitializeWhileRecording::NewL(const TDesC& aTestName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundInitializeWhileRecording* self = new (ELeave) RStepA3FDevSoundInitializeWhileRecording(aTestName, aOutputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundInitializeWhileRecording::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	Fsm(EEventRecordStart,KErrNone); // call to start the DevSound finite state machine for recording

	}

/*
 *
 * Fsm
 * - Executes recording events of DevSound in sequence
 *
 */
void RStepA3FDevSoundInitializeWhileRecording::Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateRecordStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInitializeWhileRecording"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventRecordStart)
				{
				// Initializing DevSound object for recording mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecordInitializing"));
				iState = EStateRecordInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventRecordStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateRecordInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteRecord && aError == KErrNone)
				{
				// Initialize audio device and start the recording process
				INFO_PRINTF1(_L("Calling CMMFDevSound::RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::RecordInitL() left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecording"));
				iState = EStateRecording;

				User::After(500000); // wait an approximate time (half second) for actual recording process to start
				// Calling CMMFDevSound::InitializeL again while recording!
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL called again while recording!"));
				TRAP(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
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
			else if (aDevSoundEvent == EEventInitCompleteRecord && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteRecord not received as expected. Received event: %d"), aDevSoundEvent);
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
// RStepA3FDevSoundCancelInitializeRecord
//

/*
 *
 * RStepA3FDevSoundCancelInitializeRecord - Test step constructor
 *
 */
RStepA3FDevSoundCancelInitializeRecord::RStepA3FDevSoundCancelInitializeRecord(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aSectName, aOutputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundCancelInitializeRecord* RStepA3FDevSoundCancelInitializeRecord::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundCancelInitializeRecord* self = new (ELeave) RStepA3FDevSoundCancelInitializeRecord(aTestName, aSectName, aOutputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundCancelInitializeRecord::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();
	iRecordTimer = CPeriodic::NewL(CActive::EPriorityHigh);

	Fsm(EEventRecordStart,KErrNone); // call to start the DevSound finite state machine for recording

	}
	
/*
 *
 * InitializeComplete
 *
 */
void RStepA3FDevSoundCancelInitializeRecord::InitializeComplete(TInt aError)
	{
	INFO_PRINTF1(_L("InitializeComplete callback was received. This is not the expected behaviour"));
	INFO_PRINTF1(_L("InitializeL was not cancelled"));
	StopTest(aError, EFail);
	}

/*
 *
 * StartRecordTimer
 *
 */
void RStepA3FDevSoundCancelInitializeRecord::StartRecordTimer()
	{
	// Start the record timer
	TCallBack callback (TimerCallback, this);
	iRecordTimer->Start(KMicroSecsInTenSecs, 0, callback);
	}
	
/*
 *
 * DoTimerCallback
 *
 */
void RStepA3FDevSoundCancelInitializeRecord::DoTimerCallback()
	{
	iRecordTimer->Cancel(); // only really wanted a one-shot
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

	INFO_PRINTF2(_L("Sample rate set: %S"),&iSampleRate);
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
 *	- Executes Recording events of DevSound in sequence
 *
 */
void RStepA3FDevSoundCancelInitializeRecord::Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateRecordStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundCancelInitializeRecord"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventRecordStart)
				{
				// Initializing DevSound object for Recording mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					INFO_PRINTF1(_L("State: EStateRecordInitializing"));
					iState = EStateRecordInitializing;					
					INFO_PRINTF1(_L("Calling CMMFDevSound::CancelInitialize()"));
					iMMFDevSound->CancelInitialize();
					}
					INFO_PRINTF1(_L("Start timer to wait for InitializeComplete"));
					StartRecordTimer();
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventRecordStart not received as expected. Received event: %d"), aDevSoundEvent);
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
// RStepA3FDevSoundEmptyBuffersInitializedToRecord
//

/*
 *
 * RStepA3FDevSoundEmptyBuffersInitializedToRecord - Test step constructor
 *
 */
RStepA3FDevSoundEmptyBuffersInitializedToRecord::RStepA3FDevSoundEmptyBuffersInitializedToRecord(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aSectName, aOutputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundEmptyBuffersInitializedToRecord* RStepA3FDevSoundEmptyBuffersInitializedToRecord::NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundEmptyBuffersInitializedToRecord* self = new (ELeave) RStepA3FDevSoundEmptyBuffersInitializedToRecord(aTestName, aSectName, aOutputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundEmptyBuffersInitializedToRecord::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	Fsm(EEventRecordStart,KErrNone); // call to start the DevSound finite state machine for recording

	}

/*
 *
 * Fsm
 * - Executes recording events of DevSound in sequence
 *
 */
void RStepA3FDevSoundEmptyBuffersInitializedToRecord::Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateRecordStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundInitializeWhileRecording"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventRecordStart)
				{
				// Initializing DevSound object for recording mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecordInitializing"));
				iState = EStateRecordInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventRecordStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateRecordInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteRecord && aError == KErrNone)
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
			else if (aDevSoundEvent == EEventInitCompleteRecord && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteRecord not received as expected. Received event: %d"), aDevSoundEvent);
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
// RStepA3FDevSoundEmptyBuffersWhileRecording
//

/*
 *
 * RStepA3FDevSoundInitializeWhileRecording - Test step constructor
 *
 */
RStepA3FDevSoundEmptyBuffersWhileRecording::RStepA3FDevSoundEmptyBuffersWhileRecording(const TDesC& aTestName, const TFourCC& aOutputDataType)
:RTestStepA3FDevSoundRecordBase(aTestName, aOutputDataType)
	{
	}

/*
 *
 * NewL
 *
 */
RStepA3FDevSoundEmptyBuffersWhileRecording* RStepA3FDevSoundEmptyBuffersWhileRecording::NewL(const TDesC& aTestName, const TFourCC& aOutputDataType)
	{
	RStepA3FDevSoundEmptyBuffersWhileRecording* self = new (ELeave) RStepA3FDevSoundEmptyBuffersWhileRecording(aTestName, aOutputDataType);
	return self;

	}

/*
 *
 * KickoffTestL
 * - Starts the test
 *
 */
void RStepA3FDevSoundEmptyBuffersWhileRecording::KickoffTestL()
	{
	// Create instance of CMMFDevSound
	INFO_PRINTF1(_L("--- Creating DevSound object..."));
	iMMFDevSound = CMMFDevSound::NewL();

	Fsm(EEventRecordStart,KErrNone); // call to start the DevSound finite state machine for recording

	}

/*
 *
 * Fsm
 * - Executes recording events of DevSound in sequence
 *
 */
void RStepA3FDevSoundEmptyBuffersWhileRecording::Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError)
	{
	switch (iState)
		{
		case EStateRecordStart:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RStepA3FDevSoundEmptyBuffersWhileRecording"),EFsmIncorrectErrorPassed));
			if (aDevSoundEvent == EEventRecordStart)
				{
				// Initializing DevSound object for recording mode to process audio data via desired FourCC code
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecordInitializing"));
				iState = EStateRecordInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventRecordStart not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}

			break;

			}

		case EStateRecordInitializing:
			{
			if (aDevSoundEvent == EEventInitCompleteRecord && aError == KErrNone)
				{
				// Initialize audio device and start the recording process
				INFO_PRINTF1(_L("Calling CMMFDevSound::RecordInitL()"));
				TRAPD(err, iMMFDevSound->RecordInitL());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::RecordInitL() left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("State: EStateRecording"));
				iState = EStateRecording;

				User::After(500000); // wait an approximate time (half second) for actual recording process to start
				// Calling CMMFDevSound::EmptyBuffers while recording!
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL called again while recording!"));
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
			else if (aDevSoundEvent == EEventInitCompleteRecord && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitCompleteRecord not received as expected. Received event: %d"), aDevSoundEvent);
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
// CAsyncWriteBufferToFile
//

CAsyncWriteBufferToFile::CAsyncWriteBufferToFile(RFile& aFile, CMMFDevSound* aDevSound, RTestStepA3FDevSoundRecordBase& aTestStep)
:CActive(EPriorityStandard),
 iFile(aFile),
 iDevSound(aDevSound),
 iTestStep(aTestStep)
	{
	CActiveScheduler::Add(this);
	}

CAsyncWriteBufferToFile::~CAsyncWriteBufferToFile()
	{
	Cancel();
	}

CAsyncWriteBufferToFile* CAsyncWriteBufferToFile::NewL(RFile& aFile, CMMFDevSound* aDevSound, RTestStepA3FDevSoundRecordBase& aTestStep)
	{
	CAsyncWriteBufferToFile* self = new(ELeave) CAsyncWriteBufferToFile(aFile, aDevSound, aTestStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAsyncWriteBufferToFile::ConstructL()
	{
	// Nothing to do here
	}

void CAsyncWriteBufferToFile::RunL()
	{
	//If error occurs then deal with problem in RunError()
	User::LeaveIfError(iStatus.Int());
	// Continue recording data to a buffer
	iDevSound->RecordData();

	}

void CAsyncWriteBufferToFile::Start(CMMFDataBuffer* aBuffer)
	{
	iFile.Write(aBuffer->Data(), iStatus);
	SetActive();
	}

void CAsyncWriteBufferToFile::DoCancel()
	{
	// Can't cancel an async write request
	}

TInt CAsyncWriteBufferToFile::RunError(TInt aError)
	{
	iTestStep.CallStopTest(aError);
	return KErrNone;
	}

//
// CAsyncReadBufferFromFile
//

CAsyncReadBufferFromFile::CAsyncReadBufferFromFile(RFile& aFile, CMMFDevSound* aDevSound, RStepA3FDevSoundRecordAndPlay& aTestStep)
:CActive(EPriorityStandard),
 iFile(aFile),
 iDevSound(aDevSound),
 iTestStep(aTestStep)
	{
	CActiveScheduler::Add(this);
	}

CAsyncReadBufferFromFile::~CAsyncReadBufferFromFile()
	{
	Cancel();
	}

CAsyncReadBufferFromFile* CAsyncReadBufferFromFile::NewL(RFile& aFile, CMMFDevSound* aDevSound, RStepA3FDevSoundRecordAndPlay& aTestStep)
	{
	CAsyncReadBufferFromFile* self = new(ELeave) CAsyncReadBufferFromFile(aFile, aDevSound, aTestStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAsyncReadBufferFromFile::ConstructL()
	{
	// Nothing to do here
	}

void CAsyncReadBufferFromFile::RunL()
	{
	//If error occurs then deal with problem in RunError()
	User::LeaveIfError(iStatus.Int());

	if (iBuffer->Data().Length() != iBuffer->RequestSize())
		{
		iTestStep.BufferInfoPrinter(iBuffer->Data().Length(), iBuffer->RequestSize());
		iBuffer->SetLastBuffer(ETrue);
		}
	// Playing data in the buffer at the current default volume
	iDevSound->PlayData();

	}

void CAsyncReadBufferFromFile::Start(CMMFDataBuffer* aBuffer)
	{
	iFile.Read(aBuffer->Data(), iStatus);
	SetActive();
	iBuffer = aBuffer;
	}

void CAsyncReadBufferFromFile::DoCancel()
	{
	iFile.ReadCancel();
	}

TInt CAsyncReadBufferFromFile::RunError(TInt aError)
	{
	iTestStep.CallStopTest(aError);
	return KErrNone;
	}
