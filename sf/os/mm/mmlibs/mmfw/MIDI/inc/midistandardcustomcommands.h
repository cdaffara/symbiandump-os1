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

#ifndef __MIDISTANDARDCUSTOMCOMMANDS_H__
#define __MIDISTANDARDCUSTOMCOMMANDS_H__

#include <mmf/common/mmfcontroller.h>
#include <midiclientutility.h>
#include <mmf/common/mmfmidi.h>

/**
Interface UID for the Midi Controller API.

@publishedAll
@released
*/
const TUid KUidInterfaceMidi = { 0x101F945C };

/**
Describe a MIDI event.
Contains a UID to define the actual event type, and an integer to define the event code.

@publishedAll
@released
*/
class CMMFMidiEvent: public CBase
	{
public:
	IMPORT_C CMMFMidiEvent(TUid aEventType, TInt aErrorCode);
	IMPORT_C CMMFMidiEvent();
	IMPORT_C ~CMMFMidiEvent();
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void CopyL(const CMMFMidiEvent& aOther);
	/**
    @internalTechnology
    */
	void ZeroMembers();
public:
   /**
    * A UID to define the type of MIDI event.
    */
	TUid iEventType;
   /**
    * The error code associated with the MIDI event.
    */
	TInt iErrorCode;
   /**
    * Old state of the MIDI client utility object.
    */
	TMidiState iOldState;
   /**
    * Current state of the MIDI client utility object.
    */
	TMidiState iNewState;
   /**
    * General purpose time stamp in microseconds.
    */
	TTimeIntervalMicroSeconds iMicroSeconds;
   /**
    * General purpose time stamp in micro beats per minute. 
    */
	TInt64 iMicroBeats;
   /**
    * Channel
    */
	TInt iChannel;
   /**
    * Volume in decibel of a logical channel.
    */
	TReal32 iVolumeInDecibels;
   /**
    * Mute state of a track or of a channel.
    */
	TBool iMute;
   /**
    * Meta data entry. 
	* Client will retrieve meta data entry by calling GetMetaDataEntryL()
	* using this TInt to indicate which meta data entry to retrieve.
    */
	TInt iMetaDataEntryId;
   /**
    * Array of logical {channel, MIP} value pairs.
    */
	RArray<TMipMessageEntry> iMipMessage; 
   /**
    * Polyphony of the MIDI engine.
    */
	TInt iPolyphony;
   /**
    * Identifier of a bank occupying, at most, 14 bits.
    */
	TInt iBankId;
   /**
    * Identifier of a specific instrument.
    */
	TInt iInstrumentId;
   /**
    * Tempo value in micro beats per minute.
    */
	TInt iTempoMicroBeats;
	};

