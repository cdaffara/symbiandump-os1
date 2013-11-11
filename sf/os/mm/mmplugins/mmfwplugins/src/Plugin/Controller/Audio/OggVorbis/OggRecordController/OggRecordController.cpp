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

#include "OggRecordController.h"

//Table that maps given samplerate with the MMF samplerate
const TSampleRateTable KRateLookup[]=  {
										{96000,EMMFSampleRate96000Hz},
        						  		{88200,EMMFSampleRate88200Hz},
                   				  		{48000,EMMFSampleRate48000Hz},
			                   	  		{44100,EMMFSampleRate44100Hz},
			                   	  		{32000,EMMFSampleRate32000Hz},
			                   	  		{22050,EMMFSampleRate22050Hz},
			                   	  		{16000,EMMFSampleRate16000Hz},
			                   	  		{11025,EMMFSampleRate11025Hz},
			                   	  		{8000, EMMFSampleRate8000Hz},
			                   	  		{12000,EMMFSampleRate12000Hz},
			                   	  		{24000, EMMFSampleRate24000Hz}
                   				 	   };

/**
This method generates a panic

@param aPanicCode internal to this dll.
*/
void Panic(TInt aPanicCode)
	{
	_LIT(KOggRecordCtrlPanicCategory, "OggRecordController");
	User::Panic(KOggRecordCtrlPanicCategory, aPanicCode);
	}
	
COggRecordController* COggRecordController::NewL()
    {
    COggRecordController* self = new(ELeave)COggRecordController;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
COggRecordController::COggRecordController()
    :iState(EStopped)
    {
    
	}
    
COggRecordController::~COggRecordController()
    {
    if(iSource)
    	{
    	iSource->SourceThreadLogoff();
    	}
    if(iClip)
    	{
    	iClip->SinkThreadLogoff();
    	}
    delete iOggSink;
    delete iEventHandler;
    delete iMessage;
    }
    
void COggRecordController::ConstructL()
    {
	// Construct custom command parsers
	//audRecordDevParser
	CMMFAudioRecordDeviceCustomCommandParser* audRecordDevParser = CMMFAudioRecordDeviceCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audRecordDevParser);
	AddCustomCommandParserL(*audRecordDevParser);
	CleanupStack::Pop( audRecordDevParser );

	//audRecordConParser
	CMMFAudioRecordControllerCustomCommandParser* audRecordConParser = CMMFAudioRecordControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audRecordConParser);
	AddCustomCommandParserL(*audRecordConParser);
	CleanupStack::Pop(audRecordConParser);
    
    //audiocontroller parser
    CMMFAudioControllerCustomCommandParser* audConParser = CMMFAudioControllerCustomCommandParser::NewL(*this);
 	CleanupStack::PushL(audConParser);
 	AddCustomCommandParserL(*audConParser);
 	CleanupStack::Pop(audConParser);
    
    iEventHandler = new(ELeave) CAsyncEventHandler(*this);
    }
     
void COggRecordController::AddDataSourceL(MDataSource& aSource)
    {
    if (iState!= EStopped) 
    	{
    	User::Leave(KErrNotReady);
    	}
    if (aSource.DataSourceType() != KUidMmfAudioInput)
    	{
    	/*The reason for returning KErrNotFound instead of KErrNotSupported is that our recorderutility tries to load 
    	recordcontroller for playback also if the user does not provide playbackcontrolleruid. So we receive
    	filesource here and if we return KErrNotFound from this place, utility ignores the error and continues record atleast.
    	If we return any other error, recording will not get started at all*/ 
    	User::Leave(KErrNotFound);
    	}
    if(iMMFDevSound)
    	{
    	User::Leave(KErrAlreadyExists);
    	}
    iSource = &aSource;
    MMMFAudioInput* audioInput = static_cast<MMMFAudioInput*>(&aSource);
    User::LeaveIfError(audioInput->SourceThreadLogon(*iEventHandler));
    iMMFDevSound = &(audioInput->SoundDevice());
	iMMFDevSound->SetPrioritySettings(iPrioritySettings);
	}
    
