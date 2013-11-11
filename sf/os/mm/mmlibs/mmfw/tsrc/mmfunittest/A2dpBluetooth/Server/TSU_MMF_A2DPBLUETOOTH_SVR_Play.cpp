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

#include "TSU_MMF_A2DPBLUETOOTH_SVR_Play.h"
#include <bluetoothav.h>

using namespace SymbianSBC;

CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Buffer::CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Buffer(TFileName aFileName, TUint aBufferSize, TUint aSampleRate,  TUint aChannels, TMMFStereoSupport aStereoSupport) :
iFileName(aFileName), iBufferSize(aBufferSize), iSampleRate(aSampleRate), iChannels(aChannels), iStereoSupport(aStereoSupport)
	{	
	}

/**
Base class for playing a single pcm16 buffer
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Buffer::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	TFourCC datatype(KMMFFourCCCodePCM16);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(iSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the headset does not support the required sample rate
				// we will carry on with the test anyway but return EInconclusive
				INFO_PRINTF2(_L("headset does not support sample rate %d Hz" ), iSampleRate);
				iError = KErrNone;
				}
			verdict  = EInconclusive;
			}
		}
	if (!iError)
		{
		iA2dpBTHeadsetAudioInterface.SetChannels(iChannels,iStereoSupport);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting channels %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the headset does not support the required channels stereo support
				// we will carry on with the test anyway but return EInconclusive
				INFO_PRINTF1(_L("headset does not support channels/stereo support" ));
				iError = KErrNone;
				}
			verdict  = EInconclusive;
			}
		}

	if (!iError)
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
	if (!iError)
		{
		//try sending some data.
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, iFileName, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			iBuffer = HBufC8::NewL(iBufferSize);
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(KWavHeaderAudioDataOffset, bufferPtr,iBufferSize));
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*status);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			delete iBuffer;
			iBuffer = NULL;
			iFile.Close();
			iFileSession.Close();
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
	return verdict;
	}


CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Multiple_Buffers::CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Multiple_Buffers(TFileName aFileName, TUint aBufferSize, TUint aSampleRate,  TUint aChannels, TMMFStereoSupport aStereoSupport) :
iFileName(aFileName), iBufferSize(aBufferSize), iSampleRate(aSampleRate), iChannels(aChannels), iStereoSupport(aStereoSupport), iBufferPtr(0,0,0)
	{	
	}

/**
Base class for playing all the pcm16 buffers from the specified file
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Multiple_Buffers::DoTestStepL( void )
	{
	TVerdict verdict = EPass;

	TFourCC datatype(KMMFFourCCCodePCM16);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	iStatus = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(iSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the headset does not support the required sample rate
				// we will carry on with the test anyway but return EInconclusive
				INFO_PRINTF2(_L("headset does not support sample rate %d Hz" ), iSampleRate);
				iError = KErrNone;
				}
			verdict  = EInconclusive;
			}
		}
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetChannels(iChannels, iStereoSupport);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting channels %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the headset does not support the required channels stereo support
				// we will carry on with the test anyway but return EInconclusive
				INFO_PRINTF1(_L("headset does not support channels/stereo support" ));
				iError = KErrNone;
				}
			verdict  = EInconclusive;
			}
		}
	if (!iError)
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*iStatus);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
	if (!iError)
		{
		//try sending some data.
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, iFileName, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			iBuffer = HBufC8::NewL(iBufferSize);
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(KWavHeaderAudioDataOffset, bufferPtr, iBufferSize));
			iFilePos = bufferPtr.Length();
			iStreaming = ETrue;
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			delete iBuffer;
			iBuffer = NULL;
			iFile.Close();
			iFileSession.Close();
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
		
	return verdict;
	}

	
void CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Multiple_Buffers::AsyncNotification(TInt aError) 
	{
	iError = aError;
	TInt fileSize = 0;
	if (!iStreaming)
		{
		iActiveScheduler->Stop();
		}
	else if (!iError)
		{
		//send next buffer
		iBufferPtr.Set(iBuffer->Des());
		iError = iFile.Read(iFilePos, iBufferPtr, iBufferSize);
		if (!iError)
			{
			iFilePos += iBufferPtr.Length();
			TInt err = iFile.Size(fileSize);
			}	
		if ((iBufferPtr.Size() == 0) || (iError))
			{
			iActiveScheduler->Stop();
			}
		else
			{
			iA2dpBTHeadsetAudioInterface.PlayData(iBufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			}
		}
	else
		{
		iActiveScheduler->Stop();
		}
	}

/**
PlayData pcm16 single 16Khz mono buffer
Note the unit test spec states a sample rate of 16KHz but blueant headset does not support this
so we'll set the sample rate to 32KHz for now
*/	
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0303::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0303() :
	CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Buffer(TFileName(Kpcm16_test_07), KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames, 32000, EMMFMono, EMMFNone)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0303-HP");
	}

	
