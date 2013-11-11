    // Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

// System includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_DEVSOUND_STEP.h"
#include "TSU_MMF_DEVSOUND_SUITE.h"
 
#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include <mmf/server/mmfswcodecwrapper.h>

#include "../a3fcistubextn/testsetvol.h"

//const TInt KBufferLength = 0x1000; // default buffer length for tests
//const TInt KMaxBufferLength = 0x100000;		// max sound driver buffer size
//const TInt KMaxBufferLengthEKA2 = 0xBB800;	// max EKA2 sound driver buffer size
#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
    const TInt KMaxBufferLengthEKA2Lubbock = 0x4000;	// max EKA2 sound driver buffer size on HW
#else
    const TInt KMaxBufferLengthEKA2Lubbock = 0x8000;
#endif

const TInt KHeapSizeToneTestEKA2 = 128000;	// heapsize for tone tests on EKA2

//Test directory/files:
_LIT(KInputTestFileNamePCM16, "C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
_LIT(KInputTestFileNamePCM8, "C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
_LIT(KInputTestFileNamePCM8_16_s, "C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_s.raw");
_LIT(KInputTestFileNamePCM16_16_m, "C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_16_16_m.raw");

_LIT(KOutputDir, "C:\\TSU_MMF_DEVSOUND_SUITE\\Output\\");
_LIT(KOutputTestFileName1BufPCM16, "C:\\TSU_MMF_DEVSOUND_SUITE\\Output\\Rec1BufferPCM.raw");
_LIT(KOutputTestFileName10BufPCM16, "C:\\TSU_MMF_DEVSOUND_SUITE\\Output\\Rec10BufferPCM.raw");
_LIT(KOutputTestFileName1BufPCM8, "C:\\TSU_MMF_DEVSOUND_SUITE\\Output\\Rec1BufferPCM8.raw");
_LIT(KOutputTestFileName10BufPCM8, "C:\\TSU_MMF_DEVSOUND_SUITE\\Output\\Rec10BufferPCM8.raw");

_LIT(KMsgDeleteDevsound, "---- Deleting DevSound object ----");
_LIT(KMsgErrorDevSoundCallback,"DevSound called %S() callback with error = %d");
_LIT(KInitializeCompleteText, "InitializeComplete");
_LIT(KMsgErrorGetParameter,"Error in getting parameter %S from INI file");

_LIT(KVolumeRamp, "VolumeRamp");
_LIT(KDuration, "Duration");
_LIT(KFrequencyTone1, "FrequencyTone1");
_LIT(KFrequencyTone2, "FrequencyTone2");
_LIT(KDTMFString, "DTMFString");
_LIT(KVolume, "Volume");

const TInt KLowToneFrequency = 440;
const TInt KHighToneFrequency = 880;
const TInt KTonePlayDuration = 2000000; // 2 seconds
const TInt KRecordDuration = 2000000; // 2 seconds
const TInt KPlayDuration = 1000000; // 1 seconds

const TInt KMicroSecsInOneSec = 1000000;
const TInt KMicroSecsTwoSec = 2000000;
// To compute elapsed time for some timed test cases
const TInt64 KPlayVarianceTime = 500000; // 0.5 seconds
const TInt KDefaultOnDTMFLength = 250000;
const TInt KDefaultOffDTMFLength = 50000;
const TInt KDefaultPauseDTMFLength = 250000;

/**
 *
 * CTestStepDevSound
 * Test step constructor
 *
 */
CTestStepDevSound::CTestStepDevSound()
	{
	iExpectedValue = KErrNone;
	}

/**
 *
 * ~CTestStepDevSound
 *	 Test step destructor
 *
 */
CTestStepDevSound::~CTestStepDevSound()
	{
	}

/**
 *
 * SetTestSuite
 * @param aTestSuite
 *
 */
void CTestStepDevSound::SetTestSuite(const CTestSuiteDevSound* aTestSuite)
	{
	iDevSoundTestSuite = aTestSuite;
	}

/**
 *
 * DoTestStepPreambleL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::DoTestStepPreambleL()
	{
	//Install the Active Scheduler
	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);

	iAL= new(ELeave)CActiveListener;

	INFO_PRINTF1(_L("Creating DevSound Object"));
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

/**
 *
 * DoTestStepPostambleL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::DoTestStepPostambleL()
	{
	INFO_PRINTF1(_L("Deleting DevSound Object"));
	delete iMMFDevSound;
	iMMFDevSound = NULL;
	delete iAL;
	iAL = NULL;
	delete iAS;
	iAS = NULL;
	return EPass;
	}

/**
 *
 * InitializeComplete
 * @param aError
 *
 */
void CTestStepDevSound::InitializeComplete (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called InitializeComplete with aError = %d"), aError);
	TRequestStatus* stat = &(iAL->iStatus);
	User::RequestComplete(stat, aError);
	iCallbackArray[EInitComplete]++;
	iCallbackError = aError;
	}

/**
 *
 * ToneFinished
 * @param aError
 *
 */
void CTestStepDevSound::ToneFinished (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called ToneFinished with aError = %d"), aError);
	TRequestStatus* stat = &(iAL->iStatus);
	User::RequestComplete(stat, aError);
	iCallbackArray[EToneFinished]++;
	iCallbackError = aError;
	}

/**
 *
 * BufferToBeFilled
 * @param aBuffer
 *
 */
void CTestStepDevSound::BufferToBeFilled (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	TRequestStatus* stat = &(iAL->iStatus);
	if (aBuffer != NULL)
		{
		User::RequestComplete(stat, KErrNone);
		iCallbackError = KErrNone;
		}
	else
		{
		User::RequestComplete(stat, KErrNotFound);
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToFill]++;
	}

/**
 *
 * PlayError
 * @param aError
 *
 */
void CTestStepDevSound::PlayError (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called PlayError with aError = %d"), aError);
	TRequestStatus* stat = &(iAL->iStatus);
	User::RequestComplete (stat, aError);
	iCallbackError = aError;
	iCallbackArray[EPlayError]++;
	}

/**
 *
 * BufferToBeEmptied
 * @param aBuffer
 *
 */
void CTestStepDevSound::BufferToBeEmptied (CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("DevSound called BufferToBeEmptied"));
	iBuffer = aBuffer;
	TRequestStatus* stat = &(iAL->iStatus);
	if (aBuffer != NULL)
		{
		User::RequestComplete(stat, KErrNone);
		iCallbackError = KErrNone;
		}
	else
		{
		User::RequestComplete(stat, KErrNotFound);
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToEmpty]++;
	}

/**
 *
 * RecordError
 * @param aError
 *
 */
void CTestStepDevSound::RecordError (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called RecordError with aError = %d"), aError);
	TRequestStatus* stat = &(iAL->iStatus);
	User::RequestComplete (stat, aError);
	iCallbackError = aError;
	iCallbackArray[ERecError]++;
	}

/**
 *
 * ConvertError
 *
 */
void CTestStepDevSound::ConvertError (TInt /*aError*/)
	{}

/**
 *
 * DeviceMessage
 *
 */
void CTestStepDevSound::DeviceMessage (TUid /*aHwDeviceUid*/, const TDesC8& /*aMsg*/)
	{}


/**
 *
 * TestInitialize
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestInitialize(TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	INFO_PRINTF2(_L("Initializing DevSound aMode=%d"), aMode);

	ResetCallbacks();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		iAL->InitialiseActiveListener();
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			ERR_PRINTF2 (_L("DevSound InitializeComplete was called %d times, expected 1"), iCallbackArray[EInitComplete]);
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aHwDeviceUid
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestInitialize(TUid aHwDeviceUid, TMMFState aMode)
	{
	iCallbackError = KErrNone;

	INFO_PRINTF3(_L("Initializing DevSound aHwDeviceUid=0x%x aMode=%d"), aHwDeviceUid, aMode);

	ResetCallbacks();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aHwDeviceUid, aMode));
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		if (err != KErrNotSupported)
			{
			WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
			return EFail;
			}
		INFO_PRINTF2 (_L("DevSound InitializeL left with expected error = %d"), err);
		return EPass;
	#else
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		iAL->InitialiseActiveListener();
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			ERR_PRINTF2 (_L("DevSound InitializeComplete was called %d times, expected 1"), iCallbackArray[EInitComplete]);
			return EFail;
			}
		}
	return EPass;
	#endif
	}


/**
 *
 * TestInitialize
 * @param aDataType
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestInitialize(TFourCC& aDataType, TMMFState aMode)
	{
	iCallbackError = KErrNone;

	INFO_PRINTF3(_L("Initializing DevSound aDataType=0x%x aMode=%d"), aDataType.FourCC(), aMode);

	ResetCallbacks();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aDataType, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		iAL->InitialiseActiveListener();
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			ERR_PRINTF2 (_L("DevSound InitializeComplete was called %d times, expected 1"), iCallbackArray[EInitComplete]);
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestCaps
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestCaps()
	{
	INFO_PRINTF1(_L("Checking Capabilities"));
	TMMFCapabilities caps = iMMFDevSound->Capabilities();
	iExpectedValue = (EMMFSampleRate8000Hz |
						EMMFSampleRate11025Hz |
						EMMFSampleRate16000Hz |
						EMMFSampleRate22050Hz |
						EMMFSampleRate32000Hz |
						EMMFSampleRate44100Hz);
	if ((caps.iRate & iExpectedValue) != (TUint)iExpectedValue)
		{
		ERR_PRINTF3 (_L("DevSound Capabilities Supported Sample Rates returned %d, expected %d"), caps.iRate, iExpectedValue);
		return EFail;
		}


	//// For SOUND DEVICE ENCODING (Amendment by Tony Ma 06/2003):
	////
	//// WINS supports PCM, ALaw and MuLaw.
	//// However Lubbock supports EMMFSoundEncoding16BitPCM only.
	//// So the test has to split up into two parts:

	// Rearranged by NJ - 06/08/03
	// On EKA2 the driver supports the same caps as on hardware

	// if running on EKA2 or WINS is not defined [running on HW]
#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND				//encoder value is not supported on a3f
	iExpectedValue = (EMMFSoundEncoding16BitPCM);

	if ((caps.iEncoding & iExpectedValue) != (TUint)iExpectedValue)
		{
		ERR_PRINTF3 (_L("DevSound Capabilities Supported Encoding returned %d, expected %d"), caps.iEncoding, iExpectedValue);
		return EFail;
		}
#endif

	iExpectedValue = (EMMFMono | EMMFStereo);
	if (caps.iChannels != (TUint)iExpectedValue)
		{
		ERR_PRINTF3 (_L("DevSound Capabilities Supported Channels returned %d, expected %d"), caps.iChannels, iExpectedValue);
		return EFail;
		}

// buffer size differs on EKA2
#if defined __WINS__
    #ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
	    const TInt KMaxBufferLengthEKA2 = 0x4000;	// max EKA2 sound driver buffer size
    #else
        const TInt KMaxBufferLengthEKA2 = 0xBB800;
    #endif //SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
    iExpectedValue = KMaxBufferLengthEKA2;
#else
	iExpectedValue = KMaxBufferLengthEKA2Lubbock;
#endif // __WINS__

#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND				//buffer size is not supported on a3f
	if (caps.iBufferSize != iExpectedValue)
		{
		ERR_PRINTF3 (_L("DevSound Capabilities Supported Buffer Size returned %d, expected %d"), caps.iBufferSize, iExpectedValue);
		return EFail;
		}
#endif

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
TVerdict CTestStepDevSound::TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur)
	{
	iCallbackError = KErrNone;
	//iExpectedValue = KErrUnderflow;

	ResetCallbacks();

	INFO_PRINTF3(_L("Playing Simple Tone aFreq=%d aDur=%d"), aFreq, aDur.Int64());
	TRAPD(err, iMMFDevSound->PlayToneL(aFreq, aDur));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayToneL left with error = %d"), err);
		iCallbackError = err;
		return EFail;
		}
	else
		{
		iAL->InitialiseActiveListener();
		// Start the active scheduler and catch the callback
 		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound ToneFinished returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EToneFinished] != 1)
			{
			ERR_PRINTF2 (_L("DevSound ToneFinished was called %d times, expected 1"), iCallbackArray[EToneFinished]);
			return EFail;
			}
		TInt tot = GetCallbackTotal();
		if (tot > 1)
			{
			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), tot);
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestPlayDTMFString
 * @param aDTMFString
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestPlayDTMFString(TPtrC aDTMFString)
	{
	iCallbackError = KErrNone;

	ResetCallbacks();

	iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());

	// Set request active
	iAL->InitialiseActiveListener();
	INFO_PRINTF2(_L("Playing DTMF String %S"), &aDTMFString);
	TRAPD(err, iMMFDevSound->PlayDTMFStringL(aDTMFString));
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayToneL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound ToneFinished returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EToneFinished] != 1)
			{
			ERR_PRINTF2 (_L("DevSound ToneFinished was called %d times, expected 1"), iCallbackArray[EToneFinished]);
			return EFail;
			}
		TInt tot = GetCallbackTotal();
		if (tot > 1)
			{
			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), tot);
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
void CTestStepDevSound::TestSetVolume(TInt aVol)
	{
	INFO_PRINTF2(_L("Setting Volume = %d"), aVol);
	iMMFDevSound->SetVolume(aVol);
	}

/**
 *
 * TestSetGain
 * @param aGain
 *
 */
void CTestStepDevSound::TestSetGain(TInt aGain)
	{
	INFO_PRINTF2(_L("Setting Gain = %d"), aGain);
	iMMFDevSound->SetGain(aGain);
	}

/**
 *
 * TestSetVolRamp
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestSetVolRamp(TInt /*aVol*/, TInt /*aDur*/)
	{
	return EFail;
	}

/**
 *
 * TestSetGain
 * @result TInt
 *
 */
TInt CTestStepDevSound::TestMaxVolume()
	{
	INFO_PRINTF1(_L("Getting MaxVolume from DevSound"));
	return iMMFDevSound->MaxVolume();
	}

/**
 *
 * TestMaxGain
 * @result TInt
 *
 */
TInt CTestStepDevSound::TestMaxGain()
	{
	INFO_PRINTF1(_L("Getting MaxGain from DevSound"));
	return iMMFDevSound->MaxGain();
	}

/**
 *
 * TestVolume
 * @result TInt
 *
 */
TInt CTestStepDevSound::TestVolume()
	{
	INFO_PRINTF1(_L("Getting current Volume from DevSound"));
	return iMMFDevSound->Volume();
	}

/**
 *
 * TestGain
 * @result TInt
 *
 */
TInt CTestStepDevSound::TestGain()
	{
	INFO_PRINTF1(_L("Getting current Gain from DevSound"));
	return iMMFDevSound->Gain();
	}

/**
 *
 * TestPlayInit
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestPlayInit()
	{
	ResetCallbacks();

	//get buffer from devsound
	TRAPD(err, iMMFDevSound->PlayInitL());
 	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayInitL left with error = %d"), err);
		return EFail;
		}
	else
		{
		// Check for alternative CI implementation
		MTestSetVolIf *volIf = static_cast<MTestSetVolIf*>(iMMFDevSound->CustomInterface(KUidTestSetVolIf));
		if (volIf)
			{
			err = volIf->SetVol(0);
			if (err)
				{
				ERR_PRINTF2 (_L("Unexpected error from SetVol() - %d"), err);
				return EFail;			
				}
			}
		
		iAL->InitialiseActiveListener();

		// Start the active scheduler and catch the callback
		CActiveScheduler::Start();
		if (iCallbackArray[EBuffToFill] != 1)
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeFilled was called %d times, expected 1"), iCallbackArray[EBuffToFill]);
			return EFail;
			}
		TInt tot = GetCallbackTotal();
		if (tot > 1)
			{
			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), tot);
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestPlayData
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestPlayData()
	{
	ResetCallbacks();

	iAL->InitialiseActiveListener();

	iMMFDevSound->PlayData();
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();

	if (iCallbackArray[EBuffToFill] != 1)
		{
		if (iCallbackArray[EBuffToFill] == 0 && iCallbackArray[EPlayError] == 1)
			{
			INFO_PRINTF1(_L("DevSound PlayError was called 1 time.  Must be EOF."));
			}
		else
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeFilled was called %d times, expected 1"), iCallbackArray[EBuffToFill]);
			return EFail;
			}
		}

	TInt tot = GetCallbackTotal();
	if (tot > 1)
		{
		ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), tot);
		return EFail;
		}
	return EPass;
	}

/**
 *
 * TestRecordInit
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestRecordInit()
	{
	ResetCallbacks();

	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Initializing Digital Audio Recording"));
	//get buffer from devsound
	TRAPD(err, iMMFDevSound->RecordInitL());
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound RecordInitL left with error = %d"), err);
		return EFail;
		}
	else
		{
		if (iCallbackArray[EBuffToEmpty] != 1)
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeEmpty was called %d times, expected 1"), iCallbackArray[EBuffToEmpty]);
			return EFail;
			}
		TInt tot = GetCallbackTotal();
		if (tot > 1)
			{
			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), tot);
			return EFail;
			}
		}
	return EPass;
	}


TVerdict CTestStepDevSound::TestRecordInitCapsNotMatch()
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrPermissionDenied;
	ResetCallbacks();
	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Initializing Digital Audio Recording"));
	//get buffer from devsound
	TRAPD(err, iMMFDevSound->RecordInitL());
	if (err != KErrPermissionDenied && err!= KErrNone)
		{
		WARN_PRINTF2 (_L("DevSound RecordInitL left with error = %d"), err);
		return EFail;
		}
	if(err == KErrNone)
		{
		CActiveScheduler::Start();
		// Start the active scheduler and catch the callback
	 	if (iCallbackArray[EBuffToEmpty] != 0)
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeEmptied was called %d times, expected 0"), iCallbackArray[EBuffToEmpty]);
			return EFail;
			}
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound RecordError returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
	 	if (iCallbackArray[ERecError] != 1)
			{
			ERR_PRINTF2 (_L("DevSound RecordError was called %d times, expected 1"), iCallbackArray[ERecError]);
			return EFail;
			}
		}
	return EPass;
	}



/**
 *
 * TestRecordData
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestRecordData()
	{
	ResetCallbacks();

	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Recording Data to a Buffer"));
	iMMFDevSound->RecordData();
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();

	if (iCallbackArray[EBuffToEmpty] != 1)
		{
		if (iCallbackArray[EBuffToEmpty] == 0 && iCallbackArray[ERecError] == 1)
			{
			INFO_PRINTF1(_L("DevSound RecordError was called 1 time.  Must be EOF."));
			}
		else
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeEmptied was called %d times, expected 1"), iCallbackArray[EBuffToEmpty]);
			return EFail;
			}
		}

	TInt tot = GetCallbackTotal();
	if (tot > 1)
		{
		ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), tot);
		return EFail;
		}
	return EPass;
	}


TVerdict CTestStepDevSound::TestRecordDataCapsNotMatch()
	{

	ResetCallbacks();

	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Recording Data to a Buffer"));
	iMMFDevSound->RecordData();
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();

 	if (iCallbackError != KErrPermissionDenied)
 		{
 		WARN_PRINTF2 (_L("DevSound RecordInitL left with error = %d"), iCallbackError);
		return EFail;
		}

	return EPass;
	}



/**
 *
 * ResetCallbacks
 *
 */
void CTestStepDevSound::ResetCallbacks()
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
TInt CTestStepDevSound::GetCallbackTotal()
	{
	TInt total = 0;
	for (TInt i = EInitComplete; i < EDeviceMsg; i++)
		{
		total += iCallbackArray[i];
		}
	return total;
	}

/**
 *
 * GetCallbackTotal
 * @param aNumSamples
 * @param aFilename
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestDigitalPlayback(TInt aNumSamples, TDesC& aFilename)
	{
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type
	return TestDigitalPlayback(aNumSamples, aFilename, pcm16);
	}

/**
 *
 * TestDigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestDigitalPlayback(TInt aNumSamples, TDesC& aFilename, TFourCC& aDataType)
	{
	INFO_PRINTF4(_L("TestDigitalPlayback aNumSamples=%d aFilename=%S aDataType=0x%x"), aNumSamples, &aFilename, aDataType.FourCC());

	//Initialize
	TVerdict initOK = TestInitialize(aDataType, EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	//SetVolume
	TestSetVolume(iMMFDevSound->MaxVolume());

	//Get a buffer to fill
	initOK = TestPlayInit();
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	//open file
	RFs fs;
	RFile file;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not connect to Filesystem. Error is %d"), err);
		return EInconclusive;
		}

	err = file.Open(fs, aFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not open input file. Error is %d"), err);
		return EInconclusive;
		}

	TInt bufferCount = 0;
	if (aNumSamples < 0)
		{// Play to EOF
		while (initOK == KErrNone && iCallbackError == KErrNone)
			{
			//read sizeof buffer from file
			CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
			file.Read(buffer->Data());
			if (buffer->Data().Length()!= buffer->RequestSize())
				{
				INFO_PRINTF3(_L("Data length copied from file = %d. Expected %d. Must be EOF"), buffer->Data().Length(), buffer->RequestSize());
				iBuffer->SetLastBuffer(ETrue);
				}

			//DevSound Play
			initOK = TestPlayData();
			bufferCount ++;
			}
		}
	else
		{
		while (bufferCount < aNumSamples && initOK == KErrNone && iCallbackError == KErrNone)
			{
			//read sizeof buffer from file
			CMMFDataBuffer* buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
			file.Read(buffer->Data());
			if (buffer->Data().Length()!= buffer->RequestSize())
				{
				INFO_PRINTF3(_L("Data length copied from file = %d. Expected %d. Must be EOF"), buffer->Data().Length(), buffer->RequestSize());
				iBuffer->SetLastBuffer(ETrue);
				}

			//DevSound Play
			initOK = TestPlayData();
			bufferCount ++;
			}

#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		TRequestStatus* stat = &iAL->iStatus;
		User::WaitForRequest(*stat);
		User::RequestComplete(stat, KErrNone);
#endif		
		}

	file.Close();
	fs.Close();

	if (initOK != KErrNone)
		{
		return EFail;
		}

	if (aNumSamples >= 0 && bufferCount != aNumSamples)
		{
		return EFail;
		}

	return EPass;
	}

/**
 *
 * TestDigitalRecord
 * @param aNumSamples
 * @param aFilename
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestDigitalRecord(TInt aNumSamples, TDesC& aFilename)
	{
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type
	return TestDigitalRecord(aNumSamples, aFilename, pcm16);
	}

/**
 *
 * TestDigitalRecord
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound::TestDigitalRecord(TInt aNumSamples, TDesC& aFilename, TFourCC& aDataType)
	{
	INFO_PRINTF4(_L("TestDigitalRecord aNumSamples=%d aFilename=%S aDataType=0x%x"), aNumSamples, &aFilename, aDataType.FourCC());

	//Initialize
	TVerdict initOK = TestInitialize(aDataType, EMMFStateRecording);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	//open file
	RFs fs;
	RFile file;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not connect to Filesystem. Error is %d"), err);
		return EInconclusive;
		}

	//Create output directory if it doesn't already exist
	fs.MkDir(KOutputDir);

	err = file.Replace(fs, aFilename, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not create output file. Error is %d"), err);
		return EInconclusive;
		}

	//Initialize Recording
	initOK = TestRecordInit();
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	//SetGain
	TInt vol = iMMFDevSound->MaxVolume();	// Hacked... Fix this...
	TestSetGain(vol);

	TInt bufferCount = 0;
	while (bufferCount < aNumSamples && initOK == KErrNone && iCallbackError == KErrNone)
		{
		//DevSound Record
		initOK = TestRecordData();

		//Write buffer to file
		CMMFDataBuffer* buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
		file.Write(buffer->Data());

		bufferCount ++;
		}

	TRequestStatus* stat = &iAL->iStatus;
	User::RequestComplete(stat, KErrNone);
	INFO_PRINTF3(_L("Total buffers recorded = %d, Expected %d"), bufferCount, aNumSamples);

	file.Close();
	fs.Close();

	if (initOK != KErrNone)
		{
		return EFail;
		}

	if (aNumSamples >= 0 && bufferCount != aNumSamples)
		{
		return EFail;
		}

	return EPass;
	}

/**
 *
 * CTestStepDevSoundCustomInterface
 * Test step constructor
 *
 */
CTestStepDevSoundCustomInterface::CTestStepDevSoundCustomInterface()
	{
	iExpectedValue = KErrNone;
	}

/**
 *
 * ~CTestStepDevSoundCustomInterface
 *	 Test step destructor
 *
 */
CTestStepDevSoundCustomInterface::~CTestStepDevSoundCustomInterface()
	{
	}

/**
 *
 * SetTestSuite
 * @param aTestSuite
 *
 */
void CTestStepDevSoundCustomInterface::SetTestSuite(const CTestSuiteDevSound* aTestSuite)
	{
	iDevSoundTestSuite = aTestSuite;
	}


/**
 *
 * DoTestStepPreambleL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundCustomInterface::DoTestStepPreambleL()
	{
	//Install the Active Scheduler
	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);

	INFO_PRINTF1(_L("Creating DevSound Object"));
	TRAPD(err, iMMFDevSound = CMMFDevSound::NewL());
	if (err)
		{
		return EInconclusive;
		}
	TFileName filename(KInputTestFileNamePCM8_16_s);

	err = iFs.Connect();
    if (err != KErrNone)
        {
        INFO_PRINTF2(_L("Could not connect to Filesystem. Error = %d"), err);
        return EInconclusive;
        }

    err = iFile.Open(iFs, filename, EFileRead);
    if (err != KErrNone)
        {
        INFO_PRINTF2(_L("Could not open input file. Error = %d"), err);
        iFs.Close();
        return EInconclusive;
        }

    return EPass;
	}

/**
 *
 * DoTestStepPostambleL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundCustomInterface::DoTestStepPostambleL()
	{
	INFO_PRINTF1(_L("Deleting DevSound Object"));
	if (iMMFDevSound)
		{
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	delete iAS;
	iAS = NULL;
 	iFile.Close();
    iFs.Close();
	return EPass;
	}

/**
 *
 * RunTest
 * Run the test till the last buffer
 *
 * @result TVerdict
 *
 */

TVerdict CTestStepDevSoundCustomInterface::RunTest()
	{
	iTestStepResult = EFail;
	if (iLastBuffer)
		{
		CActiveScheduler::Stop();
		iTestStepResult = EPass;
		}
	return 	iTestStepResult;
	}


/**
 *
 * FsmL()
 *
 * @param TMmfDevSoundEvents aDevSoundEvent
 *
 */
void CTestStepDevSoundCustomInterface::FsmL(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
			{
	        iCallbackError = KErrNone;
            INFO_PRINTF1(_L("InitializeL - Initializing DevSound..."));
			TFourCC inputDataType(KMMFFourCCCodePCM16);
            TRAPD(err, iMMFDevSound->InitializeL(*this, inputDataType, EMMFStatePlaying)); // Initialize DevSound
   			if (err)
				{//should accept stereo
				INFO_PRINTF1(_L("Failed to Initialize DevSound ..."));
        	 	CActiveScheduler::Stop();
        	 	break;
				}
          	iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
			}
            break;
        case EDevSoundPlayInit:
        	{
        	ResetCallbacks();
            TMMFCapabilities aCapabilities;
			aCapabilities.iRate = EMMFSampleRate8000Hz;
			aCapabilities.iEncoding = EMMFSoundEncoding16BitPCM;
			aCapabilities.iChannels = EMMFStereo;
			TRAPD(err, iMMFDevSound->SetConfigL(aCapabilities));
			if (err)
				{//should accept stereo
				INFO_PRINTF1(_L("Failed to set configuration..."));
        	 	CActiveScheduler::Stop();
        	 	break;
				}
	        INFO_PRINTF1(_L("Initializing the audio device and starting the play process..."));
            TRAPD(error, iMMFDevSound->PlayInitL());// Get buffer from DevSound
			if (error)
				{//should accept stereo
				INFO_PRINTF1(_L("DevSound PlayInitL Failed ..."));
        	 	CActiveScheduler::Stop();
        	 	break;
				}
        	}
            break;
        case EDevSoundPlayData:
			{
            // Fill the buffer with audio data and play the data in the buffer
            // Read sizeof buffer from file
            CMMFDataBuffer*  buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
            if (iBufferCount > 1)
            	{
            	TInt pos=iFile.Size(pos);
            	pos-=1024;
            	iFile.Seek(ESeekCurrent,pos);
            	}
            iFile.Read(buffer->Data());
            if (buffer->Data().Length() != buffer->RequestSize())
                {
                INFO_PRINTF3(_L("Data length copied from file in FsmL = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
                iBuffer->SetLastBuffer(ETrue);
                iLastBuffer = ETrue;
                RunTest();
				break;
                }
            INFO_PRINTF1(_L("PlayData - Playing data..."));
            iMMFDevSound->PlayData();
			}
            break;
		case EDevSoundEmptyBuffer:
			{
			INFO_PRINTF1(_L("Calling - DevSound EmptyBuffers..."));
			TInt error = iMMFDevSound->EmptyBuffers();
			if (error == KErrNotReady)
				{
				INFO_PRINTF2(_L("DevSound EmptyBuffers Device still not ready = %d"), error);
				}
			else if (error != KErrNone)
				{
				INFO_PRINTF2(_L("DevSound EmptyBuffers left with error = %d"), error);
        		CActiveScheduler::Stop();
				break;
				}
			RunTest();
			}
			break;
        default:
            INFO_PRINTF1(_L("Invalid DevSound event!"));
        }
    }

/**
 *
 * InitializeComplete
 * @param aError
 *
 */
void CTestStepDevSoundCustomInterface::InitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called InitializeComplete with aError = %d"), aError);
	iCallbackArray[EInitComplete]++;
	iCallbackError = aError;
	if (iCallbackError)
        {
        INFO_PRINTF2(_L("DevSound InitializeL left with error = %d"), iCallbackError);
        CActiveScheduler::Stop();
        }
    else if (iCallbackArray[EInitComplete] != 1)
        {
        INFO_PRINTF2(_L("DevSound InitializeComplete was called %d times, expected 1."), iCallbackArray[EInitComplete]);
        CActiveScheduler::Stop();
        }
    else
        {
        // Call to initialize the audio device and start the play process
        FsmL(EDevSoundPlayInit);
        }
	}

/**
 *
 * ToneFinished
 * @param aError
 *
 */
void CTestStepDevSoundCustomInterface::ToneFinished(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called ToneFinished with aError = %d"), aError);
	iCallbackArray[EToneFinished]++;
	iCallbackError = aError;
	}

/**
 *
 * BufferToBeFilled
 * @param aBuffer
 *
 */
void CTestStepDevSoundCustomInterface::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("DevSound called BufferToBeFilled."));
	iBufferCount++;
	ERR_PRINTF2 (_L("Buffer Count %d"), iBufferCount);
	iBuffer = aBuffer;
	if (aBuffer != NULL)
		{
		iCallbackError = KErrNone;
		}
	else
		{
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToFill]++;
    // Call to initialize the audio device and start the play process
    TRAPD(err, FsmL(EDevSoundPlayData));
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Play datafailed with Error %d"), err);
		iCallbackError = EFail;
		}
	 else
	 	{
	 	//Call EmptyBuffers on each even number of buffer count
	 	if ( (iBufferCount % 2) == 0 && !iLastBuffer)
	 		{
	 	 	TRAPD(err, FsmL(EDevSoundEmptyBuffer));
	 		if (err != KErrNone)
				{
				ERR_PRINTF2 (_L("EmptyBuffers failed with Error %d"), err);
				iCallbackError = EFail;
				}
	 		}
		}
	}

/**
 *
 * BufferToBeEmptied
 * @param aBuffer
 *
 */
void CTestStepDevSoundCustomInterface::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("DevSound called BufferToBeEmptied"));
	iBuffer = aBuffer;
	if (aBuffer != NULL)
		{
		iCallbackError = KErrNone;
		}
	else
		{
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToEmpty]++;
	}

/**
 *
 * PlayError
 * @param aError
 *
 */
void CTestStepDevSoundCustomInterface::PlayError(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called PlayError with aError = %d"), aError);
	iCallbackError = aError;
	iCallbackArray[EPlayError]++;
	if (iCallbackArray[EBuffToFill] != 1)
    	{
        if (iCallbackArray[EBuffToFill] == 0 && iCallbackArray[EPlayError] == 1)
        	{
            INFO_PRINTF1(_L("DevSound PlayError was called 1 time.  Must be EOF."));
            iTestStepResult = EPass;

           	RunTest();
        	}
        else
        	{
            INFO_PRINTF2(_L("DevSound BufferToBeFilled was called %d times, expected 1"), iCallbackArray[EBuffToFill]);
            CActiveScheduler::Stop();
        	}
		}
    else if (iCallbackError)
    	{
        CActiveScheduler::Stop();
    	}
    else
    	{
        TInt total = GetCallbackTotal();
        if (total > 1)
        	{
            INFO_PRINTF2(_L("DevSound called %d callbacks, expected 1"), total);
            CActiveScheduler::Stop();
        	}
        }
	}

/**
 *
 * RecordError
 * @param aError
 *
 */
void CTestStepDevSoundCustomInterface::RecordError(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called RecordError with aError = %d"), aError);
	iCallbackError = aError;
	iCallbackArray[ERecError]++;
	}

/**
 *
 * ConvertError
 *
 */
void CTestStepDevSoundCustomInterface::ConvertError(TInt /*aError*/)
	{}

/**
 *
 * DeviceMessage
 *
 */
void CTestStepDevSoundCustomInterface::DeviceMessage(TUid, const TDesC8& /*aMsg*/)
	{}

/**
 *
 * ResetCallbacks
 *
 */
