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

/**
@test
@internalComponent

This contains CT_MdaDevSoundData
*/

//	User includes
#include "t_mdadevsounddata.h"
#include "t_sounddriverserver.h"

/*@{*/
///	Parameters

_LIT(KSoundFormatsSupportedObject,	"SoundFormatsSupportedObject");
_LIT(KCurrentSoundFormatObject,		"CurrentSoundFormatObject");

_LIT(KUUnit,						"unit");
_LIT(KUUnitEnum,					"UnitEnum");
_LIT(KUToFile,						"to_file");
_LIT(KUBlockSize,					"block_size");
_LIT(KUVersion,						"version");

_LIT(KUFromFile,					"from_file");
_LIT(KUDataSize,					"data_size");
_LIT(KUVolumeExpected,				"volume_expected");
_LIT(KUVolume,						"volume");
_LIT(KUBytesPlayed,					"bytes_played");
_LIT(KUCancel,						"cancel");
_LIT(KUFlush,						"flush");

_LIT(KUSoundLevel,					"sound_level");
_LIT(KUSoundLevelExpected,			"sound_level_expected");

_LIT(KUFileToDelete,				"file_to_delete");

_LIT(KLDDName,						"ldd");
_LIT(KPDDName,						"pdd");



/*@{*/
///	Commands
_LIT(KCmdUtilityDeleteFile,			"utilityDeleteFile");

_LIT(KCmdRecordData,				"RecordData");
_LIT(KCmdCancelRecordData,			"CancelRecordData");
_LIT(KCmdFlushRecordBuffer,			"FlushRecordBuffer");
_LIT(KCmdRecordLevel,				"RecordLevel");
_LIT(KCmdSetRecordLevel,			"SetRecordLevel");
_LIT(KCmdNotifyRecordError,			"NotifyRecordError");
_LIT(KCmdCancelNotifyRecordError,	"CancelNotifyRecordError");

_LIT(KCmdPlayVolume,				"PlayVolume");
_LIT(KCmdSetPlayVolume,				"SetPlayVolume");
_LIT(KCmdPlayData,					"PlayData");
_LIT(KCmdCancelPlayData,			"CancelPlayData");
_LIT(KCmdFlushPlayBuffer,			"FlushPlayBuffer");
_LIT(KCmdNotifyPlayError,			"NotifyPlayError");
_LIT(KCmdCancelNotifyPlayError,		"CancelNotifyPlayError");

_LIT(KCmdBytesPlayed,				"BytesPlayed");
_LIT(KCmdResetBytesPlayed,			"ResetBytesPlayed");
_LIT(KCmdPausePlayBuffer,			"PausePlayBuffer");
_LIT(KCmdResumePlaying,				"ResumePlaying");

_LIT(KCmdPlayFormatsSupported,		"PlayFormatsSupported");
_LIT(KCmdGetPlayFormat,				"GetPlayFormat");
_LIT(KCmdSetPlayFormat,				"SetPlayFormat");
_LIT(KCmdRecordFormatsSupported,	"RecordFormatsSupported");
_LIT(KCmdGetRecordFormat,			"GetRecordFormat");
_LIT(KCmdSetRecordFormat,			"SetRecordFormat");

_LIT(KCmdNew,						"new");
_LIT(KCmdOpen,						"Open");
_LIT(KCmdClose,						"Close");
_LIT(KCmdDestructor,				"~");
_LIT(KCmdIsMdaSound,				"IsMdaSound");
_LIT(KCmdVersionRequired,			"VersionRequired");
/*@}*/

/**
 * Two phase constructor
 */
CT_MdaDevSoundData* CT_MdaDevSoundData::NewL(RMdaDevSound* aServerStore)
	{
	CT_MdaDevSoundData*	ret = new (ELeave) CT_MdaDevSoundData(aServerStore);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;	
	}

/**
 * Protected constructor. First phase construction
 */
CT_MdaDevSoundData::CT_MdaDevSoundData(RMdaDevSound* aServerStore)
	:iServerStore(aServerStore)
	{
	}

/**
* Protected constructor. Second phase construction
*/
void CT_MdaDevSoundData::ConstructL()
	{
	}

/**
 * Destructor.
 */
CT_MdaDevSoundData::~CT_MdaDevSoundData()
	{	
	DoCleanup();

	iNotifyPlayError.ResetAndDestroy();
	iNotifyRecordError.ResetAndDestroy();
	iPlay.ResetAndDestroy();
	iRecord.ResetAndDestroy();
	}

/**
 * Contains cleanup implementation
 */
