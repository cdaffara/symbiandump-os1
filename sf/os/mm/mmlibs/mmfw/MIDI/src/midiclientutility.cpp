// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <midiclientutility.h>
#include "midiclientutilitybody.h"


/**
Static factory function for creating a MIDI client utility object.
This function is synchronous, unlike the other factory functions,
because it doesn't need to perform any MIDI resource initialisation.

@param aObserver
       Reference to an object to receive callbacks on completion of asynchronous functions.
@param aPriority
       The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
       EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param aPref
       The Priority Preference - an additional audio policy parameter. The suggested default is 
       EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
       values may be supported by given phones and/or platforms, but should not be depended upon by 
       portable code.
@param aUseSharedHeap
	   Select if the underlying controller will have its own heap or share a single heap with other
	   controller instances.
	   The default behaviour, or if this value is EFalse, is that each controller is created with
	   its own heap. The alternative, if the value is ETrue, is that controllers share a special
	   heap with other controllers created the same way. Each heap uses a chunk, so this avoids
	   situations where the number of chunks per process is limited. The default behaviour is
	   generally to be preferred, and should give lower overall memory usage. However, if many
	   controllers are to be created for a particular thread, then ETrue should be provided to
	   prevent running out of heaps or chunks.
@return Fully constructed utility object ready to have an OpenXxxx() function called.

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CMidiClientUtility* CMidiClientUtility::NewL(MMidiClientUtilityObserver& aObserver, TInt aPriority, TInt aPref, TBool aUseSharedHeap)
	{
	CMidiClientUtility* self = new(ELeave) CMidiClientUtility();
	CleanupStack::PushL(self);
	self->iBody = CMidiClientUtility::CBody::NewL(self, aObserver, aPriority, aPref, aUseSharedHeap );
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Static factory function for creating a MIDI client utility object.
This function is synchronous, unlike the other factory functions,
because it doesn't need to perform any MIDI resource initialisation
The underlying controller that is created will be given its own heap.

@param aObserver
       Reference to an object to receive callbacks on completion of asynchronous functions.
@param aPriority
       The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
       EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param aPref
       The Priority Preference - an additional audio policy parameter. The suggested default is 
       EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
       values may be supported by given phones and/or platforms, but should not be depended upon by 
       portable code.
@return Fully constructed utility object ready to have an OpenXxxx() function called.

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CMidiClientUtility* CMidiClientUtility::NewL(MMidiClientUtilityObserver& aObserver, TInt aPriority, TInt aPref)
	{
	return NewL( aObserver, aPriority, aPref, EFalse );
	}

CMidiClientUtility::~CMidiClientUtility()
	{
	delete iBody;
	}

/**
Asynchronous function to open a file containing MIDI data and perform
initialisation ready for playback

@param aFileName Name of the MIDI file to open
*/
EXPORT_C void CMidiClientUtility::OpenFile(const TDesC& aFileName)
	{
	iBody->OpenFile(aFileName);
	}
	
/**
Asynchronous function to open a file containing MIDI data and perform
initialisation ready for playback

@param aFile Open shared protected session handle to the midi file to read
*/
EXPORT_C void CMidiClientUtility::OpenFile(const RFile& aFile)
	{
	iBody->OpenFile(const_cast<RFile&>(aFile));
	}

/**
Asynchronous function to open a file containing MIDI data and perform
initialisation ready for playback

@param aFileSource TFileSource object which references either a filename or a
file handle to the midi file to read
*/
EXPORT_C void CMidiClientUtility::OpenFile(const TMMSource& aSource)
	{
	iBody->OpenFile(aSource);
	}

/**
Asynchronous function to open a descriptor containing MIDI data and perform
initialisation ready for playback

@param aDescriptor descriptor containing MIDI data
*/
EXPORT_C void CMidiClientUtility::OpenDes(const TDesC8& aDescriptor)
	{
	iBody->OpenDes(aDescriptor);
	}

/**
Asynchronous function to open a URL containing MIDI data and perform
initialisation ready for playback

@param  aUrl
        Uniform Resource Locator for a MIDI data stream
@param  aIapId
        Identifier of the Internet Access Point to use -
        available from CommDB, the comms connections database.
        Defaults to using the default access point, as defined by CommDB
@param  aMimeType
        Mime type of the MIDI data stream to be played.
        Defaults to nothing in which case the an attempt will be made to recognise the type of the MIDI data automatically.
*/
EXPORT_C void CMidiClientUtility::OpenUrl(const TDesC& aUrl,TInt aIapId,const TDesC8& aMimeType)
	{
	iBody->OpenUrl(aUrl, aIapId, aMimeType);
	}

/**
Asynchronous function to initiate or resume playback of a previously opened resource.
Also used to start an internal timer to establish a zero-time for the media stream
time relative to which commands with timestamps are timed against
*/
EXPORT_C void CMidiClientUtility::Play()
	{
	iBody->Play();
	}