void CTestStepDevSoundCustomInterface::ResetCallbacks()
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
TInt CTestStepDevSoundCustomInterface::GetCallbackTotal()
	{
	TInt total = 0;
	for (TInt i = EInitComplete; i < EDeviceMsg; i++)
		{
		total += iCallbackArray[i];
		}
	return total;
	}


/**
 *
 * CTestStepDevSoundInitializePlay
 *
 */
CTestStepDevSoundInitializePlay::CTestStepDevSoundInitializePlay()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0001-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundInitializePlay::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Initialize Play"));
	return TestInitialize(EMMFStatePlaying);
	}

/**
 *
 * CTestStepDevSoundInitializeHwDeviceUidPlay
 *
 */
CTestStepDevSoundInitializeHwDeviceUidPlay::CTestStepDevSoundInitializeHwDeviceUidPlay()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0002-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundInitializeHwDeviceUidPlay::DoTestStepL(void)
	{
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		INFO_PRINTF1(_L("Testing Initialize with HwDevice uid Play - no longer supported"));
	#else
		INFO_PRINTF1(_L("Testing Initialize with HwDevice uid Play"));
	#endif
	TUid uidHwDeviceMulawToPCM16;
	uidHwDeviceMulawToPCM16.iUid = KMmfUidHwDeviceMulawToPCM16; //use uLaw for this test
	return TestInitialize(uidHwDeviceMulawToPCM16, EMMFStatePlaying);
	}

/**
 *
 * CTestStepDevSoundInitializeFourCCPlay
 *
 */
CTestStepDevSoundInitializeFourCCPlay::CTestStepDevSoundInitializeFourCCPlay()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0003-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundInitializeFourCCPlay::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Initialize with fourCC code Play"));
	TFourCC pcm(' ','P','1','6'); //use pcm16 fourcc code
	return TestInitialize(pcm, EMMFStatePlaying);
	}

/**
 *
 * CTestStepDevSoundInitializeRecord
 *
 */
CTestStepDevSoundInitializeRecord::CTestStepDevSoundInitializeRecord()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0004-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundInitializeRecord::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Initialize Record"));
	return TestInitialize(EMMFStateRecording);
	}

/**
 *
 * CTestStepDevSoundInitializeHwDeviceUidRecord
 *
 */
CTestStepDevSoundInitializeHwDeviceUidRecord::CTestStepDevSoundInitializeHwDeviceUidRecord()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0005-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundInitializeHwDeviceUidRecord::DoTestStepL(void)
	{
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		INFO_PRINTF1(_L("Testing Initialize with HwDevice uid Record - no longer supported"));
	#else
		INFO_PRINTF1(_L("Testing Initialize with HwDevice uid Record"));
	#endif
	TUid uidHwDeviceMulawToPCM16;
	uidHwDeviceMulawToPCM16.iUid = KMmfUidHwDeviceMulawToPCM16; //use uLaw for this test
	return TestInitialize(uidHwDeviceMulawToPCM16, EMMFStateRecording);
	}

/**
 *
 * CTestStepDevSoundInitializeFourCCRecord
 *
 */
CTestStepDevSoundInitializeFourCCRecord::CTestStepDevSoundInitializeFourCCRecord()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0006-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundInitializeFourCCRecord::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Initialize with fourCC code Record"));
	TFourCC pcm(' ', 'P', '1', '6'); //use pcm16 fourcc code
	return TestInitialize(pcm, EMMFStateRecording);
	}

/**
 *
 * CTestStepDevSoundInitializeInvalidFourCC
 *
 */
CTestStepDevSoundInitializeInvalidFourCC::CTestStepDevSoundInitializeInvalidFourCC()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0007-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundInitializeInvalidFourCC::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Initialize with invalid fourCC code"));
	TFourCC invalidFourCC('A','B','C','D'); //use invalid fourcc code
	INFO_PRINTF1(_L("Initializing DevSound with invalid fourCC"));

	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, invalidFourCC, EMMFStatePlaying));
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		if (err != KErrNotSupported)
	#else
		if (err != KErrNotFound)
	#endif
		{
		return EFail;
		}
	else
		{
		return EPass;
		}
	}

/**
 *
 * CTestStepDevSoundCheckCaps
 *
 */
CTestStepDevSoundCheckCaps::CTestStepDevSoundCheckCaps()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0008-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundCheckCaps::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Capabilities"));
	iExpectedValue = KErrNone;
	// Initialize
	TVerdict initOK = TestInitialize(EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	// Check Capabilities
	return TestCaps();

	}

/**
 *
 * CTestStepDevSoundSimpleTone
 *
 */
CTestStepDevSoundSimpleTone::CTestStepDevSoundSimpleTone()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0009-HP");

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
TVerdict CTestStepDevSoundSimpleTone::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Simple Tone Playback"));
	TInt freq = 100;
	TTimeIntervalMicroSeconds dur(2000000);

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetVolume(iMMFDevSound->MaxVolume());
	iExpectedValue = KErrUnderflow;
	return TestPlayTone(freq, dur);
	}

/**
 *
 * CTestStepDevSoundDTMFTones
 *
 */
CTestStepDevSoundDTMFTones::CTestStepDevSoundDTMFTones()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0010-HP");

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
TVerdict CTestStepDevSoundDTMFTones::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DTMF Playback"));
	TPtrC dtmfString = (_L("0123456789,abcdef,*#"));

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	iExpectedValue = KErrUnderflow;
	return TestPlayDTMFString(dtmfString);
	}

/**
 *
 * CTestStepDevSoundSimpleToneChangeVolume
 *
 */
CTestStepDevSoundSimpleToneChangeVolume::CTestStepDevSoundSimpleToneChangeVolume()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0011-HP");

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
TVerdict CTestStepDevSoundSimpleToneChangeVolume::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Simple Tone Playback with various Volumes"));
	TVerdict testOK = EFail;

	TInt freq = 100;
	TTimeIntervalMicroSeconds dur(2000000);

	//Initialize
	testOK = TestInitialize(EMMFStateTonePlaying);
	if (testOK != EPass)
		{
		return EInconclusive;
		}

	// Do all the error handling too....
	// Set volume = 0
	TestSetVolume (0);
	// Play Tone
	iExpectedValue = KErrUnderflow;
	testOK = TestPlayTone(freq, dur);
	if (testOK != EPass)
		{
		return EFail;
		}
	// Set volume = MaxVolume/2
	TestSetVolume (iMMFDevSound->MaxVolume()/2);
	// Play Tone
	testOK = TestPlayTone(freq, dur);
	if (testOK != EPass)
		{
		return EFail;
		}
	// Set volume = MaxVolume
	TestSetVolume (iMMFDevSound->MaxVolume());
	// Play Tone
	return TestPlayTone(freq, dur);
	}

/**
 *
 * CTestStepDevSoundDTMFTonesInvalidStrings
 *
 */
CTestStepDevSoundDTMFTonesInvalidStrings::CTestStepDevSoundDTMFTonesInvalidStrings()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0012-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundDTMFTonesInvalidStrings::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DTMF Playback with Invalid Characters"));
	TPtrC dtmfString = (_L("12345$$6gh"));

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	//iExpectedValue = KErrCorrupt; //Return KErrUnderFlow by Nokia adaption
	TestPlayDTMFString(dtmfString);
	if (iCallbackError == KErrNone)
		{
		return EFail;
		}
	return EPass;
	}

/**
 *
 * CTestStepDevSoundSimpleToneInvalidFreq
 *
 */
CTestStepDevSoundSimpleToneInvalidFreq::CTestStepDevSoundSimpleToneInvalidFreq()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0013-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundSimpleToneInvalidFreq::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Simple Tone Playback with Invalid Frequency"));
	TInt freq = -100;
	TTimeIntervalMicroSeconds dur (2000000);

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	TestSetVolume(iMMFDevSound->MaxVolume());
	//iExpectedValue = KErrArgument; //Return KErrUnderFlow by Nokia adaption 
	TestPlayTone(freq, dur);
	if (iCallbackError != KErrNone)
		{
		return EPass;
		}
	return EFail;
	}

/**
 *
 * CTestStepDevSoundSimpleToneInvalidDuration
 *
 */
CTestStepDevSoundSimpleToneInvalidDuration::CTestStepDevSoundSimpleToneInvalidDuration()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0014-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundSimpleToneInvalidDuration::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Simple Tone Playback with Invalid Duration"));
	TInt freq = 100;
	TTimeIntervalMicroSeconds dur (-2000000);

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetVolume(iMMFDevSound->MaxVolume());
    //iExpectedValue = KErrArgument; //Return KErrUnderFlow by Nokia adaption 
	TestPlayTone(freq, dur);
	if (iCallbackError != KErrNone)
		{
		return EPass;
		}
	return EFail;
	}

/**
 *
 * CTestStepDevSoundPlayInit
 *
 */
CTestStepDevSoundPlayInit::CTestStepDevSoundPlayInit()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0015-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundPlayInit::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio PlayInit"));
	iExpectedValue = KErrNone;

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	return TestPlayInit();
	}

/**
 *
 * CTestStepDevSoundPlay1BufferPCM16
 *
 */
CTestStepDevSoundPlay1BufferPCM16::CTestStepDevSoundPlay1BufferPCM16()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0016-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundPlay1BufferPCM16::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Playback (1 Buffer)"));
	iExpectedValue = KErrNone;
	iCallbackError = KErrNone;

	TFileName filename(KInputTestFileNamePCM16);
	return TestDigitalPlayback (1, filename);
	}

/**
 *
 * CTestStepDevSoundPlay5BuffersPCM16
 *
 */
CTestStepDevSoundPlay5BuffersPCM16::CTestStepDevSoundPlay5BuffersPCM16()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0017-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundPlay5BuffersPCM16::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Playback (5 Buffers)"));
	iExpectedValue = KErrNone;
	iCallbackError = KErrNone;

	TFileName filename(KInputTestFileNamePCM16);
	return TestDigitalPlayback (5, filename);
	}

/**
 *
 * CTestStepDevSoundPlayEOFPCM16
 *
 */
CTestStepDevSoundPlayEOFPCM16::CTestStepDevSoundPlayEOFPCM16()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0018-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundPlayEOFPCM16::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Playback to EOF"));
	iCallbackError = KErrNone;

	TFileName filename(KInputTestFileNamePCM16);
	return TestDigitalPlayback (-1, filename);
	}

/**
 *
 * CTestStepDevSoundPlay1BufferPCM8
 *
 */
CTestStepDevSoundPlay1BufferPCM8::CTestStepDevSoundPlay1BufferPCM8()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0019-HP");
	}

/**
 *
 * CTestStepDevSoundPlayPauseTone
 *
 * Play, pause and resume audio, then attempt to play a higher priority tone
 *
 */
CTestStepDevSoundPlayPauseTone* CTestStepDevSoundPlayPauseTone::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) CTestStepDevSoundPlayPauseTone(aSuite);
	}

CTestStepDevSoundPlayPauseTone::CTestStepDevSoundPlayPauseTone(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0065-HP");
	}

void CTestStepDevSoundPlayPauseTone::CloseTest()
	{
	iInputFile.Close();
	iFs.Close();
	delete iMMFToneDevSound;
	delete iMMFAudioDevSound;
	}

void CTestStepDevSoundPlayPauseTone::InitializeComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF1(_L("An initialisation problem occurred"));
		StopTest(aError);
		}
	else
		{
		Fsm();
		}
	}

void CTestStepDevSoundPlayPauseTone::ToneFinished(TInt aError)
	{
	if (iTonePlayKickedOff && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("The tone has played successfully"));
		StopTest();
		}
	else
		{
		INFO_PRINTF1(_L("A tone play problem occurred"));
		StopTest(aError);
		}
	}

void CTestStepDevSoundPlayPauseTone::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	Fsm();
	iBufferCount++;
	}

void CTestStepDevSoundPlayPauseTone::PlayError(TInt aError)
	{
	if (iTonePlayKickedOff && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The tone devsound has gained control correctly"));
		}
	else
		{
		INFO_PRINTF1(_L("A play problem occurred"));
		StopTest(aError);
		}
	}

void CTestStepDevSoundPlayPauseTone::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeEmptied should not be called!"));
	StopTest(KErrNone, EFail);
	}

void CTestStepDevSoundPlayPauseTone::RecordError(TInt aError)
	{
	INFO_PRINTF1(_L("RecordError should not be called!"));
	StopTest(aError, EFail);
	}

void CTestStepDevSoundPlayPauseTone::ConvertError(TInt aError)
	{
	INFO_PRINTF1(_L("ConvertError should not be called!"));
	StopTest(aError, EFail);
	}

void CTestStepDevSoundPlayPauseTone::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	INFO_PRINTF1(_L("DeviceMessage should not be called!"));
	StopTest(KErrNone, EFail);
	}

void CTestStepDevSoundPlayPauseTone::KickoffTestL()
	{
	User::LeaveIfError(iFs.Connect());

	//create DevSound for audio play
	iMMFAudioDevSound = CMMFDevSound::NewL();

	//create DevSound for tone play
	iMMFToneDevSound = CMMFDevSound::NewL();

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 100;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFToneDevSound->SetPrioritySettings(prioritySettings);

	User::LeaveIfError(iInputFile.Open(iFs, KInputTestFileNamePCM16, EFileRead));

	// kick off state machine
	iTestState = EStateInitialiseAudioDevSound;
	INFO_PRINTF1(_L("Initialising audio devsound"));
	TFourCC dataType(KMMFFourCCCodePCM16);
	iMMFAudioDevSound->InitializeL(*this, dataType, EMMFStatePlaying);
	}

void CTestStepDevSoundPlayPauseTone::Fsm()
	{
	TRAPD(err, FsmL());
	if (err != KErrNone)
		{
		StopTest(err);
		}
	}

void CTestStepDevSoundPlayPauseTone::FsmL()
	{
	const TInt KBuffersToPlay = 10;
	const TInt KPauseAtBufferCount = 3;
	const TInt KPlayToneAtBufferCount = 6;

	switch (iTestState)
		{
	case EStateInitialiseAudioDevSound:
		{
		iTestState = EStateInitialiseToneDevSound;
		INFO_PRINTF1(_L("Initialising tone devsound"));
		iMMFToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
		break;
		}
	case EStateInitialiseToneDevSound:
		{
		iTestState = EStatePlayInit;
		INFO_PRINTF1(_L("Initialising audio play"));
		iMMFAudioDevSound->PlayInitL();
		break;
		}
	case EStatePlayInit:
	case EStatePlayData:
		{
		if (iBufferCount < KBuffersToPlay)
			{
			iTestState = EStatePlayData;
			INFO_PRINTF2(_L("Playing buffer %d"), iBufferCount);
			CMMFDataBuffer* buffer = static_cast<CMMFDataBuffer*>(iBuffer);
			iInputFile.Read(buffer->Data());
			if (buffer->Data().Length() != buffer->RequestSize())
				{
				iBuffer->SetLastBuffer(ETrue);
				}

			if (iBufferCount == KPlayToneAtBufferCount)
				{
				iTonePlayKickedOff = ETrue;
				TestPlayToneL();
				}

			iMMFAudioDevSound->PlayData();
			if (iBufferCount == KPauseAtBufferCount)
				{
				// pause and resume
#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				iMMFAudioDevSound->Pause();
				User::After(1000000);	// allow listener to hear the pause
				iMMFAudioDevSound->PlayData();
#endif				
				}
			}
		else
			{
			INFO_PRINTF1(_L("Audio finished playing with no tone play pre-emption"));
			StopTest(KErrNone, EFail);
			}
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrNone, EFail);
		break;
		}
		}
	}

void CTestStepDevSoundPlayPauseTone::TestPlayToneL()
	{
	TInt freq = 440;
	TTimeIntervalMicroSeconds dur(1000000);

	INFO_PRINTF1(_L("Playing 440Hz tone for 1s"));
	iMMFToneDevSound->PlayToneL(freq, dur);
	}

/**
 *
 * CTestStepDevSoundPlayPauseTone2
 *
 * Play and pause audio, then attempt to play a higher priority tone
 *
 */
CTestStepDevSoundPlayPauseTone2* CTestStepDevSoundPlayPauseTone2::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) CTestStepDevSoundPlayPauseTone2(aSuite);
	}

CTestStepDevSoundPlayPauseTone2::CTestStepDevSoundPlayPauseTone2(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0066-HP");
	}

void CTestStepDevSoundPlayPauseTone2::CloseTest()
	{
	iInputFile.Close();
	iFs.Close();
	delete iMMFToneDevSound;
	delete iMMFAudioDevSound;
	}

void CTestStepDevSoundPlayPauseTone2::InitializeComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF1(_L("An initialisation problem occurred"));
		StopTest(aError);
		}
	else
		{
		Fsm();
		}
	}

void CTestStepDevSoundPlayPauseTone2::ToneFinished(TInt aError)
	{
	if (iTonePlayKickedOff && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("The tone has played successfully"));
		StopTest();
		}
	else
		{
		INFO_PRINTF1(_L("A tone play problem occurred"));
		StopTest(aError);
		}
	}

void CTestStepDevSoundPlayPauseTone2::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	Fsm();
	iBufferCount++;
	}

void CTestStepDevSoundPlayPauseTone2::PlayError(TInt aError)
	{
	if (iTonePlayKickedOff && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The tone devsound has gained control correctly"));
		}
	else
		{
		INFO_PRINTF1(_L("A play problem occurred"));
		StopTest(aError);
		}
	}

void CTestStepDevSoundPlayPauseTone2::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeEmptied should not be called!"));
	StopTest(KErrNone, EFail);
	}

void CTestStepDevSoundPlayPauseTone2::RecordError(TInt aError)
	{
	INFO_PRINTF1(_L("RecordError should not be called!"));
	StopTest(aError, EFail);
	}

void CTestStepDevSoundPlayPauseTone2::ConvertError(TInt aError)
	{
	INFO_PRINTF1(_L("ConvertError should not be called!"));
	StopTest(aError, EFail);
	}

void CTestStepDevSoundPlayPauseTone2::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	INFO_PRINTF1(_L("DeviceMessage should not be called!"));
	StopTest(KErrNone, EFail);
	}

void CTestStepDevSoundPlayPauseTone2::KickoffTestL()
	{
	User::LeaveIfError(iFs.Connect());

	//create DevSound for audio play
	iMMFAudioDevSound = CMMFDevSound::NewL();

	//create DevSound for tone play
	iMMFToneDevSound = CMMFDevSound::NewL();

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 100;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFToneDevSound->SetPrioritySettings(prioritySettings);

	User::LeaveIfError(iInputFile.Open(iFs, KInputTestFileNamePCM16, EFileRead));

	// kick off state machine
	iTestState = EStateInitialiseAudioDevSound;
	INFO_PRINTF1(_L("Initialising audio devsound"));
	TFourCC dataType(KMMFFourCCCodePCM16);
	iMMFAudioDevSound->InitializeL(*this, dataType, EMMFStatePlaying);
	}

void CTestStepDevSoundPlayPauseTone2::Fsm()
	{
	TRAPD(err, FsmL());
	if (err != KErrNone)
		{
		StopTest(err);
		}
	}

void CTestStepDevSoundPlayPauseTone2::FsmL()
	{
	const TInt KBuffersToPlay = 10;
	const TInt KPauseAtBufferCount = 3;

	switch (iTestState)
		{
	case EStateInitialiseAudioDevSound:
		{
		iTestState = EStateInitialiseToneDevSound;
		INFO_PRINTF1(_L("Initialising tone devsound"));
		iMMFToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
		break;
		}
	case EStateInitialiseToneDevSound:
		{
		iTestState = EStatePlayInit;
		INFO_PRINTF1(_L("Initialising audio play"));
		iMMFAudioDevSound->PlayInitL();
		break;
		}
	case EStatePlayInit:
	case EStatePlayData:
		{
		if (iBufferCount < KBuffersToPlay)
			{
			iTestState = EStatePlayData;
			INFO_PRINTF2(_L("Playing buffer %d"), iBufferCount);
			CMMFDataBuffer* buffer = static_cast<CMMFDataBuffer*>(iBuffer);
			iInputFile.Read(buffer->Data());
			if (buffer->Data().Length() != buffer->RequestSize())
				{
				iBuffer->SetLastBuffer(ETrue);
				}

			iMMFAudioDevSound->PlayData();
			if (iBufferCount == KPauseAtBufferCount)
				{
				iMMFAudioDevSound->Pause();
				User::After(1000000);	// allow listener to hear the pause
				iTonePlayKickedOff = ETrue;
				TestPlayToneL();
				}
			}
		else
			{
			INFO_PRINTF1(_L("Audio finished playing with no tone play pre-emption"));
			StopTest(KErrNone, EFail);
			}
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrNone, EFail);
		break;
		}
		}
	}

void CTestStepDevSoundPlayPauseTone2::TestPlayToneL()
	{
	TInt freq = 440;
	TTimeIntervalMicroSeconds dur(1000000);

	INFO_PRINTF1(_L("Playing 440Hz tone for 1s"));
	iMMFToneDevSound->PlayToneL(freq, dur);
	}

/**
 *
 * CTestStepDevSoundPlayPauseTone3
 *
 * Play, pause and resume audio
 * Attempt to play a lower priority tone
 * Attempt to play a higher priority tone
 *
 */
CTestStepDevSoundPlayPauseTone3* CTestStepDevSoundPlayPauseTone3::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) CTestStepDevSoundPlayPauseTone3(aSuite);
	}

CTestStepDevSoundPlayPauseTone3::CTestStepDevSoundPlayPauseTone3(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0067-HP");
	}

void CTestStepDevSoundPlayPauseTone3::CloseTest()
	{
	iInputFile.Close();
	iFs.Close();
	delete iMMFLowPriorityToneDevSound;
	delete iMMFHighPriorityToneDevSound;
	delete iMMFAudioDevSound;
	}

void CTestStepDevSoundPlayPauseTone3::InitializeComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF1(_L("An initialisation problem occurred"));
		StopTest(aError);
		}
	else
		{
		Fsm();
		}
	}

void CTestStepDevSoundPlayPauseTone3::ToneFinished(TInt aError)
	{
	if (iHighPriorityTonePlayKickedOff && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("The high priority tone was played successfully"));
		StopTest();
		return;
		}
	else if (iLowPriorityTonePlayKickedOff && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The low priority tone devsound was correctly ignored"));

		// start the higher priority tone play
		TestPlayToneL(EFalse);
		return;
		}

	INFO_PRINTF1(_L("A tone play problem occurred"));
	StopTest(aError);
	}

void CTestStepDevSoundPlayPauseTone3::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	Fsm();
	iBufferCount++;
	}

void CTestStepDevSoundPlayPauseTone3::PlayError(TInt aError)
	{
	if (iLowPriorityTonePlayKickedOff && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The low priority tone devsound has gained control incorrectly"));
		StopTest(aError);
		return;
		}
	else if (iHighPriorityTonePlayKickedOff && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The high priority tone devsound has gained control correctly"));
		return;
		}

	INFO_PRINTF1(_L("An audio play error occurred"));
	StopTest(aError);
	}

void CTestStepDevSoundPlayPauseTone3::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeEmptied should not be called!"));
	StopTest(KErrNone, EFail);
	}

void CTestStepDevSoundPlayPauseTone3::RecordError(TInt aError)
	{
	INFO_PRINTF1(_L("RecordError should not be called!"));
	StopTest(aError, EFail);
	}

void CTestStepDevSoundPlayPauseTone3::ConvertError(TInt aError)
	{
	INFO_PRINTF1(_L("ConvertError should not be called!"));
	StopTest(aError, EFail);
	}

void CTestStepDevSoundPlayPauseTone3::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	INFO_PRINTF1(_L("DeviceMessage should not be called!"));
	StopTest(KErrNone, EFail);
	}

void CTestStepDevSoundPlayPauseTone3::KickoffTestL()
	{
	User::LeaveIfError(iFs.Connect());

	//create DevSound for audio play
	iMMFAudioDevSound = CMMFDevSound::NewL();

	//create low priority DevSound for tone play
	iMMFLowPriorityToneDevSound = CMMFDevSound::NewL();

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFLowPriorityToneDevSound->SetPrioritySettings(prioritySettings);

	//create high priority DevSound for tone play
	iMMFHighPriorityToneDevSound = CMMFDevSound::NewL();

	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 100;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFHighPriorityToneDevSound->SetPrioritySettings(prioritySettings);

	User::LeaveIfError(iInputFile.Open(iFs, KInputTestFileNamePCM16, EFileRead));

	// kick off state machine
	iTestState = EStateInitialiseAudioDevSound;
	INFO_PRINTF1(_L("Initialising audio devsound"));
	TFourCC dataType(KMMFFourCCCodePCM16);
	iMMFAudioDevSound->InitializeL(*this, dataType, EMMFStatePlaying);
	}

void CTestStepDevSoundPlayPauseTone3::Fsm()
	{
	TRAPD(err, FsmL());
	if (err != KErrNone)
		{
		StopTest(err);
		}
	}

void CTestStepDevSoundPlayPauseTone3::FsmL()
	{
	const TInt KBuffersToPlay = 10;
	const TInt KPauseAtBufferCount = 3;

	switch (iTestState)
		{
	case EStateInitialiseAudioDevSound:
		{
		iTestState = EStateInitialiseLowPriorityToneDevSound;
		INFO_PRINTF1(_L("Initialising low priority tone devsound"));
		iMMFLowPriorityToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
		break;
		}
	case EStateInitialiseLowPriorityToneDevSound:
		{
		iTestState = EStateInitialiseHighPriorityToneDevSound;
		INFO_PRINTF1(_L("Initialising high priority tone devsound"));
		iMMFHighPriorityToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
		break;
		}
	case EStateInitialiseHighPriorityToneDevSound:
		{
		iTestState = EStatePlayInit;
		INFO_PRINTF1(_L("Initialising audio play"));
		iMMFAudioDevSound->PlayInitL();
		break;
		}
	case EStatePlayInit:
	case EStatePlayData:
		{
		if (iBufferCount < KBuffersToPlay)
			{
			iTestState = EStatePlayData;
			INFO_PRINTF2(_L("Playing buffer %d"), iBufferCount);
			CMMFDataBuffer* buffer = static_cast<CMMFDataBuffer*>(iBuffer);
			iInputFile.Read(buffer->Data());
			if (buffer->Data().Length() != buffer->RequestSize())
				{
				iBuffer->SetLastBuffer(ETrue);
				}

			iMMFAudioDevSound->PlayData();
			if (iBufferCount == KPauseAtBufferCount)
				{
				iMMFAudioDevSound->Pause();
				User::After(1000000);	// allow listener to hear the pause
				iLowPriorityTonePlayKickedOff = ETrue;
				TestPlayToneL();
				}
			}
		else
			{
			INFO_PRINTF1(_L("Audio finished playing with no tone play pre-emption"));
			StopTest(KErrNone, EFail);
			}
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrNone, EFail);
		break;
		}
		}
	}

void CTestStepDevSoundPlayPauseTone3::TestPlayToneL(TBool aLowPriority)
	{
	TInt freq = 440;
	TTimeIntervalMicroSeconds dur(1000000);

	INFO_PRINTF1(_L("Playing 440Hz tone for 1s"));
	if (aLowPriority)
		{
		iLowPriorityTonePlayKickedOff = ETrue;
		iHighPriorityTonePlayKickedOff = EFalse;
		iMMFLowPriorityToneDevSound->PlayToneL(freq, dur);
		}
	else
		{
		iLowPriorityTonePlayKickedOff = EFalse;
		iHighPriorityTonePlayKickedOff = ETrue;
		iMMFHighPriorityToneDevSound->PlayToneL(freq, dur);
		}
	}

/**
 *
 * RTestStepDevSoundToneTone
 *
 * Play a high priority tone
 * Attempt to pre-empt with a low priority tone (it should fail with KErrInUse)
 * Stop the high priority tone and try to play the low priority tone again
 *
 */
RTestStepDevSoundToneTone* RTestStepDevSoundToneTone::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) RTestStepDevSoundToneTone(aSuite);
	}

RTestStepDevSoundToneTone::RTestStepDevSoundToneTone(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0068-HP");
	}

void RTestStepDevSoundToneTone::CloseTest()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	delete iMMFLowPriorityToneDevSound;
	delete iMMFHighPriorityToneDevSound;
	}

void RTestStepDevSoundToneTone::InitializeComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF1(_L("An initialisation problem occurred"));
		StopTest(aError);
		}
	else
		{
		Fsm();
		}
	}

void RTestStepDevSoundToneTone::ToneFinished(TInt aError)
	{
	if ((iTestState == EStatePlayingLowPriorityTone) && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The low priority tone was rejected successfully"));

		INFO_PRINTF1(_L("Stopping high priority tone"));
		iMMFHighPriorityToneDevSound->Stop();

		INFO_PRINTF1(_L("Attempting to play low priority tone again"));
		TRAPD(err, TestPlayToneL(ETrue));
		if (err != KErrNone)
			{
			INFO_PRINTF1(_L("A leave occurred in TestPlayToneL"));
			StopTest(err);
			}

		return;
		}
	else if ((iTestState == EStatePlayingLowPriorityTone) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("The low priority tone was played successfully"));
		StopTest();
		return;
		}

	INFO_PRINTF1(_L("A tone play problem occurred"));
	StopTest(aError);
	}

void RTestStepDevSoundToneTone::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeFilled should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundToneTone::PlayError(TInt /*aError*/)
	{
	INFO_PRINTF1(_L("PlayError should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundToneTone::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeEmptied should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundToneTone::RecordError(TInt aError)
	{
	INFO_PRINTF1(_L("RecordError should not be called!"));
	StopTest(aError, EFail);
	}

void RTestStepDevSoundToneTone::ConvertError(TInt aError)
	{
	INFO_PRINTF1(_L("ConvertError should not be called!"));
	StopTest(aError, EFail);
	}

void RTestStepDevSoundToneTone::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	INFO_PRINTF1(_L("DeviceMessage should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundToneTone::KickoffTestL()
	{
	iTimer = CPeriodic::NewL(CActive::EPriorityLow);

	//create low priority DevSound for tone play
	iMMFLowPriorityToneDevSound = CMMFDevSound::NewL();

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFLowPriorityToneDevSound->SetPrioritySettings(prioritySettings);

	//create high priority DevSound for tone play
	iMMFHighPriorityToneDevSound = CMMFDevSound::NewL();

	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 100;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFHighPriorityToneDevSound->SetPrioritySettings(prioritySettings);

	// kick off state machine
	iTestState = EStateInitialiseLowPriorityToneDevSound;
	INFO_PRINTF1(_L("Initialising low priority tone devsound"));
	iMMFLowPriorityToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
	}

void RTestStepDevSoundToneTone::Fsm()
	{
	TRAPD(err, FsmL());
	if (err != KErrNone)
		{
		StopTest(err);
		}
	}

TInt RTestStepDevSoundToneTone::TimerCallBack(TAny* aPtr)
   {
   RTestStepDevSoundToneTone* self = static_cast<RTestStepDevSoundToneTone*>(aPtr);
   self->Fsm();
   return EFalse;
   }

void RTestStepDevSoundToneTone::FsmL()
	{
	iTimer->Cancel();

	switch (iTestState)
		{
	case EStateInitialiseLowPriorityToneDevSound:
		{
		iTestState = EStateInitialiseHighPriorityToneDevSound;
		INFO_PRINTF1(_L("Initialising high priority tone devsound"));
		iMMFHighPriorityToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
		break;
		}
	case EStateInitialiseHighPriorityToneDevSound:
		{
		iTestState = EStatePlayingHighPriorityTone;
		INFO_PRINTF1(_L("Playing high priority tone"));
		TestPlayToneL(EFalse);

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KTonePlayDuration / 4, KTonePlayDuration / 4, callBack);
		break;
		}
	case EStatePlayingHighPriorityTone:
		{
		iTestState = EStatePlayingLowPriorityTone;
		INFO_PRINTF1(_L("Attempting to play low priority tone"));
		TestPlayToneL(ETrue);
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrNone, EFail);
		break;
		}
		}
	}

void RTestStepDevSoundToneTone::TestPlayToneL(TBool aLowPriority)
	{
	if (aLowPriority)
		{
		INFO_PRINTF3(_L("Playing %dHz tone for %ds"), KLowToneFrequency, KTonePlayDuration / 1000000);
		iMMFLowPriorityToneDevSound->PlayToneL(KLowToneFrequency, KTonePlayDuration);
		}
	else
		{
		INFO_PRINTF3(_L("Playing %dHz tone for %ds"), KHighToneFrequency, KTonePlayDuration / 1000000);
		iMMFHighPriorityToneDevSound->PlayToneL(KHighToneFrequency, KTonePlayDuration);
		}
	}

/**
 *
 * RTestStepDevSoundRecordRecord
 *
 * Record low priority audio
 * Attempt to pre-empt with a higher priority record
 *
 */
RTestStepDevSoundRecordRecord* RTestStepDevSoundRecordRecord::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) RTestStepDevSoundRecordRecord(aSuite);
	}

RTestStepDevSoundRecordRecord::RTestStepDevSoundRecordRecord(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0069-HP");
	}

void RTestStepDevSoundRecordRecord::CloseTest()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	delete iMMFLowPriorityRecordDevSound;
	delete iMMFHighPriorityRecordDevSound;
	}

void RTestStepDevSoundRecordRecord::InitializeComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF1(_L("An initialisation problem occurred"));
		StopTest(aError);
		}
	else
		{
		Fsm();
		}
	}