void COggRecordController::AddDataSinkL(MDataSink& aSink)
    {
    if (iState!= EStopped) 
    	{
    	User::Leave(KErrNotReady);
    	}
    if (iClip)
    	{
    	User::Leave(KErrAlreadyExists);
    	}
	if ((aSink.DataSinkType()==KUidMmfDescriptorSink)||
			(aSink.DataSinkType()==KUidMmfFileSink))
	    {
	    User::LeaveIfError(aSink.SinkThreadLogon(*iEventHandler));
	    iClip = static_cast<CMMFClip*>(&aSink);
	    TRAPD(err, iOggSink = COggEncode::NewL(*this, *iClip));
	    if(err)
	    	{
	    	aSink.SinkThreadLogoff();
	    	iClip = NULL;
	    	User::Leave(err);
	    	}
	    }
	else 
		{
		/*The reason for returning KErrNotFound instead of KErrNotSupported is that our recorderutility tries to load 
    	recordcontroller for playback also if the user does not provide playbackcontrolleruid. So we receive
    	audiooutput here and if we return KErrNotFound from this place, utility ignores the error and continues record atleast.
    	If we return any other error, recording will not get started at all*/ 
		User::Leave(KErrNotFound);
		}
	}

void COggRecordController::RemoveDataSourceL(MDataSource& aSource)
    {
    if((!iMMFDevSound)||(iState!= EStopped))
    	{
    	User::Leave(KErrNotReady);
    	}
    if (aSource.DataSourceType() != KUidMmfAudioInput)
    	{
    	User::Leave(KErrNotSupported);
    	}
    MMMFAudioInput* audioInput = static_cast<MMMFAudioInput*>(&aSource);
    CMMFDevSound& devSound = audioInput->SoundDevice();
	if (iMMFDevSound != &devSound) 
		{
		User::Leave(KErrArgument);
		}
	else
		{
		iSource->SourceThreadLogoff();
		iMMFDevSound = NULL;
		}
    }

void COggRecordController::RemoveDataSinkL(MDataSink& aSink)
    {
    if((!iClip)||(iState!= EStopped))
    	{
    	User::Leave(KErrNotReady);
    	}
    if (iClip != &aSink) 
    	{
    	User::Leave(KErrArgument);
    	}
    else
    	{
    	delete iOggSink;
    	iOggSink = NULL;
    	iClip->SinkThreadLogoff();
    	iClip = NULL;
    	}
    }
    
void COggRecordController::ResetL()
    {
    if(iState != EStopped)
    	{
    	StopL();
    	}
    if(iSource)
    	{
    	iSource->SourceThreadLogoff();
    	iMMFDevSound = NULL;
    	iSource = NULL;
    	}
    if(iClip)
    	{
    	iClip->SinkThreadLogoff();
    	iClip=NULL;
    	}
    delete iOggSink;
    iOggSink = NULL;
    }
    
void COggRecordController::PrimeL(TMMFMessage& aMessage)
	{
	if ((iMMFDevSound==NULL)||(iClip==NULL)||(iState != EStopped))
    	{
    	User::Leave(KErrNotReady);
    	}
	ASSERT(!iMessage);
	iMessage = CMMFMessageHolder::NewL(aMessage);
	StartPrimeL();
	iState = EPriming;
	}
	
void COggRecordController::PrimeL()
    {
    Panic(EBadCall);
    }
    
void COggRecordController::StartPrimeL()
    {
    iClip->SinkPrimeL();
    if(iClip->Size())
    	{
    	// OggRecordController does not support recording into an already existing file.
    	// This is due to unavailability of APIs at the level of Ogg Vorbis C libraries.
    	User::Leave(KErrNotSupported);
    	}
    iMMFDevSound->InitializeL(*this, KMMFFourCCCodeVORB, EMMFStateRecording);
    }
    
