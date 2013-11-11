// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>

#include "TSI_MMF_A2DP_DS.h"

#include <mmfHwDeviceImplementationUids.hrh>

const TInt KHeapSizeToneTestEKA2 = 128000;	// heapsize for tone tests on EKA2


/**
 *
 * InitializeDevSound (route to A2DP interface)
 *
 */
TVerdict RTestStepA2dp::InitializeDevSound(TMMFState aMode, TBool aHeadset)
	{
	INFO_PRINTF1(_L("Initializing DevSound"));

	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	if (aHeadset)
		{
		INFO_PRINTF1(_L("Routing the sound to A2DP interface"));
		pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
		}
	else
		{
		INFO_PRINTF1(_L("Routing the sound to speaker"));
		}
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;
	
	// To use SetPrioritySettings API to choose the A2DP headset as the route of the audio 
	iMMFDevSound->SetPrioritySettings(prioritySettings);

	// Initialize DevSound to A2DP headset
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		iAsyncRequestHandler->HandleAsyncRequest();	
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * DevSoundPlayInit (route to A2DP interface)
 *
 */
TVerdict RTestStepA2dp::DevSoundPlayInit()
	{
	INFO_PRINTF1(_L("PlayInit begins"));

	//Set the capability to 32kHz and Mono
	//Cannot change the default config in the BlueAnt headset
	//So it can only support 32Khz and mono by default
	INFO_PRINTF1(_L("Set the capability to 32Khz and Mono"));
	TMMFCapabilities	capbilitySetting;
	capbilitySetting.iRate = EMMFSampleRate32000Hz;
	capbilitySetting.iChannels = EMMFMono;
	TRAPD(err, iMMFDevSound->SetConfigL(capbilitySetting));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound SetConfigL left with error = %d"), err);
		return EInconclusive;
		}

	INFO_PRINTF1(_L("Run PlayInitL"));
	TRAP(err, iMMFDevSound->PlayInitL());
 	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayInitL left with error = %d"), err);
		return EFail;
		}
	else
		{
		iAsyncRequestHandler->HandleAsyncRequest();	
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound PlayInit CallBackError returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * DevSoundPlayData
 *
 */
TInt RTestStepA2dp::DevSoundPlayData()
	{
	ResetCallbacks();
	
	iMMFDevSound->PlayData();
	iAsyncRequestHandler->HandleAsyncRequest();	

	if (iCallbackArray[EBuffToFill] != 1)
		{
		if (iCallbackArray[EBuffToFill] == 0 && iCallbackArray[EPlayError] == 1)
			{
			INFO_PRINTF1(_L("DevSound PlayError was called 1 time.  Must be EOF."));
			return KErrCompletion;
			}
		else
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeFilled was called %d times, expected 1"), iCallbackArray[EBuffToFill]);
			return KErrGeneral;
			}
		}
	return KErrNone;
	}


/**
 *
 * RTestStepA2dpInitializeHeadset
 *
 */
RTestStepA2dpInitializeHeadset::RTestStepA2dpInitializeHeadset(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpInitializeHeadset::DoTestStepL()
	{
	INFO_PRINTF1(_L("Testing A2DP Initialization"));

	TVerdict res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("Initialization fails"));
		return EFail;
		}
	else
		{
		return res;
		}
	}


/**
 *
 * RTestStepA2dpReinitialDisconnectHeadset
 *
 */
RTestStepA2dpReinitialDisconnectHeadset::RTestStepA2dpReinitialDisconnectHeadset(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpReinitialDisconnectHeadset::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test A2DP: re-initialize a disconnected headset"));

	TVerdict res;
	res = EFail;

	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("First Initialization fails"));
		return res;
		}
	else
		{
		//Disconnect the BT link
		INFO_PRINTF1(_L("Initialization succeeds, now disconnecting the link"));
		iBtPhysicalLink->Disconnect(*iBTheadsetAddress);
		iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeToRegisterLinkLost));
		CActiveScheduler::Current()->Start();
		}
		
	if (!iBTLinkDisconnected) 
		{
		INFO_PRINTF1(_L("WARNING: BT link failed to disconnect" ));
		res = EInconclusive;
		return res;
		}
	
	INFO_PRINTF1(_L("Now, reinitialize the headset"));	
	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res  != EPass)
		{
		INFO_PRINTF1(_L("Re-Initialization fails"));
		return EFail;
		}
	else
		{
		return res;
		}
	}	

/**
 *
 * RTestStepA2dpReinitializeHeadset
 *
 */
