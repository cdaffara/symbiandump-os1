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

#include <e32base.h>

#include "TSI_MMF_SDEVSOUND_STEP.h"
#include "TSI_MMF_SDEVSOUND_SUITE.h"

#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include <simulprocclient.h>

#include <e32msgqueue.h>
const TInt KMaxMessageQueueItems = 16;

TAny GetDriveName(TDes& aFileName)
	{
	#ifdef __WINS__
		aFileName = _L("c:");	
	#elif defined(__MARM__) || defined(__X86GCC__)
		#ifdef __TEST_FILES_ON_C_DRIVE__
			aFileName = _L("c:");
		#else
			aFileName = _L("z:");	
		#endif // __TEST_FILES_ON_C_DRIVE__
	#endif	
	}

/**
 *
 * CTestStepDevSound
 * Test step constructor
 *
 */
CTestStepSDevSound::CTestStepSDevSound()
	{ 
	iExpectedValue = KErrNone;
	}

/**
 *
 * ~CTestStepDevSound
 *	 Test step destructor
 *
 */
CTestStepSDevSound::~CTestStepSDevSound()
	{
	}

/**
 *
 * SetTestSuite
 * @param aTestSuite
 *
 */
void CTestStepSDevSound::SetTestSuite(const CTestSuiteSDevSound* aTestSuite)
	{
	iDevSoundTestSuite = aTestSuite;
	}

/**
 *
 * DoTestStepPostambleL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSound::DoTestStepPostambleL()
	{
	if (iMMFDevSound)
		{
		INFO_PRINTF1(_L("Deleting DevSound Object"));
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	
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
void CTestStepSDevSound::InitializeComplete (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called InitializeComplete with aError = %d"), aError);
	TRequestStatus* stat = &(iAL->iStatus);
	User::RequestComplete(stat, aError);
	iCallbackArray[EInitComplete] ++;
	iCallbackError = aError;
	}

/**
 *
 * ToneFinished
 * @param aError
 *
 */
void CTestStepSDevSound::ToneFinished (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called ToneFinished with aError = %d"), aError);
	TRequestStatus* stat = &(iAL->iStatus);
	User::RequestComplete(stat, aError);
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	}

/**
 *
 * BufferToBeFilled
 * @param aBuffer
 *
 */
void CTestStepSDevSound::BufferToBeFilled (CMMFBuffer* aBuffer)
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
	iCallbackArray[EBuffToFill] ++;
	}

/**
 *
 * PlayError
 * @param aError
 *
 */
void CTestStepSDevSound::PlayError (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called PlayError with aError = %d"), aError);
	TRequestStatus* stat = &(iAL->iStatus);
	User::RequestComplete (stat, aError);
	iCallbackError = aError;
	iCallbackArray[EPlayError] ++;
	}

/**
 * 
 * BufferToBeEmptied
 * @param aBuffer
 *
 */
void CTestStepSDevSound::BufferToBeEmptied (CMMFBuffer* aBuffer)
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
	iCallbackArray[EBuffToEmpty] ++;
	}

/**
 *
 * RecordError
 * @param aError
 *
 */
void CTestStepSDevSound::RecordError (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called RecordError with aError = %d"), aError);
	TRequestStatus* stat = &(iAL->iStatus);
	User::RequestComplete (stat, aError);
	iCallbackError = aError;
	iCallbackArray[ERecError] ++;
	}

/**
 *
 * ConvertError
 *
 */
void CTestStepSDevSound::ConvertError (TInt /*aError*/)
	{}

/**
 *
 * DeviceMessage
 *
 */
void CTestStepSDevSound::DeviceMessage (TUid, const TDesC8& /*aMsg*/)
	{}


/**
 *
 * InitializeDevSound
 *
 */
