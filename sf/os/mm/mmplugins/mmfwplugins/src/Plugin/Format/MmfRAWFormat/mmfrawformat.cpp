// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// include\server\mmfrawformat.cpp
// 
//


#include <ecom/ecom.h>
#include <mmf/plugin/mmfformatimplementationuids.hrh>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfaudioinput.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>

#include "mmfrawformat.h"

#include "formatUtils.h"
#include "MMFFormatBaseClasses.h"



/*
 TMmfWavFormatPanics is an enumeration with the following entries:
 EPreconditionViolation indicates a precondition violation
 EPostConditionViolation indicates a post condition violation
*/
enum TMmfRawFormatPanics
	{
	EPostConditionViolation,
	EPreconditionViolation
	};

/**
* Panic
* This method generates a panic
* @internalTechnology
* @param "TInt"
*/
void Panic(TInt aPanicCode)
	{
	_LIT(KMMFRawFormatPanicCategory, "MMFRawFormat");
	User::Panic(KMMFRawFormatPanicCategory, aPanicCode);
	}

/**
*
* NewL
* @param  aSource
* @return CMMFFormatDecode* 
*
*/
CMMFFormatDecode* CMMFRawFormatRead::NewL(MDataSource* aSource)
	{
	if ((aSource->DataSourceType()==KUidMmfDescriptorSource)||
			(aSource->DataSourceType()==KUidMmfFileSource))
		{//currently only files and descriptor MDataSources are supported
		CMMFRawFormatRead* self = new(ELeave)CMMFRawFormatRead;
		CleanupStack::PushL(self);
		self->ConstructL(aSource);
		CleanupStack::Pop();
		return STATIC_CAST( CMMFFormatDecode*, self);
		}
	else
		{
		User::Leave(KErrNotSupported);
		return NULL;
		}
	}

/**
*
* ~CMMFRawFormatRead
*
*/
CMMFRawFormatRead::~CMMFRawFormatRead()
	{
	}


/**
*
* ProcessFormatHeaderL
*
* Method to scan the header of a au file and sets the audio setting data members
* iSampleRate, iChannels, iClipSize
*
* @leave "This method may leave with one of the system wide error codes"
*/
void CMMFRawFormatRead::ProcessFormatHeaderL()
	{
	iClipLength = STATIC_CAST(CMMFClip*,iClip)->Size();
	CalculateFrameSize();
	iStartPosition=0;

	iDataLength = iClipLength;
	}





/**
*
* SourceDataTypeCode
* @param aMediaId
* @return TFourCC
*
*/
TFourCC CMMFRawFormatRead::SourceDataTypeCode(TMediaId aMediaId)
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio)
		return iFourCC;
	else
		return TFourCC(); //defaults to 'NULL' fourCC
	}

/**
* 
* SetSourceDataTypeCode
* @param aSinkFourCC
* @param aMediaId
* @return TInt 
*
*/
TInt CMMFRawFormatRead::SetSourceDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		return KErrNotSupported;
	else
		iFourCC = aSinkFourCC;
	
	if ((iFourCC == KMMFFourCCCodePCM16) || 
		(iFourCC == KMMFFourCCCodePCM16B) || 
		(iFourCC == KMMFFourCCCodePCMU16)) 
		iBitsPerSample = 16;
	else if ((iFourCC == KMMFFourCCCodeIMAD) || 
		(iFourCC == KMMFFourCCCodeIMAS)) 
		iBitsPerSample = 4;
	else 
		iBitsPerSample = 8; //default to 8
	return KErrNone;
	}


/**
*
* SetNumChannels
* @param aChannels
* @return TInt
*
*/
TInt CMMFRawFormatRead::SetNumChannels(TUint aChannels)
	{
	TInt error = KErrNone;
	if ((aChannels ==  KMono)||(aChannels == KStereo))
		iChannels = aChannels;
	else
		error = KErrNotSupported; //only alow one or two channels
	return error;
	}



