// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32svr.h>

//const TInt KOpenDelay =500000;;		// EABI warning removal
//const TInt KPrepareDelay = 500000;;	// EABI warning removal
//const TInt KFrameReadyDelay = 500000;;	// EABI warning removal
//const TInt KDuration = 1000000;		// EABI warning removal
//const TInt KFrameLength = 100000;		// EABI warning removal

//const TInt KMaxVolume = 100;		// EABI warning removal
//const TInt KFrameWidth = 100;		// EABI warning removal
//const TInt KFrameHeight = 50;		// EABI warning removal

//const TInt KBufMimeTypeGranularity = 4;		// EABI warning removal

const TUint8 KMaxChannelNumber = 15;

#include <mmf/server/mmffile.h>
#include "Ts_cmmfmiditestcontroller.h"

#include <mmf/common/mmfmidi.h>



/*
_LIT(KMmfTestControllerCategory, "CMMFTestController");
GLDEF_C void Panic(TMmfTestControllerPanic aReason)
	{
	User::Panic(KMmfTestControllerCategory, aReason);
	}
*/
_LIT(KMidiClntChunk, "MidiClntChunk");
_LIT(KMidiClntSemaphore, "MidiClntSemaphore");


CMMFMidiTestController* CMMFMidiTestController::NewL()
	{
	CMMFMidiTestController* self = CMMFMidiTestController::NewLC();
	CleanupStack::Pop();
	return self;
	}

CMMFMidiTestController* CMMFMidiTestController::NewLC(void)
	{
	CMMFMidiTestController* self = new (ELeave) CMMFMidiTestController();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMMFMidiTestController::CMMFMidiTestController()
	{
	iPlayBackRate = 1001;
	iMinRate = 1002;
	iMaxRate = 1003;
	iTempoMicroBeatsPerMinute = 1004;
	iPitchTranspositionCents= 1005;
	iCentsApplied = 1006;
	iDurationMicroSeconds = 1007;
	iDurationMicroBeats = 1008;
	iNumTracks = 1009;
	iPositionMicroSeconds = 1010;
	iPositionMicroBeats = 1011;
	iNumBanks = 1012;
	iNumInstruments = 1013;
	iStopTime = 999;
	iNumNotes = 57;
	iChannels = 15;
	iMaxVol = 60000;
	iMaxVolume = 100000;
	iBalance = 2;
	iRepeatNumberOfTimes = 0;
	iTrailingSilence = TTimeIntervalMicroSeconds(0);
	iCurrentMidiTestControllerState = EMidiStateClosedDisengaged;
	}

CMMFMidiTestController::~CMMFMidiTestController(void)
	{
	delete iDelayEvent;
	delete iMidiEventGenerator;
	delete iChanVolumeValues;
	}
                    
void CMMFMidiTestController::ConstructL (void)
	{
	iDelayEvent = CControllerTimer::NewL(this);
	CActiveScheduler::Add(iDelayEvent);

	iMidiEventGenerator = CMidiEventGenerator::NewL(this);
	CActiveScheduler::Add(iMidiEventGenerator);

	//TUid uid = {0x101F945C};
	CMidiCustomCommandParser* midiParser = CMidiCustomCommandParser::NewL(*this);
	iMidiEventHandler = midiParser;
	CleanupStack::PushL(midiParser);
	AddCustomCommandParserL(*midiParser);
	CleanupStack::Pop();
	
	CMMFDRMCustomCommandParser* drmParser = CMMFDRMCustomCommandParser::NewL(*this);
	CleanupStack::PushL(drmParser);
	AddCustomCommandParserL(*drmParser);
	CleanupStack::Pop();
	
	iChanVolumeValues = new(ELeave)CArrayFixFlat<TReal32>(KMaxChannelNumber);
	if (iChanVolumeValues)
		{
		for (TInt i=0; i<=KMaxChannelNumber; i++)
			iChanVolumeValues->AppendL(0);
		}
	}

// The method below need to be filled in with the correct responses

void CMMFMidiTestController::AddDataSourceL(MDataSource& aDataSource)
	{
	iDataSource = &aDataSource;
	iDelayEvent->DelayOpen(TTimeIntervalMicroSeconds(1000000));//1sec
	}

void CMMFMidiTestController::AddDataSinkL(MDataSink& /*aDataSink*/)
	{
	}

void CMMFMidiTestController::RemoveDataSourceL(MDataSource& /*aDataSource*/)
	{
	iDataSource = NULL;
	}

void CMMFMidiTestController::RemoveDataSinkL(MDataSink& /*aDataSink*/)
	{
	}

void CMMFMidiTestController::RemoveSourceSinkL()
	{
	}


void CMMFMidiTestController::ResetL() //XXX - replacement for RemoveSourceSink()
	{
	}

/*No leave happens when there is no iDataSource here because a
  user suppose to perform Midi Live Event even when there 
  is no resource (file) opened.*/
void CMMFMidiTestController::PrimeL()
	{
	 if (iDataSource)
		{
		iDataSource->SourcePrimeL();
		}
	}

void CMMFMidiTestController::PlayL()
	{
	if (iDataSource)
		{
		if (!iDisableAutoIntent)
			{
			User::LeaveIfError( MdcExecuteIntent(ContentAccess::EPlay) );
			}
		}	
	iDelayEvent->DelayPlayStarted(TTimeIntervalMicroSeconds(100000));//1 sec
	}

void CMMFMidiTestController::PauseL()
	{
	if (iDataSource)
		{
		iDataSource->SourcePauseL();
		}
	else
		{
		User::Leave(KErrNotReady);
		}			
	}

void CMMFMidiTestController::StopL()
	{
	if(iDelayEvent->IsActive())
		{
		iDelayEvent->Cancel();
		}
	iDelayEvent->DelayStop(TTimeIntervalMicroSeconds(0));
	if (iDataSource)
		{
		iDataSource->SourceStopL();
		}
	}

TTimeIntervalMicroSeconds CMMFMidiTestController::PositionL() const
	{
	if (!iDataSource)
		{
		User::Leave(KErrNotReady);
		}
	return iPositionMicroSeconds;				

	}

void CMMFMidiTestController::SetPositionL(const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	}

TTimeIntervalMicroSeconds CMMFMidiTestController::DurationL() const
	{
	return iDurationMicroSeconds;
	}

void CMMFMidiTestController::CustomCommand(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFDRMControl)
		{
		aMessage.Complete(KErrNotSupported);
		}
	else
		{
		aMessage.Complete(KErrNone);
		}
	}

