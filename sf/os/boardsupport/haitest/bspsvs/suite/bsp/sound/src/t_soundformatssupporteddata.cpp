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

This contains CT_SoundFormatsSupportedData
*/

//	User includes
#include "t_soundformatssupporteddata.h"
#include "sound_util.h"

/*@{*/
///	Commands
_LIT(KCmdNew,						"new");
_LIT(KCmdDestructor,				"~");
_LIT(KCmd_iMinRate_get,				"imin_rate_get");
_LIT(KCmd_iMaxRate_get,				"imax_rate_get");
_LIT(KCmd_iChannels_get,			"ichannels_get");
_LIT(KCmd_iMinBufferSize_get,		"imin_buffer_size_get");
_LIT(KCmd_iMaxBufferSize_get,		"imax_buffer_size_get");
_LIT(KCmd_iMinVolume_get,			"imin_volume_get");
_LIT(KCmd_iMaxVolume_get,			"imax_volume_get");
_LIT(KCmd_iEncodings_get,			"iencodings_get");

/*@{*/
///	Parameters
_LIT(KMinRateExpected,				"min_rate_expected");
_LIT(KMaxRateExpected,				"max_rate_expected");
_LIT(KChannelsExpected,				"channels_expected");
_LIT(KMinBufferSizeExpected,		"min_buffer_size_expected");
_LIT(KMaxBufferSizeExpected,		"max_buffer_size_expected");
_LIT(KMinVolumeExpected,			"min_volume_expected");
_LIT(KMaxVolumeExpected,			"max_volume_expected");
_LIT(KEncodingsExpected,			"encodings_expected");

///	Logging
_LIT(KLogNew,						"New object created successfully");
_LIT(KLogNewError,					"Error creating new object, error code=%d");

/**
 * Two phase constructor
 */
CT_SoundFormatsSupportedData* CT_SoundFormatsSupportedData::NewL()
	{
	CT_SoundFormatsSupportedData*	ret=new (ELeave) CT_SoundFormatsSupportedData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
* Constructor. First phase construction
*/
CT_SoundFormatsSupportedData::CT_SoundFormatsSupportedData()
:	CDataWrapperBase()
,	iFormatsSupported(NULL)
	{
	}

/**
 * Second phase construction
 */
void CT_SoundFormatsSupportedData::ConstructL()
	{	
	}

/**
* Destructor
*/
CT_SoundFormatsSupportedData::~CT_SoundFormatsSupportedData()
	{
	DoCleanup();
	}

/**
 * Helper for cleanup operation
 */
void CT_SoundFormatsSupportedData::DoCleanup()
	{
	delete iFormatsSupported;
	iFormatsSupported=NULL;	
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return pointer to the object that the data wraps
 */
TAny* CT_SoundFormatsSupportedData::GetObject()
	{
	return iFormatsSupported;
	}

/**
 * Set the wrapped data object with new value
 */
void CT_SoundFormatsSupportedData::SetObjectL(TAny* aAny)
	{
	DoCleanup();
	iFormatsSupported=static_cast<RMdaDevSound::TSoundFormatsSupported*>(aAny);
	}

/**
 * Clear the wrapped data object pointer w/o de-initialization
 */
void CT_SoundFormatsSupportedData::DisownObjectL()
	{
	iFormatsSupported=NULL;
	}

/**
 * Return static cleanup function
 */
inline TCleanupOperation CT_SoundFormatsSupportedData::CleanupOperation()
	{
	return CleanupOperation;
	}

/**
 * Static cleanup function
 */
void CT_SoundFormatsSupportedData::CleanupOperation( TAny* aAny )
	{
	RMdaDevSound::TSoundFormatsSupported* format = static_cast<RMdaDevSound::TSoundFormatsSupported*> ( aAny );
	delete format;
	}

/**
* Process a command read from the ini file
*
* @param aDataWrapper	test step requiring command to be processed
* @param aCommand	the command to process
* @param aSection		the entry in the ini file requiring the command to be processed
*
* @return ETrue if the command is processed
*/
TBool CT_SoundFormatsSupportedData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;
	if ( aCommand==KCmdNew )
		{
		DoCmdNew();
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmd_iMinRate_get )
		{
		DoCmd_iMinRate_get(aSection);
		}
	else if ( aCommand==KCmd_iMaxRate_get )
		{
		DoCmd_iMaxRate_get(aSection);
		}
	else if ( aCommand==KCmd_iChannels_get )
		{
		DoCmd_iChannels_get(aSection);
		}
	else if ( aCommand==KCmd_iMinBufferSize_get )
		{
		DoCmd_iMinBufferSize_get(aSection);
		}
	else if ( aCommand==KCmd_iMaxBufferSize_get )
		{
		DoCmd_iMaxBufferSize_get(aSection);
		}
	else if ( aCommand==KCmd_iMinVolume_get )
		{
		DoCmd_iMinVolume_get(aSection);
		}
	else if ( aCommand==KCmd_iMaxVolume_get )
		{
		DoCmd_iMaxVolume_get(aSection);
		}
	else if ( aCommand==KCmd_iEncodings_get )
		{
		DoCmd_iEncodings_getL(aSection);
		}
	else
		{
		ret=CDataWrapperBase::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}

/**
 * Creates new instance of TSoundFormatsSupported
 */
void CT_SoundFormatsSupportedData::DoCmdNew()
	{
	DoCleanup();
	TRAPD(err, iFormatsSupported = new (ELeave) RMdaDevSound::TSoundFormatsSupported());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogNewError, err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(KLogNew);
		}
	}

/**
 * Performs cleanup operation
 */
void CT_SoundFormatsSupportedData::DoCmdDestructor()
	{
	DoCleanup();
	}

/**
 * Gets iMinRate
 * 
 * @param aSection 			section name where expected value can be read
 * 							KMinRateExpected - min rate expected
 *
 * @return					void
 *
 * @leave					no
 */
void CT_SoundFormatsSupportedData::DoCmd_iMinRate_get(const TDesC& aSection)
	{
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KMinRateExpected(), aSection, expected))
		{
		if(iFormatsSupported->iMinRate != expected)
			{
			ERR_PRINTF3(_L("Expected result != iMinRate, expected=%d, iMinRate=%d"), expected, iFormatsSupported->iMinRate);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("iMinRate == expected, iMinRate=%d, expected=%d"), iFormatsSupported->iMinRate, expected);
			}
		}
	else
		{
		INFO_PRINTF2(_L("iMinRate=%d"), iFormatsSupported->iMinRate);
		}
	}

