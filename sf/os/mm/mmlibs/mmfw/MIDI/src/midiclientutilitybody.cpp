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

#include "midiclientutilitybody.h"

const TInt KMimeLength         = 256;
const TInt KMinChannel         = 0;
const TInt KMaxChannel         = 15;
const TInt KMinNote            = 0;
const TInt KMaxNote            = 127;
const TInt KMinNoteOnVelocity  = 0;
const TInt KMaxNoteOnVelocity  = 127;
const TInt KMinNoteOffVelocity = 0;
const TInt KMaxNoteOffVelocity = 127;
const TInt KMinInstrumentId    = 0;
const TInt KMaxInstrumentId    = 127;


CMidiClientUtility::CBody* CMidiClientUtility::CBody::NewL(CMidiClientUtility* aParent,
														   MMidiClientUtilityObserver& aObserver, 
														   TInt aPriority, 
														   TInt aPref,
														   TBool aUseSharedHeap)
	{
	CBody* self = new(ELeave) CBody(aParent, aObserver, aPriority, aPref);
	CleanupStack::PushL(self);
	self->ConstructL(aUseSharedHeap);
	CleanupStack::Pop(self);
	return self;
	}

CMidiClientUtility::CBody::CBody(CMidiClientUtility* aParent, 
								 MMidiClientUtilityObserver& aObserver, 
								 TInt aPriority, 
								 TInt aPref) :
	iObserver(aObserver),
	iMidiControllerCommands(iController),
	iDRMCustomCommands(iController)

	{
	iParent = aParent;
	iState = EMidiStateClosedDisengaged;
	iPrioritySettings.iPriority = aPriority;
	iPrioritySettings.iPref = aPref;
	iIntervalSec = ETrue;
	iStopPosition = TTimeIntervalMicroSeconds(0);
	}

CMidiClientUtility::CBody::~CBody()
	{
	delete iAddDataSourceSinkAsync;
	if (iMidiControllerEventMonitor)
		iMidiControllerEventMonitor->Cancel();
		
	iController.Close();
	delete iMidiControllerEventMonitor;
	delete iMimeType;
	delete iRepeatTrailingSilenceTimer;
	delete iSource;
	}

void CMidiClientUtility::CBody::ConstructL(TBool aUseSharedHeap)
	{
	iMidiControllerEventMonitor = CMidiControllerEventMonitor::NewL(*this, iMidiControllerCommands, *iParent);
	iMimeType = HBufC8::NewL(KMimeLength);
		
	RMMFControllerImplInfoArray controllers;
	CleanupResetAndDestroyPushL(controllers);
	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();

	// Select the media IDs to allow
	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);
	User::LeaveIfError(mediaIds.Append(KUidMediaTypeMidi));
	cSelect->SetMediaIdsL(mediaIds,CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds);
	CleanupStack::PopAndDestroy();//mediaIds
	cSelect->ListImplementationsL(controllers);
	// Open and configure a controller
	User::LeaveIfError(DoOpen(controllers, KUidMmfAudioOutput, KNullDesC8, aUseSharedHeap));
	CleanupStack::PopAndDestroy(2);//controllers, cSelect
	iRepeatTrailingSilenceTimer = CRepeatTrailingSilenceTimer::NewL(*this);
	iAddDataSourceSinkAsync = CMMFAddDataSourceSinkAsync::NewL(*this);
	}

void CMidiClientUtility::CBody::MadssaoAddDataSourceSinkAsyncComplete(TInt aError, const TMMFMessageDestination& aHandle)
	{
	if (aError == KErrNone)
		{
		iSourceHandle = aHandle;
		}
	else
		{
 		iMidiControllerEventMonitor->SelfComplete(aError);
		}
	}
void CMidiClientUtility::CBody::OpenFile(const TDesC& aFileName)
	{
	TMMFFileConfig sourceCfg;
	sourceCfg().iPath = aFileName;
	// Add the data source to the controller. MmcuoStateChanged will be call on completition.
	iAddDataSourceSinkAsync->AddDataSource(iController, KUidMmfFileSource, sourceCfg);
	}

void CMidiClientUtility::CBody::OpenFile(RFile& aFile)
	{
		// Add the data source to the controller. MmcuoStateChanged will be call on completition.
	iAddDataSourceSinkAsync->AddFileHandleDataSource(iController, aFile);
	}

void CMidiClientUtility::CBody::OpenFile(const TMMSource& aSource)
	{
	TRAPD(err, DoOpenFileL(aSource));
	if (err != KErrNone)
		{
 		iMidiControllerEventMonitor->SelfComplete(err);
		}
	}
	
	
void CMidiClientUtility::CBody::DoOpenFileL(const TMMSource& aSource)
	{
	delete iSource;
	iSource = NULL;
	iSource = CMMFileSourceSink::NewL(KUidMmfFileSource, aSource);
	static_cast<CMMFileSourceSink*>(iSource)->EvaluateIntentL( aSource.Intent() );
	iAddDataSourceSinkAsync->AddDataSource(iController, 
								iSource->SourceSinkUid(),
								iSource->SourceSinkData());
	}