void RTestStepDevSoundRecordRecord::ToneFinished(TInt /*aError*/)
	{
	INFO_PRINTF1(_L("ToneFinished should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundRecordRecord::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeFilled should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundRecordRecord::PlayError(TInt /*aError*/)
	{
	INFO_PRINTF1(_L("PlayError should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundRecordRecord::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("Audio data received"));
	// no need to store the data
	if (iTestState == EStateRecordingLowPriorityAudio)
		{
		iMMFLowPriorityRecordDevSound->RecordData();
		}
	else if (iTestState == EStateRecordingHighPriorityAudio)
		{
		iMMFHighPriorityRecordDevSound->RecordData();
		}
	else
		{
		INFO_PRINTF1(_L("A record buffer problem occurred"));
		StopTest(KErrGeneral);
		}
	}

void RTestStepDevSoundRecordRecord::RecordError(TInt aError)
	{
	if ((iTestState == EStateRecordingHighPriorityAudio) && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The low priority recording was successfully pre-empted"));

		iMMFHighPriorityRecordDevSound->Stop();
		StopTest();
		return;
		}

	INFO_PRINTF1(_L("A record pre-emption problem occurred"));
	StopTest(aError);
	}

void RTestStepDevSoundRecordRecord::ConvertError(TInt aError)
	{
	INFO_PRINTF1(_L("ConvertError should not be called!"));
	StopTest(aError, EFail);
	}

void RTestStepDevSoundRecordRecord::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	INFO_PRINTF1(_L("DeviceMessage should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundRecordRecord::KickoffTestL()
	{
	iTimer = CPeriodic::NewL(CActive::EPriorityLow);

	//create low priority DevSound for recording
	iMMFLowPriorityRecordDevSound = CMMFDevSound::NewL();

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStateRecording;
	iMMFLowPriorityRecordDevSound->SetPrioritySettings(prioritySettings);

	//create high priority DevSound for recording
	iMMFHighPriorityRecordDevSound = CMMFDevSound::NewL();

	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 100;
	prioritySettings.iState = EMMFStateRecording;
	iMMFHighPriorityRecordDevSound->SetPrioritySettings(prioritySettings);

	// kick off state machine
	iTestState = EStateInitialiseLowPriorityRecordDevSound;
	INFO_PRINTF1(_L("Initialising low priority record devsound"));
	iMMFLowPriorityRecordDevSound->InitializeL(*this, EMMFStateRecording);
	}

void RTestStepDevSoundRecordRecord::Fsm()
	{
	TRAPD(err, FsmL());
	if (err != KErrNone)
		{
		StopTest(err);
		}
	}

TInt RTestStepDevSoundRecordRecord::TimerCallBack(TAny* aPtr)
   {
   RTestStepDevSoundRecordRecord* self = static_cast<RTestStepDevSoundRecordRecord*>(aPtr);
   self->Fsm();
   return EFalse;
   }

void RTestStepDevSoundRecordRecord::FsmL()
	{
	iTimer->Cancel();

	switch (iTestState)
		{
	case EStateInitialiseLowPriorityRecordDevSound:
		{
		iTestState = EStateInitialiseHighPriorityRecordDevSound;
		INFO_PRINTF1(_L("Initialising high priority record devsound"));
		iMMFHighPriorityRecordDevSound->InitializeL(*this, EMMFStateRecording);
		break;
		}
	case EStateInitialiseHighPriorityRecordDevSound:
		{
		iTestState = EStateRecordingLowPriorityAudio;
		INFO_PRINTF1(_L("Recording low priority audio"));
		iMMFLowPriorityRecordDevSound->RecordInitL();

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KRecordDuration, KRecordDuration, callBack);
		break;
		}
	case EStateRecordingLowPriorityAudio:
		{
		iTestState = EStateRecordingHighPriorityAudio;
		INFO_PRINTF1(_L("Attempting to record high priority audio"));
		iMMFHighPriorityRecordDevSound->RecordInitL();
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrNone, EFail);
		break;
		}
		}
	}

/**
 *
 * RTestStepDevSoundToneReplay
 *
 * Play a low priority tone
 * Pre-empt with a high priority tone
 * After completion of high priority tone, play the low priority tone
 * Stop the low priority tone and check that no further callbacks occur
 */
RTestStepDevSoundToneReplay* RTestStepDevSoundToneReplay::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) RTestStepDevSoundToneReplay(aSuite);
	}

RTestStepDevSoundToneReplay::RTestStepDevSoundToneReplay(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0070-HP");
	}

void RTestStepDevSoundToneReplay::CloseTest()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	delete iMMFLowPriorityToneDevSound;
	delete iMMFHighPriorityToneDevSound;
	}

void RTestStepDevSoundToneReplay::InitializeComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF1(_L("An initialisation problem occurred"));
		StopTest(aError);
		}
	else
		{
		Fsm();
		}
	}

void RTestStepDevSoundToneReplay::ToneFinished(TInt aError)
	{
	if ((iTestState == EStatePlayingHighPriorityTone) && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The low priority tone was rejected successfully"));
		return;
		}
	else if ((iTestState == EStatePlayingHighPriorityTone) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("The high priority tone was played successfully"));
		Fsm();
		return;
		}
	else if (iTestState == EStateWaitingAfterStop)
		{
		INFO_PRINTF1(_L("A callback occurred after calling Stop"));
		StopTest(KErrNone, EFail);
		return;
		}
	else
		{
		INFO_PRINTF1(_L("A tone play problem occurred"));
		StopTest(aError);
		}
	}

void RTestStepDevSoundToneReplay::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeFilled should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundToneReplay::PlayError(TInt /*aError*/)
	{
	INFO_PRINTF1(_L("PlayError should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundToneReplay::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeEmptied should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundToneReplay::RecordError(TInt aError)
	{
	INFO_PRINTF1(_L("RecordError should not be called!"));
	StopTest(aError, EFail);
	}

void RTestStepDevSoundToneReplay::ConvertError(TInt aError)
	{
	INFO_PRINTF1(_L("ConvertError should not be called!"));
	StopTest(aError, EFail);
	}

void RTestStepDevSoundToneReplay::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	INFO_PRINTF1(_L("DeviceMessage should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundToneReplay::KickoffTestL()
	{
	iTimer = CPeriodic::NewL(CActive::EPriorityLow);

	//create low priority DevSound for tone play
	iMMFLowPriorityToneDevSound = CMMFDevSound::NewL();

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFLowPriorityToneDevSound->SetPrioritySettings(prioritySettings);

	//create high priority DevSound for tone play
	iMMFHighPriorityToneDevSound = CMMFDevSound::NewL();

	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 100;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFHighPriorityToneDevSound->SetPrioritySettings(prioritySettings);

	// kick off state machine
	iTestState = EStateInitialiseLowPriorityToneDevSound;
	INFO_PRINTF1(_L("Initialising low priority tone devsound"));
	iMMFLowPriorityToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
	}

void RTestStepDevSoundToneReplay::Fsm()
	{
	TRAPD(err, FsmL());
	if (err != KErrNone)
		{
		StopTest(err);
		}
	}

TInt RTestStepDevSoundToneReplay::TimerCallBack(TAny* aPtr)
   {
   RTestStepDevSoundToneReplay* self = static_cast<RTestStepDevSoundToneReplay*>(aPtr);
   self->Fsm();
   return EFalse;
   }

void RTestStepDevSoundToneReplay::FsmL()
	{
	iTimer->Cancel();

	switch (iTestState)
		{
	case EStateInitialiseLowPriorityToneDevSound:
		{
		iTestState = EStateInitialiseHighPriorityToneDevSound;
		INFO_PRINTF1(_L("Initialising high priority tone devsound"));
		iMMFHighPriorityToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
		break;
		}
	case EStateInitialiseHighPriorityToneDevSound:
		{
		iTestState = EStatePlayingLowPriorityTone;
		INFO_PRINTF1(_L("Playing low priority tone"));
		TestPlayToneL(ETrue);

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KTonePlayDuration / 4, KTonePlayDuration / 4, callBack);
		break;
		}
	case EStatePlayingLowPriorityTone:
		{
		iTestState = EStatePlayingHighPriorityTone;
		INFO_PRINTF1(_L("Attempting to pre-empt with a high priority tone"));
		TestPlayToneL(EFalse);
		break;
		}
	case EStatePlayingHighPriorityTone:
		{
		iTestState = EStateReplayingLowPriorityTone;
		INFO_PRINTF1(_L("Attempting to replay the low priority tone"));
		TestPlayToneL(ETrue);

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KTonePlayDuration / 4, KTonePlayDuration / 4, callBack);
		break;
		}
	case EStateReplayingLowPriorityTone:
		{
		iTestState = EStateWaitingAfterStop;
		INFO_PRINTF1(_L("Stopping low priority tone"));
		iMMFLowPriorityToneDevSound->Stop();

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KTonePlayDuration, KTonePlayDuration, callBack);
		break;
		}
	case EStateWaitingAfterStop:
		{
		INFO_PRINTF1(_L("No callbacks occurred after Stop call"));
		StopTest();
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrNone, EFail);
		break;
		}
		}
	}

void RTestStepDevSoundToneReplay::TestPlayToneL(TBool aLowPriority)
	{
	if (aLowPriority)
		{
		INFO_PRINTF3(_L("Playing %dHz tone for %ds"), KLowToneFrequency, KTonePlayDuration / 1000000);
		iMMFLowPriorityToneDevSound->PlayToneL(KLowToneFrequency, KTonePlayDuration);
		}
	else
		{
		INFO_PRINTF3(_L("Playing %dHz tone for %ds"), KHighToneFrequency, KTonePlayDuration / 1000000);
		iMMFHighPriorityToneDevSound->PlayToneL(KHighToneFrequency, KTonePlayDuration);
		}
	}


/**
 *
 * RTestStepDevSoundRecordRerecord
 *
 * Record low priority audio
 * Pre-empt with a high priority tone play
 * After completion of high priority tone play, restart the low priority record
 * Stop the low priority record and check that no further callbacks occur
 */
RTestStepDevSoundRecordRerecord* RTestStepDevSoundRecordRerecord::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) RTestStepDevSoundRecordRerecord(aSuite);
	}

RTestStepDevSoundRecordRerecord::RTestStepDevSoundRecordRerecord(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0071-HP");
	}

void RTestStepDevSoundRecordRerecord::CloseTest()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	delete iMMFLowPriorityRecordDevSound;
	delete iMMFHighPriorityToneDevSound;
	}

void RTestStepDevSoundRecordRerecord::InitializeComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF1(_L("An initialisation problem occurred"));
		StopTest(aError);
		}
	else
		{
		Fsm();
		}
	}

void RTestStepDevSoundRecordRerecord::ToneFinished(TInt aError)
	{
	if ((iTestState == EStatePlayingHighPriorityTone) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("The high priority tone was played successfully"));
		Fsm();
		return;
		}
	INFO_PRINTF1(_L("A tone play problem occurred"));
	StopTest(aError);
	}

void RTestStepDevSoundRecordRerecord::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeFilled should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundRecordRerecord::PlayError(TInt /*aError*/)
	{
	INFO_PRINTF1(_L("PlayError should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundRecordRerecord::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	if (iTestState == EStateWaitingAfterStop)
		{
		INFO_PRINTF1(_L("A callback occurred after calling Stop"));
		StopTest(KErrNone, EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Audio data received"));
		iBuffersRecordedCount++;
		// no need to store the data
		iMMFLowPriorityRecordDevSound->RecordData();
		}
	}

void RTestStepDevSoundRecordRerecord::RecordError(TInt aError)
	{
	if ((iTestState == EStatePlayingHighPriorityTone) && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The low priority record was rejected successfully"));
		}
	else if (iTestState == EStateWaitingAfterStop)
		{
		INFO_PRINTF1(_L("A callback occurred after calling Stop"));
		StopTest(KErrNone, EFail);
		}
	else
		{
		INFO_PRINTF1(_L("A record problem occurred"));
		StopTest(aError);
		}
	}

void RTestStepDevSoundRecordRerecord::ConvertError(TInt aError)
	{
	INFO_PRINTF1(_L("ConvertError should not be called!"));
	StopTest(aError, EFail);
	}

void RTestStepDevSoundRecordRerecord::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	INFO_PRINTF1(_L("DeviceMessage should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundRecordRerecord::KickoffTestL()
	{
	iTimer = CPeriodic::NewL(CActive::EPriorityLow);

	//create low priority DevSound for recording
	iMMFLowPriorityRecordDevSound = CMMFDevSound::NewL();

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStateRecording;
	iMMFLowPriorityRecordDevSound->SetPrioritySettings(prioritySettings);

	//create high priority DevSound for tone play
	iMMFHighPriorityToneDevSound = CMMFDevSound::NewL();

	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 100;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFHighPriorityToneDevSound->SetPrioritySettings(prioritySettings);

	// kick off state machine
	iTestState = EStateInitialiseLowPriorityRecordDevSound;
	INFO_PRINTF1(_L("Initialising low priority record devsound"));
	iMMFLowPriorityRecordDevSound->InitializeL(*this, EMMFStateRecording);
	}

void RTestStepDevSoundRecordRerecord::Fsm()
	{
	TRAPD(err, FsmL());
	if (err != KErrNone)
		{
		StopTest(err);
		}
	}

TInt RTestStepDevSoundRecordRerecord::TimerCallBack(TAny* aPtr)
   {
   RTestStepDevSoundRecordRerecord* self = static_cast<RTestStepDevSoundRecordRerecord*>(aPtr);
   self->Fsm();
   return EFalse;
   }

void RTestStepDevSoundRecordRerecord::FsmL()
	{
	iTimer->Cancel();

	switch (iTestState)
		{
	case EStateInitialiseLowPriorityRecordDevSound:
		{
		iTestState = EStateInitialiseHighPriorityToneDevSound;
		INFO_PRINTF1(_L("Initialising high priority tone devsound"));
		iMMFHighPriorityToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
		break;
		}
	case EStateInitialiseHighPriorityToneDevSound:
		{
		iTestState = EStateRecordingLowPriority;
		INFO_PRINTF1(_L("Recording low priority audio"));
		iBuffersRecordedCount = 0;
		iMMFLowPriorityRecordDevSound->RecordInitL();

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KRecordDuration, KRecordDuration, callBack);
		break;
		}
	case EStateRecordingLowPriority:
		{
		if (iBuffersRecordedCount < 2)
			{
			INFO_PRINTF1(_L("Too few BufferToBeEmptied calls received"));
			StopTest(KErrNone, EFail);
			return;
			}
		iTestState = EStatePlayingHighPriorityTone;
		INFO_PRINTF3(_L("Playing high priority %dHz tone for %ds"), KHighToneFrequency, KTonePlayDuration / 1000000);
		iMMFHighPriorityToneDevSound->PlayToneL(KHighToneFrequency, KTonePlayDuration);
		break;
		}
	case EStatePlayingHighPriorityTone:
		{
		iTestState = EStateRerecordingLowPriority;
		INFO_PRINTF1(_L("Attempting to restart record low priority audio"));
		iBuffersRecordedCount = 0;
		iMMFLowPriorityRecordDevSound->RecordInitL();

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KRecordDuration, KRecordDuration, callBack);
		break;
		}
	case EStateRerecordingLowPriority:
		{
		if (iBuffersRecordedCount < 2)
			{
			INFO_PRINTF1(_L("Too few BufferToBeEmptied calls received"));
			StopTest(KErrNone, EFail);
			return;
			}
		iTestState = EStateWaitingAfterStop;
		INFO_PRINTF1(_L("Stopping low priority record"));
		iMMFLowPriorityRecordDevSound->Stop();

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KRecordDuration, KRecordDuration, callBack);
		break;
		}
	case EStateWaitingAfterStop:
		{
		INFO_PRINTF1(_L("No callbacks occurred after Stop call"));
		StopTest();
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrNone, EFail);
		break;
		}
		}
	}


/**
 *
 * RTestStepDevSoundPlayReplay
 *
 * Play low priority audio
 * Pre-empt with a high priority tone play
 * After completion of high priority tone play, restart the low priority play
 * Stop the low priority play and check that no further callbacks occur
 */
RTestStepDevSoundPlayReplay* RTestStepDevSoundPlayReplay::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) RTestStepDevSoundPlayReplay(aSuite);
	}

RTestStepDevSoundPlayReplay::RTestStepDevSoundPlayReplay(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0072-HP");
	}

void RTestStepDevSoundPlayReplay::CloseTest()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	delete iMMFLowPriorityPlayDevSound;
	delete iMMFHighPriorityToneDevSound;
	iInputFile.Close();
	iFs.Close();
	}

void RTestStepDevSoundPlayReplay::InitializeComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF1(_L("An initialisation problem occurred"));
		StopTest(aError);
		}
	else
		{
		Fsm();
		}
	}

void RTestStepDevSoundPlayReplay::ToneFinished(TInt aError)
	{
	if ((iTestState == EStatePlayingHighPriorityTone) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("The high priority tone was played successfully"));
		Fsm();
		return;
		}
	INFO_PRINTF1(_L("A tone play problem occurred"));
	StopTest(aError);
	}

void RTestStepDevSoundPlayReplay::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (iTestState == EStateWaitingAfterStop)
		{
		INFO_PRINTF1(_L("A callback occurred after calling Stop"));
		StopTest(KErrNone, EFail);
		}
	else
		{
		iBuffersPlayedCount++;
		INFO_PRINTF1(_L("Audio data requested"));
		CMMFDataBuffer* buffer = static_cast<CMMFDataBuffer*>(aBuffer);
		iInputFile.Read(buffer->Data());
		iMMFLowPriorityPlayDevSound->PlayData();
		}
	}

void RTestStepDevSoundPlayReplay::PlayError(TInt aError)
	{
	if ((iTestState == EStatePlayingHighPriorityTone) && (aError == KErrInUse))
		{
		INFO_PRINTF1(_L("The low priority play was rejected successfully"));
		}
	else if (iTestState == EStateWaitingAfterStop)
		{
		INFO_PRINTF1(_L("A callback occurred after calling Stop"));
		StopTest(KErrNone, EFail);
		}
	else
		{
		INFO_PRINTF1(_L("A play problem occurred"));
		StopTest(aError);
		}
	}

void RTestStepDevSoundPlayReplay::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	INFO_PRINTF1(_L("BufferToBeEmptied should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundPlayReplay::RecordError(TInt aError)
	{
	INFO_PRINTF1(_L("RecordError should not be called!"));
	StopTest(aError, EFail);
	}

void RTestStepDevSoundPlayReplay::ConvertError(TInt aError)
	{
	INFO_PRINTF1(_L("ConvertError should not be called!"));
	StopTest(aError, EFail);
	}

void RTestStepDevSoundPlayReplay::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	INFO_PRINTF1(_L("DeviceMessage should not be called!"));
	StopTest(KErrNone, EFail);
	}

void RTestStepDevSoundPlayReplay::KickoffTestL()
	{
	User::LeaveIfError(iFs.Connect());

	iTimer = CPeriodic::NewL(CActive::EPriorityLow);

	//create low priority DevSound for playing
	iMMFLowPriorityPlayDevSound = CMMFDevSound::NewL();

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStatePlaying;
	iMMFLowPriorityPlayDevSound->SetPrioritySettings(prioritySettings);

	//create high priority DevSound for tone playing
	iMMFHighPriorityToneDevSound = CMMFDevSound::NewL();

	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 100;
	prioritySettings.iState = EMMFStateTonePlaying;
	iMMFHighPriorityToneDevSound->SetPrioritySettings(prioritySettings);

	User::LeaveIfError(iInputFile.Open(iFs, KInputTestFileNamePCM16, EFileRead));

	// kick off state machine
	iTestState = EStateInitialiseLowPriorityPlayDevSound;
	INFO_PRINTF1(_L("Initialising low priority play devsound"));
	TFourCC dataType(KMMFFourCCCodePCM16);
	iMMFLowPriorityPlayDevSound->InitializeL(*this, dataType, EMMFStatePlaying);
	}

void RTestStepDevSoundPlayReplay::Fsm()
	{
	TRAPD(err, FsmL());
	if (err != KErrNone)
		{
		StopTest(err);
		}
	}

TInt RTestStepDevSoundPlayReplay::TimerCallBack(TAny* aPtr)
   {
   RTestStepDevSoundPlayReplay* self = static_cast<RTestStepDevSoundPlayReplay*>(aPtr);
   self->Fsm();
   return EFalse;
   }

void RTestStepDevSoundPlayReplay::FsmL()
	{
	iTimer->Cancel();

	switch (iTestState)
		{
	case EStateInitialiseLowPriorityPlayDevSound:
		{
		iTestState = EStateInitialiseHighPriorityToneDevSound;
		INFO_PRINTF1(_L("Initialising high priority tone devsound"));
		iMMFHighPriorityToneDevSound->InitializeL(*this, EMMFStateTonePlaying);
		break;
		}
	case EStateInitialiseHighPriorityToneDevSound:
		{
		iTestState = EStatePlayingLowPriority;
		INFO_PRINTF1(_L("Playing low priority audio"));
		iBuffersPlayedCount = 0;
		iMMFLowPriorityPlayDevSound->PlayInitL();

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KPlayDuration, KPlayDuration, callBack);
		break;
		}
	case EStatePlayingLowPriority:
		{
		if (iBuffersPlayedCount < 2)
			{
			INFO_PRINTF1(_L("Too few BufferToBeFilled calls received"));
			StopTest(KErrNone, EFail);
			return;
			}
		iTestState = EStatePlayingHighPriorityTone;
		INFO_PRINTF3(_L("Playing high priority %dHz tone for %ds"), KHighToneFrequency, KTonePlayDuration / 1000000);
		iMMFHighPriorityToneDevSound->PlayToneL(KHighToneFrequency, KTonePlayDuration);
		break;
		}
	case EStatePlayingHighPriorityTone:
		{
		iTestState = EStateReplayingLowPriority;
		INFO_PRINTF1(_L("Attempting to restart low priority audio play"));
		iBuffersPlayedCount = 0;
		iMMFLowPriorityPlayDevSound->PlayInitL();

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KPlayDuration, KPlayDuration, callBack);
		break;
		}
	case EStateReplayingLowPriority:
		{
		if (iBuffersPlayedCount < 2)
			{
			INFO_PRINTF1(_L("Too few BufferToBeFilled calls received"));
			StopTest(KErrNone, EFail);
			return;
			}
		iTestState = EStateWaitingAfterStop;
		INFO_PRINTF1(_L("Stopping low priority audio play"));
		iMMFLowPriorityPlayDevSound->Stop();

		TCallBack callBack(TimerCallBack, this);
		iTimer->Start(KPlayDuration, KPlayDuration, callBack);
		break;
		}
	case EStateWaitingAfterStop:
		{
		INFO_PRINTF1(_L("No callbacks occurred after Stop call"));
		StopTest();
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrNone, EFail);
		break;
		}
		}
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundPlay1BufferPCM8::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Playback pcm8(1 Buffer)"));
	iExpectedValue = KErrNone;
	iCallbackError = KErrNone;

	TFileName filename(KInputTestFileNamePCM8);
	TFourCC pcm8(' ','P','U','8'); //use unsigned pcm8 fourcc code
	return TestDigitalPlayback (1, filename, pcm8);
	}

/**
 *
 * CTestStepDevSoundPlay5BuffersPCM8
 *
 */
CTestStepDevSoundPlay5BuffersPCM8::CTestStepDevSoundPlay5BuffersPCM8()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0020-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundPlay5BuffersPCM8::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Playback pcm8(5 Buffers)"));
	iExpectedValue = KErrNone;
	iCallbackError = KErrNone;

	TFileName filename(KInputTestFileNamePCM8);
	TFourCC pcm8(' ','P','U','8'); //use unsigned pcm8 fourcc code
	return TestDigitalPlayback (5, filename, pcm8);
	}

/**
 *
 * CTestStepDevSoundPlayEOFPCM8
 *
 */
CTestStepDevSoundPlayEOFPCM8::CTestStepDevSoundPlayEOFPCM8()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0021-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundPlayEOFPCM8::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Playback with PU8 to EOF"));
	iExpectedValue = KErrNone;
	iCallbackError = KErrNone;

	TFileName filename(KInputTestFileNamePCM8);
	TFourCC pcm8(' ','P','U','8'); //use unsigned pcm8 fourcc code
	return TestDigitalPlayback (-1, filename, pcm8);
	}

/**
 *
 * CTestStepDevSoundPlayGarbageEOF
 *
 */
CTestStepDevSoundPlayGarbageEOF::CTestStepDevSoundPlayGarbageEOF()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0022-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundPlayGarbageEOF::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Playback to EOF"));
	iCallbackError = KErrNone;

	TFileName filename = _L("Z:\\TSU_MMF_DEVSOUND_SUITE\\Input\\garbage.raw");
	return TestDigitalPlayback (-1, filename);
	}

/**
 *
 * CTestStepDevSoundMaxGain
 *
 */
CTestStepDevSoundMaxGain::CTestStepDevSoundMaxGain()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0023-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundMaxGain::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound MaxGain"));
	TVerdict initOK = TestInitialize(EMMFStateRecording);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TInt maxGain = TestMaxGain();
	if ((maxGain <= 0) || (maxGain > KMaxGain))
		{
		ERR_PRINTF3(_L("DevSound returned MaxGain = %d.  Expected MaxGain <= %d."), TestMaxGain(), KMaxGain);
		return EFail;
		}
	return EPass;
	}

/**
 *
 * CTestStepDevSoundMaxVolume
 *
 */
CTestStepDevSoundMaxVolume::CTestStepDevSoundMaxVolume()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0024-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundMaxVolume::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound MaxVolume"));
	TVerdict initOK = TestInitialize(EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TInt maxVolume = TestMaxVolume();
	if ((maxVolume <= 0) || (maxVolume > KMaxVolume))
		{
		ERR_PRINTF3(_L("DevSound returned MaxVolume = %d.  Expected MaxVolume = %d."), TestMaxVolume(), KMaxVolume);
		return EFail;
		}
	return EPass;
	}

/**
 *
 * CTestStepDevSoundSetGain
 *
 */
CTestStepDevSoundSetGain::CTestStepDevSoundSetGain()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0025-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundSetGain::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound SetGain"));
	TVerdict initOK = TestInitialize(EMMFStateRecording);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TInt gain = TestGain();
	INFO_PRINTF2(_L("Current DevSound Gain = "), gain);
	INFO_PRINTF1(_L("Setting DevSound Gain to MaxGain"));
	TestSetGain (TestMaxGain());
	if (TestGain() != TestMaxGain())
		{
		ERR_PRINTF3(_L("DevSound returned Gain = %d.  Expected Gain = %d."), TestGain(), TestMaxGain());
		return EFail;
		}

	INFO_PRINTF1(_L("Setting DevSound Gain to MaxGain/2"));
	TestSetGain (TestMaxGain()/2);
	if (TestGain() != TestMaxGain()/2)
		{
		ERR_PRINTF3(_L("DevSound returned Gain = %d.  Expected Gain = %d."), TestGain(), TestMaxGain()/2);
		return EFail;
		}

	INFO_PRINTF1(_L("Setting DevSound Gain to 0"));
	TestSetGain (0);
	if (TestGain() != 0)
		{
		ERR_PRINTF2(_L("DevSound returned Gain = %d.  Expected Gain = 0."), TestGain());
		return EFail;
		}

	return EPass;
	}

/**
 *
 * CTestStepDevSoundSetVolume
 *
 */
CTestStepDevSoundSetVolume::CTestStepDevSoundSetVolume()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0026-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundSetVolume::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound SetVolume"));
	TVerdict initOK = TestInitialize(EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	TInt volume = TestVolume();
	INFO_PRINTF2(_L("Current DevSound Volume = "), volume);
	INFO_PRINTF1(_L("Setting DevSound Volume to MaxVolume"));
	TestSetVolume (TestMaxVolume());
	if (TestVolume() != TestMaxVolume())
		{
		ERR_PRINTF3(_L("DevSound returned Volume = %d.  Expected Volume = %d."), TestVolume(), TestMaxVolume());
		return EFail;
		}

	INFO_PRINTF1(_L("Setting DevSound Volume to MaxVolume/2"));
	TestSetVolume (TestMaxVolume()/2);
	if (TestVolume() != TestMaxVolume()/2)
		{
		ERR_PRINTF3(_L("DevSound returned Volume = %d.  Expected Volume = %d."), TestVolume(), TestMaxVolume()/2);
		return EFail;
		}

	INFO_PRINTF1(_L("Setting DevSound Volume to 0"));
	TestSetVolume (0);
	if (TestVolume() != 0)
		{
		ERR_PRINTF2(_L("DevSound returned Volume = %d.  Expected Volume = 0."), TestVolume());
		return EFail;
		}

	return EPass;
	}

/**
 *
 * CTestStepDevSoundRecord1BufferPCM16
 *
 */
CTestStepDevSoundRecord1BufferPCM16::CTestStepDevSoundRecord1BufferPCM16()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0027-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundRecord1BufferPCM16::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Record (1 buffer)"));
	iCallbackError = KErrNone;

	TFileName filename(KOutputTestFileName1BufPCM16);
	return TestDigitalRecord(1, filename);
	}

/**
 *
 * CTestStepDevSoundRecord10BuffersPCM16
 *
 */
CTestStepDevSoundRecord10BuffersPCM16::CTestStepDevSoundRecord10BuffersPCM16()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0028-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundRecord10BuffersPCM16::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Record (10 buffer)"));
	iCallbackError = KErrNone;

	TFileName filename(KOutputTestFileName10BufPCM16);
	return TestDigitalRecord(10, filename);
	}

/**
 *
 * CTestStepDevSoundRecord1BufferPCM8
 *
 */
CTestStepDevSoundRecord1BufferPCM8::CTestStepDevSoundRecord1BufferPCM8()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0029-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundRecord1BufferPCM8::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Record pcm8(1 buffer)"));
	iExpectedValue = KErrNone;
	iCallbackError = KErrNone;

	TFileName filename(KOutputTestFileName1BufPCM8);
	TFourCC pcm8(' ','P','U','8'); //use unsigned pcm8 fourcc code
	return TestDigitalRecord(1, filename, pcm8);
	}

/**
 *
 * CTestStepDevSoundRecord10BuffersPCM8
 *
 */
CTestStepDevSoundRecord10BuffersPCM8::CTestStepDevSoundRecord10BuffersPCM8()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0030-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundRecord10BuffersPCM8::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Record pcm8(10 buffer)"));
	iExpectedValue = KErrNone;
	iCallbackError = KErrNone;

	TFileName filename(KOutputTestFileName10BufPCM8);
	TFourCC pcm8(' ','P','U','8'); //use unsigned pcm8 fourcc code
	return TestDigitalRecord(10, filename, pcm8);
	}

/**
 *
 * CTestStepDevSoundSetConfigStereo
 *
 */
CTestStepDevSoundSetConfigStereo::CTestStepDevSoundSetConfigStereo()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0031-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundSetConfigStereo::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Stereo configured Audio Playback to EOF"));
	iCallbackError = KErrNone;
	TMMFCapabilities aCapabilities;
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TVerdict aVerdict;
	#endif
	aCapabilities.iRate = EMMFSampleRate8000Hz;
	aCapabilities.iEncoding = EMMFSoundEncoding16BitPCM;
	aCapabilities.iChannels = EMMFStereo;
	TRAPD(err,iMMFDevSound->SetConfigL(aCapabilities));
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	if (err==KErrNotReady)
		{//behaviour no longer supported
		INFO_PRINTF2(_L("SetConfigL failed with expected error %d"),err);
		aVerdict = TestInitialize(EMMFStatePlaying);
		if (aVerdict == EFail)
			{
			return EFail;
			}
		else
			{
			TRAPD(err,iMMFDevSound->SetConfigL(aCapabilities));
			if (err)
				{
				INFO_PRINTF2(_L("SetConfigL failed with error %d"),err);
				return EFail;
				}
			else
				{
				TFileName filename(KInputTestFileNamePCM8_16_s);
				return TestDigitalPlayback (-1, filename);
				}
			}
		}
	else
		{
		INFO_PRINTF2(_L("SetConfigL failed with a different error %d"),err);
		return EFail;
		}
	#else
	if (err)
		{//should accept stereo
		return EFail;
		}
	else
		{
		TFileName filename(KInputTestFileNamePCM8_16_s);
		return TestDigitalPlayback (-1, filename);
		}
	#endif
	}

/**
 *
 * CTestStepDevSoundSetConfigSampleRate
 *
 */
CTestStepDevSoundSetConfigSampleRate::CTestStepDevSoundSetConfigSampleRate()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0032-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundSetConfigSampleRate::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing 16Khz Sample rate configured Audio Playback to EOF"));
	iCallbackError = KErrNone;
	TMMFCapabilities aCapabilities;
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TVerdict aVerdict;
	#endif
	aCapabilities.iRate = EMMFSampleRate16000Hz;
	aCapabilities.iEncoding = EMMFSoundEncoding16BitPCM;
	aCapabilities.iChannels = EMMFMono;
	TRAPD(err, iMMFDevSound->SetConfigL(aCapabilities));
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	if (err==KErrNotReady)
		{//behaviour no longer supported
		INFO_PRINTF2(_L("SetConfigL failed with expected error %d"),err);
		aVerdict = TestInitialize(EMMFStatePlaying);
		if (aVerdict == EFail)
			{
			return EFail;
			}
		else
			{
			TRAPD(err,iMMFDevSound->SetConfigL(aCapabilities));
			if (err)
				{
				INFO_PRINTF2(_L("SetConfigL failed with error %d"),err);
				return EFail;
				}
			else
				{
				TFileName filename(KInputTestFileNamePCM16_16_m);
				return TestDigitalPlayback (-1, filename);
				}
			}
		}
	else
		{
		INFO_PRINTF2(_L("SetConfigL failed with a different error %d"),err);
		return EFail;
		}
	#else
	if (err)
		{//should accept 16KHz
		return EFail;
		}
	else
		{
		TFileName filename(KInputTestFileNamePCM16_16_m);
		return TestDigitalPlayback (-1, filename);
		}
	#endif
	}


/**
 *
 * CTestStepDevSoundGetSupportedInputDataTypes
 *
 */
