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
// Mmf\Common\MmfMidiconfig.h
// 
//

#ifndef __MMF_COMMON_MMFMIDI_CONFIG_H__
#define __MMF_COMMON_MMFMIDI_CONFIG_H__

/**
@internalComponent
*/
class TMMFMidiConfig1
	{
public:
	inline TMMFMidiConfig1();
public:
	/** 
	 * Number of tracks present in the currently open MIDI resource.
	 */
	TInt iNumTracks;

	/** 
	 * Current state of the MIDI client utility object 
	 * (EMidiStateClosedDisengaged,EMidiStateOpenDisengaged,EMidiStateOpenPlaying,EMidiStatePlaybackIncompatible,
	 *  EMidiStatePlaybackSilent, EMidiStateClosedEngaged, EMidiStateOpenEngaged).
	 */
	 
	TInt iState;

	/** 
	 * Tempo in microbeats per minute (BPM*1000000).
	 */
	TInt iTempo;

	/** 
	 * Pitch shift in cents, i.e. semitones * 100. One octave equals 1200 cents.
	 */
	TInt iPitch;

	/** 
	 * Array of logical {channel, MIP} value pairs to send SP-MIDI MIP message to the MIDI engine.
	 */
	const TArray<TMipMessageEntry>* iMipMessage; 

	/** 
	 * The maximum number of notes that the MIDI engine supports simultaneously (polyphony).
	 */
	TInt iNumNotes; 

	/** 
	 * Max polyphony level: 0 <= PolyphonyL() <= iMaxNotes.
	 */
	TInt iMaxNotes;

	/** 
	 * Overall volume of the MIDI client.
	 */
	TInt iVolume;

	/** 
	 * Maximum volume that may be applied overall to the MIDI client.
	 */
	TInt iMaxVolume;

	/** 
	 * Length of time over which the volume is faded up from zero to the current settings when playback is started.
	 */
	TTimeIntervalMicroSeconds iRampDuration;

	/** 
	 * Stereo balance value: KMMFBalanceMaxLeft <= iBalance <= KMMFBalanceMaxRight.
	 */
	TInt iBalance;

	/** 
	 * The number of times the current opened resources has to be repeated.
	 * This includes the first instance of playing.
	 */
	TInt iNumRepeats;

	/** 
	 * Length of time over which the volume is faded out from the current settings to zero.
	 */
	TTimeIntervalMicroSeconds iFadeOutDuration;
	};