/**
*
* SetSampleRate
* @param aSampleRate
* @return TInt
*
*/
TInt CMMFRawFormatRead::SetSampleRate(TUint aSampleRate)
	{
	TInt status = KErrNotSupported;
	//we'll iterate through the valid sample table
	TInt i = sizeof(KMMFFormatSampleRates) / sizeof(TUint);
	
	while ((i--) && (status != KErrNone))
		{
		if (aSampleRate == KMMFFormatSampleRates[i])
			{
			iSampleRate = aSampleRate;
			status = KErrNone;
			}
		}
	return status;
	}


/**
*
* GetSupportedDataTypesL
* @param aMediaId
* @param aDataTypes
* 
*/
void CMMFRawFormatRead::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		User::Leave(KErrNotSupported);
	aDataTypes.Reset();
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16B));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCMU16));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAD));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAS));
	}

/***********************************************************************************************/

/**
* 
* NewL
* @param  aSink
* @returns CMMFFormatEncode*
* 
*/
CMMFFormatEncode* CMMFRawFormatWrite::NewL(MDataSink* aSink)
	{
	if ((aSink->DataSinkType()==KUidMmfDescriptorSink)||
		(aSink->DataSinkType()==KUidMmfFileSink))
		{//currently only files and descriptor MDataSources are supported
		CMMFRawFormatWrite* self = new(ELeave)CMMFRawFormatWrite;
		CleanupStack::PushL(self);
		self->ConstructL(aSink);
		CleanupStack::Pop();
		return STATIC_CAST(CMMFFormatEncode*, self);
		}
	else
		{
		return NULL;
		}
	}

/**
*
* ~CMMFRawFormatWrite
* 
*/
CMMFRawFormatWrite::~CMMFRawFormatWrite()
	{
	}



/**
*
* NegotiateL
* @param aSource
* 
*/
void CMMFRawFormatWrite::NegotiateL(MDataSource& aSource)
	{
	if (aSource.DataSourceType() == KUidMmfAudioInput)
		{
		CMMFDevSound& soundDevice = STATIC_CAST(MMMFAudioInput&, aSource).SoundDevice();
		GetAudioSettingsFromDevice(&soundDevice, iSourceSampleRate, iSourceChannels, iSourceFourCC);
		}
	else if (aSource.DataSourceType() == KUidMmfFormatDecode)
		{//source is a clip so for now set sink settings to match source
		iSourceSampleRate = ((CMMFFormatDecode&)aSource).SampleRate();
		iSourceChannels = ((CMMFFormatDecode&)aSource).NumChannels();
		iSourceFourCC.Set(aSource.SourceDataTypeCode(TMediaId(KUidMediaTypeAudio)));
		
		iSourceWillSampleConvert = STATIC_CAST(CMMFFormatDecode&, aSource).SourceSampleConvert();

		//make the start position the end of the clip
		}
	else
		{
		return;
		}
	//set default sink parameters to be the same as the source
	if (!IsValidSampleRate( iSampleRate))
		{
		if(IsValidSampleRate( iSourceSampleRate))
			{
			iSampleRate = iSourceSampleRate; //might have already been set by custom command
			}
		else
			{
			iSampleRate =  KMMFFormatDefaultSampleRate;
			}
		}

	if (!iChannels)
		{
		//[ IMA ADPCM only support mono ]
		if(iFourCC == KMMFFourCCCodeIMAD)
			iChannels = EMMFMono;
		else
			//[ we will accept the number of source channels as being no problem]
			iChannels = iSourceChannels;
		}

	//[ IMA ADPCM only support mono ]
	if((iFourCC == KMMFFourCCCodeIMAD) && (iChannels != EMMFMono))
		User::Leave(KErrNotSupported);

	if (!iBitsPerSample)
		{
		TInt errorCode =SetSinkDataTypeCode( iSourceFourCC, KUidMediaTypeAudio);
		if( errorCode != KErrNone)
			{
			// [ acceptable default for raw ]
			User::LeaveIfError( SetSinkDataTypeCode( KMMFFourCCCodePCM16, KUidMediaTypeAudio) );
			}
		}

	//now re-evaluate the frame size & interval
	CalculateFrameSize();
	
	// if we're converting suggest a frame size to the source format
	if (aSource.DataSourceType() == KUidMmfFormatDecode)
		((CMMFFormatDecode&)aSource).SuggestSourceBufferSize(iFrameSize); //for now suggest format src takes same buf size as sink??
	}


