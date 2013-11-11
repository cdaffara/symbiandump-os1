// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "avirecordcontroller.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfvideoenums.h>
#endif

_LIT8(KXvidEncoderMimeType,"video/mp4v-es");
_LIT8(KXvidVideoCodec,"XVID");
const TInt KNumberOfBuffersToUse = 2;
const TInt KNumberOfFramesPerBuffer = 1;
const TInt KNumFrameRates = 3;
const TInt KNumFrameSizes = 2;


//This method generates a panic internal to this dll.
void CAviRecordController::Panic(TInt aPanicCode)
	{
	_LIT(KAviRecordControllerPanicCategory, "AviRecordController");
	User::Panic(KAviRecordControllerPanicCategory, aPanicCode);
	}
	

//This function creates an object of CAviRecordController.
CAviRecordController* CAviRecordController::NewL()
	{
	CAviRecordController* self = new(ELeave)CAviRecordController;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	

//This constructs the custom command parsers of CAviRecordController.
void CAviRecordController::ConstructL()
	{
	iState = EStopped;
	iAudioEnabled = EFalse;

	// Construct custom command parsers
	CMMFAudioRecordDeviceCustomCommandParser* audRecDevParser = CMMFAudioRecordDeviceCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audRecDevParser);
	AddCustomCommandParserL(*audRecDevParser);
	CleanupStack::Pop(audRecDevParser);

	CMMFVideoControllerCustomCommandParser* vidConParser = CMMFVideoControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidConParser);
	AddCustomCommandParserL(*vidConParser);
	CleanupStack::Pop(vidConParser);

	CMMFVideoRecordControllerCustomCommandParser* vidRecordConParser = CMMFVideoRecordControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidRecordConParser);
	AddCustomCommandParserL(*vidRecordConParser);
	CleanupStack::Pop(vidRecordConParser);

	CMMFVideoPixelAspectRatioCustomCommandParser* parParser = CMMFVideoPixelAspectRatioCustomCommandParser::NewL(*this);
	CleanupStack::PushL(parParser);
	AddCustomCommandParserL(*parParser);
	CleanupStack::Pop(parParser);

	CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser* asrccParser = CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::NewL(*this);
	CleanupStack::PushL(asrccParser);
	AddCustomCommandParserL(*asrccParser);
	CleanupStack::Pop(asrccParser);

	CMMFVideoRecordControllerExtCustomCommandParser* videoRecExtParser = CMMFVideoRecordControllerExtCustomCommandParser::NewL(*this);
	CleanupStack::PushL(videoRecExtParser);
	AddCustomCommandParserL(*videoRecExtParser);
	CleanupStack::Pop(videoRecExtParser);
	
	iEventHandler   = new(ELeave) CAsyncEventHandler(*this);
	iVideoEncoderInitialized = EFalse;
	//Create DevvideoRecord object
	iDevVideoRecord = CMMFDevVideoRecord::NewL(*this);	
	User::LeaveIfError(RFbsSession::Connect());
	User::LeaveIfError(iVideoFrameSizes.Append( TSize(176, 144) ));
	User::LeaveIfError(iVideoFrameSizes.Append( TSize(128, 96) ));  
	User::LeaveIfError(iVideoFrameRates.Append( 10.00 ));
	User::LeaveIfError(iVideoFrameRates.Append(  5.00 ));
	User::LeaveIfError(iVideoFrameRates.Append(  1.00 ));
	}



//Default Constructor of CAviRecordController
CAviRecordController::CAviRecordController()
	: iAspectRatio(1,1), iAudioChannels(2), iAudioSampleRate(22050)
	{
	}
	

//Destructor of CAviRecordController.
CAviRecordController::~CAviRecordController()
	{
	delete iAviWriter;
	delete iDevVideoRecord;
	delete iEventHandler;
	delete iVideoCodecName;
	delete iMessage;
	if (iCamera!=NULL)
		{
		iCamera->PowerOff();
		iCamera->Release();	
		}
	delete iCamera;
	iVideoFrameSizes.Close();
	iVideoFrameRates.Close();
	RFbsSession::Disconnect();
	}



//Adds a data source to the controller
void CAviRecordController::AddDataSourceL(MDataSource &aDataSource)
	{
	if (iState != EStopped) 
		{
		User::Leave(KErrNotReady);
		}

	if (aDataSource.DataSourceType() != KUidMmfAudioInput)
		{
		User::Leave(KErrNotSupported);
		}

	MMMFAudioInput* audioInput = static_cast<MMMFAudioInput*>(&aDataSource);
	User::LeaveIfError(audioInput->SourceThreadLogon(*iEventHandler));
	iDevSound = &(audioInput->SoundDevice());
	iDevSound->SetPrioritySettings(iPrioritySettings);
	}


//Adds a data sink to the controller
void CAviRecordController::AddDataSinkL(MDataSink &aDataSink)
	{
	if (iState!= EStopped) 
    	{
    	User::Leave(KErrNotReady);
    	}
    if (iClip || iAviWriter)
    	{
    	User::Leave(KErrAlreadyExists);
    	}
	if ((aDataSink.DataSinkType() == KUidMmfDescriptorSink)||(aDataSink.DataSinkType() == KUidMmfFileSink))
		{
	    iClip = static_cast<CMMFClip*>(&aDataSink);
	    iAviWriter = CAviWriter::NewL(*iClip,*this);
	    User::LeaveIfError(iClip->SinkThreadLogon(*iEventHandler));
	    }
	else 
		{
		User::Leave(KErrNotSupported);
		}
	}