TVerdict CTestStepSDevSound::InitializeDevSound(TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	
	iAL->InitialiseActiveListener();
	
	INFO_PRINTF1(_L("Initializing DevSound"));
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
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
 * SetVolume
 *
 */
void CTestStepSDevSound::SetVolume(TInt aVolume)
	{
	INFO_PRINTF2(_L("Setting Volume = %d"), aVolume);
	iMMFDevSound->SetVolume(aVolume);
	}
	
/**
 *
 * InitializeDevSound
 * @param aDataType
 * @param aMode
 * @result TVerdict
 *
 */	
TVerdict CTestStepSDevSound::InitializeDevSound(TFourCC& aDataType, TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	
	iAL->InitialiseActiveListener();
	
	INFO_PRINTF1(_L("Initializing DevSound"));
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aDataType, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
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
 * PlayInit
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSound::PlayInit()
	{
	iCallbackArray.Reset();
	iAL->InitialiseActiveListener();

	//get buffer from devsound
	TRAPD(err, iMMFDevSound->PlayInitL());
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayInitL left with error = %d"), err);
		return EFail;
		}
	else
		{
		if (iCallbackArray[EBuffToFill] != 1)
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeFilled was called %d times, expected 1"), iCallbackArray[EBuffToFill]);
			return EFail;
			}
		TInt total = 0;
		for (TInt i = EInitComplete; i < EDeviceMsg; i++)
			{
			total += iCallbackArray[i];
			}
		if (total > 1)
			{
			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), total);
			return EFail;
			}
		}
	return EPass;
	}
	

/**
 *
 * PlayData
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSound::PlayData()
	{
	iCallbackArray.Reset();

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

	TInt total = 0;
	for (TInt i = EInitComplete; i < EDeviceMsg; i++)
		{
		total += iCallbackArray[i];
		}
	if (total > 1)
		{
		ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), total);
		return EFail;
		}
	return EPass;
	}	
	

//------------------------------------------------------------------	

/**
 *
 * DoTestStepPreamble
 * @result TVerdict
 *
 */

TVerdict CTestStepSDevSoundPlaySimpleTone::DoTestStepPreambleL()
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
 * CTestStepDevSoundPlaySimpleTone
 *
 */
CTestStepSDevSoundPlaySimpleTone::CTestStepSDevSoundPlaySimpleTone(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}
	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlaySimpleTone::DoTestStepL()
	{
	INFO_PRINTF1(_L("Testing Simple Tone Playback"));
	TInt freq = 100;
	TTimeIntervalMicroSeconds dur(2000000);

	//Initialize
	TVerdict initializeOK = InitializeDevSound(EMMFStateTonePlaying);
	if (initializeOK  != EPass)
		{
		return EInconclusive;
		}

	SetVolume(iMMFDevSound->MaxVolume());
	iExpectedValue = KErrUnderflow;
	iCallbackError = KErrNone;

	iCallbackArray.Reset();

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
		TInt total = 0;
		for (TInt i = EInitComplete; i < EDeviceMsg; i++)
			{
			total += iCallbackArray[i];
			}
		if (total > 1)
			{
			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), total);
			return EFail;
			}
		}
	return EPass;
	
	}
//------------------------------------------------------------------	

/**
 *
 * DoTestStepPreamble
 * @result TVerdict
 *
 */

TVerdict CTestStepSDevSoundPlayDTMFTones::DoTestStepPreambleL()
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
 * CTestStepSDevSoundPlayDTMFTones
 *
 */
CTestStepSDevSoundPlayDTMFTones::CTestStepSDevSoundPlayDTMFTones(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}
	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayDTMFTones::DoTestStepL()
	{
	INFO_PRINTF1(_L("Testing DTMF Playback"));
	TPtrC dtmfString = (_L("0123456789,abcdef,*#"));

	//Initialize
	TVerdict initializeOK = InitializeDevSound(EMMFStateTonePlaying);
	if (initializeOK != EPass)
		{
		return EInconclusive;
		}
	
	iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
	iExpectedValue = KErrUnderflow;
		
	iCallbackError = KErrNone;
	iCallbackArray.Reset();
	
	// Set request active
	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Playing DTMF String"));
	TRAPD(err, iMMFDevSound->PlayDTMFStringL(dtmfString));
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayDTMFStringL left with error = %d"), err);
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
		TInt total = 0;
		for (TInt i = EInitComplete; i < EDeviceMsg; i++)
			{
			total += iCallbackArray[i];
			}
		if (total > 1)
			{
			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), total);
			return EFail;
			}
		}
	return EPass;
	}

