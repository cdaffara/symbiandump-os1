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



#include "aviplaycontroller.h"

const TInt KTestBufferSize = 0x10000; 
_LIT8(KXvidDecoderMimeType,"video/mp4v-es");
_LIT8(KAviVideoCodec,"XVID");


//Table that maps given samplerate with the MMF samplerate
const TSampleRateTable KRateLookup[]=
								 {
									{96000,EMMFSampleRate96000Hz},
        							{88200,EMMFSampleRate88200Hz},
                   					{48000,EMMFSampleRate48000Hz},
			                   		{44100,EMMFSampleRate44100Hz},
				                  	{32000,EMMFSampleRate32000Hz},
				                  	{22050,EMMFSampleRate22050Hz},
				                  	{16000,EMMFSampleRate16000Hz},
				                  	{11025,EMMFSampleRate11025Hz},
				                  	{8000, EMMFSampleRate8000Hz}
                   				 };


//This method generates a panic internal to this dll.
void CAviPlayController::Panic(TInt aPanicCode)
	{
	_LIT(KAviPlayControllerPanicCategory, "AviPlayController");
	User::Panic(KAviPlayControllerPanicCategory, aPanicCode);
	}

	
//This function creates an object of CAviPlayController.
CAviPlayController* CAviPlayController::NewL()
	{
    CAviPlayController* self = new(ELeave)CAviPlayController();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


//Default Constructor of CAviPlayController
CAviPlayController::CAviPlayController()
	{
	
	}


//Destructor of CAviPlayController.
CAviPlayController::~CAviPlayController()
	{
	iDisplayRegion.Close();
	iDerivedClipRegion.Close();
	delete iAviReader;
	delete iDevVideoPlay;
	delete iMessage;
	delete iEventHandler;
	delete iScreenGc;
	delete iScreenDev;
	RFbsSession::Disconnect();
	}

//This constructs the custom command parsers of CAviPlayController.
void CAviPlayController::ConstructL()
	{
	CMMFVideoPlayControllerCustomCommandParser* vidPlayConParser = CMMFVideoPlayControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidPlayConParser);
	AddCustomCommandParserL(*vidPlayConParser);
	CleanupStack::Pop(vidPlayConParser);
	
	CMMFVideoControllerCustomCommandParser* vidConParser = CMMFVideoControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidConParser);
	AddCustomCommandParserL(*vidConParser);
	CleanupStack::Pop(vidConParser);

	CMMFAudioPlayDeviceCustomCommandParser* audPlayDevParser = CMMFAudioPlayDeviceCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayDevParser);
	AddCustomCommandParserL(*audPlayDevParser);
	CleanupStack::Pop(audPlayDevParser);

	CMMFResourceNotificationCustomCommandParser* notiParser = CMMFResourceNotificationCustomCommandParser::NewL(*this);
	CleanupStack::PushL(notiParser);
	AddCustomCommandParserL(*notiParser);
	CleanupStack::Pop(notiParser);//audio resource Notification Parser
	
	CMMFVideoSetInitScreenCustomCommandParser* vidScrDevParser = CMMFVideoSetInitScreenCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidScrDevParser);
	AddCustomCommandParserL(*vidScrDevParser);
	CleanupStack::Pop(vidScrDevParser);	
	
	iEventHandler = new(ELeave) CSourceSinkEventHandler(*this);
	iAudioEnabled = EFalse;
	User::LeaveIfError(RFbsSession::Connect());
	}


//Adds a data source to the controller
void CAviPlayController::AddDataSourceL(MDataSource& aDataSource)
	{
	if (iState != EStopped) 
		{
		User::Leave(KErrNotReady);
		}
	if (iClip)
		{
		User::Leave(KErrAlreadyExists);
		}
	if (aDataSource.DataSourceType()==KUidMmfFileSource)
		{
		iClip = static_cast<CMMFClip*>(&aDataSource);
		iAviReader = CAviReader::NewL(*iClip,*this);
		User::LeaveIfError(iClip->SourceThreadLogon(*iEventHandler));
		iAviReader->AudioEnabled(iAudioEnabled);
		}
	else 
		{
		User::Leave(KErrNotSupported);
		}
	TMMFEvent controllerEvent;
	controllerEvent.iEventType = KMMFEventCategoryVideoOpenComplete;
	controllerEvent.iErrorCode = KErrNone;
	DoSendEventToClient(controllerEvent);		
	}

 