RTestStepA2dpReinitializeHeadset::RTestStepA2dpReinitializeHeadset(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpReinitializeHeadset::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test A2DP: re-initialize a connected headset"));

	TVerdict res;
	res = EFail;
	
	//Count the required time to initialize the DevSound for the first time
	TTime startTime1;
	startTime1.HomeTime();
	
	INFO_PRINTF1(_L("Initialize the headset for the first time"));
	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("First Initialization fails"));
		return res;
		}

	TTime endTime1;
	endTime1.HomeTime();
	
	TInt64 duration1 = I64INT(endTime1.MicroSecondsFrom(startTime1).Int64());
	INFO_PRINTF3(_L("Start time = %d End time = %d"), I64INT(startTime1.Int64()), I64INT(endTime1.Int64()));
	INFO_PRINTF2(_L("First initialization duration = %d"), duration1);  	


	//Count the required time to initialize the DevSound for the second time
	TTime startTime2;
	startTime2.HomeTime();
	
	INFO_PRINTF1(_L("Reinitialize the headset"));
	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("Re-Initialization fails"));
		return EFail;
		}
	
	TTime endTime2;
	endTime2.HomeTime();
	
	TInt64 duration2 = I64INT(endTime2.MicroSecondsFrom(startTime2).Int64());
	INFO_PRINTF3(_L("Start time = %d End time = %d"), I64INT(startTime2.Int64()), I64INT(endTime2.Int64()));
	INFO_PRINTF2(_L("Re-Initialization duration = %d"), duration2);  	

	if (duration2 <= duration1)
		{
		INFO_PRINTF1(_L("2nd initialization duration is shorter."));
		res = EPass;
		}
	else
		{
		INFO_PRINTF1(_L("2nd initialization duration should not be longer."));
		return EFail;
		}		
		
	return res;
	}


/**
 *
 * RTestStepA2dpCapabilities
 *
 */
RTestStepA2dpCapabilities::RTestStepA2dpCapabilities(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}
		
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpCapabilities::DoTestStepL()
	{	
	INFO_PRINTF1(_L("Test Capabilities (Sample rates and Channels) of the A2dp headset"));

	TVerdict res;
	res = EFail;

	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("Initialization fails"));
		return res;
		}
	
	TMMFCapabilities caps = iMMFDevSound->Capabilities();
	//44100 & 48000 sample rates are mandatory for A2DP SBC devices so check
	//However, the Blueant headset we use doesn't support 48000Hz, so we temporaily remove this from the test
	iExpectedValue = (EMMFSampleRate44100Hz/* | EMMFSampleRate48000Hz*/);
	if ((caps.iRate & iExpectedValue) != (TUint)iExpectedValue)
		{
		ERR_PRINTF3 (_L("DevSound Capabilities Supported Sample Rates returned %d, expected %d"), caps.iRate, iExpectedValue);
		return EFail;
		}

	iExpectedValue = (EMMFMono | EMMFStereo);
	if (caps.iChannels != (TUint)iExpectedValue)
		{
		ERR_PRINTF3 (_L("DevSound Capabilities Supported Channels returned %d, expected %d"), caps.iChannels, iExpectedValue);
		return EFail;
		}
	
	 return EPass;
	}
	
	

/**
 *
 * RTestStepA2dpPlayInit
 *
 */
RTestStepA2dpPlayInit::RTestStepA2dpPlayInit(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpPlayInit::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test PlayInit with DevSound routing to A2DP"));
		
	TVerdict res;
	res = EFail;

	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("Initialization fails"));
		return res;
		}

	res = DevSoundPlayInit();
	if (res != EPass)
		{
		INFO_PRINTF1(_L("PlayInit fails"));
		return EFail;
		}
	
	
	iMMFDevSound->Stop();
	
	return res;
	}
	

/**
 *
 * RTestStepA2dpPlayEofPcm16
 *
 */
