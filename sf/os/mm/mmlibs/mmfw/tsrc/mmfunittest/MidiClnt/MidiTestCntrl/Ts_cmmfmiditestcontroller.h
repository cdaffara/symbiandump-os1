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


#ifndef TS_CMMFMIDITESTCONTROLLER_H
#define TS_CMMFMIDITESTCONTROLLER_H

#include <e32std.h>
#include <e32base.h>



#include <ecom/implementationproxy.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>

#include <testframework.h>
#include <mmf/common/midistandardcustomcommands.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include "CmmfMidiTestControllerUIDs.hrh"


class CMMFMidiTestController : public CMMFController,
								public MMidiCustomCommandImplementor,
								public MMMFDRMCustomCommandImplementor
{
public:
	static CMMFMidiTestController* NewL(void);
	static CMMFMidiTestController* NewLC(void);

	//implement CMMFMidiTestController ecom plugin interface
	virtual void AddDataSourceL(MDataSource& aDataSource);
	virtual void AddDataSinkL(MDataSink& aDataSink);
	virtual void RemoveDataSourceL(MDataSource& aDataSource);
	virtual void RemoveDataSinkL(MDataSink& aDataSink);
	virtual void RemoveSourceSinkL();
	virtual void ResetL();//XXX - replacement for RemoveSourceSink()
	virtual void PrimeL();
	virtual void PlayL();
	virtual void PauseL();
	virtual void StopL();
	virtual TTimeIntervalMicroSeconds PositionL() const;
	virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	virtual TTimeIntervalMicroSeconds DurationL() const;
	virtual void CustomCommand(TMMFMessage& aMessage);
	virtual void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	virtual void GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries);
	virtual CMMFMetaDataEntry* GetMetaDataEntryL(TInt aIndex);

	TInt SendEventToClient(const TMMFEvent& aEvent);
	TInt OpenComplete();
	TInt PlayStarted();
	TInt SyncUpdate();
	TInt PlayFinished();
	TInt Stop();
	TInt Close();
	TInt TempoChanged();
	TInt VolumeChanged();
	TInt MuteChanged();
	TInt PolyphonyChanged();
	TInt InstrumentChanged();
	TInt SmfMetaDataEntryFound();
	TInt MipMessageReceived();

	//-------------------------------//
	// MMidiCustomCommandImplementor //
	//-------------------------------//
	virtual void  MmcSetPositionMicroBeatsL(TInt64 aMicroBeats);
	virtual void  MmcPositionMicroBeatsL(TInt64& aMicroBeats);
	virtual void  MmcPlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	virtual void  MmcPlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aStartTime, const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	virtual void  MmcStopNotesL(TInt aChannel);
	virtual void  MmcNoteOnL(TInt aChannel,TInt aNote,TInt aVelocity);
	virtual void  MmcNoteOffL(TInt aChannel,TInt aNote,TInt aVelocity);
	virtual void  MmcPlaybackRateL(TInt& aPlayBackRate);
	virtual void  MmcSetPlaybackRateL(TInt aPlayBackRate);
	virtual void  MmcMaxPlaybackRateL(TInt& aMaxRate);
	virtual void  MmcMinPlaybackRateL(TInt& aMinRate);
	virtual void  MmcTempoMicroBeatsPerMinuteL(TInt& aMicroBeatsPerMinute);
	virtual void  MmcSetTempoL(TInt aMicroBeatsPerMinute);
	virtual void  MmcPitchTranspositionCentsL(TInt& aPitch);
	virtual void  MmcSetPitchTranspositionL(TInt aCents, TInt& aCentsApplied);
	virtual void  MmcDurationMicroBeatsL(TInt64& aDuration);
	virtual void  MmcNumTracksL(TInt& aTracks);
	virtual void  MmcSetTrackMuteL(TInt aTrack, TBool aMuted);
	virtual void  MmcMimeTypeL(TDes8& aMimeType);
	virtual void  MmcSetSyncUpdateCallbackIntervalL(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats);
	virtual void  MmcSendMessageL(const TDesC8& aMidiMessage, TInt& aBytes);
	virtual void  MmcSendMessageL(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& aTime, TInt& aBytes);
	virtual void  MmcSendMipMessageL(const TArray<TMipMessageEntry>& aEntry);
	virtual void  MmcNumberOfBanksL(TBool aCustom, TInt& aNumBanks);
	virtual void  MmcGetBankIdL(TBool aCustom, TInt aBankIndex, TInt& aBankId);
	virtual void  MmcLoadCustomBankL(const TDesC& aFileName,TInt& aBankId);
	virtual void  MmcUnloadCustomBankL(TInt aBankId);
	virtual void  MmcCustomBankLoadedL(TInt aBankId, TBool& aBankLoaded);
	virtual void  MmcUnloadAllCustomBanksL();
	virtual void  MmcNumberOfInstrumentsL(TInt aBankId, TBool aCustom, TInt& aNumInstruments);
	virtual void  MmcGetInstrumentIdL(TInt aBankId,TBool aCustom,TInt aInstrumentIndex, TInt& aInstrumentId);
	virtual const TDesC& MmcInstrumentNameL(TInt aBankId, TBool aCustom, TInt aInstrumentId);
	virtual void  MmcSetInstrumentL(TInt aChannel,TInt aBankId,TInt aInstrumentId);
	virtual void  MmcLoadCustomInstrumentL(const TDesC& aFileName, TInt aFileBankId, TInt aFileInstrumentId, TInt aMemoryBankId, TInt aMemoryInstrumentId);
	virtual void  MmcUnloadCustomInstrumentL(TInt aCustomBankId,TInt aInstrumentId);
	virtual const TDesC& MmcPercussionKeyNameL(TInt aNote, TInt aBankId, TBool aCustom, TInt aInstrumentId);
	virtual void  MmcStopTimeL(TTimeIntervalMicroSeconds& aStopTime);
	virtual void  MmcSetStopTimeL(const TTimeIntervalMicroSeconds& aStopTime);
	virtual void  MmcPolyphonyL(TInt& aNumNotes);
	virtual void  MmcChannelsSupportedL(TInt& aChannels);
	virtual void  MmcChannelVolumeL(TInt aChannel, TReal32& aChannelVol);
	virtual void  MmcMaxChannelVolumeL(TReal32& aMaxVol);
	virtual void  MmcSetChannelVolumeL(TInt aChannel,TReal32 aVolume);
	virtual void  MmcSetChannelMuteL(TInt aChannel,TBool aMuted);
	virtual void  MmcVolumeL(TInt& aVolume);
	virtual void  MmcMaxVolumeL(TInt& aMaxVolume);
	virtual void  MmcSetVolumeL(TInt aVolume);
	virtual void  MmcSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration);
	virtual void  MmcGetBalanceL(TInt& aBalance);
	virtual void  MmcSetBalanceL(TInt aBalance);
	virtual void  MmcLoadCustomBankDataL(const TDesC8& aBankData,TInt& aBankId);
	virtual void  MmcLoadCustomInstrumentDataL(const TDesC8& aInstrumentData, TInt aBankDataId, TInt aInstrumentDataId, TInt aMemoryBankId, TInt aMemoryInstrumentId);
	virtual void  MmcSetMaxPolyphonyL(TInt aMaxNotes);
	virtual void  MmcGetRepeatsL(TInt& aNumRepeats);
	virtual void  MmcSetRepeatsL(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	virtual void  MmcSetBankL(TBool aCustom);
	virtual void  MmcIsTrackMuteL(TInt aTrack, TBool& aTrackMute);
	virtual void  MmcIsChannelMuteL(TInt aChannel, TBool& aChannelMute);
	virtual void  MmcGetInstrumentL(TInt aChannel, TInt& aInstrumentId, TInt& aBankId);
	virtual void  MmcCloseL();
	virtual void  MmcStopL(const TTimeIntervalMicroSeconds& aFadeOutDuration);
	virtual void  MmcMaxPolyphonyL(TInt& aMaxNotes);
	
	// MMMFDRMCustomCommandImplementor
	virtual TInt MdcDisableAutomaticIntent(TBool aDisableAutoIntent);
	virtual TInt MdcExecuteIntent(ContentAccess::TIntent aIntent);
	virtual TInt MdcEvaluateIntent(ContentAccess::TIntent aIntent);
	virtual TInt MdcSetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue);