void CMMFMidiTestController::SetPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

void CMMFMidiTestController::GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries)
	{
	aNumberOfEntries = 10;
	}

CMMFMetaDataEntry* CMMFMidiTestController::GetMetaDataEntryL(TInt /*aIndex*/)
	{ 
	CMMFMetaDataEntry* metaDataEntry = NULL;
	return metaDataEntry;
	}

TInt CMMFMidiTestController::SendEventToClient(const TMMFEvent& aEvent)
	{
	//send event to client...
	TInt result = DoSendEventToClient(aEvent);
	return result;
	}

TInt CMMFMidiTestController::OpenComplete()
	{
	CMMFMidiEvent* opencomplete = new CMMFMidiEvent(KMMFEventCategoryMidiOpenDataSourceComplete, KErrNone);
	if(!opencomplete)
		return KErrNoMemory;
	switch (iCurrentMidiTestControllerState)
	{
		case EMidiStateClosedDisengaged: 
			opencomplete->iOldState = EMidiStateClosedDisengaged;
			opencomplete->iNewState = EMidiStateOpenDisengaged;
			iCurrentMidiTestControllerState = EMidiStateOpenDisengaged;
			break;
		case EMidiStateOpenPlaying:
			opencomplete->iOldState = EMidiStateOpenPlaying;
			opencomplete->iNewState = EMidiStateOpenEngaged;
			iCurrentMidiTestControllerState = EMidiStateOpenEngaged;
			break;
		default:
			opencomplete->iOldState = iCurrentMidiTestControllerState;
			opencomplete->iNewState = iCurrentMidiTestControllerState;
			opencomplete->iEventType = KMMFErrorCategoryControllerGeneralError;
			break;
	}	
	opencomplete->iMicroSeconds = iDurationMicroSeconds;
	iMidiEventHandler->SendMidiEventToClient(*opencomplete);
	delete opencomplete;
	return KErrNone;
	}

TInt CMMFMidiTestController::PlayStarted()
	{
	CMMFMidiEvent* playStarted = new CMMFMidiEvent(KMMFEventCategoryMidiPlaying, KErrNone);
	if(!playStarted)
		return KErrNoMemory;
	// decide which is the state the MIDI controller can go into when Play is called
	switch (iCurrentMidiTestControllerState)
		{
		case EMidiStateClosedDisengaged:
		playStarted->iOldState = EMidiStateClosedDisengaged;
		playStarted->iNewState = EMidiStateClosedEngaged;
		iCurrentMidiTestControllerState = EMidiStateClosedEngaged;
		playStarted->iMicroSeconds = TTimeIntervalMicroSeconds(0);
		iMidiEventHandler->SendMidiEventToClient(*playStarted);
		delete playStarted; 
		iDelayEvent->PlayFinished(TTimeIntervalMicroSeconds(20000000));//20 sec
		iMidiEventGenerator->MidiEventTrigger();
		break;
	
		case EMidiStateOpenDisengaged:
		playStarted->iOldState = EMidiStateOpenDisengaged;
		playStarted->iNewState = EMidiStateOpenPlaying;
		iCurrentMidiTestControllerState = EMidiStateOpenPlaying;
		playStarted->iMicroSeconds = TTimeIntervalMicroSeconds(0);
		iMidiEventHandler->SendMidiEventToClient(*playStarted);
		delete playStarted; 
		iDelayEvent->PlayFinished(TTimeIntervalMicroSeconds(20000000));//20 sec
		iMidiEventGenerator->MidiEventTrigger();
		break;
		
		case EMidiStateOpenEngaged:
		playStarted->iOldState = EMidiStateOpenEngaged;
		playStarted->iNewState = EMidiStateOpenPlaying;
		iCurrentMidiTestControllerState = EMidiStateOpenPlaying;
		playStarted->iMicroSeconds = TTimeIntervalMicroSeconds(0);
		iMidiEventHandler->SendMidiEventToClient(*playStarted);
		delete playStarted; 
		iDelayEvent->PlayFinished(TTimeIntervalMicroSeconds(20000000));//20 sec
		iMidiEventGenerator->MidiEventTrigger();
		break;
		
		default:
		playStarted->iOldState = iCurrentMidiTestControllerState;
		playStarted->iNewState = iCurrentMidiTestControllerState;
		playStarted->iEventType = KMMFErrorCategoryControllerGeneralError;
		playStarted->iMicroSeconds = TTimeIntervalMicroSeconds(0);
		iMidiEventHandler->SendMidiEventToClient(*playStarted);
		delete playStarted; 
		break;
		}
	return KErrNone;
	}

