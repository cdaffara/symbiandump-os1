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

#include "audioplayagent.h"
#include <e32def.h>
#include <flogger.h>


/**
Constructs and initialises a new instance of the MVS audio player utility.

The function leaves if the MVS audio player utility object cannot be created.

No callback notification is made upon completion of NewL().

@param  aObserver
		Class to receive state change events from play agent.

@return     A pointer to the new MVS audio player utility object.
*/

EXPORT_C CMVSAudioPlayAgent* CMVSAudioPlayAgent::NewL(MMVSClientObserver& aObserver)
     {
     CMVSAudioPlayAgent* self = new(ELeave) CMVSAudioPlayAgent(aObserver);
     CleanupStack::PushL(self);
     self->ConstructL();
     CleanupStack::Pop(self);
     return self;
     }


void CMVSAudioPlayAgent::ConstructL()
     {
     iPlayer = CMdaAudioPlayerUtility::NewL(*this);
     User::LeaveIfError(iFileLogger.Connect());
     iFileLogger.CreateLog(_L("LogMVSappUi"),_L("LogFile.txt"),EFileLoggingModeAppend);
     }


EXPORT_C CMVSAudioPlayAgent::~CMVSAudioPlayAgent()
     {
     delete iPlayer;
     if(iFileLogger.Handle())
     	{
     	iFileLogger.CloseLog();
	 	iFileLogger.Close();
     	}
     }


CMVSAudioPlayAgent::CMVSAudioPlayAgent(MMVSClientObserver& aObserver):iObserver(aObserver)
     {
     iObserver.UpdateStateChange(ENotReady, KErrNone);
     }

EXPORT_C void CMVSAudioPlayAgent::OpenFileL(TDesC& aFile)
     {
     iPlayer->Close();//Close any existing clip
     iState = EAudioOpening;
     iObserver.UpdateStateChange(EAudioOpening, KErrNone);
     iPlayer->OpenFileL(aFile);
     }

EXPORT_C void CMVSAudioPlayAgent::OpenFileL(TMMSource& /*aSource*/)
     {
     
     }

/**
Begins playback of the initialised audio sample at the current volume
and priority levels.

Sets the current state of the system to EAudioPlaying
When playing of the audio sample is complete, successfully or
otherwise, the callback function
CMVSAudioPlayAgent::MapcPlayComplete() is called, which inturn
sets the state to EAudioOpened. 
*/
EXPORT_C void CMVSAudioPlayAgent::Play()
     {
     //must be in open or paused states 
     if((iState == EAudioOpened)||(iState == EAudioPaused)||(iState == EAudioStopped))
     	{
     	iPlayer->Play();
     	iState = EAudioPlaying;
     	iObserver.UpdateStateChange(EAudioPlaying, KErrNone);	
     	}
     }


/**
Pauses the playback of the audio clip.
and updates the current state to EAudioPaused.
*/
EXPORT_C TInt CMVSAudioPlayAgent::Pause()
     {
     if(iState == EAudioPlaying)
     	{
     	TInt err = iPlayer->Pause();
     	if ( err != KErrNone)
     		{
     		return err;
     		}
     	iState = EAudioPaused;
     	iObserver.UpdateStateChange(EAudioPaused, KErrNone);
     	}
     return KErrNone;	
     }


/**
Stops playback of the audio sample as soon as possible.

Sets the current state to EAudioStopped
If the audio sample is playing, playback is stopped as soon as
possible. If playback is already complete, nothing further happens as
a result of calling this function.  
*/
EXPORT_C void CMVSAudioPlayAgent::Stop()
	{
    if(iState == EAudioPlaying || iState == EAudioPaused)
    	{
     	iPlayer->Stop();
     	iState = EAudioStopped;
     	iObserver.UpdateStateChange(iState, KErrNone);	
     	}
    }     

/**
Added for future implimentation. Currently not supported
*/
EXPORT_C void CMVSAudioPlayAgent::Forward()
    {
    // future implementation	
    }  
     