/**
Stops playback of a resource but does not change the current position or release any resources.
Pauses the internal timer if no resource is open

@param aFadeOutDuration
       Length of time over which the volume is faded out from the current settings to zero.
*/
EXPORT_C void CMidiClientUtility::Stop(const TTimeIntervalMicroSeconds& aFadeOutDuration)
	{
	iBody->Stop(aFadeOutDuration);
	}

/**
Asynchronous function which closes any currently open resources, such as files, descriptors or URLs in use.
Does nothing if there is nothing currently open.
*/
EXPORT_C void CMidiClientUtility::Close()
	{
	iBody->Close();
	}

/**
Gets the current state of the MIDI client utility with regard to MIDI resources

@return The current state of the utility
*/
EXPORT_C TMidiState CMidiClientUtility::State() const
	{
	return iBody->State();
	}

/**
Synchronous function to play a single note.
Multiple calls to this function will be accommodated as far as the MIDI engine can
manage. The same functionality could be implemented using the SendMessage function

@param aChannel 
       Logical channel to play note on. 0 <= aChannel <= 15.
@param aNote 
       Note to play. 0 <= aNote <= 127
@param aDuration 
       Length of time to play note for.
@param aNoteOnVelocity 
       Velocity with which to start the note. 0 <= aNoteOnVelocity <= 127.
@param aNoteOffVelocity 
       Velocity with which to stop the note. 0 <= aNoteOffVelocity <= 127.
*/
EXPORT_C void CMidiClientUtility::PlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity)
	{
	iBody->PlayNoteL(aChannel, aNote, aDuration, aNoteOnVelocity, aNoteOffVelocity);
	}

/**
Synchronous function to play a single note at a specified time.
Multiple calls to this function will be accommodated as far as the MIDI engine can
manage. The same functionality could be implemented using the SendMessage function

@param aChannel 
       Logical channel to play note on. 0 <= aChannel <= 15.
@param aNote 
       Note to play. 0 <= aNote <= 127
@param aStartTime 
       specifies the time at which to start playing the note,
       relative to the MIDI resource playing time or the time elapsed since Play() was called if no resource is present
@param aDuration
       Length of time to play note for.
@param aNoteOnVelocity 
       Velocity with which to start the note. 0 <= aNoteOnVelocity <= 127.
@param aNoteOffVelocity
       Velocity with which to stop the note. 0 <= aNoteOffVelocity <= 127.
*/
EXPORT_C void CMidiClientUtility::PlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aStartTime,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity)
	{
	iBody->PlayNoteL(aChannel, aNote, aStartTime, aDuration, aNoteOnVelocity, aNoteOffVelocity);
	}

/**
Stops the playback of all notes on the given channel,
by means of an All Notes Off MIDI message

@param aChannel 
       Logical channel to stop notes on. 0 <= aChannel <= 15
*/
EXPORT_C void CMidiClientUtility::StopNotes(TInt aChannel)
	{
	iBody->StopNotes(aChannel);
	}

/**
Synchronous function to commence playback of a note.
Multiple calls to this function will be accommodated as far as the MIDI engine can manage

@param aChannel 
       Logical channel to play note on. 0 <= aChannel <= 15
@param aNote 
       Note to play. 0 <= aNote <= 127
@param aVelocity 
       Velocity with which to start the note.
       The legal integer range is 0 <= aVelocity <= 127, but the value zero
       actually causes the message to be interpreted as a Note Off message
       instead of a Note On.
*/
EXPORT_C void CMidiClientUtility::NoteOnL(TInt aChannel,TInt aNote,TInt aVelocity)
	{
	iBody->NoteOnL(aChannel, aNote, aVelocity);
	}

/**
Synchronous function to terminate playback of a note. If no corresponding note
is found then no error is raised.

@param aChannel 
       Logical channel on which the note is playing. 0 <= aChannel <= 15.
@param aNote 
       Note to terminate. 0 <= aNote <= 127.
@param aVelocity 
       Velocity with which to stop the note. 0 <= aVelocity <= 127. There is no 
	   standard behaviour corresponding with note off velocity.
*/
EXPORT_C void CMidiClientUtility::NoteOffL(TInt aChannel,TInt aNote,TInt aVelocity)
	{
	iBody->NoteOffL(aChannel, aNote, aVelocity);
	}

/**
Gets the current playback rate factor of the currently open MIDI resource.
The playback rate is independent from tempo,
i.e., it can be used to give an overall speed factor for playback

@return Current playback rate in percent times 1000,
        i.e., 100000 means original playback speed, 200000 means double speed,
        and 50000 means half speed playback
*/
EXPORT_C TInt CMidiClientUtility::PlaybackRateL() const
	{
	return iBody->PlaybackRateL();
	}

