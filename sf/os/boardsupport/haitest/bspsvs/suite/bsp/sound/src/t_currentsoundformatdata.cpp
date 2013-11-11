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

This contains CT_CurrentSoundFormatData
*/

//	User includes
#include "t_currentsoundformatdata.h"
#include "sound_util.h"

/*@{*/
///	Commands
_LIT(KCmdNew,						"new");
_LIT(KCmdDestructor,				"~");
_LIT(KCmd_iRate_get,				"irate_get");
_LIT(KCmd_iRate_set,				"irate_set");
_LIT(KCmd_iEncoding_get,			"iencoding_get");
_LIT(KCmd_iEncoding_set,			"iencoding_set");
_LIT(KCmd_iChannels_get,			"ichannels_get");
_LIT(KCmd_iChannels_set,			"ichannels_set");
_LIT(KCmd_iBufferSize_get,			"ibuffer_size_get");
_LIT(KCmd_iBufferSize_set,			"ibuffer_size_set");

/*@{*/
///	Parameters

_LIT(KRate,							"rate");
_LIT(KRateExpected,					"rate_expected");
_LIT(KEncoding,						"encoding");
_LIT(KEncodingExpected,				"encoding_expected");
_LIT(KChannels,						"channels");
_LIT(KChannelsExpected,				"channels_expected");
_LIT(KBufferSize,					"buffer_size");
_LIT(KBufferSizeExpected,			"buffer_size_expected");
// possible parameter for new to indicate loading settings from 
// SoundFormatsSupported object
_LIT(KNewFromSoundFormatsSupported,	"new_from_sound_formats_supported");
// for SoundFormatsSupported object
_LIT(KSoundFormatsSupportedObject,	"SoundFormatsSupportedObject");
// Enums declared in INI file

///	Logging
_LIT(KLogNew,						"New object created successfully");
_LIT(KLogNewError,					"Error in creating object error code=%d");
/*@}*/

/**
 * Two phase constructor
 */
CT_CurrentSoundFormatData* CT_CurrentSoundFormatData::NewL()
	{
	CT_CurrentSoundFormatData*	ret=new (ELeave) CT_CurrentSoundFormatData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
* Constructor. First phase construction
*/
CT_CurrentSoundFormatData::CT_CurrentSoundFormatData()
:	CDataWrapperBase()
,	iCurrentFormat(NULL)
	{
	}

/**
 * Second phase construction
 */
void CT_CurrentSoundFormatData::ConstructL()
	{	
	}

/**
* Destructor
*/
CT_CurrentSoundFormatData::~CT_CurrentSoundFormatData()
	{
	DoCleanup();
	}

/**
 * Helper method for doing cleanup operation
 */
void CT_CurrentSoundFormatData::DoCleanup()
	{
	delete iCurrentFormat;
	iCurrentFormat=NULL;	
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return pointer to the object that the data wraps
 */
TAny* CT_CurrentSoundFormatData::GetObject()

	{
	return iCurrentFormat;
	}

/**
 * Set the wrapped data object with new value
 */
void CT_CurrentSoundFormatData::SetObjectL(TAny* aAny)

	{
	DoCleanup();
	iCurrentFormat=static_cast<RMdaDevSound::TCurrentSoundFormat*>(aAny);
	}

/**
 * Clear the wrapped data object pointer w/o de-initialization
 */
void CT_CurrentSoundFormatData::DisownObjectL()

	{
	iCurrentFormat=NULL;
	}

/**
 * Return static cleanup function
 */
inline TCleanupOperation CT_CurrentSoundFormatData::CleanupOperation()

	{
	return CleanupOperation;
	}

/**
 * Static cleanup function
 */
void CT_CurrentSoundFormatData::CleanupOperation( TAny* aAny )
	{
	RMdaDevSound::TCurrentSoundFormat* format = static_cast<RMdaDevSound::TCurrentSoundFormat*> ( aAny );
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
TBool CT_CurrentSoundFormatData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;
	if ( aCommand==KCmdNew )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmd_iRate_get )
		{
		DoCmd_iRate_get(aSection);
		}
	else if ( aCommand==KCmd_iEncoding_get )
		{
		DoCmd_iEncoding_getL(aSection);
		}
	else if ( aCommand==KCmd_iChannels_get )
		{
		DoCmd_iChannels_get(aSection);
		}
	else if ( aCommand==KCmd_iBufferSize_get )
		{
		DoCmd_iBufferSize_get(aSection);
		}
	else if ( aCommand==KCmd_iRate_set )
		{
		DoCmd_iRate_set(aSection);
		}
	else if ( aCommand==KCmd_iEncoding_set )
		{
		DoCmd_iEncoding_set(aSection);
		}
	else if ( aCommand==KCmd_iChannels_set )
		{
		DoCmd_iChannels_set(aSection);
		}
	else if ( aCommand==KCmd_iBufferSize_set )
		{
		DoCmd_iBufferSize_set(aSection);
		}
	else
		{
		ret=CDataWrapperBase::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}

/**
 * Function for creating new instance of RMdaDevSound::TCurrentSoundFormat
 * 
 * @param aSection 			section name where can be indicated to create new
 * 							from SoundFormatsSupported object values
 * 							KNewFromSoundFormatsSupported - create from 
 * 							SoundFormatsSupported object
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_CurrentSoundFormatData::DoCmdNewL(const TDesC& aSection)
	{
	DoCleanup();
	
	TRAPD(err, iCurrentFormat = new (ELeave) RMdaDevSound::TCurrentSoundFormat());	
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogNewError, err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(KLogNew);
		TBool new_object;
		if (GET_OPTIONAL_BOOL_PARAMETER(KNewFromSoundFormatsSupported(), aSection, new_object))
			{
			DoInitiateSupportedFormatsL(aSection);
			}
		}
	}

/**
 * Function to perform cleanup opertation
 */
void CT_CurrentSoundFormatData::DoCmdDestructor()
	{
	DoCleanup();
	}

/**
 * Get function for iRate
 * 
 * @param aSection 			section name where expected value can be read
 *							KRateExpected - expected rate
 * 		
 * @return					void
 *
 * @leave					no
 */
void CT_CurrentSoundFormatData::DoCmd_iRate_get(const TDesC& aSection)
	{	
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KRateExpected(), aSection, expected))
		{
		if(iCurrentFormat->iRate != expected)
			{
			ERR_PRINTF3(_L("Expected result != iRate, expected=%d, iRate=%d"), expected, iCurrentFormat->iRate);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("iRate == expected result!"));
			}
		}
	else
		{
		INFO_PRINTF2(_L("iRate=%d"), iCurrentFormat->iRate);
		}
	}
	

