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

#include "char_a3f_devsound_testbase.h"
#include "char_a3f_devsound_playrecordclients.h"
#include "char_a3f_devsound_toneclient.h"

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
		iDevsoundToneClient(NULL), 
		iDevsoundPlayClient(NULL),
		iDevsoundRecordClient(NULL), 
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
	if(iDevsoundToneClient)
		{
		delete iDevsoundToneClient;
		}
	if(iDevsoundPlayClient)
		{
		delete iDevsoundPlayClient;
		}
	if(iDevsoundRecordClient)
		{
		delete iDevsoundRecordClient;
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
	if(aEvent.iEventType == KMMFEventCategoryAudioResourceAvailable)
		{
		INFO_PRINTF1(_L("Received KMMFEventCategoryAudioResourceAvailable"));
		Fsm(EResourceAvailable, aEvent.iErrorCode);
		}
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

//
// CAsyncWriteBufferToFile
//

CAsyncWriteBufferToFile::CAsyncWriteBufferToFile(RFile& aFile, CMMFDevSound* aDevSound, RA3FDevSoundTestBase& aTestStep)
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

CAsyncWriteBufferToFile* CAsyncWriteBufferToFile::NewL(RFile& aFile, CMMFDevSound* aDevSound, RA3FDevSoundTestBase& aTestStep)
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