TInt CMMFMidiTestController::PlayFinished()
	{
	TUid tmp;
	tmp.iUid = 0 ;
	CMMFMidiEvent* playFinished = new CMMFMidiEvent(tmp, KErrNone);
	if(!playFinished)
		return KErrNoMemory;
	switch (iCurrentMidiTestControllerState)
		{
		case EMidiStateClosedEngaged:
		playFinished->iOldState = EMidiStateClosedEngaged;
		playFinished->iNewState = EMidiStateClosedEngaged;
		iCurrentMidiTestControllerState = EMidiStateClosedEngaged;
		break;
	
		case EMidiStateOpenPlaying:
		playFinished->iOldState = EMidiStateOpenPlaying;
		playFinished->iNewState = EMidiStateOpenEngaged;
		iCurrentMidiTestControllerState = EMidiStateOpenEngaged;
		break;
			
		default:
		playFinished->iOldState = iCurrentMidiTestControllerState;
		playFinished->iNewState = iCurrentMidiTestControllerState;
		playFinished->iEventType = KMMFErrorCategoryControllerGeneralError;
		break;
		}
		
	playFinished->iMicroSeconds = iDurationMicroSeconds;
	iMidiEventHandler->SendMidiEventToClient(*playFinished);
	delete playFinished;
	return KErrNone;
	}

TInt CMMFMidiTestController::Stop()
	{
	CMMFMidiEvent* playStop = new CMMFMidiEvent(KMMFEventCategoryMidiPlayingComplete, KErrNone);
	if(!playStop)
		return KErrNoMemory;
	
	switch (iCurrentMidiTestControllerState)
		{
		case EMidiStateOpenPlaying:
		playStop->iOldState = EMidiStateOpenPlaying;
		playStop->iNewState = EMidiStateOpenDisengaged;
		iCurrentMidiTestControllerState = EMidiStateOpenDisengaged;
		break;
		
		case EMidiStateClosedEngaged:
		playStop->iOldState = EMidiStateClosedEngaged;
		playStop->iNewState = EMidiStateClosedDisengaged;
		iCurrentMidiTestControllerState = EMidiStateClosedDisengaged;
		break;
		
		case EMidiStateOpenEngaged:
		playStop->iOldState = EMidiStateOpenEngaged;
		playStop->iNewState = EMidiStateOpenDisengaged;
		iCurrentMidiTestControllerState = EMidiStateOpenDisengaged;
		break;
		
		default:
		playStop->iOldState = iCurrentMidiTestControllerState;
		playStop->iNewState = iCurrentMidiTestControllerState;
		playStop->iEventType = KMMFErrorCategoryControllerGeneralError;
		break;
		}
	
	playStop->iMicroSeconds = iPositionMicroSeconds;
	iMidiEventHandler->SendMidiEventToClient(*playStop);
	delete playStop;
	return KErrNone;
	}

TInt CMMFMidiTestController::Close()
	{
	CMMFMidiEvent* close = new CMMFMidiEvent(KMMFEventCategoryMidiClose, KErrNone);
	if(!close)
		return KErrNoMemory;
	
	switch (iCurrentMidiTestControllerState)
		{
		case EMidiStateOpenDisengaged:
		close->iOldState = EMidiStateOpenDisengaged;
		close->iNewState = EMidiStateClosedDisengaged;
		iCurrentMidiTestControllerState = EMidiStateClosedDisengaged;
		break;
		
		case EMidiStateOpenPlaying:
		close->iOldState = EMidiStateOpenPlaying;
		close->iNewState = EMidiStateClosedDisengaged;
		iCurrentMidiTestControllerState = EMidiStateClosedEngaged;
		break;
		
		case EMidiStateOpenEngaged:
		close->iOldState = EMidiStateOpenEngaged;
		close->iNewState = EMidiStateClosedEngaged;
		iCurrentMidiTestControllerState = EMidiStateClosedEngaged;
		break;
		
		default:
		close->iOldState = iCurrentMidiTestControllerState;
		close->iNewState = iCurrentMidiTestControllerState;
		close->iEventType = KMMFErrorCategoryControllerGeneralError;
		break;
		}
	close->iMicroSeconds = TTimeIntervalMicroSeconds(0);
	iMidiEventHandler->SendMidiEventToClient(*close);
	delete close;
	return KErrNone;
	}

TInt CMMFMidiTestController::SyncUpdate()
	{
	CMMFMidiEvent* syncUpd = new CMMFMidiEvent(KMMFEventCategoryMidiSyncUpdate, KErrNone);
	if(!syncUpd)
		return KErrNoMemory;
	iMidiEventHandler->SendMidiEventToClient(*syncUpd);
	delete syncUpd;
	return KErrNone;
	}

TInt CMMFMidiTestController::TempoChanged()
	{
	CMMFMidiEvent* tempoChanged = new CMMFMidiEvent(KMMFEventCategoryTempoChanged, KErrNone);
	if(!tempoChanged)
		return KErrNoMemory;
	tempoChanged->iTempoMicroBeats = 60; //Adagio
	iMidiEventHandler->SendMidiEventToClient(*tempoChanged);
	delete tempoChanged;
	iMidiEventGenerator->MidiEventTrigger();
	return KErrNone;
	}