private:

	class CControllerTimer : public CTimer
		{
	public:
		enum TimerType
			{
			ETmrOpenComplete,
			ETmrSyncUpdate,
			ETmrDelayPlayStarted,
			ETmrPlayFinished,
			ETmrDelayStop,
			ETmrDelayClose
			};
	public:
		static CControllerTimer* NewL(CMMFMidiTestController* aController);
		void DelayOpen(TTimeIntervalMicroSeconds aDelay);
		void DelayPlayStarted(TTimeIntervalMicroSeconds aDelay);
		void PlayFinished(TTimeIntervalMicroSeconds aDelay);
		void DelaySyncUpdate(TTimeIntervalMicroSeconds aDelay);
		void DelayStop(TTimeIntervalMicroSeconds aDelay);
		void DelayClose(TTimeIntervalMicroSeconds aDelay);

		void RunL();
	private:
		CControllerTimer(CMMFMidiTestController* aController);
		CMMFMidiTestController* iController;
		TimerType iType;
		};

	class CMidiEventGenerator : public CTimer
		{
	public:
		enum TEventType
			{
			EEventTempoChanged,
			EEventVolumeChanged,
			EEventMuteChanged,
			EEventPolyphonyChanged,
			EEventInstrumentChanged,
			EEventSmfMetaDataEntryFound,
			EEventMipMessageReceived
			};
	public:
		static CMidiEventGenerator* NewL(CMMFMidiTestController* aController);
		void MidiEventTrigger();
		void RunL();
	private:
		CMidiEventGenerator(CMMFMidiTestController* aController);
		CMMFMidiTestController* iController;
		TEventType iType;
		TTimeIntervalMicroSeconds iDelay;
		TInt iTriggerCount;
		};