//------------------------------------------------------------------	

/**
 *
 * DoTestStepPreamble
 * @result TVerdict
 *
 */

TVerdict CTestStepSDevSoundPlayDualTone::DoTestStepPreambleL()
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
 * CTestStepSDevSoundPlayDualTone
 *
 */
CTestStepSDevSoundPlayDualTone::CTestStepSDevSoundPlayDualTone(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}
	
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayDualTone::DoTestStepL()
	{
	TInt freq1 = 100;
	TInt freq2 = 200;
	TTimeIntervalMicroSeconds dur(2000000);

	INFO_PRINTF1(_L("Testing Dual Tone Playback"));
	
	//Initialize
	TVerdict initializeOK = InitializeDevSound(EMMFStateTonePlaying);
	if (initializeOK != EPass)
		{
		return EInconclusive;
		}
	
	iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume());
	iExpectedValue = KErrUnderflow;
		
	iCallbackError = KErrNone;
	iCallbackArray.Reset();
	
	// Set request active
	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Playing Dual Tone"));
	TRAPD(err, iMMFDevSound->PlayDualToneL(freq1, freq2, dur));
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayDualToneL left with error = %d"), err);
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
		TInt total = 0;
		for (TInt i = EInitComplete; i < EDeviceMsg; i++)
			{
			total += iCallbackArray[i];
			}
		if (total > 1)
			{
			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), total);
			return EFail;
			}
		}
	return EPass;
	}


//------------------------------------------------------------------

/**
 *
 * DigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *	
 */	
TVerdict CTestStepSDevSoundPlayEOFPCM16::DigitalPlayback(TInt aNumSamples, TDesC& aFilename, TFourCC& aDataType)
	{
	TVerdict initializeOK = InitializeDevSound(aDataType, EMMFStatePlaying);
	if (initializeOK != EPass)
		{
		return EInconclusive;
		}
	SetVolume(iMMFDevSound->MaxVolume());
	
	//Get a buffer to fill
	initializeOK = PlayInit();
	if (initializeOK != EPass)
		{
		return EInconclusive;
		}

	TInt error = iFs.Connect();
	if (error != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not connect to Filesystem. Error is %d"), error);
		return EInconclusive;
		}	
	
	
	TInt err = iFile.Open(iFs, aFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not open input file. Error is %d"), err);
		return EInconclusive;
		}		
		
	TInt bufferCount = 0;
	if (aNumSamples < 0)
		{// Play to EOF
		while (initializeOK == KErrNone && iCallbackError == KErrNone)
			{
			//read sizeof buffer from file
			CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
			iFile.Read(buffer->Data());
			if (buffer->Data().Length()!= buffer->RequestSize())
				{
				INFO_PRINTF3(_L("Data length copied from file = %d. Expected %d. Must be EOF"), buffer->Data().Length(), buffer->RequestSize());
				iBuffer->SetLastBuffer(ETrue);
				}

			//DevSound Play
			initializeOK = PlayData();
			bufferCount ++;
			}
		}
	else
		{
		while (bufferCount < aNumSamples && initializeOK  == KErrNone && iCallbackError == KErrNone)
			{
			//read sizeof buffer from file
			CMMFDataBuffer* buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
			iFile.Read(buffer->Data());
			if (buffer->Data().Length()!= buffer->RequestSize())
				{
				INFO_PRINTF3(_L("Data length copied from file = %d. Expected %d. Must be EOF"), buffer->Data().Length(), buffer->RequestSize());
				iBuffer->SetLastBuffer(ETrue);
				}

			//DevSound Play
			initializeOK = PlayData();
			bufferCount ++;
			}
		TRequestStatus* stat = &iAL->iStatus;
		User::WaitForRequest(*stat);
		User::RequestComplete(stat, KErrNone);
		}
		
		iFile.Close();
		iFs.Close();
		if (initializeOK != KErrNone)
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
 * DoTestStepPreamble
 * @result TVerdict
 *
 */

TVerdict CTestStepSDevSoundPlayEOFPCM16::DoTestStepPreambleL()
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
 * CTestStepSDevSoundPlayEOFPCM16
 *
 */
CTestStepSDevSoundPlayEOFPCM16::CTestStepSDevSoundPlayEOFPCM16(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}
	
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayEOFPCM16::DoTestStepL()
	{
	INFO_PRINTF1(_L("Testing DevSound Digital Audio Playback to EOF"));
	iCallbackError = KErrNone;

	TFileName filename = _L("C:\\sdevsoundinttestdata\\PCM_08_16_m.raw");
	
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type
		
	return DigitalPlayback (-1, filename, pcm16);
	}	


//------------------------------------------------------------------

/**
 *
 * CTestStepSDevSoundPlayFile
 *
 */
CTestStepSDevSoundPlayFile::CTestStepSDevSoundPlayFile(const TDesC& aTestName, const TDesC& aKeyName)
	{
	iTestStepName = aTestName;
	iKeyFileName = aKeyName;
	}
	
/**
 *
 * MapcInitComplete
 *
 */
void CTestStepSDevSoundPlayFile::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds & )
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