void CT_MdaDevSoundData::DoCleanup()
	{
	if(iMdaDevSound)
		{
		INFO_PRINTF1(_L("Deleting current RMdaDevSound"));
		delete iMdaDevSound;
		iMdaDevSound = NULL;
		}
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return pointer to the object that the data wraps
 */
TAny* CT_MdaDevSoundData::GetObject()
	{
	return iMdaDevSound;
	}

/**
 * Set the wrapped data object with new value
 */
void CT_MdaDevSoundData::SetObjectL( TAny* aAny )
	{
	DoCleanup();
	iMdaDevSound = static_cast<RMdaDevSound*> ( aAny );
	*iServerStore = *iMdaDevSound; 
	}

/**
 * Clear the wrapped data object pointer w/o de-initialization
 */
void CT_MdaDevSoundData::DisownObjectL()
	{
	iMdaDevSound = NULL;
	iServerStore->SetHandle(KNullHandle);
	}

/**
 * Return static cleanup function
 */
inline TCleanupOperation CT_MdaDevSoundData::CleanupOperation()
	{
	return CleanupOperation;
	}

/**
 * Static cleanup function
 */
void CT_MdaDevSoundData::CleanupOperation( TAny* aAny )
	{
	RMdaDevSound* sound = static_cast<RMdaDevSound*> ( aAny );
	delete sound;
	}

/**
 * Process a command read from the ini file
 *
 * @param aCommand	the command to process
 * @param aSection		the entry in the ini file requiring the command to be processed
 *
 * @return ETrue if the command is processed
 */
TBool CT_MdaDevSoundData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;
	if (aCommand == KCmdNew)
		{
		DoCmdNewL();
		}
	else if (aCommand == KCmdClose)
		{
		DoCmdClose();
		}
	else if (aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else if (aCommand == KCmdOpen)
		{
		DoCmdOpen(aSection);
		}
	else if (aCommand == KCmdIsMdaSound)
		{
		DoCmdIsMdaSound();
		}
	else if (aCommand == KCmdVersionRequired)
		{
		DoCmdVersionRequired(aSection);
		}
	else if (aCommand == KCmdPlayFormatsSupported)
		{
		DoCmdPlayFormatsSupportedL(aSection);
		}
	else if (aCommand == KCmdGetPlayFormat)
		{
		DoCmdGetPlayFormatL(aSection);
		}
	else if (aCommand == KCmdSetPlayFormat)
		{
		DoCmdSetPlayFormatL(aSection);
		}
	else if (aCommand == KCmdRecordFormatsSupported)
		{
		DoCmdRecordFormatsSupportedL(aSection);
		}
	else if (aCommand == KCmdGetRecordFormat)
		{
		DoCmdGetRecordFormatL(aSection);
		}
	else if (aCommand == KCmdSetRecordFormat)
		{
		DoCmdSetRecordFormatL(aSection);
		}
	else if (aCommand == KCmdPlayVolume)
		{
		DoCmdPlayVolume(aSection);
		}
	else if (aCommand == KCmdSetPlayVolume)
		{
		DoCmdSetPlayVolume(aSection);
		}
	else if (aCommand == KCmdPlayData)
		{
		DoCmdPlayDataL(aSection, aAsyncErrorIndex);
		}
	else if (aCommand == KCmdCancelPlayData)
		{
		DoCmdCancelPlayData();
		}
	else if (aCommand == KCmdFlushPlayBuffer)
		{
		DoCmdFlushPlayBuffer();
		}
	else if (aCommand == KCmdFlushPlayBuffer)
		{
		DoCmdNotifyPlayErrorL(aAsyncErrorIndex);
		}
	else if (aCommand == KCmdFlushPlayBuffer)
		{
		DoCmdCancelNotifyPlayError();
		}
	else if (aCommand == KCmdNotifyPlayError)
		{
		DoCmdNotifyPlayErrorL(aAsyncErrorIndex);
		}
	else if (aCommand == KCmdCancelNotifyPlayError)
		{
		DoCmdCancelNotifyPlayError();
		}
	else if (aCommand == KCmdBytesPlayed)
		{
		DoCmdBytesPlayed(aSection);
		}
	else if (aCommand == KCmdResetBytesPlayed)
		{
		DoCmdResetBytesPlayed();
		}
	else if (aCommand == KCmdPausePlayBuffer)
		{
		DoCmdPausePlayBuffer();
		}
	else if (aCommand == KCmdResumePlaying)
		{
		DoCmdResumePlaying();
		}
	else if (aCommand == KCmdRecordData)
		{
		DoCmdRecordDataL(aSection, aAsyncErrorIndex);
		}
	else if (aCommand == KCmdCancelRecordData)
		{
		DoCmdCancelRecordData();
		}
	else if (aCommand == KCmdFlushRecordBuffer)
		{
		DoCmdFlushRecordBuffer();
		}
	else if (aCommand == KCmdRecordLevel)
		{
		DoCmdRecordLevel(aSection);
		}
	else if (aCommand == KCmdSetRecordLevel)
		{
		DoCmdSetRecordLevel(aSection);
		}
	else if (aCommand == KCmdNotifyRecordError)
		{
		DoCmdNotifyRecordErrorL(aAsyncErrorIndex);
		}
	else if (aCommand == KCmdCancelNotifyRecordError)
		{
		DoCmdCancelNotifyRecordError();
		}
	else if (aCommand == KCmdUtilityDeleteFile)
		{
		DoCmdUtilityDeleteFileL(aSection);
		}
	else
		{
		ret=CDataWrapperBase::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}

/** 
 * Creates new RMdaDevSound class instance 
 */
void CT_MdaDevSoundData::DoCmdNewL()
	{
	INFO_PRINTF1(_L("Create new RMdaDevSound class instance"));
	
	//Deletes previous RMdaDevsound class instance if it was already created.
	DoCleanup();

	// do create
	TRAPD(err, iMdaDevSound = new (ELeave) RMdaDevSound());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("new error %d"), err);
		SetError( err );
		}
	}