CTestStepDevSoundGetSupportedInputDataTypes::CTestStepDevSoundGetSupportedInputDataTypes()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0033-HP");
	}

/**
 *
 * DoTestStepL
 * check the number of supported input datatypes
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundGetSupportedInputDataTypes::DoTestStepL(void)
	{
	TVerdict verdict = EPass;
	TUint numberOfSupportedDataTypes = 0;

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStateRecording; //if recording does not support any input data types
	RArray<TFourCC> supportedDataTypes;
	TRAPD(err, iMMFDevSound->GetSupportedInputDataTypesL(supportedDataTypes, prioritySettings));

	//behaviour is DevSound specific if priority settings set to EMMFStateRecording
	//for input data types ref DevSound returns supported data types- but should
	//not return an error
	if (err)
		{
		INFO_PRINTF2(_L("Error getting supported data types %d"),err);
		verdict = EFail;
		}
	if (verdict == EPass)
		{ //now check for playing
		prioritySettings.iState = EMMFStatePlaying; //should give supported input data types
		TRAP(err, iMMFDevSound->GetSupportedInputDataTypesL(supportedDataTypes, prioritySettings));

		if (!err)
			{
			numberOfSupportedDataTypes = supportedDataTypes.Count();
			//print out the supported data types
			//note test does not make any assumptions as to what the supported datatypes are
			if (numberOfSupportedDataTypes)
				{
				TUint32 testsFourCC = TFourCC(_L8("TST*")).FourCC(); // if any fourCc matches first three chars then we treat as test
				for (TUint entryNumber = 0; entryNumber < numberOfSupportedDataTypes; entryNumber++)
					{
					TUint32 fourCC = supportedDataTypes[entryNumber].FourCC();
					if ((testsFourCC&0xFFFFFF) == (fourCC&0xFFFFFF))
						{
						// comparison ignoring top byte - equates to last digit of fourCC
						continue;
						}
					
					ResetCallbacks();
					TRAP(err,iMMFDevSound->InitializeL(*this, fourCC, prioritySettings.iState));
					if ((err)||(iCallbackError))
						{
						INFO_PRINTF5(_L("DevSound failed to support data type fourCC = %c%c%c%c"), fourCC,fourCC>>8,fourCC>>16,fourCC>>24);
						INFO_PRINTF2(_L("left with error = %d"), err);
						verdict = EFail;
						}
					else
						{
						iAL->InitialiseActiveListener();
						CActiveScheduler::Start();
						INFO_PRINTF5(_L("DevSound supports DataType fourCC =  %c%c%c%c"), fourCC,fourCC>>8,fourCC>>16,fourCC>>24);
						}
					}
				}
			else
				{
				INFO_PRINTF1(_L("No supported datatypes detected for playing"));
				verdict = EFail;
				}
			}//if (!err)
		else
			{
			INFO_PRINTF2(_L("Error getting supported data types %d"),err);
			verdict = EFail;
			}
		}//	if (verdict == EPass)

	supportedDataTypes.Close();

	return verdict;
	}


/**
 *
 * CTestStepDevSoundGetSupportedOutputDataTypes
 *
 */
CTestStepDevSoundGetSupportedOutputDataTypes::CTestStepDevSoundGetSupportedOutputDataTypes()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0034-HP");
	}

/**
 *
 * DoTestStepL
 * check the number of supported output datatypes
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundGetSupportedOutputDataTypes::DoTestStepL(void) {

	TVerdict verdict = EPass;

	TUint numberOfSupportedDataTypes = 0;

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStatePlaying; //if playing does not support any output data types
	RArray<TFourCC> supportedDataTypes;
	TRAPD(err, iMMFDevSound->GetSupportedOutputDataTypesL(supportedDataTypes, prioritySettings));

	//behaviour is DevSound specific if priority settings set to EMMFStatePlaying
	//for output data types ref DevSound returns supported data types- but should
	//not return an error
	if (err)
		{
		INFO_PRINTF2(_L("Error getting supported data types %d"),err);
		verdict = EFail;
		}
	if (verdict == EPass)
		{ //now check for recording
		prioritySettings.iState = EMMFStateRecording; //should give supported output data types
		TRAP(err, iMMFDevSound->GetSupportedOutputDataTypesL(supportedDataTypes, prioritySettings));

		if (!err)
			{
			numberOfSupportedDataTypes = supportedDataTypes.Count();
			//print out the supported data types
			//note test does not make any assumptions as to what the supported datatypes are
			if (numberOfSupportedDataTypes)
				{
				TUint32 testsFourCC = TFourCC(_L8("TST*")).FourCC(); // if any fourCc matches first three chars then we treat as test
				for (TUint entryNumber = 0; entryNumber < numberOfSupportedDataTypes; entryNumber++)
					{
					TUint32 fourCC = supportedDataTypes[entryNumber].FourCC();
					if ((testsFourCC&0xFFFFFF) == (fourCC&0xFFFFFF))
						{
						// comparison ignoring top byte - equates to last digit of fourCC
						continue;
						}
					
					ResetCallbacks();
					TRAP(err,iMMFDevSound->InitializeL(*this, fourCC, prioritySettings.iState));
					if ((err)||(iCallbackError))
						{
						INFO_PRINTF5(_L("DevSound failed to support data type fourCC = %c%c%c%c"), fourCC,fourCC>>8,fourCC>>16,fourCC>>24);
						INFO_PRINTF2(_L("left with error = %d"), err);
						verdict = EFail;
						}
					else
						{
						iAL->InitialiseActiveListener();
						CActiveScheduler::Start();
						INFO_PRINTF5(_L("DevSound supports DataType fourCC =  %c%c%c%c"), fourCC,fourCC>>8,fourCC>>16,fourCC>>24);
						}
					}
				}
			else
				{
				INFO_PRINTF1(_L("No supported datatypes detected for recording"));
				verdict = EFail;
				}
			}//if (!err)
		else
			{
			INFO_PRINTF2(_L("Error getting supported data types %d"),err);
			verdict = EFail;
			}
		}//	if (verdict == EPass)

	supportedDataTypes.Close();

	return verdict;
	}


/**
 *
 * CTestStepDevSoundGetSupportedInputDataTypesOOM
 *
 */
CTestStepDevSoundGetSupportedInputDataTypesOOM::CTestStepDevSoundGetSupportedInputDataTypesOOM()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0035-HP");
	}


/**
 *
 * DoTestStepL
 * check the number of supported input datatypes for OOM
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundGetSupportedInputDataTypesOOM::DoTestStepL(void)
	{

	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	TInt KMaxFailRate = 100000;
	TBool complete = EFalse;
	TBool nullAlloc = EFalse;
	TInt failCount;
	TInt err = 0;
	TInt expectedNumberOfSupportedDataTypes = 0;

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStatePlaying;
	RArray<TFourCC> supportedDataTypes;

	TRAP(err, iMMFDevSound->GetSupportedInputDataTypesL(supportedDataTypes, prioritySettings));
	if (!err)
		expectedNumberOfSupportedDataTypes = supportedDataTypes.Count();
	else
		{
		__MM_HEAP_MARKEND;
		INFO_PRINTF2(_L("Unexpected leave on GetSupportedInputDataTypesL %d" ), err );
		return EInconclusive;
		}

	for (failCount=1; !complete && failCount<KMaxFailRate; ++failCount)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAP(err, iMMFDevSound->GetSupportedInputDataTypesL(supportedDataTypes, prioritySettings));
		if (err == KErrNone)
			{//need to check if next alloc fails
			TAny* testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				nullAlloc = ETrue;
				}
			else
				{
				User::Free(testAlloc);
				complete = ETrue;
				}
			//check that supported data types have been found
			if (supportedDataTypes.Count() != expectedNumberOfSupportedDataTypes)
				{
				verdict = EFail;
				complete = ETrue;
				INFO_PRINTF3(_L("number of supported datatypes found %d expected %d"),supportedDataTypes.Count(),expectedNumberOfSupportedDataTypes);
				}
			}
		else if (err != KErrNoMemory)
			{
			verdict = EFail; // bad error code
			complete = ETrue;
			INFO_PRINTF2(_L("OOM test error %d" ), err );
			}
		else if (supportedDataTypes.Count())
			{//error is KErrNoMemory check no supported data types have been found
			verdict = EFail;
			INFO_PRINTF2(_L("OOM occured but we still had %d supported datatypes" ), supportedDataTypes.Count());
			}
		}

	if (failCount >= KMaxFailRate-1)
		{
		INFO_PRINTF2(_L("fail count too high %d" ), failCount );
		verdict = EFail;
		}

	if (!nullAlloc)
		{
		INFO_PRINTF1(_L("never alloced after no OOM "));
		verdict = EFail;
		}

	__UHEAP_RESET;

	supportedDataTypes.Close();

	__MM_HEAP_MARKEND;

	return verdict;
	}


/**
 *
 * CTestStepDevSoundGetSupportedOutputDataTypesOOM
 *
 */
CTestStepDevSoundGetSupportedOutputDataTypesOOM::CTestStepDevSoundGetSupportedOutputDataTypesOOM()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0036-HP");
	}


/**
 *
 * DoTestStepL
 * check the number of supported output datatypes for OOM
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundGetSupportedOutputDataTypesOOM::DoTestStepL(void)
	{

	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	TInt KMaxFailRate = 100000;
	TBool complete = EFalse;
	TBool nullAlloc = EFalse;
	TInt failCount;
	TInt err = 0;
	TInt expectedNumberOfSupportedDataTypes = 0;

	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = 0;
	prioritySettings.iState = EMMFStateRecording;
	RArray<TFourCC> supportedDataTypes;

	TRAP(err, iMMFDevSound->GetSupportedOutputDataTypesL(supportedDataTypes, prioritySettings));
	if (!err)
		expectedNumberOfSupportedDataTypes = supportedDataTypes.Count();
	else
		{
		__MM_HEAP_MARKEND;
		INFO_PRINTF2(_L("Unexpected leave on GetSupportedOutputDataTypesL %d" ), err );
		return EInconclusive;
		}

	for (failCount=1; !complete && failCount<KMaxFailRate; ++failCount)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAP(err, iMMFDevSound->GetSupportedOutputDataTypesL(supportedDataTypes, prioritySettings));
		if (err == KErrNone)
			{//need to check if next alloc fails
			TAny* testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				nullAlloc = ETrue;
				}
			else
				{
				User::Free(testAlloc);
				complete = ETrue;
				}
			//check that supported data types have been found
			if (supportedDataTypes.Count() != expectedNumberOfSupportedDataTypes)
				{
				verdict = EFail;
				complete = ETrue;
				INFO_PRINTF3(_L("number of supported datatypes found %d expected %d"),supportedDataTypes.Count(),expectedNumberOfSupportedDataTypes);
				}
			}
		else if (err != KErrNoMemory)
			{
			verdict = EFail; // bad error code
			complete = ETrue;
			INFO_PRINTF2(_L("OOM test error %d" ), err );
			}
		else if (supportedDataTypes.Count())
			{//error is KErrNoMemory check no supported data types have been found
			verdict = EFail;
			INFO_PRINTF2(_L("OOM occured but we still had %d supported datatypes" ), supportedDataTypes.Count());
			}
		}

	if (failCount >= KMaxFailRate-1)
		{
		INFO_PRINTF2(_L("fail count too high %d" ), failCount );
		verdict = EFail;
		}

	if (!nullAlloc)
		{
		INFO_PRINTF1(_L("never alloced after no OOM "));
		verdict = EFail;
		}

	__UHEAP_RESET;

	supportedDataTypes.Close();

	__MM_HEAP_MARKEND;

	return verdict;
	}



CTestStepDevSoundRecordInitNoUECap::CTestStepDevSoundRecordInitNoUECap(TBool aHasMMDDCap)
	{
	if (aHasMMDDCap)
		{
		iTestStepName = _L("MM-MMF-DEVSOUND-U-0051-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUND-U-0052-HP");
		}
	}


TVerdict CTestStepDevSoundRecordInitNoUECap::DoTestStepL(void)
	{
	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateRecording);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	INFO_PRINTF1(_L("Testing DevSound Audio RecordInit - App has no User Environment capability"));
	iCallbackError = KErrNone;

	return TestRecordInitCapsNotMatch();
	}


CTestStepDevSoundRecordInitWithUECap::CTestStepDevSoundRecordInitWithUECap(TBool aHasAllCaps)
	{
	if (aHasAllCaps)
		{
		iTestStepName = _L("MM-MMF-DEVSOUND-U-0053-HP");
		}
	else
		{
		//Has only UserEnvironment capability
		iTestStepName = _L("MM-MMF-DEVSOUND-U-0054-HP");
		}
	}

TVerdict CTestStepDevSoundRecordInitWithUECap::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing DevSound Audio RecordInit - App has the User Environment capability"));
	iCallbackError = KErrNone;

	//Recording 1 PCM16 buffer
	TFileName filename(KOutputTestFileName1BufPCM16);
	return TestDigitalRecord(1, filename);
	}

CTestStepDevSoundRecordDataWithUECap::CTestStepDevSoundRecordDataWithUECap(TBool aHasAllCaps)
	{
		if (aHasAllCaps)
		{
		iTestStepName = _L("MM-MMF-DEVSOUND-U-0057-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUND-U-0058-HP");
		}
	}


TVerdict CTestStepDevSoundRecordDataWithUECap::DoTestStepL(void)
	{

	INFO_PRINTF1(_L("Testing DevSound Audio RecordInit - App has the User Environment capability"));
	iCallbackError = KErrNone;

	//Recording 1 PCM buffer
	TFileName filename(KOutputTestFileName1BufPCM16);
	return TestDigitalRecord(1, filename);
	}


const TUid KUidDevSoundTestCustomInterface = {KMmfDevSoundTestCustomInterfaceMux};

//
// Basic Custom Interface Test
//

RTestStepBasicCustomInterface* RTestStepBasicCustomInterface::NewL()
	{
	RTestStepBasicCustomInterface* self = new (ELeave) RTestStepBasicCustomInterface();
	return self;
	}

RTestStepBasicCustomInterface::RTestStepBasicCustomInterface()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0101-HP");
	}

TVerdict RTestStepBasicCustomInterface::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test CMMFDevSound Custom Interface Framework"));

	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	CMMFDevSound* devSound = CMMFDevSound::NewL();
	CleanupStack::PushL(devSound);

	TVerdict result = EPass;

	// fetch the custom interface
	MMMFDevSoundTestCustomInterface* ptr = static_cast <MMMFDevSoundTestCustomInterface*> (devSound->CustomInterface(KUidDevSoundTestCustomInterface));

	if (ptr)
		{
		// can only do this with a valid interface
		result = TestCustomInterface(ptr);
		}
	else
		{
		result = EFail;
		}

	CleanupStack::PopAndDestroy(2, scheduler);
	return result;
	}


TVerdict RTestStepBasicCustomInterface::TestCustomInterface(MMMFDevSoundTestCustomInterface* aInterface)
	{
	TVerdict result = EPass;
	TInt err = KErrNone;

	CActiveListener* listener= new(ELeave)CActiveListener;
	CleanupStack::PushL(listener);


	INFO_PRINTF1(_L("Testing TestSyncCommand(const TDesC&)"));
	err = aInterface->TestSyncCommand(KDevSoundCITestIn);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected result %d"), err);
		result = EFail;
		}


	TBuf8<16> dummyResult;
	dummyResult.FillZ();

	INFO_PRINTF1(_L("Testing TestSyncCommandResult(const TDesC&, TPtr8*)"));
	err = aInterface->TestSyncCommandResult(KDevSoundCITestIn, dummyResult);

	if (dummyResult != KDevSoundCITestOut)
		{
		INFO_PRINTF1(_L("Result string is not correct!"));
		result = EFail;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected result %d"), err);
		result = EFail;
		}


	INFO_PRINTF1(_L("Testing TestAsyncCommand(const TDesC&, TRequestStatus&)"));
	listener->InitialiseActiveListener();
	TRAP(err, aInterface->TestAsyncCommandL(KDevSoundCITestIn, listener->iStatus));
	CActiveScheduler::Start();
	err = listener->iStatus.Int();

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected result %d"), err);
		result = EFail;
		}


	INFO_PRINTF1(_L("Testing TestAsyncCommandResult(const TDesC&, TDes8&, TRequestStatus&)"));
	dummyResult.FillZ();
	listener->InitialiseActiveListener();
	TRAP(err, aInterface->TestAsyncCommandResultL(KDevSoundCITestIn, dummyResult, listener->iStatus));
	CActiveScheduler::Start();

	if (dummyResult != KDevSoundCITestOut)
		{
		INFO_PRINTF1(_L("Result string is not correct!"));
		result = EFail;
		}

	err = listener->iStatus.Int();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected result %d"), err);
		result = EFail;
		}

	CleanupStack::PopAndDestroy(listener);

	return result;
	}


RTestStepBasicCustomInterfaceAlloc* RTestStepBasicCustomInterfaceAlloc::NewL()
	{
	RTestStepBasicCustomInterfaceAlloc* self = new (ELeave) RTestStepBasicCustomInterfaceAlloc();
	return self;
	}

RTestStepBasicCustomInterfaceAlloc::RTestStepBasicCustomInterfaceAlloc()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0102-HP");
	}


/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 *
 */
TVerdict RTestStepBasicCustomInterfaceAlloc::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0102-CP"));
	INFO_PRINTF1(_L("this is Alloc Failure test of iniatilising a custom interface"));

	//__MM_HEAP_MARK;
	TVerdict testRes = EFail;
	TRAP( err, testRes = RTestStepBasicCustomInterface::DoTestStepL());
	//__MM_HEAP_MARKEND;

	if (err != KErrNone || testRes == EFail)
		return EFail;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		//__MM_HEAP_MARK;

		testRes = EFail;
		TRAP( err, testRes = RTestStepBasicCustomInterface::DoTestStepL());
		INFO_PRINTF2(_L("RTestStepBasicCustomInterface::DoTestStepL err=%d"),err);
		completed = EFalse;
		if (err == KErrNone)
			{
			if (testRes == EPass)
				{
				TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
				if (testAlloc==NULL)
					{
					reachedEnd = ETrue;
					failCount--; // -= 1;
					}
				else
					{
					User::Free(testAlloc);
					}
				completed = reachedEnd;
				}
			else
				{
				// the test failed but we have already proved above
				// that the required plugins are present
				// therefore we assume that the interface returned
				// a NULL pointer and so we can continue to test
				// until the timeout
				completed = EFalse;
				}
			}
		else if (err != KErrNoMemory) // bad error code
			{
			completed = ETrue;
			}

		//__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		// if we have completed or have exceeded the fail
		// count limit then quit
		if (completed || failCount > 1000)
			break; // exit loop

		failCount++;
		}

	//failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


CTestStepDevSoundCheckSampleRateforTone::CTestStepDevSoundCheckSampleRateforTone()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0060-HP");
	}

/**
 *
 * DoTestStepL
 * Check that the SampleRate supported for tone play is 8000Hz only. Fix for DEF054803
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundCheckSampleRateforTone::DoTestStepL(void)
	{
	ResetCallbacks();
	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		INFO_PRINTF1(_L("Initialization failed"));
		return EInconclusive;
		}
	TMMFCapabilities capabilities;
	capabilities.iRate = EMMFSampleRate16000Hz;//Passing sample rate other than 8000Hz
	capabilities.iEncoding = EMMFSoundEncoding16BitPCM;
	capabilities.iChannels = EMMFStereo;
	TRAPD(err,iMMFDevSound->SetConfigL(capabilities));
	if (err != KErrNotSupported)
		{//shouldn't take value other than 8000Hz for playing tone
		INFO_PRINTF2(_L("Returned error %d Expected -5"),err);
		return EFail;
		}
	return EPass;
	}

/**
 *
 * CTestStepDevSoundEmptyBuffersPos
 *
 */
CTestStepDevSoundEmptyBuffersPos::CTestStepDevSoundEmptyBuffersPos()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0061-HP");
	}

/**
 *
 * DoTestStepL
 * Test to check for EmptyBuffers custom interface
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundEmptyBuffersPos::DoTestStepL(void)
	{

	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Testing DevSound EmptyBuffers custom interface"));
	iExpectedValue = KErrNone;

	TRAPD(err, FsmL(EDevSoundInit)); // Call to initialize DevSound
	if (err)
   		{
		INFO_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
		return iTestStepResult;
		}

	CActiveScheduler::Start();
	return iTestStepResult;

	}

/**
 *
 * CTestStepDevSoundEmptyBuffersNeg1
 *
 */
CTestStepDevSoundEmptyBuffersNeg1::CTestStepDevSoundEmptyBuffersNeg1()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0062-HP");
	}

/**
 *
 * DoTestStepL
 * Test to check for EmptyBuffers method
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundEmptyBuffersNeg1::DoTestStepL(void)
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Testing EmptyBuffers in record mode"));
	iExpectedValue = KErrNone;

	TRAPD(err, FsmL(EDevSoundInit)); // Call to initialize DevSound   
	if (err)
   		{
		INFO_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
		return iTestStepResult;
		}

	CActiveScheduler::Start();
	return iTestStepResult;

	}

/**
 *
 * FsmL()
 *
 * @param TMmfDevSoundEvents aDevSoundEvent
 *
 */
void CTestStepDevSoundEmptyBuffersNeg1::FsmL(TMmfDevSoundEvents aDevSoundEvent)
    {

    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
			{
	        iCallbackError = KErrNone;
            INFO_PRINTF1(_L("InitializeL - Initializing DevSound..."));
            TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording)); // Initialize DevSound
   			if (err)
				{
				INFO_PRINTF1(_L("Failed to Initialize DevSound ..."));
        	 	CActiveScheduler::Stop();
				}
   			}
            break;
		case EDevSoundEmptyBuffer:
			{
			INFO_PRINTF1(_L("Calling - DevSound EmptyBuffers..."));
			TInt error = iMMFDevSound->EmptyBuffers();
			if (error == KErrNotSupported)
				{
				iTestStepResult = EPass;
				}
			else
				{
				iTestStepResult = EFail;
				}
			ERR_PRINTF3 (_L("DevSound EmptyBuffers returned %d, expected %d"), error, KErrNotSupported)
			CActiveScheduler::Stop();
   			}
			break;
        default:
            INFO_PRINTF1(_L("Invalid DevSound event!"));
        }
    }

/**
 *
 * InitializeComplete
 * @param aError
 *
 */
void CTestStepDevSoundEmptyBuffersNeg1::InitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called InitializeComplete with aError = %d"), aError);
	iCallbackArray[EInitComplete]++;
	iCallbackError = aError;
	if (iCallbackError)
        {
        INFO_PRINTF2(_L("DevSound InitializeL left with error = %d"), iCallbackError);
        CActiveScheduler::Stop();
        }
    else if (iCallbackArray[EInitComplete] != 1)
        {
        INFO_PRINTF2(_L("DevSound InitializeComplete was called %d times, expected 1."), iCallbackArray[EInitComplete]);
        CActiveScheduler::Stop();
        }
    else
        {
        // Call DevSound EmptyBuffers
        FsmL(EDevSoundEmptyBuffer);
        }
	}


#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
/**
 *
 * CTestStepDevSoundServerSideAlloc
 * This test is currently disabled via the script file.  It is ONLY used in debugging/development, and is NOT 
 * required to pass itself as a gateway to submission.  It is to be enabled in future to reproduce defects 
 * (i.e. SVR-ALLOC-0X defects as already reported in the PREQ1540 DB) and is to be edited when run, changing 
 * the method to test and the starting value of "failcount".
 *
 */
CTestStepDevSoundServerSideAlloc::CTestStepDevSoundServerSideAlloc()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-1073-HP");
	}

/**
 *
 * DoTestStepL
 * Test to stress-alloc-test A3F DevSound Server Thread via Mux/Demux ECOM Plugin
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundServerSideAlloc::DoTestStepL(void)
	{

	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Testing A3F Devsound Server Side with Alloc stress testing"));
	iExpectedValue = KErrNone;

	TRAPD(err, AllocLoopTestL()); // Call to initialize DevSound								
	if (err)
   		{
		INFO_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
		return iTestStepResult;
		}
	return iTestStepResult;
	}

/**
 *
 * AllocLoopTestL()
 *
 * @param TMmfDevSoundEvents aDevSoundEvent
 *
 */
void CTestStepDevSoundServerSideAlloc::AllocLoopTestL()
    {

	CMMFDevSound* testerDevSound = CMMFDevSound::NewL();
    TUid iUid = {KUidMmfDevSoundCustomInterfaceMuxPlugin};
    MMMFDevSoundCIMuxPluginInterface* devSoundMuxCIInstance = static_cast <MMMFDevSoundCIMuxPluginInterface*> (testerDevSound->CustomInterface(iUid));
    if (!devSoundMuxCIInstance)
    	{
		INFO_PRINTF1(_L("Failed to create tester interface"));
		iTestStepResult = EInconclusive;
    	return;
    	}
	CleanupStack::PushL(testerDevSound);
    iCallbackError = KErrNone;
    TBool continueLoop = ETrue;
	for( TInt failCount = 0; continueLoop; failCount++ ) 
		{
		/* 
		There are three predicates to Test Pass. 
		1. The synchronous call to InitializeL returns in context and must return with KErrNone or 
		KErrNoMemory - any other error is a Test Fail. 
		When InitializeL returns with KErrNone, only then is the AS is kicked off. DevSound may perform further 
		allocations before its callback method InitializeComplete is called.
		2. The asynchronous callback to InitializeComplete here should also return with KErrNone or KErrNoMemory
		- any other Error is a Test Fail.  If it returns with KErrNoMemory, we restart simulated server heap alloc fails
		with our last used value of failcount.  We then keep checking the callback until it completes with KErrNone.
		Again, any other error is a test fail.
		3. The test will fail if, after the call to InitializeL, the next server alloc tested fails.  Something has gone wrong -
		the number of allocs needed for InitializeL to complete has changed (reduced).  
		This is not a real fail, but is an indication this test cannot be trusted due to the above. The test cannot detect if
		the number of allocs needed for InitializeL to complete has increased.
		*/
		
		INFO_PRINTF2(_L("Initiating Devsound Heap Failure, failCount = %d"), failCount);
		devSoundMuxCIInstance->DevSoundCIMuxCauseHeapFail(failCount);
		  
		INFO_PRINTF1(_L("NewL - InitializeL..."));
		TInt resultOfMethodCall = MethodToTest();
		
		if ( resultOfMethodCall == KErrNoMemory )	
			{
			//Do nothing for now
			//DevSound ran out of memory before starting initialization, due to simulated heap alloc failure - continue with loop
			}
		else if ( resultOfMethodCall != KErrNone && resultOfMethodCall != KErrNoMemory )    
			{
			//3rd predicate failed
			INFO_PRINTF2(_L("Failed to Initialize DevSound - abnormal test termination - 1st or 2nd predicate failed(%d)"), resultOfMethodCall);
			iTestStepResult = EFail;
			continueLoop = EFalse;							
			}
		else if ( resultOfMethodCall == KErrNone )							
			{
			if (!devSoundMuxCIInstance->DevSoundCIMuxCheckHeapFail())
				{
				iTestStepResult = EPass;	
				}
			else
				{
				//3rd predicate failed
				INFO_PRINTF1(_L("Failed to Initialize DevSound - abnormal test termination - 3rd predicate failed"));
				iTestStepResult = EFail;
				}	
			continueLoop = EFalse;
			}			
		devSoundMuxCIInstance->DevSoundCIMuxStopHeapFail();
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	devSoundMuxCIInstance->DevSoundCIMuxStopHeapFail(); // call again just in case
	CleanupStack::PopAndDestroy(testerDevSound);
	}


/**
 *
 * InitializeComplete
 * @param aError
 *
 */
void CTestStepDevSoundServerSideAlloc::InitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    iCallbackError = aError;
	CActiveScheduler::Stop();
	}

/**
 *
 * MethodToTest
 * Attempts to initialise member instance of CMMFDevSound (both phases)
 * @result TInt
 *
 */
TInt CTestStepDevSoundServerSideAlloc::MethodToTest() 
	{
	TRAPD(err, iMMFDevSound = CMMFDevSound::NewL());
	if (err)
		{
		return err;
		}
	TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording)); 
	INFO_PRINTF1(_L("Done It"));
	if ( err != KErrNone)    
		{
		return err;						
		}
	CActiveScheduler::Start();					
	return iCallbackError;
	}

#endif

/**
 *
 * CTestStepDevSoundEmptyBuffersNeg2
 *
 */
CTestStepDevSoundEmptyBuffersNeg2::CTestStepDevSoundEmptyBuffersNeg2()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0063-HP");
	}

/**
 *
 * DoTestStepL
 * Test to check for EmptyBuffers method
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundEmptyBuffersNeg2::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing emptying buffers wihtout starting playback"));
	iTestStepResult = EFail;
	INFO_PRINTF1(_L("Calling - DevSound EmptyBuffers..."));
	TInt error = iMMFDevSound->EmptyBuffers();
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	if (error == KErrNotReady)
	#else
	if (error == KErrNotSupported)
	#endif
		{
		iTestStepResult = EPass;
		}
	else
		{
		iTestStepResult = EFail;
		}
	ERR_PRINTF3 (_L("DevSound EmptyBuffers returned %d, expected %d"), error, KErrNotSupported)
	return iTestStepResult;
	}

/**
 *
 * CTestStepDevSoundCITest
 *
 */
CTestStepDevSoundCITest::CTestStepDevSoundCITest()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0064-HP");
	}

/**
 *
 * DoTestStepL
 * Test to check DEF086171
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundCITest::DoTestStepL(void)
	{

	iTestStepResult = EFail;

	INFO_PRINTF1(_L("DEF086171  - Possible memory leak within DevSound CI Framework"));
	iExpectedValue = KErrNone;

	iFsmHelper = CFsmHelper::NewL();
	iFsmHelper->Start();
	iFsmHelper->SetDevSoundTestPointer(this);
	iFsmHelper->SetDevSoundPointer(iMMFDevSound);
	CActiveScheduler::Start();
	iTestStepResult = iFsmHelper->GetResult();
	return iTestStepResult;

	}

/**
 *
 * InitializeComplete
 * @param aError
 *
 */
void CTestStepDevSoundCITest::InitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called InitializeComplete with aError = %d"), aError);
	iCallbackArray[EInitComplete]++;
	iCallbackError = aError;
	if (iCallbackError)
		{
		INFO_PRINTF2(_L("DevSound InitializeL left with error = %d"), iCallbackError);
		CActiveScheduler::Stop();
		}
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	else
		{
		iFsmHelper->Start();
		}
	#endif
	}

/**
 *
 * Close
 *
 *
 */
void CTestStepDevSoundCITest::Close()
	{
	delete iFsmHelper;
	iFsmHelper = NULL;
	}
/**
 *
 * NewL
 * CFsmHelper
 *
 */
CFsmHelper* CFsmHelper::NewL()
	{
	CFsmHelper* self = new(ELeave) CFsmHelper();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
 	}

/**
 *
 * CFsmHelper
 * Initilaization List
 *
 */
CFsmHelper::CFsmHelper():CActive(EPriorityIdle)
	{
	CActiveScheduler::Add(this);
 	}

/**
 *
 * CFsmHelper
 * Destructor
 *
 */
CFsmHelper::~CFsmHelper()
	{
	delete iLog;
 	}

/**
 *
 * DoCancel
 * @result void
 *
 */
void CFsmHelper::DoCancel()
 	{
	}

/**
 *
 * Start
 * @result void
 *
 */
void CFsmHelper::Start()
	{
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS, KErrNone);
	SetActive();
	}

/**
 *
 * ConstructL
 * @result void
 *
 */
void CFsmHelper::ConstructL()
	{
	iLog = CLog::NewL();
	iLog->OpenLogFileL();
	iTestResult = EFail;
	}

/**
 *
 * RunL
 * @result void
 *
 */
void CFsmHelper::RunL()
	{
	switch(iEventStatus)
		{
		case EDevSoundInit1:
			{
			// Initialize the DevSound to use PCM16 hwDevice for the first time
			#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			TUid uidHwDevicePCM16ToPCM16;
			uidHwDevicePCM16ToPCM16.iUid = KMmfUidHwDevicePCM16ToPCM16;
			TRAPD(err, iMMFDevSound->InitializeL(*iTestPointer, uidHwDevicePCM16ToPCM16, EMMFStatePlaying));
			#else
			TRAPD(err, iMMFDevSound->InitializeL(*iTestPointer, KMMFFourCCCodePCM16, EMMFStatePlaying));
			#endif
			if (err)
				{
				iLog->Log(_L("Failed to Initialize DevSound ..."));
				CActiveScheduler::Stop();
				break;
				}
			else
				{
				// fetch the custom interface for bitrate
				iTestInterface = static_cast <MMMFDevSoundCustomInterfaceBitRate*> (iMMFDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate));
				if(!iTestInterface)
					{
					CActiveScheduler::Stop();
					}
				iEventStatus = EDevSoundInit2;
				#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				Start();
				#endif
				break;
				}
			}

		case EDevSoundInit2:
			{
			// Re-initializing the DevSound to use the GSM610 hwDevice
			#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			TUid uidHwDeviceGSMToPCM16;
			uidHwDeviceGSMToPCM16.iUid = KMmfUidHwDeviceGSM610ToPCM16; //use GSM610
			TRAPD(err, iMMFDevSound->InitializeL(*iTestPointer, uidHwDeviceGSMToPCM16, EMMFStatePlaying));
			#else
			TRAPD(err, iMMFDevSound->InitializeL(*iTestPointer, KMMFFourCCCodeGSM610, EMMFStatePlaying));
			#endif
			if (err)
				{
				iLog->Log(_L("Failed to Initialize DevSound ..."));
				CActiveScheduler::Stop();
				break;
				}
			else
				{
				// fetch the custom interface for bitrate.This should not return a NULL pointer as
				// there is a time lag in the interfaces being released.So add some delay and try to fetch the custom interface pointer again.
				iTestInterface = static_cast <MMMFDevSoundCustomInterfaceBitRate*> (iMMFDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate));
				if (!iTestInterface)
					{
					CActiveScheduler::Stop();
					break;
					}
				iEventStatus = 	EAddDelayClient2;
				#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				Start();
				#endif
				break;
				}
			}
		case EAddDelayClient2:
			{
			//Adding delay as there is a time lag in the interfaces being released.
			iLog->Log(_L("Adding Delay"));
			User::After(10000);
			iEventStatus = EClient2Idle;
			Start();
			break;
			}
		case EClient2Idle:
			{
			//Fetch the custom interface pointer for bitrate after some delay.
			//Since the second initialisation has properly refreshed the custom interfaces,
			//and bitrate custom interface is not supported by GSM610 hwDevice a NULL custom interface pointer is retrieved for bitrate.
			iTestInterface = static_cast <MMMFDevSoundCustomInterfaceBitRate*> (iMMFDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate));
			if (!iTestInterface)
				{
				iTestResult = EPass;
				iLog->Log(_L("TEST Passed"));
				}
			else
				{
				iTestResult = EFail;
				iLog->Log(_L("TEST Failed"));
				}
			CActiveScheduler::Stop();
			break;
			}
		default:
			break;
		}
	}