/**
Sets the playback rate for the playback of the current MIDI resource.
The playback rate is independent from tempo,
i.e., it can be used to give an overall speed factor for playback.
May be called whether playback is in progress or not.

@param aRate 
       Playback rate in percent times 1000,
       i.e., 100000 means original playback speed, 200000 means double speed,
       and 50000 means half speed playback
*/
EXPORT_C void CMidiClientUtility::SetPlaybackRateL(TInt aRate)
	{
	iBody->SetPlaybackRateL(aRate);
	}

/**
Gets the maximum playback rate in milli-percentage from the MIDI engine.
@see SetPlaybackRate() for milli-percentage details

@return Maximum playback rate supported by MIDI player
*/
EXPORT_C TInt CMidiClientUtility::MaxPlaybackRateL() const
	{
	return iBody->MaxPlaybackRateL();
	}

/**
Gets the minimum playback rate in milli-percentage from the MIDI engine.
@see SetPlaybackRate() for milli-percentage details.

@return Minimum playback rate supported by MIDI player.
*/
EXPORT_C TInt CMidiClientUtility::MinPlaybackRateL() const
	{
	return iBody->MinPlaybackRateL();
	}

/**
Gets the current tempo of the currently open MIDI resource. The tempo is independent
from the playback rate, i.e., the resulting playback speed will be affected by both.

@return Tempo at the current position of the currently open resource in microbeats per minute,
        i.e. BPM * 1000000. Filled in by the controller framework
*/
EXPORT_C TInt CMidiClientUtility::TempoMicroBeatsPerMinuteL() const
	{
	return iBody->TempoMicroBeatsPerMinuteL();
	}

/**
Sets the tempo at which the current MIDI resource should be played.
May be called whether playback is in progress or not.
The tempo is independent from the playback rate,
i.e., the resulting playback speed will be affected by both

@param aMicroBeatsPerMinute 
       Tempo in microbeats per minute (BPM*1000000) to set
*/
EXPORT_C void CMidiClientUtility::SetTempoL(TInt aMicroBeatsPerMinute)
	{
	iBody->SetTempoL(aMicroBeatsPerMinute);
	}

/**
Gets the pitch shift in use for the currently open MIDI resource

@return Pitch shift in cents, i.e. semitones * 100. One octave equals 1200 cents
*/
EXPORT_C TInt CMidiClientUtility::PitchTranspositionCentsL() const
	{
	return iBody->PitchTranspositionCentsL();
	}

/**
Sets the pitch shift to apply to the currently open MIDI resource.
May be called during playback
aCents parameter is not checked - if the value is out of range, it is expected KErrArgument is return by MIDI engine.

@param aCents 
       Pitch shift in cents, i.e. semitones * 100. One octave equals 1200 cents
@return Actual pitch shift applied -
        may differ from the requested value due to limitations of the MIDI engine
*/
EXPORT_C TInt CMidiClientUtility::SetPitchTranspositionL(TInt aCents)
	{
	return iBody->SetPitchTranspositionL(aCents);
	}

/**
Gets the length of the currently open MIDI resource in micro-seconds

@return Duration in microseconds (seconds * 1000000).
*/
EXPORT_C TTimeIntervalMicroSeconds CMidiClientUtility::DurationMicroSecondsL() const
	{
	return iBody->DurationMicroSecondsL();
	}

/**
Gets the length of the currently open MIDI resource in micro-beats

@return Duration in microbeats (beats * 1000000).
*/
EXPORT_C TInt64 CMidiClientUtility::DurationMicroBeatsL() const
	{
	return iBody->DurationMicroBeatsL();
	}

/**
Gets the number of tracks present in the currently open MIDI resource

@return Number of tracks
*/
EXPORT_C TInt CMidiClientUtility::NumTracksL() const
	{
	return iBody->NumTracksL();
	}

/**
Mutes or unmutes a particular track

@param aTrack
       Index of the track to mute - 0 <= aTrack < NumTracksL().
@param aMuted
       ETrue to mute the track, EFalse to unmute it.
*/
EXPORT_C void CMidiClientUtility::SetTrackMuteL(TInt aTrack,TBool aMuted) const
	{
	iBody->SetTrackMuteL(aTrack, aMuted);
	}


/**
Gets the MIME type of the MIDI resource currently open

@return Descriptor containing the MIDI mime type
*/
EXPORT_C const TDesC8& CMidiClientUtility::MimeTypeL()
	{
	return iBody->MimeTypeL();
	}

/**
Gets the current temporal position of the MIDI resource being played.

@return Microseconds relative to the start of the resource
*/
EXPORT_C TTimeIntervalMicroSeconds CMidiClientUtility::PositionMicroSecondsL() const
	{
	return iBody->PositionMicroSecondsL();
	}

/**
Change the position of the currently playing MIDI resource to the given position.
May be called whenever a MIDI resource is open

@param aPosition
       Temporal position to move to. Clamped to (0, DurationMicroSecondsL()).
*/
EXPORT_C void CMidiClientUtility::SetPositionMicroSecondsL(const TTimeIntervalMicroSeconds& aPosition)
	{
	iBody->SetPositionMicroSecondsL(aPosition);
	}