/**
 *
 * MapcPlayComplete
 *
 */
void CTestStepSDevSoundPlayFile::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayFile::DoTestStepL()
	{
	//Install the Active Scheduler
	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);

	iError = KErrTimedOut;
	TVerdict res = EFail;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(_L("SectionTen"),iKeyFileName,filename1))
		{
		return EInconclusive;
		}

	GetDriveName(filename);
	filename.Append(filename1);

	CMdaAudioPlayerUtility* player = CMdaAudioPlayerUtility::NewFilePlayerL(filename1,*this);
	CleanupStack::PushL(player);
	CActiveScheduler::Start();
	
	if (iError == KErrNone)
		{
		player->Play();
		CActiveScheduler::Start();
		if(iError == KErrNone) 
			{
			res = EPass;
			}
		else
			{
			INFO_PRINTF1(_L("Failed during playing the audio"));
			}
		}
	else
		{
		INFO_PRINTF2(_L("Failed during opening the file. Error =  %d "), iError);
		}			
	CleanupStack::PopAndDestroy(player);
	return res;
	}


//------------------------------------------------------------------

/**
 *
 * SetupFormatL
 *
 */
void CTestStepSDevSoundRecordFileBase::SetupFormatL(const TTestFormat aFormat)
	{
	if(iFormat)
		{
		delete iFormat;
		iFormat = NULL;
		}
	if(iCodec)
		{
		delete iCodec;
		iCodec = NULL;
		}

	switch(aFormat)
		{
		case EPcm16Wav:
			iFormat = new (ELeave) TMdaWavClipFormat;
			iCodec = new (ELeave) TMdaPcmWavCodec;
			break;
		case EMulawRaw:
			iFormat = new (ELeave) TMdaRawAudioClipFormat;
			iCodec = new (ELeave) TMdaAlawRawAudioCodec;
			break;
		case E16BitAu:
			iFormat = new (ELeave) TMdaAuClipFormat;
			iCodec = new (ELeave) TMdaAuCodec;
			break;
		case EAlawAu:
			iFormat = new (ELeave) TMdaAuClipFormat;
			iCodec = new (ELeave) TMdaAlawAuCodec;	
			break;
		case EPcm16Au:
			iFormat = new (ELeave) TMdaAuClipFormat;
			iCodec = new (ELeave) TMdaPcm16BitAuCodec;
			break;
		case EImaAdpcmWav:
			iFormat = new (ELeave) TMdaWavClipFormat;
			iCodec = new (ELeave) TMdaImaAdpcmWavCodec;
			break;
		case EAlawWav:
			iFormat = new (ELeave) TMdaWavClipFormat;
			iCodec = new (ELeave) TMdaAlawWavCodec;
			break;
		case EPcmU16:
			iFormat = new (ELeave) TMdaRawAudioClipFormat();
			iCodec = new (ELeave) TMdaUB16RawAudioCodec();
			break;
		case EPcmU8:
			iFormat = new (ELeave) TMdaRawAudioClipFormat();
			iCodec = new (ELeave) TMdaU8PcmRawAudioCodec();
			break;
		case EImasPcmWav:
			iFormat = new (ELeave) TMdaWavClipFormat();
			iCodec = new (ELeave) TMdaImaAdpcmWavCodec();
			break;
		case EPcm8:
			iFormat = new (ELeave) TMdaWavClipFormat();
			iCodec = new (ELeave) TMdaPcmWavCodec(TMdaPcmWavCodec::E8BitPcm);
			break;
		case EEpocWve:
		case ENone:
		default:
		// will create an inconlusive result as preamble leaves.
			iFormat = NULL;
			iCodec = NULL;
			break;
		}
	}

