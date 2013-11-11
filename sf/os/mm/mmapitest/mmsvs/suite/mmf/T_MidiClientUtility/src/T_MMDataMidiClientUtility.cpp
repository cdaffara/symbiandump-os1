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


#include "T_MMDataMidiClientUtility.h"
#include "T_MMUtil.h"

/*@{*/
_LIT(KRFile,							"rfile");
_LIT(KTMMSource,						"tmmsource");
_LIT(KUrl,								"url");
_LIT(KChannel,							"channel");
_LIT(KChannelsNum,						"channelsNum");
_LIT(KTrack,							"track");
_LIT(KVolume,							"volume");
_LIT(KBankId,							"bankId");
_LIT(KInstrumentId,						"instrumentId");
_LIT(KCustomBank,						"customBank");
_LIT(KBalance,							"balance");
_LIT(KMute,								"mute");
_LIT(KRepeats,							"repeats");
_LIT(KTimeIntervalSec,					"intervalSec");
_LIT(KTempo,							"tempo");
_LIT(KNote,								"note");
_LIT(KVelocity,							"velocity");
_LIT(KStartTime,						"startTime");
_LIT(KDuration,							"duration");
_LIT(KNoteOnVelocity,					"noteOnVelocity");
_LIT(KNoteOffVelocity,					"noteOffVelocity");
_LIT(KCents,							"cents");
_LIT(KRate,								"rate");
_LIT(KPosition,							"position");
_LIT(KMaxNotes,							"maxNotes");
_LIT(KCustom,							"custom");
_LIT(KMetaDataIndex,					"metaDataIndex");
_LIT(KBankData,							"bankData");
_LIT(KMicroBeats,						"microBeats");
_LIT(KMicroSeconds,						"microSeconds");
_LIT(KFileBankId,						"fileBankId");
_LIT(KFileInstrumentId,					"fileInstrumentId");
_LIT(KMemoryBankId,						"memoryBankId");
_LIT(KMemoryInstrumentId,				"memoryInstrumentId");
_LIT(KFileName,							"fileName");
_LIT(KUseDataFrom,						"useDataFrom");
_LIT(KFunctionId,						"functionId");
_LIT(KTime,								"time");
_LIT(KMIPValue,							"mipValue%d");
_LIT(KData1,							"data1");
_LIT(KData2,							"data2");
_LIT(KLoaded,							"loaded");
_LIT(KIsNull,							"isnull");
_LIT(KName,								"name");
_LIT(KMimeType,							"mimeType");
_LIT(KNumTracks,						"numTracks");
_LIT(KBanks,							"banks");
_LIT(KInstruments,						"instruments");
_LIT(KMetaDataNum,						"metaDataNum");
_LIT(KUniqueId, 						"uniqueid");
_LIT(KEnableUI, 						"enableui");
_LIT(KAccessPointID, 					"accesspoint");
_LIT(KState, 							"state");
_LIT(KShift, 							"shift");
_LIT(KIdentifier, 						"identifier");
_LIT(KOption,							"option%d");

_LIT(KCmdNewL,							"NewL");
_LIT(KCmdDestructor,					"~");
_LIT(KCmdOpenFile,						"OpenFile");
_LIT(KCmdOpenDes,						"OpenDes");
_LIT(KCmdOpenUrl,						"OpenUrl");
_LIT(KCmdPlay,							"Play");
_LIT(KCmdStop,							"Stop");
_LIT(KCmdClose,							"Close");
_LIT(KCmdDurationMicroBeatsL,			"DurationMicroBeatsL");
_LIT(KCmdDurationMicroSecondsL,			"DurationMicroSecondsL");
_LIT(KCmdNumTracksL,					"NumTracksL");
_LIT(KCmdChannelsSupportedL,			"ChannelsSupportedL");
_LIT(KCmdChannelVolumeL,				"ChannelVolumeL");
_LIT(KCmdMaxChannelVolumeL,				"MaxChannelVolumeL");
_LIT(KCmdSetChannelVolumeL,				"SetChannelVolumeL");
_LIT(KCmdSetVolumeL,					"SetVolumeL");
_LIT(KCmdMaxVolumeL,					"MaxVolumeL");
_LIT(KCmdVolumeL,						"VolumeL");
_LIT(KCmdState,							"State");
_LIT(KCmdCustomBankLoadedL,				"CustomBankLoadedL");
_LIT(KCmdNumberOfBanksL,				"NumberOfBanksL");
_LIT(KCmdNumberOfInstrumentsL,			"NumberOfInstrumentsL");
_LIT(KCmdNumberOfMetaDataEntriesL,		"NumberOfMetaDataEntriesL");
_LIT(KCmdGetBalanceL,					"GetBalanceL");
_LIT(KCmdSetBalanceL,					"SetBalanceL");
_LIT(KCmdGetBankIdL,					"GetBankIdL");
_LIT(KCmdGetInstrumentIdL,				"GetInstrumentIdL");
_LIT(KCmdIsChannelMuteL,				"IsChannelMuteL");
_LIT(KCmdIsTrackMuteL,					"IsTrackMuteL");
_LIT(KCmdSetChannelMuteL,				"SetChannelMuteL");
_LIT(KCmdSetTrackMuteL,					"SetTrackMuteL");
_LIT(KCmdGetRepeats,					"GetRepeats");
_LIT(KCmdSetRepeatsL,					"SetRepeatsL");
_LIT(KCmdSetStopTimeL,					"SetStopTimeL");
_LIT(KCmdStopTimeL,						"StopTimeL");
_LIT(KCmdTempoMicroBeatsPerMinuteL,		"TempoMicroBeatsPerMinuteL");
_LIT(KCmdSetTempoL,						"SetTempoL");
_LIT(KCmdNoteOnL,						"NoteOnL");
_LIT(KCmdNoteOffL,						"NoteOffL");
_LIT(KCmdPlayNoteL,						"PlayNoteL");
_LIT(KCmdStopNotes,						"StopNotes");
_LIT(KCmdPitchTranspositionCentsL,		"PitchTranspositionCentsL");
_LIT(KCmdSetPitchTranspositionL,		"SetPitchTranspositionL");
_LIT(KCmdPlaybackRateL,					"PlaybackRateL");
_LIT(KCmdSetPlaybackRateL,				"SetPlaybackRateL");
_LIT(KCmdMaxPlaybackRateL,				"MaxPlaybackRateL");
_LIT(KCmdMinPlaybackRateL,				"MinPlaybackRateL");
_LIT(KCmdPositionMicroSecondsL,			"PositionMicroSecondsL");
_LIT(KCmdSetPositionMicroSecondsL,		"SetPositionMicroSecondsL");
_LIT(KCmdPositionMicroBeatsL,			"PositionMicroBeatsL");
_LIT(KCmdSetPositionMicroBeatsL,		"SetPositionMicroBeatsL");
_LIT(KCmdSetMaxPolyphonyL,				"SetMaxPolyphonyL");
_LIT(KCmdPolyphonyL,					"PolyphonyL");
_LIT(KCmdMaxPolyphonyL,					"MaxPolyphonyL");
_LIT(KCmdSetPriorityL,					"SetPriorityL");
_LIT(KCmdSetVolumeRampL,				"SetVolumeRampL");
_LIT(KCmdUnloadAllCustomBanksL,			"UnloadAllCustomBanksL");
_LIT(KCmdUnloadCustomBankL,				"UnloadCustomBankL");
_LIT(KCmdUnloadCustomInstrumentL,		"UnloadCustomInstrumentL");
_LIT(KCmdSetBankL,						"SetBankL");
_LIT(KCmdSetInstrumentL,				"SetInstrumentL");
_LIT(KCmdMimeTypeL,						"MimeTypeL");
_LIT(KCmdGetDRMCustomCommand,			"GetDRMCustomCommand");
_LIT(KCmdInstrumentNameL,				"InstrumentNameL");
_LIT(KCmdGetMetaDataEntryL,				"GetMetaDataEntryL");
_LIT(KCmdPercussionKeyNameL,			"PercussionKeyNameL");
_LIT(KCmdGetInstrumentL,				"GetInstrumentL");
_LIT(KCmdLoadCustomBankL,				"LoadCustomBankL");
_LIT(KCmdLoadCustomBankDataL,			"LoadCustomBankDataL");
_LIT(KCmdSetSyncUpdateCallbackIntervalL,"SetSyncUpdateCallbackIntervalL");
_LIT(KCmdLoadCustomInstrumentL,			"LoadCustomInstrumentL");
_LIT(KCmdLoadCustomInstrumentDataL,		"LoadCustomInstrumentDataL");
_LIT(KCmdCustomCommandSyncL,			"CustomCommandSyncL");
_LIT(KCmdCustomCommandAsync,			"CustomCommandAsync");
_LIT(KCmdSendMessageL,					"SendMessageL");
_LIT(KCmdSendMipMessageL,				"SendMipMessageL");

_LIT(KNullString,						"null");
/*@}*/

const TInt	KMimeTypeBufferSize			=30;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
Two phase constructor
@internalComponent
@return Data Dictionary object
@pre None
@post None
*/
CT_MMDataMidiClientUtility* CT_MMDataMidiClientUtility::NewL()
	{
	CT_MMDataMidiClientUtility*	ret=new (ELeave) CT_MMDataMidiClientUtility();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
CT_MMDataMidiClientUtility constructor.
@internalComponent
@return N/A
@pre None
@post CT_MMDataMidiClientUtility 1st stage constructor
*/
CT_MMDataMidiClientUtility::CT_MMDataMidiClientUtility()
:	iMidiClient(NULL)
,	iActiveCallback(NULL)
,	iBuffer(NULL)
,	iApiAsyncErrorIndex(0)
,	iCustomBankId(0)
,	iFileOpened(EFalse)
,	iMaxChannelVolume(0)
,	iMaxVolume(0)
,	iPlaybackRate(0)
,	iMaxPolyphony(0)
,	iCommandDataBuffer1(NULL)
,	iCommandDataBuffer2(NULL)
,	iCustomCommandData(EFalse)
,	iBufferPtr(NULL)
	{
	}

void CT_MMDataMidiClientUtility::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	iActiveCallback=CActiveCallback::NewL(*this);
	}

/**
CT_MMDataMidiClientUtility destructor
@internalComponent
@return N/A
@pre None
@post releases CT_MMDataMidiClientUtility heap memory
*/
CT_MMDataMidiClientUtility::~CT_MMDataMidiClientUtility()
	{
	DestroyData();

	delete iActiveCallback;
	iActiveCallback = NULL;

	delete iCommandDataBuffer1;
	iCommandDataBuffer1=NULL;

	delete iCommandDataBuffer2;
	iCommandDataBuffer2=NULL;

	iFs.Close();

	}

TAny* CT_MMDataMidiClientUtility::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return pointer to the object that the data wraps
 */
	{
	return iMidiClient;
	}

