// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mdasoundadapter.h"
#include "mdasoundadapterbody.h"
#include <e32debug.h>

EXPORT_C RMdaDevSound::RMdaDevSound()
	:iBody(NULL)
	{
	}
		
/*
 @capability MultimediaDD
 
 This function creates the handle to the sound media driver.
 
 @param aUnit	A unit of the device.
 
 @return KErrNone on success, otherwise system wide error code.
 
 @capability MultimediaDD
*/
EXPORT_C TInt RMdaDevSound::Open(TInt aUnit)
	{
	TInt err = KErrNone;
	if(iBody == NULL)
		{
		TRAP(err, iBody = RMdaDevSound::CBody::NewL());
		}
	if(err == KErrNone)
		{
		err = iBody->Open(aUnit);
		}
	return err;
	}

/*
 Gets the version object of sound media driver.

@return version object.

*/
EXPORT_C TVersion RMdaDevSound::VersionRequired() const
	{
	if(iBody)
		{
		return iBody->VersionRequired();
		}
	return TVersion();
	}

/*
 Indicates whether the driver is sound media driver.

@return KErrNone on success, otherwise System wide error code.

*/
EXPORT_C TInt RMdaDevSound::IsMdaSound()
	{
	return iBody->IsMdaSound();
	}

/*
 This function gets the play volume. 
 The range of volume level supported depends on the physical audio device used.

@return Volume level.

*/
EXPORT_C TInt RMdaDevSound::PlayVolume()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	return iBody->PlayVolume();
	}
	
/*
 This function sets the play volume.
 The volume level depends on the physical audio device used.
	
@param aVolume	Play volume level in the range 0 to 255 inclusive
@see TSoundFormatsSupported

*/
EXPORT_C void RMdaDevSound::SetPlayVolume(TInt aVolume)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->SetPlayVolume(aVolume);
	}

/*
 This function sets the play volume.
 The volume level depends on the physical audio device used.
	
@param aVolume	Play volume level. Logarithmic value.
@see TSoundFormatsSupported

*/
EXPORT_C void RMdaDevSound::SetVolume(TInt aLogarithmicVolume)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->SetVolume(aLogarithmicVolume);	
	}
	
/*
 This function cancels the currently playing sound.
 If paused, the pause will be cancelled.
 Will panic if not open.
 Will not cancel Notify*Error().
*/
EXPORT_C void RMdaDevSound::CancelPlayData()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->CancelPlayData();
	}

/*
 Gets the sound record volume.
 This depends on the physical audio device used.
@return Record volume level.

*/
EXPORT_C TInt RMdaDevSound::RecordLevel()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	return iBody->RecordLevel();
	}

/*
 This function sets the device record volume level.
 This depends on the physical audio device used.
@param aLevel Record volume level.	
@see TSoundFormatsSupported

*/
EXPORT_C void RMdaDevSound::SetRecordLevel(TInt aLevel)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->SetRecordLevel(aLevel);
	}

/*
 This function cancels the recording in progress.
 If paused, the pause will be cancelled.
 Any buffered data will be discarded.
 Will panic if not open.
 Will not cancel Notify*Error().
*/
EXPORT_C void RMdaDevSound::CancelRecordData()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->CancelRecordData();
	}

/*
 This function stops recording and completes the outstanding RecordData request immediately with any available data.
 Any following RecordData calls will be completed immediately returning any buffered data, they will NOT restart recording.

 It  maybe called either when recording or stopped.

 The flushing state should be exited by calling CancelRecordData.
 
 The adaptor implements this functionality via Pause, which results in slightly different behaviour to the old RMdaDevSound driver.
 In particular the flushing state can also be exited by calling ResumeRecording, do NOT do  this... If you want this behaviour, use the
 new PauseRecording/ResumeRecording functions.
 */
EXPORT_C void RMdaDevSound::FlushRecordBuffer()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->FlushRecordBuffer();
	}

/*
 This function returns the number of bytes played by the driver since calling Open or 
 calling ResetBytesPlayed().

 It is not reset by PlayData or PausePlayBuffer/ResumePlayBuffer

@see RMdaDevSound::ResetBytesPlayed() 
@return Number of bytes played.
*/
EXPORT_C TInt RMdaDevSound::BytesPlayed()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	return iBody->BytesPlayed();
	}

/*
 Resets the count of bytes played.

 If called whilst playing, the counter might not reset to exactly zero, it will reset to the number of bytes played in the current
 internal transfer.
*/
EXPORT_C void RMdaDevSound::ResetBytesPlayed()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	return iBody->ResetBytesPlayed();
	}

/*
 This function changes the audio play state to pause.
 It is legal to pause whilst not playing, in which case a following (single) PlayData request will be queued until
 ResumePlaying is called.
*/
EXPORT_C void RMdaDevSound::PausePlayBuffer()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->PausePlayBuffer();
	}

	
/*
 This function starts the audio play from pause state.
 If a PlaData request was active when the Pause was requested it will continue.
 If a PlayData request was not active when the Pause was requested, but a one was issued whilst paused,
 it will start.
 If there is nothing to resume, we will notify KErrUnderflow.
*/
EXPORT_C void RMdaDevSound::ResumePlaying()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->ResumePlaying();
	}

/*
 This function is identical to RMdaDevSound::ResumePlaying(), the parameter is ignored.
*/
EXPORT_C void RMdaDevSound::ResumePlaying(TRequestStatus&)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->ResumePlaying();
	}

/*
The current record request will be completed early with partial contents and further
recording paused.

Any following RecordData calls will be completed immediately using any buffered data, it will NOT restart recording.

*/
EXPORT_C void RMdaDevSound::PauseRecordBuffer()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->PauseRecordBuffer();
	}