/**
*
* DetermineIfValidClip
* @param aClip
*
*/
TInt CMMFRawFormatWrite::DetermineIfValidClip()
	{
	TRAPD(err,DoReadL(0)); //read from the beginning of the clip
	if (err) 
		return err;
	if (iBuffer->Data().Size() == 0)
		{
		err = KErrNotFound; //no file present
		}

	return err;
	}

/**
*
* ProcessFormatHeaderL
*
* Method to scan the header of a RAW file and sets the audio setting data members
* iSampleRate, iChannels, iClipSize
*
* @leave "This method may leave with one of the system wide error codes"
*/
void CMMFRawFormatWrite::ProcessFormatHeaderL()
	{
	iClipLength = STATIC_CAST(CMMFClip*,iClip)->Size();
	CalculateFrameSize();
	iStartPosition=0;

	iDataLength = iClipLength;
	iHeaderLength = 0;
	}


/**
*
* SetDefaultHeaderParametersL
*
*/
void CMMFRawFormatWrite::SetDefaultHeaderParametersL()
	{
	//If this is a new file, set initial data parameters, else retain the existing settings.
	//This situation can occur if the file has been deleted 
	if(!iClipAlreadyExists)
		{
		//Need an initial frame size, so assume certain settings
		iChannels      = KMMFFormatDefaultChannels;		       // can't have stereo
		iSampleRate    = KMMFFormatDefaultSampleRate;  // assume 8KHz for now
		iBitsPerSample = KMMFFormatDefaultBitsPerSample; // default bits per sample
		CalculateFrameSize();

		//these values must be set via Negotiation or explicitly set via a custom command
		iChannels      = 0;
	    iSampleRate    = 0;
		iBitsPerSample  =0;	

		iFourCC.Set(KMMFFourCCCodePCM16B); //16 bit PCM
		}

	iDataLength = 0;
	iClipLength = 0;
	iStartPosition = iClipLength;

	iState = EInitialised;
	}


/**
*
* SetSinkDataTypeCode
* @param aSinkFourCC
* @param aMediaId
* @return TInt
*
*/
TInt CMMFRawFormatWrite::SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		return KErrNotSupported;
	else
		iFourCC = aSinkFourCC;

	if ((iFourCC == KMMFFourCCCodePCM16) || 
		(iFourCC == KMMFFourCCCodePCM16B) ||
		(iFourCC == KMMFFourCCCodePCMU16)) 
		iBitsPerSample = 16;
	else if ((iFourCC == KMMFFourCCCodeIMAD) ||
		(iFourCC == KMMFFourCCCodeIMAS)) 
		iBitsPerSample = 4;
	else 
		iBitsPerSample = 8; //default to 8
	
	return KErrNone;
	}

/**
*
* SetNumChannels
* @param aChannels
* @return TInt
*
*/
TInt CMMFRawFormatWrite::SetNumChannels(TUint aChannels)
	{
	TInt error = KErrNone;
	if ((aChannels ==  KMono)||(aChannels == KStereo))
		iChannels = aChannels;
	else
		error = KErrNotSupported; //only alow one or two channels
	return error;
	}

/**
*
* SetSampleRate
* @param aSampleRate
* @return TInt
*
*/
TInt CMMFRawFormatWrite::SetSampleRate(TUint aSampleRate)
	{
	TInt status = KErrNotSupported;
	//we'll iterate through the valid sample table
	TInt i = sizeof(KMMFFormatSampleRates) / sizeof(TUint);
	
	while ((i--) && (status != KErrNone))
		{
		if (aSampleRate == KMMFFormatSampleRates[i])
			{
			iSampleRate = aSampleRate;
			status = KErrNone;
			}
		}
	return status;
	}





/**
*
* GetSupportedDataTypesL
* @param aMediaId
* @param aDataTypes
* 
*/
void CMMFRawFormatWrite::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		User::Leave(KErrNotSupported);
	aDataTypes.Reset();
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16B));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCMU16));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAD));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAS));
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidFormatRAWRead,	CMMFRawFormatRead::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidFormatRAWWrite,	CMMFRawFormatWrite::NewL)
	};

/**
*
* ImplementationGroupProxy
* @param aTableCount
* @returns TImplementationProxy*
*
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}


