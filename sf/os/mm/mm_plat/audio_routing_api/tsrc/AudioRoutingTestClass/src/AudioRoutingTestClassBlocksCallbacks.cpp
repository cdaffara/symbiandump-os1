/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  AudioroutingControl test module
*
*/



// INCLUDE FILES
#include "AudioRoutingTestClass.h"
#include "debug.h"





/************************************************************************************************************/

//Devsound initialization is completed.Configure devsound capabilities.
void CAudioRoutingTestClass::InitializeComplete(TInt aError)
	{
	if (aError)
		{
		FTRACE(FPrint(_L("InitializeComplete Error, return code =%d!!!"),aError));
		iLog->Log(_L("InitializeComplete Error, return code =%d!!!"),aError);
		}
	else
		{
		FTRACE(FPrint(_L("InitializeComplete")));
		iLog->Log(_L("InitializeComplete OK, return code =%d!!!"),aError);
		}
    ProcessEvent(EInitializeComplete, aError);

	}


void CAudioRoutingTestClass::ToneFinished(TInt aError)
   	{
    FTRACE(FPrint(_L("CAudioRoutingTestClass::ToneFinished")));
    iLog->Log(_L("ToneFinished, return code =%d!!!"),aError);
 //	Panic(KErrNotSupported);
 	}

//This is called when an empty audio buffer is available.Fill the buffer with audio data.
void CAudioRoutingTestClass::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
    FTRACE(FPrint(_L("CAudioRoutingTestClass::BufferToBeFilled")));
    iLog->Log(_L("BufferToBeFilled, return code =!!!"));
	}

//This is called when an audio play completion is successfully played or otherwise
void CAudioRoutingTestClass::PlayError(TInt  aError)
 	{
    FTRACE(FPrint(_L("CAudioRoutingTestClass::PlayError")));
    iLog->Log(_L("PlayError, return code =%d!!!"),aError);
	}


//Will panic if called.Should not be called during playing
void CAudioRoutingTestClass::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	 {
	    FTRACE(FPrint(_L("CAudioRoutingTestClass::BufferToBeEmptied")));
	    iLog->Log(_L("BufferToBeEmptied, return code =!!!"));
	 }


//Will panic if called.Should not be called during playing
void CAudioRoutingTestClass::RecordError(TInt aError)
	 {
	    FTRACE(FPrint(_L("CAudioRoutingTestClass::RecordError")));
	    iLog->Log(_L("RecordError, return code =%d!!!"),aError);
	 }


//Will panic if called.Should not be called during playing
void CAudioRoutingTestClass::ConvertError(TInt aError)
	 {
	    FTRACE(FPrint(_L("CAudioRoutingTestClass::ConvertError")));
	    iLog->Log(_L("ConvertError, return code =%d!!!"),aError);
	 }


//Will panic if called.Should not be called during playing
void CAudioRoutingTestClass::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	 {
//	 Panic(KErrNotSupported);
	 }

void CAudioRoutingTestClass::MapcInitComplete(TInt aError,const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
    iLog->Log(_L("CAudioInput::MapcInitComplete(*iRecorder) aError = %d!!!"),aError);
	if (isInput != EFalse)
		{
		TInt err = KErrNone;
		TRAP(err,iRecorder->OpenFileL(_L("c:\\testrecord.amr")));
		if (err!=KErrNone)
			{
		    iLog->Log(_L("iRecorder->OpenFileL Failed =%d!!!"),err);
			}
		}
	}
void CAudioRoutingTestClass::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
		if (aCurrentState == CMdaAudioClipUtility::EOpen)
			{
			if (isInput != EFalse)
				{
			    iLog->Log(_L("MoscoStateChangeEvent = %d!!!"),aErrorCode);
				}
			}
	}

void CAudioRoutingTestClass::MapcPlayComplete(
				TInt /*aError*/
				)
	{

	}

void CAudioRoutingTestClass::MaiscOpenComplete(TInt aError)
	{
	iPlayError = aError;
	if (aError==KErrNone && iAudioInputStream)
		{
		iReady = ETrue;

		iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
		iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;

		iAudioInputStream->SetAudioPropertiesL(iStreamSettings.iSampleRate,iStreamSettings.iChannels);
		iAudioInputStream->SetGain(iAudioInputStream->MaxGain());
		iAudioInputStream->SetPriority(EPriorityNormal, EMdaPriorityPreferenceTime);
		iStreamIdx=0;
		iAudioInputStream->ReadL(*iStreamBuffer[iStreamIdx]);
		}
	}


void CAudioRoutingTestClass::MaiscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{
	iPlayError = aError;
	if (aError==KErrNone && iAudioInputStream)
		{
		if (&aBuffer==iStreamBuffer[iStreamBuffer.Count()-1])
			iStreamIdx=0;
		else
			iStreamIdx++;

		iAudioInputStream->ReadL(*iStreamBuffer[iStreamIdx]);
		if(aBuffer.Length())
			{
			TInt ll = aBuffer.Length() + iSoundData->Des().Length();

			if(ll < KMaxFileSize)
				{
				iSoundData->Des().Append(aBuffer);
				}
			else
				{
				iAudioInputStream->Stop();
				}
			}
		}
	}

void CAudioRoutingTestClass::MaiscRecordComplete(TInt aError)
	{
	iPlayError = aError;
	}