/**
Gets the current metrical position of the MIDI resource being played

@return Microbeats (BPM*1000000) relative to the start of the resource
*/
EXPORT_C TInt64 CMidiClientUtility::PositionMicroBeatsL() const
	{
	return iBody->PositionMicroBeatsL();
	}

/**
Change the position of the currently playing MIDI resource to the given position.
May be called whenever a MIDI resource is open.

@param aMicroBeats 
       Metrical position to move to. Clamped to (0, DurationMicroBeatsL()).
*/
EXPORT_C void CMidiClientUtility::SetPositionMicroBeatsL(TInt64 aMicroBeats)
	{
	iBody->SetPositionMicroBeatsL(aMicroBeats);
	}

/**
Sets the frequency at which MMIDIClientUtilityObserver::MmcuoSyncUpdateL(…) is called
to allow other components to synchronise with playback of this MIDI resource

@param aMicroSeconds 
       Temporal interval to callback at. Used in preference to aMicroBeats if both are set
@param aMicroBeats 
       Metrical interval to callback at. Set both parameters to zero to cancel.
*/
EXPORT_C void CMidiClientUtility::SetSyncUpdateCallbackIntervalL(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats)
	{
	iBody->SetSyncUpdateCallbackIntervalL(aMicroSeconds, aMicroBeats);
	}

/**
Sends a single MIDI message to the MIDI engine

@param aMidiMessage 
       Descriptor containing the MIDI message data.
       If there are several MIDI messages in the buffer, only the first one is processed
*/
EXPORT_C TInt CMidiClientUtility::SendMessageL(const TDesC8& aMidiMessage)
	{
	return iBody->SendMessageL(aMidiMessage);
	}

/**
Sends a single MIDI message, with time stamp, to the MIDI engine

@param aMidiMessage 
       Descriptor containing the MIDI message data.
       If there are several MIDI messages in the buffer, only the first one is processed
@param aTime 
       The time at which to execute the message, relative to the MIDI resource playing
       time or the time elapsed since Play() was called if no resource is present
*/
EXPORT_C TInt CMidiClientUtility::SendMessageL(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& aTime)
	{
	return iBody->SendMessageL(aMidiMessage, aTime);
	}

/**
Sends a mip message to the MIDI engine. This is a convenience function,
because the same functionality could be achieved with the SendMessage() function

@param aEntry 
       Array of logical {channel, MIP} value pairs to send, highest priority first
*/
EXPORT_C void CMidiClientUtility::SendMipMessageL(const RArray<TMipMessageEntry>& aEntry)
	{
	iBody->SendMipMessageL(aEntry);
	}

/**
Gets the number of standard or custom sound banks currently available

@param aCustom 
       Specifies whether to reference a custom or standard sound bank
@return Number of custom or standard sound banks available
*/
EXPORT_C TInt CMidiClientUtility::NumberOfBanksL(TBool aCustom) const
	{
	return iBody->NumberOfBanksL(aCustom);
	}

/**
Gets the identifier of a sound bank. Bank identifier (aka bank number) is a
14-bit value consisting of MIDI bank MSB and LSB values

@param  aCustom 
        Specifies whether to reference a custom or standard sound bank
@param  aBankIndex
        Index of sound bank where 0 <= aBankIndex < NumberOfBanksL(…)
@return Identifier of the specified bank occupying, at most, 14 bits
*/
EXPORT_C TInt CMidiClientUtility::GetBankIdL(TBool aCustom, TInt aBankIndex) const
	{
	return iBody->GetBankIdL(aCustom, aBankIndex);
	}

/**
Loads one or more custom sound banks from a file into memory for use.
If several banks are loaded with consequent LoadCustomBanksL() function calls,
the banks are combined if the bank sets have colliding bank numbers

@param aFileName 
       Name of the file containing the custom sound bank
@param aBankCollectionIndex 
       Identifier of the custom sound bank loaded, occupying no more than 14 bits
*/
EXPORT_C void CMidiClientUtility::LoadCustomBankL(const TDesC& aFileName, TInt& aBankCollectionIndex)
	{
	iBody->LoadCustomBankL(aFileName, aBankCollectionIndex);
	}

/**
Removes a custom sound bank from memory.
Only valid for sound banks previously loaded from file.
Once unloaded the custom sound bank is no longer available for use.

@param aBankCollectionIndex 
       Identifier of the custom sound bank to unload,
       occupying no more than 14 bits
*/
EXPORT_C void CMidiClientUtility::UnloadCustomBankL(TInt aBankCollectionIndex)
	{
	iBody->UnloadCustomBankL(aBankCollectionIndex);
	}