/**
PlayData pcm16 multiple 16Khz mono buffers
Note the unit test spec states a sample rate of 16KHz but blueant headset does not support this
so we'll set the sample rate to 32KHz for now
*/	
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0304::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0304() :
	CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Multiple_Buffers(TFileName(Kpcm16_test_07), KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames, 32000, EMMFMono, EMMFNone)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0304-HP");
	}


/**
PlayData pcm16 single 16Khz stereo buffer
*/
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0305::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0305() :
	CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Buffer(TFileName(Kpcm16_test_08), KPcm16_TestFile08FrameSize*KDefaultNumberOfSBCFrames, 16000, EMMFStereo, EMMFInterleavedOnly)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0305-HP");
	}


/**
PlayData pcm16 multiple 16Khz stereo buffers
*/	
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0306::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0306() :
	CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Multiple_Buffers(TFileName(Kpcm16_test_08), KPcm16_TestFile08FrameSize*KDefaultNumberOfSBCFrames, 16000, EMMFStereo, EMMFInterleavedOnly)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0306-HP");
	}	


/**
PlayData pcm16 single 44.1Khz stereo buffer
*/	
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0307::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0307() :
	CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Buffer(TFileName(Kpcm16_test_04), KPcm16_TestFile04FrameSize*KDefaultNumberOfSBCFrames, 44100, EMMFStereo, EMMFInterleavedOnly)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0307-HP");
	}


/**
PlayData pcm16 multiple 44.1Khz stereo buffers
*/		
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0308::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0308() :
	CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Multiple_Buffers(TFileName(Kpcm16_test_04), KPcm16_TestFile04FrameSize*KDefaultNumberOfSBCFrames, 44100, EMMFStereo, EMMFInterleavedOnly)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0308-HP");
	}


/**
PlayData pcm16 single 48Khz stereo buffer
*/	
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0309::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0309() :
	CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Buffer(TFileName(Kpcm16_test_18), KPcm16_TestFile18FrameSize*KDefaultNumberOfSBCFrames, 48000, EMMFStereo, EMMFInterleavedOnly)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0309-HP");
	}


/**
PlayData pcm16 multiple 48Khz stereo buffers
*/	
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0310::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0310() :
	CTestStep_MMF_A2DPBLUETOOTH_SVR_Play_Multiple_Buffers(TFileName(Kpcm16_test_18), KPcm16_TestFile18FrameSize*KDefaultNumberOfSBCFrames, 48000, EMMFStereo, EMMFInterleavedOnly)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0310-HP");
	}


CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0311::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0311()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0311-HP");
	}