void COggRecordController::PlayL()
    {
    if (iState!=EPrimed)
    	{
    	User::Leave(KErrNotReady);
    	}
    iClip->SinkPlayL();
    iMMFDevSound->RecordInitL();
	iState=ERecording;
    }
    
void COggRecordController::PauseL()
    {
    //we dont have Pause functionality in recorderutility!
    User::Leave(KErrNotSupported);
    }
    
void COggRecordController::StopL(TMMFMessage& aMessage)
	{
	if ((iState == ERecording) && !iMessage)
		{
		/*we will not stop the device immd.ly. we first pause it and wait for the last
		buffer from devsound and then we stop recording.*/ 
		iMessage = CMMFMessageHolder::NewL(aMessage);
		iMMFDevSound->Pause();
		}
	else
		{
		if(iState == EStopped)
			{
			aMessage.Complete(KErrNotReady);
			}
		else
			{
			StopL();
			aMessage.Complete(KErrNone);
			}
		}
	}
	
void COggRecordController::StopL()
    {
    if (iState == EStopped)
    	{
    	return;	
    	}
    iMMFDevSound->Stop();
    iClip->SinkStopL();
    iOggSink->Reset();
    iState=EStopped;
	}

TTimeIntervalMicroSeconds COggRecordController::PositionL() const
    {
    if(iState == EStopped || !iOggSink)
    	{
    	User::Leave(KErrNotReady);
    	}
    if(iState == ERecording)
    	{
    	TInt samples = iMMFDevSound->SamplesRecorded();
    	TInt64 microS=TInt64(samples)*TInt64(1000000)/TInt64(iOggSink->SampleRate());
    	return TTimeIntervalMicroSeconds(microS);
    	}
    else
    	{
    	return iOggSink->PositionL();
    	}
    }
    
void COggRecordController::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
    {
    // we dont allow the changing of the position once recording is started!
    if(iState != EPrimed || !iOggSink)
    	{
    	User::Leave(KErrNotReady);
    	}
    iOggSink->SetPositionL(aPosition);
    }
    
TTimeIntervalMicroSeconds COggRecordController::DurationL() const
    {
    LeaveIfNullL(iOggSink);
	return iOggSink->DurationL();
    }
    
void COggRecordController::SetPrioritySettings(const TMMFPrioritySettings& aSettings)
    {
    iPrioritySettings = aSettings;
    if (iMMFDevSound) 
    	{
    	iMMFDevSound->SetPrioritySettings(aSettings);
    	}
    }
    
void COggRecordController::GetNumberOfMetaDataEntriesL(TInt& aNumber)
    {
    LeaveIfNullL(iOggSink);
    aNumber = iOggSink->Info()->Comments();
    }
    
CMMFMetaDataEntry* COggRecordController::GetMetaDataEntryL(TInt aIndex)
    {
    LeaveIfNullL(iOggSink);
    if(!iOggSink->Info()->Comments())
    	{
    	User::Leave(KErrNotFound);
    	}
    CVorbisComment& c = iOggSink->Info()->GetComment(aIndex);
    return CMMFMetaDataEntry::NewL(c.Name(), c.Value());
    }
    
void COggRecordController::LeaveIfNullL(TAny* aPtr)
	{
	if (!aPtr) 
    	{
    	User::Leave(KErrNotReady);
    	}
	}
	
void COggRecordController::SendErrorToClient(TInt aError)
	{
	TInt err = aError;
	if(iState != EStopped)
		{
		/* 
		 Stop at the utility layer has no notion of failing. So the Stop at the controller layer 
		 is treated as the housekeeping action and we ignore all the errors here during Stopping the controller.
		 (taken from JF's comments for INC080041)
		 */
		TRAP_IGNORE(StopL());
		}

	if(iMessage)
		{
		iMessage->Complete(err);
		delete iMessage;
		iMessage = NULL;
		}
	else
		{
		TMMFEvent controllerEvent(KMMFEventCategoryPlaybackComplete, err);
		SendEventToClient(controllerEvent);	
		}
	}