TInt CMMFMidiTestController::VolumeChanged()
	{
	CMMFMidiEvent* volumeChanged = new CMMFMidiEvent(KMMFEventCategoryVolumeChanged, KErrNone);
	if(!volumeChanged)
		return KErrNoMemory;
	volumeChanged->iVolumeInDecibels = 50000;
	volumeChanged->iChannel = 14;
	iMidiEventHandler->SendMidiEventToClient(*volumeChanged);
	delete volumeChanged;
	iMidiEventGenerator->MidiEventTrigger();
	return KErrNone;
	}

TInt CMMFMidiTestController::MuteChanged()
	{
	CMMFMidiEvent* muteChanged = new CMMFMidiEvent(KMMFEventCategoryMuteChanged, KErrNone);
	if(!muteChanged)
		return KErrNoMemory;
	muteChanged->iChannel = 15;
	muteChanged->iMute = ETrue;
	iMidiEventHandler->SendMidiEventToClient(*muteChanged);
	delete muteChanged;
	iMidiEventGenerator->MidiEventTrigger();
	return KErrNone;
	}

TInt CMMFMidiTestController::PolyphonyChanged()
	{
	CMMFMidiEvent* polyphonyChanged = new CMMFMidiEvent(KMMFEventCategoryPolyphonyChanged, KErrNone);
	if(!polyphonyChanged)
		return KErrNoMemory;
	polyphonyChanged->iPolyphony = 10;
	iMidiEventHandler->SendMidiEventToClient(*polyphonyChanged);
	delete polyphonyChanged;
	iMidiEventGenerator->MidiEventTrigger();
	return KErrNone;
	}

TInt CMMFMidiTestController::InstrumentChanged()
	{
	CMMFMidiEvent* instrumentChanged = new CMMFMidiEvent(KMMFEventCategoryInstrumentChanged, KErrNone);
	if(!instrumentChanged )
		return KErrNoMemory;
	instrumentChanged->iChannel = 3;
	instrumentChanged->iBankId = 4;
	instrumentChanged->iInstrumentId = 5; 
	iMidiEventHandler->SendMidiEventToClient(*instrumentChanged);
	delete instrumentChanged ;
	iMidiEventGenerator->MidiEventTrigger();
	return KErrNone;
	}

TInt CMMFMidiTestController::SmfMetaDataEntryFound()
	{
	TTimeIntervalMicroSeconds position(200000);
	CMMFMidiEvent* smfMetaDataEntryFound = new CMMFMidiEvent(KMMFEventCategoryMetaDataEntryFound, KErrNone);
	if(!smfMetaDataEntryFound )
		return KErrNoMemory;
	smfMetaDataEntryFound->iMetaDataEntryId = 15;//Not supported in this test controller
	smfMetaDataEntryFound->iMicroSeconds = position;
	iMidiEventHandler->SendMidiEventToClient(*smfMetaDataEntryFound);
	delete smfMetaDataEntryFound;
	iMidiEventGenerator->MidiEventTrigger();
	return KErrNone;
	}

TInt CMMFMidiTestController::MipMessageReceived()
	{
	TMipMessageEntry mipMessageEntry;
	mipMessageEntry.iChannel = 11;
	mipMessageEntry.iMIPValue = 21;

	TMipMessageEntry mipMessageEntry2;
	mipMessageEntry2.iChannel = 1;
	mipMessageEntry2.iMIPValue = 2;

	CMMFMidiEvent* mipMessageReceived = new CMMFMidiEvent(KMMFEventCategoryMipMessageReceived, KErrNone);
	if(!mipMessageReceived )
		return KErrNoMemory;
	mipMessageReceived->iMipMessage.Append(mipMessageEntry);

	CMMFMidiEvent* mipMessageReceived2 = new CMMFMidiEvent(KMMFEventCategoryMipMessageReceived, KErrNone);
	if(!mipMessageReceived2 )
		return KErrNoMemory;
	mipMessageReceived2->iMipMessage.Append(mipMessageEntry2);

	
	iMidiEventHandler->SendMidiEventToClient(*mipMessageReceived);
	iMidiEventHandler->SendMidiEventToClient(*mipMessageReceived2);
	delete mipMessageReceived;
	delete mipMessageReceived2;
	return KErrNone;
	}

void CMMFMidiTestController::MmcSetPositionMicroBeatsL(TInt64 /*aMicroBeats*/)
	{
	InsertInterfaceNameInChunk(_L8("SetPositionMicroBeatsL"));
	}

void CMMFMidiTestController::MmcPositionMicroBeatsL(TInt64& aMicroBeats)
	{
	aMicroBeats = iPositionMicroBeats;
	InsertInterfaceNameInChunk(_L8("PositionMicroBeatsL"));
	}

void CMMFMidiTestController::MmcPlayNoteL(TInt /*aChannel*/,TInt /*aNote*/,const TTimeIntervalMicroSeconds& /*aDuration*/,TInt /*aNoteOnVelocity*/,TInt /*aNoteOffVelocity*/)
	{
	InsertInterfaceNameInChunk(_L8("PlayNoteL"));
	}

void CMMFMidiTestController::MmcPlayNoteL(TInt /*aChannel*/,TInt /*aNote*/,const TTimeIntervalMicroSeconds& /*aStartTime*/, const TTimeIntervalMicroSeconds& /*aDuration*/,TInt /*aNoteOnVelocity*/,TInt /*aNoteOffVelocity*/)
	{
	InsertInterfaceNameInChunk(_L8("PlayNoteWithStartTime"));
	}

void CMMFMidiTestController::MmcStopNotesL(TInt /*aChannel*/)
	{
	InsertInterfaceNameInChunk(_L8("StopNotes"));
	}

