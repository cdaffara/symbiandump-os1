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


#include "T_MMDataVideoRecorderUtility.h"
#include "T_MMUtil.h"

//	EPOC includes
#include <ecam.h>

//	System Includes
#include <bautils.h>

/*@{*/
_LIT(KMetaDataEntryName,						"metaDataEntryname");
_LIT(KMetaDataEntryValue,						"metaDataEntryvalue");
_LIT(KMetaDataEntryID,							"metaDataEntryid");
_LIT(KMetaDataEntryIndex,						"metaDataEntryindex");
_LIT(KOutputFile,								"outputfile");
_LIT(KUseRFile,									"useRFile");
_LIT(KOutputURL,								"outputURL");
_LIT(KBitRate,									"bitrate");
_LIT(KEnabled,									"enabled");
_LIT(KGain,										"gain");
_LIT(KMaxClipSize,								"maxclipsize");
_LIT(KFrameRate,								"framerate");
_LIT(KWidth,									"width");
_LIT(KHeight,									"height");
_LIT(KVideoType,								"videotype");
_LIT(KBufferSize,								"buffersize");
_LIT(KFunction,									"function");
_LIT(KUseDataFrom,								"useDataFrom");
_LIT(KVideoControllerUid,						"videocontrolleruid");
_LIT(KVideoFormatUid,							"videoformatuid");
_LIT(KMaxValue,									"maxvalue");
_LIT(KMinValue,									"minvalue");
_LIT(KNoOfMetaDataEntries,						"noofmetadataentries");
_LIT(KCamera,									"camera");
_LIT(KCameraHandle,								"camerahandle");
_LIT(KAccessPointID,							"accesspointid");

_LIT(KCmdFilename,								"filename");


_LIT(KCmdDestructor,							"~");
_LIT(KCmdAddMetaDataEntryL,						"AddMetaDataEntryL");
_LIT(KCmdAudioBitRateL,							"AudioBitRateL");
_LIT(KCmdAudioEnabledL,							"AudioEnabledL");
_LIT(KCmdAudioTypeL,							"AudioTypeL");
_LIT(KCmdClose,									"Close");
_LIT(KCmdControllerImplementationInformationL,	"ControllerImplementationInformationL");
_LIT(KCmdCustomCommandAsync,					"CustomCommandAsync");
_LIT(KCmdCustomCommandSync,						"CustomCommandSync");
_LIT(KCmdDurationL,								"DurationL");
_LIT(KCmdGainL,									"GainL");
_LIT(KCmdGetPriorityL,							"GetPriorityL");
_LIT(KCmdGetSupportedAudioTypesL,				"GetSupportedAudioTypesL");
_LIT(KCmdGetSupportedVideoTypesL,				"GetSupportedVideoTypesL");
_LIT(KCmdGetVideoFrameSizeL,					"GetVideoFrameSizeL");
_LIT(KCmdMaxGainL,								"MaxGainL");
_LIT(KCmdMetaDataEntryL,						"MetaDataEntryL");
_LIT(KCmdNewL,									"NewL");
_LIT(KCmdNumberOfMetaDataEntriesL,				"NumberOfMetaDataEntriesL");
_LIT(KCmdOpenDesL,								"OpenDesL");
_LIT(KCmdOpenFileL,								"OpenFileL");
_LIT(KCmdOpenUrlL,								"OpenUrlL");
_LIT(KCmdPauseL,								"PauseL");
_LIT(KCmdPrepare,								"Prepare");
_LIT(KCmdRecord,								"Record");
_LIT(KCmdRecordTimeAvailable,					"RecordTimeAvailable");
_LIT(KCmdRemoveMetaDataEntryL,					"RemoveMetaDataEntryL");
_LIT(KCmdReplaceMetaDataEntryL,					"ReplaceMetaDataEntryL");
_LIT(KCmdSetAudioBitRateL,						"SetAudioBitRateL");
_LIT(KCmdSetAudioEnabledL,						"SetAudioEnabledL");
_LIT(KCmdSetAudioTypeL,							"SetAudioTypeL");
_LIT(KCmdSetGainL,								"SetGainL");
_LIT(KCmdSetMaxClipSizeL,						"SetMaxClipSizeL");
_LIT(KCmdSetPriorityL,							"SetPriorityL");
_LIT(KCmdSetVideoBitRateL,						"SetVideoBitRateL");
_LIT(KCmdSetVideoFrameRateL,					"SetVideoFrameRateL");
_LIT(KCmdSetVideoFrameSizeL,					"SetVideoFrameSizeL");
_LIT(KCmdSetVideoTypeL,							"SetVideoTypeL");
_LIT(KCmdStop,									"Stop");
_LIT(KCmdVideoBitRateL,							"VideoBitRateL");
_LIT(KCmdVideoFormatMimeType,					"VideoFormatMimeType");
_LIT(KCmdVideoFrameRateL,						"VideoFrameRateL");

_LIT(KCmdDeleteFile,							"deletefile");
/*@}*/

