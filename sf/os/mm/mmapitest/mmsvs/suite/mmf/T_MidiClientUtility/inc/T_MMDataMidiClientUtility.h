/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



#if (!defined __T_MM_DATA_MIDI_CLIENT_UTILITY_H__ )
#define __T_MM_DATA_MIDI_CLIENT_UTILITY_H__


#include "DataWrapperBase.h"

//	EPOC includes
#include <ecam.h>
#include <w32std.h>
#include <testexecuteserverutils.h>
#include <testexecutestepbase.h>
#include <midiclientutility.h>

const TInt	KCustomCommandBufferSize		=255;

/**
CMidiClientUtility API test commands
@internalComponent
*/
class CT_MMDataMidiClientUtility : public CDataWrapperBase, private MMidiClientUtilityObserver
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_MMDataMidiClientUtility*	NewL();

	/**
	* Public destructor
	*/
	~CT_MMDataMidiClientUtility();

	//MMidiClientUtilityObserver implementation
	virtual void	MmcuoStateChanged(TMidiState aOldState, TMidiState aNewState, const TTimeIntervalMicroSeconds &aTime, TInt aError);
	virtual void	MmcuoTempoChanged(TInt aMicroBeatsPerMinute);
	virtual void	MmcuoVolumeChanged(TInt aChannel, TReal32 aVolumeInDecibels);
	virtual void	MmcuoMuteChanged(TInt aChannel, TBool aMuted);
	virtual void	MmcuoSyncUpdate(const TTimeIntervalMicroSeconds &aMicroSeconds, TInt64 aMicroBeats);
	virtual void	MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId, const TTimeIntervalMicroSeconds &aPosition);
	virtual void	MmcuoMipMessageReceived(const RArray< TMipMessageEntry > &aMessage);
	virtual void	MmcuoPolyphonyChanged(TInt aNewPolyphony);
	virtual void	MmcuoInstrumentChanged(TInt aChannel, TInt aBankId, TInt aInstrumentId);

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMDataMidiClientUtility();

	void	ConstructL();