/**
 *
 * GetStatus
 * @result TRequestStatus
 *
 */
TRequestStatus* CFsmHelper::GetStatus()
	{
	return &(this->iStatus);
	}

/**
 *
 * GetResult
 * @result TVerdict
 *
 */
TVerdict CFsmHelper::GetResult()
	{
	return iTestResult;
	}

/**
 *
 * SetDevSoundPointer
 *
 *
 */
void CFsmHelper::SetDevSoundPointer(CMMFDevSound* aMMFDevSound)
	{
	iMMFDevSound = aMMFDevSound;
	}

/**
 *
 * SetDevSoundTestPointer
 *
 *
 */
void CFsmHelper::SetDevSoundTestPointer(CTestStepDevSoundCITest* aMMFDevSndTestPointer)
	{
	iTestPointer = aMMFDevSndTestPointer;
	}

/////////////////////////////
//test case for PDEF117811 :Ref DevSound policy updating on error condition is inconsistent 
CTestDevSoundObserver* CTestDevSoundObserver::NewL(RDevSoundPolicyUpdateTest* aParent, RDevSoundPolicyUpdateTest::TClientId aClientId)
	{
	CTestDevSoundObserver* self = new(ELeave)CTestDevSoundObserver(aParent, aClientId);
	return self;
	}

CTestDevSoundObserver::CTestDevSoundObserver(RDevSoundPolicyUpdateTest* aParent, RDevSoundPolicyUpdateTest::TClientId aClientId):
iParent(aParent), iSelfId(aClientId)
	{
	}

void CTestDevSoundObserver::InitializeComplete(TInt aError)
	{
	iParent->Fsm(iSelfId, RDevSoundPolicyUpdateTest::EDevSoundInitComplete, aError);
	}

void CTestDevSoundObserver::ToneFinished(TInt aError)
	{
	iParent->Fsm(iSelfId, RDevSoundPolicyUpdateTest::ETonePlayError, aError);
	}

void CTestDevSoundObserver::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	CMMFDataBuffer* buffer = static_cast<CMMFDataBuffer*>(aBuffer);
	TDes8 des = buffer->Data();
	des.SetLength(des.Length());
	iParent->Fsm(iSelfId, RDevSoundPolicyUpdateTest::EBTBF, KErrNone);
	}

void CTestDevSoundObserver::PlayError(TInt aError)
	{
	iParent->Fsm(iSelfId, RDevSoundPolicyUpdateTest::EPlayError, aError);
	}

void CTestDevSoundObserver::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	iParent->Fsm(iSelfId, RDevSoundPolicyUpdateTest::EBTBE, KErrNone);
	}

void CTestDevSoundObserver::RecordError(TInt aError)
	{
	iParent->Fsm(iSelfId, RDevSoundPolicyUpdateTest::ERecordError, aError);
	}

void CTestDevSoundObserver::ConvertError(TInt /*aError*/)
	{
	}

void CTestDevSoundObserver::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	}

/*
 * A brief explanation of the test.
 * The uses two DevSound objects - DevSound1, DevSound2.
 * DevSound1 is initialised with test HwDevices.
 * There are two test HwDevices, which leave with KErrAbort from different functions.
 * 
 * DevSound2 is always initialised with default(PCM16) hwDevice.
 * 
 * Using DevSound1 and test scenarios, KErrAbort or KErrGeneral(only for ToneSequence test)
 * can be generated at DevSound-Server(CMMFDevSoundSvrImp). The fix should updated policy
 * on all error conditions.
 * 
 * Hence without calling DevSound1->Stop(), it should be possible to initialise and play
 * from DevSound2. The test passes in this case. Had the policy not been updated, then
 * any attempt to play from DevSound2 will result in KErrInUse.
 * 
 * The test has different stages. Every stage is targetted at error on a particular function in
 * CMMFDevSoundSvrImp.
 * 
 */
RDevSoundPolicyUpdateTest* RDevSoundPolicyUpdateTest::NewL(const TDesC& aStepName)
	{
	RDevSoundPolicyUpdateTest* self = new(ELeave)RDevSoundPolicyUpdateTest(aStepName);
	return self;
	}

RDevSoundPolicyUpdateTest::RDevSoundPolicyUpdateTest(const TDesC& aStepName):
iTestHwDevice1Uid(TUid::Uid(0x102834C3)),iTestHwDevice2Uid(TUid::Uid(0x102834C4)),
iTestHwDevice3Uid(TUid::Uid(0x102834C5))
	{
	iTestStepName = aStepName;
	iTestState = E1_InitDevSound1;
	iStage = EPlayData;
	}

void RDevSoundPolicyUpdateTest::KickoffTestL()
	{
	INFO_PRINTF1(_L("Test case for - PDEF117811:Ref DevSound policy updating on error condition is inconsistent"));
	iDevSoundObserver1 = CTestDevSoundObserver::NewL(this, EObserver1);
	iDevSoundObserver2 = CTestDevSoundObserver::NewL(this, EObserver2);
	iMMFDevSound1 = CMMFDevSound::NewL();
	iMMFDevSound2 = CMMFDevSound::NewL();
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	iMMFDevSound1->InitializeL(*iDevSoundObserver1, iTestHwDevice1Uid, EMMFStatePlaying);
	#else
	TFourCC testUID1('T','S','T','1');
	INFO_PRINTF1(_L("First INIT"));
	iMMFDevSound1->InitializeL(*iDevSoundObserver1, testUID1, EMMFStatePlaying);
	#endif
	}

void RDevSoundPolicyUpdateTest::CloseTest()
	{
	delete iDevSoundObserver1;
	iDevSoundObserver1 = NULL;
	delete iDevSoundObserver2;
	iDevSoundObserver2 = NULL;	
	delete iMMFDevSound1;
	iMMFDevSound1 = NULL;
	delete iMMFDevSound2;
	iMMFDevSound2 = NULL;
	}

void RDevSoundPolicyUpdateTest::Fsm(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	TRAPD(err, FsmL(aClientId, aTestEvent, aError));
	if(err)
		{
		StopTest(err, EFail);
		}
	}

void RDevSoundPolicyUpdateTest::FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	switch(iStage)
		{
	case EPlayData:
		PlayData_FsmL(aClientId, aTestEvent, aError);
		break;
	case EStartPlay:
		StartPlay_FsmL(aClientId, aTestEvent, aError);
		break;
	case EStartRecord:
		StartRecord_FsmL(aClientId, aTestEvent, aError);
		break;
	case ERecordData:
		RecordData_FsmL(aClientId, aTestEvent, aError);
		break;
	case EStartPlayTune:
		StartPlayTone_FsmL(aClientId, aTestEvent, aError);
		break;
	case EStartDualPlayTone:
		StartPlayDualTone_FsmL(aClientId, aTestEvent, aError);
		break;
	case EStartPlayDTMFString:
		StartPlayDTMFString_FsmL(aClientId, aTestEvent, aError);
		break;
	case EStartPlayToneSequence:
		StartPlayToneSequence_FsmL(aClientId, aTestEvent, aError);
		break;
	default:
		StopTest(aError, EFail);
		}
	}
	
void RDevSoundPolicyUpdateTest::PlayData_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	switch(iTestState)
		{
	//test states for testing CMMFDevSoundSvrImp::PlayData	
	case E1_InitDevSound1:
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound1->PlayInitL();
			iTestState = E1_PlayInitDevSound1;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E1_PlayInitDevSound1:
		if(aClientId == EObserver1 && aTestEvent == EBTBF && aError == KErrNone)
			{
			//play data, this should result in error
			iMMFDevSound1->PlayData();
			iTestState = E1_Simulate_hw_ThisHwBufferFilled_error;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E1_Simulate_hw_ThisHwBufferFilled_error:	
		if(aClientId == EObserver1 && aTestEvent == EPlayError && aError == KErrAbort)
			{
			//the error condition is succesfully simulated; now try to init and play using another DevSound
			iMMFDevSound2->InitializeL(*iDevSoundObserver2, EMMFStatePlaying);
			iTestState = E1_InitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E1_InitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound2->PlayInitL();
			iTestState = E1_PlayInitDevSound2;
			}
		break;
	case E1_PlayInitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EBTBF && aError == KErrNone)
			{
			//stage1 of testing is success.
			INFO_PRINTF1(_L("Policy update test for PlayData - succeeded"));
			iMMFDevSound2->Stop();
			#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, iTestHwDevice2Uid, EMMFStatePlaying);
			#else
			TFourCC testUID2('T','S','T','2');
			INFO_PRINTF1(_L("Second INIT"));
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, testUID2, EMMFStatePlaying);
			#endif
			iTestState = E2_1_InitDevSound1;
			iStage = EStartPlay;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	default:
		StopTest(aError, EFail);
		}
	}

//test for errors from hwDevice->SetConfig and hwDevice->StartPlay from 
//CMMFDevSoundSvrImp::StartPlayDataL
void RDevSoundPolicyUpdateTest::StartPlay_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	switch(iTestState)
		{
	//test states for testing CMMFDevSoundSvrImp::StartPlayDataL	
	case E2_1_InitDevSound1:
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound1->PlayInitL();
			iTestState = E2_Simulate_hw_SetConfig_Error;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E2_Simulate_hw_SetConfig_Error:
		if(aClientId == EObserver1 && aTestEvent == EPlayError && aError == KErrAbort)
			{
			iMMFDevSound2->PlayInitL();
			iTestState = E2_1_PlayInitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E2_1_PlayInitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EBTBF && aError == KErrNone)
			{
			//stage2_1 success
			INFO_PRINTF1(_L("Policy update test for StartPlayDataL - hwDevice->SetConfig errror succeeded"));
			iMMFDevSound2->Stop();
			iMMFDevSound1->Stop();
			#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, iTestHwDevice3Uid, EMMFStatePlaying);
			#else
			TFourCC testUID3('T','S','T','3');
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, testUID3, EMMFStatePlaying);
			#endif
			iTestState = E2_2_InitDevSound1;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E2_2_InitDevSound1:	
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound1->PlayInitL();
			iTestState = E2_Simulate_hw_Start_Error;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E2_Simulate_hw_Start_Error:
		if(aClientId == EObserver1 && aTestEvent == EPlayError && aError == KErrAbort)
			{
			iMMFDevSound2->PlayInitL();
			iTestState = E2_2_PlayInitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E2_2_PlayInitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EBTBF && aError == KErrNone)
			{
			//stage2_2 success
			INFO_PRINTF1(_L("Policy update test for StartPlayDataL -hwDevice->Start error - succeeded"));
			iMMFDevSound2->Stop();
			iMMFDevSound1->Stop();
			//initiate next stage 3 
			#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, iTestHwDevice2Uid, EMMFStateRecording);
			#else
			TFourCC testUID2('T','S','T','2');
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, testUID2, EMMFStateRecording);
			#endif
			iTestState = E3_1_InitDevSound1;
			iStage = EStartRecord;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	default:
		StopTest(aError, EFail);		
		}
	}

void RDevSoundPolicyUpdateTest::StartRecord_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	switch(iTestState)
		{
	//test states for testing CMMFDevSoundSvrImp::StartRecordDataL	
	//stage2_1: tests SetRecordFormat(iRecordFormat); error	
	//- error for	SetDeviceRecordLevel(iGain); can't be tested;
	//as initialisation would have failed.
	//stage2_2: tests iCMMFHwDevice->Start(EDevEncode, EDevInFlow); error.	
	case E3_1_InitDevSound1:
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			//initialise devsound2 to be used to check if policy is available.
			iMMFDevSound2->InitializeL(*iDevSoundObserver2, EMMFStateRecording);
			iTestState = E3_1_InitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E3_1_InitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound1->RecordInitL();
			iTestState = E3_Simulate_hw_SetConfig_Error;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E3_Simulate_hw_SetConfig_Error:
		if(aClientId == EObserver1 && aTestEvent == ERecordError && aError == KErrAbort)
			{
			iMMFDevSound2->RecordInitL();
			iTestState = E3_1_PlayInitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case  E3_1_PlayInitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EBTBE && aError == KErrNone)
			{
			//stage3_1 test is success
			iMMFDevSound2->Stop();
			#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, iTestHwDevice3Uid, EMMFStateRecording);
			#else
			TFourCC testUID3('T','S','T','3');
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, testUID3, EMMFStateRecording);
			#endif
			iTestState = E3_2_InitDevSound1;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E3_2_InitDevSound1:
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound1->RecordInitL();	
			iTestState = E3_Simulate_hw_Start_Error;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E3_Simulate_hw_Start_Error:
		if(aClientId == EObserver1 && aTestEvent == ERecordError && aError == KErrAbort)
			{
			iMMFDevSound2->RecordInitL();
			iTestState = E3_2_PlayInitDevSound1;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E3_2_PlayInitDevSound1:
		if(aClientId == EObserver2 && aTestEvent == EBTBE && aError == KErrNone)
			{
			//stage3_2 test is success
			iMMFDevSound2->Stop();
			iMMFDevSound1->Stop(); //start next stage of test newly
			INFO_PRINTF1(_L("Policy update test for StartRecord - succeeded"));
			#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, iTestHwDevice1Uid, EMMFStateRecording);
			#else
			TFourCC testUID1('T','S','T','1');
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, testUID1, EMMFStateRecording);
			#endif
			iTestState = E4_InitDevSound1;
			iStage = ERecordData;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	default:
		StopTest(aError, EFail);		
		}
	}

void RDevSoundPolicyUpdateTest::RecordData_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	switch(iTestState)
		{
	case E4_InitDevSound1:
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound1->RecordInitL();
			iTestState = E4_RecordInitDevSound1;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E4_RecordInitDevSound1:
		if(aClientId == EObserver1 && aTestEvent == EBTBE && aError == KErrNone)
			{
			iMMFDevSound1->RecordData();
			iTestState = E4_Simulate_hw_ThisHwBufferEmptied_error;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E4_Simulate_hw_ThisHwBufferEmptied_error:
		if(aClientId == EObserver1 && aTestEvent == ERecordError && aError == KErrAbort)
			{
			iMMFDevSound2->RecordInitL();
			iTestState = E4_RecordInitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E4_RecordInitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EBTBE && aError == KErrNone)
			{
			//stage4 test is success.
			iMMFDevSound1->Stop();
			iMMFDevSound2->Stop();
			INFO_PRINTF1(_L("Policy update test for RecordData - succeeded"));
			#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, iTestHwDevice2Uid, EMMFStateTonePlaying);
			#else
			//At this point the test will be stoped  because InitializeL(MDevSoundObserver &aDevSoundObserver, TFourCC aFourCC, TMMFState aMode); 
			//Is not valid for tone play on A3F
			INFO_PRINTF1(_L("The next tests will be skipped because InitializeL(); using FourCC is not valid for tone play in A3F"));
			StopTest(EPass);
			#endif
			iTestState = E5_InitDevSound1;
			iStage = EStartPlayTune;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	default:
		StopTest(aError, EFail);
		};
	}

void RDevSoundPolicyUpdateTest::StartPlayTone_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	TInt toneFreq = 1000;
	TTimeIntervalMicroSeconds duration = 1000000;
	switch(iTestState)
		{
	case E5_InitDevSound1:
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound1->PlayToneL(toneFreq, duration);
			iTestState = E5_Simulate_hw_SetConfig_error;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;	
	case E5_Simulate_hw_SetConfig_error:
		if(aClientId == EObserver1 && aTestEvent == ETonePlayError && aError == KErrAbort )
			{
			iMMFDevSound2->InitializeL(*iDevSoundObserver2, EMMFStatePlaying);
			iTestState = E5_InitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E5_InitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound2->PlayInitL();
			iTestState = E5_PlayInitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E5_PlayInitDevSound2:	
		if(aClientId == EObserver2 && aTestEvent == EBTBF && aError == KErrNone)
			{
			//stage5 test success for hw-SetConfig error. i.e. 
			//CMMFDevSoundSvrImp::SetPlayFormat error.
			iMMFDevSound2->Stop();
			iMMFDevSound1->Stop();
			INFO_PRINTF1(_L("Policy update test for StartPlayTone - succeeded"));
			iMMFDevSound1->InitializeL(*iDevSoundObserver1, iTestHwDevice2Uid, EMMFStateTonePlaying);
			iTestState = E6_InitDevSound1;
			iStage = EStartDualPlayTone;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
		}
	}

void RDevSoundPolicyUpdateTest::StartPlayDualTone_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	TInt freq1 = 1000;
	TInt freq2 = 2000;
	TTimeIntervalMicroSeconds duration = 1000000;
	//for initiating next stage.
	TTimeIntervalMicroSeconds32 onLength = 1000;
	TTimeIntervalMicroSeconds32 offLength = 1000;
	TTimeIntervalMicroSeconds32 pauseLength = 500;
	TBuf<100> dummy;
	
	switch(iTestState)
		{
	case E6_InitDevSound1:
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			iMMFDevSound1->PlayDualToneL(freq1, freq2, duration);
			iTestState = E6_Simulate_hw_SetConfig_error;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E6_Simulate_hw_SetConfig_error:
		if(aClientId == EObserver1 && aTestEvent == ETonePlayError && aError == KErrAbort )
			{
			iMMFDevSound2->PlayInitL();
			iTestState = E6_PlayInitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E6_PlayInitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EBTBF && aError == KErrNone)
			{
			//stage6 success
			iMMFDevSound1->Stop();
			iMMFDevSound2->Stop();
			INFO_PRINTF1(_L("Policy update test for StartPlayDualTone - succeeded"));
			iMMFDevSound1->SetDTMFLengths(onLength, offLength, pauseLength);
			iMMFDevSound1->PlayDTMFStringL(dummy);
			iTestState = E7_Simulate_hw_SetConfig_error;
			iStage = EStartPlayDTMFString;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
		}
	}

void RDevSoundPolicyUpdateTest::StartPlayDTMFString_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	TBuf8<100> dummy; //to initiate next stage
	dummy.Format(_L8("SQNC%d"),TUint8(0x0a));
	switch(iTestState)
		{
	case E7_Simulate_hw_SetConfig_error:
		if(aClientId == EObserver1 && aTestEvent == ETonePlayError && aError == KErrAbort )
			{
			iMMFDevSound2->PlayInitL();
			iTestState = E7_PlayInitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E7_PlayInitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EBTBF && aError == KErrNone)
			{
			//stage7 test success
			iMMFDevSound1->Stop();
			iMMFDevSound2->Stop();
			INFO_PRINTF1(_L("Policy update test for StartPlayDTMFString - succeeded"));
			iMMFDevSound1->PlayToneSequenceL(dummy);
			iTestState = E8_Simulate_hw_SetConfig_error;
			iStage = EStartPlayToneSequence;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
		}
	}

void RDevSoundPolicyUpdateTest::StartPlayToneSequence_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError)
	{
	switch(iTestState)
		{
	case E8_Simulate_hw_SetConfig_error:
	// Only HwDevice with UID= KMmfUidHwDevicePCM16ToPCM16 is valid.
	// since test hwDevice is used will get KErrGeneral.
		if(aClientId == EObserver1 && aTestEvent == ETonePlayError && aError == KErrGeneral )
			{
			iMMFDevSound2->PlayInitL();
			iTestState = E8_PlayInitDevSound2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case E8_PlayInitDevSound2:
		if(aClientId == EObserver2 && aTestEvent == EBTBF && aError == KErrNone)
			{
			INFO_PRINTF1(_L("Policy update test for StartPlayToneSequence - succeeded"));
			StopTest(EPass);
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
		}
	}


const TUint8 KFixedSequenceTestSequenceData[]=
	{
	0x53, 0x51, 0x4E, 0x43, // HEADER
	0xFE, 0xFF, // -2 STARTLOOP INDICATOR
	0x0A, 0x00,  // Number of times that the sequence will be played
	/* Tone Data constitutes the following:
	  ToneDur,    Freq1,      Vol1,       Freq2       Vol2
	The sequence is arranged in blocks of 8 bits to represent 16 bit values, 
	which allows more dynamic range, frequency and time values.
	The tag Low and High mean low and high byte block.
	Low    Hi    Low   Hi   Low   Hi    Low   Hi    Low   Hi
	*/
	0x64, 0x00, 0xB8, 0x01, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0xD2, 0x01, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0xEE, 0x01, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x0B, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x2A, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x4B, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x6E, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x93, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0xBA, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0xE4, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x3F, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x70, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0xA4, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0xDC, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x17, 0x04, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
	// Silence
	// ToneDur,    Freq1,      Vol1,       Freq2       Vol2
	0x70, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFD, 0xFF, // -3 ENDOFLOOP INDICATOR
	// Another silence
	// ToneDur,    Freq1,      Vol1,       Freq2       Vol2
	0x70, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF // -1 SEQ_CMD_RET INDICATOR
	};


RDevSoundTestBase::RDevSoundTestBase(const TDesC& aTestName)
	:	iDevSoundState(EStateInitial), 
		iMMFDevSound(NULL),
		iTimer(NULL)
	{
	iTestStepName = aTestName;
	}

void RDevSoundTestBase::KickoffTestL()
	{
	INFO_PRINTF1(_L("__________  Creating DevSound object ___________"));
	
	// Create devsound object	
	TRAPD(err,  iMMFDevSound = CMMFDevSound::NewL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create DevSound object. Error = %d"), err);
		StopTest(err);
		return;
		}
	INFO_PRINTF1(_L("DevSound State: EStateCreated"));
	iDevSoundState = EStateCreated;

	DoKickoffTestL();
	
	INFO_PRINTF1(_L("DevSound Event: EEventInitialize"));
	Fsm(EEventInitialize, KErrNone);
	}

void RDevSoundTestBase::CloseTest()
	{
	INFO_PRINTF1(KMsgDeleteDevsound);
	delete iMMFDevSound;
	delete iTimer;
	}
	
/*
 *
 * CallStopTest
 *
 */
void RDevSoundTestBase::CallStopTest(TInt aError)
	{
	StopTest(aError);
	}


void RDevSoundTestBase::StartTimer(TTimeIntervalMicroSeconds32 aWaitTime)
	{
	TTimeIntervalMicroSeconds32 timeInterval;
	
	if(aWaitTime <= TTimeIntervalMicroSeconds32(0))
		{
		timeInterval = KMicroSecsInOneSec;
		}
	else
		{
		timeInterval = aWaitTime;
		}
	TCallBack callback (TimerCallback, this);
	iTimer->Start(timeInterval, timeInterval, callback);
	INFO_PRINTF1(_L("Timer has been started"));
	}

TInt RDevSoundTestBase::TimerCallback(TAny* aPtr)
	{
	static_cast<RDevSoundTestBase*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

void RDevSoundTestBase::DoTimerCallback()
	{
	__ASSERT_ALWAYS(0, Panic(EInvalidCallbackCall));	
	}

void RDevSoundTestBase::InitializeComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound InitializeComplete() callback =========="));
	INFO_PRINTF3(KMsgErrorDevSoundCallback, &KInitializeCompleteText, aError);
	if(iDevSoundState == EStateInitializing)
		{
		INFO_PRINTF1(_L("DevSound Event: EEventInitComplete"));
		Fsm(EEventInitComplete, aError);
		}
	}

void RDevSoundTestBase::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		INFO_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished with error = %d as expected"), aError);
		StopTest(aError,EPass);
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError);
		}
	}

void RDevSoundTestBase::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0, Panic(EInvalidCallbackCall));
	}

void RDevSoundTestBase::PlayError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(EInvalidCallbackCall));
	}

void RDevSoundTestBase::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0, Panic(EInvalidCallbackCall));
	}

void RDevSoundTestBase::RecordError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(EInvalidCallbackCall));
	}

void RDevSoundTestBase::ConvertError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(EInvalidCallbackCall));
	}

void RDevSoundTestBase::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	__ASSERT_ALWAYS(0, Panic(EInvalidCallbackCall));
	}

void RDevSoundTestBase::SendEventToClient(const TMMFEvent& aEvent)
	{
	INFO_PRINTF3(_L("RDevSoundTestBase::SendEventToClient type=0x%08x errorCode=%d"),aEvent.iEventType, aEvent.iErrorCode);
	}

void RDevSoundTestBase::Panic(TInt aReason)
	{
	_LIT(KDevSoundVolumeRampTestBase, "RDevSoundVolumeRampTestBase");
	User::Panic(KDevSoundVolumeRampTestBase, aReason);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0075-HP
 *========================================================================================================
 */
RDevSoundDualToneVolumeRampTest::RDevSoundDualToneVolumeRampTest(const TDesC& aTestName)
	:	RDevSoundTestBase(aTestName), iVolumeRamp(0), 
		iDuration(0), iFrequencyTone1(0),
		iFrequencyTone2(0)
	{
	}

RDevSoundDualToneVolumeRampTest* RDevSoundDualToneVolumeRampTest::NewL(const TDesC& aTestName)
	{
	RDevSoundDualToneVolumeRampTest * self = new(ELeave)RDevSoundDualToneVolumeRampTest(aTestName);
	return self;
	}

void RDevSoundDualToneVolumeRampTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolumeRamp, iVolumeRamp)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolumeRamp);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrPathNotFound);
		return;
		}
	}

void RDevSoundDualToneVolumeRampTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(EFsmIncorrectErrorPassed));
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
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetVolume using MaxVolume"));
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				
				INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolumeRamp using volume ramp time = %d"), iVolumeRamp);
				iMMFDevSound->SetVolumeRamp(iVolumeRamp*KMicroSecsInOneSec);
				
				TRAPD(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1, iFrequencyTone2, iDuration*KMicroSecsInOneSec));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL left with error : %d!"), err);
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
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0076-HP
 *========================================================================================================
 */
RDevSoundDTMFStringVolumeRampTest::RDevSoundDTMFStringVolumeRampTest(const TDesC& aTestName)
	:	RDevSoundTestBase(aTestName), iVolumeRamp(0), 
	iDTMFString(KNullDesC)
	{
	}

RDevSoundDTMFStringVolumeRampTest* RDevSoundDTMFStringVolumeRampTest::NewL(const TDesC& aTestName)
	{
	RDevSoundDTMFStringVolumeRampTest * self = new(ELeave)RDevSoundDTMFStringVolumeRampTest(aTestName);
	return self;
	}

void RDevSoundDTMFStringVolumeRampTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolumeRamp, iVolumeRamp)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolumeRamp);
		StopTest(KErrPathNotFound);
		return;
		}
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrPathNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	}

void RDevSoundDTMFStringVolumeRampTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(EFsmIncorrectErrorPassed));
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
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetVolume using MaxVolume"));
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
				
				INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolumeRamp using volume ramp time = %d"), iVolumeRamp);
				iMMFDevSound->SetVolumeRamp(iVolumeRamp*KMicroSecsInOneSec);

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
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0077-HP
 *========================================================================================================
 */
RDevSoundToneSequenceVolumeRampTest::RDevSoundToneSequenceVolumeRampTest(const TDesC& aTestName)
	:	RDevSoundTestBase(aTestName), iVolumeRamp(0)
	{
	}

RDevSoundToneSequenceVolumeRampTest* RDevSoundToneSequenceVolumeRampTest::NewL(const TDesC& aTestName)
	{
	RDevSoundToneSequenceVolumeRampTest * self = new(ELeave)RDevSoundToneSequenceVolumeRampTest(aTestName);
	return self;
	}

void RDevSoundToneSequenceVolumeRampTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolumeRamp, iVolumeRamp)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolumeRamp);
		StopTest(KErrPathNotFound);
		return;
		}
	}

void RDevSoundToneSequenceVolumeRampTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(EFsmIncorrectErrorPassed));
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
				INFO_PRINTF1(_L("Calling CMMFDevSound::SetVolume using MaxVolume"));
				iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());

				INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolumeRamp using volume ramp time = %d"), iVolumeRamp);
				iMMFDevSound->SetVolumeRamp(iVolumeRamp*KMicroSecsInOneSec);
				
				TUint8* tablePointer = const_cast<TUint8*>(&(KFixedSequenceTestSequenceData[0])); 
				TPtrC8 KFixedSequenceData(tablePointer,  sizeof(KFixedSequenceTestSequenceData));
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
				TRAPD(err, iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL left with error : %d!"), err);
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
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0078-HP
 *========================================================================================================
 */
RDevSoundDualToneSetVolumeRampWhilePlayingTest::RDevSoundDualToneSetVolumeRampWhilePlayingTest(const TDesC& aTestName)
	:	RDevSoundTestBase(aTestName), 
		iVolume(0), iVolumeRamp(0), 
		iDuration(0), iFrequencyTone1(0), iFrequencyTone2(0), 
		iTonePlayedTwice(EFalse),
		iTimesToneFinishedCalled(0)
	{
	}

RDevSoundDualToneSetVolumeRampWhilePlayingTest* RDevSoundDualToneSetVolumeRampWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RDevSoundDualToneSetVolumeRampWhilePlayingTest * self = new(ELeave)RDevSoundDualToneSetVolumeRampWhilePlayingTest(aTestName);
	return self;
	}

void RDevSoundDualToneSetVolumeRampWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KVolumeRamp, iVolumeRamp)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolumeRamp);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrPathNotFound);
		return;
		}

	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RDevSoundDualToneSetVolumeRampWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(EFsmIncorrectErrorPassed));
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
				StopTest(KErrGeneral);
			}
		}
	}

void RDevSoundDualToneSetVolumeRampWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolumeRamp using volume ramp time = %d"), iVolumeRamp);
	iMMFDevSound->SetVolumeRamp(iVolumeRamp*KMicroSecsInOneSec);
	}

void RDevSoundDualToneSetVolumeRampWhilePlayingTest::ToneFinished(TInt aError)
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
			INFO_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished with error = %d"), aError);
			INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using frequency = %d Hz and duration = %d secs "), iFrequencyTone1, iDuration);
			TRAPD(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1, iFrequencyTone2, iDuration*KMicroSecsInOneSec));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL with error : %d!"), err);
				StopTest(err);
				}
			iTonePlayedTwice = ETrue;
			}
		else if (iPlayToneTime < totalExpectedReproductionTime + KPlayVarianceTime && iPlayToneTime > totalExpectedReproductionTime - KPlayVarianceTime)
			{
			INFO_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished twice with error = %d"), aError);
			INFO_PRINTF2(_L("Play time is %Ld microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			INFO_PRINTF3(_L("Play time is %Ld microseconds and it should have been %d microseconds"), iPlayToneTime.Int64(), totalExpectedReproductionTime);
			StopTest(aError);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError);
		}
	}


/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0079-HP
 *========================================================================================================
 */
RDevSoundDTMFStringSetVolumeRampWhilePlayingTest::RDevSoundDTMFStringSetVolumeRampWhilePlayingTest(const TDesC& aTestName)
	:	RDevSoundTestBase(aTestName), 
		iVolume(0), iVolumeRamp(0),
		iFirstToneFinishedCall(EFalse),
		iDTMFString(KNullDesC),
		iDTMFPauses(0), iDTMFTones(0),
		iTimesToneFinishedCalled(0)
	{
	}

RDevSoundDTMFStringSetVolumeRampWhilePlayingTest* RDevSoundDTMFStringSetVolumeRampWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RDevSoundDTMFStringSetVolumeRampWhilePlayingTest * self = new(ELeave)RDevSoundDTMFStringSetVolumeRampWhilePlayingTest(aTestName);
	return self;
	}

void RDevSoundDTMFStringSetVolumeRampWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KVolumeRamp, iVolumeRamp)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolumeRamp);
		StopTest(KErrPathNotFound);
		return;
		}
	TPtrC DTMF;
	if (!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrPathNotFound);
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

void RDevSoundDTMFStringSetVolumeRampWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(EFsmIncorrectErrorPassed));
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
				INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error : %d!"), err);
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
			StopTest(KErrGeneral);
			}
		}
	}

void RDevSoundDTMFStringSetVolumeRampWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolumeRamp using volume ramp time = %d"), iVolumeRamp);
	iMMFDevSound->SetVolumeRamp(iVolumeRamp*KMicroSecsInOneSec);
	}