/**
 * Gets iMaxRate
 * 
 * @param aSection 			section name where expected value can be read
 * 							KMaxRateExpected - max rate expected
 *
 * @return					void
 *
 * @leave					no
 */
void CT_SoundFormatsSupportedData::DoCmd_iMaxRate_get(const TDesC& aSection)
	{
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KMaxRateExpected(), aSection, expected))
		{
		if(iFormatsSupported->iMaxRate != expected)
			{
			ERR_PRINTF3(_L("Expected result != iMaxRate, expected=%d, iMaxRate=%d"), expected, iFormatsSupported->iMaxRate);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("iMaxRate == expected, iMaxRate=%d, expected=%d"), iFormatsSupported->iMaxRate, expected);
			}
		}
	else
		{
		INFO_PRINTF2(_L("iMaxRate=%d"), iFormatsSupported->iMaxRate);
		}
	}

/**
 * Gets iMinBufferSize
 * 
 * @param aSection 			section name where expected value can be read
 * 							KMinBufferSizeExpected - min buffer size expected
 *
 * @return					void
 *
 * @leave					no
 */
void CT_SoundFormatsSupportedData::DoCmd_iMinBufferSize_get(const TDesC& aSection)
	{
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KMinBufferSizeExpected(), aSection, expected))
		{
		if(iFormatsSupported->iMinBufferSize != expected)
			{
			ERR_PRINTF3(_L("Expected result != iMinBufferSize, expected=%d, iMinBufferSize=%d"), expected, iFormatsSupported->iMinBufferSize);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("iMinBufferSize == expected, iMinBufferSize=%d, expected=%d"), iFormatsSupported->iMinBufferSize, expected);
			}
		}
	else
		{
		INFO_PRINTF2(_L("iMinBufferSize=%d"), iFormatsSupported->iMinBufferSize);
		}
	}

/**
 * Gets iMaxBufferSize
 * 
 * @param aSection 			section name where expected value can be read
 * 							KMaxBufferSizeExpected - max buffer size expected
 *
 * @return					void
 *
 * @leave					no
 */
