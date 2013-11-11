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

#ifndef __MIDICLIENTUTILITY_H__
#define __MIDICLIENTUTILITY_H__

#include <e32std.h>
#include <e32base.h>
#include <bamdesca.h> 
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include <mmf/common/mmfmididatacommon.h>


/**
General utility class to hold instrument information.

@publishedAll
@released
*/
class TInstrumentInfo
	{
public:
	/**
	 *Identifier of sound bank to reference.
	 */
	TInt iBankId;

	/**
	 * Specifies whether to reference a custom or standard sound bank.
	 */
	TBool iCustom;

	/**
	 * Identifier of specified instrument.
	 */
	TInt iInstrumentId;
	};

/**
Defines the state of the MIDI client utility with regard to the MIDI resource.

@publishedAll
@released
*/
enum TMidiState
	{
	/**
	 * No MIDI resource is currently open. 
	 * The MIDI engine is not processing midi events.
	 */
	EMidiStateClosedDisengaged,  


	/**
	 * A MIDI resource is open and primed but not playing.
	 * The MIDI engine is not processing midi events.
	 */
	EMidiStateOpenDisengaged,

	/**
	 * A MIDI resource is open and currently playing.
	 * The MIDI engine is currently processing midi events.
	 */
	EMidiStateOpenPlaying,  

	/**
	 * Not able to play full SP-MIDI or DLS content, but attempting best-effort playback.
	 */
	EMidiStatePlaybackIncompatible,

	/**
	 * Due to system resource constraints and SP-MIDI content constraints,
	 * there is no audible MIDI content during playback currently.
	 * The situation may change if resources are freed later.
	 */
	EMidiStatePlaybackSilent,
		
	/**
	 * No MIDI resource is currently open. 
	 * The MIDI engine is currently processing midi events. 
	 */
	EMidiStateClosedEngaged, 
	
	/**
	 * A MIDI resource is open and primed but not playing. 
	 * The MIDI engine is currently processing MIDI events. 
	 * The transition to this state is only possible from EMidiStateOpenPlaying state, 
	 * when the 'playback head' reaches the end of media data or the playback
	 * stops due to stop time set.
	 */
	EMidiStateOpenEngaged,
	};

/**
Active object utility class to allow MIDI events to be signalled asynchronously.
@see CMidiClientUtility::NewL().

@publishedAll
@released
*/
class MMidiClientUtilityObserver
	{
public:
    /**
    A state change has occurred in the MIDI client utility.
	
    @param aOldState 
	       Old state.
    @param aNewState 
	       New state.	 
    @param aTime 
	       The time in microseconds at which playback stopped relative to the start of the MIDI resource.	
    @param aError	
	       Error code.
    */
	virtual void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError) = 0;
    /**
    The MIDI resource has encountered a tempo change message in the MIDI data stream.
	
    @param aMicroBeatsPerMinute 
	       New tempo value in micro beats per minute (BPM*1000000).
    */
	virtual void MmcuoTempoChanged(TInt aMicroBeatsPerMinute) = 0;
    /**
    The MIDI resource has encountered a volume change message in the MIDI data stream.
	
    @param aChannel 
	       Logical channel to which the message applies. 0 <= aChannel < ChannelsSupportedL().
    @param aVolumeInDecibels 
	       New volume setting being used.
    */
	virtual void MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels) = 0;
    /**
    The MIDI resource has encountered a mute message in the MIDI data stream.
	
    @param aChannel
	       Logical channel to which the message applies. 0 <= aChannel < ChannelsSupportedL().
    @param aMuted 
	       New muting state of the channel - ETrue is muted, EFalse is unmuted.
    */
	virtual void MmcuoMuteChanged(TInt aChannel,TBool aMuted) = 0;
    /**
    Callback in response to a call to CMIDIClientUtility::SetSyncUpdateCallbackIntervalL().
	By default there are no callbacks. Otherwise the callback is called at an interval set by the client. 
	
    @param aMicroSeconds 
	       Position in microseconds relative to the start of the resource - only set if 
	       the sync update set function was called with the microseconds parameter set.
    @param aMicroBeats 
	       Position in microbeats relative to the start of the resource - only set if
	       the sync update set function was called with the microbeats parameter set and
	       the microseconds parameter set to 0.
    */
	virtual void MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats) = 0;
    /**
    A timed meta data entry has been found in the MIDI data stream, eg a timed song lyric in an SMF file.
	
    @param aMetaDataEntryId 
	       Index of the meta data entry - if this entry has not been encountere before,
	       it will be added to the list of meta data entries. Previously encountered timed 
		   entries still generate this callback. 
    @param aPosition
	       Position in microseconds relative to the start of the MIDI resource that the MIDI
	       meta data entry was found at.
    */
	virtual void MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition) = 0;
    /**
    A MIP message has been found in the MIDI data stream.
	
    @param aMessage 
	       Reference to an array of MIP message entries (logical {channel, MIP} value pairs) 
	       contained in the message.
    */
	virtual void MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aMessage) = 0;
    /**
    The polyphony of the MIDI engine has changed.
	
    @param aNewPolyphony 
	       The new polyphony value.
    */
	virtual void MmcuoPolyphonyChanged(TInt aNewPolyphony) = 0;
    /**
    An instrument has been changed on a channel.
	
    @param aChannel 
	       Channel where the change of instrument occurred. 0 <= aChannel < ChannelsSupportedL().
    @param aBankId 
	       Identifier of the bank that the instrument belongs to, occupying no more than 14 bits. 
	       The bank ID is a concatenation of MIDI bank MSB and LSB values.
    @param aInstrumentId 
	       Identifier of the instrument that has been changed. 0 <= aInstrumentId <= 127.
    */
	virtual void MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId) = 0;
	};