void RDevSoundDTMFStringSetVolumeRampWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		iTimesToneFinishedCalled++;
		iEndTime.HomeTime();
		iPlayToneTime = iEndTime.MicroSecondsFrom(iStartTime);
		INFO_PRINTF2(_L("Play time is %d microseconds"), iPlayToneTime.Int64());
		
		TInt expectedPlayToneTime = (KDefaultOnDTMFLength*iDTMFTones + KDefaultOffDTMFLength*iDTMFTones + iDTMFPauses*KDefaultPauseDTMFLength) * iTimesToneFinishedCalled;
		INFO_PRINTF2(_L("Expected play time is %d microseconds"), expectedPlayToneTime);
		
		if (!iFirstToneFinishedCall)
			{
			INFO_PRINTF1(_L("First play tone succeded "));
			iFirstToneFinishedCall = ETrue;
			INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"), &iDTMFString);
			TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL left with error = %d"), err);
				StopTest(err);
				}
			
			return;
			}
		if ( Abs(iPlayToneTime.Int64() - expectedPlayToneTime) < KPlayVarianceTime )
			{
			INFO_PRINTF1(_L("Calling CMMFDevSound::SetDTMFLengths succeded."));
			INFO_PRINTF2(_L("Play time is %d microseconds"), iPlayToneTime.Int64());
			StopTest();
			}
		else
			{
			ERR_PRINTF2(_L("Calling CMMFDevSound::SetDTMFLengths failed with %Ld"), iPlayToneTime.Int64());
			ERR_PRINTF2(_L("Expected time %d"), expectedPlayToneTime);
			StopTest(aError, EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0080-HP
 *========================================================================================================
 */
RDevSoundToneSequenceSetVolumeRampWhilePlayingTest::RDevSoundToneSequenceSetVolumeRampWhilePlayingTest(const TDesC& aTestName)
	:	RDevSoundTestBase(aTestName), 
		iVolume(0), iVolumeRamp(0), 
		iFirstToneFinishedCall(EFalse)
	{
	}

RDevSoundToneSequenceSetVolumeRampWhilePlayingTest* RDevSoundToneSequenceSetVolumeRampWhilePlayingTest::NewL(const TDesC& aTestName)
	{
	RDevSoundToneSequenceSetVolumeRampWhilePlayingTest * self = new(ELeave)RDevSoundToneSequenceSetVolumeRampWhilePlayingTest(aTestName);
	return self;
	}

void RDevSoundToneSequenceSetVolumeRampWhilePlayingTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KVolumeRamp, iVolumeRamp)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolumeRamp);
		StopTest(KErrPathNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RDevSoundToneSequenceSetVolumeRampWhilePlayingTest::Fsm(TMmfDevSoundEvent aDevSoundEvent,  TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent == EEventInitialize)
				{
				__ASSERT_ALWAYS((aError == KErrNone), Panic(EFsmIncorrectErrorPassed));
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
				TUint8* tablePointer = const_cast<TUint8*>(&(KFixedSequenceTestSequenceData[0])); 
				TPtrC8 KFixedSequenceData(tablePointer,  sizeof(KFixedSequenceTestSequenceData));
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
				TRAPD(err, iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("Play tone sequence failed with error : %d!"), err);
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
			StopTest(KErrGeneral);
			}
		}
	}

void RDevSoundToneSequenceSetVolumeRampWhilePlayingTest::DoTimerCallback()
	{
	iTimer->Cancel();
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	INFO_PRINTF2(_L("Calling CMMFDevSound::SetVolumeRamp using volume ramp time = %d"), iVolumeRamp);
	iMMFDevSound->SetVolumeRamp(iVolumeRamp*KMicroSecsInOneSec);
	}

void RDevSoundToneSequenceSetVolumeRampWhilePlayingTest::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		if (!iFirstToneFinishedCall)
			{
			INFO_PRINTF1(_L("First play tone succeded"));

			iFirstToneFinishedCall = ETrue;

			TUint8* tablePointer = const_cast<TUint8*>(&(KFixedSequenceTestSequenceData[0])); 
			TPtrC8 KFixedSequenceData(tablePointer,  sizeof(KFixedSequenceTestSequenceData));
			INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
			TRAPD(err, iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL left with error : %d!"), err);
				StopTest(err);
				}
			}
		else if (iFirstToneFinishedCall)
			{
			INFO_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished twice with error = %d"), aError);
			StopTest();
			}
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError);
		}
	}

//PDEF121315: Observer class for the test
CMultiInitTestDevSoundObserver* CMultiInitTestDevSoundObserver::NewL(RDevSoundMultiInitTest* aParent, RDevSoundMultiInitTest::EClientId aClientId)
	{
	CMultiInitTestDevSoundObserver* self = new(ELeave)CMultiInitTestDevSoundObserver(aParent, aClientId);
	return self;
	}

CMultiInitTestDevSoundObserver::CMultiInitTestDevSoundObserver(RDevSoundMultiInitTest* aParent, RDevSoundMultiInitTest::EClientId aClientId):
iParent(aParent), iSelfId(aClientId)
	{
	}

void CMultiInitTestDevSoundObserver::InitializeComplete(TInt aError)
	{
	iParent->Fsm(iSelfId, RDevSoundMultiInitTest::EDevSoundInitComplete, aError);
	}

void CMultiInitTestDevSoundObserver::ToneFinished(TInt /*aError*/)
	{
	}

void CMultiInitTestDevSoundObserver::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	iParent->Fsm(iSelfId, RDevSoundMultiInitTest::EBTBF, KErrNone);
	}

void CMultiInitTestDevSoundObserver::PlayError(TInt /*aError*/)
	{	
	}

void CMultiInitTestDevSoundObserver::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	}

void CMultiInitTestDevSoundObserver::RecordError(TInt /*aError*/)
	{
	}

void CMultiInitTestDevSoundObserver::ConvertError(TInt /*aError*/)
	{
	}

void CMultiInitTestDevSoundObserver::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	}



//PDEF121315: test class implementation
/*
 * A brief explanation of the test
 * 
 * The Test tests all 3 overloads of CMMFDevSound::InitializeL().
 * The tests involves a CMMFDevSound object and two observer objects - Observer1 and Observer2
 * of type MDevSoundObserver.
 * Using each InitializeL(), DevSound is intialised first with Observer1.
 * Upon getting the InitializeComplete() callback, Observer1 is deleted and
 * DevSound is initialised again with Observer2. Now the the callback should be from Observer2
 * and there should not be any Panic.
 */

RDevSoundMultiInitTest* RDevSoundMultiInitTest::NewL(const TDesC& aStepName)
	{
	RDevSoundMultiInitTest* self = new(ELeave)RDevSoundMultiInitTest(aStepName);
	return self;
	}

RDevSoundMultiInitTest::RDevSoundMultiInitTest(const TDesC& aStepName)
	{
	iTestStepName = aStepName;
	iTestState = EInit1Observer1;
	}

void RDevSoundMultiInitTest::KickoffTestL()
	{
	INFO_PRINTF1(_L("Test case for - PDEF121315:DevSound can't update Observer without delete"));
	iDevSoundObserver1 = CMultiInitTestDevSoundObserver::NewL(this, EObserver1);
	iMMFDevSound = CMMFDevSound::NewL();
	iMMFDevSound->InitializeL(*iDevSoundObserver1, EMMFStatePlaying);
	}

void RDevSoundMultiInitTest::CloseTest()
	{
	delete iDevSoundObserver1;
	iDevSoundObserver1 = NULL;
	delete iDevSoundObserver2;
	iDevSoundObserver2 = NULL;	
	delete iMMFDevSound;
	iMMFDevSound = NULL;
	}

void RDevSoundMultiInitTest::Fsm(EClientId aClientId, EMultiInitTestEvent aTestEvent, TInt aError)
	{
	TRAPD(err, FsmL(aClientId, aTestEvent, aError));
	if(err)
		{
		StopTest(err, EFail);
		}
	}

void RDevSoundMultiInitTest::FsmL(EClientId aClientId, EMultiInitTestEvent aTestEvent, TInt aError)
	{
	switch(iTestState)
		{
	case EInit1Observer1:
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			INFO_PRINTF1(_L("Testing Initlize-1; got InitComplete from Observer1"));
			//INC120815 - moved the deletion part below so that iDevSoundObserver2 is created at the different address than 
			//that of iDevSoundObserver1. Before iDevSoundObserver1 and iDevSoundObserver2 are getting created at the same address and hence
			//this defect was not observed. This applies for the other cases in this switch
			iDevSoundObserver2 = CMultiInitTestDevSoundObserver::NewL(this, EObserver2);
			delete iDevSoundObserver1;
			iDevSoundObserver1 = NULL;
			iMMFDevSound->InitializeL(*iDevSoundObserver2, EMMFStatePlaying); 
			INFO_PRINTF1(_L("Initialising with Observer2"));
			iTestState = EInit1Observer2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case EInit1Observer2:
		if(aClientId == EObserver2 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			INFO_PRINTF1(_L("Testing Initialize-1; got InitComplete from  Observer2"));
			//Intialize-1 test is success; test Initialize-2
			iDevSoundObserver1 = CMultiInitTestDevSoundObserver::NewL(this, EObserver1);
			delete iDevSoundObserver2;
			iDevSoundObserver2 = NULL;
			iMMFDevSound->InitializeL(*iDevSoundObserver1, KMMFFourCCCodePCM8, EMMFStatePlaying);
			iTestState = EInit2Observer1;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case EInit2Observer1:	
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			INFO_PRINTF1(_L("Testing Initlize-2; got InitComplete from Observer1"));
			iDevSoundObserver2 = CMultiInitTestDevSoundObserver::NewL(this, EObserver2);
			delete iDevSoundObserver1;
			iDevSoundObserver1 = NULL;
			iMMFDevSound->InitializeL(*iDevSoundObserver2, KMMFFourCCCodePCM8, EMMFStatePlaying);
			INFO_PRINTF1(_L("Initialising with Observer2"));
			iTestState = EInit2Observer2;
			}
		else
			{
			StopTest(aError, EFail);
			}
		break;
	case EInit2Observer2:
		if(aClientId == EObserver2 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			INFO_PRINTF1(_L("Testing Initialize-2; got InitComplete from  Observer2"));
			//Initialize-2 test is success; test Initialize-3
			iDevSoundObserver1 = CMultiInitTestDevSoundObserver::NewL(this, EObserver1);
			delete iDevSoundObserver2;
			iDevSoundObserver2 = NULL;
			iMMFDevSound->InitializeL(*iDevSoundObserver1, KMMFFourCCCodePCM16, EMMFStatePlaying);
			iTestState = EInit3Observer1;
			}
		else
			{
			StopTest(aError, EFail);
			}			
		break;
	case EInit3Observer1:
		if(aClientId == EObserver1 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			INFO_PRINTF1(_L("Testing Initialize-3; got InitComplete from  Observer1"));
			iDevSoundObserver2 = CMultiInitTestDevSoundObserver::NewL(this, EObserver2);
			delete iDevSoundObserver1;
			iDevSoundObserver1 = NULL;
			iMMFDevSound->InitializeL(*iDevSoundObserver2, KMMFFourCCCodePCM16, EMMFStatePlaying);
			iTestState = EInit3Observer2;
			}
		else
			{
			StopTest(aError, EFail);
			}			
		break;
	case EInit3Observer2:
		if(aClientId == EObserver2 && aTestEvent == EDevSoundInitComplete && aError == KErrNone)
			{
			StopTest(EPass);
			}
		else
			{
			StopTest(aError, EFail);
			}			
		break;
	default:
		StopTest(aError, EFail);
		};
	}

//
//
//							TruePause Devsound									//
//
//

/*
 *========================================================================================================
 * Global declarations
 *========================================================================================================
 */

const TUint8 KFixedSequenceTestSequenceDataX[]=
    {
    0x53, 0x51, 0x4E, 0x43, // HEADER
    0xFE, 0xFF, // -2 STARTLOOP INDICATOR
    0x0A, 0x00,  // Number of times that the sequence will be played
    /* Tone Data constitutes the following:
      ToneDur,    Freq1,      Vol1,       Freq2       Vol2
    The sequence is arranged in blocks of 8 bits to represent 16 bit values, 
   	which allows more dynamic range, frequency and time values.
    The tag Low and High mean low and high byte block.
    Low    Hi    Low   Hi   Low   Hi    Low   Hi    Low   Hi
    */
    0x64, 0x00, 0xB8, 0x01, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0xD2, 0x01, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0xEE, 0x01, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x0B, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x2A, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x4B, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x6E, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x93, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0xBA, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0xE4, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x3F, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x70, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0xA4, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0xDC, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x17, 0x04, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
    // Silence
    // ToneDur,    Freq1,      Vol1,       Freq2       Vol2
    0x70, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFD, 0xFF, // -3 ENDOFLOOP INDICATOR
    // Another silence
    // ToneDur,    Freq1,      Vol1,       Freq2       Vol2
    0x70, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF // -1 SEQ_CMD_RET INDICATOR
    };

/*
 *========================================================================================================
 * Audio play client
 *========================================================================================================
 */

CA3FDevSoundPlayClient::CA3FDevSoundPlayClient(MA3FDevsoundPlayAudioClientObserver &aObserver, TFourCC aFourccCode, RFs &aFs, CTestSuite* aSuite)
	:	iDevSoundState(EStateInitial), 
		iDevSound(NULL), 
		iObserver(aObserver),
		iFilename(KNullDesC),
		iFs(aFs),
		iFourccCode(aFourccCode),
		iSuite(aSuite)
	{
	}

CA3FDevSoundPlayClient::~CA3FDevSoundPlayClient()
	{
	delete iDevSound;
	iFile.Close();
	}

CA3FDevSoundPlayClient* CA3FDevSoundPlayClient::NewL(MA3FDevsoundPlayAudioClientObserver &aObserver, TFourCC aFourccCode, RFs &aFs, CTestSuite* aSuite)
	{
	CA3FDevSoundPlayClient* self = new (ELeave) CA3FDevSoundPlayClient(aObserver,aFourccCode ,aFs, aSuite);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CA3FDevSoundPlayClient::ConstructL()
	{   
	iDevSound = CMMFDevSound::NewL();
	iDevSoundState = EStateCreated;
	}

void CA3FDevSoundPlayClient::SetPriority(TInt priority)
	{   
	TMMFPrioritySettings settings;
	settings.iPriority = priority;
	settings.iPref = EMdaPriorityPreferenceTime;
	iDevSound->SetPrioritySettings(settings);
	}

TInt CA3FDevSoundPlayClient::PlayInit(const TDesC& aFileName)
	{
	iFilename.Copy(aFileName);
	TInt err  = iFile.Open(iFs, iFilename, EFileRead);
	return Fsm(EEventInitialize);
	}

TInt CA3FDevSoundPlayClient::PlayAudio()
	{
	return Fsm(EEventInitComplete);
	}

void CA3FDevSoundPlayClient::StopPlay()
	{
	iDevSound->Stop();
	}

void CA3FDevSoundPlayClient::InitializeComplete(TInt aError)
	{
	iObserver.ClientInitializeCompleteCallback(aError);
	}

void CA3FDevSoundPlayClient::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	TInt err;
	if (!aBuffer)
		{
		err = KErrGeneral;	// Null Buffer was received
		}
	else
		{
		iBuffer = aBuffer;	
		err = Fsm(EEventBTBF);
		}
	iObserver.ClientBufferToBeFilledCallback(err);
	}

void CA3FDevSoundPlayClient::PlayError(TInt aError)
	{
	if (iBuffer)
		{
		iObserver.ClientPlayErrorCallback(aError, iBuffer->LastBuffer());
		}
	else
		{
		iObserver.ClientPlayErrorCallback(aError, EFalse);
		}
	}

TInt CA3FDevSoundPlayClient::Fsm(TMmfDevSoundEvent aDevSoundEvent)
	{
	TInt err;
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
			if(aDevSoundEvent == EEventInitialize)
				{
				TRAP(err, iDevSound->InitializeL(*this, iFourccCode, EMMFStatePlaying));
				if(err == KErrNone)
					{
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Leave Error when calling InitializeL() on second DevSound instance: %d"),err);
					}
				}
			else
				{
				err=EInvalidClientFSMEvent;
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete)
				{
				iDevSound->SetVolume(iDevSound->MaxVolume());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAP(err, iDevSound->PlayInitL());
				if(err == KErrNone)
					{
					INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
					iDevSoundState = EStatePlaying;
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Leave Error when calling PlayInitL() on second DevSound instance: %d"),err);
					}
				}
			else
				{
				err=EInvalidClientFSMEvent;
				}
			break;
			}
		case EStatePlaying:
			{
			if(aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					err=EReadFileErrorInClient;
					iObserver.ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(EReadFileErrorInClient);
					break;
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				iDevSound->PlayData();
				}
			else
				{
				iObserver.ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(EInvalidClientFSMEvent);
				}
			break;
			}
		default:
			{
			iObserver.ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(EInvalidClientFSMState);
			}
		}
	return err;
	}


void CA3FDevSoundPlayClient::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::RecordError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::ConvertError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::ToneFinished(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt,...)
	{
	if(!iSuite)
		{
		return;
		}
	VA_LIST list;
	VA_START(list, aFmt);
	TIntegrationTestLog16OverflowClient overflow16;
	// decode formatted data for display on console
	TBuf<KMaxLogLineLength> lineBuf;
	lineBuf.AppendFormatList(aFmt, list, &overflow16);
	// send the data to the log system via the suite
	if(LogSeverity::IsActive(aSeverity, iSuite->Severity()))
		{
		iSuite->LogExtra(aFile, aLine, aSeverity, lineBuf);
		}
	VA_END(list); 
	}

void TIntegrationTestLog16OverflowClient::Overflow(TDes16& aDes)
	{
	aDes = _L("*** ERROR : line too long, cannot log ***");
	}

/*
 *========================================================================================================
 * Base class for TruePause DevSound test steps in this suite.
 *========================================================================================================
 */

const TUint KRateLookup[] =
	{	
	EMMFSampleRate8000Hz, 
	EMMFSampleRate11025Hz, 
	EMMFSampleRate16000Hz,
	EMMFSampleRate22050Hz, 
	EMMFSampleRate32000Hz, 
	EMMFSampleRate44100Hz, 
	EMMFSampleRate48000Hz, 
	EMMFSampleRate88200Hz, 
	EMMFSampleRate96000Hz,
	EMMFSampleRate12000Hz,
	EMMFSampleRate24000Hz,
	EMMFSampleRate64000Hz 
	};

const TUint KChannelsLookup[] =
	{
	EMMFMono,
	EMMFStereo
	};

RA3FDevSoundTestBase::RA3FDevSoundTestBase(const TDesC& aTestName)
	:	iFourCCString(KNullDesC), 
		iFourCCCode(KMMFFourCCCodeNULL), 
		iDevSoundState(EStateInitial), 
		iMMFDevSound(NULL),
		iTimer(NULL), iBuffer(NULL)
	{
	iTestStepName = aTestName;
	}

void RA3FDevSoundTestBase::KickoffTestL()
	{
	INFO_PRINTF1(_L("__________  Creating DevSound object ___________"));
	
	// Create devsound object	
	TRAPD(err,  iMMFDevSound = CMMFDevSound::NewL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create DevSound object. Error = %d"), err);
		StopTest(err);
		return;
		}
	INFO_PRINTF1(_L("DevSound State: EStateCreated"));
	iDevSoundState = EStateCreated;
	// Connect RFs
	err = iFs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not connect to Filesystem. Error = %d"), err);
		StopTest(err);
		return;
		}
	DoKickoffTestL();
	INFO_PRINTF1(_L("DevSound Event: EEventInitialize"));
	Fsm(EEventInitialize, KErrNone);
	}

void RA3FDevSoundTestBase::CloseTest()
	{
	if (iMMFDevSound)
		{
		INFO_PRINTF1(KMsgDeleteDevsound);
		delete iMMFDevSound;
		}
	if(iTimer)
		{
		delete iTimer;
		}
	if (iAsyncWriteBTFAO)
		{
		delete iAsyncWriteBTFAO;
		iAsyncWriteBTFAO = NULL;
		}
	
	iFile.Close();
	iFs.Close();
	}
	
void RA3FDevSoundTestBase::CallStopTest(TInt aError)
	{
	StopTest(aError);
	}

void RA3FDevSoundTestBase::StartTimer(TTimeIntervalMicroSeconds32 aWaitTime)
	{
	TTimeIntervalMicroSeconds32 timeInterval;
	
	if(aWaitTime <= TTimeIntervalMicroSeconds32(0))
		{
		timeInterval = KMicroSecsInOneSec;
		}
	else
		{
		timeInterval = aWaitTime;
		}
	TCallBack callback (TimerCallback, this);
	iTimer->Start(timeInterval, timeInterval, callback);
	INFO_PRINTF1(_L("Timer has been started"));
	}

TInt RA3FDevSoundTestBase::TimerCallback(TAny* aPtr)
	{
	static_cast<RA3FDevSoundTestBase*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

void RA3FDevSoundTestBase::DoTimerCallback()
	{
	__ASSERT_ALWAYS(0, Panic(_L("RA3FDevSoundTestBase"), EInvalidCallbackCall));	
	}

void RA3FDevSoundTestBase::EncodingFromStringToTFourCC(const TDesC& aFourCCString)
	{
	iFourCCString.Copy(aFourCCString);
	if(aFourCCString.Length() <= KTFourCC)
		{
		while( iFourCCString.Length() < KTFourCC )
			{
			iFourCCString.Insert(0, _L(" "));  
			}
		iFourCCCode = TFourCC(iFourCCString[3] << 24 | iFourCCString[2] << 16 | iFourCCString[1] << 8 | iFourCCString[0]);
		}
	else
		{
		ERR_PRINTF2(KMsgErrorFourccLength,iFourCCString.Length());
		StopTest(KErrUnknown);
		}
	}

void RA3FDevSoundTestBase::InitializeComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound InitializeComplete() callback =========="));
	INFO_PRINTF3(KMsgErrorDevSoundCallback, &KInitializeCompleteText, aError);
	if(iDevSoundState == EStateInitializing)
		{
		INFO_PRINTF1(_L("DevSound Event: EEventInitComplete"));
		Fsm(EEventInitComplete, aError);
		}
	}

void RA3FDevSoundTestBase::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	if (aError == KErrUnderflow)
		{
		INFO_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished with error = %d as expected"), aError);
		StopTest(aError,EPass);
		}
	else
		{
		ERR_PRINTF2(_L("DevSound called CMMFDevSound::ToneFinished with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError);
		}
	}

void RA3FDevSoundTestBase::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("========== DevSound BufferToBeFilled() callback =========="));
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		StopTest(KErrGeneral);
		}
	else
		{
		iBuffer = aBuffer;	
		INFO_PRINTF1(_L("DevSound Event: EEventBTBF"));
		Fsm(EEventBTBF, KErrNone);
		}	
	}

void RA3FDevSoundTestBase::PlayError(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound PlayError() callback =========="));
	INFO_PRINTF3(KMsgErrorDevSoundCallback, &KPlayErrorText, aError);
	if(iBuffer && iBuffer->LastBuffer() && aError == KErrUnderflow)
		{
		INFO_PRINTF1(_L("Playback completed normally"));
		StopTest();
		}
	else
		{
		INFO_PRINTF1(_L("Playback completed with error"));
		StopTest(aError, EFail);
		}	
	}

void RA3FDevSoundTestBase::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("DevSound called BufferToBeEmptied."));
	if (!aBuffer)
		{
		INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
		StopTest(KErrGeneral);
		return;
		}
	iBuffer = aBuffer;
	if(aBuffer->LastBuffer())
		{
		if(iDevSoundState == EStatePause)
			{
			// We need to call CMMFDevSound->Stop() here if last buffer flag set
			INFO_PRINTF1(_L("Devsound is in Paused state and CMMFBuffer::LastBuffer is set"));
			INFO_PRINTF1(_L("iMMFDevSound->Stop()"));
			iMMFDevSound->Stop();
			StopTest();
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

void RA3FDevSoundTestBase::RecordError(TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called RecordError with error = %d"), aError);
	if (aError == KErrUnderflow)
		{
		StopTest(aError);	
		}
	}

void RA3FDevSoundTestBase::ConvertError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("RA3FDevSoundTestBase"), EInvalidCallbackCall));
	}

void RA3FDevSoundTestBase::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("RA3FDevSoundTestBase"), EInvalidCallbackCall));
	}

void RA3FDevSoundTestBase::SendEventToClient(const TMMFEvent& aEvent)
	{
	INFO_PRINTF3(_L("RA3FDevSoundTestBase::SendEventToClient type=0x%08x errorCode=%d"),aEvent.iEventType, aEvent.iErrorCode);
	}

void RA3FDevSoundTestBase::SampleRateFromTIntToTMMFSampleRate(TInt aSampleRate, TMMFSampleRate &aESampleRate)
	{
	const TSampleRateToTMMFSampleRate SampleRateLookUp [] =
	{
	{8000,	EMMFSampleRate8000Hz},
	{11025,	EMMFSampleRate11025Hz},
	{16000,	EMMFSampleRate16000Hz},
	{22050,	EMMFSampleRate22050Hz},
	{32000,	EMMFSampleRate32000Hz},
	{44100,	EMMFSampleRate44100Hz},
	{48000,	EMMFSampleRate48000Hz},
	{88200,	EMMFSampleRate88200Hz},
	{96000,	EMMFSampleRate96000Hz},
	{12000,	EMMFSampleRate12000Hz},
	{24000,	EMMFSampleRate24000Hz},
	{64000,	EMMFSampleRate64000Hz}
	};
	
	const TInt length = sizeof SampleRateLookUp / sizeof *SampleRateLookUp;
	
	for(TInt i = 0; i < length; i++)
		{
		if(aSampleRate == SampleRateLookUp[i].iTIntSampleRate)
			{
			aESampleRate = SampleRateLookUp[i].iTMMFSampleRate;
			return;
			}
		}
		ERR_PRINTF1(_L("User SampleRate doesn't match any of the specified sample rates"));
		StopTest(KErrGeneral);
	}
	

void RA3FDevSoundTestBase::SampleRateFromTUintToString(TUint aSampleRate, TDes& aStringSampleRate)
	{
	const TSampleRateToString SampleRateLookUp [] =
	{
	{0x00000001,	KEMMFSampleRate8000Hz() },
	{0x00000002,	KEMMFSampleRate11025Hz()},
	{0x00000004,  	KEMMFSampleRate16000Hz()},
	{0x00000008,	KEMMFSampleRate22050Hz()},
	{0x00000010,	KEMMFSampleRate32000Hz()},
	{0x00000020,	KEMMFSampleRate44100Hz()},
	{0x00000040,	KEMMFSampleRate48000Hz()},
	{0x00000080,	KEMMFSampleRate88200Hz()},
	{0x00000100,	KEMMFSampleRate96000Hz()},
	{0x00000200,	KEMMFSampleRate12000Hz()},
	{0x00000400,	KEMMFSampleRate24000Hz()},
	{0x00000800,	KEMMFSampleRate64000Hz()}
	};

	const TInt length = sizeof SampleRateLookUp / sizeof *SampleRateLookUp;

	for (TInt i =0; i < length; i++)
		{
		if(aSampleRate == SampleRateLookUp[i].iTUIntSampleRate)
			{
			aStringSampleRate.Copy(SampleRateLookUp[i].iTPtrSampleRate);
			return;
			}
		}
		ERR_PRINTF1(_L("SampleRate doesn't match any of the specified sample rates"));
		StopTest(KErrGeneral);
	}

void RA3FDevSoundTestBase::ChannelsFromTUintToString(TUint aChannels,TDes& aStringChannels)
	{
	const TChannelsToString ChannelsLookUp [] =
		{
		{0x00000001,	KEMMFMono() },
		{0x00000002,	KEMMFStereo()},
		};	
	const TInt length = sizeof ChannelsLookUp / sizeof *ChannelsLookUp;

	for (TInt i =0; i < length; i++)
		{
		if(aChannels == ChannelsLookUp[i].iTUIntChannels)
			{
			aStringChannels.Copy(ChannelsLookUp[i].iTPtrChannels);
			return;
			}
		}
		ERR_PRINTF1(_L("Channels doesn't match any of the specified channels"));
		StopTest(KErrGeneral);
	}

void RA3FDevSoundTestBase::PrintSupportedCapabilities(TUint aSampleRate,TUint aChannel)
	{
	TInt length = sizeof KRateLookup / sizeof *KRateLookup;
	TBuf<KMaxSampleRateStringLength> stringSampleRate;
	TBuf<KMaxChannelsStringLength> channels;
	
	for(TInt i = 0; i < length ; i++)
		{
		if(aSampleRate & KRateLookup[i])
			{
			SampleRateFromTUintToString(KRateLookup[i],stringSampleRate);
			INFO_PRINTF3(_L("Supported Sample rate 0x%08x %S"),KRateLookup[i],&stringSampleRate);
			}
		}
	length = sizeof KChannelsLookup / sizeof *KChannelsLookup;
	
	for(TInt i = 0; i < length ; i++)
		{
		if(aChannel & KChannelsLookup[i])
			{
			ChannelsFromTUintToString(KChannelsLookup[i],channels);
			INFO_PRINTF3(_L("Supported channels 0x%08x %S"),KChannelsLookup[i],&channels);
			}
		}
	}

/*
 *========================================================================================================
 * CAsyncWriteBufferToFile
 *========================================================================================================
 */

CAsyncWriteBufferToFile::CAsyncWriteBufferToFile(RFile& aFile, CMMFDevSound* aDevSound, MStoppableTestStep* aTestStep)
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

CAsyncWriteBufferToFile* CAsyncWriteBufferToFile::NewL(RFile& aFile, CMMFDevSound* aDevSound, MStoppableTestStep* aTestStep)
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
	iFile.Write(aBuffer->Data(), aBuffer->Data().Length(), iStatus);
	SetActive();
	}

void CAsyncWriteBufferToFile::DoCancel()
	{
	// Can't cancel an async write request
	}

TInt CAsyncWriteBufferToFile::RunError(TInt aError)
	{
	iTestStep->CallStopTest(aError);
	return KErrNone;
	}

/*
 *========================================================================================================
 * Play base class
 *========================================================================================================
 */

RA3FDevSoundTestPlay::RA3FDevSoundTestPlay(const TDesC& aTestName)
	: RA3FDevSoundTestBase(aTestName),
	iFilename(KNullDesC),
	iOtherFilename(KNullDesC),
	iSampleRate(0),
	iSampleRate2(0),
	iChannels(0),
	iLSpeakerBalance(0),
	iRSpeakerBalance(0),
	iVolume(0),
	iClientRejectedClause(EFalse),
	iClientPreemptedClause(EFalse),
	iStopClause(EFalse),
	iVolumeBalanceClause(EFalse),
	iConfigClause(EFalse),
	iCompletePlayBack(EFalse),
	iInitAfterPauseClause(EFalse),
	iErrExpected(0),
	iDevsoundPlayClient(NULL)
	{
	}

RA3FDevSoundTestPlay::~RA3FDevSoundTestPlay()
	{
	}

