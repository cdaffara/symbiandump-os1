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

#ifndef __MIDICLIENTUTILITYBODY_H__
#define __MIDICLIENTUTILITYBODY_H__

#include <midiclientutility.h>
#include <mmf/common/midistandardcustomcommands.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include "mmfclientutility.h"


/**
Mixin class to allow notification that the timed silence has finished.
*/
class MRepeatTrailingSilenceTimerObs
	{
public:
	virtual void RepeatTrailingSilenceTimerComplete() = 0;
	};

/**
CTimer-based active object that waits the requested time before notifying its observer.
*/
class CRepeatTrailingSilenceTimer : public CTimer
	{
public:
	static CRepeatTrailingSilenceTimer* NewL(MRepeatTrailingSilenceTimerObs& aObs);
	void RunL();
private:
	CRepeatTrailingSilenceTimer(MRepeatTrailingSilenceTimerObs& aObs);
private:
	MRepeatTrailingSilenceTimerObs& iObs;
	};


class MMidiControllerEventMonitorObserver
	{
public: 
	virtual void HandleMidiEvent(const CMMFMidiEvent& aEvent) = 0;
	};


class CMidiControllerEventMonitor : public CActive
	{
public:
	static CMidiControllerEventMonitor* NewL(MMidiControllerEventMonitorObserver& aMidiObserver, 
		RMidiControllerCustomCommands& aMidiControllerCustomCommands,
		const CMidiClientUtility& aParent);
	~CMidiControllerEventMonitor();
	void Start();
	void SelfComplete(TInt aError);
private:
	void ConstructL();
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	CMidiControllerEventMonitor(MMidiControllerEventMonitorObserver& aObserver, 
	RMidiControllerCustomCommands& aMidiControllerCustomCommands, const CMidiClientUtility& aParent);
private:
	MMidiControllerEventMonitorObserver& iMidiObserver;
	RMidiControllerCustomCommands& iMidiControllerCustomCommands;
	TPckgBuf<TInt> iSizeOfEvent;
	const CMidiClientUtility& iParent;
	CMMFMidiEvent* iMidiEvent;
	};


class CMidiEventMonitor : public CActive
	{
public:
	static CMidiEventMonitor* NewL(MMidiControllerEventMonitorObserver& aMidiObserver,
								   const CMidiClientUtility& aParent);
	~CMidiEventMonitor();
	void SelfComplete(TInt aError);
private:
	void ConstructL();
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	CMidiEventMonitor(MMidiControllerEventMonitorObserver& aObserver,
					  const CMidiClientUtility& aParent);
private:
	MMidiControllerEventMonitorObserver& iMidiObserver;
	const CMidiClientUtility& iParent;
	CMMFMidiEvent* iMidiEvent;
	};