//Removes the data source from the controller
void CAviRecordController::RemoveDataSourceL(MDataSource &aDataSource)
	{
	if ((!iDevSound) || (iState!= EStopped) || (!iDevVideoRecord))
    	{
    	User::Leave(KErrNotReady);
    	}
         	
    MMMFAudioInput* audioInput = static_cast<MMMFAudioInput*>(&aDataSource);
    CMMFDevSound& devSound = audioInput->SoundDevice();
    if (iDevSound != &devSound)
		{
		User::Leave(KErrArgument);
		}
	else
		{
		iDevSound = NULL;
		delete iDevVideoRecord;
    	iDevVideoRecord = NULL;
		}
	}
	

//Removes the data sink from the controller
void CAviRecordController::RemoveDataSinkL(MDataSink &aDataSink)
	{
	if ((!iClip) || (iState != EStopped))
		{
    	User::Leave(KErrNotReady);
    	}
    if (iClip != &aDataSink) 
    	{
    	User::Leave(KErrArgument);
    	}
    else
    	{
    	delete iAviWriter;
    	iAviWriter = NULL;
    	iClip = NULL;
    	}
	}


//Resets the controller
void CAviRecordController::ResetL()
	{
	StopL();
	iDevSound = NULL;
   	delete iDevVideoRecord;
   	iDevVideoRecord = NULL;
   	iClip = NULL;
   	delete iAviWriter; 
   	iAviWriter = NULL;
   	if (iCamera!=NULL)
		{
		iCamera->PowerOff();
		iCamera->Release();	
		} 
   	delete iCamera;
   	iCamera = NULL;
   	__ASSERT_ALWAYS((!iMessage),Panic(EBadCall));
   	}


//Primes the controller
void CAviRecordController::PrimeL(TMMFMessage &aMessage)
	{
	if (iState != EStopped ) 
		{
		User::Leave( KErrNotReady );	
		}
	if (!iClip)
		{
		User::Leave(KErrNotReady);
		}
	if(!iDevVideoRecord)
		{
    	iDevVideoRecord = CMMFDevVideoRecord::NewL(*this);
    	}
    if(iAudioEnabled)
    	{
    	if(!iDevSound)
    		{
    		User::Leave(KErrNotReady);
    		}
    	}
    __ASSERT_ALWAYS((!iMessage),Panic(EBadCall));
    iMessage = CMMFMessageHolder::NewL(aMessage);	
    StartPrimeL();
    }
    
void CAviRecordController::PrimeL()
    {
    Panic(EBadCall);
    }



//Primes the controller
void CAviRecordController::StartPrimeL()
    {
  	//open a file for writing.	
    iClip->SinkPrimeL();
    
    //Set the default values for file header.
    iAviWriter->SetDefaultHeaderParametersL();
           
 	if (iAudioEnabled)
		{
  		iDevSound->InitializeL(*this,EMMFStateRecording);
	  	}
	  	
	if(!iVideoEncoderInitialized)
		{
		// find the encoder
	  	RArray<TUid> foundEncoders;
		CleanupClosePushL(foundEncoders);
		TUint32 postProcType = 0;
		iDevVideoRecord->FindEncodersL(KXvidEncoderMimeType, postProcType, foundEncoders, EFalse);
			
		// otherwise select the first encoder which we can handle
		// Use iDevVideoRecord->VideoEncoderInfoLC() to get it
		
		TBool found = EFalse;
		TInt numEncoders = foundEncoders.Count();
		TInt thisEncoder = 0;
		while (!found && thisEncoder < numEncoders) 
			{
			TUid theEncoderUid = foundEncoders[thisEncoder];
			CVideoEncoderInfo* theEncoderInfo = iDevVideoRecord->VideoEncoderInfoLC(theEncoderUid);
		
			// input capabilities, if it can take bitmaps
			// then we can use it.
			
			iVideoFormat.iDataFormat = ERgbFbsBitmap;
			iVideoFormat.iRgbFormat  = EFbsBitmapColor16M;
			TBool isSupported = theEncoderInfo->SupportsInputFormat(iVideoFormat);
			if(isSupported) 
				{
				iEncoderDeviceId = iDevVideoRecord->SelectEncoderL(theEncoderUid);
				iDevVideoRecordInitialized = EFalse;
				found = ETrue;
				}
			CleanupStack::PopAndDestroy(theEncoderInfo);
			thisEncoder++;
			}
		CleanupStack::PopAndDestroy(&foundEncoders);//mediaIds	
			
		//Check if the valid Camera object is present.		
		CheckCameraPresentL();
		
		//Prepare the camera for video capture.Specify the videoframeformat,videosize index, video rate index, 
		//number of buffers to use and the number of frames per buffer.
		found = EFalse;
		TInt setFrameRateIndex = 0;
		TInt setFrameSizeIndex = 0;
	
		for(TInt frameRateIndex =0; frameRateIndex <KNumFrameRates; frameRateIndex++)
			{
			if (iFrameRate == iVideoFrameRates[frameRateIndex])
				{
				setFrameRateIndex = frameRateIndex;
				found = ETrue;
				}
			}
		if(found)
	    	{
	    	found = EFalse;
	        for(TInt frameSizeIndex =0; frameSizeIndex < KNumFrameSizes; frameSizeIndex++)
				{
				if(iFrameSize == iVideoFrameSizes[frameSizeIndex])
					{
					setFrameSizeIndex =  frameSizeIndex;
					found = ETrue;
					}
				}
	    	}
		if(!found)
			{
			SendErrorToClient(KErrNotSupported);
			return;
			}
   		
		iCamera->PrepareVideoCaptureL(CCamera::EFormatFbsBitmapColor16M,setFrameSizeIndex,setFrameRateIndex,KNumberOfBuffersToUse,KNumberOfFramesPerBuffer);
		//Set the video format and framesize for encoding
		iDevVideoRecord->SetInputFormatL(iEncoderDeviceId,iVideoFormat,iFrameSize);
			
		//Set the video frame rate for encoding
		iDevVideoRecord->SetSourceMemoryL(iFrameRate, ETrue, ETrue);
		
		//Encoder configured.Initialize  devvideorecord.
	  	iDevVideoRecord->Initialize();
		}
		else
			{
			if(iAudioEnabled)
				{
				iDevVideoRecordInitialized = ETrue;
				}
			else
				{
				iState = EPrimed;
				if (iMessage)
					{
					iMessage->Complete(KErrNone);
					delete iMessage;
					iMessage = NULL;
					}	
				}	
			}		
  	 	}
    