void RA3FDevSoundTestPlay::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(iTestStepName.Right(KPanicLength), EFsmIncorrectErrorPassed));
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
				if(iConfigClause)
					{
					TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
					TBuf<KMaxChannelsStringLength> stringChannelsSet;
					SampleRateFromTIntToTMMFSampleRate (iSampleRate, iESampleRate);
					iCapabilitiesSet.iRate = iESampleRate;
					iCapabilitiesSet.iChannels = iChannels;
					SampleRateFromTUintToString (iCapabilitiesSet.iRate, stringSampleRateSet);
					ChannelsFromTUintToString (iCapabilitiesSet.iChannels, stringChannelsSet);
					INFO_PRINTF3(_L("Settings for Sample rate an channel mode : %S %S"), &stringSampleRateSet, &stringChannelsSet);
					INFO_PRINTF1(_L("Calling CMMFDevSound::SetConfigL"));
					TRAPD(err, iMMFDevSound->SetConfigL(iCapabilitiesSet));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL left with error = %d"), err);
						StopTest(err);
						return;
						}
					}
				if(iClientRejectedClause)
					{
					TMMFPrioritySettings priority;
					priority.iPriority = KMaximumPriority;
					INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
					iMMFDevSound->SetPrioritySettings(priority);
					INFO_PRINTF2(_L("Setting Priority to %d using CMMFDevSound::SetPrioritySettings"), priority.iPriority);
					}
				if(iClientPreemptedClause)
					{
					TMMFPrioritySettings priority;
					priority.iPriority = KMinimumPriority;
					INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
					iMMFDevSound->SetPrioritySettings(priority);
					INFO_PRINTF2(_L("Setting Priority to %d using CMMFDevSound::SetPrioritySettings"), priority.iPriority);
					}
				if(iVolumeBalanceClause)
					{
					TInt volume = iMMFDevSound->Volume();
					INFO_PRINTF2(_L("Setting DevSound to different default volume = %d"), volume/K25Percent);
					iMMFDevSound->SetVolume(volume/K25Percent);
					if (volume == iMMFDevSound->Volume())
						{
						ERR_PRINTF2(_L("CMMFDevSound::Volume returned default value = %d"), volume);
						StopTest (KErrGeneral);
						break;
						}
					TInt getLSpeakerBalance = 0;
					TInt getRSpeakerBalance = 0;
					INFO_PRINTF3(_L("Setting DevSound to different default balance using the corresponding values %d and %d"),
												getLSpeakerBalance/K25Percent,getRSpeakerBalance/K25Percent);
					TRAPD(err,iMMFDevSound->GetPlayBalanceL(getLSpeakerBalance, getRSpeakerBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					TRAP(err,iMMFDevSound->SetPlayBalanceL(getLSpeakerBalance/K25Percent, getRSpeakerBalance/K25Percent));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					TInt getLSpeakerBalance2 = 0;
					TInt getRSpeakerBalance2 = 0;
					TRAP(err,iMMFDevSound->GetPlayBalanceL(getLSpeakerBalance2, getRSpeakerBalance2));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					if ((getLSpeakerBalance == getLSpeakerBalance2) && (getLSpeakerBalance == getRSpeakerBalance2))
						{
						ERR_PRINTF1(_L("CMMFDevSound::Balance returned default values"));
						StopTest(KErrGeneral);
						break;
						}
					}
				TRAPD(err, iMMFDevSound->PlayInitL());
				if ( err != KErrNone)
					{
					StopTest (err);
					return;
					}
				StartTimer (KMicroSecsTwoSec);
				INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
				iDevSoundState = EStatePlaying;
				break;
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
				iDevSoundState = EStatePause;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				INFO_PRINTF1(_L("DevSound EEventBTBF was ignored after pause"));
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
				if(iClientRejectedClause)
					{
					INFO_PRINTF1(_L("Starting lower priority devsound client"));
					iDevsoundPlayClient->SetPriority(KMinimumPriority);
					INFO_PRINTF1(_L("Initializing to play audio thru second devsound client"));
					TInt err = iDevsoundPlayClient->PlayInit(iOtherFilename);
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Initialization to play audio thru second devsound client failed with %d"), err);
						StopTest(err);
						}
					iClientRejectedClause = EFalse;
					break;
					}
				if(iClientPreemptedClause)
					{
					INFO_PRINTF1(_L("Starting higher priority devsound client"));
					iDevsoundPlayClient->SetPriority(KMaximumPriority);
					INFO_PRINTF1(_L("Initializing to play audio thru second devsound client"));
					TInt err = iDevsoundPlayClient->PlayInit(iOtherFilename);
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Initialization to play audio thru second devsound client failed with %d"), err);
						StopTest(err);
						}
					iClientPreemptedClause = EFalse;
					break;
					}
				if(iStopClause)
					{
					INFO_PRINTF1(_L("Calling CMMFDevSound::Stop"));
					iMMFDevSound->Stop();
					iStopClause = EFalse;
					break;
					}
				if(iVolumeBalanceClause)
					{
					INFO_PRINTF2(_L("Setting DevSound volume = %d"), iVolume);
					iMMFDevSound->SetVolume(iVolume);
					if (iVolume != iMMFDevSound->Volume())
						{
						ERR_PRINTF2(_L("CMMFDevSound::Volume returned different set value = %d"), iVolume);
						StopTest (KErrGeneral);
						break;
						}
					INFO_PRINTF3(_L("Calling iMMFDevSound->SetPlayBalanceL using the corresponding values %d and %d"),iLSpeakerBalance,iRSpeakerBalance);
					TRAPD(err,iMMFDevSound->SetPlayBalanceL(iLSpeakerBalance, iRSpeakerBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("Call GetPlayBalanceL for verifying."));
					TInt getLSpeakerBalance = 0;
					TInt getRSpeakerBalance = 0;
					TRAP(err,iMMFDevSound->GetPlayBalanceL(getLSpeakerBalance, getRSpeakerBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					if ((iLSpeakerBalance == getLSpeakerBalance) && (iRSpeakerBalance == getRSpeakerBalance))
						{
						INFO_PRINTF1(_L("Balance configuration returned set values as expected"));
						}
					else
						{
						ERR_PRINTF3(_L("Configuration of DevSound object does not match with set balance! LSpeakerBalance = %d RSpeakerBalance = %d"), 
								getLSpeakerBalance, getRSpeakerBalance);
						StopTest(KErrGeneral);
						break;
						}
					}
				if(iConfigClause)
					{
					TMMFCapabilities capabilitiesNewSet;
					TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
					TBuf<KMaxChannelsStringLength> stringChannelsSet;
					SampleRateFromTIntToTMMFSampleRate (iSampleRate2, iESampleRate);
					capabilitiesNewSet.iRate = iESampleRate;
					capabilitiesNewSet.iChannels = iChannels;
					SampleRateFromTUintToString (capabilitiesNewSet.iRate, stringSampleRateSet);
					ChannelsFromTUintToString (capabilitiesNewSet.iChannels, stringChannelsSet);
					INFO_PRINTF3(_L("Settings for Sample rate an channel mode : %S %S"), &stringSampleRateSet, &stringChannelsSet);
					INFO_PRINTF1(_L("Calling CMMFDevSound::SetConfigL"));
					TRAPD(err, iMMFDevSound->SetConfigL(capabilitiesNewSet));
					INFO_PRINTF2(_L("CMMFDevSound::SetConfigL left with error = %d"), err);
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::IsResumeSupported"));
				TBool isResumeSupported = iMMFDevSound->IsResumeSupported();
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(isResumeSupported)
					{
					TInt err;
					                    
					if(iInitAfterPauseClause)
						{
                        INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
                        TRAP(err,iMMFDevSound->PlayInitL());
						}
			        else
						{
                        INFO_PRINTF1(_L("Calling CMMFDevSound::Resume"));               
                        err=iMMFDevSound->Resume();
						}
					if (err == iErrExpected)
						{
						INFO_PRINTF2(_L("CMMFDevSound::Resume returned %d as expected"), err);
						if(iCompletePlayBack)
							{
							iDevSoundState = EStatePlaying;
							iTimer->Cancel();
							if(iVolumeBalanceClause)
								{
								INFO_PRINTF1(_L("Call iMMFDevSound::Volume for verifying."));
								if (iVolume == iMMFDevSound->Volume())
									{
									INFO_PRINTF1(_L("CMMFDevSound::Volume returned equal previous set value as expected"));
									}
								else
									{
									ERR_PRINTF2(_L("CMMFDevSound::Volume returned different set value = %d"), iVolume);
									StopTest (KErrGeneral);
									break;
									}
						        // Check the low-level a3f volume. Should have been applied by now
						        MTestSetVolIf *volIf = static_cast<MTestSetVolIf*>(iMMFDevSound->CustomInterface(KUidTestSetVolIf));
						        if (volIf)
						            {
						            TInt vol = volIf->Vol(iMMFDevSound->MaxVolume());
	                                if (vol == iVolume)
	                                    {
	                                    INFO_PRINTF1(_L("Low-level volume returned equal previous set value as expected"));
	                                    }
	                                else
	                                    {
	                                    ERR_PRINTF3(_L("Low-level volume returned different set value = %d (expect %d)"), vol, iVolume);
	                                    StopTest (KErrGeneral);
	                                    break;
	                                    }
						            }

								INFO_PRINTF1(_L("Call iMMFDevSound::GetPlayBalanceL for verifying."));
								TInt getLSpeakerBalance = 0;
								TInt getRSpeakerBalance = 0;
								TRAP(err,iMMFDevSound->GetPlayBalanceL(getLSpeakerBalance, getRSpeakerBalance));
								if (err != KErrNone)
									{
									ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
									StopTest(err);
									break;
									}
								if ((iLSpeakerBalance == getLSpeakerBalance) && (iRSpeakerBalance == getRSpeakerBalance))
									{
									INFO_PRINTF1(_L("Balance configuration returned previous set values as expected"));
									}
								else
									{
									ERR_PRINTF3(_L("Configuration of DevSound object does not match with set balance! LSpeakerBalance = %d RSpeakerBalance = %d"), 
											getLSpeakerBalance, getRSpeakerBalance);
									StopTest(KErrGeneral);
									break;
									}
								}
							if (iConfigClause)
								{
								if(iCapabilitiesSet.iRate == iMMFDevSound->Config().iRate &&
										iCapabilitiesSet.iChannels == iMMFDevSound->Config().iChannels)
									{
									INFO_PRINTF1(_L("CMMFDevSound::Config does not change as expected"));
									}
								else
									{
									ERR_PRINTF1(_L("CMMFDevSound::Config returned different set value"));
									StopTest (KErrGeneral);
									}
								}
							}
						else
							{
							StopTest();
							}
						}
					else
						{
						ERR_PRINTF2(_L("CMMFDevSound::Resume failed with %d"), err);
						ERR_PRINTF2(_L("Expected error %d"), iErrExpected);
						StopTest(KErrGeneral , EFail);
						}
					}
				else
					{
					ERR_PRINTF1(_L("Devsound is expected to support resume"));
					ERR_PRINTF3(_L("Received value is %d when the expected value is %d"), isResumeSupported, ETrue);
					StopTest(KErrNotSupported,EFail);
					}
				#else
				if(!isResumeSupported)
					{
					INFO_PRINTF2(_L("CMMFDevSound::IsResumeSupported returned %d as expected"), isResumeSupported);
					INFO_PRINTF1(_L("Calling CMMFDevSound::Resume"));
					TInt err = iMMFDevSound->Resume();
					if (err != KErrNone)
						{
						INFO_PRINTF2(_L("CMMFDevSound::Resume returned %d as expected"), err);
						StopTest();
						}
					else
						{
						ERR_PRINTF2(_L("CMMFDevSound::Resume failed with %d"), err);
						ERR_PRINTF2(_L("Expected error %d"), KErrNotSupported);
						StopTest(KErrGeneral, EFail);
						}
					}
				else
					{
					ERR_PRINTF1(_L("Non A3FDevsound should not support resume"));
					ERR_PRINTF3(_L("Received value is %d when the expected value is %d"), isResumeSupported, EFalse);
					StopTest(KErrGeneral,EFail);
					}
				#endif
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBF or EEventTimerComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest (aError, EFail);
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

void RA3FDevSoundTestPlay::DoTimerCallback()
	{
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm (EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * Record base class
 *========================================================================================================
 */

RA3FDevSoundTestRecord::RA3FDevSoundTestRecord(const TDesC& aTestName)
	: RA3FDevSoundTestBase(aTestName)
	{
	Reset(); // Note: call Reset() instead of initialisation list, to avoid dup'd code
	}

void RA3FDevSoundTestRecord::Reset()
    {
    // re-initialise if required
    iDuration = 0;
    iSampleRate = 0;
    iSampleRate2 = 0;
    iLRecordBalance = 0;
    iRRecordBalance = 0;
    iChannels = 0;
    iGain = 0;
    isSecondTimeRecording = EFalse;
    iNoWaitLastBufferClause = EFalse;
    iPCMFormatClause = EFalse;
    iGainBalanceClause = EFalse;
    iConfigClause = EFalse;
    iFollowingResume = EFalse;
    iRecordDataFollowingResume = EFalse;
    iRecordDataPriorToResume = EFalse;
    iCheckForNoDataAfterResume = EFalse;
    iInitAfterPauseClause = EFalse;
    iResumeAfterResume = EFalse;
    }

RA3FDevSoundTestRecord::~RA3FDevSoundTestRecord()
	{
	}

void RA3FDevSoundTestRecord::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(iTestStepName.Right(KPanicLength),EFsmIncorrectErrorPassed));
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
				if(iConfigClause)
					{
					TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
					TBuf<KMaxChannelsStringLength> stringChannelsSet;
					SampleRateFromTIntToTMMFSampleRate (iSampleRate, iESampleRate);
					iCapabilitiesSet.iRate = iESampleRate;
					iCapabilitiesSet.iChannels = iChannels;
					SampleRateFromTUintToString (iCapabilitiesSet.iRate, stringSampleRateSet);
					ChannelsFromTUintToString (iCapabilitiesSet.iChannels, stringChannelsSet);
					INFO_PRINTF3(_L("Settings for Sample rate an channel mode : %S %S"), &stringSampleRateSet, &stringChannelsSet);
					INFO_PRINTF1(_L("Calling CMMFDevSound::SetConfigL"));
					TRAPD(err, iMMFDevSound->SetConfigL(iCapabilitiesSet));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMMFDevSound::SetConfigL left with error = %d"), err);
						StopTest(err);
						return;
						}
					}
				if(iGainBalanceClause)
					{
					TInt gain = iMMFDevSound->Gain();
					INFO_PRINTF2(_L("Setting DevSound to different default gain = %d"), gain/K25Percent);
					iMMFDevSound->SetGain(gain/K25Percent);
					if (gain == iMMFDevSound->Gain())
						{
						ERR_PRINTF2(_L("CMMFDevSound::Gain returned default value = %d"), gain);
						StopTest (KErrGeneral);
						break;
						}
					TInt getLRecordBalance = 0;
					TInt getRRecordBalance = 0;
					INFO_PRINTF3(_L("Setting DevSound to different default balance using the corresponding values %d and %d"),
												getLRecordBalance/K25Percent,getRRecordBalance/K25Percent);
					TRAPD(err,iMMFDevSound->GetRecordBalanceL(getLRecordBalance, getRRecordBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					TRAP(err,iMMFDevSound->SetRecordBalanceL(getLRecordBalance/K25Percent, getRRecordBalance/K25Percent));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					TInt getLRecordBalance2 = 0;
					TInt getRRecordBalance2 = 0;
					TRAP(err,iMMFDevSound->GetRecordBalanceL(getLRecordBalance2, getRRecordBalance2));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					if ((getLRecordBalance2 == getLRecordBalance) && (getRRecordBalance2 == getRRecordBalance))
						{
						ERR_PRINTF1(_L("CMMFDevSound::Balance returned default values"));
						StopTest(KErrGeneral);
						break;
						}
					}
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
				if(iBuffer->LastBuffer() && iNoWaitLastBufferClause)
					{
					ERR_PRINTF1(_L("CMMFDevSound::Resume called immediately after pause should prevent last buffer notification"));
					StopTest(KErrArgument, EFail);
					break;
					}
				if (iFollowingResume && iCheckForNoDataAfterResume)
				    {
				    // we get a BufferToBeEmptied event when we're not expecting - e.g. have Resume()'d without RecordData()
                    ERR_PRINTF1(_L("BufferToBeEmptied() received following Resume() but not expecting it"));
				    StopTest(KErrNone, EFail);
				    break;
				    }
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete)
				{
				if (iFollowingResume && iCheckForNoDataAfterResume)
				    {
				    // we've been hanging around to check for no-more data and have not received any
				    // so normal exit
				    INFO_PRINTF1(_L("No data buffers received following Resume() - as expected"));
				    StopTest();
				    break;
				    }
				iMMFDevSound->Pause();
				INFO_PRINTF1(_L("DevSound State: EStatePause"));
				iDevSoundState = EStatePause;
				if(!isSecondTimeRecording)
					{
					if(iNoWaitLastBufferClause)
						{
						INFO_PRINTF1(_L("Checking if resume is supported before receiving last buffer"));
						TBool isResumeSupported = iMMFDevSound->IsResumeSupported();
						if(isResumeSupported)
							{
							INFO_PRINTF1(_L("Calling CMMFDevSound::Resume"));
							iMMFDevSound->RecordData();
							TInt err = iMMFDevSound->Resume();
							if (err == KErrNone)
								{
								INFO_PRINTF2(_L("CMMFDevSound::Resume returned %d as expected"), err);
								iDevSoundState = EStateRecording;
								isSecondTimeRecording = ETrue;
								}
							else
								{
								ERR_PRINTF2(_L("CMMFDevSound::Resume before last buffer flag failed with %d"), err);
								ERR_PRINTF2(_L("Expected value %d"), KErrNone);
								StopTest(err, EFail);
								}
							}
						else
							{
							ERR_PRINTF1(_L("Devsound is expected to support resume"));
							ERR_PRINTF3(_L("Received value is %d when the expected value is %d"), isResumeSupported, ETrue);
							StopTest(KErrNotSupported,EFail);
							}
						isSecondTimeRecording = ETrue;
						}
					}
				else
					{
					INFO_PRINTF1(_L("Recording is Paused, waiting for the last buffer callback..."));
					iTimer->Cancel();
					}
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
				INFO_PRINTF1(_L("Writing data to file"));
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				iAsyncWriteBTFAO->Start(buffer);
				}
			else if(aDevSoundEvent == EEventTimerComplete && iBuffer && iBuffer->LastBuffer())
				{
				if(iGainBalanceClause)
					{
                    if (iGain > iMMFDevSound->MaxGain()) 
                           { 
                           iGain = iMMFDevSound->MaxGain(); 
                           } 

					INFO_PRINTF2(_L("Setting DevSound gain = %d"), iGain);
					iMMFDevSound->SetGain(iGain);
					if (iGain != iMMFDevSound->Gain())
						{
						ERR_PRINTF3(_L("CMMFDevSound::Gain returned different set value = %d, expected value = %d"), iGain, iMMFDevSound->Gain());
						StopTest (KErrGeneral);
						break;
						}
					INFO_PRINTF3(_L("Calling iMMFDevSound->SetRecordBalanceL using the corresponding values %d and %d"),iLRecordBalance,iRRecordBalance);
					TRAPD(err,iMMFDevSound->SetRecordBalanceL(iLRecordBalance, iRRecordBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("Call GetRecordBalanceL for verifying."));
					TInt getLRecordBalance = 0;
					TInt getRRecordBalance = 0;
					TRAP(err,iMMFDevSound->GetRecordBalanceL(getLRecordBalance, getRRecordBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					if ((iLRecordBalance == getLRecordBalance) && (iRRecordBalance == getRRecordBalance))
						{
						INFO_PRINTF1(_L("Balance configuration returned set values as expected"));
						}
					else
						{
						ERR_PRINTF3(_L("Configuration of DevSound object does not match with set balance! LRecordBalance = %d RRecordBalance = %d"), 
								getLRecordBalance, getRRecordBalance);
						StopTest(KErrGeneral);
						break;
						}
					}
				if(iConfigClause)
					{
					TMMFCapabilities capabilitiesNewSet;
					TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
					TBuf<KMaxChannelsStringLength> stringChannelsSet;
					SampleRateFromTIntToTMMFSampleRate (iSampleRate2, iESampleRate);
					capabilitiesNewSet.iRate = iESampleRate;
					capabilitiesNewSet.iChannels = iChannels;
					SampleRateFromTUintToString (capabilitiesNewSet.iRate, stringSampleRateSet);
					ChannelsFromTUintToString (capabilitiesNewSet.iChannels, stringChannelsSet);
					INFO_PRINTF3(_L("Settings for Sample rate an channel mode : %S %S"), &stringSampleRateSet, &stringChannelsSet);
					INFO_PRINTF1(_L("Calling CMMFDevSound::SetConfigL"));
					TRAPD(err, iMMFDevSound->SetConfigL(capabilitiesNewSet));
					INFO_PRINTF2(_L("CMMFDevSound::SetConfigL left with error = %d"), err);
					}
				INFO_PRINTF1(_L("Calling CMMFDevSound::IsResumeSupported"));
				TBool isResumeSupported = iMMFDevSound->IsResumeSupported();
				if(iPCMFormatClause)
					{
					if(isResumeSupported)
						{
						if (iRecordDataPriorToResume)
						    {
						    INFO_PRINTF1(_L("Calling CMMFDevSound::RecordData()"));
						    iMMFDevSound->RecordData();
						    }
						TInt err = KErrNone;
						if(iInitAfterPauseClause)
						    {
						    INFO_PRINTF1(_L("Calling CMMFDevSound::RecordInit()"));
						    iMMFDevSound->RecordInitL();
						    }
						else
						    {
						    INFO_PRINTF1(_L("Calling CMMFDevSound::Resume()"));
						    err = iMMFDevSound->Resume();
						    }
						if(iResumeAfterResume)
						    {
						    INFO_PRINTF1(_L("Calling CMMFDevSound::Resume()"));
						    err = iMMFDevSound->Resume();
						    }
						if (iTestStepName != _L("MM-MMF-DEVSOUND-U-0088-HP"))
						    {
						    INFO_PRINTF1(_L("Calling CMMFDevSound::RecordData()"));
						    iMMFDevSound->RecordData(); 
						    }
						if (err == KErrNone)
							{
							INFO_PRINTF2(_L("CMMFDevSound::Resume returned %d as expected"), err);
							iDevSoundState = EStateRecording;
							iFollowingResume = ETrue;
							isSecondTimeRecording = ETrue;
							if (iRecordDataFollowingResume)
							    {
	                            INFO_PRINTF1(_L("Calling CMMFDevSound::RecordData()"));
	                            iMMFDevSound->RecordData();							    
							    }
							if (iCheckForNoDataAfterResume)
							    {
							    INFO_PRINTF1(_L("Wait for a while and check we get no extra buffer requests"));
							    iTimer->Cancel(); // just in case cancel current timeout
							    StartTimer(2000000); // wait for 2s
							    }
							if(iGainBalanceClause)
								{
								INFO_PRINTF1(_L("Call iMMFDevSound::Gain for verifying."));
								if (iGain == iMMFDevSound->Gain())
									{
									INFO_PRINTF1(_L("CMMFDevSound::Gain returned equal previous set value as expected"));
									}
								else
									{
									ERR_PRINTF2(_L("CMMFDevSound::Gain returned different set value = %d"), iGain);
									StopTest (KErrGeneral);
									break;
									}
								INFO_PRINTF1(_L("Call GetRecordBalanceL for verifying."));
								TInt getLRecordBalance = 0;
								TInt getRRecordBalance = 0;
								TRAP(err,iMMFDevSound->GetRecordBalanceL(getLRecordBalance, getRRecordBalance));
								if (err != KErrNone)
									{
									ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
									StopTest(err);
									break;
									}
								if ((iLRecordBalance == getLRecordBalance) && (iRRecordBalance == getRRecordBalance))
									{
									INFO_PRINTF1(_L("Balance configuration returned previous set values as expected"));
									}
								else
									{
									ERR_PRINTF3(_L("Configuration of DevSound object does not match with set balance! LRecordBalance = %d RRecordBalance = %d"), 
											getLRecordBalance, getRRecordBalance);
									StopTest(KErrGeneral);
									}
								}
							if (iConfigClause)
								{
								if(iCapabilitiesSet.iRate == iMMFDevSound->Config().iRate &&
										iCapabilitiesSet.iChannels == iMMFDevSound->Config().iChannels)
									{
									INFO_PRINTF1(_L("CMMFDevSound::Config returned as expected"));
									}
								else
									{
									ERR_PRINTF1(_L("CMMFDevSound::Config returned different set value"));
									StopTest (KErrGeneral);
									}
								}
							}
						else
							{
							ERR_PRINTF2(_L("CMMFDevSound::Resume failed with %d"), err);
							ERR_PRINTF2(_L("Expected error %d"), KErrNone);
							StopTest(err, EFail);
							}
						}
					else
						{
						ERR_PRINTF1(_L("Devsound::IsResumeSupported did not return as expected"));
						ERR_PRINTF3(_L("Received value is %d when the expected value is %d"), isResumeSupported, ETrue);
						StopTest(KErrGeneral,EFail);
						}
					}
				else
					{
					if(!isResumeSupported)
						{
						INFO_PRINTF1(_L("Calling CMMFDevSound::Resume"));
						iMMFDevSound->RecordData();
						TInt err = iMMFDevSound->Resume();
						if (err == KErrNotSupported)
							{
							INFO_PRINTF2(_L("CMMFDevSound::Resume returned %d as expected"), err);
							iDevSoundState = EStateRecording;
							isSecondTimeRecording = ETrue;
							StopTest();
							}
						else
							{
							ERR_PRINTF2(_L("CMMFDevSound::Resume failed with %d"), err);
							ERR_PRINTF2(_L("Expected error %d"), KErrNotSupported);
							StopTest(KErrGeneral, EFail);
							}
						}
					else
						{
						ERR_PRINTF1(_L("Devsound::IsResumeSupported did not return as expected"));
						ERR_PRINTF3(_L("Received value is %d when the expected value is %d"), isResumeSupported, EFalse);
						StopTest(KErrGeneral,EFail);
						}
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventBTBE, EEventTimerComplete or LastBuffer flag not received as expected. Received event: %d"), aDevSoundEvent);
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

void RA3FDevSoundTestRecord::DoTimerCallback()
	{
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm(EEventTimerComplete, KErrNone);
	}

void RA3FDevSoundTestRecord::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("DevSound called BufferToBeEmptied."));
	if (!aBuffer)
		{
		INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
		StopTest(KErrGeneral);
		return;
		}
	iBuffer = aBuffer;
	if(aBuffer->LastBuffer())
		{
		if(iDevSoundState == EStatePause)
			{
			if(isSecondTimeRecording)
				{
				// We need to call CMMFDevSound->Stop() here if last buffer flag set
				INFO_PRINTF1(_L("Devsound is in Paused state and CMMFBuffer::LastBuffer is set"));
				INFO_PRINTF1(_L("iMMFDevSound->Stop()"));
				iMMFDevSound->Stop();
				StopTest();
				}
			else
				{
				INFO_PRINTF1(_L("****** BufferToBeEmptied callback received with a CMMFBuffer with last flag buffer set"));								
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
 * Tone base class
 *========================================================================================================
 */

RA3FDevSoundTestTone::RA3FDevSoundTestTone(const TDesC& aTestName)
	: RA3FDevSoundTestBase(aTestName),
	iDuration(0),
	iDurationEvent(0),
	iFrequencyTone1(0),
	iFrequencyTone2(0),
	iLSpeakerBalance(0),
	iRSpeakerBalance(0),
	iVolume(0),
	iVolumeBalanceClause(EFalse),
	iPaused(EFalse),
	iDTMFString(KNullDesC),
	iDTMFPauses(0),
	iDTMFTones(0)
	{
	}

RA3FDevSoundTestTone::~RA3FDevSoundTestTone()
	{
	}

void RA3FDevSoundTestTone::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(iTestStepName.Right(KPanicLength), EFsmIncorrectErrorPassed));
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
				TInt err = 0;
				switch (iPlayToneType)
					{
					case EPlayTone:
						{
						INFO_PRINTF3(_L("Calling CMMFDevSound::PlayToneL using %d Hz during %d secs "), iFrequencyTone1, iDuration);
						TRAP(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration*KMicroSecsInOneSec));
						break;
						}
					case EPlayDualTone:
						{
						INFO_PRINTF4(_L("Calling CMMFDevSound::PlayDualToneL using %d Hz %d Hz during %d secs "), iFrequencyTone1, iFrequencyTone2, iDuration);
						TRAP(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1, iFrequencyTone2, iDuration*KMicroSecsInOneSec));
						break;
						}
					case EPlayDTMFString:
						{
						INFO_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL %S"),  &iDTMFString);
						TRAP(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
						break;
						}
					case EPlayToneSequence:
						{
						TUint8* tablePointer = const_cast<TUint8*>( &(KFixedSequenceTestSequenceDataX[0] ) );
						TPtrC8 KFixedSequenceData(tablePointer, sizeof(KFixedSequenceTestSequenceDataX));
						INFO_PRINTF1(_L("Calling CMMFDevSound::PlayToneSequenceL"));
						TRAP(err, iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
						break;
						}
					}
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL left with error = %d!"), err);
					StopTest(err);
					break;
					}
				if(iVolumeBalanceClause)
					{
					TInt volume = iMMFDevSound->Volume();
					INFO_PRINTF2(_L("Setting DevSound to different default volume = %d"), volume/K25Percent);
					iMMFDevSound->SetVolume(volume/K25Percent);
					if (volume == iMMFDevSound->Volume())
						{
						ERR_PRINTF2(_L("CMMFDevSound::Volume returned default value = %d"), volume);
						StopTest (KErrGeneral);
						break;
						}
					TInt getLSpeakerBalance = 0;
					TInt getRSpeakerBalance = 0;
					INFO_PRINTF3(_L("Setting DevSound to different default balance using the corresponding values %d and %d"),
												getLSpeakerBalance/K25Percent,getRSpeakerBalance/K25Percent);
					TRAPD(err,iMMFDevSound->GetPlayBalanceL(getLSpeakerBalance, getRSpeakerBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					TRAP(err,iMMFDevSound->SetPlayBalanceL(getLSpeakerBalance/K25Percent, getRSpeakerBalance/K25Percent));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					TInt getLSpeakerBalance2 = 0;
					TInt getRSpeakerBalance2 = 0;
					TRAP(err,iMMFDevSound->GetPlayBalanceL(getLSpeakerBalance2, getRSpeakerBalance2));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					if ((getLSpeakerBalance == getLSpeakerBalance2) && (getLSpeakerBalance == getRSpeakerBalance2))
						{
						ERR_PRINTF1(_L("CMMFDevSound::Balance returned default values"));
						StopTest(KErrGeneral);
						break;
						}
					}
				StartTimer(iDurationEvent*KMicroSecsInOneSec);
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
			if (aDevSoundEvent == EEventTimerComplete)
				{
				if(iVolumeBalanceClause)
					{
					INFO_PRINTF2(_L("Setting DevSound volume = %d"), iVolume);
					iMMFDevSound->SetVolume(iVolume);
					if (iVolume != iMMFDevSound->Volume())
						{
						ERR_PRINTF2(_L("CMMFDevSound::Volume returned different set value = %d"), iVolume);
						StopTest (KErrGeneral);
						break;
						}
					INFO_PRINTF3(_L("Calling iMMFDevSound->SetPlayBalanceL using the corresponding values %d and %d"),iLSpeakerBalance,iRSpeakerBalance);
					TRAPD(err,iMMFDevSound->SetPlayBalanceL(iLSpeakerBalance, iRSpeakerBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("Call GetPlayBalanceL for verifying."));
					TInt getLSpeakerBalance = 0;
					TInt getRSpeakerBalance = 0;
					TRAP(err,iMMFDevSound->GetPlayBalanceL(getLSpeakerBalance, getRSpeakerBalance));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
						StopTest(err);
						break;
						}
					if ((iLSpeakerBalance == getLSpeakerBalance) && (iRSpeakerBalance == getRSpeakerBalance))
						{
						INFO_PRINTF1(_L("Balance configuration returned set values as expected"));
						}
					else
						{
						ERR_PRINTF3(_L("Configuration of DevSound object does not match with set balance! LSpeakerBalance = %d RSpeakerBalance = %d"), 
								getLSpeakerBalance, getRSpeakerBalance);
						StopTest(KErrGeneral);
						break;
						}
					}
				INFO_PRINTF1(_L("Checking if resume is supported"));
				TBool isResumeSupported = iMMFDevSound->IsResumeSupported();
				if(isResumeSupported)
					{
					INFO_PRINTF2(_L("CMMFDevSound::IsResumeSupported returned %d as expected"),isResumeSupported);
					INFO_PRINTF1(_L("Calling CMMFDevSound::Resume"));
					TInt expectedErr = KErrNotSupported;
					if(iPlayToneType == EPlayToneSequence)
						{
						expectedErr = KErrNone;
						}
					TInt err = iMMFDevSound->Resume();
					if(err == expectedErr)
						{
						if(iPlayToneType != EPlayToneSequence)
							{
							StopTest();
							}
						INFO_PRINTF2(_L("CMMFDevSound::Resume returned = %d as expected"), err);
						if(iVolumeBalanceClause)
							{
							INFO_PRINTF1(_L("Call iMMFDevSound::Volume for verifying."));
							if (iVolume == iMMFDevSound->Volume())
								{
								INFO_PRINTF1(_L("CMMFDevSound::Volume returned equal previous set value as expected"));
								}
							else
								{
								ERR_PRINTF2(_L("CMMFDevSound::Volume returned different set value = %d"), iVolume);
								StopTest (KErrGeneral);
								break;
								}
							INFO_PRINTF1(_L("Call iMMFDevSound::GetPlayBalanceL for verifying."));
							TInt getLSpeakerBalance = 0;
							TInt getRSpeakerBalance = 0;
							TRAP(err,iMMFDevSound->GetPlayBalanceL(getLSpeakerBalance, getRSpeakerBalance));
							if (err != KErrNone)
								{
								ERR_PRINTF2(_L("Getting balance failed ! Left with error = %d"), err);
								StopTest(err);
								break;
								}
							if ((iLSpeakerBalance == getLSpeakerBalance) && (iRSpeakerBalance == getRSpeakerBalance))
								{
								INFO_PRINTF1(_L("Balance configuration returned previous set values as expected"));
								}
							else
								{
								ERR_PRINTF3(_L("Configuration of DevSound object does not match with set balance! LSpeakerBalance = %d RSpeakerBalance = %d"), 
										getLSpeakerBalance, getRSpeakerBalance);
								StopTest(KErrGeneral);
								break;
								}
							}
						}
					else
						{
						ERR_PRINTF3(_L("CMMFDevSound::Resume returned with error = %d, when the expected error is = %d"), err, expectedErr);
						StopTest (KErrGeneral, EFail);
						}
					}
				else
					{
					ERR_PRINTF1(_L("CMMFDevSound did not support resume"));
					ERR_PRINTF3(_L("Received value is %d when the expected value is %d"), isResumeSupported, ETrue);
					StopTest(KErrGeneral,EFail);
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
			StopTest(KErrGeneral);
			break;
			}
		}
	}

void RA3FDevSoundTestTone::DoTimerCallback()
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
 * MM-MMF-DEVSOUND-U-0081-HP
 */

RA3FDevSoundPauseAndResumePlayingTest::RA3FDevSoundPauseAndResumePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestPlay(aTestName)
	{
	iErrExpected = KErrNone;
	iCompletePlayBack = ETrue;
	}

RA3FDevSoundPauseAndResumePlayingTest* RA3FDevSoundPauseAndResumePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPauseAndResumePlayingTest* self = new (ELeave) RA3FDevSoundPauseAndResumePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPauseAndResumePlayingTest::DoKickoffTestL()
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
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
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

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0082-HP
 */

RA3FDevSoundPlayRejectionTestWhilePausing::RA3FDevSoundPlayRejectionTestWhilePausing(const TDesC& aTestName) :
	RA3FDevSoundTestPlay(aTestName)
	{
	iErrExpected = KErrNone;
	iCompletePlayBack = ETrue;
	iClientRejectedClause = ETrue;
	}

RA3FDevSoundPlayRejectionTestWhilePausing* RA3FDevSoundPlayRejectionTestWhilePausing::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayRejectionTestWhilePausing* self = new (ELeave) RA3FDevSoundPlayRejectionTestWhilePausing(aTestName);
	return self;
	}

void RA3FDevSoundPlayRejectionTestWhilePausing::DoKickoffTestL()
	{
	TPtrC filename;
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		StopTest(err);
		return;
		}
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
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

void RA3FDevSoundPlayRejectionTestWhilePausing::ClientInitializeCompleteCallback(TInt aError)
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
		TInt err = iDevsoundPlayClient->PlayAudio();
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Starting to play audio thru second devsound client failed with error = %d"), err);
			StopTest(err);
			}
		}
	}

void RA3FDevSoundPlayRejectionTestWhilePausing::ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
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
	StopTest(KErrGeneral);
	}

void RA3FDevSoundPlayRejectionTestWhilePausing::ClientBufferToBeFilledCallback(TInt aError)
	{
	INFO_PRINTF2(_L("Second DevSound client should not call BufferToBeFilled in rejection. Error = %d"), aError);
	StopTest(KErrGeneral);
	}

void RA3FDevSoundPlayRejectionTestWhilePausing::ClientPlayErrorCallback(TInt aError, TBool aLastBuffer)
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

void RA3FDevSoundPlayRejectionTestWhilePausing::PlayError(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound PlayError() callback =========="));
	INFO_PRINTF3(KMsgErrorDevSoundCallback, &KPlayErrorText, aError);
	if(iBuffer->LastBuffer() && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Playback completed normally"));
		StopTest();
		}
	else
		{
		INFO_PRINTF1(_L("Playback completed with error"));
		StopTest(aError, EFail);
		}	
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0083-HP
 */
RA3FDevSoundPlayPreemptionTestWhilePausing::RA3FDevSoundPlayPreemptionTestWhilePausing(const TDesC& aTestName) :
	RA3FDevSoundTestPlay(aTestName)
	{
	iErrExpected = KErrNotReady;
	iClientPreemptedClause = ETrue;
	}

RA3FDevSoundPlayPreemptionTestWhilePausing* RA3FDevSoundPlayPreemptionTestWhilePausing::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayPreemptionTestWhilePausing* self = new (ELeave) RA3FDevSoundPlayPreemptionTestWhilePausing(aTestName);
	return self;
	}

void RA3FDevSoundPlayPreemptionTestWhilePausing::DoKickoffTestL()
	{
	TPtrC filename;
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
		StopTest(err);
		return;
		}
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
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

void RA3FDevSoundPlayPreemptionTestWhilePausing::PlayError(TInt aError)
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

void RA3FDevSoundPlayPreemptionTestWhilePausing::ClientInitializeCompleteCallback(TInt aError)                               
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

void RA3FDevSoundPlayPreemptionTestWhilePausing::ClientBufferToBeFilledCallback(TInt aError)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound BufferToBeFilled() callback =========="));
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("Second DevSound client called BufferToBeFilled and failed with error = %d"), aError);
		StopTest(aError);
		}
	}

void RA3FDevSoundPlayPreemptionTestWhilePausing::ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)
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
	StopTest(KErrGeneral);
	}

void RA3FDevSoundPlayPreemptionTestWhilePausing::ClientPlayErrorCallback(TInt /*aError*/, TBool /*aLastBuffer*/)
	{
	INFO_PRINTF1(_L("========== Second Client DevSound PlayError() callback =========="));
	ERR_PRINTF1(_L("Second DevSound had finished to play before TimerEvent was received"));
	StopTest(KErrGeneral);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0084-HP
 */

RA3FDevSoundPlayPauseAndStopPlayingTest::RA3FDevSoundPlayPauseAndStopPlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestPlay(aTestName)
	{
	iErrExpected = KErrNotReady;
	iStopClause = ETrue;
	}

RA3FDevSoundPlayPauseAndStopPlayingTest* RA3FDevSoundPlayPauseAndStopPlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayPauseAndStopPlayingTest* self = new (ELeave) RA3FDevSoundPlayPauseAndStopPlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayPauseAndStopPlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		StopTest(err);
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

void RA3FDevSoundPlayPauseAndStopPlayingTest::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("========== DevSound BufferToBeFilled() callback =========="));
	if(iStopClause)
		{
		if (!aBuffer)
			{
			ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
			StopTest(KErrGeneral);
			}
		else
			{
			iBuffer = aBuffer;	
			INFO_PRINTF1(_L("DevSound Event: EEventBTBF"));
			Fsm(EEventBTBF, KErrNone);
			}
		}
	else
		{
		ERR_PRINTF1(_L("BufferToBeFilled should never occur after stop"));
		StopTest(KErrGeneral);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0085-HP
 */

RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest::RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestPlay(aTestName)
	{
	iCompletePlayBack = ETrue;
	iVolumeBalanceClause = ETrue;
	iErrExpected = KErrNone;
	}

RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest* RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest* self = new (ELeave) RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest::DoKickoffTestL()
	{
	TPtrC filename;
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound);
		return;
		}
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		StopTest(err);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}
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
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("========== DevSound BufferToBeFilled() callback =========="));
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		StopTest(KErrGeneral);
		}
	else
		{
		iBuffer = aBuffer;	
		INFO_PRINTF1(_L("DevSound Event: EEventBTBF"));
		Fsm(EEventBTBF, KErrNone);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0086-HP
 */

RA3FDevSoundPauseSetConfigAndResumePlayingTest::RA3FDevSoundPauseSetConfigAndResumePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestPlay(aTestName)
	{
	iCompletePlayBack = ETrue;
	iConfigClause = ETrue;
	iErrExpected = KErrNone;
	}

RA3FDevSoundPauseSetConfigAndResumePlayingTest* RA3FDevSoundPauseSetConfigAndResumePlayingTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPauseSetConfigAndResumePlayingTest* self = new (ELeave) RA3FDevSoundPauseSetConfigAndResumePlayingTest(aTestName);
	return self;
	}