/**
Query if a bank has been loaded to the memory

@param aBankCollectionIndex 
       Identifier of the custom sound bank to check if it's in memory or not
@return ETrue if the specified bank is in memory, EFalse otherwise
*/
EXPORT_C TBool CMidiClientUtility::CustomBankLoadedL(TInt aBankCollectionIndex) const
	{
	return iBody->CustomBankLoadedL(aBankCollectionIndex);
	}

/**
Removes all custom sound banks from memory.
*/
EXPORT_C void CMidiClientUtility::UnloadAllCustomBanksL()
	{
	iBody->UnloadAllCustomBanksL();
	}

/**
Gets the number of instruments available in a given sound bank

@param aBankId 
       Identifier of sound bank to reference, occupying no more than 14 bits
@param aCustom 
       Specifies whether to reference a custom or standard sound bank
@return Count of the number of instruments available for the specified sound bank
*/
EXPORT_C TInt CMidiClientUtility::NumberOfInstrumentsL(TInt aBankId,TBool aCustom) const
	{
	return iBody->NumberOfInstrumentsL(aBankId, aCustom);
	}

/**
Gets the identifier of an instrument.

@param aBankId 
       Identifier of the sound bank to reference, occupying no more than 14 bits.
@param aCustom 
       Specifies whether to reference a custom or standard sound bank.
@param aInstrumentIndex 
       Index of the instrument to reference where 0 <= aInstrumentIndex < NumberOfInstrumentsL(). 
@return Identifier of specified instrument.
        This may differ from the index since the index simply enumerates the instruments,
        whereas identifiers may not be contiguous, especially where certain instruments
        correspond to General MIDI-defined instruments but not all instruments are
        present. Instrument identifiers are between 0 and 127 inclusive.
*/
EXPORT_C TInt CMidiClientUtility::GetInstrumentIdL(TInt aBankId,TBool aCustom,TInt aInstrumentIndex) const
	{
	return iBody->GetInstrumentIdL(aBankId, aCustom, aInstrumentIndex);
	}

/**
Gets the name of the given instrument.

@param  aBankId
        Identifier of the bank that the instrument belongs to, occupying no more than 14 bits
@param  aCustom
        Specifies whether to reference a custom or standard sound bank
@param  aInstrumentId
        Identifier of the instrument under scrutiny. 0 <= iInstrumentId <= 127.
@return Buffer containing the name of the specified instrument.
        If it has no name then an empty descriptor is returned
*/
EXPORT_C HBufC* CMidiClientUtility::InstrumentNameL(TInt aBankId, TBool aCustom, TInt aInstrumentId) const
	{
	return iBody->InstrumentNameL(aBankId, aCustom, aInstrumentId);
	}

/**
Sets a logical channel to use the given instrument.

@param aChannel 
       Logical channel to set the instrument for. 0 <= aChannel <= 15
@param aBankId 
       Identifier of the bank that the instrument belongs to,
       occupying no more than 14 bits.
       The bank ID is a concatenation of MIDI bank MSB and LSB values
@param aInstrumentId 
       Identifier of the instrument under scrutiny. 0 <= iInstrumentId <= 127.
*/
EXPORT_C void CMidiClientUtility::SetInstrumentL(TInt aChannel,TInt aBankId,TInt aInstrumentId)
	{
	iBody->SetInstrumentL(aChannel, aBankId, aInstrumentId);
	}

/**
Loads an individual instrument from file into custom sound bank memory for use.
The bank and instrument ids given in the file can be mapped into different bank
and instrument ids in memory

@param aFileName 
       Name of the file containing the instrument
@param aFileBankId 
       Identifier of the bank in the file from which to load the instrument,
       occupying no more than 14 bits
@param aFileInstrumentId 
       Identifier of the instrument to load. 0 <= aInstrumentId <= 127
@param aMemoryBankId 
       Identifier of the custom bank in memory to load the instrument into,
       occupying no more than 14 bits.
@param aMemoryInstrumentId 
       Identifier of the instrument in memory to load the new
       instrument into. 0 <= aInstrumentId <= 127.
*/
EXPORT_C void CMidiClientUtility::LoadCustomInstrumentL(const TDesC& aFileName,TInt aFileBankId,TInt aFileInstrumentId,TInt aMemoryBankId,TInt aMemoryInstrumentId)
	{
	iBody->LoadCustomInstrumentL(aFileName, aFileBankId, aFileInstrumentId, aMemoryBankId, aMemoryInstrumentId);
	}

/**
Removes an instrument from custom sound bank memory.
Only valid for instruments previously loaded from file.
Once unloaded the instrument is no longer available for use

@param aCustomBankId 
       Identifier of the custom sound bank containing
       the instrument to unload, occupying no more than 14 bits.
@param aInstrumentId 
       Identifier of the instrument to unload. 0 <= aInstrumentId <= 127
*/
EXPORT_C void CMidiClientUtility::UnloadCustomInstrumentL(TInt aCustomBankId,TInt aInstrumentId)
	{
	iBody->UnloadCustomInstrumentL(aCustomBankId, aInstrumentId);
	}

