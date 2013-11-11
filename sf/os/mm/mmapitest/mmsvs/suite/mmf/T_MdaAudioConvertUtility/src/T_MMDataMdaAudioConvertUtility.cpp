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



#include "T_MMDataMdaAudioConvertUtility.h"
#include "T_MMUtil.h"

/*@{*/
_LIT(KPrimaryFile, 					"primaryfile");
_LIT(KSecondaryFile, 				"secondaryfile");
_LIT(KPriLocation, 					"prilocation");
_LIT(KSecLocation, 					"seclocation");
_LIT(KPriFormat, 					"priformat");
_LIT(KSecFormat, 					"secformat");
_LIT(KPriCodec, 					"pricodec");
_LIT(KSecCodec, 					"seccodec");
_LIT(KPriAudioSetings, 				"priaudiosetings");
_LIT(KSecAudioSetings, 				"secaudiosetings");
_LIT(KInvalidAudioSetings,			"invalidaudiosetings");
_LIT(KControllerUid, 				"controlleruid");
_LIT(KDestFormatUid, 				"destformatuid");
_LIT(KBitRate,	 					"bitrate");
_LIT(KSampleRate,	 				"samplerate");
_LIT(KFormat,	 					"format");
_LIT(KNrChnls,	 					"numberofchannels");
_LIT(KPosition,	 					"position");
_LIT(KStart,	 					"start");
_LIT(KEnd,	 						"end");
_LIT(KMaxLength,					"maxlength");
_LIT(KFunction,						"function");
_LIT(KUseDataFrom,					"usedatafrom");
_LIT(KFormatUid,					"formatuid");
_LIT(KExcpectedNumber,				"expectednumber");
_LIT(KServer,						"server");
_LIT(KRepeatNumber,					"repeatnumber");
_LIT(KInterval,						"interval");
_LIT(KState,						"state");
_LIT(KDuration,						"duration");
_LIT(KRecTime,						"rectime");
_LIT(KDataTo1,						"datato1");
_LIT(KDataTo2,						"datato2");
_LIT(KFullConvertRequest,			"waitforfullconvert");

_LIT(KCmdNewL,						"NewL");
_LIT(KCmdOpenL,						"OpenL");
_LIT(KCmdConvertL,					"ConvertL");
_LIT(KCmdSourceDataTypeL,			"SourceDataTypeL");
_LIT(KCmdGetSupportedDestDataTypesL,"GetSupportedDestDataTypesL");
_LIT(KCmdSetDestinationDataTypeL,	"SetDestinationDataTypeL");
_LIT(KCmdDestinationDataTypeL,		"DestinationDataTypeL");
_LIT(KCmdSourceBitRateL,			"SourceBitRateL");
_LIT(KCmdGetSupConversionBitRatesL,	"GetSupConversionBitRatesL");
_LIT(KCmdDestinationBitRateL,		"DestinationBitRateL");
_LIT(KCmdSetDestinationBitRateL,	"SetDestinationBitRateL");
_LIT(KCmdSourceSampleRateL,			"SourceSampleRateL");
_LIT(KCmdGetSupConvSampleRatesL,	"GetSupConvSampleRatesL");
_LIT(KCmdDestinationSampleRateL,	"DestinationSampleRateL");
_LIT(KCmdSetDestinationSampleRateL,	"SetDestinationSampleRateL");
_LIT(KCmdSourceFormatL,				"SourceFormatL");
_LIT(KCmdDestinationFormatL,		"DestinationFormatL");
_LIT(KCmdSetDestinationFormatL,		"SetDestinationFormatL");
_LIT(KCmdSourceNumberOfChannelsL,	"SourceNumberOfChannelsL");
_LIT(KCmdGetSupConvNrOfChnlsL,		"GetSupportedConversionNumberOfChannelsL");
_LIT(KCmdDestinationNrOfChannelsL,	"DestinationNumberOfChannelsL");
_LIT(KCmdSetDestinationNrOfChnlsL,	"SetDestinationNumberOfChannelsL");
_LIT(KCmdState,						"State");
_LIT(KCmdClose,						"Close");
_LIT(KCmdPlayL,						"PlayL");
_LIT(KCmdRecordL,					"RecordL");
_LIT(KCmdStop,						"Stop");
_LIT(KCmdPosition,					"Position");
_LIT(KCmdSetPosition,				"SetPosition");
_LIT(KCmdDuration,					"Duration");
_LIT(KCmdRecordTimeAvailable,		"RecordTimeAvailable");
_LIT(KCmdCropL,						"CropL");
_LIT(KCmdSetPlayWindow,				"SetPlayWindow");
_LIT(KCmdClearPlayWindow,			"ClearPlayWindow");
_LIT(KCmdSetMaxWriteLength,			"SetMaxWriteLength");
_LIT(KCmdCropFromBeginningL,		"CropFromBeginningL");
_LIT(KCmdControllerImplInfoL,		"ControllerImplementationInformationL");
_LIT(KCmdSetThreadPriority,			"SetThreadPriority");
_LIT(KCmdCustomCommandSync,			"CustomCommandSync");
_LIT(KCmdCustomCommandAsync,		"CustomCommandAsync");
_LIT(KCmdSetRepeats,				"SetRepeats");
_LIT(KCmdUseSharedHeap,				"UseSharedHeap");
_LIT(KCmdDestructor,				"~");

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


_LIT(KENotReady,					"ENotReady");
_LIT(KEOpen,						"EOpen");
_LIT(KEPlaying,						"EPlaying");
_LIT(KERecording,					"ERecording");
_LIT(KEUnknown,						"EUnknown");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMDataMdaAudioConvertUtility* CT_MMDataMdaAudioConvertUtility::NewL()
/**
 * Two phase constructor
 * @internalComponent
 *
 * @return	Data Dictionary object
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	CT_MMDataMdaAudioConvertUtility*	ret = new (ELeave) CT_MMDataMdaAudioConvertUtility();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_MMDataMdaAudioConvertUtility::CT_MMDataMdaAudioConvertUtility()
/**
 * CT_MMDataMdaAudioConvertUtility constructor.
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	CT_MMDataMdaAudioConvertUtility 1st stage constructor
 */