//Starts recording and transfers the data from data source to data sink.
void CAviRecordController::PlayL()
	{
	if(iState != EPrimed && iState != EPaused)
		{
		User::Leave(KErrNotReady);
		}
	CheckCameraPresentL();
	if(iState != EPaused)
		{
		iCamera->StartVideoCapture();
		}
	if(iAudioEnabled)
		{
		CheckDevSoundPresentL();
		iDevSound->RecordInitL();	
		}
	iState = ERecording;
	}


void CAviRecordController::PauseL(TMMFMessage& aMessage)
	{
	if (iState != ERecording)
    	{
    	User::Leave(KErrNotReady);
    	}
	ASSERT(!iMessage);
	iMessage = CMMFMessageHolder::NewL(aMessage);
	PauseL();
	}

//Pauses recording and the data transfer from data source to data sink.
void CAviRecordController::PauseL()
	{
	if(iAudioEnabled)
		{
		iState = EPausing;
		iDevSound->Pause();
		}
	else
		{
		iState = EPaused;
		iAviWriter->UpdateHeadersL();
		iMessage->Complete(KErrNone);	
		delete iMessage;
		iMessage = NULL;
		}
	}
	

//Stops recording and the data transfer from data source to data sink.
//This method is overridden inorder to make the Pause implementation asynchronous.
//As the Pause involves calling DevSound Pause()and waiting for the NULL length 
//buffer, it needs to be asynchronous.
void CAviRecordController::StopL(TMMFMessage &aMessage)
	{
	if ((iState == ERecording) && !iMessage)
		{
		iMessage = CMMFMessageHolder::NewL(aMessage);
		if (iAudioEnabled)
			{
			//Stop the devsound.
			iDevSound->Pause();	
			}
		//Stop video capture from Camera.	
		iCamera->StopVideoCapture();
		//Inform DevVideoRecord about end of stream.
		iDevVideoRecord->InputEnd();
		}
	else
		{
		StopL();
		aMessage.Complete(KErrNone);
		}
	}


//Stops recording and the data transfer from data source to data sink.
void CAviRecordController::StopL()
    {
    if (iState == EStopped)
    	{
    	return;	
    	}
    if(iAudioEnabled)
    	{
    	iDevSound->Stop();	
		iDevSoundStop = ETrue;
    	}
    iAviWriter->UpdateHeadersL();
    iClip->SinkStopL();
    iAviWriter->Reset();
    iState=EStopped; 
    iDevVideoRecordInitialized = EFalse;
	iDevSoundInitialized = EFalse;	
	iCamera->StopVideoCapture();
	iVideoInputEnd = ETrue;
	}
	

//Returns the current recording position from devvideorecord.
TTimeIntervalMicroSeconds CAviRecordController::PositionL() const
	{
	if (iState == EStopped)
		{
		User::Leave(KErrNotReady);
		}
	CheckDevVideoPresentL();
	return iDevVideoRecord->RecordingPosition();
	}


//Set the position to record from.
void CAviRecordController::SetPositionL(const TTimeIntervalMicroSeconds &aPosition)
	{
	if (iState != EPrimed) 
		{
		User::Leave(KErrNotReady);
		}
	else
		{
		CheckAviWriterPresentL();
		iAviWriter->SetPositionL(aPosition);	
		}
	}
	

//Returns the duration of the clip.
TTimeIntervalMicroSeconds CAviRecordController::DurationL() const
	{
	CheckAviWriterPresentL();
	//returns the duration from the clip.
	return iAviWriter->Duration();
	}


//Handles a custom command.
void CAviRecordController::CustomCommand(TMMFMessage &aMessage)
	{
	aMessage.Complete(KErrNotSupported);
	}
	