void CMMFMidiTestController::MmcNoteOnL(TInt /*aChannel*/,TInt /*aNote*/,TInt /*aVelocity*/)
	{
	InsertInterfaceNameInChunk(_L8("NoteOnL"));
	}

void CMMFMidiTestController::MmcNoteOffL(TInt /*aChannel*/,TInt /*aNote*/,TInt /*aVelocity*/)
	{
	InsertInterfaceNameInChunk(_L8("NoteOffL"));
	}

void CMMFMidiTestController::MmcPlaybackRateL(TInt& aPlayBackRate)
	{
	aPlayBackRate = iPlayBackRate;
	InsertInterfaceNameInChunk(_L8("PlaybackRateL"));
	}

void CMMFMidiTestController::MmcSetPlaybackRateL(TInt /*aPlayBackRate*/)
	{
	InsertInterfaceNameInChunk(_L8("SetPlaybackRateL"));
	}

void CMMFMidiTestController::MmcMaxPlaybackRateL(TInt& aMaxRate)
	{
	aMaxRate = iMaxRate;
	InsertInterfaceNameInChunk(_L8("MaxPlaybackRateL"));
	}

void CMMFMidiTestController::MmcMinPlaybackRateL(TInt& aMinRate)
	{
	aMinRate = iMinRate;
	InsertInterfaceNameInChunk(_L8("MinPlaybackRateL"));
	}

void CMMFMidiTestController::MmcTempoMicroBeatsPerMinuteL(TInt& aMicroBeatsPerMinute)
	{
	aMicroBeatsPerMinute = iTempoMicroBeatsPerMinute;
	InsertInterfaceNameInChunk(_L8("TempoMicroBeatsPerMinuteL"));
	}

void CMMFMidiTestController::MmcSetTempoL(TInt /*aMicroBeatsPerMinute*/)
	{
	InsertInterfaceNameInChunk(_L8("SetTempoL"));
	}

void CMMFMidiTestController::MmcPitchTranspositionCentsL(TInt& aPitch)
	{
	aPitch = iPitchTranspositionCents;
	InsertInterfaceNameInChunk(_L8("PitchTranspositionCentsL"));
	}

void CMMFMidiTestController::MmcSetPitchTranspositionL(TInt /*aCents*/, TInt& aCentsApplied)
	{
	aCentsApplied = iCentsApplied; 
	InsertInterfaceNameInChunk(_L8("SetPitchTranspositionL"));
	}

void CMMFMidiTestController::MmcDurationMicroBeatsL(TInt64& aDuration)
	{
	aDuration = iDurationMicroBeats;
	InsertInterfaceNameInChunk(_L8("DurationMicroBeatsL"));
	}

void CMMFMidiTestController::MmcNumTracksL(TInt& aTracks)
	{
	aTracks = iNumTracks;
	InsertInterfaceNameInChunk(_L8("NumTracksL"));
	}

void CMMFMidiTestController::MmcSetTrackMuteL(TInt /*aTrack*/, TBool /*aMuted*/)
	{
	InsertInterfaceNameInChunk(_L8("SetTrackMuteL"));
	}

void CMMFMidiTestController::MmcMimeTypeL(TDes8& aMimeType)
	{
	_LIT(K,"Midi-x");
	aMimeType.Copy(K);
	InsertInterfaceNameInChunk(_L8("MimeTypeL"));
	}

void CMMFMidiTestController::MmcSetSyncUpdateCallbackIntervalL(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 /*aMicroBeats*/)
	{
	InsertInterfaceNameInChunk(_L8("SetSyncUpdateCallbackIntervalL"));
	iDelayEvent->DelaySyncUpdate(aMicroSeconds);
	}

void CMMFMidiTestController::MmcSendMessageL(const TDesC8& aMidiMessage, TInt& aBytes)
	{
	aBytes = aMidiMessage.Length();
	InsertInterfaceNameInChunk(_L8("SendMessageL"));
	}

void CMMFMidiTestController::MmcSendMessageL(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& /*aTime*/, TInt& aBytes)
	{
	aBytes = aMidiMessage.Length();
	InsertInterfaceNameInChunk(_L8("SendMessageWithTimeStamp"));
	}

void CMMFMidiTestController::MmcSendMipMessageL(const TArray<TMipMessageEntry>& aEntry)
	{
	for(TInt i=0; i< aEntry.Count(); i++)
		{
		TMipMessageEntry tmp = aEntry[i];
		tmp.iChannel++;//to remove the warning
		}
	InsertInterfaceNameInChunk(_L8("SendMipMessageL"));
	}

void CMMFMidiTestController::MmcNumberOfBanksL(TBool /*aCustom*/, TInt& aNumBanks)
	{
	aNumBanks = iNumBanks;
	InsertInterfaceNameInChunk(_L8("NumberOfBanksL"));
	}

void CMMFMidiTestController::MmcGetBankIdL(TBool /*aCustom*/, TInt aBankIndex, TInt& aBankId)
	{
	aBankId = ++aBankIndex;
	InsertInterfaceNameInChunk(_L8("GetBankIdL"));
	}

void CMMFMidiTestController::MmcLoadCustomBankL(const TDesC& /*aFileName*/,TInt& aBankId)
	{
	aBankId = 60;
	InsertInterfaceNameInChunk(_L8("LoadCustomBankL"));
	}

void CMMFMidiTestController::MmcUnloadCustomBankL(TInt /*aBankId*/)
	{
	InsertInterfaceNameInChunk(_L8("UnloadCustomBankL"));
	}

