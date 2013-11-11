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

#include "audiorecordagent.h"


/**
Constructs and initialises a new instance of the MVS Audio Record Agent for playing, recording and editing audio sample data.

The function leaves if the audio record agent object cannot be created.

@param  aObserver
        Class to receive state change events from record agent

@return A pointer to the new audio record agent object
*/
EXPORT_C CMVSAudioRecordAgent* CMVSAudioRecordAgent::NewL(MMVSClientObserver& aObserver)
	{
    CMVSAudioRecordAgent* self = new(ELeave) CMVSAudioRecordAgent(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CMVSAudioRecordAgent::ConstructL()
    {
    iRecorder = CMdaAudioRecorderUtility::NewL(*this);
    User::LeaveIfError(iFileLogger.Connect());
    iFileLogger.CreateLog(_L("LogMVSappUi"),_L("LogFile.txt"),EFileLoggingModeAppend);
    }

CMVSAudioRecordAgent::CMVSAudioRecordAgent(MMVSClientObserver& aObserver):iObserver(aObserver)
    {
    iObserver.UpdateStateChange(ENotReady, KErrNone);
    }

EXPORT_C CMVSAudioRecordAgent::~CMVSAudioRecordAgent()
	{
	delete iRecorder;
	if(iFileLogger.Handle())
     	{
     	iFileLogger.CloseLog();
	 	iFileLogger.Close();
     	}
    }
	

/**
Opens an audio clip from a file to play back existing audio data or to record new audio data.
It closes any existing open files and file sessions.And opens a new session with the file server
If the UID of the record controller is not passed, it inturn calls OpenFile(aFile) and the system
picks up the corresponding matching record controller for the supplied file.

@param  aFile
        The full path and filename of the file that contains or will contain the audio data.

@param  aRecordControllerUid
        The UID of the controller to use for recording.
*/
EXPORT_C void CMVSAudioRecordAgent::CreateNewFileL(TDesC& aFile,TUid aRecordControllerUid, TUid aPlayControllerUid)

	{
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
    iObserver.UpdateStateChange(EAudioOpening ,KErrNone);
    if(aRecordControllerUid==TUid::Null())
        {
	    iRecorder->OpenFileL(aFile);	
	    }
    else
    	{
     	iRecorder->OpenFileL(aFile,aRecordControllerUid,aPlayControllerUid);		
     	}
     
    return;
	}
	

/**
Opens a file to play back or record audio sample data.

@param  aFileName
        The full path and filename of the file which is a source of existing audio sample data.

@param  aRecordControllerUid
        The UID of the controller to use for recording.
*/
EXPORT_C void CMVSAudioRecordAgent::OpenFileL(const TDesC& aFile, TUid aRecordControllerUid,TUid aPlayControllerUid)
	{
	iRecorder->OpenFileL(aFile,aRecordControllerUid,aPlayControllerUid);
	iObserver.UpdateStateChange(EAudioOpening ,KErrNone);	
	}


/**
Opens a file to play back or record audio sample data.
allows MMF to choose controller
@param  aFileName
        The full path and filename of the file which is a source of existing audio sample data.
*/
EXPORT_C void CMVSAudioRecordAgent::OpenFileL(const TDesC& aFile)
	{
	iRecorder->OpenFileL(aFile);	
	iObserver.UpdateStateChange(EAudioOpening ,KErrNone);
	}
	

/**
Starts appending new audio sample data.

The new audio sample data is appended to the existing audio sample data in the same format 
as the existing audio sample data. If existing data is to be overwritten, then it should 
be cropped to the appropriate length before appending the new data.
*/
EXPORT_C void CMVSAudioRecordAgent::RecordL()
	{
	iRecorder->RecordL();	
	}
	

/**
Begins playback of audio sample data at the current volume, gain and priority levels.
*/
EXPORT_C void CMVSAudioRecordAgent::PlayL()
	{
	iRecorder->PlayL();	
	}


/**
Stops the current operation (playback/recording).

The operation is stopped at the current location. For a playback operation, the head is positioned 
at the last played data.
The state is set to EAudioStopped.
*/
EXPORT_C void CMVSAudioRecordAgent::Stop()
	{
	iRecorder->Stop();	
	iObserver.UpdateStateChange(EAudioStopped, KErrNone);
	}
    
    
/**
Sets the gain for the audio device to a specified value.

@param  aGain
        The gain setting. This can be any value from zero to the value returned by a call to
        MaxGain(). A value which is less than zero is set to zero. A value which is greater
        than MaxGain() is set to MaxGain().
*/
EXPORT_C void CMVSAudioRecordAgent::SetGain(TInt aGain)
    {
    iRecorder->SetGain(aGain);	
    }

  
/**
Deletes all audio sample data based on the current head position. The effects of the function
cannot be undone.The cropping is done from the beginning till the current head position if the 
flag is set,else the cropping is done from current position till the end of the clip.

@param aCropStart
       The position in the audio clip, in microseconds.
@param aFlag
       If the flag is set the cropping is done from beginning to the current head position
       else its done from the current position till the end of the clip.
*/
EXPORT_C void CMVSAudioRecordAgent::CropClipL(TTimeIntervalMicroSeconds& aCropStart, 
                   TBool aFlag)
    {
    TTimeIntervalMicroSeconds oldPos = iRecorder->Position();
    iRecorder->SetPosition(aCropStart);
    if(aFlag)
    	{
    	iRecorder->CropFromBeginningL();
    	}
    else
    	{
    	iRecorder->CropL();
    	}
    iRecorder->SetPosition(oldPos);
    }
    

/**
Adds the meta data entries to the audio clip from the metaarray that is supplied from the client
side.

@param  aMetaDataArray
        An array containing new meta data entries,to be added to the clip.
*/
EXPORT_C void  CMVSAudioRecordAgent::SetMetaDataL(RPointerArray<CMMFMetaDataEntry> aMetaArray)
    {
    // add the metadata
    TInt count=aMetaArray.Count();
    for(TInt index=0;index<count;index++)
    	{
    	iRecorder->AddMetaDataEntryL(*aMetaArray[index]) ;		
    	}
	}


/**
Sets the recording priority. 

The priority is used to arbitrate between multiple objects simultaneously trying to access the sound hardware.

@param  aPriority
        The priority level to apply.
@param  aPref
        The time and quality preferences to apply. 
*/
EXPORT_C void CMVSAudioRecordAgent::SetPriority(TInt aPriority, TMdaPriorityPreference aPref)
    {
    iRecorder->SetPriority(aPriority,aPref);	
    }
    

/**
Changes the current playback volume to a specified value.

The volume can be changed before or during playback and is effective
immediately. The volume can be set to any value between zero (mute) and 
the maximum permissible volume (determined using MaxVolume()).Also the 
period over which the volume level is to rise smoothly from nothing to
the normal volume level,is given by the ramp up time.


The function raises a CMdaAudioPlayerUtility 1 panic if
the audio player utility is not initialised.

@param  aVolume
        The volume setting. This can be any value from zero to
        the value returned by a call to
        CMVSAudioPlayAgent::MaxVolume().
        Setting a zero value mutes the sound. Setting the
        maximum value results in the loudest possible sound.

@param  aRamp
        The period over which the volume is to rise. A zero
        value causes the audio sample to be played at the
        normal level for the full duration of the playback. A
        value which is longer than the duration of the audio
        sample means that the sample never reaches its normal
        volume level.

*/
EXPORT_C void CMVSAudioRecordAgent::SetVolume(TInt aVolume, TTimeIntervalMicroSeconds aRamp)
    {
    iRecorder->SetVolume(aVolume);
    iRecorder->SetVolumeRamp(aRamp);
    }


/**
Sets the current playback balance.

The balance can be any value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight, the default value being KMMFBalanceCenter.

@param  aBalance
        The playback balance to set.

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the
        system-wide error codes.
*/
EXPORT_C TInt CMVSAudioRecordAgent::SetBalance(TInt aBalance)
    {
    return iRecorder->SetPlaybackBalance(aBalance);
    }


/**
Closes the current audio clip (allowing another clip to be opened)
Sets the current state to ENotReady
*/  
EXPORT_C void CMVSAudioRecordAgent::Reset()
    {
    iRecorder->Close();
    iObserver.UpdateStateChange(ENotReady, KErrNone);	
    }

	
/**
Sets the bit rate of the data sink in bits per second.

The bit rate must be one of the supported bit rates of the data sink.Use GetSupportedBitRatesL()
to retrieve a list of supported bit rates for the data sink.

@param  aBitRate
        Destination bit rate in bits/second.
*/
EXPORT_C void CMVSAudioRecordAgent::SetDestinationBitRateL(TUint aBitRate)
	{
	iRecorder->SetDestinationBitRateL(aBitRate);	
	}
	

/**
Sets the sample rate for the conversion destination data sink.

The sample rate must be one of the supported sample rates of the data sink. Use GetSupportedSampleRatesL()
to retreive a list of supported samples rates for the data sink.

@param  aSampleRate
        The sample rate of the conversion destination.
*/	
EXPORT_C void CMVSAudioRecordAgent::SetDestinationSampleRateL(TUint aSampleRate)
	{
	iRecorder->SetDestinationSampleRateL(aSampleRate);	
	}


/**
Sets the number of channels for the recorded audio clip.

The number of channels must be one of the values returned by GetSupportedNumberOfChannelsL().

@param  aNumberOfChannels
        The number of channels to record
*/
EXPORT_C void CMVSAudioRecordAgent::SetDestinationNumberOfChannelsL(TUint aNumberOfChannels)
	{
	iRecorder->SetDestinationNumberOfChannelsL(aNumberOfChannels);	
	}


/**
Sets the codec to use with the record controller.

@param  aCodec
        Four character code, representing the codec of the destination audio clip

@since 7.0s
*/
EXPORT_C void CMVSAudioRecordAgent::SetDestinationDataTypeL(TFourCC aDataType)
	{
	iRecorder->SetDestinationDataTypeL(aDataType);	
	}
		

//Getter methods - General


/**
Returns the current head position.

The head position is defined in terms of a time interval measured from the beginning of the audio sample data.

@return The current position in the audio clip, in microseconds.
*/
EXPORT_C TTimeIntervalMicroSeconds& CMVSAudioRecordAgent::Position() const
	{
	return const_cast<TTimeIntervalMicroSeconds&>(iRecorder->Position());
	}


/**
Returns the current gain setting of the audio device.

@param  aGain
        On return contains the current gain setting for the audio device. Can be
        any value between zero and MaxGain().

@return An error code indicating if the call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMVSAudioRecordAgent::Gain(TInt& aGain) 
	{
	return iRecorder->GetGain(aGain);
	}


/**
Return the maximum value for the gain

@return The maximum gain. For devices where automatic gain control is used, this value may be zero.
*/
EXPORT_C TInt CMVSAudioRecordAgent::MaxGain() const
	{
	return iRecorder->MaxGain();
	}


/**
Returns an array containing the MetaDataEntry for the given audio clip

@param  aMetaArray
        The meta data Array

@leave	Leaves with KErrNotFound if the meta data entry does not exist or
		KErrNotSupported if the controller does not support meta data 
		information for this format. Other errors indicate more general system
		failure.
*/
EXPORT_C void CMVSAudioRecordAgent::GetMetaDataArrayL(RPointerArray<CMMFMetaDataEntry>& aMetaArray)
	{
	//Reset the meta array
    aMetaArray.Reset();
	//Find how many elements there are to obtain
	TInt noMetaEntries = 0;
    TInt err = iRecorder->GetNumberOfMetaDataEntries(noMetaEntries);
    if(err == KErrNone)
	    {
	    //Add the elements, one at a time.
	    for(TInt counter = 0; counter < noMetaEntries; ++counter)
	         {
	         aMetaArray.Append(iRecorder->GetMetaDataEntryL(counter));
	         }	
	    }
	User::LeaveIfError(err);	
	}


/**
Returns the bit rate of the audio clip.

@return The bit rate of the audio clip

@leave  KErrNotSupported
		Leaves with KErrNotSupported error, if no controllers are found
*/
EXPORT_C TUint CMVSAudioRecordAgent::SourceBitRateL()
	{
	return iRecorder->SourceBitRateL();
	}


/**
Returns the maximum volume setting for the audio device.

@return The maximum volume. This value is platform dependent but is always greater than or equal to one.
*/
EXPORT_C TInt CMVSAudioRecordAgent::MaxVolume() 
    {
    return iRecorder->MaxVolume();
    }


/**
Returns an array of the supported number of channels for recording. For example 1 for mono, 2 for stereo and so on.

@param  aNoChannelsArray
        On return, contains an array of the number of channels supported for recording
*/
EXPORT_C void CMVSAudioRecordAgent::GetSupportedNoChannelsArrayL(RArray<TUint>& aNoChannelsArray)
	{
	iRecorder->GetSupportedNumberOfChannelsL(aNoChannelsArray);
	}
	

/**
Returns an array of the supported bit rates for recording to the data sink.

@param  aBitRatesArray
        Array of bit rates supported for the data sink destination.
*/
EXPORT_C void CMVSAudioRecordAgent::GetSupportedBitRatesArrayL(RArray<TUint>& aBitRatesArray)
	{
	iRecorder->GetSupportedBitRatesL(aBitRatesArray);
	}


/**
Returns a list of supported recording sample rates.

@param  aSampleRatesArray
        On return, contains a list of the sample rates that are supported for recording.
*/
EXPORT_C void CMVSAudioRecordAgent::GetSupportedSampleRatesArrayL(RArray<TUint>& aSampleRatesArray)
	{
	iRecorder->GetSupportedSampleRatesL(aSampleRatesArray);
	}


/**
Returns a list of the supported codecs for the conversion destination.

@param  aCodecsArray
        List of four character codes, representing supported data codecs for the conversion destination.
*/
EXPORT_C void CMVSAudioRecordAgent::GetSupportedCodecsArrayL(RArray<TFourCC>& aCodecsArray)
	{
	iRecorder->GetSupportedDestinationDataTypesL(aCodecsArray);
	}
    

/**
Returns the current state of MVSClient. Provided for MVSAppUi to enable/disable the 
menu items depending on the current state of the MVSClient

@return the current state,iState of the client.
*/
EXPORT_C TMVSState CMVSAudioRecordAgent::State()
    {
    return iState;	
    }
	

//from MMdaObjectStateChangeObserver
void CMVSAudioRecordAgent::MoscoStateChangeEvent(CBase* /*aObject*/,
                                TInt aPreviousState, 
                                TInt aCurrentState, 
                                TInt aErrorCode)
	{
	iFileLogger.Write(_L("MoscoStateChangeEvent    PreviousState   CurrentState")) ;
	if(aErrorCode==KErrNone)
		{
		if(aPreviousState==0 && aCurrentState==1)
			{
			iObserver.UpdateStateChange(EAudioOpened ,KErrNone);
			iFileLogger.Write(_L("Audio Opened         0    1  ")) ;	
			}
		if(aPreviousState==1 && aCurrentState==3)
			{
			iObserver.UpdateStateChange(EAudioRecording ,KErrNone);
			iFileLogger.Write(_L("AudioRecording         1    3  ")) ;	
			}
		if(aPreviousState==3 && aCurrentState==1)
			{
			iObserver.UpdateStateChange(EAudioStopped ,KErrNone);
			iFileLogger.Write(_L("AudioStopped          3    1  ")) ;
			}
		if(aPreviousState==1 && aCurrentState==2)
			{
			iObserver.UpdateStateChange(EAudioPlaying, KErrNone);
			iFileLogger.Write(_L("AudioPlaying        1    2  ")) ;
			}
		if(aPreviousState==2 && aCurrentState==1)
			{
			iObserver.UpdateStateChange(EAudioOpened,KErrNone);
			iFileLogger.Write(_L("Audio Opened         2   1  ")) ;
			}
	 	}
	else if(aErrorCode==KErrEof) 	
		{
		if(aPreviousState==3 && aCurrentState==1)
			{
			iObserver.UpdateStateChange(EAudioStopped, aErrorCode);
			iFileLogger.Write(_L("AudioStopped          3    1  ")) ;
			}
		}
	else
		{
		iObserver.UpdateStateChange(ENotReady, aErrorCode);
		iFileLogger.Write(_L("Audio Not Ready")) ;
		}	
	}


/**
Get the current volume

@param	"TInt& aVolume"
         The current volume

@return	"TInt" One of the system wide error codes
*/
EXPORT_C TInt CMVSAudioRecordAgent::GetVolume(TInt& aVolume) 
    {
    return iRecorder->GetVolume(aVolume);
    }


/**
Sets the number of times the audio sample is to be repeated during the
playback operation.

A period of silence can follow each playing of the sample. The audio
sample can be repeated indefinitely.

@param  aNoRepeats
        The number of times the audio sample, together with
        the trailing silence, is to be repeated. If this is
        set to KMdaRepeatForever, then the audio
        sample, together with the trailing silence, is
        repeated indefinitely or until Stop() is
        called. If this is set to zero, then the audio sample
        is not repeated.
@param  aDelay
        The time interval of the training silence.
*/
EXPORT_C void CMVSAudioRecordAgent::SetRepeats(TInt aNoRepeats, TTimeIntervalMicroSeconds aDelay)
    {
    iRecorder->SetRepeats(aNoRepeats,aDelay);
    }    
     

/**
Returns the duration of the audio sample in microseconds.

The function raises a CMdaAudioPlayerUtility 1 panic if the audio
recorder utility is not initialised.

@param aDuration
	   The duration of the sample in microseconds.
@return The duration in microseconds.
*/	
EXPORT_C TTimeIntervalMicroSeconds CMVSAudioRecordAgent::Duration() 
    {
    return iRecorder->Duration();
    }
     

/**
Sets the head position.

The playback head is moved to a position which is defined in terms of a time interval measured from the beginning of the audio sample data. A subsequent call to PlayL() starts from this new position.

@param  aPosition
        The new head position measured in microseconds. 
*/     
EXPORT_C void CMVSAudioRecordAgent::SetPosition(TTimeIntervalMicroSeconds& aPosition)
    {
    iRecorder->SetPosition(aPosition);
    }
        

/**
Returns the current balance setting for the audio device.

@param  aBalance
        The playback balance, between KMMFBalanceMaxLeft and KMMFBalanceMaxRight

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the 
        system-wide error codes.
*/
EXPORT_C TInt CMVSAudioRecordAgent::GetPlayBalance(TInt& aBalance) 
    {
    return iRecorder->GetPlaybackBalance(aBalance);	
    }


/**
Sets the current playback balance.

The balance can be any value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight, the default value being KMMFBalanceCenter.

@param  aBalance
        The playback balance to set.

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the
        system-wide error codes.
*/
EXPORT_C TInt CMVSAudioRecordAgent::SetPlayBalance(TInt aBalance)
    {
    return iRecorder->SetPlaybackBalance(aBalance);	
    }
   

/**
Returns the current recording balance setting between the left and right stereo microphones.

@param  aBalance
        On return, contains the current recording balance between KMMFBalanceMaxLeft and KMMFBalanceMaxRight

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the
        system-wide error codes.
*/
EXPORT_C TInt CMVSAudioRecordAgent::GetRecordBalance(TInt& aBalance) 
    {
    return iRecorder->GetRecordBalance(aBalance);	
    }


/**
Sets the current recording balance setting between the left and right stereo microphones

The microphone balance can be changed before or during recording and is effective immediately.

@param  aBalance
        The balance value to set. Any value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight, the default 
        value being KMMFBalanceCenter

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the
        system-wide error codes.
*/
EXPORT_C TInt CMVSAudioRecordAgent::SetRecordBalance(TInt aBalance)
    {
    return iRecorder->SetRecordBalance(aBalance);
    } 
     
TBool CMVSAudioRecordAgent::EstablishFsSessionL()
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
     
TBool CMVSAudioRecordAgent::EstablishFileSessionL(TDesC& aFile)
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
void CMVSAudioRecordAgent::CloseFsSession()
    {
    iFs.Close(); //Close the Fs Session
    iFsSessionEstablished = EFalse; //Fs Session is no longer established
    return;
    }
     

/*CloseFileSession()

Close the file session. This causes the IsSessionEstablished flag to be set
to false.
*/
void CMVSAudioRecordAgent::CloseFileSession()
    {
    iFile.Close(); //Close the File session
    iFileSessionEstablished = EFalse;  //File session is no longer established
    return;
    }


EXPORT_C void CMVSAudioRecordAgent::SetMaxWriteLength(TInt aMaxWriteLength)
	{
	if(iRecorder)
		{
		if(aMaxWriteLength>0)
			{
			iRecorder->SetMaxWriteLength(aMaxWriteLength);
			}
		else
			{
			iRecorder->SetMaxWriteLength(KMdaClipLocationMaxWriteLengthNone);
			}
		}
	}
	
EXPORT_C const TTimeIntervalMicroSeconds CMVSAudioRecordAgent::RecordTimeAvailable()
	{
	if(iRecorder)
		{
		return iRecorder->RecordTimeAvailable();	
		}
	return TTimeIntervalMicroSeconds(0);
	}