RTestStepA2dpPlayEofPcm16::RTestStepA2dpPlayEofPcm16(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpPlayEofPcm16::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test Play EOF PCM16 with DevSound routing to A2DP"));

	TVerdict res;
	res = EFail;

	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("Initialization fails"));
		return res;
		}

	res = DevSoundPlayInit();
	if (res != EPass)
		{
		INFO_PRINTF1(_L("PlayInit fails"));
		return res;
		}

	//Connect to the File System
	RFs fs;
	INFO_PRINTF1(_L("Connect to the File system"));
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not connect to Filesystem. Error is %d"), err);
		return EInconclusive;
		}

	// Open the input the file
	TFileName filename16(KInputTestFileNamePCM16);
	RFile file;

	INFO_PRINTF1(_L("Open the input file"));

	//Opens an existing file for reading or writing
	err = file.Open(fs, filename16, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not open input file. Error is %d"), err);
		fs.Close();
		return EInconclusive;
		}

	TInt bufferCount = 1;
	TInt playDataErr = KErrNone;
	while (playDataErr == KErrNone && iCallbackError == KErrNone)
		{
		CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
		
		//read sizeof buffer from file
		file.Read(buffer->Data());
		
		if (buffer->Data().Length()!= buffer->RequestSize())
			{
			INFO_PRINTF3(_L("Data length copied from file = %d. Expected %d. Must be EOF"), buffer->Data().Length(), buffer->RequestSize());
			//Sets the buffer as the last buffer
			iBuffer->SetLastBuffer(ETrue);
			}

		//DevSound Play
		INFO_PRINTF2(_L("To play the data buffer: %d time" ), bufferCount);
		playDataErr = DevSoundPlayData();
		
		if(playDataErr == KErrGeneral) 
			{
			INFO_PRINTF1(_L("Error happens during PlayData"));
			file.Close();
			fs.Close();
			return EFail;
			}
		bufferCount ++;
		} //while loop

	file.Close();
	fs.Close();

	return EPass;
	}


/**
 *
 * RTestStepA2dpPlayEofPcm8
 *
 */
RTestStepA2dpPlayEofPcm8::RTestStepA2dpPlayEofPcm8(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpPlayEofPcm8::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test Play EOF PCM8 with DevSound routing to A2DP"));

	TVerdict res;
	res = EFail;

	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("Initialization fails"));
		return res;
		}

	res = DevSoundPlayInit();
	if (res != EPass)
		{
		INFO_PRINTF1(_L("PlayInit fails"));
		return res;
		}

	//Connect to the File System
	RFs fs;
	INFO_PRINTF1(_L("Connect to the File system"));
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not connect to Filesystem. Error is %d"), err);
		return EInconclusive;
		}

	// Open the input the file
	TFileName filename8(KInputTestFileNamePCM8);
	RFile file;

	INFO_PRINTF1(_L("Open the input file"));

	//Opens an existing file for reading or writing
	err = file.Open(fs, filename8, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not open input file. Error is %d"), err);
		fs.Close();
		return EInconclusive;
		}

	TInt bufferCount = 1;
	TInt playDataErr = KErrNone;
	while (playDataErr == KErrNone && iCallbackError == KErrNone)
		{
		CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
		
		//read sizeof buffer from file 
		file.Read(buffer->Data());

		if (buffer->Data().Length()!= buffer->RequestSize())
			{
			INFO_PRINTF3(_L("Data length copied from file = %d. Expected %d. Must be EOF"), buffer->Data().Length(), buffer->RequestSize());
			//Sets the buffer as the last buffer
			iBuffer->SetLastBuffer(ETrue);
			}

		//DevSound Play
		INFO_PRINTF2(_L("To play the data buffer: %d time" ), bufferCount);
		playDataErr = DevSoundPlayData();
		
		if(playDataErr == KErrGeneral) 
			{
			INFO_PRINTF1(_L("Error happens during PlayData"));
			file.Close();
			fs.Close();
			return EFail;
			}
		bufferCount ++;
		}

	file.Close();
	fs.Close();

	return EPass;
	}


/**
 *
 * RTestStepA2dpDisconnectPlay
 *
 */
RTestStepA2dpDisconnectPlay::RTestStepA2dpDisconnectPlay(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpDisconnectPlay::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test - Disconnect headset when playing a file"));

	TVerdict res;
	res = EFail;

	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("Initialization fails"));
		return res;
		}

	res = DevSoundPlayInit();
	if (res != EPass)
		{
		INFO_PRINTF1(_L("PlayInit fails"));
		return res;
		}

	//Connect to the File System
	RFs fs;
	INFO_PRINTF1(_L("Connect to the File system"));
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not connect to Filesystem. Error is %d"), err);
		return EInconclusive;
		}

	// Open the input the file
	TFileName filename8(KInputTestFileNamePCM8);
	RFile file;

	INFO_PRINTF1(_L("Open the input file"));

	//Opens an existing file for reading or writing
	err = file.Open(fs, filename8, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not open input file. Error is %d"), err);
		fs.Close();
		return EInconclusive;
		}

	CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);	
	//read sizeof buffer from file 
	file.Read(buffer->Data());

	if (buffer->Data().Length()!= buffer->RequestSize())
		{
		INFO_PRINTF3(_L("Data length copied from file = %d. Expected %d. Must be EOF"), buffer->Data().Length(), buffer->RequestSize());
		//Sets the buffer as the last buffer
		iBuffer->SetLastBuffer(ETrue);
		}

	//DevSound Play
	INFO_PRINTF1(_L("To play the data buffer"));
	TInt playDataErr = DevSoundPlayData();
	
	//Disconnect BT link
	INFO_PRINTF1(_L("Disconnecting the BT link for the next test"));
	iBtPhysicalLink->Disconnect(*iBTheadsetAddress);
	iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeToRegisterLinkLost));
	CActiveScheduler::Current()->Start();
	
	if(playDataErr == KErrGeneral) 
		{
		INFO_PRINTF1(_L("Error happens during PlayData"));
		file.Close();
		fs.Close();
		return EFail;
		}

	if (!iBTLinkDisconnected) 
		{
		INFO_PRINTF1(_L("Error: BT link failed to disconnect" ));
		file.Close();
		fs.Close();
		return EFail;
		}

	file.Close();
	fs.Close();

	return EPass;
	}