/**
Gets the name of a particular percussion key corresponding to a given note.

@param aNote 
       Note to query. 0 <= aNote <= 127
@param aBankId 
       Identifier of the bank that the instrument belongs to, occupying no more than 14 bits.
       The bank ID is a concatenation of MIDI bank MSB and LSB values.
@param aCustom 
       Specifies whether to reference a custom or standard sound bank
@param aInstrumentId 
       Identifier of an instrument
@return Descriptor containing the name of the percussion key.
        If the key does not have a name then an empty descriptor is returned
*/
EXPORT_C HBufC* CMidiClientUtility::PercussionKeyNameL(TInt aNote, TInt aBankId, TBool aCustom, TInt aInstrumentId) const
	{
	return iBody->PercussionKeyNameL(aNote, aBankId, aCustom, aInstrumentId);
	}

/**
Get the stop time currently set for the MIDI resource

@param aStopTime 
       Time at which playback will stop, relative to the start of the resource
*/
EXPORT_C void CMidiClientUtility::StopTimeL(TTimeIntervalMicroSeconds& aStopTime) const
	{
	iBody->StopTimeL(aStopTime);
	}

/**
Sets the stop time to use for the currently open MIDI resource

@param aStopTime 
       Time at which playback will stop, relative to the start of the resource.
       Clamped to 0 and the duration of the resource
*/
EXPORT_C void CMidiClientUtility::SetStopTimeL(const TTimeIntervalMicroSeconds& aStopTime)
	{
	iBody->SetStopTimeL(aStopTime);
	}

/**
Set the number of times to repeat the current MIDI resource.
After Stop() has been called, repeat number of times and the trailing silence are reset

@param aRepeatNumberOfTimes 
       Number of time to repeat the resource during playback.
       This includes the first playing
@param aTrailingSilence 
       Time in microseconds to pause between repeats
*/
EXPORT_C void CMidiClientUtility::SetRepeatsL(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	iBody->SetRepeatsL(aRepeatNumberOfTimes, aTrailingSilence);
	}

/**
Gets the number of currently active voices.

@return The number of currently active voices
*/
EXPORT_C TInt CMidiClientUtility::PolyphonyL() const
	{
	return iBody->PolyphonyL();
	}

/**
Gets the maximum number of logical channels supported by the MIDI engine.

@return  The maximum number of logical channels that the MIDI engine supports, 0 <= aChannels <=15.
*/
EXPORT_C TInt CMidiClientUtility::ChannelsSupportedL() const
	{
	return iBody->ChannelsSupportedL();
	}

/**
Get the current volume setting of a logical channel

@param aChannel 
       Logical channel to query. 0 <= aChannel <= 15.
@return Volume currently set on the specified channel in decibels
*/
EXPORT_C TReal32 CMidiClientUtility::ChannelVolumeL(TInt aChannel) const
	{
	return iBody->ChannelVolumeL(aChannel);
	}

/**
Gets the Maximum volume setting that may be applied to a logical channel

@return Maximum volume setting. Minimum value is -infinity dB, which is the
       smallest possible value that TReal32 supports.
*/
EXPORT_C TReal32 CMidiClientUtility::MaxChannelVolumeL() const
	{
	return iBody->MaxChannelVolumeL();
	}

/**
Set the volume of a channel.

@param aChannel 
       Logical channel to set the volume on. 0 <= aChannel <= 15
@param aVolume 
       Volume currently set on the specified channel in decibels. The minimum
       channel volume supported value is -infinity dB, which is the smallest
       possible value that TReal32 supports. 
	   The maximum channel volume can be set via MaxChannelVolumeL()
*/
EXPORT_C void CMidiClientUtility::SetChannelVolumeL(TInt aChannel,TReal32 aVolume)
	{
	iBody->SetChannelVolumeL(aChannel, aVolume);
	}

/**
Set the muting state of a channel without changing its volume setting.
When unmuted the channel goes back to its previous volume setting

@param aChannel 
       Logical channel to set the mute state of. 0 <= aChannel <= 15.
@param aMuted 
       ETrue to mute the channel, EFalse to unmute it.
*/
EXPORT_C void CMidiClientUtility::SetChannelMuteL(TInt aChannel,TBool aMuted)
	{
	iBody->SetChannelMuteL(aChannel, aMuted);
	}

/**
Gets the overall volume of the MIDI client.

@return The current overall volume setting
*/
EXPORT_C TInt CMidiClientUtility::VolumeL() const
	{
	return iBody->VolumeL();
	}

/**
Maximum volume setting that may be applied overall.

@return Maximum volume setting. Minimum value is always zero which is silent
*/
EXPORT_C TInt CMidiClientUtility::MaxVolumeL() const
	{
	return iBody->MaxVolumeL();
	}