/** 
 * Close RMdaDevSound handle 
 */
void CT_MdaDevSoundData::DoCmdClose()
	{
	INFO_PRINTF1(_L("Closing RMdaDevSound"));
	iServerStore->SetHandle(KNullHandle);
	iMdaDevSound->Close();
	}

/** 
 * Destroy RMdaDevSound the object 
 */
void CT_MdaDevSoundData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("Destroying the RMdaDevSound object"));
	//close RMdaDevSound first
	iServerStore->SetHandle(KNullHandle);
	iMdaDevSound->Close();
	DoCleanup();
	}

/** 
 * Connects a client to the sound driver server 
 * 
 * @param aSection 			section name which contains unit
 * 							KUUnit - unit to open
 *
 * @return					void
 *
 * @leave					no
 */
void CT_MdaDevSoundData::DoCmdOpen(const TDesC& aSection)
	{
	//Loading device drivers
	TPtrC lddName;
	TPtrC pddName;
	if(GET_OPTIONAL_STRING_PARAMETER(KLDDName(), aSection, lddName) &&
		GET_OPTIONAL_STRING_PARAMETER(KPDDName(), aSection, pddName))
		{
		TInt err = TSoundUtil::LoadSoundDrivers(lddName, pddName);	
		if( (err != KErrNone) && (err != KErrAlreadyExists ))
			{
			ERR_PRINTF4(_L("Open() -> LoadSoundDrivers(%S, %S) error %d"), &lddName, &pddName, err);
			SetError( err );		
			}
		}
	//Finish Loading device drivers
			
	TInt	unit = KNullUnit;
	TPtrC	unitName;
	if (GET_OPTIONAL_STRING_PARAMETER(KUUnit(), aSection, unitName))
		{
		GetHexFromConfig(KUUnitEnum(), unitName, unit );
		}
	TInt err = iMdaDevSound->Open(unit);
	*iServerStore = *iMdaDevSound;
	INFO_PRINTF2(_L("Call Open(unit = %d)"), unit);
	
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("Open() error %d"), err);
		SetError( err );
		}		
	}	
	
	
	

/** 
 * Checks if driver is MdaSound
 */
void CT_MdaDevSoundData::DoCmdIsMdaSound()
	{
	INFO_PRINTF1(_L("CT_MdaDevSoundData::DoCmdIsMdaSound Called"));
	
	TInt err = iMdaDevSound->IsMdaSound();	
	if(err!=KErrNone)
		{
		ERR_PRINTF2(_L("IsMdaSound() error %d"), err);
		SetError(err);
		}
	
	}

/** 
 * Checks driver version required
 * 
 * @param aSection 			section name with expected version
 *							KUVersion - version expected
 * 
 * @return					void
 *
 * @leave					no
 */
void CT_MdaDevSoundData::DoCmdVersionRequired(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CT_MdaDevSoundData::VersionRequired Called"));
	
	TVersion version = iMdaDevSound->VersionRequired();
	TPtrC versionNameFromSystem = version.Name();
	TPtrC versionName;
	if(GET_MANDATORY_STRING_PARAMETER(KUVersion(), aSection, versionName))
		{
		if(versionNameFromSystem != versionName)
			{
			ERR_PRINTF1(_L("iMdaDevSound->VersionRequired() != expected"));
			ERR_PRINTF3(_L("version.Name()=%S, expected=%S"), &versionNameFromSystem, &versionName);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("iMdaDevSound->VersionRequired() == expected"));
			}
		}
	
	}