void CMidiClientUtility::CBody::OpenDes(const TDesC8& aDescriptor)
	{
	TMMFDescriptorConfig sourceCfg;
	sourceCfg().iDes = (TAny*)&aDescriptor;
	sourceCfg().iDesThreadId = RThread().Id();
	// Add the data source to the controller. MmcuoStateChanged will be call on completition.
	iAddDataSourceSinkAsync->AddDataSource(iController, KUidMmfDescriptorSource, sourceCfg);
	}

void CMidiClientUtility::CBody::OpenUrl(const TDesC& aUrl,TInt aIapId,const TDesC8& /*aMimeType*/)
	{
	TRAPD(err, DoOpenUrlL(aUrl, aIapId));

	if (err != KErrNone)
		{
 		iMidiControllerEventMonitor->SelfComplete(err);
		}

	}

void CMidiClientUtility::CBody::DoOpenUrlL(const TDesC& aUrl,TInt aIapId)
	{
	CMMFUrlParams* sourceCfg = CMMFUrlParams::NewLC(aUrl, aIapId);
	CBufFlat* sourceCfgBuffer = sourceCfg->ExternalizeToCBufFlatLC();
	// Add the data source to the controller. MmcuoStateChanged will be call on completition.
	iAddDataSourceSinkAsync->AddDataSource(iController, KUidMmfUrlSource, sourceCfgBuffer->Ptr(0));
	CleanupStack::PopAndDestroy(2, sourceCfg);//sourceCfgBuffer, sourceCfg
	}

void CMidiClientUtility::CBody::Close()
	{
	iMidiControllerCommands.Close();
	}

void CMidiClientUtility::CBody::Play()
	{
	TInt err = iController.Prime();
	if (err==KErrNone)
		{
		err=iController.Play();
		}
	if (err!=KErrNone)
		{
		iMidiControllerEventMonitor->SelfComplete(err);
		}
	}

void CMidiClientUtility::CBody::Stop(const TTimeIntervalMicroSeconds& aFadeOutDuration)
	{
	iMidiControllerCommands.Stop(aFadeOutDuration);
	}
	
/**
 *
 * Returns the current state of the MIDI client utility
 * with regard to MIDI resources.
 *
 * @return "TMidiState" The current state of the utility
 *
 * @since	7.0s
 */

TMidiState CMidiClientUtility::CBody::State() const
	{
	return iState;
	}