/**
Set the overall volume of the MIDI client.
This setting scales all channel volumes respectively so the actual volume
that a channel is played at is (overall volume * channel volume / max volume).

@param aVolume 
       Overall volume setting to use
*/
EXPORT_C void CMidiClientUtility::SetVolumeL(TInt aVolume)
	{
	iBody->SetVolumeL(aVolume);
	}

/**
Length of time over which the volume is faded up from zero to the current settings
when playback is started.

@param aRampDuration 
       Duration of the ramping period.
*/
EXPORT_C void CMidiClientUtility::SetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	iBody->SetVolumeRampL(aRampDuration);
	}

/**
Get the current stereo balance value

@return Balance value ranging from KMMFBalanceMaxLeft to KMMFBalanceMaxRight
*/
EXPORT_C TInt CMidiClientUtility::GetBalanceL() const
	{
	return iBody->GetBalanceL();
	}

/**
Set the current stereo balance value

@param aBalance 
       Balance value to set. Defaults to KMMFBalanceCenter to restore equal left-right balance
*/
EXPORT_C void CMidiClientUtility::SetBalanceL(TInt aBalance)
	{
	iBody->SetBalanceL(aBalance);
	}

/**
Set the priority with which this client plays MIDI data

@param  aPriority
        The Priority Value.
@param  aPref
        The Priority Preference.
        
@see CMidiClientUtility::NewL()
*/
EXPORT_C void CMidiClientUtility::SetPriorityL(TInt aPriority, TInt aPref)
	{
	iBody->SetPriorityL(aPriority, aPref);
	}

/**
Get the number of meta data entries currently known about in the currently open
resource. XMF,SMF meta data are part of the XMF,SMF file header and can thus be examined
before playback. If there is no XMF,SMF resource open, will return zero.
Standard MIDI file meta data entries encountered during playback will be passed back
via MMIDIClientUtilityObserver::MmcuoMetaDataEntryFound()

@return Number of XMF meta data entries currently known about
*/
EXPORT_C TInt CMidiClientUtility::NumberOfMetaDataEntriesL() const
	{
	return iBody->NumberOfMetaDataEntriesL();
	}

/**
Retrieve the specified XMF,SMF meta data entry.

@param aMetaDataIndex 
       Index of the meta data entry to retrieve
@return Meta data entry. Ownership is passed to the client.
*/
EXPORT_C CMMFMetaDataEntry* CMidiClientUtility::GetMetaDataEntryL(TInt aMetaDataIndex) const
	{
	return iBody->GetMetaDataEntryL(aMetaDataIndex);
	}

/**
Synchronously pass implementation-specific commands to the MIDI engine
and receive a response

@param aDestination 
       Recipient of the message. Should be initialised with KUidInterfaceMIDI
       and a TInt describing the server-side object to which the command should be delivered.
       The TInt will usually be KMMFObjectHandleController, to deliver the message to the
       controller plugin, which is the default value.
@param aFunction 
       Index of the function to perform
@param aDataTo1 
       First command data buffer to send, eg command parameters
@param aDataTo2 
       Second command data buffer to send, eg data parameters
@param aDataFrom 
       Buffer to receive data in response to the command.
       The user must ensure that it is large enough to hold all the data returned.
*/
EXPORT_C void CMidiClientUtility::CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	iBody->CustomCommandSyncL(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}

/**
Synchronously pass implementation-specific commands to the MIDI engine.

@param aDestination 
       Recipient of the message. Should be initialised with KUidInterfaceMIDI
       and a TInt describing the server-side object to which the command should be delivered.
       The TInt will usually be KMMFObjectHandleController, to deliver the message to the
       controller plugin, which is the default value.
@param aFunction 
       Index of the function to perform
@param aDataTo1 
       First command data buffer to send, eg command parameters
@param aDataTo2 
       Second command data buffer to send, eg data parameters
*/
EXPORT_C void CMidiClientUtility::CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	iBody->CustomCommandSyncL(aDestination, aFunction, aDataTo1, aDataTo2);
	}

/**
Asynchronously pass implementation-specific commands to the MIDI engine
and receive a response

@param aDestination 
       aDestination Recipient of the message. Should be initialised with KUidInterfaceMIDI
       and a TInt describing the server-side object to which the command should be delivered.
       The TInt will usually be KMMFObjectHandleController, to deliver the message to the
       controller plugin, which is the default value.
@param aFunction 
       Index of the function to perform
@param aDataTo1 
       First command data buffer to send, eg command parameters
@param aDataTo2 
       Second command data buffer to send, eg data parameters
@param aDataFrom 
       Buffer to receive data in response to the command.
       The user must ensure that it is large enough to hold all the data returned.
@param aStatus
       Status flag belonging to an active object that will have it's RunL() called
       when this request complete
*/
EXPORT_C void CMidiClientUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iBody->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}