void CMMFMidiTestController::MmcCustomBankLoadedL(TInt /*aBankId*/, TBool& aBankLoaded)
	{
	aBankLoaded = ETrue;
	InsertInterfaceNameInChunk(_L8("CustomBankLoadedL"));
	}

void CMMFMidiTestController::MmcUnloadAllCustomBanksL()
	{
	InsertInterfaceNameInChunk(_L8("UnloadAllCustomBanksL"));
	}

void CMMFMidiTestController::MmcNumberOfInstrumentsL(TInt /*aBankId*/, TBool /*aCustom*/, TInt& aNumInstruments)
	{
	aNumInstruments = iNumInstruments;
	InsertInterfaceNameInChunk(_L8("NumberOfInstrumentsL"));
	}

void CMMFMidiTestController::MmcGetInstrumentIdL(TInt /*aBankId*/,TBool /*aCustom*/,TInt /*aInstrumentIndex*/, TInt& aInstrumentId)
	{
	aInstrumentId = 89;
	InsertInterfaceNameInChunk(_L8("GetInstrumentIdL"));
	}

const TDesC& CMMFMidiTestController::MmcInstrumentNameL(TInt /*aBankId*/, TBool /*aCustom*/, TInt /*aInstrumentId*/)
	{	
	_LIT(KInstrumentName, "Guitar");
	InsertInterfaceNameInChunk(_L8("InstrumentNameL"));
	return KInstrumentName;
	}

void CMMFMidiTestController::MmcSetInstrumentL(TInt /*aChannel*/,TInt /*aBankId*/,TInt /*aInstrumentId*/)
	{
	InsertInterfaceNameInChunk(_L8("SetInstrumentL"));
	}

void CMMFMidiTestController::MmcLoadCustomInstrumentL(const TDesC& /*aFileName*/, TInt /*aFileBankId*/, TInt /*aFileInstrumentId*/, TInt /*aMemoryBankId*/, TInt /*aMemoryInstrumentId*/)
	{
	InsertInterfaceNameInChunk(_L8("LoadCustomInstrumentL"));
	}

void CMMFMidiTestController::MmcUnloadCustomInstrumentL(TInt /*aCustomBankId*/,TInt /*aInstrumentId*/)
	{
	InsertInterfaceNameInChunk(_L8("UnloadCustomInstrumentL"));
	}

const TDesC& CMMFMidiTestController::MmcPercussionKeyNameL(TInt /*aNote*/, TInt /*aBankId*/, TBool /*aCustom*/, TInt /*aInstrumentId*/)
	{
	_LIT(KPercussionKeyName, "Do");
	InsertInterfaceNameInChunk(_L8("PercussionKeyNameL"));
	return KPercussionKeyName;
	}

void CMMFMidiTestController::MmcStopTimeL(TTimeIntervalMicroSeconds& aStopTime)
	{
	aStopTime = iStopTime;
	InsertInterfaceNameInChunk(_L8("StopTimeL"));
	}

void CMMFMidiTestController::MmcSetStopTimeL(const TTimeIntervalMicroSeconds& /*aStopTime*/)
	{
	InsertInterfaceNameInChunk(_L8("SetStopTimeL"));
	}

void CMMFMidiTestController::MmcPolyphonyL(TInt& aNumNotes)
	{
	aNumNotes = iNumNotes;
	InsertInterfaceNameInChunk(_L8("PolyphonyL"));
	}

void CMMFMidiTestController::MmcMaxPolyphonyL(TInt& aMaxNotes)
	{
	aMaxNotes = iMaxNotes;
	InsertInterfaceNameInChunk(_L8("MaxPolyphonyL"));
	}


void CMMFMidiTestController::MmcChannelsSupportedL(TInt& aChannels)
	{
	aChannels = iChannels;
	InsertInterfaceNameInChunk(_L8("ChannelsSupportedL"));
	}

void CMMFMidiTestController::MmcChannelVolumeL(TInt aChannel, TReal32& aChannelVol)
	{
	aChannelVol = iChanVolumeValues->At(aChannel);
	InsertInterfaceNameInChunk(_L8("ChannelVolumeL"));
	}

void CMMFMidiTestController::MmcMaxChannelVolumeL(TReal32& aMaxVol)
	{
	aMaxVol = iMaxVol;
	InsertInterfaceNameInChunk(_L8("MaxChannelVolumeL"));
	}

void CMMFMidiTestController::MmcSetChannelVolumeL(TInt aChannel,TReal32 aVolume)
	{
	iChanVolumeValues->InsertL(aChannel, aVolume);
	InsertInterfaceNameInChunk(_L8("SetChannelVolumeL"));
	}

void CMMFMidiTestController::MmcSetChannelMuteL(TInt /*aChannel*/,TBool /*aMuted*/)
	{
	InsertInterfaceNameInChunk(_L8("SetChannelMuteL"));
	}

void CMMFMidiTestController::MmcVolumeL(TInt& aVolume)
	{
	aVolume = 70000;
	InsertInterfaceNameInChunk(_L8("VolumeL"));
	}

void CMMFMidiTestController::MmcMaxVolumeL(TInt& aMaxVolume)
	{
	aMaxVolume = iMaxVolume;
	InsertInterfaceNameInChunk(_L8("MaxVolumeL"));
	}

void CMMFMidiTestController::MmcSetVolumeL(TInt /*aVolume*/)
	{
	InsertInterfaceNameInChunk(_L8("SetVolumeL"));
	}