void CMidiClientUtility::CBody::PlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity)
	{
	if((aChannel >= KMinChannel && aChannel <= KMaxChannel) 
	&& (aNote >= KMinNote && aNote <= KMaxNote) 
	&& (aNoteOnVelocity >= KMinNoteOnVelocity && aNoteOnVelocity <= KMaxNoteOnVelocity) 
	&& (aNoteOffVelocity >= KMinNoteOffVelocity && aNoteOffVelocity <= KMaxNoteOffVelocity))
		{	
		User::LeaveIfError(iMidiControllerCommands.PlayNote(aChannel, aNote, aDuration, aNoteOnVelocity, aNoteOffVelocity));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CMidiClientUtility::CBody::PlayNoteL(TInt aChannel,TInt aNote, const TTimeIntervalMicroSeconds& aStartTime, const TTimeIntervalMicroSeconds& aDuration, TInt aNoteOnVelocity, TInt aNoteOffVelocity)
	{
	if((aChannel >= KMinChannel && aChannel <= KMaxChannel) 
	&& (aNote >= KMinNote && aNote <= KMaxNote) 
	&& (aNoteOnVelocity >= KMinNoteOnVelocity && aNoteOnVelocity <= KMaxNoteOnVelocity) 
	&& (aNoteOffVelocity >= KMinNoteOffVelocity && aNoteOffVelocity <= KMaxNoteOffVelocity))
		{
		User::LeaveIfError(iMidiControllerCommands.PlayNote(aChannel, aNote, aStartTime, aDuration, aNoteOnVelocity, aNoteOffVelocity));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CMidiClientUtility::CBody::StopNotes(TInt aChannel)
	{
	if(aChannel >= KMinChannel && aChannel <= KMaxChannel) 
		{
		iMidiControllerCommands.StopNotes(aChannel);
		}
	}

void CMidiClientUtility::CBody::NoteOnL(TInt aChannel,TInt aNote,TInt aVelocity)
	{
	if((aChannel >= KMinChannel && aChannel <= KMaxChannel) 
	&& (aNote >= KMinNote && aNote <= KMaxNote) 
	&& (aVelocity >= KMinNoteOnVelocity && aVelocity <= KMaxNoteOnVelocity))
		{
		User::LeaveIfError(iMidiControllerCommands.NoteOn(aChannel, aNote, aVelocity));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CMidiClientUtility::CBody::NoteOffL(TInt aChannel,TInt aNote,TInt aVelocity)
	{
	if((aChannel >= KMinChannel && aChannel <= KMaxChannel) 
	&& (aNote >= KMinNote && aNote <= KMaxNote) 
	&& (aVelocity >= KMinNoteOffVelocity && aVelocity <= KMaxNoteOffVelocity))
		{
		User::LeaveIfError(iMidiControllerCommands.NoteOff(aChannel, aNote, aVelocity));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

TInt CMidiClientUtility::CBody::PlaybackRateL() const
	{
	TInt rate;
	User::LeaveIfError(iMidiControllerCommands.PlaybackRate(rate));
	return rate;
	}

void CMidiClientUtility::CBody::SetPlaybackRateL(TInt aRate)
	{
	User::LeaveIfError(iMidiControllerCommands.SetPlaybackRate(aRate));
	}

TInt CMidiClientUtility::CBody::MaxPlaybackRateL() const
	{
	TInt maxRate;
	User::LeaveIfError(iMidiControllerCommands.MaxPlaybackRate(maxRate));
	return maxRate;
	}

TInt CMidiClientUtility::CBody::MinPlaybackRateL() const
	{
	TInt minRate;
	User::LeaveIfError(iMidiControllerCommands.MinPlaybackRate(minRate));
	return minRate;
	}


TInt CMidiClientUtility::CBody::TempoMicroBeatsPerMinuteL() const
	{
	TInt microBeatsPerMinute;
	User::LeaveIfError(iMidiControllerCommands.TempoMicroBeatsPerMinute(microBeatsPerMinute));
	return microBeatsPerMinute;
	}

void CMidiClientUtility::CBody::SetTempoL(TInt aMicroBeatsPerMinute)
	{
	if(aMicroBeatsPerMinute > 0)
		{
		User::LeaveIfError(iMidiControllerCommands.SetTempo(aMicroBeatsPerMinute));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

TInt CMidiClientUtility::CBody::PitchTranspositionCentsL() const
	{
	TInt cents;
	User::LeaveIfError(iMidiControllerCommands.PitchTranspositionCents(cents));
	return cents;
	}

TInt CMidiClientUtility::CBody::SetPitchTranspositionL(TInt aCents)
	{
	TInt pitchApplied = 0;
	//we do not check aCents value - it is expected the controller will report KErrArgument
	//if the pitch level is not supported.
	User::LeaveIfError(iMidiControllerCommands.SetPitchTransposition(aCents, pitchApplied));

	return pitchApplied;
	}

TTimeIntervalMicroSeconds CMidiClientUtility::CBody::DurationMicroSecondsL() const
	{
	TTimeIntervalMicroSeconds duration;
	User::LeaveIfError(iController.GetDuration(duration));
	return duration;
	}

TInt64 CMidiClientUtility::CBody::DurationMicroBeatsL() const
	{
	TInt64 duration;
	User::LeaveIfError(iMidiControllerCommands.DurationMicroBeats(duration));
	return duration;
	}

TInt CMidiClientUtility::CBody::NumTracksL() const
	{
	TInt tracks;
	User::LeaveIfError(iMidiControllerCommands.NumTracks(tracks));
	return tracks;
	}

void CMidiClientUtility::CBody::SetTrackMuteL(TInt aTrack, TBool aMuted) const
	{
	TInt numTracks = NumTracksL();
	if((aTrack >= 0) && (aTrack < numTracks))
		{
		User::LeaveIfError(iMidiControllerCommands.SetTrackMute(aTrack, aMuted));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

const TDesC8& CMidiClientUtility::CBody::MimeTypeL()
	{
	TPtr8 des = iMimeType->Des();
	User::LeaveIfError(iMidiControllerCommands.MimeType(des));
	return *iMimeType;
	}

TTimeIntervalMicroSeconds CMidiClientUtility::CBody::PositionMicroSecondsL() const
	{
	TTimeIntervalMicroSeconds position;
	User::LeaveIfError(iController.GetPosition(position));
	return position;
	}

void CMidiClientUtility::CBody::SetPositionMicroSecondsL(const TTimeIntervalMicroSeconds& aPosition)
	{
	TTimeIntervalMicroSeconds maxPosition = DurationMicroSecondsL();
	TTimeIntervalMicroSeconds minPosition(0);

	TTimeIntervalMicroSeconds position = aPosition;
	if (aPosition > maxPosition)
		{
		position = maxPosition;
		}
	if (aPosition < minPosition)
		{
		position = minPosition;
		}
	User::LeaveIfError(iController.SetPosition(position));
	}

TInt64 CMidiClientUtility::CBody::PositionMicroBeatsL() const
	{
	TInt64 position;
	User::LeaveIfError(iMidiControllerCommands.PositionMicroBeats(position));
	return position;
	}

void CMidiClientUtility::CBody::SetPositionMicroBeatsL(TInt64 aMicroBeats)
	{
	TInt64 maxPosition = DurationMicroBeatsL();
	TInt64 minPosition(0);

	TInt64 position = aMicroBeats;
	if (aMicroBeats > maxPosition)
		{
		position = maxPosition;
		}
	if (aMicroBeats < minPosition)
		{
		position = minPosition;
		}
	User::LeaveIfError(iMidiControllerCommands.SetPositionMicroBeats(position));
	}

void CMidiClientUtility::CBody::SetSyncUpdateCallbackIntervalL(const TTimeIntervalMicroSeconds& aMicroSeconds, TInt64 aMicroBeats)
	{
	
	if((aMicroSeconds > TTimeIntervalMicroSeconds(0)) || (aMicroSeconds == TTimeIntervalMicroSeconds(0) && aMicroBeats == 0))
		{
		iIntervalSec = ETrue;
		}
	else 
		{
		if (aMicroBeats > 0)
			{
			iIntervalSec = EFalse;
			}
		else
			{
			User::Leave(KErrArgument);
			}
		}
	
	User::LeaveIfError(iMidiControllerCommands.SetSyncUpdateCallbackInterval(aMicroSeconds, aMicroBeats));
	}

TInt CMidiClientUtility::CBody::SendMessageL(const TDesC8& aMidiMessage)
	{
	TInt numByteProc;
	User::LeaveIfError(iMidiControllerCommands.SendMessage(aMidiMessage, numByteProc));
	return numByteProc;
	}

TInt CMidiClientUtility::CBody::SendMessageL(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& aTime)
	{
	TInt numByteProc;
	User::LeaveIfError(iMidiControllerCommands.SendMessage(aMidiMessage, aTime, numByteProc));
	return numByteProc;
	}

void CMidiClientUtility::CBody::SendMipMessageL(const RArray<TMipMessageEntry>& aEntry)
	{
	User::LeaveIfError(iMidiControllerCommands.SendMipMessage(aEntry));
	}

TInt CMidiClientUtility::CBody::NumberOfBanksL(TBool aCustom) const
	{
	TInt numBanks;
	User::LeaveIfError(iMidiControllerCommands.NumberOfBanks(aCustom, numBanks));
	return numBanks;
	}

TInt CMidiClientUtility::CBody::GetBankIdL(TBool aCustom,TInt aBankIndex) const
	{
	TInt numBanks = NumberOfBanksL(aCustom);
	TInt bankId = 0;
	if(aBankIndex >= 0 && aBankIndex < numBanks)
		{
		User::LeaveIfError(iMidiControllerCommands.GetBankId(aCustom, aBankIndex, bankId));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	return bankId;
	}

void CMidiClientUtility::CBody::LoadCustomBankL(const TDesC& aFileName,TInt& aBankCollectionIndex)
	{
	User::LeaveIfError(iMidiControllerCommands.LoadCustomBank(aFileName, aBankCollectionIndex));
	}

void CMidiClientUtility::CBody::UnloadCustomBankL(TInt aBankCollectionIndex)
	{
	User::LeaveIfError(iMidiControllerCommands.UnloadCustomBank(aBankCollectionIndex));
	}

TBool CMidiClientUtility::CBody::CustomBankLoadedL(TInt aBankCollectionIndex) const
	{
	TBool bankLoaded;
	User::LeaveIfError(iMidiControllerCommands.CustomBankLoaded(aBankCollectionIndex, bankLoaded));
	return bankLoaded;
	}

void CMidiClientUtility::CBody::UnloadAllCustomBanksL()
	{
	User::LeaveIfError(iMidiControllerCommands.UnloadAllCustomBanks());
	}

TInt CMidiClientUtility::CBody::NumberOfInstrumentsL(TInt aBankId,TBool aCustom) const
	{
	TInt numInstruments;
	User::LeaveIfError(iMidiControllerCommands.NumberOfInstruments(aBankId, aCustom, numInstruments));
	return numInstruments;
	}

TInt CMidiClientUtility::CBody::GetInstrumentIdL(TInt aBankId,TBool aCustom,TInt aInstrumentIndex) const
	{
	TInt numInstruments = NumberOfInstrumentsL(aBankId, aCustom);
	TInt instrumentId = 0;
	if(aInstrumentIndex >=0 && aInstrumentIndex < numInstruments)
		{
		User::LeaveIfError(iMidiControllerCommands.GetInstrumentId(aBankId, aCustom, aInstrumentIndex, instrumentId));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	return instrumentId;
	}

HBufC* CMidiClientUtility::CBody::InstrumentNameL(TInt aBankId, TBool aCustom, TInt aInstrumentId) const
	{
	HBufC* instrumentName = NULL;

	if(aInstrumentId >= KMinInstrumentId && aInstrumentId <= KMaxInstrumentId)
		{
		instrumentName = iMidiControllerCommands.InstrumentNameL(aBankId, aCustom, aInstrumentId);
		}
	else
		{
		User::Leave(KErrArgument);
		}

	return instrumentName;
	}

void CMidiClientUtility::CBody::SetInstrumentL(TInt aChannel,TInt aBankId,TInt aInstrumentId)
	{
	if((aChannel >= KMinChannel && aChannel <= KMaxChannel)  
	&& (aInstrumentId >= KMinInstrumentId && aInstrumentId <= KMaxInstrumentId))
		{
		User::LeaveIfError(iMidiControllerCommands.SetInstrument(aChannel, aBankId, aInstrumentId));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CMidiClientUtility::CBody::LoadCustomInstrumentL(const TDesC& aFileName, TInt aFileBankId, TInt aFileInstrumentId, TInt aMemoryBankId, TInt aMemoryInstrumentId)
	{
	if((aFileInstrumentId >= KMinInstrumentId && aFileInstrumentId <= KMaxInstrumentId) 
	&& (aMemoryInstrumentId >= KMinInstrumentId && aMemoryInstrumentId <= KMaxInstrumentId))
		{
		User::LeaveIfError(iMidiControllerCommands.LoadCustomInstrument(aFileName, aFileBankId, aFileInstrumentId, aMemoryBankId, aMemoryInstrumentId));
		}	
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CMidiClientUtility::CBody::UnloadCustomInstrumentL(TInt aCustomBankId,TInt aInstrumentId)
	{
	if(aInstrumentId >= KMinInstrumentId && aInstrumentId <= KMaxInstrumentId)
		{
		User::LeaveIfError(iMidiControllerCommands.UnloadCustomInstrument(aCustomBankId, aInstrumentId));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

HBufC* CMidiClientUtility::CBody::PercussionKeyNameL(TInt aNote, TInt aBankId, TBool aCustom, TInt aInstrumentId) const
	{
	HBufC* pKeyName = NULL;

	if((aNote >= KMinNote && aNote <= KMaxNote) 
	&& (aInstrumentId >= KMinInstrumentId && aInstrumentId <= KMaxInstrumentId))
		{
		pKeyName = iMidiControllerCommands.PercussionKeyNameL(aNote, aBankId, aCustom, aInstrumentId);
		}
	else
		{
		User::Leave(KErrArgument);
		}

	return pKeyName;
	}

void CMidiClientUtility::CBody::StopTimeL(TTimeIntervalMicroSeconds& aStopTime) const
	{
	User::LeaveIfError(iMidiControllerCommands.StopTime(aStopTime));
	}

void CMidiClientUtility::CBody::SetStopTimeL(const TTimeIntervalMicroSeconds& aStopTime)
	{
	TTimeIntervalMicroSeconds duration = DurationMicroSecondsL();
	if(aStopTime >= TTimeIntervalMicroSeconds(0) && aStopTime <= duration)
		{
		User::LeaveIfError(iMidiControllerCommands.SetStopTime(aStopTime));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CMidiClientUtility::CBody::SetRepeatsL(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	if((aRepeatNumberOfTimes >= 0) && (aTrailingSilence >= TTimeIntervalMicroSeconds(0)))
		{
		User::LeaveIfError(iMidiControllerCommands.SetRepeats(aRepeatNumberOfTimes, aTrailingSilence));
		}
	else
		{
		User::Leave(KErrArgument);
		}	
	}

TInt CMidiClientUtility::CBody::PolyphonyL() const
	{
	TInt numNotes;
	TInt maxPoly = MaxPolyphonyL();
	User::LeaveIfError(iMidiControllerCommands.Polyphony(numNotes));
	if(maxPoly <= numNotes)
		{
		return maxPoly;
		}
	else
		{
		return numNotes;	
		}
	}

TInt CMidiClientUtility::CBody::MaxPolyphonyL() const
	{
	TInt maxNotes;
	User::LeaveIfError(iMidiControllerCommands.MaxPolyphony(maxNotes));
	return maxNotes;
	}

TInt CMidiClientUtility::CBody::ChannelsSupportedL() const
	{
	TInt channels;
	User::LeaveIfError(iMidiControllerCommands.ChannelsSupported(channels));
	return channels;
	}

TReal32 CMidiClientUtility::CBody::ChannelVolumeL(TInt aChannel) const
	{
	TReal32 channelVol;
	if(aChannel >= KMinChannel && aChannel <= KMaxChannel) 
		{
		User::LeaveIfError(iMidiControllerCommands.ChannelVolume(aChannel, channelVol));
		}
	else
		{
		User::Leave(KErrArgument);	
		}
	return channelVol;
	}

TReal32 CMidiClientUtility::CBody::MaxChannelVolumeL() const
	{
	TReal32 maxChanVol;
	User::LeaveIfError(iMidiControllerCommands.MaxChannelVolume(maxChanVol));
	return maxChanVol;
	}

void CMidiClientUtility::CBody::SetChannelVolumeL(TInt aChannel,TReal32 aVolume)
	{
	TReal32 maxChanVol = MaxChannelVolumeL();
	if((aChannel >= KMinChannel && aChannel <= KMaxChannel) && aVolume <= maxChanVol)
		{
		User::LeaveIfError(iMidiControllerCommands.SetChannelVolume(aChannel, aVolume));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CMidiClientUtility::CBody::SetChannelMuteL(TInt aChannel,TBool aMuted)
	{
	if(aChannel >= KMinChannel && aChannel <= KMaxChannel)
		{
		User::LeaveIfError(iMidiControllerCommands.SetChannelMute(aChannel, aMuted));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

TInt CMidiClientUtility::CBody::VolumeL() const
	{
	TInt vol;
	User::LeaveIfError(iMidiControllerCommands.Volume(vol));
	return vol;
	}

TInt CMidiClientUtility::CBody::MaxVolumeL() const
	{
	TInt maxVol;
	User::LeaveIfError(iMidiControllerCommands.MaxVolume(maxVol));
	return maxVol;
	}

void CMidiClientUtility::CBody::SetVolumeL(TInt aVolume)
	{
	User::LeaveIfError(iMidiControllerCommands.SetVolume(aVolume));
	}

void CMidiClientUtility::CBody::SetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	User::LeaveIfError(iMidiControllerCommands.SetVolumeRamp(aRampDuration));
	}


TInt CMidiClientUtility::CBody::GetBalanceL() const
	{
	TInt balance;
	User::LeaveIfError(iMidiControllerCommands.GetBalance(balance));
	return balance;
	}

void CMidiClientUtility::CBody::SetBalanceL(TInt aBalance)
	{
	User::LeaveIfError(iMidiControllerCommands.SetBalance(aBalance));
	}

void CMidiClientUtility::CBody::SetPriorityL(TInt aPriority, TInt aPref)
	{
	TMMFPrioritySettings priority;
	priority.iPriority = aPriority;
	priority.iPref = aPref;

	User::LeaveIfError(iController.SetPrioritySettings(priority));
	}

TInt CMidiClientUtility::CBody::NumberOfMetaDataEntriesL() const
	{
	TInt numMetaData;
	User::LeaveIfError(iController.GetNumberOfMetaDataEntries(numMetaData));
	return numMetaData;
	}

CMMFMetaDataEntry* CMidiClientUtility::CBody::GetMetaDataEntryL(TInt aMetaDataIndex) const
	{
	CMMFMetaDataEntry* metaDataEntry = iController.GetMetaDataEntryL(aMetaDataIndex);
	return metaDataEntry;
	}

void CMidiClientUtility::CBody::CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	User::LeaveIfError(iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom));
	}

void CMidiClientUtility::CBody::CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	User::LeaveIfError(iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2));
	}

void CMidiClientUtility::CBody::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}

void CMidiClientUtility::CBody::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

MMMFDRMCustomCommand* CMidiClientUtility::CBody::GetDRMCustomCommand()
	{
	if (iDRMCustomCommands.IsSupported())
		{
		return static_cast<MMMFDRMCustomCommand*>(&iDRMCustomCommands);
		}
	return NULL;
	}
	
TInt CMidiClientUtility::CBody::DoOpen(const RMMFControllerImplInfoArray& aControllers, TUid aSinkUid, const TDesC8& aSinkData, TBool aUseSharedHeap)
	{
	// Make sure any existing controller is closed.
	iMidiControllerEventMonitor->Cancel();
	iController.Close();

	// Try opening and configuring each controller in turn
	TInt error = KErrNotSupported;
	TInt index = 0;
	while (error)
		{
		// Break if we're at the end of the array of controllers
		if (index >= aControllers.Count())
			break;

		// Open the controller
		error = iController.Open(aControllers[index]->Uid(), iPrioritySettings, aUseSharedHeap);

		// If the controller was opened without error, start receiving events from it.
		if (error==KErrNone)
			{
			iMidiControllerEventMonitor->Start();
			}

		// Add the data sink
		if (error==KErrNone)
			error = iController.AddDataSink(aSinkUid, aSinkData, iSinkHandle);

		// If an error occurred in any of the above, close the controller.
		if (error!=KErrNone)
			{
			iMidiControllerEventMonitor->Cancel();
			iController.Close();
			}

		// increment index
		index++;
		}

	return error;
	}

   void CMidiClientUtility::CBody::HandleMidiEvent(const CMMFMidiEvent& aEvent)
   	{
   	if(aEvent.iEventType == KMMFEventCategoryMidiOpenDataSourceComplete ||
   	   aEvent.iEventType == KMMFEventCategoryMidiClose ||
   	   aEvent.iEventType == KMMFEventCategoryMidiPrime ||
   	   aEvent.iEventType == KMMFEventCategoryMidiPlaying ||
   	   aEvent.iEventType == KMMFEventCategoryMidiPlaybackIncompatible ||
   	   aEvent.iEventType == KMMFEventCategoryMidiPlaybackSilent)
   		{
   		iState = aEvent.iNewState;
   		iObserver.MmcuoStateChanged(aEvent.iOldState, aEvent.iNewState, aEvent.iMicroSeconds, aEvent.iErrorCode);
   		
   		if (aEvent.iEventType == KMMFEventCategoryMidiClose)
   			{
			if (iSourceHandle.DestinationHandle())
				{
				iController.RemoveDataSource(iSourceHandle);
				}
			}	
 		}
   	else if(aEvent.iEventType == KMMFEventCategoryMidiPlayingComplete)
   		{
 		iState = aEvent.iNewState;
   		iObserver.MmcuoStateChanged(aEvent.iOldState, aEvent.iNewState, aEvent.iMicroSeconds, aEvent.iErrorCode);
   		}
   	else if(aEvent.iEventType == KMMFEventCategoryMidiSyncUpdate)
   		{
   		iObserver.MmcuoSyncUpdate(aEvent.iMicroSeconds, aEvent.iMicroBeats);
   		}
   	else if(aEvent.iEventType == KMMFEventCategoryTempoChanged)
   		{
   		iObserver.MmcuoTempoChanged(aEvent.iTempoMicroBeats);
   		}
   	else if(aEvent.iEventType == KMMFEventCategoryVolumeChanged)
   		{
   		iObserver.MmcuoVolumeChanged(aEvent.iChannel, aEvent.iVolumeInDecibels);
   		}
   	else if(aEvent.iEventType == KMMFEventCategoryMuteChanged)
   		{
   		iObserver.MmcuoMuteChanged(aEvent.iChannel, aEvent.iMute);
   		}
   	else if(aEvent.iEventType == KMMFEventCategoryMetaDataEntryFound)
   		{
   		iObserver.MmcuoMetaDataEntryFound(aEvent.iMetaDataEntryId, aEvent.iMicroSeconds);
   		}
   	else if(aEvent.iEventType == KMMFEventCategoryMipMessageReceived)
   		{
   		iObserver.MmcuoMipMessageReceived(aEvent.iMipMessage);
   		}
   	else if(aEvent.iEventType == KMMFEventCategoryPolyphonyChanged)
   		{
   		iObserver.MmcuoPolyphonyChanged(aEvent.iPolyphony);
   		}
   	else if(aEvent.iEventType == KMMFEventCategoryInstrumentChanged)
   		{
   		iObserver.MmcuoInstrumentChanged(aEvent.iChannel,aEvent.iBankId,aEvent.iInstrumentId);
   		}
   	else if((iState == EMidiStateOpenPlaying) || (iState == EMidiStatePlaybackIncompatible) || 
   			(iState == EMidiStatePlaybackSilent) || (iState == EMidiStateClosedEngaged) ||
   			(iState == EMidiStateOpenEngaged))
   		{
 		iState = aEvent.iNewState;
   		iObserver.MmcuoStateChanged(aEvent.iOldState, aEvent.iNewState, aEvent.iMicroSeconds, aEvent.iErrorCode);

   		}
   	else if (aEvent.iEventType == KMMFErrorCategoryControllerGeneralError)
   		{
   		iObserver.MmcuoStateChanged(iState, iState, TTimeIntervalMicroSeconds(0), aEvent.iErrorCode);
   		}
   	else
   		{
   		// xxx - what do we do when we don't understand the error type?
   		}
   	}

/**
 *
 * Used to change the value of MaxPolyphonyL()
 */ 	
void CMidiClientUtility::CBody::SetMaxPolyphonyL(TInt aMaxNotes)
	{
	User::LeaveIfError(iMidiControllerCommands.SetMaxPolyphony(aMaxNotes));
	}

TInt CMidiClientUtility::CBody::GetRepeats()
	{
	TInt numRepeats = 0;
	iMidiControllerCommands.GetRepeats(numRepeats);
	return numRepeats;
	}

void CMidiClientUtility::CBody::LoadCustomBankDataL(const TDesC8& aBankData,TInt& aBankId)
	{
	User::LeaveIfError(iMidiControllerCommands.LoadCustomBankData(aBankData, aBankId));
	}

void CMidiClientUtility::CBody::LoadCustomInstrumentDataL(const TDesC8& aInstrumentData, TInt aBankDataId, TInt aInstrumentDataId, TInt aMemoryBankId, TInt aMemoryInstrumentId)
	{
	if((aInstrumentDataId >= KMinInstrumentId && aInstrumentDataId <= KMaxInstrumentId) 
	&& (aMemoryInstrumentId >= KMinInstrumentId && aMemoryInstrumentId <= KMaxInstrumentId))
		{
		User::LeaveIfError(iMidiControllerCommands.LoadCustomInstrumentData(aInstrumentData, aBankDataId, aInstrumentDataId, aMemoryBankId, aMemoryInstrumentId));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CMidiClientUtility::CBody::SetBankL(TBool aCustom)
	{
	User::LeaveIfError(iMidiControllerCommands.SetBank(aCustom));
	}

TBool CMidiClientUtility::CBody::IsTrackMuteL(TInt aTrack) const
	{
	TBool mute;
	User::LeaveIfError(iMidiControllerCommands.IsTrackMute(aTrack, mute));
	return mute;
	}

TBool CMidiClientUtility::CBody::IsChannelMuteL(TInt aChannel) const
	{
	TBool mute;
	if (aChannel >= KMinChannel && aChannel <= KMaxChannel)
		{
		User::LeaveIfError(iMidiControllerCommands.IsChannelMute(aChannel, mute));	
		}
	else
		{
		User::Leave(KErrArgument);	
		}
		
	return mute;
	}

void CMidiClientUtility::CBody::GetInstrumentL(TInt aChannel, TInt& aInstrumentId, TInt& aBankId)
	{
	if (aChannel >= KMinChannel && aChannel <= KMaxChannel)
		{
		User::LeaveIfError(iMidiControllerCommands.GetInstrument(aChannel, aInstrumentId, aBankId));	
		}
	else
		{
		User::Leave(KErrArgument);		
		}
	}

void CMidiClientUtility::CBody::RepeatTrailingSilenceTimerComplete()
	{
	Play();
	}

CRepeatTrailingSilenceTimer* CRepeatTrailingSilenceTimer::NewL(MRepeatTrailingSilenceTimerObs& aObs)
	{
	CRepeatTrailingSilenceTimer* s = new(ELeave) CRepeatTrailingSilenceTimer(aObs);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

void CRepeatTrailingSilenceTimer::RunL()
	{
	iObs.RepeatTrailingSilenceTimerComplete();
	}

CRepeatTrailingSilenceTimer::CRepeatTrailingSilenceTimer(MRepeatTrailingSilenceTimerObs& aObs) :
	CTimer(EPriorityHigh),
	iObs(aObs)
	{
	CActiveScheduler::Add(this);
	}

//
//
//
//

CMidiControllerEventMonitor* CMidiControllerEventMonitor::NewL(MMidiControllerEventMonitorObserver& aMidiObserver, 
												RMidiControllerCustomCommands& aMidiControllerCustomCommands, const CMidiClientUtility& aParent)
	{
	CMidiControllerEventMonitor* self = new(ELeave) CMidiControllerEventMonitor(aMidiObserver, aMidiControllerCustomCommands, aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CMidiControllerEventMonitor::ConstructL()
	{
	iMidiEvent = new (ELeave) CMMFMidiEvent();
	}

CMidiControllerEventMonitor::CMidiControllerEventMonitor(MMidiControllerEventMonitorObserver& aMidiObserver, 
													   RMidiControllerCustomCommands& aMidiControllerCustomCommands, const CMidiClientUtility& aParent) :
	CActive(EPriorityStandard),
	iMidiObserver(aMidiObserver), 
	iMidiControllerCustomCommands(aMidiControllerCustomCommands),
	iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

CMidiControllerEventMonitor::~CMidiControllerEventMonitor()
	{
	Cancel();
	delete iMidiEvent;
	}

/**
Start receiving events from the controller.

This can only be called once the controller is open.
*/
void CMidiControllerEventMonitor::Start()
	{
	iMidiControllerCustomCommands.ReceiveEvents(iSizeOfEvent, iStatus);
	SetActive();
	}

void CMidiControllerEventMonitor::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	// Create a buffer big enough to hold the event, then retrieve it from the server
	HBufC8* buf = HBufC8::NewLC(iSizeOfEvent());
	TPtr8 bufPtr = buf->Des();
	User::LeaveIfError(iMidiControllerCustomCommands.RetrieveEvent(bufPtr));

	// Now internalize a CMMFMidiEvent with the info in the buffer
	RDesReadStream stream(bufPtr);
	CleanupClosePushL(stream);

	CMMFMidiEvent* theEvent = new (ELeave) CMMFMidiEvent();
	
	CleanupStack::PushL(theEvent);
	theEvent->InternalizeL(stream);
	
	iMidiObserver.HandleMidiEvent(*theEvent);
	Start();

	CleanupStack::PopAndDestroy(3);//buf, stream, theEvent
	}

void CMidiControllerEventMonitor::SelfComplete(TInt aError)
	{
	Cancel();
	TRequestStatus *status = &iStatus;
	if(!IsActive())
		SetActive();
	User::RequestComplete(status, aError);
	}

void CMidiControllerEventMonitor::DoCancel()
	{
	iMidiControllerCustomCommands.CancelReceiveEvents();
	}

TInt CMidiControllerEventMonitor::RunError(TInt aError)
	{
	iMidiEvent->iEventType = KMMFErrorCategoryControllerGeneralError;
	iMidiEvent->iErrorCode = aError;
	iMidiEvent->iOldState = iParent.State();
	iMidiEvent->iNewState = iMidiEvent->iOldState;

	iMidiObserver.HandleMidiEvent(*iMidiEvent);
	Start();
	return KErrNone;
	}