/**
Asynchronously pass implementation-specific commands to the MIDI engine

@param aDestination 
       aDestination Recipient of the message. Should be initialised with KUidInterfaceMIDI
       and a TInt describing the server-side object to which the command should be delivered.
       The TInt will usually be KMMFObjectHandleController, to deliver the message to the
       controller plugin, which is the default value.
@param aFunction 
       Index of the function to perform
@param aDataTo1 
       First command data buffer to send, eg command parameters
@param aDataTo2 
       Second command data buffer to send, eg data parameters
@param aStatus 
       Status flag belonging to an active object that will have it's RunL() called
       when this request complete
*/
EXPORT_C void CMidiClientUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iBody->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

/**
Gets a controller's DRM custom command implementation.

@return A pointer to a controller's DRM custom command implementation or NULL
		if the interface can not be obtained
*/
EXPORT_C MMMFDRMCustomCommand* CMidiClientUtility::GetDRMCustomCommand()
	{
	return iBody->GetDRMCustomCommand();
	}

/**
Set the max polyphony the engine can handle

@param aMaxNotes 
       Max polyphony level,  0 <= PolyphonyL() <= aMaxNotes
*/
EXPORT_C void CMidiClientUtility::SetMaxPolyphonyL(TInt aMaxNotes)
	{
	iBody->SetMaxPolyphonyL(aMaxNotes);
	}

/**
Gets the number of times the current opened resources has to be repeated

@return The number of time the current opened resources has to be repeated
*/
EXPORT_C TInt CMidiClientUtility::GetRepeats() const
	{
	return iBody->GetRepeats();
	}

/**
Loads one or more custom sound banks from a descriptor into memory for use.
If several banks are loaded with consequent LoadCustomBanksL() function calls,
the banks are combined if the bank sets have colliding bank numbers

@param aBankData 
       Descriptor containing the custom sound bank
@param aBankId 
       Identifier of the custom sound bank loaded, occupying no more than 14 bits.
*/
EXPORT_C void CMidiClientUtility::LoadCustomBankDataL(const TDesC8& aBankData,TInt& aBankId)
	{
	iBody->LoadCustomBankDataL(aBankData, aBankId);
	}

/**
Loads an individual instrument from descriptor into custom sound bank memory for use.
The bank and instrument ids given in the descriptor can be mapped into different bank
and instrument ids in memory

@param aInstrumentData 
       Descriptor containing the instrument
@param aBankDataId 
       Identifier of the bank in the descriptor from which to load the instrument,
       occupying no more than 14 bits
@param aInstrumentDataId 
       Identifier of the instrument to load. 0 <= aInstrumentId <= 127
@param aMemoryBankId 
       Identifier of the custom bank in memory to load the instrument into,
       occupying no more than 14 bits
@param aMemoryInstrumentId 
       Identifier of the instrument in memory to load the new
       instrument into. 0 <= aInstrumentId <= 127.
*/
EXPORT_C void CMidiClientUtility::LoadCustomInstrumentDataL(const TDesC8& aInstrumentData, TInt aBankDataId, TInt aInstrumentDataId, TInt aMemoryBankId, TInt aMemoryInstrumentId)
	{
	iBody->LoadCustomInstrumentDataL(aInstrumentData, aBankDataId, aInstrumentDataId, aMemoryBankId, aMemoryInstrumentId);
	}

/**
Tell the MIDI engine to use a custom bank or a standard bank

@param aCustom 
       If Etrue the custom bank in memory is used otherwise the standard bank
       is used leaving the custom bank in memory
*/
EXPORT_C void CMidiClientUtility::SetBankL(TBool aCustom)
	{
	iBody->SetBankL(aCustom);
	}

/**
Gets the muting status of a specific track

@param aTrack 
       The track to query
@return The mute status of the track.
*/
EXPORT_C TBool CMidiClientUtility::IsTrackMuteL(TInt aTrack) const
	{
	return iBody->IsTrackMuteL(aTrack);
	}

/**
Gets the muting status of a specific channel

@param aChannel 
       The channel to query
@return The mute status of the channel
*/
EXPORT_C TBool CMidiClientUtility::IsChannelMuteL(TInt aChannel) const
	{
	return iBody->IsChannelMuteL(aChannel);
	}

/**
Gets the instrument assigned to a specified channel

@param aChannel 
       Logical channel, 0 <= aChannel <= 15.
@param aInstrumentId 
       Identifier of the instrument assigned to aChannel. 0 <= iInstrumentId <= 127
@param aBankId 
       Identifier of the bank that the instrument belongs to, occupying no more than 14 bits
*/
EXPORT_C void CMidiClientUtility::GetInstrumentL(TInt aChannel, TInt& aInstrumentId, TInt& aBankId)
	{
	iBody->GetInstrumentL(aChannel, aInstrumentId, aBankId);
	}

/**
Get the maximum polyphony level that the engine can handle

@return The maximum number of simultaneous notes the engine can handle.
		0 <= PolyphonyL() <= MaxPolyphonyL()
*/
EXPORT_C TInt CMidiClientUtility::MaxPolyphonyL() const
	{
	return iBody->MaxPolyphonyL();
	}
	