/*
	Resume recording.
	Recorded data will be buffered internally.
	If an outstanding RecordData request was issued whilst paused it will be processed.
*/
EXPORT_C void RMdaDevSound::ResumeRecording()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->ResumeRecording();
	}

/*
	Return the duration of the audio data which has been played.
	Note that this may be less than the amount of data/time queued.
*/
EXPORT_C TInt RMdaDevSound::GetTimePlayed(TTimeIntervalMicroSeconds& aTimePlayed)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	return iBody->GetTimePlayed(aTimePlayed);
	}


/*
 Gets the play format(capability) supported by this device. 
 This record describes supported sample rate, encoding, volume level, channels, buffer size of the audio for playing. 

@param  aFormatsSupported	A reference to a client supplied TSoundFormatsSupported class to be filled by this function. 
@see TSoundFormatsSupported

*/
EXPORT_C void RMdaDevSound::PlayFormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->PlayFormatsSupported(aFormatsSupported);
	}

/*
 This function gets the current play format.

@param  aFormat	A reference to a client supplied TCurrentSoundFormat class to be filled by this function. 
@see TCurrentSoundFormat

*/
EXPORT_C void RMdaDevSound::GetPlayFormat(TCurrentSoundFormatBuf& aFormat)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->GetPlayFormat(aFormat);
	}

/*
 This functions sets the play format.

@param aFormat For the details refer to TCurrentSoundFormat. 

@see TCurrentSoundFormat

@return KErrNone on success,
		KErrInUse if playing, 
        KErrAccessDenied if play and recording sample rate is different,
        KErrNotSupported if input sound format does not match with supported capability,
        otherwise system wide error code.

*/	
EXPORT_C TInt RMdaDevSound::SetPlayFormat(const TCurrentSoundFormatBuf& aFormat)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	return iBody->SetPlayFormat(aFormat);
	}

/*
 Gets the sound record format. 
 This record describes supported sample rate, encoding, volume level, buffer size of the audio for recording.
 This depends on the physical device used.

@param  aFormatsSupported	A reference to a client supplied TSoundFormatsSupported class to be filled by this function.  
@see TSoundFormatsSupported

*/
EXPORT_C void RMdaDevSound::RecordFormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->RecordFormatsSupported(aFormatsSupported);
	}

/*
 Gets a current sound format used for recording.
 
@param aFormat	A reference to a client supplied TCurrentSoundFormat class to be filled by this function.
@see TCurrentSoundFormat

*/
EXPORT_C void RMdaDevSound::GetRecordFormat(TCurrentSoundFormatBuf& aFormat)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->GetRecordFormat(aFormat);
	}

/*
 Call this function to change the sound format used for recording.

@param aFormat	For details refer to TCurrentSoundFormat. 
@see TCurrentSoundFormat

@return KErrNone on sucess,
        KErrInUse  if recording already in progress,
        KErrAccessDenied   play and record sample rates are different,
        otherwise system wide error code.

*/
EXPORT_C TInt RMdaDevSound::SetRecordFormat(const TCurrentSoundFormatBuf& aFormat)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	return iBody->SetRecordFormat(aFormat);
	}
	
EXPORT_C void RMdaDevSound::Close()
	{
	if(iBody)
		{
		iBody->Close();
		delete iBody;
		iBody = NULL;
		}
	}
		
EXPORT_C TInt RMdaDevSound::Handle()
	{
	if(iBody)
		{
		return iBody->Handle();
		}
	return 0;
	}

/*
 Call this function to play the audio data in the supplied descriptor.

Only a single request may be outstanding at any point in time.

If paused, the request will be queued until ResumePlaying is called.

@param  aStatus	For details refer to TRequestStatus. 
@see TRequestStatus

@param	aData	Descriptor with play data

*/
EXPORT_C void RMdaDevSound::PlayData(TRequestStatus& aStatus, const TDesC8& aData)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->PlayData(aStatus, aData);
	}

/*
 Records audio data into the supplied descriptor.

Only a single request may be outstanding at any point in time.

If paused, the request will be queued until ResumeRecording is called.

@param  aStatus	Request status
@see TRequestStatus

@param  aData	Record buffer descriptor.

*/
EXPORT_C void RMdaDevSound::RecordData(TRequestStatus& aStatus, TDes8& aData)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->RecordData(aStatus, aData);
	}

/*
 Call this function to notify any error encountered while recording audio.

@param  aStatus	request object's completion code value 
@see TRequestStatus

*/
EXPORT_C void RMdaDevSound::NotifyRecordError(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->NotifyRecordError(aStatus);
	}

/*
 Call this function to notify the play error encountered while playing the sound.

@param aStatus	Error code stating the cause for the play error.

*/
EXPORT_C void RMdaDevSound::NotifyPlayError(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->NotifyPlayError(aStatus);
	}

/*
 This function cancels the play notification error.

*/
EXPORT_C void RMdaDevSound::CancelNotifyPlayError()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->CancelNotifyPlayError();
	}

/*
 This function cancels the recording error notification.
*/
EXPORT_C void RMdaDevSound::CancelNotifyRecordError()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->CancelNotifyRecordError();
	}

/*
This function cancels the currently playing sound.
If paused, the pause will be cancelled.

 This function is identical to CancelPlayData
*/
EXPORT_C void RMdaDevSound::FlushPlayBuffer()
	{
	__ASSERT_DEBUG(iBody != NULL, Panic(EDeviceNotOpened));
	iBody->FlushPlayBuffer();
	}