void COggRecordController::BufferEmptied()
	{
	if((iMessage) && iOggSink->IsAllDataWritten())
		{
		//we are ready to stop the recording
		SendErrorToClient(KErrNone);
		return;
		}
	if(iMMFDevSound)
		{
		iMMFDevSound->RecordData();	
		}
	}
	
void COggRecordController::ConvertError(TInt /*aError*/)
	{
	Panic(EBadCall);
	}

void COggRecordController::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	Panic(EBadCall);
	}

void COggRecordController::ToneFinished(TInt /*aError*/)
	{
	Panic(EBadCall);
	}

void COggRecordController::RecordError(TInt aError)
	{
	SendErrorToClient(aError);
	}


void COggRecordController::InitializeComplete(TInt aError)
	{
	TInt err = aError;
	if (err == KErrNone)
		{
		TMMFCapabilities devSoundCaps = iMMFDevSound->Capabilities();
		TMMFCapabilities caps;
		TInt rate = iOggSink->SampleRate();
		TBool found = EFalse;
		for( TInt index =0; index < KNumSampleRates; index++)
			{
			if((rate == KRateLookup[index].iRate) && (devSoundCaps.iRate & KRateLookup[index].iRateEnum))
				{
				caps.iRate = KRateLookup[index].iRateEnum;
				found = ETrue;
				}
			}
    	if(!found)
    		{
    		err = KErrNotFound;
    		}
    	if(err == KErrNone)
    		{
    	TInt channels = iOggSink->NumChannels();
			if (channels == 1) 
				{
				caps.iChannels = EMMFMono;
				}
			else if (channels == 2)
				{
				caps.iChannels = EMMFStereo;
				}
			else 
				{
				err = KErrNotFound;
				}
	    	if(err == KErrNone)
		    	{
		    	caps.iEncoding = EMMFSoundEncoding16BitPCM;
				TRAP(err,iMMFDevSound->SetConfigL(caps));
		        if(err == KErrNone)
		        	{
		        	MMMFDevSoundCustomInterfaceBitRate* ptr = static_cast <MMMFDevSoundCustomInterfaceBitRate*>(iMMFDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate));
		        	if(ptr)
		        		{
		        		/*
				         Encode using a quality mode, but select that quality mode by asking for the bitrate nominal. 
				         This is not ABR, it is true VBR, but selected using the bitrate interface, 
				         When our clientutilities support setting max and min bitrates, we need to add a new custom interface on devsound and vorbiscodec to set them on codec.
				   		 */
				        TRAP(err,ptr->SetBitRateL(iOggSink->BitRate()));
			        	if(err == KErrNone)
			        		{
			        		iState=EPrimed;
					        iMessage->Complete(KErrNone);
							delete iMessage;
							iMessage = NULL;
			        		}
		        		}
			        else
			        	{
			        	err = KErrNotFound;
			        	}
		        	}
				}
			    
		    }
		}
	if(err)
		{
		SendErrorToClient(err);
		}
	}

void COggRecordController::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	if (!CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		SendErrorToClient(KErrNotSupported);
		}
	CMMFDataBuffer* buf = static_cast<CMMFDataBuffer*>(aBuffer);
	if((iMessage) && (!buf->Data().Length())&&(buf->LastBuffer()))
		{
		//we are ready to stop the recording
		SendErrorToClient(KErrNone);
		return;
		}
	TRAPD(err, iOggSink->EmptyThisBufferL(aBuffer));
	//if error, stop devsound and send the error to client
	if(err)
		{
		SendErrorToClient(err);
		}
	}

void COggRecordController::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
    Panic(EBadCall);
	}

void COggRecordController::PlayError(TInt /*aError*/)
	{
	Panic(EBadCall);
	}

