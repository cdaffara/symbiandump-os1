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



#include "T_MMDataMdaAudioRecorderUtility.h"
#include "T_MMUtil.h"
#include <mm/mmcaf.h>


/*@{*/
_LIT(KBalance,					"balance");
_LIT(KBitRate,					"bitrate");
_LIT(KDestinationBitRate,		"destinationbitrate");
_LIT(KGain,						"gain");
_LIT(KVolume,					"volume");
_LIT(KRFile,					"rfile");
_LIT(KTDesC,					"tdesc");
_LIT(KTMMSource,			    "tmmsource");
_LIT(KTDeviceMode,				"tdevicemode");
_LIT(KInvalidTDeviceMode,		"invalidtdevicemode");
_LIT(KSeconds,					"seconds");
_LIT(KStartSeconds,				"startseconds");
_LIT(KNumberOfTimes,			"numberoftimes");
_LIT(KEndSeconds,				"endseconds");
_LIT(KFormat,					"format");
_LIT(KChannels,					"channels");
_LIT(KSampleRate,				"samplerate");
_LIT(KFileName,					"filename");
_LIT(KFunction,					"function");
_LIT(KUseDataFrom,				"useDataFrom");
_LIT(KMetaDataEntryName,		"metadataentryname");
_LIT(KMetaDataEntryValue,		"metadataentryvalue");
_LIT(KMetaDataEntryID,			"metadataentryid");
_LIT(KMetaDataEntryIndex,		"metadataentryindex");
_LIT(KUseState,					"usestate");
_LIT(KCodec, 					"codec");
_LIT(KAudioSettings, 			"audiosettings");
_LIT(KInvalidAudioSettings,		"invalidaudiosettings");
_LIT(KMimeType,					"mimetype");
_LIT(KWriteLength,				"writelength");
_LIT(KControllerUid,			"controlleruid");
_LIT(KState,					"state");
_LIT(KDataTo1,					"datato1");
_LIT(KDataTo2,					"datato2");
_LIT(KExcpectedNumber,			"expectednumber");
_LIT(KNew,						"new");
_LIT(KEventUID, 				"eventuid");
_LIT(KDuration, 				"duration");
_LIT(KLoadingProgress, 			"loadingprogress");
_LIT(KExpectNull, 				"expectnull");
_LIT(KNoOfMetaDataEntries, 		"noofmetadataentries");
_LIT(KAccessPointID, 			"accesspointid");
_LIT(KPosition,		 			"position");
_LIT(KRecTime,		 			"rectime");
_LIT(KDataNotification,			"datanotification");
_LIT(KBufferSize,				"buffersize");
_LIT(KConst,					"const");
_LIT(KUniqueId,					"uniqueid");
_LIT(KEnableUI,					"enableui");
_LIT(KDurationState, 			"durationstate");
_LIT(KAudioFormat,				"audioformat");


_LIT(KCmdDestructor,											"~");
_LIT(KCmdAddMetaDataEntryL,										"AddMetaDataEntryL");
_LIT(KCmdAudioPlayControllerImplementationInformationL,			"AudioPlayControllerImplementationInformationL");
_LIT(KCmdAudioRecorderControllerImplementationInformationL,		"AudioRecorderControllerImplementationInformationL");
_LIT(KCmdCancelRegisterAudioResourceNotification,				"CancelRegisterAudioResourceNotification");
_LIT(KCmdClearPlayWindow,										"ClearPlayWindow");
_LIT(KCmdClose,													"Close");
_LIT(KCmdCropFromBeginningL,									"CropFromBeginningL");
_LIT(KCmdCropL,													"CropL");
_LIT(KCmdDestinationBitRateL,									"DestinationBitRateL");
_LIT(KCmdDestinationDataTypeL,									"DestinationDataTypeL");
_LIT(KCmdDestinationFormatL,									"DestinationFormatL");
_LIT(KCmdDestinationNumberOfChannelsL,							"DestinationNumberOfChannelsL");
_LIT(KCmdDestinationSampleRateL,								"DestinationSampleRateL");
_LIT(KCmdDuration,												"Duration");
_LIT(KCmdGetAudioLoadingProgressL,								"GetAudioLoadingProgressL");
_LIT(KCmdGetDRMCustomCommand,									"GetDRMCustomCommand");
_LIT(KCmdGetGain,												"GetGain");
_LIT(KCmdGetMetaDataEntryL,										"GetMetaDataEntryL");
_LIT(KCmdGetNumberOfMetaDataEntries,							"GetNumberOfMetaDataEntries");
_LIT(KCmdGetPlaybackBalance,									"GetPlaybackBalance");
_LIT(KCmdGetRecordBalance,										"GetRecordBalance");
_LIT(KCmdGetSupportedBitRatesL,									"GetSupportedBitRatesL");
_LIT(KCmdGetSupportedDestinationDataTypesL,						"GetSupportedDestinationDataTypesL");
_LIT(KCmdGetSupportedNumberOfChannelsL,							"GetSupportedNumberOfChannelsL");
_LIT(KCmdGetSupportedSampleRatesL,								"GetSupportedSampleRatesL");
_LIT(KCmdGetVolume,												"GetVolume");
_LIT(KCmdMaxGain,												"MaxGain");
_LIT(KCmdMaxVolume,												"MaxVolume");
_LIT(KCmdNewL,													"NewL");
_LIT(KCmdOpenDesL,												"OpenDesL");
_LIT(KCmdOpenFileL,												"OpenFileL");
_LIT(KCmdOpenL,													"OpenL");
_LIT(KCmdOpenUrlL,												"OpenUrlL");
_LIT(KCmdPlayControllerCustomCommandAsync,						"PlayControllerCustomCommandAsync");
_LIT(KCmdPlayControllerCustomCommandSync,						"PlayControllerCustomCommandSync");
_LIT(KCmdPlayL,													"PlayL");
_LIT(KCmdPosition,												"Position");
_LIT(KCmdRecordControllerCustomCommandAsync,					"RecordControllerCustomCommandAsync");
_LIT(KCmdRecordControllerCustomCommandSync,						"RecordControllerCustomCommandSync");
_LIT(KCmdRecordL,												"RecordL");
_LIT(KCmdRecordTimeAvailable,									"RecordTimeAvailable");
_LIT(KCmdRegisterAudioResourceNotification,						"RegisterAudioResourceNotification");
_LIT(KCmdRegisterForAudioLoadingNotification,					"RegisterForAudioLoadingNotification");
_LIT(KCmdRemoveMetaDataEntry,									"RemoveMetaDataEntry");
_LIT(KCmdReplaceMetaDataEntryL,									"ReplaceMetaDataEntryL");
_LIT(KCmdSetAudioDeviceMode,									"SetAudioDeviceMode");
_LIT(KCmdSetDestinationBitRateL,								"SetDestinationBitRateL");
_LIT(KCmdSetDestinationDataTypeL,								"SetDestinationDataTypeL");
_LIT(KCmdSetDestinationFormatL,									"SetDestinationFormatL");
_LIT(KCmdSetDestinationNumberOfChannelsL,						"SetDestinationNumberOfChannelsL");
_LIT(KCmdSetDestinationSampleRateL,								"SetDestinationSampleRateL");
_LIT(KCmdSetGain,												"SetGain");
_LIT(KCmdSetMaxWriteLength,										"SetMaxWriteLength");
_LIT(KCmdSetPlayWindow,											"SetPlayWindow");
_LIT(KCmdSetPlaybackBalance,									"SetPlaybackBalance");
_LIT(KCmdSetPosition,											"SetPosition");
_LIT(KCmdSetPriority,											"SetPriority");
_LIT(KCmdSetRecordBalance,										"SetRecordBalance");
_LIT(KCmdSetRepeats,											"SetRepeats");
_LIT(KCmdSetThreadPriorityPlayback,								"SetThreadPriorityPlayback");
_LIT(KCmdSetThreadPriorityRecord,								"SetThreadPriorityRecord");
_LIT(KCmdSetVolume,												"SetVolume");
_LIT(KCmdSetVolumeRamp,											"SetVolumeRamp");
_LIT(KCmdSourceBitRateL,										"SourceBitRateL");
_LIT(KCmdState,													"State");
_LIT(KCmdStop,													"Stop");
_LIT(KCmdWillResumePlay,										"WillResumePlay");
_LIT(KCmdUseSharedHeap,											"UseSharedHeap");

//TDeviceMode
_LIT(KDefault,						"Default");
_LIT(KTelephonyOrLocal,				"TelephonyOrLocal");
_LIT(KTelephonyMixed,				"TelephonyMixed");
_LIT(KTelephonyNonMixed,			"TelephonyNonMixed");
_LIT(KLocal,						"Local");


_LIT(KENotReady,					"ENotReady");
_LIT(KEOpen,						"EOpen");
_LIT(KEPlaying,						"EPlaying");
_LIT(KERecording,					"ERecording");
_LIT(KEUnknown,						"EUnknown");

_LIT(KWavClipFormat,				"WavClipFormat");
_LIT(KAuClipFormat,					"AuClipFormat");
_LIT(KRawAudioClipFormat,			"RawAudioClipFormat");
_LIT(KInvalidClipFormat,			"InvalidClipFormat");

_LIT(KPcmWavCodec,					"PcmWavCodec");
_LIT(KAlawWavCodec,					"AlawWavCodec");
_LIT(KMulawWavCodec,				"MulawWavCodec");
_LIT(KImaAdpcmWavCodec,				"ImaAdpcmWavCodec");
_LIT(KAuCodec,						"AuCodec");
_LIT(KPcm8BitAuCodec,				"Pcm8BitAuCodec");
_LIT(KPcm16BitAuCodec,				"Pcm16BitAuCodec");
_LIT(KAlawAuCodec,					"AlawAuCodec");
_LIT(KMulawAuCodec,					"MulawAuCodec");
_LIT(KRawAudioCodec,				"RawAudioCodec");
_LIT(KMulawRawAudioCodec,			"MulawRawAudioCodec");
_LIT(KAlawRawAudioCodec,			"AlawRawAudioCodec");
_LIT(KS8PcmRawAudioCodec,			"S8PcmRawAudioCodec");
_LIT(KU8PcmRawAudioCodec,			"U8PcmRawAudioCodec");
_LIT(KSL16RawAudioCodec,			"SL16RawAudioCodec");
_LIT(KSB16RawAudioCodec,			"SB16RawAudioCodec");
_LIT(KUL16RawAudioCodec,			"UL16RawAudioCodec");
_LIT(KUB16RawAudioCodec,			"UB16RawAudioCodec");
_LIT(KInvalidCodec,					"InvalidCodec");

_LIT(KEMMFDurationInfoValid, 		"Valid");
_LIT(KEMMFDurationInfoInfinite, 	"Infinite");
_LIT(KEMMFDurationInfoUnknown, 		"Unknown");
/*@}*/