/**
 *
 * RTestStepA2dpPlaySimpleTone
 *
 */
RTestStepA2dpPlaySimpleTone::RTestStepA2dpPlaySimpleTone(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	
	// need to increase heap size on EKA2 HW
#if !defined __WINS__
		iHeapSize = KHeapSizeToneTestEKA2;
#endif // EKA2
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpPlaySimpleTone::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test PlayTone with DevSound routing to A2DP"));
	
	TVerdict res;
	res = EFail;
	
	TInt freq = 100;
	TTimeIntervalMicroSeconds dur(2000000);

	res = InitializeDevSound(EMMFStateTonePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("Initialization fails"));
		return res;
		}
	
	//To Play Tone
	iExpectedValue = KErrUnderflow;	
	INFO_PRINTF1(_L("Playing Simple Tone"));
	TRAPD(err, iMMFDevSound->PlayToneL(freq, dur));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayToneL left with error = %d"), err);
		iCallbackError = err;
		return EFail;
		}
	else
		{
		iAsyncRequestHandler->HandleAsyncRequest();	
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound PlayTone CallBackError returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		}
	return EPass;
	}


/**
 *
 * RTestStepA2dpTwoThread
 *
 */
RTestStepA2dpTwoThread::RTestStepA2dpTwoThread(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpTwoThread::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test: Two DevSound instances(two threads) to initialize to BT headset"));
	
	TVerdict res;
	res = EFail;
	
//First DevSound to initialize
	INFO_PRINTF1(_L("First DevSound to initialize(1st thread)"));
	res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res != EPass)
		{
		INFO_PRINTF1(_L("First DevSound Initialization fails"));
		return res;
		}

//Second DevSound to initialize
	INFO_PRINTF1(_L("Creating the second DevSound instance(2nd thread)"));
	CMMFDevSound* mmfDevSound2 = CMMFDevSound::NewL();
		
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	
	TMMFPrioritySettings	prioritySettings2;
	prioritySettings2.iPref = EMdaPriorityPreferenceNone;
	TInt pref2 = static_cast<TInt>(prioritySettings2.iPref);
	pref2 |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref2);
	prioritySettings2.iPref = mdapref;
	
	INFO_PRINTF1(_L("Set the audio to route to A2DP interface"));
	mmfDevSound2->SetPrioritySettings(prioritySettings2);

	// Initialize DevSound to A2DP headset	
	INFO_PRINTF1(_L("Initialize the 2nd DevSound instance with the BT headset"));
	TRAPD(err, mmfDevSound2->InitializeL(*this, EMMFStatePlaying));
	if (err)
		{
		WARN_PRINTF2 (_L("2nd DevSound InitializeL left with error = %d"), err);
		delete mmfDevSound2;
		return EFail;
		}
	else
		{
		iAsyncRequestHandler->HandleAsyncRequest();	
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("2nd DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			delete mmfDevSound2;
			return EFail;
			}
		}
		
	delete mmfDevSound2;
	return EPass;
	}


/**
 *
 * RTestStepA2dpInitializeHeadsetNeg
 *
 */
RTestStepA2dpInitializeHeadsetNeg::RTestStepA2dpInitializeHeadsetNeg(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpInitializeHeadsetNeg::DoTestStepL()
	{
	INFO_PRINTF1(_L("Testing A2DP Initialization"));

	TVerdict res = InitializeDevSound(EMMFStatePlaying, ETrue);
	if (res == EPass)
		{
		INFO_PRINTF1(_L("Initialization success, but it is not expected"));
		return EFail;
		}
	else if (iCallbackError == KErrCouldNotConnect)
		{
		INFO_PRINTF1(_L("Return -34. This is expected"));
		return EPass;
		}
	else
		{
		INFO_PRINTF2(_L("Unexpected error %d"), iCallbackError);
		return EFail;
		}
	}