void CMMFMidiTestController::MmcSetVolumeRampL(const TTimeIntervalMicroSeconds& /*aRampDuration*/)
	{
	InsertInterfaceNameInChunk(_L8("SetVolumeRampL"));
	}

void CMMFMidiTestController::MmcGetBalanceL(TInt& aBalance)
	{
	aBalance = iBalance;
	InsertInterfaceNameInChunk(_L8("GetBalanceL"));
	}

void CMMFMidiTestController::MmcSetBalanceL(TInt /*aBalance*/)
	{
	InsertInterfaceNameInChunk(_L8("SetBalanceL"));
	}

void  CMMFMidiTestController::MmcLoadCustomBankDataL(const TDesC8& /*aBankData*/,TInt& aBankId)
	{
	aBankId  = 20;
	InsertInterfaceNameInChunk(_L8("LoadCustomBankDataL"));
	}

void  CMMFMidiTestController::MmcLoadCustomInstrumentDataL(const TDesC8& /*aInstrumentData*/, TInt /*aBankDataId*/, TInt /*aInstrumentDataId*/, TInt /*aMemoryBankId*/, TInt /*aMemoryInstrumentId*/)
	{
	InsertInterfaceNameInChunk(_L8("LoadCustomInstrumentDataL"));
	}

void  CMMFMidiTestController::MmcSetMaxPolyphonyL(TInt aMaxNotes)
	{
	iMaxNotes = aMaxNotes;
	InsertInterfaceNameInChunk(_L8("SetMaxPolyphonyL"));
	}

void  CMMFMidiTestController::MmcGetRepeatsL(TInt& aNumRepeats)
	{
	aNumRepeats = iRepeatNumberOfTimes;
	InsertInterfaceNameInChunk(_L8("GetRepeats"));
	}

void  CMMFMidiTestController::MmcSetRepeatsL(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	iRepeatNumberOfTimes = aRepeatNumberOfTimes;
	iTrailingSilence = aTrailingSilence;
	InsertInterfaceNameInChunk(_L8("SetRepeatsL"));
	}

void  CMMFMidiTestController::MmcSetBankL(TBool /*aCustom*/)
	{
	InsertInterfaceNameInChunk(_L8("SetBankL"));
	}

void  CMMFMidiTestController::MmcIsTrackMuteL(TInt /*aTrack*/, TBool& aTrackMute)
	{
	aTrackMute = ETrue;
	InsertInterfaceNameInChunk(_L8("IsTrackMuteL"));
	}

void  CMMFMidiTestController::MmcIsChannelMuteL(TInt /*aChannel*/, TBool& aChannelMute)
	{
	aChannelMute = ETrue;
	InsertInterfaceNameInChunk(_L8("IsChannelMuteL"));
	}

void  CMMFMidiTestController::MmcGetInstrumentL(TInt /*aChannel*/, TInt& aInstrumentId, TInt& aBankId)
	{
	aInstrumentId = 10;
	aBankId = 11;
	InsertInterfaceNameInChunk(_L8("GetInstrumentL"));
	}

void  CMMFMidiTestController::MmcCloseL()
	{
	iDelayEvent->DelayClose(TTimeIntervalMicroSeconds(0));
	}

void  CMMFMidiTestController::MmcStopL(const TTimeIntervalMicroSeconds& /*aFadeOutDuration*/)
	{
	if(iDelayEvent->IsActive())
		iDelayEvent->Cancel();
	if(iMidiEventGenerator->IsActive())
		iMidiEventGenerator->Cancel();

	iDelayEvent->DelayStop(TTimeIntervalMicroSeconds(0));
	}



