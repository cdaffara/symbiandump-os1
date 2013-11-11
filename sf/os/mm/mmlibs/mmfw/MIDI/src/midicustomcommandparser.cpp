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
#include <mmf/common/midieventreceiver.h>
#include <mmf/common/mmfmidiconfig.h>
#endif
const TInt KMimeTypeLength = 256;


/**
Creates a new MIDI custom command parser capable of handling MIDI controller commands.

@param	aImplementor A reference to the controller plugin that owns this new object.
@leave	This function may leave with one of the system-wide error codes.
*/
EXPORT_C CMidiCustomCommandParser* CMidiCustomCommandParser::NewL(MMidiCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMidiCustomCommandParser(aImplementor);
	}

CMidiCustomCommandParser::CMidiCustomCommandParser(MMidiCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMidi),
	iImplementor(aImplementor)
	{
	}

/**
Destructor.
*/
EXPORT_C CMidiCustomCommandParser::~CMidiCustomCommandParser()
	{
	delete iMidiEventReceiver;
	delete iInstrumentName;
	delete iPercussionKeyName;
	iMidiEvents.ResetAndDestroy();
	iMidiEvents.Close();
	}

/**
Handles a request from the client. Called by the controller framework.

@param aMessage The message to be handled.
*/
void CMidiCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMidi)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMidiCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFMidiControllerSetPositionMicroBeats:
		complete = DoSetPositionMicroBeatsL(aMessage);
		break;
	case EMMFMidiControllerPositionMicroBeats:
		complete = DoPositionMicroBeatsL(aMessage);
		break;
	case EMMFMidiControllerPlayNote:
		complete = DoPlayNoteL(aMessage);
		break;
	case EMMFMidiControllerPlayNoteWithStartTime:
		complete = DoPlayNoteWithStartTimeL(aMessage);
		break;
	case EMMFMidiControllerStopNotes:
		complete = DoStopNotesL(aMessage);
		break;
	case EMMFMidiControllerNoteOn:
		complete = DoNoteOnL(aMessage);
		break;
	case EMMFMidiControllerNoteOff:
		complete = DoNoteOffL(aMessage);
		break;
	case EMMFMidiControllerPlaybackRate:
		complete = DoPlaybackRateL(aMessage);
		break;
	case EMMFMidiControllerSetPlaybackRate:
		complete = DoSetPlaybackRateL(aMessage);
		break;
	case EMMFMidiControllerMaxPlaybackRate:
		complete = DoMaxPlaybackRateL(aMessage);
		break;
	case EMMFMidiControllerMinPlaybackRate:
		complete = DoMinPlaybackRateL(aMessage);
		break;
	case EMMFMidiControllerTempo:
		complete = DoTempoMicroBeatsPerMinuteL(aMessage);
		break;
	case EMMFMidiControllerSetTempo:
		complete = DoSetTempoL(aMessage);
		break;
	case EMMFMidiControllerPitch:
		complete = DoPitchTranspositionCentsL(aMessage);
		break;
	case EMMFMidiControllerSetPitch:
		complete = DoSetPitchTranspositionL(aMessage);
		break;
	case EMMFMidiControllerDurationMicroBeats:
		complete = DoDurationMicroBeatsL(aMessage);
		break;
	case EMMFMidiControllerNumTracks:
		complete = DoNumTracksL(aMessage);
		break;
	case EMMFMidiControllerSetTrackMute:
		complete = DoSetTrackMuteL(aMessage);
		break;
	case EMMFMidiControllerMimeType:
		complete = DoMimeTypeL(aMessage);
		break;
	case EMMFMidiControllerSetSyncUpdateCallbackInterval:
		complete = DoSetSyncUpdateCallbackIntervalL(aMessage);
		break;
	case EMMFMidiControllerSendMessage:
		complete = DoSendMessageL(aMessage);
		break;
	case EMMFMidiControllerSendMessageWithTimeStamp:
		complete = DoSendMessageWithTimeStampL(aMessage);
		break;
	case EMMFMidiControllerSendMipMessage:
		complete = DoSendMipMessageL(aMessage);
		break;
	case EMMFMidiControllerNumberOfBanks:
		complete = DoNumberOfBanksL(aMessage);
		break;
	case EMMFMidiControllerGetBankId:
		complete = DoGetBankIdL(aMessage);
		break;
	case EMMFMidiControllerLoadCustomBank:
		complete = DoLoadCustomBankL(aMessage);
		break;
	case EMMFMidiControllerLoadCustomBankData:
		complete = DoLoadCustomBankDataL(aMessage);
		break;
	case EMMFMidiControllerUnloadCustomBank:
		complete = DoUnloadCustomBankL(aMessage);
		break;
	case EMMFMidiControllerCustomBankLoaded:
		complete = DoCustomBankLoadedL(aMessage);
		break;
	case EMMFMidiControllerUnloadAllCustomBanks:
		complete = DoUnloadAllCustomBanksL(aMessage);
		break;
	case EMMFMidiControllerNumberOfInstruments:
		complete = DoNumberOfInstrumentsL(aMessage);
		break;
	case EMMFMidiControllerGetInstrumentId:
		complete = DoGetInstrumentIdL(aMessage);
		break;
	case EMMFMidiControllerInstrumentName:
		complete = DoInstrumentNameL(aMessage);
		break;
	case EMMFMidiControllerCopyInstrumentName:
		complete = DoCopyInstrumentNameL(aMessage);
		break;
	case EMMFMidiControllerSetInstrument:
		complete = DoSetInstrumentL(aMessage);
		break;
	case EMMFMidiControllerLoadCustomInstrument:
		complete = DoLoadCustomInstrumentL(aMessage);
		break;
	case EMMFMidiControllerLoadCustomInstrumentData:
		complete = DoLoadCustomInstrumentDataL(aMessage);
		break;
	case EMMFMidiControllerUnloadCustomInstrument:
		complete = DoUnloadCustomInstrumentL(aMessage);
		break;
	case EMMFMidiControllerPercussionKeyName:
		complete = DoPercussionKeyNameL(aMessage);
		break;
	case EMMFMidiControllerCopyPercussionKeyName:
		complete = DoCopyPercussionKeyNameL(aMessage);
		break;
	case EMMFMidiControllerStopTime:
		complete = DoStopTimeL(aMessage);
		break;
	case EMMFMidiControllerSetStopTime:
		complete = DoSetStopTimeL(aMessage);
		break;
	case EMMFMidiControllerPolyphony:
		complete = DoPolyphonyL(aMessage);
		break;
	case EMMFMidiControllerChannelsSupported:
		complete = DoChannelsSupportedL(aMessage);
		break;
	case EMMFMidiControllerChannelVolume:
		complete = DoChannelVolumeL(aMessage);
		break;
	case EMMFMidiControllerMaxChannelVolume:
		complete = DoMaxChannelVolumeL(aMessage);
		break;
	case EMMFMidiControllerSetChannelVolume:
		complete = DoSetChannelVolumeL(aMessage);
		break;
	case EMMFMidiControllerSetChannelMute:
		complete = DoSetChannelMuteL(aMessage);
		break;
	case EMMFMidiControllerVolume:
		complete = DoVolumeL(aMessage);
		break;
	case EMMFMidiControllerMaxVolume:
		complete = DoMaxVolumeL(aMessage);
		break;
	case EMMFMidiControllerSetVolume:
		complete = DoSetVolumeL(aMessage);
		break;
	case EMMFMidiControllerSetVolumeRamp:
		complete = DoSetVolumeRampL(aMessage);
		break;
	case EMMFMidiControllerGetBalance:
		complete = DoGetBalanceL(aMessage);
		break;
	case EMMFMidiControllerSetBalance:
		complete = DoSetBalanceL(aMessage);
		break;
	case EMMFMidiControllerSetMaxPolyphony:
		complete = DoSetMaxPolyphonyL(aMessage);
		break;
	case EMMFMidiControllerGetRepeats:
		complete = DoGetRepeatsL(aMessage);
		break;
	case EMMFMidiControllerSetRepeats:
		complete = DoSetRepeatsL(aMessage);
		break;
	case EMMFMidiControllerSetBank:
		DoSetBankL(aMessage);
		break;
	case EMMFMidiControllerIsTrackMute:
		DoIsTrackMuteL(aMessage);
		break;
	case EMMFMidiControllerIsChannelMute:
		DoIsChannelMuteL(aMessage);
		break;
	case EMMFMidiControllerGetInstrument:
		DoGetInstrumentL(aMessage);
		break;
	case EMMFMidiControllerClose:
		complete = DoCloseL(aMessage);
		break;
	case EMMFMidiControllerStop:
		complete = DoStopL(aMessage);
		break;
	case EMMFMidiControllerReceiveEvents:
		complete = DoReceiveEventsL(aMessage);
		break;
	case EMMFMidiControllerRetrieveEvent:
		complete = DoRetrieveEventL(aMessage);
		break;
	case EMMFMidiControllerCancelReceiveEvents:
		complete = DoCancelReceiveEventsL(aMessage);
		break;
	case EMMFMidiControllerMaxPolyphony:
		complete = DoMaxPolyphonyL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}