/** 
 * Gets PlayFormatsSupported
 * 
 * @param aSection 			Section name which contains SoundFormatsSupported 
 * 							object name
 *  						KSoundFormatsSupportedObject - SoundFormatsSupported 
 * 							object name
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MdaDevSoundData::DoCmdPlayFormatsSupportedL(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("Getting PlayFormatsSupported!"));
	
	TPtrC		objectName;	
	if (GET_MANDATORY_STRING_PARAMETER(KSoundFormatsSupportedObject, aSection, objectName))
		{		
		RMdaDevSound::TSoundFormatsSupported* soundFormat = static_cast<RMdaDevSound::TSoundFormatsSupported*>(GetDataObjectL(objectName));		
		RMdaDevSound::TSoundFormatsSupportedBuf soundFormatsSupportedBuf(*soundFormat);		
		iMdaDevSound->PlayFormatsSupported(soundFormatsSupportedBuf);		
		*soundFormat = soundFormatsSupportedBuf();		
		}		
	}

/** 
 * Gets current PlayFormat
 * 
 * @param aSection 			Section name which contains CurrentSoundFormat 
 * 							object name
 *  						KCurrentSoundFormatObject - CurrentSoundFormat 
 * 							object name
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MdaDevSoundData::DoCmdGetPlayFormatL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Getting PlayFormat!"));
	
	TPtrC		objectName;	
	if (GET_MANDATORY_STRING_PARAMETER(KCurrentSoundFormatObject, aSection, objectName))
		{		
		RMdaDevSound::TCurrentSoundFormat* soundFormat = static_cast<RMdaDevSound::TCurrentSoundFormat*>(GetDataObjectL(objectName));		
		RMdaDevSound::TCurrentSoundFormatBuf currentSoundFormatBuf(*soundFormat);		
		iMdaDevSound->GetPlayFormat(currentSoundFormatBuf);		
		*soundFormat = currentSoundFormatBuf();
		}		
	}

/** 
 * Sets current PlayFormat
 * 
 * @param aSection 			Section name which contains CurrentSoundFormat 
 * 							object name
 * 							KCurrentSoundFormatObject - CurrentSoundFormat 
 * 							object name
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MdaDevSoundData::DoCmdSetPlayFormatL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Setting PlayFormat!"));
	
	TPtrC		objectName;	
	if (GET_MANDATORY_STRING_PARAMETER(KCurrentSoundFormatObject, aSection, objectName))
		{		
		RMdaDevSound::TCurrentSoundFormat* soundFormat = static_cast<RMdaDevSound::TCurrentSoundFormat*>(GetDataObjectL(objectName));		
		RMdaDevSound::TCurrentSoundFormatBuf currentSoundFormatBuf(*soundFormat);		
		TInt err = iMdaDevSound->SetPlayFormat(currentSoundFormatBuf);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("error in SetPlayFormatL %d"), err);
			SetError( err );
			}
		else
			{
			*soundFormat = currentSoundFormatBuf();
			}
		}
	}

/** 
 * Gets RecordFormatsSupported
 * 
 * @param aSection 			Section name which contains SoundFormatsSupported 
 * 							object name
 * 							KSoundFormatsSupportedObject - SoundFormatsSupported 
 * 							object name
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MdaDevSoundData::DoCmdRecordFormatsSupportedL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Getting RecordFormatsSupported!"));

	TPtrC		objectName;
	if (GET_MANDATORY_STRING_PARAMETER(KSoundFormatsSupportedObject, aSection, objectName))
		{		
		RMdaDevSound::TSoundFormatsSupported* soundFormat = static_cast<RMdaDevSound::TSoundFormatsSupported*>(GetDataObjectL(objectName));		
		RMdaDevSound::TSoundFormatsSupportedBuf soundFormatsSupportedBuf(*soundFormat);		
		iMdaDevSound->RecordFormatsSupported(soundFormatsSupportedBuf);		
		*soundFormat = soundFormatsSupportedBuf();		
		}
	}

/** 
 * Gets current RecordFormat
 * 
 * @param aSection 			Section name which contains CurrentSoundFormat 
 * 							object name
 *  						KCurrentSoundFormatObject - CurrentSoundFormat 
 * 							object name
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MdaDevSoundData::DoCmdGetRecordFormatL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Getting RecordFormat!"));
	
	TPtrC		objectName;	
	if (GET_MANDATORY_STRING_PARAMETER(KCurrentSoundFormatObject, aSection, objectName))
		{		
		RMdaDevSound::TCurrentSoundFormat* soundFormat = static_cast<RMdaDevSound::TCurrentSoundFormat*>(GetDataObjectL(objectName));		
		RMdaDevSound::TCurrentSoundFormatBuf currentSoundFormatBuf(*soundFormat);		
		iMdaDevSound->GetRecordFormat(currentSoundFormatBuf);		
		*soundFormat = currentSoundFormatBuf();
		}
	}

/** 
 * Sets current RecordFormat
 * 
 * @param aSection 			Section name which contains CurrentSoundFormat 
 * 							object name
 * 							KCurrentSoundFormatObject - CurrentSoundFormat 
 * 							object name
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MdaDevSoundData::DoCmdSetRecordFormatL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Setting RecordFormat!"));
	
	TPtrC		objectName;	
	if (GET_MANDATORY_STRING_PARAMETER(KCurrentSoundFormatObject, aSection, objectName))
		{		
		RMdaDevSound::TCurrentSoundFormat* soundFormat = static_cast<RMdaDevSound::TCurrentSoundFormat*>(GetDataObjectL(objectName));		
		RMdaDevSound::TCurrentSoundFormatBuf currentSoundFormatBuf(*soundFormat);		
		TInt err = iMdaDevSound->SetRecordFormat(currentSoundFormatBuf);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("error in SetRecordFormatL %d"), err);
			SetError( err );
			}
		else
			{
			*soundFormat = currentSoundFormatBuf();
			}
		}
	}

/** 
 * Gets current PlayVolume
 * 
 * @param aSection 			Section name which contains expected value
 * 							KUVolumeExpected - expected volume level
 *
 * @return					void
 *
 * @leave					no
 */
void	CT_MdaDevSoundData::DoCmdPlayVolume(const TDesC& aSection)
	{
	TInt volume = 0;
	INFO_PRINTF2(_L("calling PlayVolume(), value before execution = %d"), volume);
	volume = iMdaDevSound->PlayVolume();
	
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KUVolumeExpected(), aSection, expected))
		{
		if(volume != expected)
			{
			ERR_PRINTF3(_L("Expected result != volume, expected = %d, volume = %d"),
							expected, volume);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("volume = %d"), volume);
			}
		}
	else
		{
		INFO_PRINTF2(_L("volume = %d"), volume);
		}
	}