/**
@internalComponent
*/
class TMMFMidiConfig2
	{
public:
	inline TMMFMidiConfig2();
public:
	/** 
	 * Index of the track to mute: 0 <= iTrack < NumTracksL().
	 */
	TInt iTrack;

	/** 
	 * ETrue to mute the track/channel, EFalse to unmute it.
	 */
	TBool iMuted;

	/** 
	 * Note to play ot to terminate: 0 <= iNote <= 127.
	 */
	TInt iNote;

	/** 
	 * Length of time to play the note for.
	 */
	TTimeIntervalMicroSeconds iDurationMicroSeconds;

	/** 
	 * Specifying the time at which to start playing the note.
	 * It's relative to the MIDI resource playing time or the time elapsed
	 * since Play() was called if no resource is present.
	 */
	TTimeIntervalMicroSeconds iStartTime;

	/** 
	 * Velocity with which to start the note: 0 <= iNoteOnVelocity <= 127.
	 */
	TInt iNoteOnVelocity;

	/** 
	 * Velocity with which to stop the note: 0 <= iNoteOffVelocity <= 127.
	 */
	TInt iNoteOffVelocity;

	/** 
	 * Number of standard or custom sound banks currently available.
	 */
	TInt iNumBanks;

	/** 
	 * Specifies whether to reference a custom or standard sound bank.
	 */
	TBool iCustom;

	/** 
	 * Index of custom/deault bank: 0 <= iBankIndex < NumberOfBanksL()
	 */
	TInt iBankIndex;

	/** 
	 * Custom/deault Bank's Identifier.
	 */
	TInt iBankId;

	/** 
	 * Identifier of the custom bank in memory.
	 */
	TInt iMemoryBankId;

	/** 
	 * Name of the file containing the custom sound bank or the name 
	 * of the file containing the instrument. Includes the directory path. 
	 */
	const TDesC* iFileName;

	/** 
	 * Descriptor containing the custom sound bank.
	 */
	const TDesC8* iBankData;

	/** 
	 * Descriptor containing the instrument.
	 */
	const TDesC8* iInstrumentData;

	/** 
	 * ETrue if the specified bank is in memory, EFalse otherwise.
	 */
	TBool iBankLoaded;

	/** 
	 * Number of instruments available for the specified sound bank.
	 */
	TInt iNumInstruments;

	/** 
	 * Index of the instrument to reference: 0 <= aInstrumentIndex < NumberOfInstrumentsL().
	 */
	TInt iInstrumentIndex;

	/** 
	 * Identifier of specified instrument. This may differ from the index 
	 * since the index simply enumerates the instruments, whereas identifiers 
	 * may not be contiguous, especially where certain instruments correspond 
	 * to General MIDI-defined instruments but not all instruments are present.
	 * 0 <= iInstrumentId <= 127.
	 */
	TInt iInstrumentId;

	/** 
	 * Identifier of the instrument in memory.
	 */
	TInt iMemoryInstrumentId;

	/** 
	 * Current metrical position of the MIDI resource being played.
	 * Microbeats (BPM*1000000) relative to the start of the resource.
	 */
	TInt64 iPositionMicroBeats;

	/** 
	 * Length of the currently open MIDI resource.
	 * Duration in microbeats (beats * 1000000).
	 */
	TInt64 iDurationMicroBeats;

	/** 
	 * Time at which playback will stop, relative to the start of the resource.
	 * Clamped to 0 and the duration of the resource.
	 */
	TTimeIntervalMicroSeconds iStopTime;

	/** 
	 * Logical channel: 0 <= iChannel < ChannelsSupportedL().
	 */
	TInt iChannel;

	/** 
	 * Volume level in dB corresponding to the MIDI Channel:
	 * -infinity <= iChannelVol <= MaxVolumeL().
	 */
	TReal32 iChannelVol;

	/** 
	 * Maximum volume setting in dB, should always be 0 dB and 
	 * correspond to the MIDI Channel Volume controller maximum value.
	 */
	TReal32 iMaxChannelVol;
	};

/**
@internalComponent
*/
class TMMFMidiConfig3
	{
public:
	inline TMMFMidiConfig3();
public:
	/** 
	 * Temporal interval between MmcuoSyncUpdate callback.
	 */
	TTimeIntervalMicroSeconds iCallbackIntervalMicroSeconds;

	/** 
	 * Metrical interval between MmcuoSyncUpdate callback.
	 */
	TInt64 iCallbackIntervalMicroBeats;

	/** 
	 * Number of bytes of the MIDI message buffer actually processed when client
	 * calls SendMessageL.
	 */
	TInt iBytesProcessed;

	/** 
	 * Time at which to execute the MIDI message sent by SendMessageL.
	 * It's relative to the MIDI resource playing time or the time elapsed 
	 * since Play() was called if no resource is present.
	 */
	TTimeIntervalMicroSeconds iTimeStamp;

	/** 
	 * Number of time to repeat the resource during playback. This includes the first playing.
	 */
	TInt iRepeatNumberOfTimes;

	/** 
	 * Time in microseconds to pause between repeats.
	 */
	TTimeIntervalMicroSeconds iTrailingSilence;

	/** 
	 * Playback rate for the playback of the current MIDI resource
	 * in percent times 1000, i.e., 100000 means original playback speed,
	 * 200000 means double speed, and 50000 means half speed playback.
	 */
	TInt iPlayBackRate;

	/** 
	 * Maximum playback rate supported by MIDI player.
	 */
	TInt iPlayBackMaxRate;

	/** 
	 * Minimum playback rate supported by MIDI player.
	 */
	TInt iPlayBackMinRate;

	/** 
	 * Descriptor containing a MIDI message data. If there
	 * are several MIDI messages in the buffer, only the first
	 * one is processed.
	 */
	const TDesC8* iMidiMessage;
	};

inline TMMFMidiConfig1::TMMFMidiConfig1()
{};

inline TMMFMidiConfig2::TMMFMidiConfig2()
{};

inline TMMFMidiConfig3::TMMFMidiConfig3()
{};

#endif //__MMF_COMMON_MMFMIDI_CONFIG_H__