/**
Cancel PlayData
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0311::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	TFourCC datatype(KMMFFourCCCodePCM16);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(KDefaultTestSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the error is not supported we will carry on with the test
				iError = KErrNone;
				}
			}
		}
	
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetChannels(EMMFMono,EMMFNone);
		}

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Configuration error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
	if (!iError)
		{
		//try sending some data.
		TFileName filename(Kpcm16_test_07);//_L("\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_07.wav");
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, filename, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			iBuffer = HBufC8::NewL(KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames);//4 SBC frames =384 bytes pcm16
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(bufferPtr,KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames));
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*status);
			//don't have very long to cancel so don't bother with timer
			iA2dpBTHeadsetAudioInterface.CancelPlayData();
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (*status != KErrCancel)
				{
				INFO_PRINTF1(_L("request status not still be pending" ));
				verdict = EFail;
				}
			else if (iError != KErrCancel)
				{
				INFO_PRINTF2(_L("expected KErrCancel got error %d" ), iError);
				verdict = EFail;
				}
			User::LeaveIfError(iFile.Read(bufferPtr,KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames));
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*status);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			delete iBuffer;
			iBuffer = NULL;
			iFile.Close();
			iFileSession.Close();
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
	return verdict;
	}


CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0312::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0312() : iBufferPtr(0,0,0)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0312-HP");
	}

/**
FlushBuffer
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0312::DoTestStepL( void )
	{
	TVerdict verdict = EPass;

	TFourCC datatype(KMMFFourCCCodePCM16);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	iStatus = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(KDefaultTestSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the error is not supported we will carry on with the test
				iError = KErrNone;
				}
			}
		}
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetChannels(EMMFMono,EMMFNone);
		}
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Configuration error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*iStatus);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
		if (!iError)
		{
		//try sending some data.
		TFileName filename(Kpcm16_test_07);//_L("\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_07.wav");
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, filename, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			iBuffer = HBufC8::NewL(KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames);//4 SBC frames = 384 bytes pcm16
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(bufferPtr, KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames));
			iBufferCount = 1;
			iStreaming = ETrue;
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}
			if (verdict == EPass)
				{
				//now flush the buffer
				iA2dpBTHeadsetAudioInterface.FlushBuffer();
				iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*iStatus);
				iStreaming = EFalse;
				iAsyncTestStepNotifier->HandleAsyncRequest();
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("Play Data Error after flushing buffer %d" ), iError);
					verdict = EFail;
					}
				}
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			delete iBuffer;
			iBuffer = NULL;
			iFile.Close();
			iFileSession.Close();
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
		
	return verdict;
	}

	
void CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0312::AsyncNotification(TInt aError) 
	{
	iError = aError;
	if (!iStreaming)
		{
		iActiveScheduler->Stop();
		}
	else
		{
		//send 5 buffers
		iBufferPtr.Set(iBuffer->Des());
		iError = iFile.Read(iBufferCount*KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames, iBufferPtr, KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames);
		iBufferCount++;
		if ((iBufferCount == KNumberOfBuffersToComplete)||(iError))
			{
			iActiveScheduler->Stop();
			}
		else
			{
			iA2dpBTHeadsetAudioInterface.PlayData(iBufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			}
		}
	}

	
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0313::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0313() : iBufferPtr(0,0,0)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0313-HP");
	}

/**
Bytes Played
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0313::DoTestStepL( void )
	{
	TVerdict verdict = EPass;

	TFourCC datatype(KMMFFourCCCodePCM16);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	iStatus = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(KDefaultTestSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the error is not supported we will carry on with the test
				iError = KErrNone;
				}
			}
		}
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetChannels(EMMFMono,EMMFNone);
		}
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Configuration error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*iStatus);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
		if (!iError)
		{
		//try sending some data.
		TFileName filename(Kpcm16_test_07);//_L("\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_07.wav");
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, filename, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			iBuffer = HBufC8::NewL(KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames);//4 SBC frames = 384 bytes pcm16
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(bufferPtr, KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames));
			iBufferCount = 1;
			iStreaming = ETrue;
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*iStatus);
			iBytesSent += bufferPtr.Size();
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}
		
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			if (iBytesSent != iA2dpBTHeadsetAudioInterface.BytesPlayed())
				{
				INFO_PRINTF3(_L("Expected %d bytes played but got %d" ), iBytesSent,iA2dpBTHeadsetAudioInterface.BytesPlayed());
				verdict = EFail;
				}
			iA2dpBTHeadsetAudioInterface.ResetBytesPlayed();
			if (iA2dpBTHeadsetAudioInterface.BytesPlayed())
				{
				INFO_PRINTF2(_L("Failed to reset bytes played - got %d bytes" ), iA2dpBTHeadsetAudioInterface.BytesPlayed());
				verdict = EFail;
				}
			delete iBuffer;
			iBuffer = NULL;
			iFile.Close();
			iFileSession.Close();
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
		
	return verdict;
	}

	
void CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0313::AsyncNotification(TInt aError) 
	{
	iError = aError;
	if (!iStreaming)
		{
		iActiveScheduler->Stop();
		}
	else
		{
		//send 5 buffers
		iBufferPtr.Set(iBuffer->Des());
		iError = iFile.Read(iBufferCount*KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames, iBufferPtr, KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames);
		iBufferCount++;
		if ((iBufferCount > KNumberOfBuffersToComplete)||(iError))
			{
			iActiveScheduler->Stop();
			}
		else
			{
			iA2dpBTHeadsetAudioInterface.PlayData(iBufferPtr,*iStatus);
			iBytesSent+=iBufferPtr.Size();
			iAsyncTestStepNotifier->HandleAsyncRequest();
			}
		}
	}

	
	
CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0314::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0314() : iBufferPtr(0,0,0)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0314-HP");
	}

/**
Pause Resume
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0314::DoTestStepL( void )
	{
	TVerdict verdict = EPass;

	TFourCC datatype(KMMFFourCCCodePCM16);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	iStatus = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(KDefaultTestSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the error is not supported we will carry on with the test
				iError = KErrNone;
				}
			}
		}
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetChannels(EMMFMono,EMMFNone);
		}
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Configuration error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*iStatus);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
		if (!iError)
		{
		//try sending some data.
		TFileName filename(Kpcm16_test_07);//_L("\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_07.wav");
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, filename, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			iBuffer = HBufC8::NewL(KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames);//4 SBC frames = 384 bytes pcm16
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(bufferPtr, KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames));
			iBufferCount = 1;
			iStreaming = ETrue;
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}
			if (verdict == EPass)
				{
				//five buffers have been sent -some of these should still
				//be buffered up inside the A2dpBTHeadsetAudioInterface
				//so stop them being played out by pausing
				//the request status iStatus should not complete
				//until we resume
				iStreaming = EFalse;
				iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*iStatus);
				iA2dpBTHeadsetAudioInterface.PauseBuffer();
				iPaused = ETrue;
				//ensure no request status generated after a time of say 5 seconds
				iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
				iAsyncTestStepNotifier->HandleAsyncRequest();
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("Play Data Error after pausing %d" ), iError);
					verdict = EFail;
					}
				else
					{
					//now resume  - should playout the paused buffers
					//and complete the request status
					iA2dpBTHeadsetAudioInterface.ResumePlaying();
					iAsyncTestStepNotifier->HandleAsyncRequest();
					if (iError != KErrCompletion)
						{
						INFO_PRINTF2(_L("Failed to complete outstanding PlayData request status %d" ), iError);
						verdict = EFail;
						}
					else
						{
						iError = KErrNone; //reset
						}
					}
				}
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			delete iBuffer;
			iBuffer = NULL;
			iFile.Close();
			iFileSession.Close();
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
		
	return verdict;
	}

	
void CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0314::AsyncNotification(TInt aError) 
	{
	iError = aError;
	if (iPaused)
		{//we'll use KErrCompletion to indicate the PlayData request status has been completed when it shouldn't
		iError = KErrCompletion; 
		}
	if (!iStreaming)
		{
		iActiveScheduler->Stop();
		}
	else
		{
		//send 5 buffers
		iBufferPtr.Set(iBuffer->Des());
		iError = iFile.Read(iBufferCount*KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames, iBufferPtr, KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames);
		iBufferCount++;
		if ((iBufferCount == KNumberOfBuffersToComplete)||(iError))
			{
			iActiveScheduler->Stop();
			}
		else
			{
			iA2dpBTHeadsetAudioInterface.PlayData(iBufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			}
		}
	}
	
	

CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0315::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0315()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0315-HP");
	}

/**
Notify Error
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0315::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	TFourCC datatype(KMMFFourCCCodePCM16);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(KDefaultTestSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the error is not supported we will carry on with the test
				iError = KErrNone;
				}
			}
		}
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetChannels(EMMFMono,EMMFNone);
		}
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Configuration error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
	if (!iError)
		{
		//try sending some data.
		TFileName filename(Kpcm16_test_07);//_L("\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_07.wav");
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, filename, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			//create a separate async test step notifier for NotifyError
			CAsyncTestStepNotifier* asyncTestStepErrorNotifier = CAsyncTestStepNotifier::NewL(this);
			TRequestStatus* notifyStatus = &(asyncTestStepErrorNotifier->RequestStatus());
			iA2dpBTHeadsetAudioInterface.NotifyError(*notifyStatus);
			iBuffer = HBufC8::NewL(KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames);//4 SBC frames = 384 bytes
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(bufferPtr,KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames));
			
			//not sure why the line below is needed
			//but if it's not then a stray request panic in the
			//active scheduler occurs after calling playData
			//from this class as it has not been set active
			//despite the fact that it shouldn't be generating any events 
			asyncTestStepErrorNotifier->SetActiveIfNotActive();
			
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*status);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}	
			//now disable the link - note that there is a timelag between a link being
			//disabled and the lower layers picking it up	
			iBtPhysicalLink->Disconnect(*iBTheadsetAddress);	
			//put in a timeout
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeToRegisterLinkLost));
			asyncTestStepErrorNotifier->HandleAsyncRequest();
		
			if (*notifyStatus == KRequestPending)
				{
				INFO_PRINTF2(_L("notifyStatus is still pending %d" ), iError);
				verdict = EFail;
				}
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			delete iBuffer;
			iBuffer = NULL;
			iFile.Close();
			iFileSession.Close();
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
	return verdict;
	}
	
	

CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0316::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0316()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0316-HP");
	}

/**
Cancel Notify Error
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0316::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	TFourCC datatype(KMMFFourCCCodePCM16);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(KDefaultTestSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the error is not supported we will carry on with the test
				iError = KErrNone;
				}
			}
		}
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetChannels(EMMFMono,EMMFNone);
		}
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Configuration error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
	if (!iError)
		{
		//try sending some data.
		TFileName filename(Kpcm16_test_07);//_L("\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_07.wav");
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, filename, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			//create a separate async test step notifier for NotifyError
			CAsyncTestStepNotifier* asyncTestStepErrorNotifier = CAsyncTestStepNotifier::NewL(this);
			TRequestStatus* notifyStatus = &(asyncTestStepErrorNotifier->RequestStatus());
			iA2dpBTHeadsetAudioInterface.NotifyError(*notifyStatus);
			iBuffer = HBufC8::NewL(KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames);//4 SBC frames = 384 bytes
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(bufferPtr,KPcm16_TestFile07FrameSize*KDefaultNumberOfSBCFrames));
			
			//not sure why the line below is needed
			//but if it's not then a stray request panic in the
			//active scheduler occurs after calling playData
			//from this class as it has not been set active
			//despite the fact that it shouldn't be generating any events 
			asyncTestStepErrorNotifier->SetActiveIfNotActive();
			
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*status);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}
			//now disable the link - note that there is a timelag between a link being
			//disabled and the lower layers picking it up
			iA2dpBTHeadsetAudioInterface.CancelNotifyError();	
			iBtPhysicalLink->Disconnect(*iBTheadsetAddress);	
			//put in a timeout
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeToRegisterLinkLost));	
			asyncTestStepErrorNotifier->HandleAsyncRequest();
			if (*notifyStatus != KErrCancel)
				{
				INFO_PRINTF1(_L("request status not still be pending" ));
				verdict = EFail;
				}
			else if (iError != KErrCancel)
				{
				INFO_PRINTF2(_L("expected KErrCancel got error %d" ), iError);
				verdict = EFail;
				}
			else
				{
				iError = KErrNone; //reset error
				}
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			delete iBuffer;
			iBuffer = NULL;
			iFile.Close();
			iFileSession.Close();
			delete asyncTestStepErrorNotifier;		
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
	return verdict;
	}


CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0320::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0320() : iBufferPtr(0,0,0)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0320-HP");
	}

/**
PlayData multiple 44100Hz stereo 4K pcm16 buffers
Note we're actually using 32K mono for now due to blueant headset reconfig issue
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0320::DoTestStepL( void )
	{
	TVerdict verdict = EPass;

	TFourCC datatype(KMMFFourCCCodePCM16);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	iStatus = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(KDefaultTestSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the error is not supported we will carry on with the test
				iError = KErrNone;
				}
			}
		}
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetChannels(EMMFMono,EMMFNone);
		}
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Configuration error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*iStatus);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
		if (!iError)
		{
		//try sending some data.
		TFileName filename(Kpcm16_test_07);//_L("\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_07.wav");
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, filename, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			iBuffer = HBufC8::NewL(KLargeBufferTestSize);
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(bufferPtr,KLargeBufferTestSize));
			iFilePos = bufferPtr.Length();
			iStreaming = ETrue;
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
		
	iA2dpBTHeadsetAudioInterface.CloseDevice(*iStatus);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("error closing device %d" ), iError);
		verdict = EFail;
		}
	iFile.Close();
	iFileSession.Close();
	delete iBuffer;
	iBuffer = NULL;	
	return verdict;
	}

	
void CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0320::AsyncNotification(TInt aError) 
	{
	iError = aError;
	TInt fileSize = 0;
	if (!iStreaming)
		{
		iActiveScheduler->Stop();
		}
	else if (!iError)
		{
		//send next buffer
		iBufferPtr.Set(iBuffer->Des());
		iError = iFile.Read(iFilePos, iBufferPtr, KLargeBufferTestSize);
		if (!iError)
			{
			iFilePos += iBufferPtr.Length();
			TInt err = iFile.Size(fileSize);
			}	
		if ((iBufferPtr.Size() == 0) || (iError))
			{//we have reached the end of the file
			iActiveScheduler->Stop();
			}
		else
			{
			iA2dpBTHeadsetAudioInterface.PlayData(iBufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			}
		}
	else
		{
		iActiveScheduler->Stop();
		}
	}	

	

CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0350::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0350() : iBufferPtr(0,0,0)
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0350-HP");
	}

/**
PlayData mp3
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0350::DoTestStepL( void )
	{
	TVerdict verdict = EPass;

	TFourCC datatype(KMMFFourCCCodeMP3);
	iError = iA2dpBTHeadsetAudioInterface.SetDataType(datatype);
	iStatus = &(iAsyncTestStepNotifier->RequestStatus());
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface.SetSampleRate(KDefaultTestSampleRate);
		if (iError)
			{
			INFO_PRINTF2(_L("error setting sample rate %d " ), iError);
			if (iError == KErrNotSupported)
				{//if the error is not supported we will carry on with the test
				iError = KErrNone;
				}
			}
		}
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Configuration error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		iA2dpBTHeadsetAudioInterface.OpenDevice(*iStatus);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error opening device %d" ), iError);
			verdict = EInconclusive;
			}
		}
		if (!iError)
		{
		//try sending some data.
		TFileName filename(Kmp3_test32kbs);//temp TEST
//		TFileName filename = Kmp3Stereo_test32kbs;//temp TEST
		User::LeaveIfError(iFileSession.Connect());
		iError = iFile.Open(iFileSession, filename, EFileRead);
		if (iError)
			{
			INFO_PRINTF2(_L("error opening test file %d" ), iError);
			verdict = EInconclusive;
			}
		else
			{
			iBuffer = HBufC8::NewL(KMp3BufferTestSize);
			TPtr8 bufferPtr(iBuffer->Des());
			User::LeaveIfError(iFile.Read(bufferPtr,KMp3BufferTestSize));
			iFilePos = bufferPtr.Length();
			iStreaming = ETrue;
			iA2dpBTHeadsetAudioInterface.PlayData(bufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Play Data Error %d" ), iError);
				verdict = EFail;
				}
			//the completion of the request status only means that the
			//data buffer has been accepted by the a2dpBTheadsetAudioInterface
			//it does not mean that it has yet been sent to the headset
			//so keep active scheduler running for another 5 seconds
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KMaxTimeToPlayOutCachedBuffers));
			CActiveScheduler::Current()->Start();
			delete iBuffer;
			iBuffer = NULL;
			iFile.Close();
			iFileSession.Close();
			}
		}
	if ((iError)&&(verdict != EInconclusive))
		{
		verdict = EFail;
		}
		
	return verdict;
	}

	
void CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0350::AsyncNotification(TInt aError) 
	{
	iError = aError;
	TInt fileSize = 0;
	if (!iStreaming)
		{
		iActiveScheduler->Stop();
		}
	else if (!iError)
		{
		//send next buffer
		TPtr8 iBufferPtr(iBuffer->Des());
		iError = iFile.Read(iFilePos, iBufferPtr, KMp3BufferTestSize);
		if (!iError)
			{
			iFilePos += iBufferPtr.Length();
			TInt err = iFile.Size(fileSize);
			}	
		if ((iBufferPtr.Size() == 0) || (iError))
			{//we have reached the end of the file
			iActiveScheduler->Stop();
			}
		else
			{
			iA2dpBTHeadsetAudioInterface.PlayData(iBufferPtr,*iStatus);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			}
		}
	else
		{
		iActiveScheduler->Stop();
		}
	}			



	