/**
 * Set function for iRate
 * 
 * @param aSection 			section name where value to set can be read
 * 							KRate - rate to set
 *
 * @return					void
 *
 * @leave					no
 */
void CT_CurrentSoundFormatData::DoCmd_iRate_set(const TDesC& aSection)
	{	
	GET_MANDATORY_INT_PARAMETER(KRate(), aSection, iCurrentFormat->iRate);
	}

/**
 * Get function for iEncoding
 * 
 * @param aSection 			section name where expected value can be read
 * 							KEncodingExpected - expected encoding
 *
 * @return					void
 *
 * @leave					no
 */
void CT_CurrentSoundFormatData::DoCmd_iEncoding_getL(const TDesC& aSection)
	{	
	TUint expEncoding;
	if(GetOrFromConfig(aSection, KEncodingExpected(), TSoundUtil::iEnumTableSoundEncoding, expEncoding ))
		{
		TUint encodings = iCurrentFormat->iEncoding;
		if(expEncoding != encodings)
			{
			ERR_PRINTF3(_L("iEncoding != expected result! iEncodings=0x%x, expected result=0x%x"), encodings, expEncoding);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("iEncoding == expected result!, iEncodings=0x%x, expected=0x%x"), encodings, expEncoding);
			}			
		}
	else
		{
		INFO_PRINTF2(_L("iEncoding=0x%x"), iCurrentFormat->iEncoding);
		}
	}

/**
 * Set function for iEncoding
 * 
 * @param aSection 			section name where value to set can be read
 * 							KEncoding - encoding to set
 *
 * @return					void
 *
 * @leave					no
 */
void CT_CurrentSoundFormatData::DoCmd_iEncoding_set(const TDesC& aSection)
	{	
	TUint encoding;
	if(GetOrFromConfig(aSection, KEncoding(), TSoundUtil::iEnumTableSoundEncoding, encoding ))
		{
		iCurrentFormat->iEncoding = (RMdaDevSound::TMdaSoundEncoding)encoding;
		INFO_PRINTF2(_L("iEncoding=0x%x"), iCurrentFormat->iEncoding);
		}
	else
		{
		ERR_PRINTF1(_L("Could not get encoding from TMdaSoundEncoding in INI file"));
		SetBlockResult(EFail);
		}
	}