//
//
//
CMMFMidiTestController::CControllerTimer* CMMFMidiTestController::CControllerTimer::NewL(CMMFMidiTestController* aController)
	{
	CControllerTimer* self = new (ELeave) CControllerTimer(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMMFMidiTestController::CControllerTimer::CControllerTimer(CMMFMidiTestController* aController)
	:CTimer(EPriorityStandard)
	{
	iController = aController;
	}

void CMMFMidiTestController::CControllerTimer::DelayOpen(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrOpenComplete;
	After(I64INT(aDelay.Int64()));
	}

void CMMFMidiTestController::CControllerTimer::DelayPlayStarted(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrDelayPlayStarted;
	After(I64INT(aDelay.Int64()));
	}

void CMMFMidiTestController::CControllerTimer::PlayFinished(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrPlayFinished;
	After(I64INT(aDelay.Int64()));
	}

void CMMFMidiTestController::CControllerTimer::DelaySyncUpdate(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrSyncUpdate;
	After(I64INT(aDelay.Int64()));
	}

void CMMFMidiTestController::CControllerTimer::DelayStop(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrDelayStop;
	After(I64INT(aDelay.Int64()));
	}

void CMMFMidiTestController::CControllerTimer::DelayClose(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrDelayClose;
	After(I64INT(aDelay.Int64()));
	}

void CMMFMidiTestController::CControllerTimer::RunL()
	{
	ASSERT(iController);
	switch(iType) 
		{
		case ETmrOpenComplete :
			User::LeaveIfError(iController->OpenComplete());
			break;
		case ETmrSyncUpdate :
			User::LeaveIfError(iController->SyncUpdate());
			break;
		case ETmrDelayPlayStarted :
			User::LeaveIfError(iController->PlayStarted());
			break;
		case ETmrPlayFinished :
			User::LeaveIfError(iController->PlayFinished());
			break;
		case ETmrDelayStop :
			User::LeaveIfError(iController->Stop());
			break;
		case ETmrDelayClose :
			User::LeaveIfError(iController->Close());
			break;
		}
	}
//
//
//
CMMFMidiTestController::CMidiEventGenerator* CMMFMidiTestController::CMidiEventGenerator::NewL(CMMFMidiTestController* aController)
	{
	CMidiEventGenerator* self = new (ELeave) CMidiEventGenerator(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMMFMidiTestController::CMidiEventGenerator::CMidiEventGenerator(CMMFMidiTestController* aController)
	:CTimer(EPriorityStandard)
	{
	iController = aController;
	iDelay = TTimeIntervalMicroSeconds(0);//2 secs
	iTriggerCount = 0;
	}

void CMMFMidiTestController::CMidiEventGenerator::MidiEventTrigger()
	{
	switch(iTriggerCount) 
		{
		case  0:
			iType = EEventTempoChanged;
			break;
		case  1:
			iType = EEventVolumeChanged;
			break;
		case  2:
			iType = EEventMuteChanged;
			break;
		case  3:
			iType = EEventPolyphonyChanged;
			break;
		case  4:
			iType = EEventInstrumentChanged;
			break;
		case  5:
			iType = EEventSmfMetaDataEntryFound;
			break;
		case  6:
			iType = EEventVolumeChanged;
			break;
		case  7:
			iType = EEventMuteChanged;
			break;
		case  8:
			iType = EEventPolyphonyChanged;
			break;
		case  9:
			iType = EEventInstrumentChanged;
			break;
		case  10:
			iType = EEventSmfMetaDataEntryFound;
			break;
		case  11:
			iType = EEventMipMessageReceived;
			break;
		}
	++iTriggerCount;
	After(I64INT(iDelay.Int64()));
	}

/*
void CMMFMidiTestController::CMidiEventGenerator::DelaySyncUpdate(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrSyncUpdate;
	After(I64INT(aDelay.Int64()));
	}
*/

void CMMFMidiTestController::CMidiEventGenerator::RunL()
	{
	ASSERT(iController);
	
	switch(iType) 
		{
		case  EEventTempoChanged:
			User::LeaveIfError(iController->TempoChanged());
			break;
		case  EEventVolumeChanged:
			User::LeaveIfError(iController->VolumeChanged());
			break;
		case  EEventMuteChanged:
			User::LeaveIfError(iController->MuteChanged());
			break;
		case  EEventPolyphonyChanged:
			User::LeaveIfError(iController->PolyphonyChanged());
			break;
		case  EEventInstrumentChanged:
			User::LeaveIfError(iController->InstrumentChanged());
			break;
		case  EEventSmfMetaDataEntryFound:
			User::LeaveIfError(iController->SmfMetaDataEntryFound());
			break;
		case  EEventMipMessageReceived:
			User::LeaveIfError(iController->MipMessageReceived());
			break;
		}
	}

void CMMFMidiTestController::InsertInterfaceNameInChunk(const TDesC8& aInterfaceName)
	{
	TFindChunk findChunk(KMidiClntChunk);
	TFullName chunkName;
	TInt err = findChunk.Next(chunkName);

	if(err == KErrNone)
		{
		RChunk chunk;
		chunk.OpenGlobal(KMidiClntChunk,EFalse);
		TUint8* base = chunk.Base();
		for(TInt i=0; i<40 ; i++)
			{
			*base = 0x0;
			base++;
			}
		TBuf8<40> buf(aInterfaceName);
		TPtr8 ptr(chunk.Base(),chunk.Size());
		ptr = buf;
		chunk.Close();
		RSemaphore sem;
		sem.OpenGlobal(KMidiClntSemaphore);
		sem.Signal();
		sem.Close();
		}
	}

TInt CMMFMidiTestController::MdcEvaluateIntent(ContentAccess::TIntent aIntent) 
   {
   if (iDataSource == NULL)
	   	{
	   	return KErrNotReady;
		}      
   if (iDataSource && iDataSource->DataSourceType()==KUidMmfFileSource)
      {
      CMMFFile* file = static_cast<CMMFFile*>(iDataSource);
      TInt err = file->EvaluateIntent(aIntent);
      return err;
      }
   else
      {
      // Evaluating intent will always succeed on sinks that 
      // don't support DRM
      return KErrNone;
      }
   
   }
   
TInt CMMFMidiTestController::MdcExecuteIntent(ContentAccess::TIntent aIntent)
   {
   	if (iDataSource == NULL)
	   	{
	   	return KErrNotReady;
		}   
   	if (iDataSource->DataSourceType()==KUidMmfFileSource)
	      {
	      CMMFFile* file = static_cast<CMMFFile*>(iDataSource);
	      TInt err = file->ExecuteIntent(aIntent);
	      return err;
	      }
   	else
	      {
	      // Executing intent will always succeed on sinks that 
	      // don't support DRM
	      return KErrNone;
	      }
   }
   
TInt CMMFMidiTestController::MdcDisableAutomaticIntent(TBool aDisableAutoIntent)
   {
   iDisableAutoIntent = aDisableAutoIntent;
   return KErrNone;
   }
   
   
TInt CMMFMidiTestController::MdcSetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)
   {
   if (iDataSource == NULL)
   	{
   	return KErrNotReady;
   	}
   	
   if (iDataSource->DataSourceType()==KUidMmfFileSource)
      {
      CMMFFile* file = static_cast<CMMFFile*>(iDataSource);
      TInt err = file->SetAgentProperty(aProperty, aValue);
      return err;
      }
   else
      {
      return KErrNone;
      }
   }

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfMidiTestControllerUid,	CMMFMidiTestController::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

