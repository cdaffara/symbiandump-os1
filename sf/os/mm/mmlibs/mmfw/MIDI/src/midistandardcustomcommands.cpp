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

#include <mmf/common/midistandardcustomcommands.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfmidiconfig.h>
#include <mmf/common/midieventreceiver.h>
#endif

/**
Constructor.
@param aController	
       The client side controller object to be used by this custom command interface.
*/
EXPORT_C RMidiControllerCustomCommands::RMidiControllerCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMidi)
	{
	}

/**
Change the position of the currently playing MIDI resource to the given position.
May be called whenever a MIDI resource is open.

@param aMicroBeats 
       Metrical position to move to. Clamped to (0, DurationMicroBeats()).
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetPositionMicroBeats(TInt64 aMicroBeats) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iPositionMicroBeats = aMicroBeats;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerSetPositionMicroBeats, 
										 configPackage,
										 KNullDesC8);
	}

/**
Gets the current metrical position of the MIDI resource being played.

@param aMicroBeats 
       (BPM*1000000) relative to the start of the resource.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::PositionMicroBeats(TInt64& aMicroBeats) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
											   EMMFMidiControllerPositionMicroBeats, 
											   KNullDesC8,
											   KNullDesC8,
											   configPackage);
	if (!error)
		aMicroBeats = configPackage().iPositionMicroBeats;
	return error;
	}

/**
Synchronous function to play a single note.
Multiple calls to this function will be accommodated as far as the MIDI engine can
manage. The same functionality could be implemented using the SendMessage function.

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
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::PlayNote(TInt aChannel,TInt aNote, const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	configPackage().iNote = aNote;
	configPackage().iDurationMicroSeconds = aDuration;
	configPackage().iNoteOnVelocity = aNoteOnVelocity;
	configPackage().iNoteOffVelocity = aNoteOffVelocity;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerPlayNote, 
										 configPackage,
										 KNullDesC8);
	}

/**
Synchronous function to play a single note at a specified time.
Multiple calls to this function will be accommodated as far as the MIDI engine can
manage. The same functionality could be implemented using the SendMessage function.

@param aChannel 
       Logical channel to play note on. 0 <= aChannel <= 15.
@param aNote 
       Note to play. 0 <= aNote <= 127
@param aStartTime 
       Specifies the time at which to start playing the note, relative 
	   to the MIDI resource playing time or the time elapsed since Play()
	   was called if no resource is present.
@param aDuration 
       Length of time to play note for.
@param aNoteOnVelocity 
       Velocity with which to start the note. 0 <= aNoteOnVelocity <= 127.
@param aNoteOffVelocity 
       Velocity with which to stop the note. 0 <= aNoteOffVelocity <= 127.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::PlayNote(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aStartTime, const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	configPackage().iNote = aNote;
	configPackage().iStartTime = aStartTime;
	configPackage().iDurationMicroSeconds = aDuration;
	configPackage().iNoteOnVelocity = aNoteOnVelocity;
	configPackage().iNoteOffVelocity = aNoteOffVelocity;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerPlayNoteWithStartTime, 
										 configPackage,
										 KNullDesC8);
	}

/**
Stops the playback of all notes on the given channel, by means of an All Notes Off MIDI message.

@param aChannel 
       Logical channel to stop notes on. 0 <= aChannel <= 15.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::StopNotes(TInt aChannel)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerStopNotes, 
										 configPackage,
										 KNullDesC8);
	}

/**
Synchronous function to commence playback of a note. Multiple calls to this
function will be accommodated as far as the MIDI engine can manage.

@param aChannel 
       Logical channel to play note on. 0 <= aChannel <= 15.
@param aNote 
       Note to play. 0 <= aNote <= 127.
@param aVelocity 
       Velocity with which to start the note. The legal integer range is
	   0 <= aVelocity <= 127, but the value zero actually causes the message
	   to be interpreted as a Note Off message instead of a Note On.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::NoteOn(TInt aChannel, TInt aNote, TInt aVelocity)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	configPackage().iNote = aNote;
	configPackage().iNoteOnVelocity = aVelocity;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerNoteOn, 
										 configPackage,
										 KNullDesC8);
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
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::NoteOff(TInt aChannel,TInt aNote,TInt aVelocity)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	configPackage().iNote = aNote;
	configPackage().iNoteOffVelocity = aVelocity;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerNoteOff, 
										 configPackage,
										 KNullDesC8);
	}

/**
Gets the current playback rate factor of the currently open MIDI resource.
The playback rate is independent from tempo, i.e., it can be used to give
an overall speed factor for playback.

@param aPlayBackRate 
       Current playback rate in percent times 1000, i.e., 100000 means original
	   playback speed, 200000 means double speed, and 50000 means half speed playback.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::PlaybackRate(TInt& aPlayBackRate) const
	{
	TPckgBuf<TMMFMidiConfig3> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
											   EMMFMidiControllerPlaybackRate, 
											   KNullDesC8,
											   KNullDesC8,
											   configPackage);
	if (!error)
		aPlayBackRate = configPackage().iPlayBackRate;
	return error;
	}

/**
Sets the playback rate for the playback of the current MIDI resource.
The playback rate is independent from tempo, i.e., it can be used to give
an overall speed factor for playback. May be called whether playback
is in progress or not.

@param aPlayBackRate 
       Playback rate in percent times 1000, i.e., 100000 means original 
	   playback speed, 200000 means double speed, and 50000 means half speed playback.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetPlaybackRate(TInt aPlayBackRate)
	{
	TPckgBuf<TMMFMidiConfig3> configPackage;
	configPackage().iPlayBackRate = aPlayBackRate;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerSetPlaybackRate, 
										 configPackage,
										 KNullDesC8);
	}

/**
Gets the maximum playback rate in milli-percentage from the MIDI engine.
@see SetPlaybackRate() for milli-percentage details.

@param aMaxRate
       Playback rate supported by MIDI player.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::MaxPlaybackRate(TInt& aMaxRate) const
	{
	TPckgBuf<TMMFMidiConfig3> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerMaxPlaybackRate, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aMaxRate = configPackage().iPlayBackMaxRate;
	return error;
	}

/**
Gets the minimum playback rate in milli-percentage from the MIDI engine.
@see SetPlaybackRate() for milli-percentage details.

@param aMinRate
       Minimum playback rate supported by MIDI player.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::MinPlaybackRate(TInt& aMinRate) const
	{
	TPckgBuf<TMMFMidiConfig3> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerMinPlaybackRate, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aMinRate = configPackage().iPlayBackMinRate;
	return error;
	}

/**
Gets the current tempo of the currently open MIDI resource. The tempo is independent
from the playback rate, i.e., the resulting playback speed will be affected by both.

@param  aMicroBeatsPerMinute
        Tempo at the current position of the currently open resource in microbeats
		per minute, i.e. BPM * 1000000. Filled in by the controller framework.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::TempoMicroBeatsPerMinute(TInt& aMicroBeatsPerMinute) const
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerTempo, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aMicroBeatsPerMinute = configPackage().iTempo;
	return error;
	}

/**
Sets the tempo at which the current MIDI resource should be played. May be
called whether playback is in progress or not. The tempo is independent 
from the playback rate, i.e., the resulting playback speed will be affected by both.

@param aMicroBeatsPerMinute 
       Tempo in microbeats per minute (BPM*1000000) to set.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetTempo(TInt aMicroBeatsPerMinute)
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	configPackage().iTempo = aMicroBeatsPerMinute;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerSetTempo, 
										 configPackage,
										 KNullDesC8);
	}

/**
Gets the pitch shift in use for the currently open MIDI resource.

@param aPitch 
       Shift in cents, i.e. semitones * 100. One octave equals 1200 cents.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::PitchTranspositionCents(TInt& aPitch) const
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerPitch, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aPitch = configPackage().iPitch;
	return error;
	}

/**
Sets the pitch shift to apply to the currently open MIDI resource.
May be called during playback.

@param aCents 
       Pitch shift in cents, i.e. semitones * 100. One octave equals 1200 cents.
@param aCentsApplied 
       Actual pitch shift applied - may differ from the requested value due to
	   limitations of the MIDI engine.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetPitchTransposition(TInt aCents, TInt& aCentsApplied)
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	configPackage().iPitch = aCents;
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
											EMMFMidiControllerSetPitch, 
											configPackage,
											KNullDesC8,
											configPackage);
	aCentsApplied = configPackage().iPitch;
	return err;
	}

/**
Gets the length of the currently open MIDI resource in micro-beats.

@param aDuration 
       Duration in microbeats (beats * 1000000).
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::DurationMicroBeats(TInt64& aDuration) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerDurationMicroBeats, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aDuration = configPackage().iDurationMicroBeats;
	return error;
	}

/**
Gets the number of tracks present in the currently open MIDI resource.

@param aTracks 
       Number of tracks.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::NumTracks(TInt& aTracks) const
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerNumTracks, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aTracks = configPackage().iNumTracks;
	return error;
	}

/**
Mutes or unmutes a particular track.

@param aTrack 
       Index of the track to mute - 0 <= aTrack < NumTracksL().
@param aMuted 
       ETrue to mute the track, EFalse to unmute it.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetTrackMute(TInt aTrack, TBool aMuted) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iTrack = aTrack;
	configPackage().iMuted = aMuted;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerSetTrackMute, 
										 configPackage,
										 KNullDesC8);
	}

/**
Gets the MIME type of the MIDI resource currently open.

@param aMimeType 
       Descriptor containing the MIDI mime type.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::MimeType(TDes8& aMimeType) const
	{
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerMimeType, 
										KNullDesC8,										 
										KNullDesC8,
										aMimeType);
	}

/**
Sets the frequency at which MMIDIClientUtilityObserver::MmcuoSyncUpdateL() is called
to allow other components to synchronise with playback of this MIDI resource.

@param aMicroSeconds 
       Temporal interval to callback at. Used in preference to aMicroBeats if both are set.
@param aMicroBeats 
       Metrical interval to callback at. Set both parameters to zero to cancel.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetSyncUpdateCallbackInterval(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats)
	{
	TPckgBuf<TMMFMidiConfig3> configPackage;
	configPackage().iCallbackIntervalMicroSeconds = aMicroSeconds;
	configPackage().iCallbackIntervalMicroBeats = aMicroBeats;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerSetSyncUpdateCallbackInterval, 
										 configPackage,
										 KNullDesC8);
	}

/**
Sends a single MIDI message to the MIDI engine.

@param aMidiMessage 
       Descriptor containing the MIDI message data. If there are several
       MIDI messages in the buffer, only the first one is processed.
@param aBytes
       Number of bytes of the message buffer actually processed.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SendMessage(const TDesC8& aMidiMessage, TInt& aBytes)
	{
	TPckgBuf<TMMFMidiConfig3> configPackage;
	configPackage().iMidiMessage = &aMidiMessage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerSendMessage, 
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aBytes = configPackage().iBytesProcessed;
	return error;
	}

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
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SendMessage(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& aTime, TInt& aBytes)
	{
	TPckgBuf<TMMFMidiConfig3> configPackage;
	configPackage().iMidiMessage = &aMidiMessage;
	configPackage().iTimeStamp = aTime;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerSendMessageWithTimeStamp, 
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aBytes = configPackage().iBytesProcessed;
	return error;
	}

/**
Sends a mip message to the MIDI engine. This is a convenience function,
because the same functionality could be achieved with the SendMessage() function.

@param aEntry 
       Array of logical {channel, MIP} value pairs to send, highest priority first.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SendMipMessage(const RArray<TMipMessageEntry>& aEntry)
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TArray<TMipMessageEntry> mipMessage(aEntry.Array());
	configPackage().iMipMessage = &mipMessage;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFMidiControllerSendMipMessage, 
										 configPackage,
										 KNullDesC8);
	}

/**
Gets the number of standard or custom sound banks currently available.

@param aCustom
       Specifies whether to reference a custom or standard sound bank.
@param aNumBanks 
       Number of custom or standard sound banks available.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::NumberOfBanks(TBool aCustom, TInt& aNumBanks) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iCustom = aCustom;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerNumberOfBanks, 
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aNumBanks = configPackage().iNumBanks;
	return error;
	}

/**
Gets the identifier of a sound bank. Bank identifier (aka bank number) is a
14-bit value consisting of MIDI bank MSB and LSB values.

@param  aCustom
        Specifies whether to reference a custom or standard sound bank.
@param  aBankIndex 
        Index of sound bank where 0 <= aBankIndex < NumberOfBanks().
@param  aBankId
        Identifier of the specified bank occupying, at most, 14 bits.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::GetBankId(TBool aCustom, TInt aBankIndex, TInt& aBankId) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iCustom = aCustom;
	configPackage().iBankIndex = aBankIndex;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerGetBankId, 
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aBankId = configPackage().iBankId;
	return error;
	}


/**
Loads one or more custom sound banks from a file into memory for use.
If several banks are loaded with consequent LoadCustomBanks() function calls,
the banks are combined if the bank sets have conflicting bank numbers.

@param aFileName 
       Name of the file containing the custom sound bank.
@param aBankCollectionIndex
       Identifier of the custom sound bank loaded, occupying no more than 14 bits.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::LoadCustomBank(const TDesC& aFileName,TInt& aBankCollectionIndex)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iFileName = &aFileName;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerLoadCustomBank, 
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aBankCollectionIndex = configPackage().iBankId;
	return error;
	}


/**
Loads one or more custom sound banks from a descriptor into memory for use.
If several banks are loaded with consequent LoadCustomBanks() function calls,
the banks are combined if the bank sets have conflicting bank numbers.

@param aBankData 
       Descriptor containing the custom sound bank.
@param aBankCollectionIndex
       Identifier of the custom sound bank loaded, occupying no more than 14 bits.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::LoadCustomBankData(const TDesC8& aBankData,TInt& aBankCollectionIndex)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iBankData = &aBankData;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerLoadCustomBankData, 
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aBankCollectionIndex = configPackage().iBankId;
	return error;
	}


/**
Removes a custom sound bank from memory. Only valid for sound banks previously
loaded from file. Once unloaded the custom sound bank is no longer available for use.

@param aBankCollectionIndex 
       Identifier of the custom sound bank to unload,occupying no more than 14 bits.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::UnloadCustomBank(TInt aBankCollectionIndex)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iBankId= aBankCollectionIndex;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerUnloadCustomBank, 
										configPackage,
										KNullDesC8);
	}

/**
Query if a bank has been loaded to the memory.

@param aBankCollectionIndex
       Identifier of the custom sound bank to check if it's in memory or not.
@param aBankLoaded 
       ETrue if the specified bank is in memory, EFalse otherwise.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::CustomBankLoaded(TInt aBankCollectionIndex, TBool& aBankLoaded) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iBankId= aBankCollectionIndex;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerCustomBankLoaded, 
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aBankLoaded = configPackage().iBankLoaded;
	return error;
	}


/**
Removes all custom sound banks from memory.

@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::UnloadAllCustomBanks()
	{
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerUnloadAllCustomBanks, 
										KNullDesC8,
										KNullDesC8);
	}

/**
Gets the number of instruments available in a given sound bank.

@param aBankId
       Identifier of sound bank to reference, occupying no more than 14 bits.
@param aCustom
       Specifies whether to reference a custom or standard sound bank.
@param aNumInstruments 
       Count of the number of instruments available for the specified sound bank.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::NumberOfInstruments(TInt aBankId, TBool aCustom, TInt& aNumInstruments) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iBankId = aBankId;
	configPackage().iCustom = aCustom;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerNumberOfInstruments, 
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aNumInstruments = configPackage().iNumInstruments;
	return error;
	}


/**
Gets the identifier of an instrument.

@param aBankId 
       Identifier of the sound bank to reference, occupying no more than 14 bits.
@param aCustom 
       Specifies whether to reference a custom or standard sound bank.
@param aInstrumentIndex 
       Index of the instrument to reference where 0 <= aInstrumentIndex < NumberOfInstruments(). 
@param aInstrumentId 
       Identifier of specified instrument. This may differ from the index since the index
	   simply enumerates the instruments, whereas identifiers may not be contiguous,
	   especially where certain instruments correspond to General MIDI-defined instruments
	   but not all instruments are present. Instrument identifiers are between 0 and 127 inclusive.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::GetInstrumentId(TInt aBankId, TBool aCustom, TInt aInstrumentIndex, TInt& aInstrumentId) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iBankId = aBankId;
	configPackage().iCustom = aCustom;
	configPackage().iInstrumentIndex = aInstrumentIndex;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
									 			EMMFMidiControllerGetInstrumentId, 
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aInstrumentId = configPackage().iInstrumentId;
	return error;
	}


/**
Gets the name of the given instrument.

@param  aBankId 
        Identifier of the bank that the instrument belongs to, occupying no more than 14 bits.
@param  aCustom 
        Specifies whether to reference a custom or standard sound bank.
@param  aInstrumentId 
        Identifier of the instrument under scrutiny. 0 <= aInstrumentId <= 127.
@return Buffer containing the name of the specified instrument. If it has no name
        then an empty descriptor is returned.
*/
EXPORT_C HBufC* RMidiControllerCustomCommands::InstrumentNameL(TInt aBankId, TBool aCustom, TInt aInstrumentId) const
	{
	// First, get the size of the instrument name so we can create a descriptor big enough to hold it
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iBankId = aBankId;
	configPackage().iCustom = aCustom;
	configPackage().iInstrumentId = aInstrumentId;
	TPckgBuf<TInt> descriptorSizePckg;
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
													 EMMFMidiControllerInstrumentName,
													 configPackage,
													 KNullDesC8,
													 descriptorSizePckg));

	// Now create a descriptor of appropriate size and get the server to copy the data into it
	HBufC8* instrumentNameBuffer = HBufC8::NewLC(descriptorSizePckg());
	TPtr8 instrumentNameBufferPtr = instrumentNameBuffer->Des();
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
													 EMMFMidiControllerCopyInstrumentName,
													 KNullDesC8,
													 KNullDesC8,
													 instrumentNameBufferPtr));

	// Stream data out of the 8bit descriptor into a 16bit descriptor
	RDesReadStream stream;
	stream.Open(*instrumentNameBuffer);
	CleanupClosePushL(stream);

	HBufC* instrumentName = HBufC::NewL(stream, KMaxTInt);
	
	CleanupStack::PopAndDestroy();//stream
	CleanupStack::PopAndDestroy(instrumentNameBuffer);

	return instrumentName;
	}