const TInt	KVideoRecorderUtilityBufferSize	=1024;
const TInt	KTemporaryBufferSize			=64;
const TInt	KCustomCommandBufferSize		=255;
const TInt	KDefaultAudioBitRate			=48000 * 16;
const TInt	KDefaultVideoFrameRate			=25;
const TInt	KBufMimeTypeGranularity			=4;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMDataVideoRecorderUtility* CT_MMDataVideoRecorderUtility::NewL()
	{
	CT_MMDataVideoRecorderUtility*	ret=new (ELeave) CT_MMDataVideoRecorderUtility();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_MMDataVideoRecorderUtility::CT_MMDataVideoRecorderUtility()
:	iVideoRecorderUtility(NULL)
,	iActiveCallback(NULL)
,	iBuffer(NULL)
,	iBufferPtr(NULL)
,	iBufferSize(KVideoRecorderUtilityBufferSize)
,	iFileOpened(EFalse)
,	iApiAsyncErrorIndex(0)
,	iRecordStarted(EFalse)
	{
	}

void CT_MMDataVideoRecorderUtility::ConstructL()
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

CT_MMDataVideoRecorderUtility::~CT_MMDataVideoRecorderUtility()
	{
	DestroyData();

	delete iActiveCallback;
	iActiveCallback=NULL;

	iFs.Close();
	}

TAny* CT_MMDataVideoRecorderUtility::GetObject()
	{
	return iVideoRecorderUtility;
	}

void CT_MMDataVideoRecorderUtility::DestroyData()
	{
	iSupportedAudioTypes.Close();
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	delete iBufferPtr;
	iBufferPtr=NULL;
	delete iBuffer;
	iBuffer=NULL;
	delete iVideoRecorderUtility;
	iVideoRecorderUtility=NULL;
	}

TBool CT_MMDataVideoRecorderUtility::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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

	if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdAddMetaDataEntryL )
		{
		DoCmdAddMetaDataEntryL(aSection);
		}
	else if ( aCommand==KCmdAudioBitRateL )
		{
		DoCmdAudioBitRateL(aSection);
		}
	else if ( aCommand==KCmdAudioEnabledL )
		{
		DoCmdAudioEnabledL(aSection);
		}
	else if ( aCommand==KCmdAudioTypeL )
		{
		DoCmdAudioTypeL(aSection);
		}
	else if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if ( aCommand==KCmdControllerImplementationInformationL )
		{
		DoCmdControllerImplementationInformationL();
		}
	else if ( aCommand==KCmdCustomCommandAsync )
		{
		DoCmdCustomCommandAsync(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCustomCommandSync )
		{
		DoCmdCustomCommandSync(aSection);
		}
	else if ( aCommand==KCmdDurationL )
		{
		DoCmdDurationL(aSection);
		}
	else if ( aCommand==KCmdGainL )
		{
		DoCmdGainL(aSection);
		}
	else if ( aCommand==KCmdGetPriorityL )
		{
		DoCmdGetPriorityL(aSection);
		}
	else if ( aCommand==KCmdGetSupportedAudioTypesL )
		{
		DoCmdGetSupportedAudioTypesL();
		}
	else if ( aCommand==KCmdGetSupportedVideoTypesL )
		{
		DoCmdGetSupportedVideoTypesL();
		}
	else if ( aCommand==KCmdGetVideoFrameSizeL )
		{
		DoCmdGetVideoFrameSizeL(aSection);
		}
	else if ( aCommand==KCmdMaxGainL )
		{
		DoCmdMaxGainL(aSection);
		}
	else if ( aCommand==KCmdMetaDataEntryL )
		{
		DoCmdMetaDataEntryL(aSection);
		}
	else if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdNumberOfMetaDataEntriesL )
		{
		DoCmdNumberOfMetaDataEntriesL(aSection);
		}
	else if ( aCommand==KCmdOpenDesL )
		{
		DoCmdOpenDesL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdOpenFileL )
		{
		DoCmdOpenFileL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdOpenUrlL )
		{
		DoCmdOpenUrlL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdPauseL )
		{
		DoCmdPauseL();
		}
	else if ( aCommand==KCmdPrepare )
		{
		DoCmdPrepare(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdRecord )
		{
		DoCmdRecord(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdRecordTimeAvailable )
		{
		DoCmdRecordTimeAvailable(aSection);
		}
	else if ( aCommand==KCmdRemoveMetaDataEntryL )
		{
		DoCmdRemoveMetaDataEntryL(aSection);
		}
	else if ( aCommand==KCmdReplaceMetaDataEntryL )
		{
		DoCmdReplaceMetaDataEntryL(aSection);
		}
	else if ( aCommand==KCmdSetAudioBitRateL )
		{
		DoCmdSetAudioBitRateL(aSection);
		}
	else if ( aCommand==KCmdSetAudioEnabledL )
		{
		DoCmdSetAudioEnabledL(aSection);
		}
	else if ( aCommand==KCmdSetAudioTypeL )
		{
		DoCmdSetAudioTypeL(aSection);
		}
	else if ( aCommand==KCmdSetGainL )
		{
		DoCmdSetGainL(aSection);
		}
	else if ( aCommand==KCmdSetMaxClipSizeL )
		{
		DoCmdSetMaxClipSizeL(aSection);
		}
	else if ( aCommand==KCmdSetPriorityL )
		{
		DoCmdSetPriorityL(aSection);
		}
	else if ( aCommand==KCmdSetVideoBitRateL )
		{
		DoCmdSetVideoBitRateL(aSection);
		}
	else if ( aCommand==KCmdSetVideoFrameRateL )
		{
		DoCmdSetVideoFrameRateL(aSection);
		}
	else if ( aCommand==KCmdSetVideoFrameSizeL )
		{
		DoCmdSetVideoFrameSizeL(aSection);
		}
	else if ( aCommand==KCmdSetVideoTypeL )
		{
		DoCmdSetVideoTypeL(aSection);
		}
	else if ( aCommand==KCmdStop )
		{
		DoCmdStop();
		}
	else if ( aCommand==KCmdVideoBitRateL )
		{
		DoCmdVideoBitRateL(aSection);
		}
	else if ( aCommand==KCmdVideoFormatMimeType )
		{
		DoCmdVideoFormatMimeTypeL(aSection);
		}
	else if ( aCommand==KCmdVideoFrameRateL )
		{
		DoCmdVideoFrameRateL(aSection);
		}
	else if ( aCommand==KCmdDeleteFile )
		{
		DeleteFile(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_MMDataVideoRecorderUtility::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_MMDataVideoRecorderUtility::DoCmdAddMetaDataEntryL(const TDesC& aSection)
	{

	TPtrC	metaDataEntryName;
	if ( GetStringFromConfig(aSection, KMetaDataEntryName(), metaDataEntryName) )
		{
		TPtrC	metaDataEntryValue;
		if ( GetStringFromConfig(aSection, KMetaDataEntryValue(), metaDataEntryValue) )
			{
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
				TRAPD(err, iVideoRecorderUtility->AddMetaDataEntryL(*metaDataEntry));
				if ( err!=KErrNone )
					{
					ERR_PRINTF2(_L("AddMetaDataEntryL Error %d"), err);
					SetError(err);
					}
				}
			CleanupStack::PopAndDestroy(metaDataEntry);
			}
		else
			{
			ERR_PRINTF2(_L("No %S"), &KMetaDataEntryValue());
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("No %S"), &KMetaDataEntryName());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdAudioBitRateL(const TDesC& aSection)
	{

	TInt	bitRate=0;
	TRAPD(err, bitRate=iVideoRecorderUtility->AudioBitRateL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("AudioBitRateL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("AudioBitRateL=%d"), bitRate);
		TInt	expectedBitRate=0;
		if(GetIntFromConfig(aSection, KBitRate(), expectedBitRate))
			{
			if(bitRate != expectedBitRate)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("audioBitRate(%d) != expectedAudioBitRate(%d)"), bitRate, expectedBitRate);
				}
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdAudioEnabledL(const TDesC& aSection)
	{

	TBool	audioEnabled=EFalse;
	TRAPD(err, audioEnabled=iVideoRecorderUtility->AudioEnabledL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("AudioEnabledL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("AudioEnabledL=%d"), audioEnabled);
		TBool	expectedAudioEnabled=EFalse;
		if(GetBoolFromConfig(aSection, KEnabled(), expectedAudioEnabled))
			{
			if(audioEnabled != expectedAudioEnabled)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("audioEnabled(%d) != expectedAudioEnabled(%d)"), audioEnabled, expectedAudioEnabled);
				}
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdAudioTypeL(const TDesC& aSection)
	{
	TFourCC	audioType=KMMFFourCCCodeNULL;
	TRAPD(err, audioType=iVideoRecorderUtility->AudioTypeL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("AudioTypeL Error %d"), err);
		SetError(err);
		}
	else
		{
		TFourCC	expectedVal;
		if ( CT_MMUtil::ReadFourCC(*this, aSection, expectedVal) )
			{
			if( expectedVal!=audioType )
				{
				ERR_PRINTF3(_L("audioType(%d) != expectedAudioType(%d)"), audioType.FourCC(), expectedVal.FourCC());
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdClose()
	{
	iVideoRecorderUtility->Close();
	}

void CT_MMDataVideoRecorderUtility::DoCmdControllerImplementationInformationL()
	{
	const CMMFControllerImplementationInformation*	info = NULL;
	TRAPD(err, info = &iVideoRecorderUtility->ControllerImplementationInformationL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("ControllerImplementationInformationL Error %d"), err);
		SetError(err);
		}
	else
		{
		TPtrC	temp=info->DisplayName();
		INFO_PRINTF2(_L("ControllerImplementationInformationL=%S"), &temp);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdCustomCommandAsync(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TMMFMessageDestination		handleInfo(iVideoControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	TInt	function=0;
	GetIntFromConfig(aSection, KFunction(), function);

	TBool	useDataFrom=EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), useDataFrom);

	if ( useDataFrom )
		{
		TBuf8<KCustomCommandBufferSize>	dataFromText;
		iVideoRecorderUtility->CustomCommandAsync(messageDest, function, KNullDesC8, KNullDesC8, dataFromText, iActiveCallback->iStatus);
		}
	else
		{
		iVideoRecorderUtility->CustomCommandAsync(messageDest, function, KNullDesC8, KNullDesC8, iActiveCallback->iStatus);
		}
	iActiveCallback->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}

void CT_MMDataVideoRecorderUtility::DoCmdCustomCommandSync(const TDesC& aSection)
	{

	TMMFMessageDestination		handleInfo(iVideoControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	TInt	function=0;
	GetIntFromConfig(aSection, KFunction(), function);

	TBool	useDataFrom=EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), useDataFrom);

	TInt	err=KErrNone;
	if ( useDataFrom )
		{
		TBuf8<KCustomCommandBufferSize>	dataFromText;
		err = iVideoRecorderUtility->CustomCommandSync(messageDest, function, KNullDesC8, KNullDesC8, dataFromText);
		}
	else
		{
		err = iVideoRecorderUtility->CustomCommandSync(messageDest, function, KNullDesC8, KNullDesC8);
		}
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CustomCommandSync Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdDurationL(const TDesC& aSection)
	{

	TTimeIntervalMicroSeconds	duration;
	TRAPD(err, duration=iVideoRecorderUtility->DurationL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("DurationL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("DurationL=%d"), duration.Int64());

		TInt	maxDuration=0;
		if(GetIntFromConfig(aSection, KMaxValue(), maxDuration))
			{
			if(duration.Int64() > maxDuration)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("maxDuration(%d) < duration(%d)"), maxDuration, duration.Int64());
				}
			}

		TInt	minDuration=0;
		if(GetIntFromConfig(aSection, KMinValue(), minDuration))
			{
			if(duration.Int64() < minDuration)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("minDuration(%d) > duration(%d)"), minDuration, duration.Int64());
				}
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdGainL(const TDesC& aSection)
	{

	TInt	gain=0;
	TRAPD(err, gain=iVideoRecorderUtility->GainL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GainL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("GainL=%d"), gain);
		TInt	expectedGain=0;
		if(GetIntFromConfig(aSection, KGain(), expectedGain))
			{
			if(expectedGain == -1)
				{
				// Expect to get the maximum gain
				TInt	maxGain=0;
				TRAPD(err, maxGain=iVideoRecorderUtility->MaxGainL());
				if ( err!=KErrNone )
					{
					ERR_PRINTF2(_L("MaxGainL Error %d"), err);
					SetError(err);
					}
				else if(gain != maxGain)
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

void CT_MMDataVideoRecorderUtility::DoCmdGetPriorityL(const TDesC& aSection)
	{
	TInt					priority=EMdaPriorityMin;
	TMdaPriorityPreference	priorityPreference=EMdaPriorityPreferenceNone;
	TRAPD(err, iVideoRecorderUtility->GetPriorityL(priority, priorityPreference));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetPriorityL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("GetPriorityL. Priority=%d"), priority);
		INFO_PRINTF2(_L("GetPriorityL. Preference=%d"), priorityPreference);

		TInt	expectedPriority;
		if ( CT_MMUtil::ReadMdaPriority(*this, aSection, expectedPriority) )
			{
			if ( priority != expectedPriority )
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("priority(%d) != expectedPriority(%d)"), priority, expectedPriority);
				}
			}

		TMdaPriorityPreference	expectedPriorityPreference;
		if ( CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, expectedPriorityPreference) )
			{
			if ( priorityPreference != expectedPriorityPreference )
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("priorityPreference(%d) != expectedPriorityPreference(%d)"), priorityPreference, expectedPriorityPreference);
				}
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdGetSupportedAudioTypesL()
	{
	iSupportedAudioTypes.Reset();
	TRAPD(err, iVideoRecorderUtility->GetSupportedAudioTypesL(iSupportedAudioTypes));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetSupportedAudioTypesL Error %d"), err);
		SetError(err);
		}
	else
		{
		if ( iSupportedAudioTypes.Count()==0 )
			{
			INFO_PRINTF1(_L("GetSupportedAudioTypesL. No supported audio types"));
			}
		for (TInt i=0; i<iSupportedAudioTypes.Count(); i++)
			{
			INFO_PRINTF2(_L("GetSupportedAudioTypesL. Supported audio type=%S"), &CT_MMUtil::GetFourCCString(iSupportedAudioTypes[i].FourCC()));
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdGetSupportedVideoTypesL()
	{
	CDesC8ArrayFlat*	videoTypes = new (ELeave) CDesC8ArrayFlat(KBufMimeTypeGranularity);
	CleanupStack::PushL(videoTypes);
	TRAPD(err, iVideoRecorderUtility->GetSupportedVideoTypesL(*videoTypes));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetSupportedVideoTypesL Error %d"), err);
		SetError(err);
		}
	else
		{
		for (TInt i=0; i<videoTypes->Count(); i++)
			{
			TBuf<KTemporaryBufferSize>	temp;
			temp.Copy((*videoTypes)[i]);
			INFO_PRINTF2(_L("GetSupportedVideoTypesL. Supported video type=%S"), &temp);
			}
		}
	CleanupStack::PopAndDestroy(videoTypes);
	}

void CT_MMDataVideoRecorderUtility::DoCmdGetVideoFrameSizeL(const TDesC& aSection)
	{

	TSize	videoFrameSize(0,0);
	TRAPD(err, iVideoRecorderUtility->GetVideoFrameSizeL(videoFrameSize));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetVideoFrameSizeL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("GetVideoFrameSizeL. Width=%d"), videoFrameSize.iWidth);
		INFO_PRINTF2(_L("GetVideoFrameSizeL. Height=%d"), videoFrameSize.iHeight);

		TInt	expectedWidth=0;
		if(GetIntFromConfig(aSection, KWidth(), expectedWidth))
			{
			if(videoFrameSize.iWidth != expectedWidth)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("width(%d) != expectedWidth(%d)"), videoFrameSize.iWidth, expectedWidth);
				}
			}

		TInt	expectedHeight=0;
		if(GetIntFromConfig(aSection, KHeight(), expectedHeight))
			{
			if(videoFrameSize.iHeight != expectedHeight)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("height(%d) != expectedHeight(%d)"), videoFrameSize.iHeight, expectedHeight);
				}
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdMaxGainL(const TDesC& aSection)
	{

	TInt	maxGain=0;
	TRAPD(err, maxGain=iVideoRecorderUtility->MaxGainL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MaxGainL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("MaxGainL=%d"), maxGain);

		TInt	maxMaxGain=0;
		if(GetIntFromConfig(aSection, KMaxValue(), maxMaxGain))
			{
			if(maxGain > maxMaxGain)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("maxMaxGain(%d) < maxGain(%d)"), maxMaxGain, maxGain);
				}
			}

		TInt	minMaxGain=0;
		if(GetIntFromConfig(aSection, KMinValue(), minMaxGain))
			{
			if(maxGain < minMaxGain)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("minMaxGain(%d) > maxGain(%d)"), minMaxGain, maxGain);
				}
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdMetaDataEntryL(const TDesC& aSection)
	{

	TInt	metaDataEntryIndex=0;
	GetIntFromConfig(aSection, KMetaDataEntryIndex(), metaDataEntryIndex);
	CMMFMetaDataEntry*	metaDataEntry=NULL;
	TRAPD(err, metaDataEntry=iVideoRecorderUtility->MetaDataEntryL(metaDataEntryIndex));
	CleanupStack::PushL(metaDataEntry);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MetaDataEntryL Error %d"), err);
		SetError(err);
		}
	else
		{
		const TDesC&	temp=metaDataEntry->Name();
		INFO_PRINTF2(_L("MetaDataEntryL=%S"), &temp);
		}
	CleanupStack::PopAndDestroy(metaDataEntry);
	}

void CT_MMDataVideoRecorderUtility::DoCmdNewL(const TDesC& aSection)
	{
	delete iVideoRecorderUtility;
	iVideoRecorderUtility=NULL;

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

	TRAPD(err, iVideoRecorderUtility=CVideoRecorderUtility::NewL(*this, priority, priorityPreference));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("NewL(this, priority, priorityPreference) Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdNumberOfMetaDataEntriesL(const TDesC& aSection)
	{

	TInt	numberOfMetaDataEntries=0;
	TRAPD(err, numberOfMetaDataEntries=iVideoRecorderUtility->NumberOfMetaDataEntriesL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("NumberOfMetaDataEntriesL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("NumberOfMetaDataEntriesL=%d"), numberOfMetaDataEntries);
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

void CT_MMDataVideoRecorderUtility::DoCmdOpenDesL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	iBufferSize=KVideoRecorderUtilityBufferSize;
	GetIntFromConfig(aSection, KBufferSize(), iBufferSize);
	delete iBuffer;
	iBuffer=NULL;
	iBuffer=HBufC8::NewL(iBufferSize);
	*iBuffer=KNullDesC8;
	delete iBufferPtr;
	iBufferPtr=new (ELeave) TPtr8(iBuffer->Des());
	CT_MMUtil::GetControllerAndFormatUidL(*this, _L(""), KUidMediaTypeVideo, ETrue, iVideoControllerUid, iVideoFormatUid);

	// Use default video controller Uid unless one is passed in
	TInt	videoControllerId;
	if ( GetHexFromConfig(aSection, KVideoControllerUid(), videoControllerId) )
		{
		iVideoControllerUid=TUid::Uid(videoControllerId);
		}

	// Use default video format Uid unless one is passed in
	TInt	videoFormatId;
	if ( GetHexFromConfig(aSection, KVideoFormatUid(), videoFormatId) )
		{
		iVideoFormatUid=TUid::Uid(videoFormatId);
		}

	TFourCC	audioTypeFourCC;
	if ( !CT_MMUtil::ReadFourCC(*this, aSection, audioTypeFourCC) )
		{
		audioTypeFourCC=KMMFFourCCCodeNULL;
		}

	HBufC8*	videoType8=NULL;
	TPtrC	videoType;
	if ( GetStringFromConfig(aSection, KVideoType(), videoType) )
		{
		videoType8=HBufC8::NewL(videoType.Length());
		videoType8->Des().Copy(videoType);
		}
	else
		{
		videoType8=HBufC8::NewL(0);
		videoType8->Des().Copy(KNullDesC8);
		}

	TInt	cameraHandle;
	ReadCameraHandleL(aSection, cameraHandle);

	TRAPD(err, iVideoRecorderUtility->OpenDesL(*iBufferPtr, cameraHandle, iVideoControllerUid, iVideoFormatUid, *videoType8, audioTypeFourCC));
	if ( err==KErrNone )
		{
		INFO_PRINTF1(_L("Buffer opened"));
		IncOutstanding();

		iApiAsyncErrorIndex = aAsyncErrorIndex;
		// call back in MvruoOpenComplete
		}
	else
		{
		ERR_PRINTF2(_L("OpenDesL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdOpenFileL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TBool	useRFile=EFalse;
	GetBoolFromConfig(aSection, KUseRFile(), useRFile);

	TPtrC	filename;
	if ( GetStringFromConfig(aSection, KOutputFile(), filename) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, filename, KUidMediaTypeVideo, ETrue, iVideoControllerUid, iVideoFormatUid);

		// Use default video controller Uid unless one is passed in
		TInt	videoControllerId;
		if ( GetHexFromConfig(aSection, KVideoControllerUid(), videoControllerId) )
			{
			iVideoControllerUid=TUid::Uid(videoControllerId);
			}

		// Use default video format Uid unless one is passed in
		TInt	videoFormatId;
		if ( GetHexFromConfig(aSection, KVideoFormatUid(), videoFormatId) )
			{
			iVideoFormatUid=TUid::Uid(videoFormatId);
			}

		TFourCC	audioTypeFourCC;
		if ( !CT_MMUtil::ReadFourCC(*this, aSection, audioTypeFourCC) )
			{
			audioTypeFourCC=KMMFFourCCCodeNULL;
			}

		HBufC8*	videoType8=NULL;
		TPtrC	videoType;
		if ( GetStringFromConfig(aSection, KVideoType(), videoType) )
			{
			videoType8=HBufC8::NewL(videoType.Length());
			videoType8->Des().Copy(videoType);
			}
		else
			{
			videoType8=HBufC8::NewL(0);
			videoType8->Des().Copy(KNullDesC8);
			}

		TInt	cameraHandle;
		ReadCameraHandleL(aSection, cameraHandle);

		TInt	error=KErrNone;
		if ( useRFile )
			{
			iFs.MkDirAll(filename);
			iFs.Delete(filename);
			TInt	fileError=iFile.Create(iFs, filename, EFileWrite);
			if ( fileError==KErrNone )
				{
				INFO_PRINTF1(_L("File created"));
				iFileOpened=ETrue;
				}
			else
				{
				ERR_PRINTF3(_L("Cannot create outputfile %S. Error %d"), &filename, fileError);
				SetBlockResult(EFail);
				}
			TRAP(error,iVideoRecorderUtility->OpenFileL(iFile, cameraHandle, iVideoControllerUid, iVideoFormatUid, *videoType8, audioTypeFourCC));
			}
		else
			{
			TRAP(error,iVideoRecorderUtility->OpenFileL(filename, cameraHandle, iVideoControllerUid, iVideoFormatUid, *videoType8, audioTypeFourCC));
			}

		if ( error==KErrNone )
			{
			INFO_PRINTF1(_L("File opened"));
			IncOutstanding();
			iApiAsyncErrorIndex = aAsyncErrorIndex;
			// call back in MvruoOpenComplete
			}
		else
			{
			ERR_PRINTF2(_L("OpenFileL Error %d"), error);
			SetError(error);
			}
		}
	else
		{
		ERR_PRINTF2(_L("No %S"), &KOutputFile());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdOpenUrlL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TPtrC	url;
	if ( GetStringFromConfig(aSection, KOutputURL(), url) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, url, KUidMediaTypeVideo, ETrue, iVideoControllerUid, iVideoFormatUid);

		// Use default video controller Uid unless one is passed in
		TInt	videoControllerId;
		if ( GetHexFromConfig(aSection, KVideoControllerUid(), videoControllerId) )
			{
			iVideoControllerUid=TUid::Uid(videoControllerId);
			}

		// Use default video format Uid unless one is passed in
		TInt	videoFormatId;
		if ( GetHexFromConfig(aSection, KVideoFormatUid(), videoFormatId) )
			{
			iVideoFormatUid=TUid::Uid(videoFormatId);
			}

		TFourCC	audioTypeFourCC=KMMFFourCCCodeNULL;
		if ( !CT_MMUtil::ReadFourCC(*this, aSection, audioTypeFourCC) )
			{
			audioTypeFourCC=KMMFFourCCCodeNULL;
			}

		HBufC8*	videoType8=NULL;
		TPtrC	videoType;
		if ( GetStringFromConfig(aSection, KVideoType(), videoType) )
			{
			videoType8=HBufC8::NewL(videoType.Length());
			videoType8->Des().Copy(videoType);
			}
		else
			{
			videoType8=HBufC8::NewL(0);
			videoType8->Des().Copy(KNullDesC8);
			}

		TInt	cameraHandle;
		ReadCameraHandleL(aSection, cameraHandle);

		TInt	accessPointID = KUseDefaultIap;
		GetIntFromConfig(aSection, KAccessPointID(), accessPointID);

		TRAPD(err,iVideoRecorderUtility->OpenUrlL(url, accessPointID, cameraHandle, iVideoControllerUid, iVideoFormatUid, *videoType8, audioTypeFourCC));
		if ( err==KErrNone )
			{
			INFO_PRINTF1(_L("URL opened"));
			IncOutstanding();
			// call back in MvruoOpenComplete
			iApiAsyncErrorIndex = aAsyncErrorIndex;
			}
		else
			{
			ERR_PRINTF2(_L("OpenUrlL Error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("No %S"), &KOutputURL());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdPauseL()
	{
	TRAPD(err, iVideoRecorderUtility->PauseL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("PauseL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdPrepare(const TInt aAsyncErrorIndex)
	{
	iVideoRecorderUtility->Prepare();
	IncOutstanding();
	// call back in MvruoPrepareComplete
	iApiAsyncErrorIndex = aAsyncErrorIndex;
	}

void CT_MMDataVideoRecorderUtility::DoCmdRecord(const TInt aAsyncErrorIndex)
	{
	iVideoRecorderUtility->Record();
	IncOutstanding();
	INFO_PRINTF1(_L("Start recording"));
	iRecordStarted = ETrue;
	// call back in MvruoRecordComplete
	iApiAsyncErrorIndex = aAsyncErrorIndex;
	}

void CT_MMDataVideoRecorderUtility::DoCmdRecordTimeAvailable(const TDesC& aSection)
	{

	TTimeIntervalMicroSeconds	recordTimeAvailable=iVideoRecorderUtility->RecordTimeAvailable();
	INFO_PRINTF2(_L("RecordTimeAvailable=%d"), recordTimeAvailable.Int64());

	TInt	maxRecordTimeAvailable=0;
	if(GetIntFromConfig(aSection, KMaxValue(), maxRecordTimeAvailable))
		{
		if(recordTimeAvailable.Int64() > maxRecordTimeAvailable)
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("maxRecordTimeAvailable(%d) < recordTimeAvailable(%d)"), maxRecordTimeAvailable, recordTimeAvailable.Int64());
			}
		}

	TInt	minRecordTimeAvailable=0;
	if(GetIntFromConfig(aSection, KMinValue(), minRecordTimeAvailable))
		{
		if(recordTimeAvailable.Int64() < minRecordTimeAvailable)
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("minRecordTimeAvailable(%d) > recordTimeAvailable(%d)"), minRecordTimeAvailable, recordTimeAvailable.Int64());
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdRemoveMetaDataEntryL(const TDesC& aSection)
	{

	TInt	metaDataEntryID=0;
	GetIntFromConfig(aSection, KMetaDataEntryID(), metaDataEntryID);
	TRAPD(err, iVideoRecorderUtility->RemoveMetaDataEntryL(metaDataEntryID));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RemoveMetaDataEntryL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdReplaceMetaDataEntryL(const TDesC& aSection)
	{

	TPtrC	metaDataEntryName;
	if ( GetStringFromConfig(aSection, KMetaDataEntryName(), metaDataEntryName) )
		{
		TPtrC metaDataEntryValue;
		if ( GetStringFromConfig(aSection, KMetaDataEntryValue(), metaDataEntryValue) )
			{
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
				TRAPD(err, iVideoRecorderUtility->ReplaceMetaDataEntryL(metaDataEntryIndex, *metaDataEntry));
				if ( err!=KErrNone )
					{
					ERR_PRINTF2(_L("ReplaceMetaDataEntryL Error %d"), err);
					SetError(err);
					}
				}
			CleanupStack::PopAndDestroy(metaDataEntry);
			}
		else
			{
			ERR_PRINTF2(_L("No %S"), &KMetaDataEntryValue());
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("No %S"), &KMetaDataEntryName());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetAudioBitRateL(const TDesC& aSection)
	{

	TInt	audioBitRate=KDefaultAudioBitRate;
	GetIntFromConfig(aSection, KBitRate(), audioBitRate);
	TRAPD(err, iVideoRecorderUtility->SetAudioBitRateL(audioBitRate));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetAudioBitRateL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetAudioEnabledL(const TDesC& aSection)
	{

	TBool	audioEnabled=EFalse;
	GetBoolFromConfig(aSection, KEnabled(), audioEnabled);
	TRAPD(err, iVideoRecorderUtility->SetAudioEnabledL(audioEnabled));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetAudioEnabledL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetAudioTypeL(const TDesC& aSection)
	{
	TFourCC	val;
	if ( CT_MMUtil::ReadFourCC(*this, aSection, val) )
		{
		TRAPD(err, iVideoRecorderUtility->SetAudioTypeL(val));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("SetAudioTypeL Error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF1(_L("No data type"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetGainL(const TDesC& aSection)
	{

	TInt	gain=0;
	GetIntFromConfig(aSection, KGain(), gain);
	TRAPD(err, iVideoRecorderUtility->SetGainL(gain));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetGainL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetMaxClipSizeL(const TDesC& aSection)
	{

	TInt	maxClipSize=KMMFNoMaxClipSize;
	GetIntFromConfig(aSection, KMaxClipSize(), maxClipSize);
	TRAPD(err, iVideoRecorderUtility->SetMaxClipSizeL(maxClipSize));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetMaxClipSizeL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetPriorityL(const TDesC& aSection)
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

	TRAPD(err, iVideoRecorderUtility->SetPriorityL(priority, priorityPreference));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetPriorityL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetVideoBitRateL(const TDesC& aSection)
	{

	TInt	videoBitRate=KMMFVariableVideoBitRate;
	GetIntFromConfig(aSection, KBitRate(), videoBitRate);
	TRAPD(err, iVideoRecorderUtility->SetVideoBitRateL(videoBitRate));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetVideoBitRateL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetVideoFrameRateL(const TDesC& aSection)
	{

	TInt	videoFrameRate=KDefaultVideoFrameRate;
	GetIntFromConfig(aSection, KFrameRate(), videoFrameRate);
	TRAPD(err, iVideoRecorderUtility->SetVideoFrameRateL(TReal(videoFrameRate)));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetVideoFrameRateL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetVideoFrameSizeL(const TDesC& aSection)
	{

	TSize	videoFrameSize(0, 0);
	GetIntFromConfig(aSection, KWidth(), videoFrameSize.iWidth);
	GetIntFromConfig(aSection, KHeight(), videoFrameSize.iHeight);
	TRAPD(err, iVideoRecorderUtility->SetVideoFrameSizeL(videoFrameSize));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetVideoFrameSizeL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdSetVideoTypeL(const TDesC& aSection)
	{

	HBufC8*	videoType8=NULL;
	TPtrC	videoType;
	if ( GetStringFromConfig(aSection, KVideoType(), videoType) )
		{
		videoType8=HBufC8::NewL(videoType.Length());
		videoType8->Des().Copy(videoType);
		}
	else
		{
		videoType8=HBufC8::NewL(0);
		videoType8->Des().Copy(KNullDesC8);
		}
	TRAPD(err, iVideoRecorderUtility->SetVideoTypeL(*videoType8));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetVideoTypeL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdStop()
	{
	TInt	error=iVideoRecorderUtility->Stop();
	INFO_PRINTF1(_L("Stop recording"));
	if ( error!=KErrNone )
		{
		ERR_PRINTF2(_L("Stop Error %d"), error);
		SetError(error);
		}

	if(iRecordStarted)
		{
		iRecordStarted = EFalse;
		DecOutstanding();
		}

	}

void CT_MMDataVideoRecorderUtility::DoCmdVideoBitRateL(const TDesC& aSection)
	{

	TInt	bitRate=0;
	TRAPD(err, bitRate=iVideoRecorderUtility->VideoBitRateL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("VideoBitRateL Error %d"), err);
		SetError(err);
		}
	else
		{
		if ( bitRate==KMMFVariableVideoBitRate )
			{
			INFO_PRINTF1(_L("VideoBitRateL=KMMFVariableVideoBitRate"));
			}
		else
			{
			INFO_PRINTF2(_L("VideoBitRateL=%d"), bitRate);
			}
		TInt	expectedBitRate=0;
		if(GetIntFromConfig(aSection, KBitRate(), expectedBitRate))
			{
			if(bitRate != expectedBitRate)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("videoBitRate(%d) != expectedVideoBitRate(%d)"), bitRate, expectedBitRate);
				}
			}
		}
	}

void CT_MMDataVideoRecorderUtility::DoCmdVideoFormatMimeTypeL(const TDesC& aSection)
	{

	TPtrC8	mimeType=iVideoRecorderUtility->VideoFormatMimeType();
	HBufC*	mime=HBufC::NewLC(mimeType.Length());
	TPtr	mimeDes=mime->Des();
	mimeDes.Copy(mimeType);
	INFO_PRINTF2(_L("VideoFormatMimeType=%S"), &mimeDes);

	TPtrC	expectedMimeType;
	if(GetStringFromConfig(aSection, KVideoType(), expectedMimeType))
		{
		if(mimeDes != expectedMimeType)
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("mimeType(%S) != expectedMimeType(%S)"), &mimeDes, &expectedMimeType);
			}
		}

	CleanupStack::PopAndDestroy(mime);
	}

void CT_MMDataVideoRecorderUtility::DoCmdVideoFrameRateL(const TDesC& aSection)
	{

	TReal	frameRate=0;
	TRAPD(err, frameRate=iVideoRecorderUtility->VideoFrameRateL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("VideoFrameRateL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("VideoFrameRateL=%f"), frameRate);
		TInt	expectedFrameRate=0;
		if(GetIntFromConfig(aSection, KFrameRate(), expectedFrameRate))
			{
			if(TInt(frameRate) != expectedFrameRate)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("videoFrameRate(%f) != expectedVideoFrameRate(%d)"), frameRate, expectedFrameRate);
				}
			}
		}
	}

void CT_MMDataVideoRecorderUtility::ReadCameraHandleL(const TDesC& aSection, TInt& aCameraHandle)
	{
	TPtrC	cameraName;
	if ( GetStringFromConfig(aSection, KCamera(), cameraName) )
		{
		CCamera*	camera=static_cast<CCamera*>(GetDataObjectL(cameraName));
		if ( camera==NULL )
			{
			ERR_PRINTF2(_L("Camera %S not found"), &cameraName);
			SetBlockResult(EFail);
			}
		else
			{
			aCameraHandle=camera->Handle();
			}
		}
	else
		{
		if ( !GetIntFromConfig(aSection, KCameraHandle(), aCameraHandle) )
			{
			ERR_PRINTF1(_L("No camera or camera handle specified"));
			SetBlockResult(EFail);
			}
		}
	}

///////////////////////////////////////////////////////////////////
//from the MVideoRecorderUtilityObserver interface  / / / / / / / /
///////////////////////////////////////////////////////////////////

/**
Asynchronous callback called when CVideoRecorderUtility::Open completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if opened ok
@pre N/A
@post N/A
*/
void CT_MMDataVideoRecorderUtility::MvruoOpenComplete(TInt aError)
	{
	if ( aError==KErrNone )
		{
		INFO_PRINTF1(_L("Open OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Open Error %d"), aError);
		SetAsyncError(iApiAsyncErrorIndex, aError);
		}
	DecOutstanding();
	}

/**
Asynchronous callback called when CVideoRecorderUtility::Prepare completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if prepared ok
@pre N/A
@post N/A
*/
void CT_MMDataVideoRecorderUtility::MvruoPrepareComplete(TInt aError)
	{
	if ( aError==KErrNone )
		{
		INFO_PRINTF1(_L("Prepare OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Prepare Error %d"), aError);
		SetAsyncError(iApiAsyncErrorIndex, aError);
		}
	DecOutstanding();
	}

/**
Asynchronous callback called when CVideoRecorderUtility::Record completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if recorded ok
@pre N/A
@post N/A
*/
void CT_MMDataVideoRecorderUtility::MvruoRecordComplete(TInt aError)
	{
	if ( aError==KErrNone )
		{
		INFO_PRINTF1(_L("MvruoRecordComplete: Record OK"));
		}
	else
		{
		ERR_PRINTF2(_L("MvruoRecordComplete: Record Error %d"), aError);
		SetAsyncError(iApiAsyncErrorIndex, aError);
		}
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	iRecordStarted = EFalse;
	DecOutstanding();
	}

/**
Asynchronous callback called when general event notification from controller received
@internalAll
@return void
@param aEvent The event sent by the controller
@pre N/A
@post N/A
*/
void CT_MMDataVideoRecorderUtility::MvruoEvent(const TMMFEvent &aEvent)
	{
	if(aEvent.iErrorCode == KErrNone )
		{
		INFO_PRINTF1(_L("MvruoEvent OK"));
		INFO_PRINTF2(_L("MvruoEvent event type %d"), aEvent.iEventType);
		}
	else
		{
		ERR_PRINTF2(_L("MvruoEvent Error %d"), aEvent.iErrorCode);
		SetAsyncError(iApiAsyncErrorIndex, aEvent.iErrorCode);
		}

	DecOutstanding();
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
void CT_MMDataVideoRecorderUtility::RunL(CActive* aActive, TInt aIndex)
	{
	if ( aActive==iActiveCallback )
		{
		RunActiveL(aIndex);
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
void CT_MMDataVideoRecorderUtility::DoCancel(CActive* aActive, TInt aIndex)
	{
	if ( aActive==iActiveCallback )
		{
		SetAsyncError(aIndex, aActive->iStatus.Int());
		DoCancelActive();
		}
	else
		{
		ERR_PRINTF1(_L("Stray signal"));
		SetBlockResult(EFail);
		}
	}

/**
 RunActiveL - Process RunL for iActive
 @internalComponent
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_MMDataVideoRecorderUtility::RunActiveL(TInt aIndex)
	{
	TInt	err=iActiveCallback->iStatus.Int();
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError(aIndex, err);
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
void CT_MMDataVideoRecorderUtility::DoCancelActive()
	{

	}

/**
Helper method for deleting files
*/
void CT_MMDataVideoRecorderUtility::DeleteFile(const TDesC& aSection)
	{
	TPtrC filename;
	if ( GetStringFromConfig(aSection, KCmdFilename(), filename) )
		{
		TInt err = BaflUtils::DeleteFile(iFs, filename);
		if ( err == KErrNone )
			{
			INFO_PRINTF2(_L("File deleted: %S"), &filename);
			}
		else
			{
			ERR_PRINTF2(_L("File deletion error: %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF1(_L("No filename given."));
		SetBlockResult(EFail);
		}
	}