/**
CT_MMDataMdaAudioRecorderUtility constructor.
@internalComponent
@return N/A
@pre None
@post CT_MMDataMdaAudioRecorderUtility 1st stage constructor
*/
CT_MMDataMdaAudioRecorderUtility* CT_MMDataMdaAudioRecorderUtility::NewL()
	{
	CT_MMDataMdaAudioRecorderUtility*	ret=new (ELeave) CT_MMDataMdaAudioRecorderUtility();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_MMDataMdaAudioRecorderUtility::CT_MMDataMdaAudioRecorderUtility()
:	iRecorderUtility(NULL)
,	iActiveCustomCommandAsync(NULL)
,	iAsyncErrorIndex(KErrNone)
,	iFileOpened(EFalse)
,	iBuffer(NULL)
,	iBufferPtr(NULL)
,	iBufferSize(KMdaAudioRecorderBufferSize)
,	iCustomCommandData(EFalse)
,	iRecording(EFalse)
	{
	}

/**
CT_MMDataMdaAudioRecorderUtility destructor
@internalComponent
@return N/A
@pre None
@post releases CT_MMDataMdaAudioRecorderUtility heap memory
*/
CT_MMDataMdaAudioRecorderUtility::~CT_MMDataMdaAudioRecorderUtility()
	{
	DestroyData();

	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}

	delete iActiveCustomCommandAsync;
	iActiveCustomCommandAsync = NULL;

	iFs.Close();
	}

TAny* CT_MMDataMdaAudioRecorderUtility::GetObject()
	{
	return iRecorderUtility;
	}

void CT_MMDataMdaAudioRecorderUtility::SetObjectL(TAny* aObject)
	{
	DestroyData();
	iRecorderUtility	= static_cast<CMdaAudioRecorderUtility*> (aObject);
	}

void CT_MMDataMdaAudioRecorderUtility::DisownObjectL()
	{
	iRecorderUtility = NULL;
	}

/**
Releases CT_MMDataMdaAudioRecorderUtility dynamic memory.
@internalComponent
@return void
@pre None
@post All dynamic memory deallocated
*/
void CT_MMDataMdaAudioRecorderUtility::DestroyData()
	{
	delete iRecorderUtility;
	iRecorderUtility = NULL;

	delete iBuffer;
	iBuffer	= NULL;

	delete iBufferPtr;
	iBufferPtr	= NULL;
	}

void CT_MMDataMdaAudioRecorderUtility::ConstructL()
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
	iActiveCustomCommandAsync = CActiveCallback::NewL(*this);
	}

