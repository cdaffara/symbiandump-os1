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
// Part of the MVS Agents for TechView
//

#include "videorecordagent.h"
#include <e32def.h>

_LIT(KTestCameraPluginName, "C:\\102070cc.txt");

_LIT(KMMCameraPluginName, "C:\\mmcameraclientplugin.txt");

/**
Constructs and initialises a new instance of the MVS video recorder agent.

The function leaves if the video recorder agent object cannot be created.

@param  aObserver
        An object to receive video recorder notifications.
*/
EXPORT_C CMVSVideoRecordAgent* CMVSVideoRecordAgent::NewL(MMVSClientObserver& aObserver)
	{
	CMVSVideoRecordAgent* self = new(ELeave) CMVSVideoRecordAgent(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}	
     
void CMVSVideoRecordAgent::ConstructL()
	{  
	User::LeaveIfError(iFileLogger.Connect());
    iFileLogger.CreateLog(_L("LogMVSappUi"),_L("LogFile.txt"),EFileLoggingModeAppend);
    iVideoRecorder = CVideoRecorderUtility::NewL( *this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	// The plugin relies on a specific file existing
	// hence this has to be created.
	CreateDependencyFileL();
	}
	
     
CMVSVideoRecordAgent::CMVSVideoRecordAgent(MMVSClientObserver& aObserver):iObserver(aObserver)
	{
    
	}


/**
Destructor. Closes the current clip, if one is open, and frees resources.
*/	 
EXPORT_C CMVSVideoRecordAgent::~CMVSVideoRecordAgent()
	{
	if (iCamera)
		{
		iCamera->PowerOff();
		delete iCamera;
		}
	RemoveDependencyFile();
	delete iVideoRecorder;
	if(iFileLogger.Handle())
     	{
     	iFileLogger.CloseLog();
	 	iFileLogger.Close();
     	}
	}
	

/**
Opens a video clip from a file to play back existing video data or to record new video data.
It closes any existing open files and file sessions.And opens a new session with the file server

@param  aFile
        The full path and filename of the file which is a source of existing audio sample data.

@param  aControllerUid
        The UID of the controller to use for recording.
*/
EXPORT_C TInt CMVSVideoRecordAgent::OpenFileL(TDesC &aFile, TUid aControllerUid)
	{
	if(!iCamera)
		{
		iCamera = CCamera::NewL(*this,0,0);	
		iCamera->Reserve();
		iCamera->PowerOn();
		}
	
	TBool didICreateFsSession = EstablishFsSessionL(); //Setup an Fs session
    iFs.MkDirAll(aFile); //Make all directories
    TBool didICreateFileSession = EstablishFileSessionL(aFile);
    //If this method established these R-Handles, then it should close them
    if(didICreateFileSession)
    	{
        CloseFileSession(); //The file session
        }
    if(didICreateFsSession)
        {
        CloseFsSession(); //The file server
        }
    iVideoRecorder->OpenFileL(aFile,iCamera->Handle(),aControllerUid,
									KNullUid);	
	return(KErrNone);
	}


/**
This method launches and initializes plugin controller based on the stream 
source header passed in the descriptor buffer.
Specifies a descriptor that can be used to store a recorded video clip.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aDescriptor
        The descriptor used to store the recorded video clip.
@param  aCameraHandle
        The handle of the camera to record from.
@param  aControllerUid
        The UID of the controller to use for recording.
@param  aVideoFormatUid
        The UID of the video format to record to.
@param  aVideoType
        The descriptor containing the video MIME type.
@param  aAudioType
        A four character code representing the encoding to be used for audio data.
*/
EXPORT_C void CMVSVideoRecordAgent::OpenDesL(TDes8 &aDescriptor, TInt aCameraHandle, TUid   aControllerUid, TUid aVideoFormat, const TDesC8 &aVideoType, TFourCC aAudioType)
	{
	iVideoRecorder->OpenDesL(aDescriptor, aCameraHandle, aControllerUid, aVideoFormat, aVideoType, aAudioType);
	}
 
void CMVSVideoRecordAgent::MvruoOpenComplete(TInt aError)
	{
	iFileLogger.Write(_L("CMVSVideoRecordAgent:MvruoOpenComplete"));
	if(aError == KErrNone)
    	{
    	iState = EVideoOpening;
    	iFileLogger.Write(_L("MvruoOpenComplete:VideoOpening"));
    	iObserver.UpdateStateChange(iState, KErrNone);
    	iVideoRecorder->Prepare();
    	}
     else
		{
     	iState = ENotReady; //init failed so from opening to NotReady
     	iFileLogger.Write(_L("MvruoOpenComplete Failed:Opening to Notready"));
		iObserver.UpdateStateChange(iState, aError);
		}
	}

void CMVSVideoRecordAgent::MvruoPrepareComplete(TInt aError)
	{
	iFileLogger.Write(_L("CMVSVideoRecordAgent:MvruoPrepareComplete"));
	if(aError == KErrNone)
		{	
		iState = EVideoOpened;
		iFileLogger.Write(_L("MvruoPrepareComplete:VideoOpened"));		
		}
	else
		{
		iState = ENotReady; //prepare failed so from opening to NotReady
		iFileLogger.Write(_L("MvruoPrepareComplete Failed:Opening to Notready"));
		
		}
	iObserver.UpdateStateChange(iState, aError);
	}

void CMVSVideoRecordAgent::MvruoRecordComplete(TInt aError)
	{
	iFileLogger.Write(_L("CMVSVideoRecordAgent:MvruoRecordComplete"));
	if(aError == KErrNone)
		{	
		iState = EVideoOpened;
		iFileLogger.Write(_L("MvruoRecordComplete:VideoOpened"));	
		}
	else
		{
		iState = ENotReady; //record complete failed so from opening to NotReady
		iFileLogger.Write(_L("MvruoRecordComplete Failed:Opening to Notready"));
		
		}
	iObserver.UpdateStateChange(iState, aError);
	}

void CMVSVideoRecordAgent::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{

	}


/**
Closes a video clip.
*/
EXPORT_C void  CMVSVideoRecordAgent::Close()
	{
	iVideoRecorder->Close();
	iFileLogger.Write(_L("CMVSVideoRecordAgent Closed"));	
	iObserver.UpdateStateChange(ENotReady, KErrNone);	
	}
	

/**
Starts the recording of video data to the specified file.

If the video format also supports the use of an audio track, recording of the audio track can
be enabled using SetAudioEnabledL().

The client will receive a MvruoRecordComplete on completion of recording,
unless it explicitly ends recording with Stop. Prepare() should be called before the first
call to Record().
*/
EXPORT_C void CMVSVideoRecordAgent::Record()
	{
	iVideoRecorder->Record();
	iObserver.UpdateStateChange(EVideoRecording, KErrNone);
	}
	

/**
Stops the recording of video data to the specified file.

If the video format supports audio tracks and audio is enabled, this function also stops the
recording of the audio data.

Recording is stopped without sending the MvruoRecordComplete message to the client.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMVSVideoRecordAgent::Stop()
	{
	TInt err = iVideoRecorder->Stop();
	if(err == KErrNone)
		{
		iFileLogger.Write(_L("CMVSVideoRecordAgent Stopped"));
		iObserver.UpdateStateChange(EVideoStopped, KErrNone);	
		}
	else
		{
		iFileLogger.Write(_L("CMVSVideoRecordAgent Stop Failed"));
		iObserver.UpdateStateChange(ENotReady, err);	
		}
	return err;
	}
	
	
/**
Pauses the recording of video data to the specified file.

If the video format supports audio tracks and audio is enabled, this function also pauses the
recording of the audio data.

If an error occurs, this function leaves with one of the system wide error codes.
*/
EXPORT_C void CMVSVideoRecordAgent::PauseL()
	{
	iVideoRecorder->PauseL();
	iFileLogger.Write(_L("CMVSVideoRecordAgent Paused"));
	iState = EVideoPaused;	
	iObserver.UpdateStateChange(EVideoPaused, KErrNone);
	}


/**
Sets the recording priority. This is used to arbitrate between simultaneous accesses of
the hardware.

@param  aPriority
        A priority between EMdaPriorityMin and EMdaPriorityMax. EMdaPriorityMin means the client
        can be interrupted by any other client, EMdaPriorityNormal means the client can only
        be interrupted by a client with a higher priority or EMdaPriorityMax means the client
        cannot be interrupted by other clients.
@param  aPref
        Time vs Quality priority preferences, enumerated in TMdaPriorityPreference
*/	
EXPORT_C void CMVSVideoRecordAgent::SetPriorityL(TInt aPriority, TMdaPriorityPreference aPref)
	{
	iFileLogger.Write(_L("Setting Priority of CMVSVideoRecordAgent "));
	iVideoRecorder->SetPriorityL(aPriority, aPref);
	}
	


/**
Sets the video frame rate.

@param  aFrameRate
        The video frame rate, in frames/second.
*/
EXPORT_C void CMVSVideoRecordAgent::SetVideoFrameRateL(TReal32 aFrameRate)
	{
	iFileLogger.Write(_L("Setting VideoFrameRate of CMVSVideoRecordAgent "));
	iVideoRecorder->SetVideoFrameRateL(aFrameRate);
	}
	

/**
Sets the video frame size in pixels.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aSize
        The video frame size, in pixels
*/
EXPORT_C void CMVSVideoRecordAgent::SetVideoFrameSizeL(const TSize &aSize)
	{
	iFileLogger.Write(_L("Setting VideoFrameSize of CMVSVideoRecordAgent "));
	iVideoRecorder->SetVideoFrameSizeL(aSize);
	}
	

/**
Sets the video bit rate.

@param  aVideoBitRate
        The video bit rate in bits/second.
        This can be called with the parameter KMMFVariableVideoBitRate
        which specifies a variable bit rate.
*/
EXPORT_C void CMVSVideoRecordAgent::SetVideoBitRateL(TInt aBitRate)
	{
	iFileLogger.Write(_L("Setting VideoBitRate of CMVSVideoRecordAgent "));
	iVideoRecorder->SetVideoBitRateL(aBitRate);
	}


/**
Sets the audio bit rate.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aAudioBitRate
        The video bit rate in bits/second.
*/
EXPORT_C void CMVSVideoRecordAgent::SetAudioBitRateL(TInt aBitRate)
	{
	iFileLogger.Write(_L("Setting AudioBitRate of CMVSVideoRecordAgent "));
	iVideoRecorder->SetAudioBitRateL(aBitRate);
	}


/**
Sets whether the current clip has an audio stream.

@param  aEnabled
        A boolean indicating if the audio stream is enabled. ETrue to enable audio, EFalse to 
        disable it.
*/
EXPORT_C void CMVSVideoRecordAgent::SetAudioEnabledL(TBool aEnabled)
	{
	iVideoRecorder->SetAudioEnabledL(aEnabled);
	}


/**
Sets the audio recording gain.

If you want to disable audio, rather than setting the recording gain to 0, use SetAudioEnabledL().

If an error occurs, this function leaves with one of the system wide error codes

@param  aGain
        The recording gain, between 0 and MaxGain().
*/
EXPORT_C void CMVSVideoRecordAgent::SetGainL(TInt aGain)
	{
	iFileLogger.Write(_L("Setting Gain of CMVSVideoRecordAgent "));
	iVideoRecorder->SetGainL(aGain);
	}
	

/**
Sets the codec to use for the video component of the video clip.

This represents the encoding of the video data within the current format. To retrieve a list of 
available video codecs, use GetSupportedVideoTypesL().

If an error occurs, this function leaves with one of the system wide error codes.

@param  aDataType
        The descriptor containing the video MIME type.
*/
EXPORT_C void CMVSVideoRecordAgent::SetVideoTypeL(const TDesC8 &aType)
	{
	iFileLogger.Write(_L("Setting VideoType of CMVSVideoRecordAgent "));
	iVideoRecorder->SetVideoTypeL(aType);
	}
	

/**
Sets the audio type. This represents the encoding of the audio
data within the current format.

To retrieve a list of available audio codecs, use GetSupportedAudioTypesL().

If an error occurs, this function leaves with one of the system wide error codes.

@param  aDataType
        The Four Character Code representing the Audio Type.
*/
EXPORT_C void CMVSVideoRecordAgent::SetAudioTypeL(TFourCC aType)
	{
	iFileLogger.Write(_L("Setting AudioType of CMVSVideoRecordAgent "));
	iVideoRecorder->SetAudioTypeL(aType);
	}
	
	
/**
Gets the current recording priority. This is used to arbitrate between simultaneous accesses of
the sound hardware.

@param  aPriority
        A priority between EMdaPriorityMin and EMdaPriorityMax. EMdaPriorityMin means the client
        can be interrupted by any other client, EMdaPriorityNormal means the client can only
        be interrupted by a client with a higher priority or EMdaPriorityMax means the client
        cannot be interrupted by other clients.
@param  aPref
        Time vs Quality priority preferences, enumerated in TMdaPriorityPreference
*/
EXPORT_C void CMVSVideoRecordAgent::GetPriorityL(TInt &aPriority, TMdaPriorityPreference &aPref)
	{
	iFileLogger.Write(_L("Getting Priority of CMVSVideoRecordAgent "));
	iVideoRecorder->GetPriorityL(aPriority, aPref);
	}
	

/**
Returns the video frame rate.

If an error occurs, this function leaves with one of the system wide error codes.

@return The frame rate in frames/second.
*/
EXPORT_C TReal32 CMVSVideoRecordAgent::VideoFrameRateL()
	{
	iFileLogger.Write(_L("CMVSVideoRecordAgent:Getting VideoFrameRate "));
	return iVideoRecorder->VideoFrameRateL();
	}


/**
Gets the video frame size.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aSize
        The video frame size, in pixels.
*/
EXPORT_C void CMVSVideoRecordAgent::GetVideoFrameSizeL(TSize &aSize)
	{
	iVideoRecorder->GetVideoFrameSizeL(aSize);
	iFileLogger.Write(_L("CMVSVideoRecordAgent:Getting VideoFrameSize "));
	}
	

/**
Returns the video bit rate.

@return The video bit rate in bits/second or KMMFVariableVideoBitRate.
*/
EXPORT_C TInt CMVSVideoRecordAgent::VideoBitRateL()
	{
	iFileLogger.Write(_L("CMVSVideoRecordAgent:Getting VideoBitRate "));
	return iVideoRecorder->VideoBitRateL();
	}


/**
Gets the audio bit rate.

If an error occurs, this function leaves with one of the system wide error codes.

@return The audio bit rate in bits/second.
*/
EXPORT_C TInt CMVSVideoRecordAgent::AudioBitRateL()
	{
	iFileLogger.Write(_L("CMVSVideoRecordAgent:Getting AudioBitRate "));
	return iVideoRecorder->AudioBitRateL();
	}


/**
Returns the status of audio support for the video clip currently being recorded.

@return A boolean indicating if an audio stream is present. ETrue if it is, otherwise EFalse.
*/
EXPORT_C TBool CMVSVideoRecordAgent::AudioEnabledL()
	{
	return iVideoRecorder->AudioEnabledL();
	}
	

/**
Returns the length of the video clip.

@return The length of the video clip in microseconds.
*/
EXPORT_C TTimeIntervalMicroSeconds CMVSVideoRecordAgent::DurationL()
	{
	return iVideoRecorder->DurationL();
	}
	

/**
Returns the current recording gain.

If an error occurs, this function leaves with one of the system wide error codes.

@return The current recording gain, between 0 and MaxGain().
*/
EXPORT_C TInt CMVSVideoRecordAgent::GainL() 
	{
	return iVideoRecorder->GainL();
	}


/**
Returns the maximum possible setting for the recording gain.

@return An integer value specifying the maximum recording gain.
*/
EXPORT_C TInt CMVSVideoRecordAgent::MaxGainL() 
	{
	return iVideoRecorder->MaxGainL();
	}
	
/**
Adds the meta data entries to the video clip from the metaarray that is supplied from the client
side.

@param  aMetaArray
        An array containing new meta data entries,to be added to the clip.
*/
EXPORT_C void CMVSVideoRecordAgent::SetMetaDataL(RPointerArray<CMMFMetaDataEntry> aMetaArray)
	{
	// add the metadata
    TInt count=aMetaArray.Count();
    for(TInt index=0;index<count;index++)
    	{
    	iVideoRecorder->AddMetaDataEntryL(*aMetaArray[index]) ;		
    	}
  	}
  	
/**
Returns the audio type. This represents the encoding of audio data within the current format.

If an error occurs, this function leaves with one of the system wide error codes.

@return A Four Character Code representing the AudioType.
*/
EXPORT_C TFourCC CMVSVideoRecordAgent::AudioTypeL()
	{
	return iVideoRecorder->AudioTypeL();
	}


/**
Gets a list of the video types that are supported for recording to.

@param  aVideoTypes
        An array of Four Character Codes representing the video types supported.
*/
EXPORT_C void CMVSVideoRecordAgent::GetSupportedVideoTypesL(CDesC8ArrayFlat &aVideoTypes)
	{
	iFileLogger.Write(_L("CMVSVideoRecordAgent:Getting Supported VideoTypes "));
	iVideoRecorder->GetSupportedVideoTypesL(static_cast<CDesC8Array&>(aVideoTypes));
	}


/**
Gets a list of the audio types that are supported for recording to.

@param  aAudioTypes
        An array of Four Character Codes representing the audio types supported.
*/
EXPORT_C void CMVSVideoRecordAgent::GetSupportedAudioTypesL(RArray< TFourCC > &aAudioTypes) 
	{
	iFileLogger.Write(_L("CMVSVideoRecordAgent:Getting Supported AudioTypes "));
	iVideoRecorder->GetSupportedAudioTypesL(aAudioTypes);
	}


/**
Returns the video format's MIME type.

@return The video clip's MIME type.
*/
EXPORT_C const TDesC8& CMVSVideoRecordAgent::VideoFormatMimeType()
	{
	return iVideoRecorder->VideoFormatMimeType();
	}
	
/**
Returns the controller implementation information associated with the current controller.

@return The controller implementation structure
*/
EXPORT_C const CMMFControllerImplementationInformation & CMVSVideoRecordAgent::ControllerImplementationInformationL()
	{
	return (iVideoRecorder->ControllerImplementationInformationL());		
	}


/**
On return the function returns Supported FrameRates of the on board camera,that would be used
to record video.

@param aFrameRatesArray
	   Array containing the supported frame rates of camera.
*/
	
EXPORT_C void CMVSVideoRecordAgent::GetSupportedFrameRatesArrayL(RArray<TReal32>& aFrameRatesArray)
	{
	TInt counter = 0;
//	TInt supportedframerates = iCamera.iInfo.iNumImageSizesSupported;
	for(counter=0 ; counter < 2; counter++)
		{
		TReal32 temp;
		iCamera->EnumerateVideoFrameRates(temp, counter,
												CCamera::EFormatFbsBitmapColor16M,counter);	
		aFrameRatesArray.Append(temp);
		}
	
	}
    

/**
On return the function returns Supported FrameSizes of the on board camera,that would be used
to record video.

@param aFrameSizesArray
	   Array containing the supported frame sizes of camera.
*/
EXPORT_C void CMVSVideoRecordAgent::GetSupportedFrameSizesArrayL(RArray<TSize>& aFrameSizesArray)
	{
	TInt counter = 0;
//	TInt supportedframesizes = iCamera.iInfo.iNumVideoFrameSizesSupported;
	for(counter=0 ; counter < 2; counter++)
		{
		TSize temp;
		iCamera->EnumerateVideoFrameSizes(temp,counter,CCamera::EFormatFbsBitmapColor16M);
		aFrameSizesArray.Append(temp);
		}
	}
/**
Returns the current state of MVSClient. Provided for MVSAppUi to enable/disable the 
menu items depending on the current state of the MVSClient
*/
EXPORT_C TMVSState CMVSVideoRecordAgent::State()
  	{
  	return iState;
  	}
  	
TBool CMVSVideoRecordAgent::EstablishFsSessionL()
    {
    if(iFsSessionEstablished)
         {
         return EFalse; //An Fs Session exists, so don't create another
         }
    else
         {
         User::LeaveIfError(iFs.Connect()); //Connect to the server
         iFsSessionEstablished = ETrue; //An Fs Session is now established
         return ETrue;  //This method created the session.
         }
    }
     
TBool CMVSVideoRecordAgent::EstablishFileSessionL(TDesC& aFile)
	{
    TInt err = KErrNone;     //Holds error codes
    if(iFileSessionEstablished)
         {
         return EFalse; //A File Session exists, so don't create another
         }
    else
         {
         TBool sessionExists = EstablishFsSessionL(); //Ensure there is a valid Fs first.
         //If we created the Fs, there has been an error.
         if(sessionExists)
              {
              CloseFsSession();
              return EFalse;
              }
	     err = iFile.Create(iFs,aFile,EFileWrite);
         if(KErrNone != err)
         	{
            CloseFileSession(); //Close the session
            return EFalse; //Closed the File Session => did not create
            }
         iFileSessionEstablished = ETrue; //A File session now exists
         return ETrue; //This method created a File Session
         }
	}
/*  
CloseFsSession()

Closes the File Server session. This causes the IsFsSessionEstablished flag
to be set to false.
*/
void CMVSVideoRecordAgent::CloseFsSession()
    {
    iFs.Close(); //Close the Fs Session
    iFsSessionEstablished = EFalse; //Fs Session is no longer established
    return;
    }
    
/*
CloseFileSession()

Close the file session. This causes the IsSessionEstablished flag to be set
to false.
*/
void CMVSVideoRecordAgent::CloseFileSession()
    {
    iFile.Close(); //Close the File session
    iFileSessionEstablished = EFalse;  //File session is no longer established
    return;
    }

		
void CMVSVideoRecordAgent::CreateDependencyFileL()
	{
	// enforce dependency
	// create the file the plugin depends on.
	User::LeaveIfError(ifsSession.Connect());
	RFile file;

	ifsSession.Delete(KMMCameraPluginName);

	TInt err = file.Replace(ifsSession, KTestCameraPluginName, EFileShareAny);
  	file.Close();
  	if(err != KErrNone)
  		{
	    User::LeaveIfError(KErrNotSupported);
  		}
	}

void CMVSVideoRecordAgent::RemoveDependencyFile()
	{
	ifsSession.Delete(KTestCameraPluginName);
	ifsSession.Close();
	}
	
void CMVSVideoRecordAgent::HandleEvent(const TECAMEvent &/*aEvent*/)
	{
	}

void CMVSVideoRecordAgent::ViewFinderReady(MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
	{
	}

void CMVSVideoRecordAgent::ImageBufferReady(MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
	{
	}

void CMVSVideoRecordAgent::VideoBufferReady(MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)	
	{
	}