/**
Client class to access functionality specific to a MIDI controller.
The class uses the custom command function of the controller plugin, and removes the necessity
for the client to formulate the custom commands.

@publishedAll
@released
*/
class RMidiControllerCustomCommands : public RMMFCustomCommandsBase
	{
public:
	IMPORT_C RMidiControllerCustomCommands(RMMFController& aController);
	IMPORT_C TInt SetPositionMicroBeats(TInt64 aMicroBeats) const;
	IMPORT_C TInt PositionMicroBeats(TInt64& aMicroBeats) const;
	IMPORT_C TInt PlayNote(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	IMPORT_C TInt PlayNote(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aStartTime, const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	IMPORT_C TInt StopNotes(TInt aChannel);
	IMPORT_C TInt NoteOn(TInt aChannel,TInt aNote,TInt aVelocity);
	IMPORT_C TInt NoteOff(TInt aChannel,TInt aNote,TInt aVelocity);
	IMPORT_C TInt PlaybackRate(TInt& aPlayBackRate) const;
	IMPORT_C TInt SetPlaybackRate(TInt aPlayBackRate);
	IMPORT_C TInt MaxPlaybackRate(TInt& aMaxRate) const;
	IMPORT_C TInt MinPlaybackRate(TInt& aMinRate) const;
	IMPORT_C TInt TempoMicroBeatsPerMinute(TInt& aMicroBeatsPerMinute) const;
	IMPORT_C TInt SetTempo(TInt aMicroBeatsPerMinute);
	IMPORT_C TInt PitchTranspositionCents(TInt& aPitch) const;
	IMPORT_C TInt SetPitchTransposition(TInt aCents, TInt& aCentsApplied);
	IMPORT_C TInt DurationMicroBeats(TInt64& aDuration) const;
	IMPORT_C TInt NumTracks(TInt& aTracks) const;
	IMPORT_C TInt SetTrackMute(TInt aTrack, TBool aMuted) const;
	IMPORT_C TInt MimeType(TDes8& aMimeType) const;
	IMPORT_C TInt SetSyncUpdateCallbackInterval(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats=0);
	IMPORT_C TInt SendMessage(const TDesC8& aMidiMessage, TInt& aBytes);
	IMPORT_C TInt SendMessage(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& aTime, TInt& aBytes);
	IMPORT_C TInt SendMipMessage(const RArray<TMipMessageEntry>& aEntry);
	IMPORT_C TInt NumberOfBanks(TBool aCustom, TInt& aNumBanks) const;
	IMPORT_C TInt GetBankId(TBool aCustom, TInt aBankIndex, TInt& aBankId) const;
	IMPORT_C TInt LoadCustomBank(const TDesC& aFileName,TInt& aBankId);
	IMPORT_C TInt LoadCustomBankData(const TDesC8& aBankData,TInt& aBankId);
	IMPORT_C TInt UnloadCustomBank(TInt aBankId);
	IMPORT_C TInt CustomBankLoaded(TInt aBankId, TBool& aBankLoaded) const;
	IMPORT_C TInt UnloadAllCustomBanks();
	IMPORT_C TInt NumberOfInstruments(TInt aBankId, TBool aCustom, TInt& aNumInstruments) const;
	IMPORT_C TInt GetInstrumentId(TInt aBankId,TBool aCustom,TInt aInstrumentIndex, TInt& aInstrumentId) const;
	IMPORT_C HBufC* InstrumentNameL(TInt aBankId, TBool aCustom, TInt aInstrumentId) const;
	IMPORT_C TInt SetInstrument(TInt aChannel,TInt aBankId,TInt aInstrumentId);
	IMPORT_C TInt LoadCustomInstrument(const TDesC& aFileName, TInt aFileBankId, TInt aFileInstrumentId, TInt aMemoryBankId, TInt aMemoryInstrumentId);
	IMPORT_C TInt LoadCustomInstrumentData(const TDesC8& aInstrumentData, TInt aBankDataId, TInt aInstrumentDataId, TInt aMemoryBankId, TInt aMemoryInstrumentId);
	IMPORT_C TInt UnloadCustomInstrument(TInt aCustomBankId,TInt aInstrumentId);
	IMPORT_C HBufC* PercussionKeyNameL(TInt aNote, TInt aBankId, TBool aCustom, TInt aInstrumentId) const;
	IMPORT_C TInt StopTime(TTimeIntervalMicroSeconds& aStopTime) const;
	IMPORT_C TInt SetStopTime(const TTimeIntervalMicroSeconds& aStopTime) const;
	IMPORT_C TInt Polyphony(TInt& aNumNotes) const;
	IMPORT_C TInt ChannelsSupported(TInt& aChannels) const;
	IMPORT_C TInt ChannelVolume(TInt aChannel, TReal32& aChannelVol) const;
	IMPORT_C TInt MaxChannelVolume(TReal32& aMaxVol) const;
	IMPORT_C TInt SetChannelVolume(TInt aChannel,TReal32 aVolume);
	IMPORT_C TInt SetChannelMute(TInt aChannel,TBool aMuted);
	IMPORT_C TInt Volume(TInt& aVolume) const;
	IMPORT_C TInt MaxVolume(TInt& aMaxVolume) const;
	IMPORT_C TInt SetVolume(TInt aVolume);
	IMPORT_C TInt SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	IMPORT_C TInt GetBalance(TInt& aBalance) const;
	IMPORT_C TInt SetBalance(TInt aBalance);
	IMPORT_C TInt SetMaxPolyphony(TInt aMaxNotes);
	IMPORT_C TInt GetRepeats(TInt& aNumRepeats) const;
	IMPORT_C TInt SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
 	IMPORT_C TInt SetBank(TBool aCustom);
	IMPORT_C TInt IsTrackMute(TInt aTrack, TBool& aTrackMute) const;
	IMPORT_C TInt IsChannelMute(TInt aChannel, TBool& aChannelMute) const;
	IMPORT_C TInt GetInstrument(TInt aChannel, TInt& aInstrumentId, TInt& aBankId);
	IMPORT_C TInt Close();
	IMPORT_C TInt Stop(const TTimeIntervalMicroSeconds& aFadeOutDuration);
	IMPORT_C void ReceiveEvents(TPckgBuf<TInt>& aEventSize, TRequestStatus& aStatus);
	IMPORT_C TInt RetrieveEvent(TDes8& aMidiEventPckg);
	IMPORT_C TInt CancelReceiveEvents();
	IMPORT_C TInt MaxPolyphony(TInt& aNumNotes) const;
	};


/**
Mixin class to be derived from by controller plugins wishing 
to support the MIDI controller custom commands.

@publishedAll
@released
*/
class MMidiCustomCommandImplementor
	{
public:
    /**
    Change the position of the currently playing MIDI resource to the given position.
    May be called whenever a MIDI resource is open.
    
    @param aMicroBeats 
	       Metrical position to move to. Clamped to (0, DurationMicroBeats()).
    */
	virtual void  MmcSetPositionMicroBeatsL(TInt64 aMicroBeats) = 0;
    /**
    Gets the current metrical position of the MIDI resource being played
    
    @param aMicroBeats 
	       (BPM*1000000) relative to the start of the resource
    */
	virtual void  MmcPositionMicroBeatsL(TInt64& aMicroBeats) = 0;
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
	virtual void  MmcPlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity) = 0;
    /**
    Synchronous function to play a single note at a specified time.
    Multiple calls to this function will be accommodated as far as the MIDI engine can
    manage. The same functionality could be implemented using the SendMessage function
    
    @param aChannel 
	       Logical channel to play note on. 0 <= aChannel <= 15.
    @param aNote 
	       Note to play. 0 <= aNote <= 127
    @param aStartTime 
	       Specifies the time at which to start playing the note, relative to the MIDI
		   resource playing time or the time elapsed since Play() was called if no resource is present.
    @param aDuration 
	       Length of time to play note for.
    @param aNoteOnVelocity 
	       Velocity with which to start the note. 0 <= aNoteOnVelocity <= 127.
    @param aNoteOffVelocity 
	       Velocity with which to stop the note. 0 <= aNoteOffVelocity <= 127.
    */
	virtual void  MmcPlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aStartTime, const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity) = 0;
    /**
    Stops the playback of all notes on the given channel,
    by means of an All Notes Off MIDI message
    
    @param aChannel 
	       Logical channel to stop notes on. 0 <= aChannel <= 15
    */
	virtual void  MmcStopNotesL(TInt aChannel) = 0;
    /**
    Synchronous function to commence playback of a note.
    Multiple calls to this function will be accommodated as far as the MIDI engine can manage
    
    @param aChannel 
	       Logical channel to play note on. 0 <= aChannel <= 15.
    @param aNote 
	       Note to play. 0 <= aNote <= 127.
    @param aVelocity 
	       Velocity with which to start the note. The legal integer range
		   is 0 <= aVelocity <= 127, but the value zero actually causes the 
		   message to be interpreted as a Note Off message instead of a Note On.
    */
	virtual void  MmcNoteOnL(TInt aChannel,TInt aNote,TInt aVelocity) = 0;
    /**
    Synchronous function to terminate playback of a note.
    If no corresponding note is found then no error is raised
    
    @param aChannel 
	       Logical channel on which the note is playing. 0 <= aChannel <= 15.
    @param aNote
	       Note to terminate. 0 <= aNote <= 127.
    @param aVelocity 
	       Velocity with which to stop the note. 0 <= aVelocity <= 127.
           There is no standard behaviour corresponding with note off velocity.
    */
	virtual void  MmcNoteOffL(TInt aChannel,TInt aNote,TInt aVelocity) = 0;
    /**
    Gets the current playback rate factor of the currently open MIDI resource.
    The playback rate is independent from tempo,
    i.e., it can be used to give an overall speed factor for playback.
    
    @param aPlayBackRate 
	       Current playback rate in percent times 1000, i.e., 100000 means original 
		   playback speed, 200000 means double speed, and 50000 means half speed playback.
    */
	virtual void  MmcPlaybackRateL(TInt& aPlayBackRate) = 0;
    /**
    Sets the playback rate for the playback of the current MIDI resource.
    The playback rate is independent from tempo,
    i.e., it can be used to give an overall speed factor for playback.
    May be called whether playback is in progress or not.
    
    @param aPlayBackRate 
	       Playback rate in percent times 1000, i.e., 100000 means original playback speed,
		   200000 means double speed, and 50000 means half speed playback.
    */
	virtual void  MmcSetPlaybackRateL(TInt aPlayBackRate) = 0;
    /**
    Gets the maximum playback rate in milli-percentage from the MIDI engine.
    @see SetPlaybackRate() for milli-percentage details.
    
    @param aMaxRate 
	       Playback rate supported by MIDI player.
    */
	virtual void  MmcMaxPlaybackRateL(TInt& aMaxRate) = 0;
    /**
    Gets the minimum playback rate in milli-percentage from the MIDI engine.
    @see SetPlaybackRate() for milli-percentage details.
    
    @param aMinRate 
	       Minimum playback rate supported by MIDI player.
    */
	virtual void  MmcMinPlaybackRateL(TInt& aMinRate) = 0;
    /**
    Gets the current tempo of the currently open MIDI resource. The tempo is independent
    from the playback rate, i.e., the resulting playback speed will be affected by both.
    
    @param  aMicroBeatsPerMinute 
	        Tempo at the current position of the currently open resource in microbeats
			per minute, i.e. BPM * 1000000. Filled in by the controller framework.
    */
	virtual void  MmcTempoMicroBeatsPerMinuteL(TInt& aMicroBeatsPerMinute) = 0;
    /**
    Sets the tempo at which the current MIDI resource should be played.
    May be called whether playback is in progress or not. The tempo is 
	independent from the playback rate, i.e., the resulting playback speed
	will be affected by both.
    
    @param aMicroBeatsPerMinute 
	       Tempo in microbeats per minute (BPM*1000000) to set.
    */
	virtual void  MmcSetTempoL(TInt aMicroBeatsPerMinute) = 0;
    /**
    Gets the pitch shift in use for the currently open MIDI resource.
    
    @param aPitch 
	       Shift in cents, i.e. semitones * 100. One octave equals 1200 cents.
    @return One of the system-wide error codes.
    */
	virtual void  MmcPitchTranspositionCentsL(TInt& aPitch) = 0;
    /**
    Sets the pitch shift to apply to the currently open MIDI resource.
    May be called during playback.
    
    @param aCents 
	       Pitch shift in cents, i.e. semitones * 100. One octave equals 1200 cents.
    @param aCentsApplied 
	       Actual pitch shift applied - may differ from the requested value due
		   to limitations of the MIDI engine.
    */
	virtual void  MmcSetPitchTranspositionL(TInt aCents, TInt& aCentsApplied) = 0;
    /**
    Gets the length of the currently open MIDI resource in micro-beats
    
    @param aDuration 
	       Duration in microbeats (beats * 1000000).
    */
	virtual void  MmcDurationMicroBeatsL(TInt64& aDuration) = 0;
    /**
    Gets the number of tracks present in the currently open MIDI resource.
    
    @param aTracks 
	       Number of tracks.
    */
	virtual void  MmcNumTracksL(TInt& aTracks) = 0;
    /**
    Mutes or unmutes a particular track.
    
    @param aTrack 
	       Index of the track to mute - 0 <= aTrack < NumTracks().
    @param aMuted 
	       ETrue to mute the track, EFalse to unmute it.
    */
	virtual void  MmcSetTrackMuteL(TInt aTrack, TBool aMuted) = 0;
    /**
    Gets the MIME type of the MIDI resource currently open.
    
    @param aMimeType
	       Descriptor containing the MIDI mime type.
    */
	virtual void  MmcMimeTypeL(TDes8& aMimeType) = 0;
    /**
    Sets the frequency at which MMIDIClientUtilityObserver::MmcuoSyncUpdateL() is called
    to allow other components to synchronise with playback of this MIDI resource.
    
    @param aMicroSeconds
	       Temporal interval to callback at. Used in preference to aMicroBeats if both are set.
    @param aMicroBeats
	       Metrical interval to callback at. Set both parameters to zero to cancel.
    */
	virtual void  MmcSetSyncUpdateCallbackIntervalL(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats=0) = 0;
    /**
    Sends a single MIDI message to the MIDI engine.
    
    @param aMidiMessage
	       Descriptor containing the MIDI message data. If there are several
           MIDI messages in the buffer, only the first one is processed.
    @param aBytes
	       Number of bytes of the message buffer actually processed.
    */
	virtual void  MmcSendMessageL(const TDesC8& aMidiMessage, TInt& aBytes) = 0;
    /**
    Sends a single MIDI message, with time stamp, to the MIDI engine.
    
    @param aMidiMessage 
	       Descriptor containing the MIDI message data. If there are several
		   MIDI messages in the buffer, only the first one is processed.
    @param aTime
	       The time at which to execute the message, relative to the MIDI resource playing
           time or the time elapsed since Play() was called if no resource is present.
    @param aBytes       
	       Number of bytes of the message buffer actually processed.
    */
	virtual void  MmcSendMessageL(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& aTime, TInt& aBytes) = 0;
    /**
    Sends a mip message to the MIDI engine. This is a convenience function,
    because the same functionality could be achieved with the SendMessage() function.
    
    @param aEntry 
	       Array of logical {channel, MIP} value pairs to send, highest priority first.
    */
	virtual void  MmcSendMipMessageL(const TArray<TMipMessageEntry>& aEntry) = 0;
    /**
    Gets the number of standard or custom sound banks currently available.
    
    @param aCustom
	       Specifies whether to reference a custom or standard sound bank.
    @param aNumBanks 
	       Number of custom or standard sound banks available.
    */
	virtual void  MmcNumberOfBanksL(TBool aCustom, TInt& aNumBanks) = 0;
    /**
    Gets the identifier of a sound bank. Bank identifier (aka bank number) is a
    14-bit value consisting of MIDI bank MSB and LSB values.
    
    @param  aCustom
            Specifies whether to reference a custom or standard sound bank.
    @param  aBankIndex
	        Index of sound bank where 0 <= aBankIndex < NumberOfBanks().
    @param  aBankId
            Identifier of the specified bank occupying, at most, 14 bits.
    */
	virtual void  MmcGetBankIdL(TBool aCustom, TInt aBankIndex, TInt& aBankId) = 0;
    /**
    Loads one or more custom sound banks from a file into memory for use.
    If several banks are loaded with consequent LoadCustomBanksL() function calls,
    the banks are combined if the bank sets have conflicting bank numbers.
    
    @param aFileName
	       Name of the file containing the custom sound bank.
    @param aBankId
	       Identifier of the custom sound bank loaded, occupying no more than 14 bits.
    */
	virtual void  MmcLoadCustomBankL(const TDesC& aFileName,TInt& aBankId) = 0;
    /**
    Loads one or more custom sound banks from a descriptor into memory for use.
    If several banks are loaded with consequent LoadCustomBanksL() function calls,
    the banks are combined if the bank sets have conflicting bank numbers.
    
    @param aBankData
	       Descriptor containing the custom sound bank.
    @param aBankId
	       Identifier of the custom sound bank loaded, occupying no more than 14 bits.
    */
	virtual void  MmcLoadCustomBankDataL(const TDesC8& aBankData,TInt& aBankId) = 0;
    /**
    Removes a custom sound bank from memory. Only valid for sound banks previously
	loaded from file. Once unloaded the custom sound bank is no longer available for use.
    
    @param aBankId 
	       Identifier of the custom sound bank to unload,occupying no more than 14 bits.
    */
	virtual void  MmcUnloadCustomBankL(TInt aBankId) = 0;
    /**
    Query if a bank has been loaded to the memory.
    
    @param aBankId
	       Identifier of the custom sound bank to check if it's in memory or not.
    @param aBankLoaded
	       ETrue if the specified bank is in memory, EFalse otherwise.
    */
	virtual void  MmcCustomBankLoadedL(TInt aBankId, TBool& aBankLoaded) = 0;
    /**
    Removes all custom sound banks from memory.
    */
	virtual void  MmcUnloadAllCustomBanksL() = 0;
    /**
    Gets the number of instruments available in a given sound bank.
    
    @param aBankId
	       Identifier of sound bank to reference, occupying no more than 14 bits.
    @param aCustom
	       Specifies whether to reference a custom or standard sound bank.
    @param aNumInstruments
	       Count of the number of instruments available for the specified sound bank.
    */
	virtual void  MmcNumberOfInstrumentsL(TInt aBankId, TBool aCustom, TInt& aNumInstruments) = 0;
    /**
    Gets the identifier of an instrument.
    
    @param aBankId
	       Identifier of the sound bank to reference, occupying no more than 14 bits.
    @param aCustom
	       Specifies whether to reference a custom or standard sound bank.
    @param aInstrumentIndex
	       Index of the instrument to reference where 0 <= aInstrumentIndex < NumberOfInstrumentsL(). 
    @param aInstrumentId
	       Identifier of specified instrument. This may differ from the index since the
		   index simply enumerates the instruments, whereas identifiers may not be 
		   contiguous, especially where certain instruments correspond to General MIDI-defined
		   instruments but not all instruments are present. Instrument identifiers are
		   between 0 and 127 inclusive.
    */
	virtual void  MmcGetInstrumentIdL(TInt aBankId,TBool aCustom,TInt aInstrumentIndex, TInt& aInstrumentId) = 0;
    /**
    Gets the name of the given instrument.
    
    @param  aBankId
	        Identifier of the bank that the instrument belongs to, occupying no more than 14 bits.
    @param  aCustom
	        Specifies whether to reference a custom or standard sound bank.
    @param  aInstrumentId
	        Identifier of the instrument under scrutiny. 0 <= aInstrumentId <= 127.
    @return Buffer containing the name of the specified instrument.
            If it has no name then an empty descriptor is returned.
    */
	virtual const TDesC& MmcInstrumentNameL(TInt aBankId, TBool aCustom, TInt aInstrumentId) = 0;
    /**
    Sets a logical channel to use the given instrument.
    
    @param aChannel
	       Logical channel to set the instrument for. 0 <= aChannel <= 15.
    @param aBankId
	       Identifier of the bank that the instrument belongs to, occupying no more than 14 bits.
           The bank ID is a concatenation of MIDI bank MSB and LSB values.
    @param aInstrumentId
	       Identifier of the instrument under scrutiny. 0 <= aInstrumentId <= 127.
    */
	virtual void  MmcSetInstrumentL(TInt aChannel,TInt aBankId,TInt aInstrumentId) = 0;
    /**
    Loads an individual instrument from file into custom sound bank memory for use.
    The bank and instrument ids given in the file can be mapped into different bank
    and instrument ids in memory.
    
    @param aFileName
	       Name of the file containing the instrument.
    @param aFileBankId
	       Identifier of the bank in the file from which to load the instrument,
           occupying no more than 14 bits.
    @param aFileInstrumentId
	       Identifier of the instrument to load. 0 <= aInstrumentId <= 127.
    @param aMemoryBankId
	       Identifier of the custom bank in memory to load the instrument into,
           occupying no more than 14 bits.
    @param aMemoryInstrumentId
	       Identifier of the instrument in memory to load the new instrument into.
		   0 <= aInstrumentId <= 127.
    */
	virtual void  MmcLoadCustomInstrumentL(const TDesC& aFileName, TInt aFileBankId, TInt aFileInstrumentId, TInt aMemoryBankId, TInt aMemoryInstrumentId) = 0;
    /**
    Loads an individual instrument from descriptor into custom sound bank memory for use.
    The bank and instrument ids given in the descriptor can be mapped into different bank
    and instrument ids in memory.
    
    @param aInstrumentData
	       Descriptor containing the instrument.
    @param aBankDataId
	       Identifier of the bank in the descriptor from which to load the instrument,
           occupying no more than 14 bits.
    @param aInstrumentDataId
	       Identifier of the instrument to load. 0 <= aInstrumentId <= 127.
    @param aMemoryBankId
	       Identifier of the custom bank in memory to load the instrument into,
           occupying no more than 14 bits.
    @param aMemoryInstrumentId
	       Identifier of the instrument in memory to load the new instrument into.
		   0 <= aInstrumentId <= 127.
    */
	virtual void  MmcLoadCustomInstrumentDataL(const TDesC8& aInstrumentData, TInt aBankDataId, TInt aInstrumentDataId, TInt aMemoryBankId, TInt aMemoryInstrumentId) = 0;
    /**
    Removes an instrument from custom sound bank memory.
    Only valid for instruments previously loaded from file.
    Once unloaded the instrument is no longer available for use.
    
    @param aCustomBankId
	       Identifier of the custom sound bank containing the instrument 
           to unload, occupying no more than 14 bits.
    @param aInstrumentId
	       Identifier of the instrument to unload. 0 <= aInstrumentId <= 127.
    */
	virtual void  MmcUnloadCustomInstrumentL(TInt aCustomBankId,TInt aInstrumentId) = 0;
    /**
    Gets the name of a particular percussion key corresponding to a given note.
    
    @param aNote
	       Note to query. 0 <= aNote <= 127.
    @param aBankId
	       Identifier of the bank that the instrument belongs to, occupying no more than 14 bits.
           The bank ID is a concatenation of MIDI bank MSB and LSB values.
    @param aCustom
	       Specifies whether to reference a custom or standard sound bank.
    @param aInstrumentId
	       Identifier of an instrument.
    @return Descriptor containing the name of the percussion key.
            If the key does not have a name then an empty descriptor is returned.
    */
	virtual const TDesC& MmcPercussionKeyNameL(TInt aNote, TInt aBankId, TBool aCustom, TInt aInstrumentId) = 0;
    /**
    Get the stop time currently set for the MIDI resource.
    
    @param aStopTime
	       Time at which playback will stop, relative to the start of the resource.
    @return Descriptor containing the name of the percussion key. If the key
            does not have a name then an empty descriptor is returned.
    */
	virtual void  MmcStopTimeL(TTimeIntervalMicroSeconds& aStopTime) = 0;
    /**
    Sets the stop time to use for the currently open MIDI resource
    
    @param aStopTime
	       Time at which playback will stop, relative to the start of the resource.
           Clamped to 0 and the duration of the resource.
    */
	virtual void  MmcSetStopTimeL(const TTimeIntervalMicroSeconds& aStopTime) = 0;
    /**
    Gets the number of currently active voices.
    
    @param aNumNotes
	       The number of currently active voices.
    */
	virtual void  MmcPolyphonyL(TInt& aNumNotes) = 0;
    /**
    Get the maximum number of logical channels supported by the MIDI engine.
    
    @param aChannels 
	       The maximum number of logical channels that the MIDI engine supports, 0 <= aChannels <=15.
    */
	virtual void  MmcChannelsSupportedL(TInt& aChannels) = 0;
    /**
    Get the current volume setting of a logical channel.
    
    @param aChannel
	       Logical channel to query. 0 <= aChannel <= 15.
    @param aChannelVol
	       Volume currently set on the specified channel in decibels.
    */
	virtual void  MmcChannelVolumeL(TInt aChannel, TReal32& aChannelVol) = 0;
    /**
    Gets the Maximum volume setting that may be applied to a logical channel.
    
    @param aMaxVol
	       Maximum volume setting. Minimum value is -infinity dB, which is 
	       the smallest possible value that TReal32 supports.
    */
	virtual void  MmcMaxChannelVolumeL(TReal32& aMaxVol) = 0;
    /**
    Set the volume of a channel.
    
    @param aChannel
	       Logical channel to set the volume on. 0 <= aChannel <= 15.
    @param aVolume
	       The channel volume can be set within a range. The minimum channel 
	       volume is -infinity dB, which is the smallest possible value that
	       TReal32 supports while the maximum channel volume is set via MaxVolumeL()
	       which represents the volume level in dB corresponding to the MIDI Channel 
	       Volume controller.
    */
	virtual void  MmcSetChannelVolumeL(TInt aChannel,TReal32 aVolume) = 0;
    /**
    Set the muting state of a channel without changing its volume setting.
    When unmuted the channel goes back to its previous volume setting.
    
    @param aChannel
	       Logical channel to set the mute state of. 0 <= aChannel <= 15.
    @param aMuted
	       ETrue to mute the channel, EFalse to unmute it.
    */
	virtual void  MmcSetChannelMuteL(TInt aChannel,TBool aMuted) = 0;
    /**
    Gets the overall volume of the MIDI client.
    
    @param aVolume
	       The current overall volume setting.
    */
	virtual void  MmcVolumeL(TInt& aVolume) = 0;
    /**
    Maximum volume setting that may be applied overall.
    
    @param aMaxVolume
	       Maximum volume setting. Minimum value is always zero which is silent.
    */
	virtual void  MmcMaxVolumeL(TInt& aMaxVolume) = 0;
    /**
    Set the overall volume of the MIDI client.
    This setting scales all channel volumes respectively so the actual volume
    that a channel is played at becomes (overall volume * channel volume / max volume).
    
    @param aVolume Overall volume setting to use.
    */
	virtual void  MmcSetVolumeL(TInt aVolume) = 0;
    /**
    Length of time over which the volume is faded up from zero to the current settings
    when playback is started.
    
    @param aRampDuration
	       Duration of the ramping period.
    */
	virtual void  MmcSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration) = 0;
    /**
    Get the current stereo balance value.
    
    @param aBalance
	       Balance value ranging from KMMFBalanceMaxLeft to KMMFBalanceMaxRight.
    */
	virtual void  MmcGetBalanceL(TInt& aBalance) = 0;
    /**
    Set the current stereo balance value.
    
    @param aBalance
	       Balance value to set. Defaults to KMMFBalanceCenter to restore equal left-right balance.
    */
	virtual void  MmcSetBalanceL(TInt aBalance) = 0;
    /**
    Set the max polyphony level the engine can handle.
    
    @param aMaxNotes
	       Max polyphony level,  0 <= PolyphonyL() <= aMaxNotes.
    */
	virtual void  MmcSetMaxPolyphonyL(TInt aMaxNotes) = 0;
    /**
    Gets the number of times the current opened resources have to be repeated.
    
    @param aNumRepeats
	       The number of time the current opened resources have to be repeated.
    */
	virtual void  MmcGetRepeatsL(TInt& aNumRepeats) = 0;
    /**
    Set the number of times to repeat the current MIDI resource.
    After Stop() has been called, repeat number of times and the trailing silence are reset.
    
    @param aRepeatNumberOfTimes
	       Number of times to repeat the resource during playback. This includes the first playing.
    @param aTrailingSilence 
	       Time in microseconds to pause between repeats.
    */
	virtual void  MmcSetRepeatsL(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence) = 0;
    /**
    Tell the MIDI engine to use a custom bank or a standard bank.
    
    @param aCustom
	       If Etrue the custom bank in memory is used otherwise the standard bank 
		   is used leaving the custom bank in memory.
    */
	virtual void  MmcSetBankL(TBool aCustom) = 0;
    /**
    Gets the muting status of a specific track.
    
    @param aTrack
	       The track to query.
    @param aTrackMute
	       The mute status of the track.
    */
	virtual void  MmcIsTrackMuteL(TInt aTrack, TBool& aTrackMute) = 0;
    /**
    Gets the muting status of a specific channel.
    
    @param aChannel
	       The channel to query.
    @param aChannelMute
	       The mute status of the channel.
    */
	virtual void  MmcIsChannelMuteL(TInt aChannel, TBool& aChannelMute) = 0;
    /**
    Gets the instrument assigned to a specified channel.
    
    @param aChannel
	       Logical channel, 0 <= aChannel <= 15.
    @param aInstrumentId
	       Identifier of the instrument assigned to aChannel. 0 <= aInstrumentId <= 127.
    @param aBankId
	       Identifier of the bank that the instrument belongs to, occupying no more than 14 bits.
    */
	virtual void  MmcGetInstrumentL(TInt aChannel, TInt& aInstrumentId, TInt& aBankId) = 0;
    /**
    Closes any currently open resources, such as files, descriptors or URLs in use.
    Does nothing if there is nothing currently open.
    */
	virtual void  MmcCloseL() = 0;
    /**
    Stops playback of a resource but does not change the current position or release any resources.
    Pauses the internal timer if no resource is open.
    
    @param aFadeOutDuration
	       Length of time over which the volume is faded out from the current settings to zero.
    */
	virtual void  MmcStopL(const TTimeIntervalMicroSeconds& aFadeOutDuration) = 0;
    /**
    Gets the max polyphony level the engine can handle.
    
    @param aNumNotes
	       Max polyphony level,  PolyphonyL() <= aMaxNotes.
    */
	virtual void  MmcMaxPolyphonyL(TInt& aMaxNotes) = 0;
	};