/**
 *
 * CTestStepSDevSoundRecordFile
 *
 */

CTestStepSDevSoundRecordFileBase::CTestStepSDevSoundRecordFileBase(const TDesC& aTestName, 
														   		   const TDesC& aSectName,
														   	       const TDesC& aKeyName, 
														   		   const TTestFormat aFormat, 
														   		   const TBool aCreateFile)
	{
	iTestStepName = aTestName;
	iHeapSize = 200000;		// playback part of test loads whole sound file into buffer, so need large heap
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestFormat = aFormat;
	iCreateFile = aCreateFile;	
	}

/**
 *
 * ~CTestStepSDevSoundRecordFile
 *
 */
CTestStepSDevSoundRecordFileBase::~CTestStepSDevSoundRecordFileBase()
	{
	delete iCodec;
	iCodec = NULL;
	delete iFormat;
	iFormat = NULL;
	}

/**
 *
 * CTestStepSDevSoundRecordFile Close
 *
 */
void CTestStepSDevSoundRecordFileBase::Close()
	{
	delete iCodec;
	iCodec = NULL;
	delete iFormat;
	iFormat = NULL;
	}


/**
 *
 * DoTestStepPreambleL
 *
 */
TVerdict CTestStepSDevSoundRecordFileBase::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;
	SetupFormatL(iTestFormat);

	if(iCreateFile)
		{
		// Make sure file doesn't exist (maybe from a previous test)
		TParse fullFileName;
		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 

		
		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			{
			return EInconclusive;
			}
		GetDriveName(filename);
		filename.Append(filename1);
		
		// parse the filenames
		if(!fullFileName.Set(filename, &KDefault, NULL))
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			fs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 
			TInt error = fs.Delete(fullFileName.FullName());

			if(error == KErrPathNotFound)
				{
				error = fs.MkDirAll(fullFileName.DriveAndPath());
				INFO_PRINTF2(_L("Path for file %S was been created"), &fullFileName.FullName());	
				}
			else  if(error == KErrNotFound)
				{
				INFO_PRINTF2(_L("No need to delete %S"), &fullFileName.FullName());	
				}
			else if(error==KErrNone)
				{
				INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
				}
			else
				{
				ret = EInconclusive;
				}
			fs.Close();
			}
		}

	if(ret == EInconclusive)
		{
		return ret;	
		}
	
	return EPass;
	}



/**
 *
 * DoTestStepL
 *
 */
TVerdict CTestStepSDevSoundRecordFileBase::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder"));
	TVerdict verdict = EPass;
	iError = KErrTimedOut;

	TPtrC	filename; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		{
		return EInconclusive;
		}

	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	TMdaFileClipLocation location(filename);

	recUtil->OpenL(&location,iFormat,iCodec,&iAudioSettings);
	CActiveScheduler::Start();
	INFO_PRINTF1( _L("Open a new file for recording"));
	
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening file for recording iError = %d"), iError);
		recUtil->Close();
		CleanupStack::PopAndDestroy(recUtil);
		return EFail;
		}

	iError = KErrTimedOut;
	recUtil->RecordL();
	CActiveScheduler::Start(); // open -> record
	INFO_PRINTF1( _L("Recording"));
	User::After(KFiveSeconds);

	verdict = CheckRecordError();
	if (verdict == EFail)
		{
		INFO_PRINTF2( _L("Fail to do recording with error: %d"), iError);
		}

	recUtil->Stop();
	User::After(KOneSecond);
	
	recUtil->Close();
	CleanupStack::PopAndDestroy(recUtil);
	recUtil = NULL;
	
	return	verdict;
	}