/** 
 * Sets current PlayVolume
 * 
 * @param aSection 			Section name which contains value to set
 * 							KUVolume - volume to set
 *
 * @return					void
 *
 * @leave					no
 */
void	CT_MdaDevSoundData::DoCmdSetPlayVolume(const TDesC& aSection)
	{
	TInt volume = 0;
	INFO_PRINTF2(_L("calling SetPlayVolume(), value before execution = %d"), volume);
	if (GET_OPTIONAL_INT_PARAMETER(KUVolume(), aSection, volume))
		{
		iMdaDevSound->SetPlayVolume(volume);
		INFO_PRINTF2(_L("after execution value = %d"), volume);
		}
	else
		{
		INFO_PRINTF2(_L("volume after execution = %d"), volume);
		}
	}

/** 
 * Play audio from file. DoCmdPlayDataL is asynchronous function
 * 
 * @param aSection 			Section name which can contain values to set
 * 							KUDataSize - data size, 
 * 							KUFromFile - file name to play, 
 * 							KUBlockSize - block size
 * 							
 *
 * @return					void
 *
 * @leave					System wide error
 */
void	CT_MdaDevSoundData::DoCmdPlayDataL(const TDesC& aSection, 
											const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("calling PlayData()"));
	
	CT_SoundActiveCallback* active = CT_SoundActiveCallback::NewL(*this);
	iPlay.Append(active);
		
	active->iSoundData = new(ELeave) TSoundUtil::RAudioBuffer();
	TSoundUtil::RAudioBuffer* soundData = active->iSoundData;

	TInt dataSize = TSoundUtil::KAudioBufferMaxSize;
	GET_OPTIONAL_INT_PARAMETER(KUDataSize, aSection, dataSize);
	soundData->Close();
	soundData->CreateL(dataSize);

	TPtrC	fileName;
	TInt err = KErrNone;
	if (GET_OPTIONAL_STRING_PARAMETER(KUFromFile, aSection, fileName))
		{
		err = soundData->LoadAudioDataL(fileName);
		if (err)
			{
			SetBlockResult(EFail);
			ERR_PRINTF2(_L("Audio file loading failed with error code %d"), err);
			}
		}

	if (!err)
		{
		active->SetSection(aSection);
		IncOutstanding();

		TInt blockSize = TSoundUtil::KDefaultBlockSize;
		GET_OPTIONAL_INT_PARAMETER(KUBlockSize, aSection, blockSize);
		blockSize = Min(blockSize, soundData->Length());
		active->iBufferBlock.Set(&soundData->operator[](0), blockSize, blockSize);
		iMdaDevSound->PlayData(active->iStatus, active->iBufferBlock);
		active->Activate(aAsyncErrorIndex);
		TBool stopPlay = EFalse;
		if (GET_OPTIONAL_BOOL_PARAMETER(KUCancel, aSection, stopPlay))
			{
			if ( stopPlay )
				{
				iMdaDevSound->CancelPlayData();
				INFO_PRINTF1(_L("CancelPlayData() executed"));
				}
			}
		if (GET_OPTIONAL_BOOL_PARAMETER(KUFlush, aSection, stopPlay))
			{
			if ( stopPlay )
				{
				iMdaDevSound->FlushPlayBuffer();
				INFO_PRINTF1(_L("FlushPlayBuffer() executed"));
				}
			}
		}
	}

/** 
 * Cancel playing
 */
void	CT_MdaDevSoundData::DoCmdCancelPlayData()
	{
	INFO_PRINTF1(_L("calling CancelPlayData()"));
	
	if (iPlay.Count() > 0)
		{
		INFO_PRINTF2(_L("CancelPlayData called on active object with iStatus == %d"),
				iPlay[iPlay.Count() - 1]->iStatus.Int());
		}
	else
		{
		INFO_PRINTF1(_L("No active objects found"));
		}
	
	iMdaDevSound->CancelPlayData();
	}

/** 
 * Flush play buffer
 */
void	CT_MdaDevSoundData::DoCmdFlushPlayBuffer()
	{
	INFO_PRINTF1(_L("calling FlushPlayBuffer()"));

	if (iPlay.Count() > 0)
		{
		INFO_PRINTF2(_L("FLushPlayBuffer called on active object with iStatus == %d"),
				iPlay[iPlay.Count() - 1]->iStatus.Int());
		}
	else
		{
		INFO_PRINTF1(_L("No active objects found"));
		}
	
	iMdaDevSound->FlushPlayBuffer();
	}

/** 
 * Asynchronous function that starts notifying play error
 */
void	CT_MdaDevSoundData::DoCmdNotifyPlayErrorL(const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("calling NotifyPlayError()"));
	
	CActiveCallback* active = CActiveCallback::NewL(*this);
	iNotifyPlayError.Append(active);
	iMdaDevSound->NotifyPlayError(active->iStatus);
		
	//acitvate callback object
	active->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}

/** 
 * Cancel notifying play error
 */