/**
Mixin class to define an object capable of handling a MIDI event within the controller framework.

@publishedAll
@released
*/
class MMidiEventHandler
	{
public:
    /**
    Send an event to the client.
	
    @param aEvent
	       The MIDI event.
	@return One of the system-wide error codes.
    */
	virtual TInt SendMidiEventToClient(const CMMFMidiEvent& aEvent) = 0;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/midieventreceiver.h>
#endif

class CMidiEventReceiver;

/**
Custom command parser class to be used by controller plugins wishing to support
MIDI controller commands.
The controller plugin must be derived from MMidiCustomCommandImplementor
to use this class. The controller plugin should create an object of this type and add 
it to the list of custom command parsers in the controller framework.

@publishedAll
@released
*/
class CMidiCustomCommandParser : public CMMFCustomCommandParserBase,
								 public MMidiEventHandler	
	{
public:
	IMPORT_C static CMidiCustomCommandParser* NewL(MMidiCustomCommandImplementor& aImplementor);
	IMPORT_C ~CMidiCustomCommandParser();
	/**
    @internalTechnology
    */
	void HandleRequest(TMMFMessage& aMessage);
	virtual TInt SendMidiEventToClient(const CMMFMidiEvent& aEvent);
private:
	CMidiCustomCommandParser(MMidiCustomCommandImplementor& aImplementor);
	// Internal request handling methods.
	void DoHandleRequestL(TMMFMessage& aMessage);

	TBool DoSetPositionMicroBeatsL(TMMFMessage& aMessage);
	TBool DoPositionMicroBeatsL(TMMFMessage& aMessage);
	TBool DoPlayNoteL(TMMFMessage& aMessage);
	TBool DoPlayNoteWithStartTimeL(TMMFMessage& aMessage);
	TBool DoStopNotesL(TMMFMessage& aMessage);
	TBool DoNoteOnL(TMMFMessage& aMessage);
	TBool DoNoteOffL(TMMFMessage& aMessage);
	TBool DoPlaybackRateL(TMMFMessage& aMessage);
	TBool DoSetPlaybackRateL(TMMFMessage& aMessage);
	TBool DoMaxPlaybackRateL(TMMFMessage& aMessage);
	TBool DoMinPlaybackRateL(TMMFMessage& aMessage);
	TBool DoTempoMicroBeatsPerMinuteL(TMMFMessage& aMessage);
	TBool DoSetTempoL(TMMFMessage& aMessage);
	TBool DoPitchTranspositionCentsL(TMMFMessage& aMessage);
	TBool DoSetPitchTranspositionL(TMMFMessage& aMessage);
	TBool DoDurationMicroBeatsL(TMMFMessage& aMessage);
	TBool DoNumTracksL(TMMFMessage& aMessage);
	TBool DoSetTrackMuteL(TMMFMessage& aMessage);
	TBool DoMimeTypeL(TMMFMessage& aMessage);
	TBool DoSetSyncUpdateCallbackIntervalL(TMMFMessage& aMessage);
	TBool DoSendMessageL(TMMFMessage& aMessage);
	TBool DoSendMessageWithTimeStampL(TMMFMessage& aMessage);
	TBool DoSendMipMessageL(TMMFMessage& aMessage);
	TBool DoNumberOfBanksL(TMMFMessage& aMessage);
	TBool DoGetBankIdL(TMMFMessage& aMessage);
	TBool DoLoadCustomBankL(TMMFMessage& aMessage);
	TBool DoLoadCustomBankDataL(TMMFMessage& aMessage);
	TBool DoUnloadCustomBankL(TMMFMessage& aMessage);
	TBool DoCustomBankLoadedL(TMMFMessage& aMessage);
	TBool DoUnloadAllCustomBanksL(TMMFMessage& aMessage);
	TBool DoNumberOfInstrumentsL(TMMFMessage& aMessage);
	TBool DoGetInstrumentIdL(TMMFMessage& aMessage);
	TBool DoInstrumentNameL(TMMFMessage& aMessage);
	TBool DoCopyInstrumentNameL(TMMFMessage& aMessage);
	TBool DoSetInstrumentL(TMMFMessage& aMessage);
	TBool DoLoadCustomInstrumentL(TMMFMessage& aMessage);
	TBool DoLoadCustomInstrumentDataL(TMMFMessage& aMessage);
	TBool DoUnloadCustomInstrumentL(TMMFMessage& aMessage);
	TBool DoPercussionKeyNameL(TMMFMessage& aMessage);
	TBool DoCopyPercussionKeyNameL(TMMFMessage& aMessage);
	TBool DoStopTimeL(TMMFMessage& aMessage);
	TBool DoSetStopTimeL(TMMFMessage& aMessage);
	TBool DoPolyphonyL(TMMFMessage& aMessage);
	TBool DoChannelsSupportedL(TMMFMessage& aMessage);
	TBool DoChannelVolumeL(TMMFMessage& aMessage);
	TBool DoMaxChannelVolumeL(TMMFMessage& aMessage);
	TBool DoSetChannelVolumeL(TMMFMessage& aMessage);
	TBool DoSetChannelMuteL(TMMFMessage& aMessage);
	TBool DoVolumeL(TMMFMessage& aMessage);
	TBool DoMaxVolumeL(TMMFMessage& aMessage);
	TBool DoSetVolumeL(TMMFMessage& aMessage);
	TBool DoSetVolumeRampL(TMMFMessage& aMessage);
	TBool DoGetBalanceL(TMMFMessage& aMessage);
	TBool DoSetBalanceL(TMMFMessage& aMessage);
	TBool DoSetMaxPolyphonyL(TMMFMessage& aMessage);
	TBool DoGetRepeatsL(TMMFMessage& aMessage);
	TBool DoSetRepeatsL(TMMFMessage& aMessage);
	TBool DoSetBankL(TMMFMessage& aMessage);
	TBool DoIsTrackMuteL(TMMFMessage& aMessage);
	TBool DoIsChannelMuteL(TMMFMessage& aMessage);
	TBool DoGetInstrumentL(TMMFMessage& aMessage);
	TBool DoCloseL(TMMFMessage& aMessage);
	TBool DoStopL(TMMFMessage& aMessage);
	TBool DoReceiveEventsL(TMMFMessage& aMessage);
	TBool DoRetrieveEventL(TMMFMessage& aMessage);
	TBool DoCancelReceiveEventsL(TMMFMessage& aMessage);

	void DoCreateBufFromUintArrayL(RArray<TUint>& aArray);
	void DoCreateBufFromFourCCArrayL(RArray<TFourCC>& aArray);
	TBool DoMaxPolyphonyL(TMMFMessage& aMessage);
private:
	/** The object that implements the MIDI controller interface */
	MMidiCustomCommandImplementor& iImplementor;
	
	CMidiEventReceiver* iMidiEventReceiver;
	/** The events waiting to be sent to the client.*/
	RPointerArray<CMMFMidiEvent> iMidiEvents;

	// variable length data waiting to be copied back to the client
	CBufFlat* iInstrumentName;
	CBufFlat* iPercussionKeyName;
	};

#endif