TVerdict CTestStepSDevSoundRecordFileBase::CheckRecordError()
	{
	return EFail;
	}


void CTestStepSDevSoundRecordFileBase::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorCode*/)
	{
	}



CTestStepSDevSoundRecordFile::CTestStepSDevSoundRecordFile(const TDesC& aTestName, 
														   const TDesC& aSectName,
														   const TDesC& aKeyName, 
														   const TTestFormat aFormat, 
														   const TBool aCreateFile)
							 : CTestStepSDevSoundRecordFileBase(aTestName, aSectName, 
					  	    		  					   		aKeyName, aFormat, 
					  	    		  					   		aCreateFile)
	{
	}
					  	    		  					   	
					  	    		  					   	
CTestStepSDevSoundRecordFile::~CTestStepSDevSoundRecordFile()
	{
	}
	


/**
 *
 * MoscoStateChangeEvent
 *
 */
void CTestStepSDevSoundRecordFile::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestStepSDevSoundRecordFile : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);

	CActiveScheduler::Stop();
	}	


TVerdict CTestStepSDevSoundRecordFile::CheckRecordError()
	{
	if (iError != KErrNone)
		{
		return EFail;
		}
	return EPass;
	}
			


CTestStepSDevSoundRecordFileNoCap::CTestStepSDevSoundRecordFileNoCap(const TDesC& aTestName, 
														   const TDesC& aSectName,
														   const TDesC& aKeyName, 
														   const TTestFormat aFormat, 
														   const TBool aCreateFile)
							 : CTestStepSDevSoundRecordFileBase(aTestName, aSectName, 
					  	    		  					   		aKeyName, aFormat, 
					  	    		  					   		aCreateFile)
	{
	}
						  	    		  					   	
					  	    		  					   	
CTestStepSDevSoundRecordFileNoCap::~CTestStepSDevSoundRecordFileNoCap()
	{
	}
	

/**
 *
 * MoscoStateChangeEvent
 *
 */
void CTestStepSDevSoundRecordFileNoCap::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestStepSDevSoundRecordFileNoCap : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);

	if (aCurrentState == CMdaAudioClipUtility::EOpen)
		{
		CActiveScheduler::Stop();
		}
	}	


TVerdict CTestStepSDevSoundRecordFileNoCap::CheckRecordError()
	{
	if (iError != KErrPermissionDenied)
		{
		return EFail;
		}
	return EPass;
	}
	
/**
 *
 * TestPlayData
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayWithoutInitialize::TestPlayData()
	{
	iCallbackArray.Reset();
	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Playing Data to a Buffer"));
	iMMFDevSound->PlayData();
	CActiveScheduler::Start();
	//fail the test if it comes to this point
	INFO_PRINTF1(_L("Panic Test Step does not panic"));
	return EFail;
	}

/**
 *
 * DoTestStepPreamble
 * @result TVerdict
 *
 */

TVerdict CTestStepSDevSoundPlayWithoutInitialize::DoTestStepPreambleL()
	{	
	//Install the Active Scheduler
	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);
	iAL= new(ELeave)CActiveListener;
	INFO_PRINTF1(_L("Creating DevSound Object"));
	TRAPD(err, iMMFDevSound = CMMFDevSound::NewL());
	if (err)
		{
		INFO_PRINTF2( _L("Error in Creating Devsound %d "), err);
		return EInconclusive;
		}
	else
		{
		return EPass;
		}
	}	

/**
 *
 * CTestStepSDevSoundPlayWithoutInitialize
 *
 */
CTestStepSDevSoundPlayWithoutInitialize::CTestStepSDevSoundPlayWithoutInitialize(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}
	
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayWithoutInitialize::DoTestStepL()
	{
	INFO_PRINTF1(_L("Try playing buffer without initializing the play at the first place and check if it panics"));
	iCallbackError = KErrNone;
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type
		
	TVerdict initializeOK = InitializeDevSound(pcm16, EMMFStatePlaying);
	
	if (initializeOK != EPass)
		{
		INFO_PRINTF1( _L("Error in Initializing Devsound"));
		return EInconclusive;
		}
	SetVolume(iMMFDevSound->MaxVolume());
	
	//DEF051386 - Try playing without getting a buffer from devsound through the initializing and check if it panics
	iTestStepResult = TestPlayData();
	
	return iTestStepResult;
	}	