:	iConvertUtility(NULL)
,	iServer(NULL)
,	iActiveCustomCommandAsync(NULL)
,	iAsyncErrorIndex(0)
,	iOutstandingRepeats(0)
	{
	}

/**
Second phase construction
@internalComponent
@return N/A
@pre None
@post None
*/
void CT_MMDataMdaAudioConvertUtility::ConstructL()
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
	iActiveCustomCommandAsync = CActiveCallback::NewL(*this);
	iServer = CMdaServer::NewL();
	}

CT_MMDataMdaAudioConvertUtility::~CT_MMDataMdaAudioConvertUtility()
/**
 * CT_MMDataMdaAudioConvertUtility destructor
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	releases CT_MMDataMdaAudioConvertUtility heap memory
 */
	{
	DestroyData();

  	delete iServer;
  	iServer = NULL;

	delete iActiveCustomCommandAsync;
	iActiveCustomCommandAsync = NULL;
	}

TAny* CT_MMDataMdaAudioConvertUtility::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iConvertUtility;
	}

/**
Releases CT_MMDataMdaAudioConvertUtility dynamic memory.
@internalComponent
@return void
@pre None
@post All dynamic memory deallocated
*/
void CT_MMDataMdaAudioConvertUtility::DestroyData()
	{
	delete iConvertUtility;
	iConvertUtility = NULL;
	}