void	CT_MdaDevSoundData::DoCmdCancelNotifyPlayError()
	{
	INFO_PRINTF1(_L("calling CancelNotifyPlayError()"));
	
	iMdaDevSound->CancelNotifyPlayError();
	}

/** 
 * Gets bytes played
 * 
 * @param aSection 			Section name which can contain expected value
 * 							KUBytesPlayed - bytes played
 *
 * @return					void
 *
 * @leave					no
 */
void	CT_MdaDevSoundData::DoCmdBytesPlayed(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("calling BytesPlayed()"));
	
	TInt bytesPlayed = iMdaDevSound->BytesPlayed();
	INFO_PRINTF2(_L("Bytes played: %d"), bytesPlayed);

	TInt expected = 0;
	if (GET_OPTIONAL_INT_PARAMETER(KUBytesPlayed, aSection, expected))
		{
		if (expected != bytesPlayed)
			{
			ERR_PRINTF3(_L("Bytes played %d != %d expected"), bytesPlayed, expected);
			SetBlockResult(EFail);
			}
		}
	}

/** 
 * Resets bytes played
 */
void	CT_MdaDevSoundData::DoCmdResetBytesPlayed()
	{
	INFO_PRINTF1(_L("calling ResetBytesPlayed()"));
	
	iMdaDevSound->ResetBytesPlayed();
	}

/** 
 * Pause playing
 */
void	CT_MdaDevSoundData::DoCmdPausePlayBuffer()
	{
	INFO_PRINTF1(_L("calling PausePlayBuffer()"));
	
	iMdaDevSound->PausePlayBuffer();
	}

/** 
 * Resume playing
 */
void	CT_MdaDevSoundData::DoCmdResumePlaying()
	{
	INFO_PRINTF1(_L("calling ResumePlaying()"));
	
	iMdaDevSound->ResumePlaying();		
	}

/** 
 * DoCmdRecordDataL is asynchronous function to record audio.
 * 
 * @param aSection 			Section name which can contain value to set
 * 							KUDataSize - data size to record, 
 * 							KUToFile - file to record audio,
 * 							KUBlockSize - block size
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MdaDevSoundData::DoCmdRecordDataL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("calling RecordData()"));	
	
	CT_SoundActiveCallback* active = CT_SoundActiveCallback::NewL(*this);
	iRecord.Append(active);
	
	active->iSoundData = new(ELeave) TSoundUtil::RAudioBuffer();
	TSoundUtil::RAudioBuffer* soundData = active->iSoundData;

	TInt dataSize = TSoundUtil::KAudioBufferMaxSize;
	GET_OPTIONAL_INT_PARAMETER(KUDataSize, aSection, dataSize);
	soundData->Close();
	soundData->CreateL(dataSize);
	soundData->SetLength(dataSize);

	TInt blockSize = TSoundUtil::KDefaultBlockSize;
	GET_OPTIONAL_INT_PARAMETER(KUBlockSize, aSection, blockSize);
	active->iBufferBlock.Set(&soundData->operator[](0), 0, Min(blockSize, soundData->Length()));
	iMdaDevSound->RecordData(active->iStatus, active->iBufferBlock);

	active->SetSection(aSection);
	IncOutstanding();
	active->Activate(aAsyncErrorIndex);
	TBool stopRecord = EFalse;
	if (GET_OPTIONAL_BOOL_PARAMETER(KUCancel, aSection, stopRecord))
		{
		if ( stopRecord )
			{
			iMdaDevSound->CancelPlayData();
			INFO_PRINTF1(_L("CancelRecordData() executed"));
			}
		}
	if (GET_OPTIONAL_BOOL_PARAMETER(KUFlush, aSection, stopRecord))
		{
		if ( stopRecord )
			{
			iMdaDevSound->FlushRecordBuffer();
			INFO_PRINTF1(_L("FlushRecordBuffer() executed"));
			}
		}
	}

/** 
 * Cancels recording in progress
 */
void CT_MdaDevSoundData::DoCmdCancelRecordData()
	{
	INFO_PRINTF1(_L("calling CancelRecordData()"));
	
	if (iRecord.Count() > 0)
		{
		INFO_PRINTF2(_L("CancelRecordData called on active object with iStatus == %d"),
				iRecord[iRecord.Count() - 1]->iStatus.Int());
		}
	else
		{
		INFO_PRINTF1(_L("No active objects found"));
		}
	
	iMdaDevSound->CancelRecordData();
	}

/** 
 * Flushes record buffer
 */
void CT_MdaDevSoundData::DoCmdFlushRecordBuffer()
	{
	INFO_PRINTF1(_L("calling FlushRecordBuffer()"));
	
	if (iRecord.Count() > 0)
		{
		INFO_PRINTF2(_L("FlushRecordBuffer called on active object with iStatus == %d"),
				iRecord[iRecord.Count() - 1]->iStatus.Int());
		}
	else
		{
		INFO_PRINTF1(_L("No active objects found"));
		}
	
	iMdaDevSound->FlushRecordBuffer();
	}

/** 
 * Gets record volume level
 * 
 * @param aSection 			Section name which can contain expected value
 * 							KUSoundLevelExpected - expected sound level
 *
 * @return					void
 *
 * @leave					no
 */