/**
Sets a logical channel to use the given instrument.

@param aChannel 
       Logical channel to set the instrument for. 0 <= aChannel <= 15.
@param aBankId 
       Identifier of the bank that the instrument belongs to, occupying no more than 14 bits.
       The bank ID is a concatenation of MIDI bank MSB and LSB values.
@param aInstrumentId 
       Identifier of the instrument under scrutiny. 0 <= aInstrumentId <= 127.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetInstrument(TInt aChannel,TInt aBankId,TInt aInstrumentId)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	configPackage().iBankId = aBankId;
	configPackage().iInstrumentId = aInstrumentId;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetInstrument,
										configPackage,
										KNullDesC8);
	}

/**
Loads an individual instrument from file into custom sound bank memory for use.
The bank and instrument id given in the file can be mapped into different bank
and instrument id in memory.

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
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::LoadCustomInstrument(const TDesC& aFileName, TInt aFileBankId, TInt aFileInstrumentId, TInt aMemoryBankId, TInt aMemoryInstrumentId)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iFileName = &aFileName;
	configPackage().iBankId = aFileBankId;
	configPackage().iInstrumentId = aFileInstrumentId;
	configPackage().iMemoryBankId = aMemoryBankId;
	configPackage().iMemoryInstrumentId = aMemoryInstrumentId;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerLoadCustomInstrument,
										configPackage,
										KNullDesC8);
	}


/**
Loads an individual instrument from descriptor into custom sound bank memory for use.
The bank and instrument id given in the descriptor can be mapped into different bank
and instrument id in memory.

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
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::LoadCustomInstrumentData(const TDesC8& aInstrumentData, TInt aBankDataId, TInt aInstrumentDataId, TInt aMemoryBankId, TInt aMemoryInstrumentId)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iInstrumentData = &aInstrumentData;
	configPackage().iBankId = aBankDataId;
	configPackage().iInstrumentId = aInstrumentDataId;
	configPackage().iMemoryBankId = aMemoryBankId;
	configPackage().iMemoryInstrumentId = aMemoryInstrumentId;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerLoadCustomInstrumentData,
										configPackage,
										KNullDesC8);
	}

/**
Removes an instrument from custom sound bank memory. Only valid for
instruments previously loaded from file. Once unloaded the instrument
is no longer available for use.

@param aCustomBankId 
       Identifier of the custom sound bank containing the instrument to unload,
	   occupying no more than 14 bits.
@param aInstrumentId
       Identifier of the instrument to unload. 0 <= aInstrumentId <= 127.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::UnloadCustomInstrument(TInt aCustomBankId,TInt aInstrumentId)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iBankId = aCustomBankId;
	configPackage().iInstrumentId = aInstrumentId;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerUnloadCustomInstrument,
										configPackage,
										KNullDesC8);
	}

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
@return Descriptor containing the name of the percussion key. If the key
        does not have a name then an empty descriptor is returned.
*/
EXPORT_C HBufC* RMidiControllerCustomCommands::PercussionKeyNameL(TInt aNote, TInt aBankId, TBool aCustom, TInt aInstrumentId) const
	{
	// First, get the size of the percussion key name so we can create a descriptor big enough to hold it
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iNote = aNote;
	configPackage().iBankId = aBankId;
	configPackage().iCustom = aCustom;
	configPackage().iInstrumentId = aInstrumentId;
	TPckgBuf<TInt> descriptorSizePckg;
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
													 EMMFMidiControllerPercussionKeyName,
													 configPackage,
													 KNullDesC8,
													 descriptorSizePckg));

	// Now create a descriptor of appropriate size and get the server to copy the data into it
	HBufC8* keyNameBuffer = HBufC8::NewLC(descriptorSizePckg());
	TPtr8 keyNameBufferPtr = keyNameBuffer->Des();
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
													 EMMFMidiControllerCopyPercussionKeyName,
													 KNullDesC8,
													 KNullDesC8,
													 keyNameBufferPtr));

	// Stream data out of the 8bit descriptor into a 16bit descriptor
	RDesReadStream stream;
	stream.Open(*keyNameBuffer);
	CleanupClosePushL(stream);

	HBufC* keyName = HBufC::NewL(stream, KMaxTInt);
	
	CleanupStack::PopAndDestroy();//stream
	CleanupStack::PopAndDestroy(keyNameBuffer);
	
	return keyName;
	}