TBool CT_MMDataMdaAudioConvertUtility::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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

	if(aAsyncErrorIndex > 0) iAsyncErrorIndex = aAsyncErrorIndex;

	if (aCommand == KCmdNewL)
		{
		DoCmdConstructor(aSection);
		}
	else if (aCommand == KCmdOpenL)
		{
		DoCmdOpenL(aSection);
		}
	else if(aCommand == KCmdConvertL)
		{
		DoCmdConvertL(aSection);
		}
	else if(aCommand == KCmdSourceDataTypeL)
		{
		DoCmdSourceDataTypeL(aSection);
		}
	else if(aCommand == KCmdGetSupportedDestDataTypesL)
		{
		DoCmdGetSupportedDestDataTypesL(aSection);
		}
	else if(aCommand == KCmdSetDestinationDataTypeL)
		{
		DoCmdSetDestinationDataTypeL(aSection);
		}
	else if(aCommand == KCmdDestinationDataTypeL)
		{
		DoCmdDestinationDataTypeL(aSection);
		}
	else if(aCommand == KCmdSourceBitRateL)
		{
		DoCmdSourceBitRateL(aSection);
		}
	else if(aCommand == KCmdGetSupConversionBitRatesL)
		{
		DoCmdGetSupConversionBitRatesL(aSection);
		}
	else if(aCommand == KCmdDestinationBitRateL)
		{
		DoCmdDestinationBitRateL(aSection);
		}
	else if(aCommand == KCmdSetDestinationBitRateL)
		{
		DoCmdSetDestinationBitRateL(aSection);
		}
	else if(aCommand == KCmdSourceSampleRateL)
		{
		DoCmdSourceSampleRateL(aSection);
		}
	else if(aCommand == KCmdGetSupConvSampleRatesL)
		{
		DoCmdGetSupConvSampleRatesL(aSection);
		}
	else if(aCommand == KCmdDestinationSampleRateL)
		{
		DoCmdDestinationSampleRateL(aSection);
		}
	else if(aCommand == KCmdSetDestinationSampleRateL)
		{
		DoCmdSetDestinationSampleRateL(aSection);
		}
	else if(aCommand == KCmdSourceFormatL)
		{
		DoCmdSourceFormatL(aSection);
		}
	else if(aCommand == KCmdDestinationFormatL)
		{
		DoCmdDestinationFormatL(aSection);
		}
	else if(aCommand == KCmdSetDestinationFormatL)
		{
		DoCmdSetDestinationFormatL(aSection);
		}
	else if(aCommand == KCmdSourceNumberOfChannelsL)
		{
		DoCmdSourceNumberOfChannelsL(aSection);
		}
	else if(aCommand == KCmdGetSupConvNrOfChnlsL)
		{
		DoCmdGetSupConvNrOfChnlsL(aSection);
		}
	else if(aCommand == KCmdDestinationNrOfChannelsL)
		{
		DoCmdDestinationNrOfChannelsL(aSection);
		}
	else if(aCommand == KCmdSetDestinationNrOfChnlsL)
		{
		DoCmdSetDestinationNrOfChnlsL(aSection);
		}
	else if(aCommand == KCmdState)
		{
		DoCmdState(aSection);
		}
	else if(aCommand == KCmdClose)
		{
		DoCmdClose();
		}
	else if(aCommand == KCmdPlayL)
		{
		DoCmdPlayL();
		}
	else if(aCommand == KCmdRecordL)
		{
		DoCmdRecordL();
		}
	else if(aCommand == KCmdStop)
		{
		DoCmdStop();
		}
	else if(aCommand == KCmdPosition)
		{
		DoCmdPosition(aSection);
		}
	else if(aCommand == KCmdSetPosition)
		{
		DoCmdSetPosition(aSection);
		}
	else if(aCommand == KCmdDuration)
		{
		DoCmdDuration(aSection);
		}
	else if(aCommand == KCmdRecordTimeAvailable)
		{
		DoCmdRecordTimeAvailable(aSection);
		}
	else if(aCommand == KCmdCropL)
		{
		DoCmdCropL();
		}
	else if(aCommand == KCmdSetPlayWindow)
		{
		DoCmdSetPlayWindow(aSection);
		}
	else if(aCommand == KCmdClearPlayWindow)
		{
		DoCmdClearPlayWindow();
		}
	else if(aCommand == KCmdSetMaxWriteLength)
		{
		DoCmdSetMaxWriteLength(aSection);
		}
	else if(aCommand == KCmdCropFromBeginningL)
		{
		DoCmdCropFromBeginningL();
		}
	else if(aCommand == KCmdControllerImplInfoL)
		{
		DoCmdControllerImplInfoL();
		}
	else if (aCommand == KCmdCustomCommandSync)
		{
		DoCmdCustomCommandSyncL(aSection);
		}
	else if (aCommand == KCmdCustomCommandAsync)
		{
		DoCmdCustomCommandAsyncL(aSection);
		}
	else if(aCommand == KCmdSetThreadPriority)
		{
		DoCmdSetThreadPriority(aSection);
		}
	else if (aCommand == KCmdSetRepeats)
		{
		DoCmdSetRepeats(aSection);
		}
	else if (aCommand == KCmdUseSharedHeap)
		{
		DoCmdUseSharedHeap();
		}
	else if(aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdConstructor(const TDesC& aSection)
	{

	TInt	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority = EMdaPriorityNormal;
		}

	TMdaPriorityPreference	priorityPref;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPref) )
		{
		priorityPref=EMdaPriorityPreferenceTimeAndQuality;
		}

	delete iConvertUtility;
	iConvertUtility = NULL;
	TBool	useServer = EFalse;
	GetBoolFromConfig(aSection, KServer(), useServer);
	if (useServer)
		{
		TRAPD(err, iConvertUtility = CMdaAudioConvertUtility::NewL(*this, iServer, priority, priorityPref));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CMdaAudioConvertUtility::NewL failed with error %d."), err);
			SetError(err);
			}
		}
	else
		{
		TRAPD(err, iConvertUtility = CMdaAudioConvertUtility::NewL(*this, NULL, priority, priorityPref));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CMdaAudioConvertUtility::NewL failed with error %d."), err);
			SetError(err);
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdOpenL(const TDesC& aSection)
	{

	// @param const TDesC &aPrimaryFile
	TPtrC	primaryFile;
	TBool	hasPrimaryFile = GetStringFromConfig(aSection, KPrimaryFile(), primaryFile);

	// @param const TDesC &aSecondaryFile
	TPtrC	secondaryFile;
	TBool	hasSecondaryFile = GetStringFromConfig(aSection, KSecondaryFile(), secondaryFile);

	// @param TMdaClipLocation *aPriLocation

	TMdaClipLocation*		priLocation=NULL;
	TPtrC					priLocationStr(_L(""));
	if (GetStringFromConfig(aSection, KPriLocation(), priLocationStr))
		{
		priLocation=new (ELeave) TMdaFileClipLocation(priLocationStr);
		}
	CleanupDeletePushL(priLocation);

	// @param TMdaClipLocation *aSecLocation
	TMdaClipLocation*		secLocation=NULL;
	TPtrC					secLocationStr(_L(""));
	if (GetStringFromConfig(aSection, KSecLocation(), secLocationStr))
		{
		secLocation=new (ELeave) TMdaFileClipLocation(secLocationStr);
		}
	CleanupDeletePushL(secLocation);

	// @param TMdaClipFormat *aPriFormat
	TMdaClipFormat*	priFormat = NULL;
	TPtrC			priFormatStr;
	if (GetStringFromConfig(aSection, KPriFormat(), priFormatStr))
		{
		if (GetFormatL(priFormatStr, priFormat, aSection))
			{
			INFO_PRINTF3(_L("Format (%S) Uid = 0x%X"), &priFormatStr, priFormat->Uid().iUid);
			}
		else
			{
			ERR_PRINTF2(_L("Can not call OpenL. Parameter %S has invalid value in INI file."), &KPriFormat());
			SetBlockResult(EFail);
			}
		}
	CleanupDeletePushL(priFormat);

	// @param TMdaClipFormat *aSecFormat
	TMdaClipFormat*		secFormat = NULL;
	TPtrC				secFormatStr;
	if (GetStringFromConfig(aSection, KSecFormat(), secFormatStr))
		{
		if (GetFormatL(secFormatStr, secFormat, aSection))
			{
			INFO_PRINTF3(_L("Format (%S) Uid = 0x%X"), &secFormatStr, secFormat->Uid().iUid);
			}
		else
			{
			ERR_PRINTF2(_L("Can not call OpenL. Parameter %S has invalid value in INI file."), &KSecFormat());
			SetBlockResult(EFail);
			}
		}
	CleanupDeletePushL(secFormat);

	// @param TMdaPackage *aPriArg1
	TMdaPackage*	priCodec = NULL;
	TPtrC			priCodecStr;
	if (GetStringFromConfig(aSection, KPriCodec(), priCodecStr))
		{
		if (GetCodecL(priCodecStr, priCodec))
			{
			INFO_PRINTF3(_L("Codec (%S) Uid = 0x%X"), &priCodecStr, priCodec->Uid().iUid);
			}
		else
			{
			ERR_PRINTF2(_L("Can not call OpenL. Parameter %S has invalid value in INI file."), &KPriCodec());
			SetBlockResult(EFail);
			}
		}
	CleanupDeletePushL(priCodec);

	// @param TMdaPackage *aPriArg2
	TMdaPackage*	secCodec = NULL;
	TPtrC			secCodecStr;
	if (GetStringFromConfig(aSection, KSecCodec(), secCodecStr))
		{
		if (GetCodecL(secCodecStr, secCodec))
			{
			INFO_PRINTF3(_L("Codec (%S) Uid = 0x%X"), &secCodecStr, secCodec->Uid().iUid);
			}
		else
			{
			ERR_PRINTF2(_L("Can not call OpenL. Parameter %S has invalid value in INI file."), &KSecCodec());
			SetBlockResult(EFail);
			}
		}
	CleanupDeletePushL(secCodec);

	// @param TMdaPackage *aSecArg1
	TMdaPackage*	priSettings = NULL;
	GetAudioSettingsL(KPriAudioSetings(), *this, aSection, priSettings);
	CleanupDeletePushL(priSettings);

	// @param TMdaPackage *aSecArg2
	TMdaPackage*	secSettings = NULL;
	GetAudioSettingsL(KSecAudioSetings(), *this, aSection, secSettings);
	CleanupDeletePushL(secSettings);

	// @param The UID of the destination format.
	TUid	destFormatUid = TUid::Null();
	TInt	formatUid = 0;
	if (GetHexFromConfig(aSection, KDestFormatUid(), formatUid))
		{
		destFormatUid=TUid::Uid(formatUid);
		INFO_PRINTF3(_L("Format input required: %d, and the format read in from ini file: %d."),destFormatUid.iUid,formatUid);
		}

	// @param The UID of the controller to used for conversion.
	CT_MMUtil::GetControllerAndFormatUidL(*this, secLocationStr, KUidMediaTypeAudio, ETrue, iAudioControllerUid, iAudioFormatUid);

	TInt	destControllerUid;
	if (GetHexFromConfig(aSection, KControllerUid(), destControllerUid))
		{
		iAudioControllerUid = TUid::Uid(destControllerUid);
		}

	// @param The audio codec to use for the destination data sink
	TFourCC	destDataType;
	TBool	hasFourCC = CT_MMUtil::ReadFourCC(*this, aSection, destDataType);

	if ( ETrue )
		{
		if ( hasPrimaryFile )
			{
			if ( hasSecondaryFile )
				{
				TRAPD(err, iConvertUtility->OpenL(primaryFile, secondaryFile));

				if ( err != KErrNone )
					{
					ERR_PRINTF2(_L("CMdaAudioConvertUtility::OpenL failed with error %d."), err);
					SetError(err);
					}
				else
					{
					IncOutstanding();
					}
				}
			else
				{
				TRAPD(err, iConvertUtility->OpenL(primaryFile, secLocation, secFormat, secCodec, priSettings));
				if ( err != KErrNone )
					{
					ERR_PRINTF2(_L("CMdaAudioConvertUtility::OpenL failed with error %d."), err);
					SetError(err);
					}
				else
					{
					IncOutstanding();
					}
				}
			}
		else
			{
			if ( hasFourCC )
				{
				INFO_PRINTF2(_L("Audio Controller Uid = 0x%X"), iAudioControllerUid.iUid);
				TRAPD(err, iConvertUtility->OpenL(priLocation, secLocation, iAudioControllerUid, destFormatUid, destDataType));

				if ( err != KErrNone )
					{
					ERR_PRINTF2(_L("CMdaAudioConvertUtility::OpenL failed with error %d."), err);
					SetError(err);
					}
				else
					{
					IncOutstanding();
					}
				}
			else
				{
				TRAPD(err, iConvertUtility->OpenL(priLocation, secLocation, priFormat, secFormat, priCodec, priSettings, secCodec, secSettings));

				if ( err != KErrNone )
					{
					ERR_PRINTF2(_L("CMdaAudioConvertUtility::OpenL failed with error %d."), err);
					SetError(err);
					}
				else
					{
					IncOutstanding();
					}
				}
			}
		}

	CleanupStack::PopAndDestroy(8, priLocation);
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdConvertL(const TDesC& aSection)
	{
	TRAPD(err, iConvertUtility->ConvertL());
	//	ConvertL expects two callbacks
	//	1.	Set state to EPlay when the convert starts
	//	2.	Set state to EOpen when the convert completes
	// 	This function deals with waiting for both call backs
	// 	or waiting for the intial  state (EPlay) only
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::ConvertL failed with error %d."), err);
		SetError(err);
		}
	else
		{	
		TBool	fullConvertRequest=ETrue;
		GetBoolFromConfig(aSection, KFullConvertRequest(),  fullConvertRequest);
		if ( fullConvertRequest )
			{
			iOutstandingRepeats=1; //Flag reminder to decrement outsanding
			IncOutstanding();
			IncOutstanding();
			}
		else
			{
			INFO_PRINTF1(_L("ConvertL partial convert only."));
			iOutstandingRepeats=0;
			IncOutstanding();// wait for EPlay only
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSourceDataTypeL(const TDesC& aSection)
	{
	TFourCC	sourceDataType;
	TRAPD(err, sourceDataType=iConvertUtility->SourceDataTypeL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::SourceDataTypeL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Data type of the source audio clip: %d."), sourceDataType.FourCC());

		TFourCC	expectedDataType;
		if ( CT_MMUtil::ReadFourCC(*this, aSection, expectedDataType) )
			{
			if ( sourceDataType != expectedDataType )
				{
				ERR_PRINTF3(_L("Source datatype actual(%d)!=expected(%d)"), sourceDataType.FourCC(), expectedDataType.FourCC());
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdGetSupportedDestDataTypesL(const TDesC& aSection)
	{
	RArray<TFourCC>	supportedDestinationDataTypes;
	CleanupClosePushL(supportedDestinationDataTypes);
	TRAPD(err, iConvertUtility->GetSupportedDestinationDataTypesL(supportedDestinationDataTypes));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::GetSupportedDestinationDataTypesL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported data types for the conversion destination:"));
		for (TInt i=0; i < supportedDestinationDataTypes.Count(); i++)
			{
			INFO_PRINTF2(_L("%S"), &CT_MMUtil::GetFourCCString(supportedDestinationDataTypes[i].FourCC()));
			}
		TInt	expectedDatatypesNr;
		if (GetIntFromConfig(aSection, KExcpectedNumber(), expectedDatatypesNr))
			{
			if (supportedDestinationDataTypes.Count() != expectedDatatypesNr)
				{
				ERR_PRINTF3(_L("Number of supported datatypes actual(%d)!=expected(%d)"),
					supportedDestinationDataTypes.Count(), expectedDatatypesNr);
				SetBlockResult(EFail);
				}
			}
		}
	CleanupStack::PopAndDestroy(&supportedDestinationDataTypes);
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetDestinationDataTypeL(const TDesC& aSection)
	{
	TFourCC	dataType;
	CT_MMUtil::ReadFourCC(*this, aSection, dataType);

	TRAPD(err, iConvertUtility->SetDestinationDataTypeL(dataType));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationDataTypeL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdDestinationDataTypeL(const TDesC& aSection)
	{
	TFourCC	actualDataType;
	TRAPD(err, actualDataType=iConvertUtility->DestinationDataTypeL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::DestinationDataTypeL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Data type of the destination audio clip: %d."), actualDataType.FourCC());

		TFourCC	expectedDataType;
		if ( CT_MMUtil::ReadFourCC(*this, aSection, expectedDataType) )
			{
			if ( actualDataType != expectedDataType )
				{
				ERR_PRINTF3(_L("Source datatype actual(%d)!=expected(%d)"), actualDataType.FourCC(), expectedDataType.FourCC());
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSourceBitRateL(const TDesC& aSection)
	{
	TInt	sourceBitRate = 0;
	TRAPD(err, sourceBitRate = iConvertUtility->SourceBitRateL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::SourceBitRateL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Bit rate of the source audio clip: %d."), sourceBitRate);

		TInt	expectedBitRate;
		if (GetIntFromConfig(aSection, KBitRate(), expectedBitRate))
			{
			if (sourceBitRate != expectedBitRate)
				{
				ERR_PRINTF3(_L("Source bit rate actual(%d)!=expected(%d)"),
					sourceBitRate, expectedBitRate);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdGetSupConversionBitRatesL(const TDesC& aSection)
	{
	RArray<TUint>	supportedBitRates;
	CleanupClosePushL(supportedBitRates);
	TRAPD(err, iConvertUtility->GetSupportedConversionBitRatesL(supportedBitRates));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::GetSupportedConversionBitRatesL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported bit rates for the destination:"));
		for (TInt i=0; i < supportedBitRates.Count(); i++)
			{
			INFO_PRINTF2(_L("%d"), supportedBitRates[i]);
			}

		TInt	expectedBitRatesNr;
		if (GetIntFromConfig(aSection, KExcpectedNumber(), expectedBitRatesNr))
			{
			if (supportedBitRates.Count() != expectedBitRatesNr)
				{
				ERR_PRINTF3(_L("Number of supported bit rates actual(%d)!=expected(%d)"),
					supportedBitRates.Count(), expectedBitRatesNr);
				SetBlockResult(EFail);
				}
			}
		}
	CleanupStack::PopAndDestroy(&supportedBitRates);
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdDestinationBitRateL(const TDesC& aSection)
	{
	TInt	actualBitRate = 0;
	TRAPD(err, actualBitRate=iConvertUtility->DestinationBitRateL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::DestinationBitRateL failed with error %d."), err);
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
				ERR_PRINTF3(_L("Destiantion bit rate actual(%d)!=expected(%d)"),
					actualBitRate, expectedBitRate);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetDestinationBitRateL(const TDesC& aSection)
	{
	TInt	bitRate = 0;
	if (GetIntFromConfig(aSection, KBitRate(), bitRate))
		{
		TRAPD(err, iConvertUtility->SetDestinationBitRateL(bitRate));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationBitRateL failed with error %d."), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Can not call SetDestinationBitRateL. Parameter %S is missing in INI file."), &KBitRate());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSourceSampleRateL(const TDesC& aSection)
	{
	TInt	sourceSampleRate = 0;
	TRAPD(err, sourceSampleRate = iConvertUtility->SourceSampleRateL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::SourceSampleRateL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Sample rate of the source audio clip: %d."), sourceSampleRate);

		TInt	expectedSampleRate;
		if (GetIntFromConfig(aSection, KSampleRate(), expectedSampleRate))
			{
			if (sourceSampleRate != expectedSampleRate)
				{
				ERR_PRINTF3(_L("Source sample rate actual(%d)!=expected(%d)"),
					sourceSampleRate, expectedSampleRate);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdGetSupConvSampleRatesL(const TDesC& aSection)
	{
	RArray<TUint>	supportedSampleRates;
	CleanupClosePushL(supportedSampleRates);
	TRAPD(err, iConvertUtility->GetSupportedConversionSampleRatesL(supportedSampleRates));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::GetSupportedConversionSampleRatesL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported sample rates for the destination:"));
		for (TInt i=0; i < supportedSampleRates.Count(); i++)
			{
			INFO_PRINTF2(_L("%d"), supportedSampleRates[i]);
			}
		TInt	expectedConvSampleRatesNr;
		if (GetIntFromConfig(aSection, KExcpectedNumber(), expectedConvSampleRatesNr))
			{
			if (supportedSampleRates.Count() != expectedConvSampleRatesNr)
				{
				ERR_PRINTF3(_L("Number of supported conv. sample rates actual(%d)!=expected(%d)"),
					supportedSampleRates.Count(), expectedConvSampleRatesNr);
				SetBlockResult(EFail);
				}
			}
		}
	CleanupStack::PopAndDestroy(&supportedSampleRates);
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdDestinationSampleRateL(const TDesC& aSection)
	{
	TInt	actualSampleRate = 0;
	TRAPD(err, actualSampleRate = iConvertUtility->DestinationSampleRateL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::DestinationSampleRateL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Samplet rate of the destination audio clip: %d."), actualSampleRate);

		TInt	expectedSampleRate;
		if (GetIntFromConfig(aSection, KSampleRate(), expectedSampleRate))
			{
			if (actualSampleRate != expectedSampleRate)
				{
				ERR_PRINTF3(_L("Destiantion sample rate actual(%d)!=expected(%d)"),
					actualSampleRate, expectedSampleRate);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetDestinationSampleRateL(const TDesC& aSection)
	{
	TInt	sampleRate = 0;
	if (GetIntFromConfig(aSection, KSampleRate(), sampleRate))
		{
		TRAPD(err, iConvertUtility->SetDestinationSampleRateL(sampleRate));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationSampleRateL failed with error %d."), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Can not call SetDestinationSampleRateL. Parameter %S is missing in INI file."), &KSampleRate());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSourceFormatL(const TDesC& aSection)
	{
	TUid	sourceFormat;
	TRAPD(err, sourceFormat = iConvertUtility->SourceFormatL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::SourceFormatL failed with error %d."), err);
		SetError(err);
		}
	else
		{

		INFO_PRINTF2(_L("Format UID of the source audio clip: %d."), sourceFormat);

		TUid	expectedFormat = TUid::Null();
		TInt	formatUid = 0;
			
		if (GetHexFromConfig(aSection, KFormat(), formatUid))
			{
			expectedFormat=TUid::Uid(formatUid);
			INFO_PRINTF3(_L("Format input required: %d, and the format read in from ini file: %d."),expectedFormat.iUid,formatUid);
			if (sourceFormat != expectedFormat)
				{
				ERR_PRINTF3(_L("Format of the source audio clip actual(%d)!=expected(%d)"),
					sourceFormat,expectedFormat);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdDestinationFormatL(const TDesC& aSection)
	{
	TUid	actualFormat;
	TRAPD(err, actualFormat = iConvertUtility->DestinationFormatL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::DestinationFormatL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Format UID of the destination audio clip: %d."), actualFormat);

		TUid	expectedFormat = TUid::Null();
		TInt	formatUid = 0;
		if (GetHexFromConfig(aSection, KFormat(), formatUid))
			{
			expectedFormat=TUid::Uid(formatUid);
			INFO_PRINTF3(_L("Format input required: %d, and the format read in from ini file: %d."),expectedFormat.iUid,formatUid);
			if (actualFormat != expectedFormat)
				{
				ERR_PRINTF3(_L("Format UID's of the destination audio clip actual(%d)!=expected(%d)"),
					actualFormat, expectedFormat);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetDestinationFormatL(const TDesC& aSection)
	{
	TUid	expectedFormat = TUid::Null();
	TInt	formatUid = 0;
	if (GetHexFromConfig(aSection, KFormat(), formatUid))
		{
		expectedFormat=TUid::Uid(formatUid);;
		TRAPD(err, iConvertUtility->SetDestinationFormatL(expectedFormat));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationFormatL failed with error %d."), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Can not call SetDestinationFormatL. Parameter %S is missing in INI file."), &KFormat());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSourceNumberOfChannelsL(const TDesC& aSection)
	{
	TInt	sourceNrChannels = 0;
	TRAPD(err, sourceNrChannels = iConvertUtility->SourceNumberOfChannelsL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::SourceNumberOfChannelsL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Number of channels of the source audio clip: %d."), sourceNrChannels);

		TInt	expectedNrChnls;
		if (GetIntFromConfig(aSection, KNrChnls(), expectedNrChnls))
			{
			if (sourceNrChannels != expectedNrChnls)
				{
				ERR_PRINTF3(_L("Source number of channels actual(%d)!=expected(%d)"),
					sourceNrChannels, expectedNrChnls);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdGetSupConvNrOfChnlsL(const TDesC& aSection)
	{
	RArray<TUint>	supportedNrChnls;
	CleanupClosePushL(supportedNrChnls);
	TRAPD(err, iConvertUtility->GetSupportedConversionNumberOfChannelsL(supportedNrChnls));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::GetSupportedConversionNumberOfChannelsL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported number of channels for the destination:"));
		for (TInt i=0; i < supportedNrChnls.Count(); i++)
			{
			INFO_PRINTF2(_L("%d"), supportedNrChnls[i]);
			}

		TInt	expectedConvNrOfChnlsNr;
		if (GetIntFromConfig(aSection, KExcpectedNumber(), expectedConvNrOfChnlsNr))
			{
			if (supportedNrChnls.Count() != expectedConvNrOfChnlsNr)
				{
				ERR_PRINTF3(_L("Number of supported number of channels actual(%d)!=expected(%d)"),
					supportedNrChnls.Count(), expectedConvNrOfChnlsNr);
				SetBlockResult(EFail);
				}
			}
		}
	CleanupStack::PopAndDestroy(&supportedNrChnls);
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdDestinationNrOfChannelsL(const TDesC& aSection)
	{
	TInt	actualNrChnls = 0;
	TRAPD(err, actualNrChnls = iConvertUtility->DestinationNumberOfChannelsL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::DestinationNumberOfChannelsL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Number of channels of the destination audio clip: %d."), actualNrChnls);

		TInt	expectedNrChnls;
		if (GetIntFromConfig(aSection, KNrChnls(), expectedNrChnls))
			{
			if (actualNrChnls != expectedNrChnls)
				{
				ERR_PRINTF3(_L("Destiantion number of channels actual(%d)!=expected(%d)"),
					actualNrChnls, expectedNrChnls);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetDestinationNrOfChnlsL(const TDesC& aSection)
	{
	TInt	channels = 0;
	if (GetIntFromConfig(aSection, KNrChnls(), channels))
		{
		TRAPD(err, iConvertUtility->SetDestinationNumberOfChannelsL(channels));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationNumberOfChannelsL failed with error %d."), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Can not call SetDestinationNumberOfChannelsL. Parameter %S is missing in INI file."), &KNrChnls());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdState(const TDesC& aSection)
	{
	TInt	state = iConvertUtility->State();
	INFO_PRINTF2(_L("Current utility state: %S."), &GetState(state));

	TPtrC	expectedStateStr;
	if (GetStringFromConfig(aSection, KState(), expectedStateStr))
		{
		if (GetState(state) != expectedStateStr)
			{
			ERR_PRINTF3(_L("State actual(%S)!=expected(%S)"),
				&GetState(state), &expectedStateStr);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdClose()
	{
	iConvertUtility->Close();
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdPlayL()
	{
	TRAPD(err, iConvertUtility->PlayL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::PlayL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdRecordL()
	{
	TRAPD(err, iConvertUtility->RecordL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::RecordL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdStop()
	{
	iConvertUtility->Stop();
	IncOutstanding();
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdPosition(const TDesC& aSection)
	{
	TTimeIntervalMicroSeconds actualPosition = iConvertUtility->Position();
	INFO_PRINTF2(_L("Current position in the audio clip: %d"), actualPosition.Int64());

	TInt	expectedPosition;
	if (GetIntFromConfig(aSection, KPosition(), expectedPosition))
		{
		if (actualPosition != expectedPosition)
			{
			ERR_PRINTF3(_L("Position in the audio clip actual(%d)!=expected(%d)"),
				actualPosition.Int64(), expectedPosition);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetPosition(const TDesC& aSection)
	{
	TInt	position;
	if (GetIntFromConfig(aSection, KPosition(), position))
		{
		iConvertUtility->SetPosition(position);
		}
	else
		{
		ERR_PRINTF2(_L("Can not call SetPosition. Parameter %S is missing in INI file."), &KPosition());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdDuration(const TDesC& aSection)
	{
	TTimeIntervalMicroSeconds	duration = iConvertUtility->Duration();
	INFO_PRINTF2(_L("Duration of the audio clip: %d"), duration.Int64());

	TInt	expectedDuration;
	if (GetIntFromConfig(aSection, KDuration(), expectedDuration))
		{
		if (duration != expectedDuration)
			{
			ERR_PRINTF3(_L("Duration of the audio clip actual(%d)!=expected(%d)"),
				duration.Int64(), expectedDuration);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdRecordTimeAvailable(const TDesC& aSection)
	{
	TTimeIntervalMicroSeconds	recTime = iConvertUtility->RecordTimeAvailable();
	INFO_PRINTF2(_L("Amount of recording time available to the current clip: %d"), recTime.Int64());

	TInt	expectedRecTime;
	if (GetIntFromConfig(aSection, KRecTime(), expectedRecTime))
		{
		if (recTime != expectedRecTime)
			{
			ERR_PRINTF3(_L("Record time available actual(%d)!=expected(%d)"),
				recTime.Int64(), expectedRecTime);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdCropL()
	{
	TRAPD(err, iConvertUtility->CropL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::CropL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetPlayWindow(const TDesC& aSection)
	{
	TInt	startPosition = 0;
	TInt	endPosition = 0;

	TBool hasStartPos = GetIntFromConfig(aSection, KStart(), startPosition);
	if (!hasStartPos)
		{
		ERR_PRINTF2(_L("Can not call SetPlayWindow. Parameter %S is missing in INI file."), &KStart());
		}

	TBool hasEndPos = GetIntFromConfig(aSection, KEnd(), endPosition);
	if (!hasStartPos)
		{
		ERR_PRINTF2(_L("Can not call SetPlayWindow. Parameter %S is missing in INI file."), &KEnd());
		}

	if (hasStartPos && hasEndPos)
		{
		iConvertUtility->SetPlayWindow(startPosition, endPosition);
		}
	else
		{
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdClearPlayWindow()
	{
	iConvertUtility->ClearPlayWindow();
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetMaxWriteLength(const TDesC& aSection)
	{
	TInt	length = 0;
	GetIntFromConfig(aSection, KMaxLength(), length);
	iConvertUtility->SetMaxWriteLength(length);
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdCropFromBeginningL()
	{
	TRAPD(err, iConvertUtility->CropFromBeginningL());
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::CropFromBeginningL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdControllerImplInfoL()
	{
	const CMMFControllerImplementationInformation*	controllerInfo = NULL;
	TRAPD(err, controllerInfo = &(iConvertUtility->ControllerImplementationInformationL()));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::ControllerImplementationInformationL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		CT_MMUtil::LogL(*this, *controllerInfo);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdCustomCommandSyncL(const TDesC& aSection)
	{

	TMMFMessageDestination		handleInfo(iAudioControllerUid);
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
		err = iConvertUtility->CustomCommandSync(messageDest, function, *dataTo1, *dataTo2, dataFromText);
		}
	else
		{
		err = iConvertUtility->CustomCommandSync(messageDest, function, *dataTo1, *dataTo2);
		}
	if (err!=KErrNone)
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::CustomCommandSync failed with error %d."), err);
		SetError(err);
		}
	else
		{
		if (useDataFrom)
			{
			HBufC*	buffer=HBufC::NewLC(dataFromText.Length());
			TPtr	bufferPtr=buffer->Des();
			bufferPtr.Copy(dataFromText); // Create a 16 bit copy of the 8 bit original
			INFO_PRINTF2(_L("DataFrom: %S"), &bufferPtr);
			CleanupStack::PopAndDestroy(buffer);
			}
		}

	CleanupStack::PopAndDestroy(2, dataTo1);
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdCustomCommandAsyncL(const TDesC& aSection)
	{

	TMMFMessageDestination		handleInfo(iAudioControllerUid);
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
		iConvertUtility->CustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iDataFromText, iActiveCustomCommandAsync->iStatus);
		}
	else
		{
		iConvertUtility->CustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iActiveCustomCommandAsync->iStatus);
		}

	iActiveCustomCommandAsync->Activate(0);
	IncOutstanding();
	CleanupStack::PopAndDestroy(2, dataTo1);
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetRepeats(const TDesC& aSection)
	{

	TInt	repeatNumberOfTimes = 0;
	TBool hasRepNrOfTimes = GetIntFromConfig(aSection, KRepeatNumber(), repeatNumberOfTimes);
	if (!hasRepNrOfTimes)
		{
		ERR_PRINTF2(_L("Can not call SetRepeats. Parameter %S is missing in INI file."), &KRepeatNumber());
		}

	TInt	interval = 0;
	TBool hasInterval = GetIntFromConfig(aSection, KInterval(), interval);
	if (!hasInterval)
		{
		ERR_PRINTF2(_L("Can not call SetRepeats. Parameter %S is missing in INI file."), &KInterval());
		}

	if (hasRepNrOfTimes && hasInterval)
		{
		iConvertUtility->SetRepeats(repeatNumberOfTimes, interval);
		}
	else
		{
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdSetThreadPriority(const TDesC& aSection)
	{
	TThreadPriority	threadPriority;
	if ( !CT_MMUtil::ReadThreadPriority(*this, aSection, threadPriority) )
		{
		threadPriority=EPriorityNormal;
		}

	TInt	err = iConvertUtility->SetThreadPriority(threadPriority);
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::SetThreadPriority failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdUseSharedHeap()
	{
// Only if UseSharedHeap() is supported
#if (defined CMDA_AUDIO_CONVERT_UTILITY_USED_SHARED_HEAP)
	iConvertUtility->UseSharedHeap();
#else
	INFO_PRINTF1(_L("CMdaAudioConvertUtility::UseSharedHeap is not supported in this build."));
#endif
	}

void CT_MMDataMdaAudioConvertUtility::DoCmdDestructor()
	{
	DestroyData();
	}

///////////////////////////////////////////////////////////////////
//from the MMdaObjectStateChangeObserver interface 				//
///////////////////////////////////////////////////////////////////

/**
Callback to receive state change events from converter.
@internalComponent
@return void
@param aObject, aPreviousState, aCurrentState, aErrorCode
This can be completion of a call to CMdaAudioConvertUtility::OpenL(),
CMdaAudioConvertUtility::ConvertL(), CMdaAudioConvertUtility::PlayL(),
CMdaAudioConvertUtility::RecordL().
The event contains a aObject identifying the event and an accompanying error code.
@pre N/A
@post N/A
*/
void CT_MMDataMdaAudioConvertUtility::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	// Handle converter event
	if (aObject == iConvertUtility)
		{
		INFO_PRINTF3(_L("Current state: %S, previous state: %S"), &GetState(aCurrentState), &GetState(aPreviousState));
		if ( aErrorCode!=KErrNone )
			{
			ERR_PRINTF2(_L("MoscoStateChangeEvent Error %d."), aErrorCode);
			SetAsyncError(iAsyncErrorIndex,aErrorCode);
			while ( iOutstandingRepeats>0 )
				{
				--iOutstandingRepeats;
				DecOutstanding();
				}
			}
		}
	if ( iOutstandingRepeats>0 )
		{
		--iOutstandingRepeats;
		}
	DecOutstanding();
	}


/**
Get the audio format by string.
@internalComponent
@return N/A
@param aFormatStr, &aMdaClipFormat
@pre N/A
@post N/A
*/
TBool CT_MMDataMdaAudioConvertUtility::GetFormatL(const TDesC& aFormatStr, TMdaClipFormat* &aMdaClipFormat, const TDesC& aSection)
	{
	TBool	ret = ETrue;

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
		GetHexFromConfig(aSection, KFormatUid(), invalidUid);

		aMdaClipFormat = new(ELeave) TMdaClipFormat();
		TUid	uid = TUid::Uid(invalidUid);
		aMdaClipFormat->SetUid(uid);
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}

/**
Get Codec by String
@internalComponent
@return N/A
@param aFormatStr, &aMdaPackage
@pre N/A
@post N/A
*/
TBool CT_MMDataMdaAudioConvertUtility::GetCodecL(const TDesC& aCodecStr, TMdaPackage* &aMdaPackage)
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


/**
Get state from Id
@internalComponent
@return N/A
@param aUid
@pre N/A
@post N/A
*/
const TDesC& CT_MMDataMdaAudioConvertUtility::GetState(const TInt& aState)
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

/**
 Virtual RunL - Called on completion of an asynchronous command
 @internalComponent
 @see MT_MMActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_MMDataMdaAudioConvertUtility::RunL(CActive* aActive,const TInt aAsyncErrorIndex)
 	{
 	if(aAsyncErrorIndex>0) iAsyncErrorIndex = aAsyncErrorIndex;

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
 void CT_MMDataMdaAudioConvertUtility::DoCancel(CActive* aActive)
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
 void CT_MMDataMdaAudioConvertUtility::RunCustomCommandAsyncL(const TInt /*aAsyncErrorIndex*/)
	{
	HBufC*	buffer=HBufC::NewLC(iDataFromText.Length());
	TPtr	bufferPtr=buffer->Des();
	bufferPtr.Copy(iDataFromText); // Create a 16 bit copy of the 8 bit original
	INFO_PRINTF2(_L("DataFrom: %S"), &bufferPtr);

	TInt	err = iActiveCustomCommandAsync->iStatus.Int();
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError(iAsyncErrorIndex, err);
		}

	DecOutstanding();
	//DecAsyncCount();
	//StartCommands();
	CleanupStack::PopAndDestroy(buffer);
	}

/**
 DoCancelActive - Process DoCancel for iActive
 @internalComponent
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_MMDataMdaAudioConvertUtility::DoCancelCustomCommandAsync()
	{
	}

void CT_MMDataMdaAudioConvertUtility::GetAudioSettingsL(const TDesC& aAudioSettingsStr, CDataWrapperBase& aTestStep, const TDesC& aSection, TMdaPackage* &aMdaPackage)
	{
	TPtrC								audioSetingsStr;
	if (GetStringFromConfig(aSection, aAudioSettingsStr, audioSetingsStr))
		{
		if (GetStringFromConfig(aSection, KInvalidAudioSetings(), audioSetingsStr))
			{
			// Invalid audiosettings package
			aMdaPackage = new(ELeave) TMdaPcmWavCodec();
			}
		else
			{
			aMdaPackage = new(ELeave) TMdaAudioDataSettings();
			CT_MMUtil::ReadAudioDataSettings(aTestStep, *((TMdaAudioDataSettings*)aMdaPackage), aSection);
			}
		}
	}