void CT_MdaDevSoundData::DoCmdRecordLevel(const TDesC& aSection)
	{	
	TInt level = 0;
	INFO_PRINTF1(_L("calling RecordLevel()"));
	level = iMdaDevSound->RecordLevel();
	
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KUSoundLevelExpected(), aSection, expected))
		{
		if(level != expected)
			{
			ERR_PRINTF3(_L("Expected result != level, expected = %d, level = %d"),
							expected, level);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("level = %d"), level);
			}
		}
	else
		{
		INFO_PRINTF2(_L("level = %d"), level);
		}
	}

/** 
 * Sets record volume level
 * 
 * @param aSection 			Section name which can contain value to set
 * 							KUSoundLevel - sound level to set
 *
 * @return					void
 *
 * @leave					no
 */
void CT_MdaDevSoundData::DoCmdSetRecordLevel(const TDesC& aSection)
	{
	TInt level = 0;
	INFO_PRINTF2(_L("calling SetRecordLevel(), level before execution = %d"), level);
	
	if(GET_OPTIONAL_INT_PARAMETER(KUSoundLevel, aSection, level))
		{
		iMdaDevSound->SetRecordLevel(level);
		INFO_PRINTF2(_L("after execution level = %d"), level);
		}	
	else
		{
		INFO_PRINTF2(_L("level after execution = %d"), level);
		}
	}

/** 
 * DoCmdNotifyRecordErrorL is asynchronous function that starts notifying recording
 * error
 */
void CT_MdaDevSoundData::DoCmdNotifyRecordErrorL(const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("calling NotifyRecordError()"));
	
	CActiveCallback* active = CActiveCallback::NewL(*this);
	iNotifyRecordError.Append(active);
	
	iMdaDevSound->NotifyRecordError(active->iStatus);
	
	//acitvate callback object
	active->Activate(aAsyncErrorIndex);
	IncOutstanding();	
	}

/** 
 * Cancels notifying recording error
 */
void CT_MdaDevSoundData::DoCmdCancelNotifyRecordError()
	{
	INFO_PRINTF1(_L("calling CancelNotifyRecordError()"));
	
	iMdaDevSound->CancelNotifyRecordError();
	}

/** 
 * Deletes file specified
 * 
 * @param aSection 			Section name which contains file name to delete
 * 							KUFileToDelete - File to delete 
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MdaDevSoundData::DoCmdUtilityDeleteFileL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("DoCmdUtilityDeleteFileL called!"));
	
	TPtrC	file;	
	if (GET_MANDATORY_STRING_PARAMETER(KUFileToDelete, aSection, file))
		{
		TInt err;
		err = TSoundUtil::DeleteFileL(file);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Deleting file failed with error code %d"), err);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("File deleted successfully"));
			}
		}
	}

/** 
 * Handle asynchronous events here
 */