/**
Utility class to provide a standard client interface to a MIDI controller.

@publishedAll
@released
*/
class CMidiClientUtility : public CBase
	{
	class CBody;
public:
	IMPORT_C static CMidiClientUtility* NewL(MMidiClientUtilityObserver& aObserver,
											 TInt aPriority = EMdaPriorityNormal,
											 TInt aPref = EMdaPriorityPreferenceTimeAndQuality);
	
	// alternative NewL for selecting shared/separate controller heap(s)
	IMPORT_C static CMidiClientUtility* NewL(MMidiClientUtilityObserver& aObserver,
											 TInt aPriority,
											 TInt aPref,
											 TBool aUseSharedHeap);

	/**	Destructor */
	virtual ~CMidiClientUtility();
	IMPORT_C TMidiState State() const;
	IMPORT_C void OpenFile(const TDesC& aFileName);
	IMPORT_C void OpenFile(const RFile& aFile);

	IMPORT_C void OpenFile(const TMMSource& aSource);

	IMPORT_C void OpenDes(const TDesC8& aDescriptor);
	IMPORT_C void OpenUrl(const TDesC& aUrl,TInt aIapId = KUseDefaultIap,const TDesC8& aMimeType=KNullDesC8);
	IMPORT_C void Close();
	IMPORT_C void Play();
	IMPORT_C void Stop(const TTimeIntervalMicroSeconds& aFadeOutDuration);
	IMPORT_C void PlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	IMPORT_C void PlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aStartTime,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	IMPORT_C void StopNotes(TInt aChannel);
	IMPORT_C void NoteOnL(TInt aChannel,TInt aNote,TInt aVelocity);
	IMPORT_C void NoteOffL(TInt aChannel,TInt aNote,TInt aVelocity);
	IMPORT_C TInt PlaybackRateL() const;
	IMPORT_C void SetPlaybackRateL(TInt aRate);
	IMPORT_C TInt MaxPlaybackRateL() const;
	IMPORT_C TInt MinPlaybackRateL() const;
	IMPORT_C TInt TempoMicroBeatsPerMinuteL() const;
	IMPORT_C void SetTempoL(TInt aMicroBeatsPerMinute);
	IMPORT_C TInt PitchTranspositionCentsL() const;
	IMPORT_C TInt SetPitchTranspositionL(TInt aCents);
	IMPORT_C TTimeIntervalMicroSeconds DurationMicroSecondsL() const;
	IMPORT_C TInt64 DurationMicroBeatsL() const;
	IMPORT_C TInt NumTracksL() const;
	IMPORT_C void SetTrackMuteL(TInt aTrack,TBool aMuted) const;
	IMPORT_C const TDesC8& MimeTypeL();
	IMPORT_C TTimeIntervalMicroSeconds PositionMicroSecondsL() const;
	IMPORT_C void SetPositionMicroSecondsL(const TTimeIntervalMicroSeconds& aPosition);
	IMPORT_C TInt64 PositionMicroBeatsL() const;
	IMPORT_C void SetPositionMicroBeatsL(TInt64 aMicroBeats);
	IMPORT_C void SetSyncUpdateCallbackIntervalL(const TTimeIntervalMicroSeconds& aMicroSeconds, TInt64 aMicroBeats=0);		
	IMPORT_C TInt SendMessageL(const TDesC8& aMidiMessage);
	IMPORT_C TInt SendMessageL(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& aTime);
	IMPORT_C void SendMipMessageL(const RArray<TMipMessageEntry>& aEntry);
	IMPORT_C TInt NumberOfBanksL(TBool aCustom) const;
	IMPORT_C TInt GetBankIdL(TBool aCustom,TInt aBankIndex) const;
	IMPORT_C void LoadCustomBankL(const TDesC& aFileName,TInt& aBankCollectionIndex);
	IMPORT_C void LoadCustomBankDataL(const TDesC8& aBankData,TInt& aBankCollectionIndex);
	IMPORT_C void UnloadCustomBankL(TInt aBankCollectionIndex);
	IMPORT_C TBool CustomBankLoadedL(TInt aBankCollectionIndex) const;
	IMPORT_C void UnloadAllCustomBanksL();
	IMPORT_C TInt NumberOfInstrumentsL(TInt aBankId,TBool aCustom) const;
	IMPORT_C TInt GetInstrumentIdL(TInt aBankId,TBool aCustom,TInt aInstrumentIndex) const;
	IMPORT_C HBufC* InstrumentNameL(TInt aBankId, TBool aCustom, TInt aInstrumentId) const;
	IMPORT_C void GetInstrumentL(TInt aChannel, TInt& aInstrumentId, TInt& aBankId);
	IMPORT_C void SetInstrumentL(TInt aChannel,TInt aBankId,TInt aInstrumentId);
	IMPORT_C void LoadCustomInstrumentL(const TDesC& aFileName,TInt aFileBankId,TInt aFileInstrumentId,TInt aMemoryBankId,TInt aMemoryInstrumentId);
	IMPORT_C void LoadCustomInstrumentDataL(const TDesC8& aInstrumentData, TInt aBankDataId, TInt aInstrumentDataId, TInt aMemoryBankId, TInt aMemoryInstrumentId);
	IMPORT_C void UnloadCustomInstrumentL(TInt aCustomBankId,TInt aInstrumentId);
	IMPORT_C HBufC* PercussionKeyNameL(TInt aNote, TInt aBankId, TBool aCustom, TInt aInstrumentId) const;
	IMPORT_C void StopTimeL(TTimeIntervalMicroSeconds& aStopTime) const;
	IMPORT_C void SetStopTimeL(const TTimeIntervalMicroSeconds& aStopTime);
	IMPORT_C void SetRepeatsL(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	IMPORT_C TInt GetRepeats() const;
	IMPORT_C TInt PolyphonyL() const;
	IMPORT_C void SetMaxPolyphonyL(TInt aMaxNotes);
	IMPORT_C TInt ChannelsSupportedL() const;
	IMPORT_C TReal32 ChannelVolumeL(TInt aChannel) const;
	IMPORT_C TReal32 MaxChannelVolumeL() const;
	IMPORT_C void SetChannelVolumeL(TInt aChannel,TReal32 aVolumeInDecibels);
	IMPORT_C void SetChannelMuteL(TInt aChannel,TBool aMuted);
	IMPORT_C TInt VolumeL() const;
	IMPORT_C TInt MaxVolumeL() const;
	IMPORT_C void SetVolumeL(TInt aVolume);
	IMPORT_C void SetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration);
	IMPORT_C TInt GetBalanceL() const;
	IMPORT_C void SetBalanceL(TInt aBalance = KMMFBalanceCenter);
	IMPORT_C void SetPriorityL(TInt aPriority, TInt aPref);
	IMPORT_C TInt NumberOfMetaDataEntriesL() const;
	IMPORT_C CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex) const;
/**
 * @deprecated SetBankL is deprecated due to Mobile XMF specification requirement that user instruments (custom instruments) override standard instruments.
 */
	IMPORT_C void SetBankL(TBool aCustom);
	IMPORT_C TBool IsTrackMuteL(TInt aTrack) const;
	IMPORT_C TBool IsChannelMuteL(TInt aChannel) const;
	IMPORT_C void CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	IMPORT_C void CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);
	IMPORT_C TInt MaxPolyphonyL() const;

	IMPORT_C MMMFDRMCustomCommand* GetDRMCustomCommand();
	
private:
	/**
	 * a pointer to the implementation.
	 */
	CBody* iBody;	
	friend class CBody;
	}; 

#endif