void RA3FDevSoundPauseSetConfigAndResumePlayingTest::DoKickoffTestL()
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
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		StopTest(err);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KSampleRate, iSampleRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KSampleRate2, iSampleRate2))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate2);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KChannel, iChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound);
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

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0087-HP
 */

RNonA3FDevSoundPauseAndResumePlayingTest::RNonA3FDevSoundPauseAndResumePlayingTest(const TDesC& aTestName) :
	RA3FDevSoundTestPlay(aTestName)
	{
	}

RNonA3FDevSoundPauseAndResumePlayingTest* RNonA3FDevSoundPauseAndResumePlayingTest::NewL(const TDesC& aTestName)
	{
	RNonA3FDevSoundPauseAndResumePlayingTest* self = new (ELeave) RNonA3FDevSoundPauseAndResumePlayingTest(aTestName);
	return self;
	}

void RNonA3FDevSoundPauseAndResumePlayingTest::DoKickoffTestL()
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
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
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

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0088-HP
 */

RA3FDevSoundPauseAndResumeRecordingPCMTest::RA3FDevSoundPauseAndResumeRecordingPCMTest(const TDesC& aTestName)
	:	RA3FDevSoundTestRecord(aTestName)
	{
	iPCMFormatClause = ETrue;
	}

RA3FDevSoundPauseAndResumeRecordingPCMTest* RA3FDevSoundPauseAndResumeRecordingPCMTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPauseAndResumeRecordingPCMTest* self = new (ELeave) RA3FDevSoundPauseAndResumeRecordingPCMTest(aTestName);
	return self;
	}

void RA3FDevSoundPauseAndResumeRecordingPCMTest::DoKickoffTestL()
	{
	// re-initialise varibles so can be called multiple times
	Reset();
	iPCMFormatClause = ETrue;

	
	if ( !GetIntFromConfig(iDefaultParamSet, KDuration, iDuration) ) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	TPtrC filename;
	if ( !GetStringFromConfig(iDefaultParamSet, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
    if ( !GetBoolFromConfig(iDefaultParamSet, KRecordDataFollowingResume, iRecordDataFollowingResume) )
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KRecordDataFollowingResume);
        StopTest(KErrNotFound, ETestSuiteError);
        return;     
        }
    if ( !GetBoolFromConfig(iDefaultParamSet, KRecordDataPriorToResume, iRecordDataPriorToResume) )
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KRecordDataPriorToResume);
        StopTest(KErrNotFound, ETestSuiteError);
        return;     
        }
    if ( !GetBoolFromConfig(iDefaultParamSet, KCallResumeAfterResume, iResumeAfterResume) )
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KCallResumeAfterResume);
        StopTest(KErrNotFound, ETestSuiteError);
        return;     
        }
    if ( !GetBoolFromConfig(iDefaultParamSet, KCheckForNoDataAfterResume, iCheckForNoDataAfterResume) )
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KCheckForNoDataAfterResume);
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
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, this);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0089-HP
 */

RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest::RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest(const TDesC& aTestName)
	:	RA3FDevSoundTestRecord(aTestName)
	{
	iPCMFormatClause = ETrue;
	iNoWaitLastBufferClause = ETrue;
	}

RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest* RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest* self = new (ELeave) RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest(aTestName);
	return self;
	}

void RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest::DoKickoffTestL()
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
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, this);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0090-HP
 */

RA3FDevSoundPauseAndResumeRecordingNonPCMTest::RA3FDevSoundPauseAndResumeRecordingNonPCMTest(const TDesC& aTestName)
	:	RA3FDevSoundTestRecord(aTestName)
	{
	iPCMFormatClause = EFalse;
	}

RA3FDevSoundPauseAndResumeRecordingNonPCMTest* RA3FDevSoundPauseAndResumeRecordingNonPCMTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPauseAndResumeRecordingNonPCMTest* self = new (ELeave) RA3FDevSoundPauseAndResumeRecordingNonPCMTest(aTestName);
	return self;
	}

void RA3FDevSoundPauseAndResumeRecordingNonPCMTest::DoKickoffTestL()
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
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, this);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0091-HP
 */

RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest::RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest(const TDesC& aTestName)
	:	RA3FDevSoundTestRecord(aTestName)
	{
	iPCMFormatClause = ETrue;
	iGainBalanceClause = ETrue;
	}

RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest* RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest* self = new (ELeave) RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest(aTestName);
	return self;
	}

void RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest::DoKickoffTestL()
	{
	if ( !GetIntFromConfig(iTestStepName, KDuration, iDuration) ) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if ( !GetIntFromConfig(iTestStepName, KGain, iGain) ) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KGain);
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
	if (!GetIntFromConfig(iTestStepName, KLRecordBalance, iLRecordBalance) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KLRecordBalance);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KRRecordBalance, iRRecordBalance) )
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
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, this);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0092-HP
 */

RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest::RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest(const TDesC& aTestName)
	:	RA3FDevSoundTestRecord(aTestName)
	{
	iPCMFormatClause = ETrue;
	iConfigClause = ETrue;
	}

RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest* RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest* self = new (ELeave)RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest(aTestName);
	return self;
	}

void RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest::DoKickoffTestL()
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
	if (!GetIntFromConfig(iTestStepName, KSampleRate, iSampleRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KSampleRate2, iSampleRate2))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate2);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KChannel, iChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, this);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0093-HP
 */

RA3FDevSoundTonePlayTonePauseAndResume::RA3FDevSoundTonePlayTonePauseAndResume(const TDesC& aTestName)
	:	RA3FDevSoundTestTone(aTestName)
	{
	iPlayToneType = EPlayTone;
	}

RA3FDevSoundTonePlayTonePauseAndResume* RA3FDevSoundTonePlayTonePauseAndResume::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayTonePauseAndResume* self = new(ELeave)RA3FDevSoundTonePlayTonePauseAndResume(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayTonePauseAndResume::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDurationEvent, iDurationEvent))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDurationEvent);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrPathNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayTonePauseAndResume::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to have been stopped"));
	StopTest(aError);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0094-HP
 */

RA3FDevSoundTonePlayDualTonePauseAndResume::RA3FDevSoundTonePlayDualTonePauseAndResume(const TDesC& aTestName)
	:RA3FDevSoundTestTone(aTestName)
	{
	iPlayToneType = EPlayDualTone;
	}

RA3FDevSoundTonePlayDualTonePauseAndResume* RA3FDevSoundTonePlayDualTonePauseAndResume::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDualTonePauseAndResume* self = new(ELeave)RA3FDevSoundTonePlayDualTonePauseAndResume(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDualTonePauseAndResume::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDurationEvent, iDurationEvent))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDurationEvent);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrPathNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayDualTonePauseAndResume::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to have been stopped"));
	StopTest(aError);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0095-HP
 */

RA3FDevSoundTonePlayDTMFStringPauseAndResume::RA3FDevSoundTonePlayDTMFStringPauseAndResume(const TDesC& aTestName)
	:	RA3FDevSoundTestTone(aTestName)
	{
	iPlayToneType = EPlayDTMFString;
	}

RA3FDevSoundTonePlayDTMFStringPauseAndResume* RA3FDevSoundTonePlayDTMFStringPauseAndResume::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayDTMFStringPauseAndResume * self = new(ELeave)RA3FDevSoundTonePlayDTMFStringPauseAndResume(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayDTMFStringPauseAndResume::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDurationEvent, iDurationEvent))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDurationEvent);
		StopTest(KErrPathNotFound);
		return;
		}
	TPtrC DTMF;
	if ( !GetStringFromConfig(iTestStepName, KDTMFString, DTMF) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrPathNotFound);
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

void RA3FDevSoundTonePlayDTMFStringPauseAndResume::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to have been stopped"));
	StopTest(aError);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0096-HP
 */

RA3FDevSoundTonePlayToneSequencePauseAndResume::RA3FDevSoundTonePlayToneSequencePauseAndResume(const TDesC& aTestName)
	:	RA3FDevSoundTestTone(aTestName)
	{
	iPlayToneType = EPlayToneSequence;
	}

RA3FDevSoundTonePlayToneSequencePauseAndResume* RA3FDevSoundTonePlayToneSequencePauseAndResume::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSequencePauseAndResume* self = new(ELeave)RA3FDevSoundTonePlayToneSequencePauseAndResume(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneSequencePauseAndResume::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDurationEvent, iDurationEvent))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDurationEvent);
		StopTest(KErrPathNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0097-HP
 */

RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume::RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume(const TDesC& aTestName)
	:	RA3FDevSoundTestTone(aTestName)
	{
	iPlayToneType = EPlayTone;
	iVolumeBalanceClause = ETrue;
	}

RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume* RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume* self = new(ELeave)RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDuration, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KDurationEvent, iDurationEvent))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDurationEvent);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrPathNotFound);
		return;
		}
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
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("========== DevSound ToneFinished() callback =========="));
	ERR_PRINTF1(_L("DevSound should not have called ToneFinished because it is supposed to have been stopped"));
	StopTest(aError);
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0098-HP
 */

RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume::RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume(const TDesC& aTestName)
	:	RA3FDevSoundTestTone(aTestName)
	{
	iPlayToneType = EPlayToneSequence;
	iVolumeBalanceClause = ETrue;
	}

RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume* RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume* self = new(ELeave)RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume(aTestName);
	return self;
	}

void RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KDurationEvent, iDurationEvent))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDurationEvent);
		StopTest(KErrPathNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrPathNotFound);
		return;
		}
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
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}
	


/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0103-HP
 */
	

const TUid KUidDevSoundTestCustomInterface1 = {KMmfDevSoundTestCustomInterface1Mux};	
	
RTestStepMultipleCustomInterface* RTestStepMultipleCustomInterface::NewL()
	{
	RTestStepMultipleCustomInterface* self = new (ELeave) RTestStepMultipleCustomInterface();
	return self;
	}

RTestStepMultipleCustomInterface::RTestStepMultipleCustomInterface()
	{
	iTestStepName = _L("MM-MMF-DEVSOUND-U-0103-HP");
	iTimerFlag = KErrNone;
	iAsyncCompleteFlag = KErrNone;
	iPeriodic = NULL;
	}
	
TVerdict RTestStepMultipleCustomInterface::DoTestStepPostambleL()
	{
	if(iPeriodic)
		{
		delete iPeriodic;
		iPeriodic = NULL;
		}
	return EPass;
	}

TVerdict RTestStepMultipleCustomInterface::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test CMMFDevSound Custom Interface Framework"));
	INFO_PRINTF1(_L("Test SMP SAFE Condition for Custom Interfaces"));

	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);
	
	CMMFDevSound* devSound = CMMFDevSound::NewL();
	CleanupStack::PushL(devSound);

	TVerdict result = EPass;
	
	INFO_PRINTF1(_L("DevSound Created"));

	// fetch the custom interface
	MMMFDevSoundTestCustomInterface* ptr1 = static_cast <MMMFDevSoundTestCustomInterface*> (devSound->CustomInterface(KUidDevSoundTestCustomInterface));
	
	MMMFDevSoundTestCustomInterface* ptr2 = static_cast <MMMFDevSoundTestCustomInterface*> (devSound->CustomInterface(KUidDevSoundTestCustomInterface1));

	if (ptr1 && ptr2)
		{
		// can only do this with a valid interface
		result = TestCustomInterfaceL(ptr1, ptr2);
		}
	else
		{
		INFO_PRINTF1(_L("error in creating custom interfaces"));
		result = EFail;
		}

	CleanupStack::PopAndDestroy(2, scheduler);
	return result;
	}

LOCAL_C TInt TimerCallBack(TAny* aTimerObs)
	{
	((RTestStepMultipleCustomInterface*)aTimerObs)->iTimerFlag = KTimerCallBackFinished;
	CActiveScheduler::Stop();
	return KErrNone;
	}


const TInt KCIAsyncTimeOut = 60000000;
TVerdict RTestStepMultipleCustomInterface::TestCustomInterfaceL(MMMFDevSoundTestCustomInterface* aInterface1, MMMFDevSoundTestCustomInterface* aInterface2)
	{
	TVerdict result = EPass;
	TInt err = KErrNone;
	TBuf8<16> dummyResult;
	dummyResult.FillZ();
	INFO_PRINTF1(_L("Testing TestAsyncCommand(const TDesC&, TRequestStatus&)"));
	
	TTimeIntervalMicroSeconds32 time(KCIAsyncTimeOut);
	iTimerFlag = KErrNone;
	iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
	iPeriodic->Start(time,time,TCallBack(TimerCallBack,this));
	
	CMultipleAsyncListener* listener1= new(ELeave)CMultipleAsyncListener(this);
	CleanupStack::PushL(listener1);
	
	listener1->InitialiseActiveListener();
	aInterface1->TestAsyncCommandL(KDevSoundCITestIn, listener1->iStatus);
	
	CMultipleAsyncListener* listener2= new(ELeave)CMultipleAsyncListener(this);
	CleanupStack::PushL(listener2);
	
	listener2->InitialiseActiveListener();	

	aInterface2->TestAsyncCommandResultL(KDevSoundCITestIn,dummyResult,listener2->iStatus);
	CActiveScheduler::Start();
	
	INFO_PRINTF1(_L("testing complete"));
	
	iPeriodic->Cancel();
	delete iPeriodic;
	iPeriodic = NULL;
	
	err = listener1->iStatus.Int();

	if ( iTimerFlag == KTimerCallBackFinished )
		{
		ERR_PRINTF1(_L("AsyncCustomCommand for CI Failing. Request Not Completed "));
		result = EFail;
		}
	
	else if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Unexpected result %d"), err);
		result = EFail;
		}
	else if (dummyResult != KDevSoundCITestOut)
		{
		INFO_PRINTF1(_L("Result string is not correct!"));
		result = EFail;
		}
		
	
	CleanupStack::PopAndDestroy(listener2);
	CleanupStack::PopAndDestroy(listener1);
	
	INFO_PRINTF1(_L("Testing TestAsyncCommandResult(const TDesC&, TDes8&, TRequestStatus&)"));
	
	iAsyncCompleteFlag = 0;
		
	CMultipleAsyncListener* listener3= new(ELeave)CMultipleAsyncListener(this);
	CleanupStack::PushL(listener3);
	
	listener3->InitialiseActiveListener();
	
	TBuf8<16> dummyResult1;
	dummyResult1.FillZ();
	
	aInterface1->TestAsyncCommandResultL(KDevSoundCITestIn, dummyResult1, listener3->iStatus);
	
	CMultipleAsyncListener* listener4= new(ELeave)CMultipleAsyncListener(this);
	CleanupStack::PushL(listener4);
	
	listener4->InitialiseActiveListener();	
	aInterface2->TestAsyncCommandL(KDevSoundCITestIn, listener4->iStatus);
	
	iTimerFlag = KErrNone;
	iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
	iPeriodic->Start(time,time,TCallBack(TimerCallBack,this));
	
	CActiveScheduler::Start();
	
	iPeriodic->Cancel();
	delete iPeriodic;
	iPeriodic = NULL;

	if (iTimerFlag == KTimerCallBackFinished )
		{
		ERR_PRINTF1(_L("AsyncCustomCommandResult for CI Failing. Request Not Completed "));
		result = EFail;
		}

	if (dummyResult1 != KDevSoundCITestOut)
		{
		INFO_PRINTF1(_L("Result string is not correct!"));
		result = EFail;
		}

	err = listener4->iStatus.Int();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected result %d"), err);
		result = EFail;
		}
	CleanupStack::PopAndDestroy(listener4);
	CleanupStack::PopAndDestroy(listener3);	
	
	return result;
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0099-HP
 */

RA3FDevSoundPlayPauseFlushResumeTest* RA3FDevSoundPlayPauseFlushResumeTest::NewL(const TDesC& aTestName)
	{
	return new (ELeave) RA3FDevSoundPlayPauseFlushResumeTest(aTestName);
	}

RA3FDevSoundPlayPauseFlushResumeTest::RA3FDevSoundPlayPauseFlushResumeTest(const TDesC& aTestName)
: RA3FDevSoundTestBase(aTestName)
	{
	}
	

void RA3FDevSoundPlayPauseFlushResumeTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iTestStepName, KFilename, filename))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, EFail);
		return;
		}
	// open using RFile for playback
	iFileName.Copy(filename);
	TInt err = iFile.Open(iFs, iFileName, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFileName, err);
		iFs.Close();
		StopTest(err, EFail);
		return;
		}
	INFO_PRINTF2(_L("File under test  -> %S"), &iFileName);
	TPtrC fourccCode;
	if (!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound, EFail);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayPauseFlushResumeTest::DoTimerCallback()
	{
	iTimer->Cancel();
	Fsm(EEventTimerComplete, KErrNone);
	}


/*
 * 
 * Test outline:
 * 1) Setup DevSound
 * 2) Start playing the file and the timer
 * 3) When the timer expires pause playback, flush the playback buffers and restart the timer
 *    During this time the BufferToBeFilled() callback should not be called
 * 4) When the timer expires resume the playback and check the EOF is reached.
 * 
 * 
 */
void RA3FDevSoundPlayPauseFlushResumeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
		
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if ((aDevSoundEvent != EEventInitialize) || (aError != KErrNone))
				{
				ERR_PRINTF3(_L("ERROR: Illegal state in EStateCreated, event: %d, error: %d"), aDevSoundEvent, aError);
				StopTest(KErrGeneral, EFail);
				}
			
			//Stage 1 Setting up devsound
			INFO_PRINTF1(_L("Stage 1: Setting up DevSound")); 
			TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
			
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("ERROR: DevSound failed to initialize with error: %d"), err);
				StopTest(err, EFail);
				}
			
			iDevSoundState = EStateInitializing;
			break;
			}
		case EStateInitializing:
			{
			if ((aDevSoundEvent != EEventInitComplete) || (aError != KErrNone))
				{
				ERR_PRINTF3(_L("ERROR: DevSound failed to initialize, event: %d, error: %d"), aDevSoundEvent, aError);
				StopTest(KErrGeneral, EFail);
				}
			
			//Stage 2 Starting playback and the timer
			INFO_PRINTF1(_L("Stage 2: Starting playback and first timer"));
			iDevSoundState = EStatePlaying;
			iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
			iMMFDevSound->PlayInitL();
			StartTimer(KMicroSecsInOneSec * 3);
			break;			
			}
		case EStatePlaying:
			{
			if ((aDevSoundEvent == EEventBTBF) && (aError == KErrNone))
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				TInt err = iFile.Read (buffer->Data());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: iFile.Read() failed with %d"), err);
					StopTest(err, EFail);
					break;
					}
				if (buffer->Data().Length ()!= buffer->RequestSize())
					{
					iBuffer->SetLastBuffer (ETrue);
					}
				INFO_PRINTF1(_L("CMMFDevSound::PlayData")); //XXX tidy up
				iMMFDevSound->PlayData();
				}
			else if ((aDevSoundEvent == EEventTimerComplete) && (aError == KErrNone))
				{
				//Stage 3 Pausing and flushing buffers
				INFO_PRINTF1(_L("Stage 3: Pausing and flushing, during the paused stage there should be no calls to BufferToBeFilled()"));
				iMMFDevSound->Pause();
				iDevSoundState = EStatePause;
				TRAPD(err, iMMFDevSound->EmptyBuffers());
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Failure in CMMFDevSound::EmptyBuffers(), error: %d"), err);
					StopTest(err, EFail);
					}
				StartTimer(KMicroSecsInOneSec * 3);
				}
			else	
				{
				ERR_PRINTF3(_L("ERROR: Unexpected event during playback, event: %d, error: %d"), aDevSoundEvent, aError);
				StopTest(KErrGeneral, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aDevSoundEvent == EEventBTBF)
				{
				ERR_PRINTF1(_L("ERROR: BufferToBeFilled() called while supposed to be in paused state."));
				StopTest(KErrGeneral, EFail);
				}
			else if ((aDevSoundEvent == EEventTimerComplete) && (aError == KErrNone))
				{
				//Stage 4 DevSound halted correctly, resuming playback
				INFO_PRINTF1(_L("Stage 4: No calls to BufferToBeFilled during paused stage, resuming playback to ensure it completes properly"));
				iDevSoundState = EStatePlaying;
				iMMFDevSound->Resume();
				}
			else
				{
				ERR_PRINTF3(_L("ERROR: Unexpected event while paused, event: %d, error: %d"), aDevSoundEvent, aError);
				StopTest(KErrGeneral, EFail);
				}
			break;	
			}
		default:
			{
			ERR_PRINTF2(_L("ERROR: Incorrect test state in FSM, state: %d"), iDevSoundState);
			StopTest(KErrGeneral, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0104-HP
 */
	
RTestStepRecordOverflow* RTestStepRecordOverflow::NewL(const TDesC& aTestName)
	{
	RTestStepRecordOverflow* self = new (ELeave) RTestStepRecordOverflow(aTestName);
	return self;
	}

RTestStepRecordOverflow::RTestStepRecordOverflow(const TDesC& aTestName) : RDevSoundTestBase(aTestName), iBuffersReceived(0)
	{
	}

RTestStepRecordOverflow::~RTestStepRecordOverflow()
	{
	iFile.Close();
	iFs.Close();
	delete iAsyncWriteBTFAO;
	if (iSimulateOverflowTimer)
		{
		iSimulateOverflowTimer->Cancel();
		}	
	delete iSimulateOverflowTimer;
	}
	
void RTestStepRecordOverflow::DoKickoffTestL()
	{
	User::LeaveIfError(iFs.Connect());

	TPtrC filename;
	if ( !GetStringFromConfig(iTestStepName, KFilename, filename) )
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
		StopTest(KErrNotFound, ETestSuiteError);
		return;
		}
	
	INFO_PRINTF2(_L("Opening file for recording %S"), &filename);
	User::LeaveIfError(iFile.Replace(iFs, filename, EFileWrite));
	
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, this);
	}	
	
static const TInt KBuffersBeforeSimulatingOverflow = 3;
static const TInt KBuffersToFinishTest = 30;
	
/*static*/ TInt RTestStepRecordOverflow::TimerCallBack(TAny* aPtr)
	{
	RTestStepRecordOverflow* self = static_cast<RTestStepRecordOverflow*>(aPtr);
	self->HandleCallbackTimeout();
	return EFalse;
	}	
	
void RTestStepRecordOverflow::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	++iBuffersReceived;
	if (iMMFDevSound->SamplesRecorded() < iLastSamplesRecorded)
		{
		ERR_PRINTF3(_L("The number of samples received has decreased in between callbacks - previous count was %d, latest count is %d"), iLastSamplesRecorded, iMMFDevSound->SamplesRecorded());
		StopTest(KErrNone, EFail);
		}
	iLastSamplesRecorded = iMMFDevSound->SamplesRecorded();		
	INFO_PRINTF3(_L("Received buffer %d, samples recorded is %d"), iBuffersReceived, iMMFDevSound->SamplesRecorded());
	if (iBuffersReceived == KBuffersBeforeSimulatingOverflow)
		{
		// Start a timer to simulate overflow		
		iSimulateOverflowTimer = CPeriodic::NewL(CActive::EPriorityLow);
		TCallBack callBack(TimerCallBack, this);
		const TTimeIntervalMicroSeconds32 KTimeToSimulateOverflow = 2 * 1000 * 1000; // 2 sec
		iSimulateOverflowTimer->Start(KTimeToSimulateOverflow, KTimeToSimulateOverflow, callBack);
		}
	else if (iBuffersReceived == KBuffersToFinishTest)
		{
		StopTest();
		}
	else
		{
		iMMFDevSound->RecordData();
		CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (aBuffer);
		iAsyncWriteBTFAO->Start(buffer);
		}	
	}
	
void RTestStepRecordOverflow::HandleCallbackTimeout()
	{
	// Resume the recording flow after timeout
	iSimulateOverflowTimer->Cancel();
	iMMFDevSound->RecordData();
	}
	
void RTestStepRecordOverflow::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			if (aDevSoundEvent != EEventInitialize)
				{
				ERR_PRINTF2(_L("RTestStepRecordOverflow-DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				break;
				}
			__ASSERT_ALWAYS((aError == KErrNone), Panic(EFsmIncorrectErrorPassed));
			INFO_PRINTF1(_L("RTestStepRecordOverflow-Calling CMMFDevSound::InitializeL"));
			TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("RTestStepRecordOverflow-CMMFDevSound::InitializeL left with error = %d"), err);
				StopTest(err);
				break;
				}
			INFO_PRINTF1(_L("RTestStepRecordOverflow-DevSound State: EStateInitializing"));
			iDevSoundState = EStateInitializing;
			break;
			}
		case EStateInitializing:
			{
			if (aDevSoundEvent != EEventInitComplete)
				{
				ERR_PRINTF2(_L("RTestStepRecordOverflow-DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				break;
				}
			__ASSERT_ALWAYS((aError == KErrNone), Panic(EFsmIncorrectErrorPassed));
			
			TRAPD(err, iMMFDevSound->RecordInitL());
			INFO_PRINTF1(_L("RTestStepRecordOverflow-DevSound started recording"));

			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CMMFDevSound::RecordInitL left with error : %d!"), err);
				StopTest(err);
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

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0105-HP
 */

RA3FDevSoundPauseAndInitRecordingPCMTest::RA3FDevSoundPauseAndInitRecordingPCMTest(const TDesC& aTestName)
    :   RA3FDevSoundTestRecord(aTestName)
    {
    iPCMFormatClause = ETrue;
    }

RA3FDevSoundPauseAndInitRecordingPCMTest* RA3FDevSoundPauseAndInitRecordingPCMTest::NewL(const TDesC& aTestName)
    {
    RA3FDevSoundPauseAndInitRecordingPCMTest* self = new (ELeave) RA3FDevSoundPauseAndInitRecordingPCMTest(aTestName);
    return self;
    }

void RA3FDevSoundPauseAndInitRecordingPCMTest::DoKickoffTestL()
    {
    // re-initialise varibles so can be called multiple times
    Reset();
    iPCMFormatClause = ETrue;
    iInitAfterPauseClause = ETrue;
    
    if ( !GetIntFromConfig(iDefaultParamSet, KDuration, iDuration) ) 
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
        StopTest(KErrNotFound, ETestSuiteError);
        return;
        }
    TPtrC filename;
    if ( !GetStringFromConfig(iDefaultParamSet, KFilename, filename) )
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KFilename);
        StopTest(KErrNotFound, ETestSuiteError);
        return;
        }
    if ( !GetBoolFromConfig(iDefaultParamSet, KRecordDataFollowingResume, iRecordDataFollowingResume) )
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KRecordDataFollowingResume);
        StopTest(KErrNotFound, ETestSuiteError);
        return;     
        }
    if ( !GetBoolFromConfig(iDefaultParamSet, KRecordDataPriorToResume, iRecordDataPriorToResume) )
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KRecordDataPriorToResume);
        StopTest(KErrNotFound, ETestSuiteError);
        return;     
        }
    if ( !GetBoolFromConfig(iDefaultParamSet, KCallResumeAfterResume, iResumeAfterResume) )
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KCallResumeAfterResume);
        StopTest(KErrNotFound, ETestSuiteError);
        return;     
        }
    if ( !GetBoolFromConfig(iDefaultParamSet, KCheckForNoDataAfterResume, iCheckForNoDataAfterResume) )
        {
        ERR_PRINTF2(KMsgErrorGetParameter, &KCheckForNoDataAfterResume);
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
    iAsyncWriteBTFAO = CAsyncWriteBufferToFile::NewL(iFile, iMMFDevSound, this);
    }

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0199-HP
 */

RA3FDevSoundPauseAndResumeAndInitPlayingTest::RA3FDevSoundPauseAndResumeAndInitPlayingTest(const TDesC& aTestName) :
    RA3FDevSoundTestPlay(aTestName)
    {
    iErrExpected = KErrNone;
    iCompletePlayBack = ETrue;
    iInitAfterPauseClause = ETrue;
    }

RA3FDevSoundPauseAndResumeAndInitPlayingTest* RA3FDevSoundPauseAndResumeAndInitPlayingTest::NewL(const TDesC& aTestName)
    {
    RA3FDevSoundPauseAndResumeAndInitPlayingTest* self = new (ELeave) RA3FDevSoundPauseAndResumeAndInitPlayingTest(aTestName);
    return self;
    }

void RA3FDevSoundPauseAndResumeAndInitPlayingTest::DoKickoffTestL()
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
        ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
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