/**
Get the stop time currently set for the MIDI resource.

@param aStopTime
       Time at which playback will stop, relative to the start of the resource.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::StopTime(TTimeIntervalMicroSeconds& aStopTime) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerStopTime,
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aStopTime = configPackage().iStopTime;
	return error;
	}


/**
Sets the stop time to use for the currently open MIDI resource

@param aStopTime
       Time at which playback will stop, relative to the start of the resource.
       Clamped to 0 and the duration of the resource.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetStopTime(const TTimeIntervalMicroSeconds& aStopTime) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iStopTime = aStopTime;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetStopTime,
										configPackage,
										KNullDesC8);
	}

/**
Gets the polyphony of the MIDI engine.

@param aNumNotes 
       The number of currently active voices.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::Polyphony(TInt& aNumNotes) const
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
											EMMFMidiControllerPolyphony,
											KNullDesC8,
											KNullDesC8,
											configPackage);
	if (!error)
		aNumNotes = configPackage().iNumNotes;
	return error;
	}


/**
Gets the current maximum number of notes the engine can handle
This can be greater than the value returned by RMidiControllerCustomCommands::Polyphony

@param aNumNotes 
       The maximum number of notes the engine can handle
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::MaxPolyphony(TInt& aMaxNotes) const
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
											EMMFMidiControllerMaxPolyphony,
											KNullDesC8,
											KNullDesC8,
											configPackage);
	if (!error)
		aMaxNotes = configPackage().iMaxNotes;
	return error;
	}

/**
Get the maximum number of logical channels supported by the MIDI engine.

@param aChannels
       The maximum number of logical channels that the MIDI engine supports,
	   0 <= aChannels <=15.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::ChannelsSupported(TInt& aChannels) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
											EMMFMidiControllerChannelsSupported,
											KNullDesC8,
											KNullDesC8,
											configPackage);
	if (!error)
		aChannels = configPackage().iChannel;
	return error;
	}


/**
Get the current volume setting of a logical channel.

@param aChannel 
       Logical channel to query. 0 <= aChannel <= 15.
@param aChannelVol 
       Volume currently set on the specified channel in decibels.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::ChannelVolume(TInt aChannel, TReal32& aChannelVol) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerChannelVolume,
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aChannelVol = configPackage().iChannelVol;
	return error;
	}

/**
Gets the Maximum volume setting that may be applied to a logical channel.

@param aMaxVol 
       Maximum volume setting. Minimum value is -infinity dB, which is the 
       smallest possible value that TReal32 supports.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::MaxChannelVolume(TReal32& aMaxVol) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerMaxChannelVolume,
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aMaxVol = configPackage().iMaxChannelVol;
	return error;
	}

/**
Set the volume of a channel.

@param aChannel 
       Logical channel to set the volume on. 0 <= aChannel <= 15.
@param aVolume 
       The channel volume can be set within a range. The minimum 
       channel volume is -infinity dB, which is the smallest possible
       value that TReal32 supports while the maximum channel volume
       is set via MaxVolumeL() which represents the volume level in dB
       corresponding to the MIDI Channel Volume controller.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetChannelVolume(TInt aChannel,TReal32 aVolume)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	configPackage().iChannelVol = aVolume;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetChannelVolume,
										configPackage,
										KNullDesC8);
	}

/**
Set the muting state of a channel without changing its volume setting.
When unmuted the channel goes back to its previous volume setting.

@param aChannel 
       Logical channel to set the mute state of. 0 <= aChannel <= 15.
@param aMuted 
       ETrue to mute the channel, EFalse to unmute it.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetChannelMute(TInt aChannel,TBool aMuted)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	configPackage().iMuted = aMuted;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetChannelMute,
										configPackage,
										KNullDesC8);
	}


/**
Gets the overall volume of the MIDI client.

@param aVolume 
       The current overall volume setting.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::Volume(TInt& aVolume) const
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerVolume,
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aVolume = configPackage().iVolume;
	return error;
	}


/**
Maximum volume setting that may be applied overall.

@param aMaxVolume 
       Maximum volume setting. Minimum value is always zero which is silent.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::MaxVolume(TInt& aMaxVolume) const
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerMaxVolume,
										KNullDesC8,
										KNullDesC8,
										configPackage);
	if (!error)
		aMaxVolume = configPackage().iMaxVolume;
	return error;
	}


/**
Set the overall volume of the MIDI client.
This setting scales all channel volumes respectively so the actual volume
that a channel is played at is (overall volume * channel volume / max volume).

@param aVolume
       Overall volume setting to use.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetVolume(TInt aVolume)
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	configPackage().iVolume = aVolume;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetVolume,
										configPackage,
										KNullDesC8);
	}

/**
Length of time over which the volume is faded up from zero to the current setting
when playback is started.

@param aRampDuration 
       Duration of the ramping period.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	configPackage().iRampDuration = aRampDuration;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetVolumeRamp,
										configPackage,
										KNullDesC8);
	}

/**
Get the current stereo balance value.

@param aBalance 
       Balance value ranging from KMMFBalanceMaxLeft to KMMFBalanceMaxRight.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::GetBalance(TInt& aBalance) const
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerGetBalance,
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aBalance = configPackage().iBalance;
	return error;
	}

/**
Set the current stereo balance value.

@param aBalance 
       Balance value to set. Defaults to KMMFBalanceCenter to restore equal left-right balance.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetBalance(TInt aBalance)
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	configPackage().iBalance = aBalance;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetBalance,
										configPackage,
										KNullDesC8);
	}

/**
Set the max polyphony the engine can handle.

@param aMaxNotes 
       Max polyphony level,  1 <= Polyphony() <= aMaxNotes.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetMaxPolyphony(TInt aMaxNotes)
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	configPackage().iMaxNotes = aMaxNotes;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetMaxPolyphony,
										configPackage,
										KNullDesC8);
	}

/**
Gets the number of times the current opened resources has to be repeated.

@param aNumRepeats
       The number of times the current opened resources has to be repeated.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::GetRepeats(TInt& aNumRepeats) const
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerGetRepeats,
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aNumRepeats = configPackage().iNumRepeats;
	return error;
	}

/**
Set the number of times to repeat the current MIDI resource.
After Stop() has been called, repeat number of times and the
trailing silence are reset.

@param aRepeatNumberOfTimes 
       Number of times to repeat the resource during playback. This includes the first playing.
@param aTrailingSilence 
       Time in microseconds to pause between repeats.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	
	TPckgBuf<TMMFMidiConfig3> configPackage;
	configPackage().iRepeatNumberOfTimes = aRepeatNumberOfTimes;
	configPackage().iTrailingSilence = aTrailingSilence;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetRepeats,
										configPackage,
										KNullDesC8);
	}

/**
Tell the MIDI engine to use a custom bank or a standard bank.

@param aCustom
       If Etrue the custom bank in memory is used otherwise the standard bank
       is used leaving the custom bank in memory.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::SetBank(TBool aCustom)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iCustom = aCustom;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerSetBank,
										configPackage,
										KNullDesC8);
	}

/**
Gets the muting status of a specific track.

@param aTrack 
       The track to query.
@param aTrackMute 
       The mute status of the track.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::IsTrackMute(TInt aTrack, TBool& aTrackMute) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iTrack = aTrack;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerIsTrackMute,
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aTrackMute = configPackage().iMuted;
	return error;
	}


/**
Gets the muting status of a specific channel.

@param aChannel
       The channel to query.
@param aChannelMute
       The mute status of the channel.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::IsChannelMute(TInt aChannel, TBool& aChannelMute) const
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerIsChannelMute,
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		aChannelMute = configPackage().iMuted;
	return error;
	}


/**
Gets the instrument assigned to a specified channel.

@param aChannel 
       Logical channel, 0 <= aChannel <= 15.
@param aInstrumentId 
       Identifier of the instrument assigned to aChannel. 0 <= aInstrumentId <= 127.
@param aBankId 
       Identifier of the bank that the instrument belongs to, occupying no more than 14 bits.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::GetInstrument(TInt aChannel, TInt& aInstrumentId, TInt& aBankId)
	{
	TPckgBuf<TMMFMidiConfig2> configPackage;
	configPackage().iChannel = aChannel;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFMidiControllerGetInstrument,
												configPackage,
												KNullDesC8,
												configPackage);
	if (!error)
		{
		aInstrumentId = configPackage().iInstrumentId;
		aBankId = configPackage().iBankId;
		}
	return error;
	}

/**
Closes any currently open resources, such as files, descriptors or URLs in use.
Does nothing if there is nothing currently open.

@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::Close()
	{
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerClose,
										KNullDesC8,
										KNullDesC8);
	}

/**
Stops playback of a resource but does not change the current position
or release any resources. Pauses the internal timer if no resource is open.

@param aFadeOutDuration 
       Length of time over which the volume is faded out from the current setting to zero.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::Stop(const TTimeIntervalMicroSeconds& aFadeOutDuration)
	{
	TPckgBuf<TMMFMidiConfig1> configPackage;
	configPackage().iFadeOutDuration = aFadeOutDuration;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerStop,
										configPackage,
										KNullDesC8);
	}


/**
Start receiving events from the controller. This can only be called once the controller is open.

@param aSizeOfMidiEvent 
       The size of the MIDI event object.
@param aStatus 
       Status flag belonging to an active object that will have it's RunL() called
       when this request complete.
*/
EXPORT_C void RMidiControllerCustomCommands::ReceiveEvents(TPckgBuf<TInt>& aSizeOfMidiEvent, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(iDestinationPckg, 
									EMMFMidiControllerReceiveEvents,
									KNullDesC8,
									KNullDesC8,
									aSizeOfMidiEvent,
									aStatus);
	}