//Adds a data sink to the controller
void CAviPlayController::AddDataSinkL(MDataSink& aDataSink)
	{
	if (iState != EStopped) 
    	{
    	User::Leave(KErrNotReady);	
    	}
    if(iDevSound)
    	{
    	User::Leave(KErrAlreadyExists);
    	}
    if (aDataSink.DataSinkType()!=KUidMmfAudioOutput) 
    	{
    	User::Leave(KErrNotSupported);	
    	}
	MMMFAudioOutput* audioOutput = static_cast<MMMFAudioOutput*>(&aDataSink);
    User::LeaveIfError(audioOutput->SinkThreadLogon(*iEventHandler));
    iDevSound = &(audioOutput->SoundDevice());
	iDevSound->SetPrioritySettings(iPrioritySettings);
	delete iScreenDev;
	iScreenDev = NULL;
	TRAPD(err, iScreenDev = CFbsScreenDevice::NewL(iScreenNumber,EColor16MA));
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDev = CFbsScreenDevice::NewL(iScreenNumber,EColor16M));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDev = CFbsScreenDevice::NewL(iScreenNumber,EColor64K));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDev = CFbsScreenDevice::NewL(iScreenNumber,EColor4K));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDev = CFbsScreenDevice::NewL(iScreenNumber,EColor256));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDev = CFbsScreenDevice::NewL(iScreenNumber,EColor16MAP));
		}		
		
	User::LeaveIfError(err);
	User::LeaveIfError(iScreenDev->CreateContext(iScreenGc));
	iScreenGc->SetPenColor(KRgbBlack);
	iScreenGc->SetBrushColor(KRgbWhite);
	iScreenGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	delete iDevVideoPlay;
	iDevVideoPlay = NULL;
	iDevVideoPlay = CMMFDevVideoPlay::NewL(*this);
	iVideoDecoderInitialized = EFalse;
	}


//Removes the data source from the controller
void CAviPlayController::RemoveDataSourceL(MDataSource& aDataSource)
	{
	if (iState != EStopped) 
    	{
    	User::Leave(KErrNotReady);	
    	}
   	if (!iClip)
    	{
    	User::Leave(KErrNotReady);
    	}
    if (iClip != &aDataSource) 
	 	{
    	User::Leave(KErrArgument);
    	}
    else
    	{
    	delete iAviReader;
    	iAviReader = NULL;
    	iClip = NULL;
   		}
	}


//Removes the data sink from the controller
void CAviPlayController::RemoveDataSinkL(MDataSink& aDataSink)
	{
	if ((!iDevSound) || (iState!= EStopped))
		{
    	User::Leave(KErrNotReady);
    	}
	if (aDataSink.DataSinkType() !=	KUidMmfAudioOutput) 
		{
	   	User::Leave(KErrNotSupported);	
	   	}
	MMMFAudioOutput* audioOutput = static_cast<MMMFAudioOutput*>(&aDataSink);
	CMMFDevSound& devSound = audioOutput->SoundDevice();
	if (iDevSound != &devSound) 
		{
		User::Leave(KErrArgument);
		}
	else
		{
		iDevSound = NULL;
		}
	delete iDevVideoPlay;
   	iDevVideoPlay = NULL;
	delete iScreenDev;
	iScreenDev = NULL;
	}


//Resets the controller
void CAviPlayController::ResetL()
	{
	StopL();
	iDevSound = NULL;
	delete iDevVideoPlay;
   	iDevVideoPlay = NULL;
    iClip=NULL;
    delete iAviReader;
    iAviReader = NULL;
    delete iMessage;
    iMessage = NULL;
	}	