TBool CT_MMDataMdaAudioRecorderUtility::DoCommandL(	const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex )
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
	TBool ret=ETrue;

	if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdAddMetaDataEntryL )
		{
		DoCmdAddMetaDataEntryL(aSection);
		}
	else if ( aCommand==KCmdReplaceMetaDataEntryL )
		{
		DoCmdReplaceMetaDataEntryL(aSection );
		}
	else if ( aCommand==KCmdRemoveMetaDataEntry )
		{
		DoCmdRemoveMetaDataEntry(aSection);
		}
	else if ( aCommand==KCmdGetNumberOfMetaDataEntries )
		{
		DoCmdGetNumberOfMetaDataEntries(aSection);
		}
	else if ( aCommand==KCmdGetMetaDataEntryL )
		{
		DoCmdGetMetaDataEntryL(aSection);
		}
	else if ( aCommand==KCmdAudioPlayControllerImplementationInformationL )
		{
		DoCmdAudioPlayControllerImplementationInformationL();
		}
	else if ( aCommand==KCmdAudioRecorderControllerImplementationInformationL )
		{
		DoCmdAudioRecorderControllerImplementationInformationL();
		}
	else if ( aCommand==KCmdCancelRegisterAudioResourceNotification )
		{
		DoCmdCancelRegisterAudioResourceNotification(aSection);
		}
	else if ( aCommand==KCmdDuration )
		{
		DoCmdDuration(aSection);
		}
	else if ( aCommand==KCmdRegisterAudioResourceNotification )
		{
		DoCmdRegisterAudioResourceNotificationL(aSection);
		}
	else if ( aCommand==KCmdRegisterForAudioLoadingNotification )
		{
		DoCmdRegisterForAudioLoadingNotification();
		}
	else if ( aCommand==KCmdGetAudioLoadingProgressL )
		{
		DoCmdGetAudioLoadingProgressL(aSection);
		}
	else if ( aCommand==KCmdGetDRMCustomCommand )
		{
		DoCmdGetDRMCustomCommand(aSection);
		}
	else if ( aCommand==KCmdSetPlayWindow )
		{
		DoCmdSetPlayWindow(aSection);
		}
	else if ( aCommand==KCmdClearPlayWindow )
		{
		DoCmdClearPlayWindow();
		}
	else if ( aCommand==KCmdGetSupportedBitRatesL )
		{
		DoCmdGetSupportedBitRatesL(aSection);
		}
	else if ( aCommand==KCmdGetSupportedSampleRatesL )
		{
		DoCmdGetSupportedSampleRatesL(aSection);
		}
	else if ( aCommand==KCmdGetSupportedNumberOfChannelsL )
		{
		DoCmdGetSupportedNumberOfChannelsL(aSection);
		}
	else if ( aCommand==KCmdGetSupportedDestinationDataTypesL )
		{
		DoCmdGetSupportedDestinationDataTypesL(aSection);
		}
	else if ( aCommand==KCmdGetPlaybackBalance )
		{
		DoCmdGetPlaybackBalance(aSection);
		}
	else if ( aCommand==KCmdSetPlaybackBalance )
		{
		DoCmdSetPlaybackBalance(aSection);
		}
	else if ( aCommand==KCmdGetRecordBalance )
		{
		DoCmdGetRecordBalance(aSection);
		}
	else if ( aCommand==KCmdSetRecordBalance )
		{
		DoCmdSetRecordBalance(aSection);
		}
	else if ( aCommand==KCmdDestinationDataTypeL )
		{
		DoCmdDestinationDataTypeL(aSection);
		}
	else if ( aCommand==KCmdSetDestinationDataTypeL )
		{
		DoCmdSetDestinationDataTypeL(aSection);
		}
	else if ( aCommand==KCmdSetDestinationBitRateL )
		{
		DoCmdSetDestinationBitRateL(aSection);
		}
	else if ( aCommand==KCmdSetDestinationFormatL )
		{
		DoCmdSetDestinationFormatL(aSection);
		}
	else if ( aCommand==KCmdSetDestinationNumberOfChannelsL )
		{
		DoCmdSetDestinationNumberOfChannelsL(aSection);
		}
	else if ( aCommand==KCmdSetDestinationSampleRateL )
		{
		DoCmdSetDestinationSampleRateL(aSection);
		}
	else if ( aCommand==KCmdDestinationBitRateL )
		{
		DoCmdDestinationBitRateL(aSection);
		}
	else if ( aCommand==KCmdDestinationFormatL )
		{
		DoCmdDestinationFormatL(aSection);
		}
	else if ( aCommand==KCmdDestinationNumberOfChannelsL )
		{
		DoCmdDestinationNumberOfChannelsL(aSection);
		}
	else if ( aCommand==KCmdDestinationSampleRateL )
		{
		DoCmdDestinationSampleRateL(aSection);
		}
	else if ( aCommand==KCmdGetVolume )
		{
		DoCmdGetVolume(aSection);
		}
	else if ( aCommand==KCmdGetGain )
		{
		DoCmdGetGain(aSection);
		}
	else if ( aCommand==KCmdSetGain )
		{
		DoCmdSetGain(aSection);
		}
	else if ( aCommand==KCmdMaxGain )
		{
		DoCmdMaxGain(aSection);
		}
	else if ( aCommand==KCmdMaxVolume )
		{
		DoCmdMaxVolume(aSection);
		}
	else if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdOpenDesL )
		{
		DoCmdOpenDesL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdOpenFileL )
		{
		DoCmdOpenFileL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdOpenL )
		{
		DoCmdOpenL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdOpenUrlL )
		{
		DoCmdOpenUrlL(aSection);
		}
	else if ( aCommand==KCmdPosition )
		{
		DoCmdPosition(aSection);
		}
	else if ( aCommand==KCmdSetPosition )
		{
		DoCmdSetPosition(aSection);
		}
	else if ( aCommand==KCmdSourceBitRateL )
		{
		DoCmdSourceBitRateL(aSection);
		}
	else if ( aCommand==KCmdSetAudioDeviceMode )
		{
		DoCmdSetAudioDeviceMode(aSection);
		}
	else if ( aCommand==KCmdRecordTimeAvailable )
		{
		DoCmdRecordTimeAvailable(aSection);
		}
	else if ( aCommand==KCmdSetPriority )
		{
		DoCmdSetPriority(aSection);
		}
	else if ( aCommand==KCmdSetVolume )
		{
		DoCmdSetVolume(aSection);
		}
	else if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if ( aCommand==KCmdCropL )
		{
		DoCmdCropL();
		}
	else if ( aCommand==KCmdCropFromBeginningL )
		{
		DoCmdCropFromBeginningL();
		}
	else if ( aCommand==KCmdPlayL )
		{
		DoCmdPlayL(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdRecordL )
		{
		DoCmdRecordL(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdSetMaxWriteLength )
		{
		DoCmdSetMaxWriteLength(aSection);
		}
	else if ( aCommand==KCmdSetRepeats )
		{
		DoCmdSetRepeats(aSection);
		}
	else if ( aCommand==KCmdSetThreadPriorityPlayback )
		{
		DoCmdSetThreadPriorityPlayback(aSection);
		}
	else if ( aCommand==KCmdSetThreadPriorityRecord )
		{
		DoCmdSetThreadPriorityRecord(aSection);
		}
	else if ( aCommand==KCmdState )
		{
		DoCmdState(aSection);
		}
	else if ( aCommand==KCmdSetVolumeRamp )
		{
		DoCmdSetVolumeRamp(aSection);
		}
	else if ( aCommand==KCmdStop )
		{
		DoCmdStop();
		}
	else if ( aCommand==KCmdWillResumePlay )
		{
		DoCmdWillResumePlay();
		}
	else if ( aCommand==KCmdState )
		{
		DoCmdState(aSection);
		}
	else if (aCommand == KCmdUseSharedHeap )
		{
		DoCmdUseSharedHeap();
		}
	else if ( aCommand==KCmdPlayControllerCustomCommandSync )
		{
		DoCmdPlayControllerCustomCommandSyncL(aSection);
		}
	else if ( aCommand==KCmdPlayControllerCustomCommandAsync )
		{
		DoCmdPlayControllerCustomCommandAsyncL(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdRecordControllerCustomCommandSync )
		{
		DoCmdRecordControllerCustomCommandSyncL(aSection);
		}
	else if ( aCommand==KCmdRecordControllerCustomCommandAsync )
		{
		DoCmdRecordControllerCustomCommandAsyncL(aSection, aAsyncErrorIndex);
		}
	else
		{
		ret=EFalse;
	}

	return ret;
	}

///////////////////////////////////////////////////////////////////
//from the MMdaObjectStateChangeObserver interface 				//
///////////////////////////////////////////////////////////////////

/**
Callback to receive state change events from recorder.
@internalComponent
@return void
@param aObject, aPreviousState, aCurrentState, aErrorCode
This can be completion of a call to CMdaAudioRecorderUtility:OpenL(),
CMdaAudioRecorderUtility::OpenFileL(), CMdaAudioRecorderUtility::OpenUrlL(),
CMdaAudioRecorderUtility::OpenDesL(),CMdaAudioRecorderUtility::PlayL(),
CMdaAudioRecorderUtility::RecordL().
The event contains a aObject identifying the event and an accompanying error code.
@pre N/A
@post N/A
*/
void CT_MMDataMdaAudioRecorderUtility::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	if (aObject == iRecorderUtility)
		{
		TBool continueWaiting = EFalse;

		INFO_PRINTF3(_L("Current state: %S, previous state: %S"), &GetState(aCurrentState), &GetState(aPreviousState));
		if (aErrorCode!=KErrNone)
			{
			ERR_PRINTF2(_L("MoscoStateChangeEvent Error %d"), aErrorCode);
			SetAsyncError(iAsyncErrorIndex, aErrorCode);
			DecOutstanding();
			}
		// If current state is ERecording or EPlaying, we expect a second callback when finished
		else if ( (aCurrentState == CMdaAudioClipUtility::ERecording) ||
				  (aCurrentState == CMdaAudioClipUtility::EPlaying) )
			{
			continueWaiting = ETrue;
			}

		if (!continueWaiting)
			{
			DecOutstanding();
			}
		}
	else
		{
		ERR_PRINTF1(_L("MoscoStateChangeEvent: object aren't CMdaAudioRecorderUtility"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	if ( aNotificationEventId==KMMFEventCategoryAudioResourceAvailable )
		{
		//	Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg	timeIntvbuf;
		timeIntvbuf.Copy(aNotificationData);
		TTimeIntervalMicroSeconds	intPos = timeIntvbuf();
		INFO_PRINTF2(_L("Interval %d"), intPos.Int64());
		}
	else
		{
		INFO_PRINTF1(_L("MMMFAudioResourceNotificationCallback, but NOT KMMFEventCategoryAudioResourceAvailable"));
		}
	DecOutstanding();
	}

void CT_MMDataMdaAudioRecorderUtility::MaloLoadingStarted()
	{
	INFO_PRINTF1(_L("Loading started"));
	IncOutstanding();
	}

void CT_MMDataMdaAudioRecorderUtility::MaloLoadingComplete()
	{
	INFO_PRINTF1(_L("Loading complete"));
	DecOutstanding();
	}

TBool CT_MMDataMdaAudioRecorderUtility::ConvertToTDeviceMode(const TDesC& aDesc, CMdaAudioRecorderUtility::TDeviceMode& aDeviceMode)
	{
	TBool	ret=ETrue;

	if ( aDesc==KDefault )
		{
		aDeviceMode=CMdaAudioRecorderUtility::EDefault;
		}
	else if ( aDesc==KTelephonyOrLocal )
		{
		aDeviceMode=CMdaAudioRecorderUtility::ETelephonyOrLocal;
		}
	else if ( aDesc==KTelephonyMixed )
		{
		aDeviceMode=CMdaAudioRecorderUtility::ETelephonyMixed;
		}
	else if ( aDesc==KTelephonyNonMixed )
		{
		aDeviceMode=CMdaAudioRecorderUtility::ETelephonyNonMixed;
		}
	else if ( aDesc==KLocal )
		{
		aDeviceMode=CMdaAudioRecorderUtility::ELocal;
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
 Virtual RunL - Called on completion of an asynchronous command
 @internalComponent
 @see MTPActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_MMDataMdaAudioRecorderUtility::RunL(CActive* aActive, const TInt aAsyncErrorIndex)
 	{
 	if ( aActive==iActiveCustomCommandAsync )
 		{
 		RunCustomCommandAsyncL(aAsyncErrorIndex);
 		}
 	else
 		{
 		ERR_PRINTF1(_L("Stray signal"));
 		SetBlockResult(EFail);
 		}
 	}

/**
 Virtual DoCancel - Request to cancel the asynchronous command
 @internalComponent
 @see - MTPActiveCallback
 @param aActive Active Object that DoCancel has been called on
 @pre - N/A
 @post - N/A
 @leave system wide error code
*/
void CT_MMDataMdaAudioRecorderUtility::DoCancel(CActive* aActive)
 	{
 	if ( aActive==iActiveCustomCommandAsync )
 		{
 		DoCancelCustomCommandAsync();
 		}
 	else
 		{
 		ERR_PRINTF1(_L("Stray signal"));
 		SetBlockResult(EFail);
 		}
 	}

/**
  RunActiveL - Process RunL for iActiveCustomCommandAsync
  @internalComponent
  @pre N/A
  @post N/A
  @leave system wide error code
 */
 void CT_MMDataMdaAudioRecorderUtility::RunCustomCommandAsyncL(const TInt aAsyncErrorIndex)
	{
	if (iCustomCommandData)
	 	{
		iCustomCommandData = EFalse;
		INFO_PRINTF2(_L("DataFrom: %S"), &iDataFromText);
	 	}
	TInt	err = iActiveCustomCommandAsync->iStatus.Int();
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError(aAsyncErrorIndex, err);
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
void CT_MMDataMdaAudioRecorderUtility::DoCancelCustomCommandAsync()
	{
	}

/**
Get state from Id
@internalComponent
@return N/A
@param aUid
@pre N/A
@post N/A
*/
const TDesC& CT_MMDataMdaAudioRecorderUtility::GetState(const TInt& aState)
	{
	switch (aState)
		{
	case CMdaAudioClipUtility::ENotReady:
		return KENotReady;
	case CMdaAudioClipUtility::EOpen:
		return KEOpen;
	case CMdaAudioClipUtility::EPlaying:
		return KEPlaying;
	case CMdaAudioClipUtility::ERecording:
		return KERecording;
	default:
		break;
		};
	return KEUnknown;
	}

TBool CT_MMDataMdaAudioRecorderUtility::GetFormatL(const TDesC& aFormatStr, TMdaClipFormat* &aMdaClipFormat, const TDesC& aSection)
	{
	TBool	ret = ETrue;
	delete 	aMdaClipFormat;
	aMdaClipFormat = NULL;

	if (aFormatStr == KWavClipFormat)
		{
		aMdaClipFormat = new(ELeave) TMdaWavClipFormat();
		}
	else if (aFormatStr == KAuClipFormat)
		{
		aMdaClipFormat = new(ELeave) TMdaAuClipFormat();
		}
	else if (aFormatStr == KRawAudioClipFormat)
		{
		aMdaClipFormat = new(ELeave) TMdaRawAudioClipFormat();
		}
	else if (aFormatStr == KInvalidClipFormat)
		{
		TInt	invalidUid = 0;
		GetHexFromConfig(aSection, KFormat(), invalidUid);
		aMdaClipFormat = new(ELeave) TMdaClipFormat();
		TUid	clipUid = TUid::Uid(invalidUid);
		aMdaClipFormat->SetUid(clipUid);
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdDestructor()
	{
	DestroyData();
	}

void	CT_MMDataMdaAudioRecorderUtility::DoCmdAddMetaDataEntryL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TPtrC	metaDataEntryName;
	TPtrC	metaDataEntryValue;
	if ( !GetStringFromConfig(aSection, KMetaDataEntryName(), metaDataEntryName) )
		{
		ERR_PRINTF2(_L("No %S"), &tempStore);
		SetBlockResult(EFail);
		}
	if ( !GetStringFromConfig(aSection, KMetaDataEntryValue(), metaDataEntryValue) )
		{
		ERR_PRINTF2(_L("No %S"), &tempStore);
		SetBlockResult(EFail);
		}
	CMMFMetaDataEntry*	metaDataEntry = NULL;
	TRAPD(err, metaDataEntry=CMMFMetaDataEntry::NewL(metaDataEntryName, metaDataEntryValue));
	CleanupStack::PushL(metaDataEntry);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("Cannot create CMMFMetaDataEntry - Error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{
		TRAPD(err, iRecorderUtility->AddMetaDataEntryL(*metaDataEntry));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("AddMetaDataEntryL Error %d"), err);
			SetError(err);
			}
		}
	CleanupStack::PopAndDestroy(metaDataEntry);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdAudioPlayControllerImplementationInformationL()
	{
	const CMMFControllerImplementationInformation*	controllerInfo = NULL;
	TRAPD(err, controllerInfo = &(iRecorderUtility->AudioPlayControllerImplementationInformationL()));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility->AudioPlayControllerImplementationInformationL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		CT_MMUtil::LogL(*this, *controllerInfo);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdAudioRecorderControllerImplementationInformationL()
	{
	const CMMFControllerImplementationInformation* controllerInfo = NULL;
	TRAPD(err, controllerInfo = &(iRecorderUtility->AudioRecorderControllerImplementationInformationL()));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility->AudioRecorderControllerImplementationInformationL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		CT_MMUtil::LogL(*this, *controllerInfo);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdCancelRegisterAudioResourceNotification(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TInt	inputUid;
	if ( GetHexFromConfig(aSection, KEventUID(), inputUid) )
		{
		TUid	id = TUid::Uid(inputUid);
		TInt	err = iRecorderUtility->CancelRegisterAudioResourceNotification(id);
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CancelRegisterAudioResourceNotification Error %d"), err);
			SetError(err);
			}
		else
			{
			INFO_PRINTF1(_L("CancelRegisterAudioResourceNotification OK."));
			}
		}
	else
		{
		ERR_PRINTF2(_L("NotificationEventUid not set in file %S"), &KEventUID);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdClearPlayWindow()
	{
	iRecorderUtility->ClearPlayWindow();
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdClose()
	{
	iRecorderUtility->Close();
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdCropFromBeginningL()
	{
	TRAPD(err, iRecorderUtility->CropFromBeginningL());
	if ( err==KErrNone )
		{
		INFO_PRINTF1(_L("File cropped form beginning"));
		}
	else
		{
		ERR_PRINTF2(_L("CropFromBeginningL error %S. Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdCropL()
	{
	TRAPD(err, iRecorderUtility->CropL());
	if ( err==KErrNone )
		{
		INFO_PRINTF1(_L("File cropped"));
		}
	else
		{
		ERR_PRINTF2(_L("CropL error. Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdDestinationBitRateL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								actualBitRate = 0;
	TRAPD(err, actualBitRate=iRecorderUtility->DestinationBitRateL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationBitRateL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Bit rate of the destination audio clip: %d."), actualBitRate);

		TInt	expectedBitRate;
		if (GetIntFromConfig(aSection, KBitRate(), expectedBitRate))
			{
			if (actualBitRate != expectedBitRate)
				{
				ERR_PRINTF3(_L("Destination bit rate actual(%d)!=expected(%d)"), actualBitRate, expectedBitRate);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdDestinationDataTypeL(const TDesC& aSection)
	{
	TFourCC	actualFourCC;
	TRAPD(err, actualFourCC=iRecorderUtility->DestinationDataTypeL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationDataTypeL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		TFourCC	expectedVal;
		if ( CT_MMUtil::ReadFourCC(*this, aSection, expectedVal) )
			{
			if ( expectedVal != actualFourCC )
				{
				ERR_PRINTF3(_L("dataType(%d) != expectedDataType(%d)"), actualFourCC.FourCC(), expectedVal.FourCC());
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdDestinationFormatL(const TDesC& aSection)
	{
	TUid	actualFormat;
	TRAPD(err, actualFormat = iRecorderUtility->DestinationFormatL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationFormatL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Format of the destination audio clip: %d."), actualFormat.iUid);
		TUid	expectedFormat	= TUid::Null();
		TInt    mmfUidFormat 	= 0;
		if (GetHexFromConfig(aSection, KFormat(), mmfUidFormat) )
			{
			expectedFormat=TUid::Uid(mmfUidFormat);
			INFO_PRINTF2(_L("Format from ini file: %d."), expectedFormat.iUid);
			}
			
		if (actualFormat != expectedFormat)
			{
			ERR_PRINTF3(_L("Format of the destination audio clip actual(%D)!=expected(%D)"), actualFormat.iUid, expectedFormat.iUid);
			SetBlockResult(EFail);
			}
		
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdDestinationNumberOfChannelsL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TInt	actualNrChnls = 0;
	TRAPD(err, actualNrChnls = iRecorderUtility->DestinationNumberOfChannelsL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationNumberOfChannelsL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Number of channels of the destination audio clip: %d."), actualNrChnls);

		TInt	expectedNrChnls;
		if (GetIntFromConfig(aSection, KChannels(), expectedNrChnls))
			{
			if (actualNrChnls != expectedNrChnls)
				{
				ERR_PRINTF3(_L("Destination number of channels actual(%d)!=expected(%d)"), actualNrChnls, expectedNrChnls);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdDestinationSampleRateL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								actualSampleRate = 0;
	TRAPD(err, actualSampleRate = iRecorderUtility->DestinationSampleRateL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationSampleRateL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Samplet rate of the destination audio clip: %d."), actualSampleRate);

		TInt	expectedSampleRate=0;
		if (GetIntFromConfig(aSection, KSampleRate(), expectedSampleRate))
			{
			if (actualSampleRate != expectedSampleRate)
				{
				ERR_PRINTF3(_L("Destination sample rate actual(%d)!=expected(%d)"), actualSampleRate, expectedSampleRate);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdDuration(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TTimeIntervalMicroSeconds  			actualDuration;
	TInt 								expectedDuration=0;

	//	Read from INI file which function of Duration we can use.
	//	If parameter "command_usestate" is = TRUE then use Duration2
	TBool	useState=EFalse;
	GetBoolFromConfig(aSection, KUseState(), useState);

	if( !useState )
		{
		actualDuration = iRecorderUtility->Duration();
		INFO_PRINTF2(_L("Duration = %d (MicroSec)"), actualDuration.Int64());

		if( GetIntFromConfig(aSection, KDuration(), expectedDuration) )
			{
			if( expectedDuration != actualDuration.Int64() )
				{
	        	ERR_PRINTF3(_L("expectedDuration (%d) != actualDuration (%d)"), expectedDuration, actualDuration.Int64());
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		TMMFDurationInfo	actualDurationInfo = iRecorderUtility->Duration(actualDuration);
		switch (actualDurationInfo)
			{
		case EMMFDurationInfoValid:
			INFO_PRINTF2(_L("Duration = %d (MicroSec)"), actualDuration.Int64());
			break;
		case EMMFDurationInfoUnknown:
			INFO_PRINTF1(_L("Duration length is unknown"));
			break;
		case EMMFDurationInfoInfinite:
			INFO_PRINTF1(_L("Clip is of infinite length"));
			break;
		default:
			break;
			}

		TPtrC	expectedDurationState;
		if( GetStringFromConfig(aSection, KDurationState(), expectedDurationState) )
			{
			TMMFDurationInfo	expectedDurationInfo = EMMFDurationInfoValid;

			if( expectedDurationState == KEMMFDurationInfoValid )
				{
				expectedDurationInfo = EMMFDurationInfoValid;
				}
			else if( expectedDurationState == KEMMFDurationInfoUnknown )
				{
				expectedDurationInfo = EMMFDurationInfoUnknown;
				}
			else if( expectedDurationState == KEMMFDurationInfoInfinite )
				{
				expectedDurationInfo = EMMFDurationInfoInfinite;
				}
			else
				{
	        	ERR_PRINTF2(_L("Invalid expected duration state (%S)"), &expectedDurationState);
				SetBlockResult(EFail);
				}

			if( expectedDurationInfo != actualDurationInfo )
				{
	        	ERR_PRINTF2(_L("expectedDurationInfo (%S) != actualDurationInfo"), &expectedDurationState);
				SetBlockResult(EFail);
				}
			}

		if( GetIntFromConfig(aSection, KDuration(), expectedDuration) )
			{
			if( expectedDuration != actualDuration.Int64() )
				{
	        	ERR_PRINTF3(_L("expectedDuration (%d) != actualDuration (%d)"), expectedDuration, actualDuration.Int64());
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetAudioLoadingProgressL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								loadingprogress=0;
	TRAPD(err, iRecorderUtility->GetAudioLoadingProgressL(loadingprogress));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetAudioLoadingProgressL Error %d"), err);
		SetError(err);
		}
	else
		{
		TInt	expectedLoadingProgress=0;
		if ( GetIntFromConfig(aSection, KLoadingProgress(), expectedLoadingProgress) )
			{
			if( loadingprogress !=  expectedLoadingProgress)
				{
				ERR_PRINTF3(_L("Current loadingprogress (%d) != expected loadingprogress (%d)"), loadingprogress, expectedLoadingProgress);
				SetBlockResult(EFail);
				}
			}
		INFO_PRINTF2(_L("AudioLoadingProgress=%d"), loadingprogress);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetDRMCustomCommand(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TBool				 				isNull=EFalse;

	GetBoolFromConfig(aSection, KExpectNull(), isNull);
	MMMFDRMCustomCommand*	drmcustomcommand = iRecorderUtility->GetDRMCustomCommand();
	if( ( drmcustomcommand!=NULL && isNull ) || ( drmcustomcommand==NULL && !isNull ) )
		{
		ERR_PRINTF1(_L("expected DRMCustomCommand value != GetDRMCustomCommand value"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetGain(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt 								gain=0;
	TInt err=iRecorderUtility->GetGain(gain);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::GetGain Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Gain=%d"), gain);
		TInt	expectedGain=0;
		if(GetIntFromConfig(aSection, KGain(), expectedGain))
			{
			TInt	maxGain=iRecorderUtility->MaxGain();
			if(expectedGain>maxGain)
				{
				expectedGain=maxGain;
				}
			if(expectedGain == -1)
				{
				// Expect to get the maximum gain
				if(gain != maxGain)
					{
					SetBlockResult(EFail);
					ERR_PRINTF3(_L("gain(%d) != maxGain(%d)"), gain, maxGain);
					}
				}
			else if(gain != expectedGain)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("gain(%d) != expectedGain(%d)"), gain, expectedGain);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetMetaDataEntryL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TInt	metaDataEntryIndex=0;
	GetIntFromConfig(aSection, KMetaDataEntryIndex(), metaDataEntryIndex);

	CMMFMetaDataEntry*	metaDataEntry=NULL;
	TRAPD(err, metaDataEntry=iRecorderUtility->GetMetaDataEntryL(metaDataEntryIndex));
	CleanupStack::PushL(metaDataEntry);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MetaDataEntryL Error %d"), err);
		SetError(err);
		}
	else
		{
		const TDesC&	name=metaDataEntry->Name();
		const TDesC&	value=metaDataEntry->Value();
		INFO_PRINTF3(_L("GetMetaDataEntryL: Name = %S, value = %S"), &name, &value);
		TPtrC	expectedName;
		if ( GetStringFromConfig(aSection, KMetaDataEntryName(), expectedName) )
			{
			if(!name.Compare(expectedName))
				{
				ERR_PRINTF3(_L("name(%S) != expectedName(%S)"), &name, &expectedName);
				SetBlockResult(EFail);
				}
			}
		TPtrC	expectedValue;
		if ( GetStringFromConfig(aSection, KMetaDataEntryValue(), expectedValue) )
			{
			if(!value.Compare(expectedValue))
				{
				ERR_PRINTF3(_L("value(%S) != expectedValue(%S)"), &value, &expectedValue);
				SetBlockResult(EFail);
				}
			}
		}
	CleanupStack::PopAndDestroy(metaDataEntry);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetNumberOfMetaDataEntries(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								numberOfMetaDataEntries=0;
	TInt								err = iRecorderUtility->GetNumberOfMetaDataEntries(numberOfMetaDataEntries);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetNumberOfMetaDataEntries Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("GetNumberOfMetaDataEntries=%d"), numberOfMetaDataEntries);
		TInt	expectedNumberOfMetaDataEntries=0;
		if(GetIntFromConfig(aSection, KNoOfMetaDataEntries(), expectedNumberOfMetaDataEntries))
			{
			if(numberOfMetaDataEntries != expectedNumberOfMetaDataEntries)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("numberOfMetaDataEntries(%d) != expectedNumberOfMetaDataEntries(%d)"), numberOfMetaDataEntries, expectedNumberOfMetaDataEntries);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetPlaybackBalance(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								balance;
	TInt								err=iRecorderUtility->GetPlaybackBalance(balance);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetPlaybackBalanceL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("GetPlaybackBalanceL=%d"), balance);
		TInt	expectedBalance=0;
		if(GetIntFromConfig(aSection, KBalance(), expectedBalance))
			{
			if(balance != expectedBalance)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("Playbackbalance(%d) != expectedPlaybackBalance(%d)"), balance, expectedBalance);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetRecordBalance(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								balance;
	TRAPD(err, iRecorderUtility->GetRecordBalance(balance));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetRecordBalance Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("GetRecordBalance=%d"), balance);
		TInt	expectedBalance=0;
		if(GetIntFromConfig(aSection, KBalance(), expectedBalance))
			{
			if(balance != expectedBalance)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("Recordbalance(%d) != expectedRecordBalance(%d)"), balance, expectedBalance);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetSupportedBitRatesL(const TDesC& aSection)
	{
	RArray<TUint>	rates;
	CleanupClosePushL(rates);
	TRAPD(err, iRecorderUtility->GetSupportedBitRatesL(rates));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetSupportedBitRatesL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported bit rates for the destination:"));
		for ( TInt index=0; index<rates.Count(); ++index )
			{
			INFO_PRINTF3(_L("rates[%d]=%d"), index, rates[index]);
			}
		TBuf<KMaxTestExecuteCommandLength>	tempStore;
		TInt								expectedBitRatesNr;
		if (GetIntFromConfig(aSection, KExcpectedNumber(), expectedBitRatesNr))
			{
			if (rates.Count() != expectedBitRatesNr)
				{
				ERR_PRINTF3(_L("Number of supported bit rates actual(%d)!=expected(%d)"), rates.Count(), expectedBitRatesNr);
				SetBlockResult(EFail);
				}
			}
		}
	CleanupStack::PopAndDestroy(&rates);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetSupportedDestinationDataTypesL(const TDesC& aSection)
	{
	RArray<TFourCC>	types;
	CleanupClosePushL(types);
	TRAPD(err, iRecorderUtility->GetSupportedDestinationDataTypesL(types));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetSupportedDestinationDataTypesL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported destination data types for recording:"));
		for ( TInt index=0; index<types.Count(); ++index )
			{
			INFO_PRINTF3(_L("types[%d]=%S"), index, &CT_MMUtil::GetFourCCString(types[index].FourCC()));
			}
		TBuf<KMaxTestExecuteCommandLength>	tempStore;
		TInt								expectedDatatypesNr;
		if (GetIntFromConfig(aSection, KExcpectedNumber(), expectedDatatypesNr))
			{
			if (types.Count() != expectedDatatypesNr)
				{
				ERR_PRINTF3(_L("Number of supported datatypes actual(%d)!=expected(%d)"), types.Count(), expectedDatatypesNr);
				SetBlockResult(EFail);
				}
			}
		}
	CleanupStack::PopAndDestroy(&types);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetSupportedNumberOfChannelsL(const TDesC& aSection)
	{
	RArray<TUint>	channels;
	CleanupClosePushL(channels);
	TRAPD(err, iRecorderUtility->GetSupportedNumberOfChannelsL(channels));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetSupportedNumberOfChannelsL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported number of channels for the destination:"));
		for ( TInt index=0; index<channels.Count(); ++index )
			{
			INFO_PRINTF3(_L("channels[%d]=%d"), index, channels[index]);
			}
		TBuf<KMaxTestExecuteCommandLength>	tempStore;
		TInt								expectedNrOfChnlsNr=0;
		if (GetIntFromConfig(aSection, KExcpectedNumber(), expectedNrOfChnlsNr))
			{
			if (channels.Count() != expectedNrOfChnlsNr)
				{
				ERR_PRINTF3(_L("Number of supported number of channels actual(%d)!=expected(%d)"), channels.Count(), expectedNrOfChnlsNr);
				SetBlockResult(EFail);
				}
			}
		}
	CleanupStack::PopAndDestroy(&channels);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetSupportedSampleRatesL(const TDesC& aSection)
	{
	RArray<TUint>	rates;
	CleanupClosePushL(rates);
	TRAPD(err, iRecorderUtility->GetSupportedSampleRatesL(rates));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetSupportedSampleRatesL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported sample rates for the destination:"));
		for ( TInt index=0; index<rates.Count(); ++index )
			{
			INFO_PRINTF3(_L("rates[%d]=%d"), index, rates[index]);
			}
		TBuf<KMaxTestExecuteCommandLength>	tempStore;
		TInt								expectedSampleRatesNr=0;
		if (GetIntFromConfig(aSection, KExcpectedNumber(), expectedSampleRatesNr))
			{
			if (rates.Count() != expectedSampleRatesNr)
				{
				ERR_PRINTF3(_L("Number of supported sample rates actual(%d)!=expected(%d)"), rates.Count(), expectedSampleRatesNr);
				SetBlockResult(EFail);
				}
			}
		}
	CleanupStack::PopAndDestroy(&rates);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdGetVolume(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt 								actualVolume=0;
	TInt err=iRecorderUtility->GetVolume(actualVolume);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::GetVolume Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Volume=%d"), actualVolume);
		}
	TInt	expectedVolume=0;
	if(GetIntFromConfig(aSection, KVolume(), expectedVolume))
		{
		TInt	maxVol=iRecorderUtility->MaxVolume();
		if ( expectedVolume>maxVol )
			{
			expectedVolume=maxVol;
			}
		if(actualVolume != expectedVolume)
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("actualVolume(%d) != expectedVolume(%d)"), actualVolume, expectedVolume);
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdMaxGain(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	maxGain=iRecorderUtility->MaxGain();
	INFO_PRINTF2(_L("MaxGain=%d"), maxGain);
	TInt	expectedGain=0;
	if(GetIntFromConfig(aSection, KGain(), expectedGain))
		{
		if(maxGain != expectedGain)
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("maxGain(%d) != expectedGain(%d)"), maxGain, expectedGain);
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdMaxVolume(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								maxVolume=iRecorderUtility->MaxVolume();
	INFO_PRINTF2(_L("MaxVolume=%d"), maxVolume);

	TInt	expectedVolume=0;
	if(GetIntFromConfig(aSection, KVolume(), expectedVolume))
		{
		if(maxVolume != expectedVolume)
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("maxVolume(%d) != expectedVolume(%d)"), maxVolume, expectedVolume);
			}
		}
	}

void	CT_MMDataMdaAudioRecorderUtility::DoCmdNewL(const TDesC& aSection)
	{
	delete iRecorderUtility;
	iRecorderUtility=NULL;

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TInt	priority;
	if ( CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		TRAPD(err, iRecorderUtility=CMdaAudioRecorderUtility::NewL(*this, NULL, priority, priorityPreference));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("NewL(this, priority, priorityPreference) Error %d"), err);
			SetError(err);
			}
		}
	else
		{
		TRAPD(err, iRecorderUtility=CMdaAudioRecorderUtility::NewL(*this));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("NewL(this) Error %d"), err);
			SetError(err);
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdOpenDesL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	iBufferSize=KMdaAudioRecorderBufferSize;

	TPtrC	filename;
    TBool     hasFile=GetStringFromConfig(aSection, KFileName(), filename);
    if ( hasFile )
		{
		if ( iFileOpened )
			{
    		iFile.Close();
    		iFileOpened=EFalse;
    		}
    	User::LeaveIfError(iFile.Open(iFs, filename, EFileShareReadersOnly));
    	iFileOpened=ETrue;
    	User::LeaveIfError(iFile.Size(iBufferSize));
    	}
    GetIntFromConfig(aSection, KBufferSize(), iBufferSize);
    delete iBuffer;
    iBuffer=HBufC8::NewL(iBufferSize);
    *iBuffer=KNullDesC8;
    delete iBufferPtr;
    iBufferPtr=new (ELeave) TPtr8(iBuffer->Des());
    if ( hasFile )
      	 {
         INFO_PRINTF1(_L("Reading audio clip to buffer"));
         User::LeaveIfError(iFile.Read(*iBufferPtr));
         }

	TBool	isConst=EFalse;
	GetBoolFromConfig(aSection, KConst(), isConst);

	CT_MMUtil::GetControllerAndFormatUidL(*this, filename, KUidMediaTypeAudio, ETrue, iRecorderControllerUid, iRecorderFormatUid);
	TInt	recordControllerUid;
	if ( GetHexFromConfig(aSection, KControllerUid(), recordControllerUid) )
		{
		iRecorderControllerUid=TUid::Uid(recordControllerUid);
		}

	CT_MMUtil::GetControllerAndFormatUidL(*this, filename, KUidMediaTypeAudio, EFalse, iPlayerControllerUid, iPlayerFormatUid);
	TInt	playbackControllerUid;
	if ( GetHexFromConfig(aSection, KControllerUid(), playbackControllerUid) )
		{
		iPlayerControllerUid=TUid::Uid(playbackControllerUid);
		}

	TUid	destinationFormatUid = TUid::Null();
	TInt	formatUid 			 = 0;
	if (GetHexFromConfig(aSection, KFormat(), formatUid))
		{
		destinationFormatUid=TUid::Uid(formatUid);
		INFO_PRINTF3(_L("Format input required: %d. and the format read in from ini file: %d."),destinationFormatUid.iUid,formatUid);
		}

	TFourCC	audioTypeFourCC;
	if ( CT_MMUtil::ReadFourCC(*this, aSection, audioTypeFourCC) )
		{
		iAsyncErrorIndex = aAsyncErrorIndex;
		TRAPD(err,iRecorderUtility->OpenDesL(*iBufferPtr, iRecorderControllerUid, iPlayerControllerUid, destinationFormatUid, audioTypeFourCC));
		if ( err==KErrNone )
			{
			INFO_PRINTF1(_L("Buffer opened"));
			IncOutstanding();
			}
		else
			{
			ERR_PRINTF2(_L("OpenDesL Error %d"), err);
			SetError(err);
			}
		}
	else if(isConst)
		{
		iAsyncErrorIndex = aAsyncErrorIndex;
		const TPtrC8	tempStr=*iBufferPtr;
		TRAPD(err, iRecorderUtility->OpenDesL(tempStr));
		if ( err==KErrNone )
			{
			INFO_PRINTF1(_L("Buffer opened"));
			IncOutstanding();
			}
		else
			{
			ERR_PRINTF2(_L("OpenDesL Error %d"), err);
			SetError(err);
			}
		}
	else
		{
		iAsyncErrorIndex = aAsyncErrorIndex;
		TRAPD(err, iRecorderUtility->OpenDesL(*iBufferPtr));
		if ( err==KErrNone )
			{
			INFO_PRINTF1(_L("Buffer opened"));
			IncOutstanding();
			}
		else
			{
			ERR_PRINTF2(_L("OpenDesL Error %d"), err);
			SetError(err);
			}
		}
	}

void  CT_MMDataMdaAudioRecorderUtility::DoCmdOpenFileL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TPtrC	filename;
	if ( GetStringFromConfig(aSection, KFileName(), filename) )
		{
		TPtrC	fourCC;
		TBool	useRFile = EFalse;
		GetBoolFromConfig(aSection, KRFile(), useRFile);

		TBool	useTMMSource = EFalse;
		GetBoolFromConfig(aSection, KTMMSource(), useTMMSource);

		CT_MMUtil::GetControllerAndFormatUidL(*this, _L(".wav"), KUidMediaTypeAudio, ETrue, iRecorderControllerUid, iRecorderFormatUid);
		TInt	recordControllerUid;
		if ( GetHexFromConfig(aSection, KControllerUid(), recordControllerUid) )
			{
			iRecorderControllerUid=TUid::Uid(recordControllerUid);
			}

		CT_MMUtil::GetControllerAndFormatUidL(*this, filename, KUidMediaTypeAudio, EFalse, iPlayerControllerUid, iPlayerFormatUid);
		TInt	playbackControllerUid;
		if ( GetHexFromConfig(aSection, KControllerUid(), playbackControllerUid) )
			{
			iPlayerControllerUid=TUid::Uid(playbackControllerUid);
			}

		TUid	destinationFormatUid = TUid::Null();
		TInt	formatUid 			 = 0;
		if (GetHexFromConfig(aSection, KFormat(), formatUid))
			{
			destinationFormatUid=TUid::Uid(formatUid);
			INFO_PRINTF3(_L("Format input required: %d, and the format read in from the ini file: %d"),formatUid,destinationFormatUid.iUid);
			}

		TFourCC	dataType;
		TBool	hasFourCC = CT_MMUtil::ReadFourCC(*this, aSection, dataType);

		TBool	isNew=EFalse;
		GetBoolFromConfig(aSection, KNew(), isNew);

		if ( useRFile )
			{
			if( iFileOpened )
				{
				iFile.Close();
				iFileOpened=EFalse;
				}
			if(isNew)
				{
				FileServer().MkDirAll(filename);
				FileServer().Delete(filename);
				User::LeaveIfError(iFile.Create(FileServer(), filename, EFileWrite));
				}
			else
				{
				User::LeaveIfError(iFile.Open(FileServer(), filename, EFileShareReadersOnly));
				}
			if (hasFourCC)
				{
				iAsyncErrorIndex = aAsyncErrorIndex;
				TRAPD(err, iRecorderUtility->OpenFileL(iFile, iRecorderControllerUid, iPlayerControllerUid, destinationFormatUid, dataType ));
				if ( err==KErrNone )
					{
					INFO_PRINTF1(_L("File opened"));
					IncOutstanding();
					}
				else
					{
					ERR_PRINTF3(_L("Cannot open rfile %S. Error %d"), &filename, err);
					SetError(err);
					}
				}
			else
				{
				iAsyncErrorIndex = aAsyncErrorIndex;
				TRAPD(err, iRecorderUtility->OpenFileL(iFile));
				if ( err==KErrNone )
					{
					INFO_PRINTF1(_L("File created"));
					IncOutstanding();
					}
				else
					{
					ERR_PRINTF3(_L("Cannot open rfile %S. Error %d"), &filename, err);
					SetError(err);
					}
				}
			}
		else if ( useTMMSource )
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

		    TMMFileSource	fileSource(filename, uniqueId, intent, enableUI);
			if (hasFourCC)
				{
				iAsyncErrorIndex = aAsyncErrorIndex;
				TRAPD(err, iRecorderUtility->OpenFileL(fileSource, iRecorderControllerUid, iPlayerControllerUid, destinationFormatUid, dataType ));
				if ( err==KErrNone )
					{
					INFO_PRINTF1(_L("File opened"));
					IncOutstanding();
					}
				else
					{
					ERR_PRINTF3(_L("Cannot open tmmsource %S. Error %d"), &filename, err);
					SetError(err);
					}
				}
			else
				{
				iAsyncErrorIndex = aAsyncErrorIndex;
				TRAPD(err, iRecorderUtility->OpenFileL(fileSource));
				if ( err==KErrNone )
					{
					INFO_PRINTF1(_L("File created"));
					IncOutstanding();
					}
				else
					{
					ERR_PRINTF3(_L("Cannot open tmmsource %S. Error %d"), &filename, err);
					SetError(err);
					}
				}
			}
		else
			{
			if(isNew)
				{
				FileServer().MkDirAll(filename);
				FileServer().Delete(filename);
				}
			if (hasFourCC)
				{
				iAsyncErrorIndex = aAsyncErrorIndex;
				TRAPD(err, iRecorderUtility->OpenFileL(filename, iRecorderControllerUid, iPlayerControllerUid, destinationFormatUid, dataType));
				if ( err==KErrNone )
					{
					INFO_PRINTF1(_L("File created"));
					IncOutstanding();
					}
				else
					{
					ERR_PRINTF3(_L("Cannot open tdesc %S. Error %d"), &filename, err);
					SetError(err);
					}
				}
			else
				{
				iAsyncErrorIndex = aAsyncErrorIndex;
				TRAPD(err, iRecorderUtility->OpenFileL(filename));
				if ( err==KErrNone )
					{
					INFO_PRINTF1(_L("File opened"));
					IncOutstanding();
					}
				else
					{
					ERR_PRINTF3(_L("Cannot open tdesc %S. Error %d"), &filename, err);
					SetError(err);
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("No %S"), &tempStore);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdOpenL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	// read location from INI file
	TMdaFileClipLocation	location;
	TPtrC					locationStr;
	if (GetStringFromConfig(aSection, KFileName(), locationStr))
		{
		location.iName = locationStr;
		}
	else
		{
		ERR_PRINTF2(_L("Can not call OpenL. Parameter %S has invalid value in INI file."), &tempStore);
		SetBlockResult(EFail);
		}

	// read format of the audio clip from INI file
	TMdaClipFormat*	format = NULL;
	TPtrC			formatStr;
	if (GetStringFromConfig(aSection, KAudioFormat(), formatStr))
		{
		if(GetFormatL(formatStr, format, aSection))
			{
			INFO_PRINTF3(_L("Format (%S) Uid = %d."), &formatStr, format->Uid());
			}
		else
			{
			ERR_PRINTF2(_L("Can not call OpenL. Parameter %S has invalid value in INI file."), &tempStore);
			SetBlockResult(EFail);
			}
		}
	CleanupDeletePushL(format);

	// read codec type from INI file
	TMdaPackage*	codec = NULL;
	TPtrC			codecStr;
	if (GetStringFromConfig(aSection, KCodec(), codecStr))
		{
		if(GetCodecL(codecStr, codec))
			{
			INFO_PRINTF3(_L("Codec (%S) Uid = %d."), &codecStr, codec->Uid());
			}
		else
			{
			ERR_PRINTF2(_L("Can not call OpenL. Parameter %S has invalid value in INI file."), &tempStore);
			SetBlockResult(EFail);
			}
		}
	CleanupDeletePushL(codec);

	// read audio settings from INI file
	TMdaPackage*	settings = NULL;
	GetAudioSettingsL(KAudioSettings(), aSection, settings);
	CleanupDeletePushL(settings);

	iAsyncErrorIndex = aAsyncErrorIndex;

	TRAPD(err, iRecorderUtility->OpenL(&location, format, codec, settings));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility:::OpenL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		IncOutstanding();
		}

	CleanupStack::PopAndDestroy(3, format); // @param TMdaPackage *aFormat, *aArg1, *aArg2
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdOpenUrlL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TPtrC	url;

	if ( GetStringFromConfig(aSection, KTDesC(), url) )
		{
		TInt	accessPointID = KUseDefaultIap;
		GetIntFromConfig(aSection, KAccessPointID(), accessPointID);

		TFourCC	destinationDataType;
		if ( CT_MMUtil::ReadFourCC(*this, aSection, destinationDataType) )
			{
			CT_MMUtil::GetControllerAndFormatUidL(*this, _L(".wav"), KUidMediaTypeAudio, ETrue, iRecorderControllerUid, iRecorderFormatUid);
			TInt	recordControllerUid;
			if ( GetHexFromConfig(aSection, KControllerUid(), recordControllerUid) )
				{
				iRecorderControllerUid=TUid::Uid(recordControllerUid);
				}

			CT_MMUtil::GetControllerAndFormatUidL(*this, url, KUidMediaTypeAudio, EFalse, iPlayerControllerUid, iPlayerFormatUid);
			TInt	playbackControllerUid;
			if ( GetHexFromConfig(aSection, KControllerUid(), playbackControllerUid) )
				{
				iPlayerControllerUid=TUid::Uid(playbackControllerUid);
				}

			TUid	destinationFormatUid = TUid::Null();
			TInt	formatUid  			 = 0;
			if (GetHexFromConfig(aSection, KFormat(), formatUid))
				{
				destinationFormatUid=TUid::Uid(formatUid);
				INFO_PRINTF3(_L("Format input required: %d, and the format read in from ini file: %d."),destinationFormatUid.iUid,formatUid);
				}

			TRAPD(err, iRecorderUtility->OpenUrlL(url, accessPointID, iRecorderControllerUid, iPlayerControllerUid, destinationFormatUid, destinationDataType));
			if ( err==KErrNone )
				{
				INFO_PRINTF1(_L("File opened from url"));
				}
			else
				{
				ERR_PRINTF3(_L("Cannot open url %S. Error %d"), &url, err);
				SetError(err);
				}
			}
		else
			{
			TPtrC	mimeTypeStr;
			GetStringFromConfig(aSection, KMimeType(), mimeTypeStr);

			HBufC8*	buffer=HBufC8::NewLC(mimeTypeStr.Length());
			TPtr8	bufferPtr=buffer->Des();
			bufferPtr.Copy(mimeTypeStr);	// Create a 16 bit copy of the 8 bit original

			const TDesC8&	mimeType=bufferPtr;
			TRAPD(err, iRecorderUtility->OpenUrlL(url, accessPointID, mimeType));
			if ( err==KErrNone )
				{
				INFO_PRINTF1(_L("File opened from url"));
				}
			else
				{
				ERR_PRINTF3(_L("Cannot open url %S. Error %d"), &url, err);
				SetError(err);
				}
			CleanupStack::PopAndDestroy(buffer);
			}
		}
	else
		{
		ERR_PRINTF1(_L("Can not call OpenUrlL. Wrong parameters in INI file."));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdPlayControllerCustomCommandAsyncL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TMMFMessageDestination		handleInfo(iPlayerControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	TInt	function = 0;
	GetIntFromConfig(aSection, KFunction(), function);

	TBool	useDataFrom = EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), useDataFrom);

	// reference to the first chunk of data
	HBufC8*	dataTo1 = NULL;
	TPtrC	dataTo1Str;
	if ( GetStringFromConfig(aSection, KDataTo1(), dataTo1Str) )
		{
		dataTo1 = HBufC8::NewL(dataTo1Str.Length());
		dataTo1->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo1 = HBufC8::NewL(0);
		dataTo1->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo1);

	// reference to the second chunk of data
	HBufC8*	dataTo2 = NULL;
	TPtrC	dataTo2Str;
	if ( GetStringFromConfig(aSection, KDataTo2(), dataTo2Str) )
		{
		dataTo2 = HBufC8::NewL(dataTo2Str.Length());
		dataTo2->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo2 = HBufC8::NewL(0);
		dataTo2->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo2);

	if ( useDataFrom )
		{
		iCustomCommandData = ETrue;
		iRecorderUtility->PlayControllerCustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iDataFromText, iActiveCustomCommandAsync->iStatus);
		}
	else
		{
		iRecorderUtility->PlayControllerCustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iActiveCustomCommandAsync->iStatus);
		}

	iActiveCustomCommandAsync->Activate(aAsyncErrorIndex);
	IncOutstanding();
	CleanupStack::PopAndDestroy(2, dataTo1);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdPlayControllerCustomCommandSyncL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TMMFMessageDestination		handleInfo(iPlayerControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	TInt	function = 0;
	GetIntFromConfig(aSection, KFunction(), function);

	TBool	useDataFrom = EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), useDataFrom);

	// reference to the first chunk of data
	HBufC8*	dataTo1 = NULL;
	TPtrC	dataTo1Str;
	if (GetStringFromConfig(aSection, KDataTo1(), dataTo1Str))
		{
		dataTo1 = HBufC8::NewL(dataTo1Str.Length());
		dataTo1->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo1 = HBufC8::NewL(0);
		dataTo1->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo1);

	// reference to the second chunk of data
	HBufC8*	dataTo2 = NULL;
	TPtrC	dataTo2Str;
	if (GetStringFromConfig(aSection, KDataTo2(), dataTo2Str))
		{
		dataTo2 = HBufC8::NewL(dataTo2Str.Length());
		dataTo2->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo2 = HBufC8::NewL(0);
		dataTo2->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo2);

	TInt					err = KErrNone;
	TBuf8<KMaxdataFromText>	dataFromText;
	if (useDataFrom)
		{
		err = iRecorderUtility->PlayControllerCustomCommandSync(messageDest, function, *dataTo1, *dataTo2, dataFromText);
		}
	else
		{
		err = iRecorderUtility->PlayControllerCustomCommandSync(messageDest, function, *dataTo1, *dataTo2);
		}
	if (err!=KErrNone)
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::PlayControllerCustomCommandSync failed with error %d."), err);
		SetError(err);
		}
	else
		{
		if (useDataFrom)
			{
			INFO_PRINTF2(_L("DataFrom: %S"), &dataFromText);
			}
		}

	CleanupStack::PopAndDestroy(2, dataTo1);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdPlayL(const TInt aAsyncErrorIndex)
	{
	iAsyncErrorIndex = aAsyncErrorIndex;
	TRAPD(err, iRecorderUtility->PlayL());
	if ( err==KErrNone )
		{
		INFO_PRINTF1(_L("Started playing"));
		IncOutstanding();
		}
	else
		{
		ERR_PRINTF2(_L("Cannot start playing %S. Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdPosition(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TTimeIntervalMicroSeconds			actualPosition = iRecorderUtility->Position();
	INFO_PRINTF2(_L("Current position in the audio clip: %d"), actualPosition.Int64());

	TInt	expectedPosition=0;
	if (GetIntFromConfig(aSection, KPosition(), expectedPosition))
		{
		if (actualPosition.Int64() != expectedPosition)
			{
			ERR_PRINTF3(_L("Position in the audio clip actual(%d)!=expected(%d)"),
				actualPosition.Int64(), expectedPosition);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdRecordControllerCustomCommandAsyncL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TMMFMessageDestination		handleInfo(iRecorderControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	TInt	function = 0;
	GetIntFromConfig(aSection, KFunction(), function);

	TBool	useDataFrom = EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), useDataFrom);

	// reference to the first chunk of data
	HBufC8*	dataTo1 = NULL;
	TPtrC	dataTo1Str;
	if ( GetStringFromConfig(aSection, KDataTo1(), dataTo1Str) )
		{
		dataTo1 = HBufC8::NewL(dataTo1Str.Length());
		dataTo1->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo1 = HBufC8::NewL(0);
		dataTo1->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo1);

	// reference to the second chunk of data
	HBufC8*	dataTo2 = NULL;
	TPtrC	dataTo2Str;
	if ( GetStringFromConfig(aSection, KDataTo2(), dataTo2Str) )
		{
		dataTo2 = HBufC8::NewL(dataTo2Str.Length());
		dataTo2->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo2 = HBufC8::NewL(0);
		dataTo2->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo2);

	if ( useDataFrom )
		{
		iCustomCommandData = ETrue;
		iRecorderUtility->RecordControllerCustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iDataFromText, iActiveCustomCommandAsync->iStatus);
		}
	else
		{
		iRecorderUtility->RecordControllerCustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iActiveCustomCommandAsync->iStatus);
		}

	iActiveCustomCommandAsync->Activate(aAsyncErrorIndex);
	IncOutstanding();
	CleanupStack::PopAndDestroy(2, dataTo1);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdRecordControllerCustomCommandSyncL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TMMFMessageDestination		handleInfo(iRecorderControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	TInt	function = 0;
	GetIntFromConfig(aSection, KFunction(), function);

	TBool	useDataFrom = EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), useDataFrom);

	// reference to the first chunk of data
	HBufC8*	dataTo1 = NULL;
	TPtrC	dataTo1Str;
	if (GetStringFromConfig(aSection, KDataTo1(), dataTo1Str))
		{
		dataTo1 = HBufC8::NewL(dataTo1Str.Length());
		dataTo1->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo1 = HBufC8::NewL(0);
		dataTo1->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo1);

	// reference to the second chunk of data
	HBufC8*	dataTo2 = NULL;
	TPtrC	dataTo2Str;
	if (GetStringFromConfig(aSection, KDataTo2(), dataTo2Str))
		{
		dataTo2 = HBufC8::NewL(dataTo2Str.Length());
		dataTo2->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo2 = HBufC8::NewL(0);
		dataTo2->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo2);

	TInt					err = KErrNone;
	TBuf8<KMaxdataFromText>	dataFromText;
	if (useDataFrom)
		{
		err = iRecorderUtility->RecordControllerCustomCommandSync(messageDest, function, *dataTo1, *dataTo2, dataFromText);
		}
	else
		{
		err = iRecorderUtility->RecordControllerCustomCommandSync(messageDest, function, *dataTo1, *dataTo2);
		}
	if (err!=KErrNone)
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::RecordControllerCustomCommandSync failed with error %d."), err);
		SetError(err);
		}
	else
		{
		if (useDataFrom)
			{
			INFO_PRINTF2(_L("DataFrom: %S"), &dataFromText);
			}
		}

	CleanupStack::PopAndDestroy(2, dataTo1);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdRecordL(const TInt aAsyncErrorIndex)
	{
	iAsyncErrorIndex = aAsyncErrorIndex;
	TRAPD(err, iRecorderUtility->RecordL());
	if ( err==KErrNone )
		{
		INFO_PRINTF1(_L("Started recording"));
		iRecording=ETrue;
		IncOutstanding();
		}
	else
		{
		ERR_PRINTF2(_L("Cannot start recording %S. Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdRecordTimeAvailable(const TDesC& aSection)
	{
	TTimeIntervalMicroSeconds			recTime = iRecorderUtility->RecordTimeAvailable();
	INFO_PRINTF2(_L("Amount of recording time available to the current clip: %d"), recTime.Int64());

	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TInt	expectedRecTime=0;
	if (GetIntFromConfig(aSection, KRecTime(), expectedRecTime))
		{
		if (recTime.Int64() != expectedRecTime)
			{
			ERR_PRINTF3(_L("Record time available actual(%d)!=expected(%d)"),
				recTime.Int64(), expectedRecTime);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdRegisterAudioResourceNotificationL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	// Notification registration specific data
	HBufC8*	dataNotificationRegistration = NULL;
	TPtrC	dataNotificationRegistrationStr;
	if (GetStringFromConfig(aSection, KDataNotification(), dataNotificationRegistrationStr))
		{
		dataNotificationRegistration = HBufC8::NewL(dataNotificationRegistrationStr.Length());
		dataNotificationRegistration->Des().Copy(dataNotificationRegistrationStr);
		}
	else
		{
		dataNotificationRegistration = HBufC8::NewL(0);
		dataNotificationRegistration->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataNotificationRegistration);

	TInt	inputUid;
	if(GetHexFromConfig(aSection, KEventUID(), inputUid))
		{
		ERR_PRINTF2(_L("Read input id %d"), inputUid);

		TUid	id = TUid::Uid(inputUid);

		TInt	err = iRecorderUtility->RegisterAudioResourceNotification(*this, id, *dataNotificationRegistration);
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("RegisterAudioResourceNotification Error %d"), err);
			SetError(err);
			}
		else
			{
			INFO_PRINTF2(_L("RegisterAudioResourceNotification OK, dataNotification = %s"), &dataNotificationRegistration);
			}
		}
	else
		{
		ERR_PRINTF2(_L("NotificationEventUid not set in ini file %S"), &KEventUID);
		SetBlockResult(EFail);
		}
	CleanupStack::PopAndDestroy(1, dataNotificationRegistration);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdRegisterForAudioLoadingNotification()
	{
	iRecorderUtility->RegisterForAudioLoadingNotification(*this);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdRemoveMetaDataEntry(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	metaDataEntryID=0;
	GetIntFromConfig(aSection, KMetaDataEntryID(), metaDataEntryID);
	TInt	err=iRecorderUtility->RemoveMetaDataEntry(metaDataEntryID);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RemoveMetaDataEntryL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdReplaceMetaDataEntryL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TPtrC	metaDataEntryName;
	if ( !GetStringFromConfig(aSection, KMetaDataEntryName(), metaDataEntryName) )
		{
		ERR_PRINTF2(_L("No %S"), &tempStore);
		SetBlockResult(EFail);
		}

	TPtrC	metaDataEntryValue;
	if ( !GetStringFromConfig(aSection, KMetaDataEntryValue(), metaDataEntryValue) )
		{
		ERR_PRINTF2(_L("No %S"), &tempStore);
		SetBlockResult(EFail);
		}

	CMMFMetaDataEntry*	metaDataEntry = NULL;
	TRAPD(err, metaDataEntry=CMMFMetaDataEntry::NewL(metaDataEntryName, metaDataEntryValue));
	CleanupStack::PushL(metaDataEntry);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("Cannot create CMMFMetaDataEntry - Error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{
		TInt	metaDataEntryIndex=0;
		GetIntFromConfig(aSection, KMetaDataEntryIndex(), metaDataEntryIndex);
		TRAPD(err, iRecorderUtility->ReplaceMetaDataEntryL(metaDataEntryIndex, *metaDataEntry));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("ReplaceMetaDataEntryL Error %d"), err);
			SetError(err);
			}
		}
	CleanupStack::PopAndDestroy(metaDataEntry);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetAudioDeviceMode(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	CMdaAudioRecorderUtility::TDeviceMode deviceMode = CMdaAudioRecorderUtility::EDefault;
	TPtrC	deviceModeStr;
	if ( GetStringFromConfig(aSection, KTDeviceMode(), deviceModeStr) )
		{
		if ( !ConvertToTDeviceMode(deviceModeStr, deviceMode) )
			{
			TInt	invalidDeviceMode;
			if(!GetIntFromConfig(aSection, KInvalidTDeviceMode(), invalidDeviceMode))
				{
				ERR_PRINTF1(_L("Invalid Device Mode not in ini file"));
				SetBlockResult(EFail);
				}

			deviceMode=CMdaAudioRecorderUtility::TDeviceMode(invalidDeviceMode);
			}
		iRecorderUtility->SetAudioDeviceMode(deviceMode);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetDestinationBitRateL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	destinationBitRate=0;
	GetIntFromConfig(aSection, KDestinationBitRate(), destinationBitRate);
	TRAPD(err, iRecorderUtility->SetDestinationBitRateL(destinationBitRate));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetDestinationBitRateL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetDestinationDataTypeL(const TDesC& aSection)
	{
	TFourCC	val;
	if ( CT_MMUtil::ReadFourCC(*this, aSection, val) )
		{
		TRAPD(err, iRecorderUtility->SetDestinationDataTypeL(val));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("SetDestinationDataTypeL Error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF1(_L("Value to set not in ini file"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetDestinationFormatL(const TDesC& aSection)
	{
	TUid	formatUid = TUid::Null();
	TInt	destinationFormat = 0;
	if(GetHexFromConfig(aSection, KFormat(), destinationFormat))
		{
		formatUid=TUid::Uid(destinationFormat);
		INFO_PRINTF3(_L("Format input from ini file is : %d. and destination format %d"), formatUid.iUid,destinationFormat);
		}

	
	TRAPD(err, iRecorderUtility->SetDestinationFormatL(formatUid));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetDestinationFormatL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetDestinationNumberOfChannelsL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	destinationChannels=0;
	GetIntFromConfig(aSection, KChannels(), destinationChannels);
	TRAPD(err, iRecorderUtility->SetDestinationNumberOfChannelsL(destinationChannels));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetDestinationNumberOfChannelsL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetDestinationSampleRateL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	destinationSampleRate=0;
	GetIntFromConfig(aSection, KSampleRate(), destinationSampleRate);
	TRAPD(err, iRecorderUtility->SetDestinationSampleRateL(destinationSampleRate));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetDestinationSampleRateL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetGain(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	gain=0;
	GetIntFromConfig(aSection, KGain(), gain);
	iRecorderUtility->SetGain(gain);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetMaxWriteLength(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	length=KMdaClipLocationMaxWriteLengthNone;
	GetIntFromConfig(aSection, KWriteLength(), length);
	iRecorderUtility->SetMaxWriteLength(length);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetPlayWindow(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt startseconds=0;
	TInt endseconds=0;
	GetIntFromConfig(aSection, KStartSeconds(), startseconds);
	const TTimeIntervalMicroSeconds &start=TTimeIntervalMicroSeconds((TInt64)startseconds);
	GetIntFromConfig(aSection, KEndSeconds(), endseconds);
	const TTimeIntervalMicroSeconds &end=TTimeIntervalMicroSeconds((TInt64)startseconds);
	iRecorderUtility->SetPlayWindow(start,end);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetPlaybackBalance(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	balance=KMMFBalanceCenter;
	GetIntFromConfig(aSection, KBalance(), balance);
	TInt err = iRecorderUtility->SetPlaybackBalance(balance);
	if ( err!=KErrNone )
		{
		ERR_PRINTF3(_L("SetPlaybackBalance(%d) Error %d"), balance, err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetPosition(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	seconds=0;
	GetIntFromConfig(aSection, KSeconds(), seconds);
	INFO_PRINTF2(_L("Position(%d)"), seconds);
	const TTimeIntervalMicroSeconds &position=TTimeIntervalMicroSeconds((TInt64)seconds);
	iRecorderUtility->SetPosition(position);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetPriority(const TDesC& aSection)
	{
	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TInt	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityMin;
    	}

	iRecorderUtility->SetPriority(priority, priorityPreference);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetRecordBalance(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	balance=KMMFBalanceCenter;
	GetIntFromConfig(aSection, KBalance(), balance);
	TInt err = iRecorderUtility->SetRecordBalance(balance);
	if ( err!=KErrNone )
		{
		ERR_PRINTF3(_L("SetRecordBalance(%d) Error %d"), balance, err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetRepeats(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	seconds=0;
	GetIntFromConfig(aSection, KSeconds(), seconds);
	const TTimeIntervalMicroSeconds &trailingSilence=TTimeIntervalMicroSeconds((TInt64)seconds);
	TInt repeatNumberOfTimes=0;
	GetIntFromConfig(aSection, KNumberOfTimes(), repeatNumberOfTimes);
	iRecorderUtility->SetRepeats(repeatNumberOfTimes, trailingSilence);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetThreadPriorityPlayback(const TDesC& aSection)
	{
	TThreadPriority	threadPriority;
	if ( !CT_MMUtil::ReadThreadPriority(*this, aSection, threadPriority) )
		{
		threadPriority=EPriorityNormal;
		}

	TInt	err = iRecorderUtility->SetThreadPriorityPlayback(threadPriority);
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::SetThreadPriorityPlayback failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetThreadPriorityRecord(const TDesC& aSection)
	{
	TThreadPriority	threadPriority;
	if ( !CT_MMUtil::ReadThreadPriority(*this, aSection, threadPriority) )
		{
		threadPriority=EPriorityNormal;
		}

	TInt	err = iRecorderUtility->SetThreadPriorityRecord(threadPriority);
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility::SetThreadPriorityRecord failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetVolume(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	volume=0;
	GetIntFromConfig(aSection, KVolume(), volume);
	INFO_PRINTF2(_L("SetVolume(%d)"), volume);
	iRecorderUtility->SetVolume(volume);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSetVolumeRamp(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	seconds=0;
	GetIntFromConfig(aSection, KSeconds(), seconds);
	INFO_PRINTF2(_L("SetVolumeRamp(%d)"), seconds);
	const TTimeIntervalMicroSeconds &rampDuration=TTimeIntervalMicroSeconds((TInt64)seconds);
 	iRecorderUtility->SetVolumeRamp(rampDuration);
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdSourceBitRateL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	sourcebitRate=0;
	TRAPD(err, sourcebitRate=iRecorderUtility->SourceBitRateL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SourceBitRateL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("SourceBitRateL=%d"), sourcebitRate);
		TInt	expectedSourceBitRate=0;
		if(GetIntFromConfig(aSection, KBitRate(), expectedSourceBitRate))
			{
			if(sourcebitRate != expectedSourceBitRate)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("SourcebitRate(%d) != expectedSourceBitRate(%d)"), sourcebitRate, expectedSourceBitRate);
				}
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdState(const TDesC& aSection)
	{
	TInt	state = iRecorderUtility->State();
	INFO_PRINTF2(_L("Current utility state: %S."), &GetState(state));

	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TPtrC	expectedStateStr;
	if (GetStringFromConfig(aSection, KState(), expectedStateStr))
		{
		if (GetState(state) != expectedStateStr)
			{
			ERR_PRINTF3(_L("State actual(%S)!=expected(%S)"),	&GetState(state), &expectedStateStr);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdStop()
	{
	iRecorderUtility->Stop();
	if ( iRecording )
		{
		iRecording=EFalse;
		DecOutstanding();
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdWillResumePlay()
	{
	TInt	err=iRecorderUtility->WillResumePlay();
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("WillResumePlay Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioRecorderUtility::DoCmdUseSharedHeap()
	{
	// Only if UseSharedHeap() is supported
	#if (defined CMDA_AUDIO_RECORDER_UTILITY_USED_SHARED_HEAP)
		iRecorderUtility->UseSharedHeap();
	#else
		INFO_PRINTF1(_L("CMdaAudioRecorderUtility::UseSharedHeap is not supported in this build."));
	#endif
	}

/**
Get Codec by String
@internalComponent
@return N/A
@param aFormatStr, &aMdaPackage
@pre N/A
@post N/A
*/
TBool CT_MMDataMdaAudioRecorderUtility::GetCodecL(const TDesC& aCodecStr, TMdaPackage* &aMdaPackage)
	{
	TBool	ret = ETrue;

	if (aCodecStr == KPcmWavCodec)
		{
		aMdaPackage = new(ELeave) TMdaPcmWavCodec();
		}
	else if (aCodecStr == KAlawWavCodec)
		{
		aMdaPackage = new(ELeave) TMdaAlawWavCodec();
		}
	else if (aCodecStr == KMulawWavCodec)
		{
		aMdaPackage = new(ELeave) TMdaMulawWavCodec();
		}
	else if (aCodecStr == KImaAdpcmWavCodec)
		{
		aMdaPackage = new(ELeave) TMdaImaAdpcmWavCodec();
		}
	else if (aCodecStr == KAuCodec)
		{
		aMdaPackage = new(ELeave) TMdaAuCodec();
		}
	else if (aCodecStr == KPcm8BitAuCodec)
		{
		aMdaPackage = new(ELeave) TMdaPcm8BitAuCodec();
		}
	else if (aCodecStr == KPcm16BitAuCodec)
		{
		aMdaPackage = new(ELeave) TMdaPcm16BitAuCodec();
		}
	else if (aCodecStr == KAlawAuCodec)
		{
		aMdaPackage = new(ELeave) TMdaAlawAuCodec();
		}
	else if (aCodecStr == KMulawAuCodec)
		{
		aMdaPackage = new(ELeave) TMdaMulawAuCodec();
		}
	else if (aCodecStr == KRawAudioCodec)
		{
		aMdaPackage = new(ELeave) TMdaRawAudioCodec();
		}
	else if (aCodecStr == KMulawRawAudioCodec)
		{
		aMdaPackage = new(ELeave) TMdaMulawRawAudioCodec();
		}
	else if (aCodecStr == KAlawRawAudioCodec)
		{
		aMdaPackage = new(ELeave) TMdaAlawRawAudioCodec();
		}
	else if (aCodecStr == KS8PcmRawAudioCodec)
		{
		aMdaPackage = new(ELeave) TMdaS8PcmRawAudioCodec();
		}
	else if (aCodecStr == KU8PcmRawAudioCodec)
		{
		aMdaPackage = new(ELeave) TMdaU8PcmRawAudioCodec();
		}
	else if (aCodecStr == KSL16RawAudioCodec)
		{
		aMdaPackage = new(ELeave) TMdaSL16RawAudioCodec();
		}
	else if (aCodecStr == KSB16RawAudioCodec)
		{
		aMdaPackage = new(ELeave) TMdaSB16RawAudioCodec();
		}
	else if (aCodecStr == KUL16RawAudioCodec)
		{
		aMdaPackage = new(ELeave) TMdaUL16RawAudioCodec();
		}
	else if (aCodecStr == KUB16RawAudioCodec)
		{
		aMdaPackage = new(ELeave) TMdaUB16RawAudioCodec();
		}
	else if (aCodecStr == KInvalidCodec)
		{
		// Invalid codec package
		aMdaPackage = new(ELeave) TMdaAudioDataSettings();
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}

void CT_MMDataMdaAudioRecorderUtility::GetAudioSettingsL(const TDesC& aAudioSettingsStr, const TDesC& aSection, TMdaPackage* &aMdaPackage)
	{
	TPtrC	audioSettingsStr;
	if (GetStringFromConfig(aSection, aAudioSettingsStr, audioSettingsStr))
		{
		if (GetStringFromConfig(aSection, KInvalidAudioSettings(), audioSettingsStr))
			{
			// Invalid audiosettings package
			aMdaPackage = new(ELeave) TMdaPcmWavCodec();
			}
		else
			{
			aMdaPackage = new(ELeave) TMdaAudioDataSettings();
			CT_MMUtil::ReadAudioDataSettings(*this, *((TMdaAudioDataSettings*)aMdaPackage), aSection);
			}
		}
	}