/**
Get the MIDI event from the MIDI controller.

@param aMidiEventPckg
       MIDI event.
@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::RetrieveEvent(TDes8& aMidiEventPckg)
	{
	return iController.CustomCommandSync(iDestinationPckg,
										 EMMFMidiControllerRetrieveEvent,
										 KNullDesC8,
										 KNullDesC8,
										 aMidiEventPckg);
	}

/**
Stop receiving events from the MIDI controller.

@return One of the system-wide error codes.
*/
EXPORT_C TInt RMidiControllerCustomCommands::CancelReceiveEvents()
	{
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFMidiControllerCancelReceiveEvents,
										KNullDesC8,
										KNullDesC8);
	}


CMidiEventReceiver* CMidiEventReceiver::NewL(const TMMFMessage& aMessage)
	{
	return new(ELeave) CMidiEventReceiver(aMessage);
	}

CMidiEventReceiver::~CMidiEventReceiver()
	{
	if (!(iMessage.IsCompleted()))
		iMessage.Complete(KErrDied);
	delete iEventBuf;
	}

void CMidiEventReceiver::PrepareEventL(const CMMFMidiEvent& aEvent)
	{
	// eventbuf should be NULL.  delete it anyway though to prevent memory leaks in release mode
	ASSERT(!iEventBuf);
	
	delete iEventBuf;
	iEventBuf = NULL;

	iEventBuf = CBufFlat::NewL(32);
	RBufWriteStream s;
	s.Open(*iEventBuf);
	CleanupClosePushL(s);
	aEvent.ExternalizeL(s);
	CleanupStack::PopAndDestroy();//s
	// Write the size of the externalised data back to the client
	TPckgBuf<TInt> pckg;
	pckg() = iEventBuf->Ptr(0).Length();
	TInt error = iMessage.WriteDataToClient(pckg);
	iMessage.Complete(error);
	}