private:
	CMMFMidiTestController(void);
	~CMMFMidiTestController(void);
	void ConstructL(void);
	void InsertInterfaceNameInChunk(const TDesC8& aInterfaceName);

private:
//	CMidiCustomCommandParser* iMidiParser;
	MMidiEventHandler* iMidiEventHandler;
	CControllerTimer* iDelayEvent;
	CMidiEventGenerator* iMidiEventGenerator;

	TInt iPlayBackRate;
	TInt iMaxRate;
	TInt iMinRate;
	TInt iTempoMicroBeatsPerMinute;
	TInt iPitchTranspositionCents;
	TInt iCentsApplied;
	TTimeIntervalMicroSeconds iDurationMicroSeconds;
	TInt64 iDurationMicroBeats;
	TInt iNumTracks;
	TTimeIntervalMicroSeconds iPositionMicroSeconds;
	TInt64 iPositionMicroBeats;
	TInt iNumBanks;
	TInt iNumInstruments;
	TTimeIntervalMicroSeconds iStopTime;
	TInt iNumNotes;
	TInt iChannels;
	TReal32 iMaxVol;
	TInt iMaxVolume;
	TInt iBalance;
	TInt iRepeatNumberOfTimes;
	TTimeIntervalMicroSeconds iTrailingSilence;
	TInt iMaxNotes;
	
	CArrayFixFlat<TReal32>* iChanVolumeValues;
	TMidiState iCurrentMidiTestControllerState;
	TBool iDisableAutoIntent;
	MDataSource*  iDataSource;
};

#endif