/**
 * Get function for iChannels
 * 
 * @param aSection 			section name where expected value can be read
 * 							KChannelsExpected - channels expected
 *
 * @return					void
 *
 * @leave					no
 */
void CT_CurrentSoundFormatData::DoCmd_iChannels_get(const TDesC& aSection)
	{
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KChannelsExpected(), aSection, expected))
		{
		if(iCurrentFormat->iChannels != expected)
			{
			ERR_PRINTF3(_L("Expected result != iChannels, expected=%d, iChannels=%d"), expected, iCurrentFormat->iChannels);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("iChannels == expected result!"));
			}
		}
	else
		{
		INFO_PRINTF2(_L("iChannels=%d"), iCurrentFormat->iChannels);
		}
	}

/**
 * Set function for iChannels
 * 
 * @param aSection 			section name where value to set can be read
 * 							KChannels - channels to set
 *
 * @return					void
 *
 * @leave					no
 */
void CT_CurrentSoundFormatData::DoCmd_iChannels_set(const TDesC& aSection)
	{
	GET_MANDATORY_INT_PARAMETER(KChannels(), aSection, iCurrentFormat->iChannels);
	}

/**
 * Get function for iBufferSize
 * 
 * @param aSection 			section name where expected value can be read
 * 							KBufferSizeExpected - buffer size expected
 *
 * @return					void
 *
 * @leave					no
 */
void CT_CurrentSoundFormatData::DoCmd_iBufferSize_get(const TDesC& aSection)
	{
	TInt	expected;
	if (GET_OPTIONAL_INT_PARAMETER(KBufferSizeExpected(), aSection, expected))
		{
		if(iCurrentFormat->iBufferSize != expected)
			{
			ERR_PRINTF3(_L("Expected result != iBufferSize, expected=%d, iBufferSize=%d"), expected, iCurrentFormat->iBufferSize);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("iBufferSize == expected result!"));
			}
		}
	else
		{
		INFO_PRINTF2(_L("iBufferSize=%d"), iCurrentFormat->iBufferSize);
		}
	}

/**
 * Set function for iBufferSize
 * 
 * @param aSection 			section name where value to set can be read
 * 							KBufferSize - buffer size to set
 *
 * @return					void
 *
 * @leave					no
 */
void CT_CurrentSoundFormatData::DoCmd_iBufferSize_set(const TDesC& aSection)
	{
	GET_MANDATORY_INT_PARAMETER(KBufferSize(), aSection, iCurrentFormat->iBufferSize);
	}

/**
 * Function to set current sound format with first valid values from 
 * TSoundFormatsSupported object. Function will set iBufferSize, iChannels,
 * iRate and iEncoding variable
 * 
 * @param aSection 			section name which contains SoundFormatsSupported object
 *							KSoundFormatsSupportedObject - SoundFormatsSupported object
 * 							name 
 * 
 * @return					void
 *
 * @leave					System wide error
 */
void CT_CurrentSoundFormatData::DoInitiateSupportedFormatsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Calling DoInitiateSupportedFormats(), Setting iCurrentFormat variables from TSoundFormatsSupported"));
	TPtrC		objectName;	
	if (GET_MANDATORY_STRING_PARAMETER(KSoundFormatsSupportedObject, aSection, objectName))
		{		
		RMdaDevSound::TSoundFormatsSupported* soundFormat =
			static_cast<RMdaDevSound::TSoundFormatsSupported*>(GetDataObjectL(objectName));	
		iCurrentFormat->iBufferSize = soundFormat->iMinBufferSize;
		iCurrentFormat->iChannels = soundFormat->iChannels;
		iCurrentFormat->iRate = soundFormat->iMinRate;
		if (soundFormat->iEncodings & RMdaDevSound::EMdaSoundEncoding8BitPCM) 
			{
			iCurrentFormat->iEncoding = RMdaDevSound::EMdaSoundEncoding8BitPCM;
			}
		else if(soundFormat->iEncodings & RMdaDevSound::EMdaSoundEncoding16BitPCM)
			{
			iCurrentFormat->iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
			}
		else if(soundFormat->iEncodings & RMdaDevSound::EMdaSoundEncoding8BitALaw)
			{
			iCurrentFormat->iEncoding = RMdaDevSound::EMdaSoundEncoding8BitALaw;
			}
		else if(soundFormat->iEncodings & RMdaDevSound::EMdaSoundEncoding8BitMuLaw)
			{
			iCurrentFormat->iEncoding = RMdaDevSound::EMdaSoundEncoding8BitMuLaw;
			}
		}	
	}