void CT_MdaDevSoundData::RunL(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("CT_MdaDevSoundData::RunL Called"));
	TBool	foundActiveObject = EFalse;
	TInt	index=0;
	TInt	count=0;
	TBool	completed=ETrue;

	// See if it is in iNotifyPlayError
	count=iNotifyPlayError.Count();
	for( index=0; (index<count) && (!foundActiveObject); ++index )
 		{
 		if( aActive == iNotifyPlayError[index] )
			{
			INFO_PRINTF1(_L("RunL iNotifyPlayError called"));
			foundActiveObject = ETrue;
			iNotifyPlayError.Remove(index);
	 		}
		}

	// See if it is in iNotifyRecordError
	count=iNotifyRecordError.Count();
	for( index=0; (index<count) && (!foundActiveObject); ++index )
 		{
 		if( aActive == iNotifyRecordError[index] )
			{
			INFO_PRINTF1(_L("RunL iNotifyRecordError called"));
			foundActiveObject = ETrue;
			iNotifyRecordError.Remove(index);
	 		}
		}

	// See if it is in iPlay
	count=iPlay.Count();
	for( index=0; (index<count) && (!foundActiveObject); ++index )
 		{
 		if( aActive == iPlay[index] )
			{
			foundActiveObject = ETrue;
			INFO_PRINTF1(_L("RunL iPlay called"));
			
			TSoundUtil::RAudioBuffer* soundData = iPlay[index]->iSoundData;

			iPlay[index]->iDataPosition += iPlay[index]->iBufferBlock.Length();
			INFO_PRINTF3(_L("Played %d bytes, %d bytes played total"), iPlay[index]->iBufferBlock.Length(),
					iPlay[index]->iDataPosition);

			if ( ( iPlay[index]->iDataPosition >= soundData->Length() ) || aActive->iStatus.Int() )
				{
				iPlay.Remove(index);
				}
			else
				{
				TInt blockSize = TSoundUtil::KDefaultBlockSize;
				GET_OPTIONAL_INT_PARAMETER(KUBlockSize, *(iPlay[index]->iSection), blockSize);
				blockSize = Min(blockSize, soundData->Length() - iPlay[index]->iDataPosition);
				iPlay[index]->iBufferBlock.Set(&soundData->operator[](iPlay[index]->iDataPosition), blockSize, blockSize);
				iMdaDevSound->PlayData(iPlay[index]->iStatus, iPlay[index]->iBufferBlock);
				iPlay[index]->Activate(aIndex);
				completed = EFalse;
				}
	 		}
		}

	// See if it is in iRecord
	count=iRecord.Count();
	for( index=0; (index<count) && (!foundActiveObject); ++index )
 		{
 		if( aActive == iRecord[index] )
			{
			foundActiveObject = ETrue;	
			INFO_PRINTF1(_L("RunL iRecord called"));
			iRecord[index]->iDataPosition += iRecord[index]->iBufferBlock.Length();
			INFO_PRINTF3(_L("Stored %d bytes, %d bytes total"),
					iRecord[index]->iBufferBlock.Length(),
					iRecord[index]->iDataPosition);

			TSoundUtil::RAudioBuffer* buffer = iRecord[index]->iSoundData;
				
			TInt	err = aActive->iStatus.Int();
			if( err == KErrNone )
				{				
				if(iRecord[index]->iDataPosition >= buffer->Length() ||
						iRecord[index]->iBufferBlock.Length() == 0)
					{
					TPtrC fileName;
					if(GET_OPTIONAL_STRING_PARAMETER(KUToFile, *(iRecord[index]->iSection), fileName))
						{
						TInt err;
						err = buffer->SaveAudioDataL(fileName);
						if (err)
							{
							SetBlockResult(EFail);
							ERR_PRINTF2(_L("Saving to file failed with error code %d"), err);
							}
						}		
					iRecord.Remove(index);
					}				
				else
					{
					TInt blockSize = TSoundUtil::KDefaultBlockSize;
					//if specified in INI file get block size
					GET_OPTIONAL_INT_PARAMETER(KUBlockSize, *(iRecord[index]->iSection), blockSize);
					iRecord[index]->iBufferBlock.Set(&buffer->operator[](iRecord[index]->iDataPosition), 0, Min(blockSize, buffer->Length() - iRecord[index]->iDataPosition));
					iMdaDevSound->RecordData(iRecord[index]->iStatus, iRecord[index]->iBufferBlock);
					iRecord[index]->Activate(aIndex);
					completed = EFalse;
					}
				}//err == KErrNone
			else
				{
				iRecord.Remove(index);
				}					
			}//aActive == iRecord[index]
		}//for
	
 	if( foundActiveObject )
 		{
		if ( completed )
			{						
			if (aActive)
				{
				TInt	err = aActive->iStatus.Int();
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("RunL Error %d"), err);
					SetAsyncError( aIndex, err );
					}
				}

			// Reset the outstanding request state
			delete aActive;
			DecOutstanding();
			}
		}
 	else
 		{
 		ERR_PRINTF1(_L("An unchecked active object completed"));
 		SetBlockResult(EFail);
 		}
	}

/** 
 * Cancels asynchronous events
 */
void CT_MdaDevSoundData::DoCancel(CActive* aActive, TInt aIndex)
	{
	TBool	foundActiveObject = EFalse;

	TInt	index=0;
	TInt	count=0;

	// See if it is in iNotifyPlayError
	count=iNotifyPlayError.Count();
	for( index=0; (index<count) && (!foundActiveObject); ++index )
 		{
 		if( aActive == iNotifyPlayError[index] )
			{
			INFO_PRINTF1(_L("DoCancel iNotifyPlayError called"));
			foundActiveObject = ETrue;
			iNotifyPlayError.Remove(index);
	 		}
		}

	// See if it is in iNotifyRecordError
	count=iNotifyRecordError.Count();
	for( index=0; (index<count) && (!foundActiveObject); ++index )
 		{
 		if( aActive == iNotifyRecordError[index] )
			{
			INFO_PRINTF1(_L("DoCancel iNotifyRecordError called"));
			foundActiveObject = ETrue;
			iNotifyRecordError.Remove(index);
	 		}
		}

	// See if it is in iPlay
	count=iPlay.Count();
	for( index=0; (index<count) && (!foundActiveObject); ++index )
 		{
 		if( aActive == iPlay[index] )
			{
			INFO_PRINTF1(_L("DoCancel iPlay called"));
			foundActiveObject = ETrue;
			iPlay.Remove(index);
	 		}
		}

	// See if it is in iRecord
	count=iRecord.Count();
	for( index=0; (index<count) && (!foundActiveObject); ++index )
 		{
 		if( aActive == iRecord[index] )
			{
			INFO_PRINTF1(_L("DoCancel iRecord called"));
			foundActiveObject = ETrue;
			iRecord.Remove(index);
	 		}
		}
	
	if( foundActiveObject )
 		{
		if (aActive)
			{
			TInt	err = aActive->iStatus.Int();
			if( err != KErrNone ) 
				{
				ERR_PRINTF2(_L("DoCancel Error %d"), err);
				SetAsyncError( aIndex, err );
				}
			}

		// Reset the outstanding request state
		delete aActive;
		DecOutstanding();
		}
	else
		{
 		ERR_PRINTF1(_L("An unchecked active object canceled"));
 		SetBlockResult(EFail);
		}
	}