//Sets the priority settings for devsound.
void CAviRecordController::SetPrioritySettings(const TMMFPrioritySettings &aPrioritySettings)
	{
	iPrioritySettings = aPrioritySettings;
	if (iDevSound) 
		{
		iDevSound->SetPrioritySettings(aPrioritySettings);
		}
	 }


//Gets the number of meta entries in the clip.
void CAviRecordController::GetNumberOfMetaDataEntriesL(TInt &/*aNumberOfEntries*/)
	{
	//Support for addition and retrieval of meta data is not present.so it will leave with KErrNotSuuported.
	User::Leave(KErrNotSupported);
	}


//Gets a meta entry at a specified index.
CMMFMetaDataEntry* CAviRecordController::GetMetaDataEntryL(TInt /*aIndex*/)
	{ 
	//Support for addition and retrieval of meta data is not present.so it will leave with KErrNotSuuported.
	User::Leave(KErrNotSupported);
	return NULL;
	}
	
// MMMFVideoControllerCustomCommandImplementor
 

//Returns the video frame size set for recording.
void  CAviRecordController::MvcGetVideoFrameSizeL(TSize &aVideoFrameSize)
	{
	aVideoFrameSize = iFrameSize;
	}


//Gets the audio codec used for recording.
void CAviRecordController::MvcGetAudioCodecL(TFourCC &aCodec)
	{
    aCodec = iAudioCodecName;
	}
	

//Gets the video bit rate used for recording.
void CAviRecordController::MvcGetVideoBitRateL(TInt &aBitRate)
	{
	aBitRate = iVideoBitRate;
	}
     

//Gets the audio bit rate used for recording.
void CAviRecordController::MvcGetAudioBitRateL(TInt &aBitRate)
	{
	aBitRate = iAviWriter->GetAudioBitRate();
	}
     

//Sets the video frame rate for recording.
void CAviRecordController::MvcSetFrameRateL(TReal32 aFramesPerSecond)
	{
	iFrameRate = aFramesPerSecond;
	CheckAviWriterPresentL();
	
	
	//Sets the frame rate in the file.
	iAviWriter->SetVideoFrameRate(iFrameRate);
	}
  

//Gets the video frame rate used for recording.
void CAviRecordController::MvcGetFrameRateL(TReal32 &aFramesPerSecond)
	{
	aFramesPerSecond = iFrameRate;
	}
     

//Gets the video codec used for recording.
void CAviRecordController:: MvcGetVideoMimeTypeL(TDes8 &aMimeType)
	{
	if (iVideoCodecName)
		{
		TPtr8 mimeType = iVideoCodecName->Des();
		aMimeType.Copy(mimeType);
		}
	}

//MMMFVideoRecordControllerCustomCommandImplementor


//Sets the video format for recording.
void CAviRecordController::MvrcSetVideoFormatL(TUid aVideoFormatUid)
	{
	if(aVideoFormatUid != TUid::Uid(KAviRecordFormatUid))
		{
 		User::Leave(KErrNotSupported);	
 		}
	iVideoFormatUid = aVideoFormatUid;
	}
	  

//Sets the video codec for recording.
void CAviRecordController::MvrcSetVideoCodecL(const TDesC8 &aVideoCodec)
	{
	if (iState == ERecording )
		{
		User::Leave(KErrNotReady);
		}
	delete iVideoCodecName;
	iVideoCodecName= NULL;
	iVideoCodecName = aVideoCodec.AllocL();
	}
	 

//Sets the audio codec for recording.
void CAviRecordController::MvrcSetAudioCodecL(TFourCC aAudioCodec)
	{
    if (iState == ERecording )
 		{
		User::Leave(KErrNotReady);
		}
	iAudioCodecName = aAudioCodec;
	}
  

//Sets the video bitrate for recording.
void CAviRecordController::MvrcSetVideoBitRateL(TInt& aBitRate)
	{
	if(iState != EStopped)
		{
		User::Leave(KErrNotReady);
		}
	iVideoBitRate = aBitRate;
	}



//Sets the audio bitrate for recording.
void CAviRecordController::MvrcSetAudioBitRateL(TInt& aBitRate)
	{
	if(iState != EStopped)
		{
		User::Leave(KErrNotReady);
		}
	if(iAudioEnabled)
		{
		CheckAviWriterPresentL();
		iAviWriter->SetAudioBitRateL(aBitRate);
		}
	}
     

//Adds a meta data entry to the clip
void CAviRecordController::MvrcAddMetaDataEntryL(const CMMFMetaDataEntry &/*aNewEntry*/)
	{
	//Support for addition and retrieval of
	//meta data is not present.so it will leave with KErrNotSuuported.
	User::Leave(KErrNotSupported);	
	}
     
//Removes a meta data entry from the clip
void CAviRecordController::MvrcRemoveMetaDataEntryL(TInt /*aIndex*/)
	{
	//Support for addition and retrieval of
	//meta data is not present.so it will leave with KErrNotSuuported.
	User::Leave(KErrNotSupported);	
	}
     

//Replaces a meta data entry from the clip
void CAviRecordController::MvrcReplaceMetaDataEntryL(TInt /*aIndex*/, const CMMFMetaDataEntry &/*aNewEntry*/)
	{
	//Support for addition and retrieval of
	//meta data is not present.so it will leave with KErrNotSuuported.
	User::Leave(KErrNotSupported);
	}
     