void COggRecordController::SendEventToClient(const TMMFEvent& aEvent)
	{
	DoSendEventToClient(aEvent);
	}

//MMMFAudioRecordDeviceCustomCommandImplementor
void COggRecordController::MardSetGainL(TInt aGain)
	{
	LeaveIfNullL(iMMFDevSound);
    iMMFDevSound->SetGain(aGain);
	}
		
void COggRecordController::MardGetMaxGainL(TInt& aMaxGain)
	{
	LeaveIfNullL(iMMFDevSound);
    aMaxGain = iMMFDevSound->MaxGain();
	}

void COggRecordController::MardGetGainL(TInt& aGain)
	{
	LeaveIfNullL(iMMFDevSound);
    aGain = iMMFDevSound->Gain();
	}

/**
 Calculates left and right from given aBalance
 Preconditions:
 !(aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight)
 y = m x + c
 left = m ( aBalance ) + c
 when aBalance = KMMFBalanceMaxLeft   left = 100
 when aBalance = KMMFBalanceMaxRight  left = 0
 100 = m( KMMFBalanceMaxLeft ) + c
 0   = m( KMMFBalanceMaxRight ) + c 
 c = -(KMMFBalanceMaxRight) m
 100 = m(KMMFBalanceMaxLeft ) - m(KMMFBalanceMaxRight)
 m = 100/(KMMFBalanceMaxLeft - KMMFBalanceMaxRight )
 c = -(KMMFBalanceMaxRight) * 100 /(KMMFBalanceMaxLeft - KMMFBalanceMaxRight )
 left = ( aBalance - KMMFBalanceMaxRight ) * 100 /( KMMFBalanceMaxLeft - KMMFBalanceMaxRight )
 */
void COggRecordController::MardSetBalanceL(TInt aBalance)
	{
	LeaveIfNullL(iMMFDevSound);
    
    if (aBalance < KMMFBalanceMaxLeft)
		{
    	aBalance = KMMFBalanceMaxLeft;	
    	}
	if (aBalance > KMMFBalanceMaxRight)
		{
		aBalance = KMMFBalanceMaxRight;
		}
	TInt left = (100 * (aBalance-KMMFBalanceMaxRight)) / (KMMFBalanceMaxLeft-KMMFBalanceMaxRight);
    TInt right = 100 - left;
    iMMFDevSound->SetRecordBalanceL(left, right);
	}

/**
 Calculates aBalance from left and right balances.
 
 follows a simple straight line transformation
 y = m x + c
 m = (KMMFBalanceMaxLeft-KMMFBalanceMaxRight)/ 100 
 c = KMMFBalanceMaxRight
 by substitution
 when aLeft = 0
   KMMFBalanceMaxRight = m * 0 + c
   c = KMMFBalanceMaxRight
 when aLeft = 100
 KMMFBalanceMaxLeft = m * 100 + KMMFBalanceMaxRight
 m = ( KMMFBalanceMaxLeft - KMMFBalanceMaxRight ) /100
 */
void COggRecordController::MardGetBalanceL(TInt& aBalance)
	{
	LeaveIfNullL(iMMFDevSound);
    TInt left = 50; // arbitrary values 
	TInt right = 50;
	iMMFDevSound->GetRecordBalanceL(left, right);
    
    if ((left > 0) && (right > 0))
		{
		aBalance = (left * (KMMFBalanceMaxLeft-KMMFBalanceMaxRight))/100 + KMMFBalanceMaxRight;
		}
	else if ((left == 0) && (right == 0))
		{
		aBalance = 0;
		}
	else if ((left == 0) && (right > 0))
		{
		aBalance = 100;
		}
	else if ((left > 0) && (right == 0))
		{
		aBalance = -100;
		}
	}

//MMMFAudioRecordControllerCustomCommandImplementor
void COggRecordController::MarcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime)
	{
	LeaveIfNullL(iOggSink);
	iOggSink->GetRecordTimeAvailableL(aTime);
	}