void CMidiEventReceiver::SendEventL(TMMFMessage& aMessage)
	{
	if (!iEventBuf)
		{
		User::Leave(KErrNotReady);
		}
	else
		{
		aMessage.WriteDataToClientL(iEventBuf->Ptr(0));
		}
	}

TBool CMidiEventReceiver::IsWaitingToSendEvent()
	{
	return (iEventBuf!=NULL);
	}

CMidiEventReceiver::CMidiEventReceiver(const TMMFMessage& aMessage) : iMessage(aMessage)
	{
	}


/**
Constructor.

@param aEventType	
       A UID to define the type of MIDI event.
@param aErrorCode	
       The error code associated with the MIDI event.
*/
EXPORT_C CMMFMidiEvent::CMMFMidiEvent(TUid aEventType, TInt aErrorCode)
	: iEventType(aEventType), iErrorCode(aErrorCode)
	{
	ZeroMembers();
	}

/**
Default constructor.
*/
EXPORT_C CMMFMidiEvent::CMMFMidiEvent()
	: iEventType(KNullUid), iErrorCode(KErrNone)
	{
	ZeroMembers();
	}

/**
Set to default values all the data members.
*/
void CMMFMidiEvent::ZeroMembers()
	{
	iOldState = EMidiStateClosedDisengaged;
	iNewState = EMidiStateClosedDisengaged;
	iMicroSeconds = 0;
	iMicroBeats = 0;
	iChannel = 0;
	iVolumeInDecibels = 0;
	iMute = EFalse;
	iMetaDataEntryId = 0;
	iPolyphony = 0;
	iBankId = 0;
	iInstrumentId = 0;
	iTempoMicroBeats = 0;
	}