TBool CMidiCustomCommandParser::DoSetPositionMicroBeatsL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetPositionMicroBeatsL(pckg().iPositionMicroBeats);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoPositionMicroBeatsL(TMMFMessage& aMessage)
	{
	TInt64 microBeats = 0;
	iImplementor.MmcPositionMicroBeatsL(microBeats);
	TPckgBuf<TMMFMidiConfig2> pckg;
	pckg().iPositionMicroBeats = microBeats;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoPlayNoteL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcPlayNoteL(pckg().iChannel, pckg().iNote, pckg().iDurationMicroSeconds, pckg().iNoteOnVelocity, pckg().iNoteOffVelocity);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoPlayNoteWithStartTimeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcPlayNoteL(pckg().iChannel, pckg().iNote, pckg().iStartTime, pckg().iDurationMicroSeconds, pckg().iNoteOnVelocity, pckg().iNoteOffVelocity);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoStopNotesL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcStopNotesL(pckg().iChannel);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoNoteOnL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcNoteOnL(pckg().iChannel, pckg().iNote, pckg().iNoteOnVelocity);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoNoteOffL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcNoteOffL(pckg().iChannel, pckg().iNote, pckg().iNoteOffVelocity);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoPlaybackRateL(TMMFMessage& aMessage)
	{
	TInt playBackRate;
	iImplementor.MmcPlaybackRateL(playBackRate);
	TPckgBuf<TMMFMidiConfig3> pckg;
	pckg().iPlayBackRate = playBackRate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetPlaybackRateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig3> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetPlaybackRateL(pckg().iPlayBackRate);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoMaxPlaybackRateL(TMMFMessage& aMessage)
	{
	TInt maxRate;
	iImplementor.MmcMaxPlaybackRateL(maxRate);
	TPckgBuf<TMMFMidiConfig3> pckg;
	pckg().iPlayBackMaxRate = maxRate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoMinPlaybackRateL(TMMFMessage& aMessage)
	{
	TInt minRate;
	iImplementor.MmcMinPlaybackRateL(minRate);
	TPckgBuf<TMMFMidiConfig3> pckg;
	pckg().iPlayBackMinRate = minRate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoTempoMicroBeatsPerMinuteL(TMMFMessage& aMessage)
	{
	TInt microBeatsPerMinute;
	iImplementor.MmcTempoMicroBeatsPerMinuteL(microBeatsPerMinute);
	TPckgBuf<TMMFMidiConfig1> pckg;
	pckg().iTempo = microBeatsPerMinute;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetTempoL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig1> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetTempoL(pckg().iTempo);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoPitchTranspositionCentsL(TMMFMessage& aMessage)
	{
	TInt pitch;
	iImplementor.MmcPitchTranspositionCentsL(pitch);
	TPckgBuf<TMMFMidiConfig1> pckg;
	pckg().iPitch = pitch;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetPitchTranspositionL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig1> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt centsApplied;
	iImplementor.MmcSetPitchTranspositionL(pckg().iPitch, centsApplied);
	pckg().iPitch = centsApplied;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoDurationMicroBeatsL(TMMFMessage& aMessage)
	{
	TInt64 duration;
	iImplementor.MmcDurationMicroBeatsL(duration);
	TPckgBuf<TMMFMidiConfig2> pckg;
	pckg().iDurationMicroBeats = duration;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoNumTracksL(TMMFMessage& aMessage)
	{
	TInt numTracks;
	iImplementor.MmcNumTracksL(numTracks);
	TPckgBuf<TMMFMidiConfig1> pckg;
	pckg().iNumTracks = numTracks;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetTrackMuteL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetTrackMuteL(pckg().iTrack, pckg().iMuted);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoMimeTypeL(TMMFMessage& aMessage)
	{
	HBufC8* mimeType = HBufC8::NewL(KMimeTypeLength);
	TPtr8 des = mimeType->Des();
	CleanupStack::PushL(mimeType);
	iImplementor.MmcMimeTypeL(des);
	aMessage.WriteDataToClientL(des);
	CleanupStack::PopAndDestroy();//mimeType
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetSyncUpdateCallbackIntervalL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig3> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetSyncUpdateCallbackIntervalL(pckg().iCallbackIntervalMicroSeconds, pckg().iCallbackIntervalMicroBeats);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSendMessageL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig3> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt bytesProcessed;
	iImplementor.MmcSendMessageL(*(pckg().iMidiMessage), bytesProcessed);
	pckg().iBytesProcessed = bytesProcessed;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSendMessageWithTimeStampL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig3> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt bytesProcessed;
	iImplementor.MmcSendMessageL(*(pckg().iMidiMessage), pckg().iTimeStamp, bytesProcessed);
	pckg().iBytesProcessed = bytesProcessed;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSendMipMessageL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig1> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSendMipMessageL(*(pckg().iMipMessage));
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoNumberOfBanksL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt numBanks;
	iImplementor.MmcNumberOfBanksL(pckg().iCustom, numBanks);
	pckg().iNumBanks = numBanks;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoGetBankIdL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt bankId;
	iImplementor.MmcGetBankIdL(pckg().iCustom, pckg().iBankIndex, bankId);
	pckg().iBankId = bankId;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoLoadCustomBankL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt bankId;
	iImplementor.MmcLoadCustomBankL(*(pckg().iFileName), bankId);
	pckg().iBankId = bankId;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoLoadCustomBankDataL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt bankId;
	iImplementor.MmcLoadCustomBankDataL(*(pckg().iBankData), bankId);
	pckg().iBankId = bankId;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoUnloadCustomBankL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcUnloadCustomBankL(pckg().iBankId);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoCustomBankLoadedL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TBool bankLoaded;
	iImplementor.MmcCustomBankLoadedL(pckg().iBankId, bankLoaded);
	pckg().iBankLoaded = bankLoaded;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoUnloadAllCustomBanksL(TMMFMessage& /*aMessage*/)
	{
	iImplementor.MmcUnloadAllCustomBanksL();
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoNumberOfInstrumentsL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt numInstruments;
	iImplementor.MmcNumberOfInstrumentsL(pckg().iBankId, pckg().iCustom, numInstruments);	
	pckg().iNumInstruments = numInstruments;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoGetInstrumentIdL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt instrumentId;
	iImplementor.MmcGetInstrumentIdL(pckg().iBankId, pckg().iCustom, pckg().iInstrumentIndex, instrumentId);
	pckg().iInstrumentId = instrumentId;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoInstrumentNameL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);

	// Prevent memory leaks by deleting old instrument name - something must have gone wrong in the client
	// if it already exists
	delete iInstrumentName;
	iInstrumentName = NULL;

	// Get the instrument name from the controller
	const TDesC& instrumentName = iImplementor.MmcInstrumentNameL(pckg().iBankId, pckg().iCustom, pckg().iInstrumentId);

	iInstrumentName = CBufFlat::NewL(32);
	RBufWriteStream stream;
	stream.Open(*iInstrumentName);
	CleanupClosePushL(stream);
	stream << instrumentName;
	CleanupStack::PopAndDestroy();//s

	// Write the size of the descriptor back to the client
	TPckgBuf<TInt> descriptorSizePckg(iInstrumentName->Ptr(0).Length());	
	aMessage.WriteDataToClientL(descriptorSizePckg);

	return ETrue;
	}

TBool CMidiCustomCommandParser::DoCopyInstrumentNameL(TMMFMessage& aMessage)
	{
	if (!iInstrumentName)
		User::Leave(KErrNotReady);

	// Copy the instrument name back to the client
	aMessage.WriteDataToClientL(iInstrumentName->Ptr(0));
	delete iInstrumentName;
	iInstrumentName = NULL;
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetInstrumentL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetInstrumentL(pckg().iChannel, pckg().iBankId, pckg().iInstrumentId);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoLoadCustomInstrumentL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcLoadCustomInstrumentL(*(pckg().iFileName), pckg().iBankId, pckg().iInstrumentId, pckg().iMemoryBankId,  pckg().iMemoryInstrumentId);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoLoadCustomInstrumentDataL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcLoadCustomInstrumentDataL(*(pckg().iInstrumentData), pckg().iBankId, pckg().iInstrumentId, pckg().iMemoryBankId,  pckg().iMemoryInstrumentId);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoUnloadCustomInstrumentL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcUnloadCustomInstrumentL(pckg().iBankId, pckg().iInstrumentId);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoPercussionKeyNameL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);

	// Prevent memory leaks by deleting old key name - something must have gone wrong in the client
	// if it already exists
	delete iPercussionKeyName;
	iPercussionKeyName = NULL;

	const TDesC& percussionKeyName = iImplementor.MmcPercussionKeyNameL(pckg().iNote, pckg().iBankId, pckg().iCustom, pckg().iInstrumentId);

	iPercussionKeyName = CBufFlat::NewL(32);
	RBufWriteStream stream;
	stream.Open(*iPercussionKeyName);
	CleanupClosePushL(stream);
	stream << percussionKeyName;
	CleanupStack::PopAndDestroy();//s

	// Write the size of the descriptor back to the client
	TPckgBuf<TInt> descriptorSizePckg(iPercussionKeyName->Ptr(0).Length());	
	aMessage.WriteDataToClientL(descriptorSizePckg);	
	
	return ETrue;
	}


TBool CMidiCustomCommandParser::DoCopyPercussionKeyNameL(TMMFMessage& aMessage)
	{
	if (!iPercussionKeyName)
		User::Leave(KErrNotReady);

	// Copy the instrument name back to the client
	aMessage.WriteDataToClientL(iPercussionKeyName->Ptr(0));
	delete iPercussionKeyName;
	iPercussionKeyName = NULL;
	return ETrue;
	}


TBool CMidiCustomCommandParser::DoStopTimeL(TMMFMessage& aMessage)
	{
	TTimeIntervalMicroSeconds stopTime;
	iImplementor.MmcStopTimeL(stopTime);
	TPckgBuf<TMMFMidiConfig2> pckg;
	pckg().iStopTime = stopTime;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetStopTimeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetStopTimeL(pckg().iStopTime);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoPolyphonyL(TMMFMessage& aMessage)
	{
	TInt numNotes;
	iImplementor.MmcPolyphonyL(numNotes);
	TPckgBuf<TMMFMidiConfig1> pckg;
	pckg().iNumNotes = numNotes;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoMaxPolyphonyL(TMMFMessage& aMessage)
	{
	TInt maxNotes;
	iImplementor.MmcMaxPolyphonyL(maxNotes);
	TPckgBuf<TMMFMidiConfig1> pckg;
	pckg().iMaxNotes = maxNotes;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoChannelsSupportedL(TMMFMessage& aMessage)
	{
	TInt channels;
	iImplementor.MmcChannelsSupportedL(channels);
	TPckgBuf<TMMFMidiConfig2> pckg;
	pckg().iChannel = channels;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoChannelVolumeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TReal32 channelVol;
	iImplementor.MmcChannelVolumeL(pckg().iChannel, channelVol);
	pckg().iChannelVol = channelVol;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoMaxChannelVolumeL(TMMFMessage& aMessage)
	{
	TReal32 maxVol;
	iImplementor.MmcMaxChannelVolumeL(maxVol);
	TPckgBuf<TMMFMidiConfig2> pckg;
	pckg().iMaxChannelVol = maxVol;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetChannelVolumeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetChannelVolumeL(pckg().iChannel, pckg().iChannelVol);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetChannelMuteL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetChannelMuteL(pckg().iChannel, pckg().iMuted);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoVolumeL(TMMFMessage& aMessage)
	{
	TInt vol;  
	iImplementor.MmcVolumeL(vol);
	TPckgBuf<TMMFMidiConfig1> pckg;
	pckg().iVolume = vol;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoMaxVolumeL(TMMFMessage& aMessage)
	{
	TInt maxVol;
	iImplementor.MmcMaxVolumeL(maxVol);
	TPckgBuf<TMMFMidiConfig1> pckg;
	pckg().iMaxVolume = maxVol;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetVolumeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig1> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetVolumeL(pckg().iVolume);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetVolumeRampL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig1> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetVolumeRampL(pckg().iRampDuration);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoGetBalanceL(TMMFMessage& aMessage)
	{
	TInt balance;
	iImplementor.MmcGetBalanceL(balance);
	TPckgBuf<TMMFMidiConfig1> pckg;
	pckg().iBalance = balance;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetBalanceL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig1> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetBalanceL(pckg().iBalance);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetMaxPolyphonyL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig1> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetMaxPolyphonyL(pckg().iMaxNotes);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoGetRepeatsL(TMMFMessage& aMessage)
	{
	TInt numRepeats;
	iImplementor.MmcGetRepeatsL(numRepeats);
	TPckgBuf<TMMFMidiConfig1> pckg;
	pckg().iNumRepeats = numRepeats;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetRepeatsL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig3> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetRepeatsL(pckg().iRepeatNumberOfTimes, pckg().iTrailingSilence);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoSetBankL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcSetBankL(pckg().iCustom);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoIsTrackMuteL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TBool mute;
	iImplementor.MmcIsTrackMuteL(pckg().iTrack, mute);
	pckg().iMuted = mute;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoIsChannelMuteL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TBool mute;
	iImplementor.MmcIsChannelMuteL(pckg().iChannel, mute);
	pckg().iMuted = mute;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoGetInstrumentL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig2> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt instrumentId;
	TInt bankId;
	iImplementor.MmcGetInstrumentL(pckg().iChannel, instrumentId, bankId);
	pckg().iInstrumentId = instrumentId;
	pckg().iBankId = bankId;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoCloseL(TMMFMessage& /*aMessage*/)
	{
	iImplementor.MmcCloseL();
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoStopL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFMidiConfig1> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MmcStopL(pckg().iFadeOutDuration);
	return ETrue;
	}

TBool CMidiCustomCommandParser::DoReceiveEventsL(TMMFMessage& aMessage)
	{
	if (iMidiEventReceiver)
		{
		if (iMidiEventReceiver->IsWaitingToSendEvent())
			{
			//Something must have gone wrong in the client
			// - we're waiting to get a RetrieveEvent() call, but it didn't come.
			// So, delete the existing event receiver
			delete iMidiEventReceiver;
			iMidiEventReceiver = NULL;
			}
		else
			{
			User::Leave(KErrAlreadyExists);
			}
		}
	ASSERT(!iMidiEventReceiver);
	iMidiEventReceiver = CMidiEventReceiver::NewL(aMessage);
	//send the next cached event (if any) to the client
	if (iMidiEvents.Count() > 0)
		{
		CMMFMidiEvent* midiEvent = iMidiEvents[0];
		iMidiEventReceiver->PrepareEventL(*midiEvent);
		iMidiEvents.Remove(0);
		delete midiEvent;
		}
	return EFalse;
	}

TBool CMidiCustomCommandParser::DoRetrieveEventL(TMMFMessage& aMessage)
	{
	if (iMidiEventReceiver)
		{
		iMidiEventReceiver->SendEventL(aMessage);
		delete iMidiEventReceiver;
		iMidiEventReceiver = NULL;
		}
	else
		{
		User::Leave(KErrNotReady);
		}

	return ETrue;
	}

/**
Sent a MIDI event back to the client.

@param aEvent MIDI event to be sent to the client.
@return One of the system-wide error codes.
*/
TInt CMidiCustomCommandParser::SendMidiEventToClient(const CMMFMidiEvent& aEvent)
	{
	TInt error = KErrNone;
	if (iMidiEventReceiver && !iMidiEventReceiver->IsWaitingToSendEvent())
		{
		//prepare to send event to client
		TRAP(error, iMidiEventReceiver->PrepareEventL(aEvent));
		}
	else
		{
		//queue the request for later
		CMMFMidiEvent* midiEvent = new CMMFMidiEvent();
		if (!midiEvent)
			return KErrNoMemory;

		// coverity[leave_without_push]
		TRAP(error, midiEvent->CopyL(aEvent));
		//if we've exceeded the max number of cached messages, delete the first and append this one to the end
		if (!error)
			{
			error = iMidiEvents.Append(midiEvent);
			}

		if(error != KErrNone)
			delete midiEvent;
		}
	return error;
	}


TBool CMidiCustomCommandParser::DoCancelReceiveEventsL(TMMFMessage& /*aMessage*/)
	{
	delete iMidiEventReceiver;
	iMidiEventReceiver = NULL;
	return ETrue;
	}