void COggRecordController::MarcSetMaxDurationL(const TTimeIntervalMicroSeconds& )
	{
	//this API is deprecated
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MarcSetMaxFileSizeL(TInt aFileSize )
	{
	if((iState == ERecording)||(!iOggSink))
		{
		User::Leave( KErrNotReady );
		}
	if( aFileSize < 0 )
		{
		User::Leave( KErrArgument );
		}
    iOggSink->SetMaximumClipSizeL(aFileSize);
	}

void COggRecordController::MarcCropL(TBool /*aToEnd*/)
	{
	/*
	 Crop functionality could not be implemented for oggvorbis file. The reason is that cutting the file in 
	 the middle does not work for ogg stream as the header information in the oggpages needs to be continuous.
	 Also the presence of Eos page also causes problem in cropping the file. This cannot be implemented
	 until it is supported in the oggvorbis libraries as this requires editing the stream.
	 */
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MarcAddMetaDataEntryL(const CMMFMetaDataEntry& /*aNewEntry*/)
	{
	/* we need a custominterface on devsound and at the codec level to pass the metadata to the codec. we return
	not supported till the custominterface is available*/
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MarcRemoveMetaDataEntryL(TInt /*aIndex*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MarcReplaceMetaDataEntryL(TInt /*aIndex*/, const CMMFMetaDataEntry& /*aNewEntry*/)
	{
	User::Leave(KErrNotSupported);
	}

//MMMFAudioControllerCustomCommandImplementor

void COggRecordController::MacSetSourceSampleRateL(TUint /*aSampleRate*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacSetSourceNumChannelsL(TUint /*aNumChannels*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacSetSourceFormatL(TUid /*aFormatUid*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacSetSinkSampleRateL(TUint aSampleRate)
	{
	if (iState != EStopped )
		{
		User::Leave(KErrNotReady);
		}
	LeaveIfNullL(iOggSink);
	User::LeaveIfError(iOggSink->SetSampleRate(aSampleRate));
	}

void COggRecordController::MacSetSinkNumChannelsL(TUint aNumChannels)
	{
	if (iState != EStopped )
		{
		User::Leave(KErrNotReady);
		}
	LeaveIfNullL(iOggSink);
	User::LeaveIfError(iOggSink->SetNumChannels(aNumChannels));
	
	}

void COggRecordController::MacSetSinkFormatL(TUid aFormatUid)
	{
	if(( iState != EStopped ) || (!iOggSink))
 		{
 		User::Leave(KErrNotReady);
 		}
 	if(aFormatUid != TUid::Uid(KOggEncodeUid))
 		{
 		User::Leave(KErrNotSupported);
 		}
	}

void COggRecordController::MacSetCodecL(TFourCC /*aSourceDataType*/, TFourCC /*aSinkDataType*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacSetSourceBitRateL(TUint /*aBitRate*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacSetSourceDataTypeL(TFourCC /*aDataType*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacSetSinkBitRateL(TUint aRate)
	{
	if((iState != EStopped) || (!iOggSink))
		{
		User::Leave( KErrNotReady );
		}
	User::LeaveIfError(iOggSink->SetBitRate(aRate));
	}

void COggRecordController::MacSetSinkDataTypeL(TFourCC aDataType)
	{
	if(( iState != EStopped ) || (!iOggSink))
 		{
 		User::Leave(KErrNotReady);
 		}
 	if(aDataType != KMMFFourCCCodeVORB)
 		{
 		User::Leave(KErrNotSupported);	
 		}
	}

void COggRecordController::MacGetSourceSampleRateL(TUint& /*aRate*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacGetSourceBitRateL(TUint& /*aRate*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacGetSourceNumChannelsL(TUint& /*aNumChannels*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacGetSourceFormatL(TUid& /*aFormat*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacGetSourceDataTypeL(TFourCC& /*aDataType*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacGetSinkSampleRateL(TUint& aRate)
	{
	LeaveIfNullL(iOggSink);
	aRate = iOggSink->SampleRate();
	}

void COggRecordController::MacGetSinkBitRateL(TUint& aRate)
	{
	LeaveIfNullL(iOggSink);
	aRate = iOggSink->BitRate();
	}

void COggRecordController::MacGetSinkNumChannelsL(TUint& aNumChannels)
	{
	LeaveIfNullL(iOggSink);
	aNumChannels = iOggSink->NumChannels();
	}

void COggRecordController::MacGetSinkFormatL(TUid& aFormat)
	{
	LeaveIfNullL(iOggSink);
	aFormat = TUid::Uid(KOggEncodeUid);
	}

void COggRecordController::MacGetSinkDataTypeL(TFourCC& aDataType)
	{
	LeaveIfNullL(iOggSink);
	aDataType = KMMFFourCCCodeVORB;
	}

void COggRecordController::MacGetSupportedSourceSampleRatesL(RArray<TUint>& /*aSupportedRates*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacGetSupportedSourceBitRatesL(RArray<TUint>& /*aSupportedRates*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacGetSupportedSourceNumChannelsL(RArray<TUint>& /*aSupportedChannels*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacGetSupportedSourceDataTypesL(RArray<TFourCC>& /*aSupportedDataTypes*/)
	{
	User::Leave(KErrNotSupported);
	}

void COggRecordController::MacGetSupportedSinkSampleRatesL(RArray<TUint>& aSupportedSampleRates)
	{
	LeaveIfNullL(iOggSink);
	aSupportedSampleRates.Reset();
	iOggSink->GetSupportedSampleRatesL(aSupportedSampleRates);
	}

void COggRecordController::MacGetSupportedSinkBitRatesL(RArray<TUint>& aSupportedBitRates)
	{
	LeaveIfNullL(iOggSink);
	aSupportedBitRates.Reset();
	iOggSink->GetSupportedBitRatesL(aSupportedBitRates);
	}

void COggRecordController::MacGetSupportedSinkNumChannelsL(RArray<TUint>& aSupportedChannels)
	{
	LeaveIfNullL(iOggSink);
	aSupportedChannels.Reset();
	iOggSink->GetSupportedNumChannelsL(aSupportedChannels);
	}

void COggRecordController::MacGetSupportedSinkDataTypesL(RArray<TFourCC>& aSupportedDataTypes)
	{
	LeaveIfNullL(iOggSink);
	aSupportedDataTypes.Reset();
	User::LeaveIfError(aSupportedDataTypes.Append(KMMFFourCCCodeVORB));
	}

COggRecordController::CMMFMessageHolder* COggRecordController::CMMFMessageHolder::NewL(TMMFMessage& aMessage)
	{
	return new(ELeave) CMMFMessageHolder(aMessage);
	}
		
void COggRecordController::CMMFMessageHolder::Complete(TInt aError)
	{
	iMessage.Complete(aError);
	}
	
COggRecordController::CMMFMessageHolder::CMMFMessageHolder(TMMFMessage& aMessage): CBase(), iMessage(aMessage)
	{
	}
	
COggRecordController::CAsyncEventHandler::CAsyncEventHandler(MDevSoundObserver& aParent):iParent(aParent)
	{
	}

COggRecordController::CAsyncEventHandler::~CAsyncEventHandler()
	{
	}

TInt COggRecordController::CAsyncEventHandler::SendEventToClient(const TMMFEvent& aEvent)
	{
	if (aEvent.iEventType == KMMFErrorCategoryControllerGeneralError)
		{
		iParent.RecordError(aEvent.iErrorCode);
		return KErrNone;
		}

	iParent.SendEventToClient(aEvent);
	return KErrNone;
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] =
	{
    IMPLEMENTATION_PROXY_ENTRY(KOggRecordControllerUid,	COggRecordController::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