//Sets the maximum file size.
void CAviRecordController::MvrcSetMaxFileSizeL(TInt aMaxFileSize)
	{
	if (aMaxFileSize < 0 )
		{
		User::Leave( KErrArgument );
		}
	CheckAviWriterPresentL();	
	iAviWriter->SetMaximumClipSizeL(aMaxFileSize);
	}
	

//Enables the audio for recording.
void CAviRecordController::MvrcSetAudioEnabledL(TBool aEnable)
	{
	iAudioEnabled = aEnable;
	CheckAviWriterPresentL();		
	iAviWriter->SetAudioEnabledL(iAudioEnabled);
	}
     

//Sets the video frame size for recording.
void CAviRecordController::MvrcSetVideoFrameSizeL(TSize aFrameSize)
	{
	iFrameSize = aFrameSize; 
	CheckAviWriterPresentL();
	iAviWriter->SetVideoFrameSize(aFrameSize);
	}
  

//Prepares the controller for recording.
void CAviRecordController::MvrcPrepareL()
	{
	CheckCameraPresentL();
	iCamera->Reserve();
	}
     

//Creates a duplicate camera object using the camerahandle
void CAviRecordController::MvrcSetCameraHandleL(TInt aCameraHandle)
	{
	if (!iCamera)
		{
		iCamera = CCamera::NewDuplicateL(*this, aCameraHandle);
		iCameraHandle = aCameraHandle;
		}
	else
		{
		// this function is only meant to be called once per CVideoRecorderUtlity::OpenFile / Close pair, so panic on subsequent calls
		Panic(EBadCall);
		}
	
	//Send KMMFEventCategoryVideoOpenComplete to client	
	TMMFEvent controllerEvent;
	controllerEvent.iEventType = KMMFEventCategoryVideoOpenComplete;
	controllerEvent.iErrorCode = KErrNone;
	DoSendEventToClient(controllerEvent);	
	}
     

//Gets the remaining time available for recording.
void CAviRecordController::MvrcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds &aTime)
	{
	CheckAviWriterPresentL();
    iAviWriter->GetRecordTimeAvailableL(aTime);
	}
     

//Gets the supported audio codec types for recording.
void  CAviRecordController::MvrcGetSupportedSinkAudioTypesL(RArray <TFourCC> &aDataTypes)
	{
	aDataTypes.Reset();
	aDataTypes.AppendL(KMMFFourCCCodePCM16); 	
	}
     

//Gets the supported video codec types for recording.

void  CAviRecordController::MvrcGetSupportedSinkVideoTypesL(CDesC8Array &aDataTypes)
	{
	aDataTypes.Reset();
	aDataTypes.AppendL(KXvidVideoCodec);
	}
	

//Gets if audio is enabled during recording.
void CAviRecordController::MvrcGetAudioEnabledL(TBool &aEnabled)
	{
	aEnabled = iAudioEnabled;
	}