//Primes the controller
void CAviPlayController::PrimeL(TMMFMessage& aMessage)
	{
	if (iState != EStopped)  
		{
		User::Leave( KErrNotReady );	
		}
	if (!iClip)
		{
		User::Leave(KErrNotReady);
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
	

void CAviPlayController::PrimeL()
    {
    Panic(EBadCall);
    }
	

//Primes the controller
void CAviPlayController::StartPrimeL()
	{
	CheckAviReaderPresentL();
	CheckDevVideoPresentL();
    iAviReader->AudioEnabled(iAudioEnabled);
    iClip->SourcePrimeL();
    // Initialise Devsound
 	if (iAudioEnabled)
		{
		iDevSound->InitializeL(*this, EMMFStatePlaying);
	  	}
	if(!iVideoDecoderInitialized)
		{
		// Find the Decoder
	 	RArray<TUid> foundDecodersArray;
		CleanupClosePushL(foundDecodersArray);
		iDevVideoPlay->FindDecodersL(KXvidDecoderMimeType,
									 0, //  post-processing
										//  support is not needed
									 foundDecodersArray,
									 EFalse);
		TBool suitableDecoderFound = EFalse;
		if(foundDecodersArray.Count() > 0)
			{
 			TUncompressedVideoFormat reqOutFormat;
 			reqOutFormat.iDataFormat = ERgbFbsBitmap;
 			reqOutFormat.iRgbFormat = EFbsBitmapColor16M;
			// Here, we pick a decoder that can handle the output format
			// required
			suitableDecoderFound =
				SelectFirstSuitableDecoderL(foundDecodersArray, reqOutFormat);
			}

		// Inform client if we couldn't find a suitable decoder.
		if (!suitableDecoderFound)
			{
			SendErrorToClient(KErrNotFound);
			}
		else
			{
			// Set the video destination as Screen.This will leave if the
			// plug-in does not support DSA.
			iDevVideoPlay->SetVideoDestScreenL(ETrue);
			// Initialize devvideoPlay
			iDevVideoPlay->Initialize();
			}
		CleanupStack::PopAndDestroy(&foundDecodersArray);

		}
	else
		{
		if(iAudioEnabled)
			{
			iDevVideoInitialized = ETrue;
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


// Selection of a suitable decoder based on the output format required
TBool CAviPlayController::SelectFirstSuitableDecoderL(
	const RArray<TUid>& aDecodersArray,
	const TUncompressedVideoFormat& aRequiredOutputFormat)

	{
	RArray<TUncompressedVideoFormat> outputFormatsArray;
	CleanupClosePushL(outputFormatsArray);
	TBool suitableDecoderFound = EFalse;
	TInt numDecoders = aDecodersArray.Count();
	for (TInt i = 0; !suitableDecoderFound && i < numDecoders; i++)
		{
		iDecoderDeviceId = iDevVideoPlay->SelectDecoderL(aDecodersArray[i]);
		iDevVideoPlay->GetOutputFormatListL(iDecoderDeviceId,
											outputFormatsArray);
		if (outputFormatsArray.Find(aRequiredOutputFormat) !=
			KErrNotFound)
			{
			iDevVideoPlay->SetOutputFormatL(iDecoderDeviceId,
											aRequiredOutputFormat);
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
			CVideoDecoderInfo* pCVideoDecoderInfo =
				iDevVideoPlay->VideoDecoderInfoLC(aDecodersArray[i]);
			RArray<TInt> supportedScreensArray;
			CleanupClosePushL(supportedScreensArray);
			pCVideoDecoderInfo->GetSupportedScreensL(supportedScreensArray);
			// If supportedScreensArray.Count() is zero, it implies that
			// decoder does not support any specific screens and it should be
			// able to support any screen. Continue to initialize DevVideoPlay.
			TInt err = KErrNone;
			if(supportedScreensArray.Count() > 0)
				{
				// The decoder supports specific screens. Check if it supports
				// the screen number set by the client.
				err = supportedScreensArray.Find(iScreenNumber);
				}
			// If err is KErrNotFound, it implies that the decoder does not
			// support the given screen and hence not suitable for the client.
			// check if the next decoder in the list is suitable.
			if(KErrNotFound != err)
				{
				// Suitable decoder is found.
				iDevVideoInitialized = EFalse;
				suitableDecoderFound = ETrue;
				}
			// supportedScreensArray, pCVideoDecoderInfo
			CleanupStack::PopAndDestroy(2, pCVideoDecoderInfo);
#else
			iDevVideoInitialized = EFalse;
			suitableDecoderFound = ETrue;
#endif
			}
		outputFormatsArray.Reset();
		}
	CleanupStack::PopAndDestroy(&outputFormatsArray);

	return suitableDecoderFound;
	}


//Starts playing and transfers the data from data source to data sink.
void CAviPlayController::PlayL()
	{
	if (iState != EPrimed )
    	{
    	User::Leave(KErrNotReady);
    	}
	iClip->SourcePlayL();
	iDevVideoPlay->StartDirectScreenAccessL(iScreenRect,*iScreenDev,iDerivedClipRegion);
    iDevVideoPlay->Start();	
    if(iAudioEnabled)
  		{
    	iDevSound->PlayInitL();	
    	iState = EAudioReadyToPlay;
    	}
    else
    	{
    	iState = EPlaying;
    	}	
    	
   	}


//Pause playing and the data transfer from data source to data sink.
void CAviPlayController::PauseL()
    {
    User::Leave(KErrNotSupported);
	}


//Stops playing and the data transfer from data source to data sink.
void CAviPlayController::StopL()
	{
	if (iState == EStopped)
    	{
    	return;
    	}

	StopAudioL();
	StopVideoL();
	StopAviReaderL();
    iState = EStopped;
	}


//Returns the current playing position from devvideoplay.
TTimeIntervalMicroSeconds CAviPlayController::PositionL() const
	{
 	if (iState == EStopped)
    	{
    	return 0;
    	}
	CheckDevVideoPresentL();	
    return iDevVideoPlay->PlaybackPosition();
	}


//Set the position to play from.
void CAviPlayController::SetPositionL(const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	//This will leave with KErrNotsupported as there is no support for seeking 
	//position in an .avi file.
	User::Leave(KErrNotSupported);
	}


//Returns the duration of the clip.
TTimeIntervalMicroSeconds CAviPlayController::DurationL() const
	{
	CheckAviReaderPresentL();
	return iAviReader->Duration();
	}


//Handles a custom command.
void CAviPlayController::CustomCommand(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrNotSupported);
	}


//Sets the priority settings for devsound.
void CAviPlayController::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
 	iPrioritySettings = aPrioritySettings;
    if (iDevSound)
    	{
    	iDevSound->SetPrioritySettings(aPrioritySettings);
    	}
	}


//Gets the number of meta entries in the clip.
void CAviPlayController::GetNumberOfMetaDataEntriesL(TInt& /*aNumberOfEntries*/)
	{
	//Support to get meta data is not present.so this will leave with KErrNotSupported.
	User::Leave(KErrNotSupported);
	}



//Gets a meta entry at a specified index.
CMMFMetaDataEntry* CAviPlayController::GetMetaDataEntryL(TInt /*aIndex*/)
	{
	//Support to get meta data is not present.so this will leave with KErrNotSupported. 
	User::Leave(KErrNotSupported);
	return NULL;
	}

//----------------------------------------------- //
// MMMFVideoPlayControllerCustomCommandImplementor //
//-----------------------------------------------//


//Sets the screen clip region
void CAviPlayController::MvpcUpdateDisplayRegionL(const TRegion& aRegion)
	{
	iDisplayRegion.Copy(aRegion);
	UpdateClipRegion();
	}


//Gets the previously requested frame
void CAviPlayController::MvpcGetFrameL(MMMFVideoFrameMessage& aMessage)
	{
	CFbsBitmap& bitmap = aMessage.GetBitmap();
	TRAPD(err, CopyFrameL(bitmap));
	aMessage.FrameReady(err);	
	}


//Copies the previously requested frame
void CAviPlayController::CopyFrameL(CFbsBitmap& aBitmap)
	{
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(aBitmap.Resize(bitmap->SizeInPixels()));
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(&aBitmap);
	CleanupStack::PushL(bitmapDevice);
	CFbsBitGc* gc = NULL;
	User::LeaveIfError(bitmapDevice->CreateContext(gc));
	gc->DrawBitmap(TPoint(0,0), bitmap);
	gc->Clear();
	delete gc;
	CleanupStack::PopAndDestroy(2,bitmap);//for bitmapdevice and bitmap.
	}


//Gets if audio is enabled during playing.
void CAviPlayController::MvpcGetAudioEnabledL(TBool& aEnabled)
	{
	CheckAviReaderPresentL();
	iAviReader->AudioEnabled(iAudioEnabled);
	aEnabled = iAudioEnabled;
	}


//Sets the display window on the screen.
void CAviPlayController::MvpcSetDisplayWindowL(const TRect& aWindowRect, const TRect& aClipRect)
	{
	iScreenRect= aWindowRect;
	iClipRect = aClipRect;
	ASSERT(iScreenGc);
	UpdateClipRegion();
	}


//Aborts or resumes direct screen access depends on the DSA event received
void CAviPlayController::MvpcDirectScreenAccessEventL(const TMMFDSAEvent aDSAEvent)
	{
	CheckDevVideoPresentL();
	if (aDSAEvent == EAbortDSA)
		{
		if(iState == EPlaying)
			{
			iDevVideoPlay->AbortDirectScreenAccess();
			}
		}
	else if (aDSAEvent == EResumeDSA)
		{
		if((iState == EPrimed)||(iState == EPlaying))
			{
			iDevVideoPlay->StartDirectScreenAccessL(iScreenRect,*iScreenDev,iDerivedClipRegion);
			}
		}
	}

		
	


//Plays videoclip within the playwindow set
void CAviPlayController::MvpcPlayL(const TTimeIntervalMicroSeconds& /*aBegin*/,const TTimeIntervalMicroSeconds& /*aEnd*/)
	{
	//This will leave with KErrNotSupported as there is no provision to
	// seek the position in an .avi file.
	User::Leave(KErrNotSupported);
	}


//Redraws the current frame
void CAviPlayController::MvpcRefreshFrameL()
	{
	CheckDevVideoPresentL();
	iDevVideoPlay->Redraw();
	}


//Gets the progress of loading video clip
void CAviPlayController::MvpcGetLoadingProgressL(TInt& /*aPercentage*/)
	{
	//This will leave with KErrNotSuuported as there is no support 
	User::Leave(KErrNotSupported);
	}


//Prepares the controller for recording.
void CAviPlayController::MvpcPrepare()
	{
	TMMFEvent controllerEvent;
	controllerEvent.iEventType = KMMFEventCategoryVideoPrepareComplete;
	controllerEvent.iErrorCode = KErrNone;
	DoSendEventToClient(controllerEvent);		
	}


//Rotates the video file on the screen with the rotation angle given.
void CAviPlayController::MvpcSetRotationL(TVideoRotation aRotation)
	{
	//this will leave with KErrNotSupported as the Xvid Hwdevice does not support post processing.
	iRotation = aRotation;
	User::Leave(KErrNotSupported);
	}


//Gets the rotation applied to the video file.
void CAviPlayController::MvpcGetRotationL(TVideoRotation& aRotation)
	{
	//this will leave with KErrNotSupported as the Xvid Hwdevice does not support post processing.
	aRotation = iRotation;
	User::Leave(KErrNotSupported);
	}


//Scales the the video file on the screen with the scaling parameters given.
void CAviPlayController::MvpcSetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering)
	{
	//this will leave with KErrNotSupported as the Xvid Hwdevice does not support post processing.
	iWidthPercentage = aWidthPercentage;
	iHeightPercentage = aHeightPercentage;
	iAntiAliasFiltering = aAntiAliasFiltering;
	User::Leave(KErrNotSupported);	
	}


//Gets the scale factor applied for the video file.
void CAviPlayController::MvpcGetScaleFactorL(TReal32& aWidthPercentage, TReal32& aHeightPercentage, TBool& aAntiAliasFiltering)
	{
	//this will leave with KErrNotSupported as the Xvid Hwdevice does not support post processing.
	aWidthPercentage = iWidthPercentage;
	aHeightPercentage = iHeightPercentage;
	aAntiAliasFiltering = iAntiAliasFiltering;
	User::Leave(KErrNotSupported);
	}


//Sets the crop options for the image.
void CAviPlayController::MvpcSetCropRegionL(const TRect& aCropRegion)
	{
	//this will leave with KErrNotSupported as the Xvid Hwdevice does not support post processing.
	iCropRect = aCropRegion;
	User::Leave(KErrNotSupported);
	}


//Gets the crop options of the image.
void CAviPlayController::MvpcGetCropRegionL(TRect& aCropRegion)
	{
	//this will leave with KErrNotSupported as the Xvid Hwdevice does not support post processing.
	aCropRegion = iCropRect;
	User::Leave(KErrNotSupported);
	}


//Sets the video frame rate for recording.
void CAviPlayController::MvcSetFrameRateL(TReal32 /*aFramesPerSecond*/)
	{
	//There are no apis available at devvideo level to do this.
	//so, this will leave with KErrNotSupported.
	User::Leave(KErrNotSupported);
	}


//Gets the video frame rate applied for playing 
void CAviPlayController::MvcGetFrameRateL(TReal32& aFramesPerSecond)
	{
	CheckAviReaderPresentL();
	iAviReader->FrameRate(aFramesPerSecond);
	}


//Gets the audio codec used for playing.
void CAviPlayController::MvcGetAudioCodecL(TFourCC& aCodec)
	{
	if (iAudioEnabled)
		{
		aCodec = KMMFFourCCCodePCM16;
		CheckAviReaderPresentL();
	 	iAviReader->AudioCodec(aCodec);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}


//Gets the video codec used for playing.
void CAviPlayController::MvcGetVideoMimeTypeL(TDes8& aMimeType)
	{
	CheckAviReaderPresentL();
	aMimeType = KAviVideoCodec;
	iAviReader->VideoMimeType(aMimeType);
	}


//Gets the video bit rate used for playing.
void CAviPlayController::MvcGetVideoBitRateL(TInt& /*aBitRate*/)
	{
	//There are no devvideo api's to do this.
	//so, leave with KErrNotSuported.
	User::Leave(KErrNotSupported);
	}


//Gets the audio bit rate used for playing.
void CAviPlayController::MvcGetAudioBitRateL(TInt& aBitRate)
	{
	CheckAviReaderPresentL();
	if(iAudioEnabled)
		{
		aBitRate = (iAviReader->BitsPerSample() * iAviReader->SampleRate()* iAviReader->Channels());
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}


//Returns the video framesize of the file.
void CAviPlayController::MvcGetVideoFrameSizeL(TSize& aSize)
	{
	CheckAviReaderPresentL();
	iAviReader->VideoFrameSize(aSize);
	}



//Sets the volume during playing.
void CAviPlayController::MapdSetVolumeL(TInt aVolume)
	{
	if (iAudioEnabled)
		{
		CheckDevSoundPresentL();
		TInt maxVolume = iDevSound->MaxVolume();
		iDevSound->SetVolume(aVolume);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}


//Gets the maximum audio volume for playing.
void CAviPlayController::MapdGetMaxVolumeL(TInt& aMaxVolume)
	{
	if(iAudioEnabled)
		{
		CheckDevSoundPresentL();
		aMaxVolume = iDevSound->MaxVolume();
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}


//Gets the volume for playing
void CAviPlayController::MapdGetVolumeL(TInt& aVolume)
	{
	if (iAudioEnabled)
		{
		CheckDevSoundPresentL();
		aVolume = iDevSound->Volume();
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}


//Gets the volume ramp for playing
void CAviPlayController::MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	if (iAudioEnabled)
		{
		CheckDevSoundPresentL();
		iDevSound->SetVolumeRamp(aRampDuration);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}


//Set the audio balance for playing.
void CAviPlayController::MapdSetBalanceL(TInt aBalance)
	{
	if (iAudioEnabled)
		{
		CheckDevSoundPresentL();
		if (aBalance < KMMFBalanceMaxLeft)
			{
    		aBalance = KMMFBalanceMaxLeft;	
    		}
		else if (aBalance > KMMFBalanceMaxRight)
			{
			aBalance = KMMFBalanceMaxRight;
			}
		TInt left = (100 * (aBalance-KMMFBalanceMaxRight)) / (KMMFBalanceMaxLeft-KMMFBalanceMaxRight);
		TInt right = 100 - left;
		iDevSound->SetPlayBalanceL(left, right);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}


//Get the audio balance applied for playing
void CAviPlayController::MapdGetBalanceL(TInt& aBalance)
	{
	if (iAudioEnabled)
		{
		CheckDevSoundPresentL();
		TInt left = 50; // arbitrary values 
		TInt right = 50;
		iDevSound->GetPlayBalanceL(left, right); 
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
	

//Checks if valid devsound object is present.
void CAviPlayController::CheckDevSoundPresentL()
	{
	if (!iDevSound)
		{
		User::Leave(KErrNotReady);
		}
	}


//Checks if valid devvideoplay object present
void CAviPlayController::CheckDevVideoPresentL() const
	{
	if (iDevVideoPlay && iVideoFatalError)
		{
		// A fatal error occured. This will recreate the DevVideo instance and
		// also will clear iVideoFatalError flag. We use the cast operator here
		// as we need to modify member data.
		const_cast<CAviPlayController*>(this)->RecreateDevVideoAfterFatalErrorL();
		}
	if (!iDevVideoPlay)
		{
		User::Leave(KErrNotReady);
		}
	}


//Checks if valid avireader object present.
void CAviPlayController::CheckAviReaderPresentL() const
	{
	if (!iAviReader)
    	{
    	User::Leave(KErrNotReady);
    	}
	}


//New empty buffers are available for decoding.Fill the video buffer
//with video data and send it for decoding.
void CAviPlayController:: MdvpoNewBuffers()
	{
	TVideoInputBuffer* newBuffer = NULL;
	ASSERT(iAviReader);
	ASSERT(iDevVideoPlay);
	if (!iAviReader->IsVideoInputEnd())
		{			
	   	// get a buffer of a minimum size from DevVideoPlay.
    	TRAPD(error, newBuffer = iDevVideoPlay->GetBufferL(KTestBufferSize));
		if (!newBuffer)
			{
			//Lack of free buffers are not considered as an error.Hence return.
			return;
			}
		   	
    	TRAP(error, iAviReader->FillVideoBufferL(newBuffer));
    	if (error != KErrNone)
			{
			SendErrorToClient(error);
			return;
			}
		if(iAviReader->IsVideoInputEnd())
			{
			TRAPD(error,iDevVideoPlay->WriteCodedDataL(newBuffer));
			if (error!= KErrNone)
				{
				SendErrorToClient(error);
				return;			
				}
			iDevVideoPlay->InputEnd();
			}
		}
	}


//Fatal error occured while decoding video.Send the error to client.
void CAviPlayController::MdvpoFatalError(TInt aError)
	{
	// No need to stop Video as this call back already implies a video problem,
	// therefore it is supposed to be stopped (trying to stop it twice will
	// cause a panic in devvideo).
    TRAP_IGNORE(StopAudioL(); StopAviReaderL());
	// At this point we have to recreate the DevVideo instance. Since it is not
	// safe to delete the instance inside this error call back, we use a flag
	// so the next time we need to use DevVideo it will be recreated.
	iVideoFatalError = ETrue;
	iDevVideoInitialized = EFalse;
	iVideoDecoderInitialized = EFalse;
	iState = EStopped;
    SendErrorToClient(aError);
    }


//Notifies the client that there are one or more new pictures are available.
void CAviPlayController::MdvpoNewPictures()
	{
    // dispose of the picture
	TVideoPicture* thePicture= NULL;
	ASSERT(iDevVideoPlay);
	TRAPD(err, thePicture = iDevVideoPlay->NextPictureL());
	if (err == KErrNone && thePicture != NULL)
		{
		iDevVideoPlay->ReturnPicture(thePicture);
		}
	}


//Initialization complete for devvideoplay.If audio is not enabled
//set state to EPrimed.
void CAviPlayController::MdvpoInitComplete(TInt aError)
	{
   	if (aError != KErrNone)
		{
		SendErrorToClient(aError);
		return;
		}
    iDevVideoInitialized = ETrue;
	// if audio is enabled we need to wait for both the audio and video initializations
	//before completing client's prime message.
	CheckForInitComplete();	
	}
 

//End of the video data in the file.
void CAviPlayController::MdvpoStreamEnd()
	{
	TRAP_IGNORE(StopL());
	SendErrorToClient(KErrNone);
	}


//Intended for future use.Will panic if called.
void CAviPlayController:: MdvpoReturnPicture(TVideoPicture* /*aPicture*/)
	{
    Panic(EBadCall);
    }
 

//Intended for future use.Will panic if called.
void CAviPlayController::MdvpoSupplementalInformation(const TDesC8 &/*aData*/, const TTimeIntervalMicroSeconds &/*aTimestamp*/, const TPictureId &/*aPictureId*/)
	{
   	Panic(EBadCall);
    }


//Intended for future use.Will panic if called.
void CAviPlayController:: MdvpoPictureLoss()
	{  
 	Panic(EBadCall);
 	}


//Intended for future use.Will panic if called.
void CAviPlayController::MdvpoPictureLoss(const TArray< TPictureId > &/*aPictures*/)
	{
   	Panic(EBadCall);
    }
    

//Intended for future use.Will panic if called.
void CAviPlayController:: MdvpoSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId &/*aPicture*/)
    {
   	Panic(EBadCall);
    }
    

//Intended for future use.Will panic if called.
void CAviPlayController::MdvpoReferencePictureSelection(const TDesC8 &/*aSelectionData*/)
    {
    Panic(EBadCall);
    }
    

//Intended for future use.Will panic if called.
void CAviPlayController::MdvpoTimedSnapshotComplete(TInt /*aError*/, TPictureData */*aPictureData*/, const TTimeIntervalMicroSeconds &/*aPresentationTimestamp*/, const TPictureId &/*aPictureId*/)
    {
    Panic(EBadCall);
    }

//Sets the initial screen number for the video display
void CAviPlayController::MvsdSetInitScreenNumber(TInt aScreenNumber)
	{
	iScreenNumber = aScreenNumber;
	}
	
//Devsound initialization is completed.Configure devsound capabilities. 
void CAviPlayController::InitializeComplete(TInt aError)
	{
	TInt error = aError;
   	if (error == KErrNone)
		{
		ASSERT(iDevSound);
		TMMFCapabilities devSoundCaps = iDevSound->Capabilities();
		TMMFCapabilities caps;
		ASSERT(iAviReader);
		TInt rate = iAviReader->SampleRate();
		TBool found = EFalse;
		for( TInt index =0; index < KNumSampleRates; index++)
			{
			if(rate == KRateLookup[index].iRate)
				{
				caps.iRate = KRateLookup[index].iRateEnum;
				found = ETrue;
				}
			}
    	if(!found)
    		{
    		error = KErrNotFound;
    		}
    	if(error == KErrNone)
    		{
    		caps.iChannels = iAviReader->Channels();
	    	if (caps.iChannels == 1) 
				{
				caps.iChannels = EMMFMono;
				}
			else if (caps.iChannels == 2)
				{
				caps.iChannels = EMMFStereo;
				}
			else 
				{
				error = KErrNotFound;
				}	
    		}
    	if(error == KErrNone)
    		{
    		caps.iEncoding = EMMFSoundEncoding16BitPCM;
    		TRAP(error,iDevSound->SetConfigL(caps));
        	if(error == KErrNone)
				{
				iDevSoundInitialized = ETrue;
				CheckForInitComplete();	
				}
			}
		}
	else
		{
		SendErrorToClient(error);
		}
		
   	 }

//Intended for future use.Will panic if called.
void CAviPlayController::ToneFinished(TInt /*aError*/)
   	{
 	Panic(EBadCall);
 	}


//This is called when an empty audio buffer is available.Fill the buffer with audio data.
void CAviPlayController::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (iState == EAudioReadyToPlay)
		{
		iState = EPlaying;	
		}
	ASSERT(iAviReader);
	TRAPD(err, iAviReader->FillAudioBufferL(aBuffer));
	//if error,send the error to client
	if(err)
		{
		SendErrorToClient(err);
		}
	}
	

//This is called when an audio play completion is successfully played or otherwise
void CAviPlayController::PlayError(TInt aError)
 	{
	ASSERT(iAviReader);
	if(aError == KErrUnderflow && iAviReader->IsAudioInputEnd())
		{//audio has reached the end of the file
		aError = KErrNone;
		}
	else
		{
		//Error in audio play.Stop the controller and send error to the client.
		TRAP_IGNORE(StopL());
		}
	SendErrorToClient(aError);
	}


//Will panic if called.Should not be called during playing
void CAviPlayController::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	 {
	 Panic(EBadCall);
	 }


//Will panic if called.Should not be called during playing
void CAviPlayController::RecordError(TInt /*aError*/)
	 {
     Panic(EBadCall);
	 }


//Will panic if called.Should not be called during playing
void CAviPlayController::ConvertError(TInt /*aError*/)
	 {
	 Panic(EBadCall);
	 }
	 

//Will panic if called.Should not be called during playing
void CAviPlayController::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	 {
	 Panic(EBadCall);
	 }


//Sends an event to client.
void CAviPlayController::SendEventToClient(const TMMFEvent& aEvent)
	{
    DoSendEventToClient(aEvent);
	}
 

//This function is called when an empty audio buffer filled with audio data.
//This can now be sent to devsound for decoding.
void CAviPlayController::AudioBufferFilled()
	{
	ASSERT(iDevSound);
	iDevSound->PlayData();
	}


//This function is called when an empty video buffer is filled with video data.
//This can now be sent for decoding.If there is no data then inform devvideo
//about end of video stream.
void CAviPlayController::VideoBufferFilled(TVideoInputBuffer* aBuffer)
	{
	ASSERT(iDevVideoPlay);
	ASSERT(iAviReader);
	TRAPD(error,iDevVideoPlay->WriteCodedDataL(aBuffer));
	if(error != KErrNone)
		{
		SendErrorToClient(error);
		return;
		}
	if(iAviReader->IsVideoInputEnd())
		{
		iDevVideoPlay->InputEnd();
		}
	}



//This function is used when both audio and video are enabled for playing.
//The State is set to EPrimed when initialization is completed on both DevSound and DevVideo 
void CAviPlayController::CheckForInitComplete()
 	{
 	if(iAudioEnabled)
		{
 		if ((iDevVideoInitialized) &&(iDevSoundInitialized))
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
 	else
 		{
 		iState = EPrimed;
 		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
 		}
 	iVideoDecoderInitialized = ETrue;
 	}
	


//Sends the error message to the client.
void CAviPlayController::SendErrorToClient(TInt aError)  
	{
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

CAviPlayController::CMMFMessageHolder* CAviPlayController::CMMFMessageHolder::NewL(TMMFMessage& aMessage)
	{
	return new(ELeave) CMMFMessageHolder(aMessage);
	}
		
void CAviPlayController::CMMFMessageHolder::Complete(TInt aError) 
	{
	iMessage.Complete(aError);
	}
	
CAviPlayController::CMMFMessageHolder::CMMFMessageHolder(TMMFMessage& aMessage): CBase(), iMessage(aMessage)
	{
	}


CAviPlayController::CSourceSinkEventHandler::CSourceSinkEventHandler(CAviPlayController& aParent):iParent(aParent)
	{
	}

CAviPlayController::CSourceSinkEventHandler::~CSourceSinkEventHandler()
	{
	}

TInt CAviPlayController::CSourceSinkEventHandler::SendEventToClient(const TMMFEvent& aEvent)
	{
	iParent.SendEventToClient(aEvent);
	return KErrNone;
	}

// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KAviPlayControllerUid,CAviPlayController::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

void CAviPlayController::MarnRegisterAsClientL(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	//If file is open, check if audio is enabled
	if(iAviReader)
		{
		iAviReader->AudioEnabled(iAudioEnabled);
		if(!iAudioEnabled)
			{
			User::Leave(KErrArgument);	
			}
		}
	//[ precondition that we have a sink]
	CheckDevSoundPresentL();
	
	//[register the notification ]
	TInt err = iDevSound->RegisterAsClient(aEventType, aNotificationRegistrationData);
	User::LeaveIfError(err);
	}

void CAviPlayController::MarnCancelRegisterAsClientL(TUid aEventType)
	{
	//[ precondition that we have a sink]
	CheckDevSoundPresentL();
	
	//[cancel the notification ]
	TInt err = 	iDevSound->CancelRegisterAsClient(aEventType);
	User::LeaveIfError(err);
	}

void CAviPlayController::MarnGetResourceNotificationDataL(TUid aEventType, TDes8& aNotificationData)
	{
	//[ precondition that we have a sink]
	CheckDevSoundPresentL();
	
	//[get the notification data]
	TMMFTimeIntervalMicroSecondsPckg pckg;
	TInt err = iDevSound->GetResourceNotificationData(aEventType, pckg);
	User::LeaveIfError(err);
	
	// aNotificationData is a package buffer returned as TMMFTimeIntervalMicroSecondsPckg.
	// The contents should be converted to an integer and interpreted as samples played,
	// but not as a microsecond value. 
	// As the client expects a position (in microseconds from the beginning
	// of the clip) we need to convert the data depending on the sample rate.
	// Potential issue if using the number of samples played with VBR sampling.
	TUint rate = 0;
	TMMFCapabilities caps = iDevSound->Config();
	for( TInt i = 0; i < KNumSampleRates; i++)
		{
		if(caps.iRate == KRateLookup[i].iRateEnum)
			{
			rate = KRateLookup[i].iRate;
			break;	
			}
		}
	if(rate != 0)
		{
		// Convert the given number of samples using the sample rate
		const TInt KMicroSecsInOneSec = 1000000;
		TTimeIntervalMicroSeconds value = pckg();
		value = TTimeIntervalMicroSeconds(value.Int64() * ((TReal)KMicroSecsInOneSec / rate));
		pckg() = value;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	aNotificationData = pckg;
	}

void CAviPlayController::MarnWillResumePlayL()
	{
	//[ precondition that we have a sink]
	CheckDevSoundPresentL();
	
	//[wait for the client to resume ]
	TInt err = iDevSound->WillResumePlay();	
	User::LeaveIfError(err);
	}

// Stops video.
void CAviPlayController::StopVideoL()
	{
	// If already stopped, do nothing
	if (iState == EStopped)
    	{
    	return;
    	}

	CheckDevVideoPresentL();
	// AbortDirectScreenAccess and Stop can only be used if DevVideoPlay has
	// been already initialized. If CheckDevVideoPresentL has just recreated
	// the DevVideoPlay instance, no need to make these calls on DevVideoPlay.
	if (iDevVideoInitialized)
		{
		iDevVideoPlay->AbortDirectScreenAccess();
		iDevVideoPlay->Stop();
		iDevVideoInitialized = EFalse;
		}
	}


// Stops audio (if enabled).
void CAviPlayController::StopAudioL()
	{
	// If already stopped, do nothing
	if (iState == EStopped || !iAudioEnabled)
    	{
    	return;
    	}

	CheckDevSoundPresentL();
	iDevSound->Stop();
	iDevSoundInitialized = EFalse;
	}

// Stops the data transfer from data source to data sink.
void CAviPlayController::StopAviReaderL()
	{
	// If already stopped, do nothing
	if (iState == EStopped)
    	{
    	return;
    	}

	// Stop avi reader
    CheckAviReaderPresentL();
    iAviReader->ResetL();
	}

// This non-const method is used to encapsulate non-const actions that need to
// take place within a const method so there's no need to change the constness
// definition of the caller method.
void CAviPlayController::RecreateDevVideoAfterFatalErrorL()
	{
	iVideoFatalError = EFalse;
	delete iDevVideoPlay;
	iDevVideoPlay = NULL;
	iDevVideoPlay = CMMFDevVideoPlay::NewL(*this);
	iDevVideoInitialized = EFalse;
	}

// Called when either iDisplayRegion or iClipRect are changed. The derived
// clip region is the intersection of these. This method can be called
// during playback, so the new region is passed immediately to DevVideo.
void CAviPlayController::UpdateClipRegion()
	{
	iDerivedClipRegion.Copy(iDisplayRegion);
	iDerivedClipRegion.ClipRect(iClipRect);
	
	if(iDevVideoInitialized)
		{
		// This isn't necessary in all cases, for instance there may be a
		// direct screen access start/stop, which would also set the clipping
		// region. This is added for completeness.
		iDevVideoPlay->SetScreenClipRegion(iDerivedClipRegion);
		}
	
	if(iScreenGc)
		{
		iScreenGc->SetClippingRegion(iDerivedClipRegion);
		}
	}