/**
Releases CT_MMDataMidiClientUtility dynamic memory.
@internalComponent
@return void
@pre None
@post All dynamic memory deallocated
*/
void CT_MMDataMidiClientUtility::DestroyData()
	{
	if( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	delete iMidiClient;
	iMidiClient=NULL;
	delete iBuffer;
	iBuffer=NULL;
	delete iBufferPtr;
	iBufferPtr=NULL;
	}

TBool CT_MMDataMidiClientUtility::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool	ret=ETrue;

	if( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if( aCommand==KCmdOpenDes )
		{
		DoCmdOpenDesL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdOpenFile )
		{
		DoCmdOpenFileL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdOpenUrl )
		{
		DoCmdOpenUrlL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdPlay )
		{
		DoCmdPlay(aAsyncErrorIndex);
		}
	else if( aCommand==KCmdDurationMicroBeatsL )
		{
		DoCmdDurationMicroBeatsL(aSection);
		}
	else if( aCommand==KCmdDurationMicroSecondsL )
		{
		DoCmdDurationMicroSecondsL(aSection);
		}
	else if( aCommand==KCmdNumTracksL )
		{
		DoCmdNumTracksL(aSection);
		}
	else if( aCommand==KCmdChannelsSupportedL )
		{
		DoCmdChannelsSupportedL(aSection);
		}
	else if( aCommand==KCmdChannelVolumeL )
		{
		DoCmdChannelVolumeL(aSection);
		}
	else if( aCommand==KCmdMaxChannelVolumeL )
		{
		DoCmdMaxChannelVolumeL(aSection);
		}
	else if( aCommand==KCmdSetChannelVolumeL )
		{
		DoCmdSetChannelVolumeL(aSection);
		}
	else if( aCommand==KCmdMaxVolumeL )
		{
		DoCmdMaxVolumeL(aSection);
		}
	else if( aCommand==KCmdSetVolumeL )
		{
		DoCmdSetVolumeL(aSection);
		}
	else if( aCommand==KCmdVolumeL )
		{
		DoCmdVolumeL(aSection);
		}
	else if( aCommand==KCmdState )
		{
		DoCmdState(aSection);
		}
	else if( aCommand==KCmdCustomBankLoadedL )
		{
		DoCmdCustomBankLoadedL(aSection);
		}
	else if( aCommand==KCmdNumberOfBanksL )
		{
		DoCmdNumberOfBanksL(aSection);
		}
	else if( aCommand==KCmdNumberOfInstrumentsL )
		{
		DoCmdNumberOfInstrumentsL(aSection);
		}
	else if( aCommand==KCmdNumberOfMetaDataEntriesL )
		{
		DoCmdNumberOfMetaDataEntriesL(aSection);
		}
	else if( aCommand==KCmdGetBalanceL )
		{
		DoCmdGetBalanceL(aSection);
		}
	else if( aCommand==KCmdSetBalanceL )
		{
		DoCmdSetBalanceL(aSection);
		}
	else if( aCommand==KCmdGetBankIdL )
		{
		DoCmdGetBankIdL(aSection);
		}
	else if( aCommand==KCmdGetInstrumentIdL )
		{
		DoCmdGetInstrumentIdL(aSection);
		}
	else if( aCommand==KCmdIsChannelMuteL )
		{
		DoCmdIsChannelMuteL(aSection);
		}
	else if( aCommand==KCmdSetChannelMuteL )
		{
		DoCmdSetChannelMuteL(aSection);
		}
	else if( aCommand==KCmdIsTrackMuteL )
		{
		DoCmdIsTrackMuteL(aSection);
		}
	else if( aCommand==KCmdSetTrackMuteL )
		{
		DoCmdSetTrackMuteL(aSection);
		}
	else if( aCommand==KCmdGetRepeats )
		{
		DoCmdGetRepeats(aSection);
		}
	else if( aCommand==KCmdSetRepeatsL )
		{
		DoCmdSetRepeatsL(aSection);
		}
	else if( aCommand==KCmdSetStopTimeL )
		{
		DoCmdSetStopTimeL(aSection);
		}
	else if( aCommand==KCmdTempoMicroBeatsPerMinuteL )
		{
		DoCmdTempoMicroBeatsPerMinuteL(aSection);
		}
	else if( aCommand==KCmdSetTempoL )
		{
		DoCmdSetTempoL(aSection);
		}
	else if( aCommand==KCmdNoteOnL )
		{
		DoCmdNoteOnL(aSection);
		}
	else if( aCommand==KCmdNoteOffL )
		{
		DoCmdNoteOffL(aSection);
		}
	else if( aCommand==KCmdPlayNoteL )
		{
		DoCmdPlayNoteL(aSection);
		}
	else if( aCommand==KCmdPitchTranspositionCentsL )
		{
		DoCmdPitchTranspositionCentsL(aSection);
		}
	else if( aCommand==KCmdSetPitchTranspositionL )
		{
		DoCmdSetPitchTranspositionL(aSection);
		}
	else if( aCommand==KCmdPlaybackRateL )
		{
		DoCmdPlaybackRateL(aSection);
		}
	else if( aCommand==KCmdSetPlaybackRateL )
		{
		DoCmdSetPlaybackRateL(aSection);
		}
	else if( aCommand==KCmdMaxPlaybackRateL )
		{
		DoCmdMaxPlaybackRateL(aSection);
		}
	else if( aCommand==KCmdMinPlaybackRateL )
		{
		DoCmdMinPlaybackRateL(aSection);
		}
	else if( aCommand==KCmdSetPositionMicroSecondsL )
		{
		DoCmdSetPositionMicroSecondsL(aSection);
		}
	else if( aCommand==KCmdPositionMicroSecondsL )
		{
		DoCmdPositionMicroSecondsL(aSection);
		}
	else if( aCommand==KCmdSetPositionMicroBeatsL )
		{
		DoCmdSetPositionMicroBeatsL(aSection);
		}
	else if( aCommand==KCmdPositionMicroBeatsL )
		{
		DoCmdPositionMicroBeatsL(aSection);
		}
	else if( aCommand==KCmdSetMaxPolyphonyL )
		{
		DoCmdSetMaxPolyphonyL(aSection); }

	else if( aCommand==KCmdPolyphonyL )
		{
		DoCmdPolyphonyL(aSection);
		}
	else if( aCommand==KCmdMaxPolyphonyL )
		{
		DoCmdMaxPolyphonyL(aSection);
		}
	else if( aCommand==KCmdSetPriorityL )
		{
		DoCmdSetPriorityL(aSection);
		}
	else if( aCommand==KCmdSetVolumeRampL )
		{
		DoCmdSetVolumeRampL(aSection);
		}
	else if( aCommand==KCmdLoadCustomBankL )
		{
		DoCmdLoadCustomBankL(aSection);
		}
	else if( aCommand==KCmdLoadCustomBankDataL )
		{
		DoCmdLoadCustomBankDataL(aSection);
		}
	else if( aCommand==KCmdUnloadAllCustomBanksL )
		{
		DoCmdUnloadAllCustomBanksL();
		}
	else if( aCommand==KCmdUnloadCustomBankL )
		{
		DoCmdUnloadCustomBankL(aSection);
		}
	else if( aCommand==KCmdUnloadCustomInstrumentL )
		{
		DoCmdUnloadCustomInstrumentL(aSection);
		}
	else if( aCommand==KCmdSetInstrumentL )
		{
		DoCmdSetInstrumentL(aSection);
		}
	else if( aCommand==KCmdInstrumentNameL )
		{
		DoCmdInstrumentNameL(aSection);
		}
	else if( aCommand==KCmdSetBankL )
		{
		DoCmdSetBankL(aSection);
		}
	else if( aCommand==KCmdMimeTypeL )
		{
		DoCmdMimeTypeL(aSection);
		}
	else if( aCommand==KCmdGetDRMCustomCommand )
		{
		DoCmdGetDRMCustomCommand(aSection);
		}
	else if( aCommand==KCmdGetMetaDataEntryL )
		{
		DoCmdGetMetaDataEntryL(aSection);
		}
	else if( aCommand==KCmdPercussionKeyNameL )
		{
		DoCmdPercussionKeyNameL(aSection);
		}
	else if( aCommand==KCmdStopNotes )
		{
		DoCmdStopNotes(aSection);
		}
	else if( aCommand==KCmdStopTimeL )
		{
		DoCmdStopTimeL(aSection);
		}
	else if( aCommand==KCmdStop )
		{
		DoCmdStop(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdClose )
		{
		DoCmdClose(aAsyncErrorIndex);
		}
	else if( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if( aCommand==KCmdGetInstrumentL )
		{
		DoCmdGetInstrumentL(aSection);
		}
	else if( aCommand==KCmdSetSyncUpdateCallbackIntervalL )
		{
		DoCmdSetSyncUpdateCallbackIntervalL(aSection);
		}
	else if( aCommand==KCmdLoadCustomInstrumentL )
		{
		DoCmdLoadCustomInstrumentL(aSection);
		}
	else if( aCommand==KCmdLoadCustomInstrumentDataL )
		{
		DoCmdLoadCustomInstrumentDataL(aSection);
		}
	else if( aCommand==KCmdCustomCommandAsync )
		{
		DoCmdCustomCommandAsyncL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdCustomCommandSyncL )
		{
		DoCmdCustomCommandSyncL(aSection);
		}
	else if( aCommand==KCmdSendMessageL )
		{
		DoCmdSendMessageL(aSection);
		}
	else if( aCommand==KCmdSendMipMessageL )
		{
		DoCmdSendMipMessageL(aSection);
		}
	else if( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}
	return ret;
	}


void CT_MMDataMidiClientUtility::DoCmdChannelVolumeL(const TDesC& aSection)
	{


	TInt	channel=0;
	GetIntFromConfig(aSection, KChannel(), channel);

	TReal32	volume = 0;
	TRAPD( err, volume=iMidiClient->ChannelVolumeL(channel) );
	if( err!=KErrNone )
	    {
	    ERR_PRINTF2(_L("ChannelVolumeL() error %d"),err);
	    SetError( err );
	    }
	else
	    {
	    INFO_PRINTF3(_L("ChannelVolumeL()Channel: %d, Volume %g"),channel, volume);

	    TInt	volTmp = 0;
	    if( GetIntFromConfig(aSection, KVolume(), volTmp) )
	    	{
	    	TReal32	volumeCfg = (TReal32)volTmp;
	    	if( volume!=volumeCfg )
	    		{
	        	ERR_PRINTF4(_L("ChannelVolumeL(%d) Volume %g != Expected Volume %g"), channel, volume, volumeCfg);
	        	SetBlockResult(EFail);
	    		}
	    	}
	    }
	}

void CT_MMDataMidiClientUtility::DoCmdChannelsSupportedL(const TDesC& aSection)
	{


	TInt	numChls=0;
	TRAPD( err, numChls=iMidiClient->ChannelsSupportedL() );
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("ChannelsSupportedL() error %d"),err);
		SetError( err );
		}
	else
		{
		INFO_PRINTF2(_L("ChannelsSupportedL() %d"), numChls);

		TInt	numChlsCfg = 0;
		if( GetIntFromConfig(aSection, KChannelsNum(), numChlsCfg) )
			{
			if( numChls!=numChlsCfg )
				{
				ERR_PRINTF3(_L("Actual number of channels %d != Expected number of channels %d"),numChls,numChlsCfg);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMidiClientUtility::DoCmdClose(const TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;
	iMidiClient->Close();
	IncOutstanding();
	INFO_PRINTF1(_L("Close()"));
	}

void CT_MMDataMidiClientUtility::DoCmdCustomBankLoadedL(const TDesC& aSection)
	{


	TInt	bankId = iCustomBankId;
	GetIntFromConfig(aSection, KBankId(), bankId);

	TBool	loaded = EFalse;
	TRAPD( err, loaded=iMidiClient->CustomBankLoadedL(bankId) );
	if( err==KErrNone )
		{
		INFO_PRINTF3(_L("CustomBankLoadedL(%d) = %d"), bankId, loaded);

		TBool	loadedCfg = EFalse;
		if( GetBoolFromConfig(aSection, KLoaded(), loadedCfg) )
			{
			if( loaded!=loadedCfg )
				{
				ERR_PRINTF2(_L("CustomBankLoadedL() %d != Expected Value"),bankId);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("CustomBankLoadedL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdCustomCommandAsyncL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	iApiAsyncErrorIndex = aAsyncErrorIndex;
	TInt	function = 0;
	GetIntFromConfig(aSection, KFunctionId(), function);

	iCustomCommandData = EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), iCustomCommandData);

	// reference to the first chunk of data
	delete iCommandDataBuffer1;
	iCommandDataBuffer1=NULL;
	TPtrC	dataTo1Str;
	if( GetStringFromConfig(aSection, KData1(), dataTo1Str) )
		{
		iCommandDataBuffer1=HBufC8::NewL(dataTo1Str.Length());
		iCommandDataBuffer1->Des().Copy(dataTo1Str);
		}
	else
		{
		iCommandDataBuffer1=HBufC8::NewL(0);
		iCommandDataBuffer1->Des().Copy(KNullDesC8);
		}

	// reference to the second chunk of data
	delete iCommandDataBuffer2;
	iCommandDataBuffer2=NULL;
	TPtrC	dataTo2Str;
	if( GetStringFromConfig(aSection, KData2(), dataTo2Str) )
		{
		iCommandDataBuffer2=HBufC8::NewL(dataTo2Str.Length());
		iCommandDataBuffer2->Des().Copy(dataTo2Str);
		}
	else
		{
		iCommandDataBuffer2=HBufC8::NewL(0);
		iCommandDataBuffer2->Des().Copy(KNullDesC8);
		}

	TMMFMessageDestination		handleInfo(iMidiControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	if( iCustomCommandData )
		{
		iMidiClient->CustomCommandAsync(
		messageDest, function, *iCommandDataBuffer1, *iCommandDataBuffer2, iDataFromText, iActiveCallback->iStatus
			);
		}
	else
		{
		iMidiClient->CustomCommandAsync(
			messageDest, function, *iCommandDataBuffer1, *iCommandDataBuffer2, iActiveCallback->iStatus
			);
		}
	iActiveCallback->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}

void CT_MMDataMidiClientUtility::DoCmdCustomCommandSyncL(const TDesC& aSection)
	{


	TInt	function = 0;
	GetIntFromConfig(aSection, KFunctionId(), function);

	iCustomCommandData = EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), iCustomCommandData);

	// reference to the first chunk of data
	delete iCommandDataBuffer1;
	iCommandDataBuffer1=NULL;
	TPtrC	dataToStr;
	if( GetStringFromConfig(aSection, KData1(), dataToStr) )
		{
		iCommandDataBuffer1=HBufC8::NewL(dataToStr.Length());
		iCommandDataBuffer1->Des().Copy(dataToStr);
		}
	else
		{
		iCommandDataBuffer1=HBufC8::NewL(0);
		iCommandDataBuffer1->Des().Copy(KNullDesC8);
		}

	// reference to the second chunk of data
	delete iCommandDataBuffer2;
	iCommandDataBuffer2=NULL;
	if( GetStringFromConfig(aSection, KData2(), dataToStr) )
		{
		iCommandDataBuffer2=HBufC8::NewL(dataToStr.Length());
		iCommandDataBuffer2->Des().Copy(dataToStr);
		}
	else
		{
		iCommandDataBuffer2=HBufC8::NewL(0);
		iCommandDataBuffer2->Des().Copy(KNullDesC8);
		}

	TMMFMessageDestination		handleInfo(iMidiControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);
	TInt						err = KErrNone;
	if( iCustomCommandData )
		{
		TRAP( err,iMidiClient->CustomCommandSyncL(messageDest, function, *iCommandDataBuffer1, *iCommandDataBuffer2, iDataFromText) );
		}
	else
		{
		TRAP( err,iMidiClient->CustomCommandSyncL(messageDest, function, *iCommandDataBuffer1, *iCommandDataBuffer2) );
		}
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CustomCommandSync() failed with error %d."), err);
		SetError(err);
		}
	else if( iCustomCommandData )
		{
		LogDataFromTextL();
		}
	}

void CT_MMDataMidiClientUtility::DoCmdDurationMicroBeatsL(const TDesC& aSection)
	{


	TInt64	durationMicroBeats=0;
	TRAPD( err, durationMicroBeats=iMidiClient->DurationMicroBeatsL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("DurationMicroBeatsL() %d"),durationMicroBeats);

		TInt	tmpDur = 0;
		if( GetIntFromConfig(aSection, KDuration(),tmpDur) )
			{
			TInt64	durationMicroBeatsCfg = (TInt64)tmpDur;
			if( durationMicroBeats!=durationMicroBeatsCfg )
				{
				ERR_PRINTF3(_L("DurationMicroBeatsL() %d != Expected duration %d"),(TInt)durationMicroBeats,(TInt)durationMicroBeatsCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("DurationMicroBeatsL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdDurationMicroSecondsL(const TDesC& aSection)
	{


	TTimeIntervalMicroSeconds	durationMicroSeconds=0;
	TRAPD( err, durationMicroSeconds=iMidiClient->DurationMicroSecondsL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("DurationMicroSecondsL() %d"),durationMicroSeconds.Int64());

		TInt	tmpDur = 0;
		if( GetIntFromConfig(aSection, KDuration(),tmpDur) )
			{
			TInt64	temp64=(TInt64)tmpDur;
			TTimeIntervalMicroSeconds	durationMicroSecondsCfg(temp64);
			if( durationMicroSeconds!=durationMicroSecondsCfg )
				{
				ERR_PRINTF3(_L("DurationMicroSeconds() %d != Expected duration %d"), (TInt)durationMicroSeconds.Int64() ,(TInt)durationMicroSecondsCfg.Int64());
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("DurationMicroSecondsL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdGetBalanceL(const TDesC& aSection)
	{


	TInt	balance = 0;
	TRAPD( err, balance=iMidiClient->GetBalanceL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("GetBalanceL() %d"), balance);

		TInt	balanceCfg = 0;
		if( GetIntFromConfig(aSection, KBalance(), balanceCfg) )
			{
			if( balance!=balanceCfg )
				{
				ERR_PRINTF3(_L("GetBalanceL() %d!= %d Config Balance "),balance,balanceCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("GetBalanceL() error %d"),balance);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdGetBankIdL(const TDesC& aSection)
	{


	TBool	custom = EFalse;
	GetBoolFromConfig(aSection, KCustomBank(), custom);

	TInt	bankId = 0;
	if( GetIntFromConfig(aSection, KBankId(), bankId) )
		{
		TInt	identifier = 0;
		TRAPD( err, identifier=iMidiClient->GetBankIdL(custom, bankId) );
		if( err==KErrNone )
			{
			INFO_PRINTF4(_L("GetBankIdL(%d, %d) = %d"), custom, bankId, identifier);

			TInt	identifierCfg = 0;
			if( GetIntFromConfig(aSection, KIdentifier(), identifierCfg) )
				{
				if( identifier!=identifierCfg )
					{
					ERR_PRINTF3(_L("GetBankIdL() identifier %d!=%d Expected value"),identifier,identifierCfg);
					SetBlockResult(EFail);
					}
				}
			}
		else
			{
			ERR_PRINTF2(_L("GetBankIdL() error %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetBankIdL() error can't read BankId from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdGetDRMCustomCommand(const TDesC& aSection)
	{


	MMMFDRMCustomCommand*	command = iMidiClient->GetDRMCustomCommand();
	if( command )
		{
		INFO_PRINTF1(_L("GetDRMCustomCommand() is not NULL"));
		}
	else
		{
		INFO_PRINTF1(_L("GetDRMCustomCommand() is NULL"));
		}

	TBool	isNull = EFalse;
	if( GetBoolFromConfig(aSection, KIsNull(), isNull) )
		{
		if( ((command!=NULL) && isNull) || ((command==NULL) && !isNull) )
			{
			ERR_PRINTF1(_L("GetDRMCustomCommand() not expected value"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMidiClientUtility::DoCmdGetInstrumentIdL(const TDesC& aSection)
	{


	//Custom
	TBool	custom = EFalse;
	GetBoolFromConfig(aSection, KCustomBank(), custom);

	//BankId
	TInt	bankId = 0;
	if( !GetIntFromConfig(aSection, KBankId(), bankId) )
		{
		ERR_PRINTF1(_L("GetInstrumentIdL() error can't read BankId from Config"));
		SetBlockResult(EFail);
		}

	//InstrumentId
	TInt	instrumentId = 0;
	if( !GetIntFromConfig(aSection, KInstrumentId(), instrumentId) )
		{
		ERR_PRINTF1(_L("GetInstrumentIdL() error can't read InstrumentId from Config"));
		SetBlockResult(EFail);
		}

	TInt	identifier = 0;
	TRAPD( err, identifier=iMidiClient->GetInstrumentIdL(bankId,custom,instrumentId) );
	if( err==KErrNone )
		{
		INFO_PRINTF5(_L("GetInstrumentIdL(%d, %d, %d) = %d"), bankId, custom, instrumentId, identifier);

		TInt	identifierCfg = 0;
		if( GetIntFromConfig(aSection, KIdentifier(), identifierCfg) )
			{
			if( identifier!=identifierCfg )
				{
				ERR_PRINTF3(_L("GetInstrumentIdL() identifier %d!=%d Expected value"),identifier,identifierCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("GetInstrumentIdL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdGetInstrumentL(const TDesC& aSection)
	{


	TInt	channel=0;
	if( GetIntFromConfig(aSection, KChannel(), channel) )
		{
		TInt	instrumentId=0;
		TInt	bankId=0;
		TRAPD( err, iMidiClient->GetInstrumentL(channel,instrumentId,bankId) );

		if( err==KErrNone )
			{
			INFO_PRINTF4(_L("GetInstrumentL() channel %d, instrumentId %d, bankId %d"),channel,instrumentId,bankId);

			TInt	instrumentIdCfg = 0;
			if( GetIntFromConfig(aSection, KInstrumentId(), instrumentIdCfg) )
				{
				if( instrumentId!=instrumentIdCfg )
					{
					ERR_PRINTF3(_L("GetInstrumentL() instrumentId %d!=%d Expected value"),instrumentId,instrumentIdCfg);
					SetBlockResult(EFail);
					}
				}

			TInt	bankIdCfg=0;
			if( GetIntFromConfig(aSection, KBankId(), bankIdCfg) )
				{
				if( bankId!=bankIdCfg )
					{
					ERR_PRINTF3(_L("GetInstrumentL() bankId %d!=%d config value"),bankId,bankIdCfg);
					SetBlockResult(EFail);
					}
				}
			}
		else
			{
			ERR_PRINTF2(_L("GetInstrumentL() error %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetInstrumentL() error can't read channel from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdGetMetaDataEntryL(const TDesC& aSection)
	{

	TInt	metaDataIndex = 0;
	GetIntFromConfig(aSection, KMetaDataIndex(), metaDataIndex);

	CMMFMetaDataEntry*	metaData = NULL;
	TRAPD( err, metaData = iMidiClient->GetMetaDataEntryL(metaDataIndex) );
	CleanupStack::PushL(metaData);
	if( err==KErrNone )
		{
		INFO_PRINTF3(_L("GetMetaDataEntryL() Name = %S, Value = %S"),&(metaData->Name()),&(metaData->Value()));
		}
	else
		{
		ERR_PRINTF2(_L("GetMetaDataEntryL %d"),err);
		SetError( err );
		}
	CleanupStack::PopAndDestroy(metaData);
	}


void CT_MMDataMidiClientUtility::DoCmdGetRepeats(const TDesC& aSection)
	{


	TInt	repeats = iMidiClient->GetRepeats();
	INFO_PRINTF2(_L("GetRepeats() %d"), repeats);

	TInt	repeatsCfg = 0;
	if( GetIntFromConfig(aSection, KRepeats(), repeatsCfg) )
		{
		if( repeats!=repeatsCfg )
			{
			ERR_PRINTF3(_L("GetRepeats() %d != config repeats %d"), repeats, repeatsCfg);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMidiClientUtility::DoCmdInstrumentNameL(const TDesC& aSection)
	{


	TBool	customBank = EFalse;
	GetBoolFromConfig(aSection, KCustomBank(), customBank);

	TInt	bankIndex = 0;
	if( !GetIntFromConfig(aSection, KBankId(), bankIndex) )
		{
		ERR_PRINTF1(_L("InstrumentName() error can't read bankId from Config"));
		SetBlockResult(EFail);
		}

	TInt	instrumentIndex = 0;
	if( !GetIntFromConfig(aSection, KInstrumentId(), instrumentIndex) )
		{
		ERR_PRINTF1(_L("InstrumentName() error can't read instrumentId from Config"));
		SetBlockResult(EFail);
		}

	HBufC*	name=NULL;
	TRAPD( err, name = iMidiClient->InstrumentNameL(bankIndex,customBank,instrumentIndex) );
	CleanupStack::PushL(name);
	if( err==KErrNone )
		{
		INFO_PRINTF5(_L("InstrumentName(%d, %d, %d) = %S"), bankIndex, customBank, instrumentIndex, name);

		TPtrC	nameCfg;
		if( GetStringFromConfig(aSection, KName(), nameCfg) )
			{
			if( name->Des()!=nameCfg )
				{
				ERR_PRINTF3(_L("InstrumentName() %S != Expected value %S"), name, &nameCfg);
				SetError( err );
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("InstrumentName %d"),err);
		SetError( err );
		}
	CleanupStack::PopAndDestroy(name);
	}

void CT_MMDataMidiClientUtility::DoCmdIsChannelMuteL(const TDesC& aSection)
	{


	TInt	channel = 0;
	GetIntFromConfig(aSection, KChannel(), channel);

	TBool	muted = EFalse;
	TRAPD( err, muted=iMidiClient->IsChannelMuteL(channel) );
	if( err==KErrNone )
		{
		INFO_PRINTF3(_L("IsChannelMuteL(%d) = %d"), channel, muted);

		TBool	muteCfg = EFalse;
		if( GetBoolFromConfig(aSection, KMute(), muteCfg) )
			{
			if( muteCfg!=muted )
				{
				ERR_PRINTF2(_L("IsChannelMuteL(%d) not expected value"), channel);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("IsChannelMuteL error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdIsTrackMuteL(const TDesC& aSection)
	{


	TInt	track = 0;
	if( GetIntFromConfig(aSection, KTrack(), track) )
		{
		TBool	muted = EFalse;
		TRAPD( err, muted=iMidiClient->IsTrackMuteL(track) );
		if( err==KErrNone )
			{
			INFO_PRINTF3(_L("IsTrackMuteL(%d) = %d"), track, muted);

			TBool	muteCfg = EFalse;
			if( GetBoolFromConfig(aSection, KMute(), muteCfg) )
				{
				if( muteCfg!=muted )
					{
					ERR_PRINTF2(_L("IsTrackMuteL() track %d not expected value"),track);
					SetBlockResult(EFail);
					}
				}
			}
		else
			{
			ERR_PRINTF2(_L("IsTrackMuteL error %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("IsTrackMuteL error can't read int from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdLoadCustomBankDataL(const TDesC& aSection)
	{


	TPtrC	filename;
	if( GetStringFromConfig(aSection, KBankData(), filename) )
		{
		RFile	file;
		User::LeaveIfError(file.Open(iFs, filename, EFileRead | EFileShareAny));
		CleanupClosePushL(file);
		TInt	fileSize = 0;

		User::LeaveIfError(file.Size(fileSize));

		HBufC8 *buffer = HBufC8::NewLC(fileSize);
		TPtr8	bufferPtr(buffer->Des());
		User::LeaveIfError(file.Read(bufferPtr));

		TRAPD( err, iMidiClient->LoadCustomBankDataL(bufferPtr,iCustomBankId) );

		if( err==KErrNone )
			{
			INFO_PRINTF3(_L("LoadCustomBankDataL %S bankColectId %d"),&filename,iCustomBankId);
			}
		else
			{
			ERR_PRINTF2(_L("LoadCustomBankDataL() error %d"), err);
			SetError( err );
			}
		CleanupStack::PopAndDestroy(2, &file); // buffer, file
		}
	else
		{
		ERR_PRINTF1(_L("LoadCustomBankDataL() error can't read path to BankData"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdLoadCustomBankL(const TDesC& aSection)
	{


	TPtrC	filename;
	if( GetStringFromConfig(aSection, KBankData(), filename) )
		{
		TRAPD( err, iMidiClient->LoadCustomBankL(filename,iCustomBankId) );
		if( err == KErrNone )
			{
	   		INFO_PRINTF3(_L("LoadCustomBank %S bankColectId %d"),&filename,iCustomBankId);
			}
		else
			{
			ERR_PRINTF2(_L("LoadCustomBankL() error %d"), err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("LoadCustomBankL() error can't read filename"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdLoadCustomInstrumentDataL(const TDesC& aSection)
	{


	//fileBankId
	TInt	fileBankId = 0;
	if( !GetIntFromConfig(aSection, KFileBankId(), fileBankId) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentDataL() error can't read fileBankId"));
		SetBlockResult(EFail);
		}
	//fileInstrumentId
	TInt	fileInstrumentId = 0;
	if( !GetIntFromConfig(aSection, KFileInstrumentId(), fileInstrumentId) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentDataL() error can't read fileInstrumentId"));
		SetBlockResult(EFail);
		}
	//memoryBankId
	TInt	memoryBankId = 0;
	if( !GetIntFromConfig(aSection, KMemoryBankId(), memoryBankId) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentDataL() error can't read memoryBankId"));
		SetBlockResult(EFail);
		}
	//memoryInstrumentId
	TInt	memoryInstrumentId = 0;
	if( !GetIntFromConfig(aSection, KMemoryInstrumentId(), memoryInstrumentId) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentDataL() error can't read memoryInstrumentId"));
		SetBlockResult(EFail);
		}
	//FileName
	TPtrC	filename;
	if( !GetStringFromConfig(aSection, KFileName(), filename) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentDataL() error can't read fileName"));
		SetBlockResult(EFail);
		}
	else
		{
		RFile	file;
		User::LeaveIfError(file.Open(iFs, filename, EFileRead | EFileShareAny));
		CleanupClosePushL(file);

		TInt	fileSize = 0;
		User::LeaveIfError(file.Size(fileSize));

		HBufC8*	buffer = HBufC8::NewLC(fileSize);
		TPtr8	bufferPtr(buffer->Des());
		User::LeaveIfError(file.Read(bufferPtr));

		TRAPD( err, iMidiClient->LoadCustomInstrumentDataL(bufferPtr,fileBankId,fileInstrumentId,memoryBankId,memoryInstrumentId) );

		if( err == KErrNone )
			{
			INFO_PRINTF6(_L("LoadCustomInstrumentDataL() %S fileBankId %d, fileInstrumentId %d, memoryBankId %d, memoryInstrumentId %d")
				,&filename,fileBankId,fileInstrumentId,memoryBankId,memoryInstrumentId);
			}
		else
			{
			ERR_PRINTF2(_L("LoadCustomInstrumentDataL() error %d"), err);
			SetError( err );
			}
		CleanupStack::PopAndDestroy(2, &file); // buffer, file
		}
	}

void CT_MMDataMidiClientUtility::DoCmdLoadCustomInstrumentL(const TDesC& aSection)
	{


	//fileBankId
	TInt	fileBankId = 0;
	if( !GetIntFromConfig(aSection, KFileBankId(), fileBankId) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentL() error can't read fileBankId"));
		SetBlockResult(EFail);
		}
	//fileInstrumentId
	TInt	fileInstrumentId = 0;
	if( !GetIntFromConfig(aSection, KFileInstrumentId(), fileInstrumentId) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentL() error can't read fileInstrumentId"));
		SetBlockResult(EFail);
		}
	//memoryBankId
	TInt	memoryBankId = 0;
	if( !GetIntFromConfig(aSection, KMemoryBankId(), memoryBankId) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentL() error can't read memoryBankId"));
		SetBlockResult(EFail);
		}
	//memoryInstrumentId
	TInt	memoryInstrumentId = 0;
	if( !GetIntFromConfig(aSection, KMemoryInstrumentId(), memoryInstrumentId) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentL() error can't read memoryInstrumentId"));
		SetBlockResult(EFail);
		}
	//FileName
	TPtrC	filename;
	if( !GetStringFromConfig(aSection, KFileName(), filename) )
		{
		ERR_PRINTF1(_L("LoadCustomInstrumentL() error can't read fileName"));
		SetBlockResult(EFail);
		}
	else
		{
		TRAPD( err, iMidiClient->LoadCustomInstrumentL(filename,fileBankId,fileInstrumentId,memoryBankId,memoryInstrumentId) );

		if( err == KErrNone )
			{
			INFO_PRINTF6(_L("LoadCustomInstrumentL() %S fileBankId %d, fileInstrumentId %d, memoryBankId %d, memoryInstrumentId %d")
				,&filename,fileBankId,fileInstrumentId,memoryBankId,memoryInstrumentId);
			}
		else
			{
			ERR_PRINTF2(_L("LoadCustomInstrumentL() error %d"), err);
			SetError( err );
			}
		}
	}

void CT_MMDataMidiClientUtility::DoCmdMaxChannelVolumeL(const TDesC& aSection)
	{


	TRAPD( err, iMaxChannelVolume=iMidiClient->MaxChannelVolumeL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("MaxChannelVolumeL() %g"), iMaxChannelVolume);

		TInt	volumeTmp = 0;
		if( GetIntFromConfig(aSection, KVolume(), volumeTmp) )
			{
			INFO_PRINTF2(_L("MaxChannelVolumeL() %S"), &aSection);
			TReal32	volumeCfg = (TReal32)volumeTmp;
			if( iMaxChannelVolume!=volumeCfg )
				{
				ERR_PRINTF3(_L("MaxChannelVolumeL() %g != Expected value %g"),iMaxChannelVolume, volumeCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("MaxChannelVolumeL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdMaxPlaybackRateL(const TDesC& aSection)
	{


	TRAPD( err, iPlaybackRate = iMidiClient->MaxPlaybackRateL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("MaxPlaybackRateL() %d"),iPlaybackRate);

		TInt	rateCfg = 0;
		if( GetIntFromConfig(aSection, KRate(), rateCfg) )
			{
			if( iPlaybackRate!=rateCfg )
				{
				ERR_PRINTF3(_L("MaxPlaybackRateL() %d != Expected value %d"),iPlaybackRate,rateCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("MaxPlaybackRateL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdMaxPolyphonyL(const TDesC& aSection)
	{


	TRAPD( err, iMaxPolyphony = iMidiClient->MaxPolyphonyL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("MaxPolyphonyL() %d"),iMaxPolyphony);

		TInt	polyphonyCfg = 0;
		if( GetIntFromConfig(aSection, KMaxNotes(), polyphonyCfg) )
			{
			if( iMaxPolyphony!=polyphonyCfg )
				{
				ERR_PRINTF3(_L("MaxPolyphonyL() %d != Expected value %d"),iMaxPolyphony,polyphonyCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("MaxPolyphonyL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdMaxVolumeL(const TDesC& aSection)
	{


	TRAPD( err, iMaxVolume = iMidiClient->MaxVolumeL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("MaxVolumeL() %d"), iMaxVolume);

		TInt	volumeCfg=0;
		if( GetIntFromConfig(aSection, KVolume(), volumeCfg) )
			{
			if( iMaxVolume!=volumeCfg )
				{
				ERR_PRINTF3(_L("MaxVolumeL() %d != Expected value %d"),iMaxVolume,volumeCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("MaxVolumeL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdMimeTypeL(const TDesC& aSection)
	{


	const TDesC8*	des = NULL;
	TRAPD( err, des = &(iMidiClient->MimeTypeL()) );
	if( err==KErrNone )
		{
		TBuf<KMimeTypeBufferSize>	buff;
		buff.Copy(*des);
		TPtrC	mimeTypePtr(buff);
		INFO_PRINTF2(_L("MimeTypeL(), %S"),&mimeTypePtr);

		TPtrC	mimeTypeCfg;
		if( GetStringFromConfig(aSection, KMimeType(), mimeTypeCfg) )
			{
			if( mimeTypePtr!=mimeTypeCfg )
				{
				ERR_PRINTF3(_L("MimeTypeL() %S != Expected value %S"),&mimeTypePtr,&mimeTypeCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("MimeTypeL() %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdMinPlaybackRateL(const TDesC& aSection)
	{


	TRAPD( err, iPlaybackRate = iMidiClient->MinPlaybackRateL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("MinPlaybackRateL() %d"),iPlaybackRate);

		TInt	rateCfg = 0;
		if( GetIntFromConfig(aSection, KRate(), rateCfg) )
			{
			if( iPlaybackRate!=rateCfg )
				{
				ERR_PRINTF3(_L("MinPlaybackRateL() %d != Expected value %d"),iPlaybackRate,rateCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("MinPlaybackRateL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdNewL(const TDesC& aSection)
	{
	DestroyData();

	TInt	priority;
	if( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityNormal;
		}

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TRAPD(err, iMidiClient=CMidiClientUtility::NewL(*this, priority, priorityPreference));

	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMidiClientUtility::NewL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdNoteOffL(const TDesC& aSection)
	{


	TInt	channel = 0;
	if( !GetIntFromConfig(aSection, KChannel(), channel) )
		{
		ERR_PRINTF1(_L("NoteOffL error can't read channel from Config"));
		SetBlockResult(EFail);
		}

	TInt	note = 0;
	if( !GetIntFromConfig(aSection, KNote(), note) )
		{
		ERR_PRINTF1(_L("NoteOffL error can't read note from Config"));
		SetBlockResult(EFail);
		}

	TInt	velocity = 0;
	if( !GetIntFromConfig(aSection, KVelocity(), velocity) )
		{
		ERR_PRINTF1(_L("NoteOffL error can't read velocity from Config"));
		SetBlockResult(EFail);
		}

	TRAPD( err, iMidiClient->NoteOffL(channel,note,velocity) );
	if( err==KErrNone )
		{
		INFO_PRINTF4(_L("NoteOffL() channel %d, note %d, velocity %d"),channel,note,velocity);
		}
	else
		{
		ERR_PRINTF2(_L("NoteOffL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdNoteOnL(const TDesC& aSection)
	{

	TInt	channel = 0;
	if( !GetIntFromConfig(aSection, KChannel(), channel) )
		{
		ERR_PRINTF1(_L("NoteOnL error can't read channel from Config"));
		SetBlockResult(EFail);
		}

	TInt	note = 0;
	if( !GetIntFromConfig(aSection, KNote(), note) )
		{
		ERR_PRINTF1(_L("NoteOnL error can't read note from Config"));
		SetBlockResult(EFail);
		}

	TInt	velocity = 0;
	if( !GetIntFromConfig(aSection, KVelocity(), velocity) )
		{
		ERR_PRINTF1(_L("NoteOnL error can't read velocity from Config"));
		SetBlockResult(EFail);
		}

	TRAPD( err, iMidiClient->NoteOnL(channel,note,velocity) );
	if( err==KErrNone )
		{
		INFO_PRINTF4(_L("NoteOnL() channel %d, note %d, velocity %d"),channel,note,velocity);
		}
	else
		{
		ERR_PRINTF2(_L("NoteOnL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdNumTracksL(const TDesC& aSection)
	{


	TInt	numTracks=0;
	TRAPD( err, numTracks=iMidiClient->NumTracksL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("NumTracksL() %d"),numTracks);

		TInt	numTracksCfg = 0;
		if( GetIntFromConfig(aSection, KNumTracks(), numTracksCfg) )
			{
			if( numTracks!=numTracksCfg )
				{
				ERR_PRINTF3(_L("NumTracksL() %d != Expected Value %d"),numTracks,numTracksCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("NumTracksL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdNumberOfBanksL(const TDesC& aSection)
	{


	TBool	custom = EFalse;
	GetBoolFromConfig(aSection, KCustomBank(), custom);

	TInt	banks = 0;
	TRAPD( err, banks=iMidiClient->NumberOfBanksL(custom) );
	if( err==KErrNone )
		{
		if( custom )
			{
			INFO_PRINTF2(_L("NumberOfBanksL() custom banks  %d"),banks);
			}
		else
			{
			INFO_PRINTF2(_L("NumberOfBanksL() standard banks %d"),banks);
			}

		TInt	banksCfg = 0;
		if( GetBoolFromConfig(aSection, KBanks(), banksCfg) )
			{
			if( banks!=banksCfg )
				{
				ERR_PRINTF3(_L("NumberOfBanksL() %d != Expected Value %d"),banks,banksCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("NumberOfBanksL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdNumberOfInstrumentsL(const TDesC& aSection)
	{


	TBool	custom = EFalse;
	GetBoolFromConfig(aSection, KCustomBank(), custom);

	TInt	bankId = 0;
	if( GetIntFromConfig(aSection, KBankId(), bankId) )
		{
		TInt	instruments = 0;
		TRAPD( err, instruments=iMidiClient->NumberOfInstrumentsL(bankId,custom) );
		if( err==KErrNone )
			{
			INFO_PRINTF4(_L("NumberOfInstrumentsL(%d, %d) = %d"), bankId, custom, instruments);

			TInt	instrumentsCfg = 0;
			if( GetIntFromConfig(aSection, KInstruments(), instrumentsCfg) )
				{
				if( instruments!=instrumentsCfg )
					{
					ERR_PRINTF3(_L("NumberOfInstrumentsL() %d != Expected Value %d"),instruments,instrumentsCfg);
					SetBlockResult(EFail);
					}
				}
			}
		else
			{
			ERR_PRINTF2(_L("NumberOfInstrumentsL() error %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("NumberOfInstrumentsL() error can't read BankId from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdNumberOfMetaDataEntriesL(const TDesC& aSection)
	{


	TInt	num = 0;
	TRAPD( err, num=iMidiClient->NumberOfMetaDataEntriesL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("NumberOfMetaDataEntriesL() %d"),num);

		TInt	numCfg = 0;
		if( GetIntFromConfig(aSection, KMetaDataNum(), numCfg) )
			{
			if( num!=numCfg )
				{
				ERR_PRINTF3(_L("NumberOfMetaDataEntriesL() %d != Expected Value %d"),num,numCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("NumberOfMetaDataEntriesL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdOpenDesL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	iApiAsyncErrorIndex = aAsyncErrorIndex;
	TPtrC	filename;
	if( GetStringFromConfig(aSection, KFileName(), filename) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, filename, KUidMediaTypeMidi, ETrue, iMidiControllerUid, iMidiFormatUid);
		if( filename==KNullString )
			{
			iMidiClient->OpenDes(KNullDesC8);
			INFO_PRINTF1(_L("OpenDes() KNullDesC8"));
			}
		else
			{
			RFile file;
			User::LeaveIfError(file.Open(iFs, filename, EFileRead | EFileShareAny));
			CleanupClosePushL(file);
			TInt fileSize = 0;
			User::LeaveIfError(file.Size(fileSize));

			delete iBuffer;
			iBuffer=NULL;
			iBuffer=HBufC8::NewL(fileSize);
			*iBuffer=KNullDesC8;
			delete iBufferPtr;
			iBufferPtr=new (ELeave) TPtr8(iBuffer->Des());

			User::LeaveIfError(file.Read(*iBufferPtr));

			iMidiClient->OpenDes(*iBufferPtr);
			INFO_PRINTF2(_L("OpenDes() %S"),&filename);
			CleanupStack::PopAndDestroy(&file);
			}
		IncOutstanding();
		}
	else
		{
		ERR_PRINTF1(_L("OpenDes() error"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdOpenFileL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;
	TPtrC	filename;
	if( GetStringFromConfig(aSection, KFileName(), filename) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, filename, KUidMediaTypeMidi, ETrue, iMidiControllerUid, iMidiFormatUid);

		TBool useTMMSource = EFalse;
		GetBoolFromConfig(aSection, KTMMSource(), useTMMSource);
		TBool useRFile = EFalse;
		GetBoolFromConfig(aSection, KRFile(), useRFile);

		if( useRFile )
			{
			if( iFileOpened )
				{
				iFile.Close();
				iFileOpened = EFalse;
				}
			User::LeaveIfError(iFile.Open(iFs, filename, EFileRead | EFileShareAny));
			iFileOpened=ETrue;

			iMidiClient->OpenFile(iFile);
			INFO_PRINTF1(_L("OpenFile(const RFile&)"));
			IncOutstanding();
			}
		else if( useTMMSource )
			{
		    ContentAccess::TIntent	intent=ContentAccess::EPeek;
			if ( !CT_MMUtil::ReadIntent(*this, aSection, intent) )
				{
				intent=ContentAccess::EPeek;
				}

			TPtrC	uniqueId = _L("");
			GetStringFromConfig(aSection, KUniqueId(), uniqueId);

			TBool	enableUI = EFalse;
			GetBoolFromConfig(aSection, KEnableUI(), enableUI);

			TMMFileSource fileSource(filename, uniqueId, intent, enableUI);
			iMidiClient->OpenFile(fileSource);
			INFO_PRINTF1(_L("OpenFile(const TMMSource&)"));

			IncOutstanding();
			}
		else
			{
			iMidiClient->OpenFile(filename);
			INFO_PRINTF1(_L("OpenFile(const TDesC&)"));
			IncOutstanding();
			}
		}
	else
		{
		ERR_PRINTF2(_L("Error read parameter = %S"), &KFileName());
		ERR_PRINTF2(_L("Error read parameter = %S"), &filename);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdOpenUrlL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	iApiAsyncErrorIndex = aAsyncErrorIndex;
	TPtrC	url;
	if( GetStringFromConfig(aSection, KUrl(), url) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, url, KUidMediaTypeMidi, ETrue, iMidiControllerUid, iMidiFormatUid);

		TInt	accessPointID = KUseDefaultIap;
		GetIntFromConfig(aSection, KAccessPointID(), accessPointID);
		HBufC8*	mimeType8=NULL;
		TPtrC	mimeType;
		if( GetStringFromConfig(aSection, KMimeType(), mimeType) )
			{
			mimeType8=HBufC8::NewL(mimeType.Length());
			mimeType8->Des().Copy(mimeType);
			}
		else
			{
			mimeType8=HBufC8::NewL(0);
			mimeType8->Des().Copy(KNullDesC8);
			}
		CleanupStack::PushL(mimeType8);

		iMidiClient->OpenUrl(url, accessPointID, *mimeType8);

		IncOutstanding();

		//call back in MapcInitComplete

		CleanupStack::PopAndDestroy(mimeType8);
		}
	else
		{
		ERR_PRINTF2(_L("Error read parameter =%S"), &KMimeType());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdPercussionKeyNameL(const TDesC& aSection)
	{


	//Custom
	TBool	custom = EFalse;
	GetBoolFromConfig(aSection, KCustomBank(), custom);
	//Note
	TInt	note = 0;
	if( !GetIntFromConfig(aSection, KNote(), note) )
		{
		ERR_PRINTF1(_L("PercussionKeyNameL() error can't read note from Config"));
		SetBlockResult(EFail);
		}
	//BankId
	TInt	bankId = 0;
	if( !GetIntFromConfig(aSection, KBankId(), bankId) )
		{
		ERR_PRINTF1(_L("PercussionKeyNameL() error can't read BankId from Config"));
		SetBlockResult(EFail);
		}
	//InstrumentId
	TInt	instrumentId = 0;
	if( !GetIntFromConfig(aSection, KInstrumentId(), instrumentId) )
		{
		ERR_PRINTF1(_L("PercussionKeyNameL() error can't read InstrumentId from Config"));
		SetBlockResult(EFail);
		}

	HBufC*	keyName = NULL;
	TRAPD( err, keyName=iMidiClient->PercussionKeyNameL(note,bankId,custom,instrumentId) );
	CleanupStack::PushL(keyName);
	if( err==KErrNone )
		{
		TPtrC	str = keyName->Des();
		INFO_PRINTF6(_L("PercussionKeyNameL(%d, %d, %d, %d) = %S"), note, bankId, custom, instrumentId, &str);
		}
	else
		{
		ERR_PRINTF2(_L("PercussionKeyNameL() error %d"),err);
		SetError( err );
		}
	CleanupStack::PopAndDestroy(keyName);
	}

void CT_MMDataMidiClientUtility::DoCmdPitchTranspositionCentsL(const TDesC& aSection)
	{


	TInt	cents = 0;
	TRAPD( err, cents = iMidiClient->PitchTranspositionCentsL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("PitchTranspositionCentsL() %d"),cents);
		TInt	centsCfg = 0;
		if( GetIntFromConfig(aSection, KCents(), centsCfg) )
			{
			if( cents!=centsCfg )
				{
				ERR_PRINTF3(_L("PitchTranspositionCentsL() %d != config cents %d"), cents, centsCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("PitchTranspositionCentsL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdPlay(const TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;
	iMidiClient->Play();
	INFO_PRINTF1(_L("Play() playing"));
	IncOutstanding();
	}

void CT_MMDataMidiClientUtility::DoCmdPlayNoteL(const TDesC& aSection)
	{


	//Channel
	TInt	channel;
	if( !GetIntFromConfig(aSection, KChannel(), channel) )
		{
		ERR_PRINTF1(_L("PlayNoteL error can't read channel from Config"));
		SetBlockResult(EFail);
		}
	//Note
	TInt	note;
	if( !GetIntFromConfig(aSection, KNote(), note) )
		{
		ERR_PRINTF1(_L("PlayNoteL error can't read note from Config"));
		SetBlockResult(EFail);
		}
	//Duration
	TInt	tmp=0;
	if( !GetIntFromConfig(aSection, KDuration(), tmp) )
		{
		ERR_PRINTF1(_L("PlayNoteL error can't read duration from Config"));
		SetBlockResult(EFail);
		}
	TTimeIntervalMicroSeconds duration = tmp;
	//NoteOnVelocity
	TInt	noteOnVelocity;
	if( !GetIntFromConfig(aSection, KNoteOnVelocity(), noteOnVelocity) )
		{
		ERR_PRINTF1(_L("PlayNoteL error can't read noteOnVelocity from Config"));
		SetBlockResult(EFail);
		}
	//NoteOffVelocity
	TInt	noteOffVelocity;
	if( !GetIntFromConfig(aSection, KNoteOffVelocity(), noteOffVelocity) )
		{
		ERR_PRINTF1(_L("PlayNoteL error can't read noteOffVelocity  from Config"));
		SetBlockResult(EFail);
		}
	//startTime
	if( GetIntFromConfig(aSection, KStartTime(), tmp) )
		{
		TTimeIntervalMicroSeconds startTime = tmp;
		//PlayNoteL(6 parameters)
		TRAPD( err, iMidiClient->PlayNoteL(channel,note,startTime,duration,noteOnVelocity,noteOffVelocity) );
		if( err==KErrNone )
			{
			INFO_PRINTF7(_L("PlayNoteL() channel %d, note %d, startTime %d, duration %d, noteOnVelocity %d, noteOffVelocity %d"),
			channel,note,TInt(startTime.Int64()),TInt(duration.Int64()),noteOnVelocity,noteOffVelocity);
			}
		else
			{
			ERR_PRINTF2(_L("PlayNoteL() %d"),err);
			SetError( err );
			}
		}
	else
		{
		//PlayNoteL(5 parameters)
		TRAPD( err, iMidiClient->PlayNoteL(channel,note,duration,noteOnVelocity,noteOffVelocity) );
		if( err==KErrNone )
			{
			INFO_PRINTF6(_L("PlayNoteL() channel %d, note %d, duration %d, noteOnVelocity %d, noteOffVelocity %d"),
				channel,note,TInt(duration.Int64()),noteOnVelocity,noteOffVelocity);
			}
		else
			{
			ERR_PRINTF2(_L("PlayNoteL() %d"),err);
			SetError( err );
			}
		}
	}

void CT_MMDataMidiClientUtility::DoCmdPlaybackRateL(const TDesC& aSection)
	{


	TInt	rate = 0;
	TRAPD( err, rate = iMidiClient->PlaybackRateL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("PlaybackRateL() %d"),rate);
		TInt	rateCfg = iPlaybackRate;
		if( (GetIntFromConfig(aSection, KRate(), rateCfg)) || (iPlaybackRate > 0) )
			{
			if( rate!=rateCfg )
				{
				ERR_PRINTF3(_L("PlaybackRateL() %d != config rate %d"), rate, rateCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("PlaybackRateL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdPolyphonyL(const TDesC& aSection)
	{


	TInt	maxNotes = 0;
	TRAPD( err, maxNotes = iMidiClient->PolyphonyL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("PolyphonyL() %d"),maxNotes);
		TInt	maxNotesCfg = 0;
		if( GetIntFromConfig(aSection, KMaxNotes(), maxNotesCfg) )
			{
			if( maxNotes != maxNotesCfg )
				{
				ERR_PRINTF3(_L("PolyphonyL() %d != config value %d"),maxNotes,maxNotesCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("PolyphonyL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdPositionMicroBeatsL(const TDesC& aSection)
	{


	TInt64	position = 0;
	TRAPD( err, position = iMidiClient->PositionMicroBeatsL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("PositionMicroBeatsL() %d"),TInt(position));
		TInt	tmp = 0;
		if( GetIntFromConfig(aSection, KPosition(), tmp) )
			{
			TInt64	positionCfg = (TInt64)tmp;
			if( position!=positionCfg )
				{
				ERR_PRINTF3(_L("PositionMicroBeatsL() %d != config value %d"),TInt(position),TInt(positionCfg));
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("PositionMicroBeatsL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdPositionMicroSecondsL(const TDesC& aSection)
	{


	TTimeIntervalMicroSeconds	position = 0;
	TRAPD( err, position = iMidiClient->PositionMicroSecondsL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("PositionMicroSecondsL() %d"),(TInt)position.Int64());
		TInt	tmp = 0;
		if( GetIntFromConfig(aSection, KPosition(), tmp) )
			{
			TTimeIntervalMicroSeconds	positionCfg = tmp;
			if( position!=positionCfg )
				{
				ERR_PRINTF3(_L("PositionMicroSecondsL() %d != config value %d"),(TInt)position.Int64(),(TInt)positionCfg.Int64());
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("PositionMicroSecondsL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSendMessageL(const TDesC& aSection)
	{


	// reference to the first chunk of data
	HBufC8*	message = NULL;
	TPtrC	messageStr;
	if( GetStringFromConfig(aSection, KData1(), messageStr) )
		{
		message = HBufC8::NewL(messageStr.Length());
		message->Des().Copy(messageStr);
		}
	else
		{
		message = HBufC8::NewL(0);
		message->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(message);

	TInt	byteProcessed = 0;
	TInt	error = KErrNone;
	TInt	tmp = 0;
	if( GetIntFromConfig(aSection, KTime(), tmp) )
		{
		TTimeIntervalMicroSeconds time = tmp;
		TRAP( error, byteProcessed = iMidiClient->SendMessageL(*message,time) );
		}
	else
		{
		TRAP( error, byteProcessed = iMidiClient->SendMessageL(*message) );
		}
	if( error==KErrNone )
		{
		INFO_PRINTF2(_L("SendMessageL() bytes processed: %d"),byteProcessed);
		}
	else
		{
		ERR_PRINTF2(_L("SendMessageL() %d"),error);
		SetError( error );
		}

	CleanupStack::PopAndDestroy(message);
	}

void CT_MMDataMidiClientUtility::DoCmdSendMipMessageL(const TDesC& aSection)
	{
	TInt						index = 0;
	TBool						moreData = ETrue;
	RArray<TMipMessageEntry>	mipArray;
	CleanupClosePushL(mipArray);

	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	while ( moreData )
		{
		moreData = EFalse;

		TInt	mipValue = 0;
		tempStore.Format(KMIPValue(), ++index);
		if( GetIntFromConfig(aSection, tempStore, mipValue) )
			{
			TInt	channel = 0;
			tempStore.Format(KOption(), index);
			if ( GetIntFromConfig(aSection, tempStore, channel) )
				{
				TMipMessageEntry	message;
				message.iChannel =	channel;
				message.iMIPValue =	mipValue;

				TInt	err = mipArray.Append(message);
				if( err!=KErrNone )
					{
					ERR_PRINTF2(_L("SendMipMessageL() arrayCreating error %d"),err);
					SetBlockResult(EFail);
					}

				moreData = ETrue;
				}
			}
		}

	TRAPD( err, iMidiClient->SendMipMessageL(mipArray) );
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SendMipMessageL() error %d"),err);
		SetError( err );
		}

	CleanupStack::PopAndDestroy(&mipArray);
	}

void CT_MMDataMidiClientUtility::DoCmdSetBalanceL(const TDesC& aSection)
	{


	TInt	balance = KMMFBalanceCenter;
	GetIntFromConfig(aSection, KBalance(), balance);

	TRAPD( err, iMidiClient->SetBalanceL(balance) );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("SetBalanceL() %d"),balance);
		}
	else
		{
		ERR_PRINTF2(_L("SetBalanceL() error %d"),balance);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetBankL(const TDesC& aSection)
	{


	TBool	custom = EFalse;
	GetBoolFromConfig(aSection, KCustom(), custom);

	TRAPD( err, iMidiClient->SetBankL(custom) );
	if( err==KErrNone )
		{
		INFO_PRINTF1(custom?_L("SetBankL() True"):_L("SetBankL() False"));
		}
	else
		{
		ERR_PRINTF2(_L("SetBankL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetChannelMuteL(const TDesC& aSection)
	{


	TInt	channel = 0;
	GetIntFromConfig(aSection, KChannel(), channel);

	TBool	mute = EFalse;
	GetBoolFromConfig(aSection, KMute(), mute);

	TRAPD( err, iMidiClient->SetChannelMuteL(channel,mute) );
	if( err==KErrNone )
		{
		INFO_PRINTF3(_L("SetChannelMuteL(%d, %d)"), channel, mute);
		}
	else
		{
		ERR_PRINTF2(_L("SetChannelMuteLerror %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetChannelVolumeL(const TDesC& aSection)
	{


	TInt	channel = 0;
	if( !GetIntFromConfig(aSection, KChannel(), channel) )
		{
		ERR_PRINTF1(_L("SetChannelVolumeL error can't read channel from Config"));
		SetBlockResult(EFail);
		}

	TReal32	volume = iMaxChannelVolume;
	TInt	tmpVol = 0;
	if( GetIntFromConfig(aSection, KVolume(), tmpVol) )
		{
		volume = tmpVol;
		}

	TRAPD( err, iMidiClient->SetChannelVolumeL(channel,volume) );
	if( err==KErrNone )
		{
		INFO_PRINTF3(_L("SetChannelVolumeL() %d, Volume %g"), channel ,volume);
		}
	else
		{
		ERR_PRINTF2(_L("SetChannelVolumeL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetInstrumentL(const TDesC& aSection)
	{


	TInt	bankIndex = 0;
	if( !GetIntFromConfig(aSection, KBankId(), bankIndex) )
		{
		ERR_PRINTF1(_L("SetInstrumentL() error can't read bankId from Config"));
		SetBlockResult(EFail);
		}

	TInt	instrumentIndex = 0;
	if( !GetIntFromConfig(aSection, KInstrumentId(), instrumentIndex) )
		{
		ERR_PRINTF1(_L("SetInstrumentL() error can't read instrumentId from Config"));
		SetBlockResult(EFail);
		}

	TInt	channel = 0;
	if( !GetIntFromConfig(aSection, KChannel(), channel) )
		{
		ERR_PRINTF1(_L("SetInstrumentL() error can't read channel from Config"));
		SetBlockResult(EFail);
		}

	TRAPD( err, iMidiClient->SetInstrumentL(channel,bankIndex,instrumentIndex) );
	if( err==KErrNone )
		{
		INFO_PRINTF4(_L("SetInstrumentL() %d, channel %d, bankdId %d"),instrumentIndex,channel,bankIndex);
		}
	else
		{
		ERR_PRINTF2(_L("SetInstrumentL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetMaxPolyphonyL(const TDesC& aSection)
	{


	TInt	maxNotes = iMaxPolyphony;
	GetIntFromConfig(aSection, KMaxNotes(), maxNotes);

	TRAPD( err, iMidiClient->SetMaxPolyphonyL(maxNotes) );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("SetMaxPolyphonyL() %d"),maxNotes);
		}
	else
		{
		ERR_PRINTF2(_L("SetMaxPolyphonyL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetPitchTranspositionL(const TDesC& aSection)
	{


	TInt	cents = 0;
	if( GetIntFromConfig(aSection, KCents(), cents) )
		{
		TInt	centsActual = 0;
		TRAPD( err, centsActual = iMidiClient->SetPitchTranspositionL(cents) );
		if( err==KErrNone )
			{
			INFO_PRINTF3(_L("SetPitchTranspositionL() %d actual pitch shift applied %d "),cents,centsActual);

			TInt	expectedShift = 0;
			if( GetIntFromConfig(aSection, KShift(), expectedShift) )
				{
				if( centsActual != expectedShift )
					{
					ERR_PRINTF3(_L("Pitch shift actual(%d)!=expected(%d)"), centsActual, expectedShift);
					SetBlockResult(EFail);
					}
				}
			}
		else
			{
			ERR_PRINTF2(_L("SetPitchTranspositionL %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("SetPitchTranspositionL error can't read cents from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetPlaybackRateL(const TDesC& aSection)
	{


	TInt	rate = iPlaybackRate;
	GetIntFromConfig(aSection, KRate(), rate);

	TRAPD( err, iMidiClient->SetPlaybackRateL(rate) );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("SetPlaybackRateL() %d"),rate);
		}
	else
		{
		ERR_PRINTF2(_L("SetPlaybackRateL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetPositionMicroBeatsL(const TDesC& aSection)
	{


	TInt	tmp = 0;
	if( GetIntFromConfig(aSection, KPosition(), tmp) )
		{
		TInt64	position = (TInt64)tmp;
		TRAPD( err, iMidiClient->SetPositionMicroBeatsL(position) );
		if( err==KErrNone )
			{
			INFO_PRINTF2(_L("SetPositionMicroBeatsL() %d"),TInt(position));
			}
		else
			{
			ERR_PRINTF2(_L("SetPositionMicroBeatsL %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("SetPositionMicroBeatsL error can't read position from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetPositionMicroSecondsL(const TDesC& aSection)
	{


	TInt	tmp = 0;
	if( GetIntFromConfig(aSection, KPosition(), tmp) )
		{
		TTimeIntervalMicroSeconds	position = tmp;
		TRAPD( err, iMidiClient->SetPositionMicroSecondsL(position) );
		if( err==KErrNone )
			{
			INFO_PRINTF2(_L("SetPositionMicroSecondsL() %d"),TInt(position.Int64()));
			}
		else
			{
			ERR_PRINTF2(_L("SetPositionMicroSecondsL %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("SetPositionMicroSecondsL error can't read position from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetPriorityL(const TDesC& aSection)
	{
	TInt	priority;
	if( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityNormal;
		}

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TRAPD(err, iMidiClient->SetPriorityL(priority, priorityPreference));
	if( err==KErrNone )
		{
		INFO_PRINTF1(_L("SetPriorityL ok"));
		}
	else
		{
		ERR_PRINTF2(_L("SetPriorityL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetRepeatsL(const TDesC& aSection)
	{


	TInt	repeats = 0;
	if( !GetIntFromConfig(aSection, KRepeats(), repeats) )
		{
		ERR_PRINTF1(_L("SetRepeatsL error can't read int from Config"));
		SetBlockResult(EFail);
		}

	TInt	intervalTmp = 0;
	if( !GetIntFromConfig(aSection, KTimeIntervalSec(), intervalTmp) )
		{
		ERR_PRINTF1(_L("SetRepeatsL error can't read IntervalSec from Config"));
		SetBlockResult(EFail);
		}
	TTimeIntervalMicroSeconds	interval = intervalTmp;

	TRAPD( err, iMidiClient->SetRepeatsL(repeats,interval) );
	if( err==KErrNone )
		{
		INFO_PRINTF3(_L("SetRepeatsL() %d, TimeInterval %d"), repeats, TInt(interval.Int64()));
		}
	else
		{
		ERR_PRINTF2(_L("SetRepeatsL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetStopTimeL(const TDesC& aSection)
	{


	TInt	intervalTmp = 0;
	if( GetIntFromConfig(aSection, KTimeIntervalSec(), intervalTmp) )
		{
		TTimeIntervalMicroSeconds	interval = intervalTmp;
		TRAPD( err, iMidiClient->SetStopTimeL(interval) );
		if( err==KErrNone )
			{
			INFO_PRINTF2(_L("SetStopTimeL() %d"),TInt(interval.Int64()));
			}
		else
			{
			ERR_PRINTF2(_L("SetStopTimeL %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("SetStopTimeL error can't read IntervalSec from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetSyncUpdateCallbackIntervalL(const TDesC& aSection)
	{
	TInt	tmp = 0;
	GetIntFromConfig(aSection, KMicroBeats(), tmp);
	TInt64	microBeats(tmp);

	if( GetIntFromConfig(aSection, KMicroSeconds(), tmp) )
		{
		TTimeIntervalMicroSeconds	microSeconds = tmp;
		TRAPD( err, iMidiClient->SetSyncUpdateCallbackIntervalL(microSeconds,microBeats) );
		if( err==KErrNone )
			{
			IncOutstanding();
			INFO_PRINTF3(_L("SetSyncUpdateCallbackIntervalL() microSeconds %d, microBeats %d"),TInt(microSeconds.Int64()),TInt(microBeats));
			}
		else
			{
			ERR_PRINTF2(_L("SetSyncUpdateCallbackIntervalL() %d"),err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF1(_L("SetSyncUpdateCallbackIntervalL() error can't read microSeconds from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetTempoL(const TDesC& aSection)
	{


	TInt	tempo = 0;
	if( GetIntFromConfig(aSection, KTempo(), tempo) )
		{
		TRAPD( err, iMidiClient->SetTempoL(tempo) );
		if( err==KErrNone )
			{
			INFO_PRINTF2(_L("SetTempoL() %d"),tempo);
			}
		else
			{
			ERR_PRINTF2(_L("SetTempoL %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("SetTempoL error can't read tempo from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetTrackMuteL(const TDesC& aSection)
	{


	TBool	mute = EFalse;
	if( !GetBoolFromConfig(aSection, KMute(), mute) )
		{
		ERR_PRINTF1(_L("SetTrackMuteL error can't read bool from Config"));
		SetBlockResult(EFail);
		}

	TInt	track = 0;
	if( !GetIntFromConfig(aSection, KTrack(), track) )
		{
		ERR_PRINTF1(_L("SetTrackMuteL error can't read int from Config"));
		SetBlockResult(EFail);
		}

	TRAPD( err, iMidiClient->SetTrackMuteL(track,mute) );
	if( err==KErrNone )
		{
		INFO_PRINTF3(_L("SetTrackMuteL(%d, %d)"), track, mute);
		}
	else
		{
		ERR_PRINTF2(_L("SetTrackMuteL() %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetVolumeL(const TDesC& aSection)
	{


	TInt	volume = iMaxVolume;	//volume will be 0 or MaxVolume
	GetIntFromConfig(aSection, KVolume(), volume);
	TRAPD( err, iMidiClient->SetVolumeL(volume) );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("SetVolumeL(%d)"), volume);
		}
	else
		{
		ERR_PRINTF2(_L("SetVolumeL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdSetVolumeRampL(const TDesC& aSection)
	{


	TInt	temp = 0;
	if( GetIntFromConfig(aSection, KTimeIntervalSec(), temp) )
		{
		TTimeIntervalMicroSeconds	rampDuration = temp;
		TRAPD( err, iMidiClient->SetVolumeRampL(rampDuration) );
		if( err==KErrNone )
			{
			INFO_PRINTF2(_L("SetVolumeRampL(%d)"),temp);
			}
		else
			{
			ERR_PRINTF2(_L("SetVolumeRampL %d"),err);
			SetError( err );
			}
		}
	else
		{
		ERR_PRINTF1(_L("SetVolumeRampL() error can't read intervalSec from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdState(const TDesC& aSection)
	{
	TMidiState	actual=iMidiClient->State();
	INFO_PRINTF2(_L("Current midi state: %d"), actual);

	TMidiState	expected;
	if( CT_MMUtil::ReadMidiState(*this, aSection, KState(), expected) )
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(_L("Not expected state"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMidiClientUtility::DoCmdStop(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;

	TInt intervalTmp = 0;
	GetIntFromConfig(aSection, KTimeIntervalSec(), intervalTmp);
	TTimeIntervalMicroSeconds	fadeOut = intervalTmp;

	iMidiClient->Stop(fadeOut);

	INFO_PRINTF2(_L("Stop() FadeOutDuration %d"),TInt(fadeOut.Int64()));
	}

void CT_MMDataMidiClientUtility::DoCmdStopNotes(const TDesC& aSection)
	{


	TInt	channel = 0;
	if( GetIntFromConfig(aSection, KChannel(), channel) )
		{
		iMidiClient->StopNotes(channel);
		INFO_PRINTF2(_L("StopNotes() channel %d"),channel);
		}
	else
		{
		ERR_PRINTF1(_L("StopNotes error can't read channel from Config"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdStopTimeL(const TDesC& aSection)
	{


	TTimeIntervalMicroSeconds interval;
	TRAPD( err, iMidiClient->StopTimeL(interval) );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("StopTimeL() %d"),TInt(interval.Int64()));

		TInt intervalTmp = 0;
		if( GetIntFromConfig(aSection, KTimeIntervalSec(), intervalTmp) )
			{
			TTimeIntervalMicroSeconds	intervalCfg = intervalTmp;
			if( interval!=intervalCfg )
				{
				ERR_PRINTF3(_L("StopTimeL() %d != config interval %d"), TInt(interval.Int64()),TInt(intervalCfg.Int64()));
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("StopTimeL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdTempoMicroBeatsPerMinuteL(const TDesC& aSection)
	{


	TInt tempo = 0;
	TRAPD( err, tempo = iMidiClient->TempoMicroBeatsPerMinuteL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("TempoMicroBeatsPerMinuteL() %d"),tempo);

		TInt	tempoCfg = 0;
		if( GetIntFromConfig(aSection, KTempo(), tempoCfg) )
			{
			if( tempo!=tempoCfg )
				{
				ERR_PRINTF3(_L("TempoMicroBeatsPerMinuteL() %d != config tempo %d"), tempo, tempoCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("TempoMicroBeatsPerMinuteL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdUnloadAllCustomBanksL()
	{
	TRAPD( err, iMidiClient->UnloadAllCustomBanksL() );
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("UnloadAllCustomBanksL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdUnloadCustomBankL(const TDesC& aSection)
	{


	TInt	bankIndex = iCustomBankId;
	GetIntFromConfig(aSection, KBankId(), bankIndex);

	TRAPD( err, iMidiClient->UnloadCustomBankL(bankIndex) );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("UnloadCustomBankL(%d)"),bankIndex);
		}
	else
		{
		ERR_PRINTF2(_L("UnloadCustomBankL() %d"),err);
		SetError(err);
		}
	}

void CT_MMDataMidiClientUtility::DoCmdUnloadCustomInstrumentL(const TDesC& aSection)
	{


	TInt	bankIndex = 0;
	if( !GetIntFromConfig(aSection, KBankId(), bankIndex) )
		{
		ERR_PRINTF1(_L("UnloadCustomInstrumentL() error can't read custom bankId from Config"));
		SetBlockResult(EFail);
		}

	TInt	instrumentIndex = 0;
	if( !GetIntFromConfig(aSection, KInstrumentId(), instrumentIndex) )
		{
		ERR_PRINTF1(_L("UnloadCustomInstrumentL() error can't read instrumentId from Config"));
		SetBlockResult(EFail);
		}

	TRAPD( err, iMidiClient->UnloadCustomInstrumentL(bankIndex,instrumentIndex) );
	if( err==KErrNone )
		{
		INFO_PRINTF3(_L("UnloadCustomInstrumentL(%d, %d)"), bankIndex, instrumentIndex);
		}
	else
		{
		ERR_PRINTF2(_L("UnloadCustomInstrumentL %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdVolumeL(const TDesC& aSection)
	{


	TInt	volume = 0;
	TRAPD( err, volume=iMidiClient->VolumeL() );
	if( err==KErrNone )
		{
		INFO_PRINTF2(_L("VolumeL() %d"),volume);

		TInt	volumeCfg = iMaxVolume; //volume will be 0 or iMaxVolume
		if( GetIntFromConfig(aSection, KVolume(), volumeCfg) || iMaxVolume>0 )
			{
			if( volume==volumeCfg )
				{
				INFO_PRINTF3(_L("VolumeL() %d == Config Volume %d"),volume,volumeCfg);
				}
			else
				{
				ERR_PRINTF3(_L("VolumeL() %d != Config Volume %d"),volume,volumeCfg);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("VolumeL() error %d"),err);
		SetError( err );
		}
	}

void CT_MMDataMidiClientUtility::DoCmdDestructor()
	{
	DestroyData();
	}

/**
Log Data From Text
@internalAll
@return void
@pre None
@post Default midi controller
*/
void CT_MMDataMidiClientUtility::LogDataFromTextL()
	{
	HBufC*	buffer=HBufC::NewLC(iDataFromText.Length());
	TPtr	bufferPtr=buffer->Des();
	bufferPtr.Copy(iDataFromText); // Create a 16 bit copy of the 8 bit original
	INFO_PRINTF2(_L("DataFrom: %S"), &bufferPtr);
	CleanupStack::PopAndDestroy(buffer);
	}

///////////////////////////////////////////////////////////////////
//from the MMidiClientUtilityObserver interface 				//
///////////////////////////////////////////////////////////////////

/**
Callback to receive state change events.
@internalComponent
@return void
@param aOldState, aNewState, aTime, aErrorCode
@pre N/A
@post N/A
*/
void CT_MMDataMidiClientUtility::MmcuoStateChanged(TMidiState aOldState, TMidiState aNewState, const TTimeIntervalMicroSeconds &aTime, TInt aError)
	{
	INFO_PRINTF4(_L("New state: %d, old state: %d, aTime %d"), aNewState, aOldState, TInt(aTime.Int64()));

	if( aError!=KErrNone )
		{
		ERR_PRINTF2(_L("State Change Error %d"),aError);
		SetAsyncError(iApiAsyncErrorIndex, aError);
		}
	DecOutstanding();
	}

/**
Callback to receive Instrument change events.
@internalComponent
@return void
@param channel, bankId, instrumentId
@pre N/A
@post N/A
*/
void CT_MMDataMidiClientUtility::MmcuoInstrumentChanged(TInt aChannel, TInt aBankId, TInt aInstrumentId)
	{
	INFO_PRINTF4(_L("Instrument Changed: %d Channel %d, Bank %d"), aInstrumentId, aChannel, aBankId);
	}

/**
Callback to receive Tempo change events.
@internalComponent
@return void
@param aMicroBeatsPerMinute
@pre N/A
@post N/A
*/
void CT_MMDataMidiClientUtility::MmcuoTempoChanged(TInt aMicroBeatsPerMinute)
	{
	INFO_PRINTF2(_L("Tempo Changed to: %d"),aMicroBeatsPerMinute);
	}

/**
Callback to receive Volume change events.
@internalComponent
@return void
@param aChannel, aVolumeInDecibels
@pre N/A
@post N/A
*/
void CT_MMDataMidiClientUtility::MmcuoVolumeChanged(TInt aChannel, TReal32 aVolumeInDecibels)
	{
	INFO_PRINTF3(_L("Volume Changed to %g for channel %i "),aVolumeInDecibels, aChannel);
	}

/**
Callback to receive Muted change events.
@internalComponent
@return void
@param aChannel, aMuted
@pre N/A
@post N/A
*/
void CT_MMDataMidiClientUtility::MmcuoMuteChanged(TInt aChannel, TBool aMuted)
	{
	if( aMuted )
		{
		INFO_PRINTF2(_L("Channel %d is Muted"),aChannel);
		}
	else
		{
		INFO_PRINTF2(_L("Channel %d is UnMuted"),aChannel);
		}
	}

/**
Callback to receive Sync update events.
@internalComponent
@return void
@param aMicroSeconds, aMicroBeats
@pre N/A
@post N/A
*/
void CT_MMDataMidiClientUtility::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds &aMicroSeconds, TInt64 aMicroBeats)
	{
	INFO_PRINTF3(_L("MmcuoSyncUpdate aMicroSeconds: %d,aMicroBeats: %d"),TInt(aMicroSeconds.Int64()),TInt(aMicroBeats));
	DecOutstanding();
	}

/**
Callback to receive Meta Data Entry Found events.
@internalComponent
@return void
@param aMetaDataEntryId, aPosition
@pre N/A
@post N/A
*/
void CT_MMDataMidiClientUtility::MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId, const TTimeIntervalMicroSeconds &aPosition)
	{
	INFO_PRINTF3(_L("MetaDataEntryFound ID: %d, Position %d"),aMetaDataEntryId,TInt(aPosition.Int64()));
	}

/**
Callback to receive Mip Message Received events.
@internalComponent
@return void
@param aMessage
@pre N/A
@post N/A
*/
void CT_MMDataMidiClientUtility::MmcuoMipMessageReceived(const RArray< TMipMessageEntry > &aMessage)
	{
	for(TInt i=0;i<aMessage.Count();i++)
		{
		INFO_PRINTF3(_L("Message Received Channel %d, Value %d"),aMessage[i].iChannel,aMessage[i].iMIPValue);
		}
	}

/**
Callback to receive Polyphony change events.
@internalComponent
@return void
@param aNewPolyphony
@pre N/A
@post N/A
*/
void CT_MMDataMidiClientUtility::MmcuoPolyphonyChanged(TInt aNewPolyphony)
	{
	INFO_PRINTF2(_L("Polyphony Changed to: %d"),aNewPolyphony);
	}

/**
 Virtual RunL - Called on completion of an aAsyncErrorIndex command
 @internalComponent
 @see MT_MMActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_MMDataMidiClientUtility::RunL(CActive* aActive, TInt aIndex)
 	{
 	if( aActive==iActiveCallback )
 		{
 		RunCustomCommandAsyncL(aIndex);
 		}
 	else
 		{
 		ERR_PRINTF1(_L("Stray signal"));
 		SetBlockResult(EFail);
 		}
 	}

/**
  Virtual DoCancel - Request to cancel the aAsyncErrorIndex command
  @internalComponent
  @see - MTPActiveCallback
  @param aActive Active Object that DoCancel has been called on
  @pre - N/A
  @post - N/A
  @leave system wide error code
 */
 void CT_MMDataMidiClientUtility::DoCancel(CActive* aActive, TInt aIndex)
 	{
 	if( aActive==iActiveCallback )
 		{
 		DoCancelCustomCommandAsync();
 		SetAsyncError( aIndex, iActiveCallback->iStatus.Int() );
 		}
 	else
 		{
 		ERR_PRINTF1(_L("Stray signal"));
 		SetBlockResult(EFail);
 		}
 	}

/**
  RunActiveL - Process RunL for iActiveCallback
  @internalComponent
  @pre N/A
  @post N/A
  @leave system wide error code
 */
 void CT_MMDataMidiClientUtility::RunCustomCommandAsyncL(TInt aIndex)
	{
	LogDataFromTextL();

	TInt	err = iActiveCallback->iStatus.Int();
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError( aIndex, err );
		}
	DecOutstanding();
	}

/**
 DoCancelActive - Process DoCancel for iActive
 @internalComponent
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_MMDataMidiClientUtility::DoCancelCustomCommandAsync()
	{
	}