void CAudioRoutingTestClass::DefaultAudioOutputChanged( CAudioOutput& /*aAudioOutput*/,
#ifdef _DEBUG
		                                CAudioOutput::TAudioOutputPreference aNewDefault )
#else
		                                CAudioOutput::TAudioOutputPreference /*aNewDefault*/ )
#endif
	{
    #ifdef _DEBUG
		iLog->Log(_L("CRoutingAppUi::DefaultAudioOutputChanged - aNewDefault = %d"),aNewDefault);
    #endif
	}


	// Audio Output Stream
void CAudioRoutingTestClass::MaoscOpenComplete(TInt aError)
	{
    iLog->Log(_L("MaoscOpenComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
    iLog->Log(_L("MaoscBufferCopied, return code =%d!!!"),aError);
   // ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MaoscPlayComplete(TInt aError)
	{
    iLog->Log(_L("MaoscPlayComplete, return code =%d!!!"),aError);
   // ProcessEvent(EInitializeComplete, aError);
	}

// Audio Tone Observer
void CAudioRoutingTestClass::MatoPrepareComplete(TInt aError)
	{
    iLog->Log(_L("MatoPrepareComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MatoPlayComplete(TInt aError)
	{
    iLog->Log(_L("MatoPlayComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}


// MidiClientUtilityObserver
void CAudioRoutingTestClass::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState /*aNewState*/,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
    iLog->Log(_L("MmcuoStateChanged, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MmcuoTempoChanged(TInt aMicroBeatsPerMinute)
	{
    iLog->Log(_L("MmcuoTempoChanged, return aMicroBeatsPerMinute =%d!!!"),aMicroBeatsPerMinute);
	}

void CAudioRoutingTestClass::MmcuoVolumeChanged(TInt aChannel,TReal32 /*aVolumeInDecibels*/)
	{
    iLog->Log(_L("MmcuoVolumeChanged, return aChannel =%d!!!"),aChannel);
	}

void CAudioRoutingTestClass::MmcuoMuteChanged(TInt aChannel,TBool /*aMuted*/)
	{
    iLog->Log(_L("MmcuoMuteChanged, return aChannel =%d!!!"),aChannel);
	}

void CAudioRoutingTestClass::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 aMicroBeats)
	{
    iLog->Log(_L("MmcuoSyncUpdate, return aMicroBeats =%d!!!"),aMicroBeats);
	}

void CAudioRoutingTestClass::MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
    iLog->Log(_L("MmcuoMetaDataEntryFound, return aMetaDataEntryId =%d!!!"),aMetaDataEntryId);
	}

void CAudioRoutingTestClass::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aMessage*/)
	{
    iLog->Log(_L("MmcuoMipMessageReceived, !!!"));
	}

void CAudioRoutingTestClass::MmcuoPolyphonyChanged(TInt aNewPolyphony)
	{
    iLog->Log(_L("MmcuoPolyphonyChanged, return code =%d!!!"),aNewPolyphony);
	}

void CAudioRoutingTestClass::MmcuoInstrumentChanged(TInt aChannel,TInt /*aBankId*/,TInt /*aInstrumentId*/)
	{
    iLog->Log(_L("MmcuoInstrumentChanged, return aChannel =%d!!!"),aChannel);
	}

//MDrmAudioPlayerCallback
void CAudioRoutingTestClass::MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
    iLog->Log(_L("MdapcInitComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MdapcPlayComplete(TInt aError)
	{
    iLog->Log(_L("MdapcPlayComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}


//MVideoPlayerUtilityObserver
void CAudioRoutingTestClass::MvpuoOpenComplete(TInt aError)
	{
    iLog->Log(_L("MvpuoOpenComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	if (aError != KErrNone)
		{
		iLog->Log(_L("Cannot open "));
		_LIT(K3gpTestFile,"e:\\testing\\data\\videoTest.3gp");
		iLog->Log(K3gpTestFile);
		iLog->Log(_L("\nPress any key to exit ..."));
		}
	else
		{
		iLog->Log(_L("Ready!"));
		}
	if (isInput != EFalse)
		{}
	CActiveScheduler::Stop();
	}

void CAudioRoutingTestClass::MvpuoPrepareComplete(TInt aError)
	{
    iLog->Log(_L("MvpuoPrepareComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MvpuoFrameReady(CFbsBitmap& /*aFrame*/,TInt aError)
	{
    iLog->Log(_L("MvpuoFrameReady, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MvpuoPlayComplete(TInt aError)
	{
    iLog->Log(_L("MvpuoPlayComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MvpuoEvent(const TMMFEvent& aEvent)
	{
    iLog->Log(_L("MvpuoEvent, return code =%x!!!"),aEvent);
	}

void CAudioRoutingTestClass::MvruoOpenComplete(TInt aError)
	{
    iLog->Log(_L("MvruoOpenComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MvruoPrepareComplete(TInt aError)
	{
    iLog->Log(_L("MvruoPrepareComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MvruoRecordComplete(TInt aError)
	{
    iLog->Log(_L("MvruoRecordComplete, return code =%d!!!"),aError);
    //ProcessEvent(EInitializeComplete, aError);
	}

void CAudioRoutingTestClass::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
    iLog->Log(_L("MvruoEvent, return code =!!!")/*,aEvent*/);
	}

// End of File