void CT_SoundFormatsSupportedData::DoCmd_iMaxBufferSize_get(const TDesC& aSection)
	{
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KMaxBufferSizeExpected(), aSection, expected))
		{
		if(iFormatsSupported->iMaxBufferSize != expected)
			{
			ERR_PRINTF3(_L("Expected result != iMaxBufferSize, expected=%d, iMaxBufferSize=%d"), expected, iFormatsSupported->iMaxBufferSize);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("iMaxBufferSize == expected, iMaxBufferSize=%d, expected=%d"), iFormatsSupported->iMaxBufferSize, expected);
			}
		}
	else
		{
		INFO_PRINTF2(_L("iMaxBufferSize=%d"), iFormatsSupported->iMaxBufferSize);
		}
	}

/**
 * Gets iMinVolume
 * 
 * @param aSection 			section name where expected value can be read
 * 							KMinVolumeExpected - min volume expected
 *
 * @return					void
 *
 * @leave					no
 */
void CT_SoundFormatsSupportedData::DoCmd_iMinVolume_get(const TDesC& aSection)
	{
	TInt expected;
	if (GET_OPTIONAL_INT_PARAMETER(KMinVolumeExpected(), aSection, expected))
		{
		if(iFormatsSupported->iMinVolume != expected)
			{
			ERR_PRINTF3(_L("Expected result != iMinVolume, expected=%d, iMinVolume=%d"), expected, iFormatsSupported->iMinVolume);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("iMinVolume == expected, iMinVolume=%d, expected=%d"), iFormatsSupported->iMinVolume, expected);
			}
		}
	else
		{
		INFO_PRINTF2(_L("iMinVolume=%d"), iFormatsSupported->iMinVolume);
		}
	}

/**
 * Gets iMaxVolume
 * 
 * @param aSection 			section name where expected value can be read
 * 							KMaxVolumeExpected - max volume expected
 *
 * @return					void
 *
 * @leave					no
 */
void CT_SoundFormatsSupportedData::DoCmd_iMaxVolume_get(const TDesC& aSection)
	{
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KMaxVolumeExpected(), aSection, expected))
		{
		if(iFormatsSupported->iMaxVolume != expected)
			{
			ERR_PRINTF3(_L("Expected result != iMaxVolume, expected=%d, iMaxVolume=%d"), expected, iFormatsSupported->iMaxVolume);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("iMaxVolume == expected, iMaxVolume=%d, expected=%d"), iFormatsSupported->iMaxVolume, expected);
			}
		}
	else
		{
		INFO_PRINTF2(_L("iMaxVolume=%d"), iFormatsSupported->iMaxVolume);
		}
	}

/**
 * Gets iChannels
 * 
 * @param aSection 			section name where expected value can be read
 * 							KChannelsExpected - channels expected
 *
 * @return					void
 *
 * @leave					no
 */
void CT_SoundFormatsSupportedData::DoCmd_iChannels_get(const TDesC& aSection)
	{
	TInt	expected;	
	if (GET_OPTIONAL_INT_PARAMETER(KChannelsExpected(), aSection, expected))
		{
		if(iFormatsSupported->iChannels != expected)
			{
			ERR_PRINTF3(_L("Expected result != iChannels, expected=%d, iChannels=%d"), expected, iFormatsSupported->iChannels);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("iChannels == expected, iChannels=%d, expected=%d"), iFormatsSupported->iChannels, expected);
			}
		}
	else
		{
		INFO_PRINTF2(_L("iChannels=%d"), iFormatsSupported->iChannels);
		}
	}

/**
 * Gets iEncodings
 * 
 * @param aSection 			section name where expected value can be read
 * 							KEncodingsExpected - encodings expected
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_SoundFormatsSupportedData::DoCmd_iEncodings_getL(const TDesC& aSection)
	{
	TUint expEncoding;
	if(GetOrFromConfig(aSection, KEncodingsExpected(), TSoundUtil::iEnumTableSoundEncoding, expEncoding ))
		{
		TUint encodings = iFormatsSupported->iEncodings;
		if(encodings != expEncoding)
			{
			ERR_PRINTF3(_L("iEncodings != expected result! iEncodings=0x%x, expected result=0x%x"), encodings, expEncoding);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("iEncodings == expected result!, iEncodings=0x%x, expected=0x%x"), encodings, expEncoding);
			}
		}
	else
		{
		INFO_PRINTF2(_L("iEncodings=%d"), iFormatsSupported->iEncodings);
		}
	}