private:
	inline void			DoCmdChannelVolumeL(const TDesC& aSection);
	inline void			DoCmdChannelsSupportedL(const TDesC& aSection);
	inline void			DoCmdClose(const TInt aAsyncErrorIndex);
	inline void			DoCmdCustomBankLoadedL(const TDesC& aSection);
	inline void			DoCmdCustomCommandAsyncL(const TDesC& aSectionx, const TInt aAsyncErrorIndex);
	inline void			DoCmdCustomCommandSyncL(const TDesC& aSection);
	inline void			DoCmdDurationMicroBeatsL(const TDesC& aSection);
	inline void			DoCmdDurationMicroSecondsL(const TDesC& aSection);
	inline void			DoCmdGetBalanceL(const TDesC& aSection);
	inline void			DoCmdGetBankIdL(const TDesC& aSection);
	inline void			DoCmdGetDRMCustomCommand(const TDesC& aSection);
	inline void			DoCmdGetInstrumentIdL(const TDesC& aSection);
	inline void			DoCmdGetInstrumentL(const TDesC& aSection);
	inline void			DoCmdGetMetaDataEntryL(const TDesC& aSection);
	inline void			DoCmdGetRepeats(const TDesC& aSection);
	inline void			DoCmdInstrumentNameL(const TDesC& aSection);
	inline void			DoCmdIsChannelMuteL(const TDesC& aSection);
	inline void			DoCmdIsTrackMuteL(const TDesC& aSection);
	inline void			DoCmdLoadCustomBankDataL(const TDesC& aSection);
	inline void			DoCmdLoadCustomBankL(const TDesC& aSection);
	inline void			DoCmdLoadCustomInstrumentDataL(const TDesC& aSection);
	inline void			DoCmdLoadCustomInstrumentL(const TDesC& aSection);
	inline void			DoCmdMaxChannelVolumeL(const TDesC& aSection);
	inline void			DoCmdMaxPlaybackRateL(const TDesC& aSection);
	inline void			DoCmdMaxPolyphonyL(const TDesC& aSection);
	inline void			DoCmdMaxVolumeL(const TDesC& aSection);
	inline void			DoCmdMimeTypeL(const TDesC& aSection);
	inline void			DoCmdMinPlaybackRateL(const TDesC& aSection);
	inline void			DoCmdNewL(const TDesC& aSection);
	inline void			DoCmdNoteOffL(const TDesC& aSection);
	inline void			DoCmdNoteOnL(const TDesC& aSection);
	inline void			DoCmdNumTracksL(const TDesC& aSection);
	inline void			DoCmdNumberOfBanksL(const TDesC& aSection);
	inline void			DoCmdNumberOfInstrumentsL(const TDesC& aSection);
	inline void			DoCmdNumberOfMetaDataEntriesL(const TDesC& aSection);
	inline void			DoCmdOpenDesL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void			DoCmdOpenFileL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void			DoCmdOpenUrlL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void			DoCmdPercussionKeyNameL(const TDesC& aSection);
	inline void			DoCmdPitchTranspositionCentsL(const TDesC& aSection);
	inline void			DoCmdPlay(const TInt aAsyncErrorIndex);
	inline void			DoCmdPlayNoteL(const TDesC& aSection);
	inline void			DoCmdPlaybackRateL(const TDesC& aSection);
	inline void			DoCmdPolyphonyL(const TDesC& aSection);
	inline void			DoCmdPositionMicroBeatsL(const TDesC& aSection);
	inline void			DoCmdPositionMicroSecondsL(const TDesC& aSection);
	inline void			DoCmdSendMessageL(const TDesC& aSection);
	inline void			DoCmdSendMipMessageL(const TDesC& aSection);
	inline void			DoCmdSetBalanceL(const TDesC& aSection);
	inline void			DoCmdSetBankL(const TDesC& aSection);
	inline void			DoCmdSetChannelMuteL(const TDesC& aSection);
	inline void			DoCmdSetChannelVolumeL(const TDesC& aSection);
	inline void			DoCmdSetInstrumentL(const TDesC& aSection);
	inline void			DoCmdSetMaxPolyphonyL(const TDesC& aSection);
	inline void			DoCmdSetPitchTranspositionL(const TDesC& aSection);
	inline void			DoCmdSetPlaybackRateL(const TDesC& aSection);
	inline void			DoCmdSetPositionMicroBeatsL(const TDesC& aSection);
	inline void			DoCmdSetPositionMicroSecondsL(const TDesC& aSection);
	inline void			DoCmdSetPriorityL(const TDesC& aSection);
	inline void			DoCmdSetRepeatsL(const TDesC& aSection);
	inline void			DoCmdSetStopTimeL(const TDesC& aSection);
	inline void			DoCmdSetSyncUpdateCallbackIntervalL(const TDesC& aSection);
	inline void			DoCmdSetTempoL(const TDesC& aSection);
	inline void			DoCmdSetTrackMuteL(const TDesC& aSection);
	inline void			DoCmdSetVolumeL(const TDesC& aSection);
	inline void			DoCmdSetVolumeRampL(const TDesC& aSection);
	inline void			DoCmdState(const TDesC& aSection);
	inline void			DoCmdStop(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void			DoCmdStopNotes(const TDesC& aSection);
	inline void			DoCmdStopTimeL(const TDesC& aSection);
	inline void			DoCmdTempoMicroBeatsPerMinuteL(const TDesC& aSection);
	inline void			DoCmdUnloadAllCustomBanksL();
	inline void			DoCmdUnloadCustomBankL(const TDesC& aSection);
	inline void			DoCmdUnloadCustomInstrumentL(const TDesC& aSection);
	inline void			DoCmdVolumeL(const TDesC& aSection);
	inline void			DoCmdDestructor();

	//	Helper methods
	void				DestroyData();
	void				LogDataFromTextL();

	//	CActiveCallback implementation
 	void 				RunL(CActive* aActive, TInt aIndex);
 	void				DoCancel(CActive* aActive, TInt aIndex);
 	inline void			RunCustomCommandAsyncL(TInt aIndex);
 	inline void			DoCancelCustomCommandAsync();

private:
	CMidiClientUtility*				iMidiClient;
	RFile							iFile;
	CActiveCallback*				iActiveCallback;
	HBufC8*							iBuffer;
	TInt 							iApiAsyncErrorIndex;
	TInt							iCustomBankId;
	TBool							iFileOpened;
	RFs								iFs;
	TUid							iMidiControllerUid;
	TUid							iMidiFormatUid;
	TReal32 						iMaxChannelVolume;
	TInt	 						iMaxVolume;
	TInt							iPlaybackRate;
	TInt							iMaxPolyphony;
	HBufC8*							iCommandDataBuffer1;
	HBufC8*							iCommandDataBuffer2;
	TBuf8<KCustomCommandBufferSize>	iDataFromText;
	TBool 							iCustomCommandData;
	TPtr8*							iBufferPtr;
	};

#endif /* __T_MM_DATA_MIDI_CLIENT_UTILITY_H__ */