/**
 *
 * RecordData
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundRecordWithoutInitialize::TestRecordData()
	{
	iCallbackArray.Reset();
	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Recording Data to a Buffer"));
	iMMFDevSound->RecordData();
	CActiveScheduler::Start();
	//fail the test if it comes to this point
	INFO_PRINTF1(_L("Panic Test Step does not panic"));
	return EFail;
	}
/**
 *
 * DoTestStepPreamble
 * @result TVerdict
 *
 */

TVerdict CTestStepSDevSoundRecordWithoutInitialize::DoTestStepPreambleL()
	{	
	//Install the Active Scheduler
	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);
	iAL= new(ELeave)CActiveListener;
	INFO_PRINTF1(_L("Creating DevSound Object"));
	TRAPD(err, iMMFDevSound = CMMFDevSound::NewL());
	if (err)
		{
		INFO_PRINTF2( _L("Error in Creating Devsound %d "), err);
		return EInconclusive;
		}
	else
		{
		return EPass;
		}
	}	

/**
 *
 * CTestStepSDevSoundRecordWithoutInitialize
 *
 */
CTestStepSDevSoundRecordWithoutInitialize::CTestStepSDevSoundRecordWithoutInitialize(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}
	
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundRecordWithoutInitialize::DoTestStepL()
	{
	INFO_PRINTF1(_L("Try recording buffer without initializing the record and check if it panics"));
	iCallbackError = KErrNone;
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type
	TVerdict initializeOK = InitializeDevSound(pcm16, EMMFStateRecording);
	
	if (initializeOK != EPass)
		{
		INFO_PRINTF1( _L("Error in Initializing Devsound"));
		return EInconclusive;
		}
	
	SetVolume(iMMFDevSound->MaxVolume());
	
	//DEF051386 - Try recording without getting a buffer from devsound through initializing and check if it panics.
	iTestStepResult = TestRecordData();
		
	return iTestStepResult;
	}	

//------------------------------------------------------------------

/**
 *
 * DoTestStepPreamble
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundClientThreadCap::DoTestStepPreambleL()
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
 * CTestStepSDevSoundClientThreadCap
 *
 */
CTestStepSDevSoundClientThreadCap::CTestStepSDevSoundClientThreadCap(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundClientThreadCap::DoTestStepL()
	{
	INFO_PRINTF1(_L("Testing SetClientThread()"));

#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	//Initialize
	TVerdict initializeOK = InitializeDevSound(EMMFStateTonePlaying);
	if (initializeOK  != EPass)
		{
		return EInconclusive;
		}
#endif

	//Set client thread with current thread Id
	RThread thisThread;
	TInt err = iMMFDevSound->SetClientThreadInfo(thisThread.Id());
	thisThread.Close();
	
	if(err != KErrPermissionDenied)
		{
		INFO_PRINTF2(_L("SetClientThread() returns error code [%d]"), err);
		return EFail;
		}
	return EPass;
	}

//------------------------------------------------------------------

/**
 *
 * DoTestStepPreamble
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundClientThreadInvalidId::DoTestStepPreambleL()
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
 * CTestStepSDevSoundClientThreadCap
 *
 */
CTestStepSDevSoundClientThreadInvalidId::CTestStepSDevSoundClientThreadInvalidId(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundClientThreadInvalidId::DoTestStepL()
	{
	INFO_PRINTF1(_L("Testing SetClientThread() with invalid thread Id"));

#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TVerdict initializeOK = InitializeDevSound(EMMFStateTonePlaying);
	if (initializeOK  != EPass)
		{
		return EInconclusive;
		}
#endif

	//Set client thread with invalid thread Id
	TInt err = iMMFDevSound->SetClientThreadInfo(123456789);
	
	if(err != KErrNotFound)
		{
		return EFail;
		}
	return EPass;
	}