/**
Added for future implimentation. Currently not supported
*/
EXPORT_C void CMVSAudioPlayAgent::Rewind()
    {
    // future implementation
    }  

     
/*
Initialisation completion callback for the MVS play agent
*/
void CMVSAudioPlayAgent::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    if(aError == KErrNone)
    	{
    	iState = EAudioOpened;
     	iFileLogger.Write(_L("Audio Opened ")) ;
       	}
    else
     	{
     	iState = ENotReady; //init failed so from opening to NotReady
     	iFileLogger.Write(_L("MapcInitComplete: Audio Not Ready")) ;
     	}
    iObserver.UpdateStateChange(iState, aError);
    }


/*
Play back completion callback for the MVS play agent
*/
void CMVSAudioPlayAgent::MapcPlayComplete(TInt aError)
	{
	if(aError == KErrNone)
		{
		iState = EAudioOpened;
		iFileLogger.Write(_L("MapcPlayComplete:Audio Opened Play Complete")) ;
		}
	
	iObserver.UpdateStateChange(iState, aError);
	}

void CMVSAudioPlayAgent::MarncResourceAvailable(TUid /*aNotificationEventId*/, const TDesC8& /*aNotificationData*/)
	{
	}


/**
Returns the current playback volume

@param aVolume
       A volume value between 0 and the value returned by MaxVolume().

@return One of the global error codes.
*/
EXPORT_C TInt CMVSAudioPlayAgent::GetVolume(TInt& aVolume) 
    {
    return iPlayer->GetVolume(aVolume);
    }


/**
Returns an integer representing the maximum volume.

This is the maximum value which can be passed to
CMdaAudioPlayerUtility::SetVolume(). This value is platform 
independent, but is always greater than or equal to one.

The function raises a CMdaAudioPlayerUtility 1 panic if the 
audio player utility is not initialised.

@return The maximum volume setting.
*/
EXPORT_C TInt CMVSAudioPlayAgent::MaxVolume() 
    {
    return iPlayer->MaxVolume();
    }
    