void CAviRecordController::MvparSetPixelAspectRatioL(const TVideoAspectRatio& aAspectRatio)
	{
	if(iState != EStopped)
		{
		User::Leave(KErrNotReady);
		}
	if(aAspectRatio.iNumerator == 1 && aAspectRatio.iDenominator == 1) // as 1:1 square pixel aspect ratio is the only one supported for video recording
		{
		iAspectRatio = aAspectRatio;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}
	
void CAviRecordController::MvparGetPixelAspectRatioL(TVideoAspectRatio& aAspectRatio)
	{
	aAspectRatio = iAspectRatio;
	}

void CAviRecordController::MvparGetSupportedPixelAspectRatiosL(RArray<TVideoAspectRatio>& aAspectRatios)
	{
	aAspectRatios.Reset();
	aAspectRatios.AppendL(TVideoAspectRatio(1,1)); // This controller currently does not use PAR, therefore 1:1 is the implied default
	}

void CAviRecordController::MvasrccGetAudioChannelsL(TUint& aNumChannels)
	{
	aNumChannels = iAudioChannels;
	}
	
void CAviRecordController::MvasrccSetAudioChannelsL(const TUint aNumChannels)
	{
	if(iState != EStopped)
		{
		User::Leave(KErrNotReady);
		}
	if(aNumChannels == 2) // as 2 stereo is the only supported audio channel to record.
		{
		iAudioChannels = aNumChannels;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}

void CAviRecordController::MvasrccGetSupportedAudioChannelsL(RArray<TUint>& aChannels)
	{
	aChannels.Reset();
	aChannels.AppendL(2); // 2 used as stereo is the only audio channel supported.
	}

void CAviRecordController::MvasrccGetAudioSampleRateL(TUint& aSampleRate)
	{
	aSampleRate = iAudioSampleRate;
	}
	
void CAviRecordController::MvasrccSetAudioSampleRateL(const TUint aSampleRate)
	{
	if(iState != EStopped)
		{
		User::Leave(KErrNotReady);
		}
	if(aSampleRate == 22050) // as 22050Hz is the only supported sample rate.
		{
		iAudioSampleRate = aSampleRate;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}		
	}

void CAviRecordController::MvasrccGetSupportedAudioSampleRatesL(RArray<TUint> &aSampleRates)
	{
	aSampleRates.Reset();
	aSampleRates.AppendL(22050); // 22050Hz used as it is the only supported sample rate.
	}     

// MMMFAudioRecordDeviceCustomCommandImplementor
//Sets the gain for recording.
void  CAviRecordController::MardSetGainL(TInt aGain)
	{
	if (iAudioEnabled)
		{
		ASSERT(iDevSound);
		TInt maxGain = iDevSound->MaxGain();
		iDevSound->SetGain(aGain);				
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}



//Gets the maximum audio gain for recording.
void CAviRecordController::MardGetMaxGainL(TInt& aMaxGain)
	{
	if(iAudioEnabled)
		{
		ASSERT(iDevSound);
		aMaxGain = iDevSound->MaxGain();
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}


//Gets the gain set for recording.
void CAviRecordController::MardGetGainL(TInt& aGain)
	{
	if(iAudioEnabled)
		{
		ASSERT(iDevSound);
		aGain = iDevSound->Gain();
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}
	

//Set the audio balance for recording.
void CAviRecordController::MardSetBalanceL(TInt aBalance)
	{
	if (iAudioEnabled)
		{
		ASSERT(iDevSound);
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
		iDevSound->SetRecordBalanceL(left, right);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}
	

//Get the audio balance for recording.
void CAviRecordController::MardGetBalanceL(TInt& aBalance)
	{
	if (iAudioEnabled)
		{
		ASSERT(iDevSound);
		TInt left = 50; 
		TInt right = 50;
		iDevSound->GetRecordBalanceL(left, right);
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
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}
	
     
//MMFDevVideoRecordObserver


//Intended for future use.Will panic if called.
void CAviRecordController::MdvroReturnPicture(TVideoPicture */*aPicture*/)
	{
 	
 	}
	 
	 

//Intended for future use.Will panic if called.
void CAviRecordController::MdvroSupplementalInfoSent()
	{
	Panic(EBadCall);
	}
     

//Get the encoded video buffer from devvideorecord and write it to a file.
void CAviRecordController::MdvroNewBuffers()
	{
	ASSERT(iDevVideoRecord);
	
	TVideoOutputBuffer* buf = NULL;
	//Get the encoded buffer from devvideorecord
	TRAPD(error,buf = iDevVideoRecord->NextBufferL());
	if (error != KErrNone)
		{
		SendErrorToClient(error);
		return;
		}
		
	if (!buf->iData.Length())
		{
		//end of recording.Just update the file headers.
		iDevVideoRecord->InputEnd();
		return;
		}
		
	ASSERT(iAviWriter);
	//Write it to a file
	TRAPD(err,iAviWriter->WriteVideoDataL(buf));	
	  
	if (err)
		{
		if(err == KErrEof)//maxsize shutdown
			{
			iDevVideoRecord->InputEnd();
			return;	
			}
		else
			{
			SendErrorToClient(err);
			return;
			}	
		}
		
	//return the buffer to encoder for disposal	
	iDevVideoRecord->ReturnBuffer(buf);
    }
  


//Fatal error occured while encoding video.Send the error to client.
void CAviRecordController::MdvroFatalError(TInt aError)
	{
	TRAP_IGNORE(StopL());
	SendErrorToClient(aError);	
	}
   


//Initialization complete for devvideorecord.If audio is not enabled
//set state to EPrimed.
void CAviRecordController::MdvroInitializeComplete(TInt aError)
	{
     if (aError != KErrNone)
		{
		SendErrorToClient(aError);
		return;
		}
	iVideoInputEnd = EFalse;
	iDevVideoRecordInitialized = ETrue;
	// if audio is enabled we need to wait for both the audio and video initializations
	//before completing client's prime message.
	CheckForInitComplete();	
	}
    

//End of the encoded video stream.
//Update the .avi file headers and close the file.If audio, is enabled
//wait for audio to stop.
void CAviRecordController::MdvroStreamEnd()
     {
     iVideoInputEnd = ETrue;
     if (iAudioEnabled)
	     {
  	     //if audio is enabled, we need to wait for audio completion also before completing
	     //the client's stop message
	     CheckForStopComplete();	
	     }
   
     else
	     {
	     ASSERT(iAviWriter);
   	     //audio is not enabled. so we complete the stop message from here itself
	     TRAPD(error,iAviWriter->UpdateHeadersL());
	     SendErrorToClient(error);	
	     }
	 }
    	
	
//MDevSoundObserver


//Devsound initialization is completed.Configure devsound capabilities. 
void  CAviRecordController::InitializeComplete(TInt aError)
	{
	TInt error = aError;
	if (error == KErrNone)
		{
		ASSERT(iDevSound);
		TMMFCapabilities devSoundCaps = iDevSound->Capabilities();
		TMMFCapabilities caps;
		
		//there is no video utlity class support to set samplerate, channels on devsound.
		//setting CD quality audio configuration
		caps.iRate = EMMFSampleRate22050Hz;
		caps.iChannels = EMMFStereo;
		caps.iEncoding = EMMFSoundEncoding16BitPCM;
		
		TRAP(error,iDevSound->SetConfigL(caps));
		if (error == KErrNone)
			{
			iDevSoundInitialized = ETrue;
			//Check if state can be set to EPrimed.
			CheckForInitComplete();	
			}
		}
	if (error)
		{
		SendErrorToClient(error);
		}
	}
	

//Intended for future use.Will panic if called.
void CAviRecordController::ToneFinished(TInt /*aError*/)
	{
	Panic(EBadCall);
	}
	

//Intended for future use.Will panic if called.
void CAviRecordController::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	Panic(EBadCall);
	}
	

//Intended for future use.Will panic if called.
void CAviRecordController::PlayError(TInt /*aError*/)
	{
	Panic(EBadCall);
	}
	


//Encoded audio buffer avaialble.If the buffer is empty continue recording.
//Else write the encoded audio data to a file.
void CAviRecordController::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	TInt err = KErrNone;
	
	CMMFDataBuffer* buf = static_cast<CMMFDataBuffer*>(aBuffer);
	
	if ((iMessage) && (!buf->Data().Length())&&(buf->LastBuffer()))
		{
		//we are ready to stop the recording
		iDevSoundStop = ETrue;
		if(iState == EPausing && iAudioEnabled)
			{
			//initialize the devsound once recording is resumed after pause.
			iDevSound->Stop();
			TRAPD(err,iAviWriter->UpdateHeadersL());
			if(err == KErrNone)
				{
				TRAP(err, iDevSound->InitializeL(*this,EMMFStateRecording));
				}			
			if(err != KErrNone)
				{
				SendErrorToClient(err);	
				}
			return;
			}
		CheckForStopComplete();
		return;
		}
	
	//the buffer is empty.So continue, recording
	
	if (!buf->Data().Length())
		{
		ASSERT(iDevSound);
		iDevSound->RecordData();
		return;
		}
	
	ASSERT(iAviWriter);
	
	//Write audio data to a file.	
	TRAP(err, iAviWriter->WriteAudioDataL(aBuffer));
	
	//if error, stop devsound and send the error to client
	if (err)
		{
		if(err == KErrEof)//maxsize shutdown
			{
			err = KErrNone;
			}
		TRAP_IGNORE(StopL());
		TMMFEvent controllerEvent;
		controllerEvent.iEventType = KMMFEventCategoryPlaybackComplete;
		controllerEvent.iErrorCode = err; 
		DoSendEventToClient(controllerEvent);
		}
	}
	

void CAviRecordController::RecordError(TInt aError)
	{
	SendErrorToClient(aError);
	}
	

//Will panic if called.Should not be called while recording
void CAviRecordController::ConvertError(TInt /*aError*/)
	{
	Panic(EBadCall);
	}
	

//Will panic if called.Should not be called while recording
void CAviRecordController::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	Panic(EBadCall);
	}
	


//Encoded audio data is written to a file.Recording can continue.
void CAviRecordController::BufferEmptied()
	{
	if (!iAudioEnabled)
		{
		return;			
		}
		
	if (ibAudioWritten)
		{
		//continue recording.
		ASSERT(iDevSound);
		iDevSound->RecordData();
		ibAudioWritten = EFalse;	
		}	
	}


void CAviRecordController::AudioWritten(TBool abVal)
	{
	ibAudioWritten = abVal;	
	}
	
	
//MCameraObserver2


//Handles the camera event.
void CAviRecordController::HandleEvent(const TECAMEvent &aEvent)
	{
	//powers on the camera following KUidECamEventReserveComplete.
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		iCamera->PowerOn();
		}
	if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		//sends the prepare complete event to client following KUidECamEventPowerOnComplete.
		TMMFEvent controllerEvent;
		controllerEvent.iEventType = KMMFEventCategoryVideoPrepareComplete;
		controllerEvent.iErrorCode = KErrNone;
		DoSendEventToClient(controllerEvent);
		}
	}
	
	
void CAviRecordController::ViewFinderReady(MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
	{	
	}
	
	
void CAviRecordController::ImageBufferReady(MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
	{
	}
	


//Retrieves the video buffer from CCamera and sends it for encoding to DevVideoRecord.
void CAviRecordController::VideoBufferReady(MCameraBuffer &aCameraBuffer, TInt aError)	
	{
	if (aError != KErrNone)
		{
		SendErrorToClient(aError);
		return;
		}
    if(iState == EPaused)
    	{
    	//ignore the frames coming from the camera, as the current state is
    	//EPaused.
    	aCameraBuffer.Release();
    	}
    else 
    	{
    	// send frame to Mpeg4 encoder
		// We need a pointer to the data in the buffer itself.
		// NB as the camera simulator hasn't defined handle to Chunks we
		// have to get the pointer manually
		// Size of bitmap = width * height * bytes per pixel
		// (it's EFbsBitmapColor16M so it's 3)
		CFbsBitmap* theBitmap = NULL; 
		TRAPD(error,theBitmap = &(aCameraBuffer.BitmapL(0)));
		theBitmap->LockHeap();
		TUint32* dataAddress = theBitmap->DataAddress();
		TSize theSize = theBitmap->SizeInPixels();
		TInt frameSize = theSize.iHeight * theSize.iWidth * 3;
		TVideoPicture theVideoPicture;
		TPictureData& thePictureData = theVideoPicture.iData;
		TPtr8 theRawData((TUint8*)dataAddress, frameSize);
		thePictureData.iRawData = &theRawData;
		thePictureData.iDataSize = theSize;
		//Send the video picture for encoding.
		TRAP(error,iDevVideoRecord->WritePictureL(&theVideoPicture));
		if (error!= KErrNone)
			{
			SendErrorToClient(error);
			}
		theBitmap->UnlockHeap(ETrue);
		aCameraBuffer.Release();	
    	}
	}	
	
	
//CAviRecordController functions


//Checks if valid devsound object is present.
void CAviRecordController::CheckDevSoundPresentL() const
	{
	if (!iDevSound) 
		{
		User::Leave(KErrNotReady);
		}
	}


//Checks if valid devvideorecord object present
void CAviRecordController::CheckDevVideoPresentL() const
	{
	if (!iDevVideoRecord) 
		{
		User::Leave(KErrNotReady);
		}
	}	


//Checks if valid aviwriter object present.
void CAviRecordController::CheckAviWriterPresentL() const
	{
	if (!iAviWriter)
		{
		User::Leave(KErrNotReady);
		}
	}

//Checks if valid camera object present.
void CAviRecordController::CheckCameraPresentL() const
	{
	if (!iCamera)
		{
		User::Leave(KErrNotReady);
		}
	}
	
	


//Sends the error message to the client.
void CAviRecordController::SendErrorToClient(TInt aError)
	{
	if (iState != EStopped)
		{
		TRAP_IGNORE(StopL());
		}
	if (iMessage)
		{
		iMessage->Complete(aError);
		delete iMessage;
		iMessage = NULL;
		}
	else
		{
		TMMFEvent controllerEvent;
		controllerEvent.iEventType = KMMFEventCategoryPlaybackComplete;
		controllerEvent.iErrorCode = aError;
		DoSendEventToClient(controllerEvent);	
		}
	}
	


//This function is used when both audio and video are enabled for recording.
//The State is set to EPrimed when initialization is completed on both DevSound and DevVideo 
void CAviRecordController::CheckForInitComplete()
 	{
 	if(iAudioEnabled)
 		{
 		if ((iDevVideoRecordInitialized) && (iDevSoundInitialized))
 			{
 			if(iState == EPausing)
 				{
				iState = EPaused;
				if(iMessage)
					{
					iMessage->Complete(KErrNone);
					delete iMessage;
					iMessage = NULL;	
					}
				}
			else
				{
				iState = EPrimed;
 				if (iMessage)
					{
					iMessage->Complete(KErrNone);
					delete iMessage;
					iMessage = NULL;
					}	
				}
 			}
 		}
	else
		{
		iState = EPrimed;
		if (iMessage)
			{
			iMessage->Complete(KErrNone);
			delete iMessage;
			iMessage = NULL;
			}	
		}
	iVideoEncoderInitialized = ETrue;
	}


//This function is used when both audio and video are enabled for recording.
//This calls UpdateHeadersL() in AviWriter when both devsound and devvideo are stopped.
void CAviRecordController::CheckForStopComplete()
 	{
 	if ((iVideoInputEnd) && (iDevSoundStop))
	 	{
	 	ASSERT(iAviWriter);
	 	TRAPD(error,iAviWriter->UpdateHeadersL());
		SendErrorToClient(error);
		}
 	}

CAviRecordController::CMMFMessageHolder* CAviRecordController::CMMFMessageHolder::NewL(TMMFMessage& aMessage)
	{
	return new(ELeave) CMMFMessageHolder(aMessage);
	}

	
void CAviRecordController::CMMFMessageHolder :: Complete(TInt aError)
	{
	iMessage.Complete(aError);
	}

	
CAviRecordController::CMMFMessageHolder::CMMFMessageHolder(TMMFMessage& aMessage): CBase(), iMessage(aMessage)
	{
	}

	
CAviRecordController::CAsyncEventHandler::CAsyncEventHandler(CAviRecordController& aParent):iParent(aParent)
	{
	}


CAviRecordController::CAsyncEventHandler::~CAsyncEventHandler()
	{
	}


TInt CAviRecordController::CAsyncEventHandler::SendEventToClient(const TMMFEvent& aEvent)
	{
	iParent.SendEventToClient(aEvent);
	return KErrNone;
	}
   

/*
MMMFVideoRecordControllerExtCustomCommandImplementor
*/

// Enables or disables video.
void CAviRecordController::MvrecSetVideoEnabledL(TBool )
	{
	User::Leave(KErrNotSupported);
	}

//returns video enable status
TBool CAviRecordController::MvrecVideoEnabledL() const
	{
	return ETrue;
	}

//sets quality of the video being recorded.
void CAviRecordController::MvrecSetVideoQualityL(TInt )
	{
	User::Leave(KErrNotSupported);
	}

// returns video quality.
TInt CAviRecordController::MvrecVideoQualityL() const
	{
	return EVideoQualityNormal;
	}

// enables or disables, Set frame rate fixed.
void CAviRecordController::MvrecSetVideoFrameRateFixedL(TBool )
	{
	User::Leave(KErrNotSupported);
	}

// returns fixed frame rate flag status.
TBool CAviRecordController::MvrecVideoFrameRateFixedL() const
	{
	return EFalse;
	}
