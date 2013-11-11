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
/**
@internalComponent
@released
*/
#ifndef __MIDIEVENTRECEIVER_H__
#define __MIDIEVENTRECEIVER_H__

/**
@internalComponent
*/
enum TMMFMidiControllerMessages
	{
	EMMFMidiControllerSetPositionMicroBeats,
	EMMFMidiControllerPositionMicroBeats,
	EMMFMidiControllerPlayNote,
	EMMFMidiControllerPlayNoteWithStartTime,
	EMMFMidiControllerStopNotes,
	EMMFMidiControllerNoteOn,
	EMMFMidiControllerNoteOff,
	EMMFMidiControllerPlaybackRate,
	EMMFMidiControllerSetPlaybackRate,
	EMMFMidiControllerMaxPlaybackRate,
	EMMFMidiControllerMinPlaybackRate,
	EMMFMidiControllerTempo,
	EMMFMidiControllerSetTempo,
	EMMFMidiControllerPitch,
	EMMFMidiControllerSetPitch,
	EMMFMidiControllerDurationMicroBeats,
	EMMFMidiControllerNumTracks,
	EMMFMidiControllerSetTrackMute,
	EMMFMidiControllerMimeType,
	EMMFMidiControllerSetSyncUpdateCallbackInterval,
	EMMFMidiControllerSendMessage,
	EMMFMidiControllerSendMessageWithTimeStamp,
	EMMFMidiControllerSendMipMessage,
	EMMFMidiControllerNumberOfBanks,
	EMMFMidiControllerGetBankId,
	EMMFMidiControllerLoadCustomBank,
	EMMFMidiControllerLoadCustomBankData,
	EMMFMidiControllerUnloadCustomBank,
	EMMFMidiControllerCustomBankLoaded,
	EMMFMidiControllerUnloadAllCustomBanks,
	EMMFMidiControllerNumberOfInstruments,
	EMMFMidiControllerGetInstrumentId,
	EMMFMidiControllerInstrumentName,
	EMMFMidiControllerCopyInstrumentName,
	EMMFMidiControllerSetInstrument,
	EMMFMidiControllerLoadCustomInstrument,
	EMMFMidiControllerLoadCustomInstrumentData,
	EMMFMidiControllerUnloadCustomInstrument,
	EMMFMidiControllerPercussionKeyName,
	EMMFMidiControllerCopyPercussionKeyName,
	EMMFMidiControllerSetStopTime,
	EMMFMidiControllerStopTime,
	EMMFMidiControllerPolyphony,
	EMMFMidiControllerSetMaxPolyphony,
	EMMFMidiControllerChannelsSupported,
	EMMFMidiControllerChannelVolume,
	EMMFMidiControllerMaxChannelVolume,
	EMMFMidiControllerSetChannelVolume,
	EMMFMidiControllerSetChannelMute,
	EMMFMidiControllerVolume,
	EMMFMidiControllerMaxVolume,
	EMMFMidiControllerSetVolume,
	EMMFMidiControllerSetVolumeRamp,
	EMMFMidiControllerGetBalance,
	EMMFMidiControllerSetBalance,
	EMMFMidiControllerGetRepeats,
	EMMFMidiControllerSetRepeats,
	EMMFMidiControllerSetBank,
	EMMFMidiControllerIsTrackMute,
	EMMFMidiControllerIsChannelMute,
	EMMFMidiControllerGetInstrument,
	EMMFMidiControllerClose,
	EMMFMidiControllerStop,
	EMMFMidiControllerReceiveEvents,
	EMMFMidiControllerRetrieveEvent,
	EMMFMidiControllerCancelReceiveEvents,
	EMMFMidiControllerMaxPolyphony
	};

/**
@internalComponent
*/
class CMidiEventReceiver : public CBase
	{
public:
	static CMidiEventReceiver* NewL(const TMMFMessage& aMessage);
	~CMidiEventReceiver();
	void PrepareEventL(const CMMFMidiEvent& aEvent);
	void SendEventL(TMMFMessage& aMessage);
	TBool IsWaitingToSendEvent();
private:
	CMidiEventReceiver(const TMMFMessage& aMessage);
private:
	TMMFMessage iMessage;
	CBufFlat* iEventBuf;
	};

#endif //__MIDIEVENTRECEIVER_H__