/**
Returns the duration of the audio sample.

The function raises a CMdaAudioPlayerUtility 1 panic if the audio
player utility is not initialised.

@return The duration in microseconds.
*/  
EXPORT_C TTimeIntervalMicroSeconds CMVSAudioPlayAgent::Duration() 
    {
    return iPlayer->Duration();
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

@param  aRampDuration
        The period over which the volume is to rise. A zero
        value causes the audio sample to be played at the
        normal level for the full duration of the playback. A
        value which is longer than the duration of the audio
        sample means that the sample never reaches its normal
        volume level.

*/
EXPORT_C void CMVSAudioPlayAgent::SetVolume(TInt aVolume, TTimeIntervalMicroSeconds aRamp)
    {
    iPlayer->SetVolume(aVolume);
    iPlayer->SetVolumeRamp(aRamp);
    }


/**
Returns the current playback position in microseconds from the start of the clip.

@param   aPosition
         The current time position in microseconds from the start of the clip to the current
         play position.

@return the current playback position in microseconds.
*/    
EXPORT_C TTimeIntervalMicroSeconds CMVSAudioPlayAgent::GetPosition(TTimeIntervalMicroSeconds& aPosition) 
    {
    return iPlayer->GetPosition(aPosition);
    }
     
     
/**
Sets the current playback position in microseconds from the start of the clip.

@param  aPosition
        The position to move to in microseconds from the start of the clip.
*/
EXPORT_C void CMVSAudioPlayAgent::SetPosition(TTimeIntervalMicroSeconds aPosition)
    {
    iPlayer->SetPosition(aPosition);
    }
    

/**
Returns the current playback balance.

@param  aBalance
        A value between KMMFBalanceMaxLeft
        and KMMFBalanceMaxRight.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMVSAudioPlayAgent::GetBalance(TInt& aBalance) 
    {
    return iPlayer->GetBalance(aBalance);
    }


/**
Sets the current playback balance.

@param  aBalance
        A value between KMMFBalanceMaxLeft
        and KMMFBalanceMaxRight. The default value is
        KMMFBalanceCenter.

@return "TInt" An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMVSAudioPlayAgent::SetBalance(TInt aBalance)
    {
    return iPlayer->SetBalance(aBalance);
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
EXPORT_C void CMVSAudioPlayAgent::SetRepeats(TInt aNoRepeats, TTimeIntervalMicroSeconds aDelay)
    {
    iPlayer->SetRepeats(aNoRepeats, aDelay);
    }


/**
Set the current playback window

@param	aStart
		Start time of playback window relative to start of file
@param	aEnd
		End time of playback window relative to start of file

@return "TInt" One of the global error codes
*/
EXPORT_C TInt CMVSAudioPlayAgent::SetPlayWindow(TTimeIntervalMicroSeconds aStart, TTimeIntervalMicroSeconds aEnd)
    {
    return iPlayer->SetPlayWindow(aStart,aEnd);
    }


/**
Clear the current playback window

@return "TInt" One of the global error codes
*/   
EXPORT_C TInt CMVSAudioPlayAgent::ClearPlayWindow()
    {
    return iPlayer->ClearPlayWindow();
    }     


/**
Sets the priority for playback. This is used to arbitrate between multiple
objects trying to access a single sound device.

@param  aPriority
        The priority level to apply, EMdaPriorityMin client can be interrupted by any
        other client, EMdaPriorityNormal client can only be interrupted by a client 
        with a higher priority or EMdaPriorityMax client cannot be interrupted by other 
        clients.
@param  aPreference
        The quality/time preferences to apply.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMVSAudioPlayAgent::SetPriority(TInt aPriority, TMdaPriorityPreference aPreference)
    {
    return iPlayer->SetPriority(aPriority,aPreference);
    }


/**
Closes the current audio clip (allowing another clip to be opened)
Sets the current state to ENotReady
*/  
EXPORT_C void CMVSAudioPlayAgent::Reset()
    {
    iPlayer->Close();
    iState = ENotReady;
    iObserver.UpdateStateChange(ENotReady, KErrNone);
    }

     
EXPORT_C void CMVSAudioPlayAgent::SetAutoPauseResume(TBool /*aEnable*/)
    {
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
EXPORT_C void CMVSAudioPlayAgent::GetMetaArrayL(RPointerArray<CMMFMetaDataEntry>& aMetaArray)
	{
	//Reset the meta array
    aMetaArray.Reset();
	//Find how many elements there are to obtain
	TInt noMetaEntries = 0;
    TInt err = iPlayer->GetNumberOfMetaDataEntries(noMetaEntries);
    if(err == KErrNone)
	    {
	    //Add the elements, one at a time.
	    for(TInt counter = 0; counter < noMetaEntries; ++counter)
	         {
	         aMetaArray.Append(iPlayer->GetMetaDataEntryL(counter));
	         }	
	    }
	User::LeaveIfError(err);	
	}
     
	
/**
Returns the bit rate of the audio clip.

@param  aBitRate
        Bit rate of the audio clip.

@return One of the global error codes.
*/
EXPORT_C TInt CMVSAudioPlayAgent::GetBitRate(TUint& aBitRate)
	{
	return iPlayer->GetBitRate(aBitRate);
	}


/**
Returns the controller implementation information associated with the current controller.

@return The controller implementation structure
*/
EXPORT_C const CMMFControllerImplementationInformation& CMVSAudioPlayAgent::GetControllerInfoL()
	{
	return iPlayer->ControllerImplementationInformationL();	
	}


/**
Returns the current state of the CMVSAudioPlayAgent.

@return The current state, iState.
*/
EXPORT_C TMVSState CMVSAudioPlayAgent::GetState()
	{
	return iState;	
	}