class CContent;
class CData;
/**
*/
class CMidiClientUtility::CBody : public CBase, 
								  public MMidiControllerEventMonitorObserver,
								  public MRepeatTrailingSilenceTimerObs,
								  public MMMFAddDataSourceSinkAsyncObserver
	{
public:
	static CMidiClientUtility::CBody* NewL(CMidiClientUtility* aParent,
										   MMidiClientUtilityObserver& aObserver,
										   TInt aPriority,
										   TInt aPref,
										   TBool aUseSharedHeap);
	

/**
	Destructor
*/
	~CBody();

	void OpenFile(const TDesC& aFileSource);
	void OpenFile(RFile& aFile);
	void OpenFile(const TMMSource& aFileSource);
	void OpenDes(const TDesC8& aDescriptor);
	void OpenUrl(const TDesC& aUrl,TInt aIapId = KUseDefaultIap,const TDesC8& aMimeType=KNullDesC8);
	void Close();
	void Play();
	void Stop(const TTimeIntervalMicroSeconds& aFadeOutDuration);
	TMidiState State() const;
	void PlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	void PlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aStartTime,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	void StopNotes(TInt aChannel);
	void NoteOnL(TInt aChannel,TInt aNote,TInt aVelocity);
	void NoteOffL(TInt aChannel,TInt aNote,TInt aVelocity);
	TInt PlaybackRateL() const;
	void SetPlaybackRateL(TInt aRate);
	TInt MaxPlaybackRateL() const;
	TInt MinPlaybackRateL() const;
	TInt TempoMicroBeatsPerMinuteL() const;
	void SetTempoL(TInt aMicroBeatsPerMinute);
	TInt PitchTranspositionCentsL() const;
	TInt SetPitchTranspositionL(TInt aCents);
	TTimeIntervalMicroSeconds DurationMicroSecondsL() const;
	TInt64 DurationMicroBeatsL() const;
	TInt NumTracksL() const;
	void SetTrackMuteL(TInt aTrack,TBool aMuted) const;
	const TDesC8& MimeTypeL();
	TTimeIntervalMicroSeconds PositionMicroSecondsL() const;
	void SetPositionMicroSecondsL(const TTimeIntervalMicroSeconds& aPosition);
	TInt64 PositionMicroBeatsL() const;
	void SetPositionMicroBeatsL(TInt64 aMicroBeats);
	void SetSyncUpdateCallbackIntervalL(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats=0);
	TInt SendMessageL(const TDesC8& aMidiMessage);
	TInt SendMessageL(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& aTime);
	void SendMipMessageL(const RArray<TMipMessageEntry>& aEntry);
	TInt NumberOfBanksL(TBool aCustom) const;
	TInt GetBankIdL(TBool aCustom,TInt aBankIndex) const;
	void LoadCustomBankL(const TDesC& aFileName,TInt& aBankCollectionIndex);
	void UnloadCustomBankL(TInt aBankCollectionIndex);
	TBool CustomBankLoadedL(TInt aBankCollectionIndex) const;
	void UnloadAllCustomBanksL();
	TInt NumberOfInstrumentsL(TInt aBankId,TBool aCustom) const;
	TInt GetInstrumentIdL(TInt aBankId,TBool aCustom,TInt aInstrumentIndex) const;
	HBufC* InstrumentNameL(TInt aBankId, TBool aCustom, TInt aInstrumentId) const;
	void SetInstrumentL(TInt aChannel,TInt aBankId,TInt aInstrumentId);
	void LoadCustomInstrumentL(const TDesC& aFileName,TInt aFileBankId,TInt aFileInstrumentId,TInt aMemoryBankId,TInt aMemoryInstrumentId);
	void UnloadCustomInstrumentL(TInt aCustomBankId,TInt aInstrumentId);
	HBufC* PercussionKeyNameL(TInt aNote, TInt aBankId, TBool aCustom, TInt aInstrumentId) const;
	void StopTimeL(TTimeIntervalMicroSeconds& aStopTime) const;
	void SetStopTimeL(const TTimeIntervalMicroSeconds& aStopTime);
	void SetRepeatsL(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	TInt PolyphonyL() const;
	TInt ChannelsSupportedL() const;
	TReal32 ChannelVolumeL(TInt aChannel) const;
	TReal32 MaxChannelVolumeL() const;
	void SetChannelVolumeL(TInt aChannel,TReal32 aVolume);
	void SetChannelMuteL(TInt aChannel,TBool aMuted);
	TInt VolumeL() const;
	TInt MaxVolumeL() const;
	void SetVolumeL(TInt aVolume);
	void SetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration);
	TInt GetBalanceL() const;
	void SetBalanceL(TInt aBalance = KMMFBalanceCenter);
	void SetPriorityL(TInt aPriority, TInt aPref);
	TInt NumberOfMetaDataEntriesL() const;
	CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex) const;
	void SetMaxPolyphonyL(TInt aMaxVoices);
	TInt GetRepeats();
	void LoadCustomBankDataL(const TDesC8& aBankData,TInt& aBankCollectionIndex);
	void LoadCustomInstrumentDataL(const TDesC8& aInstrumentData, TInt aBankDataId, TInt aInstrumentDataId, TInt aMemoryBankId, TInt aMemoryInstrumentId);
	void SetBankL(TBool aCustom);
	TBool IsTrackMuteL(TInt aTrack) const;
	TBool IsChannelMuteL(TInt aChannel) const;
	void GetInstrumentL(TInt aChannel, TInt& aInstrumentId, TInt& aBankId);
	TInt MaxPolyphonyL() const;
	void CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	void CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);

	MMMFDRMCustomCommand* GetDRMCustomCommand();

	virtual void RepeatTrailingSilenceTimerComplete();
	//From MMMFControllerEventMonitorObserver
	virtual void HandleMidiEvent(const CMMFMidiEvent& aEvent);

	// from MMMFAddDataSourceSinkAsyncObserver
	virtual void MadssaoAddDataSourceSinkAsyncComplete(TInt aError, const TMMFMessageDestination& aHandle);
	
private:
	CBody(CMidiClientUtility* aParent, MMidiClientUtilityObserver& aObserver, TInt aPriority, TInt aPref);
	void ConstructL(TBool aUseSharedHeap);
	TInt DoOpen(const RMMFControllerImplInfoArray& aControllers, TUid aSinkUid, const TDesC8& aSinkData, TBool aUseSharedHeap);
   	void ExternalizeToCBufFlatL(CBufFlat*& aCfgBuffer, const CMMFUrlParams* aUrlParams) const;
	void DoOpenFileL(const TMMSource& aSource);
    void DoOpenUrlL(const TDesC& aUrl, TInt aIapId);
private:
	CMidiClientUtility* iParent;
	MMidiClientUtilityObserver& iObserver;
	RMMFController iController;
	CMidiControllerEventMonitor* iMidiControllerEventMonitor;
	TMidiState iState;
	TMMFPrioritySettings iPrioritySettings;
	CRepeatTrailingSilenceTimer* iRepeatTrailingSilenceTimer;

	// Source and sink handle info
	TMMFMessageDestination iSourceHandle;
	TMMFMessageDestination iSinkHandle;

	// Custom command handlers
	RMidiControllerCustomCommands iMidiControllerCommands;

	RMMFDRMCustomCommands iDRMCustomCommands;

	//Stop position
	TTimeIntervalMicroSeconds iStopPosition;

	HBufC8* iMimeType;
	TBool iIntervalSec;
	CMMFAddDataSourceSinkAsync* iAddDataSourceSinkAsync;
	CMMSourceSink* iSource;

	ContentAccess::CContent* iContent;
	ContentAccess::CData* iData;
	}; 

#endif