/**
Destructor.
*/
EXPORT_C CMMFMidiEvent::~CMMFMidiEvent()
	{
	iMipMessage.Close();
	}
	
/**
Externalize the object to a stream. All the member variables will be written to the stream.

@param aStream	
       The write stream object.
*/
EXPORT_C void CMMFMidiEvent::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iEventType;
	aStream.WriteInt32L(iErrorCode);
	aStream.WriteInt32L(iOldState);
	aStream.WriteInt32L(iNewState);
	aStream << iMicroSeconds.Int64();
	aStream << iMicroBeats;
	aStream.WriteInt32L(iChannel);
	aStream << iVolumeInDecibels;
	aStream.WriteInt32L(iMute);
	aStream.WriteInt32L(iMetaDataEntryId);

	aStream.WriteInt32L(iMipMessage.Count());
	for (TInt i=0; i<iMipMessage.Count(); i++)
		{
		aStream.WriteInt32L(iMipMessage[i].iChannel);
		aStream.WriteInt32L(iMipMessage[i].iMIPValue);
		}

	aStream.WriteInt32L(iPolyphony);
	aStream.WriteInt32L(iBankId);
	aStream.WriteInt32L(iInstrumentId);
	aStream.WriteInt32L(iTempoMicroBeats);
	}

/**
Internalize the object from a stream. All the member variables will be read from the stream.

@param aStream	
       The read stream object.
*/
EXPORT_C void CMMFMidiEvent::InternalizeL(RReadStream& aStream)
	{
	aStream >> iEventType;
	iErrorCode = aStream.ReadInt32L();
	iOldState = STATIC_CAST(TMidiState, aStream.ReadInt32L());
	iNewState = STATIC_CAST(TMidiState, aStream.ReadInt32L());

	TInt64 microSeconds;
	aStream >> microSeconds;
	iMicroSeconds = microSeconds;
	
	aStream >> iMicroBeats;
	iChannel = aStream.ReadInt32L();
	aStream >> iVolumeInDecibels;
	iMute = aStream.ReadInt32L();
	iMetaDataEntryId = aStream.ReadInt32L();

	TInt count = aStream.ReadInt32L();
	for (TInt i=0; i<count; i++)
		{
		TMipMessageEntry entry;
		entry.iChannel = aStream.ReadInt32L();
		entry.iMIPValue = aStream.ReadInt32L();
		User::LeaveIfError(iMipMessage.Append(entry));
		}

	iPolyphony = aStream.ReadInt32L();
	iBankId = aStream.ReadInt32L();
	iInstrumentId = aStream.ReadInt32L();
	iTempoMicroBeats = aStream.ReadInt32L();
	}

/**
Copies a MIDI event into this CMMFMidiEvent.

@param aOther	
       The CMMFMidiEvent to copy from.
*/
EXPORT_C void CMMFMidiEvent::CopyL(const CMMFMidiEvent& aOther)
	{
	iEventType = aOther.iEventType;
	iErrorCode = aOther.iErrorCode;
	iOldState = aOther.iOldState;
	iNewState = aOther.iNewState;
	iMicroSeconds = aOther.iMicroSeconds;
	iMicroBeats = aOther.iMicroBeats;
	iChannel = aOther.iChannel;
	iVolumeInDecibels = aOther.iVolumeInDecibels;
	iMute = aOther.iMute;
	iMetaDataEntryId = aOther.iMetaDataEntryId;
	iPolyphony = aOther.iPolyphony;
	iBankId = aOther.iBankId;
	iInstrumentId = aOther.iInstrumentId;
	iTempoMicroBeats = aOther.iTempoMicroBeats;

	for (TInt i=0; i<aOther.iMipMessage.Count(); i++)
		{
		User::LeaveIfError(iMipMessage.Append(aOther.iMipMessage[i]));
		}
	}


